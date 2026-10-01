// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

/**
 * \ingroup examples
 * \file sl-multi-lc-example.cc
 *
 * This example is based on cttc-nr-v2x-demo-simple and simulates a simple
 * topology consisting of 2 out-of-coverage UEs, where UE-1 transmits, and UE-2
 * receives.
 * The purpose of this scenario is to show how different sidelink (SL) traffic
 * profiles can be configured in the simulation and associated to different
 * traffic flows. The scheduler then will take into consideration those traffic
 * profiles and grant resources accordingly during the simulation.
 *
 * If interested in learning how to configure the other aspects of a NR SL
 * simulation please refer to cttc-nr-v2x-demo-simple for a nice tutorial.
 *
 * Traffic generation:
 * UE-1 has three traffic flows directed towards UE-2. Each traffic flow is
 * implemented by an OnOffApplication with constant rate that generates CBR
 * traffic for the duration of the 'trafficTime' simulation parameter. The
 * packet size and the data rate can be adjusted with the simulation
 * parameters 'udpPacketSize' and 'dataRate', respectively.
 * Please note that the traffic profiles described below were selected to
 * support the default traffic configuration of 200 bytes packets and a data
 * rate of 16 kbps. Changing those values without adjusting the traffic profile
 * accordingly may result in poor performance.
 * All traffic flows are sent to the multicast address, but each of them to a
 * different port. UE-2 listens to that address and those ports and consumes
 * the packets with a PacketSink application
 *
 *
 * Traffic profiles:
 * Three different traffic profiles are created, one for each traffic flow.
 * The structure SidelinkInfo contains the traffic profile parameters to be
 * propagated in the stack. A SidelinkInfo is created for each traffic flow.
 * A Traffic Flow Template (TFT) is created for each traffic flow associating
 * the corresponding SidelinkInfo, multicast address and port. Three SL data
 * radio bearer are created, each configured with one of the above described
 * TFTs, which will result in the creation of a logical channel per traffic
 * flow on the stack. The scheduler (NrSlUeMacSchedulerDefault) will grant
 * resources for each flow according to the traffic profiles.
 *
 * In this scenario we configure four parameters of the traffic profiles:
 *  - m_dynamic: flag for whether the logical channel scheduling is dynamic
 *               scheduling (i.e., per PDU) or Semi-persistent scheduling (SPS)
 *  - m_dstL2Id: L2 id to which the traffic will be sent
 *  - m_priority: priority (the higher the value, the higher the priority of
 *                the logical channel)
 *  - m_rri: Resource Reservation Interval for SPS scheduling
 *
 *  We created different configurations for the three traffic profiles. The
 *  table below shows the simulation parameters that control which
 *  configuration is used and the resulting values for the traffic profile
 *  parameters of each traffic flow.
 *
 *  -------------------------|-----------------------------
 * |      Simulation         | Resulting traffic profile   |
 * |    configuration        | configuration per flow      |
 * |-------------------------|-----------------------------|
 * | Parameter       | Value | Flow 1  | Flow 2  | Flow 3  |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   1   | Dynamic | Dynamic | Dynamic |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   2   |   SPS   |   SPS   |   SPS   |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   3   | Dynamic | Dynamic |   SPS   |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   4   |   SPS   |   SPS   | Dynamic |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   1   |   254   |   254   |   254   |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   2   |   255   |   254   |   255   |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   3   |    2    |   254   |   255   |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   1   |    1    |    1    |    1    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   2   |    1    |    2    |    3    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   3   |    2    |    2    |    1    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   4   |    1    |    1    |    2    |
 * |-----------------|-------|---------|---------|---------|
 * |-----------------|-------|---------|---------|---------|
 * | rriConfig       |   1   |   20    |   20    |   20    |
 * |-----------------|-------|---------|---------|---------|
 * | rriConfig       |   2   |   100   |    50   |   100   |
 *  -----------------|-------|---------|---------|---------
 *
 *  Three other parameters related to the scheduling that we configure are
 *  (1) 'prioToSps' which sets the scheduler attribute 'PriorityToSps' and
 *      is a flag to give scheduling priority to logical channels that are
 *      configured with SPS scheduling in case of priority tie
 *  (2) 'harqEnabled' which, if enabled, causes the scheduler to add additional
 *      resources for retransmissions, and
 *  (3) 'psfchPeriod' which affects scheduling by imposing requirements on
 *      the number of slots between retransmission attempts
 *
 * If 'psfchPeriod' is set to zero, then no PSFCH feedback channel will be
 * added to the resource pool, and the setting of 'harqEnabled' will instead
 * cause blind retransmissions to be scheduled
 *
 * The default configuration is:
 * --schedTypeConfig=1 (i.e., all LCs will have dynamic (per-packet) scheduling)
 * --dstL2IdConfig=1   (i.e., all LCs send to the same L2 ID of value 255)
 * --priorityConfig=1  (i.e., all LCs have the same priority of value 1)
 * --rriConfig=1       (i.e., all LCs have the same RRI, which doesn't matter in
 *                      this configuration as schedTypeConfig=1)
 * --prioToSps=false   (i.e., no priority to SPS, which doesn't matter in this
 *                      configuration as schedTypeConfig=1)
 * --harqEnabled=true  (retransmissions slots will be scheduled)
 * --psfchPeriod=4     (retransmissions slots will be scheduled)
 *
 * The selection of dstL2Id value configures the following:
 * - If the dstL2Id is 255, the CastType will be Broadcast
 * - If the dstL2Id is 254, the CastType will be Groupcast
 * - If the dstL2Id is 2, the CastType will be Unicast

 * The example will print on-screen the number of transmitted and received
 * packets during the simulation and the average packet delay.
 *
 * \code{.unparsed}
$ ./ns3 run "sl-multi-lc-example --help"
    \endcode
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <iomanip>
#include <ostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SlMultiLcExample");

uint32_t g_rxPktCounter = 0;            //!< Global variable to count RX packets
uint32_t g_txPktCounter = 0;            //!< Global variable to count TX packets
std::list<double> g_delays;             //!< Global list to store packet delays upon RX
std::ofstream g_fileGrantCreated;       //!< File stream for saving scheduling output
std::ofstream g_fileGrantPublished;     //!< File stream for saving scheduling output
std::ostringstream g_firstGrantCreated; //!< String stream for saving first scheduling output
bool g_firstGrant = true;  //!< Flag to control writing first grant to g_firstGrantCreated
bool g_writeTraces = true; //!< Flag to control writing traces

/*
 * Structure to keep track of the transmission time of the packets at the
 * application layer. Used to calculate packet delay.
 */
struct PacketWithRxTimestamp
{
    Ptr<const Packet> p;
    Time txTimestamp;
};

/*
 * Map to store received packets and reception timestamps at the application
 * layer. Used to calculate packet delay at the application layer.
 */
std::map<std::string, PacketWithRxTimestamp> g_rxPacketsForDelayCalc;

/*
 * \brief Trace sink function to count and logging the transmitted data packets
 *        and their corresponding transmission timestamp at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
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
    NS_LOG_DEBUG(" TX: " << mapKey);
}

/*
 * \brief Trace sink function to count and calculate the delay upon reception
 *        of a packet at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
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
    NS_LOG_DEBUG(" RX: " << mapKey << delay);
}

// Forward declarations
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

uint32_t GetPacketSize(double dataRateKbps, Time rri);

int
main(int argc, char* argv[])
{
    // Scenario parameters
    uint16_t interUeDistance = 20; // meters
    uint16_t enableSingleFlow = 0; // 0 corresponds to all flows

    // Traffic parameters
    uint32_t udpPacketSize = 200;
    double dataRate = 16; // 16 kilobits per second

    // Traffic profile parameters
    uint16_t schedTypeConfig = 1;
    uint16_t dstL2IdConfig = 1;
    uint16_t priorityConfig = 1;
    uint16_t rriConfig = 1;
    bool prioToSps = false;
    bool harqEnabled = true;
    uint16_t psfchPeriod = 4;
    // If packet delay budget is zero, model will use T2 as selection window edge
    Time pdb = Seconds(0);

    // Simulation parameters.
    Time trafficTime = Seconds(2.0);

    bool testing = false; // Basic validation check

    // NR parameters
    uint16_t numerologyBwpSl = 2;
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    uint16_t bandwidthBandSl = 400;         // Multiple of 100 KHz; 400 = 40 MHz
    double txPower = 23;                    // dBm

    CommandLine cmd(__FILE__);
    cmd.AddValue("trafficTime", "The time traffic will be active in seconds", trafficTime);
    cmd.AddValue("interUeDistance", "The distance among the UEs in the topology", interUeDistance);
    cmd.AddValue("packetSize",
                 "packet size in bytes to be used by best effort traffic",
                 udpPacketSize);
    cmd.AddValue("dataRate",
                 "The data rate in kilobits per second for best effort traffic",
                 dataRate);
    cmd.AddValue("schedTypeConfig",
                 "The scheduling configuration for the three traffic profiles",
                 schedTypeConfig);
    cmd.AddValue("dstL2IdConfig",
                 "The dstL2Id configuration for the three traffic profiles",
                 dstL2IdConfig);
    cmd.AddValue("priorityConfig",
                 "The priority configuration for the three traffic profiles",
                 priorityConfig);
    cmd.AddValue("rriConfig", "The RRI configuration for the three traffic profiles", rriConfig);
    cmd.AddValue(
        "prioToSps",
        "Give scheduling priority to SPS logical channels in case of a tie (if set to True)",
        prioToSps);
    cmd.AddValue("harqEnabled", "Whether HARQ is enabled", harqEnabled);
    cmd.AddValue("psfchPeriod", "PSFCH period, in slots", psfchPeriod);
    cmd.AddValue("enableSingleFlow", "Enable single flow only (1, 2, or 3)", enableSingleFlow);
    cmd.AddValue(
        "testing",
        "Testing flag to do verification that the example is working as expected (if set to True)",
        testing);
    cmd.AddValue("writeTraces", "Flag to control the writing of output traces", g_writeTraces);

    // Parse the command line
    cmd.Parse(argc, argv);

    // Check command line values
    NS_ABORT_MSG_UNLESS(enableSingleFlow <= 3, "Unsupported value: " << enableSingleFlow);

    // Final simulation time
    Time slBearersActivationTime = Seconds(2.0);
    Time finalSlBearersActivationTime = slBearersActivationTime;
    Time finalSimTime = trafficTime + finalSlBearersActivationTime +
                        Seconds(0.05); // Give some extra simulation time for packets to be Rx

    /*
     * Default values for the simulation. We are progressively removing all
     * the instances of SetDefault, but we need it for legacy code (LTE)
     */
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Create UE nodes
    NodeContainer ueNodeContainer;
    uint16_t ueNum = 2;
    ueNodeContainer.Create(ueNum);

    // Assign position and mobility to the UEs
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < ueNum; i++)
    {
        positionAllocUe->Add(Vector(interUeDistance * i, 0.0, 1.5));
    }
    mobility.SetPositionAllocator(positionAllocUe);
    mobility.Install(ueNodeContainer);

    /* The default topology is the following:
     *
     *         UE1..........(20 m)..........UE2
     *   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)
     */
    Packet::EnableChecking();
    Packet::EnablePrinting();

    // Configure NR module
    auto nrHelper = CreateObject<NrSlHelper>();

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

    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));

    // NR Sidelink attribute of UE MAC, which are common for all the UEs
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrHelper->SetUeMacAttribute("T2", UintegerValue(33));
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

    // Install UE NetDevices
    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueNodeContainer, allBwps);

    /**************************** SL configuration *****************************/
    // SL scheduler
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(14));
    nrHelper->SetUeSlSchedulerAttribute("PriorityToSps", BooleanValue(prioToSps));

    nrHelper->PrepareUeForSidelink(ueNetDev, bwpIdContainer);

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(50);
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
    nrHelper->InstallNrSlPreConfiguration(ueNetDev, slPreConfigNr);

    /****************************** End SL Configuration ***********************/

    // Fix random streams
    int64_t streamBase{1000};
    int64_t streamsUsed{0};
    streamsUsed = nrHelper->AssignStreams(ueNetDev, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in NrHelper");

    // Configure internet
    InternetStackHelper internet;
    internet.Install(ueNodeContainer);
    streamBase = 2000;
    streamsUsed = internet.AssignStreams(ueNodeContainer, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in InternetStackHelper");

    // Target multicast address; target unicast address set below
    Ipv4Address groupAddress4("225.0.0.0"); // use multicast address as destination

    /************************** Traffic flows configuration ********************/
    /*
     * All traffic flows will be directed to the target IP (groupAddress) but each to a
     * different port. For each flow:
     * - we create a traffic profile (SlInfo),
     * - we create a traffic flow template (NrSlTft) using the traffic profile, and
     * - we activate a SL data radio bearer that will steer the traffic according to the TFT
     */

    // Create the traffic profiles
    uint32_t dstL2Broadcast = 255;
    uint32_t dstL2Groupcast = 254;
    uint32_t dstL2Unicast = 2; // IMSI 1 is assigned to first node, IMSI 2 to second
                               // Source L2 ID is the lower bits of the IMSI.

    SidelinkInfo slInfo1;
    slInfo1.m_harqEnabled = harqEnabled;
    slInfo1.m_pdb = pdb;
    SidelinkInfo slInfo2;
    slInfo2.m_harqEnabled = harqEnabled;
    slInfo2.m_pdb = pdb;
    SidelinkInfo slInfo3;
    slInfo3.m_harqEnabled = harqEnabled;
    slInfo3.m_pdb = pdb;

    // Assign the three traffic profile parameter values depending on the configuration
    switch (schedTypeConfig)
    {
    case 1:
        slInfo1.m_dynamic = true;
        slInfo2.m_dynamic = true;
        slInfo3.m_dynamic = true;
        break;
    case 2:
        slInfo1.m_dynamic = false;
        slInfo2.m_dynamic = false;
        slInfo3.m_dynamic = false;
        break;
    case 3:
        slInfo1.m_dynamic = true;
        slInfo2.m_dynamic = true;
        slInfo3.m_dynamic = false;
        break;
    case 4:
        slInfo1.m_dynamic = false;
        slInfo2.m_dynamic = false;
        slInfo3.m_dynamic = true;
        break;
    }

    switch (dstL2IdConfig)
    {
    case 1:
        slInfo1.m_dstL2Id = dstL2Groupcast;
        slInfo1.m_castType = SidelinkInfo::CastType::Groupcast;
        slInfo2.m_dstL2Id = dstL2Groupcast;
        slInfo2.m_castType = SidelinkInfo::CastType::Groupcast;
        slInfo3.m_dstL2Id = dstL2Groupcast;
        slInfo3.m_castType = SidelinkInfo::CastType::Groupcast;
        break;
    case 2:
        slInfo1.m_dstL2Id = dstL2Broadcast;
        slInfo1.m_castType = SidelinkInfo::CastType::Broadcast;
        slInfo2.m_dstL2Id = dstL2Groupcast;
        slInfo2.m_castType = SidelinkInfo::CastType::Groupcast;
        slInfo3.m_dstL2Id = dstL2Broadcast;
        slInfo3.m_castType = SidelinkInfo::CastType::Broadcast;
        break;
    case 3:
        slInfo1.m_dstL2Id = dstL2Unicast;
        slInfo1.m_castType = SidelinkInfo::CastType::Unicast;
        slInfo2.m_dstL2Id = dstL2Groupcast;
        slInfo2.m_castType = SidelinkInfo::CastType::Groupcast;
        slInfo3.m_dstL2Id = dstL2Broadcast;
        slInfo3.m_castType = SidelinkInfo::CastType::Broadcast;
        break;
    }

    switch (priorityConfig)
    {
    case 1:
        slInfo1.m_priority = 1;
        slInfo2.m_priority = 1;
        slInfo3.m_priority = 1;
        break;
    case 2:
        slInfo1.m_priority = 1;
        slInfo2.m_priority = 2;
        slInfo3.m_priority = 3;
        break;
    case 3:
        slInfo1.m_priority = 2;
        slInfo2.m_priority = 2;
        slInfo3.m_priority = 1;
        break;
    case 4:
        slInfo1.m_priority = 1;
        slInfo2.m_priority = 1;
        slInfo3.m_priority = 2;
        break;
    }

    switch (rriConfig)
    {
    case 1:
        slInfo1.m_rri = MilliSeconds(20);
        slInfo2.m_rri = MilliSeconds(20);
        slInfo3.m_rri = MilliSeconds(20);
        break;
    case 2:
        slInfo1.m_rri = MilliSeconds(100);
        slInfo2.m_rri = MilliSeconds(50);
        slInfo3.m_rri = MilliSeconds(100);
        break;
    }

    // Create TFTs and activate SL-DRBs
    Address remoteAddress1;
    Address remoteAddress2;
    Address remoteAddress3;
    Address localAddress1;
    Address localAddress2;
    Address localAddress3;
    uint16_t port1 = 8001;
    uint16_t port2 = 8002;
    uint16_t port3 = 8003;
    Ptr<NrSlTft> tft1;
    Ptr<NrSlTft> tft2;
    Ptr<NrSlTft> tft3;

    // Assign IP address for the UEs
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(ueNetDev);
    // Set target unicast address
    Ipv4Address unicastAddress4 = ueIpIface.GetAddress(1); // should be 7.0.0.2

    NS_LOG_DEBUG("Device 0 has address " << ueIpIface.GetAddress(0)); // 7.0.0.1
    NS_LOG_DEBUG("Device 1 has address " << ueIpIface.GetAddress(1)); // 7.0.0.2

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ipv4InterfaceContainer::Iterator i;
    for (i = ueIpIface.Begin(); i != ueIpIface.End(); ++i)
    {
        const auto [ipv4, index] = *i;
        auto ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ipv4);
        ueStaticRouting->SetDefaultMulticastRoute(index);
    }

    // Create TFTs for each traffic profile and corresponding addresses/port
    // Use groupAddress4 for both Groupcast and Broadcast cast types
    localAddress1 = InetSocketAddress(Ipv4Address::GetAny(), port1);
    if (dstL2IdConfig == 3)
    {
        remoteAddress1 = InetSocketAddress(unicastAddress4, port1);
        tft1 = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL, unicastAddress4, port1, slInfo1);
    }
    else
    {
        remoteAddress1 = InetSocketAddress(groupAddress4, port1);
        tft1 = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL, groupAddress4, port1, slInfo1);
    }

    remoteAddress2 = InetSocketAddress(groupAddress4, port2);
    localAddress2 = InetSocketAddress(Ipv4Address::GetAny(), port2);
    tft2 = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL, groupAddress4, port2, slInfo2);

    remoteAddress3 = InetSocketAddress(groupAddress4, port3);
    localAddress3 = InetSocketAddress(Ipv4Address::GetAny(), port3);
    tft3 = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL, groupAddress4, port3, slInfo3);

    // Activate SL data radio bearers for each traffic flow template and profile
    if (!enableSingleFlow || enableSingleFlow == 1)
    {
        nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDev, tft1);
    }
    if (!enableSingleFlow || enableSingleFlow == 2)
    {
        nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDev, tft2);
    }
    if (!enableSingleFlow || enableSingleFlow == 3)
    {
        nrHelper->ActivateNrSlBearer(finalSlBearersActivationTime, ueNetDev, tft3);
    }

    /*
     * Configure the applications:
     * Client app: OnOff application configure to generate CBR traffic
     * Server app: PacketSink application.
     */

    // Configure client applications
    std::string dataRateString = std::to_string(dataRate) + "kb/s";
    OnOffHelper sidelinkClient1("ns3::UdpSocketFactory", remoteAddress1);
    sidelinkClient1.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient1.SetConstantRate(DataRate(dataRateString),
                                    GetPacketSize(dataRate, slInfo1.m_rri));

    OnOffHelper sidelinkClient2("ns3::UdpSocketFactory", remoteAddress2);
    sidelinkClient2.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient2.SetConstantRate(DataRate(dataRateString),
                                    GetPacketSize(dataRate, slInfo2.m_rri));

    OnOffHelper sidelinkClient3("ns3::UdpSocketFactory", remoteAddress3);
    sidelinkClient3.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient3.SetConstantRate(DataRate(dataRateString),
                                    GetPacketSize(dataRate, slInfo3.m_rri));

    ApplicationContainer allClientApps;
    ApplicationContainer allServerApps;

    if (!enableSingleFlow || enableSingleFlow == 1)
    {
        // Install client applications on the first UE (Tx)
        ApplicationContainer clientApps1 = sidelinkClient1.Install(ueNodeContainer.Get(0));
        clientApps1.Start(finalSlBearersActivationTime);
        clientApps1.Stop(finalSimTime);
        allClientApps.Add(clientApps1);
    }

    if (!enableSingleFlow || enableSingleFlow == 2)
    {
        ApplicationContainer clientApps2 = sidelinkClient2.Install(ueNodeContainer.Get(0));
        clientApps2.Start(finalSlBearersActivationTime);
        clientApps2.Stop(finalSimTime);
        allClientApps.Add(clientApps2);
    }

    if (!enableSingleFlow || enableSingleFlow == 3)
    {
        ApplicationContainer clientApps3 = sidelinkClient3.Install(ueNodeContainer.Get(0));
        clientApps3.Start(finalSlBearersActivationTime);
        clientApps3.Stop(finalSimTime);
        allClientApps.Add(clientApps3);
    }

    if (!enableSingleFlow || enableSingleFlow == 1)
    {
        // Configure server applications and install them in the second UE (Rx)
        ApplicationContainer serverApps1;
        PacketSinkHelper sidelinkSink1("ns3::UdpSocketFactory", localAddress1);
        sidelinkSink1.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
        serverApps1 = sidelinkSink1.Install(ueNodeContainer.Get(1));
        serverApps1.Start(Seconds(2.0));
        allServerApps.Add(serverApps1);
    }

    if (!enableSingleFlow || enableSingleFlow == 2)
    {
        ApplicationContainer serverApps2;
        PacketSinkHelper sidelinkSink2("ns3::UdpSocketFactory", localAddress2);
        sidelinkSink2.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
        serverApps2 = sidelinkSink2.Install(ueNodeContainer.Get(1));
        serverApps2.Start(Seconds(2.0));
        allServerApps.Add(serverApps2);
    }

    if (!enableSingleFlow || enableSingleFlow == 3)
    {
        ApplicationContainer serverApps3;
        PacketSinkHelper sidelinkSink3("ns3::UdpSocketFactory", localAddress3);
        sidelinkSink3.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
        serverApps3 = sidelinkSink3.Install(ueNodeContainer.Get(1));
        serverApps3.Start(Seconds(2.0));
        allServerApps.Add(serverApps3);
    }

    /************************ END Traffic flows configuration ******************/

    /******************** Application packet tracing ***************************/
    if (g_writeTraces)
    {
        AsciiTraceHelper ascii;
        Ptr<OutputStreamWrapper> PacketTraceForDelayStream =
            ascii.CreateFileStream("NrSlAppRxPacketDelayTrace.txt");
        *PacketTraceForDelayStream->GetStream()
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

    if (g_writeTraces)
    {
        g_fileGrantCreated.open("sl-multi-lc-scheduling.dat", std::ofstream::out);
        g_fileGrantPublished.open("sl-multi-lc-scheduling-published.dat", std::ofstream::out);
    }
    auto ueDevice0 = ueNetDev.Get(0)->GetObject<NrUeNetDevice>();
    auto ueMac0 = ueDevice0->GetMac(0)->GetObject<NrSlUeMac>();
    PointerValue v;
    ueMac0->GetAttribute("NrSlUeMacScheduler", v);
    auto scheduler0 = v.Get<NrSlUeMacScheduler>()->GetObject<NrSlUeMacSchedulerDefault>();
    scheduler0->TraceConnect("GrantCreated", "0", MakeCallback(&TraceGrantCreated));
    scheduler0->TraceConnect("GrantPublished", "0", MakeCallback(&TraceGrantPublished));

    Simulator::Stop(finalSimTime);
    Simulator::Run();

    if (g_writeTraces)
    {
        g_fileGrantCreated.close();
        g_fileGrantPublished.close();
    }
    std::cout << "schedTypeConfig = " << schedTypeConfig << "; dstL2IdConfig = " << dstL2IdConfig;
    std::cout << " priorityConfig = " << priorityConfig << "; rriConfig = " << rriConfig
              << std::endl;
    std::cout << "prioToSps = " << prioToSps << "; harqEnabled = " << harqEnabled
              << "; psfchPeriod = " << psfchPeriod << std::endl;

    std::cout << "Total Tx packets = " << g_txPktCounter << std::endl;
    std::cout << "Total Rx packets = " << g_rxPktCounter << std::endl;
    double delaySum = 0;
    for (auto it = g_delays.begin(); it != g_delays.end(); it++)
    {
        delaySum += *it;
    }
    std::cout << "Average packet delay = " << delaySum / g_delays.size() << " ms" << std::endl;
    std::cout << "Output trace of first grant created:" << std::endl;
    std::cout << g_firstGrantCreated.str();

    Simulator::Destroy();
    if (testing)
    {
        if (g_rxPktCounter < 0.95 * g_txPktCounter)
        {
            std::cout << "Not enough packets received" << std::endl;
            return 1;
        }
    }
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
GetPacketSize(double dataRateKbps, Time rri)
{
    return static_cast<uint32_t>(dataRateKbps * 1000 * rri.GetSeconds() / 8);
}
