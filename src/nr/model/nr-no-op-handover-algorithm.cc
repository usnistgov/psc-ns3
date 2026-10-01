// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Budiarto Herman <budiarto.herman@magister.fi>

#include "nr-no-op-handover-algorithm.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrNoOpHandoverAlgorithm");

NS_OBJECT_ENSURE_REGISTERED(NrNoOpHandoverAlgorithm);

NrNoOpHandoverAlgorithm::NrNoOpHandoverAlgorithm()
    : m_handoverManagementSapUser(nullptr)
{
    NS_LOG_FUNCTION(this);
    m_handoverManagementSapProvider =
        new MemberNrHandoverManagementSapProvider<NrNoOpHandoverAlgorithm>(this);
}

NrNoOpHandoverAlgorithm::~NrNoOpHandoverAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

void
NrNoOpHandoverAlgorithm::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_handoverManagementSapProvider;
}

TypeId
NrNoOpHandoverAlgorithm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrNoOpHandoverAlgorithm")
                            .SetParent<NrHandoverAlgorithm>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrNoOpHandoverAlgorithm>();
    return tid;
}

void
NrNoOpHandoverAlgorithm::SetNrHandoverManagementSapUser(NrHandoverManagementSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_handoverManagementSapUser = s;
}

NrHandoverManagementSapProvider*
NrNoOpHandoverAlgorithm::GetNrHandoverManagementSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_handoverManagementSapProvider;
}

void
NrNoOpHandoverAlgorithm::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    NrHandoverAlgorithm::DoInitialize();
}

void
NrNoOpHandoverAlgorithm::DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults)
{
    NS_LOG_FUNCTION(this << rnti << (uint16_t)measResults.measId);
}

} // end of namespace ns3
