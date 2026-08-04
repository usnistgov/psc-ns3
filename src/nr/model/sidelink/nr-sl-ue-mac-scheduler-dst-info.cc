// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-ue-mac-scheduler-dst-info.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeMacSchedulerDstInfo");

NrSlUeMacSchedulerDstInfo::NrSlUeMacSchedulerDstInfo(uint32_t dstL2Id)
    : m_dstL2Id(dstL2Id)
{
}

NrSlUeMacSchedulerDstInfo::~NrSlUeMacSchedulerDstInfo()
{
}

std::map<uint8_t, NrSlLCGPtr>&
NrSlUeMacSchedulerDstInfo::GetNrSlLCG()
{
    return m_nrSlLCG;
}

NrSlLCGIt
NrSlUeMacSchedulerDstInfo::Insert(NrSlLCGPtr&& lcg)
{
    auto ret = m_nrSlLCG.emplace(lcg->m_id, std::move(lcg));
    bool insertStatus = ret.second;
    NS_ASSERT_MSG(insertStatus,
                  "Destination " << m_dstL2Id << " already contains LCG ID "
                                 << +ret.first->second->m_id);
    return ret.first;
}

void
NrSlUeMacSchedulerDstInfo::Remove(uint8_t lcgid)
{
    std::map<uint8_t, NrSlLCGPtr>::iterator it = m_nrSlLCG.find(lcgid);
    NS_ASSERT_MSG(it != m_nrSlLCG.end(),
                  "Can't find LCG ID " << lcgid << " for destination " << m_dstL2Id);
    m_nrSlLCG.erase(it);
}

uint32_t
NrSlUeMacSchedulerDstInfo::GetDstL2Id() const
{
    return m_dstL2Id;
}

void
NrSlUeMacSchedulerDstInfo::SetDstMcs(uint8_t mcs)
{
    m_mcs = mcs;
}

uint8_t
NrSlUeMacSchedulerDstInfo::GetDstMcs() const
{
    return m_mcs;
}

} // namespace ns3
