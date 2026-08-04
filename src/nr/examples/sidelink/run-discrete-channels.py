#!/usr/bin/env python3
"""
Run the discrete-channels convergence experiment end-to-end.

Pipeline:
    1. Run the nr-sl-mcs-discrete-channels C++ program via ./ns3
    2. Run mcs-convergence.py to detect convergence per epoch
    3. Compute and print summary statistics
    4. Save a copy of this script and invocation args in the experiment directory

Usage (from ns-3 top-level directory):
    python contrib/nr/examples/sidelink/run-discrete-channels.py
    python contrib/nr/examples/sidelink/run-discrete-channels.py --rngRun 5 --numEpochs 30
"""

import argparse
import os
import re
import shutil
import subprocess
import sys

import numpy as np

# Study parameters: each entry drives both argparse and the C++ command line.
# "cpp_name" is the --flag passed to the C++ program (or an ns-3 attribute path).
# "default" of None means: optional; do not pass to C++ unless explicitly set.
STUDY_PARAMS = [
    {
        "name": "rngRun",
        "cpp_name": "RngRun",
        "type": int, "default": 1,
        "help": "RNG run number"
    },
    {
        "name": "numEpochs",
        "cpp_name": "numEpochs",
        "type": int,
        "default": 20,
        "help": "Number of channel epochs",
    },
    {
        "name": "discountFactor",
        "cpp_name": "ns3::NrSlThompsonSamplingMcsController::DiscountFactor",
        "type": float,
        "default": None,
        "help": "TS discount factor",
    },
    {
        "name": "mcsControllerType",
        "cpp_name": "mcsControllerType",
        "type": str,
        "default": None,
        "help": "ideal, ts, or olla",
    },
    {
        "name": "errorModel",
        "cpp_name": "errorModel",
        "type": str,
        "default": None,
        "help": "Error model type (static, epa)",
    },
    {
        "name": "cqiPeriod",
        "cpp_name": "ns3::NrSlOllaMcsController::CqiPeriod",
        "type": str,
        "default": None,
        "help": "OLLA CQI period random variable (e.g. ns3::ConstantRandomVariable[Constant=40])",
    },
    {
        "name": "pathlossMin",
        "cpp_name": "pathlossMin",
        "type": float,
        "default": None,
        "help": "Minimum path loss for uniform draw (dB)",
    },
    {
        "name": "pathlossMax",
        "cpp_name": "pathlossMax",
        "type": float,
        "default": None,
        "help": "Maximum path loss for uniform draw (dB)",
    },
    {
        "name": "mcsIndex",
        "cpp_name": "ns3::NrSlThompsonSamplingMcsController::McsIndex",
        "type": str,
        "default": None,
        "help": "The MCS index",
    },
]


def build_cpp_args(args):
    """Build the C++ command-line argument string from parsed args and STUDY_PARAMS."""
    cpp_args = []
    for param in STUDY_PARAMS:
        value = getattr(args, param["name"])
        if value is not None:
            cpp_args.append(f"--{param['cpp_name']}={value}")
    return " ".join(cpp_args)


def run_simulation(args):
    """Run the C++ program and return the experiment directory path."""
    cpp_args = build_cpp_args(args)
    cmd = ["./ns3", "run", f"nr-sl-mcs-discrete-channels {cpp_args}"]
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Simulation failed:")
        print(result.stderr)
        sys.exit(1)

    # Parse experiment directory from stdout
    experiment_dir = None
    for line in result.stdout.splitlines():
        match = re.search(r"Experiment directory: (.+)", line)
        if match:
            experiment_dir = match.group(1)
            break

    if experiment_dir is None:
        print("Could not find experiment directory in simulation output")
        print(result.stdout)
        sys.exit(1)

    # Save stderr (e.g. NS_LOG output) to experiment directory if non-empty
    if result.stderr:
        log_path = os.path.join(experiment_dir, "log.out")
        with open(log_path, "w") as f:
            f.write(result.stderr)
        print(f"Simulation log written to {log_path}")

    return experiment_dir


def run_convergence_analysis(experiment_dir):
    """Run mcs-convergence.py and return the path to the output file."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    convergence_script = os.path.join(script_dir, "mcs-convergence.py")
    trial_file = os.path.join(experiment_dir, "trial-results.dat")
    changes_file = os.path.join(experiment_dir, "channel-changes.dat")
    output_file = os.path.join(experiment_dir, "convergence-results.dat")

    cmd = [
        sys.executable,
        convergence_script,
        trial_file,
        "--change-times-file",
        changes_file,
        "--output-file",
        output_file,
    ]
    print(f"\nRunning convergence analysis...")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Convergence analysis failed:")
        print(result.stderr)
        sys.exit(1)

    # Print the human-readable output
    print(result.stdout)

    return output_file


def load_epoch_summary(experiment_dir):
    """Load epoch-summary.dat, returning a structured array."""
    filename = os.path.join(experiment_dir, "epoch-summary.dat")
    data = np.loadtxt(filename, skiprows=1)
    # Columns: epoch pathloss step_size tx rx loss_rate
    return {
        "epoch": data[:, 0].astype(int),
        "pathloss": data[:, 1],
        "step_size": data[:, 2],
        "tx": data[:, 3].astype(int),
        "rx": data[:, 4].astype(int),
        "loss_rate": data[:, 5],
    }


def load_convergence_results(filename):
    """Load convergence-results.dat, returning a list of dicts."""
    rows = []
    with open(filename) as f:
        header = f.readline()  # skip header
        for line in f:
            parts = line.split()
            rows.append(
                {
                    "epoch": int(parts[0]),
                    "converged": int(parts[4]),
                    "delay": float(parts[5]),
                }
            )
    return rows


def format_stats(values, unit="s"):
    """Format min/median/mean/std/max for an array of values."""
    if len(values) == 0:
        return "no data"
    return (
        f"min={np.min(values):.3f}{unit}  "
        f"median={np.median(values):.3f}{unit}  "
        f"mean={np.mean(values):.3f}{unit}  "
        f"std={np.std(values, ddof=1):.3f}{unit}  "
        f"max={np.max(values):.3f}{unit}"
    )


def format_pct_stats(values):
    """Format min/median/mean/std/max for percentage values."""
    if len(values) == 0:
        return "no data"
    return (
        f"min={100 * np.min(values):.2f}%  "
        f"median={100 * np.median(values):.2f}%  "
        f"mean={100 * np.mean(values):.2f}%  "
        f"std={100 * np.std(values, ddof=1):.2f}%  "
        f"max={100 * np.max(values):.2f}%"
    )


def compute_statistics(epoch_summary, convergence_results):
    """Compute and return statistics as a formatted string."""
    lines = []

    # Exclude epoch 0 from all statistics
    epoch_mask = epoch_summary["epoch"] > 0
    n_epochs = int(np.sum(epoch_mask))
    step_sizes = epoch_summary["step_size"][epoch_mask]
    loss_rates = epoch_summary["loss_rate"][epoch_mask]
    tx_counts = epoch_summary["tx"][epoch_mask]
    rx_counts = epoch_summary["rx"][epoch_mask]

    # Build convergence arrays excluding epoch 0
    conv_by_epoch = {r["epoch"]: r for r in convergence_results}
    delays = []
    converged_mask = np.zeros(n_epochs, dtype=bool)
    for idx, epoch_num in enumerate(epoch_summary["epoch"][epoch_mask]):
        r = conv_by_epoch.get(epoch_num)
        if r and r["converged"]:
            converged_mask[idx] = True
            delays.append(r["delay"])
    delays = np.array(delays)
    n_converged = len(delays)
    n_not_converged = n_epochs - n_converged

    # Direction masks (within the already epoch>0 subset)
    improved_mask = step_sizes < 0
    degraded_mask = step_sizes > 0

    # Convergence summary
    lines.append("=== Convergence ===")
    lines.append(f"  Epochs: {n_epochs} (excluding epoch 0)")
    lines.append(f"  Converged: {n_converged} ({100 * n_converged / n_epochs:.1f}%)")
    lines.append(f"  Not converged: {n_not_converged}")

    # Convergence delay
    lines.append(f"\n=== Convergence Delay ({n_converged} epochs) ===")
    if n_converged > 0:
        lines.append(f"  {format_stats(delays)}")

        # By direction: need to match converged epochs with their step_size
        improved_delays = []
        degraded_delays = []
        for idx, epoch_num in enumerate(epoch_summary["epoch"][epoch_mask]):
            r = conv_by_epoch.get(epoch_num)
            if r and r["converged"]:
                ss = step_sizes[idx]
                if ss < 0:
                    improved_delays.append(r["delay"])
                elif ss > 0:
                    degraded_delays.append(r["delay"])
        improved_delays = np.array(improved_delays)
        degraded_delays = np.array(degraded_delays)

        lines.append(
            f"\n  Channel improved ({len(improved_delays)} epochs): "
            f"{format_stats(improved_delays)}"
        )
        lines.append(
            f"  Channel degraded ({len(degraded_delays)} epochs): "
            f"{format_stats(degraded_delays)}"
        )

    # Packet loss
    lines.append("\n=== Packet Loss ===")
    total_tx = int(np.sum(tx_counts))
    total_rx = int(np.sum(rx_counts))
    overall_loss = 1 - total_rx / total_tx if total_tx > 0 else 0
    lines.append(f"  All epochs ({n_epochs} epochs):")
    lines.append(f"  Total TX: {total_tx}  RX: {total_rx}  Loss: {100 * overall_loss:.2f}%")
    lines.append(f"  Per-epoch: {format_pct_stats(loss_rates)}")

    improved_loss = loss_rates[improved_mask]
    degraded_loss = loss_rates[degraded_mask]
    n_improved = int(np.sum(improved_mask))
    n_degraded = int(np.sum(degraded_mask))
    lines.append(f"\n  Channel improved ({n_improved} epochs): {format_pct_stats(improved_loss)}")
    lines.append(f"  Channel degraded ({n_degraded} epochs): {format_pct_stats(degraded_loss)}")

    return "\n".join(lines)


def save_artifacts(experiment_dir, argv):
    """Copy this script and save invocation args to the experiment directory."""
    # Copy this script
    script_path = os.path.abspath(__file__)
    shutil.copy2(script_path, os.path.join(experiment_dir, "run-discrete-channels.py"))

    # Save invocation
    invocation = "python " + " ".join(argv)
    with open(os.path.join(experiment_dir, "invocation.txt"), "w") as f:
        f.write(invocation + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Run discrete-channels convergence experiment end-to-end."
    )
    for param in STUDY_PARAMS:
        parser.add_argument(
            f"--{param['name']}",
            type=param["type"],
            default=param["default"],
            help=f"{param['help']} (default: %(default)s)",
        )
    args = parser.parse_args()

    # Step 1: Run simulation
    experiment_dir = run_simulation(args)
    print(f"Experiment directory: {experiment_dir}")

    # Step 2: Run convergence analysis
    output_file = run_convergence_analysis(experiment_dir)

    # Step 3: Compute statistics
    epoch_summary = load_epoch_summary(experiment_dir)
    convergence_results = load_convergence_results(output_file)
    stats_text = compute_statistics(epoch_summary, convergence_results)

    # Print to stdout and save to file
    print("========== Summary statistics ===========\n")
    print(stats_text)
    output_path = os.path.join(experiment_dir, "output.txt")
    with open(output_path, "w") as f:
        f.write(stats_text + "\n")
    print(f"\nStatistics saved to {output_path}")

    # Step 4: Save artifacts
    save_artifacts(experiment_dir, sys.argv)


if __name__ == "__main__":
    main()
