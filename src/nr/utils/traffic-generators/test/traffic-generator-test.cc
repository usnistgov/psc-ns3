// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-test.h"

#include <ns3/boolean.h>
#include <ns3/ping-helper.h>
#include <ns3/rng-seed-manager.h>

namespace ns3
{

bool TGT_ENABLE_PRINTING =
    false; // variable that allows printing of tested generated random values to files

TrafficGeneratorTestCase::TrafficGeneratorTestCase(std::string name,
                                                   TypeId trafficGeneratorType,
                                                   std::string transportProtocol)
    : TestCase("(TX bytes == RX bytes) when " + name)
{
    m_trafficGeneratorType = trafficGeneratorType;
    m_transportProtocol = transportProtocol;
}

TrafficGeneratorTestCase::~TrafficGeneratorTestCase()
{
}

void
TrafficGeneratorTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

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

    PacketSinkHelper packetSinkHelper(m_transportProtocol, rxAddress);

    // install the application on the rx device
    ApplicationContainer sinkApplication = packetSinkHelper.Install(nodes.Get(1));
    sinkApplication.Start(Seconds(1));
    sinkApplication.Stop(Seconds(4));

    // install the traffic generator at the transmitter node
    TrafficGeneratorHelper trafficGeneratorHelper(
        m_transportProtocol,
        InetSocketAddress(ipv4Interfaces.GetAddress(1, 0), port),
        m_trafficGeneratorType);

    ApplicationContainer generatorApplication = trafficGeneratorHelper.Install(nodes.Get(0));
    generatorApplication.Start(Seconds(2));
    generatorApplication.Stop(Seconds(3));

    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    PingHelper pingHelper(ipv4Interfaces.GetAddress(1, 0));
    ApplicationContainer pingApps = pingHelper.Install(nodes.Get(0));
    pingApps.Start(Seconds(1));
    pingApps.Stop(Seconds(2));

    Ptr<TrafficGenerator> trafficGenerator =
        generatorApplication.Get(0)->GetObject<TrafficGenerator>();
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);

    Simulator::Run();

    uint64_t totalBytesSent = trafficGenerator->GetTotalBytes();

    Ptr<PacketSink> packetSink = sinkApplication.Get(0)->GetObject<PacketSink>();
    uint64_t totalBytesReceived = packetSink->GetTotalRx();

    NS_TEST_ASSERT_MSG_EQ(totalBytesSent, totalBytesReceived, "Packets were lost !");

    Simulator::Destroy();
}

TrafficGeneratorNgmnFtpTestCase::TrafficGeneratorNgmnFtpTestCase()
    : TestCase(
          "(The mean file size == 2MBytes) && (The mean reading time == 180 seconds) for NGMN FTP")
{
}

TrafficGeneratorNgmnFtpTestCase::~TrafficGeneratorNgmnFtpTestCase()
{
}

void
TrafficGeneratorNgmnFtpTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    Ptr<TrafficGeneratorNgmnFtpMulti> trafficGenerator =
        CreateObject<TrafficGeneratorNgmnFtpMulti>();
    // we need to call it manually because in this test since we do not run the simulation, nothing
    // will call DoInitialize
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);
    uint64_t totalFileSizeBytes = 0;
    Time totalReadingTime = Seconds(0);
    uint16_t repetitions = 1000;

    //////////////////////

    std::ofstream outFileFtpReadingTime;
    std::ofstream outFileFtpFileSize;

    if (TGT_ENABLE_PRINTING)
    {
        std::string fileNameReadingTime = "ftp-reading-time.csv";
        outFileFtpReadingTime.open(fileNameReadingTime.c_str(), std::ios_base::out);

        NS_ABORT_MSG_IF(!outFileFtpReadingTime.is_open(),
                        "Can't open file " << fileNameReadingTime);
        outFileFtpReadingTime.setf(std::ios_base::fixed);

        std::string fileNameFileSize = "ftp-file-size.csv";
        outFileFtpFileSize.open(fileNameFileSize.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileFtpFileSize.is_open(), "Can't open file " << fileNameFileSize);
        outFileFtpFileSize.setf(std::ios_base::fixed);
    }

    for (uint16_t i = 0; i < repetitions; i++)
    {
        trafficGenerator->GenerateNextPacketBurstSize();
        uint32_t fileSize = trafficGenerator->GetPacketBurstSizeInBytes();
        totalFileSizeBytes += fileSize;
        Time readingTime = trafficGenerator->GetNextReadingTime();
        totalReadingTime += readingTime;

        if (TGT_ENABLE_PRINTING)
        {
            outFileFtpFileSize << fileSize << "\n";
            outFileFtpReadingTime << readingTime.GetSeconds() << "\n";
        }
    }

    if (TGT_ENABLE_PRINTING)
    {
        outFileFtpFileSize.close();
        outFileFtpReadingTime.close();
    }

    uint64_t averageFileSize = totalFileSizeBytes / repetitions;
    Time averageReadingTime = totalReadingTime / repetitions;
    // According to the NMGN white paper the mean value should be aprox. 2MBytes
    NS_TEST_ASSERT_MSG_EQ_TOL(averageFileSize,
                              2e6,
                              2e6 * 0.1,
                              "The mean FTP file size is not according to the NGMN white paper.");
    NS_TEST_ASSERT_MSG_EQ_TOL(averageReadingTime,
                              Seconds(180),
                              Seconds(180 * 0.1),
                              "The mean reading time according to the NGMN white paper.");
    Simulator::Destroy();
}

TrafficGeneratorNgmnVideoTestCase::TrafficGeneratorNgmnVideoTestCase()
    : TestCase("(The mean packet size == 100 Bytes) && (The mean packet arrival time == 6 ms) for "
               "NGMN VIDEO")
{
}

TrafficGeneratorNgmnVideoTestCase::~TrafficGeneratorNgmnVideoTestCase()
{
}

void
TrafficGeneratorNgmnVideoTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    Ptr<TrafficGeneratorNgmnVideo> trafficGenerator = CreateObject<TrafficGeneratorNgmnVideo>();
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);
    uint64_t totalPacketSize = 0;
    Time totalPacketTime = Seconds(0);
    uint16_t repetitions = 1000;

    std::ofstream outFileVideoPacketSize;
    std::ofstream outFileVideoPacketTime;

    if (TGT_ENABLE_PRINTING)
    {
        std::string fileNameVideoPacketSize = "video-packet-size.csv";
        outFileVideoPacketSize.open(fileNameVideoPacketSize.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileVideoPacketSize.is_open(),
                        "Can't open file " << fileNameVideoPacketSize);
        outFileVideoPacketSize.setf(std::ios_base::fixed);

        std::string fileNameVideoPacketTime = "video-packet-time.csv";
        outFileVideoPacketTime.open(fileNameVideoPacketTime.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileVideoPacketTime.is_open(),
                        "Can't open file " << fileNameVideoPacketTime);
        outFileVideoPacketTime.setf(std::ios_base::fixed);
    }

    for (uint16_t i = 0; i < repetitions; i++)
    {
        uint32_t packetSize = trafficGenerator->GetNextPacketSize();
        Time packetTime = trafficGenerator->GetNextPacketTime();

        totalPacketSize += packetSize;
        totalPacketTime += packetTime;

        if (TGT_ENABLE_PRINTING)
        {
            outFileVideoPacketSize << packetSize << "\n";
            outFileVideoPacketTime << packetTime.GetSeconds() << "\n";
        }
    }

    if (TGT_ENABLE_PRINTING)
    {
        outFileVideoPacketSize.close();
        outFileVideoPacketTime.close();
    }

    uint64_t averagePacketSize = totalPacketSize / repetitions;
    Time averagePacketTime = totalPacketTime / repetitions;
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averagePacketSize,
        100,
        100 * 0.01,
        "The mean video packet size is not according to the NGMN white paper.");
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averagePacketTime,
        MilliSeconds(6),
        MilliSeconds(6) * 0.05,
        "The mean video packet size is not according to the NGMN white paper.");
    Simulator::Destroy();
}

TrafficGeneratorNgmnGamingTestCase::TrafficGeneratorNgmnGamingTestCase()
    : TestCase("Check the mean initial packet arrival time, the mean packet size and the mean "
               "packet arrival time for the NGMN GAMING DL and UL.")
{
}

TrafficGeneratorNgmnGamingTestCase::~TrafficGeneratorNgmnGamingTestCase()
{
}

void
TrafficGeneratorNgmnGamingTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    Ptr<TrafficGeneratorNgmnGaming> trafficGenerator = CreateObject<TrafficGeneratorNgmnGaming>();
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);
    double eulerConst = 0.577215665; // truncated to 9 decimals, we dont need more precision since
                                     // we will anyway use some tolerance
    uint16_t repetitions = 1000;
    uint64_t totalPacketSizeDl = 0;
    Time totalPacketTimeDl = Seconds(0);
    Time totalInitPacketTimeDl = Seconds(0);
    uint64_t totalPacketSizeUl = 0;
    Time totalInitPacketTimeUl = Seconds(0);
    Time totalPacketTimeUl = Seconds(0);

    std::ofstream outFileGamingPacketSizeDl;
    std::ofstream outFileGamingPacketTimeDl;
    std::ofstream outFileGamingInitPacketTimeDl;

    std::ofstream outFileGamingPacketSizeUl;
    std::ofstream outFileGamingPacketTimeUl;
    std::ofstream outFileGamingInitPacketTimeUl;

    if (TGT_ENABLE_PRINTING)
    {
        std::string fileNameGamingPacketSizeDl = "gaming-packet-size-dl.csv";
        outFileGamingPacketSizeDl.open(fileNameGamingPacketSizeDl.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingPacketSizeDl.is_open(),
                        "Can't open file " << fileNameGamingPacketSizeDl);
        outFileGamingPacketSizeDl.setf(std::ios_base::fixed);

        std::string fileNameGamingPacketTimeDl = "gaming-packet-time-dl.csv";
        outFileGamingPacketTimeDl.open(fileNameGamingPacketTimeDl.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingPacketTimeDl.is_open(),
                        "Can't open file " << fileNameGamingPacketTimeDl);
        outFileGamingPacketTimeDl.setf(std::ios_base::fixed);

        std::string fileNameGamingInitPacketTimeDl = "gaming-packet-init-time-dl.csv";
        outFileGamingInitPacketTimeDl.open(fileNameGamingInitPacketTimeDl.c_str(),
                                           std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingInitPacketTimeDl.is_open(),
                        "Can't open file " << fileNameGamingInitPacketTimeDl);
        outFileGamingInitPacketTimeDl.setf(std::ios_base::fixed);

        std::string fileNameGamingPacketSizeUl = "gaming-packet-size-ul.csv";
        outFileGamingPacketSizeUl.open(fileNameGamingPacketSizeUl.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingPacketSizeUl.is_open(),
                        "Can't open file " << fileNameGamingPacketSizeUl);
        outFileGamingPacketSizeUl.setf(std::ios_base::fixed);

        std::string fileNameGamingPacketTimeUl = "gaming-packet-time-ul.csv";
        outFileGamingPacketTimeUl.open(fileNameGamingPacketTimeUl.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingPacketTimeUl.is_open(),
                        "Can't open file " << fileNameGamingPacketTimeUl);
        outFileGamingPacketTimeUl.setf(std::ios_base::fixed);

        std::string fileNameGamingInitPacketTimeUl = "gaming-packet-init-time-ul.csv";
        outFileGamingInitPacketTimeUl.open(fileNameGamingInitPacketTimeUl.c_str(),
                                           std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileGamingInitPacketTimeUl.is_open(),
                        "Can't open file " << fileNameGamingInitPacketTimeUl);
        outFileGamingInitPacketTimeUl.setf(std::ios_base::fixed);
    }

    trafficGenerator->SetAttribute("IsDownlink", BooleanValue(true));
    for (uint16_t i = 0; i < repetitions; i++)
    {
        uint32_t packetSizeDl = trafficGenerator->GetNextPacketSize();
        Time initPacketTimeDl = trafficGenerator->GetInitialPacketArrivalTime();
        Time packetTimeDl = trafficGenerator->GetNextPacketTime();

        totalPacketSizeDl += packetSizeDl;
        totalInitPacketTimeDl += initPacketTimeDl;
        totalPacketTimeDl += packetTimeDl;

        if (TGT_ENABLE_PRINTING)
        {
            outFileGamingPacketSizeDl << packetSizeDl << "\n";
            outFileGamingInitPacketTimeDl << initPacketTimeDl.GetSeconds() << "\n";
            outFileGamingPacketTimeDl << packetTimeDl.GetSeconds() << "\n";
        }
    }
    uint64_t averagePacketSizeDl = totalPacketSizeDl / repetitions;
    Time averageInitPacketTimeDl = totalInitPacketTimeDl / repetitions;
    Time averagePacketArrivalTimeDl = totalPacketTimeDl / repetitions;

    trafficGenerator->SetAttribute("IsDownlink", BooleanValue(false));
    for (uint16_t i = 0; i < repetitions; i++)
    {
        uint32_t packetSizeUl = trafficGenerator->GetNextPacketSize();
        Time initPacketTimeUl = trafficGenerator->GetInitialPacketArrivalTime();
        Time packetTimeUl = trafficGenerator->GetNextPacketTime();

        totalPacketSizeUl += packetSizeUl;
        totalInitPacketTimeUl += initPacketTimeUl;
        totalPacketTimeUl += packetTimeUl;

        if (TGT_ENABLE_PRINTING)
        {
            outFileGamingPacketSizeUl << packetSizeUl << "\n";
            outFileGamingInitPacketTimeUl << initPacketTimeUl.GetSeconds() << "\n";
            outFileGamingPacketTimeUl << packetTimeUl.GetSeconds() << "\n";
        }
    }

    if (TGT_ENABLE_PRINTING)
    {
        outFileGamingPacketSizeDl.close();
        outFileGamingPacketTimeDl.close();
        outFileGamingInitPacketTimeDl.close();

        outFileGamingPacketSizeUl.close();
        outFileGamingPacketTimeUl.close();
        outFileGamingInitPacketTimeUl.close();
    }

    uint64_t averagePacketSizeUl = totalPacketSizeUl / repetitions;
    Time averageInitPacketTimeUl = totalInitPacketTimeUl / repetitions;
    Time averagePacketArrivalTimeUl = totalPacketTimeUl / repetitions;

    // check the mean DL packet size
    uint32_t aPacketSizeDl = 120; // in bytes
    uint32_t bPacketSizeDl = 36;
    uint32_t meanPacketSizeDl = floor(aPacketSizeDl + bPacketSizeDl * eulerConst);
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averagePacketSizeDl,
        meanPacketSizeDl,
        meanPacketSizeDl * 0.02,
        "The mean DL gaming packet size is not according to the NGMN white paper.");

    // test the mean UL packet size
    uint32_t aPacketSizeUl = 45;
    double bPacketSizeUl = 5.7;
    uint32_t meanPacketSizeUl = floor(aPacketSizeUl + bPacketSizeUl * eulerConst);
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averagePacketSizeUl,
        meanPacketSizeUl,
        meanPacketSizeUl * 0.03,
        "The mean UL gaming packet size is not according to the NGMN white paper.");

    // test the mean UL and DL initial packet arrival time
    uint32_t meanInitPacketTimeUl = 20;
    uint32_t meanInitPacketTimeDl = 20;
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averageInitPacketTimeUl,
        MilliSeconds(meanInitPacketTimeUl),
        MilliSeconds(meanInitPacketTimeUl) * 0.05,
        "The mean initial UL gaming packet time is not according to the NGMN white paper.");
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averageInitPacketTimeDl,
        MilliSeconds(meanInitPacketTimeDl),
        MilliSeconds(meanInitPacketTimeDl) * 0.05,
        "The mean initial DL gaming packet time is not according to the NGMN white paper.");

    // test the mean UL and DL packet arrival time
    uint32_t meanPacketArrivalTimeUl = 40; // ms
    uint32_t aPacketTimeDl = 55;           // ms
    uint32_t bPacketTimeDl = 6;
    uint32_t meanPacketArrivalTimeDl = floor(aPacketTimeDl + bPacketTimeDl * eulerConst);
    NS_TEST_ASSERT_MSG_EQ(
        averagePacketArrivalTimeUl,
        MilliSeconds(meanPacketArrivalTimeUl),
        "The mean arrival time of the UL gaming packets is not according to the NGMN white paper.");
    NS_TEST_ASSERT_MSG_EQ_TOL(
        averagePacketArrivalTimeDl,
        MilliSeconds(meanPacketArrivalTimeDl),
        MilliSeconds(meanPacketArrivalTimeDl) * 0.01,
        "The mean arrival time of the DL gaming packets is not according to the NGMN white paper.");

    Simulator::Destroy();
}

TrafficGeneratorNgmnVoipTestCase::TrafficGeneratorNgmnVoipTestCase(std::string transportProtocol)
    : TestCase("(NGMN VoIP throughput == 12.2 Kbps) when " + transportProtocol)
{
    m_transportProtocol = transportProtocol;
}

TrafficGeneratorNgmnVoipTestCase::~TrafficGeneratorNgmnVoipTestCase()
{
}

void
TrafficGeneratorNgmnVoipTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

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
    double durationInSeconds = 1000;

    // install the packet sink at the receiver node
    uint16_t port = 4000;
    InetSocketAddress rxAddress(Ipv4Address::GetAny(), port);

    PacketSinkHelper packetSinkHelper(m_transportProtocol, rxAddress);

    // install the application on the rx device
    ApplicationContainer sinkApplication = packetSinkHelper.Install(nodes.Get(1));
    sinkApplication.Start(Seconds(1));
    sinkApplication.Stop(Seconds(durationInSeconds));

    // install the traffic generator at the transmitter node
    TrafficGeneratorHelper trafficGeneratorHelper(
        m_transportProtocol,
        InetSocketAddress(ipv4Interfaces.GetAddress(1, 0), port),
        TrafficGeneratorNgmnVoip::GetTypeId());

    ApplicationContainer generatorApplication = trafficGeneratorHelper.Install(nodes.Get(0));
    generatorApplication.Start(Seconds(2));
    generatorApplication.Stop(Seconds(durationInSeconds));

    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    PingHelper pingHelper(ipv4Interfaces.GetAddress(1, 0));
    ApplicationContainer pingApps = pingHelper.Install(nodes.Get(0));
    pingApps.Start(Seconds(1));
    pingApps.Stop(Seconds(2));

    Ptr<TrafficGeneratorNgmnVoip> trafficGenerator =
        generatorApplication.Get(0)->GetObject<TrafficGeneratorNgmnVoip>();
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);

    Simulator::Run();

    uint64_t totalBytesSent = trafficGenerator->GetTotalBytes();

    Ptr<PacketSink> packetSink = sinkApplication.Get(0)->GetObject<PacketSink>();
    uint64_t totalBytesReceived = packetSink->GetTotalRx();

    NS_TEST_ASSERT_MSG_EQ_TOL(((double)totalBytesSent * 8) / durationInSeconds,
                              6.475e3,
                              6.475e3 * 0.15,
                              "TX: The NGMN VoIP traffic offered throughput is not as expected!");
    NS_TEST_ASSERT_MSG_EQ_TOL(((double)totalBytesReceived * 8) / durationInSeconds,
                              6.475e3,
                              6.475e3 * 0.15,
                              "RX: The NGMN VoIP traffic received throughput is not as expected!");

    Simulator::Destroy();
}

TrafficGeneratorThreeGppHttpTestCase::TrafficGeneratorThreeGppHttpTestCase()
    : TestCase(
          "(The mean object size == 10710Bytes) && (The mean embedded object size == 7758B)"
          "&& (The mean number of embedded objects == 5.64) && (The mean reading time == 30seconds)"
          "&& (The mean parsing time == 0.13seconds) for 3GPP HTTP")
{
}

TrafficGeneratorThreeGppHttpTestCase::~TrafficGeneratorThreeGppHttpTestCase()
{
}

void
TrafficGeneratorThreeGppHttpTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    Ptr<ThreeGppHttpVariables> trafficGenerator = CreateObject<ThreeGppHttpVariables>();
    // we need to call it manually because in this test since we do not run the simulation, nothing
    // will call DoInitialize
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(1);
    uint64_t totalNumEmbeddedObjects = 0;
    uint64_t totalObjectSize = 0;
    uint64_t totalEmbeddedObjectSize = 0;
    Time totalReadingTime = Seconds(0);
    Time totalParsingTime = Seconds(0);
    uint16_t repetitions = 10000;

    //////////////////////

    std::ofstream outFileHttpReadingTime;
    std::ofstream outFileHttpParsingTime;
    std::ofstream outFileHttpObjectSize;
    std::ofstream outFileHttpEmbeddedObjectSize;
    std::ofstream outFileHttpNumber;

    if (TGT_ENABLE_PRINTING)
    {
        std::string fileNameReadingTime = "http-reading-time.csv";
        outFileHttpReadingTime.open(fileNameReadingTime.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileHttpReadingTime.is_open(),
                        "Can't open file " << fileNameReadingTime);
        outFileHttpReadingTime.setf(std::ios_base::fixed);

        std::string fileNameParsingTime = "http-parsing-time.csv";
        outFileHttpParsingTime.open(fileNameParsingTime.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileHttpParsingTime.is_open(),
                        "Can't open file " << fileNameParsingTime);
        outFileHttpParsingTime.setf(std::ios_base::fixed);

        std::string fileNameObjectSize = "http-object-size.csv";
        outFileHttpObjectSize.open(fileNameObjectSize.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileHttpObjectSize.is_open(), "Can't open file " << fileNameObjectSize);
        outFileHttpObjectSize.setf(std::ios_base::fixed);

        std::string fileNameEmbObjectSize = "http-embedded-object-size.csv";
        outFileHttpEmbeddedObjectSize.open(fileNameEmbObjectSize.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileHttpEmbeddedObjectSize.is_open(),
                        "Can't open file " << fileNameEmbObjectSize);
        outFileHttpEmbeddedObjectSize.setf(std::ios_base::fixed);

        std::string fileNameNumber = "http-number-objects.csv";
        outFileHttpNumber.open(fileNameNumber.c_str(), std::ios_base::out);
        ;
        NS_ABORT_MSG_IF(!outFileHttpNumber.is_open(), "Can't open file " << fileNameNumber);
        outFileHttpNumber.setf(std::ios_base::fixed);
    }

    for (uint16_t i = 0; i < repetitions; i++)
    {
        uint32_t numEmbeddedObjects = trafficGenerator->GetNumOfEmbeddedObjects();
        totalNumEmbeddedObjects += numEmbeddedObjects;
        Time readingTime = trafficGenerator->GetReadingTime();
        totalReadingTime += readingTime;
        Time parsingTime = trafficGenerator->GetParsingTime();
        totalParsingTime += parsingTime;
        uint32_t objectSize = trafficGenerator->GetMainObjectSize();
        totalObjectSize += objectSize;
        uint32_t embeddedObjectSize = trafficGenerator->GetEmbeddedObjectSize();
        totalEmbeddedObjectSize += embeddedObjectSize;

        if (TGT_ENABLE_PRINTING)
        {
            outFileHttpObjectSize << objectSize << "\n";
            outFileHttpEmbeddedObjectSize << embeddedObjectSize << "\n";
            outFileHttpNumber << numEmbeddedObjects << "\n";
            outFileHttpReadingTime << readingTime.GetSeconds() << "\n";
            outFileHttpParsingTime << parsingTime.GetSeconds() << "\n";
        }
    }

    if (TGT_ENABLE_PRINTING)
    {
        outFileHttpObjectSize.close();
        outFileHttpEmbeddedObjectSize.close();
        outFileHttpNumber.close();
        outFileHttpReadingTime.close();
        outFileHttpParsingTime.close();
    }

    double avgNumEmbObjects = ((double)totalNumEmbeddedObjects) / repetitions;
    uint64_t avgObjectSize = totalObjectSize / repetitions;
    uint64_t avgEmbObjectSize = totalEmbeddedObjectSize / repetitions;
    Time averageReadingTime = totalReadingTime / repetitions;
    Time averageParsingTime = totalParsingTime / repetitions;

    NS_TEST_ASSERT_MSG_EQ_TOL(avgNumEmbObjects,
                              5.64,
                              5.64 * 0.1,
                              "The mean number of embedded objects per page is not according to "
                              "the 3GPP."); // 10% tolerance used because of the quantization used
                                            // for the number of embedded objects
    NS_TEST_ASSERT_MSG_EQ_TOL(avgObjectSize,
                              10710,
                              10710 * 0.03,
                              "The mean main object size is not according to the 3GPP.");
    NS_TEST_ASSERT_MSG_EQ_TOL(avgEmbObjectSize,
                              7758,
                              7758 * 0.03,
                              "The mean embedded object size is not according to the 3GPP.");
    NS_TEST_ASSERT_MSG_EQ_TOL(averageReadingTime,
                              Seconds(30),
                              Seconds(30 * 0.03),
                              "The mean reading time is not according to the 3GPP.");
    NS_TEST_ASSERT_MSG_EQ_TOL(averageParsingTime,
                              Seconds(0.13),
                              Seconds(0.13 * 0.03),
                              "The mean parsing time is not according to the 3GPP.");
    Simulator::Destroy();
}

TrafficGeneratorTestSuite::TrafficGeneratorTestSuite()
    : TestSuite("traffic-generator-test", Type::UNIT)
{
    std::list<TypeId> trafficGeneratorTypes = {TrafficGeneratorNgmnFtpMulti::GetTypeId(),
                                               TrafficGeneratorNgmnVideo::GetTypeId(),
                                               TrafficGeneratorNgmnGaming::GetTypeId(),
                                               TrafficGeneratorNgmnVoip::GetTypeId()};

    std::list<std::string> transportProtocols = {"ns3::UdpSocketFactory", "ns3::TcpSocketFactory"};

    for (const auto& trafficGeneratorType : trafficGeneratorTypes)
    {
        for (const auto& transportProtocol : transportProtocols)
        {
            std::string name = trafficGeneratorType.GetName() + " and " + transportProtocol;
            AddTestCase(new TrafficGeneratorTestCase(name, trafficGeneratorType, transportProtocol),
                        Duration::QUICK);
        }
    }

    AddTestCase(new TrafficGeneratorNgmnFtpTestCase(), Duration::QUICK);
    AddTestCase(new TrafficGeneratorNgmnVideoTestCase(), Duration::QUICK);
    AddTestCase(new TrafficGeneratorNgmnGamingTestCase(), Duration::QUICK);
    AddTestCase(new TrafficGeneratorNgmnVoipTestCase("ns3::UdpSocketFactory"), Duration::QUICK);
    AddTestCase(new TrafficGeneratorNgmnVoipTestCase("ns3::TcpSocketFactory"), Duration::QUICK);
    //  AddTestCase(new TrafficGeneratorThreeGppHttpTestCase(), Duration::QUICK);
}

static TrafficGeneratorTestSuite
    trafficGeneratorTestSuite; //!< Static variable for test initialization

}; // namespace ns3
