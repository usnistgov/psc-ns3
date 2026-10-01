// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ofdma-rr.h"

#include "nr-mac-scheduler-ue-info-rr.h"

#include <ns3/log.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NrMacSchedulerOfdmaRR");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerOfdmaRR);

TypeId
NrMacSchedulerOfdmaRR::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerOfdmaRR")
                            .SetParent<NrMacSchedulerOfdma>()
                            .AddConstructor<NrMacSchedulerOfdmaRR>();
    return tid;
}

NrMacSchedulerOfdmaRR::NrMacSchedulerOfdmaRR()
    : NrMacSchedulerOfdma()
{
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerOfdmaRR::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoRR>(
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerOfdmaRR::GetNumRbPerRbg, this));
}

void
NrMacSchedulerOfdmaRR::AssignedDlResources(const UePtrAndBufferReq& ue,
                                           [[maybe_unused]] const FTResources& assigned,
                                           [[maybe_unused]] const FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    GetFirst GetUe;
    GetUe(ue)->UpdateDlMetric(m_dlAmc);
}

void
NrMacSchedulerOfdmaRR::AssignedUlResources(const UePtrAndBufferReq& ue,
                                           [[maybe_unused]] const FTResources& assigned,
                                           [[maybe_unused]] const FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    GetFirst GetUe;
    GetUe(ue)->UpdateUlMetric(m_ulAmc);
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaRR::GetUeCompareDlFn() const
{
    return NrMacSchedulerUeInfoRR::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaRR::GetUeCompareUlFn() const
{
    return NrMacSchedulerUeInfoRR::CompareUeWeightsUl;
}

} // namespace ns3
