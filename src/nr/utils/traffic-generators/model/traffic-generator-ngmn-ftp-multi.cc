// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-ngmn-ftp-multi.h"

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

NS_LOG_COMPONENT_DEFINE("TrafficGeneratorNgmnFtpMulti");
NS_OBJECT_ENSURE_REGISTERED(TrafficGeneratorNgmnFtpMulti);

TypeId
TrafficGeneratorNgmnFtpMulti::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGeneratorNgmnFtpMulti")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGeneratorNgmnFtpMulti>()
            .AddAttribute("MaxFileSize",
                          "Max file size in number of bytes",
                          UintegerValue(5e6),
                          MakeUintegerAccessor(&TrafficGeneratorNgmnFtpMulti::m_maxFileSize),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("PacketSize",
                          "The number of bytes to write per socket send",
                          UintegerValue(512),
                          MakeUintegerAccessor(&TrafficGeneratorNgmnFtpMulti::SetPacketSize),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("ReadingTimeMean",
                          "The mean reading time in seconds",
                          DoubleValue(180),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnFtpMulti::m_readingTimeMean),
                          MakeDoubleChecker<double>())
            .AddAttribute("FileSizeMu",
                          "Mu parameter of lognormal distribution "
                          "for the file size generation",
                          DoubleValue(14.45),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnFtpMulti::m_fileSizeMu),
                          MakeDoubleChecker<double>())
            .AddAttribute("FileSizeSigma",
                          "Sigma parameter of lognormal distribution "
                          "for the file size generation",
                          DoubleValue(0.35),
                          MakeDoubleAccessor(&TrafficGeneratorNgmnFtpMulti::m_fileSizeSigma),
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

TrafficGeneratorNgmnFtpMulti::TrafficGeneratorNgmnFtpMulti()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGeneratorNgmnFtpMulti::~TrafficGeneratorNgmnFtpMulti()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGeneratorNgmnFtpMulti::StartApplication()
{
    NS_LOG_FUNCTION(this);
    SendPacketBurst();
}

void
TrafficGeneratorNgmnFtpMulti::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    Time readingTime = GetNextReadingTime();
    NS_LOG_DEBUG("Next file transfer:" << readingTime);
    Simulator::Schedule(readingTime, &TrafficGenerator::SendPacketBurst, this);
}

void
TrafficGeneratorNgmnFtpMulti::SetPacketSize(uint32_t sendSize)
{
    m_packetSize = sendSize;
}

Time
TrafficGeneratorNgmnFtpMulti::GetNextReadingTime()
{
    NS_LOG_FUNCTION(this);
    return Seconds(m_readingTime->GetValue());
}

void
TrafficGeneratorNgmnFtpMulti::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    uint32_t fileSize = 0;
    while (true)
    {
        fileSize = m_fileSize->GetValue();
        if (fileSize <= m_maxFileSize)
        {
            break;
        }
        else
        {
            NS_LOG_DEBUG("Generated file size value is higher than the maximum "
                         "allowed value. Max value: "
                         << m_maxFileSize << ", generated value:" << fileSize);
        }
    }

    NS_LOG_DEBUG("New file size:" << fileSize);
    SetPacketBurstSizeInBytes(fileSize);
}

uint32_t
TrafficGeneratorNgmnFtpMulti::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    return m_packetSize;
}

void
TrafficGeneratorNgmnFtpMulti::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_readingTime = nullptr;
    m_fileSize = nullptr;
    // chain up
    TrafficGenerator::DoDispose();
}

void
TrafficGeneratorNgmnFtpMulti::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    // configure random number generators parameters
    m_readingTime = CreateObject<ExponentialRandomVariable>();
    m_readingTime->SetAttribute("Mean", DoubleValue(m_readingTimeMean));
    m_fileSize = CreateObject<LogNormalRandomVariable>();
    m_fileSize->SetAttribute("Mu", DoubleValue(m_fileSizeMu));
    m_fileSize->SetAttribute("Sigma", DoubleValue(m_fileSizeSigma)); // chain up
    TrafficGenerator::DoInitialize();
}

int64_t
TrafficGeneratorNgmnFtpMulti::AssignStreams(int64_t stream)
{
    m_readingTime->SetStream(stream);
    m_fileSize->SetStream(stream + 1);
    return 2;
}

} // Namespace ns3
