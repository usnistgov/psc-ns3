//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file nr-prose-olsrv2-rlf.cc
 * @brief An example using ProSe Unicast communication and L3 U2U relay using
 *        OLSRv2/NHDP as a MANET router
 *
 * Channel configuration:
 * This example setups an NR sidelink out-of-coverage simulation using the
 * Matrix channel model to control the ability for nodes to hear one another.
 *
 * System configuration:
 * Sidelink will use one operational band, containing one component carrier,
 * and a single bandwidth part centered at the frequency specified by the
 * corresponding input parameter. The system bandwidth, the numerology to
 * be used and the transmission power can be configured as well.  However, this
 * is mainly an L3 U2U demonstration so the physical layer configuration does
 * not matter so much.
 *
 * Topology:
 * There are 2 UEs in the topology located with an inter-UE distance of 100 meters.
 * However, we control the ability for nodes to hear one another according to the
 * topology shown below by using the MatrixPropagationLossModel.  The tuple displayed
 * below each node is the coordinate position vector.
 *
 *          UE1..............UE2
 *   (0.0, 0.0, 1.5)     (100, 0.0, 1.5)
 *
 * The ns-3 node with the ID of zero is created but unused in the topology.
 *
 * Identifiers:
 *   - UE1:  IP = 8.0.0.1, L2ID = 1
 *   - UE2:  IP = 8.0.0.2, L2ID = 2
 *
 * ProSe Unicast:
 * A ProSe direct link is formed according to each dotted line depicted above, as
 * relays are discovered by ProSe Discovery.
 *
 * Traffic:
 * There is only one CBR traffic flow that goes from UE1 towards UE4. The
 * packet size, data rate and starting time of the flow can be specified in
 * the input parameters.
 *
 * Link impairments:
 *
 * At time 20 seconds into the simulation, the link between UE2 and UE3 is blocked.
 *
 *          UE1........X........UE2
 *   (0.0, 0.0, 1.5)     (100, 0.0, 1.5)
 *
 * The OLSRv2 eventually learns this and reroutes
 *
 * Output:
 *
 * Total Tx bits = 756800
 * Total Tx packets = 473
 * Total Rx bits = 673600
 * Total Rx packets = 421
 * Avrg thput = 7.01667 kbps
 * Average Packet Inter-Reception (PIR) 0.198512 sec
 *
 * The example also produces PCAP output files for each node
 * 1. NrSlPc5SignallingPacketTrace.txt: log of the transmitted and received PC5
 * signaling messages used for the establishment of the ProSe unicast direct
 * link.
 *  - NrSlDiscoveryTrace.txt
 *  - NrSlPc5SignallingPacketTrace.txt
 *  - nr-prose-olsrv2-rlf.routes
 *  - nr-prose-olsrv2-rlf-n*-i1.pcap
 * 2. The RlfTrace.txt contains the events that lead to RLF, the time that it
 * occured
 * \code{.unparsed}
$ ./ns3 run "nr-prose-olsrv2-rlf -- --PrintHelp"
    \endcode
 *
 * If this program is run with the NS_LOG component "NrProseOlsrv2", a few events
 * are noted, including, times that the link between 1 and 2 are broken and restored::
 *
 * +20.000000000s -1 NrProseOlsrv2:BreakLink(): [INFO ] Breaking link between 1 and 2
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nhdp-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/olsrv2-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <iomanip>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("NrProseOlsrv2");

/*
 * Global methods and variable to hook trace sources from different layers of
 * the protocol stack.
 */

Ptr<MatrixPropagationLossModel> g_lossModel;

void
NotifyLinkEstablished(std::string context,
                      uint32_t selfL2Id,
                      Ipv4Address selfIpv4Addr,
                      uint32_t peerL2Id,
                      Ipv4Address peerIpv4Addr)
{
    NS_LOG_INFO("Established " << selfIpv4Addr << " to " << peerIpv4Addr);
}

void
NotifyLinkReleasing(std::string context,
                    uint32_t selfL2Id,
                    Ipv4Address selfIpv4Addr,
                    uint32_t peerL2Id,
                    Ipv4Address peerIpv4Addr)
{
    NS_LOG_INFO("Releasing " << selfIpv4Addr << " to " << peerIpv4Addr);
}

/**
 * @brief Method to listen to the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * @param pscchStats Pointer to the \link UeMacPscchTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchScheduling(UeMacPscchTxOutputStats* pscchStats,
                        const SlPscchUeMacStatParameters pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

/**
 * @brief Method to listen to the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * @param psschStats Pointer to the \link UeMacPsschTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschScheduling(UeMacPsschTxOutputStats* psschStats,
                        const SlPsschUeMacStatParameters psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

void
AdjustLink(Ptr<Node> a, Ptr<Node> b, double pathloss)
{
    NS_LOG_INFO("Adjusting link between " << a->GetId() << " and " << b->GetId());
    g_lossModel->SetLoss(a->GetObject<MobilityModel>(),
                         b->GetObject<MobilityModel>(),
                         pathloss,
                         true);
}

/**
 * @brief Method to listen to the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * @param pscchStats Pointer to the \link UePhyPscchRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param pscchStatsParams Parameters of the trace source.
 */
void
NotifySlPscchRx(UePhyPscchRxOutputStats* pscchStats,
                const SlRxCtrlPacketTraceParams pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

/**
 * @brief Method to listen to the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * @param psschStats Pointer to the \link UePhyPsschRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * @param psschStatsParams Parameters of the trace source.
 */
void
NotifySlPsschRx(UePhyPsschRxOutputStats* psschStats,
                const SlRxDataPacketTraceParams psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

/**
 * @brief Method to listen to the application level traces of type TxWithAddresses
 *        and RxWithAddresses.
 * @param stats Pointer to the \link UeToUePktTxRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database. *
 * @param nodeId The node id of the TX or RX node
 * @param localAddrs The local IPV4 address of the node
 * @param txRx The string indicating the type of node, i.e., TX or RX
 * @param p The packet
 * @param srcAddrs The source address from the trace
 * @param dstAddrs The destination address from the trace
 * @param seqTsSizeHeader The SeqTsSizeHeader
 */
void
UePacketTraceDb(UeToUePktTxRxOutputStats* stats,
                Ptr<Node> node,
                const Address& localAddrs,
                std::string txRx,
                Ptr<const Packet> p,
                const Address& srcAddrs,
                const Address& dstAddrs,
                const SeqTsSizeHeader& seqTsSizeHeader)
{
    uint32_t nodeId = node->GetId();
    uint64_t imsi = node->GetDevice(0)->GetObject<NrUeNetDevice>()->GetImsi();
    uint32_t seq = seqTsSizeHeader.GetSeq();
    uint32_t pktSize = p->GetSize() + seqTsSizeHeader.GetSerializedSize();

    stats->Save(txRx, localAddrs, nodeId, imsi, pktSize, srcAddrs, dstAddrs, seq);
}

/*
 * Global variables to count TX/RX packets and bytes.
 */

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

/*
 * Global variables used to compute PIR
 */
uint64_t pirCounter = 0;
Time lastPktRxTime;
Time pir;

/**
 * @brief This method listens to the packet sink application trace Rx.
 * @param packet The packet
 * @param from The address of the transmitter
 */
void
ComputePir(Ptr<const Packet> packet, const Address& from [[maybe_unused]])
{
    if (pirCounter == 0 && lastPktRxTime.GetSeconds() == 0.0)
    {
        // this the first packet, just store the time and get out
        lastPktRxTime = Simulator::Now();
        return;
    }
    pir = pir + (Simulator::Now() - lastPktRxTime);
    lastPktRxTime = Simulator::Now();
    pirCounter++;
}

/*
 * @brief Trace sink function for logging transmission and reception of PC5
 *        signaling (PC5-S) messages
 *
 * @param stream the output stream wrapper where the trace will be written
 * @param node the pointer to the UE node
 * @param srcL2Id the L2 ID of the UE sending the PC5-S packet
 * @param dstL2Id the L2 ID of the UE receiving the PC5-S packet
 * @param isTx flag that indicates if the UE is transmitting the PC5-S packet
 * @param p the PC5-S packet
 */
void
TraceSinkPC5SignallingPacketTrace(Ptr<OutputStreamWrapper> stream,
                                  Ptr<Node> node,
                                  uint32_t srcL2Id,
                                  uint32_t dstL2Id,
                                  bool isTx,
                                  Ptr<Packet> p)
{
    NrSlPc5SignallingMessageType pc5smt;
    p->PeekHeader(pc5smt);
    *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << node->GetId();
    if (isTx)
    {
        *stream->GetStream() << "\t"
                             << "TX";
    }
    else
    {
        *stream->GetStream() << "\t"
                             << "RX";
    }
    *stream->GetStream() << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << pc5smt.GetMessageName();
    *stream->GetStream() << std::endl;
}

void
RlfTrace(Ptr<OutputStreamWrapper> stream,
         uint32_t srcL2Id,
         uint32_t dstL2Id,
         NrSlL3ManetService::RlfEvent event)
{
    std::string eventName;
    *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << srcL2Id << "\t" << dstL2Id;
    switch (event)
    {
    case NrSlL3ManetService::RlfEvent::Harq:
        eventName = "harq";
        break;
    case NrSlL3ManetService::RlfEvent::KeepAlive:
        eventName = "keep-alive";
        break;
    case NrSlL3ManetService::RlfEvent::SdRsrp:
        eventName = "sd-rsrp";
        break;
    case NrSlL3ManetService::RlfEvent::SlRsrp:
        eventName = "sl-rsrp";
        break;
    default:
        NS_ABORT_MSG("Unknown RLF event");
    }
    *stream->GetStream() << "\t" << eventName << std::endl;
}

int
main(int argc, char* argv[])
{
    // Traffic parameters
    uint32_t udpPacketSize = 200; // bytes
    double dataRate = 16;         // 16 kilobits per second

    // Simulation timeline parameters
    double nominalPathloss = 0;           // The initial and "restored" pathloss (dB)
    double adjustedPathloss = 1000;       // The "broken" link pathloss (dB)
    Time simTime = Seconds(100);          // Total simulation time
    Time startTrafficTime = Seconds(3);   // Time to start the traffic in the application layer
    Time breakLinkTime = Seconds(20.0);   // Time to break the N1<->N2 link
    Time restoreLinkTime = Seconds(60.0); // Time to restore the N1<->N2 link

    // NR parameters
    uint16_t numerologyBwpSl = 2;           // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
    uint16_t bandwidthBandSl = 400;         // Multiple of 100 KHz; 400 = 40 MHz
    double txPower = 23;                    // dBm

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";
    bool log{false};

    // RLF parameters
    bool useSdRsrp = false; // Indicates if SD-RSRP measurements should be used to detect RLF
    bool useSlRsrp = false; // Indicates if SL-RSRP measurements should be used to detect RLF
    bool useHarq = false;   // Indicates if HARQ-FB should be used for the data links
    double rsrpThreshold =
        -110; // The RSRP threshold (dBm) when enabling SD-RSRP/SL-RSRP RLF detection
    double rsrpCoefficient = 0.5; // The coefficient when enabling SD-RSRP/SL-RSRP RLF detection
    double rsrpHysteresis = 10;   // The hysteresis when enabling SD-RSRP/SL-RSRP RLF detection
    uint32_t slMaxNumConsecutiveDtx =
        0;                   // Set sl-maxNumConsecutiveDTX (a value of 0 disables this mechanism)
    Time t5084 = Seconds(5); // Set the delay of timer T5084 (ProSe keep-alive procedure)
    Time t5085 =
        Seconds(5); // Set the dealy of timer T5085 (ProSe keep-alive request retransmission)

    CommandLine cmd;

    cmd.AddValue("packetSizeBe", "packet size in bytes to be used by the traffic", udpPacketSize);
    cmd.AddValue("dataRate", "The data rate in kilobits per second", dataRate);
    cmd.AddValue("simTime", "Simulation time in seconds", simTime);
    cmd.AddValue("startTrafficTime",
                 "Time to start the traffic in the application layer, in seconds",
                 startTrafficTime);
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
    cmd.AddValue("log", "Logging flag", log);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("breakLinkTime", "The time at which to \"break\" the link", breakLinkTime);
    cmd.AddValue("restoreLinkTime", "The time at which to restore the link", restoreLinkTime);
    cmd.AddValue("nominalPathloss",
                 "The initial and restored pathloss (dB) to use for the link",
                 nominalPathloss);
    cmd.AddValue("adjustedPathloss",
                 "The adjusted pathloss (dB) to use to \"break\" the link",
                 adjustedPathloss);
    cmd.AddValue("useSdRsrp",
                 "Indicates if SD-RSRP measurements should be used for RLF detection",
                 useSdRsrp);
    cmd.AddValue("useSlRsrp",
                 "Indicates if SL-RSRP measurements should be used for RLF detection",
                 useSlRsrp);
    cmd.AddValue("useHarq", "Indicates if HARQ-feedback should be used", useHarq);
    cmd.AddValue("rsrpThreshold", "The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold", rsrpThreshold);
    cmd.AddValue("rsrpCoefficient", "The SD-RSRP and/or SL-RSRP coefficent", rsrpCoefficient);
    cmd.AddValue("rsrpHysteresis", "The SD-RSRP and/or SL-RSRP (dB) hysteresis", rsrpHysteresis);
    cmd.AddValue("slMaxNumConsecutiveDtx",
                 "The maximum number of PSFCH absences to tolerate before declaring RLF",
                 slMaxNumConsecutiveDtx);
    cmd.AddValue("t5084", "The value to use for the ProSe keep-alive procedure timer", t5084);
    cmd.AddValue("t5085",
                 "The value to use for the ProSe keep-alive request retransmission timer",
                 t5085);

    // Parse the command line
    cmd.Parse(argc, argv);

    if (log)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
        LogComponentEnable("NrSlUeProseDirectLink", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlL3ManetService", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeService", LOG_LEVEL_ALL);
    }

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Burn one node, so that node ID 0 is not used and Node IDs align with dstL2Ids
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    // Node creation
    NodeContainer ueVoiceContainer;
    NodeContainer manetContainer;

    auto n1 = CreateObject<Node>();
    ueVoiceContainer.Add(n1);
    manetContainer.Add(n1);
    auto n2 = CreateObject<Node>();
    ueVoiceContainer.Add(n2);
    manetContainer.Add(n2);

    // UE nodes mobility setup
    auto mm1 = CreateObject<ConstantPositionMobilityModel>();
    mm1->SetPosition(Vector(0, 0, 1.5));
    n1->AggregateObject(mm1);
    auto mm2 = CreateObject<ConstantPositionMobilityModel>();
    mm2->SetPosition(Vector(100, 0, 1.5));
    n2->AggregateObject(mm2);

    /*
     * Setup the NR module. We create the NrHelper, which takes care of
     * creating and connecting the various part of the NR sidelink stack
     */
    auto nrHelper = CreateObject<NrSlHelper>();

    // Basic channel with connectivity controlled by MatrixPropagationLossModel
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    g_lossModel = CreateObject<MatrixPropagationLossModel>();
    // auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    // channel->SetPropagationDelayModel(delayModel);
    channel->AddPropagationLossModel(g_lossModel);
    // Set initial loss values.  The value of 0 below indicates 0 dB (no loss), while the value
    // 1000 configures 1000 dB (total loss).  The loss values are bidirectional by default.
    g_lossModel->SetLoss(n1->GetObject<MobilityModel>(),
                         n2->GetObject<MobilityModel>(),
                         nominalPathloss);

    // Schedule times for the link between n2 and n3 to break and be restored
    Simulator::Schedule(breakLinkTime, &AdjustLink, n1, n2, adjustedPathloss);
    Simulator::Schedule(restoreLinkTime, &AdjustLink, n1, n2, nominalPathloss);

    // Static error model is suitable for this configuration
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

    // NR Sidelink attribute of UE MAC, which are would be common for all the UEs
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));
    nrHelper->SetUeMacAttribute("NumHarqProcess", UintegerValue(4));
    nrHelper->SetUeMacAttribute("SlMaxNumConsecutiveDtx", UintegerValue(slMaxNumConsecutiveDtx));

    uint8_t bwpIdForGbrMcptt = 0;

    // following parameter has no impact at the moment because:
    // 1. No support for PQI based mapping between the application and the LCs
    // 2. No scheduler to consider PQI
    // However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
    // because we hard coded the PQI 65 in UE RRC.
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK",
                                                UintegerValue(bwpIdForGbrMcptt));

    std::set<uint8_t> bwpIdContainer;
    bwpIdContainer.insert(bwpIdForGbrMcptt);

    NetDeviceContainer ueVoiceNetDev = nrHelper->InstallUeDevice(manetContainer, allBwps);

    /*
     * Set the SL scheduler attributes
     * In this example we use NrSlUeMacSchedulerDefault scheduler, which uses
     * fixed MCS value and schedules logical channels by priority order first
     * and then by creation order
     */
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(0));

    /*
     * Very important method to configure UE protocol stack, i.e., it would
     * configure all the SAPs among the layers, setup callbacks, configure
     * error model, configure AMC, and configure ChunkProcessor in Interference
     * API.
     */
    nrHelper->PrepareUeForSidelink(ueVoiceNetDev, bwpIdContainer);

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

    if (useHarq)
    {
        ptrFactory->SetSlPsfchPeriod(4);
        ptrFactory->SetSlMinTimeGapPsfch(3);
    }

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
    nrHelper->InstallNrSlPreConfiguration(ueVoiceNetDev, slPreConfigNr);

    /****************************** End SL Configuration ***********************/

    /*
     * Configure the IPv4 stack
     */
    Olsrv2Helper olsrv2;
    Ipv4ListRoutingHelper list;
    list.Add(olsrv2, 10);
    InternetStackHelper internet;
    internet.SetRoutingHelper(list);
    internet.Install(manetContainer);
    NhdpHelper nhdpHelper;
    auto nhdpApps [[maybe_unused]] = nhdpHelper.Install(manetContainer);

    /*
     * Fix the random streams
     */
    int64_t streamBase{1000};
    [[maybe_unused]] int64_t streamsUsed{0};
    streamsUsed = nrHelper->AssignStreams(ueVoiceNetDev, streamBase);
    streamBase = 2000;
    streamsUsed = internet.AssignStreams(manetContainer, streamBase);
    streamBase = 3000;
    streamsUsed = olsrv2.AssignStreams(manetContainer, streamBase);
    NS_LOG_DEBUG("Streams used by OLSRv2 models: " << streamsUsed);
    streamBase = 4000;
    streamsUsed = nhdpHelper.AssignStreams(manetContainer, streamBase);
    NS_LOG_DEBUG("Streams used by NHDP models: " << streamsUsed);

    uint16_t port = 8000;
    Ipv4AddressHelper addrHelper;
    auto ueIpIface = addrHelper.AssignManet(ueVoiceNetDev, Ipv4Address("8.0.0.1"));
    // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the
    // direct link
    Ipv4Address remoteAddress1, remoteAddress2;
    remoteAddress1 =
        ueVoiceContainer.Get(0)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal(); // UE1
    remoteAddress2 =
        ueVoiceContainer.Get(1)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal(); // UE2

    /*
     * Configure ProSe
     */
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryInterval", TimeValue(Seconds(2)));
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryStart", TimeValue(Seconds(2.5)));
    Config::SetDefault("ns3::NrSlL3ManetService::UseSdRsrpForRlf", BooleanValue(useSdRsrp));
    Config::SetDefault("ns3::NrSlL3ManetService::UseSlRsrpForRlf", BooleanValue(useSlRsrp));
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5084", TimeValue(t5084));
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5085", TimeValue(t5085));

    // Create ProSe helper
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    // Install ProSe layer and corresponding SAPs in the UEs
    nrSlProseHelper->PrepareUesForL3Manet(ueVoiceNetDev);

    // Traffic profile for unicast links  signaling bearers
    SidelinkInfo slSrbSlInfo;
    slSrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slSrbSlInfo.m_dynamic = true;
    slSrbSlInfo.m_pdb = MilliSeconds(100);
    slSrbSlInfo.m_priority = 1;

    nrSlProseHelper->SetL3ManetDefaultSlSrbSlInfo(ueVoiceNetDev, slSrbSlInfo);

    // Traffic profile for unicast links default data bearers
    SidelinkInfo slDrbSlInfo;
    slDrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;

    if (useHarq)
    {
        slDrbSlInfo.m_dynamic = false;
        slDrbSlInfo.m_harqEnabled = true;
        slDrbSlInfo.m_pdb = MilliSeconds(100);
        slDrbSlInfo.m_rri = MilliSeconds(100);
    }
    else
    {
        slDrbSlInfo.m_dynamic = true;
        slDrbSlInfo.m_harqEnabled = false;
        slDrbSlInfo.m_pdb = MilliSeconds(100);
    }

    nrSlProseHelper->SetL3ManetDefaultSlDrbSlInfo(ueVoiceNetDev, slDrbSlInfo);

    NrSlRrcSap::SlRemoteUeConfig slRemoteConfig;
    slRemoteConfig.slReselectionConfig.slRsrpThres = rsrpThreshold; // dBm
    slRemoteConfig.slReselectionConfig.slFilterCoefficientRsrp = rsrpCoefficient;
    slRemoteConfig.slReselectionConfig.slHystMin = rsrpHysteresis; // dB

    // Define SD-/SL-RSRP thresholds and enable RSRP measurements for UEs
    for (uint32_t i = 0; i < ueVoiceNetDev.GetN(); ++i)
    {
        Ptr<NrSlUeRrc> remoteRrc =
            ueVoiceNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetRrc()->GetObject<NrSlUeRrc>();
        remoteRrc->EnableUeSdRsrpMeasurements();
        remoteRrc->EnableUeSlRsrpMeasurements();
        remoteRrc->SetNrSlDiscoveryRemoteConfiguration(slRemoteConfig);
    }

    /*********************** End ProSe configuration ***************************/

    /*
     * Configure the applications:
     * - Client app: OnOff application configured to generate CBR traffic. Installed in UE1
     * - Server app: PacketSink application to consume the received traffic. Installed in UE2
     */
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", InetSocketAddress(remoteAddress2, port));
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    std::string dataRateString = std::to_string(dataRate) + "kb/s";
    std::cout << "Data rate " << DataRate(dataRateString) << std::endl;
    sidelinkClient.SetConstantRate(DataRate(dataRateString), udpPacketSize);

    ApplicationContainer clientApps =
        sidelinkClient.Install(ueVoiceContainer.Get(0)); // Installed in UE1
    clientApps.Start(startTrafficTime);
    clientApps.Stop(simTime - Seconds(1));

    std::cout << "App start time " << startTrafficTime.GetSeconds() << " sec" << std::endl;
    std::cout << "App stop time " << simTime.GetSeconds() - 1 << " sec" << std::endl;

    ApplicationContainer serverApps;
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory",
                                  InetSocketAddress(Ipv4Address::GetAny(), port));
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    serverApps = sidelinkSink.Install(ueVoiceContainer.Get(1)); // Installed in UE2
    serverApps.Start(Seconds(2.0));

    streamBase = 3000;
    streamsUsed = ApplicationHelper::AssignStreamsToAllApps(ueVoiceContainer, streamBase);

    /******************** End application configuration ************************/

    /*
     * Hook the traces, to be used to compute average PIR and to data to be
     * stored in a database
     */

    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> rlfTraceStream = ascii.CreateFileStream("RlfTrace.txt");
    *rlfTraceStream->GetStream() << "time(s)\tsrcL2Id\tdstL2Id\tevent" << std::endl;

    for (uint32_t i = 0; i < ueVoiceNetDev.GetN(); i++)
    {
        Ptr<NrUeNetDevice> device = ueVoiceNetDev.Get(i)->GetObject<NrUeNetDevice>();
        Ptr<NrSlL3ManetService> prose = device->GetObject<NrSlL3ManetService>();
        prose->TraceConnect("DirectLinkEstablished",
                            std::to_string(device->GetNode()->GetId()),
                            MakeCallback(&NotifyLinkEstablished));
        prose->TraceConnect("DirectLinkReleasing",
                            std::to_string(device->GetNode()->GetId()),
                            MakeCallback(&NotifyLinkReleasing));
        prose->TraceConnectWithoutContext("RadioLinkFailure",
                                          MakeBoundCallback(&RlfTrace, rlfTraceStream));
    }

    // Trace receptions; use the following to be robust to node ID changes
    std::ostringstream path;
    path << "/NodeList/" << ueVoiceContainer.Get(1)->GetId()
         << "/ApplicationList/1/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    path.str("");

    path << "/NodeList/" << ueVoiceContainer.Get(1)->GetId()
         << "/ApplicationList/1/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ComputePir));
    path.str("");

    path << "/NodeList/" << ueVoiceContainer.Get(0)->GetId()
         << "/ApplicationList/1/$ns3::OnOffApplication/Tx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&TransmitPacket));
    path.str("");

    /******************* PC5-S messages tracing ********************************/
    Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream =
        ascii.CreateFileStream("NrSlPc5SignallingPacketTrace.txt");
    *Pc5SignallingPacketTraceStream->GetStream()
        << "time(s)\tnodeId\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
    for (uint32_t i = 0; i < ueVoiceNetDev.GetN(); ++i)
    {
        Ptr<NrSlL3ManetService> prose = ueVoiceNetDev.Get(i)->GetObject<NrSlL3ManetService>();
        prose->TraceConnectWithoutContext("PC5SignallingPacketTrace",
                                          MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream,
                                                            ueVoiceNetDev.Get(i)->GetNode()));
    }
    /******************* END PC5-S messages tracing **************************/

    // Enable discovery traces
    nrSlProseHelper->EnableDiscoveryTraces();

    // Enable IPv4 tracing
    for (uint32_t i = 0; i < manetContainer.GetN(); i++)
    {
        internet.EnablePcapIpv4("nr-prose-olsrv2-rlf", manetContainer.Get(i)->GetId(), 1, false);
    }

    // Trace routing tables
    Ptr<OutputStreamWrapper> routingStream =
        Create<OutputStreamWrapper>("nr-prose-olsrv2-rlf.routes", std::ios::out);
    // Trace at the time of the first topology change
    Ipv4RoutingHelper::PrintRoutingTableAllAt(breakLinkTime, routingStream);
    // Trace at the time of the second topology change
    Ipv4RoutingHelper::PrintRoutingTableAllAt(restoreLinkTime, routingStream);
    // Trace towards the end of the simulation
    Ipv4RoutingHelper::PrintRoutingTableAllAt(simTime - Seconds(1), routingStream);

    Simulator::Stop(simTime + TimeStep(1));
    Simulator::Run();

    std::cout << "Total Tx bits = " << txByteCounter * 8 << std::endl;
    std::cout << "Total Tx packets = " << txPktCounter << std::endl;

    std::cout << "Total Rx bits = " << rxByteCounter * 8 << std::endl;
    std::cout << "Total Rx packets = " << rxPktCounter << std::endl;

    std::cout << "Avrg thput = "
              << (rxByteCounter * 8) / (simTime - startTrafficTime - Seconds(1)).GetSeconds() /
                     1000.0
              << " kbps" << std::endl;

    std::cout << "Average Packet Inter-Reception (PIR) " << pir.GetSeconds() / pirCounter << " sec"
              << std::endl;

    Simulator::Destroy();
    return 0;
}
