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

#-------------Usage:-------------------------------------------------------------#
# Each evaluation is a string with the format:                                   #
#     {evalname}_{resp}_{grp}_{rings}_{stime}_{onoff}_{syncTxThreshOoC}          #
#                                                                                #
# To run several evaluations separate them with a space -->" "<--                # 
# e.g.:                                                                          #
#     evaluations="{Evaluation1} {Evaluation2} {Evaluation n}"                   #
#--------------------------------------------------------------------------------#

#-------------To run the evaluation on the WNS3 2017 paper-----------------------#
# 1. Comment the simplified version of the evaluation                            # 
# 2. Unncomment the Paper version of the evaluation                              #
# 3. Change the number of Runs to 50 (MAXRUNS=50)                                # 
# 4. Save and Execute                                                            #
#--------------------------------------------------------------------------------#


#Simplified version: (2 rings, 63 transmitters, 0 receivers, 50 s of simulation time)
evaluations="syncTxThreshOoC-110-OnOff_3_63_2_50_true_-110 syncTxThreshOoC-100-OnOff_3_63_2_50_true_-100 syncTxThreshOoC-90-OnOff_3_63_2_50_true_-90 syncTxThreshOoC-80-OnOff_3_63_2_50_true_-80 syncTxThreshOoC-70-OnOff_3_63_2_50_true_-70 syncTxThreshOoC-60-OnOff_3_63_2_50_true_-60 syncTxThreshOoC-110-AllOn_3_63_2_50_false_-110 syncTxThreshOoC-100-AllOn_3_63_2_50_false_-100 syncTxThreshOoC-90-AllOn_3_63_2_50_false_-90 syncTxThreshOoC-80-AllOn_3_63_2_50_false_-80 syncTxThreshOoC-70-AllOn_3_63_2_50_false_-70 syncTxThreshOoC-60-AllOn_3_63_2_50_false_-60"

#Paper version (very time consuming): (2 rings, 63 transmitters, 0 receivers, 450 s of simulation time)
#evaluations="syncTxThreshOoC-110-OnOff_3_63_2_450_true_-110 syncTxThreshOoC-100-OnOff_3_63_2_450_true_-100 syncTxThreshOoC-90-OnOff_3_63_2_450_true_-90 syncTxThreshOoC-80-OnOff_3_63_2_450_true_-80 syncTxThreshOoC-70-OnOff_3_63_2_450_true_-70 syncTxThreshOoC-60-OnOff_3_63_2_450_true_-60 syncTxThreshOoC-110-AllOn_3_63_2_450_false_-110 syncTxThreshOoC-100-AllOn_3_63_2_450_false_-100 syncTxThreshOoC-90-AllOn_3_63_2_450_false_-90 syncTxThreshOoC-80-AllOn_3_63_2_450_false_-80 syncTxThreshOoC-70-AllOn_3_63_2_450_false_-70 syncTxThreshOoC-60-AllOn_3_63_2_450_false_-60"


for e in $evaluations
do
  echo -e "----------------------------------------------------------------------------"
        echo $e
  echo -e "----------------------------------------------------------------------------"

        IFS='_' read -r -a array <<< "$e"

        #Evaluation parameters 
        evalname=${array[0]}
        resp=${array[1]} #Responder UEs per sector
        grp=${array[2]} #Number of broadcast groups in the whole topology (Number of transmitters)
        rings=${array[3]} #Topology Rings
        nSectors=3
        isd=1732 #Inter Site Distance

        nThreads=1 #Number of simultaneous runs to be executed for each evaluation

        #Simulation parameters
        scenario="wns3-2017-synch"  
        STARTRUN=1 #Run number to start serie
        MAXRUNS=5 #Number of runs 5 used for evaluations
        stime=${array[4]} #Simulation time in s

        #Communication protocol parameters
        slMcs=10 #MCS for D2D communication
        slGrantRbs=2 #grant size for D2D communication
        slKtrp=2 #Index for repetition pattern (determiness number of subframes for D2D)
  slPeriod=40 #(ms)
        pscchRbs=22
        pscchTrp="0x00000000FF"
        ctrlerror=0 #0 for disabled or 1 enabled; when disabled, bypass errors in PSCCH and PSBCH to evaluate PSSCH only.
  dropOnCollision=0 #
  rPckSize=7 #[bytes] Responder's pck size
        onoff=${array[5]} #On/Off traffic pattern 
        rPckInt=0.02 #Packet interval time in seconds for responders
        rMaxPck=100000000 #Used only for constant UDP appliation; it is disregarded when onoff app is enable.

        #Synchronization protocol parameters
        syncTxThreshOoC=${array[6]}
        syncRefDiffHyst=0
        syncRefMinHyst=0
        filterCoefficient=0
        scanTime=40
        measTime=400
        evalTime=400

        #Synchronization scenario parameters
        unsyncSl="true" #if true: random frame/subframe indication and random SLSSID
        slSyncActive="true" # if true: activate SLSS transmission and schedule SyncRef seleciton
        firstScanTimeMin=0
        firstScanTimeMax=2000 
        interScanTimeMin=1000
        interScanTimeMax=1000

        ver="d2d-sync-run" #Version for logging run output
        basedir="wns3-2017-synchronization-"${evalname}"/"

        arguments=" --responders=$resp --groups=$grp --ring=$rings --isd=$isd --simTime=$stime --respMaxPkt=$rMaxPck --respPktSize=$rPckSize --respPktIntvl=$rPckInt --slPeriod=$slPeriod --mcs=$slMcs --ktrp=$slKtrp --rbSize=$slGrantRbs --pscchRbs=$pscchRbs --pscchTrp=$pscchTrp --ctrlError=$ctrlerror --dropOnCol=$dropOnCollision --onoff=$onoff --syncTxThreshOoC=$syncTxThreshOoC --filterCoefficient=$filterCoefficient --syncRefMinHyst=$syncRefMinHyst --syncRefDiffHyst=$syncRefDiffHyst  --interScanTimeMin=$interScanTimeMin --interScanTimeMax=$interScanTimeMax --scanTime=$scanTime --measTime=$measTime --evalTime=$evalTime --firstScanTimeMin=$firstScanTimeMin --firstScanTimeMax=$firstScanTimeMax --unsyncSl=$unsyncSl --slSyncActive=$slSyncActive "

        ./waf #First compilation to avoid problems of simultaneous compilation when using simultaneous execution

        echo "The experiment ($STARTRUN - $MAXRUNS runs) : $evalname" 
        current_time=$(date "+%Y-%m-%d %H:%M:%S")
        echo "Simulations Start Time : $current_time"

        for ((run=$STARTRUN; run<=$MAXRUNS; run++))
        do
                newdir="${basedir}${ver}${run}"
                mkdir -p $newdir
                OUTFILE="${newdir}/log-${ver}${run}.txt"
                rm -f $OUTFILE
                runinfo="$scenario, RUN: ${ver}${run}"
           echo -e " $runinfo saved to dir: \n  - ${newdir}"
                run_args="--RngSeed=$run $arguments"
                echo -e "$runinfo, $run_args " >> $OUTFILE

           ./waf --cwd=$newdir --run "$scenario $run_args" >> $OUTFILE 2>&1 &

                 n=$(($run % $nThreads)) 
                 if [ "$n" -eq 0 ];then
                    wait
                 fi

            done

            wait

            current_time=$(date "+%Y-%m-%d %H:%M:%S")
            echo -e "\nSimulations End Time : $current_time"
            echo -e "Simulations Duration: $SECONDS seconds"

            stimeMs=$(($stime*1000))
      sites=$((1+6*$rings*($rings-1)/2))
            nSectorsTotal=$(($nSectors*$sites))

      for ((run=$STARTRUN; run<=$MAXRUNS; run++))
      do
            dir="${basedir}${ver}${run}"
            dirIn=$dir
            dirOut=$dir/ProcessedSync
            mkdir -p $dirOut

      echo -e " Processing run $run... Synchronization statistic saved to dir: \n  - $dirOut"

      ###Number of transmitters transmitting SLSSs at the end of the simulation
      awk -v stime="$stimeMs" 'BEGIN{count=0;}{if ($1 <= stime && $1 > (stime -400) ) {a[$2]=1;}}END{for (i in a) {count++;} print count;}' $dirIn/TxSlss.txt > $dirOut/FinalNumberTxTransmittingSLSS.txt 
        
      n=$(($run % $nThreads)) 
            if [ "$n" -eq 0 ];then
              wait
            fi

      done

      wait

      if [[ $(($MAXRUNS - $STARTRUN)) -gt 0 ]];then
         echo -e "\nGenerating aggregated stat (averages + confidence intervals)..."

         dir="${basedir%?}-AggregatedStats-Synchronization"
        mkdir -p $dir

        #Calculate confidenceInterval of the number of transmitters transitting SLSSs at the end of the simulations
                awk 'BEGIN{ FS="\t";} {if($1 != "n") {ln++; avg[ln]=$1; sum=sum+avg[ln];}}END{ average=sum/ln; for(i=1;i<=ln;i++){ gap = avg[i]-average; sum2=sum2+gap*gap;} variance=sqrt(sum2/(ln-1)); c_i=1.96*variance/sqrt(ln); print average, c_i; }'  ${basedir}${ver}*/ProcessedSync/FinalNumberTxTransmittingSLSS.txt > "$dir/FinalNumberTxTransmittingSLSS-CI.txt"

        max=`awk ' BEGIN{ max =0;} { if ($1>max) {max = $1; } } END {print max}' $dir/FinalNumberTxTransmittingSLSS-CI.txt`
        echo " reset 
                set terminal png nocrop enhanced size 800,600
                set style fill solid 0.5 
                set style histogram errorbars gap 2 lw 2
                set style data histograms
                set yrange [0:2*$max]
                set title \"Number of transmitters transmitting SLSSs \n at the end of the simulation \"
                set output '$dir/FinalNumberTxTransmittingSLSS-CI.png'
                plot '$dir/FinalNumberTxTransmittingSLSS-CI.txt' notitle, '$dir/FinalNumberTxTransmittingSLSS-CI.txt' using (0.125):($max-(0.025*$max)):1 with labels offset 2 notitle" | gnuplot

        else
              echo "Can not generate aggregated stats. Total runs should be greater than $MAXRUNS"

        fi

        echo -e "\nSynchronization evaluation End Time : $current_time"
        echo -e "Synchronization evaluation Duration: $SECONDS seconds"


       echo -e "\nExperiment $evalname (simulation + evaluation) duration: $SECONDS seconds"
       echo -e "----------------------------------------------------------------------------"
   done

   echo -e "\nAll experiments duration: $SECONDS seconds \n\n"

   echo -e "\nGenerating final plot..."
   fPlotFilename=wns3-2017-synchronization-syncTxThreshOoC
   awk '{split(FILENAME,a,"-"); print a[6]"\t-"a[5]"\t"$1"\t"$2;}' wns3-2017-synchronization-syncTxThreshOoC*-AggregatedStats-Synchronization/FinalNumberTxTransmittingSLSS-CI.txt | sort -k1,1 -k2,2n | awk '{if (NR == 1){print "\""$1"\""; print $0; a=$1;} else {if (a!= $1) {print "\n""\n""\""$1"\"""\n"$0; a=$1 } else {print$0; a=$1};}}' > "$fPlotFilename.txt"
   echo "  reset  
    set terminal png nocrop enhanced size 800,600
    set output '$fPlotFilename.png'
    set offset 0.5,0.5,0,0
    set xlabel 'syncTxThreshOoC (dBm)'
    set yrange [0:80]
    set title 'Number of transmitters transmitting SLSS at the end of the simulation'
    plot '$fPlotFilename.txt' index 0 u 3:4:xticlabels(2) w yerrorbars ls 1 title columnhead(1),  '' index 1 u 3:4:xticlabels(2) w yerrorbars ls 2 title columnhead(1), '$fPlotFilename.txt'  index 0 u 3:xticlabels(2) w lines ls 1
    title columnhead(1), '' index 1 u 3:xticlabels(2) w lines ls 2  title columnhead(1)"  | gnuplot

    echo -e "Final plot generated: $fPlotFilename.png"

