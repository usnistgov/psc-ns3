// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-lcg.h"

#include "nr-eps-bearer.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerLCG");

NrMacSchedulerLC::NrMacSchedulerLC(const nr::LogicalChannelConfigListElement_s& conf)
    : m_id(conf.m_logicalChannelIdentity)
{
    NrEpsBearer bearer(static_cast<NrEpsBearer::Qci>(conf.m_qci));

    m_delayBudget = MilliSeconds(bearer.GetPacketDelayBudgetMs());
    m_resourceType = bearer.GetResourceType();
    m_PER = bearer.GetPacketErrorLossRate();
    m_qci = conf.m_qci;
    m_priority = bearer.GetPriority();
    m_eRabGuaranteedBitrateDl = conf.m_eRabGuaranteedBitrateDl;
}

void
NrMacSchedulerLC::Update(const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& params)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(params.m_logicalChannelIdentity == m_id);
    m_rlcTransmissionQueueSize = params.m_rlcTransmissionQueueSize;
    m_rlcRetransmissionQueueSize = params.m_rlcRetransmissionQueueSize;
    m_rlcStatusPduSize = params.m_rlcStatusPduSize;
    m_rlcRetransmissionHolDelay = params.m_rlcRetransmissionHolDelay;
    m_rlcTransmissionQueueHolDelay = params.m_rlcTransmissionQueueHolDelay;
}

uint32_t
NrMacSchedulerLC::GetTotalSize() const
{
    return m_rlcTransmissionQueueSize + m_rlcRetransmissionQueueSize + m_rlcStatusPduSize;
}

////////////////////////////////////////////////////////////////////////////////
// NrMacSchedulerLCG

NrMacSchedulerLCG::NrMacSchedulerLCG(uint8_t id)
    : m_id(id)
{
    NS_LOG_FUNCTION(this);
    (void)m_id;
}

bool
NrMacSchedulerLCG::Contains(uint8_t lcId) const
{
    NS_LOG_FUNCTION(this);
    return m_lcMap.find(lcId) != m_lcMap.end();
}

uint32_t
NrMacSchedulerLCG::NumOfLC() const
{
    NS_LOG_FUNCTION(this);
    return static_cast<uint32_t>(m_lcMap.size());
}

bool
NrMacSchedulerLCG::Insert(LCPtr&& lc)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(!Contains(lc->m_id));
    return m_lcMap.emplace(lc->m_id, std::move(lc)).second;
}

void
NrMacSchedulerLCG::UpdateInfo(const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& params)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(Contains(params.m_logicalChannelIdentity));
    m_lcMap.at(params.m_logicalChannelIdentity)->Update(params);
}

void
NrMacSchedulerLCG::UpdateInfo(uint32_t lcgQueueSize)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_IF(m_lcMap.size() > 1);
    uint32_t lcIdPart = lcgQueueSize / m_lcMap.size();

    for (auto& lc : m_lcMap)
    {
        lc.second->m_rlcTransmissionQueueSize = lcIdPart;
    }
}

uint32_t
NrMacSchedulerLCG::GetTotalSize() const
{
    NS_LOG_FUNCTION(this);
    uint32_t totalSize = 0;
    for (const auto& lc : m_lcMap)
    {
        totalSize += lc.second->GetTotalSize();
    }
    NS_LOG_INFO("Total size: " << totalSize);
    return totalSize;
}

uint32_t
NrMacSchedulerLCG::GetTotalSizeOfLC(uint8_t lcId) const
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_IF(m_lcMap.empty());
    return m_lcMap.at(lcId)->GetTotalSize();
}

std::vector<uint8_t>
NrMacSchedulerLCG::GetLCId() const
{
    std::vector<uint8_t> ret;
    ret.reserve(m_lcMap.size());
    for (const auto& lc : m_lcMap)
    {
        ret.emplace_back(lc.first);
    }
    return ret;
}

std::vector<uint8_t>
NrMacSchedulerLCG::GetActiveLCIds() const
{
    NS_LOG_FUNCTION(this);
    std::vector<uint8_t> ret;
    for (const auto& lc : m_lcMap)
    {
        if (GetTotalSizeOfLC(lc.first) > 0)
        {
            ret.emplace_back(lc.first);
        }
    }
    return ret;
}

uint8_t
NrMacSchedulerLCG::GetQci(uint8_t lcId) const
{
    NS_LOG_FUNCTION(this);
    return m_lcMap.at(lcId)->m_qci;
}

std::unique_ptr<NrMacSchedulerLC>&
NrMacSchedulerLCG::GetLC(uint8_t lcId)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT(!m_lcMap.empty());
    NS_ASSERT(GetTotalSizeOfLC(lcId) > 0);

    return m_lcMap.at(lcId);
}

void
NrMacSchedulerLCG::AssignedData(uint8_t lcId, uint32_t size, std::string type)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(!m_lcMap.empty());

    NS_LOG_INFO("Assigning " << size << " bytes to lcId: " << +lcId);
    // Update queues: RLC tx order Status, ReTx, Tx. To understand this, you have
    // to see RlcAm::NotifyTxOpportunity
    NS_LOG_INFO("Status of LCID " << static_cast<uint32_t>(lcId)
                                  << " before: RLC PDU =" << m_lcMap.at(lcId)->m_rlcStatusPduSize
                                  << ", RLC RX=" << m_lcMap.at(lcId)->m_rlcRetransmissionQueueSize
                                  << ", RLC TX=" << m_lcMap.at(lcId)->m_rlcTransmissionQueueSize);

    if ((m_lcMap.at(lcId)->m_rlcStatusPduSize > 0) &&
        (size >= m_lcMap.at(lcId)->m_rlcStatusPduSize))
    {
        m_lcMap.at(lcId)->m_rlcStatusPduSize = 0;
    }
    else if ((m_lcMap.at(lcId)->m_rlcRetransmissionQueueSize > 0) &&
             (size >= m_lcMap.at(lcId)->m_rlcRetransmissionQueueSize))
    {
        m_lcMap.at(lcId)->m_rlcRetransmissionQueueSize = 0;
    }
    else if (m_lcMap.at(lcId)->m_rlcTransmissionQueueSize >
             0) // if not enough size for retransmission use if for transmission if there is any
                // data to be transmitted
    {
        uint32_t rlcOverhead = 0;
        // The following logic of selecting the overhead is
        // inherited from the LTE module scheduler API
        if (lcId == 1 && type == "DL")
        {
            // for SRB1 (using RLC AM) it's better to
            // overestimate RLC overhead rather than
            // underestimate it and risk unneeded
            // segmentation which increases delay
            rlcOverhead = 4;
        }
        else
        {
            // minimum RLC overhead due to header
            rlcOverhead = 2;
        }

        if (size - rlcOverhead >= m_lcMap.at(lcId)->m_rlcTransmissionQueueSize)
        {
            // we can transmit everything from the queue, reset it
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize = 0;
        }
        else
        {
            // not enough to empty all queue, but send what you can, this is normal situation to
            // happen
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize -= size - rlcOverhead;
        }

        // If there are 5 bytes the RLC TX queue info at MAC, MAC will assign 5 bytes Tx
        // opportunity, but NrRlcAm will complain that the minimum TX opportunity should be at
        // least 7 bytes. To be sure that the MAC scheduler will assign at least 7 bytes (so that 5
        // bytes can be transmitted), we tell here to MAC that there are 7 bytes in the queue
        // instead of e.g. 5 bytes. Yeah, this is a workaround, because MAC and RLC have to be "on
        // the same page". We however should take into account the next UL SHORT_BSR (we add 5
        // bytes, because in the current TX opportunity 5 bytes is being spent on SHORT_BSR).

        if (type == "UL" && m_lcMap.at(lcId)->m_rlcTransmissionQueueSize > 0 &&
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize < 12)
        {
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize = 12;
        }

        // in order to take into account the MAC header of 3 bytes
        // 10 -3 = 7 which is the minimum allowed TX opportunity by RLC AM
        if (type == "DL" && m_lcMap.at(lcId)->m_rlcTransmissionQueueSize > 0 &&
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize < 10)
        {
            m_lcMap.at(lcId)->m_rlcTransmissionQueueSize = 10;
        }
    }
    else
    {
        NS_LOG_WARN(" This opportunity cannot be used, not enough bytes to perform retransmission "
                    "or not active flows.");
    }

    NS_LOG_INFO("Status of LCID " << static_cast<uint32_t>(lcId)
                                  << " after: RLC PDU=" << m_lcMap.at(lcId)->m_rlcStatusPduSize
                                  << ", RLC RX=" << m_lcMap.at(lcId)->m_rlcRetransmissionQueueSize
                                  << ", RLC TX=" << m_lcMap.at(lcId)->m_rlcTransmissionQueueSize);
}

void
NrMacSchedulerLCG::ReleaseLC(uint8_t lcId)
{
    m_lcMap.erase(lcId);
}

} // namespace ns3
