#!/usr/bin/env python3
"""
Generate a multi-panel visualization of CUSUM-based MCS convergence detection.

Produces a 3-panel PNG figure illustrating the convergence detection technique:
    Panel 1 (top):    Raw MCS time series with convergence markers
    Panel 2 (middle): Binary "in convergence set" signal
    Panel 3 (bottom): CUSUM statistic S(t) with threshold, alarm, and
                       changepoint markers

Usage (for trace that has channel changes at 33.333 and 66.667):
    python mcs-convergence-plot.py mcs-changes.dat 0 33.333 66.667
    python mcs-convergence-plot.py mcs-changes.dat 0 33.333 66.667 -o my-plot.png
    python mcs-convergence-plot.py trial-results.dat --change-times-file channel-changes.dat
"""

import argparse
import sys

import matplotlib.pyplot as plt
import numpy as np

# Import shared functions from the convergence detector
sys.path.insert(0, ".")
from importlib import import_module

mcs_conv = import_module("mcs-convergence")
load_data = mcs_conv.load_data
identify_convergence_set = mcs_conv.identify_convergence_set
confirm_stability = mcs_conv.confirm_stability


def cusum_trace(binary_signal, mu_0=0.3, mu_1=0.9, threshold=5.0):
    """
    Compute the full CUSUM trace over a binary signal.

    Unlike cusum_detect in mcs-convergence.py, this does not short-circuit
    at the alarm.  It returns the full S array for plotting, along with
    the changepoint and alarm indices.

    Returns:
        (S_array, changepoint_index, alarm_index)
        S_array has one entry per observation.
        changepoint_index and alarm_index are None if no alarm fired.
    """
    k = (mu_0 + mu_1) / 2.0
    n = len(binary_signal)
    S_array = np.zeros(n)
    S = 0.0
    last_reset = 0
    cp_index = None
    alarm_index = None

    for i, x in enumerate(binary_signal):
        S = S + (x - k)
        if S < 0:
            S = 0.0
            last_reset = i + 1
        S_array[i] = S
        # Record first alarm only
        if alarm_index is None and S >= threshold:
            cp_index = last_reset
            alarm_index = i

    return S_array, cp_index, alarm_index


def analyze_segment(
    times,
    mcs_values,
    change_time,
    deadline,
    tail_duration,
    concentration_ratio,
    cusum_threshold,
    mu_0,
    mu_1,
    confirmation_duration,
    confirmation_fraction,
):
    """
    Run convergence analysis on a single segment and return all intermediate
    data needed for plotting.

    Returns a dict with keys: seg_times, seg_mcs, binary_signal, S_array,
    convergence_set, cp_index, alarm_index, cp_time, confirmed, confirm_end,
    or None if the segment has no data or no convergence set.
    """
    segment_mask = (times >= change_time) & (times <= deadline)
    seg_times = times[segment_mask]
    seg_mcs = mcs_values[segment_mask]

    if len(seg_times) == 0:
        return None

    _, convergence_set = identify_convergence_set(
        seg_times, seg_mcs, deadline, tail_duration, concentration_ratio
    )

    if not convergence_set:
        return None

    binary_signal = np.array([1.0 if m in convergence_set else 0.0 for m in seg_mcs])

    # Compute full S trace for plotting (does not short-circuit)
    S_array, _, _ = cusum_trace(binary_signal, mu_0, mu_1, cusum_threshold)

    # Find the first alarm that passes confirmation, retrying after failures
    # (mirrors the retry loop in detect_convergence)
    confirmed = False
    confirm_end = None
    cp_time = None
    cp_index = None
    alarm_index = None
    start_index = 0
    while start_index < len(binary_signal):
        sub_S, sub_cp, sub_alarm = cusum_trace(
            binary_signal[start_index:], mu_0, mu_1, cusum_threshold
        )
        if sub_cp is None:
            break
        # Adjust to absolute indices
        abs_cp = sub_cp + start_index
        abs_alarm = sub_alarm + start_index
        abs_cp_time = seg_times[abs_cp]
        abs_confirm_end = abs_cp_time + confirmation_duration
        if confirm_stability(
            binary_signal, seg_times, abs_cp, confirmation_duration, confirmation_fraction
        ):
            cp_index = abs_cp
            alarm_index = abs_alarm
            cp_time = abs_cp_time
            confirm_end = abs_confirm_end
            confirmed = True
            break
        # Confirmation failed; resume after this alarm
        start_index = abs_alarm + 1

    return {
        "seg_times": seg_times,
        "seg_mcs": seg_mcs,
        "binary_signal": binary_signal,
        "S_array": S_array,
        "convergence_set": convergence_set,
        "cp_index": cp_index,
        "alarm_index": alarm_index,
        "cp_time": cp_time,
        "confirmed": confirmed,
        "confirm_end": confirm_end,
    }


def main():
    parser = argparse.ArgumentParser(description="Plot CUSUM convergence detection visualization.")
    parser.add_argument("datafile", help="MCS trace file")
    parser.add_argument(
        "change_times", type=float, nargs="*", help="Channel change times (seconds)"
    )
    parser.add_argument(
        "--change-times-file",
        type=str,
        default=None,
        help="File with channel change schedule (columns: time pathloss); "
        "change times are read from the first column",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="mcs-convergence.png",
        help="Output PNG file (default: mcs-convergence.png)",
    )
    parser.add_argument("--deadline", type=float, default=None)
    parser.add_argument("--tail-duration", type=float, default=10.0)
    parser.add_argument("--concentration-ratio", type=float, default=0.90)
    parser.add_argument("--cusum-threshold", type=float, default=5.0)
    parser.add_argument("--mu-0", type=float, default=0.3)
    parser.add_argument("--mu-1", type=float, default=0.9)
    parser.add_argument("--confirmation-duration", type=float, default=5.0)
    parser.add_argument("--confirmation-fraction", type=float, default=0.85)
    parser.add_argument("--no-legend", action="store_true", help="Disable legends on all subplots")

    args = parser.parse_args()

    times, dst_ids, mcs_values = load_data(args.datafile)

    # Collect change times from positional args and/or file
    all_change_times = list(args.change_times) if args.change_times else []
    if args.change_times_file is not None:
        file_data = np.loadtxt(args.change_times_file, skiprows=1)
        if file_data.ndim == 1:
            all_change_times.append(float(file_data[0]))
        else:
            all_change_times.extend(file_data[:, 0].tolist())
    if not all_change_times:
        parser.error("No change times provided; use positional args or --change-times-file")
    change_times = sorted(all_change_times)

    # For plotting, use the first dstL2Id found
    unique_dsts = sorted(set(dst_ids))
    dst = unique_dsts[0]
    dst_mask = dst_ids == dst
    dst_times = times[dst_mask]
    dst_mcs = mcs_values[dst_mask]

    # Analyze each segment
    segments = []
    for i, ct in enumerate(change_times):
        if args.deadline is not None:
            dl = min(
                args.deadline, change_times[i + 1] if i + 1 < len(change_times) else args.deadline
            )
        elif i + 1 < len(change_times):
            dl = change_times[i + 1]
        else:
            dl = dst_times[-1]

        result = analyze_segment(
            dst_times,
            dst_mcs,
            ct,
            dl,
            args.tail_duration,
            args.concentration_ratio,
            args.cusum_threshold,
            args.mu_0,
            args.mu_1,
            args.confirmation_duration,
            args.confirmation_fraction,
        )
        segments.append((ct, dl, result))

    # Colors for each segment
    colors = ["tab:blue", "tab:orange"]

    fig, (ax_mcs, ax_bin, ax_cusum) = plt.subplots(
        3, 1, sharex=True, figsize=(12, 8), gridspec_kw={"height_ratios": [2, 1, 2]}
    )

    # Panel 1: Raw MCS time series
    ax_mcs.plot(
        dst_times, dst_mcs, ".", color="gray", markersize=3, alpha=0.5, label="MCS observations"
    )

    for idx, (ct, dl, seg) in enumerate(segments):
        color = colors[idx % len(colors)]
        ax_mcs.axvline(
            ct, color=color, linestyle="--", linewidth=1, label=f"Channel change ({ct:.1f}s)"
        )
        if seg is not None and seg["confirmed"] and seg["cp_time"] is not None:
            mcs_str = ", ".join(str(int(m)) for m in sorted(seg["convergence_set"]))
            ax_mcs.axvline(
                seg["cp_time"],
                color=color,
                linestyle="-",
                linewidth=1.5,
                label=f"Converged to {{{mcs_str}}} " f"({seg['cp_time']:.1f}s)",
            )

    ax_mcs.set_ylabel("MCS Index")
    ax_mcs.set_ylim(-1, 30)
    if not args.no_legend:
        ax_mcs.legend(loc="upper right", fontsize=8)
    ax_mcs.set_title(f"CUSUM Convergence Detection (DstL2Id {dst})")

    # Panel 2: Binary signal
    for idx, (ct, dl, seg) in enumerate(segments):
        if seg is None:
            continue
        color = colors[idx % len(colors)]
        ax_bin.stem(
            seg["seg_times"],
            seg["binary_signal"],
            linefmt=color,
            markerfmt=f".",
            basefmt=" ",
            label=f"Segment {idx + 1}",
        )

    ax_bin.set_ylabel("In Set")
    ax_bin.set_ylim(-0.1, 1.3)
    ax_bin.set_yticks([0, 1])
    ax_bin.set_yticklabels(["Out", "In"])
    if not args.no_legend:
        ax_bin.legend(loc="upper right", fontsize=8)

    # Panel 3: CUSUM statistic
    ax_cusum.axhline(
        args.cusum_threshold,
        color="red",
        linestyle=":",
        linewidth=1,
        label=f"Threshold h={args.cusum_threshold}",
    )

    for idx, (ct, dl, seg) in enumerate(segments):
        if seg is None:
            continue
        color = colors[idx % len(colors)]
        ax_cusum.plot(
            seg["seg_times"],
            seg["S_array"],
            color=color,
            linewidth=1,
            label=f"S(t) segment {idx + 1}",
        )

        if seg["alarm_index"] is not None:
            alarm_time = seg["seg_times"][seg["alarm_index"]]
            alarm_S = seg["S_array"][seg["alarm_index"]]
            ax_cusum.plot(
                alarm_time,
                alarm_S,
                "v",
                color=color,
                markersize=10,
                label=f"Alarm ({alarm_time:.1f}s)",
            )

        if seg["cp_index"] is not None:
            cp_S = seg["S_array"][seg["cp_index"]]
            ax_cusum.plot(
                seg["cp_time"],
                cp_S,
                "^",
                color=color,
                markersize=10,
                label=f"Changepoint ({seg['cp_time']:.1f}s)",
            )

        # Shade confirmation window
        if seg["confirmed"] and seg["cp_time"] is not None:
            ax_cusum.axvspan(
                seg["cp_time"],
                seg["confirm_end"],
                alpha=0.15,
                color=color,
                label=f"Confirmation window",
            )

    ax_cusum.set_ylabel("CUSUM Statistic S")
    ax_cusum.set_xlabel("Time (s)")
    if not args.no_legend:
        ax_cusum.legend(loc="upper right", fontsize=8)

    plt.tight_layout()
    plt.savefig(args.output, dpi=150)
    print(f"Saved plot to {args.output}")


if __name__ == "__main__":
    main()
