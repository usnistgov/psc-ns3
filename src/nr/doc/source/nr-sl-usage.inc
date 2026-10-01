.. Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
..
.. SPDX-License-Identifier: GPL-2.0-only

.. _NrV2xExamples:

.. _cttcNrV2xDemoSimple:

cttc-nr-v2x-demo-simple.cc
==========================

This is a simplest example describing how to setup an NR sidelink out-of-coverage
simulation using the 3GPP channel model from TR 37.885. This example simulates a
simple topology consist of 2 UEs placed 20 m apart, where UE-1 transmits,
and UE-2 receives.

With the default configuration, the example will create a flow that will
go through a subband or bandwidth part. For that,
specifically, one band with a single CC, and one bandwidth part is used.

The example will print on-screen the average Packet Inter-Reception (PIR)
type 2 computed as defined in TR 37.885. The code to compute the PIR is embedded
in the example, i.e., it does not use `V2xKpi` class to compute PIR. Thus, it
serves as an example to show how one could compute the PIR without using
the V2X KPI management framework. On the other hand, it also demonstrates the 
use of V2X KPI management framework to save MAC and PHY layer
traces in a SQLITE database using ns-3 stats module. Moreover, since there
is only one transmitter in the scenario, sensing is by default not enabled.

.. _nrV2xWestToEastHighway:

nr-v2x-west-to-east-highway.cc
==============================

This example setups an NR Sidelink broadcast out-of-coverage simulation
using the 3GPP V2V highway channel model from TR 37.885. It simulates a
configurable highway topology consisting of type 2 vehicular UEs (see TR 37.885 sec 6.1.2)
in each lane, which travels from west to east. When it comes to the number of
transmitting and receiving vehicular UEs, it allows two configurations:

- To make all the vehicular UEs to transmit and receive during a simulation.

- To make a middle vehicular UE per lane the transmitter, and rest of the
  vehicular UEs the receivers. In this configuration, there could be only odd
  number of vehicles per lane.
 
Note, it does not limit the number of lanes or number of  vehicular UEs per
lane. With the default configuration, it uses one band with a single CC, and
one bandwidth part.

Moreover, this example fully utilizes the V2X KPI management framework and saves
RLC, MAC, PHY layer traces in a SQLITE database using ns-3 stats module. At the
end of the simulation, using the KPI management framework it computes the
following V2X KPIs:

- Average PIR delay for a fixed range of 200 m

- Average PRR for a fixed range of 200 m

- Throughput (by considering all the links)

- Simultaneous PSSCH Tx from MAC

- PSSCH TB corruption

These KPIs are written in the same database where traces are written. This example,
also provides an option to generate GNU plot scripts to plot initial positions
of the vehicles in the scenario, and also to generate a GIF to see the mobility
of the vehicles.

.. _slHarqExample:

sl-harq-example.cc
==================

This is a basic small-scale example allowing users to explore the effect
of changing HARQ modes and configuration on packet delivery ratio and
latency.

By default, a simulation scenario similar to the cttc-nr-v2x-demo-simple.cc
program is created with two UEs configured to use HARQ to support a
unicast session.  One band with a single component carrier, and one
bandwidth part, is used.  Since there
is only one transmitter in the scenario, sensing is by default not enabled.

By default, a groupcast option 2 (positive-negative acknowledgment) HARQ
configuration is used (option 1 is not supported).  For groupcast, three UEs
are used, with one sender and two receivers.  Optionally, a unicast HARQ
configuration can be configured, in which two UEs are used with one sender
and one receiver.  Finally, a broadcast can be used, with one sending UE
and two receiving UEs, and blind retransmissions.  In all three cases, the
use of retransmissions can optionally be disabled.

The transmission process is a constant bit rate application sending
at 16Kbps.  The application starts at time 2 seconds into the simulation.
The bearer is activated at time 1.9 seconds.  The simulation then runs
for sufficient time to send and receive the configured number of
application data packets (default 100).

Delay samples are calculated by the NrSlUeMac reading the timestamp
from the RlcTag attached to each transmitted packet.  The delay
samples are stored in a file 'sidelink-harq-example-delay.dat' with
a format of "Time delay" (units of seconds, resolution of microseconds).
Additionally, the minimum and maximum delay are tracked and output
to the terminal.

The example configures trace sinks to listen to HARQ-related traces
(PSFCH transmissions, HARQ feedback received, and HARQ IDs allocated
and deallocated) and prints those events to stdout.

The example will also output the average Packet Inter-Reception (PIR)
type 2 computed as defined in 3GPP TR 37.885.  The PIR is measured by
tracing the reception process in the receiving (PacketSink) applications.

.. _slMultiLcExample:

sl-multi-lc-example.cc
======================

This example is based on cttc-nr-v2x-demo-simple and simulates a simple
topology consisting of 2 out-of-coverage UEs, where UE-1 transmits, and UE-2
receives.

The purpose of this scenario is to show how different sidelink (SL) traffic
profiles can be configured in the simulation and associated to different
traffic flows. The scheduler then will take into consideration those traffic
profiles and grant resources accordingly during the simulation.

UE-1 has three traffic flows directed towards UE-2. Each traffic flow is
implemented by an OnOffApplication with constant rate that generates CBR
traffic for the duration of the 'trafficTime' simulation parameter. The
packet size and the data rate can be adjusted with the simulation
parameters 'udpPacketSize' and 'dataRate', respectively.
Please note that the traffic profiles described below were selected to
support the default traffic configuration of 200 bytes packets and a data
rate of 16 kbps. Changing those values without adjusting the traffic profile
accordingly may result in poor performance.
All traffic flows are sent to the multicast address, but each of them to a
different port. UE-2 listens to that address and those ports and consumes
the packets with a PacketSink application

Three different traffic profiles are created, one for each traffic flow.
The structure SidelinkInfo contains the traffic profile parameters to be
propagated in the stack. A SidelinkInfo is created for each traffic flow.
A Traffic Flow Template (TFT) is created for each traffic flow associating
the corresponding SidelinkInfo, multicast address and port. Three SL data
radio bearer are created, each configured with one of the above described
TFTs, which will result in the creation of a logical channel per traffic
flow on the stack. The scheduler (NrSlUeMacSchedulerDefault) will grant
resources for each flow according to the traffic profiles.

In this scenario we configure four parameters of the traffic profiles:

- m_dynamic: flag for whether the logical channel scheduling is dynamic scheduling (i.e., per PDU) or Semi-persistent scheduling (SPS)

- m_dstL2Id: L2 id to which the traffic will be sent

- m_priority: priority (the higher the value, the higher the priority of the logical channel)

- m_rri: Resource Reservation Interval for SPS scheduling

We created different configurations for the three traffic profiles. The
table below shows the simulation parameters that control which
configuration is used and the resulting values for the traffic profile
parameters of each traffic flow.

+-------------------------+-----------------------------+
|      Simulation         | Resulting traffic profile   |
|    configuration        | configuration per flow      |
+-----------------+-------+---------+---------+---------+
| Parameter       | Value | Flow 1  | Flow 2  | Flow 3  |
+=================+=======+=========+=========+=========+
| schedTypeConfig |   1   | Dynamic | Dynamic | Dynamic |
+-----------------+-------+---------+---------+---------+
| schedTypeConfig |   2   |   SPS   |   SPS   |   SPS   |
+-----------------+-------+---------+---------+---------+
| schedTypeConfig |   3   | Dynamic | Dynamic |   SPS   |
+-----------------+-------+---------+---------+---------+
| schedTypeConfig |   4   |   SPS   |   SPS   | Dynamic |
+-----------------+-------+---------+---------+---------+
| dstL2IdConfig   |   1   |   254   |   254   |   254   |
+-----------------+-------+---------+---------+---------+
| dstL2IdConfig   |   2   |   255   |   254   |   255   |
+-----------------+-------+---------+---------+---------+
| dstL2IdConfig   |   3   |    2    |   254   |   255   |
+-----------------+-------+---------+---------+---------+
| priorityConfig  |   1   |    1    |    1    |    1    |
+-----------------+-------+---------+---------+---------+
| priorityConfig  |   2   |    1    |    2    |    3    |
+-----------------+-------+---------+---------+---------+
| priorityConfig  |   3   |    2    |    2    |    1    |
+-----------------+-------+---------+---------+---------+
| priorityConfig  |   4   |    1    |    1    |    2    |
+-----------------+-------+---------+---------+---------+
| rriConfig       |   1   |   20    |   20    |   20    |
+-----------------+-------+---------+---------+---------+
| rriConfig       |   2   |   100   |    50   |   100   |
+-----------------+-------+---------+---------+---------+

Three other parameters related to the scheduling that we configure are

1. 'prioToSps' which sets the scheduler attribute 'PriorityToSps' and
   is a flag to give scheduling priority to logical channels that are
   configured with SPS scheduling in case of priority tie
2. 'harqEnabled' which, if enabled, causes the scheduler to add additional
   resources for retransmissions, and
3. 'psfchPeriod' which affects scheduling by imposing requirements on
   the number of slots between retransmission attempts

If 'psfchPeriod' is set to zero, then no PSFCH feedback channel will be
added to the resource pool, and the setting of 'harqEnabled' will instead
cause blind retransmissions to be scheduled

The default configuration is:

.. code-block:: text

  --schedTypeConfig=1 (i.e., all LCs will have dynamic (per-packet) scheduling)
  --dstL2IdConfig=1   (i.e., all LCs send to the same L2 ID of value 255)
  --priorityConfig=1  (i.e., all LCs have the same priority of value 1)
  --rriConfig=1       (i.e., all LCs have the same RRI, which doesn't matter in
                      this configuration as schedTypeConfig=1)
  --prioToSps=false   (i.e., no priority to SPS, which doesn't matter in this
                      configuration as schedTypeConfig=1)
  --harqEnabled=true  (retransmissions slots will be scheduled)
  --psfchPeriod=4     (retransmissions slots will be scheduled)

The selection of dstL2Id value configures the following:

- If the dstL2Id is 255, the CastType will be Broadcast

- If the dstL2Id is 254, the CastType will be Groupcast

- If the dstL2Id is 2, the CastType will be Unicast

The example will print on-screen the number of transmitted and received
packets during the simulation and the average packet delay.
