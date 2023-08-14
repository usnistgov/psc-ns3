/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/lte-module.h"
#include "ns3/psc-module.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;
using namespace psc;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeBs,
 * attaches one UE per eNodeB starts a flow for each UE to and from a remote host.
 * It also starts another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("LenaSimpleEpc");

int
main (int argc, char *argv[])
{
  DataRate dataRate = DataRate ("24kb/s");
  double distance = 60.0;
  uint16_t numNodePairs = 2;
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  double onOffMean = 5.0;
  Time simTime = Seconds (14.0);
  Time start = Seconds (2.0);
  Time stop = Seconds (12.0);
  bool useCa = false;

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("numNodePairs", "Number of eNodeBs + UE pairs", numNodePairs);
  cmd.AddValue ("on-off-mean", "The average number of seconds a user pushes and releases the button.", onOffMean);
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("start-time", "The number of seconds into the simulation that the applications should start.", start);
  cmd.AddValue ("stop-time", "The number of seconds into the simulation that the applications should stop.", stop);
  cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.Parse (argc, argv);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

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

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numNodePairs);
  ueNodes.Create (numNodePairs);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numNodePairs; i++)
    {
      positionAlloc->Add (Vector (distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);
  mobility.Install (ueNodes);

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
  for (uint16_t i = 0; i < numNodePairs; i++)
    {
      lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (i));
      // side effect: the default EPS bearer will be activated
    }

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer serverApps;
  McpttServerHelper mcpttServerHelper;
  serverApps.Add (mcpttServerHelper.Install (imsHelper->GetImsNode ()));
  serverApps.Start (start);
  serverApps.Stop (stop);
  // For use below
  Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverApps.Get (0));


  ApplicationContainer clientApps;
  McpttHelper mcpttClientHelper;
  mcpttClientHelper.SetPttApp ("ns3::psc::McpttPttApp",
                               "PushOnStart", BooleanValue (true));
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

  clientApps.Add (mcpttClientHelper.Install (ueNodes));
  clientApps.Start (start);
  clientApps.Stop (stop);

  for (uint32_t idx = 0; idx < serverApps.GetN (); idx++)
    {
      Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverApps.Get (idx));
      Ipv4Address serverAddress = Ipv4Address::ConvertFrom (imsHelper->GetImsGmAddress ());
      serverApp->SetLocalAddress (serverAddress);
      NS_LOG_INFO ("server " << idx << " IMS IP address = " << serverAddress);
    }

  ObjectFactory floorFac;
  floorFac.SetTypeId ("ns3::psc::McpttOnNetworkFloorParticipant");
  floorFac.Set ("ImplicitRequest", BooleanValue (false));

  for (uint32_t idx = 0; idx < clientApps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp> (clientApps.Get (idx));

      Ipv4Address clientAddress = ueIpIface.GetAddress (idx);
      pttApp->SetLocalAddress (clientAddress);
      NS_LOG_INFO ("client " << idx << " ip address = " << clientAddress);
    }

  McpttCallHelper callHelper;
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                            "AckRequired", BooleanValue (false),
                            "AudioCutIn", BooleanValue (false),
                            "DualFloorSupported", BooleanValue (false),
                            "TxSsrc", UintegerValue (100),
                            "QueueingSupported", BooleanValue (true));
  // Dual control not yet in the refactoring
  // callHelper.SetDualControl ("ns3::psc::McpttOnNetworkFloorDualControl");
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (false),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  uint32_t groupId = 1;
  callHelper.AddCall (clientApps, serverApp, groupId, callType, start, stop);

  NS_LOG_INFO ("Enabling MCPTT traces...");
  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();
  traceHelper->EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  traceHelper->EnableAccessTimeTrace ("mcptt-access-time.txt");

  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  // p2ph.EnablePcap("example-mcptt-on-network-floor-control-lte.ims.pcap", imsHelper->GetImsGmDevice (), true, true);

  Simulator::Stop (simTime);
  Simulator::Run ();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}
