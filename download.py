#! /usr/bin/env python3
import sys
import os
import subprocess

ns3_path = os.path.dirname(os.path.realpath(os.path.abspath(__file__)))
contribdir = os.sep.join([ns3_path, "contrib"])

# Arguments are:  modulename, URL, branch name, tag name
# tag name may be empty 
modules = [
  ("nr", "git@github.com:usnistgov/pscr-net-sim-nr-module.git", "sidelink"),
  ("nr-prose", "git@github.com:usnistgov/pscr-net-sim-nr-prose-module.git", "sidelink"),
  ("psc", "git@github.com:usnistgov/pscr-net-sim-psc-module.git", "sidelink"),
  ("sip", "https://gitlab.com/tomhend/modules/sip.git", "master"),
  ("netsimulyzer", "git@github.com:usnistgov/netsimulyzer-ns3-module.git", "master", "v1.0.12"),
]

def get_module(modulename, url, branch, tag=""):

    def clone(moduledir):
        if tag != "":
            print("Retrieving " + modulename + " branch " + branch + " tag " + tag + " from " + url)
            clone_output = subprocess.check_output(["git", "clone", "-b", branch, url, modulename])
            os.chdir(moduledir)
            clone_output = subprocess.check_output(["git", "checkout", "-b", tag + "-branch", tag])
        else:
            print("Retrieving " + modulename + " branch " + branch + " from " + url)
            clone_output = subprocess.check_output(["git", "clone", "-b", branch, url, modulename])

    def update():
        print("Pulling " + modulename + " updates from " + url)
        update_output = subprocess.check_output(["git", "-C", modulename, "pull"])

    os.chdir(contribdir)
    moduledir = os.sep.join([contribdir, modulename])
    if not os.path.exists(modulename):
        clone(moduledir)
    else:
        update()

def main():

    os.chdir(ns3_path)

    for item in modules:
        if len(item) == 4:
            get_module(item[0], item[1], item[2], item[3])
        else:
            get_module(item[0], item[1], item[2])

    return 0

if __name__ == '__main__':
    sys.exit(main())
