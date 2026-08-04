# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
# %SPDX-License-Identifier: GPL-2.0-only

import os
import sys

sys.path.insert(0, os.path.abspath("extensions"))

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "sphinx.ext.todo",
    "sphinx.ext.coverage",
    "sphinx.ext.imgmath",
    "sphinx.ext.ifconfig",
    "sphinx.ext.autodoc",
]

latex_engine = "xelatex"
todo_include_todos = True
templates_path = ["_templates"]
source_suffix = ".rst"
master_doc = "nr-module"
exclude_patterns = []
add_function_parentheses = True
# add_module_names = True
# modindex_common_prefix = []

project = "NR Module"
copyright = "2024"
author = "OpenSim CTTC/CERCA"

version = "3.3"
release = "3.3"
