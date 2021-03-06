/*
 * uan-routing-dbr.cc
 *
 *  Created on: 2018年5月31日
 *      Author: wjj
 */

#include "uan-routing-dbr.h"
#include "ns3/log.h"

using namespace std;
using namespace ns3;

#define MAX_WAIT_TIME 10;

NS_LOG_COMPONENT_DEFINE("UanRoutingDBR");
NS_OBJECT_ENSURE_REGISTERED(UanRoutingDBR);

uint16_t UanRoutingDBR::sendDataNum = 0;
uint16_t UanRoutingDBR::recvDataNum = 0;
uint16_t UanRoutingDBR::sinkRecvDataNum = 0;
vector<map<int, int>> UanRoutingDBR::packetSituation;

UanRoutingDBR::UanRoutingDBR()
	:m_nodeId(0),
	 m_depth(0),
	 m_pktsSent(0)
	 //m_pktsToBeSend()
{
	NS_LOG_FUNCTION(this);
}

UanRoutingDBR::~UanRoutingDBR()
{
	NS_LOG_FUNCTION(this);
}

TypeId
UanRoutingDBR::GetTypeId()
{
	static TypeId tid = TypeId("ns3::UanRoutingDBR")
			.SetParent<UanRouting>()
			.AddConstructor<UanRoutingDBR>()
			.AddAttribute("NodeID", "The id of the node",
						UintegerValue(),
						MakeUintegerAccessor (&UanRoutingDBR::m_nodeId),
						MakeUintegerChecker<uint16_t> ())
			.AddAttribute("Depth", "The depth of the node",
						DoubleValue (),
						MakeDoubleAccessor (&UanRoutingDBR::m_depth),
						MakeDoubleChecker<double> ())
			;
	return tid;
}

bool
UanRoutingDBR::Recv(Ptr<Packet> p, const UanAddress &dest)
{
	NS_LOG_INFO("Node " << m_nodeId << " receive packet");
	NS_ASSERT(p != 0);

	Ptr<Packet> pkt = p->Copy();

	UanRoutingHeader routingHeader;
	pkt->RemoveHeader(routingHeader);

	NS_LOG_INFO("Packet source is " << routingHeader.GetSAddr());
	NS_LOG_INFO("Packet destination is " << routingHeader.GetDAddr());
	NS_LOG_INFO("Packet NextHop is " << routingHeader.GetNextHop());
	if(routingHeader.GetNextHop() == GetNetDevice()->GetAddress() ||
				routingHeader.GetNextHop() == UanAddress::ConvertFrom(GetNetDevice()->GetAddress()).GetBroadcast())
	{
		DBRDataHeader dataHeader;
		pkt->RemoveHeader(dataHeader);
		uint16_t senderId = dataHeader.getSenderId();
		NS_LOG_INFO("This packet is from Node "<< senderId << " to Node " << m_nodeId);

		for(uint32_t i = 0; i < m_pktsSent.size(); i++){
			if(m_pktsSent[i] == dataHeader.getPacketId()){
				UanRoutingDBR::recvDataNum++;
				return false;
			}
		}

		if (m_depth == 0) {
			UanRoutingDBR::sinkRecvDataNum++;
			NS_LOG_INFO("sink " << m_nodeId << " receive data");
			m_pktsSent.push_back(dataHeader.getPacketId());
			return true;
		} else {
			std::map<uint16_t, EventId>::iterator it = m_pktsToBeSend.find(dataHeader.getPacketId());
			if(it != m_pktsToBeSend.end()){
				EventId eventId = it->second;
				eventId.Cancel();
				m_pktsToBeSend.erase(it);
			}
			UanRoutingDBR::recvDataNum++;
			if (dataHeader.getDepth() - m_depth >= dataHeader.getDeltaDepth()) {
				double deltaDepth = dataHeader.getDepth() - m_depth;
				double waitTime = 10 * (1-deltaDepth/(double)500) * (1-deltaDepth/(double)500);
				forwardPacket(p, waitTime);
				return true;
			}
		}
	}
	return false;
}

void
UanRoutingDBR::forwardPacket(Ptr<Packet> p, double waitTime)
{
	NS_LOG_INFO("Node " << m_nodeId << " forward data");
	UanRoutingHeader routingHeader;
	p->RemoveHeader(routingHeader);
	DBRDataHeader dataHeader;
	p->RemoveHeader(dataHeader);
	uint16_t currentHop = dataHeader.getHop();
	uint16_t currentPktSeq = dataHeader.getPacketId();

	dataHeader.setHop(currentHop + 1);
	dataHeader.setSenderId(m_nodeId);
	dataHeader.setDepth(m_depth);
	p->AddHeader(dataHeader);
	routingHeader.SetSAddr(UanAddress(m_nodeId));
	routingHeader.SetNextHop(UanAddress::GetBroadcast());
	routingHeader.SetDAddr(UanAddress::GetBroadcast());
	p->AddHeader(routingHeader);
	UanRoutingDBR::sendDataNum++;
	EventId eventId = Simulator::Schedule(Seconds(waitTime), &UanRoutingDBR::SendData, this, p, currentPktSeq, currentHop);
	m_pktsToBeSend.insert(pair<uint16_t, EventId>(dataHeader.getPacketId(), eventId));
}

void
UanRoutingDBR::SendData(Ptr<Packet> p, uint16_t currentPktSeq, uint16_t currentHop)
{
	map<int, int> currentPktMap = UanRoutingDBR::packetSituation.at(currentPktSeq-1);
	currentPktMap[currentHop] = m_nodeId;

	SendDown(p, UanAddress::GetBroadcast(), Seconds(0));
	UanRoutingHeader routingHeader;
	p->RemoveHeader(routingHeader);
	DBRDataHeader dataHeader;
	p->RemoveHeader(dataHeader);
	uint16_t packetId = dataHeader.getPacketId();
	std::map<uint16_t, EventId>::iterator it = m_pktsToBeSend.find(packetId);
	m_pktsToBeSend.erase(it);
	m_pktsSent.push_back(packetId);
}

void
UanRoutingDBR::Start()
{
	m_nodeId = GetNetDevice()->GetNode()->GetId();
	Ptr<MobilityModel> mobilitymodel =  this->GetNetDevice()->GetNode()->GetObject<MobilityModel> ();
	m_depth = mobilitymodel->GetPosition().z;
	NS_LOG_INFO("Node " << m_nodeId << " start");
}

void
UanRoutingDBR::SendDataAsSrc(uint32_t pktNum)
{
	NS_LOG_INFO("Node " << m_nodeId << " send data as source, depth is " << m_depth << "m");
	Ptr<Packet> p = Create<Packet>();
	DBRDataHeader dataHeader;
	dataHeader.setSenderId(m_nodeId);
	dataHeader.setPacketId(pktNum);
	dataHeader.setDepth(m_depth);
	dataHeader.setDeltaDepth(0);
	p->AddHeader(dataHeader);

	UanRoutingHeader routingHeader;
	routingHeader.SetSAddr(UanAddress(m_nodeId));
	routingHeader.SetNextHop(UanAddress::GetBroadcast());
	routingHeader.SetDAddr(UanAddress::GetBroadcast());
	p->AddHeader(routingHeader);

	SendDown(p, UanAddress::GetBroadcast(), Seconds(0));
}

void
UanRoutingDBR::SetUanIp (Ptr<UanIp> uanip)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_FUNCTION (this << uanip);
	m_uanip = uanip;
	Simulator::ScheduleNow(&UanRoutingDBR::Start,this);
}
