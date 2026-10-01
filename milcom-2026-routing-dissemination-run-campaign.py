#!/usr/bin/env python3
"""Run and process the simulation campaigns for the paper "Routing Control 
Dissemination for 5G NR Multi-Hop UE-to-UE Relays" accepted to MILCOM 2026 
conference.

This script builds and runs the milcom-2026-routing-dissemination ns-3
scenario, aggregates the per-run statistics, and creates the figures used in
the paper.

Campaigns
---------
1. Connection-range sweep (default)
   Evaluates Ideal, Unicast, and Groupcast routing-message dissemination at
   SD-RSRP thresholds corresponding to connection ranges of 200, 300, 400,
   500, and 600 m. This produces 15 evaluations.

2. Application-count sweep
   Evaluates Ideal, Unicast, and Groupcast routing-message dissemination with
   1 through 10 VoIP applications. This produces 30 evaluations and simulation
   time scales with application load, making this campaign time-intensive.

For each evaluation, the script runs RngRun=1 through RngRun=N, where N is
selected with the parameter --runs. Runs execute concurrently. The --jobs 
parameter sets the maximum number of simultaneous simulations.

The results reported in the paper were generated with 5,000 runs per
evaluation. A full campaign at that size is time- and resource-intensive, so
this demonstration script defaults to 10 runs per evaluation. Use
--runs 5000 to reproduce the paper's run count when sufficient computing
resources and time are available.

Usage
-----
Run from the ns-3 root:

    python3 milcom-2026-routing-dissemination-run-campaign.py
    python3 milcom-2026-routing-dissemination-run-campaign.py --campaign 1 --runs 10
    python3 milcom-2026-routing-dissemination-run-campaign.py --campaign 2 --runs 10

Output
------
Results are written under:

    milcom-2026-routing-dissemination-outputs/
        connection-range-sweep/   # Campaign 1
        nApp-sweep/               # Campaign 2

Each evaluation directory contains one RunN directory per simulation and the
evaluation-level CSV summaries. The top level of each campaign directory also
contains the campaign-level CSV summaries and plots on the paper.
Each plot and its source CSV share the paper figure name:

* Fig3a-Fig3f for the six connection-range results.
* Fig4a-Fig4b for the two application-count results.

Existing run directories are never overwritten; remove the applicable
campaign output before rerunning it.

Requirements
------------
* A configured ns-3 tree with examples and the nr-prose module enabled.
* Python 3 with matplotlib, NumPy, pandas, and SciPy.

The script must reside in the ns-3 root because it invokes the local ns3
launcher to check the configuration, build the scenario, and run simulations.
"""

import argparse
import csv
import math
import os
from pathlib import Path
import re
import shlex
import subprocess
from concurrent.futures import ThreadPoolExecutor

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.patches import Patch
from scipy import stats

ROOT = Path(__file__).resolve().parent
SCENARIO = "milcom-2026-routing-dissemination"
CHANNEL_METRICS = [
    "chUsageApp",
    "chUsageDisc",
    "chUsagePc5S",
    "chUsageNhdp",
    "chUsageOlsr",
]
LOSS_METRICS = ["nRxDataCorr", "nRxDataHd", "nRxDataNotExp"]


def mean_ci(series):
    data = series.dropna().astype(float)
    if data.empty:
        return np.nan, np.nan
    return float(data.mean()), (float(stats.sem(data) * stats.t.ppf(0.975, len(data)-1))
                               if len(data) > 1 else 0.0)


def read_runs(source, runs, filename, columns):
    frames = []
    for run in runs:
        path = source / f"Run{run}" / filename
        frame = pd.read_csv(path)
        frame.columns = frame.columns.str.lstrip("#")
        missing = set(columns) - set(frame.columns)
        if missing:
            raise ValueError(f"{path}: missing columns {sorted(missing)}")
        if frame.empty:
            raise ValueError(f"{path}: no simulation statistics")
        frames.append(frame[columns])
    return pd.concat(frames, ignore_index=True)


def process_evaluation(source, destination, runs, campaign, hop_mos):
    destination.mkdir(parents=True, exist_ok=True)
    metrics = ["meanMos"] if campaign == 1 else ["meanMos", "nAppOutageMos"]
    voip = read_runs(source, runs, "voip-stats-sim.csv",
                     ["RngSeed", "RngRun", "meanLossRatio", *metrics])
    valid = voip[~np.isclose(voip["meanLossRatio"], 1.0)]
    rows = []
    for metric in metrics:
        mean, ci = mean_ci(valid[metric])
        rows.append({"metricName": metric, "Mean": mean, "95%CI": ci})
    pd.DataFrame(rows).to_csv(destination / "voip-stats-sim-all-mean-95-ci.csv", index=False)
    if campaign == 2:
        return

    route = read_runs(source, runs, "route-change-stats.csv", ["RngSeed", "RngRun", "avgNhops"])
    route.to_csv(destination / "route-change-stats-all.csv", index=False)
    if hop_mos:
        merged = voip.merge(route, on=["RngSeed", "RngRun"], validate="one_to_one")
        merged = merged.dropna(subset=["avgNhops"])
        merged["hopBinLabel"] = merged["avgNhops"].map(
            lambda x: "<1" if x < 1 else f"{math.floor(x)} to {math.floor(x)+1}")
        rows = []
        for label, group in merged.groupby("hopBinLabel"):
            mean, ci = mean_ci(group["meanMos"].replace([np.inf, -np.inf], np.nan))
            rows.append({"hopMetricName": f"avgNhopsBin={label}", "voipMetricName": "meanMos",
                         "Mean": mean, "95%CI": ci})
        pd.DataFrame(rows, columns=["hopMetricName", "voipMetricName", "Mean", "95%CI"]).to_csv(
            destination / "voip-stats-sim-all-per-hops-mean-95-ci.csv", index=False)

    phy = read_runs(source, runs, "phy-stats-sim-percent.csv", ["Type", *LOSS_METRICS])
    phy = phy[phy["Type"].str.lower() == "app"]
    if phy.empty:
        raise ValueError(f"{source}: no App PHY statistics")
    pd.DataFrame([{"Type": "app", "Metric": metric, "Mean": phy[metric].mean()}
                  for metric in LOSS_METRICS]).to_csv(
        destination / "phy-stats-sim-percent-mean.csv", index=False)
    channel = read_runs(source, runs, "phy-tx-stats-sim.csv", CHANNEL_METRICS)
    pd.DataFrame([{"metricName": metric, "Mean": channel[metric].mean()}
                  for metric in CHANNEL_METRICS]).to_csv(
        destination / "phy-tx-stats-sim-all-mean.csv", index=False)
    neighbors = read_runs(source, runs, "neighbor-degree-stats-sim.csv", ["MeanAvgNeighbDegree"])
    mean, ci = mean_ci(neighbors["MeanAvgNeighbDegree"])
    pd.DataFrame([{"metricName": "MeanAvgNeighbDegree", "Mean": mean, "95%CI": ci}]).to_csv(
        destination / "neighbor-degree-stats-sim-mean-95-ci.csv", index=False)


def extract_outer_params(folder_name, outer_loop_param):
    if isinstance(outer_loop_param, list):
        values = []
        for param in outer_loop_param:
            pattern = rf"(?:^|_){param}-(.*?)(?=_[a-zA-Z]+-|$)"
            matches = re.findall(pattern, folder_name)
            if matches:
                values.append(matches[-1])
            else:
                print(f"Could not find parameter '{param}' in folder name '{folder_name}'")
                values.append(None)
        return tuple(values)
    else:
        pattern = rf"(?:^|_){outer_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches = re.findall(pattern, folder_name)
        return matches[-1] if matches else None


def extract_inner_params(folder_name, inner_loop_param):
    if isinstance(inner_loop_param, list):
        values = []
        for param in inner_loop_param:
            pattern = rf"(?:^|_){param}-(.*?)(?=_[a-zA-Z]+-|$)"
            matches = re.findall(pattern, folder_name)
            if matches:
                values.append(matches[-1])
            else:
                print(f"Could not find parameter '{param}' in folder name '{folder_name}'")
                values.append(None)
        return tuple(values)
    else:
        pattern = rf"(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches = re.findall(pattern, folder_name)
        return matches[-1] if matches else None


def format_loop_label(loop_param, extracted):
    if isinstance(loop_param, list):
        return ", ".join([f"{p}={v}" for p, v in zip(loop_param, extracted)])
    else:
        return f"{loop_param}={extracted}"


def get_axis_display_label(loop_param, axis_label_map=None):
    if axis_label_map is None:
        return loop_param if not isinstance(loop_param, list) else ", ".join(loop_param)

    key = tuple(loop_param) if isinstance(loop_param, list) else loop_param
    default_label = loop_param if not isinstance(loop_param, list) else ", ".join(loop_param)
    return axis_label_map.get(key, default_label)


def normalize_value_label_map(value_label_map):
    if value_label_map is None:
        return None

    flat_map = {}
    for k, v in value_label_map.items():
        if isinstance(v, dict) and (isinstance(k, (list, tuple)) or (isinstance(k, str) and "=" not in k)):
            loop_param = list(k) if isinstance(k, tuple) else k
            for raw_value, display_value in v.items():
                flat_map[format_loop_label(loop_param, raw_value)] = display_value
        else:
            flat_map[k] = v
    return flat_map


def get_value_display_label(loop_label, value_label_map=None):
    if value_label_map is None:
        return loop_label
    return value_label_map.get(loop_label, loop_label)



def process_campaign(
    campaignName,
    outer_loop_param,
    outer_loop_values,
    inner_loop_param,
    inner_loop_values,
    evalFolders,
    axis_label_map=None,
    value_label_map=None,
):

    if inner_loop_param == "rsrpThreshold":
        figure_names = {
            "neighborDegree": "Fig3a",
            "hopDistribution": "Fig3b",
            "channelUsage": "Fig3c",
            "meanMos": "Fig3d",
            "phyLoss": "Fig3e",
            "meanMosByNhops": "Fig3f",
        }
    elif inner_loop_param == "nApps":
        figure_names = {
            "meanMos": "Fig4a",
            "nAppOutageMos": "Fig4b",
        }
    value_label_map = normalize_value_label_map(value_label_map)
    metrics_data = {}
    inner_labels_in_order = [format_loop_label(inner_loop_param, iv) for iv in inner_loop_values]
    # -------------------------------------------------------------------------
    # Part 1: Process voip-stats-sim-all-mean-95-ci.csv
    # -------------------------------------------------------------------------
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)
        # Extract outer and inner parameter(s) and compute composite labels
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)

        if outer_extracted is None or inner_extracted is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue


        metrics_file = os.path.join(evalFolder, "voip-stats-sim-all-mean-95-ci.csv")
        if not os.path.isfile(metrics_file):
            print(f"Metrics file not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)
        for _, row in df.iterrows():
            metric = row["metricName"]
            if metric not in metrics_data:
                metrics_data[metric] = []
            metrics_data[metric].append(
                {
                    "outer_label": outer_label,
                    "inner_label": inner_label,
                    "inner_order": inner_labels_in_order.index(inner_label),
                    "Mean": row["Mean"],
                    "95%CI": row["95%CI"],
                }
            )

    # Plot error-bar charts for each flowsStats metric
    for metric, data in metrics_data.items():
        data_df = pd.DataFrame(data)
        figure_name = figure_names[metric]
        data_csv_file = os.path.join(campaignName, f"{figure_name}.csv")
        data_df.to_csv(data_csv_file, index=False)

        plt.figure(figsize=(3.2, 2.5))

        for ov in outer_loop_values:
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_parts = []
                for p, v in zip(outer_loop_param, ov):
                    if p == "t2":
                        display_parts.append(rf"$T_2$={v}")
                    elif p == "maxNumTx":
                        display_parts.append(rf"$n_{{\mathrm{{TX}}}}$={v}")
                    else:
                        display_parts.append(f"{p}={v}")
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = rf"$T_2$={ov}" if outer_loop_param == "t2" else get_value_display_label(label_str, value_label_map)

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by="inner_order")
            x = subset["inner_label"].map(lambda v: get_value_display_label(v, value_label_map))
            y = subset["Mean"]
            ci = subset["95%CI"]

            if len(inner_loop_values) == 1:
                # Bar plot with error bars
                plt.bar(
                    x,
                    y,
                    yerr=ci,
                    label=display_label,
                    capsize=4,
                    edgecolor="black",
                    linewidth=0.8,
                    width=0.5,
                )
            else:
                # Standard errorbar line+markers
                plt.errorbar(
                    x,
                    y,
                    yerr=ci,
                    label=display_label,
                    capsize=4,
                    linewidth=0.8,
                    marker="o",
                    markersize=2,
                )

        plt.xlabel(get_axis_display_label(inner_loop_param, axis_label_map))

        if inner_loop_param == "nApps":
            xticks = [get_value_display_label(v, value_label_map) for v in data_df.sort_values(by="inner_order")["inner_label"].unique()]
            plt.xticks(xticks)
        if metric == "meanMos":
            plt.ylabel("MOS")
            plt.ylim(3.45, 4.55)
            plt.yticks([3.5, 4, 4.5])
            plt.axhline(4, linestyle="--", color="gray")
        else:  # nAppOutageMos (campaign 2)
            plt.ylabel(r"$n_{\mathrm{APP}}$ in outage")
            plt.ylim(-0.5, 6)

        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)
        plot_file = os.path.join(campaignName, f"{figure_name}.png")
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()
    # -------------------------------------------------------------------------
    # MOS by hop count: Process voip-stats-sim-all-per-hops-mean-95-ci.csv
    #   - only avgNhops bins
    #   - MOS vs hop bins at 400m only
    # -------------------------------------------------------------------------

    if inner_loop_param == "nApps":
        return

    voip_per_hops_data = {}
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)

        if outer_extracted is None or inner_extracted is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        if get_value_display_label(inner_label, value_label_map) != "400":
            continue
        metrics_file = os.path.join(evalFolder, "voip-stats-sim-all-per-hops-mean-95-ci.csv")
        if not os.path.isfile(metrics_file):
            print(f"[WARN] {metrics_file} not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)
        df = df[df["hopMetricName"].astype(str).str.startswith("avgNhopsBin=")]
        if df.empty:
            continue

        for _, row in df.iterrows():
            hop_metric_name = str(row["hopMetricName"])
            voip_metric = row["voipMetricName"]

            hop_bin_label = hop_metric_name.split("=", 1)[1]

            if hop_bin_label == "<1":
                hop_bin_order = 0
            else:
                hop_bin_order = int(hop_bin_label.split(" to ")[0])

            voip_per_hops_data.setdefault(voip_metric, [])
            voip_per_hops_data[voip_metric].append(
                {
                    "avgNhopsBinOrder": hop_bin_order,
                    "avgNhopsBinLabel": hop_bin_label,
                    "outer_label": outer_label,
                    "inner_label": inner_label,
                    "inner_order": inner_labels_in_order.index(inner_label),
                    "Mean": float(row["Mean"]),
                    "95%CI": float(row["95%CI"]),
                }
            )
    only_integer_nhops = True

    for voip_metric, data in voip_per_hops_data.items():
        data_df = pd.DataFrame(data)
        if data_df.empty:
            continue

        output_base_name = figure_names["meanMosByNhops"]
        data_csv_file = os.path.join(
            campaignName,
            f"{output_base_name}.csv",
        )
        data_df.to_csv(data_csv_file, index=False)

        # Generate the paper figure for 400m
        for inner_val in inner_loop_values:
            inner_label = format_loop_label(inner_loop_param, inner_val)
            inner_display_label = get_value_display_label(inner_label, value_label_map)
            if inner_display_label != "400":
                continue

            sub_df = data_df[data_df["inner_label"] == inner_label].copy()
            if sub_df.empty:
                continue
            if only_integer_nhops:
                sub_df = sub_df[
                    sub_df["avgNhopsBinLabel"].astype(str).str.contains(" to ", regex=False)
                ].copy()
                if sub_df.empty:
                    continue

            # Build the x-axis ONLY from the hop bins present for THIS inner value.
            hop_bins_df = (
                sub_df[["avgNhopsBinOrder", "avgNhopsBinLabel"]]
                .drop_duplicates()
                .sort_values(by="avgNhopsBinOrder")
                .reset_index(drop=True)
            )
            hop_bin_labels = hop_bins_df["avgNhopsBinLabel"].tolist()
            hop_bin_display_labels = []
            for label in hop_bin_labels:
                if label in ["No route", "<1"]:
                    hop_bin_display_labels.append(label)
                elif " to " in label:
                    hop_bin_display_labels.append(label.split(" to ")[0])
                else:
                    hop_bin_display_labels.append(label)
            x_positions = np.arange(len(hop_bin_labels), dtype=float)

            plt.figure(figsize=(2.5, 2.8))

            for i, ov in enumerate(outer_loop_values):
                if isinstance(outer_loop_param, list):
                    outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                    outer_display_label = get_value_display_label(outer_label, value_label_map)
                else:
                    outer_label = f"{outer_loop_param}={ov}"
                    outer_display_label = (
                        rf"$T_2$={ov}"
                        if outer_loop_param == "t2"
                        else get_value_display_label(outer_label, value_label_map)
                    )

                curve_df = sub_df[sub_df["outer_label"] == outer_label].copy()
                if curve_df.empty:
                    continue

                curve_df = curve_df.sort_values(by="avgNhopsBinOrder")
                y_vals = []
                ci_vals = []
                x_vals = []

                for j, hop_label in enumerate(hop_bin_labels):
                    row_match = curve_df[curve_df["avgNhopsBinLabel"] == hop_label]
                    if row_match.empty:
                        continue

                    x_vals.append(x_positions[j])
                    y_vals.append(float(row_match.iloc[0]["Mean"]))
                    ci_vals.append(float(row_match.iloc[0]["95%CI"]))

                if len(x_vals) == 0:
                    continue

                x_vals = np.array(x_vals, dtype=float)
                y_vals = np.array(y_vals, dtype=float)
                ci_vals = np.array(ci_vals, dtype=float)

                # If there is only one hop bin, use grouped bars to avoid overlap
                if len(hop_bin_labels) == 1:
                    n_groups = len(outer_loop_values)
                    bar_width = 0.12
                    gap = 0.02
                    cluster_w = n_groups * bar_width + (n_groups - 1) * gap

                    x_bar = x_vals - cluster_w / 2 + (bar_width / 2) + i * (bar_width + gap)

                    plt.bar(
                        x_bar,
                        y_vals,
                        yerr=ci_vals,
                        width=bar_width,
                        capsize=4,
                        label=outer_display_label,
                        edgecolor="black",
                        linewidth=0.8,
                        align="center",
                    )
                else:
                    plt.errorbar(
                        x_vals,
                        y_vals,
                        yerr=ci_vals,
                        label=outer_display_label,
                        capsize=4,
                        linewidth=0.8,
                        marker="o",
                        markersize=2,
                    )

            plt.xticks(x_positions, hop_bin_display_labels)
            if len(hop_bin_labels) == 1:
                plt.xlim(-0.5, 0.5)
            else:
                plt.xlim(-0.15, len(hop_bin_labels) - 0.85)

            plt.xlabel(r"$n_{\mathrm{HOP}}$")

            plt.ylabel("MOS")
            plt.ylim(3.45, 4.55)
            plt.yticks([3.5, 4, 4.5])
            plt.axhline(4, linestyle="--", color="gray")

            plt.title(inner_display_label)
            plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
            plt.grid(True)

            plot_file = os.path.join(
                campaignName,
                f"{output_base_name}.png",
            )
            plt.savefig(plot_file, bbox_inches="tight")
            plt.close()
    simphy_percent_mean = {}
    types_seen = set()

    # -------------------------------------------------------------------------
    # Part 2: Process phy-stats-sim-percent-mean.csv
    # -------------------------------------------------------------------------
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        # Extract outer/inner loop labels from folder name
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)
        if outer_extracted is None or inner_extracted is None:
            continue

        inner_value_str = inner_label
        simPhy_percent_mean_file = os.path.join(
            evalFolder, "phy-stats-sim-percent-mean.csv"
        )
        if not os.path.isfile(simPhy_percent_mean_file):
            print(f"[WARN] {simPhy_percent_mean_file} not found in {evalFolder}")
            continue

        df_percent_mean = pd.read_csv(simPhy_percent_mean_file)

        simphy_percent_mean.setdefault(inner_value_str, {})
        simphy_percent_mean[inner_value_str].setdefault(outer_label, {})

        for _, row in df_percent_mean.iterrows():
            t = str(row["Type"])
            m = row["Metric"]
            mean_val = float(row["Mean"])
            types_seen.add(t)

            simphy_percent_mean[inner_value_str][outer_label].setdefault(t, {})
            simphy_percent_mean[inner_value_str][outer_label][t][m] = mean_val

    # Style knobs
    ylabel_fontsize = 9
    ytick_fontsize = 8
    title_fontsize = 9
    legend_fontsize = 8.2

    # Stacked metrics
    stacked_metrics = [
        "nRxDataCorr",
        "nRxDataHd",
        "nRxDataNotExp",
    ]
    alias_map = {
        "nRxDataCorr": "PSSCH error",
        "nRxDataHd": "Half duplex",
        "nRxDataNotExp": "PSCCH error",
    }
    colors = {
        "nRxDataCorr": "C0",
        "nRxDataHd": "C1",
        "nRxDataNotExp": "C2",
    }

    # Build plotting data per (Type -> inner_val -> arrays over outer labels)
    plot_data_by_type = {}
    for inner_val in inner_loop_values:
        inner_val_str = format_loop_label(inner_loop_param, inner_val)
        if inner_val_str not in simphy_percent_mean:
            continue

        outer_labels_present = list(simphy_percent_mean[inner_val_str].keys())
        if not outer_labels_present:
            continue

        for t in sorted(types_seen):
            plot_data_by_type.setdefault(t, {})
            plot_data_by_type[t].setdefault(
                inner_val_str,
                {
                    "outer_labels": outer_labels_present.copy(),
                    "data": {metric: [] for metric in stacked_metrics},
                    "aggregated": [],
                },
            )

            aggregated_list = []
            for ol in outer_labels_present:
                total = 0.0
                for metric in stacked_metrics:
                    mean_val = (
                        simphy_percent_mean[inner_val_str]
                        .get(ol, {})
                        .get(t, {})
                        .get(metric, 0.0)
                    )
                    plot_data_by_type[t][inner_val_str]["data"][metric].append(mean_val)
                    total += mean_val
                aggregated_list.append(total)

            plot_data_by_type[t][inner_val_str]["aggregated"] = aggregated_list

    csv_filename = os.path.join(
        campaignName,
        f"{figure_names['phyLoss']}.csv",
    )
    with open(csv_filename, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        header = ["Type", "Inner_Value", "Outer_Label"] + stacked_metrics
        csvwriter.writerow(header)

        for t in sorted(types_seen):
            if t not in plot_data_by_type:
                continue

            for inner_val in inner_loop_values:
                inner_val_str = format_loop_label(inner_loop_param, inner_val)
                if inner_val_str not in plot_data_by_type[t]:
                    continue

                outer_labels_present = plot_data_by_type[t][inner_val_str]["outer_labels"]
                data_dict = plot_data_by_type[t][inner_val_str]["data"]

                for i, ol in enumerate(outer_labels_present):
                    row = [t, inner_val_str, ol]
                    for metric in stacked_metrics:
                        row.append(data_dict[metric][i])
                    csvwriter.writerow(row)

    # Determine y-limit per Type across all inner values
    ymax_by_type = {}
    for t, inner_map in plot_data_by_type.items():
        local_max = 0.0
        for _, val in inner_map.items():
            if val["aggregated"]:
                local_max = max(local_max, max(val["aggregated"]))
        ymax_by_type[t] = (local_max * 1.2) if local_max > 0 else 1.0

    # Figure: one figure per Type, arranged in a 2-column grid
    unique_types = sorted([t for t in types_seen if t in plot_data_by_type])
    n_inner_val = len(inner_loop_values)

    if len(unique_types) == 0:
        print("No Types with data; skipping plot.")
    else:
        for t in unique_types:
            ncols = 2
            nrows = math.ceil(n_inner_val / ncols)

            fig, axs = plt.subplots(
                nrows,
                ncols,
                figsize=(1.6 * ncols, 1.6 * nrows + 0.5),
                squeeze=False,
            )

            mapping_text = None

            for idx, inner_val in enumerate(inner_loop_values):
                r, c = divmod(idx, ncols)
                ax = axs[r][c]

                inner_val_str = format_loop_label(inner_loop_param, inner_val)

                if inner_val_str not in plot_data_by_type[t]:
                    ax.set_visible(False)
                    continue

                outer_labels_present = plot_data_by_type[t][inner_val_str]["outer_labels"]
                data_dict = plot_data_by_type[t][inner_val_str]["data"]

                if isinstance(outer_loop_param, list) and len(outer_loop_param) > 1:
                    x_labels = []
                    for i in range(len(outer_labels_present)):
                        n = i + 1
                        s = ""
                        while n > 0:
                            n, rem = divmod(n - 1, 26)
                            s = chr(ord("A") + rem) + s
                        x_labels.append(s)

                    if mapping_text is None:
                        mapping_parts = []
                        for i, lbl in enumerate(outer_labels_present):
                            mapping_parts.append(
                                f"{x_labels[i]}) {get_value_display_label(lbl, value_label_map)}"
                            )
                        mapping_text = "    ".join(mapping_parts)
                else:
                    x_labels = [
                        get_value_display_label(label, value_label_map)
                        for label in outer_labels_present
                    ]

                x = np.arange(len(x_labels))
                bottoms = np.zeros(len(x_labels))

                for metric in stacked_metrics:
                    values = np.array(data_dict[metric])
                    ax.bar(
                        x,
                        values,
                        0.8,
                        bottom=bottoms,
                        color=colors[metric],
                        label=alias_map[metric],
                    )
                    bottoms += values

                ax.set_ylim(0, ymax_by_type.get(t, 1.0))

                if c == 0:
                    ax.set_ylabel("%", fontsize=ylabel_fontsize, labelpad=0)

                # Only inner-loop value in subplot title
                ax.set_title(
                    f"{get_axis_display_label(inner_loop_param, axis_label_map)}="
                    f"{get_value_display_label(format_loop_label(inner_loop_param, inner_val), value_label_map)}",
                    fontsize=title_fontsize,
                    pad=1,
                )

                ax.set_xticks(x)
                if isinstance(outer_loop_param, list) and len(outer_loop_param) > 1:
                    ax.set_xticklabels(x_labels, fontsize=7, rotation=0, ha="center")
                else:
                    ax.set_xticklabels(x_labels, fontsize=7, rotation=30, ha="right")

                ax.tick_params(axis="y", labelsize=ytick_fontsize)
                ax.grid(True, axis="y", alpha=0.25)

            # Hide unused subplots
            total_axes = nrows * ncols
            for idx in range(n_inner_val, total_axes):
                r, c = divmod(idx, ncols)
                axs[r][c].set_visible(False)

            # One figure title only
            fig.suptitle(f"Type={t}", fontsize=10, y=0.985)

            # Outer-set mapping in one line, BELOW the title
            if mapping_text is not None:
                fig.text(
                    0.45,
                    0.93,
                    mapping_text,
                    ha="center",
                    va="top",
                    fontsize=9,
                    family="monospace",
                )

            # Loss-cause legend
            handles = [Patch(facecolor=colors[m], label=alias_map[m]) for m in stacked_metrics]
            fig.legend(
                handles,
                [alias_map[m] for m in stacked_metrics],
                loc="lower center",
                ncol=2,
                fontsize=legend_fontsize,
                frameon=False,
                bbox_to_anchor=(0.5, -0.01),
            )

            # Tighter layout:s
            fig.subplots_adjust(
                top=0.84,
                bottom=0.16,
                wspace=0.35,
                hspace=0.40,
            )

            stacked_filename = os.path.join(
                campaignName,
                f"{figure_names['phyLoss']}.png",
            )
            plt.savefig(stacked_filename, bbox_inches="tight")
            plt.close()

    # -------------------------------------------------------------------------
    # Part 3: Process route-change-stats-all.csv
    # -------------------------------------------------------------------------
    display_avg_nhops_legend_as_integers = True
    avg_nhops_dist_rows = []
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)

        if outer_extracted is None or inner_extracted is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        route_all_file = os.path.join(evalFolder, "route-change-stats-all.csv")
        if not os.path.isfile(route_all_file):
            print(f"[WARN] {route_all_file} not found in {evalFolder}")
            continue

        df = pd.read_csv(route_all_file)
        df.columns = df.columns.str.lstrip("#")

        if "avgNhops" not in df.columns:
            print(f"[WARN] avgNhops not found in {route_all_file}")
            continue

        hop_df = df.copy()
        if hop_df.empty:
            continue

        hop_df["avgNhops"] = pd.to_numeric(hop_df["avgNhops"], errors="coerce")

        hop_df["hopBinLabel"] = hop_df["avgNhops"].apply(
            lambda x: "No route"
            if pd.isna(x)
            else ("<1" if x < 1.0 else f"{int(math.floor(x))} to {int(math.floor(x)) + 1}")
        )

        hop_df["hopBinOrder"] = hop_df["avgNhops"].apply(
            lambda x: 999 if pd.isna(x) else (0 if x < 1.0 else int(math.floor(x)))
        )

        dist_df = (
            hop_df.groupby(["hopBinOrder", "hopBinLabel"])
            .size()
            .reset_index(name="nRuns")
            .sort_values(by="hopBinOrder")
        )

        total_runs = dist_df["nRuns"].sum()
        if total_runs == 0:
            continue

        dist_df["Percentage"] = 100.0 * dist_df["nRuns"] / total_runs

        for _, row in dist_df.iterrows():
            avg_nhops_dist_rows.append(
                {
                    "outer_label": outer_label,
                    "inner_label": inner_label,
                    "inner_order": inner_labels_in_order.index(inner_label),
                    "hopBinOrder": int(row["hopBinOrder"]),
                    "hopBinLabel": row["hopBinLabel"],
                    "Percentage": float(row["Percentage"]),
                }
            )

    if avg_nhops_dist_rows:
        data_df = pd.DataFrame(avg_nhops_dist_rows)
        data_csv_file = os.path.join(
            campaignName,
            f"{figure_names['hopDistribution']}.csv",
        )
        data_df.to_csv(data_csv_file, index=False)

        hop_bins_df = (
            data_df[["hopBinOrder", "hopBinLabel"]]
            .drop_duplicates()
            .sort_values(by="hopBinOrder")
            .reset_index(drop=True)
        )
        hop_bin_labels = hop_bins_df["hopBinLabel"].tolist()

        if display_avg_nhops_legend_as_integers:
            hop_bin_display_map = {}
            for label in hop_bin_labels:
                if label == "No route" or label == "<1":
                    hop_bin_display_map[label] = label
                elif " to " in label:
                    hop_bin_display_map[label] = label.split(" to ")[0]
                else:
                    hop_bin_display_map[label] = label
        else:
            hop_bin_display_map = {label: label for label in hop_bin_labels}

        fig_width = max(4.0, 0.6 * len(inner_labels_in_order) + 2.8)
        plt.figure(figsize=(fig_width, 2.8))

        color_map = {hop_bin_labels[i]: f"C{i % 10}" for i in range(len(hop_bin_labels))}

        hatch_patterns = ["", "//", "\\\\", "xx", ".", "++"]

        x_base = np.arange(len(inner_labels_in_order), dtype=float)

        n_groups = len(outer_loop_values)
        bar_width = 0.8 / n_groups if n_groups > 0 else 0.8

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = (
                    rf"$T_2$={ov}"
                    if outer_loop_param == "t2"
                    else get_value_display_label(label_str, value_label_map)
                )

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by=["inner_order", "hopBinOrder"])

            x_positions = x_base - 0.4 + (bar_width / 2.0) + i * bar_width
            bottoms = np.zeros(len(inner_labels_in_order))

            for hop_bin_label in hop_bin_labels:
                hop_bin_sub = subset[subset["hopBinLabel"] == hop_bin_label].sort_values(
                    by="inner_order"
                )
                pct_map = {
                    row["inner_label"]: row["Percentage"]
                    for _, row in hop_bin_sub.iterrows()
                }

                values = np.array(
                    [pct_map.get(lbl, 0.0) for lbl in inner_labels_in_order],
                    dtype=float,
                )

                plt.bar(
                    x_positions,
                    values,
                    width=bar_width,
                    bottom=bottoms,
                    color=color_map[hop_bin_label],
                    edgecolor="black",
                    linewidth=0.8,
                    hatch=hatch_patterns[i % len(hatch_patterns)],
                    align="center",
                )
                bottoms += values

        x_labels = [
            get_value_display_label(lbl, value_label_map) for lbl in inner_labels_in_order
        ]
        plt.xticks(x_base, x_labels)

        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
        else:
            plt.xlabel(get_axis_display_label(inner_loop_param, axis_label_map))

        plt.ylabel("Simulations (%)")
        plt.ylim(0, 100)
        plt.grid(True, axis="y")

        # Legend for hop bins
        hop_handles = [
            Patch(
                facecolor=color_map[label],
                edgecolor="black",
                label=hop_bin_display_map[label],
            )
            for label in hop_bin_labels
        ]
        legend1 = plt.legend(
            handles=hop_handles,
            title=r"$n_{\mathrm{HOP}}$",
            fontsize=7.5,
            title_fontsize=8,
            loc="upper left",
            bbox_to_anchor=(1.02, 1.0),
            borderaxespad=0.0,
            frameon=True,
        )
        plt.gca().add_artist(legend1)

        # Legend for outer-loop values
        outer_handles = []
        outer_labels = []

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = (
                    rf"$T_2$={ov}"
                    if outer_loop_param == "t2"
                    else get_value_display_label(label_str, value_label_map)
                )

            outer_handles.append(
                Patch(
                    facecolor="white",
                    edgecolor="black",
                    hatch=hatch_patterns[i % len(hatch_patterns)],
                )
            )
            outer_labels.append(display_label)

        plt.legend(
            outer_handles,
            outer_labels,
            fontsize=7.5,
            loc="lower left",
            bbox_to_anchor=(1.02, -0.0),
            borderaxespad=0.0,
            frameon=True,
        )

        plot_file = os.path.join(
            campaignName,
            f"{figure_names['hopDistribution']}.png",
        )
        plt.tight_layout(rect=[0, 0, 0.72, 1])
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()
    else:
        print("[WARN] No avgNhops distribution data found across evaluations; skipping plot.")

    # -------------------------------------------------------------------------
    # Part 4: Process phy-tx-stats-sim-all-mean.csv
    # -------------------------------------------------------------------------
    phy_tx_metrics_data = {}
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        # Extract outer and inner parameter(s) and compute composite labels
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)

        if outer_extracted is None or inner_extracted is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        metrics_file = os.path.join(evalFolder, "phy-tx-stats-sim-all-mean.csv")
        if not os.path.isfile(metrics_file):
            print(f"[WARN] {metrics_file} not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)
        for _, row in df.iterrows():
            metric = row["metricName"]
            if metric not in CHANNEL_METRICS:
                continue

            phy_tx_metrics_data.setdefault(metric, [])
            phy_tx_metrics_data[metric].append(
                {
                    "outer_label": outer_label,
                    "inner_label": inner_label,
                    "inner_order": inner_labels_in_order.index(inner_label),
                    "Mean": float(row["Mean"]),
                }
            )

    stacked_metrics = [
        "chUsageApp",
        "chUsageDisc",
        "chUsagePc5S",
        "chUsageNhdp",
        "chUsageOlsr",
    ]

    if all(m in phy_tx_metrics_data for m in stacked_metrics):
        stacked_rows = []

        for metric in stacked_metrics:
            for row in phy_tx_metrics_data[metric]:
                stacked_rows.append(
                    {
                        "metricName": metric,
                        "outer_label": row["outer_label"],
                        "inner_label": row["inner_label"],
                        "inner_order": row["inner_order"],
                        "Mean": row["Mean"],
                    }
                )

        stacked_df = pd.DataFrame(stacked_rows)
        stacked_csv_file = os.path.join(
            campaignName,
            f"{figure_names['channelUsage']}.csv",
        )
        stacked_df.to_csv(stacked_csv_file, index=False)

        fig_width = max(4.0, 0.6 * len(inner_labels_in_order) + 2.8)
        plt.figure(figsize=(fig_width, 2.8))

        color_map = {
            "chUsageNhdp": "C0",
            "chUsageOlsr": "C1",
            "chUsageApp": "C2",
            "chUsagePc5S": "C3",
            "chUsageDisc": "C4",
        }

        label_map = {
            "chUsageNhdp": "NHDP",
            "chUsageOlsr": "OLSR",
            "chUsageApp": "App",
            "chUsagePc5S": "Pc5S",
            "chUsageDisc": "Disc",
        }

        hatch_patterns = ["", "//", "\\\\", "xx", "..", "++"]

        # x positions for inner-loop values
        x_base = np.arange(len(inner_labels_in_order), dtype=float)

        # grouped bars for outer-loop values
        n_groups = len(outer_loop_values)
        bar_width = 0.8 / n_groups if n_groups > 0 else 0.8

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = (
                    rf"$T_2$={ov}"
                    if outer_loop_param == "t2"
                    else get_value_display_label(label_str, value_label_map)
                )

            subset = stacked_df[stacked_df["outer_label"] == label_str]
            subset = subset.sort_values(by=["inner_order", "metricName"])

            x_positions = x_base - 0.4 + (bar_width / 2.0) + i * bar_width
            bottoms = np.zeros(len(inner_labels_in_order))

            for metric in stacked_metrics:
                metric_sub = subset[subset["metricName"] == metric].sort_values(by="inner_order")
                mean_map = {
                    row["inner_label"]: row["Mean"]
                    for _, row in metric_sub.iterrows()
                }

                values = np.array(
                    [mean_map.get(lbl, 0.0) for lbl in inner_labels_in_order],
                    dtype=float,
                )

                plt.bar(
                    x_positions,
                    values,
                    width=bar_width,
                    bottom=bottoms,
                    color=color_map[metric],
                    edgecolor="black",
                    linewidth=0.8,
                    hatch=hatch_patterns[i % len(hatch_patterns)],
                    align="center",
                )
                bottoms += values

        x_labels = [
            get_value_display_label(lbl, value_label_map) for lbl in inner_labels_in_order
        ]
        plt.xticks(x_base, x_labels)

        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
        else:
            plt.xlabel(get_axis_display_label(inner_loop_param, axis_label_map))

        plt.ylabel("Channel usage (%)")
        plt.ylim(bottom=0)
        plt.grid(True, axis="y")

        # Legend for stacked components
        stack_handles = [
            Patch(facecolor=color_map[m], edgecolor="black", label=label_map[m])
            for m in stacked_metrics
        ]

        legend1 = plt.legend(
            handles=stack_handles,
            title="Tx type",
            fontsize=7.5,
            title_fontsize=8,
            loc="upper left",
            bbox_to_anchor=(1.02, 1.0),
            borderaxespad=0.0,
            frameon=True,
        )
        plt.gca().add_artist(legend1)

        # Legend for outer-loop values
        outer_handles = []
        outer_labels = []

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = (
                    rf"$T_2$={ov}"
                    if outer_loop_param == "t2"
                    else get_value_display_label(label_str, value_label_map)
                )

            outer_handles.append(
                Patch(
                    facecolor="white",
                    edgecolor="black",
                    hatch=hatch_patterns[i % len(hatch_patterns)],
                )
            )
            outer_labels.append(display_label)

        plt.legend(
            outer_handles,
            outer_labels,
            title=get_axis_display_label(outer_loop_param, axis_label_map),
            fontsize=7.5,
            title_fontsize=8,
            loc="lower left",
            bbox_to_anchor=(1.02, 0.0),
            borderaxespad=0.0,
            frameon=True,
        )

        plot_file = os.path.join(
            campaignName,
            f"{figure_names['channelUsage']}.png",
        )
        plt.tight_layout(rect=[0, 0, 0.72, 1])
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()
    else:
        print("[WARN] Missing one or more PHY TX channel-usage metrics; skipping stacked plot.")
    
    # -------------------------------------------------------------------------
    # Part 5: Process neighbor-degree-stats-sim-mean-95-ci.csv
    # -------------------------------------------------------------------------
    neighb_degree_data = []

    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        # Extract outer and inner parameter(s) and labels
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        outer_label = format_loop_label(outer_loop_param, outer_extracted)
        inner_extracted = extract_inner_params(folder_name, inner_loop_param)
        inner_label = format_loop_label(inner_loop_param, inner_extracted)

        if outer_extracted is None or inner_extracted is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        metrics_file = os.path.join(
            evalFolder,
            "neighbor-degree-stats-sim-mean-95-ci.csv",
        )
        if not os.path.isfile(metrics_file):
            print(f"[WARN] {metrics_file} not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)

        row = df[df["metricName"] == "MeanAvgNeighbDegree"]
        if row.empty:
            print(f"[WARN] MeanAvgNeighbDegree not found in {metrics_file}")
            continue

        neighb_degree_data.append(
            {
                "outer_label": outer_label,
                "inner_label": inner_label,
                "inner_order": inner_labels_in_order.index(inner_label),
                "Mean": float(row.iloc[0]["Mean"]),
                "95%CI": float(row.iloc[0]["95%CI"]),
            }
        )

    if neighb_degree_data:
        data_df = pd.DataFrame(neighb_degree_data)
        data_csv_file = os.path.join(
            campaignName,
            f"{figure_names['neighborDegree']}.csv",
        )
        data_df.to_csv(data_csv_file, index=False)

        plt.figure(figsize=(2.5, 3))

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_label = get_value_display_label(label_str, value_label_map)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = (
                    rf"$T_2$={ov}"
                    if outer_loop_param == "t2"
                    else get_value_display_label(label_str, value_label_map)
                )

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by="inner_order")

            x = subset["inner_label"].map(lambda v: get_value_display_label(v, value_label_map))
            y = subset["Mean"].to_numpy(dtype=float)
            ci = subset["95%CI"].to_numpy(dtype=float)

            if len(inner_loop_values) == 1:
                n_groups = len(outer_loop_values)

                bar_width = 0.12
                gap = 0.02
                cluster_w = n_groups * bar_width + (n_groups - 1) * gap

                x_positions = x - cluster_w / 2 + (bar_width / 2) + i * (bar_width + gap)

                plt.bar(
                    x_positions,
                    y,
                    yerr=ci,
                    width=bar_width,
                    capsize=4,
                    label=display_label,
                    edgecolor="black",
                    linewidth=0.8,
                    align="center",
                )

                if inner_loop_param == "nApps" and x.size == 1:
                    v = x[0]
                    pad = 0.35
                    plt.xlim(v - (cluster_w / 2 + pad), v + (cluster_w / 2 + pad))
            else:
                plt.errorbar(
                    x,
                    y,
                    yerr=ci,
                    label=display_label,
                    capsize=4,
                    linewidth=0.8,
                    marker="o",
                    markersize=2,
                )

        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
            xticks = [
                get_value_display_label(v, value_label_map)
                for v in data_df.sort_values(by="inner_order")["inner_label"].unique()
            ]
            plt.xticks(xticks)
        else:
            plt.xlabel(get_axis_display_label(inner_loop_param, axis_label_map))

        plt.ylabel("Neighbor degree")
        plt.ylim(bottom=0)
        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)

        plot_file = os.path.join(
            campaignName,
            f"{figure_names['neighborDegree']}.png",
        )
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()
    else:
        print("[WARN] No MeanAvgNeighbDegree mean/CI data found across evaluations; skipping plot.")


def positive_int(value):
    result = int(value)
    if result < 1:
        raise argparse.ArgumentTypeError("must be a positive integer")
    return result


def run_simulation(task):
    params, run, directory = task
    directory.mkdir(parents=True, exist_ok=False)
    command = [str(ROOT / "ns3"), "run", "--no-build", f"--cwd={directory}", SCENARIO,
               "--", f"--RngRun={run}",
               *(f"--{key}={value}" for key, value in params.items())]
    with (directory / "output.txt").open("w") as log:
        log.write("Run Command: " + shlex.join(command) + "\n")
        log.flush()
        subprocess.run(command, cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    print(f"Completed {directory}", flush=True)


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--campaign",
        type=int,
        choices=[1, 2],
        default=1,
        help="1: Connection range sweep (default); 2: nApps sweep",
    )
    parser.add_argument(
        "--runs",
        type=positive_int,
        default=10,
        help="runs per evaluation (default: 10; RngRun=1..N)",
    )
    parser.add_argument(
        "--jobs",
        type=positive_int,
        default=min(10, os.cpu_count() or 1),
        help="maximum concurrent simulations",
    )
    args = parser.parse_args()

    outer_loop_param = ["useGcForRouting", "routingBypass"]
    outer_loop_values = [("true", "true"), ("false", "false"), ("true", "false")]

    if args.campaign == 1:
        campaign_folder = "connection-range-sweep"
        inner_loop_param = "rsrpThreshold"
        inner_loop_values = [-94.1, -101.2, -106.1, -110, -113.2]
        inner_value_label_map = {
            "rsrpThreshold=-94.1": "200",
            "rsrpThreshold=-101.2": "300",
            "rsrpThreshold=-106.1": "400",
            "rsrpThreshold=-110": "500",
            "rsrpThreshold=-113.2": "600",
        }
    elif args.campaign == 2:
        campaign_folder = "nApp-sweep"
        inner_loop_param = "nApps"
        inner_loop_values = list(range(1, 11))
        inner_value_label_map = {
            f"nApps={n_apps}": str(n_apps) for n_apps in inner_loop_values
        }

    destination = (
        ROOT / "milcom-2026-routing-dissemination-outputs" / campaign_folder
    ).resolve()

    axis_label_map = {
        ("useGcForRouting", "routingBypass"): " ",
        "nApps": r"$n_{\mathrm{APP}}$",
        "rsrpThreshold": r"$r_{\gamma}$ (m)",
    }

    value_label_map = {
        "useGcForRouting=false, routingBypass=false": "Unicast",
        "useGcForRouting=true, routingBypass=false": "Groupcast",
        "useGcForRouting=true, routingBypass=true": "Ideal",
    }
    value_label_map.update(inner_value_label_map)

    evaluations = []
    for outer in outer_loop_values:
        for inner in inner_loop_values:
            current = dict(zip(outer_loop_param, outer))
            current[inner_loop_param] = inner
            folder = "_".join(f"{key}-{current[key]}" for key in sorted(current))
            evaluations.append((current, folder))
    runs = range(1, args.runs + 1)
    # Refuse to mix old and new simulation data.
    for _, folder in evaluations:
        for run in runs:
            path = destination / folder / f"Run{run}"
            if path.exists():
                parser.error(f"{path} already exists; remove the campaign output before rerunning")

    ns3 = str(ROOT / "ns3")
    config_result = subprocess.run(
        [ns3, "show", "config"],
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if config_result.returncode != 0:
        parser.error("ns-3 is not configured; run ./ns3 configure --enable-examples")

    subprocess.run(
        [ns3, "build", SCENARIO, "-j", str(args.jobs)],
        cwd=ROOT,
        check=True,
    )
    destination.mkdir(parents=True, exist_ok=True)

    eval_folders = []
    total_evaluations = len(evaluations)
    for evaluation_number, (current, folder) in enumerate(evaluations, start=1):
        print(
            f"Running evaluation {evaluation_number}/{total_evaluations} {folder}",
            flush=True,
        )

        tasks = [(current, run, destination / folder / f"Run{run}") for run in runs]
        with ThreadPoolExecutor(max_workers=min(args.jobs, args.runs)) as pool:
            # Consume results so a failed simulation aborts the campaign.
            list(pool.map(run_simulation, tasks))

        process_evaluation(
            destination / folder,
            destination / folder,
            runs,
            args.campaign,
            args.campaign == 1 and float(current["rsrpThreshold"]) == -106.1,
        )
        eval_folders.append(str(destination / folder))

        print(
            f"Finished evaluation {evaluation_number}/{total_evaluations} {folder}",
            flush=True,
        )

    process_campaign(str(destination), outer_loop_param, outer_loop_values,
                     inner_loop_param, inner_loop_values, eval_folders, axis_label_map, value_label_map)
    print(f"Paper figures written to {destination}")


if __name__ == "__main__":
    main()
