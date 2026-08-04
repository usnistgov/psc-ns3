//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_MCS_CONTROLLER_H
#define NR_SL_MCS_CONTROLLER_H

#include "nr-sl-ue-mac.h"

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>

#include <optional>

namespace ns3
{

class NrSlUeMacSchedulerDefault;

/**
 * \ingroup scheduler
 *
 * \brief Per-grant parameters returned by an MCS controller.
 *
 * mcs is the selected MCS.  numTx is the per-grant number of transmissions
 * to reserve in SCI-1A.  If numTx is std::nullopt the scheduler uses the
 * pool-configured maxNumTx.  feasibleTbSize is populated only by
 * GetLargestFeasibleTb() and reports the largest TB size (in bytes) the
 * controller predicts deliverable at the queried subchannel-width budget.
 */
struct GrantParams
{
    std::optional<uint8_t> mcs;   //!< Selected MCS, or std::nullopt if no opinion
    std::optional<uint8_t> numTx; //!< Per-grant announced numTx; std::nullopt = pool maxNumTx
    std::optional<uint32_t>
        feasibleTbSize; //!< Largest deliverable TB size in bytes, or std::nullopt
};

/**
 * \ingroup scheduler
 *
 * This object is compatible with NrSlUeMacSchedulerDefault and any subclasses
 *
 * \brief Abstract interface for all MCS controllers
 */
class NrSlMcsController : public Object
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlMcsController constructor
     */
    NrSlMcsController();

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    virtual int64_t AssignStreams(int64_t stream);

    /**
     * Set the pointer to the NrSlUeMac
     * \param mac The pointer to the NrSlUeMac
     */
    void SetNrSlUeMac(Ptr<NrSlUeMac> mac);

    /**
     * Set the pointer to the NrSlUeMacScheduler
     * \param scheduler The pointer to the NrSlUeMacScheduler
     */
    void SetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler);

    /**
     * Get the per-grant parameters for the given destination.  Delegates to
     * the subclass DoGetGrantParams() and calls the GrantSelection trace
     * source.
     *
     * @param dstL2Id the destination L2 ID
     * @param castType the cast type for this logical channel
     * @param numRbs maximum allocation width in resource blocks (if
     *        provided); candidate MCS allocations for the TB are
     *        constrained to fit within this width
     * @param tbSize the transport block size in bytes (if known)
     * @return the selected GrantParams (mcs and numTx, each may be
     *         std::nullopt if the controller has no opinion on that axis)
     */
    GrantParams GetGrantParams(uint32_t dstL2Id,
                               SidelinkInfo::CastType castType,
                               std::optional<uint32_t> numRbs = std::nullopt,
                               std::optional<uint32_t> tbSize = std::nullopt);

    /**
     * Ask the controller for the largest TB size it predicts deliverable
     * at the queried subchannel-width budget, without targeting a specific
     * TB size.  Used by the scheduler when GetGrantParams() returned no
     * opinion for the requested tbSize and a fallback is needed to keep
     * data flowing rather than stall the LC.  Delegates to the subclass
     * DoGetLargestFeasibleTb().
     *
     * @param dstL2Id the destination L2 ID
     * @param castType the cast type for this logical channel
     * @param maxSubCh upper bound on subchannel count to consider
     * @return GrantParams with mcs and feasibleTbSize populated, or all
     *         std::nullopt if the controller has no feasible answer
     *         (e.g. no SINR history, or channel too weak for any MCS at
     *         any width up to maxSubCh).  numTx is always std::nullopt
     *         from this entry point.
     */
    GrantParams GetLargestFeasibleTb(uint32_t dstL2Id,
                                     SidelinkInfo::CastType castType,
                                     uint16_t maxSubCh);

    /**
     * Set the numerology of the associated resource pool.  If not set
     * explicitly, this object will perform a lazy fetch of the value
     * from the associated NrSlUeMac upon the first call to GetGrantParams().
     *
     * @param numerology the numerology value
     */
    void SetNumerology(uint16_t numerology);

    /**
     * TracedCallback signature for grant-parameter selection events.
     *
     * @param params the selected grant parameters (mcs and numTx)
     * @param dstL2Id the destination L2 ID
     * @param castType the cast type for this logical channel
     * @param numRbs maximum allocation width in resource blocks, as
     *        passed to GetGrantParams() (if provided)
     * @param tbSize the transport block size in bytes (if known)
     */
    typedef void (*GrantSelectionCallback)(GrantParams params,
                                           uint32_t dstL2Id,
                                           SidelinkInfo::CastType castType,
                                           std::optional<uint32_t> numRbs,
                                           std::optional<uint32_t> tbSize);

  protected:
    void DoDispose() override;

    Ptr<NrSlUeMac> m_mac;                       //!< Pointer to NrSlUeMac
    Ptr<NrSlUeMacSchedulerDefault> m_scheduler; //!< Pointer to NrSlUeMacScheduler
    std::optional<uint16_t> m_numerology;       //!< Numerology of the resource pool

  private:
    /**
     * Subclass implementation of grant-parameter selection.
     *
     * @param dstL2Id the destination L2 ID
     * @param castType the cast type for this logical channel
     * @param numRbs maximum allocation width in resource blocks (if
     *        provided); candidate MCS allocations for the TB are
     *        constrained to fit within this width
     * @param tbSize the transport block size in bytes (if known)
     * @return the selected GrantParams
     */
    virtual GrantParams DoGetGrantParams(uint32_t dstL2Id,
                                         SidelinkInfo::CastType castType,
                                         std::optional<uint32_t> numRbs,
                                         std::optional<uint32_t> tbSize) = 0;

    /**
     * Subclass implementation of the largest-feasible-TB query.  Default
     * returns a default-constructed GrantParams (all std::nullopt),
     * meaning the controller offers no opinion and the scheduler should
     * skip the slot.  Subclasses with an SINR/BLER predictor (Ideal,
     * OLLA) override this to walk (lSubch, mcs) combinations and return
     * the largest deliverable TB.
     *
     * @param dstL2Id the destination L2 ID
     * @param castType the cast type for this logical channel
     * @param maxSubCh upper bound on subchannel count to consider
     * @return GrantParams with mcs and feasibleTbSize populated, or all
     *         std::nullopt
     */
    virtual GrantParams DoGetLargestFeasibleTb(uint32_t dstL2Id,
                                               SidelinkInfo::CastType castType,
                                               uint16_t maxSubCh);

    /**
     * Virtual method to allow subclasses to apply post-conditions on setting the scheduler
     * pointer
     * \param scheduler The pointer to the NrSlUeMacScheduler
     */
    virtual void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler);

    TracedCallback<GrantParams,
                   uint32_t,
                   SidelinkInfo::CastType,
                   std::optional<uint32_t>,
                   std::optional<uint32_t>>
        m_grantSelectionTrace; //!< Trace source for grant-parameter selection events
};

} // namespace ns3

#endif /* NR_SL_MCS_CONTROLLER_H */
