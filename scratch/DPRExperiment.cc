/*
 * DPRExperiment.cc
 *
 *  Created on: 2017年12月13日
 *      Author: wjj
 */

#include "DPRExperiment.h"
#include <fstream>
#include <math.h>
#include "ns3/log.h"

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DPRExperiment");

DPRExperiment::DPRExperiment()
	:
	m_sinkNum(10),
	m_nodeNum(100),
	m_bNodeNum(20),
	m_pktNum(100),
	m_boundary(500),
	m_reqTxPower(190),
	m_moduleDataRate(4800),
	m_simTime(1000),
	m_asciitracefile("DPRExperiment.asc")
{
}

void
DPRExperiment::PosConfigure()
{
	NS_LOG_FUNCTION(this);
	SeedManager::SetSeed(12345);
	Ptr<UniformRandomVariable> urv = CreateObject<UniformRandomVariable>();
	for(int i = 0; i < m_sinkNum; i++)
	{
		double x = urv->GetValue(0, m_boundary);
		double y = urv->GetValue(0, m_boundary);
		sinksPos->Add(Vector(x, y, 0));
		NS_LOG_INFO("sink " << i << " position is (" << x << ", " << ", " << y << ", 0)");
	}
	for(int i = 0; i < m_nodeNum; i++)
	{
		double x = urv->GetValue(0, m_boundary);
		double y = urv->GetValue(0, m_boundary);
		double z = urv->GetValue(0, m_boundary);
		nodesPos->Add(Vector(x, y, z));
		NS_LOG_INFO("node " << i << " position is (" << x << ", " << ", " << y << ", "<< z <<")");
	}
	for(int i = 0; i < m_bNodeNum; i++)
	{
		double x = urv->GetValue(0, m_boundary);
		double y = urv->GetValue(0, m_boundary);
		bNodesPos->Add(Vector(x, y, m_boundary));
		NS_LOG_INFO("bNode " << i << " position is (" << x << ", " << ", " << y << ", "<< m_boundary <<")");
	}
	NS_LOG_DEBUG("Configure node mobility completed");
}

void
DPRExperiment::NameNodes(NodeContainer &nodeContainer, string prefix)
{
	NS_LOG_FUNCTION(this);
	for(uint32_t i = 0; i < nodeContainer.GetN(); i++)
	{
		ostringstream os;
		os << prefix << i;
		Names::Add(os.str(), nodeContainer.Get(i));
		NS_LOG_INFO("Node id "<<nodeContainer.Get(i)->GetId()<<" names is "<<os.str()<<".");
	}
}

void
DPRExperiment::CreateNodes()
{
	NS_LOG_FUNCTION(this);
	//Create nodes
	sinks.Create(m_sinkNum);
	nodes.Create(m_nodeNum);
	bNodes.Create(m_bNodeNum);

	//Name nodes
	NameNodes(sinks, "Sink-");
	NameNodes(nodes, "node-");
	NameNodes(bNodes, "bNode-");

	//install mobility model
	MobilityHelper mobility4sinks;
	mobility4sinks.SetPositionAllocator(sinksPos);
	mobility4sinks.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility4sinks.Install(sinks);

	MobilityHelper mobility4nodes;
	mobility4nodes.SetPositionAllocator(nodesPos);
	mobility4nodes.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility4nodes.Install(nodes);

	MobilityHelper mobility4bNodes;
	mobility4bNodes.SetPositionAllocator(bNodesPos);
	mobility4bNodes.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility4bNodes.Install(bNodes);
}

void
DPRExperiment::CreateDevices()
{
	NS_LOG_FUNCTION(this);
	UanHelper uanHelper;

	//Phy configure
	//Phy
	std::string perModel = "ns3::UanPhyPerGenDefault";
	std::string sinrModel = "ns3::UanPhyCalcSinrDefault";
	ObjectFactory obf;
	obf.SetTypeId (perModel);
	Ptr<UanPhyPer> per = obf.Create<UanPhyPer> ();
	obf.SetTypeId (sinrModel);
	Ptr<UanPhyCalcSinr> sinr = obf.Create<UanPhyCalcSinr> ();
	//TxMode
	UanTxMode mode;
	mode = UanTxModeFactory::CreateMode(UanTxMode::FSK, m_moduleDataRate, m_moduleDataRate, 25000,
			m_moduleDataRate, 2, "Default mode");
	UanModesList myModes;
	myModes.AppendMode(mode);
	uanHelper.SetPhy("ns3::UanPhyGen",
			"PerModel", PointerValue(per),
			"SinrModel", PointerValue(sinr),
			"SupportedModes", UanModesListValue(myModes),
			"TxPower", DoubleValue(m_reqTxPower));
	//mac
	//uanHelper.SetMac("ns3::UanMacAloha");
	//channel
	Ptr<UanPropModelThorp> prop = CreateObjectWithAttributes<UanPropModelThorp>();
	//Ptr<UanPropModelIdeal> prop = CreateObject<UanPropModelIdeal>();
	Ptr<UanChannel> channel = CreateObjectWithAttributes<UanChannel>("PropagationModel",
			PointerValue(prop));
	sinksDevices = uanHelper.Install(sinks, channel);
	nodesDevices = uanHelper.Install(nodes, channel);
	bNodesDevices = uanHelper.Install(bNodes, channel);

	//energy
	/*BasicEnergySourceHelper eh;
	eh.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(80000.0));

	EnergySourceContainer esCont4sink = eh.Install(sinks);
	AcousticModemEnergyModelHelper modemHelper4sink;
	DeviceEnergyModelContainer cont4sink = modemHelper4sink.Install(sinksDevices, esCont4sink);
	EnergySourceContainer esCont4node = eh.Install(nodes);
	AcousticModemEnergyModelHelper modemHelper4node;
	DeviceEnergyModelContainer cont4node = modemHelper4node.Install(nodesDevices, esCont4node);*/

	/*ofstream ascii(m_asciitracefile.c_str());
	uanHelper.EnableAsciiAll(ascii);*/
}

void
DPRExperiment::InstallInternetStack()
{
	NS_LOG_FUNCTION(this);

	UanL3Helper uanL3Helper;
	uanL3Helper.SetRouting("ns3::UanRoutingDPR");
	uanL3Helper.Install(sinks);
	uanL3Helper.Install(nodes);
	uanL3Helper.Install(bNodes);
}
void
DPRExperiment::Start(Ptr<UanRoutingDPR> routing, uint32_t nodeId, uint32_t pktNum)
{
	routing->SendData(nodeId, pktNum);
}

void DPRExperiment::Run()
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("Ready simulator.");
	PosConfigure();
	CreateNodes();
	CreateDevices();
	InstallInternetStack();

	SeedManager::SetRun(SeedManager::GetRun()+1);

	Ptr<UniformRandomVariable> urv = CreateObject<UniformRandomVariable> ();

	double sendTime = 0;
	for(int i = 0; i < m_pktNum; i++){
		int nodeId = urv->GetValue(0, m_bNodeNum);
		NS_LOG_INFO(nodeId);
		sendTime = sendTime + urv->GetValue(20, 30);
		NS_LOG_UNCOND("Test the nodeId "<<nodeId);
		Ptr<UanRoutingDPR> routing = bNodes.Get(nodeId)->GetObject<UanRoutingDPR>();
		Simulator::Schedule(Seconds(sendTime), &DPRExperiment::Start, this, routing, nodeId+m_sinkNum+m_nodeNum, m_pktNum);
	}

	Simulator::Run();
	Simulator::Stop (Seconds(m_simTime));


    for (uint32_t i=0; i < sinks.GetN (); i++)
      {
    	sinks.Get (i) = 0;
      }
    for (uint32_t i=0; i < nodes.GetN (); i++)
      {
    	nodes.Get (i) = 0;
      }
    for (uint32_t i=0; i < bNodes.GetN (); i++)
	 {
    	bNodes.Get (i) = 0;
	 }
    for (uint32_t i=0; i < sinksDevices.GetN (); i++)
      {
    	sinksDevices.Get (i) = 0;
      }
    for (uint32_t i=0; i < nodesDevices.GetN (); i++)
      {
    	nodesDevices.Get (i) = 0;
      }
    for (uint32_t i=0; i < bNodesDevices.GetN (); i++)
	  {
		bNodesDevices.Get (i) = 0;
	  }
	Names::Clear ();
    Simulator::Destroy ();
}

int
main (int argc, char **argv)
{
	LogComponentEnable ("DPRExperiment",LOG_LEVEL_INFO);
	LogComponentEnable ("UanRoutingDPR",LOG_LEVEL_INFO);
	LogComponentEnable ("UanPhyGen",LOG_LEVEL_INFO);
//	LogComponentEnable ("UanPhyDual",LOG_LEVEL_INFO);
	/*LogComponentEnable ("UanRoutingHeader",LOG_LEVEL_INFO);
	LogComponentEnable ("UanL3Protocol",LOG_LEVEL_ALL);
	LogComponentEnable ("UanRoutingTest",LOG_LEVEL_ALL);
	LogComponentEnable ("UanMacCw",LOG_LEVEL_ALL);*/
	/*LogComponentEnable ("UanMacAloha",LOG_LEVEL_ALL);
	LogComponentEnable ("UanIp",LOG_LEVEL_ALL);
	LogComponentEnable ("UanChannel",LOG_LEVEL_ALL);
	LogComponentEnable ("UanL3Helper",LOG_LEVEL_ALL);
	LogComponentEnable ("UanNetDevice",LOG_LEVEL_ALL);
	LogComponentEnable ("UanPhyDual",LOG_LEVEL_ALL);
	LogComponentEnable ("UanPhyGen",LOG_LEVEL_ALL);
	LogComponentEnable ("UanTransducerHd",LOG_LEVEL_ALL);
	LogComponentEnable ("UanHelper",LOG_LEVEL_ALL);
	LogComponentEnable ("UanTxMode",LOG_LEVEL_ALL);
	LogComponentEnable ("UanPropModelThorp",LOG_LEVEL_ALL);*/


    LogComponentEnableAll (LOG_PREFIX_ALL);

	std::vector<std::string> macName;
	macName.push_back("ns3::UanMacCw");

	int sinkNum = 10;
	int nodeNum = 200;
	int bNodeNum = 20;
	int pktNum = 1;
	double boundary = 2000;
	double simTime = 800;
	int SimulatorTimes = 1;
	int count = SimulatorTimes;
	while(count)
	{
		DPRExperiment exp;
		exp.m_boundary = boundary;
		exp.m_nodeNum = nodeNum;
		exp.m_sinkNum = sinkNum;
		exp.m_bNodeNum = bNodeNum;
		exp.m_pktNum = pktNum;
		exp.m_simTime = simTime;
		SeedManager::SetRun(SeedManager::GetRun()+1);
		exp.Run();
	    count--;
	}
	NS_LOG_INFO("The number of received packet is " << ns3::UanRoutingDPR::sinkRecvDataNum);
}
