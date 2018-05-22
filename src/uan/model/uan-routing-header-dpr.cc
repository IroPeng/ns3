/*
 * uan-routing-header-dpr.cc
 *
 *  Created on: 2017年11月27日
 *      Author: wjj
 */

#include "uan-routing-header-dpr.h"
#include "ns3/log.h"
#include "ns3/packet.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("DPRHeader");

NS_OBJECT_ENSURE_REGISTERED(TypeHeader);

TypeHeader::TypeHeader(MessageType t)
	:m_type(t)
{
	NS_LOG_FUNCTION(this);
}

TypeHeader::~TypeHeader()
{
}

TypeId
TypeHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DPR::TypeHeader")
		.SetParent<Header>()
		.SetGroupName("DPR")
		.AddConstructor<TypeHeader>()
	;
	return tid;
}

TypeId
TypeHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

void
TypeHeader::SetMessageType(MessageType type)
{
	NS_LOG_FUNCTION(this << type);
	m_type = type;
}

TypeHeader::MessageType
TypeHeader::GetMessageType()
{
	return m_type;
}
uint32_t
TypeHeader::GetSerializedSize() const
{
	return sizeof(uint8_t);
}

void
TypeHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteU8((uint8_t) m_type);
}

uint32_t
TypeHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	uint8_t type = i.ReadU8();
	m_type = (MessageType)type;
	uint32_t dist = i.GetDistanceFrom (start);
	NS_ASSERT (dist == GetSerializedSize ());
	return dist;
}

void
TypeHeader::Print (ostream &os) const
{
  switch (m_type)
    {
    case REQUIRE:
      {
        os << "require";
        break;
      }
    case RESPONSE:
      {
        os << "response";
        break;
      }
    case DATA:
	  {
		os << "data";
		break;
	  }
    default:
      os << "unknown type";
    }
}

bool
TypeHeader::operator== (TypeHeader const & o) const
{
	return m_type == o.m_type;
}

RequireHeader::RequireHeader(double nodeId, double senderDepth, double m_deltaDepth, double sendPower)
	:m_nodeId(nodeId),
	 m_senderDepth(senderDepth),
	 m_sendPower(sendPower)
{
	NS_LOG_FUNCTION(this);
}

RequireHeader::~RequireHeader()
{
}

TypeId
RequireHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DPR::RequireHeader")
		.SetParent<Header>()
		.SetGroupName("DRP")
		.AddConstructor<RequireHeader>()
	;
	return tid;
}

TypeId
RequireHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
RequireHeader::GetSerializedSize() const
{
	return sizeof(double)*2 + sizeof(uint16_t);
}

void
RequireHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_nodeId);
	start.WriteHtonU64((uint32_t)(m_senderDepth * 1000 + 0.5));
	start.WriteHtonU64((uint32_t)(m_sendPower * 1000 + 0.5));
}

uint32_t
RequireHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;

	m_nodeId = i.ReadNtohU16();
	m_senderDepth = ((double)i.ReadNtohU64())/1000;
	m_sendPower = ((double)i.ReadNtohU64())/1000;

	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
RequireHeader::Print(ostream &os) const
{
	os << "The require node id is " << m_nodeId << endl;
	os << "The depth of this node is " << m_senderDepth << endl;
	os << "The sendPower of this node is " << m_sendPower << endl;
}

bool
RequireHeader::operator== (RequireHeader const & o) const
{
  return (m_nodeId == o.m_nodeId && m_senderDepth == o.m_senderDepth
		  && m_sendPower ==o.m_sendPower);
}

ResponseHeader::ResponseHeader(uint16_t nodeid, uint16_t destid, double recverDepth,
		double recvPower, double reEnergy)
	:m_nodeId(nodeid),
	 m_destId(destid),
	 m_recverDepth(recverDepth),
	 m_recvPower(recvPower),
	 m_reEnergy(reEnergy)
{
	NS_LOG_FUNCTION(this);
}

ResponseHeader::~ResponseHeader()
{
	NS_LOG_FUNCTION(this);
}

TypeId
ResponseHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DPR::ResponseHeader")
		.SetParent<Header>()
		.SetGroupName("DPR")
		.AddConstructor<ResponseHeader>()
	;
	return tid;
}

TypeId
ResponseHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
ResponseHeader::GetSerializedSize() const
{
	return sizeof(double) * 3 + sizeof(uint16_t) * 2;
}

void
ResponseHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_nodeId);
	start.WriteHtonU16(m_destId);
	start.WriteHtonU64((uint32_t)(m_recverDepth * 1000 + 0.5));
	start.WriteHtonU64((uint32_t)(m_recvPower * 1000 + 0.5));
	start.WriteHtonU64((uint32_t)(m_reEnergy * 1000 + 0.5));
}

uint32_t
ResponseHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);

	Buffer::Iterator i = start;
	m_nodeId = i.ReadNtohU16();
	m_destId = i.ReadNtohU16();
	m_recverDepth = ((double)i.ReadNtohU64())/1000;
	m_recvPower = ((double)i.ReadNtohU64())/1000;
	m_reEnergy = ((double)i.ReadNtohU64())/1000;

	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
ResponseHeader::Print(ostream &os) const
{
	os << "The response node is " << m_nodeId << endl;
	os << "The require node is " << m_destId << endl;
	os << "The recverDepth is " << m_recverDepth << endl;
	os << "The m_recvPower is " << m_recvPower << endl;
	os << "The m_reEnergy is " << m_reEnergy << endl;
}

bool
ResponseHeader::operator== (ResponseHeader const & o) const
{
	return (m_nodeId == o.m_nodeId && m_destId == o.m_destId && m_recverDepth == o.m_recverDepth
			&& m_recvPower == o.m_recvPower && m_reEnergy == o.m_reEnergy);
}

DataHeader::DataHeader(uint16_t packetId, uint16_t nextId)
	:m_packetId(packetId),
	 m_nextId(nextId)
{
	NS_LOG_FUNCTION(this);
}

DataHeader::~DataHeader()
{
	NS_LOG_FUNCTION(this);
}

TypeId
DataHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::DPR::DataHeader")
		.SetParent<Header>()
		.SetGroupName("DPR")
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
	return sizeof(uint16_t) * 2;
}

void
DataHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_packetId);
	start.WriteHtonU16(m_nextId);
}

uint32_t
DataHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);

	Buffer::Iterator i = start;
	m_packetId = i.ReadNtohU16();
	m_nextId = i.ReadNtohU16();

	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
DataHeader::Print(ostream &os) const
{
	os << "The packetId is " << m_packetId << endl;
	os << "The next node is " << m_nextId << endl;
}

bool
DataHeader::operator== (DataHeader const & o) const
{
	return (m_packetId == o.m_packetId && m_nextId == o.m_nextId);
}


