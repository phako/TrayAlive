/*
   IpPacket.h - Thin OOP wrapper for 
   
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

#include "inttypes.h"

#pragma pack(1)

/**
 * IpPacket is a thin wrapper class for the IP header. This is done to hide the
 * pointer arithmetic for accessing the members of the IP header as well as the
 * payload.
 */
class IpPacket
{
	public:
		/**
		 * Maximum size of IP packet including header information
		 */
		const static unsigned int MAX_PACKET_SIZE;

		/**
		 * Maximum size of payload = MAX_PACKET_SIZE - sizeof(ip_header)
		 */
		const static unsigned int MAX_PAYLOAD_SIZE;
		/**
		 * Constructor. Creates the IpPacket from */
		IpPacket(const char* rawPacket, const unsigned int size);

		/**
		 * Get a const pointer to the payload of the IP packet.
		 *
		 * @return a const char pointer pointing to the beginning of
		 * the payload
		 */
		const char* getPayload() const { return &(m_ip_packet->payload); };

		/**
		 * Get the size of the payload contained in the IP packet.
		 *
		 * @return the size of the payload in bytes.
		 */
		const unsigned int getPayloadSize() const { return m_payloadSize; };
	private:
		// this is not endianess safe!!
		struct ip_header
		{
			uint8_t h_len:4;
			uint8_t version:4;
			uint8_t tos;
			uint16_t total_length;
			uint16_t ident;
			uint16_t flags;
			uint8_t ttl;
			uint8_t proto;
			uint16_t checksum;
			uint32_t source_ip;
			uint32_t dest_ip;
		};

		struct ip_packet
		{
			ip_header header;
			char payload;
		};

		/// Raw data buffer for the IP packet
		char *m_buffer;

		/// Pointer to the IP packet structure.
		ip_packet* m_ip_packet;

		/// Size of the IP packet.
		unsigned int m_size;

		/// Size of the payload contained in the IP packet.
		unsigned int m_payloadSize;
};
