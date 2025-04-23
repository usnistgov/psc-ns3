#/usr/bin/env python3
# SPDX-License-Identifier: NIST-Software
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import sys

# No preamble detection and no link quality
f = open('data-olsr.dat')
speed_olsr = []
pdr_olsr = []
pdr_err_olsr = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_olsr.append (float(columns[0]))
    pdr_olsr.append (float(columns[1]))
    pdr_err_olsr.append (float(columns[2]))
f.close()

# No preamble detection and no link quality
f = open('data-nhdp.dat')
speed_nhdp = []
pdr_nhdp = []
pdr_err_nhdp = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_nhdp.append (float(columns[0]))
    pdr_nhdp.append (float(columns[1]))
    pdr_err_nhdp.append (float(columns[2]))
f.close()

# link quality at -1 dB threshold
f = open('data-nhdp-1.dat')
speed_nhdp_1 = []
pdr_nhdp_1 = []
pdr_err_nhdp_1 = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_nhdp_1.append (float(columns[0]))
    pdr_nhdp_1.append (float(columns[1]))
    pdr_err_nhdp_1.append (float(columns[2]))
f.close()

# link quality at -0.5 dB threshold
f = open('data-nhdp-0.5.dat')
speed_nhdp_5 = []
pdr_nhdp_5 = []
pdr_err_nhdp_5 = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_nhdp_5.append (float(columns[0]))
    pdr_nhdp_5.append (float(columns[1]))
    pdr_err_nhdp_5.append (float(columns[2]))
f.close()

# link quality at 0 dB threshold
f = open('data-nhdp-0.dat')
speed_nhdp_0 = []
pdr_nhdp_0 = []
pdr_err_nhdp_0 = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_nhdp_0.append (float(columns[0]))
    pdr_nhdp_0.append (float(columns[1]))
    pdr_err_nhdp_0.append (float(columns[2]))
f.close()

fig = plt.figure()
plt.errorbar(speed_nhdp_0, pdr_nhdp_0, yerr=pdr_err_nhdp_0, marker='*', capsize=3, color="C{}".format(4), label='NHDP (0 dB threshold)')
plt.errorbar(speed_nhdp_5, pdr_nhdp_5, yerr=pdr_err_nhdp_5, marker='*', capsize=3, color="C{}".format(3), label='NHDP (-0.5 dB threshold)')
plt.errorbar(speed_olsr, pdr_olsr, yerr=pdr_err_olsr, marker='*', capsize=3, color="C{}".format(0), label='OLSR')
plt.errorbar(speed_nhdp_1, pdr_nhdp_1, yerr=pdr_err_nhdp_1, marker='*', capsize=3, color="C{}".format(2), label='NHDP (-1 dB threshold)')
plt.errorbar(speed_nhdp, pdr_nhdp, yerr=pdr_err_nhdp, marker='*', capsize=3, color="C{}".format(1), label='NHDP')
plt.grid(visible=True, which ='major', color='k', linestyle='-', alpha=0.2)
plt.xlabel('Speed (m/s)')
plt.ylabel('Packet Delivery Ratio')
plt.ylim(0.8, 1)
plt.title("Speed vs Packet Delivery Ratio")
plt.legend()
fig.savefig ("figure-6.png", bbox_inches='tight')
