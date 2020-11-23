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
#include "ns3/config-store.h"
#include <cfloat>
#include <sstream>
#include <math.h>
#include "ns3/gnuplot.h"
#include "ns3/udp-echo-client.h"
#include <ns3/buildings-helper.h>
#include <ns3/buildings-module.h>

using namespace ns3;

/*
 * This scenario show the activation of UE-to-Network Relay service on the
 * Remote UE depending on the RSRP measurements of the primary cell.
 *
 * The topology:
 *                                                        .-------------.  -
 *                                                        |             |  |
 *                             Relay         Remote       |  Building   | 200 m
 *    eNodeB                    UE             UE         |             |  |
 *                                                        '-------------'  -
 *     |------------------------||-------------||--------||-------------|
 *                400 m               50 m         10 m          200 m
 *
 *                       Remote UE mobility:    >--------->-------->
 *                       Approx. time:          |---10 s--||--5 s--|
 *
 *                       Application ON time:        |----10 s-----|
 *
 * The Remote UE moves towards  the building at a constant speed of 1 m/s.
 * crossing the 'edge' of the building at approximately 10.0 s.
 * The Remote UE starts its application at 5.0 s sending one packet every 0.5 s,
 * and stops at 15.0 s, for a total of 20 sent packets towards a server in the
 * network. The server echoes back the packets to the Remote UE and we trace
 * those received packets.
 *
 * Two parameters can be configured on the command line:
 * - useRelay
 * - threshold.
 * When 'useRelay' is set to false the Remote UE does not use the Relay UE,
 * i.e., it uses the network directly and its service degrades when entering
 * into the building due to channel degradation, which is reflected in packet
 * loss.
 * When 'useRelay' is set to true, the Remote UE activates the UE-to-Network
 * Relay service when the RSRP of the primary cell falls below 'threshold'.
 * Depending on the value of 'threshold' we see that the packet loss could be
 * mitigated with the use of the UE-to-Network Relay UE in this scenario.
 *
 * The total number of received packets are displayed using the standard output
 * upon completion of the simulation.
 *
 * Given the default configuration in this script, the following are example
 * values for the parameters and the expected output
 * .----------------------------.-------------.
 * |        Parameters          |   Output    |
 * |----------.-----------------.-------------|
 * | useRelay | threshold (dBm) | Rx Packets  |
 * |----------.-----------------.-------------|
 * | false    | -100 (not used) |   10/20     | // Remote UE does not activate Relay Service
 * |----------.-----------------.-------------|
 * | true     | -100            |   20/20     | // Remote UE activates Relay Service at the beginning of the simulation (the RSRP is already below -100 dBm in the initial position)
 * |----------.-----------------.-------------|
 * | true     | -120            |   16/20     | // Remote UE activates Relay Service when 'entering' to the building (the RSRP decays below -120 dBm when entering to the building, i.e., after 10 s).
 * |----------.-----------------.-------------|
 * | true     | -127            |   12/20     | // Remote UE activates Relay Service when inside of the building (the RSRP decays below -127 dBm after 12 s).
 * '------------------------------------------'
 *
 * Additionally, users can inspect the generated output files to gain more
 * insights about the simulation:
 * - AppPacketTrace.txt: Log of received packets by the Remote UE at the
 *                       application layer
 * - MobilityTrace.txt: Log of the position and velocity of the Remote UE
 * - PC5SignalingPacketTrace.txt: Log of the received PC5 signaling messages by
 *                                both UEs
 * - RsrpMeasurementTrace.txt: Log of primary cell RSRP monitoring by the
 *                             Remote UE
 * - UlMacStats.txt: MAC layer statistics for the UL
 * - UlPdcpStats.txt PDCP layer statistics for the UL
 * - DlMacStats.txt: MAC layer statistics for the DL
 * - DlPdcpStats.txt: PDCP layer statistics for the DL
 * E.g., users can follow the movement of the Remote UE in 'MobilityTrace.txt',
 * and see when the RSRP crosses the 'threshold' in 'RsrpMeasurementTrace.txt'.
 * Then, observe that the one-to-one communication setup procedure was
 * performed between the Remote UE and the Relay UE by checking the
 * corresponding PC5 message exchange in 'PC5SignalingPacketTrace.txt'.
 * Finally, users can verify which UE is using the network (UL/DL) to send and
 * receive packets at any given point of the simulation in the last four listed
 * output files. When 'useRelay' parameter is set to true, the Remote UE uses
 * the network before connection to the Relay UE. After connection, the Relay UE
 * is the one using the network to transmit and receive the packets that it
 * is relaying for the Remote UE.
 */


NS_LOG_COMPONENT_DEFINE ("lte-sl-in-covrg-relay-building");

uint32_t SERVICE_CODE = 33;
int TX_PACKETS = 20;

int RX_PACKETS = 0; //Used to trace the number of Rx packets


/*
 * Trace sink function for logging when a packet is transmitted or received
 * at the application layer
 */
void
UePacketTrace (Ptr<OutputStreamWrapper> stream, std::string context, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << context << "\t"
                        << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":"
                        << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t"
                        << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":"
                        << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << "\t"
                        << p->GetSize () << "\t"
                        << std::endl;
  RX_PACKETS++;
}

/*
 * Trace sink function to monitor primary cell RSRP and activate UE-to-Network
 * Relay service as Remote UE when the RSRP falls below a threshold
 */
void
RsrpMeasurementTrace (Ptr<OutputStreamWrapper> stream, const double threshold, Ptr<NetDevice> ueDevice, std::string context, uint64_t imsi, uint16_t cellId, double rsrp)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);
  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << imsi << "\t"
                        << cellId << "\t"
                        << rsrp << "\t"
                        << threshold << "\t"
                        << std::endl;

  Ptr<LteUeRrc> rrc = ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ();
  PointerValue ptrOne;
  rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
  Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();

  //Avoid that in-coverage Remote UE starts Relay service before cell
  //attachment (happening at the first RSRP report: 200 ms)

  if (Simulator::Now ().GetMilliSeconds () < 400)
    {
      return;
    }
  else
    {
      //If the RSRP of the primary cell is below the threshold and
      //the UE is not in Remote UE role already (i.e., already monitoring Discovery Relay Announcements (we are using discovery Model A))
      //then start UE-to-Network Relay Service as Remote UE
      if (rsrp < threshold && !slUeRrc->IsMonitoringRelayServiceCode (LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT, SERVICE_CODE))
        {
          rrc->StartRelayService (SERVICE_CODE, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
        }
    }
}

/*
 * Trace sink function for logging when PC5 signaling messages are received
 */
void
TraceSinkPC5SignalingPacketTrace (Ptr<OutputStreamWrapper> stream, uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p)
{
  LteSlPc5SignallingMessageType lpc5smt;
  p->PeekHeader (lpc5smt);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << lpc5smt.GetMessageName () << std::endl;
}


int main (int argc, char *argv[])
{

  double simTime = 1.0; //s //Simulation time (in seconds) updated automatically based on number of nodes
  double relayRadius = 400.0; //m
  double remoteRadius = 50.0; //m
  uint32_t nRelayUes = 1;
  uint32_t nRemoteUesPerRelay = 1;
  bool remoteUeMobility = true;
  bool useRelay = true;
  double threshold = -120; //dBm

  CommandLine cmd;

  cmd.AddValue ("useRelay", "The Remote UEs will use Relay Service", useRelay);
  cmd.AddValue ("threshold", "The RSRP value (dBm) for which the Remote UE will start Relay Service", threshold);

  cmd.Parse (argc, argv);

  //Set the start time of the relay service for the Relay UE and
  //the start of the application for the Remote UE
  double startTimeRelay = 2.0; //s
  double startTimeRemoteApp = 5.0; //s

  //Calculate simTime to give 10 s of traffic for the Remote UE
  simTime = startTimeRemoteApp + 10.0; //s

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

  //Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

  //Create the lte helper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //Create and set the EPC helper
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  //Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  //Connect Sidelink controller and Sidelink Helper
  Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper", PointerValue (proseHelper));

  //Configure Relay UE (re)selection algorithm (RandomNoReselection|MaxSDRsrpNoReselection|MaxSDRsrp)
  Config::SetDefault ("ns3::LteSlBasicUeController::RelayUeSelectionAlgorithm", StringValue ("MaxSDRsrpNoReselection"));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (1));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (1.5));
  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");
  lteHelper->SetPathlossModelAttribute ("Frequency", DoubleValue (700e6));

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
  relayUeNodes.Create (nRelayUes);

  NodeContainer remoteUeNodes;
  remoteUeNodes.Create (nRelayUes * nRemoteUesPerRelay);

  NodeContainer allUeNodes = NodeContainer (relayUeNodes,remoteUeNodes);

  //Position of the nodes
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  NS_LOG_INFO ("eNodeB 0 node id = [" << enbNode.Get (0)->GetId () << "]"
               " position: (" << 0.0 << " , " << 0.0 << ", " << 30.0 << ")");

  //UEs
  Ptr<ListPositionAllocator> positionAllocRelays = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> positionAllocRemotes = CreateObject<ListPositionAllocator> ();

  //Relay UE
  double ry_pos_x = relayRadius;
  double ry_pos_y = 0.0;
  positionAllocRelays->Add (Vector (ry_pos_x, ry_pos_y, 1.5));

  NS_LOG_INFO ("Relay UE node id = [" << relayUeNodes.Get (0)->GetId () << "]"
               " position: (" << ry_pos_x << " , " << ry_pos_y  << ", " << 1.5 << ")");

  //Remote UE
  double rm_pos_x = ry_pos_x + remoteRadius;
  double rm_pos_y = 0.0;

  positionAllocRemotes->Add (Vector (rm_pos_x, rm_pos_y, 1.5));

  NS_LOG_INFO ("Remote UE node id = [" << remoteUeNodes.Get (0)->GetId () << "]"
               " initial position: (" << rm_pos_x << " , " << rm_pos_y  << ", " << 1.5 << ")");

  //Position of the building
  double xMin = relayRadius + remoteRadius + 10;

  Ptr<Building> building1 = CreateObject<Building> ();
  building1->SetBoundaries (Box (xMin, xMin + 200, -100, 100, 0.0, 20.0));
  building1->SetBuildingType (Building::Residential);
  building1->SetExtWallsType (Building::ConcreteWithWindows);

  AsciiTraceHelper ascii;

  //Install mobility
  //eNodeB
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);
  BuildingsHelper::Install (enbNode);

  //Relay UEs
  MobilityHelper mobilityRelays;
  mobilityRelays.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityRelays.SetPositionAllocator (positionAllocRelays);
  mobilityRelays.Install (relayUeNodes);
  BuildingsHelper::Install (relayUeNodes);

  //Remote UEs
  MobilityHelper mobilityRemotes;
  if (!remoteUeMobility)
    {
      mobilityRemotes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobilityRemotes.SetPositionAllocator (positionAllocRemotes);
      mobilityRemotes.Install (remoteUeNodes);
    }
  else
    {
      mobilityRemotes.SetPositionAllocator (positionAllocRemotes);
      mobilityRemotes.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
      mobilityRemotes.Install (remoteUeNodes);
      BuildingsHelper::Install (remoteUeNodes);

      double speed = 1; // m/s
      double rm_vel_x = speed;
      double rm_vel_y = 0.0;
      remoteUeNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (rm_vel_x, rm_vel_y, 0.0));

      //Just for tracing purposes:
      //The function ConstantVelocityMobilityModel::SetVelocity calls
      //'NotifyCourseChange' function, which trace is connected in
      //MobilityHelper::EnableAsciiAll
      for (double t = 1.0; t < simTime; t++)
        {
          Simulator::Schedule (Seconds (t),
                               &ConstantVelocityMobilityModel::SetVelocity,
                               remoteUeNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> (), Vector (rm_vel_x, rm_vel_y, 0.0));
        }
      mobilityRemotes.EnableAscii (ascii.CreateFileStream ("MobilityTrace.txt"), remoteUeNodes);

    }

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer relayUeDevs = lteHelper->InstallUeDevice (relayUeNodes);
  NetDeviceContainer remoteUeDevs = lteHelper->InstallUeDevice (remoteUeNodes);
  NetDeviceContainer allUeDevs = NetDeviceContainer (relayUeDevs, remoteUeDevs);

  //Configure eNodeB for Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);

  //-Configure Sidelink communication pool
  enbSidelinkConfiguration->SetDefaultPool (proseHelper->GetDefaultSlCommTxResourcesSetupUeSelected ());

  //-Enable Sidelink discovery
  enbSidelinkConfiguration->SetDiscEnabled (true);

  //-Configure Sidelink discovery pool
  enbSidelinkConfiguration->AddDiscPool (proseHelper->GetDefaultSlDiscTxResourcesSetupUeSelected ());

  //-Configure UE-to-Network Relay parameters
  enbSidelinkConfiguration->SetDiscConfigRelay (proseHelper->GetDefaultSib19DiscConfigRelay ());

  //Install eNodeB Sidelink configuration on the eNodeB devices
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Preconfigure UEs for Sidelink
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  //-Set default Sidelink preconfiguration
  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  //-Enable Sidelink discovery
  ueSidelinkConfiguration->SetDiscEnabled (true);
  //-Set frequency for Sidelink discovery messages monitoring
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  //Install UE Sidelink configuration on the UE devices
  lteHelper->InstallSidelinkConfiguration (relayUeDevs, ueSidelinkConfiguration);
  lteHelper->InstallSidelinkConfiguration (remoteUeDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign network IP addresses
  internet.Install (relayUeNodes);
  internet.Install (remoteUeNodes);
  Ipv6InterfaceContainer ueIpIfaceRelays;
  Ipv6InterfaceContainer ueIpIfaceRemotes;
  ueIpIfaceRelays = epcHelper->AssignUeIpv6Address (relayUeDevs);
  ueIpIfaceRemotes = epcHelper->AssignUeIpv6Address (remoteUeDevs);

  //Set the default gateway for the UEs
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;
  for (uint32_t u = 0; u < allUeNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = allUeNodes.Get (u);
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  //Configure IP for the nodes in the Internet (PGW and RemoteHost)
  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);
  internetIpIfaces.SetForwarding (0, true);
  internetIpIfaces.SetDefaultRouteInAllNodes (0);

  //Set route for the Remote Host to join the LTE network nodes
  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
  remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);

  //Configure UE-to-Network Relay network
  proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (60));

  //Configure route between PGW and UE-to-Network Relay network
  proseHelper->ConfigurePgwToUeToNetworkRelayRoute (pgw);

  //Attach UEs to the eNB
  lteHelper->Attach (relayUeDevs);
  lteHelper->Attach (remoteUeDevs);

  ///*** Configure applications ***///
  //For each Remote UE, we have a pair (UpdEchoClient, UdpEchoServer)
  //Each Remote UE has an assigned port
  //UdpEchoClient installed in the Remote UE, sending to the echoServerAddr
  //in the corresponding Remote UE port
  //UdpEchoServer installed in the echoServerNode, listening to the
  //corresponding Remote UE port

  Ipv6Address echoServerAddr = internetIpIfaces.GetAddress (1, 1);
  uint16_t echoPortBase = 50000;

  ApplicationContainer serverApps;
  ApplicationContainer clientApps;

  uint16_t remUePort = echoPortBase + 1;
  UdpEchoServerHelper echoServerHelper (remUePort);
  ApplicationContainer singleServerApp;
  singleServerApp.Add (echoServerHelper.Install (remoteHost));
  singleServerApp.Start (Seconds (1.0));
  singleServerApp.Stop (Seconds (simTime));

  serverApps.Add (singleServerApp);

  //UdpEchoClient in the Remote UE
  UdpEchoClientHelper echoClientHelper (echoServerAddr);
  echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (TX_PACKETS));
  echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
  echoClientHelper.SetAttribute ("PacketSize", UintegerValue (150));
  echoClientHelper.SetAttribute ("RemotePort", UintegerValue (remUePort));

  ApplicationContainer singleClientApp = echoClientHelper.Install (remoteUeNodes.Get (0));
  singleClientApp.Start (Seconds (startTimeRemoteApp));
  NS_LOG_INFO ("Remote UE node id = [" << remoteUeNodes.Get (0)->GetId () << "] start application at " << startTimeRemoteApp << " s");

  //Stop the application after 10.0 s
  singleClientApp.Stop (Seconds (startTimeRemoteApp + 10.0));

  std::ostringstream oss;
  Ptr<OutputStreamWrapper> packetOutputStream = ascii.CreateFileStream ("AppPacketTrace.txt");
  *packetOutputStream->GetStream () << "time(sec)\ttx/rx\tC/S\tNodeID\tIP[src]\tIP[dst]\tPktSize(bytes)" << std::endl;

  //Tracing packets on the UdpEchoClient (C)
  //Tracing only the packets that are Rx back.
  oss << "rx\tC\t" << remoteUeNodes.Get (0)->GetId ();
  singleClientApp.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetOutputStream));
  oss.str ("");

  clientApps.Add (singleClientApp);

  ///*** Configure Relaying ***///
  if (useRelay)
    {
      //Setup dedicated bearer for the Relay UE
      Ptr<EpcTft> tft = Create<EpcTft> ();
      EpcTft::PacketFilter dlpf;
      dlpf.localIpv6Address = proseHelper->GetIpv6NetworkForRelayCommunication ();
      dlpf.localIpv6Prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
      tft->Add (dlpf);
      EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
      lteHelper->ActivateDedicatedEpsBearer (relayUeDevs, bearer, tft);

      //Schedule the start of the relay service in the Relay UE
      Simulator::Schedule (Seconds (startTimeRelay), &LteSidelinkHelper::StartRelayService,
                           proseHelper, relayUeDevs.Get (0), SERVICE_CODE,
                           LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
      NS_LOG_INFO ("Relay UE node id = [" << relayUeNodes.Get (0)->GetId () << "] provides Service Code " << SERVICE_CODE << " and start service at " << startTimeRelay << " s");

      //Connect trace in all UEs for monitoring UE-to-Network Relay signaling messages
      Ptr<OutputStreamWrapper> PC5SignalingPacketTraceStream = ascii.CreateFileStream ("PC5SignalingPacketTrace.txt");
      *PC5SignalingPacketTraceStream->GetStream () << "time(s)\ttxId\tRxId\tmsgType" << std::endl;
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

      //Connect trace in the Remote UE to monitor primary cell RSRP and activate
      //Relay service when it falls below 'threshold' (See 'RsrpMeasurementTrace' function)
      Ptr<OutputStreamWrapper> RsrpMeasurementsStream = ascii.CreateFileStream ("RsrpMeasurementTrace.txt");
      *RsrpMeasurementsStream->GetStream () << "time(s)\timsi\tcellId\trsrp\tthreshold" << std::endl;

      std::ostringstream ctx;
      ctx << remoteUeNodes.Get (0)->GetId ();
      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (0)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->TraceConnect ("RsrpMeasurement",  ctx.str (),
                         MakeBoundCallback (&RsrpMeasurementTrace,
                                            RsrpMeasurementsStream,
                                            threshold,
                                            remoteUeDevs.Get (0)->GetObject<LteUeNetDevice> ()));
    }

  NS_LOG_INFO ("Enabling traces...");

  lteHelper->EnablePdcpTraces ();
  lteHelper->EnableUlMacTraces ();
  lteHelper->EnableDlMacTraces ();

  NS_LOG_INFO ("Simulation time " << simTime << " s");
  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  Simulator::Destroy ();

  std::cout << "Rx packets: " << RX_PACKETS << "/" << TX_PACKETS << std::endl;

  return 0;

}
