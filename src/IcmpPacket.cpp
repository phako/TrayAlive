/*
   IcmpPacket.cpp - Class wrapper for ICMP packets, implementation.
   
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

#include <memory.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <time.h>
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#include "IcmpPacket.h"

const uint32_t IcmpPacket::m_dataBytes = 0xCAFEBABE;

// define a icmp header structure
IcmpPacket::IcmpPacket(const unsigned int id, const unsigned int sequence, const unsigned int size)
{
	m_size = size;
	m_buffer = new char[size];
	memset(m_buffer, 0, size);
	m_icmp_packet = (icmp_packet *)m_buffer;

	m_icmp_packet->header.type = ICMP_TYPE_ECHO_REQUEST;
	m_icmp_packet->header.code = 0;
	m_icmp_packet->header.checksum = 0;
	m_icmp_packet->header.id = htons(id);
	m_icmp_packet->header.seq = htons(sequence);
#ifdef _WIN32
	m_icmp_packet->header.timestamp = GetTickCount();
#else
	time_t t;
	m_icmp_packet->header.timestamp = time(&t);
#endif

	fillData();
	calculateChecksum();
}

IcmpPacket::IcmpPacket(const char* payload, const int size)
{
	m_size = size;
	m_buffer = new char[size];
	memcpy(m_buffer, payload, size);
	m_icmp_packet = (icmp_packet *)m_buffer;
}

IcmpPacket::~IcmpPacket()
{
	delete [] m_buffer;
}

const char* IcmpPacket::rawData() const
{
	return m_buffer;
}

void IcmpPacket::fillData()
{
	char *pos = 0;
	int bytesLeft = m_size - sizeof(icmp_header);

	pos = &(m_icmp_packet->data);

	while (bytesLeft > 0)
	{
		memcpy (pos, (const char *)&m_dataBytes, 
				min(bytesLeft,(int)sizeof(m_dataBytes)));
		pos += sizeof(m_dataBytes);
		bytesLeft -= sizeof(m_dataBytes);
	}
}

void IcmpPacket::calculateChecksum()
{
	uint16_t* buf = 0;
	int count = 0;
	uint32_t checksum = 0L;

	buf = (uint16_t*) m_buffer;
	count = m_size;
	while (count > 1)
	{
		checksum += *buf;
		buf++;
		count -= 2;
	}

	if (count > 0)
	{
		checksum += *buf;
	}

	checksum = (checksum & 0xffff) + (checksum >> 16);

	m_icmp_packet->header.checksum = (uint16_t)~checksum;
}
