// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Budiarto Herman <budiarto.herman@magister.fi>

#ifndef NR_HANDOVER_ALGORITHM_H
#define NR_HANDOVER_ALGORITHM_H

#include "nr-rrc-sap.h"

#include <ns3/object.h>

namespace ns3
{

class NrHandoverManagementSapUser;
class NrHandoverManagementSapProvider;

/**
 * \brief The abstract base class of a handover algorithm that operates using
 *        the Handover Management SAP interface.
 *
 * Handover algorithm receives measurement reports from an eNodeB RRC instance
 * and tells the eNodeB RRC instance when to do a handover.
 *
 * This class is an abstract class intended to be inherited by subclasses that
 * implement its virtual methods. By inheriting from this abstract class, the
 * subclasses gain the benefits of being compatible with the NrGnbNetDevice
 * class, being accessible using namespace-based access through ns-3 Config
 * subsystem, and being installed and configured by NrHelper class (see
 * NrHelper::SetHandoverAlgorithmType and
 * NrHelper::SetHandoverAlgorithmAttribute methods).
 *
 * The communication with the eNodeB RRC instance is done through the *Handover
 * Management SAP* interface. The handover algorithm instance corresponds to the
 * "provider" part of this interface, while the eNodeB RRC instance takes the
 * role of the "user" part. The following code skeleton establishes the
 * connection between both instances:
 *
 *     Ptr<NrGnbRrc> u = ...;
 *     Ptr<NrHandoverAlgorithm> p = ...;
 *     u->SetNrHandoverManagementSapProvider (p->GetNrHandoverManagementSapProvider ());
 *     p->SetNrHandoverManagementSapUser (u->GetNrHandoverManagementSapUser ());
 *
 * However, user rarely needs to use the above code, since it has already been
 * taken care by NrHelper::InstallGnbDevice.
 *
 * \sa NrHandoverManagementSapProvider, NrHandoverManagementSapUser
 */
class NrHandoverAlgorithm : public Object
{
  public:
    NrHandoverAlgorithm();
    ~NrHandoverAlgorithm() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Set the "user" part of the Handover Management SAP interface that
     *        this handover algorithm instance will interact with.
     * \param s a reference to the "user" part of the interface, typically a
     *          member of an NrGnbRrc instance
     */
    virtual void SetNrHandoverManagementSapUser(NrHandoverManagementSapUser* s) = 0;

    /**
     * \brief Export the "provider" part of the Handover Management SAP interface.
     * \return the reference to the "provider" part of the interface, typically to
     *         be kept by an NrGnbRrc instance
     */
    virtual NrHandoverManagementSapProvider* GetNrHandoverManagementSapProvider() = 0;

  protected:
    // inherited from Object
    void DoDispose() override;

    // HANDOVER MANAGEMENT SAP PROVIDER IMPLEMENTATION

    /**
     * \brief Implementation of NrHandoverManagementSapProvider::ReportUeMeas.
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE
     *             where the report originates from
     * \param measResults a single report of one measurement identity
     */
    virtual void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) = 0;

}; // end of class NrHandoverAlgorithm

} // end of namespace ns3

#endif /* NR_HANDOVER_ALGORITHM_H */
