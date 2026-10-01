#!/usr/bin/python3
import subprocess
import sys

tests = [
    "./ns3 build",
    "./ns3 run --no-build nr-prose-olsrv2",
    "./ns3 run --no-build multihop-random-waypoint",
    "./ns3 run --no-build multihop-grid",
    "./ns3 run --no-build nr-prose-multihop-linear",
    "./ns3 run --no-build mcptt-nr-operational-modes",
    "./ns3 run --no-build nr-sl-example -- --enableLogging=0 --writeTraces=0",
    "./ns3 run --no-build nr-v2x-west-to-east-highway -- --enableSensing=0 --simTag=testpy-nosensing --saveDb=0",
    "./ns3 run --no-build nr-v2x-west-to-east-highway -- --enableSensing=1 --simTag=testpy-sensing --saveDb=0 ",
    "./ns3 run --no-build cttc-nr-v2x-demo-simple -- --simTag=testpy --saveDb=0 ",
    "./ns3 run --no-build nr-sl-multi-lc-example -- --testing=1 --writeTraces=0 ",
    "./ns3 run --no-build nr-sl-harq-example -- --writeTraces=0 ",
    "./ns3 run --no-build nr-sl-harq-example -- --writeTraces=0 ",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-scheduler",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-rlc-um",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-ideal-mcs-controller",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-harq",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-error-model",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-sensing ",
    "./ns3 run --no-build test-runner -- --suite=nr-sl-sci-headers",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-dyn-gcast-blind ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-dyn-gcast-no-harq ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-dyn-gcast-harq ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-prio-sps ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-prio-dyn ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-rri ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-prio-bcast ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-prio-uni ",
    "./ns3 run --no-build test-runner -- --suite=sl-multi-lc-prio-gcast ",
    "./ns3 run --no-build test-runner -- --suite=olsrv2-link-failure ",
    "./ns3 run --no-build nr-prose-discovery-l3-relay-selection -- --writeTraces=0 ",
    "./ns3 run --no-build nr-sl-error-model-example"
]

commands = [s.split() for s in tests]

for cmd in commands:

    errors = False

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout_results, stderr_results = proc.communicate()
    retval = proc.returncode

    print(f"Checking: {' '.join(cmd)}")

    if proc.returncode != 0:
        print(f"Error: Command failed with exit code {proc.returncode}\n")
        errors = True

if errors:
    print("\n*** At least one program exited with error.***");

