// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-scheduler-ns3.h"
#include "nr-mac-scheduler-ue-info-rr.h"

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief UE representation for a maximum rate scheduler
 *
 * The class does not store anything more than the NrMacSchedulerUeInfo
 * base class. However, it provides functions to sort the UE based on their
 * maximum achievable rate.
 *
 * \see CompareUeWeightsDl
 */
class NrMacSchedulerUeInfoMR : public NrMacSchedulerUeInfo
{
  public:
    /**
     * \brief NrMacSchedulerUeInfoMR constructor
     * \param rnti RNTI of the UE
     * \param beamId Beam ID of the UE
     * \param fn A function that tells how many RB per RBG
     */
    NrMacSchedulerUeInfoMR(uint16_t rnti, BeamId beamId, const GetRbPerRbgFn& fn)
        : NrMacSchedulerUeInfo(rnti, beamId, fn)
    {
    }

    /**
     * \brief comparison function object (i.e. an object that satisfies the
     * requirements of Compare) which returns true if the first argument is less
     * than (i.e. is ordered before) the second.
     * \param lue Left UE
     * \param rue Right UE
     * \return true if the MCS of lue is greater than the MCS of rue
     *
     * The ordering is made by considering the MCS of the UE. The higher the MCS,
     * the higher the assigned resources until it has enough to transmit the data.
     */
    static bool CompareUeWeightsDl(const NrMacSchedulerNs3::UePtrAndBufferReq& lue,
                                   const NrMacSchedulerNs3::UePtrAndBufferReq& rue)
    {
        if (lue.first->m_dlMcs == rue.first->m_dlMcs)
        {
            return NrMacSchedulerUeInfoRR::CompareUeWeightsDl(lue, rue);
        }

        return (lue.first->m_dlMcs > rue.first->m_dlMcs);
    }

    /**
     * \brief comparison function object (i.e. an object that satisfies the
     * requirements of Compare) which returns true if the first argument is less
     * than (i.e. is ordered before) the second.
     * \param lue Left UE
     * \param rue Right UE
     * \return true if the MCS of lue is greater than the MCS of rue
     *
     * The ordering is made by considering the MCS of the UE. The higher the MCS,
     * the higher the assigned resources until it has enough to transmit the data.
     */
    static bool CompareUeWeightsUl(const NrMacSchedulerNs3::UePtrAndBufferReq& lue,
                                   const NrMacSchedulerNs3::UePtrAndBufferReq& rue)
    {
        if (lue.first->m_ulMcs == rue.first->m_ulMcs)
        {
            return NrMacSchedulerUeInfoRR::CompareUeWeightsUl(lue, rue);
        }

        return (lue.first->m_ulMcs > rue.first->m_ulMcs);
    }
};

} // namespace ns3
