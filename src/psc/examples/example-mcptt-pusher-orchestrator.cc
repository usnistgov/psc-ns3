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
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/psc-module.h"
#include "ns3/wifi-module.h"

#include <iostream>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Main");

bool s_verbose = false;
Time s_totalSpurtTime = Seconds (0);
uint32_t s_totalPtts = 0;
uint32_t s_totalContentions = 0;
Time s_totalSessionDurationTime = Seconds (0);
std::map<std::string, Time> s_eventSpans;

void
PttDurationCallback (Time duration)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - PTT Duration: " << duration.GetSeconds () << std::endl;
    }
  s_totalPtts++;
}

void
PttIatCallback (Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - PTT IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
ContentionPttDurationCallback (Time duration)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Contention PTT Duration: " << duration.GetSeconds () << std::endl;
    }
  s_totalContentions++;
}

void
ContentionPttIatCallback (Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Contention PTT IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
SessionDurationCallback (Time duration)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Session Duration: " << duration.GetSeconds () << std::endl;
    }
}

void
SessionIatCallback (Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Session IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
PusherStateCallback (std::string context, bool oldState, bool newState)
{
  if (newState)
    {
      s_eventSpans[context] = Simulator::Now ();
    }
  else if (s_eventSpans[context] != Seconds (0))
    {
      s_totalSpurtTime += Simulator::Now () - s_eventSpans[context];
      s_eventSpans[context] = Seconds (0);
    }
 
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << " Pusher " << context << "'s state = " << (newState ? "pushed" : "released") << std::endl;
    }
}

void
SessionStateCallback (bool oldState, bool newState)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << " Session = " << (newState ? "active" : "inactive") << std::endl;
    }

  if (newState)
    {
      s_eventSpans["session"] = Simulator::Now ();
    }
  else if (s_eventSpans["session"] != Seconds (0))
    {
      s_totalSessionDurationTime += Simulator::Now () - s_eventSpans["session"];
      s_eventSpans["session"] = Seconds (0);
    }
}

int main (int argc, char *argv[])
{
  uint32_t appCount = 2;
  DataRate dataRate = DataRate ("24kb/s");
  TypeId floorTid = McpttFloorParticipant::GetTypeId ();
  double maxX = 5.0;
  double maxY = 5.0;
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double startSec = 2.0;
  double stopSec = 62.0;
  double vaf = 1.0;
  double cp = 0.0;
  double saf = 0.5;
  TypeId socketFacTid = UdpSocketFactory::GetTypeId ();
  Ipv4Address peerAddress = Ipv4Address ("255.255.255.255");

  CommandLine cmd;
  cmd.AddValue ("users", "Number of users to include in the group.", appCount);
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("max-x", "The largest x-coordinate that a UE can have.", maxX);
  cmd.AddValue ("max-y", "The largest y-coordinate that a UE can have.", maxY);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("start-time", "The number of seconds into the simulation that the applications should start.", startSec);
  cmd.AddValue ("stop-time", "The number of seconds into the simulation that the applications should stop.", stopSec);
  cmd.AddValue ("vaf", "The voice activity factor to use for the orchestrator.", vaf);
  cmd.AddValue ("cp", "The probability of contention.", cp);
  cmd.AddValue ("saf", "The session activity factor to use for the orchestrator.", saf);
  cmd.AddValue ("verbose", "Whether or not to print out PTT duration and PTT IAT information.", s_verbose);
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::McpttMsgStats::CallControl", BooleanValue (false));
 
  Time start = Seconds (startSec);
  Time stop = Seconds (stopSec);

  NodeContainer nodes;
  nodes.Create (appCount);

  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g); //2.4Ghz
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("ErpOfdmRate6Mbps"));
 
  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel",
                                "Frequency", DoubleValue (2.407e9)); //2.4Ghz

  wifiMac.SetType ("ns3::AdhocWifiMac");

  YansWifiPhyHelper phy = wifiPhy;
  phy.SetChannel (wifiChannel.Create ());

  WifiMacHelper mac = wifiMac;
  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  NS_LOG_INFO ("Building physical topology...");
  Ptr<RandomBoxPositionAllocator> rndBoxPosAllocator = CreateObject <RandomBoxPositionAllocator> ();
  rndBoxPosAllocator->SetX (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxX)));
  rndBoxPosAllocator->SetY (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxY)));
  rndBoxPosAllocator->SetZ (CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1.5)));

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint32_t count = 0; count < appCount; count++)
    {
      Vector position = rndBoxPosAllocator->GetNext ();

      NS_LOG_INFO ("UE " << (count + 1) << " located at " << position << ".");

      positionAlloc->Add (position);
    }

  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  NS_LOG_INFO ("Installing internet stack on all nodes...");
  InternetStackHelper internet;
  internet.Install (nodes);

  NS_LOG_INFO ("Assigning IP addresses to each net device...");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer clientApps;
  McpttHelper mcpttHelper;
  mcpttHelper.SetPttApp ("ns3::McpttPttApp",
                         "PushOnStart", BooleanValue (true));
  mcpttHelper.SetMediaSrc ("ns3::McpttMediaSrc",
                         "Bytes", UintegerValue (msgSize),
                         "DataRate", DataRateValue (dataRate));
  mcpttHelper.SetPusher ("ns3::McpttPusher",
                         "Automatic", BooleanValue (false));

  clientApps.Add (mcpttHelper.Install (nodes));
  clientApps.Start (start);
  clientApps.Stop (stop);

  McpttCallHelper callHelper;
  callHelper.ConfigureOffNetworkBasicGrpCall (clientApps, peerAddress, appCount);

  Ptr<McpttPusherOrchestratorCdf> cdfOrchestrator = CreateObject<McpttPusherOrchestratorCdf> ();
  cdfOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (vaf));
  cdfOrchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback(&PttDurationCallback));
  cdfOrchestrator->TraceConnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback(&PttIatCallback));

  Ptr<McpttPusherOrchestratorContention> contentionOrchestrator = CreateObject<McpttPusherOrchestratorContention> ();
  contentionOrchestrator->SetAttribute ("ContentionProbability", DoubleValue (cp));
  contentionOrchestrator->SetAttribute ("Orchestrator", PointerValue (cdfOrchestrator));
  contentionOrchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback(&ContentionPttDurationCallback));
  contentionOrchestrator->TraceConnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback(&ContentionPttIatCallback));

  Ptr<McpttPusherOrchestratorSessionCdf> sessionOrchestrator = CreateObject<McpttPusherOrchestratorSessionCdf> ();
  sessionOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (saf));
  sessionOrchestrator->SetAttribute ("Orchestrator", PointerValue (contentionOrchestrator));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionDurationTrace", MakeCallback(&SessionDurationCallback));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionInterarrivalTimeTrace", MakeCallback(&SessionIatCallback));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionState", MakeCallback(&SessionStateCallback));

  sessionOrchestrator->StartAt (start);
  sessionOrchestrator->StopAt (stop);

  mcpttHelper.AddPushersToOrchestrator (sessionOrchestrator, clientApps);

  for (uint32_t appIdx = 0; appIdx < clientApps.GetN (); appIdx++)
    {
      std::stringstream ss;
      ss << (appIdx + 1);
      Ptr<McpttPusher> pusher = clientApps.Get (appIdx)->GetObject<McpttPttApp> ()->GetPusher ();
      pusher->TraceConnect ("PushingState", ss.str (), MakeCallback(&PusherStateCallback));
    }

  NS_LOG_INFO ("Enabling MCPTT traces...");
  McpttTraceHelper traceHelper;
  traceHelper.EnableMsgTraces ();
  traceHelper.EnableStateMachineTraces ();

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (Seconds (stopSec + 2));
  Simulator::Run ();
  Simulator::Destroy ();

  if (s_verbose)
    {
      std::cout << "Talkspurt time = " << s_totalSpurtTime.GetSeconds () << " s (" << (s_totalSpurtTime.GetSeconds () / (stopSec - startSec)) * 100.0 << " %)" << std::endl;
      std::cout << "Session time = " << s_totalSessionDurationTime.GetSeconds () << " s (" << (s_totalSessionDurationTime.GetSeconds () / (stopSec - startSec)) * 100.0 << " %)" << std::endl;
      std::cout << "Contention = " << s_totalContentions << " (" << ((double)s_totalContentions / s_totalPtts) * 100.0 << " %)" << std::endl;
    }

  NS_LOG_INFO ("Done.");

  return 0;
}
