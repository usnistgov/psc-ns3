// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-scheduler-tdma-rr.h"

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief Assign entire symbols in a maximum-rate fashion
 *
 * The UEs will be sorted by their MCS. Higher MCS will always be scheduled
 * before lower MCS, until they do not have any more bytes to transmit.
 *
 * \see NrMacSchedulerUeInfoMR
 */
class NrMacSchedulerTdmaMR : public NrMacSchedulerTdmaRR
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerTdmaMR constructor
     */
    NrMacSchedulerTdmaMR();

    /**
     * \brief ~NrMacSchedulerTdmaMR deconstructor
     */
    ~NrMacSchedulerTdmaMR() override
    {
    }

  protected:
    /**
     * \brief Create an UE representation of the type NrMacSchedulerUeInfoMR
     * \param params parameters
     * \return NrMacSchedulerUeInfoRR instance
     */
    std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

    /**
     * \brief Return the comparison function to sort DL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoMR::CompareUeWeightsDl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareDlFn() const override;

    /**
     * \brief Return the comparison function to sort UL UE according to the scheduler policy
     * \return a pointer to NrMacSchedulerUeInfoMR::CompareUeWeightsUl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareUlFn() const override;
};

} // namespace ns3
