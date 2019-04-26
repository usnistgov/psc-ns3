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
  cmd.Parse(argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  Config::SetDefault ("ns3::McpttMsgStats::CallControl", BooleanValue (true));
  Config::SetDefault ("ns3::McpttMsgStats::FloorControl", BooleanValue (true));
  Config::SetDefault ("ns3::McpttMsgStats::Media", BooleanValue (true));
  Config::SetDefault ("ns3::McpttMsgStats::IncludeMessageContent", BooleanValue (false));
  Config::SetDefault ("ns3::McpttOffNetworkFloorParticipant::GenMedia", BooleanValue (true));

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

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
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

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
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

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
      lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
      // side effect: the default EPS bearer will be activated
    }

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer serverApps;
  McpttOnNetworkFloorControlHelper mcpttServerHelper;
  mcpttServerHelper.SetApp ("ns3::McpttOnNetworkFloorServerApp");
  mcpttServerHelper.SetArbitrator ("ns3::McpttOnNetworkFloorArbitrator",
                         "AckRequired", BooleanValue (false),
                         "AudioCutIn", BooleanValue (false),
                         "DualFloorSupported", BooleanValue (false),
                         "TxSsrc", UintegerValue (100),
                         "QueueCapacity", UintegerValue (1),
                         "McGranted", BooleanValue (false));
  mcpttServerHelper.SetDualControl ("ns3::McpttOnNetworkFloorDualControl");
  mcpttServerHelper.SetParticipant ("ns3::McpttOnNetworkFloorTowardsParticipant",
                         "McImplicitRequest", BooleanValue (false),
                         "McQueuing", BooleanValue (true),
                         "ReceiveOnly", BooleanValue (false));
  mcpttServerHelper.SetCallInfo ("ns3::McpttCallControlInfo",
                         "AmbientListening", BooleanValue (false),
                         "CallId", UintegerValue (1),
                         "CallTypeId", UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                         "TemporaryGroup", BooleanValue (false));

  serverApps.Add (mcpttServerHelper.Install (remoteHostContainer));
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

  clientApps.Add (mcpttClientHelper.Install (ueNodes));
  clientApps.Start (start);
  clientApps.Stop (stop);

  for (uint32_t idx = 0; idx < serverApps.GetN (); idx++)
    {
      Ptr<McpttOnNetworkFloorServerApp> serverApp = DynamicCast<McpttOnNetworkFloorServerApp, Application> (serverApps.Get (idx));
      serverApp->SetLocalAddress (remoteHostAddr);
      NS_LOG_INFO ("server " << idx << " ip address = " << remoteHostAddr);
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

      Ipv4Address clientAddress = ueIpIface.GetAddress (idx);
      pttApp->SetLocalAddress (clientAddress);
      NS_LOG_INFO ("client " << idx << " ip address = " << clientAddress);

      //floorFac.Set ("Priority", UintegerValue ((idx + 6) % 7));

      pttApp->CreateCall (callFac, floorFac);
      pttApp->SelectLastCall ();
    }

  mcpttServerHelper.SetupFloorControl (serverApps, clientApps);

  NS_LOG_INFO ("Enabling MCPTT traces...");
  mcpttClientHelper.EnableMsgTraces ();
  mcpttClientHelper.EnableStateMachineTraces ();

  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-simple-epc");

  Simulator::Stop (simTime);
  Simulator::Run ();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}
