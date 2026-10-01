#!/usr/bin/env python3
#
# SPDX-License-Identifier: NIST-Software
#
"""
Run nr-sl-ideal-mcs-controller-sweep with different margin values and target BLER values,
and plot MCS vs SNR results.

This program is designed to run from the main ns-3 directory.

This program runs 16 simulations and generates 5 png files from the resulting ns-3 output files,
all with the prefix: "ideal-mcs-controller-". The 5 png files are named:
- ideal-mcs-controller-sweep-epa-margin.png
- ideal-mcs-controller-sweep-epa-target-bler.png
- ideal-mcs-controller-sweep-static-margin.png
- ideal-mcs-controller-sweep-static-target-bler.png
- ideal-mcs-controller-sweep-combined.png (combines all four plots as subplots)
"""

import os
import subprocess
import sys

import matplotlib.pyplot as plt

# Values to sweep
MARGIN_VALUES = [1, 2, 3, 4]
TARGET_BLER_VALUES = [0.1, 0.01, 0.001, 0.0001]
ERROR_MODELS = ["static", "epa"]

DEFAULT_TARGET_BLER = 0.1
DEFAULT_MARGIN = 0


def run_simulation(margin, target_bler, error_model):
    """Run program with specified margin value, target BLER, and error model."""
    print(
        f"Running simulation with Margin={margin} dB, TargetBler={target_bler}, Error Model={error_model}"
    )

    cmd = [
        "./ns3",
        "run",
        "nr-sl-ideal-mcs-controller-sweep",
        "--",
        f"--ns3::NrSlIdealMcsController::Margin={margin}",
        f"--ns3::NrSlIdealMcsController::TargetBler={target_bler}",
        f"--errorModel={error_model}",
    ]

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        output_file = (
            f"ideal-mcs-controller-{error_model}-{float(margin):.1f}-{target_bler}.dat"
        )

        if os.path.exists(output_file):
            print(f"  Generated: {output_file}")
            return output_file
        else:
            print(f"  ERROR: Expected output file not found: {output_file}")
            return None
    except subprocess.CalledProcessError as e:
        print(f"  ERROR: Simulation failed with return code {e.returncode}")
        print(f"  stderr: {e.stderr}")
        return None


def read_data_file(filename):
    """Read SNR, distance, and MCS data from output file."""
    snr_values = []
    distance_values = []
    mcs_values = []

    try:
        with open(filename, "r") as f:
            for line in f:
                values = line.strip().split()
                if len(values) >= 3:
                    snr = float(values[0])
                    distance = float(values[1])
                    mcs = int(values[2])
                    snr_values.append(snr)
                    distance_values.append(distance)
                    mcs_values.append(mcs)
        return snr_values, distance_values, mcs_values
    except FileNotFoundError:
        print(f"ERROR: File not found: {filename}")
        return None, None, None
    except Exception as e:
        print(f"ERROR reading {filename}: {e}")
        return None, None, None


def plot_margin_results(data_dict, error_model):
    """Create plot of MCS vs SNR for all margin values."""
    fig, ax1 = plt.subplots(figsize=(4, 3))

    # Plot each margin value on primary axis (using distance as x-axis)
    for margin in MARGIN_VALUES:
        if margin in data_dict:
            snr, distance, mcs = data_dict[margin]
            ax1.plot(
                distance, mcs, marker="o", markersize=2, label=f"Margin={margin} dB", linewidth=1
            )

    # Configure primary axis
    ax1.set_ylabel("MCS Index", fontsize=8)
    ax1.set_xlim(100, 1500)  # Set distance range
    ax1.tick_params(axis="both", which="major", labelsize=7)

    # Set title based on error model
    if error_model == "static":
        title = "SNR vs. MCS for different adaptive MCS margins (static error model)"
    elif error_model == "epa":
        title = "SNR vs. MCS for different adaptive MCS margins (EPA error model)"
    else:
        title = f"SNR vs. MCS for different adaptive MCS margins ({error_model} error model)"

    ax1.set_title(title, fontsize=7, fontweight="bold")
    ax1.grid(True, alpha=0.3)
    ax1.legend(loc="best", fontsize=7)
    ax1.set_ylim(-1, 29)  # MCS range is 0-28

    # Create custom x-axis labels showing both distance and SNR
    if MARGIN_VALUES[0] in data_dict:
        snr_ref, distance_ref, _ = data_dict[MARGIN_VALUES[0]]

        # Set specific tick positions at 100, 300, 500, ..., 1500
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]

        # Create labels showing both distance and corresponding SNR
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    # Find closest distance value and get corresponding SNR
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")

        # Explicitly set ticks before setting labels to avoid warning
        ax1.set_xticks(distance_ticks)
        ax1.set_xticklabels(combined_labels, fontsize=7)
        ax1.set_xlabel("Distance (m) / SNR (dB)", fontsize=8)
    else:
        ax1.set_xlabel("Distance (m)", fontsize=8)

    # Save plot
    output_filename = f"ideal-mcs-controller-sweep-{error_model}-margin.png"
    plt.savefig(output_filename, dpi=200, bbox_inches="tight")
    print(f"Plot saved as: {output_filename}")
    plt.close()


def plot_target_bler_results(data_dict, error_model):
    """Create plot of MCS vs SNR for different target BLER values."""
    fig, ax1 = plt.subplots(figsize=(4, 3))

    # Plot each target BLER value on primary axis (using distance as x-axis)
    for target_bler in TARGET_BLER_VALUES:
        if target_bler in data_dict:
            snr, distance, mcs = data_dict[target_bler]
            ax1.plot(
                distance,
                mcs,
                marker="o",
                markersize=2,
                label=f"TargetBler={target_bler}",
                linewidth=1,
            )

    # Configure primary axis
    ax1.set_ylabel("MCS Index", fontsize=8)
    ax1.set_xlim(100, 1500)  # Set distance range
    ax1.tick_params(axis="both", which="major", labelsize=7)

    # Set title based on error model
    if error_model == "static":
        title = "SNR vs. MCS for different target BLER values (static error model)"
    elif error_model == "epa":
        title = "SNR vs. MCS for different target BLER values (EPA error model)"
    else:
        title = f"SNR vs. MCS for different target BLER values ({error_model} error model)"

    ax1.set_title(title, fontsize=7, fontweight="bold")
    ax1.grid(True, alpha=0.3)
    ax1.legend(loc="best", fontsize=7)
    ax1.set_ylim(-1, 29)  # MCS range is 0-28

    # Create custom x-axis labels showing both distance and SNR
    if TARGET_BLER_VALUES[0] in data_dict:
        snr_ref, distance_ref, _ = data_dict[TARGET_BLER_VALUES[0]]

        # Set specific tick positions at 100, 300, 500, ..., 1500
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]

        # Create labels showing both distance and corresponding SNR
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    # Find closest distance value and get corresponding SNR
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")

        # Explicitly set ticks before setting labels to avoid warning
        ax1.set_xticks(distance_ticks)
        ax1.set_xticklabels(combined_labels)
        ax1.set_xlabel("Distance (m) / SNR (dB)", fontsize=12)
    else:
        ax1.set_xlabel("Distance (m)", fontsize=12)

    # Save plot
    output_filename = f"ideal-mcs-controller-sweep-{error_model}-target-bler.png"
    plt.savefig(output_filename, dpi=200, bbox_inches="tight")
    print(f"Plot saved as: {output_filename}")
    plt.close()


def plot_combined_subplots(static_margin_data, static_bler_data, epa_margin_data, epa_bler_data):
    """Create a combined figure with four subplots showing all results."""
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(10, 7))

    # Upper left: Static + Margin
    for margin in MARGIN_VALUES:
        if margin in static_margin_data:
            snr, distance, mcs = static_margin_data[margin]
            ax1.plot(
                distance, mcs, marker="o", markersize=2, label=f"Margin={margin} dB", linewidth=1
            )

    ax1.set_ylabel("MCS Index", fontsize=8)
    ax1.set_xlim(100, 1500)
    ax1.tick_params(axis="both", which="major", labelsize=7)
    ax1.set_title(
        "SNR vs. MCS for different adaptive MCS margins (static error model)",
        fontsize=7,
        fontweight="bold",
    )
    ax1.grid(True, alpha=0.3)
    ax1.legend(loc="best", fontsize=6)
    ax1.set_ylim(-1, 29)

    # Set x-axis labels for upper left
    if MARGIN_VALUES[0] in static_margin_data:
        snr_ref, distance_ref, _ = static_margin_data[MARGIN_VALUES[0]]
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")
        ax1.set_xticks(distance_ticks)
        ax1.set_xticklabels(combined_labels, fontsize=6)
        ax1.set_xlabel("Distance (m) / SNR (dB)", fontsize=8)
    else:
        ax1.set_xlabel("Distance (m)", fontsize=8)

    # Upper right: EPA + Margin
    for margin in MARGIN_VALUES:
        if margin in epa_margin_data:
            snr, distance, mcs = epa_margin_data[margin]
            ax2.plot(
                distance, mcs, marker="o", markersize=2, label=f"Margin={margin} dB", linewidth=1
            )

    ax2.set_ylabel("MCS Index", fontsize=8)
    ax2.set_xlim(100, 1500)
    ax2.tick_params(axis="both", which="major", labelsize=7)
    ax2.set_title(
        "SNR vs. MCS for different adaptive MCS margins (EPA error model)",
        fontsize=7,
        fontweight="bold",
    )
    ax2.grid(True, alpha=0.3)
    ax2.legend(loc="best", fontsize=6)
    ax2.set_ylim(-1, 29)

    # Set x-axis labels for upper right
    if MARGIN_VALUES[0] in epa_margin_data:
        snr_ref, distance_ref, _ = epa_margin_data[MARGIN_VALUES[0]]
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")
        ax2.set_xticks(distance_ticks)
        ax2.set_xticklabels(combined_labels, fontsize=6)
        ax2.set_xlabel("Distance (m) / SNR (dB)", fontsize=8)
    else:
        ax2.set_xlabel("Distance (m)", fontsize=8)

    # Lower left: Static + Target BLER
    for target_bler in TARGET_BLER_VALUES:
        if target_bler in static_bler_data:
            snr, distance, mcs = static_bler_data[target_bler]
            ax3.plot(
                distance,
                mcs,
                marker="o",
                markersize=2,
                label=f"TargetBler={target_bler}",
                linewidth=1,
            )

    ax3.set_ylabel("MCS Index", fontsize=8)
    ax3.set_xlim(100, 1500)
    ax3.tick_params(axis="both", which="major", labelsize=7)
    ax3.set_title(
        "SNR vs. MCS for different target BLER values (static error model)",
        fontsize=7,
        fontweight="bold",
    )
    ax3.grid(True, alpha=0.3)
    ax3.legend(loc="best", fontsize=6)
    ax3.set_ylim(-1, 29)

    # Set x-axis labels for lower left
    if TARGET_BLER_VALUES[0] in static_bler_data:
        snr_ref, distance_ref, _ = static_bler_data[TARGET_BLER_VALUES[0]]
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")
        ax3.set_xticks(distance_ticks)
        ax3.set_xticklabels(combined_labels, fontsize=6)
        ax3.set_xlabel("Distance (m) / SNR (dB)", fontsize=8)
    else:
        ax3.set_xlabel("Distance (m)", fontsize=8)

    # Lower right: EPA + Target BLER
    for target_bler in TARGET_BLER_VALUES:
        if target_bler in epa_bler_data:
            snr, distance, mcs = epa_bler_data[target_bler]
            ax4.plot(
                distance,
                mcs,
                marker="o",
                markersize=2,
                label=f"TargetBler={target_bler}",
                linewidth=1,
            )

    ax4.set_ylabel("MCS Index", fontsize=8)
    ax4.set_xlim(100, 1500)
    ax4.tick_params(axis="both", which="major", labelsize=7)
    ax4.set_title(
        "SNR vs. MCS for different target BLER values (EPA error model)",
        fontsize=7,
        fontweight="bold",
    )
    ax4.grid(True, alpha=0.3)
    ax4.legend(loc="best", fontsize=6)
    ax4.set_ylim(-1, 29)

    # Set x-axis labels for lower right
    if TARGET_BLER_VALUES[0] in epa_bler_data:
        snr_ref, distance_ref, _ = epa_bler_data[TARGET_BLER_VALUES[0]]
        distance_ticks = [100, 300, 500, 700, 900, 1100, 1300, 1500]
        combined_labels = []
        for distance_tick in distance_ticks:
            if len(distance_ref) > 0:
                try:
                    idx = min(
                        range(len(distance_ref)), key=lambda i: abs(distance_ref[i] - distance_tick)
                    )
                    snr = snr_ref[idx]
                    combined_labels.append(f"{int(distance_tick)}m\n{snr:.1f}")
                except:
                    combined_labels.append(f"{int(distance_tick)}m")
            else:
                combined_labels.append(f"{int(distance_tick)}m")
        ax4.set_xticks(distance_ticks)
        ax4.set_xticklabels(combined_labels, fontsize=6)
        ax4.set_xlabel("Distance (m) / SNR (dB)", fontsize=8)
    else:
        ax4.set_xlabel("Distance (m)", fontsize=8)

    # Adjust spacing between subplots
    plt.tight_layout()

    # Save combined plot
    output_filename = "ideal-mcs-controller-sweep-combined.png"
    plt.savefig(output_filename, dpi=200, bbox_inches="tight")
    print(f"\nCombined plot saved as: {output_filename}")
    plt.close()


def main():
    # Store data for combined plot
    all_margin_data = {}
    all_bler_data = {}

    # Process each error model
    for error_model in ERROR_MODELS:
        print(f"Running with error model: {error_model}")

        print(f"\n[1/4] Sweeping margin and distance for {error_model}...")
        margin_output_files = {}
        for margin in MARGIN_VALUES:
            output_file = run_simulation(margin, DEFAULT_TARGET_BLER, error_model)
            if output_file:
                margin_output_files[margin] = output_file

        if not margin_output_files:
            print(f"\nWARNING: No simulations completed successfully for {error_model}!")
        else:
            print(
                f"\nCompleted {len(margin_output_files)}/{len(MARGIN_VALUES)} simulations successfully."
            )

            # Read data files for margin sweep
            print(f"\n[2/4] Reading data and generating plot for {error_model}...")
            margin_data_dict = {}
            for margin, filename in margin_output_files.items():
                snr, distance, mcs = read_data_file(filename)
                if snr is not None and distance is not None and mcs is not None:
                    margin_data_dict[margin] = (snr, distance, mcs)
                    print(f"  Read {len(snr)} data points from {filename}")

            if not margin_data_dict:
                print(f"\nWARNING: No margin sweep data files could be read for {error_model}!")
            else:
                # Generate margin plot
                plot_margin_results(margin_data_dict, error_model)
                # Store data for combined plot
                all_margin_data[error_model] = margin_data_dict

        # ==== PART 2: Target BLER sweep with default margin ====
        print(f"\n[3/4] Sweeping target BLER and distance for {error_model}...")
        bler_output_files = {}
        for target_bler in TARGET_BLER_VALUES:
            output_file = run_simulation(DEFAULT_MARGIN, target_bler, error_model)
            if output_file:
                bler_output_files[target_bler] = output_file

        if not bler_output_files:
            print(f"\nWARNING: No simulations completed successfully for {error_model}!")
        else:
            print(
                f"\nCompleted {len(bler_output_files)}/{len(TARGET_BLER_VALUES)} simulations successfully."
            )

            # Read data files for target BLER sweep
            print(f"\n[4/4] Reading data and generating plot for {error_model}...")
            bler_data_dict = {}
            for target_bler, filename in bler_output_files.items():
                snr, distance, mcs = read_data_file(filename)
                if snr is not None and distance is not None and mcs is not None:
                    bler_data_dict[target_bler] = (snr, distance, mcs)
                    print(f"  Read {len(snr)} data points from {filename}")

            if not bler_data_dict:
                print(
                    f"\nWARNING: No target BLER sweep data files could be read for {error_model}!"
                )
            else:
                # Generate target BLER plot
                plot_target_bler_results(bler_data_dict, error_model)
                # Store data for combined plot
                all_bler_data[error_model] = bler_data_dict

    # Generate combined subplot figure if we have all necessary data
    print("\n[5/5] Generating combined subplot figure...")
    if (
        "static" in all_margin_data
        and "epa" in all_margin_data
        and "static" in all_bler_data
        and "epa" in all_bler_data
    ):
        plot_combined_subplots(
            all_margin_data["static"],
            all_bler_data["static"],
            all_margin_data["epa"],
            all_bler_data["epa"],
        )
    else:
        print("WARNING: Not all data available for combined plot!")


if __name__ == "__main__":
    main()
