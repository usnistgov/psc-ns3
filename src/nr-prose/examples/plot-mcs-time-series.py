#!/usr/bin/env python3
"""Run the four (mcsController x idealSchedLevel) configurations of
nr-prose-adaptive-mcs across K replications and plot, per config,
median MCS over time with an inter-quartile band (top panel) and
sorted-rank MoS bars across the MCPTT pairs (bottom panel).

Topology: ring layout with mcpttPairs MCPTT pairs + bgPairs background-
traffic pairs, all pairs at the same distance d.  The program staggers
per-pair start times so app i (1-indexed) starts at startTrafficTime +
(i - 1) * (simTime / nAppPairs).  Pair start times appear as gray
dotted vertical lines in the top panel; the contention ramp the time
series shows comes from those staggered starts.

Configurations plotted (all four on each subplot):
  1. Ideal,  idealSchedLevel=Global -- oracle controller + oracle scheduler
  2. Ideal,  idealSchedLevel=None   -- oracle controller, contended channel
  3. OLLA,   idealSchedLevel=Global -- adaptive controller, no contention
  4. OLLA,   idealSchedLevel=None   -- adaptive controller, contended channel

Methodology:
  Each (controller, schedLevel, seed) run is replicated K times
  (--rngRuns).

  Top panel: every per-(seed, pair, decision) MCS sample from MCPTT-
  pair nodes is pooled per time bucket and reported as median +/-
  inter-quartile band (25th-75th percentile of the pooled samples).
  The band represents pair-to-pair spread within the running system at
  that time -- it does NOT shrink with K; a wide band indicates that
  pairs disagree on MCS (some collapsed, some not).  Bg-pair decisions
  are excluded for panel-to-panel consistency with the bottom panel.

  Bottom panel: per (config, seed), per-MCPTT-pair MoS is recomputed
  over [pair_start + warmup, simTime] from voip-packet-trace.csv using
  the E-model formula.  Per seed, the four MCPTT-pair MoS values are
  sorted ascending; per rank position [worst, 2nd worst, 2nd best,
  best] the cross-seed mean and Student-t 95% half-width are reported
  as bars + error caps.  This makes the contention-driven asymmetry the
  explicit subject of the panel rather than anchoring bars to pair
  index (which under contention may be a different collapsed pair
  across seeds).

  --warmup is a per-pair-relative truncation (relative to each pair's
  own start_time), used only for the bottom panel; the time-series
  panel plots the full simulation.

Quickstart:
    python3 src/nr-prose/examples/plot-mcs-time-series.py --warmup 30
    python3 src/nr-prose/examples/plot-mcs-time-series.py --skipRun --warmup 30

Single-config terminal experiment (recommended before running the full sweep):
    ./ns3 run nr-prose-adaptive-mcs -- \\
        --lossModel=umi --errorModel=epa --mcsController=ideal \\
        --idealSchedLevel=None --layout=ring --mcpttPairs=4 --bgPairs=8 \\
        --mediaPktSize=60 --mediaDataRate=148kb/s --maxNumTx=1 \\
        --simTime=180 --d=400 --RngRun=1
"""

import argparse
import csv
import math
import os
import shlex
import shutil
import subprocess
import sys
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MaxNLocator
from scipy import stats as scipy_stats

# (controller, idealSchedLevel, label, style)
CONFIGS = [
    ("ideal", "Global", "Ideal, no contention", {"color": "blue", "marker": "o", "hollow": False}),
    ("ideal", "None", "Ideal, with contention", {"color": "orange", "marker": "s", "hollow": True}),
    ("olla", "Global", "OLLA, no contention", {"color": "purple", "marker": "D", "hollow": False}),
    ("olla", "None", "OLLA, with contention", {"color": "green", "marker": "^", "hollow": False}),
]

TRIAL_FILES = {
    "ideal": "ideal-trial-results.csv",
    "olla": "olla-trial-results.csv",
}

VOIP_PKT_TRACE = "voip-packet-trace.csv"
VOIP_STATS = "voip-stats-sim.csv"
DIRECT_LINKS_TRACE = "direct-links-trace.csv"

# Files moved into per-run result directories after each simulation.
OUTPUT_FILES = [
    "ideal-trial-results.csv",
    "olla-trial-results.csv",
    VOIP_PKT_TRACE,
    VOIP_STATS,
    DIRECT_LINKS_TRACE,
    "sched-stats-total.csv",
    "sched-stats-per-node.csv",
    "phy-stats-sim.csv",
    "phy-stats-sim-total.csv",
    "phy-stats-sim-percent.csv",
    "node-position-trace.csv",
    "rlf-trace.csv",
    "rlc-tx-pdu-drop-per-node.csv",
    "rlc-tx-pdu-drop-total.csv",
]

# Per-event traces produced by the simulation but not consumed by this
# plotter.  Add a file here only after verifying no consumer needs it.
DISCARD_FILES = [
    "sl-channel-events-trace.csv",
    "mcptt-msg-stats.txt",
    "mcptt-state-machine-stats.txt",
    "mcptt-m2e-latency.dat",
    "mcptt-access-time.dat",
    "harq-feedback-trace.csv",
    "harq-tb-completion-trace.csv",
    "l1-sd-rsrp-trace.csv",
    "l3-sd-rsrp-trace.csv",
    "rlc-tx-pdu-drop-trace.csv",
    "pssch-per-tb-trace.csv",
    "mcs-change-trace.csv",
    "olla-sinr-estimate-trace.csv",
    "phy-stats-per-node.csv",
    "phy-stats-per-node-app.csv",
    "phy-stats-per-node-pc5s.csv",
]


def parse_args():
    p = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter, description=__doc__
    )
    p.add_argument("--simTime", type=float, default=180.0)
    p.add_argument("--d", type=float, default=400.0, help="Per-pair distance (m)")
    p.add_argument("--mcpttPairs", type=int, default=4, help="Number of MCPTT pairs (default 4)")
    p.add_argument(
        "--bgPairs", type=int, default=8, help="Number of background-traffic pairs (default 8)"
    )
    p.add_argument("--layout", default="ring", help="Node layout (ring or grid)")
    p.add_argument("--mediaPktSize", type=int, default=60)
    p.add_argument("--mediaDataRate", default="148kb/s")
    p.add_argument("--maxNumTx", type=int, default=1)
    p.add_argument("--lossModel", default="umi")
    p.add_argument("--errorModel", default="epa")
    p.add_argument(
        "--rngRuns",
        type=int,
        default=20,
        help="Number of independent RngRun replications per config",
    )
    p.add_argument(
        "--rngRunBase",
        type=int,
        default=1,
        help="First RngRun index; replications use [base, base+rngRuns-1]",
    )
    p.add_argument(
        "--warmup",
        type=float,
        default=None,
        help="Per-pair-relative warmup window in seconds, discarded "
        "from per-pair MoS computation in the bottom panel.  "
        "Required.",
    )
    p.add_argument(
        "--window",
        type=float,
        default=5.0,
        help="Time bucket width (s) for top-panel MCS aggregation",
    )
    p.add_argument(
        "--trafficStart",
        type=float,
        default=3.0,
        help="Match nr-prose-adaptive-mcs.cc's startTrafficTime",
    )
    p.add_argument("--resultsDir", default=os.path.join("results", "mcs-time-series"))
    p.add_argument("--out", default="mcs-time-series.png")
    p.add_argument(
        "--extraArgs", default="", help="Extra args passed verbatim to nr-prose-adaptive-mcs"
    )
    p.add_argument(
        "--skipBuild", action="store_true", help="Skip ./ns3 build (assume binary is current)"
    )
    p.add_argument(
        "--skipRun",
        action="store_true",
        help="Reuse per-config result directories under --resultsDir",
    )
    args = p.parse_args()
    if args.warmup is None:
        sys.exit(
            "--warmup is required (per-pair-relative truncation for the bottom-"
            "panel MoS).  Pass --warmup 0 to disable."
        )
    return args


def build():
    print("Building ns-3 ...", flush=True)
    r = subprocess.run(["./ns3", "build"], capture_output=True, text=True)
    if r.returncode != 0:
        sys.stderr.write(r.stderr[-1000:])
        sys.exit("build failed")
    print("Build OK.")


def save_version_info(args):
    """Record repository state and run parameters to ${resultsDir}/version.txt."""
    os.makedirs(args.resultsDir, exist_ok=True)
    out_path = os.path.join(args.resultsDir, "version.txt")

    def _git(*cmd):
        result = subprocess.run(["git", *cmd], capture_output=True, text=True)
        return result.stdout if result.returncode == 0 else ""

    branch = _git("rev-parse", "--abbrev-ref", "HEAD").strip()
    commit = _git("rev-parse", "--short", "HEAD").strip()
    date = _git("log", "-1", "--format=%cd").strip()
    diff = _git("diff")

    with open(out_path, "w") as f:
        f.write(f"{branch} commit {commit} {date}\n")
        f.write(
            f"warmup={args.warmup}s rngRuns={args.rngRuns} simTime={args.simTime}s "
            f"d={args.d}m mcpttPairs={args.mcpttPairs} bgPairs={args.bgPairs} "
            f"window={args.window}s\n"
        )
        if diff:
            f.write(diff)
            if not diff.endswith("\n"):
                f.write("\n")

    print(f"Recorded repository state to {out_path}")


def save_command_info(args):
    """Record the invocation command line to ${resultsDir}/command.txt
    (or command.skipRun.txt under --skipRun)."""
    os.makedirs(args.resultsDir, exist_ok=True)
    fname = "command.skipRun.txt" if args.skipRun else "command.txt"
    out_path = os.path.join(args.resultsDir, fname)
    full_command = " ".join(shlex.quote(arg) for arg in sys.argv)
    with open(out_path, "w") as f:
        f.write(full_command + "\n")
    print(f"Recorded invocation to {out_path}")


def run_dir_name(results_dir, controller, sched_level, run):
    return os.path.join(results_dir, f"{controller}_{sched_level}_r{run}")


def run_one(args, controller, sched_level, run):
    """Run one simulation instance and move output files to its run dir."""
    out_dir = run_dir_name(args.resultsDir, controller, sched_level, run)
    os.makedirs(out_dir, exist_ok=True)
    cmd = [
        "./ns3",
        "run",
        "--no-build",
        "nr-prose-adaptive-mcs",
        "--",
        f"--lossModel={args.lossModel}",
        f"--errorModel={args.errorModel}",
        f"--mcsController={controller}",
        f"--idealSchedLevel={sched_level}",
        f"--layout={args.layout}",
        f"--mcpttPairs={args.mcpttPairs}",
        f"--bgPairs={args.bgPairs}",
        f"--mediaPktSize={args.mediaPktSize}",
        f"--mediaDataRate={args.mediaDataRate}",
        f"--maxNumTx={args.maxNumTx}",
        f"--simTime={args.simTime}",
        f"--d={args.d}",
        f"--RngRun={run}",
    ]
    if args.extraArgs:
        cmd.extend(shlex.split(args.extraArgs))
    print(
        f"  Running [{controller}, sched={sched_level}, r{run}]: " f"{' '.join(cmd[3:])}",
        flush=True,
    )
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.stderr.write(r.stderr[-1500:])
        sys.exit(f"sim failed for {controller}/{sched_level}/r{run}")

    for fname in OUTPUT_FILES:
        if os.path.exists(fname):
            shutil.move(fname, os.path.join(out_dir, fname))

    for fname in DISCARD_FILES:
        if os.path.exists(fname):
            os.remove(fname)

    with open(os.path.join(out_dir, "cmdline.txt"), "w") as f:
        f.write(" ".join(cmd) + "\n")

    return out_dir


def app_start_times(traffic_start, sim_time, n_app_pairs):
    """Per-pair start times.  Returns 1-indexed list (index 0 unused)."""
    return [None] + [traffic_start + i * (sim_time / n_app_pairs) for i in range(n_app_pairs)]


# -- Per-seed parsing --


def parse_mcs_per_bucket_mcptt_only(run_dir, controller, mcptt_pairs, sim_time, window):
    """Bucket every controller MCS decision from MCPTT-pair nodes by time.

    The example program creates MCPTT pair endpoints first (nodes
    0 .. 2*mcpttPairs - 1), so we filter trial-results rows by nodeId
    against that range.  This is a convention rather than a contract;
    if the example program ever shuffles node creation order, the
    filter would silently include bg-pair decisions.

    Returns {bucket_idx: [mcs_value, ...]}; pooled per bucket across
    every (node, decision) within the bucket.
    """
    path = os.path.join(run_dir, TRIAL_FILES[controller])
    if not os.path.exists(path):
        return {}
    mcptt_node_count = 2 * mcptt_pairs
    buckets = defaultdict(list)
    with open(path) as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#"):
                continue
            t = float(row[0])
            if t < 0 or t > sim_time:
                continue
            node_id = int(row[1])
            if node_id >= mcptt_node_count:
                continue
            mcs = int(row[3])
            b = int(t // window)
            buckets[b].append(mcs)
    return buckets


def parse_per_pair_mos(run_dir, mcptt_pairs, traffic_start, sim_time, n_app_pairs, warmup):
    """Recompute per-MCPTT-pair MoS from voip-packet-trace.csv over the
    truncated window [pair_start + warmup, sim_time].

    Returns list of mcptt_pairs MoS values; only the file-missing case
    yields None.  A pair with zero transmissions in the analysis window
    is scored at the E-model floor (MoS=1.0): under contention, this is
    the cascade endpoint where PC5 link establishment never completes
    and the call delivers no audio.  Returning None here would cause
    the seed to be dropped downstream, silently excluding the worst
    cascade outcomes.

    The MoS formula is the E-model approximation used in
    nr-prose-adaptive-mcs.cc (kept consistent with Figure 1).
    """
    path = os.path.join(run_dir, VOIP_PKT_TRACE)
    if not os.path.exists(path):
        return [None] * mcptt_pairs

    by_app = defaultdict(list)
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            app_id = int(parts[0])
            if app_id < 1 or app_id > mcptt_pairs:
                continue
            tx = float(parts[1])
            try:
                delay = float(parts[5])
            except ValueError:
                delay = float("nan")
            by_app[app_id].append((tx, delay))

    starts = app_start_times(traffic_start, sim_time, n_app_pairs)
    mos_per_pair = []
    for app_id in range(1, mcptt_pairs + 1):
        pair_start = starts[app_id]
        t_lo = pair_start + warmup
        t_hi = sim_time
        n_tx = 0
        n_rx = 0
        total_delay_ms = 0.0
        for tx, delay in by_app.get(app_id, []):
            if tx < t_lo or tx > t_hi:
                continue
            n_tx += 1
            if not math.isnan(delay):
                n_rx += 1
                total_delay_ms += delay
        if n_tx == 0:
            mos_per_pair.append(1.0)
            continue
        loss = 1.0 - n_rx / n_tx
        avg_delay = total_delay_ms / n_rx if n_rx > 0 else 0.0
        r = 93.2 - (avg_delay * 0.024) - (loss * 100.0 * 2.5)
        if r < 0:
            r = 0.0
        mos = 1.0 + 0.035 * r + r * (r - 60.0) * (100.0 - r) * 7.0e-6
        mos_per_pair.append(mos)
    return mos_per_pair


def has_link_release(run_dir):
    path = os.path.join(run_dir, DIRECT_LINKS_TRACE)
    if not os.path.exists(path):
        return False
    with open(path) as f:
        for line in f:
            if "Released" in line:
                return True
    return False


# -- Aggregation --


def aggregate_mcs_buckets(per_seed_buckets, n_buckets):
    """For each bucket, pool MCS samples across seeds and report median,
    25th, and 75th percentiles.  Returns four parallel lists of length
    n_buckets: medians, p25s, p75s, sample counts.
    """
    medians = []
    p25s = []
    p75s = []
    counts = []
    for b in range(n_buckets):
        pooled = []
        for buckets in per_seed_buckets:
            pooled.extend(buckets.get(b, []))
        if pooled:
            medians.append(float(np.median(pooled)))
            p25s.append(float(np.percentile(pooled, 25)))
            p75s.append(float(np.percentile(pooled, 75)))
        else:
            medians.append(None)
            p25s.append(None)
            p75s.append(None)
        counts.append(len(pooled))
    return medians, p25s, p75s, counts


def aggregate_sorted_ranks(per_seed_pair_mos):
    """Per seed, sort the per-pair MoS values descending (best first);
    return per-rank list of cross-seed values for a strip-plot panel.

    Returns a list of length n_pairs where index r holds the list of MoS
    values at rank position r across seeds.  Seeds with any None per-pair
    MoS are skipped.  No mean/CI aggregation is performed because the
    underlying distribution is bimodal under contention; the bottom panel
    plots the raw per-seed values directly with horizontal jitter.
    """
    valid_seeds = [m for m in per_seed_pair_mos if m and not any(v is None for v in m)]
    if not valid_seeds:
        return []
    n_pairs = len(valid_seeds[0])
    per_rank = []
    for r in range(n_pairs):
        per_rank.append([sorted(seed_mos, reverse=True)[r] for seed_mos in valid_seeds])
    return per_rank


# -- Plot --


def plot(args, mcs_aggregates, mos_aggregates):
    fig, (ax_mcs, ax_mos) = plt.subplots(2, 1, figsize=(8, 9), layout="constrained")

    # Top panel: median MCS line + inter-quartile band per config.  The
    # three near-ceiling configurations are visually crowded, so the
    # legend is split: the with-contention OLLA entry sits in its own box
    # near its descending curve to make the marker/colour-to-curve mapping
    # easier to follow.
    n_buckets = int(args.simTime // args.window) + 1
    times = np.array([(b + 0.5) * args.window for b in range(n_buckets)])
    n_configs = len(CONFIGS)
    line_handles = {}
    for i, (controller, sched_level, label, style) in enumerate(CONFIGS):
        medians, p25s, p75s, _counts = mcs_aggregates[(controller, sched_level)]
        med = np.array([m if m is not None else np.nan for m in medians], dtype=float)
        lo = np.array([v if v is not None else np.nan for v in p25s], dtype=float)
        hi = np.array([v if v is not None else np.nan for v in p75s], dtype=float)
        color = style["color"]
        hollow = style["hollow"]
        (line,) = ax_mcs.plot(
            times,
            med,
            color=color,
            linewidth=1.6,
            marker=style["marker"],
            markersize=10 if hollow else 5,
            markerfacecolor="none" if hollow else color,
            markeredgecolor=color,
            markeredgewidth=1.4 if hollow else 1.0,
            markevery=(i, n_configs),
            label=label,
        )
        line_handles[(controller, sched_level)] = (line, label)
        valid = ~np.isnan(lo)
        if valid.any():
            ax_mcs.fill_between(times, lo, hi, where=valid, color=color, alpha=0.20, linewidth=0)

    # Pair start times (gray dotted verticals).
    n_app_pairs = args.mcpttPairs + args.bgPairs
    for t0 in app_start_times(args.trafficStart, args.simTime, n_app_pairs)[1:]:
        ax_mcs.axvline(t0, ls=":", color="gray", lw=0.6, zorder=0)

    LABEL_FONTSIZE = 14
    LEGEND_FONTSIZE = 12
    TICK_LABELSIZE = 12
    PANEL_LABEL_KWARGS = dict(
        fontsize=12,
        fontweight="bold",
        bbox=dict(facecolor="white", edgecolor="black", boxstyle="round,pad=0.3", alpha=0.9),
    )

    ax_mcs.set_ylabel(
        f"MCS (median per {args.window:.0f} s window;\n"
        f"shaded = 25th-75th pct. across {args.rngRuns} runs)",
        fontsize=LABEL_FONTSIZE,
    )
    ax_mcs.set_ylim(-0.5, 28.5)
    ax_mcs.set_xlim(0, args.simTime)
    ax_mcs.set_xlabel("Simulation time (s)", fontsize=LABEL_FONTSIZE)
    ax_mcs.xaxis.set_major_locator(MaxNLocator(8))
    ax_mcs.tick_params(axis="both", labelsize=TICK_LABELSIZE)
    ax_mcs.grid(alpha=0.3)
    ax_mcs.text(
        0.02,
        0.05,
        "Fig. 4a",
        transform=ax_mcs.transAxes,
        ha="left",
        va="bottom",
        **PANEL_LABEL_KWARGS,
    )

    # Two legends: the three near-ceiling configurations in the upper
    # right (away from their own crowded curves), and the with-contention
    # OLLA entry on its own near its descending curve so the reader can
    # map the marker/colour to the dropping line at a glance.
    high_rail_keys = [k for k in line_handles if k != ("olla", "None")]
    high_rail_lines = [line_handles[k][0] for k in high_rail_keys]
    high_rail_labels = [line_handles[k][1] for k in high_rail_keys]
    legend_top = ax_mcs.legend(
        high_rail_lines,
        high_rail_labels,
        bbox_to_anchor=(0.99, 0.85),
        loc="upper right",
        fontsize=LEGEND_FONTSIZE,
    )
    ax_mcs.add_artist(legend_top)
    olla_line, olla_label = line_handles[("olla", "None")]
    ax_mcs.legend(
        [olla_line],
        [olla_label],
        bbox_to_anchor=(0.99, 0.30),
        loc="upper right",
        fontsize=LEGEND_FONTSIZE,
    )

    # Bottom panel: strip plot.  Per (config, rank), one dot per seed with
    # small horizontal jitter so equal-MoS points do not stack.
    n_ranks = args.mcpttPairs
    n_configs = len(CONFIGS)
    cluster_width = 0.6 / n_configs
    jitter_amplitude = cluster_width * 0.35
    rng = np.random.default_rng(seed=0)  # deterministic jitter for reproducibility

    rank_x = list(range(n_ranks))
    for i, (controller, sched_level, label, style) in enumerate(CONFIGS):
        per_rank = mos_aggregates[(controller, sched_level)]
        if not per_rank:
            continue
        color = style["color"]
        hollow = style["hollow"]
        for r, values in enumerate(per_rank):
            x_center = r + (i - (n_configs - 1) / 2) * cluster_width
            x_dots = x_center + rng.uniform(-jitter_amplitude, jitter_amplitude, len(values))
            ax_mos.scatter(
                x_dots,
                values,
                marker=style["marker"],
                facecolor="none" if hollow else color,
                edgecolor=color,
                linewidth=0.8 if hollow else 0.3,
                s=28,
                alpha=0.85,
                label=label if r == 0 else None,
            )

    ax_mos.axhline(3.0, color="gray", lw=0.6, ls="--", alpha=0.6)
    ax_mos.set_xticks(rank_x)
    if n_ranks == 4:
        rank_labels = ["Best", "2nd best", "2nd worst", "Worst"]
    else:
        rank_labels = []
        for r in range(n_ranks):
            if r == 0:
                rank_labels.append("Best")
            elif r == n_ranks - 1:
                rank_labels.append("Worst")
            else:
                rank_labels.append(f"Rank {r + 1}")
    ax_mos.set_xticklabels(rank_labels)
    ax_mos.set_xlabel("MCPTT per-call MOS rank (sorted per-run by MoS)", fontsize=LABEL_FONTSIZE)
    ax_mos.set_ylabel(
        f"MoS, per seed (one dot per RngRun;\n" f"{args.rngRuns} runs, jittered horizontally)",
        fontsize=LABEL_FONTSIZE,
    )
    ax_mos.set_ylim(0, 4.6)
    ax_mos.tick_params(axis="both", labelsize=TICK_LABELSIZE)
    ax_mos.grid(alpha=0.3, axis="y")
    ax_mos.text(
        0.02,
        0.05,
        "Fig. 4b",
        transform=ax_mos.transAxes,
        ha="left",
        va="bottom",
        **PANEL_LABEL_KWARGS,
    )

    fig.savefig(args.out, dpi=150)
    print(f"Wrote {args.out}")


def write_estimates_csv(args, mcs_aggregates, mos_aggregates):
    """Dump per-config aggregated estimates for paper tables."""
    n_buckets = int(args.simTime // args.window) + 1
    times = [(b + 0.5) * args.window for b in range(n_buckets)]

    mcs_path = "point-estimates-time-series-mcs.csv"
    with open(mcs_path, "w") as f:
        f.write("controller,sched_level,t_center,median,p25,p75,n_samples\n")
        for controller, sched_level, _label, _style in CONFIGS:
            medians, p25s, p75s, counts = mcs_aggregates[(controller, sched_level)]
            for i, t in enumerate(times):
                med_s = "" if medians[i] is None else f"{medians[i]:.4f}"
                p25_s = "" if p25s[i] is None else f"{p25s[i]:.4f}"
                p75_s = "" if p75s[i] is None else f"{p75s[i]:.4f}"
                f.write(
                    f"{controller},{sched_level},{t:.2f},{med_s},{p25_s}," f"{p75_s},{counts[i]}\n"
                )
    print(f"MCS time-series estimates written to {mcs_path}")

    mos_path = "point-estimates-time-series-mos.csv"
    with open(mos_path, "w") as f:
        f.write("controller,sched_level,rank,seed_idx,mos\n")
        for controller, sched_level, _label, _style in CONFIGS:
            per_rank = mos_aggregates[(controller, sched_level)]
            for r, values in enumerate(per_rank):
                for s_idx, mos in enumerate(values):
                    f.write(f"{controller},{sched_level},{r},{s_idx},{mos:.4f}\n")
    print(f"MoS per-seed sorted-rank values written to {mos_path}")


# -- Main --


def main():
    args = parse_args()

    if not args.skipRun and not args.skipBuild:
        build()

    os.makedirs(args.resultsDir, exist_ok=True)
    if not args.skipRun:
        save_version_info(args)
    save_command_info(args)

    runs = list(range(args.rngRunBase, args.rngRunBase + args.rngRuns))
    n_app_pairs = args.mcpttPairs + args.bgPairs
    n_buckets = int(args.simTime // args.window) + 1

    if not args.skipRun:
        print(f"\nRunning configs={[c[0]+'/'+c[1] for c in CONFIGS]} " f"for RngRun in {runs}")
        # Outer loop on RngRun so an early termination leaves every config
        # with the same number of completed seeds, rather than all seeds
        # for some configs and none for others.
        for run in runs:
            for controller, sched_level, _label, _style in CONFIGS:
                run_one(args, controller, sched_level, run)

    print("\nCollecting results ...")
    mcs_aggregates = {}
    mos_aggregates = {}
    for controller, sched_level, _label, _style in CONFIGS:
        per_seed_buckets = []
        per_seed_pair_mos = []
        for run in runs:
            out_dir = run_dir_name(args.resultsDir, controller, sched_level, run)
            if not os.path.isdir(out_dir):
                print(f"  Missing {out_dir}; skipping seed")
                continue
            # Note: under contention at d=600 m, MCPTT or bg pair direct
            # links may release as the cascade endpoint.  parse_per_pair_mos
            # records post-release packets as losses (via NaN RxDelay in
            # voip-packet-trace), so the seed's data is a valid signal and
            # the seed is kept rather than skipped.
            if has_link_release(out_dir):
                print(f"  {out_dir}: direct link released " "(kept; expected under contention)")
            buckets = parse_mcs_per_bucket_mcptt_only(
                out_dir, controller, args.mcpttPairs, args.simTime, args.window
            )
            per_seed_buckets.append(buckets)
            mos = parse_per_pair_mos(
                out_dir, args.mcpttPairs, args.trafficStart, args.simTime, n_app_pairs, args.warmup
            )
            per_seed_pair_mos.append(mos)
        mcs_aggregates[(controller, sched_level)] = aggregate_mcs_buckets(
            per_seed_buckets, n_buckets
        )
        mos_aggregates[(controller, sched_level)] = aggregate_sorted_ranks(per_seed_pair_mos)

    write_estimates_csv(args, mcs_aggregates, mos_aggregates)
    plot(args, mcs_aggregates, mos_aggregates)


if __name__ == "__main__":
    main()
