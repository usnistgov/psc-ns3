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
uint64_t NrPc5SignallingHeaderSequenceNumber::s_seqNum = 0;

/** Size of the Nonce field */
static const uint8_t NonceSize = 16;

/** Size of the signature field */
static const uint8_t SignatureSize = 129;

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
ProseDirectLinkEstablishmentRequest::GetSerializedSize (void) const
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
ProseDirectLinkEstablishmentRequest::Serialize (Buffer::Iterator start) const
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
ProseDirectLinkEstablishmentRequest::Deserialize (Buffer::Iterator start)
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
ProseDirectLinkEstablishmentRequest::SetParameters (
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
ProseDirectLinkEstablishmentRequest::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
ProseDirectLinkEstablishmentRequest::SetUserInfo (uint8_t userInfoType, uint64_t userInfo)
{
  NS_ASSERT_MSG ( userInfoType == 3, "Only User Info Type = 3 (IMSI) supported"); //Refer 24.334 Section 12.5.1.3
  m_userInfoType = userInfoType;
  m_userInfo = userInfo;
}

uint64_t
ProseDirectLinkEstablishmentRequest::GetUserInfo ()
{
  return m_userInfo;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetUserInfoType ()
{
  return m_userInfoType;
}

void
ProseDirectLinkEstablishmentRequest::SetImsi (uint64_t imsi)
{
  SetUserInfo (3, imsi);
}

uint64_t
ProseDirectLinkEstablishmentRequest::GetImsi ()
{
  return m_userInfo;
}

void
ProseDirectLinkEstablishmentRequest::SetIpAddressConfig (uint8_t ipAddressConfig)
{
  m_ipAddressConfig = ipAddressConfig;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetIpAddressConfig ()
{
  return m_ipAddressConfig;
}

void
ProseDirectLinkEstablishmentRequest::SetMaximumInactivityPeriod (uint32_t maxInactivityPeriod)
{
  m_maxInactivityPeriod = maxInactivityPeriod;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetMaximumInactivityPeriod ()
{
  return m_maxInactivityPeriod;
}

void
ProseDirectLinkEstablishmentRequest::SetNonce1 (std::vector<uint8_t> nonce1)
{
  m_nonce1.assign (NonceSize,0);
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
ProseDirectLinkEstablishmentRequest::SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities)
{
  m_ueSecurityCapabilities = ueSecurityCapabilities;
}

uint16_t
ProseDirectLinkEstablishmentRequest::GetUeSecurityCapabilities ()
{
  return m_ueSecurityCapabilities;
}

void
ProseDirectLinkEstablishmentRequest::SetMsbKdId (uint8_t msb)
{
  m_msb = msb;
}

uint8_t
ProseDirectLinkEstablishmentRequest::GetMsbKdId ()
{
  return m_msb;
}

void
ProseDirectLinkEstablishmentRequest::SetKdId (uint32_t kdId)
{
  m_hasKdId = true;
  m_kdId = kdId;
}

uint32_t
ProseDirectLinkEstablishmentRequest::GetKdId ()
{
  return m_kdId;
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

void
ProseDirectLinkEstablishmentRequest::SetSignature (std::vector<uint8_t> signature)
{
  m_hasSignature = true;
  m_signature.assign (SignatureSize,0);
  for (uint16_t i = 0; i < signature.size (); i++)
    {
      m_signature[i] = signature[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentRequest::GetSignature ()
{
  return m_signature;
}

void
ProseDirectLinkEstablishmentRequest::SetLinkLocalIpv6Address (std::vector<uint8_t> linkLocalIpv6Address)
{
  m_hasLinkLocalIpv6Address = true;
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
  for (uint16_t i = 0; i < linkLocalIpv6Address.size (); i++)
    {
      m_linkLocalIpv6Address[i] = linkLocalIpv6Address[i];
    }
}

std::vector<uint8_t>
ProseDirectLinkEstablishmentRequest::GetLinkLocalIpv6Address ()
{
  return m_linkLocalIpv6Address;
}

/*****     ProseDirectLinkEstablishmentAccept Message       *****/

ProseDirectLinkEstablishmentAccept::ProseDirectLinkEstablishmentAccept ()
{
  m_msgId = 2;

  m_seqNum = 0; //m_seqNum = seq;

  m_ipAddressConfig = 0;
  m_hasLinkLocalIpv6Address = false;
  m_linkLocalIpv6Address.clear ();
  m_linkLocalIpv6Address.assign (LinkLocalIpv6AddressSize,0);
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
ProseDirectLinkEstablishmentAccept::GetSerializedSize (void) const
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
ProseDirectLinkEstablishmentAccept::Serialize (Buffer::Iterator start) const
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
ProseDirectLinkEstablishmentAccept::Deserialize (Buffer::Iterator start)
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
ProseDirectLinkEstablishmentAccept::SetParameters (uint8_t ipAddressConfig, std::vector<uint8_t> linkLocalIpv6Address)
{
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
ProseDirectLinkEstablishmentAccept::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}

void
ProseDirectLinkEstablishmentAccept::SetIpAddressConfig (uint8_t ipAddressConfig)
{
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

  m_seqNum = 0; //m_seqNum = seq;

  m_pc5SignallingCauseValue = 0;
}

ProseDirectLinkEstablishmentReject::~ProseDirectLinkEstablishmentReject ()
{}

TypeId
ProseDirectLinkEstablishmentReject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProseDirectLinkEstablishmentReject")
    .SetParent<Header> ()
    .AddConstructor<ProseDirectLinkEstablishmentReject> ()
  ;
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
     << "seqNum: " << m_seqNum << " "
     << "pc5SignallingCauseValue: " << (uint16_t) m_pc5SignallingCauseValue;
}

uint32_t
ProseDirectLinkEstablishmentReject::GetSerializedSize (void) const
{
  return
    sizeof (m_msgId) +
    sizeof (m_seqNum) +
    sizeof (m_pc5SignallingCauseValue)
  ;
}

void
ProseDirectLinkEstablishmentReject::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_msgId);
  i.WriteHtonU16 (m_seqNum);
  i.WriteU8 (m_pc5SignallingCauseValue);
}

uint32_t
ProseDirectLinkEstablishmentReject::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_msgId = i.ReadU8 ();
  m_seqNum = i.ReadNtohU16 ();
  m_pc5SignallingCauseValue = i.ReadU8 ();
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
ProseDirectLinkEstablishmentReject::SetSequenceNumber (uint32_t seq)
{
  m_seqNum = seq;
}


void
ProseDirectLinkEstablishmentReject::SetPc5SignallingCauseValue (uint8_t pc5SignallingCauseValue)
{
  m_pc5SignallingCauseValue = pc5SignallingCauseValue;
}

uint8_t
ProseDirectLinkEstablishmentReject::GetPc5SignallingCauseValue ()
{
  return m_pc5SignallingCauseValue;
}



} // namespace ns3

