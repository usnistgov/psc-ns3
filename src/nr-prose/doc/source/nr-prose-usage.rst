NR ProSe Usage
--------------

.. _NrProSeExamples:

Examples
********

A set of example scenarios showcasing the different ProSe functionalities is
present in the nr module in the folder nr/examples/nr-prose-examples. We
discuss them in the following sections depending on the ProSe functionality
they demonstrate.
Please note that the system and SL configuration on these scenario is done
following CTTC's NR and NR V2X examples configuration. Particularly, please
refer to nr/examples/nr-v2x-examples/cttc-nr-v2x-demo-simple and
nr/examples/nr-v2x-examples/nr-v2x-west-to-east-highway for out-of-network and
SL communication configuration and to nr/examples/cttc-nr-bwp-demo for
in-network communication configuration.


5G ProSe direct discovery
=========================

nr-prose-discovery.cc
#####################

This is a simple direct discovery scenario using Model A (announcement).
The default configuration sets up two UEs, both announcing and monitoring
discovery messages. However the number of UEs can be configurable.

**Functionality configuration:**
The discovery application configuration for this scenario is shown below
We first create the NrSlProseHelper instance and configure the UE devices for
ProSe with the PrepareUesForProse function. Then, we establish maps linking UEs
and the ProSe Application Codes intended to be used for both the transmission
and reception of discovery messages, along with the Destination L2 IDs
associated with these UEs/codes.
Then we schedule the discovery start for each UE by specifying the UE ID,
the ProSe Application Code and the role played (either announcing or
monitoring) for the StartDiscovery function from the NrSlProseHelper
class.
Finally, we schedule when and what discovery application to stop
announcing/monitoring using the NrSlProseHelper::StopDiscovery function.

.. sourcecode:: c++

   Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
   nrSlProseHelper->PrepareUesForProse (ueVoiceNetDev);

   std::map<Ptr<NetDevice>, std::list<uint32_t> > announcePayloads;
   std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorPayloads;
   std::map<Ptr<NetDevice>, std::list<uint32_t> > announceDstL2IdsMap;
   std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorDstL2IdsMap;
   for (uint32_t i = 1; i <= ueVoiceNetDev.GetN (); ++i)
   {
     //For each UE, announce one appCode and monitor all the others appCode
     announcePayloads[ueVoiceNetDev.Get (i - 1)].push_back (i);
     announceDstL2IdsMap[ueVoiceNetDev.Get (i - 1)].push_back (100*i);
     for (uint32_t j = 1; j <= ueVoiceNetDev.GetN (); ++j)
       {
         if (i != j)
           {
             monitorPayloads[ueVoiceNetDev.Get (i - 1)].push_back (j);
             monitorDstL2IdsMap[ueVoiceNetDev.Get (i - 1)].push_back (100*j);
           }
       }
   }

   for (uint32_t i = 0; i < ueVoiceNetDev.GetN (); ++i)
   {
     Simulator::Schedule (startDiscTime,
                          &NrSlProseHelper::StartDiscovery,
                          nrSlProseHelper, ueVoiceNetDev.Get (i),
                          announcePayloads[ueVoiceNetDev.Get (i)],
                          announceDstL2IdsMap[ueVoiceNetDev.Get (i)],
                          NrSlUeProse::Announcing);

     Simulator::Schedule (startDiscTime,
                          &NrSlProseHelper::StartDiscovery,
                          nrSlProseHelper, ueVoiceNetDev.Get (i),
                          monitorPayloads[ueVoiceNetDev.Get (i)],
                          monitorDstL2IdsMap[ueVoiceNetDev.Get (i)],
                          NrSlUeProse::Monitoring);

     Simulator::Schedule (stopDiscTime,
                          &NrSlProseHelper::StopDiscovery,
                          nrSlProseHelper, ueVoiceNetDev.Get (i),
                          announcePayloads[ueVoiceNetDev.Get (i)],
                          NrSlUeProse::Announcing);
     Simulator::Schedule (stopDiscTime,
                          &NrSlProseHelper::StopDiscovery,
                          nrSlProseHelper, ueVoiceNetDev.Get (i),
                          monitorPayloads[ueVoiceNetDev.Get (i)],
                          NrSlUeProse::Monitoring);
   }


nr-prose-discovery-l3-relay.cc
##############################

This is a relay discovery scenario with two UEs operating using Model B: one UE
acts as a remote (sending requests looking for a relay to discover in its
vicinity) and the other UE is a relay (responding to the previous requests).

**Functionality configuration:**
This relay discovery configuration can be seen below.
The discovery is set to start at the same designated time, calling the
following function NrSlProseHelper::StartRelayDiscovery. The same Relay Service
Code, Destination L2 ID, and discovery model are defined for both UEs. The only
difference is the assigned role for each UE (relay or remote).

.. sourcecode:: c++

   Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
   nrSlProseHelper->PrepareUesForProse (ueVoiceNetDev);

   uint32_t relayCode = 5;
   uint32_t relayDstL2Id = 500;

   Simulator::Schedule (startDiscTime,
                        &NrSlProseHelper::StartRelayDiscovery,
                        nrSlProseHelper, ueVoiceNetDev.Get (0),
                        relayCode,
                        relayDstL2Id,
                        NrSlUeProse::ModelB,
                        NrSlUeProse::RelayUE);

   Simulator::Schedule (startDiscTime,
                        &NrSlProseHelper::StartRelayDiscovery,
                        nrSlProseHelper,
                        ueVoiceNetDev.Get (1),
                        relayCode,
                        relayDstL2Id,
                        NrSlUeProse::ModelB,
                        NrSlUeProse::RemoteUE);

Both scenarios will generate a discovery trace file with timestamps,
transmitter's and receiver's IDs, and the model/type of messages exchanged.

*Discovery traces:*
Discovery traces are defined in NrSlDiscoveryTrace class under src/nr/helper.

Once the discovery traces are enabled in the scenario, a results file entitled
"NrSlDiscoveryTrace.txt" is created to store the scenario discovery details.
It saves the time (in nanoseconds) a discovery message is sent or received
along with L2 ID sender and receiver information. It also indicates the
discovery type (open or restricted). discovery model (Model A or Model B),
content type (Announcement, Request/Solicitation, or Response depending on
whether it is Model A or Model B and on whether it is a direct discovery or a
relay discovery), and discovery message content (which includes the ProSe
Application Code or Relay Service Code).

Figure :ref:`prose-disc-traces-direct-modelA` and
Figure :ref:`prose-disc-traces-relay-modelB` represent examples of discovery
traces.
In Figure :ref:`prose-disc-traces-direct-modelA`, two UEs are discovering each
other using Model A. At 2 seconds, UE 1 announces its presence to a destination
L2 ID equal to 100 and ProSe Application Code 1, and UE 2 announces to
destination L2 ID equal to 200 using ProSe Application Code 2. UE 1 receives
UE 2's discovery message first, while UE 2 discovers UE 1 four milliseconds
later. And since the discovery interval is set to 2 milliseconds, more discovery
messages are sent at 4 seconds, 6 seconds, and 8 seconds until the end of the
simulation at 10 seconds.
In Figure :ref:`prose-disc-traces-relay-modelB`, the remote UE of L2 ID equal
to 2 is looking for relays in its vicinity using Model B. The UE with L2 ID
equal to 1 receives that request and sends a response to UE 2.


.. _prose-disc-traces-direct-modelA:

.. figure:: figures/prose-disc-traces-direct-modelA.*
   :align: center
   :scale: 100%

   Discovery Traces example - Model A


.. _prose-disc-traces-relay-modelB:

.. figure:: figures/prose-disc-traces-relay-modelB.*
   :align: center
   :scale: 100%

   Discovery Traces example - Model B


nr-prose-discovery-l3-relay-selection.cc
########################################
This scenario is similar to the nr-prose-discovery-l3-relay.cc, however the
relay UEs and remotes are deployed near the cell edge. This scenario also
showcases how to conigure the relay reselection algorithm. As the relay and
remote UEs near the cell edge move, with the default Max RSRP algorithm,
one can see from the output and traces that the remote UE selects the relay
UE with the highest RSRP at any given time.


Unicast mode 5G ProSe direct communication
==========================================

nr-prose-unicast-single-link.cc:
################################

Scenario with two out-of-network UEs that establish a ProSe unicast direct link
over the sidelink and have a unidirectional Constant Bit Rate (CBR) traffic
flow during simulation time.

nr-prose-unicast-multi-link.cc:
###############################

Scenario with three out-of-network UEs that establish ProSe unicast direct
links with each other. A unidirectional CBR traffic from the initiating UE of
each link towards the other UE in the link is activated by default, and
bidirectional traffic can be activated using the corresponding parameter.

nr-prose-network-coex.cc:
#########################

Scenario with a UE doing in-network communication coexisting in parallel with
two other out-of-network UEs doing ProSe unicast direct communication over the
sidelink. The scenario uses one operational band, containing one component
carrier, and two bandwidth parts. One bandwidth part is used for in-network
communication, i.e., UL and DL between the in-network UE and gNBs, and the
other bandwidth part is used for SL communication between the out-of-network
UEs. The traffic comprises bidirectional CBR traffic flows with a Remote Host
in the network for the in-network UE, and bidirectional CBR traffic flows
between the two UEs in the ProSe unicast direct link.

**Output:**
The first two examples will print the Packet Inter-Reception (PIR) per traffic
flow on the standard output and the nr-prose-unicast-multi-link example will
also print the total transmitted and received bits in the system and the
corresponding number of packets. The examples also produce two output files:
one containing sidelink MAC and PHY layer traces in an sqlite3 database created
reusing the framework located in the V2X examples folder; and the other contains
the log of the transmitted and received PC5-S messages used for the
establishment of each ProSe unicast direct link.
In addition to the two above output files, the nr-prose-network-coex example
will print the end-to-end statistics of each traffic flow on the standard
output and write them in a third output file.

**Functionality configuration:**
The portion of code used to configure the ProSe unicast direct communication in
the first example is shown below and it is the baseline used for all scenarios
in this section.
We create the NrSlProseHelper instance and use it to configure the devices for
ProSe and for Unicast direct communication with the PrepareUesForProse and
PrepareUesForUnicast functions, respectively. Then, we call the
EstablishRealDirectLink function for each pair of UEs that will establish a
direct link in the scenario with the following parameters, in order: the
simulation time where the establishment of the direct link should start, the
device of the initiating UE, the IPv4 address of the initiating UE, the device
of the target UE, and the IPv4 address of the target UE.

.. sourcecode:: c++

   Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
   nrSlProseHelper->PrepareUesForProse (ueVoiceNetDev);
   nrSlProseHelper->PrepareUesForUnicast (ueVoiceNetDev);
   nrSlProseHelper->EstablishRealDirectLink (startDirLinkTime,
                                             ueVoiceNetDev.Get (0),
                                             remoteAddress1,
                                             ueVoiceNetDev.Get (1),
                                             remoteAddress2);


5G ProSe L3 UE-to-network relay
===============================

nr-prose-l3-relay.cc:
#####################

Scenario with some UEs doing direct in-network communication, some UEs doing
ProSe unicast communication over SL and also indirect in-network communication
through an L3 UE-to-Network (U2N) relay UE. CBR bidirectional traffic is
performed over the ProSe unicast direct link, as well as between each UE doing
in-network communication and a Remote Host in the network.

nr-prose-l3-relay-on-off.cc:
############################

Scenario with some UEs doing direct in-network communication and some
out-of-network UEs (remote UEs) doing indirect in-network communication through
ProSe L3 UE-to-Network (U2N) relay UEs. The number of in-network only UEs, relay
UEs, and remote UEs can be configured using the corresponding parameters. Each
UE has two traffic flows: one from a Remote Host on the internet towards the UE
(DL), and one from the UEs towards the Remote Host (UL). Each traffic flow is
controlled by an On-Off application that generates CBR traffic during the "On"
periods, and no packets during the "Off" periods.

**Output:**
The above examples will print the traffic flow configurations on the standard
output and once the simulation is done, they will print the end-to-end
statistics of each traffic flow, as well as the number of packets relayed by
each L3 U2N relay UE towards each link for each flow. They also produce several
output files. As in the unicast examples, each scenario produces an output file
with the sidelink MAC and PHY layer traces database, and another output file
containing the log of the transmitted and received PC5 signaling messages used
for the establishment of each ProSe unicast direct link between remote UEs and
relay UEs. Additionally, an output file is generated with the log of the data
packets relayed by the relay UEs at the NAS layer and another output file
contains the end-to-end statistics of each traffic flow.
The nr-prose-relay-on-off scenario also produces a gnuplot script to generate
a plot of the topology, and a log of the application layer packet delay.

If the netsimulyzer module [nist-Netsimulyzer]_ is part of the ns-3 tree, the
nr-prose-relay-on-off scenario generates a JSON file that can be loaded in the
Netsimulyzer application. Users will be able to visualize the topology,
reproduce the simulation, and examine timeline plots and eCDF plots of the
different performance metrics calculated for each scenario, including
throughput and media packet delay.

**Functionality configuration:**
The portion of code used to configure the L3 UE-to-network relay functionality
in the above examples is shown below and it is the baseline used for all
scenarios in this section.
We create the NrSlProseHelper instance, set the EPC helper on it, and configure
all the devices for ProSe and for Unicast direct communication with the
PrepareUesForProse and PrepareUesForUnicast functions, respectively.
We configure the relay service code that the relay UEs will provide, the
network TFT and the EPS bearer the relay UEs will use for the relayed traffic.
Then we apply these configurations on the devices acting as relay UEs with the
function ConfigureL3UeToNetworkRelay provided by the NrSlProseHelper.
Finally, we call the function EstablishL3UeToNetworkRelayConnection to
configure the relay connection between each remote UE and relay UE with the
following parameters, in order: the simulation time where the establishment of
the direct link should start, the device of the initiating UE (the remote UE),
the IPv4 address of the initiating UE (remote UE), the device of the target UE
(relay UE), the IPv4 address of the target UE (relay UE), and the relay service
code to be used for the connection.

.. sourcecode:: c++

   Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
   nrSlProseHelper->SetEpcHelper (epcHelper);

   nrSlProseHelper->PrepareUesForProse (relayUeNetDev);
   nrSlProseHelper->PrepareUesForProse (remoteUeNetDev);

   nrSlProseHelper->PrepareUesForUnicast (relayUeNetDev);
   nrSlProseHelper->PrepareUesForUnicast (remoteUeNetDev);

   uint32_t relayServiceCode = 5;
   std::set<uint32_t> providedRelaySCs;
   providedRelaySCs.insert (relayServiceCode);

   Ptr<EpcTft> tftRelay = Create<EpcTft> ();
   EpcTft::PacketFilter pfRelay;
   tftRelay->Add (pfRelay);
   enum EpsBearer::Qci qciRelay;
   qciRelay = EpsBearer::GBR_CONV_VOICE;
   EpsBearer bearerRelay (qciRelay);

   nrSlProseHelper->ConfigureL3UeToNetworkRelay (relayUeNetDev, providedRelaySCs,
                                                 bearerRelay, tftRelay);

   for (uint32_t i = 0; i < remoteUeNodes.GetN (); ++i)
     {
     for (uint32_t j = 0; j < relayUeNetDev.GetN (); ++j)
       {
         nrSlProseHelper->EstablishL3UeToNetworkRelayConnection (
                                                   startRelayConnTime,
                                                   remoteUeNetDev.Get (i),
                                                   remotesIpv4AddressVector [i],
                                                   relayUeNetDev.Get (j),
                                                   relaysIpv4AddressVector [j],
                                                   relayServiceCode);
       }
     }

.. [nist-Netsimulyzer] Evan Black, Samantha Gamboa and Richard Rouil, "Netsimulyzer: A 3d network simulation analyzer for ns-3," in Proceedings of the Workshop on ns-3, WNS3 ’21, p. 6572, 2021.

5G ProSe L3 UE-to-UE relay
==========================

nr-prose-olsrv2.cc:
###################

This scenario shows the use of ProSe unicast communication and L3 U2U relays
using OLSRv2/NHDP via the MANET service. This scenario consists of six UEs,
each participating in the MANET, with data traffic flowing from UE 1 to UE 4.
The UEs are deployed in a matrix/grid orientation with each node separated by
100 m with its adjacent neighbors. The first row consists of UE 1, UE 2, UE 3,
and UE 4 in columns 1, 2, 3, and 4, respecitely, while the second row consists
of UE 5 and UE 6 in columns 2 and 3. A matrix channel model is used to ensure
that UEs only receive valid signal from adjacent neighbors in its initial
state. Thus, there are two paths from from UE 1 to UE 4: UE 1 -> UE 2 -> UE 3
-> UE 4 and UE 1 -> UE 2 UE 5 -> UE 6 -> UE 3 -> UE 4. In this scenario both
routes are discovered, however, it is configured in the scenario to "break"
the link between UE 2 and UE 3, thus data flows on the first path for some
then on the second path, and again on the first path once the connection
between UE 2 and UE 3 is re-established.

**Output:**

The output of this scenario includes the total number of bits and packets
sent by UE 1, and the total number of bits and packets received by UE 4. It
also includes the average throughtput and average Packet Inter-Reception (PIR)
of all of the received packets.

If this program is run with the NS_LOG component "NrProseOlsrv2", a few events
are noted, including, times that the link between 2 and 3 are broken and restored:

.. sourcecode:: c++

    +20.000000000s -1 NrProseOlsrv2:BreakLink(): [INFO ] Breaking link between 2 and 3
    +60.000000000s -1 NrProseOlsrv2:RestoreLink(): [INFO ] Restoring link between 2 and 3

In addition, it can be observed in nr-prose-olsrv2.routes that the route to node 4
from node 2 goes through next hop 3 at time 20 seconds, but later at time 60 seconds,
the route to node 4 goes through node 5 as observed in the routing table on node 2.
By the end of the simulation (99 seconds), the routing table for node 2 again
lists node 3 as the next hop for reaching node 4.

This example also writes to the NrSlDiscoveryTrace.txt and
NrSlPc5SignallingPacketTrace.txt files to capture discovery and other PC5 signalling.
As well as nr-prose-olsrv2-n*-i1.cap PCAP files and a
default-nr-prose-inicast-single-link.db database with MAC and PHY statistics

**RSRP for RLF**
The following lines of code in the scenario make it possible to use the SL-RSRP
measurements to detect RLF which can result in route changes in addition to
OLSRv2/NHDP detection mechanisms.

.. sourcecode:: c++

    NrSlRrcSap::SlRemoteUeConfig slRemoteConfig;
    slRemoteConfig.slReselectionConfig.slRsrpThres = -110; // dBm
    slRemoteConfig.slReselectionConfig.slFilterCoefficientRsrp = 0.5;
    slRemoteConfig.slReselectionConfig.slHystMin = 10; // dB

    // Define SD-/SL-RSRP thresholds and enable RSRP measurements for UEs
    for (uint32_t i = 0; i < ueVoiceNetDev.GetN(); ++i)
    {
        Ptr<NrSlUeRrc> remoteRrc =
            ueVoiceNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetRrc()->GetObject<NrSlUeRrc>();
            remoteRrc->EnableUeSlRsrpMeasurements();
            remoteRrc->SetNrSlDiscoveryRemoteConfiguration(slRemoteConfig);
    }

    Config::SetDefault("ns3::NrSlL3ManetService::UseSlRsrpForRlf", BooleanValue(true));

nr-prose-olsrv2-rlf.cc:
#######################
This scenario was originally adopted from the `nr-prose-olsrv2.cc` in which
only two nodes exist with traffic from UE 1 -> UE 2. This simplified scenario
exists to explore the effects of the various RLF detection procedures, which
includes the ProSe keep-alive, SD-/SL-RSRP threshold, and HARQ failures. This
scenario includes the following parameters relavent to RLF:

.. sourcecode:: c++

    --breakLinkTime:           The time at which to "break" the link [+20s]
    --restoreLinkTime:         The time at which to restore the link [+1min]
    --nominalPathloss:         The initial and restored pathloss (dB) to use for the link [0]
    --adjustedPathloss:        The adjusted pathloss (dB) to use to "break" the link [1000]
    --useSdRsrp:               Indicates if SD-RSRP measurements should be used for RLF detection [false]
    --useSlRsrp:               Indicates if SL-RSRP measurements should be used for RLF detection [false]
    --useHarq:                 Indicates if HARQ-feedback should be used [false]
    --rsrpThreshold:           The SD-RSRP and/or SL-RSRP RSRP (dBm) threshold [-110]
    --rsrpCoefficient:         The SD-RSRP and/or SL-RSRP coefficent [0.5]
    --rsrpHysteresis:          The SD-RSRP and/or SL-RSRP hysteresis (dB) [10]
    --slMaxNumConsecutiveDtx:  The maximum number of PSFCH absences to tolerate before declaring RLF [0]
    --t5084:                   The value to use for the ProSe keep-alive procedure timer [+5s]
    --t5085:                   The value to use for the ProSe keep-alive request retransmission timer [+5s]

With this scenario users can adjust the time at which the link is broken, the
pathloss that is used when the link is broken, and the time at which to
restore the link to it's initial state. Breaking and restoring the link is
achieved by changing the pathloss to the adjusted and nominal values,
respectively. The adjusted pathloss is only applied when the link is
designated to be in a broken state, while the nominal pathloss is applied to
the initial state and when it is restored from a brken state. The options to
indicate if SD-RSRP, SL-RSRP, and HARQ should be used provide control over
whether collected RSRP measurements are used to determine RLF, as well as if
HARQ feedback should be used for the unicast links created for data exchange.
The RSRP threshold, RSRP coefficient, and RSRP hysteresis options allow the
user to adjust when either SD-RSRP or SL-RSRP measurements meet the desired
measurement value for determining RLF when either SL-RSRP or SD-RSRP is used.
See 3GPP TS 38.331 Section 5.5.3.2 [TS38331]_ for more information on this.
The variable used to indicate the maximum number of PSFCH absences to
tolerate is used to determine when to declare RLF when HARQ feedback is used
in this scenario. A value of 0 disables the enforcement of this maximum,
meaning that an infinite amount of PSFCH feedback absences will be tolerated.
The ProSe keep-alive parameter T5084 is used to control how often a ProSe
keep-alive procedure should be initiated which is restarted after a successful
keep-alive procedure or data is received fora  particular destination. The
T5085 timer is used to indicate the perodicity of a ProSe keep-alive request
message retransmission after the keep-alive procedure is initiated.

**Output:**
Given that the scenario is intended to study RLF detection, the RlfTrace.txt
can be used to trace when RLF events are detected. Therefore, the user can
use this trace in combination with the specified parameters to determine
what mechanisms detected the RLF and how long it took for that mechanism to
do so. Below is an example of output captured in the trace.

.. sourcecode:: c++

    time(s) srcL2Id dstL2Id event
    44.7651 1       2       keep-alive
    44.9271 2       1       keep-alive

As can be seen, this trace includes the time at which RLF was detected, the
source L2 ID of the node that detected the link failure, the destination L2
ID that the link was for, and the mechanism that detected the failure. The
currently supported mechanisms include, "keep-alive" which corresponds to the
ProSe keep-alive procedure, "harq" which corresponds to consecutive PSFCH
absences, "sd-rsrp" which corresponds to the RSRP of a discovery message
falling below the threshold, and "sl-rsrp" which corresponds to the RSRP of
any other PC5 message falling below the threshold.
