// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2022 University of Washington (HARQ extensions)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

/**
 * \ingroup examples
 * \file nr-sl-mcs-random-walk.cc
 *
 * This example studies MCS controller tracking behavior under continuously
 * evolving channel conditions produced by pedestrian random waypoint mobility.
 * Each UE moves independently within its own bounding box using the
 * SteadyStateRandomWaypointMobilityModel.  The two boxes are separated
 * along the x-axis so that the inter-UE distance ranges between
 * configurable minimum and maximum values, producing a range of channel
 * conditions as the UEs move.
 *
 * Output files (written to a timestamped experiment directory) include
 * the inter-node distance trace, MCS trial results, and MCS change traces.
 * Packet counters exclude an initial warmup phase to avoid the MCS
 * controller convergence transient.
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SlMcsRandomWalk");

struct McsChange
{
    Time ts;
    uint32_t dstL2Id;
    uint8_t mcs;
};

/*
 * Global variables
 */

bool g_writePlts = false;                     //!< Indicates if gnuplot .plt files should be created
Gnuplot2dDataset g_mcsChangeDataset;          //!< The MCS plot dataset
Gnuplot2dDataset g_trialResultSuccessDataset; //!< The MCS plot dataset
Gnuplot2dDataset g_trialResultFailureDataset; //!< The MCS plot dataset
Time g_delayMin = Time::Max();                //!< Global variable to store delay min value
Time g_delayMax = Time::Min();                //!< Global variable to store delay max value
bool g_verbose = false;                       //!< Global variable to store verbose mode
bool g_writeTraces = true;                    //!< Flag to control writing traces
std::ofstream g_mcsChangeStream;              //!< File stream object to save MCS change trace
std::ofstream g_rxPsschStream;                //!< File stream object to save RX PSSCH trace
std::ofstream g_txPscchStream;                //!< File stream object to save TX PSCCH trace
std::ofstream g_txPsschStream;                //!< File stream object to save TX PSSCH trace
std::ofstream g_pktStream;                    //!< File stream object to save packet trace
std::ofstream g_trialResultStream;            //!< File stream object to save trial results
uint32_t rxByteCounter = 0;                   //!< Global variable to count RX bytes
uint32_t txByteCounter = 0;                   //!< Global variable to count TX bytes
uint32_t rxPktCounter = 0;                    //!< Global variable to count RX packets
uint32_t txPktCounter = 0;                    //!< Global variable to count TX packets
std::vector<McsChange> mcsChanges;
std::vector<std::pair<Time, uint8_t>> pscchTransmissions;

uint64_t pirCounter = 0;        //!< counter to count PIR samples
Time lastPktRxTime(Seconds(0)); //!< Global variable to store the RX time of a packet
Time pir(Seconds(0));           //!< Global variable to store PIR value

uint64_t pirCounter2 = 0;        //!< counter to count PIR samples
Time lastPktRxTime2(Seconds(0)); //!< Global variable to store the RX time of a packet
Time pir2(Seconds(0));           //!< Global variable to store PIR value

std::string g_experimentDir;         //!< Timestamped experiment output directory
std::ofstream g_distanceTraceStream; //!< File stream for distance-trace.dat
Ptr<Node> g_nodeA;                   //!< First UE node (for distance computation)
Ptr<Node> g_nodeB;                   //!< Second UE node (for distance computation)
bool g_counting = false;             //!< Whether warmup has elapsed

/*
 * Function declarations (defined below)
 */
uint32_t GetMinSubchannels(uint8_t mcs,
                           uint32_t transportBlockSize,
                           uint16_t maxSubchannels = 5,
                           uint8_t mcsTable = 1,
                           uint32_t subchannelSize = 10,
                           uint32_t numPscchRbs = 10,
                           uint32_t pscchSymLength = 1);
// Utility function for tracing
uint32_t ContextToNodeId(std::string context);
// Trace received packets at the application layer
void ReceivePacket(Ptr<const Packet> packet,
                   const Address& from,
                   const Address& to,
                   const SeqTsSizeHeader& header);
// Trace transmitted packets
void TransmitPacket(Ptr<const Packet> packet,
                    const Address& from,
                    const Address& to,
                    const SeqTsSizeHeader& header);
// Functions to compute the PIR statistic
void ComputePir(Ptr<const Packet> packet,
                const Address& from,
                const Address& to,
                const SeqTsSizeHeader& header);
void ComputePir2(Ptr<const Packet> packet,
                 const Address& from,
                 const Address& to,
                 const SeqTsSizeHeader& header);
// Trace transmission of RLC PDUs
void TraceTxRlcPduWithTxRnti(uint64_t imsi,
                             uint16_t rnti,
                             uint16_t txRnti,
                             uint8_t lcid,
                             uint32_t bytes,
                             double delay);
// Trace reception of RLC PDUs
void TraceRxRlcPduWithTxRnti(uint64_t imsi,
                             uint16_t rnti,
                             uint16_t txRnti,
                             uint8_t lcid,
                             uint32_t bytes,
                             double delay);
//  Verbose tracing of PSCCH scheduling events
void NotifySlPscchScheduling(const SlPscchUeMacStatParameters pscchStatsParams);
// Verbose tracing of PSSCH scheduling events
void NotifySlPsschScheduling(const SlPsschUeMacStatParameters psschStatsParams);
// Listen to received events of SCI format 1-A
void NotifySlPscchRx(const SlRxCtrlPacketTraceParams pscchStatsParams);
// Listen to received events of SCI format 2-A
void NotifySlPsschRx(const SlRxDataPacketTraceParams psschStatsParams);
// Listen to NrSpectrumPhy::TxFeedbackTrace events
void NotifyTxFeedback(std::string context, Time duration);
// Listen to NrSlUeMacHarq::HarqFeedbackReceived events
void NotifyRxHarqFeedback(std::string context, const SlHarqInfo& harqInfo);
// Listen to HARQ process ID allocation events
void NotifyAllocate(std::string context,
                    uint8_t harqId,
                    uint32_t dstL2Id,
                    bool multiplePdu,
                    Time timeout,
                    std::size_t available);
// Listen to HARQ process ID deallocation events
void NotifyDeallocate(std::string context, uint8_t harqId, std::size_t available);

void McsChangeCb(uint32_t dstL2Id, uint8_t oldMcs, uint8_t newMcs);

void CaptureMcsChanges();

void TrialResult(uint8_t mcs, uint32_t dstL2Id, bool success);

void LogDistance();
void StartCounting();

int
main(int argc, char* argv[])
{
    // Command-line variables
    bool binMcsIndex = false;
    std::string lossModelType = "umi";
    std::string mcsControllerType = "ts";
    bool harqEnabled = true;
    bool dynamicEnabled = true;
    uint8_t maxPsschTx = 1;
    uint16_t psfchPeriod = 4;
    uint16_t numerologyBwpSl = 0;
    uint16_t slSubchannelSize = 10; // PRBs
    uint16_t t1 = 2;
    uint16_t t2 = 33;
    std::string errorModelType("static");
    bool logging = false;

    // Constants for traffic generation
    uint32_t udpPacketSize = 150; // bytes
    DataRate dataRate("16Kbps");
    double warmupDuration = 20.0;       // seconds (excluded from packet statistics)
    double measurementDuration = 600.0; // seconds of statistics gathering

    // Mobility parameters (pedestrian first responder)
    //
    // Each UE moves within its own bounding box.  The two boxes are separated
    // along the x-axis so that the inter-UE distance ranges from minDistance
    // (closest edges) to maxDistance (opposite diagonal corners).
    //
    // The defaults target an MCS range of roughly 5 to 25 with the static
    // error model and umi propagation at 5.89 GHz (band n47).
    //
    // Common derivation (umi with exponent 4):
    //   Noise floor ~ -106 dBm, TX power 23 dBm => SNR = 129 - PL
    //   umi path loss: PL = 3.55 + 40*log10(d)  [dB]
    //
    // Static error model (AWGN TBLER, steep transitions ~0-17 dB SNR):
    //   MCS 25 => SNR ~ 17 dB => PL ~ 112 dB => d ~ 500 m
    //   MCS  5 => SNR ~  4 dB => PL ~ 125 dB => d ~ 1060 m
    //   Recommended: --minDistance=500 --maxDistance=1060
    //   Discrete:    --pathlossMin=112 --pathlossMax=125
    //
    // EPA error model (fading TBLER, gradual transitions ~10-25 dB SNR):
    //   MCS 25 => SNR ~ 25 dB => PL ~ 104 dB => d ~ 325 m
    //   MCS  5 => SNR ~ 10 dB => PL ~ 119 dB => d ~ 770 m
    //   Recommended: --minDistance=325 --maxDistance=770
    //   Discrete:    --pathlossMin=104 --pathlossMax=119
    //
    // Given minDistance and maxDistance, the box width W and center offset D
    // are computed from:
    //   D - W = minDistance
    //   sqrt((D + W)^2 + W^2) = maxDistance
    //
    // For minDistance=500, maxDistance=1060 (static): W ~ 260 m, D ~ 760 m
    // For minDistance=325, maxDistance=770  (EPA):    W ~ 184 m, D ~ 509 m
    //
    // Box A: [0, W] x [0, W],  Box B: [D, D+W] x [0, W]
    //
    double minDistance = 0.0;
    double maxDistance = 0.0;
    double minSpeed = 0.5; // m/s (slow walk)
    double maxSpeed = 2.0; // m/s (brisk walk)
    double minPause = 0.0; // seconds
    double maxPause = 5.0; // seconds

    // NR parameters and constants
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    double bandwidthBandSl = 10e6;
    double txPower = 23; // dBm

    // Other constants
    // Sidelink bearers activation time
    Time slBearersActivationTime = Seconds(1.9);
    Time delayBudget = Seconds(0); // Use T2 to configure selection window edge

    CommandLine cmd;
    cmd.AddValue("binMcsIndex",
                 "Group MCS values by estimated number of subchannels needed",
                 binMcsIndex);
    cmd.AddValue("warmupDuration",
                 "Initial warmup before measurement begins (seconds)",
                 warmupDuration);
    cmd.AddValue("measurementDuration",
                 "Duration of the measurement phase (seconds)",
                 measurementDuration);
    cmd.AddValue("minDistance",
                 "Minimum inter-UE distance in meters (closest box edges)",
                 minDistance);
    cmd.AddValue("maxDistance",
                 "Maximum inter-UE distance in meters (opposite diagonal corners)",
                 maxDistance);
    cmd.AddValue("minSpeed", "Minimum UE speed in m/s", minSpeed);
    cmd.AddValue("maxSpeed", "Maximum UE speed in m/s", maxSpeed);
    cmd.AddValue("minPause", "Minimum pause duration in seconds", minPause);
    cmd.AddValue("maxPause", "Maximum pause duration in seconds", maxPause);
    cmd.AddValue("lossModelType", "friis, log-distance, umi", lossModelType);
    cmd.AddValue("mcsControllerType", "ts, ideal, or olla", mcsControllerType);
    cmd.AddValue("harqEnabled", "Whether HARQ is enabled", harqEnabled);
    cmd.AddValue("dynamicEnabled",
                 "Whether dynamic scheduling is enabled; otherwise, SPS",
                 dynamicEnabled);
    cmd.AddValue("maxPsschTx", "The maximum number of PSSCH transmissions", maxPsschTx);
    cmd.AddValue("psfchPeriod", "PSFCH period, in slots", psfchPeriod);
    cmd.AddValue("numerologyBwpSl",
                 "The numerology to be used in Sidelink bandwidth part",
                 numerologyBwpSl);
    cmd.AddValue("slSubchannelSize", "The Sidelink subchannel size in RBs", slSubchannelSize);
    cmd.AddValue("t1",
                 "The start of the selection window in physical slots, "
                 "accounting for physical layer processing delay",
                 t1);
    cmd.AddValue("t2", "The end of the selection window in physical slots", t2);
    cmd.AddValue("errorModel", "Error model type (static, epa)", errorModelType);
    cmd.AddValue("logging", "Enable logging (if logging is enabled in the build)", logging);
    cmd.AddValue("writeTraces", "Flag to control the writing of output traces", g_writeTraces);
    cmd.AddValue("writePlts", "Indicates if gnuplot .plt files should be generated", g_writePlts);
    cmd.AddValue("verbose", "Indicates if output should be verbose", g_verbose);
    cmd.Parse(argc, argv);

    // Apply per-error-model defaults if the user did not override distance range
    if (minDistance == 0.0 && maxDistance == 0.0)
    {
        if (errorModelType == "epa")
        {
            minDistance = 325.0;
            maxDistance = 770.0;
        }
        else
        {
            minDistance = 500.0;
            maxDistance = 1060.0;
        }
    }

    Time finalSlBearersActivationTime = slBearersActivationTime + Seconds(0.01);
    Time measurementStart = finalSlBearersActivationTime + Seconds(warmupDuration);
    Time simTime = measurementStart + Seconds(measurementDuration);
    Time finalSimTime = simTime + MilliSeconds(100);

    // Compute box width W and center offset D from minDistance and maxDistance.
    //   D - W = minDistance
    //   sqrt((D + W)^2 + W^2) = maxDistance
    // Substituting D = minDistance + W:
    //   (minDistance + 2W)^2 + W^2 = maxDistance^2
    //   5W^2 + 4*minDistance*W + minDistance^2 - maxDistance^2 = 0
    NS_ABORT_MSG_IF(maxDistance <= minDistance, "maxDistance must be greater than minDistance");
    double a = 5.0;
    double b = 4.0 * minDistance;
    double c = minDistance * minDistance - maxDistance * maxDistance;
    double discriminant = b * b - 4.0 * a * c;
    NS_ABORT_MSG_IF(discriminant < 0, "No solution for box geometry");
    double boxWidth = (-b + std::sqrt(discriminant)) / (2.0 * a);
    double centerOffset = minDistance + boxWidth;

    std::cout << "Warmup " << warmupDuration << " s, measurement " << measurementDuration
              << " s, total sim " << finalSimTime.GetSeconds() << " s" << std::endl;
    std::cout << "Distance range [" << minDistance << ", " << maxDistance << "] m" << std::endl;
    std::cout << "Box width " << boxWidth << " m, center offset " << centerOffset << " m"
              << std::endl;
    std::cout << "Speed range [" << minSpeed << ", " << maxSpeed << "] m/s" << std::endl;
    std::cout << "Pause range [" << minPause << ", " << maxPause << "] s" << std::endl;

    // Create timestamped experiment output directory
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&timeT, &tm);
    std::ostringstream dirName;
    dirName << "experiments/random-walk-" << mcsControllerType << "-" << errorModelType << "-"
            << RngSeedManager::GetRun() << "-" << std::put_time(&tm, "%Y-%m-%dT%H-%M-%S");
    g_experimentDir = dirName.str();
    std::filesystem::create_directories(g_experimentDir);
    std::cout << "Experiment directory: " << g_experimentDir << std::endl;

    // Checks on configurable values
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);
    NS_ABORT_UNLESS(psfchPeriod == 0 || psfchPeriod == 1 || psfchPeriod == 2 || psfchPeriod == 4);

    if (logging)
    {
        LogLevel logLevel =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeMacHarq", logLevel);
        LogComponentEnable("NrSlUeMacScheduler", logLevel);
        LogComponentEnable("NrSlUeMacSchedulerDefault", logLevel);
        LogComponentEnable("NrSlThompsonSamplingMcsController", logLevel);
        LogComponentEnable("NrUeMac", logLevel);
    }

    // Default values for the simulation. We are progressively removing all
    // the instances of SetDefault, but we need it for legacy code (LTE)
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    if (binMcsIndex)
    {
        std::stringstream mcsIndex;
        mcsIndex << "0";
        uint32_t minSubs = GetMinSubchannels(0, udpPacketSize + 35);
        for (uint8_t mcs = 1; mcs < 29; mcs++)
        {
            uint32_t subs = GetMinSubchannels(mcs, udpPacketSize + 35);
            if (subs < minSubs)
            {
                minSubs = subs;
                mcsIndex << "," << (+mcs);
                if (g_verbose)
                {
                    std::cout << "MCS " << +mcs << " requires " << minSubs << " subchannels"
                              << std::endl;
                }
            }
        }
        if (g_verbose)
        {
            std::cout << "MCS Index = {" << mcsIndex.str() << "}" << std::endl;
        }

        Config::SetDefault("ns3::NrSlThompsonSamplingMcsController::McsIndex",
                           StringValue(mcsIndex.str()));
    }

    NodeContainer ueContainer;
    ueContainer.Create(2);

    // Set up random waypoint mobility.  Each UE moves within its own
    // bounding box.  The SteadyStateRandomWaypointMobilityModel starts
    // each node at a position and velocity drawn from the steady-state
    // distribution, avoiding initialization transients.
    //
    // Box A: [0, boxWidth] x [0, boxWidth]
    // Box B: [centerOffset, centerOffset + boxWidth] x [0, boxWidth]
    //
    MobilityHelper mobilityA;
    mobilityA.SetMobilityModel("ns3::SteadyStateRandomWaypointMobilityModel",
                               "MinSpeed",
                               DoubleValue(minSpeed),
                               "MaxSpeed",
                               DoubleValue(maxSpeed),
                               "MinPause",
                               DoubleValue(minPause),
                               "MaxPause",
                               DoubleValue(maxPause),
                               "MinX",
                               DoubleValue(0.0),
                               "MaxX",
                               DoubleValue(boxWidth),
                               "MinY",
                               DoubleValue(0.0),
                               "MaxY",
                               DoubleValue(boxWidth),
                               "Z",
                               DoubleValue(1.5));
    mobilityA.Install(ueContainer.Get(0));

    MobilityHelper mobilityB;
    mobilityB.SetMobilityModel("ns3::SteadyStateRandomWaypointMobilityModel",
                               "MinSpeed",
                               DoubleValue(minSpeed),
                               "MaxSpeed",
                               DoubleValue(maxSpeed),
                               "MinPause",
                               DoubleValue(minPause),
                               "MaxPause",
                               DoubleValue(maxPause),
                               "MinX",
                               DoubleValue(centerOffset),
                               "MaxX",
                               DoubleValue(centerOffset + boxWidth),
                               "MinY",
                               DoubleValue(0.0),
                               "MaxY",
                               DoubleValue(boxWidth),
                               "Z",
                               DoubleValue(1.5));
    mobilityB.Install(ueContainer.Get(1));

    // Store node pointers for distance computation in callbacks
    g_nodeA = ueContainer.Get(0);
    g_nodeB = ueContainer.Get(1);

    // NR configuration
    Ptr<NrSlHelper> nrHelper = CreateObject<NrSlHelper>();

    auto channel = CreateObject<SingleModelSpectrumChannel>();
    Ptr<PropagationLossModel> lossModel;
    if (lossModelType == "friis")
    {
        lossModel = CreateObject<FriisPropagationLossModel>();
        lossModel->GetObject<FriisPropagationLossModel>()->SetAttribute(
            "Frequency",
            DoubleValue(centralFrequencyBandSl));
    }
    else if (lossModelType == "log-distance")
    {
        lossModel = CreateObject<LogDistancePropagationLossModel>();
        double referenceLoss = 46.6 + 20 * std::log10(centralFrequencyBandSl / 5e9);
        lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute("Exponent",
                                                                              DoubleValue(2));
        lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute(
            "ReferenceLoss",
            DoubleValue(referenceLoss));
    }
    else if (lossModelType == "umi")
    {
        lossModel = CreateObject<LogDistancePropagationLossModel>();
        double referenceLoss =
            7.56 - 2 * 17.3 * std::log10(1.5) + 2.7 * std::log10(centralFrequencyBandSl / 1e9);
        lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute("Exponent",
                                                                              DoubleValue(4));
        lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute(
            "ReferenceLoss",
            DoubleValue(referenceLoss));
    }
    else
    {
        NS_FATAL_ERROR("Supported loss models: friis, log-distance, umi");
    }
    channel->AddPropagationLossModel(lossModel);

    nrHelper->SetAttribute("CentralFrequency", DoubleValue(centralFrequencyBandSl));
    nrHelper->SetAttribute("Bandwidth", DoubleValue(bandwidthBandSl));
    nrHelper->SetAttribute("Numerology", UintegerValue(numerologyBwpSl));

    // The sidelink fast fading error model is installed in the NrSlSpectrumPhy
    // and needs to be configured separately from the above loss model.
    nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

    // Configure error model
    if (errorModelType == "static")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
    }
    else if (errorModelType == "epa")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlEpaErrorModel::GetTypeId());
    }

    if (mcsControllerType == "ideal")
    {
        nrHelper->SetMcsControllerTypeId(NrSlIdealMcsController::GetTypeId());
    }
    else if (mcsControllerType == "ts")
    {
        nrHelper->SetMcsControllerTypeId(NrSlThompsonSamplingMcsController::GetTypeId());
    }
    else if (mcsControllerType == "olla")
    {
        nrHelper->SetMcsControllerTypeId(NrSlOllaMcsController::GetTypeId());
    }

    /*
     * Antennas for all the UEs
     * We are not using beamforming in SL, rather we are using
     * quasi-omnidirectional transmission and reception, which is the default
     * configuration of the beams.
     */
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetUeSpectrumTypeId(NrSlSpectrumPhy::GetTypeId());

    // Sidelink attribute of UE MAC, which are would be common for all the UEs
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrHelper->SetUeMacAttribute("T1", UintegerValue(t1));
    nrHelper->SetUeMacAttribute("T2", UintegerValue(t2));
    nrHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));

    /*
     * Set the SL scheduler attributes
     * In this example we use NrSlUeMacSchedulerDefault scheduler, which uses
     * a fixed MCS value by default
     */
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(0));
    nrHelper->SetSlResourceReservePeriodList({0, 100});
    nrHelper->SetSlPsfchPeriod(psfchPeriod);
    nrHelper->SetSlMinTimeGapPsfch(3);
    nrHelper->SetSlMaxTxTransNumPssch(maxPsschTx);

    auto ueNetDevices = nrHelper->ConfigureSlNetwork(ueContainer, channel);

    /****************************** End SL Configuration ***********************/

    /*
     * Configure the IP stack, and activate NR sidelink bearer (s)
     *
     * This example supports IPV4 and IPV6
     */

    InternetStackHelper internet;
    internet.Install(ueContainer);

    int64_t streamBase = 2000;
    int64_t streamsUsed = internet.AssignStreams(ueContainer, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in InternetStackHelper");

    uint32_t dstL2Id = 2;
    uint16_t port = 8000;

    SidelinkInfo slInfo;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_harqEnabled = harqEnabled;
    slInfo.m_dynamic = dynamicEnabled;
    slInfo.m_pdb = delayBudget;
    slInfo.m_dstL2Id = dstL2Id;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(ueNetDevices);
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);
    Address remoteAddress = InetSocketAddress(ueIpIface.GetAddress(1, 0), port);
    Ptr<NrSlTft> tft =
        Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL, ueIpIface.GetAddress(1, 0), slInfo);
    nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDevices, tft);

    /*
     * Configure the applications:
     * Client app: OnOff application configure to generate CBR traffic
     * Server app: PacketSink application.
     */

    // Set Application in the UEs
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    std::cout << "Data rate " << dataRate.GetBitRate() << " bps" << std::endl;
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);

    ApplicationContainer clientApps = sidelinkClient.Install(ueContainer.Get(0));
    // onoff application will send the first packet at :
    // finalSlBearersActivationTime + ((Pkt size in bits) / (Data rate in bits per sec))
    clientApps.Start(finalSlBearersActivationTime);
    clientApps.Stop(finalSimTime);

    // Output app start, stop and duration
    double realAppStart = finalSlBearersActivationTime.GetSeconds() +
                          ((double)udpPacketSize * 8.0 / (dataRate.GetBitRate()));
    double appStopTime = (finalSimTime).GetSeconds();

    std::cout << "App start time " << realAppStart << " sec" << std::endl;
    std::cout << "App stop time " << appStopTime << " sec" << std::endl;

    ApplicationContainer serverApps;
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    NodeContainer sinks;
    sinks.Add(ueContainer.Get(1));

    serverApps = sidelinkSink.Install(sinks);
    serverApps.Start(Seconds(2.0));

    /*
     * Hook the traces, to be used to compute average PIR
     */
    // Trace MCS change
    NrSlTraceHelper traceHelper;
    auto scheduler = traceHelper.GetNrSlUeMacScheduler(ueContainer.Get(0))
                         ->GetObject<NrSlUeMacSchedulerDefault>();
    scheduler->TraceConnectWithoutContext("McsChange", MakeCallback(&McsChangeCb));

    PointerValue mcsControllerPtrVal;
    scheduler->GetAttribute("McsController", mcsControllerPtrVal);
    auto mcsController = mcsControllerPtrVal.Get<NrSlMcsController>();
    if (mcsController)
    {
        mcsController->TraceConnectWithoutContext("TrialResult", MakeCallback(&TrialResult));
    }

    // Trace receptions; use the following to be robust to node ID changes
    std::ostringstream path;
    path << "/NodeList/" << ueContainer.Get(1)->GetId()
         << "/ApplicationList/0/$ns3::PacketSink/RxWithSeqTsSize";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ComputePir));
    path.str("");
    path << "/NodeList/" << ueContainer.Get(0)->GetId()
         << "/ApplicationList/0/$ns3::OnOffApplication/TxWithSeqTsSize";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&TransmitPacket));
    path.str("");

    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
        "ComponentCarrierMapUe/*/NrUeMac/$ns3::NrSlUeMac/SlPscchScheduling",
        MakeCallback(&NotifySlPscchScheduling));

    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
        "ComponentCarrierMapUe/*/NrUeMac/$ns3::NrSlUeMac/SlPsschScheduling",
        MakeCallback(&NotifySlPsschScheduling));

    Config::ConnectWithoutContextFailSafe("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                          "ComponentCarrierMapUe/*/NrUeMac/TxRlcPduWithTxRnti",
                                          MakeCallback(&TraceTxRlcPduWithTxRnti));
    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
        "ComponentCarrierMapUe/*/NrUeMac/$ns3::NrSlUeMac/RxRlcPduWithTxRnti",
        MakeCallback(&TraceRxRlcPduWithTxRnti));

    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUePhy/SpectrumPhy/RxPscchTraceUe",
                                  MakeCallback(NotifySlPscchRx));

    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUePhy/SpectrumPhy/RxPsschTraceUe",
                                  MakeCallback(&NotifySlPsschRx));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/"
                    "SpectrumPhy/TxFeedbackTrace",
                    MakeCallback(&NotifyTxFeedback));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/"
                    "HarqFeedbackReceived",
                    MakeCallback(&NotifyRxHarqFeedback));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/Allocate",
                    MakeCallback(&NotifyAllocate));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/Deallocate",
                    MakeCallback(&NotifyDeallocate));

    Simulator::Schedule(Seconds(1), &CaptureMcsChanges);

    // Schedule periodic distance logging (every 1 second)
    Simulator::Schedule(finalSlBearersActivationTime, &LogDistance);

    // Start counting packets after warmup
    Simulator::Schedule(measurementStart, &StartCounting);

    if (g_writeTraces)
    {
        g_mcsChangeStream.open(g_experimentDir + "/mcs-changes.dat", std::ofstream::out);
        g_rxPsschStream.open(g_experimentDir + "/rx-pssch.dat", std::ofstream::out);
        g_txPscchStream.open(g_experimentDir + "/tx-pscch.dat", std::ofstream::out);
        g_txPsschStream.open(g_experimentDir + "/tx-pssch.dat", std::ofstream::out);
        g_pktStream.open(g_experimentDir + "/pkts.dat", std::ofstream::out);
        g_trialResultStream.open(g_experimentDir + "/trial-results.dat", std::ofstream::out);

        g_mcsChangeStream << "time dstL2Id mcs" << std::endl;
        g_rxPsschStream << "time dstL2Id mcs sinr tbler" << std::endl;
        g_txPscchStream << "time imsi mcs" << std::endl;
        g_txPsschStream << "time imsi slot harqid rv" << std::endl;
        g_pktStream << "time bytes" << std::endl;
        g_trialResultStream << "time dstL2Id mcs success" << std::endl;
    }

    // Open distance trace file
    g_distanceTraceStream.open(g_experimentDir + "/distance-trace.dat", std::ofstream::out);
    g_distanceTraceStream << "time distance" << std::endl;

    Gnuplot mcsChangesPlot;
    Gnuplot trialResultsPlot;
    if (g_writePlts)
    {
        mcsChangesPlot.SetOutputFilename(g_experimentDir + "/mcs-changes.png");
        mcsChangesPlot.SetTitle("MCS Changes");
        mcsChangesPlot.SetTerminal("png");
        mcsChangesPlot.SetLegend("Time (s)", "MCS");
        mcsChangesPlot.AppendExtra("set yrange [0:30]");

        g_mcsChangeDataset.SetTitle(mcsControllerType);
        g_mcsChangeDataset.SetStyle(Gnuplot2dDataset::POINTS);

        mcsChangesPlot.AddDataset(g_mcsChangeDataset);

        trialResultsPlot.SetOutputFilename(g_experimentDir + "/trial-results.png");
        trialResultsPlot.SetTitle("Trial Results");
        trialResultsPlot.SetTerminal("png");
        trialResultsPlot.SetLegend("Time (s)", "MCS");
        trialResultsPlot.AppendExtra("set yrange [0:30]");

        g_trialResultSuccessDataset.SetTitle("Successes");
        g_trialResultSuccessDataset.SetStyle(Gnuplot2dDataset::POINTS);
        g_trialResultSuccessDataset.SetExtra("lc rgb 'green'");

        g_trialResultFailureDataset.SetTitle("Failures");
        g_trialResultFailureDataset.SetStyle(Gnuplot2dDataset::POINTS);
        g_trialResultFailureDataset.SetExtra("lc rgb 'red'");

        trialResultsPlot.AddDataset(g_trialResultSuccessDataset);
        trialResultsPlot.AddDataset(g_trialResultFailureDataset);
    }

    Simulator::Stop(finalSimTime);
    Simulator::Run();

    if (mcsChanges.size() > 1)
    {
        std::cout << "Final MCS changed to " << +mcsChanges.back().mcs << std::endl;
    }
    else
    {
        std::cout << "No MCS changes after initialization" << std::endl;
    }
    NS_ASSERT_MSG(!pscchTransmissions.empty(), "Expected at least one SCI-1");
    const auto& [scheduledTime, mcs] = pscchTransmissions.back();
    std::cout << "Last MCS value signaled in SCI-1 was MCS " << +mcs << " at time "
              << scheduledTime.As(Time::S) << std::endl;

    std::cout << "Measurement phase Tx packets = " << txPktCounter << std::endl;
    std::cout << "Measurement phase Rx packets = " << rxPktCounter << std::endl;
    if (txPktCounter > 0)
    {
        std::cout << "Measurement phase loss rate = "
                  << 100.0 * (1.0 - static_cast<double>(rxPktCounter) / txPktCounter) << " %"
                  << std::endl;
    }

    std::cout << "Average throughput = " << (rxByteCounter * 8) / measurementDuration / 1000.0
              << " kbps" << std::endl;

    std::cout << "Average Packet Inter-Reception (PIR) "
              << (pir + pir2).GetSeconds() / (pirCounter + pirCounter2) << " sec" << std::endl;
    std::cout << "Min/max delay (us) " << g_delayMin.GetMilliSeconds() << " "
              << g_delayMax.GetMilliSeconds() << std::endl;

    Simulator::Destroy();
    if (g_writeTraces)
    {
        g_mcsChangeStream.flush();
        g_mcsChangeStream.close();
        g_rxPsschStream.flush();
        g_rxPsschStream.close();
        g_txPscchStream.flush();
        g_txPscchStream.close();
        g_txPsschStream.flush();
        g_txPsschStream.close();
        g_pktStream.flush();
        g_pktStream.close();
        g_trialResultStream.flush();
        g_trialResultStream.close();
    }

    g_distanceTraceStream.flush();
    g_distanceTraceStream.close();

    if (g_writePlts)
    {
        std::ofstream mcsChangesPlotFile(g_experimentDir + "/mcs-changes.plt");
        mcsChangesPlot.GenerateOutput(mcsChangesPlotFile);
        mcsChangesPlotFile.close();

        std::ofstream trialResultsPlotFile(g_experimentDir + "/trial-results.plt");
        trialResultsPlot.GenerateOutput(trialResultsPlotFile);
        trialResultsPlotFile.close();
    }

    return 0;
}

uint32_t
GetMinSubchannels(uint8_t mcs,
                  uint32_t transportBlockSize,
                  uint16_t maxSubchannels,
                  uint8_t mcsTable,
                  uint32_t subchannelSize,
                  uint32_t numPscchRbs,
                  uint32_t pscchSymLength)

{
    for (uint16_t nSubch = 1; nSubch <= maxSubchannels; nSubch++)
    {
        // These are currently hardcoded values in this implementation
        constexpr uint8_t RANK = 1;             // Sidelink Release 16 uses rank 1
        constexpr uint16_t N_DMRS_SYM_SLOT = 2; // NIST LLS assumes Type 1 with 2 symbols/slot

        // The number of subcarriers per physical resource block is a constant
        constexpr uint32_t N_SC_RB = 12; // Subcarriers per RB (TS 38.211 Section 4.4.4.1)

        // Compute N_RE' per PRB
        uint32_t nSymbPssch = 12; // 12 with no PSFCH, otherwise 9 with PSFCH
        uint32_t nReDmrs = 6 * N_DMRS_SYM_SLOT;
        uint32_t nRePrime = N_SC_RB * nSymbPssch - nReDmrs; // N_OH_PRB = 0 for FR1

        // Compute total N_RE (number of resource elements)
        // Per TS 38.214 Section 8.1.3.2:
        // N_RE = N_RE' * n_PRB - N_RE_SCI1 - N_RE_SCI2
        // where N_RE_SCI1 accounts for PSCCH REs (same frequency, different symbols)
        // and N_RE_SCI2 accounts for 2nd-stage SCI multiplexed with PSSCH
        uint32_t nPrb = nSubch * subchannelSize;
        uint32_t nReSci1 = pscchSymLength * numPscchRbs * 12;

        // Compute N_info
        uint8_t Qm = NrMcsTables::GetModulationOrder(mcs, mcsTable);
        double R = NrMcsTables::GetCodeRate(mcs, mcsTable);

        // N_RE_SCI2 depends on the data code rate per TS 38.212 Section 8.4.4
        constexpr uint32_t O_SCI2 = 35;            // SCI format 2-A payload size (bits)
        constexpr uint32_t L_SCI2 = 24;            // CRC length for 2nd-stage SCI
        constexpr double BETA_OFFSET_SCI2 = 1.125; // From sl-BetaOffsets2ndSCI index 1
        constexpr uint8_t Q_M_SCI2 = 2;            // Modulation order for SCI2 (QPSK)

        double part1 =
            std::ceil(static_cast<double>(O_SCI2 + L_SCI2) * BETA_OFFSET_SCI2 / (Q_M_SCI2 * R));

        // For typical configurations, part1 < part2, so min(part1, part2) = part1
        // gamma = 0 per NIST New Radio Sidelink LLS implementation (citing 3GPP R1-2007161)
        uint32_t nReSci2 = static_cast<uint32_t>(part1);
        uint32_t nRe = nRePrime * nPrb - nReSci1 - nReSci2;
        double nInfo = static_cast<double>(nRe) * Qm * R * RANK;

        NS_LOG_INFO("TBS calc: nSubch=" << nSubch << " nPrb=" << nPrb
                                        << " nSymbPssch=" << nSymbPssch << " nReDmrs=" << nReDmrs
                                        << " N_RE'=" << nRePrime << " N_RE_SCI1=" << nReSci1
                                        << " N_RE_SCI2=" << nReSci2 << " N_RE=" << nRe
                                        << " Qm=" << +Qm << " R=" << R << " N_info=" << nInfo);

        uint32_t tbsBits;
        // TS 38.214 Section 5.1.3.2 TBS determination
        if (nInfo <= 0)
        {
            tbsBits = 0;
        }
        else
        {
            uint32_t tbs{0};
            if (nInfo <= 3824)
            {
                // Step 3: Quantize N_info to N'_info
                uint32_t n = std::max(3u, static_cast<uint32_t>(std::floor(std::log2(nInfo))) - 6);
                uint32_t nInfoPrime =
                    std::max(24u,
                             static_cast<uint32_t>(std::pow(2, n)) *
                                 static_cast<uint32_t>(std::floor(nInfo / std::pow(2, n))));

                // TBS lookup table (TS 38.214 Table 5.1.3.2-1)
                // Simplified lookup: find closest TBS >= N'_info
                static constexpr std::array<uint32_t, 93> tbsTable = {
                    24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,
                    120,  128,  136,  144,  152,  160,  168,  176,  184,  192,  208,  224,
                    240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,
                    456,  480,  504,  528,  552,  576,  608,  640,  672,  704,  736,  768,
                    808,  848,  888,  928,  984,  1032, 1064, 1128, 1160, 1192, 1224, 1256,
                    1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928,
                    2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792,
                    2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

                auto it = std::lower_bound(tbsTable.begin(), tbsTable.end(), nInfoPrime);
                if (it != tbsTable.end())
                {
                    tbs = *it;
                }
                else
                {
                    tbs = tbsTable.back();
                }
                NS_LOG_INFO("TB size calc step 3: n=" << n << " N_info_prime=" << nInfoPrime
                                                      << " size (from table)=" << tbs);
            }
            else
            {
                // Step 4: For N_info > 3824
                uint32_t n = static_cast<uint32_t>(std::floor(std::log2(nInfo - 24))) - 5;
                uint32_t nInfoPrime =
                    std::max(3840u,
                             static_cast<uint32_t>(std::pow(2, n)) *
                                 static_cast<uint32_t>(std::round((nInfo - 24) / std::pow(2, n))));

                if (R <= 0.25)
                {
                    uint32_t C = std::ceil((nInfoPrime + 24) / 3816.0);
                    tbs = 8 * C * std::ceil((nInfoPrime + 24) / (8.0 * C)) - 24;
                }
                else
                {
                    if (nInfoPrime > 8424)
                    {
                        uint32_t C = std::ceil((nInfoPrime + 24) / 8424.0);
                        tbs = 8 * C * std::ceil((nInfoPrime + 24) / (8.0 * C)) - 24;
                    }
                    else
                    {
                        tbs = 8 * std::ceil((nInfoPrime + 24) / 8.0) - 24;
                    }
                }
                NS_LOG_INFO("TB size calc step 4: n=" << n << " N_info_prime=" << nInfoPrime
                                                      << " size (from formula)=" << tbs);
            }
            tbsBits = tbs;
        }

        NS_LOG_INFO("TBS result: tbsBits=" << tbsBits << " tbsBytes=" << tbsBits / 8
                                           << " needed=" << transportBlockSize);
        uint32_t tbs = tbsBits / 8;
        if (tbs >= transportBlockSize)
        {
            return nSubch;
        }
    }

    NS_LOG_WARN("Transport block size " << transportBlockSize << " cannot fit in max subchannels "
                                        << maxSubchannels);
    return maxSubchannels;
}

/**
 * Parse context strings such as "/NodeList/3/DeviceList/1/$ns3::NrUeNetDevice"
 * to extract the NodeId
 *
 * @param context context string
 */
uint32_t
ContextToNodeId(std::string context)
{
    std::string sub = context.substr(10); // skip "/NodeList/"
    uint32_t pos = sub.find("/Device");
    return atoi(sub.substr(0, pos).c_str());
}

/**
 * \brief Method to listen the packet sink application trace Rx.
 * @param packet The packet
 * @param from The address of the transmitting node
 * @param to The address of the receiving node
 * @param header The SeqTsSize header
 */
void
ReceivePacket(Ptr<const Packet> packet,
              const Address& from,
              const Address& to,
              const SeqTsSizeHeader& header)
{
    if (g_writeTraces)
    {
        g_pktStream << std::fixed << std::showpoint << std::setprecision(6)
                    << Simulator::Now().GetSeconds() << " " << "RX" << " "
                    << (packet->GetSize() + header.GetSerializedSize()) << std::endl;
    }

    if (g_counting)
    {
        rxByteCounter += packet->GetSize() + header.GetSerializedSize();
        rxPktCounter++;
    }
}

/**
 * \brief Method to listen the transmitting application trace Tx.
 * @param packet The packet
 * @param from The address of the transmitting node
 * @param to The address of the receiving node
 * @param header The SeqTsSize header
 */
void
TransmitPacket(Ptr<const Packet> packet,
               const Address& from,
               const Address& to,
               const SeqTsSizeHeader& header)
{
    if (g_verbose)
    {
        std::cout << Simulator::Now().GetSeconds() << " transmit "
                  << packet->GetSize() + header.GetSerializedSize() << " bytes" << std::endl;
    }

    if (g_writeTraces)
    {
        g_pktStream << std::fixed << std::showpoint << std::setprecision(6)
                    << Simulator::Now().GetSeconds() << " " << "TX" << " "
                    << (packet->GetSize() + header.GetSerializedSize()) << std::endl;
    }

    if (g_counting)
    {
        txByteCounter += packet->GetSize() + header.GetSerializedSize();
        txPktCounter++;
    }
}

/**
 * \brief This method listens to the packet sink application trace Rx.
 * @param packet The packet
 * @param from The address of the transmitter
 * @param to The address of the receiving node
 * @param header The SeqTsSize header
 */
void
ComputePir(Ptr<const Packet> packet,
           [[maybe_unused]] const Address& from,
           const Address& to,
           const SeqTsSizeHeader& header)
{
    if (pirCounter == 0 && lastPktRxTime.GetSeconds() == 0.0)
    {
        // this the first packet, just store the time and get out
        lastPktRxTime = Simulator::Now();
        pirCounter++;
        if (g_verbose)
        {
            std::cout << Simulator::Now().GetSeconds() << " receive" << std::endl;
        }
        return;
    }
    pir = pir + (Simulator::Now() - lastPktRxTime);
    if (g_verbose)
    {
        std::cout << Simulator::Now().GetSeconds() << " seq " << header.GetSeq()
                  << " receive ir(us) " << (Now() - lastPktRxTime).GetMicroSeconds() << std::endl;
    }
    lastPktRxTime = Simulator::Now();
    pirCounter++;
}

/**
 * \brief This method listens to the second packet sink application trace Rx.
 * @param packet The packet
 * @param from The address of the transmitter
 * @param to The address of the receiving node
 * @param header The SeqTsSize header
 */
void
ComputePir2(Ptr<const Packet> packet,
            [[maybe_unused]] const Address& from,
            const Address& to,
            const SeqTsSizeHeader& header)
{
    if (pirCounter2 == 0 && lastPktRxTime2.GetSeconds() == 0.0)
    {
        // this the first packet, just store the time and get out
        lastPktRxTime2 = Simulator::Now();
        pirCounter2++;
        if (g_verbose)
        {
            std::cout << Simulator::Now().GetSeconds() << " receive" << std::endl;
        }
        return;
    }
    pir2 = pir2 + (Simulator::Now() - lastPktRxTime2);
    if (g_verbose)
    {
        std::cout << Simulator::Now().GetSeconds() << " seq " << header.GetSeq()
                  << " receive ir(us) " << (Now() - lastPktRxTime2).GetMicroSeconds() << std::endl;
    }
    lastPktRxTime2 = Simulator::Now();
    pirCounter2++;
}

/**
 * Listen to TxRlcPdu events
 */
void
TraceTxRlcPduWithTxRnti(uint64_t imsi,
                        uint16_t rnti,
                        uint16_t txRnti,
                        uint8_t lcid,
                        uint32_t bytes,
                        double delay)
{
    if (g_verbose)
    {
        std::cout << Now().GetSeconds() << " TX" << std::endl;
    }
}

/**
 * Listen to RxRlcPdu events
 */
void
TraceRxRlcPduWithTxRnti(uint64_t imsi,
                        uint16_t rnti,
                        uint16_t txRnti,
                        uint8_t lcid,
                        uint32_t bytes,
                        double delay)
{
    Time d = Seconds(delay);
    if (g_verbose)
    {
        std::cout << Now().GetSeconds() << " RX bytes " << bytes << " delay " << d << std::endl;
    }
    if (d > g_delayMax)
    {
        g_delayMax = d;
    }
    if (d < g_delayMin)
    {
        g_delayMin = d;
    }
}

/**
 * \brief Method to listen the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * @param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchScheduling(const SlPscchUeMacStatParameters pscchStatsParams)
{
    if (g_verbose)
    {
        std::cout << Now().GetSeconds() << " PSCCH imsi " << pscchStatsParams.imsi << " slotNum "
                  << pscchStatsParams.slotNum << " symStart " << pscchStatsParams.symStart
                  << " symLen " << pscchStatsParams.symLength << " rbStart "
                  << pscchStatsParams.rbStart << " rbLen " << pscchStatsParams.rbLength
                  << std::endl;
    }

    if (g_verbose)
    {
        std::cout << "MCS announced in SCI-1: " << +pscchStatsParams.mcs << std::endl;
    }

    pscchTransmissions.push_back(std::make_pair(Now(), pscchStatsParams.mcs));

    if (g_writeTraces)
    {
        g_txPscchStream << std::fixed << std::showpoint << std::setprecision(6)
                        << Simulator::Now().GetSeconds() << " " << pscchStatsParams.imsi << " "
                        << +pscchStatsParams.mcs << std::endl;
    }
}

/**
 * \brief Method to listen the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * @param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschScheduling(const SlPsschUeMacStatParameters psschStatsParams)
{
    if (g_verbose)
    {
        std::cout << Now().GetSeconds() << " PSSCH imsi " << psschStatsParams.imsi << " slotNum "
                  << psschStatsParams.slotNum << " symStart " << psschStatsParams.symStart
                  << " symLen " << psschStatsParams.symLength << " rbStart "
                  << psschStatsParams.rbStart << " rbLen " << psschStatsParams.rbLength
                  << " harqId " << +psschStatsParams.harqId << " rv " << +psschStatsParams.rv
                  << std::endl;
    }

    if (g_writeTraces)
    {
        g_txPsschStream << std::fixed << std::showpoint << std::setprecision(6)
                        << Simulator::Now().GetSeconds() << " " << psschStatsParams.imsi << " "
                        << psschStatsParams.slotNum << " " << +psschStatsParams.harqId << " "
                        << +psschStatsParams.rv << " " << psschStatsParams.rbStart << " "
                        << psschStatsParams.rbLength << std::endl;
    }
}

/**
 * \brief Method to listen the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * @param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchRx(const SlRxCtrlPacketTraceParams pscchStatsParams)
{
}

/**
 * \brief Method to listen the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * @param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschRx(const SlRxDataPacketTraceParams psschStatsParams)
{
    if (g_writeTraces)
    {
        g_rxPsschStream << std::fixed << std::showpoint << std::setprecision(6)
                        << Simulator::Now().GetSeconds() << " " << psschStatsParams.m_dstL2Id << " "
                        << +psschStatsParams.m_mcs << " " << psschStatsParams.m_sinr << " "
                        << psschStatsParams.m_tbler << std::endl;
    }
}

/**
 * \brief Listen to TxFeedbackTrace events.
 *
 * @param context The trace context string
 * @param duration Duration of the transmission
 */
void
NotifyTxFeedback(std::string context, Time duration)
{
    if (g_verbose)
    {
        std::cout << std::fixed << std::showpoint << std::setprecision(9)
                  << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
                  << " tx feedback duration " << duration.GetNanoSeconds() << "ns" << std::endl;
    }
}

void
NotifyRxHarqFeedback(std::string context, const SlHarqInfo& harqInfo)
{
    if (g_verbose)
    {
        std::cout << std::fixed << std::showpoint << std::setprecision(9)
                  << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
                  << " rx harq; rnti " << harqInfo.m_rnti << " process ID "
                  << +harqInfo.m_harqProcessId << " bwpIndex " << +harqInfo.m_bwpIndex << std::endl;
    }
}

void
NotifyAllocate(std::string context,
               uint8_t harqId,
               uint32_t dstL2Id,
               bool multiplePdu,
               Time timeout,
               std::size_t available)
{
    if (g_verbose)
    {
        std::cout << std::fixed << std::showpoint << std::setprecision(9)
                  << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
                  << " allocate; processId " << +harqId << " dstL2Id " << dstL2Id
                  << " multiple PDU " << multiplePdu << " timeout " << timeout.GetMilliSeconds()
                  << "ms"
                  << " available " << available << std::endl;
    }
}

void
NotifyDeallocate(std::string context, uint8_t harqId, std::size_t available)
{
    if (g_verbose)
    {
        std::cout << std::fixed << std::showpoint << std::setprecision(9)
                  << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
                  << " deallocate; processId " << +harqId << " available " << available
                  << std::endl;
    }
}

void
McsChangeCb(uint32_t dstL2Id, uint8_t oldMcs, uint8_t newMcs)
{
    if (oldMcs != newMcs)
    {
        if (g_verbose)
        {
            std::cout << "MCS change from " << +oldMcs << " to " << +newMcs
                      << " towards destination " << dstL2Id << std::endl;
        }

        McsChange change;
        change.ts = Simulator::Now();
        change.dstL2Id = dstL2Id;
        change.mcs = newMcs;

        mcsChanges.push_back(change);
    }
}

void
CaptureMcsChanges()
{
    if (!mcsChanges.empty())
    {
        if (Simulator::Now() - mcsChanges.back().ts > Seconds(1))
        {
            if (g_writeTraces)
            {
                g_mcsChangeStream << std::fixed << std::showpoint << std::setprecision(9)
                                  << Simulator::Now().GetSeconds() << " "
                                  << mcsChanges.back().dstL2Id << " " << +mcsChanges.back().mcs
                                  << std::endl;
            }

            if (g_writePlts)
            {
                g_mcsChangeDataset.Add(Simulator::Now().GetSeconds(), +mcsChanges.back().mcs);
            }
        }
        else
        {
            std::vector<McsChange> reverse;
            for (auto it = mcsChanges.rbegin(); it != mcsChanges.rend(); it++)
            {
                if (Simulator::Now() - it->ts <= Seconds(1))
                {
                    reverse.push_back(*it);
                }
            }

            for (auto it = reverse.rbegin(); it != reverse.rend(); it++)
            {
                if (g_writeTraces)
                {
                    g_mcsChangeStream << std::fixed << std::showpoint << std::setprecision(9)
                                      << it->ts.GetSeconds() << " " << it->dstL2Id << " "
                                      << +it->mcs << std::endl;
                }

                if (g_writePlts)
                {
                    g_mcsChangeDataset.Add(it->ts.GetSeconds(), +it->mcs);
                }
            }
        }
    }

    Simulator::Schedule(Seconds(1), &CaptureMcsChanges);
}

void
TrialResult(uint8_t mcs, uint32_t dstL2Id, bool success)
{
    if (g_verbose)
    {
        std::cout << "Trial Result:"
                  << " mcs " << +mcs << " dstL2Id " << dstL2Id << " success " << +success
                  << std::endl;
    }
    if (g_writeTraces)
    {
        g_trialResultStream << std::fixed << std::showpoint << std::setprecision(9)
                            << Simulator::Now().GetSeconds() << " " << dstL2Id << " " << +mcs << " "
                            << +success << std::endl;
    }

    if (g_writePlts)
    {
        if (success)
        {
            g_trialResultSuccessDataset.Add(Simulator::Now().GetSeconds(), +mcs);
        }
        else
        {
            g_trialResultFailureDataset.Add(Simulator::Now().GetSeconds(), +mcs);
        }
    }
}

void
LogDistance()
{
    auto posA = g_nodeA->GetObject<MobilityModel>()->GetPosition();
    auto posB = g_nodeB->GetObject<MobilityModel>()->GetPosition();
    double dist = CalculateDistance(posA, posB);
    g_distanceTraceStream << std::fixed << std::setprecision(6) << Simulator::Now().GetSeconds()
                          << " " << std::setprecision(3) << dist << std::endl;
    Simulator::Schedule(Seconds(1), &LogDistance);
}

void
StartCounting()
{
    NS_LOG_INFO("Measurement phase begins at " << Now().GetSeconds() << " s");
    g_counting = true;
}
