/*
 * uan-routing-pcdbr.h
 *
 *  Created on: 2018年9月3日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_PCDBR_H
#define UAN_ROUTING_PCDBR_H

#include "uan-routing-test.h"
#include "uan-routing-header.h"
#include "uan-routing-header-dbr.h"
#include "ns3/vector.h"
#include "ns3/random-variable-stream.h"
#include "ns3/packet.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"
#include "ns3/timer.h"
#include "uan-upper-layer.h"

namespace ns3 {

struct PCDBRPri{
	uint8_t nodeId;
	double deltaPower;
	double depth;
};

class UanRoutingPCDBR : public UanRouting
{
public:
	UanRoutingPCDBR();
	virtual ~UanRoutingPCDBR();
	static TypeId GetTypeId(void);

	//core
	bool Recv(Ptr<Packet> packet, const UanAddress &dest, uint16_t protocolNumber){return true;}
	bool Recv(Ptr<Packet> packet, const UanAddress &dest);
	//Getters and Setters
	void SetNodeID(uint16_t nodeid) {m_nodeId = nodeid;}
	uint16_t GetNodeID() {return m_nodeId;}
	void SetDepth(uint16_t depth) {m_depth = depth;}
	uint16_t GetDepth() {return m_depth;}
	void SetDeltaDepth(uint16_t deltaDepth) {m_deltaDepth = deltaDepth;}
	uint16_t GetDeltaDepth() {return m_deltaDepth;}
	void SetTxPower(uint16_t txPower);
	uint16_t GetTxPower() {return m_txPower;}
	void SetRxPower(uint16_t rxPower) {m_rxPower = rxPower;}
	uint16_t GetRxPower() {return m_rxPower;}
	void SetReEnergy(uint16_t reEnergy) {m_reEnergy = reEnergy;}
	uint16_t GetReEnergy() {return m_reEnergy;}

public:
	void SendRequire();
	void SendResponse(uint16_t reqNodeId, double reqDepth, double reqTxPower);
	void HandleRecvRequire(Ptr<Packet> p);
	void HandleRecvResponse(Ptr<Packet> p);
	void HandleRecvData(Ptr<Packet> p);
	void SendData(PCDBRPri maxPri);
	void SendData(uint32_t nodeId, uint32_t pktNum);
	void Start();
	void WaitRespTO();
	void SetUanIp(Ptr<UanIp> uanip);

private:
	uint16_t m_nodeId;
	uint16_t m_depth;
	uint16_t m_deltaDepth;
	uint16_t m_txPower;
	uint16_t m_rxPower;
	bool m_sendRequire;
	double m_responseWT;
	uint16_t m_reEnergy;
	std::deque<uint16_t> m_requiresReply;
	std::map<uint16_t, EventId> m_requiresToBeReply;
	Ptr<UanIp> m_uanip;
	std::deque<PCDBRPri> m_pri;
	Ptr<Packet> m_pkt;
	std::deque<Ptr<Packet>> m_pkts;

public:
	static uint16_t sendRequireNum;
	static uint16_t recvRequireNum;
	static uint16_t sendResponseNum;
	static uint16_t recvResponseNum;
	static uint16_t sendDataNum;
	static uint16_t recvDataNum;
	static uint16_t sinkRecvDataNum;
	static vector<map<int, int>> packetSituation;
};

}

#endif /* UAN_ROUTING_PCDBR_H */
