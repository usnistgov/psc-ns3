//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_IDEAL_MCS_CONTROLLER_H
#define NR_SL_IDEAL_MCS_CONTROLLER_H

#include "nr-sl-error-model.h"
#include "nr-sl-mcs-controller.h"
#include "nr-sl-phy-mac-common.h"

#include <ns3/object.h>

#include <limits>
#include <string>
#include <unordered_map>

namespace ns3
{

/**
 * \ingroup scheduler
 *
 * An ideal MCS controller that learns the most recent SNR of a previous transmission and,
 * if necessary, converts it to an equivalent wideband SNR (for the whole pool bandwidth). It
 * uses the wideband SNR and the currently configured error model to select the highest MCS
 * that will provide a BLER  greater than a target BLER.  It can also be configured with a margin
 * that will apply an additional offset to the SNR value provided to the error model.
 * In the absence of recent information, MCS 0 will be used.
 *
 * To suppress adjustment of broadcast or groupcast L2 IDs, this model uses a MaximumL2Id
 * attribute to indicate the maximum value of L2 ID that will be controlled by this controller.
 * For values greater than this maximum, this controller will not attempt any MCS changes.
 * The value is a 32 bit unsigned integer initialized to the value 200.
 */
class NrSlIdealMcsController : public NrSlMcsController
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlIdealMcsController constructor
     */
    NrSlIdealMcsController();

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

    /**
     * Callback function indicating that the scheduler is about to use the MCS that it
     * has cached for the destination.  This provides an opportunity to change the MCS
     * before it is queried.
     */
    void NotifyMcsQueryIndication(const SfnSf& sfn,
                                  uint32_t dstL2Id,
                                  uint32_t tbSize,
                                  uint32_t numRbs);

    /**
     * Remove history value(s) for a given L2 ID
     */
    void Flush(uint32_t dstL2Id);

  protected:
    // override base class methods
    void DoDispose() override;

  private:
    // override base class methods
    void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler) override;

    void NotifyRxPssch(std::string context, SlRxDataPacketTraceParams traceParams);
    void ConfigureTraces();

    /**
     * Information recorded every time
     * `NotifyRxPssch()` is triggered
     */
    struct TraceEntry
    {
        Time time;
        double sinrDb;
        uint8_t rv;
    };

    /**
     * Use binary search and heuristics to search for the highest MCS that provides
     * a BLER less than or equal to the target BLER.  Return zero if no such MCS is found.
     *
     * @param numerology The numerology
     * @param entry The trace entry that provides the SINR
     * @param tbSize The transport block size
     * @param numRbs The number of resource blocks
     * @return The highest MCS (if any) meeting the configured target BLER for the SNR
     */
    uint8_t BinarySearch(uint8_t numerology,
                         const TraceEntry& entry,
                         uint32_t tbSize,
                         uint32_t numRbs);

    Ptr<const NrSlErrorModel> m_slErrorModel;
    uint32_t m_srcL2Id{std::numeric_limits<uint32_t>::max()}; //!< source L2 id;
    uint32_t m_maximumL2Id{
        std::numeric_limits<uint32_t>::max()}; //!< maximum L2 id value to control;
    std::unordered_map<uint32_t, TraceEntry> m_traceMap;
    double m_tblerTarget; // `TargetBler` attribute
    double m_sinrMargin;  // `Margin` attribute
};

} // namespace ns3

#endif /* NR_SL_IDEAL_MCS_CONTROLLER_H */
