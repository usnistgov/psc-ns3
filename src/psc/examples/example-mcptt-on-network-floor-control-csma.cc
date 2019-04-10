/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 * 
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 * 
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include "ns3/core-module.h"
#include <ns3/csma-module.h>
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/psc-module.h"
#include "ns3/stats-module.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Main");

void
SimTimeUpdate (void)
{
  NS_LOG_INFO ("Time: " << Simulator::Now ().GetSeconds () << "s");
}

int main (int argc, char *argv[])
{
  uint32_t appCount = 1;
  DataRate dataRate = DataRate ("24kb/s");
  TypeId floorTid = McpttFloorParticipant::GetTypeId ();
  double maxX = 5.0;
  double maxY = 5.0;
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double onOffMean = 5.0;
  double startSec = 2.0;
  double stopSec = 12.0;
  TypeId socketFacTid = UdpSocketFactory::GetTypeId ();

  CommandLine cmd;
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("max-x", "The largest x-coordinate that a UE can have.", maxX);
  cmd.AddValue ("max-y", "The largest y-coordinate that a UE can have.", maxY);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("on-off-mean", "The average number of seconds a user pushes and releases the button.", onOffMean);
  cmd.AddValue ("start-time", "The number of seconds into the simulation that the applications should start.", startSec);
  cmd.AddValue ("stop-time", "The number of seconds into the simulation that the applications should stop.", stopSec);
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::McpttMsgStats::CallControl", BooleanValue (true));
  Config::SetDefault ("ns3::McpttMsgStats::FloorControl", BooleanValue (true));
  Config::SetDefault ("ns3::McpttMsgStats::Media", BooleanValue (false));
  Config::SetDefault ("ns3::McpttMsgStats::IncludeMessageContent", BooleanValue (false));
  Config::SetDefault ("ns3::McpttOffNetworkFloorParticipant::GenMedia", BooleanValue (true));
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200")); //Disable fragmentation in wifi
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200")); //Disable fragmentation in wifi
 
  Time start = Seconds (startSec);
  Time stop = Seconds (stopSec);

  for (uint32_t s = (uint32_t)startSec; s < (uint32_t)stopSec; s+=1)
    {
      Simulator::Schedule (Seconds (s), &SimTimeUpdate);
    }

  NodeContainer nodes;
  NodeContainer servers;
  NodeContainer clients;
  servers.Create (1);
  clients.Create (appCount);
  nodes.Add (servers);
  nodes.Add (clients);

  NS_LOG_INFO ("Building physical topology...");
  Ptr<RandomBoxPositionAllocator> rndBoxPosAllocator = CreateObject <RandomBoxPositionAllocator> ();
  rndBoxPosAllocator->SetX (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxX)));
  rndBoxPosAllocator->SetY (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxY)));
  rndBoxPosAllocator->SetZ (CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1.5)));

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint32_t count = 0; count < nodes.GetN (); count++)
    {
      Vector position = rndBoxPosAllocator->GetNext ();

      NS_LOG_INFO ("Node " << (count + 1) << " located at " << position << ".");

      positionAlloc->Add (position);
    }

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (Seconds (0.001)));

  NetDeviceContainer devices;
  devices = csma.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces;
  interfaces = address.Assign (devices);

  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer serverApps;
  McpttOnNetworkFloorControlHelper mcpttServerHelper;
  mcpttServerHelper.SetApp ("ns3::McpttOnNetworkFloorServerApp");
  mcpttServerHelper.SetArbitrator ("ns3::McpttOnNetworkFloorArbitrator",
                         "AckRequired", BooleanValue (false),
                         "AudioCutIn", BooleanValue (false),
                         "DualFloorSupported", BooleanValue (true),
                         "TxSsrc", UintegerValue (100),
                         "McGranted", BooleanValue (false));
  mcpttServerHelper.SetDualControl ("ns3::McpttOnNetworkFloorDualControl");
  mcpttServerHelper.SetParticipant ("ns3::McpttOnNetworkFloorTowardsParticipant",
                         "McImplicitRequest", BooleanValue (false),
                         "McQueuing", BooleanValue (false),
                         "ReceiveOnly", BooleanValue (false));
  mcpttServerHelper.SetCallInfo ("ns3::McpttCallControlInfo",
                         "AmbientListening", BooleanValue (false),
                         "CallId", UintegerValue (1),
                         "CallTypeId", UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                         "TemporaryGroup", BooleanValue (false));

  serverApps.Add (mcpttServerHelper.Install (servers));
  serverApps.Start (start);
  serverApps.Stop (stop);

  ApplicationContainer clientApps;
  McpttHelper mcpttClientHelper;
  mcpttClientHelper.SetPttApp ("ns3::McpttPttApp",
                         "PushOnStart", BooleanValue (true));
  mcpttClientHelper.SetMediaSrc ("ns3::McpttMediaSrc",
                         "Bytes", UintegerValue (msgSize),
                         "DataRate", DataRateValue (dataRate));
  mcpttClientHelper.SetPusher ("ns3::McpttPusher",
                         "Automatic", BooleanValue (true));
  mcpttClientHelper.SetPusherPushVariable ("ns3::NormalRandomVariable",
                         "Mean", DoubleValue (onOffMean),
                         "Variance", DoubleValue (2.0));
  mcpttClientHelper.SetPusherReleaseVariable ("ns3::NormalRandomVariable",
                         "Mean", DoubleValue (onOffMean),
                         "Variance", DoubleValue (2.0));

  clientApps.Add (mcpttClientHelper.Install (clients));
  clientApps.Start (start);
  clientApps.Stop (stop);

  for (uint32_t idx = 0; idx < servers.GetN (); idx++)
    {
      Ptr<McpttOnNetworkFloorServerApp> serverApp = DynamicCast<McpttOnNetworkFloorServerApp, Application> (serverApps.Get (idx));
      Ipv4Address serverAddress = interfaces.GetAddress (idx);
      serverApp->SetLocalAddress (serverAddress);
      NS_LOG_INFO ("server " << idx << " ip address = " << serverAddress);
    }

  ObjectFactory callFac;
  callFac.SetTypeId ("ns3::McpttCallMachineNull");

  ObjectFactory floorFac;
  floorFac.SetTypeId ("ns3::McpttOnNetworkFloorParticipant");
  floorFac.Set ("McImplicitRequest", BooleanValue (false));
  floorFac.Set ("AckRequired", BooleanValue (false));
  floorFac.Set ("GenMedia", BooleanValue (true));

  for (uint32_t idx = 0; idx < clientApps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp, Application> (clientApps.Get (idx));

      Ipv4Address clientAddress = interfaces.GetAddress (idx + servers.GetN ());
      pttApp->SetLocalAddress (clientAddress);
      NS_LOG_INFO ("client " << idx << " ip address = " << clientAddress);

      pttApp->CreateCall (callFac, floorFac);
      pttApp->SelectLastCall ();
    }

  mcpttServerHelper.SetupFloorControl (serverApps, clientApps);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Enabling MCPTT traces...");
  mcpttClientHelper.EnableMsgTraces ();
  mcpttClientHelper.EnableStateMachineTraces ();

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (Seconds (stopSec + 2));
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Done.");

  return 0;
}
