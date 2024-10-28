/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
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

NS_OBJECT_ENSURE_REGISTERED(NhdpClient);
NS_LOG_COMPONENT_DEFINE("NhdpClient");

NhdpClient::NhdpClient()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NhdpClient::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NhdpClient")
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
                          MakeTimeChecker());
    return tid;
}

void
NhdpClient::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    if (!m_rng)
    {
        m_rng = CreateObject<UniformRandomVariable>();
        m_rng->SetAttribute("Max", DoubleValue(DEFAULT_HP_MAX_JITTER.GetSeconds()));
    }
    Application::DoInitialize();
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
NhdpClient::HandleRecv(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    Address from;
    Ptr<Packet> packet = socket->RecvFrom(from);
    NS_LOG_INFO("To: " << m_socketAddresses[socket]
                       << " From: " << InetSocketAddress::ConvertFrom(from).GetIpv4());
}

void
NhdpClient::DoDispose()
{
    NS_LOG_FUNCTION(this);
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

        NS_LOG_INFO("Binding to " << address);
        socket->Bind(InetSocketAddress(address, m_port));
        socket->SetRecvCallback(MakeCallback(&NhdpClient::HandleRecv, this));
        socket->Connect(InetSocketAddress(Ipv4Address::GetBroadcast(), m_port));

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
    NS_ASSERT_MSG(m_rng, "No jitter random variable; is NhdpClient initialized?");
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

    /* Validity time message TLV */

    Ptr<PbbAddressBlock> addrBlock = BuildLocalAddressBlock(socket);
    message->AddressBlockPushBack(addrBlock);

    /* Add any other messages other protocols want to send */
    /*
    while (!m_messages.empty ())
      {
        pbb.MessagePushBack (m_messages.front ());
        m_messages.pop ();
      }
    */

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(pbb);

    NS_LOG_INFO("Send HELLO from " << m_socketAddresses[socket]);
    socket->Send(packet);
    ScheduleHello(socket);
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
    addrTlv->SetValue(&ADDR_TLV_LOCAL_IF_THIS, sizeof(ADDR_TLV_LOCAL_IF_THIS));
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
        addrTlv->SetValue(&ADDR_TLV_LOCAL_IF_OTHER, sizeof(ADDR_TLV_LOCAL_IF_OTHER));
        addrTlv->SetIndexStart(numLocalAddrs);
        addrTlv->SetIndexStop(addrBlock->AddressSize() - 1);
    }

    return addrBlock;
}

} /* Namespace ns3 */
