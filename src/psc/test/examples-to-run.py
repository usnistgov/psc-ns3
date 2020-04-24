#! /usr/bin/env python
# -*- coding: utf-8 -*-
## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# A list of C++ examples to run in order to ensure that they remain
# buildable and runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run, do_valgrind_run).
#
# See test.py for more information.
cpp_examples = [
    ("example-udp-group-echo --nExtra=0 --verbose=1 --time=10", "True", "True"),
    ("example-mcptt-wifi-adhoc --users=10 --groups=2 --data-rate=48kb/s --max-x=6 --max-y=6 --media-size=70 --on-off-mean=4 --start-time=3 --stop-time=15", "True", "True"),
    ("camad-2019-communication", "True", "True"),
    ("camad-2019-connection", "True", "True"),
    ("camad-2019-discovery", "True", "True"),
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
