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

OVERWRITE=0
if [[ "$#" -gt 0 && $1 == "-f" ]];then
OVERWRITE=1
fi

if [[ ! -d "scratch" ]];then
echo "ERROR: $0 must be copied to ns-3 directory!" 
exit
fi

SCENARIO="wns3-2017-discovery"
STARTRUN=1
ENDRUN=10 #Number of runs 10
STARTSEED=1 #Run number to start serie
MAXSEEDS=10 #Number of trials 500
STEPSEED=10 #step
UE=10 #Number of UEs
stime=15 #Simulation time in s
version="wns3" #Version for logging run output
tx=100
recovery=false

container="wns3-2017-discovery-scenarios"
mkdir -p $container

main_discovery_log_file="${container}/discovery-log.txt"
echo "$version: $ENDRUN runs, $MAXSEEDS seeds - $SCENARIO, $UE UEs in simulation, simTime=$stime, enableRecovery=$recovery" >> $main_discovery_log_file

./waf

for ((run=$STARTRUN; run<=$ENDRUN; run++))
do
  seed=$STARTSEED
  while [ $seed -le $MAXSEEDS ]
  do
    result=`ps aux | grep -i "wns3-2017-discovery*" | grep -i "build" | grep -v "grep" | wc -l`
    if [ $result -le $STEPSEED ]
    then
      max=`python -c "print $seed+$STEPSEED"`
      for ((i=$seed; i<$max; i++))
      do
        echo "RUN: ${version}-${run}-${i}"
        newdir="${container}/disc-${version}-${run}-${i}"
        if [[ -d $newdir && $OVERWRITE == "0" ]];then
          echo "$newdir exist! Use -f option to overwrite."
          continue
        fi
        mkdir -p $newdir
        OUTFILE="${newdir}/log-${version}-${run}-${i}.txt"
        rm -f $OUTFILE
        echo "UEs in simluation = $UE" >> $OUTFILE
        echo "Simulation time = $stime" >> $OUTFILE
        echo -e "-------------------------------\n" >> $OUTFILE
        ./waf --cwd=$newdir --run "$SCENARIO --RngRun=$run --RngSeed=$i --numUe=$UE --simTime=$stime --enableRecovery=$recovery" >> $OUTFILE 2>&1 &
        sleep 1 
      done
      seed=`python -c "print $seed+$STEPSEED"`
    else
      sleep 10
    fi
  done
done

result=`ps aux | grep -i "wns3-2017-discovery*"  | grep -i "build" | grep -v "grep" | wc -l`
while [ $result -ge 1 ]
do
  wait
  result=`ps aux | grep -i "wns3-2017-discovery*"  | grep -i "build" | grep -v "grep" | wc -l`
done

numues=$UE
numruns=$ENDRUN #runs
numseeds=$MAXSEEDS #trials
chosenue=1
periodlength=320 #Time in SlDchRxRrcStats.txt is in milliseconds
INFILE="SlDchRxRrcStats.txt"
discoverystart=2000 #Time in SlDchRxRrcStats.txt is in milliseconds

for j in $(seq 1 $numruns)
do
  outfile="${container}/discovery-cdf-${version}-ue${chosenue}-run${j}-${numseeds}trials.tr"
  rm -f $outfile
  
  for i in $(seq 1 $numseeds)
  do 
    #echo processing ${version}-${j}-${i} 
    dir="${container}/disc-${version}-${j}-${i}"
    cd $dir
    grep -v IMSI $INFILE | awk -v maxues=$numues '
      {
        if (! ($2, $8) in times) {
          times[$2, $8] = $1
        } 
      } 
      END{
        for (i=1; i<=maxues; i++) {
          maxtime=0
          count=0
          for (j=1; j<=maxues; j++) {
            if (i != j && (i, j) in times) {
              count++
              if (times[i, j] > maxtime) {
                maxtime = times[i, j]
              }
            }
          }
          print i, maxtime, count, count == (maxues - 1)?"OK":"ERROR"
        } 
      }' > discovery-first.txt
    cd ../..
  done
  errors=$(grep -c ERROR ${container}/disc-${version}-${j}-*/discovery-first.txt | grep -v 0$)
  if [ $? -eq 1 ]
  then
    head -qn$chosenue ${container}/disc-${version}-${j}-*/discovery-first.txt | sort -n | tail -n $numseeds | awk -v periodlength=$periodlength -v discoverystart=$discoverystart '
    {
      printf "%3.0f\n", (($2 - discoverystart)/ periodlength) + 0.5
    }' | awk -v numseeds=$numseeds '
    BEGIN{
      maxperiod=1
    }
    {
      count[$1]++; 
      if ($1 > maxperiod) {
        maxperiod = $1
      }
    }
    END{
      cdf=0
      for (i=1; i <= maxperiod; i++) {
        if (i in count) {
          #mean+=i*count[i]
          cdf+= count[i]
          print i, count[i], cdf, cdf*1.0/numseeds
        } else {
          print i, 0, cdf, cdf*1.0/numseeds
        }
      }
      #print ""
      #print "Mean=", mean*1.0/numseeds
    }'
  else 
    echo $errors
  fi > $outfile
done 

outfile="${container}/discovery-cdf-${version}-ue${chosenue}-average.tr"
cat ${container}/discovery-cdf-${version}-ue${chosenue}-run*-${numseeds}trials.tr | awk -v numseeds=$numseeds -v numruns=$numruns '
BEGIN{
  maxperiod=1
  totalsims=numseeds*numruns
}
{
  count[$1]+=$2
  if ($1 > maxperiod) {
    maxperiod = $1
  }
}
END{
  cdf=0
  for (i=1; i <= maxperiod; i++) {
    if (i in count) {
      cdf+= count[i]
      print i, count[i], cdf, cdf*1.0/totalsims
    } else {
      print i, 0, cdf, cdf*1.0/totalsims
    }
  }
}' >  $outfile

gnuplot -persist << EOF
  #set term x11 1
  set term png
  set out "wns3-2017-discovery-cdf.png"
  set style line 81 lt 0
  set style line 81 lt rgb "#808080"
  set grid back linestyle 81
  set border 3 back linestyle 80 
  set xtics nomirror
  set ytics nomirror
  unset surface
  set autoscale   
  unset log       
  unset label     
  set xtic auto   
  set ytic auto 
  set key on  
  set xlabel "Number of periods"
  set ylabel "CDF" 
  set key right bottom
  set title "CDF of UEs Discovered: 10 UEs and 50 Reources"
  plot "$outfile" using 1:4 title " ns-3 simulations" with steps
EOF

display "wns3-2017-discovery-cdf.png"
