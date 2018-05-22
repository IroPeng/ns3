/*
 * uan-routing-header-dpr.h
 *
 *  Created on: 2017年11月27日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_HEADER_DPR_H
#define UAN_ROUTING_HEADER_DPR_H

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
namespace ns3{

class TypeHeader : public Header
{
public:
	enum MessageType
	{
		REQUIRE,
		RESPONSE,
		DATA
	};
	TypeHeader(MessageType t = REQUIRE);
	~TypeHeader();
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

	bool operator== (TypeHeader const & o) const;
private:
	MessageType m_type;
};

class RequireHeader : public Header
{
public:
	RequireHeader(){}
	RequireHeader (double m_nodeId, double m_sendDepth, double m_deltaDepth, double m_sendPower);
	virtual ~RequireHeader();
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
	bool operator== (RequireHeader const & o) const;
private:
	uint16_t m_nodeId;
	double m_senderDepth;
	double m_deltaDepth;
	double m_sendPower;
};

class ResponseHeader : public Header
{
public:
	ResponseHeader(uint16_t nodeid = 0, uint16_t destid = 0, double recverDepth = 0,
					double recvPower = 0, double reEnergy = 0);
	virtual ~ResponseHeader();
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

	bool operator== (ResponseHeader const & o) const;
private:
	uint16_t m_nodeId;
	uint16_t m_destId;
	double m_recverDepth;
	double m_recvPower;
	double m_reEnergy;
};
class DataHeader : public Header
{
public:
	DataHeader(uint16_t m_packetId = 0, uint16_t m_nextId = 0);
	virtual ~DataHeader();
	static TypeId GetTypeId();

	//inherited
	TypeId GetInstanceTypeId () const;
	uint32_t GetSerializedSize () const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	void Print (ostream &os) const;

	//Setters and Getters
	void SetPacketId(uint16_t packetId) {m_packetId = packetId;}
	uint16_t GetPacketId() const {return m_packetId;}
	void SetNextId(uint16_t nextId) {m_nextId = nextId;}
	uint16_t GetNextId() const {return m_nextId;}
	void SetSrcId(uint16_t srcId) {m_srcId = srcId;}
	uint16_t GetSrcId() const {return m_srcId;}

	bool operator== (DataHeader const & o) const;
private:
	uint16_t m_srcId;
	uint16_t m_packetId;
	uint16_t m_nextId;
};
}

#endif
