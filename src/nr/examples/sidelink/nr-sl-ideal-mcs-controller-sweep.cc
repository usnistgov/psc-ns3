//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file nr-sl-ideal-mcs-controller-sweep.cc
 * @brief A example used to observe MCS changes as distance is sweeped
 *
 * The program is configured to start two nodes with minimum distance
 * separation and gradually increase the distance in 10 meter steps
 * until the maximum distance is reached.  Packets are sent every
 * 250 ms, and one node moves a step every 500 ms.  There are therefore
 * Two packets that are sent at each distance.  The first packet sent
 * at a particular distance uses the MCS that was calculated from the
 * SNR of the packet sent at the previous distance.  The program tracks
 * all transmissions and receptions and correlates the MCS used by a
 * transmission with the SNR and distance used by a reception.
 *
 * At the end of the program, all samples are written to a file
 * named 'ideal-mcs-controller-<error-model>-<margin>-<target>.dat'
 * and the number of transmitted and received packets are printed.
 *
 * The Urban Microcell (UMi) LOS path loss model from WINNER+ B1
 * model is used.  The fading error model may be static or EPA.
 *
 * The command line arguments are:
 * - minDistance (in m)
 * - maxDistance (in m)
 * - errorModel (static or epa)
 *
 * The margin and TBLER target can be modified by command-line arguments:
 * --ns3::NrSlIdealMcsController::Margin (in dB)
 * --ns3::NrSlIdealMcsController::TargetBler
 *
 * The distance is swept for (maxDistance - minDistance)/distanceStep distances.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <tuple>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlIdealMcsControllerSweep");

Ptr<ConstantPositionMobilityModel> mobilityModel;
double distance;     // meters
double distanceStep; // meters
double lastSnr{0};
double lastDistance{0};
std::vector<std::pair<Time, uint8_t>> mcsChanges;
std::vector<std::tuple<Time, uint8_t, double, double>> pscchTransmissions;
std::vector<std::tuple<Time, double, double>> psschReceptions;
std::tuple<double, double, uint8_t> lastEntry;
std::vector<std::tuple<double, double, uint8_t>> dataset;
bool firstTransmission{true};

void
McsChange(uint32_t dstL2Id, uint8_t oldMcs, uint8_t newMcs)
{
    if (oldMcs != newMcs)
    {
        NS_LOG_INFO("MCS change from " << +oldMcs << " to " << +newMcs << " towards destination "
                                       << dstL2Id);
    }
    mcsChanges.push_back(std::make_pair(Now(), newMcs));
}

void
SlPscchScheduling(SlPscchUeMacStatParameters params)
{
    NS_LOG_INFO("MCS announced in SCI-1: " << +params.mcs << " subchannels "
                                           << params.slPsschSubChLength << " due to SNR " << lastSnr
                                           << " distance " << lastDistance);
    std::tuple<Time, uint8_t, double, double> entry(Now(), params.mcs, lastSnr, lastDistance);
    pscchTransmissions.push_back(entry);
    if (firstTransmission)
    {
        firstTransmission = false;
    }
    else
    {
        if (std::get<0>(lastEntry) != lastSnr || std::get<1>(lastEntry) != lastDistance ||
            std::get<2>(lastEntry) != params.mcs)
        {
            dataset.push_back(
                std::tuple<double, double, uint8_t>(lastSnr, lastDistance, params.mcs));
        }
    }
    lastEntry = std::tuple<double, double, uint8_t>(lastSnr, lastDistance, params.mcs);
}

void
NotifyRxPssch(SlRxDataPacketTraceParams traceParams)
{
    double snrDb = 10 * std::log10(traceParams.m_sinr);
    NS_LOG_INFO("PSSCH received with SNR " << snrDb << " across " << traceParams.m_rbAssignedNum
                                           << " resource blocks at distance " << distance);
    // The SL configuration is for 5 subchannels of 10 RBs each, or 50 RBs.  Some
    // transmissions will not use the full 50 RBs so will require
    if (traceParams.m_rbAssignedNum < 50)
    {
        double widebandCorrectionDb = 10 * std::log10(50.0 / traceParams.m_rbAssignedNum);
        NS_LOG_DEBUG("Reducing received " << snrDb << " dB by " << widebandCorrectionDb
                                          << " dB to calculate the wideband SNR");
        snrDb -= widebandCorrectionDb;
    }
    std::tuple<Time, double, double> entry(Now(), snrDb, distance);
    psschReceptions.push_back(entry);
    lastSnr = snrDb;
    lastDistance = distance;
}

std::tuple<ApplicationContainer, ApplicationContainer> CreateTraffic(Ptr<NetDevice> clientDevice,
                                                                     Ptr<NetDevice> serverDevice,
                                                                     Ipv4Address destination,
                                                                     uint16_t port,
                                                                     SidelinkInfo slInfo,
                                                                     uint32_t maxPackets,
                                                                     uint32_t packetSize,
                                                                     Time interval,
                                                                     Time applicationJitterTime,
                                                                     Ptr<NrSlHelper> nrSlHelper,
                                                                     Time startTime,
                                                                     Time finalSimTime,
                                                                     int64_t streamIndex);

uint32_t rxByteCounter = 0;
uint32_t txByteCounter = 0;
uint32_t rxPktCounter = 0;
uint32_t txPktCounter = 0;

/**
 * @brief Method to listen to the packet sink application trace Rx.
 * @param packet The packet
 * @param The address of the transmitter
 */
void
ReceivePacket(Ptr<const Packet> packet, const Address& from [[maybe_unused]])
{
    rxByteCounter += packet->GetSize();
    rxPktCounter++;
}

/**
 * @brief Method to listen to the transmitting application trace Tx.
 * @param packet The packet
 */
void
TransmitPacket(Ptr<const Packet> packet)
{
    txByteCounter += packet->GetSize();
    txPktCounter++;
}

void
Move()
{
    NS_LOG_DEBUG("Moving from distance " << distance << " to " << distance + distanceStep);
    distance += distanceStep;
    mobilityModel->SetPosition(Vector(distance, 0, 0));
}

int
main(int argc, char* argv[])
{
    // Topology
    double maxDistance = 1500; // meters
    distanceStep = 10;
    double minDistance = 100; // meters

    // Traffic parameters
    const uint32_t packetSize = 50; // bytes. Keep below 57 bytes to avoid RLC segmentation;
                                    // account for 20 (IPv4) + 8 (UDP) + 2 (RLC) + 5 (SCI-2A)
                                    // = 35 bytes overhead.

    // The application should start at time 10 seconds or later to allow the routing to converge
    Time trafficStartTime(Seconds(2)); // Time to start the traffic in the application layer
                                       // Actual start time will be at 3 sec
    Time packetInterval = MilliSeconds(250);
    Time movementInterval = MilliSeconds(500);

    // NR parameters
    uint16_t numerologyBwpSl(0);          // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6); // band n14 (793 MHz)
    double bandwidthBandSl(10e6);         // Units of Hz
    double txPower(23);                   // Units of dBm
    std::string errorModelType("static"); // error model type (static or epa)

    int64_t randomStreamIndex(1000);
    int64_t randomStreamIncrement(1000);
    [[maybe_unused]] int64_t streamsUsed(0);

    CommandLine cmd;
    cmd.Usage("Sidelink ideal MCS controller example");
    cmd.AddValue("errorModel", "Loss model type (static or epa)", errorModelType);
    cmd.AddValue("maxDistance", "Max distance in meters", maxDistance);
    cmd.AddValue("minDistance", "Min distance in meters", minDistance);
    cmd.Parse(argc, argv);

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

    NS_ABORT_MSG_IF(minDistance >= maxDistance, "Error in distance");
    NS_ABORT_MSG_IF(distanceStep <= 1, "Error in distance step");
    uint32_t movements = static_cast<uint32_t>((maxDistance - minDistance) / distanceStep);
    uint32_t maxPackets = 2 * movements;
    distance = minDistance;

    for (uint32_t i = 1; i <= movements; i++)
    {
        Simulator::Schedule(trafficStartTime + i * movementInterval, &Move);
    }

    // Simulation timeline parameters
    Time simTime(trafficStartTime + MilliSeconds(500) * movements + Seconds(2));

    // Create channel before nodes
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    auto lossModel = CreateObject<LogDistancePropagationLossModel>();
    // Urban Microcell (UMi) LOS model from WINNER+ B1 model, also in NIST LLS
    // The NIST LLS code is:
    // 40 * log10(d_m) + 7.56 - 17.3 * log10(hBS_e) - 17.3 * log10(hMS_e) + 2.7 * log10(f_GHz);
    // We can represent this by the ns-3 LogDistance model by setting the exponent to 4
    // and by setting the ReferenceLoss attribute to the other terms.  The heights are
    // 1.5m.
    //
    // Using this loss model and the static error model:
    // - the distance of 1200m can be used to send at MCS 0
    // - the distance of 800m can be used to send at MCS 14
    // - the distance of 400m can be used to send at MCS 28
    //
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
    auto mobilityModelOrigin = CreateObject<ConstantPositionMobilityModel>();
    mobilityModelOrigin->SetPosition(Vector(0, 0, 0));
    node->AggregateObject(mobilityModelOrigin);
    nodes.Add(node);
    node = CreateObject<Node>();
    mobilityModel = CreateObject<ConstantPositionMobilityModel>();
    mobilityModel->SetPosition(Vector(distance, 0, 0));
    node->AggregateObject(mobilityModel);
    nodes.Add(node);

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
    nrHelper->SetMcsControllerTypeId(NrSlIdealMcsController::GetTypeId());

    // Override helper method defaults
    nrHelper->SetAttribute("CentralFrequency", DoubleValue(centralFrequencyBandSl));
    nrHelper->SetAttribute("Bandwidth", DoubleValue(bandwidthBandSl));
    nrHelper->SetAttribute("Numerology", UintegerValue(numerologyBwpSl));
    // Add NR SL devices to the nodes in the MANET container
    auto netDevices = nrHelper->ConfigureSlNetwork(nodes, channel);

    // Configure internet
    InternetStackHelper internet;
    internet.Install(nodes);
    streamsUsed = internet.AssignStreams(nodes, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;

    // Assign IP address for the UEs
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ipIfaces = addrHelper.Assign(netDevices);

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;           // this is unused
    slInfo.m_rri = MilliSeconds(20); // this is unused
    uint16_t port = 8000;
    Time applicationJitterTime = Seconds(0);
    auto [clientApps, serverApps] = CreateTraffic(netDevices.Get(0),
                                                  netDevices.Get(1),
                                                  ipIfaces.GetAddress(1),
                                                  port,
                                                  slInfo,
                                                  maxPackets,
                                                  packetSize,
                                                  packetInterval,
                                                  applicationJitterTime,
                                                  nrHelper,
                                                  trafficStartTime,
                                                  simTime,
                                                  randomStreamIndex);

    // Trace receptions
    std::ostringstream path;
    path << "/NodeList/1/ApplicationList/0/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    path.str("");
    path << "/NodeList/0/ApplicationList/0/$ns3::OnOffApplication/Tx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&TransmitPacket));
    path.str("");

    // Trace MCS change
    NrSlTraceHelper traceHelper;
    auto scheduler =
        traceHelper.GetNrSlUeMacScheduler(nodes.Get(0))->GetObject<NrSlUeMacSchedulerDefault>();
    scheduler->TraceConnectWithoutContext("McsChange", MakeCallback(&McsChange));
    // Trace the use of the MCS change by the transmitting NrSlUeMac
    auto mac = traceHelper.GetNrSlUeMac(nodes.Get(0));
    mac->TraceConnectWithoutContext("SlPscchScheduling", MakeCallback(&SlPscchScheduling));
    // Trace the reception of the PSSCH to obtain the SNR
    auto phy = traceHelper.GetNrSlSpectrumPhy(nodes.Get(1));
    phy->TraceConnectWithoutContext("RxPsschTraceUe", MakeCallback(&NotifyRxPssch));

    /******************* Set random variable stream numbers **************************/
    randomStreamIndex += randomStreamIncrement;
    streamsUsed = nrHelper->AssignStreams(netDevices, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;
    streamsUsed += ApplicationHelper::AssignStreamsToAllApps(nodes, randomStreamIndex);
    /******************* End set random variable stream numbers **********************/

    Simulator::Stop(simTime + TimeStep(1));
    Simulator::Run();

    PointerValue val;
    scheduler->GetAttribute("McsController", val);
    auto controller = val.Get<NrSlIdealMcsController>();
    NS_ASSERT_MSG(controller, "Did not find controller");
    DoubleValue margin;
    controller->GetAttribute("Margin", margin);
    std::ostringstream ossMargin;
    ossMargin << std::fixed << std::setprecision(1) << margin.Get();
    DoubleValue target;
    controller->GetAttribute("TargetBler", target);
    int exponent = static_cast<int>(std::floor(std::log10(std::abs(target.Get()))));
    int precision = 0 - exponent;
    std::ostringstream ossTarget;
    ossTarget << std::fixed << std::setprecision(precision) << target.Get();

    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream =
        ascii.CreateFileStream("ideal-mcs-controller-" + errorModelType + "-" + ossMargin.str() +
                               "-" + ossTarget.str() + ".dat");

    NS_LOG_DEBUG("Printing contents of ideal-mcs-controller-" + errorModelType + "-" +
                 ossMargin.str() + "-" + ossTarget.str() + ".dat");
    for (auto& it : dataset)
    {
        NS_LOG_DEBUG(std::get<0>(it) << " " << std::get<1>(it) << " " << +std::get<2>(it));
        *stream->GetStream() << std::get<0>(it) << " " << std::get<1>(it) << " " << +std::get<2>(it)
                             << std::endl;
    }
    std::cout << "Total Tx packets = " << txPktCounter << std::endl;
    std::cout << "Total Rx packets = " << rxPktCounter << std::endl;
    std::cout << dataset.size()
              << " distances traced in ideal-mcs-controller-" + errorModelType + "-" +
                     ossMargin.str() + "-" + ossTarget.str() + ".dat"
              << std::endl;
    NS_LOG_DEBUG("Printing contents of pscchTransmissions");
    for (auto& it : pscchTransmissions)
    {
        NS_LOG_DEBUG("Tx " << std::get<0>(it).GetSeconds() << " " << +std::get<1>(it) << " "
                           << std::get<2>(it) << " " << std::get<3>(it));
    }
    NS_LOG_DEBUG("Printing contents of psschReceptions");
    for (auto& it : psschReceptions)
    {
        NS_LOG_DEBUG("Rx " << std::get<0>(it).GetSeconds() << " " << std::get<1>(it) << " "
                           << std::get<2>(it));
    }
    NS_LOG_DEBUG("Printing MCS changes");
    for (auto& it : mcsChanges)
    {
        NS_LOG_DEBUG("MCS " << it.first.GetSeconds() << " " << +it.second);
    }

    Simulator::Destroy();
    return 0;
}

std::tuple<ApplicationContainer, ApplicationContainer>
CreateTraffic(Ptr<NetDevice> clientDevice,
              Ptr<NetDevice> serverDevice,
              Ipv4Address destination,
              uint16_t port,
              SidelinkInfo slInfo,
              uint32_t maxPackets,
              uint32_t packetSize,
              Time interval,
              Time applicationJitterTime,
              Ptr<NrSlHelper> nrSlHelper,
              Time startTime,
              Time finalSimTime,
              int64_t streamIndex)
{
    Ptr<NrSlTft> txTft;
    Ptr<NrSlTft> rxTft;
    Address remoteAddress = InetSocketAddress(destination, port);
    txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, port, slInfo);
    rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, port, slInfo);
    nrSlHelper->ActivateNrSlBearer(startTime, clientDevice, txTft);
    nrSlHelper->ActivateNrSlBearer(startTime, serverDevice, rxTft);
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(maxPackets * packetSize));
    // In the future (ns-3.44 or greater) the below two lines can be replaced
    // by OnOffHelper::SetConstantInterval()
    DataRate dataRate{static_cast<uint64_t>(packetSize * 8 / interval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, packetSize);
    ApplicationContainer clientApps = sidelinkClient.Install(clientDevice->GetNode());
    auto uniformRv = CreateObject<UniformRandomVariable>();
    uniformRv->SetAttribute("Max", DoubleValue(applicationJitterTime.GetSeconds()));
    uniformRv->SetStream(streamIndex++);
    clientApps.StartWithJitter(startTime, uniformRv);
    clientApps.Stop(finalSimTime);
    ApplicationContainer serverApps;
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    serverApps = sidelinkSink.Install(serverDevice->GetNode());
    serverApps.Start(startTime);
    return std::make_tuple(clientApps, serverApps);
}
