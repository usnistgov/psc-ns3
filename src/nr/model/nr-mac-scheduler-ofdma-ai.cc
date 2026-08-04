// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ofdma-ai.h"

#include <ns3/boolean.h>
#include <ns3/log.h>

#include <algorithm>
#include <functional>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NrMacSchedulerOfdmaAi");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerOfdmaAi);

TypeId
NrMacSchedulerOfdmaAi::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacSchedulerOfdmaAi")
            .SetParent<NrMacSchedulerOfdmaQos>()
            .AddConstructor<NrMacSchedulerOfdmaAi>()
            .AddAttribute("NotifyCbDl",
                          "The callback function to notify the AI model for the downlink",
                          CallbackValue(MakeNullCallback<NrMacSchedulerUeInfoAi::NotifyCb>()),
                          MakeCallbackAccessor(&NrMacSchedulerOfdmaAi::m_notifyCbDl),
                          MakeCallbackChecker())
            .AddAttribute("NotifyCbUl",
                          "The callback function to notify the AI model for the uplink",
                          CallbackValue(MakeNullCallback<NrMacSchedulerUeInfoAi::NotifyCb>()),
                          MakeCallbackAccessor(&NrMacSchedulerOfdmaAi::m_notifyCbUl),
                          MakeCallbackChecker())
            .AddAttribute("ActiveDlAi",
                          "The flag to activate the AI model for the downlink",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrMacSchedulerOfdmaAi::m_activeDlAi),
                          MakeBooleanChecker())
            .AddAttribute("ActiveUlAi",
                          "The flag to activate the AI model for the uplink",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrMacSchedulerOfdmaAi::m_activeUlAi),
                          MakeBooleanChecker());
    return tid;
}

NrMacSchedulerOfdmaAi::NrMacSchedulerOfdmaAi()
    : NrMacSchedulerOfdmaQos()
{
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerOfdmaAi::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoAi>(
        m_alpha,
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerOfdmaAi::GetNumRbPerRbg, this));
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaAi::GetUeCompareDlFn() const
{
    if (m_activeDlAi)
    {
        return NrMacSchedulerUeInfoAi::CompareUeWeightsDl;
    }
    return NrMacSchedulerUeInfoQos::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerOfdmaAi::GetUeCompareUlFn() const
{
    if (m_activeUlAi)
    {
        return NrMacSchedulerUeInfoAi::CompareUeWeightsUl;
    }
    return NrMacSchedulerUeInfoQos::CompareUeWeightsUl;
}

void
NrMacSchedulerOfdmaAi::SetNotifyCbDl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb)
{
    NS_LOG_FUNCTION(this);
    m_notifyCbDl = notifyCb;
    m_activeDlAi = true;
}

void
NrMacSchedulerOfdmaAi::SetNotifyCbUl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb)
{
    NS_LOG_FUNCTION(this);
    m_notifyCbUl = notifyCb;
    m_activeUlAi = true;
}

std::vector<NrMacSchedulerUeInfoAi::LcObservation>
NrMacSchedulerOfdmaAi::GetUeObservationsDl(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> observations;
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        std::vector<NrMacSchedulerUeInfoAi::LcObservation> ueObservation =
            uePtr->GetDlObservation();
        observations.insert(observations.end(), ueObservation.begin(), ueObservation.end());
    }
    return observations;
}

std::vector<NrMacSchedulerUeInfoAi::LcObservation>
NrMacSchedulerOfdmaAi::GetUeObservationsUl(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> observations;
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        std::vector<NrMacSchedulerUeInfoAi::LcObservation> ueObservation =
            uePtr->GetUlObservation();
        observations.insert(observations.end(), ueObservation.begin(), ueObservation.end());
    }
    return observations;
}

bool
NrMacSchedulerOfdmaAi::GetIsGameOverDl() const
{
    return false;
}

bool
NrMacSchedulerOfdmaAi::GetIsGameOverUl() const
{
    return false;
}

float
NrMacSchedulerOfdmaAi::GetUeRewardsDl(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    float reward = 0.0;
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        reward += uePtr->GetDlReward();
    }
    return reward;
}

float
NrMacSchedulerOfdmaAi::GetUeRewardsUl(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    float reward = 0.0;
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        reward += uePtr->GetUlReward();
    }
    return reward;
}

void
NrMacSchedulerOfdmaAi::CallNotifyDlFn(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    if (!m_notifyCbDl.IsNull())
    {
        std::string extraInfo = "";
        NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn updateWeightsFn =
            std::bind(&NrMacSchedulerOfdmaAi::UpdateAllUeWeightsDl,
                      this,
                      std::placeholders::_1,
                      ueVector);
        m_notifyCbDl(GetUeObservationsDl(ueVector),
                     GetIsGameOverDl(),
                     GetUeRewardsDl(ueVector),
                     extraInfo,
                     updateWeightsFn);
    }
}

void
NrMacSchedulerOfdmaAi::CallNotifyUlFn(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    if (!m_notifyCbUl.IsNull())
    {
        std::string extraInfo = "";
        NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn updateWeightsFn =
            std::bind(&NrMacSchedulerOfdmaAi::UpdateAllUeWeightsUl,
                      this,
                      std::placeholders::_1,
                      ueVector);
        m_notifyCbUl(GetUeObservationsUl(ueVector),
                     GetIsGameOverUl(),
                     GetUeRewardsUl(ueVector),
                     extraInfo,
                     updateWeightsFn);
    }
}

void
NrMacSchedulerOfdmaAi::UpdateAllUeWeightsDl(
    const NrMacSchedulerUeInfoAi::UeWeightsMap& ueWeights,
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        NrMacSchedulerUeInfoAi::Weights weights = ueWeights.at(uePtr->m_rnti);
        uePtr->UpdateDlWeights(weights);
    }
}

void
NrMacSchedulerOfdmaAi::UpdateAllUeWeightsUl(
    const NrMacSchedulerUeInfoAi::UeWeightsMap& ueWeights,
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    for (const auto& ue : ueVector)
    {
        auto uePtr = std::dynamic_pointer_cast<NrMacSchedulerUeInfoAi>(ue.first);
        NrMacSchedulerUeInfoAi::Weights weights = ueWeights.at(uePtr->m_rnti);
        uePtr->UpdateUlWeights(weights);
    }
}

} // namespace ns3
