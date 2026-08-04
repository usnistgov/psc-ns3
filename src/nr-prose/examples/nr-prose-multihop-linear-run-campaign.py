#!/usr/bin/env python3

# Description:
# This script runs a simulation campaign for the nr-prose-multihop-linear
# scenario.
#
# Prerequisites:
# - Python3 matplotlib, numpy, pandas, scipy
# - nr-prose-multihop-linear-process-sim.py should be in the ns3 root folder
#
# Usage (from the ns3 root folder): python3 nr-prose-multihop-linear-run-campaign.py
# Program outputs:
# 1) Directories with simulation results, named with prefix 'campaignName'
# 2) PNG image files and CSV files with prefix 'campaignName', corresponding to campaign results

import csv
import os
import subprocess
import sys
import math
import time
import re
from multiprocessing import Pool

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy import stats
from matplotlib.patches import Patch


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
            pattern = rf'(?:^|_){param}-(.*?)(?=_[a-zA-Z]+-|$)'
            matches = re.findall(pattern, folder_name)
            if matches:
                values.append(matches[-1])
            else:
                print(f"Could not find parameter '{param}' in folder name '{folder_name}'")
                values.append(None)
        return tuple(values)
    else:
        pattern = rf'(?:^|_){outer_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)'
        matches = re.findall(pattern, folder_name)
        return matches[-1] if matches else None

# ------------------------------------------------------------------
# Function that creates the run folder and executes the ns3 command
# ------------------------------------------------------------------
def start_simulation(runParams):
    (
        nUes,
        iud,
        codec,
        mcs,
        maxNumTx,
        rtxType,
        wSlotEx,
        t2,
        rngRun,
        outputDir
    ) = runParams

    runDir = f"{outputDir}/Run{rngRun}"
    try:
        os.mkdir(runDir)
    except FileExistsError:
        print(f"Folder {runDir} already exists, overwriting...")

    # Run the simulation
    run_command = f"./ns3 run --cwd={runDir} nr-prose-multihop-linear -- --RngRun={rngRun} --nUes={nUes} --iud={iud} --codec={codec} --mcs={mcs} --maxNumTx={maxNumTx} --rtxType={rtxType} --wSlotEx={wSlotEx} --t2={t2}"
    print(run_command)
    with open(runDir + "/output.txt", "w") as f:
        subprocess.run(run_command.split(), stdout=f, stderr=subprocess.STDOUT)
    with open(runDir + "/output.txt", "a") as f:
        f.write("\nRun Command: " + run_command + "\n")

    # Run the processing script for the simulation
    subprocess.run(["python3", "nr-prose-multihop-linear-process-sim.py", runDir], check=True)


# ------------------------------------------------------------------
# Build evaluation folder name using parameters
# ------------------------------------------------------------------
def build_evalFolder_name(params):
    outputDir = (
        f"{params['campaignName']}_nUes-{params['nUes']}_iud-{params['iud']}_codec-{params['codec']}_mcs-{params['mcs']}_maxNumTx-{params['maxNumTx']}_rtxType-{params['rtxType']}_wSlotEx-{params['wSlotEx']}_t2-{params['t2']}"
    )
    return outputDir

# ------------------------------------------------------------------
# Function that creates the evaluation folder, runs simulations,
# and processes evaluation statistics.
# ------------------------------------------------------------------
def start_evaluation(params, nRuns, nProcesses):
    outputDir = build_evalFolder_name(params)
    print(f"Output dir: {outputDir}")
    try:
        os.mkdir(outputDir)
    except FileExistsError:
        print(f"Folder {outputDir} already exists, overwriting...")

    # Create a list with all simulation to run
    allSims = []
    for rngRun in range(1, nRuns + 1):
        runParams = (
            params['nUes'],
            params['iud'],
            params['codec'],
            params['mcs'],
            params['maxNumTx'],
            params['rtxType'],
            params['wSlotEx'],
            params['t2'],
            rngRun,
            outputDir
        )
        allSims.append(runParams)

    pool = Pool(processes=nProcesses)
    print("Running simulations...")
    pool.imap_unordered(start_simulation, allSims)
    pool.close()
    pool.join()

    # Run the evaluation processing script for the campaign
    process_evaluation(outputDir)

    return outputDir
# ------------------------------------------------------------------
# Function that processes evaluation statistics.
# ------------------------------------------------------------------
def process_evaluation(outputDir):

    if not os.path.isdir(outputDir):
        print(f"Error: {outputDir} is not a directory.")
        sys.exit(1)

    print(f'Processing EVAL {outputDir}')

    # Get all RunX folders in outputDir
    run_dirs = [os.path.join(outputDir, d) for d in os.listdir(outputDir)
                if os.path.isdir(os.path.join(outputDir, d)) and d.startswith('Run')]
    if not run_dirs:
        print(f"No Run directories found in {outputDir}")
        sys.exit(1)

    # -------------------------------------------------------------------------
    # Part 1: Process voip-stats-sim.csv
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        flow_stats_file = os.path.join(run_dir, 'voip-stats-sim.csv')
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
    combined_csv_file = os.path.join(outputDir, 'voip-stats-sim-all.csv')
    combined_df.to_csv(combined_csv_file, index=False)
    # Now process the metrics
    metrics = combined_df.columns[4:]
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
        h = sem * stats.t.ppf((1 + confidence) / 2., n-1)
        ci = h
        mean_ci_data.append({'metricName': metric, 'Mean': mean, '95%CI': ci})
        # Generate CDF
        sorted_data = np.sort(data)
        yvals = np.arange(1, len(sorted_data)+1) / float(len(sorted_data))
        # Save CDF data
        cdf_data_file = os.path.join(outputDir, f'voip-stats-sim-{metric}-cdf.csv')
        cdf_df = pd.DataFrame({metric: sorted_data, 'cdf': yvals})
        cdf_df.to_csv(cdf_data_file, index=False)
        # Plot CDF
        plt.figure()
        plt.plot(sorted_data, yvals)
        plt.xlabel(metric)
        plt.ylabel('CDF')
        plt.title(f'Cumulative Distribution Function of {metric}')
        plt.grid(True)
        cdf_plot_file = os.path.join(outputDir, f'voip-stats-sim-{metric}-cdf.png')
        plt.savefig(cdf_plot_file)
        plt.close()
    # Save mean and 95% CI data
    mean_ci_df = pd.DataFrame(mean_ci_data)
    mean_ci_file = os.path.join(outputDir, 'voip-stats-sim-all-mean-95-ci.csv')
    mean_ci_df.to_csv(mean_ci_file, index=False)

    # -------------------------------------------------------------------------
    # Part 2: Process phy-stats-sim.csv
    #         (Just for info, no statistics are calculated)
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        simPhy_stats_file = os.path.join(run_dir, 'phy-stats-sim.csv')
        if os.path.isfile(simPhy_stats_file):
            df = pd.read_csv(simPhy_stats_file)
            all_data.append(df)
        else:
            print(f"Warning: {simPhy_stats_file} does not exist.")

    if not all_data:
        print(f"No phy-stats-sim.csv files found in any Run directories.")
        sys.exit(1)

    combined_df = pd.concat(all_data, ignore_index=True)
    combined_csv_file = os.path.join(outputDir, 'phy-stats-sim-all.csv')
    combined_df.to_csv(combined_csv_file, index=False)

    # -------------------------------------------------------------------------
    # Part 3: Process phy-stats-sim-percent.csv
    # -------------------------------------------------------------------------
    all_data_percent = []
    for run_dir in run_dirs:
        simPhy_statsPercent_file = os.path.join(run_dir, 'phy-stats-sim-percent.csv')
        if os.path.isfile(simPhy_statsPercent_file):
            df = pd.read_csv(simPhy_statsPercent_file)
            all_data_percent.append(df)
        else:
            print(f"Warning: {simPhy_statsPercent_file} does not exist.")

    if not all_data_percent:
        print(f"No phy-stats-sim-percent.csv files found in any Run directories.")
        sys.exit(1)

    combined_df_percent = pd.concat(all_data_percent, ignore_index=True)
    combined_csv_file_percent = os.path.join(outputDir, 'phy-stats-sim-percent-all.csv')
    combined_df_percent.to_csv(combined_csv_file_percent, index=False)

    metrics_to_sum_percent = combined_df_percent.columns[6:]

    mean_vals_percent = {}
    ci_vals_percent = {}

    for metric in metrics_to_sum_percent:
        if metric not in combined_df_percent.columns:
            # If it's not in the file at all, skip
            mean_vals_percent[metric] = 0.0
            ci_vals_percent[metric] = 0.0
            continue

        data_percent = combined_df_percent[metric].dropna()
        if len(data_percent) > 0:
            mean_val = data_percent.mean()
            if len(data_percent) > 1:
                sem = stats.sem(data_percent)
                ci_val = sem * stats.t.ppf((1 + 0.95) / 2., len(data_percent) - 1)
            else:
                ci_val = 0.0
        else:
            mean_val = 0.0
            ci_val = 0.0

        mean_vals_percent[metric] = mean_val
        ci_vals_percent[metric] = ci_val

    # Build a DataFrame with *all* metrics
    simPhy_percent_mean_ci_df = pd.DataFrame({
        "Metric": list(mean_vals_percent.keys()),
        "Mean": [mean_vals_percent[m] for m in mean_vals_percent],
        "95%CI": [ci_vals_percent[m] for m in ci_vals_percent]
    })

    simPhy_percent_mean_ci_file = os.path.join(outputDir, 'phy-stats-sim-percent-mean-95-ci.csv')
    simPhy_percent_mean_ci_df.to_csv(simPhy_percent_mean_ci_file, index=False)

    # Now plot the bar chart for *only* metrics_to_sum_percent
    means_to_plot_percent = [mean_vals_percent[m] for m in metrics_to_sum_percent]
    cis_to_plot_percent = [ci_vals_percent[m] for m in metrics_to_sum_percent]

    plt.figure()
    x_positions = np.arange(len(metrics_to_sum_percent))
    plt.bar(
        x_positions,
        means_to_plot_percent,
        yerr=cis_to_plot_percent,
        align='center',
        alpha=0.7,
        capsize=5,
        color='lightgreen',
        edgecolor='black'
    )
    plt.xticks(x_positions, metrics_to_sum_percent, rotation=25, ha='right')
    plt.xlabel('Metrics')
    plt.ylabel('Mean ± 95% CI')
    plt.title('Mean and 95% Confidence Interval of Metrics (simPhyStatsPercent)')
    plt.grid(True, axis='y')
    plt.tight_layout()
    mean_ci_percent_hist_file = os.path.join(outputDir, 'phy-stats-sim-percent-mean-95-ci-histogram.png')
    plt.savefig(mean_ci_percent_hist_file)
    plt.close()

    # -------------------------------------------------------------------------
    # Part 4: Process route-change-stats.csv
    # -------------------------------------------------------------------------
    all_data = []
    for run_dir in run_dirs:
        flow_stats_file = os.path.join(run_dir, 'route-change-stats.csv')
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
    combined_csv_file = os.path.join(outputDir, 'route-change-stats-all.csv')
    combined_df.to_csv(combined_csv_file, index=False)
    # Now process the metrics
    metrics = combined_df.columns[4:]
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
        h = sem * stats.t.ppf((1 + confidence) / 2., n-1)
        ci = h
        mean_ci_data.append({'metricName': metric, 'Mean': mean, '95%CI': ci})
        # Generate CDF
        sorted_data = np.sort(data)
        yvals = np.arange(1, len(sorted_data)+1) / float(len(sorted_data))
        # Save CDF data
        cdf_data_file = os.path.join(outputDir, f'route-change-stats-{metric}-cdf.csv')
        cdf_df = pd.DataFrame({metric: sorted_data, 'cdf': yvals})
        cdf_df.to_csv(cdf_data_file, index=False)
        # Plot CDF
        plt.figure()
        plt.plot(sorted_data, yvals)
        plt.xlabel(metric)
        plt.ylabel('CDF')
        plt.title(f'Cumulative Distribution Function of {metric}')
        plt.grid(True)
        cdf_plot_file = os.path.join(outputDir, f'route-change-stats-{metric}-cdf.png')
        plt.savefig(cdf_plot_file)
        plt.close()
    # Save mean and 95% CI data
    mean_ci_df = pd.DataFrame(mean_ci_data)
    mean_ci_file = os.path.join(outputDir, 'route-change-stats-all-mean-95-ci.csv')
    mean_ci_df.to_csv(mean_ci_file, index=False)


# ------------------------------------------------------------------
# Function that processes the campaign.
# ------------------------------------------------------------------
def process_campaign(campaignName, outer_loop_param, outer_loop_values, inner_loop_param, inner_loop_values, evalFolders):

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
        pattern_inner = rf'(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)'
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None

        if outer_extracted is None or inner_value is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        folder_mapping[(outer_label, inner_value)] = evalFolder



        metrics_file = os.path.join(evalFolder, 'voip-stats-sim-all-mean-95-ci.csv')
        if not os.path.isfile(metrics_file):
            print(f"Metrics file not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)
        for _, row in df.iterrows():
            metric = row['metricName']
            if metric not in metrics_data:
                metrics_data[metric] = []
            metrics_data[metric].append({
                "outer_label": outer_label,
                inner_loop_param: float(inner_value),
                'Mean': row['Mean'],
                '95%CI': row['95%CI']
            })

    # Plot error-bar charts for each flowsStats metric
    for metric, data in metrics_data.items():
        data_df = pd.DataFrame(data)
        data_csv_file = f"{campaignName}-{metric}-mean-95-ci.csv"
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
            y = subset['Mean']
            ci = subset['95%CI']
            plt.errorbar(x, y, yerr=ci, label=display_label, capsize=4, linewidth=0.8, marker='o', markersize=2)


        plt.xlabel(inner_loop_param)

        if inner_loop_param == "nUes":
            plt.xlabel(r"$n_{\text{UE}}$")
        else:
            plt.xlabel(inner_loop_param)

        if metric == "Mos":
            plt.ylabel("MOS")
            plt.ylim(2.45, 4.55)
            plt.yticks([ 2.5, 3, 3.5, 4, 4.5])  # or more if you want
            plt.axhline(4, linestyle='--', color='gray')  # <-- horizontal dashed line
        else:
            plt.ylabel(metric)

        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)
        plot_file = f"{campaignName}-{metric}-mean-95-ci.png"
        plt.savefig(plot_file, bbox_inches='tight')
        plt.close()


    # -------------------------------------------------------------------------
    # Part 2: Process phy-stats-sim-percent-mean-95-ci.csv
    # -------------------------------------------------------------------------

    simphy_percent_mean_ci = {}
    for evalFolder in evalFolders:
        folder_name = os.path.basename(evalFolder)
        outer_extracted = extract_outer_params(folder_name, outer_loop_param)
        if isinstance(outer_extracted, tuple):
            outer_label = ", ".join([f"{p}={v}" for p, v in zip(outer_loop_param, outer_extracted)])
        else:
            outer_label = f"{outer_loop_param}={outer_extracted}"
        pattern_inner = rf'(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)'
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None
        if outer_extracted is None or inner_value is None:
            continue
        inner_value_str = str(inner_value)
        if inner_value_str not in simphy_percent_mean_ci:
            simphy_percent_mean_ci[inner_value_str] = {}
        if outer_label not in simphy_percent_mean_ci[inner_value_str]:
            simphy_percent_mean_ci[inner_value_str][outer_label] = {}
        simPhy_percent_mean_ci_file = os.path.join(evalFolder, "phy-stats-sim-percent-mean-95-ci.csv")
        if not os.path.isfile(simPhy_percent_mean_ci_file):
            print(f"[WARN] {simPhy_percent_mean_ci_file} not found in {evalFolder}")
            continue
        df_percent_mean_ci = pd.read_csv(simPhy_percent_mean_ci_file)
        for _, row in df_percent_mean_ci.iterrows():
            metric = row['Metric']
            simphy_percent_mean_ci[inner_value_str][outer_label][metric] = (row['Mean'], row['95%CI'])

    xlabel_fontsize = 9
    ylabel_fontsize = 9
    xtick_fontsize = 9
    ytick_fontsize = 8
    title_fontsize = 9
    legend_fontsize = 8.2

    stacked_metrics = [
        'nRxDataCorr',
        'nRxDataHd',
        'nRxDataNotExp'
    ]
    alias_map = {
        'nRxDataCorr': 'PSSCH decoding error',
        'nRxDataHd': 'Half duplex',
        'nRxDataNotExp': 'PSCCH decoding error'
    }
    colors = {
        'nRxDataCorr': 'C0',
        'nRxDataHd': 'C1',
        'nRxDataNotExp': 'C2'
    }

    plot_data_stack = {}
    for inner_val in inner_loop_values:
        inner_val_str = str(inner_val)
        if inner_val_str not in simphy_percent_mean_ci:
            continue
        outer_labels_present = list(simphy_percent_mean_ci[inner_val_str].keys())
        if not outer_labels_present:
            continue
        data_dict = {metric: [] for metric in stacked_metrics}
        aggregated_list = []
        for ol in outer_labels_present:
            total = 0
            for metric in stacked_metrics:
                mean_val = simphy_percent_mean_ci[inner_val_str][ol].get(metric, (0, 0))[0]
                data_dict[metric].append(mean_val)
                total += mean_val
            aggregated_list.append(total)
        plot_data_stack[inner_val_str] = {
            'outer_labels': outer_labels_present,
            'data': data_dict,
            'aggregated': aggregated_list
        }

    # Generate CSV file with the data that is plotted.
    csv_filename = f"{campaignName}-phy-loss-dist.csv"
    with open(csv_filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        header = ["Inner_Value", "Outer_Label"] + stacked_metrics
        csvwriter.writerow(header)
        # Loop over each inner loop value and its data.
        for inner_val in inner_loop_values:
            inner_val_str = str(inner_val)
            if inner_val_str not in plot_data_stack:
                continue
            outer_labels_present = plot_data_stack[inner_val_str]['outer_labels']
            data_dict = plot_data_stack[inner_val_str]['data']
            aggregated_list = plot_data_stack[inner_val_str]['aggregated']
            for i, ol in enumerate(outer_labels_present):
                row = [inner_val_str, ol]
                for metric in stacked_metrics:
                    row.append(data_dict[metric][i])
                csvwriter.writerow(row)

    global_max = 0
    for val in plot_data_stack.values():
        if val['aggregated']:
            local_max = max(val['aggregated'])
            if local_max > global_max:
                global_max = local_max
    margin_factor = 1.2
    ymax = global_max * margin_factor if global_max > 0 else 1.0

    n_inner_val = len(inner_loop_values)
    ncols_stack = 3
    nrows_stack = math.ceil(n_inner_val / ncols_stack)
    fig, axs = plt.subplots(nrows_stack, ncols_stack, figsize=(1.7 * ncols_stack, 2.1 * nrows_stack), squeeze=True)
    if n_inner_val == 1:
        axs = [axs]
    else:
        axs = axs.flatten()

    for idx, inner_val in enumerate(inner_loop_values):
        ax = axs[idx]
        inner_val_str = str(inner_val)
        if inner_val_str not in plot_data_stack:
            ax.set_visible(False)
            continue
        outer_labels_present = plot_data_stack[inner_val_str]['outer_labels']
        data_dict = plot_data_stack[inner_val_str]['data']
        aggregated_list = plot_data_stack[inner_val_str]['aggregated']

        # Determine x labels
        if isinstance(outer_loop_param, list) and len(outer_loop_param) > 1:
            x_labels = outer_labels_present
            x_param_label = "Outer param"
        else:
            pattern = rf"{outer_loop_param}=([^\s,]+)"
            x_labels = [
                re.search(pattern, label).group(1) if re.search(pattern, label) else label
                for label in outer_labels_present
            ]
            x_param_label = outer_loop_param

        x = np.arange(len(x_labels))
        bar_width = 0.8
        bottoms = np.zeros(len(x_labels))
        for metric in stacked_metrics:
            values = np.array(data_dict[metric])
            ax.bar(x, values, bar_width, bottom=bottoms, color=colors[metric], label=alias_map[metric])
            bottoms += values

        ax.set_ylim(0, ymax)
        ax.set_xlabel(x_param_label, fontsize=xlabel_fontsize, labelpad=0)
        ax.set_ylabel('%', fontsize=ylabel_fontsize, labelpad=0)
        #ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.1f'))

        if inner_loop_param == "nUes":
            ax.set_title(r"$n_{\text{UE}}$ = " + str(inner_val_str), fontsize=title_fontsize, pad=1)
        else:
            ax.set_title(f"{inner_loop_param} = {inner_val_str}", fontsize=title_fontsize, pad=1)
        if outer_loop_param == "t2":
            ax.set_xlabel(r"$T_2$", fontsize=xlabel_fontsize)
        else:
            ax.set_xlabel(x_param_label, fontsize=xlabel_fontsize)
        ax.set_xticks(x)
        ax.set_xticklabels(x_labels, fontsize=xtick_fontsize)
        ax.tick_params(axis='y', labelsize=ytick_fontsize)

    for j in range(idx+1, len(axs)):
        fig.delaxes(axs[j])

    plt.tight_layout(rect=[0, 0.04, 1, 1], w_pad=0.2, h_pad=0.5)
    legend_handles = [Patch(facecolor=colors[metric], label=alias_map[metric]) for metric in stacked_metrics]
    fig.legend(legend_handles, [alias_map[metric] for metric in stacked_metrics],
            loc='lower center', ncol=len(stacked_metrics), fontsize=legend_fontsize, frameon=False,
            bbox_to_anchor=(0.5, 0.0))

    stacked_filename = f"{campaignName}-phy-loss-dist.png"
    plt.savefig(stacked_filename, bbox_inches='tight')
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
        pattern_inner = rf'(?:^|_){inner_loop_param}-(.*?)(?=_[a-zA-Z]+-|$)'
        matches_inner = re.findall(pattern_inner, folder_name)
        inner_value = matches_inner[-1] if matches_inner else None

        if outer_extracted is None or inner_value is None:
            print(f"Could not extract parameters from folder '{evalFolder}'")
            continue

        folder_mapping[(outer_label, inner_value)] = evalFolder



        metrics_file = os.path.join(evalFolder, 'route-change-stats-all-mean-95-ci.csv')
        if not os.path.isfile(metrics_file):
            print(f"Metrics file not found in {evalFolder}")
            continue

        df = pd.read_csv(metrics_file)
        for _, row in df.iterrows():
            metric = row['metricName']
            if metric not in metrics_data:
                metrics_data[metric] = []
            metrics_data[metric].append({
                "outer_label": outer_label,
                inner_loop_param: float(inner_value),
                'Mean': row['Mean'],
                '95%CI': row['95%CI']
            })

    # Plot error-bar charts for each flowsStats metric
    for metric, data in metrics_data.items():
        data_df = pd.DataFrame(data)
        data_csv_file = f"{campaignName}-{metric}-mean-95-ci.csv"
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
            y = subset['Mean']
            ci = subset['95%CI']
            plt.errorbar(x, y, yerr=ci, label=display_label, capsize=4, linewidth=0.8, marker='o', markersize=2)


        plt.xlabel(inner_loop_param)

        if inner_loop_param == "nUes":
            plt.xlabel(r"$n_{\text{UE}}$")
        else:
            plt.xlabel(inner_loop_param)

        if metric == "Mos":
            plt.ylabel("MOS")
            plt.ylim(2.45, 4.55)
            plt.yticks([ 2.5, 3, 3.5, 4, 4.5])  # or more if you want
            plt.axhline(4, linestyle='--', color='gray')  # <-- horizontal dashed line
        else:
            plt.ylabel(metric)

        plt.legend(fontsize=8.5, labelspacing=0.2, borderpad=0.3, handlelength=2)
        plt.grid(True)
        plot_file = f"{campaignName}-{metric}-mean-95-ci.png"
        plt.savefig(plot_file, bbox_inches='tight')
        plt.close()


# ------------------------------------------------------------------
# Main script entry point
# ------------------------------------------------------------------
def main():
    start_time = time.time()

    # Inital compilation
    print("Running ./ns3 build...")
    output = subprocess.run(["./ns3", "show", "config"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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
    nRuns = 5

    # Set maximum number of simultaneous processes
    nProcesses = 100

    # Default simulation parameters
    params = {
        "nUes": 6,
        "iud": 700,       # m
        "codec": "EVS_13.2",
        "mcs": 0,
        "maxNumTx": 1,
        "rtxType": "No",
        "wSlotEx": "true",
        "t2": 16,
    }

    outer_loop_param = 't2'
    outer_loop_values = [8,16]
    inner_loop_param = 'nUes'
    inner_loop_values = [3,4,5,6]
    campaignName = "z-test2"

    evalFolders = []
    # Launch evaluations: unpack outer values if necessary.
    if isinstance(outer_loop_param, list):
        for outer_value in outer_loop_values:
            for i, key in enumerate(outer_loop_param):
                params[key] = outer_value[i]
            params['campaignName'] = campaignName
            for inner_value in inner_loop_values:
                params[inner_loop_param] = inner_value
                evalFolder = start_evaluation(params, nRuns, nProcesses)
                evalFolders.append(evalFolder)
    else:
        for outer_value in outer_loop_values:
            params[outer_loop_param] = outer_value
            params['campaignName'] = campaignName
            for inner_value in inner_loop_values:
                params[inner_loop_param] = inner_value
                evalFolder = start_evaluation(params, nRuns, nProcesses)
                evalFolders.append(evalFolder)

    print(f"Processing CAMPAIGN {campaignName}")
    process_campaign(campaignName, outer_loop_param, outer_loop_values, inner_loop_param, inner_loop_values, evalFolders)

    end_time = time.time()
    duration_s = end_time - start_time
    duration_m = duration_s / 60.0
    print(f"The script took {duration_m:.2f} minutes to run.")

if __name__ == "__main__":
    main()



# # MILCOM 2025 eval parameters
#    nRuns = 1000

# #MCS 0, IUD = 800
#     params = {
#         "nUes": 6,
#         "iud": 800,       # m
#         "codec": "EVS_13.2",
#         "mcs": 0,
#         "maxNumTx": 1,
#         "rtxType": "No",
#         "wSlotEx": "true",
#         "t2": 16,
#     }
#     outer_loop_param = 't2'
#     outer_loop_values = [4,8,12,16]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD800-MCS0-ntx1-t2_vs-{inner_loop_param}"

# #MCS 0, IUD = 800 , Rtx
#     params = {
#         "nUes": 6,
#         "iud": 800,       # m
#         "codec": "EVS_13.2",
#         "mcs": 0,
#         "sensing": "true",
#         "maxNumTx": 1,
#         "rtxType": "Feedback1",
#         "wSlotEx": "true",
#         "t2": 16,
#     }

#     outer_loop_param = ["t2", "maxNumTx"]
#     outer_loop_values = [(8, 2), (12, 2), (12, 3), (16, 2), (16, 4)]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD800-MCS0-FB-t2_vs-{inner_loop_param}"

# #MCS 0, IUD = 880
#     params = {
#         "nUes": 6,
#         "iud": 880,       # m
#         "codec": "EVS_13.2",
#         "mcs": 0,
#         "sensing": "true",
#         "maxNumTx": 1,
#         "rtxType": "No",
#         "wSlotEx": "true",
#         "t2": 16,
#     }
#     outer_loop_param = 't2'
#     outer_loop_values = [4,8,12,16]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD880-MCS0-ntx1-t2_vs-{inner_loop_param}"

# #MCS 0, IUD = 880 , Rtx
#     params = {
#         "nUes": 6,
#         "iud": 880,       # m
#         "codec": "EVS_13.2",
#         "mcs": 0,
#         "sensing": "true",
#         "maxNumTx": 1,
#         "rtxType": "Feedback1",
#         "wSlotEx": "true",
#         "t2": 16,
#     }
#     outer_loop_param = ["t2", "maxNumTx"]
#     outer_loop_values = [(8, 2), (12, 2), (12, 3), (16, 2), (16, 4)]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD880-MCS0-FB-t2_vs-{inner_loop_param}"

# #MCS 2, IUD = 800, No slot exclusion
#     params = {
#         "nUes": 6,
#         "iud": 800,       # m
#         "codec": "EVS_13.2",
#         "mcs": 2,
#         "sensing": "true",
#         "maxNumTx": 1,
#         "rtxType": "No",
#         "wSlotEx": "false",
#         "t2": 16,
#     }
#     outer_loop_param = 't2'
#     outer_loop_values = [4,8,12,16]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD800-MCS2-ntx1-NoExcl-t2_vs-{inner_loop_param}"

# #MCS 2, IUD = 800
#     params = {
#         "nUes": 6,
#         "iud": 800,       # m
#         "codec": "EVS_13.2",
#         "mcs": 2,
#         "sensing": "true",
#         "maxNumTx": 1,
#         "rtxType": "No",
#         "wSlotEx": "true",
#         "t2": 16,
#     }
#     outer_loop_param = 't2'
#     outer_loop_values = [4,8,12,16]
#     inner_loop_param = 'nUes'
#     inner_loop_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
#     campaignName = f"milcom-IUD800-MCS2-ntx1-t2_vs-{inner_loop_param}"
