// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-sched-sap.h"
#include "nr-phy-mac-common.h"

#include <ns3/nstime.h>

#include <memory>

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief Represent a DL Logical Channel of an UE
 *
 * The scheduler stores here the information that comes from BSR, arriving
 * from the gNB.
 *
 * Please use the unique ptr defined by the typedef LCPtr.
 *
 * \see Update
 * \see GetTotalSize
 */
class NrMacSchedulerLC
{
  public:
    /**
     * \brief NrMacSchedulerLC constructor
     * \param conf Configuration of the LC
     */
    NrMacSchedulerLC(const nr::LogicalChannelConfigListElement_s& conf);
    /**
     * \brief NrMacSchedulerLC default constructor (deletec)
     */
    NrMacSchedulerLC() = delete;
    /**
     * \brief NrMacSchedulerLC copy constructor (deleted)
     * \param o other instance
     */
    NrMacSchedulerLC(const NrMacSchedulerLC& o) = delete;
    /**
     * \brief Overwrite all the parameters with the one contained in the message
     * \param params the message received from the RLC layer, containing the information about the
     * queues
     */
    void Update(const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);
    /**
     * \brief Get the total size of the LC
     * \return the total size of the LC
     */
    uint32_t GetTotalSize() const;

    uint32_t m_id{0}; //!< ID of the LC
    uint32_t m_rlcTransmissionQueueSize{
        0}; //!< The current size of the new transmission queue in byte.
    uint16_t m_rlcTransmissionQueueHolDelay{0}; //!< Head of line delay of new transmissions in ms.
    uint16_t m_rlcRetransmissionHolDelay{0};    //!< Head of line delay of retransmissions in ms.
    uint32_t m_rlcRetransmissionQueueSize{
        0};                         //!< The current size of the retransmission queue in byte.
    uint16_t m_rlcStatusPduSize{0}; //!< The current size of the pending STATUS message in byte.

    Time m_delayBudget{Time::Min()}; //!< Delay budget of the flow
    double m_PER{0.0};               //!< PER of the flow
    uint8_t m_resourceType{0};       //!< the resource type associated with the QCI of the flow
    uint8_t m_qci{0};                //!< QoS Class Identifier of the flow
    uint8_t m_priority{0}; //!< the priority associated with the QCI of the flow 3GPP 23.203
    uint64_t m_eRabGuaranteedBitrateDl{UINT64_MAX}; //!< ERAB guaranteed bit rate DL
};

/**
 * \brief Unique pointer to an instance of NrMacSchedulerLC
 * \ingroup scheduler
 */
typedef std::unique_ptr<NrMacSchedulerLC> LCPtr;

/**
 * \ingroup scheduler
 * \brief Represent an UE LCG (can be DL or UL)
 *
 * A Logical Channel Group has an id (represented by m_id) and can contain
 * logical channels. The LC are stored inside an unordered map, indexed
 * by their ID.
 *
 * The LCs are inserted through the method Insert, and they can be updated with
 * a call to UpdateInfo. The update is different in DL and UL: in UL only the
 * sum of all components is available, while for DL there is a complete picture,
 * thanks to all the variables defined in
 * NrMacSchedSapProvider::SchedDlRlcBufferReqParameters.
 *
 * The general usage of this class is to insert each LC, and then update the
 * amount of bytes stored. The removal of an LC is still missing.
 *
 * For what regards UL, we currently support only one LC per LCG. This comes
 * from the fact that the BSR is reported for all the LCG, and the scheduler
 * has no way to identify which LCID contains bytes. So, even at the cost to
 * have a misrepresentation between the ID inside the UEs and the ID inside
 * the scheduler, we should make sure that each LCG in UL has only one LC.
 *
 * \see UpdateInfo
 */
class NrMacSchedulerLCG
{
  public:
    /**
     * \brief NrMacSchedulerLCG constructor
     * \param id The id of the LCG
     */
    NrMacSchedulerLCG(uint8_t id);
    /**
     * \brief NrMacSchedulerLCG copy constructor (deleted)
     * \param other other instance
     */
    NrMacSchedulerLCG(const NrMacSchedulerLCG& other) = delete;
    /**
     * \brief Check if the LCG contains the LC id specified
     * \param lcId LC ID to check for
     * \return true if the LCG contains the LC
     */
    bool Contains(uint8_t lcId) const;
    /**
     * \brief Get the number of LC currently in the LCG
     * \return the number of LC
     */
    uint32_t NumOfLC() const;
    /**
     * \brief Insert LC in the group
     * \param lc LC to insert
     * \return true if the insertion was fine (false in the case the LC already exists)
     */
    bool Insert(LCPtr&& lc);
    /**
     * \brief Update the LCG with a message coming from RLC in the gNB.
     * \param params message from gNB RLC layer.
     *
     * The method is able to update the LC using all the information such as
     * Retx queue, Tx queue, and the various delays.
     *
     * A call to NrMacSchedulerLC::Update is performed.
     */
    void UpdateInfo(const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);
    /**
     * \brief Update the LCG with just the LCG occupancy. Used in UL case when a BSR is received.
     * \param lcgQueueSize Sum of the size of all components in B
     *
     * Used in the UL case, in which only the sum of the components are
     * available. For the LC, only the value m_rlcTransmissionQueueSize is updated.
     *
     * For UL, only 1 LC per LCG is supported.
     */
    void UpdateInfo(uint32_t lcgQueueSize);
    /**
     * \brief Get the total size of the LCG
     * \return the total size of the LCG
     */
    uint32_t GetTotalSize() const;

    /**
     * \brief Get TotalSize Of LC
     * \param lcId LC ID
     * \return the total size of the LC
     */
    uint32_t GetTotalSizeOfLC(uint8_t lcId) const;

    /**
     * \brief Get a vector of LC ID
     * \return a vector with all the LC id present in this LCG
     */
    std::vector<uint8_t> GetLCId() const;

    /**
     * \brief Get a vector of the active LC IDs
     * \return a vector with all the LC ids in this LCG that have data
     */
    std::vector<uint8_t> GetActiveLCIds() const;

    /**
     * \brief Get the QoS Class Identifier of the flow
     * \param lcId LC ID
     * \return the QoS Class Identifier of the flow with lcId
     */
    uint8_t GetQci(uint8_t lcId) const;

    /**
     * \brief Get the LC Ptr for a specific LC ID
     * \param lcId LC ID
     * \return the LC Ptr for the LC ID
     */
    std::unique_ptr<NrMacSchedulerLC>& GetLC(uint8_t lcId);

    /**
     * \brief Inform the LCG of the assigned data to a LC id
     * \param lcId the LC id to which the data was assigned
     * \param size amount of assigned data
     * \param type String representing the type of allocation currently in act (DL or UL)
     */
    void AssignedData(uint8_t lcId, uint32_t size, std::string type);

    void ReleaseLC(uint8_t lcId);

  private:
    uint8_t m_id{0};                            //!< ID of the LCG
    std::unordered_map<uint8_t, LCPtr> m_lcMap; //!< Map between LC id and their pointer
};

/**
 * \brief LCGPtr unique pointer to a LCG
 * \ingroup scheduler
 */
typedef std::unique_ptr<NrMacSchedulerLCG> LCGPtr;

} // namespace ns3
