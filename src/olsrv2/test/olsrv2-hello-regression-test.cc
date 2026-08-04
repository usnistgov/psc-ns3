/*
 * Copyright (c) 2009 IITP RAS
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Pavel Boyko <boyko@iitp.ru>
 */

#include "olsrv2-hello-regression-test.h"

#include "ns3/abort.h"
#include "ns3/double.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-raw-socket-factory.h"
#include "ns3/olsrv2-header.h"
#include "ns3/olsrv2-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/simple-net-device-helper.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/string.h"
#include "ns3/udp-header.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/uinteger.h"

#include <vector>

namespace ns3
{
namespace olsrv2
{

Olsrv2HelloRegressionTest::Olsrv2HelloRegressionTest()
    : TestCase("Test OLSR Hello messages generation"),
      m_time(Seconds(5)),
      m_countA(0),
      m_countB(0)
{
}

Olsrv2HelloRegressionTest::~Olsrv2HelloRegressionTest()
{
}

void
Olsrv2HelloRegressionTest::DoRun()
{
    RngSeedManager::SetSeed(12345);
    RngSeedManager::SetRun(7);
    CreateNodes();

    Simulator::Stop(m_time);
    Simulator::Run();

    m_rxSocketA = nullptr;
    m_rxSocketB = nullptr;
    Simulator::Destroy();
}

void
Olsrv2HelloRegressionTest::CreateNodes()
{
    // create 2 nodes
    NodeContainer c;
    c.Create(2);
    // install TCP/IP & OLSR
    Olsrv2Helper olsr;
    InternetStackHelper internet;
    internet.SetRoutingHelper(olsr);
    internet.Install(c);
    // Assign OLSR RVs to specific streams
    int64_t streamsUsed = olsr.AssignStreams(c, 0);
    NS_TEST_ASSERT_MSG_EQ(streamsUsed, 2, "Should have assigned 2 streams");
    // create channel & devices
    SimpleNetDeviceHelper simpleNetHelper;
    simpleNetHelper.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    simpleNetHelper.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer nd = simpleNetHelper.Install(c);
    // setup IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    ipv4.Assign(nd);

    // Create the sockets
    Ptr<SocketFactory> rxSocketFactoryA = c.Get(0)->GetObject<Ipv4RawSocketFactory>();
    m_rxSocketA = DynamicCast<Ipv4RawSocketImpl>(rxSocketFactoryA->CreateSocket());
    m_rxSocketA->SetProtocol(UdpL4Protocol::PROT_NUMBER);
    m_rxSocketA->SetRecvCallback(MakeCallback(&Olsrv2HelloRegressionTest::ReceivePktProbeA, this));

    Ptr<SocketFactory> rxSocketFactoryB = c.Get(1)->GetObject<Ipv4RawSocketFactory>();
    m_rxSocketB = DynamicCast<Ipv4RawSocketImpl>(rxSocketFactoryB->CreateSocket());
    m_rxSocketB->SetProtocol(UdpL4Protocol::PROT_NUMBER);
    m_rxSocketB->SetRecvCallback(MakeCallback(&Olsrv2HelloRegressionTest::ReceivePktProbeB, this));
}

void
Olsrv2HelloRegressionTest::ReceivePktProbeA(Ptr<Socket> socket)
{
    uint32_t availableData;
    availableData = socket->GetRxAvailable();
    Ptr<Packet> receivedPacketProbe = socket->Recv(std::numeric_limits<uint32_t>::max(), 0);
    NS_ASSERT(availableData == receivedPacketProbe->GetSize());

    Ipv4Header ipHdr;
    receivedPacketProbe->RemoveHeader(ipHdr);
    UdpHeader udpHdr;
    receivedPacketProbe->RemoveHeader(udpHdr);
    PacketHeader pktHdr;
    receivedPacketProbe->RemoveHeader(pktHdr);
    MessageHeader msgHdr;
    receivedPacketProbe->RemoveHeader(msgHdr);

    const olsrv2::MessageHeader::Hello& hello = msgHdr.GetHello();
    NS_TEST_EXPECT_MSG_EQ(msgHdr.GetOriginatorAddress(),
                          Ipv4Address("10.1.1.2"),
                          "Originator address.");

    if (m_countA == 0)
    {
        NS_TEST_EXPECT_MSG_EQ(hello.linkMessages.size(), 0, "No Link messages on the first Hello.");
    }
    else
    {
        NS_TEST_EXPECT_MSG_EQ(hello.linkMessages.size(),
                              1,
                              "One Link message on the second and third Hello.");
    }

    for (auto iter = hello.linkMessages.begin(); iter != hello.linkMessages.end(); iter++)
    {
        if (m_countA == 1)
        {
            NS_TEST_EXPECT_MSG_EQ(iter->linkCode, 1, "Asymmetric link on second Hello.");
        }
        else
        {
            NS_TEST_EXPECT_MSG_EQ(iter->linkCode, 6, "Symmetric link on second Hello.");
        }

        NS_TEST_EXPECT_MSG_EQ(iter->neighborInterfaceAddresses.size(), 1, "Only one neighbor.");
        NS_TEST_EXPECT_MSG_EQ(iter->neighborInterfaceAddresses[0],
                              Ipv4Address("10.1.1.1"),
                              "Only one neighbor.");
    }

    m_countA++;
}

void
Olsrv2HelloRegressionTest::ReceivePktProbeB(Ptr<Socket> socket)
{
    uint32_t availableData;
    availableData = socket->GetRxAvailable();
    Ptr<Packet> receivedPacketProbe = socket->Recv(std::numeric_limits<uint32_t>::max(), 0);
    NS_ASSERT(availableData == receivedPacketProbe->GetSize());

    Ipv4Header ipHdr;
    receivedPacketProbe->RemoveHeader(ipHdr);
    UdpHeader udpHdr;
    receivedPacketProbe->RemoveHeader(udpHdr);
    PacketHeader pktHdr;
    receivedPacketProbe->RemoveHeader(pktHdr);
    MessageHeader msgHdr;
    receivedPacketProbe->RemoveHeader(msgHdr);

    const olsrv2::MessageHeader::Hello& hello = msgHdr.GetHello();
    NS_TEST_EXPECT_MSG_EQ(msgHdr.GetOriginatorAddress(),
                          Ipv4Address("10.1.1.1"),
                          "Originator address.");

    if (m_countA == 0)
    {
        NS_TEST_EXPECT_MSG_EQ(hello.linkMessages.size(), 0, "No Link messages on the first Hello.");
    }
    else
    {
        NS_TEST_EXPECT_MSG_EQ(hello.linkMessages.size(),
                              1,
                              "One Link message on the second and third Hello.");
    }

    for (auto iter = hello.linkMessages.begin(); iter != hello.linkMessages.end(); iter++)
    {
        if (m_countA == 1)
        {
            NS_TEST_EXPECT_MSG_EQ(iter->linkCode, 1, "Asymmetric link on second Hello.");
        }
        else
        {
            NS_TEST_EXPECT_MSG_EQ(iter->linkCode, 6, "Symmetric link on second Hello.");
        }

        NS_TEST_EXPECT_MSG_EQ(iter->neighborInterfaceAddresses.size(), 1, "Only one neighbor.");
        NS_TEST_EXPECT_MSG_EQ(iter->neighborInterfaceAddresses[0],
                              Ipv4Address("10.1.1.2"),
                              "Only one neighbor.");
    }

    m_countB++;
}

} // namespace olsrv2
} // namespace ns3
