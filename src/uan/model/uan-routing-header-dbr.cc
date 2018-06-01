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

NS_OBJECT_ENSURE_REGISTERED(DataHeader);

DataHeader::DataHeader(uint16_t senderId = 0, uint16_t packetId = 0, uint16_t depth = 0, uint16_t deltaDepth = 0)
	:m_senderId(senderId),
	 m_packetId(packetId),
	 m_depth(depth),
	 m_deltaDepth(deltaDepth)
{
	NS_LOG_FUNCTION(this);
}

DataHeader::~DataHeader()
{
}

TypeId
DataHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DBR::DataHeader")
			.SetParent<Header>()
			.SetGroupName("DBR")
			.AddConstructor<DataHeader>()
		;
	return tid;
}

TypeId
DataHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
DataHeader::GetSerializedSize() const
{
	return sizeof(uint16_t) * 3;
}

void
DataHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_senderId);
	start.WriteHtonU16(m_packetId);
	start.WriteHtonU16(m_depth);
	start.WriteHtonU16(m_deltaDepth);
}

uint32_t
DataHeader::Deserialize(Buffer::Iterator start)
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
DataHeader::Print(ostream &os) const
{
	os << "The senderId is " << m_senderId << endl;
	os << "The packetId is " << m_packetId << endl;
	os << "The depth is " << m_depth << endl;
	os << "The deltaDepth is " << m_deltaDepth << endl;
}

bool
DataHeader::operator==(DataHeader const & o) const
{
	return (m_packetId == o.m_packetId && m_nextId == o.m_nextId);
}

}
