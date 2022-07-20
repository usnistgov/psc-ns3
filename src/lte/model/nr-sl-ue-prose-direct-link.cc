/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include "nr-sl-ue-prose-direct-link.h"

#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-radio-bearer-info.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeProseDirectLink");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeProseDirectLink);


Ipv4AddrTag::Ipv4AddrTag ()
  : m_addr (Ipv4Address ())
{
  NS_LOG_FUNCTION (this);
}

void
Ipv4AddrTag::SetAddress (Ipv4Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_addr = addr;
}

Ipv4Address
Ipv4AddrTag::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_addr;
}

TypeId
Ipv4AddrTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Ipv4AddrTag")
    .SetParent<Tag> ()
    .SetGroupName ("Lte")
    .AddConstructor<Ipv4AddrTag> ()
  ;
  return tid;
}
TypeId
Ipv4AddrTag::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Ipv4AddrTag::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 4 + sizeof (uint32_t) + sizeof (uint8_t);
}
void
Ipv4AddrTag::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this << &i);
  uint8_t buf[4];
  m_addr.Serialize (buf);
  i.Write (buf, 4);
}
void
Ipv4AddrTag::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this << &i);
  uint8_t buf[4];
  i.Read (buf, 4);
  m_addr = Ipv4Address::Deserialize (buf);
}
void
Ipv4AddrTag::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "[" << m_addr << "] ";
}

// Map each of direct link state to its string representation.
static const std::string g_dirLinkStateName[NrSlUeProseDirectLink::NUM_STATES] =
{
  "INIT",
  "ESTABLISHING",
  "ESTABLISHED",
  "RELEASING",
  "RELEASED"
};

/**
 * \param s The direct link state.
 * \return The string representation of the given state.
 */
static const std::string & ToString (NrSlUeProseDirectLink::DirectLinkState s)
{
  return g_dirLinkStateName[s];
}

TypeId NrSlUeProseDirectLink::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProseDirectLink")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<NrSlUeProseDirectLink> ()
    .AddAttribute ("T5080",
                   "Duration of Timer T5080 (Prose Direct Link Establishment Request Retransmission) in seconds. "
                   "Default value = 8 s, as per TS 24.554 Table 12.3.1 ",
                   TimeValue (Seconds (8.0)),
                   MakeTimeAccessor (&NrSlUeProseDirectLink::SetT5080),
                   MakeTimeChecker ())
    .AddAttribute ("PdlEsRqRtxMax",
                   "Maximum number of Prose Direct Link Establishment Request Retransmissions",
                   UintegerValue (3),
                   MakeUintegerAccessor (&NrSlUeProseDirectLink::SetPdlEsRqRtxMax),
                   MakeUintegerChecker<uint32_t>())
  ;
  return tid;
}
NrSlUeProseDirectLink::NrSlUeProseDirectLink ()
{
  NS_LOG_FUNCTION (this);

  m_selfL2Id = 0;
  m_peerL2Id = 0;
  m_isInitiating = false;
  m_isRelayConn = false;
  m_relayServiceCode = 0;

  m_state = INIT;
  m_nrSlUeProseDirLnkSapProvider = new MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink> (this);

  //Establishment parameters
  m_pdlEsParam.t5080 = new Timer ();
  m_pdlEsParam.t5080->SetFunction (&NrSlUeProseDirectLink::T5080Expiry, this);
  m_pdlEsParam.rtxCounter = 0;
}


NrSlUeProseDirectLink::~NrSlUeProseDirectLink (void)
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeProseDirectLink::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_nrSlUeProseDirLnkSapProvider;
  m_pdlEsParam.t5080->Cancel ();
  delete m_pdlEsParam.t5080;
}

void
NrSlUeProseDirectLink::SetParameters (uint32_t selfL2Id, uint32_t peerL2Id,
                                      bool isInitiating, bool isRelayConn,
                                      uint32_t relayServiceCode, Ipv4Address selfIp)
{
  NS_LOG_FUNCTION (this << selfL2Id << peerL2Id << isInitiating << isRelayConn << relayServiceCode << selfIp);
  m_selfL2Id = selfL2Id;
  m_peerL2Id = peerL2Id;
  m_isInitiating = isInitiating;
  m_isRelayConn = isRelayConn;
  m_relayServiceCode = relayServiceCode;
  m_ipInfo.selfIpv4Addr = selfIp;
}

NrSlUeProseDirLnkSapProvider*
NrSlUeProseDirectLink::GetNrSlUeProseDirLnkSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeProseDirLnkSapProvider;
}

void
NrSlUeProseDirectLink::SetNrSlUeProseDirLnkSapUser (NrSlUeProseDirLnkSapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeProseDirLnkSapUser = s;
}

void
NrSlUeProseDirectLink::SendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeProseDirLnkSapUser->SendNrSlPc5SMessage (packet, dstL2Id, lcId);

}
void
NrSlUeProseDirectLink::DoReceiveNrSlPc5Message (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  NrSlPc5SignallingMessageType pc5smt;
  packet->PeekHeader (pc5smt);

  switch (pc5smt.GetMessageType ())
    {
      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest:
        ProcessDirectLinkEstablishmentRequest (packet);
        break;


      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentAccept:
        ProcessDirectLinkEstablishmentAccept (packet);

        break;


      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentReject:
        ProcessDirectLinkEstablishmentReject (packet);

        break;

      default:
        NS_LOG_INFO ("Unknown message type. Size: " << packet->GetSize ());

        break;
    }
}

void
NrSlUeProseDirectLink::SwitchToState (DirectLinkState newState)
{
  NS_LOG_FUNCTION (this << ToString (m_state) << "-> " << ToString (newState));
  //DirectLinkState oldState = m_state;
  m_state = newState;
  //Notify ProSe layer
  NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info;
  info.newStateEnum = newState;
  info.newStateStr = ToString (newState);

  switch (newState)
    {
      case NrSlUeProseDirectLink::INIT:
      case NrSlUeProseDirectLink::ESTABLISHING:
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        info.ipInfo.peerIpv4Addr = m_ipInfo.peerIpv4Addr;
        info.ipInfo.selfIpv4Addr = m_ipInfo.selfIpv4Addr;

        if (m_isRelayConn)
          {
            NS_LOG_INFO ("Relay");

            info.relayInfo.isRelayConn = true;
            info.relayInfo.relayServiceCode = m_relayServiceCode;
            if (m_isInitiating)
              {
                NS_LOG_INFO ("-> Role: Remote UE");
                info.relayInfo.role = NrSlUeProseDirLnkSapUser::RemoteUe;
              }
            else
              {
                NS_LOG_INFO ("-> Role: Relay UE");
                info.relayInfo.role = NrSlUeProseDirLnkSapUser::RelayUe;
              }
          }
        break;
      case NrSlUeProseDirectLink::RELEASING:
      case NrSlUeProseDirectLink::RELEASED:
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }
  m_nrSlUeProseDirLnkSapUser->NotifyChangeOfDirectLinkState (m_peerL2Id, info);
}

void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentRequest (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  ProseDirectLinkEstablishmentRequest pdlEsRqHeader;
  packet->PeekHeader (pdlEsRqHeader);

  // Examine content for debug
  std::ostringstream oss (std::ostringstream::out);
  pdlEsRqHeader.Print (oss);
  NS_LOG_DEBUG ("Message content: " << oss.str ());
  oss.str ("");

  bool accept = false;
  uint8_t cause = 0;


  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        NS_LOG_INFO ("INIT");
        //Normal case

        //Change of state and notify ProSe layer about change of state
        SwitchToState (ESTABLISHING);

        //Security procedures are not currently implemented.
        //Security procedures are assumed to be completed successfully.

        if (m_isRelayConn && pdlEsRqHeader.GetRelayServiceCode () != 0)
          {
            uint32_t relaySC = pdlEsRqHeader.GetRelayServiceCode ();
            NS_LOG_INFO (" Direct Link connection for Relay - DirectLinkEstablishmentRequest has Relay Service Code: " << relaySC);
            if (m_isRelayConn && !m_isInitiating            //1. This UE is a Relay UE
                && relaySC == m_relayServiceCode            //2. It provides the service pointed by the relay service code
                /*Not implemented at the moment */          //3. It can accept a new connection
                )
              {
                NS_LOG_INFO (" UE does provide this service and can accept the connection");
                accept = true;
              }
            else
              {
                NS_LOG_INFO (" UE does not provide this service or cannot accept this service");
                cause = 1; // PC5 signalling cause value 00000001 = 'Direct communication to the target UE not allowed'
              }
          }
        else
          {
            NS_LOG_INFO (" Direct Link connection for Unicast");
            // Here we verify preconditions to accept direct link connection for unicast
            // At the moment we always accept
            accept = true;
          }

        if (!accept)
          {
            //Preconditions no satisfied. Reject.
            NS_LOG_INFO ("Direct Link cannot be established");

            //Send reject message to the peer UE
            SendDirectLinkEstablishmentReject (cause);

            //Change of state and notify ProSe layer about change of state
            SwitchToState (RELEASED);
          }
        else
          {
            //Preconditions satisfied. Accept.
            NS_LOG_INFO ("Direct Link can be established");

            //Retrieve tag with Ip of the peer UE and store it.
            //This may be replaced by proper IP configuration protocols in the future
            Ipv4AddrTag ipTag;
            packet->PeekPacketTag (ipTag);
            m_ipInfo.peerIpv4Addr = ipTag.GetAddress ();
            ipTag.Print (oss);
            NS_LOG_DEBUG ("Peer Ipv4 address: " << oss.str ());

            //Send accept message to the peer UE
            SendDirectLinkEstablishmentAccept ();

            //Change of state and notify ProSe layer about change of state
            SwitchToState (ESTABLISHED);

          }
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        NS_LOG_INFO ("ESTABLISHING");
        //This should not happen, as ESTABLISHING is an internal state to this function
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO ("ESTABLISHED");
        //Possible causes identified so far:
        //- Initiating peer UE retransmitted request before accept was received
        //- Accept was lost and initiating peer UE retransmitted request

        //TS 24.554 section 7.2.2.6.2 Abnormal cases at the target UE:
        //"For a received PROSE DIRECT LINK ESTABLISHMENT REQUEST message from a
        //source layer-2 ID (for unicast communication), if the target UE already
        //has an existing link established to the UE known to use the same source
        //layer-2 ID, the same source user info, the same type of data (IP or
        //non-IP) and the same security policy, the UE shall process the new
        //request. However, the target UE shall only delete the existing link
        //context after the new link establishment procedure succeeds"

        //For simplicity, and as in this implementation 1. Security policies are not
        //implemented, and 2. Only one direct link is allowed between two peer UEs;
        //we simply retransmit the accept message
        //Possible outcomes identified so far:
        //1. initiating UE receives one of the accept message, link is established
        //   on its side too.
        //2. initiating UE timer T5080 expires and releases link on its side,
        //   and eventually, timer T5084 expires in this UE and link is released
        //   on this side too.
        SendDirectLinkEstablishmentAccept ();

        break;
      case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO ("RELEASING");
        //Ignore message
        break;
      case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO ("RELEASED");
        //Ignore message
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }

}

void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentAccept (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  ProseDirectLinkEstablishmentAccept pdlEsAcHeader;
  packet->PeekHeader (pdlEsAcHeader);

  //Process message and store info
  //No field is really used at the moment.

  // Examine content for debug
  std::ostringstream oss (std::ostringstream::out);
  pdlEsAcHeader.Print (oss);
  NS_LOG_DEBUG ("Message content: " << oss.str ());
  oss.str ("");

  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        //Ignore message
        NS_LOG_INFO ("Ignoring message." );
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        //Normal case

        {
          //Retrieve tag with Ip of the peer UE and store it.
          //This may be replaced by proper IP configuration protocols in the future
          Ipv4AddrTag ipTag;
          packet->PeekPacketTag (ipTag);
          m_ipInfo.peerIpv4Addr = ipTag.GetAddress ();
          ipTag.Print (oss);
          NS_LOG_DEBUG ("Peer Ipv4 address: " << oss.str ());
        }

        //Cancel request retransmission timer
        m_pdlEsParam.t5080->Remove ();

        //Change of state and notify ProSe layer about change of state
        SwitchToState (ESTABLISHED);

        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
      case NrSlUeProseDirectLink::RELEASING:
      case NrSlUeProseDirectLink::RELEASED:
        //Ignore message
        NS_LOG_INFO ("Ignoring message." );
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }


}
void
NrSlUeProseDirectLink::ProcessDirectLinkEstablishmentReject (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  ProseDirectLinkEstablishmentReject pdlEsRjHeader;
  packet->PeekHeader (pdlEsRjHeader);

  // Examine content for debug
  std::ostringstream oss (std::ostringstream::out);
  pdlEsRjHeader.Print (oss);
  NS_LOG_DEBUG ("Message content: " << oss.str ());
  oss.str ("");

  //Process message and store info if needed
  uint8_t cause =  pdlEsRjHeader.GetPc5SignallingProtocolCause ();

  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        //Normal case

        //Causes from TS 24.554 Table 11.3.8.1
        switch (cause)
          {
            case 1:
              NS_LOG_INFO ("Direct communication to the target UE not allowed");
              break;
            case 2:
              NS_LOG_INFO ("Direct communication to the target UE no longer needed");
              break;
            case 3:
              NS_LOG_INFO ("Conflict of layer-2 ID for unicast communication is detected");
              break;
            case 4:
              NS_LOG_INFO ("Direct connection is not available anymore");
              break;
            case 5:
              NS_LOG_INFO ("Lack of resources for PC5 unicast link");
              break;
            case 6:
              NS_LOG_INFO ("Authentication failure");
              break;
            case 7:
              NS_LOG_INFO ("Integrity failure");
              break;
            case 8:
              NS_LOG_INFO ("UE security capabilities mismatch");
              break;
            case 9:
              NS_LOG_INFO ("LSB of KNRP-sess ID conflict");
              break;
            case 10:
              NS_LOG_INFO ("UE PC5 unicast signalling security policy mismatch");
              break;
            case 11:
              NS_LOG_INFO (" Required service not allowed");
              break;
            case 12:
              NS_LOG_INFO ("Security policy not aligned");
              break;
            case 111:
              NS_LOG_INFO ("Protocol error, unspecified");
              break;
            default:
              NS_FATAL_ERROR ("Invalid PC5 Signalling Protocol Cause: " << +cause);
          }

        //Cancel request retransmission timer
        m_pdlEsParam.t5080->Remove ();


        //Change of state and notify ProSe layer about change of state
        SwitchToState (RELEASED);

        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        //This should not happen, as the target UE should not send an Accept message
        //and then a Reject message
        NS_FATAL_ERROR ("Unexpected in state " << ToString (m_state));
        break;
      case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO ("Direct link is already releasing");
        break;
      case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO ("Direct link was recently released");
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }
}


void
NrSlUeProseDirectLink::StartConnectionEstablishment ()
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:

        //Send the request
        SendDirectLinkEstablishmentRequest ();

        //Start retransmission timer
        m_pdlEsParam.t5080->Schedule ();

        //Change of state and notify ProSe layer about change of state
        SwitchToState (ESTABLISHING);

        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
      case NrSlUeProseDirectLink::ESTABLISHED:
      case NrSlUeProseDirectLink::RELEASING:
      case NrSlUeProseDirectLink::RELEASED:
        NS_FATAL_ERROR ("Unexpected in state " << ToString (m_state));
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentRequest ()
{
  NS_LOG_FUNCTION (this);

  uint8_t lcId = 0;  //pdlEsRq is an unprotected PC5 message to be sent in SL-SRB0 (TS 38.331 - Section 9.1.14)

  //Create packet
  Ptr<Packet> pdlEsRqPacket = Create<Packet>();

  //Fill the request message header with the appropriated information
  ProseDirectLinkEstablishmentRequest pdlEsRqHeader;
  //Mandatory IEs
  pdlEsRqHeader.SetSequenceNumber (m_pc5SigMsgSeqNum.GenerateSeqNum ());
  pdlEsRqHeader.SetSourceUserInfo (m_selfL2Id);

  std::vector<uint32_t> proseAppIds;
  proseAppIds.assign (1, 0); //Not used
  pdlEsRqHeader.SetProseApplicationIds (proseAppIds);
  std::vector<uint8_t> secCapabilities;
  secCapabilities.assign (1, 0); //Not used
  pdlEsRqHeader.SetUeSecurityCapabilities (secCapabilities);
  uint8_t ueSigSecPolicy = 0; //Not used
  pdlEsRqHeader.SetUeSignallingSecurityPolicy (ueSigSecPolicy);

  //Optional IEs
  pdlEsRqHeader.SetTargetUserInfo (m_peerL2Id);
  if (m_isRelayConn && m_isInitiating)
    {
      NS_LOG_INFO ("Remote UE sending DirectLinkEstablishmentRequest");
      pdlEsRqHeader.SetRelayServiceCode (m_relayServiceCode);
    }

  //Store it for retransmission
  m_pdlEsParam.rqMsgCopy = pdlEsRqHeader;

  //Add header to packet
  pdlEsRqPacket->AddHeader (pdlEsRqHeader);

  //Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
  //This may be replaced by proper IP configuration protocols in the future
  Ipv4AddrTag ipTag;
  ipTag.SetAddress (m_ipInfo.selfIpv4Addr);
  pdlEsRqPacket->AddPacketTag (ipTag);

  //Send it
  SendNrSlPc5SMessage (pdlEsRqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentAccept ()
{
  NS_LOG_FUNCTION (this);

  uint8_t lcId = 2;  //pdlEsAc is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)

  //Create the packet
  Ptr<Packet> pdlEsAcPacket = Create<Packet>();

  //Fill the accept message  header with the appropriated information
  ProseDirectLinkEstablishmentAccept pdlEsAcHeader;
  //Mandatory IEs
  pdlEsAcHeader.SetSequenceNumber (m_pc5SigMsgSeqNum.GenerateSeqNum ());
  pdlEsAcHeader.SetSourceUserInfo (m_selfL2Id);
  std::vector<uint8_t> qosFlowDescriptions;
  qosFlowDescriptions.assign (3, 0); //Not used
  pdlEsAcHeader.SetPc5QoSFlowDescriptions (qosFlowDescriptions);
  uint8_t secConfig = 0; //Not used
  pdlEsAcHeader.SetUserPlaneSecurityProtectionConfiguration (secConfig);

  //Add header to packet
  pdlEsAcPacket->AddHeader (pdlEsAcHeader);

  //Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
  //This may be replaced by proper IP configuration protocols in the future
  Ipv4AddrTag ipTag;
  ipTag.SetAddress (m_ipInfo.selfIpv4Addr);
  pdlEsAcPacket->AddPacketTag (ipTag);

  //Send it
  SendNrSlPc5SMessage (pdlEsAcPacket, m_peerL2Id, lcId);
}
void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentReject (uint8_t cause)
{
  NS_LOG_FUNCTION (this);

  uint8_t lcId = 2;  //pdlEsRj is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)

  //Create the packet
  Ptr<Packet> pdlEsRjPacket = Create<Packet>();

  //Fill the reject message with the appropriated information
  ProseDirectLinkEstablishmentReject pdlEsRjHeader;
  //All fields are mandatory fields in the reject message
  pdlEsRjHeader.SetSequenceNumber (m_pc5SigMsgSeqNum.GenerateSeqNum ());
  pdlEsRjHeader.SetPc5SignallingProtocolCause (cause);

  //Add header to packet
  pdlEsRjPacket->AddHeader (pdlEsRjHeader);

  //Send it
  SendNrSlPc5SMessage (pdlEsRjPacket, m_peerL2Id, lcId);

}

void
NrSlUeProseDirectLink::SetT5080 (Time t)
{
  NS_LOG_FUNCTION (this);
  m_pdlEsParam.t5080->SetDelay (t);
}

void
NrSlUeProseDirectLink::SetPdlEsRqRtxMax (uint32_t max)
{
  NS_LOG_FUNCTION (this << max);
  m_pdlEsParam.rtxMax = max;
}

void
NrSlUeProseDirectLink::T5080Expiry ()
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        NS_FATAL_ERROR ("Unexpected in state " << ToString (m_state));
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        //Normal case
        if (m_pdlEsParam.rtxCounter < m_pdlEsParam.rtxMax)
          {
            //Retransmit request
            NS_LOG_INFO ("Retransmitting Prose Direct Link Establishment Request" );
            RetransmitDirectLinkEstablishmentRequest ();

            // Increase retransmission counter
            m_pdlEsParam.rtxCounter++;

            //Restart timer
            m_pdlEsParam.t5080->Cancel ();
            m_pdlEsParam.t5080->Schedule ();
          }
        else
          {
            NS_LOG_INFO ("Maximum number of Prose Direct Link Establishment Request "
                         "retransmissions reached. Releasing link..." );

            //Release link
            //Not implemented at the moment
            NS_LOG_INFO ("Release procedure is not implemented." );
          }
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
      case NrSlUeProseDirectLink::RELEASING:
      case NrSlUeProseDirectLink::RELEASED:
        NS_FATAL_ERROR ("Unexpected in state " << ToString (m_state));
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << ToString (m_state));
    }
}

void
NrSlUeProseDirectLink::RetransmitDirectLinkEstablishmentRequest ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> pdlEsRqPacket = Create<Packet>();
  uint8_t lcId = 0;  //pdlEsRq is an unprotected PC5 message to be sent in SL-SRB0 (TS 38.331 - Section 9.1.14)
  pdlEsRqPacket->AddHeader (m_pdlEsParam.rqMsgCopy);

  //Add tag with selfIp for easy SL-DRB and TFTs configuration in the peer UE
  //This may be replaced by proper IP configuration protocols in the future
  Ipv4AddrTag ipTag;
  ipTag.SetAddress (m_ipInfo.selfIpv4Addr);
  pdlEsRqPacket->AddPacketTag (ipTag);

  SendNrSlPc5SMessage (pdlEsRqPacket, m_peerL2Id, lcId);
}


} // namespace ns3
