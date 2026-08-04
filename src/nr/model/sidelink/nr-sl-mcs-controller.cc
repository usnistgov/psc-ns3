//
// SPDX-License-Identifier: NIST-Software

#include "nr-sl-mcs-controller.h"

#include "nr-sl-ue-mac-scheduler-default.h"
#include "nr-sl-ue-mac.h"

#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/pointer.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlMcsController");

NS_OBJECT_ENSURE_REGISTERED(NrSlMcsController);

TypeId
NrSlMcsController::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlMcsController")
            .SetParent<Object>()
            .SetGroupName("nr")
            .AddTraceSource("GrantSelection",
                            "Report on the outcome of a GetGrantParams() call",
                            MakeTraceSourceAccessor(&NrSlMcsController::m_grantSelectionTrace),
                            "ns3::NrSlMcsController::GrantSelectionCallback");
    return tid;
}

NrSlMcsController::NrSlMcsController()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlMcsController::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_mac = nullptr;
    m_scheduler = nullptr;
    Object::DoDispose();
}

int64_t
NrSlMcsController::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    return 0;
}

void
NrSlMcsController::SetNrSlUeMac(Ptr<NrSlUeMac> mac)
{
    NS_LOG_FUNCTION(this << mac);
    m_mac = mac;
}

void
NrSlMcsController::SetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    m_scheduler = scheduler;
    DoSetNrSlUeMacScheduler(scheduler);
}

void
NrSlMcsController::SetNumerology(uint16_t numerology)
{
    NS_LOG_FUNCTION(this << numerology);
    m_numerology = numerology;
}

GrantParams
NrSlMcsController::GetGrantParams(uint32_t dstL2Id,
                                  SidelinkInfo::CastType castType,
                                  std::optional<uint32_t> numRbs,
                                  std::optional<uint32_t> tbSize)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    if (!m_numerology.has_value())
    {
        NS_ASSERT_MSG(m_mac, "NrSlUeMac not set; call SetNumerology() or SetNrSlUeMac() first");
        m_numerology = m_mac->GetNumerology();
        NS_LOG_DEBUG("Numerology set to " << m_numerology.value());
    }
    auto params = DoGetGrantParams(dstL2Id, castType, numRbs, tbSize);
    if (params.mcs.has_value())
    {
        NS_LOG_DEBUG("MCS " << +params.mcs.value() << " selected for dstL2Id " << dstL2Id);
    }
    else
    {
        NS_LOG_DEBUG("No MCS value for dstL2Id " << dstL2Id);
    }
    m_grantSelectionTrace(params, dstL2Id, castType, numRbs, tbSize);
    return params;
}

GrantParams
NrSlMcsController::GetLargestFeasibleTb(uint32_t dstL2Id,
                                        SidelinkInfo::CastType castType,
                                        uint16_t maxSubCh)
{
    NS_LOG_FUNCTION(this << dstL2Id << maxSubCh);
    if (!m_numerology.has_value())
    {
        NS_ASSERT_MSG(m_mac, "NrSlUeMac not set; call SetNumerology() or SetNrSlUeMac() first");
        m_numerology = m_mac->GetNumerology();
        NS_LOG_DEBUG("Numerology set to " << m_numerology.value());
    }
    return DoGetLargestFeasibleTb(dstL2Id, castType, maxSubCh);
}

GrantParams
NrSlMcsController::DoGetLargestFeasibleTb([[maybe_unused]] uint32_t dstL2Id,
                                          [[maybe_unused]] SidelinkInfo::CastType castType,
                                          [[maybe_unused]] uint16_t maxSubCh)
{
    NS_LOG_FUNCTION(this << dstL2Id << maxSubCh);
    return {};
}

void
NrSlMcsController::DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    ; // Default no-op
}

} // namespace ns3
