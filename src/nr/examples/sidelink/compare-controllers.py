#!/usr/bin/env python3
"""
Compare MCS controller tracking by running ideal, TS, and OLLA controllers
on the same channel sequence, then plotting all on the same axes.

Supports two modes:
  - discrete: Uses run-discrete-channels.py with piecewise-constant channel
    epochs.  Ideal controller is plotted as a step function, TS and OLLA as
    1-second mode scatter.  Secondary axis shows path loss.
  - random-walk: Uses run-random-walk.py with pedestrian mobility.  All
    controllers are plotted as 1-second mode scatter.  Secondary axis shows
    inter-node distance.

Pipeline:
    1. Run the appropriate script for each controller with the same RNG seed
    2. Load trial-results.dat and channel/distance data from all experiments
    3. Downsample and plot MCS vs time with secondary axis
    4. Save comparison plot and artifacts

Usage (from ns-3 top-level directory):
    python contrib/nr/examples/sidelink/compare-controllers.py
    python contrib/nr/examples/sidelink/compare-controllers.py --mode random-walk
    python contrib/nr/examples/sidelink/compare-controllers.py --rngRun 5 --numEpochs 10
    python contrib/nr/examples/sidelink/compare-controllers.py --mode random-walk --measurementDuration 300
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
from collections import Counter
from datetime import datetime

import matplotlib.pyplot as plt
import numpy as np

# Fraction of trials in a 1-second bin that the top MCS must account for
# to be plotted.  A threshold of 0.40 allows bins where two adjacent MCS
# values share nearly all the trials.
MODE_CONCENTRATION_THRESHOLD = 0.40

# Parameters forwarded to the underlying script (excluding mcsControllerType).
# Discrete-only and random-walk-only parameters use the "modes" key.
COMPARE_PARAMS = [
    {"name": "rngRun", "type": int, "default": 1, "help": "RNG run number"},
    {
        "name": "numEpochs",
        "type": int,
        "default": 20,
        "help": "Number of channel epochs (discrete mode)",
        "modes": ["discrete"],
    },
    {
        "name": "discountFactor",
        "type": float,
        "default": None,
        "help": "TS discount factor",
    },
    {
        "name": "measurementDuration",
        "type": float,
        "default": None,
        "help": "Measurement duration in seconds (random-walk mode)",
        "modes": ["random-walk"],
    },
    {
        "name": "minDistance",
        "type": float,
        "default": None,
        "help": "Minimum inter-UE distance in meters (random-walk mode)",
        "modes": ["random-walk"],
    },
    {
        "name": "maxDistance",
        "type": float,
        "default": None,
        "help": "Maximum inter-UE distance in meters (random-walk mode)",
        "modes": ["random-walk"],
    },
    {
        "name": "minSpeed",
        "type": float,
        "default": None,
        "help": "Minimum UE speed in m/s (random-walk mode)",
        "modes": ["random-walk"],
    },
    {
        "name": "maxSpeed",
        "type": float,
        "default": None,
        "help": "Maximum UE speed in m/s (random-walk mode)",
        "modes": ["random-walk"],
    },
    {
        "name": "errorModel",
        "type": str,
        "default": None,
        "help": "Error model type (static, epa)",
    },
    {
        "name": "cqiPeriod",
        "type": str,
        "default": None,
        "help": "OLLA CQI period random variable (e.g. ns3::ConstantRandomVariable[Constant=40])",
    },
    {
        "name": "pathlossMin",
        "type": float,
        "default": None,
        "help": "Minimum path loss in dB (discrete mode)",
        "modes": ["discrete"],
    },
    {
        "name": "pathlossMax",
        "type": float,
        "default": None,
        "help": "Maximum path loss in dB (discrete mode)",
        "modes": ["discrete"],
    },
]


def run_single(mode, controller_type, args):
    """Run the appropriate script for one controller type, return experiment dir."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if mode == "discrete":
        run_script = os.path.join(script_dir, "run-discrete-channels.py")
    else:
        run_script = os.path.join(script_dir, "run-random-walk.py")

    cmd = [
        sys.executable,
        run_script,
        f"--mcsControllerType={controller_type}",
        f"--rngRun={args.rngRun}",
    ]

    # Add mode-appropriate parameters
    for param in COMPARE_PARAMS:
        if param["name"] == "rngRun":
            continue  # already added
        modes = param.get("modes")
        if modes and mode not in modes:
            continue
        value = getattr(args, param["name"], None)
        if value is not None:
            cmd.append(f"--{param['name']}={value}")

    print(f"\n{'='*60}")
    print(f"Running {controller_type} controller ({mode} mode)...")
    print(f"{'='*60}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"{controller_type} run failed:")
        print(result.stderr)
        sys.exit(1)

    # Print the output
    print(result.stdout)

    # Parse experiment directory from output
    for line in result.stdout.splitlines():
        match = re.search(r"Experiment directory: (.+)", line)
        if match:
            return match.group(1)

    print(f"Could not find experiment directory in {controller_type} output")
    sys.exit(1)


def load_trial_results(experiment_dir):
    """Load trial-results.dat, returning (times, mcs_values, success) arrays."""
    filename = os.path.join(experiment_dir, "trial-results.dat")
    data = np.loadtxt(filename, skiprows=1)
    if data.ndim == 1:
        data = data.reshape(1, -1)
    times = data[:, 0]
    mcs_values = data[:, 2].astype(int)
    success = data[:, 3].astype(int)
    return times, mcs_values, success


def load_channel_changes(experiment_dir):
    """Load channel-changes.dat, returning (times, pathloss) arrays."""
    filename = os.path.join(experiment_dir, "channel-changes.dat")
    data = np.loadtxt(filename, skiprows=1)
    if data.ndim == 1:
        data = data.reshape(1, -1)
    return data[:, 0], data[:, 1]


def load_distance_trace(experiment_dir):
    """Load distance-trace.dat, returning (times, distances) arrays."""
    filename = os.path.join(experiment_dir, "distance-trace.dat")
    data = np.loadtxt(filename, skiprows=1)
    if data.ndim == 1:
        data = data.reshape(1, -1)
    return data[:, 0], data[:, 1]


def downsample_ideal(times, mcs_values, change_times, sim_end):
    """Downsample ideal controller to step function: one point at epoch start, one at epoch end."""
    plot_times = []
    plot_mcs = []

    for i, t_change in enumerate(change_times):
        t_end = change_times[i + 1] if i + 1 < len(change_times) else sim_end
        # Find the dominant MCS in this epoch
        mask = (times >= t_change) & (times < t_end)
        epoch_mcs = mcs_values[mask]
        if len(epoch_mcs) == 0:
            continue
        counts = Counter(epoch_mcs)
        dominant_mcs = counts.most_common(1)[0][0]
        # Two points: start and end of epoch
        plot_times.append(t_change)
        plot_mcs.append(dominant_mcs)
        plot_times.append(t_end)
        plot_mcs.append(dominant_mcs)

    return np.array(plot_times), np.array(plot_mcs)


def compute_packet_loss(times, success, sim_start, sim_end, window=10.0):
    """Compute rolling-average packet loss ratio from trial results.

    For each second, the loss ratio is averaged over a centered window of
    *window* seconds (default 10).  This smooths out the quantization
    artifacts that arise from having only ~13-14 trials per second.
    """
    # First, build per-second loss ratios
    bin_times = []
    bin_loss = []

    bin_start = np.floor(sim_start)
    while bin_start < sim_end:
        bin_end = bin_start + 1.0
        mask = (times > bin_start) & (times <= bin_end)
        bin_success = success[mask]
        if len(bin_success) > 0:
            bin_times.append(bin_start + 0.5)
            bin_loss.append(1.0 - np.mean(bin_success))
        bin_start += 1.0

    bin_times = np.array(bin_times)
    bin_loss = np.array(bin_loss)

    if len(bin_loss) == 0:
        return bin_times, bin_loss

    # Apply a centered rolling mean
    half_win = window / 2.0
    smooth_loss = np.empty_like(bin_loss)
    for i, t in enumerate(bin_times):
        win_mask = (bin_times >= t - half_win) & (bin_times <= t + half_win)
        smooth_loss[i] = np.mean(bin_loss[win_mask])

    return bin_times, smooth_loss


def downsample_mode(times, mcs_values, sim_start, sim_end, threshold):
    """Downsample to 1-second bins, plotting mode if above threshold."""
    plot_times = []
    plot_mcs = []

    bin_start = np.floor(sim_start)
    while bin_start < sim_end:
        bin_end = bin_start + 1.0
        mask = (times > bin_start) & (times <= bin_end)
        bin_mcs = mcs_values[mask]
        if len(bin_mcs) > 0:
            counts = Counter(bin_mcs)
            top_mcs, top_count = counts.most_common(1)[0]
            if top_count / len(bin_mcs) >= threshold:
                plot_times.append(bin_start + 0.5)
                plot_mcs.append(top_mcs)
        bin_start += 1.0

    return np.array(plot_times), np.array(plot_mcs)


def create_plot_discrete(ideal_dir, ts_dir, compare_dir, threshold, olla_dir=None):
    """Create comparison plot for discrete mode with packet loss subplot."""
    ideal_times, ideal_mcs, ideal_success = load_trial_results(ideal_dir)
    ts_times, ts_mcs, ts_success = load_trial_results(ts_dir)
    change_times, pathloss_values = load_channel_changes(ideal_dir)

    sim_start = change_times[0]
    sim_end = max(ideal_times[-1], ts_times[-1])

    if olla_dir:
        olla_times, olla_mcs, olla_success = load_trial_results(olla_dir)
        sim_end = max(sim_end, olla_times[-1])

    # Skip epoch 0 (initial transient) by starting from the second epoch
    plot_start = change_times[1] if len(change_times) > 1 else sim_start

    # Downsample (ideal uses full range for step function; others start after
    # the transient)
    ideal_plot_t, ideal_plot_mcs = downsample_ideal(ideal_times, ideal_mcs, change_times, sim_end)
    ts_plot_t, ts_plot_mcs = downsample_mode(ts_times, ts_mcs, plot_start, sim_end, threshold)

    # Compute per-second packet loss ratios (rolling average)
    ideal_loss_t, ideal_loss = compute_packet_loss(ideal_times, ideal_success, plot_start, sim_end)
    ts_loss_t, ts_loss = compute_packet_loss(ts_times, ts_success, plot_start, sim_end)

    if olla_dir:
        olla_plot_t, olla_plot_mcs = downsample_mode(
            olla_times, olla_mcs, plot_start, sim_end, threshold
        )
        olla_loss_t, olla_loss = compute_packet_loss(olla_times, olla_success, plot_start, sim_end)

    # Create figure with two subplots sharing the x-axis
    fig, (ax_mcs, ax_loss) = plt.subplots(2, 1, figsize=(14, 8), sharex=True, height_ratios=[3, 2])

    ax_mcs.set_xlim(plot_start, sim_end)

    # Plot channel change vertical lines (skip epoch 0)
    for t in change_times:
        if t >= plot_start:
            ax_mcs.axvline(x=t, color="gray", linestyle="--", linewidth=0.5, alpha=0.6)
            ax_loss.axvline(x=t, color="gray", linestyle="--", linewidth=0.5, alpha=0.6)

    # Plot ideal as a step function (line)
    ax_mcs.plot(
        ideal_plot_t,
        ideal_plot_mcs,
        color="blue",
        linewidth=1.5,
        label="Ideal",
        zorder=3,
    )

    # Plot TS as scatter points
    ax_mcs.scatter(
        ts_plot_t,
        ts_plot_mcs,
        color="red",
        s=8,
        alpha=0.7,
        label="Thompson Sampling",
        zorder=4,
    )

    # Plot OLLA as scatter points
    if olla_dir:
        ax_mcs.scatter(
            olla_plot_t,
            olla_plot_mcs,
            color="orange",
            s=8,
            alpha=0.7,
            label="OLLA",
            zorder=5,
        )

    ax_mcs.set_ylabel("MCS")
    ax_mcs.set_ylim(-1, 29)
    ax_mcs.legend(loc="upper right")
    title = "MCS controller comparison (discrete pathloss changes)"
    ax_mcs.set_title(title)

    # Secondary y-axis for path loss
    ax_pl = ax_mcs.twinx()
    for i, t in enumerate(change_times):
        t_end = change_times[i + 1] if i + 1 < len(change_times) else sim_end
        ax_pl.plot(
            [t, t_end],
            [pathloss_values[i], pathloss_values[i]],
            color="green",
            linewidth=1.0,
            alpha=0.4,
        )
    ax_pl.set_ylabel("Path Loss (dB)", color="green")
    ax_pl.tick_params(axis="y", labelcolor="green")
    # Invert path loss axis so lower path loss (better channel) is at the top
    ax_pl.invert_yaxis()

    # -- Bottom subplot: Packet loss ratio --

    ax_loss.plot(
        ideal_loss_t,
        ideal_loss,
        color="blue",
        linewidth=1.0,
        alpha=0.8,
        label="Ideal",
        zorder=3,
    )
    ax_loss.plot(
        ts_loss_t,
        ts_loss,
        color="red",
        linewidth=1.0,
        alpha=0.8,
        label="Thompson Sampling",
        zorder=4,
    )
    if olla_dir:
        ax_loss.plot(
            olla_loss_t,
            olla_loss,
            color="orange",
            linewidth=1.0,
            alpha=0.8,
            label="OLLA",
            zorder=5,
        )

    ax_loss.set_xlabel("Time (s)")
    ax_loss.set_ylabel("Packet Loss Ratio")
    ax_loss.set_ylim(-0.05, 1.05)
    ax_loss.legend(loc="upper right")
    ax_loss.axhline(y=0.1, color="gray", linestyle="--", linewidth=0.5, alpha=0.6)

    plt.tight_layout()
    plot_path = os.path.join(compare_dir, "comparison.png")
    fig.savefig(plot_path, dpi=150)
    plt.close(fig)
    print(f"Plot saved to {plot_path}")
    return plot_path


def create_plot_random_walk(ideal_dir, ts_dir, compare_dir, threshold, olla_dir=None):
    """Create comparison plot for random-walk mode with packet loss subplot."""
    ideal_times, ideal_mcs, ideal_success = load_trial_results(ideal_dir)
    ts_times, ts_mcs, ts_success = load_trial_results(ts_dir)
    dist_times, dist_values = load_distance_trace(ideal_dir)

    sim_start = min(ideal_times[0], ts_times[0])
    sim_end = max(ideal_times[-1], ts_times[-1])

    if olla_dir:
        olla_times, olla_mcs, olla_success = load_trial_results(olla_dir)
        sim_start = min(sim_start, olla_times[0])
        sim_end = max(sim_end, olla_times[-1])

    # Skip initial transient (warmup + controller convergence)
    plot_start = sim_start + 40.0

    # Downsample all controllers the same way (1-second mode),
    # starting after the transient
    ideal_plot_t, ideal_plot_mcs = downsample_mode(
        ideal_times, ideal_mcs, plot_start, sim_end, threshold
    )
    ts_plot_t, ts_plot_mcs = downsample_mode(ts_times, ts_mcs, plot_start, sim_end, threshold)

    # Compute per-second packet loss ratios
    ideal_loss_t, ideal_loss = compute_packet_loss(ideal_times, ideal_success, plot_start, sim_end)
    ts_loss_t, ts_loss = compute_packet_loss(ts_times, ts_success, plot_start, sim_end)

    if olla_dir:
        olla_plot_t, olla_plot_mcs = downsample_mode(
            olla_times, olla_mcs, plot_start, sim_end, threshold
        )
        olla_loss_t, olla_loss = compute_packet_loss(olla_times, olla_success, plot_start, sim_end)

    # Create figure with two subplots sharing the x-axis
    fig, (ax_mcs, ax_loss) = plt.subplots(2, 1, figsize=(14, 8), sharex=True, height_ratios=[3, 2])

    ax_mcs.set_xlim(plot_start, sim_end)

    # -- Top subplot: MCS comparison --

    # Plot ideal as scatter points
    ax_mcs.scatter(
        ideal_plot_t,
        ideal_plot_mcs,
        color="blue",
        s=8,
        alpha=0.7,
        label="Ideal",
        zorder=3,
    )

    # Plot TS as scatter points
    ax_mcs.scatter(
        ts_plot_t,
        ts_plot_mcs,
        color="red",
        s=8,
        alpha=0.7,
        label="Thompson Sampling",
        zorder=4,
    )

    # Plot OLLA as scatter points
    if olla_dir:
        ax_mcs.scatter(
            olla_plot_t,
            olla_plot_mcs,
            color="orange",
            s=8,
            alpha=0.7,
            label="OLLA",
            zorder=5,
        )

    ax_mcs.set_ylabel("MCS")
    ax_mcs.set_ylim(-1, 29)
    ax_mcs.legend(loc="upper right")
    title = "MCS controller comparison (random walk mobility)"
    ax_mcs.set_title(title)

    # Secondary y-axis for distance on MCS subplot
    ax_dist = ax_mcs.twinx()
    ax_dist.plot(
        dist_times,
        dist_values,
        color="green",
        linewidth=1.0,
        alpha=0.4,
    )
    ax_dist.set_ylabel("Distance (m)", color="green")
    ax_dist.tick_params(axis="y", labelcolor="green")
    # Invert distance axis so shorter distance (better channel) is at the top
    ax_dist.invert_yaxis()

    # -- Bottom subplot: Packet loss ratio --

    ax_loss.plot(
        ideal_loss_t,
        ideal_loss,
        color="blue",
        linewidth=1.0,
        alpha=0.8,
        label="Ideal",
        zorder=3,
    )
    ax_loss.plot(
        ts_loss_t,
        ts_loss,
        color="red",
        linewidth=1.0,
        alpha=0.8,
        label="Thompson Sampling",
        zorder=4,
    )
    if olla_dir:
        ax_loss.plot(
            olla_loss_t,
            olla_loss,
            color="orange",
            linewidth=1.0,
            alpha=0.8,
            label="OLLA",
            zorder=5,
        )

    ax_loss.set_xlabel("Time (s)")
    ax_loss.set_ylabel("Packet Loss Ratio")
    ax_loss.set_ylim(-0.05, 1.05)
    ax_loss.legend(loc="upper right")
    ax_loss.axhline(y=0.1, color="gray", linestyle="--", linewidth=0.5, alpha=0.6)

    plt.tight_layout()
    plot_path = os.path.join(compare_dir, "comparison.png")
    fig.savefig(plot_path, dpi=150)
    plt.close(fig)
    print(f"Plot saved to {plot_path}")
    return plot_path


def replot_from_directory(compare_dir, threshold):
    """Regenerate comparison plot from an existing compare directory."""
    manifest_path = os.path.join(compare_dir, "experiments.txt")
    manifest = {}
    with open(manifest_path) as f:
        for line in f:
            key, _, value = line.strip().partition(": ")
            manifest[key] = value

    mode = manifest["mode"]
    ideal_dir = manifest["ideal"]
    ts_dir = manifest["ts"]
    olla_dir = manifest.get("olla")

    print(f"Replotting from {compare_dir}")
    print(f"  Mode: {mode}")
    print(f"  Ideal: {ideal_dir}")
    print(f"  TS: {ts_dir}")
    if olla_dir:
        print(f"  OLLA: {olla_dir}")

    if mode == "discrete":
        create_plot_discrete(ideal_dir, ts_dir, compare_dir, threshold, olla_dir)
    else:
        create_plot_random_walk(ideal_dir, ts_dir, compare_dir, threshold, olla_dir)


def main():
    parser = argparse.ArgumentParser(
        description="Compare MCS controllers on the same channel sequence."
    )
    parser.add_argument(
        "--mode",
        choices=["discrete", "random-walk"],
        default="discrete",
        help="Experiment mode (default: discrete)",
    )
    parser.add_argument(
        "--replot",
        type=str,
        default=None,
        metavar="DIR",
        help="Regenerate the plot from an existing compare directory (no simulation run)",
    )
    for param in COMPARE_PARAMS:
        parser.add_argument(
            f"--{param['name']}",
            type=param["type"],
            default=param["default"],
            help=f"{param['help']} (default: %(default)s)",
        )
    args = parser.parse_args()

    # Replot mode: regenerate from existing experiment data
    if args.replot:
        replot_from_directory(args.replot, MODE_CONCENTRATION_THRESHOLD)
        return

    # Create compare directory
    timestamp = datetime.now().strftime("%Y-%m-%dT%H-%M-%S")
    error_model_tag = f"-{args.errorModel}" if args.errorModel else ""
    compare_dir = f"experiments/compare-{args.mode}{error_model_tag}-{args.rngRun}-{timestamp}"
    os.makedirs(compare_dir, exist_ok=True)
    print(f"Compare directory: {compare_dir}")

    # Run all three controllers
    ideal_dir = run_single(args.mode, "ideal", args)
    ts_dir = run_single(args.mode, "ts", args)
    olla_dir = run_single(args.mode, "olla", args)

    # Record which experiments were used
    manifest_path = os.path.join(compare_dir, "experiments.txt")
    with open(manifest_path, "w") as f:
        f.write(f"mode: {args.mode}\n")
        f.write(f"ideal: {ideal_dir}\n")
        f.write(f"ts: {ts_dir}\n")
        f.write(f"olla: {olla_dir}\n")

    # Create comparison plot
    if args.mode == "discrete":
        create_plot_discrete(ideal_dir, ts_dir, compare_dir, MODE_CONCENTRATION_THRESHOLD, olla_dir)
    else:
        create_plot_random_walk(
            ideal_dir, ts_dir, compare_dir, MODE_CONCENTRATION_THRESHOLD, olla_dir
        )

    # Save artifacts
    script_path = os.path.abspath(__file__)
    shutil.copy2(script_path, os.path.join(compare_dir, "compare-controllers.py"))
    invocation = "python " + " ".join(sys.argv)
    with open(os.path.join(compare_dir, "invocation.txt"), "w") as f:
        f.write(invocation + "\n")

    print(f"\nAll results in {compare_dir}")


if __name__ == "__main__":
    main()
