// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>
//
///

#include "nr-simple-ue-component-carrier-manager.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSimpleUeComponentCarrierManager");

NS_OBJECT_ENSURE_REGISTERED(NrSimpleUeComponentCarrierManager);

///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// MAC SAP PROVIDER SAP forwarders
///////////////////////////////////////////////////////////

/// NrSimpleUeCcmMacSapProvider class
class NrSimpleUeCcmMacSapProvider : public NrMacSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param mac the component carrier manager
     */
    NrSimpleUeCcmMacSapProvider(NrSimpleUeComponentCarrierManager* mac);

    // inherited from NrMacSapProvider
    void TransmitPdu(NrMacSapProvider::TransmitPduParameters params) override;
    void ReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params) override;

  private:
    NrSimpleUeComponentCarrierManager* m_mac; ///< the component carrier manager
};

NrSimpleUeCcmMacSapProvider::NrSimpleUeCcmMacSapProvider(NrSimpleUeComponentCarrierManager* mac)
    : m_mac(mac)
{
}

void
NrSimpleUeCcmMacSapProvider::TransmitPdu(TransmitPduParameters params)
{
    m_mac->DoTransmitPdu(params);
}

void
NrSimpleUeCcmMacSapProvider::ReportBufferStatus(ReportBufferStatusParameters params)
{
    m_mac->DoReportBufferStatus(params);
}

///////////////////////////////////////////////////////////
// MAC SAP USER SAP forwarders
/////////////// ////////////////////////////////////////////

/// NrSimpleUeCcmMacSapUser class
class NrSimpleUeCcmMacSapUser : public NrMacSapUser
{
  public:
    /**
     * Constructor
     *
     * \param mac the component carrier manager
     */
    NrSimpleUeCcmMacSapUser(NrSimpleUeComponentCarrierManager* mac);

    // inherited from NrMacSapUser
    void NotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams) override;
    void ReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams) override;
    void NotifyHarqDeliveryFailure() override;

  private:
    NrSimpleUeComponentCarrierManager* m_mac; ///< the component carrier manager
};

NrSimpleUeCcmMacSapUser::NrSimpleUeCcmMacSapUser(NrSimpleUeComponentCarrierManager* mac)
    : m_mac(mac)
{
}

void
NrSimpleUeCcmMacSapUser::NotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_LOG_INFO("NrSimpleUeCcmMacSapUser::NotifyTxOpportunity for ccId:"
                << (uint32_t)txOpParams.componentCarrierId);
    m_mac->DoNotifyTxOpportunity(txOpParams);
}

void
NrSimpleUeCcmMacSapUser::ReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    m_mac->DoReceivePdu(rxPduParams);
}

void
NrSimpleUeCcmMacSapUser::NotifyHarqDeliveryFailure()
{
    m_mac->DoNotifyHarqDeliveryFailure();
}

//////////////////////////////////////////////////////////
// NrSimpleUeComponentCarrierManager methods
///////////////////////////////////////////////////////////

NrSimpleUeComponentCarrierManager::NrSimpleUeComponentCarrierManager()
{
    NS_LOG_FUNCTION(this);
    m_ccmRrcSapProvider = new MemberNrUeCcmRrcSapProvider<NrSimpleUeComponentCarrierManager>(this);
    m_ccmMacSapUser = new NrSimpleUeCcmMacSapUser(this);
    m_ccmMacSapProvider = new NrSimpleUeCcmMacSapProvider(this);
}

NrSimpleUeComponentCarrierManager::~NrSimpleUeComponentCarrierManager()
{
    NS_LOG_FUNCTION(this);
}

void
NrSimpleUeComponentCarrierManager::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_ccmRrcSapProvider;
    delete m_ccmMacSapUser;
    delete m_ccmMacSapProvider;
}

TypeId
NrSimpleUeComponentCarrierManager::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSimpleUeComponentCarrierManager")
                            .SetParent<NrUeComponentCarrierManager>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSimpleUeComponentCarrierManager>();
    return tid;
}

NrMacSapProvider*
NrSimpleUeComponentCarrierManager::GetNrMacSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_ccmMacSapProvider;
}

void
NrSimpleUeComponentCarrierManager::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    NrUeComponentCarrierManager::DoInitialize();
}

void
NrSimpleUeComponentCarrierManager::DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults)
{
    NS_LOG_FUNCTION(this << rnti << (uint16_t)measResults.measId);
}

void
NrSimpleUeComponentCarrierManager::DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params)
{
    NS_LOG_FUNCTION(this);
    auto it = m_macSapProvidersMap.find(params.componentCarrierId);
    NS_ABORT_MSG_IF(it == m_macSapProvidersMap.end(),
                    "could not find Sap for NrComponentCarrier "
                        << (uint16_t)params.componentCarrierId);
    // with this algorithm all traffic is on Primary Carrier, is it?
    it->second->TransmitPdu(params);
}

void
NrSimpleUeComponentCarrierManager::DoReportBufferStatus(
    NrMacSapProvider::ReportBufferStatusParameters params)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("BSR from RLC for LCID = " << (uint16_t)params.lcid);
    auto it = m_macSapProvidersMap.find(0);
    NS_ABORT_MSG_IF(it == m_macSapProvidersMap.end(), "could not find Sap for NrComponentCarrier");

    NS_LOG_DEBUG("Size of component carrier LC map " << m_componentCarrierLcMap.size());

    for (auto ccLcMapIt = m_componentCarrierLcMap.begin();
         ccLcMapIt != m_componentCarrierLcMap.end();
         ccLcMapIt++)
    {
        NS_LOG_DEBUG("BSR from RLC for CC id = " << (uint16_t)ccLcMapIt->first);
        auto it = ccLcMapIt->second.find(params.lcid);
        if (it != ccLcMapIt->second.end())
        {
            it->second->ReportBufferStatus(params);
        }
    }
}

void
NrSimpleUeComponentCarrierManager::DoNotifyHarqDeliveryFailure()
{
    NS_LOG_FUNCTION(this);
}

void
NrSimpleUeComponentCarrierManager::DoNotifyTxOpportunity(
    NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_LOG_FUNCTION(this);
    auto lcidIt = m_lcAttached.find(txOpParams.lcid);
    NS_ABORT_MSG_IF(lcidIt == m_lcAttached.end(),
                    "could not find LCID" << (uint16_t)txOpParams.lcid);
    NS_LOG_DEBUG(this << " lcid = " << (uint32_t)txOpParams.lcid
                      << " layer= " << (uint16_t)txOpParams.layer << " componentCarrierId "
                      << (uint16_t)txOpParams.componentCarrierId << " rnti " << txOpParams.rnti);

    NS_LOG_DEBUG(this << " MAC is asking component carrier id = "
                      << (uint16_t)txOpParams.componentCarrierId
                      << " with lcid = " << (uint32_t)txOpParams.lcid << " to transmit "
                      << txOpParams.bytes << " bytes");
    (*lcidIt).second->NotifyTxOpportunity(txOpParams);
}

void
NrSimpleUeComponentCarrierManager::DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_LOG_FUNCTION(this);
    auto lcidIt = m_lcAttached.find(rxPduParams.lcid);
    NS_ABORT_MSG_IF(lcidIt == m_lcAttached.end(),
                    "could not find LCID" << (uint16_t)rxPduParams.lcid);
    if (lcidIt != m_lcAttached.end())
    {
        (*lcidIt).second->ReceivePdu(rxPduParams);
    }
}

///////////////////////////////////////////////////////////
// Ue CCM RRC SAP PROVIDER SAP forwarders
///////////////////////////////////////////////////////////
std::vector<uint16_t>
NrSimpleUeComponentCarrierManager::DoRemoveLc(uint8_t lcid)
{
    NS_LOG_FUNCTION(this << " lcId" << lcid);
    std::vector<uint16_t> res;
    NS_ABORT_MSG_IF(m_lcAttached.find(lcid) == m_lcAttached.end(), "could not find LCID " << lcid);
    m_lcAttached.erase(lcid);

    // send back all the configuration to the NrComponentCarrier where we want to remove the Lc
    auto it = m_componentCarrierLcMap.begin();
    while (it != m_componentCarrierLcMap.end())
    {
        auto lcToRemove = it->second.find(lcid);
        if (lcToRemove != it->second.end())
        {
            res.insert(res.end(), it->first);
        }
        it->second.erase(lcToRemove);
        it++;
    }
    NS_ABORT_MSG_IF(res.empty(),
                    "LCID " << lcid << " not found in the ComponentCarrierManager map");

    return res;
}

void
NrSimpleUeComponentCarrierManager::DoReset()
{
    NS_LOG_FUNCTION(this);
    // same semantics as NrUeMac::DoRest
    auto it = m_lcAttached.begin();
    while (it != m_lcAttached.end())
    {
        // don't delete CCCH
        if (it->first == 0)
        {
            ++it;
        }
        else
        {
            // note: use of postfix operator preserves validity of iterator
            m_lcAttached.erase(it++);
        }
    }
}

std::vector<NrUeCcmRrcSapProvider::LcsConfig>
NrSimpleUeComponentCarrierManager::DoAddLc(uint8_t lcId,
                                           NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
                                           NrMacSapUser* msu)
{
    NS_LOG_FUNCTION(this);
    std::vector<NrUeCcmRrcSapProvider::LcsConfig> res;
    auto it = m_lcAttached.find(lcId);
    NS_ABORT_MSG_IF(it != m_lcAttached.end(), "Warning, LCID " << lcId << " already exist");
    m_lcAttached.insert(std::pair<uint8_t, NrMacSapUser*>(lcId, msu));
    NrUeCcmRrcSapProvider::LcsConfig elem;
    for (uint8_t ncc = 0; ncc < m_noOfComponentCarriers; ncc++)
    {
        elem.componentCarrierId = ncc;
        elem.lcConfig = lcConfig;
        elem.msu = m_ccmMacSapUser;
        res.insert(res.end(), elem);

        auto ccLcMapIt = m_componentCarrierLcMap.find(ncc);
        if (ccLcMapIt != m_componentCarrierLcMap.end())
        {
            ccLcMapIt->second.insert(
                std::pair<uint8_t, NrMacSapProvider*>(lcId, m_macSapProvidersMap.at(ncc)));
        }
        else
        {
            std::map<uint8_t, NrMacSapProvider*> empty;
            auto ret = m_componentCarrierLcMap.insert(
                std::pair<uint8_t, std::map<uint8_t, NrMacSapProvider*>>(ncc, empty));
            NS_ABORT_MSG_IF(!ret.second,
                            "element already present, ComponentCarrierId already exist");
            ccLcMapIt = m_componentCarrierLcMap.find(ncc);
            ccLcMapIt->second.insert(
                std::pair<uint8_t, NrMacSapProvider*>(lcId, m_macSapProvidersMap.at(ncc)));
        }
    }

    return res;
}

NrMacSapUser*
NrSimpleUeComponentCarrierManager::DoConfigureSignalBearer(
    uint8_t lcid,
    NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
    NrMacSapUser* msu)
{
    NS_LOG_FUNCTION(this);
    auto it = m_lcAttached.find(lcid);
    // if the following assert is hit, e.g., in handover scenarios, it means
    //  the DoRest function is not called by UE RRC
    NS_ABORT_MSG_IF(it != m_lcAttached.end(),
                    "Warning, LCID " << (uint8_t)lcid << " already exist");

    m_lcAttached.insert(std::pair<uint8_t, NrMacSapUser*>(lcid, msu));

    for (uint8_t ncc = 0; ncc < m_noOfComponentCarriers; ncc++)
    {
        auto ccLcMapIt = m_componentCarrierLcMap.find(ncc);
        if (ccLcMapIt != m_componentCarrierLcMap.end())
        {
            ccLcMapIt->second.insert(
                std::pair<uint8_t, NrMacSapProvider*>(lcid, m_macSapProvidersMap.at(ncc)));
        }
        else
        {
            std::map<uint8_t, NrMacSapProvider*> empty;
            auto ret = m_componentCarrierLcMap.insert(
                std::pair<uint8_t, std::map<uint8_t, NrMacSapProvider*>>(ncc, empty));
            NS_ABORT_MSG_IF(!ret.second,
                            "element already present, ComponentCarrierId already existed");
            ccLcMapIt = m_componentCarrierLcMap.find(ncc);
            ccLcMapIt->second.insert(
                std::pair<uint8_t, NrMacSapProvider*>(lcid, m_macSapProvidersMap.at(ncc)));
        }
    }

    return m_ccmMacSapUser;
}

} // end of namespace ns3
