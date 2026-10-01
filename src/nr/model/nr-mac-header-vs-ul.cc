// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-header-vs-ul.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrMacHeaderVsUl);
NS_LOG_COMPONENT_DEFINE("NrMacHeaderVsUl");

TypeId
NrMacHeaderVsUl::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacHeaderVsUl").SetParent<NrMacHeaderVs>().AddConstructor<NrMacHeaderVsUl>();
    return tid;
}

TypeId
NrMacHeaderVsUl::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrMacHeaderVsUl::NrMacHeaderVsUl()
{
    NS_LOG_FUNCTION(this);
}

NrMacHeaderVsUl::~NrMacHeaderVsUl()
{
    NS_LOG_FUNCTION(this);
}

void
NrMacHeaderVsUl::SetLcId(uint8_t lcId)
{
    if (lcId <= 32)
    {
        NrMacHeaderVs::SetLcId(lcId);
    }
    else
    {
        m_lcid = lcId;
        NS_ASSERT(IsVariableSizeHeader());
    }
}

bool
NrMacHeaderVsUl::IsVariableSizeHeader() const
{
    if (m_lcid <= 32)
    {
        return true;
    }
    if (m_lcid == MULTIPLE_ENTRY_PHR_FOUR_OCTET)
    {
        return true;
    }
    if (m_lcid == MULTIPLE_ENTRY_PHR_ONE_OCTET)
    {
        return true;
    }
    if (m_lcid == LONG_TRUNCATED_BSR)
    {
        return true;
    }
    if (m_lcid == LONG_BSR)
    {
        return true;
    }

    return false;
}

} // namespace ns3
