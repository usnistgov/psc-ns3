/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */


#include <sstream>
#include "ns3/object.h"
#include "ns3/test.h"
#include "ns3/lte-helper.h"
#include "ns3/lte-sidelink-helper.h"
#include "ns3/lte-sl-resource-pool-factory.h"
#include "ns3/rr-sl-ff-mac-scheduler.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-epc-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/application-container.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/log.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestSidelinkInCoverageComm");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-coverage communication test case.
 */
class SidelinkInCoverageCommTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param scheduledPool Flag to indicate if the resources are scheduled
   */
  SidelinkInCoverageCommTestCase (bool scheduledPool);
  virtual ~SidelinkInCoverageCommTestCase ();

private:
  bool m_scheduledPool; ///< Flag to indicate if the resources are scheduled
  virtual void DoRun (void);
  /**
   * \brief Sink Rx function
   *
   * \param p The packet
   * \param add Address
   */
  void SinkRxNode (Ptr<const Packet> p, const Address &add);
  uint32_t m_numPacketRx; ///< Total number of Rx packets
};

SidelinkInCoverageCommTestCase::SidelinkInCoverageCommTestCase (bool scheduledPool)
  : TestCase ("Scenario with 1 eNodeB and 2 UEs using SideLink"),
  m_scheduledPool (scheduledPool),
  m_numPacketRx (0)
{
}

SidelinkInCoverageCommTestCase::~SidelinkInCoverageCommTestCase ()
{
}

void
SidelinkInCoverageCommTestCase::SinkRxNode (Ptr<const Packet> p, const Address &add)
{
  NS_LOG_INFO ("Node received " << m_numPacketRx << " packets");
  m_numPacketRx++;
}

void
SidelinkInCoverageCommTestCase::DoRun (void)
{
  /**
  * Create a scenario with two UEs, which are in coverage of one eNodeB
  * One of the UE will send traffic directly to the other UE via Sidelink
  * The resource scheduling is done by the eNodeB.
  * Default configuration will send 10 packets per second for 2 seconds
  * The expected output is that the receiver node would receive 20 packets.
  */

  //Initialize simulation time
  double simTime = 6;
  uint32_t groupL2Address = 0xFF;

  // Configure the scheduler (and UE for UE_SELECTED scenarios)
  Config::SetDefault ("ns3::RrSlFfMacScheduler::Itrp", UintegerValue (0));
  Config::SetDefault ("ns3::RrSlFfMacScheduler::SlGrantSize", UintegerValue (5)); //The number of RBs allocated per UE for Sidelink

  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true)); //use default Trp index of 0

  //Set the frequency to use the Public Safety case (band 14 : 700 MHz)
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", StringValue ("5330"));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", StringValue ("5330"));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", StringValue ("23330"));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", StringValue ("50"));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", StringValue ("50"));

  //Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (31.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (46.0));

  //Create the helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //ProSe
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  // Choose the appropriate scheduler
  // In one hand, the default scheduler for the downlink is Proportional Fair (PF),
  // so we will have to set the downlink scheduler to Round Robin
  // It is 3GPP requirements for D2D communications
  // In the other hand, the default scheduler for the uplink is Round Robin (RR)
  lteHelper->SetSchedulerType ("ns3::RrSlFfMacScheduler");

  //Create nodes (eNb + UEs)
  NodeContainer enbNode;
  enbNode.Create (1);
  NS_LOG_INFO ("eNb id = [" << enbNode.Get (0)->GetId () << "]");
  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("Ue 0 id = " << ueNodes.Get (0)->GetId ());
  NS_LOG_INFO ("Ue 1 id = " << ueNodes.Get (1)->GetId ());

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (100, 10, 1.5));
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (100, -10, 1.5));

  //Install mobility
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Add X2 inteface
  lteHelper->AddX2Interface (enbNode);

  //Configure Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);
  //Preconfigure pool for the group
  LteRrcSap::SlCommTxResourcesSetup pool;

  if (m_scheduledPool)
    {
      pool.setup = LteRrcSap::SlCommTxResourcesSetup::SCHEDULED;
      //BSR timers
      pool.scheduled.macMainConfig.periodicBsrTimer.period = LteRrcSap::PeriodicBsrTimer::sf16;
      pool.scheduled.macMainConfig.retxBsrTimer.period = LteRrcSap::RetxBsrTimer::sf640;
      //MCS
      pool.scheduled.haveMcs = true;
      pool.scheduled.mcs = 16;
      //resource pool
      LteSlResourcePoolFactory pfactory;
      pfactory.SetHaveUeSelectedResourceConfig (false); //since we want eNB to schedule
      pool.scheduled.commTxConfig = pfactory.CreatePool ();
    }
  else
    {
      pool.setup = LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED;
      pool.ueSelected.havePoolToRelease = false;
      pool.ueSelected.havePoolToAdd = true;
      pool.ueSelected.poolToAddModList.nbPools = 1;
      pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;
      //resource pool (default configuration is UE selected)
      LteSlResourcePoolFactory pfactory;
      pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();
    }

  enbSidelinkConfiguration->AddPreconfiguredDedicatedPool (groupL2Address, pool);
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //pre-configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs
  InternetStackHelper internet;
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

  //Assign IP address to UEs, and install applications
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  //Attach each UE to the best available eNB
  lteHelper->Attach (ueDevs);

  ///*** Configure applications ***///

  //Set Application in the UEs
  Ipv4Address groupAddress ("225.0.0.0"); //use multicast address as destination
  UdpClientHelper udpClient (groupAddress, 8000);
  udpClient.SetAttribute ("MaxPackets", UintegerValue (500));
  udpClient.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  udpClient.SetAttribute ("PacketSize", UintegerValue (280));

  ApplicationContainer clientApps = udpClient.Install (ueNodes.Get (0));
  clientApps.Get (0)->SetStartTime (Seconds (3.0));
  clientApps.Stop (Seconds (5.0));

  ApplicationContainer serverApps;
  PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory",Address (InetSocketAddress (Ipv4Address::GetAny (), 8000)));
  serverApps = sidelinkSink.Install (ueNodes.Get (1));
  serverApps.Get (0)->SetStartTime (Seconds (3.0));
  //Trace receptions; use the following to be robust to node ID changes
  std::ostringstream oss;
  oss << "/NodeList/" << ueNodes.Get (1)->GetId () << "/ApplicationList/0/$ns3::PacketSink/Rx";
  Config::ConnectWithoutContext (oss.str (), MakeCallback (&SidelinkInCoverageCommTestCase::SinkRxNode,this));
  //                                        ^
  //                                        |
  //adding more eNB nodes would increase the node id of the receiving UE. Therefore, this number should be incremented
  //if there is more than one eNB.

  //Set Sidelink bearers
  Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress, groupL2Address);
  proseHelper->ActivateSidelinkBearer (Seconds (2.0), ueDevs, tft);
  ///*** End of application configuration ***///

  //Enable traces
  lteHelper->EnableTraces ();

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_numPacketRx, 20, "20 packets should be received at the receiver!");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-coverage communication test suite.
 */
class SidelinkInCoverageCommTestSuite : public TestSuite
{
public:
  SidelinkInCoverageCommTestSuite ();
};

SidelinkInCoverageCommTestSuite::SidelinkInCoverageCommTestSuite ()
  : TestSuite ("sidelink-in-coverage-comm", SYSTEM)
{
  // LogComponentEnable ("TestSidelinkInCoverageComm", LOG_LEVEL_ALL);

  //scheduledPool -> true for SCHEDULED, false for UE_SELECTED
  //Test 1
  AddTestCase (new SidelinkInCoverageCommTestCase (true), TestCase::QUICK);
  //Test 2
  AddTestCase (new SidelinkInCoverageCommTestCase (false), TestCase::QUICK);
}

static SidelinkInCoverageCommTestSuite staticSidelinkInCoverageCommTestSuite;
