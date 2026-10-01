// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>
//
///

#include "nr-ue-component-carrier-manager.h"

#include "nr-common.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrUeComponentCarrierManager");
NS_OBJECT_ENSURE_REGISTERED(NrUeComponentCarrierManager);

NrUeComponentCarrierManager::NrUeComponentCarrierManager()
    : m_ccmRrcSapUser(nullptr),
      m_ccmRrcSapProvider(nullptr),
      m_noOfComponentCarriers(0)
{
    NS_LOG_FUNCTION(this);
}

NrUeComponentCarrierManager::~NrUeComponentCarrierManager()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrUeComponentCarrierManager::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrUeComponentCarrierManager").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

void
NrUeComponentCarrierManager::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

void
NrUeComponentCarrierManager::SetNrCcmRrcSapUser(NrUeCcmRrcSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_ccmRrcSapUser = s;
}

NrUeCcmRrcSapProvider*
NrUeComponentCarrierManager::GetNrCcmRrcSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_ccmRrcSapProvider;
}

bool
NrUeComponentCarrierManager::SetComponentCarrierMacSapProviders(uint8_t componentCarrierId,
                                                                NrMacSapProvider* sap)
{
    NS_LOG_FUNCTION(this);
    bool result = false;
    auto it = m_macSapProvidersMap.find(componentCarrierId);
    if (componentCarrierId > m_noOfComponentCarriers)
    {
        NS_FATAL_ERROR("Inconsistent componentCarrierId or you didn't call "
                       "SetNumberOfComponentCarriers before calling this method");
    }
    if (it != m_macSapProvidersMap.end())
    {
        NS_FATAL_ERROR("Tried to allocated an existing componentCarrierId");
    }
    else
    {
        m_macSapProvidersMap.insert(std::pair<uint8_t, NrMacSapProvider*>(componentCarrierId, sap));
        result = true;
    }
    return result;
}

void
NrUeComponentCarrierManager::SetNumberOfComponentCarriers(uint8_t noOfComponentCarriers)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(noOfComponentCarriers < nr::MIN_NO_CC || noOfComponentCarriers > nr::MAX_NO_CC,
                    "Number of component carriers should be greater than 0 and less than 6");
    m_noOfComponentCarriers = noOfComponentCarriers;
    // Set the number of component carriers in UE RRC
    m_ccmRrcSapUser->SetNumberOfComponentCarriers(noOfComponentCarriers);
}

} // end of namespace ns3
