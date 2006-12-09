/*
   Ping.cpp - Implementation of the pinger component
   
   Copyright 2006 Jens Georg
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <winsock2.h>
#ifndef IP_TTL
#	include <ws2tcpip.h>
#endif
#include <windows.h>
#include <process.h>
#include <iphlpapi.h>
#include <errno.h>

#include <string>
#include <iostream>

#include "PingException.h"
#include "Ping.h"

const unsigned long int cg_ulDataBytes = 0xCAFEBABE;
const BYTE cg_ICMP_TYPE_ECHO_REQUEST = 8;
const BYTE cg_ulICMP_ECHO_REPLY = 0;

#pragma pack(1)

struct icmp_header 
{
	BYTE type;
	BYTE code;
	USHORT checksum;
	USHORT id;
	USHORT seq;
	ULONG timestamp;
};

struct ip_header
{
	BYTE h_len:4;
	BYTE version:4;
	BYTE tos;
	USHORT total_length;
	USHORT ident;
	USHORT flags;
	BYTE ttl;
	BYTE proto;
	USHORT checksum;
	ULONG source_ip;
	ULONG dest_ip;
};

CPing::CPing(
		const HWND hMessageWindow,
		const char* c_pszHost,
		const bool c_bResolveAddress,
		const unsigned int c_uTimeout,
		const unsigned int c_uSleep,
		const unsigned int c_uUnreachableThreshold,
		const unsigned int c_lBufferSize, 
		const unsigned int c_uTtl) :
m_hMessageWindow(hMessageWindow),
m_pszHost(NULL),
m_uBufferSize(c_lBufferSize),
m_uTtl(c_uTtl),
m_uUnreachableThreshold(c_uUnreachableThreshold),
m_ulInetAddr(0L),
m_bDownSignalled(false),
m_uFailedPings(0),
m_uTimeout(c_uTimeout),
m_uSleep(c_uSleep),
m_sd(INVALID_SOCKET),
m_uSequenceNumber(0)
{
	struct hostent* pHostInfo = NULL;
	in_addr in = {0};
	int nResult = 0;

	m_uBufferSize -= sizeof(icmp_header);

	// try to convert the host address into a inet address
	m_ulInetAddr = inet_addr(c_pszHost);
	if (INADDR_NONE == m_ulInetAddr)
	{
		// we assume this is a hostname, not an ip
		pHostInfo = gethostbyname(c_pszHost);
		if (NULL == pHostInfo)
		{
			throw CPingException("Could not resolve hostname");
		}
		else
		{
			m_pszHost = new char[strlen(c_pszHost) + 1];
			memset(m_pszHost, 0, strlen(c_pszHost) + 1);
			memcpy(m_pszHost, c_pszHost, strlen(c_pszHost)); 

			memcpy(&(m_saDest.sin_addr), pHostInfo->h_addr, pHostInfo->h_length);
			m_saDest.sin_family = pHostInfo->h_addrtype;
		}	
	}
	else
	{
		// fill inet address for destination
		m_saDest.sin_addr.s_addr = m_ulInetAddr;
		m_saDest.sin_family = AF_INET;
		if (c_bResolveAddress)
		{
			pHostInfo = gethostbyaddr((char *)&m_ulInetAddr, 4, AF_INET);
			if (NULL != pHostInfo)
			{
				m_pszHost = new char[strlen(pHostInfo->h_name) + 1];
				memset(m_pszHost, 0, strlen(pHostInfo->h_name) + 1);
				memcpy(m_pszHost, pHostInfo->h_name, strlen(pHostInfo->h_name));
			}
			else
			{
				m_pszHost = new char[strlen(c_pszHost) + 1];
				memset(m_pszHost, 0, strlen(c_pszHost) + 1);
				memcpy(m_pszHost, c_pszHost, strlen(c_pszHost)); 
			}
		}
		else
		{
			m_pszHost = new char[strlen(c_pszHost) + 1];
			memset(m_pszHost, 0, strlen(c_pszHost) + 1);
			memcpy(m_pszHost, c_pszHost, strlen(c_pszHost)); 
		}
	}

	createSocket();

	m_hEvent = CreateEvent(NULL, TRUE, FALSE, "PingCancel event");

	// create random buffer
	m_pBuffer = new char[m_uBufferSize + sizeof(icmp_header)];
	memset(m_pBuffer, 0, m_uBufferSize + sizeof(icmp_header));

	long lBuflen = sizeof(ip_header) + 1024;
	m_pReplyBuffer = (char *) malloc(lBuflen);
	memset(m_pReplyBuffer, 0, lBuflen);
}

void CPing::start()
{
	// todo: check for successful thread creation
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, CPing::threadFunc, this, 0, &m_uThreadId);
	if (0 == m_hThread)
	{
		// failed to create thread
		// check what exactly failed
		switch (errno)
		{
		case EAGAIN:
			// too many threads
			OutputDebugString("Could not spawn ping thread. To many open threads");
			// throw exception
			break;
		case EINVAL:
			OutputDebugString("Could not spawn ping thread. Invalid argument or stack size");
			break;
		default:
			break;
		}
	}
}

void CPing::stop()
{
	// todo: mutex for m_hEvent
	SetEvent(m_hEvent);
}

unsigned int __stdcall CPing::threadFunc(void *pParam)
{
	CPing *pInstance = reinterpret_cast<CPing *>(pParam);

	if (isValidInstance(pInstance))
	{
		pInstance->pingThread();
	}

	return 0;
}

bool CPing::isValidInstance(const CPing* pInstance)
{
	return true;
}

CPing::~CPing()
{
	if (m_sd != INVALID_SOCKET)
	{
		closesocket(m_sd);
		m_sd = INVALID_SOCKET;
	}

	long lResult;

	// wait for ping thread to end
	lResult = WaitForSingleObject(m_hThread, INFINITE);
	switch (lResult)
	{
	case WAIT_OBJECT_0:
		// all fine
		break;
	case WAIT_FAILED:
		OutputDebugString("Waiting for ping thread to finish failed.");
		break;
	}


	CloseHandle(m_hEvent);

	delete [] m_pReplyBuffer;
	delete [] m_pBuffer;

	// free host name
	if (m_pszHost != NULL)
	{
		delete [] m_pszHost;
		m_pszHost = NULL;
	}
}

void CPing::createSocket()
{
	int nResult = 0;

	// create raw socket 
	m_sd = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0,0,0);
	if (INVALID_SOCKET == m_sd)
	{
		throw CPingException(std::string("Could not create raw socket. Are you Administrator?"));
	}

	// set TTL of socket
	nResult = setsockopt(m_sd, IPPROTO_IP, IP_TTL, (const char *)&m_uTtl, sizeof(m_uTtl));
	if (SOCKET_ERROR == nResult)
	{
		throw CPingException(std::string("Could not set TTL of socket."));
	}
}

void CPing::pingThread()
{
	long lResult;
	bool bLeave = false;

	// have a first checkup of the host
	if (!ping())
	{
		fireDown();
		m_bDownSignalled = true;
	}

	while (!bLeave)
	{
		lResult = WaitForSingleObject(m_hEvent, 1000);
		switch (lResult)
		{
		case WAIT_TIMEOUT:
			// Check if host is reachable
			if (ping())
			{
				OutputDebugString("Up\r\n");
				if (m_bDownSignalled)
				{
					fireUp();
					m_bDownSignalled = false;
				}
				m_uFailedPings = 0;
			}
			else
			{
				OutputDebugString("Down\r\n");
				m_uFailedPings++;
				if (m_uFailedPings > m_uUnreachableThreshold && !m_bDownSignalled)
				{
					fireDown();
					m_bDownSignalled = true;
				}
			}
			break;
		default:
			bLeave = true;
		}
	}
	ResetEvent(m_hEvent);
	m_uSequenceNumber = 0;
}

void CPing::prepareMessage()
{
	int nBytesLeft = m_uBufferSize;
	unsigned long checksum = 0L;
	int count;

	icmp_header* pHeader = (icmp_header *) m_pBuffer;

	pHeader->type = cg_ICMP_TYPE_ECHO_REQUEST;
	pHeader->code = 0;
	pHeader->checksum = 0;
	pHeader->id = (USHORT)GetCurrentProcessId();
	pHeader->seq = htons(++m_uSequenceNumber);
	pHeader->timestamp = GetTickCount();

	// fill up packet with a little java ;)
	char* pos = (char *)(pHeader);
	pos += sizeof(icmp_header);
	while (nBytesLeft > 0)
	{
		memcpy(pos, (const char*)&cg_ulDataBytes, std::min((int)sizeof(cg_ulDataBytes), nBytesLeft));
		nBytesLeft -= sizeof(cg_ulDataBytes);
		pos += sizeof(cg_ulDataBytes);
	}

	USHORT* buf = (USHORT *)pHeader;
	count = m_uBufferSize + sizeof(icmp_header);
	while (count > 1)
	{
		checksum += *buf;
		buf++;
		count -= sizeof(USHORT);
	}

	if (count > 0)
	{
		checksum += *buf;
	}

	checksum = (checksum & 0xffff) + (checksum >> 16);
	//checksum = (checksum >> 16);

	pHeader->checksum = ~checksum;
}

bool CPing::ping()
{
	int nResult = SOCKET_ERROR;
	TIMEVAL tvTimeout = {0};
	fd_set fds = {0};

	tvTimeout.tv_usec = m_uTimeout * 1000;
	FD_ZERO(&fds);
	FD_SET(m_sd, &fds);
	sockaddr_in sa;
	ip_header* pIpHeader;
	icmp_header* pIcmpHeader;

	// copy dest address, routers overwrite it on DEST_UNREACHABLE
	memcpy(&sa, &m_saDest, sizeof(m_saDest));

	prepareMessage();

	nResult = sendto(m_sd, 
		m_pBuffer, m_uBufferSize + sizeof(icmp_header), 0, 
		(const sockaddr *)&m_saDest, sizeof(m_saDest));

	if (SOCKET_ERROR == nResult)
	{
		nResult = WSAGetLastError();
		return false;
	}


	do 
	{
		nResult = select(1, &fds, NULL, NULL, &tvTimeout);
		OutputDebugString("After select\r\n");

		if (nResult != 1)
		{
			return false;
		}

		int size = sizeof(m_saDest);
		nResult = recvfrom(m_sd, 
			(char *)m_pReplyBuffer, 1024 + sizeof(ip_header), 0,
			(sockaddr *)&sa, &size);

		OutputDebugString("After recvfrom\r\n");
		if (SOCKET_ERROR == nResult)
		{
			nResult = WSAGetLastError();
			return false;
		}

		pIpHeader = (ip_header *) m_pReplyBuffer;
		pIcmpHeader = (icmp_header *) (m_pReplyBuffer + pIpHeader->h_len * 4);

		// check sequence number
	}
	while (ntohs(pIcmpHeader->seq) != m_uSequenceNumber);

	char *data = (char *)(pIcmpHeader + sizeof(icmp_header));

	// check length
	if (pIcmpHeader->type != cg_ulICMP_ECHO_REPLY)
	{
		OutputDebugString("Answer is not ECHO_REPLY");
		return false;
	}

	if (pIcmpHeader->id != GetCurrentProcessId())
	{
		OutputDebugString("Answer is not for us");
		return false;
	}
	return true;
}

void CPing::fireUp()
{
	::PostMessage(m_hMessageWindow, CPing::PING_HOST_UP, (LPARAM)this, 0);
}

void CPing::fireDown()
{
	::PostMessage(m_hMessageWindow, CPing::PING_HOST_DOWN, (LPARAM)this, 0);
}
