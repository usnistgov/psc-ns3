// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>
//
///

#ifndef NR_UE_COMPONENT_CARRIER_MANAGER_H
#define NR_UE_COMPONENT_CARRIER_MANAGER_H

#include "nr-mac-sap.h"
#include "nr-ue-ccm-rrc-sap.h"

#include <ns3/object.h>

#include <map>
#include <vector>

namespace ns3
{

class NrUeCcmRrcSapUser;
class NrUeCcmRrcSapProvider;

class NrMacSapUser;
class NrMacSapProvider;

/**
 * \brief The abstract base class of a Component Carrier Manager* for UE
  that operates using the component carrier manager SAP interface.
 *
 */
class NrUeComponentCarrierManager : public Object
{
  public:
    NrUeComponentCarrierManager();
    ~NrUeComponentCarrierManager() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Set the "user" part of the NrComponentCarrier Management SAP interface
     * that this UE component carrier manager will interact with.
     * \param s a reference to the "user" part of the interface, typically a
     *          member of an NrGnbRrc instance
     */
    virtual void SetNrCcmRrcSapUser(NrUeCcmRrcSapUser* s);

    /**
     * \brief Exports the "provider" part of the NrComponentCarrier Management SAP interface.
     * \return the reference to the "provider" part of the interface, typically to
     *         be kept by an NrUeRrc instance
     */
    virtual NrUeCcmRrcSapProvider* GetNrCcmRrcSapProvider();

    /**
     * \brief Returns the MAC sap provider interface that if forwarding calls to the
     * instance of the NrUeComponentCarrierManager.
     * \return the reference to the "provider" part of the interface
     */
    virtual NrMacSapProvider* GetNrMacSapProvider() = 0;

    /**
     * \brief Sets a pointer to SAP interface of MAC instance for the specified carrier.
     * \param componentCarrierId the component carrier id
     * \param sap the pointer to the sap interface
     * \return whether the settings of the sap provider was successful
     */
    bool SetComponentCarrierMacSapProviders(uint8_t componentCarrierId, NrMacSapProvider* sap);

    /**
     * \brief Sets number of component carriers that are supported by this UE.
     * \param noOfComponentCarriers number of component carriers
     */
    void SetNumberOfComponentCarriers(uint8_t noOfComponentCarriers);

  protected:
    // inherited from Object
    void DoDispose() override;

    NrUeCcmRrcSapUser* m_ccmRrcSapUser;         //!< Interface to the UE RRC instance.
    NrUeCcmRrcSapProvider* m_ccmRrcSapProvider; //!< Receive API calls from the UE RRC instance.

    std::map<uint8_t, NrMacSapUser*> m_lcAttached; //!< Map of pointers to SAP interfaces of the
                                                   //!< RLC instance of the flows of this UE.
    std::map<uint8_t, std::map<uint8_t, NrMacSapProvider*>>
        m_componentCarrierLcMap;     //!< Flow configuration per flow Id of this UE.
    uint8_t m_noOfComponentCarriers; //!< The number of component carriers that this UE can support.
    std::map<uint8_t, NrMacSapProvider*>
        m_macSapProvidersMap; //!< Map of pointers to SAP to interfaces of the MAC instance if the
                              //!< flows of this UE.

}; // end of class NrUeComponentCarrierManager

} // end of namespace ns3

#endif /* NR_UE_COMPONENT_CARRIER_MANAGER_H */
