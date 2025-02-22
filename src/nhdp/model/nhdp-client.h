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

#include <map>
#include <queue>
#include <set>

namespace ns3
{

namespace nhdp
{

/* PacketBB Types */
const uint8_t ADDR_TLV_LOCAL_IF = 1;

/* PacketBB Values */
const uint8_t ADDR_TLV_LOCAL_IF_THIS = 1;
const uint8_t ADDR_TLV_LOCAL_IF_OTHER = 2;

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

    void HandleRecv(Ptr<Socket> socket);

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  private:
    void StartApplication() override;
    void StopApplication() override;

    void ScheduleHello(Ptr<Socket> socket);
    void SendHello(Ptr<Socket> socket);

    // void CleanRemovedInterfaceAddressSet (void);

    Ptr<PbbAddressBlock> BuildLocalAddressBlock(Ptr<Socket> socket);

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

    /* Other attributes */
    bool m_running{false};
    std::set<uint32_t> m_nonManetSet;
    Ptr<UniformRandomVariable> m_rng;
    std::map<Ptr<Socket>, Ipv4Address> m_socketAddresses;
    Ptr<Socket> m_recvSocket; //!< Receiving socket

    /*
    std::map< uint32_t, Ptr<Socket> > m_indexSockets;
    std::map< Ptr<Socket>, uint32_t > m_socketsIndex;

    std::queue< Ptr<PbbMessage> > m_messages;
    std::multimap< uint8_t, Callback<PbbMessage> > m_callbacks;
    */
};

} // namespace nhdp

} // namespace ns3

#endif /* NHDP_CLIENT_H */
