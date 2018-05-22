/*
 * uan-l3-protocol-helper.cc
 *
 *  Created on: Dec 10, 2017
 *      Author: arcca
 */
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/config.h"

#include "uan-l3-protocol-helper.h"

#include <sstream>
#include <string>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("UanL3Helper");

UanL3Helper::UanL3Helper()
{
}//end UanL3Helper

UanL3Helper::~UanL3Helper ()
{

}//end ~UanL3Helper

void
UanL3Helper::SetRouting (std::string type,
                                              std::string n0, const AttributeValue &v0,
                                              std::string n1, const AttributeValue &v1,
                                              std::string n2, const AttributeValue &v2,
                                              std::string n3, const AttributeValue &v3,
                                              std::string n4, const AttributeValue &v4,
                                              std::string n5, const AttributeValue &v5,
                                              std::string n6, const AttributeValue &v6,
                                              std::string n7, const AttributeValue &v7)
{
	NS_LOG_FUNCTION(this);
	ObjectFactory factory;
	factory.SetTypeId (type);
	factory.Set (n0,v0);
	factory.Set (n1,v1);
	factory.Set (n2,v2);
	factory.Set (n3,v3);
	factory.Set (n4,v4);
	factory.Set (n5,v5);
	factory.Set (n6,v6);
	factory.Set (n7,v7);
	m_routing = factory;
}//end function setrouting

void
UanL3Helper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
	NS_LOG_FUNCTION("CreateAndAggregateObjectFromTypeId"<<""<<node->GetId()<<" "<<typeId<<".");
	ObjectFactory factory;
	factory.SetTypeId (typeId);
	Ptr<Object> protocol = factory.Create <Object> ();
	node->AggregateObject (protocol);
}//end function CreateAndAggregateObjectFromTypeId

void
UanL3Helper::ConnectRouting(Ptr<Node> node, Ptr<UanRouting> routingProtocol)
{
	NS_LOG_FUNCTION(this);
    Ptr<UanIp> uanip = node->GetObject<UanIp> ();
	routingProtocol->SetUanIp(uanip);

}//end function connectrouting

void
UanL3Helper::Install(Ptr<Node> node) const
{
	NS_LOG_FUNCTION(this);
    if (node->GetObject<UanIp> () != 0)
      {
        NS_FATAL_ERROR ("UanL3Helper::Install (): Aggregating "
                        "an InternetStack to a node with an existing UanIp object");
        return;
      }
    CreateAndAggregateObjectFromTypeId (node, "ns3::UanL3Protocol");
    Ptr<UanIp> uanip = node->GetObject<UanIp> ();
    Ptr<UanRouting> routing = m_routing.Create<UanRouting>();
    uanip->SetRoutingProtocol(routing);
    //check and attach the recv to mac upforward
    NS_ASSERT(node->GetDevice(0)->GetObject<UanNetDevice>()->GetMac());
    Ptr<UanMac> mac = node->GetDevice(0)->GetObject<UanNetDevice>()->GetMac();
    //mac->SetForwardUpCb(MakeCallback (&ns3::UanRouting::MacForwardUpCB, routing));
    Ptr<UanNetDevice> uandevice = node->GetDevice(0)->GetObject<UanNetDevice>();
/*    Ptr<UanL3Protocol> uanl3 = node->GetObject<UanL3Protocol>();

    uandevice->SetForwardL3Callback(MakeCallback (&ns3::UanL3Protocol::Receive, uanl3));*/
    uandevice->SetForwardL3Callback(MakeCallback (&ns3::UanIp::Receive, uanip));
    node->AggregateObject (routing);//m_routing aggregate into node
	routing->SetUanIp(uanip);
	routing->SetNetDevice(uandevice);
	NS_LOG_UNCOND(uandevice->GetAddress());
	routing->SetMac(mac);
}//end function install

void
UanL3Helper::Install(NodeContainer c)const
{
	NS_LOG_FUNCTION(this);
	for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
	{
	  Install (*i);
	}
}//end fucntion install



}//end namespace ns3
