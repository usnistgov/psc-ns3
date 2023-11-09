#!/usr/bin/python3

# NIST-developed software is provided by NIST as a public 
# service. You may use, copy and distribute copies of the software in
# any medium, provided that you keep intact this entire notice. You
# may improve, modify and create derivative works of the software or
# any portion of the software, and you may copy and distribute such
# modifications or works. Modified works should carry a notice
# stating that you changed the software and should note the date and
# nature of any such change. Please explicitly acknowledge the
# National Institute of Standards and Technology as the source of the
# software.

# NIST-developed software is expressly provided "AS IS." NIST MAKES
# NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
# OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
# NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
# WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
# OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
# WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
# SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
# CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.

# You are solely responsible for determining the appropriateness of
# using and distributing the software and you assume all risks
# associated with its use, including but not limited to the risks and
# costs of program errors, compliance with applicable laws, damage to
# or loss of data, programs or equipment, and the unavailability or
# interruption of operation. This software is not intended to be used
# in any situation where a failure could cause risk of injury or
# damage to property. The software developed by NIST employees is not
# subject to copyright protection within the United States.

# Description:
# This script runs the simulations corresponding to the 'Results' 
# section of the paper entitled "Towards System Level Simulations of 
# Public Safety Applications over 5G NR Sidelink" in submission to 
# the 2024 IEEE World Forum on Public Safety Technology (WF-PST) 
# conference. This program also processes the simulation output and
# generates the plots corresponding to the results figures in the 
# paper.
# Prerequisites:  Python3 matplotlib and numpy
# Usage (from the ns3 root folder): python3 run_wfpst-2024.py
# Program outputs:  
# 1) Four directories with simulation results, named with prefix 'wfpst-2024'
# 2) Four PNG image files with prefix 'wfpst-2024'

import os
import subprocess
import sys
import csv
import matplotlib.pyplot as plt
import numpy as np
from multiprocessing import Pool


def process_access_time (outputDir):

    filename = "%s/mcptt-access-time.txt" % (outputDir)
    basename = "mcptt-access-time"

    # The 'counts' array is for tabulating access time outcomes for the table
    # columns: I, Q, A, D, F
    # rows:  on-network, relay, off-network
    counts = np.zeros(15).reshape(3,5)

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

    #Write Outcome table to file
    outputTableRaw = outputDir + '/' + basename + '-outcomes.txt'
    outputTable = [['%.0f%%' % (100*elem) for elem in percentages_list[0]],
        ['%.0f%%' % (100*elem) for elem in percentages_list[1]],
        ['%.0f%%' % (100*elem) for elem in percentages_list[2]]]
    with open(outputTableRaw, "w") as f:
        f.write("-\t" + "\t".join(columns) + "\n")
        f.write("on-network\t" + "\t".join(outputTable[0]) + "\n")
        f.write("off-network\t" + "\t".join(outputTable[1]) + "\n")
        f.write("relay\t" + "\t".join(outputTable[2]) + "\n")

    #Generate eCDFs
    arr = np.array(on_network)
    x_onNet = np.sort(arr)
    y_onNet = np.arange(len(x_onNet))/float(len(x_onNet))
    arr = np.array(off_network)
    x_offNet = np.sort(arr)
    y_offNet = np.arange(len(x_offNet))/float(len(x_offNet))
    arr = np.array(relay)
    x_relay = np.sort(arr)
    y_relay = np.arange(len(x_relay))/float(len(x_relay))
   
    #Write eCDFs raw data to files
    outputRaw = outputDir + '/' + basename + '-cdf_onNet.txt'
    np.savetxt(outputRaw, (x_onNet, y_onNet), fmt='%.6g', delimiter='\t', newline=os.linesep)
    outputRaw = outputDir + '/' + basename + '-cdf_offNet.txt'
    np.savetxt(outputRaw, (x_offNet, y_offNet), fmt='%.6g', delimiter='\t', newline=os.linesep)
    outputRaw = outputDir + '/' + basename + '-cdf_relay.txt'
    np.savetxt(outputRaw, (x_relay, y_relay), fmt='%.6g', delimiter='\t', newline=os.linesep)


def process_m2e_latency (outputDir):

    filename = "%s/mcptt-m2e-latency.txt" % (outputDir)
    basename = "mcptt-m2e-latency"

    f = open(filename, 'r')
    on_network = []
    off_network = []
    relay = []
    for line in f:
        if (line.startswith("#")):
            continue
        col = line.split()
        callid = int(col[3])
        if callid == 1:
            on_network.append(1000*float(col[4]))
        elif callid == 2:
            relay.append(1000*float(col[4]))
        elif callid > 2:
            off_network.append(1000*float(col[4]))
    f.close()

    arr = np.array(on_network)
    x_onNet = np.sort(arr)
    y_onNet = np.arange(len(x_onNet))/float(len(x_onNet))
    arr = np.array(off_network)
    x_offNet = np.sort(arr)
    y_offNet = np.arange(len(x_offNet))/float(len(x_offNet))
    arr = np.array(relay)
    x_relay = np.sort(arr)
    y_relay = np.arange(len(x_relay))/float(len(x_relay))

    #Write eCDFs raw data to files
    outputRaw = outputDir + '/' + basename + '-cdf_onNet.txt'
    np.savetxt(outputRaw, (x_onNet, y_onNet), fmt='%.6g', delimiter='\t', newline=os.linesep)
    outputRaw = outputDir + '/' + basename + '-cdf_offNet.txt'
    np.savetxt(outputRaw, (x_offNet, y_offNet), fmt='%.6g', delimiter='\t', newline=os.linesep)
    outputRaw = outputDir + '/' + basename + '-cdf_relay.txt'
    np.savetxt(outputRaw, (x_relay, y_relay), fmt='%.6g', delimiter='\t', newline=os.linesep)


def plot_cdf_for_metric_by_tech_one_folder(folder, metric):
    
    # Define technology files for each metric
    tech_files = {
        "mcptt-m2e-latency": {
            "on-network": "mcptt-m2e-latency-cdf_onNet.txt",
            "relay": "mcptt-m2e-latency-cdf_relay.txt",
            "off-network": "mcptt-m2e-latency-cdf_offNet.txt"
        },
        "mcptt-access-time": {
            "on-network": "mcptt-access-time-cdf_onNet.txt",
            "relay": "mcptt-access-time-cdf_relay.txt",
            "off-network": "mcptt-access-time-cdf_offNet.txt"
        }
    }
    # Check if metric files are available
    if metric not in tech_files:
        print(f"Warning: No tech files defined for metric '{metric}'.")
        return
    
    tech_line_styles = {
        "on-network": '-',
        "relay": '--',
        "off-network": ':'
    }
    xlim_values = {
        'mcptt-access-time': (0, 200),
        'mcptt-m2e-latency':(0, 200) 
    }

    # Create plot
    plt.figure(figsize=(18, 2)) #Horizontal
    for tech, file_name in tech_files[metric].items():
        specific_file_path = os.path.join(folder, file_name)
        
        if not os.path.exists(specific_file_path):
            print(f"Warning: File '{specific_file_path}' not found.")
            continue
        
        with open(specific_file_path, 'r') as file:
            reader = csv.reader(file, delimiter='\t')
            x_vals = next(reader)
            y_vals = next(reader)
            
            x_vals = [float(val) for val in x_vals if val]
            y_vals = [float(val) for val in y_vals if val]
            
            linestyle = tech_line_styles[tech]
            plt.plot(x_vals, y_vals, label=tech, linewidth=2.0, linestyle=linestyle)
    
    plt.xlabel(metric.replace("mcptt-", "").replace("-", " ").title()+' (ms)')
    xlims = xlim_values.get(metric, {})
    plt.xlim(xlims)
    plt.ylabel('CDF')
    plt.grid(True)
    plt.tight_layout()
    plt.subplots_adjust(right=0.75)
    plt.legend(loc='lower right', fontsize='small')
    plt.gca().set_position([0.1, 0.1, 0.2, 0.8])
    
    base_filename = f"wfpst-2024_Fig5_{metric}_cdf_by_tech.png"
    plt.savefig(base_filename, bbox_inches='tight')
    print(f'Figure: %s'  % (base_filename))



def plot_cdf_metric_for_tech_by_selected_folders(folders, legends, metric):
    # Define technology files for each metric
    metrics_files = {
        "mcptt-access-time": {
            "relay": "mcptt-access-time-cdf_relay.txt",
            "off-network": "mcptt-access-time-cdf_offNet.txt"
        },
        "mcptt-m2e-latency": {
            "relay": "mcptt-m2e-latency-cdf_relay.txt",
            "off-network": "mcptt-m2e-latency-cdf_offNet.txt"
        }
    }
    xlim_values = {
        'mcptt-access-time': {
            'relay': (0, 200),
            'off-network': (0, 200)
        },
        'mcptt-m2e-latency': {
            'relay': (0, 300),
            'off-network': (0, 200)
        }
    }
    line_styles = ['-', '-', '--', ':']
    
    #Create the plots
    for tech, file_name in metrics_files[metric].items():
        plt.figure(figsize=(18, 2)) 
        
        line_style_index = 0
        
        for folder_name, legend_name in zip(folders, legends):
            specific_file_path = os.path.join(folder_name, file_name)
            
            if not os.path.exists(specific_file_path):
                print(f"Warning: File '{specific_file_path}' not found.")
                continue
            
            with open(specific_file_path, 'r') as file:
                reader = csv.reader(file, delimiter='\t')
                x_vals = next(reader)
                y_vals = next(reader)
                
                x_vals = [float(val) for val in x_vals if val]
                y_vals = [float(val) for val in y_vals if val]
                
                linestyle = line_styles[line_style_index]
                plt.plot(x_vals, y_vals, label=legend_name, linewidth=2.0, linestyle=linestyle)
                line_style_index = (line_style_index + 1) % len(line_styles)
        
        plt.xlabel(metric.replace("mcptt-", "").replace("-", " ").title()+' (ms)')
        plt.ylabel('CDF')
        xlims = xlim_values.get(metric, {}).get(tech, None)
        plt.xlim(xlims)
        plt.grid(True)
        plt.tight_layout()
        plt.subplots_adjust(right=0.75)
        plt.legend(loc='lower right', fontsize='small')  
        plt.gca().set_position([0.1, 0.1, 0.2, 0.8])
        
        base_filename = f"wfpst-2024_Fig6_{metric}_cdf_by_sched_{tech}.png"
        plt.savefig(base_filename, bbox_inches='tight')
        print(f'Figure: %s'  % (base_filename))


def start_simulation(params):
    minNumAccess, numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps = params
    outputDir = "wfpst-2024_slNum-%d_rri-%d_maxNTx-%d_harq-%s_psfchPeriod-%d_offNetMediaSps-%s_relayMediaSps-%s" % (numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps)
    print(f'Output dir: %s'  % (outputDir))
    try:
        os.mkdir(outputDir)
    except FileExistsError:
        print("folder %s already exists, overwriting..." % outputDir)
    run_command = f"./ns3 run --cwd={outputDir} nr-mcptt-operational-modes-static -- --minNumAccess={minNumAccess} --RngRun=2 --showProgress=true --slNumerology={numerology} --rri={rri} --maxNTx={maxNTx} --harq={harq} --psfchPeriod={psfchPeriod} --offNetMediaSps={offNetMediaSps} --relayMediaSps={relayMediaSps}"
    with open(outputDir + "/output.txt", "w") as f:
        output = subprocess.run(run_command.split(), stdout=f, stderr=subprocess.STDOUT)
    f.close()
    print("Processing data...")
    process_access_time (outputDir)
    process_m2e_latency (outputDir)

#Main script

# Create list with all simulations to run. Each element is a tuple of simulation parameters values
allSims = []

#Define the minimum number of MCPTT floor accesses all calls must have before stopping the simulation. 
#The results in the paper are with minNumAccess=1000
minNumAccess = 1000

# Fig. 5 simulation parameters
numerology = 0
rri = 20
maxNTx = 1
harq = 'false'
psfchPeriod = 0
offNetMediaSps = 'false'
relayMediaSps = 'false'

params = (minNumAccess, numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps)
allSims.append (params)

# Fig. 6 simulation parameters

# -Single LC - SPS
#    Note: Do not run this case because the simulation is the same as in Fig. 5.

# -Two LCs - SPS+SPS
numerology = 0
rri = 20
maxNTx = 1
harq = 'false'
psfchPeriod = 0
offNetMediaSps = 'true'
relayMediaSps = 'true'

params = (minNumAccess, numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps)
allSims.append (params)

# -Two LCs - Dyn+SPS
numerology = 0
rri = 0
maxNTx = 1
harq = 'false'
psfchPeriod = 0
offNetMediaSps = 'true'
relayMediaSps = 'true'

params = (minNumAccess, numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps)
allSims.append (params)

# -Two LCs - Dyn+Dyn
numerology = 0
rri = 0
maxNTx = 1
harq = 'false'
psfchPeriod = 0
offNetMediaSps = 'false'
relayMediaSps = 'false'

params = (minNumAccess, numerology, rri, maxNTx, harq, psfchPeriod, offNetMediaSps, relayMediaSps)
allSims.append (params)

# Run all simulations 
#  Inital run for compilation
print ("Running ./ns3 build...")
output = subprocess.run(['./ns3', 'show', 'config'], capture_output=True)
if output.returncode:
    print('Error:  Is the project configured?  Run ./ns3 configure ... first')
    print(output.stdout.decode("utf-8"))
    print(output.stderr.decode("utf-8"))
    sys.exit(1)
output = subprocess.run(['./ns3', 'build'], capture_output=True)
if output.returncode:
    print('Error:  The build failed; fix it to continue')
    print(output.stdout.decode("utf-8"))
    print(output.stderr.decode("utf-8"))
    sys.exit(1)

#  Create a Pool object with "nProcesses" number of simultaneous processes/simulations
nProcesses = 4
pool = Pool(processes=nProcesses)

#  Run the simulations
print ("Running simulations...")
for _ in pool.imap_unordered(start_simulation, allSims):
    pass
pool.close()
pool.join()

print ("Generating plots...")
 
# Generate Fig. 5 plots
outputDir = "wfpst-2024_slNum-0_rri-20_maxNTx-1_harq-false_psfchPeriod-0_offNetMediaSps-false_relayMediaSps-false"
plot_cdf_for_metric_by_tech_one_folder(outputDir, 'mcptt-m2e-latency')
plot_cdf_for_metric_by_tech_one_folder(outputDir, 'mcptt-access-time')


# Generate Fig. 6 plots:
selected_folders = [
    "wfpst-2024_slNum-0_rri-20_maxNTx-1_harq-false_psfchPeriod-0_offNetMediaSps-false_relayMediaSps-false",
    "wfpst-2024_slNum-0_rri-20_maxNTx-1_harq-false_psfchPeriod-0_offNetMediaSps-true_relayMediaSps-true",
    "wfpst-2024_slNum-0_rri-0_maxNTx-1_harq-false_psfchPeriod-0_offNetMediaSps-true_relayMediaSps-true",
    "wfpst-2024_slNum-0_rri-0_maxNTx-1_harq-false_psfchPeriod-0_offNetMediaSps-false_relayMediaSps-false"
]
corresponding_legends = [
    "One LC - SPS",
    "Two LCs - SPS",
    "Two LCs - Dyn+SPS",
    "Two LCs - Dyn+Dyn"
]

plot_cdf_metric_for_tech_by_selected_folders(selected_folders, corresponding_legends, 'mcptt-m2e-latency')
