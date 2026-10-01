#! /usr/bin/env python

# Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

# -*- coding: utf-8 -*-

# A list of C++ examples to run in order to ensure that they remain
# buildable and runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run, do_valgrind_run).
#
# See test.py for more information.
cpp_examples = [
    (
        "lena-lte-comparison-user --simTag=test1-user --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=1 --calibration=false --freqScenario=0 --operationMode=FDD --direction=DL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-user --simTag=test2-user --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=1 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test1 --trafficScenario=2 --simulator=LENA --technology=LTE --numRings=0 --ueNumPergNb=1 --calibration=false --freqScenario=0 --operationMode=FDD --direction=DL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test2 --trafficScenario=2 --simulator=LENA --technology=LTE --numRings=0 --ueNumPergNb=1 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test3 --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=1 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test4 --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=2 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test5 --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=2 --calibration=false --freqScenario=0 --operationMode=FDD --direction=DL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test6 --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=4 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test7 --trafficScenario=2 --simulator=5GLENA --technology=LTE --numRings=0 --ueNumPergNb=4 --calibration=false --freqScenario=0 --operationMode=FDD --direction=DL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test8 --trafficScenario=2 --simulator=5GLENA --technology=NR --numRings=0 --ueNumPergNb=4 --calibration=false --freqScenario=0 --operationMode=TDD --direction=UL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test9 --trafficScenario=1 --simulator=5GLENA --technology=NR --numRings=0 --ueNumPergNb=4 --calibration=false --freqScenario=0 --operationMode=TDD --direction=DL",
        "True",
        "True",
    ),
    (
        "lena-lte-comparison-campaign --simTag=test10 --trafficScenario=1 --simulator=5GLENA --technology=NR --numRings=0 --ueNumPergNb=4 --calibration=false --freqScenario=0 --operationMode=FDD --direction=UL",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=1 --enableDl=1 --notchedRbStartDl=5 --numOfNotchedRbsDl=5 --notchedRbStartUl=15 --numOfNotchedRbsUl=3 --bandwidth=5e6",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=1 --enableDl=1 --notchedRbStartDl=5 --numOfNotchedRbsDl=7 --notchedRbStartUl=15 --numOfNotchedRbsUl=5",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=1 --enableDl=1 --validationValue1=10.18 --validationValue2=10.08",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=0 --enableDl=1 --validationValue1=10.17 --validationValue2=10.17",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=false --enableUl=0 --enableDl=1 --validationValue1=10.18 --validationValue2=10.18",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=1 --operationMode=FDD --enableOfdma=false --enableUl=0 --enableDl=1 --validationValue1=10.18",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=1 --operationMode=TDD --enableOfdma=false --enableUl=0 --enableDl=1 --validationValue1=10.18",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=1 --operationMode=TDD --enableOfdma=false --enableUl=1 --enableDl=0 --validationValue1=10.12",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=1 --enableDl=1 --notchedRbStartDl=5 --numOfNotchedRbsDl=7 --notchedRbStartUl=15 --numOfNotchedRbsUl=5 --validationValue1=10.15 --validationValue2=10.18",
        "True",
        "True",
    ),
    (
        "cttc-nr-notching --gNbNum=1 --ueNumPergNb=2 --operationMode=FDD --enableOfdma=true --enableUl=1 --enableDl=1 --notchedRbStartDl=5 --numOfNotchedRbsDl=5 --notchedRbStartUl=15 --numOfNotchedRbsUl=3 --bandwidth=5e6 --validationValue1=8.27 --validationValue2=9.028",
        "True",
        "True",
    ),
    ("cttc-3gpp-channel-example", "True", "True"),
    ("cttc-3gpp-channel-nums", "True", "True"),
    ("cttc-3gpp-channel-nums-fdm", "True", "True"),
    ("cttc-3gpp-channel-simple-fdm", "True", "True"),
    ("cttc-3gpp-channel-simple-ran", "True", "True"),
    ("cttc-3gpp-indoor-calibration", "True", "True"),
    ("cttc-nr-demo", "True", "True"),
    ("cttc-lte-ca-demo --simTime=1", "True", "True"),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|UL|DL|DL|S|UL|UL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|DL|DL|DL|S|UL|DL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|UL|UL|DL|DL|DL|DL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|UL|DL|DL|DL|DL|DL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|DL|DL|DL|DL|DL|DL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|UL|UL|DL|S|UL|UL|DL|"',
        "True",
        "True",
    ),
    (
        'cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="DL|S|UL|UL|UL|DL|S|UL|UL|UL|"',
        "True",
        "True",
    ),
    ('cttc-nr-cc-bwp-demo --simTime=0.8 --tddPattern="F|F|F|F|F|F|F|F|F|F|"', "True", "True"),
    ("cttc-realistic-beamforming", "True", "True"),
    ("cttc-fh-compression --simTimeMs=800", "True", "True"),
    ("cttc-error-model --simTime=2", "True", "True"),
    ("cttc-error-model-amc --simTime=2", "True", "True"),
    ("cttc-error-model-comparison", "True", "True"),
    ("cttc-channel-randomness", "True", "True"),
    ("rem-beam-example", "True", "True"),
    ("rem-example", "True", "True"),
    ("cttc-nr-demo --ueNumPergNb=9", "True", "True"),
    (
        "cttc-nr-demo --ns3::NrMacSchedulerNs3::EnableHarqReTx=false",
        "True",
        "True",
    ),
    ("cttc-nr-traffic-ngmn-mixed", "True", "True"),
    ("cttc-nr-traffic-3gpp-xr", "True", "True"),
    ("traffic-generator-example", "True", "True"),
    (
        "cttc-nr-3gpp-calibration-user --simTag=NrCali1 --technology=NR --nrConfigurationScenario=DenseA --operationMode=TDD --numRings=0 --crossPolarizedGnb=false --polSlantAngleGnb1=45 --polSlantAngleUe1=0.0 --ueBearingAngle=0 --appGenerationTime=0.5 --enableFading=true --enableShadowing=true --bfMethod=Omni --attachToClosest=1 --freqScenario=1 --trafficScenario=0",
        "True",
        "True",
    ),
    (
        "cttc-nr-3gpp-calibration-user --simTag=NrCali2 --technology=NR --nrConfigurationScenario=DenseA --operationMode=FDD --numRings=0 --crossPolarizedGnb=false --polSlantAngleGnb1=45 --polSlantAngleUe1=0.0 --ueBearingAngle=0 --appGenerationTime=0.5 --enableFading=true --enableShadowing=true --bfMethod=CellScan --attachToClosest=1 --freqScenario=1 --trafficScenario=1",
        "True",
        "True",
    ),
    (
        "cttc-nr-3gpp-calibration-user --simTag=NrCali3 --technology=NR --nrConfigurationScenario=RuralA --operationMode=FDD --numRings=0 --crossPolarizedGnb=true --appGenerationTime=0.1 --enableFading=false --enableShadowing=false --bfMethod=CellScan --freqScenario=0 --trafficScenario=2",
        "True",
        "True",
    ),
    (
        "cttc-nr-3gpp-calibration-user --simTag=NrCali4 --technology=LTE --errorModelType=ns3::LenaErrorModel --nrConfigurationScenario=DenseA --numRings=0 --operationMode=TDD --crossPolarizedGnb=false --polSlantAngleGnb1=45 --polSlantAngleUe1=0.0 --ueBearingAngle=0 --appGenerationTime=0.5 --enableFading=true --enableShadowing=true --bfMethod=CellScan --attachToClosest=1 --freqScenario=1 --trafficScenario=3",
        "True",
        "True",
    ),
    (
        "cttc-nr-3gpp-calibration-user --simTag=NrCali5 --simulator=LENA --technology=LTE --errorModelType=LteMiErrorModel --nrConfigurationScenario=RuralA --numRings=0 --operationMode=FDD --crossPolarizedGnb=true --appGenerationTime=0.1 --enableFading=false --enableShadowing=false --bfMethod=CellScan --freqScenario=0 --trafficScenario=4",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --bandwidth=2.5e6 --subbandSize=1",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --bandwidth=2.5e6 --subbandSize=1 --beamformingMethod=ns3::KroneckerBeamforming",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --bandwidth=2.5e6 --subbandSize=1 --beamformingMethod=ns3::KroneckerQuasiOmniBeamforming",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --bandwidth=2.5e6 --subbandSize=1 --downsamplingTechnique=FirstPRB",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --bandwidth=5e6 --subbandSize=4 --downsamplingTechnique=FirstPRB",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --bandwidth=10e6 --subbandSize=8 --downsamplingTechnique=FirstPRB",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --bandwidth=10e6 --useConfigSetDefault=false",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --bandwidth=20e6 --subbandSize=16 --downsamplingTechnique=AveragePRB",
        "True",
        "True",
    ),
    (
        "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --bandwidth=40e6 --subbandSize=32 --downsamplingTechnique=AveragePRB",
        "True",
        "True",
    ),
    (
        "cttc-nr-fh-xr --fhCapacity=5000 --fhControlMethod=OptimizeRBs --frequency=30e9 --bandwidth=400e6 --numerology=3 --deployment=SIMPLE --arUeNum=3 --vrUeNum=3 --cgUeNum=3 --voiceUeNum=3 --appDuration=3000 --enableTDD4_1=1 --enableMimoFeedback=1 --txPower=30 --distance=2 --channelUpdatePeriod=0 --channelConditionUpdatePeriod=0 --enableShadowing=0 --isLos=1 --enableHarqRetx=1 --useFixedMcs=0 --enableInterServ=0 --enablePdcpDiscarding=1 --schedulerType=PF --reorderingTimerMs=10",
        "True",
        "True",
    ),
    (
        "cttc-nr-fh-xr --fhCapacity=2000 --fhControlMethod=OptimizeMcs --frequency=30e9 --bandwidth=400e6 --numerology=3 --deployment=SIMPLE --arUeNum=3 --vrUeNum=3 --cgUeNum=3 --voiceUeNum=3 --appDuration=3000 --enableTDD4_1=1 --enableMimoFeedback=1 --txPower=30 --distance=2 --channelUpdatePeriod=0 --channelConditionUpdatePeriod=0 --enableShadowing=0 --isLos=1 --enableHarqRetx=1 --useFixedMcs=0 --enableInterServ=0 --enablePdcpDiscarding=1 --schedulerType=PF --reorderingTimerMs=10",
        "True",
        "True",
    ),
    ("gsoc-nr-rl-based-sched", "True", "True"),
    ("gsoc-nr-channel-models", "True", "True"),
    ("gsoc-nr-channel-models --channelModel=Friis", "True", "True"),
    ("nr-v2x-west-to-east-highway --enableSensing=1 --simTag=testpy-sensing --saveDb=0", "True", "True"),
    ("nr-v2x-west-to-east-highway --enableSensing=0 --simTag=testpy-nosensing --saveDb=0", "True", "True"),
    ("cttc-nr-v2x-demo-simple --simTag=testpy --saveDb=0", "True", "True"),
    ("nr-sl-harq-example --writeTraces=0", "True", "True"),
    ("nr-sl-example", "True", "True"),
    ("nr-sl-error-model-example", "True", "True"),
    # nr-sl-multi-lc-example.cc covered as an ExampleAsTestSuite program
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
