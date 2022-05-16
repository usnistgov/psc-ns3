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
# 2. Copy the scenario file (camad-2019-communication.cc) to the 'scratch'    # 
# folder                                                                      #
# 3. Copy this script to the ns3 root folder                                  #  
#                                                                             #
# Running the evaluation:                                                     #
# 1. Set the number of trials/run per set you desire to run by modifying the  #
# 'endRun' parameter below (CAMAD 2019 -> endRun=100)                         #
# 2. Save                                                                     #
# 3. From the ns-3 root folder run the script                                 #
#    E.g.                                                                     #
#       $ ./camad-2019-communication.sh                                       #
#-----------------------------------------------------------------------------#
###############################################################################

scenario="camad-2019-communication"

startRun=1
endRun=20
#endRun=100

nThreads=20

function RunEvaluation {

  #Compiling
  ./ns3 build

  appDurationTime="10"
  slPeriodArray="40 80 120 160 240 280 320"
  txTypeArray="Regular Relay Remote" 
  outputDirBase="output_camad-2019-communication"

  echo -e "Starting Evaluation... \n   Output: $outputDirBase"

  mkdir -p $outputDirBase

  rxTypeArray="Remote Relay Regular"
  metricArray="AverageMeanDelay AveragePacketLoss" 

  for txType in $txTypeArray
  do
    for slPeriod in $slPeriodArray
    do
      
      outputDirSet="${outputDirBase}/set_txType-${txType}_slPeriod-${slPeriod}"
     
      mkdir -p $outputDirSet
              
      parameters="--slPeriod=${slPeriod} --txType=${txType} --appDurationTime=${appDurationTime}" 
      RunSet $outputDirSet "$parameters" #&
      wait
      ProcessSet $outputDirSet $appDurationTime #&
      
    done #for slPeriod in $slPeriodArray
  done #for txType in $txTypeArray

  echo "Evaluation (Output: $outputDirBase) Done. "

  echo "Generating evaluation plots..."

  #Generating plots slPeriod-vs-${metric}, one curve per part Tx->Rx of interest
  for txType in $txTypeArray
  do
    for rxType in $rxTypeArray
    do    
      for metric in $metricArray
      do 
        echo -e "slPeriod\tmean\tci" > "${outputDirBase}/set_txType-${txType}_rxType-${rxType}_slPeriod-vs-${metric}_CI.txt"
      done
    done #for rxType in $rxTypeArray

    for slPeriod in $slPeriodArray
    do
      outputDirSet="${outputDirBase}/set_txType-${txType}_slPeriod-${slPeriod}"
      for metric in $metricArray
      do
        for rxType in $rxTypeArray
        do    
          awk -v slPeriod=${slPeriod} '{print slPeriod"\t"$0; }' $outputDirSet/${metric}_${rxType}_CI.txt >> "${outputDirBase}/set_txType-${txType}_rxType-${rxType}_slPeriod-vs-${metric}_CI.txt"
        done #for rxType in $rxTypeArray
        
      done 
    done #for slPeriod in $slPeriodArray
    
  done #for txType in $txTypeArray

  
  pairsOfInterest="Regular_Relay Regular_Remote Relay_Remote Remote_Regular Remote_Remote"
  for metric in $metricArray
  do
    stringToPlotnSlPeriodsVsMetric=''
    lc=1
    for pair in $pairsOfInterest
    do
      #Get tx and rx

      IFS='_'; 
      arrIN=($pair); 
      unset IFS;
      
      txType=${arrIN[0]} 
      rxType=${arrIN[1]} 

      #Build the plot 
      stringToPlotnSlPeriodsVsMetric=" $stringToPlotnSlPeriodsVsMetric \"${outputDirBase}/set_txType-${txType}_rxType-${rxType}_slPeriod-vs-${metric}_CI.txt\" using 1:2:3 with yerr ps 2 lc $lc title \"${txType} -> ${rxType}\", '' using 1:2 with lines lc $lc notitle,"
      lc=$(($lc+1))  
    done #for pair in $pairsOfInterest 
    
    #Plot
    echo "reset 
            set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
            set output \"${outputDirBase}/set_slPeriod-vs-${metric}_CI.png\"
            set xlabel \"Sl Period length (ms) \"
            set ylabel \"${metric} (Mean and 95 % CI)\"
            set yrange [0:]
            set key outside right center
            plot $stringToPlotnSlPeriodsVsMetric " | gnuplot
    
  done #for metric in $metricArray

  echo "Done. "

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
} #function RunSet {



function ProcessSet {

  outputDirSet=$1
  appDuration=$2
   
  sep="/set_"
  setParamsString="${outputDirSet#*$sep}"  
  echo -e "Processing Set: $outputDirSet " 
  
  rxTypeArray="Remote Relay Regular"
 
  #Obtain run metrics
  for ((runN=$startRun; runN<=$endRun; runN++))
  do  
    outputDir="${outputDirSet}/run${runN}"

    cd $outputDir

    echo -e "Processing Trial: $outputDir" 

    #### RUN METRICS ####################################        
  
    #Obtain number of transmitted packets 
    nTxPkts=`awk 'BEGIN {nTxPkts=0;} {if ($2 == "tx") {nTxPkts++;} }END{print nTxPkts;}' AppPacketTrace.txt`

    for rxType in $rxTypeArray
    do
      echo -n > MeanDelay_${rxType}_AllNodes.txt 
      echo -n > PacketLoss_${rxType}_AllNodes.txt 
      
      #Get Rx IDs  
      rxNodes=`awk -v rxType=${rxType} '{if ($2 == "rx" && $3 == rxType) {rxs[$4]++;}}END{for (rx in rxs){print rx;}}' AppPacketTrace.txt`

      #For each Rx ID
      for nodeId in $rxNodes
      do
        #Obtain Rx packets for the node (rx packets with PktSize ($5) = 12, are the 'ping' packets, ignore them)
        awk -v nodeId=${nodeId} -v rxType=${rxType} '{if ($3 == rxType && $4 == nodeId && $2 == "rx" && $5 != 12 ) {print $0;}}' AppPacketTrace.txt > AppPacketTrace_${rxType}_Node${nodeId}.txt
        
        #Calculate Packet Loss
        awk -v nTxPkts=${nTxPkts} 'BEGIN{count=0;}{count ++; }
        END {print (nTxPkts - count)/nTxPkts;}' AppPacketTrace_${rxType}_Node${nodeId}.txt > PacketLoss_${rxType}_Node${nodeId}.txt
        cat PacketLoss_${rxType}_Node${nodeId}.txt >> PacketLoss_${rxType}_AllNodes.txt
        rm PacketLoss_${rxType}_Node${nodeId}.txt

        #Calculate Mean Delay 
        awk 'BEGIN{sumDelay=0; count=0; }{count ++; sumDelay+=$7;}
        END {if (count > 0) {print sumDelay/count;}}' AppPacketTrace_${rxType}_Node${nodeId}.txt > MeanDelay_${rxType}_Node${nodeId}.txt   
        cat MeanDelay_${rxType}_Node${nodeId}.txt >> MeanDelay_${rxType}_AllNodes.txt 
        rm MeanDelay_${rxType}_Node${nodeId}.txt
        rm AppPacketTrace_${rxType}_Node${nodeId}.txt
          
      done #for nodeId in $rxNodes
  
      #Geting the averages 
      awk 'BEGIN { sum=0; count=0;}{sum+=$1; count++;} END{ if (count > 0) {print sum/count;} }' MeanDelay_${rxType}_AllNodes.txt  > AverageMeanDelay_${rxType}.txt
      awk 'BEGIN { sum=0; count=0;}{sum+=$1; count++;} END{ if (count > 0) {print sum/count;} }' PacketLoss_${rxType}_AllNodes.txt  > AveragePacketLoss_${rxType}.txt
      rm MeanDelay_${rxType}_AllNodes.txt
      rm PacketLoss_${rxType}_AllNodes.txt
      
    done #for rxType in $rxTypeArray
    
    #### END RUN METRICS ################################
                      
    n=$(($runN % $nThreads))
    if [ "$n" -eq 0 ];then
       wait
    fi
    
    cd ../../.. #ns3/baseDir/SetDir/RunDir   Important!

  done #  for ((runN=$startRun; runN<=$endRun; runN++))
 
  ######## SET METRICS ###########################
      
  for rxType in $rxTypeArray
  do
    metricArray="AverageMeanDelay AveragePacketLoss" 
    for metric in $metricArray
    do
    
      #Gathering all the data
      awk '{print $1;}' ${outputDirSet}/run*/${metric}_${rxType}.txt | sort -n > "${outputDirSet}/${metric}_${rxType}_AllData.txt"
      
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
      }'  "${outputDirSet}/${metric}_${rxType}_AllData.txt" | sort -g > "${outputDirSet}/${metric}_${rxType}_CI.txt"
      
      #Cleaning
      rm "${outputDirSet}/${metric}_${rxType}_AllData.txt"
    done #for metric in $metricArray
  done #for rxType in $rxTypeArray
} #function ProcessSet 


RunEvaluation

