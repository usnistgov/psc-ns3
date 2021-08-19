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
                   "Duration of Timer T5080 (Prose Direct Link Establishment Request Retransmission) in seconds",
                   TimeValue (Seconds (1.0)),
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
  NS_FATAL_ERROR ("Please use the parameterized constructor ");

}

NrSlUeProseDirectLink::NrSlUeProseDirectLink (uint32_t selfL2Id, uint32_t peerL2Id, bool isInitiating, bool ideal, Ipv4Address selfIp)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeProseDirLnkSapProvider = new MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink> (this);
  m_selfL2Id = selfL2Id;
  m_peerL2Id = peerL2Id;
  m_isInitiating = isInitiating;
  m_ideal = ideal;
  m_state = INIT;
  //Establishment parameters
  m_pdlEsParam.t5080 = new Timer ();
  m_pdlEsParam.t5080->SetFunction (&NrSlUeProseDirectLink::T5080Expiry, this);
  m_pdlEsParam.rtxCounter = 0;
  m_pdlEsParam.rtxMax = 0;
  //Ip parameters
  m_ipInfo.selfIpv4Addr = selfIp;
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
NrSlUeProseDirectLink::SetPeerNrSlUeProseDirLnkSapProvider (NrSlUeProseDirLnkSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_peerNrSlUeProseDirLnkSapProvider = s;
}

void
NrSlUeProseDirectLink::SendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId)
{
  NS_LOG_FUNCTION (this);
  if (m_ideal)
    {
      //Use m_peerNrSlUeProseDirLnkSapProvider
    }
  else
    {
      // Use ProSe layer
      m_nrSlUeProseDirLnkSapUser->SendNrSlPc5SMessage (packet, dstL2Id, lcId);
    }
}
void
NrSlUeProseDirectLink::DoReceiveNrSlPc5Message (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  //Here we will have the logic to differentiate between different type of signaling messages
  //and what entity/function take care of which
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

  bool accept;
  uint8_t cause;

  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        NS_LOG_INFO ("INIT");
        //Normal case

        //Change of state and notify ProSe layer about change of state
        SwitchToState (ESTABLISHING);

        //Security procedures are not currently implemented.
        //Security procedures are assumed to be completed successfully.

        // TODO Verify Preconditions to accept direct link connection

        accept = true;
        cause = 0;

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
            //TODO: This is a shortcut to speed development and may be replaced
            //      by proper IP configuration protocols in the future
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
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO ("ESTABLISHED");
        //Possible causes identified so far:  (TODO any other)
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
        //Possible outcomes identified so far: (TODO any other?)
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

        //Process message and store info
        //TODO

        //Cancel request retransmission timer
        m_pdlEsParam.t5080->Remove ();

        {
          //Retrieve tag with Ip of the peer UE and store it.
          //TODO: This is a shortcut to speed development and may be replaced
          //      by proper IP configuration protocols in the future
          Ipv4AddrTag ipTag;
          packet->PeekPacketTag (ipTag);
          m_ipInfo.peerIpv4Addr = ipTag.GetAddress ();
          ipTag.Print (oss);
          NS_LOG_DEBUG ("Peer Ipv4 address: " << oss.str ());
        }

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

  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        //Normal case

        //Process message and store info if needed
        //TODO

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

  ProseDirectLinkEstablishmentRequest pdlEsRqHeader;
  Ptr<Packet> pdlEsRqPacket = Create<Packet>();


  NS_LOG_INFO ("In state: " << ToString (m_state) );
  switch (m_state)
    {
      case NrSlUeProseDirectLink::INIT:
        //Normal case

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

  ProseDirectLinkEstablishmentRequest pdlEsRqHeader;
  Ptr<Packet> pdlEsRqPacket = Create<Packet>();

  //Fill the request message with the appropriated information
  //TODO

  //Store it for retransmission
  m_pdlEsParam.rqMsgCopy = pdlEsRqHeader;

  //Add tag with selfIp for ease SL-DRB and TFTs configuration in the peer UE
  //TODO: This is a shortcut to speed development and may be replaced
  //      by proper IP configuration protocols in the future
  Ipv4AddrTag ipTag;
  ipTag.SetAddress (m_ipInfo.selfIpv4Addr);
  pdlEsRqPacket->AddPacketTag (ipTag);


  uint8_t lcId = 0;  //pdlEsRq is an unprotected PC5 message to be sent in SL-SRB0 (TS 38.331 - Section 9.1.14)
  pdlEsRqPacket->AddHeader (pdlEsRqHeader);

  //Send it
  SendNrSlPc5SMessage (pdlEsRqPacket, m_peerL2Id, lcId);
}

void
NrSlUeProseDirectLink::SendDirectLinkEstablishmentAccept ()
{
  NS_LOG_FUNCTION (this);

  ProseDirectLinkEstablishmentAccept pdlEsAcHeader;
  //Fill the accept message with the appropriated information
  //TODO

  uint8_t lcId = 2;  //pdlEsAc is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
  Ptr<Packet> pdlEsAcPacket = Create<Packet>();
  pdlEsAcPacket->AddHeader (pdlEsAcHeader);

  //Add tag with selfIp for ease SL-DRB and TFTs configuration in the peer UE
  //TODO: This is a shortcut to speed development and may be replaced
  //      by proper IP configuration protocols in the future
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

  ProseDirectLinkEstablishmentReject pdlEsRjHeader;
  //Fill the reject message with the appropriated information
  //TODO

  uint8_t lcId = 2;  //pdlEsAc is a protected PC5 message to be sent in SL-SRB2 (TS 38.331 - Section 9.1.14)
  Ptr<Packet> pdlEsRjPacket = Create<Packet>();
  pdlEsRjPacket->AddHeader (pdlEsRjHeader);
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
            //TODO
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

  SendNrSlPc5SMessage (pdlEsRqPacket, m_peerL2Id, lcId);
}


} // namespace ns3
