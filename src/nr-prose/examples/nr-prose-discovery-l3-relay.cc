//
// SPDX-License-Identifier: NIST-Software
//

/**
 * \ingroup examples
 * \file nr-prose-discovery-l3-relay.cc
 * \brief A simple example using ProSe Relay Discovery, with a sidelink
 *        configuration based on cttc-nr-v2x-demo-simple.cc example
 *
 * Channel configuration:
 * This example setups an NR sidelink out-of-coverage simulation using the 3GPP
 * channel model from TR 37.885.
 *
 * System configuration:
 * Sidelink will use one operational band, containing one component carrier,
 * and a single bandwidth part centered at the frequency specified by the
 * corresponding input parameter. The system bandwidth, and the numerology to
 * be used and the transmission power can be setup as well.
 *
 *
 * Topology:
 * There are 2 UEs in the topology located with an inter-UE distance specified
 * by the corresponding input parameter. Default configuration:
 *          UE1..........(20 m)..........UE2
 *   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/stats-module.h"

#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrProseDiscoveryL3Relay");

/*
 * Global methods and variable to hook trace sources from different layers of
 * the protocol stack.
 */

/**
 * \brief Method to listen to the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * \param pscchStats Pointer to the \link UeMacPscchTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchScheduling(UeMacPscchTxOutputStats* pscchStats,
                        const SlPscchUeMacStatParameters pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

/**
 * \brief Method to listen to the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * \param psschStats Pointer to the \link UeMacPsschTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschScheduling(UeMacPsschTxOutputStats* psschStats,
                        const SlPsschUeMacStatParameters psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

/**
 * \brief Method to listen to the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * \param pscchStats Pointer to the \link UePhyPscchRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchRx(UePhyPscchRxOutputStats* pscchStats,
                const SlRxCtrlPacketTraceParams pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

/**
 * \brief Method to listen to the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * \param psschStats Pointer to the \link UePhyPsschRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschRx(UePhyPsschRxOutputStats* psschStats,
                const SlRxDataPacketTraceParams psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

int
main(int argc, char* argv[])
{
    // Topology parameters
    uint16_t ueNum = 2;            // Number of UEs in the simulation
    uint16_t interUeDistance = 20; // meters

    // Simulation timeline parameters
    Time simTime = Seconds(10.0);      // Total simulation time
    Time startDiscTime = Seconds(2.0); // Time to start the Prose discovery procedure in seconds
    Time stopDiscTime = Seconds(50.0); // Time to stop the Prose discovery procedure in seconds
    Time discInterval = Seconds(2.0);  // Interval between two discovery announcements

    // NR parameters
    uint16_t numerologyBwpSl = 2;           // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    uint16_t bandwidthBandSl = 400;         // Multiple of 100 KHz; 400 = 40 MHz
    double txPower = 23;                    // dBm

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    CommandLine cmd;
    cmd.AddValue("interUeDistance", "The distance among the UEs in the topology", interUeDistance);
    cmd.AddValue("simTime", "Simulation time in seconds", simTime);
    cmd.AddValue("startDiscTime",
                 "Time to start the Prose discovery procedure in seconds",
                 startDiscTime);
    cmd.AddValue("stopDiscTime",
                 "Time to stop the Prose discovery procedure in seconds",
                 stopDiscTime);
    cmd.AddValue("discInterval",
                 "Interval between two Prose discovery announcements in seconds",
                 discInterval);
    cmd.AddValue("numerologyBwpSl",
                 "The numerology to be used in sidelink bandwidth part",
                 numerologyBwpSl);
    cmd.AddValue("centralFrequencyBandSl",
                 "The central frequency to be used for sidelink band/channel",
                 centralFrequencyBandSl);
    cmd.AddValue("bandwidthBandSl",
                 "The system bandwidth to be used for sidelink",
                 bandwidthBandSl);
    cmd.AddValue("txPower", "total tx power in dBm", txPower);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);

    // Parse the command line
    cmd.Parse(argc, argv);

    // ConfigStore
    ConfigStore config;
    config.ConfigureDefaults();

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Discovery Frequency
    Config::SetDefault("ns3::NrSlUeProse::DiscoveryInterval", TimeValue(discInterval));

    // UE nodes creation
    NodeContainer ueVoiceContainer;
    ueVoiceContainer.Create(ueNum);

    // UE nodes mobility setup
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < ueNum; i++)
    {
        positionAllocUe->Add(Vector(interUeDistance * i, 0.0, 1.5));
    }
    mobility.SetPositionAllocator(positionAllocUe);
    mobility.Install(ueVoiceContainer);

    /*
     * Setup the NR module. We create  NrSlHelper, which takes care of
     * creating and connecting the various parts of the NR stack
     */
    Ptr<NrSlHelper> nrSlHelper = CreateObject<NrSlHelper>();

    // The sidelink fast fading error model is installed in the NrSlSpectrumPhy
    // and needs to be configured separately from the above loss model.
    // The default is a static channel, but the NrSlEpaErrorModel may also be
    // configured for a fading channel
    nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

    /*
     * Spectrum division. We create one operational band, containing
     * one component carrier, and a single bandwidth part
     * centered at the frequency specified by the input parameters.
     */
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1;

    // Create the configuration for the CcBwpHelper. SimpleOperationBandConfcreates a single BWP per
    // CC
    CcBwpCreator::SimpleOperationBandConf bandConfSl(centralFrequencyBandSl,
                                                     bandwidthBandSl,
                                                     numCcPerBand);

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo bandSl = ccBwpCreator.CreateOperationBandContiguousCc(bandConfSl);

#if 0
    // Configure 3GPP channel model
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(100)));
    nrSlHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrSlHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));

    /*
     * Initialize channel and pathloss, plus other things inside bandSl. If needed,
     * the band configuration can be done manually, but we leave it for more
     * sophisticated examples. For the moment, this method will take care
     * of all the spectrum initialization needs.
     */
    nrSlHelper->InitializeOperationBand(&bandSl);
#endif

    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    channelHelper->ConfigureFactories("V2V-Highway", "Default", "ThreeGpp");
    /**
     * Use channelHelper API to define the attributes for the channel model (condition, pathloss and
     * spectrum)
     */
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    channelHelper->AssignChannelsToBands({bandSl});

    allBwps = CcBwpCreator::GetAllBwps({bandSl});

    Packet::EnableChecking();
    Packet::EnablePrinting();

    /*
     * Antennas for all the UEs
     * We are not using beamforming in SL, rather we are using
     * quasi-omnidirectional transmission and reception, which is the default
     * configuration of the beams.
     */
    nrSlHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("AntennaElement",
                                      PointerValue(CreateObject<IsotropicAntennaModel>()));

    nrSlHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));

    // NR Sidelink attribute of UE MAC, which are would be common for all the UEs
    nrSlHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrSlHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrSlHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));

    uint8_t bwpIdForGbrMcptt = 0;

    // following parameter has no impact at the moment because:
    // 1. No support for PQI based mapping between the application and the LCs
    // 2. No scheduler to consider PQI
    // However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
    // because we hard coded the PQI 65 in UE RRC.
    nrSlHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK",
                                                  UintegerValue(bwpIdForGbrMcptt));

    std::set<uint8_t> bwpIdContainer;
    bwpIdContainer.insert(bwpIdForGbrMcptt);

    NetDeviceContainer ueVoiceNetDev = nrSlHelper->InstallUeDevice(ueVoiceContainer, allBwps);

    /*
     * Set the SL scheduler attributes
     * In this example we use NrSlUeMacSchedulerDefault scheduler, which uses
     * fix MCS value and schedules logical channels by priority order first and
     * then by creation order
     */
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(14));

    /*
     * Very important method to configure UE protocol stack, i.e., it would
     * configure all the SAPs among the layers, setup callbacks, configure
     * error model, configure AMC, and configure ChunkProcessor in Interference
     * API.
     */
    nrSlHelper->PrepareUeForSidelink(ueVoiceNetDev, bwpIdContainer);

    /*
     * Start preparing for all the sub Structs/RRC Information Element (IEs)
     * of NrSlRrcSap::SidelinkPreconfigNr. This is the main structure, which would
     * hold all the pre-configuration related to Sidelink.
     */

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    // get it from pool factory
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    // Configure specific parameters of interest:
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(3);
    std::list<uint16_t> resourceReservePeriodList = {0, 100}; // in ms
    ptrFactory->SetSlResourceReservePeriodList(resourceReservePeriodList);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which holds a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    // Pool id, ranges from 0 to 15
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which holds an array of pools
    NrSlRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
    // Array for pools, we insert the pool in the array as per its poolId
    slBwpPoolConfigCommonNr.slTxPoolSelectedNormal[slResourcePoolIdNr.id] = slresoPoolConfigNr;

    // Configure the BWP IE
    NrSlRrcSap::Bwp bwp;
    bwp.numerology = numerologyBwpSl;
    bwp.symbolsPerSlots = 14;
    bwp.rbPerRbg = 1;
    bwp.bandwidth = bandwidthBandSl;

    // Configure the SlBwpGeneric IE
    NrSlRrcSap::SlBwpGeneric slBwpGeneric;
    slBwpGeneric.bwp = bwp;
    slBwpGeneric.slLengthSymbols = NrSlRrcSap::GetSlLengthSymbolsEnum(14);
    slBwpGeneric.slStartSymbol = NrSlRrcSap::GetSlStartSymbolEnum(0);

    // Configure the SlBwpConfigCommonNr IE
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
    slBwpConfigCommonNr.haveSlBwpGeneric = true;
    slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
    slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
    slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

    // Configure the SlFreqConfigCommonNr IE, which holds the array to store
    // the configuration of all Sidelink BWP (s).
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
    // Array for BWPs. Here we will iterate over the BWPs, which
    // we want to use for SL.
    for (const auto& it : bwpIdContainer)
    {
        // it is the BWP id
        slFreConfigCommonNr.slBwpList[it] = slBwpConfigCommonNr;
    }

    // Configure the TddUlDlConfigCommon IE
    NrSlRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
    tddUlDlConfigCommon.tddPattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|";

    // Configure the SlPreconfigGeneralNr IE
    NrSlRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
    slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

    // Configure the SlUeSelectedConfig IE
    NrSlRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
    slUeSelectedPreConfig.slProbResourceKeep = 0;
    // Configure the SlPsschTxParameters IE
    NrSlRrcSap::SlPsschTxParameters psschParams;
    psschParams.slMaxTxTransNumPssch = 5;
    // Configure the SlPsschTxConfigList IE
    NrSlRrcSap::SlPsschTxConfigList pscchTxConfigList;
    pscchTxConfigList.slPsschTxParameters[0] = psschParams;
    slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

    /*
     * Finally, configure the SidelinkPreconfigNr This is the main structure
     * that needs to be communicated to NrSlUeRrc class
     */
    NrSlRrcSap::SidelinkPreconfigNr slPreConfigNr;
    slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
    slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
    slPreConfigNr.slPreconfigFreqInfoList[0] = slFreConfigCommonNr;

    // Communicate the above pre-configuration to the NrSlHelper
    nrSlHelper->InstallNrSlPreConfiguration(ueVoiceNetDev, slPreConfigNr);

    /****************************** End SL Configuration ***********************/

    /*
     * Fix the random streams
     */
    int64_t stream = 1;
    const uint64_t streamIncrement = 1000;
    nrSlHelper->AssignStreams(ueVoiceNetDev, stream);
    stream += streamIncrement;
    nrSlHelper->AssignStreams(ueVoiceNetDev, stream);

    /*
     * Configure the IPv4 stack
     */
    InternetStackHelper internet;
    internet.Install(ueVoiceContainer);
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(ueVoiceNetDev);

    /*
     * Configure ProSe
     */

    // Create ProSe helper
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    // Install ProSe layer and corresponding SAPs in the UEs
    nrSlProseHelper->PrepareUesForProse(ueVoiceNetDev);

    /*
     * Setup discovery applications
     */
    NS_LOG_INFO("Configuring discovery relay");

    // Relay Discovery
    uint32_t relayCode = 5;
    uint32_t relayDstL2Id = 500;

    Simulator::Schedule(startDiscTime,
                        &NrSlProseHelper::StartRelayDiscovery,
                        nrSlProseHelper,
                        ueVoiceNetDev.Get(0),
                        relayCode,
                        relayDstL2Id,
                        NrSlUeProse::ModelB,
                        NrSlUeProse::RelayUE);
    Simulator::Schedule(startDiscTime,
                        &NrSlProseHelper::StartRelayDiscovery,
                        nrSlProseHelper,
                        ueVoiceNetDev.Get(1),
                        relayCode,
                        relayDstL2Id,
                        NrSlUeProse::ModelB,
                        NrSlUeProse::RemoteUE);

    /*********************** End ProSe configuration ***************************/

    // Database setup
    std::string exampleName = simTag + "-" + "nr-prose-discovery-relay";
    SQLiteOutput db(outputDir + exampleName + ".db");

    UeMacPscchTxOutputStats pscchStats;
    pscchStats.SetDb(&db, "pscchTxUeMac");
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUeMac/SlPscchScheduling",
                                  MakeBoundCallback(&NotifySlPscchScheduling, &pscchStats));

    UeMacPsschTxOutputStats psschStats;
    psschStats.SetDb(&db, "psschTxUeMac");
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUeMac/SlPsschScheduling",
                                  MakeBoundCallback(&NotifySlPsschScheduling, &psschStats));

    UePhyPscchRxOutputStats pscchPhyStats;
    pscchPhyStats.SetDb(&db, "pscchRxUePhy");
    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/"
        "SpectrumPhy/RxPscchTraceUe",
        MakeBoundCallback(&NotifySlPscchRx, &pscchPhyStats));

    UePhyPsschRxOutputStats psschPhyStats;
    psschPhyStats.SetDb(&db, "psschRxUePhy");
    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/"
        "SpectrumPhy/RxPsschTraceUe",
        MakeBoundCallback(&NotifySlPsschRx, &psschPhyStats));

    UeToUePktTxRxOutputStats pktStats;
    pktStats.SetDb(&db, "pktTxRx");

    // Enable discovery traces
    nrSlProseHelper->EnableDiscoveryTraces();

    // ConfigStore
    config.ConfigureAttributes();

    Simulator::Stop(simTime);
    Simulator::Run();

    /*
     * VERY IMPORTANT: Do not forget to empty the database cache, which would
     * dump the data store towards the end of the simulation in to a database.
     */
    pktStats.EmptyCache();
    pscchStats.EmptyCache();
    psschStats.EmptyCache();
    pscchPhyStats.EmptyCache();
    psschPhyStats.EmptyCache();

    Simulator::Destroy();
    return 0;
}
