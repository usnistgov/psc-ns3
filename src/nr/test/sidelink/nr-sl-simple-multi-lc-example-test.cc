// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

#include <ns3/example-as-test.h>

//
// Each statement below runs a specific variation of the program
// nr-sl-multi-lc-example.cc and then checks the output produced against
// the known good output in the '.reflog' files in the directory 'sl-test-data'
//
// The first test is dynamic grants, groupcast, HARQ enabled.  The output
// demonstrates that all three LCs are scheduled in the same grant (going to the
// same destination), and that three transmissions can be fit into the selection
// window.
//
static ns3::ExampleAsTestSuite g_slMultiLcDynGcastHarq("sl-multi-lc-dyn-gcast-harq",
                                                       "nr-sl-multi-lc-example",
                                                       "src/nr/test/sidelink/sl-test-data",
                                                       "--writeTraces=0");

// The next test is dynamic grants, groupcast, HARQ disabled.  The output
// demonstrates that all three LCs are scheduled in the same grant, but only
// one transmission is scheduled in the grant.
//
static ns3::ExampleAsTestSuite g_slMultiLcDynGcastNoHarq("sl-multi-lc-dyn-gcast-no-harq",
                                                         "nr-sl-multi-lc-example",
                                                         "src/nr/test/sidelink/sl-test-data",
                                                         "--harqEnabled=0 --writeTraces=0");

// The next test is dynamic grants, groupcast, blind retransmissions.  The output
// demonstrates that all three LCs are scheduled in the same grant, and five
// transmissions can be scheduled in the selection window, because there is no
// MinTimeGapPsfch constraint on scheduling.
//
static ns3::ExampleAsTestSuite g_slMultiLcDynGcastBlind("sl-multi-lc-dyn-gcast-blind",
                                                        "nr-sl-multi-lc-example",
                                                        "src/nr/test/sidelink/sl-test-data",
                                                        "--psfchPeriod=0 --writeTraces=0");

// The following two configurations demonstrate the prioritization operation when
// there is no LC prioritization configured.  schedTypeConfig 3 configures dynamic
// grants on flows 1 and 2, and SPS grants on flow 3.  By default, the variable
// 'prioToSps' is false, and as a result, the first grant scheduled is the dynamic
// grant for LCs 4 and 5.
//
static ns3::ExampleAsTestSuite g_slMultiLcPrioDyn("sl-multi-lc-prio-dyn",
                                                  "nr-sl-multi-lc-example",
                                                  "src/nr/test/sidelink/sl-test-data",
                                                  "--schedTypeConfig=3 --writeTraces=0");

// When the 'prioToSps' flag is set to true, the SPS grant (LC 7) is scheduled first.
static ns3::ExampleAsTestSuite g_slMultiLcPrioSps(
    "sl-multi-lc-prio-sps",
    "nr-sl-multi-lc-example",
    "src/nr/test/sidelink/sl-test-data",
    "--schedTypeConfig=3 --prioToSps=1 --writeTraces=0");

// When the 'dstL2IdConfg is set to 3, the first flow to dstL2Id=2 will be sent as
// unicast, the second to dstL2Id=254 will be sent as groupcast, and the third to
// dstL2Id=255 will be sent as broadcast.  This will cause all flows to have to
// use a separate LC.  The priorityConfig value of 2 will cause the broadcast flow
// to be scheduled with highest priority.
static ns3::ExampleAsTestSuite g_slMultiLcPrioBcast(
    "sl-multi-lc-prio-bcast",
    "nr-sl-multi-lc-example",
    "src/nr/test/sidelink/sl-test-data",
    "--dstL2IdConfig=3 --priorityConfig=2 --writeTraces=0");

// When the 'dstL2IdConfig is set to 3 again, and the priorityConfig value set to 3,
// the groupcast and unicast flow will have equal priority value of 2, above that
// of the broadcast (1).  Which one is selected will depend on a random variable
// draw.  With RngRun=1, the groupcast (dstL2Id 254) will be scheduled first,
// while with RngRun=2, the unicast one (dstL2Id 2 LC 5) will be scheduled first.
static ns3::ExampleAsTestSuite g_slMultiLcPrioGcast(
    "sl-multi-lc-prio-gcast",
    "nr-sl-multi-lc-example",
    "src/nr/test/sidelink/sl-test-data",
    "--dstL2IdConfig=3 --priorityConfig=3 --writeTraces=0");
// Repeat with RngRun=2
static ns3::ExampleAsTestSuite g_slMultiLcPrioUni(
    "sl-multi-lc-prio-uni",
    "nr-sl-multi-lc-example",
    "src/nr/test/sidelink/sl-test-data",
    "--dstL2IdConfig=3 --priorityConfig=3 --RngRun=2 --writeTraces=0");

// This test illustrates that use of a non-uniform RRI prevents all LCs from
// being scheduled in the same grant.  rriConfig=2 and schedTypeConfig=2 will allow
// LCIDs 5 and 7, but not 6 (flow 2) to be scheduled in the same grant
// Since flow 2 has a smaller RRI, its packets arrive first and it is the
// first to be scheduled (and saved in the reference log).
static ns3::ExampleAsTestSuite g_slMultiLcRri("sl-multi-lc-rri",
                                              "nr-sl-multi-lc-example",
                                              "src/nr/test/sidelink/sl-test-data",
                                              "--rriConfig=2 --schedTypeConfig=2 --writeTraces=0");
