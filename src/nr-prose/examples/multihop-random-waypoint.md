# Objective

The objective of this example is to provide a fully working L3 U2U sidelink
relay network with all nodes serving as relays, with nodes moving in a 2D
random walk pattern, and generating various traces and plots of the operation
of different protocol layers.

# Options

- the number of nodes
- the x-axis range for the bounding box
- the y-axis range for the bounding box
- the loss model type (friis)
- the error model type (static or epa)
- the node speed
- the simulation time
- whether to enable PCAP
- whether to enable selected logs

# System configuration

Sidelink will use one operational band, containing one component carrier,
and a single bandwidth part centered at the frequency specified by the
corresponding input parameter. The system bandwidth, the numerology to
be used and the transmission power can be configured as well.  However, this
is mainly an L3 U2U demonstration so the physical layer configuration does
not matter so much.

# Topology

Node 0 is created and unused, so that nodes 1..N can have a node ID that
aligns with their IP address and L2 ID.  All UEs are at 1.5 m height.

A steady state random waypoint model is used for mobility within the
specified bounding box and with the configured node speed.

# Identifiers

IP addresses align with layer-2 IDs such as follows:

- UE1:  IP = 8.0.0.1, L2ID = 1
- UE2:  IP = 8.0.0.2, L2ID = 2
- UE3:  IP = 8.0.0.3, L2ID = 3
- UE4:  IP = 8.0.0.4, L2ID = 4

# ProSe Unicast

A ProSe direct link is formed according to the possible links, as relays are discovered by ProSe Discovery.

# Traffic

One constant bit rate UDP flow from UE1 to the last UE (e.g., UE4)
- this is configured for a SPS-scheduled bearer

# Output

The example produces PCAP output files for each node
- multihop-random-waypoint-n*-i1.pcap

default-multihop-random-waypoint-single-link.db: contains MAC and PHY layer traces in a sqlite3 database created using ns-3 stats module.

NrSlPc5SignallingPacketTrace.txt: log of the transmitted and received PC5 signaling messages used for the establishment of the ProSe unicast direct link.

- NrSlDiscoveryTrace.txt
- NrSlPc5SignallingPacketTrace.txt
- multihop-random-waypoint.routes
