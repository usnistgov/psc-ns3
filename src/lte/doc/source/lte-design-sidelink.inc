++++++++++++
LTE Sidelink
++++++++++++

--------
Overview
--------

This section describes the |ns3| support for LTE Device to Device
(D2D) communication based on the model published in [NIST2016]_
[NIST2017]_ and extended to support UE-to-Network Relay operations [NIST2019]_.


3GPP introduced D2D Proximity Services (ProSe) in release 12 to allow ProSe
enabled devices to exchange information directly, i.e., without traversing the
eNB. Figure :ref:`fig-prose-non-roaming-arch` shows new ProSe entities and the
new interfaces added to the network [TS23303]_.

.. _fig-prose-non-roaming-arch:

.. figure:: figures/prose-non-roaming-arch.*
   :scale: 50 %
   :align: center

   Non-Roaming Reference Architecture

At the time of writing this documentation only the new radio interface, i.e.,
PC5 is implemented. This interface is also known as Sidelink at physical layer.
The model supports all the three following LTE D2D functionalities defined under
ProSe services:

 #. Direct communication
 #. Direct discovery
 #. Synchronization

These LTE D2D functionalities can operate regardless of the network status of
the UEs. Thus, four scenarios were identified by 3GPP [TR36843]_:

 1A. Out-of-Coverage

 1B. Partial-Coverage

 1C. In-Coverage-Single-Cell

 1D. In-Coverage-Multi-Cell

At this stage, the model has been tested for the four scenarios. The Table
:ref:`tab-ns-3-LTE-Sidelink-supported-scenarios` gives more information about
the support of these four scenarios for all the three ProSe services.

.. tabularcolumns:: |p{0.4cm}|p{1.58cm}|p{1.2cm}|p{1.2cm}|p{1.9cm}|p{1.8cm}|p{2.3cm}|p{3cm}|

.. _tab-ns-3-LTE-Sidelink-supported-scenarios:

.. table:: ns-3 LTE Sidelink supported/tested scenarios

   +----+------------------+-----------------+-----------------+---------------+-------------+-----------------+-----------------------------------------------+
   |  # | Description      | UE A            | UE B            | Direct        | Direct      | Synchronization |                   Example                     |
   |    |                  |                 |                 | Communication | Discovery   |                 |                                               |
   +====+==================+=================+=================+===============+=============+=================+===============================================+
   | 1A | Out-of-Coverage  | Out-of-Coverage | Out-of-Coverage | Yes           | Yes         | Yes             | .. image:: figures/sidelink-scenarios-1a.*    |
   |    |                  |                 |                 |               |             |                 |    :scale: 30 %                               |
   |    |                  |                 |                 | RA = Mode 2   | RA = Type 1 | Autonomous      |                                               |
   |    |                  |                 |                 |               |             | synchronization |                                               |
   +----+------------------+-----------------+-----------------+---------------+-------------+-----------------+-----------------------------------------------+
   | 1B | Partial Coverage | In-Coverage     | Out-of-Coverage | Yes           | Yes         | Yes             | .. image:: figures/sidelink-scenarios-1b.*    |
   |    | Single Cell      |                 |                 |               |             |                 |    :scale: 30 %                               |
   |    |                  |                 |                 | RA = Mode 2   | RA = Type 1 | Network and     |                                               |
   |    |                  |                 |                 |               |             | autonomous      |                                               |
   |    |                  |                 |                 |               |             | synchronization |                                               |
   +----+------------------+-----------------+-----------------+---------------+-------------+-----------------+-----------------------------------------------+
   | 1C | In-Coverage      | In-Coverage     | In-Coverage     | Yes           | Yes         | Yes             | .. image:: figures/sidelink-scenarios-1c.*    |
   |    | Single Cell      |                 |                 |               |             |                 |    :scale: 30 %                               |
   |    |                  |                 |                 | RA = Mode 1   | RA = Type 1 | Network         |                                               |
   |    |                  |                 |                 |               |             | synchronization |                                               |
   |    |                  |                 |                 | RA = Mode 2   |             |                 |                                               |
   +----+------------------+-----------------+-----------------+---------------+-------------+-----------------+-----------------------------------------------+
   | 1D | In-Coverage      | In-Coverage     | In-Coverage     | Yes           | Yes         | Yes             | .. image:: figures/sidelink-scenarios-1d.*    |
   |    | Multiple         |                 |                 |               |             |                 |    :scale: 30 %                               |
   |    | Cells            |                 |                 | RA = Mode 1   | RA = Type 1 | Network         |                                               |
   |    |                  |                 |                 |               |             | synchronization |                                               |
   |    |                  |                 |                 | RA = Mode 2   |             |                 |                                               |
   +----+------------------+-----------------+-----------------+---------------+-------------+-----------------+-----------------------------------------------+
   | **RA = Resource Allocation**                                                                                                                              |
   +-----------------------------------------------------------------------------------------------------------------------------------------------------------+

The model is developed in a way that simulating the above ProSe services are not
interdependent. In the following, we describe all the changes introduced in the
|ns3| LTE architecture and its protocol stack to realize Sidelink functionality.


------------
Architecture
------------


eNB architecture
++++++++++++++++
There is no change in the eNB data (lte-design:ref:`fig-ca-enb-data-plane`),
control (lte-design:ref:`fig-ca-enb-ctrl-plane`) plane and neither in its
PHY/channel (lte-design:ref:`fig-lte-enb-phy`) model.


UE architecture
+++++++++++++++

Figure :ref:`fig-lte-sl-ue-arch-data-plane` and  Figure 
:ref:`fig-lte-sl-ue-arch-ctrl-plane` show the current UE model architecture in
the data plane and the control plane, respectively. 

The class ``LteSlUeNetDevice`` was created to handle packets in the data plane 
when the UEs are using the UE-to-Network Relay functionality and have
established a one-to-one direct communication link with another UE over the 
Sidelink. Each UE has an ``LteSlUeNetDevice`` object per active one-to-one 
direct communication link and the ``EpcUeNas`` is now in charge of filtering the
data packets towards the appropriate interface.   

.. _fig-lte-sl-ue-arch-data-plane:

.. figure:: figures/lte-sl-ue-arch-data-plane.*
   :scale: 30 %
   :align: center

   UE data plane architecture

In the control plane, the class ``LteSlUeController`` was added to the model to
provide a framework for the implementation of custom algorithms controlling the 
different decision processes involved in the UE-to-Network Relay functionality,
e.g., Relay selection, connection setup, IP configuration, etc..   
More details are provided below in the Sidelink UE Controller section. 

.. _fig-lte-sl-ue-arch-ctrl-plane:

.. figure:: figures/lte-sl-ue-arch-ctrl-plane.*
   :scale: 30 %
   :align: center

   UE control plane architecture

The Sidelink uses the same spectrum than the uplink for transmission and 
reception. Figure :ref:`fig-lte-sl-ue-phy` shows that a new instance of the
``SpectrumPhy`` class is now present in the PHY/channel architecture of the UE
in order to support the reception of packets transmitted by other UEs in the 
uplink spectrum.

.. _fig-lte-sl-ue-phy:

.. figure:: figures/lte-sl-ue-phy.*
   :scale: 90 %
   :align: center

   PHY and channel model architecture for the UE

NAS
+++

In LTE module, the NAS layer functionality is provided by ``EpcUeNas``
class. In addition to its existing capabilities, this class has been
extended to support LTE ProSe services, namely direct
communication, direct discovery, and UE-to-Network Relay.


For direct communication, it supports the functions to
activate/deactivate Sidelink bearers. Since there is no EPS bearer
for Sidelink communication, the existing TFT cannot be used to map IP
packets to a Sidelink bearer. Therefore, a new type of TFT, called
``LteSlTft`` is implemented. It maps IP packets to the Sidelink
bearers based only on their destination IP address. Moreover, the
``Send`` function has been extended for both UE NAS "Active" and "Off"
states.  In the active state, if there is a Sidelink bearer
established between the source and the destination UE, it utilizes
Sidelink bearer to send the packet, thus, prioritizing a Sidelink
bearer over normal LTE uplink bearer. On the other hand, in off state
in which previously UE was unable to send the packets, is now able to
use a Sidelink bearer, if established.

The extensions for the direct communication are also useful for
UE-to-Network Relay communication by helping in sending and receiving
of packets for the communication between Relay UE and Remote
UE. Additionally, to support public safety communication scenarios using
UE-to-Network Relays, the NAS has also been extended to include the
transfer of IPv6 packets. The
``EpcUeNas`` now uses TFTs for ingress filtering to forward the
received packets to the appropriate ``NetDevice``. For the Remote UE,
an ``LteSlTft`` is created to receive any packet with the destination
address matching the address generated for the Remote UE’s
``LteSlUeNetDevice``. For a Relay UE, an ``LteSlTft`` is created to
receive any packet with the source address prefix matching the prefix
delegated for relay purposes by the core network. Upon receiving a
packet in the DoRecvData function of ``EpcUeNas``, a check is
initially made to verify if the Source IP address and the Destination
IP address in the packet header matches the ingress ``LteSlTft``. If
yes, the packet is forwarded onto the appropriate sidelink callback
function. Otherwise, the packet is forwarded onto the uplink callback
function.

For the direct discovery, it conveys the information, e.g., list of
discovery applications and the nature of the application, i.e.,
announcing or monitoring to ``LteUeRrc`` class.

.. _sec-lte-sl-rrc:

RRC
+++

The RRC layer has been modified to support all the three ProSe services. Among
all the new changes, a major modification which is common in both eNB and UE RRC
is the addition of two new classes called ``LteSlEnbRrc`` and ``LteSlUeRrc``.
Both the classes serve a common purpose of holding Sidelink resource pool (i.e.,
communication and discovery) configuration done through the functions added to
the ``LteHelper`` class. Figures :ref:`fig-lte-sl-enb-rrc` and
:ref:`fig-lte-sl-ue-rrc` show the relationship between
``LteEnbRrc<-->LteSlEnbRrc`` and ``LteUeRrc<-->LteSlUeRrc`` classes.

**Note: Only the key responsibilities of the new classes are shown. For complete
view of these classes please refer to their class APIs**.

.. _fig-lte-sl-enb-rrc:

.. figure:: figures/lte-sl-enb-rrc.*
   :scale: 90 %
   :align: center

   Relationship between LteEnbRrc and LteSlEnbRrc

.. _fig-lte-sl-ue-rrc:

.. figure:: figures/lte-sl-ue-rrc.*
   :scale: 90 %
   :align: center

   Relationship between LteUeRrc and LteSlUeRrc

The following Figure :ref:`fig-lte-sl-pool-config` shows the interaction among
the classes to configure a Sidelink pool.

.. _fig-lte-sl-pool-config:

.. figure:: figures/lte-sl-pool-config.*
   :scale: 90 %
   :align: center

   ns-3 LTE Sidelink pool configuration flow

The Sidelink pools, for both in-coverage and out-of-coverage UEs are
configured through the user's simulation script. Moreover, all the RRC SAP
classes, i.e., control and data including the ``LteRrcSap`` class have been
extended to support Sidelink functionalities.


In the following we will explain the remaining modifications specifically
introduced in the eNB and UE RRC layer.

eNB RRC
-------

To support in-coverage Sidelink scenarios, SIB18 and SIB19 were added to
broadcast Sidelink resource pool configuration for communication and discovery
respectively. For simplicity, we use the same periodicity (default: 80 ms) of
all the SIB messages, which could be configured by changing the attribute
``SystemInformationPeriodicity`` value. The resource pools are configured
through ``LteHelper`` as discussed earlier. Similar to other SIB messages, SIB18
and SIB19 are defined in ``LteRrcSap class``. By receiving these SIB messages, a
UE can deduce the type of ProSe service an eNB can support. The eNB is also now
capable of processing ``SidelinkUeInformation`` messages sent by the UEs
[TS36331]_. In general, this type of message contains information related to the
frequency the UE is intended to use for Sidelink, resources required by the UE for
Sidelink communication or discovery and a list of destination identities.
We note that, **at this moment only one Tx/Rx pool per UE** is supported. 
In response to the ``SidelinkUeInformation``, eNB sends a
``RrcConnectionReconfiguration`` message containing the resource allocation
information as per the supported ProSe services. In case of in-coverage Sidelink
communication, resource allocation is performed as per MODE1, which is also
referred as ``Scheduled`` mode [TR36843]_. In this mode, the
``RrcConnectionReconfiguration``, as per the standard, includes the
specifications of the pool to be used and the timing specification for Sidelink
Buffer Status Report (SL-BSR) transmission and re-transmission. On the other
hand, if the pool is for MODE 2, i.e., ``UeSelected`` mode, it only includes the
dedicated pool specifications.

For the in-coverage discovery, only ``Type1``, i.e. UE selected resource
allocation is supported.

UE RRC
------

The ``LteUeRrc`` and ``LteSlUeRrc`` classes have been extended to support all 
the ProSe services for the scenarios in Table
:ref:`tab-ns-3-LTE-Sidelink-supported-scenarios`. To highlight all the 
modifications, lets discuss them in context of each ProSe service.

Sidelink direct communication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The UE RRC now supports the creation of Sidelink bearers for both in-coverage
and out-of-coverage UEs. A new function ``DoActivateSidelinkRadioBearer``
is implemented for this purpose. If the configuration of the TFT used indicates
the nature of the Sidelink radio bearer as ``Bidirectional``, the creation of
Sidelink bearers for Tx and Rx, which is to populate the Tx/Rx pool
configuration along with the list of destinations to the lower layers, occurs at
the same time. Only the creation of PDCP/RLC instances for TX bearer is done in
``DoActivateSidelinkRadioBearer`` function. The creation of PDCP/RLC instances
for Rx occurs upon the reception of first Sidelink packet [TS36300]_. For the
in-coverage case, the bearer establishment procedure involves the communication
with the eNB by sending ``SidelinkUeInformation`` message for the sake of
resource allocation as shown in Figures :ref:`fig-lte-sl-in-coverage-tx` and
:ref:`fig-lte-sl-in-coverage-rx`.

.. _fig-lte-sl-in-coverage-tx:

.. figure:: figures/lte-sl-in-coverage-tx.*
   :scale: 80 %
   :align: center

   ns-3 LTE Sidelink in-coverage radio bearer activation (Tx)

.. _fig-lte-sl-in-coverage-rx:

.. figure:: figures/lte-sl-in-coverage-rx.*
   :scale: 70 %
   :align: center

   ns-3 LTE Sidelink in-coverage radio bearer activation (Rx)

For the out-of-coverage UEs, the Sidelink bearer activation and the
resource allocation is done by the UE autonomously. Figure
:ref:`fig-lte-sl-out-of-coverage-tx` and :ref:`fig-lte-sl-out-of-coverage-rx`
show sequence diagrams of the out-of-coverage Sidelink bearer activation for TX
and RX respectively.

.. _fig-lte-sl-out-of-coverage-tx:

.. figure:: figures/lte-sl-out-of-coverage-tx.*
   :scale: 80 %
   :align: center

   ns-3 LTE Sidelink out-of-coverage radio bearer activation (Tx)

.. _fig-lte-sl-out-of-coverage-rx:

.. figure:: figures/lte-sl-out-of-coverage-rx.*
   :scale: 70 %
   :align: center

   ns-3 LTE Sidelink out-of-coverage radio bearer activation (Rx)

As mentioned earlier, the UE RRC is now also capable of processing new SIB18
message and the Sidelink direct communication configuration received in
``RrcConnectionReconfiguration`` message. For in-coverage UEs, resource
allocation can be done in MODE 1 and MODE 2. In MODE 1, the resources are set to
"Scheduled" and the eNB is responsible of scheduling the exact number of
resources (i.e., with the help of the scheduler), while in MODE 2, resources are
"UeSelected" and a UE receives only one dedicated resource pool through
``RrcConnectionReconfiguration`` message to choose the resources from
[TS36331]_. By standard the SIB18 message is used as an indicator that an eNB
supports ProSe communication services, thus, the UE transmits
``SidelinkUeInformation`` if it has received SIB18 message. A UE in
"IDLE_CAMPED_NORMALLY" or in "CONNECTED_NORMALLY" is able to receive SIB18
messages, upon which, Rx pools are updated and also populated to the MAC and PHY
layers. On the other hand, if the UE is out-of-coverage it uses a pre-configured
pool and MODE 2 for resource allocation.

Sidelink direct discovery
^^^^^^^^^^^^^^^^^^^^^^^^^

For the Sidelink direct discovery, ``LteUeRrc`` and ``LteSlUeRrc``
classes now support creation/removal of discovery applications for
both in-coverage and out-of-coverage UEs. The ``LteSlUeRrc``
class has functions called StartDiscoveryApps and StopDiscoveryApps to
accomplish this process. These functions take in as arguments the
application codes as payloads and the discovery role of the UE which
are populated using the ``LteSidelinkHelper``. For the in-coverage UEs,
communication occurs with the eNB by transmitting the
``SidelinkUeInformation`` message for resource allocation, followed by
the eNB ``RrcConnectionReconfiguration`` message. An in-coverage UE
only transmits ``SidelinkUeInformation`` if it has received SIB19
message from its serving eNB.
We note that, for in-coverage UEs, Scheduled resource
allocation mode is not supported yet. The model supports only Type1,
i.e., "UeSelected" resource allocation. An in-coverage UE can receive a
list of multiple pools with the criteria of selection either RANDOM or
RSRPBASED via the ``RrcConnectionReconfiguration`` message. A UE in
IDLE_CAMPED_NORMALLY or in CONNECTED_NORMALLY can receive SIB19
messages, upon which, Rx pools are updated and populated to the MAC
and PHY layers. On the other hand, when out-of-coverage, the
UE only uses the pre-configured pool.
 
The classes ``LteUeRrc``, ``LteSlUeRrc`` and ``LteSlDiscHeader`` are
also extended to incorporate Model B application discovery along with
Model A application discovery. In model A, the announcing UE
periodically broadcasts PC5_DISCOVERY messages, called announcements,
to indicate its presence. In model B, the discoverer UE seeks to
discover UEs by sending a request message with the required
application code. The discoveree UEs check if the application code is
present in a list of monitored codes. If present, the UE then
transmits the same application code as the intended response message
payload. For both Model A and Model B discovery, the payloads are
added to the appropriate message in the ``LteSlDiscHeader`` class and
transmitted using the TransmitApp and TransmitDiscoveryMessage
functions in ``LteSlUeRrc`` and ``LteUeRrc`` classes respectively. In
case multiple requests from separate UEs are received, the
model ensures that only one PC5_DISCOVERY response message is sent per
discovery period. Multiple responses are not needed since these
messages are broadcasted.

**Note: At this stage only one pool is supported**

Sidelink synchronization
^^^^^^^^^^^^^^^^^^^^^^^^

The ``LteUeRrc`` class holds the main implementation for performing Sidelink
synchronization. In particular, it supports

 * Activation/Deactivation of Sidelink Synchronization Signal (SLSS)
 * The configuration of the SLSS
 * Delivering of the configured SLSS to phy layer for transmission
 * Reception and L3 filtering of the SLSS from other UEs
 * Selection of a Synchronization Reference UE (SyncRef)
 * Notification of SyncRef change to the lower layers

By setting the attribute ``UeSlssTransmissionEnabled == true`` enables the
transmission of SLSS. The purpose of synchronization process is to align the
frame and subframe number of a UE with a SyncRef. An SLSS from a SyncRef is
considered detectable if;

.. math::
   :label: SyncRef detection criterion

      SRSRP_{strongestSyncRef}[dBm] - MinSrsrp[dBm] > syncRefMinHyst  [dB]

:math:`MinSrsrp` is the minimum threshold for detecting SLSS configured using
``MinSrsrp`` attribute of ``LteUeRrc`` class and :math:`syncRefMinHyst` is the
pre-configured hysteresis value, which could be configured from the user's
simulation script. In case more than one SyncRefs are detected during a scanning
period (Refer to the UE phy section below for details), a SyncRef with the
highest Sidelink RSRP (S-RSRP) is selected. Upon re-selection of the SyncRef, if
a better SyncRef is detected the UE selects this new SyncRef if;

.. math::

      SRSRP_{NewStrongestSyncRef}[dBm] - SRSRP_{OldStrongestSyncRef}[dBm] > syncRefDiffHyst  [dB]

where :math:`syncRefDiffHyst` is a threshold representing how higher the S-RSRP
of the newly detected SyncRef should be than the currently selected SyncRef's
S-RSRP to consider a change. On the other hand, if the S-RSRP of the previously
selected SyncRef satisfies the :eq:`SyncRef detection criterion` and its S-RSRP
is greater than a pre-configured :math:`syncTxThreshOoC` (i.e. out-of-coverage
synchronization threshold) [TS36331]_ the SyncRef is considered as valid till
the next re-selection.

In case, the previously selected SyncRef is not valid anymore and no other
SyncRef has been detected, the UE itself becomes a SyncRef by choosing SLSS-ID
and the Sidelink synchronization offset indicator. Different from the standard
[TS36331]_, the current implementation for the sake of simplicity selects a
SLSS-ID as ``SLSS-ID = IMSI * 10``, while the synchronization offset indicator,
as per the standard, is randomly selected between the two pre-configured
offsets, i.e., ``syncOffsetIndicator1`` and ``syncOffsetIndicator2``. We note
that, when a Synchronization process is followed by ProSe group communication or
discovery, a UE could only be able to receive a message from other UE, if they
have same SLSS-ID and the group id is known to the receiving UE. However, when
simulating only ProSe communication or discovery the SLSS-ID of all the UEs are
initialized to 0 and the UE receives the transmission if the group id is known.
Moreover, the transmission of the SLSS is subjective to the data transmission by
the UE. At the beginning of every Sidelink Control (SC) period, a notification
by the UE MAC layer is sent to the UE RRC, indicating if there is data to be
transmitted. The function ``DoNotifyMacHasSlDataToSend`` and
``DoNotifyMacHasNoSlDataToSend`` of ``LteUeRrc`` class serve this purpose. If
there is no data to be transmitted the UE stops transmitting the SLSS.

UE-to-Network Relay communication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The classes ``LteUeRrc`` and ``LteSlUeRrc`` now support the creation and 
maintenance of one-to-one direct communication links between two UEs using
the Sidelink. A new type of signaling protocol, called PC5 signaling, has been
defined for this purpose [TS24334]_.
In the model, the PC5 signaling messages are modeled as ns-3 headers 
and are listed in Table :ref:`tab-pc5-signaling-messages` together with the
acronyms used in the rest of the section to refer to them.  
One-to-one direct communication links are used for UE-to-Network Relay 
communication functionality and two roles are defined for the UEs in this
context: Relay UE and Remote UE.
Figure :ref:`fig-lte-sl-o2o-comm-message-exchange` shows a typical PC5  
messages exchange between a Remote UE and a Relay UE to successfully establish
and maintain a one-to-one direct communication link between them.  

The Remote UE initiates the procedure by sending a DCRq message to the Relay 
UE. After the successful exchange of the DSMCm and DSMCp messages for link 
authentication, the Relay UE sends a DCA message to accept the connection.
Once established, the Relay UE can request additional information from the
Remote UE, by sending a RUIRq message, for which the Remote UE responds back
with a RUIRs message. This procedure is optional and deactivated by the default,
but it can be enabled using the attribute "RuirqEnabled" of the class 
``LteSlUeRrc``.

For both UEs, the reception of PC5 data or PC5 signaling
messages indicates that the connection is active. Additionally, probe DCK
messages are periodically sent by the Remote UE to maintain the one-to-one
direct communication link connection in the event that there is no data to
exchange. The Relay UE then responds by sending a DCKA message.

When either the Remote UE or the Relay UE requires to end the one-to-one direct
communication link, it sends a DCR message to the peer UE, which then replies
with a DCRA message. 

During the one-to-one direct communication link setup, the Relay UE sends a 
DCRj message if it cannot accept the connection. Currently in the model, this
only happen when the DSMCp message is not received on time (i.e., T4111 
expires). The Remote UE can in turn reject the DSMCm and send a DSMRj. 
Currently, no logic is implemented for this, i.e., the Remote UE always accept
the DSMCm. 

**Note: Rekeying messages are defined in the model but currently not used in
the implementation.**

.. tabularcolumns:: |p{2cm}|p{8cm}|c|

.. _tab-pc5-signaling-messages:

.. table:: PC5 Signaling Messages 

   +---------------+--------------------------------------------+-----------------+
   | **Acronym**   | **PC5 Signaling Message**                  | **Message ID**  |
   +===============+============================================+=================+
   | DCA           | DIRECT_COMMUNICATION_ACCEPT                | 2               |
   +---------------+--------------------------------------------+-----------------+
   | DCK           | DIRECT_COMMUNICATION_KEEPALIVE             | 4               |
   +---------------+--------------------------------------------+-----------------+
   | DCKA          | DIRECT_COMMUNICATION_KEEPALIVE_ACK         | 5               |
   +---------------+--------------------------------------------+-----------------+
   | DCR           | DIRECT_COMMUNICATION_RELEASE               | 6               |
   +---------------+--------------------------------------------+-----------------+
   | DCRA          | DIRECT_COMMUNICATION_RELEASE_ACCEPT        | 7               |
   +---------------+--------------------------------------------+-----------------+
   | DCRj          | DIRECT_COMMUNICATION_REJECT                | 3               |
   +---------------+--------------------------------------------+-----------------+
   | DCRq          | DIRECT_COMMUNICATION_REQUEST               | 1               |
   +---------------+--------------------------------------------+-----------------+
   | DRRq *        | DIRECT_REKEYING_REQUEST                    | 15              |
   +---------------+--------------------------------------------+-----------------+
   | DRRs *        | DIRECT_REKEYING_RESPONSE                   | 16              |
   +---------------+--------------------------------------------+-----------------+
   | DRT *         | DIRECT_REKEYING_TRIGGER                    | 17              |
   +---------------+--------------------------------------------+-----------------+
   | DSMCm         | DIRECT_SECURITY_MODE_COMMAND               | 12              |
   +---------------+--------------------------------------------+-----------------+
   | DSMCp         | DIRECT_SECURITY_MODE_COMPLETE              | 13              |
   +---------------+--------------------------------------------+-----------------+
   | DSMRj         | DIRECT_SECURITY_MODE_REJECT                | 14              |
   +---------------+--------------------------------------------+-----------------+
   | RUIRq         | REMOTE_UE_INFO_REQUEST                     | 18              |
   +---------------+--------------------------------------------+-----------------+
   | RUIRs         | REMOTE_UE_INFO_RESPONSE                    | 19              |
   +---------------+--------------------------------------------+-----------------+
   | \* Messages defined but currently not used in the implementation             |
   |                                                                              |
   +------------------------------------------------------------------------------+

.. _fig-lte-sl-o2o-comm-message-exchange:

.. figure:: figures/lte-sl-o2o-comm-message-exchange.*
   :scale: 35 %
   :align: center

   One-to-one direct communication typical PC5 messages exchange

The class ``LteSlUeRrc`` uses a map of ``LteSlO2oCommParams`` objects indexed 
by an ``LteSlPc5ContextId`` structure to keep track of each active one-to-one
direct communication link. 
The ``LteSlPc5ContextId`` is composed of the Layer 2 ID of the peer UE  
and the context ID. The context ID is a sequence number that each UE having a 
Remote UE role increments when it starts a relay direct communication process. 
This sequence number is then sent in the DCRq to be used by the Relay UE for the 
``LteSlPc5ContextId`` of the corresponding ``LteSlO2oCommParams`` object on his
side.  

The class ``LteSlO2oCommParams`` implements the one-to-one direct communication
link state machine, the different one-to-one communication timers and counters,
and also keeps the copies of certain PC5 signaling messages used for 
retransmission purposes. 
Figure :ref:`fig-lte-sl-o2o-comm-state-machine-relay` and Figure 
:ref:`fig-lte-sl-o2o-comm-state-machine-remote` depict the one-to-one direct
communication link state machines depending on the UE role.

The timers involved in the one-to-one direct communication link establishment and
maintenance are listed in Table :ref:`tab-one-to-one-comm-timers` together with
their start, stop, and expiration conditions. 
Timers T4100, T4101, T4103, and TRUIR control the retransmission of DCRq, DCK, 
DCR, and RUIRq messages, respectively, bounded by the corresponding
maximum number of retransmissions listed in Table 
:ref:`tab-one-to-one-comm-max-msg-rtx`. 

.. _fig-lte-sl-o2o-comm-state-machine-relay:

.. figure:: figures/lte-sl-o2o-comm-state-machine-relay.*
   :scale: 35 %
   :align: center

   One-to-one direct communication link state machine for the Relay UE

.. _fig-lte-sl-o2o-comm-state-machine-remote:

.. figure:: figures/lte-sl-o2o-comm-state-machine-remote.*
   :scale: 35 %
   :align: center

   One-to-one direct communication link state machine for the Remote UE



.. tabularcolumns:: |l|p{1cm}|p{4.5cm}|p{4.8cm}|p{2.8cm}|

.. _tab-one-to-one-comm-timers:

.. table:: One-to-one direct communication link timers 

   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | **Timer**          | **UE Role** | **Start**                                | **Stop**                           | **On Expiry**                                 | 
   |                    |             |                                          |                                    |                                               |
   +====================+=============+==========================================+====================================+===============================================+
   | T4100              | Remote      | Upon transmitting a DCRq                 | Upon receiving a DCA or a DCRj     | Retransmit DCRq                               |
   |                    |             |                                          |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | T4101              | Remote      | Upon transmitting a DCK                  | Upon receiving a DCKA or PC5 data  | Retransmit DCK                                |
   |                    |             |                                          |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | T4102              | Remote      | Upon completing connection and upon      | Upon receiving a DCKA or PC5 data  | Transmit DCK                                  |
   |                    |             | receiving a DCKA or PC5 data thereafter  |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | T4103              | Remote      | Upon transmitting a DCR                  | Upon receiving a DCRA              | Retransmit DCR                                |
   |                    | Relay       |                                          |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | T4108              | Relay       | Upon completing connection and upon      | Upon receiving a DCK or PC5 data   | Initiate connection release                   |
   |                    |             | receiving a DCK or PC5 data thereafter   |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | T4111              | Relay       | Upon transmitting a DSMCm                | Upon receiving a DSMCp or DSMRj    | Abort connection setup                        |
   |                    |             |                                          |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+
   | TRUIR              | Relay       | Upon transmitting a RUIRq                | Upon receiving a RUIRs             | Retransmit RUIRq                              |
   |                    |             |                                          |                                    |                                               |
   +--------------------+-------------+------------------------------------------+------------------------------------+-----------------------------------------------+


.. tabularcolumns:: |l|p{1cm}|p{8cm}|

.. _tab-one-to-one-comm-max-msg-rtx:

.. table:: One-to-one direct communication PC5 signaling messages retransmission limits

   +--------------------+-------------+-----------------------------------------------+
   | **Variable**       | **UE Role** | **Description**                               | 
   |                    |             |                                               |     
   +====================+=============+===============================================+ 
   | DCRq_maximum       | Remote      | Maximum number of DCRq retransmissions        |
   |                    |             |                                               |
   +--------------------+-------------+-----------------------------------------------+
   | DCK_maximum        | Remote      | Maximum number of DCK retransmissions         |
   |                    |             |                                               |
   +--------------------+-------------+-----------------------------------------------+
   | DCR_maximum        | Remote      | Maximum number of DCR retransmissions         |
   |                    | Relay       |                                               |
   +--------------------+-------------+-----------------------------------------------+
   | RUIR_maximum       | Relay       | Maximum number of RUIR retransmissions        |
   |                    |             |                                               |
   +--------------------+-------------+-----------------------------------------------+


UE-to-Network Relay discovery and selection 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 
The classes ``LteUeRrc`` and ``LteSlUeRrc`` were extended to support the 
creation/removal of UE-to-Network Relay services, which are identified by relay
service codes, and are used for the UE-to-Network Relay discovery procedure.
Both discovery models (A and B) are supported for UE-to-Network Relay discovery.
 
With Model A, the Relay UE periodically broadcasts PC5_DISCOVERY messages of 
type *UE-to-Network Relay Discovery Announcement* to indicate its presence and 
includes the supported relay service code in the message. Remote UEs monitor
those messages and react accordingly when they find the relay service code of 
interest. 
 
With Model B, the Remote UE sends a PC5_DISCOVERY message of type 
*UE-to-Network Relay Discovery Solicitation* containing the relay service code 
it is looking for. Relay UEs monitor those  messages and verify if the relay
service code in the solicitation message is in their list of provided service
codes. In affirmative case, they respond by sending a message of type 
*UE-to-Network Relay Discovery Response*, which are monitored by the Remote UEs.
In case multiple solicitations from separate Remote UEs are received, the model
ensures that only one PC5_DISCOVERY response message is sent per discovery 
period. Multiple responses are not needed since these messages are broadcasted. 
 
For both discovery models, the appropriate parameters depending on the 
discovery message type are set in an ``LteSlDiscHeader`` object in the function
TransmitRelayMessage of the ``LteSlUeRrc``, which is then passed to lower layers 
for transmission using the TransmitDiscoveryMessage function in the ``LteUeRrc``.  
 
As in the Sidelink Direct Discovery, the relay service code and the role of the
UE (Remote UE or Relay UE) are populated using the ``LteSidelinkHelper``.
When using Model A, the Relay UE sends announcements every discovery period.
When using Model B, the frequency of the solicitation messages
sent by the Remote UEs can be set with the attribute ``LteSlUeRrc::RelaySolFreq``.
 
When a Remote UE starts the UE-to-Network Relay service, the ``LteUeRrc`` 
notifies the ``LteUePhy`` to enable the Sidelink Discovery Reference
Signals Received Power (SD-RSRP) measurements. Afterwards, the 
SD-RSRP measurement reports are received periodically by the ``LteUeRrc``  
which processes them (e.g., apply L3 filtering if it is enabled) and report the 
valid Relay UEs together with their relay service code and the (L3
filtered) SD-RSRP to the ``LteSlUeRrc``. 
A Relay UE is considered valid if its (L3 filtered) SD-RSRP satisfies:
 
.. math::

      SD{\text -}RSRP [dBm] - qRxLevMin [dBm] > minHyst  [dB]
 
where :math:`qRxLevMin`  and :math:`minHyst` are provided via the Sidelink
preconfiguration if the Remote UE is out-of-coverage, or via the SIB19 
broadcasted by the eNB if the Remote UE is in-coverage. In either case, these
parameters must be configured in the simulation scenario. 
 
Then, the ``LteSlUeRrc`` reports the list of valid Relay UEs to the 
``LteSlUeController`` which executes the configured Relay UE selection 
algorithm and report back the selected Relay UE (if any) to the ``LteSlUeRrc``.
Depending on the result of the selection algorithm, the ``LteSlUeRrc`` 
determines if the Remote UE should: 
 
*  disconnect from its current Relay UE if any, e.g., if the current Relay UE
   is not suitable anymore, or a most suitable Relay UE was selected, and/or
*  try to connect (i.e., start the one-to-one direct communication link setup) 
   to the selected Relay UE, or
*  perform no action, e.g., no suitable Relay UE is available, or the current 
   Relay UE is still the most suitable.  
 
An example of the overall UE-to-Network Relay selection process and the functions involved is
depicted in Figure :ref:`fig-lte-sl-relay-selection` and the extensions made to
the other components of the model to support them are described on each 
corresponding section in the rest of the document.
 
.. _fig-lte-sl-relay-selection:
 
.. figure:: figures/lte-sl-relay-selection.png
   :scale: 120 %
   :align: center
 
   UE-to-Network Relay selection mechanism. 
 
 
Sidelink UE controller
----------------------
The class ``LteSlUeController`` was created to provide a framework for the
implementation of custom UE-to-Network Relay configurations and algorithms.
The Sidelink UE Controller service interface is provided by two Service Access
Points (SAPs):

*  The ``LteSlUeCtrlSapProvider`` part is provided by the ``LteSlUeController``
   and used by the ``LteSlUeRrc``
*  The ``LteSlUeCtrlSapUser`` part is provided by the ``LteSlUeRrc`` and used
   by the ``LteSlUeController``

Currently, no primitives are implemented for the ``LteSlUeCtrlSapUser`` and the
following functions are defined for the ``LteSlUeCtrlSapProvider``:

*  ``LteSlUeCtrlSapProvider::RecvRelayServiceDiscovery`` is used to indicate that the Remote UE 
   received a UE-to-Network Relay Discovery Announcement or a UE-to-Network 
   Relay Discovery Response containing a monitored relay service code. 
*  ``LteSlUeCtrlSapProvider::Pc5ConnectionStarted`` indicates that the one-to-one direct
   communication link setup procedure started. This occurs for Remote UE when
   it starts the relay direct communication service and it sends the first DCRq
   to a given Relay UE, and for the Relay UE the indication is done when it
   receives a DCRq from a Remote UE and process it.  
*  ``LteSlUeCtrlSapProvider::Pc5SecuredEstablished`` indicates that the one-to-one direct
   communication link setup was completed successfully by the UE. Currently, 
   this happens when the Relay UE receives DSMCp message from the Remote UE and
   the link authentication was completed, and when the Remote UE receives the 
   corresponding DCA message from the Relay UE. This function should configure
   the UE to be able to use the one-to-one direct communication link to send
   and receive data on it and towards the network, e.g., IP configuration and
   forwarding, traffic filtering and Sidelink bearers configuration. 
*  ``LteSlUeCtrlSapProvider::Pc5ConnectionTerminated`` indicates that the direct communication 
   link release procedure is complete for an established one-to-one link. 
   This happen for the UEs in both roles, when they receive a DCR, or when they
   already started the procedure and receive a DCRA message, or when T4103 
   expires and DCR_maximum was reached. This function should implement the 
   logic to clean up the configurations in the UE associated to the released link.  
*  ``LteSlUeCtrlSapProvider::Pc5ConnectionAborted`` is called by the Remote UE when the direct
   one-to-one direct communication link setup fails, i.e., when a DCRj message
   is received or when T4100 expires and DCRq_maximum was reached. This 
   function should implement the logic to clean up the configurations 
   associated to the link being setup.  
*  ``LteSlUeCtrlSapProvider::RecvRemoteUeReport`` is used by the Relay UE upon 
   connecting to a Remote UE to indicate that the network was informed about 
   the Remote UE connection.   
*  ``LteSlUeCtrlSapProvider::RelayUeSelection`` is called during the Relay selection
   procedure and it is the primitive that should implement the Relay selection 
   logic of the controller given the list of valid detected Relay UEs and their
   corresponding Sidelink Discovery Reference Signals Received Power (SD-RSRP). 
   This function should return the ID of the selected 
   Relay UE if any, or zero otherwise to indicate no suitable Relay UE is available.
  
A Sidelink UE controller is implemented by writing a subclass of the 
``LteSlUeController`` superclass and implementing each of the above mentioned
SAP interface functions. Users may develop their own controller this way, or may 
use the provided example implementation called ``LteSlBasicUeController``, which
is available in the model to provide the following baseline implementation 
of the functions mentioned above: 

*  ``LteSlBasicUeController::DoRecvRelayServiceDiscovery`` does not 
   implement any logic but can be edited to store the content of the
   discovery messages (e.g. status indicator) and contribute to future Relay UE 
   selection algorithms.
*  ``LteSlBasicUeController::DoPc5ConnectionStarted`` does not 
   implement any logic and is currently only used for tracing the start of the 
   one-to-one connection setup. 
*  ``LteSlBasicUeController::DoPc5SecuredEstablished`` of this controller 
   interacts with the ``LteUeNetDevice`` of the UE and the ``LteSidelinkHelper``
   to create the ``LteSlUeNetDevice`` corresponding to the one-to-one direct 
   communication link that was established. It also configures static
   IPv6 addresses and routing, enables the packet forwarding for the 
   Relay UE, and configures the appropriate packet filters by creating the 
   corresponding ``LteSlTft`` objects. Please note that this controller sets 
   the Relay UE address as the default route for the Remote UE. Thus, all 
   packets from the Remote UE will be sent to Relay UE instead of to the eNodeB
   after the connection is established.
*  ``LteSlBasicUeController::DoPc5ConnectionTerminated`` of this controller 
   reverts the configurations made during the 
   ``LteSlBasicUeController::DPc5SecuredEstablished`` call for the link being 
   released.
*  ``LteSlBasicUeController::DoPc5ConnectionAborted`` resets the link setup 
   process for the Remote UE. 
*  ``LteSlBasicUeController::DoRecvRemoteUeReport`` is used by the Relay UE
   to inform the network about a new Remote UE connection. This is done by calling
   the helper function ``LteSidelinkHelper::RemoteUeContextConnected`` with the 
   corresponding Remote UE information. 
*  ``LteSlBasicUeController::DoRelayUeSelection`` currently implements the
   following three Relay UE selection algorithms, and the controller will use the
   one configured in the ``LteSlBasicUeController::RelayUeSelectionAlgorithm`` 
   attribute.

   *  RANDOM_NO_RESELECTION: (Default) If not connected to any Relay UE, the
      algorithm randomly selects a Relay UE from the list of valid Relay UEs, 
      and then returns its ID. Otherwise, it simply returns the ID of the Relay
      UE to which the Remote UE is already connected.
   *  MAX_SDRSRP_NO_RESELECTION:  If not connected to any Relay UE, the 
      algorithm selects the Relay UE with the strongest SD-RSRP from the list
      of valid Relay UEs, and then returns its ID. Otherwise, it simply returns
      the ID of the Relay UE to which the Remote UE is already connected.
   *  MAX_SDRSRP: The algorithm selects the Relay UE with the strongest 
      SD-RSRP from the list of valid Relay UEs, and then returns its ID, no 
      matter the Remote UE current connection status. 

*  Additionally, two traces are provided which can be hooked up to some custom 
   functions:

   *  ``LteSlBasicUeController::RelayUeSelection`` traces each time a 
      Remote UE selects a new Relay UE, and
   *  ``LteSlBasicUeController::Pc5ConnectionStatus`` traces each status change
      of each one-to-one connection between a Remote UE and a Relay UE. 

**Note: Only IPv6 is supported when using the UE-to-Network Relay functionality**


PDCP
++++

The ``LtePdcp`` class has been extended to include ProSe source Layer
2 ID and the ProSe Layer-2 Group ID to identify the PDCP/RLC pair to
be used in the receiving UE. The reason is that in Sidelink, each
radio bearer is associated with one PDCP entity. And, each PDCP entity
is associated to one RLC entity, since it uses RLC-UM mode for the
transmission / reception [TS36323]_. Thus, given the nature of
Sidelink communication, i.e., one to many, it may happen that multiple
UEs transmit to a single UE, assigning LCIDs independently. It may
happen that two UEs select same LCID for the same group. Therefore,
for a receiving UE to distinguish between two Sidelink Radio bearers
(SLRBs) with the similar LCIDs requires additional identifiers. An
enumeration for the Service Data Unit (SDU) type is added in the
LtePdcpSapUser class to support the differentiation between distinct
types of packets such as IP, ARP, PC5_SIGNALING, and NON_IP at the RRC
layer.

RLC
+++

Similar to the PDCP layer, the RLC layer now has ProSe source Layer 2 ID and the
ProSe Layer-2 Group ID. This is achieved by extending the structure of
``TransmitPduParameters`` of ``LteMacSapProvider`` class. As mentioned earlier,
only RLC-UM mode is used for Sidelink. Moreover, ``LteRlcUm`` class is also
extended in accordance to the specification define in section 7.1 of [TS36322]_.
It says that "For RLC entity configured for Sidelink Traffic Channel (STCH), the
state variables ``VR(UR)`` and ``VR(UH)`` are initially set to the SN of the
first received PDU". A new attribute named "ChannelType" is introduced, which is
set to type ``STCH`` for Sidelink.

MAC
+++

Among the following eNB and UE specific enhancements for ProSe, one of the
important changes, irrespective of the eNB or UE implementation, is the
limitation to the maximum frame number, which is now set to 1024. This is
because all the computations to determine correctly the boundaries of ProSe
communication and discovery periods and their resource allocation considers the
maximum frame number to be 1024. This limitation also helps us to implement the
rollover constraints more accurately, e.g., to have a configurable offset to be
applied at the beginning of each communication and discovery periods. This
change mainly impacted all the schedulers. In particular, every scheduler
maintains a map of uplink resource allocation using the SFnSF (i.e., combination
of frame and subframe number) as an unique key to update the PUSCH based CQIs.
By limiting the frame number to 1024, means that a combination could repeat
after 10 sec of simulation, thus, a c++ map would not allow to insert new
allocation information with the same SFnSF combination. To handle this, we
appropriately remove any old allocation information before inserting a new one.

eNB MAC
-------

``LteEnbMac`` has been extended to support in-coverage ProSe communication,
i.e., Mode 1 resource allocation. The SAP between the ``LteEnbRrc`` and
``LteEnbMac`` is extended to receive the pool configuration to be conveyed to
the scheduler to schedule the resources. If the type of resource allocation for
the configured resource pool is ``Scheduled`` the eNB MAC is now capable of
receiving Sidelink Buffer status Report (SLBSR) from the UEs. To achieve this,
the ``structure`` ``MacCeListElement_s`` in ``ff-mac-common.h`` file is extended
to support BSR of type SLBSR. The reporting of the BSR in uplink is the same as
shown in Figure lte-design:ref:`fig-ca-uplink-bsr`. To handle the scheduling of
Sidelink resources for MODE 1, a scheduler ``RrSlFfMacScheduler`` based on the
existing Round Robin implementation is also provided. Thus, making it the only
scheduler, for the time being, in LTE module capable of supporting Sidelink and
normal LTE resource scheduling. It also makes sure that Sidelink and the uplink
resources are not scheduled in the same subframe. For Sidelink discovery, as
stated before, the "Scheduled" mode is not implemented yet.

UE MAC
------

The ``LteUeMac`` is extended to support all the three ProSe
services. The scheduling of Sidelink transmissions is executed in
a new function called ``DoSlDelayedSubframeIndication`` that occurs
after the downlink control messages are processed in order to know if
there are any uplink transmissions scheduled in the current subframe.

The DoNotifyUlTransmission function in the UE MAC is where the PHY
notifies the MAC that it has scheduled a transmission. The UE MAC now
processes the discovery message’s PHY PDU, the Sidelink Control
Information message’s PHY PDU and the data message’s PHY PDU
separately.

In the following we explain these extensions in context of each ProSe
service.

Sidelink direct communication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The UE MAC is now capable of receiving the RLC buffer status notifications with
extended Sidelink identifiers, i.e., layer 2 source and destination ID. A tuple
``SidelinkLcIdentifier`` of ``LteUeMac`` holds the *LCID*, *SrcL2Id* and *DstL2Id*,
which is then used as a key for a c++ map storing all the buffer status
notifications from RLC layer. 

Before a Sidelink D2D communication transmission on the Physical Sidelink
Shared Channel (PSSCH) takes place, a Sidelink grant needs to be configured.
The grant contains information used to configure how, when, and where in the
Sidelink resource pool transmissions will occur.  This information is exchanged
between communicating parties using the Sidelink Control Information Message
(SCI) on the Physical Sidelink Control Channel (PSCCH).

For "Scheduled" resources, i.e. MODE 1, a UE is
now capable of processing the Sidelink DCI (i.e., DCI 5 [TS36212]_) message
containing the resource allocation information for PSCCH and PSSCH
transmissions. A UE for transmitting PSCCH uses the PSCCH resource indicated in
the SL DCI from the eNB. To ensure the reliability of Sidelink Control
information (SCI), each control message is transmitted twice using two different
subframe, each utilizing 1 RB belonging to the configured resource block pool
[TS36213]_. Thus, supporting 2 PSCCH transmissions per grant. While the PSSCH
(i.e. Data) transmissions are computed asper the current frame and subframe
number and the information including in the SL DCI, i.e., Time Resource Pattern
Index (iTrp), Starting RB index and the total number of RBs to be used. Since
there are no HARQ feedbacks in Sidelink PSSCH, each transport block is
transmitted using 4 HARQ transmissions (i.e. RV = 4). Hence, for each grant
every transport block is transmitted using 4 subframes. 


On the other hand, for the "UeSelected" resource scheduling, i.e.
MODE 2, ``LteUeMac`` chooses a random PSCCH resource among the
available resources and similarly transmits twice using two
subframes. While for transmitting PSSCH it uses a pre-configured pool
configuration to determine RBs and subframe for 4 PSSCH
transmissions. The ``LteUeMac`` has been enhanced to dynamically configure the
Modulation and Coding Scheme (MCS), and the number of Physical
Resource Blocks (PRBs) to be used in the PSSCH of each Sidelink
period. When there is data to be transmitted over the PSSCH, the
transmission buffer size and the available number of transmissions per
PSSCH are used to determine the Transport Block Size (TBS) required
per Sidelink period. The number of transmission opportunities in the
PSSCH is configured with the KTRP value. Network operators have the
option to choose a single KTRP value, or a range from the acceptable
values, by setting the trpt- subset accordingly in the Sidelink pool
configuration [TS36.213].

A set of configuration metrics are implemented in the LTE UE MAC class; the grant
scheduling is performed following the configuration (including optimization goal
when applicable) associated with the metrics to meet the required TBS.

1. Fixed (FIXED): Default setting. Uses grant configuration specified
by user through the LteUeMac attributes: Ktrp, UseSetTrp, SetTrpIndex,
SlGrantMcs, and SlGrantSize. Ktrp and SetTrpIndex are used to specify
the number of active subframes and the index of the TRP respectively
[TS36213]_. The last two attributes are used to specify the MCS and
the number of RBs (per subframe) to be used for Sidelink
transmissions.

2. Random (RANDOM): Selects a grant configuration (MCS, PRBs, KTRP) at random. For
each enabled KTRP value, keep record of all <PRBs, MCS> pairs satisfying the
required TBS, choose <PRB, MCS> pair at random.

3. Minimum PRBs Usage (MIN_PRB): Selects a grant configuration that utilizes the
least number of PRBs per transmission. For each enabled KTRP value, find the <PRB,
MCS> pair with the fewer number of PRBs satisfying the required TBS and choose the
<PRB, MCS> pair with fewer PRBs.

4. Maximum Coverage (MAX_COVERAGE): Selects a grant configuration that would
maximize the communication range. For each enabled KTRP value, keep record of all
<PRBs, MCS> pairs satisfying the required TBS. Evaluate and rank each pair with
utility function D = 10 log (PRB) + SNR_threshold (Target BLER at 4th HARQ
transmission). Choose the <PRB, MCS> pair having the lowest value D.

Use the SlScheduler attribute from the UE MAC to select the scheduling grant metric
(LteUeMac::FIXED is the default). 


Moreover, upon the reception of a PHY PDU the ``LteUeMac`` class is
now able to notify UE RRC to establish a Sidelink radio bearer for the
reception.

Sidelink direct discovery
^^^^^^^^^^^^^^^^^^^^^^^^^

The list of pending discovery messages (received from RRC) to be sent
are stored in the m_discPendingTxMsgs variable. The Sidelink discovery
grant related variables are stored in the ``DiscGrant``
structure. This contains the discovery message to be sent in a
selected resource and a list of PSDCH transmissions within the pool
(each discovery message can be retransmitted up to 3 times). The
Sidelink discovery pool information is stored in the ``DiscPoolInfo``
structure. This contains a list of grants for the current discovery
period and the next discovery period. These extensions help the model
support the transmission of multiple discovery messages within a
Sidelink period. Each discovery message occupies 2 contiguous RBs in a
subframe belonging to the assigned pool.

In reception, the discovery messages are passed to the RRC layer,
where they are appropriately processed.

Sidelink synchronization
^^^^^^^^^^^^^^^^^^^^^^^^

For the Sidelink synchronization, ``LteUeMac`` receives notification from
``LteUePhy`` upon change of time, i.e., change of frame and subframe number when
a new SyncRef is selected.

PHY
+++

eNB PHY
-------

The LTE D2D module of |ns3| at the time of writing this documentation
does not support Radio Link Failure (RLF) functionality. Therefore, to
simulate out-of-coverage scenarios a workaround has been implemented
to disable eNB PHY layer. An eNB can be disable by configuring the
attribute "DisableEnbPhy" of ``LteHelper``. Additionally, new
conditions have been implemented to disregard control messages such as
Sidelink Control Information SCI, Master Information Block for
Sidelink MIB_SL, and Sidelink Discovery Message SL_DISC_MSG as these do
not pertain to uplink control messages to the eNB.

UE PHY
------

As shown in Figure :ref:`fig-lte-sl-ue-phy`, to receive the Sidelink
transmission in the uplink channel the ``LteUePhy`` includes an
additional ``LteSpectrumPhy`` instance. In particular, the function
``DoConfigureUplink``, which is called by ``LteUeRrc`` is responsible
of adding this new instance to the channel. The ``LteUePhy`` class has
new functions PhyPscchPduReceived, PhyPsdchPduReceived, and
PhyPsbchPduReceived to respectively receive control, discovery, and
broadcast PHY PDU respectively. Similarly, ``LteSpectrumPhy`` is
extended with new function RxSlPscch, RxSlPsdch, and RxSlPsbch (and
corresponding callbacks for interconnecting PHY and MAC) to accomplish
reception of control, discovery, and broadcast messages
respectively. It also has the function
StartRandomInitialSubframeIndication for triggering the
SubframeIndication method by randomly choosing the frame and subframe
number. Additionally, the function SetDownlinkCqiPeriodicity sets the
periodicity for the downlink periodic wideband (P10) and aperiodic
subband (A30) CQI reporting. The function InitializeDiscRxPool is used
to initialize the discovery reception pool taking in appropriate frame
number and subframe number as parameters.
 
To differentiate between sending uplink and Sidelink MAC PDUs,
separate functions DoSendMacPdu and DoSendSlMacPdu respectively have
been used. To aid in packet burst transfer, the function
GetSlPhyParameters has been created to get the transmission parameters
for the given packet burst. Occasionally, the UE PHY informs the MAC
through the NotifyUlTransmission function that other transmissions
with higher priority needs to occur and that the MAC should not
schedule any Sidelink transmissions.


The ``LteUePhy`` class is extensively edited to support all the
three ProSe services.

Sidelink direct communication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``LteUePhy`` is now capable of receiving and transmitting Sidelink control
messages. In particular, DCI format 5, and SCI format 0 is now supported
[TS36212]_. The UE PHY receives the information related to the Sidelink
communication pool from the UE RRC in order to compute the boundaries of an SC
period to correctly map the frame/subframe and the RBs associated with the PSCCH
and PSSCH. At the beginning of each subframe, it checks if the Sidelink
transmission pool is added, it make sure to initialize it in order to compute
exactly the frame and subframe number of the next SC period. This is needed to
remove the Sidelink transmission grant associated to the previous SC period,
since a grant is valid only for one SC period.

Sidelink direct discovery
^^^^^^^^^^^^^^^^^^^^^^^^^

For the discovery, the ``LteUePhy`` now also supports the transmission
and reception of the discovery messages. A UE upon the reception of a
discovery message, simply passes it to the MAC layer, which in turn
delivers it to the RRC for further filtering.

The ``LteUePhy`` was also extended to support the measurements storage, layer-1
filtering, and report to upper layers of the Sidelink Discovery Reference
Signals Received Power (SD-RSRP), which is used for UE-to-Network Relay 
(re)selection in the model.  
When a Remote UE starts the UE-to-Network Relay service, the ``LteUeRrc`` 
notifies the ``LteUePhy`` to enable the SD-RSRP measurements. The ``LteUePhy``
then calculates the SD-RSRP of each received discovery message and stores them
indexed by Relay UE ID and Service Code in the function ``ReceivePsdchSdRsrp``.
The SD-RSRP measurements are reported to the ``LteUeRrc`` periodically.
A layer-1 filtering measurement period is defined as 4 discovery periods. 
Given that the length of the discovery period is configurable, the function
``GetUeSdRsrpMeasurementFilterPeriod`` calculates the layer-1 filtering 
measurement period, and the reporting function ``ReportUeSdRsrpMeasurements``
is scheduled to be executed with this periodicity. This function averages the
SD-RSRP of each Relay ID and Service Code that were stored during a layer-1
measurement period, and then reports these quantities to the ``LteUeRrc``.
The trace ``LteUePhy::ReportUeSdRsrpMeasurements`` was also added, which can be
hooked up with a custom function to monitor the SD-RSRP measurements values.   

.. _sec-lte-sl-ue-phy-sync:

Sidelink synchronization
^^^^^^^^^^^^^^^^^^^^^^^^

The ``LteUePhy`` is extended to support Sidelink synchronization functionality. In particular, it supports:

 * Transmission and reception of SLSS
 * Scheduling of SLSS scanning periods
 * Measuring S-RSRP and reporting it to the RRC layer
 * Synchronizing to the chosen SyncRef
 * Performing change of timing
 * Scheduling of the SyncRef re-selection process

In the current implementation, all the UEs by default are perfectly
synchronized, i.e., all the UEs in a simulation upon being initialized pick the
same frame and subframe 1 to start with. Therefore, to simulate synchronization
and to make every UE to pick a random frame and subframe number a new attribute
"UeRandomInitialSubframeIndication" is introduced. We note that, in our model a
UE being synchronized to a SyncRef refers to a state, where both the UEs have
similar SLSS-ID and are aligned on frame and subframe level. The SLSS is
represented by MIB-SL, which is modeled as a broadcast message. MIB-SL
is implemented as per the standard, [TS36331]_, with additional metadata fields:
SLSSID, MIB-SL creation time, MIB-SL reception time and the reception offset.
The S-RSRP of the SLSS received by a UE is computed as explained in the section
lte-design:ref:`sec-phy-ue-measurements`. Only those SLSS are considered
detected, which have the S-RSRP greater than the configured minimum S-RSRP and
for which MIB-SL has been decoded successfully. The minimum S-RSRP is configured
through the attribute "MinSrsrp". The modeled synchronization process can be
categorized by three sequential processes:

 #. Scanning
 #. Measurement
 #. Evaluation

To tailor these processes, the following new attributes have been introduced in
the ``LteUePhy`` class.

 * UeSlssScanningPeriod
 * UeSlssInterScanningPeriodMin
 * UeSlssInterScanningPeriodMax
 * UeSlssMeasurementPeriod
 * UeSlssEvaluationPeriod
 * NSamplesSrsrpMeas

For more information about these attributes the reader is referred to the API of
``LteUePhy`` class. After the scanning process, only 6 detected SyncRefs with the
highest S-RSRP are measured during the measurement period. Moreover, if a UE
receives multiple SLSS from different UEs but have the same SLSS-ID and the
reception offset, they are considered as different S-RSRP samples of the same
SyncRef. The start of the measurement and evaluation processes are subject to
the detection of at least 1 SynchRef during the scanning process. At least one
SyncRef selection process within 20 seconds is scheduled as per the standard
[TS36331]_. The MIB-SL is transmitted with a fixed periodicity of 40 ms
[TS36331]_. The ``LteUeRrc`` class is responsible of scheduling and delivering
the MIB-SL to ``LteUePhy`` class. Once a suitable SyncRef is selected, the
change of timing is performed upon subframe indication, i.e., before the next
subframe to avoid any miss alignments. We also note that the transmission of a
MIB-SL has priority over the transmission of SCI, if they are scheduled in the
same subframe. On the other hand, an MIB-SL cannot be transmitted if a SyncRef
scanning or measurement process is in progress. The ``LteUeMac`` and
``LteUeRrc`` class are notified about the successful change of timings using the
existing SAP interfaces, i.e., ``LteUeCphySapUser`` and ``LteUePhySapUser``
respectively.

LteSpectrumPhy
^^^^^^^^^^^^^^

As mentioned before, the ``LteUePhy`` class includes an additional
``LteSpectrumPhy`` instance to receive Sidelink transmissions from
other UEs.  Therefore, new functions are implemented in
``LteSpectrumPhy`` class to relay the received Sidelink transmission
to the ``LteUePhy`` class. Those functions are RxSlPscch, RxSlPsdch,
and RxSlPsbch (and corresponding callbacks for interconnecting PHY and
MAC) to accomplish reception of control, discovery, and broadcast
messages, respectively. To model the half duplex for Sidelink, since it
is difficult for the UE to receive and transmit at the same time using
the same frequency, a new attribute "CtrlFullDuplexEnabled" is
introduced. This attribute is "false" by default, thus, all the
Sidelink simulations are performed using half duplex mode. 

To notify ``LteUePhy`` about the reception of SLSS a new function
``SetLtePhyRxSlssCallback`` was implemented, which is hooked to the
``ReceiveSlss`` function of ``LteUePhy`` through a callback, while
installing UE devices in the ``LteHelper`` class. 
Similarly, the function ``SetLtePhyRxPsdchSdRsrpCallback`` was implemented to
set the callback that hooks to the ``ReceivePsdchSdRsrp`` function of 
``LteUePhy``, which is used to measure the Sidelink Discovery Reference Signals
Received Power (SD-RSRP) of each received discovery message. 
For other Sidelink signals, i.e., control and data the already implemented
callbacks are utilized.

The ``LteSpectrumPhy`` class is still responsible for evaluating the TB BLER,
however, with the introduction of new physical Sidelink channels, i.e. PSCCH,
PSSCH, PSDCH, and PSBCH, a new physical error model ``LteNistErrorModel`` is
implemented. This error model uses the BLER vs SNR curves for LSM from
[NISTBLERD2D]_, for all the Sidelink physical channels. In particular, these
curves are obtained by extending the LTE toolbox in Matlab and performing Monte
Carlo simulations by considering AWGN channel and SISO mode for transmission. In
the following, we plot these curves for each Sidelink physical channel.

.. _fig-lte-sl-bler-pscch:

.. figure:: figures/lte-sl-bler-pscch.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSCCH (First transmission)

.. _fig-lte-sl-bler-psbch:

.. figure:: figures/lte-sl-bler-psbch.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSBCH

.. _fig-lte-sl-bler-psdch:

.. figure:: figures/lte-sl-bler-psdch.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSDCH

.. _fig-lte-sl-bler-pssch-harq1:

.. figure:: figures/lte-sl-bler-pssch-harq1.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSSCH (HARQ 1)

.. _fig-lte-sl-bler-pssch-harq2:

.. figure:: figures/lte-sl-bler-pssch-harq2.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSSCH (HARQ 2)

.. _fig-lte-sl-bler-pssch-harq3:

.. figure:: figures/lte-sl-bler-pssch-harq3.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSSCH (HARQ 3)

.. _fig-lte-sl-bler-pssch-harq4:

.. figure:: figures/lte-sl-bler-pssch-harq4.*
   :scale: 60 %
   :align: center

   BLER vs SNR of PSSCH (HARQ 4)

Different from the already existing error models, e.g., ``LteMiErrorModel``,
which uses the Mutual Information Based Effective SINR (MIESM) technique for
soft combining process lte-design:ref:`sec-data-phy-error-model`, the
``LteNistErrorModel`` uses a weighted averaging algorithm explained in
[NISTBLERD2D]_ for this purpose. Therefore, a new class ``LteSlHarq`` is
implemented to store the SINR values of each Sidelink transmission that is used
for soft combining process of the retransmission. It is to be noted that, as per
the standard, no HARQ feedback is available for any Sidelink physical channels.
Moreover, to handle interference in D2D scenarios, since all the Sidelink
physical channels employ broadcast solution, a new interference model is
implemented. This is needed because unlike LTE implementation in which unwanted
signals are filtered on the basis of a cell id embedded in transmitted/received
``LteSpectrumSignalParameters``, for D2D scenarios two signals received at the
same time could be intended for a single UE. This overlapping of signal could
occur in out-of-coverage scenarios, where two UEs pick an identical resource to
transmit. To tackle such conditions two new classes, ``LteSlInterference`` and
``LteSlChunkProcessor`` are implemented.

The ``LteSpectrumPhy`` class, at the time of reception maintains a vector to
store the information about the received signal(s). These signals including the
overlapping ones (i.e., they occur at the same time and have equal duration) are
passed to ``LteSlInterference`` class, which maintains the indexing of each
signal by storing them into a vector. Once the signal(s) duration is elapsed, a
call to ``ConditionallyEvaluateChunk`` function is invoked. This function
iterates over this vector and computes the perceived SINR, interference, and the
power of each signal, which are then passed to the respective
``LteSlChunkProcessor`` instance along with their signal id. Similarly, each
``LteSlChunkProcessor`` instance maintain its assigned values by storing them in
a vector as per the signal id. For example, an ``LteSlChunkProcessor``
initialized to store the computed SINR values will maintain a vector containing
the SINR of each overlapping signal as per their signal id. In the end, each
vector is passed to the respective function, which are hooked through a callback
in ``LteHelper`` class at the time of installing the UE device. For instance,
the ``UpdateSlSinrPerceived`` function of ``LteSpectrumPhy`` receive the vector
of perceived SINR values.

Based on the BLER computation in ``LteSpectrumPhy``, the messages received on
Sidelink physical channels can be divided into three types.

 #. The messages whose BLER computation is performed without HARQ, e.g., SCI and MIB-SL

 #. The messages whose BLER computation is performed with HARQ and follow the control information, e.g., Sidelink data

 #. The messages whose BLER computation is performed with HARQ but do not have the control information, e.g., Discovery messages

For first type of messages, we compute the average SINR per RB for each received
TB and sort them in the descending order of the SINR. Then, we try to decode
them by computing the BLER with the help of ``LteNistErrorModel``. For the
second type of signals, we have the transport block information available in
``m_expectedSlTbs`` given by ``LteUePhy`` class. With this information, we
perform a TB to SINR index mapping and retrieve the SINR of the expected TB from
the perceived SINR vector. Then, this SINR and the HARQ info (if it is the
retransmission) along with other parameters are used to compute the BLER of this
message. Finally, the third type of messages, similar to the first type, are
decoded by first storing them in descending order of the SINR, since we do not
have the prior information about the TB. The only difference, is that the
discovery messages can be retransmitted up to 3 times, therefore, we maintain
the HARQ history of each transmission and use it along with other parameters for
computing the BLER. We also note that for type 1 and type 3 messages, if the
received TBs collide, i.e., they use the same RBs, the UE will try to decode the
sorted TBs one at a time, and if any of the TB is decoded the remaining TB(s)
are marked corrupted, thus, are not received by the UE. Alternatively, by
setting the ``DropRbOnCollisionEnabled`` attribute all the colliding TBs can be
dropped irrespective of their perceived SINR.

-----------------
Frequency Hopping
-----------------

The D2D model also supports the frequency hopping on Sidelink PSSCH for the
"UeSelected" resource scheduling, i.e., MODE 2. At the time of writing this
documentation, only the *inter-subframe* hopping mode, with constant (i.e., Type
1) and pseudo-random (Type 2) is supported.

**Note: The documentation for this section will be extended in the later release
of the D2D code. Users, interested to gain further information are referred to**
[NISTFREQHOPP]_


.. _sec-helpers:

-------
Helpers
-------

Four helper objects are used to setup simulations and configure the various
components. These objects are:


 * ``LteHelper``, which takes care of the configuration of the LTE/Sidelink
   radio access network, as well as of coordinating the setup and release of EPS
   and Sidelink radio bearers and start/stop ProSe discovery. The ``LteHelper``
   class provides both the API definition and its implementation.

 * ``EpcHelper``, which takes care of the configuration of the Evolved Packet
   Core. The ``EpcHelper`` class is an abstract base class, which only provides
   the API definition; the implementation is delegated to the child classes in
   order to allow for different EPC network models.
 * ``CcHelper``, which takes care of the configuration of the ``LteEnbComponentCarrierMap``,
   basically, it creates a user specified number of ``LteEnbComponentCarrier``. 
   ``LteUeComponentCarrierMap`` is currently created starting from the 
   ``LteEnbComponentCarrierMap``. ``LteHelper:InstallSingleUeDevice``, 
   in this implementation, is needed to invoke after the ``LteHelper:InstallSingleEnbDevice``
   to ensure that the ``LteEnbComponentCarrierMap`` is properly initialized.
 * ``LteSidelinkHelper``, this helper class is provided to ease the burden of a
   user to configure public safety scenarios involving Sidelink. In particular, it
   uses other helper classes, e.g., LteHelper to activate/deactivate Sidelink
   bearers, create groups for Sidelink broadcast or groupcast communication and
   Lte3gppHexGridEnbTopologyHelper to associate UEs to a Sidelink group or an eNB
   using wrap around method.

**Note: Wrap-around functionality is not fully supported yet.**

It is possible to create a simple LTE-only simulation by using the
``LteHelper`` alone, or to create complete LTE-EPC simulations by using both
``LteHelper`` and ``EpcHelper``. When both helpers are used, they interact in a
master-slave fashion, with the ``LteHelper`` being the Master that interacts
directly with the user program, and the ``EpcHelper`` working "under the hood"
to configure the EPC upon explicit methods called by the ``LteHelper``. The
exact interactions are displayed in the Figure lte-design:ref:`fig-helpers`.

.. _fig-helpers:
   
.. figure:: figures/helpers.*
   :align: center

   Sequence diagram of the interaction between ``LteHelper`` and ``EpcHelper``.

