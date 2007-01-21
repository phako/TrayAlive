/*
   IcmpPacket.h - include file for ICMP packet structure

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

#ifndef _ICMP_PACKET_H
#define _ICMP_PACKET_H

#include "inttypes.h"

class IcmpPacket
{
	private:
		// private copy constructor
		IcmpPacket(IcmpPacket& p) {};
	public:

		enum {
			ICMP_TYPE_ECHO_REPLY = 0,
			ICMP_TYPE_ECHO_REQUEST = 8
		};
		/**
		 * Constructor. Create a fresh ICMP packet with size, id and
		 * sequence number.
		 *
		 * @param size of the ICMP packet including header.
		 * @param id unique identifier of the packet
		 * @param sequence is the sequence number of the packet
		 */
		IcmpPacket(const unsigned int id, 
				const unsigned int sequence,
				const unsigned int size = 32);

		/**
		 * Constructor. Parse raw data as received from RAW socket in
		 * an ICMP packet.
		 *
		 * @param payload the raw payload.
		 * @param size of the payload
		 */
		IcmpPacket(const char* payload, const int size);

		/**
		 * Destructor. Free all buffers.
		 */
		~IcmpPacket();

		/**
		 * Get the raw data of the ICMP packet for sending.
		 * 
		 * @return const pointer to raw ICMP packet data
		 */
		const char* rawData() const;

		/**
		 * Get the size of the ICMP packet.
		 *
		 * @return size of packet in bytes.
		 */
		const unsigned int getSize() const { return m_size; };

		/**
		 * Get the sequence number of the ICMP packet.
		 *
		 * @return sequence number in host byte order
		 */
		const unsigned int getSequenceNumber() const { return ntohs(m_icmp_packet->header.seq); }

		/**
		 * Get the ICMP type of the packet
		 *
		 * @return the icmp type
		 */
		const unsigned int getType() const { return ntohs(m_icmp_packet->header.type); }
		
		/**
		 * Get the ICMP packet identifier.
		 *
		 * @return the ICMP packet identifier in host byte order.
		 */
		const unsigned int getId() const { return ntohs(m_icmp_packet->header.id); }
	private:

		/** 
		 * fill the data part of the ICMP packet
		 */
		void fillData();

		/**
		 * Fill the checksum header field.
		 */
		void calculateChecksum();

		/**
		 * Struct for the ICMP header.
		 */
		struct icmp_header 
		{
			uint8_t type;
			uint8_t code;
			uint16_t checksum;
			uint16_t id;
			uint16_t seq;
			uint32_t timestamp;
		};

		/**
		 * Struct for the ICMP packet including header and data.
		 * data is used as the start address of the actual data here.
		 */
		struct icmp_packet
		{
			icmp_header header;
			char data;
		};

		/**
		 * Buffer containing the raw ICMP packet.
		 */
		char* m_buffer;

		/**
		 * Size of m_buffer.
		 */
		unsigned int m_size;

		/**
		 * Pointer to m_buffer for easy access of the fields of the 
		 * packet.
		 */
		icmp_packet* m_icmp_packet;

		/**
		 * Static member containing the data used to fill the ICMP
		 * packet. 0xCAFEBABE, which is tha magic for Java class files.
		 */
		static const uint32_t m_dataBytes;
};
#endif
