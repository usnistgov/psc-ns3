//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file multihop-random-waypoint.cc
 * @brief An example using ProSe Unicast communication and L3 U2U relay using
 *        OLSRv2/NHDP as a MANET router
 *
 * See multihop-random-waypoint.md for additional documentation.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MultihopRandomWaypoint");

// Forward declarations
NodeContainer CreateNetwork(uint32_t numNodes, double nodeSpeed, double xRange, double yRange);

/*
 * Global methods and variables to hook trace sources from different layers of
 * the protocol stack.
 */

void
NotifyLinkEstablished(std::string context,
                      uint32_t selfL2Id,
                      Ipv4Address selfIpv4Addr,
                      uint32_t peerL2Id,
                      Ipv4Address peerIpv4Addr)
{
    NS_LOG_INFO("Established " << selfIpv4Addr << " to " << peerIpv4Addr << " with L2Id "
                               << peerL2Id);
}

void
NotifyLinkReleasing(std::string context,
                    uint32_t selfL2Id,
                    Ipv4Address selfIpv4Addr,
                    uint32_t peerL2Id,
                    Ipv4Address peerIpv4Addr)
{
    NS_LOG_INFO("Releasing " << selfIpv4Addr << " to " << peerIpv4Addr << " with L2Id "
                             << peerL2Id);
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
Time lastPktTxTime;
Time lastPktRxTime;

/*
 * Global variables used to compute PIR
 */
uint64_t pirCounter = 0;
Time pir;

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
    lastPktTxTime = Simulator::Now();
}

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

int
main(int argc, char* argv[])
{
    // Topology
    uint32_t numNodes(10);
    std::string lossModelType("friis"); // propagation loss model
    double nodeSpeed(10);
    double xRange(200000); // meters
    double yRange(200000); // meters

    // Traffic parameters
    uint32_t udpPacketSize = 200;   // bytes
    DataRate udpDataRate("16Kbps"); // 16 kilobits per second

    // Simulation timeline parameters
    Time simTime(Seconds(100)); // Total simulation time
    // The application should start at time 10 seconds or later to allow the routing to converge
    Time udpStartTime(Seconds(10)); // Time to start the traffic in the application layer

    // NR parameters
    uint16_t numerologyBwpSl(0);          // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6); // band n14 (793 MHz)
    double bandwidthBandSl(10e6);         // Units of Hz
    double txPower(23);                   // Units of dBm
    std::string errorModelType("static"); // error model type (static or epa)

    // ProSe parameters
    double rsrpThreshold = -110;  // The RSRP threshold (dBm) when enabling SD-RSRP/SL-RSRP
    double rsrpCoefficient = 0.5; // The coefficient when enabling SD-RSRP/SL-RSRP
    double rsrpHysteresis = 0;    // The hysteresis when enabling SD-RSRP/SL-RSRP

    // Other
    bool log(false);
    bool enablePcap(false);

    CommandLine cmd;
    cmd.Usage("Sidelink Multihop U2U relay example program with random waypoint topology");
    cmd.AddValue("numNodes", "Size of network (2 nodes or greater)", numNodes);
    cmd.AddValue("xRange", "Limit of bounding box in x dimension", xRange);
    cmd.AddValue("yRange", "Limit of bounding box in y dimension", yRange);
    cmd.AddValue("nodeSpeed", "Speed of each node (m/s)", nodeSpeed);
    cmd.AddValue("lossModel", "Loss model type (friis)", lossModelType);
    cmd.AddValue("errorModel", "Error model type (static, epa)", errorModelType);
    cmd.AddValue("rsrpThreshold", "The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold", rsrpThreshold);
    cmd.AddValue("rsrpCoefficient", "The SD-RSRP and/or SL-RSRP coefficent", rsrpCoefficient);
    cmd.AddValue("rsrpHysteresis", "The SD-RSRP and/or SL-RSRP (dB) hysteresis", rsrpHysteresis);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("enablePcap", "Whether to enable PCAP tracing", enablePcap);
    cmd.AddValue("log", "Whether to enable selected logging of ProSe components", log);
    cmd.Parse(argc, argv);

    if (log)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
        LogComponentEnable("NrSlUeProseDirectLink", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlL3ManetService", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeService", LOG_LEVEL_ALL);
        LogComponentEnable("MultihopRandomWaypoint", LOG_LEVEL_ALL);
    }
    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

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
    else
    {
        NS_FATAL_ERROR("Other loss models not yet supported");
    }
    channel->AddPropagationLossModel(lossModel);

    // Node creation
    NodeContainer manetNodes = CreateNetwork(numNodes, nodeSpeed, xRange, yRange);
    NodeContainer udpNodes;
    udpNodes.Add(manetNodes.Get(0));
    udpNodes.Add(manetNodes.Get(numNodes - 1));

    // Configure NR SL
    auto nrHelper = CreateObject<NrSlHelper>();
    if (errorModelType == "static")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
    }
    else if (errorModelType == "epa")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlEpaErrorModel::GetTypeId());
    }

    // Configure any attributes for the UE SL configuration and then install to the nodes
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(true));
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(0));

    // Override helper method defaults
    nrHelper->SetAttribute("CentralFrequency", DoubleValue(centralFrequencyBandSl));
    nrHelper->SetAttribute("Bandwidth", DoubleValue(bandwidthBandSl));
    nrHelper->SetAttribute("Numerology", UintegerValue(numerologyBwpSl));
    // Add NR SL devices to the nodes in the MANET container
    auto manetNetDev = nrHelper->ConfigureSlNetwork(manetNodes, channel);

    // Configure ProSe
    // Set attribute defaults
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryInterval", TimeValue(Seconds(2)));
    Config::SetDefault("ns3::NrSlL3ManetService::DiscoveryStart", TimeValue(Seconds(2.5)));
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    // Install ProSe and Internet layer and corresponding SAPs in the UEs
    nrSlProseHelper->ConfigureUesForL3Manet(manetNodes, manetNetDev, Ipv4Address("8.0.0.1"));

    // Configure the collection of SD-RSRP measurements
    NrSlRrcSap::SlRemoteUeConfig slRemoteConfig;
    slRemoteConfig.slReselectionConfig.slRsrpThres = rsrpThreshold; // dBm
    slRemoteConfig.slReselectionConfig.slFilterCoefficientRsrp = rsrpCoefficient;
    slRemoteConfig.slReselectionConfig.slHystMin = rsrpHysteresis; // dB

    // Define SD-RSRP thresholds and enable RSRP measurement collection
    for (uint32_t i = 0; i < manetNetDev.GetN(); ++i)
    {
        Ptr<NrSlUeRrc> remoteRrc =
            manetNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetRrc()->GetObject<NrSlUeRrc>();
        remoteRrc->EnableUeSdRsrpMeasurements();
        remoteRrc->SetNrSlDiscoveryRemoteConfiguration(slRemoteConfig);
    }

    // Add UDP flow from first node in grid to last node
    uint16_t port = 8000;
    NS_LOG_INFO("UDP app start time " << udpStartTime.As(Time::S));
    NS_LOG_INFO("UDP app stop time " << (simTime - Seconds(1)).As(Time::S));
    NS_LOG_INFO("UDP app data rate " << udpDataRate);
    NS_LOG_INFO("UDP port " << port);
    nrSlProseHelper->AddUdpFlow(udpNodes.Get(0),
                                udpNodes.Get(1),
                                udpStartTime,                // applications start time
                                simTime - Seconds(1),        // OnOff stop time
                                simTime - MilliSeconds(500), // PacketSink stop time
                                port,
                                udpDataRate,
                                udpPacketSize);

    // Add callbacks to log direct links establishing and releasing
    for (uint32_t i = 0; i < manetNetDev.GetN(); i++)
    {
        Ptr<NrUeNetDevice> device = manetNetDev.Get(i)->GetObject<NrUeNetDevice>();
        Ptr<NrSlL3ManetService> prose = device->GetObject<NrSlL3ManetService>();
        prose->TraceConnect("DirectLinkEstablished",
                            std::to_string(device->GetNode()->GetId()),
                            MakeCallback(&NotifyLinkEstablished));
        prose->TraceConnect("DirectLinkReleasing",
                            std::to_string(device->GetNode()->GetId()),
                            MakeCallback(&NotifyLinkReleasing));
    }

    // Trace receptions; use the following to be robust to node ID changes
    std::ostringstream path;
    path << "/NodeList/" << udpNodes.Get(1)->GetId() << "/ApplicationList/1/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ReceivePacket));
    path.str("");

    path << "/NodeList/" << udpNodes.Get(1)->GetId() << "/ApplicationList/1/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&ComputePir));
    path.str("");

    path << "/NodeList/" << udpNodes.Get(0)->GetId()
         << "/ApplicationList/1/$ns3::OnOffApplication/Tx";
    Config::ConnectWithoutContext(path.str(), MakeCallback(&TransmitPacket));
    path.str("");

    /******************* PC5-S messages tracing ********************************/
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream =
        ascii.CreateFileStream("NrSlPc5SignallingPacketTrace.txt");
    *Pc5SignallingPacketTraceStream->GetStream()
        << "time(s)\tnodeId\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
    for (uint32_t i = 0; i < manetNetDev.GetN(); ++i)
    {
        Ptr<NrSlL3ManetService> prose = manetNetDev.Get(i)->GetObject<NrSlL3ManetService>();
        prose->TraceConnectWithoutContext("PC5SignallingPacketTrace",
                                          MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream,
                                                            manetNetDev.Get(i)->GetNode()));
    }
    // Enable discovery traces
    nrSlProseHelper->EnableDiscoveryTraces();

    /******************* END PC5-S messages tracing **************************/

    // Enable IPv4 tracing
    if (enablePcap)
    {
        InternetStackHelper internet;
        for (uint32_t i = 0; i < manetNodes.GetN(); i++)
        {
            internet.EnablePcapIpv4("multihop-random-waypoint",
                                    manetNodes.Get(i)->GetId(),
                                    1,
                                    false);
        }
    }

    // Trace routing tables
    Ptr<OutputStreamWrapper> routingStream =
        Create<OutputStreamWrapper>("multihop-random-waypoint.routes", std::ios::out);
    // Trace at the time of the first topology change
    Ipv4RoutingHelper::PrintRoutingTableAllAt(simTime - Seconds(1), routingStream);

    /******************* Set random variable stream numbers **************************/
    int64_t streamBase(1000);
    [[maybe_unused]] int64_t streamsUsed(0);
    streamsUsed = nrHelper->AssignStreams(manetNetDev, streamBase);
    streamBase = 2000;
    streamsUsed += nrSlProseHelper->AssignStreams(manetNodes, streamBase);
    streamBase = 3000;
    streamsUsed += ApplicationHelper::AssignStreamsToAllApps(manetNodes, streamBase);
    NS_LOG_DEBUG("Random variable streams used: " << streamsUsed);
    /******************* End set random variable stream numbers **********************/

    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream("multihop-random-waypoint.mob"));
    Simulator::Stop(simTime + TimeStep(1));
    Simulator::Run();

    std::cout << "Total Tx bits = " << txByteCounter * 8 << std::endl;
    std::cout << "Total Tx packets = " << txPktCounter << std::endl;
    std::cout << "Last packet Tx time = " << lastPktTxTime.As(Time::S) << std::endl;

    std::cout << "Total Rx bits = " << rxByteCounter * 8 << std::endl;
    std::cout << "Total Rx packets = " << rxPktCounter << std::endl;
    std::cout << "Last packet Rx time = " << lastPktRxTime.As(Time::S) << std::endl;

    std::cout << "Avrg thput = "
              << (rxByteCounter * 8) / (simTime - udpStartTime - Seconds(1)).GetSeconds() / 1000.0
              << " kbps" << std::endl;

    std::cout << "Average Packet Inter-Reception (PIR) " << pir.GetSeconds() / pirCounter << " sec"
              << std::endl;

    Simulator::Destroy();
    return 0;
}

NodeContainer
CreateNetwork(uint32_t numNodes, double nodeSpeed, double xRange, double yRange)
{
    NS_ABORT_MSG_UNLESS(numNodes >= 2, "At least two nodes are required");
    NS_ABORT_MSG_UNLESS(xRange > 0 && yRange > 0, "xRange and yRange must be positive");
    // Create and destroy one unused node, so that node ID 0 is not used in
    // the simulation and node IDs align with dstL2Ids
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    NodeContainer n;
    MobilityHelper m;
    m.SetMobilityModel("ns3::SteadyStateRandomWaypointMobilityModel",
                       "MinSpeed",
                       DoubleValue(nodeSpeed),
                       "MaxSpeed",
                       DoubleValue(nodeSpeed),
                       "MinPause",
                       DoubleValue(0),
                       "MaxPause",
                       DoubleValue(0),
                       "MinX",
                       DoubleValue(0),
                       "MaxX",
                       DoubleValue(xRange),
                       "MinY",
                       DoubleValue(0),
                       "MaxY",
                       DoubleValue(yRange));
    n.Create(numNodes);
    m.Install(n);
    return n;
}
