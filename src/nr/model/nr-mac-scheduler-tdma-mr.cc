// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-tdma-mr.h"

#include "nr-mac-scheduler-ue-info-mr.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NrMacSchedulerTdmaMR");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerTdmaMR);

TypeId
NrMacSchedulerTdmaMR::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerTdmaMR")
                            .SetParent<NrMacSchedulerTdmaRR>()
                            .AddConstructor<NrMacSchedulerTdmaMR>();
    return tid;
}

NrMacSchedulerTdmaMR::NrMacSchedulerTdmaMR()
    : NrMacSchedulerTdmaRR()
{
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerTdmaMR::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoMR>(
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerTdmaMR::GetNumRbPerRbg, this));
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaMR::GetUeCompareDlFn() const
{
    return NrMacSchedulerUeInfoMR::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaMR::GetUeCompareUlFn() const
{
    return NrMacSchedulerUeInfoMR::CompareUeWeightsUl;
}

} // namespace ns3
