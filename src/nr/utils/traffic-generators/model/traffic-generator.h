// Copyright (c) 2010 Georgia Institute of Technology
// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/seq-ts-size-header.h"
#include "ns3/traced-callback.h"

namespace ns3
{

class Address;
class Socket;

/**
 * \ingroup applications
 * \defgroup traffic TrafficGenerator
 *
 * This traffic generator simply sends data
 * as fast as possible up to FileSize or until
 * the application is stopped (if FileSize is
 * zero). Once the lower layer send buffer is
 * filled, it waits until space is free to
 * send more data, essentially keeping a
 * constant flow of data. Only SOCK_STREAM
 * and SOCK_SEQPACKET sockets are supported.
 * For example, TCP sockets can be used, but
 * UDP sockets can not be used.
 */

class TrafficGenerator : public Application
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    TrafficGenerator();

    ~TrafficGenerator() override;

    /**
     * \brief Get the total number of bytes that have been sent during this
     *        object's lifetime.
     *
     * return the total number of bytes that have been sent
     */
    uint64_t GetTotalBytes() const;

    /**
     * \brief Get the total number of packets that have been sent during this
     *        object's lifetime.
     *
     * return the total number of packets that have been sent
     */
    uint64_t GetTotalPackets() const;

    /**
     * \brief Send another packet burst, which can be e.g., a file, or a video frame
     *
     * return true if another packet burst was started; false if the request
     *        didn't succeed (possibly because another transfer is ongoing)
     */
    bool SendPacketBurst();

    /**
     * \brief Get the socket this application is attached to.
     * \return pointer to associated socket
     */
    Ptr<Socket> GetSocket() const;

    /**
     * \brief Sets the packet size
     */
    void SetPacketSize(uint32_t packetSize);
    /**
     * \brief Sets the remote address
     */
    void SetRemote(Address remote);
    /**
     * \brief Sets the protocol
     */
    void SetProtocol(TypeId protocol);

    /// Traced Callback: sent packets
    typedef TracedCallback<Ptr<const Packet>> TxTracedCallback;
    TxTracedCallback m_txTrace;

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model. Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

  protected:
    void DoDispose() override;
    void DoInitialize() override;
    /*
     * \brief Used by child classes to configure the burst size in the number
     * of bytes when GenerateNextPucketBurstSize is called
     */
    void SetPacketBurstSizeInBytes(uint32_t burstSize);
    /*
     * \brief Used by child classes to configure the burst size in the number
     * of packets when GenerateNextPucketBurstSize is called
     */
    void SetPacketBurstSizeInPackets(uint32_t burstSize);
    /*
     * \brief Returns the latest generated packet burst size in the number
     * of bytes
     */
    uint32_t GetPacketBurstSizeInBytes() const;
    /*
     * \brief Returns the latest generated packet burst size in the number
     * of bytes
     */
    uint32_t GetPacketBurstSizeInPackets() const;
    /*
     * \brief Called at the time specified by the Stop.
     * Notice that we want to allow that child classes can call stop of this class,
     * which is why we change its default access level from private to protected.
     */
    void StopApplication() override; // Called at time specified by Stop
    /*
     * \return Traffic generator ID
     */
    uint16_t GetTgId() const;

    /*
     * \brief Returns peer address
     * \return the peer address
     */
    Address GetPeer() const;

  private:
    // inherited from Application base class.
    void StartApplication() override; // Called at time specified by Start
    /**
     * \brief Send next packet.
     * Send packet until the L4 transmission buffer is full, or all
     * scheduled packets are sent, or all packet burst is being sent.
     */
    void SendNextPacket();
    /**
     * \brief Connection Succeeded (called by Socket through a callback)
     * \param socket the connected socket
     */
    void ConnectionSucceeded(Ptr<Socket> socket);
    /**
     * \brief Connection Failed (called by Socket through a callback)
     * \param socket the connected socket
     */
    void ConnectionFailed(Ptr<Socket> socket);
    /**
     * \brief Close Succeeded (called by Socket through a callback)
     * \param socket the closed socket
     */
    void CloseSucceeded(Ptr<Socket> socket);
    /**
     * \brief Close Failed (called by Socket through a callback)
     * \param socket the closed socket
     */
    void CloseFailed(Ptr<Socket> socket);
    /**
     * \brief Send more data as soon as some has been transmitted.
     */
    void SendNextPacketIfConnected(Ptr<Socket>, uint32_t);
    /**
     * \brief This function can be used by child classes to schedule some event
     * after sending the file
     */
    virtual void PacketBurstSent();
    /**
     * \brief Generate the next packet burst size in bytes or packets
     */
    virtual void GenerateNextPacketBurstSize();
    /**
     * \brief Returns what is the next packet size. Overridden by child classes
     * that generate variable packet sizes
     */
    virtual uint32_t GetNextPacketSize() const = 0;
    /**
     * \brief Get the relative time when the next packet should be sent. Override
     * this function if there is some specific inter packet interval time.
     * \return the relative time when the next packet will be sent
     */
    virtual Time GetNextPacketTime() const;

    Ptr<Socket> m_socket;                   //!< Associated socket
    Address m_peer;                         //!< Peer address
    bool m_connected{false};                //!< True if connected
    uint32_t m_currentBurstTotBytes{0};     //!< Total bytes sent so far in the current burst
    TypeId m_tid;                           //!< The type of protocol to use.
    uint32_t m_currentBurstTotPackets{0};   //!< Total packets sent so far in the current burst
    uint64_t m_totBytes{0};                 //!< Total bytes sent so far
    uint64_t m_totPackets{0};               //!< Total packets sent so far
    bool m_stopped{false};                  //!< flag that indicates if the application is stopped
    uint32_t m_packetBurstSizeInBytes{0};   //!< The last generated packet burst size in bytes
    uint32_t m_packetBurstSizeInPackets{0}; //!< The last generated packet burst size in packets
    EventId m_eventIdSendNextPacket; //!< We need to track if there is an active event to not create
                                     //!< a new one based on the traces from the socket
    bool m_waitForNextPacketBurst{
        false}; //!< When we are waiting that the next packet burst start we should have an
                //!< indicator and discard callbacks that would otherwise trigger send packet

    static uint16_t m_tgIdCounter; //!< traffic generator ID counter for the tracing purposes
    uint16_t m_tgId{0};            //!< traffic generator ID for the tracing purposes
    uint16_t m_packetId{
        0}; //!< packetId of the current flow, when it reaches the maximum value starts from zero
    bool m_enableSeqTsSizeHeader{false}; //!< Enable or disable the use of SeqTsSizeHeader
    uint32_t m_seq{0};                   //!< Sequence number
    /// Callback for tracing the packet Tx events, includes source, destination, the packet sent,
    /// and header
    TracedCallback<Ptr<const Packet>, const Address&, const Address&, const SeqTsSizeHeader&>
        m_txTraceWithSeqTsSize;
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_H */
