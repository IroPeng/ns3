/*
 * uan-routing-header-dbr.cc
 *
 *  Created on: 2018年5月23日
 *      Author: wjj
 */

#include "uan-routing-header-dbr.h"
#include "ns3/log.h"
#include "ns3/packet.h"

using namespace std;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("DBRHeader");

NS_OBJECT_ENSURE_REGISTERED(DBRDataHeader);

DBRDataHeader::DBRDataHeader(uint16_t senderId, uint16_t packetId, uint16_t depth, uint16_t deltaDepth)
	:m_senderId(senderId),
	 m_packetId(packetId),
	 m_depth(depth),
	 m_deltaDepth(deltaDepth)
{
	NS_LOG_FUNCTION(this);
}

DBRDataHeader::~DBRDataHeader()
{
}

TypeId
DBRDataHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DBR::DBRDataHeader")
			.SetParent<Header>()
			.SetGroupName("DBR")
			.AddConstructor<DBRDataHeader>()
		;
	return tid;
}

TypeId
DBRDataHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
DBRDataHeader::GetSerializedSize() const
{
	return sizeof(uint16_t) * 4;
}

void
DBRDataHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_senderId);
	start.WriteHtonU16(m_packetId);
	start.WriteHtonU16(m_depth);
	start.WriteHtonU16(m_deltaDepth);
}

uint32_t
DBRDataHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);

	Buffer::Iterator i = start;
	m_senderId = i.ReadNtohU16();
	m_packetId = i.ReadNtohU16();
	m_depth = i.ReadNtohU16();
	m_deltaDepth = i.ReadNtohU16();

	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
DBRDataHeader::Print(ostream &os) const
{
	os << "The senderId is " << m_senderId << endl;
	os << "The packetId is " << m_packetId << endl;
	os << "The depth is " << m_depth << endl;
	os << "The deltaDepth is " << m_deltaDepth << endl;
}

bool
DBRDataHeader::operator==(DBRDataHeader const & o) const
{
	return (m_packetId == o.m_packetId && m_senderId == o.m_senderId);
}

}
