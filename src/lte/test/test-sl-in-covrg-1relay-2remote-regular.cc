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


#include "ns3/object.h"
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store.h"
#include <cfloat>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestSlInCovrg1Relay2RemoteRegular");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg 1-relay 2-remote regular test case.
 */
class SlInCovrg1Relay2RemoteRegularTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param simTime simTime
   */
  SlInCovrg1Relay2RemoteRegularTestCase (double simTime);
  virtual ~SlInCovrg1Relay2RemoteRegularTestCase ();
  /**
   * DataPacketSinkTxNode
   *
   * \param p Packet
   * \param src src
   * \param dst dst
   */
  void DataPacketSinkTxNode (Ptr<const Packet> p, const Address &src, const Address &dst);
  /**
   * DataPacketSinkRxNode
   *
   * \param p Packet
   * \param src src
   * \param dst dst
   */
  void DataPacketSinkRxNode (Ptr<const Packet> p, const Address &src, const Address &dst);

private:
  virtual void DoRun (void);
  double m_simTime; ///< simTime
  double m_txPacketNum; ///< txPacketNum
  double m_rxPacketNum; ///< rxPacketNum
};

SlInCovrg1Relay2RemoteRegularTestCase::SlInCovrg1Relay2RemoteRegularTestCase (double simTime)
  : TestCase ("Scenario with 1 eNodeB and 3 UEs using SideLink"),
  m_simTime (simTime),
  m_txPacketNum (0),
  m_rxPacketNum (0)
{
}

SlInCovrg1Relay2RemoteRegularTestCase::~SlInCovrg1Relay2RemoteRegularTestCase ()
{
}

void
SlInCovrg1Relay2RemoteRegularTestCase::DataPacketSinkTxNode (Ptr<const Packet> p, const Address &src, const Address &dst)
{
  m_txPacketNum++;
}

void
SlInCovrg1Relay2RemoteRegularTestCase::DataPacketSinkRxNode (Ptr<const Packet> p, const Address &src, const Address &dst)
{
  m_rxPacketNum++;
}

void
SlInCovrg1Relay2RemoteRegularTestCase::DoRun (void)
{
  double simTime = m_simTime; //Total duration of the simulation [s]
  bool useRelay = true; //Use the UE-to-Network Relay
  double relayUeInitXPos = 300.0; //Initial X coordinate of the relay UE
  double remoteUeInitXPos = 350.0; //Initial X coordinate of the remote UE
  uint32_t nRelayUes = 1; //Number of relay UEs
  uint32_t nRemoteUes = 2; //Number of remote UEs
  bool remoteTraffic = true; //The Remote UEs have their own traffic
  bool relayTraffic = false; //The Relay UEs have their own traffic

  NS_LOG_INFO ("Configuring default parameters...");

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (6)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false)); //use default Trp index of 0

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (23330));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::CtrlFullDuplexEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));


  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (46.0));

  //Sidelink bearers activation time
  Time slBearersActivationTime = Seconds (2.0);

  //Create and set the helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);
  Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper", PointerValue (proseHelper));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

  //Create nodes (eNb + UEs)
  NodeContainer enbNode;
  enbNode.Create (1);
  NS_LOG_INFO ("eNb node id = [" << enbNode.Get (0)->GetId () << "]");

  NodeContainer relayUeNodes;
  relayUeNodes.Create (nRelayUes);

  NodeContainer remoteUeNodes;
  remoteUeNodes.Create (nRemoteUes);
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ry++)
    {
      NS_LOG_INFO ("Relay UE " << ry + 1 << " node id = [" << relayUeNodes.Get (ry)->GetId () << "]");
    }
  for (uint32_t rm = 0; rm < remoteUeNodes.GetN (); rm++)
    {
      NS_LOG_INFO ("Remote UE " << rm + 1 << " node id = [" << remoteUeNodes.Get (rm)->GetId () << "]");
    }
  NodeContainer allUeNodes = NodeContainer (relayUeNodes,remoteUeNodes);

  //Position of the nodes
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));

  //Relay UEs
  Ptr<ListPositionAllocator> positionAllocRelays = CreateObject<ListPositionAllocator> ();
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ++ry)
    {
      positionAllocRelays->Add (Vector (relayUeInitXPos, 0.0, 1.5));
    }
  //Remote UEs
  Ptr<ListPositionAllocator> positionAllocRemotes = CreateObject<ListPositionAllocator> ();
  for (uint32_t rm = 0; rm < remoteUeNodes.GetN (); ++rm)
    {
      positionAllocRemotes->Add (Vector (remoteUeInitXPos, 0.0, 1.5));

    }

  //Install mobility
  //eNodeB
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  //Relay UEs
  MobilityHelper mobilityRelays;
  mobilityRelays.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityRelays.SetPositionAllocator (positionAllocRelays);
  mobilityRelays.Install (relayUeNodes);

  //Remote UE
  MobilityHelper mobilityRemotes;
  mobilityRemotes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityRemotes.SetPositionAllocator (positionAllocRemotes);
  mobilityRemotes.Install (remoteUeNodes);

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer relayUeDevs = lteHelper->InstallUeDevice (relayUeNodes);
  NetDeviceContainer remoteUeDevs = lteHelper->InstallUeDevice (remoteUeNodes);
  NetDeviceContainer allUeDevs = NetDeviceContainer (relayUeDevs, remoteUeDevs);

  //Configure Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);

  //Configure communication pool
  LteRrcSap::SlCommTxResourcesSetup pool;

  pool.setup = LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlResourcePoolFactory pfactory;
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

  pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();

  //Add the pool as a default pool
  enbSidelinkConfiguration->SetDefaultPool (pool);

  //Configure discovery pool
  enbSidelinkConfiguration->SetDiscEnabled (true);

  LteRrcSap::SlDiscTxResourcesSetup discPool;
  discPool.setup =  LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED;
  discPool.ueSelected.havePoolToRelease = false;
  discPool.ueSelected.havePoolToAdd = true;
  discPool.ueSelected.poolToAddModList.nbPools = 1;
  discPool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlDiscResourcePoolFactory pDiscFactory;
  pDiscFactory.SetDiscCpLen ("NORMAL");
  pDiscFactory.SetDiscPeriod ("rf32");
  pDiscFactory.SetNumRetx (0);
  pDiscFactory.SetNumRepetition (1);
  pDiscFactory.SetDiscPrbNum (10);
  pDiscFactory.SetDiscPrbStart (10);
  pDiscFactory.SetDiscPrbEnd (40);
  pDiscFactory.SetDiscOffset (0);
  pDiscFactory.SetDiscBitmap (0x11111);
  pDiscFactory.SetDiscTxProbability ("p100");

  discPool.ueSelected.poolToAddModList.pools[0].pool =  pDiscFactory.CreatePool ();

  enbSidelinkConfiguration->AddDiscPool (discPool);

  //-Relay UE (re)selection
  LteRrcSap::Sib19DiscConfigRelay discConfigRelay;
  discConfigRelay.haveRemoteUeConfig = true;
  discConfigRelay.remoteUeConfig.haveReselectionInfoIc = true;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.filterCoefficient = 0;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.minHyst = 0;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.qRxLevMin = -125;
  enbSidelinkConfiguration->SetDiscConfigRelay (discConfigRelay);

  //Install Sidelink configuration for eNBs
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Configure Sidelink Preconfiguration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  ueSidelinkConfiguration->SetDiscEnabled (true);
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  lteHelper->InstallSidelinkConfiguration (relayUeDevs, ueSidelinkConfiguration);
  lteHelper->InstallSidelinkConfiguration (remoteUeDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign IP address
  internet.Install (relayUeNodes);
  internet.Install (remoteUeNodes);

  Ipv6InterfaceContainer ueIpIfaceRelays;
  Ipv6InterfaceContainer ueIpIfaceRemotes;
  ueIpIfaceRelays = epcHelper->AssignUeIpv6Address (relayUeDevs);
  ueIpIfaceRemotes = epcHelper->AssignUeIpv6Address (remoteUeDevs);

  //Define and set routing
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;
  for (uint32_t u = 0; u <  allUeNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = allUeNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);

  internetIpIfaces.SetForwarding (0, true);
  internetIpIfaces.SetDefaultRouteInAllNodes (0);

  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
  remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);


  //Routing downward traffic
  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo ("7777:f00e:0:0::", Ipv6Prefix (60), Ipv6Address ("::"), 1, 0);

  //Attach each UE to the best available eNB
  lteHelper->Attach (allUeDevs);

  AsciiTraceHelper ascii;

  ///*** Configure applications ***///

  // interface 0 is localhost, 1 is the p2p device
  Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
  uint16_t echoPortBase = 50000;
  ApplicationContainer serverApps;
  ApplicationContainer clientApps;

  //For each UE, we have a pair (UpdEchoClient, UdpEchoServer)
  //Each UE has an assigned port
  //UdpEchoClient installed in the UE, sending to the remoteHost address in the UE port
  //UdpEchoServer installed in the remoteHost, listening to the UE port

  if (relayTraffic)
    {
      for (uint16_t relUeIdx = 0; relUeIdx < relayUeNodes.GetN (); relUeIdx++)
        {
          uint16_t relUePort = echoPortBase + 100 + relUeIdx;

          //UdpEchoServer in the remoteHost for the Remote UE
          UdpEchoServerHelper echoServerHelper (relUePort);
          ApplicationContainer singleServerApp = echoServerHelper.Install (remoteHost);
          singleServerApp.Start (Seconds (1.0));
          singleServerApp.Stop (Seconds (simTime));

          serverApps.Add (singleServerApp);


          //UdpEchoClient in the Remote UE
          UdpEchoClientHelper echoClientHelper (remoteHostAddr);
          echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (8000000));
          echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.20)));
          echoClientHelper.SetAttribute ("PacketSize", UintegerValue (150 - (12)));

          echoClientHelper.SetAttribute ("RemotePort", UintegerValue (relUePort));

          ApplicationContainer singleClientApp = echoClientHelper.Install (relayUeNodes.Get (relUeIdx));
          singleClientApp.Start (Seconds (3.0) + MilliSeconds (relUeIdx * 50) );
          singleClientApp.Stop (Seconds (simTime));
          clientApps.Add (singleClientApp);

        }
    }

  if (remoteTraffic)
    {

      for (uint16_t remUeIdx = 0; remUeIdx < remoteUeNodes.GetN (); remUeIdx++)
        {
          uint16_t remUePort = echoPortBase + remUeIdx;

          //UdpEchoServer in the remoteHost for the Remote UE
          UdpEchoServerHelper echoServerHelper (remUePort);
          ApplicationContainer singleServerApp = echoServerHelper.Install (remoteHost);
          singleServerApp.Start (Seconds (1.0));
          singleServerApp.Stop (Seconds (simTime));

          serverApps.Add (singleServerApp);


          //UdpEchoClient in the Remote UE
          UdpEchoClientHelper echoClientHelper (remoteHostAddr);
          echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (8000000));
          echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.2)));
          echoClientHelper.SetAttribute ("PacketSize", UintegerValue (150 - (12)));

          echoClientHelper.SetAttribute ("RemotePort", UintegerValue (remUePort));

          ApplicationContainer singleClientApp = echoClientHelper.Install (remoteUeNodes.Get (remUeIdx));
          singleClientApp.Start (Seconds (3.0) + MilliSeconds (remUeIdx * 50) );
          singleClientApp.Stop (Seconds (simTime));
          clientApps.Add (singleClientApp);

        }
    }
  ///*** End of application configuration ***///


  ///*** Configure Relaying ***///
  if (useRelay)
    {
      proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (48));

      Ptr<EpcTft> tft = Create<EpcTft> ();
      EpcTft::PacketFilter dlpf;
      dlpf.localIpv6Address.Set ("7777:f00e::");
      dlpf.localIpv6Prefix = Ipv6Prefix (32);
      tft->Add (dlpf);
      EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
      lteHelper->ActivateDedicatedEpsBearer (relayUeDevs, bearer, tft);

      for (uint32_t ryDevIdx = 0; ryDevIdx < relayUeDevs.GetN (); ryDevIdx++)
        {
          Simulator::Schedule (Seconds (2.0 + 4 * ryDevIdx), &LteSidelinkHelper::StartRelayService, proseHelper, relayUeDevs.Get (ryDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
        }
      for (uint32_t rmDevIdx = 0; rmDevIdx < remoteUeDevs.GetN (); rmDevIdx++)
        {
          Simulator::Schedule ((Seconds (4.0 + rmDevIdx)), &LteSidelinkHelper::StartRelayService, proseHelper, remoteUeDevs.Get (rmDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
        }
    }

  for (uint32_t i = 0; i < remoteUeDevs.GetN (); i++)
    {
      std::ostringstream txWithAddresses;
      txWithAddresses << "/NodeList/" << remoteUeNodes.Get (i)->GetId () << "/ApplicationList/0/TxWithAddresses";
      Config::ConnectWithoutContext (txWithAddresses.str (), MakeCallback (&SlInCovrg1Relay2RemoteRegularTestCase::DataPacketSinkTxNode, this));

      std::ostringstream rxWithAddresses;
      rxWithAddresses << "/NodeList/" << remoteUeNodes.Get (i)->GetId () << "/ApplicationList/0/RxWithAddresses";
      Config::ConnectWithoutContext (rxWithAddresses.str (), MakeCallback (&SlInCovrg1Relay2RemoteRegularTestCase::DataPacketSinkRxNode, this));
    }

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();

  NS_LOG_DEBUG ("m_txPacketNum: " << m_txPacketNum << " m_rxPacketNum: " << m_rxPacketNum);

  NS_TEST_ASSERT_MSG_GT (m_rxPacketNum, (0.9 * m_txPacketNum), "Not enough received packets");

  Simulator::Destroy ();
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg 1-relay 2-remote regular test suite.
 */
class SlInCovrg1Relay2RemoteRegularTestSuite : public TestSuite
{
public:
  SlInCovrg1Relay2RemoteRegularTestSuite ();
};

SlInCovrg1Relay2RemoteRegularTestSuite::SlInCovrg1Relay2RemoteRegularTestSuite ()
  : TestSuite ("sl-in-covrg-1relay-2remote-regular", SYSTEM)
{
  //Test regular scenario
  //2 Remote UEs detects, connects, and use Relay UE to send and receive traffic.
  //Pass the variables simTime
  AddTestCase (new SlInCovrg1Relay2RemoteRegularTestCase (20.0), TestCase::QUICK);
}

static SlInCovrg1Relay2RemoteRegularTestSuite staticSlInCovrg1Relay2RemoteRegularTestSuite;
