/*
 * Copyright (c) 2011 University of Kansas
 *
 * SPDX-License-Identifier: GPL-2.0-only and NIST-Software
 *
 * Author: Justin Rohrer <rohrej@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

/*
 * Adaptation of examples/routing/manet-routing-compare.cc
 *
 * This example program allows one to run ns-3 OLSR under
 * a typical random waypoint mobility model.
 *
 * By default, the simulation runs for 200 simulated seconds, of which
 * the first 100 are used for start-up time.  The number of nodes is 50.
 * Nodes move according to RandomWaypointMobilityModel with a speed of
 * 20 m/s and no pause time within a 300x1500 m region.  The WiFi is
 * in ad hoc mode with a 11 Mb/s rate (802.11b) and a Friis loss model.
 * The transmit power is set to 7.5 dBm.
 *
 * It is possible to change the mobility and density of the network by
 * directly modifying the speed and the number of nodes.  It is also
 * possible to change the characteristics of the network by changing
 * the transmit power (as power increases, the impact of mobility
 * decreases and the effective density increases).
 *
 * By default, there are 10 source/sink data pairs sending UDP data
 * at an application rate of 2.048 Kb/s each.    This is typically done
 * at a rate of 4 64-byte packets per second.  Application data is
 * started at a random time between 100 and 101 seconds and continues
 * to the end of the simulation.
 *
 * The program outputs a few items:
 * - packet receptions are notified to stdout such as:
 *   <timestamp> <node-id> received one packet from <src-address>
 * - each second, the data reception statistics are tabulated and output
 *   to a comma-separated value (csv) file
 * - mobility traces of the nodes are printed to 'manet-routing.mob';
 *   this trace can be disabled using a command-line argument
 * - some tracing and flow monitor configuration that used to work is
 *   left commented inline in the program
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nhdp-module.h"
#include "ns3/olsr-module.h"
#include "ns3/olsrv2-module.h"
#include "ns3/yans-wifi-helper.h"

#include <fstream>
#include <iostream>
#include <ranges>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("ManetRoutingExample");

/**
 * Routing experiment class.
 *
 * It handles the creation and run of an experiment.
 */
class RoutingExperiment
{
  public:
    RoutingExperiment();
    /**
     * Run the experiment.
     */
    void Run();

    /**
     * Handles the command-line parameters.
     * @param argc The argument count.
     * @param argv The argument vector.
     */
    void CommandSetup(int argc, char** argv);

  private:
    /**
     * Setup the receiving socket in a Sink Node.
     * @param addr The address of the node.
     * @param node The node pointer.
     * @return the socket.
     */
    Ptr<Socket> SetupPacketReceive(Ipv4Address addr, Ptr<Node> node);
    /**
     * Receive a packet.
     * @param socket The receiving socket.
     */
    void ReceivePacket(Ptr<Socket> socket);
    /**
     * Compute the throughput.
     */
    void CheckThroughput();

    void OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void OlsrRx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    void Olsrv2Rx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);

    void OlsrRoutingTableChange(uint32_t tableSize);
    void L3LocalDeliver(const Ipv4Header& header,
                                       Ptr<const Packet> packet,
                                       uint32_t interface)

    void AppTx(Ptr<const Packet> packet);
    void AppRx(Ptr<const Packet> packet);
    uint32_t port{9};                 //!< Receiving port number.
    uint32_t bytesReceived{0};        //!< Received bytes in throughput interval.
    uint32_t packetsReceived{0};      //!< Received packets in throughput interval.
    uint32_t packetsReceivedTotal{0}; //!< Total received packets in simulation.
    uint64_t m_packetsSent{}; //! Totall application packets sent
    uint64_t m_packetsReceived{}; //! Total application packets received

    std::string m_csvFileName{"manet-routing.csv"}; //!< CSV filename.
    int m_nSinks{10};                               //!< Number of sink nodes.
    std::string m_protocolName{"OLSR"};             //!< Protocol name.
    double m_txp{7.5};                              //!< Tx power.
    bool m_traceMobility{false};                    //!< Enable mobility tracing.
    uint32_t m_nodes{50};                           //!< Number of nodes
    bool m_flowMonitor{true};                       //!< Enable FlowMonitor
    uint64_t m_txPacketsOlsrTrace{0u};
    uint64_t m_txPacketsOlsrBytesTotal{0u};
    uint64_t m_rxPacketsOlsrTrace{0u};
    Time m_simulationTime{Seconds(200)};            //!< Simulation time
    int m_nodeSpeed{20};                            //!< Node speed in m/s
    double m_scale{1};                              //!< Scale factor for waypoint coordinates
    Time m_startTime{Seconds(60)};  //! Time to start applications

    uint64_t m_totalRoutingTableChanges{0u};
    uint64_t m_periodRoutingTableChanges{0u};

    unsigned long m_totalHops{};

    int m_scenarioId{};
};

RoutingExperiment::RoutingExperiment()
{
}

static inline std::string
PrintReceivedPacket(Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress)
{
    std::ostringstream oss;

    oss << Simulator::Now().GetSeconds() << " " << socket->GetNode()->GetId();

    if (InetSocketAddress::IsMatchingType(senderAddress))
    {
        InetSocketAddress addr = InetSocketAddress::ConvertFrom(senderAddress);
        oss << " received one packet from " << addr.GetIpv4();
    }
    else
    {
        oss << " received one packet!";
    }
    return oss.str();
}

void
RoutingExperiment::ReceivePacket(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address senderAddress;
    while ((packet = socket->RecvFrom(senderAddress)))
    {
        bytesReceived += packet->GetSize();
        packetsReceived += 1;
        packetsReceivedTotal += 1;
        NS_LOG_INFO(PrintReceivedPacket(socket, packet, senderAddress));
        AppRx(packet);
        NS_LOG_UNCOND(PrintReceivedPacket(socket, packet, senderAddress));
        AppRx(packet);
    }
}

void
RoutingExperiment::CheckThroughput()
{
    double kbs = (bytesReceived * 8.0) / 1000;
    bytesReceived = 0;

    std::ofstream out(m_csvFileName, std::ios::app);

    out << (Simulator::Now()).GetSeconds() << "," << kbs << "," << packetsReceived << ","
        << m_nSinks << "," << m_protocolName << "," << m_txp << "" << std::endl;

    out.close();
    packetsReceived = 0;
    Simulator::Schedule(Seconds(1), &RoutingExperiment::CheckThroughput, this);
}

Ptr<Socket>
RoutingExperiment::SetupPacketReceive(Ipv4Address addr, Ptr<Node> node)
{
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> sink = Socket::CreateSocket(node, tid);
    InetSocketAddress local = InetSocketAddress(addr, port);
    sink->Bind(local);
    sink->SetRecvCallback(MakeCallback(&RoutingExperiment::ReceivePacket, this));

    return sink;
}

void
RoutingExperiment::CommandSetup(int argc, char** argv)
{
    CommandLine cmd(__FILE__);
    cmd.AddValue("csvFileName", "The name of the CSV output file name", m_csvFileName);
    cmd.AddValue("traceMobility", "Enable mobility tracing", m_traceMobility);
    cmd.AddValue("protocol", "Routing protocol (OLSR)", m_protocolName);
    cmd.AddValue("nodes", "Number of nodes", m_nodes);
    cmd.AddValue("flowMonitor", "enable FlowMonitor", m_flowMonitor);
    cmd.AddValue("simulationTime", "simulation time", m_simulationTime);
    cmd.AddValue("speed", "Node speed in m/s", m_nodeSpeed);
    cmd.AddValue("scale", "Scale factor for waypoint coordinates", m_scale);
    cmd.AddValue("scenarioId", "", m_scenarioId);
    cmd.AddValue("nodeSpeed", "", m_nodeSpeed);
    cmd.AddValue("startTime", "", m_startTime);

    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(m_nodes < 20, "Number of nodes " << m_nodes << " must be >= 20");

    std::vector<std::string> allowedProtocols{"OLSR", "OLSRv2"};

    if (std::find(std::begin(allowedProtocols), std::end(allowedProtocols), m_protocolName) ==
        std::end(allowedProtocols))
    {
        NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    }

    std::ofstream scenarioInfo{"scenario-info-" + std::to_string(m_scenarioId) + ".json"};
    scenarioInfo << '{'
    << "\"scenarioId\": " << m_scenarioId << ','
    << "\"speed\": " << m_nodeSpeed << ','
    << "\"scale\": " << m_scale << ','
    << "\"startTimeSeconds\": " << m_startTime.ToInteger(Time::S) << ','
    << "\"simulationTimeSeconds\": " << m_simulationTime.ToInteger(Time::S) << ','
    << "\"nodes\": " << m_nodes
    << '}';
}

void
RoutingExperiment::OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
RoutingExperiment::OlsrRx(const olsr::PacketHeader&, const olsr::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

void
RoutingExperiment::Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
RoutingExperiment::Olsrv2Rx(const olsrv2::PacketHeader&, const olsrv2::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

void
RoutingExperiment::OlsrRoutingTableChange(uint32_t)
{
    m_totalRoutingTableChanges++;
    m_periodRoutingTableChanges++;
}

void
RoutingExperiment::AppTx(Ptr<const Packet>)
{
    m_packetsSent++;
}

void
RoutingExperiment::AppRx(Ptr<const Packet>)
{
    m_packetsReceived++;
}

int
main(int argc, char* argv[])
{
    RoutingExperiment experiment;
    experiment.CommandSetup(argc, argv);
    experiment.Run();

    return 0;
}

void
RoutingExperiment::Run()
{
    Packet::EnablePrinting();

    // blank out the last output file and write the column headers
    std::ofstream out(m_csvFileName);
    out << "SimulationSecond,"
        << "ReceiveRate,"
        << "PacketsReceived,"
        << "NumberOfSinks,"
        << "RoutingProtocol,"
        << "TransmissionPower" << std::endl;
    out.close();

    std::string rate("2048bps");
    std::string phyMode("HeMcs0");
    std::string tr_name("manet-routing");
    int nodePause = 0; // in s

    Config::SetDefault("ns3::OnOffApplication::PacketSize", StringValue("64"));
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(rate));

    // Set Non-unicastMode rate to unicast mode
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

    NodeContainer adhocNodes;
    adhocNodes.Create(m_nodes);

    MobilityHelper mobilityAdhoc;
    int64_t streamIndex = 0;        // used to get consistent mobility across scenarios
    int64_t streamIncrement = 1000; // used to decouple stream assignments

    ObjectFactory pos;
    pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
    double xMax = 200 * m_scale;
    double yMax = 200 * m_scale;
    pos.Set("X",
            StringValue("ns3::UniformRandomVariable[Min=0.0|Max=" + std::to_string(xMax) + "]"));
    pos.Set("Y",
            StringValue("ns3::UniformRandomVariable[Min=0.0|Max=" + std::to_string(yMax) + "]"));

    Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
    auto streamsUsed = taPositionAlloc->AssignStreams(streamIndex);
    NS_LOG_DEBUG("Streams used by position allocator: " << streamsUsed);
    streamIndex += streamIncrement;

    if (m_nodeSpeed == 0)
    {
        mobilityAdhoc.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobilityAdhoc.SetPositionAllocator(taPositionAlloc);
    }
    else
    {
        std::stringstream ssSpeed;
        ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << m_nodeSpeed << "]";
        std::stringstream ssPause;
        ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
        mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                       "Speed",
                                       StringValue(ssSpeed.str()),
                                       "Pause",
                                       StringValue(ssPause.str()),
                                       "PositionAllocator",
                                       PointerValue(taPositionAlloc));
    }

    mobilityAdhoc.Install(adhocNodes);
    streamsUsed = mobilityAdhoc.AssignStreams(adhocNodes, streamIndex);
    NS_LOG_DEBUG("Streams used by mobility models: " << streamsUsed);
    streamIndex += streamIncrement;

    // setting up wifi phy and channel using helpers
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);

    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel(wifiChannel.Create());

    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(phyMode),
                                 "ControlMode",
                                 StringValue(phyMode));

    wifiPhy.Set("TxPowerStart", DoubleValue(m_txp));
    wifiPhy.Set("TxPowerEnd", DoubleValue(m_txp));

    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);
    streamsUsed = WifiHelper::AssignStreams(adhocDevices, streamIndex);
    NS_LOG_DEBUG("Streams used by wifi models: " << streamsUsed);
    streamIndex += streamIncrement;

    OlsrHelper olsr;
    Olsrv2Helper olsrv2;
    Ipv4ListRoutingHelper list;
    InternetStackHelper internet;
    NhdpHelper nhdpHelper;
    ApplicationContainer nhdpApps;

    if (m_protocolName == "OLSR")
    {
        list.Add(olsr, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "OLSRv2")
    {
        list.Add(olsrv2, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
        nhdpApps = nhdpHelper.Install(adhocNodes);
    }
    else
    {
        NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    }
    streamsUsed = internet.AssignStreams(adhocNodes, streamIndex);
    NS_LOG_DEBUG("Streams used by internet models: " << streamsUsed);
    streamIndex += streamIncrement;
    if (m_protocolName == "OLSRv2")
    {
        streamsUsed = olsrv2.AssignStreams(adhocNodes, streamIndex);
        NS_LOG_DEBUG("Streams used by OLSRv2 models: " << streamsUsed);
        streamsUsed = nhdpHelper.AssignStreams(adhocNodes, streamIndex + streamsUsed);
        NS_LOG_DEBUG("Streams used by NHDP models: " << streamsUsed);
        streamIndex += streamIncrement;
    }
    else
    {
        streamsUsed = olsr.AssignStreams(adhocNodes, streamIndex);
        NS_LOG_DEBUG("Streams used by OLSR models: " << streamsUsed);
        streamIndex += streamIncrement;
    }

    NS_LOG_INFO("assigning ip address");

    Ipv4AddressHelper addressAdhoc;
    addressAdhoc.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer adhocInterfaces;
    adhocInterfaces = addressAdhoc.Assign(adhocDevices);

    OnOffHelper onoff1("ns3::UdpSocketFactory", Address());
    onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));

    for (int i = 0; i < m_nSinks; i++)
    {
        Ptr<Socket> sink = SetupPacketReceive(adhocInterfaces.GetAddress(i), adhocNodes.Get(i));

        AddressValue remoteAddress(InetSocketAddress(adhocInterfaces.GetAddress(i), port));
        onoff1.SetAttribute("Remote", remoteAddress);

        Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable>();
        var->SetStream(streamIndex++);
        ApplicationContainer temp = onoff1.Install(adhocNodes.Get(i + m_nSinks));
        const auto startTime = m_startTime.ToInteger(Time::S);
        temp.Start(Seconds(var->GetValue(startTime, startTime + 1)));
        temp.Stop(m_simulationTime);

        // App Tx
        for (auto app = temp.Begin(); app != temp.End(); ++app)
        {
            (*app)->TraceConnectWithoutContext("Tx", MakeCallback(&RoutingExperiment::AppTx, this));
        }
    }

    std::stringstream ss;
    ss << m_nodes;
    std::string nodes = ss.str();

    std::stringstream ss2;
    ss2 << m_nodeSpeed;
    std::string sNodeSpeed = ss2.str();

    std::stringstream ss3;
    ss3 << nodePause;
    std::string sNodePause = ss3.str();

    std::stringstream ss4;
    ss4 << rate;
    std::string sRate = ss4.str();

    // NS_LOG_INFO("Configure Tracing.");
    // tr_name = tr_name + "_" + m_protocolName +"_" + nodes + "nodes_" + sNodeSpeed + "speed_" +
    // sNodePause + "pause_" + sRate + "rate";

    // AsciiTraceHelper ascii;
    // Ptr<OutputStreamWrapper> osw = ascii.CreateFileStream(tr_name + ".tr");
    // wifiPhy.EnableAsciiAll(osw);
    AsciiTraceHelper ascii;
    if (m_traceMobility)
    {
        MobilityHelper::EnableAsciiAll(ascii.CreateFileStream(tr_name + ".mob"));
    }

    // ---- packet-delivery-ratio_olsr-traces.csv ----
    if (m_protocolName == "OLSRv2")
    {
    Simulator::Schedule(m_startTime, [this] {
        Config::ConnectWithoutContext("/NodeList/*/$ns3::olsrv2::RoutingProtocol/Tx",
                                      MakeCallback(&RoutingExperiment::Olsrv2Tx, this));

        Config::ConnectWithoutContext("/NodeList/*/$ns3::olsrv2::RoutingProtocol/Rx",
                                      MakeCallback(&RoutingExperiment::Olsrv2Rx, this));
    });
    }
    else
    {
    Simulator::Schedule(m_startTime, [this] {
        Config::ConnectWithoutContext("/NodeList/*/$ns3::olsr::RoutingProtocol/Tx",
                                      MakeCallback(&RoutingExperiment::OlsrTx, this));
        Config::ConnectWithoutContext("/NodeList/*/$ns3::olsr::RoutingProtocol/Rx",
                                      MakeCallback(&RoutingExperiment::OlsrRx, this));
    });
    }

    std::ofstream olsrTracePdrCsv{"packet-delivery-ratio_olsr-traces-" +
                                  std::to_string(m_scenarioId) + ".csv"};
    olsrTracePdrCsv << "TimeSeconds,TotalTx,TotalRx,PacketDeliveryRatio\n";
    auto writeOlsrTraces = [&olsrTracePdrCsv, this] {
        olsrTracePdrCsv << Simulator::Now().ToInteger(Time::S) << ',' << m_txPacketsOlsrTrace << ','
                        << m_rxPacketsOlsrTrace << ','
                        << (m_txPacketsOlsrTrace > 0u
                                ? static_cast<double>(m_rxPacketsOlsrTrace) / m_txPacketsOlsrTrace
                                : 0u)
                        << '\n';
    };

    for (auto i = 0; i < m_simulationTime.ToInteger(Time::S); i++)
    {
        Simulator::Schedule(Seconds(i), writeOlsrTraces);
    }

    // ---- olsr-overhead.csv ----
    std::ofstream olsrOverheadCsv{"olsr-overhead-" + std::to_string(m_scenarioId) + ".csv"};
    olsrOverheadCsv << "TimeSeconds,TxBytesPeriod,TxBytesTotal\n";
    uint64_t olsrOverheadLast{};
    for (auto i = m_startTime.ToInteger(Time::S); i < m_simulationTime.ToInteger(Time::S); i++)
    {
        Simulator::Schedule(Seconds(i), [this, &olsrOverheadCsv, &olsrOverheadLast]() {
            olsrOverheadCsv << Simulator::Now().ToInteger(Time::S) << ','
                            << m_txPacketsOlsrBytesTotal - olsrOverheadLast << ','
                            << m_txPacketsOlsrBytesTotal << '\n';
            olsrOverheadLast = m_txPacketsOlsrBytesTotal;
        });
    }

    // ---- routing-table-changes.csv ----
    if (m_protocolName == "OLSRv2")
    {
        Config::ConnectWithoutContext(
            "/NodeList/*/$ns3::olsrv2::RoutingProtocol/RoutingTableChanged",
            MakeCallback(&RoutingExperiment::OlsrRoutingTableChange, this));
    }
    else
    {
        Config::ConnectWithoutContext(
            "/NodeList/*/$ns3::olsr::RoutingProtocol/RoutingTableChanged",
            MakeCallback(&RoutingExperiment::OlsrRoutingTableChange, this));
    }

    std::ofstream olsrRoutingChangesCsv{"routing-table-changes-" + std::to_string(m_scenarioId) +
                                        ".csv"};
    olsrRoutingChangesCsv << "TimeSeconds,PeriodRoutingTableChanges,TotalRoutingTableChanges\n";
    auto writeOlsrRoutingTableChanges = [this, &olsrRoutingChangesCsv] {
        olsrRoutingChangesCsv << Simulator::Now().ToInteger(Time::S) << ','
                              << m_periodRoutingTableChanges << ',' << m_totalRoutingTableChanges
                              << '\n';

        m_periodRoutingTableChanges = 0u;
    };

    for (auto i = 0; i < m_simulationTime.ToInteger(Time::S); i++)
    {
        Simulator::Schedule(Seconds(i), writeOlsrRoutingTableChanges);
    }

    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> flowmon;
    std::ofstream flowmonTotalsCsv;
    std::ofstream flowmonPerFlowCsv;
    if (m_flowMonitor)
    {
        flowmonTotalsCsv.open("flowmonitor-totals-" + std::to_string(m_scenarioId) + ".csv");
        flowmonTotalsCsv << "TimeSeconds,TotalTx,TotalRx,PacketDeliveryRatio\n";

        flowmonPerFlowCsv.open("flowmon-per-flow-" + std::to_string(m_scenarioId) + ".csv");
        flowmonPerFlowCsv
            << "TimeSeconds,FlowId,SourceIp,DestinationIp,Tx,Rx,PacketDeliveryRatio\n";

        flowmon = flowmonHelper.InstallAll();
        auto writeFlowmonStats =
            [&flowmon, &flowmonTotalsCsv, &flowmonPerFlowCsv, &flowmonHelper]() {
                flowmon->CheckForLostPackets();
                const auto& flowStats = flowmon->GetFlowStats();
                const auto classifier =
                    DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

                // Totals
                uint64_t totalTx{0u};
                uint64_t totalRx{0u};

                for (const auto& [flowId, stats] : flowStats)
                {
                    const auto& flow = classifier->FindFlow(flowId);
                    const auto packetDeliveryRatio =
                        stats.txPackets > 0 ? static_cast<double>(stats.rxPackets) / stats.txPackets
                                            : 0.0;

                    flowmonPerFlowCsv << Simulator::Now().ToInteger(Time::S) << ',' << flowId << ','
                                      << flow.sourceAddress << ',' << flow.destinationAddress << ','
                                      << stats.txPackets << ',' << stats.rxPackets << ','
                                      << packetDeliveryRatio << '\n';

                    totalTx += stats.txPackets;
                    totalRx += stats.rxPackets;
                }

                flowmonTotalsCsv << Simulator::Now().ToInteger(Time::S) << ',' << totalTx << ','
                                 << totalRx << ','
                                 << (totalTx > 0u ? static_cast<double>(totalRx) / totalTx : 0u)
                                 << '\n';
            };

        for (auto i = 1; i < m_simulationTime.ToInteger(Time::S); i++)
        {
            Simulator::Schedule(Seconds(i), writeFlowmonStats);
        }
    }

    // ---- app-tx-rx.csv -----
    std::ofstream appPackets{"app-tx-rx-" + std::to_string(m_scenarioId) + ".csv"};
    appPackets << "TimeSeconds,TxPacketsPeriod,TxPacketsTotal,RxPacketsPeriod,RxPacketsTotal\n";
    uint64_t lastPacketsSent{};
    uint64_t lastPacketsReceived{};
    auto writeAppTxRx = [this, &appPackets, &lastPacketsSent, &lastPacketsReceived] {
        appPackets << Simulator::Now().ToInteger(Time::S) << ','
        << m_packetsSent - lastPacketsSent << ','
        << m_packetsSent << ','
        << m_packetsReceived - lastPacketsReceived << ','
        << m_packetsReceived << '\n';
        lastPacketsSent = m_packetsSent;
        lastPacketsReceived = m_packetsReceived;
    };
    for (auto i = m_startTime.ToInteger(Time::S); i < m_simulationTime.ToInteger(Time::S); i++)
    {
        Simulator::Schedule(Seconds(i), writeAppTxRx);
    }

    // ---- hop-count.csv ----
    std::ofstream hopCounts{"hop-count-" + std::to_string(m_scenarioId) + ".csv"};
    hopCounts << "TimeSeconds,HopsPeriod,HopsTotal\n";

    for (auto nodeIter = adhocNodes.Begin(); nodeIter != adhocNodes.End(); nodeIter++)
    {
        const auto &node = *nodeIter;

        auto olsrRouting = node->GetObject<olsr::RoutingProtocol>();
        const auto ipv4 = node->GetObject<Ipv4>();
        const auto ip = ipv4->GetAddress(1, 0);

        auto l3 = node->GetObject<Ipv4L3Protocol>();
        if (l3 == nullptr)
            std::clog << "fail\n";

        l3->TraceConnectWithoutContext("LocalDeliver", MakeCallback(&RoutingExperiment::L3LocalDeliver, this));

    }


    unsigned long lastHopCount{};
    auto writeHopCount = [this, &hopCounts, &lastHopCount] {
        hopCounts << Simulator::Now().ToInteger(Time::S) << ','
        << m_totalHops - lastHopCount << ','
        << m_totalHops << '\n';

        lastHopCount = m_totalHops;
    };

    for (auto i = 0; i < m_simulationTime.ToInteger(Time::S); i++)
    {
        Simulator::Schedule(Seconds(i), writeHopCount);
    }

    NS_LOG_INFO("Run Simulation.");

    CheckThroughput();

    Simulator::Stop(m_simulationTime);
    Simulator::Run();

    if (m_flowMonitor)
    {
        flowmon->SerializeToXmlFile(tr_name + ".flowmon", false, false);
    }

    std::cout << "Packets received: " << packetsReceivedTotal << std::endl;
    Simulator::Destroy();
}
