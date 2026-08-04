#!/usr/bin/python3

# TODO:
# - Check boundary conditions (at t0, t1, t2)
# - Document this program

import argparse
import pathlib
import sys

import matplotlib
import numpy as np
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("--node", help="Node number (required)")
parser.add_argument("--slot", help="Slot number (required)")
parser.add_argument("--file", help="File name", default="sl-basic-example-scheduling.csv")
parser.add_argument("--list", help="List available nodes and slots, and exit", action="store_true")
parser.add_argument(
    "--show", help="Show plot instead of saving png", action="store_true", default=False
)
args = parser.parse_args()

if not args.show:
    matplotlib.use("Agg")

if args.list:
    slots = []
    nodes = {}
    with open(args.file) as f:
        for line in f:
            if line.startswith("#Trace"):
                col = line.split(",")
                slot = int(col[2].split("=", 1)[1])
                node = int(col[3].split("=", 1)[1])
                nodes[node] = node
                slots.append(slot)
                continue
    print("Slots available to print: %s" % slots)
    print("Nodes available to print: %s" % list(nodes.keys()))
    sys.exit()

if args.node is None or args.slot is None:
    print("Need to use --node and --slot arguments; exiting without plotting")
    sys.exit(1)

lines = []
with open(args.file) as f:
    for line in f:
        if line.startswith("#Trace"):
            col = line.split(",")
            time = col[1].split("=", 1)[1]
            slot = int(col[2].split("=", 1)[1])
            node = int(col[3].split("=", 1)[1])
            bwpSl = int(col[4].split("=", 1)[1])
            len = int(col[5].split("=", 1)[1])
            subch = int(col[6].split("=", 1)[1])
            l_subch = int(col[7].split("=", 1)[1])
            psfch_period = int(col[8].split("=", 1)[1])
            n_selected = int(col[9].split("=", 1)[1])
            rri = int(col[10].split("=", 1)[1])
            harq_enabled = int(col[11].split("=", 1)[1])
            continue
        if int(args.slot) == slot and int(args.node) == node:
            lines.append(line)

if not lines:
    print("Node/slot combination not found; exiting without plotting")
    sys.exit(1)

selection_window = np.loadtxt(lines, delimiter=",")
if selection_window.ndim == 1:
    selection_window = np.expand_dims(selection_window, axis=0)

node = int(args.node)
slot = int(args.slot)
file_prefix = args.file.split(".", 1)[0]

# Remap the selection_window xtick values/labels to run from the slot number to the slot + t2 value
selection_window_xtick_range = list(range(0, len))
selection_window_xtick_values = [i for i in selection_window_xtick_range if i % 5 == 0]
selection_window_xtick_labels = [(slot + i) for i in selection_window_xtick_range if i % 5 == 0]

fig, ax1 = pyplot.subplots(constrained_layout=True)
fig.suptitle("Node %s at time %s s (normalized slot %s)" % (node, time, slot))
im1 = ax1.imshow(
    selection_window, cmap="seismic", clim=(-200, 200), interpolation="none", aspect="auto"
)
ax1.set_yticks(np.arange(-0.5, subch, 1), minor=True)
ax1.set_yticklabels([])
ax1.set_ylabel("subchannels")
ax1.grid(which="minor", color="w", linestyle="dotted")
ax1.set_title(
    "Candidates in dark blue, existing grants in light blue\nNew grant in red: L_subCH=%d, nSelected=%d, harqEnabled=%d, RRI=%d ms"
    % (l_subch, n_selected, harq_enabled, rri),
    fontsize=10,
)
ax1.set_xticks(selection_window_xtick_values, labels=selection_window_xtick_labels)
ax1.set_xlabel("Normalized slot number")
# show colorbars on right
# fig.colorbar(im1)
# fig.colorbar(im2)

# command to overlay RSRP values on sensed value
# may not work well for large sensing window
# for (j,i),label in np.ndenumerate(sensing_window):
#    ax.text(i,j,label,ha='center',va='center')

# Command to show on desktop
if args.show:
    pyplot.show()
else:
    fig.savefig(file_prefix + "-" + str(slot) + "-" + str(node) + ".png", bbox_inches="tight")
