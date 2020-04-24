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


#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/psc-module.h"
#include "ns3/config-store.h"
#include "lte-sl-ue-controller-camad-2019.h"
#include <cfloat>
#include <sstream>

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("camad-2019-communication");

//Trace sink function for logging when a packet is transmitted or received
void
UePacketTrace (Ptr<OutputStreamWrapper> stream, std::string context, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);

  SeqTsHeader seqTs;
  p->PeekHeader (seqTs);

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << context << "\t" << p->GetSize () << "\t" << seqTs.GetSeq () << "\t"
                        << Simulator::Now ().GetMilliSeconds () - seqTs.GetTs ().GetMilliSeconds ()
                        << std::endl;

}


/*
 * This scenario is used to evaluate the UE-to-Network Relay communication
 * traffic. There are 4 UEs in the network:
 * - A Regular UE connected to the eNodeB
 * - A Relay UE connected to the eNodeB and offering UE-to-Network Relay
 * services
 * - Two Remote UEs that connects to the Relay UE
 * All the UEs register to a UdpGroupEchoServer Application in a Remote Host in
 * the internet. One of the UEs transmits packets using an OnOf Application
 * towards the Remote Host. The UdpGroupEchoServer in the Remote Host receive
 * the packets and then echoes them to all registered UEs.
 * The configurable paratmeter are:
 * - the sildelink communication period (slPeriod)
 * - the UE that is transmitting (txType)
 * - the time the traffic is active (appDurationTime)
 */
int main (int argc, char *argv[])
{
  double simTime = 20.0; //Simulation time (in seconds) updated automatically based on number of nodes and traffic duration
  double appStartTime = 0.0; //Application start time (in seconds) updated automatically based on number of nodes
  double regularUeInitXPos = 200.0;
  double relayUeInitXPos = 400.0;
  double remoteUeInitXPos = 800.0;
  Time timeout = Seconds (0);
  std::string mode ("InfSession");
  bool echoClient = false;

  uint32_t slPeriod = 40; //ms
  uint32_t nRelayUes = 1;
  uint32_t nRemoteUes = 2;
  uint32_t nRegularUes = 1;
  std::string txType ("Regular");
  double appDurationTime = 10.0; //Duration of traffic to generate

  CommandLine cmd;
  cmd.AddValue ("slPeriod", "Sidelink Communication Period", slPeriod);
  cmd.AddValue ("txType", "Set txType attribute (Regular|Relay|Remote)", txType);
  cmd.AddValue ("appDurationTime", "Duration of traffic to generate (s)", appDurationTime);

  cmd.Parse (argc, argv);

  //Calculate simTime based on number of remote UEs
  appStartTime = 10.0 + (10.0 * slPeriod / 1000.0) * nRemoteUes;
  simTime = appStartTime + appDurationTime + 1;

  NS_LOG_INFO ("Simulation time = " << simTime);
  NS_LOG_INFO ("Application start time = " << appStartTime);

  NS_LOG_INFO ("Configuring default parameters...");

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (6)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false));
  Config::SetDefault ("ns3::LteUeMac::SlScheduler", StringValue ("MaxCoverage")); //Values include Fixed, Random, MinPrb, MaxCoverage

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (23330));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::CtrlFullDuplexEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (false));

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

  //Set the Sl UE controller to be used
  lteHelper->SetAttribute ("SlUeController", StringValue ("ns3::LteSlUeControllerCamad2019"));
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::Campaign", StringValue ("Communication"));
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::ProseHelper", PointerValue (proseHelper));

  //Configure Timers
  //High timers to support all SL periods and maintain connection during simulation time
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4108", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4101", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4102", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4103", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayTRUIR", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4103", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4100", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4111", UintegerValue (simTime * 1000));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

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

  NodeContainer regularUeNodes;
  regularUeNodes.Create (nRegularUes);

  NodeContainer relayUeNodes;
  relayUeNodes.Create (nRelayUes);

  NodeContainer remoteUeNodes;
  remoteUeNodes.Create (nRemoteUes);


  for (uint32_t ry = 0; ry < regularUeNodes.GetN (); ry++)
    {
      NS_LOG_INFO ("Regular UE " << ry + 1 << " node id = [" << regularUeNodes.Get (ry)->GetId () << "]");
    }
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ry++)
    {
      NS_LOG_INFO ("Relay UE " << ry + 1 << " node id = [" << relayUeNodes.Get (ry)->GetId () << "]");
    }
  for (uint32_t rm = 0; rm < remoteUeNodes.GetN (); rm++)
    {
      NS_LOG_INFO ("Remote UE " << rm + 1 << " node id = [" << remoteUeNodes.Get (rm)->GetId () << "]");
    }
  NodeContainer allUeNodes = NodeContainer (relayUeNodes,remoteUeNodes);
  allUeNodes.Add (regularUeNodes);

  //Position of the nodes
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));

  //Regular UEs
  Ptr<ListPositionAllocator> positionAllocRegularUes = CreateObject<ListPositionAllocator> ();
  for (uint32_t ry = 0; ry < regularUeNodes.GetN (); ++ry)
    {
      positionAllocRegularUes->Add (Vector (regularUeInitXPos, 0.0, 1.5));
    }
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

  //Regular UEs
  MobilityHelper mobilityRegularUes;
  mobilityRegularUes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityRegularUes.SetPositionAllocator (positionAllocRegularUes);
  mobilityRegularUes.Install (regularUeNodes);

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
  NetDeviceContainer regularUeDevs = lteHelper->InstallUeDevice (regularUeNodes);
  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));
  NetDeviceContainer relayUeDevs = lteHelper->InstallUeDevice (relayUeNodes);
  NetDeviceContainer remoteUeDevs = lteHelper->InstallUeDevice (remoteUeNodes);

  NetDeviceContainer allUeDevs = NetDeviceContainer (relayUeDevs, remoteUeDevs);
  allUeDevs.Add (regularUeDevs);

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
  std::stringstream pstream;
  pstream << "sf" << slPeriod;
  pfactory.SetControlPeriod (pstream.str ());

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
  //-Relay UE (re)selection
  preconfiguration.preconfigRelay.haveReselectionInfoOoc = true;
  preconfiguration.preconfigRelay.reselectionInfoOoc.filterCoefficient = 0;
  preconfiguration.preconfigRelay.reselectionInfoOoc.minHyst = 0;
  preconfiguration.preconfigRelay.reselectionInfoOoc.qRxLevMin = -200;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  ueSidelinkConfiguration->SetDiscEnabled (true);
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  lteHelper->InstallSidelinkConfiguration (relayUeDevs, ueSidelinkConfiguration);
  lteHelper->InstallSidelinkConfiguration (remoteUeDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign IP address
  internet.Install (regularUeNodes);
  internet.Install (relayUeNodes);
  internet.Install (remoteUeNodes);

  Ipv6InterfaceContainer ueIpIfaceRegularUes;
  Ipv6InterfaceContainer ueIpIfaceRelays;
  Ipv6InterfaceContainer ueIpIfaceRemotes;
  ueIpIfaceRegularUes = epcHelper->AssignUeIpv6Address (regularUeDevs);
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


  //This is needed for the downward traffic
  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo ("7777:f00e::", Ipv6Prefix (48), Ipv6Address ("::"), 1, 0);

  //Attach each UE to the best available eNB
  lteHelper->Attach (allUeDevs);

  for (uint32_t ry = 0; ry < regularUeDevs.GetN (); ry++)
    {
      NS_LOG_INFO ("Regular UE " << ry + 1 << " IMSI = [" << regularUeDevs.Get (ry)->GetObject<LteUeNetDevice> ()->GetRrc ()->GetImsi () << "]");
    }
  for (uint32_t ry = 0; ry < relayUeDevs.GetN (); ry++)
    {
      NS_LOG_INFO ("Relay UE " << ry + 1 << " IMSI = [" << relayUeDevs.Get (ry)->GetObject<LteUeNetDevice> ()->GetRrc ()->GetImsi () << "]");
    }
  for (uint32_t rm = 0; rm < remoteUeDevs.GetN (); rm++)
    {
      NS_LOG_INFO ("Remote UE " << rm + 1 << " IMSI = [" << remoteUeDevs.Get (rm)->GetObject<LteUeNetDevice> ()->GetRrc ()->GetImsi () << "]");
    }

  AsciiTraceHelper ascii;

  ///*** Configure applications ***///

  // interface 0 is localhost, 1 is the p2p device
  Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
  ApplicationContainer onoffClientApps;
  ApplicationContainer udpClientApps;

  //Group Echo Server in the Remote Host
  uint16_t serverPort = 9;
  uint16_t echoPort = 1000;

  UdpGroupEchoServerHelper echoServer (serverPort);
  echoServer.SetAttribute ("Timeout", TimeValue (timeout));
  echoServer.SetAttribute ("Mode", StringValue (mode));
  echoServer.SetAttribute ("EchoClient", BooleanValue (echoClient));
  echoServer.SetAttribute ("EchoPort", UintegerValue (echoPort));

  ApplicationContainer serverApps = echoServer.Install (remoteHost);
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (simTime));

  // UDP clients to register nodes
  UdpClientHelper clientUdpHelper (remoteHostAddr, serverPort);
  clientUdpHelper.SetAttribute ("MaxPackets", UintegerValue (1));
  clientUdpHelper.SetAttribute ("PacketSize", UintegerValue (12));
  ApplicationContainer regularClientUdpApps = clientUdpHelper.Install (regularUeNodes);
  regularClientUdpApps.Start (Seconds (2.0));
  regularClientUdpApps.Stop (Seconds (simTime));

  ApplicationContainer relayClientUdpApps = clientUdpHelper.Install (relayUeNodes);
  relayClientUdpApps.Start (Seconds (2.0));
  relayClientUdpApps.Stop (Seconds (simTime));


  ApplicationContainer remoteClientUdpApps = clientUdpHelper.Install (remoteUeNodes);
  for (uint32_t clientIdx = 0; clientIdx < remoteClientUdpApps.GetN (); clientIdx++)
    {
      remoteClientUdpApps.Get (clientIdx)->SetStartTime (Seconds (8.0 + (10.0 * slPeriod / 1000.0) * clientIdx));
    }
  remoteClientUdpApps.Stop (Seconds (simTime));


  // On-Off Clients to generate the traffic towards the remote host (echo server)
  // Exponential Random Variables: ON(75% with mean 2.5s), OFF(25%)
  Ptr<ExponentialRandomVariable> onRv = CreateObject<ExponentialRandomVariable> ();
  onRv->SetAttribute ("Mean", DoubleValue (2.5));
  Ptr<ExponentialRandomVariable> offRv = CreateObject<ExponentialRandomVariable> ();
  offRv->SetAttribute ("Mean", DoubleValue (0.8333));

  OnOffHelper clientOnOffHelper ("ns3::UdpSocketFactory", Address (Inet6SocketAddress (remoteHostAddr, serverPort)));
  clientOnOffHelper.SetAttribute ("PacketSize", UintegerValue (41)); //Bytes
  clientOnOffHelper.SetAttribute ("DataRate", DataRateValue (16400)); //bps
  clientOnOffHelper.SetAttribute ("OnTime", PointerValue (onRv));
  clientOnOffHelper.SetAttribute ("OffTime", PointerValue (offRv));

  ApplicationContainer regularUeOnOffApps = clientOnOffHelper.Install (regularUeNodes);
  ApplicationContainer relayUeOnOffApps = clientOnOffHelper.Install (relayUeNodes);
  ApplicationContainer remoteUeOnOffApps = clientOnOffHelper.Install (remoteUeNodes);

  ApplicationContainer clientOnOffApps;
  clientOnOffApps.Add (regularUeOnOffApps);
  clientOnOffApps.Add (relayUeOnOffApps);
  clientOnOffApps.Add (remoteUeOnOffApps);

  clientOnOffApps.Start (Seconds (simTime)); // Modified below for the transmitter
  clientOnOffApps.Stop (Seconds (simTime - 1.0)); // To ensure we receive the packets in simulation time

  std::ostringstream oss;
  Ptr<OutputStreamWrapper> packetTraceStream = ascii.CreateFileStream ("AppPacketTrace.txt");
  *packetTraceStream->GetStream () << "time(sec)\ttx/rx\tnodeType\tNodeID\tPktSize(bytes)\tPktSeq\tDelay(ms)" << std::endl;

  // Start only the transmitter of the desired type (txType)
  if (txType.compare ("Regular") == 0)
    {
      regularUeOnOffApps.Get (0)->SetStartTime (Seconds (appStartTime));

      oss << "tx\tRegular\t" << regularUeOnOffApps.Get (0)->GetNode ()->GetId ();
      regularUeOnOffApps.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      regularClientUdpApps.Get (0)->SetStartTime (Seconds (simTime)); //do not pre-register sender node
    }
  else if (txType.compare ("Relay") == 0)
    {
      relayUeOnOffApps.Get (0)->SetStartTime (Seconds (appStartTime));

      oss << "tx\tRelay\t" << relayUeOnOffApps.Get (0)->GetNode ()->GetId ();
      relayUeOnOffApps.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      relayClientUdpApps.Get (0)->SetStartTime (Seconds (simTime)); //do not pre-register sender node
    }
  else if (txType.compare ("Remote") == 0)
    {
      remoteUeOnOffApps.Get (0)->SetStartTime (Seconds (appStartTime));

      oss << "tx\tRemote\t" << remoteUeOnOffApps.Get (0)->GetNode ()->GetId ();
      remoteUeOnOffApps.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      remoteClientUdpApps.Get (0)->SetStartTime (Seconds (simTime)); //do not pre-register sender node
    }
  else
    {
      NS_FATAL_ERROR ("Invalid tx type " << txType << ". Use Regular, Relay, or Remote");
      return 1;
    }
  oss.str ("");

  //UDP server to consume the packets
  UdpServerHelper serverUdpHelper (echoPort);
  ApplicationContainer regularServerUdpApps = serverUdpHelper.Install (regularUeNodes);
  ApplicationContainer remoteServerUdpApps = serverUdpHelper.Install (remoteUeNodes);
  ApplicationContainer relayServerUdpApps = serverUdpHelper.Install (relayUeNodes);

  ApplicationContainer serverUdpApps;
  serverUdpApps.Add (regularServerUdpApps);
  serverUdpApps.Add (remoteServerUdpApps);
  serverUdpApps.Add (relayServerUdpApps);

  serverUdpApps.Start (Seconds (2.0));
  serverUdpApps.Stop (Seconds (simTime));

  for (uint32_t serverIdx = 0; serverIdx < regularServerUdpApps.GetN (); serverIdx++)
    {
      oss << "rx\tRegular\t" << regularServerUdpApps.Get (serverIdx)->GetNode ()->GetId ();
      regularServerUdpApps.Get (serverIdx)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      oss.str ("");
    }
  for (uint32_t serverIdx = 0; serverIdx < remoteServerUdpApps.GetN (); serverIdx++)
    {
      oss << "rx\tRemote\t" << remoteServerUdpApps.Get (serverIdx)->GetNode ()->GetId ();
      remoteServerUdpApps.Get (serverIdx)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      oss.str ("");
    }
  for (uint32_t serverIdx = 0; serverIdx < relayServerUdpApps.GetN (); serverIdx++)
    {
      oss << "rx\tRelay\t" << relayServerUdpApps.Get (serverIdx)->GetNode ()->GetId ();
      relayServerUdpApps.Get (serverIdx)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetTraceStream));
      oss.str ("");
    }
  ///*** End of application configuration ***///

  ///*** Configure Relaying ***///
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
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, relayUeDevs.Get (ryDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
    }
  for (uint32_t rmDevIdx = 0; rmDevIdx < remoteUeDevs.GetN (); rmDevIdx++)
    {
      Simulator::Schedule ((Seconds (4.0 + (10.0 * slPeriod / 1000.0) * rmDevIdx)), &LteSidelinkHelper::StartRelayService, proseHelper, remoteUeDevs.Get (rmDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
    }
  ///*** End of Configure Relaying ***///

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();

  Simulator::Destroy ();
  return 0;

}
