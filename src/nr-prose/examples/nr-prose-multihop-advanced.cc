//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file nr-prose-multihop-advanced.cc
 * @brief Example of ProSe multi-hop U2U relay with OLSRv2/NHDP in a grid UE
 *        topology with VoIP traffic
 *
 * System configuration:
 * A single sidelink band (n14, 793 MHz) with 10 MHz bandwidth and numerology 0.
 * UEs use the default scheduler with configurable default MCS (parameter 'mcs')
 * and sensing‐based scheduling with activation of the whole‐slot exclusion with
 * the parameter 'wSlotEx'. The length of the selection window used for application traffic
 * can be configured by tuning the parameter 'appPerHopPdb'.
 * The retransmission scheme and the maximum number of transmissions for application traffic
 * can be selected using the parameters 'rtxType' and 'maxNumTx' respectively.
 *
 * Topology:
 * The UEs are configured in a 2D grid and at 1.5 m height. Three grid sizes are currently
 * supported, controlled by the paramter 'gridSize' corresponding to the total number of UEs
 * in the topology (4: 2x2, 9: 3x3, 16: 4x4).
 * The X/Y distance between the nodes is configured by the parameter 'd'.
 * This is a representation of gridSize = 4:
 *
 *              UE13..................UE14..................UE15..................UE16
 *       (0, 3d, 1.5)          (d, 3d, 1.5)          (2d, 3d, 1.5)          (3d, 3d, 1.5)
 *            |                     |                     |                     |
 *            |                     |                     |                     |
 *            UE9...................UE10..................UE11..................UE12
 *       (0, 2d, 1.5)          (d, 2d, 1.5)          (2d, 2d, 1.5)          (3d, 2d, 1.5)
 *            |                     |                     |                     |
 *            |                     |                     |                     |
 *            UE5...................UE6...................UE7...................UE8
 *       (0, d, 1.5)           (d, d, 1.5)           (2d, d, 1.5)           (3d, d, 1.5)
 *            |                     |                     |                     |
 *            |                     |                     |                     |
 *            UE1...................UE2...................UE3...................UE4
 *       (0, 0, 1.5)           (d, 0, 1.5)           (2d, 0, 1.5)           (3d, 0, 1.5)
 *
 * Channel configuration:
 * Currently four proppagation loss models can be selected with the parameter 'lossModel':
 * - matrix: low loss for links along grid lines between adjacent nodes, high loss for the
 *           diagonals.
 * - friis: loss between nodes is derived from Friis equation.
 * - log-distance: log-distance model adapted for outdoor-to-outdoor scenario (exponent 2) and
 *                              for the frequency we are using (See 3GPP TR 36.843 Section A.2.1.2,
 *                              WINNER II Channel Models, D1.1.2 V1.2., equation (4.24) p.43,
 *                              available at:
 *                 http://www.cept.org/files/1050/documents/winner2%20-%20final%20report.pdf).
 * - umi: Urban Microcell (UMi) LOS model from WINNER+ B1 model, also used in NIST LLS. It is
 *        represented by the ns-3 LogDistance model by setting the exponent to 4 and adapting the
 *        reference loss to the other terms.
 * Currently two error model types are supported and can be selected by the parameter 'lossModel':
 * - static: AWGN static channel error model.
 * - epa: Extended Pedestrian A (EPA) error model.
 *
 *
 * Traffic:
 * Unicast VoIP applications are configured to send traffic from a source UE to a target UE.
 * Traffic is generated acording to the EVS codec and the source rate can be selected with the
 * parameter 'codec'.
 * The number of VoIP applications in the topology can be selected with the parameter 'nApps',
 * and the source and target UEs will be selected to be on the outtermost positions and in
 * opposite sides of the grid topology.
 * Due to the limited number of nodes in the supported topologies, only source and target UE
 * pairs {source UE Node ID, target UE Node ID} in the following sets can be configured:
 * - gridSize = 2, max nApps = 2, set = {{1, 4}, {2, 3}}
 * - gridSize = 3, max nApps = 4, set = {{1, 9}, {3, 7}, {4, 6}, {2, 8}}
 * - gridSize = 4, max nApps = 6, set = {{1, 16}, {4, 13}, {2, 15}, {5, 12}, {14, 3}, {8, 9}}
 *
 * Output:
 * - direct-links-trace.csv: Timeline of the establishment and/or releases of direct link between
 *   pairs of nodes in the scenario.
 * - node-position-trace.csv: Timeline of the node positions over time. Currently only the initial
 *   node position is logged, as there is no mobility in the scenario.
 * - route-change-trace-{appId}.csv: IP route changes timeline for the application with ID 'appId'.
 * - route-change-stats-per-app.csv: IP route changes statistics including the number of route
 *   changes and time without route during traffic activity for all applications.
 * - route-change-stats.csv: Average route change statistics considering all applications
 * - voip-packet-trace-{appId}.csv: VoIP application packets timeline with transmission and
 *   reception timestamps for the application with ID 'appId'.
 * - voip-stats-sim-per-app.csv: VoIP statistics for the simulation including total transmitted
 *   and received packets, packet loss ratio, average packet delay and Mean Opinion Score (MOS)
 *   for all the applications.
 * - voip-stats-sim.csv: Average VoIP statics considering all applications, as well as the number
 *   of applications in outage for both MOS < 4.0, and 98% tail delay > 200 ms.
 * - phy-stats-per-node-{messageType}.csv: Count of PSSCH transmissions and reception outcomes at
 *   the PHY for each node and for the type of message 'messageType'.
 * - phy-stats-sim-total.csv: Sum of phy-stats-per-node.csv for all nodes in the simulation for each
 *   message type.
 * - phy-stats-sim-percent.csv: Same as phy-stats-sim-total.csv but as percentage of the number of
 *   transmissions.
 * - mac-phy-trace.db: Database with NR SL MAC and PHY traces (only written if logMacPhyInDb =
 *   true).


 *
 * \code{.unparsed}
$ ./ns3 run "nr-prose-multihop-advanced -- --PrintHelp"
    \endcode
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nhdp-hello-tag.h"
#include "ns3/nhdp-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/nr-sl-prose-tag.h"
#include "ns3/olsrv2-module.h"
#include "ns3/olsrv2-tag.h"
#include "ns3/point-to-point-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("NrProseMultihopAdvanced");

/**
 * @brief A function for tracing RLF events
 *
 * @param stream The output stream to write to
 * @param srcL2Id The source L2 ID that the event is associated with
 * @param dstL2Id The destination L2 ID that the event is associated with
 * @param even The indicated event
 */
void
RlfTrace(Ptr<OutputStreamWrapper> stream,
         uint32_t srcL2Id,
         uint32_t dstL2Id,
         NrSlL3ManetService::RlfEvent event)
{
    std::string eventName;
    *stream->GetStream() << Simulator::Now().GetSeconds() << "," << srcL2Id << "," << dstL2Id;
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
    *stream->GetStream() << "," << eventName << std::endl;
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
            uint16_t appId,
            uint32_t srcNodeId,
            uint32_t tgtNodeId,
            std::vector<uint32_t> prevPath,
            bool loopOngoing,
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
            if (!loopOngoing)
            {
                *fileRouteChange << now.GetSeconds() << "," << appId << "," << srcNodeId << ","
                                 << tgtNodeId << ",loop\n";
                loopOngoing = true;
            }
        }
        else if (!routeFound)
        {
            if (!prevPath.empty())
            {
                *fileRouteChange << now.GetSeconds() << "," << appId << "," << srcNodeId << ","
                                 << tgtNodeId << ",NaN\n";
            }
            path.clear(); // suppress further repeats
            loopOngoing = false;
        }
        else if (path != prevPath)
        {
            *fileRouteChange << now.GetSeconds() << "," << appId << "," << srcNodeId << ","
                             << tgtNodeId << ",";
            for (size_t i = 0; i < path.size(); ++i)
            {
                *fileRouteChange << path[i];
                if (i != path.size() - 1)
                    *fileRouteChange << "->";
            }
            *fileRouteChange << "\n";
            loopOngoing = false;
        }
    }

    if (fileRouteChange)
        fileRouteChange->flush();

    Simulator::Schedule(interval,
                        &LookupRoute,
                        ueNodes,
                        appId,
                        srcNodeId,
                        tgtNodeId,
                        path,
                        loopOngoing,
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
ProcessRouteChanges(Time startTime, Time endTime, uint32_t gridSize)
{
    std::ifstream file("route-change-trace.csv");
    if (!file.is_open())
    {
        std::cerr << "Error opening route-change-trace.csv" << std::endl;
        return;
    }

    std::map<std::tuple<uint16_t, uint32_t, uint32_t>, std::string> lastKnownPathBeforeWindow;
    std::map<std::tuple<uint16_t, uint32_t, uint32_t>, uint16_t> routeChangeCount;
    std::map<std::tuple<uint16_t, uint32_t, uint32_t>, double> nanTimeAccum;
    std::map<std::tuple<uint16_t, uint32_t, uint32_t>, double> lastNanStart;
    std::map<std::tuple<uint16_t, uint32_t, uint32_t>, std::string> lastPath;
    std::set<std::tuple<uint16_t, uint32_t, uint32_t>> allTripletsSeen;

    std::map<uint16_t, std::ofstream> appFiles;

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(iss, token, ','))
            tokens.push_back(token);

        if (tokens.size() < 5)
        {
            std::cerr << "Invalid line: " << line << std::endl;
            continue;
        }

        double time = std::stod(tokens[0]);
        uint16_t appId = static_cast<uint16_t>(std::stoi(tokens[1]));
        uint32_t src = std::stoi(tokens[2]);
        uint32_t dst = std::stoi(tokens[3]);
        std::string path = tokens[4];
        auto key = std::make_tuple(appId, src, dst);
        allTripletsSeen.insert(key);

        // Split output file per appId
        if (appFiles.find(appId) == appFiles.end())
        {
            std::string outFile = "route-change-trace-" + std::to_string(appId) + ".csv";
            appFiles[appId].open(outFile);
            if (!appFiles[appId].is_open())
            {
                std::cerr << "Error opening output file: " << outFile << std::endl;
                continue;
            }
            appFiles[appId] << "#Time(s),appId,srcNodeId,tgtNodeId,route(NodeIds)\n";
            appFiles[appId] << "#Grid size: " << gridSize << "\n";
            appFiles[appId] << "#Start traffic: " << startTime.GetSeconds() << "\n";
            appFiles[appId] << "#Stop traffic: " << endTime.GetSeconds() << "\n";
        }
        appFiles[appId] << line << "\n";

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
            if (lastKnownPathBeforeWindow[key] == "NaN" || lastKnownPathBeforeWindow[key] == "loop")
            {
                lastNanStart[key] = startTime.GetSeconds();
            }
        }

        if (path != lastPath[key])
        {
            routeChangeCount[key]++;
            lastPath[key] = path;
        }

        if (path == "NaN" || path == "loop")
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

    // For entries only seen before the window
    for (const auto& kv : lastKnownPathBeforeWindow)
    {
        const auto& key = kv.first;
        const std::string& pathBefore = kv.second;

        if (!lastPath.count(key)) // not seen during window
        {
            lastPath[key] = pathBefore;
            if (pathBefore == "NaN" || pathBefore == "loop")
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

    // Output summary
    double sim_sumChanges = 0;
    double sim_sumNanTime = 0;
    uint16_t sim_nApps = allTripletsSeen.size();
    std::ofstream csv("route-change-stats-per-app.csv");
    csv << "#RngSeed,RngRun,AppId,SrcNode,DstNode,RouteChanges,TimeWithoutRoute\n";

    std::cout << "\nRoute Change Summary (for the simulation period with traffic ["
              << startTime.GetSeconds() << "s, " << endTime.GetSeconds() << "s]):\n";

    std::cout << std::left << std::setw(8) << "AppId"
              << "| " << std::setw(10) << "SrcNode"
              << "| " << std::setw(10) << "DstNode"
              << "| " << std::setw(15) << "RouteChanges"
              << "| " << std::setw(20) << "TimeWithoutRoute(s)"
              << "|\n";
    std::cout << std::string(72, '-') << "\n";

    for (const auto& key : allTripletsSeen)
    {
        uint16_t appId = std::get<0>(key);
        uint32_t src = std::get<1>(key);
        uint32_t dst = std::get<2>(key);

        uint16_t changes = routeChangeCount[key];
        sim_sumChanges += changes;
        double nanTime = nanTimeAccum[key];
        sim_sumNanTime += nanTime;
        std::cout << std::left << std::setw(8) << appId << "| " << std::setw(10) << src << "| "
                  << std::setw(10) << dst << "| " << std::setw(15) << changes << "| "
                  << std::setw(20) << std::fixed << std::setprecision(4) << nanTime << "|\n";

        csv << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << "," << appId << ","
            << src << "," << dst << "," << changes << "," << nanTime << "\n";
    }

    std::cout << std::string(72, '-') << "\n";
    csv.close();
    std::ofstream csv_sim("route-change-stats.csv");
    csv_sim << "#RngSeed,RngRun,meanRouteChanges,meanTimeWithoutRoute\n";
    csv_sim << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
            << sim_sumChanges / sim_nApps << "," << sim_sumNanTime / sim_nApps << "\n";
    csv_sim.close();

    for (auto& kv : appFiles)
    {
        kv.second.close();
    }
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
CodecParameters g_codecParams;

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

struct AppInfo
{
    uint32_t appId;
    uint32_t srcNodeId;
    uint32_t tgtNodeId;
    Ipv4Address srcUeIpAddress;
    Ipv4Address tgtUeIpAddress;
    CodecParameters codecParams;
    PacketTrace pktTrace;
};

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

    NrSlProseAppTag appTag;
    packet->AddByteTag(appTag);
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
ProcessPacketTrace(const std::list<AppInfo>& appInfoList)
{
    double sim_sumLossRatio = 0.0;
    double sim_sumAvgDelay = 0.0;
    double sim_sumMos = 0.0;
    double sim_sumTail98Delay = 0.0;
    uint16_t sim_nApps = appInfoList.size();
    uint16_t sim_nAppOutageMos = 0;
    uint16_t sim_nAppOutageTail98Delay = 0;

    // Process all apps
    std::ofstream fileStats("voip-stats-sim-per-app.csv", std::ios_base::out);
    if (!fileStats.is_open())
    {
        std::cerr << "Error: Could not open voip-stats-sim-per-app.csv\n";
        return;
    }
    fileStats << "#RngSeed,RngRun,appId,srcNodeId,tgtNodeId,"
                 "nTxPkts,nRxPkts,LossRatio,AvgDelay,Mos,98TailDelay(ms)\n";

    // Console header
    std::cout << "\nVoIP Statistics:\n";
    std::cout << std::left << std::setw(10) << "RngSeed"
              << "| " << std::setw(8) << "RngRun"
              << "| " << std::setw(8) << "AppId"
              << "| " << std::setw(10) << "SrcNode"
              << "| " << std::setw(10) << "TgtNode"
              << "| " << std::setw(10) << "nTxPkts"
              << "| " << std::setw(10) << "nRxPkts"
              << "| " << std::setw(10) << "LossRatio"
              << "| " << std::setw(14) << "AvgDelay(ms)"
              << "| " << std::setw(10) << "MOS"
              << "| " << std::setw(16) << "98TailDelay(ms)"
              << "|\n";
    std::cout << std::string(138, '-') << "\n";

    for (const auto& app : appInfoList)
    {
        const PacketTrace& pktTrace = app.pktTrace;
        uint32_t nTx = pktTrace.size();
        uint32_t nRx = 0;
        double totalDelayMs = 0.0;
        std::vector<double> sampleDelays;

        // Save per-app packet trace
        std::string traceName = "voip-packet-trace-" + std::to_string(app.appId) + ".csv";
        std::ofstream fileTrace(traceName);
        if (!fileTrace.is_open())
        {
            std::cerr << "Error: Could not open " << traceName << "\n";
            continue;
        }
        fileTrace << "#TxTime(s),RxTime(s),Seq,Size,RxDelay(ms)\n";

        for (const auto& [seq, info] : pktTrace)
        {
            fileTrace << std::fixed << std::setprecision(6) << info.txTime.GetSeconds() << ","
                      << info.rxTime.GetSeconds() << "," << seq << "," << info.size << ",";

            if (info.rxTime != Seconds(0))
            {
                nRx++;
                totalDelayMs += info.rxDelay.GetMilliSeconds();
                sampleDelays.push_back(info.rxDelay.GetMilliSeconds());
                fileTrace << info.rxDelay.GetMilliSeconds() << "\n";
            }
            else
            {
                sampleDelays.push_back(std::numeric_limits<double>::infinity());
                fileTrace << "NaN\n";
            }
        }
        fileTrace.close();

        // Compute stats
        double lossRatio = (double)(nTx - nRx) / nTx;
        double avgDelay = 0.0;
        double mos = 0.0;
        double tail98Delay = 0.0;
        std::string avgDelayStr, mosStr, tail98DelayStr;

        if (nTx == 0)
        {
            avgDelayStr = "NaN";
            mosStr = "NaN";
            tail98DelayStr = "NaN";
            sim_nAppOutageMos++;
            sim_nAppOutageTail98Delay++;
            sim_sumLossRatio += lossRatio;
        }
        else if (lossRatio == 1.0)
        {
            avgDelayStr = "NaN";
            mosStr = "NaN";
            tail98DelayStr = "Inf";
            sim_nAppOutageMos++;
            sim_nAppOutageTail98Delay++;
            sim_sumLossRatio += lossRatio;
        }
        else
        {
            avgDelay = totalDelayMs / nRx;
            avgDelayStr = std::to_string(avgDelay);

            double rFactor = CalculateRFactor(avgDelay, lossRatio, app.codecParams);
            mos = CalculateMOS(rFactor, app.codecParams);
            mosStr = std::to_string(mos);
            if (mos < 4.0)
            {
                sim_nAppOutageMos++;
            }

            std::sort(sampleDelays.begin(), sampleDelays.end());
            uint16_t rank = std::floor(0.98 * (double)sampleDelays.size());
            tail98Delay = sampleDelays[rank - 1];
            if (!std::isfinite(tail98Delay))
            {
                tail98DelayStr = "Inf";
                sim_nAppOutageTail98Delay++;
            }
            else
            {
                tail98DelayStr = std::to_string(tail98Delay);
                if (tail98Delay > 200)
                {
                    sim_nAppOutageTail98Delay++;
                }
            }

            sim_sumLossRatio += lossRatio;
            sim_sumAvgDelay += avgDelay;
            sim_sumMos += mos;
            sim_sumTail98Delay += tail98Delay;
        }

        // Append to stats file
        fileStats << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                  << app.appId << "," << app.srcNodeId << "," << app.tgtNodeId << "," << nTx << ","
                  << nRx << "," << std::fixed << std::setprecision(4) << lossRatio << ","
                  << avgDelayStr << "," << mosStr << "," << tail98DelayStr << "\n";

        // Console row
        std::cout << std::left << std::setw(10) << RngSeedManager::GetSeed() << "| " << std::setw(8)
                  << RngSeedManager::GetRun() << "| " << std::setw(8) << app.appId << "| "
                  << std::setw(10) << app.srcNodeId << "| " << std::setw(10) << app.tgtNodeId
                  << "| " << std::setw(10) << nTx << "| " << std::setw(10) << nRx << "| "
                  << std::setw(10) << std::fixed << std::setprecision(4) << lossRatio << "| "
                  << std::setw(14) << avgDelayStr << "| " << std::setw(10) << mosStr << "| "
                  << std::setw(16) << tail98DelayStr << "|\n";
    }
    std::cout << std::string(137, '-') << "\n";
    fileStats.close();

    // Save average of simulations
    std::ofstream fileSimStats("voip-stats-sim.csv", std::ios_base::out);
    if (!fileSimStats.is_open())
    {
        std::cerr << "Error: Could not open voip-stats-sim.csv\n";
        return;
    }
    fileSimStats
        << "#RngSeed,RngRun,meanLossRatio,meanAvgDelay,meanMos,mean98TailDelay(ms),nAppOutageMos,"
           "nAppOutage98TailDelay\n";
    fileSimStats << RngSeedManager::GetSeed() << "," << RngSeedManager::GetRun() << ","
                 << sim_sumLossRatio / sim_nApps << "," << sim_sumAvgDelay / sim_nApps << ","
                 << sim_sumMos / sim_nApps << "," << sim_sumTail98Delay / sim_nApps << ","
                 << sim_nAppOutageMos << "," << sim_nAppOutageTail98Delay << "\n";

    std::cout << std::right << std::setw(80) << " Mean | " << std::left << std::setw(10)
              << std::fixed << std::setprecision(4) << sim_sumLossRatio / sim_nApps << "| "
              << std::setw(14) << sim_sumAvgDelay / sim_nApps << "| " << std::setw(10)
              << sim_sumMos / sim_nApps << "| " << std::setw(16) << sim_sumTail98Delay / sim_nApps
              << "|\n";
    std::cout << std::string(137, '-') << "\n";

    std::cout << "Number of Apps in Outage (MOS): " << sim_nAppOutageMos << "\n"
              << "Number of Apps in Outage (98TailDelay): " << sim_nAppOutageTail98Delay << "\n";
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
    PhyStats m_appStats;
    PhyStats m_pc5sStats;
    PhyStats m_nhdpStats;
    PhyStats m_olsrStats;
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

    // Classify burst and obtain the corresponding PhyStats bucket to update
    ns3::nhdp::HelloTag nhdpHelloTag;
    ns3::olsrv2::MessageTag olsrTag;
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
        if (!bucket && pkt->FindFirstMatchingByteTag(nhdpHelloTag))
        {
            bucket = &stats->m_nhdpStats;
            break;
        }
        if (!bucket && pkt->FindFirstMatchingByteTag(olsrTag))
        {
            bucket = &stats->m_olsrStats;
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
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_OK:
        bucket->nRxCtrl++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_DECODE_FAILURE:
        bucket->nCorruptRxCtrl++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSCCH_RX_HALF_DUPLEX:
        bucket->nHdRxCtrl++;
        break;

    case NrSlSpectrumPhy::SlEventType::PSSCH_TX:
        bucket->nTxData++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_OK:
        bucket->nRxData++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_SCI2:
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_DECODE_FAILURE_TB:
        bucket->nCorruptRxData++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_HALF_DUPLEX:
        bucket->nHdRxData++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_NOT_EXPECTED:
        bucket->nDataNotExpected++;
        break;
    case NrSlSpectrumPhy::SlEventType::PSSCH_RX_ALREADY_DECODED:
        bucket->nDataAlreadyDecoded++;
        break;

    default:
        NS_ABORT_MSG("Unknown SlEventType " << +static_cast<uint8_t>(type));
    }
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
        {"nhdp",
         [](const NodePhyStats& s) -> const NodePhyStats::PhyStats& { return s.m_nhdpStats; }},
        {"olsr",
         [](const NodePhyStats& s) -> const NodePhyStats::PhyStats& { return s.m_olsrStats; }},
    };

    // Build StatField vector for a given bucket.
    auto makeDataFields = [&](const Bucket& b) {
        return std::vector<StatField>{
            {"nTxData", [&, b](const NodePhyStats& s) { return b.accessor(s).nTxData; }},
            {"nRxData", [&, b](const NodePhyStats& s) { return b.accessor(s).nRxData; }},
            {"nRxDataCorr", [&, b](const NodePhyStats& s) { return b.accessor(s).nCorruptRxData; }},
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
    simTotals << "#RngSeed,RngRun,Type,nTxData,nRxData,nRxDataCorr,nRxDataHd,"
                 "nRxDataNotExp,nRxDataAlrDec\n";

    std::ofstream simPercent("phy-stats-sim-percent.csv");
    if (!simPercent.is_open())
    {
        std::cerr << "Can't open phy-stats-sim-percent.csv\n";
        return;
    }
    simPercent << "#RngSeed,RngRun,Type,nTxData,nRxData,nRxDataCorr,nRxDataHd,"
                  "nRxDataNotExp,nRxDataAlrDec\n";

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

            std::cout << std::left << std::setw(8) << s.m_nodeId << "| " << std::setw(8) << s.m_l2Id
                      << "| ";

            for (auto& f : statFields)
            {
                uint64_t value = f.getter(s);
                f.total += value;
                std::cout << std::setw(14) << value << "| ";
            }
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

/**
 * @brief Trace sink for the L1 SD-RSRP measurements report.
 * @param stream        Pointer to the stream wrapper object
 * @param selfL2Id      The L2 Id of the measuring node
 * @param rnti          The RNTI
 * @param peerL2Id      The L2 Id of the measured node
 * @param l1sdRsrp      The L1 SD-RSRP measurement
 */
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

/**
 * @brief Trace sink for the L3 SD-RSRP measurements report.
 * @param stream        Pointer to the stream wrapper object
 * @param selfL2Id      The L2 Id of the measuring node
 * @param peerL2Id      The L2 Id of the measured node
 * @param l3sdRrsrp     The L3 SD-RSRP measurement
 * @param thCond        Threshold condition flag
 */
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

/**
 * @brief Trace sink for the link establishment notificiation.
 * @param stream        Pointer to the stream wrapper object
 * @param context       Node ID
 * @param selfL2Id      The L2 Id of the establishing node
 * @param selfIpv4Addr  The IPv4 address of the establishing node
 * @param peerL2Id      The L2 Id of the peer node
 * @param peerIpv4Addr  The IPv4 address of the peer node
 */
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

/**
 * @brief Trace sink for the link release notificiation.
 * @param stream        Pointer to the stream wrapper object
 * @param context       Node ID
 * @param selfL2Id      The L2 Id of the releasing node
 * @param selfIpv4Addr  The IPv4 address of the releasing node
 * @param peerL2Id      The L2 Id of the peer node
 * @param peerIpv4Addr  The IPv4 address of the peer node
 */
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
DisableLoss(Ptr<MatrixPropagationLossModel> lossModel, Ptr<Node> a, Ptr<Node> b)
{
    lossModel->SetLoss(a->GetObject<MobilityModel>(), b->GetObject<MobilityModel>(), 0);
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

    NS_ABORT_MSG_UNLESS(distance >= 100, "Distance must be 100m or greater");
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
        // By default, the MatrixPropagationLossModel disables all pairwise
        // communication.  Selectively enable the edges (not diagonals) on
        // the grid
        const uint32_t w = static_cast<uint32_t>(std::sqrt(gridSize));
        for (uint32_t i = 0; i < w; ++i)
        {
            for (uint32_t j = 0; j < w; ++j)
            {
                const uint32_t idx = i * w + j;

                // Right neighbor (no diagonals)
                if (j + 1 < w)
                {
                    DisableLoss(matrixLossModel, n.Get(idx), n.Get(idx + 1));
                }

                // Down neighbor (no diagonals)
                if (i + 1 < w)
                {
                    DisableLoss(matrixLossModel, n.Get(idx), n.Get(idx + w));
                }
            }
        }
    }
    return n;
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
 * @brief Trace sink for RLC PDU transmissions that updates RLC statistics.
 * @param stats  Per-bearer RLC statistics object
 * @param rnti   UE RNTI
 * @param lcId   Logical channel ID
 * @param pktSize Size in bytes
 */
void
TraceRlcTxPdu(Ptr<RlcStats> stats, uint16_t /*rnti*/, uint8_t /*lcId*/, uint32_t /*pktSize*/)
{
    stats->m_nTxPdu++;
}

/**
 * @brief Trace sink for RLC PDU transmissions drops that updates RLC statistics.
 * @param rlcDropsTrace Output stream wrapper used to log drop events
 * @param stats         Per-bearer RLC statistics object
 * @param p             Packet instance associated with the drop event.
 */
void
TraceRlcTxDrop(Ptr<OutputStreamWrapper> rlcDropsTrace, Ptr<RlcStats> stats, Ptr<const Packet> p)
{
    stats->m_nDroppedPdu++;

    std::ostream* os = rlcDropsTrace->GetStream();
    (*os) << Simulator::Now().GetSeconds() << "," << stats->m_nodeId << "," << stats->m_dstL2Id
          << "," << +stats->m_lcId << "," << p->GetSize() << "\n";
}

/**
 * @brief Trace sink invoked when a sidelink logical channel is added.
 * @param rlcDropsTrace Output stream wrapper used to log drop events to CSV
 * @param nodeRlcStats  Per-node RLC statistics container
 * @param params        Logical channel information reported by the scheduler trace.
 */
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

/**
 * @brief Post-process and export aggregated RLC PDU statistics.
 * @param nodesRlcStats Map from nodeId to per-node RLC statistics container.
 */
void
ProcessRlcStats(const std::map<uint32_t, Ptr<NodeRlcStats>>& nodesRlcStats)
{
    // Standard output column widths
    const int wNode = 8;
    const int wDst = 8;
    const int wLc = 6;
    const int wTx = 12;
    const int wDrop = 14;
    const int totalWidth = 2 + wNode + 3 + wDst + 3 + wLc + 3 + wTx + 3 + wDrop + 2;

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

int
main(int argc, char* argv[])
{
    // Topology parameters
    uint32_t gridSize(16); // 4, 9, or 16
    double d(500);         // meters

    // Simulation timeline parameters
    Time simTime(Seconds(180));         // Total simulation time
    Time startTrafficTime(Seconds(60)); // Time to start the traffic in the application layer

    // NR SL parameters
    uint16_t numerologyBwpSl(0);             // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6);    // band n14 (793 MHz)
    uint16_t bandwidthBandSl(100);           // Multiple of 100 KHz; 100 = 10 MHz
    double txPower(23);                      // Units of dBm
    uint32_t mcs(0);                         // The default MCS to be used by the MAC scheduler
    bool sensing(true);                      // Flag to enable sensing-based resource selection
    bool wSlotEx(true);                      // Flag to enable Whole slot exclusion upon sensing
    std::string rtxType("No");               // Retransmission scheme (No/Blind/Feedback1/Feedback2)
    uint32_t maxNumTx(1);                    // The maximum number of transmissions in the PSSCH
    std::string lossModelType("matrix");     // propagation loss model
    std::string errorModelType("static");    // error model type (static or epa)
    std::string mcsControllerType("static"); // MCS controller type (static or ideal)

    // Traffic parameters
    std::string codec("EVS_13.2"); // The codec information to be used
    uint16_t nApps(1);             // The number of VoIP applications
    uint16_t appPerHopPdb(16);     // PDB defining selection window length for the VoIP traffic (ms)

    // Internal
    bool logMacPhyInDb(false); // Flag to enable database logging for MAC and PHY SL
    bool log(false);           // Flag to enable logging
    std::string idealSchedLevel(
        "Global"); // The level of knowledge from external nodes used by the SL scheduler when
                   // filtering candidate resources (None/Global/LocalUnicast)
    bool disableRlcPduDiscard(false);  // Flag to disable the RLC PDB enforcement
    bool routingBypass(false);         // Flag to enable the NHDP and OLSR bypass mode
    bool writeSchedulingTraces(false); // Flag to enable writing detailed sensing/scheduling traces

    // Discovery and SD-RSRP meassurement
    double rsrpThreshold(-111);      // The RSRP threshold (dBm) when enabling SD-RSRP/SL-RSRP
    double rsrpCoefficient(0);       // The L3 filter coefficient when enabling SD-RSRP/SL-RSRP
    double rsrpHysteresis(0);        // The hysteresis when enabling SD-RSRP/SL-RSRP
    Time discInterval(Seconds(2.0)); // The periodicity of the discovery messages

    CommandLine cmd;

    cmd.AddValue("simTime", "Simulation time, in seconds", simTime);
    cmd.AddValue("gridSize", "Size of grid (4, 9, or 16 nodes)", gridSize);
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
    cmd.AddValue("codec", "Codec name and source rate in the format 'CodecName_SourceRate'", codec);
    cmd.AddValue("nApps", "The number of applications generating traffic", nApps);
    cmd.AddValue("idealSchedLevel",
                 "The level of knowledge from external nodes used by the SL scheduler when "
                 "filtering candidate resources. Options: None/Global/LocalUnicast",
                 idealSchedLevel);
    cmd.AddValue("routingBypass", "Whether to enable the NHDP and OLSR bypass mode", routingBypass);
    cmd.AddValue("rsrpThreshold", "The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold", rsrpThreshold);
    cmd.AddValue("rsrpCoefficient", "The SD-RSRP and/or SL-RSRP coefficent", rsrpCoefficient);
    cmd.AddValue("rsrpHysteresis", "The SD-RSRP and/or SL-RSRP (dB) hysteresis", rsrpHysteresis);
    cmd.AddValue("writeSchedulingTraces",
                 "Whether to write scheduling and sensing traces",
                 writeSchedulingTraces);
    cmd.AddValue("discInterval", "The periodicity of the discovery messages", discInterval);

    // Configuration defaults that could be overridden at command line
    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));
    Config::SetDefault("ns3::NrSlRlcUm::DiscardTimerScale", DoubleValue(4.0));
    Config::SetDefault("ns3::NrSlUeMacSchedulerDefault::MinimumSpsGrantSize", UintegerValue(80));
    Config::SetDefault("ns3::NrSlUeMacSchedulerDefault::SpsReselectionThreshold",
                       UintegerValue(240));

    // Parse the command line
    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(simTime <= startTrafficTime,
                    "Configuration error, simTime must be greater than startTime");
    // Configure discovery start
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryStart", TimeValue(Seconds(1.0)));
    // Configure discovery interval and L1 SD-RSRP measurement interval to four discovery intervals
    // (i.e., T_measure = 4 in TS 38.133 Section 12.10, table 12.10.2-1)
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryInterval", TimeValue(discInterval));
    Config::SetDefault("ns3::NrSlUePhy::RsrpFilterPeriod", TimeValue(4 * discInterval));

    if (log)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
        LogComponentEnable("NrSlUeProseDirectLink", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlL3ManetService", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeService", LOG_LEVEL_ALL);
        LogComponentEnable("NrProseMultihopAdvanced", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlEpcUeNas", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeRrc", LOG_LEVEL_ALL);
    }

    g_codecParams = GetCodecParameters(codec);

    if (disableRlcPduDiscard)
    {
        Config::SetDefault("ns3::NrSlRlcUm::EnablePdcpDiscarding", BooleanValue(false));
    }
    if (routingBypass)
    {
        Config::SetDefault("ns3::nhdp::NhdpClient::BypassMode", BooleanValue(true));
        Config::SetDefault("ns3::olsrv2::RoutingProtocol::BypassMode", BooleanValue(true));
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
    NodeContainer manetNodes = CreateGrid(gridSize, d, lossModel);

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
        nrSlHelper->SetMcsControllerTypeId(NrSlMcsController::GetTypeId());
    }
    else if (mcsControllerType == "ideal")
    {
        nrSlHelper->SetMcsControllerTypeId(NrSlIdealMcsController::GetTypeId());
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

    NetDeviceContainer manetNetDevs = nrSlHelper->InstallUeDevice(manetNodes, allBwps);

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

    // Install ProSe layer and corresponding SAPs in the UEs
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();

    // Configure the MANET
    nrSlProseHelper->ConfigureUesForL3Manet(manetNodes, manetNetDevs, Ipv4Address("8.0.0.1"));

    // Traffic profile for unicast links  signaling bearers
    SidelinkInfo slSrbSlInfo;
    slSrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slSrbSlInfo.m_dynamic = true;
    slSrbSlInfo.m_pdb = MilliSeconds(100);
    slSrbSlInfo.m_priority = 1;

    nrSlProseHelper->SetL3ManetDefaultSlSrbSlInfo(manetNetDevs, slSrbSlInfo);

    // Traffic profile for unicast links default data bearers
    SidelinkInfo slDrbSlInfo;
    slDrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slDrbSlInfo.m_dynamic = true;
    slDrbSlInfo.m_pdb = MilliSeconds(100);

    nrSlProseHelper->SetL3ManetDefaultSlDrbSlInfo(manetNetDevs, slDrbSlInfo);

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

    /*
     * Configure the applications:
     * - Client app: TrafficGeneratorNgmnVoip application configured according to codec.
     * Installed in SRC UE
     * - Server app: PacketSink application to consume the received traffic. Installed in TGT UE
     */
    std::cout << "VoIp application configuration: " << std::endl;
    std::cout << std::left << std::setw(8) << "AppId"
              << " | " << std::setw(12) << "SRC Node ID"
              << " | " << std::setw(12) << "TGT Node ID"
              << " | " << std::setw(12) << "Start(s)"
              << " | " << std::setw(12) << "Stop(s)"
              << " | " << std::setw(12) << "Codec"
              << " | " << std::setw(12) << "SrcRate"
              << " | " << std::setw(14) << "Period(ms)"
              << " | " << std::setw(14) << "Payload(Bytes)"
              << " | " << std::endl;
    std::cout << std::string(134, '-') << std::endl;
    // Route change tracing
    std::ofstream* fileRouteChange =
        new std::ofstream("route-change-trace.csv", std::ios_base::out);
    if (!fileRouteChange->is_open())
    {
        std::cerr << "Error: Could not open route-change-trace.csv\n";
        return 1;
    }
    *fileRouteChange << "#Time(s),appId,srcNodeId,tgtNodeId,route(NodeIds)\n";

    // Predefined pairs
    std::vector<std::pair<uint32_t, uint32_t>> twoByTwoPairs = {{1, 4}, {2, 3}};
    std::vector<std::pair<uint32_t, uint32_t>> threeByThreePairs = {{1, 9}, {3, 7}, {4, 6}, {2, 8}};
    std::vector<std::pair<uint32_t, uint32_t>> fourByFourPairs =
        {{1, 16}, {4, 13}, {2, 15}, {5, 12}, {14, 3}, {8, 9}};

    // Select appPairs according to grid size and nApps
    std::vector<std::pair<uint32_t, uint32_t>> appPairs;

    if (gridSize == 2)
    {
        if (nApps > 1)
        {
            NS_ABORT_MSG("nApps (" << nApps << ") should be 1 for gridSize=2");
        }
        appPairs.push_back({1, 2});
    }
    else if (gridSize == 4)
    {
        if (nApps > twoByTwoPairs.size())
        {
            NS_ABORT_MSG("nApps (" << nApps << ") is larger than available pairs for gridSize=4 ("
                                   << twoByTwoPairs.size() << ")");
        }
        appPairs.assign(twoByTwoPairs.begin(), twoByTwoPairs.begin() + nApps);
    }
    else if (gridSize == 9)
    {
        if (nApps > threeByThreePairs.size())
        {
            NS_ABORT_MSG("nApps (" << nApps << ") is larger than available pairs for gridSize=9 ("
                                   << threeByThreePairs.size() << ")");
        }
        appPairs.assign(threeByThreePairs.begin(), threeByThreePairs.begin() + nApps);
    }
    else if (gridSize == 16)
    {
        if (nApps > fourByFourPairs.size())
        {
            NS_ABORT_MSG("nApps (" << nApps << ") is larger than available pairs for gridSize=16 ("
                                   << fourByFourPairs.size() << ")");
        }
        appPairs.assign(fourByFourPairs.begin(), fourByFourPairs.begin() + nApps);
    }
    else
    {
        NS_ABORT_MSG("Unsupported grid size: " << gridSize);
    }

    std::list<AppInfo> appInfoList;
    uint32_t appId = 1;
    uint16_t appPort = 1234;

    for (const auto& p : appPairs)
    {
        const uint32_t srcId = p.first;
        const uint32_t tgtId = p.second;

        // Resolve nodes by ID (returns nullptr if not found)
        Ptr<Node> srcNode = FindNodeById(srcId, manetNodes);
        Ptr<Node> tgtNode = FindNodeById(tgtId, manetNodes);
        if (srcNode == nullptr || tgtNode == nullptr)
        {
            std::cerr << "Skip app " << appId << " — node not found (srcId=" << srcId
                      << ", tgtId=" << tgtId << ")\n";
            continue;
        }

        // Create the AppInfo inside the list so its storage is stable for trace callbacks.
        appInfoList.emplace_back();
        AppInfo& appInfo = appInfoList.back();

        appInfo.appId = appId;
        appInfo.srcNodeId = srcId;
        appInfo.tgtNodeId = tgtId;
        appInfo.codecParams = g_codecParams;

        // Get IPs from nodes (iface index 1 assumed)
        appInfo.srcUeIpAddress = srcNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        appInfo.tgtUeIpAddress = tgtNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();

        // --- Configure client (TX) on SRC ---
        TrafficGeneratorHelper voipTrafficGenHelper(
            "ns3::UdpSocketFactory",
            InetSocketAddress(appInfo.tgtUeIpAddress, appPort),
            TrafficGeneratorNgmnVoip::GetTypeId());

        voipTrafficGenHelper.SetAttribute(
            "EncoderFrameLength",
            UintegerValue(appInfo.codecParams.voicePeriodicity.GetMilliSeconds()));
        voipTrafficGenHelper.SetAttribute("VoicePayload",
                                          UintegerValue(appInfo.codecParams.voicePayloadBytes));
        voipTrafficGenHelper.SetAttribute("SidEnabled", BooleanValue(false));
        voipTrafficGenHelper.SetAttribute("VoiceActivityFactor", DoubleValue(0.5));
        voipTrafficGenHelper.SetAttribute("MeanTalkSpurtDuration", UintegerValue(4690)); // ms
        voipTrafficGenHelper.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));

        ApplicationContainer clientApp = voipTrafficGenHelper.Install(srcNode);
        clientApp.Start(startTrafficTime);
        clientApp.Stop(simTime - Seconds(1));
        Ptr<TrafficGenerator> trafficGenerator = clientApp.Get(0)->GetObject<TrafficGenerator>();
        trafficGenerator->Initialize();

        // --- Configure server (RX) on TGT ---
        PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory",
                                      InetSocketAddress(Ipv4Address::GetAny(), appPort));
        sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
        ApplicationContainer serverApp = sidelinkSink.Install(tgtNode);
        serverApp.Start(startTrafficTime);
        serverApp.Stop(simTime);
        // --- QoS rule for this app traffic ---
        SidelinkInfo customSlInfo;
        customSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
        if (rtxType != "No")
        {
            customSlInfo.m_harqEnabled = true;
        }
        customSlInfo.m_dynamic = false; // SPS
        customSlInfo.m_rri = appInfo.codecParams.voicePeriodicity;
        customSlInfo.m_pdb = MilliSeconds(appPerHopPdb);

        Ptr<NrSlTft> customQosRule = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT,
                                                     appInfo.tgtUeIpAddress,
                                                     appPort,
                                                     customSlInfo);
        nrSlProseHelper->ConfigureL3ManetQosRule(manetNetDevs, customQosRule);

        // --- Hook trace sinks using the actual indices we just installed ---
        uint32_t srcAppIdx = srcNode->GetNApplications() - 1;
        uint32_t tgtAppIdx = tgtNode->GetNApplications() - 1;

        std::ostringstream path;
        path << "/NodeList/" << appInfo.tgtNodeId << "/ApplicationList/" << tgtAppIdx
             << "/$ns3::PacketSink/RxWithSeqTsSize";
        Config::ConnectWithoutContext(path.str(),
                                      MakeBoundCallback(&ReceivePacketSeqTs, &appInfo.pktTrace));
        path.str("");

        path << "/NodeList/" << appInfo.srcNodeId << "/ApplicationList/" << srcAppIdx
             << "/$ns3::TrafficGeneratorNgmnVoip/TxWithSeqTsSize";
        Config::ConnectWithoutContext(path.str(),
                                      MakeBoundCallback(&TransmitPacketSeqTs, &appInfo.pktTrace));
        path.str("");

        // --- Print configuration row ---
        std::cout << std::left << std::setw(8) << appInfo.appId << " | " << std::setw(12)
                  << appInfo.srcNodeId << " | " << std::setw(12) << appInfo.tgtNodeId << " | "
                  << std::setw(12) << startTrafficTime.GetSeconds() << " | " << std::setw(12)
                  << simTime.GetSeconds() << " | " << std::setw(12) << appInfo.codecParams.codecName
                  << " | " << std::setw(12) << appInfo.codecParams.sourceRate << " | "
                  << std::setw(14) << appInfo.codecParams.voicePeriodicity.GetMilliSeconds()
                  << " | " << std::setw(14) << appInfo.codecParams.voicePayloadBytes << " | "
                  << std::endl;

        // --- Route change tracing (unchanged) ---
        *fileRouteChange << "0," << appInfo.appId << "," << appInfo.srcNodeId << ","
                         << appInfo.tgtNodeId << ",NaN\n";
        Simulator::Schedule(Seconds(1.0),
                            &LookupRoute,
                            manetNodes,
                            appInfo.appId,
                            appInfo.srcNodeId,
                            appInfo.tgtNodeId,
                            std::vector<uint32_t>(),
                            false,
                            Seconds(0.01),
                            fileRouteChange);

        // Next app
        appId++;
        appPort++;
    }
    std::cout << std::string(134, '-') << std::endl;

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
        Ptr<NrSlL3ManetService> prose = device->GetObject<NrSlL3ManetService>();
        prose->TraceConnect("DirectLinkEstablished",
                            std::to_string(device->GetNode()->GetId()),
                            MakeBoundCallback(&NotifyLinkEstablished, directLinkPacketTrace));
        prose->TraceConnect("DirectLinkReleasing",
                            std::to_string(device->GetNode()->GetId()),
                            MakeBoundCallback(&NotifyLinkReleasing, directLinkPacketTrace));
        prose->TraceConnectWithoutContext("RadioLinkFailure",
                                          MakeBoundCallback(&RlfTrace, rlfTraceStream));
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

        auto prose =
            manetNetDevs.Get(i)->GetObject<NrUeNetDevice>()->GetObject<NrSlL3ManetService>();

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
        Config::ConnectWithoutContext(ossSlEvent.str(),
                                      MakeBoundCallback(&TraceSlChannelsEvent, stats));
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
    SaveNodePositions(manetNodes, fileNodePositions);

    /******************* Set random variable stream numbers **************************/
    int64_t streamBase(1000);
    [[maybe_unused]] int64_t streamsUsed(0);
    streamsUsed = nrSlHelper->AssignStreams(manetNetDevs, streamBase);
    streamBase = 2000;
    streamsUsed += nrSlProseHelper->AssignStreams(manetNodes, streamBase);
    streamBase = 3000;
    streamsUsed += ApplicationHelper::AssignStreamsToAllApps(manetNodes, streamBase);
    NS_LOG_DEBUG("Random variable streams used: " << streamsUsed);
    /******************* End set random variable stream numbers **********************/

    NrSlTraceHelper nrSlTraceHelper;
    if (writeSchedulingTraces)
    {
        nrSlTraceHelper.TraceUePositions(manetNodes,
                                         Seconds(0),
                                         "nr-prose-multihop-advanced-ue-positions.dat");
        for (uint32_t i = 0; i < manetNodes.GetN(); i++)
        {
            // Node IDs are one greater than the index value of the container
            std::string sensingString =
                "nr-prose-multihop-advanced-sensing-" + std::to_string(i + 1) + ".dat";
            std::string schedString =
                "nr-prose-multihop-advanced-scheduling-" + std::to_string(i + 1) + ".dat";
            nrSlTraceHelper.TraceSensingAlgorithm(manetNodes.Get(i), sensingString);
            nrSlTraceHelper.TraceSchedulingAlgorithm(manetNodes.Get(i), schedString);
        }
    }

    if (mcsControllerType == "ideal")
    {
        Ptr<OutputStreamWrapper> mcsChangeTrace = ascii.CreateFileStream("mcs-change-trace.csv");
        *mcsChangeTrace->GetStream() << "#time(s),nodeId,dstL2Id,newMcs" << std::endl;
        for (uint32_t i = 0; i < manetNodes.GetN(); i++)
        {
            Ptr<NrSlUeMacSchedulerDefault> sched =
                nrSlTraceHelper.GetNrSlUeMacScheduler(manetNodes.Get(i))
                    ->GetObject<NrSlUeMacSchedulerDefault>();
            sched->TraceConnect("McsChange",
                                std::to_string(manetNodes.Get(i)->GetId()),
                                MakeBoundCallback(&TraceMcsChange, mcsChangeTrace));
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
        Ptr<NrSlUeMacSchedulerDefault> sched =
            nrSlTraceHelper.GetNrSlUeMacScheduler(manetNodes.Get(i))
                ->GetObject<NrSlUeMacSchedulerDefault>();
        sched->TraceConnectWithoutContext(
            "AddLogicalChannel",
            MakeBoundCallback(&TraceLogicalChannelAddition, rlcDropsTrace, nodeRlcStats));
    }

    Simulator::Stop(simTime);

    Simulator::Run();

    // Process and save PHY statistics
    ProcessPhyStats(nodePhyStats);

    // Process and save VoIP packet trace
    ProcessPacketTrace(appInfoList);

    // Process and save route changes during traffic
    fileRouteChange->close();
    ProcessRouteChanges(startTrafficTime, simTime, gridSize);

    // Process and save RLC statistics
    ProcessRlcStats(nodesRlcStats);

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
