#!/usr/bin/python3

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

# Plotting program for mcptt-operational-modes-static-access-time.dat
# usage:  python3 mcptt-operational-modes-plot-access-time.py <optional-filename>

import sys
import os
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

if (len(sys.argv) == 1):
    filename = 'mcptt-operational-modes-static-access-time.dat'
else:
    filename = sys.argv[1]
basename = os.path.splitext(os.path.basename(filename))[0]

# The 'counts' array is for tabulating access time outcomes for the table
# columns: I, Q, A, D, F
# rows:  on-network, relay, off-network
counts = np.zeros(15).reshape(3,5)
#counts: ([[ 0, 0, 0, 0, 0],
#          [ 0, 0, 0, 0, 0],
#          [ 0, 0, 0, 0, 0]])

# Create lists for observed values
on_network = []
relay = []
off_network = []

# Read in values 
fd = open(filename, 'r')
for line in fd:
    col = line.split()
    if (line.startswith("#")):
        continue
    timestamp = float(col[0])
    callid = int(col[2])
    result = col[3]
    if (callid == 1):
        # on network
        if (result == 'I'):
            counts[0][0] += 1
        elif result == 'Q':
            counts[0][1] += 1
        elif result == 'A':
            counts[0][2] += 1
        elif result == 'D':
            counts[0][3] += 1
        elif result == 'F':
            counts[0][4] += 1
    elif (callid > 2):
        # off network
        if (result == 'I'):
            counts[1][0] += 1
        elif result == 'Q':
            counts[1][1] += 1
        elif result == 'A':
            counts[1][2] += 1
        elif result == 'D':
            counts[1][3] += 1
        elif result == 'F':
            counts[1][4] += 1
    elif (callid == 2):
        # relay
        if (result == 'I'):
            counts[2][0] += 1
        elif result == 'Q':
            counts[2][1] += 1
        elif result == 'A':
            counts[2][2] += 1
        elif result == 'D':
            counts[2][3] += 1
        elif result == 'F':
            counts[2][4] += 1
    if (callid == 1 and (result == 'I' or result == 'Q')):
         on_network.append(1000*float(col[4]))
    elif (callid == 2 and (result == 'I' or result == 'Q')):
         relay.append(1000*float(col[4]))
    elif (callid > 2 and (result == 'I' or result == 'Q')):
         off_network.append(1000*float(col[4]))

columns = ('Immed', 'Queue', 'Aband', 'Deny', 'Fail')
rows = ('on-network', 'off-network', 'relay')

# Convert counts to percentages, formatted as strings
percentages = counts/counts.sum(axis=1)[:,None] 
percentages_list = percentages.tolist()
formatted = [ [ '%.0f%%' % (100*elem) for elem in percentages_list[0]],
	[ '%.0f%%' % (100*elem) for elem in percentages_list[1]],
	[ '%.0f%%' % (100*elem) for elem in percentages_list[2]]]

# Add table of outcomes; zorder ensures that the table overlays the gridlines
the_table = plt.table(cellText=formatted, rowLabels=rows, colLabels=columns, loc='lower right', zorder=3)
the_table.set_fontsize(10)
#the_table.scale(0.6,0.7)
the_table.scale(0.6,1.1)

#plt.title('Access time for immediate responses (queuing disabled)')
plt.xlabel('t (ms)', fontsize=12)
plt.xlim([0,1400])
plt.xticks(np.arange(0,1400,200))
plt.ylabel('Probability that access time <= t', fontsize=12)
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
plt.legend(loc="center right")
plt.grid()
output = basename + '-cdf.eps'
plt.savefig(output, bbox_inches='tight', format='eps')
plt.close()
