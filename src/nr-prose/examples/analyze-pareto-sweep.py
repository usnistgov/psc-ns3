#!/usr/bin/env python3
"""
Analyze the output of nr-prose-mcs-pareto-sweep.

Two studies run against the same CSV:

  (A) 2-D vs 1-D controller (fixed numTx = first transmission only).
      For each (widebandSnrDb, tbSizeBytes, target BLER):
        - 1-D pick = current OLLA: max MCS whose first-tx BLER at the
          minimum capacity-feasible N_sc passes target.
        - 2-D pick = resource-minimal: smallest N_sc at which any
          capacity-feasible MCS passes target; tie-break by max MCS.
      Reports where 2-D strictly beats 1-D (coverage, N_sc, MCS).

  (B) 3-D (MCS, N_sc, numTx) vs current OLLA, given the pool's
      configured maxNumTx. For each (snr, tb, target):
        - Current OLLA: as in (A); resource = N_sc * maxNumTx
          (SCI reservation footprint when the pool is configured for
          up to maxNumTx transmissions).
        - Fixed-numTx HARQ-aware: max MCS whose post-maxNumTx
          effective BLER at min-capacity N_sc passes target;
          resource = N_sc * maxNumTx.
        - Optimizing: argmin (N_sc * numTx) over all capacity-feasible
          (mcs, nSc, numTx) triples with post-numTx BLER <= target.
      Reports mean resource reduction and fraction of points where
      optimizing picks numTx < maxNumTx.

Outputs summary CSV, a 2-D bar plot (study A), and a 3-D plot (study B).

Usage:
    python3 analyze-pareto-sweep.py [--csv mcs-pareto-sweep.csv]
                                    [--target 0.001,0.01,0.03,0.05,0.10]
                                    [--summary-csv out.csv]
                                    [--plot-2d pareto-2d.png]
                                    [--plot-3d pareto-3d.png]
"""

import argparse
import csv
import sys
from collections import defaultdict


def load_sweep(path):
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f)
        for r in reader:
            # sci2Bler column was added for joint-target analyses; older CSVs
            # without it are treated as sci2Bler=0 so the tb-only default path
            # is bit-for-bit equivalent to the original behavior.
            sci2 = float(r["sci2Bler"]) if "sci2Bler" in r and r["sci2Bler"] else 0.0
            rows.append(
                {
                    "snr": float(r["widebandSnrDb"]),
                    "tb": int(r["tbSizeBytes"]),
                    "mcs": int(r["mcs"]),
                    "nsc": int(r["nSubCh"]),
                    "numTx": int(r["numTx"]),
                    "bler": float(r["effectiveBler"]),
                    "sci2": sci2,
                    "cap": bool(int(r["capacityOk"])),
                }
            )
    return rows


def effective_bler(entry, target_model):
    """Return the BLER to compare against the target, per target_model.

    tb    -> TB-decode BLER only (post-numTx IR-combined), matches the
             original Study A / Study B analysis.
    joint -> P_total = 1 - (1 - sci2Bler) * (1 - tbBler). Treats SCI-2a
             as a single end-of-chain decode event composed with the
             IR-combined TB BLER. Exact for numTx=1 (the FINDINGS §7
             formula); a first-pass approximation for numTx>1 that does
             not track per-transmission SCI-2a outcomes.
    """
    if target_model == "joint":
        return 1.0 - (1.0 - entry["sci2"]) * (1.0 - entry["bler"])
    return entry["bler"]


def group_by_snr_tb(rows):
    g = defaultdict(list)
    for r in rows:
        g[(r["snr"], r["tb"])].append(r)
    return g


def max_num_tx(rows):
    return max(r["numTx"] for r in rows)


# ---------- Study A: 2-D vs 1-D at numTx=1 only ----------


def olla_1d_pick(entries, target, target_model):
    """Current OLLA: for each MCS, min-capacity N_sc at numTx=1 (first
    transmission), take max MCS whose first-tx BLER <= target."""
    per_mcs = defaultdict(list)
    for e in entries:
        if e["cap"] and e["numTx"] == 1:
            per_mcs[e["mcs"]].append(e)
    best = None
    for mcs, es in per_mcs.items():
        e_min_nsc = min(es, key=lambda x: x["nsc"])
        if effective_bler(e_min_nsc, target_model) <= target:
            if best is None or mcs > best["mcs"]:
                best = e_min_nsc
    return best


def pareto_2d_pick(entries, target, target_model):
    """Smallest N_sc at numTx=1 with any capacity-feasible MCS passing
    target; tie-break by max MCS."""
    per_nsc = defaultdict(list)
    for e in entries:
        if e["cap"] and e["numTx"] == 1 and effective_bler(e, target_model) <= target:
            per_nsc[e["nsc"]].append(e)
    if not per_nsc:
        return None
    min_nsc = min(per_nsc.keys())
    return max(per_nsc[min_nsc], key=lambda x: x["mcs"])


def monotonicity_violations(entries, target_model):
    """Count adjacent-MCS BLER inversions at numTx=1 across fixed nSc."""
    per_nsc = defaultdict(list)
    for e in entries:
        if e["cap"] and e["numTx"] == 1:
            per_nsc[e["nsc"]].append(e)
    violations = 0
    for _nsc, es in per_nsc.items():
        es_sorted = sorted(es, key=lambda x: x["mcs"])
        for i in range(len(es_sorted) - 1):
            if effective_bler(es_sorted[i + 1], target_model) < effective_bler(
                es_sorted[i], target_model
            ):
                violations += 1
    return violations


def analyze_target_2d(groups, target, target_model):
    out = {
        "target": target,
        "total": 0,
        "feasible_1d": 0,
        "feasible_2d_only": 0,
        "strict_nsc_win": 0,
        "tie_nsc_higher_mcs": 0,
        "tie_identical": 0,
        "mono_violations": 0,
    }
    for entries in groups.values():
        out["total"] += 1
        out["mono_violations"] += monotonicity_violations(entries, target_model)
        one = olla_1d_pick(entries, target, target_model)
        two = pareto_2d_pick(entries, target, target_model)
        if one is None and two is None:
            continue
        if one is None:
            out["feasible_2d_only"] += 1
            continue
        out["feasible_1d"] += 1
        if two is None:
            continue
        d_nsc = one["nsc"] - two["nsc"]
        d_mcs = two["mcs"] - one["mcs"]
        if d_nsc > 0:
            out["strict_nsc_win"] += 1
        elif d_mcs > 0:
            out["tie_nsc_higher_mcs"] += 1
        else:
            out["tie_identical"] += 1
    return out


# ---------- Study B: 3-D (MCS, N_sc, numTx) vs current OLLA ----------


def current_olla_pick(entries, target, max_num_tx_pool, target_model):
    """Current OLLA (HARQ-unaware): picks on first-tx BLER, but the pool
    reserves max_num_tx_pool * N_sc. Returns (entry_at_numTx=1, resource)
    or (None, None)."""
    pick = olla_1d_pick(entries, target, target_model)
    if pick is None:
        return None, None
    return pick, pick["nsc"] * max_num_tx_pool


def fixed_harq_pick(entries, target, max_num_tx_pool, target_model):
    """HARQ-aware but fixed numTx: max MCS whose post-max-tx BLER at
    min-capacity N_sc passes target. Resource = N_sc * max_num_tx_pool."""
    per_mcs = defaultdict(list)
    for e in entries:
        if e["cap"] and e["numTx"] == max_num_tx_pool:
            per_mcs[e["mcs"]].append(e)
    best = None
    for mcs, es in per_mcs.items():
        e_min_nsc = min(es, key=lambda x: x["nsc"])
        if effective_bler(e_min_nsc, target_model) <= target:
            if best is None or mcs > best["mcs"]:
                best = e_min_nsc
    if best is None:
        return None, None
    return best, best["nsc"] * max_num_tx_pool


def optimizing_pick(entries, target, max_num_tx_pool, target_model):
    """argmin (N_sc * numTx) over all (mcs, nSc, numTx) capacity-feasible
    triples whose post-numTx effective BLER <= target. Tie-break in
    order: smaller numTx, larger MCS, smaller N_sc."""
    candidates = []
    for e in entries:
        if (
            e["cap"]
            and 1 <= e["numTx"] <= max_num_tx_pool
            and effective_bler(e, target_model) <= target
        ):
            candidates.append(e)
    if not candidates:
        return None, None
    best = None
    best_cost = None
    for c in candidates:
        cost = c["nsc"] * c["numTx"]
        key = (cost, c["numTx"], -c["mcs"], c["nsc"])
        if best is None or key < best_cost:
            best = c
            best_cost = key
    return best, best["nsc"] * best["numTx"]


def analyze_target_3d(groups, target, max_num_tx_pool, target_model):
    out = {
        "target": target,
        "total": 0,
        "current_feasible": 0,
        "fixed_feasible": 0,
        "opt_feasible": 0,
        "all_three_feasible": 0,
        "opt_numTx_lt_max": 0,
        "opt_numTx_hist": defaultdict(int),
        "sum_resource_current": 0,
        "sum_resource_fixed": 0,
        "sum_resource_opt": 0,
        "sum_savings_vs_current": 0.0,
        "sum_savings_vs_fixed": 0.0,
    }
    for entries in groups.values():
        out["total"] += 1
        cur, r_cur = current_olla_pick(entries, target, max_num_tx_pool, target_model)
        fix, r_fix = fixed_harq_pick(entries, target, max_num_tx_pool, target_model)
        opt, r_opt = optimizing_pick(entries, target, max_num_tx_pool, target_model)
        if cur is not None:
            out["current_feasible"] += 1
        if fix is not None:
            out["fixed_feasible"] += 1
        if opt is not None:
            out["opt_feasible"] += 1
            out["opt_numTx_hist"][opt["numTx"]] += 1
            if opt["numTx"] < max_num_tx_pool:
                out["opt_numTx_lt_max"] += 1
        if cur is not None and fix is not None and opt is not None:
            out["all_three_feasible"] += 1
            out["sum_resource_current"] += r_cur
            out["sum_resource_fixed"] += r_fix
            out["sum_resource_opt"] += r_opt
            out["sum_savings_vs_current"] += (r_cur - r_opt) / r_cur
            out["sum_savings_vs_fixed"] += (r_fix - r_opt) / r_fix
    return out


# ---------- Reporting ----------


def write_summary_csv(rows_2d, rows_3d, path):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["study", "targetBler", "key", "value"])
        for r in rows_2d:
            w.writerow(["2D", f"{r['target']:.4f}", "total", r["total"]])
            w.writerow(["2D", f"{r['target']:.4f}", "feasible1D", r["feasible_1d"]])
            w.writerow(["2D", f"{r['target']:.4f}", "feasible2DOnly", r["feasible_2d_only"]])
            w.writerow(["2D", f"{r['target']:.4f}", "nscWin2D", r["strict_nsc_win"]])
            w.writerow(["2D", f"{r['target']:.4f}", "sameNscHigherMcs2D", r["tie_nsc_higher_mcs"]])
            w.writerow(["2D", f"{r['target']:.4f}", "identical", r["tie_identical"]])
            w.writerow(["2D", f"{r['target']:.4f}", "monoViolations", r["mono_violations"]])
        for r in rows_3d:
            w.writerow(["3D", f"{r['target']:.4f}", "total", r["total"]])
            w.writerow(["3D", f"{r['target']:.4f}", "currentFeasible", r["current_feasible"]])
            w.writerow(["3D", f"{r['target']:.4f}", "fixedFeasible", r["fixed_feasible"]])
            w.writerow(["3D", f"{r['target']:.4f}", "optFeasible", r["opt_feasible"]])
            w.writerow(["3D", f"{r['target']:.4f}", "allThreeFeasible", r["all_three_feasible"]])
            w.writerow(["3D", f"{r['target']:.4f}", "optNumTxLtMax", r["opt_numTx_lt_max"]])
            for ntx, cnt in sorted(r["opt_numTx_hist"].items()):
                w.writerow(["3D", f"{r['target']:.4f}", f"optNumTxEq{ntx}", cnt])
            if r["all_three_feasible"] > 0:
                n = r["all_three_feasible"]
                w.writerow(
                    [
                        "3D",
                        f"{r['target']:.4f}",
                        "meanResourceCurrent",
                        f"{r['sum_resource_current'] / n:.3f}",
                    ]
                )
                w.writerow(
                    [
                        "3D",
                        f"{r['target']:.4f}",
                        "meanResourceFixed",
                        f"{r['sum_resource_fixed'] / n:.3f}",
                    ]
                )
                w.writerow(
                    [
                        "3D",
                        f"{r['target']:.4f}",
                        "meanResourceOpt",
                        f"{r['sum_resource_opt'] / n:.3f}",
                    ]
                )
                w.writerow(
                    [
                        "3D",
                        f"{r['target']:.4f}",
                        "meanSavingsVsCurrent",
                        f"{r['sum_savings_vs_current'] / n:.4f}",
                    ]
                )
                w.writerow(
                    [
                        "3D",
                        f"{r['target']:.4f}",
                        "meanSavingsVsFixed",
                        f"{r['sum_savings_vs_fixed'] / n:.4f}",
                    ]
                )


def print_2d_table(rows):
    print("== Study A: 2-D vs 1-D controller (numTx = 1) ==")
    print(
        f"{'target':>8}  {'total':>5}  {'1D-fea':>6}  {'2D-only':>7}  "
        f"{'nsc-win':>7}  {'mcs-win':>7}  {'ident':>5}  {'mono':>5}"
    )
    for r in rows:
        print(
            f"{r['target']:>8.4f}  {r['total']:>5}  {r['feasible_1d']:>6}  "
            f"{r['feasible_2d_only']:>7}  {r['strict_nsc_win']:>7}  "
            f"{r['tie_nsc_higher_mcs']:>7}  {r['tie_identical']:>5}  "
            f"{r['mono_violations']:>5}"
        )


def print_3d_table(rows, max_num_tx_pool):
    print(
        f"\n== Study B: 3-D (MCS, N_sc, numTx) vs current OLLA "
        f"(maxNumTx = {max_num_tx_pool}) =="
    )
    print(
        f"{'target':>8}  {'cur-fea':>7}  {'fix-fea':>7}  {'opt-fea':>7}  "
        f"{'all-fea':>7}  {'<maxTx':>7}  {'R_cur':>7}  {'R_fix':>7}  "
        f"{'R_opt':>7}  {'%savCur':>8}  {'%savFix':>8}"
    )
    for r in rows:
        n = r["all_three_feasible"]
        if n == 0:
            print(f"{r['target']:>8.4f}  (none feasible)")
            continue
        rc = r["sum_resource_current"] / n
        rf = r["sum_resource_fixed"] / n
        ro = r["sum_resource_opt"] / n
        sc = 100.0 * r["sum_savings_vs_current"] / n
        sf = 100.0 * r["sum_savings_vs_fixed"] / n
        print(
            f"{r['target']:>8.4f}  {r['current_feasible']:>7}  "
            f"{r['fixed_feasible']:>7}  {r['opt_feasible']:>7}  "
            f"{n:>7}  {r['opt_numTx_lt_max']:>7}  "
            f"{rc:>7.2f}  {rf:>7.2f}  {ro:>7.2f}  "
            f"{sc:>8.2f}  {sf:>8.2f}"
        )
        # NumTx histogram line
        hist = r["opt_numTx_hist"]
        parts = [f"numTx={k}: {v}" for k, v in sorted(hist.items())]
        print(f"            optimizer picks  " + "  ".join(parts))


def plot_2d_summary(rows_2d, path):
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import numpy as np

    targets = [r["target"] for r in rows_2d]
    x = np.arange(len(targets))
    ident = np.array([r["tie_identical"] for r in rows_2d])
    cov = np.array([r["feasible_2d_only"] for r in rows_2d])
    nsc_w = np.array([r["strict_nsc_win"] for r in rows_2d])
    mcs_w = np.array([r["tie_nsc_higher_mcs"] for r in rows_2d])
    feas = np.array([r["feasible_1d"] for r in rows_2d])

    fig, ax = plt.subplots(figsize=(7.0, 4.0))
    width = 0.6
    ax.bar(x, ident, width, label="1-D == 2-D (identical pick)", color="#9ecae1", edgecolor="black")
    ax.bar(
        x,
        cov,
        width,
        bottom=ident,
        label="2-D feasible only (coverage gain)",
        color="#fdae6b",
        edgecolor="black",
    )
    ax.bar(
        x,
        nsc_w,
        width,
        bottom=ident + cov,
        label="2-D uses smaller N_sc",
        color="#e6550d",
        edgecolor="black",
    )
    ax.bar(
        x,
        mcs_w,
        width,
        bottom=ident + cov + nsc_w,
        label="2-D picks higher MCS at same N_sc",
        color="#31a354",
        edgecolor="black",
    )

    ax.set_xticks(x)
    ax.set_xticklabels([f"{t:g}" for t in targets])
    ax.set_xlabel("Target BLER")
    ax.set_ylabel("Number of (SNR, TB-size) operating points")
    ax.set_title("Study A: 2-D controller advantage over 1-D, across target BLER")
    ax.set_ylim(0, 700)
    ax.grid(axis="y", alpha=0.3)
    ax.legend(loc="upper left", fontsize=8)

    for i, r in enumerate(rows_2d):
        adv = r["feasible_2d_only"] + r["strict_nsc_win"] + r["tie_nsc_higher_mcs"]
        ax.annotate(
            f"advantage\n= {adv}/{r['feasible_1d'] or 1}",
            xy=(i, feas[i]),
            xytext=(0, 4),
            textcoords="offset points",
            ha="center",
            fontsize=7,
            color="#555",
        )

    fig.tight_layout()
    fig.savefig(path, dpi=150)


def plot_3d_summary(rows_3d, path, max_num_tx_pool):
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import numpy as np

    targets = [r["target"] for r in rows_3d]
    x = np.arange(len(targets))

    r_cur = []
    r_fix = []
    r_opt = []
    sav_cur_pct = []
    n_feas = []
    for r in rows_3d:
        n = r["all_three_feasible"]
        n_feas.append(n)
        if n == 0:
            r_cur.append(0.0)
            r_fix.append(0.0)
            r_opt.append(0.0)
            sav_cur_pct.append(0.0)
        else:
            r_cur.append(r["sum_resource_current"] / n)
            r_fix.append(r["sum_resource_fixed"] / n)
            r_opt.append(r["sum_resource_opt"] / n)
            sav_cur_pct.append(100.0 * r["sum_savings_vs_current"] / n)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11.5, 4.2))

    # Panel 1: mean resource cost per strategy
    width = 0.26
    ax1.bar(
        x - width,
        r_cur,
        width,
        label="Current OLLA (first-tx target)",
        color="#9ecae1",
        edgecolor="black",
    )
    ax1.bar(
        x,
        r_fix,
        width,
        label=f"Fixed HARQ (post-{max_num_tx_pool}-tx target)",
        color="#fdae6b",
        edgecolor="black",
    )
    ax1.bar(
        x + width,
        r_opt,
        width,
        label="Optimizing (MCS, N_sc, numTx)",
        color="#31a354",
        edgecolor="black",
    )
    ax1.set_xticks(x)
    ax1.set_xticklabels([f"{t:g}" for t in targets])
    ax1.set_xlabel("Target BLER (effective, post-HARQ)")
    ax1.set_ylabel("Mean announced reservation (subchannel-slots)")
    ax1.set_title(f"Average resource consumption (maximum numTx = {max_num_tx_pool})")
    ax1.grid(axis="y", alpha=0.3)
    ax1.legend(fontsize=8, loc="upper right")
    for i, (a, b, c, n) in enumerate(zip(r_cur, r_fix, r_opt, n_feas)):
        ax1.annotate(
            f"n={n}",
            xy=(i, max(a, b, c)),
            xytext=(0, 4),
            textcoords="offset points",
            ha="center",
            fontsize=7,
            color="#555",
        )

    # Panel 2: numTx histogram for the optimizer
    colors = {1: "#2b8cbe", 2: "#fdae6b", 3: "#31a354", 4: "#756bb1", 5: "#e6550d"}
    bottoms = np.zeros(len(targets))
    for ntx in range(1, max_num_tx_pool + 1):
        heights = np.array(
            [100.0 * r["opt_numTx_hist"].get(ntx, 0) / max(r["opt_feasible"], 1) for r in rows_3d]
        )
        ax2.bar(
            x,
            heights,
            0.6,
            bottom=bottoms,
            label=f"numTx = {ntx}",
            color=colors.get(ntx, "#777"),
            edgecolor="black",
        )
        bottoms += heights
    ax2.set_xticks(x)
    ax2.set_xticklabels([f"{t:g}" for t in targets])
    ax2.set_xlabel("Target BLER")
    ax2.set_ylabel("Fraction of feasible (SNR, TB) points (%)")
    ax2.set_title("Frequency of selection of different numTx")
    ax2.set_ylim(0, 105)
    ax2.grid(axis="y", alpha=0.3)
    ax2.legend(fontsize=8, loc="upper right")

    fig.tight_layout()
    fig.savefig(path, dpi=150)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default="mcs-pareto-sweep.csv")
    ap.add_argument(
        "--target",
        default="0.001,0.01,0.03,0.05,0.10",
        help="Comma-separated list of target BLER values",
    )
    ap.add_argument("--summary-csv", default=None)
    ap.add_argument("--plot-2d", default=None)
    ap.add_argument("--plot-3d", default=None)
    ap.add_argument(
        "--max-num-tx",
        type=int,
        default=None,
        help="Override maxNumTx; defaults to max numTx present in the CSV.",
    )
    ap.add_argument(
        "--target-model",
        choices=["tb", "joint"],
        default="tb",
        help="tb: target against TB-decode BLER only (original behavior). "
        "joint: target against P_total = 1 - (1 - sci2Bler) * (1 - tbBler), "
        "incorporating SCI-2a decode failures per FINDINGS Sec. 7.",
    )
    args = ap.parse_args()

    rows = load_sweep(args.csv)
    groups = group_by_snr_tb(rows)
    max_tx = args.max_num_tx if args.max_num_tx else max_num_tx(rows)

    if args.target_model == "joint" and not any(r["sci2"] > 0 for r in rows):
        print(
            "warning: --target-model=joint requested but sci2Bler column is "
            "absent or all zero; results will match --target-model=tb.",
            file=sys.stderr,
        )

    targets = [float(t) for t in args.target.split(",") if t]

    rows_2d = [analyze_target_2d(groups, t, args.target_model) for t in targets]
    rows_3d = [analyze_target_3d(groups, t, max_tx, args.target_model) for t in targets]

    print_2d_table(rows_2d)
    print_3d_table(rows_3d, max_tx)

    if args.summary_csv:
        write_summary_csv(rows_2d, rows_3d, args.summary_csv)
        print(f"\nWrote {args.summary_csv}")
    if args.plot_2d:
        plot_2d_summary(rows_2d, args.plot_2d)
        print(f"Wrote {args.plot_2d}")
    if args.plot_3d:
        plot_3d_summary(rows_3d, args.plot_3d, max_tx)
        print(f"Wrote {args.plot_3d}")


if __name__ == "__main__":
    main()
