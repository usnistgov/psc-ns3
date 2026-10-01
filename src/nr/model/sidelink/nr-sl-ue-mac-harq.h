// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_UE_MAC_HARQ_H
#define NR_SL_UE_MAC_HARQ_H

#include "nr-sl-phy-mac-common.h"

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>

#include <deque>
#include <map>
#include <optional>
#include <unordered_set>

namespace ns3
{

class PacketBurst;
class Packet;

/**
 * \ingroup MAC
 * \brief NR Sidelink HARQ Entity
 *
 * Objects of this class provide the Sidelink HARQ Entity defined in
 * Section 5.22.1.3.1 of TS 38.331.  Objects are responsible for associating
 * a TB with a sidelink process ID (HARQ ID) and for ensuring that the
 * total number of process IDs do not exceed configured maximums.
 * This object caches TBs until notified by positive HARQ feedback that
 * the TB can be freed.  To guard against the possibility that the MAC
 * allocates a HARQ ID but feedback is never received for the TB, a
 * timer is used to eventually deallocate the HARQ ID if not explicitly
 * deallocated by positive feedback (in the case of dynamic grants)
 * or by the scheduler (in the case of SPS grants).  The number of
 * HARQ/Sidelink processes can be configured by calling
 * \ref InitHarqBuffer(), which is the responsibility of a SL MAC.
 *
 * The standard describes that processes may be configured for transmission
 * of multiple MAC PDUs.  We interpret that mode of operation to correspond
 * to semi-persistent scheduling (SPS) grants.  If not configured for
 * transmission of multiple MAC PDUs, we interpret the mode of operation
 * to correspond to dynamic (or single PDU) grants.
 */
class NrSlUeMacHarq : public Object
{
  public:
    /**
     * \brief Get the type ID
     * \return the type ID of the class
     */
    static TypeId GetTypeId(void);

    /**
     * \brief NrSlUeMacHarq constructor
     */
    NrSlUeMacHarq();

    /**
     * \brief NrSlUeMacHarq destructor
     */
    virtual ~NrSlUeMacHarq();

    /**
     * This method initializes and sizes the NR SL HARQ process ID buffer
     * (see NrSlUeMacHarq#m_idBuffer) and NR SL HARQ packet buffer
     * (see NrSlUeMacHarq#m_pktBuffer). The size of these buffers
     * will be setto the maximum number of sidelink processes passed
     * through this method.
     *
     * \param maxSlProcessesMultiplePdu The maximum number of sidelink
     *        processes for multiple PDU grants for this HARQ entity.
     * \param maxSlProcesses The maximum number of sidelink processes for
     *        this HARQ entity.
     */
    void InitHarqBuffer(uint8_t maxSlProcessesMultiplePdu, uint8_t maxSlProcesses);

    /**
     * \brief Allocate and assign a HARQ Process ID to a destination
     *
     * This method will return an assigned HARQ process ID if one is available.
     * For SPS grants, the MAC is responsible for deallocating (and
     * reallocating) the Process ID when the SPS grant resources are
     * reselected.  For dynamic grants, the MAC does not have responsibility
     * for deallocation; the ID will be deallocated upon positive HARQ
     * feedback or else if it times out.
     *
     * \param dstL2Id The destination Layer 2 ID
     * \param multiplePdu Whether the process corresponds to a multiple PDU (i.e., SPS) grant
     * \param timeout The timeout value
     * \return The NR Sidelink HARQ ID assigned, or an empty value
     */
    std::optional<uint8_t> AllocateHarqProcessId(uint32_t dstL2Id, bool multiplePdu, Time timeout);

    /**
     * A previously allocated HARQ Process ID can be updated with information
     * about the maximum number of transmissions for the TB, whether HARQ
     * feedback is enabled, and the maximum TB size.  This information can
     * be used to make decisions about freeing resources and for consistency
     * checking.
     *
     * \param harqId HARQ Process ID to update
     * \param numTx Maximum number of transmissions of this TB
     * \param harqEnabled Whether HARQ feedback is enabled for this process
     * \param tbSize The maximum TB size
     */
    void UpdateHarqProcess(uint8_t harqId, uint32_t numTx, bool harqEnabled, uint32_t tbSize);

    /**
     * \brief Deallocate a previously allocated HARQ process ID
     *
     * If the HARQ ID is no longer allocated (e.g., due to a previous
     * timeout or HARQ acknowledgment), then this method does nothing.
     *
     * \param harqId The HARQ process ID to deallocate
     */
    void DeallocateHarqProcessId(uint8_t harqId);

    /**
     * Stop and restart the timer protecting the deallocation of the
     * HARQ process ID.  If the HARQ ID is not allocated, this method
     * will return false.
     * \param harqId the HARQ process ID
     * \param timeout The new expiration time (relative to now)
     * \return true if the timer was renewed
     */
    bool RenewHarqProcessIdTimer(uint8_t harqId, Time timeout);

    /**
     * \brief Get the number of available HARQ process IDs
     * \return The number of available HARQ process IDs
     */
    uint32_t GetNumAvailableHarqIds() const;

    /**
     * \brief Is the given HARQ ID available
     * \param harqId The HARQ process ID
     * \return returns true if the HARQ ID is available; otherwise false
     */
    bool IsHarqIdAvailable(uint8_t harqId) const;

    /**
     * \brief Add the packet to the Sidelink process buffer, which is identified
     *        using destination L2 ID, LC ID, and the HARQ ID.
     * \param dstL2Id The destination Layer 2 ID
     * \param lcId The logical channel ID
     * \param harqId The HARQ ID
     * \param pkt Packet
     */
    void AddPacket(uint32_t dstL2Id, uint8_t lcId, uint8_t harqId, Ptr<Packet> pkt);

    /**
     * \brief Get the packet burst from the Sidelink process buffer, which is
     *        identified using destination L2 ID and the HARQ ID.
     *
     * This method may return nullptr if no matching PacketBurst is found
     *
     * \param dstL2Id The destination Layer 2 ID
     * \param harqId The HARQ ID
     * \return The packet burst (if found) or a nullptr if not found
     */
    Ptr<PacketBurst> GetPacketBurst(uint32_t dstL2Id, uint8_t harqId);

    /**
     * \brief Receive NR Sidelink Harq feedback
     * \param harqInfo Sidelink HARQ info structure
     * \return Whether the feedback resulted in the ACK and removal of a TB
     */
    bool RecvHarqFeedback(SlHarqInfo harqInfo);

    /**
     * Flush the HARQ buffer associated with the HARQ process ID.
     *
     * The HARQ process ID remains allocated (i.e., \ref AddPacket()
     * may be called again).
     *
     * \param harqId HARQ process ID
     */
    void FlushHarqBuffer(uint8_t harqId);

    /**
     * TracedCallback signature for received HARQ feedback
     * \param [in] slHarqInfo received SlHarqInfo parameter
     */
    typedef void (*RxHarqFeedbackTracedCallback)(const SlHarqInfo& slHarqInfo);

    /**
     * TracedCallback signature for HARQ process allocate
     * \param [in] harqId HARQ process ID
     * \param [in] dstL2Id Destination L2 ID
     * \param [in] multiplePdu whether the process is for a multiple PDU grant
     * \param [in] timeout Timeout
     * \param [in] available Number of remaining available HARQ process IDs
     */
    typedef void (*AllocateTracedCallback)(uint8_t harqId,
                                           uint32_t dstL2Id,
                                           bool multiplePdu,
                                           Time timeout,
                                           std::size_t available);

    /**
     * TracedCallback signature for HARQ process deallocate
     * \param [in] harqId HARQ process ID
     * \param [in] available Number of remaining available HARQ process IDs
     */
    typedef void (*DeallocateTracedCallback)(uint8_t harqId, std::size_t available);

    /**
     * TracedCallback signature for request for packet burst (retransmission)
     * \param [in] dstL2Id Destination L2 ID
     * \param [in] harqId HARQ process ID
     */
    typedef void (*PacketBurstTracedCallback)(uint32_t dstL2Id, uint8_t harqId);

    /**
     * TracedCallback signature for HARQ timer expiry
     * \param [in] harqId HARQ process ID
     */
    typedef void (*TimeoutTracedCallback)(uint8_t harqId);

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void virtual DoDispose() override;

  private:
    /**
     * \brief struct to store the NR SL HARQ information
     */
    struct NrSlProcessInfo
    {
        Ptr<PacketBurst> pktBurst; //!< TB under HARQ
        // maintain list of LCs contained in this TB
        // used to signal HARQ failure to RLC handlers
        std::unordered_set<uint8_t> lcidList;                   //!< LC ID container
        uint16_t dstL2Id{std::numeric_limits<uint16_t>::max()}; //!< Destination L2 ID
        bool multiplePdu{false}; //!< Whether this process is for a multiple PDU grant
        EventId timer;           //!< Timer to expire process ID if not successfully ACKed
        bool allocated{false};   //!< Whether this process is allocated
        bool harqEnabled{false}; //!< Whether this process has HARQ feedback
        uint32_t numTx{0};       //!< Number of transmissions
        uint32_t maxNumTx{0};    //!< Maximum number of transmissions
        uint32_t tbSize{0};      //!< Maximum TB size in bytes
    };

    /**
     * Timer handler to prevent HARQ process from being bound to a transport
     * block that is never acknowledged.
     * \param harqId the HARQ process ID
     */
    void HarqProcessTimerExpiry(uint8_t harqId);

    /**
     * Re-initialize the HARQ buffer data structure.
     * \param harqId the HARQ process ID to reset
     */
    void ResetHarqBuffer(uint8_t harqId);

    uint8_t m_maxSlProcessesMultiplePdu{0}; //!< Maximum no. of SL processes for multiple PDU grants
    uint8_t m_maxSlProcesses{0};            //!< Maximum no. of SL processes
    std::vector<NrSlProcessInfo> m_pktBuffer; //!< NR SL HARQ packet buffer
    uint8_t m_numProcessesMultiplePdu{
        0};                         //!< Number of SL processes allocated for multiple PDU grants
    std::deque<uint8_t> m_idBuffer; //!< A container to store available HARQ/SL process IDs

    TracedCallback<const SlHarqInfo&> m_rxHarqFeedback; //!< Trace of SlHarqInfo
    TracedCallback<uint8_t, uint32_t, bool, Time, std::size_t>
        m_allocateTrace;                                    //!< Trace HARQ ID allocation
    TracedCallback<uint8_t, std::size_t> m_deallocateTrace; //! Trace HARQ ID deallocation
    TracedCallback<uint32_t, uint8_t> m_packetBurstTrace;   //! Trace PB requests
    TracedCallback<uint8_t> m_timeoutTrace;                 //! Trace HARQ timer expiry
};

} // namespace ns3

#endif /* NR_SL_UE_MAC_HARQ_H */
