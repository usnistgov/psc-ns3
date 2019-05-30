WNS3 2017 - LTE D2D Model for ns-3: Validation examples (v1.0 06/13/2017)

----------------------
I. General description
----------------------
This folder contains the scripts needed to reproduce the evaluations performed
to validate the ns-3 LTE D2D Model presented in the WNS3 2017.

For further information please refer to the corresponding publication:
R. Rouil, F. Cintron, A. Ben Mosbah and S. Gamboa, "Implementation and 
Validation of an LTE D2D Model for ns-3", Proceedings of the Workshop
on ns-3 - 2017 WNS3, 2017.

-------------
II. File list  
-------------
README.txt                  This file
wns3-2017-discovery.cc      Discovery validation (scenario)
wns3-2017-discovery.sh      Discovery validation (master)
wns3-2017-pscch.cc          Communication control channel validation (scenario)
wns3-2017-pscch-stats.sh    Communication control channel validation (stats)
wns3-2017-pscch-mac-trace-formatter.py Communication control channel validation (stats formatter)
wns3-2017-pscch.sh          Communication control channel validation (master)
wns3-2017-pssch.cc          Communication shared channel validation (scenario)
wns3-2017-pssch.sh          Communication shared channel validation (master)
wns3-2017-synch.cc          Synchronization validation (scenario)  
wns3-2017-synch.sh          Synchronization validation (master)

-------------------------
III. Running instructions
-------------------------
Copy the scenario scripts (.cc files) to the ns-3 scratch folder 
(~/ns-3-dev/scratch/ or ~/ns-3.29/scratch/)

Copy the master, stats scripts (.sh files) and the stats formatter (.py) to the
ns-3 root folder. 

**Note : Python 3 is required to run wns3-2017-pscch-mac-trace-formatter.py**

From the ns-3 root folder (~/ns-3-dev/ or ~/ns-3.29/), run the desired master script to
execute the corresponding evaluation. 
E.g.:
  ./wns3-2017-discovery.sh 
  ./wns3-2017-pscch.sh
  ./wns3-2017-pssch.sh
  ./wns3-2017-synch.sh

---------------------------
IV. Evaluations description
---------------------------

A. Discovery 

1. Scenario
The "wns3-2017-discovery.cc" scenario is used to validate the implementation of 
the device-to-device (D2D) discovery.
We considered a resource pool consisting of 5 subframes and 10 pairs of resource 
blocks. We deployed 10 out-of-coverage UEs, distributed randomly within an area 
of 100 m x 100 m.
Discovery resource pool and other position/mobility parameters can be changed
in the scenario.

The master script is a pre-configured short version of the discovery validation 
(10 UEs). Multiple discovery parameters can be modified such as:
  * the number of users
  * the transmission probability
  * the number of runs
  * the number of trials

2. Output
The master script will compute the number of periods needed for one random 
UE to discover everyone else in the group. The average of the results 
collected from all the runs (10 runs x 500 trials) was used to generate 
the Cumulative Distribution Function (CDF) of the number of periods needed 
for one UE of the discovery group to discover all other UEs.

The simulation results (all runs and trials) are saved under the directory 
"wns3-2017-discovery-scenarios". The final plot is saved under the name 
"wns3-2017-discovery-cdf.png". Its data is saved in the following file:
"wns3-2017-discovery-scenarios/discovery-cdf-wns3-ue1-average.tr"

B. Communication control channel 

1. Scenario
The wns3-2017-pscch.cc scenario is used to validate the Physical
Sidelink Control Channel (PSCCH) resource collision probability given a 
particular resource pool configuration and a fixed number of UEs contending for
transmission resources. The UEs are deployed using a hexagonal cell grid ring
topology, in a per sector manner.

The master script (wns3-2017-pscch.sh) can run the scenario with different
Sidelink pool configurations for a given range of fixed number of UEs.
Furthermore, the script will re-run each configuration a number of times 
(adjustable within the script) with different random seeds.

The master script  is pre-configured with a shorter version of the PSCCH 
validation. Users can modify the parameters values with those suggested in the
commented lines.

2. Output
The master script will compute the mean and standard deviation of the PSCCH 
resource collision ratios, and other PSCCH error ratios corresponding to each
configuration. The results are saved to a filename starting with the prefix 
"ProsePSCCH-errors-mean" followed with the scenario input parameters.

C. Communication shared channel 

1. Scenario
This example is used to validate the maximum achievable
data rate on the Physical Sidelink Shared Channel (PSSCH) between 2
UEs given a particular resource pool configuration. The two UEs are
placed close enough so there is no loss due to the channel.

The master script will run the scenario for three different sidelink
configurations by varying the following parameters 
- period: duration of the sidelink period 
- pscchLength: length of the pysical sidelink control channel (PSCCH) 
- ktrp: repetition pattern defining how many subframe can be used for 
sidelink 
- mcs: modulation and coding scheme for transmission on the sidelink 
shared channel 
- rbSize: allocation size in resource block (RB)

2. Output
The master script will compute the achievable data rate by
looking at the packets received and print the results on the standard
output. Each line will have 25 values corresponding to the data rate
when using between 2 RBs and 50 RBs by steps of 2.

D. Synchronization 

1. Scenario
The scenario is composed of 63 UEs uniformly distributed in a urban macro 
scenario area covering 7 sites with a inter-site distance of 1732 m. All UEs
are considered out-of-coverage and unsynchronized at the beginning of the 
simulation. The UEs transmit communication data according to a given traffic
pattern. Each UE is configured to perform the D2D synchronization protocol. 
Thus, each UE transmits synchronization signals every 40 ms when transmitting
data, and each UE is configured to perform the Synchronization Reference 
selection every 1000ms. The master script is configured to run simulations
with different traffic patterns (On-Off and Always On patterns) and different
synchronization protocol configuration (values for the syncTxThreshOoC 
threshold). All this data is aggregated afterwards for comparison. 

The master script is pre-configured with a shorter/manageable version of the 
synchronization validation. It includes a shorter simulation time and a reduced 
number of simulations for each configuration. Users interested in enabling the 
complete validation should edit the master script(wns3-2017-synch.sh)
following the instructions provided in it to do so.   

2. Output
Upon completion of the master script execution, the results of the evaluation
can be found in the ns-3 root folder (~/ns-3-dev/ or ~/ns-3.29/):
  - wns3-2017-synchronization-syncTxThreshOoC.txt 
The output data format is:
  TP  Th  NAvg  NCI   
where:
  TP    traffic pattern
  Th    value of the syncTxThreshOoC threshold
  NAvg  average number of UEs acting as Synchronization Reference at the end of
        the simulation time  
  NCI   95% confidence interval value

The master script is also configured to plot this data using gnuplot (if 
available). The plot is written to:
  - wns3-2017-synchronization-syncTxThreshOoC.png  

----------
V. License
----------

NIST-developed software is provided by NIST as a public
service. You may use, copy and distribute copies of the software in
any medium, provided that you keep intact this entire notice. You
may improve, modify and create derivative works of the software or
any portion of the software, and you may copy and distribute such
modifications or works. Modified works should carry a notice
stating that you changed the software and should note the date and
nature of any such change. Please explicitly acknowledge the
National Institute of Standards and Technology as the source of the
software.

NIST-developed software is expressly provided "AS IS." NIST MAKES
NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.

You are solely responsible for determining the appropriateness of
using and distributing the software and you assume all risks
associated with its use, including but not limited to the risks and
costs of program errors, compliance with applicable laws, damage to
or loss of data, programs or equipment, and the unavailability or
interruption of operation. This software is not intended to be used
in any situation where a failure could cause risk of injury or
damage to property. The software developed by NIST employees is not
subject to copyright protection within the United States.             

-----------
VI. Contact
-----------
Richard Rouil - richard.rouil@nist.gov
