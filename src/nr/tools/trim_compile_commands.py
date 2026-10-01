#! /usr/bin/env python3

# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

import argparse
import json
import os

args_parser = argparse.ArgumentParser()
args_parser.add_argument("json_path", help="compile_commands.json path", type=str)
args = args_parser.parse_args()

with open(args.json_path, "r") as f:
    compiledb = json.load(f)

filtered_list = []
for entry in compiledb:
    if "contrib/nr" in entry["file"]:
        filtered_list.append(entry)

with open(args.json_path, "w") as f:
    json.dump(filtered_list, f, indent=2)
