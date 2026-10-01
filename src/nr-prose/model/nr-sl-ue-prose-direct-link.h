//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_UE_PROSE_DIRECT_LINK_H
#define NR_SL_UE_PROSE_DIRECT_LINK_H

#include "nr-sl-pc5-signalling-header.h"

#include <ns3/nr-sl-ue-prose-dir-lnk-sap.h>
#include <ns3/object.h>
#include <ns3/tag.h>
#include <ns3/timer.h>
#include <ns3/traced-callback.h>

#include <unordered_map>
#include <vector>

namespace ns3
{

/**
 * \ingroup nr
 *
 * Packet tag used to exchange IPv4 address information between peer UEs of the
 * direct link during establishment procedure.
 */
class Ipv4AddrTag : public Tag
{
  public:
    Ipv4AddrTag();

    /**
     * \brief Set the tag's address
     *
     * \param addr the address
     */
    void SetAddress(Ipv4Address addr);

    /**
     * \brief Get the tag's address
     *
     * \returns the address
     */
    Ipv4Address GetAddress(void) const;

    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual void Print(std::ostream& os) const;

  private:
    Ipv4Address m_addr; //!< address
};

/**
 * \ingroup nr
 *
 * \brief NR ProSe Direct Link with a peer UE
 *
 * This class implements the NR ProSe direct link with a peer UE. It stores all
 * the information corresponding to the direct link in this UE and executes the
 * the different protocols depending on the UE role in the direct link.
 */
class NrSlUeProseDirectLink : public Object
{
    /// allow MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink> class friend access
    friend class MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink>;

  protected:
    virtual void DoDispose();

  public:
    NrSlUeProseDirectLink();
    virtual ~NrSlUeProseDirectLink(void);
    static TypeId GetTypeId(void);

    /**
     * \brief Set parameters of the direct link
     *
     * \param selfL2Id layer 2 ID of this UE
     * \param peerL2Id layer 2 ID of the peer UE
     * \param isInitiating true if the UE is the one initiating the direct link establishment
     * \param isRelayConn true if the direct link is part of a relay connection
     * \param relayServiceCode the relay service code associated to this direct link
     * \param selfIp the Ipv4 address this UE is using
     */
    void SetParameters(uint32_t selfL2Id,
                       uint32_t peerL2Id,
                       bool isInitiating,
                       bool isRelayConn,
                       uint32_t relayServiceCode,
                       Ipv4Address selfIp);

    /**
     * \brief Get the pointer of the ProSe Direct Link SAP provider interface
     *        offered to the Prose layer by this class
     *
     * \return the pointer of type NrSlUeProseDirLnkSapProvider
     */
    NrSlUeProseDirLnkSapProvider* GetNrSlUeProseDirLnkSapProvider();

    /**
     * \brief Set the pointer for the Prose Direct Link SAP User interface
     *        offered to this class by the ProSe layer class
     *
     * \param s the pointer of type m_nrSlUeProseDirLnkSapUser
     */
    void SetNrSlUeProseDirLnkSapUser(NrSlUeProseDirLnkSapUser* s);

    /**
     * \brief Start the ProSe direct link establishment procedure
     *
     */
    void StartConnectionEstablishment();

    /**
     * \brief Start the ProSe direct link release procedure
     *
     * \param cause The connection release cause
     */
    void StartConnectionRelease(uint8_t cause);

    /**
     * \brief Allow the reinitialization of the link
     *        and trigger the connection establishment
     *
     */
    void ResetCurrentLink();

    /**
     * @brief Notifies the unicast link that data was received.
     */
    void NotifyDataReceived();

    enum DirectLinkState
    {
        INIT = 0,
        ESTABLISHING,
        ESTABLISHED,
        RELEASING,
        RELEASED,
        NUM_STATES
    };

    DirectLinkState GetState() const
    {
        return m_state;
    }

    /**
     * \brief Parameters for the ProSe direct link establishment procedure
     *        to be use by a initiating UE
     *
     */
    struct ProcessDirectLinkEstablishmentParams
    {
        Timer* t5080;        ///< Timer T5080 (establishment request retransmission)
        uint32_t rtxCounter; ///< request retransmission counter
        uint32_t rtxMax;     ///< maximum number of request retransmissions
        ProseDirectLinkEstablishmentRequest
            rqMsgCopy; ///< copy of the establishment request message used for rtxs
    };

    /**
     * \brief Parameters for the ProSe direct link keepalive procedure
     *        to be use by a initiating UE
     *
     */
    struct ProcessDirectLinkKeepaliveParams
    {
        Timer* t5084;        ///< Timer T5084 (inactivity)
        Timer* t5085;        ///< Timer T5085 (keepalive request retransmission)
        uint32_t counter;    ///< keepalive counter
        uint32_t rtxCounter; ///< request retransmission counter
        uint32_t rtxMax;     ///< maximum number of request retransmissions
        ProseDirectLinkKeepaliveRequest
            kaMsgCopy; ///< copy of the keepalive request message used for rtxs
    };

    /**
     * \brief Parameters for the ProSe direct link release procedure
     *        to be use by a initiating UE
     *
     */
    struct ProcessDirectLinkReleaseParams
    {
        Timer* t5087;        ///< Timer T5087 (release request retransmission)
        uint32_t rtxCounter; ///< request retransmission counter
        uint32_t rtxMax;     ///< maximum number of request retransmissions
        ProseDirectLinkReleaseRequest
            rqMsgCopy; ///< copy of the release request message used for rtxs
    };

    /**
     * TracedCallback signature for ProSe keep-alive procedure failure
     *
     * @param [in] srcL2Id The source L2 ID of the link
     * @param [in] dstL2Id The destination L2 ID of the link
     */
    typedef void (*KeepAliveFailureTracedCallback)(uint32_t srcL2Id, uint32_t dstL2Id);

  private:
    // NrSlUeProseDirLink SAPs
    NrSlUeProseDirLnkSapUser* m_nrSlUeProseDirLnkSapUser{nullptr}; ///< ProSe Direct Link SAP user
    NrSlUeProseDirLnkSapProvider* m_nrSlUeProseDirLnkSapProvider{
        nullptr}; ///< ProSe Direct Link SAP provider

    // NrSlUeProseDirLnkSapProvider methods
    void DoReceiveNrSlPc5Message(Ptr<Packet> packet);

    // Class internal private methods and member variables

    /**
     * \brief Change the state of this direct link
     *
     * \param newState the state to which the link is changing to
     */
    void SwitchToState(DirectLinkState newState);

    /**
     * \brief Send a NR SL PC5-S message
     *
     * When the direct link is real, this function will pass the message to the
     * ProSe layer to be sent on the appropriate SL-SRB and over the SL.
     *
     * \param packet the NR SL PC5-S message
     * \param dstL2Id the layer 2 ID of the peer UE
     * \param lcId the logical channel ID of the logical channel to be used to
     *             send the message if the direct link is real
     */
    void SendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);

    void ProcessDirectLinkEstablishmentRequest(
        Ptr<Packet> packet); ///< Process a DirectLink Establishment Request message
    void ProcessDirectLinkEstablishmentAccept(
        Ptr<Packet> packet); ///< Process a DirectLink Establishment Accept message
    void ProcessDirectLinkEstablishmentReject(
        Ptr<Packet> packet); ///< Process a DirectLink Establishment Reject message

    void SendDirectLinkEstablishmentRequest(); ///< Send a DirectLink Establishment Request message
    void SendDirectLinkEstablishmentAccept();  ///< Send a DirectLink Establishment Accept message
    void SendDirectLinkEstablishmentReject(
        uint8_t cause); ///< Send a DirectLink Establishment Reject message
    void RetransmitDirectLinkEstablishmentRequest(); ///< Retransmit a DirectLink Establishment
                                                     ///< Request message

    void ProcessDirectLinkReleaseRequest(
        Ptr<Packet> packet); ///< Process a DirectLink Release Request message
    void ProcessDirectLinkReleaseAccept(
        Ptr<Packet> packet); ///< Process a DirectLink Release Accept message

    void SendDirectLinkReleaseRequest(uint8_t cause); ///< Send a DirectLink Release Request message
    void SendDirectLinkReleaseAccept();               ///< Send a DirectLink Release Accept message
    void RetransmitDirectLinkReleaseRequest(); ///< Retransmit a DirectLink Release Request message

    void ProcessDirectLinkKeepaliveRequest(
        Ptr<Packet> packet); ///< Send a Direct Link Keepalive Request message
    void ProcessDirectLinkKeepaliveResponse(
        Ptr<Packet> packet); ///< Send a Direct Link Keepalive Response message

    void SendDirectLinkKeepaliveRequest(); ///< Send a DirectLink Keepalive Request message
    void SendDirectLinkKeepaliveResponse(
        uint32_t counter);                       ///< Send a DirectLink Keepalive Response message
    void RetransmitDirectLinkKeepaliveRequest(); ///< Retransmit a DirectLink Keepalive
                                                 ///< Request message

    // IDs
    uint32_t m_selfL2Id; ///< L2Id of the UE where this direct link object is installed
    uint32_t m_peerL2Id; ///< L2Id of the peer UE for this direct link

    bool m_isInitiating; ///< Indicates if the UE is the initiating UE of the direct link
    bool m_isRelayConn;  ///< Indicates if the direct link is part of a relay connection

    uint32_t m_relayServiceCode; ///< The relay service code associated with this direct link

    DirectLinkState m_state; ///< State of this direct link

    NrPc5SignallingHeaderSequenceNumber m_pc5SigMsgSeqNum; ///< Unique sequence number generator for
                                                           ///< PC5-S messages to be transmitted

    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo m_ipInfo; ///< IP configuration

    // Parameters and functions depending on the procedure

    // Establishment procedure
    ProcessDirectLinkEstablishmentParams
        m_pdlEsParam; ///< Parameters of the establishment procedure
    void SetT5080(
        Time t); ///< Set the value of the timer T5080 (Establishment Request Retransmission)
    void SetPdlEsReqRtxMax(
        uint32_t max); ///< Set the maximum number of Establishment Request Retransmissions
    void T5080Expiry();

    // Keepalive procedure
    ProcessDirectLinkKeepaliveParams m_pdlKaParam; ///< Parameters of the keepalive procedure
    void SetT5084(Time t); ///< Set the value of the timer T5084 (inactivity)
    void SetT5085(Time t); ///< Set the value of the timer T5085 (keepalive retransmission)
    void SetPdlKaRtxMax(
        uint32_t max); ///< Set the maximum number of Keepalive Request Retransmissions
    void T5084Expiry();
    void T5085Expiry();

    // Release procedure
    ProcessDirectLinkReleaseParams m_pdlReParam; ///< Parameters of the release procedure
    void SetT5087(Time t); ///< Set the value of the timer T5080 (Release Request Retransmission)
    void SetPdlReReqRtxMax(
        uint32_t max); ///< Set the maximum number of Release Request Retransmissions
    void T5087Expiry();

    /**
     * Trace fired when a Prose direct link enters releasing state
     */
    TracedCallback<uint32_t, uint32_t> m_keepAliveFailureTrace;

}; // end of NrSlUeProseDirectLink

/**
 * \brief Stream output operator
 * \param os output stream
 * \param e enum value to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, enum NrSlUeProseDirectLink::DirectLinkState e);

} // namespace ns3

#endif // NR_SL_UE_PROSE_DIRECT_LINK_H
