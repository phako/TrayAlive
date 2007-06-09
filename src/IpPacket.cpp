/*
   IpPacket.cpp - implementation of thin OOP wrapper for raw ip packet data.
   
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

/** @file */
#include <memory.h>
#ifdef _WIN32
#   include <winsock2.h>
#else
#   include <arpa/inet.h>
#endif

#include "IpPacket.h"

const unsigned int IpPacket::MAX_PAYLOAD_SIZE = 1024;
const unsigned int IpPacket::MAX_PACKET_SIZE = IpPacket::MAX_PAYLOAD_SIZE + sizeof(IpPacket::ip_header);

IpPacket::IpPacket(const char* rawPacket, const unsigned int size)
{
	m_buffer = new char[size];
	memcpy(m_buffer, rawPacket, size);
	m_ip_packet = (ip_packet *)m_buffer;

	m_size = size;
	m_payloadSize = ntohs(m_ip_packet->header.total_length) - (m_ip_packet->header.h_len * 4);
}

IpPacket::~IpPacket()
{
	if (m_buffer != 0)
	{
		delete[] m_buffer;
		m_buffer = 0;
	}
}