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

#include <iostream>

#include <ns3/buffer.h>
#include <ns3/header.h>
#include <ns3/log.h>
#include <ns3/type-id.h>

#include "mcptt-rtp-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttRtpHeader");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttRtpHeader);

TypeId
McpttRtpHeader::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttRtpHeader")
    .SetParent<Header> ()
    .AddConstructor<McpttRtpHeader> ()
  ;

  return tid;
}

McpttRtpHeader::McpttRtpHeader (void)
  : Header (),
    m_csrcCount (0),
    m_csrcs (std::vector<uint32_t> ()),
    m_extension (0),
    m_marker (0),
    m_padding (0),
    m_payloadType (0),
    m_seqNum (0),
    m_ssrc (0),
    m_timestamp (0),
    m_version (0)
{
  NS_LOG_FUNCTION (this);
}

McpttRtpHeader::McpttRtpHeader (uint32_t ssrc)
  : Header (),
    m_csrcCount (0),
    m_csrcs (std::vector<uint32_t> ()),
    m_extension (0),
    m_marker (0),
    m_padding (0),
    m_payloadType (0),
    m_seqNum (0),
    m_ssrc (ssrc),
    m_timestamp (0),
    m_version (0)
{
  NS_LOG_FUNCTION (this);
}

McpttRtpHeader::McpttRtpHeader (uint32_t ssrc, uint8_t payloadType)
  : Header (),
    m_csrcCount (0),
    m_csrcs (std::vector<uint32_t> ()),
    m_extension (0),
    m_marker (0),
    m_padding (0),
    m_payloadType (payloadType),
    m_seqNum (0),
    m_ssrc (ssrc),
    m_timestamp (0),
    m_version (0)
{
  NS_LOG_FUNCTION (this);
}


McpttRtpHeader::~McpttRtpHeader (void)
{
  NS_LOG_FUNCTION (this);
}

bool
McpttRtpHeader::AddCsrc (uint32_t csrc)
{
  NS_LOG_FUNCTION (this << csrc);

  bool success = false;
  std::vector<uint32_t> csrcs = GetCsrcs ();

  if (csrcs.size () < 15)
    {
      success = true;

      csrcs.push_back (csrc);

      SetCsrcs (csrcs);
      SetCsrcCount ((uint8_t)csrcs.size ());
    }

  return success;
}

void
McpttRtpHeader::ClearCsrcs (void)
{
  NS_LOG_FUNCTION (this);

  std::vector<uint32_t> csrcs = GetCsrcs ();

  csrcs.clear ();

  SetCsrcs (csrcs);
  SetCsrcCount (0);
}

uint32_t
McpttRtpHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);

  uint32_t bytesRead = 0;

  uint8_t firstByte = start.ReadU8 ();
  bytesRead += 1;
  uint8_t csrcCount = ((firstByte >> 0) & 0x0F);
  uint8_t extension = ((firstByte >> 4) & 0x01);
  uint8_t padding = ((firstByte >> 5) & 0x01);
  uint8_t version = ((firstByte >> 6) & 0x03);

  uint8_t secondByte = start.ReadU8 ();
  bytesRead += 1;
  uint8_t payloadType = (secondByte & 0x7F);
  uint8_t marker = ((secondByte >> 7) & 0x1);

  uint16_t seqNum = start.ReadNtohU16 ();
  bytesRead += 2;
  uint32_t timestamp = start.ReadNtohU32 ();
  bytesRead += 4;
  uint32_t ssrc = start.ReadNtohU32 ();
  bytesRead += 4;

  std::vector<uint32_t> csrcs;
  for (int idx = 0; idx < csrcCount; idx++)
    {
      bytesRead += 4;

      csrcs.push_back (start.ReadNtohU32 ());
    }

  SetCsrcCount (csrcCount);
  SetExtension (extension);
  SetPadding (padding);
  SetVersion (version);
  SetPayloadType (payloadType);
  SetMarker (marker);
  SetSeqNum (seqNum);
  SetTimestamp (timestamp);
  SetSsrc (ssrc);
  SetCsrcs (csrcs);

  return bytesRead;
}

TypeId
McpttRtpHeader::GetInstanceTypeId (void) const
{
  return McpttRtpHeader::GetTypeId ();
}

uint32_t
McpttRtpHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  uint8_t size = 0;
  uint8_t csrcCount = GetCsrcCount ();

  size += 1; // V, P, X, CC fields.
  size += 1; // M and PT fields.
  size += 2; // Sequence Number field.
  size += 4; // Timestamp field.
  size += 4; // SSRC field.
  size += (csrcCount * 4); // CSRC field.

  return size;
}

void
McpttRtpHeader::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  uint32_t version = GetVersion ();
  uint32_t padding = GetPadding ();
  uint32_t extension = GetExtension ();
  uint32_t csrcCount = GetCsrcCount ();
  uint32_t marker = GetMarker ();
  uint32_t payloadType = GetPayloadType ();
  uint32_t seqNum = GetSeqNum ();
  uint32_t timestamp = GetTimestamp ();
  uint32_t ssrc = GetSsrc ();
  std::vector<uint32_t> csrcs = GetCsrcs ();

  os << "McpttRtpHeader(";
  os << "V=" << version;
  os << ";P=" << padding;
  os << ";X=" << extension;
  os << ";CC=" << csrcCount;
  os << ";M=" << marker;
  os << ";PT=" << payloadType;
  os << ";seqNum=" << seqNum;
  os << ";timestamp=" << timestamp;
  os << ";SSRC=" << ssrc;
  os << ";CSRC=[";

  for (uint32_t idx = 0; idx < csrcCount; idx++)
    {
      os << csrcs[idx] << (idx == (csrcs.size () - 1) ? "" : ",");
    }

  os << "]";
  os << ")";
}

bool
McpttRtpHeader::RemoveCsrc (uint32_t csrc)
{
  NS_LOG_FUNCTION (this << csrc);

  bool found = false;
  std::vector<uint32_t> csrcs = GetCsrcs ();

  for (uint32_t idx = 0; idx < csrcs.size () && !found; idx++)
    {
      if (csrcs[idx] == csrc)
        {
          found = true;
          csrcs.erase (csrcs.begin () + idx);
        }
    }

  SetCsrcs (csrcs);

  return found;
}

void
McpttRtpHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this);

  uint8_t version = GetVersion ();
  uint8_t padding = GetPadding ();
  uint8_t extension = GetExtension ();
  uint8_t csrcCount = GetCsrcCount ();
  uint8_t marker = GetMarker ();
  uint8_t payloadType = GetPayloadType ();
  uint16_t seqNum = GetSeqNum ();
  uint32_t timestamp = GetTimestamp ();
  uint32_t ssrc = GetSsrc ();
  std::vector<uint32_t> csrcs = GetCsrcs ();

  uint8_t firstByte = 0;
  firstByte += (version << 6);
  firstByte += (padding << 5);
  firstByte += (extension << 4);
  firstByte += csrcCount;
  start.WriteU8 (firstByte);

  uint8_t secondByte = 0;
  secondByte += (marker << 7);
  secondByte += (payloadType);
  start.WriteU8 (secondByte);

  start.WriteHtonU16 (seqNum);
  start.WriteHtonU32 (timestamp);
  start.WriteHtonU32 (ssrc);

  for (int idx = 0; idx < csrcCount; csrcCount++)
    {
      start.WriteHtonU32 (csrcs[idx]);
    }
}

void
McpttRtpHeader::SetCsrcCount (uint8_t csrcCount)
{
  NS_LOG_FUNCTION (this << (uint32_t)csrcCount);

  NS_ASSERT_MSG (csrcCount < 16, "CC must be less than 16");

  m_csrcCount = csrcCount;
}

void
McpttRtpHeader::SetCsrcs (const std::vector<uint32_t>  csrcs)
{
  NS_LOG_FUNCTION (this << &csrcs);

  NS_ASSERT_MSG (csrcs.size () < 16, "Number of CSRCs must be less than 16");

  m_csrcs = csrcs;
}

uint8_t
McpttRtpHeader::GetCsrcCount (void) const
{
  NS_LOG_FUNCTION (this);

  return m_csrcCount;
}

std::vector<uint32_t>
McpttRtpHeader::GetCsrcs (void) const
{
  NS_LOG_FUNCTION (this);

  return m_csrcs;
}

uint8_t
McpttRtpHeader::GetExtension (void) const
{
  NS_LOG_FUNCTION (this);

  return m_extension;
}

uint8_t
McpttRtpHeader::GetMarker (void) const
{
  NS_LOG_FUNCTION (this);

  return m_marker;
}

uint8_t
McpttRtpHeader::GetPadding (void) const
{
  NS_LOG_FUNCTION (this);

  return m_padding;
}

uint8_t
McpttRtpHeader::GetPayloadType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_payloadType;
}

uint16_t
McpttRtpHeader::GetSeqNum (void) const
{
  NS_LOG_FUNCTION (this);

  return m_seqNum;
}

uint32_t
McpttRtpHeader::GetSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return m_ssrc;
}

uint32_t
McpttRtpHeader::GetTimestamp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_timestamp;
}

uint8_t
McpttRtpHeader::GetVersion (void) const
{
  NS_LOG_FUNCTION (this);

  return m_version;
}

void
McpttRtpHeader::SetExtension (uint8_t extension)
{
  NS_LOG_FUNCTION (this << (uint32_t)extension);

  NS_ASSERT_MSG (extension < 2, "The X field must be less than two.");

  m_extension = extension;
}

void
McpttRtpHeader::SetMarker (uint8_t marker)
{
  NS_LOG_FUNCTION (this << (uint32_t)marker);

  NS_ASSERT_MSG (marker < 2, "The M field must be less than two.");

  m_marker = marker;
}

void
McpttRtpHeader::SetPadding (uint8_t padding)
{
  NS_LOG_FUNCTION (this << (uint32_t)padding);

  NS_ASSERT_MSG (padding < 2, "The P field must be less than two.");

  m_padding = padding;
}

void
McpttRtpHeader::SetPayloadType (uint8_t payloadType)
{
  NS_LOG_FUNCTION (this << (uint32_t)payloadType);

  NS_ASSERT_MSG (payloadType < 128, "The PT field must be less than 128.");

  m_payloadType = payloadType;
}

void
McpttRtpHeader::SetSeqNum (uint16_t seqNum)
{
  NS_LOG_FUNCTION (this << (uint32_t)seqNum);

  m_seqNum = seqNum;
}

void
McpttRtpHeader::SetSsrc (uint32_t ssrc)
{
  NS_LOG_FUNCTION (this << ssrc);

  m_ssrc = ssrc;
}

void
McpttRtpHeader::SetTimestamp (uint32_t timestamp)
{
  NS_LOG_FUNCTION (this << timestamp);

  m_timestamp = timestamp;
}

void
McpttRtpHeader::SetVersion (uint8_t version)
{
  NS_LOG_FUNCTION (this << (uint32_t)version);

  m_version = version;
}

} // namespace psc
} // namespace ns3

