/*
 * SPDX-License-Identifier: NIST-Software
 */

#include "ns3/application-container.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device-container.h"
#include "ns3/nhdp-client.h"
#include "ns3/nhdp-helper.h"
#include "ns3/nhdp-info-base.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/olsr-header.h"
#include "ns3/olsr-helper.h"
#include "ns3/olsrv2-header.h"
#include "ns3/olsrv2-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/position-allocator.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/simulator.h"
#include "ns3/snr-tag.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/test.h"
#include "ns3/vector.h"
#include "ns3/waypoint-mobility-model.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/wifi-utils.h"
#include "ns3/yans-wifi-helper.h"

#include <map>
#include <vector>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("NhdpSystem");

/**
 * @defgroup nhdp-tests Tests for nhdp
 * @ingroup nhdp
 * @ingroup tests
 */

/**
 * @ingroup nhdp-tests
 *
 * Base class for common functions like trace sinks and counters
 */
class NhdpTestCase : public TestCase
{
  public:
    NhdpTestCase(std::string name);

    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    void Change(Ptr<Node> a, Ptr<Node> b, double lossDb);

    void NeighborChange(std::string context,
                        NeighborStatus neighborStatus,
                        const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, TwoHopStatus twoHopStatus, const TwoHopTuple& linkTuple);
    void HelloSend(std::string context,
                   Ipv4Address addr,
                   const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links);
    void HelloRecv(std::string context,
                   Ipv4Address addr,
                   const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links,
                   std::optional<double> quality);

    void ClearNeighborChanges();
    void CheckNeighbor(const NeighborTuple& tuple);
    void CheckNeighborChangesSize(std::size_t expectedSize);

    void ClearLinkChanges();
    void CheckLink(const LinkTuple& tuple);
    void CheckLinkChangesSize(std::size_t expectedSize);

    void ClearTwoHopChanges();
    void CheckTwoHopChangesSize(std::size_t expectedSize);

    void OlsrRoutingTableChange(std::string context, uint32_t tableSize);
    void OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void OlsrRx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    void Olsrv2Rx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    /**
     * Tests don't usually print but this can be useful when debugging the tests
     * @param client The NhdpClient to print
     */
    void Print(Ptr<NhdpClient> client);

  protected:
    void DoTeardown() override;

    std::map<Ipv4Address, NeighborTuple> m_neighborChanges;
    std::map<Ipv4Address, LinkTuple> m_linkChanges;
    std::map<Ipv4Address, TwoHopTuple> m_twoHopChanges;
    std::map<Ipv4Address, NeighborTuple> m_symmetricNeighbors;
    uint32_t m_txPacketsOlsrTrace;
    uint32_t m_txPacketsOlsrBytesTotal;
    uint32_t m_rxPacketsOlsrTrace;

    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
};

NhdpTestCase::NhdpTestCase(std::string name)
    : TestCase(name)
{
}

void
NhdpTestCase::DoTeardown()
{
    NS_LOG_FUNCTION(this);
}

void
NhdpTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               1000,
                               true);
}

void
NhdpTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
NhdpTestCase::Change(Ptr<Node> a, Ptr<Node> b, double lossDb)
{
    NS_LOG_INFO("Changing loss to " << lossDb << " on link between " << a->GetId() << " and "
                                    << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               lossDb,
                               true);
}

void
NhdpTestCase::HelloSend(std::string context,
                        Ipv4Address addr,
                        const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links)
{
    std::stringstream sstr;
    for (auto& [addr, status] : links)
    {
        sstr << " " << addr << " " << status;
    }
    NS_LOG_INFO("Sending HELLO from " << addr << " links " << links.size() << sstr.str());
}

void
NhdpTestCase::HelloRecv(std::string context,
                        Ipv4Address addr,
                        const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links,
                        std::optional<double> quality)
{
    std::stringstream sstr;
    for (auto& [addr, status] : links)
    {
        sstr << " " << addr << " " << status;
    }
    NS_LOG_INFO("Receiving HELLO from "
                << addr << " quality "
                << (quality.has_value() ? std::to_string(quality.value()) : "void") << " links "
                << links.size() << sstr.str());
}

void
NhdpTestCase::Print(Ptr<NhdpClient> client)
{
    for (const auto& [addr, tuple] : client->GetNeighborInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Neighbor tuple " << addr << " symmetric " << tuple.m_symmetric);
    }
    for (const auto& [addr, tuple] : client->GetLinkInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Link tuple " << addr << " status " << tuple.GetLinkStatus() << " lost "
                    << tuple.m_lost << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
    for (const auto& [key, tuple] : client->GetTwoHopInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Two hop neighbor " << tuple.m_twoHopAddr << " via " << key.first
                    << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
}

void
NhdpTestCase::NeighborChange(std::string context,
                             NeighborStatus neighborStatus,
                             const NeighborTuple& neighborTuple)
{
    if (neighborStatus == NeighborStatus::NEW)
    {
        NS_LOG_INFO(context << " New neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    else if (neighborStatus == NeighborStatus::MODIFIED && neighborTuple.m_symmetric)
    {
        NS_LOG_INFO(context << " Symmetric neighbor " << neighborTuple.m_neighborAddrList[0]);
        m_symmetricNeighbors.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
    }
    else if (neighborStatus == NeighborStatus::MODIFIED && !neighborTuple.m_symmetric)
    {
        NS_LOG_INFO(context << " Neighbor transitions to not symmetric "
                            << neighborTuple.m_neighborAddrList[0]);
        m_symmetricNeighbors.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
    }
    else if (neighborStatus == NeighborStatus::REMOVED)
    {
        NS_LOG_INFO(context << " Removed neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    else
    {
        NS_TEST_ASSERT_MSG_EQ(false,
                              true,
                              "Neighbor state change to "
                                  << neighborStatus << " with " << neighborTuple.m_symmetric
                                  << " not interpreted at time " << Simulator::Now().As(Time::S));
    }
    m_neighborChanges.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
}

void
NhdpTestCase::CheckNeighbor(const NeighborTuple& checkTuple)
{
    NS_LOG_INFO("Checking tuple " << checkTuple.m_neighborAddrList[0]);
    bool found{false};
    for (auto& [neighborAddr, neighborTuple] : m_neighborChanges)
    {
        if (neighborAddr == checkTuple.m_neighborAddrList[0] &&
            neighborTuple.m_symmetric == checkTuple.m_symmetric)
        {
            found = true;
            break;
        }
    }
    NS_TEST_ASSERT_MSG_EQ(found,
                          true,
                          "CheckNeighbor() failed at time " << Simulator::Now().As(Time::S));
}

void
NhdpTestCase::CheckNeighborChangesSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_neighborChanges.size(),
                          expectedSize,
                          "CheckNeighborChangesSize() failed at time " << Now().As(Time::S));
}

void
NhdpTestCase::ClearNeighborChanges()
{
    m_neighborChanges.clear();
}

void
NhdpTestCase::CheckLink(const LinkTuple& checkTuple)
{
    NS_LOG_INFO("Checking link tuple " << checkTuple.m_neighborAddrList[0]);
    bool found{false};
    // Allow some tolerance in the time values
    Time tolerance{MilliSeconds(1)};
    Time checkHeardTime = checkTuple.m_heardTime;
    Time checkHeardTimeMin =
        (checkHeardTime >= tolerance) ? (checkHeardTime - tolerance) : Seconds(0);
    Time checkHeardTimeMax = (checkHeardTime + tolerance);
    Time checkSymTime = checkTuple.m_symTime;
    Time checkSymTimeMin = (checkSymTime >= tolerance) ? (checkSymTime - tolerance) : Seconds(0);
    Time checkSymTimeMax = (checkSymTime + tolerance);
    Time checkLTime = checkTuple.m_expirationTime;
    Time checkLTimeMin = (checkLTime >= tolerance) ? (checkLTime - tolerance) : Seconds(0);
    Time checkLTimeMax = (checkLTime + tolerance);
    for (auto& [neighborAddr, linkTuple] : m_linkChanges)
    {
        NS_LOG_INFO("Checking neighborAddr " << neighborAddr);
        if (neighborAddr == checkTuple.m_neighborAddrList[0] &&
            linkTuple.m_heardTime >= checkHeardTimeMin &&
            linkTuple.m_heardTime <= checkHeardTimeMax && linkTuple.m_symTime >= checkSymTimeMin &&
            linkTuple.m_symTime <= checkSymTimeMax && linkTuple.m_quality == checkTuple.m_quality &&
            linkTuple.m_pending == checkTuple.m_pending && linkTuple.m_lost == checkTuple.m_lost &&
            linkTuple.m_expirationTime >= checkLTimeMin &&
            linkTuple.m_expirationTime <= checkLTimeMax)
        {
            found = true;
            break;
        }
    }
    NS_TEST_ASSERT_MSG_EQ(found,
                          true,
                          "CheckLink() failed at time " << Simulator::Now().As(Time::S));
}

void
NhdpTestCase::CheckLinkChangesSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_linkChanges.size(),
                          expectedSize,
                          "CheckLinkChangesSize() failed at time " << Simulator::Now().As(Time::S));
}

void
NhdpTestCase::ClearLinkChanges()
{
    m_linkChanges.clear();
}

void
NhdpTestCase::CheckTwoHopChangesSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_twoHopChanges.size(),
                          expectedSize,
                          "CheckTwoHopChangesSize() failed at time "
                              << Simulator::Now().As(Time::S));
}

void
NhdpTestCase::ClearTwoHopChanges()
{
    m_twoHopChanges.clear();
}

void
NhdpTestCase::LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple)
{
    NS_LOG_INFO(context << " Link old status " << oldLinkStatus << " new status "
                        << linkTuple.GetLinkStatus() << " " << linkTuple.m_neighborAddrList[0]
                        << " heard " << linkTuple.m_heardTime.As(Time::S) << " sym "
                        << linkTuple.m_symTime.As(Time::S) << " quality " << linkTuple.m_quality
                        << " pending " << linkTuple.m_pending << " lost " << linkTuple.m_lost
                        << " expiration " << linkTuple.m_expirationTime.As(Time::S));
    m_linkChanges.insert_or_assign(linkTuple.m_neighborAddrList[0], linkTuple);
}

void
NhdpTestCase::TwoHopChange(std::string context,
                           TwoHopStatus twoHopStatus,
                           const TwoHopTuple& twoHopTuple)
{
    if (twoHopStatus == TwoHopStatus::NEW)
    {
        NS_LOG_INFO(context << " New two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0] << " status " << twoHopStatus);
    }
    else
    {
        NS_LOG_INFO(context << " Existing two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0] << " status " << twoHopStatus);
    }
    m_twoHopChanges.insert_or_assign(twoHopTuple.m_neighborAddrList[0], twoHopTuple);
}

void
NhdpTestCase::OlsrRoutingTableChange(std::string context, uint32_t tableSize)
{
    NS_LOG_INFO(context << " routing table change to " << tableSize);
}

void
NhdpTestCase::OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpTestCase::OlsrRx(const olsr::PacketHeader&, const olsr::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

void
NhdpTestCase::Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpTestCase::Olsrv2Rx(const olsrv2::PacketHeader&, const olsrv2::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a two-node topology
 * in which each node can possibly hear one neighbor, as follows:
 *
 * 1 <------> 2
 */
class TwoNodeNhdpTestCase : public NhdpTestCase
{
  public:
    TwoNodeNhdpTestCase(std::string name);

    double LinkQualityCallback(Ptr<Packet> packet) const;

  protected:
    void DoSetup() override;
};

TwoNodeNhdpTestCase::TwoNodeNhdpTestCase(std::string name)
    : NhdpTestCase(name)
{
}

double
TwoNodeNhdpTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    double thresholdHigh = 4; // dB
    double thresholdLow = 2;  // dB
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double quality = 0.1;
    double snrDb = RatioToDb(snrTag.Get());
    if (snrDb > thresholdHigh)
    {
        quality = 1;
    }
    else if (snrDb > thresholdLow)
    {
        quality = 0.7;
    }
    NS_LOG_INFO("Link quality: " << quality << " SNR_dB: " << snrDb);
    return quality;
}

void
TwoNodeNhdpTestCase::DoSetup()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(2);

    // Not that important because we are using a Matrix prop. loss model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    // By default, the loss is 'infinite' and must be changed by SetLoss() commands in DoRun()
    channel->SetPropagationLossModel(m_matrixLossModel);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HeMcs0"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
    wifi.AssignStreams(m_devices, 100);
    // Leave Internet configuration for subclasses
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a two-node topology
 * in which each node can possibly hear one neighbor, as follows:
 *
 * At time 5, enable:   1 <------> 2
 * At time 10, disable: 1 <-- X -> 2
 * At time 25, enable:  1 <------> 2
 */
class DisableLinkTestCase : public TwoNodeNhdpTestCase
{
  public:
    DisableLinkTestCase(std::string name);

  protected:
    void DoRun() override;
};

DisableLinkTestCase::DisableLinkTestCase(std::string name)
    : TwoNodeNhdpTestCase(name)
{
}

void
DisableLinkTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(30)};

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->AssignStreams(10);
    auto nhdp2 = apps.Get(1)->GetObject<NhdpClient>();
    nhdp2->AssignStreams(11);

    nhdp1->TraceConnect("HelloSend", "1", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp2->TraceConnect("HelloSend", "2", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp1->TraceConnect("HelloRecv", "1", MakeCallback(&NhdpTestCase::HelloRecv, this));
    nhdp2->TraceConnect("HelloRecv", "2", MakeCallback(&NhdpTestCase::HelloRecv, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Enable sets the matrix loss to zero dB.  The transmit power in dBm is 16.026.  The
    // noise power in 20 MHz (default 11ax channel width for group addressed frames)
    // is approximately -101 dBm.  The default Wi-Fi noise figure is 7 dB, so the expected
    // SNR if zero loss is configured is 16.026 - (-101) - 7 = 110 dB for NHDP HELLOs.
    //
    // Therefore, to set the SNR around 0 dB for NHDP frames requires around 110 dB of loss.
    // However, for data frames to reach 0 dB SNR, the loss should be set to 104 dB because
    // the channel width is 80 MHz and data will be sent in 80 MHz.
    //
    // A 110 dB loss will result in a PER of about 0.1 for NHDP hello
    // A 104 dB loss will result in a data PER of about 0.52 and a NHDP PER of zero
    //
    // In this test we are concerned with NHDP only.  Start NHDP with the channel disabled.
    //   - at 5 seconds, enable the channel
    //   - at 6 and 8 seconds, check that HELLOs have been received and state is correct
    //   - at 10 seconds, disable the channel
    //   - at 15 seconds, check that lost link status are being circulated
    //   - at 20 seconds, check that things are removed
    //   - at 25 seconds, enable the channel
    //   - at 30 seconds, check that things are restored
    //
    Simulator::Schedule(Seconds(5), &NhdpTestCase::Enable, this, m_nodes.Get(0), m_nodes.Get(1));

    // By time 6 seconds, we should have heard that there is a link and neighbor change;
    // the link state should be HEARD.  The Neighbor is not yet symmetric.
    NeighborTuple ntAt6s{Ipv4Address("7.0.0.2")};
    ntAt6s.m_symmetric = false;
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckNeighbor, this, ntAt6s);
    // at time 6, we should see heardTime of 11.9907s, symTime should be zero,
    // quality 1, m_pending false, m_lost false.  Some values are defaults and don't need setting
    LinkTuple ltAt6s{Ipv4Address("7.0.0.2")};
    ltAt6s.m_heardTime = Seconds(11.9907); // arrival of HELLO at 5.9907 + 6 seconds
    ltAt6s.m_quality = 1;
    ltAt6s.m_expirationTime = Seconds(11.9907); // arrival of HELLO at 5.9907 + 6 seconds
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckLink, this, ltAt6s);
    // No 2-hop changes should be seen
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckTwoHopChangesSize, this, 0);

    // Print out the databases at time 6
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp2);

    // by time 8, the neighbor should be symmetric.  The heardTime and symTime should be now
    // 13.8889 seconds, and an expiration time of 19.8889 seconds (heard time + 6 sec).
    NeighborTuple ntAt8s{Ipv4Address("7.0.0.2")};
    ntAt8s.m_symmetric = true;
    Simulator::Schedule(Seconds(8), &NhdpTestCase::CheckNeighbor, this, ntAt8s);
    LinkTuple ltAt8s{Ipv4Address("7.0.0.2")};
    ltAt8s.m_heardTime = Seconds(13.8889); // arrival of last HELLO at 7.8889 + 6 seconds
    ltAt8s.m_symTime = Seconds(13.8889);
    ltAt8s.m_expirationTime = Seconds(19.8889); // heard time + 6 seconds
    ltAt8s.m_quality = 1;
    Simulator::Schedule(Seconds(8), &NhdpTestCase::CheckLink, this, ltAt8s);

    Simulator::Schedule(Seconds(9), &NhdpTestCase::ClearNeighborChanges, this);
    // There should not be neighbor changes since they were cleared at time 9
    Simulator::Schedule(Seconds(10), &NhdpTestCase::CheckNeighborChangesSize, this, 0);

    Simulator::Schedule(Seconds(10), &NhdpTestCase::Disable, this, m_nodes.Get(0), m_nodes.Get(1));

    // After disabling, we should see the link first being advertised as symmetric
    // until that expires, then as lost until the expiration time expires (by 20 s)

    // Enable again after everything has been lost
    Simulator::Schedule(Seconds(25), &NhdpTestCase::Enable, this, m_nodes.Get(0), m_nodes.Get(1));

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a two-node topology
 * in which each node can possibly hear one neighbor, as follows:
 *
 * At time 5, enable:   1 <------> 2
 * At time 10, disable: 1 <-- X -> 2
 * At time 25, enable:  1 <------> 2
 * At time 30, drop link quality but above threshold  1 <------> 2
 * At time 35, drop link quality below threshold  1 <------> 2
 *
 * This variant of the previous test checks link quality feature
 */
class DisableLinkWithQualityTestCase : public TwoNodeNhdpTestCase
{
  public:
    DisableLinkWithQualityTestCase(std::string name);

  protected:
    void DoRun() override;
};

DisableLinkWithQualityTestCase::DisableLinkWithQualityTestCase(std::string name)
    : TwoNodeNhdpTestCase(name)
{
}

void
DisableLinkWithQualityTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(45)};

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->AssignStreams(10);
    auto nhdp2 = apps.Get(1)->GetObject<NhdpClient>();
    nhdp2->AssignStreams(11);
    nhdp1->RegisterLinkQualityCallback(
        MakeCallback(&TwoNodeNhdpTestCase::LinkQualityCallback, this));
    nhdp1->SetAttribute("HystReject", DoubleValue(0.5));
    nhdp1->SetAttribute("HystAccept", DoubleValue(1));
    nhdp1->SetAttribute("InitialPending", BooleanValue(true));
    nhdp2->RegisterLinkQualityCallback(
        MakeCallback(&TwoNodeNhdpTestCase::LinkQualityCallback, this));
    nhdp2->SetAttribute("HystReject", DoubleValue(0.5));
    nhdp2->SetAttribute("HystAccept", DoubleValue(1));
    nhdp2->SetAttribute("InitialPending", BooleanValue(true));

    nhdp1->TraceConnect("HelloSend", "1", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp2->TraceConnect("HelloSend", "2", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp1->TraceConnect("HelloRecv", "1", MakeCallback(&NhdpTestCase::HelloRecv, this));
    nhdp2->TraceConnect("HelloRecv", "2", MakeCallback(&NhdpTestCase::HelloRecv, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Enable sets the matrix loss to zero dB.  The transmit power in dBm is 16.026.  The
    // noise power in 20 MHz (default 11ax channel width for group addressed frames)
    // is approximately -101 dBm.  The default Wi-Fi noise figure is 7 dB, so the expected
    // SNR if zero loss is configured is 16.026 - (-101) - 7 = 110 dB for NHDP HELLOs.
    //
    // Therefore, to set the SNR around 0 dB for NHDP frames requires around 110 dB of loss.
    // However, for data frames to reach 0 dB SNR, the loss should be set to 104 dB because
    // the channel width is 80 MHz and data will be sent in 80 MHz.
    //
    // A 110 dB loss will result in a PER of about 0.1 for NHDP hello
    // A 104 dB loss will result in a data PER of about 0.52 and a NHDP PER of zero
    //
    // In this test we are concerned with NHDP only.  Start NHDP with the channel disabled.
    //   - at 5 seconds, enable the channel but to only around 0 dB, not meeting the threshold
    //   - at 6 and 8 seconds, check that HELLOs have been received and state is correct
    //   - at 10 seconds, disable the channel
    //   - at 20 seconds, check that things are removed
    //   - at 25 seconds, enable the channel to around 6 dB.  Check that symmetric links form
    //
    Simulator::Schedule(Seconds(5),
                        &NhdpTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        110);

    // By time 6 seconds, we should have heard that there is a link and neighbor change;
    // the link state should be HEARD.  The Neighbor is not yet symmetric.
    NeighborTuple ntAt6s{Ipv4Address("7.0.0.2")};
    ntAt6s.m_symmetric = false;
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckNeighbor, this, ntAt6s);
    // at time 6, we should see heardTime of 11.9907s, symTime should be zero,
    // quality 1, m_pending false, m_lost false.  Some values are defaults and don't need setting
    LinkTuple ltAt6s{Ipv4Address("7.0.0.2")};
    ltAt6s.m_heardTime = Seconds(11.9907); // arrival of HELLO at 5.9907 + 6 seconds
    ltAt6s.m_quality = 0.1;
    ltAt6s.m_pending = true;
    ltAt6s.m_lost = false;
    ltAt6s.m_expirationTime = Seconds(11.9907); // arrival of HELLO at 5.9907 + 6 seconds
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckLink, this, ltAt6s);
    // No 2-hop changes should be seen
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckTwoHopChangesSize, this, 0);

    // Print out the databases at time 6
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp2);

    Simulator::Schedule(Seconds(9), &NhdpTestCase::ClearNeighborChanges, this);
    // There should not be neighbor changes since they were cleared at time 9
    Simulator::Schedule(Seconds(10), &NhdpTestCase::CheckNeighborChangesSize, this, 0);

    Simulator::Schedule(Seconds(10), &NhdpTestCase::Disable, this, m_nodes.Get(0), m_nodes.Get(1));

    Simulator::Schedule(Seconds(10), &NhdpTestCase::ClearNeighborChanges, this);
    // After disabling, we should see the link first being advertised as symmetric
    // until that expires, then as lost until the expiration time expires (by 20 s)
    // This should result in one neighbor changes (directly from symmetric to
    // removed state) by time 20s
    Simulator::Schedule(Seconds(20), &NhdpTestCase::CheckNeighborChangesSize, this, 1);
    // Clear the neighbor changes list
    Simulator::Schedule(Seconds(21), &NhdpTestCase::ClearNeighborChanges, this);

    // Enable above the threshold
    Simulator::Schedule(Seconds(25),
                        &NhdpTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        104);
    // Within two seconds, this should result in two neighbor changes
    // (first to non-symmetric, then to symmetric).  Since there is only one
    // map entry per IP address, there should be one entry stored in the map
    // with state symmetric
    Simulator::Schedule(Seconds(27), &NhdpTestCase::CheckNeighborChangesSize, this, 1);
    NeighborTuple ntAt27s{Ipv4Address("7.0.0.2")};
    ntAt27s.m_symmetric = true;
    Simulator::Schedule(Seconds(27), &NhdpTestCase::CheckNeighbor, this, ntAt27s);

    Simulator::Schedule(Seconds(30), &NhdpTestCase::ClearNeighborChanges, this);
    // Lower the signal strength but keep above the HYST_REJECT threshold
    Simulator::Schedule(Seconds(30),
                        &NhdpTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        106.5);

    // Check that neighbor is still symmetric at time 34 by confirming that
    // there are no new changes reported
    Simulator::Schedule(Seconds(34), &NhdpTestCase::CheckNeighborChangesSize, this, 0);

    // Drop below the threshold
    Simulator::Schedule(Seconds(35),
                        &NhdpTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        110);

    // Check for a neighbor change to non-symmetric
    Simulator::Schedule(Seconds(39), &NhdpTestCase::CheckNeighborChangesSize, this, 1);
    NeighborTuple ntAt39s{Ipv4Address("7.0.0.2")};
    ntAt39s.m_symmetric = false;
    Simulator::Schedule(Seconds(39), &NhdpTestCase::CheckNeighbor, this, ntAt39s);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a three-node topology
 * that causes two-hop neighbors to be formed, as follows:
 *
 * 1 <------> 2 <------> 3
 */
class NhdpThreeNodeTestCase : public NhdpTestCase
{
  public:
    NhdpThreeNodeTestCase(std::string name);

    double LinkQualityCallback(Ptr<Packet> packet) const;

  protected:
    void DoSetup() override;
};

NhdpThreeNodeTestCase::NhdpThreeNodeTestCase(std::string name)
    : NhdpTestCase(name)
{
}

double
NhdpThreeNodeTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    double thresholdHigh = 4; // dB
    double thresholdLow = 2;  // dB
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double quality = 0.1;
    double snrDb = RatioToDb(snrTag.Get());
    if (snrDb > thresholdHigh)
    {
        quality = 1;
    }
    else if (snrDb > thresholdLow)
    {
        quality = 0.7;
    }
    NS_LOG_INFO("Link quality: " << quality << " SNR_dB: " << snrDb);
    return quality;
}

void
NhdpThreeNodeTestCase::DoSetup()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(3);

    // Not that important because we are using a Matrix prop. loss model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(200.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    // By default, the loss is 'infinite' and must be changed by SetLoss() commands in DoRun()
    channel->SetPropagationLossModel(m_matrixLossModel);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HeMcs0"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
    wifi.AssignStreams(m_devices, 100);
    // Leave Internet configuration for subclasses
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a three-node topology
 * in which each node can possibly hear one neighbor, as follows:
 *
 * At time 5, enable:   1 <------> 2
 * At time 10, disable: 1 <-- X -> 2
 * At time 25, enable:  1 <------> 2
 */
class ThreeNodeNhdpOlsrTestCase : public NhdpThreeNodeTestCase
{
  public:
    ThreeNodeNhdpOlsrTestCase(std::string name);

  protected:
    void DoRun() override;
};

ThreeNodeNhdpOlsrTestCase::ThreeNodeNhdpOlsrTestCase(std::string name)
    : NhdpThreeNodeTestCase(name)
{
}

void
ThreeNodeNhdpOlsrTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(30)};

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->AssignStreams(10);
    auto nhdp2 = apps.Get(1)->GetObject<NhdpClient>();
    nhdp2->AssignStreams(11);
    auto nhdp3 = apps.Get(2)->GetObject<NhdpClient>();
    nhdp2->AssignStreams(12);
    nhdp1->TraceConnect("HelloSend", "1", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp2->TraceConnect("HelloSend", "2", MakeCallback(&NhdpTestCase::HelloSend, this));
    nhdp1->TraceConnect("HelloRecv", "1", MakeCallback(&NhdpTestCase::HelloRecv, this));
    nhdp2->TraceConnect("HelloRecv", "2", MakeCallback(&NhdpTestCase::HelloRecv, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Enable sets the matrix loss to zero dB.  The transmit power in dBm is 16.026.  The
    // noise power in 20 MHz (default 11ax channel width for group addressed frames)
    // is approximately -101 dBm.  The default Wi-Fi noise figure is 7 dB, so the expected
    // SNR if zero loss is configured is 16.026 - (-101) - 7 = 110 dB for NHDP HELLOs.
    //
    // Therefore, to set the SNR around 0 dB for NHDP frames requires around 110 dB of loss.
    // However, for data frames to reach 0 dB SNR, the loss should be set to 104 dB because
    // the channel width is 80 MHz and data will be sent in 80 MHz.
    //
    // A 110 dB loss will result in a PER of about 0.1 for NHDP hello
    // A 104 dB loss will result in a data PER of about 0.52 and a NHDP PER of zero
    //
    // In this test we are concerned with NHDP only.  Start NHDP with the channel disabled.
    //   - at 5 seconds, enable the channel
    //   - at 6 and 8 seconds, check that HELLOs have been received and state is correct
    //   - at 10 seconds, disable the channel
    //   - at 15 seconds, check that lost link status are being circulated
    //   - at 20 seconds, check that things are removed
    //   - at 25 seconds, enable the channel
    //   - at 30 seconds, check that things are restored
    //
    Simulator::Schedule(Seconds(5), &NhdpTestCase::Enable, this, m_nodes.Get(0), m_nodes.Get(1));
    Simulator::Schedule(Seconds(5), &NhdpTestCase::Enable, this, m_nodes.Get(1), m_nodes.Get(2));

    // By time 7 seconds, we should have heard that there is a link and neighbor change;
    // the link state should be SYMMETRIC and the neighbor is symmetric.
    NeighborTuple ntAt7s{Ipv4Address("7.0.0.2")};
    ntAt7s.m_symmetric = true;
    Simulator::Schedule(Seconds(7), &NhdpTestCase::CheckNeighbor, this, ntAt7s);
    // at time 7, we should see heardTime of 12.6726s, symTime should be zero,
    // quality 1, m_pending false, m_lost false.  Some values are defaults and don't need setting
    LinkTuple ltAt7s{Ipv4Address("7.0.0.2")};
    ltAt7s.m_heardTime = Seconds(12.6726); // arrival of HELLO at 6.6726 + 6 seconds
    ltAt7s.m_symTime = Seconds(12.6726);   // arrival of HELLO at 6.6726 + 6 seconds
    ltAt7s.m_quality = 1;
    ltAt7s.m_pending = false;
    ltAt7s.m_expirationTime = Seconds(18.6726); // m_symTime + 6 seconds
    Simulator::Schedule(Seconds(7), &NhdpTestCase::CheckLink, this, ltAt7s);

    // No 2-hop changes should be seen
    Simulator::Schedule(Seconds(6), &NhdpTestCase::CheckTwoHopChangesSize, this, 0);

    // Print out the databases at time 6
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(6), &NhdpTestCase::Print, this, nhdp2);

    Simulator::Schedule(Seconds(9), &NhdpTestCase::ClearNeighborChanges, this);
    // There should not be neighbor changes since they were cleared at time 9
    Simulator::Schedule(Seconds(10), &NhdpTestCase::CheckNeighborChangesSize, this, 0);

    // There should be a two-hop change
    Simulator::Schedule(Seconds(10), &NhdpTestCase::CheckTwoHopChangesSize, this, 1);
    Simulator::Schedule(Seconds(10.1), &NhdpTestCase::ClearTwoHopChanges, this);

    Simulator::Schedule(Seconds(12), &NhdpTestCase::Disable, this, m_nodes.Get(0), m_nodes.Get(1));

    // After disabling, we should see the link first being advertised as symmetric
    // until that expires, then as lost until the expiration time expires (by 20 s)

    // There should be a two-hop change by time 18
    Simulator::Schedule(Seconds(18), &NhdpTestCase::CheckTwoHopChangesSize, this, 1);

    // Enable again after everything has been lost
    Simulator::Schedule(Seconds(25), &NhdpTestCase::Enable, this, m_nodes.Get(0), m_nodes.Get(1));

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.3"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(9));
    apps2.Stop(Seconds(11));

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(2));
    apps3.Start(Seconds(9));
    apps3.Stop(Seconds(11));

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 */
class FourNodeTestCase : public NhdpTestCase
{
  public:
    FourNodeTestCase(std::string name);

    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);

  protected:
    void DoSetup() override;
    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
};

FourNodeTestCase::FourNodeTestCase(std::string name)
    : NhdpTestCase(name)
{
}

void
FourNodeTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
                               true);
}

void
FourNodeTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
FourNodeTestCase::DoSetup()
{
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(4);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 100.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(m_matrixLossModel);
    // Create 100 dB loss on the diagonals so that each node has two neighbors
    m_matrixLossModel->SetDefaultLoss(0);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(2)->GetObject<MobilityModel>(),
                               100,
                               true);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("OfdmRate54Mbps"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
}

/**
 * @ingroup nhdp-tests
 * Configure NHDP on four node topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class FourNodeNhdpTestCase : public FourNodeTestCase
{
  public:
    FourNodeNhdpTestCase(std::string name);

  protected:
    void DoRun() override;
};

FourNodeNhdpTestCase::FourNodeNhdpTestCase(std::string name)
    : FourNodeTestCase(name)
{
}

void
FourNodeNhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(51)};

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Schedule losses
    // Simulator::Schedule(Seconds(3), &FourNodeTestCase::Disable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(19), &NhdpTestCase::Print, this, nhdp1);
    // Simulator::Schedule(Seconds(20), &FourNodeTestCase::Enable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(30), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(31),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(31),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(40), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(41),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(41),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(50), &FourNodeTestCase::Print, this, nhdp1);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class FourNodeOlsrTestCase : public FourNodeTestCase
{
  public:
    FourNodeOlsrTestCase(std::string name);

  protected:
    void DoRun() override;
};

FourNodeOlsrTestCase::FourNodeOlsrTestCase(std::string name)
    : FourNodeTestCase(name)
{
}

void
FourNodeOlsrTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &FourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &FourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &FourNodeTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class FourNodeOlsrv2TestCase : public FourNodeTestCase
{
  public:
    FourNodeOlsrv2TestCase(std::string name);

  protected:
    void DoRun() override;
};

FourNodeOlsrv2TestCase::FourNodeOlsrv2TestCase(std::string name)
    : FourNodeTestCase(name)
{
}

void
FourNodeOlsrv2TestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &FourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &FourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &FourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &FourNodeTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a five-node topology with two unequal hop count paths
 *
 * 3<-->4 <-> 5
 * |          |
 * |          |
 * 1 <------> 2
 */
class UnequalPathTestCase : public NhdpTestCase
{
  public:
    UnequalPathTestCase(std::string name);

    double LinkQualityCallback(Ptr<Packet> packet) const;
    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    void Change(Ptr<Node> a, Ptr<Node> b, double lossDb);

  protected:
    void DoSetup() override;

    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
    std::vector<NeighborTuple> m_neighborTupleVector;
};

UnequalPathTestCase::UnequalPathTestCase(std::string name)
    : NhdpTestCase(name)
{
}

double
UnequalPathTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double snrDb = RatioToDb(snrTag.Get());
    NS_LOG_INFO("SnrTag " << snrDb);
    double quality = 0;
    if (snrDb > 6)
    {
        quality = 1;
    }
    else if (snrDb > 3)
    {
        quality = 0.7;
    }
    else if (snrDb > 0)
    {
        quality = 0.5;
    }
    return quality;
}

void
UnequalPathTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
                               true);
}

void
UnequalPathTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
UnequalPathTestCase::Change(Ptr<Node> a, Ptr<Node> b, double lossDb)
{
    NS_LOG_INFO("Changing loss to " << lossDb << " on link between " << a->GetId() << " and "
                                    << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               lossDb,
                               true);
}

void
UnequalPathTestCase::DoSetup()
{
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(5);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 100.0, 0.0));
    positionAlloc->Add(Vector(50.0, 100.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    auto friisModel = CreateObject<FriisPropagationLossModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    friisModel->SetNext(m_matrixLossModel);
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(friisModel);
    // Isolate the two paths by adding 100 dB of loss for selected node pairs
    // * 3<-->4 <-> 5
    // * |          |
    // * |          |
    // * 1 <------> 2
    // node 1 (m_nodes.Get(0)) can talk to 2 and 3 but not 4 and 5
    m_matrixLossModel->SetDefaultLoss(0);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(4)->GetObject<MobilityModel>(),
                               100,
                               true);
    // node 2 (m_nodes.Get(1)) can talk to 1 and 5 but not 3 and 4
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(2)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    // node 3 (m_nodes.Get(2)) can talk to 1 and 4 but not 2 and 5
    m_matrixLossModel->SetLoss(m_nodes.Get(2)->GetObject<MobilityModel>(),
                               m_nodes.Get(4)->GetObject<MobilityModel>(),
                               100,
                               true);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HeMcs0"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class UnequalPathOlsrTestCase : public UnequalPathTestCase
{
  public:
    UnequalPathOlsrTestCase(std::string name);

  protected:
    void DoRun() override;
};

UnequalPathOlsrTestCase::UnequalPathOlsrTestCase(std::string name)
    : UnequalPathTestCase(name)
{
}

void
UnequalPathOlsrTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(90)};

    InternetStackHelper internet;
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->RegisterLinkQualityCallback(
        MakeCallback(&UnequalPathTestCase::LinkQualityCallback, this));
    // SNR above 1 will yield a quality of 1
    // SNR between 0 and 1 will yield a quality of 0.5
    // SNR below 0 will yield a quality of zero
    nhdp1->SetAttribute("HystAccept", DoubleValue(1));
    nhdp1->SetAttribute("HystReject", DoubleValue(0.5));
    nhdp1->SetAttribute("InitialPending", BooleanValue(true));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable alternate path
    Simulator::Schedule(Seconds(0),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));

    // The baseline SNR without additional path loss impairiment
    // between link 0 and 1 is 23.3027 dB
    // Force the SNR on the link from 0 to 1 to the value of -5 dB
    // at time 0 seconds.  This will force a routing change.
    Simulator::Schedule(Seconds(0),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        28.3027);

    // No neighbor relationships should be found before the link is enabled.
    Simulator::Schedule(Seconds(6.99), &UnequalPathTestCase::CheckNeighborChangesSize, this, 0);

    // At time 7s, bring up the SNR to just under 0 dB.  Even though SNR
    // is strong enough, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(7),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.31);

    // At time 9s, after a neighbor HELLO was received, check that we created neighbor
    // state (not symmetric) to one neighbor.  There should be no link to address 7.0.0.2.
    Ipv4Address neighborAddr{"7.0.0.2"};
    NeighborTuple checkTuple{neighborAddr};
    checkTuple.m_symmetric = false;
    Simulator::Schedule(Seconds(9), &UnequalPathTestCase::CheckNeighbor, this, checkTuple);
    Simulator::Schedule(Seconds(9), &UnequalPathTestCase::CheckNeighborChangesSize, this, 1);
    Simulator::Schedule(Seconds(9) + TimeStep(1), &UnequalPathTestCase::ClearNeighborChanges, this);
    Simulator::Schedule(Seconds(9), &UnequalPathTestCase::CheckLinkChangesSize, this, 1);

    // At time 17s, bring up the SNR to  0.2 dB.  Even though SNR
    // is strong enough for reception, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(17),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.11);

    // At time 27s, bring up the SNR to  0.8 dB.  Even though quality is above
    // HYST_REJECT, the link quality will still block the HELLO adjacency
    Simulator::Schedule(Seconds(27),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 37s, bring up the SNR to  >1  dB.  This should form adjacency and add
    // a 2-hop neighbor
    Simulator::Schedule(Seconds(37),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22);

    // At time 47s, drop the SNR to  0.8 dB.  Quality will still be above HYST_REJECT
    Simulator::Schedule(Seconds(47),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 57s, drop the SNR to  0 dB.  Quality falls below HYST_REJECT
    Simulator::Schedule(Seconds(57),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.3027);

    // At time 67s, up the SNR to  0.8 dB.  No HELLO adjacency.
    Simulator::Schedule(Seconds(67),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 77s, bring up the SNR to  >1  dB.  Even though SNR
    // is strong enough, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(77),
                        &UnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &UnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &UnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &UnequalPathTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.5"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(8));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(4));
    apps3.Start(Seconds(8));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class UnequalPathOlsrv2TestCase : public UnequalPathTestCase
{
  public:
    UnequalPathOlsrv2TestCase(std::string name);

  protected:
    void DoRun() override;
};

UnequalPathOlsrv2TestCase::UnequalPathOlsrv2TestCase(std::string name)
    : UnequalPathTestCase(name)
{
}

void
UnequalPathOlsrv2TestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &UnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &UnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &UnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &UnequalPathTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * @ingroup nhdp-tests
 * TestSuite for module nhdp
 */
class NhdpTestSuite : public TestSuite
{
  public:
    NhdpTestSuite();
};

NhdpTestSuite::NhdpTestSuite()
    : TestSuite("nhdp-system", Type::SYSTEM)
{
    AddTestCase(new DisableLinkTestCase("Checking state transitions with disabled link"),
                TestCase::Duration::QUICK);
    AddTestCase(new DisableLinkWithQualityTestCase(
                    "Two node with link quality test checking state transitions"),
                TestCase::Duration::QUICK);
    AddTestCase(new ThreeNodeNhdpOlsrTestCase("Three node test checking state transitions"),
                TestCase::Duration::QUICK);
    AddTestCase(new FourNodeNhdpTestCase("Four node matrix test with losses"),
                TestCase::Duration::QUICK);
    AddTestCase(new FourNodeOlsrTestCase("Four node matrix test with OLSRv1"),
                TestCase::Duration::QUICK);
    AddTestCase(new FourNodeOlsrv2TestCase("Four node matrix test with OLSRv2"),
                TestCase::Duration::QUICK);
    AddTestCase(new UnequalPathOlsrTestCase("Unequal path matrix test with OLSRv1"),
                TestCase::Duration::QUICK);
    AddTestCase(new UnequalPathOlsrv2TestCase("Unequal path matrix test with OLSRv2"),
                TestCase::Duration::QUICK);
}

/**
 * @ingroup nhdp-tests
 * Static variable for test initialization
 */
static NhdpTestSuite s_nhdpTestSuite;
