/*
 * uan-l3-protocol.cc
 *
 *  Created on: Dec 7, 2017
 *      Author: arcca
 */
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include "uan-l3-protocol.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UanL3Protocol");
//const uint16_t UanL3Protocol::PORT_NUMBER;

NS_OBJECT_ENSURE_REGISTERED (UanL3Protocol);

TypeId
UanL3Protocol::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::UanL3Protocol")
			.SetParent<UanIp>()
			.SetGroupName("Uan")
			.AddConstructor<UanL3Protocol> ()
			;
	return tid;
}//end gettypeid

UanL3Protocol::UanL3Protocol()
{
  NS_LOG_FUNCTION (this);
}//end function UanL3Protocol

UanL3Protocol::~UanL3Protocol ()
{
  NS_LOG_FUNCTION (this);
}//end fucntion UanL3Protocol

void
UanL3Protocol::SetNode (Ptr<Node> node)
{
	NS_LOG_FUNCTION(this<<node);
	m_node = node;
	//whether need loopback device?
	//need further discussion
}//end function setnode

void
UanL3Protocol::SetRoutingProtocol (Ptr<UanRouting> routingProtocol)
{
	NS_LOG_FUNCTION (this << routingProtocol);
	m_routingProtocol = routingProtocol;
	//m_routingProtocol->SetUanIp (this);
}//end function setroutingprotocol


Ptr<UanRouting>
UanL3Protocol::GetRoutingProtocol (void) const
{
	NS_LOG_FUNCTION (this);
	return m_routingProtocol;
}//end function getroutingprotocol

void
UanL3Protocol::DoDispose()
{
	NS_LOG_FUNCTION(this);
	m_node = 0;
	m_routingProtocol = 0;
	m_uandevicelist.clear();
	m_uandevice_interfaceContainer.clear();

}//end fucntion dodispose

uint32_t
UanL3Protocol::AddInterface(Ptr<UanNetDevice> device)
{
	NS_LOG_FUNCTION(this);
	NS_ASSERT (m_node != 0);
	uint32_t index = m_uandevicelist.size();
	m_uandevicelist.push_back(device);
	m_uandevice_interfaceContainer[device] = index;
	return index;
}//end addinterface

uint32_t
UanL3Protocol::GetNInterfaces(void) const
{
	NS_LOG_FUNCTION(this);
	return m_uandevicelist.size();
}//end getninterfaces

Ptr<UanNetDevice>
UanL3Protocol::GetNetDevice(uint32_t interface)
{
	NS_LOG_FUNCTION(this);
	if (interface < m_uandevicelist.size())
	{
		return m_uandevicelist[interface];
	}
	return 0;
}//end getnetdevice

bool
UanL3Protocol::Receive(Ptr<UanNetDevice> netdevice, Ptr<const Packet> packet , uint16_t proto, const UanAddress & address)
{
	Ptr<Packet> copy = packet->Copy();
	NS_LOG_FUNCTION(this<<netdevice<<copy<<proto<<address);
	NS_ASSERT (copy != 0);
	m_routingProtocol->Recv(copy,address);
	return true;
}//end receive

}//end namespace ns3




