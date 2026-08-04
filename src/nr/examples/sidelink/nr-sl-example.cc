// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

/**
 * \ingroup examples
 * \file sl-network-example.cc
 *
 * This example sets up a collection of N >= 2 out-of-coverage UEs randomly
 * distributed (uniformly) within a disc.  Periodic UDP traffic is generated
 * from UE K to UE K+1 for 0 <= K < N.
 *
 * The following parameters can be varied:
 * - Traffic time (duration)
 * - number of UEs (numUe) (default 2)
 * - enableDynTraffic (default true)
 * - dynTrafficPacketSize (default 20 bytes)
 * - enableSpsTraffic (default false)
 * - spsTrafficPacketSize (default 20 bytes)
 * - psfchPeriod (0, 1, 2, 4) default 4
 * - mcs (0-28) default 0
 *
 * The following are TODO:
 * - UDP data rate (per UE)
 * - Radius of disc (position allocator)
 * - Cast type (unicast, broadcast, groupcast)
 * - Subcarrier spacing (15, 30, 45 KHz)
 * - RRI interval (for SPS)
 * - Fading model (Static or Extended Pedestrian A (EPA))
 * - HARQ model (Blind, Feedback, None)
 * - Max number of transmissions
 *
 * A configuration summary and basic statistics on packet transmissions
 * are printed at program exit.
 *
 * \code{.unparsed}
$ ./ns3 run "nr-sl-example --help"
    \endcode
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"

#include <iomanip>
#include <ostream>
#include <tuple>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SlExample");

uint32_t g_rxPktCounter = 0;            //!< Global variable to count RX packets
uint32_t g_txPktCounter = 0;            //!< Global variable to count TX packets
std::list<double> g_delays;             //!< Global list to store packet delays upon RX
std::ofstream g_fileGrantCreated;       //!< File stream for saving scheduling output
std::ofstream g_fileGrantPublished;     //!< File stream for saving scheduling output
std::ofstream g_fileSpectrumTrace;      //!< File stream for saving NrSlSpectrumPhy trace output
std::ostringstream g_firstGrantCreated; //!< String stream for saving first scheduling output
bool g_firstGrant = true;   //!< Flag to control writing first grant to g_firstGrantCreated
bool g_writeTraces = false; //!< Flag to control writing traces

/**
 * Structure to keep track of the transmission time of the packets at the
 * application layer. Used to calculate packet delay.
 */
struct PacketWithRxTimestamp
{
    Ptr<const Packet> p;
    Time txTimestamp;
};

/**
 * Map to store received packets and reception timestamps at the application
 * layer. Used to calculate packet delay at the application layer.
 */
std::map<std::string, PacketWithRxTimestamp> g_rxPacketsForDelayCalc;

// Forward declarations

/**
 * \brief Trace sink function to count and logging the transmitted data packets
 *        and their corresponding transmission timestamp at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
void TxPacketTraceForDelay(Ptr<const Packet> p,
                           const Address& srcAddrs,
                           const Address& dstAddrs,
                           const SeqTsSizeHeader& seqTsSizeHeader);
/**
 * \brief Trace sink function to count and calculate the delay upon reception
 *        of a packet at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
void RxPacketTraceForDelay(Ptr<const Packet> p,
                           const Address& srcAddrs,
                           const Address& dstAddrs,
                           const SeqTsSizeHeader& seqTsSizeHeader);

void TraceGrantCreated(std::string context,
                       const struct NrSlUeMacScheduler::GrantInfo& grantInfo,
                       uint16_t psfchPeriod);

void WriteGrantCreated(std::ostream& grantStream,
                       std::string context,
                       const struct NrSlUeMacScheduler::GrantInfo& grantInfo,
                       uint16_t psfchPeriod);

void TraceGrantPublished(std::string context,
                         const struct NrSlUeMac::NrSlGrant& grant,
                         uint16_t psfchPeriod);

void WriteGrantPublished(std::ostream& grantStream,
                         std::string context,
                         const struct NrSlUeMac::NrSlGrant& grant,
                         uint16_t psfchPeriod);

uint32_t GetPacketSize(const DataRate& dataRate, Time rri);

void TraceSlPscchDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue);

void TraceSlSci2aDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue);

void TraceSlTbDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue);

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
                                                                     Time slBearersActivationTime,
                                                                     Time finalSimTime,
                                                                     int64_t streamIndex);

int
main(int argc, char* argv[])
{
    // Scenario parameters
    uint16_t numUe = 2;
    uint16_t positionAllocatorRadius = 20; // meters
    int64_t streamIndex = 1000;
    int64_t streamIncrement = 1000;
    Time applicationJitterTime = MilliSeconds(100);
    bool enableLogging = true;
    bool prioToSps = false;
    uint16_t psfchPeriod = 4;
    Time slBearersActivationTime = Seconds(2);
    Time trafficDuration = Seconds(10);
    Time windDownDuration = Seconds(1); // Time for any lingering packets to be received

    bool enableDynTraffic = true;
    // traffic type is dynamic, HARQ feedback enabled, unicast
    // packet sizes are variable between 20 and 1400 bytes
    bool dynTrafficHarqEnabled = true;
    bool dynTrafficDynamic = true;
    SidelinkInfo::CastType dynTrafficCastType{SidelinkInfo::CastType::Unicast};
    uint32_t dynTrafficPriority = 1;
    Time dynTrafficPdb = Seconds(0); // will use T2 as the delay budget
    uint32_t dynTrafficMaxPackets = 1;
    uint32_t dynTrafficPacketSize = 20; // bytes
    Time dynTrafficInterval = MilliSeconds(100);
    Time dynTrafficRri = MilliSeconds(20); // this is unused

    bool enableSpsTraffic = false;
    // traffic type is SPS, HARQ feedback enabled, groupcast, lower priority
    bool spsTrafficHarqEnabled = true;
    bool spsTrafficDynamic = false;
    SidelinkInfo::CastType spsTrafficCastType{SidelinkInfo::CastType::Groupcast};
    uint32_t spsTrafficPriority = 2;
    Time spsTrafficPdb = MilliSeconds(20);
    uint32_t spsTrafficMaxPackets = 100;
    uint32_t spsTrafficPacketSize = 20; // bytes
    Time spsTrafficInterval = MilliSeconds(20);
    Time spsTrafficRri = MilliSeconds(20);
    uint16_t mcs = 0;

    // NR parameters
    uint16_t numerologyBwpSl = 2;
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    uint16_t bandwidthBandSl = 400;         // Multiple of 100 KHz; 400 = 40 MHz
    double txPower = 23;                    // dBm

    CommandLine cmd(__FILE__);
    cmd.AddValue("trafficDuration", "The time traffic will be active", trafficDuration);
    cmd.AddValue("numUe", "Number of UEs", numUe);
    cmd.AddValue("enableDynTraffic", "Enable dynamic traffic", enableDynTraffic);
    cmd.AddValue("dynTrafficPacketSize", "UDP payload size (bytes)", dynTrafficPacketSize);
    cmd.AddValue("enableSpsTraffic", "Enable SPS traffic", enableSpsTraffic);
    cmd.AddValue("spsTrafficPacketSize", "UDP payload size (bytes)", spsTrafficPacketSize);
    cmd.AddValue("psfchPeriod", "PSFCH period (0, 1, 2, 4)", psfchPeriod);
    cmd.AddValue("mcs", "MCS (0-28)", mcs);
    cmd.AddValue("enableLogging", "Enable logging", enableLogging);
    cmd.AddValue("writeTraces", "Flag to control the writing of output traces", g_writeTraces);

    // Parse the command line
    cmd.Parse(argc, argv);

    // log data plane
    if (enableLogging)
    {
        LogLevel info =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_NODE | LOG_PREFIX_TIME | LOG_LEVEL_INFO);
        LogLevel debug [[maybe_unused]] =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_NODE | LOG_PREFIX_TIME | LOG_LEVEL_DEBUG);
        LogComponentEnable("OnOffApplication", info);
        LogComponentEnable("PacketSink", info);
        LogComponentEnable("NrNetDevice", info);
        LogComponentEnable("NrUeNetDevice", info);
        LogComponentEnable("NrEpcUeNas", info);
        LogComponentEnable("NrPdcp", info);
        LogComponentEnable("NrRlcUm", info);
        LogComponentEnable("NrSlBwpManagerUe", info);
        LogComponentEnable("NrSlUeMac", info);
        LogComponentEnable("NrSlUeMacHarq", info);
        LogComponentEnable("NrPhy", info);
        LogComponentEnable("NrSlUePhy", info);
        LogComponentEnable("NrSlSpectrumPhy", info);
        LogComponentEnable("NrSpectrumPhy", info);
    }

    // Final simulation time
    Time finalSimTime = slBearersActivationTime + trafficDuration + windDownDuration;

    /*
     * Default values for the simulation. We are progressively removing all
     * the instances of SetDefault, but we need it for legacy code (LTE)
     */
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Create UE nodes
    NodeContainer ueNodeContainer;
    ueNodeContainer.Create(numUe);

    // Assign position and mobility to the UEs
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<UniformDiscPositionAllocator> positionAllocUe =
        CreateObject<UniformDiscPositionAllocator>();
    positionAllocUe->SetRho(positionAllocatorRadius);
    positionAllocUe->AssignStreams(streamIndex);
    streamIndex += streamIncrement;
    mobility.SetPositionAllocator(positionAllocUe);
    mobility.Install(ueNodeContainer);

    NrSlTraceHelper nrSlTraceHelper;
    if (g_writeTraces)
    {
        nrSlTraceHelper.TraceUePositions(ueNodeContainer,
                                         Seconds(1000),
                                         "sl-example-ue-positions.dat");
    }

    // Configure NR module
    auto nrSlHelper = CreateObject<NrSlHelper>();
    nrSlHelper->SetAttribute("Numerology", UintegerValue(0));

    // Basic channel with V2V urban propagation loss model from TR 37.885
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    auto lossModel = CreateObject<ThreeGppV2vUrbanPropagationLossModel>();
    lossModel->SetAttribute("ShadowingEnabled", BooleanValue(false));
    auto channelCondition = CreateObject<AlwaysLosChannelConditionModel>();
    lossModel->SetChannelConditionModel(channelCondition);
    lossModel->SetFrequency(centralFrequencyBandSl);
    channel->AddPropagationLossModel(lossModel);

    // The sidelink fast fading error model is installed in the NrSlSpectrumPhy
    // and needs to be configured separately from the above loss model.
    // The default is a static channel, but the NrSlEpaErrorModel may also be
    // configured for a fading channel
    nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

    // Create a single bandwidth part and add it to the container (allBwps)
    // needed by the NrSlHelper::Install() method
    std::unique_ptr<BandwidthPartInfo> bwpInfo(new BandwidthPartInfo());
    bwpInfo->m_bwpId = 0;
    bwpInfo->m_centralFrequency = centralFrequencyBandSl;
    bwpInfo->m_lowerFrequency = centralFrequencyBandSl - bandwidthBandSl * 1e5 / 2;
    bwpInfo->m_higherFrequency = centralFrequencyBandSl + bandwidthBandSl * 1e5 / 2;
    bwpInfo->SetChannel(channel);
    BandwidthPartInfoPtrVector allBwps;
    allBwps.push_back(bwpInfo);

    nrSlHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("AntennaElement",
                                      PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrSlHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));

    // NR Sidelink attribute of UE MAC, which are common for all the UEs
    nrSlHelper->SetUeMacAttribute("EnableSensing", BooleanValue(true));
    nrSlHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrSlHelper->SetUeMacAttribute("T2", UintegerValue(33));
    nrSlHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));

    uint8_t bwpIdForGbrMcptt = 0;

    nrSlHelper->SetBwpManagerTypeId(TypeId::LookupByName("ns3::NrSlBwpManagerUe"));
    // following parameter has no impact at the moment because:
    // 1. No support for PQI based mapping between the application and the LCs
    // 2. No scheduler to consider PQI
    // However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
    // because we hard coded the PQI 65 in UE RRC.
    nrSlHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK",
                                                  UintegerValue(bwpIdForGbrMcptt));

    std::set<uint8_t> bwpIdContainer;
    bwpIdContainer.insert(bwpIdForGbrMcptt);

    // Install UE NetDevices
    NetDeviceContainer ueNetDev = nrSlHelper->InstallUeDevice(ueNodeContainer, allBwps);

    /**************************** SL configuration *****************************/

    // SL scheduler
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(mcs));
    nrSlHelper->SetUeSlSchedulerAttribute("PriorityToSps", BooleanValue(prioToSps));

    nrSlHelper->PrepareUeForSidelink(ueNetDev, bwpIdContainer);

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(3);
    ptrFactory->SetSlPsfchPeriod(psfchPeriod);
    ptrFactory->SetSlMinTimeGapPsfch(3);

    std::list<uint16_t> resourceReservePeriodList = {0, 20, 50, 100}; // in ms
    ptrFactory->SetSlResourceReservePeriodList(resourceReservePeriodList);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which hold a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which hold an array of pools
    NrSlRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
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

    // Configure the SlFreqConfigCommonNr IE, which hold the array to store
    // the configuration of all Sidelink BWP (s).
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
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
    nrSlHelper->InstallNrSlPreConfiguration(ueNetDev, slPreConfigNr);

    /****************************** End SL Configuration ***********************/

    // Fix random streams
    int64_t streamsUsed = nrSlHelper->AssignStreams(ueNetDev, streamIndex);
    streamIndex += streamIncrement;
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in NrSlHelper");

    // Configure internet
    InternetStackHelper internet;
    internet.Install(ueNodeContainer);
    streamsUsed = internet.AssignStreams(ueNodeContainer, streamIndex);
    streamIndex += streamIncrement;
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in InternetStackHelper");

    // Assign IP address for the UEs
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(ueNetDev);

    /************************** Traffic flows configuration ********************/
    ApplicationContainer allClientApps;
    ApplicationContainer allServerApps;
    for (uint32_t i = 0; i < ueNodeContainer.GetN() - 1; i++)
    {
        if (enableDynTraffic)
        {
            SidelinkInfo slInfo;
            slInfo.m_harqEnabled = dynTrafficHarqEnabled;
            slInfo.m_pdb = dynTrafficPdb;
            slInfo.m_dynamic = dynTrafficDynamic;
            slInfo.m_dstL2Id = i + 1;
            slInfo.m_castType = dynTrafficCastType;
            slInfo.m_priority = dynTrafficPriority;
            slInfo.m_rri = dynTrafficRri;
            uint16_t port = 8000 + i + 1;
            auto [clientApps, serverApps] = CreateTraffic(ueNetDev.Get(i),
                                                          ueNetDev.Get(i + 1),
                                                          ueIpIface.GetAddress(i + 1),
                                                          port,
                                                          slInfo,
                                                          dynTrafficMaxPackets,
                                                          dynTrafficPacketSize,
                                                          dynTrafficInterval,
                                                          applicationJitterTime,
                                                          nrSlHelper,
                                                          slBearersActivationTime,
                                                          finalSimTime,
                                                          streamIndex);
            allClientApps.Add(clientApps);
            allServerApps.Add(serverApps);
        }
        streamIndex += 1;
        if (enableSpsTraffic)
        {
            SidelinkInfo slInfo;
            slInfo.m_harqEnabled = spsTrafficHarqEnabled;
            slInfo.m_pdb = spsTrafficPdb;
            slInfo.m_dynamic = spsTrafficDynamic;
            slInfo.m_dstL2Id = i + 1;
            slInfo.m_castType = spsTrafficCastType;
            slInfo.m_priority = spsTrafficPriority;
            slInfo.m_rri = spsTrafficRri;
            uint16_t port = 9000 + i + 1;
            auto [clientApps, serverApps] = CreateTraffic(ueNetDev.Get(i),
                                                          ueNetDev.Get(i + 1),
                                                          ueIpIface.GetAddress(i + 1),
                                                          port,
                                                          slInfo,
                                                          spsTrafficMaxPackets,
                                                          spsTrafficPacketSize,
                                                          spsTrafficInterval,
                                                          applicationJitterTime,
                                                          nrSlHelper,
                                                          slBearersActivationTime,
                                                          finalSimTime,
                                                          streamIndex);
            allClientApps.Add(clientApps);
            allServerApps.Add(serverApps);
        }
    }

    /************************ END Traffic flows configuration ******************/

    /******************** Application packet tracing ***************************/
    if (g_writeTraces)
    {
        AsciiTraceHelper ascii;
        Ptr<OutputStreamWrapper> packetTraceForDelayStream =
            ascii.CreateFileStream("NrSlAppRxPacketDelayTrace.txt");
        *packetTraceForDelayStream->GetStream()
            << "time(s)\trxNodeId\tsrcIp\tdstIp\tseqNum\tdelay(ms)" << std::endl;
    }

    for (uint16_t ac = 0; ac < allClientApps.GetN(); ac++)
    {
        allClientApps.Get(ac)->TraceConnectWithoutContext("TxWithSeqTsSize",
                                                          MakeCallback(&TxPacketTraceForDelay));
    }
    for (uint16_t ac = 0; ac < allServerApps.GetN(); ac++)
    {
        allServerApps.Get(ac)->TraceConnectWithoutContext("RxWithSeqTsSize",
                                                          MakeCallback(&RxPacketTraceForDelay));
    }
    /******************** END Application packet  tracing **********************/

    for (uint32_t i = 0; i < ueNodeContainer.GetN() - 1; i++)
    {
        if (g_writeTraces)
        {
            std::string sensingString = "sl-example-sensing-" + std::to_string(i) + ".dat";
            std::string schedString = "sl-example-scheduling-" + std::to_string(i) + ".dat";
            nrSlTraceHelper.TraceSensingAlgorithm(ueNodeContainer.Get(i), sensingString);
            nrSlTraceHelper.TraceSchedulingAlgorithm(ueNodeContainer.Get(i), schedString);
        }
        auto ueDevice = ueNetDev.Get(i)->GetObject<NrUeNetDevice>();
        NS_ASSERT(ueDevice);
        auto uePhy = ueDevice->GetPhy(0)->GetObject<NrSlUePhy>();
        NS_ASSERT(uePhy);
        auto ueSpectrumPhy = uePhy->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
        NS_ASSERT(ueSpectrumPhy);
        ueSpectrumPhy->TraceConnect("SlPscchDecodeFailure",
                                    std::to_string(i),
                                    MakeCallback(&TraceSlPscchDecodeFailure));
        ueSpectrumPhy->TraceConnect("SlSci2aDecodeFailure",
                                    std::to_string(i),
                                    MakeCallback(&TraceSlSci2aDecodeFailure));
        ueSpectrumPhy->TraceConnect("SlTbDecodeFailure",
                                    std::to_string(i),
                                    MakeCallback(&TraceSlTbDecodeFailure));
        auto ueMac = ueDevice->GetMac(0)->GetObject<NrSlUeMac>();
        PointerValue v;
        ueMac->GetAttribute("NrSlUeMacScheduler", v);
        auto scheduler = v.Get<NrSlUeMacScheduler>()->GetObject<NrSlUeMacSchedulerDefault>();
        scheduler->TraceConnect("GrantCreated",
                                std::to_string(i),
                                MakeCallback(&TraceGrantCreated));
        scheduler->TraceConnect("GrantPublished",
                                std::to_string(i),
                                MakeCallback(&TraceGrantPublished));
    }

    if (g_writeTraces)
    {
        g_fileGrantCreated.open("sl-example-grants.dat", std::ofstream::out);
        g_fileGrantPublished.open("sl-example-grants-published.dat", std::ofstream::out);
        g_fileSpectrumTrace.open("sl-example-spectrum-trace.dat", std::ofstream::out);
    }

    // Configure FlowMonitor to get traffic flow statistics
    FlowMonitorHelper flowmonHelper;
    auto monitor = flowmonHelper.Install(ueNodeContainer);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(finalSimTime);
    std::cout << "Simulation ends at " << finalSimTime.GetSeconds() << "s" << std::endl;
    Simulator::Run();

    if (g_writeTraces)
    {
        g_fileGrantCreated.close();
        g_fileGrantPublished.close();
        g_fileSpectrumTrace.close();
    }
    std::cout << "Total Tx packets = " << g_txPktCounter << std::endl;
    std::cout << "Total Rx packets = " << g_rxPktCounter << std::endl;

    Ptr<MinMaxAvgTotalCalculator<double>> delayStats =
        CreateObject<MinMaxAvgTotalCalculator<double>>();
    for (auto it = g_delays.begin(); it != g_delays.end(); it++)
    {
        delayStats->Update(*it);
    }
    std::cout << "Average packet delay = " << delayStats->getMean() << " ms" << std::endl;
    std::cout << "Min packet delay = " << delayStats->getMin() << " ms" << std::endl;
    std::cout << "Max packet delay = " << delayStats->getMax() << " ms" << std::endl;

    Simulator::Destroy();
    return 0;
}

void
TraceGrantCreated(std::string context,
                  const struct NrSlUeMacScheduler::GrantInfo& grantInfo,
                  uint16_t psfchPeriod)
{
    if (g_firstGrant)
    {
        WriteGrantCreated(g_firstGrantCreated, context, grantInfo, psfchPeriod);
        g_firstGrant = false;
    }
    if (g_writeTraces)
    {
        WriteGrantCreated(g_fileGrantCreated, context, grantInfo, psfchPeriod);
    }
}

void
TraceGrantPublished(std::string context,
                    const struct NrSlUeMac::NrSlGrant& grant,
                    uint16_t psfchPeriod)
{
    if (g_writeTraces)
    {
        WriteGrantPublished(g_fileGrantPublished, context, grant, psfchPeriod);
    }
}

void
WriteGrantCreated(std::ostream& grantStream,
                  std::string context,
                  const struct NrSlUeMacScheduler::GrantInfo& grantInfo,
                  uint16_t psfchPeriod)
{
    grantStream << Now().As(Time::S) << " " << context << " ";
    grantStream << (grantInfo.isDynamic ? "dynamic " : "sps ");
    grantStream << +grantInfo.harqId << " ";
    grantStream << (grantInfo.harqEnabled ? "harq:" : "no-harq:");
    grantStream << grantInfo.slotAllocations.size();
    if (!grantInfo.isDynamic)
    {
        grantStream << " " << +grantInfo.cReselCounter << " " << +grantInfo.slResoReselCounter
                    << " " << +grantInfo.nSelected << " " << +grantInfo.tbTxCounter
                    << grantInfo.rri.GetMilliSeconds() << std::endl;
    }
    else
    {
        grantStream << std::endl;
    }
    for (const auto& it : grantInfo.slotAllocations)
    {
        uint64_t slot = it.sfn.Normalize();
        double slotDurationS = 0.001 / (1 << it.sfn.GetNumerology());
        double slotTimeS = slot * slotDurationS;
        grantStream << "    " << std::fixed << std::setprecision(6) << slotTimeS << " "
                    << it.sfn.Normalize() << " ";
        grantStream << it.slPsschSubChStart << ":" << it.slPsschSubChLength << " " << it.dstL2Id
                    << " ";
        grantStream << psfchPeriod << " " << it.txSci1A << " " << +it.slotNumInd;
        for (const auto& it2 : it.slRlcPduInfo)
        {
            grantStream << " (LCID " << +it2.lcid << " size " << it2.size << ")";
        }
        grantStream << std::endl;
    }
}

void
WriteGrantPublished(std::ostream& grantStream,
                    std::string context,
                    const struct NrSlUeMac::NrSlGrant& grant,
                    uint16_t psfchPeriod)
{
    grantStream << Now().As(Time::S) << " " << context << " ";
    grantStream << +grant.harqId << " ";
    grantStream << (grant.harqEnabled ? "harq " : "no-harq ");
    grantStream << grant.slotAllocations.size() << " ";
    grantStream << grant.rri.GetMilliSeconds() << "ms ";
    grantStream << grant.tbSize << std::endl;
    for (const auto& it : grant.slotAllocations)
    {
        uint64_t slot = it.sfn.Normalize();
        double slotDurationS = 0.001 / (1 << it.sfn.GetNumerology());
        double slotTimeS = slot * slotDurationS;
        grantStream << "    " << std::fixed << std::setprecision(6) << slotTimeS << " "
                    << it.sfn.Normalize() << " ";
        grantStream << it.slPsschSubChStart << ":" << it.slPsschSubChLength << " " << it.dstL2Id
                    << " ";
        grantStream << psfchPeriod << " " << it.txSci1A << " " << +it.slotNumInd;
        for (const auto& it2 : it.slRlcPduInfo)
        {
            grantStream << " (LCID " << +it2.lcid << " size " << it2.size << ")";
        }
        grantStream << std::endl;
    }
}

// Get the packet size in bytes that supports the provided data rate and RRI
uint32_t
GetPacketSize(const DataRate& dataRate, Time rri)
{
    return static_cast<uint32_t>(dataRate.GetBitRate() * rri.GetSeconds());
}

void
TraceSlPscchDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue)
{
    if (g_writeTraces)
    {
        g_fileSpectrumTrace << Now().GetSeconds() << " " << context << " PSCCH decode failure"
                            << std::endl;
    }
}

void
TraceSlSci2aDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue)
{
    if (g_writeTraces)
    {
        g_fileSpectrumTrace << Now().GetSeconds() << " " << context << " SCI-2A decode failure"
                            << std::endl;
    }
}

void
TraceSlTbDecodeFailure(std::string context, uint64_t oldValue, uint64_t newValue)
{
    if (g_writeTraces)
    {
        g_fileSpectrumTrace << Now().GetSeconds() << " " << context << " TB decode failure"
                            << std::endl;
    }
}

void
TxPacketTraceForDelay(Ptr<const Packet> p,
                      const Address& srcAddrs,
                      const Address& dstAddrs,
                      const SeqTsSizeHeader& seqTsSizeHeader)
{
    g_txPktCounter++;
    std::ostringstream oss;
    oss << InetSocketAddress::ConvertFrom(dstAddrs).GetPort() << "(" << seqTsSizeHeader.GetSeq()
        << ")";
    std::string mapKey = oss.str();
    PacketWithRxTimestamp mapValue;
    mapValue.p = p;
    mapValue.txTimestamp = Simulator::Now();
    g_rxPacketsForDelayCalc.insert(std::pair<std::string, PacketWithRxTimestamp>(mapKey, mapValue));
    NS_LOG_DEBUG("TX: " << mapKey);
}

void
RxPacketTraceForDelay(Ptr<const Packet> p,
                      const Address& srcAddrs,
                      const Address& dstAddrs,
                      const SeqTsSizeHeader& seqTsSizeHeader)
{
    g_rxPktCounter++;

    double delay = 0.0;
    std::ostringstream oss;
    oss << InetSocketAddress::ConvertFrom(dstAddrs).GetPort() << "(" << seqTsSizeHeader.GetSeq()
        << ")";
    std::string mapKey = oss.str();

    NS_LOG_DEBUG(" RX: " << mapKey << " " << delay << " ms");
    auto it = g_rxPacketsForDelayCalc.find(mapKey);
    if (it == g_rxPacketsForDelayCalc.end())
    {
        NS_FATAL_ERROR("Rx packet not found?!");
    }
    else
    {
        delay =
            Simulator::Now().GetSeconds() * 1000.0 - it->second.txTimestamp.GetSeconds() * 1000.0;
        g_delays.push_back(delay);
        g_rxPacketsForDelayCalc.erase(mapKey);
    }
    NS_LOG_DEBUG(" RX: " << mapKey << " size " << p->GetSize() << delay << " ms");
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
              Time slBearersActivationTime,
              Time finalSimTime,
              int64_t streamIndex)
{
    Ptr<NrSlTft> txTft;
    Ptr<NrSlTft> rxTft;
    Address remoteAddress = InetSocketAddress(destination, port);
    txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, port, slInfo);
    rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, port, slInfo);
    nrSlHelper->ActivateNrSlBearer(slBearersActivationTime, clientDevice, txTft);
    nrSlHelper->ActivateNrSlBearer(slBearersActivationTime, serverDevice, rxTft);
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(maxPackets * packetSize));
    // In the future (ns-3.44 or greater) the below two lines can be replaced
    // by OnOffHelper::SetConstantInterval()
    DataRate dataRate{static_cast<uint64_t>(packetSize * 8 / interval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, packetSize);
    ApplicationContainer clientApps = sidelinkClient.Install(clientDevice->GetNode());
    auto uniformRv = CreateObject<UniformRandomVariable>();
    uniformRv->SetAttribute("Max", DoubleValue(applicationJitterTime.GetSeconds()));
    uniformRv->SetStream(streamIndex++);
    clientApps.StartWithJitter(slBearersActivationTime, uniformRv);
    clientApps.Stop(finalSimTime);
    ApplicationContainer serverApps;
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    serverApps = sidelinkSink.Install(serverDevice->GetNode());
    serverApps.Start(slBearersActivationTime);
    return std::make_tuple(clientApps, serverApps);
}
