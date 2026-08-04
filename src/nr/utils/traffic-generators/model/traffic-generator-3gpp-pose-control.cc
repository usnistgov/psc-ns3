// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-3gpp-pose-control.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TrafficGenerator3gppPoseControl");
NS_OBJECT_ENSURE_REGISTERED(TrafficGenerator3gppPoseControl);

TypeId
TrafficGenerator3gppPoseControl::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGenerator3gppPoseControl")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGenerator3gppPoseControl>()
            .AddAttribute("PacketSize",
                          "The packet size in bytes.",
                          UintegerValue(100),
                          MakeUintegerAccessor(&TrafficGenerator3gppPoseControl::m_packetSize),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("Periodicity",
                          "The periodicity in milliseconds.",
                          UintegerValue(4),
                          MakeUintegerAccessor(&TrafficGenerator3gppPoseControl::m_periodicity),
                          MakeUintegerChecker<uint32_t>())
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

TrafficGenerator3gppPoseControl::TrafficGenerator3gppPoseControl()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGenerator3gppPoseControl::~TrafficGenerator3gppPoseControl()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGenerator3gppPoseControl::StartApplication()
{
    NS_LOG_FUNCTION(this);
    SendPacketBurst();
}

void
TrafficGenerator3gppPoseControl::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    // in 3GPP description of the pose/control traffic there is no notion of frames or packet
    // bursts, just packets
    NS_ABORT_MSG("This function should not be called for the pose control traffic");
}

void
TrafficGenerator3gppPoseControl::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInPackets(1);
}

uint32_t
TrafficGenerator3gppPoseControl::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    return m_packetSize;
}

Time
TrafficGenerator3gppPoseControl::GetNextPacketTime() const
{
    NS_LOG_FUNCTION(this);
    return MilliSeconds(m_periodicity);
}

int64_t
TrafficGenerator3gppPoseControl::AssignStreams(int64_t stream)
{
    return 0;
}

} // Namespace ns3
