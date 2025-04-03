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
#
# This script orchestrates 18 simulation trials used for the 18 data points
# in Figures 4-5 of the ICNS3 2025 paper on NHDP

pathToTopLevelDir="../.."
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/${pathToTopLevelDir}/build/lib

export heading="Figures-4-5"
export RngRun=100
export use20Mhz=0
export preambleDetectionModel=1

export saveDatFiles=false
numSims=30 # number of simultaneous simulations to run.
	   # Ideally set this equal to the number of cores on your machine

# If a directory name (label) is provided, create the appropriate results directory, copy all necessary scripts (including this one)
# into that directory, cd there, and launch the copy of this script that lives there (using the -L flag), then exit.
if [ "$1" != "-L" ]
then
	dirname=$1
	if [ -z ${dirname} ]
	then
		dirname="results"
	fi

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
	cp plot-figures-4-5.py ${resultsDir}/.
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
	scenario_id=${1}
	nodes=50
	protocol=${2}
	speed=${3}
	use20Mhz=${4}
	initialPending=${5}
	hystReject=${6}
	hystAccept=${7}
	threshold=${8}

	echo starting scenario $scenario_id
	logfile=log-${protocol}-${scenario_id}.out
	summaryFiles="$summaryFiles ${fileNameSummary}"

	./manet-routing \
		--nodes=$nodes \
		--RngRun=${RngRun} \
		--scenarioId="${scenario_id}" \
		--protocol="${protocol}" \
		--speed="${speed}" \
		--use20Mhz="${use20Mhz}" \
		--ns3::nhdp::NhdpClient::InitialPending="${initialPending}" \
		--ns3::nhdp::NhdpClient::HystReject="${hystReject}" \
		--ns3::nhdp::NhdpClient::HystAccept="${hystAccept}" \
		--threshold="${threshold}" \
		--preambleDetectionModel="${preambleDetectionModel}" \
		> $logfile  2>&1

	#python create-per-simulation-figures.py "${scenario_id}" "${protocol}"
	wait

	echo finished scenario $scenario_id

}
export -f run-scenario

# Scenario definitions:
# 1.       Default configuration 50 nodes
# 2.       20 nodes

#scenario arguments:  scenario_id protocol speed use20Mhz initialPending hystReject hystAccept threshold
declare -a scenario=(\
#	S# Speed
	"2 OLSR 2 false false 1 1 -1"
	"3 OLSR 3 false false 1 1 -1"
	"4 OLSR 4 false false 1 1 -1"
	"5 OLSR 5 false false 1 1 -1"
	"6 OLSR 6 false false 1 1 -1"
	"7 OLSR 7 false false 1 1 -1"
	"8 OLSR 8 false false 1 1 -1"
	"9 OLSR 9 false false 1 1 -1"
	"10 OLSR 10 false false 1 1 -1"
	"2 NHDP 2 false false 1 1 -0.5"
	"3 NHDP 3 false false 1 1 -0.5"
	"4 NHDP 4 false false 1 1 -0.5"
	"5 NHDP 5 false false 1 1 -0.5"
	"6 NHDP 6 false false 1 1 -0.5"
	"7 NHDP 7 false false 1 1 -0.5"
	"8 NHDP 8 false false 1 1 -0.5"
	"9 NHDP 9 false false 1 1 -0.5"
	"10 NHDP 10 false false 1 1 -0.5"
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

for i in {2..10}; do
	cat manet-routing-OLSR-${i}.csv >> data-olsr-pd.dat
done

for i in {2..10}; do
	cat manet-routing-NHDP-${i}.csv >> data-nhdp-pd.dat
done

python3 plot-figures-4-5.py

if ! $saveDatFiles
then
	rm -rf temp
fi

rm -f manet-routing
exit
