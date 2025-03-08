/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only and NIST-Software
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

/* These classes implement the NHDP neighbor discovery protocol.  See
 * https://datatracker.ietf.org/doc/html/rfc6130 for more info */

#include "nhdp-client.h"

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/ipv4.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/string.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/udp-socket.h"
#include "ns3/uinteger.h"

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NhdpClient");

namespace nhdp
{

/* From RFC 5498 */
static const Ipv4Address LL_MANET_ROUTERS_IPV4("224.0.0.109");
/* From RFC 5498 */
static const Ipv6Address LL_MANET_ROUTERS_IPV6("FF02:0:0:0:0:0:0:6D");

/* From RFC 5498 */
static const uint32_t UDP_PORT_MANET = 269;

static const Time DEFAULT_HELLO_INTERVAL = Seconds(2);
static const Time DEFAULT_HELLO_MIN_INTERVAL = DEFAULT_HELLO_INTERVAL / 4;
static const Time DEFAULT_REFRESH_INTERVAL = DEFAULT_HELLO_INTERVAL;

static const Time DEFAULT_H_HOLD_TIME = 3 * DEFAULT_REFRESH_INTERVAL;
static const Time DEFAULT_L_HOLD_TIME = DEFAULT_H_HOLD_TIME;

static const double DEFAULT_HYST_ACCEPT = 1.0;
static const double DEFAULT_HYST_REJECT = 0.0;
static const double DEFAULT_INITIAL_QUALITY = 1.0;
static const bool DEFAULT_INITIAL_PENDING = false;

static const Time DEFAULT_HP_MAX_JITTER = DEFAULT_HELLO_INTERVAL / 4;
static const Time DEFAULT_HT_MAX_JITTER = DEFAULT_HP_MAX_JITTER;

static const Time DEFAULT_N_HOLD_TIME = DEFAULT_L_HOLD_TIME;
static const Time DEFAULT_I_HOLD_TIME = DEFAULT_N_HOLD_TIME;

static const Time EXPIRED = Seconds(0);

NS_OBJECT_ENSURE_REGISTERED(NhdpClient);

NhdpClient::NhdpClient()
{
    NS_LOG_FUNCTION(this);
    m_rng = CreateObject<UniformRandomVariable>();
}

TypeId
NhdpClient::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::nhdp::NhdpClient")
            .SetParent<Application>()
            .AddConstructor<NhdpClient>()

            .AddAttribute("Address",
                          "Multicast address to use.",
                          Ipv4AddressValue(LL_MANET_ROUTERS_IPV4),
                          MakeIpv4AddressAccessor(&NhdpClient::m_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("Port",
                          "UDP port to use.",
                          UintegerValue(UDP_PORT_MANET),
                          MakeUintegerAccessor(&NhdpClient::m_port),
                          MakeUintegerChecker<uint32_t>())

            /* Interface Parameters */
            .AddAttribute("HelloInterval",
                          "Default maximum interval between HELLOs on a MANET interface.",
                          TimeValue(DEFAULT_HELLO_INTERVAL),
                          MakeTimeAccessor(&NhdpClient::m_helloInterval),
                          MakeTimeChecker())
            .AddAttribute("HelloMinInterval",
                          "Default minimum interval between HELLOs on a MANET interface.",
                          TimeValue(DEFAULT_HELLO_MIN_INTERVAL),
                          MakeTimeAccessor(&NhdpClient::m_helloMinInterval),
                          MakeTimeChecker())
            .AddAttribute("RefreshInterval",
                          "Default maximum interval between advertisements of each 1-hop neighbor "
                          "in a HELLO.",
                          TimeValue(DEFAULT_REFRESH_INTERVAL),
                          MakeTimeAccessor(&NhdpClient::m_refreshInterval),
                          MakeTimeChecker())

            .AddAttribute("LHoldTime",
                          "Time to advertise former 1-hop neighbor addresses as lost for removal "
                          "from Link Set.",
                          TimeValue(DEFAULT_L_HOLD_TIME),
                          MakeTimeAccessor(&NhdpClient::m_lHoldTime),
                          MakeTimeChecker())
            .AddAttribute(
                "HHoldTime",
                "Time advertised for the validity of messages sent from a MANET interface.",
                TimeValue(DEFAULT_H_HOLD_TIME),
                MakeTimeAccessor(&NhdpClient::m_hHoldTime),
                MakeTimeChecker())

            .AddAttribute("HystAccept",
                          "Link quality threshold at or above which a link becomes usable.",
                          DoubleValue(DEFAULT_HYST_ACCEPT),
                          MakeDoubleAccessor(&NhdpClient::m_hystAccept),
                          MakeDoubleChecker<double>(0.0, 1.0))
            .AddAttribute("HystReject",
                          "Link quality threshold below which a link becomes unusable.",
                          DoubleValue(DEFAULT_HYST_REJECT),
                          MakeDoubleAccessor(&NhdpClient::m_hystReject),
                          MakeDoubleChecker<double>(0.0, 1.0))
            .AddAttribute("InitialQuality",
                          "The initial quality of a newly identified link.",
                          DoubleValue(DEFAULT_INITIAL_QUALITY),
                          MakeDoubleAccessor(&NhdpClient::m_initialQuality),
                          MakeDoubleChecker<double>(0.0, 1.0))
            .AddAttribute("InitialPending",
                          "If true, newly identified links are considered pending, and are not "
                          "usable until quality reaches HystAccept.",
                          BooleanValue(DEFAULT_INITIAL_PENDING),
                          MakeBooleanAccessor(&NhdpClient::m_initialPending),
                          MakeBooleanChecker())

            .AddAttribute("HPMaxJitter",
                          "MAXJITTER used in periodically generated HELLO messages",
                          TimeValue(DEFAULT_HP_MAX_JITTER),
                          MakeTimeAccessor(&NhdpClient::m_hpMaxJitter),
                          MakeTimeChecker())
            .AddAttribute("HTMaxJitter",
                          "MAXJITTER used in externally triggered HELLO messages",
                          TimeValue(DEFAULT_HT_MAX_JITTER),
                          MakeTimeAccessor(&NhdpClient::m_htMaxJitter),
                          MakeTimeChecker())

            /* Router parameters */
            .AddAttribute("NHoldTime",
                          "Time to advertise former 1-hop neighbor addresses as lost for removal "
                          "from TwoHopSets.",
                          TimeValue(DEFAULT_N_HOLD_TIME),
                          MakeTimeAccessor(&NhdpClient::m_nHoldTime),
                          MakeTimeChecker())
            .AddAttribute("IHoldTime",
                          "Time to record recently used local interface addresses.",
                          TimeValue(DEFAULT_I_HOLD_TIME),
                          MakeTimeAccessor(&NhdpClient::m_iHoldTime),
                          MakeTimeChecker())
            .AddTraceSource("NeighborChange",
                            "Notification that neighbor information base changed",
                            MakeTraceSourceAccessor(&NhdpClient::m_neighborChangeTrace),
                            "ns3::nhdp::NhdpClient::NeighborChangeTracedCallback")
            .AddTraceSource("LinkChange",
                            "Notification that link information base changed",
                            MakeTraceSourceAccessor(&NhdpClient::m_linkChangeTrace),
                            "ns3::nhdp::NhdpClient::LinkChangeTracedCallback")
            .AddTraceSource("TwoHopChange",
                            "Notification that two-hop information base changed",
                            MakeTraceSourceAccessor(&NhdpClient::m_twoHopChangeTrace),
                            "ns3::nhdp::NhdpClient::TwoHopChangeTracedCallback")
            .AddTraceSource("HelloMessageSend",
                            "Trace of (modifiable) PbbMessage before it is sent out as HELLO",
                            MakeTraceSourceAccessor(&NhdpClient::m_helloMessageSendTrace),
                            "ns3::nhdp::NhdpClient::HelloMessageSendTracedCallback")
            .AddTraceSource("HelloMessageRecv",
                            "Trace of PbbMessage HELLO after it has been processed by NHDP",
                            MakeTraceSourceAccessor(&NhdpClient::m_helloMessageRecvTrace),
                            "ns3::nhdp::NhdpClient::HelloMessageRecvTracedCallback")
            .AddTraceSource("Tx",
                            "Trace of Packet just before sending to UDP socket",
                            MakeTraceSourceAccessor(&NhdpClient::m_txTrace),
                            "ns3::Packet::TracedCallback");
    return tid;
}

void
NhdpClient::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_rng->SetAttribute("Max", DoubleValue(DEFAULT_HP_MAX_JITTER.GetSeconds()));
    if (!m_recvSocket)
    {
        m_recvSocket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
        m_recvSocket->SetAllowBroadcast(true);
        InetSocketAddress inetAddr(Ipv4Address::GetAny(), UDP_PORT_MANET);
        m_recvSocket->SetRecvCallback(MakeCallback(&NhdpClient::HandleRecv, this));
        if (m_recvSocket->Bind(inetAddr))
        {
            NS_FATAL_ERROR("Failed to bind() NHDP receive socket");
        }
        m_recvSocket->SetRecvPktInfo(true);
        m_recvSocket->ShutdownSend();
    }
    Application::DoInitialize();
}

int64_t
NhdpClient::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    auto currentStream = stream;
    m_rng->SetStream(currentStream++);
    currentStream += Application::AssignStreams(currentStream);
    return (currentStream - stream);
}

const std::map<Ipv4Address, NeighborTuple>&
NhdpClient::GetNeighborInfoBase() const
{
    return m_neighborInfoBase;
}

const std::map<Ipv4Address, LinkTuple>&
NhdpClient::GetLinkInfoBase() const
{
    return m_linkInfoBase;
}

const std::map<std::pair<Ipv4Address, Ipv4Address>, TwoHopTuple>&
NhdpClient::GetTwoHopInfoBase() const
{
    return m_twoHopInfoBase;
}

/* Local Information Base Methods */
void
NhdpClient::MarkIfaceNonManet(uint32_t ifaddr)
{
    m_nonManetSet.insert(ifaddr);
}

/* Removed Interface Address Set */
/*
void
NhdpClient::AddRemovedInterfaceAddress (Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION(this << address);

  Ptr<RemovedInterfaceAddressTuple> newTuple
    = Create<RemovedInterfaceAddressTuple>();
  newTuple->addr = address;
  newTuple->time = Simulator::Now();

  m_removedInterfaceAddressSet.push_back(newTuple);
  std::push_heap(
      m_removedInterfaceAddressSet.begin(),
      m_removedInterfaceAddressSet.end(),
      TimeCompare<RemovedInterfaceAddressTuple>());
}
*/

/* End information base methods */

/*
void
NhdpClient::QueueMessage(Ptr<PbbMessage> message)
{
  NS_LOG_FUNCTION(this << message);
  m_messages.push(message);
}

void
NhdpClient::RegisterMessageCallback(uint8_t messageType,
    Callback<PbbMessage> cb)
{
  NS_LOG_FUNCTION(this << messageType);
  NS_ASSERT (!cb.IsNull ());
  m_callbacks.insert (std::pair<uint8_t, Callback<PbbMessage> > (messageType, cb));
}
*/

void
NhdpClient::RegisterLinkQualityCallback(Callback<double, Ptr<Packet>> cb)
{
    NS_LOG_FUNCTION(this);
    m_linkQualityCallback = cb;
}

void
NhdpClient::HandleRecv(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    Address from;
    Ptr<Packet> packet = socket->RecvFrom(from);
    Ipv4PacketInfoTag tag;
    auto found = packet->RemovePacketTag(tag);
    NS_ASSERT_MSG(found, "Did not find Ipv4PacketInfoTag");
    std::optional<double> quality;
    if (!m_linkQualityCallback.IsNull())
    {
        quality = m_linkQualityCallback(packet);
        NS_ABORT_MSG_IF(quality.value() < 0 || quality.value() > 1,
                        "Link quality not between [0,1]");
        NS_LOG_DEBUG("Receive HELLO with quality "
                     << quality.value() << " to: " << tag.GetAddress()
                     << " from: " << InetSocketAddress::ConvertFrom(from).GetIpv4());
    }
    else
    {
        NS_LOG_DEBUG("Receive HELLO (link quality callback disabled) to: "
                     << tag.GetAddress()
                     << " from: " << InetSocketAddress::ConvertFrom(from).GetIpv4());
    }
    PbbPacket pbb;
    packet->RemoveHeader(pbb);
    int16_t seq(-1);
    if (pbb.HasSequenceNumber())
    {
        seq = pbb.GetSequenceNumber();
    }
    NS_LOG_DEBUG("Pbb message size " << pbb.MessageSize() << " TLV size " << pbb.TlvSize()
                                     << " seq. no. " << seq);
    // TODO:  Clarify whether a NHDP packet can have more than one message (HELLO message)
    NS_ASSERT_MSG(pbb.MessageSize() == 1, "There should be one message in a HELLO");
    auto neighborAddr = HandlePbbMessage(pbb.MessageFront(), quality);

    // Pass HELLO to other protocols that are clients of NHDP
    m_helloMessageRecvTrace(pbb.MessageFront(), neighborAddr, quality);
}

Ipv4Address
NhdpClient::HandlePbbMessage(Ptr<PbbMessage> msg, std::optional<double> quality)
{
    NS_LOG_FUNCTION(this << msg << quality.has_value());
    NS_LOG_INFO("PbbMessage TLV size " << msg->TlvSize() << " address block size "
                                       << msg->AddressBlockSize() << " type " << +msg->GetType()
                                       << " hops " << msg->HasHopLimit() << " seq "
                                       << msg->HasSequenceNumber());
    NS_ASSERT_MSG(msg->GetType() == 0, "HELLO should be message type 0");

    // Process message TLVs; these are attributes that pertain to the overall message
    // INTERVAL_TIME and VALIDITY_TIME are message TLVs

    // OLSRv2 will add the MPR_WILLING TLV here

    // For now, assume all routers have the same interval and validity time; therefore,
    // no need yet to include these TLVs

    // Process address blocks
    // OLSRv2 will add the MPR, LINK_METRIC, and NBR_ADDR_TYPE address blocks

    Ipv4Address neighborIpv4Addr;
    for (auto it = msg->AddressBlockBegin(); it != msg->AddressBlockEnd(); ++it)
    {
        auto addressBlock = *it;
        auto addressTlv = addressBlock->TlvFront();
        NS_LOG_INFO("Address block size " << addressBlock->AddressSize() << " TLV type "
                                          << +addressTlv->GetType());
        if (addressTlv->GetType() == ADDR_TLV_LOCAL_IF)
        {
            neighborIpv4Addr = HandleLocalAddressBlock(addressBlock);
        }
        else if (addressTlv->GetType() == ADDR_TLV_LINK_STATUS)
        {
            HandleLinkStatusAddressBlock(addressBlock, neighborIpv4Addr, quality);
        }
        else if (addressTlv->GetType() == ADDR_TLV_OTHER_NEIGHB)
        {
            NS_FATAL_ERROR("OTHER_NEIGHB type not yet supported");
            // HandleOtherNeighbAddressBlock(addressBlock, addressTlv, neighborIpv4Addr);
        }
        else
        {
            NS_LOG_DEBUG("Unknown type " << +addressTlv->GetType());
        }
    }

    // Perform housekeeping (TODO: Move to HandleRecv()?)

    // Update Lost Neighbor Set (RFC 6130 Sec. 12.4)
    for (auto& itAddr : m_lostAddressList)
    {
        auto itNeigh = m_lostNeighborSet.find(itAddr);
        if (itNeigh != m_lostNeighborSet.end())
        {
            NS_LOG_DEBUG("Inserting lost address " << itAddr << " into lost neighbor set");
            LostNeighborTuple lostNeighborTuple;
            lostNeighborTuple.m_neighborAddr = itAddr;
            lostNeighborTuple.m_expirationTime = Simulator::Now() + m_nHoldTime;
            m_lostNeighborSet.emplace(itAddr, lostNeighborTuple);
        }
    }
    m_lostAddressList.clear();
    // Remove expired Lost Neighbors
    for (auto it = m_lostNeighborSet.begin(); it != m_lostNeighborSet.end();)
    {
        if (it->second.m_expirationTime <= Simulator::Now())
        {
            NS_LOG_DEBUG("Erasing expired lost neighbor set entry for " << it->first);
            it = m_lostNeighborSet.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // Remove expired two hop entries
    for (auto it = m_twoHopInfoBase.begin(); it != m_twoHopInfoBase.end();)
    {
        if (it->second.m_expirationTime <= Simulator::Now())
        {
            NS_LOG_DEBUG("Erasing expired two hop entry for " << it->second.m_twoHopAddr << " via "
                                                              << it->second.m_neighborAddrList[0]);
            m_twoHopChangeTrace(TwoHopStatus::REMOVED, it->second);
            it = m_twoHopInfoBase.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return neighborIpv4Addr;
}

Ipv4Address
NhdpClient::HandleLocalAddressBlock(Ptr<PbbAddressBlock> addressBlock)
{
    NS_LOG_FUNCTION(this << addressBlock);
    NS_ASSERT_MSG(addressBlock->TlvSize() == 1,
                  "Expected AddressBlock TLV size of 1, got " << addressBlock->TlvSize());
    auto addressTlv = addressBlock->TlvFront();
    Ipv4Address neighborIpv4Addr;
    for (auto it = addressBlock->AddressBegin(); it != addressBlock->AddressEnd(); ++it)
    {
        auto addr = (*it);
        NS_ASSERT_MSG(Ipv4Address::IsMatchingType(addr), "Only supporting IPv6 for now");
        neighborIpv4Addr = Ipv4Address::ConvertFrom(addr);
        auto itNeigh = m_neighborInfoBase.find(neighborIpv4Addr);
        if (itNeigh == m_neighborInfoBase.end())
        {
            NS_LOG_INFO("Creating new NeighborTuple and LinkTuple to " << neighborIpv4Addr);
            NeighborTuple neighborTuple(neighborIpv4Addr);
            m_neighborInfoBase.emplace(neighborIpv4Addr, neighborTuple);
            m_neighborChangeTrace(NeighborStatus::NEW, neighborTuple);
            LinkTuple linkTuple(neighborIpv4Addr, 0);
            const auto oldStatus = linkTuple.GetLinkStatus();
            linkTuple.m_heardTime = Simulator::Now() + m_hHoldTime;
            linkTuple.m_expirationTime = Simulator::Now() + m_hHoldTime;
            m_linkInfoBase.emplace(neighborIpv4Addr, linkTuple);
            m_linkChangeTrace(oldStatus, linkTuple);
        }
        else
        {
            NS_LOG_DEBUG("Heard from an existing neighbor " << neighborIpv4Addr);
        }
    }
    return neighborIpv4Addr;
}

void
NhdpClient::HandleLinkStatusAddressBlock(Ptr<PbbAddressBlock> addressBlock,
                                         Ipv4Address neighborIpv4Addr,
                                         std::optional<double> quality)
{
    NS_LOG_FUNCTION(this << addressBlock << neighborIpv4Addr << quality.has_value());
    auto numAddresses = addressBlock->AddressSize();
    auto numTlvs = addressBlock->TlvSize();
    NS_ASSERT_MSG(numTlvs > 0, "Error, no address block TLVs");
    NS_LOG_DEBUG("Number of addresses " << numAddresses << " address TLVs " << numTlvs);
    // Process TLVs.  There are addressBlock->AddressSize() addresses, and at least one TLV
    // containing the values corresponding to those addresses.  If there is only one address
    // TLV, the outcome is simple-- all addresses in the block have the same link status type.
    // However, if there is more than one TLV, we need to associate the addresses with the
    // link status types.  Store these in a vector.
    std::vector<uint8_t> linkStatusValues;
    auto addressesRemaining = numAddresses;
    for (auto it = addressBlock->TlvBegin(); it != addressBlock->TlvEnd(); ++it)
    {
        auto addrTlv = (*it);
        NS_ASSERT_MSG(addrTlv->GetValue().GetSize() == 1, "TLV should have one byte of value data");
        auto value = addrTlv->GetValue().Begin().ReadU8();
        int numAddressesThisTlv{0};
        if (addrTlv->HasIndexStop())
        {
            numAddressesThisTlv = addrTlv->GetIndexStop() - addrTlv->GetIndexStart() + 1;
        }
        else
        {
            numAddressesThisTlv = addressesRemaining;
        }
        for (int j = 0; j < numAddressesThisTlv; j++)
        {
            linkStatusValues.push_back(value);
            addressesRemaining--;
        }
    }
    NS_ASSERT_MSG(addressesRemaining == 0, "Logic error in linkStatusValues assignment");
    NS_ASSERT_MSG(linkStatusValues.size() == numAddresses,
                  "Logic error in linkStatusValues assignment");
    Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
    std::vector<Ipv4Address>
        lostSymmetricNeighbor; // keep track of any neighbors that lost symmetric status
    std::size_t k{0};
    auto qualityValue = quality.has_value() ? quality.value() : 1.0;
    for (auto it = addressBlock->AddressBegin(); it != addressBlock->AddressEnd(); ++it)
    {
        auto value = linkStatusValues[k];
        auto addr = (*it);
        NS_ASSERT_MSG(value < 3, "Value " << +value << " unsupported");
        NS_ASSERT_MSG(Ipv4Address::IsMatchingType(addr), "Only supporting IPv6 for now");
        auto ipv4Addr = Ipv4Address::ConvertFrom(addr);
        if (ipv4Addr == m_localIpv4Address)
        {
            if (value == ADDR_TLV_LINK_STATUS_HEARD || value == ADDR_TLV_LINK_STATUS_SYMMETRIC)
            {
                std::string valueStr =
                    (value == ADDR_TLV_LINK_STATUS_HEARD) ? "HEARD" : "SYMMETRIC";
                NS_LOG_DEBUG("Neighbor " << neighborIpv4Addr << " lists my address as "
                                         << valueStr);
                if ((!m_initialPending || qualityValue >= m_hystAccept) ||
                    qualityValue >= m_hystReject)
                {
                    auto itNeigh = m_neighborInfoBase.find(neighborIpv4Addr);
                    if (!itNeigh->second.m_symmetric)
                    {
                        // change neighbor to symmetric
                        NS_LOG_DEBUG("Changing neighbor " << neighborIpv4Addr
                                                          << " from HEARD to SYMMETRIC");
                        itNeigh->second.m_symmetric = true;
                        m_neighborChangeTrace(NeighborStatus::MODIFIED, itNeigh->second);
                    }
                    auto itLink = m_linkInfoBase.find(neighborIpv4Addr);
                    NS_ASSERT_MSG(itLink != m_linkInfoBase.end(), "Error: LinkTuple not found");
                    const auto oldStatus = itLink->second.GetLinkStatus();
                    itLink->second.m_symTime = Simulator::Now() + m_hHoldTime;
                    itLink->second.m_expirationTime = Simulator::Now() + m_hHoldTime;
                    NS_LOG_DEBUG("Changing link sym time to "
                                 << itLink->second.m_symTime.GetSeconds());
                    if (qualityValue < m_hystReject)
                    {
                        // Force a move to LOST state
                        itLink->second.m_symTime = Simulator::Now();
                        itLink->second.m_heardTime = Simulator::Now();
                        itLink->second.m_expirationTime = Simulator::Now();
                    }
                    if (oldStatus != itLink->second.GetLinkStatus())
                    {
                        m_linkChangeTrace(oldStatus, itLink->second);
                    }
                }
            }
            else if (value == ADDR_TLV_LINK_STATUS_LOST)
            {
                // Received hello from neighbor who considers me as lost
                auto itNeigh = m_neighborInfoBase.find(neighborIpv4Addr);
                if (itNeigh != m_neighborInfoBase.end())
                {
                    auto itLink = m_linkInfoBase.find(neighborIpv4Addr);
                    if (qualityValue < m_hystReject ||
                        (m_initialPending && qualityValue < m_hystAccept))
                    {
                        itLink->second.m_pending = true;
                    }
                    NS_ASSERT_MSG(itLink != m_linkInfoBase.end(),
                                  "Expected link tuple to be present");
                    if (itNeigh->second.m_symmetric)
                    {
                        NS_LOG_DEBUG("Setting neighbor " << itLink->second.m_neighborAddrList[0]
                                                         << " to not symmetric");
                        itNeigh->second.m_symmetric = false;
                        m_neighborChangeTrace(NeighborStatus::MODIFIED, itNeigh->second);
                        lostSymmetricNeighbor.push_back(neighborIpv4Addr);
                    }
                    const auto oldStatus = itLink->second.GetLinkStatus();
                    itLink->second.m_heardTime = Simulator::Now() + m_hHoldTime;
                    itLink->second.m_expirationTime = Simulator::Now() + m_hHoldTime;
                    m_linkChangeTrace(oldStatus, itLink->second);
                }
                else
                {
                    NS_LOG_INFO("Creating new NeighborTuple and LinkTuple to " << neighborIpv4Addr);
                    NeighborTuple neighborTuple(neighborIpv4Addr);
                    m_neighborInfoBase.emplace(neighborIpv4Addr, neighborTuple);
                    m_neighborChangeTrace(NeighborStatus::NEW, neighborTuple);
                    auto it = m_linkInfoBase.find(neighborIpv4Addr);
                    if (it == m_linkInfoBase.end())
                    {
                        LinkTuple linkTuple(neighborIpv4Addr, 0);
                        if (qualityValue < m_hystReject ||
                            (m_initialPending && qualityValue < m_hystAccept))
                        {
                            linkTuple.m_pending = true;
                        }
                        const auto oldStatus = linkTuple.GetLinkStatus();
                        linkTuple.m_heardTime = Simulator::Now() + m_hHoldTime;
                        linkTuple.m_expirationTime = Simulator::Now() + m_hHoldTime;
                        m_linkInfoBase.emplace(neighborIpv4Addr, linkTuple);
                        m_linkChangeTrace(oldStatus, linkTuple);
                    }
                    else
                    {
                        if (qualityValue < m_hystReject ||
                            (m_initialPending && qualityValue < m_hystAccept))
                        {
                            it->second.m_pending = true;
                        }
                        const auto oldStatus = it->second.GetLinkStatus();
                        it->second.m_heardTime = Simulator::Now() + m_hHoldTime;
                        it->second.m_expirationTime = Simulator::Now() + m_hHoldTime;
                        m_linkChangeTrace(oldStatus, it->second);
                    }
                }
            }
        }
        else
        {
            // Neighbor is reporting a link to a 2-hop neighbor
            bool removeIfFound{false};
            if (value == ADDR_TLV_LINK_STATUS_LOST)
            {
                NS_LOG_DEBUG("Two hop neighbor " << ipv4Addr << " reported as LOST by "
                                                 << neighborIpv4Addr);
                removeIfFound = true;
            }
            else if (value == ADDR_TLV_LINK_STATUS_HEARD)
            {
                NS_LOG_DEBUG("Two hop neighbor " << ipv4Addr << " reported as HEARD by "
                                                 << neighborIpv4Addr);
                removeIfFound = true;
            }
            else if (value == ADDR_TLV_LINK_STATUS_SYMMETRIC)
            {
                auto key = std::make_pair(neighborIpv4Addr, ipv4Addr);
                auto itTwoHop = m_twoHopInfoBase.find(key);
                if (itTwoHop == m_twoHopInfoBase.end())
                {
                    TwoHopTuple twoHopTuple(neighborIpv4Addr, ipv4Addr);
                    twoHopTuple.m_expirationTime = Simulator::Now() + m_hHoldTime;
                    m_twoHopInfoBase.emplace(key, twoHopTuple);
                    m_twoHopChangeTrace(TwoHopStatus::NEW, twoHopTuple);
                    NS_LOG_INFO("Creating new TwoHopTuple to " << ipv4Addr << " via "
                                                               << neighborIpv4Addr);
                }
                else
                {
                    NS_LOG_DEBUG("Updating TwoHopTuple to " << ipv4Addr << " via "
                                                            << neighborIpv4Addr);
                    itTwoHop->second.m_expirationTime = Simulator::Now() + m_hHoldTime;
                }
            }
            if (removeIfFound)
            {
                auto key = std::make_pair(neighborIpv4Addr, ipv4Addr);
                auto itTwoHop = m_twoHopInfoBase.find(key);
                if (itTwoHop != m_twoHopInfoBase.end())
                {
                    m_twoHopChangeTrace(TwoHopStatus::REMOVED, itTwoHop->second);
                    NS_LOG_INFO("Removing TwoHopTuple to " << ipv4Addr << " via "
                                                           << neighborIpv4Addr);
                    m_twoHopInfoBase.erase(itTwoHop);
                }
            }
        }
        k++;
    }
    // If any 1-hop neighbors transitioned from symmetric to lost or heard, remove their 2-hop
    // neighbors
    for (const auto& it : lostSymmetricNeighbor)
    {
        for (auto itTuple = m_twoHopInfoBase.begin(); itTuple != m_twoHopInfoBase.end();)
        {
            auto key = itTuple->first;
            if (key.first == it)
            {
                m_twoHopChangeTrace(TwoHopStatus::REMOVED, itTuple->second);
                NS_LOG_INFO("Removing TwoHopTuple to " << itTuple->second.m_twoHopAddr << " via "
                                                       << it);
                itTuple = m_twoHopInfoBase.erase(itTuple);
            }
            else
            {
                ++itTuple;
            }
        }
    }
}

void
NhdpClient::DoDispose()
{
    NS_LOG_FUNCTION(this);
    if (m_recvSocket)
    {
        m_recvSocket->Close();
        m_recvSocket = nullptr;
    }
    for (auto& [socket, addr] : m_socketAddresses)
    {
        socket->Close();
    }
    m_socketAddresses.clear();
    Application::DoDispose();
}

void
NhdpClient::StartApplication()
{
    NS_LOG_FUNCTION(this);

    Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
    NS_ASSERT(ipv4);

    for (uint32_t i = 0; i < ipv4->GetNInterfaces(); i++)
    {
        if (m_nonManetSet.find(i) != m_nonManetSet.end())
        {
            continue;
        }

        Ipv4Address address = ipv4->GetAddress(i, 0).GetLocal();

        if (address == Ipv4Address::GetLoopback())
        {
            continue;
        }

        Ptr<Socket> socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
        socket->SetAllowBroadcast(true);
        socket->Bind(InetSocketAddress(address, m_port));
        socket->Connect(InetSocketAddress(LL_MANET_ROUTERS_IPV4, m_port));

        m_socketAddresses[socket] = address;
        ScheduleHello(socket);
    }
    m_running = true;
}

void
NhdpClient::StopApplication()
{
    NS_LOG_FUNCTION(this);

    for (auto& [socket, address] : m_socketAddresses)
    {
        socket->Close();
        socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    }
    m_socketAddresses.clear();
    m_running = false;
}

void
NhdpClient::ScheduleHello(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    /* TODO: Should be able to store a helloInterval on a per-device basis */
    Time time = m_helloInterval - Seconds(m_rng->GetValue());
    Simulator::Schedule(time, &NhdpClient::SendHello, this, socket);
}

void
NhdpClient::SendHello(Ptr<Socket> socket)
{
    if (!m_running)
    {
        return;
    }

    NS_LOG_FUNCTION(this << m_socketAddresses[socket]);

    PbbPacket pbb;
    Ptr<PbbMessage> message = Create<PbbMessageIpv4>();
    pbb.MessagePushBack(message);

    // Add Message TLVs here (VALIDITY_TIME and INTERVAL_TIME).  Other protocols
    // (e.g., OLSRv2) will add message TLVs (e.g., MPR_WILLING).

    // Next, add Address Blocks.  Other protocols (e.g., OLSRv2) will add address
    // blocks here (e.g., LINK_METRIC, MPR, NBR_ADDR_TYPE).

    if (!m_localAddrBlock)
    {
        m_localAddrBlock = BuildLocalAddressBlock(socket);
    }
    NS_ASSERT_MSG(m_localAddrBlock, "Local address block is required");
    message->AddressBlockPushBack(m_localAddrBlock);

    auto addrBlock = BuildLinkStatusAddressBlock(socket);
    if (addrBlock)
    {
        NS_LOG_DEBUG("Adding LinkStatus Address Block");
        message->AddressBlockPushBack(addrBlock);
    }
    else
    {
        NS_LOG_DEBUG("Not sending an empty LinkStatus address block");
    }

    // Add any queued messages
    /*
    while (!m_messages.empty ())
      {
        pbb.MessagePushBack (m_messages.front ());
        m_messages.pop ();
      }
    */

    // Give access to the PbbMessage to other protocols that may wish to extend it

    uint32_t addressBlockSize [[maybe_unused]] = message->AddressBlockSize();
    m_helloMessageSendTrace(message);
    if (message->AddressBlockSize() > addressBlockSize)
    {
        NS_LOG_DEBUG("Other protocols added " << message->AddressBlockSize() - addressBlockSize
                                              << " address blocks to HELLO");
    }

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(pbb);

    NS_LOG_INFO("Send HELLO from " << m_socketAddresses[socket] << " to " << LL_MANET_ROUTERS_IPV4
                                   << ":" << UDP_PORT_MANET);
    m_txTrace(packet);
    socket->Send(packet);
    ScheduleHello(socket);
    // Rather than run a separate timer process to periodically check this, append it here
    RemoveExpiredTwoHopNeighbors();
}

void
NhdpClient::RemoveExpiredTwoHopNeighbors()
{
    NS_LOG_FUNCTION(this);
    for (auto it = m_twoHopInfoBase.begin(); it != m_twoHopInfoBase.end();)
    {
        if (it->second.m_expirationTime <= Simulator::Now())
        {
            m_twoHopChangeTrace(TwoHopStatus::REMOVED, it->second);
            it = m_twoHopInfoBase.erase(it);
            NS_LOG_INFO("Removing TwoHopTuple to " << it->second.m_twoHopAddr << " via "
                                                   << it->second.m_neighborAddrList[0]);
        }
        else
        {
            ++it;
        }
    }
}

/*
void
NhdpClient::CleanRemovedInterfaceAddressSet()
{
  NS_LOG_FUNCTION(this);
  while (m_removedInterfaceAddressSet.front()->time <= Simulator::Now())
    {
      std::pop_heap (m_removedInterfaceAddressSet.begin(),
          m_removedInterfaceAddressSet.end(),
          TimeCompare<RemovedInterfaceAddressTuple>());
      m_removedInterfaceAddressSet.pop_back();
    }
}
*/

Ptr<PbbAddressBlock>
NhdpClient::BuildLocalAddressBlock(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << m_socketAddresses[socket]);
    Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
    NS_ASSERT(ipv4);

    Ptr<PbbAddressBlock> addrBlock = Create<PbbAddressBlockIpv4>();

    /* Put all the addresses that belong to this interface first */
    int32_t localIface = ipv4->GetInterfaceForAddress(m_socketAddresses[socket]);
    NS_ASSERT(localIface >= 0);
    uint32_t numLocalAddrs = ipv4->GetNAddresses(localIface);
    for (uint32_t i = 0; i < numLocalAddrs; i++)
    {
        Ipv4InterfaceAddress ifaceAddr = ipv4->GetAddress(localIface, i);
        addrBlock->AddressPushBack(ifaceAddr.GetLocal());
        addrBlock->PrefixPushBack(ifaceAddr.GetMask().GetPrefixLength());
        NS_LOG_DEBUG("Adding address " << ifaceAddr.GetLocal() << " to address block");
        m_localIpv4Address = ifaceAddr.GetLocal();
    }

    /* Put in the addresses belonging to all the other interfaces */
    for (uint32_t i = 0; i < ipv4->GetNInterfaces(); i++)
    {
        /* We already covered this interface */
        if (i == (uint32_t)localIface) /* TODO: Is this typedef OK? */
        {
            continue;
        }

        for (uint32_t j = 0; j < ipv4->GetNAddresses(i); j++)
        {
            Ipv4InterfaceAddress ifaceAddr = ipv4->GetAddress(i, j);
            if (ifaceAddr.GetLocal() == Ipv4Address::GetLoopback())
            {
                continue;
            }
            addrBlock->AddressPushBack(ifaceAddr.GetLocal());
            addrBlock->PrefixPushBack(ifaceAddr.GetMask().GetPrefixLength());
        }
    }

    /* Mark the local addresses */
    Ptr<PbbAddressTlv> addrTlv = Create<PbbAddressTlv>();
    addrBlock->TlvPushBack(addrTlv);
    addrTlv->SetType(ADDR_TLV_LOCAL_IF);
    addrTlv->SetValue(&ADDR_TLV_LOCAL_IF_THIS_IF, sizeof(ADDR_TLV_LOCAL_IF_THIS_IF));
    addrTlv->SetIndexStart(0);
    /* We only need to set an index stop if there is more than one local address */
    if (numLocalAddrs > 1)
    {
        addrTlv->SetIndexStop(numLocalAddrs - 1);
    }

    /* Don't make another tlv if there are no other addresses */
    if (numLocalAddrs != (uint32_t)addrBlock->AddressSize())
    {
        addrTlv = Create<PbbAddressTlv>();
        addrBlock->TlvPushBack(addrTlv);
        addrTlv->SetType(ADDR_TLV_LOCAL_IF);
        addrTlv->SetValue(&ADDR_TLV_LOCAL_IF_OTHER_IF, sizeof(ADDR_TLV_LOCAL_IF_OTHER_IF));
        addrTlv->SetIndexStart(numLocalAddrs);
        addrTlv->SetIndexStop(addrBlock->AddressSize() - 1);
    }

    return addrBlock;
}

Ptr<PbbAddressBlock>
NhdpClient::BuildLinkStatusAddressBlock(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << m_socketAddresses[socket]);

    std::vector<Ipv4Address> heard;
    std::vector<Ipv4Address> symmetric;
    std::vector<Ipv4Address> lost;

    for (auto& [addr, linkTuple] : m_linkInfoBase)
    {
        // o  Network addresses of MANET interfaces of 1-hop neighbors from the
        // Link Set of the Interface Information Base for this MANET
        // interface (i.e., from an L_neighbor_iface_addr_list), other than
        // those from Link Tuples with L_status = PENDING.
        NS_LOG_DEBUG("LinkTuple neighbor "
                     << linkTuple.m_neighborAddrList[0] << " heardTime "
                     << linkTuple.m_heardTime.GetSeconds() << " symTime "
                     << linkTuple.m_symTime.GetSeconds() << " quality " << linkTuple.m_quality
                     << " pending " << linkTuple.m_pending << " lost " << linkTuple.m_lost
                     << " expiration " << linkTuple.m_expirationTime.GetSeconds());
        if (!linkTuple.m_pending)
        {
            if (linkTuple.m_symTime >= Simulator::Now())
            {
                symmetric.push_back(linkTuple.m_neighborAddrList[0]);
                if (linkTuple.m_lost)
                {
                    NS_LOG_INFO("Setting link to " << linkTuple.m_neighborAddrList[0]
                                                   << " to symmetric");
                    linkTuple.m_lost = false;
                    m_linkChangeTrace(linkTuple.GetLinkStatus(), linkTuple);
                }
            }
            else if (linkTuple.m_heardTime >= Simulator::Now())
            {
                heard.push_back(linkTuple.m_neighborAddrList[0]);
                if (linkTuple.m_lost)
                {
                    NS_LOG_INFO("Setting link to " << linkTuple.m_neighborAddrList[0]
                                                   << " to heard");
                    linkTuple.m_lost = false;
                    m_linkChangeTrace(linkTuple.GetLinkStatus(), linkTuple);
                }
            }
            else
            {
                lost.push_back(linkTuple.m_neighborAddrList[0]);
                if (!linkTuple.m_lost)
                {
                    NS_LOG_INFO("Setting link to " << linkTuple.m_neighborAddrList[0]
                                                   << " to lost");
                    linkTuple.m_lost = true;
                    m_linkChangeTrace(linkTuple.GetLinkStatus(), linkTuple);
                }
                auto itNeigh = m_neighborInfoBase.find(linkTuple.m_neighborAddrList[0]);
                if (itNeigh != m_neighborInfoBase.end())
                {
                    if (linkTuple.m_heardTime <= Simulator::Now())
                    {
                        NS_LOG_INFO("Erasing neighbor " << linkTuple.m_neighborAddrList[0]);
                        m_neighborChangeTrace(NeighborStatus::REMOVED, itNeigh->second);
                        m_neighborInfoBase.erase(itNeigh);
                    }
                    else
                    {
                        if (itNeigh->second.m_symmetric)
                        {
                            NS_LOG_DEBUG("Setting neighbor " << linkTuple.m_neighborAddrList[0]
                                                             << " to not symmetric");
                            itNeigh->second.m_symmetric = false;
                            m_neighborChangeTrace(NeighborStatus::MODIFIED, itNeigh->second);
                        }
                    }
                }
            }
        }
    }
    if (heard.empty() && symmetric.empty() && lost.empty())
    {
        return nullptr;
    }
    Ptr<PbbAddressBlock> addrBlock = Create<PbbAddressBlockIpv4>();
    for (const auto& it : heard)
    {
        addrBlock->AddressPushBack(it);
        NS_LOG_DEBUG("Adding HEARD address " << it);
    }
    for (const auto& it : symmetric)
    {
        addrBlock->AddressPushBack(it);
        NS_LOG_DEBUG("Adding SYMMETRIC address " << it);
    }
    for (const auto& it : lost)
    {
        addrBlock->AddressPushBack(it);
        NS_LOG_DEBUG("Adding LOST address " << it);
    }
    uint32_t index = 0;
    if (!heard.empty())
    {
        Ptr<PbbAddressTlv> addrTlv = Create<PbbAddressTlv>();
        addrTlv->SetType(ADDR_TLV_LINK_STATUS);
        addrTlv->SetValue(&ADDR_TLV_LINK_STATUS_HEARD, sizeof(ADDR_TLV_LINK_STATUS_HEARD));
        addrTlv->SetIndexStart(0);
        addrTlv->SetIndexStop(heard.size() - 1);
        addrBlock->TlvPushBack(addrTlv);
        NS_LOG_DEBUG("Adding PbbAddressTlv for HEARD address from index 0 to " << heard.size() - 1);
    }
    index += heard.size();
    if (!symmetric.empty())
    {
        Ptr<PbbAddressTlv> addrTlv = Create<PbbAddressTlv>();
        addrTlv->SetType(ADDR_TLV_LINK_STATUS);
        addrTlv->SetValue(&ADDR_TLV_LINK_STATUS_SYMMETRIC, sizeof(ADDR_TLV_LINK_STATUS_SYMMETRIC));
        addrTlv->SetIndexStart(index);
        addrTlv->SetIndexStop(index + symmetric.size() - 1);
        addrBlock->TlvPushBack(addrTlv);
        NS_LOG_DEBUG("Adding PbbAddressTlv for SYMMETRIC address from index "
                     << index << " to " << index + symmetric.size() - 1);
    }
    index += symmetric.size();
    if (!lost.empty())
    {
        Ptr<PbbAddressTlv> addrTlv = Create<PbbAddressTlv>();
        addrTlv->SetType(ADDR_TLV_LINK_STATUS);
        addrTlv->SetValue(&ADDR_TLV_LINK_STATUS_LOST, sizeof(ADDR_TLV_LINK_STATUS_LOST));
        addrTlv->SetIndexStart(index);
        addrTlv->SetIndexStop(index + lost.size() - 1);
        addrBlock->TlvPushBack(addrTlv);
        NS_LOG_DEBUG("Adding PbbAddressTlv for LOST address from index "
                     << index << " to " << index + lost.size() - 1);
    }
    return addrBlock;
}

} // namespace nhdp

} // namespace ns3
