#!/usr/bin/env python3
"""
Run the random-walk MCS tracking experiment end-to-end.

Pipeline:
    1. Run the nr-sl-mcs-random-walk C++ program via ./ns3
    2. Compute and print summary statistics from distance-trace.dat
    3. Save a copy of this script and invocation args in the experiment directory

Usage (from ns-3 top-level directory):
    python contrib/nr/examples/sidelink/run-random-walk.py
    python contrib/nr/examples/sidelink/run-random-walk.py --rngRun 5 --simDuration 300
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
    {"name": "rngRun", "cpp_name": "RngRun", "type": int, "default": 1, "help": "RNG run number"},
    {
        "name": "measurementDuration",
        "cpp_name": "measurementDuration",
        "type": float,
        "default": 600.0,
        "help": "Duration of the measurement phase in seconds",
    },
    {
        "name": "minDistance",
        "cpp_name": "minDistance",
        "type": float,
        "default": None,
        "help": "Minimum inter-UE distance in meters (closest box edges)",
    },
    {
        "name": "maxDistance",
        "cpp_name": "maxDistance",
        "type": float,
        "default": None,
        "help": "Maximum inter-UE distance in meters (opposite diagonal corners)",
    },
    {
        "name": "minSpeed",
        "cpp_name": "minSpeed",
        "type": float,
        "default": None,
        "help": "Minimum UE speed in m/s",
    },
    {
        "name": "maxSpeed",
        "cpp_name": "maxSpeed",
        "type": float,
        "default": None,
        "help": "Maximum UE speed in m/s",
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
        "name": "lossModelType",
        "cpp_name": "lossModelType",
        "type": str,
        "default": None,
        "help": "Propagation loss model: friis, log-distance, umi",
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
    cmd = ["./ns3", "run", f"nr-sl-mcs-random-walk {cpp_args}"]
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Simulation failed:")
        print(result.stderr)
        sys.exit(1)

    # Print the simulation output
    print(result.stdout)

    # Parse experiment directory from stdout
    experiment_dir = None
    for line in result.stdout.splitlines():
        match = re.search(r"Experiment directory: (.+)", line)
        if match:
            experiment_dir = match.group(1)
            break

    if experiment_dir is None:
        print("Could not find experiment directory in simulation output")
        sys.exit(1)

    # Save stderr (e.g. NS_LOG output) to experiment directory if non-empty
    if result.stderr:
        log_path = os.path.join(experiment_dir, "log.out")
        with open(log_path, "w") as f:
            f.write(result.stderr)
        print(f"Simulation log written to {log_path}")

    return experiment_dir


def load_distance_trace(experiment_dir):
    """Load distance-trace.dat, returning (times, distances) arrays."""
    filename = os.path.join(experiment_dir, "distance-trace.dat")
    data = np.loadtxt(filename, skiprows=1)
    if data.ndim == 1:
        data = data.reshape(1, -1)
    return data[:, 0], data[:, 1]


def format_stats(values, unit=""):
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


def compute_statistics(experiment_dir):
    """Compute and return statistics as a formatted string."""
    lines = []

    # Distance statistics from the full trace
    try:
        trace_times, trace_distances = load_distance_trace(experiment_dir)
        lines.append("=== Distance ===")
        lines.append(f"  Samples: {len(trace_distances)}")
        lines.append(f"  {format_stats(trace_distances, 'm')}")
    except Exception as e:
        lines.append(f"=== Distance === (could not load: {e})")

    return "\n".join(lines)


def save_artifacts(experiment_dir, argv):
    """Copy this script and save invocation args to the experiment directory."""
    # Copy this script
    script_path = os.path.abspath(__file__)
    shutil.copy2(script_path, os.path.join(experiment_dir, "run-random-walk.py"))

    # Save invocation
    invocation = "python " + " ".join(argv)
    with open(os.path.join(experiment_dir, "invocation.txt"), "w") as f:
        f.write(invocation + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Run random-walk MCS tracking experiment end-to-end."
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

    # Step 2: Compute statistics
    stats_text = compute_statistics(experiment_dir)

    # Print to stdout and save to file
    print("\n========== Summary statistics ===========\n")
    print(stats_text)
    output_path = os.path.join(experiment_dir, "output.txt")
    with open(output_path, "w") as f:
        f.write(stats_text + "\n")
    print(f"\nStatistics saved to {output_path}")

    # Step 3: Save artifacts
    save_artifacts(experiment_dir, sys.argv)


if __name__ == "__main__":
    main()
