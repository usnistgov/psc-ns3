#/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import sys

f = open('data-olsr-pd.dat')
speed_olsr = []
pdr_olsr = []
pdr_err_olsr = []
oh_olsr = []
oh_err_olsr = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_olsr.append (float(columns[0]))
    pdr_olsr.append (float(columns[1]))
    pdr_err_olsr.append (float(columns[2]))
    oh_olsr.append (float(columns[3])/1000)
    oh_err_olsr.append (float(columns[4])/1000)
f.close()

f = open('data-nhdp-pd.dat')
speed_nhdp = []
pdr_nhdp = []
pdr_err_nhdp = []
oh_nhdp = []
oh_err_nhdp = []
for line in f:
    if line.startswith('#'):
        continue
    if line.strip() == "":
        continue
    columns = line.split(',')
    speed_nhdp.append (float(columns[0]))
    pdr_nhdp.append (float(columns[1]))
    pdr_err_nhdp.append (float(columns[2]))
    oh_nhdp.append (float(columns[3])/1000)
    oh_err_nhdp.append (float(columns[4])/1000)
f.close()

fig = plt.figure()
plt.errorbar(speed_olsr, pdr_olsr, yerr=pdr_err_olsr, marker='*', capsize=3, color="C{}".format(0), label='OLSR')
plt.errorbar(speed_nhdp, pdr_nhdp, yerr=pdr_err_nhdp, marker='*', capsize=3, color="C{}".format(1), label='NHDP')
plt.grid(visible=True, which ='major', color='k', linestyle='-', alpha=0.2)
plt.xlabel('Speed (m/s)')
plt.ylabel('Packet Delivery Ratio')
plt.ylim(0.8, 1)
plt.title("Speed vs Packet Delivery Ratio")
plt.legend(['OLSR', 'NHDP'])
fig.savefig ("figure-4-packet-delivery-ratio.png", bbox_inches='tight')

fig = plt.figure()
plt.errorbar(speed_olsr, oh_olsr, yerr=oh_err_olsr, marker='*', capsize=3, color="C{}".format(0), label='OLSR')
plt.errorbar(speed_nhdp, oh_nhdp, yerr=oh_err_nhdp, marker='*', capsize=3, color="C{}".format(1), label='NHDP')
plt.grid(visible=True, which ='major', color='k', linestyle='-', alpha=0.2)
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Overhead (Kb/s)')
plt.ylim(0, 100)
plt.title("Speed vs Average Overhead")
plt.legend(['OLSR', 'NHDP'])
fig.savefig ("figure-5-average-overhead.png", bbox_inches='tight')
