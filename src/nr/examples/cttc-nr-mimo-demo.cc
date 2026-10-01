// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \ingroup examples
 * \file cttc-nr-mimo-demo.cc
 * \brief An example that shows how to setup and use MIMO
 *
 * This example describes how to setup a simulation using MIMO. The scenario
 * consists of a simple topology, in which there
 * is only one gNB and one UE. An additional pair of gNB and UE can be enabled
 * to simulate the interference (see enableInterfNode).
 * Example creates one DL flow that goes through only BWP.
 *
 * The example prints on-screen and into the file the end-to-end result of the flow.
 * To see all the input parameters run:
 *
 * \code{.unparsed}
$ ./ns3 run cttc-nr-mimo-demo -- --PrintHelp
    \endcode
 *
 *
 * MIMO is enabled by default. To disable it run:
 *  * \code{.unparsed}
$  ./ns3 run cttc-nr-mimo-demo -- --enableMimoFeedback=0
    \endcode
 *
 *
 */

#include "mimo-sim-helpers/cttc-mimo-simple-db-helper.h"

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/basic-data-calculators.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/stats-module.h"
#include "ns3/traffic-generator-helper.h"

#include <map>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("CttcNrMimoDemo");

struct CqiFeedbackTraceStats
{
    Ptr<MinMaxAvgTotalCalculator<uint8_t>> m_ri;
    Ptr<MinMaxAvgTotalCalculator<uint8_t>> m_mcs;

    CqiFeedbackTraceStats()
    {
        m_ri = CreateObject<MinMaxAvgTotalCalculator<uint8_t>>();
        m_mcs = CreateObject<MinMaxAvgTotalCalculator<uint8_t>>();
    }

    CqiFeedbackTraceStats(uint8_t rank, uint8_t mcs)
    {
        m_ri = CreateObject<MinMaxAvgTotalCalculator<uint8_t>>();
        m_ri->Update(rank);
        m_mcs = CreateObject<MinMaxAvgTotalCalculator<uint8_t>>();
        m_mcs->Update(mcs);
    }
};

void
CqiFeedbackTracedCallback(std::map<uint16_t, CqiFeedbackTraceStats>* stats,
                          uint16_t rnti,
                          [[maybe_unused]] uint8_t cqi,
                          uint8_t mcs,
                          uint8_t rank)
{
    auto it = stats->find(rnti);
    if (it != stats->end())
    {
        it->second.m_ri->Update(rank);
        it->second.m_mcs->Update(mcs);
    }
    else
    {
        (*stats)[rnti] = CqiFeedbackTraceStats(rank, mcs);
    }
}

namespace ns3
{
class FastFadingTestingMobilityModel : public ConstantPositionMobilityModel
{
  public:
    static TypeId GetTypeId();
    Vector m_fakeVelocity{0.0, 0.0, 0.0};

  private:
    Vector DoGetVelocity() const override;
};

NS_OBJECT_ENSURE_REGISTERED(FastFadingTestingMobilityModel);

TypeId
FastFadingTestingMobilityModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::FastFadingTestingMobilityModel")
            .SetParent<ConstantPositionMobilityModel>()
            .SetGroupName("Mobility")
            .AddConstructor<FastFadingTestingMobilityModel>()
            .AddAttribute("FakeVelocity",
                          "The current velocity of the mobility model.",
                          VectorValue(Vector(0.0, 0.0, 0.0)), // ignored initial value.
                          MakeVectorAccessor(&FastFadingTestingMobilityModel::m_fakeVelocity),
                          MakeVectorChecker());
    return tid;
}

Vector
FastFadingTestingMobilityModel::DoGetVelocity() const
{
    return m_fakeVelocity;
}
} // namespace ns3

int
main(int argc, char* argv[])
{
    auto startExecTime = std::chrono::system_clock::now();
    bool enableMimoFeedback = true;
    bool useConfigSetDefault = false;
    uint8_t csiFlags = 1;

    NrHelper::AntennaParams apUe;
    NrHelper::AntennaParams apGnb;
    apUe.antennaElem = "ns3::ThreeGppAntennaModel";
    apUe.nAntCols = 2;
    apUe.nAntRows = 2;
    apUe.nHorizPorts = 2;
    apUe.nVertPorts = 1;
    apUe.isDualPolarized = false;
    apGnb.antennaElem = "ns3::ThreeGppAntennaModel";
    apGnb.nAntCols = 4;
    apGnb.nAntRows = 2;
    apGnb.nHorizPorts = 2;
    apGnb.nVertPorts = 1;
    apGnb.isDualPolarized = false;
    double downtiltAngleGnb = 10;

    // The polarization slant angle in degrees in case of x-polarized
    double polSlantAngleGnb = 0.0;
    double polSlantAngleUe = 90.0;
    // The bearing angles in degrees
    double bearingAngleGnb = 0.0;
    double bearingAngleUe = 180.0;

    std::string trafficType = "cbr";
    // Traffic parameters
    uint32_t udpPacketSize = 1000;
    // Packet interval is 40000 ns to reach 200 Mbps
    // For MCS Table 2, and 10 MHz BW, 200 Mbps can be achieved by using 4 MIMO streams
    Time packetInterval = MilliSeconds(30);
    Time udpAppStartTime = MilliSeconds(400);

    // Interference
    bool enableInterfNode = false; // if true an additional pair of gNB and UE will be created to
                                   // create an interference towards the original pair
    double interfDistance =
        1000.0; // the distance in meters between the gNB1 and the interfering gNB2
    double interfPolSlantDelta = 0; // the difference between the pol. slant angle between the
                                    // original node and the interfering one

    // Other simulation scenario parameters
    Time simTime = MilliSeconds(1000);
    uint16_t gnbUeDistance = 20; // meters
    uint16_t numerology = 0;
    double centralFrequency = 3.5e9;
    double bandwidth = 10e6;
    double txPowerGnb = 23; // dBm
    double txPowerUe = 23;  // dBm
    uint16_t updatePeriodMs = 0;
    std::string errorModel = "ns3::NrEesmIrT2";
    std::string scheduler = "ns3::NrMacSchedulerTdmaRR";
    std::string beamformingMethod = "ns3::DirectPathBeamforming";

    uint32_t wbPmiUpdateIntervalMs = 10; // Wideband PMI update interval in ms
    uint32_t sbPmiUpdateIntervalMs = 2;  // Subband PMI update interval in ms

    // Default channel condition
    std::string losCondition = "Default";
    NrHelper::MimoPmiParams mimoPmiParams;
    mimoPmiParams.subbandSize = 8;
    double xyVelocity = 0;

    // Where the example stores the output files.
    std::string simTag = "default";
    std::string outputDir = "./";
    bool logging = false;

    CommandLine cmd(__FILE__);
    /**
     * The main parameters for testing MIMO
     */
    cmd.AddValue("enableMimoFeedback", "Enables MIMO feedback", enableMimoFeedback);
    cmd.AddValue(
        "pmSearchMethod",
        "Precoding matrix search method, currently implemented only exhaustive search method"
        "(ns3::NrPmSearchFull)",
        mimoPmiParams.pmSearchMethod);
    cmd.AddValue("fullSearchCb",
                 "The codebook to be used for the full search. Available codebooks are "
                 "a) ns3::NrCbTwoPort, the two-port codebook defined in 3GPP TS 38.214 Table "
                 "5.2.2.2.1-1, and"
                 "b) ns3::NrCbTypeOneSp, Type-I Single-Panel Codebook 3GPP TS 38.214 Rel. 15, "
                 "Sec. 5.2.2.2.1 supporting codebook mode 1 only, and limited to rank 4.",
                 mimoPmiParams.fullSearchCb);
    cmd.AddValue("rankLimit", "The maximum rank number to be used.", mimoPmiParams.rankLimit);
    cmd.AddValue("subbandSize", "Sub-band size for downsampling", mimoPmiParams.subbandSize);
    cmd.AddValue("downsamplingTechnique",
                 "Sub-band downsampling technique",
                 mimoPmiParams.downsamplingTechnique);
    cmd.AddValue("numRowsGnb", "Number of antenna rows at the gNB", apGnb.nAntRows);
    cmd.AddValue("numRowsUe", "Number of antenna rows at the UE", apUe.nAntRows);
    cmd.AddValue("numColumnsGnb", "Number of antenna columns at the gNB", apGnb.nAntCols);
    cmd.AddValue("numColumnsUe", "Number of antenna columns at the UE", apUe.nAntCols);
    cmd.AddValue("numVPortsGnb",
                 "Number of vertical ports of the antenna at the gNB",
                 apGnb.nVertPorts);
    cmd.AddValue("numVPortsUe",
                 "Number of vertical ports of the antenna at the UE",
                 apUe.nVertPorts);
    cmd.AddValue("numHPortsGnb",
                 "Number of horizontal ports of the antenna the gNB",
                 apGnb.nHorizPorts);
    cmd.AddValue("numHPortsUe",
                 "Number of horizontal ports of the antenna at the UE",
                 apUe.nHorizPorts);
    cmd.AddValue("xPolGnb",
                 "Whether the gNB antenna array has the cross polarized antenna "
                 "elements.",
                 apGnb.isDualPolarized);
    cmd.AddValue("xPolUe",
                 "Whether the UE antenna array has the cross polarized antenna "
                 "elements.",
                 apUe.isDualPolarized);
    cmd.AddValue("polSlantAngleGnb",
                 "Polarization slant angle of gNB in degrees",
                 polSlantAngleGnb);
    cmd.AddValue("polSlantAngleUe", "Polarization slant angle of UE in degrees", polSlantAngleUe);
    cmd.AddValue("bearingAngleGnb", "Bearing angle of gNB in degrees", bearingAngleGnb);
    cmd.AddValue("bearingAngleUe", "Bearing angle of UE in degrees", bearingAngleUe);
    cmd.AddValue("downtiltAngleGnb", "Downtilt angle of gNB in degrees", downtiltAngleGnb);
    cmd.AddValue("enableInterfNode", "Whether to enable an interfering node", enableInterfNode);
    cmd.AddValue("wbPmiUpdateInterval",
                 "Wideband PMI update interval in ms",
                 wbPmiUpdateIntervalMs);
    cmd.AddValue("sbPmiUpdateInterval", "Subband PMI update interval in ms", sbPmiUpdateIntervalMs);
    cmd.AddValue("interfDistance",
                 "The distance between the gNB1 and the interfering gNB2 (the original and the "
                 "interfering one)",
                 interfDistance);
    cmd.AddValue("interfPolSlantDelta",
                 "The difference between the pol. slant angles of the original pairs of gNB and UE "
                 "and the interfering one",
                 interfPolSlantDelta);
    cmd.AddValue("csiFlags", "CsiFlags to be configured. See NrHelper::CsiFlags", csiFlags);
    /**
     * Other simulation parameters
     */
    cmd.AddValue("trafficType",
                 "Traffic type to be installed at the source: cbr or ftp.",
                 trafficType);
    cmd.AddValue("packetSize",
                 "packet size in bytes to be used by best effort traffic",
                 udpPacketSize);
    cmd.AddValue("packetInterval", "Inter-packet interval for CBR traffic", packetInterval);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("numerology", "The numerology to be used", numerology);
    cmd.AddValue("centralFrequency", "The system frequency to be used in band 1", centralFrequency);
    cmd.AddValue("bandwidth", "The system bandwidth to be used", bandwidth);
    cmd.AddValue("txPowerGnb", "gNB TX power", txPowerGnb);
    cmd.AddValue("txPowerUe", "UE TX power", txPowerUe);
    cmd.AddValue("gnbUeDistance",
                 "The distance between the gNB and the UE in the scenario",
                 gnbUeDistance);
    cmd.AddValue(
        "updatePeriodMs",
        "Channel update period in ms. If set to 0 then the channel update will be disabled",
        updatePeriodMs);
    cmd.AddValue("errorModel",
                 "Error model: ns3::NrEesmCcT1, ns3::NrEesmCcT2, "
                 "ns3::NrEesmIrT1, ns3::NrEesmIrT2, ns3::NrLteMiErrorModel",
                 errorModel);
    cmd.AddValue("scheduler",
                 "The scheduler: ns3::NrMacSchedulerTdmaRR, "
                 "ns3::NrMacSchedulerTdmaPF, ns3::NrMacSchedulerTdmaMR,"
                 "ns3::NrMacSchedulerTdmaQos, ns3::NrMacSchedulerOfdmaRR, "
                 "ns3::NrMacSchedulerOfdmaPF, ns3::NrMacSchedulerOfdmaMR,"
                 "ns3::NrMacSchedulerOfdmaQos",
                 scheduler);
    cmd.AddValue("beamformingMethod",
                 "The beamforming method: ns3::CellScanBeamforming,"
                 "ns3::CellScanBeamformingAzimuthZenith,"
                 "ns3::CellScanQuasiOmniBeamforming,"
                 "ns3::DirectPathBeamforming,"
                 "ns3::QuasiOmniDirectPathBeamforming,"
                 "ns3::DirectPathQuasiOmniBeamforming,"
                 "ns3::KronBeamforming,"
                 "ns3::KronQuasiOmniBeamforming",
                 beamformingMethod);
    cmd.AddValue("losCondition",
                 "Default - for 3GPP channel condition model,"
                 "LOS - for always LOS channel condition model,"
                 "NLOS - for always NLOS channel condition model",
                 losCondition);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.AddValue("useConfigSetDefault",
                 "Configure via Config::SetDefault instead of the MimoPmiParams structure",
                 useConfigSetDefault);
    cmd.AddValue("xyVelocity",
                 "Velocity in X and Y directions m/s for fake fading model.",
                 xyVelocity);
    // Parse the command line
    cmd.Parse(argc, argv);

    // convert angle values into radians
    apUe.bearingAngle = bearingAngleUe * (M_PI / 180);
    apUe.polSlantAngle = polSlantAngleUe * (M_PI / 180);
    apGnb.bearingAngle = bearingAngleGnb * (M_PI / 180);
    apGnb.polSlantAngle = polSlantAngleGnb * (M_PI / 180);

    NS_ABORT_IF(centralFrequency < 0.5e9 && centralFrequency > 100e9);

    if (logging)
    {
        LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
        LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
        LogComponentEnable("NrPdcp", LOG_LEVEL_INFO);
    }

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod",
                       TimeValue(MilliSeconds(updatePeriodMs)));

    uint16_t pairsToCreate = 1;
    if (enableInterfNode)
    {
        pairsToCreate = 2;
    }

    NodeContainer gnbContainer;
    gnbContainer.Create(pairsToCreate);
    NodeContainer ueContainer;
    ueContainer.Create(pairsToCreate);

    /**
     * We configure the mobility model to ConstantPositionMobilityModel.
     * The default topology is the following:
     *
     * gNB1.................UE1................UE2........................gNB2(interferer)
     *(0.0, 0.0, 25.0)  (d, 0.0, 1.5)    (interfDistance/2, 0.0, 1.5)    (interfDistance,0.0, 25.0)
     * bearingAngle=0   bearingAngle=180 bearingAngle=0                   bearingAngle=180
     */
    MobilityHelper gnbMobility;
    gnbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> gnbPositionAlloc = CreateObject<ListPositionAllocator>();
    gnbPositionAlloc->Add(Vector(0.0, 0.0, 25.0));

    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::FastFadingTestingMobilityModel",
                                "FakeVelocity",
                                VectorValue(Vector{xyVelocity, xyVelocity, 0}));
    Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
    uePositionAlloc->Add(Vector(gnbUeDistance, 0.0, 1.5));
    // the positions for the second interfering pair of gNB and UE
    if (enableInterfNode)
    {
        gnbPositionAlloc->Add(Vector(interfDistance / 2, 0.0, 25.0)); // gNB2 position
        uePositionAlloc->Add(Vector(interfDistance, 0.0, 1.5));       // UE2 position
    }
    gnbMobility.SetPositionAllocator(gnbPositionAlloc);
    ueMobility.SetPositionAllocator(uePositionAlloc);

    gnbMobility.Install(gnbContainer.Get(0));
    ueMobility.Install(ueContainer.Get(0));
    // install mobility of the second pair of gNB and UE
    if (enableInterfNode)
    {
        gnbMobility.Install(gnbContainer.Get(1));
        ueMobility.Install(ueContainer.Get(1));
    }

    /**
     * Create the NR helpers that will be used to create and setup NR devices, spectrum, ...
     */
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    /**
     * Prepare spectrum. Prepare one operational band, containing
     * one component carrier, and a single bandwidth part
     * centered at the frequency specified by the input parameters.
     *
     *
     * The configured spectrum division is:
     * ------------Band--------------
     * ------------CC1----------------
     * ------------BWP1---------------
     */

    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1;
    CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, numCcPerBand);
    OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    // Create the channel helper
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    // Set the channel using the scenario and user input
    channelHelper->ConfigureFactories("UMa", losCondition, "ThreeGpp");
    // Set the channel update period and shadowing
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod",
                                                     TimeValue(MilliSeconds(updatePeriodMs)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    // Create and set the channel with the band
    channelHelper->AssignChannelsToBands({band});

    // Configure NrHelper, prepare most of the parameters that will be used in the simulation.
    nrHelper->SetAttribute("CsiFeedbackFlags", UintegerValue(csiFlags));
    nrHelper->SetDlErrorModel(errorModel);
    nrHelper->SetUlErrorModel(errorModel);
    nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
    nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName(scheduler));
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(TypeId::LookupByName(beamformingMethod)));
    // Core latency
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // We can configure via Config::SetDefault
    if (enableMimoFeedback)
    {
        // We can configure not only via Config::SetDefault, but also via the MimoPmiParams
        // structure
        if (useConfigSetDefault)
        {
            Config::SetDefault("ns3::NrHelper::EnableMimoFeedback", BooleanValue(true));
            Config::SetDefault("ns3::NrPmSearch::SubbandSize", UintegerValue(16));
        }
        else
        {
            nrHelper->SetupMimoPmi(mimoPmiParams);
        }
    }

    /**
     * Configure gNb antenna
     */
    nrHelper->SetupGnbAntennas(apGnb);
    // TODO consider adding DowntiltAngle to AntennaParams
    nrHelper->SetGnbAntennaAttribute("DowntiltAngle", DoubleValue(downtiltAngleGnb * M_PI / 180.0));
    /**
     * Configure UE antenna
     */
    nrHelper->SetupUeAntennas(apUe);

    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(numerology));
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(txPowerGnb));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPowerUe));
    nrHelper->SetUePhyAttribute("WbPmiUpdateInterval",
                                TimeValue(MilliSeconds(wbPmiUpdateIntervalMs)));
    nrHelper->SetUePhyAttribute("SbPmiUpdateInterval",
                                TimeValue(MilliSeconds(sbPmiUpdateIntervalMs)));

    uint32_t bwpId = 0;
    // gNb routing between bearer type and bandwidth part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpId));
    // UE routing between bearer type and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpId));
    BandwidthPartInfoPtrVector allBwps;
    allBwps = CcBwpCreator::GetAllBwps({band});

    /**
     * Finally, create the gNB and the UE device.
     */
    NetDeviceContainer gnbNetDev = nrHelper->InstallGnbDevice(gnbContainer, allBwps);
    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueContainer, allBwps);

    if (enableInterfNode)
    {
        nrHelper->GetGnbPhy(gnbNetDev.Get(1), 0)
            ->GetSpectrumPhy()
            ->GetAntenna()
            ->SetAttribute("BearingAngle", DoubleValue(0));
        nrHelper->GetUePhy(ueNetDev.Get(1), 0)
            ->GetSpectrumPhy()
            ->GetAntenna()
            ->SetAttribute("BearingAngle", DoubleValue(M_PI));
        if (interfPolSlantDelta)
        {
            // reconfigure the polarization slant angle of the interferer
            nrHelper->GetGnbPhy(gnbNetDev.Get(1), 0)
                ->GetSpectrumPhy()
                ->GetAntenna()
                ->SetAttribute(
                    "PolSlantAngle",
                    DoubleValue((polSlantAngleGnb + interfPolSlantDelta) * (M_PI / 180)));
            nrHelper->GetUePhy(ueNetDev.Get(1), 0)
                ->GetSpectrumPhy()
                ->GetAntenna()
                ->SetAttribute("PolSlantAngle",
                               DoubleValue((polSlantAngleUe + interfPolSlantDelta) * (M_PI / 180)));
        }
    }

    /**
     * Fix the random stream throughout the nr, propagation, and spectrum
     * modules classes. This configuration is extremely important for the
     * reproducibility of the results.
     */
    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    std::map<uint16_t, CqiFeedbackTraceStats> cqiTraces;
    for (auto it = ueNetDev.Begin(); it != ueNetDev.End(); ++it)
    {
        auto cqiCb = MakeBoundCallback(&CqiFeedbackTracedCallback, &cqiTraces);
        nrHelper->GetUePhy(*it, 0)->TraceConnectWithoutContext("CqiFeedbackTrace", cqiCb);
    }

    // create the Internet and install the IP stack on the UEs
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
    internet.Install(ueContainer);
    Ipv4InterfaceContainer ueIpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));
    // Set the default gateway for the UE
    Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(ueContainer.Get(0)->GetObject<Ipv4>());
    ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);

    // attach each UE to its gNB according to desired scenario
    nrHelper->AttachToGnb(ueNetDev.Get(0), gnbNetDev.Get(0));
    if (enableInterfNode)
    {
        nrHelper->AttachToGnb(ueNetDev.Get(1), gnbNetDev.Get(1));
    }

    /**
     * Install DL traffic part.
     */
    uint16_t dlPort = 1234;
    ApplicationContainer serverApps;
    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSink(dlPort);
    // The server, that is the application which is listening, is installed in the UE
    serverApps.Add(dlPacketSink.Install(ueContainer));

    // The bearer that will carry the traffic
    NrEpsBearer epsBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);

    // The filter for the traffic
    Ptr<NrEpcTft> dlTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlPktFilter;
    dlPktFilter.localPortStart = dlPort;
    dlPktFilter.localPortEnd = dlPort;
    dlTft->Add(dlPktFilter);

    /**
     * Let's install the applications!
     */
    ApplicationContainer clientApps;
    if (trafficType == "cbr")
    {
        UdpClientHelper dlClient;
        /**
         * Configure attributes for the CBR traffic generator, using user-provided
         * parameters
         */
        dlClient.SetAttribute("RemotePort", UintegerValue(dlPort));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        dlClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
        dlClient.SetAttribute("Interval", TimeValue(packetInterval));
        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClient.SetAttribute("RemoteAddress", AddressValue(ueIpIface.GetAddress(0)));
        clientApps.Add(dlClient.Install(remoteHost));
        // Activate a dedicated bearer for the traffic
        nrHelper->ActivateDedicatedEpsBearer(ueNetDev.Get(0), epsBearer, dlTft);
    }
    else if (trafficType == "ftp")
    {
        // configure FTP clients with file transfer application that generates multiple file
        // transfers
        TrafficGeneratorHelper ftpHelper =
            TrafficGeneratorHelper("ns3::UdpSocketFactory",
                                   Address(),
                                   TrafficGeneratorNgmnFtpMulti::GetTypeId());
        ftpHelper.SetAttribute("PacketSize", UintegerValue(512));
        ftpHelper.SetAttribute("MaxFileSize", UintegerValue(5e6));
        ftpHelper.SetAttribute("FileSizeMu", DoubleValue(14.45));

        ftpHelper.SetAttribute("Remote",
                               AddressValue(InetSocketAddress(ueIpIface.GetAddress(0, 0), dlPort)));
        clientApps.Add(ftpHelper.Install(remoteHost));
        // Activate a dedicated bearer for the traffic
        nrHelper->ActivateDedicatedEpsBearer(ueNetDev.Get(0), epsBearer, dlTft);
    }

    if (enableInterfNode)
    {
        UdpClientHelper dlClient;
        /**
         * Configure attributes for the CBR traffic generator, using user-provided
         * parameters
         */
        dlClient.SetAttribute("RemotePort", UintegerValue(dlPort));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        dlClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
        dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        dlClient.SetAttribute("RemoteAddress", AddressValue(ueIpIface.GetAddress(1)));
        clientApps.Add(dlClient.Install(remoteHost));
        // Activate a dedicated bearer for the traffic
        nrHelper->ActivateDedicatedEpsBearer(ueNetDev.Get(1), epsBearer, dlTft);
    }

    // start UDP server and client apps
    serverApps.Start(udpAppStartTime);
    clientApps.Start(udpAppStartTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);

    // enable the traces provided by the nr module
    nrHelper->EnableTraces();

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(ueContainer);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(simTime);
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
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    CttcMimoSimpleDbHelper dbHelper;
    dbHelper.SetResultsDirPath(outputDir);
    dbHelper.SetDbName("MimoSimple.db");
    dbHelper.PrepareTable();

    CttcMimoSimpleResults dbResults;
    // set the parameters
    dbResults.simTime = simTime.GetSeconds();
    dbResults.enableMimoFeedback = enableMimoFeedback;
    dbResults.gnbUeDistance = gnbUeDistance;
    dbResults.rngRun = SeedManager::GetRun();
    dbResults.pmSearchMethod = mimoPmiParams.pmSearchMethod;
    dbResults.fullSearchCb = mimoPmiParams.fullSearchCb;
    dbResults.rankLimit = mimoPmiParams.rankLimit;
    // gnb antenna params
    dbResults.numRowsGnb = apGnb.nAntRows;
    dbResults.numColumnsGnb = apGnb.nAntCols;
    dbResults.numVPortsGnb = apGnb.nVertPorts;
    dbResults.numHPortsGnb = apGnb.nHorizPorts;
    dbResults.isXPolGnb = apGnb.isDualPolarized;
    // ue antenna params
    dbResults.numRowsUe = apUe.nAntRows;
    dbResults.numColumnsUe = apUe.nAntCols;
    dbResults.numVPortsUe = apUe.nVertPorts;
    dbResults.numHPortsUe = apUe.nHorizPorts;
    dbResults.isXPolUe = apUe.isDualPolarized;
    dbResults.schedulerType = scheduler;
    dbResults.sbPmiUpdateIntervalMs = sbPmiUpdateIntervalMs;
    dbResults.wbPmiUpdateIntervalMs = wbPmiUpdateIntervalMs;
    dbResults.enableInterfNode = enableInterfNode;
    dbResults.csiFlags = csiFlags;
    dbResults.trafficType = trafficType;
    dbResults.xyVelocity = xyVelocity;

    // calculate the execution time
    auto endExecTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endExecTime - startExecTime;
    dbResults.execTimeSec = elapsed_seconds.count();

    double averageMcsForAllUes = 0.0;
    double averageRiForAllUes = 0.0;
    for (const auto& ue : cqiTraces)
    {
        averageRiForAllUes += ue.second.m_ri->getMean();
        averageMcsForAllUes += ue.second.m_mcs->getMean();
    }

    if (ueNetDev.GetN() != cqiTraces.size())
    {
        NS_LOG_WARN("Not all UEs have generated CQI feedback.");
    }

    if (!cqiTraces.empty())
    {
        dbResults.rank = averageRiForAllUes / cqiTraces.size();
        dbResults.mcs = averageMcsForAllUes / cqiTraces.size();
    }
    else
    {
        dbResults.rank = 1;
        dbResults.mcs = 0;
    }

    double flowDuration = (simTime - udpAppStartTime).GetSeconds();
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
        outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / flowDuration / 1000.0 / 1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            averageFlowThroughput += i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            double thr = i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            double delay = 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;
            double jitter = 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets;
            double packetLoss = 1 - ((double)(i->second.rxPackets) / (double)i->second.txPackets);

            outFile << "  Throughput: " << thr << " Mbps\n";
            outFile << "  Mean delay:  " << delay << " ms\n";
            outFile << "  Mean jitter:  " << jitter << " ms\n";

            // we want to save to the database only the flow stats from the first flow
            // from the first gNB-UE pair
            if (i == stats.begin())
            {
                dbResults.throughputMbps = thr;
                dbResults.delayMs = delay;
                dbResults.jitterMs = jitter;
                dbResults.bytesReceived = i->second.rxBytes;
                dbResults.bytesTransmitted = i->second.txBytes;
                dbResults.packetLoss = packetLoss;
            }
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";

        dbHelper.InsertResults(dbResults);
    }

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / stats.size() << "\n";
    outFile << " Mean rank: " << dbResults.rank << "\n";
    outFile << " Mean MCS: " << dbResults.mcs << "\n";

    outFile.close();
    std::ifstream f(filename.c_str());
    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();
    return 0;
}
