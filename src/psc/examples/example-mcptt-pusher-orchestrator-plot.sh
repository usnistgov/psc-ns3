#!/bin/sh
nonzero=false
gnuplot="set key autotitle columnhead; plot"
for pusher in $(find . -maxdepth 1 -type f -name "pusher-state*.csv")
do
  gnuplot+=" '${pusher}' using (\$1):3:ytic(2) w step,"
  nonzero=true
done

if $nonzero
then
  gnuplot+=" 'session-state.csv' using (\$1):3:ytic(2) w step, 'contention-state.csv' using (\$1):3:ytic(2)"
  gnuplot -p -e "${gnuplot}"
  echo $gnuplot
fi
