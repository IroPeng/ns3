/*
 * uan-routing-dpr.h
 *
 *  Created on: 2017年11月28日
 *      Author: wjj
 */

#ifndef UAN_ROUTING_DPR_H
#define UAN_ROUTING_DPR_H

#include "uan-routing-test.h"
#include "uan-routing-header.h"
#include "uan-routing-header-dpr.h"
#include "ns3/vector.h"
#include "ns3/random-variable-stream.h"
#include "ns3/packet.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"
#include "ns3/timer.h"
#include "uan-upper-layer.h"

#include <deque>

namespace ns3{

struct Pri{
	uint8_t nodeId;
	double deltaPower;
	double priority;
};

class UanRoutingDPR : public UanRouting
{
	public:
		UanRoutingDPR();
		virtual ~UanRoutingDPR();
		static TypeId GetTypeId(void);
		//core
		bool Recv(Ptr<Packet> packet, const UanAddress &dest, uint16_t protocolNumber){return true;}
		bool Recv(Ptr<Packet> packet, const UanAddress &dest);
		//Getters and Setters
		void SetNodeID(uint16_t nodeid) {m_nodeId = nodeid;}
		uint16_t GetNodeID() {return m_nodeId;}
		void SetDepth(double depth) {m_depth = depth;}
		double GetDepth() {return m_depth;}
		void SetDeltaDepth(double deltaDepth) {m_deltaDepth = deltaDepth;}
		double GetDeltaDepth() {return m_deltaDepth;}
		void SetTxPower(double txPower);
		double GetTxPower() {return m_txPower;}
		void SetRxPower(double rxPower) {m_rxPower = rxPower;}
		double GetRxPower() {return m_rxPower;}
		void SetReEnergy(double reEnergy) {m_reEnergy = reEnergy;}
		double GetReEnergy() {return m_reEnergy;}
		void SetResponseWT(double reponseWT) {m_responseWT = reponseWT;};
		double GetResponseWT() { return m_responseWT;}

	public:
		void Start();
		void SendData(uint32_t nodeId, uint32_t pktNum);
		void SendRequire();
		void SendResponse(uint16_t destid);
		void HandleRecvRequire(Ptr<Packet> p);
		void HandleRecvResponse(Ptr<Packet> p);
		void HandleRecvData(Ptr<Packet> p);
		void SendData(Pri maxPri);
		void WaitRespTO();
		void SetUanIp(Ptr<UanIp> uanip);

	private:
		uint16_t m_nodeId;
		double m_depth;
		double m_deltaDepth;
		double m_txPower;
		double m_rxPower;
		double m_reEnergy;
		bool m_sendRequire;
		double m_responseWT;
		std::deque<Pri> m_pri;
		Ptr<Packet> m_pkt;
		std::deque<Ptr<Packet>> m_pkts;
		Ptr<UanIp> m_uanip;
	public:
		static uint16_t sendDataNum;
		static uint16_t recvDataNum;
		static uint16_t sendRequireNum;
		static uint16_t recvRequireNum;
		static uint16_t sendResponseNum;
		static uint16_t recvResponseNum;
		static uint16_t sinkRecvDataNum;
	};
}

#endif
