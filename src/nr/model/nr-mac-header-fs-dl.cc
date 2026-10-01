// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-header-fs-dl.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrMacHeaderFsDl);
NS_LOG_COMPONENT_DEFINE("NrMacHeaderFsDl");

TypeId
NrMacHeaderFsDl::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacHeaderFsDl").SetParent<NrMacHeaderFs>().AddConstructor<NrMacHeaderFsDl>();
    return tid;
}

TypeId
NrMacHeaderFsDl::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrMacHeaderFsDl::NrMacHeaderFsDl()
{
    NS_LOG_FUNCTION(this);
}

NrMacHeaderFsDl::~NrMacHeaderFsDl()
{
    NS_LOG_FUNCTION(this);
}

bool
NrMacHeaderFsDl::IsFixedSizeHeader() const
{
    if (m_lcid == RECOMMENDED_BIT_RATE)
    {
        return true;
    }
    if (m_lcid == SP_ZP_CSI_RS)
    {
        return true;
    }
    if (m_lcid == PUCCH_SPATIAL_RELATION)
    {
        return true;
    }
    if (m_lcid == SP_CSI_REPORT)
    {
        return true;
    }
    if (m_lcid == TCI_STATE_INDICATION_PDCCH)
    {
        return true;
    }
    if (m_lcid == DUPLICATION)
    {
        return true;
    }
    if (m_lcid == SCELL_FOUR_OCTET)
    {
        return true;
    }
    if (m_lcid == SCELL_ONE_OCTET)
    {
        return true;
    }
    if (m_lcid == LONG_DRX)
    {
        return true;
    }
    if (m_lcid == DRX)
    {
        return true;
    }
    if (m_lcid == TIMING_ADVANCE)
    {
        return true;
    }
    if (m_lcid == UE_CONTENTION_RESOLUTION)
    {
        return true;
    }
    if (m_lcid == PADDING)
    {
        return true;
    }

    return false;
}

void
NrMacHeaderFsDl::SetLcId(uint8_t v)
{
    // Here we are sure, thanks to the compiler, that v is one of the allowed values
    m_lcid = v;
    NS_ASSERT(IsFixedSizeHeader());
}

} // namespace ns3
