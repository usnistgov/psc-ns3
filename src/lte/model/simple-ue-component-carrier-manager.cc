/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
* Copyright (c) 2015 Danilo Abrignani
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation;
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Author: Danilo Abrignani <danilo.abrignani@unibo.it>
*
*/

#include "simple-ue-component-carrier-manager.h"
#include <ns3/log.h>
#include <ns3/lte-ue-mac.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SimpleUeComponentCarrierManager");

NS_OBJECT_ENSURE_REGISTERED (SimpleUeComponentCarrierManager);

///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// MAC SAP PROVIDER SAP forwarders
///////////////////////////////////////////////////////////

/// SimpleUeCcmMacSapProvider class
class SimpleUeCcmMacSapProvider : public LteMacSapProvider
{ 
public:
  /**
   * Constructor
   *
   * \param mac the component carrier manager
   */
  SimpleUeCcmMacSapProvider (SimpleUeComponentCarrierManager* mac);

  // inherited from LteMacSapProvider
  virtual void TransmitPdu (LteMacSapProvider::TransmitPduParameters params);
  virtual void ReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params);

private:
  SimpleUeComponentCarrierManager* m_mac; ///< the component carrier manager
};

SimpleUeCcmMacSapProvider::SimpleUeCcmMacSapProvider (SimpleUeComponentCarrierManager* mac)
  : m_mac (mac)
{
}

void
SimpleUeCcmMacSapProvider::TransmitPdu (TransmitPduParameters params)
{
  m_mac->DoTransmitPdu (params);
}


void
SimpleUeCcmMacSapProvider::ReportBufferStatus (ReportBufferStatusParameters params)
{
  m_mac->DoReportBufferStatus (params);
}

///////////////////////////////////////////////////////////
// MAC SAP USER SAP forwarders
/////////////// ////////////////////////////////////////////

/// SimpleUeCcmMacSapUser class
class SimpleUeCcmMacSapUser : public LteMacSapUser
{ 
public:
  /**
   * Constructor
   *
   * \param mac the component carrier manager
   */
  SimpleUeCcmMacSapUser  (SimpleUeComponentCarrierManager* mac);

  // inherited from LteMacSapUser
  virtual void NotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters txOpParams);
  virtual void ReceivePdu (LteMacSapUser::ReceivePduParameters rxPduParams);
  virtual void NotifyHarqDeliveryFailure ();


private:
  SimpleUeComponentCarrierManager* m_mac; ///< the component carrier manager
};

SimpleUeCcmMacSapUser::SimpleUeCcmMacSapUser (SimpleUeComponentCarrierManager* mac)
  : m_mac (mac)
{
}

void
SimpleUeCcmMacSapUser::NotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters txOpParams)
{
  NS_LOG_INFO ("SimpleUeCcmMacSapUser::NotifyTxOpportunity for ccId:"<<(uint32_t)txOpParams.componentCarrierId);
  m_mac->DoNotifyTxOpportunity (txOpParams);
}


void
SimpleUeCcmMacSapUser::ReceivePdu (LteMacSapUser::ReceivePduParameters rxPduParams)
{
  m_mac->DoReceivePdu (rxPduParams);
}

void
SimpleUeCcmMacSapUser::NotifyHarqDeliveryFailure ()
{
  m_mac->DoNotifyHarqDeliveryFailure ();
}

//////////////////////////////////////////////////////////
// SimpleUeComponentCarrierManager methods
///////////////////////////////////////////////////////////

SimpleUeComponentCarrierManager::SimpleUeComponentCarrierManager ()
{
  NS_LOG_FUNCTION (this);
  m_ccmRrcSapProvider = new MemberLteUeCcmRrcSapProvider<SimpleUeComponentCarrierManager> (this);
  m_ccmMacSapUser = new SimpleUeCcmMacSapUser (this);
  m_ccmMacSapProvider = new SimpleUeCcmMacSapProvider (this);
}


SimpleUeComponentCarrierManager::~SimpleUeComponentCarrierManager ()
{
  NS_LOG_FUNCTION (this);
}


void
SimpleUeComponentCarrierManager::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ccmRrcSapProvider;
  delete m_ccmMacSapUser;
  delete m_ccmMacSapProvider;
}


TypeId
SimpleUeComponentCarrierManager::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::SimpleUeComponentCarrierManager")
    .SetParent<LteUeComponentCarrierManager> ()
    .SetGroupName("Lte")
    .AddConstructor<SimpleUeComponentCarrierManager> ()
    ;
  return tid;
}

LteMacSapProvider*
SimpleUeComponentCarrierManager::GetLteMacSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ccmMacSapProvider;
}


void
SimpleUeComponentCarrierManager::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  LteUeComponentCarrierManager::DoInitialize ();
}


void
SimpleUeComponentCarrierManager::DoReportUeMeas (uint16_t rnti,
                                                 LteRrcSap::MeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);
}


void
SimpleUeComponentCarrierManager::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);

  if (params.srcL2Id == 0)
    {
      NS_LOG_INFO ("Simple UE CCM forwarding LTE RLC PDU to MAC");
      std::map <uint8_t, LteMacSapProvider*>::iterator it =  m_macSapProvidersMap.find (params.componentCarrierId);
      NS_ABORT_MSG_IF (it == m_macSapProvidersMap.end (), "could not find Sap for ComponentCarrier "
                                                                         << (uint16_t) params.componentCarrierId);
      // with this algorithm all traffic is on Primary Carrier, is it?
      it->second->TransmitPdu (params);
    }
  else
    {
      NS_LOG_INFO ("Simple UE CCM forwarding Sidelink RLC PDU to MAC");
      std::map <uint8_t, LteMacSapProvider*>::iterator it =  m_macSapProvidersMap.find (params.componentCarrierId);
      NS_ABORT_MSG_IF (it == m_macSapProvidersMap.end (), "could not find Sap for Sidelink ComponentCarrier "
                                                                          << (uint16_t) params.componentCarrierId);
      // with this algorithm all traffic is on Primary Carrier, is it?
      it->second->TransmitPdu (params);
    }

}

void
SimpleUeComponentCarrierManager::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this);

  if (params.srcL2Id == 0)
    {
      NS_LOG_DEBUG ("BSR from RLC for LTE LCID = " << (uint16_t)params.lcid);
      std::map <uint8_t, LteMacSapProvider*>::iterator it =  m_macSapProvidersMap.find (0);
      NS_ABORT_MSG_IF (it == m_macSapProvidersMap.end (), "could not find Sap for ComponentCarrier");

      NS_LOG_DEBUG ("Size of component carrier LC map "<< m_componentCarrierLcMap.size());

      for (std::map <uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator ccLcMapIt = m_componentCarrierLcMap.begin();
                                                                   ccLcMapIt != m_componentCarrierLcMap.end(); ccLcMapIt++)
        {
          NS_LOG_DEBUG ("BSR from RLC for CC id = "<< (uint16_t)ccLcMapIt->first);
          std::map <uint8_t, LteMacSapProvider*>::iterator it = ccLcMapIt->second.find (params.lcid);
          if (it !=ccLcMapIt->second.end())
            {
              it->second->ReportBufferStatus (params);
            }
        }
    }
  else
    {
      std::map <uint8_t, LteMacSapProvider*>::iterator it =  m_macSapProvidersMap.find (0);
      NS_ABORT_MSG_IF (it == m_macSapProvidersMap.end (), "could not find Sap for Sidelink Component Carrier");

      NS_LOG_DEBUG ("Size of component carrier LC map "<< m_slComponentCarrierLcMap.size());

      // Replicate the BSR
      // Bug : 2861
      for (std::map <uint8_t, std::map<UeCcmSidelinkLcIdentifier,
                                       LteMacSapProvider*> >::iterator slCcLcMapIt = m_slComponentCarrierLcMap.begin();
                                                          slCcLcMapIt != m_slComponentCarrierLcMap.end(); slCcLcMapIt++)
        {
          UeCcmSidelinkLcIdentifier slLcIdentifier;
          slLcIdentifier.lcId = params.lcid;
          slLcIdentifier.srcL2Id = params.srcL2Id;
          slLcIdentifier.dstL2Id = params.dstL2Id;
          // Check if UE CCM have the LteMacSapProvider pointer
          // of the MAC to which the RLC with Sidelink LC
          // Identifiers is sending the BSR.
          // Note: If UE RRC will not call AddSlLc of
          // UE CCM, here it will not be able to find
          // the LteMacSapProvider pointer.
          std::map <UeCcmSidelinkLcIdentifier, LteMacSapProvider*>::iterator it = slCcLcMapIt->second.find (slLcIdentifier);
          if (it != slCcLcMapIt->second.end())
            {
              NS_LOG_DEBUG ("UE CCM sending Sidelink BSR from RLC with LCID = "
                            << (uint16_t)it->first.lcId
                            << " Source L2 ID = " << it->first.srcL2Id
                            << " Destination L2 ID = " << it->first.dstL2Id
                            << " to the MAC of CC id = " << (uint16_t)slCcLcMapIt->first);

              it->second->ReportBufferStatus (params);
            }
          else
            {
              NS_FATAL_ERROR ("UE CCM is unable to send the BSR to the MAC. Did you forget to"
                              " call AddSlLc of UE CCM in UE RRC");
            }
        }
    }
}

void 
SimpleUeComponentCarrierManager::DoNotifyHarqDeliveryFailure ()
{
 NS_LOG_FUNCTION (this);
}


void 
SimpleUeComponentCarrierManager::DoNotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters txOpParams)
{
  NS_LOG_FUNCTION (this);

  if (txOpParams.srcL2Id == 0)
    {
      std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = m_lcAttached.find (txOpParams.lcid);
      NS_ABORT_MSG_IF (lcidIt == m_lcAttached.end (), "could not find LCID" << (uint16_t) txOpParams.lcid);
      NS_LOG_DEBUG (this << " lcid = " << (uint32_t) txOpParams.lcid << " layer= "
                    << (uint16_t) txOpParams.layer << " componentCarierId "
                    << (uint16_t) txOpParams.componentCarrierId << " rnti " << txOpParams.rnti);

      NS_LOG_DEBUG (this << " MAC is asking component carrier id = " << (uint16_t) txOpParams.componentCarrierId
                    << " with lcid = " << (uint32_t) txOpParams.lcid << " to transmit "<< txOpParams.bytes<< " bytes");
      (*lcidIt).second->NotifyTxOpportunity (txOpParams);
    }
  else
    {
      UeCcmSidelinkLcIdentifier slLcIdentifier;
      slLcIdentifier.lcId = txOpParams.lcid;
      slLcIdentifier.srcL2Id = txOpParams.srcL2Id;
      slLcIdentifier.dstL2Id = txOpParams.dstL2Id;

      std::map <UeCcmSidelinkLcIdentifier, UeCcmLcInfo>::iterator slLcIdIt = m_slLcInfoMap.find (slLcIdentifier);

      NS_ABORT_MSG_IF (slLcIdIt == m_slLcInfoMap.end (), "cannot find SL LCID " << (uint16_t) txOpParams.lcid
                                        << ", srcL2Id " << txOpParams.srcL2Id << ", dstL2Id " << txOpParams.dstL2Id);

      NS_LOG_DEBUG (this << " SL lcid= " << (uint32_t) txOpParams.lcid << " layer= "
                    << (uint16_t) txOpParams.layer << " componentCarierId " << (uint16_t) txOpParams.componentCarrierId
                    << " rnti " << txOpParams.rnti);

      NS_LOG_DEBUG (this << " MAC is asking component carrier id = " << (uint16_t) txOpParams.componentCarrierId
                    <<" with SL lcid = " << (uint32_t) txOpParams.lcid << " to transmit "<< txOpParams.bytes<< " bytes");

      slLcIdIt->second.macSapUser->NotifyTxOpportunity(txOpParams);
    }

}
void
SimpleUeComponentCarrierManager::DoReceivePdu (LteMacSapUser::ReceivePduParameters rxPduParams)
{
  NS_LOG_FUNCTION (this);

  if (rxPduParams.srcL2Id == 0)
    {
      std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = m_lcAttached.find (rxPduParams.lcid);
      NS_ABORT_MSG_IF (lcidIt == m_lcAttached.end (), "could not find LCID" << (uint16_t) rxPduParams.lcid);
      if (lcidIt != m_lcAttached.end ())
        {
          (*lcidIt).second->ReceivePdu (rxPduParams);
        }
    }
  else
    {
      UeCcmSidelinkLcIdentifier slLcIdentifier;
      slLcIdentifier.lcId = rxPduParams.lcid;
      slLcIdentifier.srcL2Id = rxPduParams.srcL2Id;
      slLcIdentifier.dstL2Id = rxPduParams.dstL2Id;

      std::map <UeCcmSidelinkLcIdentifier, UeCcmLcInfo>::iterator slLcIdIt = m_slLcInfoMap.find (slLcIdentifier);

      NS_ABORT_MSG_IF (slLcIdIt == m_slLcInfoMap.end (), "cannot find SL LCID " << (uint16_t) rxPduParams.lcid
                                          << ", srcL2Id " << rxPduParams.srcL2Id << ", dstL2Id " << rxPduParams.dstL2Id);

      slLcIdIt->second.macSapUser->ReceivePdu (rxPduParams);
    }

}

///////////////////////////////////////////////////////////
// Ue CCM RRC SAP PROVIDER SAP forwarders
///////////////////////////////////////////////////////////
std::vector<uint16_t>
SimpleUeComponentCarrierManager::DoRemoveLc (uint8_t lcid)
{
  NS_LOG_FUNCTION (this << " lcId" << lcid);
  std::vector<uint16_t> res;
  NS_ABORT_MSG_IF (m_lcAttached.find (lcid) == m_lcAttached.end (), "could not find LCID " << lcid);
  m_lcAttached.erase (lcid);
  // send back all the configuration to the componentCarrier where we want to remove the Lc
  std::map<uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator it =  m_componentCarrierLcMap.begin ();
  while (it != m_componentCarrierLcMap.end ())
    {
      std::map<uint8_t, LteMacSapProvider*>::iterator lcToRemove = it->second.find (lcid);
      if (lcToRemove != it->second.end ())
        {
          res.insert (res.end (), it->first);
        }
      it++;
    }
  NS_ABORT_MSG_IF (res.size () == 0, "LCID " << lcid << " not found in the ComponentCarrierManager map");

  return res; 

}

void
SimpleUeComponentCarrierManager::DoReset ()
{
  NS_LOG_FUNCTION (this);
  // same semantics as LteUeMac::DoRest
  std::map<uint8_t, LteMacSapUser*>::iterator it = m_lcAttached.begin ();
  while (it != m_lcAttached.end ())
    {
      // don't delete CCCH
      if (it->first == 0)
        {
          ++it;
        }
      else
        {
          // note: use of postfix operator preserves validity of iterator
          m_lcAttached.erase (it++);
        }
    }
}

std::vector<LteUeCcmRrcSapProvider::LcsConfig>
SimpleUeComponentCarrierManager::DoAddLc (uint8_t lcId,  LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);
  std::vector<LteUeCcmRrcSapProvider::LcsConfig> res;
  std::map<uint8_t, LteMacSapUser*>::iterator it = m_lcAttached.find (lcId);
  NS_ABORT_MSG_IF (it != m_lcAttached.end (), "Warning, LCID " << lcId << " already exist");
  m_lcAttached.insert (std::pair<uint8_t, LteMacSapUser*> (lcId, msu));
  LteUeCcmRrcSapProvider::LcsConfig elem;
  std::map <uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator ccLcMapIt;
  for (uint8_t ncc = 0; ncc < m_noOfComponentCarriers; ncc++)
    {
      elem.componentCarrierId = ncc;
      elem.lcConfig = lcConfig;
      elem.msu = m_ccmMacSapUser;
      res.insert (res.end (), elem);
      
      ccLcMapIt = m_componentCarrierLcMap.find (ncc);
      if (ccLcMapIt != m_componentCarrierLcMap.end ())
        {
          ccLcMapIt->second.insert (std::pair <uint8_t, LteMacSapProvider*> (lcId, m_macSapProvidersMap.at (ncc)));
        }
      else
        {
          std::map<uint8_t, LteMacSapProvider*> empty;
          std::pair <std::map <uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator, bool>
          ret = m_componentCarrierLcMap.insert (std::pair <uint8_t,  std::map<uint8_t, LteMacSapProvider*> > (ncc, empty));
          NS_ABORT_MSG_IF (!ret.second, "element already present, ComponentCarrierId already exist");
          ccLcMapIt = m_componentCarrierLcMap.find (ncc);
          ccLcMapIt->second.insert (std::pair <uint8_t, LteMacSapProvider*> (lcId, m_macSapProvidersMap.at (ncc)));
        }
    }
  
  return res;  
}

std::vector<LteUeCcmRrcSapProvider::LcsConfig>
SimpleUeComponentCarrierManager::DoAddSlLc (uint8_t slLcId, uint32_t srcL2Id, uint32_t dstL2Id, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);
  std::vector<LteUeCcmRrcSapProvider::LcsConfig> res;
  UeCcmSidelinkLcIdentifier slLcIdentifier;
  slLcIdentifier.lcId = slLcId;
  slLcIdentifier.srcL2Id = srcL2Id;
  slLcIdentifier.dstL2Id = dstL2Id;

  NS_LOG_DEBUG ("UE CCM Adding Sl LcId = "<< (uint16_t) slLcId << " srcL2Id = "<< srcL2Id<< " dstL2Id = "<< dstL2Id);

  NS_ASSERT_MSG (m_slLcInfoMap.find (slLcIdentifier) == m_slLcInfoMap.end (), "cannot add channel because LCID " << slLcId
                                    << ", srcL2Id " << srcL2Id << ", dstL2Id " << dstL2Id << " is already present");

  UeCcmLcInfo lcInfo;
  lcInfo.lcConfig = lcConfig;
  lcInfo.macSapUser = msu;
  m_slLcInfoMap[slLcIdentifier] = lcInfo;

  LteUeCcmRrcSapProvider::LcsConfig elem;
  std::map <uint8_t, std::map<UeCcmSidelinkLcIdentifier, LteMacSapProvider*> >::iterator slCcLcMapIt;
  for (uint8_t ncc = 0; ncc < m_noOfComponentCarriers; ncc++)
    {
      elem.componentCarrierId = ncc;
      elem.lcConfig = lcConfig;
      elem.msu = m_ccmMacSapUser;
      res.insert (res.end (), elem);

      slCcLcMapIt = m_slComponentCarrierLcMap.find (ncc);
      if (slCcLcMapIt != m_slComponentCarrierLcMap.end ())
        {
          slCcLcMapIt->second.insert (std::pair <UeCcmSidelinkLcIdentifier, LteMacSapProvider*> (slLcIdentifier,
                                                                                           m_macSapProvidersMap.at (ncc)));
        }
      else
        {
          std::map<UeCcmSidelinkLcIdentifier, LteMacSapProvider*> empty;
          std::pair <std::map <uint8_t, std::map<UeCcmSidelinkLcIdentifier, LteMacSapProvider*> >::iterator, bool>
          ret = m_slComponentCarrierLcMap.insert (std::pair <uint8_t,  std::map<UeCcmSidelinkLcIdentifier,
                                                                                LteMacSapProvider*> > (ncc, empty));
          NS_ABORT_MSG_IF (!ret.second, "element already present, ComponentCarrierId already exist");
          slCcLcMapIt = m_slComponentCarrierLcMap.find (ncc);
          slCcLcMapIt->second.insert (std::pair <UeCcmSidelinkLcIdentifier, LteMacSapProvider*> (slLcIdentifier,
                                                                                           m_macSapProvidersMap.at (ncc)));
        }
    }
  return res;
}

LteMacSapUser*
SimpleUeComponentCarrierManager::DoConfigureSignalBearer (uint8_t lcid,  LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);
  std::map<uint8_t, LteMacSapUser*>::iterator it = m_lcAttached.find (lcid);
  //if the following assert is hit, e.g., in handover scenarios, it means
  // the DoRest function is not called by UE RRC
  NS_ABORT_MSG_IF (it != m_lcAttached.end (), "Warning, LCID " << (uint8_t) lcid << " already exist");

  m_lcAttached.insert (std::pair<uint8_t, LteMacSapUser*> (lcid, msu));

  std::map <uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator ccLcMapIt;
  for (uint8_t ncc = 0; ncc < m_noOfComponentCarriers; ncc++)
    {
      ccLcMapIt = m_componentCarrierLcMap.find (ncc);
      if (ccLcMapIt != m_componentCarrierLcMap.end ())
        {
          ccLcMapIt->second.insert (std::pair <uint8_t, LteMacSapProvider*> (lcid, m_macSapProvidersMap.at (ncc)));
        }
      else
        {
          std::map<uint8_t, LteMacSapProvider*> empty;
          std::pair <std::map <uint8_t, std::map<uint8_t, LteMacSapProvider*> >::iterator, bool>
          ret = m_componentCarrierLcMap.insert (std::pair <uint8_t,  std::map<uint8_t, LteMacSapProvider*> > (ncc, empty));
          NS_ABORT_MSG_IF (!ret.second, "element already present, ComponentCarrierId already existed");
          ccLcMapIt = m_componentCarrierLcMap.find (ncc);
          ccLcMapIt->second.insert (std::pair <uint8_t, LteMacSapProvider*> (lcid, m_macSapProvidersMap.at (ncc)));
        }

    }

  return m_ccmMacSapUser;
 } 

} // end of namespace ns3
