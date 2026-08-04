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
    ("nr-prose-discovery", "True", "True"),
    ("nr-prose-discovery-l3-relay", "True", "True"),
    ("nr-prose-discovery-l3-relay-selection", "True", "True"),
    ("nr-prose-l3-relay", "True", "True"),
    ("nr-prose-l3-relay-on-off", "True", "True"),
    ("nr-prose-network-coex", "True", "True"),
    ("nr-prose-unicast-multi-link", "True", "True"),
    ("nr-prose-unicast-single-link", "True", "True"),
    ("nr-prose-unicast-l3-relay", "True", "True"),
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
