/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 * 
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 * 
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

// Some elements ported from NIST ns-2.31 SIP implementation

#include "sip-header.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SipHeader);

std::string
SipMessageTypeToString (SipHeader::SipMessageType messageType)
{
  switch (messageType)
    {
    case SipHeader::SIP_REQUEST:
      return "Request";
    case SipHeader::SIP_RESPONSE:
      return "Response";
    default:
      return "Unknown";
    }
  return "Unknown";
}

std::string
SipMethodToString (SipHeader::SipMethod method)
{
  switch (method)
    {
    case SipHeader::INVITE:
      return "INVITE";
    case SipHeader::BYE:
      return "BYE";
    case SipHeader::INVALID_METHOD:
      return "Invalid";
    default:
      return "Unknown";
    }
  return "Unknown";
}

SipHeader::SipHeader ()
{
}

SipHeader::~SipHeader ()
{
}

TypeId 
SipHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SipHeader")
    .SetParent<Header> ()
    .SetGroupName ("Mcptt")
    .AddConstructor<SipHeader> ()
  ;
  return tid;
}

  // These eight methods need doxygen

void
SipHeader::SetMessageType (SipHeader::SipMessageType messageType)
{
  m_messageType = messageType;
}

SipHeader::SipMessageType
SipHeader::GetMessageType (void) const
{
  return SipHeader::SipMessageType (m_messageType);
}

void
SipHeader::SetMethod (SipMethod method)
{
  m_method = method;
}

SipHeader::SipMethod
SipHeader::GetMethod (void) const
{
  return SipHeader::SipMethod (m_method);
}

void
SipHeader::SetStatusCode (uint16_t statusCode)
{
  m_statusCode = statusCode;
}

uint16_t
SipHeader::GetStatusCode (void) const
{
  return m_statusCode;
}

void
SipHeader::SetRequestUri (uint32_t requestUri)
{
  m_requestUri = requestUri;
}

uint32_t
SipHeader::GetRequestUri (void) const
{
  return m_requestUri;
}

void
SipHeader::SetFrom (uint32_t from)
{
  m_from = from;
}

uint32_t
SipHeader::GetFrom (void) const
{
  return m_from;
}

void
SipHeader::SetTo (uint32_t to)
{
  m_to = to;
}

uint32_t
SipHeader::GetTo (void) const
{
  return m_to;
}

void
SipHeader::SetCallId (uint16_t callId)
{
  m_callId = callId;
}

uint16_t
SipHeader::GetCallId (void) const
{
  return m_callId;
}

TypeId 
SipHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void 
SipHeader::Print (std::ostream &os) const
{
  if (m_messageType == SipHeader::SIP_REQUEST)
    {
      os << SipMessageTypeToString (SipHeader::SipMessageType (m_messageType)) << " "
         << SipMethodToString (SipHeader::SipMethod (m_method)) << " "
         << "RequestUri=" << m_requestUri << " "
      ;
    }
  else if (m_messageType == SipHeader::SIP_RESPONSE)
    {
      os << SipMessageTypeToString (SipHeader::SipMessageType (m_messageType)) << " "
         << "Status Code: " << m_statusCode << " "
         << SipMethodToString (SipHeader::SipMethod (m_method)) << " "
      ;
    }
  os << "From=" << m_from << " "
     << "To=" << m_to << " "
     << "CallId=" << m_callId
  ;
}

uint32_t 
SipHeader::GetSerializedSize (void) const
{
  return 1 + 1 + 2 + 4 + 4 + 4 + 2;
}

void
SipHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_messageType);
  i.WriteU8 (m_method);
  i.WriteHtonU16 (m_statusCode);
  i.WriteHtonU32 (m_requestUri);
  i.WriteHtonU32 (m_from);
  i.WriteHtonU32 (m_to);
  i.WriteHtonU16 (m_callId);
}

uint32_t
SipHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_messageType = i.ReadU8 ();
  m_method = i.ReadU8 ();
  m_statusCode = i.ReadNtohU16 (); 
  m_requestUri = i.ReadNtohU32 ();
  m_from = i.ReadNtohU32 ();
  m_to = i.ReadNtohU32 ();
  m_callId = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

} // namespace ns3
