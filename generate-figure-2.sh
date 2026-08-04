#!/bin/bash
python3 src/nr-prose/examples/plot-mcs-vs-distance.py --warmup=90 --rngRuns=5 --eventCount=14000 --simTime=700 --dMin=20 --dMax=700 --dStep=20 --controllers static ideal ts olla --layout=ring --lossModel=umi --errorModel=epa --mcpttPairs=1 --bgPairs=0
