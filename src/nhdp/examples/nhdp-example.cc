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
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("NhdpExample");

NetDeviceContainer CreateAdhocNetwork(NodeContainer c, Ssid ssid);

int
main(int argc, char* argv[])
{
    bool verbose{true};
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(10)};

    CommandLine cmd;
    cmd.AddValue("verbose", "turn on log components", verbose);
    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnableAll(
            LogLevel(LOG_PREFIX_FUNC | LOG_PREFIX_LEVEL | LOG_PREFIX_TIME | LOG_PREFIX_NODE));
        LogComponentEnable("NhdpClient", LOG_LEVEL_ALL);
    }
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    NS_LOG_INFO("Creating nodes...");
    NodeContainer nodes;
    nodes.Create(2);

    NS_LOG_INFO("Creating mobility model...");
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    auto devices = CreateAdhocNetwork(nodes, Ssid("nhdp-example"));

    NS_LOG_INFO("Installing internet stack...");
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(devices, Ipv4Address("7.0.0.1"));

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

    YansWifiPhyHelper phy;
    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    phy.EnablePcap("nhdp-example", devices);

    NS_LOG_INFO("Starting simulation...");
    apps.Start(startTime);
    apps.Stop(stopTime);

    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

NetDeviceContainer
CreateAdhocNetwork(NodeContainer c, Ssid ssid)
{
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("OfdmRate54Mbps"));
    return wifi.Install(wifiPhy, wifiMac, c);
}
