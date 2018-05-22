/*
 * uan-upper-layer.h
 *
 *  Created on: Dec 6, 2017
 *      Author: arcca
 */

#ifndef UAN_UPPER_LAYER_H
#define UAN_UPPER_LAYER_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/socket.h"
#include "ns3/callback.h"
#include "ns3/uan-address.h"
#include "ns3/uan-net-device.h"
#include "uan-routing-test.h"
#include "uan-routing-header.h"

namespace ns3{
class Node;
class Packet;
class UanRouting;
//bried uan layer 3 protocol
//used to acces to the routing protocol
class UanIp: public Object{
public:
	static TypeId GetTypeId (void);
	UanIp ();
	virtual ~UanIp();
	virtual void SetRoutingProtocol (Ptr<UanRouting> routingProtocol) = 0;
	virtual Ptr<UanRouting> GetRoutingProtocol (void) const = 0;
	virtual uint32_t AddInterface (Ptr<UanNetDevice> device) = 0;
	virtual uint32_t GetNInterfaces (void) const = 0;
	virtual Ptr<UanNetDevice> GetNetDevice (uint32_t interface) = 0;
	virtual bool Receive(Ptr<UanNetDevice> netdevice, Ptr<const Packet> packet , uint16_t proto, const UanAddress & address) = 0;
};

}//end namespace ns3

#endif /* UAN_UPPER_LAYER_H_ */
