// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>

#ifndef NR_GNB_COMPONENT_CARRIER_MANAGER_H
#define NR_GNB_COMPONENT_CARRIER_MANAGER_H

#include "nr-ccm-mac-sap.h"
#include "nr-ccm-rrc-sap.h"
#include "nr-gnb-cmac-sap.h"
#include "nr-gnb-rrc.h"
#include "nr-mac-sap.h"
#include "nr-rrc-sap.h"

#include <ns3/object.h>

#include <map>
#include <vector>

namespace ns3
{

class NrCcmRrcSapUser;
class NrCcmRrcSapProvider;
class NrMacSapUser;
class NrMacSapProvider;
class NrGnbCmacSapProvider;
class NrCcmMacSapProvider;

/**
 * \brief The class implements Component Carrier Manager (CCM) that operates
 * using the Component Carrier Manager SAP interfaces.
 *
 * CCM receives measurement reports from an eNode RRC instance and is forwarding
 * calls from RLC to MAC layer, and from MAC to RLC.
 *
 * This class is an abstract class intended to be inherited by subclasses that
 * will implement its virtual methods. The subclasses are compatible with the
 * NrGnbNetDevice class, and are accessible using namespace-based access
 * through ns-3 Config subsystem, and can be installed and configured by
 * NrHelper class.
 *
 * The communication with the eNodeB RRC instance is done through the *Component
 * Carrier Manager SAP* interface. The NrGnbComponentCarrierManager instance
 * corresponds to the "provider" part of this interface, while the eNodeB RRC
 * instance takes the role of the "user" part. The following code skeleton
 * establishes the connection between both instances:
 *
 * Ptr<NrGnbRrc> rrc = ...;
 * Ptr<NrComponentCarrierManager> ccmGnb = ...;
 * rrc->SetNrCcmRrcSapProvider (ccmGnb->GetNrCcmRrcSapProvider ());
 * ccmGnb->SetNrCcmRrcSapUser (rrc->GetNrCcmRrcSapUser ())
 *
 * Similarly, NrGnbComponentCarrierManager instance communicates with MAC, and
 * it takes the role of the "user".
 *
 * However, user rarely needs to use the above code, since it has already been
 * taken care by NrHelper::InstallGnbDevice.
 *
 * \sa NrCcmRrcSapUser, NrCcmRrcSapProvider, NrCcmMacSapUser, NrCcmMacSapProvider
 */

class NrGnbComponentCarrierManager : public Object
{
  public:
    NrGnbComponentCarrierManager();
    ~NrGnbComponentCarrierManager() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Set the "user" part of the NrComponentCarrier Management SAP interface that
     *        this NrComponentCarrier algorithm instance will interact with.
     * \param s a reference to the "user" part of the interface, typically a
     *          member of an NrGnbRrc instance
     */
    virtual void SetNrCcmRrcSapUser(NrCcmRrcSapUser* s);

    /**
     * \brief Export the "provider" part of the NrComponentCarrier Management SAP interface.
     * \return the reference to the "provider" part of the interface, typically to
     *         be kept by an NrGnbRlc instance
     */
    virtual NrCcmRrcSapProvider* GetNrCcmRrcSapProvider();

    /**
     * \brief This function returns a pointer to the NrCcmMacSapUser interface, which
     * is used by MAC to communicate to CCM when e.g. UL buffer status report is
     * received, or to notify CCM about PRB occupancy, and similar. Functions that are
     * specific for the communication between MAC and CCM.
     *
     * \returns NrCcmMacSapUser*
     */
    virtual NrCcmMacSapUser* GetNrCcmMacSapUser();

    /**
     * \brief Returns the pointer to the NrMacSapProvider interface, the
     * provider of MAC, which is this new architecture served by
     * NrGnbComponentCarrierManager object which will behave as a
     * proxy, and will forward calls between to MAC objects of
     * component carriers based on the logic implemented in the
     * specific component carrier manager.
     *
     * \returns NrMacSapProvider*
     */
    virtual NrMacSapProvider* GetNrMacSapProvider();

    /**
     * \brief Set NrMacSapProvider interface for the MAC object of
     * the specified component carrier.
     *
     * \param componentCarrierId component carrier ID
     * \param sap the MAC SAP provider
     * \returns true if successful
     */
    virtual bool SetMacSapProvider(uint8_t componentCarrierId, NrMacSapProvider* sap);

    /**
     * \brief Set NrCcmMacSapProvider interface for the MAC object of
     * the specified component carrier. Through this interface CCM communicates with
     * MAC, e.g. it notifies MAC of the specific carrier when to scheduler UL BSR.
     *
     * \param componentCarrierId component carrier ID
     * \param sap the MAC SAP provider
     * \returns true if successful
     */
    virtual bool SetCcmMacSapProviders(uint8_t componentCarrierId, NrCcmMacSapProvider* sap);

    /**
     * \brief Sets the total number of component carriers.
     * \param noOfComponentCarriers number of component carriers
     */
    virtual void SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers);

  protected:
    // inherited from Object
    void DoDispose() override;

    /**
     * \brief Implementation of ReportUeMeas.
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE
     *             where the report originates from
     * \param measResults a single report of one measurement identity
     */
    virtual void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) = 0;

    /**
     * \brief Structure to represent UE info
     */
    struct NrUeInfo
    {
        std::map<uint8_t, NrMacSapUser*>
            m_ueAttached; //!< Map from LCID to SAP of the RLC instance.
        std::map<uint8_t, NrGnbCmacSapProvider::LcInfo>
            m_rlcLcInstantiated; //!< Logical channel configuration per flow Id (rnti, lcid).
        uint8_t m_enabledComponentCarrier; //!< The number of enabled component carriers.
        uint8_t m_ueState;                 //!< RRC states of UE, e.g. CONNECTED_NORMALLY
    };

    std::map<uint16_t, NrUeInfo> m_ueInfo; //!< The map from RNTI to UE information.
    uint16_t m_noOfComponentCarriers; //!< The number component of carriers that are supported by
                                      //!< this eNb.
    // pointer to RRC object for direct function calls, e.g. when CCM needs to obtain
    // a pointer to RLC object of a specific flow
    Ptr<NrGnbRrc> m_rrc; //!< A pointer to the RRC instance of this eNb.

    /*
     * This interface is used to receive API calls from the RLC instance that through
     * NrMacSapProvider interface. The component carrier manager acts a proxy. This means that all
     * RLC instances will see as in previous architecture the NrMacSapProvider interface, but the
     * actual provider in new architecture will be some of child classes of
     * NrGnbComponentCarrierManager. So, NrGnbComponentCarrierManager class will receive function
     * calls that are meant for MAC, and will forward them to the MAC of the component carriers
     * based on the logic implemented in NrComponentCarrierManager. This attribute will be
     * initialized by using class that implements NrMacSapProvider interface and class that
     * implements NrGnbComponentCarrierManager base class e.g.:GnbMacMemberNrMacSapProvider
     * <NrGnbComponentCarrierManagerImpl>
     */
    NrMacSapProvider* m_macSapProvider; //!< A pointer to main SAP interface of the MAC instance,
                                        //!< which is in this case handled by CCM.
    // This map is initialized in NrHelper when the Component Carrier Manager is initialized,
    // contains component carrier id and a pointer to the corresponding NrMacSapProvider interface
    // of the MAC instance
    std::map<uint8_t, NrMacSapProvider*>
        m_macSapProvidersMap; //!< A map of pointers to real SAP interfaces of MAC instances.
    // This map contains pointers to NrCcmMacSapProvider interfaces of the
    // MAC instances. NrCcmMacSapProvider is new interface added for the
    // communication between component carrier manager and MAC instance,
    // to allow CCM to control UL buffer status reporting, and forwarding to
    // schedulers. Before adding carrier aggregation to NR module, MAC was
    // directly forwarding UL buffer status report to scheduler. Now is this
    // done through CCM, which decides to which MAC scheduler to forward UL BSR.
    std::map<uint8_t, NrCcmMacSapProvider*>
        m_ccmMacSapProviderMap; //!< A map of pointers to the SAP interfaces of CCM instance that
                                //!< provides the  CCM specific functionalities to MAC, i.e.
                                //!< ReportMacCeToScheduler.
    NrCcmMacSapUser*
        m_ccmMacSapUser; //!< NrCcmMacSapUser is extended version of NrMacSapUser interface.
                         //!< Contains functions that allow reporting of UL BSR from MAC to CCM.
    NrCcmRrcSapUser* m_ccmRrcSapUser; //!< A pointer to SAP interface of RRC instance, i.e. to
                                      //!< configure measurements reporting for CCM.
    NrCcmRrcSapProvider*
        m_ccmRrcSapProvider; //!< A pointer to the SAP interface of the CCM instance to receive API
                             //!< calls from the eNodeB RRC instance.

}; // end of class NrGnbComponentCarrierManager

} // end of namespace ns3

#endif /* NR_GNB_COMPONENT_CARRIER_MANAGER_H */
