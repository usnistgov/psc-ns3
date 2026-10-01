// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-tdma-pf.h"

#include "nr-mac-scheduler-ue-info-pf.h"

#include <ns3/double.h>
#include <ns3/log.h>

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerTdmaPF");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerTdmaPF);

TypeId
NrMacSchedulerTdmaPF::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacSchedulerTdmaPF")
            .SetParent<NrMacSchedulerTdmaRR>()
            .AddConstructor<NrMacSchedulerTdmaPF>()
            .AddAttribute("FairnessIndex",
                          "Value (between 0 and 1) that defines the PF metric (1 is the "
                          "traditional 3GPP PF, 0 is RR in throughput",
                          DoubleValue(1),
                          MakeDoubleAccessor(&NrMacSchedulerTdmaPF::SetFairnessIndex,
                                             &NrMacSchedulerTdmaPF::GetFairnessIndex),
                          MakeDoubleChecker<double>(0, 1))
            .AddAttribute(
                "LastAvgTPutWeight",
                "Weight of the last average throughput in the average throughput calculation",
                DoubleValue(99),
                MakeDoubleAccessor(&NrMacSchedulerTdmaPF::SetTimeWindow,
                                   &NrMacSchedulerTdmaPF::GetTimeWindow),
                MakeDoubleChecker<double>(0));
    return tid;
}

NrMacSchedulerTdmaPF::NrMacSchedulerTdmaPF()
    : NrMacSchedulerTdmaRR()
{
    NS_LOG_FUNCTION(this);
}

void
NrMacSchedulerTdmaPF::SetFairnessIndex(double v)
{
    NS_LOG_FUNCTION(this);
    m_alpha = v;
}

double
NrMacSchedulerTdmaPF::GetFairnessIndex() const
{
    NS_LOG_FUNCTION(this);
    return m_alpha;
}

void
NrMacSchedulerTdmaPF::SetTimeWindow(double v)
{
    NS_LOG_FUNCTION(this);
    m_timeWindow = v;
}

double
NrMacSchedulerTdmaPF::GetTimeWindow() const
{
    NS_LOG_FUNCTION(this);
    return m_timeWindow;
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerTdmaPF::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoPF>(
        m_alpha,
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerTdmaPF::GetNumRbPerRbg, this));
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaPF::GetUeCompareDlFn() const
{
    return NrMacSchedulerUeInfoPF::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaPF::GetUeCompareUlFn() const
{
    return NrMacSchedulerUeInfoPF::CompareUeWeightsUl;
}

void
NrMacSchedulerTdmaPF::AssignedDlResources(const UePtrAndBufferReq& ue,
                                          [[maybe_unused]] const FTResources& assigned,
                                          const FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->UpdateDlPFMetric(totAssigned, m_timeWindow, m_dlAmc);
}

void
NrMacSchedulerTdmaPF::NotAssignedDlResources(
    const NrMacSchedulerNs3::UePtrAndBufferReq& ue,
    [[maybe_unused]] const NrMacSchedulerNs3::FTResources& notAssigned,
    const NrMacSchedulerNs3::FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->UpdateDlPFMetric(totAssigned, m_timeWindow, m_dlAmc);
}

void
NrMacSchedulerTdmaPF::AssignedUlResources(const UePtrAndBufferReq& ue,
                                          [[maybe_unused]] const FTResources& assigned,
                                          const FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->UpdateUlPFMetric(totAssigned, m_timeWindow, m_ulAmc);
}

void
NrMacSchedulerTdmaPF::NotAssignedUlResources(
    const NrMacSchedulerNs3::UePtrAndBufferReq& ue,
    [[maybe_unused]] const NrMacSchedulerNs3::FTResources& notAssigned,
    const NrMacSchedulerNs3::FTResources& totAssigned) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->UpdateUlPFMetric(totAssigned, m_timeWindow, m_ulAmc);
}

void
NrMacSchedulerTdmaPF::BeforeDlSched(const UePtrAndBufferReq& ue,
                                    const FTResources& assignableInIteration) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->CalculatePotentialTPutDl(assignableInIteration, m_dlAmc);
}

void
NrMacSchedulerTdmaPF::BeforeUlSched(const UePtrAndBufferReq& ue,
                                    const FTResources& assignableInIteration) const
{
    NS_LOG_FUNCTION(this);
    auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoPF>(ue.first);
    uePtr->CalculatePotentialTPutUl(assignableInIteration, m_ulAmc);
}

} // namespace ns3
