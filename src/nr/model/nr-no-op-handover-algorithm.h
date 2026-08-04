// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Budiarto Herman <budiarto.herman@magister.fi>

#ifndef NO_OP_HANDOVER_ALGORITHM_H
#define NO_OP_HANDOVER_ALGORITHM_H

#include "nr-handover-algorithm.h"
#include "nr-handover-management-sap.h"
#include "nr-rrc-sap.h"

namespace ns3
{

/**
 * \brief Handover algorithm implementation which simply does nothing.
 *
 * Selecting this handover algorithm is equivalent to disabling automatic
 * triggering of handover. This is the default choice.
 *
 * To enable automatic handover, please select another handover algorithm, i.e.,
 * another child class of NrHandoverAlgorithm.
 */
class NrNoOpHandoverAlgorithm : public NrHandoverAlgorithm
{
  public:
    /// Creates a No-op handover algorithm instance.
    NrNoOpHandoverAlgorithm();

    ~NrNoOpHandoverAlgorithm() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    // inherited from NrHandoverAlgorithm
    void SetNrHandoverManagementSapUser(NrHandoverManagementSapUser* s) override;
    NrHandoverManagementSapProvider* GetNrHandoverManagementSapProvider() override;

    /// let the forwarder class access the protected and private members
    friend class MemberNrHandoverManagementSapProvider<NrNoOpHandoverAlgorithm>;

  protected:
    // inherited from Object
    void DoInitialize() override;
    void DoDispose() override;

    // inherited from NrHandoverAlgorithm as a Handover Management SAP implementation
    void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) override;

  private:
    /// Interface to the eNodeB RRC instance.
    NrHandoverManagementSapUser* m_handoverManagementSapUser;
    /// Receive API calls from the eNodeB RRC instance.
    NrHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class NrNoOpHandoverAlgorithm

} // end of namespace ns3

#endif /* NO_OP_HANDOVER_ALGORITHM_H */
