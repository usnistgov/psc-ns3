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
#include "ns3/config-store-module.h"
#include "ns3/test.h"
#include "ns3/lte-helper.h"
#include "ns3/lte-sidelink-helper.h"
#include "ns3/lte-sl-preconfig-pool-factory.h"
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

NS_LOG_COMPONENT_DEFINE ("TestSidelinkOutOfCoverageComm");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink out of coverage communication test case.
 */
class SidelinkOutOfCoverageCommTestCase : public TestCase
{
public:
  /**
   * Constructor
   */
  SidelinkOutOfCoverageCommTestCase ();
  virtual ~SidelinkOutOfCoverageCommTestCase ();

private:
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

SidelinkOutOfCoverageCommTestCase::SidelinkOutOfCoverageCommTestCase ()
  : TestCase ("Scenario with 2 out of coverage UEs performing Sidelink communication"),
    m_numPacketRx (0)
{
}

SidelinkOutOfCoverageCommTestCase::~SidelinkOutOfCoverageCommTestCase ()
{
}

void
SidelinkOutOfCoverageCommTestCase::SinkRxNode (Ptr<const Packet> p, const Address &add)
{
  NS_LOG_INFO ("Node received " << m_numPacketRx << " packets");
  m_numPacketRx++;
}

void
SidelinkOutOfCoverageCommTestCase::DoRun (void)
{
 /**
  * Create a scenario with two UEs, which are out of coverage.
  * One of the UE will send traffic directly to the other UE via Sidelink
  * The resource scheduling is performed by the transmitting UE, i.e.,
  * MODE 2. Default configuration will send 10 packets per second for 2 seconds.
  * The expected output is that the receiver UE node would receive 20 packets.
  */
  /*
   LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
   LogComponentEnable ("LteUePhy", logLevel);
   LogComponentEnable ("LteUeRrc", logLevel);
   LogComponentEnable ("LteUeMac", logLevel);
   LogComponentEnable ("LteSlPool", logLevel);
  */
  //Initialize simulation time
  double simTime = 6;
  uint32_t groupL2Address = 0xFF;

  // Configure the UE for UE_SELECTED scenarios
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true)); //use default Trp index of 0

  //Set the frequency to use the Public Safety case (band 14 : 700 MHz)
  uint32_t ulEarfcn = 23330;
  uint16_t ulBandwidth = 50;

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));

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

  // channel model initialization
  lteHelper->Initialize ();

  // Since we are not installing eNB, we need to set the frequency attribute of pathloss model here
  double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (ulEarfcn);
  NS_LOG_LOGIC ("UL freq: " << ulFreq);
  Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ABORT_MSG_IF (lossModel == nullptr, "No PathLossModel");
  bool ulFreqOk = uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

  NS_LOG_INFO ("Deploying UE's...");

  // Create UE nodes
  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("Ue 0 node id = " << ueNodes.Get (0)->GetId ());
  NS_LOG_INFO ("Ue 1 node id = " << ueNodes.Get (1)->GetId ());

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (100, 10, 1.5));
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (100, -10, 1.5));

  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Sidelink pre-configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = ulEarfcn;
  preconfiguration.preconfigGeneral.slBandwidth = ulBandwidth;
  preconfiguration.preconfigComm.nbPools = 1;

  LteSlPreconfigPoolFactory pfactory;

  //Control
  pfactory.SetControlPeriod ("sf40");
  pfactory.SetControlBitmap (0x00000000FF); //8 subframes for PSCCH
  pfactory.SetControlOffset (0);
  pfactory.SetControlPrbNum (22);
  pfactory.SetControlPrbStart (0);
  pfactory.SetControlPrbEnd (49);

  //Data
  pfactory.SetDataBitmap (0xFFFFFFFFFF);
  pfactory.SetDataOffset (8); //After 8 subframes of PSCCH
  pfactory.SetDataPrbNum (25);
  pfactory.SetDataPrbStart (0);
  pfactory.SetDataPrbEnd (49);

  preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();

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
  Config::ConnectWithoutContext (oss.str (), MakeCallback (&SidelinkOutOfCoverageCommTestCase::SinkRxNode,this));

  //Set Sidelink bearers
  Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress, groupL2Address);
  proseHelper->ActivateSidelinkBearer (Seconds (2.0), ueDevs, tft);
  ///*** End of application configuration ***///

  // Output config store to txt format
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-attributes.txt"));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("RawText"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig2;
  outputConfig2.ConfigureDefaults ();
  outputConfig2.ConfigureAttributes ();

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_numPacketRx, 20, "20 packets should be received at the receiver!");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink out of coverage communication test suite.
 */
class SidelinkOutOfCoverageCommTestSuite : public TestSuite
{
public:
  SidelinkOutOfCoverageCommTestSuite ();
};

SidelinkOutOfCoverageCommTestSuite::SidelinkOutOfCoverageCommTestSuite ()
  : TestSuite ("sidelink-out-of-coverage-comm", SYSTEM)
{
  // LogComponentEnable ("TestSidelinkOutOfCoverageComm", LOG_LEVEL_ALL);

  //Test 1
  AddTestCase (new SidelinkOutOfCoverageCommTestCase (), TestCase::QUICK);
}

static SidelinkOutOfCoverageCommTestSuite staticSidelinkOutOfCoverageCommTestSuite;
