// Copyright (c) 2024 LASSE / Universidade Federal do Pará (UFPA)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/command-line.h>
#include <ns3/constant-velocity-mobility-model.h>
#include <ns3/flow-monitor-module.h>
#include <ns3/isotropic-antenna-model.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/nr-module.h>
#include <ns3/parabolic-antenna-model.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/pointer.h>
#include <ns3/udp-client-server-helper.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("GsocNrChannelModels");

/**
 * \ingroup examples
 * \file gsoc-nr-channel-models.cc
 * \brief A simple NR example demonstrating the simulation of different spectrum channels.
 * This example showcases configuration of the spectrum channel with antenna and propagation
 * models that explicitly model multiple antenna elements (phased arrays), and with antenna
 * and propagation models that abstract away the individual elements (called 'non-phased' herein).
 * The 3GPP propagation models require Uniform Planar Array and propagation models of the
 * PhasedArraySpectrumPropagationLossModel type. Other propagation models are designed
 * to use antenna models without explicit array configuration (e.g., ParabolicAntennaModel)
 * and with propagation models (e.g., TraceFadingLossModel) that do not take into account the
 * explicit array configuration. This comment is a reminder that propagation models are
 * also dependent on the antenna type.
 *
 * In this example, the user can simulate a phased-array channel or the Friis model
 * (non-phased array model). By default, the example uses the 3GPP channel model with the default
 * channel condition and Urban Macro scenario. When selecting to simulate using the Friis model, the
 * ParabolicAntennaModel will be used as the antenna type.
 *
 * The simulation generates multiple text files containing flow statistics and pathloss traces.
 * Each SpectrumChannel produces distinct pathloss traces, which may or may not affect the
 * statistical results.
 *
 * \note This example was produced during the Google Summer of Code 2024 program. The main author is
 * João Albuquerque, under the supervision of Biljana Bojovic, Amir Ashtari, Gabriel Ferreira, in
 * project: 5G NR Module Benchmark and Analysis for Distinct Channel Models
 *
 * <joao.barbosa.albuquerque@itec.ufpa.br>
 */

int
main(int argc, char* argv[])
{
    int64_t randomStream = 1;
    double centralFrequency = 30.5e9;      // 30.5 GHz
    double bandwidth = 100e6;              // 100 MHz
    Time simTime = Seconds(1.0);           // 1 second simulation time
    Time udpTime = MilliSeconds(0);        // 0 ms
    Time maxDelay = MilliSeconds(100);     // 100 ms
    std::string scenario = "UMa";          // Urban Macro
    std::string channelModel = "ThreeGpp"; // 3GPP channel model
    uint32_t numUes = 1;                   // Number of UEs
    uint32_t numGnbs = 1;                  // Number of gNBs
    bool logging = false;                  // Enable logging
    uint16_t numerology = 1;               // Numerology

    /**
     * Default channel condition model: This model varies based on the selected scenario.
     * For instance, in the Urban Macro scenario, the default channel condition model is
     * the ThreeGppUMaChannelConditionModel.
     */
    std::string channelConditionModel = "Default";
    // Output file with the statistics
    std::ofstream outputFile("channels-example-flows.txt");

    CommandLine cmd(__FILE__);
    // cmd.Usage(""); Leave it empty until we decide the final example
    cmd.AddValue("channelModel",
                 "The channel model for the simulation, which can be 'NYU', "
                 "'ThreeGpp', 'TwoRay', 'Friis'. ",
                 channelModel);
    cmd.AddValue("channelConditionModel",
                 "The channel condition model for the simulation. Choose among 'Default', 'LOS',"
                 "'NLOS', 'Buildings'.",
                 channelConditionModel);
    cmd.AddValue("ueNum", "Number of UEs in the simulation.", numUes);
    cmd.AddValue("gNbNum", "Number of gNBs in the simulation.", numGnbs);
    cmd.AddValue("frequency", "The central carrier frequency in Hz.", centralFrequency);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.Parse(argc, argv);

    if (logging)
    {
        LogComponentEnable("GsocNrChannelModels", LOG_LEVEL_INFO);
    }

    // Create the simulated scenario
    HexagonalGridScenarioHelper hexGrid;
    /**
     * Set the scenario parameters for the simulation, considering the UMa scenario.
     * Following the TR 38.901 specification - Table 7.4.1-1 pathloss models.
     * hBS = 25m for UMa scenario.
     * hUT = 1.5m for UMa scenario.
     */
    hexGrid.SetUtHeight(1.5);    // Height of the UE in meters
    hexGrid.SetBsHeight(25);     // Height of the gNB in meters
    hexGrid.SetSectorization(1); // Number of sectors
    hexGrid.m_isd = 200;         // Inter-site distance in meters
    uint32_t ueTxPower = 23;     // UE transmission power in dBm
    uint32_t bsTxPower = 41;     // gNB transmission power in dBm
    double ueSpeed = 0.8333;     // in m/s (3 km/h)
    // Antenna parameters
    uint32_t ueNumRows = 1;  // Number of rows for the UE antenna
    uint32_t ueNumCols = 1;  // Number of columns for the UE antenna
    uint32_t gnbNumRows = 4; // Number of rows for the gNB antenna
    uint32_t gnbNumCols = 8; // Number of columns for the gNB antenna
    // Set the number of UEs and gNBs nodes in the scenario
    hexGrid.SetUtNumber(numUes);  // Number of UEs
    hexGrid.SetBsNumber(numGnbs); // Number of gNBs
    // Create a scenario with mobility
    hexGrid.CreateScenarioWithMobility(Vector(ueSpeed, 0.0, 0.0),
                                       0); // move UE with 3 km/h in x-axis

    auto ueNodes = hexGrid.GetUserTerminals();
    auto gNbNodes = hexGrid.GetBaseStations();

    NS_LOG_INFO("Number of UEs: " << ueNodes.GetN() << ", Number of gNBs: " << gNbNodes.GetN());
    for (size_t ueIndex = 0; ueIndex < ueNodes.GetN(); ueIndex++)
    {
        NS_LOG_INFO("UE [" << ueNodes.Get(ueIndex) << "] at "
                           << ueNodes.Get(ueIndex)->GetObject<MobilityModel>()->GetPosition());
    }
    for (size_t gnbIndex = 0; gnbIndex < gNbNodes.GetN(); gnbIndex++)
    {
        NS_LOG_INFO("gNB [" << gNbNodes.Get(gnbIndex) << "] at "
                            << gNbNodes.Get(gnbIndex)->GetObject<MobilityModel>()->GetPosition());
    }
    /*
     * Setup the NR module:
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     * - NrChannelHelper, which takes care of the spectrum channel
     */
    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    nrHelper->SetEpcHelper(epcHelper);

    uint8_t numCc = 1; // Number of component carriers
    CcBwpCreator ccBwpCreator;
    auto band = ccBwpCreator.CreateOperationBandContiguousCc({centralFrequency, bandwidth, numCc});

    if (channelModel == "ThreeGpp" || channelModel == "NYU" || channelModel == "TwoRay")
    {
        // Create the ideal beamforming helper in case of a non-phased array model
        Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
        nrHelper->SetBeamformingHelper(idealBeamformingHelper);
        // First configure the channel helper object factories
        channelHelper->ConfigureFactories(scenario, channelConditionModel, channelModel);
        // Set channel condition attributes
        channelHelper->SetChannelConditionModelAttribute("UpdatePeriod",
                                                         TimeValue(MilliSeconds(100)));
        // Beamforming method
        idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                             TypeIdValue(DirectPathBeamforming::GetTypeId()));

        // Antennas for all the UEs
        nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(ueNumRows));
        nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(ueNumCols));
        nrHelper->SetUeAntennaAttribute("AntennaElement",
                                        PointerValue(CreateObject<IsotropicAntennaModel>()));

        // Antennas for all the gNbs
        nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(gnbNumRows));
        nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(gnbNumCols));
        nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                         PointerValue(CreateObject<IsotropicAntennaModel>()));
    }
    else if (channelModel == "Friis")
    {
        // Override the default antenna model with ParabolicAntennaModel
        nrHelper->SetUeAntennaTypeId(ParabolicAntennaModel::GetTypeId().GetName());
        nrHelper->SetGnbAntennaTypeId(ParabolicAntennaModel::GetTypeId().GetName());
        // Configure Friis propagation loss model before assign it to band
        channelHelper->ConfigurePropagationFactory(FriisPropagationLossModel::GetTypeId());
    }
    else
    {
        NS_FATAL_ERROR("Invalid channel model: "
                       << channelModel << ". Choose among 'ThreeGpp', 'NYU', 'TwoRay', 'Friis'.");
    }

    // After configuring the factories, create and assign the spectrum channels to the bands
    channelHelper->AssignChannelsToBands({band});

    // Get all the BWPs
    auto allBwps = CcBwpCreator::GetAllBwps({band});
    // Set the numerology and transmission powers attributes to all the gNBs and UEs
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(bsTxPower));
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(numerology));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(ueTxPower));

    // Install and get the pointers to the NetDevices
    NetDeviceContainer gNbNetDev = nrHelper->InstallGnbDevice(gNbNodes, allBwps);
    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueNodes, allBwps);

    randomStream += nrHelper->AssignStreams(gNbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    Ptr<Node> remoteHost = CreateObject<Node>();
    InternetStackHelper internet;
    internet.Install(remoteHost);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);

    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;

    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(ueNodes);

    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // assign IP address to UEs, and install UDP downlink applications
    uint16_t dlPort = 1234;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for (size_t i = 0; i < ueNodes.GetN(); i++)
    {
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(i)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
        UdpServerHelper dlPacketSinkHelper(dlPort);
        serverApps.Add(dlPacketSinkHelper.Install(ueNodes.Get(i)));
        UdpClientHelper dlClient(ueIpIface.GetAddress(i), dlPort);
        dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        dlClient.SetAttribute("PacketSize", UintegerValue(100));
        clientApps.Add(dlClient.Install(remoteHost));
    }
    // attach UEs to the closest eNB
    nrHelper->AttachToClosestGnb(ueNetDev, gNbNetDev);
    // start UDP server and client apps
    serverApps.Start(udpTime);
    clientApps.Start(udpTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);

    // Check pathloss traces
    nrHelper->EnablePathlossTraces();
    FlowMonitorHelper flowmonHelper;
    NodeContainer flowNodes;
    flowNodes.Add(remoteHost);
    flowNodes.Add(ueNodes);

    auto monitor = flowmonHelper.Install(flowNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(1));
    Simulator::Stop(simTime);
    Simulator::Run();

    monitor->CheckForLostPackets(maxDelay);
    auto stats = monitor->GetFlowStats();
    auto classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    double flowDuration = (simTime - udpTime).GetSeconds();
    for (auto stat = stats.begin(); stat != stats.end(); stat++)
    {
        auto flow = classifier->FindFlow(stat->first);
        outputFile << "Flow ID: " << stat->first << " Src Addr " << flow.sourceAddress
                   << " Dst Addr " << flow.destinationAddress << " Src Port " << flow.sourcePort
                   << " Dst Port " << flow.destinationPort << std::endl;
        outputFile << "Tx Packets: " << stat->second.txPackets << std::endl;
        outputFile << "Rx Packets: " << stat->second.rxPackets << std::endl;
        outputFile << "Lost Packets: " << stat->second.lostPackets << std::endl;
        outputFile << "Throughput: " << stat->second.rxBytes * 8.0 / flowDuration / 1000 / 1000
                   << " Mbps\n"
                   << std::endl;
        outputFile << "Mean delay:  "
                   << 1000 * stat->second.delaySum.GetSeconds() / stat->second.rxPackets
                   << std::endl;
        outputFile << "Mean jitter:  "
                   << 1000 * stat->second.jitterSum.GetSeconds() / stat->second.rxPackets
                   << " ms\n";
    }
}
