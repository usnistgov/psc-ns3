#!/usr/bin/env python3
"""
Offline MCS convergence detector.

Given an MCS time series and known channel-change times, detects whether
the adaptive MCS controller converged, which MCS it converged to, and when,
on a per dstL2Id basis.

The implementation uses a custom CUSUM changepoint detector on a binary
"in convergence set" signal, with a concentration-ratio test to identify
the convergence set from the tail, and a post-alarm stability confirmation
window. It handles Thompson Sampling exploration noise, boundary oscillation
between adjacent MCS values, multiple destinations, and correctly reports no
convergence when the deadline is too early.

References for this technique:

    Montes De Oca V, Jeske DR, Zhang Q, Rendon C, Marvasti M. A CUSUM change-point
    detection algorithm for non-stationary sequences with application to data network
    surveillance. International Journal of Production Economics. 2010.
    doi:10.1016/j.ijpe.2010.04.010.

    Ahad N, Davenport MA, Xie Y. Data-Adaptive Symmetric CUSUM for Sequential Change
    Detection. Sequential Analysis. 2024;43(1):1-27. doi:10.1080/07474946.2023.2272908.

Principles of this technique:

1.  Use the CUSUM (a cumulative-sum test that sequentially flags statistically meaningful
    departures from a target epoch) algorithm to process the data after a change time
    and before either the next change time or a deadline.

2.  Handle sampling algorithms like Thompson Sampling by making allowances for exploration
    phases.

3.  Handle cases in which the channel does not pick a clear winner and the MCS converges
    but to two adjacent values instead of one.

4.  Require post-alarm stability confirmation to distinguish true convergence
    from transient behavior during exploration.

Tuning parameters:

    deadline: The end of the search window for each channel change.  Defaults
        to the next channel-change time, or the end of the data for the last
        segment.  Can be overridden with --deadline to impose a maximum time
        by which convergence must occur.

    tail_duration (default 10s): Duration of the window at the end of each
        segment used to identify the converged MCS value(s).  Must be shorter
        than the segment duration (otherwise it extends before the channel
        change), and long enough to contain a statistically meaningful number
        of samples in steady state.

    concentration_ratio (default 0.90): The minimum combined fraction of tail
        samples that the top one or two MCS values must account for.  This
        distinguishes a converged tail (one or two values dominate) from an
        exploration tail (many values with similar counts).  The threshold of
        0.85 allows up to 15% of tail samples to be exploration outliers
        (as seen with Thompson Sampling trial data), while also ensuring that
        boundary oscillation cases (e.g., MCS 22/23) include both values in
        the convergence set rather than just the mode.

    cusum_threshold (default 5.0): The CUSUM alarm threshold h.  Larger values
        require more cumulative evidence before declaring a changepoint,
        reducing false alarms but increasing detection delay.  For a binary
        signal with reference value k = 0.6, each in-set observation adds 0.4
        to the statistic, so a threshold of 5.0 requires roughly 13
        consecutive in-set observations to trigger (fewer if the pre-alarm
        signal already had partial credit).

    mu_0 (default 0.3) and mu_1 (default 0.9): The expected fraction of
        observations in the convergence set during the exploration phase
        (mu_0) and the converged phase (mu_1).  Their midpoint k = (mu_0 +
        mu_1) / 2 is the CUSUM reference value.  mu_0 should reflect how
        often the controller happens to land on the eventual MCS during early
        exploration; mu_1 should reflect the steady-state hit rate (less than
        1.0 because Thompson Sampling still explores occasionally).

    confirmation_duration (default 5s): After the CUSUM alarm, the detector
        requires sustained stability for this many seconds.  This filters out
        false alarms from transient lucky streaks during exploration.  Should
        be long enough to span several inter-sample intervals but short enough
        to not overshoot the segment boundary.

    confirmation_fraction (default 0.80): The minimum fraction of observations
        in the confirmation window that must belong to the convergence set.
        Set below 1.0 to tolerate the exploration noise present in Thompson
        Sampling trial data.

Usage:

    The datafile path is passed as the first argument and can be anywhere on the file
    system in relation to this program.

    For a sample mcs-changes.dat file with two channel step changes (times 33.333 and 66.667):
        python mcs-convergence.py mcs-changes.dat 33.333 66.667

    Repeating the same, but requiring a deadline of 60 seconds for convergence
        python mcs-convergence.py mcs-changes.dat 33.333 66.667 --deadline 60.0

    Reading change times from a channel-changes.dat file (columns: time pathloss):
        python mcs-convergence.py trial-results.dat --change-times-file channel-changes.dat

Sample output:

    Channel change at 33.333s:
      DstL2Id 2 converged to: {28}
      Convergence time: 41.960s (delay: 8.627s)

    Channel change at 66.667s:
      DstL2Id 2 converged to: {22, 23}
      Convergence time: 69.560s (delay: 2.893s)

    Channel change at 33.333s:
      DstL2Id 2 no convergence detected
"""

import argparse
import sys
from collections import Counter

import numpy as np

# Default tuning parameters (single source of truth for function signatures and argparse)
DEFAULT_TAIL_DURATION = 10.0
DEFAULT_CONCENTRATION_RATIO = 0.85
DEFAULT_CUSUM_THRESHOLD = 5.0
DEFAULT_MU_0 = 0.3
DEFAULT_MU_1 = 0.9
DEFAULT_CONFIRMATION_DURATION = 5.0
DEFAULT_CONFIRMATION_FRACTION = 0.80


def load_data(filename):
    """Load MCS trace file (columns: time, dstL2Id, mcs) with header."""
    data = np.loadtxt(filename, skiprows=1)
    times = data[:, 0]
    dst_ids = data[:, 1].astype(int)
    mcs_values = data[:, 2].astype(int)
    return times, dst_ids, mcs_values


def identify_convergence_set(
    times,
    mcs_values,
    deadline,
    tail_duration=DEFAULT_TAIL_DURATION,
    concentration_ratio=DEFAULT_CONCENTRATION_RATIO,
):
    """
    Identify the converged MCS and convergence set from the tail of the segment.

    The convergence set is the smallest set of MCS values (up to 2) whose
    combined frequency in the tail meets the concentration_ratio.  If even the
    top 2 values cannot reach the ratio, the tail is not converged.

    Returns:
        (converged_mcs, convergence_set) or (None, set()) if not converged.
    """
    tail_start = deadline - tail_duration
    tail_mask = times >= tail_start
    tail_mcs = mcs_values[tail_mask]

    if len(tail_mcs) == 0:
        return None, set()

    counts = Counter(tail_mcs)
    total = len(tail_mcs)
    ranked = counts.most_common()

    # Check if the top 1 value alone meets the concentration ratio
    top_mcs, top_count = ranked[0]
    if top_count / total >= concentration_ratio:
        return top_mcs, {top_mcs}

    # Check if the top 2 values meet the ratio, but only if they are adjacent
    if len(ranked) >= 2:
        second_mcs, second_count = ranked[1]
        if (
            abs(top_mcs - second_mcs) <= 2
            and (top_count + second_count) / total >= concentration_ratio
        ):
            return top_mcs, {top_mcs, second_mcs}

    # Top values do not account for enough of the tail, or are not adjacent
    return None, set()


def cusum_detect(
    binary_signal, times, mu_0=DEFAULT_MU_0, mu_1=DEFAULT_MU_1, threshold=DEFAULT_CUSUM_THRESHOLD
):
    """
    Run CUSUM on a binary signal to detect shift from low to high mean.

    Returns:
        (changepoint_index, alarm_index) or (None, None).
        changepoint_index is the last reset point before the alarm (the
        estimated start of the new epoch).  alarm_index is where S >= h.
    """
    k = (mu_0 + mu_1) / 2.0
    S = 0.0
    # Track the last index where S was reset to zero.  The alarm trigger
    # marks the estimated changepoint: the start of the run of evidence
    # that triggered the alarm.  The alarm index itself lags the true change
    # due to the evidence accumulation delay, but the last reset point is
    # where the new epoch most likely began.
    last_reset = 0
    for i, x in enumerate(binary_signal):
        S = S + (x - k)
        if S < 0:
            S = 0.0
            last_reset = i + 1
        if S >= threshold:
            return last_reset, i
    return None, None


def confirm_stability(
    binary_signal,
    times,
    alarm_index,
    confirmation_duration=DEFAULT_CONFIRMATION_DURATION,
    confirmation_fraction=DEFAULT_CONFIRMATION_FRACTION,
):
    """
    Verify that the convergence set dominates in a window after the alarm.

    Returns:
        True if the confirmation window passes, False otherwise.
    """
    alarm_time = times[alarm_index]
    confirm_end = alarm_time + confirmation_duration
    confirm_mask = (times >= alarm_time) & (times <= confirm_end)
    confirm_signal = binary_signal[confirm_mask]

    if len(confirm_signal) == 0:
        return False

    fraction = np.mean(confirm_signal)
    return fraction >= confirmation_fraction


def detect_convergence(
    times,
    mcs_values,
    change_time,
    deadline=None,
    tail_duration=DEFAULT_TAIL_DURATION,
    concentration_ratio=DEFAULT_CONCENTRATION_RATIO,
    cusum_threshold=DEFAULT_CUSUM_THRESHOLD,
    mu_0=DEFAULT_MU_0,
    mu_1=DEFAULT_MU_1,
    confirmation_duration=DEFAULT_CONFIRMATION_DURATION,
    confirmation_fraction=DEFAULT_CONFIRMATION_FRACTION,
):
    """
    Detect MCS convergence after a channel change.  Default values are provided in case
    this is called as a library function, but otherwise argparse will provide the
    default values.

    Args:
        times: Array of timestamps.
        mcs_values: Array of MCS values.
        change_time: Time of the channel change.
        deadline: End of the search window (default: end of data).
        tail_duration: Seconds of tail to use for identifying converged MCS.
        concentration_ratio: Required combined fraction of top 1-2 MCS in tail.
        cusum_threshold: CUSUM alarm threshold.
        mu_0: Expected pre-change mean of binary signal (exploration phase).
        mu_1: Expected post-change mean of binary signal (converged phase).
        confirmation_duration: Post-alarm stability window in seconds.
        confirmation_fraction: Required fraction in confirmation window.

    Returns:
        dict with keys: convergence_time, convergence_set,
        or None if no convergence detected.
    """
    if deadline is None:
        deadline = times[-1]

    # Extract segment between change_time and deadline
    segment_mask = (times >= change_time) & (times <= deadline)
    seg_times = times[segment_mask]
    seg_mcs = mcs_values[segment_mask]

    if len(seg_times) == 0:
        return None

    # Phase 1: Identify convergence set from tail
    _, convergence_set = identify_convergence_set(
        seg_times, seg_mcs, deadline, tail_duration, concentration_ratio
    )

    if not convergence_set:
        return None

    # Phase 2: Build binary signal and run CUSUM
    binary_signal = np.array([1.0 if m in convergence_set else 0.0 for m in seg_mcs])

    # Scan with CUSUM, retrying after failed confirmations
    start_index = 0
    while start_index < len(binary_signal):
        cp_index, alarm_index = cusum_detect(
            binary_signal[start_index:], seg_times[start_index:], mu_0, mu_1, cusum_threshold
        )

        if cp_index is None:
            return None  # No convergence detected

        # Adjust to absolute indices within segment
        cp_index += start_index
        alarm_index += start_index

        # Phase 3: Confirm stability from the changepoint onward
        if confirm_stability(
            binary_signal, seg_times, cp_index, confirmation_duration, confirmation_fraction
        ):
            return {
                "convergence_time": seg_times[cp_index],
                "convergence_set": {int(m) for m in convergence_set},
            }

        # Confirmation failed; resume scanning after this alarm point
        start_index = alarm_index + 1

    return None


def _write_output_file(filename, rows):
    """Write machine-readable convergence results to a space-aligned file."""
    if not rows:
        return
    # Column widths chosen to accommodate typical values
    header = f"{'epoch':>5} {'change_time':>11} {'pathloss':>8} {'dst':>3} {'converged':>9} {'delay':>8} mcs_set"
    with open(filename, "w") as f:
        f.write(header + "\n")
        for r in rows:
            pl_str = f"{r['pathloss']:.1f}" if not np.isnan(r["pathloss"]) else "-"
            delay_str = f"{r['delay']:.3f}" if r["converged"] else "-1"
            f.write(
                f"{r['epoch']:>5} {r['change_time']:>11.3f} {pl_str:>8} {r['dst']:>3}"
                f" {r['converged']:>9} {delay_str:>8} {r['mcs_set']}\n"
            )


def main():
    parser = argparse.ArgumentParser(description="Detect MCS convergence after channel changes.")
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
        "--deadline",
        type=float,
        default=None,
        help="Global deadline (default: next change time or " "end of data)",
    )
    parser.add_argument(
        "--tail-duration",
        type=float,
        default=DEFAULT_TAIL_DURATION,
        help="Tail window for identifying converged MCS (s)",
    )
    parser.add_argument(
        "--concentration-ratio",
        type=float,
        default=DEFAULT_CONCENTRATION_RATIO,
        help="Required combined fraction of top 1-2 MCS in " "tail to declare convergence",
    )
    parser.add_argument(
        "--cusum-threshold",
        type=float,
        default=DEFAULT_CUSUM_THRESHOLD,
        help="CUSUM alarm threshold",
    )
    parser.add_argument(
        "--confirmation-duration",
        type=float,
        default=DEFAULT_CONFIRMATION_DURATION,
        help="Post-alarm stability window (s)",
    )
    parser.add_argument(
        "--confirmation-fraction",
        type=float,
        default=DEFAULT_CONFIRMATION_FRACTION,
        help="Required fraction in confirmation window",
    )
    parser.add_argument(
        "--output-file",
        type=str,
        default=None,
        help="Write machine-readable convergence results to this file",
    )

    args = parser.parse_args()

    # Collect change times (and optional path losses) from positional args and/or file
    all_change_times = list(args.change_times) if args.change_times else []
    pathloss_map = {}  # change_time -> pathloss (populated only from file)
    if args.change_times_file is not None:
        file_data = np.loadtxt(args.change_times_file, skiprows=1)
        if file_data.ndim == 1:
            all_change_times.append(float(file_data[0]))
            if file_data.shape[0] >= 2:
                pathloss_map[float(file_data[0])] = file_data[1]
        else:
            all_change_times.extend(file_data[:, 0].tolist())
            if file_data.shape[1] >= 2:
                for row in file_data:
                    pathloss_map[float(row[0])] = row[1]
    if not all_change_times:
        parser.error("No change times provided; use positional args or --change-times-file")

    times, dst_ids, mcs_values = load_data(args.datafile)
    change_times = sorted(all_change_times)
    unique_dsts = sorted(set(dst_ids))

    results_rows = []

    for i, ct in enumerate(change_times):
        # Deadline is next change time or user-specified or end of data
        if args.deadline is not None:
            dl = min(
                args.deadline, change_times[i + 1] if i + 1 < len(change_times) else args.deadline
            )
        elif i + 1 < len(change_times):
            dl = change_times[i + 1]
        else:
            dl = None

        pl_str = f" (pathloss {pathloss_map[ct]:.1f} dB)" if ct in pathloss_map else ""
        print(f"Channel change at {ct:.3f}s{pl_str}:")
        for dst in unique_dsts:
            dst_mask = dst_ids == dst
            dst_times = times[dst_mask]
            dst_mcs = mcs_values[dst_mask]

            result = detect_convergence(
                dst_times,
                dst_mcs,
                ct,
                deadline=dl,
                tail_duration=args.tail_duration,
                concentration_ratio=args.concentration_ratio,
                cusum_threshold=args.cusum_threshold,
                confirmation_duration=args.confirmation_duration,
                confirmation_fraction=args.confirmation_fraction,
            )

            if result is None:
                print(f"  DstL2Id {dst} no convergence detected")
                results_rows.append(
                    {
                        "epoch": i,
                        "change_time": ct,
                        "pathloss": pathloss_map.get(ct, float("nan")),
                        "dst": dst,
                        "converged": 0,
                        "delay": -1.0,
                        "mcs_set": "-",
                    }
                )
            else:
                delay = result["convergence_time"] - ct
                mcs_str = ", ".join(str(m) for m in sorted(result["convergence_set"]))
                print(f"  DstL2Id {dst} converged to: {{{mcs_str}}}")
                print(
                    f"  Convergence time: {result['convergence_time']:.3f}s"
                    f" (delay: {delay:.3f}s)"
                )
                results_rows.append(
                    {
                        "epoch": i,
                        "change_time": ct,
                        "pathloss": pathloss_map.get(ct, float("nan")),
                        "dst": dst,
                        "converged": 1,
                        "delay": delay,
                        "mcs_set": ",".join(str(m) for m in sorted(result["convergence_set"])),
                    }
                )
        print()

    if args.output_file is not None:
        _write_output_file(args.output_file, results_rows)


if __name__ == "__main__":
    main()
