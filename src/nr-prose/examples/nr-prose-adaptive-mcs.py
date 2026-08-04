#!/usr/bin/env python3
"""
This is a description
"""

import sys
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import pandas as pd

# --- Configuration ---
MCS_FILE = "mcs-change-trace.csv"
MCPTT_MSG_STATS_FILE = "mcptt-msg-stats.txt"
VOIP_PACKET_TRACE_FILE = "voip-packet-trace.csv"
TS_TRIAL_RESULTS_FILE = "olla-trial-results.csv"


def parse_voip_packet_trace(path):
    voip_packet_trace = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            # AppId,TxTime(s),RxTime(s),Seq,Size,RxDelay(ms)
            app_id = int(parts[0])
            tx_time = float(parts[1])
            rx_time = float(parts[2])
            seq = float(parts[3])
            size = float(parts[4])
            rx_delay = float(parts[5])
            voip_packet_trace.append((app_id, tx_time, rx_time, seq, size, rx_delay))
    return voip_packet_trace


def parse_mcptt_msg_stats(path):
    mcptt_msg_stats = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            # time(s),nodeid,callid,ssrc,selected,rx/tx,bytes,message
            t = float(parts[0])
            node_id = int(parts[1])
            tx_rx = parts[5]
            size = int(parts[6])
            if node_id not in mcptt_msg_stats:
                mcptt_msg_stats[node_id] = []
            mcptt_msg_stats[node_id].append((t, tx_rx, size))
    return mcptt_msg_stats


def parse_mcs_changes(path):
    mcs_changes = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            # time(s),nodeid,dstL2Id,newMcs
            t = float(parts[0])
            node_id = int(parts[1])
            dst_l2_id = int(parts[2])
            new_mcs = int(parts[3])
            if node_id not in mcs_changes:
                mcs_changes[node_id] = []
            mcs_changes[node_id].append((t, dst_l2_id, new_mcs))
    return mcs_changes


def parse_ts_trial_results(path):
    ts_trial_results = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            # time(s),nodeid,dstL2Id,mcs,success
            t = float(parts[0])
            node_id = int(parts[1])
            dst_l2_id = int(parts[2])
            mcs = int(parts[3])
            success = int(parts[4])
            if node_id not in ts_trial_results:
                ts_trial_results[node_id] = []
            ts_trial_results[node_id].append((t, dst_l2_id, mcs, success))
    return ts_trial_results


def compute_throughput(voip_packet_trace):
    throughput = {}
    for tx_time, rx_time, seq, size, rx_delay in voip_packet_trace:
        sec = int(tx_time)
        if sec not in throughput:
            throughput[sec] = (0, 0)
        tx, rx = throughput[sec]
        tx += size
        rx += size
        throughput[sec] = (tx, rx)
    return throughput, sec


def compute_ts_trial_result_ratio(ts_trial_results):
    ratios = {}
    for node_id in ts_trial_results:
        for t, dst_l2_id, mcs, success in ts_trial_results[node_id]:
            if node_id not in ratios:
                ratios[node_id] = {}
            sec = int(t)
            if sec not in ratios[node_id]:
                ratios[node_id][sec] = (0, 0)
            acks, nacks = ratios[node_id][sec]
            if success == 1:
                acks += 1
            else:
                nacks += 1
            ratios[node_id][sec] = (acks, nacks)
    return ratios


def plot_throughput(throughput, last_sec):
    x = []
    y1 = []
    y2 = []
    for sec in range(0, last_sec + 1):
        tx = 0
        rx = 0
        x.append(sec)
        if sec in throughput:
            tx, rx = throughput[sec]
        y1.append(tx * 8 / 1000)
        y2.append(rx * 8 / 1000)
    plt.ylim(0, 1000)
    plt.xlim(0, 180)
    plt.title("Throughput")
    plt.scatter(x, y2, label="RX")
    plt.xlabel("Time (s)")
    plt.ylabel("Kbits/s")
    plt.legend()
    plt.savefig("throughput.png")
    plt.clf()
    plt.cla()


def plot_ts_trial_result_ratios(ratios, last_sec, node_id):
    x = []
    y1 = []
    y2 = []
    for sec in range(0, last_sec + 1):
        acks = 0
        nacks = 0
        x.append(sec)
        if sec in ratios[node_id]:
            acks, nacks = ratios[node_id][sec]
        y1.append(acks)
        y2.append(nacks)
    plt.ylim(0, 200)
    plt.xlim(0, 180)
    plt.bar(x, y1, label=str(node_id) + " ACKS")
    plt.bar(x, y2, bottom=y1, label=str(node_id) + " NACKS")
    plt.title("HARQ Trial Results")
    plt.xlabel("Time (s)")
    plt.ylabel("Count")
    plt.legend()
    plt.savefig("olla-trial-results.png")
    plt.clf()
    plt.cla()


def plot_mcs_changes(mcs_changes, last_sec, node_id):
    x = []
    y1 = []
    for t, dst_l2_id, new_mcs in mcs_changes[node_id]:
        x.append(t)
        y1.append(new_mcs)
    plt.ylim(0, 30)
    plt.xlim(0, 180)
    plt.scatter(x, y1, label=str(node_id) + " to " + str(dst_l2_id))
    plt.title("MCS Changes")
    plt.xlabel("Time (s)")
    plt.ylabel("MCS")
    plt.legend()
    plt.savefig("mcs-changes.png")
    plt.clf()
    plt.cla()


def main():
    node_id = 1
    voip_packet_trace = parse_voip_packet_trace(VOIP_PACKET_TRACE_FILE)
    mcptt_msg_stats = parse_mcptt_msg_stats(MCPTT_MSG_STATS_FILE)
    mcs_changes = parse_mcs_changes(MCS_FILE)
    ts_trial_results = parse_ts_trial_results(TS_TRIAL_RESULTS_FILE)

    throughput, last_sec = compute_throughput(voip_packet_trace)
    ratios = compute_ts_trial_result_ratio(ts_trial_results)

    plot_throughput(throughput, last_sec)
    plot_ts_trial_result_ratios(ratios, last_sec, node_id)
    plot_mcs_changes(mcs_changes, last_sec, node_id)


if __name__ == "__main__":
    main()
