#!/usr/bin/env python3

# Description:
# This script runs a simulation campaign for the nr-prose-multihop-advanced
# scenario.
#
# Prerequisites:
# - Python3 matplotlib, numpy, pandas, scipy
# - nr-prose-multihop-advanced-process-sim.py should be in the ns3 root folder
#
# Usage (from the ns3 root folder): python3 nr-prose-multihop-advanced-run-campaign.py
# Program outputs:
# The program will create a folder named according to the parameter 'campaignName' in the main script.
# Inside the campaign folder the program will generate:
# 1) A folder for each evaluation in the campaign. Each evaluation folder will contain:
#    a) A folder for each simualtion instance containing the raw outputs of the simulation
#       as well as the processed simulation data upon completion.
#    b) PNG image files and CSV files corresponding to the processed evaluation data, e.g., mean
#       and 95% confidence interval, empirical CDFs, etc. of the different metrics, calculated
#       using the data of all simulations in the evaluation.
# 2) PNG image files and CSV files with prefix 'campaignName', corresponding to the agregation of all
#    the data of the evaluations in the campaign

import csv
import math
import os
import re
import subprocess
import sys
import time
from multiprocessing import Pool

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.patches import Patch
import matplotlib.image as mpimg
from scipy import stats


# ------------------------------------------------------------------
# Helper function to extract outer-loop parameter value(s)
# ------------------------------------------------------------------
def extract_outer_params(folder_name, outer_loop_param):
    """
    Extracts the outer parameter value(s) from folder_name.
    If outer_loop_param is a list, returns a tuple of extracted values.
    Otherwise, returns a single string value.
    """
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


# ------------------------------------------------------------------
# Function that creates the run folder and executes the ns3 command
# ------------------------------------------------------------------
def start_simulation(runParams):
    params, rngRun, outputDir, glob_params = runParams

    runDir = f"{outputDir}/Run{rngRun}"
    try:
        os.mkdir(runDir)
    except FileExistsError:
        print(f"Folder {runDir} already exists, overwriting...")

    # Build generic parameter list: --RngRun plus every param in 'params' except campaignName
    param_kv = [f"--{k}={v}" for k, v in params.items() if k != "campaignName"]
    run_command = (
        f"./ns3 run --cwd={runDir} nr-prose-multihop-advanced -- --RngRun={rngRun} "
        + (f"{glob_params} " if glob_params else " ")
        + " ".join(param_kv)
    )

    print(run_command)
    with open(runDir + "/output.txt", "w") as f:
        subprocess.run(run_command.split(), stdout=f, stderr=subprocess.STDOUT)
    with open(runDir + "/output.txt", "a") as f:
        f.write("\nRun Command: " + run_command + "\n")

    subprocess.run(["python3", "nr-prose-multihop-advanced-process-sim.py", runDir], check=True)


# ------------------------------------------------------------------
# Build evaluation folder name using parameters
# ------------------------------------------------------------------
def build_evalFolder_path(params):
    # Put *all* params except the campaignName into the folder name, sorted by key for stability
    parts = []
    for k in sorted(params.keys()):
        if k == "campaignName":
            continue
        parts.append(f"{k}-{params[k]}")
    return os.path.join(params["campaignName"], "_".join(parts))


# ------------------------------------------------------------------
# Function that creates the evaluation folder, runs simulations,
# and processes evaluation statistics.
# ------------------------------------------------------------------
def start_evaluation(outputDir, params, nRuns, nProcesses, glob_params):
    print(f"Output dir: {outputDir}")
    try:
        os.mkdir(outputDir)
    except FileExistsError:
        print(f"Folder {outputDir} already exists, overwriting...")

    allSims = []
    for rngRun in range(1, nRuns + 1):
        # pass a COPY so each worker has its own snapshot
        allSims.append((params.copy(), rngRun, outputDir, glob_params))

    pool = Pool(processes=nProcesses)
    print("Running simulations...")
    pool.imap_unordered(start_simulation, allSims)
    pool.close()
    pool.join()

    process_evaluation(outputDir)
    return outputDir


# ------------------------------------------------------------------
# Function that processes evaluation statistics.
# ------------------------------------------------------------------
def process_evaluation(outputDir):

    if not os.path.isdir(outputDir):
        print(f"Error: {outputDir} is not a directory.")
        sys.exit(1)

    print(f"Processing EVAL {outputDir}")

    # Get all RunX folders in outputDir
    run_dirs = [
        os.path.join(outputDir, d)
        for d in os.listdir(outputDir)
        if os.path.isdir(os.path.join(outputDir, d)) and d.startswith("Run")
    ]
    if not run_dirs:
        print(f"No Run directories found in {outputDir}")
        sys.exit(1)

    # -------------------------------------------------------------------------
    # Part 1: Process voip-stats-sim.csv
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        flow_stats_file = os.path.join(run_dir, "voip-stats-sim.csv")
        if os.path.isfile(flow_stats_file):
            df = pd.read_csv(flow_stats_file)
            all_data.append(df)
        else:
            print(f"Warning: {flow_stats_file} does not exist.")
    if not all_data:
        print(f"No voip-stats-sim.csv files found in any Run directories.")
        sys.exit(1)
    combined_df = pd.concat(all_data, ignore_index=True)
    # Save combined DataFrame
    combined_csv_file = os.path.join(outputDir, "voip-stats-sim-all.csv")
    combined_df.to_csv(combined_csv_file, index=False)
    # Now process the metrics
    metrics = combined_df.columns[2:]
    metrics = [
        m for m in metrics if m != "mean98TailDelay(ms)"
    ]  # Ignore it for 95% CI calculation as it can be 'inf'

    # Prepare lists to store mean and CI
    mean_ci_data = []
    for metric in metrics:
        data = combined_df[metric].dropna()
        if data.empty:
            print(f"No data for metric {metric}")
            continue
        # Compute mean
        mean = data.mean()
        # Compute 95% confidence interval
        n = len(data)
        sem = stats.sem(data)
        confidence = 0.95
        h = sem * stats.t.ppf((1 + confidence) / 2.0, n - 1)
        ci = h
        mean_ci_data.append({"metricName": metric, "Mean": mean, "95%CI": ci})
        # Generate CDF
        sorted_data = np.sort(data)
        yvals = np.arange(1, len(sorted_data) + 1) / float(len(sorted_data))
        # Save CDF data
        cdf_data_file = os.path.join(outputDir, f"voip-stats-sim-{metric}-cdf.csv")
        cdf_df = pd.DataFrame({metric: sorted_data, "cdf": yvals})
        cdf_df.to_csv(cdf_data_file, index=False)
        # Plot CDF
        plt.figure()
        plt.plot(sorted_data, yvals)
        plt.xlabel(metric)
        plt.ylabel("CDF")
        plt.title(f"Cumulative Distribution Function of {metric}")
        plt.grid(True)
        cdf_plot_file = os.path.join(outputDir, f"voip-stats-sim-{metric}-cdf.png")
        plt.savefig(cdf_plot_file)
        plt.close()
    # Save mean and 95% CI data
    mean_ci_df = pd.DataFrame(mean_ci_data)
    mean_ci_file = os.path.join(outputDir, "voip-stats-sim-all-mean-95-ci.csv")
    mean_ci_df.to_csv(mean_ci_file, index=False)

    # -------------------------------------------------------------------------
    # Part 2: Process phy-stats-sim-total.csv
    #         (Just for info, no statistics are calculated)
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        simPhy_stats_file = os.path.join(run_dir, "phy-stats-sim-total.csv")
        if os.path.isfile(simPhy_stats_file):
            df = pd.read_csv(simPhy_stats_file)
            all_data.append(df)
        else:
            print(f"Warning: {simPhy_stats_file} does not exist.")

    if not all_data:
        print(f"No phy-stats-sim-total.csv files found in any Run directories.")
        sys.exit(1)

    combined_df = pd.concat(all_data, ignore_index=True)
    combined_csv_file = os.path.join(outputDir, "phy-stats-sim-total-all.csv")
    combined_df.to_csv(combined_csv_file, index=False)

    # -------------------------------------------------------------------------
    # Part 3: Process phy-stats-sim-percent.csv
    # -------------------------------------------------------------------------
    all_data_percent = []
    for run_dir in run_dirs:
        simPhy_statsPercent_file = os.path.join(run_dir, "phy-stats-sim-percent.csv")
        if os.path.isfile(simPhy_statsPercent_file):
            df = pd.read_csv(simPhy_statsPercent_file)
            all_data_percent.append(df)
        else:
            print(f"Warning: {simPhy_statsPercent_file} does not exist.")

    if not all_data_percent:
        print("No phy-stats-sim-percent.csv files found in any Run directories.")
        sys.exit(1)

    combined_df_percent = pd.concat(all_data_percent, ignore_index=True)

    # Save the combined raw table (same as before)
    combined_csv_file_percent = os.path.join(outputDir, "phy-stats-sim-percent-all.csv")
    combined_df_percent.to_csv(combined_csv_file_percent, index=False)

    metrics_to_use = combined_df_percent.columns[3:]

    # Helper to compute mean and 95% CI (Student's t, two-sided)
    def mean_ci_95(series: pd.Series) -> tuple[float, float]:
        data = series.dropna().astype(float)
        if len(data) == 0:
            return 0.0, 0.0
        mean_val = data.mean()
        if len(data) == 1:
            return float(mean_val), 0.0
        sem = stats.sem(data)
        ci = float(sem * stats.t.ppf((1 + 0.95) / 2.0, len(data) - 1))
        return float(mean_val), ci

    # Compute Mean and 95% CI per (Type, Metric)
    rows = []
    for t, g in combined_df_percent.groupby("Type", dropna=False):
        for metric in metrics_to_use:
            mean_val, ci_val = mean_ci_95(g[metric])
            rows.append(
                {
                    "Type": t,
                    "Metric": metric,
                    "Mean": mean_val,
                    "95%CI": ci_val,
                    "N": int(g[metric].dropna().shape[0]),
                }
            )

    simPhy_percent_mean_ci_df = pd.DataFrame(rows)

    # Save
    simPhy_percent_mean_ci_file = os.path.join(outputDir, "phy-stats-sim-percent-mean-95-ci.csv")
    simPhy_percent_mean_ci_df.to_csv(simPhy_percent_mean_ci_file, index=False)

    # Plot
    plot_metrics = ["nRxDataCorr", "nRxDataHd", "nRxDataNotExp"]

    unique_types = list(simPhy_percent_mean_ci_df["Type"].dropna().unique())
    n_types = len(unique_types)
    if n_types == 0:
        print("No Types found in phy-stats-sim-percent data; skipping plot.")
    else:
        n_rows, n_cols = 1, n_types

        fig, axes = plt.subplots(n_rows, n_cols, figsize=(3 * n_cols, 3 * n_rows), squeeze=False)

        for idx, t in enumerate(unique_types):
            r, c = divmod(idx, n_cols)
            ax = axes[r][c]

            sub = (
                simPhy_percent_mean_ci_df[
                    (simPhy_percent_mean_ci_df["Type"] == t)
                    & (simPhy_percent_mean_ci_df["Metric"].isin(plot_metrics))
                ]
                .set_index("Metric")
                .reindex(plot_metrics)
                .reset_index()
            )

            means = sub["Mean"].to_numpy(dtype=float)
            cis = sub["95%CI"].to_numpy(dtype=float)

            x = np.arange(len(plot_metrics))
            ax.bar(
                x,
                means,
                yerr=cis,
                align="center",
                alpha=0.8,
                capsize=5,
                edgecolor="black",
            )
            ax.set_xticks(x, plot_metrics, rotation=25, ha="right")
            ax.set_ylim(bottom=0)
            ax.set_ylabel("Mean ± 95% CI")
            ax.set_title(f"Type = {t}")
            ax.grid(True, axis="y")

        # Hide unused subplots
        total_slots = n_rows * n_cols
        for k in range(n_types, total_slots):
            r, c = divmod(k, n_cols)
            fig.delaxes(axes[r][c])

        fig.suptitle("Mean ± 95% CI of Selected PHY Stats (Percent) by Type", y=0.995)
        fig.tight_layout()
        mean_ci_percent_bytype_file = os.path.join(
            outputDir, "phy-stats-sim-percent-mean-95-ci-by-type.png"
        )
        plt.savefig(mean_ci_percent_bytype_file)
        plt.close()

    # -------------------------------------------------------------------------
    # Part 4: Process route-change-stats.csv
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        flow_stats_file = os.path.join(run_dir, "route-change-stats.csv")
        if os.path.isfile(flow_stats_file):
            df = pd.read_csv(flow_stats_file)
            all_data.append(df)
        else:
            print(f"Warning: {flow_stats_file} does not exist.")
    if not all_data:
        print(f"No route-change-stats.csv files found in any Run directories.")
        sys.exit(1)
    combined_df = pd.concat(all_data, ignore_index=True)
    # Save combined DataFrame
    combined_csv_file = os.path.join(outputDir, "route-change-stats-all.csv")
    combined_df.to_csv(combined_csv_file, index=False)
    # Now process the metrics
    metrics = combined_df.columns[2:]
    # Prepare lists to store mean and CI
    mean_ci_data = []
    for metric in metrics:
        data = combined_df[metric].dropna()
        if data.empty:
            print(f"No data for metric {metric}")
            continue
        # Compute mean
        mean = data.mean()
        # Compute 95% confidence interval
        n = len(data)
        sem = stats.sem(data)
        confidence = 0.95
        h = sem * stats.t.ppf((1 + confidence) / 2.0, n - 1)
        ci = h
        mean_ci_data.append({"metricName": metric, "Mean": mean, "95%CI": ci})
        # Generate CDF
        sorted_data = np.sort(data)
        yvals = np.arange(1, len(sorted_data) + 1) / float(len(sorted_data))
        # Save CDF data
        cdf_data_file = os.path.join(outputDir, f"route-change-stats-{metric}-cdf.csv")
        cdf_df = pd.DataFrame({metric: sorted_data, "cdf": yvals})
        cdf_df.to_csv(cdf_data_file, index=False)
        # Plot CDF
        plt.figure()
        plt.plot(sorted_data, yvals)
        plt.xlabel(metric)
        plt.ylabel("CDF")
        plt.title(f"Cumulative Distribution Function of {metric}")
        plt.grid(True)
        cdf_plot_file = os.path.join(outputDir, f"route-change-stats-{metric}-cdf.png")
        plt.savefig(cdf_plot_file)
        plt.close()
    # Save mean and 95% CI data
    mean_ci_df = pd.DataFrame(mean_ci_data)
    mean_ci_file = os.path.join(outputDir, "route-change-stats-all-mean-95-ci.csv")
    mean_ci_df.to_csv(mean_ci_file, index=False)

    # -------------------------------------------------------------------------
    # Part 5: Process rlc-tx-pdu-drop-total.csv  (dropRatio = nDroppedPdu / nTxPdu)
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        rlc_total_file = os.path.join(run_dir, "rlc-tx-pdu-drop-total.csv")
        if os.path.isfile(rlc_total_file):
            df = pd.read_csv(rlc_total_file)
            all_data.append(df)
        else:
            print(f"Warning: {rlc_total_file} does not exist.")

    if not all_data:
        print("No rlc-tx-pdu-drop-total.csv files found in any Run directories.")
        sys.exit(1)

    combined_rlc_df = pd.concat(all_data, ignore_index=True)

    # Save combined raw table
    combined_csv_file = os.path.join(outputDir, "rlc-tx-pdu-drop-total-all.csv")
    combined_rlc_df.to_csv(combined_csv_file, index=False)

    # Validate required columns
    for col in ["nTxPdu", "nDroppedPdu"]:
        if col not in combined_rlc_df.columns:
            print(f"Error: Column '{col}' not found in rlc-tx-pdu-drop-total.csv")
            sys.exit(1)

    # Compute per-run dropRatio safely
    tx = combined_rlc_df["nTxPdu"].astype(float)
    dr = combined_rlc_df["nDroppedPdu"].astype(float)

    # Define ratio as 0 when nTxPdu==0 (keeps a bounded ratio and avoids NaNs)
    combined_rlc_df["dropRatio"] = np.where(tx > 0, dr / tx, np.nan)

    # Mean and 95% CI over runs
    data = combined_rlc_df["dropRatio"].dropna().astype(float)
    if data.empty:
        print("No data for metric dropRatio in rlc-tx-pdu-drop-total.csv")
        sys.exit(1)

    mean_val = float(data.mean())
    n = int(len(data))
    if n == 1:
        ci_val = 0.0
    else:
        sem = stats.sem(data)
        ci_val = float(sem * stats.t.ppf((1 + 0.95) / 2.0, n - 1))

    mean_ci_df = pd.DataFrame([{"metricName": "dropRatio", "Mean": mean_val, "95%CI": ci_val}])
    mean_ci_file = os.path.join(outputDir, "rlc-tx-pdu-drop-total-mean-95-ci.csv")
    mean_ci_df.to_csv(mean_ci_file, index=False)

# ------------------------------------------------------------------
# Function that processes the campaign.
# ------------------------------------------------------------------
def process_campaign(
    campaignName,
    outer_loop_param,
    outer_loop_values,
    inner_loop_param,
    inner_loop_values,
    evalFolders,
):

    metrics_data = {}
    folder_mapping = {}  # key: (outer_label, inner_value) -> evalFolder
    # -------------------------------------------------------------------------
    # Part 1: Process voip-stats-sim-all-mean-95-ci.csv
    # -------------------------------------------------------------------------
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)
        # Extract outer parameter(s) and compute a composite label
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        if isinstance(outer_extracted, tuple):
            outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, outer_extracted)])
        else:
            outer_label = f"{outer_loop_param}={outer_extracted}"
        # Extract inner parameter value
        pattern_inner = rf"(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None

        if outer_extracted is None or inner_value is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        folder_mapping[(outer_label, inner_value)] = evalFolder

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
                    inner_loop_param: inner_value,
                    "Mean": row["Mean"],
                    "95%CI": row["95%CI"],
                }
            )

    # Plot error-bar charts for each flowsStats metric
    for metric, data in metrics_data.items():
        data_df = pd.DataFrame(data)
        data_csv_file = os.path.join(campaignName, f"{campaignName}-{metric}-mean-95-ci.csv")
        data_df.to_csv(data_csv_file, index=False)

        # plt.figure(figsize=(5, 2.2))
        plt.figure(figsize=(3, 3))

        for ov in outer_loop_values:
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_parts = []
                for p, v in zip(outer_loop_param, ov):
                    if p == "t2":
                        display_parts.append(rf"$T_2$={v}")
                    elif p == "maxNumTx":
                        display_parts.append(rf"$n_{{\text{{TX}}}}$={v}")
                    else:
                        display_parts.append(f"{p}={v}")
                display_label = ", ".join(display_parts)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = rf"$T_2$={ov}" if outer_loop_param == "t2" else label_str

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by=inner_loop_param)
            x = subset[inner_loop_param]
            y = subset["Mean"]
            ci = subset["95%CI"]

            # map zeros for log plot of meanLossRatio
            if metric == "meanLossRatio":
                y = y.clip(lower=1e-4)
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

        plt.xlabel(inner_loop_param)

        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
            xticks = sorted(data_df[inner_loop_param].unique().astype(int))
            plt.xticks(xticks)   # force only those integers
        else:
            plt.xlabel(inner_loop_param)
        if metric == "meanMos":
            plt.ylabel(metric)
            plt.ylim(2.45, 4.55)
            plt.yticks([2.5, 3, 3.5, 4, 4.5])  # or more if you want
            plt.axhline(4, linestyle="--", color="gray")  # <-- horizontal dashed line
        elif metric == "meanAvgDelay":
            plt.ylabel("meanAvgDelay (ms)")
            plt.ylim(0, 100)
        elif metric == "meanLossRatio":
            plt.ylabel("meanLossRatio")
            plt.yscale("log")
            plt.ylim(5e-5, 1.0)
            plt.yticks([1.0, 0.1, 0.01, 0.001, 0.0001])
            plt.axhline(0.015, linestyle="--", color="gray")  # 1.5%
        elif metric == "nAppOutageMos" or metric == "nAppOutage98TailDelay":
            plt.ylabel(metric)
            plt.ylim(-0.5, 6)
        else:
            plt.ylabel(metric)
            plt.ylim(bottom=0)

        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)
        plot_file = os.path.join(campaignName, f"{campaignName}-{metric}-mean-95-ci.png")
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()

    # -------------------------------------------------------------------------
    # Part 2: Process phy-stats-sim-percent-mean-95-ci.csv
    # -------------------------------------------------------------------------

    simphy_percent_mean_ci = {}
    types_seen = set()

    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        # Extract outer/inner loop labels from folder name (unchanged)
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        if isinstance(outer_extracted, tuple):
            outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, outer_extracted)])
        else:
            outer_label = f"{outer_loop_param}={outer_extracted}"
        pattern_inner = rf"(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None
        if outer_extracted is None or inner_value is None:
            continue

        inner_value_str = str(inner_value)
        simPhy_percent_mean_ci_file = os.path.join(
            evalFolder, "phy-stats-sim-percent-mean-95-ci.csv"
        )
        if not os.path.isfile(simPhy_percent_mean_ci_file):
            print(f"[WARN] {simPhy_percent_mean_ci_file} not found in {evalFolder}")
            continue

        df_percent_mean_ci = pd.read_csv(simPhy_percent_mean_ci_file)

        # Initialize nested dicts
        simphy_percent_mean_ci.setdefault(inner_value_str, {})
        simphy_percent_mean_ci[inner_value_str].setdefault(outer_label, {})

        # Expect columns: Type, Metric, Mean, 95%CI, N
        for _, row in df_percent_mean_ci.iterrows():
            t = str(row["Type"])
            m = row["Metric"]
            mean_val = float(row["Mean"])
            ci_val = float(row["95%CI"])
            types_seen.add(t)

            simphy_percent_mean_ci[inner_value_str][outer_label].setdefault(t, {})
            simphy_percent_mean_ci[inner_value_str][outer_label][t][m] = (
                mean_val,
                ci_val,
            )

    # Style knobs
    xlabel_fontsize = 9
    ylabel_fontsize = 9
    xtick_fontsize = 9
    ytick_fontsize = 8
    title_fontsize = 9
    legend_fontsize = 8.2

    # Stacked metrics (keep the three used in your prior campaign plot)
    stacked_metrics = [
        "nRxDataCorr",
        "nRxDataHd",
        "nRxDataNotExp",
    ]
    alias_map = {
        "nRxDataCorr": "PSSCH decoding error",
        "nRxDataHd": "Half duplex",
        "nRxDataNotExp": "PSCCH decoding error",
    }
    colors = {
        "nRxDataCorr": "C0",
        "nRxDataHd": "C1",
        "nRxDataNotExp": "C2",
    }

    # Build plotting data per (Type -> inner_val -> arrays over outer labels)
    plot_data_by_type = {}
    for inner_val in inner_loop_values:
        inner_val_str = str(inner_val)
        if inner_val_str not in simphy_percent_mean_ci:
            continue
        outer_labels_present = list(simphy_percent_mean_ci[inner_val_str].keys())
        if not outer_labels_present:
            continue

        for t in sorted(types_seen):
            # Prepare containers if needed
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
            # Fill values per outer label
            for ol in outer_labels_present:
                total = 0.0
                for metric in stacked_metrics:
                    mean_val = (
                        simphy_percent_mean_ci[inner_val_str]
                        .get(ol, {})
                        .get(t, {})
                        .get(metric, (0.0, 0.0))[0]
                    )
                    plot_data_by_type[t][inner_val_str]["data"][metric].append(mean_val)
                    total += mean_val
                aggregated_list.append(total)
            plot_data_by_type[t][inner_val_str]["aggregated"] = aggregated_list

    # Write a campaign CSV including Type
    csv_filename = os.path.join(campaignName, f"{campaignName}-phy-loss-dist-by-type.csv")
    with open(csv_filename, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        header = ["Type", "Inner_Value", "Outer_Label"] + stacked_metrics
        csvwriter.writerow(header)
        for t in sorted(types_seen):
            if t not in plot_data_by_type:
                continue
            for inner_val in inner_loop_values:
                inner_val_str = str(inner_val)
                if inner_val_str not in plot_data_by_type[t]:
                    continue
                outer_labels_present = plot_data_by_type[t][inner_val_str]["outer_labels"]
                data_dict = plot_data_by_type[t][inner_val_str]["data"]
                for i, ol in enumerate(outer_labels_present):
                    row = [t, inner_val_str, ol]
                    for metric in stacked_metrics:
                        row.append(data_dict[metric][i])
                    csvwriter.writerow(row)

    # Determine y-limit *per Type* across all inner values
    ymax_by_type = {}
    for t, inner_map in plot_data_by_type.items():
        local_max = 0.0
        for inner_val_str, val in inner_map.items():
            if val["aggregated"]:
                local_max = max(local_max, max(val["aggregated"]))
        margin_factor = 1.2
        ymax_by_type[t] = (local_max * margin_factor) if local_max > 0 else 1.0

    # ----------------------------
    # Figure: one figure per Type, cols = inner loop values
    # ----------------------------
    unique_types = sorted([t for t in types_seen if t in plot_data_by_type])
    n_inner_val = len(inner_loop_values)

    if len(unique_types) == 0:
        print("No Types with data; skipping plot.")
    else:
        for t in unique_types:
            fig, axs = plt.subplots(
                1,
                n_inner_val,
                figsize=(1.7 * n_inner_val, 2.6),
                squeeze=False,
            )

            # Keep one mapping for the whole figure (letters -> outer label)
            mapping_text = None

            for c, inner_val in enumerate(inner_loop_values):
                ax = axs[0][c]
                inner_val_str = str(inner_val)

                if inner_val_str not in plot_data_by_type[t]:
                    ax.set_visible(False)
                    continue

                outer_labels_present = plot_data_by_type[t][inner_val_str]["outer_labels"]
                data_dict = plot_data_by_type[t][inner_val_str]["data"]

                # x labels
                if isinstance(outer_loop_param, list) and len(outer_loop_param) > 1:
                    # Letters on ticks: A, B, ..., Z, AA, AB, ...
                    x_labels = []
                    for i in range(len(outer_labels_present)):
                        n = i + 1
                        s = ""
                        while n > 0:
                            n, rem = divmod(n - 1, 26)
                            s = chr(ord("A") + rem) + s
                        x_labels.append(s)

                    x_param_label = "Outer set"

                    # Build mapping only once (first visible subplot)
                    if mapping_text is None:
                        mapping_lines = [f"{x_labels[i]}) {lbl}" for i, lbl in enumerate(outer_labels_present)]
                        mapping_text = "\n".join(mapping_lines)
                else:
                    pattern = rf"{outer_loop_param}=([^\s,]+)"
                    x_labels = [
                        (re.search(pattern, label).group(1) if re.search(pattern, label) else label)
                        for label in outer_labels_present
                    ]
                    x_param_label = outer_loop_param

                x = np.arange(len(x_labels))
                bar_width = 0.8
                bottoms = np.zeros(len(x_labels))
                for metric in stacked_metrics:
                    values = np.array(data_dict[metric])
                    ax.bar(
                        x,
                        values,
                        bar_width,
                        bottom=bottoms,
                        color=colors[metric],
                        label=alias_map[metric],
                    )
                    bottoms += values

                ax.set_ylim(0, ymax_by_type.get(t, 1.0))
                ax.set_xlabel(x_param_label, fontsize=xlabel_fontsize, labelpad=0)
                if c == 0:
                    ax.set_ylabel("%", fontsize=ylabel_fontsize, labelpad=0)
                ax.set_title(
                    f"Type={t} · {inner_loop_param}={inner_val_str}",
                    fontsize=title_fontsize,
                    pad=1,
                )
                if outer_loop_param == "t2":
                    ax.set_xlabel(r"$T_2$", fontsize=xlabel_fontsize)
                else:
                    ax.set_xlabel(x_param_label, fontsize=xlabel_fontsize)

                ax.set_xticks(x)
                if isinstance(outer_loop_param, list) and len(outer_loop_param) > 1:
                    ax.set_xticklabels(x_labels, fontsize=7, rotation=0, ha="center")
                else:
                    ax.set_xticklabels(x_labels, fontsize=7, rotation=30, ha="right")

                ax.tick_params(axis="y", labelsize=ytick_fontsize)
                ax.grid(True, axis="y", alpha=0.25)

            # Legend (per-figure) — moved up to leave room below
            handles = [Patch(facecolor=colors[m], label=alias_map[m]) for m in stacked_metrics]
            fig.legend(
                handles,
                [alias_map[m] for m in stacked_metrics],
                loc="lower center",
                ncol=len(stacked_metrics),
                fontsize=legend_fontsize,
                frameon=False,
                bbox_to_anchor=(0.5, 0.05*len(outer_labels_present)),
            )

            # Mapping (once per figure, bottom-left)
            if mapping_text is not None:
                fig.text(
                    0.5, 0, mapping_text,
                    ha="center", va="bottom",
                    fontsize=8,
                    family="monospace",
                )

            # Fix spacing: reserve bottom area for mapping + legend, avoid squeezing axes
            fig.subplots_adjust(bottom=(0.18*len(outer_labels_present)), wspace=0.4)

            stacked_filename = os.path.join(
                campaignName, f"{campaignName}-phy-loss-dist-type-{t}.png"
            )
            plt.savefig(stacked_filename, bbox_inches="tight")
            plt.close()


    # -------------------------------------------------------------------------
    # Part 4: Process route-change-stats.csv
    # -------------------------------------------------------------------------
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)
        # Extract outer parameter(s) and compute a composite label
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        if isinstance(outer_extracted, tuple):
            outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, outer_extracted)])
        else:
            outer_label = f"{outer_loop_param}={outer_extracted}"
        # Extract inner parameter value
        pattern_inner = rf"(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None

        if outer_extracted is None or inner_value is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        folder_mapping[(outer_label, inner_value)] = evalFolder

        metrics_file = os.path.join(evalFolder, "route-change-stats-all-mean-95-ci.csv")
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
                    inner_loop_param: inner_value,
                    "Mean": row["Mean"],
                    "95%CI": row["95%CI"],
                }
            )

    # Plot error-bar charts for each flowsStats metric
    for metric, data in metrics_data.items():
        data_df = pd.DataFrame(data)
        data_csv_file = os.path.join(campaignName, f"{campaignName}-{metric}-mean-95-ci.csv")
        data_df.to_csv(data_csv_file, index=False)

        # plt.figure(figsize=(5, 2.2))
        plt.figure(figsize=(3, 3))

        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_parts = []
                for p, v in zip(outer_loop_param, ov):
                    if p == "t2":
                        display_parts.append(rf"$T_2$={v}")
                    elif p == "maxNumTx":
                        display_parts.append(rf"$n_{{\text{{TX}}}}$={v}")
                    else:
                        display_parts.append(f"{p}={v}")
                display_label = ", ".join(display_parts)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = rf"$T_2$={ov}" if outer_loop_param == "t2" else label_str

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by=inner_loop_param)
            x = subset[inner_loop_param]
            y = subset["Mean"]
            ci = subset["95%CI"]
            # map zeros for log plot of meanLossRatio
            if metric == "meanLossRatio":
                y = y.clip(lower=1e-4)
            if len(inner_loop_values) == 1:
                # --- tightly clustered grouped bars around each integer x ---
                n_groups = len(outer_loop_values)

                # choose a small absolute bar width and gap (in "x units")
                bar_width = 0.12                # ~12% of the distance between integer ticks
                gap       = 0.02                # small separation between bars

                    # total width of the cluster (n bars + (n-1) gaps)
                cluster_w = n_groups * bar_width + (n_groups - 1) * gap

                # centers for each group's bar, all packed around x
                # start at the left edge of the cluster and step by (bar_width + gap)
                x = subset[inner_loop_param].astype(float).to_numpy()
                y = subset["Mean"].to_numpy()
                ci = subset["95%CI"].to_numpy()
                if metric == "meanLossRatio":
                    y = np.clip(y, 1e-4, None)

                x_positions = x - cluster_w/2 + (bar_width/2) + i * (bar_width + gap)

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
                # keep the cluster comfortably inside the axes if there’s only one tick
                if inner_loop_param == "nApps" and x.size == 1:
                    v = x[0]
                    pad = 0.35
                    plt.xlim(v - (cluster_w/2 + pad), v + (cluster_w/2 + pad))
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

        plt.xlabel(inner_loop_param)

        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
            xticks = sorted(data_df[inner_loop_param].unique().astype(int))
            plt.xticks(xticks)   # force only those integers
        else:
            plt.xlabel(inner_loop_param)
        if metric == "meanMos":
            plt.ylabel(metric)
            plt.ylim(2.45, 4.55)
            plt.yticks([2.5, 3, 3.5, 4, 4.5])  # or more if you want
            plt.axhline(4, linestyle="--", color="gray")  # <-- horizontal dashed line
        elif metric == "meanAvgDelay":
            plt.ylabel("meanAvgDelay (ms)")
            plt.ylim(0, 100)
        elif metric == "meanLossRatio":
            plt.ylabel("meanLossRatio")
            plt.yscale("log")
            plt.ylim(5e-5, 1.0)
            plt.yticks([1.0, 0.1, 0.01, 0.001, 0.0001])
            plt.axhline(0.015, linestyle="--", color="gray")  # 1.5%
        elif metric == "nAppOutageMos" or metric == "nAppOutage98TailDelay":
            plt.ylabel(metric)
            plt.ylim(-0.5, 6)
        else:
            plt.ylabel(metric)
            plt.ylim(bottom=0)

        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)
        plot_file = os.path.join(campaignName, f"{campaignName}-{metric}-mean-95-ci.png")
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()

    # -------------------------------------------------------------------------
    # Part 5: Process rlc-tx-pdu-drop-total-mean-95-ci.csv (dropRatio)
    # -------------------------------------------------------------------------
    rlc_drop_data = []

    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)

        # Extract outer parameter(s) and label
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        if isinstance(outer_extracted, tuple):
            outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, outer_extracted)])
        else:
            outer_label = f"{outer_loop_param}={outer_extracted}"

        # Extract inner parameter value
        pattern_inner = rf"(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)"
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None

        if outer_extracted is None or inner_value is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        metrics_file = os.path.join(evalFolder, "rlc-tx-pdu-drop-total-mean-95-ci.csv")
        if not os.path.isfile(metrics_file):
            print(f"[WARN] {metrics_file} not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)

        row = df[df["metricName"] == "dropRatio"]
        if row.empty:
            print(f"[WARN] dropRatio not found in {metrics_file}")
            continue

        rlc_drop_data.append(
            {
                "outer_label": outer_label,
                inner_loop_param: inner_value,
                "Mean": float(row.iloc[0]["Mean"]),
                "95%CI": float(row.iloc[0]["95%CI"]),
            }
        )

    if rlc_drop_data:
        data_df = pd.DataFrame(rlc_drop_data)
        data_csv_file = os.path.join(campaignName, f"{campaignName}-rlcDropRatio-mean-95-ci.csv")
        data_df.to_csv(data_csv_file, index=False)

        plt.figure(figsize=(3, 3))

        # IMPORTANT: enumerate(...) so we can cluster bars when only one inner value
        for i, ov in enumerate(outer_loop_values):
            if isinstance(outer_loop_param, list):
                label_str = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, ov)])
                display_parts = []
                for p, v in zip(outer_loop_param, ov):
                    if p == "t2":
                        display_parts.append(rf"$T_2$={v}")
                    elif p == "maxNumTx":
                        display_parts.append(rf"$n_{{\text{{TX}}}}$={v}")
                    else:
                        display_parts.append(f"{p}={v}")
                display_label = ", ".join(display_parts)
            else:
                label_str = f"{outer_loop_param}={ov}"
                display_label = rf"$T_2$={ov}" if outer_loop_param == "t2" else label_str

            subset = data_df[data_df["outer_label"] == label_str]
            subset = subset.sort_values(by=inner_loop_param)

            x = subset[inner_loop_param].astype(float).to_numpy()
            y = subset["Mean"].to_numpy(dtype=float)
            ci = subset["95%CI"].to_numpy(dtype=float)

            if len(inner_loop_values) == 1:
                # --- tightly clustered grouped bars around each integer x ---
                n_groups = len(outer_loop_values)

                bar_width = 0.12
                gap = 0.02
                cluster_w = n_groups * bar_width + (n_groups - 1) * gap

                # centers for each group's bar, packed around x
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

                # keep the cluster comfortably inside the axes if there’s only one tick
                if inner_loop_param == "nApps" and x.size == 1:
                    v = x[0]
                    pad = 0.35
                    plt.xlim(v - (cluster_w / 2 + pad), v + (cluster_w / 2 + pad))

            else:
                # Standard errorbar line+markers (same style as other plots)
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

        # X label formatting consistent with the rest of the script
        if inner_loop_param == "nApps":
            plt.xlabel("Number of VoIP flows (nApp)")
            xticks = sorted(data_df[inner_loop_param].unique().astype(int))
            plt.xticks(xticks)
        else:
            plt.xlabel(inner_loop_param)

        plt.ylabel("Mean RLC drop ratio (nDrop/nTx)")
        plt.ylim(bottom=0)
        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)

        plot_file = os.path.join(campaignName, f"{campaignName}-rlcDropRatio-mean-95-ci.png")
        plt.savefig(plot_file, bbox_inches="tight")
        plt.close()
    else:
        print("[WARN] No RLC dropRatio mean/CI data found across evaluations; skipping plot.")

def combine_plots(image_paths, out_path, ncols=2, width_in=12, dpi=200):
    """
    Tiles existing PNGs into one image.
    - image_paths: list of file paths to PNGs (order = left->right, top->bottom)
    - out_path: output PNG path
    - ncols: columns in the grid
    - width_in: final figure width (inches)
    - dpi: output DPI
    """
    if not image_paths:
        print("combine_plots: no images given")
        return

    n = len(image_paths)
    nrows = math.ceil(n / ncols)
    # Keep a reasonable aspect by scaling height with rows/cols
    height_in = width_in * (nrows / ncols)

    fig, axes = plt.subplots(nrows, ncols, figsize=(width_in, height_in), squeeze=False)
    # Hide all axes by default
    for ax in axes.ravel():
        ax.axis("off")

    for ax, path in zip(axes.ravel(), image_paths):
        img = mpimg.imread(path)  # works great with PNG
        ax.imshow(img)
        ax.set_aspect("auto")
        ax.axis("off")

    plt.tight_layout(pad=0.05)
    fig.savefig(out_path, dpi=dpi, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved combined figure to: {out_path}")

# ------------------------------------------------------------------
# Main script entry point
# ------------------------------------------------------------------
def main():
    start_time = time.time()
    process_only = False


    if not process_only:
        # Inital compilation
        print("Running ./ns3 build...")
        output = subprocess.run(
            ["./ns3", "show", "config"], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )
        if output.returncode:
            print("Error:  Is the project configured?  Run ./ns3 configure ... first")
            print(output.stdout.decode("utf-8"))
            print(output.stderr.decode("utf-8"))
            sys.exit(1)
        output = subprocess.run(["./ns3", "build"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if output.returncode:
            print("Error:  The build failed; fix it to continue")
            print(output.stdout.decode("utf-8"))
            print(output.stderr.decode("utf-8"))
            sys.exit(1)

    # Number of runs per evaluation
    nRuns = 10

    # Set maximum number of simultaneous processes
    nProcesses = 100

    # Global simulation parameters that will be used for all simulations in the campaing
    # Simulation parameters later in this script and parameters configured with Config::SetDefault
    # in the scenario after CommandLine processing will override this
    glob_params = ("--ns3::NrSlRlcUm::DiscardTimerScale=3  "
                   "--ns3::NrSlUeMacSchedulerDefault::MinimumSpsGrantSize=80 "
                   "--ns3::NrSlUeMacSchedulerDefault::SpsReselectionThreshold=240 "
                   "--ns3::NrSlUeMacSchedulerDefault::AllowSupplementalDynamicGrants=false"
                   )

    # Default simulation parameters.
    # This is generic now, i.e., any parameter of the nr-prose-multihop-advanced scenario can be set
    params = {
        "gridSize": 4,  # 4,9,or 16
        "lossModel": "matrix",
        "idealSchedLevel": "Global",
        "routingBypass" : "true",
        "errorModel" : "static",
        "maxNumTx" : 1,
        "rtxType" : "No",
        "rsrpThreshold" : "-129"
    }

    # The two parameters to sweep (override default values)
    outer_loop_param = ["mcs", "d", "rsrpThreshold"]
    outer_loop_values = [(0, 1300, -126.6), (14, 800, -115.2), (28, 400, -103.2)]
    inner_loop_param = "nApps"
    inner_loop_values = [1]

    campaignName = "z_1"
    os.makedirs(campaignName, exist_ok=True)

    evalFolders = []
    # Launch evaluations: unpack outer values if necessary.
    if isinstance(outer_loop_param, list):
        for outer_value in outer_loop_values:
            for i, key in enumerate(outer_loop_param):
                params[key] = outer_value[i]
            params["campaignName"] = campaignName
            for inner_value in inner_loop_values:
                params[inner_loop_param] = inner_value
                evalFolder = build_evalFolder_path(params)
                if not process_only:
                    start_evaluation(evalFolder, params, nRuns, nProcesses, glob_params)
                evalFolders.append(evalFolder)
    else:
        for outer_value in outer_loop_values:
            params[outer_loop_param] = outer_value
            params["campaignName"] = campaignName
            for inner_value in inner_loop_values:
                params[inner_loop_param] = inner_value
                evalFolder = build_evalFolder_path(params)
                if not process_only:
                    start_evaluation(evalFolder, params, nRuns, nProcesses, glob_params)
                evalFolders.append(evalFolder)

    print(f"Processing CAMPAIGN {campaignName}")
    process_campaign(
        campaignName,
        outer_loop_param,
        outer_loop_values,
        inner_loop_param,
        inner_loop_values,
        evalFolders,
    )

    #Combine some of the plots generated by process_campaign in one image
    plots_to_merge = [
        os.path.join(campaignName, f"{campaignName}-meanMos-mean-95-ci.png"),
        os.path.join(campaignName, f"{campaignName}-meanLossRatio-mean-95-ci.png"),
        os.path.join(campaignName, f"{campaignName}-meanAvgDelay-mean-95-ci.png"),
        os.path.join(campaignName, f"{campaignName}-nAppOutageMos-mean-95-ci.png"),
        os.path.join(campaignName, f"{campaignName}-nAppOutage98TailDelay-mean-95-ci.png"),
    ]
    output_filename = "CombinedResults"
    combine_plots(
        plots_to_merge,
        os.path.join(campaignName, f"{campaignName}-{output_filename}.png"),
        ncols=3,
        width_in=12,
        dpi=200
)

    end_time = time.time()
    duration_s = end_time - start_time
    duration_m = duration_s / 60.0
    print(f"The script took {duration_m:.2f} minutes to run.")


if __name__ == "__main__":
    main()
