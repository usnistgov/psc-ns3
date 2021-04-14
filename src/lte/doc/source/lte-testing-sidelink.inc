++++++++++++++++++
LTE Sidelink tests
++++++++++++++++++

----------
Unit Tests
----------

Sidelink Communication pool
+++++++++++++++++++++++++++

The test suite ``Sidelink-comm-pool`` checks the validity of Sidelink
communication parameters computed during a simulation, e.g., the expected
starting frame and subframe number, index of the starting Resource Block (RB),
and the number of RBs in a subframe. The test suite contains a pre-configured
communication pool for which these parameters are computed. Table
:ref:`tab-sl-comm-pool-params` below lists the configuration of the pool used.

.. _tab-sl-comm-pool-params:

.. table:: Sidelink communication pool configuration

   ==============================	============================ 
   Parameter				 Value    
   ==============================	============================ 
   Control period			 sf120 (120 ms)
   Control bitmap			 0x0000000003 (40 bits)
   Number of control RBs 		 1
   First control RB index 		 10
   End control RB index			 40
   Control Offset			 80 ms
   Data Bitmap  			 0xCCCCCCCCCC (40 bits)
   Number of data RBs   		 1
   First data RB index  		 0
   End data RB index			 49
   Data Offset  			 8 ms
   ==============================	============================ 

There are four test cases:

1. SidelinkCommPoolNextFrameSubframeTestCase
--------------------------------------------
This test checks the starting frame and subframe number of the next Sidelink
Control (SC) period given the current frame and subframe number. Let :math:`Fr`
be the current frame number and :math:`Sf` the current subframe number. The
absolute subframe number :math:`Sf_\mathrm{abs}`, since the :math:`Sf = 0` of
:math:`Fr = 0` is given by :

.. math::

   Sf_\mathrm{abs} = 10 * (Fr \bmod 1024) + Sf \bmod 10;

The number of current Sidelink control period :math:`ScPr_\mathrm{cur}` is
calculated as, 

.. math::

   ScPr_\mathrm{cur} = \left\lfloor \frac{Sf_\mathrm{abs} - Sc_\mathrm{offset}}{ScPr} \right\rfloor

where :math:`Sc_\mathrm{offset}` is the Sidelink control offset and :math:`ScPr` is the Sidelink control period from 
Table :ref:`tab-sl-comm-pool-params`. The range of :math:`ScPr_\mathrm{cur}` is : :math:`0 \leq ScPr_\mathrm{cur} < \frac{10240}{ScPr}`

If the frame number is the last period, the next SC period will start after :math:`Sc_\mathrm{offset}`

Finally, the next start frame and subframe number is calculated as,

.. math::

   Next_\mathrm{start} = Sc_\mathrm{offset} + ScPr_\mathrm{cur} * ScPr

   Fr_\mathrm{next} = \frac{Next_\mathrm{start}}{10} \bmod 1024
   
   Sf_\mathrm{next} = Next_\mathrm{start} \bmod 10

Three different configuration has been tested:

1. :math:`Fr = 0` and :math:`Sf = 5` 

For this configuration, the expected behavior is that the next Sidelink control
period will start in frame number 8 and subframe 0 because of the
:math:`Sc_\mathrm{offset}` of 80 ms.

2. :math:`Fr = 1023` and :math:`Sf = 0`

This configuration tests the roll over upon reaching the maximum frame number
range. Here, the expected behavior is to have a :math:`Sc_\mathrm{offset}` of 80
ms once again starting from :math:`Sf = 0` of :math:`Fr = 0`.

3. :math:`Fr = 1002` and :math:`Sf = 0`

This configuration reflects the condition when the current frame and subframe
number are part of an ongoing Sidelink control period. The expected behavior is
that the next period will start in frame 1004 and subframe 0, i.e., right after
the remaining frames/subframes of the current Sidelink control period.

2. SidelinkCommPoolResourceOpportunityTestCase
----------------------------------------------
This test takes the PSCCH resource and transmission number as an input and
verifies the frame, subframe number, index of the starting RB, and the number of
RBs available in a subframe. The total number of resources in a PSCCH pool are
calculated as per [TS36213]_ 14.2.1.1.

.. math::

   RES_\mathrm{PSCCH} = L_\mathrm{PSCCH} * \left\lfloor \frac{RB_\mathrm{PSCCH}} {2} \right\rfloor

where 

:math:`L_\mathrm{PSCCH} = 2` is the total number of subframes available for PSCCH in this pool

:math:`RB_\mathrm{PSCCH} = 2` (i.e., RB 10 and RB 40) is the total number of resource blocks per subframe available 
for PSCCH in this pool.

Hence, :math:`RES_\mathrm{PSCCH} = 2`

These resources in time and frequency are indexed from 0 to
:math:`RES_\mathrm{PSCCH} - 1`. In this test, it will range from 0 -> 1,
therefore, we test both the PSCCH resources to verify the frame/subframe number,
index of the starting RB and the total number of RBs in a subframe available for
Sidelink Control Information (SCI) transmission. We note that, to ensure the
reliability of the SCI message delivery, each message is transmitted in two
identical instances, where each instance occupies one RB. The two instances are
transmitted over two different subframes. Therefore, for each PSCCH resource
there will be two transmissions. As per [TS36213]_ 14.2.1.1, for a resource
index :math:`n_\mathrm{PSCCH}` first transmission occurs in RB :math:`a1` of
subframe :math:`b1` where

.. math::

   a1 = \left\lfloor \frac{n_\mathrm{PSCCH}} {L_\mathrm{PSCCH}} \right\rfloor 

   b1 = n_\mathrm{PSCCH} \bmod L_\mathrm{PSCCH}

and the second transmission occurs in RB :math:`a2` of subframe :math:`b2` where

.. math::

   a2 = \left\lfloor \frac{n_\mathrm{PSCCH}} {L_\mathrm{PSCCH}} \right\rfloor + \left\lfloor \frac{RB_\mathrm{PSCCH}} {2} \right\rfloor

   b2 = (n_\mathrm{PSCCH} + 1 + \frac{n_\mathrm{PSCCH}} {L_\mathrm{PSCCH}} \bmod (L_\mathrm{PSCCH} - 1)) \bmod L_\mathrm{PSCCH}

:math:`RES_\mathrm{PSCCH} = 2` would mean that we have following four configuration to verify,

1. :math:`n_\mathrm{PSCCH} = 0`, Transmission 0:

For :math:`n_\mathrm{PSCCH} = 0`, the expected behavior is that the first
transmission of PSCCH would be in :math:`RB = 10` of :math:`Fr = 0` and
:math:`Sf = 0` and number of RBs for this transmission would be 1.

2. :math:`n_\mathrm{PSCCH} = 0`, Transmission 1:

For :math:`n_\mathrm{PSCCH} = 0`, the expected behavior is that the second
transmission of PSCCH would be in :math:`RB = 40` of :math:`Fr = 0` and
:math:`Sf = 1` and number of RBs for this transmission would be 1.

3. :math:`n_\mathrm{PSCCH} = 1`, Transmission 0:

For :math:`n_\mathrm{PSCCH} = 1`, the expected behavior is that the first
transmission of PSCCH would be in :math:`RB = 40` of :math:`Fr = 0` and
:math:`Sf = 0` and number of RBs for this transmission would be 1.

4. :math:`n_\mathrm{PSCCH} = 1`, Transmission 1:

For :math:`n_\mathrm{PSCCH} = 1`, the expected behavior is that the second
transmission of PSCCH would be in :math:`RB = 10` of :math:`Fr = 0` and
:math:`Sf = 1` and number of RBs for this transmission would be 1.

3. SidelinkCommPoolSubframeOpportunityTestCase
----------------------------------------------
This test verifies index/indices of the RBs available for a given frame -
subframe number. The following three configurations have been tested.

1. :math:`Fr = 8` and :math:`Sf = 0`

This configuration refers to the start of SC period, i.e., after 80 ms of offset
from :math:`Fr = 0` and :math:`Sf = 0`. Thus, we should be on the first subframe
of the control channel. The expected RB indices available for this subframe are
10 and 40.

2. :math:`Fr = 8` and :math:`Sf = 1`

This configuration refers to second subframe of the SC period. The expected RB
indices available for this subframe are 10 and 40.

3. :math:`Fr = 8` and :math:`Sf = 2`

This configuration refers the subframe number, which is not part of SC period as
indicated by control bit map. Thus, in this subframe none of the resource block
will be allocated for PSCCH transmission.

.. _SidelinkCommPoolPsschTestCase:

4. SidelinkCommPoolPsschTestCase
--------------------------------
This test checks for the frame-subframe number for PSSCH transmissions. In this
test beside the parameters from the pool configuration, an important parameter,
i.e., Time Resource Pattern index (iTRP) is provided. All the tests are
performed with :math:`iTRP = 5`, which corresponds to the bit map of
(0,0,0,0,0,1,0,0) given by Table 14.1.1.1.1-1 of [TS36213]_. Every
transmission on PSSCH is transmitted using 4 HARQ processes using 4 different
subframes. Therefore, this test verifies the frame and subframe number of all
the 4 transmissions.

1. :math:`Fr = 0`, :math:`Sf = 5`, :math:`iTRP = 5`, Transmission 0

Starting from :math:`Fr = 0`, the first PSSCH transmission should occur on subframe :

.. math::

   Sf_\mathrm{T0} = Sc_\mathrm{offset} + Data_\mathrm{offset} + Sf_\mathrm{TRP}

where :math:`Sc_\mathrm{offset}` and :math:`Data_\mathrm{offset}` values are
from the Table :ref:`tab-sl-comm-pool-params` and :math:`Sf_\mathrm{TRP}` are
obtained after applying TRP bit map on active bits of data bit map. For example,

.. figure:: figures/lte-sl-pssch-bitmap.*
   :width: 80 %
   :align: center

If we apply the TRP bit map on the active bits of data bit map, starting from
the least significant bit (lbs) of TRP bit map the sixth bit indicates an active
subframe. In other words, an offset of 11 subframes as per data bit map, hence,
:math:`Sf_\mathrm{TRP} = 11`. By using above equation the first PSSCH
transmission since subframe 0, would occur on 80 + 8 + 11 = 99 subframe, i.e.
:math:`Fr = 9` and :math:`Sf = 9`.

Similarly, the remaining three transmissions and all other transmissions will
occur every 16 ms. This has been tested by the following tests.

2. :math:`Fr = 0`, :math:`Sf = 5`, :math:`iTRP = 5`, Transmission 1

The transmission should occur after 16 ms of transmission 0, i.e., in :math:`Fr = 11`, :math:`Sf = 5`.

3. :math:`Fr = 0`, :math:`Sf = 5`, :math:`iTRP = 5`, Transmission 2

The transmission should occur after 16 ms of transmission 1, i.e., in :math:`Fr = 13`, :math:`Sf = 1`.

4. :math:`Fr = 0`, :math:`Sf = 5`, :math:`iTRP = 5`, Transmission 3

The transmission should occur after 16 ms of transmission 2, i.e., in :math:`Fr = 14`, :math:`Sf = 7`.    

Sidelink discovery pool
+++++++++++++++++++++++
The test suite ``Sidelink-disc-pool`` checks the validity of Sidelink discovery
parameters computed during a simulation, e.g., the expected starting frame and
subframe number, index of the starting Resource Block (RB) and the number of RBs
in a subframe. The test suite contains a pre-configured discovery pool for which
these parameters are computed. Table :ref:`tab-sl-disc-pool-params` below lists
the configuration of the pool used.

.. _tab-sl-disc-pool-params:

.. table:: Sidelink discovery pool configuration

   ==============================	============================ 
   Parameter				 Value    
   ==============================	============================ 
   Discovery cyclic prefix length	 NORMAL
   Discovery period			 rf32 (320 ms)
   Number of retransmissions  		 0
   Number of repetition 		 1
   Number of RBs	 		 1
   First RB index	 		 10
   End RB index 			 11
   Offset 				 80 ms
   Bitmap 				 0x0000000003 (40 bits)
   ==============================	============================ 

There are three test cases:

1. SidelinkDiscPoolNextFrameSubframeTestCase
--------------------------------------------
This test checks the starting frame and subframe number of the next Discovery
Period (DiscPr) given the current frame and subframe number. Let :math:`Fr` be
the current frame number and :math:`Sf` the current subframe number. The
absolute subframe number :math:`Sf_\mathrm{abs}` , since :math:`Sf = 0` of
:math:`Fr = 0` is given by :

.. math::

   Sf_\mathrm{abs} = 10 * (Fr \bmod 1024) + Sf \bmod 10;

The number of current discovery period :math:`DiscPr_\mathrm{cur}` is calculated as, 

.. math::

   DiscPr_\mathrm{cur} = \left\lfloor \frac{Sf_\mathrm{abs} - Disc_\mathrm{offset}}{DiscPr} \right\rfloor

where :math:`Disc_\mathrm{offset}` is the discovery offset and :math:`DiscPr`
is the discovery period from Table :ref:`tab-sl-disc-pool-params`. The range of
:math:`DiscPr_\mathrm{cur}` is : :math:`0 \leq DiscPr_\mathrm{cur} < \frac{10240}{DiscPr}`

If the frame number is the last period, the next DiscPr will start after :math:`Disc_\mathrm{offset}`

Finally the next start frame and subframe number are calculated as,

.. math::

   Next_\mathrm{start} = Disc_\mathrm{offset} + DiscPr_\mathrm{cur} * DiscPr

   Fr_\mathrm{next} = \frac{Next_\mathrm{start}}{10} \bmod 1024
   
   Sf_\mathrm{next} = Next_\mathrm{start} \bmod 10

Four different configurations have been tested:

1. :math:`Fr = 0` and :math:`Sf = 5` 

For this configuration, the expected behavior is that the next discovery period
will start in frame number 8 and subframe 0 because of the
:math:`Disc_\mathrm{offset}` of 80 ms.

2. :math:`Fr = 8` and :math:`Sf = 0`

Here, we are at the start of first discovery period the next discovery period
will start after :math:`DiscPr = 320 ms` in frame number 40 and subframe 0.

3. :math:`Fr = 1023` and :math:`Sf = 0`

This configuration tests the roll over upon reaching the maximum frame number
range. Here the expected behavior is to have a :math:`Disc_\mathrm{offset}` of
80 ms once again starting from frame number 0.

4. :math:`Fr = 800` and :math:`Sf = 0`

This configuration reflects the condition when the current frame and subframe
number are part of an ongoing discovery period. The expected behavior is that
the next discovery period will start in frame 808 and subframe 0, i.e., right
after the remaining frames/subframes of the current discovery period.

2. SidelinkDiscPoolResourceOpportunityTestCase
----------------------------------------------
This test takes as input the PSDCH resource number from the configured pool and
checks for the correct frame/subframe number of the starting RB and the total
number of RBs in a subframe assigned for Sidelink discovery. The computation of
these parameters are done as per 14.3.1 and 14.3.3 of [TS36213]_. Specifically,
the methods ``GetPsdchTransmissions`` and ``ComputeNumberOfPsdchResources`` in
``lte-sl-pool.cc``, respectively implements the mentioned clauses of the
standard.

The total number of resources for PSDCH are calculated as,

.. math::

   RES_\mathrm{PSDCH} = \left\lfloor \frac{L_\mathrm{PSDCH}}{Retx + 1} \right\rfloor * \left\lfloor \frac{RB_\mathrm{PSDCH}} {2} \right\rfloor

where 

:math:`L_\mathrm{PSDCH} = 2` is the total number of subframes available for
PSDCH in this pool

:math:`Retx = 0` is the total number of retransmission allowed

:math:`RB_\mathrm{PSDCH} = 2` (i.e., RB 10 and RB 11) is the total number of
resource blocks per subframe available for PSDCH in this pool.

These resources in time and frequency are indexed from 0 to
:math:`RES_\mathrm{PSDCH} - 1`. In this test, it will range from 0 -> 1,
therefore, we test both the PSDCH resources to verify the frame/subframe number,
index of the starting RB and the total number of RBs in a subframe available for
Sidelink discovery.

1. PsdchResourceNo:0

By selecting the PsdchResourceNo = 0 the expected frame and subframe would be 0
and since, the first RB index of the configured pool is 10 the index of the
starting RB is expected to be 10 and, at last, the total number of resource
blocks are expected to be 2.

2. PsdchResourceNo:1 

Compared to the PsdchResourceNo:0 case, by choosing resource number 1 we expect
the discovery transmission to occur on subframe = 1.

3. SidlelinkDiscPoolRbsPerSubframeTestCase
-------------------------------------------
This test checks for the total number of resource blocks available in a single
subframe and also verifies the indices of the resource blocks. The following
three configurations have been tested.

1. :math:`Fr = 8` and :math:`Sf = 0`

If we go to 80 ms, we should be on the first subframe of the discovery channel 
and RB 10 and 11 should be assigned for discovery transmission.

2. :math:`Fr = 8` and :math:`Sf = 1`

If we go to 81 ms, we should be on the second subframe of the discovery channel 
and RB 10 and 11 should be assigned for discovery transmission.

2. :math:`Fr = 8` and :math:`Sf = 2`

If we go to 82 ms, we should be outside the discovery channel and there
should be no resource block assignment during this subframe.

System Tests
++++++++++++

In-coverage Sidelink communication
----------------------------------
The test suite ``sidelink-in-coverage-comm`` is a system test to test
in-coverage Sidelink communication. In particular, it contains two cases to test
Mode 1 and Mode 2 for Sidelink communication, respectively. In Mode 1, D2D
communications are assisted by the eNodeB, i.e., resource scheduling is
performed dynamically by the eNodeB. In Mode 2, UEs manage resource scheduling
autonomously relying on pre-configured settings, and both, PSCCH and PSSCH,
resources are selected at random from their respective resource pools.

This test creates a scenario with two UEs, which are in coverage of one eNodeB.
One of the UE will send traffic directly to the other UE via Sidelink as per
configured mode, i.e., Mode 1 or Mode 2. Default configuration will send 10
packets per second for 2 seconds. The expected output is that the receiver UE
node would receive 20 packets.

Out of coverage Sidelink communication
--------------------------------------
Similar to the in-coverage test, the test suite
``sidelink-out-of-coverage-comm`` is a system test to test the out of coverage
Sidelink communication. This test creates a scenario with two out of coverage
UEs, where one of the UE will send traffic directly to the other UE via
Sidelink. Default configuration will send 10 packets per second for 2 seconds.
The expected output is that the receiver UE node would receive 20 packets.

Sidelink synchronization
------------------------
The test suite ``sidelink-synch`` is a system test to test the synchronization
of the UEs in a test scenario with a selected SyncRef UE. In particular, it
simulates the following two topologies by varying the traffic type, traffic
direction in the sidelink TFT, and the random stream.

.. _fig-lte-sl-synch-test-topology1:
.. figure:: figures/lte-sl-synch-test-topology1.*
   :align: center

   Sidelink synchronization test topology 1

.. _fig-lte-sl-synch-test-topology2:
.. figure:: figures/lte-sl-synch-test-topology2.*
   :align: center

   Sidelink synchronization test topology 2

There are four test cases,
two for each topology. Each of the four tests uses ten different random number
streams. The reason behind changing the random stream value is to make sure that
at the time of applying the change of timings, the newly computed frame and
subframe number is aligned with the selected SyncRef UE since the computation of an
absolute subframe number is sensitive to the combination of the frame and
subframe number used. Currently, all the tests are performed using an OnOff
application configured to generate CBR traffic. The test for the alignment of
the frame and subframe number is performed after 0.3 ms the trace
``ChangeOfSyncRef`` gets trigger from the RRC layer of the synchronizing UE.
This delay of 0.3 ms is needed because if the SynchRef UE is instantiated after
the synchronizing UE in the simulation script, the event triggering the
subframe indication method, which is responsible to update the frame and
subframe number at the physical layer of the SyncRef will happen after this UE.

Let's take an example of node 5 (RX UE) and node 6 (TX UE/SyncRef) in topology
2. In the following logs, node 5 is synchronizing with node 6 by changing its
frame to 982 and subframe to 7. At this stage, the frame and subframe number
at the SyncRef UE (i.e., Node 6) would be 982 and 6, respectively. This is
because the ``SubframeIndication`` method with frame 982 and subframe 7 for node
6 is later in the simulator event list. Therefore, we chose to delay the
CheckSfnAlignment method in the test. ::

  +6.440000000s 5 LteUePhy:SubframeIndication(0xf6e430, 973, 10)

  +6.440000000s 5 LteUePhy:ChangeOfTiming(0xf6e430, 973, 10)

  +6.440000000s 5 LteUePhy:ChangeOfTiming(): The UE is not currently transmitting
                  Sidelink communication... Applying the change of timing

  +6.440000000s 5 LteUePhy:ChangeOfTiming(): mibTime frame/subframe = 982/3

  +6.440000000s 5 LteUePhy:ChangeOfTiming(): currentTime frame/subframe = 973/10

  +6.440000000s 5 LteUePhy:ChangeOfTiming(): rxSubframe frame/subframe = 973/6

  +6.440000000s 5 LteUePhy:ChangeOfTiming(): UE RNTI 3 does not have a Tx pool and
                  changed the Subframe Indication from 973/10 to 982/7

  +6.440000000s 5 LteUeRrc:DoReportChangeOfSyncRef(0xf70080)

  +6.440000000s 5 LteUeRrc:DoReportChangeOfSyncRef(): 0xf70080 UE IMSI 3
                  reported successful change of SyncRef, selected SyncRef SLSSID
                  40 offset 36

  +6.440000000s 5 LteUePhy:SubframeIndication(): (re)synchronization successfully
                  performed
   .
   .
  +6.440000000s 6 LteUePhy:SubframeIndication(0xf73e60, 982, 7)

  +6.440299999s Time of CheckSfnAlignment in the test


Finally, the test would pass if the synchronizing UE has a similar
frame and subframe number as its SyncRef UE.

In-coverage Relay communication
-------------------------------

The test suite ``sl-in-covrg-1relay-1remote-regular`` is a system test
to test the in-coverage sidelink communication scenario between a
Relay UE and a Remote UE. The test uses as parameters the simulation
time, number of packets for the echo client application, the interval
between packets for the echo client application, and the packet size
for the echo client application. The echo client is installed on the
Remote UE and the echo server is installed on a remote host machine on
the network. After the successful setup of the one-to-one
communication state machine, the packets are expected to flow from the
Remote UE to the Relay UE and packet gateway and ultimately end up at
the remote host, which echoes the packets back on the reverse route. The
test is considered to have passed if both the UEs reach the right
state by transmitting the ‘Remote UE Info Request’ and ‘Remote UE Info
Response’ messages between each other and if at least one packet has
been echoed between the Remote UE and the remote host.

The test suite ``sl-in-covrg-1relay-1remote-keepalive`` modifies the
timers associated with the one-to-one communication state machine to
force the Relay UE and the Remote UE to transmit ‘Direct Communication
Keepalive’ and ‘Direct Communication Keepalive Acknowledgement’
messages between each other. The test is considered to have passed if
both the UEs keep the link alive by receiving the respective keepalive
messages.

The test suites ``sl-in-covrg-1relay-1remote-disconnect-remote`` and
``sl-in-covrg-1relay-1remote-disconnect-relay`` forces the Remote UE
and the Relay UE respectively to relinquish the established one-to-one
communication link. The tests are considered to have passed if the
appropriate UE sends the ‘Direct Communication Release’ message and
the other UE responds with the ‘Direct Communication Release Accept’
message.

The test suites ``sl-in-covrg-1relay-1remote-reconnect-remote`` and
``sl-in-covrg-1relay-1remote-reconnect-relay`` forces the Remote UE
and the Relay UE respectively to relinquish the established one-to-one
communication link and reestablish it again later in the
simulation. Since the UEs are in coverage of the network, once the
sidelink path for the packets between the UEs is removed, the Remote
UE uses the uplink path to send the packets to the remote host
machine. The tests are considered to have passed if the packets
inspected at the remote host have appropriate source IP addresses at
specific times in the scenario.

The test suite ``sl-in-covrg-1relay-2remote-regular`` is a system test
to test the in-coverage sidelink communication scenario between a
Relay UE and two Remote UEs. Each Remote UE sends certain number of
packets to the remote host and receives the echoes of the packets from
the remote host. The test is considered to have passed if the
collective number of packets sent from the Remote UEs is equal to the
collective number of packets received at the Remote UEs.

The test suite ``sl-in-covrg-2relay-1remote-regular`` is a system test
to test the in-coverage sidelink communication scenario between two
Relay UEs and a Remote UE. The remote host can receive packets
originating from the Remote UE through either Relay UEs. To ensure the
packets have taken both paths through each Relay UE, the source
address of the packets received at the remote host are inspected for
two different IP addresses. The test is considered to have passed if
the number of packets received at the remote host is equal to the
number of packets sent from the remote host and the received packets
at the remote host are from two different source IP addresses.

The test suite ``sl-in-covrg-2relay-1remote-disconnect-relay`` builds
on the ``sl-in-covrg-2relay-1remote-regular`` by disconnecting the
Relay UEs one by one.  Once the first Relay UE is disconnected, the
Remote UE is forced to use the second Relay UE. Once the second Relay
UE is also disconnected, the Remote UE is forced to use its
uplink. The test is considered to have passed if the received packets
at the remote host are from three different source IP addresses,
namely Remote UE uplink IP address, Remote UE IP address under first
Relay UE and Remote UE IP address under second Relay UE.

Out of coverage Relay communication
-----------------------------------

The test suite ``sl-ooc-1relay-1remote-regular`` adds to the
``sl-in-covrg-1relay-1remote-regular`` test suite by having the
necessary preconfiguration in the Remote UE to help the UEs operate
out of network coverage. The out-of-coverage and in-coverage scenarios
are similar in input parameters and the passing conditions for the
individual test cases. The test suite
``sl-ooc-1relay-1remote-misaligned`` is a special case of the test
suite ``sl-ooc-1relay-1remote-regular`` where the preconfiguration
parameters can be modified for the Remote UE. The comments provided in
the test suite tabulate the modifications that can be made to the
variables and the responses that can be expected from running the test
suite.

The test suites ``sl-ooc-relay-generic-traffic-rm2rh`` and 
``sl-ooc-relay-generic-traffic-rm2rm`` are system tests to verify the 
UE-to-Network Relay functionality in scenarios with multiple Relay UEs and 
with multiple out-of-coverage Remote UEs that are not attached to the network.
Both tests suites share the same topology comprising 'nRelayUes' Relay UEs
that are deployed around the eNB uniformly on a circle of a given radius. Each
Relay UE has a cluster of 'nRemoteUesPerRelay' Remote UEs deployed around 
itself uniformly on a circle of another given radius. The cluster of Remote UEs
around a given Relay UE are interested only in its Relay Service Code and thus
will connect only to that Relay UE. The UEs start their relay service 
sequentially in time. First the Relay UE, then the cluster of Remote UEs 
associated to that Relay UE (sequentially as well), then the next Relay UE, and
so on.

The difference between both test suites is the direction of the traffic. 
In ``sl-ooc-relay-generic-traffic-rm2rh``, each Remote UE sends traffic to a 
Remote Host in the network, which echoes back the message to the Remote UE. In 
``sl-ooc-relay-generic-traffic-rm2rm``, the first Remote UE connected to the
first Relay UE is the echoServer. Each of the other Remote UEs send traffic to 
that Remote UE, which echoes back the message to the corresponding initiating 
Remote UE. In both test suites, each initiating Remote UE starts sending 
traffic 1.00 s after the start of the one-to-one connection procedure with its 
Relay UE and remain active during 10.0 s. The simulation time is calculated so 
that the last Remote UE can have its 10.0s of traffic activity. 
Each test suite contains several test cases with different number of Relay UEs 
(nRelayUes) and Remote UEs per Relay UE (nRemoteUesPerRelay).
Each test case passes if at least 50\% of the initiating Remote UEs in the test
received back at least 50\% of their transmitted packets. Given that the 
Remote UEs are out-of-coverage, the only way that they can reach the peer node 
(Remote Host or first Remote UE depending on the suite) and receive back the 
echoed message, is by successfully connecting to the Relay UE. Thus, these two 
test suites can be used to verify both UE-to-Network Relay one-to-one link 
setup and UE-to-Network Relay communication. 


In-coverage Relay discovery
--------------------------- 

The test suite ``sl-in-covrg-discovery-1relay-1remote`` modifies
``sl-in-covrg-1relay-1remote-regular`` by making one UE the Relay UE
and the other UE the Remote UE. The passing condition for the single
test case in ``sl-in-covrg-discovery-1relay-1remote`` is that in both
Model A and Model B discovery modes, the Remote UE can successfully
discover the relay service code broadcasted by the Relay UE.
