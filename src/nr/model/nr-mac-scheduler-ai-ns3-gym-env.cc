// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ai-ns3-gym-env.h"

#ifdef HAVE_OPENGYM

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NrMacSchedulerAiNs3GymEnv");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerAiNs3GymEnv);

NrMacSchedulerAiNs3GymEnv::NrMacSchedulerAiNs3GymEnv()
{
    NS_LOG_FUNCTION(this);
}

NrMacSchedulerAiNs3GymEnv::NrMacSchedulerAiNs3GymEnv(uint32_t numFlows)
{
    NS_LOG_FUNCTION(this);
    m_numFlows = numFlows;
}

NrMacSchedulerAiNs3GymEnv::~NrMacSchedulerAiNs3GymEnv()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrMacSchedulerAiNs3GymEnv::GetTypeId()
{
    static TypeId tid = TypeId("NrMacSchedulerAiNs3GymEnv")
                            .SetParent<OpenGymEnv>()
                            .AddConstructor<NrMacSchedulerAiNs3GymEnv>();
    return tid;
}

void
NrMacSchedulerAiNs3GymEnv::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

Ptr<OpenGymSpace>
NrMacSchedulerAiNs3GymEnv::GetActionSpace()
{
    NS_LOG_FUNCTION(this);
    float low = 0.0;
    float high = m_numFlows;
    std::vector<uint32_t> shape = {m_numFlows};
    std::string dtype = TypeNameGet<float>();
    return Create<OpenGymBoxSpace>(low, high, shape, dtype);
}

Ptr<OpenGymSpace>
NrMacSchedulerAiNs3GymEnv::GetObservationSpace()
{
    NS_LOG_FUNCTION(this);
    float low = 0.0;
    float high = 100.0;
    std::vector<uint32_t> shape = {
        m_numFlows,
        4,
    };
    std::string dtype = TypeNameGet<uint16_t>();
    return Create<OpenGymBoxSpace>(low, high, shape, dtype);
}

bool
NrMacSchedulerAiNs3GymEnv::GetGameOver()
{
    NS_LOG_FUNCTION(this);
    return m_gameOver;
}

Ptr<OpenGymDataContainer>
NrMacSchedulerAiNs3GymEnv::GetObservation()
{
    NS_LOG_FUNCTION(this);
    std::vector<uint32_t> shape = {
        m_numFlows,
        4,
    };
    Ptr<OpenGymBoxContainer<uint16_t>> observation =
        CreateObject<OpenGymBoxContainer<uint16_t>>(shape);
    for (auto& obs : m_observation)
    {
        observation->AddValue(obs.rnti);
        observation->AddValue(obs.lcId);
        observation->AddValue(obs.priority);
        observation->AddValue(obs.holDelay);
    }
    return observation;
}

float
NrMacSchedulerAiNs3GymEnv::GetReward()
{
    NS_LOG_FUNCTION(this);
    return m_reward;
}

std::string
NrMacSchedulerAiNs3GymEnv::GetExtraInfo()
{
    NS_LOG_FUNCTION(this);
    return m_extraInfo;
}

bool
NrMacSchedulerAiNs3GymEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    NS_LOG_FUNCTION(this);
    Ptr<OpenGymBoxContainer<float>> actionBox = DynamicCast<OpenGymBoxContainer<float>>(action);
    std::vector<float> actionData = actionBox->GetData();
    NrMacSchedulerUeInfoAi::UeWeightsMap ueWeightsMap;
    for (uint32_t i = 0; i < m_numFlows; i++)
    {
        if (ueWeightsMap.end() == ueWeightsMap.find(m_observation[i].rnti))
        {
            ueWeightsMap[m_observation[i].rnti] = NrMacSchedulerUeInfoAi::Weights();
        }
        ueWeightsMap[m_observation[i].rnti][m_observation[i].lcId] = actionData[i];
    }
    m_updateAllUeWeightsFn(ueWeightsMap);
    return true;
}

void
NrMacSchedulerAiNs3GymEnv::NotifyCurrentIteration(
    const std::vector<NrMacSchedulerUeInfoAi::LcObservation>& observations,
    bool isGameOver,
    float reward,
    const std::string& extraInfo,
    const NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn& updateAllUeWeightsFn)
{
    NS_LOG_FUNCTION(this);
    m_observation = observations;
    m_gameOver = isGameOver;
    m_reward = reward;
    m_extraInfo = extraInfo;
    m_updateAllUeWeightsFn = updateAllUeWeightsFn;
    Notify();
}

} // namespace ns3

#endif // HAVE_OPENGYM
