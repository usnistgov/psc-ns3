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

NS_LOG_COMPONENT_DEFINE ("camad-2019-connection");


//Trace sink function for logging when a UE receives a PC5 signaling packet
//to connect to the Trace 'PC5SignalingPacketTrace' of 'LteSlUeRrc'
void
TraceSinkPC5SignalingPacketTrace (Ptr<OutputStreamWrapper> stream, uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p)
{
  LteSlPc5SignallingMessageType lpc5smt;
  p->PeekHeader (lpc5smt);

  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << (uint32_t)lpc5smt.GetMessageType () << std::endl;
}

//Trace sink function for logging when a Remote UE starts the one-to-one connection procedure
//to connect to the Trace 'RemoteConnectionStart' of the 'LteSlUeControllerCamad2019'
void
TraceSinkRemoteConnectionStart (Ptr<OutputStreamWrapper> stream,  uint32_t remoteId, uint32_t relayId, uint32_t connectionCount)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << remoteId << "\t" << relayId << "\t" << "S" << "\t" << connectionCount
                        << std::endl;
}

//Trace sink function for logging when a Remote UE completes successfully the one-to-one connection procedure
//to connect to the Trace 'RemoteConnectionEstablished' of the 'LteSlUeControllerCamad2019'
void
TraceSinkRemoteConnectionEstablished (Ptr<OutputStreamWrapper> stream, uint32_t remoteId, uint32_t relayId, uint32_t connectionCount, double timeToConnect)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << remoteId << "\t" << relayId << "\t" << "E" << "\t" << connectionCount
                        << "\t" << timeToConnect << std::endl;
}

/*
 * This scenario is used to evaluate the UE-to-Network Relay one-to-one
 * connection protocol.
 * Parameters:
 * - the number of Remote UEs (nRemoteUes) in the scenario,
 * - the value of the timer T4100 controlling Direct Communication Requests
 * retransmissions for the Remote UE (remoteT4100),
 * - the maximum value of Direct Communication Requests retransmissions for the
 * Remote UE (remoteDCRqMax),
 * - whether the Relay UE has its own UL traffic or not (relayTraffic)
 * - the frequency of the UL Relay UE traffic if aplicable (relayTrafficOffMean)
 */

int main (int argc, char *argv[])
{
  double simTime = 10.0; //s
  double relayUeInitXPos = 800.0;
  double remoteUeInitXPos = 1000.0;
  uint32_t nRelayUes = 1;


  uint32_t nRemoteUes = 1;
  bool relayTraffic = false;
  double relayTrafficOffMean = 20;
  double maxConnectionAttempts = 1;

  double remoteT4100 = 200; //ms
  double remoteDCRqMax = 4;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation [s]", simTime);

  cmd.AddValue ("nRemoteUes", "Number of remote UEs", nRemoteUes);

  cmd.AddValue ("remoteT4100", "Remote UE retransmission timer T4100 (ms)", remoteT4100);
  cmd.AddValue ("remoteDCRqMax", "Maximum Value of Direct Communication Requests retransmissions for Remote UE", remoteDCRqMax);

  cmd.AddValue ("relayTraffic", "The Relay UE has its own traffic", relayTraffic);
  cmd.AddValue ("relayTrafficOffMean", "The mean of the distribution of the 'Off' periods for the Relay UE traffic (if set to 100 no traffic will be generated, regardless of relayTraffic flag)", relayTrafficOffMean);


  cmd.Parse (argc, argv);

  //relayTrafficOffMean=100 is the indication of no traffic
  if (relayTrafficOffMean == 100)
    {
      relayTraffic = false;
    }

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
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::Campaign", StringValue ("Connection"));
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::MaxConnectionAttempts", DoubleValue (maxConnectionAttempts));
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::ProseHelper", PointerValue (proseHelper));


  //Configure Timers
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4100", UintegerValue (remoteT4100));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteDcrqMaximum", UintegerValue (remoteDCRqMax));

  //High timers to maintain connection during simulation time
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4111", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4108", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4101", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4102", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4103", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayTRUIR", UintegerValue (simTime * 1000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4103", UintegerValue (simTime * 1000));


  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  //Internet configuration
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);
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

  //Configure Sidelink on the eNB
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);

  //Configure Communication pool
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

  //Configure Discovery pool
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

  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Configure Sidelink Preconfiguration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  ueSidelinkConfiguration->SetDiscEnabled (true);
  LteRrcSap::SlPreconfiguration preconfiguration;
  //-Relay UE (re)selection
  preconfiguration.preconfigRelay.haveReselectionInfoOoc = true;
  preconfiguration.preconfigRelay.reselectionInfoOoc.filterCoefficient = 0;
  preconfiguration.preconfigRelay.reselectionInfoOoc.minHyst = 0;
  preconfiguration.preconfigRelay.reselectionInfoOoc.qRxLevMin = -200;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  lteHelper->InstallSidelinkConfiguration (allUeDevs, ueSidelinkConfiguration);

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

  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo ("7777:f00e::", Ipv6Prefix (48), Ipv6Address ("::"), 1, 0);

  //Attach each UE to the best available eNB
  lteHelper->Attach (allUeDevs);

  AsciiTraceHelper ascii;

  ///*** Configure applications ***///

  // interface 0 is localhost, 1 is the p2p device
  Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
  uint16_t echoPortBase = 50000;
  ApplicationContainer relayServerApps;
  ApplicationContainer remoteServerApps;

  ApplicationContainer relayClientApps;
  ApplicationContainer remoteClientApps;

  //For each Relay UE, we have a pair (OnOffApplication, PacketSink)
  //Each UE has an assigned port
  //OnOffApplication installed in the UE, sending to the remoteHost address in the UE port
  //PacketSink installed in the remoteHost, listening to the UE port, consuming packets
  if (relayTraffic)
    {
      Ptr<ConstantRandomVariable> onRv = CreateObject<ConstantRandomVariable> ();
      onRv->SetAttribute ("Constant", DoubleValue (1 / 1000.0)); //Seconds

      Ptr<NormalRandomVariable> offRv = CreateObject<NormalRandomVariable> ();
      offRv->SetAttribute ("Mean", DoubleValue (relayTrafficOffMean / 1000.0)); //Seconds
      //Variance=0.00001 gives a 4 ms 90 th percentile variation
      // e.g., With Mean=0.020, the 90th percentile is 0.024 --> values in [16 ms, 24 ms]
      offRv->SetAttribute ("Variance", DoubleValue (0.00001));
      //Bound = Mean/2, i.e., values are in [Mean - Mean/2; Mean + Mean/2]
      // e.g., Mean = 0.020 we will have values in [10 ms, 30 ms]
      offRv->SetAttribute ("Bound", DoubleValue ((relayTrafficOffMean / 2) / 1000.0));

      for (uint16_t relUeIdx = 0; relUeIdx < relayUeNodes.GetN (); relUeIdx++)
        {
          uint16_t relUePort = echoPortBase + 100 + relUeIdx;

          //Packet Sink in the remoteHost
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (remoteHostAddr, relUePort));
          ApplicationContainer singleServerApp = packetSinkHelper.Install (remoteHost);
          singleServerApp.Start (Seconds (1.0));
          singleServerApp.Stop (Seconds (simTime));
          relayServerApps.Add (singleServerApp);

          //On-off application on the Relay UE
          OnOffHelper onOffHelper ("ns3::UdpSocketFactory",
                                   Address ( Inet6SocketAddress (remoteHostAddr, relUePort)));
          //Values chosen to generate a packet the next ms after the start of an "on" period
          // nextTime=(100*8)/800000 = 0.001 s
          onOffHelper.SetAttribute ("PacketSize", UintegerValue (100)); //Bytes
          onOffHelper.SetAttribute ("DataRate", DataRateValue (800000)); // bit/s

          onOffHelper.SetAttribute ("OnTime", PointerValue (onRv));
          onOffHelper.SetAttribute ("OffTime", PointerValue (offRv));
          ApplicationContainer singleClientApp = onOffHelper.Install (relayUeNodes.Get (relUeIdx));
          singleClientApp.Start (Seconds (3.0));
          singleClientApp.Stop (Seconds (simTime));

          relayClientApps.Add (singleClientApp);
        }
    }
  ///*** End of application configuration ***///


  ///*** Configure Relaying ***///
  Ptr<OutputStreamWrapper> PC5SignalingPacketTraceStream = ascii.CreateFileStream ("PC5SignalingPacketTrace.txt");
  *PC5SignalingPacketTraceStream->GetStream () << "time(s)\ttxId\tRxId\tmsgType" << std::endl;
  Ptr<OutputStreamWrapper> remoteConnectionEventsStream = ascii.CreateFileStream ("remoteConnectionEventsTrace.txt");
  *remoteConnectionEventsStream->GetStream () << "time(s)\tremoteId\trelayId\teventType\tconnId\tconnTime" << std::endl;

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

      Simulator::Schedule (Seconds (2.0 ), &LteSidelinkHelper::StartRelayService, proseHelper, relayUeDevs.Get (ryDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
    }
  for (uint32_t rmDevIdx = 0; rmDevIdx < remoteUeDevs.GetN (); rmDevIdx++)
    {
      Simulator::Schedule ((Seconds (4.0 )), &LteSidelinkHelper::StartRelayService, proseHelper, remoteUeDevs.Get (rmDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);

      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (rmDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      PointerValue ptrTwo;
      slUeRrc->GetAttribute ("SlController", ptrTwo);
      Ptr<LteSlUeControllerCamad2019> ctlr = ptrTwo.Get<LteSlUeControllerCamad2019>();

      ctlr->TraceConnectWithoutContext ("RemoteConnectionStart",
                                        MakeBoundCallback (&TraceSinkRemoteConnectionStart,
                                                           remoteConnectionEventsStream));
      ctlr->TraceConnectWithoutContext ("RemoteConnectionEstablished",
                                        MakeBoundCallback (&TraceSinkRemoteConnectionEstablished,
                                                           remoteConnectionEventsStream));
    }

  for (uint32_t ueDevIdx = 0; ueDevIdx < allUeDevs.GetN (); ueDevIdx++)
    {
      Ptr<LteUeRrc> rrc = allUeDevs.Get (ueDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      slUeRrc->TraceConnectWithoutContext ("PC5SignalingPacketTrace",
                                           MakeBoundCallback (&TraceSinkPC5SignalingPacketTrace,
                                                              PC5SignalingPacketTraceStream));
    }
  ///*** End of Configure Relaying ***///

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}
