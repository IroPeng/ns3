/*
 * uan-l3-protocol.h
 *
 *  Created on: Dec 6, 2017
 *      Author: arcca
 */

#ifndef UAN_L3_PROTOCOL_H_
#define UAN_L3_PROTOCOL_H_

#include <stdint.h>
#include <vector>
#include "ns3/object.h"
#include "ns3/socket.h"
#include "ns3/callback.h"
#include "ns3/uan-address.h"
#include "ns3/uan-net-device.h"
#include "uan-routing-test.h"
#include "uan-routing-header.h"
#include "uan-upper-layer.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"

namespace ns3 {
class UanL3Protocol : public UanIp
{
public:
	static TypeId GetTypeId (void);
	static const uint16_t PORT_NUMBER;

	UanL3Protocol ();
	virtual ~UanL3Protocol ();

	void SetNode (Ptr<Node> node);
	void SetRoutingProtocol (Ptr<UanRouting> routingProtocol);
	Ptr<UanRouting> GetRoutingProtocol (void) const;
	//Need further exploration about interface
	uint32_t AddInterface (Ptr<UanNetDevice> device);
	uint32_t GetNInterfaces (void) const;
	Ptr<UanNetDevice> GetNetDevice (uint32_t interface);
	//connect with uan netdevice
	bool Receive(Ptr<UanNetDevice> netdevice, Ptr<const Packet> packet , uint16_t proto, const UanAddress & address);
	//we assume that tht uan netdeive only got one interface and one address
	//which means there is no need to implent some class similaried to ipv4interface
protected:
	virtual void DoDispose (void);

	typedef std::vector<Ptr<UanNetDevice>> UanNetDeviceList;
	typedef std::map<Ptr<const UanNetDevice>, uint32_t> UanDevice_InterfaceContainer;

	UanDevice_InterfaceContainer m_uandevice_interfaceContainer;//<Container of UanNetdevice / Interface index associations
	UanNetDeviceList m_uandevicelist;

	std::map<std::pair<uint64_t, uint8_t>, uint16_t> m_identification; //!< Identification (for each {src, dst, proto} tuple)

	Ptr<Node> m_node;//<Node attached to stack
	Ptr<UanRouting> m_routingProtocol; //!< Routing protocol associated with the stack

	//TracedCallback<const Ipv4Header &, Ptr<const Packet>, uint32_t> m_localDeliverTrace;
/*	TracedCallback<Ptr<const Packet>, Ptr<Ipv4>,  uint32_t> m_txTrace;
	TracedCallback<Ptr<const Packet>, Ptr<Ipv4>, uint32_t> m_rxTrace;*/
};
}//end namespace ns3


#endif /* UAN_L3_PROTOCOL_H */
