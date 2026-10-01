// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-scheduler-ofdma.h"

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief Assign frequencies in a round-robin fashion
 *
 * Each UE will receive a proportional number of frequencies, with a fixed
 * number of symbols depending on the requirements of each beam. With \f$n\f$ UE,
 * each one will receive:
 *
 * \f$ freq_{i} = \frac{totFreq}{n} \f$
 *
 * If \f$ n > totFreq \f$, then there will be UEs which will not have any
 * resource assigned. The class does not remember the UEs which did not get
 * any resource in the previous slot, so this opens the door to a possible
 * starvation.
 *
 * \see NrMacSchedulerUeInfoRR
 */
class NrMacSchedulerOfdmaRR : public NrMacSchedulerOfdma
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerTdmaRR constructor
     */
    NrMacSchedulerOfdmaRR();

    /**
     * \brief ~NrMacSchedulerTdmaRR deconstructor
     */
    ~NrMacSchedulerOfdmaRR() override
    {
    }

  protected:
    /**
     * \brief Create an UE representation of the type NrMacSchedulerUeInfoRR
     * \param params parameters
     * \return NrMacSchedulerUeInfoRR instance
     */
    std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

    /**
     * \brief Return the comparison function to sort DL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoRR::CompareUeWeightsDl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareDlFn() const override;

    /**
     * \brief Return the comparison function to sort UL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoRR::CompareUeWeightsUl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareUlFn() const override;

    /**
     * \brief Update the UE representation after a symbol (DL) has been assigned to it
     * \param ue UE to which a symbol has been assigned
     * \param assigned the amount of resources assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * Update DL metrics by calling NrMacSchedulerUeInfoRR::UpdateDlMetric
     */
    void AssignedDlResources(const UePtrAndBufferReq& ue,
                             const FTResources& assigned,
                             const FTResources& totAssigned) const override;

    /**
     * \brief Update the UE representation after a symbol (DL) has been assigned to it
     * \param ue UE to which a symbol has been assigned
     * \param assigned the amount of resources assigned
     * \param totAssigned the total amount of resources assigned in the slot
     *
     * Update DL metrics by calling NrMacSchedulerUeInfoRR::UpdateUlMetric
     */
    void AssignedUlResources(const UePtrAndBufferReq& ue,
                             const FTResources& assigned,
                             const FTResources& totAssigned) const override;

    // RR is a simple scheduler: it doesn't do anything in the next
    // inherited calls.
    void NotAssignedDlResources(const UePtrAndBufferReq& ue,
                                const FTResources& notAssigned,
                                const FTResources& totalAssigned) const override
    {
    }

    void NotAssignedUlResources(const UePtrAndBufferReq& ue,
                                const FTResources& notAssigned,
                                const FTResources& totalAssigned) const override
    {
    }

    void BeforeDlSched(const UePtrAndBufferReq& ue,
                       const FTResources& assignableInIteration) const override
    {
    }

    void BeforeUlSched(const UePtrAndBufferReq& ue,
                       const FTResources& assignableInIteration) const override
    {
    }
};

} // namespace ns3
