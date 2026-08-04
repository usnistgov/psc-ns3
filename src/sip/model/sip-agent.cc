/**
 * Copyright 2020 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 *
 */

#include "sip-agent.h"

#include "sip-header.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/object.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SipAgent");

namespace sip
{

NS_OBJECT_ENSURE_REGISTERED(SipAgent);

TypeId
SipAgent::GetTypeId()
{
    static TypeId tid = TypeId("ns3::sip::SipAgent")
                            .SetParent<SipElement>()
                            .SetGroupName("Sip")
                            .AddConstructor<SipAgent>();
    return tid;
}

SipAgent::SipAgent()
    : SipElement()
{
    NS_LOG_FUNCTION(this);
}

SipAgent::~SipAgent()
{
    NS_LOG_FUNCTION(this);
}

void
SipAgent::DoDispose()
{
    NS_LOG_FUNCTION(this);
    SipElement::DoDispose();
}

} // namespace sip

} // namespace ns3
