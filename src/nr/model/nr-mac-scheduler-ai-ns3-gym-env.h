// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#if __has_include("ns3/opengym-module.h")
#define HAVE_OPENGYM

#include "ns3/core-module.h"
#include "ns3/nr-module.h"
#include "ns3/opengym-module.h"

namespace ns3
{
/**
 * \brief The Gym environment for the RL-based scheduler
 *
 * This class extends the OpenGymEnv class and implements the Gym environment for the RL-based
 * scheduler. The environment receives observations, gameover status, rewards and extra information
 * from the scheduler and sends them to the RL model via the the OpenGymInterface. The class also
 * receives actions from the RL model and sends them to the scheduler.
 *
 * The OpenAI Gym framework supports the following spaces:
 * - Discrete: a discrete number between 0 and N
 * - Box: a vector or matrix of numbers of single type with values bounded between low and high
 * limits
 * - Tuple: a tuple of simpler spaces
 * - Dict: a dictionary of simpler spaces
 *
 * \see NotifyCurrentIteration
 * \see ExecuteActions
 */
class NrMacSchedulerAiNs3GymEnv : public OpenGymEnv
{
  public:
    /**
     * \brief Constructor for NrMacSchedulerAiNs3GymEnv
     * Initializes the Gym environment for the RL-based scheduler.
     */
    NrMacSchedulerAiNs3GymEnv();

    /**
     * \brief Constructor with number of UEs
     * \param numFlows The number of flows in the environment
     * Initializes the Gym environment with the given number of flows.
     */
    NrMacSchedulerAiNs3GymEnv(uint32_t numFlows);

    /**
     * \brief Destructor for NrMacSchedulerAiNs3GymEnv
     */
    ~NrMacSchedulerAiNs3GymEnv() override;

    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief Dispose of the object
     * Cleans up resources when the object is no longer needed.
     */
    void DoDispose() override;

    /**
     * \brief Get the action space of the environment
     * \return the action space definition
     *
     * Define the action space for the RL-based scheduler. The action space specifies
     * the range and type of actions that the RL model can take.  In this environment,
     * the action space is defined as a continuous space (a "box" space)
     * where each action corresponds to a specific weight for the flows managed by the scheduler.
     * The action values are bounded between `low` (0.0) and `high` (`m_numFlows`), and the action
     * space has a shape based on the number of flows (`m_numFlows`), meaning each flow is
     * associated with an individual action.
     */
    Ptr<OpenGymSpace> GetActionSpace() override;

    /**
     * \brief Get the observation space of the environment
     * \return the observation space definition
     *
     * Define the observation space for the RL-based scheduler. The observation space
     * specifies the structure of the state that the reinforcement learning model observes. In this
     * environment, the observation space is also defined as a continuous space where each
     * observation is a set of parameters describing the flows (e.g., RNTI, LCID, HOL delay,
     * priority). The observation values are bounded between `low` (0.0) and `high` (100.0) and the
     * space has a shape of `[m_numFlows, 4]`, meaning each flow has four observable properties.
     */
    Ptr<OpenGymSpace> GetObservationSpace() override;

    /**
     * \brief Check if the game is over
     * \return true if the game is over, false otherwise
     *
     * Determines whether the current episode in the Gym environment has ended.
     */
    bool GetGameOver() override;

    /**
     * \brief Get the current observation
     * \return the current observation
     *
     * Collect values of the current observation from the environment, which is used by the RL
     * model. The observation contains information about all flows, including  their RNTI, LCID,
     * priority, and HOL (head-of-line) delay. This data is packed into an `OpenGymBoxContainer`,
     * which is then returned for the RL model to process and use.
     */
    Ptr<OpenGymDataContainer> GetObservation() override;

    /**
     * \brief Get the reward for the current step
     * \return the reward value
     *
     * Return the reward achieved during the last step, which is used by the RL model to update its
     * policy.
     */
    float GetReward() override;

    /**
     * \brief Get extra information from the environment
     * \return additional information associated with current environment state
     *
     * Retrieve any extra information associated with the current environment state.
     */
    std::string GetExtraInfo() override;

    /**
     * \brief Execute actions received from the RL model
     * \param action The action received from the RL model
     * \return bool True if the action was executed successfully, false otherwise
     *
     * Apply the actions received from the RL model to the environment. The actions
     * are provided as a container (`OpenGymBoxContainer<float>`) where each element represents a
     * weight for a specific flow. The method updates the internal scheduler state by adjusting the
     * weights of the flows based on the RL model's decisions. After applying the actions, the
     * scheduler's behavior is updated accordingly.
     */
    bool ExecuteActions(Ptr<OpenGymDataContainer> action) override;

    /**
     * \brief Notify the environment about the current iteration
     * \param observations Observations from the scheduler
     * \param isGameOver Whether the game/episode is over
     * \param reward Reward for the current iteration
     * \param extraInfo Additional information
     * \param updateAllUeWeightsFn A callback function to update the weights of all UEs
     *
     * This method is called at each iteration of the simulation to provide the environment with
     * the latest observations, reward, and any other relevant information. The environment updates
     * its internal state (e.g., whether the game is over, what the current reward is) based on this
     * information. It also stores the callback function (`updateAllUeWeightsFn`) that will be used
     * to update the weights for all UEs when the RL model sends back its actions.
     */
    void NotifyCurrentIteration(
        const std::vector<NrMacSchedulerUeInfoAi::LcObservation>& observations,
        bool isGameOver,
        float reward,
        const std::string& extraInfo,
        const NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn& updateAllUeWeightsFn);

  private:
    uint32_t m_numFlows; //!< The number of flows in the environment
    bool m_gameOver;     //!< Whether the current game/episode is over
    std::vector<NrMacSchedulerUeInfoAi::LcObservation> m_observation; //!< Current observation data
    float m_reward;                                                   //!< The current reward
    std::string m_extraInfo; //!< Additional information for logging or debugging
    NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn
        m_updateAllUeWeightsFn; //!< Callback function to update UE weights
};
} // namespace ns3
#endif
