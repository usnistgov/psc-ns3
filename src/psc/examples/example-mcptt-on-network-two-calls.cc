/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2020 University of Washington (MCPTT modifications)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Jaume Nin <jaume.nin@cttc.cat>
 *          Manuel Requena <manuel.requena@cttc.es>
 *          Tom Henderson <tomhend@uw.edu>
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/psc-module.h"

using namespace ns3;
using namespace psc;

/**
 * Adaptation of lena-simple-epc.cc to experiment with two MCPTT on-network
 * calls involving two pairs of UEs to a single MCPTT server managing both
 * calls.  The first group call (between the first pair of UEs) starts
 * when the McpttPttApps start at time 2 seconds and runs until simulation
 * time 16 seconds.  The second call starts at time 18 seconds and runs
 * until time 34 seconds.
 */

NS_LOG_COMPONENT_DEFINE ("ExampleMcpttOnNetworkTwoCalls");

int
main (int argc, char *argv[])
{
  DataRate dataRate = DataRate ("12kb/s");
  double distance = 60.0;
  uint16_t numEnb = 2;  // do not change
  uint16_t numUe = 4;   // do not change
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double onOffMean = 5.0;
  Time simTime = Seconds (35.1);
  Time start = Seconds (2.0);
  Time stop = Seconds (35.0);
  bool useCa = false;
  bool verbose = false;

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("on-off-mean", "The average number of seconds a user pushes and releases the button.", onOffMean);
  cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.AddValue ("verbose", "Whether to enable debug logging", verbose);
  cmd.Parse (argc, argv);

  // parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
      LogComponentEnable ("McpttChannel", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttPttApp", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCall", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCall", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCallHelper", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCallMachine", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCallMachine", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttPusher", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerApp", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorParticipant", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorParticipantState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorArbitrator", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorTowardsParticipant", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorTowardsParticipantState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorArbitratorState", LOG_LEVEL_ALL);
    }

  if (useCa)
    {
      Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
      Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
      Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
    }

  Config::SetDefault ("ns3::psc::McpttMsgStats::CallControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::FloorControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::Media", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::IncludeMessageContent", BooleanValue (false));
  Config::SetDefault ("ns3::psc::McpttOnNetworkFloorParticipant::GenMedia", BooleanValue (true));

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  Ptr<ImsHelper> imsHelper = CreateObject<ImsHelper> ();
  imsHelper->ConnectPgw (epcHelper->GetPgwNode ());

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
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodePair1;
  NodeContainer ueNodePair2;
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numEnb);
  ueNodePair1.Create (numUe / 2);
  ueNodePair2.Create (numUe / 2);
  ueNodes.Add (ueNodePair1);
  ueNodes.Add (ueNodePair2);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // eNB positions
  positionAlloc->Add (Vector (distance, 0, 0));
  positionAlloc->Add (Vector (distance * 2, 0, 0));
  // UE pair 1 positions
  positionAlloc->Add (Vector (distance, 10, 0));
  positionAlloc->Add (Vector (distance * 2, 10, 0));
  // UE pair 2 positions
  positionAlloc->Add (Vector (distance, -10, 0));
  positionAlloc->Add (Vector (distance * 2, -10, 0));
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);
  mobility.Install (ueNodePair1);
  mobility.Install (ueNodePair2);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  // side effect: the default EPS bearer will be activated
  lteHelper->Attach (ueLteDevs.Get (0), enbLteDevs.Get (0));
  lteHelper->Attach (ueLteDevs.Get (1), enbLteDevs.Get (1));
  lteHelper->Attach (ueLteDevs.Get (2), enbLteDevs.Get (0));
  lteHelper->Attach (ueLteDevs.Get (3), enbLteDevs.Get (1));

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer serverAppContainer;
  McpttServerHelper mcpttServerHelper;

  serverAppContainer.Add (mcpttServerHelper.Install (imsHelper->GetImsNode ()));
  serverAppContainer.Start (start);
  serverAppContainer.Stop (stop);

  ApplicationContainer clientAppContainer;
  ApplicationContainer clientAppContainer1;
  ApplicationContainer clientAppContainer2;
  McpttHelper mcpttClientHelper;
  mcpttClientHelper.SetPttApp ("ns3::psc::McpttPttApp");
  mcpttClientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                                 "Bytes", UintegerValue (msgSize),
                                 "DataRate", DataRateValue (dataRate));
  mcpttClientHelper.SetPusher ("ns3::psc::McpttPusher",
                               "Automatic", BooleanValue (true));
  mcpttClientHelper.SetPusherPttInterarrivalTimeVariable ("ns3::NormalRandomVariable",
                                                          "Mean", DoubleValue (onOffMean),
                                                          "Variance", DoubleValue (2.0));
  mcpttClientHelper.SetPusherPttDurationVariable ("ns3::NormalRandomVariable",
                                                  "Mean", DoubleValue (onOffMean),
                                                  "Variance", DoubleValue (2.0));

  clientAppContainer1.Add (mcpttClientHelper.Install (ueNodePair1));
  clientAppContainer2.Add (mcpttClientHelper.Install (ueNodePair2));
  clientAppContainer1.Start (start);
  clientAppContainer1.Stop (stop);
  clientAppContainer2.Start (start);
  clientAppContainer2.Stop (stop);
  clientAppContainer.Add (clientAppContainer1);
  clientAppContainer.Add (clientAppContainer2);

  Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverAppContainer.Get (0));
  Ipv4Address serverAddress = Ipv4Address::ConvertFrom (imsHelper->GetImsGmAddress ());
  serverApp->SetLocalAddress (serverAddress);
  NS_LOG_INFO ("server IMS IP address = " << serverAddress);

  for (uint32_t index = 0; index < ueIpIface.GetN (); index++)
    {
      Ptr<McpttPttApp> pttApp = clientAppContainer.Get (index)->GetObject<McpttPttApp> ();
      Ipv4Address clientAddress = ueIpIface.GetAddress (index);
      pttApp->SetLocalAddress (clientAddress);
      NS_LOG_INFO ("client " << index << " ip address = " << clientAddress);
    }

  McpttCallHelper callHelper;
  // Optional statements to tailor the configurable attributes
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                            "AckRequired", BooleanValue (false),
                            "AudioCutIn", BooleanValue (false),
                            "DualFloorSupported", BooleanValue (false),
                            "QueueingSupported", BooleanValue (true));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (false),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  // Add first call, to start at time 2 and stop at time 10
  // Call will involve two nodes (7 and 8) and the MCPTT server (node 3)
  uint32_t groupId = 1;
  callHelper.AddCall (clientAppContainer1, serverApp, groupId, callType, Seconds (2), Seconds (16));

  // Add second call, on new groupId, to start at time 8 and stop at time 15
  // Call will involve two nodes (9 and 10) and the MCPTT server (node 3)
  groupId = 2;
  callHelper.AddCall (clientAppContainer2, serverApp, groupId, callType, Seconds (18), Seconds (34));

  NS_LOG_INFO ("Enabling MCPTT traces...");
  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();
  traceHelper->EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  traceHelper->EnableAccessTimeTrace ("mcptt-access-time.txt");

  // Uncomment to enable PCAP tracing
  // p2ph.EnablePcap("example-mcptt-on-network-two-calls.ims.pcap", imsHelper->GetImsGmDevice (), true, true);

  Simulator::Stop (simTime);
  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}
