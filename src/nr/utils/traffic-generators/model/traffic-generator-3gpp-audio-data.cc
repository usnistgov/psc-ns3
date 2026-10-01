// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-3gpp-audio-data.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TrafficGenerator3gppAudioData");
NS_OBJECT_ENSURE_REGISTERED(TrafficGenerator3gppAudioData);

TypeId
TrafficGenerator3gppAudioData::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGenerator3gppAudioData")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGenerator3gppAudioData>()
            .AddAttribute(
                "DataRate",
                "The desired data rate in Mbps. Typical values are 0.756 Mbps and 1.12 Mbps.",
                DoubleValue(0.756),
                MakeDoubleAccessor(&TrafficGenerator3gppAudioData::SetDataRate),
                MakeDoubleChecker<double>())
            .AddAttribute("Periodicity",
                          "The periodicity in milliseconds.",
                          UintegerValue(4),
                          MakeUintegerAccessor(&TrafficGenerator3gppAudioData::m_periodicity),
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

TrafficGenerator3gppAudioData::TrafficGenerator3gppAudioData()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGenerator3gppAudioData::~TrafficGenerator3gppAudioData()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGenerator3gppAudioData::DoInitialize()
{
    m_packetSize = (m_dataRate * 1e6 * m_periodicity * 1e-3) / 8;
    NS_ASSERT(m_packetSize);
    TrafficGenerator::DoInitialize();
}

void
TrafficGenerator3gppAudioData::StartApplication()
{
    NS_LOG_FUNCTION(this);
    SendPacketBurst();
}

void
TrafficGenerator3gppAudioData::SetDataRate(double dataRate)
{
    NS_LOG_FUNCTION(this);
    m_dataRate = dataRate;
}

void
TrafficGenerator3gppAudioData::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    // in 3GPP description of the Option 2 (video + audio/data) there is no notion of frames or
    // packet bursts, just packets
    NS_ABORT_MSG("This function should not be called for the video + audio/data traffic");
}

void
TrafficGenerator3gppAudioData::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInPackets(1);
}

uint32_t
TrafficGenerator3gppAudioData::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    return m_packetSize;
}

Time
TrafficGenerator3gppAudioData::GetNextPacketTime() const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_periodicity);
    NS_LOG_DEBUG("Next packet time in Milliseconds: " << m_periodicity);
    return MilliSeconds(m_periodicity);
}

int64_t
TrafficGenerator3gppAudioData::AssignStreams(int64_t stream)
{
    return 0;
}

} // Namespace ns3
