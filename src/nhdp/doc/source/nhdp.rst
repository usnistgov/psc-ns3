.. include:: replace.txt
.. highlight:: cpp
.. highlight:: bash

Neighborhood Discovery Protocol
===============================

The |ns3| model for the Neighborhood Discovery Protocol (NHDP), specified
in Internet RFC 6130, is intended to support mobile ad hoc networks (MANETs)
at the IP layer, by discovering one-hop and two-hop neighbor relationships.

NHDP sends and receives HELLO messages.  Each node sends information in
the HELLO about its interfaces and associated IPv4 and IPv6 addresses.
Each node also informs its one-hop neighbors about those neighbors that
it has heard, and whether the neighbor relationship is considered to
be one-way (if a node has heard from a neighbor but doesn't see its
own addresses in the neighbor's HELLO) or symmetric (the node finds its
address in the neighbor's HELLO).   This information sharing allows
each node to locally build and maintain state about its one-hop and
two-hop neighborhood.  HELLO messages are sent periodically, and can
also be extended with messaging from client protocols.

NHDP derives from the Optimized Link State Routing (OLSR) protocol; the
neighbor discovery aspects of OLSR (RFC 3626) were refactored into a
separate protocol entity (NHDP) that provides neighbor discovery service
to next-generation MANET protocols such as AODVv2 and OLSRv2.  NHDP
also uses the generalized MANET packet format known as PacketBB (RFC 5444).
NHDP message encoding uses a PacketBB-defined 'Type-Length-Value (TLV)'
encoding.

The following figure illustrates the relationship between NHDP and a
client protocol such as OLSRv2 (RFC 7181).  NHDP sends and receives HELLO
messages and uses the information gathered, and its local information, to build
three information repositories:  a Link Set, a Neighbor Set, and a 2-Hop
Neighbor set.  It shares this information with OLSRv2.  NHDP also provides
a mechanism for OLSRv2 to add or extract protocol-specific information
from the HELLO messages.  In the case of OLSRv2, it adds TLVs for
selecting Multipoint Relays to the HELLO messages; the NHDP instance
just treats these as opaque TLVs and allows OLSRv2 clients to read
and write them.  The figure also depicts the other information bases
in OLSRv2 and the other OLSRv2 messages that are sent without coordination
from NHDP.  The basis for this figure is the diagram of OLSR data flow from
Wikipedia, authored by Gonsie, available under the Creative Commons
Attribution-Share Alike 3.0 Unported license, downloaded from
https://en.wikipedia.org/wiki/File:Olsr-overview.pdf on 2 March 2025.

.. _fig-nhdp-olsr-architecture:

.. figure:: nhdp-olsr-architecture.*

    NHDP architecture

Scope and Limitations
---------------------

The current |ns3| model supports single interface devices with a single
IPv4 address on the (MANET) interface.
The following NHDP features are described in RFC 6130 but are not yet
included in the |ns3| model:

* IPv6 support
* Support for multiple interfaces (including Interface Information Base) and multiple addresses per node
* More generalized support for carrying messages of other protocols as part of NHDP HELLOs
* Heuristics to allow some HELLOs to be sent without full link information

Usage
-----

NHDP as a standalone application is straightforward to install in the typical |ns3| way
of creating a helper and installing it to a container of nodes.  However, the more interesting
usage will be to install it along with some other client protocol (such as OLSRv2) that can make
use of the neighbor information.  In the example OLSRv2 protocol in this repository, at node
initialization time, the OLSRv2 protocol traverses the list of applications on the node, and
when it finds NHDP, hooks the HELLO send and receive traces, which allows OLSRv2 to piggyback
TLVs (for MPR selectors) on the NHDP HELLOs, as well as to process the neighbor information
in NHDP HELLos.

Helpers
~~~~~~~

The following example code shows how to use the class :cpp:class:`NhdpHelper`:

::

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(nodes);


Attributes
~~~~~~~~~~

Class :cpp:class:`NhdpClient` contains the following attributes:

* ``Address``: Multicast adress to use.
* ``Port``: UDP port to use.
* ``HelloInterval``: Default maximum interval between HELLOs on a MANET interface.
* ``Port``: "UDP port to use.
* ``HelloInterval``: Default maximum interval between HELLOs on a MANET interface.
* ``HelloMinInterval``: Default minimum interval between HELLOs on a MANET interface.
* ``RefreshInterval``: Default maximum interval between advertisements of each 1-hop neighbor in a HELLO.
* ``LHoldTime``: Time to advertise former 1-hop neighbor addresses as lost for removal from Link Set.
* ``HHoldTime``: Time advertised for the validity of messages sent from a MANET interface.
* ``HystAccept``: Link quality threshold at or above which a link becomes usable.
* ``HystReject``: Link quality threshold below which a link becomes unusable.
* ``InitialQuality``: The initial quality of a newly identified link.
* ``InitialPending``: If true, newly identified links are considered pending, and are not usable until quality reaches HystAccept.
* ``HPMaxJitter``: MAXJITTER used in periodically generated HELLO messages.
* ``HTMaxJitter``: MAXJITTER used in externally triggered HELLO messages.
* ``NHoldTime``: Time to advertise former 1-hop neighbor addresses as lost for removal from TwoHopSets.
* ``IHoldTime``: Time to record recently used local interface addresses.

Traces
~~~~~~

Class :cpp:class:`NhdpClient` contains the following trace sources:

* ``NeighborChange``: Notification that neighbor information base changed.
* ``LinkChange``: Notification that link information base changed.
* ``TwoHopChange``: Notification that two-hop information base changed.
* ``HelloSend``: Read-only trace of the contents of a sent HELLO.
* ``HelloRecv``: Read-only trace of the contents of a received HELLO.
* ``HelloMessageSend``: Trace of (modifiable) PbbMessage before it is sent out as HELLO.
* ``HelloMessageRecv``: Trace of PbbMessage HELLO after it has been processed by NHDP.
* ``Tx``: Trace of Packet just before sending to UDP socket.

Examples and Tests
------------------
Two examples are provided.

nhdp-example.cc
~~~~~~~~~~~~~~~
This is a trivial example illustrating how to install and configure NHDP on two Wi-Fi mobile
adhoc nodes, running a small simulation for 10 seconds.  The example is configured to print
debug log messages from the two nodes illustrating the generation and reception of the
HELLO messages.

manet-routing.cc
~~~~~~~~~~~~~~~~

This example program allows one to run ns-3 OLSR or OLSR/NHDP under a random waypoint mobility
model configured for constant speed.

By default, the simulation runs for a startup time and a data collection time (variable,
depending on speed).  The number of nodes is 50.  Nodes move according to
SteadyStateRandomWaypointMobilityModel with a speed of 10 m/s and no pause time within a
800x800 m region.  The WiFi is in ad hoc mode with a MCS 0 rate (802.11ax) and a Friis loss model.
The transmit power is set to 7.5 dBm.

It is possible to change the mobility and density characteristics of the network by directly
modifying the speed, the number of nodes, the transmit power, and/or the bounding box for node
positions.

By default, there are 10 source/sink data pairs sending UDP data at an application rate of
2.048 Kb/s each.  This is typically done at a rate of 4 64-byte packets per second.  Application
data is started at a random time after a warmup of 12 seconds (configurable).

The program outputs a few items:

* packet receptions are notified to stdout such as: <timestamp> <node-id> received one packet from <src-address>
* each second, the data reception statistics are tabulated and output to a comma-separated value (csv) file
* mobility traces of the nodes are printed to 'manet-routing.mob'; this trace can be disabled using a command-line argument
* some tracing and flow monitor configuration that used to work is left commented inline in the program

Validation
----------

There has been no validation against external sources.  However, a test suite is available
in the ``src/test`` directory (named ``nhdp-system-test-suite.cc``) that checks for expected
behavior according to the RFC.

References
----------

[`1 <https://www.ietf.org/rfc/rfc6130.htm>`_] RFC 6130: Mobile Ad Hoc Network (MANET) Neighborhood Discovery Protocol (NHDP)

[`2 <https://www.ietf.org/rfc/rfc3626.htm>`_] RFC 3626: Optimized Link State Routing Protocol (OLSR)

[`3 <https://www.ietf.org/rfc/rfc5444.html>`_] RFC 5444: Generalized Mobile Ad Hoc Network (MANET) Packet/Message Format

[`4 <https://www.ietf.org/rfc/rfc7181.html>`_] RFC 7181: The Optimized Link State Routing Protocol Version 2
