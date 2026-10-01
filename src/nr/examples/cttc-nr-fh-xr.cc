// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/boolean.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/xr-traffic-mixer-helper.h"

#include <vector>

/**
 * \file cttc-nr-fh-xr.cc
 * \ingroup examples
 * \brief An example to study Fronthaul limitations on XR traffic
 *
 * This example has been implemented in order to study the Fronthaul (FH) Control feature
 * implemented in the NrFhControl class. It allows for simulations of single- or multi-cell
 * deployments with variable number of UEs with XR traffic per cell. Each UE can be configured
 * with AR, VR, CG and VoIP traffic. Moreover, a variety of parameters can be configured by
 * the user, such as the propagation scenario, the data rate, the frame per seconds (FPS),
 * the transmit power, and the antenna parameters. You can have a look at the rest of the
 * parameters to check all the possible options offered using the following command:
 *
 * \code{.unparsed}
$ ./ns3 run "cttc-nr-fh-xr --PrintHelp"
    \endcode
 *
 * The basic command to study a single-cell FH schenario with XR traffic and evaluate the impact
 * that the FH limitation can have on the end-to-end throughput and latency, looks as follows:
 *
 *./ns3 run cttc-nr-fh-xr -- --fhCapacity=5000 --fhControlMethod=OptimizeRBs --frequency=30e9
 * --bandwidth=400e6 --numerology=3 --deployment=SIMPLE --arUeNum=3 --vrUeNum=3 --cgUeNum=3
 * --voiceUeNum=3 --appDuration=5000 --enableTDD4_1=1 --enableMimoFeedback=1 --txPower=30
 * --distance=2 --channelUpdatePeriod=0 --channelConditionUpdatePeriod=0 --enableShadowing=0
 * --isLos=1 --enableHarqRetx=1 --useFixedMcs=0 --enableInterServ=0 --enablePdcpDiscarding=1
 * --schedulerType=PF --reorderingTimerMs=10
 *
 * The configuration used in this command is based on the 3GPP R1-2111046 specification for
 * for performance evaluations for XR traffic.
 * Varying the fhCapacity and the fhControlMethod you can reproduce the results presented in
 * the paper "On the impact of Open RAN Fronthaul Control in scenarios with XR Traffic",
 * K. Koutlia, S. Lagen, Computer Networks, Volume 253, August 2024, where you can find
 * a detailed description of the FH Control implementation and an explanation of the results.
 *
 * Notice that the hexagonal deployment is not updated to latest versions of ns-3 and 5G-LENA
 * therefore errors might por-up. If you are interested on working on it, you will have first
 * to ensure its proper operation.
 *
 * For the REM generation use:
 *
 * ./ns3 run "cttc-nr-fh-xr --voiceUeNum=2 --numRings=1 --deployment=HEX
 * --dlRem=1 --xMin=-250 --xMax=250 --xRes=700 --yMin=-250 --yMax=250 --yRes=700
 * --remSector=0 --ns3::NrRadioEnvironmentMapHelper::RemMode=BeamShape
 * --ns3::NrRadioEnvironmentMapHelper::SimTag=testREM"
 *
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CttcNrFhXr");

std::string m_fhControlMethod;
uint32_t m_fhCapacity;
std::ofstream m_fhTraceFile;
std::string m_fhTraceFileName;
std::ofstream m_aiTraceFile;
std::string m_aiTraceFileName;
std::string m_outputDir;

struct VoiceApplicationSettings
{
    Ptr<Node> ue;
    uint32_t i;
    Ipv4Address ueIp;
    uint16_t uePort;
    std::string transportProtocol;
    Ptr<Node> remoteHost;
    Ptr<NetDevice> ueNetDev;
    Ptr<NrHelper> nrHelper;
    NrEpsBearer& bearer;
    Ptr<NrEpcTft> tft;
    ApplicationContainer& serverApps;
    ApplicationContainer& clientApps;
    ApplicationContainer& pingApps;
    std::string direction;
    Ipv4Address remoteHostAddress;
    uint16_t remoteHostPort;
};

static void PrintUePosition(NodeContainer ueNodes);
void ConfigureBwpTo(BandwidthPartInfoPtr& bwp, double centerFreq, double bwpBw);
void ReportFhTrace(const SfnSf& sfn, uint16_t physCellId, uint16_t bwpId, uint64_t reqFh);
void ReportAiTrace(const SfnSf& sfn, uint16_t physCellId, uint16_t bwpId, uint32_t airRbs);

void ConfigurePhy(Ptr<NrHelper> nrHelper,
                  Ptr<NetDevice> gnb,
                  double orientationRads,
                  uint16_t beamConfSector,
                  double beamConfElevation);

void ConfigureVoiceApp(VoiceApplicationSettings& voiceAppSettings);
void ConfigureXrApp(NodeContainer& ueContainer,
                    uint32_t i,
                    Ipv4InterfaceContainer& ueIpIface,
                    enum NrXrConfig config,
                    uint16_t uePort,
                    std::string transportProtocol,
                    NodeContainer& remoteHostContainer,
                    NetDeviceContainer& ueNetDev,
                    Ptr<NrHelper> nrHelper,
                    NrEpsBearer& bearer,
                    Ptr<NrEpcTft> tft,
                    bool isMx1,
                    std::vector<Ptr<NrEpcTft>>& tfts,
                    ApplicationContainer& serverApps,
                    ApplicationContainer& clientApps,
                    ApplicationContainer& pingApps,
                    std::string direction,
                    double arDataRate,
                    uint16_t arFps,
                    double vrDataRate,
                    uint16_t vrFps,
                    double cgDataRate,
                    Ipv4Address remoteHostAddress,
                    uint16_t remoteHostPort);

int
main(int argc, char* argv[])
{
    // enable logging or not
    bool logging = false;

    std::string nrConfigurationScenario = "DenseA";
    std::string deployment = "HEX";
    uint32_t freqScenario = 0; // 0 is NON-OVERLAPPING, 1 OVERLAPPING

    // set simulation time and mobility
    uint32_t appDurationParam = 10000;
    Time appStartTimeMs = MilliSeconds(400);

    uint16_t arUeNum = 0;
    uint16_t vrUeNum = 0;
    uint16_t cgUeNum = 0;
    uint16_t voiceUeNum = 0;

    double centralFrequency = 4e9;
    double bandwidth = 10e6;
    double txPower = 41;
    double ueTxPower = 23;
    uint16_t numerology = 1;
    std::string pattern = "DL|DL|DL|DL|UL|DL|DL|DL|DL|UL|";
    bool enableTDD4_1 = false;

    std::string propScenario = "UMa";
    std::string propChannelCondition = "Default";
    uint16_t numOuterRings = 0;
    double isd = 200;
    double bsHeight = 25.0;
    double utHeight = 1.5;
    double maxUeClosestSiteDistance = 1000;
    double minBsUtDistance = 10.0;
    double speed = 0;
    double antennaOffset = 1.0;
    double uesWithRandomUtHeight = 0;
    double distance = 200;

    double gnbNoiseFigure = 5.0;
    double ueNoiseFigure = 7.0;

    bool enableMimoFeedback = false;

    bool isGnbDualPolarized = false;
    uint32_t gnbNumRows = 4;
    uint32_t gnbNumColumns = 8;
    size_t gnbHorizPorts = 1;
    size_t gnbVertPorts = 1;
    double gnbHSpacing = 0.5;
    double gnbVSpacing = 0.8;
    double polSlantAngleGnb = 0.0;
    double bearingAngleGnb = 0.0;

    bool isUeDualPolarized = false;
    uint32_t ueNumRows = 1;
    uint32_t ueNumColumns = 1;
    size_t ueHorizPorts = 1;
    size_t ueVertPorts = 1;
    double ueHSpacing = 0.5;
    double ueVSpacing = 0.5;
    double polSlantAngleUe = 90.0;
    double bearingAngleUe = 180.0;

    double downtiltAngle = 0.0;
    uint16_t bfConfSector = 1;
    double bfConfElevation = 30;
    std::string bfMethod = "CellScan";

    NrHelper::MimoPmiParams mimoPmiParams;
    mimoPmiParams.pmSearchMethod = "ns3::NrPmSearchFull";
    mimoPmiParams.fullSearchCb = "ns3::NrCbTwoPort";
    mimoPmiParams.rankLimit = 2;
    mimoPmiParams.subbandSize = 8;

    bool enableOfdma = true;
    std::string schedulerType = "RR";

    bool isLos = false;
    int channelUpdatePeriod = 20;
    int channelConditionUpdatePeriod = 100;

    double o2iThreshold = 0;
    double o2iLowLossThreshold =
        1.0; // shows the percentage of low losses. Default value is 100% low
    bool linkO2iConditionToAntennaHeight = false;

    bool enableShadowing = true;
    uint8_t fixedMcs = 0;
    bool useFixedMcs = false;
    std::string errorModel = "ns3::NrEesmIrT2";

    // modulation compression parameters:
    uint32_t fhCapacity = 10000;                 // in Mbps
    uint8_t ohDyn = 100;                         // in bits
    std::string fhControlMethod = "OptimizeMcs"; // The FH Control Method to be applied
    // (Dropping, Postponing, OptimmizeMcs, OptimizeRBs)

    bool isMx1 = true;
    bool enableHarqRetx = true;
    bool enableInterServ = false;
    bool useUdp = true;
    bool useRlcUm = true;
    bool enableUl = false;

    double arDataRate = 5; // Mbps
    uint16_t arFps = 60;
    double vrDataRate = 5; // Mbps
    uint16_t vrFps = 60;
    double cgDataRate = 5; // Mbps

    bool enablePdcpDiscarding = false;
    uint32_t discardTimerMs = 0;
    uint32_t reorderingTimerMs = 100;

    bool enableNrHelperTraces = false;
    bool enableQosTrafficTraces = true;

    // Where we will store the output files.
    std::string simTag = "";
    std::string outputDir = "./";

    bool dlRem = false;
    double xMinRem = -2000.0;
    double xMaxRem = 2000.0;
    uint16_t xResRem = 100;
    double yMinRem = -2000.0;
    double yMaxRem = 2000.0;
    uint16_t yResRem = 100;
    double zRem = 1.5;
    uint32_t remSector = 0;
    bool enableFading = true;

    double progressIntervalInSeconds = 600; // 10 minutes

    CommandLine cmd;
    cmd.AddValue("deployment",
                 "The deployment of the cells. Choose among HEX or SIMPLE",
                 deployment);
    cmd.AddValue("nrConfigurationScenario",
                 "The NR calibration scenario string. Choose among:"
                 "DenseA (default), RuralA.",
                 nrConfigurationScenario);
    cmd.AddValue("propScenario", "The urban scenario string (UMa, RMa)", propScenario);
    cmd.AddValue("freqScenario",
                 "0: NON_OVERLAPPING (each sector in different freq - FR3), "
                 "1: OVERLAPPING (same freq for all sectors - FR1)",
                 freqScenario);
    cmd.AddValue("isd", "The ISD", isd);
    cmd.AddValue("numRings", "The number of rings", numOuterRings);
    cmd.AddValue("arUeNum", "The number of AR UEs", arUeNum);
    cmd.AddValue("vrUeNum", "The number of VR UEs", vrUeNum);
    cmd.AddValue("cgUeNum", "The number of CG UEs", cgUeNum);
    cmd.AddValue("voiceUeNum", "The number of VoIP UEs", voiceUeNum);
    cmd.AddValue("numerology", "The numerology to be used.", numerology);
    cmd.AddValue("enableTDD4_1",
                 "If True enables TDD 4:1 and numerology 1, DataRate 30Mbps for VR"
                 "and Fps 30 for AR.",
                 enableTDD4_1);
    cmd.AddValue("txPower", "Tx power to be configured to gNB", txPower);
    cmd.AddValue("bsHeight", "The gNB antenna height", bsHeight);
    cmd.AddValue("distance",
                 "The radius of the disc (in meters) that the UEs will be distributed."
                 "Default value is 200m",
                 distance);
    cmd.AddValue("enableMimoFeedback", "Enables MIMO feedback", enableMimoFeedback);
    cmd.AddValue("gnbNumRows", "The number of rows of the phased array of the gNB", gnbNumRows);
    cmd.AddValue("gnbNumColumns",
                 "The number of columns of the phased array of the gNB",
                 gnbNumColumns);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("frequency", "The system frequency", centralFrequency);
    cmd.AddValue("bandwidth", "The system bandwidth", bandwidth);
    cmd.AddValue(
        "fixedMcs",
        "The fixed MCS that will be used in this example if useFixedMcs is configured to true (1).",
        fixedMcs);
    cmd.AddValue("useFixedMcs",
                 "Whether to use fixed mcs, normally used for testing purposes",
                 useFixedMcs);
    cmd.AddValue("gnbNoiseFigure", "gNB Noise Figure", gnbNoiseFigure);
    cmd.AddValue("ueNoiseFigure", "UE Noise Figure", ueNoiseFigure);
    cmd.AddValue("useUdp",
                 "if true, the applications will run over UDP connection, otherwise a TCP "
                 "connection will be used. ",
                 useUdp);
    cmd.AddValue("useRlcUm", "if true, the Rlc UM will be used, otherwise RLC AM ", useRlcUm);
    cmd.AddValue("isLos", "if true, configure the LOS scenario, otherwise the default.", isLos);
    cmd.AddValue("enableOfdma",
                 "If set to true it enables Ofdma scheduler. Default value is false (Tdma)",
                 enableOfdma);
    cmd.AddValue("schedulerType",
                 "RR: Round-Robin (default), PF: Proportional Fair, Qos",
                 schedulerType);
    cmd.AddValue("isMx1",
                 "if true M SDFs will be mapped to 1 DRB, otherwise the mapping will "
                 "be 1x1, i.e., 1 SDF to 1 DRB.",
                 isMx1);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.AddValue("enableNrHelperTraces",
                 "If true, it enables the generation of the NrHelper traces, otherwise"
                 "NrHelper traces will not be generated. Default value is true",
                 enableNrHelperTraces);
    cmd.AddValue("enableQosTrafficTraces",
                 "If true, it enables the generation of the the Delay and Throughput"
                 "traces, otherwise these traces will not be generated. Default value is true",
                 enableQosTrafficTraces);
    cmd.AddValue("enableInterServ",
                 "If set to true VR is assigned 5QI87. Default value is false (5QI80)",
                 enableInterServ);
    cmd.AddValue("channelUpdatePeriod",
                 "The channel updated period value in ms. Default value is 20 ms",
                 channelUpdatePeriod);
    cmd.AddValue("channelConditionUpdatePeriod",
                 "The channel condition updated period value in ms. Default value is 100 ms",
                 channelConditionUpdatePeriod);
    cmd.AddValue("enableShadowing",
                 "If set to false shadowing is disabled. Default value is true",
                 enableShadowing);
    cmd.AddValue(
        "enableFading",
        "Used to enable/disable fading. By default is enabled. Used for the testing purposes.",
        enableFading);
    cmd.AddValue("appDuration", "Duration of the application in milliseconds.", appDurationParam);
    cmd.AddValue("enableHarqRetx",
                 "If set to false HARQ retransmissions are disabled. Default value is true",
                 enableHarqRetx);
    cmd.AddValue("maxUeClosestSiteDistance",
                 "Max distance between UE and the closest site",
                 maxUeClosestSiteDistance);
    cmd.AddValue("enablePdcpDiscarding",
                 "Whether to enable PDCP TX discarding",
                 enablePdcpDiscarding);
    cmd.AddValue("discardTimerMs",
                 "Discard timer value in milliseconds to use for all the flows",
                 discardTimerMs);
    cmd.AddValue("reorderingTimerMs",
                 "RLC t-Reordering timer value (See section 7.3 of 3GPP TS 36.322) in milliseconds "
                 "to use for all the flows",
                 reorderingTimerMs);
    cmd.AddValue("enableUl",
                 "If true, it enables UL direction traffic for AR and VoIP."
                 "Default is false",
                 enableUl);
    cmd.AddValue("dlRem",
                 "Generates DL REM without executing simulation. REM needs the"
                 "declaration of VoIP UEs for illustrative purposes",
                 dlRem);
    cmd.AddValue("xMin", "The min x coordinate of the rem map", xMinRem);
    cmd.AddValue("xMax", "The max x coordinate of the rem map", xMaxRem);
    cmd.AddValue("xRes", "The resolution on the x axis of the rem map", xResRem);
    cmd.AddValue("yMin", "The min y coordinate of the rem map", yMinRem);
    cmd.AddValue("yMax", "The max y coordinate of the rem map", yMaxRem);
    cmd.AddValue("yRes", "The resolution on the y axis of the rem map", yResRem);
    cmd.AddValue("z", "The z coordinate of the rem map", zRem);
    cmd.AddValue("remSector", "For which sector to generate the rem", remSector);
    cmd.AddValue("progressInterval", "Progress reporting interval", progressIntervalInSeconds);
    cmd.AddValue("fhCapacity", "Fronthaul capacity (Mbps)", fhCapacity);
    cmd.AddValue("ohDyn", "Overhead for dynamic modulation compression (bits)", ohDyn);
    cmd.AddValue(
        "fhControlMethod",
        "The FH Control Method to be applied. Choose among: Dropping, Postponing, OptimmizeMcs, "
        "OptimizeRBs",
        fhControlMethod);

    cmd.Parse(argc, argv);

    Time appDuration = MilliSeconds(appDurationParam);
    NS_ABORT_MSG_IF(deployment == "HEX",
                    "HEX deployment needs to be updated for proper operation."
                    "Currently, only SIMPLE deployment can be tested.");
    NS_ABORT_MSG_IF(appDuration < MilliSeconds(1000), "The appDuration should be at least 1000ms.");
    NS_ABORT_MSG_IF(!voiceUeNum && !vrUeNum && !arUeNum && !cgUeNum,
                    "Activate at least one type of traffic");
    NS_ABORT_MSG_IF(dlRem && !voiceUeNum, "For REM generation please declare a VoIP UE.");
    NS_ABORT_MSG_IF(deployment == "SIMPLE" && (nrConfigurationScenario == "RuralA"),
                    "SIMPLE can be used only with default DenseA configuration");

    // TODO: Fix HEX deployment to work properly with the latest code updates

    m_fhControlMethod = fhControlMethod;
    m_fhCapacity = fhCapacity;
    m_outputDir = outputDir;

    if (deployment == "HEX")
    {
        if (nrConfigurationScenario == "DenseA")
        {
            // For Dense most params are default, but data rates are not
            arDataRate = 1; // Mbps
            arFps = 30;
            vrDataRate = 5; // Mbps
            cgDataRate = 5; // Mbps
        }
        else if (nrConfigurationScenario == "RuralA")
        {
            propScenario = "RMa";
            isd = 1732;
            centralFrequency = 700e6;
            pattern = "DL|DL|DL|DL|UL|DL|DL|DL|DL|UL|";
            enableTDD4_1 = true;

            txPower = 46;
            bsHeight = 35;
            maxUeClosestSiteDistance = 500;

            useFixedMcs = false;

            gnbNumRows = 8;
            gnbNumColumns = 1;
            bfMethod = "Omni";

            arDataRate = 1; // Mbps
            arFps = 30;
            vrDataRate = 5; // Mbps
            cgDataRate = 5; // Mbps
        }
    }
    else if (deployment == "SIMPLE")
    {
        nrConfigurationScenario = "InH_OfficeOpen_LoS";
        propChannelCondition = "LOS";
        propScenario = "InH-OfficeOpen";
        centralFrequency = 30e9;
        pattern = "DL|DL|DL|DL|UL|DL|DL|DL|DL|UL|";
        enableTDD4_1 = true;
        numerology = 3;

        txPower = 30;
        bsHeight = 3.0;
        utHeight = 1.5;

        gnbNoiseFigure = 7;
        ueNoiseFigure = 13;

        useFixedMcs = false;

        gnbNumRows = 16;
        gnbNumColumns = 8;
        gnbHSpacing = 0.5;
        gnbVSpacing = 0.5;

        ueNumRows = 1;
        ueNumColumns = 4;
        ueHSpacing = 0.5;
        ueVSpacing = 0.5;

        bearingAngleGnb = 0.0;
        bearingAngleUe = 180.0;

        if (enableMimoFeedback)
        {
            Config::SetDefault("ns3::NrHelper::EnableMimoFeedback", BooleanValue(true));

            isGnbDualPolarized = true;
            gnbHorizPorts = 1;
            gnbVertPorts = 1;
            polSlantAngleGnb = 0.0;

            isUeDualPolarized = true;
            ueHorizPorts = 2;
            ueVertPorts = 1;
            polSlantAngleUe = 0.0;

            if (bandwidth == 400e6)
            {
                mimoPmiParams.subbandSize = 32;
            }
        }

        downtiltAngle = 90.0;

        arDataRate = 20; // Mbps
        arFps = 60;
        vrDataRate = 45; // Mbps
        vrFps = 120;
        cgDataRate = 30; // Mbps
    }
    else
    {
        NS_ABORT_MSG("Please choose between HEX and SIMPLE deployment");
    }

    NS_ABORT_MSG_IF(discardTimerMs && !enablePdcpDiscarding,
                    "General discard timer enabled but PDCP discarding not enabled!");

    ShowProgress spinner(Seconds(progressIntervalInSeconds));

    Time simTimeMs = appStartTimeMs + appDuration + MilliSeconds(10);
    std::cout << "Start example" << std::endl;

    if (deployment == "HEX")
    {
        std::string frChosen = freqScenario == 0 ? "FR3" : "FR1";
    }
    std::cout << "Deployment chosen: " << deployment
              << " - Configuration: " << nrConfigurationScenario << std::endl;

    std::string qosScenarioState = enableInterServ == 1 ? "Enabled" : "Disabled";
    std::cout << "Interactive Service for VR is: " << qosScenarioState << std::endl;

    std::string mappingArch = isMx1 == 1 ? "Mx1" : "1x1";
    std::cout << "Mapping architecture is set to: " << mappingArch << std::endl;

    std::string enableMimo = enableMimoFeedback == 1 ? "Enabled" : "Disabled";
    std::cout << "Mimo is set to: " << enableMimo << std::endl;

    if (logging)
    {
        LogLevel logLevel1 =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_INFO);
        // LogLevel logLevel2 =
        //     (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_DEBUG);
        LogComponentEnable("NrFhControl", logLevel1);
        // LogComponentEnable("NrGnbPhy", logLevel1);
        // LogComponentEnable("NrMacSchedulerNs3", logLevel2);
        // LogComponentEnable("NrMacSchedulerOfdma", logLevel2);
        // LogComponentEnable("NrRlcUm", logLevel2);
        // LogComponentEnable("NrHelper", logLevel2);
        // LogComponentEnable("FlowMonitor", logLevel2);
    }
    Config::SetDefault("ns3::NrRlcUm::EnablePdcpDiscarding", BooleanValue(enablePdcpDiscarding));
    Config::SetDefault("ns3::NrRlcUm::DiscardTimerMs", UintegerValue(discardTimerMs));
    Config::SetDefault("ns3::NrRlcUm::ReorderingTimer", TimeValue(MilliSeconds(reorderingTimerMs)));

    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod",
                       TimeValue(MilliSeconds(channelUpdatePeriod)));
    Config::SetDefault("ns3::NrGnbRrc::EpsBearerToRlcMapping",
                       EnumValue(useUdp ? NrGnbRrc::RLC_UM_ALWAYS : NrGnbRrc::RLC_AM_ALWAYS));
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Create Hex Deployment
    ScenarioParameters scenarioParams;

    // The essentials describing a laydown
    uint32_t gnbSites = 0;
    NodeContainer gnbNodes;
    NodeContainer ueNodes;
    MobilityHelper mobility;
    double sector0AngleRad = 30;
    uint32_t sectors = 3;
    NodeDistributionScenarioInterface* scenario{nullptr};
    HexagonalGridScenarioHelper gridScenario;

    if (deployment == "HEX")
    {
        scenarioParams.m_isd = isd;
        scenarioParams.m_bsHeight = bsHeight;
        scenarioParams.m_utHeight = utHeight;
        scenarioParams.m_minBsUtDistance = minBsUtDistance;
        scenarioParams.m_antennaOffset = antennaOffset;

        scenarioParams.SetSectorization(sectors);
        scenarioParams.SetScenarioParameters(scenarioParams);

        std::cout << "  hexagonal grid: ";
        gridScenario.SetScenarioParameters(scenarioParams);
        gridScenario.SetSimTag(simTag);
        gridScenario.SetResultsDir(outputDir);
        gridScenario.SetNumRings(numOuterRings);
        gnbSites = gridScenario.GetNumSites();
        uint32_t ueNum = (voiceUeNum + arUeNum + vrUeNum + cgUeNum) * gnbSites * sectors;
        gridScenario.SetUtNumber(ueNum);
        sector0AngleRad = gridScenario.GetAntennaOrientationRadians(0);
        std::cout << sector0AngleRad << std::endl;

        // Creates and plots the network deployment
        gridScenario.SetMaxUeDistanceToClosestSite(maxUeClosestSiteDistance);
        gridScenario.CreateScenarioWithMobility(Vector(speed, 0, 0),
                                                uesWithRandomUtHeight); // move UEs along the x axis

        gnbNodes = gridScenario.GetBaseStations();
        ueNodes = gridScenario.GetUserTerminals();
        scenario = &gridScenario;
    }
    else
    {
        sectors = 0; // this would be the sector index
        gnbSites = 1;
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

        gnbNodes.Create(1);
        ueNodes.Create(voiceUeNum + arUeNum + vrUeNum + cgUeNum);

        Ptr<ListPositionAllocator> bsPositionAlloc = CreateObject<ListPositionAllocator>();
        bsPositionAlloc->Add(Vector(0.0, 0.0, bsHeight));
        mobility.SetPositionAllocator(bsPositionAlloc);
        mobility.Install(gnbNodes);

        Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
        uePositionAlloc->Add(Vector(0.0, distance, utHeight));
        mobility.SetPositionAllocator(uePositionAlloc);
        mobility.Install(ueNodes.Get(
            0)); // we want the first node on a specific location, the rest is randomly distributed

        // by default the disc is of the radius of 200 meters - we change it to 20 meters
        Ptr<RandomDiscPositionAllocator> ueDiscPositionAlloc =
            CreateObject<RandomDiscPositionAllocator>();
        ueDiscPositionAlloc->SetX(0.0);
        ueDiscPositionAlloc->SetY(0.0);
        ueDiscPositionAlloc->SetZ(utHeight);
        Ptr<UniformRandomVariable> randomDiscPos = CreateObject<UniformRandomVariable>();
        randomDiscPos->SetAttribute("Min", DoubleValue(0));
        randomDiscPos->SetAttribute("Max", DoubleValue(20));
        ueDiscPositionAlloc->SetRho(randomDiscPos);
        mobility.SetPositionAllocator(ueDiscPositionAlloc);

        for (uint32_t i = 1; i < ueNodes.GetN(); i++)
        {
            mobility.Install(ueNodes.Get(i));
        }
    }

    // Log the configuration
    std::cout << "\n    Topology configuration: " << gnbSites << " sites, " << sectors
              << " sectors/site, " << gnbNodes.GetN() << " cells, " << ueNodes.GetN() << " UEs\n";

    /*
     * Create different gNB NodeContainer for the different sectors.
     *
     * Relationships between ueId, cellId, sectorId and siteId:
     * ~~~{.cc}
     *   cellId = scenario->GetCellIndex (ueId);
     *   sector = scenario->GetSectorIndex (cellId);
     *   siteId = scenario->GetSiteIndex (cellId);
     * ~~~{.cc}
     *
     * Iterate/index gnbNodes, gnbNetDevs by `cellId`.
     * Iterate/index gnbSector<N>Container, gnbNodesBySector[sector],
     *   gnbSector<N>NetDev, gnbNdBySector[sector] by `siteId`
     */
    NodeContainer gnbSector1Container;
    NodeContainer gnbSector2Container;
    NodeContainer gnbSector3Container;
    std::vector<NodeContainer*> gnbNodesBySector{&gnbSector1Container,
                                                 &gnbSector2Container,
                                                 &gnbSector3Container};

    for (uint32_t cellId = 0; cellId < gnbNodes.GetN(); ++cellId)
    {
        Ptr<Node> gnb = gnbNodes.Get(cellId);
        auto sectorIndex = deployment == "HEX" ? scenario->GetSectorIndex(cellId) : 0;
        gnbNodesBySector[sectorIndex]->Add(gnb);
    }
    std::cout << "    gNb containers: " << gnbSector1Container.GetN() << ", "
              << gnbSector2Container.GetN() << ", " << gnbSector3Container.GetN() << std::endl;

    /*
     * Create different UE NodeContainer for the different sectors and the
     * different traffic types.
     *
     * Multiple UEs per sector!
     * Iterate/index ueNodes, ueNetDevs, ueIpIfaces by `ueId`.
     * Iterate/Index ueSector<N>Container, ueNodesBySector[sector],
     *   ueSector<N>NetDev, ueNdBySector[sector] with i % gnbSites
     */
    NodeContainer ueSector1Container;
    NodeContainer ueSector2Container;
    NodeContainer ueSector3Container;
    std::vector<NodeContainer*> ueNodesBySector{&ueSector1Container,
                                                &ueSector2Container,
                                                &ueSector3Container};

    NodeContainer ueArSector1Container;
    NodeContainer ueVrSector1Container;
    NodeContainer ueCgSector1Container;
    NodeContainer ueVoiceSector1Container;
    NodeContainer ueArSector2Container;
    NodeContainer ueVrSector2Container;
    NodeContainer ueCgSector2Container;
    NodeContainer ueVoiceSector2Container;
    NodeContainer ueArSector3Container;
    NodeContainer ueVrSector3Container;
    NodeContainer ueCgSector3Container;
    NodeContainer ueVoiceSector3Container;

    std::vector<NodeContainer*> ueVoiceBySector{&ueVoiceSector1Container,
                                                &ueVoiceSector2Container,
                                                &ueVoiceSector3Container};
    std::vector<NodeContainer*> ueArBySector{&ueArSector1Container,
                                             &ueArSector2Container,
                                             &ueArSector3Container};
    std::vector<NodeContainer*> ueVrBySector{&ueVrSector1Container,
                                             &ueVrSector2Container,
                                             &ueVrSector3Container};
    std::vector<NodeContainer*> ueCgBySector{&ueCgSector1Container,
                                             &ueCgSector2Container,
                                             &ueCgSector3Container};

    uint32_t voiceUeCnt = voiceUeNum * gnbNodes.GetN();
    uint32_t arUeCnt = arUeNum * gnbNodes.GetN();
    uint32_t vrUeCnt = vrUeNum * gnbNodes.GetN();
    uint32_t cgUeCnt = cgUeNum * gnbNodes.GetN();

    for (uint32_t ueId = 0; ueId < ueNodes.GetN(); ++ueId)
    {
        Ptr<Node> ue = ueNodes.Get(ueId);
        auto cellId = deployment == "HEX" ? scenario->GetCellIndex(ueId) : 0;
        auto sectorIndex = deployment == "HEX" ? scenario->GetSectorIndex(cellId) : 0;
        ueNodesBySector[sectorIndex]->Add(ue);

        if (voiceUeCnt > 0)
        {
            ueVoiceBySector[sectorIndex]->Add(ue);
            voiceUeCnt--;
        }
        else if (arUeCnt > 0)
        {
            ueArBySector[sectorIndex]->Add(ue);
            arUeCnt--;
        }
        else if (vrUeCnt > 0)
        {
            ueVrBySector[sectorIndex]->Add(ue);
            vrUeCnt--;
        }
        else if (cgUeCnt > 0)
        {
            ueCgBySector[sectorIndex]->Add(ue);
            cgUeCnt--;
        }
    }
    std::cout << "    UE containers: " << ueSector1Container.GetN() << ", "
              << ueSector2Container.GetN() << ", " << ueSector3Container.GetN() << std::endl;

    std::cout << "    UE Traffic containers: "
              << ", "
              << "Sector 1: " << ueVoiceSector1Container.GetN() << ", "
              << ueArSector1Container.GetN() << ", " << ueVrSector1Container.GetN() << ", "
              << ueCgSector1Container.GetN() << ", "
              << "Sector 2: " << ueVoiceSector2Container.GetN() << ", "
              << ueArSector2Container.GetN() << ", " << ueVrSector2Container.GetN() << ", "
              << ueCgSector2Container.GetN() << ", "
              << "Sector 3: " << ueVoiceSector3Container.GetN() << ", "
              << ueArSector3Container.GetN() << ", " << ueVrSector3Container.GetN() << ", "
              << ueCgSector3Container.GetN() << ", " << std::endl;

    // setup the nr simulation
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();

    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    nrHelper->SetEpcHelper(epcHelper);

    Ptr<IdealBeamformingHelper> idealBeamformingHelper;
    idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();

    uint8_t numScPerRb = 1;
    double rbOverhead = 0.04;
    uint32_t harqProcesses = 16;

    uint32_t n1Delay = 2;
    uint32_t n2Delay = 2;
    uint8_t dlCtrlSymbols = 1;

    // Create ChannelHelper API
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    if (isLos)
    {
        propChannelCondition = "LOS";
    }

    NS_ABORT_MSG_UNLESS(
        propScenario == "UMa" || propScenario == "RMa" || propScenario == "InH-OfficeOpen",
        "Unsupported scenario " << scenario << ". Supported values: UMa, RMa, InH-OfficeOpen");
    // Configure the factories for the channel creation
    channelHelper->ConfigureFactories(propScenario, propChannelCondition);
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(enableShadowing));
    if (!isLos)
    {
        channelHelper->SetChannelConditionModelAttribute(
            "UpdatePeriod",
            TimeValue(MilliSeconds(channelConditionUpdatePeriod)));
    }
    // In case of DistanceBasedThreeGppSpectrumPropagationLossModel, the creation of the channel
    // must be manually done, as the channel helper does not support the creation of this specific
    // model.
    ObjectFactory distanceBasedChannelFactory;
    if (deployment == "HEX")
    {
        distanceBasedChannelFactory.SetTypeId(
            DistanceBasedThreeGppSpectrumPropagationLossModel::GetTypeId());
        distanceBasedChannelFactory.Set("MaxDistance", DoubleValue(2 * isd));
        channelHelper->SetChannelConditionModelAttribute(
            "LinkO2iConditionToAntennaHeight",
            BooleanValue(linkO2iConditionToAntennaHeight));
        channelHelper->SetChannelConditionModelAttribute("O2iThreshold", DoubleValue(o2iThreshold));
        channelHelper->SetChannelConditionModelAttribute("O2iLowLossThreshold",
                                                         DoubleValue(o2iLowLossThreshold));

        std::cout << "o2iThreshold: " << o2iThreshold << std::endl;
    }

    /********************************************************************/
    nrHelper->EnableFhControl();
    nrHelper->SetFhControlAttribute("FhControlMethod", StringValue(fhControlMethod));
    nrHelper->SetFhControlAttribute("FhCapacity", UintegerValue(fhCapacity));
    nrHelper->SetFhControlAttribute("OverheadDyn", UintegerValue(ohDyn));
    /********************************************************************/

    std::stringstream scheduler;
    std::string subType;

    subType = !enableOfdma ? "Tdma" : "Ofdma";
    scheduler << "ns3::NrMacScheduler" << subType << schedulerType;
    std::cout << "Scheduler: " << scheduler.str() << std::endl;
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName(scheduler.str()));

    if (enableTDD4_1)
    {
        nrHelper->SetGnbPhyAttribute("Pattern", StringValue(pattern));
    }

    // Error Model: UE and GNB with same spectrum error model.
    nrHelper->SetUlErrorModel(errorModel);
    nrHelper->SetDlErrorModel(errorModel);

    // Both DL and UL AMC will have the same model behind.
    nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
    nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));

    if (deployment == "HEX")
    {
        Config::SetDefault("ns3::NrMacSchedulerSrsDefault::StartingPeriodicity", UintegerValue(16));
        // configure SRS symbols
        nrHelper->SetSchedulerAttribute("SrsSymbols", UintegerValue(1));
        nrHelper->SetSchedulerAttribute("EnableSrsInUlSlots", BooleanValue(false));
        nrHelper->SetSchedulerAttribute("EnableSrsInFSlots", BooleanValue(false));

        /*
         * Adjust the average number of Reference symbols per RB only for LTE case,
         * which is larger than in NR. We assume a value of 4 (could be 3 too).
         */
        nrHelper->SetGnbDlAmcAttribute("NumRefScPerRb", UintegerValue(numScPerRb));
        nrHelper->SetGnbUlAmcAttribute("NumRefScPerRb",
                                       UintegerValue(1)); // FIXME: Might change in LTE

        nrHelper->SetGnbPhyAttribute("RbOverhead", DoubleValue(rbOverhead));
        nrHelper->SetGnbPhyAttribute("N2Delay", UintegerValue(n2Delay));
        nrHelper->SetGnbPhyAttribute("N1Delay", UintegerValue(n1Delay));

        nrHelper->SetUeMacAttribute("NumHarqProcess", UintegerValue(harqProcesses));
        nrHelper->SetGnbMacAttribute("NumHarqProcess", UintegerValue(harqProcesses));

        // configure CTRL symbols
        nrHelper->SetSchedulerAttribute("DlCtrlSymbols", UintegerValue(dlCtrlSymbols));
    }

    nrHelper->SetSchedulerAttribute("EnableHarqReTx", BooleanValue(enableHarqRetx));
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(numerology));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(ueTxPower));

    nrHelper->SetSchedulerAttribute("FixedMcsDl", BooleanValue(useFixedMcs));
    nrHelper->SetSchedulerAttribute("FixedMcsUl", BooleanValue(useFixedMcs));
    if (useFixedMcs)
    {
        nrHelper->SetSchedulerAttribute("StartingMcsDl", UintegerValue(fixedMcs));
        nrHelper->SetSchedulerAttribute("StartingMcsUl", UintegerValue(fixedMcs));
    }

    // Noise figure for the gNB
    nrHelper->SetGnbPhyAttribute("NoiseFigure", DoubleValue(gnbNoiseFigure));
    // Noise figure for the UE
    nrHelper->SetUePhyAttribute("NoiseFigure", DoubleValue(ueNoiseFigure));

    const double band0Start = centralFrequency;
    uint8_t numBwp = 1;
    double bandwidthCc = numBwp * bandwidth;
    uint8_t numCcPerBand = 1;
    double bandwidthBand = numCcPerBand * bandwidthCc;
    double bandCenter = band0Start + bandwidthBand / 2.0;

    OperationBandInfo band0;
    OperationBandInfo band1;
    OperationBandInfo band2;
    band0.m_bandId = 0;
    band1.m_bandId = 1;
    band2.m_bandId = 2;

    uint8_t bandMask = NrChannelHelper::INIT_PROPAGATION;
    if (enableFading)
    {
        bandMask |= NrChannelHelper::INIT_FADING;
    }
    // Create NrChannelHelper
    if (deployment == "SIMPLE")
    {
        // simple band configuration and initialize
        CcBwpCreator ccBwpCreator;
        CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, 1);
        band0 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
        channelHelper->AssignChannelsToBands({band0}, bandMask);
    }
    else if (deployment == "HEX" && freqScenario == 0) // NON_OVERLAPPING
    {
        NS_LOG_LOGIC("NON_OVERLAPPING, " << ": " << bandwidthBand << ":" << bandwidthCc << ", "
                                         << (int)numCcPerBand << ", " << (int)numBwp);

        NS_LOG_LOGIC("bandConf0: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf0(bandCenter, bandwidthBand, numCcPerBand);
        bandConf0.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        NS_LOG_LOGIC("bandConf1: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf1(bandCenter, bandwidthBand, numCcPerBand);
        bandConf1.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        NS_LOG_LOGIC("bandConf2: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf2(bandCenter, bandwidthBand, numCcPerBand);
        bandConf2.m_numBwp = numBwp;

        // Create, then configure
        CcBwpCreator ccBwpCreator;
        band0 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf0);
        band0.m_bandId = 0;

        band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
        band1.m_bandId = 1;

        band2 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf2);
        band2.m_bandId = 2;

        bandCenter = band0Start + bandwidth / 2.0;

        NS_LOG_LOGIC("band0[0][0]: " << bandCenter << " " << bandwidth);
        ConfigureBwpTo(band0.m_cc[0]->m_bwp[0], bandCenter, bandwidth);
        bandCenter += bandwidth;

        NS_LOG_LOGIC("band1[0][0]: " << bandCenter << " " << bandwidth);
        ConfigureBwpTo(band1.m_cc[0]->m_bwp[0], bandCenter, bandwidth);
        bandCenter += bandwidth;

        NS_LOG_LOGIC("band2[0][0]: " << bandCenter << " " << bandwidth);
        ConfigureBwpTo(band2.m_cc[0]->m_bwp[0], bandCenter, bandwidth);
        bandCenter += bandwidth;

        std::cout << "BWP Configuration for NON_OVERLAPPING case "
                  << "\n"
                  << band0 << band1 << band2;

        // Manually assignment of the distance-based channels to the bands
        for (size_t i = 0; i < band0.GetBwps().size(); i++)
        {
            auto distanceBased3gpp =
                distanceBasedChannelFactory
                    .Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
            distanceBased3gpp->SetChannelModelAttribute(
                "Frequency",
                DoubleValue(band0.GetBwpAt(0, i)->m_centralFrequency));
            distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(propScenario));
            auto specChannelBand0 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
            if (enableFading)
            {
                PointerValue channelConditionModel0;
                specChannelBand0->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                          channelConditionModel0);
                distanceBased3gpp->SetChannelModelAttribute(
                    "ChannelConditionModel",
                    PointerValue(channelConditionModel0.Get<ChannelConditionModel>()));
                specChannelBand0->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
            }
            band0.GetBwpAt(0, i)->SetChannel(specChannelBand0);
        }
        for (size_t i = 0; i < band1.GetBwps().size(); i++)
        {
            auto distanceBased3gpp =
                distanceBasedChannelFactory
                    .Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
            distanceBased3gpp->SetChannelModelAttribute(
                "Frequency",
                DoubleValue(band1.GetBwpAt(0, i)->m_centralFrequency));
            distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(propScenario));
            auto specChannelBand1 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
            if (enableFading)
            {
                PointerValue channelConditionModel1;
                specChannelBand1->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                          channelConditionModel1);
                distanceBased3gpp->SetChannelModelAttribute(
                    "ChannelConditionModel",
                    PointerValue(channelConditionModel1.Get<ChannelConditionModel>()));
                specChannelBand1->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
            }
            band1.GetBwpAt(0, i)->SetChannel(specChannelBand1);
        }
        for (size_t i = 0; i < band2.GetBwps().size(); i++)
        {
            auto distanceBased3gpp =
                distanceBasedChannelFactory
                    .Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
            distanceBased3gpp->SetAttribute("MaxDistance", DoubleValue(2 * isd));
            distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(propScenario));
            distanceBased3gpp->SetChannelModelAttribute(
                "Frequency",
                DoubleValue(band2.GetBwpAt(0, i)->m_centralFrequency));
            auto specChannelBand2 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
            if (enableFading)
            {
                PointerValue channelConditionModel2;
                specChannelBand2->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                          channelConditionModel2);
                distanceBased3gpp->SetChannelModelAttribute(
                    "ChannelConditionModel",
                    PointerValue(channelConditionModel2.Get<ChannelConditionModel>()));
                specChannelBand2->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
            }
            band2.GetBwpAt(0, i)->SetChannel(specChannelBand2);
        }
    }
    else if (deployment == "HEX" && freqScenario == 1) // // OVERLAPPING
    {
        NS_LOG_LOGIC("OVERLAPPING, " << bandwidthBand << ":" << bandwidthCc << ":" << bandwidth
                                     << ", " << (int)numCcPerBand << ", " << (int)numBwp);

        NS_LOG_LOGIC("bandConf0: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf0(bandCenter, bandwidthBand, numCcPerBand);
        bandConf0.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        // Create, then configure
        CcBwpCreator ccBwpCreator;
        band0 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf0);
        band0.m_bandId = 0;

        bandCenter = band0Start + bandwidth / 2.0;

        NS_LOG_LOGIC("band0[0][0]: " << bandCenter << " " << bandwidth);
        ConfigureBwpTo(band0.m_cc[0]->m_bwp[0], bandCenter, bandwidth);
        bandCenter += bandwidth;

        for (size_t i = 0; i < band0.GetBwps().size(); i++)
        {
            auto distanceBased3gpp =
                distanceBasedChannelFactory
                    .Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
            distanceBased3gpp->SetChannelModelAttribute(
                "Frequency",
                DoubleValue(band0.GetBwpAt(0, i)->m_centralFrequency));
            distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(propScenario));
            auto specChannelBand0 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
            if (enableFading)
            {
                PointerValue channelConditionModel0;
                specChannelBand0->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                          channelConditionModel0);
                distanceBased3gpp->SetChannelModelAttribute(
                    "ChannelConditionModel",
                    PointerValue(channelConditionModel0.Get<ChannelConditionModel>()));
                specChannelBand0->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
            }
            band0.GetBwpAt(0, i)->SetChannel(specChannelBand0);
        }
    }

    BandwidthPartInfoPtrVector sector1Bwps;
    BandwidthPartInfoPtrVector sector2Bwps;
    BandwidthPartInfoPtrVector sector3Bwps;

    if (deployment == "SIMPLE")
    {
        sector1Bwps = CcBwpCreator::GetAllBwps({band0});
    }
    else if (deployment == "HEX" && freqScenario == 0) // NON_OVERLAPPING
    {
        sector1Bwps = CcBwpCreator::GetAllBwps({band0});
        sector2Bwps = CcBwpCreator::GetAllBwps({band1});
        sector3Bwps = CcBwpCreator::GetAllBwps({band2});
    }
    else // OVERLAPPING
    {
        sector1Bwps = CcBwpCreator::GetAllBwps({band0});
        sector2Bwps = CcBwpCreator::GetAllBwps({band0});
        sector3Bwps = CcBwpCreator::GetAllBwps({band0});
    }

    // Beamforming method
    if (deployment == "HEX")
    {
        if (bfMethod == "Omni")
        {
            idealBeamformingHelper->SetBeamformingMethod(
                QuasiOmniDirectPathBeamforming::GetTypeId());
        }
        else if (bfMethod == "CellScan")
        {
            idealBeamformingHelper->SetBeamformingMethod(CellScanBeamforming::GetTypeId());
            idealBeamformingHelper->SetAttribute("BeamformingPeriodicity",
                                                 TimeValue(MilliSeconds(10)));
        }
    }
    else if (deployment == "SIMPLE")
    {
        idealBeamformingHelper->SetAttribute(
            "BeamformingMethod",
            TypeIdValue(QuasiOmniDirectPathBeamforming::GetTypeId()));
    }
    if (enableFading)
    {
        nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    }

    bearingAngleGnb = bearingAngleGnb * (M_PI / 180);
    bearingAngleUe = bearingAngleUe * (M_PI / 180);

    epcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(gnbNumRows));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(gnbNumColumns));
    nrHelper->SetGnbAntennaAttribute("AntennaHorizontalSpacing", DoubleValue(gnbHSpacing));
    nrHelper->SetGnbAntennaAttribute("AntennaVerticalSpacing", DoubleValue(gnbVSpacing));
    nrHelper->SetGnbAntennaAttribute("DowntiltAngle", DoubleValue(downtiltAngle * M_PI / 180.0));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrHelper->SetGnbAntennaAttribute("BearingAngle", DoubleValue(bearingAngleGnb));

    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(ueNumRows));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(ueNumColumns));
    nrHelper->SetUeAntennaAttribute("AntennaHorizontalSpacing", DoubleValue(ueHSpacing));
    nrHelper->SetUeAntennaAttribute("AntennaVerticalSpacing", DoubleValue(ueVSpacing));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrHelper->SetUeAntennaAttribute("BearingAngle", DoubleValue(bearingAngleUe));

    if (enableMimoFeedback)
    {
        polSlantAngleGnb = bearingAngleGnb * (M_PI / 180);

        nrHelper->SetGnbAntennaAttribute("IsDualPolarized", BooleanValue(isGnbDualPolarized));
        nrHelper->SetGnbAntennaAttribute("NumHorizontalPorts", UintegerValue(gnbHorizPorts));
        nrHelper->SetGnbAntennaAttribute("NumVerticalPorts", UintegerValue(gnbVertPorts));
        nrHelper->SetGnbAntennaAttribute("PolSlantAngle", DoubleValue(polSlantAngleGnb));

        polSlantAngleUe = polSlantAngleUe * (M_PI / 180);

        nrHelper->SetUeAntennaAttribute("IsDualPolarized", BooleanValue(isUeDualPolarized));
        nrHelper->SetUeAntennaAttribute("NumHorizontalPorts", UintegerValue(ueHorizPorts));
        nrHelper->SetUeAntennaAttribute("NumVerticalPorts", UintegerValue(ueVertPorts));
        nrHelper->SetUeAntennaAttribute("PolSlantAngle", DoubleValue(polSlantAngleUe));

        nrHelper->SetupMimoPmi(mimoPmiParams);
    }

    uint32_t bwpIdForLowLat = 0;
    uint32_t bwpIdForVoice = 0;
    uint32_t bwpIdForVR = 0;

    // gNb routing between Bearer and bandwidh part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB",
                                                 UintegerValue(bwpIdForLowLat));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForLowLat));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    if (enableInterServ)
    {
        nrHelper->SetGnbBwpManagerAlgorithmAttribute("DGBR_INTER_SERV_87",
                                                     UintegerValue(bwpIdForVR));
        nrHelper->SetUeBwpManagerAlgorithmAttribute("DGBR_INTER_SERV_87",
                                                    UintegerValue(bwpIdForVR));
    }

    // Initialize nrHelper
    nrHelper->Initialize();

    NetDeviceContainer gnbSector1NetDev;
    NetDeviceContainer gnbSector2NetDev;
    NetDeviceContainer gnbSector3NetDev;

    NetDeviceContainer ueVoiceSector1NetDev;
    NetDeviceContainer ueArSector1NetDev;
    NetDeviceContainer ueVrSector1NetDev;
    NetDeviceContainer ueCgSector1NetDev;
    NetDeviceContainer ueVoiceSector2NetDev;
    NetDeviceContainer ueArSector2NetDev;
    NetDeviceContainer ueVrSector2NetDev;
    NetDeviceContainer ueCgSector2NetDev;
    NetDeviceContainer ueVoiceSector3NetDev;
    NetDeviceContainer ueArSector3NetDev;
    NetDeviceContainer ueVrSector3NetDev;
    NetDeviceContainer ueCgSector3NetDev;

    // Defined for REM purposes
    std::vector<NetDeviceContainer*> gnbNdBySector{&gnbSector1NetDev,
                                                   &gnbSector2NetDev,
                                                   &gnbSector3NetDev};
    std::vector<NetDeviceContainer*> ueNdBySector{&ueVoiceSector1NetDev,
                                                  &ueVoiceSector2NetDev,
                                                  &ueVoiceSector3NetDev};

    gnbSector1NetDev = nrHelper->InstallGnbDevice(gnbSector1Container, sector1Bwps);
    NetDeviceContainer gnbNetDevs(gnbSector1NetDev);

    ueVoiceSector1NetDev = nrHelper->InstallUeDevice(ueVoiceSector1Container, sector1Bwps);
    ueArSector1NetDev = nrHelper->InstallUeDevice(ueArSector1Container, sector1Bwps);
    ueVrSector1NetDev = nrHelper->InstallUeDevice(ueVrSector1Container, sector1Bwps);
    ueCgSector1NetDev = nrHelper->InstallUeDevice(ueCgSector1Container, sector1Bwps);

    NetDeviceContainer ueNetDevs(ueVoiceSector1NetDev);
    ueNetDevs.Add(ueArSector1NetDev);
    ueNetDevs.Add(ueVrSector1NetDev);
    ueNetDevs.Add(ueCgSector1NetDev);

    if (deployment == "HEX")
    {
        gnbSector2NetDev = nrHelper->InstallGnbDevice(gnbSector2Container, sector2Bwps);
        gnbNetDevs.Add(gnbSector2NetDev);
        gnbSector3NetDev = nrHelper->InstallGnbDevice(gnbSector3Container, sector3Bwps);
        gnbNetDevs.Add(gnbSector3NetDev);

        ueVoiceSector2NetDev = nrHelper->InstallUeDevice(ueVoiceSector2Container, sector2Bwps);
        ueArSector2NetDev = nrHelper->InstallUeDevice(ueArSector2Container, sector2Bwps);
        ueVrSector2NetDev = nrHelper->InstallUeDevice(ueVrSector2Container, sector2Bwps);
        ueCgSector2NetDev = nrHelper->InstallUeDevice(ueCgSector2Container, sector2Bwps);
        ueNetDevs.Add(ueVoiceSector2NetDev);
        ueNetDevs.Add(ueArSector2NetDev);
        ueNetDevs.Add(ueVrSector2NetDev);
        ueNetDevs.Add(ueCgSector2NetDev);

        ueVoiceSector3NetDev = nrHelper->InstallUeDevice(ueVoiceSector3Container, sector3Bwps);
        ueArSector3NetDev = nrHelper->InstallUeDevice(ueArSector3Container, sector3Bwps);
        ueVrSector3NetDev = nrHelper->InstallUeDevice(ueVrSector3Container, sector3Bwps);
        ueCgSector3NetDev = nrHelper->InstallUeDevice(ueCgSector3Container, sector3Bwps);
        ueNetDevs.Add(ueVoiceSector3NetDev);
        ueNetDevs.Add(ueArSector3NetDev);
        ueNetDevs.Add(ueVrSector3NetDev);
        ueNetDevs.Add(ueCgSector3NetDev);
    }

    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(gnbNetDevs, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDevs, randomStream);

    // Sectors (cells) of a site are pointing at different directions
    std::vector<double> sectorOrientationRad{
        sector0AngleRad,
        sector0AngleRad + 2.0 * M_PI / 3.0, // + 120 deg
        sector0AngleRad - 2.0 * M_PI / 3.0  // - 120 deg
    };

    // TODO: Check that for HEX case we configure correctly the antenna orientations
    if (deployment == "HEX")
    {
        for (uint32_t cellId = 0; cellId < gnbNetDevs.GetN(); ++cellId)
        {
            Ptr<NetDevice> gnb = gnbNetDevs.Get(cellId);
            uint32_t numBwps = nrHelper->GetNumberBwp(gnb);
            if (numBwps > 2)
            {
                NS_ABORT_MSG("Incorrect number of BWPs per CC");
            }

            uint32_t sector = cellId % (gnbSector3NetDev.GetN() == 0 ? 1 : 3);
            double orientation = sectorOrientationRad[sector];

            // BWP (in case of TDD)
            ConfigurePhy(nrHelper, gnb, orientation, bfConfSector, bfConfElevation);
        }
    }

    nrHelper->ConfigureFhControl(gnbSector1NetDev);
    if (deployment == "HEX")
    {
        nrHelper->ConfigureFhControl(gnbSector2NetDev);
        nrHelper->ConfigureFhControl(gnbSector3NetDev);
    }

    PrintUePosition(ueNodes);

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1000));
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

    Ipv4InterfaceContainer ueVoiceSector1IpIface;
    Ipv4InterfaceContainer ueArSector1IpIface;
    Ipv4InterfaceContainer ueVrSector1IpIface;
    Ipv4InterfaceContainer ueCgSector1IpIface;

    Ipv4InterfaceContainer ueVoiceSector2IpIface;
    Ipv4InterfaceContainer ueArSector2IpIface;
    Ipv4InterfaceContainer ueVrSector2IpIface;
    Ipv4InterfaceContainer ueCgSector2IpIface;

    Ipv4InterfaceContainer ueVoiceSector3IpIface;
    Ipv4InterfaceContainer ueArSector3IpIface;
    Ipv4InterfaceContainer ueVrSector3IpIface;
    Ipv4InterfaceContainer ueCgSector3IpIface;

    ueVoiceSector1IpIface =
        epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVoiceSector1NetDev));
    ueArSector1IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueArSector1NetDev));
    ueVrSector1IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVrSector1NetDev));
    ueCgSector1IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueCgSector1NetDev));

    if (deployment == "HEX")
    {
        ueVoiceSector2IpIface =
            epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVoiceSector2NetDev));
        ueArSector2IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueArSector2NetDev));
        ueVrSector2IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVrSector2NetDev));
        ueCgSector2IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueCgSector2NetDev));

        ueVoiceSector3IpIface =
            epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVoiceSector3NetDev));
        ueArSector3IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueArSector3NetDev));
        ueVrSector3IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueVrSector3NetDev));
        ueCgSector3IpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueCgSector3NetDev));
    }

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < ueNodes.GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest eNB
    nrHelper->AttachToClosestGnb(ueVoiceSector1NetDev, gnbSector1NetDev);
    nrHelper->AttachToClosestGnb(ueArSector1NetDev, gnbSector1NetDev);
    nrHelper->AttachToClosestGnb(ueVrSector1NetDev, gnbSector1NetDev);
    nrHelper->AttachToClosestGnb(ueCgSector1NetDev, gnbSector1NetDev);

    if (deployment == "HEX")
    {
        nrHelper->AttachToClosestGnb(ueVoiceSector2NetDev, gnbSector2NetDev);
        nrHelper->AttachToClosestGnb(ueArSector2NetDev, gnbSector2NetDev);
        nrHelper->AttachToClosestGnb(ueVrSector2NetDev, gnbSector2NetDev);
        nrHelper->AttachToClosestGnb(ueCgSector2NetDev, gnbSector2NetDev);

        nrHelper->AttachToClosestGnb(ueVoiceSector3NetDev, gnbSector3NetDev);
        nrHelper->AttachToClosestGnb(ueArSector3NetDev, gnbSector3NetDev);
        nrHelper->AttachToClosestGnb(ueVrSector3NetDev, gnbSector3NetDev);
        nrHelper->AttachToClosestGnb(ueCgSector3NetDev, gnbSector3NetDev);
    }

    // Install sink application
    ApplicationContainer serverApps;

    // configure the transport protocol to be used
    std::string transportProtocol;
    transportProtocol = useUdp ? "ns3::UdpSocketFactory" : "ns3::TcpSocketFactory";

    // DL
    uint16_t dlPortArStart = 1121; // AR has 3 flows
    uint16_t dlPortArStop = 1124;
    uint16_t dlPortVrStart = 1131; // VR Traffic (1 flow)
    uint16_t dlPortCgStart = 1141; // CG Traffic (1 flow)

    uint16_t dlPortVoiceStart = 1254; // VoIP Traffic (1 flow)

    // UL
    uint16_t ulPortArStart = 2121; // AR has 3 flows
    uint16_t ulPortArStop = 2124;
    uint16_t ulPortVoiceStart = 2254; // VoIP has 1 flow

    // The bearer that will carry AR traffic (QCI80)
    NrEpsBearer arBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);

    Ptr<NrEpcTft> arTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfAr;
    std::vector<Ptr<NrEpcTft>> arTfts;

    if (isMx1)
    {
        dlpfAr.localPortStart = dlPortArStart;
        dlpfAr.localPortEnd = dlPortArStop;
        arTft->Add(dlpfAr);
    }
    else
    {
        // create 3 xrTfts for 1x1 mapping
        for (uint32_t i = 0; i < 3; i++)
        {
            Ptr<NrEpcTft> tempTft = Create<NrEpcTft>();
            dlpfAr.localPortStart = dlPortArStart + i;
            dlpfAr.localPortEnd = dlPortArStart + i;
            tempTft->Add(dlpfAr);
            arTfts.push_back(tempTft);
        }
    }

    // The bearer that will carry VR traffic (can be QCI80/QCI87)
    NrEpsBearer vrConfig =
        enableInterServ == 0 ? NrEpsBearer::NGBR_LOW_LAT_EMBB : NrEpsBearer::DGBR_INTER_SERV_87;
    NrEpsBearer vrBearer(vrConfig);

    Ptr<NrEpcTft> vrTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfVr;
    dlpfVr.localPortStart = dlPortVrStart;
    dlpfVr.localPortEnd = dlPortVrStart;
    vrTft->Add(dlpfVr);

    // The bearer that will carry CG traffic (QCI80)
    NrEpsBearer cgBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);

    Ptr<NrEpcTft> cgTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfCg;
    dlpfCg.localPortStart = dlPortCgStart;
    dlpfCg.localPortEnd = dlPortCgStart;
    cgTft->Add(dlpfCg);

    // The bearer that will carry VoIP traffic
    NrEpsBearer voiceBearer(NrEpsBearer::GBR_CONV_VOICE);

    Ptr<NrEpcTft> voiceTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfVoice;
    dlpfVoice.localPortStart = dlPortVoiceStart;
    dlpfVoice.localPortEnd = dlPortVoiceStart;
    voiceTft->Add(dlpfVoice);

    // UL
    //  The bearer that will carry UL AR traffic (QCI80)
    NrEpsBearer arUlBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);

    Ptr<NrEpcTft> arUlTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter ulpfAr;
    std::vector<Ptr<NrEpcTft>> arUlTfts;

    if (isMx1)
    {
        ulpfAr.localPortStart = ulPortArStart;
        ulpfAr.localPortEnd = ulPortArStop;
        ulpfAr.direction = NrEpcTft::UPLINK;
        arUlTft->Add(ulpfAr);
    }
    else
    {
        // create 3 xrTfts for 1x1 mapping
        for (uint32_t i = 0; i < 3; i++)
        {
            Ptr<NrEpcTft> tempTft = Create<NrEpcTft>();
            ulpfAr.localPortStart = ulPortArStart + i;
            ulpfAr.localPortEnd = ulPortArStart + i;
            ulpfAr.direction = NrEpcTft::UPLINK;
            tempTft->Add(ulpfAr);
            arUlTfts.push_back(tempTft);
        }
    }

    // The bearer that will carry UL VoIP traffic
    NrEpsBearer voiceUlBearer(NrEpsBearer::GBR_CONV_VOICE);

    Ptr<NrEpcTft> voiceUlTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter ulpfVoice;
    ulpfVoice.localPortStart = ulPortVoiceStart;
    ulpfVoice.localPortEnd = ulPortVoiceStart;
    ulpfVoice.direction = NrEpcTft::UPLINK;
    voiceUlTft->Add(ulpfVoice);

    // Install traffic generators
    ApplicationContainer clientApps;
    ApplicationContainer pingApps;

    auto sectorContainers =
        std::vector<std::tuple<NodeContainer, NetDeviceContainer, Ipv4InterfaceContainer>>{
            {ueVoiceSector1Container, ueVoiceSector1NetDev, ueVoiceSector1IpIface},
            {ueVoiceSector2Container, ueVoiceSector2NetDev, ueVoiceSector2IpIface},
            {ueVoiceSector3Container, ueVoiceSector3NetDev, ueVoiceSector3IpIface}};

    VoiceApplicationSettings voiceAppSettings = {
        .uePort = dlPortVoiceStart,
        .transportProtocol = transportProtocol,
        .nrHelper = nrHelper,
        .bearer = voiceBearer,
        .tft = voiceTft,
        .serverApps = serverApps,
        .clientApps = clientApps,
        .pingApps = pingApps,
        .direction = "DL",

    };
    for (auto [nodeContainer, netDevContainer, ipIfaceContainer] : sectorContainers)
    {
        for (uint32_t i = 0; i < nodeContainer.GetN(); ++i)
        {
            voiceAppSettings.ue = nodeContainer.Get(i);
            voiceAppSettings.ueNetDev = netDevContainer.Get(i);
            voiceAppSettings.ueIp = ipIfaceContainer.GetAddress(i, 0);
            voiceAppSettings.remoteHost = remoteHostContainer.Get(0);
            ConfigureVoiceApp(voiceAppSettings);
        }
    }

    uint16_t remoteHostPort = 3254;
    if (enableUl)
    {
        voiceAppSettings.bearer = voiceUlBearer;
        voiceAppSettings.tft = voiceUlTft;
        voiceAppSettings.direction = "UL";
        for (auto [nodeContainer, netDevContainer, ipIfaceContainer] : sectorContainers)
        {
            for (uint32_t i = 0; i < nodeContainer.GetN(); ++i)
            {
                voiceAppSettings.uePort = remoteHostPort++;
                ConfigureVoiceApp(voiceAppSettings);
            }
        }
    }
    /*std::map<NrXrConfig, std::pair<double, std::optional<uint32_t>>> xrconfigs{
            {AR_M3, {arDataRate, arFps}},
            {VR_DL1, {vrDataRate, vrFps}},
            {CG_DL1, {cgDataRate, {}}},
            };
     for (auto config: xrconfigs)
     {
    */

    for (uint32_t i = 0; i < ueArSector1Container.GetN(); ++i)
    {
        ConfigureXrApp(ueArSector1Container,
                       i,
                       ueArSector1IpIface,
                       AR_M3,
                       dlPortArStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueArSector1NetDev,
                       nrHelper,
                       arBearer,
                       arTft,
                       isMx1,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);

        /* Ptr<TrafficGenerator3gppGenericVideo> app =
         DynamicCast<TrafficGenerator3gppGenericVideo>(currentUeClientApps.Get(j));

         app->SetAttribute("DataRate", DoubleValue(xrconfigs.at(config).first));
         if(!xrconfigs.at(config).second.empty())
         {
         app->SetAttribute("Fps", DoubleValue(xrconfigs.at(config).second));
         }*/
    }
    for (uint32_t i = 0; i < ueArSector2Container.GetN(); ++i)
    {
        ConfigureXrApp(ueArSector2Container,
                       i,
                       ueArSector2IpIface,
                       AR_M3,
                       dlPortArStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueArSector2NetDev,
                       nrHelper,
                       arBearer,
                       arTft,
                       isMx1,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }
    for (uint32_t i = 0; i < ueArSector3Container.GetN(); ++i)
    {
        ConfigureXrApp(ueArSector3Container,
                       i,
                       ueArSector3IpIface,
                       AR_M3,
                       dlPortArStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueArSector3NetDev,
                       nrHelper,
                       arBearer,
                       arTft,
                       isMx1,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }

    if (enableUl)
    {
        remoteHostPort = 4121;
        for (uint32_t i = 0; i < ueArSector1Container.GetN(); ++i)
        {
            ConfigureXrApp(ueArSector1Container,
                           i,
                           ueArSector1IpIface,
                           AR_M3,
                           ulPortArStart,
                           transportProtocol,
                           remoteHostContainer,
                           ueArSector1NetDev,
                           nrHelper,
                           arUlBearer,
                           arUlTft,
                           isMx1,
                           arUlTfts,
                           serverApps,
                           clientApps,
                           pingApps,
                           "UL",
                           arDataRate,
                           arFps,
                           vrDataRate,
                           vrFps,
                           cgDataRate,
                           internetIpIfaces.GetAddress(1),
                           remoteHostPort);
            remoteHostPort += 3;
        }
        for (uint32_t i = 0; i < ueArSector2Container.GetN(); ++i)
        {
            ConfigureXrApp(ueArSector2Container,
                           i,
                           ueArSector2IpIface,
                           AR_M3,
                           ulPortArStart,
                           transportProtocol,
                           remoteHostContainer,
                           ueArSector2NetDev,
                           nrHelper,
                           arUlBearer,
                           arUlTft,
                           isMx1,
                           arUlTfts,
                           serverApps,
                           clientApps,
                           pingApps,
                           "UL",
                           arDataRate,
                           arFps,
                           vrDataRate,
                           vrFps,
                           cgDataRate,
                           internetIpIfaces.GetAddress(1),
                           remoteHostPort);
            remoteHostPort += 3;
        }
        for (uint32_t i = 0; i < ueArSector3Container.GetN(); ++i)
        {
            ConfigureXrApp(ueArSector3Container,
                           i,
                           ueArSector3IpIface,
                           AR_M3,
                           ulPortArStart,
                           transportProtocol,
                           remoteHostContainer,
                           ueArSector3NetDev,
                           nrHelper,
                           arUlBearer,
                           arUlTft,
                           isMx1,
                           arUlTfts,
                           serverApps,
                           clientApps,
                           pingApps,
                           "UL",
                           arDataRate,
                           arFps,
                           vrDataRate,
                           vrFps,
                           cgDataRate,
                           internetIpIfaces.GetAddress(1),
                           remoteHostPort);
            remoteHostPort += 3;
        }
    }

    for (uint32_t i = 0; i < ueVrSector1Container.GetN(); ++i)
    {
        ConfigureXrApp(ueVrSector1Container,
                       i,
                       ueVrSector1IpIface,
                       VR_DL1,
                       dlPortVrStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueVrSector1NetDev,
                       nrHelper,
                       vrBearer,
                       vrTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }
    for (uint32_t i = 0; i < ueVrSector2Container.GetN(); ++i)
    {
        ConfigureXrApp(ueVrSector2Container,
                       i,
                       ueVrSector2IpIface,
                       VR_DL1,
                       dlPortVrStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueVrSector2NetDev,
                       nrHelper,
                       vrBearer,
                       vrTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }
    for (uint32_t i = 0; i < ueVrSector3Container.GetN(); ++i)
    {
        ConfigureXrApp(ueVrSector3Container,
                       i,
                       ueVrSector3IpIface,
                       VR_DL1,
                       dlPortVrStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueVrSector3NetDev,
                       nrHelper,
                       vrBearer,
                       vrTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }

    for (uint32_t i = 0; i < ueCgSector1Container.GetN(); ++i)
    {
        ConfigureXrApp(ueCgSector1Container,
                       i,
                       ueCgSector1IpIface,
                       CG_DL1,
                       dlPortCgStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueCgSector1NetDev,
                       nrHelper,
                       cgBearer,
                       cgTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }
    for (uint32_t i = 0; i < ueCgSector2Container.GetN(); ++i)
    {
        ConfigureXrApp(ueCgSector2Container,
                       i,
                       ueCgSector2IpIface,
                       CG_DL1,
                       dlPortCgStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueCgSector2NetDev,
                       nrHelper,
                       cgBearer,
                       cgTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }
    for (uint32_t i = 0; i < ueCgSector3Container.GetN(); ++i)
    {
        ConfigureXrApp(ueCgSector3Container,
                       i,
                       ueCgSector3IpIface,
                       CG_DL1,
                       dlPortCgStart,
                       transportProtocol,
                       remoteHostContainer,
                       ueCgSector3NetDev,
                       nrHelper,
                       cgBearer,
                       cgTft,
                       true,
                       arTfts,
                       serverApps,
                       clientApps,
                       pingApps,
                       "DL",
                       arDataRate,
                       arFps,
                       vrDataRate,
                       vrFps,
                       cgDataRate,
                       internetIpIfaces.GetAddress(1),
                       0);
    }

    pingApps.Start(MilliSeconds(100));
    pingApps.Stop(appStartTimeMs);

    // start server and client apps
    serverApps.Start(appStartTimeMs);
    clientApps.Start(appStartTimeMs);
    serverApps.Stop(simTimeMs);
    clientApps.Stop(appStartTimeMs + appDuration);

    // enable the traces provided by the nr module
    if (enableNrHelperTraces)
    {
        nrHelper->EnableTraces();
    }

    for (NetDeviceContainer::Iterator i = gnbSector1NetDev.Begin(); i != gnbSector1NetDev.End();
         ++i)
    {
        Ptr<NrGnbNetDevice> gnbNetDev = DynamicCast<NrGnbNetDevice>(*i);
        gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("RequiredFhDlThroughput",
                                                                MakeCallback(&ReportFhTrace));
        gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("UsedAirRbs",
                                                                MakeCallback(&ReportAiTrace));
    }

    if (deployment == "HEX")
    {
        for (NetDeviceContainer::Iterator i = gnbSector2NetDev.Begin(); i != gnbSector2NetDev.End();
             ++i)
        {
            Ptr<NrGnbNetDevice> gnbNetDev = DynamicCast<NrGnbNetDevice>(*i);
            gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("RequiredFhDlThroughput",
                                                                    MakeCallback(&ReportFhTrace));
            gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("UsedAirRbs",
                                                                    MakeCallback(&ReportAiTrace));
        }

        for (NetDeviceContainer::Iterator i = gnbSector3NetDev.Begin(); i != gnbSector3NetDev.End();
             ++i)
        {
            Ptr<NrGnbNetDevice> gnbNetDev = DynamicCast<NrGnbNetDevice>(*i);
            gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("RequiredFhDlThroughput",
                                                                    MakeCallback(&ReportFhTrace));
            gnbNetDev->GetNrFhControl()->TraceConnectWithoutContext("UsedAirRbs",
                                                                    MakeCallback(&ReportAiTrace));
        }
    }

    // REM
    Ptr<NrRadioEnvironmentMapHelper> remHelper;

    if (dlRem)
    {
        std::cout << "  rem helper\n";
        uint16_t remPhyIndex = 0;

        NetDeviceContainer remNd;
        Ptr<NetDevice> remDevice;

        std::vector<NetDeviceContainer*> remNdBySector{gnbNdBySector};
        std::vector<NetDeviceContainer*> remDevBySector{ueNdBySector};

        uint32_t sectorIndex = 0;
        // Reverse order so we get sector 1 for the remSector == 0 case
        for (uint32_t sector = sectors; sector > 0; --sector)
        {
            if (remSector == sector || remSector == 0)
            {
                sectorIndex = sector - 1;
                remNd.Add(*remNdBySector[sectorIndex]);
                remDevice = remDevBySector[sectorIndex]->Get(0);
            }
        }

        // Radio Environment Map Generation for ccId 0
        remHelper = CreateObject<NrRadioEnvironmentMapHelper>();
        remHelper->SetMinX(xMinRem);
        remHelper->SetMaxX(xMaxRem);
        remHelper->SetResX(xResRem);
        remHelper->SetMinY(yMinRem);
        remHelper->SetMaxY(yMaxRem);
        remHelper->SetResY(yResRem);
        remHelper->SetZ(zRem);

        // save beamforming vectors, one per site (?)
        for (uint32_t sector = sectors; sector > 0; --sector)
        {
            if ((remSector == sector) || (remSector == 0))
            {
                sectorIndex = sector - 1;
                for (uint32_t siteId = 0; siteId < gnbSites; ++siteId)
                {
                    gnbNdBySector[sectorIndex]
                        ->Get(siteId)
                        ->GetObject<NrGnbNetDevice>()
                        ->GetPhy(remPhyIndex)
                        ->ChangeBeamformingVector(
                            DynamicCast<NrUeNetDevice>(ueNdBySector[sectorIndex]->Get(siteId)));
                }
            }
        }

        remHelper->CreateRem(remNd, remDevice, remPhyIndex);
    }

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(ueNodes);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.0001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(simTimeMs);

    std::cout << "Run simulation" << std::endl;

    Simulator::Run();

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream delayFile;
    std::ofstream throughputFile;

    std::ostringstream delayFileName;
    std::ostringstream throughputFileName;
    if (simTag.empty())
    {
        delayFileName << "XR_Delay"
                      << "_ar_" << std::to_string(arUeNum).c_str() << "_vr_"
                      << std::to_string(vrUeNum).c_str() << "_cg_"
                      << std::to_string(cgUeNum).c_str() << "_voice_"
                      << std::to_string(voiceUeNum).c_str() << "_" << schedulerType.c_str()
                      << "_Mx1_" << isMx1 << ".txt";

        throughputFileName << "XR_Throughput"
                           << "_ar_" << std::to_string(arUeNum).c_str() << "_vr_"
                           << std::to_string(vrUeNum).c_str() << "_cg_"
                           << std::to_string(cgUeNum).c_str() << "_voice_"
                           << std::to_string(voiceUeNum).c_str() << "_" << schedulerType.c_str()
                           << "_Mx1_" << isMx1 << ".txt";
    }
    else
    {
        delayFileName << outputDir << "Delay_" << simTag << std::string(".txt").c_str();
        throughputFileName << outputDir << "Throughput_" << simTag << std::string(".txt").c_str();
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

        const Time& txDuration = appDuration;
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                  << t.destinationAddress << ":" << t.destinationPort << ") proto "
                  << protoStream.str() << "\n";
        std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
        std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
        std::cout << "  TxOffered:  "
                  << ((i->second.txBytes * 8.0) / txDuration.GetSeconds()) * 1e-6 << " Mbps\n";
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";

        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            Time rxDuration = Seconds(0);
            if (t.protocol == 6) // tcp
            {
                rxDuration = appDuration;
            }
            else if (t.protocol == 17) // udp
            {
                // rxDuration = i->second.timeLastRxPacket - i->second.timeFirstTxPacket;
                rxDuration = appDuration + MilliSeconds(10);
            }
            else
            {
                continue;
            }

            // Time rxDuration = i->second.timeLastRxPacket - i->second.timeFirstRxPacket;
            averageFlowThroughput += ((i->second.rxBytes * 8.0) / rxDuration.GetSeconds()) * 1e-6;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;

            double throughput = ((i->second.rxBytes * 8.0) / rxDuration.GetSeconds()) * 1e-6;
            double delay = 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;
            double jitter = 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets;

            std::cout << "  Throughput: " << throughput << " Mbps\n";
            std::cout << "  Mean delay:  " << delay << " ms\n";
            std::cout << "  Mean jitter:  " << jitter << " ms\n";

            if (enableQosTrafficTraces)
            {
                throughputFile << t.sourcePort << "\t" << t.destinationPort << "\t" << throughput
                               << "\t" << delay << std::endl;
            }
        }
        else
        {
            std::cout << "  Throughput:  0 Mbps\n";
            std::cout << "  Mean delay:  0 ms\n";
            std::cout << "  Mean upt:  0  Mbps \n";
            std::cout << "  Mean jitter: 0 ms\n";

            if (enableQosTrafficTraces)
            {
                throughputFile << t.sourcePort << "\t" << t.destinationPort << "\t" << 0 << "\t"
                               << 0 << std::endl;
            }
        }
        std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    delayFile.close();
    throughputFile.close();

    std::cout << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size()
              << "Mbps \n";
    std::cout << "  Mean flow delay: " << averageFlowDelay / stats.size() << " ms\n";

    Simulator::Destroy();
    return 0;
}

static void
PrintUePosition(NodeContainer ueNodes)
{
    std::ofstream outUePositionsFile;
    std::string filenameUePositions = "uePositions.txt";

    outUePositionsFile.open(filenameUePositions.c_str());
    outUePositionsFile.setf(std::ios_base::fixed);

    if (!outUePositionsFile.is_open())
    {
        NS_ABORT_MSG("Can't open file " << filenameUePositions);
    }

    for (uint32_t ueId = 0; ueId < ueNodes.GetN(); ++ueId)
    {
        Vector uepos = ueNodes.Get(ueId)->GetObject<MobilityModel>()->GetPosition();
        outUePositionsFile << "ueId: " << ueId << ", at " << uepos << std::endl;
    }

    outUePositionsFile.close();
}

void
ConfigureBwpTo(BandwidthPartInfoPtr& bwp, double centerFreq, double bwpBw)
{
    bwp->m_centralFrequency = centerFreq;
    bwp->m_higherFrequency = centerFreq + (bwpBw / 2);
    bwp->m_lowerFrequency = centerFreq - (bwpBw / 2);
    bwp->m_channelBandwidth = bwpBw;
}

void
ConfigurePhy(Ptr<NrHelper> nrHelper,
             Ptr<NetDevice> gnb,
             double orientationRads,
             uint16_t beamConfSector,
             double beamConfElevation)
{
    // Change the antenna orientation
    Ptr<NrGnbPhy> phy0 = nrHelper->GetGnbPhy(gnb, 0); // BWP 0
    Ptr<UniformPlanarArray> antenna0 = ConstCast<UniformPlanarArray>(
        phy0->GetSpectrumPhy()->GetAntenna()->GetObject<UniformPlanarArray>());
    antenna0->SetAttribute("BearingAngle", DoubleValue(orientationRads));

    // configure the beam that points toward the center of hexagonal
    // In case of beamforming, it will be overwritten.
    phy0->GetSpectrumPhy()->GetBeamManager()->SetPredefinedBeam(beamConfSector, beamConfElevation);
}

void
ConfigureXrApp(NodeContainer& ueContainer,
               uint32_t i,
               Ipv4InterfaceContainer& ueIpIface,
               enum NrXrConfig config,
               uint16_t uePort,
               std::string transportProtocol,
               NodeContainer& remoteHostContainer,
               NetDeviceContainer& ueNetDev,
               Ptr<NrHelper> nrHelper,
               NrEpsBearer& bearer,
               Ptr<NrEpcTft> tft,
               bool isMx1,
               std::vector<Ptr<NrEpcTft>>& tfts,
               ApplicationContainer& serverApps,
               ApplicationContainer& clientApps,
               ApplicationContainer& pingApps,
               std::string direction,
               double arDataRate,
               uint16_t arFps,
               double vrDataRate,
               uint16_t vrFps,
               double cgDataRate,
               Ipv4Address remoteHostAddress,
               uint16_t remoteHostPort)
{
    XrTrafficMixerHelper trafficMixerHelper;
    Ipv4Address ipAddress = ueIpIface.GetAddress(i, 0);
    trafficMixerHelper.ConfigureXr(config);
    auto it = XrPreconfig.find(config);

    Ipv4Address address = direction == "UL" ? remoteHostAddress : ipAddress;
    uint16_t port = direction == "UL" ? remoteHostPort : uePort;

    std::vector<Address> addresses;
    std::vector<InetSocketAddress> localAddresses;

    for (uint j = 0; j < it->second.size(); j++)
    {
        addresses.push_back(InetSocketAddress(address, port + j));
        // The sink will always listen to the specified ports
        localAddresses.emplace_back(Ipv4Address::GetAny(), port + j);
    }

    ApplicationContainer currentUeClientApps;

    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    PingHelper ping(address);

    if (direction == "UL")
    {
        pingApps.Add(ping.Install(ueContainer.Get(i)));
        currentUeClientApps.Add(
            trafficMixerHelper.Install(transportProtocol, addresses, ueContainer.Get(i)));
    }
    else
    {
        pingApps.Add(ping.Install(remoteHostContainer));
        currentUeClientApps.Add(
            trafficMixerHelper.Install(transportProtocol, addresses, remoteHostContainer.Get(0)));
    }

    Ptr<NetDevice> ueDevice = ueNetDev.Get(i);

    // Activate a dedicated bearer for the traffic type per node
    nrHelper->ActivateDedicatedEpsBearer(ueDevice, bearer, tft);

    // Activate a dedicated bearer for the traffic type per node
    if (isMx1)
    {
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, bearer, tft);
    }
    else
    {
        NS_ASSERT(tfts.size() >= currentUeClientApps.GetN());
        for (uint32_t j = 0; j < currentUeClientApps.GetN(); j++)
        {
            nrHelper->ActivateDedicatedEpsBearer(ueDevice, bearer, tfts[j]);
        }
    }

    for (uint32_t j = 0; j < currentUeClientApps.GetN(); j++)
    {
        PacketSinkHelper dlPacketSinkHelper(transportProtocol, localAddresses.at(j));
        Ptr<Application> packetSink;
        if (direction == "UL")
        {
            packetSink = dlPacketSinkHelper.Install(remoteHostContainer.Get(0)).Get(0);
        }
        else
        {
            packetSink = dlPacketSinkHelper.Install(ueContainer.Get(i)).Get(0);
        }

        serverApps.Add(packetSink);

        Ptr<TrafficGenerator3gppGenericVideo> app =
            DynamicCast<TrafficGenerator3gppGenericVideo>(currentUeClientApps.Get(j));
        if (app && config == NrXrConfig::AR_M3)
        {
            app->SetAttribute("DataRate", DoubleValue(arDataRate));
            app->SetAttribute("Fps", UintegerValue(arFps));
        }
        else if (app && config == NrXrConfig::VR_DL1)
        {
            app->SetAttribute("DataRate", DoubleValue(vrDataRate));
            app->SetAttribute("Fps", UintegerValue(vrFps));
        }
        else if (app && config == NrXrConfig::CG_DL1)
        {
            app->SetAttribute("DataRate", DoubleValue(cgDataRate));
        }
    }
    clientApps.Add(currentUeClientApps);
}

void
ConfigureVoiceApp(VoiceApplicationSettings& voiceAppSettings)
{
    Ipv4Address ipAddress = voiceAppSettings.ueIp;
    Ipv4Address address =
        voiceAppSettings.direction == "UL" ? voiceAppSettings.remoteHostAddress : ipAddress;
    uint16_t port = voiceAppSettings.direction == "UL" ? voiceAppSettings.remoteHostPort
                                                       : voiceAppSettings.uePort;

    TrafficGeneratorHelper trafficGeneratorHelper(voiceAppSettings.transportProtocol,
                                                  InetSocketAddress(address, port),
                                                  TrafficGeneratorNgmnVoip::GetTypeId());

    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    PingHelper ping(ipAddress);

    if (voiceAppSettings.direction == "UL")
    {
        voiceAppSettings.clientApps.Add(trafficGeneratorHelper.Install(voiceAppSettings.ue).Get(0));
        voiceAppSettings.pingApps.Add(ping.Install(voiceAppSettings.ue));
    }
    else
    {
        voiceAppSettings.clientApps.Add(
            trafficGeneratorHelper.Install(voiceAppSettings.remoteHost));
        voiceAppSettings.pingApps.Add(ping.Install(voiceAppSettings.remoteHost));
    }

    Ptr<NetDevice> ueDevice = voiceAppSettings.ueNetDev;
    // Activate a dedicated bearer for the traffic type per node
    voiceAppSettings.nrHelper->ActivateDedicatedEpsBearer(ueDevice,
                                                          voiceAppSettings.bearer,
                                                          voiceAppSettings.tft);

    InetSocketAddress localAddress(Ipv4Address::GetAny(), port);
    PacketSinkHelper dlPacketSinkHelper(voiceAppSettings.transportProtocol, localAddress);
    Ptr<Application> packetSink;
    if (voiceAppSettings.direction == "UL")
    {
        packetSink = dlPacketSinkHelper.Install(voiceAppSettings.remoteHost).Get(0);
    }
    else
    {
        packetSink = dlPacketSinkHelper.Install(voiceAppSettings.ue).Get(0);
    }

    voiceAppSettings.serverApps.Add(packetSink);
}

void
ReportFhTrace(const SfnSf& sfn, uint16_t physCellId, uint16_t bwpId, uint64_t reqFh)
{
    if (!m_fhTraceFile.is_open())
    {
        std::stringstream fileName;
        fileName << m_outputDir << "fh-trace_" << m_fhControlMethod.c_str() << "_"
                 << std::to_string(m_fhCapacity) << ".txt";
        m_fhTraceFileName = fileName.str();
        m_fhTraceFile.open(m_fhTraceFileName.c_str());

        if (!m_fhTraceFile.is_open())
        {
            NS_FATAL_ERROR("Could not open FH tracefile");
        }

        m_fhTraceFile << "CellId"
                      << "\t"
                      << "BwpId"
                      << "\t"
                      << "FhThroughput"
                      << "\n";
    }
    m_fhTraceFile << physCellId << "\t" << bwpId << "\t" << reqFh << std::endl;
}

void
ReportAiTrace(const SfnSf& sfn, uint16_t physCellId, uint16_t bwpId, uint32_t airRbs)
{
    if (!m_aiTraceFile.is_open())
    {
        std::stringstream fileName;
        fileName << m_outputDir << "air-trace_" << m_fhControlMethod.c_str() << "_"
                 << std::to_string(m_fhCapacity) << ".txt";
        m_aiTraceFileName = fileName.str();
        m_aiTraceFile.open(m_aiTraceFileName.c_str());

        if (!m_aiTraceFile.is_open())
        {
            NS_FATAL_ERROR("Could not open Air tracefile");
        }
    }
    m_aiTraceFile << physCellId << "\t" << bwpId << "\t" << airRbs << std::endl;
}
