#! /bin/bash

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

scenario="wns3-2017-pssch"
RB_START=2
RB_END=50
RB_STEP=2
Time=10 #simulation length

function run_config ()
{    
    config=$1
    period=$2
    pscch=$3
    mcs=$4
    ktrp=$5
  
    Log="DataRate_$config.txt"
    TraceFile="SlRxPhyStats_$config.txt"

    if [ -e $Log ] || [ -e $TraceFile ];then
    echo "ERROR: $Log or $TraceFile already exists. Appending the old $Log or reading old $TraceFile is not allowed!" 
    exit
    fi 
  
    printf "SlPeriod(ms)\tPscchSubframes\tMCS\tKTRP\tPsschRbs\tDataRate(bits/sec)\n" >> $Log

    for ((rb=$RB_START; rb<=$RB_END;rb=$((rb+RB_STEP))))
    do
    #echo "Running RB=$rb"
    ./waf --run "$scenario --period=${period} --pscchLength=${pscch} --mcs=${mcs} --ktrp=${ktrp} --rbSize=${rb} --simTime=${Time} --SlRxOutputFilename=${TraceFile}" >> /dev/null 2>&1
    datarate=`cat ${TraceFile} | awk 'BEGIN{bytes=0;start=0;stop=0}{if ($9==1 && $10==1) { if (bytes==0) start=$1; bytes+=$7; stop=$1}}END{print 8*bytes/(stop-start)}'` 
        printf "$period\t$pscch\t$mcs\t$ktrp\t$rb\t$datarate\n" >> $Log   
    done
}

echo "Config 1"
run_config "Config1" "sf40" 8 10 2
echo "Config 2"
run_config "Config2" "sf80" 8 12 4
echo "Config 3"
run_config "Config3" "sf320" 40 15 8
