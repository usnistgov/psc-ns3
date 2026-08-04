//
// SPDX-License-Identifier: NIST-Software
//

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/nr-sl-prose-tag.h"
#include "ns3/point-to-point-module.h"
#include "ns3/propagation-module.h"
#include "ns3/psc-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrProseAdaptiveMcs");

struct PacketInfo
{
    Time txTime;
    Time rxTime;
    Time rxDelay;
    uint16_t size; // Bytes

    PacketInfo()
        : txTime(Seconds(0)),
          rxTime(Seconds(0)),
          rxDelay(Seconds(0)),
          size(0)
    {
    }
};

using PacketTrace = std::map<uint32_t, PacketInfo>; // seqNum -> PacketInfo;

struct AppInfo
{
    uint32_t appId;
    uint32_t srcNodeId;
    uint32_t tgtNodeId;
    Ipv4Address srcUeIpAddress;
    Ipv4Address tgtUeIpAddress;
    PacketTrace pktTrace;
};

class NodePhyStats
{
  public:
    struct PhyStats
    {
        uint64_t nTxCtrl = 0;             // PSCCH
        uint64_t nTxData = 0;             // PSSCH
        uint64_t nRxCtrl = 0;             // Successfully decoded PSCCH
        uint64_t nRxData = 0;             // Successfully decoded PSSCH
        uint64_t nCorruptRxCtrl = 0;      // PSCCH corrupted (BLER)
        uint64_t nCorruptRxData = 0;      // PSSCH corrupted (BLER), combined SCI-2a + TB
        uint64_t nCorruptRxDataSci2 = 0;  // PSSCH corrupted due to SCI-2a decode failure
        uint64_t nCorruptRxDataTb = 0;    // PSSCH corrupted due to data-TB decode failure
        uint64_t nHdRxCtrl = 0;           // PSCCH ignored due to half duplex
        uint64_t nHdRxData = 0;           // PSSCH ignored due to half duplex
        uint64_t nDataNotExpected = 0;    // Not expected (PSCCH not received)
        uint64_t nDataAlreadyDecoded = 0; // Not expected (retransmission of an already decoded TB)
    };

    uint32_t m_nodeId;
    uint32_t m_l2Id;
    PhyStats m_appStats;
    PhyStats m_pc5sStats;
};

class RlcStats : public Object
{
  public:
    uint64_t m_nTxPdu = 0;
    uint64_t m_nDroppedPdu = 0;
    uint8_t m_lcId = 0;
    uint32_t m_nodeId = 0;
    uint32_t m_dstL2Id = 0;
};

class NodeRlcStats : public Object
{
  public:
    uint32_t m_nodeId = 0;
    Ptr<NrUeNetDevice> m_netDevice;
    std::map<std::pair<uint32_t, uint8_t>, Ptr<RlcStats>> m_rlcStats; // (dstL2Id, lcId) -> stats
};

/**
 * Per-node counters for scheduling resource usage.
 * The 'nSubchannels' counts the resources used (with granularity of a subchannel)
 * e.g., if a grant uses three subchannels and recurs ten times, count would increment by thirty
 */
struct NodeSchedulingStats
{
    uint32_t m_nodeId = 0;
    uint32_t m_l2Id = 0;
    uint16_t m_totalSubchannels = 0;  //!< Total subchannels in pool (from SchedulingReport)
    uint64_t m_nGrants = 0;           //!< Number of grants created
    uint64_t m_nSubchannels = 0;      //!< Total resource allocations, units of subchannels
    uint64_t m_nSlotsWithNewData = 0; //!< Slots carrying new data (NDI=1)
    uint64_t m_nSlotsWithRetx = 0;    //!< Slots carrying retransmissions (NDI=0)
};

NodeContainer CreateGrid(uint32_t gridSize, double distance, Ptr<PropagationLossModel> lossModel);

NodeContainer CreateRing(uint32_t nNodes, double distance, Ptr<PropagationLossModel> lossModel);

void DisableLoss(Ptr<MatrixPropagationLossModel> lossModel, Ptr<Node> a, Ptr<Node> b);
/**
 * @brief Find a node in a container by its ID.
 *
 * @param id     The identifier of the node to locate.
 * @param nodes  The NodeContainer to search.
 * @return Ptr<Node>  Pointer to the node if found; nullptr otherwise.
 */
Ptr<Node> FindNodeById(uint32_t id, NodeContainer nodes);
/**
 * @brief Trace sink for the L1 SD-RSRP measurements report.
 * @param stream        Pointer to the stream wrapper object
 * @param selfL2Id      The L2 Id of the measuring node
 * @param rnti          The RNTI
 * @param peerL2Id      The L2 Id of the measured node
 * @param l1sdRsrp      The L1 SD-RSRP measurement
 */
void L1SdRsrpMeasurementTrace(Ptr<OutputStreamWrapper> stream,
                              std::string selfL2Id,
                              uint16_t rnti,
                              uint32_t peerL2Id,
                              double l1sdRsrp);
/**
 * @brief Trace sink for the L3 SD-RSRP measurements report.
 * @param stream        Pointer to the stream wrapper object
 * @param selfL2Id      The L2 Id of the measuring node
 * @param peerL2Id      The L2 Id of the measured node
 * @param l3sdRrsrp     The L3 SD-RSRP measurement
 * @param thCond        Threshold condition flag
 */
void L3SdRsrpMeasurementTrace(Ptr<OutputStreamWrapper> stream,
                              uint32_t selfL2Id,
                              uint32_t peerL2Id,
                              double l3sdRrsrp,
                              bool thCond);
/**
 * @brief Trace sink for the link establishment notificiation.
 * @param stream        Pointer to the stream wrapper object
 * @param context       Node ID
 * @param selfL2Id      The L2 Id of the establishing node
 * @param selfIpv4Addr  The IPv4 address of the establishing node
 * @param peerL2Id      The L2 Id of the peer node
 * @param peerIpv4Addr  The IPv4 address of the peer node
 */
void NotifyLinkEstablished(Ptr<OutputStreamWrapper> stream,
                           std::string context,
                           uint32_t selfL2Id,
                           Ipv4Address selfIpv4Addr,
                           uint32_t peerL2Id,
                           Ipv4Address peerIpv4Addr);
/**
 * @brief Trace sink for the link release notificiation.
 * @param stream        Pointer to the stream wrapper object
 * @param context       Node ID
 * @param selfL2Id      The L2 Id of the releasing node
 * @param selfIpv4Addr  The IPv4 address of the releasing node
 * @param peerL2Id      The L2 Id of the peer node
 * @param peerIpv4Addr  The IPv4 address of the peer node
 */
void NotifyLinkReleasing(Ptr<OutputStreamWrapper> stream,
                         std::string context,
                         uint32_t selfL2Id,
                         Ipv4Address selfIpv4Addr,
                         uint32_t peerL2Id,
                         Ipv4Address peerIpv4Addr);
/**
 * @brief Method to listen to the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * @param pscchStats Pointer to the \link UeMacPscchTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param pscchStatsParams Parameters of the trace source.
 */
void NotifySlPscchScheduling(UeMacPscchTxOutputStats* pscchStats,
                             const SlPscchUeMacStatParameters pscchStatsParams);
/**
 * @brief Method to listen to the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * @param psschStats Pointer to the \link UeMacPsschTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param psschStatsParams Parameters of the trace source.
 */
void NotifySlPsschScheduling(UeMacPsschTxOutputStats* psschStats,
                             const SlPsschUeMacStatParameters psschStatsParams);
/**
 * @brief Method to listen to the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * @param pscchStats Pointer to the \link UePhyPscchRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param pscchStatsParams Parameters of the trace source.
 */
void NotifySlPscchRx(UePhyPscchRxOutputStats* pscchStats,
                     const SlRxCtrlPacketTraceParams pscchStatsParams);
/**
 * @brief Method to listen to the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * @param psschStats Pointer to the \link UePhyPsschRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param psschStatsParams Parameters of the trace source.
 */
void NotifySlPsschRx(UePhyPsschRxOutputStats* psschStats,
                     const SlRxDataPacketTraceParams psschStatsParams);
/**
 * @brief Post-process and export aggregated RLC PDU statistics.
 * @param nodesRlcStats Map from nodeId to per-node RLC statistics container.
 */
void ProcessRlcStats(const std::map<uint32_t, Ptr<NodeRlcStats>>& nodesRlcStats);
/**
 * @brief Aggregate per‐node PHY counters, print tables, calculate balances, and write CSVs.
 * @param nodePhyStats  Map from node ID to its NodePhyStats pointer.
 */
void ProcessPhyStats(const std::map<uint32_t, NodePhyStats*>& nodePhyStats);
/**
 * @brief Record reception time, delay, and size for an application packet.
 * @param app           The MCPTT application
 * @param callId        The ID of the call
 * @param pkt           The packet that was received
 * @param headerType    The header type of the received packet
 */
void ReceivePacketMcpttApp(PacketTrace* pktTrace,
                           Ptr<const Application> app,
                           uint16_t callId,
                           Ptr<const Packet> pkt,
                           const TypeId& headerType);
/**
 * @brief Record reception time, delay, and size for an application packet.
 * @param pktTrace  Pointer to the pacet trace map
 * @param packet    The Packet being received.
 * @param from      Source Address.
 * @param to        Destination Address.
 * @param header    SeqTsSizeHeader carrying seq and timestamp.
 */
void ReceivePacketSeqTs(PacketTrace* pktTrace,
                        Ptr<const Packet> packet,
                        const Address& from,
                        const Address& to,
                        const SeqTsSizeHeader& header);
void SaveNodePositions(const ns3::NodeContainer& nodes, std::ofstream* file);
/**
 * @brief Trace sink for SL channels (PSCCH/PSSCH) events that updates PHY statistics.
 * @param stats    Pointer to the NodePhyStats object
 * @param type     The SL event type
 * @param pktBurst The PacketBurst associated with this event
 */
void TraceSlChannelsEvent(NodePhyStats* stats,
                          Ptr<OutputStreamWrapper> slChannelEventsTrace,
                          const NrSlSpectrumPhy::SlEventType type,
                          Ptr<const PacketBurst> pktBurst);
/**
 * @brief Trace sink for NrSlSpectrumPhy::RxPsschTraceUe. Emits one CSV row per
 *        PSSCH decode attempt (both the SCI-2a-corrupt short-circuit path and
 *        the full TB-decode path), exposing the runtime error-model inputs and
 *        output TBLER.  Used to directly compare the runtime LLS table against
 *        rendered BLER-vs-SNR curves.
 * @param stream Output stream for the CSV.
 * @param params Trace source parameters describing the decode attempt.
 */
void TracePsschPerTb(Ptr<OutputStreamWrapper> stream, const SlRxDataPacketTraceParams params);
/**
 * @brief Write one row per PSSCH scheduling decision to the trace CSV.
 *
 * Driven by NrSlUeMac::SlPsschScheduling. Each row carries the per-slot
 * scheduling info plus the announced numTx of the TB transmitted in this
 * slot. NDI=1 rows correspond to first transmissions and can be filtered
 * downstream to recover per-TB quantities.
 *
 * @param stream Output stream for the CSV.
 * @param params Trace source parameters for the scheduled PSSCH transmission.
 */
void TracePsschSched(Ptr<OutputStreamWrapper> stream, const SlPsschUeMacStatParameters params);
void TraceMcsChange(Ptr<OutputStreamWrapper> stream,
                    std::string context,
                    uint32_t dstL2Id,
                    uint8_t oldMcs,
                    uint8_t newMcs);
/**
 * @brief Trace sink for the SchedulingReport trace of NrSlUeMacSchedulerDefault.
 *        Tracks per-node (subchannels) resource usage.
 * @param stats      Per-node scheduling statistics counter
 * @param report     Scheduling report with pool configuration
 * @param candidateResources  Candidate resources considered by sensing
 * @param params     Transmission parameters
 * @param publishedGrants  Published grant resources
 * @param existingGrants   Existing grants by destination L2 ID
 * @param grant      The grant being scheduled
 */
void TraceSchedulingReport(
    NodeSchedulingStats* stats,
    const NrSlUeMacSchedulerDefault::SchedulingReport& report,
    const std::list<SlResourceInfo>& candidateResources,
    const NrSlUeMac::NrSlTransmissionParams& params,
    const std::vector<SlGrantResource>& publishedGrants,
    const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& existingGrants,
    const NrSlUeMacScheduler::GrantInfo& grant);
/**
 * @brief Post-process and export scheduling resource utilization statistics.
 * @param schedulingStats  Map from node ID to per-node scheduling stats.
 * @param simTime          Total simulation time.
 * @param numerology       SL BWP numerology (0, 1, 2, ...).
 * @param slBitmapFraction Fraction of slots eligible for SL (e.g. 1.0 if all slots).
 */
void ProcessSchedulingStats(const std::map<uint32_t, NodeSchedulingStats*>& schedulingStats,
                            Time simTime,
                            uint16_t numerology,
                            double slBitmapFraction);
/**
 * @brief Post-process and export application-level packet delivery statistics.
 * @param appInfoList  List of per-application packet trace data.
 */
void ProcessAppStats(const std::list<AppInfo>& appInfoList);
/**
 * @brief Trace sink for RLC PDU transmissions that updates RLC statistics.
 * @param stats  Per-bearer RLC statistics object
 * @param rnti   UE RNTI
 * @param lcId   Logical channel ID
 * @param pktSize Size in bytes
 */
void TraceRlcTxPdu(Ptr<RlcStats> stats, uint16_t /*rnti*/, uint8_t /*lcId*/, uint32_t /*pktSize*/);
/**
 * @brief Trace sink for RLC PDU transmissions drops that updates RLC statistics.
 * @param rlcDropsTrace Output stream wrapper used to log drop events
 * @param stats         Per-bearer RLC statistics object
 * @param p             Packet instance associated with the drop event.
 */
void TraceRlcTxDrop(Ptr<OutputStreamWrapper> rlcDropsTrace,
                    Ptr<RlcStats> stats,
                    Ptr<const Packet> p);
/**
 * @brief Trace sink invoked when a sidelink logical channel is added.
 * @param rlcDropsTrace Output stream wrapper used to log drop events to CSV
 * @param nodeRlcStats  Per-node RLC statistics container
 * @param params        Logical channel information reported by the scheduler trace.
 */
void TraceLogicalChannelAddition(Ptr<OutputStreamWrapper> rlcDropsTrace,
                                 Ptr<NodeRlcStats> nodeRlcStats,
                                 const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params);
/**
 * @brief Record transmission time and size for an application packet.
 *
 * @param app           The MCPTT application
 * @param callId        The ID of the call
 * @param pkt           The packet that was transmitted
 * @param headerType    The header type of the transmitted packet
 */
void TransmitPacketMcpttApp(PacketTrace* pktTrace,
                            Ptr<const Application> app,
                            uint16_t callId,
                            Ptr<const Packet> pkt,
                            const TypeId& headerType);
/**
 * @brief Record transmission time and size for an application packet.
 *
 * @param pktTrace  Pointer to the pacet trace map
 * @param packet    The Packet being transmitted.
 * @param from      Source Address.
 * @param to        Destination Address.
 * @param header    SeqTsSizeHeader carrying seq and size metadata.
 */
void TransmitPacketSeqTs(PacketTrace* pktTrace,
                         Ptr<const Packet> packet,
                         const Address& from,
                         const Address& to,
                         const SeqTsSizeHeader& header);

void TrialResult(Ptr<OutputStreamWrapper> trace,
                 std::string nodeId,
                 uint8_t mcs,
                 uint32_t dstL2Id,
                 bool success);

void SinrEstimate(Ptr<OutputStreamWrapper> trace,
                  std::string nodeId,
                  uint32_t dstL2Id,
                  double unfilteredSinrDb,
                  double filteredSinrDb,
                  double sinrOffsetDb,
                  double sinrEstimateDb);

void TraceHarqFeedbackReceived(Ptr<OutputStreamWrapper> trace,
                               std::string nodeId,
                               const SlHarqInfo& info);

void TraceHarqTbCompletion(Ptr<OutputStreamWrapper> trace,
                           std::string nodeId,
                           const SlHarqInfo& info);

int
main(int argc, char* argv[])
{
    // Topology parameters
    std::string layout("ring");
    uint32_t bgPktSize = 60; // in bytes
    DataRate bgDataRate = DataRate("24kb/s");
    uint32_t mediaPktSize = 60; // in bytes
    DataRate mediaDataRate = DataRate("24kb/s");
    double d(5); // meters

    // Simulation timeline parameters
    Time simTime(Seconds(180));        // Total simulation time
    Time startTrafficTime(Seconds(3)); // Time to start the traffic in the application layer

    // NR SL parameters
    bool chatterbox =
        true;         // Flag to indicate if one or both nodes in a pair should push the PTT button
    bool harq = true; // Flag to enable harq
    uint16_t numerologyBwpSl(0);            // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6);   // band n14 (793 MHz)
    uint16_t bandwidthBandSl(100);          // Multiple of 100 KHz; 100 = 10 MHz
    double txPower(23);                     // Units of dBm
    uint32_t mcs(0);                        // The default MCS to be used by the MAC scheduler
    bool sensing(true);                     // Flag to enable sensing-based resource selection
    bool wSlotEx(true);                     // Flag to enable Whole slot exclusion upon sensing
    std::string rtxType("Feedback1");       // Retransmission scheme (No/Blind/Feedback1/Feedback2)
    uint32_t maxNumTx(1);                   // The maximum number of transmissions in the PSSCH
    std::string lossModelType("umi");       // propagation loss model
    std::string errorModelType("epa");      // error model type (static or epa)
    std::string mcsControllerType("ideal"); // MCS controller type (static or ideal)

    uint32_t bgPairs(7);       // Number of background traffic application pairs
    uint32_t mcpttPairs(1);    // Number of MCPTT pairs
    uint16_t appPerHopPdb(20); // PDB defining selection window length for the VoIP traffic (ms)

    // Internal
    bool logMacPhyInDb(false); // Flag to enable database logging for MAC and PHY SL
    bool log(false);           // Flag to enable logging
    std::string idealSchedLevel(
        "Global"); // The level of knowledge from external nodes used by the SL scheduler when
                   // filtering candidate resources (None/Global/LocalUnicast)
    bool disableRlcPduDiscard(false);  // Flag to disable the RLC PDB enforcement
    bool writeSchedulingTraces(false); // Flag to enable writing detailed sensing/scheduling traces

    // Discovery and SD-RSRP meassurement
    double rsrpThreshold(-111);      // The RSRP threshold (dBm) when enabling SD-RSRP/SL-RSRP
    double rsrpCoefficient(0);       // The L3 filter coefficient when enabling SD-RSRP/SL-RSRP
    double rsrpHysteresis(0);        // The hysteresis when enabling SD-RSRP/SL-RSRP
    Time discInterval(Seconds(2.0)); // The periodicity of the discovery messages

    CommandLine cmd;

    cmd.AddValue("bgPairs", "The number of background traffic application pairs", bgPairs);
    cmd.AddValue("mcpttPairs", "The number of MCPTT application pairs", mcpttPairs);
    cmd.AddValue("layout", "The layout of the nodes (ring or grid)", layout);
    cmd.AddValue("bgPktSize", "The size (in bytes) of a background traffic packet", bgPktSize);
    cmd.AddValue("bgDataRate",
                 "The data rate at which background traffic packets will be sent",
                 bgDataRate);
    cmd.AddValue("mediaPktSize", "The size (in bytes) of an MCPTT media packet", mediaPktSize);
    cmd.AddValue("mediaDataRate",
                 "The data rate at which MCPTT media packets will be sent",
                 mediaDataRate);
    cmd.AddValue(
        "chatterbox",
        "Indicates if one (true) or both (false) nodes in a pair should push the PTT button",
        chatterbox);
    cmd.AddValue("harq", "Enable HARQ feedback", harq);
    cmd.AddValue("simTime", "Simulation time, in seconds", simTime);
    cmd.AddValue("d", "Distance between adjacent nodes in the grid, in meters", d);
    cmd.AddValue("lossModel", "Loss model type (matrix, friis, log-distance, umi)", lossModelType);
    cmd.AddValue("errorModel", "Error model type (static, epa)", errorModelType);
    cmd.AddValue("mcsController", "MCS controller type (static, ideal)", mcsControllerType);
    cmd.AddValue("mcs", "The MCS to be used in the SL", mcs);
    cmd.AddValue("appPerHopPdb",
                 "The per-hop packet delay budget for the VoIP traffic, in milliseconds",
                 appPerHopPdb);
    cmd.AddValue("wSlotEx",
                 "Whether to exclude all resources of the slot upon sensing a transmission in a "
                 "resource of the slot",
                 wSlotEx);
    cmd.AddValue("rtxType",
                 "The type of retransmission scheme used in the SL. Options: "
                 "No/Blind/Feedback1/Feedback2",
                 rtxType);
    cmd.AddValue("maxNumTx", "The maximum number of transmissions in the PSSCH", maxNumTx);
    cmd.AddValue("idealSchedLevel",
                 "The level of knowledge from external nodes used by the SL scheduler when "
                 "filtering candidate resources. Options: None/Global/LocalUnicast",
                 idealSchedLevel);
    cmd.AddValue("rsrpThreshold", "The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold", rsrpThreshold);
    cmd.AddValue("rsrpCoefficient", "The SD-RSRP and/or SL-RSRP coefficent", rsrpCoefficient);
    cmd.AddValue("rsrpHysteresis", "The SD-RSRP and/or SL-RSRP (dB) hysteresis", rsrpHysteresis);
    cmd.AddValue("writeSchedulingTraces",
                 "Whether to write scheduling and sensing traces",
                 writeSchedulingTraces);
    cmd.AddValue("discInterval", "The periodicity of the discovery messages", discInterval);

    // Configuration defaults that could be overridden at command line
    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::psc::McpttPusherOrchestrator::MinIat", TimeValue(Seconds(1.0)));
    Config::SetDefault("ns3::psc::McpttOffNetworkFloorParticipant::T203", TimeValue(Seconds(0.5)));
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::NrSlRlcUm::DiscardTimerScale", DoubleValue(4.0));
    Config::SetDefault("ns3::NrSlUeMacSchedulerDefault::MinimumSpsGrantSize", UintegerValue(80));
    Config::SetDefault("ns3::NrSlUeMacSchedulerDefault::SpsReselectionThreshold",
                       UintegerValue(std::numeric_limits<uint16_t>::max()));

    // Target BLER of 3% or less is necessary to keep voice MOS >= 4
    Config::SetDefault("ns3::NrSlOllaMcsController::TargetBler", DoubleValue(0.03));
    Config::SetDefault("ns3::NrSlIdealMcsController::TargetBler", DoubleValue(0.03));
    Config::SetDefault("ns3::NrSlThompsonSamplingMcsController::TargetBler", DoubleValue(0.03));

    // Parse the command line
    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(simTime <= startTrafficTime,
                    "Configuration error, simTime must be greater than startTime");
    // Configure discovery interval and L1 SD-RSRP measurement interval to four discovery intervals
    // (i.e., T_measure = 4 in TS 38.133 Section 12.10, table 12.10.2-1)
    Config::SetDefault("ns3::NrSlUeProse::DiscoveryInterval", TimeValue(discInterval));
    Config::SetDefault("ns3::NrSlUePhy::RsrpFilterPeriod", TimeValue(4 * discInterval));

    if (log)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
        LogComponentEnable("NrSlUeProseDirectLink", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeProse", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeService", LOG_LEVEL_ALL);
        LogComponentEnable("NrProseMultihopAdvanced", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlEpcUeNas", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeRrc", LOG_LEVEL_ALL);
    }

    if (disableRlcPduDiscard)
    {
        Config::SetDefault("ns3::NrSlRlcUm::EnablePdcpDiscarding", BooleanValue(false));
    }

    // Create channel before nodes
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    Ptr<PropagationLossModel> lossModel;
    if (lossModelType == "matrix")
    {
        lossModel = CreateObject<MatrixPropagationLossModel>();
    }
    else if (lossModelType == "friis")
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
        NS_FATAL_ERROR("Other loss models not yet supported");
    }
    channel->AddPropagationLossModel(lossModel);

    // UE nodes creation and deployment
    uint32_t nAppPairs = mcpttPairs + bgPairs;
    uint32_t nNodes = 2 * nAppPairs;
    NodeContainer nodes;
    if (layout == "ring")
    {
        nodes = CreateRing(nNodes, d, lossModel);
    }
    else if (layout == "grid")
    {
        nodes = CreateGrid(nNodes, d, lossModel);
    }
    else
    {
        NS_ABORT_MSG("Layout " << layout << " not supported.");
    }

    // Start NR SL configuration
    auto nrSlHelper = CreateObject<NrSlHelper>();

    // Configure error model
    if (errorModelType == "static")
    {
        nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
    }
    else if (errorModelType == "epa")
    {
        nrSlHelper->SetSlErrorModelTypeId(NrSlEpaErrorModel::GetTypeId());
    }

    // Configure MCS controller
    if (mcsControllerType == "static")
    {
        ; // no-op; will default to MCS 0
    }
    else if (mcsControllerType == "ideal")
    {
        nrSlHelper->SetMcsControllerTypeId(NrSlIdealMcsController::GetTypeId());
    }
    else if (mcsControllerType == "ts")
    {
        nrSlHelper->SetMcsControllerTypeId(NrSlThompsonSamplingMcsController::GetTypeId());
    }
    else if (mcsControllerType == "olla")
    {
        nrSlHelper->SetMcsControllerTypeId(NrSlOllaMcsController::GetTypeId());
    }
    else if (mcsControllerType == "none")
    {
        ; // no-op
    }
    else
    {
        NS_ABORT_MSG("MCS controller type not recognized");
    }

    // NR SL will use one BWP
    std::unique_ptr<BandwidthPartInfo> bwpInfo(new BandwidthPartInfo());
    bwpInfo->m_bwpId = 0;
    bwpInfo->m_centralFrequency = centralFrequencyBandSl;
    bwpInfo->m_lowerFrequency = centralFrequencyBandSl - bandwidthBandSl * 1e5 / 2;
    bwpInfo->m_higherFrequency = centralFrequencyBandSl + bandwidthBandSl * 1e5 / 2;
    bwpInfo->SetChannel(channel);
    BandwidthPartInfoPtrVector allBwps;
    allBwps.push_back(bwpInfo);

    // Configure UEs antennas
    nrSlHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrSlHelper->SetUeAntennaAttribute("AntennaElement",
                                      PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrSlHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));

    // Configure UEs MAC
    nrSlHelper->SetUeMacAttribute("EnableSensing", BooleanValue(sensing));
    nrSlHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrSlHelper->SetUeMacAttribute("T2", UintegerValue(20));
    nrSlHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));
    nrSlHelper->SetUeMacAttribute("Tproc0", UintegerValue(0));
    nrSlHelper->SetUeMacAttribute("MinTimeGapProcessing", UintegerValue(0)); // HARQ Feedback

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

    NetDeviceContainer manetNetDevs = nrSlHelper->InstallUeDevice(nodes, allBwps);

    // Configure scheduler
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(mcs));
    nrSlHelper->SetUeSlSchedulerAttribute("WholeSlotExclusion", BooleanValue(wSlotEx));
    nrSlHelper->SetUeSlSchedulerAttribute("IdealSchedLevel", StringValue(idealSchedLevel));

    // Install NR configuration in the UEs
    nrSlHelper->PrepareUeForSidelink(manetNetDevs, bwpIdContainer);

    // Continue configuration of NR SL MAC resources pool
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100);
    ptrFactory->SetSlSelectionWindow(1);
    ptrFactory->SetSlFreqResourcePscch(10);
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(3);
    std::list<uint16_t> resourceReservePeriodList = {0, 20, 100}; // in ms
    ptrFactory->SetSlResourceReservePeriodList(resourceReservePeriodList);
    ptrFactory->SetSlMinTimeGapPsfch(2);
    if (rtxType == "Blind")
    {
        ptrFactory->SetSlPsfchPeriod(0); // 0 (blind)
    }
    else if (rtxType == "Feedback1")
    {
        ptrFactory->SetSlPsfchPeriod(1); // 1 (every slot)
    }
    else if (rtxType == "Feedback2")
    {
        ptrFactory->SetSlPsfchPeriod(2); // 2 (every 2 slots)
    }
    else if (rtxType == "Feedback4")
    {
        ptrFactory->SetSlPsfchPeriod(4); // 4 (every 4 slots)
    }
    else
    {
        ptrFactory->SetSlPsfchPeriod(0); // 0 (blind)
    }
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;
    NrSlRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
    slBwpPoolConfigCommonNr.slTxPoolSelectedNormal[slResourcePoolIdNr.id] = slresoPoolConfigNr;

    // Configure the BWP IE
    NrSlRrcSap::Bwp bwp;
    bwp.numerology = numerologyBwpSl;
    bwp.symbolsPerSlots = 14;
    bwp.rbPerRbg = 1;
    bwp.bandwidth = bandwidthBandSl;
    NrSlRrcSap::SlBwpGeneric slBwpGeneric;
    slBwpGeneric.bwp = bwp;
    slBwpGeneric.slLengthSymbols = NrSlRrcSap::GetSlLengthSymbolsEnum(14);
    slBwpGeneric.slStartSymbol = NrSlRrcSap::GetSlStartSymbolEnum(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
    slBwpConfigCommonNr.haveSlBwpGeneric = true;
    slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
    slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
    slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
    for (const auto& it : bwpIdContainer)
    {
        slFreConfigCommonNr.slBwpList[it] = slBwpConfigCommonNr;
    }

    // Configure TDD pattern
    NrSlRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
    tddUlDlConfigCommon.tddPattern = "UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|";
    NrSlRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
    slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

    // Configure the SlUeSelectedConfig IE
    NrSlRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
    slUeSelectedPreConfig.slProbResourceKeep = 0;
    // Configure the SlPsschTxParameters IE
    NrSlRrcSap::SlPsschTxParameters psschParams;
    psschParams.slMaxTxTransNumPssch = maxNumTx;
    // Configure the SlPsschTxConfigList IE
    NrSlRrcSap::SlPsschTxConfigList pscchTxConfigList;
    pscchTxConfigList.slPsschTxParameters[0] = psschParams;
    slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

    // Communicate the above pre-configuration to the NrSlHelper
    NrSlRrcSap::SidelinkPreconfigNr slPreConfigNr;
    slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
    slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
    slPreConfigNr.slPreconfigFreqInfoList[0] = slFreConfigCommonNr;
    nrSlHelper->InstallNrSlPreConfiguration(manetNetDevs, slPreConfigNr);

    // Configure the IPv4 stack
    /*
     * Configure the IPv4 stack
     */
    InternetStackHelper internet;
    internet.Install(nodes);
    int64_t streamBase = 1000;
    int64_t streamsUsed = internet.AssignStreams(nodes, streamBase);
    NS_LOG_DEBUG("Used " << streamsUsed << " random variable streams in InternetStackHelper");
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(manetNetDevs);

    std::vector<Ipv4Address> ipv4AddressVector(nNodes);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    for (uint32_t u = 0; u < nodes.GetN(); ++u)
    {
        Ptr<Node> ueNode = nodes.Get(u);
        // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of
        // the direct link
        ipv4AddressVector[u] =
            nodes.Get(u)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
    }
    //
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    // Install ProSe layer and corresponding SAPs in the UEs
    nrSlProseHelper->PrepareUesForProse(manetNetDevs);
    // Configure ProSe Unicast parameters. At the moment it only instruct the MAC
    // layer (and PHY therefore) to monitor packets directed the UE's own Layer 2 ID
    nrSlProseHelper->PrepareUesForUnicast(manetNetDevs);
    // Configure the value of timer Timer T5080 (Prose Direct Link Establishment Request
    // Retransmission) to a lower value than the standard (8.0 s) to speed connection in shorter
    // simulation time
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5080", TimeValue(Seconds(1.0)));

    NrSlRrcSap::SlRemoteUeConfig slRemoteConfig;
    slRemoteConfig.slReselectionConfig.slRsrpThres = rsrpThreshold; // dBm
    slRemoteConfig.slReselectionConfig.slFilterCoefficientRsrp = rsrpCoefficient;
    slRemoteConfig.slReselectionConfig.slHystMin = rsrpHysteresis; // dB

    // Define SD-/SL-RSRP thresholds and enable RSRP measurements for UEs
    for (uint32_t i = 0; i < manetNetDevs.GetN(); ++i)
    {
        Ptr<NrSlUeRrc> remoteRrc =
            manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetRrc()->GetObject<NrSlUeRrc>();
        remoteRrc->EnableUeSdRsrpMeasurements();
        remoteRrc->SetNrSlDiscoveryRemoteConfiguration(slRemoteConfig);
    }

    std::vector<std::pair<uint32_t, uint32_t>> appPairs;
    for (uint32_t i = 0; i < nAppPairs; i++)
    {
        appPairs.push_back({i + 1, nNodes - i});
    }

    ns3::psc::McpttHelper mcpttHelper;
    mcpttHelper.SetMediaSrc("ns3::psc::McpttMediaSrc",
                            "Bytes",
                            UintegerValue(mediaPktSize),
                            "DataRate",
                            DataRateValue(mediaDataRate));
    mcpttHelper.SetPusher("ns3::psc::McpttPusher", "Automatic", BooleanValue(false));

    uint32_t appPairId = 1;
    std::list<AppInfo> appInfoList;
    uint16_t mediaPort = ns3::psc::McpttPttApp::GetCurrentMediaPortNumber();

    NrSlTraceHelper nrSlTraceHelper;
    std::map<uint32_t, NodeSchedulingStats*> schedulingStats; // nodeId -> stats
    std::vector<Ptr<ns3::psc::McpttPusherOrchestratorInterface>> orchestrators;
    for (const auto& p : appPairs)
    {
        ApplicationContainer mcpttApps;
        const uint32_t srcId = p.first;
        const uint32_t tgtId = p.second;

        // Resolve nodes by ID (returns nullptr if not found)
        Ptr<Node> srcNode = FindNodeById(srcId, nodes);
        Ptr<Node> tgtNode = FindNodeById(tgtId, nodes);
        if (srcNode == nullptr || tgtNode == nullptr)
        {
            std::cerr << "Skip app " << appPairId << " — node not found (srcId=" << srcId
                      << ", tgtId=" << tgtId << ")\n";
            continue;
        }

        // Create the AppInfo inside the list so its storage is stable for trace callbacks.
        appInfoList.emplace_back();
        AppInfo& appInfo = appInfoList.back();

        appInfo.appId = appPairId;
        appInfo.srcNodeId = srcId;
        appInfo.tgtNodeId = tgtId;

        // Get IPs from nodes (iface index 1 assumed)
        appInfo.srcUeIpAddress = srcNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        appInfo.tgtUeIpAddress = tgtNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();

        if (appPairId <= mcpttPairs)
        {
            ApplicationContainer srcApp = mcpttHelper.Install(srcNode);
            mcpttHelper.SetPttApp("ns3::psc::McpttPttApp", "PushOnStart", BooleanValue(false));
            srcApp.Start(startTrafficTime);
            srcApp.Stop(simTime - Seconds(1));
            mcpttApps.Add(srcApp);

            mcpttHelper.SetPttApp("ns3::psc::McpttPttApp", "PushOnStart", BooleanValue(false));
            ApplicationContainer tgtApp = mcpttHelper.Install(tgtNode);
            tgtApp.Start(startTrafficTime);
            tgtApp.Stop(simTime - Seconds(1));
            mcpttApps.Add(tgtApp);

            Ptr<ns3::psc::McpttPusherOrchestratorInterface> orchestrator = nullptr;
            Ptr<ns3::psc::McpttPusherOrchestratorSpurtCdf> spurtOrchestrator =
                CreateObject<ns3::psc::McpttPusherOrchestratorSpurtCdf>();
            spurtOrchestrator->SetAttribute("ActivityFactor", DoubleValue(1.0));
            orchestrator = spurtOrchestrator;

            Ptr<ns3::psc::McpttPusherOrchestratorSessionCdf> sessionOrchestrator =
                CreateObject<ns3::psc::McpttPusherOrchestratorSessionCdf>();
            sessionOrchestrator->SetAttribute("ActivityFactor", DoubleValue(1.0));
            sessionOrchestrator->SetAttribute("Orchestrator", PointerValue(orchestrator));
            orchestrator = sessionOrchestrator;

            orchestrator->StartAt(startTrafficTime +
                                  Seconds((appPairId - 1) * (simTime.GetSeconds() / nAppPairs)));
            orchestrator->StopAt(simTime - Seconds(2));

            if (chatterbox)
            {
                mcpttHelper.AddPushersToOrchestrator(orchestrator, srcApp);
            }
            else
            {
                mcpttHelper.AddPushersToOrchestrator(orchestrator, mcpttApps);
            }
            orchestrators.push_back(orchestrator);

            ObjectFactory callFac;
            ObjectFactory floorFac;

            callFac.SetTypeId("ns3::psc::McpttCallMachinePrivate");
            floorFac.SetTypeId("ns3::psc::McpttOffNetworkFloorParticipant");

            Ptr<ns3::psc::McpttPttApp> srcPttApp =
                DynamicCast<ns3::psc::McpttPttApp>(srcApp.Get(0));
            Ptr<ns3::psc::McpttPttApp> tgtPttApp =
                DynamicCast<ns3::psc::McpttPttApp>(tgtApp.Get(0));

            srcPttApp->SetLocalAddress(appInfo.srcUeIpAddress);
            tgtPttApp->SetLocalAddress(appInfo.tgtUeIpAddress);

            callFac.Set("TargetId", UintegerValue(tgtPttApp->GetUserId()));

            Ptr<ns3::psc::McpttCall> srcCall =
                srcPttApp->CreateCall(callFac,
                                      floorFac,
                                      ns3::psc::McpttCall::NetworkCallType::OFF_NETWORK);
            srcCall->SetAttribute("PeerAddress", AddressValue(appInfo.tgtUeIpAddress));
            srcPttApp->SelectCall(srcCall->GetCallId());

            callFac.Set("TargetId", UintegerValue(srcPttApp->GetUserId()));

            Ptr<ns3::psc::McpttCall> tgtCall =
                tgtPttApp->CreateCall(callFac,
                                      floorFac,
                                      ns3::psc::McpttCall::NetworkCallType::OFF_NETWORK);
            tgtCall->SetAttribute("PeerAddress", AddressValue(appInfo.srcUeIpAddress));
            tgtPttApp->SelectCall(tgtCall->GetCallId());
        }
        else
        {
            uint16_t port = 50000;
            PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory",
                                              InetSocketAddress(Ipv4Address::GetAny(), port));
            packetSinkHelper.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
            ApplicationContainer pktSinks = packetSinkHelper.Install(tgtNode);
            pktSinks.Start(startTrafficTime);
            pktSinks.Stop(simTime);

            OnOffHelper onOffHelper("ns3::UdpSocketFactory",
                                    InetSocketAddress(appInfo.tgtUeIpAddress, port));
            onOffHelper.SetAttribute("OnTime",
                                     StringValue("ns3::ConstantRandomVariable[Constant=1]"));
            onOffHelper.SetAttribute("OffTime",
                                     StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            onOffHelper.SetAttribute("PacketSize", UintegerValue(bgPktSize));
            onOffHelper.SetAttribute("DataRate", DataRateValue(bgDataRate));
            onOffHelper.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));

            ApplicationContainer srcApps = onOffHelper.Install(srcNode);
            srcApps.Start(startTrafficTime +
                          Seconds((appPairId - 1) * (simTime.GetSeconds() / nAppPairs)));
            srcApps.Stop(simTime - Seconds(1.0));
        }

        Ptr<NrUeNetDevice> srcUeNetDev = manetNetDevs.Get(srcId - 1)->GetObject<NrUeNetDevice>();
        Ptr<NrUeNetDevice> tgtUeNetDev = manetNetDevs.Get(tgtId - 1)->GetObject<NrUeNetDevice>();
        Ptr<NrSlUeProse> srcUeProse = srcUeNetDev->GetObject<NrSlUeProse>();
        Ptr<NrSlUeProse> tgtUeProse = tgtUeNetDev->GetObject<NrSlUeProse>();
        Ptr<NrSlUeRrc> srcUeRrc = srcUeNetDev->GetRrc()->GetObject<NrSlUeRrc>();
        Ptr<NrSlUeRrc> tgtUeRrc = tgtUeNetDev->GetRrc()->GetObject<NrSlUeRrc>();

        srcUeProse->SetImsi(srcUeRrc->GetImsi());
        tgtUeProse->SetImsi(tgtUeRrc->GetImsi());

        uint32_t srcUeL2Id = srcUeRrc->GetSourceL2Id();
        uint32_t tgtUeL2Id = tgtUeRrc->GetSourceL2Id();

        srcUeProse->SetL2Id(srcUeL2Id);
        tgtUeProse->SetL2Id(tgtUeL2Id);

        // Before the other bearer activation otherwise it doesn't work
        SidelinkInfo offNetUeSlInfo;
        offNetUeSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
        offNetUeSlInfo.m_dstL2Id = tgtUeL2Id;
        offNetUeSlInfo.m_dynamic = true;
        offNetUeSlInfo.m_harqEnabled = harq;
        offNetUeSlInfo.m_priority = 0;
        offNetUeSlInfo.m_rri = MilliSeconds(0);

        Ptr<NrSlTft> tft;
        tft = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                              appInfo.tgtUeIpAddress,
                              offNetUeSlInfo);
        nrSlHelper->ActivateNrSlBearer(startTrafficTime - Seconds(1),
                                       manetNetDevs.Get(srcId - 1),
                                       tft);

        SidelinkInfo offNetUeSlInfoMedia;
        Ptr<NrSlTft> tftMedia;
        if (appPairId <= mcpttPairs)
        {
            offNetUeSlInfoMedia.m_castType = SidelinkInfo::CastType::Unicast;
            offNetUeSlInfoMedia.m_dstL2Id = tgtUeL2Id;
            offNetUeSlInfoMedia.m_dynamic = false;
            offNetUeSlInfoMedia.m_harqEnabled = harq;
            offNetUeSlInfoMedia.m_priority = 0;
            offNetUeSlInfoMedia.m_rri = MilliSeconds(20);
            tftMedia = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                                       appInfo.tgtUeIpAddress,
                                       mediaPort,
                                       offNetUeSlInfoMedia);
            nrSlHelper->ActivateNrSlBearer(startTrafficTime - Seconds(1),
                                           manetNetDevs.Get(srcId - 1),
                                           tftMedia);
        }

        Simulator::Schedule(startTrafficTime - Seconds(1),
                            &NrSlUeProse::AddDirectLinkConnection,
                            srcUeProse,
                            srcUeL2Id,
                            appInfo.srcUeIpAddress,
                            tgtUeL2Id,
                            true,
                            0,
                            offNetUeSlInfo);

        // Before the other bearer activation otherwise it doesn't work
        offNetUeSlInfo.m_dstL2Id = srcUeL2Id;
        tft = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                              appInfo.srcUeIpAddress,
                              offNetUeSlInfo);
        nrSlHelper->ActivateNrSlBearer(startTrafficTime - Seconds(1),
                                       manetNetDevs.Get(tgtId - 1),
                                       tft);

        if (appPairId <= mcpttPairs)
        {
            offNetUeSlInfoMedia.m_dstL2Id = srcUeL2Id;
            tftMedia = Create<NrSlTft>(NrSlTft::BearerType::BIDIRECTIONAL,
                                       appInfo.srcUeIpAddress,
                                       mediaPort,
                                       offNetUeSlInfoMedia);
            nrSlHelper->ActivateNrSlBearer(startTrafficTime - Seconds(1),
                                           manetNetDevs.Get(tgtId - 1),
                                           tftMedia);
        }

        Simulator::Schedule(startTrafficTime - Seconds(1),
                            &NrSlUeProse::AddDirectLinkConnection,
                            tgtUeProse,
                            tgtUeL2Id,
                            appInfo.tgtUeIpAddress,
                            srcUeL2Id,
                            true,
                            0,
                            offNetUeSlInfo);

        // Connect SchedulingReport trace for call participant nodes
        auto connectSchedTrace = [&](uint32_t nodeId, Ptr<Node> node, uint32_t ueL2Id) {
            if (schedulingStats.find(nodeId) == schedulingStats.end())
            {
                auto* stats = new NodeSchedulingStats();
                stats->m_nodeId = nodeId;
                stats->m_l2Id = ueL2Id;
                schedulingStats[nodeId] = stats;
                Ptr<NrSlUeMacSchedulerDefault> sched = nrSlTraceHelper.GetNrSlUeMacScheduler(node)
                                                           ->GetObject<NrSlUeMacSchedulerDefault>();
                sched->TraceConnectWithoutContext("SchedulingReport",
                                                  MakeBoundCallback(&TraceSchedulingReport, stats));
            }
        };
        connectSchedTrace(srcId, srcNode, srcUeL2Id);
        connectSchedTrace(tgtId, tgtNode, tgtUeL2Id);

        std::ostringstream path;
        if (appPairId <= mcpttPairs)
        {
            path << "/NodeList/" << appInfo.tgtNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::psc::McpttPttApp/RxTrace";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&ReceivePacketMcpttApp, &appInfo.pktTrace));
            path.str("");

            path << "/NodeList/" << appInfo.tgtNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::psc::McpttPttApp/TxTrace";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&TransmitPacketMcpttApp, &appInfo.pktTrace));
            path.str("");

            path << "/NodeList/" << appInfo.srcNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::psc::McpttPttApp/RxTrace";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&ReceivePacketMcpttApp, &appInfo.pktTrace));
            path.str("");

            path << "/NodeList/" << appInfo.srcNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::psc::McpttPttApp/TxTrace";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&TransmitPacketMcpttApp, &appInfo.pktTrace));
            path.str("");
        }
        else
        {
            path << "/NodeList/" << appInfo.srcNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::OnOffApplication/TxWithSeqTsSize";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&TransmitPacketSeqTs, &appInfo.pktTrace));
            path.str("");

            path << "/NodeList/" << appInfo.tgtNodeId << "/ApplicationList/" << "0"
                 << "/$ns3::PacketSink/RxWithSeqTsSize";
            Config::ConnectWithoutContext(
                path.str(),
                MakeBoundCallback(&ReceivePacketSeqTs, &appInfo.pktTrace));
            path.str("");
        }

        // Next pair of apps
        appPairId++;
    }

    Ptr<ns3::psc::McpttTraceHelper> traceHelper = CreateObject<ns3::psc::McpttTraceHelper>();
    traceHelper->EnableMsgTraces();
    traceHelper->EnableStateMachineTraces();
    traceHelper->EnableMouthToEarLatencyTrace("mcptt-m2e-latency.dat");
    traceHelper->EnableAccessTimeTrace("mcptt-access-time.dat");

    // ProSe unicast link establishment and release tracing
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> rlfTraceStream = ascii.CreateFileStream("rlf-trace.csv");
    *rlfTraceStream->GetStream() << "#time(s),srcL2Id,dstL2Id,event" << std::endl;
    Ptr<OutputStreamWrapper> directLinkPacketTrace =
        ascii.CreateFileStream("direct-links-trace.csv");
    *directLinkPacketTrace->GetStream()
        << "#time(s),selfL2Id,selfIpv4Addr,peerL2Id,peerIpv4Add,status" << std::endl;
    for (uint32_t i = 0; i < manetNetDevs.GetN(); i++)
    {
        Ptr<NrUeNetDevice> device = manetNetDevs.Get(i)->GetObject<NrUeNetDevice>();
        Ptr<NrSlUeProse> prose = device->GetObject<NrSlUeProse>();
        prose->TraceConnect("DirectLinkEstablished",
                            std::to_string(device->GetNode()->GetId()),
                            MakeBoundCallback(&NotifyLinkEstablished, directLinkPacketTrace));
        prose->TraceConnect("DirectLinkReleasing",
                            std::to_string(device->GetNode()->GetId()),
                            MakeBoundCallback(&NotifyLinkReleasing, directLinkPacketTrace));
    }

    // L1 and L3 SD-RSRP measurement tracing
    Ptr<OutputStreamWrapper> l1SdRsrpTrace = ascii.CreateFileStream("l1-sd-rsrp-trace.csv");
    *l1SdRsrpTrace->GetStream() << "time(s),selfL2Id,peerL2Id,SD-RSRP(dBm)" << std::endl;
    Ptr<OutputStreamWrapper> l3SdRsrpTrace = ascii.CreateFileStream("l3-sd-rsrp-trace.csv");
    *l3SdRsrpTrace->GetStream() << "time(s),selfL2Id,peerL2Id,SD-RSRP(dBm),thCond" << std::endl;
    for (uint32_t i = 0; i < manetNetDevs.GetN(); ++i)
    {
        auto nrSlPhy =
            DynamicCast<NrSlUePhy>(manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetPhy(0));
        auto nrSlUeRrc =
            DynamicCast<NrSlUeRrc>(manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetRrc());
        uint32_t srcl2Id = nrSlUeRrc->GetSourceL2Id();

        nrSlPhy->TraceConnect("SdRsrpMeasurement",
                              std::to_string(srcl2Id),
                              MakeBoundCallback(&L1SdRsrpMeasurementTrace, l1SdRsrpTrace));

        auto prose = manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();

        prose->TraceConnectWithoutContext(
            "L3SdRsrpReport",
            MakeBoundCallback(&L3SdRsrpMeasurementTrace, l3SdRsrpTrace));
    }

    // NR SL MAC and PHY tracing if enabled
    SQLiteOutput db("mac-phy-trace.db");
    UeMacPscchTxOutputStats pscchStats;
    UeMacPsschTxOutputStats psschStats;
    UePhyPscchRxOutputStats pscchPhyStats;
    UePhyPsschRxOutputStats psschPhyStats;

    if (logMacPhyInDb)
    {
        pscchStats.SetDb(&db, "pscchTxUeMac");
        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                      "ComponentCarrierMapUe/*/NrUeMac/SlPscchScheduling",
                                      MakeBoundCallback(&NotifySlPscchScheduling, &pscchStats));

        psschStats.SetDb(&db, "psschTxUeMac");
        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                      "ComponentCarrierMapUe/*/NrUeMac/SlPsschScheduling",
                                      MakeBoundCallback(&NotifySlPsschScheduling, &psschStats));

        pscchPhyStats.SetDb(&db, "pscchRxUePhy");
        Config::ConnectWithoutContext(
            "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
            "ComponentCarrierMapUe/*/$ns3::BandwidthPartUe/NrUePhy/$ns3::NrUePhy/"
            "NrSpectrumPhy/$ns3::NrSpectrumPhy/RxPscchTraceUe",
            MakeBoundCallback(&NotifySlPscchRx, &pscchPhyStats));

        psschPhyStats.SetDb(&db, "psschRxUePhy");
        Config::ConnectWithoutContext(
            "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
            "ComponentCarrierMapUe/*/$ns3::BandwidthPartUe/NrUePhy/$ns3::NrUePhy/"
            "NrSpectrumPhy/$ns3::NrSpectrumPhy/RxPsschTraceUe",
            MakeBoundCallback(&NotifySlPsschRx, &psschPhyStats));
    }

    // PSCCH and PSSCH transmission and reception outcome tracing
    Ptr<OutputStreamWrapper> slChannelEventsTrace =
        ascii.CreateFileStream("sl-channel-events-trace.csv");
    *slChannelEventsTrace->GetStream() << "#time(s),selfL2Id,eventId,eventName" << std::endl;

    // Per-TB PSSCH decode trace: one row per decode attempt on any node.
    // Exposes the error-model inputs (mcs, sinrDb, tbSize, nRbs, rv) and the
    // returned TBLER, so the runtime LLS table can be checked directly.
    Ptr<OutputStreamWrapper> psschPerTbTrace = ascii.CreateFileStream("pssch-per-tb-trace.csv");
    *psschPerTbTrace->GetStream()
        << "#time(s),srcL2Id,dstL2Id,mcs,rv,tbSize,nRbs,sinrDb,tbler,tblerSci2,corrupt,sci2Corrupt,"
           "ndi"
        << std::endl;

    // Per-slot PSSCH scheduling trace (sender-side): one row per scheduled
    // PSSCH transmission, with the announced numTx of the TB transmitted in
    // this slot.  Downstream consumers filter to ndi=1 to recover per-TB
    // first-transmission rows and compute the announced reservation footprint
    // (subch_per_tx * announcedNumTx).
    Ptr<OutputStreamWrapper> psschSchedTrace = ascii.CreateFileStream("pssch-sched-trace.csv");
    *psschSchedTrace->GetStream()
        << "#time(s),srcL2Id,dstL2Id,harqId,ndi,rv,subChannelSize,rbLength,announcedNumTx"
        << std::endl;
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUeMac/SlPsschScheduling",
                                  MakeBoundCallback(&TracePsschSched, psschSchedTrace));

    std::map<uint32_t, NodePhyStats*> nodePhyStats;
    for (uint32_t i = 0; i < manetNetDevs.GetN(); ++i)
    {
        uint32_t nodeId = manetNetDevs.Get(i)->GetNode()->GetId();
        NodePhyStats* stats = new NodePhyStats(); // Create a new NodePhyStats object
        stats->m_nodeId = nodeId;

        Ptr<NrSlUeRrc> rrc =
            DynamicCast<NrSlUeRrc>(manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetRrc());
        stats->m_l2Id = rrc->GetSourceL2Id();
        nodePhyStats[nodeId] = stats;

        std::ostringstream ossSlEvent;
        ossSlEvent << "/NodeList/" << nodeId
                   << "/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                      "$ns3::BandwidthPartUe/"
                      "NrUePhy/$ns3::NrUePhy/NrSpectrumPhy/$ns3::NrSpectrumPhy/SlChannelsEvent";
        Config::ConnectWithoutContext(
            ossSlEvent.str(),
            MakeBoundCallback(&TraceSlChannelsEvent, stats, slChannelEventsTrace));

        std::ostringstream ossPssch;
        ossPssch << "/NodeList/" << nodeId
                 << "/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/"
                    "$ns3::BandwidthPartUe/"
                    "NrUePhy/$ns3::NrUePhy/NrSpectrumPhy/$ns3::NrSpectrumPhy/RxPsschTraceUe";
        Config::ConnectWithoutContext(ossPssch.str(),
                                      MakeBoundCallback(&TracePsschPerTb, psschPerTbTrace));
    }

    // Trace node positions
    // Currently saving only the initial node positions.
    // A periodic call to this function will give us the evolution of the topology in time when we
    // introduce mobility
    std::ofstream* fileNodePositions =
        new std::ofstream("node-position-trace.csv", std::ios_base::out);
    if (!fileNodePositions->is_open())
    {
        std::cerr << "Error: Could not open nodes-position-trace.csv\n";
        return 1;
    }
    *fileNodePositions << "#Time(s),nodeId,xPos(m),yPos(m),zPos(m)\n";
    SaveNodePositions(nodes, fileNodePositions);

    /******************* Set random variable stream numbers **************************/
    streamsUsed = nrSlHelper->AssignStreams(manetNetDevs, streamBase);
    streamBase = 2000;
    streamsUsed += nrSlProseHelper->AssignStreams(nodes, streamBase);
    streamBase = 3000;
    streamsUsed += ApplicationHelper::AssignStreamsToAllApps(nodes, streamBase);
    NS_LOG_DEBUG("Random variable streams used: " << streamsUsed);
    /******************* End set random variable stream numbers **********************/

    if (writeSchedulingTraces)
    {
        nrSlTraceHelper.TraceUePositions(nodes,
                                         Seconds(0),
                                         "nr-prose-multihop-advanced-ue-positions.dat");
        for (uint32_t i = 0; i < nodes.GetN(); i++)
        {
            // Node IDs are one greater than the index value of the container
            std::string sensingString =
                "nr-prose-multihop-advanced-sensing-" + std::to_string(i + 1) + ".dat";
            std::string schedString =
                "nr-prose-multihop-advanced-scheduling-" + std::to_string(i + 1) + ".dat";
            nrSlTraceHelper.TraceSensingAlgorithm(nodes.Get(i), sensingString);
            nrSlTraceHelper.TraceSchedulingAlgorithm(nodes.Get(i), schedString);
        }
    }

    Ptr<OutputStreamWrapper> mcsChangeTrace = ascii.CreateFileStream("mcs-change-trace.csv");
    *mcsChangeTrace->GetStream() << "#time(s),nodeId,dstL2Id,newMcs" << std::endl;

    Ptr<OutputStreamWrapper> trialResultsTrace;
    if (mcsControllerType != "static")
    {
        trialResultsTrace =
            Create<OutputStreamWrapper>(mcsControllerType + "-trial-results.csv", std::ios::out);
        *trialResultsTrace->GetStream() << "#time(s),nodeId,dstL2Id,mcs,success" << std::endl;
    }

    Ptr<OutputStreamWrapper> sinrEstimateTrace;
    if (mcsControllerType == "olla")
    {
        sinrEstimateTrace =
            Create<OutputStreamWrapper>("olla-sinr-estimate-trace.csv", std::ios::out);
        *sinrEstimateTrace->GetStream()
            << "#time(s),nodeId,dstL2Id,unfilteredSinrDb,filteredSinrDb,sinrOffsetDb,"
               "sinrEstimateDb"
            << std::endl;
    }

    Ptr<OutputStreamWrapper> harqFeedbackTrace =
        Create<OutputStreamWrapper>("harq-feedback-trace.csv", std::ios::out);
    *harqFeedbackTrace->GetStream()
        << "#time(s),nodeId,harqProcId,dstL2Id,mcs,txIndex,selectedNumTx,isFinal,status"
        << std::endl;

    Ptr<OutputStreamWrapper> harqTbCompletionTrace =
        Create<OutputStreamWrapper>("harq-tb-completion-trace.csv", std::ios::out);
    *harqTbCompletionTrace->GetStream()
        << "#time(s),nodeId,harqProcId,dstL2Id,mcs,selectedNumTx,txsUsed,status" << std::endl;

    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        Ptr<NrSlUeMacSchedulerDefault> sched = nrSlTraceHelper.GetNrSlUeMacScheduler(nodes.Get(i))
                                                   ->GetObject<NrSlUeMacSchedulerDefault>();
        sched->TraceConnect("McsChange",
                            std::to_string(nodes.Get(i)->GetId()),
                            MakeBoundCallback(&TraceMcsChange, mcsChangeTrace));

        PointerValue nrSlUeMacHarqPtr;
        sched->GetMac()->GetAttribute("NrSlUeMacHarq", nrSlUeMacHarqPtr);
        auto nrSlUeMacHarq = DynamicCast<NrSlUeMacHarq>(nrSlUeMacHarqPtr.GetObject());
        nrSlUeMacHarq->TraceConnect(
            "HarqFeedbackReceived",
            std::to_string(nodes.Get(i)->GetId()),
            MakeBoundCallback(&TraceHarqFeedbackReceived, harqFeedbackTrace));
        nrSlUeMacHarq->TraceConnect(
            "HarqTbCompletion",
            std::to_string(nodes.Get(i)->GetId()),
            MakeBoundCallback(&TraceHarqTbCompletion, harqTbCompletionTrace));

        if (mcsControllerType == "static")
        {
            continue;
        }

        PointerValue mcsControllerPtrVal;
        sched->GetAttribute("McsController", mcsControllerPtrVal);
        if (mcsControllerType == "ts")
        {
            auto tsMcsController = mcsControllerPtrVal.Get<NrSlThompsonSamplingMcsController>();
            if (tsMcsController)
            {
                tsMcsController->TraceConnect("TrialResult",
                                              std::to_string(nodes.Get(i)->GetId()),
                                              MakeBoundCallback(&TrialResult, trialResultsTrace));
            }
        }
        else if (mcsControllerType == "olla")
        {
            auto ollaMcsController = mcsControllerPtrVal.Get<NrSlOllaMcsController>();
            if (ollaMcsController)
            {
                ollaMcsController->TraceConnect("TrialResult",
                                                std::to_string(nodes.Get(i)->GetId()),
                                                MakeBoundCallback(&TrialResult, trialResultsTrace));
                ollaMcsController->TraceConnect(
                    "SinrEstimate",
                    std::to_string(nodes.Get(i)->GetId()),
                    MakeBoundCallback(&SinrEstimate, sinrEstimateTrace));
            }
        }
        else if (mcsControllerType == "ideal")
        {
            auto idealMcsController = mcsControllerPtrVal.Get<NrSlIdealMcsController>();
            if (idealMcsController)
            {
                idealMcsController->TraceConnect(
                    "TrialResult",
                    std::to_string(nodes.Get(i)->GetId()),
                    MakeBoundCallback(&TrialResult, trialResultsTrace));
            }
        }
    }

    // Trace logical channel creation. Used for installing RLC traces.
    std::map<uint32_t, Ptr<NodeRlcStats>> nodesRlcStats;
    Ptr<OutputStreamWrapper> rlcDropsTrace =
        Create<OutputStreamWrapper>("rlc-tx-pdu-drop-trace.csv", std::ios::out);
    *rlcDropsTrace->GetStream() << "time(s),nodeId,dstL2Id,lcId,pktSize\n";
    for (uint32_t i = 0; i < manetNetDevs.GetN(); i++)
    {
        uint32_t nodeId = manetNetDevs.Get(i)->GetNode()->GetId();
        Ptr<NodeRlcStats> nodeRlcStats = CreateObject<NodeRlcStats>();
        nodeRlcStats->m_nodeId = nodeId;
        nodeRlcStats->m_netDevice = manetNetDevs.Get(i)->GetObject<NrUeNetDevice>();
        nodesRlcStats[nodeId] = nodeRlcStats;
        Ptr<NrSlUeMacSchedulerDefault> sched = nrSlTraceHelper.GetNrSlUeMacScheduler(nodes.Get(i))
                                                   ->GetObject<NrSlUeMacSchedulerDefault>();
        sched->TraceConnectWithoutContext(
            "AddLogicalChannel",
            MakeBoundCallback(&TraceLogicalChannelAddition, rlcDropsTrace, nodeRlcStats));
    }

    Simulator::Stop(simTime);

    Simulator::Run();

    // Process and save PHY statistics
    ProcessPhyStats(nodePhyStats);

    // Process and save RLC statistics
    ProcessRlcStats(nodesRlcStats);

    // Process and save scheduling resource utilization statistics
    {
        double slBitmapOnes = 0;
        for (const auto& b : slBitmap)
        {
            if (b.test(0))
            {
                slBitmapOnes++;
            }
        }
        double slBitmapFraction = slBitmapOnes / slBitmap.size();
        ProcessSchedulingStats(schedulingStats, simTime, numerologyBwpSl, slBitmapFraction);
    }

    // Process and save application-level packet delivery statistics
    ProcessAppStats(appInfoList);

    // Clean up scheduling stats
    for (auto& [nodeId, stats] : schedulingStats)
    {
        delete stats;
    }

    // Write on the database if enabled
    if (logMacPhyInDb)
    {
        pscchStats.EmptyCache();
        psschStats.EmptyCache();
        pscchPhyStats.EmptyCache();
        psschPhyStats.EmptyCache();
    }

    // Other cleanup
    fileNodePositions->close();

    Simulator::Destroy();
    return 0;
}

NodeContainer
CreateGrid(uint32_t gridSize, double distance, Ptr<PropagationLossModel> lossModel)
{
    NS_ABORT_MSG_UNLESS(gridSize == 2 || gridSize == 4 || gridSize == 9 || gridSize == 16,
                        "Grid size must be 2, 4, 9, or 16");

    auto matrixLossModel = DynamicCast<MatrixPropagationLossModel>(lossModel);
    // Create and destroy one unused node, so that node ID 0 is not used in
    // the simulation and node IDs align with dstL2Ids
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    NodeContainer n;

    if (gridSize == 2)
    {
        for (uint32_t j = 0; j < 2; ++j)
        {
            auto node = CreateObject<Node>();
            auto mm = CreateObject<ConstantPositionMobilityModel>();
            NS_LOG_INFO("Creating node " << node->GetId() << " at position " << j * distance
                                         << ",0,1.5");
            mm->SetPosition(Vector(j * distance, 0.0, 1.5));
            node->AggregateObject(mm);
            n.Add(node);
        }

        if (matrixLossModel)
        {
            // Enable the single link between the two nodes
            DisableLoss(matrixLossModel, n.Get(0), n.Get(1));
        }

        return n;
    }

    auto gridWidth = sqrt(gridSize);
    for (uint32_t i = 0; i < gridWidth; ++i)
    {
        for (uint32_t j = 0; j < gridWidth; ++j)
        {
            auto node = CreateObject<Node>();
            auto mm = CreateObject<ConstantPositionMobilityModel>();
            NS_LOG_INFO("Creating node " << node->GetId() << " at position " << j * distance << ","
                                         << i * distance << ",1.5");
            mm->SetPosition(Vector(j * distance, i * distance, 1.5));
            node->AggregateObject(mm);
            n.Add(node);
        }
    }
    if (matrixLossModel)
    {
        for (uint32_t i = 0; i < gridSize; ++i)
        {
            for (uint32_t j = 0; j < gridSize; ++j)
            {
                if (i != j)
                {
                    DisableLoss(matrixLossModel, n.Get(i), n.Get(j));
                }
            }
        }
    }
    return n;
}

NodeContainer
CreateRing(uint32_t nNodes, double distance, Ptr<PropagationLossModel> lossModel)
{
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    auto matrixLossModel = DynamicCast<MatrixPropagationLossModel>(lossModel);
    // Create and destroy one unused node, so that node ID 0 is not used in
    // the simulation and node IDs align with dstL2Ids
    NodeContainer n(nNodes);
    ;
    double radius = distance / 2.0;
    double step = 2 * std::numbers::pi / nNodes;
    for (uint32_t i = 0; i < (nNodes / 2); i++)
    {
        double theta = i * step;
        double x1 = radius * std::cos(theta);
        double y1 = radius * std::sin(theta);
        double x2 = radius * std::cos(theta + std::numbers::pi);
        double y2 = radius * std::sin(theta + std::numbers::pi);

        auto node1 = n.Get(i);
        auto mm1 = CreateObject<ConstantPositionMobilityModel>();
        NS_LOG_INFO("Placing node " << node1->GetId() << " at position " << x1 << "," << y1
                                    << ",1.5");
        mm1->SetPosition(Vector(x1, y1, 1.5));
        node1->AggregateObject(mm1);

        auto node2 = n.Get(nNodes - i - 1);
        auto mm2 = CreateObject<ConstantPositionMobilityModel>();
        NS_LOG_INFO("Placing node " << node2->GetId() << " at position " << x2 << "," << y2
                                    << ",1.5");
        mm2->SetPosition(Vector(x2, y2, 1.5));
        node2->AggregateObject(mm2);

        if (matrixLossModel)
        {
            DisableLoss(matrixLossModel, node1, node2);
        }
    }

    return n;
}

void
DisableLoss(Ptr<MatrixPropagationLossModel> lossModel, Ptr<Node> a, Ptr<Node> b)
{
    lossModel->SetLoss(a->GetObject<MobilityModel>(), b->GetObject<MobilityModel>(), 0);
}

Ptr<Node>
FindNodeById(uint32_t id, NodeContainer nodes)
{
    for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
        if (nodes.Get(i)->GetId() == id)
        {
            return nodes.Get(i);
        }
    }
    return nullptr;
}

void
L1SdRsrpMeasurementTrace(Ptr<OutputStreamWrapper> stream,
                         std::string selfL2Id,
                         uint16_t rnti,
                         uint32_t peerL2Id,
                         double l1sdRsrp)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << "," << selfL2Id << "," << peerL2Id
                         << "," << l1sdRsrp << "\n";
}

void
L3SdRsrpMeasurementTrace(Ptr<OutputStreamWrapper> stream,
                         uint32_t selfL2Id,
                         uint32_t peerL2Id,
                         double l3sdRrsrp,
                         bool thCond)
{
    *stream->GetStream() << std::boolalpha << Simulator::Now().GetSeconds() << "," << selfL2Id
                         << "," << peerL2Id << "," << l3sdRrsrp << "," << thCond << "\n";
}

void
NotifyLinkEstablished(Ptr<OutputStreamWrapper> stream,
                      std::string context,
                      uint32_t selfL2Id,
                      Ipv4Address selfIpv4Addr,
                      uint32_t peerL2Id,
                      Ipv4Address peerIpv4Addr)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << "," << selfL2Id << "," << selfIpv4Addr
                         << "," << peerL2Id << "," << peerIpv4Addr << ",Established\n";
    NS_LOG_INFO("Established " << selfIpv4Addr << " to " << peerIpv4Addr);
}

void
NotifyLinkReleasing(Ptr<OutputStreamWrapper> stream,
                    std::string context,
                    uint32_t selfL2Id,
                    Ipv4Address selfIpv4Addr,
                    uint32_t peerL2Id,
                    Ipv4Address peerIpv4Addr)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << "," << selfL2Id << "," << selfIpv4Addr
                         << "," << peerL2Id << "," << peerIpv4Addr << ",Released\n";
    NS_LOG_INFO("Releasing " << selfIpv4Addr << " to " << peerIpv4Addr);
}

void
NotifySlPscchScheduling(UeMacPscchTxOutputStats* pscchStats,
                        const SlPscchUeMacStatParameters pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

void
NotifySlPsschScheduling(UeMacPsschTxOutputStats* psschStats,
                        const SlPsschUeMacStatParameters psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

void
NotifySlPscchRx(UePhyPscchRxOutputStats* pscchStats,
                const SlRxCtrlPacketTraceParams pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

void
NotifySlPsschRx(UePhyPsschRxOutputStats* psschStats,
                const SlRxDataPacketTraceParams psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

void
ProcessPhyStats(const std::map<uint32_t, NodePhyStats*>& nodePhyStats)
{
    struct StatField
    {
        std::string name;
        std::function<uint64_t(const NodePhyStats&)> getter;
        uint64_t total = 0;
    };

    struct Bucket
    {
        std::string label;
        // Accessor: function that extracts the correct PhyStats sub-struct
        // from a given NodePhyStats object.
        std::function<const NodePhyStats::PhyStats&(const NodePhyStats&)> accessor;
    };

    // Buckets: one for each type of message we want to print/trace.
    const std::vector<Bucket> buckets = {
        {"app",
         [](const NodePhyStats& s) -> const NodePhyStats::PhyStats& { return s.m_appStats; }},
        {"pc5s",
         [](const NodePhyStats& s) -> const NodePhyStats::PhyStats& { return s.m_pc5sStats; }},
    };

    // Build StatField vector for a given bucket.  nRxDataCorr is the combined
    // PSSCH decode-failure count; nRxDataCorrSci2 and nRxDataCorrTb break it
    // out into SCI-2a-decode failures and TB-decode failures respectively.
    auto makeDataFields = [&](const Bucket& b) {
        return std::vector<StatField>{
            {"nTxData", [&, b](const NodePhyStats& s) { return b.accessor(s).nTxData; }},
            {"nRxData", [&, b](const NodePhyStats& s) { return b.accessor(s).nRxData; }},
            {"nRxDataCorr", [&, b](const NodePhyStats& s) { return b.accessor(s).nCorruptRxData; }},
            {"nRxDataCorrSci2",
             [&, b](const NodePhyStats& s) { return b.accessor(s).nCorruptRxDataSci2; }},
            {"nRxDataCorrTb",
             [&, b](const NodePhyStats& s) { return b.accessor(s).nCorruptRxDataTb; }},
            {"nRxDataHd", [&, b](const NodePhyStats& s) { return b.accessor(s).nHdRxData; }},
            {"nRxDataNotExp",
             [&, b](const NodePhyStats& s) { return b.accessor(s).nDataNotExpected; }},
            {"nRxDataAlrDec",
             [&, b](const NodePhyStats& s) { return b.accessor(s).nDataAlreadyDecoded; }},
        };
    };

    std::ofstream simTotals("phy-stats-sim-total.csv");
    if (!simTotals.is_open())
    {
        std::cerr << "Can't open phy-stats-sim-total.csv\n";
        return;
    }
    simTotals << "#RngSeed,RngRun,Type,nTxData,nRxData,nRxDataCorr,nRxDataCorrSci2,"
                 "nRxDataCorrTb,nRxDataHd,nRxDataNotExp,nRxDataAlrDec\n";

    std::ofstream simPercent("phy-stats-sim-percent.csv");
    if (!simPercent.is_open())
    {
        std::cerr << "Can't open phy-stats-sim-percent.csv\n";
        return;
    }
    simPercent << "#RngSeed,RngRun,Type,nTxData,nRxData,nRxDataCorr,nRxDataCorrSci2,"
                  "nRxDataCorrTb,nRxDataHd,nRxDataNotExp,nRxDataAlrDec\n";

    for (const auto& bucket : buckets)
    {
        auto statFields = makeDataFields(bucket);

        // Print on standard output
        std::cout << "\nPSSCH Statistics (" << bucket.label << "):\n";
        std::cout << std::left << std::setw(8) << "Node ID"
                  << "| " << std::setw(8) << "L2 ID"
                  << "| ";
        for (const auto& f : statFields)
            std::cout << std::setw(14) << f.name << "| ";
        std::cout << "\n" << std::string(115, '-') << "\n";

        for (const auto& [nodeId, statsPtr] : nodePhyStats)
        {
            const NodePhyStats& s = *statsPtr;

            // Accumulate totals and check if any field is non-zero
            bool hasData = false;
            for (auto& f : statFields)
            {
                uint64_t value = f.getter(s);
                f.total += value;
                if (value > 0)
                {
                    hasData = true;
                }
            }

            if (!hasData)
            {
                continue;
            }

            std::cout << std::left << std::setw(8) << s.m_nodeId << "| " << std::setw(8) << s.m_l2Id
                      << "| ";
            for (const auto& f : statFields)
                std::cout << std::setw(14) << f.getter(s) << "| ";
            std::cout << "\n";
        }

        std::cout << std::string(115, '-') << "\n";
        std::cout << std::left << std::setw(18) << "Total"
                  << "| ";
        for (const auto& f : statFields)
            std::cout << std::setw(14) << f.total << "| ";
        std::cout << "\n" << std::string(115, '-') << "\n";

        // Balance check
        auto getTotal = [&](const std::string& name) -> uint64_t {
            for (const auto& f : statFields)
                if (f.name == name)
                    return f.total;
            return 0;
        };
        const int64_t balanceData = getTotal("nTxData") - getTotal("nRxData") -
                                    getTotal("nRxDataCorr") - getTotal("nRxDataHd") -
                                    getTotal("nRxDataNotExp") - getTotal("nRxDataAlrDec");

        std::cout << "Balance (" << bucket.label << "): " << balanceData << "\n";

        NS_ASSERT_MSG(balanceData == 0,
                      "Non-zero PSSCH balance for "
                          << bucket.label << ": " << balanceData << " (Tx=" << getTotal("nTxData")
                          << ", Rx=" << getTotal("nRxData") << ", Corr=" << getTotal("nRxDataCorr")
                          << ", Hd=" << getTotal("nRxDataHd")
                          << ", NotExp=" << getTotal("nRxDataNotExp")
                          << ", AlrDec=" << getTotal("nRxDataAlrDec") << ")");

        // CSV per-node
        {
            std::string fname = "phy-stats-per-node-" + bucket.label + ".csv";
            std::ofstream file(fname);
            if (file.is_open())
            {
                file << "#RngSeed,RngRun,NodeID,L2ID";
                for (const auto& f : statFields)
                    file << "," << f.name;
                file << "\n";

                for (const auto& [nodeId, statsPtr] : nodePhyStats)
                {
                    const NodePhyStats& s = *statsPtr;
                    file << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                         << s.m_nodeId << "," << s.m_l2Id;
                    for (const auto& f : statFields)
                        file << "," << f.getter(s);
                    file << "\n";
                }
            }
        }

        // Append to totals csv
        simTotals << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                  << bucket.label;
        for (const auto& f : statFields)
            simTotals << "," << f.total;
        simTotals << "\n";

        // Append to percents csv
        const double tx = static_cast<double>(getTotal("nTxData"));
        simPercent << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                   << bucket.label;
        for (const auto& f : statFields)
        {
            const double pct = (tx > 0.0) ? (100.0 * static_cast<double>(f.total) / tx) : 0.0;
            simPercent << "," << std::fixed << std::setprecision(4) << pct;
        }
        simPercent << "\n";
    }
}

void
ProcessRlcStats(const std::map<uint32_t, Ptr<NodeRlcStats>>& nodesRlcStats)
{
    // Standard output column widths
    const uint32_t wNode = 8;
    const uint32_t wDst = 8;
    const uint32_t wLc = 6;
    const uint32_t wTx = 12;
    const uint32_t wDrop = 14;
    const uint32_t totalWidth = 2 + wNode + 3 + wDst + 3 + wLc + 3 + wTx + 3 + wDrop + 2;

    // Standard output headers
    std::cout << std::string(totalWidth, '-') << "\n";
    std::cout << "\nRLC PDU statistics (only showing LCs with at least one entry > 0):\n"
              << "| " << std::left << std::setw(wNode) << "NodeID"
              << " | " << std::left << std::setw(wDst) << "dstL2Id"
              << " | " << std::left << std::setw(wLc) << "LCID"
              << " | " << std::left << std::setw(wTx) << "nTxPdu"
              << " | " << std::left << std::setw(wDrop) << "nDroppedPdu"
              << " |\n";
    std::cout << std::string(totalWidth, '-') << "\n";

    // CSV per node
    const std::string outNamePerNode = "rlc-tx-pdu-drop-per-node.csv";
    std::ofstream csvPerNode(outNamePerNode, std::ios::out | std::ios::trunc);
    if (!csvPerNode.is_open())
    {
        NS_FATAL_ERROR("ERROR: Could not open " << outNamePerNode << " for writing");
        return;
    }
    csvPerNode << "#RngSeed,RngRun,NodeID,dstL2Id,LCID,nTxPdu,nDroppedPdu\n";

    uint64_t totalTx = 0;
    uint64_t totalDrop = 0;
    for (const auto& [nodeId, nodeStats] : nodesRlcStats)
    {
        if (!nodeStats)
        {
            continue;
        }
        for (const auto& [key, rlcStats] : nodeStats->m_rlcStats)
        {
            const uint32_t dstL2Id = key.first;
            const uint8_t lcId = key.second;

            if (!rlcStats)
            {
                continue;
            }
            if (rlcStats->m_nTxPdu == 0 && rlcStats->m_nDroppedPdu == 0)
            {
                continue;
            }

            std::cout << "| " << std::left << std::setw(wNode) << nodeId << " | " << std::left
                      << std::setw(wDst) << dstL2Id << " | " << std::left << std::setw(wLc) << +lcId
                      << " | " << std::left << std::setw(wTx) << rlcStats->m_nTxPdu << " | "
                      << std::left << std::setw(wDrop) << rlcStats->m_nDroppedPdu << " |\n";

            csvPerNode << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                       << nodeId << "," << dstL2Id << "," << +lcId << "," << rlcStats->m_nTxPdu
                       << "," << rlcStats->m_nDroppedPdu << "\n";

            totalTx += rlcStats->m_nTxPdu;
            totalDrop += rlcStats->m_nDroppedPdu;
        }
    }

    std::cout << std::string(totalWidth, '-') << "\n";
    std::cout << "| " << std::left << std::setw(wNode) << "Total"
              << "   " << std::left << std::setw(wDst) << ""
              << " | " << std::left << std::setw(wLc) << ""
              << " | " << std::left << std::setw(wTx) << totalTx << " | " << std::left
              << std::setw(wDrop) << totalDrop << " |\n";
    std::cout << std::string(totalWidth, '-') << "\n";

    csvPerNode.close();
    std::cout << std::endl;

    // Simulation totals CSV
    const std::string outNameSim = "rlc-tx-pdu-drop-total.csv";
    std::ofstream csvSim(outNameSim, std::ios::out | std::ios::trunc);
    if (!csvSim.is_open())
    {
        NS_FATAL_ERROR("ERROR: Could not open " << outNameSim << " for writing");
        return;
    }

    csvSim << "#RngSeed,RngRun,nTxPdu,nDroppedPdu\n";
    csvSim << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << "," << totalTx << ","
           << totalDrop << "\n";
    csvSim.close();
}

void
ProcessSchedulingStats(const std::map<uint32_t, NodeSchedulingStats*>& schedulingStats,
                       Time simTime,
                       uint16_t numerology,
                       double slBitmapFraction)
{
    // Calculate total available subchannel-slots in the pool
    double slotDurationMs = 1.0 / (1 << numerology); // 1 ms for mu=0, 0.5 ms for mu=1, etc.
    uint64_t totalSlots =
        static_cast<uint64_t>(simTime.GetMilliSeconds() / slotDurationMs * slBitmapFraction);

    // Get total subchannels from first node that has data
    uint16_t poolSubchannels = 0;
    for (const auto& [nodeId, stats] : schedulingStats)
    {
        if (stats->m_totalSubchannels > 0)
        {
            poolSubchannels = stats->m_totalSubchannels;
            break;
        }
    }
    uint64_t totalAvailable = totalSlots * poolSubchannels;

    const uint32_t wNode = 4;
    const uint32_t wL2 = 4;
    const uint32_t wGrants = 9;
    const uint32_t wSubch = 14;
    const uint32_t wNewData = 14;
    const uint32_t wRetx = 8;
    const uint32_t wUtil = 13;
    const uint32_t totalWidth =
        2 + wNode + 3 + wL2 + 3 + wGrants + 3 + wSubch + 3 + wNewData + 3 + wRetx + 3 + wUtil + 2;

    std::cout << "\nScheduling Resource Statistics:\n";
    std::cout << "  Pool: " << poolSubchannels << " subchannels, " << totalSlots << " SL slots, "
              << totalAvailable << " available subchannel-slots\n";
    std::cout << "| " << std::left << std::setw(wNode) << "Node"
              << " | " << std::setw(wL2) << "L2"
              << " | " << std::setw(wGrants) << "grants"
              << " | " << std::setw(wSubch) << "subchannels"
              << " | " << std::setw(wNewData) << "newDataSlots"
              << " | " << std::setw(wRetx) << "retx"
              << " | " << std::setw(wUtil) << "utilization"
              << " |\n";
    std::cout << std::string(totalWidth, '-') << "\n";

    uint64_t totalGrants = 0;
    uint64_t totalSubch = 0;
    uint64_t totalNewData = 0;
    uint64_t totalRetx = 0;

    for (const auto& [nodeId, stats] : schedulingStats)
    {
        double nodeUtil = (totalAvailable > 0)
                              ? static_cast<double>(stats->m_nSubchannels) / totalAvailable
                              : 0.0;
        std::cout << "| " << std::left << std::setw(wNode) << stats->m_nodeId << " | "
                  << std::setw(wL2) << stats->m_l2Id << " | " << std::setw(wGrants)
                  << stats->m_nGrants << " | " << std::setw(wSubch) << stats->m_nSubchannels
                  << " | " << std::setw(wNewData) << stats->m_nSlotsWithNewData << " | "
                  << std::setw(wRetx) << stats->m_nSlotsWithRetx << " | " << std::fixed
                  << std::setprecision(4) << std::setw(wUtil) << nodeUtil << " |\n";

        totalGrants += stats->m_nGrants;
        totalSubch += stats->m_nSubchannels;
        totalNewData += stats->m_nSlotsWithNewData;
        totalRetx += stats->m_nSlotsWithRetx;
    }

    double utilization =
        (totalAvailable > 0) ? static_cast<double>(totalSubch) / totalAvailable : 0.0;

    std::cout << std::string(totalWidth, '-') << "\n";
    std::cout << "| " << std::left << std::setw(wNode) << "Tot"
              << " | " << std::setw(wL2) << ""
              << " | " << std::setw(wGrants) << totalGrants << " | " << std::setw(wSubch)
              << totalSubch << " | " << std::setw(wNewData) << totalNewData << " | "
              << std::setw(wRetx) << totalRetx << " | " << std::fixed << std::setprecision(4)
              << std::setw(wUtil) << utilization << " |\n";
    std::cout << std::string(totalWidth, '-') << "\n";

    // CSV per-node
    {
        std::ofstream csv("sched-stats-per-node.csv");
        if (csv.is_open())
        {
            csv << "#RngSeed,RngRun,NodeID,L2ID,grants,subchannels,newDataSlots,"
                   "retxSlots,utilization\n";
            for (const auto& [nodeId, stats] : schedulingStats)
            {
                double nodeUtil = (totalAvailable > 0)
                                      ? static_cast<double>(stats->m_nSubchannels) / totalAvailable
                                      : 0.0;
                csv << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                    << stats->m_nodeId << "," << stats->m_l2Id << "," << stats->m_nGrants << ","
                    << stats->m_nSubchannels << "," << stats->m_nSlotsWithNewData << ","
                    << stats->m_nSlotsWithRetx << "," << std::fixed << std::setprecision(4)
                    << nodeUtil << "\n";
            }
        }
    }

    // CSV totals
    {
        std::ofstream csv("sched-stats-total.csv");
        if (csv.is_open())
        {
            csv << "#RngSeed,RngRun,grants,subchannels,newDataSlots,retxSlots,"
                   "availableSubchannels,utilization\n";
            csv << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                << totalGrants << "," << totalSubch << "," << totalNewData << "," << totalRetx
                << "," << totalAvailable << "," << std::fixed << std::setprecision(4) << utilization
                << "\n";
        }
    }
}

void
ProcessAppStats(const std::list<AppInfo>& appInfoList)
{
    std::ofstream pktCsv("voip-packet-trace.csv");
    std::ofstream statsCsv("voip-stats-sim.csv");

    if (pktCsv.is_open())
    {
        pktCsv << "#AppId,TxTime(s),RxTime(s),Seq,Size,RxDelay(ms)\n";
    }
    if (statsCsv.is_open())
    {
        statsCsv << "#RngSeed,RngRun,nTxPkts,nRxPkts,LossRatio,AvgDelay,Mos\n";
    }

    std::cout << "\nApplication Packet Statistics:\n";

    for (const auto& app : appInfoList)
    {
        uint64_t nTx = 0;
        uint64_t nRx = 0;
        double totalDelayMs = 0.0;

        for (const auto& [seq, info] : app.pktTrace)
        {
            if (info.txTime > Seconds(0))
            {
                nTx++;
            }
            bool received = info.rxTime > Seconds(0);
            if (received)
            {
                nRx++;
                totalDelayMs += info.rxDelay.GetMilliSeconds();
            }

            // Persist every transmitted packet so consumers can recompute
            // loss / delay / MoS over a truncated time window.  Lost packets
            // have rxTime = 0 and RxDelay = NaN.
            if (pktCsv.is_open() && info.txTime > Seconds(0))
            {
                pktCsv << app.appId << "," << std::fixed << std::setprecision(6)
                       << info.txTime.GetSeconds() << "," << info.rxTime.GetSeconds() << "," << seq
                       << "," << info.size << ",";
                if (received)
                {
                    pktCsv << info.rxDelay.GetMilliSeconds() << "\n";
                }
                else
                {
                    pktCsv << "NaN\n";
                }
            }
        }

        double lossRatio = (nTx > 0) ? (1.0 - static_cast<double>(nRx) / nTx) : 0.0;
        double avgDelay = (nRx > 0) ? (totalDelayMs / nRx) : 0.0;

        // E-model R-factor approximation for VoIP MOS
        double r = 93.2 - (avgDelay * 0.024) - (lossRatio * 100.0 * 2.5);
        if (r < 0)
        {
            r = 0;
        }
        double mos = 1.0 + 0.035 * r + r * (r - 60.0) * (100.0 - r) * 7.0e-6;

        std::cout << "  App " << app.appId << " (Node " << app.srcNodeId << " <-> Node "
                  << app.tgtNodeId << "): Tx=" << nTx << " Rx=" << nRx << " Loss=" << std::fixed
                  << std::setprecision(4) << lossRatio << " AvgDelay=" << std::setprecision(2)
                  << avgDelay << " ms MOS=" << std::setprecision(2) << mos << "\n";

        if (statsCsv.is_open())
        {
            statsCsv << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << "," << nTx
                     << "," << nRx << "," << std::fixed << std::setprecision(4) << lossRatio << ","
                     << std::setprecision(6) << avgDelay << "," << std::setprecision(6) << mos
                     << "\n";
        }
    }
}

void
ReceivePacketMcpttApp(PacketTrace* pktTrace,
                      Ptr<const Application> app,
                      uint16_t callId,
                      Ptr<const Packet> pkt,
                      const TypeId& headerType)
{
    if (headerType == ns3::psc::McpttMediaMsg::GetTypeId())
    {
        ns3::psc::McpttMediaMsg mediaMsg;
        pkt->PeekHeader(mediaMsg);

        uint32_t seq = mediaMsg.GetHeader().GetSeqNum();
        Time now = Simulator::Now();
        Time tx = MicroSeconds(mediaMsg.GetHeader().GetTimestamp() * 10);

        PacketInfo& info = (*pktTrace)[seq];
        info.rxTime = now;
        info.rxDelay = (now - tx);
        info.size = pkt->GetSize();
    }
}

void
ReceivePacketSeqTs(PacketTrace* pktTrace,
                   Ptr<const Packet> packet,
                   const Address& from,
                   const Address& to,
                   const SeqTsSizeHeader& header)
{
    uint32_t seq = header.GetSeq();
    Time now = Simulator::Now();
    Time tx = header.GetTs();

    PacketInfo& info = (*pktTrace)[seq];
    info.rxTime = now;
    info.rxDelay = (now - tx);
    info.size = header.GetSize();
}

void
SaveNodePositions(const ns3::NodeContainer& nodes, std::ofstream* file)
{
    for (auto it = nodes.Begin(); it != nodes.End(); ++it)
    {
        Ptr<Node> node = *it;
        Ptr<MobilityModel> mob = (*it)->GetObject<MobilityModel>();
        const Vector p = mob->GetPosition();
        *file << Simulator::Now().GetSeconds() << "," << node->GetId() << "," << p.x << "," << p.y
              << "," << p.z << "\n";
    }
    file->flush();
}

void
TraceMcsChange(Ptr<OutputStreamWrapper> stream,
               std::string context,
               uint32_t dstL2Id,
               uint8_t oldMcs,
               uint8_t newMcs)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << "," << context << "," << dstL2Id << ","
                         << +newMcs << std::endl;
}

void
TraceSchedulingReport(
    NodeSchedulingStats* stats,
    const NrSlUeMacSchedulerDefault::SchedulingReport& report,
    const std::list<SlResourceInfo>& /*candidateResources*/,
    const NrSlUeMac::NrSlTransmissionParams& /*params*/,
    const std::vector<SlGrantResource>& /*publishedGrants*/,
    const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& /*existingGrants*/,
    const NrSlUeMacScheduler::GrantInfo& grant)
{
    stats->m_totalSubchannels = report.m_subchannels;
    stats->m_nGrants++;
    for (const auto& slot : grant.slotAllocations)
    {
        stats->m_nSubchannels += slot.slPsschSubChLength;
        if (slot.ndi == 1)
        {
            stats->m_nSlotsWithNewData++;
        }
        else
        {
            stats->m_nSlotsWithRetx++;
        }
    }
}

void
TraceSlChannelsEvent(NodePhyStats* stats,
                     Ptr<OutputStreamWrapper> slChannelEventsTrace,
                     const NrSlSpectrumPhy::SlEventType type,
                     Ptr<const PacketBurst> pktBurst)
{
    // Ignore transmissions we don't want to trace
    if (type == NrSlSpectrumPhy::SlEventType::PSCCH_TX)
    {
        Ptr<Packet> packet = pktBurst->GetPackets().front();
        NrSlMacPduTag tag;
        packet->PeekPacketTag(tag);

        // skip broadcast transmissions
        if (tag.GetDstL2Id() == 255)
        {
            return;
        }
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_OK ||
             type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_DECODE_FAILURE ||
             type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_HALF_DUPLEX)
    {
        Ptr<Packet> packet = pktBurst->GetPackets().front();
        NrSlMacPduTag tag;
        packet->PeekPacketTag(tag);

        // skip receptions not directed to this node
        if (tag.GetDstL2Id() != stats->m_l2Id)
        {
            return;
        }
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_TX)
    {
        Ptr<Packet> packet = pktBurst->GetPackets().back();
        NrSlSciF2aHeader sciF2a;
        packet->PeekHeader(sciF2a);

        // skip broadcast transmissions
        if (sciF2a.GetDstId() == 255)
        {
            return;
        }
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_OK ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_SCI2 ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_TB ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_HALF_DUPLEX ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_NOT_EXPECTED ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_ALREADY_DECODED)
    {
        Ptr<Packet> packet = pktBurst->GetPackets().back();
        NrSlSciF2aHeader sciF2a;
        packet->PeekHeader(sciF2a);

        // skip receptions not directed to this node
        if (sciF2a.GetDstId() != stats->m_l2Id)
        {
            return;
        }
    }
    else
    {
        NS_ABORT_MSG("Unknown SlEventType " << +static_cast<uint8_t>(type));
    }

    NrSlProseAppTag appTag;
    NrSlProsePc5SignallingTag pc5SignallingTag;

    NodePhyStats::PhyStats* bucket = nullptr;

    for (auto it = pktBurst->Begin(); it != pktBurst->End(); ++it)
    {
        Ptr<Packet> pkt = *it;

        if (!bucket && pkt->FindFirstMatchingByteTag(appTag))
        {
            bucket = &stats->m_appStats;
            break; // classification found
        }
        if (!bucket && pkt->FindFirstMatchingByteTag(pc5SignallingTag))
        {
            bucket = &stats->m_pc5sStats;
            break;
        }
    }

    if (!bucket)
    {
        return; // no relevant tag found, do not trace
    }

    // Update PhyStats bucket depending on the type of SL event
    switch (type)
    {
    case NrSlSpectrumPhy::SlEventType::PSCCH_TX:
        bucket->nTxCtrl++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSCCH_TX) << "," << "PSCCH_TX"
            << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_OK:
        bucket->nRxCtrl++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSCCH_RX_OK) << ","
            << "PSCCH_RX_OK" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_DECODE_FAILURE:
        bucket->nCorruptRxCtrl++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSCCH_RX_DECODE_FAILURE) << ","
            << "PSCCH_RX_DECODE_FAILURE" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_HALF_DUPLEX:
        bucket->nHdRxCtrl++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSCCH_RX_HALF_DUPLEX) << ","
            << "PSCCH_RX_HALF_DUPLEX" << std::endl;
        break;

    case NrSlSpectrumPhy::SlEventType::PSSCH_TX:
        bucket->nTxData++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_TX) << "," << "PSSCH_TX"
            << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_OK:
        bucket->nRxData++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_OK) << ","
            << "PSSCH_RX_OK" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_SCI2:
        bucket->nCorruptRxData++;
        bucket->nCorruptRxDataSci2++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_SCI2)
            << "," << "PSSCH_RX_DECODE_FAILURE_SCI2" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_TB:
        bucket->nCorruptRxData++;
        bucket->nCorruptRxDataTb++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_TB)
            << "," << "PSSCH_RX_DECODE_FAILURE_TB" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_HALF_DUPLEX:
        bucket->nHdRxData++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_HALF_DUPLEX) << ","
            << "PSSCH_RX_HALF_DUPLEX" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_NOT_EXPECTED:
        bucket->nDataNotExpected++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_NOT_EXPECTED) << ","
            << "PSSCH_RX_NOT_EXPECTED" << std::endl;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_ALREADY_DECODED:
        bucket->nDataAlreadyDecoded++;
        *slChannelEventsTrace->GetStream()
            << Simulator::Now().GetSeconds() << "," << stats->m_l2Id << ","
            << static_cast<uint32_t>(NrSlSpectrumPhy::SlEventType::PSSCH_RX_ALREADY_DECODED) << ","
            << "PSSCH_RX_ALREADY_DECODED" << std::endl;
        break;

    default:
        NS_ABORT_MSG("Unknown SlEventType " << +static_cast<uint8_t>(type));
    }
}

void
TracePsschPerTb(Ptr<OutputStreamWrapper> stream, const SlRxDataPacketTraceParams params)
{
    // m_sinr is linear; convert to dB for logging.  m_tbler is 0 when
    // SCI-2a failed (the PSSCH TBLER lookup is short-circuited in that
    // branch); m_tblerSci2 carries the SCI-2a BLER in that case.
    const double sinrDb = (params.m_sinr > 0.0) ? 10.0 * std::log10(params.m_sinr)
                                                : -std::numeric_limits<double>::infinity();
    *stream->GetStream() << std::fixed << std::setprecision(6) << Simulator::Now().GetSeconds()
                         << "," << params.m_srcL2Id << "," << params.m_dstL2Id << ","
                         << +params.m_mcs << "," << +params.m_rv << "," << params.m_tbSize << ","
                         << params.m_rbAssignedNum << "," << std::setprecision(4) << sinrDb << ","
                         << std::setprecision(6) << params.m_tbler << "," << params.m_tblerSci2
                         << "," << (params.m_corrupt ? 1 : 0) << ","
                         << (params.m_sci2Corrupted ? 1 : 0) << "," << +params.m_ndi << std::endl;
}

void
TracePsschSched(Ptr<OutputStreamWrapper> stream, const SlPsschUeMacStatParameters params)
{
    *stream->GetStream() << std::fixed << std::setprecision(6) << (params.timeMs / 1000.0) << ","
                         << params.srcL2Id << "," << params.dstL2Id << "," << +params.harqId << ","
                         << +params.ndi << "," << +params.rv << "," << params.subChannelSize << ","
                         << params.rbLength << ",";
    if (params.announcedNumTx.has_value())
    {
        *stream->GetStream() << +(*params.announcedNumTx);
    }
    *stream->GetStream() << std::endl;
}

void
TraceLogicalChannelAddition(Ptr<OutputStreamWrapper> rlcDropsTrace,
                            Ptr<NodeRlcStats> nodeRlcStats,
                            const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params)
{
    auto rrc = nodeRlcStats->m_netDevice->GetRrc()->GetObject<NrSlUeRrc>();
    NS_ASSERT_MSG(rrc, "Failed to get NrSlUeRrc from UE net device");

    auto bearers = rrc->GetAllSidelinkTxDataRadioBearers(params.dstL2Id);
    if (bearers.empty())
    {
        NS_LOG_WARN("No TX bearers found for dstL2Id=" << params.dstL2Id);
        return;
    }

    for (const auto& [lcId, bearer] : bearers)
    {
        if (lcId != params.lcId)
        {
            continue;
        }

        auto rlcUm = bearer->m_rlc->GetObject<NrSlRlcUm>();
        if (!rlcUm)
        {
            NS_LOG_WARN("Failed to get NrSlRlcUm for dstL2Id=" << params.dstL2Id
                                                               << " lcId=" << +lcId);
            return;
        }

        const auto key = std::make_pair(params.dstL2Id, params.lcId);

        // Avoid overwriting stats / reconnecting traces.
        // This needs to be revisited when we'd have dynamic scenarios where LCs are removed and
        // lcId values can be reused for new LCs
        if (nodeRlcStats->m_rlcStats.find(key) != nodeRlcStats->m_rlcStats.end())
        {
            return;
        }

        Ptr<RlcStats> rlcStats = CreateObject<RlcStats>();
        rlcStats->m_lcId = params.lcId;
        rlcStats->m_nodeId = nodeRlcStats->m_nodeId;
        rlcStats->m_dstL2Id = params.dstL2Id;

        nodeRlcStats->m_rlcStats[key] = rlcStats;
        rlcUm->TraceConnectWithoutContext("TxPDU", MakeBoundCallback(&TraceRlcTxPdu, rlcStats));
        rlcUm->TraceConnectWithoutContext(
            "TxDrop",
            MakeBoundCallback(&TraceRlcTxDrop, rlcDropsTrace, rlcStats));
        return;
    }
}

void
TraceRlcTxPdu(Ptr<RlcStats> stats, uint16_t /*rnti*/, uint8_t /*lcId*/, uint32_t /*pktSize*/)
{
    stats->m_nTxPdu++;
}

void
TraceRlcTxDrop(Ptr<OutputStreamWrapper> rlcDropsTrace, Ptr<RlcStats> stats, Ptr<const Packet> p)
{
    stats->m_nDroppedPdu++;

    std::ostream* os = rlcDropsTrace->GetStream();
    (*os) << Simulator::Now().GetSeconds() << "," << stats->m_nodeId << "," << stats->m_dstL2Id
          << "," << +stats->m_lcId << "," << p->GetSize() << "\n";
}

void
TransmitPacketMcpttApp(PacketTrace* pktTrace,
                       Ptr<const Application> app,
                       uint16_t callId,
                       Ptr<const Packet> pkt,
                       const TypeId& headerType)
{
    if (headerType == ns3::psc::McpttMediaMsg::GetTypeId())
    {
        ns3::psc::McpttMediaMsg mediaMsg;
        pkt->PeekHeader(mediaMsg);

        uint32_t seq = mediaMsg.GetHeader().GetSeqNum();
        Time now = Simulator::Now();
        PacketInfo& info = (*pktTrace)[seq];
        info.txTime = now;
        info.size = pkt->GetSize();
    }

    NrSlProseAppTag appTag;
    pkt->AddByteTag(appTag);
}

void
TransmitPacketSeqTs(PacketTrace* pktTrace,
                    Ptr<const Packet> packet,
                    const Address& from,
                    const Address& to,
                    const SeqTsSizeHeader& header)
{
    uint32_t seq = header.GetSeq();
    Time now = Simulator::Now();

    PacketInfo& info = (*pktTrace)[seq];
    info.txTime = now;
    info.size = header.GetSize();

    NrSlProseAppTag appTag;
    packet->AddByteTag(appTag);
}

void
TrialResult(Ptr<OutputStreamWrapper> trace,
            std::string nodeId,
            uint8_t mcs,
            uint32_t dstL2Id,
            bool success)
{
    *trace->GetStream() << Simulator::Now().GetSeconds() << "," << nodeId << "," << dstL2Id << ","
                        << +mcs << "," << +success << std::endl;
}

void
SinrEstimate(Ptr<OutputStreamWrapper> trace,
             std::string nodeId,
             uint32_t dstL2Id,
             double unfilteredSinrDb,
             double filteredSinrDb,
             double sinrOffsetDb,
             double sinrEstimateDb)
{
    *trace->GetStream() << Simulator::Now().GetSeconds() << "," << nodeId << "," << dstL2Id << ","
                        << unfilteredSinrDb << "," << filteredSinrDb << "," << sinrOffsetDb << ","
                        << sinrEstimateDb << std::endl;
}

namespace
{
const char*
HarqStatusString(SlHarqInfo::HarqStatus status)
{
    switch (status)
    {
    case SlHarqInfo::ACK:
        return "ACK";
    case SlHarqInfo::NACK:
        return "NACK";
    case SlHarqInfo::TIMEOUT:
        return "TIMEOUT";
    case SlHarqInfo::UNSET:
        return "UNSET";
    }
    return "INVALID";
}
} // namespace

void
TraceHarqFeedbackReceived(Ptr<OutputStreamWrapper> trace,
                          std::string nodeId,
                          const SlHarqInfo& info)
{
    auto& s = *trace->GetStream();
    s << Simulator::Now().GetSeconds() << "," << nodeId << "," << +info.m_harqProcessId << ","
      << info.m_dstL2Id << "," << +info.m_mcs << ",";
    if (info.m_txIndex.has_value())
    {
        s << +info.m_txIndex.value();
    }
    s << ",";
    if (info.m_selectedNumTx.has_value())
    {
        s << +info.m_selectedNumTx.value();
    }
    s << ",";
    if (info.m_isFinal.has_value())
    {
        s << (info.m_isFinal.value() ? 1 : 0);
    }
    s << "," << HarqStatusString(info.m_harqStatus) << "\n";
}

void
TraceHarqTbCompletion(Ptr<OutputStreamWrapper> trace, std::string nodeId, const SlHarqInfo& info)
{
    auto& s = *trace->GetStream();
    s << Simulator::Now().GetSeconds() << "," << nodeId << "," << +info.m_harqProcessId << ","
      << info.m_dstL2Id << "," << +info.m_mcs << ","
      << (info.m_selectedNumTx.has_value() ? +info.m_selectedNumTx.value() : 0) << ","
      << (info.m_txIndex.has_value() ? +info.m_txIndex.value() : 0) << ","
      << HarqStatusString(info.m_harqStatus) << "\n";
}
