// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>

#include "nr-gnb-component-carrier-manager.h"

#include "nr-common.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrGnbComponentCarrierManager");
NS_OBJECT_ENSURE_REGISTERED(NrGnbComponentCarrierManager);

NrGnbComponentCarrierManager::NrGnbComponentCarrierManager()
{
}

NrGnbComponentCarrierManager::~NrGnbComponentCarrierManager()
{
}

TypeId
NrGnbComponentCarrierManager::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrGnbComponentCarrierManager").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

void
NrGnbComponentCarrierManager::DoDispose()
{
}

void
NrGnbComponentCarrierManager::SetNrCcmRrcSapUser(NrCcmRrcSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_ccmRrcSapUser = s;
}

NrCcmRrcSapProvider*
NrGnbComponentCarrierManager::GetNrCcmRrcSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_ccmRrcSapProvider;
}

NrMacSapProvider*
NrGnbComponentCarrierManager::GetNrMacSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_macSapProvider;
}

NrCcmMacSapUser*
NrGnbComponentCarrierManager::GetNrCcmMacSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_ccmMacSapUser;
}

bool
NrGnbComponentCarrierManager::SetMacSapProvider(uint8_t componentCarrierId, NrMacSapProvider* sap)
{
    NS_LOG_FUNCTION(this);
    bool res = false;
    auto it = m_macSapProvidersMap.find(componentCarrierId);
    if ((uint16_t)componentCarrierId > m_noOfComponentCarriers)
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
        res = true;
    }
    return res;
}

bool
NrGnbComponentCarrierManager::SetCcmMacSapProviders(uint8_t componentCarrierId,
                                                    NrCcmMacSapProvider* sap)
{
    NS_LOG_FUNCTION(this);
    bool res = false;
    auto it = m_ccmMacSapProviderMap.find(componentCarrierId);

    if (it == m_ccmMacSapProviderMap.end())
    {
        m_ccmMacSapProviderMap.insert(
            std::pair<uint8_t, NrCcmMacSapProvider*>(componentCarrierId, sap));
    }

    res = true;
    return res;
}

void
NrGnbComponentCarrierManager::SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(noOfComponentCarriers < nr::MIN_NO_CC || noOfComponentCarriers > nr::MAX_NO_CC,
                    "Number of component carriers should be greater than 0 and less than 6");
    m_noOfComponentCarriers = noOfComponentCarriers;
    // Set the number of component carriers in gNB RRC
    m_ccmRrcSapUser->SetNumberOfComponentCarriers(noOfComponentCarriers);
}

} // end of namespace ns3
