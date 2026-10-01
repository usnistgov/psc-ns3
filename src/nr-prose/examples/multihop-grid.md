# Objective

The objective of this example is to provide a fully working L3 U2U sidelink
relay network with all nodes serving as relays, with nodes positioned
on a 2D grid, and generating various traces and plots of the operation
of different protocol layers.

# Options

- the grid size is variable (4: 2x2, 9: 3x3, 16: 4x4)
- four options for propagation model:
    - MatrixPropagationModel that permits high SNR links along grid lines with nearest neighbors
    - Friis free space propagation loss model
    - Winner II LOS (log distance model) free space propagation loss model
    - Winner B+ LOS (log distance model) Urban Microcell (UMi) propagation loss model
- the grid distance between neighboring nodes is configurable
    - when the propagation model is LogDistance, the range of every link depends on the configurable distance (ranging from matrix-like to all nodes within one hop of one another)
- two options for error model:
    - static (AWGN)
    - epa (Extended Pedestrian A)

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
For example, with 'd' as the x/y distance between nodes:

         UE3...................UE4
     (0, d, 1.5)          (d, d, 1.5)
         |                     |
         |                     |
         UE1...................UE2
     (0, 0, 1.5)          (d, 0, 1.5)

# Identifiers

In the case of the 2x2 grid:

- UE1:  IP = 8.0.0.1, L2ID = 1
- UE2:  IP = 8.0.0.2, L2ID = 2
- UE3:  IP = 8.0.0.3, L2ID = 3
- UE4:  IP = 8.0.0.4, L2ID = 4

# ProSe Unicast

A ProSe direct link is formed according to the possible links, as relays are discovered by ProSe Discovery.

# Traffic

One constant bit rate UDP flow from UE1 to the last UE (e.g., UE4)
- this is configured for a SPS-scheduled bearer

One TCP flow from UE1 to the last UE (e.g., UE4)
- this is configured for a dynamically-scheduled bearer

# Output

The example produces PCAP output files for each node
- multihop-grid-n*-i1.pcap

default-multihop-grid-single-link.db: contains MAC and PHY layer traces in a sqlite3 database created using ns-3 stats module.

NrSlPc5SignallingPacketTrace.txt: log of the transmitted and received PC5 signaling messages used for the establishment of the ProSe unicast direct link.

- NrSlDiscoveryTrace.txt
- NrSlPc5SignallingPacketTrace.txt
- multihop-grid.routes
