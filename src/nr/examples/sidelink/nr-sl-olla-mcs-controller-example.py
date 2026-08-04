#!/usr/bin/env python3
#
# SPDX-License-Identifier: NIST-Software
#
"""
Run nr-sl-olla-mcs-controller-example with different NACK step sizes
and plot results of OLLA SINR tracking.

This program is designed to run from the main ns-3 directory.

It runs three simulations with PenaltyStep values of 0.5, 1.0, and 3.0,
then plots the ground truth SINR and the three OLLA SINR estimates
on a single figure.  It produces a figure similar to Fig. 1 of the
SALAD paper.

The error model of the channel can be set to epa or static, using
the --channel argument.  The error model of the OLLA model (that it
uses to select MCS from SINR estimate) can be set to epa or static
as well.  This allows the configuration of a mismatch of models
that OLLA must correct for.

Usage:
    python3 contrib/nr/examples/sidelink/nr-sl-olla-mcs-controller-example.py
    python3 contrib/nr/examples/sidelink/nr-sl-olla-mcs-controller-example.py --channel epa --olla static
"""

import argparse
import subprocess
import sys

import matplotlib.pyplot as plt

PENALTY_STEPS = [0.5, 1.0, 3.0]


def make_filename(penalty_step, channel_model, olla_model):
    """Build output filename for a given parameter combination."""
    return f"olla-example-ch-{channel_model}-olla-{olla_model}-step-{penalty_step}.dat"


def run_simulation(penalty_step, channel_model, olla_model):
    """Run the example with given parameters."""
    output_file = make_filename(penalty_step, channel_model, olla_model)
    print(f"Running: PenaltyStep={penalty_step}, channel={channel_model}, olla={olla_model}")

    cmd = [
        "./ns3",
        "run",
        "nr-sl-olla-mcs-controller-example",
        "--",
        f"--ns3::NrSlOllaMcsController::PenaltyStep={penalty_step}",
        f"--errorModel={channel_model}",
        f"--ollaErrorModel={olla_model}",
        f"--outputFile={output_file}",
    ]

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"  {result.stdout.strip().splitlines()[-1]}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"  ERROR: Simulation failed with return code {e.returncode}")
        print(f"  stderr: {e.stderr}")
        return False


def read_data_file(filename):
    """Read time, SINR (unfiltered), filtered SINR, and SINR estimate from output file."""
    times = []
    sinr_values = []
    filtered_sinr_values = []
    sinr_est_values = []

    try:
        with open(filename, "r") as f:
            for line in f:
                if line.startswith("#"):
                    continue
                parts = line.strip().split()
                if len(parts) >= 4:
                    times.append(float(parts[0]))
                    sinr_values.append(float(parts[1]))
                    filtered_sinr_values.append(float(parts[2]))
                    sinr_est_values.append(float(parts[3]))
        return times, sinr_values, filtered_sinr_values, sinr_est_values
    except FileNotFoundError:
        print(f"ERROR: File not found: {filename}")
        return None, None, None, None
    except Exception as e:
        print(f"ERROR reading {filename}: {e}")
        return None, None, None, None


def to_slots(times, t0):
    """Convert absolute times to slot indices relative to t0."""
    return [(t - t0) * 1000 for t in times]


def main():
    parser = argparse.ArgumentParser(description="OLLA SINR tracking plot")
    parser.add_argument(
        "--channel",
        default="static",
        choices=["static", "epa"],
        help="Channel error model (default: static)",
    )
    parser.add_argument(
        "--olla",
        default="static",
        choices=["static", "epa"],
        help="OLLA controller error model (default: static)",
    )
    args = parser.parse_args()

    channel_model = args.channel
    olla_model = args.olla

    # Run simulations
    for step in PENALTY_STEPS:
        if not run_simulation(step, channel_model, olla_model):
            print(f"Aborting due to simulation failure for PenaltyStep={step}")
            sys.exit(1)

    # Read data
    all_data = {}
    for step in PENALTY_STEPS:
        filename = make_filename(step, channel_model, olla_model)
        times, sinr, filtered_sinr, sinr_est = read_data_file(filename)
        if times is not None:
            all_data[step] = (times, sinr, filtered_sinr, sinr_est)
            print(f"  Read {len(times)} data points from {filename}")

    if not all_data:
        print("ERROR: No data files could be read")
        sys.exit(1)

    # Plot
    fig, ax = plt.subplots(figsize=(8, 4))

    first_step = next(iter(all_data))
    times_gt, sinr_gt, _, _ = all_data[first_step]
    t0 = times_gt[0]

    ax.plot(to_slots(times_gt, t0), sinr_gt, "k--", linewidth=1.5, label="Ground truth")

    colors = ["tab:blue", "tab:orange", "tab:red"]
    for i, step in enumerate(PENALTY_STEPS):
        if step in all_data:
            times, _, _, sinr_est = all_data[step]
            ax.plot(
                to_slots(times, t0),
                sinr_est,
                color=colors[i],
                linewidth=0.8,
                alpha=0.8,
                label=f"OLLA ($\\Delta_{{NACK}}$ = {step})",
            )

    ax.set_xlim(0, 2000)
    ax.set_xlabel("Slot")
    ax.set_ylabel("SINR (dB)")
    ax.set_title(f"OLLA SINR Tracking (channel = {channel_model}; CQI estimation = {olla_model})")
    ax.legend(loc="best", fontsize=8)
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    output_filename = f"olla-example-sinr-tracking-ch-{channel_model}-olla-{olla_model}.png"
    plt.savefig(output_filename, dpi=200, bbox_inches="tight")
    print(f"\nPlot saved as: {output_filename}")
    plt.close()


if __name__ == "__main__":
    main()
