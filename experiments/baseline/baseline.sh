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

pathToTopLevelDir="../.."
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/${pathToTopLevelDir}/build/lib

export heading="Baseline"
export protocol=OLSR
export RngRun=1

export saveDatFiles=false
numSims=8 # number of simultaneous simulations to run.
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
	#cp plot-latency.py ${resultsDir}/.
	cp create-per-simulation-figures.py "${resultsDir}"/
	cp create-aggregate-figures.py "${resultsDir}"/
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
	speed=${2}

	echo starting scenario $scenario_id
	logfile=log${scenario_id}.out
        csvFileName=baseline-${scenario_id}.csv
	summaryFiles="$summaryFiles ${fileNameSummary}"

	./manet-routing \
		--csvFileName=$csvFileName \
		--nodes=$nodes \
		--RngRun=${RngRun} \
		--scenarioId="${scenario_id}" \
		--speed="${speed}" \
		> $logfile  2>&1

	#python3 plot-latency.py ${numTcpDownloads} ${numTcpUploads} ${numTcpDashStreams} ${numDctcpDownloads} ${numDctcpUploads} ${numDctcpDashStreams} ${numWebUsers} ${heading} ${simulationEndTime} --fileNameCm=${fileNameCm} --fileNameCmts=${fileNameCmts} --plotNameCm=${pdfNameCm} --plotNameCmts=${pdfNameCmts} --plotNameRtt=${pdfNameRtt} --imageNameRtt=${imageNameRtt} --fileNameSummary=${fileNameSummary} --scenarioId=${scenario_id} >/dev/null  &
	python create-per-simulation-figures.py "${scenario_id}"
	wait

	echo finished scenario $scenario_id

}
export -f run-scenario

# Scenario definitions:
# 1.       Default configuration 50 nodes
# 2.       20 nodes

#scenario arguments:  scenario_id speed
declare -a scenario=(\
#	S# Speed
	"2 1"
	"3 2"
	"4 3"
	"5 4"
	"6 5"
	"7 6"
	"8 7"
	"9 8"
	"10 9"
	"11 10"
	"12 11"
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
python create-aggregate-figures.py

if ! $saveDatFiles
then
	rm -rf temp
fi

rm -f manet-routing
exit
