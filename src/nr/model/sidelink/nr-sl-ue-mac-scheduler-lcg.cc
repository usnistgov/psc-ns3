// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-ue-mac-scheduler-lcg.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeMacSchedulerLCG");

NrSlUeMacSchedulerLC::NrSlUeMacSchedulerLC(
    const struct NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& conf)
{
    NS_LOG_FUNCTION(this << +conf.lcId << +conf.pqi << +conf.priority << conf.isGbr << conf.mbr
                         << conf.gbr);
    m_id = conf.lcId;
    m_pqi = conf.pqi;
    m_priority = conf.priority;
    m_isGbr = conf.isGbr;
    m_mbr = conf.mbr;
    m_gbr = conf.gbr;
    m_harqEnabled = conf.harqEnabled;
    m_pdb = conf.pdb;
    m_dynamic = conf.dynamic;
    m_rri = conf.rri;
    m_castType = conf.castType;
}

int
NrSlUeMacSchedulerLC::UpdateLC(
    const struct NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this << +params.lcid);
    NS_ASSERT(params.lcid == m_id);

    int ret = 0;

    ret += params.txQueueSize - m_txQueueSize;
    ret += params.retxQueueSize - m_retxQueueSize;
    ret += params.statusPduSize - m_statusPduSize;

    m_txQueueSize = params.txQueueSize;
    m_txQueueHolDelay = params.txQueueHolDelay;
    m_retxQueueSize = params.retxQueueSize;
    m_retxQueueHolDelay = params.retxQueueHolDelay;
    m_statusPduSize = params.statusPduSize;

    return ret;
}

uint32_t
NrSlUeMacSchedulerLC::GetTotalQueueSize() const
{
    return m_txQueueSize + m_retxQueueSize + m_statusPduSize;
}

// NrSlUeMacSchedulerLCG

NrSlUeMacSchedulerLCG::NrSlUeMacSchedulerLCG(uint8_t id)
{
    NS_LOG_FUNCTION(this << +id);
    m_id = id;
}

bool
NrSlUeMacSchedulerLCG::Contains(uint8_t lcId) const
{
    return m_lcMap.find(lcId) != m_lcMap.end();
}

uint32_t
NrSlUeMacSchedulerLCG::GetNumOfLC() const
{
    return static_cast<uint32_t>(m_lcMap.size());
}

void
NrSlUeMacSchedulerLCG::Insert(NrSlLCPtr&& lc)
{
    NS_LOG_FUNCTION(this << +lc->m_id);
    std::pair<NrSlLCIt, bool> ret;
    ret = m_lcMap.emplace(lc->m_id, std::move(lc));
    bool insertStatus = ret.second;
    NS_ASSERT_MSG(insertStatus,
                  "LCG " << +m_id << " already contains LCID " << +ret.first->second->m_id);
}

void
NrSlUeMacSchedulerLCG::Remove(uint8_t lcid)
{
    if (Contains(lcid))
    {
        m_lcMap.erase(lcid);
    }
    else
    {
        NS_LOG_INFO("LCID " << lcid << " doesn't belong to LCGID " << m_id);
    }
}

void
NrSlUeMacSchedulerLCG::UpdateInfo(
    const NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this << +params.lcid);
    NS_ASSERT(Contains(params.lcid));
    int ret = m_lcMap.at(params.lcid)->UpdateLC(params);
    if (ret < 0)
    {
        NS_ASSERT_MSG(m_totalSize >= static_cast<uint32_t>(std::abs(ret)),
                      "totSize: " << m_totalSize << " ret: " << ret);
    }
    m_totalSize += ret;
}

uint32_t
NrSlUeMacSchedulerLCG::GetTotalSize() const
{
    return m_totalSize;
}

uint32_t
NrSlUeMacSchedulerLCG::GetTotalSizeOfLC(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->GetTotalQueueSize();
}

std::vector<uint8_t>
NrSlUeMacSchedulerLCG::GetLCId() const
{
    std::vector<uint8_t> ret;
    ret.reserve(m_lcMap.size());
    for (const auto& lc : m_lcMap)
    {
        ret.emplace_back(lc.first);
    }
    return ret;
}

uint8_t
NrSlUeMacSchedulerLCG::GetLcPqi(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_pqi;
}

uint8_t
NrSlUeMacSchedulerLCG::GetLcPriority(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_priority;
}

bool
NrSlUeMacSchedulerLCG::IsLcGbr(uint16_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_isGbr;
}

bool
NrSlUeMacSchedulerLCG::IsHarqEnabled(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_harqEnabled;
}

uint64_t
NrSlUeMacSchedulerLCG::GetLcMbr(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_mbr;
}

uint64_t
NrSlUeMacSchedulerLCG::GetLcGbr(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_gbr;
}

bool
NrSlUeMacSchedulerLCG::IsLcDynamic(uint16_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_dynamic;
}

bool
NrSlUeMacSchedulerLCG::IsLcHarqEnabled(uint16_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_harqEnabled;
}

Time
NrSlUeMacSchedulerLCG::GetLcRri(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_rri;
}

Time
NrSlUeMacSchedulerLCG::GetLcPdb(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_pdb;
}

SidelinkInfo::CastType
NrSlUeMacSchedulerLCG::GetLcCastType(uint8_t lcId) const
{
    NS_ASSERT(Contains(lcId));
    return m_lcMap.at(lcId)->m_castType;
}

void
NrSlUeMacSchedulerLCG::AssignedData(uint8_t lcId, uint32_t size)
{
    NS_LOG_FUNCTION(this << +lcId << size);
    NS_ASSERT(m_lcMap.size() > 0);

    // Update queues: RLC tx order Status, ReTx, Tx. To understand this, you have
    // to see RlcAm::NotifyTxOpportunity
    NS_LOG_INFO("Status of LCID " << static_cast<uint32_t>(lcId)
                                  << ": RLC STATUS PDU size =" << m_lcMap.at(lcId)->m_statusPduSize
                                  << ", RLC Retr queue size =" << m_lcMap.at(lcId)->m_retxQueueSize
                                  << ", RLC TX queue size =" << m_lcMap.at(lcId)->m_txQueueSize);

    if ((m_lcMap.at(lcId)->m_statusPduSize > 0) && (size >= m_lcMap.at(lcId)->m_statusPduSize))
    {
        // Update status queue
        m_lcMap.at(lcId)->m_statusPduSize = 0;
    }
    else if ((m_lcMap.at(lcId)->m_retxQueueSize > 0) && (size >= m_lcMap.at(lcId)->m_retxQueueSize))
    {
        // update retx queue
        m_lcMap.at(lcId)->m_retxQueueSize = 0;
    }
    else if (m_lcMap.at(lcId)->m_txQueueSize > 0)
    {
        if (m_lcMap.at(lcId)->m_txQueueSize <= size)
        {
            m_lcMap.at(lcId)->m_txQueueSize = 0;
        }
        else
        {
            NS_ASSERT(m_lcMap.at(lcId)->m_txQueueSize > size);
            m_lcMap.at(lcId)->m_txQueueSize -= size;
        }
    }

    if (m_totalSize >= size)
    {
        m_totalSize -= size;
    }
    else
    {
        m_totalSize = 0;
    }
}

} // namespace ns3
