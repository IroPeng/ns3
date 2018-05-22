/*
 * uan-routing-header.h
 *
 *  Created on: Nov 21, 2017
 *      Author: arcca
 */

#ifndef UAN_ROUTING_HEADER_H
#define UAN_ROUTING_HEADER_H

#include "ns3/address.h"
#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/uan-address.h"
#include "ns3/simulator.h"

#include <string>
#include <iostream>

namespace ns3{

class Packet;

struct addr_t {
  UanAddress addr;
  int32_t port;    //Currently not used
};


class UanRoutingHeader : public Header
{
public:
	enum forward_dir { DOWN = 0, NONE = 1, UP = 2};
	UanRoutingHeader();
	virtual ~UanRoutingHeader();
	static TypeId GetTypeId(void);

	//inherited by Header class
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);
	virtual uint32_t GetSerializedSize(void) const;

	//Getters
	double GetTxTime();  // tx time for this packet in sec(stored as 4bytes)
	uint32_t GetHeaderSize();	// simulated packet size
	uint8_t GetDirection();/* setting all direction of pkts to be none as default */
	UanAddress GetNextHop();	// next hop for this packet
	uint16_t GetNumForwards();	// how many times this pkt was forwarded
	UanAddress GetSAddr();
	UanAddress GetDAddr();
	int32_t GetSPort();
	int32_t GetDPort();
	bool GetErrorFlag();
	uint32_t GetUId();
	double GetTimeStamp();

	//Setters
	void SetTxTime(double time);
	void SetHeaderSize(uint16_t size);
	void SetDirection(uint8_t direction);
	void SetNextHop(UanAddress nextHop);
	void SetNumForwards(uint16_t numForwards);
	void SetSAddr(UanAddress sAddr);
	void SetDAddr(UanAddress dAddr);
	void SetSPort(int32_t sPort);
	void SetDPort(int32_t dPort);
	void SetErrorFlag(bool error);
	void SetUId(uint32_t uId);
	void SetTimeStamp(double timestamp);


private:
	double m_txTime;
	uint32_t m_direction;  // direction: 0=down, 1=none, 2=up
	UanAddress m_nextHop;
	uint16_t m_numForwards;
	addr_t m_src;
	addr_t m_dst;
	uint8_t m_errorFlag;
	uint32_t m_uId;
	uint16_t m_size; //figmented size of packet...
	double m_timestamp;

};


}//end namespace ns3


#endif /* UAN_ROUTING_HEADER_H */
