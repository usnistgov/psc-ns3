// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_SCHEDULER_LC_RR_H
#define NR_MAC_SCHEDULER_LC_RR_H

#include "nr-mac-scheduler-lc-alg.h"

namespace ns3
{

/**
 *\ingroup scheduler
 *
 * \brief Default algorithm for distributing the assigned bytes to the different
 * LCGs/LCs of a UE in a Round Robin fashion
 *
 */
class NrMacSchedulerLcRR : public NrMacSchedulerLcAlgorithm
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerLcRR constructor
     */
    NrMacSchedulerLcRR();
    /**
     * \brief NrMacSchedulerLcRR deconstructor
     */
    ~NrMacSchedulerLcRR() override;

    /**
     * \brief Method to decide how to distribute the assigned bytes to the different LCs
     *        for the DL direction. In the RR case the method to distribute the bytes will
     *        be the same as in the UL direction.
     * \param ueLCG LCG of an UE
     * \param tbs TBS to divide between the LCG/LC
     * \return A vector of Assignation
     */
    std::vector<Assignation> AssignBytesToDlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                               uint32_t tbs,
                                               Time slotPeriod) const override;

    /**
     * \brief Method to decide how to distribute the assigned bytes to the different LCs
     *        for the UL direction. In the RR case the method to distribute the bytes will
     *        be the same as in the DL direction.
     * \param ueLCG LCG of an UE
     * \param tbs TBS to divide between the LCG/LC
     * \return A vector of Assignation
     */
    std::vector<Assignation> AssignBytesToUlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                               uint32_t tbs) const override;

    /**
     * \brief Method to decide how to distribute the assigned bytes to the different LCs
     * \param ueLCG LCG of an UE
     * \param tbs TBS to divide between the LCG/LC
     * \return A vector of Assignation
     */
  private:
    std::vector<Assignation> AssignBytesToLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                             uint32_t tbs) const;
};
} // namespace ns3

#endif /*NR_MAC_SCHEDULER_LC_RR_H*/
