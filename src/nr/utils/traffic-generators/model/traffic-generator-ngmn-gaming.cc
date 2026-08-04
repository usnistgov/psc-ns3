// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-ngmn-gaming.h"

#include "ns3/address.h"
#include "ns3/boolean.h"
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

NS_LOG_COMPONENT_DEFINE("TrafficGeneratorNgmnGaming");
NS_OBJECT_ENSURE_REGISTERED(TrafficGeneratorNgmnGaming);

TypeId
TrafficGeneratorNgmnGaming::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGeneratorNgmnGaming")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGeneratorNgmnGaming>()
            .AddAttribute("IsDownlink",
                          "If set to true the traffic will be generated according to "
                          "parameters and model for gaming downlink, otherwise, if false,"
                          "it will be generated according to parameters and model for uplink.",
                          BooleanValue(true),
                          MakeBooleanAccessor(&TrafficGeneratorNgmnGaming::m_isDownlink),
                          MakeBooleanChecker())
            .AddAttribute("aParamPacketSizeUl",
                          "The a parameter in number of bytes for the packet size "
                          "calculation in uplink according to the NGMN white paper Annex A. The "
                          "packet size is "
                          "determined using Largest Extreme Value Distribution "
                          "(also known as Fisher-Tippett distribution) random variable.",
                          UintegerValue(45),
                          MakeUintegerAccessor(&TrafficGeneratorNgmnGaming::m_aParamPacketSizeUl),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("bParamPacketSizeUl",
                          "The b parameter in number of bytes for the packet size "
                          " calculation in uplink according to the NGMN white paper Annex A. The "
                          "packet size is "
                          "determined using Largest Extreme Value Distribution "
                          "(also known as Fisher-Tippett distribution) random variable.",
                          DoubleValue(5.7),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnGaming::m_bParamPacketSizeUl),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketArrivalUl",
                          "Packet arrival time in milliseconds for uplink. "
                          "Packet arrival in uplink is deterministic",
                          UintegerValue(40),
                          MakeUintegerAccessor(&TrafficGeneratorNgmnGaming::m_packetArrivalUl),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "aParamPacketSizeDl",
                "The a parameter in number of bytes for the packet size "
                "calculation in downlink according to NGMN white paper Annex A. The packet size is "
                "determined using the Largest Extreme Value Distribution "
                "(also known as Fisher-Tippett distribution) random variable.",
                UintegerValue(120),
                MakeUintegerAccessor(&TrafficGeneratorNgmnGaming::m_aParamPacketSizeDl),
                MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "bParamPacketSizeDl",
                "The b parameter in number of bytes for the packet size "
                "calculation in downlink according to NGMN white paper Annex A. The packet size is "
                "determined using the Largest Extreme Value Distribution "
                "(also known as Fisher-Tippett distribution) random variable.",
                DoubleValue(36),
                MakeDoubleAccessor(&TrafficGeneratorNgmnGaming::m_bParamPacketSizeDl),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "aParamPacketArrivalDl",
                "The a parameter for the packet arrival "
                "calculation in downlink according to NGMN white paper Annex A. The packet arrival "
                "in downlink is determined using Largest Extreme Value Distribution "
                "(also known as Fisher-Tippett distribution) random variable.",
                DoubleValue(55),
                MakeDoubleAccessor(&TrafficGeneratorNgmnGaming::m_aParamPacketArrivalDl),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "bParamPacketArrivalDl",
                "The b parameter for the packet arrival "
                "calculation in downlink according to NGMN white paper Annex A. The packet arrival"
                "in downlink is determined using Largest Extreme Value Distribution "
                "(also known as Fisher-Tippett distribution) random variable.",
                DoubleValue(5.7),
                MakeDoubleAccessor(&TrafficGeneratorNgmnGaming::m_bParamPacketArrivalDl),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "InitialPacketArrivalMin",
                "The minimum value in milliseconds for the "
                "initial packet arrival calculation according to NGMN white paper Annex A. "
                "The packet arrival in both, downlink and uplink, is determined using the "
                "Uniform Distribution.",
                UintegerValue(0),
                MakeUintegerAccessor(&TrafficGeneratorNgmnGaming::m_initialPacketArrivalMin),
                MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "InitialPacketArrivalMax",
                "The maximum value in milliseconds for the "
                "initial packet arrival calculation according to NGMN white paper Annex A. "
                "The packet arrival in both, downlink and uplink, is determined using the "
                "Uniform Distribution.",
                UintegerValue(40),
                MakeUintegerAccessor(&TrafficGeneratorNgmnGaming::m_initialPacketArrivalMax),
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

TrafficGeneratorNgmnGaming::TrafficGeneratorNgmnGaming()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGeneratorNgmnGaming::~TrafficGeneratorNgmnGaming()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGeneratorNgmnGaming::StartApplication()
{
    NS_LOG_FUNCTION(this);
    Simulator::Schedule(GetInitialPacketArrivalTime(), &TrafficGenerator::SendPacketBurst, this);
}

void
TrafficGeneratorNgmnGaming::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG(
        "This function should not be called for the gaming traffic"); // in NGMN description of the
                                                                      // gaming traffic there is no
                                                                      // notion of frames or packet
                                                                      // bursts, just packets
}

void
TrafficGeneratorNgmnGaming::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInPackets(1);
}

Time
TrafficGeneratorNgmnGaming::GetInitialPacketArrivalTime() const
{
    return MilliSeconds(ceil(m_initPacketArrivalVariable->GetValue()));
}

uint32_t
TrafficGeneratorNgmnGaming::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    // here we follow Annex A of NGMN white paper for gaming UL and DL
    // the packet size should be generated according to the largest Extreme Value Distribution
    // (also known as Fisher-Tippett distribution)
    // According to NGMN document, the values for this distribution can be
    // generated by the following procedure: y = a - b * ln (-ln (y)), where y is
    // drawn from a uniform random variable with range [0, 1]
    double y = m_packetSizeRandomVariable->GetValue();
    double x = 0;
    uint32_t a = 0;
    double b = 0.0;

    if (m_isDownlink)
    {
        a = m_aParamPacketSizeDl;
        b = m_bParamPacketSizeDl;
    }
    else
    {
        a = m_aParamPacketSizeUl;
        b = m_bParamPacketSizeUl;
    }
    x = a - b * log(-log(y));
    // Because packet size has to be integer number of bytes, the largest integer less than
    // or equal to x is used as the actual packet size
    return floor(x);
}

Time
TrafficGeneratorNgmnGaming::GetNextPacketTime() const
{
    NS_LOG_FUNCTION(this);
    // here we follow the Annex A of the NGMN white paper for gaming UL and DL
    // The packet arrival for the downlink should be generated according to the
    // largest Extreme Value Distribution (also known as Fisher-Tippett distribution)
    // According to the NGMN document, the values for this distribution can be
    // generated by the following procedure: y = a - b * ln (-ln (y)), where y is
    // drawn from a uniform random variable with range [0, 1]
    // The packet arrival for the uplink is deterministic and can be configured.
    Time x = MilliSeconds(0);
    if (m_isDownlink)
    {
        double y = m_packetArrivalVariable->GetValue();
        double a = m_aParamPacketArrivalDl;
        double b = m_bParamPacketArrivalDl;
        // Because we need here an integer number of milliseconds,
        /// the largest integer less than
        // or equal to x is used as the actual packet arrival time in ms
        x = MilliSeconds(floor(a - b * log(-log(y))));
    }
    else
    {
        x = MilliSeconds(m_packetArrivalUl);
    }
    return x;
}

void
TrafficGeneratorNgmnGaming::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_initPacketArrivalVariable = nullptr;
    m_packetSizeRandomVariable = nullptr;
    m_packetArrivalVariable = nullptr;

    // chain up
    TrafficGenerator::DoDispose();
}

void
TrafficGeneratorNgmnGaming::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_initPacketArrivalVariable = CreateObject<UniformRandomVariable>();
    m_initPacketArrivalVariable->SetAttribute("Min", DoubleValue(m_initialPacketArrivalMin));
    m_initPacketArrivalVariable->SetAttribute("Max", DoubleValue(m_initialPacketArrivalMax));
    m_packetSizeRandomVariable = CreateObject<UniformRandomVariable>();
    m_packetSizeRandomVariable->SetAttribute("Min", DoubleValue(0));
    m_packetSizeRandomVariable->SetAttribute("Max", DoubleValue(1));
    m_packetArrivalVariable = CreateObject<UniformRandomVariable>();
    m_packetArrivalVariable->SetAttribute("Min", DoubleValue(0));
    m_packetArrivalVariable->SetAttribute("Max", DoubleValue(1));
    // chain up
    TrafficGenerator::DoInitialize();
}

int64_t
TrafficGeneratorNgmnGaming::AssignStreams(int64_t stream)
{
    m_initPacketArrivalVariable->SetStream(stream);
    m_packetSizeRandomVariable->SetStream(stream + 1);
    m_packetArrivalVariable->SetStream(stream + 2);

    return 3;
}

} // Namespace ns3
