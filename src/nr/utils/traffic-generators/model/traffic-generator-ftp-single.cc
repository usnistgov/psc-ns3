// Copyright (c) 2010 Georgia Institute of Technology
// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-ftp-single.h"

#include "ns3/address.h"
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

NS_LOG_COMPONENT_DEFINE("TrafficGeneratorFtpSingle");
NS_OBJECT_ENSURE_REGISTERED(TrafficGeneratorFtpSingle);

TypeId
TrafficGeneratorFtpSingle::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGeneratorFtpSingle")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGeneratorFtpSingle>()
            .AddAttribute("FileSize",
                          "The total number of bytes to send. The value zero means "
                          "that there is no limit.",
                          UintegerValue(0),
                          MakeUintegerAccessor(&TrafficGeneratorFtpSingle::SetFileSize),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("PacketSize",
                          "The number of bytes to write per socket send",
                          UintegerValue(512),
                          MakeUintegerAccessor(&TrafficGeneratorFtpSingle::SetPacketSize),
                          MakeUintegerChecker<uint32_t>(1))
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

TrafficGeneratorFtpSingle::TrafficGeneratorFtpSingle()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGeneratorFtpSingle::~TrafficGeneratorFtpSingle()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGeneratorFtpSingle::SetPacketSize(uint32_t sendSize)
{
    m_packetSize = sendSize;
}

void
TrafficGeneratorFtpSingle::SetFileSize(uint32_t fileSize)
{
    NS_LOG_FUNCTION(this << fileSize);
    m_fileSize = fileSize;
}

void
TrafficGeneratorFtpSingle::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInBytes(m_fileSize);
}

uint32_t
TrafficGeneratorFtpSingle::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    return m_packetSize;
}

void
TrafficGeneratorFtpSingle::DoDispose()
{
    NS_LOG_FUNCTION(this);
    // chain up
    TrafficGenerator::DoDispose();
}

void
TrafficGeneratorFtpSingle::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    // chain up
    TrafficGenerator::DoInitialize();
}

int64_t
TrafficGeneratorFtpSingle::AssignStreams(int64_t stream)
{
    return 0;
}

} // Namespace ns3
