// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-ue-mac-scheduler.h"

#include "nr-sl-ue-mac.h"

#include <ns3/log.h>
#include <ns3/pointer.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeMacScheduler");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeMacScheduler);

TypeId
NrSlUeMacScheduler::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlUeMacScheduler")
            .SetParent<Object>()
            .SetGroupName("nr")
            .AddTraceSource("BufferStatusRequest",
                            "Trace the signaling of a status request from the RLC",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_bsrTrace),
                            "ns3::NrSlUeMacScheduler::BufferStatusRequestTracedCallback")
            .AddTraceSource("AddLogicalChannel",
                            "Trace the addition of a logical channel",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_addLcTrace),
                            "ns3::NrSlUeMacScheduler::AddLogicalChannelTracedCallback")
            .AddTraceSource("RemoveLogicalChannel",
                            "Trace the removal of a logical channel",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_removeLcTrace),
                            "ns3::NrSlUeMacScheduler::RemoveLogicalChannelTracedCallback")
            .AddTraceSource("GrantCreated",
                            "Trace the creation of a grant",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_grantCreatedTrace),
                            "ns3::NrSlUeMacScheduler::GrantCreatedCallback")
            .AddTraceSource("GrantReused",
                            "Trace the reuse of an SPS grant when resource allocations are kept",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_grantReusedTrace),
                            "ns3::NrSlUeMacScheduler::GrantReusedCallback")
            .AddTraceSource("GrantPublished",
                            "Trace the publishing of a grant to the NrSlUeMac",
                            MakeTraceSourceAccessor(&NrSlUeMacScheduler::m_grantPublishedTrace),
                            "ns3::NrSlUeMacScheduler::GrantPublishedCallback");
    return tid;
}

NrSlUeMacScheduler::NrSlUeMacScheduler()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeMacScheduler::~NrSlUeMacScheduler()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlUeMacScheduler::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_ueMac = nullptr;
    Object::DoDispose();
}

void
NrSlUeMacScheduler::SchedNrSlTriggerReq(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this << sfn);
    DoSchedNrSlTriggerReq(sfn);
}

void
NrSlUeMacScheduler::SchedNrSlRlcBufferReq(
    const struct NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this);
    m_bsrTrace(params);
    DoSchedNrSlRlcBufferReq(params);
}

void
NrSlUeMacScheduler::CschedNrSlLcConfigReq(
    const struct NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params)
{
    NS_LOG_FUNCTION(this);
    m_addLcTrace(params);
    DoCschedNrSlLcConfigReq(params);
}

void
NrSlUeMacScheduler::RemoveNrSlLcConfigReq(uint8_t lcid, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    m_removeLcTrace(lcid, dstL2Id);
    DoRemoveNrSlLcConfigReq(lcid, dstL2Id);
}

void
NrSlUeMacScheduler::NotifyNrSlRlcPduDequeue(uint32_t dstL2Id, uint8_t lcId, uint32_t size)
{
    NS_LOG_FUNCTION(this);
    DoNotifyNrSlRlcPduDequeue(dstL2Id, lcId, size);
}

void
NrSlUeMacScheduler::SetNrSlUeMac(Ptr<NrSlUeMac> ueMac)
{
    NS_LOG_FUNCTION(this);
    m_ueMac = ueMac;
}

Ptr<NrSlUeMac>
NrSlUeMacScheduler::GetMac() const
{
    return m_ueMac;
}

void
NrSlUeMacScheduler::NotifyGrantCreated(const struct GrantInfo& grant) const
{
    m_grantCreatedTrace(grant, m_ueMac->GetPsfchPeriod());
}

void
NrSlUeMacScheduler::NotifyGrantReused(double probResourceKeep, double randomVariate) const
{
    m_grantReusedTrace(probResourceKeep, randomVariate);
}

void
NrSlUeMacScheduler::NotifyGrantPublished(const struct NrSlUeMac::NrSlGrant& grant) const
{
    m_grantPublishedTrace(grant, m_ueMac->GetPsfchPeriod());
}

} // namespace ns3
