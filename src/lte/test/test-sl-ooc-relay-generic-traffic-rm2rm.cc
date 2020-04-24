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

NS_LOG_COMPONENT_DEFINE ("TestSlOocRelayGenericTrafficRm2Rm");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief UE-to-Network Relay with traffic from Remote UEs towards Remote Host and back test case.
 */
class SlOocRelayGenericTrafficRm2RmTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param nRelayUes number of Relay UEs
   * \param nRemoteUesPerRelay number of Remote UEs per Relay UE
   */
  SlOocRelayGenericTrafficRm2RmTestCase (uint32_t nRelayUes, uint32_t nRemoteUesPerRelay);
  virtual ~SlOocRelayGenericTrafficRm2RmTestCase ();

private:
  virtual void DoRun (void);

  /**
   * \brief Build name string function
   *
   * \returns the name string
   */
  static std::string BuildNameString (uint32_t nRelayUes, uint32_t nRemoteUesPerRelay, std::string description = "");

  /**
   * \brief Trace Sink to count the number of packets received by the Remote UE
   *
   * \param remUeIdxStr the index of the Remote UE
   * \param p pointer to the packet
   * \param srcAddrs the source address
   * \param dstAddrs the destination address
   */
  void UePacketTrace (std::string remUeIdxStr, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs);

  /**
   * \brief Change the UdpEchoClient remote node address (to where it will send packets)
   *  to the relay network address of the new remote node
   *
   * \param newRemoteNode the new Remote Node
   * \param app the UdpEchoClient application to change
   * \param port the port
   */
  void ChangeUdpEchoClientRemote (Ptr<Node> newRemoteNode, Ptr<UdpEchoClient> app, uint16_t port );


  uint32_t m_nRelayUes;
  uint32_t m_nRemoteUesPerRelay;
  Ipv6Address m_relayNetwork;
  Ipv6Prefix m_relayPrefix;
  Ipv6Address m_echoServerAddress;
  uint32_t m_nPackets;
  std::vector<uint32_t> m_nRxPacketsPerRemoteUe;

};

SlOocRelayGenericTrafficRm2RmTestCase::SlOocRelayGenericTrafficRm2RmTestCase (uint32_t nRelayUes, uint32_t nRemoteUesPerRelay )
  : TestCase (BuildNameString (nRelayUes,nRemoteUesPerRelay, "sl-ooc-relay-generic-traffic-rm2rm: ")),
  m_nRelayUes (nRelayUes),
  m_nRemoteUesPerRelay (nRemoteUesPerRelay),
  m_relayNetwork ("7777:f00e::"),
  m_relayPrefix (Ipv6Prefix (48)),
  m_nPackets (20)
{
  NS_LOG_FUNCTION (this);
  m_echoServerAddress = Ipv6Address::GetOnes ();
  m_nRxPacketsPerRemoteUe.resize (nRelayUes * nRemoteUesPerRelay, 0);

}

SlOocRelayGenericTrafficRm2RmTestCase::~SlOocRelayGenericTrafficRm2RmTestCase ()
{
}

void
SlOocRelayGenericTrafficRm2RmTestCase::UePacketTrace (std::string remUeIdxStr, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  uint32_t remUeIdx = std::stoi (remUeIdxStr);
  m_nRxPacketsPerRemoteUe[remUeIdx]++;
}

void
SlOocRelayGenericTrafficRm2RmTestCase::ChangeUdpEchoClientRemote (Ptr<Node> newRemoteNode, Ptr<UdpEchoClient> app, uint16_t port )
{

  Ptr<Ipv6> ipv6 = newRemoteNode->GetObject<Ipv6> ();
  //Get the interface used for the relay network
  int32_t ipInterfaceIndex = ipv6->GetInterfaceForPrefix (m_relayNetwork, m_relayPrefix);

  Ipv6Address remoteNodeSlIpAddress = newRemoteNode->GetObject<Ipv6L3Protocol> ()->GetAddress (ipInterfaceIndex,1).GetAddress ();
  NS_LOG_DEBUG (" Node id = [" << app->GetNode ()->GetId ()
                               << "] changed the UdpEchoClient Remote Ip Address to " << remoteNodeSlIpAddress);
  m_echoServerAddress = remoteNodeSlIpAddress;
  NS_LOG_DEBUG ("Changed m_echoServerAddress to " << m_echoServerAddress);

  app->SetRemote (remoteNodeSlIpAddress, port);
}

void
SlOocRelayGenericTrafficRm2RmTestCase::DoRun (void)
{
  /*
   * *Scenario description:*
   * Topology:
   * The 'nRelayUes' Relay UEs are deployed around the eNB uniformly on a
   * circle of radius 'relayRadius' meters.
   * Each Relay UE has a cluster of 'nRemoteUesPerRelay' Remote UEs deployed
   * around itself uniformly on a circle of radius 'remoteRadius' meters.
   *
   * One-to-one connection:
   * The cluster of Remote UEs around a given Relay UE are interested only in its
   * Relay Service Code and thus will connect only to that Relay UE.
   * The UEs start their relay service sequentially in time. First the Relay UE,
   * then the cluster of Remote UEs associated to that Relay UE (sequentially as
   * well), then the next Relay UE, and so on.
   *
   * Traffic:
   * The first Remote UE connected to the first Relay UE is the echoServer.
   * Each of the other Remote UEs send traffic to that Remote UE, which echo back
   * the message to the initiating Remote UE.
   * Each initiating Remote UE starts sending traffic 3.00 s after the start of
   * the one-to-one connection procedure with its Relay UE and remain active
   * during 10.0 s. The simulation time is calculated so that the last Remote UE
   * can have its 10.0s of traffic activity.
   */

  NS_LOG_DEBUG ("***************************************************************");
  NS_LOG_DEBUG ("Starting Test with: "
                << " m_nRelayUes= " << m_nRelayUes
                << ", m_nRemoteUesPerRelay= " << m_nRemoteUesPerRelay
                << ", m_relayNetwork= " << m_relayNetwork
                << ", m_relayPrefix= " << m_relayPrefix
                << ", m_echoServerAddress= " << m_echoServerAddress);

  //Calculate the start time of the relay service for Relay UEs and Remote UEs
  //Do it sequentially for easy of tractability
  double startTimeRelay[m_nRelayUes];
  double startTimeRemote[m_nRelayUes * m_nRemoteUesPerRelay];
  //The time between Remote UE's start of service
  //Four discovery periods (4 * 0.32 s) to ensure relay selection (measurement report every 4 periods)
  //One discovery period (0.32 s) to avoid some of the congestion for the connection messages
  double timeBetweenRemoteStarts = 4 * 0.32 + 0.32; //s
  // The time between Relay UE's start of service
  // 1.0 s of baseline
  // 0.320 s to ensure sending the 1st discovery message
  // plus the time needed to all Remote UEs to connect to it:
  // (2+2*nRemoteUesPerRelay)*0.04 is the time in the worst case for all connection messages to go through between a Remote UE and a Relay UE:
  // 2 msgs from the Remote UE, each of them in 1 single SL period (0.04 s) and
  // 2 msgs from the Relay UE, which given the SL period RR scheduling, it can take in the worst case up to
  // nRemoteUesPerRelay SL periods to be sent
  // timeBetweenRemoteStarts to ensure sequentiality of activation
  double timeBetweenRelayStarts = 1.0 + m_nRemoteUesPerRelay * ((2 + 2 * m_nRemoteUesPerRelay) * 0.04 + timeBetweenRemoteStarts); //s

  for (uint32_t ryIdx = 0; ryIdx < m_nRelayUes; ryIdx++)
    {
      startTimeRelay[ryIdx] = 2.0 + 0.320 + timeBetweenRelayStarts * ryIdx;

      for (uint32_t rm = 0; rm < m_nRemoteUesPerRelay; ++rm)
        {
          uint32_t rmIdx = ryIdx * m_nRemoteUesPerRelay + rm;
          startTimeRemote[rmIdx] = startTimeRelay[ryIdx] + timeBetweenRemoteStarts * (rm + 1);
        }
    }

  //Calculate simTime based on relay service starts and give 10 s of traffic for the last one
  double simTime = startTimeRemote[(m_nRelayUes * m_nRemoteUesPerRelay - 1)] + 3.0 + 10.0; //s

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (6)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false));
  Config::SetDefault ("ns3::LteUeMac::SlScheduler", StringValue ("Random")); //Values include Fixed, Random, MinPrb, MaxCoverage

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
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));

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

  //Configure Timers
  //With this configuration the Remote UEs will try only one time to connect to the Relay UE
  //And in case of success the connection will remain established until the simulation ends
  //regardless of the traffic
  //If the connection fails for a Remote UE, it remains OOC and no traffic can go through
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4108", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4101", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4102", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4103", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayTRUIR", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4103", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4100", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4111", UintegerValue (simTime * 1000));

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

  NodeContainer relayUeNodes;
  relayUeNodes.Create (m_nRelayUes);

  NodeContainer remoteUeNodes;
  remoteUeNodes.Create (m_nRelayUes * m_nRemoteUesPerRelay);
  NodeContainer allUeNodes = NodeContainer (relayUeNodes,remoteUeNodes);

  //Position of the nodes
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));

  //UEs
  double relayRadius = 300.0; //m
  double remoteRadius = 50.0; //m

  Ptr<ListPositionAllocator> positionAllocRelays = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> positionAllocRemotes = CreateObject<ListPositionAllocator> ();
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ++ry)
    {
      //Relay UE
      double ry_angle = ry * (360.0 / relayUeNodes.GetN ()); //degrees
      double ry_pos_x = std::floor (relayRadius * std::cos (ry_angle * M_PI / 180.0) );
      double ry_pos_y = std::floor (relayRadius * std::sin (ry_angle * M_PI / 180.0) );

      positionAllocRelays->Add (Vector (ry_pos_x, ry_pos_y, 1.5));

      //Remote UEs
      for (uint32_t rm = 0; rm < m_nRemoteUesPerRelay; ++rm)
        {
          double rm_angle = rm * (360.0 / m_nRemoteUesPerRelay); //degrees
          double rm_pos_x = std::floor (ry_pos_x + remoteRadius * std::cos (rm_angle * M_PI / 180.0));
          double rm_pos_y = std::floor (ry_pos_y + remoteRadius * std::sin (rm_angle * M_PI / 180.0));

          positionAllocRemotes->Add (Vector (rm_pos_x, rm_pos_y, 1.5));
        }
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
  lteHelper->AssignStreams (allUeDevs,1);

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

  //Install Sidelink configuration for eNBs
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Configure Sidelink Preconfiguration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);

  LteRrcSap::SlPreconfiguration preconfigurationRemote;
  LteRrcSap::SlPreconfiguration preconfigurationRelay;

  //Preconfiguration for OOC
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

  ueSidelinkConfiguration->SetDiscEnabled (true);
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRelay);
  lteHelper->InstallSidelinkConfiguration (relayUeDevs, ueSidelinkConfiguration);

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRemote);
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

  //Routing downward traffic for the Remote UEs
  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo (m_relayNetwork, m_relayPrefix, Ipv6Address ("::"), 1, 0);

  //Attach Relay UEs to the eNB
  lteHelper->Attach (relayUeDevs);


  ///*** Configure applications ***///
  // We use a dummy IP address for initial configuration as we don't know the
  // IP address of the 'first Remote UE' before it connects to the Relay UE
  Ipv6Address dummyAddr;
  uint16_t echoPortBase = 50000;
  ApplicationContainer serverApps;
  ApplicationContainer clientApps;

  //For each UE (except the 'first Remote UE'), we have a pair (UpdEchoClient, UdpEchoServer)
  //Each UE has an assigned port
  //UdpEchoClient installed in the Remote UE, sending to the 'first Remote UE' address in the UE port
  //UdpEchoServer installed in the 'first Remote UE', listening to the Remote UE port
  std::ostringstream oss;
  for (uint16_t remUeIdx = 1; remUeIdx < remoteUeNodes.GetN (); remUeIdx++)
    {
      uint16_t remUePort = echoPortBase + remUeIdx;

      //UdpEchoServer in the 'first Remote UE' listening for packets from the other UEs
      UdpEchoServerHelper echoServerHelper (remUePort);
      ApplicationContainer singleServerApp = echoServerHelper.Install (remoteUeNodes.Get (0));
      singleServerApp.Start (Seconds (1.0));
      singleServerApp.Stop (Seconds (simTime));

      serverApps.Add (singleServerApp);

      //UdpEchoClient in the Remote UE
      UdpEchoClientHelper echoClientHelper (dummyAddr); //The address will be changed in the function ChangeRemote
      echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (m_nPackets));
      echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
      echoClientHelper.SetAttribute ("PacketSize", UintegerValue (150));
      echoClientHelper.SetAttribute ("RemotePort", UintegerValue (remUePort));

      ApplicationContainer singleClientApp = echoClientHelper.Install (remoteUeNodes.Get (remUeIdx));
      //Start the application 3 s after the remote UE started the relay service
      //normally it should be enough time to connect
      singleClientApp.Start (Seconds (3.0 + startTimeRemote[remUeIdx]) );
      //Stop the application after 10.0 s
      singleClientApp.Stop (Seconds (3.0 + startTimeRemote[remUeIdx] + 10.0));

      oss << remUeIdx;
      singleClientApp.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeCallback (&SlOocRelayGenericTrafficRm2RmTestCase::UePacketTrace,this ));
      oss.str ("");

      clientApps.Add (singleClientApp);

      //Schedule the change of the RemoteAddress to 100 ms before the start of the application
      //normally 'first Remote UE' should be already connected to its Relay UE so we can
      //assign its address as RemoteAddress
      Simulator::Schedule (Seconds (3.0 + startTimeRemote[remUeIdx] - 0.100),
                           &SlOocRelayGenericTrafficRm2RmTestCase::ChangeUdpEchoClientRemote,
                           this,
                           remoteUeNodes.Get (0),
                           singleClientApp.Get (0)->GetObject<UdpEchoClient> (), remUePort );

    }

  ///*** End of application configuration ***///

  ///*** Configure Relaying ***///
  proseHelper->SetIpv6BaseForRelayCommunication (m_relayNetwork, m_relayPrefix);

  Ptr<EpcTft> tft = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localIpv6Address.Set ("7777:f00e::");
  dlpf.localIpv6Prefix = Ipv6Prefix (32);
  tft->Add (dlpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (relayUeDevs, bearer, tft);

  for (uint32_t ryDevIdx = 0; ryDevIdx < relayUeDevs.GetN (); ryDevIdx++)
    {
      uint32_t serviceCode = relayUeDevs.Get (ryDevIdx)->GetObject<LteUeNetDevice> ()->GetImsi ();

      Simulator::Schedule (Seconds (startTimeRelay[ryDevIdx]), &LteSidelinkHelper::StartRelayService, proseHelper, relayUeDevs.Get (ryDevIdx), serviceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
      NS_LOG_DEBUG ("Relay UE " << ryDevIdx << " node id = [" << relayUeNodes.Get (ryDevIdx)->GetId () << "] provides Service Code " << serviceCode << " and start service at " << startTimeRelay[ryDevIdx] << " s");
      //Remote UEs
      for (uint32_t rm = 0; rm < m_nRemoteUesPerRelay; ++rm)
        {
          uint32_t rmDevIdx = ryDevIdx * m_nRemoteUesPerRelay + rm;
          Simulator::Schedule ((Seconds (startTimeRemote[rmDevIdx])), &LteSidelinkHelper::StartRelayService, proseHelper, remoteUeDevs.Get (rmDevIdx), serviceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
          NS_LOG_DEBUG ("Remote UE " << rmDevIdx << " node id = [" << remoteUeNodes.Get (rmDevIdx)->GetId () << "] interested in Service Code " << serviceCode << " and start service at " << startTimeRemote[rmDevIdx] << " s");
        }
    }

  NS_LOG_DEBUG ("Simulation time " << simTime << " s");

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();

  double nRemoteFlows = 0.0;
  double nRemoteFlowsWithEnoughPackets = 0.0;

  NS_LOG_DEBUG ("Results:");

  for (uint32_t remUeIdx = 1; remUeIdx < m_nRelayUes * m_nRemoteUesPerRelay; remUeIdx++)
    {
      NS_LOG_DEBUG ("remUeIdx " << remUeIdx << " RxPackets " << m_nRxPacketsPerRemoteUe[remUeIdx]);
      if (m_nRxPacketsPerRemoteUe[remUeIdx] > 0)
        {
          nRemoteFlows++;
          if (m_nRxPacketsPerRemoteUe[remUeIdx] > m_nPackets / 2)
            {
              nRemoteFlowsWithEnoughPackets++;
            }
        }
    }

  //Remember that the 1st Remote UE is the EchoServer (flows to check = (m_nRelayUes*m_nRemoteUesPerRelay - 1)
  NS_LOG_DEBUG ("Ratio of Remote UEs with traffic: " << nRemoteFlows << " / " << (m_nRelayUes * m_nRemoteUesPerRelay - 1) << " = " << nRemoteFlows / (m_nRelayUes * m_nRemoteUesPerRelay - 1));
  NS_LOG_DEBUG ("Ratio of Remote UEs with enough packets: " << nRemoteFlowsWithEnoughPackets << " / " << (m_nRelayUes * m_nRemoteUesPerRelay - 1) << " = " << nRemoteFlowsWithEnoughPackets / (m_nRelayUes * m_nRemoteUesPerRelay - 1));

  NS_TEST_ASSERT_MSG_GT (nRemoteFlows / (m_nRelayUes * m_nRemoteUesPerRelay - 1), 0.5, "Not enough Remote UEs with traffic");
  NS_TEST_ASSERT_MSG_GT (nRemoteFlowsWithEnoughPackets / (m_nRelayUes * m_nRemoteUesPerRelay - 1), 0.5, "Not enough Remote UEs with enough packets");

  NS_LOG_DEBUG ("***************************************************************");

  Simulator::Destroy ();

}

std::string
SlOocRelayGenericTrafficRm2RmTestCase::BuildNameString (uint32_t nRelayUes, uint32_t nRemoteUesPerRelay, std::string description )
{
  std::ostringstream oss;
  oss << description << " nRelayUes= " << nRelayUes << " nRemoteUesPerRelay= " << nRemoteUesPerRelay;
  return oss.str ();
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief UE-to-Network Relay with traffic from Remote UEs towards Remote Host and back test suite.
 */
class SlOocRelayGenericTrafficRm2RmTestSuite : public TestSuite
{
public:
  SlOocRelayGenericTrafficRm2RmTestSuite ();
};

SlOocRelayGenericTrafficRm2RmTestSuite::SlOocRelayGenericTrafficRm2RmTestSuite ()
  : TestSuite ("sl-ooc-relay-generic-traffic-rm2rm", SYSTEM)
{

  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (1, 2), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (1, 5), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (1, 10), TestCase::QUICK);

  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (2, 1), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (2, 2), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (2, 5), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (2, 10), TestCase::EXTENSIVE);

  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (5, 1), TestCase::QUICK);
  AddTestCase (new SlOocRelayGenericTrafficRm2RmTestCase (5, 2), TestCase::EXTENSIVE);

}

static SlOocRelayGenericTrafficRm2RmTestSuite staticSlOocRelayGenericTrafficRm2RmTestSuite;
