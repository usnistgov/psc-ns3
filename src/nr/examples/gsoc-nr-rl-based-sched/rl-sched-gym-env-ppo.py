#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Seoul National University (SNU)
# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

import argparse
from collections import deque
from types import MappingProxyType

import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from ns3gym import ns3env


class PPO:
    """!
    Proximal Policy Optimization (PPO) implementation for training a reinforcement learning agent.

    @param state_shape: The shape of the state space, typically a tuple representing the observation dimensions.
    @param hidden_dim: The size of the hidden layers for the neural network (default is 64).
    @param lr: Learning rate for the optimizer (default is 0.0003).
    @param gamma: Discount factor for future rewards (default is 0.99).
    @param eps_clip: Clipping value for the surrogate loss to stabilize training (default is 0.2).
    @param k_epochs: Number of epochs for which PPO updates are applied (default is 4).

    @return PPO object for policy optimization during the RL simulation.
    """

    def __init__(
        self,
        state_shape,
        hidden_dim=64,
        lr=0.0003,
        gamma=0.99,
        eps_clip=0.2,
        k_epochs=4,
    ):
        self.num_flows = state_shape[0]  # Number of flows in the environment

        self.gamma = gamma
        self.eps_clip = eps_clip
        self.k_epochs = k_epochs

        self.device = torch.device(
            "cuda" if torch.cuda.is_available() and args.enableCuda else "cpu"
        )  # GPU or CPU
        if args.enableCuda and not torch.cuda.is_available():
            print("CUDA is not available. Using CPU instead.")
        print(f"Using Device: {str(self.device).upper()}")

        action_dim = state_shape[0]  # The number of actions per row is the same as state_shape[0]
        self.policy = self.ActorCritic(state_shape, action_dim, hidden_dim).to(self.device)
        self.optimizer = optim.Adam(self.policy.parameters(), lr=lr)
        self.policy_old = self.ActorCritic(state_shape, action_dim, hidden_dim).to(self.device)
        self.policy_old.load_state_dict(self.policy.state_dict())

        self.mse_loss = nn.MSELoss()

    class ActorCritic(nn.Module):
        """!
        A neural network module that contains both the actor and critic components for PPO.

        @param state_shape: Shape of the state space (e.g., for a 3D observation, it could be (batch_size, 3, 4)).
        @param action_dim: Number of possible actions the agent can take.
        @param hidden_dim: Dimension of hidden layers for both actor and critic networks.

        @return ActorCritic object with methods to compute actions and evaluate states.
        """

        def __init__(self, state_shape, action_dim, hidden_dim):
            super(PPO.ActorCritic, self).__init__()

            self.action_dim = action_dim  # Store action_dim as an instance variable

            # Actor Network outputs mean and log_std for Normal distribution
            self.actor = nn.Sequential(
                nn.Linear(state_shape[1], hidden_dim),  # Expect input of shape (4,)
                nn.ReLU(),
                nn.Linear(hidden_dim, hidden_dim),
                nn.ReLU(),
                nn.Linear(hidden_dim, 2),  # Output: 2 (mean and log_std) for each row
                nn.Tanh(),  # Add Tanh activation to limit the range of mean and log_std
            )

            # Critic Network for value estimation
            self.critic = nn.Sequential(
                nn.Linear(
                    state_shape[0] * state_shape[1], hidden_dim
                ),  # Input size is 12 after flattening
                nn.ReLU(),
                nn.Linear(hidden_dim, 1),
            )

        def forward(self):
            raise NotImplementedError

        def act(self, state, mask, device):
            """!
            Takes in the current state of the environment and outputs a continuous action based on the policy.

            @param state: The state of the environment, usually a multi-dimensional array (e.g., (3, 4)).
            @param mask: Boolean mask indicating which rows are active.
            @return Tuple containing the selected action and the log-probability of that action.
            """
            if len(state.shape) == 2:  # If state is (3, 4), add batch dimension
                state = state[np.newaxis, :]  # Shape: (1, 3, 4)

            state = torch.from_numpy(state).float().to(device)  # Convert state to tensor
            batch_size, num_flows, _ = state.size()

            actions = []
            action_log_probs = []
            eps = 1e-6  # Small value to prevent log(0) errors

            for i in range(num_flows):
                if mask[i]:  # If the row is active
                    actor_output = self.actor(state[:, i, :])  # Shape: (batch_size, 2)

                    # Scaling after Tanh activation to adjust the mean value
                    mean, log_std = (
                        actor_output[:, 0] * (num_flows - 1) / 2 + (num_flows - 1) / 2,
                        actor_output[:, 1],
                    )

                    # Limit the log_std value
                    log_std = torch.clamp(
                        log_std, -2, 2
                    )  # Log standard deviation within a reasonable range

                    std = torch.exp(log_std)  # Convert log_std to std
                    dist = torch.distributions.Normal(mean, std)
                    action = dist.sample()  # Sample action from the distribution

                    # Ensure action is within (0, num_flows - 1]
                    action = torch.clamp(
                        action, eps, num_flows - 1
                    )  # Adjust the range to `num_flows - 1`

                    actions.append(action)
                    action_log_probs.append(dist.log_prob(action))  # Save log probability

                    # Print statements for debugging
                    debug(
                        f"Selected action for flow {i}: {action.item()} (mean: {mean.item()}, std: {std.item()})",
                        args.debug,
                    )
                else:  # Inactive row
                    actions.append(torch.tensor([0.0]).to(device))  # Set action to 0
                    action_log_probs.append(
                        torch.tensor([0.0]).to(device)
                    )  # Log-probability is also 0
                    debug(f"Zero action for flow {i}", args.debug)

            # Convert list of actions to a single tensor
            actions = torch.stack(actions).squeeze()  # Shape: (num_flows,)
            action_log_probs = torch.stack(action_log_probs).squeeze()  # Shape: (num_flows,)

            return actions.detach().cpu().numpy(), action_log_probs

        def evaluate(self, state, action, mask):
            """!
            Evaluates the state and action to compute log-probabilities, state values, and entropy.

            @param state: The state from the environment, potentially batch-processed.
            @param action: The action taken by the agent.
            @param mask: Boolean mask indicating which rows are active.
            @return Tuple containing the log-probabilities of the actions, the state values (critic), and the entropy.
            """
            # Check if state and action are already tensors
            if not isinstance(state, torch.Tensor):
                state = torch.from_numpy(state).float()  # Convert state to tensor if not already
            if not isinstance(action, torch.Tensor):
                action = torch.from_numpy(action).float()  # Convert action to tensor if not already

            if len(state.shape) == 2:  # If state is (3, 4), add batch dimension
                state = state[np.newaxis, :]  # Shape: (1, 3, 4)
            if len(action.shape) == 1:  # If action is (3,), add batch dimension
                action = action[np.newaxis, :]  # Shape: (1, 3)

            batch_size, num_flows, _ = state.size()
            action_log_probs = []
            entropies = []

            for i in range(num_flows):
                if mask[i]:  # If the row is active
                    actor_output = self.actor(state[:, i, :])  # Shape: (batch_size, 2)
                    mean, log_std = actor_output[:, 0], actor_output[:, 1]
                    std = torch.exp(log_std)
                    dist = torch.distributions.Normal(mean, std)

                    log_prob = dist.log_prob(action[:, i])  # Shape: (batch_size,)
                    entropy = dist.entropy()  # Shape: (batch_size,)

                    # Check if log_prob and entropy need dimension adjustment
                    if log_prob.dim() == 0:
                        log_prob = log_prob.unsqueeze(0)
                    if entropy.dim() == 0:
                        entropy = entropy.unsqueeze(0)

                    action_log_probs.append(log_prob)  # Add with correct dimensions
                    entropies.append(entropy)
                else:
                    # Add zero tensors with appropriate batch size
                    action_log_probs.append(torch.zeros(batch_size))
                    entropies.append(torch.zeros(batch_size))

            # Check dimensions and apply unsqueeze if needed
            action_log_probs = [
                log_prob.unsqueeze(1) if log_prob.dim() == 1 else log_prob
                for log_prob in action_log_probs
            ]
            entropies = [
                entropy.unsqueeze(1) if entropy.dim() == 1 else entropy for entropy in entropies
            ]

            # Stack tensors along new dimension
            action_log_probs = torch.stack(
                action_log_probs, dim=1
            )  # Shape: (batch_size, num_flows)
            dist_entropy = torch.stack(entropies, dim=1).mean()  # Average over all objects

            # Flatten state for critic input
            flattened_state = state.view(state.shape[0], -1)  # Flatten each sample in the batch
            state_value = self.critic(flattened_state)  # Pass flattened states to the critic

            return action_log_probs, torch.squeeze(state_value), dist_entropy

    def select_action(self, state, memory, mask):
        """!
        Selects an action based on the current policy and stores relevant information in memory.

        @param state: The current state of the environment.
        @param memory: An instance of the Memory class to store states, actions, and log probabilities for future updates.
        @param mask: Boolean mask indicating which rows are active.
        @return action: The action selected by the policy.
        """
        action, action_logprob = self.policy_old.act(state, mask, self.device)
        memory.states.append(state)
        memory.actions.append(action)
        memory.logprobs.append(action_logprob)
        memory.mask.append(mask)  # Store mask in memory for future updates
        return action

    def update(self, memory):
        """!
        Updates the policy using the memory of past actions and states based on the PPO update rule.

        @param memory: Memory instance containing past states, actions, log probabilities, and rewards.
        @return None: This function updates the policy parameters based on the experience stored in memory.
        """
        rewards = []
        discounted_reward = 0
        for reward, is_terminal in zip(reversed(memory.rewards), reversed(memory.is_terminals)):
            if is_terminal:
                discounted_reward = 0
            discounted_reward = reward + (self.gamma * discounted_reward)
            rewards.insert(0, discounted_reward)

        rewards = torch.tensor(rewards, dtype=torch.float32).to(self.device)
        rewards = (rewards - rewards.mean()) / (rewards.std() + 1e-5)

        old_states = memory.states
        old_actions = memory.actions

        # Convert old_states and old_actions to tensors if they are not already
        if not isinstance(old_states, torch.Tensor):
            old_states = torch.tensor(np.array(old_states), dtype=torch.float32).to(self.device)
        if not isinstance(old_actions, torch.Tensor):
            old_actions = torch.tensor(np.array(old_actions), dtype=torch.float32).to(self.device)

        # Convert logprobs more efficiently to avoid warnings
        if isinstance(memory.logprobs[0], torch.Tensor):
            old_logprobs = torch.stack([lp.detach() for lp in memory.logprobs]).to(
                self.device
            )  # Stack tensors directly
        else:
            old_logprobs = torch.tensor(np.array(memory.logprobs), dtype=torch.float32).to(
                self.device
            )

        # Adjust the size of old_logprobs to match the size of logprobs from evaluate function
        old_logprobs = old_logprobs.view(
            -1, self.num_flows
        )  # Adjust the shape to match logprobs from evaluate

        for _ in range(self.k_epochs):
            logprobs, state_values, dist_entropy = self.policy.evaluate(
                old_states, old_actions, memory.mask
            )

            # Adjust size of logprobs to match old_logprobs if needed
            logprobs = logprobs.view(-1, self.num_flows)  # Adjust the shape to match old_logprobs

            ratios = torch.exp(logprobs - old_logprobs.detach())

            advantages = rewards - state_values.detach()
            advantages = advantages.unsqueeze(1)  # Add a new dimension for broadcasting

            surr1 = ratios * advantages
            surr2 = torch.clamp(ratios, 1 - self.eps_clip, 1 + self.eps_clip) * advantages

            loss = (
                -torch.min(surr1, surr2)
                + 0.5 * self.mse_loss(state_values, rewards)
                - 0.01 * dist_entropy
            )

            self.optimizer.zero_grad()
            loss.mean().backward()
            self.optimizer.step()

        self.policy_old.load_state_dict(self.policy.state_dict())


class Memory:
    """!
    Memory class for storing states, actions, log probabilities, rewards, and terminal flags during training.

    The PPO algorithm requires storage of past experiences (states, actions, rewards, etc.) so that they can be
    used during the policy update step. This class implements a memory buffer using dequeues with a fixed size to
    prevent excessive memory consumption.

    @param maxlen: Maximum length of the memory buffer. The default value is 1000, which means the memory will hold
                   up to 1000 samples before older samples start being discarded.

    @return Memory object for storing experiences during the RL simulation.
    """

    def __init__(self, maxlen=1000):
        self.states = deque(maxlen=maxlen)
        self.actions = deque(maxlen=maxlen)
        self.logprobs = deque(maxlen=maxlen)
        self.rewards = deque(maxlen=maxlen)
        self.is_terminals = deque(maxlen=maxlen)
        self.mask = deque(maxlen=maxlen)

    def clear_memory(self):
        """!
        Clears the memory of stored states, actions, log probabilities, rewards, and terminal flags after an update.

        @return None: This function resets the memory buffers.
        """
        self.states.clear()
        self.actions.clear()
        self.logprobs.clear()
        self.rewards.clear()
        self.is_terminals.clear()
        self.mask.clear()  # Clear mask as well


def debug(msg, debug_flag):
    """!
    Prints debug messages if the debug flag is enabled.

    @param msg: The debug message to print.
    @param debug_flag: A boolean flag indicating whether to print debug information.
    @return None: This function prints debug messages when necessary.
    """
    if debug_flag:
        print(msg)


def create_flow_map(flow_order):
    """!
    Creates a flow order map for mapping flow order to indices.

    @param flow_order: A list of flow order tuples, where each tuple contains the flow ID and the UE ID.
    @return MappingProxyType: An immutable mapping of flow order tuples to indices.
    """
    mutable_flow_order = {tuple(row): i for i, row in enumerate(flow_order)}
    return MappingProxyType(mutable_flow_order)


def reorder_state(reshaped_obs, flow_order, num_features):
    """Reorders the state rows to match the obj_order and fills missing entries with zeros."""
    # Initialize the reordered state with zeros
    reordered_state = np.zeros((len(flow_order), num_features))

    # Iterate over the flow_order and match it with the new state
    for row in reshaped_obs:
        key = tuple(row[0:2])
        if key in flow_order:
            reordered_state[flow_order[key]] = row
    # If no matching row is found, the initialized zeros will remain

    mask = [0 if np.array_equal(row, np.zeros(num_features)) else 1 for row in reordered_state]

    return reordered_state, mask


def reorder_action(action, reshaped_obs, flow_order):
    reordered_action = []

    for row in reshaped_obs:
        key = tuple(row[0:2])
        if key in flow_order:
            reordered_action.append(action[flow_order[key]])

    return reordered_action


def main(args):
    """!
    Main function for running the PPO-based reinforcement learning simulation using the ns-3 gym environment.

    @param args: Command-line arguments containing simulation parameters, such as the number of UEs,
                 simulation time, bandwidth, and other necessary configuration options.

    @return None: This function runs the PPO agent in the environment, selecting actions, and updating the model.
    """
    simArgs = {
        "--ueNum": args.ueNum,
        "--logging": args.logging,
        "--priorityTrafficScenario": args.priorityTrafficScenario,
        "--simTime": args.simTime,
        "--numerology": args.numerology,
        "--centralFrequency": args.centralFrequency,
        "--bandwidth": args.bandwidth,
        "--totalTxPower": args.totalTxPower,
        "--simTag": args.simTag,
        "--outputDir": args.outputDir,
        "--enableOfdma": args.enableOfdma,
        "--enableLcLevelQos": args.enableLcLevelQos,
        "--ueLevelSchedulerType": "Ai",  # Type of scheduler being used in the simulation (here, AI-based)
    }
    # Create the environment
    env = ns3env.Ns3Env(port=args.port, simSeed=args.simSeed, simArgs=simArgs, debug=args.debug)
    state_shape = env.observation_space.shape
    action_dim = state_shape[0]
    ppo = PPO(state_shape, action_dim)
    memory = Memory(maxlen=args.stepInterval)

    # Training the agent
    step_idx = 0
    step_interval = args.stepInterval
    try:
        obs = env.reset()
        reshaped_obs = obs.reshape(state_shape)
        sorted_obs = np.lexsort((reshaped_obs[:, 1], reshaped_obs[:, 0]))
        flow_map = create_flow_map(reshaped_obs[sorted_obs, 0:2])
        while True:
            debug(f"Observation: \n{reshaped_obs}", args.debug)
            state, mask = reorder_state(reshaped_obs, flow_map, state_shape[1])
            debug(f"State: \n{state.reshape(state_shape)}", args.debug)
            debug(f"Mask: {mask}", args.debug)
            action = ppo.select_action(state, memory, mask)
            debug(f"Selected action: {action}", args.debug)
            reordered_action = reorder_action(action, reshaped_obs, flow_map)
            debug(f"Reordered action: {reordered_action}", args.debug)

            obs, reward, done, _ = env.step(reordered_action)
            reshaped_obs = obs.reshape(int(len(obs) / state_shape[1]), state_shape[1])
            debug(f"Reward: {reward}", args.debug)

            memory.rewards.append(reward)
            memory.is_terminals.append(done)

            if done:
                break

            if step_idx > 0 and step_idx % step_interval == 0:
                ppo.update(memory)
                memory.clear_memory()

            step_idx += 1

    except KeyboardInterrupt:
        print("Ctrl-C -> Exit")
    finally:
        env.close()
        print("Done")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser = argparse.ArgumentParser()
    # Arguments used for the gym script
    parser.add_argument("--port", type=int, default=5552, help="Port number")
    parser.add_argument("--simSeed", type=int, default=3002, help="Seed number")
    parser.add_argument("--debug", type=bool, default=False, help="Debug mode")
    parser.add_argument("--enableCuda", type=int, default=0, help="Enable CUDA")
    # Arguments used for the ns3 simulation (simArgs)
    parser.add_argument(
        "--ueNum", type=int, default=2, help="Number of UEs (User Equipment) in the simulation"
    )
    parser.add_argument(
        "--logging",
        type=bool,
        default=False,
        help="Enable or disable logging during the simulation",
    )
    parser.add_argument(
        "--priorityTrafficScenario",
        type=int,
        default=0,
        help="The traffic scenario for the case of priority. Can be 0: saturation or 1: medium-load",
    )
    parser.add_argument(
        "--simTime", type=float, default=1.0, help="Total simulation time in seconds"
    )
    parser.add_argument(
        "--numerology", type=int, default=0, help="Numerology parameter for the simulation"
    )
    parser.add_argument(
        "--centralFrequency",
        type=float,
        default=4e9,
        help="Central frequency for the simulation in Hz",
    )
    parser.add_argument(
        "--bandwidth", type=float, default=10e6, help="Bandwidth in Hz for the simulation"
    )
    parser.add_argument(
        "--totalTxPower", type=float, default=43, help="Total transmission power in dBm"
    )
    parser.add_argument(
        "--simTag", type=str, default="default", help="Tag to label the simulation for reference"
    )
    parser.add_argument(
        "--outputDir", type=str, default="./", help="Directory where output data will be stored"
    )
    parser.add_argument(
        "--enableOfdma", type=int, default=0, help="Whether to enable OFDMA in the simulation"
    )
    parser.add_argument(
        "--enableLcLevelQos",
        type=int,
        default=0,
        help="Whether to enable LC assignment based on QoS requirements in the simulation",
    )
    # Update interval for the PPO algorithm
    parser.add_argument(
        "--stepInterval", type=int, default=1000, help="Step interval for updating the PPO"
    )
    args = parser.parse_args()

    main(args)
