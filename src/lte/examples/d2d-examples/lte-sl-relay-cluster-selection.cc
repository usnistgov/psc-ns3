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

using namespace ns3;

/*
 * Scenario with configurable number of Relay UEs and Remote UEs organized in
 * a cluster topology with configurable Relay UE selection algorithm.
 * The Remote UEs can be configured to be in-coverage or out-of-coverage, and
 * the traffic can be configured to flow either between Remote UEs and a
 * RemoteHost in the Internet, or between Remote UEs.
 *
 * Topology:
 * The 'nRelayUes' Relay UEs are deployed around the eNB uniformly on a
 * circle of radius 'relayRadius' meters.
 * Each Relay UE has a cluster of 'nRemoteUesPerRelay' Remote UEs deployed
 * around itself uniformly on a circle of radius 'remoteRadius' meters.
 *
 * The parameter 'remoteUesOoc' set to true indicates the Remote UEs are
 * out-of-coverage (not attached to the eNodeB and use SL preconfiguration).
 *
 * One-to-one connection:
 * The parameter 'clusterSC' set to true means that each Relay UE provides an
 * unique Realy Service (i.e., a unique Relay Service Code) and
 * the cluster of Remote UEs around a given Relay UE are interested only in its
 * Relay Service Code and thus will connect only to that Relay UE.
 * The parameter 'clusterSC' set to false means that all Relay UEs provide the
 * same service (i.e., the same Relay Service Code), and all Remote UEs are
 * interested in that service.
 * The UEs start their relay service sequentially in time. First the Relay UE,
 * then the cluster of Remote UEs associated to that Relay UE (sequentially as
 * well), then the next Relay UE, and so on.
 *
 * The parameter 'selectionAlgorithm' determines the Relay UE (re)selection
 * algorithm the Remote UEs will use to select the Relay UE to connect to.
 * This parameter is only relevant if 'clusterSC' is set to false.
 *
 * Traffic:
 * Each Remote UE sends packets to a given node in the network, which echoes
 * back each packet to the Remote UE, showcasing both upward and downward
 * traffic through the Relay UE.
 * The parameter 'echoServerNode' determines the node towards which the Remote
 * UEs send their traffic: either a Remote Host in the Internet (when set to
 * 'RemoteHost') or the first Remote UE connected to the first Relay UE (when
 * set to 'RemoteUE').
 * Each transmitting Remote UE starts sending traffic 3.00 s after the start
 * of the one-to-one connection procedure with its Relay UE and remain active
 * during 10.0 s. The simulation time is calculated so that the last Remote UE
 * can have its 10.0s of traffic activity.
 *
 * Scenario outputs:
 * - topology.plt: gnuplot script to plot the topology of the scenario.
 * - ReportUeSdRsrpMeasurementsTrace.txt: Log of the SD-RSRP measurement
 *                                        reports made by the Remote UEs
 * - PC5SignalingPacketTrace.txt: Log of the received PC5 signaling messages
 * - Pc5ConnectionStatusTrace.txt: Log of the changes in the status of the
 *                                 ongoing one-to-one communication connections
 * - RelayUeSelectionTrace.txt: Log of when a Remote UE selects a Relay UE
 * - AppPacketTrace.txt: Log of application layer transmitted and received
 *                       packets
 * - DlPdcpStats.txt: PDCP layer statistics for the DL
 * - UlPdcpStats.txt: PDCP layer statistics for the UL
 *
 * Users can generate the topology plot by running:
 * $ gnuplot topology.plt
 * It will generate the file 'topology.png' with a plot of the eNodeB,
 * Relay UEs, and Remote UEs positions.
 * Users can track the SD-RSRP measurement reports made by the Remote UEs in
 * the file 'ReportUeSdRsrpMeasurementsTrace.txt'. Those reports are of
 * particular interest when using the relay selection algorithms that depends
 * on the SD-RSRP of the Relay UEs (MaxSDRsrpNoReselection|MaxSDRsrp).
 * The Relay UE selection decisions of each Remote UE can be seen in
 * the file 'RelayUeSelectionTrace.txt'. Users can then follow the one-to-one
 * communication setup procedure between each pair of Remote and Relay UEs
 * by checking the corresponding PC5 signaling messages exchange in the file
 * 'PC5SignalingPacketTrace.txt' and the connection status change in
 * the file 'Pc5ConnectionStatusTrace.txt'.
 * Users can verify the traffic flows on the file 'AppPacketTrace.txt' and also
 * can verify which nodes are using the network UL/DL in 'DlPdcpStats.txt' and
 * 'UlPdcpStats.txt'. If the one-to-one connections were successfully
 * established and maintained, only the Relay UEs should be using the UL/DL
 * to transmit and receive the Remote UEs relayed traffic.
 *
 */

NS_LOG_COMPONENT_DEFINE ("lte-sl-relay-cluster-selection");

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
}

/**
 * Trace sink function for logging when a SD-RSRP measurement is done by a
 * Remote UE
 */
void
ReportUeSdRsrpMeasurementsTrace (Ptr<OutputStreamWrapper> stream, uint16_t rnti, uint64_t relayId, uint32_t serviceCode, double sdRsrp)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << rnti << "\t"
                        << relayId << "\t"
                        << serviceCode << "\t"
                        << sdRsrp << "\t"
                        << std::endl;
}

/**
 * Trace sink function for logging when a Remote UE selects a new Relay UE
 */
void
RelayUeSelectionTrace (Ptr<OutputStreamWrapper> stream, uint64_t imsi, uint32_t serviceCode, uint64_t currentRelayId, uint64_t selectedRelayId)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << imsi << "\t"
                        << serviceCode << "\t"
                        << currentRelayId << "\t"
                        << selectedRelayId << "\t"
                        << std::endl;
}

/**
 * Trace sink function for logging PC5 connection change of status
 */
void
Pc5ConnectionStatusTrace (Ptr<OutputStreamWrapper> stream, uint32_t selfUeId,  uint32_t peerUeId, LteSlUeRrc::RelayRole role,
                          LteSlBasicUeController::Pc5ConnectionStatus status, uint32_t iface)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << selfUeId << "\t"
                        << peerUeId << "\t"
                        << role << "\t"
                        << status << "\t"
                        << std::endl;
}

/**
 * Function that generates a gnuplot script file that can be used to plot the
 * topology of the scenario access network (eNBs, Relay UEs and Remote UEs)
 */
void
GenerateTopologyPlotFile (NodeContainer enbNode, NodeContainer relayUeNodes, NodeContainer remoteUeNodes,
                          double relayRadius, double remoteRadius )
{
  std::string fileNameWithNoExtension = "topology";
  std::string graphicsFileName = fileNameWithNoExtension + ".png";
  std::string gnuplotFileName = fileNameWithNoExtension + ".plt";
  std::string plotTitle = "Topology (Labels = Node IDs)";

  Gnuplot plot (graphicsFileName);
  plot.SetTitle (plotTitle);
  plot.SetTerminal ("png size 1280,1024");
  plot.SetLegend ("X", "Y"); //These are the axis, not the legend
  std::ostringstream plotExtras;
  plotExtras << "set xrange [-" << 1.1 * (relayRadius + remoteRadius) << ":+" << 1.1 * (relayRadius + remoteRadius) << "]" << std::endl;
  plotExtras << "set yrange [-" << 1.1 * (relayRadius + remoteRadius) << ":+" << 1.1 * (relayRadius + remoteRadius) << "]" << std::endl;
  plotExtras << "set linetype 1 pt 3 ps 2 " << std::endl;
  plotExtras << "set linetype 2 lc rgb \"green\" pt 2 ps 2" << std::endl;
  plotExtras << "set linetype 3 pt 1 ps 2" << std::endl;
  plot.AppendExtra (plotExtras.str ());

  //eNB
  Gnuplot2dDataset datasetEnodeB;
  datasetEnodeB.SetTitle ("eNodeB");
  datasetEnodeB.SetStyle (Gnuplot2dDataset::POINTS);

  double x = enbNode.Get (0)->GetObject<MobilityModel> ()->GetPosition ().x;
  double y = enbNode.Get (0)->GetObject<MobilityModel> ()->GetPosition ().y;
  std::ostringstream strForLabel;
  strForLabel << "set label \"" << enbNode.Get (0)->GetId () << "\" at " << x << "," << y << " textcolor rgb \"grey\" center front offset 0,1";
  plot.AppendExtra (strForLabel.str ());
  datasetEnodeB.Add (x, y);
  plot.AddDataset (datasetEnodeB);

  //Relay UEs
  Gnuplot2dDataset datasetRelays;
  datasetRelays.SetTitle ("Relay UEs");
  datasetRelays.SetStyle (Gnuplot2dDataset::POINTS);
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ry++)
    {
      double x = relayUeNodes.Get (ry)->GetObject<MobilityModel> ()->GetPosition ().x;
      double y = relayUeNodes.Get (ry)->GetObject<MobilityModel> ()->GetPosition ().y;
      std::ostringstream strForLabel;
      strForLabel << "set label \"" << relayUeNodes.Get (ry)->GetId () << "\" at " << x << "," << y << " textcolor rgb \"grey\" center front offset 0,1";
      plot.AppendExtra (strForLabel.str ());
      datasetRelays.Add (x, y);
    }
  plot.AddDataset (datasetRelays);

  //Remote UEs
  Gnuplot2dDataset datasetRemotes;
  datasetRemotes.SetTitle ("Remote UEs");
  datasetRemotes.SetStyle (Gnuplot2dDataset::POINTS);
  for (uint32_t rm = 0; rm < remoteUeNodes.GetN (); rm++)
    {
      double x = remoteUeNodes.Get (rm)->GetObject<MobilityModel> ()->GetPosition ().x;
      double y = remoteUeNodes.Get (rm)->GetObject<MobilityModel> ()->GetPosition ().y;
      std::ostringstream strForLabel;
      strForLabel << "set label \"" << remoteUeNodes.Get (rm)->GetId () << "\" at " << x << "," << y << " textcolor rgb \"grey\" center front offset 0,1";
      plot.AppendExtra (strForLabel.str ());
      datasetRemotes.Add (x, y);
    }
  plot.AddDataset (datasetRemotes);

  std::ofstream plotFile (gnuplotFileName.c_str ());
  plot.GenerateOutput (plotFile);
  plotFile.close ();
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

/**
 * Function to update the remote address of a UdpEchoClient application to
 * the UE-to-Network Relay network address of a node.
 * Used when the UdpEchoServer application is installed in a Remote UE.
 * The Remote UE will use an IPv6 address from the UE-to-Network Relay network
 * once it connects to a Relay UE. Thus, the UdpEchoClient applications
 * sending to the UdpEchoServer application in the Remote UE need to update
 * their remote address to this new address to be able to reach it.
 */
void
ChangeUdpEchoClientRemote (Ptr<Node> newRemoteNode, Ptr<UdpEchoClient> app, uint16_t port, Ipv6Address network, Ipv6Prefix prefix)
{
  Ptr<Ipv6> ipv6 = newRemoteNode->GetObject<Ipv6> ();
  //Get the interface used for UE-to-Network Relay (LteSlUeNetDevices)
  int32_t ipInterfaceIndex = ipv6->GetInterfaceForPrefix (network, prefix);

  Ipv6Address remoteNodeSlIpAddress = newRemoteNode->GetObject<Ipv6L3Protocol> ()->GetAddress (ipInterfaceIndex,1).GetAddress ();
  NS_LOG_INFO (" Node id = [" << app->GetNode ()->GetId ()
                              << "] changed the UdpEchoClient Remote Ip Address to " << remoteNodeSlIpAddress);
  app->SetRemote (remoteNodeSlIpAddress, port);
}


int main (int argc, char *argv[])
{

  double simTime = 1.0; //s //Simulation time (in seconds) updated automatically based on number of nodes
  double relayRadius = 20.0; //m
  double remoteRadius = 50.0; //m
  uint32_t nRelayUes = 2;
  uint32_t nRemoteUesPerRelay = 1;
  bool remoteUesOoc = true;
  bool clusterSC = false;
  std::string selectionAlgorithm ("MaxSDRsrpNoReselection");
  std::string echoServerNode ("RemoteHost");

  CommandLine cmd;

  cmd.AddValue ("relayRadius", "The radius of the circle (with center on the eNB) where the Relay UEs are positioned", relayRadius);
  cmd.AddValue ("remoteRadius", "The radius of the circle (with center on the Relay UE) where the Remote UEs are positioned", remoteRadius);
  cmd.AddValue ("nRelayUes", "Number of Relay UEs in the scenario", nRelayUes);
  cmd.AddValue ("nRemoteUesPerRelay", "Number of Remote UEs per deployed Relay UE", nRemoteUesPerRelay);
  cmd.AddValue ("remoteUesOoc", "The Remote UEs are out-of-coverage", remoteUesOoc);
  cmd.AddValue ("clusterSC", "Each Relay UE provides a different Relay Service Code", clusterSC);
  cmd.AddValue ("selectionAlgorithm", "The Relay UE (re)selection algorithm the Remote UEs will use (RandomNoReselection|MaxSDRsrpNoReselection|MaxSDRsrp)", selectionAlgorithm);
  cmd.AddValue ("echoServerNode", "The node towards which the Remote UE traffic is directed to (RemoteHost|RemoteUE)", echoServerNode);

  cmd.Parse (argc, argv);

  if (echoServerNode.compare ("RemoteHost") != 0 && echoServerNode.compare ("RemoteUE") != 0)
    {
      NS_FATAL_ERROR ("Wrong echoServerNode!. Options are (RemoteHost|RemoteUE).");
      return 1;
    }
  if (echoServerNode.compare ("RemoteUE") == 0 && nRelayUes * nRemoteUesPerRelay < 2)
    {
      NS_FATAL_ERROR ("At least 2 Remote UEs are needed when echoServerNode is a RemoteUE !");
      return 1;
    }

  //Calculate the start time of the relay service for Relay UEs and Remote UEs
  //Do it sequentially for easy of tractability
  double startTimeRelay [nRelayUes];
  double startTimeRemote [nRelayUes * nRemoteUesPerRelay];
  //The time between Remote UE's start of service
  //Four discovery periods (4 * 0.32 s) to ensure relay selection (measurement report every 4 periods)
  //One discovery period (0.32 s) to avoid some of the congestion for the connection messages
  double timeBetweenRemoteStarts = 4 * 0.32 + 0.32; //s
  // The time between Relay UE's start of service
  // 5.0 s of baseline
  // plus the time needed to all Remote UEs to connect to it:
  // (2+2*nRemoteUesPerRelay)*0.04 is the time in the worst case for all connection messages to go through between a Remote UE and a Relay UE:
  // 2 msgs from the Remote UE, each of them in 1 single SL period (0.04 s) and
  // 2 msgs from the Relay UE, which given the SL period RR scheduling, it can take in the worst case up to
  // nRemoteUesPerRelay SL periods to be sent
  // timeBetweenRemoteStarts to ensure sequentiality of activation
  double timeBetweenRelayStarts = 5.0 + nRemoteUesPerRelay * (((2 + 2 * nRemoteUesPerRelay) * 0.04) + timeBetweenRemoteStarts); //s

  for (uint32_t ryIdx = 0; ryIdx < nRelayUes; ryIdx++)
    {
      startTimeRelay [ryIdx] = 2.0 + timeBetweenRelayStarts * ryIdx;

      NS_LOG_INFO ("Relay UE Idx " << ryIdx << " start time " << startTimeRelay [ryIdx] << "s");

      for (uint32_t rm = 0; rm < nRemoteUesPerRelay; ++rm)
        {
          uint32_t rmIdx = ryIdx * nRemoteUesPerRelay + rm;
          startTimeRemote [rmIdx] = startTimeRelay [ryIdx] + timeBetweenRemoteStarts * (rm + 1);
          NS_LOG_INFO ("Remote UE Idx " << rmIdx << " start time " << startTimeRemote [rmIdx] << "s");
        }
    }

  //Calculate simTime based on relay service starts and give 10 s of traffic for the last one
  simTime = startTimeRemote [(nRelayUes * nRemoteUesPerRelay - 1)] + 3.0 + 10.0; //s
  NS_LOG_INFO ("Simulation time = " << simTime << " s");

  NS_LOG_INFO ("Configuring default parameters...");

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

  //Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  //Increase SRS periodicity to allow larger number of UEs in the system
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (320));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (46.0));

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

  //Configure (Relay UE (re)selection algorithm (RandomNoReselection|MaxSDRsrpNoReselection|MaxSDRsrp)
  Config::SetDefault ("ns3::LteSlBasicUeController::RelayUeSelectionAlgorithm", StringValue (selectionAlgorithm));

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
  remoteUeNodes.Create (nRelayUes * nRemoteUesPerRelay);

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

  //UEs
  Ptr<ListPositionAllocator> positionAllocRelays = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> positionAllocRemotes = CreateObject<ListPositionAllocator> ();
  for (uint32_t ry = 0; ry < relayUeNodes.GetN (); ++ry)
    {
      //Relay UE
      double ry_angle = ry * (360.0 / relayUeNodes.GetN ()); //degrees
      double ry_pos_x = std::floor (relayRadius * std::cos (ry_angle * M_PI / 180.0) );
      double ry_pos_y = std::floor (relayRadius * std::sin (ry_angle * M_PI / 180.0) );

      positionAllocRelays->Add (Vector (ry_pos_x, ry_pos_y, 1.5));

      NS_LOG_INFO ("Relay UE " << ry + 1 << " node id = [" << relayUeNodes.Get (ry)->GetId () << "]"
                   " x " << ry_pos_x << " y " << ry_pos_y);
      //Remote UEs
      for (uint32_t rm = 0; rm < nRemoteUesPerRelay; ++rm)
        {
          double rm_angle = rm * (360.0 / nRemoteUesPerRelay); //degrees
          double rm_pos_x = std::floor (ry_pos_x + remoteRadius * std::cos (rm_angle * M_PI / 180.0));
          double rm_pos_y = std::floor (ry_pos_y + remoteRadius * std::sin (rm_angle * M_PI / 180.0));

          positionAllocRemotes->Add (Vector (rm_pos_x, rm_pos_y, 1.5));

          uint32_t remoteIdx = ry * nRemoteUesPerRelay + rm;
          NS_LOG_INFO ("Remote UE " << remoteIdx << " node id = [" << remoteUeNodes.Get (remoteIdx)->GetId () << "]"
                       " x " << rm_pos_x << " y " << rm_pos_y);
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

  //Generate gnuplot file with the script to generate the topology plot
  GenerateTopologyPlotFile (enbNode, relayUeNodes, remoteUeNodes, relayRadius, remoteRadius);

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
  //-Configure Sidelink preconfiguration
  // Relay UEs: Empty configuration
  LteRrcSap::SlPreconfiguration preconfigurationRelay;
  // Remote UEs: Empty configuration if in-coverage
  //             Custom configuration (see below) if out-of-coverage
  LteRrcSap::SlPreconfiguration preconfigurationRemote;

  if (remoteUesOoc)
    {
      //Configure general preconfiguration parameters
      preconfigurationRemote.preconfigGeneral.carrierFreq = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ();
      preconfigurationRemote.preconfigGeneral.slBandwidth = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlBandwidth ();

      //-Configure preconfigured Sidelink communication pool
      preconfigurationRemote.preconfigComm = proseHelper->GetDefaultSlPreconfigCommPoolList ();

      //-Configure preconfigured Sidelink discovery pool
      preconfigurationRemote.preconfigDisc = proseHelper->GetDefaultSlPreconfigDiscPoolList ();

      //-Configure preconfigured UE-to-Network Relay parameters
      preconfigurationRemote.preconfigRelay = proseHelper->GetDefaultSlPreconfigRelay ();
    }

  //-Enable Sidelink discovery
  ueSidelinkConfiguration->SetDiscEnabled (true);
  //-Set frequency for Sidelink discovery messages monitoring
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  //Install UE Sidelink configuration on the UE devices with the corresponding preconfiguration
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRelay);
  lteHelper->InstallSidelinkConfiguration (relayUeDevs, ueSidelinkConfiguration);

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationRemote);
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
  proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (48));

  //Configure route between PGW and UE-to-Network Relay network
  proseHelper->ConfigurePgwToUeToNetworkRelayRoute (pgw);

  //Attach Relay UEs to the eNB
  lteHelper->Attach (relayUeDevs);
  //If the Remote UEs are not OOC attach them to the eNodeB as well
  if (!remoteUesOoc)
    {
      lteHelper->Attach (remoteUeDevs);
    }

  ///*** Configure applications ***///
  //For each Remote UE, we have a pair (UpdEchoClient, UdpEchoServer)
  //Each Remote UE has an assigned port
  //UdpEchoClient installed in the Remote UE, sending to the echoServerAddr
  //and to the corresponding Remote UE port
  //UdpEchoServer installed in the echoServerNode, listening to the
  //corresponding Remote UE port

  Ipv6Address echoServerAddr;
  if (echoServerNode.compare ("RemoteHost") == 0)
    {
      echoServerAddr = internetIpIfaces.GetAddress (1, 1);
    }
  else if (echoServerNode.compare ("RemoteUE") == 0)
    {
      // We use a dummy IP address for initial configuration as we don't know the
      // IP address of the 'Remote UE (0)' before it connects to the Relay UE
      echoServerAddr = Ipv6Address::GetOnes ();
    }
  uint16_t echoPortBase = 50000;
  ApplicationContainer serverApps;
  ApplicationContainer clientApps;
  AsciiTraceHelper ascii;

  std::ostringstream oss;
  Ptr<OutputStreamWrapper> packetOutputStream = ascii.CreateFileStream ("AppPacketTrace.txt");
  *packetOutputStream->GetStream () << "time(sec)\ttx/rx\tC/S\tNodeID\tIP[src]\tIP[dst]\tPktSize(bytes)" << std::endl;

  for (uint16_t remUeIdx = 0; remUeIdx < remoteUeNodes.GetN (); remUeIdx++)
    {
      if (echoServerNode.compare ("RemoteUE") == 0 && remUeIdx == 0)
        {
          //No own traffic applications for Remote UE (0) as it is the echoServerNode
          continue;
        }

      uint16_t remUePort = echoPortBase + remUeIdx;
      uint32_t echoServerNodeId = 0;
      //UdpEchoServer listening in the Remote UE port
      UdpEchoServerHelper echoServerHelper (remUePort);
      ApplicationContainer singleServerApp;
      if (echoServerNode.compare ("RemoteHost") == 0)
        {
          singleServerApp.Add (echoServerHelper.Install (remoteHost));
          echoServerNodeId = remoteHost->GetId ();
        }
      else if (echoServerNode.compare ("RemoteUE") == 0)
        {
          singleServerApp.Add (echoServerHelper.Install (remoteUeNodes.Get (0)));
          echoServerNodeId = remoteUeNodes.Get (0)->GetId ();

        }
      singleServerApp.Start (Seconds (1.0));
      singleServerApp.Stop (Seconds (simTime));

      //Tracing packets on the UdpEchoServer (S)
      oss << "rx\tS\t" << echoServerNodeId;
      singleServerApp.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetOutputStream));
      oss.str ("");
      oss << "tx\tS\t" << echoServerNodeId;
      singleServerApp.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetOutputStream));
      oss.str ("");

      serverApps.Add (singleServerApp);

      //UdpEchoClient in the Remote UE
      UdpEchoClientHelper echoClientHelper (echoServerAddr);
      echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (20));
      echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
      echoClientHelper.SetAttribute ("PacketSize", UintegerValue (150));
      echoClientHelper.SetAttribute ("RemotePort", UintegerValue (remUePort));

      ApplicationContainer singleClientApp = echoClientHelper.Install (remoteUeNodes.Get (remUeIdx));
      //Start the application 3.0 s after the remote UE started the relay service
      //normally it should be enough time to connect
      singleClientApp.Start (Seconds (3.0 + startTimeRemote [remUeIdx]) );
      //Stop the application after 10.0 s
      singleClientApp.Stop (Seconds (3.0 + startTimeRemote [remUeIdx] + 10.0));

      //Tracing packets on the UdpEchoClient (C)
      oss << "tx\tC\t" << remoteUeNodes.Get (remUeIdx)->GetId ();
      singleClientApp.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetOutputStream));
      oss.str ("");
      oss << "rx\tC\t" << remoteUeNodes.Get (remUeIdx)->GetId ();
      singleClientApp.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, packetOutputStream));
      oss.str ("");

      clientApps.Add (singleClientApp);

      if (echoServerNode.compare ("RemoteUE") == 0 && remUeIdx != 0)
        {
          //Schedule the change of the RemoteAddress to 100 ms before the start of the application
          //normally 'Remote UE (0)' should be already connected to its Relay UE so we can
          //assign its address as RemoteAddress
          Simulator::Schedule (Seconds (3.0 + startTimeRemote [remUeIdx] - 0.100),
                               &ChangeUdpEchoClientRemote, remoteUeNodes.Get (0),
                               singleClientApp.Get (0)->GetObject<UdpEchoClient> (),
                               remUePort,
                               proseHelper->GetIpv6NetworkForRelayCommunication (),
                               proseHelper->GetIpv6PrefixForRelayCommunication ());
        }
    }

  ///*** Configure Relaying ***///

  //Setup dedicated bearer for the Relay UEs
  Ptr<EpcTft> tft = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localIpv6Address = proseHelper->GetIpv6NetworkForRelayCommunication ();
  dlpf.localIpv6Prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
  tft->Add (dlpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (relayUeDevs, bearer, tft);

  //Schedule the start of the relay service in each UE with their corresponding
  //roles and service codes
  for (uint32_t ryDevIdx = 0; ryDevIdx < relayUeDevs.GetN (); ryDevIdx++)
    {
      uint32_t serviceCode;
      if (clusterSC)
        {
          serviceCode = relayUeDevs.Get (ryDevIdx)->GetObject<LteUeNetDevice> ()->GetImsi ();
        }
      else
        {
          serviceCode = 33;
        }

      Simulator::Schedule (Seconds (startTimeRelay [ryDevIdx]), &LteSidelinkHelper::StartRelayService, proseHelper, relayUeDevs.Get (ryDevIdx), serviceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
      NS_LOG_INFO ("Relay UE " << ryDevIdx << " node id = [" << relayUeNodes.Get (ryDevIdx)->GetId () << "] provides Service Code " << serviceCode << " and start service at " << startTimeRelay [ryDevIdx] << " s");
      //Remote UEs
      for (uint32_t rm = 0; rm < nRemoteUesPerRelay; ++rm)
        {
          uint32_t rmDevIdx = ryDevIdx * nRemoteUesPerRelay + rm;
          Simulator::Schedule ((Seconds (startTimeRemote [rmDevIdx])), &LteSidelinkHelper::StartRelayService, proseHelper, remoteUeDevs.Get (rmDevIdx), serviceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
          NS_LOG_INFO ("Remote UE " << rmDevIdx << " node id = [" << remoteUeNodes.Get (rmDevIdx)->GetId () << "] interested in Service Code " << serviceCode << " and start service at " << startTimeRemote [rmDevIdx] << " s");
        }
    }

  //Tracing PC5 signaling messages in all UEs (Remote UEs and Relay UEs)
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

  //Tracing SD-RSRP measurements in the Remote UEs
  Ptr<OutputStreamWrapper> ReportUeSdRsrpMeasurementsStream = ascii.CreateFileStream ("ReportUeSdRsrpMeasurementsTrace.txt");
  *ReportUeSdRsrpMeasurementsStream->GetStream () << "time(s)\trnti\trelayId\tSC\tsdRsrp" << std::endl;
  for (uint32_t ueDevIdx = 0; ueDevIdx < remoteUeDevs.GetN (); ueDevIdx++)
    {
      Ptr<LteUePhy> phy = remoteUeDevs.Get (ueDevIdx)->GetObject<LteUeNetDevice> ()->GetPhy ();
      phy->TraceConnectWithoutContext ("ReportUeSdRsrpMeasurements",
                                       MakeBoundCallback (&ReportUeSdRsrpMeasurementsTrace,
                                                          ReportUeSdRsrpMeasurementsStream));
    }

  //Tracing Relay Selection decisions in the Remote UEs
  Ptr<OutputStreamWrapper> RelayUeSelectionStream = ascii.CreateFileStream ("RelayUeSelectionTrace.txt");
  *RelayUeSelectionStream->GetStream () << "time(s)\timsi\tSC\tcRelayId\tsRelayId" << std::endl;

  for (uint32_t ueDevIdx = 0; ueDevIdx < remoteUeDevs.GetN (); ueDevIdx++)
    {
      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (ueDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      PointerValue ptrTwo;
      slUeRrc->GetAttribute ("SlController", ptrTwo);
      Ptr<LteSlBasicUeController> ctlr = ptrTwo.Get<LteSlBasicUeController>();
      ctlr->TraceConnectWithoutContext ("RelayUeSelection",
                                        MakeBoundCallback (&RelayUeSelectionTrace,
                                                           RelayUeSelectionStream));
    }

  //Tracing onte-to-one connection status in all UEs (Remote UEs and Relay UEs)
  Ptr<OutputStreamWrapper> Pc5ConnectionStatusStream = ascii.CreateFileStream ("Pc5ConnectionStatusTrace.txt");
  *Pc5ConnectionStatusStream->GetStream () << "time(s)\tselfUeId\tpeerUeId\trole\tstatus" << std::endl;

  for (uint32_t ueDevIdx = 0; ueDevIdx < allUeDevs.GetN (); ueDevIdx++)
    {
      Ptr<LteUeRrc> rrc = allUeDevs.Get (ueDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      PointerValue ptrTwo;
      slUeRrc->GetAttribute ("SlController", ptrTwo);
      Ptr<LteSlBasicUeController> ctlr = ptrTwo.Get<LteSlBasicUeController>();
      ctlr->TraceConnectWithoutContext ("Pc5ConnectionStatus",
                                        MakeBoundCallback (&Pc5ConnectionStatusTrace,
                                                           Pc5ConnectionStatusStream));
    }

  lteHelper->EnablePdcpTraces ();

  NS_LOG_INFO ("Simulation time " << simTime << " s");
  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  Simulator::Destroy ();
  return 0;

}
