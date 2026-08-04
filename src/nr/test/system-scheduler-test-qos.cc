// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "system-scheduler-test-qos.h"

#include <ns3/antenna-module.h>
#include <ns3/applications-module.h>
#include <ns3/config.h>
#include <ns3/internet-module.h>
#include <ns3/nr-module.h>
#include <ns3/packet.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

namespace ns3
{

SystemSchedulerTestQos::SystemSchedulerTestQos(uint32_t ueNumPergNb,
                                               uint32_t numerology,
                                               double bw1,
                                               bool isDownlnk,
                                               bool isUplink,
                                               double p1,
                                               double p2,
                                               uint32_t priorityTrafficScenario,
                                               const std::string& schedulerType)
    : TestCase("QoS Scheduler Test Case")
{
    m_ueNumPergNb = ueNumPergNb;
    m_numerology = numerology;
    m_bw1 = bw1;
    m_isDownlink = isDownlnk;
    m_isUplink = isUplink;
    m_p1 = p1;
    m_p2 = p2;
    m_priorityTrafficScenario = priorityTrafficScenario;
    m_schedulerType = schedulerType;
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
SystemSchedulerTestQos::~SystemSchedulerTestQos()
{
}

void
SystemSchedulerTestQos::DoRun()
{
    NS_ABORT_IF(!m_isUplink && !m_isDownlink);

    // set simulation time and mobility
    Time simTime = MilliSeconds(1500);
    Time udpAppStartTimeDl = MilliSeconds(500);
    Time udpAppStartTimeUl = MilliSeconds(500);
    Time udpAppStopTimeDl = MilliSeconds(1500); // Let's give 1s to end the tx
    Time udpAppStopTimeUl = MilliSeconds(1500); // Let's give 1 to end the tx
    uint16_t gNbNum = 1;

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::NrRlcUm::ReorderingTimer", TimeValue(Seconds(1)));
    Config::SetDefault("ns3::NrEpsBearer::Release", UintegerValue(15));

    // create base stations and mobile terminals
    int64_t randomStream = 1;

    GridScenarioHelper gridScenario;
    gridScenario.SetRows(1);
    gridScenario.SetColumns(gNbNum);
    gridScenario.SetHorizontalBsDistance(5.0);
    gridScenario.SetVerticalBsDistance(5.0);
    gridScenario.SetBsHeight(1.5);
    gridScenario.SetUtHeight(1.5);
    // must be set before BS number
    gridScenario.SetSectorization(GridScenarioHelper::SINGLE);
    gridScenario.SetBsNumber(gNbNum);
    gridScenario.SetUtNumber(m_ueNumPergNb * gNbNum);
    gridScenario.SetScenarioHeight(3); // Create a 3x3 scenario where the UE will
    gridScenario.SetScenarioLength(3); // be distributed.
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.CreateScenario();

    if (verbose)
    {
        std::cout << "Test case: Scheduler type: " << m_schedulerType
                  << " numerology: " << m_numerology << " BW: " << m_bw1 << " DL: " << m_isDownlink
                  << " UL: " << m_isUplink << " number of UEs: " << m_ueNumPergNb << std::endl;
    }

    uint32_t udpPacketSizeULL = 3000; // m_priorityTrafficScenario == 0 //saturation
    uint32_t udpPacketSizeBe = 3000;
    uint32_t lambdaULL = 1000;
    uint32_t lambdaBe = 1000;

    if (m_priorityTrafficScenario == 1) // medium-load
    {
        udpPacketSizeBe = 1252;
    }

    NodeContainer ueLowLatContainer;
    NodeContainer ueVoiceContainer;

    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(j);
        j % 2 == 0 ? ueLowLatContainer.Add(ue) : ueVoiceContainer.Add(ue);
    }

    if (m_priorityTrafficScenario == 1)
    {
        lambdaULL = 1000 / ueLowLatContainer.GetN();
        lambdaBe = 1000 / ueVoiceContainer.GetN();
    }

    // setup the nr simulation
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    channelHelper->ConfigureFactories("UMi", "LOS");
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Set the scheduler type
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName(m_schedulerType));

    uint16_t mcsTable = 2;
    // Error Model: gNB and UE with same spectrum error model.
    std::string errorModel = "ns3::NrEesmIrT" + std::to_string(mcsTable);
    nrHelper->SetDlErrorModel(errorModel);
    nrHelper->SetUlErrorModel(errorModel);

    // Both DL and UL AMC will have the same model behind.
    nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
    nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));

    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));

    // set the number of antenna elements of UE
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // set the number of antenna elements of gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<ThreeGppAntennaModel>()));

    // gNB transmit power
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(43.0));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(43.0));

    // gNB numerology
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(m_numerology));

    /*
     * The configured spectrum division for TDD is:
     *
     * |----Band1----|
     * |-----CC1-----|
     * |-----BWP1----|
     */
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    OperationBandInfo band;
    double centralFrequency = 4e9;
    double bandwidth = m_bw1;
    const uint8_t numCcPerBand = 1;
    CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, numCcPerBand);

    // By using the configuration created, it is time to make the operation bands
    band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    channelHelper->AssignChannelsToBands({band});
    allBwps = CcBwpCreator::GetAllBwps({band});

    uint32_t bwpIdForLowLat = 0;
    uint32_t bwpIdForVoice = 0;

    // gNb routing between Bearer and bandwidh part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB",
                                                 UintegerValue(bwpIdForLowLat));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForLowLat));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // install mmWave net devices
    NetDeviceContainer gNbNetDevs =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueLowLatNetDev = nrHelper->InstallUeDevice(ueLowLatContainer, allBwps);
    NetDeviceContainer ueVoiceNetDev = nrHelper->InstallUeDevice(ueVoiceContainer, allBwps);

    randomStream = 1;
    randomStream += nrHelper->AssignStreams(gNbNetDevs, randomStream);
    randomStream += nrHelper->AssignStreams(ueLowLatNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueVoiceNetDev, randomStream);

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    Ptr<Node> remoteHostLowLat;
    Ptr<Node> remoteHostVoice;
    Ptr<Node> remoteHost;

    if (m_isDownlink)
    {
        remoteHostContainer.Create(1);
        remoteHost = remoteHostContainer.Get(0);
    }
    else
    {
        remoteHostContainer.Create(2);
        remoteHostLowLat = remoteHostContainer.Get(0);
        remoteHostVoice = remoteHostContainer.Get(1);
    }

    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));

    NetDeviceContainer internetDevicesLowLat;
    NetDeviceContainer internetDevicesVoice;
    NetDeviceContainer internetDevices;

    Ipv4AddressHelper ipv4h;
    Ipv4InterfaceContainer internetIpIfacesLowLat;
    Ipv4InterfaceContainer internetIpIfacesVoice;
    Ipv4InterfaceContainer internetIpIfaces;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;

    Ptr<Ipv4StaticRouting> remoteHostStaticRoutingLowLat;
    Ptr<Ipv4StaticRouting> remoteHostStaticRoutingVoice;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting;

    if (m_isDownlink)
    {
        internetDevices = p2ph.Install(pgw, remoteHost);

        ipv4h.SetBase("1.0.0.0", "255.0.0.0");
        internetIpIfaces = ipv4h.Assign(internetDevices);

        remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
        remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),
                                                   Ipv4Mask("255.0.0.0"),
                                                   1);
    }
    else
    {
        internetDevicesLowLat = p2ph.Install(pgw, remoteHostLowLat);
        internetDevicesVoice = p2ph.Install(pgw, remoteHostVoice);

        ipv4h.SetBase("1.0.0.0", "255.0.0.0");
        internetIpIfacesLowLat = ipv4h.Assign(internetDevicesLowLat);
        ipv4h.SetBase("2.0.0.0", "255.0.0.0");
        internetIpIfacesVoice = ipv4h.Assign(internetDevicesVoice);

        remoteHostStaticRoutingLowLat =
            ipv4RoutingHelper.GetStaticRouting(remoteHostLowLat->GetObject<Ipv4>());
        remoteHostStaticRoutingLowLat->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),
                                                         Ipv4Mask("255.0.0.0"),
                                                         1);
        remoteHostStaticRoutingVoice =
            ipv4RoutingHelper.GetStaticRouting(remoteHostVoice->GetObject<Ipv4>());
        remoteHostStaticRoutingVoice->AddNetworkRouteTo(Ipv4Address("8.0.0.0"),
                                                        Ipv4Mask("255.0.0.0"),
                                                        1);
    }

    internet.Install(gridScenario.GetUserTerminals());

    Ipv4InterfaceContainer ueLowLatIpIface;
    Ipv4InterfaceContainer ueVoiceIpIface;
    ueLowLatIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLowLatNetDev));
    ueVoiceIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVoiceNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest gNB
    nrHelper->AttachToClosestGnb(ueLowLatNetDev, gNbNetDevs);
    nrHelper->AttachToClosestGnb(ueVoiceNetDev, gNbNetDevs);

    /*
     * Traffic part. Install two kind of traffic: low-latency and voice, each
     * identified by a particular source port.
     */
    uint16_t dlPortLowLat = 1234;
    uint16_t dlPortVoice = 1235;

    uint16_t ulPortLowLat = 2000;
    uint16_t ulPortVoice = 2001;

    ApplicationContainer clientAppsDl;
    ApplicationContainer serverAppsDlLowLat;
    ApplicationContainer serverAppsDlVoice;

    ApplicationContainer clientAppsUl;
    ApplicationContainer serverAppsUlLowLat;
    ApplicationContainer serverAppsUlVoice;

    if (m_isUplink)
    {
        UdpServerHelper ulPacketSinkLowLat(ulPortLowLat);
        UdpServerHelper ulPacketSinkVoice(ulPortVoice);

        serverAppsUlLowLat = (ulPacketSinkLowLat.Install(remoteHostLowLat));
        serverAppsUlVoice = (ulPacketSinkVoice.Install(remoteHostVoice));

        UdpClientHelper ulClientLowlat;
        ulClientLowlat.SetAttribute("RemotePort", UintegerValue(ulPortLowLat));
        ulClientLowlat.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        ulClientLowlat.SetAttribute("PacketSize", UintegerValue(udpPacketSizeULL));
        ulClientLowlat.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaULL)));

        Ptr<NrEpcTft> ulLowLatTft = Create<NrEpcTft>();
        NrEpcTft::PacketFilter ulpfLowLat;
        ulpfLowLat.remotePortStart = ulPortLowLat;
        ulpfLowLat.remotePortEnd = ulPortLowLat;
        ulpfLowLat.direction = NrEpcTft::UPLINK;
        ulLowLatTft->Add(ulpfLowLat);

        NrEpsBearer bearerLowLat(NrEpsBearer::NGBR_LOW_LAT_EMBB);

        UdpClientHelper ulClientVoice;
        ulClientVoice.SetAttribute("RemotePort", UintegerValue(ulPortVoice));
        ulClientVoice.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        ulClientVoice.SetAttribute("PacketSize", UintegerValue(udpPacketSizeBe));
        ulClientVoice.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaBe)));

        Ptr<NrEpcTft> ulVoiceTft = Create<NrEpcTft>();
        NrEpcTft::PacketFilter ulpfVoice;
        ulpfVoice.remotePortStart = ulPortVoice;
        ulpfVoice.remotePortEnd = ulPortVoice;
        ulpfVoice.direction = NrEpcTft::UPLINK;
        ulVoiceTft->Add(ulpfVoice);

        NrEpsBearer bearerVoice(NrEpsBearer::GBR_CONV_VOICE);

        // configure here UDP traffic flows
        for (uint32_t j = 0; j < ueLowLatContainer.GetN(); ++j)
        {
            ulClientLowlat.SetAttribute("RemoteAddress",
                                        AddressValue(internetIpIfacesLowLat.GetAddress(1)));
            clientAppsUl.Add(ulClientLowlat.Install(ueLowLatContainer.Get(j)));
            nrHelper->ActivateDedicatedEpsBearer(ueLowLatNetDev.Get(j), bearerLowLat, ulLowLatTft);
        }

        // configure here UDP traffic flows
        for (uint32_t j = 0; j < ueVoiceContainer.GetN(); ++j)
        {
            ulClientVoice.SetAttribute("RemoteAddress",
                                       AddressValue(internetIpIfacesVoice.GetAddress(1)));
            clientAppsUl.Add(ulClientVoice.Install(ueVoiceContainer.Get(j)));
            nrHelper->ActivateDedicatedEpsBearer(ueVoiceNetDev.Get(j), bearerVoice, ulVoiceTft);
        }

        serverAppsUlLowLat.Start(udpAppStartTimeUl);
        serverAppsUlVoice.Start(udpAppStartTimeUl);
        clientAppsUl.Start(udpAppStartTimeUl);

        serverAppsUlLowLat.Stop(udpAppStopTimeUl);
        serverAppsUlVoice.Stop(udpAppStopTimeUl);
        clientAppsUl.Stop(udpAppStopTimeUl);
    }

    if (m_isDownlink)
    {
        UdpServerHelper dlPacketSinkLowLat(dlPortLowLat);
        UdpServerHelper dlPacketSinkVoice(dlPortVoice);

        serverAppsDlLowLat = (dlPacketSinkLowLat.Install(ueLowLatContainer));
        serverAppsDlVoice = (dlPacketSinkVoice.Install(ueVoiceContainer));

        Ptr<NrEpcTft> dlLowLatTft = Create<NrEpcTft>();
        NrEpcTft::PacketFilter dlpfLowLat;
        dlpfLowLat.localPortStart = dlPortLowLat;
        dlpfLowLat.localPortEnd = dlPortLowLat;
        dlpfLowLat.direction = NrEpcTft::DOWNLINK;
        dlLowLatTft->Add(dlpfLowLat);

        NrEpsBearer bearerLowlat(NrEpsBearer::NGBR_LOW_LAT_EMBB);

        Ptr<NrEpcTft> dlVoiceTft = Create<NrEpcTft>();
        NrEpcTft::PacketFilter dlpfVoice;
        dlpfVoice.localPortStart = dlPortVoice;
        dlpfVoice.localPortEnd = dlPortVoice;
        dlpfVoice.direction = NrEpcTft::DOWNLINK;
        dlVoiceTft->Add(dlpfVoice);

        NrEpsBearer bearerVoice(NrEpsBearer::GBR_CONV_VOICE);

        for (uint32_t j = 0; j < ueLowLatContainer.GetN(); ++j)
        {
            UdpClientHelper dlClient(ueLowLatIpIface.GetAddress(j), dlPortLowLat);
            dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
            dlClient.SetAttribute("PacketSize", UintegerValue(udpPacketSizeULL));
            dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaULL)));
            clientAppsDl.Add(dlClient.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueLowLatNetDev.Get(j), bearerLowlat, dlLowLatTft);
        }

        for (uint32_t j = 0; j < ueVoiceContainer.GetN(); ++j)
        {
            UdpClientHelper dlClient(ueVoiceIpIface.GetAddress(j), dlPortVoice);
            dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
            dlClient.SetAttribute("PacketSize", UintegerValue(udpPacketSizeBe));
            dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaBe)));
            clientAppsDl.Add(dlClient.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueVoiceNetDev.Get(j), bearerVoice, dlVoiceTft);
        }

        // start UDP server and client apps
        serverAppsDlLowLat.Start(udpAppStartTimeDl);
        serverAppsDlVoice.Start(udpAppStartTimeDl);
        clientAppsDl.Start(udpAppStartTimeDl);

        serverAppsDlLowLat.Stop(udpAppStopTimeDl);
        serverAppsDlVoice.Stop(udpAppStopTimeDl);
        clientAppsDl.Stop(udpAppStopTimeDl);
    }

    // nrHelper->EnableTraces();
    Simulator::Stop(simTime);
    Simulator::Run();

    uint32_t appTime = (simTime.GetSeconds() - udpAppStartTimeDl.GetSeconds());

    // Test Case 1: Half UEs QCI 1 saturated
    // and Half UEs QCI 80
    // check if ratio of throughputs is equal to ratio of priorities
    double dlThroughputLowLat = 0;
    double dlThroughputVoice = 0;
    double ulThroughputLowLat = 0;
    double ulThroughputVoice = 0;

    if (m_isDownlink)
    {
        for (uint32_t i = 0; i < serverAppsDlLowLat.GetN(); i++)
        {
            Ptr<UdpServer> serverApp = serverAppsDlLowLat.Get(i)->GetObject<UdpServer>();
            dlThroughputLowLat += (serverApp->GetReceived() * udpPacketSizeULL * 8) / appTime;
        }
        for (uint32_t i = 0; i < serverAppsDlVoice.GetN(); i++)
        {
            Ptr<UdpServer> serverApp = serverAppsDlVoice.Get(i)->GetObject<UdpServer>();
            dlThroughputVoice += (serverApp->GetReceived() * udpPacketSizeBe * 8) / appTime;
        }

        // Flow 2 is saturated and it must be prioritized (QCI 1 vs 80)

        double qciRatio = (100 - m_p1) / (100 - m_p2);
        double throughputRatio = dlThroughputVoice / dlThroughputLowLat;

        if (verbose)
        {
            std::cout << "dlThroughputLowLat: " << dlThroughputVoice
                      << " dlThroughputVoice: " << dlThroughputLowLat << std::endl;
            std::cout << "ratio: " << qciRatio << " throughput ratio: " << throughputRatio
                      << std::endl;
        }

        NS_TEST_ASSERT_MSG_EQ_TOL(qciRatio,
                                  throughputRatio,
                                  (qciRatio * 0.1),
                                  "DL qci Ratio and throughput Ratio are not "
                                  "equal within tolerance");
    }
    if (m_isUplink)
    {
        for (uint32_t i = 0; i < serverAppsUlLowLat.GetN(); i++)
        {
            Ptr<UdpServer> serverApp = serverAppsUlLowLat.Get(i)->GetObject<UdpServer>();
            ulThroughputLowLat += (serverApp->GetReceived() * udpPacketSizeULL * 8) / appTime;
        }
        for (uint32_t i = 0; i < serverAppsUlVoice.GetN(); i++)
        {
            Ptr<UdpServer> serverApp = serverAppsUlVoice.Get(i)->GetObject<UdpServer>();
            ulThroughputVoice += (serverApp->GetReceived() * udpPacketSizeBe * 8) / appTime;
        }

        double qciRatio = (100 - m_p1) / (100 - 90); // Hardcoded P due to scheduler restrictions
        double throughputRatio = ulThroughputVoice / ulThroughputLowLat;

        if (verbose)
        {
            std::cout << "ulThroughputLowLat: " << ulThroughputVoice
                      << " ulThroughputVoice: " << ulThroughputLowLat << std::endl;
            std::cout << "ratio: " << qciRatio << " throughput ratio: " << throughputRatio
                      << std::endl;
        }

        NS_TEST_ASSERT_MSG_EQ_TOL(qciRatio,
                                  throughputRatio,
                                  (qciRatio * 0.1),
                                  "UL qci Ratio and throughput Ratio are not "
                                  "equal within tolerance");
    }

    Simulator::Destroy();
}

} // namespace ns3
