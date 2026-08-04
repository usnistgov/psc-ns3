// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-ngmn-video.h"

#include "ns3/address.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/socket.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TrafficGeneratorNgmnVideo");
NS_OBJECT_ENSURE_REGISTERED(TrafficGeneratorNgmnVideo);

uint32_t TrafficGeneratorNgmnVideo::m_flowIdCounter = 0;

TypeId
TrafficGeneratorNgmnVideo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGeneratorNgmnVideo")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGeneratorNgmnVideo>()
            .AddAttribute(
                "NumberOfPacketsInFrame",
                "Number of packets in frame",
                UintegerValue(20),
                MakeUintegerAccessor(&TrafficGeneratorNgmnVideo::m_numberOfPacketsInFrame),
                MakeUintegerChecker<uint32_t>(8))
            .AddAttribute("InterframeIntervalTime",
                          "Interframe interval time ",
                          TimeValue(MilliSeconds(100)),
                          MakeTimeAccessor(&TrafficGeneratorNgmnVideo::m_interframeIntervalTime),
                          MakeTimeChecker())
            .AddAttribute("PacketSizeScale",
                          "The scale parameter for the Pareto distribution "
                          "for the packet size generation",
                          DoubleValue(40),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetSizeScale),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketSizeShape",
                          "The shape parameter for the Pareto distribution "
                          "for the packet size generation",
                          DoubleValue(1.2),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetSizeShape),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketSizeBound",
                          "The bound parameter for the Pareto distribution "
                          "for the packet size generation",
                          DoubleValue(250),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetSizeBound),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketTimeScale",
                          "The scale parameter for the Pareto distribution "
                          "for the packet time generation",
                          DoubleValue(2.5),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetTimeScale),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketTimeShape",
                          "The shape parameter for the Pareto distribution "
                          "for the packet timee generation",
                          DoubleValue(1.2),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetTimeShape),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketTimeBound",
                          "The bound parameter for the Pareto distribution "
                          "for the packet time generation",
                          DoubleValue(12.5),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnVideo::m_packetTimeBound),
                          MakeDoubleChecker<double>())
            .AddAttribute("Remote",
                          "The address of the destination",
                          AddressValue(),
                          MakeAddressAccessor(&TrafficGenerator::SetRemote),
                          MakeAddressChecker())
            .AddAttribute("Protocol",
                          "The type of protocol to use.",
                          TypeIdValue(TcpSocketFactory::GetTypeId()),
                          MakeTypeIdAccessor(&TrafficGenerator::SetProtocol),
                          MakeTypeIdChecker())
            .AddTraceSource("Tx",
                            "A new packet is created and is sent",
                            MakeTraceSourceAccessor(&TrafficGenerator::m_txTrace),
                            "ns3::TrafficGenerator::TxTracedCallback");
    return tid;
}

TrafficGeneratorNgmnVideo::TrafficGeneratorNgmnVideo()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
    m_flowId = m_flowIdCounter++;
}

TrafficGeneratorNgmnVideo::~TrafficGeneratorNgmnVideo()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGeneratorNgmnVideo::StartApplication()
{
    NS_LOG_FUNCTION(this);
    SendPacketBurst();
}

uint32_t
TrafficGeneratorNgmnVideo::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    // We implement a bounded pareto (not truncated Pareto), to get the expected mean.
    // This way, if RV x (generated according to Pareto type I distribution) is lower
    // than the maximum value, x=max.
    // Also, in NGMN doc there is a typo in the scale value for video packet size,
    // which is 40B according to wifi doc IEEE 802.16m-08/004r2.
    uint32_t packetSize = floor(std::min(m_packetSizeGenerator->GetValue(), m_packetSizeBound));
    NS_LOG_DEBUG(" Next packet size :" << packetSize);
    return packetSize;
}

Time
TrafficGeneratorNgmnVideo::GetNextPacketTime() const
{
    NS_LOG_FUNCTION(this);
    // We implement a bounded pareto (not truncated Pareto), to get the expected mean.
    // This way, if RV x (generated according to Pareto type I distribution) is lower than the
    // maximum value, x=max.
    Time packetTime =
        Seconds(std::min(m_packetTimeGenerator->GetValue(), m_packetTimeBound) * 0.001);
    NS_LOG_DEBUG("Next packet time :" << packetTime.As(Time::MS));
    return packetTime;
}

void
TrafficGeneratorNgmnVideo::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    m_packetFrameCounter++;
    NS_LOG_INFO("Next frame to send: " << m_packetFrameCounter);
    // inter-frame interval time
    Simulator::Schedule(m_interframeIntervalTime, &TrafficGenerator::SendPacketBurst, this);
}

void
TrafficGeneratorNgmnVideo::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInPackets(m_numberOfPacketsInFrame);
}

void
TrafficGeneratorNgmnVideo::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_packetSizeGenerator = nullptr;
    m_packetTimeGenerator = nullptr;
    // chain up
    TrafficGenerator::DoDispose();
}

void
TrafficGeneratorNgmnVideo::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_packetSizeGenerator = CreateObject<ParetoRandomVariable>();
    m_packetSizeGenerator->SetAttribute("Scale", DoubleValue(m_packetSizeScale));
    m_packetSizeGenerator->SetAttribute("Shape", DoubleValue(m_packetSizeShape));
    m_packetTimeGenerator = CreateObject<ParetoRandomVariable>();
    m_packetTimeGenerator->SetAttribute("Scale", DoubleValue(m_packetTimeScale));
    m_packetTimeGenerator->SetAttribute("Shape", DoubleValue(m_packetTimeShape));
    // chain up
    TrafficGenerator::DoInitialize();
}

int64_t
TrafficGeneratorNgmnVideo::AssignStreams(int64_t stream)
{
    m_packetSizeGenerator->SetStream(stream);
    m_packetTimeGenerator->SetStream(stream + 1);

    return 2;
}

} // Namespace ns3
