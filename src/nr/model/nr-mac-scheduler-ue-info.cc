// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ue-info.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerUeInfo");

NrMacSchedulerUeInfo::NrMacSchedulerUeInfo(uint16_t rnti, BeamId beamId, const GetRbPerRbgFn& fn)
    : m_rnti(rnti),
      m_beamId(beamId),
      m_getNumRbPerRbg(fn)
{
}

NrMacSchedulerUeInfo::~NrMacSchedulerUeInfo()
{
}

uint32_t&
NrMacSchedulerUeInfo::GetDlRBG(const UePtr& ue)
{
    return ue->m_dlRBG;
}

uint32_t&
NrMacSchedulerUeInfo::GetUlRBG(const UePtr& ue)
{
    return ue->m_ulRBG;
}

uint8_t&
NrMacSchedulerUeInfo::GetDlSym(const UePtr& ue)
{
    return ue->m_dlSym;
}

uint8_t&
NrMacSchedulerUeInfo::GetUlSym(const UePtr& ue)
{
    return ue->m_ulSym;
}

uint8_t&
NrMacSchedulerUeInfo::GetDlMcs(const UePtr& ue)
{
    return ue->m_dlMcs;
}

uint8_t&
NrMacSchedulerUeInfo::GetUlMcs(const UePtr& ue)
{
    return ue->m_ulMcs;
}

uint32_t&
NrMacSchedulerUeInfo::GetDlTBS(const UePtr& ue)
{
    return ue->m_dlTbSize;
}

std::unordered_map<uint8_t, LCGPtr>&
NrMacSchedulerUeInfo::GetDlLCG(const UePtr& ue)
{
    return ue->m_dlLCG;
}

std::unordered_map<uint8_t, LCGPtr>&
NrMacSchedulerUeInfo::GetUlLCG(const UePtr& ue)
{
    return ue->m_ulLCG;
}

NrMacHarqVector&
NrMacSchedulerUeInfo::GetDlHarqVector(const UePtr& ue)
{
    return ue->m_dlHarq;
}

NrMacHarqVector&
NrMacSchedulerUeInfo::GetUlHarqVector(const UePtr& ue)
{
    return ue->m_ulHarq;
}

void
NrMacSchedulerUeInfo::PrintLcInfo(uint16_t ue,
                                  uint8_t lcgId,
                                  uint8_t lcId,
                                  uint8_t qci,
                                  uint8_t P,
                                  uint8_t minP)
{
    NS_LOG_DEBUG("UE " << ue << " LCG ID: " << static_cast<uint32_t>(lcgId) << " LC ID "
                       << static_cast<uint32_t>(lcId) << " QCI: " << static_cast<uint32_t>(qci)
                       << " P: " << static_cast<uint32_t>(P) << " minP: " << +minP);
}

void
NrMacSchedulerUeInfo::ResetDlSchedInfo()
{
    m_dlMRBRetx = 0;
    m_dlRBG = 0;
    m_dlSym = 0;
    m_dlTbSize = 0;
}

void
NrMacSchedulerUeInfo::ResetUlSchedInfo()
{
    m_ulMRBRetx = 0;
    m_ulRBG = 0;
    m_ulSym = 0;
    m_ulTbSize = 0;
}

void
NrMacSchedulerUeInfo::UpdateDlMetric(const Ptr<const NrAmc>& amc)
{
    if (m_dlRBG == 0)
    {
        m_dlTbSize = 0;
    }
    else
    {
        m_dlTbSize = amc->CalculateTbSize(m_dlMcs, m_dlRank, m_dlRBG * GetNumRbPerRbg());
    }
}

void
NrMacSchedulerUeInfo::ResetDlMetric()
{
    m_dlTbSize = 0;
}

void
NrMacSchedulerUeInfo::UpdateUlMetric(const Ptr<const NrAmc>& amc)
{
    if (m_ulRBG == 0)
    {
        m_ulTbSize = 0;
    }
    else
    {
        m_ulTbSize = amc->CalculateTbSize(m_ulMcs, m_ulRank, m_ulRBG * GetNumRbPerRbg());
    }
}

void
NrMacSchedulerUeInfo::ResetUlMetric()
{
    m_ulTbSize = 0;
}

uint32_t
NrMacSchedulerUeInfo::GetTotalDlBuffer() const
{
    uint32_t totBuffer = 0;
    for (const auto& lcgInfo : m_dlLCG)
    {
        const auto& lcg = lcgInfo.second;
        totBuffer += lcg->GetTotalSize();
    }
    return totBuffer;
}

uint32_t
NrMacSchedulerUeInfo::GetNumRbPerRbg() const
{
    return m_getNumRbPerRbg();
}

void
NrMacSchedulerUeInfo::ReleaseLC(uint8_t lcid)
{
    for (auto lcgIt = m_dlLCG.begin(); lcgIt != m_dlLCG.end(); lcgIt++)
    {
        lcgIt->second->ReleaseLC(lcid);
    }
    for (auto lcgIt = m_ulLCG.begin(); lcgIt != m_ulLCG.end(); lcgIt++)
    {
        lcgIt->second->ReleaseLC(lcid);
    }
    auto it = m_dlLCG.begin();
    while (it != m_dlLCG.end())
    {
        if (it->second->GetLCId().empty())
        {
            m_dlLCG.erase(it);
            it = m_dlLCG.begin();
        }
        else
        {
            it++;
        }
    }
    it = m_ulLCG.begin();
    while (it != m_ulLCG.end())
    {
        if (it->second->GetLCId().empty())
        {
            m_ulLCG.erase(it);
            it = m_ulLCG.begin();
        }
        else
        {
            it++;
        }
    }
}

} // namespace ns3
