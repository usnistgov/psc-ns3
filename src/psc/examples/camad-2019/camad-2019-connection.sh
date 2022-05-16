#!/bin/bash

  #/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
  #
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
  #
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
  #
  # You are solely responsible for determining the appropriateness of
  # using and distributing the software and you assume all risks
  # associated with its use, including but not limited to the risks and
  # costs of program errors, compliance with applicable laws, damage to
  # or loss of data, programs or equipment, and the unavailability or
  # interruption of operation. This software is not intended to be used
  # in any situation where a failure could cause risk of injury or
  # damage to property. The software developed by NIST employees is not
  # subject to copyright protection within the United States.


trap "kill 0" EXIT #If you kill this process, it will kill all childs too (e.g., all simulations running in paralel)

###############################################################################
#---------------------------------- Usage:------------------------------------#
# Requirements:                                                               #
# 1. Add the built scenario to your path before running this script           #
#    E.g., using the command:                                                 #
#       $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/build/lib             #
# 2. Copy the scenario file (camad-2019-connection.cc) to the 'scratch' folder#
# 3. Copy this script to the ns3 root folder                                  #  
#                                                                             #
# Running the evaluation:                                                     #
# 1. Set the number of trials/run per set you desire to run by modifying the  #
# 'endRun' parameter below (CAMAD 2019 -> endRun=100)                         #
# 2. Save                                                                     #
# 3. From the ns-3 root folder run the script with the evaluation number as   #
# argument                                                                    #
#    E.g.                                                                     #
#       $ ./camad-2019-connection.sh 3                                        #
#                                                                             #
# The available evaluations are                                               #
#    - Eval #1: Impact of Timer T4100 (CAMAD 2019)                            #
#    - Eval #2: Impact of relay UL traffic (CAMAD 2019)                       #
#    - Eval #3: Short example with small number of sets                       #
#-----------------------------------------------------------------------------#
###############################################################################

scenario="camad-2019-connection"

startRun=1
endRun=20
#endRun=100

nThreads=20

slPeriod=40.0 #ms

function RunEvaluation {
  evalNumber=$1
  
  simTime="40" #s 

  if [ "$evalNumber" -eq 1 ];then
    ###Eval 1: CAMAD 2019 paper - Impact of Timer T4100 (3 * 16 * 2 = 96 sets)
    nRemoteUesArray="1 4 6" #3 values
    relayTraffic="false"
    relayTrafficOffMeanArray="100" #1 value
    remoteT4100Array="80 160 240 320 400 480 560 640 720 800 880 960 1040 1120 1200 1280" #16 values
    remoteDCRqMaxArray="0 4" #2 values
    outputDirBase="output_camad-2019-connection_ImpactOfTimerT4100"

  elif [ "$evalNumber" -eq 2 ];then
    ###Eval 2: CAMAD 2019 paper - Impact of relay UL traffic  (8 * 5 = 40 sets)
    nRemoteUesArray="4" #1 value
    relayTraffic="true"
    relayTrafficOffMeanArray="100 20 10 5 3.3 2.5 2 1.33" #8 values
    remoteT4100Array="640" #1 value
    remoteDCRqMaxArray="0 1 4 10 20" #5 values
    outputDirBase="output_camad-2019-connection_ImpactOfUplinkTraffic"

  elif [ "$evalNumber" -eq 3 ];then
    ###Eval 3: Short evaluation with reduced number of sets (2 * 3 = 6 sets)###
    nRemoteUesArray="4" #1 value
    relayTraffic="false"
    relayTrafficOffMeanArray="100" #1 value
    remoteT4100Array="80 640 1040" #3 values
    remoteDCRqMaxArray="0 4" #2 values
    outputDirBase="output_camad-2019-connection_Example"

  else 
    echo "ERROR! Invalid evaluation number"
    exit
  fi 

  echo -e "Starting Evaluation... \n   Output: $outputDirBase"

  mkdir -p $outputDirBase

  metricArray="allRemotesConnectionSuccessRatio allRemotesAverageConnectionTime"

  for relayTrafficOffMean in $relayTrafficOffMeanArray
  do
    for nRemoteUes in $nRemoteUesArray
    do
    
      for remoteDCRqMax in $remoteDCRqMaxArray
      do
        baseFor_remoteT4100="nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_relayTrafficOffMean-${relayTrafficOffMean}_remoteDCRqMax-${remoteDCRqMax}"
        
        for metric in $metricArray
        do
          echo -e "remoteT4100\tmean\tCI" > ${outputDirBase}/${baseFor_remoteT4100}_remoteT4100-vs-${metric}_CI.txt
        done #for metric in $metricArray

        for remoteT4100 in $remoteT4100Array
        do
          outputDirSet="${outputDirBase}/set_nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_relayTrafficOffMean-${relayTrafficOffMean}_remoteT4100-${remoteT4100}_remoteDCRqMax-${remoteDCRqMax}"
          mkdir -p $outputDirSet

          parameters="--simTime=${simTime} --nRemoteUes=${nRemoteUes} --relayTraffic=${relayTraffic} --relayTrafficOffMean=${relayTrafficOffMean} --remoteT4100=${remoteT4100} --remoteDCRqMax=${remoteDCRqMax}" 

          RunSet $outputDirSet "$parameters" #&

          wait

          ProcessSet $outputDirSet $nRemoteUes #&

          #Collecting metrics for remoteT4100
          for metric in $metricArray
          do
            awk -v remoteT4100=${remoteT4100} -v slPeriod=${slPeriod} '{print (remoteT4100/slPeriod)"\t"$0; }' $outputDirSet/${metric}_CI.txt >> ${outputDirBase}/${baseFor_remoteT4100}_remoteT4100-vs-${metric}_CI.txt
          done #for metric in $metricArray

        done #for remoteT4100 in $remoteT4100Array
      done #for remoteDCRqMax in $remoteDCRqMaxArray
    done #for nRemoteUes in $nRemoteUesArray
  done #for relayTrafficOffMean in $relayTrafficOffMeanArray

  echo "Evaluation (Output: $outputDirBase) Done. "



  remoteT4100RealArray=( $remoteT4100Array )
  arrayLenght=${#remoteT4100RealArray[@]}
  if [ "$arrayLenght" -gt 1 ];then    
    echo "Generating remoteT4100-vs-metrics plot... "
    #Plotting for every remoteDCRqMax: Metric vs T4100, one curve per nRemoteUes
    for relayTrafficOffMean in $relayTrafficOffMeanArray
    do
      for metric in $metricArray
      do
        stringToPlotForDCRqMaxAndMetric=''
        lc=1
        dt=1
        for remoteDCRqMax in $remoteDCRqMaxArray
        do
          for nRemoteUes in $nRemoteUesArray
          do
            baseFor_remoteT4100="nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_relayTrafficOffMean-${relayTrafficOffMean}_remoteDCRqMax-${remoteDCRqMax}"
            
            stringToPlotForDCRqMaxAndMetric=" $stringToPlotForDCRqMaxAndMetric \"${outputDirBase}/${baseFor_remoteT4100}_remoteT4100-vs-${metric}_CI.txt\" using 1:2:3 with yerr ps 2 lc $lc title \"${nRemoteUes} Remote UEs, Ntrx = ${remoteDCRqMax} \", '' using 1:2 with lines lc $lc dt $dt notitle,"
            lc=$(($lc+1))
          done #for nRemoteUes in $nRemoteUesArray      
          dt=$(($dt+1))
        done #remoteDCRqMax in $remoteDCRqMaxArray
        
        echo "reset 
              set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
              set output \"${outputDirBase}/set_relayTraffic-${relayTraffic}_relayTrafficOffMean-${relayTrafficOffMean}_Plot_remoteT4100-vs-${metric}_CI.png\"
              set title \" Parameters: relayTraffic: ${relayTraffic} \"
              set xlabel \"T4100 (number of SL periods)\"
              set ylabel \"${metric} (Mean and 95 % CI)\"
              set yrange [0:]
              set key outside right center
              plot $stringToPlotForDCRqMaxAndMetric " | gnuplot
                  
      done #for metric in $metricArray
    done #for relayTrafficOffMean in $relayTrafficOffMeanArray
  fi #if [ "$arrayLenght" -gt 1 ];then    


  relayTrafficOffMeanRealArray=( $relayTrafficOffMeanArray )
  arrayLenght=${#relayTrafficOffMeanRealArray[@]}
  if [ "$arrayLenght" -gt 1 ];then    
    echo "Generating relayTrafficOffMean-vs-metrics plot... "
    #Ploting for every pair: nRemoteUes-remoteT4100, Metric vs relayTrafficOffMean, one curve per remoteDCRqMax
    # Create One file per tuple {nRemoteUes,remoteT4100,remoteDCRqMax} containing $1=relayTrafficOffMean $2,3=Metric
    for nRemoteUes in $nRemoteUesArray
    do
      for remoteT4100 in $remoteT4100Array
      do
        for remoteDCRqMax in $remoteDCRqMaxArray
        do
          baseFor_relayTrafficOffMean="nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_remoteT4100-${remoteT4100}_remoteDCRqMax-${remoteDCRqMax}"
        
          for metric in $metricArray
          do
            echo -e "relayTrafficOffMean4100\tmean\tCI" > ${outputDirBase}/${baseFor_relayTrafficOffMean}_relayTrafficOffMean-vs-${metric}_CI.txt
          done #for metric in $metricArray
          
          for relayTrafficOffMean in $relayTrafficOffMeanArray
          do
            outputDirSet="${outputDirBase}/set_nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_relayTrafficOffMean-${relayTrafficOffMean}_remoteT4100-${remoteT4100}_remoteDCRqMax-${remoteDCRqMax}"
            
            for metric in $metricArray
            do
              #Access to the value 
              awk -v relayTrafficOffMean=${relayTrafficOffMean} '{if (relayTrafficOffMean == 100) { ULOcupancy=0; } else {ULOcupancy=(1/relayTrafficOffMean)*100} print ULOcupancy"\t"$0; }' $outputDirSet/${metric}_CI.txt >> ${outputDirBase}/${baseFor_relayTrafficOffMean}_relayTrafficOffMean-vs-${metric}_CI.txt
              
            done #for metric in $metricArray
          done #for relayTrafficOffMean in $relayTrafficOffMeanArray
          
        done #for remoteDCRqMax in $remoteDCRqMaxArray
      done #for remoteT4100 in $remoteT4100Array  
    done #for nRemoteUes in $nRemoteUesArray


    for nRemoteUes in $nRemoteUesArray
    do
      for remoteT4100 in $remoteT4100Array
      do
        # Construct the plot per pair nRemoteUes-remoteT4100 (and the other parameters)
        for metric in $metricArray
        do
          stringToPlotForRelayTrafficOffMeanAndMetric=''
          lc=1
          for remoteDCRqMax in $remoteDCRqMaxArray
          do
            baseFor_relayTrafficOffMean="nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_remoteT4100-${remoteT4100}_remoteDCRqMax-${remoteDCRqMax}"
            stringToPlotForRelayTrafficOffMeanAndMetric=" $stringToPlotForRelayTrafficOffMeanAndMetric \"${outputDirBase}/${baseFor_relayTrafficOffMean}_relayTrafficOffMean-vs-${metric}_CI.txt\" using 1:2:3 with yerr ps 2 lc $lc title \"remoteDCRqMax ${remoteDCRqMax}\", '' using 1:2 with lines lc $lc notitle,"
            lc=$(($lc+1))
          
          done #for nRemoteUes in $nRemoteUesArray
          #echo "$stringToPlotForRelayTrafficOffMeanAndMetric"        
          echo "reset 
                set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
                set output \"${outputDirBase}/set_nRemoteUes-${nRemoteUes}_relayTraffic-${relayTraffic}_remoteT4100-${remoteT4100}_Plot_relayTrafficOffMean-vs-${metric}_CI.png\"
                set title \" Parameters \n nRemoteUes: ${nRemoteUes}, relayTraffic:${relayTraffic}, \n  remoteT4100: ${remoteT4100} ms\"
                set xlabel \"UL Occupancy \"
                set ylabel \"${metric} (Mean and 95 % CI)\"
                set yrange [0:]
                set key outside right center
                plot $stringToPlotForRelayTrafficOffMeanAndMetric " | gnuplot
        done #for metric in $metricArray
     done #for remoteT4100 in $remoteT4100Array  
    done #for nRemoteUes in $nRemoteUesArray
  fi #if [ "$arrayLenght" -gt 1 ];then    

  echo "Done."

} #function RunEvaluation 

function RunSet {
  outputDirSet=$1
  parameters=$2
  echo -e "Running Set: $outputDirSet" 
  for ((runN=$startRun; runN<=$endRun; runN++))
  do  
    outputDir="${outputDirSet}/run${runN}"
    mkdir -p $outputDir
    cd $outputDir
    simParameters="$parameters --RngRun=${runN}"

    echo -e "Running Trial: $outputDir" 
    echo -e "outputDir: $outputDir \n simParameters: $simParameters" > info.txt 
    
    #Run the simulation
    ../../../build/scratch/$scenario $simParameters &

    n=$(($runN % $nThreads))
    if [ "$n" -eq 0 ];then
	     wait
    fi
 
    cd ../../.. #ns3/baseDir/SetDir/RunDir   Important!

  done # for ((runN=$startRun; runN<=$endRun; runN++))
} #function RunSet 



function ProcessSet {

  outputDirSet=$1
  nRemoteUes=$2
   
  sep="/set_"
  setParamsString="${outputDirSet#*$sep}"  
  echo -e "Processing Set: $outputDirSet nRemoteUes: $nRemoteUes" 
 
  #Obtain run metrics
  for ((runN=$startRun; runN<=$endRun; runN++))
  do  
    outputDir="${outputDirSet}/run${runN}"

    cd $outputDir

    echo -e "Processing Trial: $outputDir" 

    #### RUN METRICS ####################################        
    
    ### Success ratio - considering all Remotes in the sim ###
    awk 'BEGIN {startedCount=0; establishedCount=0;}
        {if ($4 == "S") {startedCount++;} if ($4 == "E") {establishedCount++;} } 
        END {if (startedCount > 0) {print establishedCount/startedCount }}' remoteConnectionEventsTrace.txt > allRemotesConnectionSuccessRatio.txt
    ### Average connection time (in terms of SL periods)- considering all Remotes in the sim ###
    awk -v slPeriod=${slPeriod} 'BEGIN {connCount=0; sumTime=0;}
        {if ($4 == "E") {connCount++; sumTime+=($6*1000.0/slPeriod);} } 
        END {if (connCount > 0) {print sumTime/connCount }}' remoteConnectionEventsTrace.txt > allRemotesAverageConnectionTime.txt   
                              
    n=$(($runN % $nThreads))
    if [ "$n" -eq 0 ];then
       wait
    fi
    
    cd ../../.. #ns3/baseDir/SetDir/RunDir   Important!

  done #  for ((runN=$startRun; runN<=$endRun; runN++))
  
  ######## SET METRICS ###########################
  metricArray="allRemotesConnectionSuccessRatio allRemotesAverageConnectionTime" 
  for metric in $metricArray
  do
  
    #Gathering all the data
    awk '{print $1;}' ${outputDirSet}/run*/${metric}.txt | sort -n > "${outputDirSet}/${metric}_AllData.txt"

    #Mean and 95% Confidence interval
    awk 'BEGIN{sum=0; count=0; sumForStd=0; }
    {
      if ($1 != "nan"){
        sum+=$1; count++; values[count]=$1;
      }
    }END{
      if (count > 0 ){
        mean=sum/count; 
        for (i in values) {
          delta=values[i]-mean; 
          sumForStd+=(delta^2); 
        }
        stdev = sqrt(sumForStd/count); 
        ci = 1.96 * (stdev/(sqrt (count)));
        #print mean, stdev;
        print mean"\t"ci;
      } 
      else {
        print "nan\tnan";
      }
    }'  "${outputDirSet}/${metric}_AllData.txt" | sort -g > "${outputDirSet}/${metric}_CI.txt"
    
    
  #Cleaning
  rm "${outputDirSet}/${metric}_AllData.txt"
  done #for metric in $metricArray
} #function ProcessSet 





if [ "$1" != "" ]; then
  # Compiling
  ./ns3 build
  # Running evaluation
  RunEvaluation "$1"
else
    echo -e "Error: Please provide an evaluation number (N)!"
    echo -e "Options:"
    echo -e "\tN\tDescription"
    echo -e "\t1\tImpact of Timer T4100 (CAMAD 2019)"
    echo -e "\t2\tImpact of relay UL traffic (CAMAD 2019)"
    echo -e "\t3\tShort example with small number of sets "
    echo -e "Usage:"
    echo -e "\t$ ./camad-2019-connection.sh <N>"
    exit
fi



