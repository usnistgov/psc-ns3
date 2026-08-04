// Copyright (c) 2021 IITP RAS (adapted from Wi-Fi Thompson Sampling implementation)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-thompson-sampling-mcs-controller.h"

#include "nr-sl-ue-mac-scheduler-default.h"

#include <ns3/attribute-container.h>
#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlThompsonSamplingMcsController");

NS_OBJECT_ENSURE_REGISTERED(NrSlThompsonSamplingMcsController);

const std::vector<uint8_t> NrSlThompsonSamplingMcsController::DEFAULT_MCS_INDEX = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};

TypeId
NrSlThompsonSamplingMcsController::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlThompsonSamplingMcsController")
            .SetParent<NrSlMcsController>()
            .AddConstructor<NrSlThompsonSamplingMcsController>()
            .SetGroupName("nr")
            .AddAttribute("DiscountFactor",
                          "Per-observation decay factor for MCS statistics. Value of 1 is static, "
                          "values below 0.5 are not recommended.",
                          DoubleValue(0.99),
                          MakeDoubleAccessor(&NrSlThompsonSamplingMcsController::m_discountFactor),
                          MakeDoubleChecker<double>(0.0, 1.0))
            .AddAttribute("TargetBler",
                          "The highest allowed block error rate for the selected MCS",
                          DoubleValue(0.1),
                          MakeDoubleAccessor(&NrSlThompsonSamplingMcsController::m_targetBler),
                          MakeDoubleChecker<double>(0.0, 1.0))
            .AddAttribute("MaximumL2Id",
                          "The highest destination L2 ID value to control",
                          UintegerValue(200),
                          MakeUintegerAccessor(&NrSlThompsonSamplingMcsController::m_maximumL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "PropagateOutcomes",
                "Indicates if recorded outcomes affect more than one arm."
                "If true, successes are counted for all lower MCS values"
                "and failures are counted for all higher MCS values.",
                BooleanValue(false),
                MakeBooleanAccessor(&NrSlThompsonSamplingMcsController::m_propagateOutcomes),
                MakeBooleanChecker())
            .AddAttribute("DecayInterval",
                          "Time interval for time-based decay of unvisited arms. "
                          "Arms not updated by observation-based decay are decayed by "
                          "discount^(elapsed_time / interval) at each GetMcs() call.",
                          TimeValue(MilliSeconds(100)),
                          MakeTimeAccessor(&NrSlThompsonSamplingMcsController::m_decayInterval),
                          MakeTimeChecker())
            .AddAttribute(
                "McsIndex",
                "The MCS index to use",
                AttributeContainerValue<UintegerValue>(),
                MakeAttributeContainerAccessor<UintegerValue>(
                    &NrSlThompsonSamplingMcsController::m_mcsIndex),
                MakeAttributeContainerChecker<UintegerValue>(MakeUintegerChecker<uint8_t>()))
            .AddTraceSource(
                "TrialResult",
                "The outcome of an armed bandit trial.",
                MakeTraceSourceAccessor(&NrSlThompsonSamplingMcsController::m_trialResultTrace),
                "ns3::NrSlThompsonSamplingMcsController::TrialResultCallback");

    return tid;
}

NrSlThompsonSamplingMcsController::NrSlThompsonSamplingMcsController()
{
    NS_LOG_FUNCTION(this);
    m_gammaRandomVariable = CreateObject<GammaRandomVariable>();
}

int64_t
NrSlThompsonSamplingMcsController::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_gammaRandomVariable->SetStream(stream);
    return 1;
}

void
NrSlThompsonSamplingMcsController::NotifyHarqFeedbackReceived(const SlHarqInfo& slHarqInfo)
{
    NS_LOG_FUNCTION(this << slHarqInfo);

    RecordOutcome(slHarqInfo.m_dstL2Id, slHarqInfo.m_mcs, slHarqInfo.IsReceivedOk());
}

GrantParams
NrSlThompsonSamplingMcsController::DoGetGrantParams(
    uint32_t dstL2Id,
    [[maybe_unused]] SidelinkInfo::CastType castType,
    [[maybe_unused]] std::optional<uint32_t> numRbs,
    [[maybe_unused]] std::optional<uint32_t> tbSize)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    if (dstL2Id > m_maximumL2Id)
    {
        return {};
    }
    auto [it, inserted] = m_armStats.try_emplace(dstL2Id, CreateArms());
    auto& arms = it->second;
    if (inserted)
    {
        NS_LOG_DEBUG("Initialized " << arms.size() << " arms for destination " << dstL2Id);
    }

    TimeBasedDecay(dstL2Id, arms);

    double successThreshold = 1.0 - m_targetBler;
    uint8_t bestMcs = 0;

    for (auto& [mcs, arm] : arms)
    {
        double theta = SampleBeta(1.0 + arm.success, 1.0 + arm.fails);
        NS_LOG_DEBUG("MCS " << +mcs << " success=" << arm.success << " fails=" << arm.fails
                            << " theta=" << theta);
        if (theta >= successThreshold)
        {
            bestMcs = mcs;
        }
    }

    NS_LOG_INFO("Selected MCS " << +bestMcs << " for destination " << dstL2Id);
    return {bestMcs, std::nullopt};
}

void
NrSlThompsonSamplingMcsController::RecordOutcome(uint32_t dstL2Id, uint8_t mcs, bool success)
{
    NS_LOG_FUNCTION(this << dstL2Id << +mcs << success);
    auto [it, inserted] = m_armStats.try_emplace(dstL2Id, CreateArms());
    auto& arms = it->second;

    if (inserted)
    {
        NS_LOG_DEBUG("Initialized " << arms.size() << " arms for destination " << dstL2Id);
    }

    for (auto& [armMcs, arm] : arms)
    {
        if (success && (armMcs == mcs || (m_propagateOutcomes && armMcs < mcs)))
        {
            Decay(arm, armMcs);
            arm.success += 1.0;
            NS_LOG_DEBUG("Updated MCS " << +armMcs << " for destination " << dstL2Id
                                        << ": success=" << arm.success << " fails=" << arm.fails);
        }
        else if (!success && (armMcs == mcs || (m_propagateOutcomes && armMcs > mcs)))
        {
            Decay(arm, armMcs);
            arm.fails += 1.0;
            NS_LOG_DEBUG("Updated MCS " << +armMcs << " for destination " << dstL2Id
                                        << ": success=" << arm.success << " fails=" << arm.fails);
        }
    }

    m_trialResultTrace(mcs, dstL2Id, success);
}

void
NrSlThompsonSamplingMcsController::Flush(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    m_armStats.erase(dstL2Id);
}

double
NrSlThompsonSamplingMcsController::SampleBeta(double alpha, double beta) const
{
    double x = m_gammaRandomVariable->GetValue(alpha, 1.0);
    double y = m_gammaRandomVariable->GetValue(beta, 1.0);
    return x / (x + y);
}

void
NrSlThompsonSamplingMcsController::TimeBasedDecay(uint32_t dstL2Id,
                                                  std::map<uint8_t, ArmStats>& arms)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    if (m_discountFactor >= 1 || m_decayInterval.IsZero())
    {
        return;
    }
    Time now = Simulator::Now();
    for (auto& [mcs, arm] : arms)
    {
        if (arm.lastDecay < now)
        {
            int64_t missed = ((now - arm.lastDecay) / m_decayInterval).GetHigh();
            if (missed > 0)
            {
                double factor = std::pow(m_discountFactor, static_cast<double>(missed));
                arm.success *= factor;
                arm.fails *= factor;
                arm.lastDecay = now;
                NS_LOG_DEBUG("Time-based decay for MCS "
                             << +mcs << ": missed intervals=" << missed << " factor=" << factor
                             << " success=" << arm.success << " fails=" << arm.fails);
            }
        }
    }
}

void
NrSlThompsonSamplingMcsController::Decay(ArmStats& stats, uint8_t mcs)
{
    NS_LOG_FUNCTION(this << mcs);
    if (m_discountFactor < 1)
    {
        NS_LOG_DEBUG("Applying discount factor " << m_discountFactor << " to decay stats for arm "
                                                 << +mcs);
        stats.success *= m_discountFactor;
        stats.fails *= m_discountFactor;
        stats.lastDecay = Simulator::Now();
    }
}

std::map<uint8_t, ns3::NrSlThompsonSamplingMcsController::ArmStats>
NrSlThompsonSamplingMcsController::CreateArms()
{
    NS_LOG_FUNCTION(this);
    std::map<uint8_t, ArmStats> arms;

    if (m_mcsIndex.empty())
    {
        m_mcsIndex = DEFAULT_MCS_INDEX;
    }

    for (auto mcs : m_mcsIndex)
    {
        arms[mcs] = ArmStats();
    }
    return arms;
}

void
NrSlThompsonSamplingMcsController::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_armStats.clear();
    NrSlMcsController::DoDispose();
}

void
NrSlThompsonSamplingMcsController::DoSetNrSlUeMacScheduler(
    [[maybe_unused]] Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    PointerValue nrSlUeMacHarqPtr;
    m_mac->GetAttribute("NrSlUeMacHarq", nrSlUeMacHarqPtr);
    DynamicCast<NrSlUeMacHarq>(nrSlUeMacHarqPtr.GetObject())
        ->TraceConnectWithoutContext(
            "HarqFeedbackReceived",
            MakeCallback(&NrSlThompsonSamplingMcsController::NotifyHarqFeedbackReceived, this));
}

} // namespace ns3
