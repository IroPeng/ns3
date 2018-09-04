/*
 * PCDBRExperiment.h
 *
 *  Created on: 2018年9月4日
 *      Author: wjj
 */

#ifndef SCRATCH_PCDBREXPERIMENT_H_
#define SCRATCH_PCDBREXPERIMENT_H_

#include "ns3/uan-module.h"
#include "ns3/stats-module.h"
#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/energy-module.h"
#include "ns3/aodv-module.h"
#include "ns3/wifi-module.h"
#include "ns3/tag.h"
#include "ns3/gnuplot.h"
#include "ns3/traffic-control-module.h"
#include "ns3/netanim-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <cmath>
#include <sstream>
#include <ostream>


using namespace std;
using namespace ns3;

class PCDBRExperiment
{
public:
	PCDBRExperiment();
	void Run();

	void PosConfigure();
	void CreateNodes();
	void CreateDevices();
	void NameNodes(NodeContainer &nodeContainer, string prefix);
	void InstallInternetStack();
	void Start(Ptr<UanRoutingPCDBR> routing, uint32_t nodeId, uint32_t pktNum);
public:
	int m_sinkNum;
	int m_nodeNum;
	int m_bNodeNum;
	int m_pktNum;
	double m_boundary;
	double m_reqTxPower;
	double m_moduleDataRate;
	double m_simTime;

	string m_asciitracefile;

	Ptr<UniformRandomVariable> m_urv;
	Ptr<ExponentialRandomVariable> m_erv;

	NodeContainer sinks;
	NodeContainer nodes;
	NodeContainer bNodes;
	NetDeviceContainer sinksDevices;
	NetDeviceContainer nodesDevices;
	NetDeviceContainer bNodesDevices;

	Ptr<ListPositionAllocator> sinksPos = CreateObject<ListPositionAllocator>();
	Ptr<ListPositionAllocator> nodesPos = CreateObject<ListPositionAllocator>();
	Ptr<ListPositionAllocator> bNodesPos = CreateObject<ListPositionAllocator>();
};

#endif /* SCRATCH_PCDBREXPERIMENT_H_ */
