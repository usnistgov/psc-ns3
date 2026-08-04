//
// SPDX-License-Identifier: NIST-Software

#include "nr-sl-ideal-mcs-controller.h"

#include "nr-sl-effective-bler-calculator.h"
#include "nr-sl-spectrum-phy.h"
#include "nr-sl-ue-mac-harq.h"
#include "nr-sl-ue-mac-scheduler-default.h"
#include "nr-sl-ue-phy.h"

#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/double.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nr-mcs-tables.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlIdealMcsController");

NS_OBJECT_ENSURE_REGISTERED(NrSlIdealMcsController);

TypeId
NrSlIdealMcsController::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlIdealMcsController")
            .SetParent<NrSlMcsController>()
            .AddConstructor<NrSlIdealMcsController>()
            .SetGroupName("nr")
            .AddAttribute("Margin",
                          "A margin (in dB) subtracted from the SINR "
                          "before computing the block error rate",
                          DoubleValue(0.0),
                          MakeDoubleAccessor(&NrSlIdealMcsController::m_sinrMargin),
                          MakeDoubleChecker<double>())
            .AddAttribute("TargetBler",
                          "The highest allowed block error rate for the calculated MCS",
                          DoubleValue(0.1),
                          MakeDoubleAccessor(&NrSlIdealMcsController::m_tblerTarget),
                          MakeDoubleChecker<double>())
            .AddAttribute("HarqAwareTarget",
                          "If true, the TargetBler applies to the post-HARQ effective "
                          "BLER of up to maxNumTx soft-combined transmissions using "
                          "the RV sequence {0, 2, 3, 1}.  If false (default), the "
                          "target applies to first-transmission BLER only.  When "
                          "DynamicNumTx is false and HarqAwareTarget is true, the "
                          "predictor accumulates the joint failure probability "
                          "across maxNumTx transmissions before evaluating the "
                          "target.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlIdealMcsController::m_harqAwareTarget),
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
                          MakeBooleanAccessor(&NrSlIdealMcsController::m_dynamicNumTx),
                          MakeBooleanChecker())
            .AddAttribute("MaximumL2Id",
                          "The highest destination L2 ID value to control",
                          UintegerValue(200),
                          MakeUintegerAccessor(&NrSlIdealMcsController::m_maximumL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddTraceSource("TrialResult",
                            "The outcome of a HARQ feedback trial.",
                            MakeTraceSourceAccessor(&NrSlIdealMcsController::m_trialResultTrace),
                            "ns3::NrSlIdealMcsController::TrialResultCallback");
    return tid;
}

NrSlIdealMcsController::NrSlIdealMcsController()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlIdealMcsController::DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    m_srcL2Id = scheduler->GetMac()->GetImsi(); // Helper sets srcL2Id to the IMSI
    m_srcRnti = scheduler->GetMac()->GetRnti();
    NS_LOG_DEBUG("Source L2 Id set to " << m_srcL2Id << " RNTI " << m_srcRnti);
    Simulator::ScheduleWithContext(m_srcL2Id,
                                   Seconds(0),
                                   &NrSlIdealMcsController::ConfigureTraces,
                                   this);
    PointerValue nrSlUeMacHarqPtr;
    m_mac->GetAttribute("NrSlUeMacHarq", nrSlUeMacHarqPtr);
    DynamicCast<NrSlUeMacHarq>(nrSlUeMacHarqPtr.GetObject())
        ->TraceConnectWithoutContext(
            "HarqFeedbackReceived",
            MakeCallback(&NrSlIdealMcsController::NotifyHarqFeedbackReceived, this));
}

void
NrSlIdealMcsController::NotifyHarqFeedbackReceived(const SlHarqInfo& slHarqInfo)
{
    NS_LOG_FUNCTION(this << slHarqInfo);
    m_trialResultTrace(slHarqInfo.m_mcs, slHarqInfo.m_dstL2Id, slHarqInfo.IsReceivedOk());
}

void
NrSlIdealMcsController::ConfigureTraces(void)
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
                bool found = ueSpectrumPhy->TraceConnect(
                    "RxPsschTraceUe",
                    std::to_string(node->GetId()),
                    MakeCallback(&NrSlIdealMcsController::NotifyRxPssch, this));
                NS_ASSERT(found);
                found = ueSpectrumPhy->TraceConnect(
                    "RxPscchTraceUe",
                    std::to_string(node->GetId()),
                    MakeCallback(&NrSlIdealMcsController::NotifyRxPscch, this));
                NS_ASSERT(found);
                if (node->GetId() == m_srcL2Id)
                {
                    m_slErrorModel = ueSpectrumPhy->GetSlErrorModel();
                }
            }
        }
    }
    NS_ASSERT_MSG(m_slErrorModel, "Error model not found");
}

NrSlCommResourcePool::SlotInfo
NrSlIdealMcsController::CreateSlotInfo() const
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
NrSlIdealMcsController::DoGetGrantParams(uint32_t dstL2Id,
                                         [[maybe_unused]] SidelinkInfo::CastType castType,
                                         std::optional<uint32_t> numRbs,
                                         std::optional<uint32_t> tbSize)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    if (dstL2Id > m_maximumL2Id)
    {
        return {};
    }
    const auto& it = m_traceMap.find(dstL2Id);
    if (it == m_traceMap.end())
    {
        NS_LOG_INFO("No history for destination " << dstL2Id << "; returning MCS 0");
        return {uint8_t{0}, std::nullopt};
    }

    // If no TB size is provided, fall back to BinarySearch with the observed
    // SNR (no per-MCS RB correction is possible without knowing the TB size).
    if (!tbSize.has_value())
    {
        uint32_t numRbsVal = numRbs.value_or(it->second.rbsUsed);
        double snrDb = it->second.sinrDb;
        if (numRbsVal != it->second.rbsUsed)
        {
            snrDb += 10.0 * std::log10(static_cast<double>(it->second.rbsUsed) / numRbsVal);
        }
        NS_FATAL_ERROR("Unreachable; remove std::optional params in future");
        auto bestMcs = BinarySearch(m_slErrorModel,
                                    m_numerology.value(),
                                    0, // first-tx prediction for a fresh TB
                                    snrDb - m_sinrMargin,
                                    m_tblerTarget,
                                    100,
                                    numRbsVal);
        NS_LOG_INFO("Returning MCS " << (bestMcs.has_value() ? +bestMcs.value() : -1)
                                     << " for dstL2Id " << dstL2Id << " (no tbSize, SNR " << snrDb
                                     << " dB)");
        return {bestMcs, std::nullopt};
    }

    // With a known TB size, iterate over candidate MCS values and adjust the
    // SNR for the number of RBs each candidate would require.  The observed
    // SNR was measured over rbsUsed RBs.  A candidate MCS that needs fewer
    // RBs concentrates the same total power into fewer RBs, raising the
    // per-RB SNR; a candidate needing more RBs lowers it.
    //
    // When the caller provides numRbs, it is a width budget: candidates
    // whose minimum allocation for the TB exceeds it are not considered.
    // Grant creation passes the current search width; SPS publication
    // passes the grant's fixed allocation width.
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
    double observedSnrDb = it->second.sinrDb;
    uint32_t observedRbs = it->second.rbsUsed;
    uint8_t maxMcs = NrMcsTables::GetMaxMcs(MCS_TABLE);
    static const std::vector<uint8_t> rvSeq{0, 2, 3, 1};

    if (!m_dynamicNumTx)
    {
        std::optional<uint8_t> bestMcs;
        double bestTbler = 0.0;
        uint32_t bestMinSubCh = 0;
        double bestCandidateSnrDb = 0.0;
        for (uint8_t mcs = 0; mcs <= maxMcs; mcs++)
        {
            auto minSubCh =
                pool->GetMinSubchannels(slotInfo, mcs, tbSize.value(), maxSubCh, MCS_TABLE);
            if (!minSubCh.has_value())
            {
                // TB cannot fit at this MCS even with all subchannels
                continue;
            }
            uint32_t candidateRbs = minSubCh.value() * subChSize;
            double candidateSnrDb = observedSnrDb;
            if (candidateRbs != observedRbs)
            {
                candidateSnrDb +=
                    10.0 * std::log10(static_cast<double>(observedRbs) / candidateRbs);
            }

            // Predict the effective BLER for this candidate.  When
            // HarqAwareTarget is set, query the conditional TBLER of each of
            // maxNumTx transmissions (RV sequence {0, 2, 3, 1}) and
            // accumulate the joint probability that all attempts fail.
            // Otherwise a single query predicts first-tx BLER (rv=0), which
            // is the prediction for a fresh TB at grant creation or
            // publication.  When m_useSci2aComposition is set, the
            // per-attempt SCI-2a decoding probability is folded in (SCI-2a
            // uses fixed QPSK coding, so its signature is (mcs=0,
            // numerology, sub-band SNR); a missed SCI-2a excludes that
            // attempt from soft combining).
            double sci2aBler = 0.0;
            if (m_useSci2aComposition)
            {
                sci2aBler =
                    m_slErrorModel->GetSci2ErrorRate(0,
                                                     static_cast<uint8_t>(m_numerology.value()),
                                                     candidateSnrDb - m_sinrMargin);
            }
            NrSlEffectiveBlerCalculator calc;
            NrErrorModel::NrErrorModelHistory history;
            uint8_t numAttempts = m_harqAwareTarget ? m_mac->GetSlMaxTxTransNumPssch() : 1;
            for (uint8_t i = 0; i < numAttempts; i++)
            {
                uint8_t rvK = rvSeq[i % rvSeq.size()];
                double conditionalTbler =
                    m_slErrorModel->GetTbler(mcs,
                                             static_cast<uint8_t>(m_numerology.value()),
                                             rvK,
                                             candidateSnrDb - m_sinrMargin,
                                             tbSize.value(),
                                             candidateRbs,
                                             history);
                calc.AddAttempt(conditionalTbler, sci2aBler, rvK);
                history.push_back(Create<NrSlErrorModelOutput>(conditionalTbler,
                                                               candidateSnrDb - m_sinrMargin,
                                                               rvK));
            }
            double effBler = calc.GetEffectiveBler(m_useSci2aComposition);
            NS_LOG_DEBUG("MCS " << +mcs << ": " << minSubCh.value() << " subCh, " << candidateRbs
                                << " RBs, SNR " << candidateSnrDb << " dB, sci2aBler " << sci2aBler
                                << ", effBler " << effBler);
            if (effBler <= m_tblerTarget)
            {
                bestMcs = mcs;
                bestTbler = effBler;
                bestMinSubCh = minSubCh.value();
                bestCandidateSnrDb = candidateSnrDb;
            }
        }

        NS_LOG_INFO("Returning MCS " << (bestMcs.has_value() ? +bestMcs.value() : -1)
                                     << " for dstL2Id " << dstL2Id << " tbSize " << tbSize.value()
                                     << " (observed " << observedRbs << " RBs, SNR "
                                     << observedSnrDb << " dB)");
        NS_LOG_DEBUG("decision factors: dstL2Id="
                     << dstL2Id << " tbSize=" << tbSize.value() << " observedRbs=" << observedRbs
                     << " observedSnrDb=" << observedSnrDb
                     << " mcs=" << (bestMcs.has_value() ? +bestMcs.value() : -1)
                     << " minSubCh=" << bestMinSubCh << " candSnrDb=" << bestCandidateSnrDb
                     << " predTbler=" << bestTbler);
        return {bestMcs, std::nullopt};
    }

    // Joint (MCS, numTx) search: minimize N_sc * numTx subject to the target
    // effective BLER.  When HarqAwareTarget is set, the inner walk on numTx
    // extends the prediction one transmission at a time (RV sequence
    // {0, 2, 3, 1}), accumulating the joint probability that all numTx
    // attempts fail.  When HarqAwareTarget is clear, every numTx query
    // predicts first-tx BLER, so the walk collapses to numTx = 1 at the
    // highest feasible MCS.  Tie-break prefers smaller numTx, then larger
    // MCS.
    uint8_t maxNumTx = m_mac->GetSlMaxTxTransNumPssch();
    std::optional<uint8_t> bestMcs;
    std::optional<uint8_t> bestNumTx;
    uint32_t bestResource = std::numeric_limits<uint32_t>::max();
    double bestTbler = 0.0;
    uint32_t bestMinSubCh = 0;
    double bestCandidateSnrDb = 0.0;
    for (uint8_t mcs = 0; mcs <= maxMcs; mcs++)
    {
        auto minSubCh = pool->GetMinSubchannels(slotInfo, mcs, tbSize.value(), maxSubCh, MCS_TABLE);
        if (!minSubCh.has_value())
        {
            continue;
        }
        uint32_t candidateRbs = minSubCh.value() * subChSize;
        double candidateSnrDb = observedSnrDb;
        if (candidateRbs != observedRbs)
        {
            candidateSnrDb += 10.0 * std::log10(static_cast<double>(observedRbs) / candidateRbs);
        }
        double sci2aBler = 0.0;
        if (m_useSci2aComposition)
        {
            sci2aBler = m_slErrorModel->GetSci2ErrorRate(0,
                                                         static_cast<uint8_t>(m_numerology.value()),
                                                         candidateSnrDb - m_sinrMargin);
        }

        NrSlEffectiveBlerCalculator calc;
        NrErrorModel::NrErrorModelHistory history;
        for (uint8_t numTx = 1; numTx <= maxNumTx; numTx++)
        {
            uint8_t rvK = m_harqAwareTarget ? rvSeq[(numTx - 1) % rvSeq.size()] : rvSeq[0];
            auto conditionalTbler =
                m_slErrorModel->GetTbler(mcs,
                                         static_cast<uint8_t>(m_numerology.value()),
                                         rvK,
                                         candidateSnrDb - m_sinrMargin,
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
                effBler = m_useSci2aComposition ? 1.0 - (1.0 - sci2aBler) * (1.0 - conditionalTbler)
                                                : conditionalTbler;
            }
            NS_LOG_DEBUG("MCS " << +mcs << " numTx " << +numTx << ": " << minSubCh.value()
                                << " subCh, " << candidateRbs << " RBs, SNR " << candidateSnrDb
                                << " dB, TBLER " << conditionalTbler << ", effBler " << effBler);
            if (effBler <= m_tblerTarget)
            {
                uint32_t resource = static_cast<uint32_t>(minSubCh.value()) * numTx;
                bool isBetter = !bestMcs.has_value() || resource < bestResource ||
                                (resource == bestResource && numTx <= bestNumTx.value());
                if (isBetter)
                {
                    bestMcs = mcs;
                    bestNumTx = numTx;
                    bestResource = resource;
                    bestTbler = effBler;
                    bestMinSubCh = minSubCh.value();
                    bestCandidateSnrDb = candidateSnrDb;
                }
                break; // smallest feasible numTx for this MCS
            }
            if (m_harqAwareTarget)
            {
                history.push_back(Create<NrSlErrorModelOutput>(conditionalTbler,
                                                               candidateSnrDb - m_sinrMargin,
                                                               rvK));
            }
        }
    }

    NS_LOG_INFO("Returning MCS " << (bestMcs.has_value() ? +bestMcs.value() : -1) << " numTx "
                                 << (bestNumTx.has_value() ? +bestNumTx.value() : -1)
                                 << " for dstL2Id " << dstL2Id << " tbSize " << tbSize.value()
                                 << " (observed " << observedRbs << " RBs, SNR " << observedSnrDb
                                 << " dB)");
    NS_LOG_DEBUG("decision factors: dstL2Id="
                 << dstL2Id << " tbSize=" << tbSize.value() << " observedRbs=" << observedRbs
                 << " observedSnrDb=" << observedSnrDb
                 << " mcs=" << (bestMcs.has_value() ? +bestMcs.value() : -1)
                 << " numTx=" << (bestNumTx.has_value() ? +bestNumTx.value() : -1)
                 << " minSubCh=" << bestMinSubCh << " resource=" << bestResource
                 << " candSnrDb=" << bestCandidateSnrDb << " predTbler=" << bestTbler);
    return {bestMcs, bestNumTx};
}

GrantParams
NrSlIdealMcsController::DoGetLargestFeasibleTb(uint32_t dstL2Id,
                                               [[maybe_unused]] SidelinkInfo::CastType castType,
                                               uint16_t maxSubCh)
{
    NS_LOG_FUNCTION(this << dstL2Id << maxSubCh);
    if (dstL2Id > m_maximumL2Id)
    {
        return {};
    }
    const auto& it = m_traceMap.find(dstL2Id);
    if (it == m_traceMap.end())
    {
        NS_LOG_INFO("No history for destination " << dstL2Id << "; no feasible answer");
        return {};
    }
    constexpr uint8_t MCS_TABLE = 1;
    auto slotInfo = CreateSlotInfo();
    auto pool = m_mac->GetTxPool();
    uint16_t subChSize = slotInfo.slSubchannelSize;
    double observedSnrDb = it->second.sinrDb;
    uint32_t observedRbs = it->second.rbsUsed;
    uint8_t maxMcs = NrMcsTables::GetMaxMcs(MCS_TABLE);

    // Walk lSubch outer, mcs inner (from high to low).  At each lSubch,
    // de-normalize the observed sub-band SINR for the candidate width and
    // pick the highest MCS whose predicted effective BLER meets the target;
    // its TB size is the largest deliverable at this width.  When
    // HarqAwareTarget is set, the prediction covers maxNumTx soft-combined
    // transmissions; otherwise it is first-tx BLER.  Track the overall
    // maximum across lSubch.
    static const std::vector<uint8_t> rvSeq{0, 2, 3, 1};
    uint8_t numAttempts = m_harqAwareTarget ? m_mac->GetSlMaxTxTransNumPssch() : 1;
    std::optional<uint8_t> bestMcs;
    std::optional<uint32_t> bestTbSize;
    for (uint16_t lSubch = 1; lSubch <= maxSubCh; lSubch++)
    {
        uint32_t candidateRbs = static_cast<uint32_t>(lSubch) * subChSize;
        double candPerRbSinr = observedSnrDb;
        if (candidateRbs != observedRbs)
        {
            candPerRbSinr += 10.0 * std::log10(static_cast<double>(observedRbs) / candidateRbs);
        }
        double sci2aBler = 0.0;
        if (m_useSci2aComposition)
        {
            sci2aBler = m_slErrorModel->GetSci2ErrorRate(0,
                                                         static_cast<uint8_t>(m_numerology.value()),
                                                         candPerRbSinr - m_sinrMargin);
        }
        for (int16_t mcs = maxMcs; mcs >= 0; mcs--)
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
                                             candPerRbSinr - m_sinrMargin,
                                             deliverableTbSize,
                                             candidateRbs,
                                             history);
                calc.AddAttempt(conditionalTbler, sci2aBler, rvK);
                history.push_back(Create<NrSlErrorModelOutput>(conditionalTbler,
                                                               candPerRbSinr - m_sinrMargin,
                                                               rvK));
            }
            double effBler = calc.GetEffectiveBler(m_useSci2aComposition);
            if (effBler <= m_tblerTarget)
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
    NS_LOG_INFO("Largest feasible TB for dstL2Id "
                << dstL2Id
                << ": mcs=" << (bestMcs.has_value() ? std::to_string(+bestMcs.value()) : "nullopt")
                << " tbSize="
                << (bestTbSize.has_value() ? std::to_string(bestTbSize.value()) : "nullopt")
                << " (observed " << observedRbs << " RBs, SNR " << observedSnrDb << " dB)");
    GrantParams params;
    params.mcs = bestMcs;
    params.feasibleTbSize = bestTbSize;
    return params;
}

void
NrSlIdealMcsController::Flush(const uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    m_traceMap.erase(dstL2Id);
}

std::optional<uint8_t>
NrSlIdealMcsController::BinarySearch(Ptr<const NrSlErrorModel> errorModel,
                                     const uint16_t numerology,
                                     const uint8_t rv,
                                     const double snrDb,
                                     const double targetBler,
                                     const uint32_t tbSize,
                                     const uint32_t numRbs)
{
    NS_LOG_FUNCTION(+numerology << +rv << snrDb << targetBler << tbSize << numRbs);
    auto tbler =
        errorModel->GetTbler(0, static_cast<uint8_t>(numerology), rv, snrDb, tbSize, numRbs, {});
    if (tbler > targetBler)
    {
        NS_LOG_DEBUG("No MCS meets target " << targetBler << " for SINR(dB) " << snrDb
                                            << " TB(bytes) " << tbSize << " numerology "
                                            << +numerology << " TBLER " << tbler);
        return std::nullopt;
    }
    tbler =
        errorModel->GetTbler(28, static_cast<uint8_t>(numerology), rv, snrDb, tbSize, numRbs, {});
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
    uint8_t low{0};
    uint8_t high{27};
    while (low <= high)
    {
        uint8_t mid = low + (high - low) / 2;
        tbler =
            errorModel
                ->GetTbler(mid, static_cast<uint8_t>(numerology), rv, snrDb, tbSize, numRbs, {});
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
NrSlIdealMcsController::NotifyRxPssch(std::string context, SlRxDataPacketTraceParams traceParams)
{
    NS_LOG_FUNCTION(this << context);
    double snrDb = 10 * std::log10(traceParams.m_sinr);
    // Store the sub-band SNR as observed.  The SNR correction for the candidate
    // number of RBs is applied later in DoGetGrantParams(), because the correction
    // depends on how many RBs the candidate transmission would use.
    //
    // Under contention, individual RX SINR samples are pulled down by collision
    // interference and would otherwise overwrite cleaner readings.  Hold a
    // per-destination high-water mark on a channel-quality measure that is
    // invariant to the candidate's grant width:
    //   quality = sinrDb + 10 * log10(rbsUsed)
    // Total TX power is held constant and spread across the assigned RBs, so
    // a wide-grant sample reads ~10*log10(rbsUsed) lower sub-band SNR than a
    // narrow-grant sample of the same physical channel.  Comparing the
    // normalized quantity prevents a wide-grant sample from displacing a
    // narrow-grant sample on the same channel (a lock-in failure mode where
    // one MCS-0 sample at full grant width then locks the controller at MCS 0
    // because every subsequent same-channel sample reads the same).  A more
    // principled filter (e.g., skip samples flagged corrupt or collided)
    // could replace this later.
    if (traceParams.m_srcL2Id == m_srcL2Id)
    {
        const uint32_t rbsUsed = traceParams.m_rbAssignedNum;
        const double quality = snrDb + 10.0 * std::log10(static_cast<double>(rbsUsed));
        auto it = m_traceMap.find(traceParams.m_dstL2Id);
        if (it == m_traceMap.end())
        {
            m_traceMap.emplace(traceParams.m_dstL2Id,
                               TraceEntry{.time = Now(), .sinrDb = snrDb, .rbsUsed = rbsUsed});
            NS_LOG_DEBUG("Controller " << m_srcL2Id << " inserted new entry for "
                                       << traceParams.m_dstL2Id << " of "
                                       << "sub-band snrDb: " << snrDb << " dB, rbsUsed: " << rbsUsed
                                       << " (quality " << quality << " dB)");
        }
        else
        {
            const double cachedQuality =
                it->second.sinrDb + 10.0 * std::log10(static_cast<double>(it->second.rbsUsed));
            if (quality > cachedQuality)
            {
                it->second = TraceEntry{.time = Now(), .sinrDb = snrDb, .rbsUsed = rbsUsed};
                NS_LOG_DEBUG("Controller " << m_srcL2Id << " improved entry for "
                                           << traceParams.m_dstL2Id << " to "
                                           << "sub-band snrDb: " << snrDb
                                           << " dB, rbsUsed: " << rbsUsed << " (quality " << quality
                                           << " > cached " << cachedQuality << " dB)");
            }
            else
            {
                NS_LOG_DEBUG("Controller " << m_srcL2Id << " kept entry for "
                                           << traceParams.m_dstL2Id << " (cached quality "
                                           << cachedQuality << " >= sample " << quality << " dB)");
            }
        }
    }
}

void
NrSlIdealMcsController::NotifyRxPscch(std::string context, SlRxCtrlPacketTraceParams traceParams)
{
    NS_LOG_FUNCTION(this << context);
    // Non-collision PSCCH decode failures for our own transmissions are
    // logged but do not invalidate the per-destination SNR cache: the
    // high-water-mark logic in NotifyRxPssch is the single source of truth
    // for channel quality and ignores interference-corrupted samples on its
    // own.
    if (traceParams.m_corrupt && !traceParams.m_rbCollision && traceParams.m_txRnti == m_srcRnti)
    {
        NS_LOG_DEBUG("PSCCH decode failure (non-collision) for RNTI "
                     << m_srcRnti << " to dstL2Id " << traceParams.m_dstL2Id
                     << "; preserving cached SNR history");
    }
}

void
NrSlIdealMcsController::DoDispose()
{
    NS_LOG_FUNCTION(this);
    NrSlMcsController::DoDispose();
}

int64_t
NrSlIdealMcsController::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    return NrSlMcsController::AssignStreams(stream);
}

} // namespace ns3
