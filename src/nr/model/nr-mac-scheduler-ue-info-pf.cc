// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ue-info-pf.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerUeInfoPF");

void
NrMacSchedulerUeInfoPF::UpdateDlPFMetric(const NrMacSchedulerNs3::FTResources& totAssigned,
                                         double timeWindow,
                                         const Ptr<const NrAmc>& amc)
{
    NS_LOG_FUNCTION(this);

    NrMacSchedulerUeInfo::UpdateDlMetric(amc);

    m_currTputDl = static_cast<double>(m_dlTbSize) / (totAssigned.m_sym);
    m_avgTputDl = ((1.0 - (1.0 / static_cast<double>(timeWindow))) * m_lastAvgTputDl) +
                  ((1.0 / timeWindow) * m_currTputDl);

    NS_LOG_DEBUG("Update DL PF Metric for UE "
                 << m_rnti << " DL TBS: " << m_dlTbSize << " Updated currTputDl " << m_currTputDl
                 << " avgTputDl " << m_avgTputDl << " over n. of syms: " << +totAssigned.m_sym
                 << ", last Avg TH Dl " << m_lastAvgTputDl << " total sym assigned "
                 << static_cast<uint32_t>(totAssigned.m_sym)
                 << " updated DL metric: " << m_potentialTputDl / std::max(1E-9, m_avgTputDl));
}

void
NrMacSchedulerUeInfoPF::UpdateUlPFMetric(const NrMacSchedulerNs3::FTResources& totAssigned,
                                         double timeWindow,
                                         const Ptr<const NrAmc>& amc)
{
    NS_LOG_FUNCTION(this);

    NrMacSchedulerUeInfo::UpdateUlMetric(amc);

    m_currTputUl = static_cast<double>(m_ulTbSize) / (totAssigned.m_sym);
    m_avgTputUl = ((1.0 - (1.0 / static_cast<double>(timeWindow))) * m_lastAvgTputUl) +
                  ((1.0 / timeWindow) * m_currTputUl);

    NS_LOG_DEBUG("Update UL PF Metric for UE "
                 << m_rnti << " UL TBS: " << m_ulTbSize << " Updated currTputUl " << m_currTputUl
                 << " avgTputUl " << m_avgTputUl << " over n. of syms: " << +totAssigned.m_sym
                 << ", last Avg TH Ul " << m_lastAvgTputUl << " total sym assigned "
                 << static_cast<uint32_t>(totAssigned.m_sym)
                 << " updated UL metric: " << m_potentialTputUl / std::max(1E-9, m_avgTputUl));
}

void
NrMacSchedulerUeInfoPF::CalculatePotentialTPutDl(
    const NrMacSchedulerNs3::FTResources& assignableInIteration,
    const Ptr<const NrAmc>& amc)
{
    NS_LOG_FUNCTION(this);

    uint32_t rbsAssignable = assignableInIteration.m_rbg * GetNumRbPerRbg();
    m_potentialTputDl = amc->GetPayloadSize(m_dlMcs, m_dlRank, rbsAssignable);
    m_potentialTputDl /= assignableInIteration.m_sym;

    NS_LOG_INFO("UE " << m_rnti << " potentialTputDl " << m_potentialTputDl << " lastAvgThDl "
                      << m_lastAvgTputDl
                      << " DL metric: " << m_potentialTputDl / std::max(1E-9, m_avgTputDl));
}

void
NrMacSchedulerUeInfoPF::CalculatePotentialTPutUl(
    const NrMacSchedulerNs3::FTResources& assignableInIteration,
    const Ptr<const NrAmc>& amc)
{
    NS_LOG_FUNCTION(this);

    uint32_t rbsAssignable = assignableInIteration.m_rbg * GetNumRbPerRbg();
    m_potentialTputUl = amc->GetPayloadSize(m_ulMcs, m_ulRank, rbsAssignable);
    m_potentialTputUl /= assignableInIteration.m_sym;

    NS_LOG_INFO("UE " << m_rnti << " potentialTputUl " << m_potentialTputUl << " lastAvgThUl "
                      << m_lastAvgTputUl
                      << " UL metric: " << m_potentialTputUl / std::max(1E-9, m_avgTputUl));
}

} // namespace ns3
