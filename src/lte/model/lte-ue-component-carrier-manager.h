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

#ifndef LTE_UE_COMPONENT_CARRIER_MANAGER_H
#define LTE_UE_COMPONENT_CARRIER_MANAGER_H

#include <ns3/object.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-ue-ccm-rrc-sap.h>
#include <ns3/lte-mac-sap.h>
#include <map>
#include <vector>

#define MIN_NO_CC 1
#define MAX_NO_CC 5 // this is the maximum number of carrier components allowed by 3GPP up to R13

namespace ns3 {


class LteUeCcmRrcSapUser;
class LteUeCcmRrcSapProvider;

class LteMacSapUser;
class LteMacSapProvider;


/**
 * \brief The abstract base class of a Component Carrier Manager* for UE 
  that operates using the component carrier manager SAP interface.
 *
 */
class LteUeComponentCarrierManager : public Object
{

public:
  LteUeComponentCarrierManager ();
  virtual ~LteUeComponentCarrierManager ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  /**
   * \brief Set the "user" part of the ComponentCarrier Management SAP interface
   * that this UE component carrier manager will interact with.
   * \param s a reference to the "user" part of the interface, typically a
   *          member of an LteEnbRrc instance
   */
  virtual void SetLteCcmRrcSapUser (LteUeCcmRrcSapUser* s);

  /**
   * \brief Exports the "provider" part of the ComponentCarrier Management SAP interface.
   * \return the reference to the "provider" part of the interface, typically to
   *         be kept by an LteUeRrc instance
   */
  virtual LteUeCcmRrcSapProvider* GetLteCcmRrcSapProvider ();
  
  /**
   * \brief Returns the MAC sap provider interface that if forwarding calls to the
   * instance of the LteUeComponentCarrierManager.
   * \return the reference to the "provider" part of the interface
   */
  virtual LteMacSapProvider* GetLteMacSapProvider () = 0;

  /**
   * \brief Sets a pointer to SAP interface of MAC instance for the specified carrier.
   * \param componentCarrierId the component carrier id
   * \param sap the pointer to the sap interface
   * \return whether the settings of the sap provider was successful
   */
  bool SetComponentCarrierMacSapProviders (uint8_t componentCarrierId, LteMacSapProvider* sap);

  /**
   * \brief Sets number of component carriers that are supported by this UE.
   * \param noOfComponentCarriers number of component carriers
   */
  void SetNumberOfComponentCarriers (uint8_t noOfComponentCarriers);

protected:

  // inherited from Object
  virtual void DoDispose ();

  LteUeCcmRrcSapUser* m_ccmRrcSapUser;//!< Interface to the UE RRC instance.
  LteUeCcmRrcSapProvider* m_ccmRrcSapProvider; //!< Receive API calls from the UE RRC instance.

  std::map<uint8_t, LteMacSapUser*> m_lcAttached; //!< Map of pointers to SAP interfaces of the RLC instance of the flows of this UE.
  std::map<uint8_t, std::map<uint8_t, LteMacSapProvider*> > m_componentCarrierLcMap; //!< Flow configuration per flow Id of this UE.
  uint8_t m_noOfComponentCarriers; //!< The number of component carriers that this UE can support.
  std::map <uint8_t, LteMacSapProvider*> m_macSapProvidersMap; //!< Map of pointers to SAP to interfaces of the MAC instance if the flows of this UE.

  /// UE Component Carrier Manager Sidelink Logical Channel Identifier
  struct UeCcmSidelinkLcIdentifier
  {
    uint8_t lcId; ///< Sidelink LCID
    uint32_t srcL2Id; ///< Source L2 ID
    uint32_t dstL2Id; ///< Destination L2 ID
  };

  /**
   * Less than operator
   *
   * \param l first SidelinkLcIdentifier
   * \param r second SidelinkLcIdentifier
   * \returns true if first SidelinkLcIdentifier parameter values are less than the second SidelinkLcIdentifier parameters"
   */
  friend bool operator < (const UeCcmSidelinkLcIdentifier &l, const UeCcmSidelinkLcIdentifier &r)
  {
    return l.lcId < r.lcId || (l.lcId == r.lcId && l.srcL2Id < r.srcL2Id) || (l.lcId == r.lcId && l.srcL2Id == r.srcL2Id && l.dstL2Id < r.dstL2Id);
  }
  /// Ue CCM LcInfo structure
  struct UeCcmLcInfo
  {
    LteUeCmacSapProvider::LogicalChannelConfig lcConfig; ///< logical channel config
    LteMacSapUser* macSapUser; ///< MAC SAP user
  };

  /**
   * \brief Sidelink logical channel configuration per
   * component carrier Id of this UE.
   * Key : Component carrier id
   * Mapped value: A map whose key is a struct
   * object of type UeCcmSidelinkLcIdentifier
   * and mapped value is a LteMacSapProvider pointer
   */
  std::map<uint8_t, std::map<UeCcmSidelinkLcIdentifier, LteMacSapProvider*> > m_slComponentCarrierLcMap;
  std::map <UeCcmSidelinkLcIdentifier, UeCcmLcInfo> m_slLcInfoMap; ///< Sidelink logical channel info map

}; // end of class LteUeComponentCarrierManager


} // end of namespace ns3


#endif /* LTE_UE_COMPONENT_CARRIER_MANAGER_H */
