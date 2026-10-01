//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-ue-prose-direct-link.h"

#include "nr-sl-prose-tag.h"

#include <ns3/abort.h>
#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/object-factory.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeProseDirectLink");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeProseDirectLink);

Ipv4AddrTag::Ipv4AddrTag()
    : m_addr(Ipv4Address())
{
    NS_LOG_FUNCTION(this);
}

void
Ipv4AddrTag::SetAddress(Ipv4Address addr)
{
    NS_LOG_FUNCTION(this << addr);
    m_addr = addr;
}

Ipv4Address
Ipv4AddrTag::GetAddress(void) const
{
    NS_LOG_FUNCTION(this);
    return m_addr;
}

TypeId
Ipv4AddrTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Ipv4AddrTag")
                            .SetParent<Tag>()
                            .SetGroupName("Nr")
                            .AddConstructor<Ipv4AddrTag>();
    return tid;
}

TypeId
Ipv4AddrTag::GetInstanceTypeId(void) const
{
    NS_LOG_FUNCTION(this);
    return GetTypeId();
}

uint32_t
Ipv4AddrTag::GetSerializedSize(void) const
{
    NS_LOG_FUNCTION(this);
    return 4 + sizeof(uint32_t) + sizeof(uint8_t);
}

void
Ipv4AddrTag::Serialize(TagBuffer i) const
{
    NS_LOG_FUNCTION(this << &i);
    uint8_t buf[4];
    m_addr.Serialize(buf);
    i.Write(buf, 4);
}

void
Ipv4AddrTag::Deserialize(TagBuffer i)
{
    NS_LOG_FUNCTION(this << &i);
    uint8_t buf[4];
    i.Read(buf, 4);
    m_addr = Ipv4Address::Deserialize(buf);
}

void
Ipv4AddrTag::Print(std::ostream& os) const
{
    NS_LOG_FUNCTION(this << &os);
    os << "[" << m_addr << "] ";
}

// Map each of direct link state to its string representation.
static const std::string g_dirLinkStateName[NrSlUeProseDirectLink::NUM_STATES] = {"INIT",
                                                                                  "ESTABLISHING",
                                                                                  "ESTABLISHED",
                                                                                  "RELEASING",
                                                                                  "RELEASED"};

/**
 * \param s The direct link state.
 * \return The string representation of the given state.
 */
static const std::string&
ToString(NrSlUeProseDirectLink::DirectLinkState s)
{
    return g_dirLinkStateName[s];
}

TypeId
NrSlUeProseDirectLink::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlUeProseDirectLink")
            .SetParent<Object>()
            .SetGroupName("Nr")
            .AddConstructor<NrSlUeProseDirectLink>()
            .AddAttribute(
                "T5080",
                "Duration of Timer T5080 (Prose Direct Link Establishment Request Retransmission)"
                "Default value = 8 s, as per TS 24.554 Table 12.3.1 ",
                TimeValue(Seconds(8.0)),
                MakeTimeAccessor(&NrSlUeProseDirectLink::SetT5080),
                MakeTimeChecker())
            .AddAttribute(
                "PdlEsReqRtxMax",
                "Maximum number of Prose Direct Link Establishment Request Retransmissions",
                UintegerValue(3),
                MakeUintegerAccessor(&NrSlUeProseDirectLink::SetPdlEsReqRtxMax),
                MakeUintegerChecker<uint32_t>())
            .AddAttribute("T5084",
                          "Duration of Timer T5084 (Prose Keep-Alive Timer)"
                          "Default value = 5 s, as per TS 24.554 Table 12.3.1 ",
                          TimeValue(Seconds(5.0)),
                          MakeTimeAccessor(&NrSlUeProseDirectLink::SetT5084),
                          MakeTimeChecker())
            .AddAttribute("T5085",
                          "Duration of Timer T5085 (Prose Keep-Alive Request Retransmission)"
                          "Default value = 5 s, as per TS 24.554 Table 12.3.1 ",
                          TimeValue(Seconds(5.0)),
                          MakeTimeAccessor(&NrSlUeProseDirectLink::SetT5085),
                          MakeTimeChecker())
            .AddAttribute("PdlKaRtxMax",
                          "Maximum number of Prose Direct Link Keepalive Request Retransmissions",
                          UintegerValue(3),
                          MakeUintegerAccessor(&NrSlUeProseDirectLink::SetPdlKaRtxMax),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "T5087",
                "Duration of Timer T5087 (Prose Direct Link Release Request Retransmission)"
                "Default value = 5 s, as per TS 24.554 Table 12.3.1 ",
                TimeValue(Seconds(5.0)),
                MakeTimeAccessor(&NrSlUeProseDirectLink::SetT5087),
                MakeTimeChecker())
            .AddAttribute("PdlReReqRtxMax",
                          "Maximum number of Prose Direct Link Release Request Retransmissions",
                          UintegerValue(3),
                          MakeUintegerAccessor(&NrSlUeProseDirectLink::SetPdlReReqRtxMax),
                          MakeUintegerChecker<uint32_t>())
            .AddTraceSource(
                "KeepAliveFailure",
                "Trace fired upon failure of a ProSe keep-alive procedure",
                MakeTraceSourceAccessor(&NrSlUeProseDirectLink::m_keepAliveFailureTrace),
                "ns3::NrSlUeProseDirectLink::KeepAliveFailureTracedCallback");
    return tid;
}

NrSlUeProseDirectLink::NrSlUeProseDirectLink()
{
    NS_LOG_FUNCTION(this);

    m_selfL2Id = 0;
    m_peerL2Id = 0;
    m_isInitiating = false;
    m_isRelayConn = false;
    m_relayServiceCode = 0;

    m_state = INIT;
    m_nrSlUeProseDirLnkSapProvider =
        new MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink>(this);

    // Establishment parameters
    m_pdlEsParam.t5080 = new Timer();
    m_pdlEsParam.t5080->SetFunction(&NrSlUeProseDirectLink::T5080Expiry, this);
    m_pdlEsParam.rtxCounter = 0;

    // Keepalive parameters
    m_pdlKaParam.t5084 = new Timer();
    m_pdlKaParam.t5084->SetFunction(&NrSlUeProseDirectLink::T5084Expiry, this);
    m_pdlKaParam.t5085 = new Timer();
    m_pdlKaParam.t5085->SetFunction(&NrSlUeProseDirectLink::T5085Expiry, this);
    m_pdlKaParam.counter = 0;
    m_pdlKaParam.rtxCounter = 0;

    // Release parameters
    m_pdlReParam.t5087 = new Timer();
    m_pdlReParam.t5087->SetFunction(&NrSlUeProseDirectLink::T5087Expiry, this);
    m_pdlReParam.rtxCounter = 0;
}

NrSlUeProseDirectLink::~NrSlUeProseDirectLink(void)
{
    NS_LOG_FUNCTION(this);
}

void
NrSlUeProseDirectLink::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlUeProseDirLnkSapProvider;
    m_pdlEsParam.t5080->Cancel();
    delete m_pdlEsParam.t5080;
    m_pdlReParam.t5087->Cancel();
    delete m_pdlReParam.t5087;
}

void
NrSlUeProseDirectLink::SetParameters(uint32_t selfL2Id,
                                     uint32_t peerL2Id,
                                     bool isInitiating,
                                     bool isRelayConn,
                                     uint32_t relayServiceCode,
                                     Ipv4Address selfIp)
{
    NS_LOG_FUNCTION(this << selfL2Id << peerL2Id << isInitiating << isRelayConn << relayServiceCode
                         << selfIp);
    m_selfL2Id = selfL2Id;
    m_peerL2Id = peerL2Id;
    m_isInitiating = isInitiating;
    m_isRelayConn = isRelayConn;
    m_relayServiceCode = relayServiceCode;
    m_ipInfo.selfIpv4Addr = selfIp;
}

NrSlUeProseDirLnkSapProvider*
NrSlUeProseDirectLink::GetNrSlUeProseDirLnkSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeProseDirLnkSapProvider;
}

void
NrSlUeProseDirectLink::SetNrSlUeProseDirLnkSapUser(NrSlUeProseDirLnkSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeProseDirLnkSapUser = s;
}

void
NrSlUeProseDirectLink::SendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeProseDirLnkSapUser->SendNrSlPc5SMessage(packet, dstL2Id, lcId);
}

void
NrSlUeProseDirectLink::DoReceiveNrSlPc5Message(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrSlPc5SignallingMessageType pc5smt;
    packet->PeekHeader(pc5smt);

    switch (pc5smt.GetMessageType())
    {
    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest:
        ProcessDirectLinkEstablishmentRequest(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentAccept:
        ProcessDirectLinkEstablishmentAccept(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentReject:
        ProcessDirectLinkEstablishmentReject(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkReleaseRequest:
        ProcessDirectLinkReleaseRequest(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkReleaseAccept:
        ProcessDirectLinkReleaseAccept(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkKeepaliveRequest:
        ProcessDirectLinkKeepaliveRequest(packet);
        break;

    case NrSlPc5SignallingMessageType::ProseDirectLinkKeepaliveResponse:
        ProcessDirectLinkKeepaliveResponse(packet);
        break;

    default:
        NS_LOG_INFO("Unknown message type. Size: " << packet->GetSize());
        break;
    }
}

void
NrSlUeProseDirectLink::SwitchToState(DirectLinkState newState)
{
    NS_LOG_FUNCTION(this << ToString(m_state) << "-> " << ToString(newState));
    // DirectLinkState oldState = m_state;
    NS_LOG_INFO("Switch direct link from " << m_selfL2Id << " to " << m_peerL2Id << " from state "
                                           << ToString(m_state) << " to " << ToString(newState));
    // Notify ProSe layer
    NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info;
    info.oldStateEnum = m_state;
    m_state = newState;
    info.newStateEnum = newState;
    info.newStateStr = ToString(newState);

    switch (newState)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        info.ipInfo.peerIpv4Addr = m_ipInfo.peerIpv4Addr;
        info.ipInfo.selfIpv4Addr = m_ipInfo.selfIpv4Addr;

        if (m_isRelayConn)
        {
            NS_LOG_INFO("Relay");

            info.relayInfo.isRelayConn = true;
            info.relayInfo.relayServiceCode = m_relayServiceCode;
            if (m_isInitiating)
            {
                NS_LOG_INFO("-> Role: Remote UE");
                info.relayInfo.role = NrSlUeProseDirLnkSapUser::RemoteUe;
            }
            else
            {
                NS_LOG_INFO("-> Role: Relay UE");
                info.relayInfo.role = NrSlUeProseDirLnkSapUser::RelayUe;
            }
        }
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
    m_nrSlUeProseDirLnkSapUser->NotifyChangeOfDirectLinkState(m_peerL2Id, info);
}

void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    ProseDirectLinkEstablishmentRequest pdlEsReqHeader;
    packet->PeekHeader(pdlEsReqHeader);

    // Examine content for debug
    std::ostringstream oss(std::ostringstream::out);
    pdlEsReqHeader.Print(oss);
    NS_LOG_DEBUG("Message content: " << oss.str());
    oss.str("");

    bool accept = false;
    uint8_t cause = 0;

    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        // Normal case:
        // First time connecting to this UE
    case NrSlUeProseDirectLink::RELEASED:
        // Special case:
        //(Re)Connection establishment between a pair of UEs who performed the direct link release
        // process before

        // Change of state and notify ProSe layer about change of state
        SwitchToState(ESTABLISHING);

        // fall through; link could be in ESTABLISHING if requests collide
    case NrSlUeProseDirectLink::ESTABLISHING:

        // Security procedures are not currently implemented.
        // Security procedures are assumed to be completed successfully.

        if (m_isRelayConn && pdlEsReqHeader.GetRelayServiceCode() != 0)
        {
            uint32_t relaySC = pdlEsReqHeader.GetRelayServiceCode();
            NS_LOG_INFO(" Direct Link connection for Relay - DirectLinkEstablishmentRequest has "
                        "Relay Service Code: "
                        << relaySC);
            if (m_isRelayConn && !m_isInitiating // 1. This UE is a Relay UE
                && relaySC == m_relayServiceCode // 2. It provides the service pointed by the relay
                                                 // service code
                /*Not implemented at the moment */ // 3. It can accept a new connection
            )
            {
                NS_LOG_INFO(" UE does provide this service and can accept the connection");
                accept = true;
            }
            else
            {
                NS_LOG_INFO(" UE does not provide this service or cannot accept this service");
                cause = 1; // PC5 signalling cause value 00000001 = 'Direct communication to the
                           // target UE not allowed'
            }
        }
        else
        {
            NS_LOG_INFO(" Direct Link connection for Unicast");
            // Here we verify preconditions to accept direct link connection for unicast
            // At the moment we always accept
            accept = true;
        }

        if (!accept)
        {
            // Preconditions no satisfied. Reject.
            NS_LOG_INFO("Direct Link cannot be established");

            // Send reject message to the peer UE
            SendDirectLinkEstablishmentReject(cause);

            // Change of state and notify ProSe layer about change of state
            SwitchToState(RELEASED);
        }
        else
        {
            // Preconditions satisfied. Accept.
            NS_LOG_INFO("Direct Link can be established");

            // Retrieve tag with Ip of the peer UE and store it.
            // This may be replaced by proper IP configuration protocols in the future
            Ipv4AddrTag ipTag;
            packet->PeekPacketTag(ipTag);
            m_ipInfo.peerIpv4Addr = ipTag.GetAddress();
            ipTag.Print(oss);
            NS_LOG_DEBUG("Peer Ipv4 address: " << oss.str());

            // Send accept message to the peer UE
            SendDirectLinkEstablishmentAccept();

            // Change of state and notify ProSe layer about change of state
            SwitchToState(ESTABLISHED);

            // Start keep-alive timer
            NS_LOG_INFO("Starting timer T5084 (keep-alive procedure) towards " << m_peerL2Id);
            m_pdlKaParam.t5084->Schedule();

            if (m_pdlEsParam.t5080->IsRunning())
            {
                NS_LOG_INFO("Stopping timer T5080 (establishment request)");
                m_pdlEsParam.t5080->Cancel();
            }
        }
        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO("ESTABLISHED");
        // Possible causes identified so far:
        //- Initiating peer UE retransmitted request before accept was received
        //- Accept was lost and initiating peer UE retransmitted request

        // TS 24.554 section 7.2.2.6.2 Abnormal cases at the target UE:
        //"For a received PROSE DIRECT LINK ESTABLISHMENT REQUEST message from a
        // source layer-2 ID (for unicast communication), if the target UE already
        // has an existing link established to the UE known to use the same source
        // layer-2 ID, the same source user info, the same type of data (IP or
        // non-IP) and the same security policy, the UE shall process the new
        // request. However, the target UE shall only delete the existing link
        // context after the new link establishment procedure succeeds"

        // For simplicity, and as in this implementation 1. Security policies are not
        // implemented, and 2. Only one direct link is allowed between two peer UEs;
        // we simply retransmit the accept message
        // Possible outcomes identified so far:
        // 1. initiating UE receives one of the accept message, link is established
        //    on its side too.
        // 2. initiating UE timer T5080 expires and releases link on its side,
        //    and eventually, timer T5084 expires in this UE and link is released
        //    on this side too.
        SendDirectLinkEstablishmentAccept();

        break;
    case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO("In RELEASING state; reset current link and move to RELEASED");
        SwitchToState(RELEASED);
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentAccept(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    ProseDirectLinkEstablishmentAccept pdlEsAcHeader;
    packet->PeekHeader(pdlEsAcHeader);

    // Process message and store info
    // No field is really used at the moment.

    // Examine content for debug
    std::ostringstream oss(std::ostringstream::out);
    pdlEsAcHeader.Print(oss);
    NS_LOG_DEBUG("Message content: " << oss.str());
    oss.str("");

    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        // Ignore message
        NS_LOG_INFO("Ignoring message.");
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
        // Normal case

        {
            // Retrieve tag with Ip of the peer UE and store it.
            // This may be replaced by proper IP configuration protocols in the future
            Ipv4AddrTag ipTag;
            packet->PeekPacketTag(ipTag);
            m_ipInfo.peerIpv4Addr = ipTag.GetAddress();
            ipTag.Print(oss);
            NS_LOG_DEBUG("Peer Ipv4 address: " << oss.str());
        }

        // Cancel request retransmission timer
        NS_LOG_INFO("Stopping timer T5080 (establishment request)");
        m_pdlEsParam.t5080->Cancel();

        // Change of state and notify ProSe layer about change of state
        SwitchToState(ESTABLISHED);

        // Start keep-alive timer
        NS_LOG_INFO("Starting timer T5084 (keep-alive procedure) towards " << m_peerL2Id);
        m_pdlKaParam.t5084->Schedule();

        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        // Ignore message
        NS_LOG_INFO("Ignoring message.");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentReject(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    ProseDirectLinkEstablishmentReject pdlEsRjHeader;
    packet->PeekHeader(pdlEsRjHeader);

    // Examine content for debug
    std::ostringstream oss(std::ostringstream::out);
    pdlEsRjHeader.Print(oss);
    NS_LOG_DEBUG("Message content: " << oss.str());
    oss.str("");

    // Process message and store info if needed
    uint8_t cause = pdlEsRjHeader.GetPc5SignallingProtocolCause();

    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
        // Normal case

        // Causes from TS 24.554 Table 11.3.8.1
        switch (cause)
        {
        case 1:
            NS_LOG_INFO("Direct communication to the target UE not allowed");
            break;
        case 2:
            NS_LOG_INFO("Direct communication to the target UE no longer needed");
            break;
        case 3:
            NS_LOG_INFO("Conflict of layer-2 ID for unicast communication is detected");
            break;
        case 4:
            NS_LOG_INFO("Direct connection is not available anymore");
            break;
        case 5:
            NS_LOG_INFO("Lack of resources for PC5 unicast link");
            break;
        case 6:
            NS_LOG_INFO("Authentication failure");
            break;
        case 7:
            NS_LOG_INFO("Integrity failure");
            break;
        case 8:
            NS_LOG_INFO("UE security capabilities mismatch");
            break;
        case 9:
            NS_LOG_INFO("LSB of KNRP-sess ID conflict");
            break;
        case 10:
            NS_LOG_INFO("UE PC5 unicast signalling security policy mismatch");
            break;
        case 11:
            NS_LOG_INFO("Required service not allowed");
            break;
        case 12:
            NS_LOG_INFO("Security policy not aligned");
            break;
        case 13:
            NS_LOG_INFO("Congestion situation");
            break;
        case 14:
            NS_LOG_INFO("Authentication synchronisation error");
            break;
        case 15:
            NS_LOG_INFO("Security procedure failure of 5G ProSe UE-to-network relay");
            break;
        case 111:
            NS_LOG_INFO("Protocol error, unspecified");
            break;
        default:
            NS_FATAL_ERROR("Invalid PC5 Signalling Protocol Cause: " << +cause);
        }

        // Cancel request retransmission timer
        NS_LOG_INFO("Stopping timer T5080 (establishment request)");
        m_pdlEsParam.t5080->Cancel();

        // Change of state and notify ProSe layer about change of state
        SwitchToState(RELEASED);

        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
        // This should not happen, as the target UE should not send an Accept message
        // and then a Reject message
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO("Direct link is already releasing");
        break;
    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO("Direct link was recently released");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::StartConnectionEstablishment()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:

        // Send the request
        SendDirectLinkEstablishmentRequest();

        // Start retransmission timer
        NS_LOG_INFO("Starting timer T5080 (establishment request)");
        m_pdlEsParam.t5080->Schedule();

        // Change of state and notify ProSe layer about change of state
        SwitchToState(ESTABLISHING);

        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentRequest()
{
    NS_LOG_FUNCTION(this);

    uint8_t lcId = 0; // pdlEsReq is an unprotected PC5 message to be sent in SL-SRB0 (TS 38.331 -
                      // Section 9.1.14)

    // Create packet
    Ptr<Packet> pdlEsReqPacket = Create<Packet>();

    // Fill the request message header with the appropriated information
    ProseDirectLinkEstablishmentRequest pdlEsReqHeader;
    // Mandatory IEs
    pdlEsReqHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlEsReqHeader.SetSourceUserInfo(m_selfL2Id);

    std::vector<uint32_t> proseAppIds;
    proseAppIds.assign(1, 0); // Not used
    pdlEsReqHeader.SetProseApplicationIds(proseAppIds);
    std::vector<uint8_t> secCapabilities;
    secCapabilities.assign(1, 0); // Not used
    pdlEsReqHeader.SetUeSecurityCapabilities(secCapabilities);
    uint8_t ueSigSecPolicy = 0; // Not used
    pdlEsReqHeader.SetUeSignallingSecurityPolicy(ueSigSecPolicy);

    // Optional IEs
    pdlEsReqHeader.SetTargetUserInfo(m_peerL2Id);
    if (m_isRelayConn && m_isInitiating)
    {
        NS_LOG_INFO("Remote UE sending DirectLinkEstablishmentRequest");
        pdlEsReqHeader.SetRelayServiceCode(m_relayServiceCode);
    }

    // Store it for retransmission
    m_pdlEsParam.rqMsgCopy = pdlEsReqHeader;

    // Add header to packet
    pdlEsReqPacket->AddHeader(pdlEsReqHeader);

    // Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
    // This may be replaced by proper IP configuration protocols in the future
    Ipv4AddrTag ipTag;
    ipTag.SetAddress(m_ipInfo.selfIpv4Addr);
    pdlEsReqPacket->AddPacketTag(ipTag);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlEsReqHeader.GetMessageIdentifier());
    pdlEsReqPacket->AddByteTag(pc5SignallingTag);

    // Send it
    SendNrSlPc5SMessage(pdlEsReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentAccept()
{
    NS_LOG_FUNCTION(this);

    // pdlEsAc is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
    uint8_t lcId = 2;

    // Create the packet
    Ptr<Packet> pdlEsAcPacket = Create<Packet>();

    // Fill the accept message  header with the appropriated information
    ProseDirectLinkEstablishmentAccept pdlEsAcHeader;
    // Mandatory IEs
    pdlEsAcHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlEsAcHeader.SetSourceUserInfo(m_selfL2Id);
    std::vector<uint8_t> qosFlowDescriptions;
    qosFlowDescriptions.assign(3, 0); // Not used
    pdlEsAcHeader.SetPc5QoSFlowDescriptions(qosFlowDescriptions);
    uint8_t secConfig = 0; // Not used
    pdlEsAcHeader.SetUserPlaneSecurityProtectionConfiguration(secConfig);

    // Add header to packet
    pdlEsAcPacket->AddHeader(pdlEsAcHeader);

    // Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
    // This may be replaced by proper IP configuration protocols in the future
    Ipv4AddrTag ipTag;
    ipTag.SetAddress(m_ipInfo.selfIpv4Addr);
    pdlEsAcPacket->AddPacketTag(ipTag);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlEsAcHeader.GetMessageIdentifier());
    pdlEsAcPacket->AddByteTag(pc5SignallingTag);

    // Send it
    SendNrSlPc5SMessage(pdlEsAcPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentReject(uint8_t cause)
{
    NS_LOG_FUNCTION(this);

    // pdlEsRj is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
    uint8_t lcId = 2;

    // Create the packet
    Ptr<Packet> pdlEsRjPacket = Create<Packet>();

    // Fill the reject message with the appropriated information
    ProseDirectLinkEstablishmentReject pdlEsRjHeader;
    // All fields are mandatory fields in the reject message
    pdlEsRjHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlEsRjHeader.SetPc5SignallingProtocolCause(cause);

    // Add header to packet
    pdlEsRjPacket->AddHeader(pdlEsRjHeader);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlEsRjHeader.GetMessageIdentifier());
    pdlEsRjPacket->AddByteTag(pc5SignallingTag);

    // Send it
    SendNrSlPc5SMessage(pdlEsRjPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SetT5080(Time t)
{
    NS_LOG_FUNCTION(this << t.As(Time::S));
    m_pdlEsParam.t5080->SetDelay(t);
}

void
NrSlUeProseDirectLink::SetPdlEsReqRtxMax(uint32_t max)
{
    NS_LOG_FUNCTION(this << max);
    m_pdlEsParam.rtxMax = max;
}

void
NrSlUeProseDirectLink::T5080Expiry()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
        // Normal case
        if (m_pdlEsParam.rtxCounter < m_pdlEsParam.rtxMax)
        {
            // Retransmit request
            NS_LOG_INFO("Retransmitting Prose Direct Link Establishment Request");
            RetransmitDirectLinkEstablishmentRequest();

            // Increase retransmission counter
            m_pdlEsParam.rtxCounter++;

            // Restart timer
            NS_LOG_INFO("Stopping timer T5080 (establishment request)");
            m_pdlEsParam.t5080->Cancel();
            NS_LOG_INFO("Starting timer T5080 (establishment request)");
            m_pdlEsParam.t5080->Schedule();
        }
        else
        {
            NS_LOG_INFO("Maximum number of Prose Direct Link Establishment Request "
                        "retransmissions reached. Releasing link...");

            // Release link
            NS_LOG_INFO("Release procedure is initiating.");
            uint8_t cause = 5; // Lack of resources for PC5 unicast link
            StartConnectionRelease(cause);
        }
        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::RetransmitDirectLinkEstablishmentRequest()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> pdlEsReqPacket = Create<Packet>();
    uint8_t lcId = 0; // pdlEsReq is an unprotected PC5 message to be sent in SL-SRB0 (TS 38.331 -
                      // Section 9.1.14)
    pdlEsReqPacket->AddHeader(m_pdlEsParam.rqMsgCopy);

    // Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
    // This may be replaced by proper IP configuration protocols in the future
    Ipv4AddrTag ipTag;
    ipTag.SetAddress(m_ipInfo.selfIpv4Addr);
    pdlEsReqPacket->AddPacketTag(ipTag);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(m_pdlEsParam.rqMsgCopy.GetMessageIdentifier());
    pdlEsReqPacket->AddByteTag(pc5SignallingTag);

    SendNrSlPc5SMessage(pdlEsReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SetT5084(Time t)
{
    NS_LOG_FUNCTION(this << t.As(Time::S));
    m_pdlKaParam.t5084->SetDelay(t);
}

void
NrSlUeProseDirectLink::SetT5085(Time t)
{
    NS_LOG_FUNCTION(this << t.As(Time::S));
    m_pdlKaParam.t5085->SetDelay(t);
}

void
NrSlUeProseDirectLink::T5084Expiry()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_DEBUG("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO("Sending ProSe Direct Link Keepalive Request");
        SendDirectLinkKeepaliveRequest();
        NS_LOG_INFO("Resetting T5085 (keep-alive request) counter");
        m_pdlKaParam.rtxCounter = 0;
        NS_LOG_INFO("Starting timer T5085 (keep-alive request)");
        m_pdlKaParam.t5085->Schedule();
        break;
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::T5085Expiry()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_DEBUG("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
        // Normal case
        if (m_pdlKaParam.rtxCounter < m_pdlKaParam.rtxMax)
        {
            // Retransmit request
            NS_LOG_INFO("Retransmitting Prose Direct Link Keepalive Request");
            RetransmitDirectLinkKeepaliveRequest();

            // Increase retransmission counter
            m_pdlKaParam.rtxCounter++;

            // Restart timer
            NS_LOG_INFO("Starting timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Schedule();
        }
        else
        {
            NS_LOG_INFO("Maximum number of Prose Direct Link Keepalive Request "
                        "retransmissions reached. Releasing link...");

            // Release link
            NS_LOG_INFO("Release procedure is initiating.");
            uint8_t cause = 4; // Link no longer available
            StartConnectionRelease(cause);
            m_keepAliveFailureTrace(m_selfL2Id, m_peerL2Id);
        }
        break;
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_DEBUG("T5085 (keep-alive request) expired while releasing or already released.");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::StartConnectionRelease(uint8_t cause)
{
    NS_LOG_FUNCTION(this << cause);
    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        NS_FATAL_ERROR("Unexpected state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:

        // Send release request
        // Cause can be: (3GPP 24.554)
        //   #1 direct communication to the target UE not allowed;
        //   #2 direct communication to the target UE no longer needed;
        //   #4 direct connection is not available anymore;
        //   #5 lack of resources for PC5 unicast link; or
        //   #111   protocol error, unspecified.

        SendDirectLinkReleaseRequest(cause);

        // Stop T5080 timer
        if (m_pdlEsParam.t5080->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5080 (establishment request)");
            m_pdlEsParam.t5080->Cancel();
        }
        // Stop T5084 timer
        if (m_pdlKaParam.t5084->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure)");
            m_pdlKaParam.t5084->Cancel();
        }
        // Stop T5085 timer
        if (m_pdlKaParam.t5085->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Cancel();
        }
        // Start T5087 timer
        if (m_pdlReParam.t5087->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5087 (release request)");
            m_pdlReParam.t5087->Cancel();
        }
        NS_LOG_INFO("Starting timer T5087 (release request)");
        m_pdlReParam.t5087->Schedule();

        // Change the state and notify the ProSe layer
        SwitchToState(RELEASING);

        break;
    case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO("Direct link is being released ...");
        break;
    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO("Direct link was recently released!");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::SendDirectLinkReleaseRequest(uint8_t cause)
{
    NS_LOG_FUNCTION(this << cause);
    uint8_t lcId = 0;

    // Create packet
    Ptr<Packet> pdlReReqPacket = Create<Packet>();

    // Fill the request message header with the appropriated information
    ProseDirectLinkReleaseRequest pdlReReqHeader;
    // Mandatory IEs
    pdlReReqHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlReReqHeader.SetPc5SignallingProtocolCause(cause);
    pdlReReqHeader.SetMsbKnrpId(0);
    pdlReReqHeader.SetBackoffValue(0);

    // Store it for retransmission
    m_pdlReParam.rqMsgCopy = pdlReReqHeader;

    // Add header to packet
    pdlReReqPacket->AddHeader(pdlReReqHeader);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlReReqHeader.GetMessageIdentifier());
    pdlReReqPacket->AddByteTag(pc5SignallingTag);

    // Send it
    SendNrSlPc5SMessage(pdlReReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkReleaseAccept()
{
    NS_LOG_FUNCTION(this);
    uint8_t lcId = 2;

    // Create the packet
    Ptr<Packet> pdlReAcPacket = Create<Packet>();

    // Fill the accept message  header with the appropriated information
    ProseDirectLinkReleaseAccept pdlReAcHeader;
    // Mandatory IEs
    pdlReAcHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());

    // Add header to packet
    pdlReAcPacket->AddHeader(pdlReAcHeader);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlReAcHeader.GetMessageIdentifier());
    pdlReAcPacket->AddByteTag(pc5SignallingTag);

    // Send it
    SendNrSlPc5SMessage(pdlReAcPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SetPdlKaRtxMax(uint32_t max)
{
    NS_LOG_FUNCTION(this << max);
    m_pdlKaParam.rtxMax = max;
}

void
NrSlUeProseDirectLink::SetT5087(Time t)
{
    NS_LOG_FUNCTION(this);
    m_pdlReParam.t5087->SetDelay(t);
}

void
NrSlUeProseDirectLink::SetPdlReReqRtxMax(uint32_t max)
{
    NS_LOG_FUNCTION(this << max);
    m_pdlReParam.rtxMax = max;
}

void
NrSlUeProseDirectLink::T5087Expiry()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("In state: " << ToString(m_state));
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
        NS_FATAL_ERROR("Unexpected in state " << ToString(m_state));
        break;
    case NrSlUeProseDirectLink::RELEASING:
        // Normal case
        if (m_pdlReParam.rtxCounter < m_pdlReParam.rtxMax)
        {
            // Retransmit request
            NS_LOG_INFO("Retransmitting Prose Direct Link Release Request");
            RetransmitDirectLinkReleaseRequest();

            // Increase retransmission counter
            m_pdlReParam.rtxCounter++;

            // Restart timer
            NS_LOG_INFO("Stopping timer T5087 (release request)");
            m_pdlReParam.t5087->Cancel();
            NS_LOG_INFO("Starting timer T5087 (release request)");
            m_pdlReParam.t5087->Schedule();
        }
        else
        {
            NS_LOG_INFO("Maximum number of Prose Direct Link Release Request "
                        "retransmissions reached. Releasing link...");

            // Release link locally
            NS_LOG_INFO("Stopping timer T5087 (release request)");
            m_pdlReParam.t5087->Cancel();
            SwitchToState(RELEASED);
        }
        break;

    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO("Connection already released!");
        m_pdlReParam.t5087->Cancel();
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::RetransmitDirectLinkReleaseRequest()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> pdlReReqPacket = Create<Packet>();
    uint8_t lcId = 0;
    pdlReReqPacket->AddHeader(m_pdlReParam.rqMsgCopy);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(m_pdlReParam.rqMsgCopy.GetMessageIdentifier());
    pdlReReqPacket->AddByteTag(pc5SignallingTag);

    SendNrSlPc5SMessage(pdlReReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::ProcessDirectLinkReleaseRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    ProseDirectLinkReleaseRequest pdlReReqHeader;
    packet->PeekHeader(pdlReReqHeader);

    // Examine content for debug
    std::ostringstream oss(std::ostringstream::out);
    pdlReReqHeader.Print(oss);
    NS_LOG_DEBUG("Message content: " << oss.str());
    oss.str("");

    uint8_t cause = pdlReReqHeader.GetPc5SignallingProtocolCause();

    NS_LOG_INFO("In state: " << ToString(m_state));

    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        NS_LOG_INFO("Ignore message");
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
        // Stop T5080 timer
        if (m_pdlEsParam.t5080->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5080 (establishment request)");
            m_pdlEsParam.t5080->Cancel();
        }
        // Stop T5084 timer
        if (m_pdlKaParam.t5084->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure)");
            m_pdlKaParam.t5084->Cancel();
        }
        // Stop T5085 timer
        if (m_pdlKaParam.t5085->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Cancel();
        }

        SwitchToState(RELEASING);

        // Causes from TS 24.554 Table 11.3.8.1
        switch (cause)
        {
        case 1:
            NS_LOG_INFO("Direct communication to the target UE not allowed");
            break;
        case 2:
            NS_LOG_INFO("Direct communication to the target UE no longer needed");
            break;
        case 4:
            NS_LOG_INFO("Direct connection is not available anymore");
            break;
        case 5:
            NS_LOG_INFO("Lack of resources for PC5 unicast link");
            break;
        case 13:
            NS_LOG_INFO("Congestion situation");
            break;
        case 111:
            NS_LOG_INFO("Protocol error, unspecified");
            break;
        default:
            NS_FATAL_ERROR("Invalid PC5 Signalling Protocol Cause: " << +cause);
        }

        NS_LOG_INFO("Direct Link can be released");

        // Send accept message to the peer UE
        SendDirectLinkReleaseAccept();

        // Change of state and notify ProSe layer about change of state
        SwitchToState(RELEASED);

        break;
    case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO("Internal state here: this should not happen!");
        break;
    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO("Direct link already released");
        // Retransmit ReleaseAccept message
        SendDirectLinkReleaseAccept();
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::ProcessDirectLinkReleaseAccept(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    ProseDirectLinkReleaseAccept pdlReAcHeader;
    packet->PeekHeader(pdlReAcHeader);

    // Process message and store info
    // No field is really used at the moment.

    // Examine content for debug
    std::ostringstream oss(std::ostringstream::out);
    pdlReAcHeader.Print(oss);
    NS_LOG_DEBUG("Message content: " << oss.str());
    oss.str("");

    NS_LOG_INFO("In state: " << ToString(m_state));

    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
        // Ignore message
        NS_LOG_INFO("Ignoring message.");
        break;
    case NrSlUeProseDirectLink::RELEASING:

        // Cancel request retransmission timer
        NS_LOG_INFO("Stopping timer T5087 (release request)");
        m_pdlReParam.t5087->Cancel();

        // Change of state and notify ProSe layer about change of state
        SwitchToState(RELEASED);
        break;

    case NrSlUeProseDirectLink::RELEASED:
        // Ignore message
        NS_LOG_INFO("Ignoring message. Link already released!");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::ProcessDirectLinkKeepaliveRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
        // Ignore message
        NS_LOG_INFO("Ignoring keep-alive message from " << m_peerL2Id << " to " << m_selfL2Id);
        break;
    case NrSlUeProseDirectLink::ESTABLISHED: {
        ProseDirectLinkKeepaliveRequest pdlKaReqHeader;
        packet->PeekHeader(pdlKaReqHeader);

        // Examine content for debug
        std::ostringstream oss(std::ostringstream::out);
        pdlKaReqHeader.Print(oss);
        NS_LOG_DEBUG("Message content: " << oss.str());
        oss.str("");

        uint32_t counter = pdlKaReqHeader.GetKeepaliveCounter();

        // Send response message to the peer UE
        NS_LOG_INFO("Responding to keep-alive message from " << m_peerL2Id << " to " << m_selfL2Id);
        SendDirectLinkKeepaliveResponse(counter);

        if (m_pdlKaParam.t5085->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Cancel();
            m_pdlKaParam.counter++;
        }

        if (m_pdlKaParam.t5084->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure)");
            m_pdlKaParam.t5084->Cancel();
        }

        NS_LOG_INFO("Starting timer T5084 (keep-alive procedure) towards " << m_peerL2Id);
        m_pdlKaParam.t5084->Schedule();

        break;
    }
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        // Ignore message
        NS_LOG_INFO("Ignoring message. Link already released!");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::ProcessDirectLinkKeepaliveResponse(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
    case NrSlUeProseDirectLink::ESTABLISHING:
        // Ignore message
        NS_LOG_INFO("Ignoring message.");
        break;
    case NrSlUeProseDirectLink::ESTABLISHED: {
        ProseDirectLinkKeepaliveResponse pdlKaResHeader;
        packet->PeekHeader(pdlKaResHeader);

        // Examine content for debug
        std::ostringstream oss(std::ostringstream::out);
        pdlKaResHeader.Print(oss);
        NS_LOG_DEBUG("Message content: " << oss.str());
        oss.str("");

        uint32_t counter = pdlKaResHeader.GetKeepaliveCounter();
        if (counter == m_pdlKaParam.counter)
        {
            NS_LOG_DEBUG("Received keep-alive response from " << m_peerL2Id << " to " << m_selfL2Id
                                                              << "; canceling retx timer");
            m_pdlKaParam.counter++;
            NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Cancel();
            NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure)");
            m_pdlKaParam.t5084->Cancel();
            NS_LOG_INFO("Starting timer T5084 (keep-alive procedure) towards " << m_peerL2Id);
            m_pdlKaParam.t5084->Schedule();
            NS_LOG_INFO("Resetting T5085 (keep-alive request) counter");
            m_pdlKaParam.rtxCounter = 0;
        }
        else if (counter < m_pdlKaParam.counter)
        {
            NS_LOG_DEBUG("Received stale keep-alive response from " << m_peerL2Id << " to "
                                                                    << m_selfL2Id << "; ignoring");
        }
        else
        {
            NS_FATAL_ERROR("counter " << counter << " greater than " << m_pdlKaParam.counter);
        }
        break;
    }
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        // Ignore message
        NS_LOG_INFO("Ignoring message. Link already released!");
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::SendDirectLinkKeepaliveRequest()
{
    NS_LOG_FUNCTION(this);
    // pdlKaReq is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
    uint8_t lcId = 2;

    // Create the packet
    Ptr<Packet> pdlKaReqPacket = Create<Packet>();

    ProseDirectLinkKeepaliveRequest pdlKaReqHeader;
    pdlKaReqHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlKaReqHeader.SetKeepaliveCounter(m_pdlKaParam.counter);
    m_pdlKaParam.kaMsgCopy = pdlKaReqHeader;

    // Add header to packet
    pdlKaReqPacket->AddHeader(pdlKaReqHeader);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlKaReqHeader.GetMessageIdentifier());
    pdlKaReqPacket->AddByteTag(pc5SignallingTag);

    // Send it
    NS_LOG_DEBUG("Send Keepalive request from " << m_selfL2Id << " to " << m_peerL2Id);
    SendNrSlPc5SMessage(pdlKaReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkKeepaliveResponse(uint32_t counter)
{
    NS_LOG_FUNCTION(this << counter);
    // pdlKaRes is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
    uint8_t lcId = 2;

    // Create the packet
    Ptr<Packet> pdlKaResPacket = Create<Packet>();

    ProseDirectLinkKeepaliveResponse pdlKaResHeader;
    pdlKaResHeader.SetSequenceNumber(m_pc5SigMsgSeqNum.GenerateSeqNum());
    pdlKaResHeader.SetKeepaliveCounter(counter);

    // Add header to packet
    pdlKaResPacket->AddHeader(pdlKaResHeader);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(pdlKaResHeader.GetMessageIdentifier());
    pdlKaResPacket->AddByteTag(pc5SignallingTag);

    // Send it
    NS_LOG_DEBUG("Send Keepalive response from " << m_selfL2Id << " to " << m_peerL2Id);
    SendNrSlPc5SMessage(pdlKaResPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::RetransmitDirectLinkKeepaliveRequest()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> pdlKaReqPacket = Create<Packet>();
    uint8_t lcId = 2; // pdlKaReq is an protected PC5 message to be sent in SL-SRB2 (TS 38.331 -
                      // Section 9.1.14)
    pdlKaReqPacket->AddHeader(m_pdlKaParam.kaMsgCopy);

    NrSlProsePc5SignallingTag pc5SignallingTag;
    pc5SignallingTag.SetType(m_pdlKaParam.kaMsgCopy.GetMessageIdentifier());
    pdlKaReqPacket->AddByteTag(pc5SignallingTag);

    NS_LOG_DEBUG("Retransmit Keepalive request from " << m_selfL2Id << " to " << m_peerL2Id);
    SendNrSlPc5SMessage(pdlKaReqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::ResetCurrentLink()
{
    NS_LOG_FUNCTION(this);
    switch (m_state)
    {
    case NrSlUeProseDirectLink::INIT:
        // No link would have existed
        NS_LOG_INFO("This should not happen!");
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
    case NrSlUeProseDirectLink::ESTABLISHED:
    case NrSlUeProseDirectLink::RELEASING:
    case NrSlUeProseDirectLink::RELEASED:
        // UE is in the process of establishing or releasing the link
        // Stop T5084 timer
        if (m_pdlKaParam.t5084->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure)");
            m_pdlKaParam.t5084->Cancel();
        }
        // Stop T5085 timer
        if (m_pdlKaParam.t5085->IsRunning())
        {
            NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
            m_pdlKaParam.t5085->Cancel();
        }

        // Reset T5080 timer and related counter
        NS_LOG_INFO("Stopping timer T5080 (establishment request)");
        m_pdlEsParam.t5080->Cancel();
        NS_LOG_INFO("Resetting T5080 (establishment request) counter");
        m_pdlEsParam.rtxCounter = 0;
        // Reset T5087 and related counter
        NS_LOG_INFO("Stopping timer T5087 (release request)");
        m_pdlReParam.t5087->Cancel();
        NS_LOG_INFO("Resetting T5087 (release request) counter");
        m_pdlReParam.rtxCounter = 0;

        if (m_isInitiating)
        {
            // Initialize state
            SwitchToState(INIT);
            // Initiate the establishment process if UE is a remote or initiating connection
            // establishment
            StartConnectionEstablishment();
        }
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << ToString(m_state));
    }
}

void
NrSlUeProseDirectLink::NotifyDataReceived()
{
    NS_LOG_FUNCTION(this);

    if (m_pdlKaParam.t5085->IsRunning())
    {
        NS_LOG_INFO("Stopping timer T5085 (keep-alive request)");
        m_pdlKaParam.t5085->Cancel();
        // Count this as a completed ProSe keep-alive procedure
        m_pdlKaParam.counter++;
    }

    if (m_pdlKaParam.t5084->IsRunning())
    {
        NS_LOG_INFO("Stopping timer T5084 (keep-alive procedure) " << this);
        m_pdlKaParam.t5084->Cancel();
    }

    if (m_state == NrSlUeProseDirectLink::DirectLinkState::ESTABLISHED)
    {
        NS_LOG_INFO("Starting timer T5084 (keep-alive procedure) towards " << m_peerL2Id);
        m_pdlKaParam.t5084->Schedule();
    }
}

std::ostream&
operator<<(std::ostream& os, enum NrSlUeProseDirectLink::DirectLinkState e)
{
    switch (e)
    {
    case NrSlUeProseDirectLink::DirectLinkState::INIT:
        os << "INIT";
        break;
    case NrSlUeProseDirectLink::DirectLinkState::ESTABLISHING:
        os << "ESTABLISHING";
        break;
    case NrSlUeProseDirectLink::DirectLinkState::ESTABLISHED:
        os << "ESTABLISHED";
        break;
    case NrSlUeProseDirectLink::DirectLinkState::RELEASING:
        os << "RELEASING";
        break;
    case NrSlUeProseDirectLink::DirectLinkState::RELEASED:
        os << "RELEASED";
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << static_cast<uint16_t>(e));
        break;
    }

    return os;
}

} // namespace ns3
