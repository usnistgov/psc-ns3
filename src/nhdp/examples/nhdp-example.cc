/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nhdp-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NhdpExample");

int
main(int argc, char* argv[])
{
    LogComponentEnable("NhdpClient", LOG_LEVEL_ALL);
    LogComponentEnable("NhdpExample", LOG_LEVEL_ALL);

    CommandLine cmd;
    cmd.Parse(argc, argv);

    NS_LOG_INFO("Creating nodes...");
    NodeContainer nodes;
    nodes.Create(2);

    NS_LOG_INFO("Installing internet stack...");
    InternetStackHelper internet;
    internet.Install(nodes);

    /*
      NS_LOG_INFO ("Creating wifi channel...");
      YansWifiChannelHelper wifiChannelHelper = YansWifiChannelHelper::Default ();

      NS_LOG_INFO ("Creating wifi phy...");
      YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
      wifiPhyHelper.SetChannel (wifiChannelHelper.Create ());

      NS_LOG_INFO ("Creating wifi mac...");
      NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();
      wifiMacHelper.SetType ("ns3::AdhocWifiMac");

      WifiHelper wifiHelper = WifiHelper::Default ();
      wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211a);
      wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
          "DataMode", StringValue ("wifia-54mbs"));

      NS_LOG_INFO ("Creating wifi devices...");
      NetDeviceContainer wifiContainer = wifiHelper.Install (wifiPhyHelper,
          wifiMacHelper, nodes);
      NetDeviceContainer wifiContainer2 = wifiHelper.Install (wifiPhyHelper,
          wifiMacHelper, nodes);

      NS_LOG_INFO ("Assigning IP addresses.");
      Ipv4AddressHelper ipv4 ("10.1.1.0", "255.255.255.0");
      Ipv4InterfaceContainer ipv4container = ipv4.Assign (wifiContainer);

      Ipv4AddressHelper ipv4b ("10.1.2.0", "255.255.255.0");
      Ipv4InterfaceContainer ipv4container2 = ipv4b.Assign (wifiContainer2);
    */

    NS_LOG_INFO("Creating mobility model...");
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    NS_LOG_INFO("Installing applications...");
    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(nodes);

    /*
    NS_LOG_INFO ("Adding interfaces to NHDP...");
    for (NodeContainer::Iterator iter = nodes.Begin ();
        iter != nodes.End ();
        iter++)
      {
        Ptr<NhdpClient> nhdp = DynamicCast<NhdpClient> ((*iter)->GetApplication (0));
        nhdp->AddLocalInterface(1, true);
      }
    */

    NS_LOG_INFO("Starting simulation...");
    apps.Start(Seconds(1));
    apps.Stop(Seconds(10));

    Simulator::Stop(Seconds(20));
    Simulator::Run();
    std::cout << Now().GetSeconds() << std::endl;
    Simulator::Destroy();
}
