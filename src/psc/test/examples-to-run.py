#! /usr/bin/env python

# Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

# -*- coding: utf-8 -*-
## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# A list of C++ examples to run in order to ensure that they remain
# buildable and runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run, do_valgrind_run).
#
# See test.py for more information.
cpp_examples = [
    ("example-udp-group-echo", "True", "True"),
    ("uav-mobility-energy-example", "True", "True"),
    ("uav-mobility-energy-helper-example", "True", "True"),
    ("example-intel-http", "True", "True"),
    ("example-video-streaming", "True", "True"),
    ("example-mcptt-wifi-adhoc", "True", "True"),
    ("example-mcptt-pusher-orchestrator", "True", "True"),
    ("example-mcptt-on-network-floor-control-csma", "True", "True"),
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
