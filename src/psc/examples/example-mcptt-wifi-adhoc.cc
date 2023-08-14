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

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("Main");

void
SimTimeUpdate (void)
{
  NS_LOG_INFO ("Time: " << Simulator::Now ().GetSeconds () << "s");
}

int main (int argc, char *argv[])
{
  uint32_t appCount;
  uint32_t groupCount = 1;
  uint32_t usersPerGroup = 2;
  DataRate dataRate = DataRate ("24kb/s");
  TypeId floorTid = McpttFloorParticipant::GetTypeId ();
  double maxX = 5.0;
  double maxY = 5.0;
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double onOffMean = 5.0;
  double startSec = 2.0;
  double stopSec = 12.0;
  TypeId socketFacTid = UdpSocketFactory::GetTypeId ();
  Ipv4Address peerAddress = Ipv4Address ("255.255.255.255");

  CommandLine cmd;
  cmd.AddValue ("users", "Number of users to include in a group.", usersPerGroup);
  cmd.AddValue ("groups", "Number of groups to include in the scenario.", groupCount);
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("max-x", "The largest x-coordinate that a UE can have.", maxX);
  cmd.AddValue ("max-y", "The largest y-coordinate that a UE can have.", maxY);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("on-off-mean", "The average number of seconds a user pushes and releases the button.", onOffMean);
  cmd.AddValue ("start-time", "The number of seconds into the simulation that the applications should start.", startSec);
  cmd.AddValue ("stop-time", "The number of seconds into the simulation that the applications should stop.", stopSec);
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::psc::McpttMsgStats::CallControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::FloorControl", BooleanValue (true));
  Config::SetDefault ("ns3::psc::McpttMsgStats::Media", BooleanValue (false));
  Config::SetDefault ("ns3::psc::McpttMsgStats::IncludeMessageContent", BooleanValue (false));
  Config::SetDefault ("ns3::psc::McpttOffNetworkFloorParticipant::GenMedia", BooleanValue (true));
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200")); //Disable fragmentation in wifi
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200")); //Disable fragmentation in wifi

  appCount = usersPerGroup * groupCount;

  Time start = Seconds (startSec);
  Time stop = Seconds (stopSec);

  for (uint32_t s = (uint32_t)startSec; s < (uint32_t)stopSec; s += 1)
    {
      Simulator::Schedule (Seconds (s), &SimTimeUpdate);
    }

  NodeContainer nodes;
  nodes.Create (appCount);

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211g); //2.4Ghz
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("ErpOfdmRate54Mbps"));

  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy;
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
  mcpttHelper.SetPttApp ("ns3::psc::McpttPttApp",
                         "PushOnStart", BooleanValue (true));
  mcpttHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                           "Bytes", UintegerValue (msgSize),
                           "DataRate", DataRateValue (dataRate));
  mcpttHelper.SetPusher ("ns3::psc::McpttPusher",
                         "Automatic", BooleanValue (true));
  mcpttHelper.SetPusherPttInterarrivalTimeVariable ("ns3::NormalRandomVariable",
                                                    "Mean", DoubleValue (onOffMean),
                                                    "Variance", DoubleValue (2.0));
  mcpttHelper.SetPusherPttDurationVariable ("ns3::NormalRandomVariable",
                                            "Mean", DoubleValue (onOffMean),
                                            "Variance", DoubleValue (2.0));

  clientApps.Add (mcpttHelper.Install (nodes));
  clientApps.Start (start);
  clientApps.Stop (stop);

  McpttCallHelper callHelper;
  callHelper.ConfigureOffNetworkBasicGrpCall (clientApps, peerAddress, usersPerGroup);

  NS_LOG_INFO ("Enabling MCPTT traces...");
  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (Seconds (stopSec + 2));
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Done.");

  return 0;
}
