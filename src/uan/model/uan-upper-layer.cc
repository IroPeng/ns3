/*
 * uan-upper-layer.cc
 *
 *  Created on: Dec 6, 2017
 *      Author: arcca
 */
#include "ns3/assert.h"
#include "ns3/node.h"
#include "ns3/boolean.h"
#include "uan-upper-layer.h"
#include "ns3/log.h"

namespace ns3 {
NS_LOG_COMPONENT_DEFINE("UanIp");

NS_OBJECT_ENSURE_REGISTERED (UanIp);

TypeId
UanIp::GetTypeId(void)
{
	static TypeId tid = TypeId("ns3::UanIp")
			.SetParent<Object> ()
			.SetGroupName("Uan")
			;
	return tid;
}//end gettypeid

UanIp::UanIp ()
{
	NS_LOG_FUNCTION (this);
}//end uanip

UanIp::~UanIp()
{
	NS_LOG_FUNCTION (this);
}//end ~uanip

}//end namespace ns3



