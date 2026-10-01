.. SPDX-License-Identifier: NIST-Software

.. _NrV2xTests:

Tests for sidelink (mode 2) sensing
===================================

The test suite labeled ``nr-sl-sensing`` provides unit tests on the operation
of the sensing algorithm.  The sensing code in ``NrSlUeMac`` has been written
to permit the unit testing of the method
``NrSlUeMac::GetNrSlCandidateResourcesPrivate()``, which implements the logic
of Section 8.1.4 of [TS38214]_.

.. note::

   Although TS 38.214 describes the sensing-based resource selection as a
   PHY layer procedure, the ns-3 implementation places this algorithm in
   ``NrSlUeMac``.  This design choice was made because the algorithm requires
   access to MAC-layer information (transmit history, buffer status) and
   the sensing data from PHY is passed up to MAC for processing.  The unit
   tests therefore test the MAC implementation of this algorithm.

The main data that is provided to this algorithm is the sensing data,
transmit history, and list of candidate slots within the resource selection
window.  The algorithm produces a list of (filtered) candidate resources
based on additional parameters passed into the method, such as the requested
number of contiguous subchannels for the selected candidate resources.  The
unit tests are able to create specific test conditions by creating artificial
sensing data and transmit history and passing it to the algorithm, and then
by inspecting the resulting candidate resources.  In addition, enabling the
ns-3 log component ``NrSlSensingTest`` will allow details of the sensing
data, transmit history, and operation of the algorithm to be provided as
logging output.

The test suite contains three test case classes:

**NrSlSensingTestCase** - Tests the candidate resource selection algorithm:

1.  The test artificially injects sensing data into the first of two
    subchannels in all slots within the sensing window, and this artificial
    data is provided to the algorithm with a request to find resources that
    are one subchannel wide.  The resulting candidates are inspected to check
    that the expected number of sidelink slots in the selection window are
    available and that only the upper subchannel is listed as a candidate
    resource in each slot.

2.  The test repeats the previous test with the previous sensing data, but
    the request for candidate resources instead requests candidates that are
    two subchannels wide.  As a result, the first pass through the algorithm
    will yield no candidates, because the sensing data presence in the lower
    subchannel blocks the inclusion of each slot.  However, in this case, the
    algorithm will raise the RSRP threshold by 3 dB and try again, and the
    second attempt will succeed to find candidates in all slots.  This output
    condition is checked.

3.  The test repeats the previous test but with different sensing data,
    occupying only two slots within the sensing window, and the algorithm is
    requested to find candidate resources.  The test checks that the candidate
    resources do not include slots that correspond to the projection of the
    slots with sensing data into the selection window, based on the configured
    RRI of 100 ms.

**NrSlSensingTransmitHistoryTest** - Tests transmit history exclusion:

4.  This test injects an artificial transmit history that specifies that
    transmissions took place in two slots within the sensing window (with no
    sensing data).  The test code checks that the corresponding slots in the
    selection window (projected forward by the RRI of 100 ms), are not
    candidates.

**NrSlRemoveOldSensingDataTest** - Tests sensing data cleanup:

5.  This test checks that the method ``NrSlUeMac::RemoveOldSensingData()``
    correctly removes old sensing data that falls outside the sensing window.

Tests for sidelink HARQ
=======================

The test suite labeled ``nr-sl-harq`` sets up a small network of sidelink
UEs with small test applications to generate traffic.  It instruments trace
sources to check for specific outcomes induced by the traffic sending pattern
and an error model that can force reception drops to test HARQ operation.

The test suite contains two test case classes:

**TestSidelinkHarq** - Main HARQ functionality tests:

1.  Check basic broadcast, groupcast, and unicast HARQ configurations with
    no packet losses and the number of UEs varying from 2 to 9.

2.  Check that when HARQ is disabled by TFT configuration that no HARQ
    feedback is sent despite the presence of a PSFCH channel.

3.  Check that when HARQ is enabled but PSFCH period is zero (no PSFCH
    channel), that blind retransmissions are used.

4.  Check the basic operation of HARQ under different TDD and SL bitmap
    patterns and different numerologies (0, 1, 2, 3, 4).

5.  Check behavior for PSSCH loss without HARQ enabled.

6.  Check behavior for both dynamic and SPS grants under no loss conditions.

7.  Check behavior for both dynamic and SPS grants when SCI-1 is lost
    (there should be no HARQ feedback).

8.  Check behavior for both dynamic and SPS grants when SCI-2A is lost
    (there should be no HARQ feedback).

9.  Check behavior for both dynamic and SPS grants when (HARQ timeouts and
    process ID deallocations) when both TBs are lost (HARQ feedback occurs).

10. Check behavior for both dynamic and SPS grants when (HARQ timeouts and
    process ID deallocations) when the first TB is lost (HARQ NACK) but
    second TB is received successfully (HARQ ACK).

11. Check that IPv4 and IPv6 behavior is the same for a few cases.

12. Check that when two senders collide (collision of TB upon reception),
    that HARQ retransmissions recover the TBs, for unicast, groupcast, and
    broadcast.

13. Check the assignment of PSFCH slots to the overall SL pattern for
    various TDD patterns and PSFCH periods.

Tests for the default scheduler
===============================

Tests for the default scheduler (``NrSlUeMacSchedulerDefault``) are split
across two test files.

Unit tests (nr-sl-scheduler-test.cc)
------------------------------------

The test suite labeled ``nr-sl-scheduler`` provides unit tests that directly
exercise scheduler methods.  The test design uses a base class
``NrSlSchedulerTestCase`` that provides a simple two-node sidelink setup,
with subclasses defining specific test scenarios.

1.  **NrSlSchedulerMcsConfigTestCase**: This test verifies that the MCS value
    for a destination can be
    configured to a non-default value even before there is any traffic for
    that destination.  It also verifies that the MCS for a destination can
    be changed during an active flow.  The test sends 10 packets at 1-second
    intervals, changes the MCS from 1 to 2 at time 10 seconds, and verifies
    that packets received before 10 seconds used MCS 1 while packets after
    used MCS 2.

2.  **NrSlSchedulerMultipleTbTestCase**: This test verifies that when a
    packet exceeds the capacity of a single TB, the scheduler creates
    multiple grants at the same scheduling trigger time.  A 300-byte UDP
    packet (which expands to roughly 335 bytes at the RLC) requires two
    TBs at MCS 0 (185-byte TB, 183-byte payload).  The test asserts that
    exactly two dynamic grants are created in the same slot and that the
    candidate resource pool shrinks between the first and second grant
    allocation.

3.  **NrSlSchedulerNumTbCalculationTestCase**: This test verifies
    ``CalculateNumTbNeeded()`` for the boundary case where a packet fits
    exactly within one TB.  A 148-byte UDP packet expands to a 183-byte
    RLC PDU, which fits in a single 185-byte TB (183-byte payload plus
    2-byte MAC subheader).  The test asserts that exactly one grant is
    created.

4.  **NrSlSchedulerHarqExhaustionTestCase**: This test verifies scheduler
    behavior when a large packet requires more TBs than the 16 available
    HARQ process IDs.  A 4000-byte UDP packet requires 23 TBs at MCS 0.
    The test asserts that 16 grants are created in the first batch
    (exhausting all HARQ IDs), and 7 additional grants are created one at
    a time as HARQ processes are freed after transmission.  It also
    verifies that all grants use unique transmission slots and that each
    later grant has a distinct allocation time.

5.  **NrSlSchedulerSpsTestCase**: This test validates baseline SPS grant
    behavior by sending 100 packets at 20 ms intervals on an SPS logical
    channel.  The test asserts that all 100 packets are received and that
    every packet arrives with end-to-end latency under 20 ms.

6.  **NrSlSchedulerSpsJitterTestCase**: This test sends 10 packets on an
    SPS logical channel at a nominal 20 ms interval, but introduces jitter
    by modifying the inter-packet interval at specific times (30 ms, then
    10 ms, then back to 20 ms).  The jitter causes two packets to queue
    before an SPS grant is available.  The test is parameterized by packet
    size and MCS:

    - At MCS 0 with 20-byte and 130-byte packets, the TB is too small
      for two packets, so a supplemental dynamic grant is triggered.
    - At MCS 14 with 20-byte packets, TB expansion absorbs both packets
      in a single grant (no supplemental grant needed).
    - At MCS 14 with 100-byte packets, the TB cannot hold two packets,
      so a supplemental dynamic grant is required.

    The test asserts that all packets are received within the latency
    deadline and that the expected number of supplemental grants are
    created.

7.  **NrSlSchedulerDynamicTbExpansionTestCase**: This test verifies that
    TB expansion at grant publication time allows a dynamic grant to carry
    more data than the original buffer that triggered it.  A 20-byte
    UdpClient packet (which includes the 12-byte SeqTsHeader) expands to
    a 53-byte RLC SDU (20 + 8 UDP + 20 IP + 5 PDCP), producing a 55-byte
    buffer status (53 + 2 estimated RLC header).  At MCS 14, the scheduler
    fits this in 1 subchannel (TB 253 bytes, payload 251 bytes).  Two
    packets are sent 2 ms apart so that the first packet triggers a grant
    sized for 55 bytes before the second packet arrives.  The second packet
    arrives before the first grant publishes and triggers a second grant
    for the excess.  At publication of the first grant, TB expansion gives
    the full 251-byte payload to the RLC, which dequeues both packets
    (110 bytes total).  The test asserts that both packets are received at
    the receiver before the second dynamic grant is published, proving that
    the first grant carried both packets via TB expansion.

8.  **NrSlSchedulerRlcSegmentationTestCase**: This test verifies that the
    scheduler correctly accounts for RLC segmentation overhead when
    multiple SDUs span TB boundaries.  Three SDUs (100, 100, and 160
    bytes) produce a 366-byte buffer.  At MCS 0, the scheduler accounts
    for the 2-byte per-segment RLC header overhead and allocates 3 TBs
    (ceil(366/181) = 3) rather than the 2 that would be computed without
    the overhead adjustment.  The test also connects a trace source on the
    RLC transmit buffer to verify that no stray bytes remain after
    transmission.

9.  **NrSlSchedulerSpsReselectionTestCase**: This test verifies that the
    scheduler correctly reselects SPS grants with new resources after
    each time that the reselection counter expires. This test is run
    with slProbResourceKeep set to the minimum value of 0.

10. **NrSlSchedulerSpsReselectionTestCase**: This test verifies that the
    scheduler usually keeps but sometimes reselects SPS grants with new
    resources after each time that the reselection counter expires. This
    test is run with slProbResourceKeep set to the maximum value of 0.8.

Example-based tests (nr-sl-simple-multi-lc-example-test.cc)
-----------------------------------------------------------

A number of tests of the default scheduler have been defined using the ns-3
``ExampleAsTestSuite`` framework to run the example program
``nr-sl-multi-lc-example.cc`` in different configurations and check the
output against reference output stored in the test directory ``sl-test-data``.

The following tests are implemented:

1.  **sl-multi-lc-dyn-gcast-harq**: Dynamic grants, groupcast, HARQ enabled.
    The output demonstrates that all three LCs are scheduled in the same
    grant (going to the same destination), and that three transmissions can
    be fit into the selection window subject to the MinTimeGapPsfch constraint.

2.  **sl-multi-lc-dyn-gcast-no-harq**: Dynamic grants, groupcast, HARQ
    disabled.  The output demonstrates that all three LCs are scheduled in
    the same grant, but only one transmission is scheduled (no retransmissions).

3.  **sl-multi-lc-dyn-gcast-blind**: Dynamic grants, groupcast, blind
    retransmissions (HARQ enabled but PSFCH period = 0).  The output
    demonstrates that all three LCs are scheduled in the same grant, and
    five transmissions can be scheduled in the selection window because
    there is no MinTimeGapPsfch constraint.

4.  **sl-multi-lc-prio-dyn**: Tests prioritization when ``schedTypeConfig=3``
    configures dynamic grants on flows 1 and 2, and SPS grants on flow 3.
    With ``prioToSps=false`` (default), the first grant scheduled is the
    dynamic grant for LCs 5 and 6.

5.  **sl-multi-lc-prio-sps**: Same configuration as above but with
    ``prioToSps=true``.  The SPS grant (LC 7) is scheduled first.

6.  **sl-multi-lc-prio-bcast**: Tests LC assignment with different
    destination types.  With ``dstL2IdConfig=3``, flow 1 (dstL2Id=2) is
    unicast, flow 2 (dstL2Id=254) is groupcast, and flow 3 (dstL2Id=255) is
    broadcast.  Each flow uses a separate LC.  With ``priorityConfig=2``,
    the broadcast flow is scheduled with highest priority.

7.  **sl-multi-lc-prio-gcast**: With ``dstL2IdConfig=3`` and
    ``priorityConfig=3``, the groupcast and unicast flows have equal
    priority (2), above that of broadcast (1).  Selection between equal
    priorities depends on random variable draw.  With ``RngRun=1``, the
    groupcast (dstL2Id=254) is scheduled first.

8.  **sl-multi-lc-prio-uni**: Same as above but with ``RngRun=2``.  The
    unicast flow (dstL2Id=2, LC 5) is scheduled first.

9.  **sl-multi-lc-rri**: Tests that non-uniform RRI prevents LCs from
    sharing a grant.  With ``rriConfig=2`` and ``schedTypeConfig=2``,
    LCIDs 5 and 7 can share a grant (same RRI), but LCID 6 (different RRI)
    must be scheduled separately.

Tests for the Ideal MCS Controller
==================================

The test suite labeled ``nr-sl-ideal-mcs-controller`` tests the
``NrSlIdealMcsController`` class, which dynamically selects MCS based on
RSRP measurements to minimize the number of subchannels while maintaining
link reliability.

**NrSlIdealMcsControllerTest** - Tests MCS selection logic:

The test verifies that the controller correctly selects the combination of
MCS and number of subchannels that:

- Provides sufficient TB size for the requested data
- Uses the minimum number of subchannels
- Selects the most robust (lowest) MCS among options with equal subchannel count

Tests for the SL RLC UM
=======================

The test suite labeled ``nr-sl-rlc-um`` tests the sidelink RLC Unacknowledged
Mode implementation.

**NrSlRlcUmOneSduTestCase** - Tests basic RLC UM operation:

- Verifies that a single SDU is correctly transmitted and received
- Tests segmentation and reassembly

**NrSlRlcUmPdbTestCase** - Tests Packet Delay Budget handling:

- Verifies that packets exceeding their PDB are correctly discarded
- Tests the interaction between PDB and RLC queue management

Tests for SL Transport Block Size
=================================

The test suite labeled ``nr-sl-transport-block-size`` tests the TB size
calculation in ``NrSlCommResourcePool::GetTransportBlockSize()``.

**NrSlTransportBlockSizeTest** - Tests TB size calculation:

- Verifies TB size calculations match expected values from TS 38.214
- Tests various combinations of MCS, number of subchannels, and symbols
- Validates against reference values from the NIST Link Level Simulator

Tests for SL Error Model
========================

The test suite labeled ``nr-sl-error-model`` tests the sidelink PHY error
model.

**NrSlStaticErrorModelTest** - Tests error model behavior:

- Verifies BLER calculations for various SINR values
- Tests both PSCCH and PSSCH error models
- Validates error model outputs against expected values

Tests for SCI Headers
=====================

The test suite labeled ``nr-sl-sci-headers`` tests the serialization and
deserialization of Sidelink Control Information headers.

**NrSlSciF1aTestCase** - Tests SCI Format 1-A:

- Verifies correct encoding and decoding of SCI 1-A fields
- Tests priority, frequency resource assignment, time resource assignment,
  and other SCI 1-A parameters

**NrSlSciF2aTestCase** - Tests SCI Format 2-A:

- Verifies correct encoding and decoding of SCI 2-A fields
- Tests HARQ process ID, NDI, source/destination L2 IDs, and other
  SCI 2-A parameters
