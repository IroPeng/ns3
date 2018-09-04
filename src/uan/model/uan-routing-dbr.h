/*
 * uan-routing-dbr.h
 *
 *  Created on: 2018年5月23日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_DBR_H
#define UAN_ROUTING_DBR_H

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

class UanRoutingDBR : public UanRouting
{
public:
	UanRoutingDBR();
	virtual ~UanRoutingDBR();
	static TypeId GetTypeId(void);

	//core
	bool Recv(Ptr<Packet> packet, const UanAddress &dest, uint16_t protocolNumber){return true;}
	bool Recv(Ptr<Packet> packet, const UanAddress &dest);
	//Getters and Setters
	void SetNodeID(uint16_t nodeid) {m_nodeId = nodeid;}
	uint16_t GetNodeID() {return m_nodeId;}
	void SetDepth(uint16_t depth) {m_depth = depth;}
	uint16_t GetDepth() {return m_depth;}

public:
	void forwardPacket(Ptr<Packet> p, double waitTime);
	void SendData(Ptr<Packet> p, uint16_t currentPktSeq, uint16_t currentHop);
	void Start();
	void SendDataAsSrc(uint32_t pktNum);
	void SetUanIp(Ptr<UanIp> uanip);

private:
	uint16_t m_nodeId;
	uint16_t m_depth;
	std::deque<uint16_t> m_pktsSent;
	std::map<uint16_t, EventId> m_pktsToBeSend;
	Ptr<UanIp> m_uanip;

public:
	static uint16_t sendDataNum;
	static uint16_t recvDataNum;
	static uint16_t sinkRecvDataNum;
	static vector<map<int, int>> packetSituation;
};

}

#endif /* UAN_ROUTING_DBR_H */
