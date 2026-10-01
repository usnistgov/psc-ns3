//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file nr-prose-multihop-linear.cc
 * @brief Example of ProSe multi-hop U2U relay with OLSRv2/NHDP in a linear UE
 *        topology with VoIP traffic
 *
 * System configuration:
 * A single sidelink band (n14, 793 MHz) with 10 MHz bandwidth and numerology 0.
 * UEs use the default scheduler with configurable default MCS (parameter 'mcs')
 * and sensing‐based scheduling with activation of the whole‐slot exclusion with
 * the parameter 'wSlotEx'. The length of the selection window can be configured by tuning
 * the parameter 't2'.
 * The retransmission scheme and the maximum number of transmissions can be selected using the
 * parameters 'rtxType' and 'maxNumTx' respectively.
 *
 * Topology:
 * The UEs are configured in a straight line at 1.5 m height, spaced by the parameter 'iud' ± 1 m.
 * The total number of UEs can be configured with the parameter 'nUes'.
 *
 * Channel configuration:
 * This example sets up an NR sidelink out-of-coverage simulation using the
 * Matrix channel model so that only adjacent nodes can hear each other.
 *
 * Traffic:
 * Single VoIP flow from the first UE to the last, using an EVS codec that
 * can be selected with the parameter 'codec'.
 *
 * Output:
 * - voip-packet-trace.csv: VoIP application packets timeline with transmission and reception
 *                          timestamps.
 * - voip-stats-sim.csv: VoIP statistics for the simulation including total transmitted and
 *                       received packets, packet loss ratio, average packet delay and Mean
 *                       Opinion Score (MOS).
 * - phy-stats-per-node.csv: Count of PSCCH and PSSCH transmissions and reception outcomes at the
 *                           PHY for each node. All unicast transmissions are counted including
 *                           PC5-S messages, NHDP/OLSR routing messages, and VoIP application
 *                           packets.
 * - phy-stats-sim.csv: Sum of phy-stats-per-node.csv for all nodes in the simulation.
 * - phy-stats-sim-percent.csv: Same as phy-stats-sim.csv but as percentage of the number of
 *                              transmissions.
 * - route-change-trace.csv: IP route changes timeline.
 * - route-change-stats.csv: IP route changes statistics including the number of route changes and
 *                           time without route during traffic activity.
 * - mac-phy-trace.db: Database with NR SL MAC and PHY traces (only written if logMacPhyInDb =
 *                     true).
 * \code{.unparsed}
$ ./ns3 run "nr-prose-multihop-linear -- --PrintHelp"
    \endcode
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

NS_LOG_COMPONENT_DEFINE("NrProseMultihopLinear");

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
 *
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

/**
 * @brief Find a node in a container by its ID.
 *
 * @param id     The identifier of the node to locate.
 * @param nodes  The NodeContainer to search.
 * @return Ptr<Node>  Pointer to the node if found; nullptr otherwise.
 */
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

/**
 * @brief Look up a node’s ID given its IPv4 address.
 *
 * @param ip     The IPv4 address to search for.
 * @param nodes  The NodeContainer housing the nodes.
 * @return uint32_t  The node’s ID if found; UINT32_MAX otherwise.
 */
uint32_t
GetNodeIdFromIp(Ipv4Address ip, NodeContainer nodes)
{
    for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
        Ptr<Node> node = nodes.Get(i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        for (uint32_t j = 0; j < ipv4->GetNInterfaces(); ++j)
        {
            if (ipv4->GetAddress(j, 0).GetLocal() == ip)
            {
                return node->GetId();
            }
        }
    }
    return UINT32_MAX;
}

/**
 * @brief Retrieve the IPv4 address of a node by its ID.
 *
 * @param nodeId  The ID of the node whose IP is required.
 * @param nodes   The NodeContainer to search.
 * @return Ipv4Address  The node’s IP if found; “0.0.0.0” otherwise.
 */
Ipv4Address
GetIpFromNodeId(uint32_t nodeId, NodeContainer nodes)
{
    Ptr<Node> node = FindNodeById(nodeId, nodes);
    if (node)
    {
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4 && ipv4->GetNInterfaces() > 1)
        {
            return ipv4->GetAddress(1, 0).GetLocal();
        }
    }
    return Ipv4Address("0.0.0.0");
}

/**
 * @brief Periodically compute and log the route between two UEs.
 *
 * @param ueNodes          The UE NodeContainer.
 * @param srcNodeId        Source node ID.
 * @param tgtNodeId        Target node ID.
 * @param prevPath         Previously recorded path for change detection.
 * @param interval         Time interval between lookups.
 * @param fileRouteChange  Stream for writing route‐change entries.
 */
void
LookupRoute(NodeContainer ueNodes,
            uint32_t srcNodeId,
            uint32_t tgtNodeId,
            std::vector<uint32_t> prevPath,
            Time interval,
            std::ofstream* fileRouteChange)
{
    std::vector<uint32_t> path;
    std::set<uint32_t> visited;
    uint32_t currentId = srcNodeId;
    bool loop = false, routeFound = true;

    Ipv4Address dstAddr = GetIpFromNodeId(tgtNodeId, ueNodes);
    Ipv4Address srcAddr = GetIpFromNodeId(srcNodeId, ueNodes);

    while (true)
    {
        if (visited.count(currentId))
        {
            loop = true;
            break;
        }
        visited.insert(currentId);
        path.push_back(currentId);

        Ptr<Node> currentNode = FindNodeById(currentId, ueNodes);
        if (!currentNode)
        {
            routeFound = false;
            break;
        }

        Ptr<Ipv4> ipv4 = currentNode->GetObject<Ipv4>();
        if (!ipv4)
        {
            routeFound = false;
            break;
        }

        Ptr<Ipv4RoutingProtocol> rp = ipv4->GetRoutingProtocol();
        Ptr<olsrv2::RoutingProtocol> olsr = DynamicCast<olsrv2::RoutingProtocol>(rp);
        if (!olsr)
        {
            Ptr<Ipv4ListRouting> list = DynamicCast<Ipv4ListRouting>(rp);
            for (uint32_t i = 0; list && i < list->GetNRoutingProtocols(); ++i)
            {
                int16_t prio;
                Ptr<Ipv4RoutingProtocol> proto = list->GetRoutingProtocol(i, prio);
                olsr = DynamicCast<olsrv2::RoutingProtocol>(proto);
                if (olsr)
                    break;
            }
        }

        if (!olsr)
        {
            std::cerr << "No OLSRv2 found on node ID " << currentId << std::endl;
            routeFound = false;
            break;
        }

        Ptr<Packet> dummyPacket = Create<Packet>();
        Ipv4Header dummyHeader;
        dummyHeader.SetSource(srcAddr);
        dummyHeader.SetDestination(dstAddr);
        dummyHeader.SetProtocol(17); // UDP
        Socket::SocketErrno err;

        Ptr<Ipv4Route> route = olsr->RouteOutput(dummyPacket, dummyHeader, nullptr, err);
        if (!route)
        {
            routeFound = false;
            break;
        }

        Ipv4Address nextHopAddr = route->GetGateway();
        if (nextHopAddr == Ipv4Address("0.0.0.0"))
        {
            nextHopAddr = dstAddr;
        }

        uint32_t nextHopId = GetNodeIdFromIp(nextHopAddr, ueNodes);
        if (nextHopId == UINT32_MAX)
        {
            routeFound = false;
            break;
        }

        if (nextHopId == tgtNodeId)
        {
            path.push_back(tgtNodeId);
            break;
        }

        currentId = nextHopId;
    }

    Time now = Simulator::Now();

    if (fileRouteChange && fileRouteChange->is_open())
    {
        if (loop)
        {
            *fileRouteChange << now.GetSeconds() << "," << srcNodeId << "," << tgtNodeId
                             << ",loop\n";
        }
        else if (!routeFound)
        {
            if (!prevPath.empty())
            {
                *fileRouteChange << now.GetSeconds() << "," << srcNodeId << "," << tgtNodeId
                                 << ",NaN\n";
            }
            path.clear(); // avoid triggering re-print of same nan
        }
        else if (path != prevPath)
        {
            *fileRouteChange << now.GetSeconds() << "," << srcNodeId << "," << tgtNodeId << ",";
            for (size_t i = 0; i < path.size(); ++i)
            {
                *fileRouteChange << path[i];
                if (i != path.size() - 1)
                    *fileRouteChange << "->";
            }
            *fileRouteChange << "\n";
        }
    }
    if (fileRouteChange)
        fileRouteChange->flush();
    Simulator::Schedule(interval,
                        &LookupRoute,
                        ueNodes,
                        srcNodeId,
                        tgtNodeId,
                        path,
                        interval,
                        fileRouteChange);
}

/**
 * @brief Parse the route‐change trace and summarize changes and downtime.
 *
 * @param startTime  Start of the analysis window.
 * @param endTime    End of the analysis window.
 */

void
ProcessRouteChanges(Time startTime, Time endTime)
{
    std::ifstream file("route-change-trace.csv");
    if (!file.is_open())
    {
        std::cerr << "Error opening route-change-trace.csv" << std::endl;
        return;
    }

    std::map<std::pair<uint32_t, uint32_t>, std::string> lastKnownPathBeforeWindow;
    std::map<std::pair<uint32_t, uint32_t>, uint16_t> routeChangeCount;
    std::map<std::pair<uint32_t, uint32_t>, double> nanTimeAccum;
    std::map<std::pair<uint32_t, uint32_t>, double> lastNanStart;
    std::map<std::pair<uint32_t, uint32_t>, std::string> lastPath;
    std::set<std::pair<uint32_t, uint32_t>> allPairsSeen;

    std::string line;
    std::getline(file, line); // skip header

    // Read and process trace
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, ','))
            tokens.push_back(token);

        double time = std::stod(tokens[0]);
        uint32_t src = std::stoi(tokens[1]);
        uint32_t dst = std::stoi(tokens[2]);
        std::string path = tokens[3];
        auto key = std::make_pair(src, dst);
        allPairsSeen.insert(key);

        if (time < startTime.GetSeconds())
        {
            lastKnownPathBeforeWindow[key] = path;
            continue;
        }
        if (time > endTime.GetSeconds())
        {
            break;
        }

        if (!lastPath.count(key))
        {
            lastPath[key] = lastKnownPathBeforeWindow[key];
            if (lastKnownPathBeforeWindow[key] == "NaN")
            {
                lastNanStart[key] = startTime.GetSeconds();
            }
        }

        if (path != lastPath[key])
        {
            routeChangeCount[key]++;
            lastPath[key] = path;
        }

        if (path == "NaN")
        {
            if (!lastNanStart.count(key))
            {
                lastNanStart[key] = time;
            }
        }
        else
        {
            if (lastNanStart.count(key))
            {
                nanTimeAccum[key] += time - lastNanStart[key];
                lastNanStart.erase(key);
            }
        }
    }
    // Process entries that had NaN before the window but no update during the window
    for (const auto& kv : lastKnownPathBeforeWindow)
    {
        const auto& key = kv.first;
        const std::string& pathBefore = kv.second;

        if (!lastPath.count(key)) // not processed at all during window
        {
            lastPath[key] = pathBefore;
            if (pathBefore == "NaN")
            {
                lastNanStart[key] = startTime.GetSeconds();
            }
        }
    }
    // Finalize open nan intervals
    for (const auto& it : lastNanStart)
    {
        nanTimeAccum[it.first] += endTime.GetSeconds() - it.second;
    }

    // Output
    std::ofstream csv("route-change-stats.csv");
    csv << "RngSeed,RngRun,SrcNode,DstNode,RouteChanges,TimeWithoutRoute\n";

    std::cout << "\nRoute Change Summary (for the simulation period with traffic ["
              << startTime.GetSeconds() << "s, " << endTime.GetSeconds() << "s]):\n";

    std::cout << std::left << std::setw(10) << "SrcNode"
              << "| ";
    std::cout << std::setw(10) << "DstNode"
              << "| ";
    std::cout << std::setw(15) << "RouteChanges"
              << "| ";
    std::cout << std::setw(20) << "TimeWithoutRoute(s)"
              << "|\n";
    std::cout << std::string(62, '-') << "\n";

    for (const auto& key : allPairsSeen)
    {
        uint16_t changes = routeChangeCount[key];
        double nanTime = nanTimeAccum[key];

        std::cout << std::left << std::setw(10) << key.first << "| ";
        std::cout << std::setw(10) << key.second << "| ";
        std::cout << std::setw(15) << changes << "| ";
        std::cout << std::setw(20) << std::fixed << std::setprecision(4) << nanTime << "|\n";

        csv << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ", " << key.first
            << "," << key.second << "," << changes << "," << nanTime << "\n";
    }

    std::cout << std::string(62, '-') << "\n";
    csv.close();
}

struct CodecParameters
{
    std::string codecName;
    std::string sourceRate;
    Time voicePeriodicity;
    uint16_t voicePayloadBytes;
    Time sidPeriodicity;
    uint16_t sidPayloadBytes;
    double Ie;         // E-model Ie
    double Bpl;        // E-model Bpl
    std::string model; // The E-model to use
};

// Map to hold codec parameters with the key as "CodecName_SourceRate"
// Payload size include RTP header overhead as RTP is not implemented in the application used in
// this scenario
std::map<std::string, CodecParameters> g_codecMap = {
    {"EVS_9.6", {"EVS", "9.6kb/s", MilliSeconds(20), 36, MilliSeconds(160), 24, 22.7, 13, "FB"}},
    {"EVS_13.2",
     {"EVS", "13.2kb/s", MilliSeconds(20), 45, MilliSeconds(160), 24, 17.1, 11.7, "FB"}},
    {"EVS_16.4",
     {"EVS", "16.4kb/s", MilliSeconds(20), 53, MilliSeconds(160), 24, 10.8, 10.3, "FB"}},
    {"EVS_24.4", {"EVS", "24.4kb/s", MilliSeconds(20), 73, MilliSeconds(160), 24, 7.2, 11.4, "FB"}},
    {"EVS_32.0", {"EVS", "32.0kb/s", MilliSeconds(20), 92, MilliSeconds(160), 24, 8.7, 9.3, "FB"}},
    {"EVS_48.0",
     {"EVS", "48.0kb/s", MilliSeconds(20), 132, MilliSeconds(160), 24, 10.2, 9.6, "FB"}},
};

/**
 * @brief Look up codec parameters in the global map by key.
 * @param codecKey  Identifier in form "CodecName_SourceRate".
 * @return CodecParameters struct for the given key.
 */
CodecParameters
GetCodecParameters(const std::string& codecKey)
{
    auto it = g_codecMap.find(codecKey);
    if (it != g_codecMap.end())
    {
        return it->second;
    }
    else
    {
        NS_FATAL_ERROR("Codec parameters not found for key: " + codecKey);
    }
}

// Global variable to hold the codec parameters to be used
CodecParameters g_codec_params;

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

using PacketTrace = std::map<uint32_t, PacketInfo>; // seqNum -> PacketInfo

/**
 * @brief Record transmission time and size for an application packet.
 *
 * @param pktTrace  Pointer to the pacet trace map
 * @param packet    The Packet being transmitted.
 * @param from      Source Address.
 * @param to        Destination Address.
 * @param header    SeqTsSizeHeader carrying seq and size metadata.
 */
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
}

/**
 * @brief Record reception time, delay, and size for an application packet.
 * @param pktTrace  Pointer to the pacet trace map
 * @param packet    The Packet being received.
 * @param from      Source Address.
 * @param to        Destination Address.
 * @param header    SeqTsSizeHeader carrying seq and timestamp.
 */
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

/**
 * @brief Calculate the R‐factor using the ITU-T G.107.2 E-model.
 *
 * @param avgDelay         Average one-way delay in milliseconds.
 * @param packetLossRatio  Packet loss ratio.
 * @param codecParams      Codec parameters
 * @return double          Computed R-factor.
 */
double
CalculateRFactor(double avgDelay, double packetLossRatio, CodecParameters codecParams)
{
    double Ta = avgDelay;
    double Ppl = packetLossRatio * 100;

    double Bpl = codecParams.Bpl;
    double Ie = codecParams.Ie;

    double R0 = 0;
    double Is = 0;
    double Id = 0;
    double IeEff = 0;
    double A = 0; // Default value
    double Idd = 0;
    double X = 0;
    double mT = 0;
    double sT = 0;
    double SLR = 0;
    double No = 0;
    double Nc = 0;
    double Nfo = 0;
    double rFactor = 0;

    if (codecParams.model == "FB") // Fromula and parameters from  ITU-T Recommendation G.107.2
    {
        mT = 100;  // ms (target delay)
        sT = 1;    // Depends on mT
        Nc = -96;  // Default value
        Nfo = -96; // Default value
        SLR = 8;   // Default value
        Is = 0;

        // Assuming no circuit noise due to room noise:
        No = 10.0 * log10(pow(10.0, Nc / 10.0) + pow(10.0, Nfo / 10.0));
        R0 = 20 - 1.5 * (SLR + No);

        // Currently (Edition 2.0), echo is not part of the model, so Id = Idd
        if (avgDelay <= mT) // ms
        {
            Idd = 0;
        }
        else
        {
            X = log10(Ta / mT) / log10(2.0);
            double exponent = 6.0 * sT;
            double root = 1.0 / exponent;
            double term1 = pow(1.0 + pow(X, exponent), root);
            double term2 = pow(1.0 + pow(X / 3.0, exponent), root);
            Idd = 1.48 * 25.0 * (term1 - 3.0 * term2 + 2.0);
        }
        Id = Idd;

        // Assuming packet loss is random (i.e., independent)
        IeEff = Ie + (132.0 - Ie) * (Ppl / (Ppl + Bpl));

        rFactor = R0 - Is - Id - IeEff + A;
    }
    else
    {
        NS_FATAL_ERROR("E-model " << codecParams.model << " not recognized.");
    }
    return rFactor;
}

/**
 * @brief Compute the Mean Opinion Score (MOS) from an R-factor.
 *
 * @param rFactor      R-factor value.
 * @param codecParams  CodecParameters specifying E-model.
 * @return double      MOS in the range [1.0, 4.5].
 */
double
CalculateMOS(double rFactor, CodecParameters codecParams)
{
    double mos = 0;
    if (codecParams.model == "FB") // Fromula and parameters from  ITU-T Recommendation G.107.2
    {
        double Rx = rFactor / 1.48;
        if (Rx < 0)
        {
            mos = 1;
        }
        else if (Rx > 0 && Rx <= 100)
        {
            mos = 1.0 + 0.035 * Rx + Rx * (Rx - 60.0) * (100.0 - Rx) * 0.000007;
        }
        else if (Rx > 100)
        {
            mos = 4.5;
        }
    }
    else
    {
        NS_FATAL_ERROR("E-model " << codecParams.model << "not recognized.");
    }
    return mos;
}

/**
 * @brief Process the full packet trace to compute VoIP metrics, write CSVs, and print summary.
 *
 * @param pktTrace      Packet trace
 * @param codecParams   CodecParameters for calculating R-factor and MOS.
 */
void
ProcessPacketTrace(const PacketTrace& pktTrace, const CodecParameters& codecParams)
{
    uint32_t nTx = pktTrace.size();
    uint32_t nRx = 0;
    double totalDelayMs = 0.0;

    // Save full packet trace
    std::ofstream fileTrace("voip-packet-trace.csv");
    if (!fileTrace.is_open())
    {
        std::cerr << "Error: Could not open voip-packet-trace.csv\n";
        return;
    }
    fileTrace << "TxTime(s),RxTime(s),Seq,Size,RxDelay(ms)\n";

    for (const auto& [seq, info] : pktTrace)
    {
        fileTrace << std::fixed << std::setprecision(6) << info.txTime.GetSeconds() << ","
                  << info.rxTime.GetSeconds() << "," << seq << "," << info.size << ",";

        if (info.rxTime != Seconds(0))
        {
            nRx++;
            totalDelayMs += info.rxDelay.GetMilliSeconds();
            fileTrace << info.rxDelay.GetMilliSeconds() << "\n";
        }
        else
        {
            fileTrace << "NaN"
                      << "\n";
        }
    }

    fileTrace.close();

    // Compute stats
    double lossRatio = (nTx == 0) ? 0.0 : static_cast<double>(nTx - nRx) / nTx;
    double avgDelay = 0.0;
    double mos = 0.0;
    std::string avgDelayStr, mosStr;

    if (lossRatio == 1.0)
    {
        avgDelayStr = "NaN";
        mosStr = "NaN";
    }
    else
    {
        avgDelay = (nRx == 0) ? 0.0 : totalDelayMs / nRx;
        avgDelayStr = std::to_string(avgDelay);

        double rFactor = CalculateRFactor(avgDelay, lossRatio, codecParams);
        mos = CalculateMOS(rFactor, codecParams);
        mosStr = std::to_string(mos);
    }

    // Append stats to voipStats.csv
    std::ofstream fileStats("voip-stats-sim.csv");
    if (!fileStats.is_open())
    {
        std::cerr << "Error: Could not open voip-stats-sim.csv\n";
        return;
    }
    fileStats << "RngSeed,RngRun,nTxPkts,nRxPkts,LossRatio,AvgDelay,Mos\n";

    fileStats << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << "," << nTx << ","
              << nRx << "," << std::fixed << std::setprecision(4) << lossRatio << "," << avgDelayStr
              << "," << mosStr << "\n";
    fileStats.close();

    // Console output in aligned columns
    std::cout << "\nVoIP Statistics:\n";
    std::cout << std::left << std::setw(10) << "RngSeed"
              << "| ";
    std::cout << std::setw(8) << "RngRun"
              << "| ";
    std::cout << std::setw(10) << "nTxPkts"
              << "| ";
    std::cout << std::setw(10) << "nRxPkts"
              << "| ";
    std::cout << std::setw(10) << "LossRatio"
              << "| ";
    std::cout << std::setw(14) << "AvgDelay(ms)"
              << "| ";
    std::cout << std::setw(10) << "MOS "
              << "|\n";

    std::cout << std::string(85, '-') << "\n";

    std::cout << std::left << std::setw(10) << RngSeedManager::GetSeed() << "| ";
    std::cout << std::setw(8) << RngSeedManager::GetRun() << "| ";
    std::cout << std::setw(10) << nTx << "| ";
    std::cout << std::setw(10) << nRx << "| ";
    std::cout << std::setw(10) << std::fixed << std::setprecision(4) << lossRatio << "| ";
    std::cout << std::setw(14) << avgDelayStr << "| ";
    std::cout << std::setw(10) << mosStr << "|\n";

    std::cout << std::string(85, '-') << "\n\n";
}

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
        uint64_t nCorruptRxData = 0;      // PSSCH corrupted (BLER)
        uint64_t nHdRxCtrl = 0;           // PSCCH ignored due to half duplex
        uint64_t nHdRxData = 0;           // PSSCH ignored due to half duplex
        uint64_t nDataNotExpected = 0;    // Not expected (PSCCH not received)
        uint64_t nDataAlreadyDecoded = 0; // Not expected (retransmission of an already decoded TB)
    };

    uint32_t m_nodeId;
    uint32_t m_l2Id;
    PhyStats m_stats;
};

/**
 * @brief Trace sink for SL channels (PSCCH/PSSCH) events that updates PHY statistics.
 * @param stats    Pointer to the NodePhyStats object
 * @param type     The SL event type
 * @param pktBurst The PacketBurst associated with this event
 */
void
TraceSlChannelsEvent(NodePhyStats* stats,
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

        // skip non-application layer packets
        bool hasApp = false;
        uint16_t appPacketSizeAtPhy = g_codec_params.voicePayloadBytes + 35; // Bytes
        for (auto p : pktBurst->GetPackets())
        {
            if (p->GetSize() != appPacketSizeAtPhy)
            {
                continue;
            }
            else
            {
                hasApp = true;
                break;
            }
        }
        if (!hasApp)
        {
            return; // no application‐layer packet in this burst
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

        // skip non-application layer packets
        bool hasApp = false;
        uint16_t appPacketSizeAtPhy = g_codec_params.voicePayloadBytes + 35; // Bytes
        for (auto p : pktBurst->GetPackets())
        {
            if (p->GetSize() != appPacketSizeAtPhy)
            {
                continue;
            }
            else
            {
                hasApp = true;
                break;
            }
        }
        if (!hasApp)
        {
            return; // no application‐layer packet in this burst
        }
    }
    else
    {
        NS_ABORT_MSG("Unknown SlEventType " << +static_cast<uint8_t>(type));
    }

    // Trace transmissions for stats
    if (type == NrSlSpectrumPhy::SlEventType::PSCCH_TX)
    {
        stats->m_stats.nTxCtrl++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_OK)
    {
        stats->m_stats.nRxCtrl++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_DECODE_FAILURE)
    {
        stats->m_stats.nCorruptRxCtrl++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSCCH_RX_HALF_DUPLEX)
    {
        stats->m_stats.nHdRxCtrl++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_TX)
    {
        stats->m_stats.nTxData++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_OK)
    {
        stats->m_stats.nRxData++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_SCI2 ||
             type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_TB)
    {
        stats->m_stats.nCorruptRxData++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_HALF_DUPLEX)
    {
        stats->m_stats.nHdRxData++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_NOT_EXPECTED)
    {
        stats->m_stats.nDataNotExpected++;
    }
    else if (type == NrSlSpectrumPhy::SlEventType::PSSCH_RX_ALREADY_DECODED)
    {
        stats->m_stats.nDataAlreadyDecoded++;
    }
    else
    {
        NS_ABORT_MSG("Unknown SlEventType " << +static_cast<uint8_t>(type));
    }
}

/**
 * @brief Aggregate per‐node PHY counters, print tables, compute balances, and write CSVs.
 * @param nodePhyStats  Map from node ID to its NodePhyStats pointer.
 */
void
ProcessPhyStats(const std::map<uint32_t, NodePhyStats*>& nodePhyStats)
{
    struct StatField
    {
        std::string name;
        std::function<uint64_t(const NodePhyStats&)> getter;
        uint64_t total = 0;
    };

    std::vector<StatField> statFields = {
        {"nTxCtrl", [](const NodePhyStats& s) { return s.m_stats.nTxCtrl; }},
        {"nTxData", [](const NodePhyStats& s) { return s.m_stats.nTxData; }},
        {"nRxCtrl", [](const NodePhyStats& s) { return s.m_stats.nRxCtrl; }},
        {"nRxData", [](const NodePhyStats& s) { return s.m_stats.nRxData; }},
        {"nRxCtrlCorr", [](const NodePhyStats& s) { return s.m_stats.nCorruptRxCtrl; }},
        {"nRxDataCorr", [](const NodePhyStats& s) { return s.m_stats.nCorruptRxData; }},
        {"nRxCtrlHd", [](const NodePhyStats& s) { return s.m_stats.nHdRxCtrl; }},
        {"nRxDataHd", [](const NodePhyStats& s) { return s.m_stats.nHdRxData; }},
        {"nRxDataNotExp", [](const NodePhyStats& s) { return s.m_stats.nDataNotExpected; }},
        {"nRxDataAlrDec", [](const NodePhyStats& s) { return s.m_stats.nDataAlreadyDecoded; }},
    };

    // --- Terminal Output ---
    std::cout << "\nPhy statistics:\n";
    std::cout << "Warning: PSSCH (Data) metrics only count transmissions with an application "
                 "packet on it, while PSCCH (Ctrl) metrics count all transmissions\n";
    std::cout << std::left << std::setw(8) << "Node ID"
              << "| " << std::setw(8) << "L2 ID"
              << "| ";
    for (const auto& f : statFields)
        std::cout << std::setw(14) << f.name << "| ";
    std::cout << "\n" << std::string(179, '-') << "\n";

    for (const auto& [nodeId, stats] : nodePhyStats)
    {
        std::cout << std::left << std::setw(8) << stats->m_nodeId << "| " << std::setw(8)
                  << stats->m_l2Id << "| ";
        for (auto& f : statFields)
        {
            uint64_t value = f.getter(*stats);
            f.total += value;
            std::cout << std::setw(14) << value << "| ";
        }
        std::cout << "\n";
    }

    std::cout << std::string(179, '-') << "\n";
    std::cout << std::left << std::setw(18) << "Total"
              << "| ";
    for (const auto& f : statFields)
        std::cout << std::setw(14) << f.total << "| ";
    std::cout << "\n" << std::string(179, '-') << "\n";

    // --- Balances ---
    auto getTotal = [&](const std::string& name) {
        for (const auto& f : statFields)
            if (f.name == name)
                return f.total;
        return uint64_t(0);
    };

    std::cout << "Balance Control: "
              << int64_t(getTotal("nTxCtrl")) - int64_t(getTotal("nRxCtrl")) -
                     int64_t(getTotal("nRxCtrlCorr")) - int64_t(getTotal("nRxCtrlHd"))
              << "\n";

    std::cout << "Balance Data: "
              << int64_t(getTotal("nTxData")) - int64_t(getTotal("nRxData")) -
                     int64_t(getTotal("nRxDataCorr")) - int64_t(getTotal("nRxDataHd")) -
                     int64_t(getTotal("nRxDataNotExp")) - int64_t(getTotal("nRxDataAlrDec"))
              << "\n";

    // --- Per Node CSV Output ---
    {
        std::ofstream file("phy-stats-per-node.csv");
        if (!file.is_open())
        {
            std::cerr << "Can't open phy-stats-per-node.csv\n";
            return;
        }

        file << "RngSeed,RngRun,NodeID,L2ID";
        for (const auto& f : statFields)
            file << "," << f.name;
        file << "\n";

        for (const auto& [nodeId, stats] : nodePhyStats)
        {
            file << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                 << stats->m_nodeId << "," << stats->m_l2Id;
            for (const auto& f : statFields)
                file << "," << f.getter(*stats);
            file << "\n";
        }
        file.close();
    }

    // --- Aggregate CSV Output ---
    {
        std::ofstream file("phy-stats-sim.csv");
        if (!file.is_open())
        {
            std::cerr << "Can't open phy-stats-sim.csv\n";
            return;
        }

        file << "RngSeed,RngRun";
        for (const auto& f : statFields)
            file << "," << f.name;
        file << "\n";

        file << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun();
        for (const auto& f : statFields)
            file << "," << f.total;
        file << "\n";
        file.close();
    }

    // --- Percentage CSV Output ---
    {
        std::ofstream file("phy-stats-sim-percent.csv");
        if (!file.is_open())
        {
            std::cerr << "Can't open phy-stats-sim-percent.csv\n";
            return;
        }

        file << "RngSeed,RngRun";
        for (const auto& f : statFields)
            file << "," << f.name;
        file << "\n";

        double txCtrl = static_cast<double>(getTotal("nTxCtrl"));
        double txData = static_cast<double>(getTotal("nTxData"));

        file << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun();

        for (const auto& f : statFields)
        {
            double refTx = 0.0;
            if (f.name.find("Ctrl") != std::string::npos)
                refTx = txCtrl;
            else if (f.name.find("Data") != std::string::npos)
                refTx = txData;

            double percent = (refTx > 0.0) ? (100.0 * f.total / refTx) : 0.0;
            file << "," << std::fixed << std::setprecision(4) << percent;
        }

        file << "\n";
        file.close();
    }
}

int
main(int argc, char* argv[])
{
    // Topology parameters
    uint16_t nUes = 5;
    uint16_t iud = 800; // meters //TODO: No impact at the moment

    // Simulation timeline parameters
    Time simTime = Seconds(90);          // Total simulation time
    Time startTrafficTime = Seconds(30); // Time to start the traffic in the application layer

    // NR SL parameters
    uint16_t numerologyBwpSl = 0;           // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl = 793e6;  // 796 MHz - band n14
    uint16_t bandwidthBandSl = 100;         // Multiple of 100 KHz; 100 = 10 MHz
    double txPower = 23;                    // dBm
    uint32_t mcs = 0;                       // The default MCS to be used by the MAC scheduler
    uint16_t t2 = 4;                        // T2 parameter defining selection window lenght
    bool sensing = true;                    // Flag to enable sensing-based resource selection
    bool wSlotEx = true;                    // Flag to enable Whole slot exclusion upon sensing
    std::string rtxType = "No";             // Retransmission scheme (No/Blind/Feedback1/Feedback2)
    uint32_t maxNumTx = 1;                  // The maximum number of transmissions in the PSSCH
    std::string propagationType = "Matrix"; // The type of propagation environment to use

    // Traffic parameters
    std::string codec = "EVS_13.2"; // The codec information to be used

    // Internal
    bool logMacPhyInDb = false; // Flag to enable database logging for MAC and PHY SL

    CommandLine cmd;

    cmd.AddValue("nUes", "The number of UEs in the linear topology", nUes);
    cmd.AddValue("iud", "The inter UE distance", iud);
    cmd.AddValue("mcs", "The MCS to be used in the SL", mcs);
    cmd.AddValue("t2", "The value of T2", t2);
    cmd.AddValue("wSlotEx",
                 "Whether to exclude all resources of the slot upon sensing a transmission in a "
                 "resource of the slot",
                 wSlotEx);
    cmd.AddValue("rtxType",
                 "The type of retransmission scheme used in the SL. Options: "
                 "No/Blind/Feedback1/Feedback2",
                 rtxType);
    cmd.AddValue("maxNumTx", "The maximum number of transmissions in the PSSCH", maxNumTx);
    cmd.AddValue("codec", "Codec name and source rate in the format 'CodecName_SourceRate'", codec);

    // Parse the command line
    cmd.Parse(argc, argv);

    g_codec_params = GetCodecParameters(codec);

    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Burn one node, so that node ID 0 is not used and Node IDs align with dstL2Ids
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    // UE nodes creation
    NodeContainer ueNodes;
    ueNodes.Create(nUes);

    // UE nodes mobility setup
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> listPositionAllocator = CreateObject<ListPositionAllocator>();

    // UE nodes deployment (Linear)
    Ptr<UniformRandomVariable> iudRnd = CreateObject<UniformRandomVariable>();
    iudRnd->SetStream(70050);
    iudRnd->SetAttribute("Min", DoubleValue(iud - 1));
    iudRnd->SetAttribute("Max", DoubleValue(iud + 1));
    double x_pos = 0;
    for (uint16_t i = 0; i < nUes; i++)
    {
        double rndIud = iudRnd->GetValue();
        x_pos = x_pos + rndIud;
        listPositionAllocator->Add(Vector(x_pos, 0, 1.5));
    }
    mobility.SetPositionAllocator(listPositionAllocator);
    mobility.Install(ueNodes);

    // Start NR SL configuration
    auto nrSlHelper = CreateObject<NrSlHelper>();

    // NR SL will use one BWP
    std::unique_ptr<BandwidthPartInfo> bwpInfo(new BandwidthPartInfo());
    bwpInfo->m_bwpId = 0;
    bwpInfo->m_centralFrequency = centralFrequencyBandSl;
    bwpInfo->m_lowerFrequency = centralFrequencyBandSl - bandwidthBandSl * 1e5 / 2;
    bwpInfo->m_higherFrequency = centralFrequencyBandSl + bandwidthBandSl * 1e5 / 2;

    // Configure channel and propagation model
    if (propagationType == "Matrix")
    {
        // Basic channel with connectivity controlled by MatrixPropagationLossModel
        auto channel = CreateObject<SingleModelSpectrumChannel>();
        Ptr<MatrixPropagationLossModel> lossModel;
        lossModel = CreateObject<MatrixPropagationLossModel>();
        lossModel->SetDefaultLoss(1000); // default loss between all nodes is high
        channel->AddPropagationLossModel(lossModel);

        // Adjacent nodes get very small random loss
        Ptr<UniformRandomVariable> lossRnd = CreateObject<UniformRandomVariable>();
        lossRnd->SetStream(70060);
        lossRnd->SetAttribute("Min", DoubleValue(-1));
        lossRnd->SetAttribute("Max", DoubleValue(1));
        for (uint16_t i = 0; i < nUes - 1; ++i)
        {
            Ptr<MobilityModel> mobA = ueNodes.Get(i)->GetObject<MobilityModel>();
            Ptr<MobilityModel> mobB = ueNodes.Get(i + 1)->GetObject<MobilityModel>();
            lossModel->SetLoss(mobA, mobB, lossRnd->GetValue());
        }
        // Static error model is suitable for this configuration
        nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
        // Configure channel in the bwp
        bwpInfo->SetChannel(channel);
    }
    else
    {
        NS_FATAL_ERROR("Currently only MatrixPropagationLossModel is supported");
    }

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
    nrSlHelper->SetUeMacAttribute("T2", UintegerValue(100));
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

    NetDeviceContainer ueNetDev = nrSlHelper->InstallUeDevice(ueNodes, allBwps);

    // Configure scheduler
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(mcs));
    nrSlHelper->SetUeSlSchedulerAttribute("WholeSlotExclusion", BooleanValue(wSlotEx));

    // Install NR configuration in the UEs
    nrSlHelper->PrepareUeForSidelink(ueNetDev, bwpIdContainer);

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
    nrSlHelper->InstallNrSlPreConfiguration(ueNetDev, slPreConfigNr);

    // Configure the IPv4 stack
    Olsrv2Helper olsrv2;
    Ipv4ListRoutingHelper list;
    list.Add(olsrv2, 10);
    InternetStackHelper internet;
    internet.SetRoutingHelper(list);
    internet.Install(ueNodes);
    NhdpHelper nhdpHelper;
    auto nhdpApps [[maybe_unused]] = nhdpHelper.Install(ueNodes);

    // Fix the random streams used for routing
    int64_t streamBase{1000};
    [[maybe_unused]] int64_t streamsUsed{0};
    streamsUsed = nrSlHelper->AssignStreams(ueNetDev, streamBase);
    streamBase = 2000;
    streamsUsed = internet.AssignStreams(ueNodes, streamBase);
    streamBase = 3000;
    streamsUsed = olsrv2.AssignStreams(ueNodes, streamBase);
    NS_LOG_DEBUG("Streams used by OLSRv2 models: " << streamsUsed);
    streamBase = 4000;
    streamsUsed = nhdpHelper.AssignStreams(ueNodes, streamBase);
    NS_LOG_DEBUG("Streams used by NHDP models: " << streamsUsed);

    Ipv4AddressHelper addrHelper;
    auto ueIpIface = addrHelper.AssignManet(ueNetDev, Ipv4Address("8.0.0.1"));

    // Obtain local IPv4 addresses and node IDs that will be used to route the unicast traffic
    Ipv4Address srcUeIpAddress, tgtUeIpAddress;
    uint32_t srcNodeId, tgtNodeId;
    srcUeIpAddress =
        ueNodes.Get(0)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal(); // SRC UE
    tgtUeIpAddress =
        ueNodes.Get(nUes - 1)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal(); // TGT UE
    srcNodeId = ueNodes.Get(0)->GetId();
    tgtNodeId = ueNodes.Get(nUes - 1)->GetId();

    // Install ProSe layer and corresponding SAPs in the UEs
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();

    // Configure discovery
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryInterval", TimeValue(Seconds(2)));
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryStart", TimeValue(Seconds(1)));

    nrSlProseHelper->PrepareUesForL3Manet(ueNetDev);

    // Traffic profile for unicast links  signaling bearers
    SidelinkInfo slSrbSlInfo;
    slSrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slSrbSlInfo.m_dynamic = true;
    slSrbSlInfo.m_pdb = MilliSeconds(100);
    slSrbSlInfo.m_priority = 1;

    nrSlProseHelper->SetL3ManetDefaultSlSrbSlInfo(ueNetDev, slSrbSlInfo);

    // Traffic profile for unicast links default data bearers
    SidelinkInfo slDrbSlInfo;
    slDrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slDrbSlInfo.m_dynamic = true;
    slDrbSlInfo.m_pdb = MilliSeconds(100);

    nrSlProseHelper->SetL3ManetDefaultSlDrbSlInfo(ueNetDev, slDrbSlInfo);

    /*
     * Configure the applications:
     * - Client app: TrafficGeneratorNgmnVoip application configured according to codec. Installed
     * in SRC UE
     * - Server app: PacketSink application to consume the received traffic. Installed in TGT UE
     */
    uint16_t appPort = 8000;
    TrafficGeneratorHelper voipTrafficGenHelper(
        "ns3::UdpSocketFactory",
        InetSocketAddress(tgtUeIpAddress, appPort), // Towards target UE and port
        TrafficGeneratorNgmnVoip::GetTypeId());

    voipTrafficGenHelper.SetAttribute(
        "EncoderFrameLength",
        UintegerValue(g_codec_params.voicePeriodicity.GetMilliSeconds()));
    voipTrafficGenHelper.SetAttribute("VoicePayload",
                                      UintegerValue(g_codec_params.voicePayloadBytes)); // Bytes
    voipTrafficGenHelper.SetAttribute("SidEnabled", BooleanValue(false));
    voipTrafficGenHelper.SetAttribute("VoiceActivityFactor", DoubleValue(0.5));
    voipTrafficGenHelper.SetAttribute("MeanTalkSpurtDuration",
                                      UintegerValue(4690)); // ms  MCPTT logs (paper)
    voipTrafficGenHelper.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));

    ApplicationContainer clientApp =
        voipTrafficGenHelper.Install(ueNodes.Get(0)); // Installed in SRC UE
    clientApp.Start(startTrafficTime);
    clientApp.Stop(simTime);
    Ptr<TrafficGenerator> trafficGenerator = clientApp.Get(0)->GetObject<TrafficGenerator>();
    trafficGenerator->Initialize();
    trafficGenerator->AssignStreams(70070);

    std::cout << "VoIp application configuration: " << std::endl;
    std::cout << " Start time: " << startTrafficTime.GetSeconds() << " s" << std::endl;
    std::cout << " Stop time: " << simTime.GetSeconds() << " s" << std::endl;
    std::cout << " Codec: " << g_codec_params.codecName << std::endl;
    std::cout << " Source Rate: " << g_codec_params.sourceRate << std::endl;
    std::cout << " Voice Periodicity: " << g_codec_params.voicePeriodicity.GetMilliSeconds()
              << " ms" << std::endl;
    std::cout << " Voice Payload: " << g_codec_params.voicePayloadBytes << " Bytes" << std::endl;

    ApplicationContainer serverApp;
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory",
                                  InetSocketAddress(Ipv4Address::GetAny(), appPort));
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    serverApp = sidelinkSink.Install(ueNodes.Get(nUes - 1)); // Installed in TGT UE
    serverApp.Start(Seconds(2.0));

    // Custom QoS rule for this application traffic
    SidelinkInfo customSlInfo;
    customSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    if (rtxType != "No")
    {
        customSlInfo.m_harqEnabled = true;
    }
    customSlInfo.m_dynamic = false; // SPS
    customSlInfo.m_rri = MilliSeconds(20);
    customSlInfo.m_pdb = MilliSeconds(t2);

    Ptr<NrSlTft> customQosRule =
        Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, tgtUeIpAddress, appPort, customSlInfo);
    nrSlProseHelper->ConfigureL3ManetQosRule(ueNetDev, customQosRule);

    // Application packets tracing
    PacketTrace pktTrace;
    std::ostringstream path;
    path << "/NodeList/" << tgtNodeId << "/ApplicationList/1/$ns3::PacketSink/RxWithSeqTsSize";
    Config::ConnectWithoutContext(path.str(), MakeBoundCallback(&ReceivePacketSeqTs, &pktTrace));
    path.str("");

    path << "/NodeList/" << srcNodeId
         << "/ApplicationList/1/$ns3::TrafficGeneratorNgmnVoip/TxWithSeqTsSize";
    Config::ConnectWithoutContext(path.str(), MakeBoundCallback(&TransmitPacketSeqTs, &pktTrace));
    path.str("");

    // Route change tracing
    std::ofstream* fileRouteChange =
        new std::ofstream("route-change-trace.csv", std::ios_base::out);
    if (!fileRouteChange->is_open())
    {
        std::cerr << "Error: Could not open route-change-trace.csv\n";
        return 1;
    }
    *fileRouteChange << "Time(s),src(NodeId),tgt(NodeId),route(NodeIds)\n";
    *fileRouteChange << "0," << srcNodeId << "," << tgtNodeId << ","
                     << "NaN"
                     << "\n";

    Simulator::Schedule(Seconds(1.0),
                        &LookupRoute,
                        ueNodes,
                        srcNodeId,
                        tgtNodeId,
                        std::vector<uint32_t>(),
                        Seconds(0.01),
                        fileRouteChange);

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
    std::map<uint32_t, NodePhyStats*> nodePhyStats;
    for (uint32_t i = 0; i < ueNetDev.GetN(); ++i)
    {
        uint32_t nodeId = ueNetDev.Get(i)->GetNode()->GetId();
        NodePhyStats* stats = new NodePhyStats(); // Create a new NodePhyStats object
        stats->m_nodeId = nodeId;

        Ptr<NrSlUeRrc> rrc =
            DynamicCast<NrSlUeRrc>(ueNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetRrc());
        stats->m_l2Id = rrc->GetSourceL2Id();
        nodePhyStats[nodeId] = stats;

        std::ostringstream ossSlEvent;
        ossSlEvent
            << "/NodeList/" << nodeId
            << "/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/$ns3::BandwidthPartUe/"
               "NrUePhy/$ns3::NrUePhy/NrSpectrumPhy/$ns3::NrSpectrumPhy/SlChannelsEvent";
        Config::ConnectWithoutContext(ossSlEvent.str(),
                                      MakeBoundCallback(&TraceSlChannelsEvent, stats));
    }

    Simulator::Stop(simTime + TimeStep(1));

    Simulator::Run();

    // Process and save VoIP packet trace
    ProcessPacketTrace(pktTrace, g_codec_params);

    // Process and save PHY statistics
    ProcessPhyStats(nodePhyStats);

    // Process and save route changes during traffic
    fileRouteChange->close();
    ProcessRouteChanges(startTrafficTime, simTime);

    // Write on the database if enabled
    if (logMacPhyInDb)
    {
        pscchStats.EmptyCache();
        psschStats.EmptyCache();
        pscchPhyStats.EmptyCache();
        psschPhyStats.EmptyCache();
    }

    Simulator::Destroy();
    return 0;
}
