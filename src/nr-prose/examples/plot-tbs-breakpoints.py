#!/usr/bin/python3

"""
Plot minimum subchannels required vs MCS for one or more TB sizes.

Reads CSV produced by the `tbs-breakpoints` scratch program in CSV mode,
e.g.:

    ./ns3 run "tbs-breakpoints --targetBytes=100,500,1000 --csv=true" \
        > tbs-breakpoints.csv

The CSV columns are: tb,mcs,minPsfch,minNoPsfch. Empty minPsfch /
minNoPsfch cells indicate that the (tb, mcs) combination is infeasible
within the pool's maxSubchannels; those points are omitted from the plot
(leaving a visible gap at low MCS for the largest TB sizes).

Usage:
    python3 plot-tbs-breakpoints.py [--csv tbs-breakpoints.csv]
                                    [--out tbs-breakpoints]
                                    [--slot psfch|no-psfch]
                                    [--maxSubchannels 5]
"""

import argparse
import csv
import math
import os
import sys
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def read_breakpoints(path, column):
    per_tb = defaultdict(dict)
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        required = {"tb", "mcs", column}
        missing = required - set(reader.fieldnames or [])
        if missing:
            raise SystemExit(
                f"{path}: missing required column(s): {sorted(missing)}; "
                f"found: {reader.fieldnames}"
            )
        for row in reader:
            tb = int(row["tb"])
            mcs = int(row["mcs"])
            raw = row[column].strip()
            per_tb[tb][mcs] = int(raw) if raw else math.nan
    return per_tb


def plot(per_tb, column, max_subchannels, out_stem):
    fig, ax = plt.subplots(figsize=(6.0, 3.6))

    tb_sizes = sorted(per_tb.keys())
    colors = plt.get_cmap("tab10").colors
    markers = ["o", "s", "D", "^", "v", "P", "X"]

    mcs_range = list(range(0, 29))
    for idx, tb in enumerate(tb_sizes):
        subch = [per_tb[tb].get(m, math.nan) for m in mcs_range]
        ax.plot(
            subch,
            mcs_range,
            linestyle="-",
            marker=markers[idx % len(markers)],
            color=colors[idx % len(colors)],
            linewidth=1.4,
            markersize=6,
            alpha=0.9,
            label=f"TB = {tb} B",
        )

 #   ax.axvline(
 #       max_subchannels,
 #       color="gray",
 #       linestyle=":",
 #       linewidth=1.0,
 #       label=f"pool max ({max_subchannels} subch)",
 #   )

    ax.set_xlabel("Minimum subchannels required")
    ax.set_ylabel("MCS index")
    slot_label = {"minPsfch": "PSFCH slot", "minNoPsfch": "non-PSFCH slot"}[column]
    ax.set_title(f"MCS vs minimum subchannels required, by TB size ({slot_label})")
    ax.set_xlim(0, 6)
    ax.set_ylim(-0.5, 28.5)
    ax.set_xticks(range(0, 7))
    ax.set_yticks(range(0, 29, 4))
    ax.grid(True, axis="both", alpha=0.3)
    ax.legend(loc="lower left", framealpha=0.9)

    fig.tight_layout()
    for ext in ("svg", "png"):
        path = f"{out_stem}.{ext}"
        fig.savefig(path, dpi=150)
        print(f"wrote {path}")
    plt.close(fig)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--csv", default="tbs-breakpoints.csv", help="input CSV from tbs-breakpoints")
    ap.add_argument("--out", default="tbs-breakpoints", help="output filename stem (no extension)")
    ap.add_argument(
        "--slot",
        choices=["psfch", "no-psfch"],
        default="psfch",
        help="which slot type to plot (default: psfch)",
    )
    ap.add_argument(
        "--maxSubchannels",
        type=int,
        default=5,
        help="pool max subchannels (annotated as a horizontal reference line)",
    )
    args = ap.parse_args()

    if not os.path.exists(args.csv):
        print(f"{args.csv}: not found", file=sys.stderr)
        sys.exit(1)

    column = "minPsfch" if args.slot == "psfch" else "minNoPsfch"
    per_tb = read_breakpoints(args.csv, column)
    if not per_tb:
        print(f"{args.csv}: no rows found", file=sys.stderr)
        sys.exit(1)
    plot(per_tb, column, args.maxSubchannels, args.out)


if __name__ == "__main__":
    main()
