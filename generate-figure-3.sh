#!/bin/bash
python3 src/nr-prose/examples/plot-mcs-vs-distance-joint.py --warmup=90 --rngRuns=10 --eventCount=14000 --simTime=700 --dMin=20 --dMax=1000 --dStep=20 --mcsController=olla --configs numtx1 first-tx harq-aware dynamic --layout=ring --lossModel=umi --errorModel=epa --mcpttPairs=1 --bgPairs=0 
