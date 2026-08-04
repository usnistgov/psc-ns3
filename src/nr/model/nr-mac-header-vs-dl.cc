// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-header-vs-dl.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrMacHeaderVsDl);
NS_LOG_COMPONENT_DEFINE("NrMacHeaderVsDl");

TypeId
NrMacHeaderVsDl::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacHeaderVsDl").SetParent<NrMacHeaderVs>().AddConstructor<NrMacHeaderVsDl>();
    return tid;
}

TypeId
NrMacHeaderVsDl::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrMacHeaderVsDl::NrMacHeaderVsDl()
{
    NS_LOG_FUNCTION(this);
}

NrMacHeaderVsDl::~NrMacHeaderVsDl()
{
    NS_LOG_FUNCTION(this);
}

void
NrMacHeaderVsDl::SetLcId(uint8_t lcId)
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
NrMacHeaderVsDl::IsVariableSizeHeader() const
{
    if (m_lcid <= 32)
    {
        return true;
    }
    if (m_lcid == SP_SRS)
    {
        return true;
    }
    if (m_lcid == TCI_STATES_PDSCH)
    {
        return true;
    }
    if (m_lcid == APERIODIC_CSI)
    {
        return true;
    }
    if (m_lcid == SP_CSI_RS_IM)
    {
        return true;
    }
    return false;
}

} // namespace ns3
