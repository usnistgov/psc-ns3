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
#include "ns3/lte-sl-pool.h"
#include "lte-sl-ue-controller-camad-2019.h"
#include <cfloat>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("camad-2019-discovery");


//Trace sink function for logging when a Remote UE discovers all the Relays
//to connect to the Trace 'DiscoversAllRelays' of the 'LteSlUeControllerCamad2019'
void TraceSinkARemoteDiscoversAllRelays (Ptr<OutputStreamWrapper> stream, uint32_t remoteId, uint32_t lastRelayId, double timeToDisc, uint32_t nPeriodstoDisc)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << remoteId << "\t" << lastRelayId
                        << "\t" << timeToDisc << "\t" << nPeriodstoDisc << std::endl;

}

/*
 * This scenario is used to evaluate the UE-to-Network Relay discovery protocol
 * depending on:
 * - the number of Relay UEs (nRelayUes) and Remote UEs (nRemoteUes) in the scenario,
 * - the discovery model (discModel), and
 * - the discovery pool configuration (discPrbNum, nSubframes, and txProb).
 */
int main (int argc, char *argv[])
{
  double startRelayTime = 2.0; //Seconds
  double startRemoteTime = 4.0; //Seconds
  double relayUeInitXPos = 300.0;
  double remoteUeInitXPos = 500.0;

  double simTime = 10.0; // Seconds
  uint32_t nRelayUes = 1;
  uint32_t nRemoteUes = 1;
  std::string discModel = "A";
  uint32_t discPrbNum = 2;
  uint32_t nSubframes = 5;
  uint32_t txProb = 100; // Percentage

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation [s]", simTime);
  cmd.AddValue ("nRelayUes", "Number of relay UEs --> N_y", nRelayUes);
  cmd.AddValue ("nRemoteUes", "Number of remote UEs --> N_x", nRemoteUes);
  cmd.AddValue ("discModel", "Discovery Model to be used (A or B)", discModel);
  cmd.AddValue ("discPrbNum", "Number of discovery resources per subframe in the discovery pool --> N_f", discPrbNum);
  cmd.AddValue ("nSubframes", "Number of subframes in bitmap of the discovery pool --> N_t", nSubframes);
  cmd.AddValue ("txProb", "Discovery transmission probability", txProb);
  cmd.Parse (argc, argv);

  //Set the frequencies
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

  // Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  // Set the SRS periodicity in ms (increase the capability of handling more UEs per cell)
  // Allowed values : 2, 5, 10, 20, 40, 80, 160 and 320
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (80));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

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
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::Campaign", StringValue ("Discovery"));
  Config::SetDefault ("ns3::LteSlUeControllerCamad2019::ProseHelper", PointerValue (proseHelper));

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
  pDiscFactory.SetDiscPrbNum (discPrbNum);
  pDiscFactory.SetDiscPrbStart (10);
  pDiscFactory.SetDiscPrbEnd (40);
  pDiscFactory.SetDiscOffset (0);
  pDiscFactory.SetDiscBitmap ( (pow (2,nSubframes) - 1) );

  std::stringstream txProbStream;
  txProbStream << "p" << txProb;
  pDiscFactory.SetDiscTxProbability (txProbStream.str ());

  discPool.ueSelected.poolToAddModList.pools[0].pool =  pDiscFactory.CreatePool ();

  //Get the discovery period in uint32_t form (used for the controller configuration below)
  uint32_t discPeriodInt = LteRrcSap::DiscPeriodAsInt (discPool.ueSelected.poolToAddModList.pools[0].pool.discPeriod);

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

  ///*** Configure Relaying ***///
  Ptr<EpcTft> tft = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localIpv6Address.Set ("7777:f00e::");
  dlpf.localIpv6Prefix = Ipv6Prefix (32);
  tft->Add (dlpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (relayUeDevs, bearer, tft);
  proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (48));

  //Relays
  for (uint32_t ryDevIdx = 0; ryDevIdx < relayUeDevs.GetN (); ryDevIdx++)
    {
      if (discModel == "A")
        {
          Simulator::Schedule (Seconds (startRelayTime), &LteSidelinkHelper::StartRelayService, proseHelper,
                               relayUeDevs.Get (ryDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
        }
      else if (discModel == "B")
        {
          Simulator::Schedule (Seconds (startRelayTime), &LteSidelinkHelper::StartRelayService, proseHelper,
                               relayUeDevs.Get (ryDevIdx), 33, LteSlUeRrc::ModelB, LteSlUeRrc::RelayUE);
        }
      else
        {
          NS_FATAL_ERROR ( " Discovery Model " << discModel << " not recognized");
        }
    }
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> aRemoteDiscoversAllRelaysStream = ascii.CreateFileStream ("TimesEachRemoteDiscoversAllRelays.txt");
  *aRemoteDiscoversAllRelaysStream->GetStream () << "time(s)\tremoteId\tlastRelayId\ttimeToDisc(s)\tnPeriodstoDisc" << std::endl;

  //Remotes
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  for (uint32_t rmDevIdx = 0; rmDevIdx < remoteUeDevs.GetN (); rmDevIdx++)
    {
      //Set the corresponding values in the controller for this campaign
      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (rmDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      PointerValue ptrTwo;
      slUeRrc->GetAttribute ("SlController", ptrTwo);
      Ptr<LteSlUeControllerCamad2019> ctlr = ptrTwo.Get<LteSlUeControllerCamad2019>();

      //Randomize the start of the relay service for the Remote UEs by one discovery period ( 0.320 s)
      double startRemoteTimeToUse = startRemoteTime;
      if (x->GetValue (0.0, 1.0) < 0.5)
        {
          startRemoteTimeToUse = startRemoteTime + 0.320;
        }

      ctlr->SetStartRelayServiceTime (Seconds (startRemoteTime));
      ctlr->SetDiscoveryPeriodDuration (discPeriodInt);
      ctlr->SetNumberProseRelaysToDiscover (nRelayUes);
      ctlr->TraceConnectWithoutContext ("DiscoversAllRelays",
                                        MakeBoundCallback (&TraceSinkARemoteDiscoversAllRelays,
                                                           aRemoteDiscoversAllRelaysStream));
      if (discModel == "A")
        {
          Simulator::Schedule (Seconds (startRemoteTimeToUse), &LteSidelinkHelper::StartRelayService, proseHelper,
                               remoteUeDevs.Get (rmDevIdx), 33, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
        }
      else if  (discModel == "B")
        {
          Simulator::Schedule (Seconds (startRemoteTimeToUse), &LteSidelinkHelper::StartRelayService, proseHelper,
                               remoteUeDevs.Get (rmDevIdx), 33, LteSlUeRrc::ModelB, LteSlUeRrc::RemoteUE);
        }
      else
        {
          NS_FATAL_ERROR (" Discovery Model not recognized");
        }
    }
  ///*** End of Configure Relaying ***///


  lteHelper->EnableSlPsdchMacTraces ();

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
