//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-ue-service.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeService");

NS_OBJECT_ENSURE_REGISTERED(NrSlUeService);

NrSlUeService::NrSlUeService()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeService::~NrSlUeService()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlUeService::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlUeService::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::NrSlUeService").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

} // namespace ns3
