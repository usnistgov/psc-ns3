// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \file traffic-generator.cc
 * \ingroup examples
 * \brief Traffic generator example
 */

#include "ns3/core-module.h"
#include <ns3/abort.h>
#include <ns3/config.h>
#include <ns3/inet-socket-address.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv4-global-routing-helper.h>
#include <ns3/log.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/packet-sink.h>
#include <ns3/ping-helper.h>
#include <ns3/simple-channel.h>
#include <ns3/simple-net-device.h>
#include <ns3/simulator.h>
#include <ns3/traffic-generator-ftp-single.h>
#include <ns3/traffic-generator-helper.h>
#include <ns3/traffic-generator-ngmn-ftp-multi.h>
#include <ns3/traffic-generator-ngmn-gaming.h>
#include <ns3/traffic-generator-ngmn-video.h>
#include <ns3/traffic-generator-ngmn-voip.h>

#include <fstream>
#include <ostream>

using namespace ns3;

enum TrafficType
{
    NGMN_FTP,
    NGMN_VIDEO,
    NGMN_GAMING,
    NGMN_VOIP
};

static inline std::istream&
operator>>(std::istream& is, TrafficType& item)
{
    uint32_t inputValue;
    is >> inputValue;
    item = (TrafficType)inputValue;
    return is;
}

TypeId
GetTypeId(const TrafficType& item)
{
    switch (item)
    {
    case NGMN_FTP:
        return TrafficGeneratorNgmnFtpMulti::GetTypeId();
    case NGMN_VIDEO:
        return TrafficGeneratorNgmnVideo::GetTypeId();
    case NGMN_GAMING:
        return TrafficGeneratorNgmnGaming::GetTypeId();
    case NGMN_VOIP:
        return TrafficGeneratorNgmnVoip::GetTypeId();
    default:
        NS_ABORT_MSG("Unknown traffic type");
    };
}

std::string
GetName(const TrafficType& item)
{
    switch (item)
    {
    case NGMN_FTP:
        return "ftp";
    case NGMN_VIDEO:
        return "video";
    case NGMN_GAMING:
        return "gaming";
    case NGMN_VOIP:
        return "voip";
    default:
        NS_ABORT_MSG("Unknown traffic type");
    };
}

void
WriteBytesSent(Ptr<TrafficGenerator> trafficGenerator,
               uint64_t* previousBytesSent,
               uint64_t* previousWindowBytesSent,
               enum TrafficType trafficType,
               std::ofstream* outFileTx)
{
    uint64_t totalBytesSent = trafficGenerator->GetTotalBytes();
    (*outFileTx) << "\n"
                 << Simulator::Now().GetMilliSeconds() << "\t" << *previousWindowBytesSent
                 << std::endl;
    (*outFileTx) << "\n"
                 << Simulator::Now().GetMilliSeconds() << "\t"
                 << totalBytesSent - *previousBytesSent << std::endl;

    *previousWindowBytesSent = totalBytesSent - *previousBytesSent;
    *previousBytesSent = totalBytesSent;
};

void
WriteBytesReceived(Ptr<PacketSink> packetSink, uint64_t* previousBytesReceived)
{
    uint64_t totalBytesReceived = packetSink->GetTotalRx();
    *previousBytesReceived = totalBytesReceived;
};

int
main(int argc, char* argv[])
{
    enum TrafficType trafficType = NGMN_FTP;
    bool useUdp = false;
    uint32_t measWindowMs = 10;
    uint32_t appStartMs = 500;
    uint32_t appDurationMs = 100;

    CommandLine cmd(__FILE__);
    cmd.AddValue("trafficType",
                 "The traffic type to be configured. Currently the following options are "
                 "available: 0 (ftp), 1 (video), 2 (gaming) and 3 (voip).",
                 trafficType);
    cmd.AddValue("useUdp",
                 "if true, the NGMN applications will run over UDP connection, otherwise a TCP "
                 "connection will be used. ",
                 useUdp);
    cmd.AddValue("appStartMs", "Application start time in ms, greater than 500", appStartMs);
    cmd.AddValue("appDurationMs",
                 "Application duration time in ms, greater than 100",
                 appDurationMs);
    cmd.AddValue("measWindowMs", "Measurement window time in ms, greathen than 10", measWindowMs);

    cmd.Parse(argc, argv);

    NS_ASSERT(appStartMs >= 500);
    NS_ASSERT(appDurationMs >= 100);
    NS_ASSERT(measWindowMs >= 10);

    // configure the transport protocol to be used
    std::string transportProtocol;
    if (useUdp)
    {
        transportProtocol = "ns3::UdpSocketFactory";
    }
    else
    {
        transportProtocol = "ns3::TcpSocketFactory";
    }

    NodeContainer nodes;
    nodes.Create(2);
    InternetStackHelper internet;
    internet.Install(nodes);
    // link the two nodes
    Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice>();
    Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice>();
    nodes.Get(0)->AddDevice(txDev);
    nodes.Get(1)->AddDevice(rxDev);
    Ptr<SimpleChannel> channel1 = CreateObject<SimpleChannel>();
    rxDev->SetChannel(channel1);
    txDev->SetChannel(channel1);
    NetDeviceContainer devices;
    devices.Add(txDev);
    devices.Add(rxDev);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ipv4Interfaces = ipv4.Assign(devices);

    // install the packet sink at the receiver node
    uint16_t port = 4000;
    InetSocketAddress rxAddress(Ipv4Address::GetAny(), port);

    PacketSinkHelper packetSinkHelper(transportProtocol, rxAddress);

    // install the application on the rx device
    ApplicationContainer sinkApplication = packetSinkHelper.Install(nodes.Get(1));
    sinkApplication.Start(MilliSeconds(appStartMs));
    sinkApplication.Stop(MilliSeconds(appStartMs + appDurationMs));

    // install the traffic generator at the transmitter node
    TrafficGeneratorHelper trafficGeneratorHelper(
        transportProtocol,
        InetSocketAddress(ipv4Interfaces.GetAddress(1, 0), port),
        GetTypeId(trafficType));

    ApplicationContainer generatorApplication = trafficGeneratorHelper.Install(nodes.Get(0));
    generatorApplication.Start(MilliSeconds(appStartMs));
    generatorApplication.Stop(MilliSeconds(appStartMs + appDurationMs));

    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    PingHelper pingHelper(ipv4Interfaces.GetAddress(1, 0));
    ApplicationContainer pingApps = pingHelper.Install(nodes.Get(0));
    pingApps.Start(MilliSeconds(10));
    pingApps.Stop(MilliSeconds(500));

    Ptr<TrafficGenerator> trafficGenerator =
        generatorApplication.Get(0)->GetObject<TrafficGenerator>();
    Ptr<PacketSink> packetSink = sinkApplication.Get(0)->GetObject<PacketSink>();

    uint64_t previousBytesSent = 0;
    uint64_t previousBytesReceived = 0;
    uint64_t previousWindowBytesSent = 0;

    std::ofstream outFileTx;
    std::string txFileName = "tx-" + GetName(trafficType) + ".csv";
    outFileTx.open(txFileName.c_str(), std::ios_base::out);
    ;
    NS_ABORT_MSG_IF(!outFileTx.is_open(), "Can't open file " << txFileName);
    outFileTx.setf(std::ios_base::fixed);

    for (uint32_t i = appStartMs; i < appStartMs + appDurationMs; i = i + measWindowMs)
    {
        Simulator::Schedule(MilliSeconds(i),
                            &WriteBytesSent,
                            trafficGenerator,
                            &previousBytesSent,
                            &previousWindowBytesSent,
                            trafficType,
                            &outFileTx);
        Simulator::Schedule(MilliSeconds(i),
                            &WriteBytesReceived,
                            packetSink,
                            &previousBytesReceived);
    }

    Simulator::Stop(MilliSeconds(appStartMs + appDurationMs));
    Simulator::Run();
    Simulator::Destroy();

    outFileTx.close();
    std::cout << "\n Traffic generator example finished. Results written into " << txFileName
              << " file in the ns-3-dev root directory." << std::endl;

    return 0;
}
