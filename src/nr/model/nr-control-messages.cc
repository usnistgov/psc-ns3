// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-control-messages.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrControlMessage");

NrControlMessage::NrControlMessage()
{
    NS_LOG_FUNCTION(this);
}

NrControlMessage::~NrControlMessage()
{
    NS_LOG_FUNCTION(this);
}

void
NrControlMessage::SetMessageType(messageType type)
{
    m_messageType = type;
}

NrControlMessage::messageType
NrControlMessage::GetMessageType() const
{
    return m_messageType;
}

void
NrControlMessage::SetSourceBwp(uint16_t bwpId)
{
    m_bwpId = bwpId;
}

uint16_t
NrControlMessage::GetSourceBwp() const
{
    NS_ABORT_IF(m_bwpId < 0);
    return static_cast<uint16_t>(m_bwpId);
}

NrSRMessage::NrSRMessage()
{
    NS_LOG_FUNCTION(this);
    SetMessageType(NrControlMessage::SR);
}

NrSRMessage::~NrSRMessage()
{
    NS_LOG_FUNCTION(this);
}

void
NrSRMessage::SetRNTI(uint16_t rnti)
{
    m_rnti = rnti;
}

uint16_t
NrSRMessage::GetRNTI() const
{
    return m_rnti;
}

NrDlDciMessage::NrDlDciMessage(const std::shared_ptr<DciInfoElementTdma>& dci)
    : m_dciInfoElement(dci)
{
    NS_LOG_FUNCTION(this);
    SetMessageType(NrControlMessage::DL_DCI);
}

NrDlDciMessage::~NrDlDciMessage()
{
    NS_LOG_FUNCTION(this);
}

std::shared_ptr<DciInfoElementTdma>
NrDlDciMessage::GetDciInfoElement()
{
    return m_dciInfoElement;
}

void
NrDlDciMessage::SetKDelay(uint32_t delay)
{
    m_k = delay;
}

void
NrDlDciMessage::SetK1Delay(uint32_t delay)
{
    m_k1 = delay;
}

uint32_t
NrDlDciMessage::GetKDelay() const
{
    return m_k;
}

uint32_t
NrDlDciMessage::GetK1Delay() const
{
    return m_k1;
}

NrUlDciMessage::NrUlDciMessage(const std::shared_ptr<DciInfoElementTdma>& dci)
    : m_dciInfoElement(dci)
{
    NS_LOG_FUNCTION(this);
    SetMessageType(NrControlMessage::UL_DCI);
}

NrUlDciMessage::~NrUlDciMessage()
{
    NS_LOG_FUNCTION(this);
}

std::shared_ptr<DciInfoElementTdma>
NrUlDciMessage::GetDciInfoElement()
{
    return m_dciInfoElement;
}

void
NrUlDciMessage::SetKDelay(uint32_t delay)
{
    m_k = delay;
}

uint32_t
NrUlDciMessage::GetKDelay() const
{
    return m_k;
}

NrDlCqiMessage::NrDlCqiMessage()
{
    NS_LOG_FUNCTION(this);
    SetMessageType(NrControlMessage::DL_CQI);
}

NrDlCqiMessage::~NrDlCqiMessage()
{
    NS_LOG_FUNCTION(this);
}

void
NrDlCqiMessage::SetDlCqi(DlCqiInfo cqi)
{
    m_cqi = cqi;
}

DlCqiInfo
NrDlCqiMessage::GetDlCqi()
{
    return m_cqi;
}

// ----------------------------------------------------------------------------------------------------------

NrBsrMessage::NrBsrMessage()
{
    SetMessageType(NrControlMessage::BSR);
}

NrBsrMessage::~NrBsrMessage()
{
}

void
NrBsrMessage::SetBsr(MacCeElement bsr)
{
    m_bsr = bsr;
}

MacCeElement
NrBsrMessage::GetBsr()
{
    return m_bsr;
}

// ----------------------------------------------------------------------------------------------------------

NrMibMessage::NrMibMessage()
{
    SetMessageType(NrControlMessage::MIB);
}

void
NrMibMessage::SetMib(NrRrcSap::MasterInformationBlock mib)
{
    m_mib = mib;
}

NrRrcSap::MasterInformationBlock
NrMibMessage::GetMib() const
{
    return m_mib;
}

// ----------------------------------------------------------------------------------------------------------

NrSib1Message::NrSib1Message()
{
    SetMessageType(NrControlMessage::SIB1);
}

void
NrSib1Message::SetSib1(NrRrcSap::SystemInformationBlockType1 sib1)
{
    m_sib1 = sib1;
}

NrRrcSap::SystemInformationBlockType1
NrSib1Message::GetSib1() const
{
    return m_sib1;
}

// ----------------------------------------------------------------------------------------------------------

NrRachPreambleMessage::NrRachPreambleMessage()
{
    SetMessageType(NrControlMessage::RACH_PREAMBLE);
}

NrRachPreambleMessage::~NrRachPreambleMessage()
{
}

void
NrRachPreambleMessage::SetRapId(uint32_t rapId)
{
    m_rapId = rapId;
}

uint32_t
NrRachPreambleMessage::GetRapId() const
{
    return m_rapId;
}

// ----------------------------------------------------------------------------------------------------------

NrRarMessage::NrRarMessage()
{
    SetMessageType(NrControlMessage::RAR);
}

NrRarMessage::~NrRarMessage()
{
}

void
NrRarMessage::SetRaRnti(uint16_t raRnti)
{
    m_raRnti = raRnti;
}

uint16_t
NrRarMessage::GetRaRnti() const
{
    return m_raRnti;
}

void
NrRarMessage::AddRar(Rar rar)
{
    m_rarList.push_back(rar);
}

std::list<NrRarMessage::Rar>::const_iterator
NrRarMessage::RarListBegin() const
{
    return m_rarList.begin();
}

std::list<NrRarMessage::Rar>::const_iterator
NrRarMessage::RarListEnd() const
{
    return m_rarList.end();
}

NrDlHarqFeedbackMessage::NrDlHarqFeedbackMessage()
{
    SetMessageType(NrControlMessage::DL_HARQ);
}

NrDlHarqFeedbackMessage::~NrDlHarqFeedbackMessage()
{
}

void
NrDlHarqFeedbackMessage::SetDlHarqFeedback(DlHarqInfo m)
{
    m_dlHarqInfo = m;
}

DlHarqInfo
NrDlHarqFeedbackMessage::GetDlHarqFeedback()
{
    return m_dlHarqInfo;
}

NrSrsMessage::NrSrsMessage()
{
    SetMessageType(NrControlMessage::SRS);
}

std::ostream&
operator<<(std::ostream& os, const LteNrTddSlotType& item)
{
    switch (item)
    {
    case LteNrTddSlotType::DL:
        os << "DL";
        break;
    case LteNrTddSlotType::F:
        os << "F";
        break;
    case LteNrTddSlotType::S:
        os << "S";
        break;
    case LteNrTddSlotType::UL:
        os << "UL";
        break;
    }
    return os;
}

} // namespace ns3
