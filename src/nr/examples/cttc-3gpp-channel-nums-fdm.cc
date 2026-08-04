// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 *
 * \file cttc-3gpp-channel-nums-fdm.cc
 * \ingroup examples
 * \brief Frequency division multiplexing example, with TDD and FDD
 *
 * The example is showing how to configure multiple bandwidth parts, in which
 * some of them form a FDD configuration, while others uses TDD. The user
 * can configure the bandwidth and the frequency of these BWPs. Three types
 * of traffic are available: two are DL (video and voice) while one is
 * UL (gaming). Each traffic will be routed to different BWP. Voice will go
 * in the TDD BWP, while video will go in the FDD-DL one, and gaming in the
 * FDD-UL one.
 *
 * The configured spectrum division is the following:
\verbatim
    |------------BandTdd--------------|--------------BandFdd---------------|
    |------------CC0------------------|--------------CC1-------------------|
    |------------BWP0-----------------|------BWP1-------|-------BWP2-------|
\endverbatim
 * We will configure BWP0 as TDD, BWP1 as FDD-DL, BWP2 as FDD-UL.
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("3gppChannelNumsFdm");

int
main(int argc, char* argv[])
{
    uint16_t gNbNum = 4;
    uint16_t ueNum = 4;

    uint32_t udpPacketSizeVideo = 100;
    uint32_t udpPacketSizeVoice = 1252;
    uint32_t udpPacketSizeGaming = 500;
    uint32_t lambdaVideo = 50;
    uint32_t lambdaVoice = 100;
    uint32_t lambdaGaming = 250;

    uint32_t simTimeMs = 1400;
    uint32_t udpAppStartTimeMs = 400;

    double centralFrequencyBand1 = 28e9;
    double bandwidthBand1 = 100e6;
    double centralFrequencyBand2 = 28.2e9;
    double bandwidthBand2 = 100e6;
    double totalTxPower = 4;
    std::string simTag = "default";
    std::string outputDir = "./";
    bool enableVideo = true;
    bool enableVoice = true;
    bool enableGaming = true;

    CommandLine cmd(__FILE__);

    cmd.AddValue("packetSizeVideo",
                 "packet size in bytes to be used by video traffic",
                 udpPacketSizeVideo);
    cmd.AddValue("packetSizeVoice",
                 "packet size in bytes to be used by voice traffic",
                 udpPacketSizeVoice);
    cmd.AddValue("packetSizeGaming",
                 "packet size in bytes to be used by gaming traffic",
                 udpPacketSizeGaming);
    cmd.AddValue("lambdaVideo",
                 "Number of UDP packets in one second for video traffic",
                 lambdaVideo);
    cmd.AddValue("lambdaVoice",
                 "Number of UDP packets in one second for voice traffic",
                 lambdaVoice);
    cmd.AddValue("lambdaGaming",
                 "Number of UDP packets in one second for gaming traffic",
                 lambdaGaming);
    cmd.AddValue("enableVideo", "If true, enables video traffic transmission (DL)", enableVideo);
    cmd.AddValue("enableVoice", "If true, enables voice traffic transmission (DL)", enableVoice);
    cmd.AddValue("enableGaming", "If true, enables gaming traffic transmission (UL)", enableGaming);
    cmd.AddValue("simTimeMs", "Simulation time", simTimeMs);
    cmd.AddValue("centralFrequencyBand1",
                 "The system frequency to be used in band 1",
                 centralFrequencyBand1);
    cmd.AddValue("bandwidthBand1", "The system bandwidth to be used in band 1", bandwidthBand1);
    cmd.AddValue("centralFrequencyBand2",
                 "The system frequency to be used in band 2",
                 centralFrequencyBand2);
    cmd.AddValue("bandwidthBand2", "The system bandwidth to be used in band 2", bandwidthBand2);
    cmd.AddValue("totalTxPower",
                 "total tx power that will be proportionally assigned to"
                 " bands, CCs and bandwidth parts depending on each BWP bandwidth ",
                 totalTxPower);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);

    cmd.Parse(argc, argv);

    NS_ABORT_IF(centralFrequencyBand1 > 100e9);
    NS_ABORT_IF(centralFrequencyBand2 > 100e9);

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    int64_t randomStream = 1;

    GridScenarioHelper gridScenario;
    gridScenario.SetRows(gNbNum / 2);
    gridScenario.SetColumns(gNbNum);
    gridScenario.SetHorizontalBsDistance(5.0);
    gridScenario.SetBsHeight(10.0);
    gridScenario.SetUtHeight(1.5);
    // must be set before BS number
    gridScenario.SetSectorization(GridScenarioHelper::SINGLE);
    gridScenario.SetBsNumber(gNbNum);
    gridScenario.SetUtNumber(ueNum);
    gridScenario.SetScenarioHeight(3); // Create a 3x3 scenario where the UE will
    gridScenario.SetScenarioLength(3); // be distributed.
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.CreateScenario();

    /*
     * TODO: Add a print, or a plot, that shows the scenario.
     */
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    // Create the spectrum channel
    channelHelper->ConfigureFactories("UMi", "Default", "ThreeGpp");
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));

    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // in this example, both bands have a single CC

    CcBwpCreator::SimpleOperationBandConf bandConfTdd(centralFrequencyBand1,
                                                      bandwidthBand1,
                                                      numCcPerBand);

    CcBwpCreator::SimpleOperationBandConf bandConfFdd(centralFrequencyBand2,
                                                      bandwidthBand2,
                                                      numCcPerBand);

    bandConfFdd.m_numBwp = 2; // Here, bandFdd will have 2 BWPs

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo bandTdd = ccBwpCreator.CreateOperationBandContiguousCc(bandConfTdd);
    OperationBandInfo bandFdd = ccBwpCreator.CreateOperationBandContiguousCc(bandConfFdd);
    // Create the same spectrum channel for both bands with different frequencies
    channelHelper->AssignChannelsToBands({bandTdd, bandFdd});

    /*
     * The configured spectrum division is:
     * |------------BandTdd--------------|--------------BandFdd---------------|
     * |------------CC0------------------|--------------CC1-------------------|
     * |------------BWP0-----------------|------BWP1-------|-------BWP2-------|
     *
     * We will configure BWP0 as TDD, BWP1 as FDD-DL, BWP2 as FDD-UL.
     */
    allBwps = CcBwpCreator::GetAllBwps({bandTdd, bandFdd});
    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));

    // Core latency
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(4.0));

    uint32_t bwpIdForVoice = 0;
    uint32_t bwpIdForVideo = 1;
    uint32_t bwpIdForGaming = 2;

    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO", UintegerValue(bwpIdForVideo));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_GAMING", UintegerValue(bwpIdForGaming));

    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VIDEO", UintegerValue(bwpIdForVideo));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_GAMING", UintegerValue(bwpIdForGaming));

    NetDeviceContainer gnbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueNetDev =
        nrHelper->InstallUeDevice(gridScenario.GetUserTerminals(), allBwps);

    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    NS_ASSERT(gnbNetDev.GetN() == 4);

    // -------------- First GNB:

    // BWP0, the TDD one
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("Numerology", UintegerValue(0));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("Pattern", StringValue("F|F|F|F|F|F|F|F|F|F|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP1, FDD-DL
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)->SetAttribute("Numerology", UintegerValue(0));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)
        ->SetAttribute("Pattern", StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP2, FDD-UL
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 2)->SetAttribute("Numerology", UintegerValue(0));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 2)
        ->SetAttribute("Pattern", StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 2)->SetAttribute("TxPower", DoubleValue(0.0));

    // Link the two FDD BWP:
    nrHelper->GetBwpManagerGnb(gnbNetDev.Get(0))->SetOutputLink(2, 1);

    // -------------- Second GNB:

    // BWP0, the TDD one
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 0)->SetAttribute("Numerology", UintegerValue(1));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 0)
        ->SetAttribute("Pattern", StringValue("F|F|F|F|F|F|F|F|F|F|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 0)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP1, FDD-DL
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 1)->SetAttribute("Numerology", UintegerValue(1));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 1)
        ->SetAttribute("Pattern", StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 1)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP2, FDD-UL
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 2)->SetAttribute("Numerology", UintegerValue(1));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 2)
        ->SetAttribute("Pattern", StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(1), 2)->SetAttribute("TxPower", DoubleValue(0.0));

    // Link the two FDD BWP:
    nrHelper->GetBwpManagerGnb(gnbNetDev.Get(1))->SetOutputLink(2, 1);

    // -------------- Third GNB:

    // BWP0, the TDD one
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 0)->SetAttribute("Numerology", UintegerValue(2));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 0)
        ->SetAttribute("Pattern", StringValue("F|F|F|F|F|F|F|F|F|F|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 0)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP1, FDD-DL
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 1)->SetAttribute("Numerology", UintegerValue(2));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 1)
        ->SetAttribute("Pattern", StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 1)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP2, FDD-UL
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 2)->SetAttribute("Numerology", UintegerValue(2));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 2)
        ->SetAttribute("Pattern", StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(2), 2)->SetAttribute("TxPower", DoubleValue(0.0));

    // Link the two FDD BWP:
    nrHelper->GetBwpManagerGnb(gnbNetDev.Get(2))->SetOutputLink(2, 1);

    // -------------- Fourth GNB:

    // BWP0, the TDD one
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 0)->SetAttribute("Numerology", UintegerValue(3));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 0)
        ->SetAttribute("Pattern", StringValue("F|F|F|F|F|F|F|F|F|F|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 0)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP1, FDD-DL
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 1)->SetAttribute("Numerology", UintegerValue(3));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 1)
        ->SetAttribute("Pattern", StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 1)->SetAttribute("TxPower", DoubleValue(4.0));

    // BWP2, FDD-UL
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 2)->SetAttribute("Numerology", UintegerValue(3));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 2)
        ->SetAttribute("Pattern", StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
    nrHelper->GetGnbPhy(gnbNetDev.Get(3), 2)->SetAttribute("TxPower", DoubleValue(0.0));

    // Link the two FDD BWP:
    nrHelper->GetBwpManagerGnb(gnbNetDev.Get(3))->SetOutputLink(2, 1);

    // Set the UE routing:

    for (uint32_t i = 0; i < ueNetDev.GetN(); i++)
    {
        nrHelper->GetBwpManagerUe(ueNetDev.Get(i))->SetOutputLink(1, 2);
    }

    // From here, it is standard NS3. In the future, we will create helpers
    // for this part as well.

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(gridScenario.GetUserTerminals());

    Ipv4InterfaceContainer ueIpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // Fix the attachment of the UEs: UE_i attached to GNB_i
    for (uint32_t i = 0; i < ueNetDev.GetN(); ++i)
    {
        auto gnbDev = DynamicCast<NrGnbNetDevice>(gnbNetDev.Get(i));
        auto ueDev = DynamicCast<NrUeNetDevice>(ueNetDev.Get(i));
        NS_ASSERT(gnbDev != nullptr);
        NS_ASSERT(ueDev != nullptr);
        nrHelper->AttachToGnb(ueDev, gnbDev);
    }

    /*
     * Traffic part. Install two kind of traffic: low-latency and voice, each
     * identified by a particular source port.
     */
    uint16_t dlPortVideo = 1234;
    uint16_t dlPortVoice = 1235;
    uint16_t ulPortGaming = 1236;

    ApplicationContainer serverApps;

    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSinkVideo(dlPortVideo);
    UdpServerHelper dlPacketSinkVoice(dlPortVoice);
    UdpServerHelper ulPacketSinkVoice(ulPortGaming);

    // The server, that is the application which is listening, is installed in the UE
    // for the DL traffic, and in the remote host for the UL traffic
    serverApps.Add(dlPacketSinkVideo.Install(gridScenario.GetUserTerminals()));
    serverApps.Add(dlPacketSinkVoice.Install(gridScenario.GetUserTerminals()));
    serverApps.Add(ulPacketSinkVoice.Install(remoteHost));

    /*
     * Configure attributes for the different generators, using user-provided
     * parameters for generating a CBR traffic
     *
     * Low-Latency configuration and object creation:
     */
    UdpClientHelper dlClientVideo;
    dlClientVideo.SetAttribute("RemotePort", UintegerValue(dlPortVideo));
    dlClientVideo.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientVideo.SetAttribute("PacketSize", UintegerValue(udpPacketSizeVideo));
    dlClientVideo.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaVideo)));

    // The bearer that will carry low latency traffic
    NrEpsBearer videoBearer(NrEpsBearer::GBR_CONV_VIDEO);

    // The filter for the low-latency traffic
    Ptr<NrEpcTft> videoTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfVideo;
    dlpfVideo.localPortStart = dlPortVideo;
    dlpfVideo.localPortEnd = dlPortVideo;
    videoTft->Add(dlpfVideo);

    // Voice configuration and object creation:
    UdpClientHelper dlClientVoice;
    dlClientVoice.SetAttribute("RemotePort", UintegerValue(dlPortVoice));
    dlClientVoice.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientVoice.SetAttribute("PacketSize", UintegerValue(udpPacketSizeVoice));
    dlClientVoice.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaVoice)));

    // The bearer that will carry voice traffic
    NrEpsBearer voiceBearer(NrEpsBearer::GBR_CONV_VOICE);

    // The filter for the voice traffic
    Ptr<NrEpcTft> voiceTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfVoice;
    dlpfVoice.localPortStart = dlPortVoice;
    dlpfVoice.localPortEnd = dlPortVoice;
    voiceTft->Add(dlpfVoice);

    // Gaming configuration and object creation:
    UdpClientHelper ulClientGaming;
    ulClientGaming.SetAttribute("RemotePort", UintegerValue(ulPortGaming));
    ulClientGaming.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    ulClientGaming.SetAttribute("PacketSize", UintegerValue(udpPacketSizeGaming));
    ulClientGaming.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaGaming)));

    // The bearer that will carry gaming traffic
    NrEpsBearer gamingBearer(NrEpsBearer::GBR_GAMING);

    // The filter for the gaming traffic
    Ptr<NrEpcTft> gamingTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter ulpfGaming;
    ulpfGaming.remotePortStart = ulPortGaming;
    ulpfGaming.remotePortEnd = ulPortGaming;
    ulpfGaming.direction = NrEpcTft::UPLINK;
    gamingTft->Add(ulpfGaming);

    /*
     * Let's install the applications!
     */
    ApplicationContainer clientApps;

    for (uint32_t i = 0; i < gridScenario.GetUserTerminals().GetN(); ++i)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(i);
        Ptr<NetDevice> ueDevice = ueNetDev.Get(i);
        Address ueAddress = ueIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        if (enableVoice)
        {
            dlClientVoice.SetAttribute("RemoteAddress", AddressValue(ueAddress));
            clientApps.Add(dlClientVoice.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, voiceBearer, voiceTft);
        }

        if (enableVideo)
        {
            dlClientVideo.SetAttribute("RemoteAddress", AddressValue(ueAddress));
            clientApps.Add(dlClientVideo.Install(remoteHost));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, videoBearer, videoTft);
        }

        // For the uplink, the installation happens in the UE, and the remote address
        // is the one of the remote host

        if (enableGaming)
        {
            ulClientGaming.SetAttribute("RemoteAddress",
                                        AddressValue(internetIpIfaces.GetAddress(1)));
            clientApps.Add(ulClientGaming.Install(ue));

            nrHelper->ActivateDedicatedEpsBearer(ueDevice, gamingBearer, gamingTft);
        }
    }

    // start UDP server and client apps
    serverApps.Start(MilliSeconds(udpAppStartTimeMs));
    clientApps.Start(MilliSeconds(udpAppStartTimeMs));
    serverApps.Stop(MilliSeconds(simTimeMs));
    clientApps.Stop(MilliSeconds(simTimeMs));

    // enable the traces provided by the nr module
    // nrHelper->EnableTraces();

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(gridScenario.GetUserTerminals());

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(MilliSeconds(simTimeMs));
    Simulator::Run();

    /*
     * To check what was installed in the memory, i.e., BWPs of gNB Device, and its configuration.
     * Example is: Node 1 -> Device 0 -> BandwidthPartMap -> {0,1} BWPs -> NrGnbPhy -> Numerology,
    GtkConfigStore config;
    config.ConfigureAttributes ();
    */

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream outFile;
    std::string filename = outputDir + "/" + simTag;
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                << t.destinationAddress << ":" << t.destinationPort << ") proto "
                << protoStream.str() << "\n";
        outFile << "  Tx Packets: " << i->second.txPackets << "\n";
        outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
        outFile << "  TxOffered:  "
                << i->second.txBytes * 8.0 / ((simTimeMs - udpAppStartTimeMs) / 1000.0) / 1000.0 /
                       1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            // double rxDuration = i->second.timeLastRxPacket.GetSeconds () -
            // i->second.timeFirstTxPacket.GetSeconds ();
            double rxDuration = (simTimeMs - udpAppStartTimeMs) / 1000.0;

            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            outFile << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "  Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            // outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << "
            // Mbps \n";
            outFile << "  Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    double meanFlowThroughput = averageFlowThroughput / stats.size();
    double meanFlowDelay = averageFlowDelay / stats.size();
    double throughputTolerance = meanFlowThroughput * 0.001;

    outFile << "\n\n  Mean flow throughput: " << meanFlowThroughput << "\n";
    outFile << "  Mean flow delay: " << meanFlowDelay << "\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();

    // called from examples-to-run.py with all default parameters
    if (argc == 0 && (meanFlowThroughput < 0.709696 - throughputTolerance ||
                      meanFlowThroughput > 0.709696 + throughputTolerance))
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
