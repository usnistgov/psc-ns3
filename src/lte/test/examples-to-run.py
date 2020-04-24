#! /usr/bin/env python3
# -*- coding: utf-8 -*-
## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# A list of C++ examples to run in order to ensure that they remain
# buildable and runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run, do_valgrind_run).
#
# See test.py for more information.
cpp_examples = [
    ("lena-cqi-threshold", "True", "True"),
    ("lena-dual-stripe", "True", "True"),
    ("lena-dual-stripe --simTime=0.0 --nApartmentsX=1 --homeEnbDeploymentRatio=0.5 --nMacroEnbSites=0 --macroUeDensity=0 --nBlocks=1", "True", "True"),
    ("lena-dual-stripe --epc=1 --simTime=0.0 --nApartmentsX=1 --homeEnbDeploymentRatio=0.5 --nMacroEnbSites=0 --macroUeDensity=0 --nBlocks=1", "True", "True"),
    ("lena-dual-stripe --simTime=0.01", "True", "True"),
    ("lena-dual-stripe --epc=1 --simTime=0.01", "True", "True"),
    ("lena-dual-stripe --epc=1 --useUdp=0 --simTime=0.01", "True", "True"),
    ("lena-dual-stripe --epc=1 --fadingTrace=../../src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad --simTime=0.01", "True", "True"),
    ("lena-dual-stripe --nBlocks=1  --nMacroEnbSites=0 --macroUeDensity=0 --homeEnbDeploymentRatio=1 --homeEnbActivationRatio=1 --homeUesHomeEnbRatio=2 --macroEnbTxPowerDbm=0 --simTime=0.01", "True", "True"),
    ("lena-dual-stripe --nMacroEnbSites=0 --macroUeDensity=0 --nBlocks=1 --nApartmentsX=4 --nMacroEnbSitesX=0 --homeEnbDeploymentRatio=1 --homeEnbActivationRatio=1 --macroEnbTxPowerDbm=0 --epcDl=1 --epcUl=0 --epc=1 --numBearersPerUe=4 --homeUesHomeEnbRatio=15 --simTime=0.01", "True", "True"),
    ("lena-fading", "True", "True"),
    ("lena-gtpu-tunnel", "True", "True"),
    ("lena-intercell-interference --simTime=0.1", "True", "True"),
    ("lena-pathloss-traces", "True", "True"),
    ("lena-profiling", "True", "True"),
    ("lena-profiling --simTime=0.1 --nUe=2 --nEnb=5 --nFloors=0", "True", "True"),
    ("lena-profiling --simTime=0.1 --nUe=3 --nEnb=6 --nFloors=1", "True", "True"),
    ("lena-rlc-traces", "True", "True"),
    ("lena-rem", "True", "True"),
    ("lena-rem-sector-antenna", "True", "True"),
    ("lena-simple", "True", "True"),
    ("lena-simple-epc", "True", "True"),
    ("lena-x2-handover", "True", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::TtaFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::TdTbfqFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::TdMtFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::TdBetFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::RrFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::PssFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::PfFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::FdTbfqFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::FdMtFfMacScheduler", "options.valgrind", "True"),
    ("lena-simple-epc --simTime=1.1 --ns3::LteHelper::Scheduler=ns3::FdBetFfMacScheduler", "options.valgrind", "True"),
    ("lena-ipv6-addr-conf", "True", "True"), 
    ("lena-ipv6-ue-rh", "True", "True"), 
    ("lena-ipv6-ue-ue", "True", "True"),
    ("lena-radio-link-failure --numberOfEnbs=1 --simTime=17", "True", "True"),
    ("lena-radio-link-failure --numberOfEnbs=2 --interSiteDistance=700 --simTime=17", "True", "True"),
    ("lena-radio-link-failure --numberOfEnbs=1 --useIdealRrc=0 --simTime=17", "True", "True"),
    ("lena-radio-link-failure --numberOfEnbs=2 --useIdealRrc=0 --interSiteDistance=700 --simTime=17", "True", "True"),
    ("lte-sl-in-covrg-comm-mode1", "True", "True"),
    ("lte-sl-in-covrg-comm-mode2", "True", "True"),
    ("lte-sl-out-of-covrg-comm", "True", "True"),
    ("lte-sl-in-covrg-comm-mode1 --useIPv6=true", "True", "True"),
    ("lte-sl-in-covrg-comm-mode2 --useIPv6=true", "True", "True"),
    ("lte-sl-out-of-covrg-comm --useIPv6=true", "True", "True"),
    ("wns3-2017-discovery", "True", "True"),
    ("wns3-2017-pscch", "True", "True"),
    ("wns3-2017-pssch", "True", "True"),
    ("wns3-2017-synch", "True", "True"),
    ("lte-3gpp-hex-topology --simTime=2 --enableChannelLogs=1 --enableLteTraces=1 --epc=1", "True", "True"),
    ("lte-wrap-around-hex-topology --simTime=2 --enableChannelLogs=1 --enableLteTraces=1 enableTopologyLogs=1", "True", "True"),
    ("lte-sl-in-covrg-discovery", "True", "True"),
    ("lte-sl-in-covrg-discovery-collision", "True", "True"),
    ("lte-sl-out-of-covrg-discovery-collision", "True", "True"),
    ("lte-sl-out-of-covrg-synch --simTime=10 --enableNsLogs=0 --useIPv6=0", "True", "True"),
    ("lte-sl-out-of-covrg-synch --simTime=10 --enableNsLogs=0 --useIPv6=1", "True", "True"),
    ("lte-sl-out-of-covrg-synch-twotx --simTime=10 --enableNsLogs=0 --useIPv6=0", "True", "True"),
    ("lte-sl-out-of-covrg-synch-twotx --simTime=10 --enableNsLogs=0 --useIPv6=1", "True", "True"),
    ("lte-sl-relay-cluster", "True", "True"),
    ("lte-sl-relay-cluster-selection", "True", "True"),
    ("lte-sl-in-covrg-relay-building", "True", "True"),
    ("lte-sl-in-covrg-comm-mode1-multicell --test=true", "True", "True"),
    ("lte-sl-in-covrg-comm-mode2-multicell --test=true", "True", "True"),
    ("lte-sl-in-covrg-comm-mode1-multicell --useIPv6=true --test=true", "True", "True"),
    ("lte-sl-in-covrg-comm-mode2-multicell --useIPv6=true --test=true", "True", "True"),
    ("lte-sl-in-covrg-discovery-multicell --test=true", "True", "True"),
    ("lte-sl-in-covrg-relay-mode2-multicell --test=true", "True", "True"),
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
