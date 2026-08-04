#!/usr/bin/python3

# TODO:
# - Handle possibility that multiple calls (with different lsubCh values) can occur
#   at the same time
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
parser.add_argument("--file", help="File name", default="sl-basic-example-sensing.csv")
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
    print("Need to use --node and --slot arguments")
    sys.exit()

lines = []
with open(args.file) as f:
    for line in f:
        if line.startswith("#Trace"):
            col = line.split(",")
            time = col[1].split("=", 1)[1]
            slot = int(col[2].split("=", 1)[1])
            node = int(col[3].split("=", 1)[1])
            bwpSl = col[4].split("=", 1)[1]
            len = int(col[5].split("=", 1)[1])
            subch = int(col[6].split("=", 1)[1])
            l_subch = int(col[7].split("=", 1)[1])
            t0 = int(col[8].split("=", 1)[1])
            t1 = int(col[9].split("=", 1)[1])
            t2 = int(col[10].split("=", 1)[1])
            continue
        if int(args.slot) == slot and int(args.node) == node:
            lines.append(line)

if not lines:
    print("Node/slot combination not found; exiting without plotting")
    sys.exit(1)

values = np.loadtxt(lines, delimiter=",")

node = int(args.node)
slot = int(args.slot)
file_prefix = args.file.split(".", 1)[0]

# Split into two subplots, one for sensing window, one for selection window
if values.ndim > 1:
    sensing_window = values[:, :t0]
    selection_window = values[:, t0:]
else:
    sensing_window = values[:t0]
    sensing_window = np.expand_dims(sensing_window, axis=0)
    selection_window = values[t0:]
    selection_window = np.expand_dims(selection_window, axis=0)

# If there is no data in the sensing window, set all values to 'nan'
# so that they will show as white in imshow() below
if np.all(sensing_window == 0):
    sensing_window[sensing_window == 0] = np.nan

# Remap the resource xtick values/labels to run from (slot - t0) to current slot number
sensing_window_xtick_range = list(range(0, t0 + 1))
sensing_window_xtick_labels = [(slot - t0 + i) for i in sensing_window_xtick_range if i % 50 == 0]
sensing_window_xtick_values = [i for i in sensing_window_xtick_range if i % 50 == 0]

# Remap the selection_window xtick values/labels to run from the slot number to the slot + t2 value
selection_window_xtick_range = list(range(0, len - t0))
selection_window_xtick_values = [i for i in selection_window_xtick_range if i % 5 == 0]
selection_window_xtick_labels = [(slot + i) for i in selection_window_xtick_range if i % 5 == 0]

fig, (ax1, ax2) = pyplot.subplots(2, sharex=False, sharey=False, constrained_layout=True)
fig.suptitle("Node %s at time %s s (normalized slot %s) LsubCH=%s" % (node, time, slot, l_subch))
im1 = ax1.imshow(sensing_window, cmap="hot", interpolation="none", aspect="auto")
#ax1.set_yticks(np.arange(-0.5, 5, 1), minor=True)
ax1.set_yticks(np.arange(-0.5, subch, 1), minor=True)
print(np.arange(-0.5,subch,1))
ax1.set_yticklabels([])
ax1.grid(which="minor", color="k", linestyle="dotted")
ax1.set_ylabel("subchannels")
ax1.set_xlabel("Normalized slot number")
ax1.set_xticks(sensing_window_xtick_values, labels=sensing_window_xtick_labels)
ax1.set_title("Sensing window (transmit slots: black, sensed sensing_window: orange)", fontsize=10)
im2 = ax2.imshow(selection_window, cmap="Blues", interpolation="none", aspect="auto")
#ax2.set_yticks(np.arange(-0.5, 5, 1), minor=True)
ax2.set_yticks(np.arange(-0.5, subch, 1), minor=True)
ax2.set_yticklabels([])
ax2.set_ylabel("subchannels")
ax2.grid(which="minor", color="w", linestyle="dotted")
ax2.set_title("Selection window (candidates identified in dark blue)", fontsize=10)
ax2.set_xticks(selection_window_xtick_values, labels=selection_window_xtick_labels)
ax2.set_xlabel("Normalized slot number")
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
