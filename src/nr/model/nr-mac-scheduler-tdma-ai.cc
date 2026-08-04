// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-tdma-ai.h"

#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/log.h>

#include <algorithm>
#include <functional>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerTdmaAi");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerTdmaAi);

TypeId
NrMacSchedulerTdmaAi::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacSchedulerTdmaAi")
            .SetParent<NrMacSchedulerTdmaQos>()
            .AddConstructor<NrMacSchedulerTdmaAi>()
            .AddAttribute("NotifyCbDl",
                          "The callback function to notify the AI model for the downlink",
                          CallbackValue(MakeNullCallback<NrMacSchedulerUeInfoAi::NotifyCb>()),
                          MakeCallbackAccessor(&NrMacSchedulerTdmaAi::m_notifyCbDl),
                          MakeCallbackChecker())
            .AddAttribute("NotifyCbUl",
                          "The callback function to notify the AI model for the uplink",
                          CallbackValue(MakeNullCallback<NrMacSchedulerUeInfoAi::NotifyCb>()),
                          MakeCallbackAccessor(&NrMacSchedulerTdmaAi::m_notifyCbUl),
                          MakeCallbackChecker())
            .AddAttribute("ActiveDlAi",
                          "The flag to activate the AI model for the downlink",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrMacSchedulerTdmaAi::m_activeDlAi),
                          MakeBooleanChecker())
            .AddAttribute("ActiveUlAi",
                          "The flag to activate the AI model for the uplink",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrMacSchedulerTdmaAi::m_activeUlAi),
                          MakeBooleanChecker());
    return tid;
}

NrMacSchedulerTdmaAi::NrMacSchedulerTdmaAi()
    : NrMacSchedulerTdmaQos()
{
}

std::shared_ptr<NrMacSchedulerUeInfo>
NrMacSchedulerTdmaAi::CreateUeRepresentation(
    const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const
{
    NS_LOG_FUNCTION(this);
    return std::make_shared<NrMacSchedulerUeInfoAi>(
        m_alpha,
        params.m_rnti,
        params.m_beamId,
        std::bind(&NrMacSchedulerTdmaAi::GetNumRbPerRbg, this));
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaAi::GetUeCompareDlFn() const
{
    if (m_activeDlAi)
    {
        return NrMacSchedulerUeInfoAi::CompareUeWeightsDl;
    }
    return NrMacSchedulerUeInfoQos::CompareUeWeightsDl;
}

std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                   const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
NrMacSchedulerTdmaAi::GetUeCompareUlFn() const
{
    if (m_activeUlAi)
    {
        return NrMacSchedulerUeInfoAi::CompareUeWeightsUl;
    }
    return NrMacSchedulerUeInfoQos::CompareUeWeightsUl;
}

void
NrMacSchedulerTdmaAi::SetNotifyCbDl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb)
{
    NS_LOG_FUNCTION(this);
    m_notifyCbDl = notifyCb;
    m_activeDlAi = true;
}

void
NrMacSchedulerTdmaAi::SetNotifyCbUl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb)
{
    NS_LOG_FUNCTION(this);
    m_notifyCbUl = notifyCb;
    m_activeUlAi = true;
}

std::vector<NrMacSchedulerUeInfoAi::LcObservation>
NrMacSchedulerTdmaAi::GetUeObservationsDl(
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
NrMacSchedulerTdmaAi::GetUeObservationsUl(
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
NrMacSchedulerTdmaAi::GetIsGameOverDl() const
{
    return false;
}

bool
NrMacSchedulerTdmaAi::GetIsGameOverUl() const
{
    return false;
}

float
NrMacSchedulerTdmaAi::GetUeRewardsDl(
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
NrMacSchedulerTdmaAi::GetUeRewardsUl(
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
NrMacSchedulerTdmaAi::CallNotifyDlFn(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    if (!m_notifyCbDl.IsNull())
    {
        std::string extraInfo = "";
        NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn updateWeightsFn =
            std::bind(&NrMacSchedulerTdmaAi::UpdateAllUeWeightsDl,
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
NrMacSchedulerTdmaAi::CallNotifyUlFn(
    const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const
{
    NS_LOG_FUNCTION(this);
    if (!m_notifyCbUl.IsNull())
    {
        std::string extraInfo = "";
        NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn updateWeightsFn =
            std::bind(&NrMacSchedulerTdmaAi::UpdateAllUeWeightsUl,
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
NrMacSchedulerTdmaAi::UpdateAllUeWeightsDl(
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
NrMacSchedulerTdmaAi::UpdateAllUeWeightsUl(
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
