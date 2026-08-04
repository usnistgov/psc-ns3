// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ofdma-mr.h"

#include "nr-mac-scheduler-ue-info-mr.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerOfdmaMR");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerOfdmaMR);

TypeId
NrMacSchedulerOfdmaMR::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerOfdmaMR")
                            .SetParent<NrMacSchedulerOfdmaRR>()
                            .AddConstructor<NrMacSchedulerOfdmaMR>();
    return tid;
}

NrMacSchedulerOfdmaMR::NrMacSchedulerOfdmaMR()
    : NrMacSchedulerOfdmaRR()
{
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerOfdmaMR::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoMR>(
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerOfdmaMR::GetNumRbPerRbg, this));
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaMR::GetUeCompareDlFn() const
{
    return NrMacSchedulerUeInfoMR::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaMR::GetUeCompareUlFn() const
{
    return NrMacSchedulerUeInfoMR::CompareUeWeightsUl;
}

} // namespace ns3
