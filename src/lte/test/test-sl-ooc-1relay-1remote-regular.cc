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

NS_LOG_COMPONENT_DEFINE ("TestSlOoc1Relay1RemoteRegular");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink ooc 1-relay 1-remote regular test case.
 */
class SlOoc1Relay1RemoteRegularTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param simTime simTime
   * \param echoClientMaxPackets echoClientMaxPackets
   * \param echoClientInterval echoClientInterval
   * \param echoClientPacketSize echoClientPacketSize
   */
  SlOoc1Relay1RemoteRegularTestCase (double simTime, double echoClientMaxPackets, double echoClientInterval, double echoClientPacketSize);
  virtual ~SlOoc1Relay1RemoteRegularTestCase ();
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
  /**
   * PC5SignalingPacketTraceFunction
   *
   * \param srcL2Id srcL2Id
   * \param dstL2Id dstL2Id
   * \param p Packet
   */
  void PC5SignalingPacketTraceFunction (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p);

private:
  virtual void DoRun (void);
  double m_simTime; ///< simTime
  double m_echoClientMaxPackets; ///< echoClientMaxPackets
  double m_echoClientInterval; ///< echoClientInterval
  double m_echoClientPacketSize; ///< echoClientPacketSize
  bool m_recvRUIRq; ///< recvRUIRq
  bool m_recvRUIRs; ///< recvRUIRs
  uint32_t m_srcL2IdToCompareLater; ///< srcL2IdToCompareLater
  uint32_t m_dstL2IdToCompareLater; ///< dstL2IdToCompareLater
  bool m_txPacket; ///< txPacket
  bool m_rxPacket; ///< rxPacket
  Ipv6Address m_srcAddressToCompareLater; ///< srcAddressToCompareLater
  Ipv6Address m_dstAddressToCompareLater; ///< dstAddressToCompareLater
};

SlOoc1Relay1RemoteRegularTestCase::SlOoc1Relay1RemoteRegularTestCase (double simTime, double echoClientMaxPackets, double echoClientInterval, double echoClientPacketSize)
  : TestCase ("Scenario with 1 eNodeB and 2 UEs using SideLink"),
  m_simTime (simTime),
  m_echoClientMaxPackets (echoClientMaxPackets),
  m_echoClientInterval (echoClientInterval),
  m_echoClientPacketSize (echoClientPacketSize),
  m_recvRUIRq (false),
  m_recvRUIRs (false),
  m_srcL2IdToCompareLater (0),
  m_dstL2IdToCompareLater (0),
  m_txPacket (false),
  m_rxPacket (false)
{
  m_srcAddressToCompareLater = Ipv6Address::GetOnes ();
  m_dstAddressToCompareLater = Ipv6Address::GetOnes ();
}

SlOoc1Relay1RemoteRegularTestCase::~SlOoc1Relay1RemoteRegularTestCase ()
{
}


void
SlOoc1Relay1RemoteRegularTestCase::DataPacketSinkTxNode (Ptr<const Packet> p, const Address &src, const Address &dst)
{
  m_srcAddressToCompareLater = Inet6SocketAddress::ConvertFrom (src).GetIpv6 ();
  m_dstAddressToCompareLater = Inet6SocketAddress::ConvertFrom (dst).GetIpv6 ();
  m_txPacket = true;
}

void
SlOoc1Relay1RemoteRegularTestCase::DataPacketSinkRxNode (Ptr<const Packet> p, const Address &src, const Address &dst)
{
  if (Inet6SocketAddress::ConvertFrom (src).GetIpv6 () == m_dstAddressToCompareLater && Inet6SocketAddress::ConvertFrom (dst).GetIpv6 () == m_srcAddressToCompareLater)
    {
      m_rxPacket = true;
    }
}

void
SlOoc1Relay1RemoteRegularTestCase::PC5SignalingPacketTraceFunction (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p)
{
  LteSlPc5SignallingMessageType lpc5smt;
  p->PeekHeader (lpc5smt);

  if (lpc5smt.GetMessageType () == LteSlPc5SignallingMessageType::RemoteUeInfoRequest)
    {
      m_srcL2IdToCompareLater = srcL2Id;
      m_dstL2IdToCompareLater = dstL2Id;
      m_recvRUIRq = true;
    }

  if (lpc5smt.GetMessageType () == LteSlPc5SignallingMessageType::RemoteUeInfoResponse)
    {
      if ((srcL2Id == m_dstL2IdToCompareLater) && (dstL2Id == m_srcL2IdToCompareLater))
        {
          m_recvRUIRs = true;
        }
    }

}

void
SlOoc1Relay1RemoteRegularTestCase::DoRun (void)
{
  Time simTime = Seconds (m_simTime);
  bool useRelay = true;
  double relayUeInitXPos = 300.0;
  double remoteUeInitXPos = 350.0;

  //Configure One-To-One Communication timers and counters between Relay and Remote UE
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4108", UintegerValue (10000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4100", UintegerValue (10000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4101", UintegerValue (50000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4102", UintegerValue (1000));

  //Enable the Remote UE information request procedure
  Config::SetDefault ("ns3::LteSlUeRrc::RuirqEnabled",  BooleanValue (true));

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
  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");

  //Position of the nodes
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  //Relay UE
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (relayUeInitXPos, 0.0, 1.5));
  //Remote UE
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (remoteUeInitXPos, 0.0, 1.5));

  //Install mobility
  //eNodeB
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  //Relay UE
  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  //Remote UE
  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

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

  uint32_t groupL2Address = 255;
  enbSidelinkConfiguration->AddPreconfiguredDedicatedPool (groupL2Address, pool);
  enbSidelinkConfiguration->AddPreconfiguredDedicatedPool (1, pool); //update to use L2ID for remote UE
  enbSidelinkConfiguration->AddPreconfiguredDedicatedPool (2, pool); //update to use L2ID for remote UE

  //Configure default communication pool
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

  //Install Sidelink configuration for eNBs
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Configure Sidelink Preconfiguration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  LteRrcSap::SlPreconfiguration preconfigurationRemote;
  LteRrcSap::SlPreconfiguration preconfigurationRelay;

  if (useRelay)
    {
      //General
      preconfigurationRemote.preconfigGeneral.carrierFreq = 23330;
      preconfigurationRemote.preconfigGeneral.slBandwidth = 50;

      //Discovery
      preconfigurationRemote.preconfigDisc.nbPools = 1;
      LteSlDiscPreconfigPoolFactory preconfDiscPoolFactory;
      preconfDiscPoolFactory.SetDiscCpLen ("NORMAL");
      preconfDiscPoolFactory.SetDiscPeriod ("rf32");
      preconfDiscPoolFactory.SetNumRetx (0);
      preconfDiscPoolFactory.SetNumRepetition (1);
      preconfDiscPoolFactory.SetDiscPrbNum (10);
      preconfDiscPoolFactory.SetDiscPrbStart (10);
      preconfDiscPoolFactory.SetDiscPrbEnd (40);
      preconfDiscPoolFactory.SetDiscOffset (0);
      preconfDiscPoolFactory.SetDiscBitmap (0x11111);
      preconfDiscPoolFactory.SetDiscTxProbability ("p100");

      preconfigurationRemote.preconfigDisc.pools[0] = preconfDiscPoolFactory.CreatePool ();

      //Communication
      preconfigurationRemote.preconfigComm.nbPools = 1;
      LteSlPreconfigPoolFactory preconfCommPoolFactory;
      //-Control
      preconfCommPoolFactory.SetControlPeriod ("sf40");
      preconfCommPoolFactory.SetControlBitmap (0x00000000FF); //8 subframes for PSCCH
      preconfCommPoolFactory.SetControlOffset (0);
      preconfCommPoolFactory.SetControlPrbNum (22);
      preconfCommPoolFactory.SetControlPrbStart (0);
      preconfCommPoolFactory.SetControlPrbEnd (49);
      //-Data
      preconfCommPoolFactory.SetDataBitmap (0xFFFFFFFFFF);
      preconfCommPoolFactory.SetDataOffset (8); //After 8 subframes of PSCCH
      preconfCommPoolFactory.SetDataPrbNum (25);
      preconfCommPoolFactory.SetDataPrbStart (0);
      preconfCommPoolFactory.SetDataPrbEnd (49);

      preconfigurationRemote.preconfigComm.pools[0] = preconfCommPoolFactory.CreatePool ();

      //-Relay UE (re)selection
      preconfigurationRemote.preconfigRelay.haveReselectionInfoOoc = true;
      preconfigurationRemote.preconfigRelay.reselectionInfoOoc.filterCoefficient = 0;
      preconfigurationRemote.preconfigRelay.reselectionInfoOoc.minHyst = 0;
      preconfigurationRemote.preconfigRelay.reselectionInfoOoc.qRxLevMin = -125;
    }

  ueSidelinkConfiguration->SetDiscEnabled (true);
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRelay);
  lteHelper->InstallSidelinkConfiguration (ueDevs.Get (0), ueSidelinkConfiguration);

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRemote);
  lteHelper->InstallSidelinkConfiguration (ueDevs.Get (1), ueSidelinkConfiguration);


  //Install the IP stack on the UEs and assign IP address
  internet.Install (ueNodes);
  Ipv6InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv6Address (NetDeviceContainer (ueDevs));

  //Define and set routing
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }
  ueIpIface.SetForwarding (0, true);

  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);

  internetIpIfaces.SetForwarding (0, true);
  internetIpIfaces.SetDefaultRouteInAllNodes (0);

  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
  remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);

  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo ("7777:f00e:0:0::", Ipv6Prefix (60), Ipv6Address ("::"), 1, 0);

  //Attach the relay UE to the eNodeB
  lteHelper->Attach (ueDevs.Get (0));
  //If not using relay, attach the remote to the eNodeB as well
  if (!useRelay)
    {
      lteHelper->Attach (ueDevs.Get (1));
    }

  // interface 0 is localhost, 1 is the p2p device
  Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
  uint16_t echoPort = 8000;

  //Set echo server in the remote host
  UdpEchoServerHelper echoServer (echoPort);
  ApplicationContainer serverApps = echoServer.Install (remoteHost);
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (simTime);

  //Set echo client in the UEs
  UdpEchoClientHelper echoClient (remoteHostAddr, echoPort);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (m_echoClientMaxPackets));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (m_echoClientInterval)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (m_echoClientPacketSize));
  ApplicationContainer clientApps;
  clientApps = echoClient.Install (ueNodes.Get (1)); //Only the remote UE will have traffic
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (simTime);
  ///*** End of application configuration ***///


  ///*** Configure Relaying ***///
  if (useRelay)
    {
      Ptr<EpcTft> tft = Create<EpcTft> ();
      EpcTft::PacketFilter dlpf;
      dlpf.localIpv6Address.Set ("7777:f00e::");
      dlpf.localIpv6Prefix = Ipv6Prefix (64);
      tft->Add (dlpf);
      EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
      lteHelper->ActivateDedicatedEpsBearer (ueDevs.Get (0), bearer, tft);

      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueDevs.Get (0), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
      Simulator::Schedule (Seconds (4.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueDevs.Get (1), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
    }

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/SidelinkConfiguration/PC5SignalingPacketTrace", MakeCallback (&SlOoc1Relay1RemoteRegularTestCase::PC5SignalingPacketTraceFunction, this));

  std::ostringstream txWithAddresses;
  txWithAddresses << "/NodeList/" << ueDevs.Get (1)->GetNode ()->GetId () << "/ApplicationList/0/TxWithAddresses";
  Config::ConnectWithoutContext (txWithAddresses.str (), MakeCallback (&SlOoc1Relay1RemoteRegularTestCase::DataPacketSinkTxNode, this));

  std::ostringstream rxWithAddresses;
  rxWithAddresses << "/NodeList/" << ueDevs.Get (1)->GetNode ()->GetId () << "/ApplicationList/0/RxWithAddresses";
  Config::ConnectWithoutContext (rxWithAddresses.str (), MakeCallback (&SlOoc1Relay1RemoteRegularTestCase::DataPacketSinkRxNode, this));

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ ((m_recvRUIRq && m_recvRUIRs), true, "Error in sending and receiving RemoteUeInfoRequest and RemoteUeInfoResponse. Test regular scenario failed");
  NS_TEST_ASSERT_MSG_EQ ((m_txPacket && m_rxPacket), true, "Error in sending and receiving (echoing) of atleast one packet at the Remote UE. Test regular scenario failed");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink ooc 1-relay 1-remote regular test suite.
 */
class SlOoc1Relay1RemoteRegularTestSuite : public TestSuite
{
public:
  SlOoc1Relay1RemoteRegularTestSuite ();
};

SlOoc1Relay1RemoteRegularTestSuite::SlOoc1Relay1RemoteRegularTestSuite ()
  : TestSuite ("sl-ooc-1relay-1remote-regular", SYSTEM)
{
  //Test regular scenario
  //Remote UE (out of coverage) detects, connects, and use relay UE to send and receive traffic.
  //Pass the variables simTime, echoClientMaxPackets, echoClientInterval, echoClientPacketSize
  AddTestCase (new SlOoc1Relay1RemoteRegularTestCase (10.0, 20, 0.2, 150), TestCase::QUICK);
}

static SlOoc1Relay1RemoteRegularTestSuite staticSlOoc1Relay1RemoteRegularTestSuite;
