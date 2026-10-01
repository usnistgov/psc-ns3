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
    static TypeId tid = TypeId("ns3::NrSlMcsController")
                            .SetParent<Object>()
                            .AddConstructor<NrSlMcsController>()
                            .SetGroupName("nr");
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
NrSlMcsController::DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    ; // Default no-op
}

} // namespace ns3
