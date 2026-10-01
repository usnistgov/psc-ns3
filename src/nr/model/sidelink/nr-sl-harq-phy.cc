// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-harq-phy.h"

#include <ns3/assert.h>
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE("NrSlHarqPhy");

namespace ns3
{

NrSlHarqPhy::~NrSlHarqPhy()
{
    NS_LOG_FUNCTION(this);
    m_slHistory.clear();
}

const NrErrorModel::NrErrorModelHistory&
NrSlHarqPhy::GetHarqProcessInfoSl(uint16_t rnti, uint8_t harqProcId)
{
    return GetHarqProcessInfo(&m_slHistory, rnti, harqProcId);
}

void
NrSlHarqPhy::UpdateSlDataHarqProcessStatus(uint16_t rnti,
                                           uint8_t harqProcId,
                                           const Ptr<NrErrorModelOutput>& output)
{
    NS_LOG_FUNCTION(this);
    UpdateHarqProcessStatus(&m_slHistory, rnti, harqProcId, output);
}

void
NrSlHarqPhy::ResetSlDataHarqProcessStatus(uint16_t rnti, uint8_t id)
{
    NS_LOG_FUNCTION(this);
    ResetHarqProcessStatus(&m_slHistory, rnti, id);
}

void
NrSlHarqPhy::IndicatePrevDecoded(uint16_t rnti, uint8_t harqId)
{
    NS_LOG_FUNCTION(this << rnti << static_cast<uint16_t>(harqId));

    uint32_t decodedId = (rnti << 8) + harqId;

    if (m_slDecodedTb.find(decodedId) == m_slDecodedTb.end())
    {
        m_slDecodedTb.insert(decodedId);
    }
}

bool
NrSlHarqPhy::IsPrevDecoded(uint16_t rnti, uint8_t harqId)
{
    NS_LOG_FUNCTION(this << rnti << static_cast<uint16_t>(harqId));

    uint32_t decodedId = (rnti << 8) + harqId;

    std::unordered_set<uint32_t>::iterator it = m_slDecodedTb.find(decodedId);

    bool prevDecoded = (it != m_slDecodedTb.end());

    return prevDecoded;
}

void
NrSlHarqPhy::RemovePrevDecoded(uint16_t rnti, uint8_t harqId)
{
    NS_LOG_FUNCTION(this << rnti << static_cast<uint16_t>(harqId));

    uint32_t decodedId = (rnti << 8) + harqId;

    std::unordered_set<uint32_t>::iterator it = m_slDecodedTb.find(decodedId);

    if (it != m_slDecodedTb.end())
    {
        m_slDecodedTb.erase(it);
    }
}

} // namespace ns3
