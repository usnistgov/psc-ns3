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
# 2. Copy the scenario file (camad-2019-discovery.cc) to the 'scratch' folder #
# 3. Copy this script to the ns3 root folder                                  #  
#                                                                             #
# Running the evaluation:                                                     #
# 1. Set the number of trials/run per set you desire to run by modifying the  #
# 'endRun' parameter below (CAMAD 2019 -> endRun=100)                         #
# 2. Save                                                                     #
# 3. From the ns-3 root folder run the script with the evaluation number as   #
# argument                                                                    #                                
#    E.g.                                                                     #
#       $ ./camad-2019-discovery.sh 3                                         #
#                                                                             #
# The available evaluations are                                               #
#    - Eval #1: 6 Relay UEs and variable number of Remote UEs (CAMAD 2019)    #
#    - Eval #2: 6 Remote UEs and variable number of Relay UEs (CAMAD 2019)    #
#    - Eval #3: Short example with small number of sets (default)             #
#-----------------------------------------------------------------------------#
###############################################################################

scenario="camad-2019-discovery"

startRun=1
endRun=20
#endRun=100 

nThreads=20

function RunEvaluation {

  evalNumber=$1

  #Evals CAMAD 2019 paper
  #Common parameters 
  simTime="40" #s
  txProbArray="100 75 50 25" #Transmission probability  (4 values)
  ntArray="5" #Number of subframes per period
  nfArray="2" #Number of resources
  modelArray="A B" #(2 values)

  if [ "$evalNumber" -eq 1 ];then
    ###Eval 1: 6 Relay UEs and variable number of Remote UEs (4 * 2 * 11 = 88 sets)### 
    nyArray="6" #Number of relays
    nxArray="1 2 4 6 8 10 12 14 16 18 20" #Number of remotes (11 values)
    outputDirBase="output_camad-2019-discovery-6Relays"
  elif [ "$evalNumber" -eq 2 ];then
    ###Eval 2: 6 Remote UEs and variable number of Relay UEs (4 * 2 * 11 = 88 sets)###
    nyArray="1 2 4 6 8 10 12 14 16 18 20" #Number of relays (11 values)
    nxArray="6" #Number of remotes
    outputDirBase="output_camad-2019-discovery-6Remotes"
  elif [ "$evalNumber" -eq 3 ];then 
    ###Eval 3: Short evaluation with reduced number of sets (2 * 2 * 3 = 24 sets)###
    txProbArray="100 50" #Transmission probability (2 values)
    nyArray="1 2 6" #Number of relays (3 values)
    nxArray="6" #Number of remotes (1 value)
    outputDirBase="output_camad-2019-discovery_Example"
  else 
    echo "ERROR! Invalid evaluation number"
    exit
  fi
  
  #Running the evaluation
  mkdir -p $outputDirBase

  for txProb in $txProbArray
  do
    for nt in $ntArray
    do
      for nf in $nfArray
      do
        for ny in $nyArray
        do     
          setBaseForNy="set_txProb-${txProb}_nt-${nt}_nf-${nf}_ny-${ny}"
          for nx in $nxArray
          do        
            setBaseForCompareModels="set_txProb-${txProb}_nt-${nt}_nf-${nf}_ny-${ny}_nx-${nx}"
            for model in $modelArray
            do         
              outputDirSet="${outputDirBase}/set_txProb-${txProb}_nt-${nt}_nf-${nf}_ny-${ny}_nx-${nx}_model-${model}"
              mkdir -p $outputDirSet
              
              parameters="--simTime=${simTime} --txProb=${txProb} --nSubframes=${nt} --discPrbNum=${nf} --nRelayUes=${ny} --nRemoteUes=${nx} --discModel=${model}" 
              
              RunSet $outputDirSet "$parameters" #&
              
              wait
              
              ProcessSet $outputDirSet $ny $nx $simTime #&

            done #for model in $modelArray
                                   
            wait
            
          done #for nx in $nxArray
         
        done #for ny in $nyArray  
      
      done #for nf in $nfArray
      
    done #for nt in $ntArray

  done #for txProb in $txProbArray


  #Ploting evaluation results
  allNumTxMetrics="NumberOfDiscoveryMessagesSentByAllRelays"
  allTimeMetrics="TimeAllRemotesDiscoverAllRelays"

  nxRealArray=( $nxArray )
  nxLenght=${#nxRealArray[@]}
  if [ "$nxLenght" -gt 1 ];then      
    ###All txProb and Models for a given Ny, depending on nx
    for nt in $ntArray
      do
      for nf in $nfArray
      do
        lc=1
        for timeMetric in $allTimeMetrics
        do
          for ny in $nyArray
          do
            stringToPlotForNy=''
            dt=2
            for model in $modelArray
            do
              for txProb in $txProbArray
              do       
                setBaseForNy="set_txProb-${txProb}_nt-${nt}_nf-${nf}_ny-${ny}"
                echo -e "nx\tMean\tCI" > ${outputDirBase}/${setBaseForNy}_${timeMetric}_CI_model${model}.txt
                for nx in $nxArray
                do 
                  awk -v nx=$nx '{print nx"\t"$0; }' ${outputDirBase}/${setBaseForNy}_nx-${nx}_model-${model}/${timeMetric}_CI.txt >> ${outputDirBase}/${setBaseForNy}_${timeMetric}_CI_model${model}.txt
                done                  
                stringToPlotForNy=" $stringToPlotForNy \"${outputDirBase}/${setBaseForNy}_${timeMetric}_CI_model${model}.txt\" using 1:2:3 with yerr lc $lc ps 2 title \"Model ${model} - txProb ${txProb}\", '' using 1:2 with lines lc $lc dt $dt notitle,"
                            
                lc=$(($lc+1))                                   
              done #for txProb in $txProbArray
              dt=$(($dt-1))
            done #for model in $modelArray
            echo "reset 
                  set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
                  set output \"${outputDirBase}/set_ny-${ny}_${timeMetric}_CI.png\"
                  set title \" ${timeMetric} \n Ny = ${ny} Relays \" noenhanced
                  set xlabel \"Nx\"
                  set ylabel \"Number of discovery periods (Mean and 95 % CI)\"
                  set xrange [0:$nx+2]
                  set yrange [1:]
                  set logscale y 10
                  set key outside right center
                  plot $stringToPlotForNy " | gnuplot
           
            
          done #for ny in $nyArray  
        done #for timeMetric in $allTimeMetrics

        lc=1
        for numTxMetric in $allNumTxMetrics
        do
          for ny in $nyArray
          do
            stringToPlotForNy=''
            dt=2
            for model in $modelArray
            do
              for txProb in $txProbArray
              do       
                setBaseForNy="set_txProb-${txProb}_nt-${nt}_nf-${nf}_ny-${ny}"
                echo -e "nx\tMean\tCI" > ${outputDirBase}/${setBaseForNy}_${numTxMetric}_CI_model${model}.txt
                for nx in $nxArray
                do 
                  awk -v nx=$nx '{print nx"\t"$0; }' ${outputDirBase}/${setBaseForNy}_nx-${nx}_model-${model}/${numTxMetric}_CI.txt >> ${outputDirBase}/${setBaseForNy}_${numTxMetric}_CI_model${model}.txt
                done    
                stringToPlotForNy=" $stringToPlotForNy \"${outputDirBase}/${setBaseForNy}_${numTxMetric}_CI_model${model}.txt\" using 1:2:3 with yerr lc $lc ps 2 title \"Model ${model} - txProb ${txProb}\", '' using 1:2 with lines lc $lc dt $dt notitle,"
                lc=$(($lc+1))
              done #for txProb in $txProbArray
              dt=$(($dt-1))
            done #for model in $modelArray
            echo "reset 
                  set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
                  set output \"${outputDirBase}/set_ny-${ny}_${numTxMetric}_CI.png\"
                  set title \" ${numTxMetric} \n Ny = ${ny} Relays \" noenhanced
                  set xlabel \"Nx\"
                  set ylabel \"Number of transmissions (Mean and 95 % CI)\"
                  set xrange [0:$nx+2]
                  set yrange [0:]
                  set key outside right center
                  plot $stringToPlotForNy " | gnuplot
                             
          done #for ny in $nyArray  
        done #for numTxMetric in $allNumTxMetrics        
      done #for nf in $nfArray
    done #for nt in $ntArray
    ###END All txProb and Models for a given Ny, depending on nx
   
  fi #if [ "$nxLenght" -gt 1 ];then


  nyRealArray=( $nyArray )
  nyLenght=${#nyRealArray[@]}
  if [ "$nyLenght" -gt 1 ];then
    #All txProb and Models for a given Nx, depending on Ny
    for nt in $ntArray
      do
      for nf in $nfArray
      do
        lc=1
        for timeMetric in $allTimeMetrics
        do

        for nx in $nxArray
          do
            stringToPlotForNx=''
            dt=2

            for model in $modelArray
            do
              for txProb in $txProbArray
              do
                setBase="set_txProb-${txProb}_nt-${nt}_nf-${nf}"
                echo -e "ny\tMean\tCI" > ${outputDirBase}/${setBase}_nx-${nx}_${timeMetric}_CI_model${model}.txt
                for ny in $nyArray
                do 
                  awk -v ny=$ny '{print ny"\t"$0; }' ${outputDirBase}/${setBase}_ny-${ny}_nx-${nx}_model-${model}/${timeMetric}_CI.txt >> ${outputDirBase}/${setBase}_nx-${nx}_${timeMetric}_CI_model${model}.txt
                done
                        
                stringToPlotForNx=" $stringToPlotForNx \"${outputDirBase}/${setBase}_nx-${nx}_${timeMetric}_CI_model${model}.txt\" using 1:2:3 with yerr lc $lc ps 2 title \"Model ${model} - txProb ${txProb}\", '' using 1:2 with lines lc $lc dt $dt notitle,"
                          
                lc=$(($lc+1))

              done #for txProb in $txProbArray
              dt=$(($dt-1))
            done #for model in $modelArray
            echo "reset 
                  set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
                  set output \"${outputDirBase}/set_nx-${nx}_${timeMetric}_CI.png\"
                  set title \" ${timeMetric} \n Nx = ${nx} Remotes \" noenhanced
                  set xlabel \"Ny\"
                  set ylabel \"Number of discovery periods (Mean and 95 % CI)\"
                  set xrange [0:$ny+2]
                  set yrange [1:]
                  set logscale y 10
                  set key outside right center
                  plot $stringToPlotForNx " | gnuplot   
                  
          done #for nx in $nxArray  
        done #for timeMetric in $allTimeMetrics

        lc=1
        for numTxMetric in $allNumTxMetrics
        do
          for nx in $nxArray
          do
            stringToPlotForNx=''
            dt=2
            for model in $modelArray
            do
              for txProb in $txProbArray
              do       
                setBase="set_txProb-${txProb}_nt-${nt}_nf-${nf}"
                echo -e "ny\tMean\tCI" > ${outputDirBase}/${setBase}_nx-${nx}_${numTxMetric}_CI_model${model}.txt
                for ny in $nyArray
                do 
                  awk -v ny=$ny '{print ny"\t"$0; }' ${outputDirBase}/${setBase}_ny-${ny}_nx-${nx}_model-${model}/${numTxMetric}_CI.txt >> ${outputDirBase}/${setBase}_nx-${nx}_${numTxMetric}_CI_model${model}.txt
                done

                stringToPlotForNx=" $stringToPlotForNx \"${outputDirBase}/${setBase}_nx-${nx}_${numTxMetric}_CI_model${model}.txt\" using 1:2:3 with yerr lc $lc ps 2 title \"Model ${model} - txProb ${txProb}\", '' using 1:2 with lines lc $lc dt $dt notitle,"
                lc=$(($lc+1))
              done #for txProb in $txProbArray
              dt=$(($dt-1))
            done #for model in $modelArray
            echo "reset 
                  set terminal pngcairo nocrop enhanced size 1280,800 font 'Helvetica,16
                  set output \"${outputDirBase}/set_nx-${nx}_${numTxMetric}_CI.png\"
                  set title \" ${numTxMetric} \n Nx = ${nx} Remotes \" noenhanced
                  set xlabel \"Ny\"
                  set ylabel \"Number of transmissions (Mean and 95 % CI)\"
                  set xrange [0:$ny+2]
                  set yrange [0:]
                  set key outside right center
                  plot $stringToPlotForNx " | gnuplot
                            
          done #for nx in $nxArray  
        done #for numTxMetric in $allNumTxMetrics

      done #for nf in $nfArray
    done #for nt in $ntArray
    #END All txProb and Models for a given Nx, depending on Ny
  fi #if [ "$nyLenght" -gt 1 ];then

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
  ny=$2
  nx=$3
  simTime=$4

  sep="/set_"
  setParamsString="${outputDirSet#*$sep}"  
  echo -e "Processing Set: $outputDirSet ny: $ny nx: $nx" 
 
  #Obtain run metrics
  for ((runN=$startRun; runN<=$endRun; runN++))
  do  
    outputDir="${outputDirSet}/run${runN}"

    cd $outputDir
    
    randomRelayId=`awk -v runN=$runN -v ny=$ny 'BEGIN{srand(runN);}{}END{print (1 + int(rand()*ny)); }' info.txt`
    randomRemoteId=`awk -v runN=$runN -v ny=$ny -v nx=$nx 'BEGIN{srand(runN);}{}END{print (ny+1 + int(rand()*nx)); }' info.txt`

    echo -e "Processing Trial: $outputDir randomRelayId: $randomRelayId randomRemoteId: $randomRemoteId" 


       
    #Time for All Remote UEs to discover All Relay UEs	-		1 value
    awk -v  nx=$nx 'END{ if (FNR == (nx + 1)){ print $4"\t"$5;} else {print "nan\tnan";}}' TimesEachRemoteDiscoversAllRelays.txt > TimeAllRemotesDiscoverAllRelays.txt
   
    # The column $15 of SlDchMacStats.txt is the 'ContentType', which is 
    # equal to '4' for discovery messages sent by the relays 
    #(i.e., RelayAnnouncements and RelayResponses) 
    
    ### Number of discovery messages transmited by all relays in simulation time
    startEvalTime=4000 # (ms) remotesStartTime
    endEvalTime=$(($simTime*1000))
    awk -v startEvalTime=$startEvalTime -v endEvalTime=$endEvalTime 'BEGIN {count=0; evalPeriodLength=endEvalTime-startEvalTime; }
      {if ($1 != "Time") {if ($1 >= startEvalTime && $1 <= endEvalTime && $15 == 4) { count+=1;} else if ($1 > endEvalTime){exit;} } }
      END {print evalPeriodLength"\t"count;}' SlDchMacStats.txt > NumberOfDiscoveryMessagesSentByAllRelays.txt
      
    n=$(($runN % $nThreads))
    if [ "$n" -eq 0 ];then
       wait
    fi
    
    cd ../../.. #ns3/baseDir/SetDir/RunDir   Important!

  done #  for ((runN=$startRun; runN<=$endRun; runN++))
  
  #Obtain set metrics
  
  #### Time metrics ####
  allTimeMetrics="TimeAllRemotesDiscoverAllRelays"
  
  for timeMetric in $allTimeMetrics
  do
    #### Number of Periods ($2) #####
    #Gathering all the data
    awk '{print $2;}' ${outputDirSet}/run*/${timeMetric}.txt | sort -n > "${outputDirSet}/${timeMetric}_AllData.txt"
  
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
    }'  "${outputDirSet}/${timeMetric}_AllData.txt" | sort -g > "${outputDirSet}/${timeMetric}_CI.txt"
    
    
    #Cleaning
    rm "${outputDirSet}/${timeMetric}_AllData.txt" 
    
  done # for timeMetric in $allTimeMetrics
   
   
  #### Number of Transmissions metrics ####
  allNumTxMetrics="NumberOfDiscoveryMessagesSentByAllRelays"

 
  for numTxMetric in $allNumTxMetrics
  do

    #### Number of Transmissions ($2) #####
    #Gathering all the data
    awk '{print $2;}' ${outputDirSet}/run*/${numTxMetric}.txt | sort -n > "${outputDirSet}/${numTxMetric}_AllData.txt"
  
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
    }'  "${outputDirSet}/${numTxMetric}_AllData.txt" | sort -g > "${outputDirSet}/${numTxMetric}_CI.txt"
    
    
    #Cleaning
    rm "${outputDirSet}/${numTxMetric}_AllData.txt" 
    
  done #for numTxMetric in $allNumTxMetrics
  
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
    echo -e "\t1\t6 Relay UEs and variable number of Remote UEs (CAMAD 2019)"
    echo -e "\t2\t6 Remote UEs and variable number of Relay UEs (CAMAD 2019)"
    echo -e "\t3\tShort example with small number of sets "
    echo -e "Usage:"
    echo -e "\t$ ./camad-2019-discovery.sh <N>"
    exit
fi


