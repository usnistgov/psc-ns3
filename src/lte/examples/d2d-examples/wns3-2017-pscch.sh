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

SCENARIO="wns3-2017-pscch"
SIMULATION_TIME=5 #Change to 81 seconds to simulate 1000 sidelink periods of 80 ms
PSCCH_RB_M=22 
MCS=10
PSSCH_SUBCHANNEL_RBS=2
KTRP=2
PERIOD_LENGTH=80
PACKET_PAYLOAD_SIZE=7
MIN_UES_PER_SECTOR=3 #3 used for evaluations
MAX_UES_PER_SECTOR=3 #33 used for evaluations
PSCCH_SF_BITMAPS="0x00000000FF 0xFFFFFFFFFF" #Bitmap must be 40 bits


function run_config ()
{
if [ "$#" -ne 9 ];then
    echo "Expects 9 arguments: SIMULATION_TIME SL_PERIOD MCS GRANT_SIZE KTRP PKT_SIZE PSCCH_RBs PSCCH_TRP(hexmap) RESPONDERS"
    exit
fi

if [[ ! -d "scratch" ]];then
echo "ERROR: $0 must be copied to ns-3 root directory!" 
exit
fi

if [[ ! -e "scratch/$SCENARIO.cc" ]];then
echo "ERROR: $SCENARIO.cc must be copied to scratch folder!" 
exit
fi

stime=$1 #Simulation time in s. Add 1 second to desired simulation time to allow for configuration.
SL_PERIOD=$2 #Sidelink period in milliseconds
SL_MCS=$3 #MCS for D2D communication
SL_GRANT_RB=$4 #grant size for D2D communication, number of RBs per subchannels
SL_KTRP=$5 #Index for repetition pattern (determiness number of subframes for D2D)
r_pck_size=$6 #[bytes] Responder's pck size
PSCCH_RBs=$7  #Number of RBs for PSCCH pool.
PSCCH_TRP=$8  #PSCCH subframe bitmap
RESP=$9 #Responder UEs per sector

Rings=1 #Topology Rings
sites=$((1+6*$Rings*($Rings-1)/2)) #Number of cell sites
sectors=$(($sites*3)) #Total number of sectors, i.e., 3 sectors per site.
GRP_SECTOR=$RESP #Groups per sectors
GRP=$(($sectors*$GRP_SECTOR)) #Number of D2D groups in the whole topology

STARTRUN=1 #Run number to start
MAXRUNS=1 #Number of runs 5 used for evaluations
isd=10 #Inter Site Distance
min_center_dist=1 #Minimum deploy distance to center of cell site
GRPrx=0 #Number of receiver UEs in D2D groups
r_pck_int=0.005 #Packet interval time in seconds for responders
r_max_pck=100000000 #Used only for constant UDP appliation; it is disregarded when onoff app is enable.
ctrlerror=1 #0 for disabled or 1 enabled; when disabled, bypass errors in PSCCH to evaluate PSSCH only.
droponcollision=0 #Drop PSCCH and PSSCH messages on conflicting scheduled resources
enableNsLogs=0 #0 for disabled or 1 enabled; when enabled will output NS LOGs

v_letter="b" #a: no wrap-around topology, b: wrap-around topology
subver="2${v_letter}"

if [ $ctrlerror -eq 0 ];then
  ctrlerrorstr="ctrlerroroff"
else
  ctrlerrorstr="ctrlerroron"
fi

if [ $droponcollision -eq 1 ];then
  pscchdroponcolstr="pscchdroponcolon"
else
  pscchdroponcolstr="pscchdroponcoloff"
fi

ver="v${subver}_broadcast_sim_${stime}s_HD_${Rings}rings_${RESP}RESP_${GRP}GRP_ISD${isd}_MinDist${min_center_dist}_period${SL_PERIOD}_mcs${SL_MCS}_rb${SL_GRANT_RB}_ktrp${SL_KTRP}_pscchrb${PSCCH_RBs}_pscchtrp${PSCCH_TRP}_${ctrlerrorstr}_${pscchdroponcolstr}_run" #Version for logging run output
basedir="d2d"
arguments="--responders=$RESP --groups=$GRP --receivers=$GRPrx --ring=$Rings --isd=$isd --minDist=$min_center_dist --time=$stime --respMaxPkt=$r_max_pck --respPktSize=$r_pck_size --respPktIntvl=$r_pck_int --slPeriod=$SL_PERIOD --mcs=$SL_MCS --ktrp=$SL_KTRP --rbSize=$SL_GRANT_RB --pscchRbs=$PSCCH_RBs --pscchTrp=$PSCCH_TRP --ctrlErrorModel=$ctrlerror --dropOnCol=$droponcollision --enableNsLogs=$enableNsLogs"
linediv="\n-----------------------------------\n"

for ((run=$STARTRUN; run<=$MAXRUNS; run++))
do
  newdir="${basedir}_${ver}${run}"
  if [[ -d $newdir && $OVERWRITE == "0" ]];then
    echo -e "$newdir exist!\nNO OVERWRITE PERMISSION!"
    continue
  fi
  if [ -d $newdir ];then
    rm -rf $newdir
  fi
  mkdir -p $newdir
  OUTFILE="${newdir}/log_${ver}${run}.txt"
  rm -f $OUTFILE
  
  runinfo="$SCENARIO, RUN: ${ver}${run}"
  echo -e "\n$runinfo saved to dir: ${newdir}\n"
  run_args="--RngSeed=$run $arguments"
  echo -e "$runinfo, $run_args $linediv" >> $OUTFILE
  
  ./waf --cwd=$newdir --run "$SCENARIO $run_args" >> $OUTFILE 2>&1 &
done

wait
echo "Evaluating traces..."
pwd
bash pscch-error-stats.sh $ver $STARTRUN $MAXRUNS & # PSCCH collisions and subframe overlaps

}

#for resp in 3 20 33; do

LOG=$0.log
main_log_file="./main_${SCENARIO}.log"
echo "Running simulations..."
echo "Simulation process settings are summarized in $LOG"
echo "Execution status is logged in ${main_log_file}"
echo -e "\n============== `date` ============" >> $LOG
echo -e "\n============== `date` ============" >> $main_log_file
printf "%-7s %-7s %-7s %-4s %-5s %-11s %-11s %-11s %-12s\n" PID S_time Period MCS Ktrp PSCCH_RB_M SUBCH_size Sector_UES PSCCH_BITMAP >> $LOG

for pscch_sf_bitmap in $PSCCH_SF_BITMAPS; do
    for resp in `seq $MIN_UES_PER_SECTOR $MAX_UES_PER_SECTOR`; do
      run_config $SIMULATION_TIME $PERIOD_LENGTH $MCS $PSSCH_SUBCHANNEL_RBS $KTRP $PACKET_PAYLOAD_SIZE $PSCCH_RB_M $pscch_sf_bitmap $resp >> $main_log_file 2>&1 &
      printf "%-7s %-7s %-7s %-4s %-5s %-11s %-11s %-11s %-12s\n" $! $SIMULATION_TIME $PERIOD_LENGTH $MCS $KTRP $PSCCH_RB_M $PSSCH_SUBCHANNEL_RBS $resp $pscch_sf_bitmap >> $LOG
    done
done

