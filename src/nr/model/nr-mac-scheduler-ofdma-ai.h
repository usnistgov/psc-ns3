// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-scheduler-ofdma-qos.h"
#include "nr-mac-scheduler-ue-info-ai.h"

#include <ns3/traced-value.h>

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief The OFDMA scheduler with AI implementation
 *
 * This class extends the NrMacSchedulerOfdmaQos class and implements the AI
 * scheduler for the downlink and uplink. If the AI model is activated, the scheduler
 * uses the AI model to schedule the UEs. If the AI model is not activated, the scheduler
 * works in a similar manner as the QoS scheduler.
 *
 * When the AI model is activated, the scheduler sends observations to the OpenGymEnv
 * class in the ns3-gym module, which are used to train the AI model. The AI model then
 * sends back the weights for all flows of all UEs. The AI scheduler uses these weights
 * to schedule the UEs.
 *
 * The AI scheduler also sends rewards to the OpenGymEnv class, which are used
 * to train the AI model. All information needed by the gym is sent once through
 * the NotifyCb callback function for each iteration.
 *
 * Details in the class NrMacSchedulerUeInfoAI.
 */
class NrMacSchedulerOfdmaAi : public NrMacSchedulerOfdmaQos
{
    friend class NrTestSchedulerAiCase;

  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerOfdmaAi constructor
     */
    NrMacSchedulerOfdmaAi();

  protected:
    /**
     * \brief Create an UE representation of the type NrMacSchedulerUeInfoAi
     * \param params parameters
     * \return NrMacSchedulerUeInfo instance
     */
    std::shared_ptr<NrMacSchedulerUeInfo> CreateUeRepresentation(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) const override;

    /**
     * \brief Return the comparison function to sort DL UEs according to the scheduler policy
     * \return A pointer to NrMacSchedulerUeInfoAi::CompareUeWeightsDl if the AI model is activated,
     * otherwise, a pointer to NrMacSchedulerUeInfoQos::CompareUeWeightsDl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareDlFn() const override;

    /**
     * \brief Return the comparison function to sort UL UEs according to the scheduler policy
     * \return A pointer to NrMacSchedulerUeInfoAi::CompareUeWeightsUl if the AI model is activated,
     * otherwise, a pointer to NrMacSchedulerUeInfoQos::CompareUeWeightsUl
     */
    std::function<bool(const NrMacSchedulerNs3::UePtrAndBufferReq& lhs,
                       const NrMacSchedulerNs3::UePtrAndBufferReq& rhs)>
    GetUeCompareUlFn() const override;

    /**
     * \brief Set the notify callback function for downlink
     * \param notifyCb The callback function to be set
     */
    void SetNotifyCbDl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb);

    /**
     * \brief Set the notify callback function for uplink
     * \param notifyCb The callback function to be set
     */
    void SetNotifyCbUl(NrMacSchedulerUeInfoAi::NotifyCb notifyCb);

    /**
     * \brief Get UE observations for downlink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     * \return An Observation object representing the observations for all UEs
     */
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> GetUeObservationsDl(
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

    /**
     * \brief Get UE observations for uplink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     * \return An Observation object representing the observations for all UEs
     */
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> GetUeObservationsUl(
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

    /**
     * \brief Check if the downlink game is over
     * \return A boolean value indicating whether the downlink game is over (true) or not (false)
     */
    bool GetIsGameOverDl() const;

    /**
     * \brief Check if the uplink game is over
     * \return A boolean value indicating whether the downlink game is over (true) or not (false)
     */
    bool GetIsGameOverUl() const;

    /**
     * \brief Get rewards for downlink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     * \return A float value representing the calculated rewards
     */
    float GetUeRewardsDl(const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

    /**
     * \brief Get rewards for uplink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     * \return A float value representing the calculated rewards
     */
    float GetUeRewardsUl(const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

    /**
     * \brief Call the notify callback function in the OpenGymEnv class
     * in the ns3-gym module for downlink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     */
    void CallNotifyDlFn(
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const override;

    /**
     * \brief Call the notify callback function in the OpenGymEnv class
     * in the ns3-gym module for uplink
     * \param ueVector A vector containing pointers to active UEs and their corresponding buffer
     * requests
     */
    void CallNotifyUlFn(
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const override;

    /**
     * \brief Update weights of all UEs for downlink
     * \param ueWeights An unordered map where the key is the UE's RNTI (Radio Network Temporary
     * Identifier) and the value is the UE's weights for all flows
     * \param ueVector A vector
     * containing pointers to active UEs and their corresponding buffer requests
     */
    void UpdateAllUeWeightsDl(
        const NrMacSchedulerUeInfoAi::UeWeightsMap& ueWeights,
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

    /**
     * \brief Update weights of all UEs for uplink
     * \param ueWeights An unordered map where the key is the UE's RNTI (Radio Network Temporary
     * Identifier) and the value is the UE's weights for all flows
     * \param ueVector A vector
     * containing pointers to active UEs and their corresponding buffer requests
     */
    void UpdateAllUeWeightsUl(
        const NrMacSchedulerUeInfoAi::UeWeightsMap& ueWeights,
        const std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>& ueVector) const;

  private:
    float m_alpha{0.0};                            //!< PF Fairness index
    NrMacSchedulerUeInfoAi::NotifyCb m_notifyCbDl; //!< Notify callback function for downlink
    NrMacSchedulerUeInfoAi::NotifyCb m_notifyCbUl; //!< Notify callback function for uplink
};
} // namespace ns3
