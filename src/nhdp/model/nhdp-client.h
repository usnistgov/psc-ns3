/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

/* These classes implement the NHDP neighbor discovery protocol.  See
 * https://datatracker.ietf.org/doc/html/rfc6130 for more info */

#ifndef NHDP_CLIENT_H
#define NHDP_CLIENT_H

#include "nhdp-info-base.h"

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/packetbb.h"
#include "ns3/ptr.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/traced-callback.h"

#include <map>
#include <optional>
#include <queue>
#include <set>

namespace ns3
{

namespace nhdp
{

/* PacketBB Message Types */
const uint8_t MESSAGE_TYPE_HELLO = 0;

/* PacketBB Address Block Types */
const uint8_t ADDR_TLV_LOCAL_IF = 2;
const uint8_t ADDR_TLV_LINK_STATUS = 3;
const uint8_t ADDR_TLV_OTHER_NEIGHB = 4;

/* PacketBB Address Block Values */
const uint8_t ADDR_TLV_LOCAL_IF_THIS_IF = 0;
const uint8_t ADDR_TLV_LOCAL_IF_OTHER_IF = 1;
const uint8_t ADDR_TLV_LINK_STATUS_LOST = 0;
const uint8_t ADDR_TLV_LINK_STATUS_SYMMETRIC = 1;
const uint8_t ADDR_TLV_LINK_STATUS_HEARD = 2;
const uint8_t ADDR_TLV_OTHER_NEIGHB_LOST = 0;
const uint8_t ADDR_TLV_OTHER_NEIGHB_SYMMETRIC = 1;

/** Used as the comparator when making heaps out of tuples with expiration
 * times. */
template <class T>
class TimeCompare
{
  public:
    bool operator()(const Ptr<T>& a, const Ptr<T>& b)
    {
        return b->time < a->time;
    }
};

/**
 * @ingroup applications
 * @defgroup nhdp NHDP
 */

enum class Action
{
    ADDED,
    CHANGED,
    REMOVED
};

enum class AddressTlvLinkStatus
{
    LOST,
    SYMMETRIC,
    HEARD
};

/**
 * @ingroup nhdp
 * @brief A NHDP client
 */
class NhdpClient : public Application
{
  public:
    NhdpClient();

    static TypeId GetTypeId();

    /* Local Interface Base Methods */
    /** Prevents interface from being used for transmitting HELLO messages */
    void MarkIfaceNonManet(uint32_t ifaddr);

    /* End Information Bases */

    /**
     * @brief Adds a PacketBB message to be sent out with the next hello message.
     * @param message a PacketBB message to include
     */
    // void QueueMessage (Ptr<PbbMessage> message);

    /**
     * @brief Sets a callback for a particular message type.
     *
     * @param messageType the PbbMessage message type to listen for
     * @param cb the callback to call when a message is received
     *
     * On reception of a message of that type, the callback will be called.
     * Multiple callbacks can be registered to one message type.
     */
    // void RegisterMessageCallback (uint8_t messageType, Callback<PbbMessage> cb);

    /**
     * @brief Sets a callback for a received NHDP packet for calculating link quality
     *
     * @param packet Pointer to the received packet
     * @param cb the callback to call when a packet is received
     *
     * The packet is passed as a non-const object so that any PacketTag object can
     * be removed if needed.
     */
    void RegisterLinkQualityCallback(Callback<double, Ptr<Packet>> cb);

    /**
     * A callback to receive indications that the lower layer link has failed.
     *
     * \param neighborIpv4Addr The IP address associated with the failed link.
     */
    void HandleLinkFailure(Ipv4Address neighborIpv4Addr);

    void BypassRecv(Ptr<Packet> packet);

    void HandleRecv(Ptr<Socket> socket);

    void HandleDirectLinkEstablishedTrace(uint32_t srcL2Id,
                                          Ipv4Address selfIpv4Addr,
                                          uint32_t peerL2Id,
                                          Ipv4Address peerIpv4Addr);

    void HandleDirectLinkReleasingTrace(uint32_t srcL2Id,
                                        Ipv4Address selfIpv4Addr,
                                        uint32_t peerL2Id,
                                        Ipv4Address peerIpv4Addr);

    const std::map<Ipv4Address, NeighborTuple>& GetNeighborInfoBase() const;
    const std::map<Ipv4Address, LinkTuple>& GetLinkInfoBase() const;
    const std::map<std::pair<Ipv4Address, Ipv4Address>, TwoHopTuple>& GetTwoHopInfoBase() const;
    /**
     * TracedCallback signature for neighbor information base change event.
     *
     * @param [in] action The type of change (added, changed, removed)
     * @param [in] oldTuple The old NeighborTuple
     * @param [in] newTuple The new NeighborTuple
     */
    typedef void (*NeighborChangeTracedCallback)(Action action,
                                                 const NeighborTuple& oldTuple,
                                                 const NeighborTuple& newTuple);

    /**
     * TracedCallback signature for Link Set change event.
     *
     * If the type of change is added or removed, both oldTuple and newTuple will
     * be set to the same value.
     *
     * @param [in] action The type of change (added, changed, removed)
     * @param [in] oldTuple The old LinkTuple
     * @param [in] newTuple The new LinkTuple
     */
    typedef void (*LinkChangeTracedCallback)(Action action,
                                             const LinkTuple& oldTuple,
                                             const LinkTuple& newTuple);

    /**
     * TracedCallback signature for two-hop information base change event.
     *
     * @param [in] action The type of change (added, changed, removed)
     * @param [in] oldTuple The old TwoHopTuple
     * @param [in] newTuple The new TwoHopTuple
     */
    typedef void (*TwoHopChangeTracedCallback)(Action action,
                                               const TwoHopTuple& oldTuple,
                                               const TwoHopTuple& newTuple);

    /**
     * TracedCallback signature for lost neighbor information base change event.
     *
     * @param [in] action The type of change (added, changed, removed)
     * @param [in] oldTuple The old LostNeighborTuple
     * @param [in] newTuple The new LostNeighborTuple
     */
    typedef void (*LostNeighborChangeTracedCallback)(Action action,
                                                     const LostNeighborTuple& oldTuple,
                                                     const LostNeighborTuple& newTuple);

    /**
     * TracedCallback signature for reported L2 link failure
     *
     * @param [in] address The peer IPv4 address
     */
    typedef void (*LinkFailureTracedCallback)(const Ipv4Address& address);

    /**
     * TracedCallback signature for HELLO send trace
     *
     * @param [in] addr The sending IPv4 address
     * @param [in] links List of links advertised
     */
    typedef void (*HelloSendTracedCallback)(
        Ipv4Address addr,
        const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links);

    /**
     * TracedCallback signature for HELLO message send trace
     *
     * @param [in] addr The sending IPv4 address
     * @param [in] helloMsg The (modifiable) HELLO message
     */
    typedef void (*HelloMessageSendTracedCallback)(Ipv4Address addr, Ptr<PbbMessage> helloMsg);

    /**
     * TracedCallback signature for HELLO recv trace
     *
     * @param [in] addr The IPv4 address of the originator
     * @param [in] links List of links advertised
     * @param [in] quality Link quality
     */
    typedef void (*HelloRecvTracedCallback)(
        Ipv4Address addr,
        const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>& links,
        std::optional<double> quality);

    /**
     * TracedCallback signature for HELLO message receive trace
     *
     * @param [in] helloMsg The HELLO message
     * @param [in] neighborAddr The neighbor address
     * @param [in] quality Link quality if available
     */
    typedef void (*HelloMessageRecvTracedCallback)(Ptr<PbbMessage> helloMsg,
                                                   Ipv4Address neighborAddr,
                                                   std::optional<double> quality);

    int64_t AssignStreams(int64_t stream) override;

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  private:
    void StartApplication() override;
    void StopApplication() override;

    Ipv4Address HandlePbbMessage(Ptr<PbbMessage> msg, std::optional<double> quality);
    Ipv4Address HandleLocalAddressBlock(Ptr<PbbAddressBlock> addressBlock,
                                        std::optional<double> quality);
    void HandleLinkStatusAddressBlock(Ptr<PbbAddressBlock> addressBlock,
                                      Ipv4Address neighborIpv4Addr,
                                      std::optional<double> quality);

    void ScheduleHello(Ptr<Socket> socket);
    void SendHello(Ptr<Socket> socket);

    void UpdateLinkTuples();

    // void CleanRemovedInterfaceAddressSet (void);

    Ptr<PbbAddressBlock> BuildLocalAddressBlock(Ptr<Socket> socket);
    Ptr<PbbAddressBlock> BuildLinkStatusAddressBlock(Ptr<Socket> socket);

    /* Configuration paramters */
    Ipv4Address m_address;
    uint32_t m_port{0};

    Time m_helloInterval;
    Time m_helloMinInterval;
    Time m_refreshInterval;

    Time m_lHoldTime;
    Time m_hHoldTime;

    double m_hystAccept{0};
    double m_hystReject{0};
    double m_initialQuality{0};
    bool m_initialPending{false};

    Time m_hpMaxJitter;
    Time m_htMaxJitter;

    Time m_nHoldTime;
    Time m_iHoldTime;

    /* Information bases */
    std::map<Ipv4Address, NeighborTuple> m_neighborInfoBase;
    std::map<Ipv4Address, LinkTuple> m_linkInfoBase;
    std::map<std::pair<Ipv4Address, Ipv4Address>, TwoHopTuple> m_twoHopInfoBase;
    std::map<Ipv4Address, LostNeighborTuple> m_lostNeighborSet;
    std::vector<Ipv4Address> m_lostAddressList;

    /* Map used for bypass */
    std::map<Ipv4Address, Ptr<NhdpClient>> m_establishedPeers;

    /* Other attributes */
    bool m_running{false};
    std::set<uint32_t> m_nonManetSet;
    Ptr<UniformRandomVariable> m_rng;
    std::map<Ptr<Socket>, Ipv4Address> m_socketAddresses;
    Ptr<Socket> m_recvSocket; //!< Receiving socket
    Ipv4Address m_localIpv4Address;
    Ptr<PbbAddressBlock> m_localAddrBlock;
    //!< Bypass transmission of messages
    bool m_bypassMode{false};

    void RemoveExpiredTwoHopNeighbors();

    Callback<double, Ptr<Packet>> m_linkQualityCallback;

    TracedCallback<Action, const NeighborTuple&, const NeighborTuple&> m_neighborChangeTrace;
    TracedCallback<Action, const LinkTuple&, const LinkTuple&> m_linkChangeTrace;
    TracedCallback<Action, const TwoHopTuple&, const TwoHopTuple&> m_twoHopChangeTrace;
    TracedCallback<Action, const LostNeighborTuple&, const LostNeighborTuple&>
        m_lostNeighborChangeTrace;
    TracedCallback<Ipv4Address, const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>&>
        m_helloSendTrace;
    TracedCallback<Ipv4Address,
                   const std::vector<std::pair<Ipv4Address, AddressTlvLinkStatus>>&,
                   std::optional<double>>
        m_helloRecvTrace;
    TracedCallback<Ipv4Address, Ptr<PbbMessage>> m_helloMessageSendTrace;
    TracedCallback<Ptr<PbbMessage>, Ipv4Address, std::optional<double>> m_helloMessageRecvTrace;
    TracedCallback<const Ipv4Address&> m_linkFailureTrace;
    TracedCallback<Ptr<const Packet>> m_txTrace;

    /*
    std::map< uint32_t, Ptr<Socket> > m_indexSockets;
    std::map< Ptr<Socket>, uint32_t > m_socketsIndex;

    std::queue< Ptr<PbbMessage> > m_messages;
    std::multimap< uint8_t, Callback<PbbMessage> > m_callbacks;
    */
};

std::ostream& operator<<(std::ostream& os, const Action& action);
std::ostream& operator<<(std::ostream& os, const AddressTlvLinkStatus& status);

} // namespace nhdp

} // namespace ns3

#endif /* NHDP_CLIENT_H */
