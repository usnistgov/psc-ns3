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

// This program extends the LTE LENA example file to experiment with two
// simultaneous MCPTT on-network calls involving two groups of UEs involving
// two MCPTT servers.
//
// The first UE participates in both calls and switches between calls
// every 20 seconds.  An event trace is configured on the first UE to
// report floor machine events.  The simulation is configured to start
// both calls at time 2s.  The output displays that at time 2s, the userId 1
// has two call added (callIds 1 and 2), and then callId 1 is selected again
// and started.  Upon PTT activity, it eventually gets the floor.  At time 20s,
// the user selects the second call without releasing the first call.  This
// is analogous to a real user backgrounding the first call while putting
// the second call in the foreground.  It can be observed from the trace that
// the UE releases the floor on callId 1 at that time, and for its next
// push event, generates a floor request on callId 2.  The event trace
// shows that the UE is still receiving floor control messages from the
// first call (which will cause the floor control state machine for that call
// to evolve).  At time 40s, the UE switches back to call 1, and at time 60s,
// the simulation ends.
//

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/psc-module.h"

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("ExampleMcpttOnNetworkTwoSimultaneousCalls");

// Display floor control and application events observed on the connected UE
// This is connected below to the first UE (MCPTT user ID 1) only
void EventTrace (uint32_t userId, uint16_t callId, const std::string& selected, const char* description)
{
  std::cout << Simulator::Now ().GetSeconds () << " userId " << userId << " callId " << callId << " selected " << selected << " : " << description << std::endl;
}

int
main (int argc, char *argv[])
{
  DataRate dataRate = DataRate ("12kb/s");
  double distance = 60.0;
  const uint16_t numEnb = 2;  // do not change
  const uint16_t numUe = 6;   // do not change
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double onOffMean = 5.0;
  Time appStartTime = Seconds (1);
  Time callStartTime = Seconds (2);
  Time firstSwitchTime = Seconds (20);
  Time secondSwitchTime = Seconds (40);
  Time callStopTime = Seconds (59);
  Time appStopTime = Seconds (60);
  Time simTime = Seconds (60.1);
  bool logging = false;

  Config::SetDefault ("ns3::psc::McpttMsgStats::CallControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::FloorControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::Media", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::IncludeMessageContent", BooleanValue (false));
  Config::SetDefault ("ns3::psc::McpttOnNetworkFloorParticipant::GenMedia", BooleanValue (true));

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("on-off-mean", "The average number of seconds a user pushes and releases the button.", onOffMean);
  cmd.AddValue ("logging", "Whether to enable debug logging", logging);
  cmd.Parse (argc, argv);

  if (logging)
    {
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
      LogComponentEnable ("McpttChannel", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttPttApp", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerApp", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCall", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCall", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCallHelper", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttCallMachine", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkCallMachineClient", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkCallMachineClientState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCallMachine", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCallMachineGroupPrearranged", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttServerCallMachineGroupPrearrangedState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttPusher", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorParticipant", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorParticipantState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorArbitrator", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorTowardsParticipant", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorTowardsParticipantState", LOG_LEVEL_ALL);
      LogComponentEnable ("McpttOnNetworkFloorArbitratorState", LOG_LEVEL_ALL);
    }

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  Ptr<ImsHelper> imsHelper = CreateObject<ImsHelper> ();
  imsHelper->ConnectPgw (epcHelper->GetPgwNode ());

  // Create a second IMS and MCPTT server.  This requires an explicit
  // configuration of the IP network away from the default, because the
  // first helper took the default
  Ptr<ImsHelper> imsHelper2 = CreateObject<ImsHelper> ();
  imsHelper2->SetImsIpv4Network (Ipv4Address ("16.0.0.0"), Ipv4Mask ("255.0.0.0"));
  imsHelper2->ConnectPgw (epcHelper->GetPgwNode ());

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Configure the Internet around the remote host
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (epcHelper->GetPgwNode (), remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // Create a number of node containers to manage the configuration
  NodeContainer ueNodeGroup1;
  NodeContainer ueNodeGroup2;
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numEnb);
  ueNodeGroup1.Create (numUe / 2);
  ueNodeGroup2.Create (numUe / 2);
  ueNodes.Add (ueNodeGroup1);
  ueNodes.Add (ueNodeGroup2);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // eNB positions
  positionAlloc->Add (Vector (distance, 0, 0));
  positionAlloc->Add (Vector (distance * 2, 0, 0));
  // UE group 1 positions
  positionAlloc->Add (Vector (distance, 10, 0));
  positionAlloc->Add (Vector (distance * 2, 10, 0));
  positionAlloc->Add (Vector (distance * 3, 10, 0));
  // UE pair 2 positions
  positionAlloc->Add (Vector (distance, -10, 0));
  positionAlloc->Add (Vector (distance * 2, -10, 0));
  positionAlloc->Add (Vector (distance * 3, -10, 0));
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);
  mobility.Install (ueNodeGroup1);
  mobility.Install (ueNodeGroup2);

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
  lteHelper->Attach (ueLteDevs.Get (4), enbLteDevs.Get (0));
  lteHelper->Attach (ueLteDevs.Get (5), enbLteDevs.Get (1));

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer serverAppContainer;
  ApplicationContainer serverAppContainer2;
  McpttServerHelper mcpttServerHelper;

  serverAppContainer.Add (mcpttServerHelper.Install (imsHelper->GetImsNode ()));
  serverAppContainer.Start (appStartTime);
  serverAppContainer.Stop (appStopTime);

  serverAppContainer2.Add (mcpttServerHelper.Install (imsHelper2->GetImsNode ()));
  serverAppContainer2.Start (appStartTime);
  serverAppContainer2.Stop (appStopTime);

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

  clientAppContainer1.Add (mcpttClientHelper.Install (ueNodeGroup1));
  clientAppContainer2.Add (mcpttClientHelper.Install (ueNodeGroup2));
  clientAppContainer1.Start (appStartTime);
  clientAppContainer1.Stop (appStopTime);
  clientAppContainer2.Start (appStartTime);
  clientAppContainer2.Stop (appStopTime);
  clientAppContainer.Add (clientAppContainer1);
  clientAppContainer.Add (clientAppContainer2);

  Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverAppContainer.Get (0));
  Ipv4Address serverAddress = Ipv4Address::ConvertFrom (imsHelper->GetImsGmAddress ());
  serverApp->SetLocalAddress (serverAddress);
  NS_LOG_INFO ("server IMS IP address = " << serverAddress);

  Ptr<McpttServerApp> serverApp2 = DynamicCast<McpttServerApp> (serverAppContainer2.Get (0));
  Ipv4Address serverAddress2 = Ipv4Address::ConvertFrom (imsHelper2->GetImsGmAddress ());
  serverApp2->SetLocalAddress (serverAddress2);
  NS_LOG_INFO ("server 2 IMS IP address = " << serverAddress2);

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

  // Build the application containers used to configure each call.  The first
  // three UEs are part of the first call.  The second three UEs are part of
  // the second call, as well as the first user.
  ApplicationContainer callContainer1;
  callContainer1.Add (clientAppContainer1.Get (0));
  callContainer1.Add (clientAppContainer1.Get (1));
  callContainer1.Add (clientAppContainer1.Get (2));
  ApplicationContainer callContainer2;
  callContainer2.Add (clientAppContainer2.Get (0));
  callContainer2.Add (clientAppContainer2.Get (1));
  callContainer2.Add (clientAppContainer2.Get (2));
  // Add the first user to the second call also
  callContainer2.Add (clientAppContainer1.Get (0));

  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  uint32_t groupId = 1;
  callHelper.AddCall (callContainer1, serverApp, groupId, callType, callStartTime, callStopTime);

  groupId = 2;
  callHelper.AddCall (callContainer2, serverApp2, groupId, callType, callStartTime, callStopTime);

  // Configure the first UE to trace floor control and app events
  Ptr<McpttPttApp> pttApp = callContainer1.Get (0)->GetObject<McpttPttApp> ();
  pttApp->TraceConnectWithoutContext ("EventTrace", MakeCallback (&EventTrace));

  // schedule events to cause the user (McpttPusher) to switch calls at the
  // configured times in the simulation
  Simulator::Schedule (callStartTime, &McpttPttApp::SelectCall, pttApp, 1, true);
  Simulator::Schedule (firstSwitchTime, &McpttPttApp::SelectCall, pttApp, 2, true);
  Simulator::Schedule (secondSwitchTime, &McpttPttApp::SelectCall, pttApp, 1, true);

  NS_LOG_INFO ("Enabling MCPTT traces...");
  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();
  traceHelper->EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  traceHelper->EnableAccessTimeTrace ("mcptt-access-time.txt");

  // Uncomment to enable PCAP tracing
  // p2ph.EnablePcap("example-mcptt-on-network-two-simultaneous-calls.ims.pcap", imsHelper->GetImsGmDevice (), true, true);

  Simulator::Stop (simTime);
  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}
