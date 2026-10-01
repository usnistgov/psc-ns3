// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2022 University of Washington (HARQ extensions)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \ingroup examples
 * \file sl-harq-example.cc
 *
 * This example describes how to configure and observe sidelink HARQ
 * properties in an NR sidelink out-of-coverage simulation.  Sidelink HARQ
 * is an automatic repeat request (ARQ) mechanism for unicast and groupcast
 * communications.  For broadcast communication, a non-ARQ technique called
 * blind retransmissions may be used to improve reliability.
 *
 * By default, a simulation scenario similar to the cttc-nr-v2x-demo-simple.cc
 * program is created with two UEs configured to use HARQ to support a
 * unicast session.  One band with a single component carrier, and one
 * bandwidth part, is used.  Since there
 * is only one transmitter in the scenario, sensing is by default not enabled.
 *
 * By default, a groupcast option 2 HARQ configuration is used (option 1
 * is not supported).  For groupcast, three UEs are used, with one sender
 * and two receivers.  Optionally, a unicast HARQ configuration can be
 * configured, in which two UEs are used with one sender and one receiver.
 * Finally, a broadcast can be used, with one sending UE and two receiving
 * UEs, and blind retransmissions.  In all three cases, the use of
 * retransmissions can optionally be disabled.
 *
 * The transmission process is a constant bit rate application sending
 * at 16Kbps.  The application starts at time 2 seconds into the simulation.
 * The bearer is activated at time 1.9 seconds.  The simulation then runs
 * for sufficient time to send and receive the configured number of
 * application data packets (default 100).
 *
 * Delay samples are calculated by the NrSlUeMac reading the timestamp
 * from the RlcTag attached to each transmitted packet.  The delay
 * samples are stored in a file 'sl-harq-example-delay.dat' with
 * a format of "Time delay" (units of seconds, resolution of microseconds).
 * Additionally, the minimum and maximum delay are tracked and output
 * to the terminal.
 *
 * The example will also output the average Packet Inter-Reception (PIR)
 * type 2 computed as defined in 3GPP TR 37.885.  The PIR is measured by
 * tracing the reception process in the receiving (PacketSink) application.
 *
 * Sample output is shown below:
 * Inter UE distance 20 m
 * Cast type: groupcast
 * Data rate 16000 bps
 * App start time 2.01 sec
 * App stop time 12.01 sec
 * Total Tx packets = 100
 * Total Rx packets = 200
 * Average throughput = 32.000000000 kbps
 * Average Packet Inter-Reception (PIR) 0.099000000 sec
 * Min/max delay (us) 778.569000000 832.140000000
 *
 * \code{.unparsed}
$ ./waf --run "sl-harq-example --PrintHelp"
    \endcode
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"

#include <fstream>
#include <iomanip>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SlHarqExample");

/*
 * Global variables
 */

double g_delayMin = 1e9;      //!< Global varible to store delay min value
double g_delayMax = 0;        //!< Global varible to store delay max value
bool g_verbose = false;       //!< Global variable to store verbose mode
bool g_writeTraces = true;    //!< Flag to control writing traces
std::ofstream g_delaySamples; //!< File stream object to save latency trace
uint32_t rxByteCounter = 0;   //!< Global variable to count RX bytes
uint32_t txByteCounter = 0;   //!< Global variable to count TX bytes
uint32_t rxPktCounter = 0;    //!< Global variable to count RX packets
uint32_t txPktCounter = 0;    //!< Global variable to count TX packets

uint64_t pirCounter = 0;        //!< counter to count PIR samples
Time lastPktRxTime(Seconds(0)); //!< Global variable to store the RX time of a packet
Time pir(Seconds(0));           //!< Global varible to store PIR value

uint64_t pirCounter2 = 0;        //!< counter to count PIR samples
Time lastPktRxTime2(Seconds(0)); //!< Global variable to store the RX time of a packet
Time pir2(Seconds(0));           //!< Global varible to store PIR value
/*
 * Function declarations (defined below)
 */
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
// Listen to NrSlUeMacHarq::RxHarqFeedback events
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

int
main(int argc, char* argv[])
{
    // Command-line variables
    std::string castType = "groupcast";
    bool harqEnabled = true;
    uint16_t psfchPeriod = 4;
    uint16_t numerologyBwpSl = 2;
    uint16_t slSubchannelSize = 50; // PRBs
    uint16_t t1 = 2;
    uint16_t t2 = 33;
    uint16_t interUeDistance = 20; // meters
    bool logging = false;
    uint32_t numPackets = 100;

    // Constants for traffic generation
    bool useIPv6 = true;          // NR V2X standardized for IPv6 only
    uint32_t udpPacketSize = 200; // bytes
    DataRate dataRate("16Kbps");

    // Other constants
    // Sidelink bearers activation time
    Time slBearersActivationTime = Seconds(1.9);
    Time delayBudget = Seconds(0); // Use T2 to configure selection window edge

    // NR parameters and constants
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    uint16_t bandwidthBandSl = 400;         // Multiple of 100 KHz; 400 = 40 MHz
    double txPower = 23;                    // dBm

    CommandLine cmd;
    cmd.AddValue("castType", "unicast, groupcast, or broadcast type", castType);
    cmd.AddValue("harqEnabled", "Whether HARQ is enabled", harqEnabled);
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
    cmd.AddValue("interUeDistance",
                 "The distance in meters between UEs in the topology",
                 interUeDistance);
    cmd.AddValue("numPackets", "Number of packets to send", numPackets);
    cmd.AddValue("logging", "Enable logging (if logging is enabled in the build)", logging);
    cmd.AddValue("writeTraces", "Flag to control the writing of output traces", g_writeTraces);
    cmd.Parse(argc, argv);

    // Final simulation time is the sum of start up time, data transfer time,
    // and finishing time of 100 ms.
    Time finalSlBearersActivationTime = slBearersActivationTime + Seconds(0.01);
    Time simTime = Seconds(numPackets * udpPacketSize * 8.0 / dataRate.GetBitRate());
    Time finalSimTime = simTime + finalSlBearersActivationTime + MilliSeconds(100);
    std::cout << "Inter UE distance " << interUeDistance << " m" << std::endl;
    std::cout << "Cast type: " << castType << std::endl;

    // Checks on configurable values
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);
    NS_ABORT_UNLESS(castType == "unicast" || castType == "broadcast" || castType == "groupcast");
    NS_ABORT_UNLESS(psfchPeriod == 0 || psfchPeriod == 1 || psfchPeriod == 2 || psfchPeriod == 4);

    /*
     * If the logging variable is set to true, enable the log of some components
     * through the code. The same effect can be obtained through the use
     * of the NS_LOG environment variable:
     *
     * export NS_LOG="UdpClient=level_info|prefix_time|prefix_func|prefix_node:UdpServer=..."
     *
     * Usually, the environment variable way is preferred, as it is more customizable,
     * and more expressive.
     */
    if (logging)
    {
        LogLevel logLevel =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
#if 0
      LogComponentEnable ("OnOffApplication", logLevel);
      LogComponentEnable ("PacketSink", logLevel);
      LogComponentEnable ("LtePdcp", logLevel);
#endif
        LogComponentEnable("NrSlUeMacHarq", logLevel);
        LogComponentEnable("NrSlUeMacScheduler", logLevel);
        LogComponentEnable("NrSlUeMacSchedulerDefault", logLevel);
#if 0
      LogComponentEnable ("NrSlHelper", logLevel);
      LogComponentEnable ("NrSlUeRrc", logLevel);
#endif
        LogComponentEnable("NrUeMac", logLevel);
#if 0
      LogComponentEnable ("NrUePhy", logLevel);
      LogComponentEnable ("NrSpectrumPhy", logLevel);
      LogComponentEnable ("NrNetDevice", logLevel);
#endif
    }

    // Default values for the simulation. We are progressively removing all
    // the instances of SetDefault, but we need it for legacy code (LTE)
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    NodeContainer ueContainer;
    uint16_t ueNum = 0;
    if (castType == "unicast")
    {
        ueNum = 2;
    }
    else
    {
        ueNum = 3;
    }
    ueContainer.Create(ueNum);

    // The default topology for unicast is the following:
    //
    //        UE1..........(20 m)..........UE2
    //   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)
    //
    // where 20 ms is the default interUeDistance.  The default topology for
    // groupcast adds a third node such that an equilateral triangle between
    // nodes (at distance 20 m) is formed.
    //
    // If three nodes, the topology is an equilateral triangle with edges of
    // interUeDistance.
    //
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator>();
    if (castType == "unicast")
    {
        positionAllocUe->Add(Vector(0.0, 0.0, 1.5));
        positionAllocUe->Add(Vector(interUeDistance, 0.0, 1.5));
    }
    else
    {
        positionAllocUe->Add(Vector(0.0, 0.0, 1.5));
        positionAllocUe->Add(Vector(interUeDistance, 0.0, 1.5));
        positionAllocUe->Add(Vector(interUeDistance / 2, 0.866025 * interUeDistance, 1.5));
    }
    mobility.SetPositionAllocator(positionAllocUe);
    mobility.Install(ueContainer);

    // NR configuration
    Ptr<NrSlHelper> nrHelper = CreateObject<NrSlHelper>();

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
    nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

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

    uint8_t bwpIdForGbrMcptt = 0;

    nrHelper->SetBwpManagerTypeId(TypeId::LookupByName("ns3::NrSlBwpManagerUe"));
    // following parameter has no impact at the moment because:
    // 1. No support for PQI based mapping between the application and the LCs
    // 2. No scheduler to consider PQI
    // However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
    // because we hard coded the PQI 65 in UE RRC.
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK",
                                                UintegerValue(bwpIdForGbrMcptt));

    std::set<uint8_t> bwpIdContainer;
    bwpIdContainer.insert(bwpIdForGbrMcptt);

    /*
     * We have configured the attributes we needed. Now, install and get the pointers
     * to the NetDevices, which contains all the NR stack:
     */
    NetDeviceContainer ueNetDevices = nrHelper->InstallUeDevice(ueContainer, allBwps);

    /*
     * Set the SL scheduler attributes
     * In this example we use NrSlUeMacSchedulerSimple scheduler, which uses
     * a fixed MCS value by default
     */
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(14));

    /*
     * Very important method to configure UE protocol stack, i.e., it would
     * configure all the SAPs among the layers, setup callbacks, configure
     * error model, configure AMC, and configure ChunkProcessor in Interference
     * API.
     */
    nrHelper->PrepareUeForSidelink(ueNetDevices, bwpIdContainer);

    /*
     * Start preparing for all the sub Structs/RRC Information Element (IEs)
     * of NrSlRrcSap::SidelinkPreconfigNr. This is the main structure, which would
     * hold all the pre-configuration related to Sidelink.
     */

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    // get it from pool factory
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    /*
     * Above pool factory is created to help the users of the simulator to create
     * a pool with valid default configuration. Please have a look at the
     * constructor of NrSlCommResourcePoolFactory class.
     *
     * In the following, we show how one could change those default pool parameter
     * values as per the need.
     */
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(slSubchannelSize);
    ptrFactory->SetSlMaxNumPerReserve(3);
    ptrFactory->SetSlPsfchPeriod(psfchPeriod);
    ptrFactory->SetSlMinTimeGapPsfch(3);
    std::list<uint16_t> resourceReservePeriodList = {0, 100}; // in ms
    ptrFactory->SetSlResourceReservePeriodList(resourceReservePeriodList);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which hold a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    // Pool id, ranges from 0 to 15
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which hold an array of pools
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

    // Configure the SlFreqConfigCommonNr IE, which hold the array to store
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
    nrHelper->InstallNrSlPreConfiguration(ueNetDevices, slPreConfigNr);

    /****************************** End SL Configuration ***********************/

    /*
     * Fix the random streams
     */
    int64_t streamBase{1000};
    int64_t streamsUsed{0};
    streamsUsed = nrHelper->AssignStreams(ueNetDevices, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in NrHelper");

    /*
     * Configure the IP stack, and activate NR sidelink bearer (s)
     *
     * This example supports IPV4 and IPV6
     */

    InternetStackHelper internet;
    internet.Install(ueContainer);
    streamBase = 2000;
    streamsUsed = internet.AssignStreams(ueContainer, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in InternetStackHelper");
    uint32_t dstL2Id = 224;
    Ipv4Address groupAddress4("225.0.0.0"); // use multicast address as destination
    Ipv6Address groupAddress6("ff0e::1");   // use multicast address as destination
    Address remoteAddress;
    Address localAddress;
    uint16_t port = 8000;
    Ptr<NrSlTft> tft;
    SidelinkInfo slInfo;
    if (castType == "groupcast")
    {
        slInfo.m_castType = SidelinkInfo::CastType::Groupcast;
    }
    else if (castType == "broadcast")
    {
        slInfo.m_castType = SidelinkInfo::CastType::Broadcast;
    }
    else if (castType == "unicast")
    {
        slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    }
    slInfo.m_harqEnabled = harqEnabled;
    slInfo.m_pdb = delayBudget;
    slInfo.m_dstL2Id = dstL2Id;
    slInfo.m_rri = MilliSeconds(100);
    if (!useIPv6)
    {
        Ipv4AddressHelper addrHelper;
        addrHelper.SetBase("7.0.0.0", "255.0.0.0");
        auto ueIpIface = addrHelper.Assign(ueNetDevices);
        localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);
        if (castType == "groupcast" || castType == "broadcast")
        {
            Ipv4StaticRoutingHelper ipv4RoutingHelper;
            Ipv4InterfaceContainer::Iterator i;
            for (i = ueIpIface.Begin(); i != ueIpIface.End(); ++i)
            {
                const auto [ipv4, index] = *i;
                auto ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ipv4);
                ueStaticRouting->SetDefaultMulticastRoute(index);
            }
            remoteAddress = InetSocketAddress(groupAddress4, port);
            // The first node is a transmitter; others are receivers
            NetDeviceContainer transmitDevices;
            transmitDevices.Add(ueNetDevices.Get(0));
            tft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, groupAddress4, slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, transmitDevices, tft);
            NetDeviceContainer receiveDevices;
            receiveDevices.Add(ueNetDevices.Get(1));
            receiveDevices.Add(ueNetDevices.Get(2));
            tft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, groupAddress4, slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, receiveDevices, tft);
        }
        else
        {
            remoteAddress = InetSocketAddress(ueIpIface.GetAddress(1, 0), port);
            tft = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                                  ueIpIface.GetAddress(1, 0),
                                  slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDevices, tft);
        }
    }
    else
    {
        Ipv6AddressHelper addrHelper;
        // we use a /64 IPv6 net all UEs
        addrHelper.SetBase("7777:f00d::", Ipv6Prefix(64));
        auto ueIpIface = addrHelper.Assign(ueNetDevices);
        localAddress = Inet6SocketAddress(Ipv6Address::GetAny(), port);
        if (castType == "groupcast" || castType == "broadcast")
        {
            Ipv6StaticRoutingHelper ipv6RoutingHelper;
            Ipv6InterfaceContainer::Iterator i;
            for (i = ueIpIface.Begin(); i != ueIpIface.End(); ++i)
            {
                const auto [ipv6, index] = *i;
                auto ueStaticRouting = ipv6RoutingHelper.GetStaticRouting(ipv6);
                ueStaticRouting->SetDefaultMulticastRoute(index);
            }
            remoteAddress = Inet6SocketAddress(groupAddress6, port);
            // The first node is a transmitter; others are receivers
            NetDeviceContainer transmitDevices;
            transmitDevices.Add(ueNetDevices.Get(0));
            tft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, groupAddress6, slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, transmitDevices, tft);
            NetDeviceContainer receiveDevices;
            receiveDevices.Add(ueNetDevices.Get(1));
            receiveDevices.Add(ueNetDevices.Get(2));
            tft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, groupAddress6, slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, receiveDevices, tft);
        }
        else
        {
            remoteAddress = Inet6SocketAddress(ueIpIface.GetAddress(1, 0), port);
            tft = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                                  ueIpIface.GetAddress(1, 0),
                                  slInfo);
            nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDevices, tft);
        }
    }

    /*
     * Configure the applications:
     * Client app: OnOff application configure to generate CBR traffic
     * Server app: PacketSink application.
     */

    // Set Application in the UEs
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(numPackets * udpPacketSize));
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
    if (ueNum == 3)
    {
        sinks.Add(ueContainer.Get(2));
    }
    serverApps = sidelinkSink.Install(sinks);
    serverApps.Start(Seconds(2.0));

    /*
     * Hook the traces, to be used to compute average PIR
     */

    // Trace receptions; use the following to be robust to node ID changes
    std::ostringstream path;
    path << "/NodeList/" << ueContainer.Get(1)->GetId()
         << "/ApplicationList/0/$ns3::PacketSink/RxWithSeqTsSize";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ComputePir));
    path.str("");

    if (ueNum == 3)
    {
        path << "/NodeList/" << ueContainer.Get(2)->GetId()
             << "/ApplicationList/0/$ns3::PacketSink/RxWithSeqTsSize";
        Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
        Config::ConnectWithoutContext(path.str(), MakeCallback(&ComputePir2));
        path.str("");
    }

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
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/RxHarqFeedback",
                    MakeCallback(&NotifyRxHarqFeedback));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/Allocate",
                    MakeCallback(&NotifyAllocate));

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/NrUeMac/$ns3::NrSlUeMac/NrSlUeMacHarq/Deallocate",
                    MakeCallback(&NotifyDeallocate));

    if (useIPv6)
    {
        for (uint32_t ac = 0; ac < clientApps.GetN(); ac++)
        {
            clientApps.Get(ac)->GetNode()->GetObject<Ipv6L3Protocol>()->AddMulticastAddress(
                groupAddress6);
        }

        // Set Rx traces
        for (uint32_t ac = 0; ac < serverApps.GetN(); ac++)
        {
            serverApps.Get(ac)->GetNode()->GetObject<Ipv6L3Protocol>()->AddMulticastAddress(
                groupAddress6);
        }
    }

    if (g_writeTraces)
    {
        g_delaySamples.open("sl-harq-example-delay.dat", std::ofstream::out);
    }

    Simulator::Stop(finalSimTime);
    Simulator::Run();

    // std::cout << "Total Tx bits = " << txByteCounter * 8 << std:: endl;
    std::cout << "Total Tx packets = " << txPktCounter << std::endl;

    // std::cout << "Total Rx bits = " << rxByteCounter * 8 << std:: endl;
    std::cout << "Total Rx packets = " << rxPktCounter << std::endl;

    std::cout << "Average throughput = "
              << (rxByteCounter * 8) / (finalSimTime - Seconds(realAppStart)).GetSeconds() / 1000.0
              << " kbps" << std::endl;

    std::cout << "Average Packet Inter-Reception (PIR) "
              << (pir + pir2).GetSeconds() / (pirCounter + pirCounter2) << " sec" << std::endl;
    std::cout << "Min/max delay (us) " << g_delayMin * 1e6 << " " << g_delayMax * 1e6 << std::endl;

    Simulator::Destroy();
    if (g_writeTraces)
    {
        g_delaySamples.close();
    }
    return 0;
}

/**
 * Parse context strings such as "/NodeList/3/DeviceList/1/$ns3::NrUeNetDevice"
 * to extract the NodeId
 *
 * \param context context string
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
 * \param packet The packet
 * \param from The address of the transmitting node
 * \param to The address of the receiving node
 * \param header The SeqTsSize header
 */
void
ReceivePacket(Ptr<const Packet> packet,
              const Address& from,
              const Address& to,
              const SeqTsSizeHeader& header)
{
    rxByteCounter += packet->GetSize() + header.GetSerializedSize();
    rxPktCounter++;
}

/**
 * \brief Method to listen the transmitting application trace Tx.
 * \param packet The packet
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
    txByteCounter += packet->GetSize() + header.GetSerializedSize();
    txPktCounter++;
}

/**
 * \brief This method listens to the packet sink application trace Rx.
 * \param packet The packet
 * \param from The address of the transmitter
 * \param to The address of the receiving node
 * \param header The SeqTsSize header
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
 * \param packet The packet
 * \param from The address of the transmitter
 * \param to The address of the receiving node
 * \param header The SeqTsSize header
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
    if (g_verbose)
    {
        std::cout << Now().GetSeconds() << " RX bytes " << bytes << " delay " << delay << std::endl;
    }
    if (delay > g_delayMax)
    {
        g_delayMax = delay;
    }
    if (delay < g_delayMin)
    {
        g_delayMin = delay;
    }
    g_delaySamples << std::fixed << std::showpoint << std::setprecision(6)
                   << Now().GetMicroSeconds() / 1000000.0 << " " << delay << std::endl;
}

/**
 * \brief Method to listen the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * \param pscchStatsParams Parameters of the trace source.
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
}

/**
 * \brief Method to listen the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * \param psschStatsParams Parameters of the trace source.
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
}

/**
 * \brief Method to listen the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * \param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchRx(const SlRxCtrlPacketTraceParams pscchStatsParams)
{
}

/**
 * \brief Method to listen the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * \param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschRx(const SlRxDataPacketTraceParams psschStatsParams)
{
}

/**
 * \brief Method to listen the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * \param duration Duration of the transmission
 */
void
NotifyTxFeedback(std::string context, Time duration)
{
    std::cout << std::fixed << std::showpoint << std::setprecision(9)
              << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
              << " tx feedback duration " << duration.GetNanoSeconds() << "ns" << std::endl;
}

void
NotifyRxHarqFeedback(std::string context, const SlHarqInfo& harqInfo)
{
    std::cout << std::fixed << std::showpoint << std::setprecision(9)
              << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
              << " rx harq; rnti " << harqInfo.m_rnti << " process ID " << +harqInfo.m_harqProcessId
              << " bwpIndex " << +harqInfo.m_bwpIndex << std::endl;
}

void
NotifyAllocate(std::string context,
               uint8_t harqId,
               uint32_t dstL2Id,
               bool multiplePdu,
               Time timeout,
               std::size_t available)
{
    std::cout << std::fixed << std::showpoint << std::setprecision(9)
              << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
              << " allocate; processId " << +harqId << " dstL2Id " << dstL2Id << " multiple PDU "
              << multiplePdu << " timeout " << timeout.GetMilliSeconds() << "ms"
              << " available " << available << std::endl;
}

void
NotifyDeallocate(std::string context, uint8_t harqId, std::size_t available)
{
    std::cout << std::fixed << std::showpoint << std::setprecision(9)
              << Simulator::Now().As(Time::S) << " " << ContextToNodeId(context)
              << " deallocate; processId " << +harqId << " available " << available << std::endl;
}
