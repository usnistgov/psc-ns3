// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \ingroup examples
 * \file cttc-nr-multi-flow-qos-sched.cc
 * \brief This example allows testing the performance of the QoS scheduler
 *        (nr-mac-scheduler-ofdma/tdma-qos) in conjunction with the LC QoS
 *        scheduler versus other schedulers, such as the RR and PF in
 *        conjunction with the LC RR scheduler.
 *        The example has been designed to test the E2E delay and throughput
 *        in a single-cell scenario with 2 UEs, where 1 UE has a NON-GBR flow
 *        and the other UE has 2 flows. One NON-GBR flow, and 1 DC-GBR with
 *        its gbr requirements set (erabGuaranteedBitRate).
 *
 * \code{.unparsed}
$ ./ns3 run "cttc-nr-multi-flow-qos-sched --PrintHelp"
    \endcode
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/buildings-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CttcNrSimpleQosSched");

int
main(int argc, char* argv[])
{
    /*
     * Variables that represent the parameters we will accept as input by the
     * command line. Each of them is initialized with a default value, and
     * possibly overridden below when command-line arguments are parsed.
     */
    // Scenario parameters (that we will use inside this script):
    uint16_t gNbNum = 1;
    uint16_t ueNumPergNb = 2;
    bool logging = false;

    // Simulation parameters. Please don't use double to indicate seconds; use
    // ns-3 Time values which use integers to avoid portability issues.
    Time simTime = MilliSeconds(1000);
    Time udpAppStartTime = MilliSeconds(400);

    // NR parameters. We will take the input from the command line, and then we
    // will pass them inside the NR module.
    uint16_t numerology = 0;
    double centralFrequency = 4e9;
    double bandwidth = 10e6;
    double totalTxPower = 43;

    bool enableOfdma = false;
    std::string schedulerType = "PF";
    bool enableQoSLcScheduler = false;

    uint8_t priorityTrafficScenario = 0; // default is saturation

    uint16_t mcsTable = 2;

    bool enablePdcpDiscarding = false;
    uint32_t discardTimerMs = 0;

    bool enableNrHelperTraces = false;
    bool enableQosTrafficTraces = true;
    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    /*
     * From here, we instruct the ns3::CommandLine class of all the input parameters
     * that we may accept as input, as well as their description, and the storage
     * variable.
     */
    CommandLine cmd;

    cmd.AddValue("gNbNum", "The number of gNbs in multiple-ue topology", gNbNum);
    cmd.AddValue("ueNumPergNb", "The number of UE per gNb in multiple-ue topology", ueNumPergNb);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.AddValue("priorityTrafficScenario",
                 "The traffic scenario for the case of priority. Can be 0: saturation"
                 "or 1: medium-load",
                 priorityTrafficScenario);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("numerology", "The numerology to be used", numerology);
    cmd.AddValue("centralFrequency", "The system frequency to be used", centralFrequency);
    cmd.AddValue("bandwidth", "The system bandwidth to be used", bandwidth);
    cmd.AddValue("totalTxPower",
                 "total tx power that will be proportionally assigned to"
                 " bands, CCs and bandwidth parts depending on each BWP bandwidth ",
                 totalTxPower);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("enableOfdma",
                 "If set to true it enables Ofdma scheduler. Default value is false (Tdma)",
                 enableOfdma);
    cmd.AddValue("schedulerType",
                 "PF: Proportional Fair (default), RR: Round-Robin, Qos",
                 schedulerType);
    cmd.AddValue("enableQoSLcScheduler",
                 "If set to true, it enables the QoS LC scheduler. Default is RR (false)",
                 enableQoSLcScheduler);
    cmd.AddValue("enableNrHelperTraces",
                 "If true, it enables the generation of the NrHelper traces, otherwise"
                 "NrHelper traces will not be generated. Default value is false",
                 enableNrHelperTraces);
    cmd.AddValue("enableQosTrafficTraces",
                 "If true, it enables the generation of the the Delay and Throughput"
                 "traces, otherwise these traces will not be generated. Default value is true",
                 enableQosTrafficTraces);
    cmd.AddValue("enablePdcpDiscarding",
                 "Whether to enable PDCP TX discarding",
                 enablePdcpDiscarding);
    cmd.AddValue("discardTimerMs",
                 "Discard timer value in milliseconds to use for all the flows",
                 discardTimerMs);

    cmd.Parse(argc, argv);

    // enable logging or not
    if (logging)
    {
        LogLevel logLevel1 =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_INFO);
        LogComponentEnable("NrMacSchedulerNs3", logLevel1);
        LogComponentEnable("NrMacSchedulerTdma", logLevel1);
    }

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::NrRlcUm::EnablePdcpDiscarding", BooleanValue(enablePdcpDiscarding));
    Config::SetDefault("ns3::NrRlcUm::DiscardTimerMs", UintegerValue(discardTimerMs));

    /*
     * Create the scenario. In our examples, we heavily use helpers that setup
     * the gnbs and ue following a pre-defined pattern. Please have a look at the
     * GridScenarioHelper documentation to see how the nodes will be distributed.
     */
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
    gridScenario.SetUtNumber(ueNumPergNb * gNbNum);
    gridScenario.SetScenarioHeight(3); // Create a 3x3 scenario where the UE will
    gridScenario.SetScenarioLength(3); // be distributed.
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.CreateScenario();

    uint32_t udpPacketSize1;
    uint32_t udpPacketSize2;
    uint32_t lambda1 = 1000;
    uint32_t lambda2 = 1000;

    if (priorityTrafficScenario == 0) // saturation
    {
        udpPacketSize1 = 3000;
        udpPacketSize2 = 3000;
    }
    else if (priorityTrafficScenario == 1) // medium-load
    {
        udpPacketSize1 = 3000;
        udpPacketSize2 = 1252;
    }
    else
    {
        NS_ABORT_MSG("The priorityTrafficScenario chosen is not correct. "
                     "Please choose among 0: saturation and 1: medium-load");
    }

    /*
     * Create two different NodeContainer for the different traffic type.
     * In ueLowLat we will put the UEs that will receive low-latency traffic,
     * while in ueVoice we will put the UEs that will receive the voice traffic.
     */
    NodeContainer ue1flowContainer;
    NodeContainer ue2flowsContainer;

    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(j);

        j % 2 == 0 ? ue1flowContainer.Add(ue) : ue2flowsContainer.Add(ue);
    }

    if (priorityTrafficScenario == 1)
    {
        lambda1 = 1000 / ue1flowContainer.GetN();
        lambda2 = 1000 / ue2flowsContainer.GetN();
    }

    // setup the nr simulation
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    std::stringstream scheduler;
    std::string subType;

    subType = !enableOfdma ? "Tdma" : "Ofdma";
    scheduler << "ns3::NrMacScheduler" << subType << schedulerType;
    std::cout << "Scheduler: " << scheduler.str() << std::endl;
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName(scheduler.str()));

    if (enableQoSLcScheduler)
    {
        nrHelper->SetSchedulerAttribute("SchedLcAlgorithmType",
                                        TypeIdValue(NrMacSchedulerLcQos::GetTypeId()));
    }

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

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    /*
     * Setup the configuration of the spectrum. One operation band is deployed
     * with 1 component carrier (CC), automatically generated by the ccBwpManager
     */
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    OperationBandInfo band;
    const uint8_t numOfCcs = 1;

    /*
     * The configured spectrum division for TDD is:
     *
     * |----Band1----|
     * |-----CC1-----|
     * |-----BWP1----|
     */

    // Create the configuration for the CcBwpHelper. SimpleOperationBandConf creates
    // a single BWP per CC
    CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, numOfCcs);

    bandConf.m_numBwp = 1;
    // By using the configuration created, it is time to make the operation band
    band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    // Create the channel helper for the spectrum configuration
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    // Set the spectrum channel
    channelHelper->ConfigureFactories("UMi", "LOS", "ThreeGpp");
    // Set shadowing and update period
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    // Set and create the channel for the band with only the propagation model
    channelHelper->AssignChannelsToBands({band}, NrChannelHelper::INIT_PROPAGATION);
    allBwps = CcBwpCreator::GetAllBwps({band});

    double x = pow(10, totalTxPower / 10);

    Packet::EnableChecking();
    Packet::EnablePrinting();

    uint32_t bwpIdUe1 = 0;
    uint32_t bwpIdUe2Flow1 = 0;
    uint32_t bwpIdUe2Flow2 = 0;

    // gNb routing between Bearer and bandwidh part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdUe1));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdUe2Flow1));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("DGBR_INTER_SERV_87",
                                                 UintegerValue(bwpIdUe2Flow2));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdUe1));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdUe2Flow1));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("DGBR_INTER_SERV_87", UintegerValue(bwpIdUe2Flow2));

    /*
     * We have configured the attributes we needed. Now, install and get the pointers
     * to the NetDevices, which contains all the NR stack:
     */
    NetDeviceContainer gnbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ue1flowNetDev = nrHelper->InstallUeDevice(ue1flowContainer, allBwps);
    NetDeviceContainer ue2flowsNetDev = nrHelper->InstallUeDevice(ue2flowsContainer, allBwps);

    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ue1flowNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ue2flowsNetDev, randomStream);

    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("Numerology", UintegerValue(numerology));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("TxPower", DoubleValue(10 * log10(x)));

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

    Ipv4InterfaceContainer ue1FlowIpIface;
    Ipv4InterfaceContainer ue2FlowsIpIface;
    ue1FlowIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ue1flowNetDev));
    ue2FlowsIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ue2flowsNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest gNB
    nrHelper->AttachToClosestGnb(ue1flowNetDev, gnbNetDev);
    nrHelper->AttachToClosestGnb(ue2flowsNetDev, gnbNetDev);

    /*
     * Traffic part. Install two kind of traffic: low-latency and voice, each
     * identified by a particular source port.
     */
    uint16_t dlPortUe1flow = 1234;
    uint16_t dlPortUe2flowsNgbr = 1235;
    uint16_t dlPortUe2flowsDcGbr = 1236;

    ApplicationContainer serverApps;

    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSinkUe1flow(dlPortUe1flow);
    UdpServerHelper dlPacketSinkUe2flowsNgbr(dlPortUe2flowsNgbr);
    UdpServerHelper dlPacketSinkUe2flowsDcGgbr(dlPortUe2flowsDcGbr);

    // The server, that is the application which is listening, is installed in the UE
    serverApps.Add(dlPacketSinkUe1flow.Install(ue1flowContainer));
    serverApps.Add(dlPacketSinkUe2flowsNgbr.Install(ue2flowsContainer));
    serverApps.Add(dlPacketSinkUe2flowsDcGgbr.Install(ue2flowsContainer));

    /*
     * Configure attributes for the different generators, using user-provided
     * parameters for generating a CBR traffic
     *
     * UE with 1 flow configuration and object creation:
     */
    /******************************************************************************/
    UdpClientHelper dlClientUe1flow;
    dlClientUe1flow.SetAttribute("RemotePort", UintegerValue(dlPortUe1flow));
    dlClientUe1flow.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientUe1flow.SetAttribute("PacketSize", UintegerValue(udpPacketSize1));
    dlClientUe1flow.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambda1)));

    // The bearer that will carry UE with 1 flow Non GBR traffic
    NrEpsBearer ue1flowBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);

    // The filter for the UE with 1 flow Non GBR traffic
    Ptr<NrEpcTft> ue1flowTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfUe1flow;
    dlpfUe1flow.localPortStart = dlPortUe1flow;
    dlpfUe1flow.localPortEnd = dlPortUe1flow;
    ue1flowTft->Add(dlpfUe1flow);
    /******************************************************************************/

    /******************************************************************************/
    // UE with 2 Flows Non GBR configuration and object creation:
    UdpClientHelper dlClientUe2flowsNgbr;
    dlClientUe2flowsNgbr.SetAttribute("RemotePort", UintegerValue(dlPortUe2flowsNgbr));
    dlClientUe2flowsNgbr.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientUe2flowsNgbr.SetAttribute("PacketSize", UintegerValue(udpPacketSize1));
    dlClientUe2flowsNgbr.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambda1)));

    // NrGbrQosInformation qosInfoInterServ2;
    // qosInfoInterServ2.gbrDl = 6e6; // Downlink GBR

    // The bearer that will carry UE with 2 Flows Non GBR traffic
    NrEpsBearer ue2flowsNgbrBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB); // qosInfoInterServ2);

    // The filter for the UE with 2 Flows Non GBR traffic
    Ptr<NrEpcTft> ue2flowsNgbrTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfUe2flowsNgbr;
    dlpfUe2flowsNgbr.localPortStart = dlPortUe2flowsNgbr;
    dlpfUe2flowsNgbr.localPortEnd = dlPortUe2flowsNgbr;
    ue2flowsNgbrTft->Add(dlpfUe2flowsNgbr);
    /******************************************************************************/

    /******************************************************************************/
    UdpClientHelper dlClientUe2flowsDcGbr;
    dlClientUe2flowsDcGbr.SetAttribute("RemotePort", UintegerValue(dlPortUe2flowsDcGbr));
    dlClientUe2flowsDcGbr.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientUe2flowsDcGbr.SetAttribute("PacketSize", UintegerValue(udpPacketSize2));
    dlClientUe2flowsDcGbr.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambda2)));

    NrGbrQosInformation qosUe2flowsDcGbr;
    qosUe2flowsDcGbr.gbrDl = 5e6; // Downlink GBR

    // The bearer that will carry Ue 2 Flows DC GBR traffic
    NrEpsBearer ue2flowsDcGbrBearer(NrEpsBearer::DGBR_INTER_SERV_87, qosUe2flowsDcGbr);

    // The filter for the 2 Flows DC GBR traffic
    Ptr<NrEpcTft> ue2FlowsDcGbrTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfUe2flowsDcGbr;
    dlpfUe2flowsDcGbr.localPortStart = dlPortUe2flowsDcGbr;
    dlpfUe2flowsDcGbr.localPortEnd = dlPortUe2flowsDcGbr;
    ue2FlowsDcGbrTft->Add(dlpfUe2flowsDcGbr);
    /******************************************************************************/

    //  Install the applications
    ApplicationContainer clientApps;

    for (uint32_t i = 0; i < ue1flowContainer.GetN(); ++i)
    {
        Ptr<NetDevice> ueDevice = ue1flowNetDev.Get(i);
        Address ueAddress = ue1FlowIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClientUe1flow.SetAttribute("RemoteAddress", AddressValue(ueAddress));
        clientApps.Add(dlClientUe1flow.Install(remoteHost));

        // Activate a dedicated bearer for the traffic type
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, ue1flowBearer, ue1flowTft);
    }

    for (uint32_t i = 0; i < ue2flowsContainer.GetN(); ++i)
    {
        Ptr<NetDevice> ueDevice = ue2flowsNetDev.Get(i);
        Address ueAddress = ue2FlowsIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClientUe2flowsNgbr.SetAttribute("RemoteAddress", AddressValue(ueAddress));
        clientApps.Add(dlClientUe2flowsNgbr.Install(remoteHost));

        // Activate a dedicated bearer for the traffic type
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, ue2flowsNgbrBearer, ue2flowsNgbrTft);
    }

    for (uint32_t i = 0; i < ue2flowsContainer.GetN(); ++i)
    {
        Ptr<NetDevice> ueDevice = ue2flowsNetDev.Get(i);
        Address ueAddress = ue2FlowsIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClientUe2flowsDcGbr.SetAttribute("RemoteAddress", AddressValue(ueAddress));
        clientApps.Add(dlClientUe2flowsDcGbr.Install(remoteHost));

        // Activate a dedicated bearer for the traffic type
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, ue2flowsDcGbrBearer, ue2FlowsDcGbrTft);
    }

    // start UDP server and client apps
    serverApps.Start(udpAppStartTime);
    clientApps.Start(udpAppStartTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);

    // enable the traces provided by the nr module
    if (enableNrHelperTraces)
    {
        nrHelper->EnableTraces();
    }

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(gridScenario.GetUserTerminals());

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(simTime);
    Simulator::Run();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream delayFile;
    std::ofstream throughputFile;

    std::ostringstream delayFileName;
    std::ostringstream throughputFileName;

    std::string lcSced;
    lcSced = enableQoSLcScheduler ? "LcQos" : "LcRR";

    if (simTag.empty())
    {
        delayFileName << "Delay"
                      << "_" << schedulerType.c_str() << "_" << lcSced.c_str() << ".txt";

        throughputFileName << "Throughput"
                           << "_" << schedulerType.c_str() << "_" << lcSced.c_str() << ".txt";
    }
    else
    {
        delayFileName << outputDir << "Delay" << simTag << std::string(".txt").c_str();
        throughputFileName << outputDir << "Throughput" << simTag << std::string(".txt").c_str();
    }

    if (enableQosTrafficTraces)
    {
        delayFile.open(delayFileName.str());
        delayFile.setf(std::ios_base::fixed);

        if (!delayFile.is_open())
        {
            NS_ABORT_MSG("Can't open file " << delayFileName.str());
        }
        delayFile << "source_address"
                  << "\t"
                  << "source_port"
                  << "\t"
                  << "dest_address"
                  << "\t"
                  << "dest_port"
                  << "\t"
                  << "delay"
                  << "\n";

        throughputFile.open(throughputFileName.str());
        throughputFile.setf(std::ios_base::fixed);

        if (!throughputFile.is_open())
        {
            NS_ABORT_MSG("Can't open file " << throughputFileName.str());
        }

        throughputFile << "source_port"
                       << "\t"
                       << "dest_port"
                       << "\t"
                       << "Throughput"
                       << "\t"
                       << "Delay"
                       << "\n";
    }

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

    std::ofstream outFile;
    std::string filename = outputDir + "/" + simTag;
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    double flowDuration = (simTime - udpAppStartTime).GetSeconds();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);

        if (enableQosTrafficTraces)
        {
            for (uint32_t j = 0; j < i->second.delayHistogram.GetNBins(); j++)
            {
                Histogram h = i->second.delayHistogram;
                if (h.GetBinCount(j))
                {
                    for (uint32_t k = 0; k < h.GetBinCount(j); k++)
                    {
                        delayFile << t.sourceAddress << "\t" << t.sourcePort << "\t"
                                  << t.destinationAddress << "\t" << t.destinationPort << "\t"
                                  << h.GetBinStart(j) << "\n";
                    }
                }
            }
        }

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
        outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / flowDuration / 1000.0 / 1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            averageFlowThroughput += i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            double throughput = i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            double delay = 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            outFile << "  Throughput: " << i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "  Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            // outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << "
            // Mbps \n";
            outFile << "  Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";

            if (enableQosTrafficTraces)
            {
                throughputFile << t.sourcePort << "\t" << t.destinationPort << "\t" << throughput
                               << "\t" << delay << std::endl;
            }
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";

            if (enableQosTrafficTraces)
            {
                throughputFile << t.sourcePort << "\t" << t.destinationPort << "\t" << 0 << "\t"
                               << 0 << std::endl;
            }
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / stats.size() << "\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();
    return 0;
}
