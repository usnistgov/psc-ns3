/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/log.h"
#include "ns3/nist-epc-x2-header.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcX2Header");

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2Header);

NistEpcX2Header::NistEpcX2Header ()
  : m_messageType (0xfa),
    m_procedureCode (0xfa),
    m_lengthOfIes (0xfa),
    m_numberOfIes (0xfa)
{
}

NistEpcX2Header::~NistEpcX2Header ()
{
  m_messageType = 0xfb;
  m_procedureCode = 0xfb;
  m_lengthOfIes = 0xfb;
  m_numberOfIes = 0xfb;
}

TypeId
NistEpcX2Header::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2Header")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2Header> ()
  ;
  return tid;
}

TypeId
NistEpcX2Header::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2Header::GetSerializedSize (void) const
{
  return 7;
}

void
NistEpcX2Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_messageType);
  i.WriteU8 (m_procedureCode);

  i.WriteU8 (0x00); // criticality = REJECT
  i.WriteU8 (m_lengthOfIes + 3);
  i.WriteHtonU16 (0);
  i.WriteU8 (m_numberOfIes);
}

uint32_t
NistEpcX2Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_messageType = i.ReadU8 ();
  m_procedureCode = i.ReadU8 ();

  i.ReadU8 ();
  m_lengthOfIes = i.ReadU8 () - 3;
  i.ReadNtohU16 ();
  m_numberOfIes = i.ReadU8 ();
  
  return GetSerializedSize ();
}

void
NistEpcX2Header::Print (std::ostream &os) const
{
  os << "MessageType=" << (uint32_t) m_messageType;
  os << " ProcedureCode=" << (uint32_t) m_procedureCode;
  os << " LengthOfIEs=" << (uint32_t) m_lengthOfIes;
  os << " NumberOfIEs=" << (uint32_t) m_numberOfIes;
}

uint8_t
NistEpcX2Header::GetMessageType () const
{
  return m_messageType;
}

void
NistEpcX2Header::SetMessageType (uint8_t messageType)
{
  m_messageType = messageType;
}

uint8_t
NistEpcX2Header::GetProcedureCode () const
{
  return m_procedureCode;
}

void
NistEpcX2Header::SetProcedureCode (uint8_t procedureCode)
{
  m_procedureCode = procedureCode;
}


void
NistEpcX2Header::SetLengthOfIes (uint32_t lengthOfIes)
{
  m_lengthOfIes = lengthOfIes;
}

void
NistEpcX2Header::SetNumberOfIes (uint32_t numberOfIes)
{
  m_numberOfIes = numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2HandoverRequestHeader);

NistEpcX2HandoverRequestHeader::NistEpcX2HandoverRequestHeader ()
  : m_numberOfIes (1 + 1 + 1 + 1),
    m_headerLength (6 + 5 + 12 + (3 + 4 + 8 + 8 + 4)),
    m_oldEnbUeX2apId (0xfffa),
    m_cause (0xfffa),
    m_targetCellId (0xfffa),
    m_mmeUeS1apId (0xfffffffa)
{
  m_erabsToBeSetupList.clear ();
}

NistEpcX2HandoverRequestHeader::~NistEpcX2HandoverRequestHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_cause = 0xfffb;
  m_targetCellId = 0xfffb;
  m_mmeUeS1apId = 0xfffffffb;
  m_erabsToBeSetupList.clear ();
}

TypeId
NistEpcX2HandoverRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2HandoverRequestHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2HandoverRequestHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2HandoverRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2HandoverRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2HandoverRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (10);              // id = OLD_ENB_UE_X2AP_ID
  i.WriteU8 (0);                    // criticality = REJECT
  i.WriteU8 (2);                    // length of OLD_ENB_UE_X2AP_ID
  i.WriteHtonU16 (m_oldEnbUeX2apId);

  i.WriteHtonU16 (5);               // id = CAUSE
  i.WriteU8 (1 << 6);               // criticality = IGNORE
  i.WriteU8 (1);                    // length of CAUSE
  i.WriteU8 (m_cause);

  i.WriteHtonU16 (11);              // id = TARGET_CELLID
  i.WriteU8 (0);                    // criticality = REJECT
  i.WriteU8 (8);                    // length of TARGET_CELLID
  i.WriteHtonU32 (0x123456);        // fake PLMN
  i.WriteHtonU32 (m_targetCellId << 4);

  i.WriteHtonU16 (14);              // id = UE_CONTEXT_INFORMATION
  i.WriteU8 (0);                    // criticality = REJECT

  i.WriteHtonU32 (m_mmeUeS1apId);
  i.WriteHtonU64 (m_ueAggregateMaxBitRateDownlink);
  i.WriteHtonU64 (m_ueAggregateMaxBitRateUplink);

  std::vector <NistEpcX2Sap::NistErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size (); 
  i.WriteHtonU32 (sz);              // number of bearers
  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_erabsToBeSetupList [j].erabId);
      i.WriteHtonU16 (m_erabsToBeSetupList [j].erabLevelQosParameters.qci);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.gbrDl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.gbrUl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.mbrDl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.mbrUl);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.priorityLevel);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.preemptionCapability);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.preemptionVulnerability);
      i.WriteU8 (m_erabsToBeSetupList [j].dlForwarding);
      i.WriteHtonU32 (m_erabsToBeSetupList [j].transportLayerAddress.Get ());
      i.WriteHtonU32 (m_erabsToBeSetupList [j].gtpTeid);
    }

}

uint32_t
NistEpcX2HandoverRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_headerLength += 6;
  m_numberOfIes++;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  m_cause = i.ReadU8 ();
  m_headerLength += 5;
  m_numberOfIes++;
  
  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  i.ReadNtohU32 ();
  m_targetCellId = i.ReadNtohU32 () >> 4;
  m_headerLength += 12;
  m_numberOfIes++;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  m_mmeUeS1apId = i.ReadNtohU32 ();
  m_ueAggregateMaxBitRateDownlink = i.ReadNtohU64 ();
  m_ueAggregateMaxBitRateUplink   = i.ReadNtohU64 ();
  int sz = i.ReadNtohU32 ();
  m_headerLength += 27;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistErabToBeSetupItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
 
      erabItem.erabLevelQosParameters = NistEpsBearer ((NistEpsBearer::Qci) i.ReadNtohU16 ());
      erabItem.erabLevelQosParameters.gbrQosInfo.gbrDl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.gbrUl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.mbrDl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.mbrUl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.arp.priorityLevel = i.ReadU8 ();
      erabItem.erabLevelQosParameters.arp.preemptionCapability = i.ReadU8 ();
      erabItem.erabLevelQosParameters.arp.preemptionVulnerability = i.ReadU8 ();

      erabItem.dlForwarding = i.ReadU8 ();
      erabItem.transportLayerAddress = Ipv4Address (i.ReadNtohU32 ());
      erabItem.gtpTeid = i.ReadNtohU32 ();

      m_erabsToBeSetupList.push_back (erabItem);
      m_headerLength += 48;
    }

  return GetSerializedSize ();
}

void
NistEpcX2HandoverRequestHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId = " << m_oldEnbUeX2apId;
  os << " Cause = " << m_cause;
  os << " TargetCellId = " << m_targetCellId;
  os << " MmeUeS1apId = " << m_mmeUeS1apId;
  os << " UeAggrMaxBitRateDownlink = " << m_ueAggregateMaxBitRateDownlink;
  os << " UeAggrMaxBitRateUplink = " << m_ueAggregateMaxBitRateUplink;
  os << " NumOfBearers = " << m_erabsToBeSetupList.size ();

  std::vector <NistEpcX2Sap::NistErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size ();
  if (sz > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz; j++)
    {
      os << m_erabsToBeSetupList[j].erabId;
      if (j < (int) sz - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }
}

uint16_t
NistEpcX2HandoverRequestHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
NistEpcX2HandoverRequestHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
NistEpcX2HandoverRequestHeader::GetCause () const
{
  return m_cause;
}

void
NistEpcX2HandoverRequestHeader::SetCause (uint16_t cause)
{
  m_cause = cause;
}

uint16_t
NistEpcX2HandoverRequestHeader::GetTargetCellId () const
{
  return m_targetCellId;
}

void
NistEpcX2HandoverRequestHeader::SetTargetCellId (uint16_t targetCellId)
{
  m_targetCellId = targetCellId;
}

uint32_t
NistEpcX2HandoverRequestHeader::GetMmeUeS1apId () const
{
  return m_mmeUeS1apId;
}

void
NistEpcX2HandoverRequestHeader::SetMmeUeS1apId (uint32_t mmeUeS1apId)
{
  m_mmeUeS1apId = mmeUeS1apId;
}

std::vector <NistEpcX2Sap::NistErabToBeSetupItem>
NistEpcX2HandoverRequestHeader::GetBearers () const
{
  return m_erabsToBeSetupList;
}

void
NistEpcX2HandoverRequestHeader::SetBearers (std::vector <NistEpcX2Sap::NistErabToBeSetupItem> bearers)
{
  m_headerLength += 48 * bearers.size ();
  m_erabsToBeSetupList = bearers;
}

uint64_t
NistEpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateDownlink () const
{
  return m_ueAggregateMaxBitRateDownlink;
}

void
NistEpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateDownlink (uint64_t bitRate)
{
  m_ueAggregateMaxBitRateDownlink = bitRate;
}

uint64_t
NistEpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateUplink () const
{
  return m_ueAggregateMaxBitRateUplink;
}

void
NistEpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateUplink (uint64_t bitRate)
{
  m_ueAggregateMaxBitRateUplink = bitRate;
}

uint32_t
NistEpcX2HandoverRequestHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2HandoverRequestHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2HandoverRequestAckHeader);

NistEpcX2HandoverRequestAckHeader::NistEpcX2HandoverRequestAckHeader ()
  : m_numberOfIes (1 + 1 + 1 + 1),
    m_headerLength (2 + 2 + 4 + 4),
    m_oldEnbUeX2apId (0xfffa),
    m_newEnbUeX2apId (0xfffa)
{
}

NistEpcX2HandoverRequestAckHeader::~NistEpcX2HandoverRequestAckHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_newEnbUeX2apId = 0xfffb;
  m_erabsAdmittedList.clear ();
  m_erabsNotAdmittedList.clear ();
}

TypeId
NistEpcX2HandoverRequestAckHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2HandoverRequestAckHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2HandoverRequestAckHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2HandoverRequestAckHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2HandoverRequestAckHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2HandoverRequestAckHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_newEnbUeX2apId);

  std::vector <NistEpcX2Sap::NistErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size (); 
  i.WriteHtonU32 (sz);
  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_erabsAdmittedList [j].erabId);
      i.WriteHtonU32 (m_erabsAdmittedList [j].ulGtpTeid);
      i.WriteHtonU32 (m_erabsAdmittedList [j].dlGtpTeid);
    }

  std::vector <NistEpcX2Sap::NistErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size (); 
  i.WriteHtonU32 (sz2);
  for (int j = 0; j < (int) sz2; j++)
    {
      i.WriteHtonU16 (m_erabsNotAdmittedList [j].erabId);
      i.WriteHtonU16 (m_erabsNotAdmittedList [j].cause);
    }
}

uint32_t
NistEpcX2HandoverRequestAckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_newEnbUeX2apId = i.ReadNtohU16 ();
  m_headerLength += 4;
  m_numberOfIes += 2;

  int sz = i.ReadNtohU32 ();
  m_headerLength += 4;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistErabAdmittedItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
      erabItem.ulGtpTeid = i.ReadNtohU32 ();
      erabItem.dlGtpTeid = i.ReadNtohU32 ();

      m_erabsAdmittedList.push_back (erabItem);
      m_headerLength += 10;
    }

  sz = i.ReadNtohU32 ();
  m_headerLength += 4;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistErabNotAdmittedItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
      erabItem.cause  = i.ReadNtohU16 ();

      m_erabsNotAdmittedList.push_back (erabItem);
      m_headerLength += 4;
    }

  return GetSerializedSize ();
}

void
NistEpcX2HandoverRequestAckHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId=" << m_oldEnbUeX2apId;
  os << " NewEnbUeX2apId=" << m_newEnbUeX2apId;

  os << " AdmittedBearers=" << m_erabsAdmittedList.size ();
  std::vector <NistEpcX2Sap::NistErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size ();
  if (sz > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz; j++)
    {
      os << m_erabsAdmittedList[j].erabId;
      if (j < (int) sz - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }
  
  os << " NotAdmittedBearers=" << m_erabsNotAdmittedList.size ();
  std::vector <NistEpcX2Sap::NistErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size ();
  if (sz2 > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz2; j++)
    {
      os << m_erabsNotAdmittedList[j].erabId;
      if (j < (int) sz2 - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }

}

uint16_t
NistEpcX2HandoverRequestAckHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
NistEpcX2HandoverRequestAckHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
NistEpcX2HandoverRequestAckHeader::GetNewEnbUeX2apId () const
{
  return m_newEnbUeX2apId;
}

void
NistEpcX2HandoverRequestAckHeader::SetNewEnbUeX2apId (uint16_t x2apId)
{
  m_newEnbUeX2apId = x2apId;
}

std::vector <NistEpcX2Sap::NistErabAdmittedItem> 
NistEpcX2HandoverRequestAckHeader::GetAdmittedBearers () const
{
  return m_erabsAdmittedList;
}

void
NistEpcX2HandoverRequestAckHeader::SetAdmittedBearers (std::vector <NistEpcX2Sap::NistErabAdmittedItem> bearers)
{
  m_headerLength += 10 * bearers.size ();
  m_erabsAdmittedList = bearers;
}

std::vector <NistEpcX2Sap::NistErabNotAdmittedItem>
NistEpcX2HandoverRequestAckHeader::GetNotAdmittedBearers () const
{
  return m_erabsNotAdmittedList;
}

void
NistEpcX2HandoverRequestAckHeader::SetNotAdmittedBearers (std::vector <NistEpcX2Sap::NistErabNotAdmittedItem> bearers)
{
  m_headerLength += 4 * bearers.size ();
  m_erabsNotAdmittedList = bearers;
}

uint32_t
NistEpcX2HandoverRequestAckHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2HandoverRequestAckHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2HandoverPreparationFailureHeader);

NistEpcX2HandoverPreparationFailureHeader::NistEpcX2HandoverPreparationFailureHeader ()
  : m_numberOfIes (1 + 1 + 1),
    m_headerLength (2 + 2 + 2),
    m_oldEnbUeX2apId (0xfffa),
    m_cause (0xfffa),
    m_criticalityDiagnostics (0xfffa)
{
}

NistEpcX2HandoverPreparationFailureHeader::~NistEpcX2HandoverPreparationFailureHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_cause = 0xfffb;
  m_criticalityDiagnostics = 0xfffb;
}

TypeId
NistEpcX2HandoverPreparationFailureHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2HandoverPreparationFailureHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2HandoverPreparationFailureHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2HandoverPreparationFailureHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2HandoverPreparationFailureHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2HandoverPreparationFailureHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_cause);
  i.WriteHtonU16 (m_criticalityDiagnostics);
}

uint32_t
NistEpcX2HandoverPreparationFailureHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_cause = i.ReadNtohU16 ();
  m_criticalityDiagnostics = i.ReadNtohU16 ();

  m_headerLength = 6;
  m_numberOfIes = 3;

  return GetSerializedSize ();
}

void
NistEpcX2HandoverPreparationFailureHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId = " << m_oldEnbUeX2apId;
  os << " Cause = " << m_cause;
  os << " CriticalityDiagnostics = " << m_criticalityDiagnostics;
}

uint16_t
NistEpcX2HandoverPreparationFailureHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
NistEpcX2HandoverPreparationFailureHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
NistEpcX2HandoverPreparationFailureHeader::GetCause () const
{
  return m_cause;
}

void
NistEpcX2HandoverPreparationFailureHeader::SetCause (uint16_t cause)
{
  m_cause = cause;
}

uint16_t
NistEpcX2HandoverPreparationFailureHeader::GetCriticalityDiagnostics () const
{
  return m_criticalityDiagnostics;
}

void
NistEpcX2HandoverPreparationFailureHeader::SetCriticalityDiagnostics (uint16_t criticalityDiagnostics)
{
  m_criticalityDiagnostics = criticalityDiagnostics;
}

uint32_t
NistEpcX2HandoverPreparationFailureHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2HandoverPreparationFailureHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2SnNistStatusTransferHeader);

NistEpcX2SnNistStatusTransferHeader::NistEpcX2SnNistStatusTransferHeader ()
  : m_numberOfIes (3),
    m_headerLength (6),
    m_oldEnbUeX2apId (0xfffa),
    m_newEnbUeX2apId (0xfffa)
{
  m_erabsSubjectToNistStatusTransferList.clear (); 
}

NistEpcX2SnNistStatusTransferHeader::~NistEpcX2SnNistStatusTransferHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_newEnbUeX2apId = 0xfffb;
  m_erabsSubjectToNistStatusTransferList.clear (); 
}

TypeId
NistEpcX2SnNistStatusTransferHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2SnNistStatusTransferHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2SnNistStatusTransferHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2SnNistStatusTransferHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2SnNistStatusTransferHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2SnNistStatusTransferHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_newEnbUeX2apId);

  std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem>::size_type sz = m_erabsSubjectToNistStatusTransferList.size ();
  i.WriteHtonU16 (sz);              // number of NistErabsSubjectToNistStatusTransferItems

  for (int j = 0; j < (int) sz; j++)
    {
      NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem item = m_erabsSubjectToNistStatusTransferList [j];

      i.WriteHtonU16 (item.erabId);

      uint16_t bitsetSize = NistEpcX2Sap::m_maxPdcpSn / 64;
      for (int k = 0; k < bitsetSize; k++)
        {
          uint64_t statusValue = 0;
          for (int m = 0; m < 64; m++)
            {
              statusValue |= item.receiveNistStatusOfUlPdcpSdus[64 * k + m] << m;
            }
          i.WriteHtonU64 (statusValue);
        }

      i.WriteHtonU16 (item.ulPdcpSn);
      i.WriteHtonU32 (item.ulHfn);
      i.WriteHtonU16 (item.dlPdcpSn);
      i.WriteHtonU32 (item.dlHfn);
    }
}

uint32_t
NistEpcX2SnNistStatusTransferHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_newEnbUeX2apId = i.ReadNtohU16 ();
  int sz = i.ReadNtohU16 ();

  m_numberOfIes = 3;
  m_headerLength = 6 + sz * (14 + (NistEpcX2Sap::m_maxPdcpSn / 64));

  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem ErabItem;
      ErabItem.erabId = i.ReadNtohU16 ();

      uint16_t bitsetSize = NistEpcX2Sap::m_maxPdcpSn / 64;
      for (int k = 0; k < bitsetSize; k++)
        {
          uint64_t statusValue = i.ReadNtohU64 ();
          for (int m = 0; m < 64; m++)
            {
              ErabItem.receiveNistStatusOfUlPdcpSdus[64 * k + m] = (statusValue >> m) & 1;
            }
        }

      ErabItem.ulPdcpSn = i.ReadNtohU16 ();
      ErabItem.ulHfn    = i.ReadNtohU32 ();
      ErabItem.dlPdcpSn = i.ReadNtohU16 ();
      ErabItem.dlHfn    = i.ReadNtohU32 ();

      m_erabsSubjectToNistStatusTransferList.push_back (ErabItem);
    }

  return GetSerializedSize ();
}

void
NistEpcX2SnNistStatusTransferHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId = " << m_oldEnbUeX2apId;
  os << " NewEnbUeX2apId = " << m_newEnbUeX2apId;
  os << " ErabsSubjectToNistStatusTransferList size = " << m_erabsSubjectToNistStatusTransferList.size ();

  std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem>::size_type sz = m_erabsSubjectToNistStatusTransferList.size ();
  if (sz > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz; j++)
    {
      os << m_erabsSubjectToNistStatusTransferList[j].erabId;
      if (j < (int) sz - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }
}

uint16_t
NistEpcX2SnNistStatusTransferHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
NistEpcX2SnNistStatusTransferHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
NistEpcX2SnNistStatusTransferHeader::GetNewEnbUeX2apId () const
{
  return m_newEnbUeX2apId;
}

void
NistEpcX2SnNistStatusTransferHeader::SetNewEnbUeX2apId (uint16_t x2apId)
{
  m_newEnbUeX2apId = x2apId;
}

std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem>
NistEpcX2SnNistStatusTransferHeader::GetErabsSubjectToNistStatusTransferList () const
{
  return m_erabsSubjectToNistStatusTransferList;
}

void
NistEpcX2SnNistStatusTransferHeader::SetErabsSubjectToNistStatusTransferList (std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem> erabs)
{
  m_headerLength += erabs.size () * (14 + (NistEpcX2Sap::m_maxPdcpSn / 8));
  m_erabsSubjectToNistStatusTransferList = erabs;
}

uint32_t
NistEpcX2SnNistStatusTransferHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2SnNistStatusTransferHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2UeContextReleaseHeader);

NistEpcX2UeContextReleaseHeader::NistEpcX2UeContextReleaseHeader ()
  : m_numberOfIes (1 + 1),
    m_headerLength (2 + 2),
    m_oldEnbUeX2apId (0xfffa),
    m_newEnbUeX2apId (0xfffa)
{
}

NistEpcX2UeContextReleaseHeader::~NistEpcX2UeContextReleaseHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_newEnbUeX2apId = 0xfffb;
}

TypeId
NistEpcX2UeContextReleaseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2UeContextReleaseHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2UeContextReleaseHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2UeContextReleaseHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2UeContextReleaseHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2UeContextReleaseHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_newEnbUeX2apId);
}

uint32_t
NistEpcX2UeContextReleaseHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_newEnbUeX2apId = i.ReadNtohU16 ();
  m_numberOfIes = 2;
  m_headerLength = 4;

  return GetSerializedSize ();
}

void
NistEpcX2UeContextReleaseHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId=" << m_oldEnbUeX2apId;
  os << " NewEnbUeX2apId=" << m_newEnbUeX2apId;
}

uint16_t
NistEpcX2UeContextReleaseHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
NistEpcX2UeContextReleaseHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
NistEpcX2UeContextReleaseHeader::GetNewEnbUeX2apId () const
{
  return m_newEnbUeX2apId;
}

void
NistEpcX2UeContextReleaseHeader::SetNewEnbUeX2apId (uint16_t x2apId)
{
  m_newEnbUeX2apId = x2apId;
}

uint32_t
NistEpcX2UeContextReleaseHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2UeContextReleaseHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2LoadInformationHeader);

NistEpcX2LoadInformationHeader::NistEpcX2LoadInformationHeader ()
  : m_numberOfIes (1),
    m_headerLength (6)
{
  m_cellInformationList.clear ();
}

NistEpcX2LoadInformationHeader::~NistEpcX2LoadInformationHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_cellInformationList.clear ();
}

TypeId
NistEpcX2LoadInformationHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2LoadInformationHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2LoadInformationHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2LoadInformationHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2LoadInformationHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2LoadInformationHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (6);               // id = CELL_INFORMATION
  i.WriteU8 (1 << 6);               // criticality = IGNORE
  i.WriteU8 (4);                    // length of CELL_INFORMATION_ID

  std::vector <NistEpcX2Sap::NistCellInformationItem>::size_type sz = m_cellInformationList.size ();
  i.WriteHtonU16 (sz);              // number of cellInformationItems

  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_cellInformationList [j].sourceCellId);

      std::vector <NistEpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
      sz2 = m_cellInformationList [j].ulInterferenceOverloadIndicationList.size ();
      i.WriteHtonU16 (sz2);         // number of UlInterferenceOverloadIndicationItem

      for (int k = 0; k < (int) sz2; k++)
        {
          i.WriteU8 (m_cellInformationList [j].ulInterferenceOverloadIndicationList [k]);
        }

      std::vector <NistEpcX2Sap::NistUlHighInterferenceInformationItem>::size_type sz3;
      sz3 = m_cellInformationList [j].ulHighInterferenceInformationList.size ();
      i.WriteHtonU16 (sz3);         // number of NistUlHighInterferenceInformationItem

      for (int k = 0; k < (int) sz3; k++)
        {
          i.WriteHtonU16 (m_cellInformationList [j].ulHighInterferenceInformationList [k].targetCellId);

          std::vector <bool>::size_type sz4;
          sz4 = m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList.size ();
          i.WriteHtonU16 (sz4);

          for (int m = 0; m < (int) sz4; m++)
            {
              i.WriteU8 (m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList [m]);
            }
        }

      std::vector <bool>::size_type sz5;
      sz5 = m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList.size ();
      i.WriteHtonU16 (sz5);

      for (int k = 0; k < (int) sz5; k++)
        {
          i.WriteU8 (m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList [k]);
        }

      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.rntpThreshold);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.antennaPorts);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.pB);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.pdcchInterferenceImpact);
    }
}

uint32_t
NistEpcX2LoadInformationHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  int sz = i.ReadNtohU16 ();
  m_headerLength += 6;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistCellInformationItem cellInfoItem;
      cellInfoItem.sourceCellId = i.ReadNtohU16 ();
      m_headerLength += 2;

      int sz2 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz2; k++)
        {
          NistEpcX2Sap::UlInterferenceOverloadIndicationItem item = (NistEpcX2Sap::UlInterferenceOverloadIndicationItem) i.ReadU8 ();
          cellInfoItem.ulInterferenceOverloadIndicationList.push_back (item);
        }
      m_headerLength += sz2;

      int sz3 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz3; k++)
        {
          NistEpcX2Sap::NistUlHighInterferenceInformationItem item;
          item.targetCellId = i.ReadNtohU16 ();
          m_headerLength += 2;

          int sz4 = i.ReadNtohU16 ();
          m_headerLength += 2;
          for (int m = 0; m < sz4; m++)
            {
              item.ulHighInterferenceIndicationList.push_back (i.ReadU8 ());
            }
          m_headerLength += sz4;

          cellInfoItem.ulHighInterferenceInformationList.push_back (item);
        }

      int sz5 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz5; k++)
        {
          cellInfoItem.relativeNarrowbandTxBand.rntpPerPrbList.push_back (i.ReadU8 ());
        }
      m_headerLength += sz5;

      cellInfoItem.relativeNarrowbandTxBand.rntpThreshold = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.antennaPorts = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.pB = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.pdcchInterferenceImpact = i.ReadNtohU16 ();
      m_headerLength += 8;

      m_cellInformationList.push_back (cellInfoItem);
    }

  return GetSerializedSize ();
}

void
NistEpcX2LoadInformationHeader::Print (std::ostream &os) const
{
  os << "NumOfNistCellInformationItems=" << m_cellInformationList.size ();
}

std::vector <NistEpcX2Sap::NistCellInformationItem>
NistEpcX2LoadInformationHeader::GetCellInformationList () const
{
  return m_cellInformationList;
}

void
NistEpcX2LoadInformationHeader::SetCellInformationList (std::vector <NistEpcX2Sap::NistCellInformationItem> cellInformationList)
{
  m_cellInformationList = cellInformationList;
  m_headerLength += 2;

  std::vector <NistEpcX2Sap::NistCellInformationItem>::size_type sz = m_cellInformationList.size ();
  for (int j = 0; j < (int) sz; j++)
    {
      m_headerLength += 2;

      std::vector <NistEpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
      sz2 = m_cellInformationList [j].ulInterferenceOverloadIndicationList.size ();
      m_headerLength += 2 + sz2;

      std::vector <NistEpcX2Sap::NistUlHighInterferenceInformationItem>::size_type sz3;
      sz3 = m_cellInformationList [j].ulHighInterferenceInformationList.size ();
      m_headerLength += 2;

      for (int k = 0; k < (int) sz3; k++)
        {
          std::vector <bool>::size_type sz4;
          sz4 = m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList.size ();
          m_headerLength += 2 + 2 + sz4;
        }

      std::vector <bool>::size_type sz5;
      sz5 = m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList.size ();
      m_headerLength += 2 + sz5 + 8;
    }
}

uint32_t
NistEpcX2LoadInformationHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2LoadInformationHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2ResourceNistStatusUpdateHeader);

NistEpcX2ResourceNistStatusUpdateHeader::NistEpcX2ResourceNistStatusUpdateHeader ()
  : m_numberOfIes (3),
    m_headerLength (6),
    m_enb1MeasurementId (0xfffa),
    m_enb2MeasurementId (0xfffa)
{
  m_cellMeasurementResultList.clear ();
}

NistEpcX2ResourceNistStatusUpdateHeader::~NistEpcX2ResourceNistStatusUpdateHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_enb1MeasurementId = 0xfffb;
  m_enb2MeasurementId = 0xfffb;
  m_cellMeasurementResultList.clear ();
}

TypeId
NistEpcX2ResourceNistStatusUpdateHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2ResourceNistStatusUpdateHeader")
    .SetParent<Header> ()
    .AddConstructor<NistEpcX2ResourceNistStatusUpdateHeader> ()
  ;
  return tid;
}

TypeId
NistEpcX2ResourceNistStatusUpdateHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistEpcX2ResourceNistStatusUpdateHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
NistEpcX2ResourceNistStatusUpdateHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_enb1MeasurementId);
  i.WriteHtonU16 (m_enb2MeasurementId);

  std::vector <NistEpcX2Sap::NistCellMeasurementResultItem>::size_type sz = m_cellMeasurementResultList.size ();
  i.WriteHtonU16 (sz);              // number of NistCellMeasurementResultItem

  for (int j = 0; j < (int) sz; j++)
    {
      NistEpcX2Sap::NistCellMeasurementResultItem item = m_cellMeasurementResultList [j];

      i.WriteHtonU16 (item.sourceCellId);
      i.WriteU8 (item.dlHardwareLoadIndicator);
      i.WriteU8 (item.ulHardwareLoadIndicator);
      i.WriteU8 (item.dlS1TnlLoadIndicator);
      i.WriteU8 (item.ulS1TnlLoadIndicator);

      i.WriteHtonU16 (item.dlGbrPrbUsage);
      i.WriteHtonU16 (item.ulGbrPrbUsage);
      i.WriteHtonU16 (item.dlNonGbrPrbUsage);
      i.WriteHtonU16 (item.ulNonGbrPrbUsage);
      i.WriteHtonU16 (item.dlTotalPrbUsage);
      i.WriteHtonU16 (item.ulTotalPrbUsage);

      i.WriteHtonU16 (item.dlCompositeAvailableCapacity.cellCapacityClassValue);
      i.WriteHtonU16 (item.dlCompositeAvailableCapacity.capacityValue);
      i.WriteHtonU16 (item.ulCompositeAvailableCapacity.cellCapacityClassValue);
      i.WriteHtonU16 (item.ulCompositeAvailableCapacity.capacityValue);
    }
}

uint32_t
NistEpcX2ResourceNistStatusUpdateHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_enb1MeasurementId = i.ReadNtohU16 ();
  m_enb2MeasurementId = i.ReadNtohU16 ();

  int sz = i.ReadNtohU16 ();
  for (int j = 0; j < sz; j++)
    {
      NistEpcX2Sap::NistCellMeasurementResultItem item;

      item.sourceCellId = i.ReadNtohU16 ();
      item.dlHardwareLoadIndicator = (NistEpcX2Sap::LoadIndicator) i.ReadU8 ();
      item.ulHardwareLoadIndicator = (NistEpcX2Sap::LoadIndicator) i.ReadU8 ();
      item.dlS1TnlLoadIndicator = (NistEpcX2Sap::LoadIndicator) i.ReadU8 ();
      item.ulS1TnlLoadIndicator = (NistEpcX2Sap::LoadIndicator) i.ReadU8 ();

      item.dlGbrPrbUsage = i.ReadNtohU16 ();
      item.ulGbrPrbUsage = i.ReadNtohU16 ();
      item.dlNonGbrPrbUsage = i.ReadNtohU16 ();
      item.ulNonGbrPrbUsage = i.ReadNtohU16 ();
      item.dlTotalPrbUsage = i.ReadNtohU16 ();
      item.ulTotalPrbUsage = i.ReadNtohU16 ();

      item.dlCompositeAvailableCapacity.cellCapacityClassValue = i.ReadNtohU16 ();
      item.dlCompositeAvailableCapacity.capacityValue = i.ReadNtohU16 ();
      item.ulCompositeAvailableCapacity.cellCapacityClassValue = i.ReadNtohU16 ();
      item.ulCompositeAvailableCapacity.capacityValue = i.ReadNtohU16 ();

      m_cellMeasurementResultList.push_back (item);
    }

  m_headerLength = 6 + sz * 26;
  m_numberOfIes = 3;

  return GetSerializedSize ();
}

void
NistEpcX2ResourceNistStatusUpdateHeader::Print (std::ostream &os) const
{
  os << "Enb1MeasurementId = " << m_enb1MeasurementId
     << " Enb2MeasurementId = " << m_enb2MeasurementId
     << " NumOfNistCellMeasurementResultItems = " << m_cellMeasurementResultList.size ();
}

uint16_t
NistEpcX2ResourceNistStatusUpdateHeader::GetEnb1MeasurementId () const
{
  return m_enb1MeasurementId;
}

void
NistEpcX2ResourceNistStatusUpdateHeader::SetEnb1MeasurementId (uint16_t enb1MeasurementId)
{
  m_enb1MeasurementId = enb1MeasurementId;
}

uint16_t
NistEpcX2ResourceNistStatusUpdateHeader::GetEnb2MeasurementId () const
{
  return m_enb2MeasurementId;
}

void
NistEpcX2ResourceNistStatusUpdateHeader::SetEnb2MeasurementId (uint16_t enb2MeasurementId)
{
  m_enb2MeasurementId = enb2MeasurementId;
}

std::vector <NistEpcX2Sap::NistCellMeasurementResultItem>
NistEpcX2ResourceNistStatusUpdateHeader::GetCellMeasurementResultList () const
{
  return m_cellMeasurementResultList;
}

void
NistEpcX2ResourceNistStatusUpdateHeader::SetCellMeasurementResultList (std::vector <NistEpcX2Sap::NistCellMeasurementResultItem> cellMeasurementResultList)
{
  m_cellMeasurementResultList = cellMeasurementResultList;

  std::vector <NistEpcX2Sap::NistCellMeasurementResultItem>::size_type sz = m_cellMeasurementResultList.size ();
  m_headerLength += sz * 26;
}

uint32_t
NistEpcX2ResourceNistStatusUpdateHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
NistEpcX2ResourceNistStatusUpdateHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

} // namespace ns3
