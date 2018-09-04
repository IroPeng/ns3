/*
 * uan-routing-header-pcdbr.cc
 *
 *  Created on: 2018年8月19日
 *      Author: wjj
 */

#include "uan-routing-header-pcdbr.h"
#include "ns3/log.h"
#include "ns3/packet.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("PCDBRHeader");

NS_OBJECT_ENSURE_REGISTERED(PCDBRTypeHeader);

PCDBRTypeHeader::PCDBRTypeHeader(MessageType t)
	:m_type(t)
{
	NS_LOG_FUNCTION(this);
}

PCDBRTypeHeader::~PCDBRTypeHeader()
{
}

TypeId
PCDBRTypeHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::PCDBR::PCDBRTypeHeader")
			.SetParent<Header>()
			.SetGroupName("PCDBR")
			.AddConstructor<PCDBRTypeHeader>()
		;
	return tid;
}

TypeId
PCDBRTypeHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

PCDBRTypeHeader::MessageType
PCDBRTypeHeader::GetMessageType()
{
	return m_type;
}

uint32_t
PCDBRTypeHeader::GetSerializedSize() const
{
	return sizeof(uint8_t);
}

void
PCDBRTypeHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteU8((uint8_t)m_type);
}

uint32_t
PCDBRTypeHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	uint8_t type = i.ReadU8();
	m_type = (MessageType)type;
	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
PCDBRTypeHeader::Print (ostream &os) const
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
PCDBRTypeHeader::operator== (PCDBRTypeHeader const & o) const
{
	return m_type == o.m_type;
}

PCDBRRequireHeader::PCDBRRequireHeader(double nodeId, double senderDepth, double deltaDepth, double sendPower)
	:m_nodeId(nodeId),
	 m_senderDepth(senderDepth),
	 m_deltaDepth(deltaDepth),
	 m_sendPower(sendPower)
{
	NS_LOG_FUNCTION(this);
}

PCDBRRequireHeader::~PCDBRRequireHeader()
{
}

TypeId
PCDBRRequireHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3:PCDBR::PCDBRRequireHeader")
			.SetParent<Header>()
			.SetGroupName("PCDBR")
			.AddConstructor<PCDBRRequireHeader>()
			;
	return tid;
}

TypeId
PCDBRRequireHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
PCDBRRequireHeader::GetSerializedSize() const
{
	return sizeof(double) * 3 + sizeof(uint16_t);
}

void
PCDBRRequireHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_nodeId);
	start.WriteHtonU64((uint32_t)(m_senderDepth));
	start.WriteHtonU64((uint32_t)(m_deltaDepth));
	start.WriteHtonU64((uint32_t)(m_sendPower));
}

uint32_t
PCDBRRequireHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;

	m_nodeId = i.ReadNtohU16();
	m_senderDepth = ((double)i.ReadNtohU64());
	m_deltaDepth = ((double)i.ReadNtohU64());
	m_sendPower = ((double)i.ReadNtohU64());

	uint32_t dist = i.GetDistanceFrom(start);
	return dist;
}

void
PCDBRRequireHeader::Print(ostream &os) const
{
	os << "The require node id is " << m_nodeId << endl;
	os << "The depth of this node is " << m_senderDepth << endl;
	os << "The sendPower of this node is " << m_sendPower << endl;
}

PCDBRResponseHeader::PCDBRResponseHeader(uint16_t nodeId, uint16_t destId, double recverDepth, double recvPower, double reEnergy)
	:m_nodeId(nodeId),
	 m_destId(destId),
	 m_recverDepth(recverDepth),
	 m_recvPower(recvPower),
	 m_reEnergy(reEnergy)
{
	NS_LOG_FUNCTION(this);
}

PCDBRResponseHeader::~PCDBRResponseHeader()
{
	NS_LOG_FUNCTION(this);
}

TypeId
PCDBRResponseHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::PCDBR::PCDBRResponseHeader")
		.SetParent<Header>()
		.SetGroupName("PCDBR")
		.AddConstructor<PCDBRResponseHeader>()
	;
	return tid;
}

TypeId
PCDBRResponseHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
PCDBRResponseHeader::GetSerializedSize() const
{
	return sizeof(double) * 3 + sizeof(uint16_t) * 2;
}

void
PCDBRResponseHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_nodeId);
	start.WriteHtonU16(m_destId);
	start.WriteHtonU64((uint32_t)(m_recverDepth));
	start.WriteHtonU64((uint32_t)(m_recvPower));
	start.WriteHtonU64((uint32_t)(m_reEnergy));
}

uint32_t
PCDBRResponseHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);

	Buffer::Iterator i = start;
	m_nodeId = i.ReadNtohU16();
	m_destId = i.ReadNtohU16();
	m_recverDepth = (double)i.ReadNtohU64();
	m_recvPower = (double)i.ReadNtohU64();
	m_reEnergy = (double)i.ReadNtohU64();

	uint32_t dist = i.GetDistanceFrom(start);
	return dist;
}

void
PCDBRResponseHeader::Print(ostream &os) const
{
	os << "The response node is " << m_nodeId << endl;
	os << "The require node is " << m_destId << endl;
	os << "The recverDepth is " << m_recverDepth << endl;
	os << "The m_recvPower is " << m_recvPower << endl;
	os << "The m_reEnergy is " << m_reEnergy << endl;
}

bool
PCDBRResponseHeader::operator== (PCDBRResponseHeader const & o) const
{
	return (m_nodeId == o.m_nodeId && m_destId == o.m_destId && m_recverDepth == o.m_recverDepth
			&& m_recvPower == o.m_recvPower && m_reEnergy == o.m_reEnergy);
}

PCDBRDataHeader::PCDBRDataHeader(uint16_t senderId, uint16_t packetId, uint16_t nextId, uint16_t hop)
	:m_senderId(senderId),
	 m_packetId(packetId),
	 m_nextId(nextId),
	 m_hop(hop)
{
	NS_LOG_FUNCTION(this);
}

PCDBRDataHeader::~PCDBRDataHeader()
{
	NS_LOG_FUNCTION(this);
}

TypeId
PCDBRDataHeader::GetTypeId()
{
	static TypeId tid = TypeId("ns3::PCDBR::PCDBRDataHeader")
			.SetParent<Header>()
			.SetGroupName("DPR")
			.AddConstructor<PCDBRDataHeader>()
		;
	return tid;
}

TypeId
PCDBRDataHeader::GetInstanceTypeId() const
{
	return GetTypeId();
}

uint32_t
PCDBRDataHeader::GetSerializedSize() const
{
	return sizeof(uint16_t) * 4;
}

void
PCDBRDataHeader::Serialize(Buffer::Iterator start) const
{
	NS_LOG_FUNCTION(this);
	start.WriteHtonU16(m_senderId);
	start.WriteHtonU16(m_packetId);
	start.WriteHtonU16(m_nextId);
	start.WriteHtonU16(m_hop);
}

uint32_t
PCDBRDataHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);

	Buffer::Iterator i = start;
	m_senderId = i.ReadNtohU16();
	m_packetId = i.ReadNtohU16();
	m_nextId = i.ReadNtohU16();
	m_hop = i.ReadNtohU16();

	uint32_t dist = i.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
PCDBRDataHeader::Print(ostream &os) const
{
	os << "The senderId is " << m_senderId << endl;
	os << "The packetId is " << m_packetId << endl;
	os << "The next node is " << m_nextId << endl;
	os << "The hop is " << m_hop << endl;
}




