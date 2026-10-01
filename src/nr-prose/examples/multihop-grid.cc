//
// SPDX-License-Identifier: NIST-Software
//

/**
 * @ingroup examples
 * @file multihop-grid.cc
 * @brief An example using ProSe Unicast communication and L3 U2U relay using
 *        OLSRv2/NHDP as a MANET router
 *
 * See multihop-grid.md for additional documentation.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nhdp-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/propagation-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"

#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MultihopGrid");

// Forward declarations; see below for documentation
NodeContainer CreateGrid(uint32_t gridSize, double distance, Ptr<PropagationLossModel> lossModel);

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

int
main(int argc, char* argv[])
{
    // Topology
    uint32_t gridSize(4);                // 4, 9, or 16
    std::string lossModelType("matrix"); // propagation loss model
    double distance(100);                // meters

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
    cmd.Usage("Sidelink Multihop U2U relay example program with grid topology");
    cmd.AddValue("gridSize", "Size of grid (4, 9, or 16 nodes)", gridSize);
    cmd.AddValue("lossModel", "Loss model type (matrix, friis, log-distance, umi)", lossModelType);
    cmd.AddValue("distance", "Distance in meters", distance);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("rsrpThreshold", "The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold", rsrpThreshold);
    cmd.AddValue("rsrpCoefficient", "The SD-RSRP and/or SL-RSRP coefficent", rsrpCoefficient);
    cmd.AddValue("rsrpHysteresis", "The SD-RSRP and/or SL-RSRP (dB) hysteresis", rsrpHysteresis);
    cmd.AddValue("enablePcap", "Whether to enable PCAP tracing", enablePcap);
    cmd.AddValue("log", "Whether to enable selected logging of ProSe components", log);
    cmd.Parse(argc, argv);

    if (log)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC));
        LogComponentEnable("NrSlUeProseDirectLink", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlL3ManetService", LOG_LEVEL_ALL);
        LogComponentEnable("NrSlUeService", LOG_LEVEL_ALL);
        LogComponentEnable("MultihopGrid", LOG_LEVEL_ALL);
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
    else if (lossModelType == "log-distance")
    {
        lossModel = CreateObject<LogDistancePropagationLossModel>();
        // For outdoor-to-outdoor, the exponent is around 2, and the reference loss is
        // 46.6 dB at 5 GHz.  However, since we are at a different frequency, we add the
        // frequency-dependent term to the reference loss and change the attribute value,
        // because the ns-3 LogDistance model doesn't have a frequency attribute.
        // See 3GPP TR 36.843 Section A.2.1.2, WINNER II Channel Models, D1.1.2 V1.2.,
        // Equation (4.24) p.43, available at:
        // http://www.cept.org/files/1050/documents/winner2%20-%20final%20report.pdf.
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
        // Urban Microcell (UMi) LOS model from WINNER+ B1 model, also in NIST LLS
        // The NIST LLS code is:
        // 40 * log10(d_m) + 7.56 - 17.3 * log10(hBS_e) - 17.3 * log10(hMS_e) + 2.7 * log10(f_GHz);
        // We can represent this by the ns-3 LogDistance model by setting the exponent to 4
        // and by setting the ReferenceLoss attribute to the other terms.  The heights are
        // 1.5m.
        //
        // Using this loss model:
        // - the distance of 1200m can be used to send along the grid edges at MCS 0
        // - the distance of 800m can be used to send along the grid edges at MCS 14
        // - the distance of 400m can be used to send along the grid edges at MCS 28
        //
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

    // Node creation
    NodeContainer manetNodes = CreateGrid(gridSize, distance, lossModel);
    NodeContainer udpNodes;
    udpNodes.Add(manetNodes.Get(0));
    udpNodes.Add(manetNodes.Get(gridSize - 1));

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
            internet.EnablePcapIpv4("multihop-grid", manetNodes.Get(i)->GetId(), 1, false);
        }
    }

    // Trace routing tables
    Ptr<OutputStreamWrapper> routingStream =
        Create<OutputStreamWrapper>("multihop-grid.routes", std::ios::out);
    // Trace routing tables every second
    Ipv4RoutingHelper::PrintRoutingTableAllEvery(Seconds(1), routingStream);

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

    nhdp::NhdpHelper h;
    h.EnableLinkChangeTrace(manetNodes);

    Simulator::Stop(simTime + TimeStep(1));
    Simulator::Run();

    std::cout << "Total Tx bits = " << txByteCounter * 8 << std::endl;
    std::cout << "Total Tx packets = " << txPktCounter << std::endl;

    std::cout << "Total Rx bits = " << rxByteCounter * 8 << std::endl;
    std::cout << "Total Rx packets = " << rxPktCounter << std::endl;

    std::cout << "Avrg thput = "
              << (rxByteCounter * 8) / (simTime - udpStartTime - Seconds(1)).GetSeconds() / 1000.0
              << " kbps" << std::endl;

    std::cout << "Average Packet Inter-Reception (PIR) " << pir.GetSeconds() / pirCounter << " sec"
              << std::endl;

    Simulator::Destroy();
    return 0;
}

void
DisableLoss(Ptr<MatrixPropagationLossModel> lossModel, Ptr<Node> a, Ptr<Node> b)
{
    lossModel->SetLoss(a->GetObject<MobilityModel>(), b->GetObject<MobilityModel>(), 0);
}

NodeContainer
CreateGrid(uint32_t gridSize, double distance, Ptr<PropagationLossModel> lossModel)
{
    auto matrixLossModel = DynamicCast<MatrixPropagationLossModel>(lossModel);
    // Create and destroy one unused node, so that node ID 0 is not used in
    // the simulation and node IDs align with dstL2Ids
    auto firstNode = CreateObject<Node>();
    firstNode = nullptr;

    NS_ABORT_MSG_UNLESS(distance >= 100, "Distance must be 100m or greater");
    NodeContainer n;
    auto gridWidth = static_cast<uint32_t>(std::sqrt(gridSize));
    NS_ABORT_MSG_UNLESS(gridWidth * gridWidth == gridSize && gridSize >= 4,
                        "Grid size must be perfect square");
    for (uint32_t i = 0; i < gridWidth; i++)
    {
        for (uint32_t j = 0; j < gridWidth; j++)
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
        for (uint32_t i = 0; i < gridWidth; ++i)
        {
            for (uint32_t j = 0; j < gridWidth; ++j)
            {
                const uint32_t idx = i * gridWidth + j;

                // Right neighbor (no diagonals)
                if (j + 1 < gridWidth)
                {
                    DisableLoss(matrixLossModel, n.Get(idx), n.Get(idx + 1));
                }

                // Down neighbor (no diagonals)
                if (i + 1 < gridWidth)
                {
                    DisableLoss(matrixLossModel, n.Get(idx), n.Get(idx + gridWidth));
                }
            }
        }
    }
    return n;
}
