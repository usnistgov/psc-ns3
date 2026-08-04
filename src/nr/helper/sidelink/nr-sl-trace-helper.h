/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_TRACE_HELPER_H
#define NR_SL_TRACE_HELPER_H

#include <ns3/callback.h>
#include <ns3/node-container.h>
#include <ns3/nr-sl-phy-mac-common.h>
#include <ns3/nr-sl-spectrum-phy.h>
#include <ns3/nr-sl-ue-mac-scheduler-default.h>
#include <ns3/nr-sl-ue-mac-scheduler.h>
#include <ns3/nr-sl-ue-mac.h>
#include <ns3/nstime.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/ptr.h>
#include <ns3/sfnsf.h>

#include <string>
#include <unordered_map>

namespace ns3
{

class NrSlUeMac;
class OutputStreamWrapper;

class NrSlTraceHelper
{
  public:
    /**
     * \brief NrSlTraceHelper
     */
    NrSlTraceHelper();
    /**
     * \brief Generate NrSlUePositionTrace.txt that polls UE positions
     *
     * By default, the trace file will be named "NrSlUePositionTrace.txt" unless
     * another trace file name is provided.
     *
     * Traces will be printed every second by default but the interval can be
     * configured to a different value.  If the user configures a recurrence interval
     * of zero, the trace will only be written at simulation time zero.
     *
     * \param n NodeContainer with the UE nodes to trace
     * \param interval Time interval between tracing positions
     * \param traceFileName Optional trace file name for output
     */
    void TraceUePositions(NodeContainer n,
                          Time interval = Seconds(1),
                          std::string traceFileName = "");
    /**
     * \brief Generate a trace file of the operation of the sensing algorithm
     *
     * \param mac The NrSlUeMac instance to trace
     * \param traceFileName Trace file name
     */
    void TraceSensingAlgorithm(Ptr<NrSlUeMac> mac, std::string traceFileName);
    /**
     * \brief Generate a trace file of the operation of the sensing algorithm
     *
     * A fatal error will occur if the Node does not have exactly one instance
     * of a NrSlUeMac.
     *
     * \param node The Node containing the NrSlUeMac instance to trace
     * \param traceFileName Trace file name
     */
    void TraceSensingAlgorithm(Ptr<Node> node, std::string traceFileName);
    /**
     * \brief Generate a trace file of the operation of the scheduling algorithm
     *
     * \param mac The NrSlUeMac instance to trace
     * \param traceFileName Trace file name
     */
    void TraceSchedulingAlgorithm(Ptr<NrSlUeMac> mac, std::string traceFileName);
    /**
     * \brief Generate a trace file of the operation of the scheduling algorithm
     *
     * A fatal error will occur if the Node does not have exactly one instance
     * of a NrSlUeMac.
     *
     * \param node The Node containing the NrSlUeMac instance to trace
     * \param traceFileName Trace file name
     */
    void TraceSchedulingAlgorithm(Ptr<Node> node, std::string traceFileName);
    /**
     * Return pointer to NrSlUeMac from a Ptr<Node>
     * There must be exactly one NrSlUeMac on the node.
     *
     * \param node The node pointer
     * \return A pointer to NrSlUeMac
     */
    Ptr<NrSlUeMac> GetNrSlUeMac(Ptr<Node> node) const;
    /**
     * Return pointer to NrSlUeMacScheduler from a Ptr<Node>
     * There must be exactly one NrSlUeMac on the node.
     *
     * \param node The node pointer
     * \return A pointer to NrSlUeMac
     */
    Ptr<NrSlUeMacScheduler> GetNrSlUeMacScheduler(Ptr<Node> node) const;
    /**
     * Return pointer to NrSlSpectrumPhy from a Ptr<Node>
     * There must be exactly one NrSlSpectrumPhy on the node.
     *
     * \param node The node pointer
     * \return A pointer to NrSlSpectrumPhy
     */
    Ptr<NrSlSpectrumPhy> GetNrSlSpectrumPhy(Ptr<Node> node) const;

  private:
    /**
     * \brief Write a trace line and schedule recurrence of this method
     *
     * \param n Node pointer to trace
     * \param interval Time interval between tracing events
     */
    void TraceUePosition(Ptr<Node> n, Time interval);
    /**
     * Trace sink for sensing algorithm
     * \param context context for this trace
     * \param report sensing report.
     * \param candidateResources candidates found by the algorithm.
     * \param sensingData sensing input data.
     * \param transmitHistory transmit history.
     */
    void TraceSensing(std::string context,
                      const struct NrSlUeMac::SensingTraceReport& report,
                      const std::list<SlResourceInfo>& candidateResources,
                      const std::list<SensingData>& sensingData,
                      const std::list<SfnSf>& transmitHistory);

    /**
     * Trace sink for scheduling report
     * \param report the SchedulingReport
     * \param candidateResources Candidates returned from sensing algorithm
     * \param params TransmissionParams used as input to sensing algorithm
     * \param publishedResources Published grants
     * \param existingGrants Existing grants
     * \param newGrant New grant
     */
    void TraceScheduling(
        std::string context,
        const struct NrSlUeMacSchedulerDefault::SchedulingReport& report,
        const std::list<SlResourceInfo>& candidateResources,
        const struct NrSlUeMac::NrSlTransmissionParams& params,
        const std::vector<SlGrantResource>& publishedResources,
        const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& existingGrants,
        const struct NrSlUeMacScheduler::GrantInfo& newGrant);

    std::unordered_map<std::string, Ptr<OutputStreamWrapper>>
        m_traceSensingStreams; //!< container for output streams
    std::unordered_map<std::string, Ptr<OutputStreamWrapper>>
        m_traceSchedulingStreams; //!< container for output streams

    Ptr<OutputStreamWrapper> m_uePositionStream;
};

} // namespace ns3

#endif // NR_SL_TRACE_HELPER_H
