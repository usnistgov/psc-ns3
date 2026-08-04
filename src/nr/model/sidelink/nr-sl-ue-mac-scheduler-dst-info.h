// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_UE_MAC_SCHEDULER_DST_INFO_H
#define NR_SL_UE_MAC_SCHEDULER_DST_INFO_H

#include "nr-sl-ue-mac-scheduler-lcg.h"

#include "ns3/nr-amc.h"

#include <map>
#include <memory>

namespace ns3
{

class NrSlUeMacSchedulerDstInfo;
/**
 * \brief Shared pointer to an instance of NrSlUeMacSchedulerDstInfo
 */
typedef std::shared_ptr<NrSlUeMacSchedulerDstInfo> DstPtr;

/**
 * \ingroup scheduler
 *
 * \brief The representation of a destination for any NR Sidelink UE MAC scheduler
 *
 * Basic representation for a destination inside any NR Sidelink UE MAC scheduler.
 * The class is responsible to store all the related field that can be used
 * by any NR Sidelink scheduler to schedule resources for a destination.
 */
class NrSlUeMacSchedulerDstInfo
{
  public:
    /**
     * \brief Create a new destination info
     *
     * \param dstL2Id the destination layer 2 id
     */
    NrSlUeMacSchedulerDstInfo(uint32_t dstL2Id);

    /**
     * \brief ~NrSlUeMacSchedulerDstInfo destructor
     */
    virtual ~NrSlUeMacSchedulerDstInfo();

    /**
     * \brief Get the NR Sidelink Logical Channel Group map
     *
     * \return The map storing the pointer to the NrSlUeMacSchedulerLCG
     * per logical channel group id
     */
    std::map<uint8_t, NrSlLCGPtr>& GetNrSlLCG();

    /**
     * \brief Insert the pointer to the NrSlUeMacSchedulerLCG object
     *
     *\param lcg the pointer to the NrSlUeMacSchedulerLCG object
     * \return The pointer to the map location where this method has inserted the
     * pointer to the NrSlUeMacSchedulerLCG object
     */
    NrSlLCGIt Insert(NrSlLCGPtr&& lcg);

    /**
     *  \bried Remove the pointer to the NrSlUeMacSchedulerLCG object
     *
     *  \param lcgid LCGID to be removed
     */
    void Remove(uint8_t lcgid);

    /**
     * \brief Get destiantion layer 2 id of this destination
     *
     * \return The destination layer 2 id
     */
    uint32_t GetDstL2Id() const;

    /**
     * \brief Set destination MCS
     *
     * \param mcs The MCS used for the destination
     */
    void SetDstMcs(uint8_t mcs);

    /**
     * \brief Get destination MCS
     *
     * \return The MCS used for the destination
     */
    uint8_t GetDstMcs() const;

  private:
    std::map<uint8_t, NrSlLCGPtr> m_nrSlLCG; //!< NR Sidelink LCG
    uint32_t m_dstL2Id{0};                   //!< The destination layer 2 id
    uint8_t m_mcs{0};                        //!< The MCS
};

} // namespace ns3

#endif /* NR_SL_UE_MAC_SCHEDULER_DST_INFO_H */
