#!/usr/bin/env python3

# Description:
# This script processes the output simulation files of the
# nr-prose-multihop-linear scenario.
#
# Prerequisites:
# - Python3 matplotlib, numpy, pandas
#
# Usage (from the ns3 root folder):
# python3 nr-prose-multihop-linear-process-sim.py [path_to_folder]
#
# Program outputs:
# 1) [path_to_folder]/voip-packet-trace-delay-cdf.png: The CDF of the
#    VoIP packet delay.
# 2) [path_to_folder]/voip-packet-trace-route-change-timeline.png: Timeline
#    of the VoIP packet reception outcomes and the periods without an IP route
#    during simulation time.


import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def process_sim(path):
    # Hardcoded figure size parameters
    timeline_figure_width = 12
    timeline_figure_height = 3
    cdf_figure_width = 5
    cdf_figure_height = 3

    # --- Read VoIP packet trace ---
    voip_file = os.path.join(path, 'voip-packet-trace.csv')
    if not os.path.exists(voip_file):
        print("No voip-packet-trace.csv found.")
        sys.exit(1)
    voice_packets = pd.read_csv(voip_file)
    voice_packets['RxDelay(ms)'] = pd.to_numeric(voice_packets['RxDelay(ms)'], errors='coerce')

    # --- Classify packets ---
    total = len(voice_packets)
    rec = voice_packets['RxDelay(ms)'].notna()
    lost = ~rec
    pct_rec = rec.sum() / total * 100 if total else 0
    pct_lost = lost.sum() / total * 100 if total else 0

    # --- Read Route-Change trace and compute "no-route" intervals ---
    route_file = os.path.join(path, 'route-change-trace.csv')
    no_route_intervals = []
    # compute last Tx so we know how far to shade if needed
    last_tx = voice_packets['TxTime(s)'].max()

    if os.path.exists(route_file):
        df = pd.read_csv(route_file)
        # normalize literal "NaN" strings
        if 'route(NodeIds)' in df:
            df['route(NodeIds)'] = df['route(NodeIds)'].replace('NaN', np.nan)

        # if there are absolutely no entries, shade entire timeline
        if df.empty:
            no_route_intervals.append((0.0, last_tx))
        else:
            df = df.sort_values('Time(s)')
            times  = df['Time(s)'].to_numpy()
            is_nan = df['route(NodeIds)'].isna().to_numpy()

            in_gap = False
            for t, nan_flag in zip(times, is_nan):
                if nan_flag and not in_gap:
                    # gap starts
                    start = t
                    in_gap = True
                elif not nan_flag and in_gap:
                    # gap ends
                    no_route_intervals.append((start, t))
                    in_gap = False

            # if still in a gap at end, extend to last Tx
            if in_gap:
                no_route_intervals.append((start, last_tx))

    # --- Timeline plot ---
    fig = plt.figure(figsize=(timeline_figure_width, timeline_figure_height))
    ax  = fig.add_axes([0.15, 0.15, 0.75, 0.75])

    # Shade "no route" intervals behind everything
    for i, (s, e) in enumerate(no_route_intervals):
        ax.axvspan(s, e,
                   color='gray',
                   alpha=0.3,
                   label='No route' if i == 0 else None,
                   zorder=0)

    # Scatter received vs lost
    ax.scatter(voice_packets.loc[rec, 'TxTime(s)'],
               voice_packets.loc[rec, 'Size'],
               color='blue', marker='o',
               label=f'VoIP packets received ({pct_rec:.1f}%)',
               zorder=1)
    ax.scatter(voice_packets.loc[lost, 'TxTime(s)'],
               voice_packets.loc[lost, 'Size'],
               color='red', marker='x',
               label=f'VoIP packets lost ({pct_lost:.1f}%)',
               zorder=1)

    ax.set_xlabel('Tx Time (s)')
    ax.set_ylabel('Packet Size (Bytes)')
    ax.set_title('VoIP Packets Timeline')
    ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.25), ncol=3, borderaxespad=0)

    plt.savefig(os.path.join(path, 'voip-packet-trace-route-change-timeline.png'),
                bbox_inches='tight')
    plt.close()

    # --- CDF plot ---
    rx_clean = voice_packets['RxDelay(ms)'].dropna()
    if len(rx_clean) == 0:
        print(f"{path}: No valid rxDelay data. Skipping CDF.")
        return

    rx_sorted = np.sort(rx_clean)
    cdf_vals = np.arange(1, len(rx_sorted) + 1) / len(rx_sorted)
    mean_delay = rx_clean.mean()
    tail98 = np.percentile(rx_clean, 98)

    fig = plt.figure(figsize=(cdf_figure_width, cdf_figure_height))
    ax  = fig.add_axes([0.15, 0.15, 0.75, 0.75])
    ax.plot(rx_sorted, cdf_vals)
    ax.axvline(mean_delay, color='blue', linestyle='--', label=f'Mean = {mean_delay:.2f} ms')
    ax.axvline(tail98,      color='orange', linestyle='--', label=f'98% Tail = {tail98:.2f} ms')
    ax.set_xlabel('Rx Delay (ms)')
    ax.set_ylabel('CDF')
    ax.set_title('VoIP Packets Rx Delay CDF')
    ax.legend(bbox_to_anchor=(1.02, 1), loc='upper left', borderaxespad=0)
    plt.savefig(os.path.join(path, 'voip-packet-trace-delay-cdf.png'),
                bbox_inches='tight')
    plt.close()

# MAIN SCRIPT
if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Usage: python script.py [path_to_folder]")
        sys.exit(1)
    elif len(sys.argv) == 2:
        path = sys.argv[1]
    else:
        path = os.getcwd()

    process_sim(path)
