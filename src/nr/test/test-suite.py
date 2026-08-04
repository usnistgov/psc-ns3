#! /usr/bin/env python3

# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

"""!
Differently from examples-to-run, this test suit tests things negatively
# a.k.a. we want to make sure known bad settings are caught by asserts
"""

import glob
import os
import re
import shutil
import subprocess
import sys
import unittest
from functools import partial

# Get path containing ns3
ns3_path = os.path.dirname(os.path.abspath(os.sep.join([__file__, "../../../"])))
ns3_lock_filename = os.path.join(ns3_path, ".lock-ns3_%s_build" % sys.platform)
ns3_script = os.sep.join([ns3_path, "ns3"])
ns3rc_script = os.sep.join([ns3_path, ".ns3rc"])
usual_outdir = os.sep.join([ns3_path, "build"])
usual_lib_outdir = os.sep.join([usual_outdir, "lib"])

# Move the current working directory to the ns-3-dev folder
os.chdir(ns3_path)

# Cmake commands
num_threads = max(1, os.cpu_count() - 1)
win32 = sys.platform == "win32"
platform_makefiles = "MinGW Makefiles" if win32 else "Unix Makefiles"
ninja = shutil.which("ninja")
if ninja:
    platform_makefiles = "Ninja"
ext = ".exe" if win32 else ""


def run_ns3(args, env=None, generator=platform_makefiles):
    """!
    Runs the ns3 wrapper script with arguments
    @param args: string containing arguments that will get split before calling ns3
    @param env: environment variables dictionary
    @param generator: CMake generator
    @return tuple containing (error code, stdout and stderr)
    """
    if " -G " in args:
        args = args.format(generator=generator)
    if env is None:
        env = {}
    # Disable colored output by default during tests
    env["CLICOLOR"] = "0"
    return run_program(ns3_script, args, python=True, env=env)


# Adapted from https://github.com/metabrainz/picard/blob/master/picard/util/__init__.py
def run_program(program, args, python=False, cwd=ns3_path, env=None):
    """!
    Runs a program with the given arguments and returns a tuple containing (error code, stdout and stderr)
    @param program: program to execute (or python script)
    @param args: string containing arguments that will get split before calling the program
    @param python: flag indicating whether the program is a python script
    @param cwd: the working directory used that will be the root folder for the execution
    @param env: environment variables dictionary
    @return tuple containing (error code, stdout and stderr)
    """
    if type(args) != str:
        raise Exception("args should be a string")

    # Include python interpreter if running a python script
    if python:
        arguments = [sys.executable, program]
    else:
        arguments = [program]

    if args != "":
        arguments.extend(re.findall('(?:".*?"|\S)+', args))  # noqa

    for i in range(len(arguments)):
        arguments[i] = arguments[i].replace('"', "")

    # Forward environment variables used by the ns3 script
    current_env = os.environ.copy()

    # Add different environment variables
    if env:
        current_env.update(env)

    # Call program with arguments
    ret = subprocess.run(
        arguments,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=cwd,  # run process from the ns-3-dev path
        env=current_env,
    )
    # Return (error code, stdout and stderr)
    return (
        ret.returncode,
        ret.stdout.decode(sys.stdout.encoding),
        ret.stderr.decode(sys.stderr.encoding),
    )


def get_programs_list():
    """!
    Extracts the programs list from .lock-ns3
    @return list of programs.
    """
    values = {}
    with open(ns3_lock_filename, encoding="utf-8") as f:
        exec(f.read(), globals(), values)

    programs_list = values["ns3_runnable_programs"]

    # Add .exe suffix to programs if on Windows
    if win32:
        programs_list = list(map(lambda x: x + ext, programs_list))
    return programs_list


def get_libraries_list(lib_outdir=usual_lib_outdir):
    """!
    Gets a list of built libraries
    @param lib_outdir: path containing libraries
    @return list of built libraries.
    """
    libraries = glob.glob(lib_outdir + "/*", recursive=True)
    return list(filter(lambda x: "scratch-nested-subdir-lib" not in x, libraries))


def get_headers_list(outdir=usual_outdir):
    """!
    Gets a list of header files
    @param outdir: path containing headers
    @return list of headers.
    """
    return glob.glob(outdir + "/**/*.h", recursive=True)


def read_lock_entry(entry):
    """!
    Read interesting entries from the .lock-ns3 file
    @param entry: entry to read from .lock-ns3
    @return value of the requested entry.
    """
    values = {}
    with open(ns3_lock_filename, encoding="utf-8") as f:
        exec(f.read(), globals(), values)
    return values.get(entry, None)


def get_test_enabled():
    """!
    Check if tests are enabled in the .lock-ns3
    @return bool.
    """
    return read_lock_entry("ENABLE_TESTS")


def get_enabled_modules():
    """
    Check if tests are enabled in the .lock-ns3
    @return list of enabled modules (prefixed with 'ns3-').
    """
    return read_lock_entry("NS3_ENABLED_MODULES")


class NrBaseTestCase(unittest.TestCase):
    """!
    Generic test case with basic function inherited by more complex tests.
    """

    ONCE = False

    def config_ok(self, return_code, stdout, stderr):
        """!
        Check if configuration for release mode worked normally
        @param return_code: return code from CMake
        @param stdout: output from CMake.
        @param stderr: error from CMake.
        @return None
        """
        self.assertEqual(return_code, 0)
        self.assertIn("Build profile                 : release", stdout)
        self.assertIn("Build files have been written to", stdout)
        self.assertNotIn("uninitialized variable", stderr)

    def setUp(self):
        """!
        Clean configuration/build artifacts before testing configuration and build settings
        After configuring the build as release,
        check if configuration worked and check expected output files.
        @return None
        """
        super().setUp()

        if not NrBaseTestCase.ONCE:
            # Reconfigure from scratch before testing
            run_ns3("clean")
            return_code, stdout, stderr = run_ns3(
                'configure -G "{generator}" -d release --enable-examples --enable-asserts --filter-module-examples-and-tests=nr'
            )
            self.config_ok(return_code, stdout, stderr)

            return_code, stdout, stderr = run_ns3("build")
            self.assertEqual(return_code, 0)

            # Check if .lock-ns3 exists, then read to get list of executables.
            self.assertTrue(os.path.exists(ns3_lock_filename))
            ## ns3_executables holds a list of executables in .lock-ns3 # noqa
            self.ns3_executables = get_programs_list()

            # Check if .lock-ns3 exists than read to get the list of enabled modules.
            self.assertTrue(os.path.exists(ns3_lock_filename))
            ## ns3_modules holds a list to the modules enabled stored in .lock-ns3 # noqa
            self.ns3_modules = get_enabled_modules()
            NrBaseTestCase.ONCE = True


def run_tests(testcase, listOfTests):
    for example, stderrContains in listOfTests:
        return_code, stdout, stderr = run_ns3(f'run "{example}"')
        with testcase.subTest(example=example, stderrContains=stderrContains):
            if return_code == 0:
                testcase.assertTrue(True)
            else:
                testcase.assertIn(stderrContains, stderr)


class NrExamplesAssertInvalidSettings(NrBaseTestCase):
    def setUp(self):
        super().setUp()

    def test_cttc_nr_mimo_demo(self):
        listOfTests = [
            # User is unaware gNb already has two horizontal ports
            # and default NrCbTwoPort supports only two ports
            (
                "cttc-nr-mimo-demo --numVPortsGnb=4 --numRowsGnb=8",
                "This codebook supports at most 2 ports",
            ),
            (
                "cttc-nr-mimo-demo --numHPortsGnb=4 --numColumnsGnb=8",
                "This codebook supports at most 2 ports",
            ),
            (
                "cttc-nr-mimo-demo --numHPortsGnb=4 --numRowsGnb=8",
                "This codebook supports at most 2 ports",
            ),
            (
                "cttc-nr-mimo-demo --numVPortsGnb=2 --numRowsGnb=2",
                "This codebook supports at most 2 ports",
            ),
            # Confused user switches rows and columns
            (
                "cttc-nr-mimo-demo --numVPortsGnb=4 --numColumnsGnb=8",
                "The number of vertical ports of gNB must divide number of element rows",
            ),
            # Confused user tries to create an antenna panel with more antenna ports than elements
            (
                "cttc-nr-mimo-demo --numVPortsGnb=4 --numRowsGnb=2",
                "The number of vertical ports of gNB must divide number of element rows",
            ),
            (
                "cttc-nr-mimo-demo --numHPortsGnb=4 --numColumnsGnb=2",
                "The number of horizontal ports of gNB must divide number of element columns",
            ),
            (
                "cttc-nr-mimo-demo --numVPortsGnb=4 --numRowsGnb=3",
                "The number of vertical ports of gNB must divide number of element rows",
            ),
            (
                "cttc-nr-mimo-demo --numHPortsGnb=4 --numColumnsGnb=3",
                "The number of horizontal ports of gNB must divide number of element columns",
            ),
            # Confused user tries to create an antenna panel with a number of elements
            # that are not divisible by the number of ports
            (
                "cttc-nr-mimo-demo --numVPortsGnb=2 --numRowsGnb=3",
                "The number of vertical ports of gNB must divide number of element rows",
            ),
            (
                "cttc-nr-mimo-demo --numHPortsGnb=2 --numColumnsGnb=3",
                "The number of horizontal ports of gNB must divide number of element columns",
            ),
            # User is unaware the bandwidth of the channel and subband size must be changed together
            (
                "cttc-nr-mimo-demo --bandwidth=2.5e6",
                "Bandwidth parts with less than 24 PRBs should have subbands of size 1",
            ),
            (
                "cttc-nr-mimo-demo --bandwidth=5e6",
                "Bandwidth parts with 24<=x<=72 PRBs should have subbands of size 4 or 8",
            ),
            (
                "cttc-nr-mimo-demo --subbandSize=1",
                "Bandwidth parts with 73<=x<=144 PRBs should have subbands of size 8 or 16",
            ),
            (
                "cttc-nr-mimo-demo --subbandSize=32",
                "Bandwidth parts with 73<=x<=144 PRBs should have subbands of size 8 or 16",
            ),
            # Unaware user tries to configure more than two ports without cross-polarized antennas
            (
                "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --numVPortsGnb=2 --numRowsGnb=2 --numHPortsGnb=2 --numColumnsGnb=2 --numVPortsUe=2 --numRowsUe=2 --numHPortsUe=2 --numColumnsUe=2",
                "For > 2 antenna ports, dual polarization is required",
            ),
            # Unaware user tries to configure more than 32 ports
            (
                "cttc-nr-mimo-demo --fullSearchCb=ns3::NrCbTypeOneSp --pmSearchMethod=ns3::NrPmSearchFull --numVPortsGnb=3 --numRowsGnb=3 --numHPortsGnb=8 --numColumnsGnb=8 --xPolGnb=1 --numVPortsUe=3 --numRowsUe=3 --numHPortsUe=8 --numColumnsUe=8 --xPolUe=1",
                "Number of CSI-RS ports must not be greater than 32",
            ),
        ]
        run_tests(self, listOfTests)


class NrExamplesAssertValidSettings(NrBaseTestCase):
    def setUp(self):
        super().setUp()

    def test_cttc_nr_mimo_demo(self):
        listOfTests = [
            ("cttc-nr-mimo-demo --numHPortsGnb=2 --numColumnsGnb=2", ""),
            ("cttc-nr-mimo-demo --bandwidth=2.5e6 --subbandSize=1", ""),
        ]
        run_tests(self, listOfTests)


def main():
    """!
    Main function
    @return None
    """

    test_completeness = {
        "fast": [NrExamplesAssertInvalidSettings],
        "full": [NrExamplesAssertInvalidSettings, NrExamplesAssertValidSettings],
    }

    loader = unittest.TestLoader()
    suite = unittest.TestSuite()

    import argparse

    parser = argparse.ArgumentParser("Test suite for the nr example configurations")
    parser.add_argument("-c", "--completeness", choices=test_completeness.keys(), default="fast")
    args = parser.parse_args(sys.argv[1:])

    # Put tests cases in order
    for testCase in test_completeness[args.completeness]:
        suite.addTests(loader.loadTestsFromTestCase(testCase))

    # Run tests and fail as fast as possible
    runner = unittest.TextTestRunner(failfast=False, verbosity=1)
    runner.run(suite)


if __name__ == "__main__":
    main()
