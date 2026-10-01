# Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
html_theme = "ns3_html_theme"
html_theme_path = ["../../../../doc/"]
html_title = "Tutorial"
# html_static_path = ['_static']
html_last_updated_fmt = "%b %d, %Y %H:%M"
templates_path = ["_templates"]
source_suffix = ".rst"
master_doc = "tutorial"
exclude_patterns = []
add_function_parentheses = True
# add_module_names = True
# modindex_common_prefix = []

# project = u'cttc-nr-demo tutorial'
copyright = "2024"
author = ""

version = "3.3"
release = "3.3"

# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',
    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',
    # Additional stuff for the LaTeX preamble.
    #
    # VerbatimBorderColor:  make the box around code samples blend into the background
    # Tip from https://stackoverflow.com/questions/29403100/how-to-remove-the-box-around-the-code-block-in-restructuredtext-with-sphinx
    #
    # sphinxcode is the wrapper around \texttt that sphinx.sty provides.
    # Redefine it here as needed to change the inline literal font size
    # (double backquotes) to either \footnotesize (8pt) or \small (9pt)
    #
    # See above to change the font size of verbatim code blocks
    #
    # 'preamble': '',
    "preamble": """\\usepackage{amssymb}
 \\definecolor{VerbatimBorderColor}{rgb}{1,1,1}
 \\renewcommand{\\sphinxcode}[1]{\\texttt{\\small{#1}}}
"""
    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    ("tutorial", "cttc-nr-demo-tutorial.tex", "cttc-nr-demo tutorial", "", "manual"),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
#
# latex_logo = '../../ns3_html_theme/static/ns-3.png'

# If true, show page references after internal links.
#
# latex_show_pagerefs = False

# If true, show URL addresses after external links.
#
# latex_show_urls = False

# Documents to append as an appendix to all manuals.
#
# latex_appendices = []

# If false, will not define \strong, \code, \titleref, \crossref ... but only
# \sphinxstrong, ..., \sphinxtitleref, ... to help avoid clash with user added
# packages.
#
# latex_keep_old_macro_names = True

# If false, no module index is generated.
#
# latex_domain_indices = True

# Enable Figure references by number (:numref:)
numfig = True
