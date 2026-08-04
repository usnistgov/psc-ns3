#!/usr/bin/env python3

# Description:
# This script processes the output simulation files of the
# nr-prose-multihop-advanced scenario.
#
# Prerequisites:
# - Python3 matplotlib, numpy, pandas
#
# Usage (from the ns3 root folder):
# python3 nr-prose-multihop-advanced-process-sim.py [path_to_folder]
#
# Program outputs:
# - [path_to_folder]/appId-{appId} folders containing for each application with ID {appId}:
#   - route_snapshot_t{t}_src{srcNodeId}_tgt{tgtNodeId}.png: Plot of the topology and the IPv4
#     route the packets of the application with traffic from node with node ID {srcNodeId} to
#     node with node ID {tgtNodeId} follow at time 't' when a new route was identified.
#   - voip-packet-trace-delay-cdf.png: The CDF of the VoIP packet delay for the application.
#   - voip-packet-trace-route-change-timeline.png: Timeline of the VoIP packet reception outcomes
#     and the periods without an IP route during simulation time.
# - [path_to_folder]/phy-stats-sim-total-distribution.png: Plot with the distribution of the PSSCH
#   reception outcomes at the PHY for each message type.


import glob
import math
import os
import sys
from textwrap import fill
from typing import Dict, List, Tuple

import matplotlib.gridspec as gridspec
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.lines import Line2D


def process_voip_and_routes(path):
    timeline_figure_width = 12
    timeline_figure_height = 3
    cdf_figure_width = 5
    cdf_figure_height = 3

    voip_files = glob.glob(os.path.join(path, "voip-packet-trace-*.csv"))
    if not voip_files:
        print("No voip-packet-trace-*.csv found.")
        sys.exit(1)

    def _appid_from(fp):
        base = os.path.basename(fp)
        return base.rsplit("-", 1)[-1].split(".")[0]

    app_ids = sorted(
        {_appid_from(f) for f in voip_files},
        key=lambda x: (str(x).isdigit(), int(x) if str(x).isdigit() else x),
    )

    for app_id in app_ids:
        out_dir = os.path.join(path, f"appId-{app_id}")
        os.makedirs(out_dir, exist_ok=True)

        voip_file = os.path.join(path, f"voip-packet-trace-{app_id}.csv")
        route_file = os.path.join(path, f"route-change-trace-{app_id}.csv")

        # ---- VoIP trace ----
        if not os.path.exists(voip_file):
            print(f"[appId={app_id}] Missing {os.path.basename(voip_file)}; skipping.")
            continue

        vp = pd.read_csv(voip_file)
        vp.columns = vp.columns.str.lstrip('#')
        required_voip_cols = {"TxTime(s)", "Size", "RxDelay(ms)"}
        if not required_voip_cols.issubset(vp.columns):
            print(f"[appId={app_id}] voip trace missing required columns; skipping.")
            continue

        # numeric coercions
        vp["TxTime(s)"] = pd.to_numeric(vp["TxTime(s)"], errors="coerce")
        vp["RxDelay(ms)"] = pd.to_numeric(vp["RxDelay(ms)"], errors="coerce")
        vp = vp.dropna(subset=["TxTime(s)"])
        if vp.empty:
            print(f"[appId={app_id}] Empty/invalid voip trace; skipping.")
            continue

        total = len(vp)
        rec = vp["RxDelay(ms)"].notna()
        lost = ~rec
        pct_rec = rec.sum() / total * 100
        pct_lost = lost.sum() / total * 100
        last_tx = vp["TxTime(s)"].max()

        # ---- Route trace → no-route intervals ----
        no_route_intervals = []
        if os.path.exists(route_file):
            df = pd.read_csv(route_file)
            df.columns = df.columns.str.lstrip('#')

            # Expect exact headers:
            # "Time(s)", "appId", "srcNodeId", "tgtNodeId", "route(NodeIds)"
            needed = {"Time(s)", "route(NodeIds)"}
            if not needed.issubset(df.columns):
                # If headers don’t match, treat entire span as unknown route state
                no_route_intervals.append((0.0, last_tx))
            else:
                # Coerce and clean
                df = df.copy()
                df["Time(s)"] = pd.to_numeric(df["Time(s)"], errors="coerce")
                df = df.dropna(subset=["Time(s)"])
                df = df.sort_values("Time(s)")

                def _is_no_route(v):
                    if pd.isna(v):
                        return True
                    if isinstance(v, str):
                        s = v.strip().lower()
                        return s in ("nan", "loop", "")
                    return False

                times = df["Time(s)"].to_numpy()
                flags = df["route(NodeIds)"].apply(_is_no_route).to_numpy()

                if df.empty:
                    no_route_intervals.append((0.0, last_tx))
                else:
                    in_gap = False
                    start = None
                    for t, gap in zip(times, flags):
                        if gap and not in_gap:
                            start = t
                            in_gap = True
                        elif not gap and in_gap:
                            no_route_intervals.append((start, t))
                            in_gap = False
                            start = None
                    if in_gap:
                        no_route_intervals.append((start, last_tx))
        # else: keep no_route_intervals empty (no shading)

        # ---- Timeline plot ----
        fig = plt.figure(figsize=(timeline_figure_width, timeline_figure_height))
        ax = fig.add_axes([0.15, 0.15, 0.75, 0.75])

        for i, (s, e) in enumerate(no_route_intervals):
            ax.axvspan(
                s,
                e,
                color="gray",
                alpha=0.3,
                label="No route" if i == 0 else None,
                zorder=0,
            )

        ax.scatter(
            vp.loc[rec, "TxTime(s)"],
            vp.loc[rec, "Size"],
            color="blue",
            marker="o",
            label=f"VoIP packets received ({pct_rec:.1f}%)",
            zorder=1,
        )
        ax.scatter(
            vp.loc[lost, "TxTime(s)"],
            vp.loc[lost, "Size"],
            color="red",
            marker="x",
            label=f"VoIP packets lost ({pct_lost:.1f}%)",
            zorder=1,
        )

        ax.set_xlabel("Tx Time (s)")
        ax.set_ylabel("Packet Size (Bytes)")
        ax.set_title(f"VoIP Packets Timeline (appId={app_id})")
        ax.legend(
            loc="upper center", bbox_to_anchor=(0.5, -0.25), ncol=3, borderaxespad=0
        )
        plt.savefig(
            os.path.join(out_dir, "voip-packet-trace-route-change-timeline.png"),
            bbox_inches="tight",
        )
        plt.close()

        # ---- CDF plot ----
        rx_clean = vp["RxDelay(ms)"].dropna()
        if len(rx_clean) > 0:
            rx_sorted = np.sort(rx_clean)
            cdf_vals = np.arange(1, len(rx_sorted) + 1) / len(rx_sorted)
            mean_delay = rx_clean.mean()
            tail98 = np.percentile(rx_clean, 98)

            fig = plt.figure(figsize=(cdf_figure_width, cdf_figure_height))
            ax = fig.add_axes([0.15, 0.15, 0.75, 0.75])
            ax.plot(rx_sorted, cdf_vals)
            ax.axvline(
                mean_delay,
                color="blue",
                linestyle="--",
                label=f"Mean = {mean_delay:.2f} ms",
            )
            ax.axvline(
                tail98,
                color="orange",
                linestyle="--",
                label=f"98% Tail = {tail98:.2f} ms",
            )
            ax.set_xlabel("Rx Delay (ms)")
            ax.set_ylabel("CDF")
            ax.set_title(f"VoIP Packets Rx Delay CDF (appId={app_id})")
            ax.legend(bbox_to_anchor=(1.02, 1), loc="upper left", borderaxespad=0)
            plt.savefig(
                os.path.join(out_dir, "voip-packet-trace-delay-cdf.png"),
                bbox_inches="tight",
            )
            plt.close()
        else:
            print(f"[appId={app_id}] No valid rxDelay data. Skipping CDF.")


def process_node_positions_direct_links_and_routes(path: str):
    """
    Per appId, render one snapshot per row of route-change-trace-{appId}.csv.
    Each snapshot time t:
      - Draw current topology from direct-links-trace.csv (latest event per directed pair <= t)
          Established -> solid black arrow
          Released    -> dashed light-gray arrow
      - Overlay the route path in a distinct color on top
    """
    route_files = glob.glob(os.path.join(path, "route-change-trace-*.csv"))
    if not route_files:
        print("No route-change-trace-*.csv found.")
        return

    def _appid_from(fname: str) -> str:
        base = os.path.basename(fname)
        return base.rsplit("-", 1)[-1].split(".")[0]

    app_ids = sorted(
        {_appid_from(f) for f in route_files},
        key=lambda x: (str(x).isdigit(), int(x) if str(x).isdigit() else x),
    )

    # ---- Load node positions (headers as-is) ----
    pos_file = os.path.join(path, "node-position-trace.csv")
    if not os.path.exists(pos_file):
        print(f"Missing {pos_file}")
        return

    df_pos = pd.read_csv(pos_file)
    df_pos.columns = df_pos.columns.str.lstrip('#')
    needed_pos = {"Time(s)", "nodeId", "xPos(m)", "yPos(m)"}
    if not needed_pos.issubset(df_pos.columns):
        raise ValueError(
            f"{pos_file} missing columns: {needed_pos - set(df_pos.columns)}"
        )

    df_pos = df_pos.copy()
    df_pos["Time(s)"] = pd.to_numeric(df_pos["Time(s)"], errors="coerce")
    df_pos = df_pos.dropna(subset=["Time(s)"])
    df_pos = df_pos.sort_values(["nodeId", "Time(s)"], kind="mergesort")

    per_node: Dict[int, pd.DataFrame] = {
        int(nid): grp.reset_index(drop=True)
        for nid, grp in df_pos.groupby("nodeId", sort=True)
    }
    node_ids = sorted(per_node.keys())

    # Global bounds for consistent axes
    x_all = df_pos["xPos(m)"].to_numpy()
    y_all = df_pos["yPos(m)"].to_numpy()
    xmin, xmax = float(np.min(x_all)), float(np.max(x_all))
    ymin, ymax = float(np.min(y_all)), float(np.max(y_all))
    pad_x = (xmax - xmin) * 0.1 or 5.0
    pad_y = (ymax - ymin) * 0.1 or 5.0
    xlim = (xmin - pad_x, xmax + pad_x)
    ylim = (ymin - pad_y, ymax + pad_y)

    def pos_at_time(nid: int, t: float) -> Tuple[float, float] | None:
        grp = per_node.get(nid)
        if grp is None or grp.empty:
            return None
        times = grp["Time(s)"].to_numpy()
        idx = times.searchsorted(t, side="right") - 1
        if idx >= 0:
            row = grp.iloc[idx]
            return float(row["xPos(m)"]), float(row["yPos(m)"])
        return None

    # ---- Load direct link events once (headers as-is) ----
    links_file = os.path.join(path, "direct-links-trace.csv")
    have_links = os.path.exists(links_file)
    if have_links:
        df_links = pd.read_csv(links_file)
        df_links.columns = df_links.columns.str.lstrip('#')
        needed_links = {"time(s)", "selfL2Id", "peerL2Id", "status"}
        if not needed_links.issubset(df_links.columns):
            print(
                f"{links_file} missing columns: {needed_links - set(df_links.columns)}; topology overlay disabled."
            )
            have_links = False
        else:
            df_links = df_links.copy()
            df_links["time(s)"] = pd.to_numeric(df_links["time(s)"], errors="coerce")
            df_links = df_links.dropna(subset=["time(s)"])
            df_links["selfL2Id"] = pd.to_numeric(
                df_links["selfL2Id"], errors="coerce"
            ).astype("Int64")
            df_links["peerL2Id"] = pd.to_numeric(
                df_links["peerL2Id"], errors="coerce"
            ).astype("Int64")
            df_links = df_links.dropna(subset=["selfL2Id", "peerL2Id"])
            df_links["selfL2Id"] = df_links["selfL2Id"].astype(int)
            df_links["peerL2Id"] = df_links["peerL2Id"].astype(int)

            def norm_status(s: str) -> str:
                if isinstance(s, str):
                    st = s.strip().lower()
                    if st in ("established", "establish", "up"):
                        return "Established"
                    if st in ("released", "release", "down"):
                        return "Released"
                return "Released"

            df_links["status"] = df_links["status"].apply(norm_status)
            df_links = df_links.sort_values("time(s)").reset_index(drop=True)
            events_t = df_links["time(s)"].to_numpy()
            events_self = df_links["selfL2Id"].to_numpy()
            events_peer = df_links["peerL2Id"].to_numpy()
            events_stat = df_links["status"].to_numpy()

    # Consistent colors
    node_color = "C0"  # default matplotlib blue for scatter
    path_color = "C3"  # route overlay color

    for app_id in app_ids:
        out_dir = os.path.join(path, f"appId-{app_id}")
        os.makedirs(out_dir, exist_ok=True)
        for f in glob.glob(
            os.path.join(out_dir, "route_snapshot_*.png")
        ):  # Remove old plots if any
            try:
                os.remove(f)
            except OSError:
                pass
        route_file = os.path.join(path, f"route-change-trace-{app_id}.csv")
        if not os.path.exists(route_file):
            print(f"[appId={app_id}] Missing {os.path.basename(route_file)}; skipping.")
            continue

        df_routes = pd.read_csv(route_file)
        df_routes.columns = df_routes.columns.str.lstrip('#')
        needed_routes = {"Time(s)", "srcNodeId", "tgtNodeId", "route(NodeIds)"}
        if not needed_routes.issubset(df_routes.columns):
            print(
                f"[appId={app_id}] Route file missing columns: {needed_routes - set(df_routes.columns)}; skipping."
            )
            continue

        df_routes = df_routes.copy()
        # to_numeric() converts the time column to numbers, and invalid values will be 'NaN'.
        # This file has a few additional comment lines and the time column for those will be
        # a word preceded by '#' (an invalid value)
        df_routes["Time(s)"] = pd.to_numeric(df_routes["Time(s)"], errors="coerce")
        # remove rows where the time column was 'NaN', thereby removing the comment lines
        df_routes = df_routes.dropna(subset=["Time(s)"])
        # sort DataFrame by time in ascending order
        df_routes = df_routes.sort_values("Time(s)")

        # link state while time increases
        edge_state: Dict[Tuple[int, int], str] = {}
        ev_idx = 0
        n_events = len(df_links) if have_links else 0

        for _, r in df_routes.iterrows():
            t = float(r["Time(s)"])
            try:
                src = int(r["srcNodeId"])
                tgt = int(r["tgtNodeId"])
            except Exception:
                continue

            raw_route = r.get("route(NodeIds)", None)
            # Parse route for overlay; treat NaN/empty/"loop" as no-route
            route_nodes: List[int] = []
            if isinstance(raw_route, str):
                s = raw_route.strip()
                if s and s.lower() not in ("nan", "loop"):
                    try:
                        route_nodes = [
                            int(tok.strip()) for tok in s.split("->") if tok.strip()
                        ]
                    except ValueError:
                        route_nodes = []

            # Title line 2 string
            if isinstance(raw_route, str) and raw_route.strip():
                route_str_for_title = raw_route.strip()
            else:
                route_str_for_title = "No route"

            # positions ≤ t for all nodes
            xs, ys, ids = [], [], []
            pos_cache: Dict[int, Tuple[float, float]] = {}
            for nid in node_ids:
                p = pos_at_time(nid, t)
                if p is not None:
                    pos_cache[nid] = p
                    xs.append(p[0])
                    ys.append(p[1])
                    ids.append(nid)

            fig, ax = plt.subplots(figsize=(6.5, 6.2))

            # 1) Topology up to t
            if have_links and n_events > 0:
                while ev_idx < n_events and events_t[ev_idx] <= t:
                    edge_state[(int(events_self[ev_idx]), int(events_peer[ev_idx]))] = (
                        str(events_stat[ev_idx])
                    )
                    ev_idx += 1

                for (u, v), st in edge_state.items():
                    if u in pos_cache and v in pos_cache:
                        (x1, y1) = pos_cache[u]
                        (x2, y2) = pos_cache[v]
                        if st == "Established":
                            arrowprops = dict(
                                arrowstyle="->", linewidth=1.8, color="black"
                            )
                        else:
                            arrowprops = dict(
                                arrowstyle="->",
                                linewidth=1.2,
                                color="lightgray",
                                linestyle="--",
                            )
                        ax.annotate(
                            "",
                            xy=(x2, y2),
                            xytext=(x1, y1),
                            arrowprops=arrowprops,
                            zorder=1,
                        )

            # 2) All nodes
            ax.scatter(xs, ys, color=node_color, zorder=2)

            # 3) Route overlay (lines + path nodes)
            if len(route_nodes) >= 2:
                for a, b in zip(route_nodes, route_nodes[1:]):
                    if a in pos_cache and b in pos_cache:
                        (x1, y1) = pos_cache[a]
                        (x2, y2) = pos_cache[b]
                        ax.plot(
                            [x1, x2],
                            [y1, y2],
                            linewidth=2.8,
                            color=path_color,
                            zorder=4,
                        )

            if route_nodes:
                px, py = [], []
                for nid in route_nodes:
                    if nid in pos_cache:
                        x, y = pos_cache[nid]
                        px.append(x)
                        py.append(y)
                if px:
                    ax.scatter(px, py, color=path_color, s=46, zorder=5)

            # 4) Draw node labels LAST in the SAME blue as nodes
            for x, y, nid in zip(xs, ys, ids):
                ax.annotate(
                    str(nid),
                    (x, y),
                    textcoords="offset points",
                    xytext=(4, 4),
                    fontsize=9,
                    color=node_color,
                    zorder=6,
                )

            # Cosmetics
            ax.set_title(
                f"Route appId={app_id}: {src} → {tgt}  @ t={t:.2f}s\n Route: {route_str_for_title}"
            )
            ax.set_xlabel("x (m)")
            ax.set_ylabel("y (m)")
            ax.set_aspect("equal", adjustable="box")
            ax.grid(True, linestyle="--", alpha=0.35)
            ax.set_xlim(*xlim)
            ax.set_ylim(*ylim)

            # ---- Legend (below, outside) ----
            # Proxies for legend
            node_proxy = Line2D(
                [], [], marker="o", linestyle="None", color=node_color, label="Node"
            )
            pathnode_proxy = Line2D(
                [],
                [],
                marker="o",
                linestyle="None",
                color=path_color,
                label="Node in route",
            )
            route_proxy = Line2D([], [], color=path_color, lw=2.8, label="Route")
            est_proxy = Line2D([], [], color="black", lw=1.8, label="Established")
            rel_proxy = Line2D(
                [], [], color="lightgray", lw=1.2, linestyle="--", label="Released"
            )

            fig.legend(
                [est_proxy, rel_proxy, route_proxy, node_proxy, pathnode_proxy],
                ["Established", "Released", "Route", "Node", "Node in route"],
                loc="lower center",
                ncol=5,
                framealpha=0.95,
            )

            fig.subplots_adjust(bottom=0.14)  # leave space for legend
            outfile = os.path.join(
                out_dir, f"route_snapshot_t{t:.2f}_src{src}_tgt{tgt}.png"
            )
            fig.savefig(outfile, dpi=150)
            plt.close(fig)


def plot_topology_evolution(path: str, timeStep: int):
    """
    Render topology snapshots every `timeStep` ms:
      - Nodes drawn at last known positions <= t from node-position-trace.csv
      - Directed links from direct-links-trace.csv:
          Established -> solid black arrow
          Released    -> dashed light-gray arrow
      - Each frame shows:
          * count of established links
          * sorted list of established links (u->v) at that t

    Outputs saved to: {path}/topology-evolution-{timeStep}ms/topology_t{t:.3f}s.png
    """

    # ----------- Load node positions -----------
    pos_file = os.path.join(path, "node-position-trace.csv")
    if not os.path.exists(pos_file):
        print(f"Missing {pos_file}")
        return

    df_pos = pd.read_csv(pos_file)
    df_pos.columns = df_pos.columns.str.lstrip('#')
    needed_pos = {"Time(s)", "nodeId", "xPos(m)", "yPos(m)"}
    if not needed_pos.issubset(df_pos.columns):
        raise ValueError(
            f"{pos_file} missing columns: {needed_pos - set(df_pos.columns)}"
        )

    df_pos = df_pos.copy()
    df_pos["Time(s)"] = pd.to_numeric(df_pos["Time(s)"], errors="coerce")
    df_pos = df_pos.dropna(subset=["Time(s)"])
    df_pos = df_pos.sort_values(["nodeId", "Time(s)"], kind="mergesort")

    per_node: Dict[int, pd.DataFrame] = {
        int(nid): grp.reset_index(drop=True)
        for nid, grp in df_pos.groupby("nodeId", sort=True)
    }

    def pos_at_time(nid: int, t: float) -> Tuple[float, float] | None:
        grp = per_node.get(nid)
        if grp is None or grp.empty:
            return None
        times = grp["Time(s)"].to_numpy()
        idx = times.searchsorted(t, side="right") - 1
        if idx >= 0:
            row = grp.iloc[idx]
            return float(row["xPos(m)"]), float(row["yPos(m)"])
        return None

    # Global bounds
    x_all = df_pos["xPos(m)"].to_numpy()
    y_all = df_pos["yPos(m)"].to_numpy()
    xmin, xmax = float(np.min(x_all)), float(np.max(x_all))
    ymin, ymax = float(np.min(y_all)), float(np.max(y_all))
    pad_x = (xmax - xmin) * 0.1 or 5.0
    pad_y = (ymax - ymin) * 0.1 or 5.0
    xlim = (xmin - pad_x, xmax + pad_x)
    ylim = (ymin - pad_y, ymax + pad_y)

    # ----------- Load direct links -----------
    links_file = os.path.join(path, "direct-links-trace.csv")
    if not os.path.exists(links_file):
        print(f"Missing {links_file}")
        return

    df_links = pd.read_csv(links_file)
    df_links.columns = df_links.columns.str.lstrip('#')
    needed_links = {"time(s)", "selfL2Id", "peerL2Id", "status"}
    if not needed_links.issubset(df_links.columns):
        raise ValueError(
            f"{links_file} missing columns: {needed_links - set(df_links.columns)}"
        )

    df_links["time(s)"] = pd.to_numeric(df_links["time(s)"], errors="coerce")
    df_links = df_links.dropna(subset=["time(s)"])
    df_links["selfL2Id"] = pd.to_numeric(df_links["selfL2Id"], errors="coerce").astype(
        int
    )
    df_links["peerL2Id"] = pd.to_numeric(df_links["peerL2Id"], errors="coerce").astype(
        int
    )

    def norm_status(s: str) -> str:
        if isinstance(s, str):
            st = s.strip().lower()
            if st in ("established", "establish", "up"):
                return "Established"
            if st in ("released", "release", "down"):
                return "Released"
        return "Released"

    df_links["status"] = df_links["status"].apply(norm_status)

    # First Established
    df_est = df_links[df_links["status"] == "Established"]
    if df_est.empty:
        print("No Established events in trace.")
        return
    t_first = float(df_est["time(s)"].min())

    step_s = timeStep / 1000.0
    t_max = float(max(df_pos["Time(s)"].max(), df_links["time(s)"].max()))
    times = np.arange(t_first, t_max + step_s * 0.5, step_s)

    # Prepare event stream
    df_links = df_links.sort_values("time(s)").reset_index(drop=True)
    events_t = df_links["time(s)"].to_numpy()
    events_self = df_links["selfL2Id"].to_numpy()
    events_peer = df_links["peerL2Id"].to_numpy()
    events_stat = df_links["status"].to_numpy()

    edge_state: Dict[Tuple[int, int], str] = {}
    ev_idx, n_events = 0, len(df_links)

    # Output folder
    out_dir = os.path.join(path, f"topology-evolution-{timeStep}ms")
    os.makedirs(out_dir, exist_ok=True)

    # Legend handles
    handles = [
        Line2D([0], [0], color="black", lw=1.8, label="Established"),
        Line2D([0], [0], color="lightgray", lw=1.2, linestyle="--", label="Released"),
    ]

    for t in times:
        # apply events up to t (unchanged)
        while ev_idx < n_events and events_t[ev_idx] <= t:
            edge_state[(int(events_self[ev_idx]), int(events_peer[ev_idx]))] = str(
                events_stat[ev_idx]
            )
            ev_idx += 1

        # positions at time t (unchanged)
        pos_cache = {nid: pos_at_time(nid, t) for nid in per_node.keys()}
        pos_cache = {nid: p for nid, p in pos_cache.items() if p is not None}
        xs = [p[0] for p in pos_cache.values()]
        ys = [p[1] for p in pos_cache.values()]
        ids = list(pos_cache.keys())

        # established list (unchanged)
        established_edges = [
            (u, v) for (u, v), st in edge_state.items() if st == "Established"
        ]
        established_edges.sort()
        established_list = [f"{u}->{v}" for (u, v) in established_edges]
        est_count = len(established_edges)

        # ---------- LAYOUT: wider right panel + wrapped text ----------
        fig = plt.figure(figsize=(11, 6))
        # More space on the right (about 35–40% of width)
        gs = gridspec.GridSpec(
            nrows=1, ncols=2, figure=fig, width_ratios=[3.0, 2.0], wspace=0
        )

        ax = fig.add_subplot(gs[0, 0])  # main topology
        ax_info = fig.add_subplot(gs[0, 1])  # textbox panel
        ax_info.axis("off")

        # draw arrows
        for (u, v), st in edge_state.items():
            if u in pos_cache and v in pos_cache:
                (x1, y1), (x2, y2) = pos_cache[u], pos_cache[v]
                arrowprops = dict(
                    arrowstyle="->",
                    linewidth=1.8 if st == "Established" else 1.2,
                    color="black" if st == "Established" else "lightgray",
                    linestyle="-" if st == "Established" else "--",
                )
                ax.annotate(
                    "", xy=(x2, y2), xytext=(x1, y1), arrowprops=arrowprops, zorder=2
                )

        # nodes
        ax.scatter(xs, ys, zorder=3)
        for x, y, nid in zip(xs, ys, ids):
            ax.annotate(
                str(nid),
                (x, y),
                textcoords="offset points",
                xytext=(4, 4),
                fontsize=9,
                zorder=4,
            )

        # axes cosmetics
        ax.set_title(f"Topology @ t={t:.3f}s (Δt={timeStep}ms)")
        ax.set_xlim(*xlim)
        ax.set_ylim(*ylim)
        ax.set_aspect("equal", adjustable="box")
        ax.grid(True, linestyle="--", alpha=0.35)

        # figure-level legend BELOW the plot
        fig.legend(
            handles=[
                Line2D([0], [0], color="black", lw=1.8, label="Established"),
                Line2D(
                    [0],
                    [0],
                    color="lightgray",
                    lw=1.2,
                    linestyle="--",
                    label="Released",
                ),
            ],
            loc="lower center",
            ncol=2,
            framealpha=0.95,
        )

        # Build wrapped textbox text
        if established_list:
            links_str = ", ".join(established_list)
            wrapped_links = fill(links_str, width=50)  # adjust width if needed
            info = f"Established: {est_count}\n{wrapped_links}"
        else:
            info = "Established: 0"

        # Put textbox inside its own panel (won't be clipped)
        ax_info.text(
            0.0,
            1.0,
            info,
            va="top",
            ha="left",
            fontsize=8,
            bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.85),
        )

        # Leave space at bottom for legend
        fig.subplots_adjust(bottom=0.14)

        outfile = os.path.join(out_dir, f"topology_t{t:.3f}s.png")
        fig.savefig(outfile, dpi=150)
        plt.close(fig)


def process_phy_loss_distribution(path: str):
    """
    Reads <path>/phy-stats-sim-total.csv and generates a figure with one subplot per Type.
    Each subplot shows a stacked bar with losses (default Matplotlib colors).
    Each subplot has its own legend just below it:
      - Total transmissions
      - Total losses (sum of loss categories, with percentage)
      - Corrupted, Half-duplex, Not expected (counts + percentage)

    Output: <path>/phy-stats-sim-total-distribution.png
    """

    infile = os.path.join(path, "phy-stats-sim-total.csv")
    if not os.path.exists(infile):
        print(f"Missing {infile}")
        return

    df = pd.read_csv(infile)
    df.columns = df.columns.str.lstrip('#')

    agg_cols = [
        "nTxData",
        "nRxData",
        "nRxDataCorr",
        "nRxDataHd",
        "nRxDataNotExp",
        "nRxDataAlrDec",
    ]
    g = (
        df.groupby("Type", dropna=False)[agg_cols]
        .sum(min_count=1)
        .reset_index()
        .sort_values("Type")
        .reset_index(drop=True)
    )

    n_types = len(g)
    if n_types == 0:
        print("No rows to plot.")
        return

    # Layout: up to 4 columns per row
    ncols = 2 if n_types >= 4 else n_types
    nrows = math.ceil(n_types / ncols)

    fig, axes = plt.subplots(
        nrows=nrows, ncols=ncols, figsize=(4.2 * ncols, 4.5 * nrows), squeeze=False
    )

    parts = [
        ("nRxDataCorr", "Corrupted"),
        ("nRxDataHd", "Half-duplex"),
        ("nRxDataNotExp", "Not expected (no PSCCH)"),
    ]

    for idx, (_, row) in enumerate(g.iterrows()):
        r, c = divmod(idx, ncols)
        ax = axes[r][c]

        n_tx = float(row["nTxData"]) if pd.notna(row["nTxData"]) else 0.0
        heights = [float(row[col]) if pd.notna(row[col]) else 0.0 for col, _ in parts]
        total_losses = sum(heights)

        # Plot stacked bar
        x = [0]
        bottom = 0.0
        bars = []
        for h in heights:
            b = ax.bar(x, [h], bottom=bottom)  # default colors
            bars.append(b[0])
            bottom += h

        ax.set_xticks([])
        ax.set_ylabel("Count")
        ax.set_title(f"PSSCH Loss distribution\n{row['Type']} packets")

        def pct(val):
            return f"{(val/n_tx)*100:.1f}%" if n_tx > 0 else "0.0%"

        handles, labels = [], []

        # Total transmissions
        handles.append(plt.Line2D([], [], alpha=0))
        labels.append(f"Total transmissions: {int(n_tx):,}")

        # Total losses
        handles.append(plt.Line2D([], [], alpha=0))
        labels.append(f"Total losses: {int(total_losses):,} ({pct(total_losses)})")

        # Each component
        for bar, (col, alias) in zip(bars, parts):
            val = float(row[col]) if pd.notna(row[col]) else 0.0
            handles.append(bar)
            labels.append(f"{alias}: {int(val):,} ({pct(val)})")

        # Legend just below this axes
        ax.legend(
            handles,
            labels,
            loc="upper center",
            bbox_to_anchor=(0.5, -0.20),
            ncol=1,
            fontsize=10,
            framealpha=0.9,
        )

        ymax = min(bottom, n_tx)
        ax.set_ylim(0, ymax * 1.15 if ymax > 0 else 1)

    # Hide unused subplots
    for j in range(n_types, nrows * ncols):
        axes[j // ncols][j % ncols].axis("off")

    fig.tight_layout()
    outfile = os.path.join(path, "phy-stats-sim-total-distribution.png")
    fig.savefig(outfile, dpi=150)
    plt.close(fig)


# MAIN SCRIPT
if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Usage: python script.py [path_to_folder]")
        sys.exit(1)
    elif len(sys.argv) == 2:
        path = sys.argv[1]
    else:
        path = os.getcwd()

    process_voip_and_routes(path)
    process_node_positions_direct_links_and_routes(path)
    process_phy_loss_distribution(path)
    # The below function is slow and the outputs takes some disk space, comment if doing statistical eval
#    plot_topology_evolution(path,500)
