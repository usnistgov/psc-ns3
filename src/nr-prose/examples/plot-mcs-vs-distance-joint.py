#!/usr/bin/python3

"""
Sweep inter-node distance across the four dynamic-numTx configurations,
replicate K times per (distance, config), and plot mean +/- Student-t 95%
CI band for MCS, subchannels-per-TB, transmissions-per-TB, and MoS vs.
distance.

Parallel variant of plot-mcs-vs-distance-dynamic-numtx.py: independent
(distance, config, RngRun) tasks are dispatched to a multiprocessing.Pool
of size --jobs.  Each worker invokes ./ns3 run with --cwd pointing at the
per-task results directory, so output files land directly there and no
two workers contend for the shared current working directory.

Topology: ring layout with a single MCPTT pair (mcpttPairs=1, bgPairs=0).
Two nodes are placed antipodally on a circle; the source-destination
distance is exactly --d for every run, so the x-axis is d directly.

The controller (Ideal or OLLA) is selected via --mcsController; the four
configurations differ only in the HARQ structure used by the scheduler
and by the controller's target:

  numtx1      maxNumTx=1                                      (no HARQ)
  first-tx    maxNumTx=3, HarqAwareTarget=0, DynamicNumTx=0
  harq-aware  maxNumTx=3, HarqAwareTarget=1, DynamicNumTx=0
  dynamic     maxNumTx=3, HarqAwareTarget=1, DynamicNumTx=1

Methodology:
  Each (distance, config, seed) run is executed at fixed simTime large
  enough to deliver at least --eventCount first-tx voice TBs.  Per-seed
  point estimates are computed on the truncated window [warmup, t_N] where
  t_N is the time of the N-th voice Tx at the application layer.  Across
  seeds, the figure shows mean +/- t_{0.025, K-1} * s / sqrt(K).

  --warmup is determined separately.

Usage:
    python3 plot-mcs-vs-distance-joint.py --warmup 90 --rngRuns 5 --jobs 8
    python3 plot-mcs-vs-distance-joint.py --skipRun --warmup 90
"""

import argparse
import csv
import math
import multiprocessing as mp
import os
import shlex
import subprocess
import sys

# Force the 'fork' start method so worker processes inherit the parent's
# argparse Namespace and CONFIGS table without re-executing argument parsing.
# Set before any process pool is created.
mp.set_start_method("fork", force=True)

parser = argparse.ArgumentParser(
    description="Sweep inter-node distance across dynamic-numTx configurations (parallel)"
)
parser.add_argument("--dMin", type=float, default=20, help="Minimum pair distance d (m)")
parser.add_argument("--dMax", type=float, default=1000, help="Maximum pair distance d (m)")
parser.add_argument("--dStep", type=float, default=20, help="Step size for d (m)")
parser.add_argument(
    "--rngRuns",
    type=int,
    default=5,
    help="Number of independent RngRun replications per (d, config)",
)
parser.add_argument(
    "--rngRunBase",
    type=int,
    default=1,
    help="First RngRun index; replications use [base, base+rngRuns-1]",
)
parser.add_argument(
    "--warmup",
    type=float,
    required=False,
    default=None,
    help="Warmup window in seconds, discarded from per-run statistics. "
    "Determined empirically by pilot-warmup.py.",
)
parser.add_argument(
    "--eventCount",
    type=int,
    default=14000,
    help="Per-run target count of first-tx voice TBs (post-warmup); each run "
    "is truncated at the time of the N-th voice Tx for analysis.",
)
parser.add_argument("--simTime", type=float, default=700, help="Simulation time (s)")
parser.add_argument("--mcpttPairs", type=int, default=1, help="Number of MCPTT pairs")
parser.add_argument("--bgPairs", type=int, default=0, help="Number of background-traffic pairs")
parser.add_argument("--layout", type=str, default="ring", help="Node layout (ring or grid)")
parser.add_argument(
    "--lossModel", type=str, default="umi", help="Loss model (matrix, friis, log-distance, umi)"
)
parser.add_argument("--errorModel", type=str, default="epa", help="Error model (static, epa)")
parser.add_argument(
    "--mcsController",
    type=str,
    default="olla",
    choices=["ideal", "olla"],
    help="MCS controller under test (ideal or olla).  Both expose the same "
    "HarqAwareTarget and DynamicNumTx attributes.",
)
parser.add_argument(
    "--configs",
    nargs="+",
    default=["numtx1", "first-tx", "harq-aware", "dynamic"],
    help="Configurations to run",
)
_SCRIPT_PREFIX = os.path.splitext(os.path.basename(__file__))[0]
parser.add_argument(
    "--resultsDir",
    type=str,
    default=os.path.join("results", _SCRIPT_PREFIX),
    help="Directory to store per-run output files",
)
parser.add_argument(
    "--jobs",
    type=int,
    default=os.cpu_count() or 1,
    help="Number of parallel worker processes for the run phase. "
    "Each worker runs one ./ns3 simulation at a time in its own results "
    "subdirectory.  Default: os.cpu_count().",
)
parser.add_argument(
    "--skipBuild", action="store_true", help="Skip ./ns3 build (assume binary is current)"
)
parser.add_argument(
    "--skipRun", action="store_true", help="Skip simulation runs; plot from existing results"
)
parser.add_argument(
    "--quiet",
    action="store_true",
    help="Silently skip distances whose run directory is absent (lets you "
    "request a finer dStep than the actual run grid for plotting purposes)",
)
parser.add_argument(
    "--show", action="store_true", help="Show plot interactively instead of saving PNG"
)
args = parser.parse_args()

if args.warmup is None and not args.skipRun:
    sys.exit(
        "--warmup is required.  Run pilot-warmup.py and pass the recommended "
        "value (or --warmup 0 to skip warmup truncation)."
    )
if args.warmup is None:
    sys.exit("--warmup is required even with --skipRun (used to truncate the parsing window).")

# Per-configuration recipe.  Each entry lists the extra args forwarded to
# nr-prose-adaptive-mcs and the styling for the plot series.  The controller
# attribute paths are parameterized by the chosen --mcsController so the
# same CONFIGS table works for ideal and olla.
_CONTROLLER_TYPEID = {
    "ideal": "NrSlIdealMcsController",
    "olla": "NrSlOllaMcsController",
}
_TYPEID = _CONTROLLER_TYPEID[args.mcsController]

CONFIGS = {
    "numtx1": {
        "label": r"Retransmissions disabled ($n_\mathrm{TX}=1$)",
        "color": "black",
        "marker": "s",
        "linestyle": "-",
        "hollow": True,
        "args": ["--maxNumTx=1"],
    },
    "first-tx": {
        "label": r"Initial Tx BLER target, $n_\mathrm{TX}=3$",
        "color": "blue",
        "marker": "D",
        "linestyle": "-",
        "args": [
            "--maxNumTx=3",
            f"--ns3::{_TYPEID}::HarqAwareTarget=0",
            f"--ns3::{_TYPEID}::DynamicNumTx=0",
        ],
    },
    "harq-aware": {
        "label": r"Post-HARQ BLER target, $n_\mathrm{TX}=3$",
        "color": "orange",
        "marker": "^",
        "linestyle": "-",
        "args": [
            "--maxNumTx=3",
            f"--ns3::{_TYPEID}::HarqAwareTarget=1",
            f"--ns3::{_TYPEID}::DynamicNumTx=0",
        ],
    },
    "dynamic": {
        "label": r"Joint (MCS, $n_\mathrm{TX}$) selection, max $n_\mathrm{TX}=3$",
        "color": "green",
        "marker": "o",
        "linestyle": "-",
        "args": [
            "--maxNumTx=3",
            f"--ns3::{_TYPEID}::HarqAwareTarget=1",
            f"--ns3::{_TYPEID}::DynamicNumTx=1",
        ],
    },
}


# Output CSV files produced by the simulation
PSSCH_PER_TB_TRACE = "pssch-per-tb-trace.csv"
PSSCH_SCHED_TRACE = "pssch-sched-trace.csv"
VOIP_PKT_TRACE = "voip-packet-trace.csv"
SCHED_TOTAL = "sched-stats-total.csv"
VOIP_STATS = "voip-stats-sim.csv"
DIRECT_LINKS_TRACE = "direct-links-trace.csv"
# nr-prose-adaptive-mcs writes "<mcsController>-trial-results.csv"
TRIAL_FILE = f"{args.mcsController}-trial-results.csv"

# Per-event traces produced by the simulation but not consumed by the
# plotter or the warmup pilot.  Removed from each run directory after the
# simulation completes.
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
    # Remove the following entry from the list if pilot-warmup mode is needed.
    "olla-sinr-estimate-trace.csv",
]

# Default subchannel size (RBs); pool config in nr-prose-adaptive-mcs.cc.
SUB_CH_SIZE_RBS = 10


def node_distance(d):
    """Source-destination distance for the MCPTT pair under the configured layout.

    Ring layout (default): pairs are antipodal on a circle of radius d/2,
    so every pair is at distance d.  Grid layout (legacy): pair (1, N) is
    corner-to-corner at (sqrt(N)-1) * d * sqrt(2).
    """
    if args.layout == "ring":
        return d
    n = 2 * (args.mcpttPairs + args.bgPairs)
    grid_side = int(round(n**0.5))
    return (grid_side - 1) * d * (2**0.5)


def run_dir_name(d, config_name, run):
    """Per-(d, config, RngRun) results subdirectory."""
    return os.path.join(args.resultsDir, f"d{d:.0f}_{config_name}_r{run}")


def build_program():
    """Build the simulation program once.  Exit if the build fails."""
    print("Building ns-3 ...", flush=True)
    result = subprocess.run(["./ns3", "build"], capture_output=True, text=True)
    if result.returncode != 0:
        print("ERROR: build failed:")
        if result.stdout:
            print(result.stdout[-1000:])
        if result.stderr:
            print(result.stderr[-1000:])
        sys.exit(1)
    print("Build succeeded.")


def save_version_info():
    """Record repository state to ${resultsDir}/version.txt."""
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
            f"warmup={args.warmup}s eventCount={args.eventCount} "
            f"rngRuns={args.rngRuns} simTime={args.simTime}s jobs={args.jobs}\n"
        )
        if diff:
            f.write(diff)
            if not diff.endswith("\n"):
                f.write("\n")

    print(f"Recorded repository state to {out_path}")


def save_command_info():
    """Record the invocation command line to ${resultsDir}/command.txt
    (or command.skipRun.txt under --skipRun)."""
    os.makedirs(args.resultsDir, exist_ok=True)
    fname = "command.skipRun.txt" if args.skipRun else "command.txt"
    out_path = os.path.join(args.resultsDir, fname)
    full_command = " ".join(shlex.quote(arg) for arg in sys.argv)
    with open(out_path, "w") as f:
        f.write(full_command + "\n")
    print(f"Recorded invocation to {out_path}")


def run_simulation_task(task):
    """Run one simulation instance with --cwd pointing at its run directory.

    Returns (d, config_name, run, ok, err_tail) where err_tail is the last
    chunk of stderr on failure or None on success.  Workers do not print to
    stdout themselves; the parent process prints progress as results arrive.
    """
    d, config_name, run = task
    run_dir = run_dir_name(d, config_name, run)
    os.makedirs(run_dir, exist_ok=True)
    abs_run_dir = os.path.abspath(run_dir)
    cfg = CONFIGS[config_name]
    cmd = [
        "./ns3",
        "run",
        "--cwd",
        abs_run_dir,
        "--no-build",
        "nr-prose-adaptive-mcs",
        "--",
        f"--d={d}",
        f"--mcsController={args.mcsController}",
        f"--mcpttPairs={args.mcpttPairs}",
        f"--bgPairs={args.bgPairs}",
        f"--layout={args.layout}",
        f"--lossModel={args.lossModel}",
        f"--errorModel={args.errorModel}",
        f"--simTime={args.simTime}",
        f"--RngRun={run}",
    ]
    cmd.extend(cfg["args"])
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        return (d, config_name, run, False, (result.stderr or "")[-500:])

    for fname in DISCARD_FILES:
        p = os.path.join(run_dir, fname)
        if os.path.exists(p):
            os.remove(p)

    with open(os.path.join(run_dir, "cmdline.txt"), "w") as f:
        f.write(" ".join(cmd) + "\n")

    return (d, config_name, run, True, None)


# -- Per-seed parsing --


def load_voip_pkt_trace(run_dir):
    """Return list of (appId, txTime, rxTime, seq, size, rxDelayMs).

    rxDelayMs is NaN if the packet was lost.  Sorted by txTime.
    """
    path = os.path.join(run_dir, VOIP_PKT_TRACE)
    if not os.path.exists(path):
        return []
    rows = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            app_id = int(parts[0])
            tx = float(parts[1])
            rx = float(parts[2])
            seq = int(parts[3])
            size = int(parts[4])
            try:
                delay = float(parts[5])
            except ValueError:
                delay = float("nan")
            rows.append((app_id, tx, rx, seq, size, delay))
    rows.sort(key=lambda r: r[1])
    return rows


def truncation_window(voip_rows, warmup, event_count):
    """Return (t_lo, t_hi, n_tx_in_window).

    t_lo = warmup; t_hi = txTime of the (event_count)-th voice tx in
    [warmup, simEnd].  Returns (None, None, n) if fewer than event_count txs
    are available post-warmup -- caller must handle that as a short run.
    """
    txs_post_warmup = [r[1] for r in voip_rows if r[1] >= warmup]
    if len(txs_post_warmup) < event_count:
        return None, None, len(txs_post_warmup)
    t_hi = txs_post_warmup[event_count - 1]
    return warmup, t_hi, event_count


def voip_loss_mos(voip_rows, t_lo, t_hi):
    """Recompute (loss, mos, avgDelayMs, nTx) from voip-packet-trace over [t_lo, t_hi].

    MoS is the E-model approximation used in nr-prose-adaptive-mcs.cc.
    """
    n_tx = 0
    n_rx = 0
    total_delay_ms = 0.0
    for _, tx, rx, _, _, delay in voip_rows:
        if tx < t_lo or tx > t_hi:
            continue
        n_tx += 1
        if not math.isnan(delay):
            n_rx += 1
            total_delay_ms += delay
    if n_tx == 0:
        return None, None, None, 0
    loss = 1.0 - n_rx / n_tx
    avg_delay = total_delay_ms / n_rx if n_rx > 0 else 0.0
    r = 93.2 - (avg_delay * 0.024) - (loss * 100.0 * 2.5)
    if r < 0:
        r = 0.0
    mos = 1.0 + 0.035 * r + r * (r - 60.0) * (100.0 - r) * 7.0e-6
    return loss, mos, avg_delay, n_tx


def mcs_mean_in_window(run_dir, t_lo, t_hi):
    """Mean MCS across rows in <controller>-trial-results.csv with t in [t_lo, t_hi]."""
    path = os.path.join(run_dir, TRIAL_FILE)
    if not os.path.exists(path):
        return None
    total = 0
    count = 0
    with open(path) as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#"):
                continue
            t = float(row[0])
            if t < t_lo or t > t_hi:
                continue
            total += int(row[3])
            count += 1
    return total / count if count > 0 else None


def pssch_counts_in_window(run_dir, t_lo, t_hi):
    """Walk pssch-per-tb-trace.csv once and tally everything we need.

    Returns dict with keys n_new (NDI=1 row count), n_retx (NDI=0 row
    count), total_rbs (RB sum across all rows, used to compute realized
    per-TB airtime), total_rbs_new (RB sum across NDI=1 rows only, used
    to recover the per-first-transmission subchannel width), and bad_new
    (NDI=1 rows with corrupt or sci2Corrupt set, used for first-tx PHY
    loss).  Returns None if file missing.
    """
    path = os.path.join(run_dir, PSSCH_PER_TB_TRACE)
    if not os.path.exists(path):
        return None
    n_new = 0
    n_retx = 0
    total_rbs = 0
    total_rbs_new = 0
    bad_new = 0
    with open(path) as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#"):
                continue
            t = float(row[0])
            if t < t_lo or t > t_hi:
                continue
            n_rbs = int(row[6])
            corrupt = int(row[10])
            sci2_corrupt = int(row[11])
            ndi = int(row[12])
            total_rbs += n_rbs
            if ndi == 1:
                n_new += 1
                total_rbs_new += n_rbs
                if corrupt or sci2_corrupt:
                    bad_new += 1
            else:
                n_retx += 1
    return {
        "n_new": n_new,
        "n_retx": n_retx,
        "total_rbs": total_rbs,
        "total_rbs_new": total_rbs_new,
        "bad_new": bad_new,
    }


def pssch_sched_announced_footprint(run_dir, t_lo, t_hi):
    """Sum and count of per-TB announced reservation footprint
    (subch_per_tx * announcedNumTx) across NDI=1 rows of
    pssch-sched-trace.csv with time in [t_lo, t_hi].  Returns
    (total, count); both are 0 / file-missing yields (0.0, 0).

    Columns: time(s), srcL2Id, dstL2Id, harqId, ndi, rv, subChannelSize,
    rbLength, announcedNumTx (empty when not populated).
    """
    path = os.path.join(run_dir, PSSCH_SCHED_TRACE)
    if not os.path.exists(path):
        return 0.0, 0
    total = 0.0
    count = 0
    with open(path) as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#"):
                continue
            t = float(row[0])
            if t < t_lo or t > t_hi:
                continue
            ndi = int(row[4])
            if ndi != 1:
                continue
            sub_ch_size = int(row[6])
            rb_length = int(row[7])
            announced_str = row[8]
            if not announced_str:
                continue
            announced = int(announced_str)
            subch_per_tx = rb_length / sub_ch_size
            total += subch_per_tx * announced
            count += 1
    return total, count


def has_link_release(run_dir):
    """Return True if direct-links-trace.csv contains any Released events."""
    path = os.path.join(run_dir, DIRECT_LINKS_TRACE)
    if not os.path.exists(path):
        return False
    with open(path) as f:
        for line in f:
            if "Released" in line:
                return True
    return False


def parse_one_run(run_dir, warmup, event_count):
    """Return dict of per-seed point estimates over the truncated window,
    or None on failure / short run."""
    voip_rows = load_voip_pkt_trace(run_dir)
    if not voip_rows:
        return None
    t_lo, t_hi, n_window = truncation_window(voip_rows, warmup, event_count)
    if t_lo is None:
        print(
            f"    SHORT: only {n_window} voice TBs post-warmup in {run_dir} "
            f"(need {event_count})"
        )
        return None
    loss, mos, avg_delay, n_tx = voip_loss_mos(voip_rows, t_lo, t_hi)
    pssch = pssch_counts_in_window(run_dir, t_lo, t_hi)
    if pssch is None or pssch["n_new"] == 0:
        return None
    subch_per_tb = (pssch["total_rbs"] / pssch["n_new"]) / SUB_CH_SIZE_RBS
    subch_per_tx_new = (pssch["total_rbs_new"] / pssch["n_new"]) / SUB_CH_SIZE_RBS
    avg_tx = (pssch["n_new"] + pssch["n_retx"]) / pssch["n_new"]
    phy_loss = pssch["bad_new"] / pssch["n_new"]
    announced_total, announced_count = pssch_sched_announced_footprint(run_dir, t_lo, t_hi)
    subch_per_tb_announced = announced_total / announced_count if announced_count > 0 else None
    subch_per_voice_pkt = announced_total / n_tx if n_tx > 0 else None
    subch_actual_per_voice_pkt = (pssch["total_rbs"] / SUB_CH_SIZE_RBS) / n_tx if n_tx > 0 else None
    return {
        "loss": loss,
        "mos": mos,
        "avgDelayMs": avg_delay,
        "nTx": n_tx,
        "mcs": mcs_mean_in_window(run_dir, t_lo, t_hi),
        "subchPerTb": subch_per_tb,
        "subchPerTxNew": subch_per_tx_new,
        "subchPerTbAnnounced": subch_per_tb_announced,
        "subchPerVoicePkt": subch_per_voice_pkt,
        "subchActualPerVoicePkt": subch_actual_per_voice_pkt,
        "avgTx": avg_tx,
        "phyLoss": phy_loss,
        "t_lo": t_lo,
        "t_hi": t_hi,
    }


# -- Main --

if not args.skipRun and not args.skipBuild:
    build_program()

if not args.skipRun:
    save_version_info()
save_command_info()


# Build the (d, config, run) task grid.  np.arange isn't available yet
# (numpy is imported lazily below), so use a small float-range helper.
def _frange(start, stop, step):
    n = int(round((stop - start) / step)) + 1
    return [start + i * step for i in range(n) if start + i * step <= stop + step / 2]


distances_d = _frange(args.dMin, args.dMax, args.dStep)
runs = list(range(args.rngRunBase, args.rngRunBase + args.rngRuns))

if not args.skipRun:
    tasks = [(d, c, r) for d in distances_d for c in args.configs for r in runs]
    n_tasks = len(tasks)
    n_jobs = max(1, min(args.jobs, n_tasks))
    print(
        f"\nDispatching {n_tasks} runs across {n_jobs} workers: "
        f"configs={args.configs}, d in [{args.dMin}, {args.dMax}] step {args.dStep}, "
        f"RngRun in {runs}",
        flush=True,
    )
    n_done = 0
    n_fail = 0
    with mp.Pool(processes=n_jobs) as pool:
        for d, config_name, run, ok, err_tail in pool.imap_unordered(run_simulation_task, tasks):
            n_done += 1
            status = "OK" if ok else "FAIL"
            print(
                f"  [{n_done}/{n_tasks}] d={d:.0f} {config_name} r{run} {status}",
                flush=True,
            )
            if not ok:
                n_fail += 1
                if err_tail:
                    print(f"    {err_tail}", flush=True)
    print(f"Run phase complete: {n_done - n_fail} ok, {n_fail} failed.", flush=True)

# Defer matplotlib/numpy/scipy imports until after the parallel section so
# their initialization does not happen before fork() in the parent process.
import matplotlib

if not args.show:
    matplotlib.use("Agg")

import numpy as np
from matplotlib import pyplot as plt
from scipy import stats as scipy_stats


def aggregate_seeds(per_seed_values):
    """Across-seed mean and Student-t 95% half-width.

    Returns (mean, half_width) -- half_width is None when K < 2 or the array
    contains only None.
    """
    vals = [v for v in per_seed_values if v is not None]
    if not vals:
        return None, None
    if len(vals) == 1:
        return vals[0], None
    mean = float(np.mean(vals))
    sd = float(np.std(vals, ddof=1))
    t = float(scipy_stats.t.ppf(0.975, df=len(vals) - 1))
    half = t * sd / math.sqrt(len(vals))
    return mean, half


print("\nCollecting results ...")
results = {c: {"distance": [], "nSeeds": [], "metrics": {}} for c in args.configs}
metric_keys = [
    "mcs",
    "subchPerTb",
    "subchPerTxNew",
    "subchActualPerVoicePkt",
    "avgTx",
    "loss",
    "phyLoss",
    "mos",
]
for c in args.configs:
    for k in metric_keys:
        results[c]["metrics"][k] = {"mean": [], "half": []}

for d in distances_d:
    dist = node_distance(d)
    for config_name in args.configs:
        per_seed = []
        for run in runs:
            run_dir = run_dir_name(d, config_name, run)
            if args.quiet and not os.path.isdir(run_dir):
                continue
            if has_link_release(run_dir):
                print(f"  Dropping d={d:.0f} {config_name} r{run}: direct link released")
                continue
            point = parse_one_run(run_dir, args.warmup, args.eventCount)
            if point is not None:
                if point["subchPerVoicePkt"] is not None:
                    point["subchPerTb"] = point["subchPerVoicePkt"]
                per_seed.append(point)
        if not per_seed:
            print(f"  d={d:.0f} {config_name}: no usable seeds; skipping point")
            continue
        results[config_name]["distance"].append(dist)
        results[config_name]["nSeeds"].append(len(per_seed))
        for k in metric_keys:
            mean, half = aggregate_seeds([p[k] for p in per_seed])
            results[config_name]["metrics"][k]["mean"].append(mean)
            results[config_name]["metrics"][k]["half"].append(half)

# Dump aggregated point estimates to CSV for inspection and paper tables.
estimates_path = "point-estimates-joint.csv"
with open(estimates_path, "w") as f:
    f.write("d,config,metric,mean,ci_half_width,n_seeds\n")
    for config_name in args.configs:
        r = results[config_name]
        for i, dist in enumerate(r["distance"]):
            n_seeds = r["nSeeds"][i]
            for k in metric_keys:
                mean = r["metrics"][k]["mean"][i]
                half = r["metrics"][k]["half"][i]
                mean_s = "" if mean is None else f"{mean:.6f}"
                half_s = "" if half is None else f"{half:.6f}"
                f.write(f"{dist:.1f},{config_name},{k},{mean_s},{half_s},{n_seeds}\n")
print(f"Point estimates written to {estimates_path}")

# -- Plot --

print("Generating plot...")
fig, axes = plt.subplots(4, 1, figsize=(8, 14), sharex=True, layout="constrained")

PANEL_KEYS = ["mcs", "subchPerTb", "subchActualPerVoicePkt", "mos"]


def plot_panel(ax, config_name, dist, mean, half):
    cfg = CONFIGS[config_name]
    x_plot = np.array(dist, dtype=float)
    mean_arr = np.array([m if m is not None else np.nan for m in mean], dtype=float)
    # Hollow markers for curves that frequently overlap another series; the
    # filled marker of the other series is visible inside the outline.
    hollow = cfg.get("hollow", False)
    ax.plot(
        x_plot,
        mean_arr,
        color=cfg["color"],
        marker=cfg["marker"],
        linestyle=cfg["linestyle"],
        markersize=12 if hollow else 6,
        markerfacecolor="none" if hollow else cfg["color"],
        markeredgecolor=cfg["color"],
        markeredgewidth=1.5 if hollow else 1.0,
        linewidth=1.4,
        label=cfg["label"],
    )
    half_arr = np.array([h if h is not None else 0.0 for h in half], dtype=float)
    if np.any(half_arr > 0):
        ax.fill_between(
            x_plot,
            mean_arr - half_arr,
            mean_arr + half_arr,
            color=cfg["color"],
            alpha=0.18,
            linewidth=0,
        )


for config_name in args.configs:
    r = results[config_name]
    if not r["distance"]:
        continue
    for ax, key in zip(axes, PANEL_KEYS):
        plot_panel(
            ax, config_name, r["distance"], r["metrics"][key]["mean"], r["metrics"][key]["half"]
        )

LABEL_FONTSIZE = 14
LEGEND_FONTSIZE = 14
TICK_LABELSIZE = 14

PANEL_LABEL_KWARGS = dict(
    fontsize=LABEL_FONTSIZE,
    fontweight="bold",
    bbox=dict(facecolor="white", edgecolor="black", boxstyle="round,pad=0.3", alpha=0.9),
)

axes[0].set_ylabel("MCS Index (mean)", fontsize=LABEL_FONTSIZE)
axes[0].set_ylim(-1, 29)
axes[0].grid(True, alpha=0.3)
axes[0].text(
    0.98, 0.95, "Fig. 3a", transform=axes[0].transAxes, ha="right", va="top", **PANEL_LABEL_KWARGS
)

axes[1].set_ylabel("Mean resources reserved\n(subchannels/voice packet)", fontsize=LABEL_FONTSIZE)
axes[1].grid(True, alpha=0.3)
axes[1].set_ylim(0, 20)
axes[1].legend(loc="upper left", fontsize=LEGEND_FONTSIZE)
axes[1].text(
    0.98, 0.95, "Fig. 3b", transform=axes[1].transAxes, ha="right", va="top", **PANEL_LABEL_KWARGS
)

axes[2].set_ylabel(
    "Mean resources transmitted\n(subchannels/voice packet)", fontsize=LABEL_FONTSIZE
)
axes[2].grid(True, alpha=0.3)
axes[2].set_ylim(0, 20)
axes[2].legend(loc="upper left", fontsize=LEGEND_FONTSIZE)
axes[2].text(
    0.98, 0.95, "Fig. 3c", transform=axes[2].transAxes, ha="right", va="top", **PANEL_LABEL_KWARGS
)

axes[3].set_ylabel("Mean Opinion Score", fontsize=LABEL_FONTSIZE)
xlabel = "Inter-node distance d (m)" if args.layout == "ring" else "Source-Destination Distance (m)"
axes[3].set_xlabel(xlabel, fontsize=LABEL_FONTSIZE)
axes[3].grid(True, alpha=0.3)
axes[3].legend(loc="lower left", fontsize=LEGEND_FONTSIZE)
axes[3].set_ylim(0)
axes[3].text(
    0.98,
    0.05,
    "Fig. 3d",
    transform=axes[3].transAxes,
    ha="right",
    va="bottom",
    **PANEL_LABEL_KWARGS,
)

for ax in axes:
    ax.tick_params(axis="both", labelsize=TICK_LABELSIZE)

outfile = "mcs-controller-vs-distance-joint.png"
plt.savefig(outfile, dpi=150)
print(f"Plot saved to {outfile}")

if args.show:
    plt.show()
