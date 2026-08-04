#!/usr/bin/env python3
#
# SPDX-License-Identifier: NIST-Software
#
"""
Run nr-sl-olla-mcs-controller-example for all four combinations of
channel and OLLA error models, and produce a 2x2 subplot figure.

This program is designed to run from the main ns-3 directory.

It runs 12 simulations (4 error model combinations x 3 PenaltyStep values)
and generates a combined figure saved as olla-example-sinr-tracking-combined.png.
"""

import subprocess
import sys

import matplotlib.pyplot as plt

PENALTY_STEPS = [0.5, 1.0, 3.0]
ERROR_MODELS = ["static", "epa"]


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


def plot_subplot(ax, all_data, channel_model, olla_model):
    """Plot one error model combination on the given axes."""
    colors = ["tab:blue", "tab:orange", "tab:red"]

    first_step = next(iter(all_data))
    times_gt, sinr_gt, _, _ = all_data[first_step]
    t0 = times_gt[0]

    ax.plot(to_slots(times_gt, t0), sinr_gt, "k--", linewidth=1.5, label="Ground truth")

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

    ax.set_xlim(0, 1000)
    ax.set_xlabel("Slot", fontsize=8)
    ax.set_ylabel("SINR (dB)", fontsize=8)
    ax.set_title(
        f"Channel = {channel_model}; CQI estimation = {olla_model}",
        fontsize=9,
        fontweight="bold",
    )
    ax.legend(loc="best", fontsize=7)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis="both", which="major", labelsize=7)


def main():
    # Run all 12 simulations
    for channel_model in ERROR_MODELS:
        for olla_model in ERROR_MODELS:
            for step in PENALTY_STEPS:
                if not run_simulation(step, channel_model, olla_model):
                    print("Aborting due to simulation failure")
                    sys.exit(1)

    # Create 2x2 subplot figure
    fig, axes = plt.subplots(2, 2, figsize=(12, 8))

    combos = [
        (0, 0, "static", "static"),
        (0, 1, "static", "epa"),
        (1, 0, "epa", "static"),
        (1, 1, "epa", "epa"),
    ]

    for row, col, channel_model, olla_model in combos:
        combo_data = {}
        for step in PENALTY_STEPS:
            filename = make_filename(step, channel_model, olla_model)
            times, sinr, filtered_sinr, sinr_est = read_data_file(filename)
            if times is not None:
                combo_data[step] = (times, sinr, filtered_sinr, sinr_est)

        if combo_data:
            plot_subplot(axes[row][col], combo_data, channel_model, olla_model)
        else:
            axes[row][col].text(
                0.5,
                0.5,
                "No data",
                ha="center",
                va="center",
                transform=axes[row][col].transAxes,
            )

    plt.suptitle(
        "OLLA SINR Tracking with Step Change in Channel Conditions",
        fontsize=12,
        fontweight="bold",
    )
    plt.tight_layout()

    output_filename = "olla-example-sinr-tracking-combined.png"
    plt.savefig(output_filename, dpi=200, bbox_inches="tight")
    print(f"\nPlot saved as: {output_filename}")
    plt.close()


if __name__ == "__main__":
    main()
