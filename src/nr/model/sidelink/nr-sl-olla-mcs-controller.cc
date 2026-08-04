// SPDX-License-Identifier: NIST-Software

#include "nr-sl-olla-mcs-controller.h"

#include "nr-sl-effective-bler-calculator.h"
#include "nr-sl-epa-error-model.h"
#include "nr-sl-spectrum-phy.h"
#include "nr-sl-ue-mac-scheduler-default.h"
#include "nr-sl-ue-phy.h"

#include "ns3/fatal-error.h"
#include "ns3/object.h"
#include "ns3/pointer.h"
#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/double.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nr-mcs-tables.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <algorithm>
#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlOllaMcsController");

NS_OBJECT_ENSURE_REGISTERED(NrSlOllaMcsController);

TypeId
NrSlOllaMcsController::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlOllaMcsController")
            .SetParent<NrSlMcsController>()
            .AddConstructor<NrSlOllaMcsController>()
            .SetGroupName("nr")
            .AddAttribute("TargetBler",
                          "The block error rate this controller will attempt to center the MCS on",
                          DoubleValue(0.1),
                          MakeDoubleAccessor(&NrSlOllaMcsController::m_targetBler),
                          MakeDoubleChecker<double>())
            .AddAttribute("ErrorModel",
                          "Error model used by the controller to predict BLER when "
                          "selecting an MCS.",
                          PointerValue(CreateObject<NrSlEpaErrorModel>()),
                          MakePointerAccessor(&NrSlOllaMcsController::m_slErrorModel),
                          MakePointerChecker<NrSlErrorModel>())
            .AddAttribute("PenaltyStep",
                          "Amount to reduce the estimated SINR when a NACK is received."
                          "The ACK step is derived from this value",
                          DoubleValue(1.0),
                          MakeDoubleAccessor(&NrSlOllaMcsController::GetNackPenaltyStep,
                                             &NrSlOllaMcsController::SetNackPenaltyStep),
                          MakeDoubleChecker<double>())
            .AddAttribute("MinSinrDelta",
                          "Lower bound (clamping value) on the OLLA offset delta (dB).",
                          DoubleValue(-15.0),
                          MakeDoubleAccessor(&NrSlOllaMcsController::m_minSinrOffsetDelta),
                          MakeDoubleChecker<double>())
            .AddAttribute("MaxSinrDelta",
                          "Upper bound (clamping value) on the OLLA offset delta (dB).",
                          DoubleValue(15.0),
                          MakeDoubleAccessor(&NrSlOllaMcsController::m_maxSinrOffsetDelta),
                          MakeDoubleChecker<double>())
            .AddAttribute("RxPsschDelay",
                          "Sets the delay between `NotifyRxPssch()` being called and the results "
                          "being persisted. Converted to Milliseconds",
                          StringValue("ns3::ConstantRandomVariable[Constant=4]"),
                          MakePointerAccessor(&NrSlOllaMcsController::m_rxPsschDelay),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("CqiPeriod",
                          "Period (in milliseconds) between CQI report updates. "
                          "A value of 0 means every SINR sample is used immediately "
                          "(no gating). Typical periodic CQI values are 20-40 ms "
                          "(TS 38.331 CSI-ReportConfig periodicities).",
                          StringValue("ns3::ConstantRandomVariable[Constant=100]"),
                          MakePointerAccessor(&NrSlOllaMcsController::m_cqiPeriod),
                          MakePointerChecker<RandomVariableStream>())
            .AddTraceSource("TrialResult",
                            "The outcome of a HARQ feedback trial.",
                            MakeTraceSourceAccessor(&NrSlOllaMcsController::m_trialResultTrace),
                            "ns3::NrSlOllaMcsController::TrialResultCallback")
            .AddTraceSource("SinrEstimate",
                            "Reports the SINR estimate used for MCS selection",
                            MakeTraceSourceAccessor(&NrSlOllaMcsController::m_sinrEstimateTrace),
                            "ns3::NrSlOllaMcsController::SinrEstimateTracedCallback")
            .AddAttribute("MaximumL2Id",
                          "The highest destination L2 ID value to control",
                          UintegerValue(200u),
                          MakeUintegerAccessor(&NrSlOllaMcsController::m_maximumL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("EnableClamping",
                          "When true, stop adjusting the SINR offset when the selected "
                          "MCS is already at the minimum (0) or maximum (28) boundary",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NrSlOllaMcsController::m_enableClamping),
                          MakeBooleanChecker())
            .AddAttribute("MedianFilterSize",
                          "Length of the per-destination ring buffer of raw SINR "
                          "samples over which the receiver-side filter (max) is "
                          "evaluated.  A value of 1 disables filtering (the most "
                          "recent raw sample is used as-is).",
                          UintegerValue(5),
                          MakeUintegerAccessor(&NrSlOllaMcsController::m_medianFilterSize),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("HarqAwareTarget",
                          "If true, the TargetBler applies to the post-HARQ effective "
                          "BLER of up to maxNumTx soft-combined transmissions using "
                          "the RV sequence {0, 2, 3, 1}.  When DynamicNumTx is false, "
                          "the predictor accumulates the joint failure probability "
                          "across maxNumTx transmissions before evaluating the "
                          "target.  The OLLA offset adapter is also driven from "
                          "per-TB-completion feedback (HarqTbCompletion), so the "
                          "predictor and the closed-loop offset share the same "
                          "definition of TargetBler.  If false (default), the target "
                          "applies to first-transmission BLER and the offset is "
                          "driven by per-tx HARQ feedback.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlOllaMcsController::m_harqAwareTarget),
                          MakeBooleanChecker())
            .AddAttribute("DynamicNumTx",
                          "If true, the controller jointly selects (MCS, numTx) to "
                          "minimize the announced SCI-1A reservation N_sc * numTx "
                          "subject to the target BLER, walking numTx from 1 to "
                          "maxNumTx.  If false (default), numTx is left unspecified "
                          "and the scheduler reserves the pool's configured maxNumTx.  "
                          "HarqAwareTarget controls whether each numTx query uses "
                          "soft-combined history or first-tx BLER.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlOllaMcsController::m_dynamicNumTx),
                          MakeBooleanChecker());
    return tid;
}

NrSlOllaMcsController::NrSlOllaMcsController()
{
    NS_LOG_FUNCTION(this);
}

NrSlOllaMcsController::~NrSlOllaMcsController()
{
    NS_LOG_FUNCTION(this);
    for (auto& [_, events] : m_saveSinrEvents)
    {
        for (auto& event : events)
        {
            Simulator::Cancel(event);
        }
    }
    m_saveSinrEvents.clear();
    for (auto& [_, event] : m_cqiUpdateEvents)
    {
        Simulator::Cancel(event);
    }
    m_cqiUpdateEvents.clear();
}

void
NrSlOllaMcsController::DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    m_srcL2Id = scheduler->GetMac()->GetImsi(); // Helper sets srcL2Id to the IMSI
    NS_LOG_DEBUG("Source L2 Id set to " << m_srcL2Id);
    Simulator::ScheduleWithContext(m_srcL2Id,
                                   Seconds(0),
                                   &NrSlOllaMcsController::ConfigureTraces,
                                   this);
}

GrantParams
NrSlOllaMcsController::DoGetGrantParams(uint32_t dstL2Id,
                                        [[maybe_unused]] SidelinkInfo::CastType castType,
                                        std::optional<uint32_t> numRbs,
                                        std::optional<uint32_t> tbSize)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    if (dstL2Id > m_maximumL2Id)
    {
        return {};
    }
    std::optional<uint8_t> bestMcs;
    const auto& it = m_filteredSinrTraces.find(dstL2Id);
    if (it == m_filteredSinrTraces.end())
    {
        NS_LOG_INFO("No history for destination; setting MCS 0 in scheduler for " << dstL2Id);
        bestMcs = 0;
        return {bestMcs, std::nullopt};
    }

    const auto& entry = it->second;
    // filteredSinrDb is the wideband SINR (normalized to full band occupancy),
    // computed as the max over the destination's raw-sample ring buffer.
    const auto filteredSinrDb = entry.sinrDb;
    // Pool-RB count is a pool-config constant; query the MAC at use time.
    const auto poolRbs = static_cast<uint32_t>(m_mac->GetTotalSubCh() * m_mac->GetNrSlSubChSize());

    auto sinrOffsetDelta{0.0};
    if (const auto& deltaIt = m_sinrDeltas.find(dstL2Id); deltaIt != m_sinrDeltas.end())
    {
        sinrOffsetDelta = deltaIt->second.sinrOffsetDelta;
    }

    // Base SINR estimate is the wideband filtered SINR plus the OLLA offset.
    // Offset drift is bounded (clamped) upon update in RxHarqFeedback; the
    // channel estimate itself passes through unmodified.
    const auto baseEstimate = filteredSinrDb + sinrOffsetDelta;

    // Look up the unfiltered wideband SINR (latest channel observation,
    // not gated by CqiPeriod)
    const auto unfilteredIt = m_sinrTraces.find(dstL2Id);
    const auto unfilteredSinrDb =
        (unfilteredIt != m_sinrTraces.end()) ? unfilteredIt->second.sinrDb : filteredSinrDb;

    // All traced values are in the wideband reference frame (full band occupancy)
    m_sinrEstimateTrace(dstL2Id, unfilteredSinrDb, filteredSinrDb, sinrOffsetDelta, baseEstimate);

    NS_ASSERT_MSG(m_slErrorModel, "Error: ErrorModel is not set");

    // Iterate over candidate MCS values and de-normalize the wideband SINR
    // estimate to the sub-band SINR that the PHY would observe for each
    // candidate allocation.  Higher MCS needs fewer subchannels for the
    // same TB size, concentrating transmit power into fewer RBs and raising
    // the effective sub-band SINR.  The de-normalization formula is:
    //   sub-band SINR = wideband SINR + 10*log10(poolRBs / candidateRBs)
    // This matches the PHY receiver, which measures SINR over the RBs
    // actually used by the transmission.
    //
    // When the caller provides numRbs, it is a width budget: candidates
    // whose minimum allocation for the TB exceeds it are not considered.
    // Grant creation passes the current search width; SPS publication
    // passes the grant's fixed allocation width.
    std::optional<uint8_t> bestNumTx;
    if (tbSize.has_value())
    {
        constexpr uint8_t MCS_TABLE = 1;
        auto slotInfo = CreateSlotInfo();
        auto pool = m_mac->GetTxPool();
        uint16_t subChSize = slotInfo.slSubchannelSize;
        uint16_t maxSubCh = m_mac->GetTotalSubCh();
        if (numRbs.has_value())
        {
            auto budgetSubCh = static_cast<uint16_t>(numRbs.value() / subChSize);
            NS_ASSERT_MSG(budgetSubCh > 0,
                          "Width budget " << numRbs.value() << " RBs is below one subchannel");
            maxSubCh = std::min(maxSubCh, budgetSubCh);
        }
        static const std::vector<uint8_t> rvSeq{0, 2, 3, 1};
        uint8_t poolMaxNumTx = m_mac->GetSlMaxTxTransNumPssch();

        if (!m_dynamicNumTx)
        {
            for (uint8_t mcs = 0; mcs <= 28; mcs++)
            {
                auto minSubCh =
                    pool->GetMinSubchannels(slotInfo, mcs, tbSize.value(), maxSubCh, MCS_TABLE);
                if (!minSubCh.has_value())
                {
                    continue;
                }
                uint32_t candidateRbs = minSubCh.value() * subChSize;
                // De-normalize: convert the wideband estimate to the sub-band
                // SINR for this candidate allocation size
                double subBandSnrDb =
                    baseEstimate + 10.0 * std::log10(static_cast<double>(poolRbs) / candidateRbs);

                // Predict the effective BLER for this candidate.  When
                // HarqAwareTarget is set, query the conditional TBLER of each
                // of poolMaxNumTx transmissions (RV sequence {0, 2, 3, 1})
                // and accumulate the joint probability that all attempts
                // fail.  Otherwise a single query predicts first-tx BLER
                // (rv=0).  When m_useSci2aComposition is set, the
                // per-attempt SCI-2a decoding probability is folded in (a
                // missed SCI-2a excludes that attempt from soft combining).
                double sci2aBler = 0.0;
                if (m_useSci2aComposition)
                {
                    sci2aBler =
                        m_slErrorModel->GetSci2ErrorRate(0,
                                                         static_cast<uint8_t>(m_numerology.value()),
                                                         subBandSnrDb);
                }
                NrSlEffectiveBlerCalculator calc;
                NrErrorModel::NrErrorModelHistory history;
                uint8_t numAttempts = m_harqAwareTarget ? poolMaxNumTx : 1;
                for (uint8_t i = 0; i < numAttempts; i++)
                {
                    uint8_t rvK = rvSeq[i % rvSeq.size()];
                    double conditionalTbler =
                        m_slErrorModel->GetTbler(mcs,
                                                 static_cast<uint8_t>(m_numerology.value()),
                                                 rvK,
                                                 subBandSnrDb,
                                                 tbSize.value(),
                                                 candidateRbs,
                                                 history);
                    calc.AddAttempt(conditionalTbler, sci2aBler, rvK);
                    history.push_back(
                        Create<NrSlErrorModelOutput>(conditionalTbler, subBandSnrDb, rvK));
                }
                double effBler = calc.GetEffectiveBler(m_useSci2aComposition);
                NS_LOG_DEBUG("MCS candidate=" << +mcs << " minSubCh=" << +minSubCh.value()
                                              << " candRBs=" << candidateRbs << " subBand="
                                              << subBandSnrDb << " dB effBler=" << effBler
                                              << (effBler <= m_targetBler ? " PASS" : " FAIL"));
                if (effBler <= m_targetBler)
                {
                    bestMcs = mcs;
                }
            }
        }
        else
        {
            // Joint (MCS, numTx) search: minimize N_sc * numTx subject to the
            // target effective BLER.  When HarqAwareTarget is set, the inner
            // walk on numTx extends the prediction one transmission at a
            // time (RV sequence {0, 2, 3, 1}), accumulating the joint
            // probability that all numTx attempts fail.  When clear, every
            // numTx query predicts first-tx BLER, so the walk collapses to
            // numTx=1 at the highest feasible MCS.  Tie-break prefers
            // smaller numTx, then larger MCS.
            uint32_t bestResource = std::numeric_limits<uint32_t>::max();
            for (uint8_t mcs = 0; mcs <= 28; mcs++)
            {
                auto minSubCh =
                    pool->GetMinSubchannels(slotInfo, mcs, tbSize.value(), maxSubCh, MCS_TABLE);
                if (!minSubCh.has_value())
                {
                    continue;
                }
                uint32_t candidateRbs = minSubCh.value() * subChSize;
                double subBandSnrDb =
                    baseEstimate + 10.0 * std::log10(static_cast<double>(poolRbs) / candidateRbs);

                double sci2aBler = 0.0;
                if (m_useSci2aComposition)
                {
                    sci2aBler =
                        m_slErrorModel->GetSci2ErrorRate(0,
                                                         static_cast<uint8_t>(m_numerology.value()),
                                                         subBandSnrDb);
                }
                NrSlEffectiveBlerCalculator calc;
                NrErrorModel::NrErrorModelHistory history;
                for (uint8_t numTx = 1; numTx <= poolMaxNumTx; numTx++)
                {
                    uint8_t rvK = m_harqAwareTarget ? rvSeq[(numTx - 1) % rvSeq.size()] : rvSeq[0];
                    auto conditionalTbler =
                        m_slErrorModel->GetTbler(mcs,
                                                 static_cast<uint8_t>(m_numerology.value()),
                                                 rvK,
                                                 subBandSnrDb,
                                                 tbSize.value(),
                                                 candidateRbs,
                                                 history);
                    double effBler;
                    if (m_harqAwareTarget)
                    {
                        calc.AddAttempt(conditionalTbler, sci2aBler, rvK);
                        effBler = calc.GetEffectiveBler(m_useSci2aComposition);
                    }
                    else
                    {
                        effBler = m_useSci2aComposition
                                      ? 1.0 - (1.0 - sci2aBler) * (1.0 - conditionalTbler)
                                      : conditionalTbler;
                    }
                    NS_LOG_DEBUG("MCS " << +mcs << " numTx " << +numTx << ": " << minSubCh.value()
                                        << " subCh, " << candidateRbs << " RBs, SNR "
                                        << subBandSnrDb << " dB, TBLER " << conditionalTbler
                                        << ", effBler " << effBler);
                    if (effBler <= m_targetBler)
                    {
                        uint32_t resource = static_cast<uint32_t>(minSubCh.value()) * numTx;
                        bool isBetter = !bestMcs.has_value() || resource < bestResource ||
                                        (resource == bestResource && numTx <= bestNumTx.value());
                        if (isBetter)
                        {
                            bestMcs = mcs;
                            bestNumTx = numTx;
                            bestResource = resource;
                        }
                        break; // smallest feasible numTx for this MCS
                    }
                    if (m_harqAwareTarget)
                    {
                        history.push_back(
                            Create<NrSlErrorModelOutput>(conditionalTbler, subBandSnrDb, rvK));
                    }
                }
            }
        }
    }
    else
    {
        // Without TB size, fall back to BinarySearch using the wideband
        // estimate directly (no per-MCS de-normalization is possible
        // without knowing the candidate allocation size)
        uint32_t tbSizeVal = 100;
        uint32_t numRbsVal = numRbs.value_or(poolRbs);
        NS_FATAL_ERROR("Unreachable; remove std::optional params in future");
        bestMcs = BinarySearch(m_slErrorModel,
                               m_numerology.value(),
                               0u,
                               baseEstimate,
                               m_targetBler,
                               tbSizeVal,
                               numRbsVal);
    }

    if (bestMcs)
    {
        NS_LOG_INFO("Returning MCS " << +bestMcs.value() << " numTx "
                                     << (bestNumTx.has_value() ? +bestNumTx.value() : -1)
                                     << " to dstL2Id " << dstL2Id << " for SINR "
                                     << filteredSinrDb);
        m_scheduler->SetMcs(dstL2Id, bestMcs.value());
    }
    else
    {
        NS_LOG_INFO("No MCS meets target; setting MCS 0 for dstL2Id " << dstL2Id << " for SINR "
                                                                      << filteredSinrDb);
        m_scheduler->SetMcs(dstL2Id, 0u);
    }
    NS_LOG_DEBUG("decision: tbSize=" << tbSize.value_or(0) << " numRbs=" << numRbs.value_or(0)
                                     << " offset=" << sinrOffsetDelta
                                     << " mcs=" << (bestMcs ? +bestMcs.value() : 0) << " numTx="
                                     << (bestNumTx.has_value() ? +bestNumTx.value() : 0));
    return {bestMcs, bestNumTx};
}

void
NrSlOllaMcsController::ConfigureTraces()
{
    NS_LOG_FUNCTION(this);

    for (auto i = NodeList::Begin(); i != NodeList::End(); i++)
    {
        Ptr<Node> node = *i;
        for (uint32_t j = 0; j < node->GetNDevices(); j++)
        {
            Ptr<NrUeNetDevice> ueDevice = node->GetDevice(j)->GetObject<NrUeNetDevice>();
            if (ueDevice)
            {
                auto uePhy = ueDevice->GetPhy(0)->GetObject<NrSlUePhy>();
                NS_ASSERT(uePhy);
                auto ueSpectrumPhy = uePhy->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
                NS_ASSERT(ueSpectrumPhy);
                const auto found = ueSpectrumPhy->TraceConnect(
                    "RxPsschTraceUe",
                    std::to_string(node->GetId()),
                    MakeCallback(&NrSlOllaMcsController::NotifyRxPssch, this));
                NS_ASSERT(found);
            }
        }
    }

    PointerValue harqValue{};
    m_scheduler->GetMac()->GetAttribute("NrSlUeMacHarq", harqValue);
    auto harq = harqValue.Get<NrSlUeMacHarq>();
    // Match the offset adapter's feedback signal to the predictor's BLER target.
    // HarqAwareTarget=1 means the predictor's TargetBler is post-HARQ effective
    // BLER, so the offset must be driven by per-TB-completion outcomes
    // (HarqTbCompletion, fired once when the chain ends in ACK or maxNumTx).
    // HarqAwareTarget=0 keeps the per-tx HARQ feedback (HarqFeedbackReceived,
    // fired on every PSFCH event), which matches a first-tx BLER target.
    const std::string traceName = m_harqAwareTarget ? "HarqTbCompletion" : "HarqFeedbackReceived";
    harq->TraceConnectWithoutContext(traceName,
                                     MakeCallback(&NrSlOllaMcsController::RxHarqFeedback, this));
}

void
NrSlOllaMcsController::SaveSinr(const L2Id destination, const SinrTraceEntry entry)
{
    NS_LOG_FUNCTION(this << destination << entry.receivedAt << entry.sinrDb);

    const auto [_, isNew] = m_sinrTraces.insert_or_assign(destination, entry);

    if (isNew)
    {
        NS_LOG_DEBUG("Controller " << m_srcL2Id << " inserted new entry for " << destination
                                   << " of wideband snrDb: " << entry.sinrDb << " dB");
    }
    else
    {
        NS_LOG_DEBUG("Controller " << m_srcL2Id << " updated entry for " << destination
                                   << " to wideband snrDb: " << entry.sinrDb << " dB");
    }

    // Every raw sample feeds the receiver-side filter input buffer.
    // This upstream stage (max over the buffer) suppresses collision-
    // driven SINR drops before any CQI estimate is exported.
    auto& buffer = m_sinrBuffer[destination];
    buffer.push_back(entry.sinrDb);
    while (buffer.size() > m_medianFilterSize)
    {
        buffer.pop_front();
    }

    auto cqiPeriodMs = m_cqiPeriod->GetValue();
    if (cqiPeriodMs == 0.0)
    {
        // No gating; the CQI estimate is recomputed and emitted on
        // every raw sample
        EmitFilteredSinr(destination);
        NS_LOG_DEBUG("CQI gating disabled; filtered SINR updated immediately for " << destination);
    }
    else if (isNew)
    {
        // First sample for this destination; emit immediately and start
        // the periodic CQI update cycle
        EmitFilteredSinr(destination);
        auto delay = MilliSeconds(static_cast<int64_t>(m_cqiPeriod->GetValue()));
        m_cqiUpdateEvents[destination] =
            Simulator::Schedule(delay,
                                &NrSlOllaMcsController::UpdateFilteredSinr,
                                this,
                                destination);
        NS_LOG_INFO("First SINR for destination " << destination << ": " << entry.sinrDb
                                                  << " dB; CQI update scheduled in " << delay);
    }
    // Otherwise, the periodic timer is already running and will sample
    // the current filter output into m_filteredSinrTraces on its next firing
}

void
NrSlOllaMcsController::UpdateFilteredSinr(const L2Id destination)
{
    NS_LOG_FUNCTION(this << destination);
    auto bufIt = m_sinrBuffer.find(destination);
    if (bufIt == m_sinrBuffer.end() || bufIt->second.empty())
    {
        NS_LOG_DEBUG("No SINR samples for destination " << destination << "; skipping CQI update");
        return;
    }
    std::optional<double> prevFiltered;
    if (auto prevIt = m_filteredSinrTraces.find(destination); prevIt != m_filteredSinrTraces.end())
    {
        prevFiltered = prevIt->second.sinrDb;
    }
    EmitFilteredSinr(destination);
    const double newFiltered = m_filteredSinrTraces.at(destination).sinrDb;
    if (!prevFiltered.has_value() || *prevFiltered != newFiltered)
    {
        NS_LOG_INFO("CQI update for destination " << destination << ": filtered SINR changed to "
                                                  << newFiltered << " dB");
    }
    else
    {
        NS_LOG_DEBUG("CQI update for destination " << destination << ": filtered SINR unchanged at "
                                                   << newFiltered << " dB");
    }
    // Schedule the next CQI update
    auto delay = MilliSeconds(static_cast<int64_t>(m_cqiPeriod->GetValue()));
    m_cqiUpdateEvents[destination] =
        Simulator::Schedule(delay, &NrSlOllaMcsController::UpdateFilteredSinr, this, destination);
}

void
NrSlOllaMcsController::EmitFilteredSinr(const L2Id destination)
{
    NS_LOG_FUNCTION(this << destination);
    auto& buffer = m_sinrBuffer.at(destination);
    NS_ASSERT(!buffer.empty());
    const double filtered = Max(buffer);
    NS_LOG_INFO("Filtered SINR for destination " << destination << " = " << filtered
                                                 << " dB (max over " << buffer.size()
                                                 << " raw sample(s))");
    m_filteredSinrTraces.insert_or_assign(destination, FilteredSinrEntry{Now(), filtered});
}

double
NrSlOllaMcsController::Max(const std::deque<double>& buffer)
{
    NS_ASSERT(!buffer.empty());
    return *std::max_element(buffer.begin(), buffer.end());
}

double
NrSlOllaMcsController::GetTargetBler() const
{
    NS_LOG_FUNCTION(this);
    return m_targetBler;
}

void
NrSlOllaMcsController::SetTargetBler(const double tbler)
{
    NS_LOG_FUNCTION(this << tbler);
    NS_ABORT_MSG_IF(
        tbler == 1.0,
        "Error, Target BLER may not be `1.0` in `NrSlOllaMcsController::SetTargetBler`");
    m_targetBler = tbler;
}

double
NrSlOllaMcsController::GetNackPenaltyStep() const
{
    NS_LOG_FUNCTION(this);
    return m_nackPenaltyStep;
}

void
NrSlOllaMcsController::SetNackPenaltyStep(const double nackPenalty)
{
    NS_LOG_FUNCTION(this << nackPenalty);

    m_nackPenaltyStep = nackPenalty;
    m_ackStep = m_targetBler / (1.0 - m_targetBler) * nackPenalty;
    NS_LOG_INFO("`m_ackStep` calculated to be: " << m_ackStep);
}

double
NrSlOllaMcsController::GetAckStep() const
{
    NS_LOG_FUNCTION(this);
    return m_ackStep;
}

std::optional<double>
NrSlOllaMcsController::GetLastSinr(const uint32_t l2Id) const
{
    const auto& entry = m_sinrTraces.find(l2Id);
    if (entry == m_sinrTraces.end())
    {
        return {};
    }

    return {entry->second.sinrDb};
}

void
NrSlOllaMcsController::Flush(const uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    m_sinrTraces.erase(dstL2Id);
    m_filteredSinrTraces.erase(dstL2Id);
    m_sinrBuffer.erase(dstL2Id);
    m_sinrDeltas.erase(dstL2Id);
    auto saveSinrIt = m_saveSinrEvents.find(dstL2Id);
    if (saveSinrIt != m_saveSinrEvents.end())
    {
        for (auto& event : saveSinrIt->second)
        {
            Simulator::Cancel(event);
        }
        m_saveSinrEvents.erase(saveSinrIt);
        NS_LOG_DEBUG("Cancelled pending SaveSinr events for destination " << dstL2Id);
    }
    auto cqiIt = m_cqiUpdateEvents.find(dstL2Id);
    if (cqiIt != m_cqiUpdateEvents.end())
    {
        Simulator::Cancel(cqiIt->second);
        m_cqiUpdateEvents.erase(cqiIt);
        NS_LOG_DEBUG("Cancelled pending CQI update for destination " << dstL2Id);
    }
}

std::optional<uint8_t>
NrSlOllaMcsController::BinarySearch(Ptr<const NrSlErrorModel> errorModel,
                                    const uint16_t numerology,
                                    const uint8_t rv,
                                    const double snrDb,
                                    const double targetBler,
                                    const uint32_t tbSize,
                                    const uint32_t numRbs)
{
    NS_LOG_FUNCTION(+numerology << +rv << snrDb << targetBler << tbSize << numRbs);
    auto tbler = errorModel->GetTbler(0, numerology, rv, snrDb, tbSize, numRbs, {});
    if (tbler > targetBler)
    {
        NS_LOG_DEBUG("No MCS meets target " << targetBler << " for SINR(dB) " << snrDb
                                            << " TB(bytes) " << tbSize << " numerology "
                                            << +numerology << " TBLER " << tbler);
        return std::nullopt;
    }
    tbler = errorModel->GetTbler(28, numerology, rv, snrDb, tbSize, numRbs, {});
    if (tbler <= targetBler)
    {
        NS_LOG_DEBUG("Highest MCS meets target "
                     << targetBler << "; MCS 28 selected for SINR(dB) " << snrDb << " TB(bytes) "
                     << tbSize << " numerology " << +numerology << " TBLER " << tbler);
        return 28u;
    }
    // Binary search through the remaining MCS values
    uint8_t bestMcs{0};
    double bestTbler{0};
    int low{0};
    int high{27};
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        tbler = errorModel->GetTbler(mid, numerology, rv, snrDb, tbSize, numRbs, {});
        if (tbler <= targetBler)
        {
            bestMcs = mid;
            bestTbler = tbler;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    NS_LOG_DEBUG("MCS " << +bestMcs << " has BLER " << bestTbler << " below threshold "
                        << targetBler << " for SINR(dB) " << snrDb << " TB(bytes) " << tbSize
                        << " numerology " << +numerology);
    return bestMcs;
}

void
NrSlOllaMcsController::NotifyRxPssch(const std::string context,
                                     const SlRxDataPacketTraceParams traceParams)
{
    NS_LOG_FUNCTION(this << context);

    if (traceParams.m_srcL2Id != m_srcL2Id)
    {
        return;
    }

    // The received SINR is a sub-band measurement over the RBs actually used
    // by this transmission.  Normalize it to wideband (full band occupancy) by
    // subtracting the power concentration factor.  All stored and traced SINR
    // values use this wideband reference, which is the allocation-independent
    // channel quality -- analogous to the single SINR in idealized models.
    // De-normalization to the sub-band SINR for a specific candidate MCS
    // allocation is performed later in DoGetGrantParams().
    auto subBandSnrDb = 10.0 * std::log10(traceParams.m_sinr);
    const auto resourceBlocksUsed = static_cast<double>(traceParams.m_rbAssignedNum);
    const auto poolRbs = static_cast<uint32_t>(m_mac->GetTotalSubCh() * m_mac->GetNrSlSubChSize());
    const auto resourceBlocksInPool = static_cast<double>(poolRbs);
    // Wideband SINR = sub-band SINR - 10*log10(poolRBs / usedRBs)
    // This removes the power concentration gain from using fewer RBs than
    // the full pool, yielding the SINR that would be observed if the
    // transmission occupied the entire pool bandwidth.
    auto widebandSnrDb = subBandSnrDb;
    if (resourceBlocksUsed < resourceBlocksInPool)
    {
        widebandSnrDb -= 10.0 * std::log10(resourceBlocksInPool / resourceBlocksUsed);
    }

    const auto delay = MilliSeconds(m_rxPsschDelay->GetInteger());
    NS_LOG_DEBUG("Scheduling `SaveSinr` in delay: " << delay << " wideband SINR: " << widebandSnrDb
                                                    << " dB (sub-band: " << subBandSnrDb << " dB"
                                                    << ", RBs: " << resourceBlocksUsed << "/"
                                                    << resourceBlocksInPool << ")");
    auto eventId = Simulator::Schedule(
        delay,
        &NrSlOllaMcsController::SaveSinr,
        this,
        traceParams.m_dstL2Id,
        SinrTraceEntry{.receivedAt = Now(), .sinrDb = widebandSnrDb, .poolRbs = poolRbs});
    // Track the event and remove any expired events for this destination
    auto& events = m_saveSinrEvents[traceParams.m_dstL2Id];
    events.erase(std::remove_if(events.begin(),
                                events.end(),
                                [](const EventId& e) { return e.IsExpired(); }),
                 events.end());
    events.push_back(eventId);
}

void
NrSlOllaMcsController::RxHarqFeedback(const SlHarqInfo& harqInfo)
{
    NS_LOG_FUNCTION(this << harqInfo);
    m_trialResultTrace(harqInfo.m_mcs, harqInfo.m_dstL2Id, harqInfo.IsReceivedOk());
    const auto status = harqInfo.m_harqStatus;

    auto deltaIt = m_sinrDeltas.find(harqInfo.m_dstL2Id);
    if (deltaIt == m_sinrDeltas.end())
    {
        deltaIt = m_sinrDeltas.emplace(harqInfo.m_dstL2Id, SinrDeltaEntry{}).first;
    }

    auto& [_, sinrOffsetDelta] = deltaIt->second;

    switch (status)
    {
    case SlHarqInfo::ACK:
        // If clamping is enabled and we are already at the upper limit,
        // ignore the ACK to prevent unbounded offset growth
        if (m_enableClamping && harqInfo.m_mcs >= 28u && sinrOffsetDelta >= 0.0)
        {
            NS_LOG_DEBUG("ACK received, but we are already estimating MCS 28, ignoring");
            return;
        }
        sinrOffsetDelta += m_ackStep;
        break;
    case SlHarqInfo::NACK:
    case SlHarqInfo::TIMEOUT:
        // Same as ACK but for the lower bound
        if (m_enableClamping && harqInfo.m_mcs == 0u && sinrOffsetDelta <= 0.0)
        {
            NS_LOG_DEBUG("NACK received, but we're already estimating MCS 0, ignoring");
            return;
        }
        sinrOffsetDelta -= m_nackPenaltyStep;
        break;
    case SlHarqInfo::UNSET:
        NS_FATAL_ERROR("Error: received SlHarqInfo::UNSET (uninitialized)");
    }
    sinrOffsetDelta = std::clamp(sinrOffsetDelta, m_minSinrOffsetDelta, m_maxSinrOffsetDelta);
}

NrSlCommResourcePool::SlotInfo
NrSlOllaMcsController::CreateSlotInfo() const
{
    const auto pool = m_mac->GetTxPool();
    const auto bwpId = m_mac->GetBwpId();
    const auto poolId = m_mac->GetSlActivePoolId();
    bool hasPsfch = m_mac->GetPsfchPeriod() > 0;
    return NrSlCommResourcePool::SlotInfo{pool->GetNumSlPscchRbs(bwpId, poolId),
                                          pool->GetPscchSymStart(bwpId, poolId),
                                          pool->GetPscchSymLength(bwpId, poolId),
                                          pool->GetPsschSymStart(bwpId, poolId),
                                          pool->GetPsschSymLength(bwpId, poolId, hasPsfch),
                                          hasPsfch,
                                          pool->GetSlSubChSize(bwpId, poolId),
                                          pool->GetMaxNumPerReserve(bwpId, poolId),
                                          0,
                                          0};
}

GrantParams
NrSlOllaMcsController::DoGetLargestFeasibleTb(uint32_t dstL2Id,
                                              [[maybe_unused]] SidelinkInfo::CastType castType,
                                              uint16_t maxSubCh)
{
    NS_LOG_FUNCTION(this << dstL2Id << maxSubCh);
    if (dstL2Id > m_maximumL2Id)
    {
        return {};
    }
    const auto& it = m_filteredSinrTraces.find(dstL2Id);
    if (it == m_filteredSinrTraces.end())
    {
        NS_LOG_INFO("No history for destination " << dstL2Id << "; no feasible answer");
        return {};
    }
    NS_ASSERT_MSG(m_slErrorModel, "Error: ErrorModel is not set");

    const auto filteredSinrDb = it->second.sinrDb;
    auto sinrOffsetDelta{0.0};
    if (const auto& deltaIt = m_sinrDeltas.find(dstL2Id); deltaIt != m_sinrDeltas.end())
    {
        sinrOffsetDelta = deltaIt->second.sinrOffsetDelta;
    }
    const auto baseEstimate = filteredSinrDb + sinrOffsetDelta;

    constexpr uint8_t MCS_TABLE = 1;
    auto slotInfo = CreateSlotInfo();
    auto pool = m_mac->GetTxPool();
    uint16_t subChSize = slotInfo.slSubchannelSize;
    uint32_t poolRbs = static_cast<uint32_t>(m_mac->GetTotalSubCh() * subChSize);

    // Walk lSubch outer, mcs inner (from high to low).  At each lSubch,
    // de-normalize the wideband estimate to the sub-band SINR for the
    // candidate width and pick the highest MCS whose predicted effective
    // BLER meets the target.  When HarqAwareTarget is set, the prediction
    // covers poolMaxNumTx soft-combined transmissions; otherwise it is
    // first-tx BLER.  The chosen TB size is the largest deliverable at
    // this width.  Track the overall maximum across lSubch.
    static const std::vector<uint8_t> rvSeq{0, 2, 3, 1};
    uint8_t poolMaxNumTx = m_mac->GetSlMaxTxTransNumPssch();
    uint8_t numAttempts = m_harqAwareTarget ? poolMaxNumTx : 1;
    std::optional<uint8_t> bestMcs;
    std::optional<uint32_t> bestTbSize;
    for (uint16_t lSubch = 1; lSubch <= maxSubCh; lSubch++)
    {
        uint32_t candidateRbs = static_cast<uint32_t>(lSubch) * subChSize;
        double subBandSnrDb =
            baseEstimate + 10.0 * std::log10(static_cast<double>(poolRbs) / candidateRbs);
        double sci2aBler = 0.0;
        if (m_useSci2aComposition)
        {
            sci2aBler = m_slErrorModel->GetSci2ErrorRate(0,
                                                         static_cast<uint8_t>(m_numerology.value()),
                                                         subBandSnrDb);
        }
        for (int16_t mcs = 28; mcs >= 0; mcs--)
        {
            uint32_t deliverableTbSize =
                pool->GetTransportBlockSize(slotInfo, static_cast<uint8_t>(mcs), lSubch, MCS_TABLE);
            NrSlEffectiveBlerCalculator calc;
            NrErrorModel::NrErrorModelHistory history;
            for (uint8_t i = 0; i < numAttempts; i++)
            {
                uint8_t rvK = rvSeq[i % rvSeq.size()];
                double conditionalTbler =
                    m_slErrorModel->GetTbler(static_cast<uint8_t>(mcs),
                                             static_cast<uint8_t>(m_numerology.value()),
                                             rvK,
                                             subBandSnrDb,
                                             deliverableTbSize,
                                             candidateRbs,
                                             history);
                calc.AddAttempt(conditionalTbler, sci2aBler, rvK);
                history.push_back(
                    Create<NrSlErrorModelOutput>(conditionalTbler, subBandSnrDb, rvK));
            }
            double effBler = calc.GetEffectiveBler(m_useSci2aComposition);
            if (effBler <= m_targetBler)
            {
                if (!bestTbSize.has_value() || deliverableTbSize > bestTbSize.value())
                {
                    bestMcs = static_cast<uint8_t>(mcs);
                    bestTbSize = deliverableTbSize;
                }
                break;
            }
        }
    }
    NS_LOG_INFO(
        "Largest feasible TB for dstL2Id "
        << dstL2Id
        << ": mcs=" << (bestMcs.has_value() ? std::to_string(+bestMcs.value()) : "nullopt")
        << " tbSize=" << (bestTbSize.has_value() ? std::to_string(bestTbSize.value()) : "nullopt")
        << " (filtered SINR " << filteredSinrDb << " dB, offset " << sinrOffsetDelta << " dB)");
    GrantParams params;
    params.mcs = bestMcs;
    params.feasibleTbSize = bestTbSize;
    return params;
}

void
NrSlOllaMcsController::DoDispose()
{
    NS_LOG_FUNCTION(this);
    NrSlMcsController::DoDispose();
}

int64_t
NrSlOllaMcsController::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    auto baseStreamsUsed = NrSlMcsController::AssignStreams(stream);
    m_rxPsschDelay->SetStream(stream + baseStreamsUsed);
    m_cqiPeriod->SetStream(stream + baseStreamsUsed + 1);
    return baseStreamsUsed + 2;
}

} // namespace ns3
