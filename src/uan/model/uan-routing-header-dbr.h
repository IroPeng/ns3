/*
 * uan-routing-header-dbr.h
 *
 *  Created on: 2018年5月23日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_HEADER_DBR_H
#define UAN_ROUTING_HEADER_DBR_H

#include "uan-routing-test.h"
#include "uan-routing-header.h"
#include "ns3/vector.h"
#include "ns3/random-variable-stream.h"
#include "ns3/packet.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"
#include "ns3/timer.h"
#include <deque>
#include <iterator>
#include <vector>
#include "algorithm"
#include <iostream>

using namespace std;
namespace ns3 {

class DBRDataHeader : public Header
{
public:
	DBRDataHeader(uint16_t senderId = 0, uint16_t packetId = 0, uint16_t depth = 0, uint16_t deltaDepth = 0);
	virtual ~DBRDataHeader();
	static TypeId GetTypeId();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	//Setters and Getters
	uint16_t getDepth() const {
		return m_depth;
	}

	void setDepth(uint16_t depth) {
		m_depth = depth;
	}

	uint16_t getPacketId() const {
		return m_packetId;
	}

	void setPacketId(uint16_t packetId) {
		m_packetId = packetId;
	}

	uint16_t getSenderId() const {
		return m_senderId;
	}

	void setSenderId(uint16_t senderId) {
		m_senderId = senderId;
	}

	uint16_t getDeltaDepth(){
		return m_deltaDepth;
	}

	void setDeltaDepth(uint16_t deltaDepth){
		m_deltaDepth = deltaDepth;
	}

	bool operator== (DBRDataHeader const & o) const;
private:
	uint16_t m_senderId;
	uint16_t m_packetId;
	uint16_t m_depth;
	uint16_t m_deltaDepth;
};

}


#endif /* UAN_ROUTING_HEADER_DBR_H */
