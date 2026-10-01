// Copyright (c) 2017 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "bandwidth-part-ue.h"

#include "nr-ue-mac.h"
#include "nr-ue-phy.h"

#include <ns3/abort.h>
#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/pointer.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("BandwidthPartUe");

NS_OBJECT_ENSURE_REGISTERED(BandwidthPartUe);

TypeId
BandwidthPartUe::GetTypeId()
{
    static TypeId tid = TypeId("ns3::BandwidthPartUe")
                            .SetParent<NrComponentCarrier>()
                            .AddConstructor<BandwidthPartUe>()
                            .AddAttribute("NrUePhy",
                                          "The PHY associated to this BandwidthPartUe",
                                          PointerValue(),
                                          MakePointerAccessor(&BandwidthPartUe::m_phy),
                                          MakePointerChecker<NrUePhy>())
                            .AddAttribute("NrUeMac",
                                          "The MAC associated to this BandwidthPartUe",
                                          PointerValue(),
                                          MakePointerAccessor(&BandwidthPartUe::m_mac),
                                          MakePointerChecker<NrUeMac>());
    return tid;
}

BandwidthPartUe::BandwidthPartUe()
    : NrComponentCarrier()
{
    NS_LOG_FUNCTION(this);
    m_phy = nullptr;
}

BandwidthPartUe::~BandwidthPartUe()
{
    NS_LOG_FUNCTION(this);
}

void
BandwidthPartUe::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_phy->Dispose();
    m_phy = nullptr;
    m_mac->Dispose();
    m_mac = nullptr;
    Object::DoDispose();
}

void
BandwidthPartUe::SetPhy(Ptr<NrUePhy> s)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_IF(m_phy != nullptr);
    m_phy = s;
}

Ptr<NrUePhy>
BandwidthPartUe::GetPhy() const
{
    NS_LOG_FUNCTION(this);
    return m_phy;
}

void
BandwidthPartUe::SetMac(Ptr<NrUeMac> s)
{
    NS_LOG_FUNCTION(this);
    m_mac = s;
}

Ptr<NrUeMac>
BandwidthPartUe::GetMac() const
{
    NS_LOG_FUNCTION(this);
    return m_mac;
}

} // namespace ns3
