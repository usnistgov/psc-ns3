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

#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/buffer.h>
#include "nr-sl-pc5-signalling-header.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlPc5SignallingHeader");
NS_OBJECT_ENSURE_REGISTERED (NrSlPc5SignallingMessageType);
NS_OBJECT_ENSURE_REGISTERED (ProseDirectLinkEstablishmentRequest);
NS_OBJECT_ENSURE_REGISTERED (ProseDirectLinkEstablishmentAccept);
NS_OBJECT_ENSURE_REGISTERED (ProseDirectLinkEstablishmentReject);


// Initialize static member
uint8_t NrPc5SignallingHeaderSequenceNumber::s_seqNum = 0;

/** Size of the Nonce field */
static const uint8_t NonceSize = 16;

/** Size of the link local IPv6 address field */
static const uint8_t LinkLocalIpv6AddressSize = 16;

/** Map message type value to acronym */
static const std::vector<std::string> s_acronyms
{
  "None", "PDL-Es-Rq", "PDL-Es-Ac", "PDL-Es-Rj"
};

/** Map message type value to string name */
static const std::vector<std::string> s_names
{
  "None",
  "PROSE DIRECT LINK ESTABLISHMENT REQUEST",
  "PROSE DIRECT LINK ESTABLISHMENT ACCEPT",
  "PROSE DIRECT LINK ESTABLISHMENT REJECT"
};

/*****     NrSlPc5SignallingMessageType : To peek at the first byte of the received message and identify the type       *****/

NrSlPc5SignallingMessageType::NrSlPc5SignallingMessageType ()
{
  m_msgType = 0;
}

NrSlPc5SignallingMessageType::~NrSlPc5SignallingMessageType ()
{}

TypeId
NrSlPc5SignallingMessageType::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrSlPc5SignallingMessageType")
    .SetParent<Header> ()
    .AddConstructor<NrSlPc5SignallingMessageType> ()
  ;
  return tid;
}

TypeId
NrSlPc5SignallingMessageType::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
NrSlPc5SignallingMessageType::Print (std::ostream &os) const
{
  os << "msgName: " << GetMessageName () << " ";
}

uint32_t
NrSlPc5SignallingMessageType::GetSerializedSize (void) const
{
  return sizeof (m_msgType);
}

void
NrSlPc5SignallingMessageType::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgType);
}

uint32_t
NrSlPc5SignallingMessageType::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgType = i.ReadU8 ();

  return GetSerializedSize ();
}

uint8_t
NrSlPc5SignallingMessageType::GetMessageType ()
{
  return m_msgType;
}

std::string
NrSlPc5SignallingMessageType::GetMessageAcronym (void) const
{
  return s_acronyms[m_msgType];
}

std::string
NrSlPc5SignallingMessageType::GetMessageName (void) const
{
  return s_names[m_msgType];
}

void
NrSlPc5SignallingMessageType::SetMessageType (uint8_t msgType)
{
  switch (msgType)
    {
      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest:
        m_msgType = msgType;
        break;
      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentAccept:
        m_msgType = msgType;
        break;
      case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentReject:
        m_msgType = msgType;
        break;
      default:
        NS_ABORT_MSG ("Cannot set the Message Type to the input provided");
        break;
    }
}

/*****     ProseDirectLinkEstablishmentRequest Message       *****/

ProseDirectLinkEstablishmentRequest::ProseDirectLinkEstablishmentRequest ()
{
  m_msgId = 1;
  m_seqNum = 0;
  m_proseAppIds.clear ();
  m_proseAppIds.assign (1, 0);
  m_srcUserInfo = 0;
  m_secCapabilities.clear ();
  m_secCapabilities.assign (2, 0);
  m_ueSigSecPolicy = 0;
  m_keyEsInfoContainer = 0;
  m_hasKeyEsInfoContainer = false;
  m_nonce1.clear ();
  m_nonce1.assign (NonceSize, 0);
  m_hasNonce1 = false;
  m_msb = 0;
  m_hasMsb = false;
  m_tgtUserInfo = 0;
  m_hasTgtUserInfo = false;
  m_knrpId = 0;
  m_hasKnrpId = false;
  m_relayServiceCode = 0;
  m_hasRelayServiceCode = false;
}

ProseDirectLinkEstablishmentRequest::~ProseDirectLinkEstablishmentRequest ()
{}

TypeId
ProseDirectLinkEstablishmentRequest::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProseDirectLinkEstablishmentRequest")
    .SetParent<Header> ()
    .AddConstructor<ProseDirectLinkEstablishmentRequest> ()
  ;
  return tid;
}

TypeId
ProseDirectLinkEstablishmentRequest::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ProseDirectLinkEstablishmentRequest::Print (std::ostream &os) const
{

  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << +m_seqNum << " ";
  os << " proseAppIds: ";
  for (std::vector<uint32_t>::const_iterator it = m_proseAppIds.begin (); it != m_proseAppIds.end (); ++it)
    {
      os << (*it) << " ";
    }
  os << "srcUserInfo: " << m_srcUserInfo << " ";
  os << "secCapabilities: ";
  for (std::vector<uint8_t>::const_iterator it = m_secCapabilities.begin (); it != m_secCapabilities.end (); ++it)
    {
      os << +(*it) << " ";
    }
  os << "ueSigSecPolicy: " << +m_ueSigSecPolicy << " ";
  if (m_hasKeyEsInfoContainer)
    {
      os << "keyEsInfoContainer: " << +m_keyEsInfoContainer << " ";
    }
  if (m_hasNonce1)
    {
      os << " nonce1: ";
      for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
        {
          os << +(*it) << " ";
        }
    }
  if (m_hasMsb)
    {
      os << "msb: " << +m_msb << " ";
    }
  if (m_hasTgtUserInfo)
    {
      os << "tgtUserInfo: " << +m_tgtUserInfo << " ";
    }
  if (m_hasKnrpId)
    {
      os << "knrpId: " << m_knrpId << " ";
    }
  if (m_hasRelayServiceCode)
    {
      os << "relayServiceCode: " << m_relayServiceCode << " ";
    }
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = size
    + sizeof (m_msgId)
    + sizeof (m_seqNum)
    + 1   // ProSe application identifier IEI octet
    + 1   // Length of ProSe application identifier content octet
    + m_proseAppIds.size () * sizeof (uint32_t)  //ProSe application identifier contents
    + 1   // Application layer ID IEI octet (Source user info)
    + 1   // Length of Application layer ID content octet (Source user info)
    + sizeof (m_srcUserInfo)   // Application layer ID contents (Source user info)
    + 1   // UE security capabilities  IEI octet
    + 1   // Length of UE security capabilities  content octet
    + m_secCapabilities.size () * sizeof (uint8_t)  //UE security capabilities contents
    + 1   // UE PC5 unicast signalling security policy IEI octet
    + sizeof (m_ueSigSecPolicy);   // UE PC5 unicast signalling security policy
  if (m_hasKeyEsInfoContainer)
    {
      size = size
        + 1   // Key establishment information container IEI octet
        + 1   // Length of Key establishment information container content octet
        + sizeof (m_keyEsInfoContainer);   // Key establishment information container contents
    }
  if (m_hasNonce1)
    {
      size = size
        + 1   // Nonce IEI octet
        + NonceSize;   // Nonce IEI contents
    }
  if (m_hasMsb)
    {
      size = size
        + 1   // Most significant bits of the KNRP-sess ID IEI octet
        + sizeof (m_msb);   // Most significant bits of the KNRP-sess ID IEI contents
    }
  if (m_hasTgtUserInfo)
    {
      size = size
        + 1   // Application layer ID IEI octet (Target user info)
        + 1   // Length of Application layer ID content octet (Target user info)
        + sizeof (m_tgtUserInfo);   // Application layer ID contents (Target user info)
    }
  if  (m_hasKnrpId)
    {
      size = size
        + 1   // Knrp ID IEI octet
        + sizeof (m_knrpId);   // Knrp ID content
    }
  if (m_hasRelayServiceCode)
    {
      size = size
        + 1   // Relay service code IEI octet
        + 3;   // Relay service code content
    }
  return size;
}

void
ProseDirectLinkEstablishmentRequest::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteU8 (m_seqNum);

  i.WriteU8 (0); // ProSe application identifier IEI octet //TODO: Not defined in the standard ATM
  i.WriteU8 (m_proseAppIds.size () * sizeof (uint32_t)); // Length of ProSe application identifier content octet

  // ProSe application identifier content:
  for (std::vector<uint32_t>::const_iterator it = m_proseAppIds.begin (); it != m_proseAppIds.end (); ++it)
    {
      i.WriteU32 (*it);
    }

  i.WriteU8 (0); // Application layer ID IEI octet (Source user info) //TODO: Not defined in the standard ATM
  i.WriteU8 (sizeof (m_srcUserInfo)); // Length of Application layer ID content octet (Source user info)
  i.WriteU32 (m_srcUserInfo); // Application layer ID contents (Source user info)

  i.WriteU8 (0); // UE security capabilities  IEI octet //TODO: Not defined in the standard ATM
  i.WriteU8 (m_secCapabilities.size () * sizeof (uint8_t)); // Length of UE security capabilities  content octet

  //UE security capabilities contents:
  for (std::vector<uint8_t>::const_iterator it = m_secCapabilities.begin (); it != m_secCapabilities.end (); ++it)
    {
      i.WriteU8 (*it);
    }

  i.WriteU8 (0); // UE PC5 unicast signalling security policy IEI octet //TODO: Not defined in the standard ATM
  i.WriteU8 (m_ueSigSecPolicy); // UE PC5 unicast signalling security policy

  if (m_hasKeyEsInfoContainer)
    {
      i.WriteU8 (74); // Key establishment information container IEI octet
      i.WriteU8 (sizeof (m_keyEsInfoContainer)); // Length of Key establishment information container content octet
      i.WriteU8 (m_keyEsInfoContainer); // Key establishment information container contents
    }
  if (m_hasNonce1)
    {
      i.WriteU8 (53); // Nonce IEI octet
      // // Nonce IEI contents:
      for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasMsb)
    {
      i.WriteU8 (54); // Most significant bits of the KNRP-sess ID IEI octet
      i.WriteU8 (m_msb); // Most significant bits of the KNRP-sess ID contents
    }
  if (m_hasTgtUserInfo)
    {
      i.WriteU8 (28); // Application layer ID IEI octet (Target user info)
      i.WriteU8 (sizeof (m_tgtUserInfo)); // Length of Application layer ID content octet (Target user info)
      i.WriteU32 (m_tgtUserInfo); // Application layer ID contents (Target user info)
    }
  if  (m_hasKnrpId)
    {
      i.WriteU8 (52); // Knrp ID IEI octet
      i.WriteU32 (m_knrpId); // Knrp ID IEI contents
    }
  if (m_hasRelayServiceCode)
    {
      i.WriteU8 (25); // Relay service code IEI octet //TODO: Not defined in the standard ATM, using LTE value
      i.WriteU8 ((m_relayServiceCode >> 0) & 0xff);
      i.WriteU8 ((m_relayServiceCode >> 8) & 0xff);
      i.WriteU8 ((m_relayServiceCode >> 16) & 0xff);
    }
}

uint32_t
ProseDirectLinkEstablishmentRequest::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  uint8_t size_in_bytes;

  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadU8 ();

  i.ReadU8 ();// ProSe application identifier IEI octet
  size_in_bytes = i.ReadU8 (); // Length of ProSe application identifier content octet
  // ProSe application identifier content:
  for (uint8_t index = 0; index < (size_in_bytes / sizeof (uint32_t)); index++)
    {
      m_proseAppIds[index] = i.ReadU32 ();
    }

  i.ReadU8 (); // Application layer ID IEI octet (Source user info)
  i.ReadU8 (); // Length of Application layer ID content octet (Source user info)
  m_srcUserInfo = i.ReadU32 (); // Application layer ID contents (Source user info)

  i.ReadU8 ();  // UE security capabilities  IEI octet
  size_in_bytes = i.ReadU8 ();  // Length of UE security capabilities content octet
  // UE security capabilities contents:
  for (uint8_t index = 0; index < size_in_bytes; index++)
    {
      m_secCapabilities[index] = i.ReadU8 ();
    }

  i.ReadU8 ();// UE PC5 unicast signalling security policy IEI octet
  m_ueSigSecPolicy =  i.ReadU8 (); // UE PC5 unicast signalling security policy

  uint8_t opt_vars_iei = 0;
  uint8_t byte0, byte1, byte2;
  uint32_t relayServiceCode_dummy;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
          case 74: // Key establishment information container IEI
            i.ReadU8 (); // Length of Key establishment information container content octet
            m_keyEsInfoContainer = i.ReadU8 ();
            m_hasKeyEsInfoContainer = true;
            break;
          case 53: // Nonce IEI
            for (uint8_t index = 0; index < NonceSize; index++)
              {
                m_nonce1[index] = i.ReadU8 ();
              }
            m_hasNonce1 = true;
            break;
          case 54: // Most significant bits of the KNRP-sess ID IEI
            m_msb = i.ReadU8 ();
            m_hasMsb = true;
            break;
          case 28: // Application layer ID IEI octet (Target user info)
            i.ReadU8 (); // Length of Application layer ID content octet (Target user info)
            m_tgtUserInfo = i.ReadU32 ();
            m_hasTgtUserInfo = true;
            break;
          case 52:
            m_knrpId = i.ReadU32 ();
            m_hasKnrpId = true;
            break;
          case 25:
            byte0 = i.ReadU8 ();
            byte1 = i.ReadU8 ();
            byte2 = i.ReadU8 ();
            relayServiceCode_dummy = byte2;
            relayServiceCode_dummy <<= 8;
            relayServiceCode_dummy |= byte1;
            relayServiceCode_dummy <<= 8;
            relayServiceCode_dummy |= byte0;
            m_relayServiceCode = relayServiceCode_dummy;
            m_hasRelayServiceCode = true;
            break;
          default:
            break;
        }
    }
  return GetSerializedSize ();
}

void
ProseDirectLinkEstablishmentRequest::SetParameters (
  uint8_t seq,
  std::vector<uint32_t> proseAppIds,
  uint32_t srcUserInfo,
  std::vector<uint8_t> secCapabilities,
  uint8_t ueSigSecPolicy,
  uint8_t keyEsInfoContainer,
  std::vector<uint8_t> nonce1,
  uint8_t msb,
  uint32_t tgtUserInfo,
  uint32_t knrpId,
  uint32_t relayServiceCode
  )
{
  //Mandatory IEs
  SetSequenceNumber (seq);
  SetProseApplicationIds (proseAppIds);
  SetSourceUserInfo (srcUserInfo);
  SetUeSecurityCapabilities (secCapabilities);
  SetUeSignallingSecurityPolicy (ueSigSecPolicy);

  //Optional IEs
  if (keyEsInfoContainer != 0)
    {
      SetKeyEstablishmentInfoContainer (keyEsInfoContainer);
    }
  if (nonce1.size () > 0)
    {
      SetNonce1 (nonce1);
    }
  if (msb != 0)
    {
      SetMsbKdId (msb);
    }
  if (tgtUserInfo != 0)
    {
      SetTargetUserInfo (tgtUserInfo);
    }
  if (knrpId != 0)
    {
      SetKnrpId (knrpId);
    }
  if (relayServiceCode != 0)
    {
      SetRelayServiceCode (relayServiceCode);
    }
}


uint8_t
ProseDirectLinkEstablishmentRequest::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
ProseDirectLinkEstablishmentRequest::GetSequenceNumber ()
{
  return m_seqNum;
}

void
ProseDirectLinkEstablishmentRequest::SetSequenceNumber (uint8_t seq)
{
  m_seqNum = seq;
}

void
ProseDirectLinkEstablishmentRequest::SetProseApplicationIds (std::vector<uint32_t> proseAppIds)
{
  m_proseAppIds.clear ();
  m_proseAppIds.assign (proseAppIds.size (), 0);
  for (uint16_t i = 0; i < proseAppIds.size (); i++)
    {
      m_proseAppIds[i] = proseAppIds[i];
    }
}

std::vector<uint32_t>
ProseDirectLinkEstablishmentRequest::GetProseApplicationIds ()
{
  return m_proseAppIds;
}

void
ProseDirectLinkEstablishmentRequest::SetSourceUserInfo (uint32_t srcUserInfo)
{
  m_srcUserInfo = srcUserInfo;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetSourceUserInfo ()
{
  return m_srcUserInfo;
}

void
ProseDirectLinkEstablishmentRequest::SetUeSecurityCapabilities (std::vector<uint8_t> secCapabilities)
{
  m_secCapabilities.clear ();
  m_secCapabilities.assign (secCapabilities.size (), 0);
  for (uint16_t i = 0; i < secCapabilities.size (); i++)
    {
      m_secCapabilities[i] = secCapabilities[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentRequest::GetUeSecurityCapabilities ()
{
  return m_secCapabilities;
}

void
ProseDirectLinkEstablishmentRequest::SetUeSignallingSecurityPolicy (uint8_t ueSigSecPolicy)
{
  m_ueSigSecPolicy = ueSigSecPolicy;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetUeSignallingSecurityPolicy ()
{
  return m_ueSigSecPolicy;
}

void
ProseDirectLinkEstablishmentRequest::SetKeyEstablishmentInfoContainer (uint8_t keyEsInfoContainer)
{
  m_hasKeyEsInfoContainer = true;
  m_keyEsInfoContainer = keyEsInfoContainer;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetKeyEstablishmentInfoContainer ()
{
  return m_keyEsInfoContainer;
}

void
ProseDirectLinkEstablishmentRequest::SetNonce1 (std::vector<uint8_t> nonce1)
{
  m_hasNonce1 = true;
  m_nonce1.assign (NonceSize, 0);
  for (uint16_t i = 0; i < nonce1.size (); i++)
    {
      m_nonce1[i] = nonce1[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentRequest::GetNonce1 ()
{
  return m_nonce1;
}

void
ProseDirectLinkEstablishmentRequest::SetMsbKdId (uint8_t msb)
{
  m_hasMsb = true;
  m_msb = msb;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetMsbKdId ()
{
  return m_msb;
}

void
ProseDirectLinkEstablishmentRequest::SetTargetUserInfo (uint32_t tgtUserInfo)
{
  m_hasTgtUserInfo = true;
  m_tgtUserInfo = tgtUserInfo;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetTargetUserInfo ()
{
  return m_tgtUserInfo;
}

void
ProseDirectLinkEstablishmentRequest::SetKnrpId (uint32_t knrpId)
{
  m_hasKnrpId = true;
  m_knrpId = knrpId;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetKnrpId ()
{
  return m_knrpId;
}

void
ProseDirectLinkEstablishmentRequest::SetRelayServiceCode (uint32_t relayServiceCode)
{
  m_hasRelayServiceCode = true;
  m_relayServiceCode = relayServiceCode;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetRelayServiceCode ()
{
  return m_relayServiceCode;
}

/*****     ProseDirectLinkEstablishmentAccept Message       *****/

ProseDirectLinkEstablishmentAccept::ProseDirectLinkEstablishmentAccept ()
{
  //Mandatory IEs
  m_msgId = 2;
  m_seqNum = 0;
  m_srcUserInfo = 0;
  m_qosFlowDescriptions.clear ();
  m_qosFlowDescriptions.assign (3, 0);
  m_userPlaneSecConfig = 0;
  //Optional IEs
  m_hasIpAddressConfig = false;
  m_ipAddressConfig = 0;
  m_hasLinkLocalIpv6Address = false;
  m_linkLocalIpv6Address.clear ();
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize, 0);
}

ProseDirectLinkEstablishmentAccept::~ProseDirectLinkEstablishmentAccept ()
{}

TypeId
ProseDirectLinkEstablishmentAccept::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProseDirectLinkEstablishmentAccept")
    .SetParent<Header> ()
    .AddConstructor<ProseDirectLinkEstablishmentAccept> ()
  ;
  return tid;
}

TypeId
ProseDirectLinkEstablishmentAccept::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ProseDirectLinkEstablishmentAccept::Print (std::ostream &os) const
{
  os << "msgId: " << +m_msgId << " "
     << "seqNum: " << +m_seqNum << " "
     << "srcUserInfo: " << m_srcUserInfo << " ";
  os << "qosFlowDescriptions: ";
  for (std::vector<uint8_t>::const_iterator it = m_qosFlowDescriptions.begin (); it != m_qosFlowDescriptions.end (); ++it)
    {
      os << +(*it) << " ";
    }
  os << "userPlaneSecConfig: " << +m_userPlaneSecConfig << " ";
  if (m_hasIpAddressConfig)
    {
      os << "ipAddressConfig: " << +m_ipAddressConfig;
    }
  if (m_hasLinkLocalIpv6Address)
    {
      os << " linkLocalIpv6Address: ";
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          os << +(*it) << " ";
        }
    }
}

uint32_t
ProseDirectLinkEstablishmentAccept::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = size
    + sizeof (m_msgId)
    + sizeof (m_seqNum)
    + 1  // Source user info IEs
    + 1  // Source user info contents length
    + sizeof (m_srcUserInfo)  // Source user info contents
    + 1   // PC5 QoS flow descriptions IEI
    + 2   // PC5 QoS flow descriptions contents length
    + m_qosFlowDescriptions.size () * sizeof (uint8_t)//PC5 QoS flow descriptions contents
    + 1   // Configuration of UE PC5 unicast user plane security protection IEI
    + sizeof (m_userPlaneSecConfig);  // Configuration of UE PC5 unicast user plane security protection contents

  if (m_hasIpAddressConfig)
    {
      size = size
        + 1     // IP address configuration IEI
        + sizeof (m_ipAddressConfig);   //IP address configuration contents
    }
  if (m_hasLinkLocalIpv6Address)
    {
      size = size
        +  1   // link-local IPv6 address IEI
        + LinkLocalIpv6AddressSize;   // link-local IPv6 address contents
    }
  return size;
}

void
ProseDirectLinkEstablishmentAccept::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteU8 (m_seqNum);

  i.WriteU8 (0); // Application layer ID IEI octet (Source user info) //TODO: Not defined in the standard ATM
  i.WriteU8 (sizeof (m_srcUserInfo)); // Length of Application layer ID content octet (Source user info)
  i.WriteU32 (m_srcUserInfo); // Application layer ID contents (Source user info)

  i.WriteU8 (0); // PC5 QoS flow descriptions IEI //TODO: Not defined in the standard ATM
  i.WriteU16 (m_qosFlowDescriptions.size () * sizeof (uint8_t));// PC5 QoS flow descriptions contents length

  //UE security capabilities contents:
  for (std::vector<uint8_t>::const_iterator it = m_qosFlowDescriptions.begin (); it != m_qosFlowDescriptions.end (); ++it)
    {
      i.WriteU8 (*it);
    }

  i.WriteU8 (0); // Configuration of UE PC5 unicast user plane security protection IEI //TODO: Not defined in the standard ATM
  i.WriteU8 (m_userPlaneSecConfig); // Configuration of UE PC5 unicast user plane security protectio contents

  if (m_hasIpAddressConfig)
    {
      i.WriteU8 (57);
      i.WriteU8 (m_ipAddressConfig);
    }
  if (m_hasLinkLocalIpv6Address)
    {
      i.WriteU8 (58);
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
ProseDirectLinkEstablishmentAccept::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;

  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadU8 ();

  i.ReadU8 (); // Application layer ID IEI  (Source user info)
  i.ReadU8 (); // Application layer ID length (Source user info)
  m_srcUserInfo = i.ReadU32 (); // Application layer ID contents (Source user info)

  i.ReadU8 (); // PC5 QoS flow descriptions IEI
  uint16_t size_in_bytes = i.ReadU16 (); // PC5 QoS flow descriptions length
  // PC5 QoS flow descriptions contents:
  for (uint8_t index = 0; index < size_in_bytes; index++)
    {
      m_qosFlowDescriptions[index] = i.ReadU8 ();
    }

  i.ReadU8 (); // Configuration of UE PC5 unicast user plane security protection IEI
  m_userPlaneSecConfig = i.ReadU8 (); // Configuration of UE PC5 unicast user plane security protection contents

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
          case 57: // IP address configuration IEI
            m_ipAddressConfig = i.ReadU8 ();
            m_hasIpAddressConfig = true;
            break;
          case 58: // link-local IPv6 address IEI
            for (uint8_t index = 0; index < LinkLocalIpv6AddressSize; index++)
              {
                m_linkLocalIpv6Address[index] = i.ReadU8 ();
              }
            m_hasLinkLocalIpv6Address = true;
            break;
          default:
            break;
        }
    }

  return GetSerializedSize ();
}

void
ProseDirectLinkEstablishmentAccept::SetParameters (
  uint8_t seq,
  uint32_t srcUserInfo,
  std::vector<uint8_t> qosFlowDescriptions,
  uint8_t userPlaneSecConfig,
  uint8_t ipAddressConfig,
  std::vector<uint8_t> linkLocalIpv6Address)
{
  SetSequenceNumber (seq);
  SetSourceUserInfo (srcUserInfo);
  SetPc5QoSFlowDescriptions (qosFlowDescriptions);
  SetUserPlaneSecurityProtectionConfiguration (userPlaneSecConfig);
  SetIpAddressConfig (ipAddressConfig);
  SetLinkLocalIpv6Address (linkLocalIpv6Address);
}

uint8_t
ProseDirectLinkEstablishmentAccept::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
ProseDirectLinkEstablishmentAccept::GetSequenceNumber ()
{
  return m_seqNum;
}

void
ProseDirectLinkEstablishmentAccept::SetSequenceNumber (uint8_t seq)
{
  m_seqNum = seq;
}

void
ProseDirectLinkEstablishmentAccept::SetSourceUserInfo (uint32_t srcUserInfo)
{
  m_srcUserInfo = srcUserInfo;
}

uint32_t
ProseDirectLinkEstablishmentAccept::GetSourceUserInfo ()
{
  return m_srcUserInfo;
}

void
ProseDirectLinkEstablishmentAccept::SetPc5QoSFlowDescriptions (std::vector<uint8_t> qosFlowDescriptions)
{
  m_qosFlowDescriptions.clear ();
  m_qosFlowDescriptions.assign (qosFlowDescriptions.size (), 0);
  for (uint16_t i = 0; i < qosFlowDescriptions.size (); i++)
    {
      m_qosFlowDescriptions[i] = qosFlowDescriptions[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentAccept::GetPc5QoSFlowDescriptions ()
{
  return m_qosFlowDescriptions;
}

void
ProseDirectLinkEstablishmentAccept::SetUserPlaneSecurityProtectionConfiguration (uint8_t userPlaneSecConfig)
{
  m_userPlaneSecConfig = userPlaneSecConfig;
}

uint16_t
ProseDirectLinkEstablishmentAccept::GetUserPlaneSecurityProtectionConfiguration ()
{
  return m_userPlaneSecConfig;
}

void
ProseDirectLinkEstablishmentAccept::SetIpAddressConfig (uint8_t ipAddressConfig)
{
  m_hasIpAddressConfig = true;
  m_ipAddressConfig = ipAddressConfig;
}

uint8_t
ProseDirectLinkEstablishmentAccept::GetIpAddressConfig ()
{
  return m_ipAddressConfig;
}

void
ProseDirectLinkEstablishmentAccept::SetLinkLocalIpv6Address (std::vector<uint8_t> linkLocalIpv6Address)
{
  m_hasLinkLocalIpv6Address = true;
  m_linkLocalIpv6Address.clear ();
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
  for (uint16_t i = 0; i < linkLocalIpv6Address.size (); i++)
    {
      m_linkLocalIpv6Address[i] = linkLocalIpv6Address[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentAccept::GetLinkLocalIpv6Address ()
{
  return m_linkLocalIpv6Address;
}


/*****     ProseDirectLinkEstablishmentReject Message       *****/

ProseDirectLinkEstablishmentReject::ProseDirectLinkEstablishmentReject ()
{
  m_msgId = 3;
  m_seqNum = 0;
  m_pc5SignallingProtocolCause = 0;
}

ProseDirectLinkEstablishmentReject::~ProseDirectLinkEstablishmentReject ()
{}

TypeId
ProseDirectLinkEstablishmentReject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProseDirectLinkEstablishmentReject")
    .SetParent<Header> ()
    .AddConstructor<ProseDirectLinkEstablishmentReject> ();
  return tid;
}

TypeId
ProseDirectLinkEstablishmentReject::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ProseDirectLinkEstablishmentReject::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << +m_seqNum << " "
     << "pc5SignallingCauseValue: " << +m_pc5SignallingProtocolCause;
}

uint32_t
ProseDirectLinkEstablishmentReject::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_pc5SignallingProtocolCause);
}

void
ProseDirectLinkEstablishmentReject::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteU8 (m_seqNum);
  i.WriteU8 (m_pc5SignallingProtocolCause);
}

uint32_t
ProseDirectLinkEstablishmentReject::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadU8 ();
  m_pc5SignallingProtocolCause = i.ReadU8 ();
  return GetSerializedSize ();
}

uint8_t
ProseDirectLinkEstablishmentReject::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
ProseDirectLinkEstablishmentReject::GetSequenceNumber ()
{
  return m_seqNum;
}

void
ProseDirectLinkEstablishmentReject::SetSequenceNumber (uint8_t seq)
{
  m_seqNum = seq;
}

void
ProseDirectLinkEstablishmentReject::SetPc5SignallingProtocolCause (uint8_t pc5SignallingProtocolCause)
{
  m_pc5SignallingProtocolCause = pc5SignallingProtocolCause;
}

uint8_t
ProseDirectLinkEstablishmentReject::GetPc5SignallingProtocolCause ()
{
  return m_pc5SignallingProtocolCause;
}

} // namespace ns3

