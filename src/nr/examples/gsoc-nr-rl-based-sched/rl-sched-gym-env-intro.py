#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Seoul National University (SNU)
# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

import argparse

from ns3gym import ns3env


def main(args):
    """!
    Main function to initialize and run the ns-3 gym environment for simulating network scenarios.

    @param args: Parsed arguments that contain simulation configurations like UE number, simulation time,
                 bandwidth, transmission power, and other related settings.
    @return None: This function doesn't return any value. It runs the simulation and handles the environment
                  setup, action sampling, and result logging.
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

    # Create the environment using ns3-gym integration
    env = ns3env.Ns3Env(port=args.port, simSeed=args.simSeed, simArgs=simArgs, debug=args.debug)

    step_idx = 0  # Initialize the step counter
    step_interval = args.stepInterval  # Define the interval for logging steps

    print("Start")

    try:
        # Reset the environment and get the initial observation
        obs = env.reset()
        print("Step: ", step_idx)
        print("---obs: ", obs)

        # Run the simulation steps
        while True:
            step_idx += 1

            # Sample a random action from the action space
            action = env.action_space.sample()
            # Take the action in the environment and receive the observation, reward, and status
            obs, reward, done, info = env.step(action)

            # Log information every 'step_interval' steps
            if step_idx % step_interval == 0:
                print("Step: ", step_idx)
                print("---action: ", action)
                print("---obs, reward, done, info: ", obs, reward, done, info)

            # End the episode if the 'done' flag is True
            if done:
                break

    except KeyboardInterrupt:
        # Handle interruption with Ctrl-C
        print("Ctrl-C -> Exit")
    finally:
        # Close the environment after finishing
        env.close()
        print("Done")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    # Arguments used for the gym script
    parser.add_argument("--port", type=int, default=5552, help="Port number")
    parser.add_argument("--simSeed", type=int, default=3002, help="Seed number")
    parser.add_argument("--debug", type=bool, default=False, help="Debug mode")
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
    parser.add_argument("--simTime", type=int, default=1, help="Total simulation time in seconds")
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
    # Step interval for logging in the gym environment
    parser.add_argument("--stepInterval", type=int, default=1000, help="Step interval for logging")

    args = parser.parse_args()

    main(args)
