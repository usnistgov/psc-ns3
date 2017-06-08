/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.cat>
 */

#include "nist-epc-gtpu-header.h"
#include "ns3/log.h"
#include "ns3/packet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistGtpuHeader");

/********************************************************
 *        GTP-U-v1 Header
 ********************************************************/

NS_OBJECT_ENSURE_REGISTERED (NistGtpuHeader);

TypeId
NistGtpuHeader::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::NistGtpuHeader")
    .SetParent<Header> ()
    .AddConstructor<NistGtpuHeader> ();
  return tid;
}
NistGtpuHeader::NistGtpuHeader ()
  : m_version (1),
    m_protocolType (true),
    m_extensionHeaderFlag (false),
    m_sequenceNumberFlag (true),
    m_nPduNumberFlag (true),
    m_messageType (255),
    m_length (0),
    m_teid (0),
    m_sequenceNumber (0),
    m_nPduNumber (0),
    m_nextExtensionType (0)
{

}

NistGtpuHeader::~NistGtpuHeader ()
{
}

TypeId
NistGtpuHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistGtpuHeader::GetSerializedSize (void) const
{
  return 12;
}
void
NistGtpuHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  uint8_t firstByte = m_version << 5 | m_protocolType << 4 | 0x1 << 3;
  firstByte |=  m_extensionHeaderFlag << 2 |  m_sequenceNumberFlag << 1 | m_nPduNumberFlag;
  i.WriteU8 (firstByte);
  i.WriteU8 (m_messageType);
  i.WriteHtonU16 (m_length);
  i.WriteHtonU32 (m_teid);
  i.WriteHtonU16 (m_sequenceNumber);
  i.WriteU8 (m_nPduNumber);
  i.WriteU8 (m_nextExtensionType);

}
uint32_t
NistGtpuHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t firstByte = i.ReadU8 ();
  m_version = firstByte >> 5 & 0x7;
  m_protocolType = firstByte >> 4 & 0x1;
  m_extensionHeaderFlag = firstByte >> 2 & 0x1;
  m_sequenceNumberFlag  = firstByte >> 1 & 0x1;
  m_nPduNumberFlag = firstByte & 0x1;
  m_messageType = i.ReadU8 ();
  m_length = i.ReadNtohU16 ();
  m_teid = i.ReadNtohU32 ();
  m_sequenceNumber = i.ReadNtohU16 ();
  m_nPduNumber = i.ReadU8 ();
  m_nextExtensionType = i.ReadU8 ();
  return GetSerializedSize ();
}
void
NistGtpuHeader::Print (std::ostream &os) const
{
  os << " version=" << (uint32_t) m_version << " [";
  if (m_protocolType)
    {
      os << " PT ";
    }
  if (m_extensionHeaderFlag)
    {
      os << " E ";
    }
  if (m_sequenceNumberFlag)
    {
      os << " S ";
    }
  if (m_nPduNumberFlag)
    {
      os << " PN ";
    }
  os << "], messageType=" << (uint32_t) m_messageType << ", length=" << (uint32_t) m_length;
  os << ", teid=" << (uint32_t) m_teid << ", sequenceNumber=" << (uint32_t) m_sequenceNumber;
  os << ", nPduNumber=" << (uint32_t) m_nPduNumber << ", nextExtensionType=" << (uint32_t) m_nextExtensionType;
}

bool
NistGtpuHeader::GetExtensionHeaderFlag () const
{
  return m_extensionHeaderFlag;
}

uint16_t
NistGtpuHeader::GetLength () const
{
  return m_length;
}

uint8_t
NistGtpuHeader::GetMessageType () const
{
  return m_messageType;
}

uint8_t
NistGtpuHeader::GetNPduNumber () const
{
  return m_nPduNumber;
}

bool
NistGtpuHeader::GetNPduNumberFlag () const
{
  return m_nPduNumberFlag;
}

uint8_t
NistGtpuHeader::GetNextExtensionType () const
{
  return m_nextExtensionType;
}

bool
NistGtpuHeader::GetProtocolType () const
{
  return m_protocolType;
}

uint16_t
NistGtpuHeader::GetSequenceNumber () const
{
  return m_sequenceNumber;
}

bool
NistGtpuHeader::GetSequenceNumberFlag () const
{
  return m_sequenceNumberFlag;
}

uint32_t
NistGtpuHeader::GetTeid () const
{
  return m_teid;
}

uint8_t
NistGtpuHeader::GetVersion () const
{
  return m_version;
}

void
NistGtpuHeader::SetExtensionHeaderFlag (bool m_extensionHeaderFlag)
{
  this->m_extensionHeaderFlag = m_extensionHeaderFlag;
}

void
NistGtpuHeader::SetLength (uint16_t m_length)
{
  this->m_length = m_length;
}

void
NistGtpuHeader::SetMessageType (uint8_t m_messageType)
{
  this->m_messageType = m_messageType;
}

void
NistGtpuHeader::SetNPduNumber (uint8_t m_nPduNumber)
{
  this->m_nPduNumber = m_nPduNumber;
}

void
NistGtpuHeader::SetNPduNumberFlag (bool m_nPduNumberFlag)
{
  this->m_nPduNumberFlag = m_nPduNumberFlag;
}

void
NistGtpuHeader::SetNextExtensionType (uint8_t m_nextExtensionType)
{
  this->m_nextExtensionType = m_nextExtensionType;
}

void
NistGtpuHeader::SetProtocolType (bool m_protocolType)
{
  this->m_protocolType = m_protocolType;
}

void
NistGtpuHeader::SetSequenceNumber (uint16_t m_sequenceNumber)
{
  this->m_sequenceNumber = m_sequenceNumber;
}

void
NistGtpuHeader::SetSequenceNumberFlag (bool m_sequenceNumberFlag)
{
  this->m_sequenceNumberFlag = m_sequenceNumberFlag;
}

void
NistGtpuHeader::SetTeid (uint32_t m_teid)
{
  this->m_teid = m_teid;
}

void
NistGtpuHeader::SetVersion (uint8_t m_version)
{
  // m_version is a uint3_t
  this->m_version = m_version & 0x7;
}

bool
NistGtpuHeader::operator == (const NistGtpuHeader& b) const
{
  if (m_version == b.m_version
      && m_protocolType == b.m_protocolType
      && m_extensionHeaderFlag == b.m_extensionHeaderFlag
      && m_sequenceNumberFlag == b.m_sequenceNumberFlag
      && m_nPduNumberFlag == b.m_nPduNumberFlag
      && m_messageType == b.m_messageType
      && m_length == b.m_length
      && m_teid == b.m_teid
      && m_sequenceNumber == b.m_sequenceNumber
      && m_nPduNumber == b.m_nPduNumber
      && m_nextExtensionType == b.m_nextExtensionType
      )
    {
      return true;
    }
  return false;
}

} // namespace ns3

