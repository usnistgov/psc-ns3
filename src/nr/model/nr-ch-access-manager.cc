// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-ch-access-manager.h"

#include <ns3/double.h>
#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrChAccessManager");
NS_OBJECT_ENSURE_REGISTERED(NrChAccessManager);

TypeId
NrChAccessManager::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrChAccessManager")
                            .SetParent<Object>()
                            .SetGroupName("nr")
                            .AddAttribute("GrantDuration",
                                          "Duration of grant for transmitting.",
                                          TimeValue(Minutes(1)),
                                          MakeTimeAccessor(&NrChAccessManager::SetGrantDuration,
                                                           &NrChAccessManager::GetGrantDuration),
                                          MakeTimeChecker());
    return tid;
}

NrChAccessManager::NrChAccessManager()
{
    NS_LOG_FUNCTION(this);
}

NrChAccessManager::~NrChAccessManager()
{
    NS_LOG_FUNCTION(this);
}

void
NrChAccessManager::SetGrantDuration(Time grantDuration)
{
    NS_LOG_FUNCTION(this);
    m_grantDuration = grantDuration;
}

Time
NrChAccessManager::GetGrantDuration() const
{
    NS_LOG_FUNCTION(this);
    return m_grantDuration;
}

void
NrChAccessManager::SetNrSpectrumPhy(Ptr<NrSpectrumPhy> spectrumPhy)
{
    NS_LOG_FUNCTION(this);
    m_spectrumPhy = spectrumPhy;
}

Ptr<NrSpectrumPhy>
NrChAccessManager::GetNrSpectrumPhy()
{
    NS_LOG_FUNCTION(this);
    return m_spectrumPhy;
}

void
NrChAccessManager::SetNrGnbMac(Ptr<NrGnbMac> mac)
{
    NS_LOG_FUNCTION(this);
    m_mac = mac;
}

Ptr<NrGnbMac>
NrChAccessManager::GetNrGnbMac()
{
    NS_LOG_FUNCTION(this);
    return m_mac;
}

// -----------------------------------------------------------------

NS_OBJECT_ENSURE_REGISTERED(NrAlwaysOnAccessManager);

TypeId
NrAlwaysOnAccessManager::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrAlwaysOnAccessManager")
                            .SetParent<NrChAccessManager>()
                            .SetGroupName("nr")
                            .AddConstructor<NrAlwaysOnAccessManager>();
    return tid;
}

NrAlwaysOnAccessManager::NrAlwaysOnAccessManager()
    : NrChAccessManager()
{
    NS_LOG_FUNCTION(this);
}

NrAlwaysOnAccessManager::~NrAlwaysOnAccessManager()
{
    NS_LOG_FUNCTION(this);
}

void
NrAlwaysOnAccessManager::RequestAccess()
{
    NS_LOG_FUNCTION(this);
    // We are always on!!
    for (const auto& cb : m_accessGrantedCb)
    {
        cb(Time::Max() - Simulator::Now());
    }
}

void
NrAlwaysOnAccessManager::SetAccessGrantedCallback(const AccessGrantedCallback& cb)
{
    NS_LOG_FUNCTION(this);
    m_accessGrantedCb.push_back(cb);
}

void
NrAlwaysOnAccessManager::SetAccessDeniedCallback(
    [[maybe_unused]] const NrChAccessManager::AccessDeniedCallback& cb)
{
    NS_LOG_FUNCTION(this);
    // Don't store it, as we will not call it..
}

void
NrAlwaysOnAccessManager::Cancel()
{
    NS_LOG_FUNCTION(this);
    // Do nothing, we are always on and we call the callback only when RequestAccess()
    // is called
}

} // namespace ns3
