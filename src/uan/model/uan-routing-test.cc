/*
 * uan-routing-test.cc
 *
 *  Created on: Nov 20, 2017
 *      Author: arcca
 */

#include "uan-routing-test.h"
#include "ns3/log.h"
#include "ns3/attribute.h"
#include "ns3/simulator.h"
#include "ns3/ptr.h"

//A basic routing protocol type
namespace ns3 {


NS_LOG_COMPONENT_DEFINE("UanRoutingTest");

NS_OBJECT_ENSURE_REGISTERED(UanRouting);


TypeId
UanRouting::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::UanRouting")
    .SetParent<Object>()
    .SetGroupName("Uan")
    .AddAttribute("SetNetDevice", "The net device where this routing layer resides on",
      PointerValue (),
      MakePointerAccessor (&UanRouting::m_device),MakePointerChecker<UanRouting> ())
    .AddTraceSource ("RoutingTx",
      "Trace source indicating a packet has started transmitting.",
      MakeTraceSourceAccessor (&UanRouting::m_routingTxTrace),
      "ns3::UanRouting::TxCallback")
    .AddTraceSource ("RoutingRx",
      "Trace source indicating a packet has been received.",
      MakeTraceSourceAccessor (&UanRouting::m_routingRxTrace),
      "ns3::UanRouting::RxCallback")
  ;
  return tid;
}//end TypeId

UanRouting::UanRouting() :
  m_sendUpPktCount(0)
{
  NS_LOG_FUNCTION(this);

}//end UanRouting

UanRouting::~UanRouting()
{
  NS_LOG_FUNCTION(this);
}//end ~UanRouting

void
UanRouting::SetNetDevice(Ptr<UanNetDevice> device)
{
  NS_LOG_FUNCTION(this);
  m_device = device;
}//end SetNetDevice

void
UanRouting::SetMac(Ptr<UanMac> mac)
{
  NS_LOG_FUNCTION(this << mac);
  m_mac = mac;
}//end SetMac

Ptr<UanNetDevice>
UanRouting::GetNetDevice()
{
  NS_LOG_FUNCTION(this);
  return m_device;
}//end GetNetDevice

Ptr<UanMac>
UanRouting::GetMac()
{
  NS_LOG_FUNCTION(this);
  return m_mac;
}//end GetMac

bool
UanRouting::ForwardUp(const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this << copy << " : currently a dummy sendup on nodeAddr:" <<
  UanAddress::ConvertFrom(m_device->GetAddress())<<". " );

  //Delete the header before deliver it to upper layer
  UanRoutingHeader Header;
  copy->RemoveHeader(Header);

  m_sendUpPktCount++;
  NS_LOG_INFO("There are "<<m_sendUpPktCount<<" packets have been sent up to higher layer.");
  return true;
}//end SendUp

bool
UanRouting::SendDown(Ptr<const Packet> p, UanAddress nextHop, Time delay)
{
	Ptr<Packet> copy = p->Copy();
	NS_LOG_FUNCTION(this);
	NS_LOG_FUNCTION(this<<*p<<delay);
	NS_ASSERT(copy !=NULL);
	//here add the basic routing header
	UanRoutingHeader Header;
	copy->RemoveHeader(Header);


	/*uint8_t uid = Header.GetUId();
	if(Header.GetUId() == -1)
		{
		Header.SetUId(p->GetUid());
		}*/
	Header.SetDirection(UanRoutingHeader::DOWN);
	Header.SetNextHop(nextHop);
	Header.SetSAddr(UanAddress::ConvertFrom(m_device->GetAddress()));
	copy->AddHeader(Header);
	Simulator::Schedule(delay, &UanRouting::SendPacket, this, copy);
	return true;
}//end SendDown

bool
UanRouting::SendUp(Ptr<const Packet> p, UanAddress nextHop, Time delay)
{
	Ptr<Packet> copy = p->Copy();
	NS_LOG_FUNCTION(this);
	NS_LOG_FUNCTION(this<<*p<<delay);
	NS_ASSERT(copy !=NULL);
	//here add the basic routing header
	UanRoutingHeader Header;
	copy->RemoveHeader(Header);


	/*uint8_t uid = Header.GetUId();
	if(Header.GetUId() == -1)
		{
		Header.SetUId(p->GetUid());
		}*/
	Header.SetDirection(UanRoutingHeader::UP);
	Header.SetNextHop(nextHop);
	Header.SetSAddr(UanAddress::ConvertFrom(m_device->GetAddress()));
	copy->AddHeader(Header);
	Simulator::Schedule(delay, &UanRouting::SendPacket, this, copy);
	return true;
}//end SendDown


void
UanRouting::SendPacket(Ptr<Packet> p)
{

	NS_LOG_FUNCTION(this <<*p<<" "<< m_mac);

	if (!m_mac->Enqueue(p, UanAddress::GetBroadcast(), 0))
	{
	  NS_LOG_DEBUG(this << "Mac recv error");
	}
	return;
}//end SendPacket

bool
UanRouting::IsDeadLoop(const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this<<p);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);
  UanRoutingHeader Header;
  copy->PeekHeader(Header);

  return (Header.GetSAddr()==UanAddress::ConvertFrom(m_device->GetAddress())) && (Header.GetNumForwards() > 0);
}//end IsDeadLoop

bool
UanRouting::AmISrc(const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this<<p);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);

  UanRoutingHeader Header;
  copy->PeekHeader(Header);

  return (Header.GetSAddr()==UanAddress::ConvertFrom(m_device->GetAddress())) && (Header.GetNumForwards() == 0);
}//end AmISrc

bool
UanRouting::AmIDst(const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this<<p);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);
  UanRoutingHeader Header;
  copy->PeekHeader(Header);
  return ((Header.GetDirection()==UanRoutingHeader::UP) && (Header.GetDAddr() == UanAddress::ConvertFrom(m_device->GetAddress())));
}//end AmIDst

bool
UanRouting::AmINextHop(const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this<<p);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);
  UanRoutingHeader Header;
  copy->PeekHeader(Header);
  return ((Header.GetNextHop() == UanAddress::ConvertFrom(m_device->GetAddress()))|| ( Header.GetNextHop() == UanAddress::GetBroadcast() ));
}//end AmINextHop

void
UanRouting::NotifyRx (const Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this<<p);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);
  m_routingRxTrace = copy;
  ForwardUp(p);
  NS_LOG_UNCOND(" RX " << copy->ToString());
}//end NotifyRx

void
UanRouting::NotifyTx (const Ptr<Packet> p, UanAddress nextHop, Time delay)
{
  NS_LOG_FUNCTION(this<<p<<" "<<nextHop<<" "<<delay);
  Ptr<Packet> copy = p->Copy();
  NS_LOG_FUNCTION(this<<copy);
  m_routingTxTrace = copy;
  SendDown(copy, nextHop, delay);
  NS_LOG_UNCOND(" TX " << copy->ToString());
}//end NotifyTx

void UanRouting::DoDispose()
{
  NS_LOG_FUNCTION(this);
  m_device=0;
  m_mac=0;
  Object::DoDispose();
}//end DoDispose

void
UanRouting::MacForwardUpCB(Ptr<Packet> packet, const UanAddress &dest)
{
	NS_LOG_FUNCTION(this);
	Recv(packet, dest);
}//end function macforwardcb

}//end namespace ns3

