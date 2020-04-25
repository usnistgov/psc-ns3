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
#include "lte-sl-pc5-signalling-header.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlPc5SignallingHeader");
NS_OBJECT_ENSURE_REGISTERED (LteSlPc5SignallingMessageType);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationRequest);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationAccept);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationReject);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationKeepalive);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationKeepaliveAck);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationRelease);
NS_OBJECT_ENSURE_REGISTERED (DirectCommunicationReleaseAccept);
NS_OBJECT_ENSURE_REGISTERED (DirectSecurityModeCommand);
NS_OBJECT_ENSURE_REGISTERED (DirectSecurityModeComplete);
NS_OBJECT_ENSURE_REGISTERED (DirectSecurityModeReject);
NS_OBJECT_ENSURE_REGISTERED (DirectRekeyingRequest);
NS_OBJECT_ENSURE_REGISTERED (DirectRekeyingResponse);
NS_OBJECT_ENSURE_REGISTERED (DirectRekeyingTrigger);
NS_OBJECT_ENSURE_REGISTERED (RemoteUeInfoRequest);
NS_OBJECT_ENSURE_REGISTERED (RemoteUeInfoResponse);


// Initialize static member
uint64_t LtePc5SignallingHeaderSequenceNumber::s_seqNum = 0;

/** Size of the Nonce field */
static const uint8_t NonceSize = 16;

/** Size of the signature field */
static const uint8_t SignatureSize = 129;

/** Size of the link local IPv6 address field */
static const uint8_t LinkLocalIpv6AddressSize = 16;

/** Size of the KD Freshness field */
static const uint8_t KdFreshnessSize = 16;

/** Size of the rand field */
static const uint8_t RandSize = 16;

/** Size of the Auts field */
static const uint8_t AutsSize = 14;

/** Size of the PRUK ID */
static const uint8_t PrukIdSize = 8;

// Size of other fields (for information only)
//static const uint8_t RelayServiceCodeSize = 3;
//static const uint8_t UserInfoMinimumSize = 3;
//static const uint8_t ImeiSize = 8;
//static const uint8_t ImeisvSize = 9;

/** Map message type value to acronym */
static const std::vector<std::string> s_acronyms
{
  "None", "DCRq", "DCA", "DCRj", "DCK",      // 0-4
  "DCKA", "DCR", "DCRA", "None", "None",     // 5-9
  "None", "None", "DSMCm", "DSMCp", "DSMRj", // 10-14
  "DRRq", "DRRs", "DRT", "RUIRq", "RUIRs"    // 15-19
};

/** Map message type value to string name */
static const std::vector<std::string> s_names
{
  "None",
  "DIRECT_COMMUNICATION_REQUEST",
  "DIRECT_COMMUNICATION_ACCEPT",
  "DIRECT_COMMUNICATION_REJECT",
  "DIRECT_COMMUNICATION_KEEPALIVE",
  "DIRECT_COMMUNICATION_KEEPALIVE_ACK",
  "DIRECT_COMMUNICATION_RELEASE",
  "DIRECT_COMMUNICATION_RELEASE_ACCEPT",
  "None",
  "None",
  "None",
  "None",
  "DIRECT_SECURITY_MODE_COMMAND",
  "DIRECT_SECURITY_MODE_COMPLETE",
  "DIRECT_SECURITY_MODE_REJECT",
  "DIRECT_REKEYING_REQUEST",
  "DIRECT_REKEYING_RESPONSE",
  "DIRECT_REKEYING_TRIGGER",
  "REMOTE_UE_INFO_REQUEST",
  "REMOTE_UE_INFO_RESPONSE",
};

/*****     LteSlPc5SignallingMessageType : To peek at the first byte of the received message and identify the type       *****/

LteSlPc5SignallingMessageType::LteSlPc5SignallingMessageType ()
{
  m_msgType = 0;
}

LteSlPc5SignallingMessageType::~LteSlPc5SignallingMessageType ()
{

}

TypeId
LteSlPc5SignallingMessageType::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlPc5SignallingMessageType")
    .SetParent<Header> ()
    .AddConstructor<LteSlPc5SignallingMessageType> ()
  ;
  return tid;
}

TypeId
LteSlPc5SignallingMessageType::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LteSlPc5SignallingMessageType::Print (std::ostream &os) const
{
  os << "msgName: " << GetMessageName () << " ";
}

uint32_t
LteSlPc5SignallingMessageType::GetSerializedSize (void) const
{
  return sizeof (m_msgType);
}

void
LteSlPc5SignallingMessageType::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgType);
}

uint32_t
LteSlPc5SignallingMessageType::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgType = i.ReadU8 ();

  return GetSerializedSize ();
}

uint8_t
LteSlPc5SignallingMessageType::GetMessageType ()
{
  return m_msgType;
}

std::string
LteSlPc5SignallingMessageType::GetMessageAcronym (void) const
{
  return s_acronyms[m_msgType];
}

std::string
LteSlPc5SignallingMessageType::GetMessageName (void) const
{
  return s_names[m_msgType];
}

void
LteSlPc5SignallingMessageType::SetMessageType (uint8_t msgType)
{
  switch (msgType)
    {
    case LteSlPc5SignallingMessageType::DirectCommunicationRequest:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationAccept:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationReject:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationKeepalive:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationKeepaliveAck:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationRelease:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectCommunicationReleaseAccept:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectSecurityModeCommand:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectSecurityModeComplete:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectSecurityModeReject:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectRekeyingRequest:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectRekeyingResponse:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::DirectRekeyingTrigger:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::RemoteUeInfoRequest:
      m_msgType = msgType;
      break;
    case LteSlPc5SignallingMessageType::RemoteUeInfoResponse:
      m_msgType = msgType;
      break;
    default:
      NS_ABORT_MSG ("Cannot set the Message Type to the input provided");
      break;
    }
}

/*****     DirectCommunicationRequest Message       *****/

DirectCommunicationRequest::DirectCommunicationRequest ()
{
  m_msgId = 1;

  m_seqNum = 0; //m_seqNum = seq;

  m_userInfo = 0;
  m_userInfoType = 0;
  m_ipAddressConfig = 0;
  m_maxInactivityPeriod = 0;
  m_nonce1.clear ();
  m_nonce1.assign (NonceSize,0);
  m_ueSecurityCapabilities = 0;
  m_msb = 0;
  m_hasKdId = false;
  m_kdId = 0;
  m_hasRelayServiceCode = false;
  m_relayServiceCode = 0;
  m_hasSignature = false;
  m_signature.clear ();
  m_signature.assign (SignatureSize,0);
  m_hasLinkLocalIpv6Address = false;
  m_linkLocalIpv6Address.clear ();
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
}

DirectCommunicationRequest::~DirectCommunicationRequest ()
{

}

TypeId
DirectCommunicationRequest::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationRequest")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationRequest> ()
  ;
  return tid;
}

TypeId
DirectCommunicationRequest::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationRequest::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " userInfoType: " << (uint16_t) m_userInfoType << " userInfo: " << m_userInfo << " ";
  os << "ipAddressConfig: " << (uint16_t) m_ipAddressConfig << " "
     << "maxInactivityPeriod: " << m_maxInactivityPeriod << " nonce1: ";

  for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
    {
      os << (uint16_t) *it << " ";
    }

  os << "ueSecurityCapabilities: " << m_ueSecurityCapabilities << " "
     << "msb: " << (uint16_t) m_msb << " ";

  if (m_hasKdId)
    {
      os << "kdId: " << m_kdId << " ";
    }

  if (m_hasRelayServiceCode)
    {
      os << "relayServiceCode: " << m_relayServiceCode << " ";
    }

  if (m_hasSignature)
    {
      os << "signature: ";
      for (std::vector<uint8_t>::const_iterator it = m_signature.begin (); it != m_signature.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }

  if (m_hasLinkLocalIpv6Address)
    {
      os << "linkLocalIpv6Address: ";
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }
}

uint32_t
DirectCommunicationRequest::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = size
    + sizeof (m_msgId)
    + sizeof (m_seqNum)
    + 1
    + sizeof (m_userInfoType)
    + sizeof (m_userInfo)
    + sizeof (m_ipAddressConfig)
    + sizeof (m_maxInactivityPeriod)
    + NonceSize
    + sizeof (m_ueSecurityCapabilities)
    + sizeof (m_msb); //1 for the 8 bits of size of userInfo
  if  (m_hasKdId)
    {
      size += (1 + sizeof (m_kdId)); //1 for the type
    }
  if (m_hasRelayServiceCode)
    {
      size += (1 + 3); //1 for the type and 3 for the 24 bits of m_relayServiceCode
    }
  if (m_hasSignature)
    {
      size += (1 + SignatureSize); //1 for the type
    }
  if (m_hasLinkLocalIpv6Address)
    {
      size += (1 + LinkLocalIpv6AddressSize); //1 for the type
    }
  return size;
}

void
DirectCommunicationRequest::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (sizeof(m_userInfo));
  i.WriteU8 (m_userInfoType);
  i.WriteU64 (m_userInfo);
  i.WriteU8 (m_ipAddressConfig);
  i.WriteHtonU32 (m_maxInactivityPeriod);
  for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
    {
      i.WriteU8 (*it);
    }
  i.WriteHtonU16 (m_ueSecurityCapabilities);
  i.WriteU8 (m_msb);
  if (m_hasKdId)
    {
      i.WriteU8 (17);
      i.WriteHtonU32 (m_kdId);
    }
  if (m_hasRelayServiceCode)
    {
      i.WriteU8 (25);
      i.WriteU8 ((m_relayServiceCode >> 0) & 0xff);
      i.WriteU8 ((m_relayServiceCode >> 8) & 0xff);
      i.WriteU8 ((m_relayServiceCode >> 16) & 0xff);
    }
  if (m_hasSignature)
    {
      i.WriteU8 (22);
      for (std::vector<uint8_t>::const_iterator it = m_signature.begin (); it != m_signature.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasLinkLocalIpv6Address)
    {
      i.WriteU8 (3);
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
DirectCommunicationRequest::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  i.ReadU8 (); //userInfoLength
  m_userInfoType = i.ReadU8 ();
  m_userInfo = i.ReadU64 ();
  m_ipAddressConfig =  i.ReadU8 ();
  m_maxInactivityPeriod = i.ReadNtohU32 ();

  for (uint8_t index = 0; index < NonceSize; index++)
    {
      m_nonce1[index] = i.ReadU8 ();
    }

  m_ueSecurityCapabilities = i.ReadNtohU16 ();
  m_msb = i.ReadU8 ();

  uint8_t opt_vars_iei = 0;
  uint8_t byte0, byte1, byte2;
  uint32_t relayServiceCode_dummy;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 17:
          m_hasKdId = true;
          m_kdId = i.ReadNtohU32 ();
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
        case 22:
          for (uint8_t index = 0; index < SignatureSize; index++)
            {
              m_signature[index] = i.ReadU8 ();
            }
          m_hasSignature = true;
          break;
        case 3:
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
DirectCommunicationRequest::SetParameters (
  uint8_t userInfoType,
  uint64_t userInfo,
  uint8_t ipAddressConfig,
  uint32_t maxInactivityPeriod,
  std::vector<uint8_t> nonce1,
  uint16_t ueSecurityCapabilities,
  uint8_t msb,
  int32_t kdId,
  uint32_t relayServiceCode,
  std::vector<uint8_t> signature,
  std::vector<uint8_t> linkLocalIpv6Address
  )
{
  SetUserInfo (userInfoType, userInfo);
  SetIpAddressConfig (ipAddressConfig);
  SetMaximumInactivityPeriod (maxInactivityPeriod);
  SetNonce1 (nonce1);
  SetUeSecurityCapabilities (ueSecurityCapabilities);
  SetMsbKdId (msb);
  SetKdId (kdId);
  SetRelayServiceCode (relayServiceCode);
  SetSignature (signature);
  SetLinkLocalIpv6Address (linkLocalIpv6Address);
}

uint8_t
DirectCommunicationRequest::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationRequest::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationRequest::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
DirectCommunicationRequest::SetUserInfo (uint8_t userInfoType, uint64_t userInfo)
{
  NS_ASSERT_MSG ( userInfoType == 3, "Only User Info Type = 3 (IMSI) supported"); //Refer 24.334 Section 12.5.1.3
  m_userInfoType = userInfoType;
  m_userInfo = userInfo;
}

uint64_t
DirectCommunicationRequest::GetUserInfo ()
{
  return m_userInfo;
}

uint8_t
DirectCommunicationRequest::GetUserInfoType ()
{
  return m_userInfoType;
}

void
DirectCommunicationRequest::SetImsi (uint64_t imsi)
{
  SetUserInfo (3, imsi);
}

uint64_t
DirectCommunicationRequest::GetImsi ()
{
  return m_userInfo;
}

void
DirectCommunicationRequest::SetIpAddressConfig (uint8_t ipAddressConfig)
{
  m_ipAddressConfig = ipAddressConfig;
}

uint8_t
DirectCommunicationRequest::GetIpAddressConfig ()
{
  return m_ipAddressConfig;
}

void
DirectCommunicationRequest::SetMaximumInactivityPeriod (uint32_t maxInactivityPeriod)
{
  m_maxInactivityPeriod = maxInactivityPeriod;
}

uint32_t
DirectCommunicationRequest::GetMaximumInactivityPeriod ()
{
  return m_maxInactivityPeriod;
}

void
DirectCommunicationRequest::SetNonce1 (std::vector<uint8_t> nonce1)
{
  m_nonce1.assign (NonceSize,0);
  for (uint16_t i = 0; i < nonce1.size (); i++)
    {
      m_nonce1[i] = nonce1[i];
    }
}

std::vector<uint8_t>
DirectCommunicationRequest::GetNonce1 ()
{
  return m_nonce1;
}

void
DirectCommunicationRequest::SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities)
{
  m_ueSecurityCapabilities = ueSecurityCapabilities;
}

uint16_t
DirectCommunicationRequest::GetUeSecurityCapabilities ()
{
  return m_ueSecurityCapabilities;
}

void
DirectCommunicationRequest::SetMsbKdId (uint8_t msb)
{
  m_msb = msb;
}

uint8_t
DirectCommunicationRequest::GetMsbKdId ()
{
  return m_msb;
}

void
DirectCommunicationRequest::SetKdId (uint32_t kdId)
{
  m_hasKdId = true;
  m_kdId = kdId;
}

uint32_t
DirectCommunicationRequest::GetKdId ()
{
  return m_kdId;
}

void
DirectCommunicationRequest::SetRelayServiceCode (uint32_t relayServiceCode)
{
  m_hasRelayServiceCode = true;
  m_relayServiceCode = relayServiceCode;
}

uint32_t
DirectCommunicationRequest::GetRelayServiceCode ()
{
  return m_relayServiceCode;
}

void
DirectCommunicationRequest::SetSignature (std::vector<uint8_t> signature)
{
  m_hasSignature = true;
  m_signature.assign (SignatureSize,0);
  for (uint16_t i = 0; i < signature.size (); i++)
    {
      m_signature[i] = signature[i];
    }
}

std::vector<uint8_t>
DirectCommunicationRequest::GetSignature ()
{
  return m_signature;
}

void
DirectCommunicationRequest::SetLinkLocalIpv6Address (std::vector<uint8_t> linkLocalIpv6Address)
{
  m_hasLinkLocalIpv6Address = true;
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
  for (uint16_t i = 0; i < linkLocalIpv6Address.size (); i++)
    {
      m_linkLocalIpv6Address[i] = linkLocalIpv6Address[i];
    }
}

std::vector<uint8_t>
DirectCommunicationRequest::GetLinkLocalIpv6Address ()
{
  return m_linkLocalIpv6Address;
}

/*****     DirectCommunicationAccept Message       *****/

DirectCommunicationAccept::DirectCommunicationAccept ()
{
  m_msgId = 2;

  m_seqNum = 0; //m_seqNum = seq;

  m_ipAddressConfig = 0;
  m_hasLinkLocalIpv6Address = false;
  m_linkLocalIpv6Address.clear ();
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
}

DirectCommunicationAccept::~DirectCommunicationAccept ()
{

}

TypeId
DirectCommunicationAccept::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationAccept")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationAccept> ()
  ;
  return tid;
}

TypeId
DirectCommunicationAccept::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationAccept::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "ipAddressConfig: " << (uint16_t) m_ipAddressConfig;

  if (m_hasLinkLocalIpv6Address)
    {
      os << " linkLocalIpv6Address: ";
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }
}

uint32_t
DirectCommunicationAccept::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = sizeof (m_msgId) + sizeof (m_seqNum) + sizeof (m_ipAddressConfig);
  if (m_hasLinkLocalIpv6Address)
    {
      size += (1 + LinkLocalIpv6AddressSize); //1 for the type
    }
  return size;
}

void
DirectCommunicationAccept::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_ipAddressConfig);
  if (m_hasLinkLocalIpv6Address)
    {
      i.WriteU8 (3);
      for (std::vector<uint8_t>::const_iterator it = m_linkLocalIpv6Address.begin (); it != m_linkLocalIpv6Address.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
DirectCommunicationAccept::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_ipAddressConfig = i.ReadU8 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 3:
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
DirectCommunicationAccept::SetParameters (uint8_t ipAddressConfig, std::vector<uint8_t> linkLocalIpv6Address)
{
  SetIpAddressConfig (ipAddressConfig);
  SetLinkLocalIpv6Address (linkLocalIpv6Address);
}

uint8_t
DirectCommunicationAccept::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationAccept::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationAccept::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

void
DirectCommunicationAccept::SetIpAddressConfig (uint8_t ipAddressConfig)
{
  m_ipAddressConfig = ipAddressConfig;
}

uint8_t
DirectCommunicationAccept::GetIpAddressConfig ()
{
  return m_ipAddressConfig;
}

void
DirectCommunicationAccept::SetLinkLocalIpv6Address (std::vector<uint8_t> linkLocalIpv6Address)
{
  m_hasLinkLocalIpv6Address = true;
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
  for (uint16_t i = 0; i < linkLocalIpv6Address.size (); i++)
    {
      m_linkLocalIpv6Address[i] = linkLocalIpv6Address[i];
    }
}

std::vector<uint8_t>
DirectCommunicationAccept::GetLinkLocalIpv6Address ()
{
  return m_linkLocalIpv6Address;
}


/*****     DirectCommunicationReject Message       *****/

DirectCommunicationReject::DirectCommunicationReject ()
{
  m_msgId = 3;

  m_seqNum = 0; //m_seqNum = seq;

  m_pc5SignallingCauseValue = 0;
}

DirectCommunicationReject::~DirectCommunicationReject ()
{

}

TypeId
DirectCommunicationReject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationReject")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationReject> ()
  ;
  return tid;
}

TypeId
DirectCommunicationReject::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationReject::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "pc5SignallingCauseValue: " << (uint16_t) m_pc5SignallingCauseValue;
}

uint32_t
DirectCommunicationReject::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_pc5SignallingCauseValue)
  ;
}

void
DirectCommunicationReject::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_pc5SignallingCauseValue);
}

uint32_t
DirectCommunicationReject::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_pc5SignallingCauseValue = i.ReadU8 ();
  return GetSerializedSize ();
}

uint8_t
DirectCommunicationReject::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationReject::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationReject::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
DirectCommunicationReject::SetPc5SignallingCauseValue (uint8_t pc5SignallingCauseValue)
{
  m_pc5SignallingCauseValue = pc5SignallingCauseValue;
}

uint8_t
DirectCommunicationReject::GetPc5SignallingCauseValue ()
{
  return m_pc5SignallingCauseValue;
}


/*****     DirectCommunicationKeepalive Message       *****/

DirectCommunicationKeepalive::DirectCommunicationKeepalive ()
{
  m_msgId = 4;

  m_seqNum = 0; //m_seqNum = seq;

  m_keepaliveCounter = 0;
  m_hasMaxInactivityPeriod = false;
  m_maxInactivityPeriod = 0;
}

DirectCommunicationKeepalive::~DirectCommunicationKeepalive ()
{

}

TypeId
DirectCommunicationKeepalive::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationKeepalive")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationKeepalive> ()
  ;
  return tid;
}

TypeId
DirectCommunicationKeepalive::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationKeepalive::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "keepaliveCounter: " << m_keepaliveCounter << " ";

  if (m_hasMaxInactivityPeriod)
    {
      os << "maxInactivityPeriod: " << m_maxInactivityPeriod;
    }
}

uint32_t
DirectCommunicationKeepalive::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_keepaliveCounter);
  if (m_hasMaxInactivityPeriod)
    {
      size += (1 + sizeof (m_maxInactivityPeriod)); //1 for the type
    }
  return size;
}

void
DirectCommunicationKeepalive::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteHtonU32 (m_keepaliveCounter);
  if (m_hasMaxInactivityPeriod)
    {
      i.WriteU8 (7);
      i.WriteHtonU32 (m_maxInactivityPeriod);
    }
}

uint32_t
DirectCommunicationKeepalive::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_keepaliveCounter = i.ReadNtohU32 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 7:
          m_maxInactivityPeriod = i.ReadNtohU32 ();
          m_hasMaxInactivityPeriod = true;
          break;
        default:
          break;
        }
    }

  return GetSerializedSize ();
}

void
DirectCommunicationKeepalive::SetParameters (uint32_t keepaliveCounter, uint32_t maxInactivityPeriod)
{
  SetKeepaliveCounter (keepaliveCounter);
  SetMaximumInactivityPeriod (maxInactivityPeriod);
}

uint8_t
DirectCommunicationKeepalive::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationKeepalive::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationKeepalive::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
DirectCommunicationKeepalive::SetKeepaliveCounter (uint32_t keepaliveCounter)
{
  m_keepaliveCounter = keepaliveCounter;
}

uint32_t
DirectCommunicationKeepalive::GetKeepaliveCounter ()
{
  return m_keepaliveCounter;
}

void
DirectCommunicationKeepalive::SetMaximumInactivityPeriod (uint32_t maxInactivityPeriod)
{
  m_hasMaxInactivityPeriod = true;
  m_maxInactivityPeriod = maxInactivityPeriod;
}

uint32_t
DirectCommunicationKeepalive::GetMaximumInactivityPeriod ()
{
  return m_maxInactivityPeriod;
}


/*****     DirectCommunicationKeepaliveAck Message       *****/

DirectCommunicationKeepaliveAck::DirectCommunicationKeepaliveAck ()
{
  m_msgId = 5;

  m_seqNum = 0; //m_seqNum = seq;

  m_keepaliveCounter = 0;
}

DirectCommunicationKeepaliveAck::~DirectCommunicationKeepaliveAck ()
{

}

TypeId
DirectCommunicationKeepaliveAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationKeepaliveAck")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationKeepaliveAck> ()
  ;
  return tid;
}

TypeId
DirectCommunicationKeepaliveAck::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationKeepaliveAck::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "keepaliveCounter: " << m_keepaliveCounter << " ";
}

uint32_t
DirectCommunicationKeepaliveAck::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_keepaliveCounter)
  ;
}

void
DirectCommunicationKeepaliveAck::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteHtonU32 (m_keepaliveCounter);
}

uint32_t
DirectCommunicationKeepaliveAck::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_keepaliveCounter = i.ReadNtohU32 ();
  return GetSerializedSize ();
}

uint8_t
DirectCommunicationKeepaliveAck::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationKeepaliveAck::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationKeepaliveAck::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
DirectCommunicationKeepaliveAck::SetKeepaliveCounter (uint32_t keepaliveCounter)
{
  m_keepaliveCounter = keepaliveCounter;
}

uint32_t
DirectCommunicationKeepaliveAck::GetKeepaliveCounter ()
{
  return m_keepaliveCounter;
}



/*****     DirectCommunicationRelease Message       *****/

DirectCommunicationRelease::DirectCommunicationRelease ()
{
  m_msgId = 6;

  m_seqNum = 0; //m_seqNum = seq;

  m_releaseReason = 0;
}

DirectCommunicationRelease::~DirectCommunicationRelease ()
{

}

TypeId
DirectCommunicationRelease::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationRelease")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationRelease> ()
  ;
  return tid;
}

TypeId
DirectCommunicationRelease::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationRelease::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "ReleaseReason: " << (uint16_t) m_releaseReason << " "
  ;
}

uint32_t
DirectCommunicationRelease::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_releaseReason)
  ;
}

void
DirectCommunicationRelease::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_releaseReason);
}

uint32_t
DirectCommunicationRelease::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_releaseReason = i.ReadU8 ();
  return GetSerializedSize ();
}

uint8_t
DirectCommunicationRelease::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationRelease::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationRelease::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

void
DirectCommunicationRelease::SetReleaseReason (uint8_t releaseReason)
{
  m_releaseReason = releaseReason;
}

uint8_t
DirectCommunicationRelease::GetReleaseReason ()
{
  return m_releaseReason;
}


/*****     DirectCommunicationReleaseAccept Message       *****/

DirectCommunicationReleaseAccept::DirectCommunicationReleaseAccept ()
{
  m_msgId = 7;

  m_seqNum = 0; //m_seqNum = seq;

}

DirectCommunicationReleaseAccept::~DirectCommunicationReleaseAccept ()
{

}

TypeId
DirectCommunicationReleaseAccept::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectCommunicationReleaseAccept")
    .SetParent<Header> ()
    .AddConstructor<DirectCommunicationReleaseAccept> ()
  ;
  return tid;
}

TypeId
DirectCommunicationReleaseAccept::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectCommunicationReleaseAccept::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
  ;
}

uint32_t
DirectCommunicationReleaseAccept::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum);
}

void
DirectCommunicationReleaseAccept::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
}

uint32_t
DirectCommunicationReleaseAccept::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

uint8_t
DirectCommunicationReleaseAccept::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectCommunicationReleaseAccept::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectCommunicationReleaseAccept::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}



/*****     DirectSecurityModeCommand Message       *****/

DirectSecurityModeCommand::DirectSecurityModeCommand ()
{
  m_msgId = 12;

  m_seqNum = 0; //m_seqNum = seq;

  m_ueSecurityCapabilities = 0;
  m_nonce2.clear ();
  m_nonce2.assign (NonceSize,0);
  m_chosenAlgorithmsIntegrity = 0;
  m_chosenAlgorithmsCyphering = 0;
  m_lsb = 0;
  m_hasMsb = false;
  m_msb = 0;
  m_hasKdFreshness = false;
  m_kdFreshness.clear ();
  m_kdFreshness.assign (KdFreshnessSize,0);
  m_hasGpi = false;
  m_gpi.clear ();
  m_hasUserInfo = false;
  m_userInfo = 0;
  m_userInfoType = 0;
  m_hasSignature = false;
  m_signature.clear ();
  m_signature.assign (SignatureSize,0);
  m_hasEncryptedPayload = false;
  m_encryptedPayload.clear ();
}

DirectSecurityModeCommand::~DirectSecurityModeCommand ()
{

}

TypeId
DirectSecurityModeCommand::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectSecurityModeCommand")
    .SetParent<Header> ()
    .AddConstructor<DirectSecurityModeCommand> ()
  ;
  return tid;
}

TypeId
DirectSecurityModeCommand::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectSecurityModeCommand::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "ueSecurityCapabilities: " << m_ueSecurityCapabilities << " nonce2: ";

  for (std::vector<uint8_t>::const_iterator it = m_nonce2.begin (); it != m_nonce2.end (); ++it)
    {
      os << (uint16_t) *it << " ";
    }

  os << "chosenAlgorithmsIntegrity: " << (uint16_t) m_chosenAlgorithmsIntegrity << " chosenAlgorithmsCyphering: " << (uint16_t) m_chosenAlgorithmsCyphering << " lsb: " << (uint16_t) m_lsb << " ";

  if (m_hasMsb)
    {
      os << "msb: " << m_msb << " ";
    }

  if (m_hasKdFreshness)
    {
      os << "kdFreshness: ";
      for (std::vector<uint8_t>::const_iterator it = m_kdFreshness.begin (); it != m_kdFreshness.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }

  if (m_hasGpi)
    {
      os << "gpi: ";
      for (std::vector<uint8_t>::const_iterator it =  m_gpi.begin (); it !=  m_gpi.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }

  if (m_hasUserInfo)
    {
      os << "userInfoType: " << (uint16_t) m_userInfoType << " userInfo: " << m_userInfo << " ";
    }

  if (m_hasSignature)
    {
      os << "signature: ";
      for (std::vector<uint8_t>::const_iterator it = m_signature.begin (); it != m_signature.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }

  if (m_hasEncryptedPayload)
    {
      os << "encryptedPayload: ";
      for (std::vector<uint8_t>::const_iterator it = m_encryptedPayload.begin (); it != m_encryptedPayload.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }
}

uint32_t
DirectSecurityModeCommand::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_ueSecurityCapabilities) +
    NonceSize +
    sizeof (m_chosenAlgorithmsIntegrity) +
    sizeof (m_lsb);

  if (m_hasMsb)
    {
      size += (1 + sizeof (m_msb)); //1 for the type
    }
  if (m_hasKdFreshness)
    {
      size += (1 + KdFreshnessSize); //1 for the type
    }
  if (m_hasGpi)
    {
      size += (2 + m_gpi.size ()); //2 for the type and value
    }
  if (m_hasUserInfo)
    {
      size += (3 + sizeof (m_userInfo)); //3 for the type, size and value
    }
  if (m_hasSignature)
    {
      size += (1 + SignatureSize); //1 for the type
    }
  if (m_hasEncryptedPayload)
    {
      size += (2 + m_encryptedPayload.size ()); //2 for the type and value
    }
  return size;
}

void
DirectSecurityModeCommand::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteHtonU16 (m_ueSecurityCapabilities);

  for (std::vector<uint8_t>::const_iterator it = m_nonce2.begin (); it != m_nonce2.end (); ++it)
    {
      i.WriteU8 (*it);
    }

  uint8_t spare = 0;
  uint8_t chosenAlgorithms = (spare << 7) | (m_chosenAlgorithmsCyphering << 5) | (spare << 4) | (m_chosenAlgorithmsIntegrity);
  i.WriteU8 (chosenAlgorithms);

  i.WriteU8 (m_lsb);

  if (m_hasMsb)
    {
      i.WriteU8 (16);
      i.WriteHtonU16 (m_msb);
    }
  if (m_hasKdFreshness)
    {
      i.WriteU8 (18);
      for (std::vector<uint8_t>::const_iterator it = m_kdFreshness.begin (); it != m_kdFreshness.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasGpi)
    {
      i.WriteU8 (24);
      i.WriteU8 (m_gpi.size ());
      for (std::vector<uint8_t>::const_iterator it = m_gpi.begin (); it != m_gpi.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasUserInfo)
    {
      i.WriteU8 (1);
      i.WriteU8 (sizeof (m_userInfo));
      i.WriteU8 (m_userInfoType);
      i.WriteU64 (m_userInfo);
    }
  if (m_hasSignature)
    {
      i.WriteU8 (22);
      for (std::vector<uint8_t>::const_iterator it = m_signature.begin (); it != m_signature.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasEncryptedPayload)
    {
      i.WriteU8 (23);
      i.WriteU8 (m_encryptedPayload.size ());
      for (std::vector<uint8_t>::const_iterator it = m_encryptedPayload.begin (); it != m_encryptedPayload.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
DirectSecurityModeCommand::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_ueSecurityCapabilities = i.ReadNtohU16 ();

  for (uint8_t index = 0; index < NonceSize; index++)
    {
      m_nonce2[index] = i.ReadU8 ();
    }

  uint8_t chosenAlgorithms = i.ReadU8 ();
  m_chosenAlgorithmsIntegrity = chosenAlgorithms & 0xff;
  m_chosenAlgorithmsCyphering = (chosenAlgorithms >> 5) & 0xff;

  m_lsb = i.ReadU8 ();

  uint8_t opt_vars_iei = 0;
  uint8_t gpiLength, encryptedPayloadLength;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 16:
          m_msb = i.ReadNtohU16 ();
          m_hasMsb = true;
          break;
        case 18:
          for (uint8_t index = 0; index < KdFreshnessSize; index++)
            {
              m_kdFreshness[index] = i.ReadU8 ();
            }
          m_hasKdFreshness = true;
          break;
        case 24:
          gpiLength = i.ReadU8 ();
          for (uint8_t index = 0; index < gpiLength; index++)
            {
              m_gpi.push_back (i.ReadU8 ());
            }
          m_hasGpi = true;
          break;
        case 1:
          i.ReadU8 (); //userInfoLength
          m_userInfoType = i.ReadU8 ();
          m_userInfo = i.ReadU64 ();
          m_hasUserInfo = true;
          break;
        case 22:
          for (uint8_t index = 0; index < SignatureSize; index++)
            {
              m_signature[index] = i.ReadU8 ();
            }
          m_hasSignature = true;
          break;
        case 23:
          encryptedPayloadLength = i.ReadU8 ();
          for (uint8_t index = 0; index < encryptedPayloadLength; index++)
            {
              m_encryptedPayload.push_back (i.ReadU8 ());
            }
          break;
        default:
          break;
        }
    }

  return GetSerializedSize ();
}

void
DirectSecurityModeCommand::SetParameters (
  uint16_t ueSecurityCapabilities,
  std::vector<uint8_t> nonce2,
  uint8_t chosenAlgorithmsIntegrity,
  uint8_t chosenAlgorithmCyphering,
  uint8_t lsb,
  uint16_t msb,
  std::vector<uint8_t> kdFreshness,
  std::vector<uint8_t> gpi,
  uint8_t userInfoType,
  uint64_t userInfo,
  std::vector<uint8_t> signature,
  std::vector<uint8_t> encryptedPayload
  )
{
  SetUeSecurityCapabilities (ueSecurityCapabilities);
  SetNonce2 (nonce2);
  SetChosenAlgorithms (chosenAlgorithmsIntegrity, chosenAlgorithmCyphering);
  SetLsb (lsb);
  SetMsb (msb);
  SetKdFreshness (kdFreshness);
  SetGpi (gpi);
  SetUserInfo (userInfoType, userInfo);
  SetSignature (signature);
  SetEncryptedPayload (encryptedPayload);

}

uint8_t
DirectSecurityModeCommand::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectSecurityModeCommand::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectSecurityModeCommand::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

uint16_t
DirectSecurityModeCommand::GetUeSecurityCapabilities ()
{
  return m_ueSecurityCapabilities;
}

void
DirectSecurityModeCommand::SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities)
{
  m_ueSecurityCapabilities = ueSecurityCapabilities;
}

std::vector<uint8_t>
DirectSecurityModeCommand::GetNonce2 ()
{
  return m_nonce2;
}

void
DirectSecurityModeCommand::SetNonce2 ( std::vector<uint8_t> nonce2)
{
  m_nonce2.assign (NonceSize,0);
  for (uint16_t i = 0; i < nonce2.size (); i++)
    {
      m_nonce2[i] = nonce2[i];
    }
}

uint8_t
DirectSecurityModeCommand::GetChosenAlgorithmsIntegrity ()
{
  return m_chosenAlgorithmsIntegrity;
}

uint8_t
DirectSecurityModeCommand::GetChosenAlgorithmsCyphering ()
{
  return m_chosenAlgorithmsCyphering;
}


void
DirectSecurityModeCommand::SetChosenAlgorithms (uint8_t chosenAlgorithmsIntegrity, uint8_t chosenAlgorithmCyphering)
{
  m_chosenAlgorithmsIntegrity = chosenAlgorithmsIntegrity;
  m_chosenAlgorithmsCyphering = chosenAlgorithmCyphering;
}

uint8_t
DirectSecurityModeCommand::GetLsb ()
{
  return m_lsb;
}

void
DirectSecurityModeCommand::SetLsb (uint8_t lsb)
{
  m_lsb = lsb;
}

uint16_t
DirectSecurityModeCommand::GetMsb ()
{
  return m_msb;
}

void
DirectSecurityModeCommand::SetMsb (uint16_t msb)
{
  m_hasMsb = true;
  m_msb = msb;
}

std::vector<uint8_t>
DirectSecurityModeCommand::GetKdFreshness ()
{
  return m_kdFreshness;
}

void
DirectSecurityModeCommand::SetKdFreshness (std::vector<uint8_t> kdFreshness)
{
  m_hasKdFreshness = true;
  m_kdFreshness.assign (KdFreshnessSize,0);
  for (uint16_t i = 0; i < kdFreshness.size (); i++)
    {
      m_kdFreshness[i] = kdFreshness[i];
    }
}

std::vector<uint8_t>
DirectSecurityModeCommand::GetGpi ()
{
  return m_gpi;
}

void
DirectSecurityModeCommand::SetGpi (std::vector<uint8_t> gpi)
{
  m_hasGpi = true;
  m_gpi = gpi;
}

uint64_t
DirectSecurityModeCommand::GetUserInfo ()
{
  return m_userInfo;
}

uint8_t
DirectSecurityModeCommand::GetUserInfoType ()
{
  return m_userInfoType;
}

void
DirectSecurityModeCommand::SetUserInfo (uint8_t userInfoType, uint64_t userInfo)
{
  NS_ASSERT_MSG ( userInfoType == 3, "Only User Info Type = 3 (IMSI) supported"); //Refer 24.334 Section 12.5.1.3
  m_hasUserInfo = true;
  m_userInfo = userInfo;
  m_userInfoType = userInfoType;
}

std::vector<uint8_t>
DirectSecurityModeCommand::GetSignature ()
{
  return m_signature;
}

void
DirectSecurityModeCommand::SetSignature (std::vector<uint8_t> signature)
{
  m_hasSignature = true;
  m_signature.assign (SignatureSize,0);
  for (uint16_t i = 0; i < signature.size (); i++)
    {
      m_signature[i] = signature[i];
    }
}

std::vector<uint8_t>
DirectSecurityModeCommand::GetEncryptedPayload ()
{
  return m_encryptedPayload;
}

void
DirectSecurityModeCommand::SetEncryptedPayload (std::vector<uint8_t> encryptedPayload)
{
  m_hasEncryptedPayload = true;
  m_encryptedPayload = encryptedPayload;
}

/*****     DirectSecurityModeComplete Message       *****/

DirectSecurityModeComplete::DirectSecurityModeComplete ()
{
  m_msgId = 13;

  m_seqNum = 0; //m_seqNum = seq;

  m_hasLsb = false;
  m_lsb = 0;
}

DirectSecurityModeComplete::~DirectSecurityModeComplete ()
{

}

TypeId
DirectSecurityModeComplete::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectSecurityModeComplete")
    .SetParent<Header> ()
    .AddConstructor<DirectSecurityModeComplete> ()
  ;
  return tid;
}

TypeId
DirectSecurityModeComplete::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectSecurityModeComplete::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " ";

  if (m_hasLsb)
    {
      os << "lsb: " << m_lsb;
    }
}

uint32_t
DirectSecurityModeComplete::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = sizeof (m_msgId) + sizeof (m_seqNum);
  if (m_hasLsb)
    {
      size += (1 + sizeof (m_lsb)); //1 for the type
    }
  return size;
}

void
DirectSecurityModeComplete::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  if (m_hasLsb)
    {
      i.WriteU8 (15);
      i.WriteHtonU16 (m_lsb);
    }
}

uint32_t
DirectSecurityModeComplete::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 15:
          m_lsb = i.ReadNtohU16 ();
          m_hasLsb = true;
          break;
        default:
          break;
        }
    }
  return GetSerializedSize ();
}

uint8_t
DirectSecurityModeComplete::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectSecurityModeComplete::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectSecurityModeComplete::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


uint16_t
DirectSecurityModeComplete::GetLsb ()
{
  return m_lsb;
}

void
DirectSecurityModeComplete::SetLsb (uint16_t lsb)
{
  m_hasLsb = true;
  m_lsb = lsb;
}

/*****     DirectSecurityModeReject Message       *****/

DirectSecurityModeReject::DirectSecurityModeReject ()
{
  m_msgId = 14;

  m_seqNum = 0; //m_seqNum = seq;

  m_pc5SignallingCauseValue = 0;
  m_hasRand = false;
  m_rand.clear ();
  m_rand.assign (RandSize,0);
  m_hasAuts = false;
  m_auts.clear ();
  m_auts.assign (AutsSize,0);
}

DirectSecurityModeReject::~DirectSecurityModeReject ()
{

}

TypeId
DirectSecurityModeReject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectSecurityModeReject")
    .SetParent<Header> ()
    .AddConstructor<DirectSecurityModeReject> ()
  ;
  return tid;
}

TypeId
DirectSecurityModeReject::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectSecurityModeReject::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "pc5SignallingCauseValue: " << (uint16_t) m_pc5SignallingCauseValue << " ";

  if (m_hasRand)
    {
      os << "rand: ";
      for (std::vector<uint8_t>::const_iterator it = m_rand.begin (); it != m_rand.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }

  if (m_hasAuts)
    {
      os << "auts: ";
      for (std::vector<uint8_t>::const_iterator it = m_auts.begin (); it != m_auts.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }
}

uint32_t
DirectSecurityModeReject::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = size +
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_pc5SignallingCauseValue);

  if (m_hasRand)
    {
      size += (1 + RandSize); //1 for the type
    }
  if (m_hasAuts)
    {
      size += (1 + AutsSize); //1 for the type
    }
  return size;
}

void
DirectSecurityModeReject::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_pc5SignallingCauseValue);
  if (m_hasRand)
    {
      i.WriteU8 (10);
      for (std::vector<uint8_t>::const_iterator it = m_rand.begin (); it != m_rand.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
  if (m_hasAuts)
    {
      i.WriteU8 (9);
      for (std::vector<uint8_t>::const_iterator it = m_auts.begin (); it != m_auts.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
DirectSecurityModeReject::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_pc5SignallingCauseValue = i.ReadU8 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 10:
          for (uint8_t index = 0; index < RandSize; index++)
            {
              m_rand[index] = i.ReadU8 ();
            }
          m_hasRand = true;
          break;
        case 9:
          for (uint8_t index = 0; index < AutsSize; index++)
            {
              m_auts[index] = i.ReadU8 ();
            }
          m_hasAuts = true;
          break;
        default:
          break;
        }
    }

  return GetSerializedSize ();
}

uint8_t
DirectSecurityModeReject::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectSecurityModeReject::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectSecurityModeReject::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
DirectSecurityModeReject::SetPc5SignallingCauseValue (uint8_t pc5SignallingCauseValue)
{
  m_pc5SignallingCauseValue = pc5SignallingCauseValue;
}

uint8_t
DirectSecurityModeReject::GetPc5SignallingCauseValue ()
{
  return m_pc5SignallingCauseValue;
}


void
DirectSecurityModeReject::SetRand (std::vector<uint8_t> rand)
{
  m_hasRand = true;
  m_rand.assign (RandSize,0);
  for (uint16_t i = 0; i < rand.size (); i++)
    {
      m_rand[i] = rand[i];
    }
}

std::vector<uint8_t>
DirectSecurityModeReject::GetRand ()
{
  return m_rand;
}

void
DirectSecurityModeReject::SetAuts (std::vector<uint8_t> auts)
{
  m_hasAuts = true;
  m_auts.assign (AutsSize,0);
  for (uint16_t i = 0; i < auts.size (); i++)
    {
      m_auts[i] = auts[i];
    }
}

std::vector<uint8_t>
DirectSecurityModeReject::GetAuts ()
{
  return m_auts;
}

/*****     DirectRekeyingRequest Message       *****/

DirectRekeyingRequest::DirectRekeyingRequest ()
{
  m_msgId = 15;

  m_seqNum = 0; //m_seqNum = seq;

  m_ueSecurityCapabilities = 0;
  m_nonce1.clear ();
  m_nonce1.assign (NonceSize,0);
  m_msb = 0;
  m_hasAuthFlag = false;
  m_authFlag = 0;
  m_hasPrukId = false;
  m_prukId.clear ();
  m_prukId.assign (PrukIdSize,0);
}

DirectRekeyingRequest::~DirectRekeyingRequest ()
{

}

TypeId
DirectRekeyingRequest::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectRekeyingRequest")
    .SetParent<Header> ()
    .AddConstructor<DirectRekeyingRequest> ()
  ;
  return tid;
}

TypeId
DirectRekeyingRequest::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectRekeyingRequest::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "ueSecurityCapabilities: " << m_ueSecurityCapabilities << " nonce1: ";

  for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
    {
      os << (uint16_t) *it << " ";
    }

  os << "msb: " << (uint16_t) m_msb << " ";

  if (m_hasAuthFlag)
    {
      os << "authFlag: " << (uint16_t) m_authFlag << " ";
    }

  if (m_hasPrukId)
    {
      os << " prukId: ";
      for (std::vector<uint8_t>::const_iterator it = m_prukId.begin (); it != m_prukId.end (); ++it)
        {
          os << (uint16_t) *it << " ";
        }
    }
}

uint32_t
DirectRekeyingRequest::GetSerializedSize (void) const
{
  uint32_t  size = 0;

  size =  size +
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_ueSecurityCapabilities) +
    NonceSize +
    sizeof (m_msb);

  if (m_hasAuthFlag)
    {
      size += (1 + sizeof (m_authFlag));           //1 for the type
    }

  if (m_hasPrukId)
    {
      size += (1 + PrukIdSize);           //1 for the type
    }

  return size;
}

void
DirectRekeyingRequest::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteHtonU16 (m_ueSecurityCapabilities);
  for (std::vector<uint8_t>::const_iterator it = m_nonce1.begin (); it != m_nonce1.end (); ++it)
    {
      i.WriteU8 (*it);
    }
  i.WriteU8 (m_msb);
  if (m_hasAuthFlag)
    {
      i.WriteU8 (21);
      i.WriteU8 (m_authFlag);
    }
  if (m_hasPrukId)
    {
      i.WriteU8 (8);
      for (std::vector<uint8_t>::const_iterator it = m_prukId.begin (); it != m_prukId.end (); ++it)
        {
          i.WriteU8 (*it);
        }
    }
}

uint32_t
DirectRekeyingRequest::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_ueSecurityCapabilities = i.ReadNtohU16 ();

  for (uint8_t index = 0; index < NonceSize; index++)
    {
      m_nonce1[index] = i.ReadU8 ();
    }

  m_msb = i.ReadU8 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 21:
          m_authFlag = i.ReadU8 ();
          m_hasAuthFlag = true;
          break;
        case 8:
          for (uint8_t index = 0; index < PrukIdSize; index++)
            {
              m_prukId[index] = i.ReadU8 ();
            }
          m_hasPrukId = true;
          break;
        default:
          break;
        }
    }

  return GetSerializedSize ();
}

void
DirectRekeyingRequest::SetParameters (
  uint16_t ueSecurityCapabilities,
  std::vector<uint8_t> nonce1,
  uint8_t msb,
  uint8_t authFlag,
  std::vector<uint8_t> prukId
  )
{
  SetUeSecurityCapabilities (ueSecurityCapabilities);
  SetNonce1 (nonce1);
  SetMsb (msb);
  SetAuthFlag (authFlag);
  SetPrukId (prukId);
}



uint8_t
DirectRekeyingRequest::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectRekeyingRequest::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectRekeyingRequest::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

uint16_t
DirectRekeyingRequest::GetUeSecurityCapabilities ()
{
  return m_ueSecurityCapabilities;
}

void
DirectRekeyingRequest::SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities)
{
  m_ueSecurityCapabilities = ueSecurityCapabilities;
}

std::vector<uint8_t>
DirectRekeyingRequest::GetNonce1 ()
{
  return m_nonce1;
}

void
DirectRekeyingRequest::SetNonce1 ( std::vector<uint8_t> nonce1)
{
  m_nonce1.assign (NonceSize,0);
  for (uint16_t i = 0; i < nonce1.size (); i++)
    {
      m_nonce1[i] = nonce1[i];
    }
}

uint8_t
DirectRekeyingRequest::GetMsb ()
{
  return m_msb;
}

void
DirectRekeyingRequest::SetMsb (uint8_t msb)
{
  m_msb = msb;
}

uint8_t
DirectRekeyingRequest::GetAuthFlag ()
{
  return m_authFlag;
}

void
DirectRekeyingRequest::SetAuthFlag (uint8_t authFlag)
{
  m_hasAuthFlag = true;
  m_authFlag = authFlag;
}

std::vector<uint8_t>
DirectRekeyingRequest::GetPrukId ()
{
  return m_prukId;
}

void
DirectRekeyingRequest::SetPrukId (std::vector<uint8_t> prukId)
{
  m_hasPrukId = true;
  m_prukId.assign (PrukIdSize,0);
  for (uint16_t i = 0; i < prukId.size (); i++)
    {
      m_prukId[i] = prukId[i];
    }
}


/*****     DirectRekeyingResponse Message       *****/

DirectRekeyingResponse::DirectRekeyingResponse ()
{
  m_msgId = 16;

  m_seqNum = 0; //m_seqNum = seq;

}

DirectRekeyingResponse::~DirectRekeyingResponse ()
{

}

TypeId
DirectRekeyingResponse::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectRekeyingResponse")
    .SetParent<Header> ()
    .AddConstructor<DirectRekeyingResponse> ()
  ;
  return tid;
}

TypeId
DirectRekeyingResponse::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectRekeyingResponse::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
  ;
}

uint32_t
DirectRekeyingResponse::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum)
  ;
}

void
DirectRekeyingResponse::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
}

uint32_t
DirectRekeyingResponse::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

uint8_t
DirectRekeyingResponse::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectRekeyingResponse::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectRekeyingResponse::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


/*****     DirectRekeyingTrigger Message       *****/

DirectRekeyingTrigger::DirectRekeyingTrigger ()
{
  m_msgId = 17;

  m_seqNum = 0; //m_seqNum = seq;

}

DirectRekeyingTrigger::~DirectRekeyingTrigger ()
{

}

TypeId
DirectRekeyingTrigger::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DirectRekeyingTrigger")
    .SetParent<Header> ()
    .AddConstructor<DirectRekeyingTrigger> ()
  ;
  return tid;
}

TypeId
DirectRekeyingTrigger::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
DirectRekeyingTrigger::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
  ;
}

uint32_t
DirectRekeyingTrigger::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum)
  ;
}

void
DirectRekeyingTrigger::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
}

uint32_t
DirectRekeyingTrigger::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

uint8_t
DirectRekeyingTrigger::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
DirectRekeyingTrigger::GetSequenceNumber ()
{
  return m_seqNum;
}

void
DirectRekeyingTrigger::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

/*****     RemoteUeInfoRequest Message       *****/

RemoteUeInfoRequest::RemoteUeInfoRequest ()
{
  m_msgId = 18;

  m_seqNum = 0; //m_seqNum = seq;

  m_remoteUeInfoType = 0;
}

RemoteUeInfoRequest::~RemoteUeInfoRequest ()
{

}

TypeId
RemoteUeInfoRequest::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RemoteUeInfoRequest")
    .SetParent<Header> ()
    .AddConstructor<RemoteUeInfoRequest> ()
  ;
  return tid;
}

TypeId
RemoteUeInfoRequest::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
RemoteUeInfoRequest::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " "
     << "remoteUeInfoType: " << (uint16_t) m_remoteUeInfoType
  ;
}

uint32_t
RemoteUeInfoRequest::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_remoteUeInfoType)
  ;
}

void
RemoteUeInfoRequest::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_remoteUeInfoType);
}

uint32_t
RemoteUeInfoRequest::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_remoteUeInfoType = i.ReadU8 ();
  return GetSerializedSize ();
}

uint8_t
RemoteUeInfoRequest::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
RemoteUeInfoRequest::GetSequenceNumber ()
{
  return m_seqNum;
}

void
RemoteUeInfoRequest::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

uint8_t
RemoteUeInfoRequest::GetRemoteUeInfoType ()
{
  return m_remoteUeInfoType;
}

void
RemoteUeInfoRequest::SetRemoteUeInfoType (uint8_t remoteUeInfoType)
{
  m_remoteUeInfoType = remoteUeInfoType;
}


/*****     RemoteUeInfoResponse Message       *****/

RemoteUeInfoResponse::RemoteUeInfoResponse ()
{
  m_msgId = 19;

  m_seqNum = 0; //m_seqNum = seq;

  m_hasImei = false;
  m_imei = 0;
}

RemoteUeInfoResponse::~RemoteUeInfoResponse ()
{

}

TypeId
RemoteUeInfoResponse::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RemoteUeInfoResponse")
    .SetParent<Header> ()
    .AddConstructor<RemoteUeInfoResponse> ()
  ;
  return tid;
}

TypeId
RemoteUeInfoResponse::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
RemoteUeInfoResponse::Print (std::ostream &os) const
{
  os << "msgId: " << (uint16_t) m_msgId << " "
     << "seqNum: " << m_seqNum << " ";

  if (m_hasImei)
    {
      os << "imeiType: 4 ";
      os << "imei: " << m_imei << " ";
    }
}

uint32_t
RemoteUeInfoResponse::GetSerializedSize (void) const
{
  uint32_t  size = 0;
  size = size +
    sizeof (m_msgId) +
    sizeof (m_seqNum);

  if (m_hasImei)
    {
      size += (1 + sizeof (m_imei));                    //1 for the type
    }

  return size;
}

void
RemoteUeInfoResponse::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  if (m_hasImei)
    {
      i.WriteU8 (27);
      i.WriteU64 (m_imei);
    }
}


uint32_t
RemoteUeInfoResponse::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();

  uint8_t opt_vars_iei = 0;
  while (!i.IsEnd ())
    {
      opt_vars_iei = i.ReadU8 ();
      switch (opt_vars_iei)
        {
        case 27:
          m_imei = i.ReadU64 ();
          m_hasImei = true;
          break;
        default:
          break;
        }
    }

  return GetSerializedSize ();
}

uint8_t
RemoteUeInfoResponse::GetMessageIdentifier ()
{
  return m_msgId;
}

uint16_t
RemoteUeInfoResponse::GetSequenceNumber ()
{
  return m_seqNum;
}

void
RemoteUeInfoResponse::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

uint64_t
RemoteUeInfoResponse::GetImei ()
{
  return m_imei;
}

void
RemoteUeInfoResponse::SetImei (uint64_t imei)
{
  m_hasImei = true;
  m_imei = imei;
}

} // namespace ns3

