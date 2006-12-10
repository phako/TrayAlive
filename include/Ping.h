/*
   ping.h - header file for ping helper class
   
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

#ifndef _PING_H
#define _PING_H

#include <winsock2.h>
#include <windows.h>

class CPing  
{
public:
	/**
	 * Custom windows messages
	 */
	enum {
		PING_HOST_UP = WM_USER + 0x8000,
		PING_HOST_DOWN,
	};
	/**
	 * Ping class constructor. 
	 *
	 * The defaults are roughly taken from the windows command line utility ping.exe.
	 * 
	 * @param c_pszHost hostname or IP address to ping
	 * @param c_uSleep time between two ICMP packets
	 * @param c_uTimeout how long to wait for an answer
	 * @param c_uBufferSize ping packet size. Default is 32 bytes.
	 * @param c_uTtl time to live. Default is 60
	 * @param c_uHaveToFail how many pings in a row have to fail until a host is considered unreachable.
	 * @param c_bResolveAddress true, if you want ping to do a reverse lookup, false otherwise.
	 */
	CPing(
		const HWND hMessageWindow,
		const char* c_pszHost,
		const bool c_bResolveAddress = false,
		const unsigned int c_uTimeout = 4000,
		const unsigned int c_uSleep = 1000,
		const unsigned int c_uUnreachableThreshold = 4,
		const unsigned int c_uBufferSize = 32, 
		const unsigned int c_uTtl = 60);
	virtual ~CPing();

	/**
	 * Starts pinging the host. Notifications will be sent to the listener 
	 * using standard Windows window messages.
	 */
	void start();

	/**
	 * Stops pinging the host. 
	 */
	void stop();

	const char* getHostName() const { return m_pszHost; };

	/**
	 * Static member function used for _beginthreadex. The member will lookup if its parameter
	 * is a valid instance of CPing and will then call pInstance->pingThread().
	 */
	static unsigned int __stdcall threadFunc(void *pInstance);

private:
	/**
	 * cheks if pInstance is a valid instance of CPing.
	 *
	 * @param pInstance pointer to an instance of CPing
	 * @return true, if pInstance is valid, false otherwise.
	 */
	static bool isValidInstance(const CPing* pInstance);

	/**
	 * Host is reachable again. Notify listener window by sending CPing::PING_HOST_UP.
	 */
	void fireUp();

	/**
	 * Host is considered down. Notify listener window by sending CPing::PING_HOST_DOWN.
	 */
	void fireDown();

	/**
	 * Send single ICMP packet and wait for answer. 
	 *
	 * @return true, if host is considered up, false otherwise.
	 */
	bool ping();

	/**
	 * Thread function which periodically pings the host.
	 */
	void pingThread();

	/**
	 * create the raw socket used for ICMP messages.
	 */
	void createSocket();

	/**
	 * destination address
	 */
	sockaddr_in m_saDest;

	/**
	 * ip address as created by inet_addr
	 */
	unsigned long m_ulInetAddr;

	/**
	 * Host name of host to ping. If m_bResolveAddress is false, containts textual representation of IP address
	 */
	char * m_pszHost;

	/**
	 * Data size of ICMP packet. Defaults to 32 bytes.
	 */
	unsigned int m_uBufferSize;

	/**
	 * TTL for ICMP packet. Defaults to 60 hops.
	 */
	unsigned int m_uTtl;

	/**
	 * Threshold after how many destination unreachable replys or timeouts a host
	 * is considered unreachable. Default is 4.
	 */
	unsigned int m_uUnreachableThreshold;

	/**
	 * Timeout to wait between the sending of two ICMP packets
	 */
	unsigned int m_uTimeout;

	/**
	 * Window handle of the listener where the Up and Down notifications are sent to.
	 */
	HWND m_hMessageWindow;

	/**
	 * Handle for the pinger thread.
	 */
	HANDLE m_hThread;

	/**
	 * Event used to cancel the pinger thread.
	 */
	HANDLE m_hEvent;

	/**
	 * Thread ID of the pinger thread.
	 */
	unsigned int m_uThreadId;

	/**
	 * Flag which is true if a CPing::PING_HOST_DOWN was already sent to the listener, false otherwise.
	 */
	bool m_bDownSignalled;

	/**
	 * Counter for failed pings.
	 */
	unsigned int m_uFailedPings;

	/// reply buffer for ICMP_ECHO
	char* m_pReplyBuffer;

	/// time in milliseconds to sleep between two calls to ping()
	unsigned int m_uSleep;

	/// sequence count of current packet
	unsigned short m_uSequenceNumber;

	/// Socket descriptor used by ping()
	SOCKET m_sd;
};

#endif // #ifndef _PING_H

