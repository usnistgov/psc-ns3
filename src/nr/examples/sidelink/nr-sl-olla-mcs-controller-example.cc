//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file nr-sl-olla-mcs-controller-example.cc
 * @brief Demonstrate OLLA SINR tracking with a step change in channel conditions
 *
 * Two UEs communicate via sidelink using an SPS grant with HARQ enabled.
 * The receiver starts at distance1 and moves to distance2 at a configurable
 * slot, creating a step change in the channel SINR.  The OLLA controller
 * adapts its SINR estimate in response to ACK/NACK feedback.
 *
 * The example outputs a space-delimited data file with the ground truth
 * channel SINR and the OLLA SINR estimate at each MCS selection event.
 * A companion Python script runs this example with multiple NACK step
 * sizes and plots the results to reproduce Figure 1 (top subfigure) of
 * the SALAD paper.
 *
 * An Urban Microcell (UMi) LOS path loss model from WINNER+ B1 model is
 * used.  Using this loss model and the static error model:
 * - the distance of 1200m can be used to send at MCS 0
 * - the distance of 800m can be used to send at MCS 14
 * - the distance of 400m can be used to send at MCS 28
 *
 * The OLLA PenaltyStep and other attributes can be set via the command line
 * using --ns3::NrSlOllaMcsController::PenaltyStep=X.
 *
 * The example can be run with NS_LOG="NrSlOllaMcsControllerExample"
 * for more information.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"

#include <fstream>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlOllaMcsControllerExample");

/**
 * Record of one SinrEstimate trace callback.
 */
struct SinrEstimateRecord
{
    Time time;             ///< simulation time
    double sinrEstimateDb; ///< OLLA SINR estimate in dB (gamma_rep + Delta)
};

/// Collected SINR estimate samples
std::vector<SinrEstimateRecord> g_sinrEstimateSamples;

/// Collected unfiltered (latest observed) SINR at each SinrEstimate trace
std::vector<double> g_unfilteredSinrAtEstimate;

/// Collected filtered (CQI-gated) SINR at each SinrEstimate trace
std::vector<double> g_filteredSinrAtEstimate;

uint32_t g_rxPktCounter = 0;
uint32_t g_txPktCounter = 0;

/**
 * Callback for the NrSlOllaMcsController SinrEstimate trace source.
 *
 * @param dstL2Id the destination L2 ID
 * @param unfilteredSinrDb the unfiltered (latest observed) channel SINR in dB
 * @param filteredSinrDb the filtered (CQI-gated) channel SINR in dB
 * @param sinrOffsetDb the OLLA offset Delta in dB
 * @param sinrEstimateDb the adjusted SINR estimate in dB
 */
void
SinrEstimateCallback([[maybe_unused]] uint32_t dstL2Id,
                     double unfilteredSinrDb,
                     double filteredSinrDb,
                     [[maybe_unused]] double sinrOffsetDb,
                     double sinrEstimateDb)
{
    NS_LOG_INFO("SinrEstimate: unfiltered " << unfilteredSinrDb << " dB, filtered "
                                            << filteredSinrDb << " dB, offset " << sinrOffsetDb
                                            << " dB, estimate " << sinrEstimateDb << " dB");
    g_sinrEstimateSamples.push_back({Simulator::Now(), sinrEstimateDb});
    g_unfilteredSinrAtEstimate.push_back(unfilteredSinrDb);
    g_filteredSinrAtEstimate.push_back(filteredSinrDb);
}

/**
 * @brief Method to listen to the packet sink application trace Rx.
 * @param packet The packet
 * @param from The address of the transmitter
 */
void
ReceivePacket([[maybe_unused]] Ptr<const Packet> packet, [[maybe_unused]] const Address& from)
{
    g_rxPktCounter++;
}

/**
 * @brief Method to listen to the transmitting application trace Tx.
 * @param packet The packet
 */
void
TransmitPacket([[maybe_unused]] Ptr<const Packet> packet)
{
    g_txPktCounter++;
}

int
main(int argc, char* argv[])
{
    // Topology
    double distance1(600);    // meters, first and third phase (~10 dB)
    double distance2(250);    // meters, second phase (~25 dB)
    uint32_t stepSlot1(667);  // slot at which distance changes to distance2
    uint32_t stepSlot2(1333); // slot at which distance changes back to distance1
    uint32_t simSlots(2000);  // total slots to simulate

    // Traffic parameters
    uint32_t udpPacketSize = 60; // bytes

    // Sidelink bearers activation time
    Time slBearersActivationTime = Seconds(1.9);

    // NR parameters
    uint16_t numerologyBwpSl(0);          // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6); // band n14 (793 MHz)
    double bandwidthBandSl(10e6);         // Units of Hz
    double txPower(23);                   // Units of dBm
    std::string errorModelType("static"); // channel error model type (static or epa)
    std::string ollaErrorModelType("static"); // OLLA controller error model type (static or epa)
    std::string outputFile;

    int64_t randomStreamIndex(1000);
    int64_t randomStreamIncrement(1000);

    CommandLine cmd;
    cmd.Usage("Sidelink OLLA MCS controller example");
    cmd.AddValue("errorModel", "Channel error model type (static or epa)", errorModelType);
    cmd.AddValue("ollaErrorModel",
                 "OLLA controller error model type (static or epa)",
                 ollaErrorModelType);
    cmd.AddValue("distance1", "Distance in meters for first and third phase", distance1);
    cmd.AddValue("distance2", "Distance in meters for second phase", distance2);
    cmd.AddValue("stepSlot1", "Slot at which distance changes to distance2", stepSlot1);
    cmd.AddValue("stepSlot2", "Slot at which distance changes back to distance1", stepSlot2);
    cmd.AddValue("simSlots", "Total number of slots to simulate", simSlots);
    cmd.AddValue("outputFile", "Output data file name", outputFile);
    cmd.Parse(argc, argv);

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

    // With numerology 0, one slot is 1 ms
    Time simTime = MilliSeconds(simSlots);

    // Final simulation time is the sum of start up time, data transfer time,
    // and finishing time of 100 ms.
    Time finalSlBearersActivationTime = slBearersActivationTime + Seconds(0.01);
    Time finalSimTime = simTime + finalSlBearersActivationTime + MilliSeconds(100);

    // Create channel before nodes
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    auto lossModel = CreateObject<LogDistancePropagationLossModel>();
    // Urban Microcell (UMi) LOS model from WINNER+ B1 model, also in NIST LLS
    // The NIST LLS code is:
    // 40 * log10(d_m) + 7.56 - 17.3 * log10(hBS_e) - 17.3 * log10(hMS_e) + 2.7 * log10(f_GHz);
    // We can represent this by the ns-3 LogDistance model by setting the exponent to 4
    // and by setting the ReferenceLoss attribute to the other terms.  The heights are
    // 1.5m.
    double referenceLoss =
        7.56 - 2 * 17.3 * std::log10(1.5) + 2.7 * std::log10(centralFrequencyBandSl / 1e9);
    lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute("Exponent",
                                                                          DoubleValue(4));
    lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute(
        "ReferenceLoss",
        DoubleValue(referenceLoss));
    channel->AddPropagationLossModel(lossModel);

    // Node and mobility configuration
    NodeContainer nodes;
    auto node = CreateObject<Node>();
    auto mm = CreateObject<ConstantPositionMobilityModel>();
    mm->SetPosition(Vector(0, 0, 0));
    node->AggregateObject(mm);
    nodes.Add(node);
    node = CreateObject<Node>();
    auto receiverMobility = CreateObject<ConstantPositionMobilityModel>();
    receiverMobility->SetPosition(Vector(distance1, 0, 0));
    node->AggregateObject(receiverMobility);
    nodes.Add(node);

    // Schedule the distance step changes
    Simulator::Schedule(finalSlBearersActivationTime + MilliSeconds(stepSlot1),
                        &ConstantPositionMobilityModel::SetPosition,
                        receiverMobility,
                        Vector(distance2, 0, 0));
    Simulator::Schedule(finalSlBearersActivationTime + MilliSeconds(stepSlot2),
                        &ConstantPositionMobilityModel::SetPosition,
                        receiverMobility,
                        Vector(distance1, 0, 0));

    // Configure NR SL
    auto nrHelper = CreateObject<NrSlHelper>();
    if (errorModelType == "static")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
    }
    else if (errorModelType == "epa")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlEpaErrorModel::GetTypeId());
    }

    // Configure any attributes for the UE SL configuration and then install to the nodes
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(true));
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(0));
    nrHelper->SetMcsControllerTypeId(NrSlOllaMcsController::GetTypeId());
    nrHelper->SetSlPsfchPeriod(1);
    nrHelper->SetSlMaxTxTransNumPssch(1);

    // Override helper method defaults
    nrHelper->SetAttribute("CentralFrequency", DoubleValue(centralFrequencyBandSl));
    nrHelper->SetAttribute("Bandwidth", DoubleValue(bandwidthBandSl));
    nrHelper->SetAttribute("Numerology", UintegerValue(numerologyBwpSl));
    // Add NR SL devices to the nodes in the MANET container
    auto netDevices = nrHelper->ConfigureSlNetwork(nodes, channel);

    // Configure internet
    InternetStackHelper internet;
    internet.Install(nodes);
    internet.AssignStreams(nodes, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;

    // Assign IP address for the UEs
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ipIfaces = addrHelper.Assign(netDevices);

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = true;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_rri = MilliSeconds(100);
    uint16_t port = 8000;

    Address remoteAddress = InetSocketAddress(ipIfaces.GetAddress(1), port);
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);

    Ptr<NrSlTft> txTft =
        Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, ipIfaces.GetAddress(1), port, slInfo);
    Ptr<NrSlTft> rxTft =
        Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, ipIfaces.GetAddress(1), port, slInfo);
    nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, netDevices.Get(0), txTft);
    nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, netDevices.Get(1), rxTft);

    // Client app: OnOff application configured to generate approximately one packet per slot.
    // With numerology 0, one slot is 1 ms, so the data rate is packetSize * 8 * 1000 bps.
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    DataRate dataRate(static_cast<uint64_t>(udpPacketSize) * 8 * 1000);
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer clientApps = sidelinkClient.Install(nodes.Get(0));
    clientApps.Start(finalSlBearersActivationTime);
    clientApps.Stop(finalSimTime);

    // Server app: PacketSink
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    ApplicationContainer serverApps = sidelinkSink.Install(nodes.Get(1));
    serverApps.Start(slBearersActivationTime);

    // Trace receptions and transmissions
    std::ostringstream path;
    path << "/NodeList/1/ApplicationList/0/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    path.str("");
    path << "/NodeList/0/ApplicationList/0/$ns3::OnOffApplication/Tx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&TransmitPacket));

    // Connect to the OLLA SinrEstimate trace on the transmitter
    NrSlTraceHelper traceHelper;
    auto scheduler =
        traceHelper.GetNrSlUeMacScheduler(nodes.Get(0))->GetObject<NrSlUeMacSchedulerDefault>();
    PointerValue controllerValue;
    scheduler->GetAttribute("McsController", controllerValue);
    auto ollaController = controllerValue.Get<NrSlOllaMcsController>();
    NS_ASSERT_MSG(ollaController, "Expected NrSlOllaMcsController on transmitter");

    // Set the OLLA controller error model (may differ from the channel error model)
    if (ollaErrorModelType == "static")
    {
        ollaController->SetAttribute("ErrorModel",
                                     PointerValue(CreateObject<NrSlStaticErrorModel>()));
    }
    else if (ollaErrorModelType == "epa")
    {
        ollaController->SetAttribute("ErrorModel", PointerValue(CreateObject<NrSlEpaErrorModel>()));
    }

    // Set some attributs to try to align better with the OLLA figure in the SALAD paper
    ollaController->SetAttribute("EnableClamping", BooleanValue(false));
    ollaController->SetAttribute("MaxSinrDelta", DoubleValue(30.0));
    ollaController->SetAttribute("CqiPeriod",
                                 StringValue("ns3::ConstantRandomVariable[Constant=100000]"));

    ollaController->TraceConnectWithoutContext("SinrEstimate", MakeCallback(&SinrEstimateCallback));

    // Build default output file name from the actual PenaltyStep value
    if (outputFile.empty())
    {
        DoubleValue penaltyStepValue;
        ollaController->GetAttribute("PenaltyStep", penaltyStepValue);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "olla-example-offset-"
            << penaltyStepValue.Get() << ".dat";
        outputFile = oss.str();
    }

    /******************* Set random variable stream numbers **************************/
    randomStreamIndex += randomStreamIncrement;
    nrHelper->AssignStreams(netDevices, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(nodes, randomStreamIndex);
    /******************* End set random variable stream numbers **********************/

    Simulator::Stop(finalSimTime);
    Simulator::Run();

    std::cout << "Total Tx packets " << g_txPktCounter << std::endl;
    std::cout << "Total Rx packets " << g_rxPktCounter << std::endl;
    std::cout << "SINR estimate samples collected " << g_sinrEstimateSamples.size() << std::endl;

    // Write data file
    std::ofstream outFile(outputFile);
    NS_ASSERT_MSG(outFile.is_open(), "Failed to open output file " << outputFile);
    outFile << "# time(s) sinr(dB) filteredSinr(dB) sinrEst(dB)" << std::endl;
    outFile << std::fixed << std::setprecision(6);
    for (std::size_t i = 0; i < g_sinrEstimateSamples.size(); ++i)
    {
        outFile << g_sinrEstimateSamples[i].time.GetSeconds() << " "
                << g_unfilteredSinrAtEstimate[i] << " " << g_filteredSinrAtEstimate[i] << " "
                << g_sinrEstimateSamples[i].sinrEstimateDb << std::endl;
    }
    outFile.close();
    std::cout << "Data written to " << outputFile << std::endl;

    Simulator::Destroy();
    return 0;
}
