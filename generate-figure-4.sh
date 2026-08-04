#!/bin/bash
# All defaults, except --warmup=30 required
# --quiet is not an option with --skipRun here
python3 src/nr-prose/examples/plot-mcs-time-series.py --warmup=30 --rngRuns=20 --simTime=180 --d=400 --mcpttPairs=4 --bgPairs=8 --layout=ring --lossModel=umi --errorModel=epa --maxNumTx=1
