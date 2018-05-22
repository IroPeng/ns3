/*

 * uan-routing-test.h
 *
 *  Created on: Nov 20, 2017
 *      Author: arcca
 */

#ifndef UAN_ROUTING_TEST_H
#define UAN_ROUTING_TEST_H

#include "ns3/address.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/log.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/core-module.h"
#include "ns3/uan-net-device.h"
#include "ns3/uan-mac.h"
#include "ns3/uan-channel.h"
#include "ns3/uan-phy.h"
#include "ns3/uan-transducer.h"
#include "ns3/uan-tx-mode.h"
#include "ns3/uan-address.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include "ns3/uan-routing-header.h"

namespace ns3{
class UanIp;
class UanRouting : public Object
{
public:
	static TypeId GetTypeId(void);
	UanRouting(void);
	virtual ~UanRouting(void);

	void SetNetDevice(Ptr<UanNetDevice> device);
	void SetMac(Ptr<UanMac> mac);
	Ptr<UanNetDevice> GetNetDevice();

	virtual void MacForwardUpCB (Ptr<Packet> packet, const UanAddress &des);
	virtual bool Recv(Ptr<Packet> packet, const UanAddress &dest, uint16_t protocolNumber) = 0;	//handler not implemented
	virtual bool Recv(Ptr<Packet> packet, const UanAddress &dest) = 0;	//handler not implemented
	bool SendDown(Ptr<const Packet> p, UanAddress nextHop, Time delay);
	bool SendUp(Ptr<const Packet> p, UanAddress nextHop, Time delay);

	int SendUpPktCount() {return m_sendUpPktCount;}

	bool ForwardUp(const Ptr<Packet> p);
	void SendPacket(Ptr<Packet> p);
	bool IsDeadLoop(const Ptr<Packet> p);
	bool AmINextHop(const Ptr<Packet> p);
	bool AmIDst(const Ptr<Packet> p);
	bool AmISrc(const Ptr<Packet> p);
	Ptr<UanMac> GetMac();

	typedef Callback<bool, Ptr<Packet>, const UanAddress &> RecvCallback;

	//RecvCallback GetRecvCallback();
	typedef void (* RxCallback)(std::string path, Ptr<Packet> p);
	typedef void (* TxCallback)(std::string path, Ptr<Packet> p, Address nextHop, Time delay);

	void NotifyRx(const Ptr<Packet> p);
	void NotifyTx(const Ptr<Packet> p, UanAddress nextHop, Time delay);

	void DoDispose();
	virtual void SetUanIp (Ptr<UanIp> uanip) = 0;



	protected:
	Ptr<UanMac> m_mac;
	Ptr<UanNetDevice> m_device;
	TracedValue<Ptr<const Packet> > m_routingRxTrace;
	TracedValue<Ptr<const Packet> > m_routingTxTrace;

	int m_sendUpPktCount;


};

}//end namespace ns3


#endif /* UAN_ROUTING_TEST_H */
