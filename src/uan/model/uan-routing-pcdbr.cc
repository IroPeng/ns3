/*
 * uan-routing-pcdbr.cc
 *
 *  Created on: 2018年9月3日
 *      Author: wjj
 */

#include "uan-routing-pcdbr.h"
#include "ns3/log.h"

using namespace std;
using namespace ns3;

uint16_t UanRoutingPCDBR::sendRequireNum = 0;
uint16_t UanRoutingPCDBR::recvRequireNum = 0;
uint16_t UanRoutingPCDBR::sendResponseNum = 0;
uint16_t UanRoutingPCDBR::recvResponseNum = 0;
uint16_t UanRoutingPCDBR::sendDataNum = 0;
uint16_t UanRoutingPCDBR::recvDataNum = 0;
uint16_t UanRoutingPCDBR::sinkRecvDataNum = 0;
vector<map<int, int>> UanRoutingPCDBR::packetSituation;

UanRoutingPCDBR::UanRoutingPCDBR()
	:m_nodeId(0),
	 m_depth(0),
	 m_deltaDepth(0),
	 m_requiresReply(0),
	 m_txPower(60),
	 m_rxPower(0),
	 m_reEnergy(100),
	 m_responseWT(60),
	 m_sendRequire(false)
{
	NS_LOG_FUNCTION(this);
}

UanRoutingPCDBR::~UanRoutingPCDBR()
{
	NS_LOG_FUNCTION(this);
}

TypeId
UanRoutingPCDBR::GetTypeId()
{
	static TypeId tid = TypeId("ns3::UanRoutingPCDBR")
				.SetParent<UanRouting>()
				.AddConstructor<UanRoutingPCDBR>()
				.AddAttribute("NodeID", "The id of the node",
							UintegerValue(),
							MakeUintegerAccessor (&UanRoutingPCDBR::m_nodeId),
							MakeUintegerChecker<uint16_t> ())
				.AddAttribute("Depth", "The depth of the node",
						UintegerValue (),
							MakeUintegerAccessor (&UanRoutingPCDBR::m_depth),
							MakeUintegerChecker<uint16_t> ())
				;
		return tid;
}

void
UanRoutingPCDBR::SetTxPower(uint16_t txPower)
{
	NS_LOG_INFO("Node " << m_nodeId << " set txPower to " << txPower);
	m_txPower = txPower;
	m_device->GetPhy()->SetTxPowerDb(txPower);
}

void
UanRoutingPCDBR::Start()
{
	m_nodeId = GetNetDevice()->GetNode()->GetId();
	Ptr<MobilityModel> mobilitymodel =  this->GetNetDevice()->GetNode()->GetObject<MobilityModel> ();
	m_depth = mobilitymodel->GetPosition().z;
	NS_LOG_INFO("Node " << m_nodeId << " start");
}

void
UanRoutingPCDBR::SendRequire()
{
	NS_LOG_INFO("Node " << m_nodeId << " send require");
	Ptr<Packet> pkt = Create<Packet>();
	PCDBRRequireHeader reqh;
	reqh.SetNodeId(m_nodeId);
	reqh.SetSenderDepth(m_depth);
	reqh.SetDeltaDepth(m_deltaDepth);
	SetTxPower(60);
	reqh.SetSendPower(m_txPower);
	pkt->AddHeader(reqh);
	PCDBRTypeHeader th;
	th.SetMessageType(PCDBRTypeHeader::REQUIRE);
	pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress::GetBroadcast());
	uh.SetDAddr(UanAddress::GetBroadcast());
	pkt->AddHeader(uh);
	m_sendRequire = true;
	UanRoutingPCDBR::sendRequireNum++;
	SendDown(pkt, UanAddress::GetBroadcast(), Seconds(0));
	Simulator::Schedule(Seconds(m_responseWT), &UanRoutingPCDBR::WaitRespTO, this);
}

void
UanRoutingPCDBR::SendResponse(uint16_t reqNodeId, double reqDepth, double reqTxPower)
{
	NS_LOG_INFO("Node " << m_nodeId << "(" << m_depth << "m)" << " send response to " << reqNodeId);
	Ptr<Packet> pkt = Create<Packet>();
	PCDBRResponseHeader resh;
	resh.SetNodeId(m_nodeId);
	resh.SetDestId(reqNodeId);
	resh.SetRecvPower(m_rxPower);
	resh.SetRecverDepth(m_depth);
	resh.SetReEnergy(m_reEnergy);
	pkt->AddHeader(resh);
	PCDBRTypeHeader th;
	th.SetMessageType(PCDBRTypeHeader::RESPONSE);
	pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress(reqNodeId));
	uh.SetDAddr(UanAddress(reqNodeId));
	pkt->AddHeader(uh);
	UanRoutingPCDBR::sendResponseNum++;

	double deltaDepth = reqDepth - m_depth;
	double waitTime = 10 * (1-deltaDepth/(double)500) * (1-deltaDepth/(double)500);

	EventId eventId = Simulator::Schedule(Seconds(waitTime), &UanRoutingPCDBR::SendDown, this, pkt, UanAddress((uint8_t)reqNodeId), Seconds(0));
	m_requiresToBeReply.insert(pair<uint16_t, EventId>(reqNodeId, eventId));
}

bool
UanRoutingPCDBR::Recv(Ptr<Packet> p, const UanAddress &dest)
{
	NS_LOG_INFO("Node " << m_nodeId << " receive packet");
	NS_ASSERT(p != 0);

	Ptr<Packet> pkt = p->Copy();

	UanRoutingHeader routingHeader;
	pkt->RemoveHeader(routingHeader);

	NS_LOG_INFO("Packet source is " << routingHeader.GetSAddr());
	NS_LOG_INFO("Packet destination is " << routingHeader.GetDAddr());
	NS_LOG_INFO("Packet NextHop is " << routingHeader.GetNextHop());

	if (routingHeader.GetNextHop() == GetNetDevice()->GetAddress() ||
			routingHeader.GetNextHop() == UanAddress::ConvertFrom(GetNetDevice()->GetAddress()).GetBroadcast())
	{
		NS_LOG_INFO("This packet is delivered to Node " << m_nodeId);

		PCDBRTypeHeader typeHeader;
		p->RemoveHeader(typeHeader);

		if(typeHeader.GetMessageType() == PCDBRTypeHeader::REQUIRE)
		{
			UanRoutingPCDBR::recvRequireNum++;
			NS_LOG_INFO("Node(" << m_depth << "m) " << m_nodeId << " received a require packet.");
			m_rxPower = m_device->GetPhy()->GetRxQueue().front();
			NS_LOG_INFO("rxQueueLength: " << m_device->GetPhy()->GetRxQueue().size());
			NS_LOG_INFO("the rxPower of the packet in Node " << m_nodeId << "is" << m_rxPower);
			m_device->GetPhy()->GetRxQueue().pop_front();
			HandleRecvRequire(p);
			return true;
		}else if(typeHeader.GetMessageType() == PCDBRTypeHeader::RESPONSE)
		{
			UanRoutingPCDBR::recvResponseNum++;
			NS_LOG_INFO("Node " << m_nodeId << " received a response packet.");
			m_device->GetPhy()->GetRxQueue().pop_front();
			HandleRecvResponse(p);
			return true;
		}else if(typeHeader.GetMessageType() == PCDBRTypeHeader::DATA)
		{
			UanRoutingPCDBR::recvDataNum++;
			NS_LOG_INFO("Node " << m_nodeId << " received a data packet.");
			m_device->GetPhy()->GetRxQueue().pop_front();
			HandleRecvData(p);
			return true;
		}else{
			return true;
		}
	}else
	{
		return true;
	}
}

void
UanRoutingPCDBR::HandleRecvRequire(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received require");
	PCDBRRequireHeader requireHeader;
	p->RemoveHeader(requireHeader);
	uint8_t reqNodeId = requireHeader.GetNodeId();
	double reqDepth = requireHeader.GetSenderDepth();
	double reqDeltaDepth = requireHeader.GetDeltaDepth();
	double reqTxPower = requireHeader.GetSendPower();
	if(reqDepth - m_depth >= reqDeltaDepth)
	{
		SetTxPower(reqTxPower);
		SendResponse(reqNodeId, reqDepth, reqTxPower);
	}
}

void
UanRoutingPCDBR::HandleRecvResponse(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received response");
	PCDBRResponseHeader responseHeader;
	p->RemoveHeader(responseHeader);
	if(responseHeader.GetDestId() == m_nodeId)
	{
		uint8_t resNodeId = responseHeader.GetNodeId();
		double resDepth = responseHeader.GetRecverDepth();
		double resRxPower = responseHeader.GetRecvPower();
		//double resReEnergy = responseHeader.GetReEnergy();

		PCDBRPri pr;
		pr.nodeId = resNodeId;
		pr.deltaPower = m_txPower - resRxPower;
		pr.depth = resDepth;
		m_pri.push_back(pr);
		return;
	}else
	{
		std::map<uint16_t, EventId>::iterator it = m_requiresToBeReply.find(responseHeader.GetDestId());
		if (it != m_requiresToBeReply.end()) {
			EventId eventId = it->second;
			eventId.Cancel();
			m_requiresToBeReply.erase(it);
		}
	}
}

void
UanRoutingPCDBR::HandleRecvData(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received data");
	Ptr<Packet> pkt = p->Copy();
	if(m_depth == 0)
	{
		UanRoutingPCDBR::sinkRecvDataNum++;
		//uint16_t packetId = dataHeader.GetPacketId();
		m_pkts.push_back(pkt);
		NS_LOG_INFO(this << "sink receives " << UanRoutingPCDBR::sinkRecvDataNum << "th packet.");
		return;
	}
	DataHeader dataHeader;
	pkt->PeekHeader(dataHeader);
	NS_LOG_INFO(dataHeader.GetNextId() << " m_nodeId " << m_nodeId << " m_depth " << m_depth);

	if(dataHeader.GetNextId() == m_nodeId)
	{
		if(m_sendRequire)
		{
			m_pkts.push_back(pkt);
		}else{
			m_pkt = pkt;
			SendRequire();
		}
		return;
	}
}

void
UanRoutingPCDBR::WaitRespTO()
{
	NS_LOG_INFO("Node " << m_nodeId << " wait response time out");
	PCDBRDataHeader dataHeader;
	m_pkt->PeekHeader(dataHeader);
	if(m_pri.size() == 0){
		NS_LOG_INFO("Node: "<< m_nodeId << " can't forward the packet (sender : " << dataHeader.getSenderId()
					<< " packetId: " << dataHeader.getPacketId() << ")");
		return;
	}
	PCDBRPri maxPri = m_pri[0];
	for(uint32_t i = 0; i < m_pri.size(); i++)
	{
		NS_LOG_INFO("candidate: " << (int)m_pri[i].nodeId << " deltaPower: " << m_pri[i].deltaPower << " depth: " << m_pri[i].depth);
		if(m_pri[i].depth < maxPri.depth)
		{
			maxPri = m_pri[i];
		}
	}
	SendData(maxPri);
}

void
UanRoutingPCDBR::SendData(PCDBRPri maxPri)
{
	NS_LOG_INFO("Node " << m_nodeId << " send data");
	PCDBRDataHeader dataHeader;
	m_pkt->RemoveHeader(dataHeader);
	dataHeader.setNextId(maxPri.nodeId);
	m_pkt->AddHeader(dataHeader);
	PCDBRTypeHeader th;
	th.SetMessageType(PCDBRTypeHeader::DATA);
	m_pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress(maxPri.nodeId));
	uh.SetDAddr(UanAddress(maxPri.nodeId));
	m_pkt->AddHeader(uh);
	SetTxPower(maxPri.deltaPower+3);
	//SetTxPower(150);
	UanRoutingPCDBR::sendDataNum++;
	NS_LOG_INFO("Node " << m_nodeId << " send data to node " << int(maxPri.nodeId));
	SendDown(m_pkt, UanAddress((uint8_t)maxPri.nodeId), Seconds(0));
	m_sendRequire = false;
	m_pkt = NULL;
	if(m_pkts.size() != 0){
		m_pkt = m_pkts.front();
		m_pkts.pop_front();
		HandleRecvData(m_pkt);
	}
}

void
UanRoutingPCDBR::SendData(uint32_t nodeId, uint32_t pktNum)
{
	NS_LOG_INFO("Node " << m_nodeId << " send data as source, depth is " << m_depth << "m");
	Ptr<Packet> p = Create<Packet>();
	PCDBRDataHeader dataHeader;
	dataHeader.setSenderId(nodeId);
	dataHeader.setNextId(nodeId);
	dataHeader.setPacketId(pktNum);
	dataHeader.setHop(1);
	p->AddHeader(dataHeader);
	NS_LOG_INFO("SenderId: " << dataHeader.getSenderId() << " NextId: " << dataHeader.getNextId()
			<< " PacketId: " << dataHeader.getPacketId());
	HandleRecvData(p);
}

void
UanRoutingPCDBR::SetUanIp (Ptr<UanIp> uanip)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_FUNCTION (this << uanip);
	m_uanip = uanip;
	Simulator::ScheduleNow (&UanRoutingPCDBR::Start,this);
}
