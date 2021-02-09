#!/usr/bin/env python3

# Copyright (C) 2021 University of Washington

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Plotting program for mcptt-operational-modes-static-m2e-latency.dat
# usage:  python3 mcptt-operational-modes-plot-m2e-latency.py <optional-filename>

import sys
import os
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

if (len(sys.argv) == 1):
    filename = 'mcptt-operational-modes-static-m2e-latency.dat'
else:
    filename = sys.argv[1]
basename = os.path.splitext(os.path.basename(filename))[0]

title = 'Cumulative Distribution Function of MCPTT Mouth-To-Ear Latency'
f = open(filename, 'r')
on_network = []
off_network = []
relay = []
for line in f:
    if (line.startswith("#")):
        continue
    col = line.split()
    callid = int(col[3])
    # Disregard queued counts?
    #if (float(columns[4]) > 0.210):
    #    continue
    if callid == 1:
         on_network.append(1000*float(col[4]))
    elif callid == 2:
         relay.append(1000*float(col[4]))
    elif callid > 2:
         off_network.append(1000*float(col[4]))
f.close()

#plt.title(title)
plt.xlabel('t (ms)', fontsize=14)
plt.ylabel('Probability that latency <= t', fontsize=14)
#plt.xlim([0,110])
arr = np.array(on_network)
print("on network  num: %d min: %f max: %f" % (arr.size, np.amin (arr), np.amax (arr)))
x = np.sort(arr)
y = np.arange(len(x))/float(len(x))
plt.plot(x, y, color='black',linestyle='solid',label='on network')
arr = np.array(off_network)
print("off network num: %d min: %f max: %f" % (arr.size, np.amin (arr), np.amax (arr)))
x = np.sort(arr)
y = np.arange(len(x))/float(len(x))
plt.plot(x, y, color='blue',linestyle='dashed',label='off network')
arr = np.array(relay)
print("relay       num: %d min: %f max: %f" % (arr.size, np.amin (arr), np.amax (arr)))
x = np.sort(arr)
y = np.arange(len(x))/float(len(x))
plt.plot(x, y, color='green',linestyle='dotted',label='relay')
sys.exit
#plt.hlines(0.5, min(x), max(x), colors='black', linestyles='dashed', label='0.5ms threshold')
#plt.vlines(0.5, 0, 1, colors='black', linestyles='dashed', label='0.5ms threshold')
#plt.vlines(160, 0, 1, colors='black', linestyles='dashed', label='EPC backhaul delay')
plt.ticklabel_format(useOffset=False)
plt.legend (loc='lower right')
plt.grid()
output = basename + '-cdf.eps'
plt.savefig(output, bbox_inches='tight', format='eps')
plt.close()
