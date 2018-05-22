/*
 * uan-l3-protocol-helper.h
 *
 *  Created on: Dec 10, 2017
 *      Author: arcca
 */

#ifndef UAN_L3_PROTOCOL_HELPER_H
#define UAN_L3_PROTOCOL_HELPER_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include <string>
#include "ns3/attribute.h"
#include "ns3/uan-routing-test.h"
#include "ns3/uan-upper-layer.h"

namespace ns3{
class UanL3Helper : public Object
{
public:
	UanL3Helper (void);
	virtual ~UanL3Helper (void);
	void SetRouting (std::string name,
				 std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
				 std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
				 std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
				 std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
				 std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
				 std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
				 std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
				 std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
	void Install (Ptr<Node> node) const;
	void Install (NodeContainer c) const;
	Ptr<UanRouting> GetRouting(void);
	static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);
	void ConnectRouting(Ptr<Node> node, Ptr<UanRouting> routingProtocol);
private:
	ObjectFactory m_routing;


};

}//end namespace ns3





#endif /* UAN_L3_PROTOCOL_HELPER_H */
