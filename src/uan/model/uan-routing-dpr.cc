/*
 * uan-routing-dpr.cc
 *
 *  Created on: 2017年11月28日
 *      Author: wjj
 */

#include "uan-routing-dpr.h"
#include "ns3/log.h"

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UanRoutingDPR");
NS_OBJECT_ENSURE_REGISTERED(UanRoutingDPR);

uint16_t UanRoutingDPR::sendDataNum = 0;
uint16_t UanRoutingDPR::recvDataNum = 0;
uint16_t UanRoutingDPR::sendRequireNum = 0;
uint16_t UanRoutingDPR::recvRequireNum = 0;
uint16_t UanRoutingDPR::sendResponseNum = 0;
uint16_t UanRoutingDPR::recvResponseNum = 0;
uint16_t UanRoutingDPR::sinkRecvDataNum = 0;

UanRoutingDPR::UanRoutingDPR()
	:m_nodeId(0),
	 m_depth(0),
	 m_deltaDepth(300),
	 m_txPower(195),
	 m_rxPower(0),
	 m_reEnergy(0),
	 m_sendRequire(false),
	 m_responseWT(60),
	 m_pri(0),
	 m_pkt(NULL),
	 m_pkts(0)
{
	NS_LOG_FUNCTION(this);
}

UanRoutingDPR::~UanRoutingDPR()
{
	NS_LOG_FUNCTION(this);
}

TypeId
UanRoutingDPR::GetTypeId()
{
	static TypeId tid = TypeId("ns3::UanRoutingDPR")
		.SetParent<UanRouting>()
		.AddConstructor<UanRoutingDPR>()
		.AddAttribute("NodeID", "The id of the node",
					UintegerValue(),
					MakeUintegerAccessor (&UanRoutingDPR::m_nodeId),
					MakeUintegerChecker<uint16_t> ())
		.AddAttribute("Depth", "The depth of the node",
					DoubleValue (),
				    MakeDoubleAccessor (&UanRoutingDPR::m_depth),
				    MakeDoubleChecker<double> ())
		.AddAttribute("TxPower", "The TxPower of the node",
					DoubleValue (),
					MakeDoubleAccessor (&UanRoutingDPR::m_txPower),
					MakeDoubleChecker<double> ())
		;
	return tid;
}

void
UanRoutingDPR::SetTxPower(double txPower)
{
	NS_LOG_INFO("Node " << m_nodeId << " set txPower to " << txPower);
	m_txPower = txPower;
	m_device->GetPhy()->SetTxPowerDb(txPower);
}

void
UanRoutingDPR::Start()
{
	m_nodeId = GetNetDevice()->GetNode()->GetId();
	Ptr<MobilityModel> mobilitymodel =  this->GetNetDevice()->GetNode()->GetObject<MobilityModel> ();
	m_depth = mobilitymodel->GetPosition().z;
	NS_LOG_INFO("Node " << m_nodeId << " start");
}

void
UanRoutingDPR::SendRequire()
{
	NS_LOG_INFO("Node " << m_nodeId << " send require");
	Ptr<Packet> pkt = Create<Packet>();
	RequireHeader reqh;
	reqh.SetNodeId(m_nodeId);
	reqh.SetSenderDepth(m_depth);
	reqh.SetDeltaDepth(m_deltaDepth);
	SetTxPower(100);
	reqh.SetSendPower(m_txPower);
	pkt->AddHeader(reqh);
	TypeHeader th;
	th.SetMessageType(TypeHeader::REQUIRE);
	pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress::GetBroadcast());
	uh.SetDAddr(UanAddress::GetBroadcast());
	pkt->AddHeader(uh);
	m_sendRequire = true;
	UanRoutingDPR::sendRequireNum++;
	SendDown(pkt, UanAddress::GetBroadcast(), Seconds(0));
	Simulator::Schedule(Seconds(m_responseWT), &UanRoutingDPR::WaitRespTO, this);
}

void
UanRoutingDPR::SendResponse(uint16_t destid)
{
	NS_LOG_INFO("Node " << m_nodeId << "(" << m_depth << "m)" << " send response to " << destid);
	Ptr<Packet> pkt = Create<Packet>();
	ResponseHeader resh;
	resh.SetNodeId(m_nodeId);
	resh.SetDestId(destid);
	resh.SetRecvPower(m_rxPower);
	resh.SetRecverDepth(m_depth);
	resh.SetReEnergy(m_reEnergy);
	pkt->AddHeader(resh);
	TypeHeader th;
	th.SetMessageType(TypeHeader::RESPONSE);
	pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress(destid));
	uh.SetDAddr(UanAddress(destid));
	pkt->AddHeader(uh);
	UanRoutingDPR::sendResponseNum++;
	SendDown(pkt, UanAddress((uint8_t)destid), Seconds(0));
}

bool
UanRoutingDPR::Recv(Ptr<Packet> p, const UanAddress &dest)
{
	NS_LOG_INFO("Node " << m_nodeId << " receive packet");
	NS_ASSERT(p != 0);

	UanRoutingHeader routingHeader;
	p->RemoveHeader(routingHeader);

	NS_LOG_INFO("Packet source is " << routingHeader.GetSAddr());
	NS_LOG_INFO("Packet destination is " << routingHeader.GetDAddr());
	NS_LOG_INFO("Packet NextHop is " << routingHeader.GetNextHop());

	if(routingHeader.GetNextHop() == GetNetDevice()->GetAddress() ||
			routingHeader.GetNextHop() == UanAddress::ConvertFrom(GetNetDevice()->GetAddress()).GetBroadcast())
	{
		NS_LOG_INFO("This packet is delivered to Node " << m_nodeId);

		TypeHeader typeHeader;
		p->RemoveHeader(typeHeader);

		if(typeHeader.GetMessageType() == TypeHeader::REQUIRE)
		{
			UanRoutingDPR::recvRequireNum++;
			NS_LOG_INFO("Node " << m_nodeId << " received a require packet.");
			m_rxPower = m_device->GetPhy()->GetRxQueue().front();
			NS_LOG_INFO("rxQueueLength: " << m_device->GetPhy()->GetRxQueue().size());
			NS_LOG_INFO("the rxPower of the packet in Node " << m_nodeId << "is" << m_rxPower);
			m_device->GetPhy()->GetRxQueue().pop_front();
			HandleRecvRequire(p);
			return true;
		}else if(typeHeader.GetMessageType() == TypeHeader::RESPONSE)
		{
			UanRoutingDPR::recvResponseNum++;
			NS_LOG_INFO("Node " << m_nodeId << " received a response packet.");
			m_device->GetPhy()->GetRxQueue().pop_front();
			HandleRecvResponse(p);
			return true;
		}else if(typeHeader.GetMessageType() == TypeHeader::DATA)
		{
			UanRoutingDPR::recvDataNum++;
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
UanRoutingDPR::HandleRecvRequire(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received require");
	RequireHeader requireHeader;
	p->RemoveHeader(requireHeader);
	uint8_t reqNodeId = requireHeader.GetNodeId();
	double reqDepth = requireHeader.GetSenderDepth();
	double reqDeltaDepth = requireHeader.GetDeltaDepth();
	double reqTxPower = requireHeader.GetSendPower();
	if(reqDepth - m_depth >= reqDeltaDepth)
	{
		SetTxPower(reqTxPower);
		SendResponse(reqNodeId);
	}
}

void
UanRoutingDPR::HandleRecvResponse(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received response");
	ResponseHeader responseHeader;
	p->RemoveHeader(responseHeader);
	if(responseHeader.GetDestId() == m_nodeId)
	{
		uint8_t resNodeId = responseHeader.GetNodeId();
		double resDepth = responseHeader.GetRecverDepth();
		double resRxPower = responseHeader.GetRecvPower();
		//double resReEnergy = responseHeader.GetReEnergy();

		double pri = (m_depth - resDepth)/(m_txPower - resRxPower);
		Pri pr;
		pr.priority = pri;
		pr.nodeId = resNodeId;
		pr.deltaPower = m_txPower - resRxPower;
		m_pri.push_back(pr);
		return;
	}else
	{
		return;
	}
}

void
UanRoutingDPR::HandleRecvData(Ptr<Packet> p)
{
	NS_LOG_INFO("Node " << m_nodeId << " handle received data");
	Ptr<Packet> pkt = p->Copy();
	if(m_depth == 0)
	{
		UanRoutingDPR::sinkRecvDataNum++;
		//uint16_t packetId = dataHeader.GetPacketId();
		m_pkts.push_back(pkt);
		NS_LOG_INFO(this << "sink receives " << UanRoutingDPR::sinkRecvDataNum << "th packet.");
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
UanRoutingDPR::WaitRespTO()
{
	NS_LOG_INFO("Node " << m_nodeId << " wait response time out");
	DataHeader dataHeader;
	m_pkt->PeekHeader(dataHeader);
	if(m_pri.size() == 0){
		NS_LOG_INFO("Node: "<< m_nodeId << " can't forward the packet (src : " << dataHeader.GetSrcId()
					<< " packetId: " << dataHeader.GetPacketId() << ")");
		return;
	}
	Pri maxPri = m_pri[0];
	for(uint32_t i = 0; i < m_pri.size(); i++)
	{
		NS_LOG_INFO("candidate: " << (int)m_pri[i].nodeId << " deltaPower: " << m_pri[i].deltaPower << " priority: " << m_pri[i].priority);
		if(m_pri[i].priority > maxPri.priority)
		{
			maxPri = m_pri[i];
		}
	}
	SendData(maxPri);
}

void
UanRoutingDPR::SendData(Pri maxPri)
{
	NS_LOG_INFO("Node " << m_nodeId << " send data");
	DataHeader dataHeader;
	m_pkt->RemoveHeader(dataHeader);
	dataHeader.SetNextId(maxPri.nodeId);
	m_pkt->AddHeader(dataHeader);
	TypeHeader th;
	th.SetMessageType(TypeHeader::DATA);
	m_pkt->AddHeader(th);
	UanRoutingHeader uh;
	uh.SetSAddr(UanAddress(m_nodeId));
	uh.SetNextHop(UanAddress(maxPri.nodeId));
	uh.SetDAddr(UanAddress(maxPri.nodeId));
	m_pkt->AddHeader(uh);
	SetTxPower(maxPri.deltaPower + 50);
	//SetTxPower(150);
	UanRoutingDPR::sendDataNum++;
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
UanRoutingDPR::SendData(uint32_t nodeId, uint32_t pktNum)
{
	NS_LOG_INFO("Node " << m_nodeId << " send data as source, depth is " << m_depth << "m");
	Ptr<Packet> p = Create<Packet>();
	DataHeader dataHeader;
	dataHeader.SetSrcId(nodeId);
	dataHeader.SetNextId(nodeId);
	dataHeader.SetPacketId(pktNum);
	p->AddHeader(dataHeader);
	NS_LOG_INFO("SrcId: " << dataHeader.GetSrcId() << " NextId: " << dataHeader.GetNextId()
			<< " PacketId: " << dataHeader.GetPacketId());
	HandleRecvData(p);
}
void
UanRoutingDPR::SetUanIp (Ptr<UanIp> uanip)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_FUNCTION (this << uanip);
	m_uanip = uanip;
	Simulator::ScheduleNow (&UanRoutingDPR::Start,this);
}

