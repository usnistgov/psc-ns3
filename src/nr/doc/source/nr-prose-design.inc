This section describes the 5G New Radio (NR) Proximity Services (ProSe) ns-3
model, which is an extension of the 5G NR V2X ns-3 model described in
[cttc-nr-v2x]_ and documented in previous sections.

Overview
========

5G NR Proximity Services (ProSe) is the term used by 3GPP to define a set of
services that use direct transmissions between UEs in close proximity on a 5G
NR system.

At the time of writing, three main ProSe functionalities were defined to enable
those services in 3GPP Release 17 [TS23304-v17]_:

* 5G NR ProSe Direct Discovery, including direct, group member, and relay UE discovery,
* 5G NR ProSe Direct Communication, including unicast, broadcast, and groupcast communication, and
* 5G NR ProSe UE-to-Network (U2N) relay, including Layer 2 and Layer 3 architectures.

Those functionalities use the NR sidelink (SL) to communicate. The NR SL was
defined in 3GPP Release 16 and comprises a set of physical channels and
protocol stack that allow UEs of a 5G NR system to communicate directly between
them without routing the data through the network infrastructure. The first
services defined to use the NR SL were the NR Vehicle-to-everything (V2X)
services in Release 16, particularly the V2X communication over PC5
functionality.

As described in previous sections, a Release 16 NR V2X ns-3 model was
developed, which includes a comprehensive NR SL implementation together with
the necessary procedures for out-of-coverage scenarios with broadcast V2X
communications [cttc-nr-v2x]_.

We extended the NR V2X ns-3 model to support Release 17 NR ProSe
functionalities over their NR SL implementation. Please refer to NR V2X sections
for details on the NR SL features implemented in their model.

Following NR V2X implementation design, our extensions expands over two ns-3
modules:

* lte: includes the extensions made on layers above the MAC layer, including
  the new ProSe layer and the different entities to support its operation.
* nr: includes extensions made on the MAC layer and below, a new helper to
  support ProSe configuration, and a set of  example scenarios of the different
  implemented functionalities.

Scope and Limitations
=====================

Currently we support the following NR ProSe functionalities:

* Direct and U2N relay ProSe discovery and the following components:

  * SL PC5 discovery only, which means that discovery procedures over the PC3a
    reference point (with the ProSe function in the core network) are not
    implemented and that UEs are assumed to be authorized to perform PC5
    discovery and are provided with any needed parameters (e.g., ProSe
    Application Code, Relay Service Code, filters, etc.) to use during the PC5
    discovery process (in the scenario).
  * Both discovery models:

    * Model A: a broadcast of discovery messages.
    * Model B: a set of discovery messages based on a request/response exchange.

  * Direct peer discovery between two UEs, configurable in the scenario through
    shared ProSe Application Code, destination Layer 2 ID (L2 ID), and which
    discovery model to consider.
  * UE-to-Network relay discovery between a relay and a remote using either
    discovery model.
  * Group member discovery which is not fully implemented but can be mimicked
    if one Destination L2 ID is considered for a group of UEs.
  * Dedicated discovery bearers with neither pre-established priority nor
    Quality of Service (QoS) support.

* Unicast mode ProSe direct communication, including the following scope and
  limitations:

  * Direct link establishment between a pair of UEs, which is configured from
    the scenario.
  * Basic SL bearer setup to have traffic flowing between the two UEs upon
    connection, using NR V2X framework, i.e., all user traffic has the same
    priority and no QoS management is supported on the SL at the moment.
  * Direct link maintenance and modification procedures are not implemented,
    i.e., the direct link logical entities in the UEs are setup upon connection
    and do not change depending on scenario evolution, e.g., if the UEs are not
    in proximity anymore.
  * Only one direct link per pair of UEs is supported, but a UE can have
    multiple direct links with different UEs in the scenario.

* Layer 3 (L3) U2N relay architecture, including the following scope and
  limitations:

  * L3 U2N relay connection establishment between a pair of UEs, which is
    configured statically from the scenario.
  * Unicast mode ProSe direct communication implementation as described above
    is used for the connection between remote and relay UEs.
  * Bearer reconfiguration for traffic redirection upon relay connection
    supports only the reconfiguration of one network radio bearer for a remote
    UE, which should be configured in the scenario. The traffic redirection is
    done only by remote address at the moment.
  * Relay UEs support only one network radio bearer for relaying, i.e., the
    traffic from all the remote UEs connected to a relay UE goes through the
    same bearer, which is configured in the scenario.
  * Relay UEs support both in-network communication (UL and DL) with a gNB, and
    out-of-network SL communication with the remote UEs. Both types of
    communication are currently supported simultaneously within the same UE
    only if they use different bandwidth parts of the spectrum.


Architecture
============

UE architectures
================
The NR V2X UE control plane architecture was extended with a new SL service
layer on top of the protocol stack, which is connected to the NAS and RRC
layers. This SL service layer (class NrSlUeService) and its Service Access
Points (SAPs) (classes NrSlUeSvcNasSapUser, NrSlUeSvcNasSapProvider,
NrSlUeSvcRrcSapUser, and NrSlUeSvcRrcSapProvider) are meant to be implemented
by the services using the NR SL. The NrUeNetDevice class now holds an
NrSlUeService pointer which should be set upon initial configuration.
In the case of the ProSe services, this is done in the NrSlProseHelper used by
the user to configure the ProSe functionalities in the scenario.
Figure :ref:`prose-arch-ue-ctrl-plane` shows the resulting UE control plane
architecture when the service layer is the ProSe layer, implemented by the
class NrSlUeProse colored in purple, and the corresponding new SAPs colored in
pink. Figure :ref:`prose-arch-ue-ctrl-plane` also shows the SL Signaling Radio
Bearers (SL-SRBs) implemented to support the ProSe services. The UE data plane
architecture remains the same as the NR V2X model and can be seen in


.. _prose-arch-ue-ctrl-plane:

.. figure:: figures/prose-arch-ue-ctrl-plane.*
   :align: center
   :scale: 60%

   NR ProSe - UE architecture - Control Plane


.. _prose-arch-ue-data-plane:

.. figure:: figures/prose-arch-ue-data-plane.*
   :align: center
   :scale: 45%

   NR ProSe - UE architecture - Data plane


NR UE PROSE
===========

The ProSe layer is implemented in class NrSlUeProse, which holds the attributes,
entities, and procedures of the ProSe functionalities the model supports.
In the following sections we describe each functionality and the parts that are
supported in the model.

5G ProSe Discovery
##################

The 5G ProSe discovery is a function employed by a 5G ProSe-enabled UE to
discover other 5G ProSe-enabled UEs in its vicinity using direct NR radio
transmissions between the two UEs [TS23304-v17]_. 5G ProSe Direct
Discovery can be a standalone service or can be used for subsequent actions,
e.g., to detect if a given UE is nearby and initiate 5G ProSe Direct
Communication.

The two discovery models defined in the 3GPP standards are illustrated in
Figure :ref:`prose-disc-modelA` and Figure :ref:`prose-disc-modelB` and are
described in the following. Both models are supported in our implementation.

* Model A: It uses a single discovery protocol message (Announcement).
  The UE sending the ProSe PC5 DISCOVERY message is called the "announcing
  UE" and the "monitoring UE" is the UE that triggers the lower layer to
  start monitoring for ProSe PC5 DISCOVERY message.
* Model B: It uses two discovery protocol messages (Solicitation and Response).
  The UE sending the first PROSE PC5 DISCOVERY message is called the
  "Discoverer UE" and the other UE is called the "Discoveree UE".

  .. _prose-disc-modelA:

  .. figure:: figures/prose-disc-modelA.*
     :align: center
     :scale: 60%

     NR ProSe - Discovery - Model A

  .. _prose-disc-modelB:

  .. figure:: figures/prose-disc-modelB.*
     :align: center
     :scale: 45%

     NR ProSe - Discovery - Model B




The 5G ProSe discovery is supported over the network using the PC3a reference
point with the ProSe function in the core network, and locally using the SL
over the PC5 reference point. The model supports SL/PC5 discovery only.

PC5 discovery supports the initiation and completion of the following PC5
procedures for both models A and B [TS24554-v17]_:

* Direct discovery: to enable a ProSe-enabled UE to detect and identify another
  ProSe-enabled UE over PC5 interface.
* Group member discovery: to enable a ProSe-enabled UE to detect and identify
  another ProSe-enabled UE that belongs to the same application layer group
  (e.g., sharing the same application layer group ID) over PC5 interface.
* UE-to-network relay discovery: to enable a ProSe-enabled UE to detect and
  identify another ProSe-enabled UE over PC5 interface for UE-to-network relay
  communication between a UE and the network.

The model currently supports direct and U2N relay discovery. Group member
discovery is not fully supported but can be simulated using one Destination
L2 ID for a group of UEs.

The Sidelink Signaling Radio Bearer (SL-SRB) named SL-SRB4 is used to transmit
and receive the NR sidelink discovery messages. Its parameters are fixed and
defined as SCCH configuration in [TS38331-v17]_.


In the NrSlUeProse class, a UE is able to add and/or remove NR discovery
applications and their associated ProSe Application code or Relay Service Code,
destination L2 ID, and role (announcing, monitoring, discoveree, discoverer,
relay, remote) to and/or from a discovery map storing this information.

On the transmission side, direct discovery and relay discovery messages are
created and sent. Besides, the UE tells the RRC to inform the MAC to monitor
its own L2 ID along with other UEs' L2 IDs.
On the reception side, when a discovery message is received, the UE processes
it to check if it is interested in this specific ProSe Application code/Relay
Service Code or not (based on the pre-established discovery map) and whether a
response is needed or not (depending on the discovery model considered).
Each time a discovery message is sent or received, a corresponding trace is
called in order to record the current timestamp, sender and receiver L2 IDs,
discovery type, discovery model, and message content.

Unlike LTE, there is no periodicity associated with the transmission of
discovery messages. In order to create such recurrence, a discovery interval
was added and defined in seconds in this class. The default value is set to
1 second, but it can be modified in the scenario. For example, a discovery
interval set to 4 seconds means that a discovery message is sent every
4 seconds since the starting of the discovery process until the termination of
the discovery or the end of the simulation, independently of whether the
previous messages are received or not.


5G ProSe direct communication
#############################

5G NR ProSe direct communication defines one-to-one and one-to-many direct
traffic exchange between nearby UEs using the NR SL.
This functionality is supported when the UE is in-network (i.e., connected to a
gNB) as well as when it is out-of-network.
Radio resources for ProSe direct communication can be scheduled by the network
(in-network case only), referred to as Mode 1, or by the UE itself using
pre-configured parameters (both, in-network and out-of-network cases), referred
to as Mode 2. Currently, the model only supports Mode 2.
The following data unit types are supported in the standard: IPv4, IPv6,
Ethernet, Unstructured, and Address Resolution Protocol. The model currently
only supports IPv4.

ProSe direct communication supports the three types of transmission modes over
the NR SL: broadcast, groupcast, and unicast. The main characteristics of each
mode, described in [TS38300-v17]_, can be seen in
Table :ref:`dir-com-modes`.



.. _dir-com-modes:

.. table:: NR SL transmission modes
   :class: table

   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | **Functionality**                                                           | **Unicast**           | **Groupcast**         | **Broadcast**         |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Transmission and reception of user traffic over the Sidelink                | Between two peer UEs  | Between UEs belonging | Between UEs           |
   |                                                                             |                       | to a group            |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Transmission and reception of control information over the Sidelink         | Between two peer UEs  |                       |                       |
   |                                                                             | (PC5-S, PC5-RRC)      |                       |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Support of sidelink HARQ feedback                                           | Yes                   | Yes                   |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Support of sidelink transmit power control                                  | Yes                   |                       |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Support of RLC AM                                                           | Yes                   |                       |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Support of one PC5-RRC connection                                           | Between two peer UEs  |                       |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+
   | Detection of radio link failure for PC5-RRC connection                      | Yes                   |                       |                       |
   +-----------------------------------------------------------------------------+-----------------------+-----------------------+-----------------------+

The NR V2X model supported the broadcast mode and we added initial support for
the unicast mode, covering the transmission and reception of user traffic and
control information over the SL between peer UEs. In the following we describe
the corresponding implementation.

**Unicast mode 5G ProSe direct communication:**

To perform unicast communication, a pair of UEs need to establish a connection.
This connection is called a PC5 unicast link, a Layer-2 link [TS23304-v17]_
or a 5G ProSe direct link [TS24554-v17]_ in the different standard
documents that address its definition and procedures. We will use *direct link*
in this document.
A direct link can be UE-oriented, where the UE establishes a connection with
another known UE, or service-oriented, where multiple UEs may provide the
required service, resulting in several direct links established with the
initiating UE. Currently, the model only supports UE-oriented direct links.
The standard supports that a pair of UEs can establish multiple simultaneous
direct links with each other to satisfy their application(s) requirements.
The model currently supports only one direct link per pair of UEs, but a UE can
have multiple direct links, each one with a different UE.

The NrSlUeProse class provides a function to add a direct link connection in
the UE where it is installed, which creates the corresponding instance of the
direct link and keeps track of its status and the context it belongs to
(unicast only connection, or connection for U2N relay). Each UE in the direct
link should have a direct link instance.

In the model, a direct link is implemented by the class NrSlUeProseDirectLink
which interacts with the NrSlUeProse class using the NrSlUeProseDirLnkSapUser
and NrSlUeProseDirLnkSapProvider SAPs, as shown in Figure :ref:`prose-arch-dir-link`


.. _prose-arch-dir-link:

.. figure:: figures/prose-arch-dir-link.*
   :align: center
   :scale: 45%

   NR ProSe - UE Architecture - Control Plane - Direct Link




The NrSlUeProseDirectLink instance keeps track of the information of the UEs in
the direct link as well as other parameters used by the ProSe direct link
procedures.
The logic of the supported ProSe direct link procedures is implemented on the
NrSlUeProseDirectLink as well, which includes transmission and reception of PC5
Signaling (PC5-S) messages.

The NrSlUeProse instance uses the NrSlUeProseDirLnkSapProvider of the
corresponding NrSlUeProseDirectLink instance to deliver received PC5-S
messages, and the NrSlUeProseDirectLink instance process them depending on the
ongoing procedure(s).
The NrSlUeProseDirectLink instance uses the corresponding
NrSlUeProseDirLnkSapUser to instruct the NrSlUeProse instance to send PC5-S
messages to a given destination and also to notify of any direct link state
changes.
We extended the lower layers to support SL-SRBs as we describe in the following
sections, and the NrSlUeProse class is in charge of instructing the creation
and configuration of the appropriate SL-SRB and the transmission of the PC5-S
messages on them upon reception from an NrSlUeProseDirectLink instance.


The 3GPP standard defines procedures to establish, modify, maintain, and
release a direct link between two UEs ([TS24554-v17]_, Section 7.2).
We prioritized the system level performance evaluation of an established link,
and currently the only implemented procedure is the ProSe direct link
establishment procedure. This also means that once a direct link is established
it remains so until the end of the simulation.


The UE-oriented direct link establishment procedure can be seen in
Figure :ref:`prose-comm-dir-link-establishment`. The initiating UE sends a
PROSE DIRECT LINK ESTABLISHMENT REQUEST (PDL-Es-Rq) message to the target UE
and starts the retransmission timer T5080. The target UE receives the PDL-Es-Rq
message and if a direct link can be established, it sends a
PROSE DIRECT LINK ESTABLISHMENT ACCEPT (PDL-Es-Ac) message back to the
initiating UE. Otherwise, it sends a PROSE DIRECT LINK ESTABLISHMENT REJECT
(PDL-Es-Rj) message.
Upon reception of the PDL-Es-Ac, the Initiating UE stops T5080 timer and the
link is considered established. Upon expiration of the T5080 timer the
initiating UE retransmits the PDL-Es-Rq message.
These messages are PC5-S messages which are defined on [TS24554-v17]_
and transmitted over SL signaling radio bearers (SL-SRBs) as listed in
Table :ref:`prose-comm-sl-srbs`.
The establishment messages structure are implemented as headers in the model by
the classes ProseDirectLinkEstablishmentRequest,
ProseDirectLinkEstablishmentAccept, and ProseDirectLinkEstablishmentReject.
A trace source was defined in the NrSlUeProse class that is fired upon
transmission and reception of PC5-S messages and exports information about the
source, destination, and the message packet itself.
The standard defines security procedures to be executed during the
establishment procedure, which involve the exchange of PC5-S security messages.
These security procedures are not implemented in the model, and are assumed to
be successful during the establishment procedure.


.. _prose-comm-dir-link-establishment:

.. figure:: figures/prose-comm-dir-link-establishment.*
   :align: center
   :scale: 65%

   NR ProSe - UE-oriented direct link establishment procedure


.. _prose-comm-sl-srbs:

.. table:: Configuration of SL bearers used for ProSe direct communication.
  :class: table

  +---------------+---------+--------+--------------+---------------------+------------------------------+-----------------------------------------+
  | **Channel**   | **LCG** | **LC** | **Priority** | **Sidelink bearer** | **Purpose**                  | **Establishment PC5-S message**         |
  +---------------+---------+--------+--------------+---------------------+------------------------------+-----------------------------------------+
  |               |         | 0      |              | SL-SRB0             | Unprotected PC5-S messages   | PDL-Es-Rq                               |
  +               +         +--------+              +---------------------+------------------------------+-----------------------------------------+
  |               |         | 1      |              | SL-SRB1             | Security PC5-S messages      |                                         |
  + SCCH          +    0    +--------+      1       +---------------------+------------------------------+-----------------------------------------+
  |               |         | 2      |              | SL-SRB1             | Protected PC5-S messages     |   PDL-Es-Ac, PDL-Es-Rj                  |
  +               +         +--------+              +---------------------+------------------------------+-----------------------------------------+
  |               |         | 3      |              | SL-SRB1             | PC5-RRC messages             |                                         |
  +---------------+---------+--------+--------------+---------------------+------------------------------+-----------------------------------------+
  | STCH          |         | 5-N    |              | SL-SRB0             | Traffic                      |                                         |
  +---------------+---------+--------+--------------+---------------------+------------------------------+-----------------------------------------+


When a direct link is added on the NrSlUeProse class, the corresponding method
has a flag to indicate if the UE is an initiating UE. When this flag is set to
true, the UE will assume the role of initiating UE and start the direct link
establishment procedure after the configuration of the NrSlUeProseDirectLink
instance. If the flag is set to false, the NrSlUeProseDirectLink instance will
be created, but the UE assumes the role of target UE and will only react to the
reception of the PDL-Es-Rq message from the initiating UE.

Once the procedure ends successfully and the direct link state change to
"ESTABLISHED" in the UE, the NrSlUeProse instructs the lower layers to activate
the corresponding SL Data Radio Bearer (SL-DRB) and Traffic Flow Templates
(TFTs) to exchange user traffic with the peer UE in the direct link. The
standard defines procedures to setup IP addressing and configuration within the
direct link. Currently, the model uses a simplified IPv4 configuration for the
UEs in the direct link in which the IPv4 addresses of the UEs are exchanged in
the PC5-S messages of the establishment procedure using the Ipv4AddrTag class.

5G ProSe UE-to-network relay
############################

The 5G ProSe UE-to-network (U2N) relay functionality enables an in-network UE
(U2N relay UE) to extend the network connectivity to another nearby UE (remote
UE) by using ProSe direct communication over the NR SL.
Two U2N relay architectures are defined in the 3GPP standard [TS23304-v17]_.
The first is the Layer 3 (L3) architecture in which the relay of data packets
in the SL is performed at the network layer, and remote UEs connected to an L3
U2N relay are transparent to the network.
The second is the Layer 2 (L2) architecture in which the relaying in the SL
occurs within the L2, in a newly defined adaptation sublayer over the RLC
sublayer. A remote UE connected to an L2 U2N relay is seen by the network as
a regular UE (i.e., as if it was directly connected to the network), which
gives the network control of the connection and services.
In the model, we currently support the L3 U2N relay architecture only, which
can be seen in Figure :ref:`prose-l3-rleay-arch`

.. _prose-l3-rleay-arch:

.. figure:: figures/prose-l3-rleay-arch.*
   :align: center
   :scale: 80%

   NR ProSe - Layer 3 UE-to-Network Relay Architecture


A remote UE seeking to access the network using an L3 U2N relay UE first uses
U2N relay discovery to detect U2N Relay UEs in the vicinity, selects the most
suitable one, and then starts a UE-oriented direct link establishment with it.
Once the direct link is established, further configuration in the relay UE and
in the network is performed to setup the packet relaying.
Currently, to support L3 U2N relay simulations, we skip the U2N relay discovery
and selection and we directly establish an L3 U2N relay connection between two
specific UEs in the simulation scenario.
To do this, we directly create an extended version of a direct link connection
between two UEs (the relay UE and the remote UE). Please note that the relay UE
should be associated to a gNB and connected to the network in the simulation.

The NrSlUeProse class provides a function to add a direct link connection in
the UE, which has two parameters related to U2N relay connection. The first one
is a flag indicating if the direct link connection is actually for a U2N relay
connection. When the flag is true, both the NrSlUeProse and the
NrSlUeProseDirectLink keep this information in their contexts and react
accordingly during the direct link procedures. The second U2N relay related
parameter is the Relay Service Code, which is an identifier of the relay
service the relay UE provides and the remote UE is interested in.
In the U2N relay direct link establishment procedure, the remote UE takes the
role of initiating UE and the relay UE is the target UE, and they should be
configured this way when adding the direct link connection to each UE.

When the direct link is for relaying, the NrSlUeProse instance performs two
extra steps once the establishment procedure ends successfully. First, it
instructs the Evolved Packet Core (EPC) helper to configure the
\EpcPgwApplication to route the packets directed to the remote UE towards the
relay UE. Second, it instructs the NAS to (re)configure the UL and SL data
bearers to have the data packets flowing in the appropriate path depending on
the role of the UE (relay UE or remote UE).


LTE/EPC UE NAS
==============

EpcUeNas class was updated with the SAP provider functions for its interface
with the sidelink service layer (e.g., the NrSlUeProse class), including the
activation of SL-DRBs and (re)configuration of the data bearers (UL and SL
where it applies) upon a U2N relay  connection. The implementation also
includes a function that moves the received packet through the correct path
when the UE is acting as an L3 U2N relay UE, and a trace source that exposes
the packet information and the corresponding path.

LTE UE RRC
==========

The LteUeRrc was extended with the SAP provider functions for its interface
with the sidelink service layer (e.g., the NrSlUeProse class). The services
LteUeRrc provides to the service layer include the addition and activation
of sidelink signaling radio bearers (SL-SRB0 to SL-SRB3 used for Unicast mode
5G ProSe direct communication signaling messages, and SL-SRB4 used for
discovery messages) and the transmission of the corresponding messages to lower
layers. It also includes the implementation of two methods that instruct lower
layers to monitor messages directed to a specified L2 ID and the UE's own L2 ID.
The SL-SRBs information is stored on instances of the
NrSlSignallingRadioBearerInfo or NrSlDiscoveryRadioBearerInfo, including the
logical channel configuration and the PDCP and RLC stacks (LtePdcp and LteRlc
respectively), and the connection with lower layers is done similarly to
SL-DRBs by using the corresponding SAPs with the NrSlBwpManagerUe and NrUeMac
classes.
Signaling and discovery messages received from lower layers are passed to the
service layer using the corresponding SAP.


NR SL UE RRC
============

The class NrSlUeRrcSapUser (i.e., interface exported by the NrSlUeRrc and
called by the LteUeRrc) was extended with the definition of the methods to add
and retrieve transmission and reception SL-SRBs for both direct communication
and direct discovery.
These methods were further implemented in the NrSlUeRrc class which stores the
instances of the  NrSlSignallingRadioBearerInfo or NrSlDiscoveryRadioBearerInfo
created in the LteUeRrc for each SL-SRB.


NR UE BWP MANAGER
=================

The NrSlBwpManagerUe class is in charge of storing radio bearers and logical
channels information and of multiplexing them to/from the appropriate MAC
entities. This class was extended with functions to add SL-SRB's logical
channels to transport signaling messages needed for unicast direct
communication procedures and discovery messages. NrSlUeBwpmRrcSapProvider
class was extended with the interface definition that allows LteUeRrc to use
those methods.


NR UE MAC
=========

NrUeMac was enhanced to exclude non monitored slots from the transmission
opportunities used for resource selection when sensing-based resource selection
is used.
We also extended the NrSlUeMacSchedulerSimple scheduler, which is used for the
selection of SL resources within the resource selection window in the
sensing-based resource selection. The scheduler now can select resources for
multiple logical channels if they have packets to transmit.
The logical channels are served by priority order and by creation order within
the same priority.

LTE PGW
=======

EpcPgwApplication was extended with a method to register a remote UE upon
connection to a U2N  relay UE, and with the logic to route the packets directed
to the remote UE towards the corresponding U2N relay UE.


LTE EPC HELPER
==============

The EpcHelper class was extended with the definition of a method to inform the
PGW/SGW of a new remote UE connected to a U2N relay UE. This method is called
by the NrSlUeProse when configuring SL-DRBs for relay communication upon a
remote UE connection to a U2N relay UE. The method was implemented in the
NoBackhaulEpcHelper class which inherits from EpcHelper and is the base class
for the NrPointToPointEpcHelper used in the scenarios. NoBackhaulEpcHelper uses
an instance of the EpcPgwApplication class with the extension mentioned above
to achieve the objective.

NR PROSE HELPER
===============

The NrSlProseHelper was created to assist the users with the configuration of
the ProSe functionalities in the simulation scenarios.
It offers a method to prepare the UEs for the ProSe functionalities which
internally installs the NrSlUeProse layer on each device and connects the
corresponding SAPs to the rest of the stack. The NrSlProseHelper also offers
different methods depending on the functionality the user wants to use:

**5G ProSe Discovery:**

In the NrSlProseHelper class, direct and relay discovery can be started or
stopped for a specific UE. The related functions from the NrSlUeProse class
are called to complete the process.

**Unicast mode 5G ProSe direct communication:**
To setup a simulation with 5G ProSe unicast direct communication, two methods
of the NrSlProseHelper are used. The first is a method to prepare UEs for
unicast, which configures the NrSlUeProse instances accordingly. The second
method is used from the scenario to establish a direct link between two given
UEs at a given simulation time. The method takes care of the configuration and
sets the direct link connection in both of the UEs, NrSlUeProse instances with
their corresponding roles at the given simulation time.

**5G ProSe L3 UE-to-network relay:**
For the L3 U2N relay, the NrSlProseHelper also offers two methods to configure
the functionality. The first one is used to install the configuration on the UEs
that will act as L3 U2N relay UEs. This method activates the Evolved Packet
System (EPS) bearer to be used for relaying traffic on each relay UE device,
adds the corresponding configuration to the NrSlUeProse instances, and
configures the EpcHelper to be used during the simulation.
The other method is used to establish a 5G ProSe L3 U2N relay connection
between two given UEs (a remote UE and a relay UE) at a given simulation time.
This method configures the NrSlUeProse instances and schedules the creation of
the corresponding direct link instances in both UEs participating in the U2N
relay connection (Remote UE is the initiating UE of the direct link and relay
UE is the target UE) for the corresponding simulation time.




.. [TS23304-v17] 3GPP TS 23.304, Technical Specification Group Services and System Aspects; Proximity based Services (ProSe) in the 5G System (5GS) (Release 17), v17.2.2, Mar. 2022.
.. [TS24554-v17] 3GPP TS 24.554, Technical Specification Group Core Network and Terminals; Proximity-services (ProSe) in 5G System (5GS) protocol aspects; Stage 3, v17.0.0, Mar. 2022.
.. [TS38331-v17] 3GPP TS 38.331, Technical Specification Group Radio Access Network; NR; Radio Resource Control (RRC) protocol specification, v17.0.0, Mar. 2022.
.. [TS38300-v17] 3GPP TS 38.300, Technical Specification Group Services and System Aspects; NR; NR and NG-RAN Overall Description; Stage 2 (Release 17), v17.0.0, Mar. 2022.

