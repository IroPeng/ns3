/*
 * uan-routing-header.cc
 *
 *  Created on: Nov 22, 2017
 *      Author: arcca
 */

#include "uan-routing-header.h"
#include "uan-address.h"
#include "ns3/buffer.h"
#include "ns3/log.h"
#include "ns3/address-utils.h"
#include "ns3/address.h"

#include <iostream>
#include <bitset>

using namespace std;
namespace ns3{


//Uan routing protocol
NS_LOG_COMPONENT_DEFINE("UanRoutingHeader");
NS_OBJECT_ENSURE_REGISTERED(UanRoutingHeader);

UanRoutingHeader::UanRoutingHeader(void) :
    m_txTime(0), m_direction(DOWN),
    m_numForwards(0), m_errorFlag(0), m_uId(-1),
    m_timestamp(0)
{
/*
  m_nextHop = UanAddress(-1);
  m_src.addr = UanAddress(-1);
  m_dst.addr = UanAddress(-1);
*/

  NS_LOG_FUNCTION(this);
}

UanRoutingHeader::~UanRoutingHeader(void)
{
  NS_LOG_FUNCTION(this);
}

TypeId
UanRoutingHeader::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::UanRoutingHeader")
    .SetParent<Header>()
    .SetGroupName("HEDBR")
    .AddConstructor<UanRoutingHeader>()
  ;
  return tid;
}

TypeId
UanRoutingHeader::GetInstanceTypeId(void) const
{
  return GetTypeId();
}

uint32_t
UanRoutingHeader::Deserialize(Buffer::Iterator start)
{
	NS_LOG_FUNCTION(this);
	Buffer::Iterator i = start;
	m_txTime = i.ReadNtohU64 ();
	m_size = i.ReadNtohU16();
	m_direction = i.ReadNtohU32();
	m_numForwards = i.ReadNtohU16();
	m_nextHop = (UanAddress) i.ReadU8();
	m_src.addr = (UanAddress) i.ReadU8();
	m_dst.addr = (UanAddress) i.ReadU8();
	m_errorFlag = i.ReadU8();	//wasted space due to only needing 1 bit
	m_timestamp = i.ReadNtohU64 ();
	m_uId = i.ReadNtohU32();

	uint32_t dist = i.GetDistanceFrom (start);
	NS_ASSERT (dist == GetSerializedSize ());
	return dist;

}//end deserialize

uint32_t
UanRoutingHeader::GetSerializedSize(void) const
{
	NS_LOG_FUNCTION(this);
	uint32_t size = sizeof(uint64_t)*2 + sizeof(uint8_t)*4 + sizeof(uint32_t)*2 + sizeof(uint16_t)*2;
	return size;
}//end getserializedsize

void
UanRoutingHeader::Serialize(Buffer::Iterator i) const
{
	NS_LOG_FUNCTION(this);
	//Buffer::Iterator start = i;
	i.WriteHtonU64(m_txTime);
	i.WriteHtonU16(m_size);
	i.WriteHtonU32(m_direction);
	i.WriteHtonU16(m_numForwards);
	i.WriteU8(m_nextHop.GetAsInt());
	i.WriteU8(m_src.addr.GetAsInt());
	i.WriteU8(m_dst.addr.GetAsInt());
	i.WriteU8(m_errorFlag);
	i.WriteU64(m_timestamp);
	i.WriteHtonU32(m_uId);

  //src/dst port
}//end serialize

void
UanRoutingHeader::Print(ostream &os) const
{
	NS_LOG_FUNCTION (this<< &os);
	os << " Uan routing packet header is ";
	os << " TxTime=" << m_txTime << " Size=" << m_size << " Direction=";
	switch (m_direction){
	case DOWN:  os << "DOWN"; break;
	case NONE:  os << "NONE"; break;
	case UP:    os << "UP";   break;
	}
	os << "NumForwards(packets forwarded to upper layer)=" << m_numForwards << ". \n Error=";
	if (m_errorFlag == 0) {os << "False";}
	else  {os << "True"; }

	os << " UniqueID=" << m_uId;
	os << " Timestamp=" << m_timestamp;
	os << " SenderAddr=" << m_src.addr << " DestAddr=" << m_dst.addr << " NextHop=" << m_nextHop << "\n";
}//end print

double
UanRoutingHeader::GetTxTime(void)
{
  NS_LOG_FUNCTION(this);
  return m_txTime;
}

uint32_t
UanRoutingHeader::GetHeaderSize(void)
{
  NS_LOG_FUNCTION(this);
  return m_size;
}

uint8_t
UanRoutingHeader::GetDirection(void)
{
  NS_LOG_FUNCTION(this);
  return m_direction;
}

UanAddress
UanRoutingHeader::GetNextHop(void)
{
	NS_LOG_FUNCTION(this);
	return m_nextHop;
}

uint16_t
UanRoutingHeader::GetNumForwards(void)
{
  NS_LOG_FUNCTION(this);
  return m_numForwards;
}

UanAddress
UanRoutingHeader::GetSAddr(void)
{
  NS_LOG_FUNCTION(this);
  return m_src.addr;
}

UanAddress
UanRoutingHeader::GetDAddr(void)
{
  NS_LOG_FUNCTION(this);
  return m_dst.addr;
}

int32_t
UanRoutingHeader::GetSPort()
{
  NS_LOG_FUNCTION(this);
  return (m_src.port);
}

int32_t
UanRoutingHeader::GetDPort()
{
  NS_LOG_FUNCTION(this);
  return (m_dst.port);
}

bool
UanRoutingHeader::GetErrorFlag(void)
{
  NS_LOG_FUNCTION(this);
  return m_errorFlag;
}

uint32_t
UanRoutingHeader::GetUId(void)
{
  NS_LOG_FUNCTION(this);
  return m_uId;
}

double
UanRoutingHeader::GetTimeStamp()
{
  NS_LOG_FUNCTION(this);
  return m_timestamp;
}

void
UanRoutingHeader::SetTxTime(double time)
{
  NS_LOG_FUNCTION(this);
  m_txTime = time;
}

void
UanRoutingHeader::SetHeaderSize(uint16_t size)
{
  NS_LOG_FUNCTION(this);
  m_size = size;
}

void
UanRoutingHeader::SetDirection(uint8_t direction)
{
  NS_LOG_FUNCTION(this);
  m_direction = direction;
}

void
UanRoutingHeader::SetNextHop(UanAddress nextHop)
{
  NS_LOG_FUNCTION(this);
  m_nextHop = nextHop;
}

void
UanRoutingHeader::SetNumForwards(uint16_t numForwards)
{
  NS_LOG_FUNCTION(this);
  m_numForwards = numForwards;
}

void
UanRoutingHeader::SetSAddr(UanAddress sAddr)
{
  NS_LOG_FUNCTION(this);
  m_src.addr = sAddr;
}

void
UanRoutingHeader::SetDAddr(UanAddress dAddr)
{
  NS_LOG_FUNCTION(this);
  m_dst.addr = dAddr;
}

void
UanRoutingHeader::SetSPort(int32_t sPort)
{
  NS_LOG_FUNCTION(this);
  m_src.port = sPort;
}

void
UanRoutingHeader::SetDPort(int32_t dPort)
{
  NS_LOG_FUNCTION(this);
  m_dst.port = dPort;
}

void
UanRoutingHeader::SetErrorFlag(bool error)
{
  NS_LOG_FUNCTION(this);
  m_errorFlag = error;
}

void
UanRoutingHeader::SetUId(uint32_t uId)
{
  NS_LOG_FUNCTION(this);
  m_uId = uId;
}

void
UanRoutingHeader::SetTimeStamp(double timestamp)
{
  NS_LOG_FUNCTION(this);
  m_timestamp = timestamp;
}

std::ostream &
operator<< (std::ostream & os, UanRoutingHeader const & h)
{
  h.Print (os);
  return os;
}

}//end namespace ns3
