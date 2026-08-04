// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-epc-x2-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcX2Header");

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2Header);

NrEpcX2Header::NrEpcX2Header()
    : m_messageType(0xfa),
      m_procedureCode(0xfa),
      m_lengthOfIes(0xfa),
      m_numberOfIes(0xfa)
{
}

NrEpcX2Header::~NrEpcX2Header()
{
    m_messageType = 0xfb;
    m_procedureCode = 0xfb;
    m_lengthOfIes = 0xfb;
    m_numberOfIes = 0xfb;
}

TypeId
NrEpcX2Header::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2Header")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2Header>();
    return tid;
}

TypeId
NrEpcX2Header::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2Header::GetSerializedSize() const
{
    return 7;
}

void
NrEpcX2Header::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteU8(m_messageType);
    i.WriteU8(m_procedureCode);

    i.WriteU8(0x00); // criticality = REJECT
    i.WriteU8(m_lengthOfIes + 3);
    i.WriteHtonU16(0);
    i.WriteU8(m_numberOfIes);
}

uint32_t
NrEpcX2Header::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_messageType = i.ReadU8();
    m_procedureCode = i.ReadU8();

    i.ReadU8();
    m_lengthOfIes = i.ReadU8() - 3;
    i.ReadNtohU16();
    m_numberOfIes = i.ReadU8();

    return GetSerializedSize();
}

void
NrEpcX2Header::Print(std::ostream& os) const
{
    os << "MessageType=" << (uint32_t)m_messageType;
    os << " ProcedureCode=" << (uint32_t)m_procedureCode;
    os << " LengthOfIEs=" << (uint32_t)m_lengthOfIes;
    os << " NumberOfIEs=" << (uint32_t)m_numberOfIes;
}

uint8_t
NrEpcX2Header::GetMessageType() const
{
    return m_messageType;
}

void
NrEpcX2Header::SetMessageType(uint8_t messageType)
{
    m_messageType = messageType;
}

uint8_t
NrEpcX2Header::GetProcedureCode() const
{
    return m_procedureCode;
}

void
NrEpcX2Header::SetProcedureCode(uint8_t procedureCode)
{
    m_procedureCode = procedureCode;
}

void
NrEpcX2Header::SetLengthOfIes(uint32_t lengthOfIes)
{
    m_lengthOfIes = lengthOfIes;
}

void
NrEpcX2Header::SetNumberOfIes(uint32_t numberOfIes)
{
    m_numberOfIes = numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2HandoverRequestHeader);

NrEpcX2HandoverRequestHeader::NrEpcX2HandoverRequestHeader()
    : m_numberOfIes(1 + 1 + 1 + 1),
      m_headerLength(6 + 5 + 12 + (3 + 4 + 8 + 8 + 4)),
      m_oldGnbUeX2apId(0xfffa),
      m_cause(0xfffa),
      m_targetCellId(0xfffa),
      m_mmeUeS1apId(0xfffffffa)
{
    m_erabsToBeSetupList.clear();
}

NrEpcX2HandoverRequestHeader::~NrEpcX2HandoverRequestHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_cause = 0xfffb;
    m_targetCellId = 0xfffb;
    m_mmeUeS1apId = 0xfffffffb;
    m_erabsToBeSetupList.clear();
}

TypeId
NrEpcX2HandoverRequestHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2HandoverRequestHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2HandoverRequestHeader>();
    return tid;
}

TypeId
NrEpcX2HandoverRequestHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2HandoverRequestHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2HandoverRequestHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(10); // id = OLD_GNB_UE_X2AP_ID
    i.WriteU8(0);       // criticality = REJECT
    i.WriteU8(2);       // length of OLD_GNB_UE_X2AP_ID
    i.WriteHtonU16(m_oldGnbUeX2apId);

    i.WriteHtonU16(5); // id = CAUSE
    i.WriteU8(1 << 6); // criticality = IGNORE
    i.WriteU8(1);      // length of CAUSE
    i.WriteU8(m_cause);

    i.WriteHtonU16(11);       // id = TARGET_CELLID
    i.WriteU8(0);             // criticality = REJECT
    i.WriteU8(8);             // length of TARGET_CELLID
    i.WriteHtonU32(0x123456); // fake PLMN
    i.WriteHtonU32(m_targetCellId << 4);

    i.WriteHtonU16(14); // id = UE_CONTEXT_INFORMATION
    i.WriteU8(0);       // criticality = REJECT

    i.WriteHtonU32(m_mmeUeS1apId);
    i.WriteHtonU64(m_ueAggregateMaxBitRateDownlink);
    i.WriteHtonU64(m_ueAggregateMaxBitRateUplink);

    std::vector<NrEpcX2Sap::ErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size();
    i.WriteHtonU32(sz); // number of bearers
    for (int j = 0; j < (int)sz; j++)
    {
        i.WriteHtonU16(m_erabsToBeSetupList[j].erabId);
        i.WriteHtonU16(m_erabsToBeSetupList[j].erabLevelQosParameters.qci);
        i.WriteHtonU64(m_erabsToBeSetupList[j].erabLevelQosParameters.gbrQosInfo.gbrDl);
        i.WriteHtonU64(m_erabsToBeSetupList[j].erabLevelQosParameters.gbrQosInfo.gbrUl);
        i.WriteHtonU64(m_erabsToBeSetupList[j].erabLevelQosParameters.gbrQosInfo.mbrDl);
        i.WriteHtonU64(m_erabsToBeSetupList[j].erabLevelQosParameters.gbrQosInfo.mbrUl);
        i.WriteU8(m_erabsToBeSetupList[j].erabLevelQosParameters.arp.priorityLevel);
        i.WriteU8(m_erabsToBeSetupList[j].erabLevelQosParameters.arp.preemptionCapability);
        i.WriteU8(m_erabsToBeSetupList[j].erabLevelQosParameters.arp.preemptionVulnerability);
        i.WriteU8(m_erabsToBeSetupList[j].dlForwarding);
        i.WriteHtonU32(m_erabsToBeSetupList[j].transportLayerAddress.Get());
        i.WriteHtonU32(m_erabsToBeSetupList[j].gtpTeid);
    }
}

uint32_t
NrEpcX2HandoverRequestHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_headerLength = 0;
    m_numberOfIes = 0;

    i.ReadNtohU16();
    i.ReadU8();
    i.ReadU8();
    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_headerLength += 6;
    m_numberOfIes++;

    i.ReadNtohU16();
    i.ReadU8();
    i.ReadU8();
    m_cause = i.ReadU8();
    m_headerLength += 5;
    m_numberOfIes++;

    i.ReadNtohU16();
    i.ReadU8();
    i.ReadU8();
    i.ReadNtohU32();
    m_targetCellId = i.ReadNtohU32() >> 4;
    m_headerLength += 12;
    m_numberOfIes++;

    i.ReadNtohU16();
    i.ReadU8();
    m_mmeUeS1apId = i.ReadNtohU32();
    m_ueAggregateMaxBitRateDownlink = i.ReadNtohU64();
    m_ueAggregateMaxBitRateUplink = i.ReadNtohU64();
    int sz = i.ReadNtohU32();
    m_headerLength += 27;
    m_numberOfIes++;

    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::ErabToBeSetupItem erabItem;

        erabItem.erabId = i.ReadNtohU16();

        erabItem.erabLevelQosParameters = NrEpsBearer((NrEpsBearer::Qci)i.ReadNtohU16());
        erabItem.erabLevelQosParameters.gbrQosInfo.gbrDl = i.ReadNtohU64();
        erabItem.erabLevelQosParameters.gbrQosInfo.gbrUl = i.ReadNtohU64();
        erabItem.erabLevelQosParameters.gbrQosInfo.mbrDl = i.ReadNtohU64();
        erabItem.erabLevelQosParameters.gbrQosInfo.mbrUl = i.ReadNtohU64();
        erabItem.erabLevelQosParameters.arp.priorityLevel = i.ReadU8();
        erabItem.erabLevelQosParameters.arp.preemptionCapability = i.ReadU8();
        erabItem.erabLevelQosParameters.arp.preemptionVulnerability = i.ReadU8();

        erabItem.dlForwarding = i.ReadU8();
        erabItem.transportLayerAddress = Ipv4Address(i.ReadNtohU32());
        erabItem.gtpTeid = i.ReadNtohU32();

        m_erabsToBeSetupList.push_back(erabItem);
        m_headerLength += 48;
    }

    return GetSerializedSize();
}

void
NrEpcX2HandoverRequestHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId = " << m_oldGnbUeX2apId;
    os << " Cause = " << m_cause;
    os << " TargetCellId = " << m_targetCellId;
    os << " MmeUeS1apId = " << m_mmeUeS1apId;
    os << " UeAggrMaxBitRateDownlink = " << m_ueAggregateMaxBitRateDownlink;
    os << " UeAggrMaxBitRateUplink = " << m_ueAggregateMaxBitRateUplink;
    os << " NumOfBearers = " << m_erabsToBeSetupList.size();

    std::vector<NrEpcX2Sap::ErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size();
    if (sz > 0)
    {
        os << " [";
    }
    for (int j = 0; j < (int)sz; j++)
    {
        os << m_erabsToBeSetupList[j].erabId;
        if (j < (int)sz - 1)
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
NrEpcX2HandoverRequestHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2HandoverRequestHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2HandoverRequestHeader::GetCause() const
{
    return m_cause;
}

void
NrEpcX2HandoverRequestHeader::SetCause(uint16_t cause)
{
    m_cause = cause;
}

uint16_t
NrEpcX2HandoverRequestHeader::GetTargetCellId() const
{
    return m_targetCellId;
}

void
NrEpcX2HandoverRequestHeader::SetTargetCellId(uint16_t targetCellId)
{
    m_targetCellId = targetCellId;
}

uint32_t
NrEpcX2HandoverRequestHeader::GetMmeUeS1apId() const
{
    return m_mmeUeS1apId;
}

void
NrEpcX2HandoverRequestHeader::SetMmeUeS1apId(uint32_t mmeUeS1apId)
{
    m_mmeUeS1apId = mmeUeS1apId;
}

std::vector<NrEpcX2Sap::ErabToBeSetupItem>
NrEpcX2HandoverRequestHeader::GetBearers() const
{
    return m_erabsToBeSetupList;
}

void
NrEpcX2HandoverRequestHeader::SetBearers(std::vector<NrEpcX2Sap::ErabToBeSetupItem> bearers)
{
    m_headerLength += 48 * bearers.size();
    m_erabsToBeSetupList = bearers;
}

uint64_t
NrEpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateDownlink() const
{
    return m_ueAggregateMaxBitRateDownlink;
}

void
NrEpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateDownlink(uint64_t bitRate)
{
    m_ueAggregateMaxBitRateDownlink = bitRate;
}

uint64_t
NrEpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateUplink() const
{
    return m_ueAggregateMaxBitRateUplink;
}

void
NrEpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateUplink(uint64_t bitRate)
{
    m_ueAggregateMaxBitRateUplink = bitRate;
}

uint32_t
NrEpcX2HandoverRequestHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2HandoverRequestHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2HandoverRequestAckHeader);

NrEpcX2HandoverRequestAckHeader::NrEpcX2HandoverRequestAckHeader()
    : m_numberOfIes(1 + 1 + 1 + 1),
      m_headerLength(2 + 2 + 4 + 4),
      m_oldGnbUeX2apId(0xfffa),
      m_newGnbUeX2apId(0xfffa)
{
}

NrEpcX2HandoverRequestAckHeader::~NrEpcX2HandoverRequestAckHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_newGnbUeX2apId = 0xfffb;
    m_erabsAdmittedList.clear();
    m_erabsNotAdmittedList.clear();
}

TypeId
NrEpcX2HandoverRequestAckHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2HandoverRequestAckHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2HandoverRequestAckHeader>();
    return tid;
}

TypeId
NrEpcX2HandoverRequestAckHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2HandoverRequestAckHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2HandoverRequestAckHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_oldGnbUeX2apId);
    i.WriteHtonU16(m_newGnbUeX2apId);

    std::vector<NrEpcX2Sap::ErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size();
    i.WriteHtonU32(sz);
    for (int j = 0; j < (int)sz; j++)
    {
        i.WriteHtonU16(m_erabsAdmittedList[j].erabId);
        i.WriteHtonU32(m_erabsAdmittedList[j].ulGtpTeid);
        i.WriteHtonU32(m_erabsAdmittedList[j].dlGtpTeid);
    }

    std::vector<NrEpcX2Sap::ErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size();
    i.WriteHtonU32(sz2);
    for (int j = 0; j < (int)sz2; j++)
    {
        i.WriteHtonU16(m_erabsNotAdmittedList[j].erabId);
        i.WriteHtonU16(m_erabsNotAdmittedList[j].cause);
    }
}

uint32_t
NrEpcX2HandoverRequestAckHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_headerLength = 0;
    m_numberOfIes = 0;

    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_newGnbUeX2apId = i.ReadNtohU16();
    m_headerLength += 4;
    m_numberOfIes += 2;

    int sz = i.ReadNtohU32();
    m_headerLength += 4;
    m_numberOfIes++;

    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::ErabAdmittedItem erabItem;

        erabItem.erabId = i.ReadNtohU16();
        erabItem.ulGtpTeid = i.ReadNtohU32();
        erabItem.dlGtpTeid = i.ReadNtohU32();

        m_erabsAdmittedList.push_back(erabItem);
        m_headerLength += 10;
    }

    sz = i.ReadNtohU32();
    m_headerLength += 4;
    m_numberOfIes++;

    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::ErabNotAdmittedItem erabItem;

        erabItem.erabId = i.ReadNtohU16();
        erabItem.cause = i.ReadNtohU16();

        m_erabsNotAdmittedList.push_back(erabItem);
        m_headerLength += 4;
    }

    return GetSerializedSize();
}

void
NrEpcX2HandoverRequestAckHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId=" << m_oldGnbUeX2apId;
    os << " NewGnbUeX2apId=" << m_newGnbUeX2apId;

    os << " AdmittedBearers=" << m_erabsAdmittedList.size();
    std::vector<NrEpcX2Sap::ErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size();
    if (sz > 0)
    {
        os << " [";
    }
    for (int j = 0; j < (int)sz; j++)
    {
        os << m_erabsAdmittedList[j].erabId;
        if (j < (int)sz - 1)
        {
            os << ", ";
        }
        else
        {
            os << "]";
        }
    }

    os << " NotAdmittedBearers=" << m_erabsNotAdmittedList.size();
    std::vector<NrEpcX2Sap::ErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size();
    if (sz2 > 0)
    {
        os << " [";
    }
    for (int j = 0; j < (int)sz2; j++)
    {
        os << m_erabsNotAdmittedList[j].erabId;
        if (j < (int)sz2 - 1)
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
NrEpcX2HandoverRequestAckHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2HandoverRequestAckHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2HandoverRequestAckHeader::GetNewGnbUeX2apId() const
{
    return m_newGnbUeX2apId;
}

void
NrEpcX2HandoverRequestAckHeader::SetNewGnbUeX2apId(uint16_t x2apId)
{
    m_newGnbUeX2apId = x2apId;
}

std::vector<NrEpcX2Sap::ErabAdmittedItem>
NrEpcX2HandoverRequestAckHeader::GetAdmittedBearers() const
{
    return m_erabsAdmittedList;
}

void
NrEpcX2HandoverRequestAckHeader::SetAdmittedBearers(
    std::vector<NrEpcX2Sap::ErabAdmittedItem> bearers)
{
    m_headerLength += 10 * bearers.size();
    m_erabsAdmittedList = bearers;
}

std::vector<NrEpcX2Sap::ErabNotAdmittedItem>
NrEpcX2HandoverRequestAckHeader::GetNotAdmittedBearers() const
{
    return m_erabsNotAdmittedList;
}

void
NrEpcX2HandoverRequestAckHeader::SetNotAdmittedBearers(
    std::vector<NrEpcX2Sap::ErabNotAdmittedItem> bearers)
{
    m_headerLength += 4 * bearers.size();
    m_erabsNotAdmittedList = bearers;
}

uint32_t
NrEpcX2HandoverRequestAckHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2HandoverRequestAckHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2HandoverPreparationFailureHeader);

NrEpcX2HandoverPreparationFailureHeader::NrEpcX2HandoverPreparationFailureHeader()
    : m_numberOfIes(1 + 1 + 1),
      m_headerLength(2 + 2 + 2),
      m_oldGnbUeX2apId(0xfffa),
      m_cause(0xfffa),
      m_criticalityDiagnostics(0xfffa)
{
}

NrEpcX2HandoverPreparationFailureHeader::~NrEpcX2HandoverPreparationFailureHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_cause = 0xfffb;
    m_criticalityDiagnostics = 0xfffb;
}

TypeId
NrEpcX2HandoverPreparationFailureHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2HandoverPreparationFailureHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2HandoverPreparationFailureHeader>();
    return tid;
}

TypeId
NrEpcX2HandoverPreparationFailureHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2HandoverPreparationFailureHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2HandoverPreparationFailureHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_oldGnbUeX2apId);
    i.WriteHtonU16(m_cause);
    i.WriteHtonU16(m_criticalityDiagnostics);
}

uint32_t
NrEpcX2HandoverPreparationFailureHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_cause = i.ReadNtohU16();
    m_criticalityDiagnostics = i.ReadNtohU16();

    m_headerLength = 6;
    m_numberOfIes = 3;

    return GetSerializedSize();
}

void
NrEpcX2HandoverPreparationFailureHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId = " << m_oldGnbUeX2apId;
    os << " Cause = " << m_cause;
    os << " CriticalityDiagnostics = " << m_criticalityDiagnostics;
}

uint16_t
NrEpcX2HandoverPreparationFailureHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2HandoverPreparationFailureHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2HandoverPreparationFailureHeader::GetCause() const
{
    return m_cause;
}

void
NrEpcX2HandoverPreparationFailureHeader::SetCause(uint16_t cause)
{
    m_cause = cause;
}

uint16_t
NrEpcX2HandoverPreparationFailureHeader::GetCriticalityDiagnostics() const
{
    return m_criticalityDiagnostics;
}

void
NrEpcX2HandoverPreparationFailureHeader::SetCriticalityDiagnostics(uint16_t criticalityDiagnostics)
{
    m_criticalityDiagnostics = criticalityDiagnostics;
}

uint32_t
NrEpcX2HandoverPreparationFailureHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2HandoverPreparationFailureHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2SnStatusTransferHeader);

NrEpcX2SnStatusTransferHeader::NrEpcX2SnStatusTransferHeader()
    : m_numberOfIes(3),
      m_headerLength(6),
      m_oldGnbUeX2apId(0xfffa),
      m_newGnbUeX2apId(0xfffa)
{
    m_erabsSubjectToStatusTransferList.clear();
}

NrEpcX2SnStatusTransferHeader::~NrEpcX2SnStatusTransferHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_newGnbUeX2apId = 0xfffb;
    m_erabsSubjectToStatusTransferList.clear();
}

TypeId
NrEpcX2SnStatusTransferHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2SnStatusTransferHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2SnStatusTransferHeader>();
    return tid;
}

TypeId
NrEpcX2SnStatusTransferHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2SnStatusTransferHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2SnStatusTransferHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_oldGnbUeX2apId);
    i.WriteHtonU16(m_newGnbUeX2apId);

    std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem>::size_type sz =
        m_erabsSubjectToStatusTransferList.size();
    i.WriteHtonU16(sz); // number of ErabsSubjectToStatusTransferItems

    for (int j = 0; j < (int)sz; j++)
    {
        NrEpcX2Sap::ErabsSubjectToStatusTransferItem item = m_erabsSubjectToStatusTransferList[j];

        i.WriteHtonU16(item.erabId);

        uint16_t bitsetSize = NrEpcX2Sap::m_maxPdcpSn / 64;
        for (int k = 0; k < bitsetSize; k++)
        {
            uint64_t statusValue = 0;
            for (int m = 0; m < 64; m++)
            {
                statusValue |= item.receiveStatusOfUlPdcpSdus[64 * k + m] << m;
            }
            i.WriteHtonU64(statusValue);
        }

        i.WriteHtonU16(item.ulPdcpSn);
        i.WriteHtonU32(item.ulHfn);
        i.WriteHtonU16(item.dlPdcpSn);
        i.WriteHtonU32(item.dlHfn);
    }
}

uint32_t
NrEpcX2SnStatusTransferHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_newGnbUeX2apId = i.ReadNtohU16();
    int sz = i.ReadNtohU16();

    m_numberOfIes = 3;
    m_headerLength = 6 + sz * (14 + (NrEpcX2Sap::m_maxPdcpSn / 64));

    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::ErabsSubjectToStatusTransferItem ErabItem;
        ErabItem.erabId = i.ReadNtohU16();

        uint16_t bitsetSize = NrEpcX2Sap::m_maxPdcpSn / 64;
        for (int k = 0; k < bitsetSize; k++)
        {
            uint64_t statusValue = i.ReadNtohU64();
            for (int m = 0; m < 64; m++)
            {
                ErabItem.receiveStatusOfUlPdcpSdus[64 * k + m] = (statusValue >> m) & 1;
            }
        }

        ErabItem.ulPdcpSn = i.ReadNtohU16();
        ErabItem.ulHfn = i.ReadNtohU32();
        ErabItem.dlPdcpSn = i.ReadNtohU16();
        ErabItem.dlHfn = i.ReadNtohU32();

        m_erabsSubjectToStatusTransferList.push_back(ErabItem);
    }

    return GetSerializedSize();
}

void
NrEpcX2SnStatusTransferHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId = " << m_oldGnbUeX2apId;
    os << " NewGnbUeX2apId = " << m_newGnbUeX2apId;
    os << " ErabsSubjectToStatusTransferList size = " << m_erabsSubjectToStatusTransferList.size();

    std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem>::size_type sz =
        m_erabsSubjectToStatusTransferList.size();
    if (sz > 0)
    {
        os << " [";
    }
    for (int j = 0; j < (int)sz; j++)
    {
        os << m_erabsSubjectToStatusTransferList[j].erabId;
        if (j < (int)sz - 1)
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
NrEpcX2SnStatusTransferHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2SnStatusTransferHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2SnStatusTransferHeader::GetNewGnbUeX2apId() const
{
    return m_newGnbUeX2apId;
}

void
NrEpcX2SnStatusTransferHeader::SetNewGnbUeX2apId(uint16_t x2apId)
{
    m_newGnbUeX2apId = x2apId;
}

std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem>
NrEpcX2SnStatusTransferHeader::GetErabsSubjectToStatusTransferList() const
{
    return m_erabsSubjectToStatusTransferList;
}

void
NrEpcX2SnStatusTransferHeader::SetErabsSubjectToStatusTransferList(
    std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem> erabs)
{
    m_headerLength += erabs.size() * (14 + (NrEpcX2Sap::m_maxPdcpSn / 8));
    m_erabsSubjectToStatusTransferList = erabs;
}

uint32_t
NrEpcX2SnStatusTransferHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2SnStatusTransferHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2UeContextReleaseHeader);

NrEpcX2UeContextReleaseHeader::NrEpcX2UeContextReleaseHeader()
    : m_numberOfIes(1 + 1),
      m_headerLength(2 + 2),
      m_oldGnbUeX2apId(0xfffa),
      m_newGnbUeX2apId(0xfffa)
{
}

NrEpcX2UeContextReleaseHeader::~NrEpcX2UeContextReleaseHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_newGnbUeX2apId = 0xfffb;
}

TypeId
NrEpcX2UeContextReleaseHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2UeContextReleaseHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2UeContextReleaseHeader>();
    return tid;
}

TypeId
NrEpcX2UeContextReleaseHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2UeContextReleaseHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2UeContextReleaseHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_oldGnbUeX2apId);
    i.WriteHtonU16(m_newGnbUeX2apId);
}

uint32_t
NrEpcX2UeContextReleaseHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_newGnbUeX2apId = i.ReadNtohU16();
    m_numberOfIes = 2;
    m_headerLength = 4;

    return GetSerializedSize();
}

void
NrEpcX2UeContextReleaseHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId=" << m_oldGnbUeX2apId;
    os << " NewGnbUeX2apId=" << m_newGnbUeX2apId;
}

uint16_t
NrEpcX2UeContextReleaseHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2UeContextReleaseHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2UeContextReleaseHeader::GetNewGnbUeX2apId() const
{
    return m_newGnbUeX2apId;
}

void
NrEpcX2UeContextReleaseHeader::SetNewGnbUeX2apId(uint16_t x2apId)
{
    m_newGnbUeX2apId = x2apId;
}

uint32_t
NrEpcX2UeContextReleaseHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2UeContextReleaseHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

/////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2LoadInformationHeader);

NrEpcX2LoadInformationHeader::NrEpcX2LoadInformationHeader()
    : m_numberOfIes(1),
      m_headerLength(6)
{
    m_cellInformationList.clear();
}

NrEpcX2LoadInformationHeader::~NrEpcX2LoadInformationHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_cellInformationList.clear();
}

TypeId
NrEpcX2LoadInformationHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2LoadInformationHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2LoadInformationHeader>();
    return tid;
}

TypeId
NrEpcX2LoadInformationHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2LoadInformationHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2LoadInformationHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(6); // id = CELL_INFORMATION
    i.WriteU8(1 << 6); // criticality = IGNORE
    i.WriteU8(4);      // length of CELL_INFORMATION_ID

    std::vector<NrEpcX2Sap::CellInformationItem>::size_type sz = m_cellInformationList.size();
    i.WriteHtonU16(sz); // number of cellInformationItems

    for (int j = 0; j < (int)sz; j++)
    {
        i.WriteHtonU16(m_cellInformationList[j].sourceCellId);

        std::vector<NrEpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
        sz2 = m_cellInformationList[j].ulInterferenceOverloadIndicationList.size();
        i.WriteHtonU16(sz2); // number of UlInterferenceOverloadIndicationItem

        for (int k = 0; k < (int)sz2; k++)
        {
            i.WriteU8(m_cellInformationList[j].ulInterferenceOverloadIndicationList[k]);
        }

        std::vector<NrEpcX2Sap::UlHighInterferenceInformationItem>::size_type sz3;
        sz3 = m_cellInformationList[j].ulHighInterferenceInformationList.size();
        i.WriteHtonU16(sz3); // number of UlHighInterferenceInformationItem

        for (int k = 0; k < (int)sz3; k++)
        {
            i.WriteHtonU16(
                m_cellInformationList[j].ulHighInterferenceInformationList[k].targetCellId);

            std::vector<bool>::size_type sz4;
            sz4 = m_cellInformationList[j]
                      .ulHighInterferenceInformationList[k]
                      .ulHighInterferenceIndicationList.size();
            i.WriteHtonU16(sz4);

            for (int m = 0; m < (int)sz4; m++)
            {
                i.WriteU8(m_cellInformationList[j]
                              .ulHighInterferenceInformationList[k]
                              .ulHighInterferenceIndicationList[m]);
            }
        }

        std::vector<bool>::size_type sz5;
        sz5 = m_cellInformationList[j].relativeNarrowbandTxBand.rntpPerPrbList.size();
        i.WriteHtonU16(sz5);

        for (int k = 0; k < (int)sz5; k++)
        {
            i.WriteU8(m_cellInformationList[j].relativeNarrowbandTxBand.rntpPerPrbList[k]);
        }

        i.WriteHtonU16(m_cellInformationList[j].relativeNarrowbandTxBand.rntpThreshold);
        i.WriteHtonU16(m_cellInformationList[j].relativeNarrowbandTxBand.antennaPorts);
        i.WriteHtonU16(m_cellInformationList[j].relativeNarrowbandTxBand.pB);
        i.WriteHtonU16(m_cellInformationList[j].relativeNarrowbandTxBand.pdcchInterferenceImpact);
    }
}

uint32_t
NrEpcX2LoadInformationHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_headerLength = 0;
    m_numberOfIes = 0;

    i.ReadNtohU16();
    i.ReadU8();
    i.ReadU8();
    int sz = i.ReadNtohU16();
    m_headerLength += 6;
    m_numberOfIes++;

    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::CellInformationItem cellInfoItem;
        cellInfoItem.sourceCellId = i.ReadNtohU16();
        m_headerLength += 2;

        int sz2 = i.ReadNtohU16();
        m_headerLength += 2;
        for (int k = 0; k < sz2; k++)
        {
            auto item = (NrEpcX2Sap::UlInterferenceOverloadIndicationItem)i.ReadU8();
            cellInfoItem.ulInterferenceOverloadIndicationList.push_back(item);
        }
        m_headerLength += sz2;

        int sz3 = i.ReadNtohU16();
        m_headerLength += 2;
        for (int k = 0; k < sz3; k++)
        {
            NrEpcX2Sap::UlHighInterferenceInformationItem item;
            item.targetCellId = i.ReadNtohU16();
            m_headerLength += 2;

            int sz4 = i.ReadNtohU16();
            m_headerLength += 2;
            for (int m = 0; m < sz4; m++)
            {
                item.ulHighInterferenceIndicationList.push_back(i.ReadU8());
            }
            m_headerLength += sz4;

            cellInfoItem.ulHighInterferenceInformationList.push_back(item);
        }

        int sz5 = i.ReadNtohU16();
        m_headerLength += 2;
        for (int k = 0; k < sz5; k++)
        {
            cellInfoItem.relativeNarrowbandTxBand.rntpPerPrbList.push_back(i.ReadU8());
        }
        m_headerLength += sz5;

        cellInfoItem.relativeNarrowbandTxBand.rntpThreshold = i.ReadNtohU16();
        cellInfoItem.relativeNarrowbandTxBand.antennaPorts = i.ReadNtohU16();
        cellInfoItem.relativeNarrowbandTxBand.pB = i.ReadNtohU16();
        cellInfoItem.relativeNarrowbandTxBand.pdcchInterferenceImpact = i.ReadNtohU16();
        m_headerLength += 8;

        m_cellInformationList.push_back(cellInfoItem);
    }

    return GetSerializedSize();
}

void
NrEpcX2LoadInformationHeader::Print(std::ostream& os) const
{
    os << "NumOfCellInformationItems=" << m_cellInformationList.size();
}

std::vector<NrEpcX2Sap::CellInformationItem>
NrEpcX2LoadInformationHeader::GetCellInformationList() const
{
    return m_cellInformationList;
}

void
NrEpcX2LoadInformationHeader::SetCellInformationList(
    std::vector<NrEpcX2Sap::CellInformationItem> cellInformationList)
{
    m_cellInformationList = cellInformationList;
    m_headerLength += 2;

    std::vector<NrEpcX2Sap::CellInformationItem>::size_type sz = m_cellInformationList.size();
    for (int j = 0; j < (int)sz; j++)
    {
        m_headerLength += 2;

        std::vector<NrEpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
        sz2 = m_cellInformationList[j].ulInterferenceOverloadIndicationList.size();
        m_headerLength += 2 + sz2;

        std::vector<NrEpcX2Sap::UlHighInterferenceInformationItem>::size_type sz3;
        sz3 = m_cellInformationList[j].ulHighInterferenceInformationList.size();
        m_headerLength += 2;

        for (int k = 0; k < (int)sz3; k++)
        {
            std::vector<bool>::size_type sz4;
            sz4 = m_cellInformationList[j]
                      .ulHighInterferenceInformationList[k]
                      .ulHighInterferenceIndicationList.size();
            m_headerLength += 2 + 2 + sz4;
        }

        std::vector<bool>::size_type sz5;
        sz5 = m_cellInformationList[j].relativeNarrowbandTxBand.rntpPerPrbList.size();
        m_headerLength += 2 + sz5 + 8;
    }
}

uint32_t
NrEpcX2LoadInformationHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2LoadInformationHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2ResourceStatusUpdateHeader);

NrEpcX2ResourceStatusUpdateHeader::NrEpcX2ResourceStatusUpdateHeader()
    : m_numberOfIes(3),
      m_headerLength(6),
      m_gnb1MeasurementId(0xfffa),
      m_gnb2MeasurementId(0xfffa)
{
    m_cellMeasurementResultList.clear();
}

NrEpcX2ResourceStatusUpdateHeader::~NrEpcX2ResourceStatusUpdateHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_gnb1MeasurementId = 0xfffb;
    m_gnb2MeasurementId = 0xfffb;
    m_cellMeasurementResultList.clear();
}

TypeId
NrEpcX2ResourceStatusUpdateHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2ResourceStatusUpdateHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2ResourceStatusUpdateHeader>();
    return tid;
}

TypeId
NrEpcX2ResourceStatusUpdateHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2ResourceStatusUpdateHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2ResourceStatusUpdateHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_gnb1MeasurementId);
    i.WriteHtonU16(m_gnb2MeasurementId);

    std::vector<NrEpcX2Sap::CellMeasurementResultItem>::size_type sz =
        m_cellMeasurementResultList.size();
    i.WriteHtonU16(sz); // number of CellMeasurementResultItem

    for (int j = 0; j < (int)sz; j++)
    {
        NrEpcX2Sap::CellMeasurementResultItem item = m_cellMeasurementResultList[j];

        i.WriteHtonU16(item.sourceCellId);
        i.WriteU8(item.dlHardwareLoadIndicator);
        i.WriteU8(item.ulHardwareLoadIndicator);
        i.WriteU8(item.dlS1TnlLoadIndicator);
        i.WriteU8(item.ulS1TnlLoadIndicator);

        i.WriteHtonU16(item.dlGbrPrbUsage);
        i.WriteHtonU16(item.ulGbrPrbUsage);
        i.WriteHtonU16(item.dlNonGbrPrbUsage);
        i.WriteHtonU16(item.ulNonGbrPrbUsage);
        i.WriteHtonU16(item.dlTotalPrbUsage);
        i.WriteHtonU16(item.ulTotalPrbUsage);

        i.WriteHtonU16(item.dlCompositeAvailableCapacity.cellCapacityClassValue);
        i.WriteHtonU16(item.dlCompositeAvailableCapacity.capacityValue);
        i.WriteHtonU16(item.ulCompositeAvailableCapacity.cellCapacityClassValue);
        i.WriteHtonU16(item.ulCompositeAvailableCapacity.capacityValue);
    }
}

uint32_t
NrEpcX2ResourceStatusUpdateHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_gnb1MeasurementId = i.ReadNtohU16();
    m_gnb2MeasurementId = i.ReadNtohU16();

    int sz = i.ReadNtohU16();
    for (int j = 0; j < sz; j++)
    {
        NrEpcX2Sap::CellMeasurementResultItem item;

        item.sourceCellId = i.ReadNtohU16();
        item.dlHardwareLoadIndicator = (NrEpcX2Sap::LoadIndicator)i.ReadU8();
        item.ulHardwareLoadIndicator = (NrEpcX2Sap::LoadIndicator)i.ReadU8();
        item.dlS1TnlLoadIndicator = (NrEpcX2Sap::LoadIndicator)i.ReadU8();
        item.ulS1TnlLoadIndicator = (NrEpcX2Sap::LoadIndicator)i.ReadU8();

        item.dlGbrPrbUsage = i.ReadNtohU16();
        item.ulGbrPrbUsage = i.ReadNtohU16();
        item.dlNonGbrPrbUsage = i.ReadNtohU16();
        item.ulNonGbrPrbUsage = i.ReadNtohU16();
        item.dlTotalPrbUsage = i.ReadNtohU16();
        item.ulTotalPrbUsage = i.ReadNtohU16();

        item.dlCompositeAvailableCapacity.cellCapacityClassValue = i.ReadNtohU16();
        item.dlCompositeAvailableCapacity.capacityValue = i.ReadNtohU16();
        item.ulCompositeAvailableCapacity.cellCapacityClassValue = i.ReadNtohU16();
        item.ulCompositeAvailableCapacity.capacityValue = i.ReadNtohU16();

        m_cellMeasurementResultList.push_back(item);
    }

    m_headerLength = 6 + sz * 26;
    m_numberOfIes = 3;

    return GetSerializedSize();
}

void
NrEpcX2ResourceStatusUpdateHeader::Print(std::ostream& os) const
{
    os << "Gnb1MeasurementId = " << m_gnb1MeasurementId
       << " Gnb2MeasurementId = " << m_gnb2MeasurementId
       << " NumOfCellMeasurementResultItems = " << m_cellMeasurementResultList.size();
}

uint16_t
NrEpcX2ResourceStatusUpdateHeader::GetGnb1MeasurementId() const
{
    return m_gnb1MeasurementId;
}

void
NrEpcX2ResourceStatusUpdateHeader::SetGnb1MeasurementId(uint16_t gnb1MeasurementId)
{
    m_gnb1MeasurementId = gnb1MeasurementId;
}

uint16_t
NrEpcX2ResourceStatusUpdateHeader::GetGnb2MeasurementId() const
{
    return m_gnb2MeasurementId;
}

void
NrEpcX2ResourceStatusUpdateHeader::SetGnb2MeasurementId(uint16_t gnb2MeasurementId)
{
    m_gnb2MeasurementId = gnb2MeasurementId;
}

std::vector<NrEpcX2Sap::CellMeasurementResultItem>
NrEpcX2ResourceStatusUpdateHeader::GetCellMeasurementResultList() const
{
    return m_cellMeasurementResultList;
}

void
NrEpcX2ResourceStatusUpdateHeader::SetCellMeasurementResultList(
    std::vector<NrEpcX2Sap::CellMeasurementResultItem> cellMeasurementResultList)
{
    m_cellMeasurementResultList = cellMeasurementResultList;

    std::vector<NrEpcX2Sap::CellMeasurementResultItem>::size_type sz =
        m_cellMeasurementResultList.size();
    m_headerLength += sz * 26;
}

uint32_t
NrEpcX2ResourceStatusUpdateHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2ResourceStatusUpdateHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

///////////////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2HandoverCancelHeader);

NrEpcX2HandoverCancelHeader::NrEpcX2HandoverCancelHeader()
    : m_numberOfIes(3),
      m_headerLength(6),
      m_oldGnbUeX2apId(0xfffa),
      m_newGnbUeX2apId(0xfffa),
      m_cause(0xfffa)
{
}

NrEpcX2HandoverCancelHeader::~NrEpcX2HandoverCancelHeader()
{
    m_numberOfIes = 0;
    m_headerLength = 0;
    m_oldGnbUeX2apId = 0xfffb;
    m_newGnbUeX2apId = 0xfffb;
    m_cause = 0xfffb;
}

TypeId
NrEpcX2HandoverCancelHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2HandoverCancelHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcX2HandoverCancelHeader>();
    return tid;
}

TypeId
NrEpcX2HandoverCancelHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrEpcX2HandoverCancelHeader::GetSerializedSize() const
{
    return m_headerLength;
}

void
NrEpcX2HandoverCancelHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteHtonU16(m_oldGnbUeX2apId);
    i.WriteHtonU16(m_newGnbUeX2apId);
    i.WriteHtonU16(m_cause);
}

uint32_t
NrEpcX2HandoverCancelHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    m_oldGnbUeX2apId = i.ReadNtohU16();
    m_newGnbUeX2apId = i.ReadNtohU16();
    m_cause = i.ReadNtohU16();
    m_numberOfIes = 3;
    m_headerLength = 6;

    return GetSerializedSize();
}

void
NrEpcX2HandoverCancelHeader::Print(std::ostream& os) const
{
    os << "OldGnbUeX2apId=" << m_oldGnbUeX2apId;
    os << " NewGnbUeX2apId=" << m_newGnbUeX2apId;
    os << " Cause = " << m_cause;
}

uint16_t
NrEpcX2HandoverCancelHeader::GetOldGnbUeX2apId() const
{
    return m_oldGnbUeX2apId;
}

void
NrEpcX2HandoverCancelHeader::SetOldGnbUeX2apId(uint16_t x2apId)
{
    m_oldGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2HandoverCancelHeader::GetNewGnbUeX2apId() const
{
    return m_newGnbUeX2apId;
}

void
NrEpcX2HandoverCancelHeader::SetNewGnbUeX2apId(uint16_t x2apId)
{
    m_newGnbUeX2apId = x2apId;
}

uint16_t
NrEpcX2HandoverCancelHeader::GetCause() const
{
    return m_cause;
}

void
NrEpcX2HandoverCancelHeader::SetCause(uint16_t cause)
{
    m_cause = cause;
}

uint32_t
NrEpcX2HandoverCancelHeader::GetLengthOfIes() const
{
    return m_headerLength;
}

uint32_t
NrEpcX2HandoverCancelHeader::GetNumberOfIes() const
{
    return m_numberOfIes;
}

} // namespace ns3
