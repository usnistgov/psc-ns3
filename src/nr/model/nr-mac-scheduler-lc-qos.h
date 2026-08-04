// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_SCHEDULER_LC_QOS_H
#define NR_MAC_SCHEDULER_LC_QOS_H

#include "nr-mac-scheduler-lc-alg.h"

namespace ns3
{

/**
 *\ingroup scheduler
 *
 * \brief Algorithm for distributing the assigned bytes to the different
 * LCGs/LCs of a UE based on the resource type and the ERAB guaranteed bit rate
 *
 */
class NrMacSchedulerLcQos : public NrMacSchedulerLcAlgorithm
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerLcQos constructor
     */
    NrMacSchedulerLcQos();
    /**
     * \brief NrMacSchedulerLcQos deconstructor
     */
    ~NrMacSchedulerLcQos() override;

    /**
     * \brief Method to decide how to distribute the assigned bytes to the different LCs
     *        for the DL direction. This algorithm is based on the resource type and the
     *        guaranteed bitrate information of an LC.
     *        In particular, the operation is divided in 4 parts:
     *        1. The first part creates two lists, one with the GBR/DC-GBR active LCs that
     *        have their ERAB guaranteed bit rate requirements set and one with all the
     *        active LCs.
     *        2. In case there more than 1 GBR/DC-GBR active LCs that have their ERAB
     *        guaranteed bit rate requirements set, and their total requirements exceed the
     *        assigned bytes (tbs), then the algorithm assigns equally all the assigned
     *        bytes in RR fashion to these GBR/DC-GBR LCs.
     *        3. In case their total requirements are less than the assigned bytes, the
     *        algorithm assigns to each LC the minimum among the ERAB guaranteed bit rate
     *        and the RLC buffer size.
     *        4. The rest of the bytes, if any, are assigned in the rest of the LCs in RR
     *        fashion.
     *
     * \param ueLCG LCG of an UE
     * \param tbs TBS to divide between the LCG/LC
     * \return A vector of Assignation
     */
    std::vector<Assignation> AssignBytesToDlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                               uint32_t tbs,
                                               Time slotPeriod) const override;

    /**
     * \brief Method to decide how to distribute the assigned bytes to the different LCs
     *        for the UL direction. Due to the scheduler limitation the applied algorithm
     *        distributes bytes in a RR fashion (see NrMacSchedulerLcAlgorithm).
     * \param ueLCG LCG of an UE
     * \param tbs TBS to divide between the LCG/LC
     * \return A vector of Assignation
     */
    std::vector<Assignation> AssignBytesToUlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                               uint32_t tbs) const override;
};
} // namespace ns3

#endif /*NR_MAC_SCHEDULER_LC_QOS_H*/
