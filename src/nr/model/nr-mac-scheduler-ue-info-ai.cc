// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-ue-info-ai.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerUeInfoAi");

std::vector<NrMacSchedulerUeInfoAi::LcObservation>
NrMacSchedulerUeInfoAi::GetDlObservation()
{
    NS_LOG_FUNCTION(this);
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> observations;
    for (const auto& ueLcg : m_dlLCG)
    {
        std::vector<uint8_t> ueActiveLCs = ueLcg.second->GetActiveLCIds();

        for (const auto lcId : ueActiveLCs)
        {
            std::unique_ptr<NrMacSchedulerLC>& LCPtr = ueLcg.second->GetLC(lcId);

            NrMacSchedulerUeInfoAi::LcObservation lcObservation = {
                m_rnti,
                lcId,
                LCPtr->m_qci,
                LCPtr->m_priority,
                LCPtr->m_rlcTransmissionQueueHolDelay};

            observations.push_back(lcObservation);
        }
    }
    return observations;
}

std::vector<NrMacSchedulerUeInfoAi::LcObservation>
NrMacSchedulerUeInfoAi::GetUlObservation()
{
    NS_LOG_FUNCTION(this);
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> observations;
    for (const auto& ueLcg : m_ulLCG)
    {
        std::vector<uint8_t> ueActiveLCs = ueLcg.second->GetActiveLCIds();

        for (const auto lcId : ueActiveLCs)
        {
            std::unique_ptr<NrMacSchedulerLC>& LCPtr = ueLcg.second->GetLC(lcId);

            NrMacSchedulerUeInfoAi::LcObservation lcObservation = {
                m_rnti,
                lcId,
                LCPtr->m_qci,
                LCPtr->m_priority,
                LCPtr->m_rlcTransmissionQueueHolDelay};

            observations.push_back(lcObservation);
        }
    }
    return observations;
}

void
NrMacSchedulerUeInfoAi::UpdateDlWeights(NrMacSchedulerUeInfoAi::Weights& weights)
{
    m_weightsDl = weights;
}

void
NrMacSchedulerUeInfoAi::UpdateUlWeights(NrMacSchedulerUeInfoAi::Weights& weights)
{
    m_weightsUl = weights;
}

float
NrMacSchedulerUeInfoAi::GetDlReward()
{
    float reward = 0.0;
    for (const auto& ueLcg : m_dlLCG)
    {
        std::vector<uint8_t> ueActiveLCs = ueLcg.second->GetActiveLCIds();

        for (const auto lcId : ueActiveLCs)
        {
            std::unique_ptr<NrMacSchedulerLC>& LCPtr = ueLcg.second->GetLC(lcId);
            if (m_avgTputDl == 0 || LCPtr->m_rlcTransmissionQueueHolDelay == 0)
            {
                continue;
            }
            reward += std::pow(m_potentialTputDl, m_alpha) /
                      (std::max(1E-9, m_avgTputDl) * LCPtr->m_priority *
                       LCPtr->m_rlcTransmissionQueueHolDelay);
        }
    }

    return reward;
}

float
NrMacSchedulerUeInfoAi::GetUlReward()
{
    float reward = 0.0;
    for (const auto& ueLcg : m_ulLCG)
    {
        std::vector<uint8_t> ueActiveLCs = ueLcg.second->GetActiveLCIds();

        for (const auto lcId : ueActiveLCs)
        {
            std::unique_ptr<NrMacSchedulerLC>& LCPtr = ueLcg.second->GetLC(lcId);
            if (m_avgTputUl == 0 || LCPtr->m_rlcTransmissionQueueHolDelay == 0)
            {
                continue;
            }
            reward += std::pow(m_potentialTputUl, m_alpha) /
                      (std::max(1E-9, m_avgTputUl) * LCPtr->m_priority *
                       LCPtr->m_rlcTransmissionQueueHolDelay);
        }
    }

    return reward;
}

} // namespace ns3
