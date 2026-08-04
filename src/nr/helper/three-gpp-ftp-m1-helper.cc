// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "three-gpp-ftp-m1-helper.h"

#include <ns3/packet-sink-helper.h>
#include <ns3/traffic-generator-ftp-single.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("ThreeGppFtpM1Helper");

ThreeGppFtpM1Helper::ThreeGppFtpM1Helper(ApplicationContainer* serverApps,
                                         ApplicationContainer* clientApps,
                                         NodeContainer* serverNodes,
                                         NodeContainer* clientNodes,
                                         Ipv4InterfaceContainer* serversIps)
{
    NS_LOG_FUNCTION(this);
    m_serverApps = serverApps;
    m_clientApps = clientApps;
    m_serverNodes = serverNodes;
    m_clientNodes = clientNodes;
    m_serversIps = serversIps;
}

ThreeGppFtpM1Helper::ThreeGppFtpM1Helper()
{
    NS_LOG_FUNCTION(this);
}

ThreeGppFtpM1Helper::~ThreeGppFtpM1Helper()
{
    NS_LOG_FUNCTION(this);
}

TypeId
ThreeGppFtpM1Helper::GetTypeId()
{
    static TypeId tid = TypeId("ns3::ThreeGppFtpM1Helper")
                            .SetParent<Object>()
                            .AddConstructor<ThreeGppFtpM1Helper>();
    return tid;
}

void
ThreeGppFtpM1Helper::DoConfigureFtpServers()
{
    NS_LOG_FUNCTION(this);
    Address apLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), m_port));
    PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", apLocalAddress);
    *m_serverApps = packetSinkHelper.Install(*m_serverNodes);
    m_serverApps->Start(m_serverStartTime);
}

void
ThreeGppFtpM1Helper::DoConfigureFtpClients()
{
    NS_LOG_FUNCTION(this);
    uint32_t ftpSegSize = 1448; // bytes
    TrafficGeneratorHelper ftpHelper("ns3::UdpSocketFactory",
                                     Address(),
                                     TrafficGeneratorFtpSingle::GetTypeId());
    ftpHelper.SetAttribute("PacketSize", UintegerValue(ftpSegSize));
    ftpHelper.SetAttribute("FileSize", UintegerValue(m_ftpFileSize));

    for (uint32_t i = 0; i < m_serversIps->GetN(); i++)
    {
        Ipv4Address ipAddress = m_serversIps->GetAddress(i, 0);
        AddressValue remoteAddress(InetSocketAddress(ipAddress, m_port));
        ftpHelper.SetAttribute("Remote", remoteAddress);
        m_clientApps->Add(ftpHelper.Install(*m_clientNodes));

        // Seed the ARP cache by pinging early in the simulation
        // This is a workaround until a static ARP capability is provided
        PingHelper ping(ipAddress);
        m_pingApps.Add(ping.Install(*m_clientNodes));
    }

    m_clientApps->Start(m_clientStartTime + Seconds(m_startJitter->GetValue()));
    // Add one or two pings for ARP at the beginning of the simulation
    m_pingApps.Start(Seconds(0.300) + Seconds(m_startJitter->GetValue()));
    m_pingApps.Stop(Seconds(0.500));
}

void
ThreeGppFtpM1Helper::DoStartFileTransfer()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_lastClient >= 0 && m_lastClient < m_clientApps->GetN());
    Ptr<Application> app = m_clientApps->Get(m_lastClient);
    NS_ASSERT(app);
    Ptr<TrafficGenerator> fileTransfer = DynamicCast<TrafficGenerator>(app);
    NS_ASSERT(fileTransfer);
    fileTransfer->SendPacketBurst();

    m_lastClient += 1;
    if (m_lastClient == m_clientApps->GetN())
    {
        m_lastClient = 0;
    }
    Simulator::Schedule(DoGetNextTime(), &ThreeGppFtpM1Helper::DoStartFileTransfer, this);
}

void
ThreeGppFtpM1Helper::Configure(uint16_t port,
                               Time serverStartTime,
                               Time clientStartTime,
                               Time clientStopTime,
                               double ftpLambda,
                               uint32_t ftpFileSize)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(m_boolConfigured, "Already configured FTP M1 helper.");
    NS_ABORT_MSG_IF(m_serverNodes->GetN() == 0 || m_clientNodes->GetN() == 0 ||
                        m_serversIps->GetN() == 0,
                    "Server and/or client nodes or IP server interfaces not set.");
    m_port = port;
    m_clientStartTime = clientStartTime;
    m_clientStopTime = clientStopTime;
    m_ftpLambda = ftpLambda;
    m_ftpFileSize = ftpFileSize;
    m_serverStartTime = serverStartTime;
    m_boolConfigured = true;

    m_ftpArrivals = CreateObject<ExponentialRandomVariable>();
    m_ftpArrivals->SetAttribute("Mean", DoubleValue(1 / m_ftpLambda));
    // Randomly distribute the start times across 100ms interval
    m_startJitter = CreateObject<UniformRandomVariable>();
    m_startJitter->SetAttribute("Max", DoubleValue(0.100));
}

void
ThreeGppFtpM1Helper::Start()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_boolConfigured);

    DoConfigureFtpServers();
    DoConfigureFtpClients();

    // Start file transfer arrival process in both networks
    Simulator::Schedule(m_clientStartTime + DoGetNextTime(),
                        &ThreeGppFtpM1Helper::DoStartFileTransfer,
                        this);
}

Time
ThreeGppFtpM1Helper::DoGetNextTime() const
{
    return Seconds(m_ftpArrivals->GetValue());
}

int64_t
ThreeGppFtpM1Helper::AssignStreams(int64_t stream)
{
    m_ftpArrivals->SetStream(stream);
    m_startJitter->SetStream(stream + 1);

    return 2;
}

} // namespace ns3
