.. _NrProSeExamples:

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


nr-prose-discovery-relay.cc
###########################

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

nr-prose-network-l3-relay.cc:
#############################

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

