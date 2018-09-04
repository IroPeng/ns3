/*
 * uan-routing-header-pcdbr.h
 *
 *  Created on: 2018年8月19日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_HEADER_PCDBR_H
#define UAN_ROUTING_HEADER_PCDBR_H

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

class PCDBRTypeHeader : public Header
{
public:
	enum MessageType
	{
		REQUIRE,
		RESPONSE,
		DATA
	};
	PCDBRTypeHeader(MessageType t = REQUIRE);
	~PCDBRTypeHeader();
	static TypeId GetTypeId();
	//Getters and Setters
	void SetMessageType(MessageType type);
	MessageType GetMessageType();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	bool operator== (PCDBRTypeHeader const & o) const;
private:
	MessageType m_type;
};

class PCDBRRequireHeader : public Header
{
public:
	PCDBRRequireHeader (double nodeId = 0, double senderDepth = 0, double deltaDepth = 0, double sendPower = 0);
	virtual ~PCDBRRequireHeader();
	static TypeId GetTypeId();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	//Getters and Setters
	void SetNodeId(int nodeid) {m_nodeId = nodeid;}
	int GetNodeId() const {return m_nodeId;}
	void SetSenderDepth(int senderDepth) {m_senderDepth = senderDepth;}
	double GetSenderDepth() const {return m_senderDepth;}
	void SetDeltaDepth(int deltaDepth) {m_deltaDepth = deltaDepth;}
	double GetDeltaDepth() const {return m_deltaDepth;}
	void SetSendPower(int sendPower) {m_sendPower = sendPower;}
	double GetSendPower() const {return m_sendPower;}
private:
	uint16_t m_nodeId;
	double m_senderDepth;
	double m_deltaDepth;
	double m_sendPower;
};

class PCDBRResponseHeader : public Header
{
public:
	PCDBRResponseHeader(uint16_t nodeid = 0, uint16_t destid = 0, double recverDepth = 0,
					double recvPower = 0, double reEnergy = 0);
	virtual ~PCDBRResponseHeader();
	static TypeId GetTypeId();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	//Getters and Setters
	void SetNodeId(uint16_t nodeid) {m_nodeId = nodeid;}
	uint16_t GetNodeId() const {return m_nodeId;}
	void SetDestId(uint16_t destid) {m_destId = destid;}
	uint16_t GetDestId() const {return m_destId;}
	void SetRecverDepth(double recverDepth) {m_recverDepth = recverDepth;}
	double GetRecverDepth() const {return m_recverDepth;}
	void SetRecvPower(double recvPower) {m_recvPower = recvPower;}
	double GetRecvPower() const {return m_recvPower;}
	void SetReEnergy(double reEnergy) {m_reEnergy = reEnergy;}
	double GetReEnergy() const {return m_reEnergy;}

	bool operator== (PCDBRResponseHeader const & o) const;
private:
	uint16_t m_nodeId;
	uint16_t m_destId;
	double m_recverDepth;
	double m_recvPower;
	double m_reEnergy;
};

class PCDBRDataHeader : public Header
{
public:
	PCDBRDataHeader(uint16_t senderId = 0, uint16_t packetId = 0, uint16_t nextId = 0, uint16_t hop = 0);
	virtual ~PCDBRDataHeader();
	static TypeId GetTypeId();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	//Setters and Getters
	uint16_t getSenderId() const {
		return m_senderId;
	}
	void setSenderId(uint16_t senderId) {
		m_senderId = senderId;
	}
	uint16_t getPacketId() const {
		return m_packetId;
	}
	void setPacketId(uint16_t packetId) {
		m_packetId = packetId;
	}
	uint16_t getNextId() const {
		return m_nextId;
	}
	void setNextId(uint16_t nextId) {
		m_nextId = nextId;
	}
	uint16_t getHop() const {
		return m_hop;
	}

	void setHop(uint16_t hop) {
		m_hop = hop;
	}
private:
	uint16_t m_senderId;
	uint16_t m_packetId;
	uint16_t m_nextId;
	uint16_t m_hop;
};

}

#endif /* UAN_ROUTING_HEADER_PCDBR_H */
