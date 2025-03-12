#!/bin/bash
#
# Copyright (c) 2017-2020 Cable Television Laboratories, Inc.
#
# SPDX-License-Identifier: GPL-2.0-only and NIST-Software
#
# Ported from residential.sh from https://github.com/cablelabs/docsis-ns3
#
# Authors:
#   Greg White <g.white@cablelabs.com>
#   Tom Henderson <tomh@tomh.org>

# Running this script should generate a timestamped results directory with Figures 2 and 3
# of the NHDP paper, plus other simulation artifacts

pathToTopLevelDir="../.."
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/${pathToTopLevelDir}/build/lib

export heading="Nhdp"

export saveDatFiles=false
numSims=8 # number of simultaneous simulations to run.
	  # Set this equal to the number of cores on your machine that you want to parallelize

# If a directory name (label) is provided, create the appropriate results directory, copy all necessary scripts (including this one)
# into that directory, cd there, and launch the copy of this script that lives there (using the -L flag), then exit.
if [ "$1" != "-L" ]
then
	dirname=$1
	if [ -z ${dirname} ]
	then
		dirname="results"
	fi

	# Build ns-3 and then store in the file 'version.txt' the information about which
	# branch of code was used, and 
	${pathToTopLevelDir}/./ns3 build
	resultsDir=$dirname-`date +%Y%m%d-%H%M%S`
	mkdir -p ${resultsDir}
	repositoryVersion=`git rev-parse --abbrev-ref HEAD`
	repositoryVersion+=' commit '
	repositoryVersion+=`git rev-parse --short HEAD`
	repositoryVersion+=' '
	repositoryVersion+=`git log -1 --format=%cd`
	echo $repositoryVersion > ${resultsDir}/version.txt
	gitDiff=`git diff`
	if [[ $gitDiff ]]
	then
		echo "$gitDiff" >> ${resultsDir}/version.txt
	fi
	PROFILE=$(${pathToTopLevelDir}/ns3 show profile | awk '{print $NF}')
	VERSION=$(cat ${pathToTopLevelDir}/VERSION | tr -d '\n')
	EXECUTABLE_NAME=ns${VERSION}-manet-routing-${PROFILE}
	EXECUTABLE=${pathToTopLevelDir}/build/src/nhdp/examples/${EXECUTABLE_NAME}
	if [ -f "$EXECUTABLE" ]; then
		cp ${EXECUTABLE} ${resultsDir}/manet-routing
	else
		# Try the src directory
		EXECUTABLE=${pathToTopLevelDir}/build/src/nhdp/examples/${EXECUTABLE_NAME}
		if [ -f "$EXECUTABLE" ]; then
			cp ${EXECUTABLE} ${resultsDir}/manet-routing
		else
			echo "$EXECUTABLE not found, exiting"
			exit 1
		fi
	fi
	PROGRAM=${pathToTopLevelDir}/src/nhdp/examples/manet-routing.cc
	if [ -f "$PROGRAM" ]; then
		cp ${PROGRAM} ${resultsDir}/
	else
		# Try the src directory
		PROGRAM=${pathToTopLevelDir}/src/nhdp/examples/manet-routing.cc
		if [ -f "$PROGRAM" ]; then
			cp ${PROGRAM} ${resultsDir}/
		else
			echo "$PROGRAM not found, but continuing"
		fi
	fi
	cp plot-figures.py ${resultsDir}/.
	cp $0 ${resultsDir}/.
	cd ${resultsDir}
	mkdir temp

	./${0##*/} -L $1 $2 >commandlog.out &  # launch the copy of this script in the background

 	echo "***************************************************************"
	echo "* Launched:  ${resultsDir}/${0##*/}"
	echo "* Output in:  $resultsDir/commandlog.out"
	if [ "(uname)" == "Linux" ]; then
		echo "* Kill this run with:  kill -SIGTERM -`ps h -o pgid -q $!`"
	fi
 	echo "***************************************************************"
	echo
	exit 0
fi
shift
export summaryHeader="results directory: $1"

function run-scenario () {

	# process function arguments
	# The scenario ID is just a tag to uniquely name trace files when parallelized
	scenario_id=${1}
	nodes=50
	protocol=${2}
	speed=${3}

	echo starting scenario $scenario_id
	logfile=log-${protocol}-${scenario_id}.out

	./manet-routing \
		--nodes=$nodes \
		--scenarioId="${scenario_id}" \
		--protocol="${protocol}" \
		--speed="${speed}" \
		> $logfile  2>&1

	wait

	echo finished scenario $scenario_id

}
export -f run-scenario

# Scenario definitions:
# 1.       Default configuration 50 nodes

# Use unique scenario_ids to avoid file name collisions
# scenario arguments:  scenario_id protocol speed
declare -a scenario=(\
#	S# Protocol Speed
	"2 OLSR 2"
	"3 OLSR 3"
	"4 OLSR 4"
	"5 OLSR 5"
	"6 OLSR 6"
	"7 OLSR 7"
	"8 OLSR 8"
	"9 OLSR 9"
	"10 OLSR 10"
	"12 NHDP 2"
	"13 NHDP 3"
	"14 NHDP 4"
	"15 NHDP 5"
	"16 NHDP 6"
	"17 NHDP 7"
	"18 NHDP 8"
	"19 NHDP 9"
	"20 NHDP 10"
	)

# launch simulation scenarios using GNU Parallel if it is installed, otherwise use basic job control
if hash parallel; then
	printf '%s\n' "${scenario[@]}" | parallel --no-notice --colsep '\s+' -u run-scenario {}
else
	for x in ${!scenario[@]}
	do
		run-scenario ${scenario[$x]} &
		[[ $(( (x+1) % numSims)) -eq 0 ]] && wait # pause every numSims until those jobs complete
	done
fi

wait

# Prepare the main csv files for plotting
for f in manet-routing-NHDP-*.csv; do
	cat "$f" | sed '/^#/d' >> data-nhdp.dat
done
sort -n -o data-nhdp.dat data-nhdp.dat
for f in manet-routing-OLSR-*.csv; do
	cat "$f" | sed '/^#/d' >> data-olsr.dat
done
sort -n -o data-olsr.dat data-olsr.dat

python3 plot-figures.py

# Remove the executable
rm -f manet-routing
exit
