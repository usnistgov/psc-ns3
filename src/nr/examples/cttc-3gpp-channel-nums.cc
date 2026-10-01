// Copyright (c) 2017 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

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
/**
 * \file cttc-3gpp-channel-nums.cc
 * \ingroup examples
 * \brief Simple topology numerologies example.
 *
 * This example allows users to configure the numerology and test the end-to-end
 * performance for different numerologies. In the following figure we illustrate
 * the simulation setup.
 *
 * For example, UDP packet generation rate can be configured by setting
 * "--lambda=1000". The numerology can be toggled by the argument,
 * e.g. "--numerology=1". Additionally, in this example two arguments
 * are added "bandwidth" and "frequency", both in Hz units. The modulation
 * scheme of this example is in test mode, and it is fixed to 28.
 *
 * By default, the program uses the 3GPP channel model, without shadowing and with
 * line of sight ('l') option. The program runs for 0.4 seconds and one single
 * packet is to be transmitted. The packet size can be configured by using the
 * following parameter: "--packetSize=1000".
 *
 * This simulation prints the output to the terminal and also to the file which
 * is named by default "cttc-3gpp-channel-nums-fdm-output" and which is by
 * default placed in the root directory of the project.
 *
 * To run the simulation with the default configuration one shall run the
 * following in the command line:
 *
 * ./ns3 run cttc-3gpp-channel-nums
 *
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("3gppChannelNumerologiesExample");

int
main(int argc, char* argv[])
{
    // enable logging or not
    bool logging = false;
    if (logging)
    {
        LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
        LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
        LogComponentEnable("NrPdcp", LOG_LEVEL_INFO);
    }

    // set simulation time and mobility
    double simTime = 1;           // seconds
    double udpAppStartTime = 0.4; // seconds

    // other simulation parameters default values
    uint16_t numerology = 0;

    uint16_t gNbNum = 1;
    uint16_t ueNumPergNb = 1;

    double centralFrequency = 7e9;
    double bandwidth = 100e6;
    double txPower = 14;
    double lambda = 1000;
    uint32_t udpPacketSize = 1000;
    bool udpFullBuffer = true;
    uint8_t fixedMcs = 28;
    bool useFixedMcs = true;
    bool singleUeTopology = true;
    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    CommandLine cmd(__FILE__);
    cmd.AddValue("gNbNum", "The number of gNbs in multiple-ue topology", gNbNum);
    cmd.AddValue("ueNumPergNb", "The number of UE per gNb in multiple-ue topology", ueNumPergNb);
    cmd.AddValue("numerology", "The numerology to be used.", numerology);
    cmd.AddValue("txPower", "Tx power to be configured to gNB", txPower);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("frequency", "The system frequency", centralFrequency);
    cmd.AddValue("bandwidth", "The system bandwidth", bandwidth);
    cmd.AddValue("udpPacketSize", "UDP packet size in bytes", udpPacketSize);
    cmd.AddValue("lambda", "Number of UDP packets per second", lambda);
    cmd.AddValue("udpFullBuffer",
                 "Whether to set the full buffer traffic; if this parameter is set then the "
                 "udpInterval parameter"
                 "will be neglected",
                 udpFullBuffer);
    cmd.AddValue(
        "fixedMcs",
        "The fixed MCS that will be used in this example if useFixedMcs is configured to true (1).",
        fixedMcs);
    cmd.AddValue("useFixedMcs",
                 "Whether to use fixed mcs, normally used for testing purposes",
                 useFixedMcs);
    cmd.AddValue("singleUeTopology",
                 "If true, the example uses a predefined topology with one UE and one gNB; "
                 "if false, the example creates a grid of gNBs with a number of UEs attached",
                 singleUeTopology);

    cmd.Parse(argc, argv);

    NS_ASSERT(ueNumPergNb > 0);

    // setup the nr simulation
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    // Setup the channel helper
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();

    /*
     * Spectrum division. We create one operation band with one component carrier
     * (CC) which occupies the whole operation band bandwidth. The CC contains a
     * single Bandwidth Part (BWP). This BWP occupies the whole CC band.
     * Both operational bands will use the StreetCanyon channel modeling.
     */
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // in this example, both bands have a single CC
    std::string scenario = "RMa";
    std::string condition = "LOS";
    if (ueNumPergNb > 1)
    {
        scenario = "InH-OfficeOpen";
    }

    // Create the spectrum channel using the desired scenario and condition
    channelHelper->ConfigureFactories(scenario, condition, "ThreeGpp");
    // Set configurations for the channel model
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    // Create the configuration for the CcBwpHelper. SimpleOperationBandConf creates
    // a single BWP per CC
    CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, numCcPerBand);

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    // Set and create the channel model to the band
    channelHelper->AssignChannelsToBands({band});
    BandwidthPartInfoPtrVector allBwps = CcBwpCreator::GetAllBwps({band});

    /*
     * Continue setting the parameters which are common to all the nodes, like the
     * gNB transmit power or numerology.
     */
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(numerology));

    // Scheduler
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaRR"));
    nrHelper->SetSchedulerAttribute("FixedMcsDl", BooleanValue(useFixedMcs));
    nrHelper->SetSchedulerAttribute("FixedMcsUl", BooleanValue(useFixedMcs));

    if (useFixedMcs)
    {
        nrHelper->SetSchedulerAttribute("StartingMcsDl", UintegerValue(fixedMcs));
        nrHelper->SetSchedulerAttribute("StartingMcsUl", UintegerValue(fixedMcs));
    }

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<ThreeGppAntennaModel>()));

    // Beamforming method
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);

    // Error Model: UE and GNB with same spectrum error model.
    nrHelper->SetUlErrorModel("ns3::NrEesmIrT1");
    nrHelper->SetDlErrorModel("ns3::NrEesmIrT1");

    // Both DL and UL AMC will have the same model behind.
    nrHelper->SetGnbDlAmcAttribute(
        "AmcModel",
        EnumValue(NrAmc::ErrorModel)); // NrAmc::ShannonModel or NrAmc::ErrorModel
    nrHelper->SetGnbUlAmcAttribute(
        "AmcModel",
        EnumValue(NrAmc::ErrorModel)); // NrAmc::ShannonModel or NrAmc::ErrorModel

    // Create EPC helper
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    nrHelper->SetEpcHelper(nrEpcHelper);
    // Core latency
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // gNb routing between Bearer and bandwidh part
    uint32_t bwpIdForBearer = 0;
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForBearer));

    /*
     *  Create the gNB and UE nodes according to the network topology
     */
    NodeContainer gNbNodes;
    NodeContainer ueNodes;
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> bsPositionAlloc = CreateObject<ListPositionAllocator>();
    Ptr<ListPositionAllocator> utPositionAlloc = CreateObject<ListPositionAllocator>();

    const double gNbHeight = 10;
    const double ueHeight = 1.5;

    if (singleUeTopology)
    {
        gNbNodes.Create(1);
        ueNodes.Create(1);
        gNbNum = 1;
        ueNumPergNb = 1;

        mobility.Install(gNbNodes);
        mobility.Install(ueNodes);
        bsPositionAlloc->Add(Vector(0.0, 0.0, gNbHeight));
        utPositionAlloc->Add(Vector(0.0, 30.0, ueHeight));
    }
    else
    {
        gNbNodes.Create(gNbNum);
        ueNodes.Create(ueNumPergNb * gNbNum);

        int32_t yValue = 0.0;
        for (uint32_t i = 1; i <= gNbNodes.GetN(); ++i)
        {
            // 2.0, -2.0, 6.0, -6.0, 10.0, -10.0, ....
            if (i % 2 != 0)
            {
                yValue = static_cast<int>(i) * 30;
            }
            else
            {
                yValue = -yValue;
            }

            bsPositionAlloc->Add(Vector(0.0, yValue, gNbHeight));

            // 1.0, -1.0, 3.0, -3.0, 5.0, -5.0, ...
            double xValue = 0.0;
            for (uint16_t j = 1; j <= ueNumPergNb; ++j)
            {
                if (j % 2 != 0)
                {
                    xValue = j;
                }
                else
                {
                    xValue = -xValue;
                }

                if (yValue > 0)
                {
                    utPositionAlloc->Add(Vector(xValue, 1, ueHeight));
                }
                else
                {
                    utPositionAlloc->Add(Vector(xValue, -1, ueHeight));
                }
            }
        }
    }
    mobility.SetPositionAllocator(bsPositionAlloc);
    mobility.Install(gNbNodes);

    mobility.SetPositionAllocator(utPositionAlloc);
    mobility.Install(ueNodes);

    // Install nr net devices
    NetDeviceContainer gNbNetDev = nrHelper->InstallGnbDevice(gNbNodes, allBwps);

    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueNodes, allBwps);

    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(gNbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

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
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(ueNodes);

    Ipv4InterfaceContainer ueIpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < ueNodes.GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest gNB
    nrHelper->AttachToClosestGnb(ueNetDev, gNbNetDev);

    // assign IP address to UEs, and install UDP downlink applications
    uint16_t dlPort = 1234;

    ApplicationContainer serverApps;

    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSinkHelper(dlPort);
    serverApps.Add(dlPacketSinkHelper.Install(ueNodes.Get(0)));

    UdpClientHelper dlClient;
    dlClient.SetAttribute("RemotePort", UintegerValue(dlPort));
    dlClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    if (udpFullBuffer)
    {
        double bitRate = 75000000; // 75 Mbps will saturate the NR system of 20 MHz with the
                                   // NrEesmIrT1 error model
        bitRate /= ueNumPergNb;    // Divide the cell capacity among UEs
        if (bandwidth > 20e6)
        {
            bitRate *= bandwidth / 20e6;
        }
        lambda = bitRate / static_cast<double>(udpPacketSize * 8);
    }
    dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambda)));

    // The bearer that will carry low latency traffic
    NrEpsBearer bearer(NrEpsBearer::GBR_CONV_VOICE);

    Ptr<NrEpcTft> tft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpf;
    dlpf.localPortStart = dlPort;
    dlpf.localPortEnd = dlPort;
    tft->Add(dlpf);

    /*
     * Let's install the applications!
     */
    ApplicationContainer clientApps;

    for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
    {
        Ptr<Node> ue = ueNodes.Get(i);
        Ptr<NetDevice> ueDevice = ueNetDev.Get(i);
        Address ueAddress = ueIpIface.GetAddress(i);

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClient.SetAttribute("RemoteAddress", AddressValue(ueAddress));
        clientApps.Add(dlClient.Install(remoteHost));

        // Activate a dedicated bearer for the traffic type
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, bearer, tft);
    }

    // start server and client apps
    serverApps.Start(Seconds(udpAppStartTime));
    clientApps.Start(Seconds(udpAppStartTime));
    serverApps.Stop(Seconds(simTime));
    clientApps.Stop(Seconds(simTime));

    // enable the traces provided by the nr module
    // nrHelper->EnableTraces();

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(ueNodes);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

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
        NS_LOG_ERROR("Can't open file " << filename);
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
                << i->second.txBytes * 8.0 / (simTime - udpAppStartTime) / 1000 / 1000 << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            double rxDuration =
                i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();

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
            outFile << "  Mean upt:  0  Mbps \n";
            outFile << "  Mean jitter: 0 ms\n";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    double meanFlowThroughput = averageFlowThroughput / stats.size();
    double meanFlowDelay = averageFlowDelay / stats.size();
    Ptr<UdpServer> serverApp = serverApps.Get(0)->GetObject<UdpServer>();
    double totalUdpThroughput =
        ((serverApp->GetReceived() * udpPacketSize * 8) / (simTime - udpAppStartTime)) * 1e-6;

    outFile << "\n\n  Mean flow throughput: " << meanFlowThroughput << "\n";
    outFile << "  Mean flow delay: " << meanFlowDelay << "\n";
    outFile << "\n UDP throughput (bps) for UE with node ID 0:" << totalUdpThroughput << std::endl;

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();

    double toleranceMeanFlowThroughput = 383.557857 * 0.0001;
    double toleranceMeanFlowDelay = 3.533664 * 0.0001;
    double toleranceUdpThroughput = 372.5066667 * 0.0001;

    // called from examples-to-run.py with all default parameters
    if (argc == 0 && (meanFlowThroughput < 383.557857 - toleranceMeanFlowThroughput ||
                      meanFlowThroughput > 383.557857 + toleranceMeanFlowThroughput ||
                      meanFlowDelay < 3.533664 - toleranceMeanFlowDelay ||
                      meanFlowDelay > 3.533664 + toleranceMeanFlowDelay ||
                      totalUdpThroughput < 372.5066667 - toleranceUdpThroughput ||
                      totalUdpThroughput > 372.5066667 + toleranceUdpThroughput))
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
