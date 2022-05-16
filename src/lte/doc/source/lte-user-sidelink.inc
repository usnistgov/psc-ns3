------------
LTE Sidelink
------------

A new directory "d2d-examples" in the LTE module examples directory holds the
examples for simulating ProSe services, i.e., direct communication, direct
discovery, synchronization, and UE-to-Network Relay. 
These examples can be divided into the following two categories:

1. Simple examples 

These examples cover the different ProSe services using  
simple scenarios. The examples included in this category are the following:

* *Direct communication:*

  * lte-sl-in-covrg-comm-mode1
  * lte-sl-in-covrg-comm-mode2
  * lte-sl-out-of-covrg-comm

* *Direct discovery:*

  * lte-sl-in-covrg-discovery
  * lte-sl-in-covrg-discovery-multi
  * lte-sl-in-covrg-discovery-collision
  * lte-sl-out-of-covrg-discovery-collision

* *Synchronization:*

  * lte-sl-out-of-covrg-synch
  * lte-sl-out-of-covrg-synch-twotx

* *UE-to-Network Relay:*

  * lte-sl-in-covrg-relay
  * lte-sl-in-covrg-relay-building
  * lte-sl-relay-cluster
  * lte-sl-relay-cluster-selection

* *Multicell scenarios:*

  * lte-sl-in-covrg-comm-mode1-multicell
  * lte-sl-in-covrg-comm-mode2-multicell
  * lte-sl-in-covrg-discovery-multicell
  * lte-sl-in-covrg-relay-mode2-multicell

2. Detailed examples 

These examples can be used to simulate out-of-coverage ProSe communication,
discovery and synchronization. In particular, these examples covers the
simulation scenarios used for the study published in [NIST2017]_. The users
interested in the extensive simulation campaigns described in this study, are
referred to a "README.txt" file for more information. The examples included in
this category are:

* wns3-2017-pscch
* wns3-2017-pssch
* wns3-2017-synch
* wns3-2017-discovery

The Sidelink example scripts follow the same rules as writing an LTE simulation
script, however, there are additional configurations required to simulate
Sidelink. Following are some rules of thumb for writing Sidelink scripts;

* Sidelink simulations require EPC.
* eNB should be disabled to simulate out-of-coverage scenarios.
* Sidelink pools should be configured before the start of the simulation.
* ``Lte3gppHexGridEnbTopologyHelper`` cannot be use without initializing eNB
  nodes and appropriate antenna model. * It is possible to avoid the
  initialization of eNB nodes in out-of-coverage scenarios, if hexagonal ring
  topology is not used.

In general, all the D2D examples are highly parameterizable and could be divided
in the following parts:

 #. Configuration of LTE and Sidelink default parameters, e.g., the ones
    configured by calling ``Config::SetDefault`` function

 #. Topology configuration

 #. Sidelink pool configuration
 
 #. IP configuration (if any)
 
 #. Application configuration (if any)

 #. Sidelink bearer configuration (if any)
 
 #. Service configuration (if any)
 
In the following, we will discuss four examples from the "Simple examples"
category (i.e., lte-sl-in-covrg-comm-mode1, lte-sl-in-covrg-comm-mode2, 
lte-sl-out-of-covrg-comm, and lte-sl-in-covrg-relay) and
two examples from the "Detailed examples" category, which are
wns3-2017-synch and wns3-2017-discovery. We choose wns3-2017-synch, since it
also simulates the PSCCH and PSSCH.

lte-sl-in-covrg-comm-mode1
++++++++++++++++++++++++++

This example simulates an in-coverage MODE 1 ProSe communication by using the
following scenario.

.. _fig-lte-sl-simple-scenario:
 
.. figure:: figures/lte-sl-simple-scenario.*
   :align: center

   Sidelink simple in-coverage scenario

Both the UEs are in-coverage of the eNB where UE1 sends the data to UE2 via
Sidelink by using the resources assigned by the eNB. A user can configure the
simulation time and the output of NS logs of the specified classes by using the
corresponding command line variables in the simulation script. For example, a
user can run the simulation as follows::

./waf --run "lte-sl-in-covrg-comm-mode1 --simTime=7 --enableNsLogs=false"

The simulation time is in seconds. Moreover, this example can support IPv6
instead of the default IPv4 if the ``--useIPv6`` command-line argument
is provided.

.. highlight:: none


Configuration of LTE and Sidelink default parameters
----------------------------------------------------

The simulation script begins with the configuration of the parameters of the
Sidelink scheduler as follows.
 
* Configure the Sidelink scheduler : ::

   Config::SetDefault ("ns3::RrSlFfMacScheduler::Itrp", UintegerValue (0));
   Config::SetDefault ("ns3::RrSlFfMacScheduler::SlGrantSize", UintegerValue (5));

The ``ns3::RrSlFfMacScheduler`` is a very simple round robin scheduler, which
uses a fixed TRP index value and number of RBs to be allocated to a UE.

   
* Configure the frequency and the bandwidth : ::

   Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
   Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
   Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
   Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
   Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

We use the LTE Band 1 for both LTE and Sidelink communication.

* Configure the error models : ::
   
   // For PSSCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled",
                                                        BooleanValue (true));

   // For PSCCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled",
                                                        BooleanValue (true));

   Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled",
							BooleanValue (false));

* Configure the transmit power of the eNB and the UEs : ::

   Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
   Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

The powers configured are in dBm.

Topology configuration
----------------------

* Instantiating LTE, EPC and Sidelink helpers : ::

   Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
   Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
   lteHelper->SetEpcHelper (epcHelper);

   Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
   proseHelper->SetLteHelper (lteHelper);

* Configuring the pathloss model : ::

   lteHelper->SetAttribute ("PathlossModel", 
				StringValue ("ns3::Cost231PropagationLossModel"));

* Enabling the Sidelink : ::
   
   lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

**Note : Attribute ``UseSidelink`` must be set before installing the UE devices.**

* Configuring the scheduler : ::

   lteHelper->SetSchedulerType ("ns3::RrSlFfMacScheduler");

* Creating the eNB and UE nodes and setting their mobility : ::

   NodeContainer enbNode;
   enbNode.Create (1);
   NodeContainer ueNodes;
   ueNodes.Create (2);

   Ptr<ListPositionAllocator> positionAllocEnb = 
					CreateObject<ListPositionAllocator> ();
   positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
   Ptr<ListPositionAllocator> positionAllocUe1 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe1->Add (Vector (10.0, 0.0, 1.5));
   Ptr<ListPositionAllocator> positionAllocUe2 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe2->Add (Vector (-10.0, 0.0, 1.5));

   //Install mobility
   MobilityHelper mobilityeNodeB;
   mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
   mobilityeNodeB.Install (enbNode);

   MobilityHelper mobilityUe1;
   mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe1.SetPositionAllocator (positionAllocUe1);
   mobilityUe1.Install (ueNodes.Get (0));

   MobilityHelper mobilityUe2;
   mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe2.SetPositionAllocator (positionAllocUe2);
   mobilityUe2.Install (ueNodes.Get (1));

* Installing LTE devices to the nodes : ::

   NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
   NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

Sidelink pool configuration
---------------------------

This example simulates an in-coverage scenario, therefore, the eNB will be
configured with a pre-configured dedicated Sidelink pool. As mentioned in
:ref:`sec-lte-sl-rrc`, in this scenario the ``LteSlEnbRrc`` class will be
responsible for holding this per-configured pool configuration. The pool
configuration starts by setting a flag in ``LteSlEnbRrc`` as an indication
that the Sidelink is enabled. It is configured as follows::

   Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
   enbSidelinkConfiguration->SetSlEnabled (true);

For configuring Sidelink parameters, the "setup" structure of the field
"commTxResources-r12" of IE "SL-Preconfiguration" defined in the standard
[TS36331]_ is converted into a C++ structure. This example uses this structure
to configure the pool parameters for Sidelink control. The pool configuration is
done by using the ``LteSlPreconfigPoolFactory`` in the following manner, ::

   
   LteRrcSap::SlCommTxResourcesSetup pool;

   pool.setup = LteRrcSap::SlCommTxResourcesSetup::SCHEDULED;
   //BSR timers
   pool.scheduled.macMainConfig.periodicBsrTimer.period = 
                                              LteRrcSap::PeriodicBsrTimer::sf16;
   pool.scheduled.macMainConfig.retxBsrTimer.period = LteRrcSap::RetxBsrTimer::sf640;
   //MCS
   pool.scheduled.haveMcs = true;
   pool.scheduled.mcs = 16;
   //resource pool
   LteSlResourcePoolFactory pfactory;
   pfactory.SetHaveUeSelectedResourceConfig (false); //since we want eNB to schedule

   //Control
   pfactory.SetControlPeriod("sf40");
   pfactory.SetControlBitmap(0x00000000FF); //8 subframes for PSCCH
   pfactory.SetControlOffset(0);
   pfactory.SetControlPrbNum(22);
   pfactory.SetControlPrbStart(0);
   pfactory.SetControlPrbEnd(49);

   pool.scheduled.commTxConfig = pfactory.CreatePool ();

   uint32_t groupL2Address = 255;

   enbSidelinkConfiguration->AddPreconfiguredDedicatedPool (groupL2Address, pool);
   lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

The resources for data are computed by the scheduler on the basis of the
scheduler's attributes configured in the start of this simulation script and
the above pool configuration.

Similarly, for the UEs we need to enable the Sidelink in ``LteSlUeRrc`` by
setting a flag and a pre-configuration object, which will be initialized with
the pool configurations once the UE receives an ``RrcConnectionReconfiguration``
message from the eNB, as shown in Figures :ref:`fig-lte-sl-in-coverage-tx`
and :ref:`fig-lte-sl-in-coverage-rx`::

  //pre-configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

IP configuration
----------------

* Installing the IP stack on the UEs and assigning IP address : ::

   //Install the IP stack on the UEs and assign IP address
   InternetStackHelper internet;
   internet.Install (ueNodes);
   Ipv4InterfaceContainer ueIpIface;
   ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

   // set the default gateway for the UE
   Ipv4StaticRoutingHelper ipv4RoutingHelper;
   for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
     {
       Ptr<Node> ueNode = ueNodes.Get (u);
       // Set the default gateway for the UE
       Ptr<Ipv4StaticRouting> ueStaticRouting = 
                ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
       ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (),
								1);
     }

The above configuration is similar to any usual LTE example with EPC. 

* Attaching the UEs to the eNB : ::

   lteHelper->Attach (ueDevs);

Application configuration
-------------------------

* Installing applications and activating Sidelink radio bearers. Note that
  two configuration options (IPv4 and IPv6) are supported; we only document
  below the IPv4 path through the code : ::

   Ipv4Address groupAddress4 ("225.0.0.0"); //use multicast address as destination
   ...
   remoteAddress = InetSocketAddress (groupAddress4, 8000);
   localAddress = InetSocketAddress (Ipv4Address::GetAny (), 8000);
   ...
   OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
   sidelinkClient.SetConstantRate (DataRate ("16kb/s"), 200);

   ApplicationContainer clientApps = sidelinkClient.Install (ueNodes.Get (0));
   //onoff application will send the first packet at :
   //(2.9 (App Start Time) + (1600 (Pkt size in bits) / 16000 (Data rate)) = 3.0 sec
   clientApps.Start (slBearersActivationTime + Seconds (0.9));
   clientApps.Stop (simTime - slBearersActivationTime + Seconds (1.0));

   ApplicationContainer serverApps;
   PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", localAddress);
   serverApps = sidelinkSink.Install (ueNodes.Get (1));
   serverApps.Start(Seconds (2.0));

In this example, an "OnOff" application is installed in the UE 1, which sends a
200 byte packet with the constant bit rate of 16 kb/s. On the other hand, the UE
2 is configured with the "PacketSink" application.

The Sidelink radio bearers are activated by calling the ``ActivateSidelinkBearer``
method of ``LteSidelinkHelper`` as follows: ::

   //Set Sidelink bearers
   Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, 
                                                     groupAddress4, groupL2Address);
   proseHelper->ActivateSidelinkBearer (Seconds (2.0), ueDevs, tft);

The ``ActivateSidelinkBearer`` method takes the following three parameters as input,

    #. Time to activate the bearer

    #. Devices for which the bearer will be activated

    #. The Sidelink traffic flow template

The ``tft`` in this example is an object of ``LteSlTft`` class created by
initializing its following parameters,

    #. Direction of the bearer, i.e., "Transmit", "Receive" or Bidirectional

    #. An IPv4 multicast address of the group

    #. Sidelink layer 2 group address (used as Sidelink layer 2 group id)

* Activating the Sidelink traces : ::

   lteHelper->EnableSlPscchMacTraces ();
   lteHelper->EnableSlPsschMacTraces ();

   lteHelper->EnableSlTxPhyTraces ();
   lteHelper->EnableSlRxPhyTraces ();
   lteHelper->EnableSlPscchRxPhyTraces ();

The above code will enable all the Sidelink related traces. We note that, the
user can also use the classical function "LteHelper::EnableTraces ()", but this
will also output the LTE traces.

Upon the completion of the simulation, the following trace files can be found in
the repository's root folder,

* SlCchMacStats.txt
* SlSchMacStats.txt
* SlCchRxPhyStats.txt
* SlRxPhyStats.txt

For more information related to the above files please refer to the
:any:ref:`sec-sidelink-simulation-output` section.

* UePacketTrace.tr

  The information in this file is obtained by using the traces "TxWithAddresses"
  and "RxWithAddresses" of  ``OnOff`` and ``PacketSink`` application, respectively.
  Following table shows the snippet of the data from this file for the two UEs,

  .. tabularcolumns:: |l|l|l|l|l|l|l|
  .. list-table:: UePacketTrace.tr
   :header-rows: 1

   * - Time (sec)
     - tx/rx
     - NodeID
     - IMSI
     - PktSize (bytes)
     - IP[src]
     - IP[dst]
   * - 3
     - tx
     - 4
     - 1
     - 200
     - 7.0.0.2:49153
     - 225.0.0.0:8000
   * - 3.09693
     - rx
     - 5
     - 2
     - 200
     - 7.0.0.2:49153
     - 7.0.0.3:8000

The first row shows that the UE 1 with IMSI 1 transmits a multicast
packet of 200 bytes and the UE 2 receives the packet transmitted by the UE 1.
As per the client's application data rate and ON time, i.e., 16 kb/s and 2 seconds,
respectively, a total of 20 packets are sent and received by the transmitting and
the receiving UE.

lte-sl-in-covrg-comm-mode2
++++++++++++++++++++++++++

This example simulates an in-coverage MODE 2 ProSe communication using the same
scenario :ref:`fig-lte-sl-simple-scenario`, used for previous example. The only
difference is that the resources are not scheduled by the eNB, instead, the UE
1 selects the resources autonomously from a pool specified by the eNB through
``RrcConnectionReconfiguration`` message. Besides using the same scenario, this
example also has the same application, therefore, in the following, we will
discuss only those configurations, which are MODE 2 specific. A user can run the
simulation as follows::

./waf --run "lte-sl-in-covrg-comm-mode2 --simTime=7 --enableNsLogs=false"

The simulation time is in seconds. Similarly, this example supports IPv6 as
an option if the ``--useIPv6`` argument is provided.

.. highlight:: none

Configuration of LTE and Sidelink default parameters
----------------------------------------------------

* Configuring parameters for PSSCH resource selection : ::
  
   // Fixed MCS and the number of RBs
   Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
   Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5));

   // For selecting subframe indicator bitmap
   Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
   //use default Trp index of 0 
   Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true));

The above parameters of ``LteUeMac`` class enable the UE to select the time
(i.e., frame/subframe) and the frequency (i.e., RBs) resources autonomously. If
the attribute "UseSetTrp" is false, a UE will select the TRP index randomly from
the range of values depending on the KTRP value [TS36213]_.

* Configure the error models : ::

   // For PSSCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled",
                                                        BooleanValue (true));

   // For PSCCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled",
                                                        BooleanValue (true));
  
   Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", 
							BooleanValue (false));

Along with the error model configuration, by setting the "DropRbOnCollisionEnabled"
attribute, all the TBs collided in time and frequency are dropped. This attribute
could be of particular interest for the users, e.g., to study the impact of
collisions in MODE 2. In this example, this attribute has no impact as only one
UE is acting as a transmitter. It is included just to make the users aware of
this attribute.

Topology configuration
----------------------

* There is no need to configure the scheduler, since this example uses MODE 2 
  for resource selection.

Sidelink pool configuration
---------------------------

The Sidelink pool configuration is similar to the MODE 1 configuration, e.g.,
the configuration for Sidelink control is the same. However, the following
additional configurations are needed for in-coverage MODE 2. ::

  LteRrcSap::SlCommTxResourcesSetup pool;

  pool.setup = LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  //Data
  pfactory.SetDataBitmap(0xFFFFFFFFFF);
  pfactory.SetDataOffset(8); //After 8 subframes of PSCCH
  pfactory.SetDataPrbNum(25);
  pfactory.SetDataPrbStart(0);
  pfactory.SetDataPrbEnd(49);

Compared to MODE 1, the pool resources are now indicated as "UE_SELECTED" along
with other parameters. Moreover, now we need to configure the pool parameters
related to the data, i.e., PSSCH. In addition to the subframe indicator bitmap
specified by KTRP and iTRP, Mode 2 introduces another level of subframe
filtering for the subframe pool via "DataBitmap" to limit the number of possible
values for iTRP. The PSSCH transmission occurs on the filtered subframes after
applying TRP bitmap on this "DataBitmap". Users interested to learn about how it
is done are referred to :ref:`SidelinkCommPoolPsschTestCase`.

Finally, at the end of the simulation the trace files, similar to the previous
example, can be found in the repository's root folder. One important difference,
compared to MODE 1, is that in the trace files "SlCchMacStats.txt" and
"SlCchRxPhyStats.txt" the parameters, e.g., PSCCH resource index and the
starting RB for PSSCH are randomly selected by the UE for every SC period.
Furthermore, a similar amount of packets, i.e. 20, are sent and received by the
UEs.

lte-sl-out-of-covrg-comm
++++++++++++++++++++++++

This example simulates an out-of-coverage MODE 2 ProSe communication by using
the following scenario.

.. _fig-lte-sl-simple-OofCovrg-scenario:
 
.. figure:: figures/lte-sl-simple-OofCovrg-scenario.*
   :align: center

   Sidelink simple out-of-coverage scenario

This example allows a user to configure the simulation time and the output of NS
logs of the specified classes by using the corresponding command line variables
in the simulation script. For example, a user can run the simulation as
follows::

./waf --run "lte-sl-out-of-covrg-comm --simTime=7 --enableNsLogs=false"

The simulation time is in seconds, and ``--useIPv6`` can be used to run
the example with IPv6 instead of IPv4.

.. highlight:: none

Configuration of LTE and Sidelink default parameters
----------------------------------------------------

* Configuring parameters for PSSCH resource selection : ::
  
   // Fixed MCS and the number of RBs
   Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
   Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5));

   // For selecting subframe indicator bitmap
   Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
   //use default Trp index of 0 
   Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true));

The above parameters of ``LteUeMac`` class enable the UE to select the time
(i.e., frame/subframe) and the frequency (i.e., RBs) resources autonomously. If
the attribute "UseSetTrp" is false, a UE will select the TRP index randomly from
the range of values depending on the KTRP value [TS36213]_.

* Configure the frequency : ::

   uint32_t ulEarfcn = 18100;
   uint16_t ulBandwidth = 50;

  Here, it is not necessary to configure the EARFCNs and the bandwidth of the UE
  and eNB for the two reasons. First, in this example we will not instantiate
  the eNB node, thus, setting these attributes would have no impact. Second,
  both the UEs will use only the Sidelink to communicate, therefore, the EARFCN
  and the bandwidth are specified in the pool configuration. At this stage, the
  above two variables are initialized to be used later to configure the pathloss
  model and the Sidelink pool. Similar, to the previous simple examples, we use
  the LTE Band 1 for Sidelink communication.

* Configure the error models : ::

   // For PSSCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled",
                                                        BooleanValue (true));

   // For PSCCH
   Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled",
                                                        BooleanValue (true));
  
   Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", 
							BooleanValue (false));

Along with the error model configuration, by setting the
"DropRbOnCollisionEnabled" attribute all the TBs collided in time and frequency
are dropped. This attribute could be of particular interest for the users, e.g.,
to study the impact of collisions in MODE 2. In this example, this attribute has
no impact as only one UE is acting as a transmitter. It is included just to make
the users aware of this attribute.

* Configure the transmit power for the UEs : ::

   Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

The power configured is in dBm.

Topology configuration
----------------------

* Instantiating LTE, EPC and Sidelink helpers : ::

   Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
   Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
   lteHelper->SetEpcHelper (epcHelper);

   Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
   proseHelper->SetLteHelper (lteHelper);

* Enabling the Sidelink : ::
   
   lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

**Note : Attribute ``UseSidelink`` must be set before installing the UE devices.**

* Configuring the pathloss model and bypass the use of eNB nodes ::

   (1)

     lteHelper->SetAttribute ("PathlossModel", 
                              StringValue ("ns3::Cost231PropagationLossModel"));

   (2)

     lteHelper->Initialize ();

   (3)

     double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (ulEarfcn); //18100
     NS_LOG_LOGIC ("UL freq: " << ulFreq);
     Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
     Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->
                                             GetObject<PropagationLossModel> ();
     NS_ABORT_MSG_IF (lossModel == NULL, "No PathLossModel");
     bool ulFreqOk = uplinkPathlossModel->
                       SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));

     if (!ulFreqOk)
       {
       NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
       }


The use of eNB nodes can be bypassed by using the above commands strictly in the
order they are listed. The command "lteHelper->Initialize ()" basically performs
the channel model initialization of all the component carriers. Therefore, it is
necessary to configure any desired pathloss model before issuing this command.
The commands in step 3 are to properly configure the frequency attribute of the
pathloss model used, which is normally done in ``InstallSingleEnb`` method of
``LteHelper``.

* Creating the UE nodes and setting their mobility : ::

   NodeContainer ueNodes;
   ueNodes.Create (2);

   Ptr<ListPositionAllocator> positionAllocUe1 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe1->Add (Vector (0.0, 0.0, 1.5));
   Ptr<ListPositionAllocator> positionAllocUe2 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe2->Add (Vector (20.0, 0.0, 1.5));

   //Install mobility
   MobilityHelper mobilityUe1;
   mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe1.SetPositionAllocator (positionAllocUe1);
   mobilityUe1.Install (ueNodes.Get (0));

   MobilityHelper mobilityUe2;
   mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe2.SetPositionAllocator (positionAllocUe2);
   mobilityUe2.Install (ueNodes.Get (1));

* Installing LTE devices to the nodes : ::

   NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

Sidelink pool configuration
---------------------------

This example simulates an out-of-coverage scenario, therefore, both the UEs are
configured with a pre-configured Sidelink communication pool. As mentioned in
:ref:`sec-lte-sl-rrc`, in this scenario the ``LteSlUeRrc`` class will be
responsible for holding this per-configured pool configuration. The pool
configuration starts by setting a flag in ``LteSlUeRrc`` as an indication that
the Sidelink is enabled. It is configured as follows::

 Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
 ueSidelinkConfiguration->SetSlEnabled (true);

For configuring Sidelink communication pre-configured pool parameters, the IE
"SL-Preconfiguration" defined in the standard [TS36331]_ is converted into a C++
structure and similar to the basic LTE layer 3 messages it can be found in
``LteRrcSap`` class. This example uses this structure to configure the Sidelink
communication pool parameters. The pool configuration is done by using the
``LteSlPreconfigPoolFactory`` in the following manner, ::

  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = ulEarfcn; //18100
  preconfiguration.preconfigGeneral.slBandwidth = ulBandwidth; // 50 RBs
  preconfiguration.preconfigComm.nbPools = 1;

  LteSlPreconfigPoolFactory pfactory;

  //Control
  pfactory.SetControlPeriod("sf40");
  pfactory.SetControlBitmap(0x00000000FF); //8 subframes for PSCCH
  pfactory.SetControlOffset(0);
  pfactory.SetControlPrbNum(22);
  pfactory.SetControlPrbStart(0);
  pfactory.SetControlPrbEnd(49);

  //Data
  pfactory.SetDataBitmap(0xFFFFFFFFFF);
  pfactory.SetDataOffset(8); //After 8 subframes of PSCCH
  pfactory.SetDataPrbNum(25);
  pfactory.SetDataPrbStart(0);
  pfactory.SetDataPrbEnd(49);

  preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

In addition to the subframe indicator bitmap specified by KTRP and iTRP, Mode 2
introduces another level of subframe filtering for the subframe pool via
"DataBitmap" to limit the number of possible values for iTRP. The PSSCH
transmission occurs on the filtered subframes after applying TRP bitmap on this
"DataBitmap". Users interested to learn about how it is applied are referred to
:ref:`SidelinkCommPoolPsschTestCase`.

IP configuration
----------------

* Installing the IP stack on the UEs and assigning IP addresses (IPv4
  code statements shown below) : ::

   InternetStackHelper internet;
   internet.Install (ueNodes);
   uint32_t groupL2Address = 255;
   Ipv4Address groupAddress4 ("225.0.0.0");//use multicast address as destination
   remoteAddress = InetSocketAddress (groupAddress4, 8000);
   localAddress = InetSocketAddress (Ipv4Address::GetAny (), 8000);
   ...
   Ipv4InterfaceContainer ueIpIface;
   ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));
   Ipv4StaticRoutingHelper ipv4RoutingHelper;
   for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
     {
       Ptr<Node> ueNode = ueNodes.Get (u);
       // Set the default gateway for the UE
       Ptr<Ipv4StaticRouting> ueStaticRouting = 
                ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
       ueStaticRouting->SetDefaultRoute 
                                  (epcHelper->GetUeDefaultGatewayAddress (), 1);
     }
  
Application configuration
-------------------------

* Installing applications and activating Sidelink radio bearers : ::

   //Set Application in the UEs
   OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
   sidelinkClient.SetConstantRate (DataRate ("16kb/s"), 200);

   ApplicationContainer clientApps = sidelinkClient.Install (ueNodes.Get (0));
   //onoff application will send the first packet at :
   //(2.9 (App Start Time) + (1600 (Pkt size in bits) / 16000 (Data rate)) = 3.0 sec
   clientApps.Start (slBearersActivationTime + Seconds (0.9));
   clientApps.Stop (simTime - slBearersActivationTime + Seconds (1.0));

   ApplicationContainer serverApps;
   PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", localAddress);
   serverApps = sidelinkSink.Install (ueNodes.Get (1));
   serverApps.Start(Seconds (2.0));

In this example, an "OnOff" application is installed in the UE 1, which sends a
200 byte packet with the constant bit rate of 16 kb/s. On the other hand, the UE
2 is configured with the "PacketSink" application.

The Sidelink radio bearers are activated by calling the
``ActivateSidelinkBearer`` method of ``LteSidelinkHelper`` as follows: ::

   //Set Sidelink bearers
   Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL,
                                                 groupAddress4, groupL2Address);
   proseHelper->ActivateSidelinkBearer (Seconds (2.0), ueDevs, tft);

The ``ActivateSidelinkBearer`` method takes the following three parameters as input,

    #. Time to activate the bearer

    #. Devices for which the bearer will be activated

    #. The Sidelink traffic flow template

The ``tft`` in this example is an object of ``LteSlTft`` class created by
initializing its following parameters,

    #. Direction of the bearer, i.e., "Transmit", "Receive" or Bidirectional

    #. An IPv4 multicast address of the group

    #. Sidelink layer 2 group address (used as Sidelink layer 2 group id)

* Activating the Sidelink traces : ::

   lteHelper->EnableSlPscchMacTraces ();
   lteHelper->EnableSlPsschMacTraces ();

   lteHelper->EnableSlTxPhyTraces ();
   lteHelper->EnableSlRxPhyTraces ();
   lteHelper->EnableSlPscchRxPhyTraces ();

The above code will enable all the Sidelink related traces.

Upon the completion of the simulation, the following trace files can be found in
the repository's root folder,

* SlCchMacStats.txt
* SlSchMacStats.txt
* SlCchRxPhyStats.txt
* SlRxPhyStats.txt

For more information related to the above files please refer to the
:any:ref:`sec-sidelink-simulation-output` section.

* UePacketTrace.tr

  The information in this file is obtained by using the traces "TxWithAddresses"
  and "RxWithAddresses" of ``OnOff`` and ``PacketSink`` application,
  respectively. Following table shows the snippet of the data from this file for
  the two UEs,

  .. tabularcolumns:: |l|l|l|l|l|l|l|
  .. list-table:: UePacketTrace.tr
   :header-rows: 1

   * - Time (sec)
     - tx/rx
     - NodeID
     - IMSI
     - PktSize (bytes)
     - IP[src]
     - IP[dst]
   * - 3
     - tx
     - 3
     - 1
     - 200
     - 7.0.0.2:49153
     - 225.0.0.0:8000
   * - 3.08893
     - rx
     - 4
     - 2
     - 200
     - 7.0.0.2:49153
     - 7.0.0.3:8000

The first row shows that the UE 1 with IMSI 1 transmits a multicast packet of
200 bytes and the UE 2 receives the packet transmitted by the UE 1. As per the
client's application data rate and ON time, i.e., 16 kb/s and 2 seconds,
respectively, a total of 20 packets are sent and received by the transmitting
and the receiving UE.


lte-sl-in-covrg-relay
+++++++++++++++++++++
This example simulates an in-coverage UE-to-Network Relay scenario, which
uses both direct discovery and direct communication. The topology of the 
access network is the one depicted in :ref:`fig-lte-sl-simple-scenario` 
in which two UEs are in-coverage of a single eNB. 
In this scenario, UE 1 has the role of *Relay UE* and UE 2 has the role of
*Remote UE*. Additionally, there is a Remote Host in the network with which the
Remote UE communicates, using the connection to the eNB at the beginning of the
simulation, and using the Relay UE once connected to it.

When the relay service starts in the simulation, both UEs are configured 
to perform UE-to-Network Relay discovery for their respective roles. Thus, 
the Remote UE will be able to detect the Relay UE, select it, and hence 
establish a one-to-one communication connection with it (See section 
:ref:`sec-lte-sl-rrc` for more information about the protocols). Then, all the
packets the Remote UE application sends to the Remote Host are transmitted on
the Sidelink to the Relay UE, which then forwards them to the network via the 
eNB, to finally be routed in the network to the Remote Host. 
Packets from the Remote Host towards the Remote UE follow the reverse path 
(i.e., Remote Host -> Network -> eNB -> Relay UE -> Remote UE).    

The scenario uses MODE 2 (UE_SELECTED) for both direct discovery and
direct communication.

The user can configure the simulation time (in seconds) and to output the
configured NS logs by using the corresponding command line variables in the
simulation script. 
For example, a user can run the simulation as follows::

./waf --run "lte-sl-in-covrg-relay --simTime=10 --enableNsLogs=false"

.. highlight:: none

Configuration of LTE and Sidelink default parameters
----------------------------------------------------

* Configuring parameters for PSSCH resource selection : ::
  
   // Fixed MCS and the number of RBs
   Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
   Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5));

   // For selecting subframe indicator bitmap
   Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
   //use default Trp index of 0 
   Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false));

The scenario uses MODE 2 for the Sidelink communication, where the UEs 
select the resources autonomously from a pool specified by the eNB through
``RrcConnectionReconfiguration`` message. The configuration
here is similar to the one in the lte-sl-in-covrg-comm-mode2 scenario. 

* Configure the frequency and the bandwidth : ::

   Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
   Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
   Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
   Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
   Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

We use the LTE Band 1 for both LTE and Sidelink communication.

* Reduce frequency of Downlink CQI report to allow for sidelink transmissions : ::

   Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", 
			TimeValue (MilliSeconds (79)));

The Downlink CQI report is sent in the Uplink. In this implementation, the 
Sidelink uses the same spectrum as the Uplink and Uplink transmissions have
higher priority than Sidelink transmissions. Thus, we need to space out in time
the Downlink CQI report to avoid too frequent Sidelink transmission drops. 
Setting the attribute value to 79 ms in the scenario script results in a 
periodicity of 80 ms in simulation time.   

* Configure the transmission power of the Radio Access Network nodes : ::

   // Set the UEs power in dBm
   Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
   // Set the eNBs power in dBm
   Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));


Topology configuration
----------------------

* Instantiating LTE, EPC and Sidelink helpers and connecting them: ::

   Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
   Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
   lteHelper->SetEpcHelper (epcHelper);
   Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
   proseHelper->SetLteHelper (lteHelper);

* Configuring the Sidelink UE Controller : ::

   Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper", 
				   PointerValue (proseHelper));

As described in the :ref:`sec-lte-sl-rrc` section, the Sidelink UE controller 
was created to provide a framework for the implementation of custom 
UE-to-Network Relay configurations and algorithms. The default controller in 
the model is the ``LteSlBasicUeController``, which uses the instance of 
``LteSidelinkHelper`` in the scenario to perform some of its tasks and must be 
connected to it using the attribute system as shown above. 

* Configuring the pathloss model : ::

   lteHelper->SetAttribute ("PathlossModel", 
				StringValue ("ns3::Hybrid3gppPropagationLossModel"));

* Enabling the Sidelink : ::
   
   lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

* Creating the Remote Host and the Internet : ::

   Ptr<Node> pgw = epcHelper->GetPgwNode ();
   // Create a single RemoteHost
   NodeContainer remoteHostContainer;
   remoteHostContainer.Create (1);
   Ptr<Node> remoteHost = remoteHostContainer.Get (0);
   InternetStackHelper internet;
   internet.Install (remoteHostContainer);
   // Create the Internet
   PointToPointHelper p2ph;
   p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
   p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
   p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
   NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);


* Creating the eNB and UE nodes and setting their position and mobility : ::

   NodeContainer enbNode;
   enbNode.Create (1);
   NodeContainer ueNodes;
   ueNodes.Create (2);
   Ptr<ListPositionAllocator> positionAllocEnb = 
					CreateObject<ListPositionAllocator> ();
   positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
   Ptr<ListPositionAllocator> positionAllocUe1 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe1->Add (Vector (10.0, 0.0, 1.5));
   Ptr<ListPositionAllocator> positionAllocUe2 = 
					CreateObject<ListPositionAllocator> ();
   positionAllocUe2->Add (Vector (-10.0, 0.0, 1.5));
   //Install mobility
   MobilityHelper mobilityeNodeB;
   mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
   mobilityeNodeB.Install (enbNode);
   MobilityHelper mobilityUe1;
   mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe1.SetPositionAllocator (positionAllocUe1);
   mobilityUe1.Install (ueNodes.Get (0));
   MobilityHelper mobilityUe2;
   mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe2.SetPositionAllocator (positionAllocUe2);
   mobilityUe2.Install (ueNodes.Get (1));

* Installing LTE devices to the nodes : ::

   NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
   NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);


Sidelink pool configuration
---------------------------

In this example, the eNB will be configured with the Sidelink communication 
and discovery pools to be used, which are hold by the ``LteSlEnbRrc`` class
as mentioned in :ref:`sec-lte-sl-rrc`. The pool configuration starts by 
setting a flag in ``LteSlEnbRrc`` as an indication that the Sidelink is 
enabled and it is configured as follows::

   Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
   enbSidelinkConfiguration->SetSlEnabled (true);

* Configure Sidelink communication pool : ::

   enbSidelinkConfiguration->SetDefaultPool (
			proseHelper->GetDefaultSlCommTxResourcesSetupUeSelected ());

The above code uses the Sidelink helper to obtain a Mode 2 (UE_SELECTED) 
Sidelink communication pool configured with default values, which is set
as the default pool to be used by all the UEs attached to the eNB. 

* Enable Sidelink discovery : ::

   enbSidelinkConfiguration->SetDiscEnabled (true);

* Configure Sidelink discovery pool : ::

   enbSidelinkConfiguration->AddDiscPool (
			proseHelper->GetDefaultSlDiscTxResourcesSetupUeSelected ());

Similarly to the communication pool, the Sidelink helper is used to obtain a 
Sidelink discovery pool configured with default values, which is then installed
as the discovery pool to be used by all the UEs attached to the eNB. 

* Configure UE-to-Network Relay selection parameters : ::

   enbSidelinkConfiguration->SetDiscConfigRelay (
			proseHelper->GetDefaultSib19DiscConfigRelay ());

The parameters for UE-to-Network Relay (re)selection are broadcasted in the 
SIB19 to the UEs attached to the eNB. These parameters must be configured in the 
scenario when using UE-to-Network Relay, as they are required by the 
``LteUeRrc`` for SD-RSRP measurement and Relay Selection. In this case, the 
Sidelink helper is used to obtain a default configuration for those parameters,
which are then installed in the eNB Sidelink configuration. 

* Install Sidelink configuration on the eNB : ::

   lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

* Configure and install Sidelink preconfiguration on the UEs : ::

   Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
   ueSidelinkConfiguration->SetSlEnabled (true);
   LteRrcSap::SlPreconfiguration preconfiguration;
   ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
   ueSidelinkConfiguration->SetDiscEnabled (true);
   uint8_t nTxResources = 3;
   ueSidelinkConfiguration->SetDiscTxResources (nTxResources);
   ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
   lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);
 
IP configuration
----------------

* Installing the IP stack on the UEs and assigning IP address (**Please note 
  that the UE-to-Network Relay functionality only supports IPv6 
  configuration**) : ::

   //Install the IP stack on the UEs and assign network IP addresses
   internet.Install (ueNodes);
   Ipv6InterfaceContainer ueIpIface;
   ueIpIface = epcHelper->AssignUeIpv6Address (NetDeviceContainer (ueDevs));

   //Set the default gateway for the UEs
   Ipv6StaticRoutingHelper Ipv6RoutingHelper;
   for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv6StaticRouting> ueStaticRouting = 
	  	Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (),
					  1);
    }

   //Configure IP for the nodes in the Internet (PGW and RemoteHost)
   Ipv6AddressHelper ipv6h;
   ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
   Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);
   internetIpIfaces.SetForwarding (0, true);
   internetIpIfaces.SetDefaultRouteInAllNodes (0);

   //Set route for the Remote Host to join the LTE network nodes
   Ipv6StaticRoutingHelper ipv6RoutingHelper;
   Ptr<Ipv6StaticRouting> remoteHostStaticRouting = 
   	ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
   remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60),
   				internetIpIfaces.GetAddress (0, 1), 1, 0);
											   

   //Configure UE-to-Network Relay network
   proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (60));

   //Configure route between PGW and UE-to-Network Relay network
   proseHelper->ConfigurePgwToUeToNetworkRelayRoute (pgw);


The above configuration is similar to any usual LTE example with EPC and IPv6, 
except the two last commands, which are used to configure the UE-to-Network 
Relay subnetwork and to add a route in the PGW for the packets directed to it. 
In this example, this route will be used for the packets from the Remote Host
towards the Remote UE once connected to the Relay UE. 
  

* Attaching the UEs to the eNB : ::

   lteHelper->Attach (ueDevs);
 
Application configuration
-------------------------

* Installing applications : :: 

   // interface 0 is localhost, 1 is the p2p device
   Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
   uint16_t echoPort = 8000;

   //Set echo server in the remote host
   UdpEchoServerHelper echoServer (echoPort);
   ApplicationContainer serverApps = echoServer.Install (remoteHost);
   serverApps.Start (Seconds (1.0));
   serverApps.Stop (Seconds (10.0));

   //Set echo client in the remote UEs
   UdpEchoClientHelper echoClient (remoteHostAddr, echoPort);
   echoClient.SetAttribute ("MaxPackets", UintegerValue (20));
   echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
   echoClient.SetAttribute ("PacketSize", UintegerValue (200));
   ApplicationContainer clientApps = echoClient.Install (ueNodes.Get (1));
   clientApps.Start (Seconds (2.0));
   clientApps.Stop (Seconds (10.0));

In this example, an ``UdpEchoClient`` application is installed on UE 2, i.e.,
the Remote UE, and an ``UdpEchoServer`` application is installed on the Remote
Host. The Remote UE application will send packets every 0.5 seconds to the 
Remote Host, whose application then echoes back the packet to the Remote UE 
upon reception.  

 
Service configuration
---------------------

* Configure network bearer for UE-to-Network Relay packet routing : ::

   //Setup dedicated bearer for the Relay UE
   Ptr<EpcTft> tft = Create<EpcTft> ();
   EpcTft::PacketFilter dlpf;
   dlpf.localIpv6Address = proseHelper->GetIpv6NetworkForRelayCommunication ();
   dlpf.localIpv6Prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
   tft->Add (dlpf);
   EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
   lteHelper->ActivateDedicatedEpsBearer (ueDevs.Get (0), bearer, tft);

In the above code, we configure an EPS bearer for the Relay UE with a packet
filter corresponding to the UE-to-Network Relay subnetwork. This will be used
to support the routing of packet directed to the Remote UE towards the Relay 
UE.  

* Configure start of UE-to-Network Relay services : ::

   uint32_t serviceCode = 33;
   Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, 
                            proseHelper, ueDevs.Get (0), serviceCode, 
                            LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);

   Simulator::Schedule (Seconds (4.0), &LteSidelinkHelper::StartRelayService, 
                            proseHelper, ueDevs.Get (1), serviceCode, 
                            LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);


The function ``LteSidelinkHelper::StartRelayService`` is the one used from the
scenario to start the UE-to-Network Relay functionality on a given UE. In this
example, both UEs are configured with the same service code and to use direct 
discovery Model A. We observe the role differentiation in the last parameter of 
the respective function calls. As described before, UE 1 will have the role of
Relay UE and UE 2 will have the role of Remote UE. 
The Relay UE service will start first, at 2 seconds of simulation time. Then, 
the Relay UE will start to advertise the service code on its UE-to-Network 
Relay Discovery Announcements. At 4 seconds of simulation time, the Remote UE
starts its service and proceeds to monitor the announcements and to measure
their SD-RSRP. Eventually (4 discovery periods), the Relay Selection procedure
will be executed, the Remote UE will select UE 1 as its Relay UE (as it is the 
only one in the system) and then it will start the one-to-one connection 
procedure to connect to it. Please note that even though the Remote UE service 
starts at 4 s, the Remote UE will start sending its traffic using the Relay UE
(instead of the network) only after the one-to-one connection is established.

Upon the completion of the simulation, the following trace files contain the 
generated output:

* AppPacketTrace.txt

  The information in this file is obtained by using the traces 
  ``TxWithAddresses`` and ``RxWithAddresses`` of  ``UdpEchoClient``
  and  ``UdpEchoServer`` applications.
  The following table shows a snippet of the data from this file.
  Please note that the IPv6 address used by the Remote UE changes in simulation 
  time. An IPv6 address belonging to the UE-to-Network Relay subnet is assigned to it
  when it connects to the Relay UE. This last is unknown during the
  scenario configuration when the trace sinks are connected. 
  Thus, for simplicity and legibility, the IPv6 address of the Remote UE is 
  set to Zero when the trace sinks of the UEs are connected, and thus appear
  as Zero on the traces. This has no impact on the behavior of the scenario 
  whatsoever, and the actual IPv6 address the Remote UE is using can be 
  observed in the traces by looking to the source address of the packets 
  received by the Remote Host.  
  
  .. tabularcolumns:: |p{1.3cm}|l|p{0.8cm}|p{0.7cm}|p{1cm}|l|l|
  .. list-table:: AppPacketTrace.txt
   :header-rows: 1

   * - time (s)
     - tx/rx
     - Node ID
     - IMSI
     - PktSize (bytes)
     - IP[src]
     - IP[dst]
   * - 3
     - tx-UE
     - 6
     - 2
     - 200
     - :::49153
     - 6001:db80::200:ff:fe00:7:8000
   * - 3.02193 
     - rx-RH
     - 3 
     - nan
     - 200
     - 7777:f00d::2:49153
     - 6001:db80::200:ff:fe00:7:8000
   * - 3.02193 
     - tx-RH
     - 3 
     - nan 
     - 200
     - 6001:db80::200:ff:fe00:7:8000
     - 7777:f00d::2:49153
   * - 3.035
     - rx-UE
     - 6
     - 2
     - 200
     - 6001:db80::200:ff:fe00:7:8000
     - :::49153
   * - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
   * - 7
     - tx-UE
     - 6
     - 2
     - 200
     - :::49153
     - 6001:db80::200:ff:fe00:7:8000
   * - 7.11293 
     - rx-RH
     - 3
     - nan
     - 200
     - 7777:f00e::4:49153
     - 6001:db80::200:ff:fe00:7:8000
   * - 7.11293
     - tx-RH
     - 3
     - nan
     - 200
     - 6001:db80::200:ff:fe00:7:8000
     - 7777:f00e::4:49153
   * - 7.21093 
     - rx-UE
     - 6
     - 2
     - 200
     - 6001:db80::200:ff:fe00:7:8000
     - :::49153

  We observe that the packet received by the Remote Host before the relay service
  starts for the Remote UE, i.e., with t < 4.0 s, comes from (and it is 
  transmitted back to) the address ``7777:f00d::2`` which belongs to the network. 
  However, the packet received at 7.11593 s comes from (and it is transmitted 
  back to) the address ``7777:f00e::4`` which belongs to the UE-to-Network Relay
  network.    
  In both cases, the packet is successfully received back by the Remote UE, which
  means that the bidirectional communication with the Remote Host is possible before
  and after connecting to the Relay UE.  

* DlPdcpStats.txt
* UlPdcpStats.txt

  These two traces are standard to the ns-3 LTE simulations, but in this example 
  they allow us to verify which UE is using the Downlink and the Uplink for 
  communicating with the network. Tables :ref:`tab-UlPdcpStats` and 
  :ref:`tab-DlPdcpStats` show snippets of the data on the corresponding files. 
  In this example, the Relay UE IMSI is 1 and the Remote UE IMSI is 2. We see 
  that before the relay service is activated for the Remote UE (t < 4.0 s), 
  this last is using the Uplink to send its packet towards the Remote Host and 
  the Downlink to receive the corresponding echoed packet. Once the Remote UE 
  connects to the Relay UE, we see in the traces that it is the Relay UE who is
  communicating with the network using Uplink and Downlink, i.e., the Relay
  UE is forwarding the packets received from the Remote UE in the Sidelink to 
  the network using the Uplink and receiving packets in the Downlink and 
  forwarding them to the Remote UE on the Sidelink.

  .. tabularcolumns:: |l|l|l|l|l|l|l|l|l|
  .. _tab-UlPdcpStats:
  .. list-table:: UlPdcpStats.txt
   :header-rows: 1

   * - start (s)
     - end (s)
     - CellId	
     - IMSI
     - RNTI
     - LCID
     - nTxPDUs
     - TxBytes
     - ...	 
   * - 2.75
     - 3
     - 1
     - 2
     - 2
     - 3
     - 1
     - 250
     - ...
   * - 3.25
     - 3.5
     - 1
     - 2
     - 2
     - 3
     - 1
     - 250
     - ...
   * - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
   * - 7
     - 7.25
     - 1
     - 1
     - 1
     - 4
     - 1
     - 250
     - ...	 
   * - 7.5
     - 7.75
     - 1
     - 1
     - 1
     - 4
     - 1
     - 250
     - ...

  .. tabularcolumns:: |l|l|l|l|l|l|l|l|l|
  .. _tab-DlPdcpStats:
  .. list-table:: DlPdcpStats.txt
   :header-rows: 1

   * - start (s)
     - end (s)
     - CellId	
     - IMSI
     - RNTI
     - LCID
     - nTxPDUs
     - TxBytes
     - ...	 
   * - 3.5
     - 3.75
     - 1
     - 2
     - 2
     - 3
     - 1
     - 250
     - ...
   * - 4
     - 4.25
     - 1
     - 2
     - 2
     - 3
     - 1
     - 250
     - ...
   * - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
     - ...
   * - 7
     - 7.25
     - 1
     - 1
     - 1
     - 4
     - 1
     - 250
     - ...	 
   * - 7.5
     - 7.75
     - 1
     - 1
     - 1
     - 4
     - 1
     - 250
     - ...
	 
	 

In the following we will discuss the detailed examples mentioned above, however,
we will mainly discuss about the topology and the Sidelink configuration, since
the example scripts already contain the details of the default attributes and
also, most of them are already covered by the previous examples.

wns3-2017-synch
+++++++++++++++

The scenario in this example is composed of 1 hexagonal cell site divided into 3
sectors. Each sector has 1 UE, i.e., 3 in total, which are randomly dropped.
These UEs are then grouped in a single group by choosing randomly one UE as a
transmitter while the other 2 UEs act as receivers.

Topology configuration
----------------------

In the following, we walk through the example line by line and discuss each
important topology configuration. However, some configuration parameters are
skipped, which are already elaborated in the previous sections of LTE.
Therefore, it is highly recommended for the new users to go through the basic
LTE configuration before digging into D2D examples.

.. highlight:: none

* Randomizing the frame and subframe number of the UEs : ::

   Config::SetDefault ("ns3::LteUePhy::UeRandomInitialSubframeIndication",
                                                          BooleanValue (unsyncSl));

As mentioned in the design documentation :ref:`sec-lte-sl-ue-phy-sync`, by
default all the UEs are perfectly synchronized, i.e., all the UEs in a
simulation upon being initialized pick the same frame and subframe 1 to start
with. Therefore, to simulate synchronization and to make every UE to pick a
random frame and subframe number the attribute
"UeRandomInitialSubframeIndication" should be set.

* Instantiating ``LteSidelinkHelper`` and setting ``LteHelper`` : ::

    Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
    proseHelper->SetLteHelper (lteHelper);

* Instantiating ``Lte3gppHexGridEnbTopologyHelper`` and setting ``LteHelper`` ::

    Ptr<Lte3gppHexGridEnbTopologyHelper> topoHelper =
                               CreateObject<Lte3gppHexGridEnbTopologyHelper> ();
    topoHelper->SetLteHelper (lteHelper);

* Disabling the eNB for out-of-coverage scenario : ::

    lteHelper->DisableEnbPhy (true);

**Note : Call to disable eNB PHY should happen before installing the eNB devices**

* Configure the parameters for hexagonal ring topology : ::

    topoHelper->SetNumRings (numRings); // 1 Ring
    topoHelper->SetInterSiteDistance (isd); // 500 m
    topoHelper->SetMinimumDistance (10); // in meter
    topoHelper->SetSiteHeight (32); // in meter

The code above will set the parameters to build 1 ring, i.e., 1 hexagonal site,
which will have 500 meters of inter-site distance between the sites, if the
number of rings is more than 1. The minimum distance between an eNB and the UEs
is set to 10 m and the eNB is at the height of 32 m above the ground.

* Creating eNB nodes : ::

    NodeContainer sectorNodes;
    sectorNodes.Create (topoHelper->GetNumNodes ());

Call to ``topoHelper->GetNumNodes ()`` will return 3, which in turns would
create 3 sector nodes, since, it requires 3 eNBs to cover one hexagon.

* Fixing eNB mobility and installing eNB devices : ::

    MobilityHelper mobilityeNodeB;
    mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobilityeNodeB.Install (sectorNodes);
    NetDeviceContainer enbDevs = topoHelper->
                                      SetPositionAndInstallEnbDevice (sectorNodes);

The call ``SetPositionAndInstallEnbDevice`` will compute the position of each
site and the antenna orientation of each eNB. The antenna orientation of each
eNB in a hexagon is 30, 150 and 270 degrees, respectively [TS25814]_ [TR36814]_.
As mentioned before, even if the eNB PHY is disabled, the hexagonal topology
requires to instantiate the eNB nodes to form the sectors of an hexagon. It is
also to be noted that the use of antenna models, e.g.,
``Parabolic3dAntennaModel``, ``ParabolicAntennaModel``, or
``CosineAntennaModel`` is needed to configure antenna orientation. Moreover, the
function ``SetPositionAndInstallEnbDevice`` is also responsible to call
``InstallEnbDevice`` of ``LteHelper`` for each eNB in this scenario.

* Creating UE nodes : ::

    NodeContainer ueResponders;
    ueResponders.Create (ueRespondersPerSector * sectorNodes.GetN ());


This will create 3 UE nodes in total, 1 for each sector of hexagonal site.

* Enabling Sidelink ::

    lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

**Note : Attribute ``UseSidelink`` must be set before installing the UE devices.**

* Fixing UE mobility and installing UE devices : ::


    MobilityHelper mobilityResponders;
    mobilityResponders.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    topoHelper->DropUEsUniformlyPerSector (ueResponders);

This will place 1 UE per sector by choosing their position randomly with in the
sector, taking into account the configured minimum eNB to UE distance of 10 m.
This will also install the UE devices by calling ``InstallUeDevice`` of
``LteHelper``. The installation of the IP stack and IP address assignment is
done in a similar fashion as explain in lte-design:ref:`sec-evolved-packet-core`

* Creating groups of UEs : ::

    double ulEarfcn = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ();
    double ulBandwidth = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->
                                                                   GetUlBandwidth ();

    std::vector < NetDeviceContainer > createdgroups;
    createdgroups = proseHelper->AssociateForBroadcast (ueTxPower, ulEarfcn,
                                                        ulBandwidth, ueRespondersDevs,
                                                        -112, numGroups,
                                                        LteSidelinkHelper::
                                                                        SLRSRP_PSBCH);


The ``AssociateForBroadcast`` function will basically form the specified number
of groups, i.e., 1 in this example, by choosing the transmitting UE randomly
from the total number of UEs. After choosing the transmitting UE, the receiving
UEs of the group are selected if the Sidelink RSRP, calculated as per the method
defined in [TS36214]_, between the transmitting UE and the receiving UE is
higher than -112 dBm. In this example, UE node 7 with IMSI 2 is selected as a
transmitter while remaining 2 UEs will act as receivers. It is to be noted that
``AssociateForBroadcast`` is only responsible for forming the groups and not
installing any application.

All the UEs in a group are stored in a ``NetDeviceContainer``, such that the
first UE in this container is the transmitter of the group. Finally, this
``NetDeviceContainer`` is stored in a vector (createdgroups), which is used to
install appropriate application in UE devices.

Application configuration
-------------------------

* Installing applications and activating Sidelink radio bearers : ::

   **Note : Only IPV4 is supported at this stage**

   // Client Application :

    uint32_t groupL2Address = 0x00;

    Ipv4AddressGenerator::Init (Ipv4Address ("225.0.0.0"), Ipv4Mask ("255.0.0.0"));
    Ipv4Address groupRespondersIpv4Address = Ipv4AddressGenerator::
                                              NextAddress (Ipv4Mask ("255.0.0.0"));

    std::vector < NetDeviceContainer >::iterator gIt;
    for (gIt = createdgroups.begin (); gIt != createdgroups.end (); gIt++)
     {
      //Create sidelink bearers
      //Use Tx for the group transmitter and Rx for all the receivers
      //split Tx/Rx
      NetDeviceContainer txUe ((*gIt).Get (0));
      NetDeviceContainer rxUes = proseHelper->
                                          RemoveNetDevice ((*gIt), (*gIt).Get (0));
      Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::TRANSMIT,
                                       groupRespondersIpv4Address, groupL2Address);

      //Sidelink bearer activation
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), txUe, tft);
      tft = Create<LteSlTft> 
                   (LteSlTft::RECEIVE, groupRespondersIpv4Address, groupL2Address);
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), rxUes, tft);

        .
        .
        .

      UdpEchoClientHelper echoClientHelper 
                                   (groupRespondersIpv4Address, grpEchoServerPort);
      clientRespondersApps = echoClientHelper.Install ((*gIt).Get (0)->GetNode ());

        .
        .
        .

      groupL2Address++;
      groupRespondersIpv4Address = Ipv4AddressGenerator::
                                              NextAddress (Ipv4Mask ("255.0.0.0"));
     }

   // Server Application :

      PacketSinkHelper clientPacketSinkHelper ("ns3::UdpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), echoPort));
      ApplicationContainer clientRespondersSrvApps = 
                                       clientPacketSinkHelper.Install (ueResponders);


In the previous step we obtained the vector ``createdgroups``, containing a
group formed by one transmitter UE and 2 receiver UEs. Now, it is easy to
anticipate that the client application will be installed in the transmitter UE
and the receiver UEs will have server application. In this example, a user can
configure any of the two client applications, i.e., ``OnOff`` and
``UdpEchoClientApplication``, where the later one is the default client
application.

The Sidelink radio bearers for Tx (for transmitting UE) and Rx (for receiving
UEs) are activated by calling the `ActivateSidelinkBearer`` method of
``LteSidelinkHelper``.

Finally, a ``PacketSink`` application is installed as a server application in
the receiving UEs.


Sidelink pool configuration
---------------------------

This example simulates an out-of-coverage scenario, therefore, all the UEs will
be configured with a pre-configured Sidelink pool. As mentioned in
:ref:`sec-lte-sl-rrc`, in this scenario the ``LteSlUeRrc`` class will be
responsible for holding this per-configured pool configuration. The pool
configuration starts by setting a flag in ``LteSlUeRrc`` as an indication that
the Sidelink is enabled. It is configured as follows::

 Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
 ueSidelinkConfiguration->SetSlEnabled (true);

For configuring Sidelink pre-configured pool parameters, the IE
"SL-Preconfiguration" defined in the standard [TS36331]_ is converted into a C++
structure and similar to the basic LTE layer 3 messages it can be found in
``LteRrcSap`` class. This example uses this structure to configure the Sidelink
pool parameters for control, data and synchronization in the following manner,
::

 LteRrcSap::SlPreconfiguration preconfiguration;

 preconfiguration.preconfigGeneral.carrierFreq = 23330;
 preconfiguration.preconfigGeneral.slBandwidth = 50;
 preconfiguration.preconfigComm.nbPools = 1;

 //control
 preconfiguration.preconfigComm.pools[0].scCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
 preconfiguration.preconfigComm.pools[0].scPeriod.period = LteRrcSap::
                                                       PeriodAsEnum (slPeriod).period;
 preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbNum = pscchRbs;
 preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbStart = 3;
 preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbEnd = 46;
 preconfiguration.preconfigComm.pools[0].scTfResourceConfig.offsetIndicator.offset = 0;
 preconfiguration.preconfigComm.pools[0].scTfResourceConfig.subframeBitmap.
                                                            bitmap = pscchTrpNumber;

 //data
 preconfiguration.preconfigComm.pools[0].dataCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
 preconfiguration.preconfigComm.pools[0].dataHoppingConfig.hoppingParameter = 0;
 preconfiguration.preconfigComm.pools[0].dataHoppingConfig.numSubbands = LteRrcSap::
                                                             SlHoppingConfigComm::ns4;
 preconfiguration.preconfigComm.pools[0].dataHoppingConfig.rbOffset = 0;

 preconfiguration.preconfigComm.pools[0].trptSubset.subset = std::bitset<3> (0x7);
 preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbNum = 25;
 preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbStart = 0;
 preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbEnd = 49;
 preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.offsetIndicator.
                                                                 offset = pscchLength;
 preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.subframeBitmap.bitmap = 
                                                        std::bitset<40>(0xFFFFFFFFFF);

 preconfiguration.preconfigComm.pools[0].scTxParameters.alpha = LteRrcSap::
                                                                 SlTxParameters::al09;
 preconfiguration.preconfigComm.pools[0].scTxParameters.p0 = -40;
 preconfiguration.preconfigComm.pools[0].dataTxParameters.alpha = LteRrcSap::
                                                                 SlTxParameters::al09;
 preconfiguration.preconfigComm.pools[0].dataTxParameters.p0 = -40;

 //Synchronization
 preconfiguration.preconfigSync.syncOffsetIndicator1 = 18;
 preconfiguration.preconfigSync.syncOffsetIndicator2 = 29;
 preconfiguration.preconfigSync.syncTxThreshOoC = syncTxThreshOoC;
 preconfiguration.preconfigSync.syncRefDiffHyst = syncRefDiffHyst;
 preconfiguration.preconfigSync.syncRefMinHyst = syncRefMinHyst;
 preconfiguration.preconfigSync.filterCoefficient = filterCoefficient;


Finally, the configured pool is stored in ``LteSlUeRrc`` class by calling
``SetSlPreconfiguration`` function ::

 ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);

Then, by calling the ``InstallSidelinkConfiguration`` method of ``LteHelper``
class it configures the ``LteUeRrc`` attribute named "SidelinkConfiguration" of
a UE, which is nothing but a pointer to the ``LteSlUeRrc`` object used to
configure the pool::

 lteHelper->InstallSidelinkConfiguration (ueRespondersDevs, ueSidelinkConfiguration);

This interaction of classes to populate the Sidelink pool is also depicted in
:ref:`fig-lte-sl-pool-config`.

Alternatively, the pool configuration for control and data can also be done in a
semi-automatic way by using the ``LteSlPreconfigPoolFactory``. This would ease
the configuration of the pool by only changing the parameters of interest and
leaving the rest as default. For example, the above configuration can also be
done as follows, ::

 preconfiguration.preconfigGeneral.carrierFreq = 23330;
 preconfiguration.preconfigGeneral.slBandwidth = 50;
 preconfiguration.preconfigComm.nbPools = 1;

 LteSlPreconfigPoolFactory pfactory;

 //Control
 pfactory.SetControlPrbStart (3);
 pfactory.SetControlPrbEnd (46);

 //Data
 pfactory.SetDataOffset (pscchLength);

 //Synchronization
 preconfiguration.preconfigSync.syncOffsetIndicator1 = 18;
 preconfiguration.preconfigSync.syncOffsetIndicator2 = 29;
 preconfiguration.preconfigSync.syncTxThreshOoC = syncTxThreshOoC;
 preconfiguration.preconfigSync.syncRefDiffHyst = syncRefDiffHyst;
 preconfiguration.preconfigSync.syncRefMinHyst = syncRefMinHyst;
 preconfiguration.preconfigSync.filterCoefficient = filterCoefficient;

 preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();

 ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);

 lteHelper->InstallSidelinkConfiguration (ueRespondersDevs, ueSidelinkConfiguration);


Upon the completion of the simulation, the information related to the topology
and the data related to the synchronization process gathered through the traces
is written to the following files,

 * nPositions.txt

   This file stores the position of all the nodes, i.e., eNBs and the UEs in the
   simulation.

 * prose-connections.txt

   This file stores the information, e.g., Node id, and IMSI of the transmitting
   and the corresponding receiving UEs.

 * FirstScan.txt

   This file stores the start time (in ms) of the first scanning period and the
   IMSI of each UE nodes. The start time for each UE is randomly chosen between
   2000 ms and 4000 ms defined by the variables "firstScanTimeMin" and
   "firstScanTimeMax" in the simulation script.


 * SyncRef.txt

   The information in this file is gathered by listening to the trace
   "ChangeOfSyncRef" of ``LteUeRrc``.

  .. tabularcolumns:: |l|l|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|
  .. list-table:: SyncRef.txt
   :header-rows: 1

   * - Time (ms)
     - IMSI
     - prev
       SLSSID
     - prev
       RxOffset
     - prev
       FrameNo
     - prev
       SframeNo
     - curr
       SLSSID
     - curr
       RxOffset
     - curr
       FrameNo
     - curr
       SframeNo
   * - 0
     - 1
     - 0
     - 0
     - 0
     - 0
     - 148921
     - 0
     - 0
     - 0
   * - 0
     - 2
     - 0
     - 0
     - 0
     - 0
     - 175823
     - 0
     - 0
     - 0
   * - 0
     - 3
     - 0
     - 0
     - 0
     - 0
     - 170827
     - 0
     - 0
     - 0
   * - 20023
     - 2
     - 175823
     - 0
     - 637
     - 5
     - 20
     - 0
     - 637
     - 5
   * - 21407
     - 1
     - 148921
     - 0
     - 224
     - 3
     - 20
     - 2
     - 775
     - 9
   * - 21460
     - 3
     - 170827
     - 0
     - 791
     - 7
     - 20
     - 2
     - 781
     - 2
   * - 50000
     - 1
     - 20
     - 0
     - 563
     - 5
     - 0
     - 0
     - 0
     - 0
   * - 50000
     - 2
     - 20
     - 0
     - 563
     - 5
     - 0
     - 0
     - 0
     - 0
   * - 50000
     - 3
     - 20
     - 0
     - 563
     - 5
     - 0
     - 0
     - 0
     - 0

  It is to be noted that the data in first and last three rows of above table
  are written by this simulation script to mark the start and end of the
  simulation. From row 4 we can observe that the UE with IMSI 1 becomes the
  SyncRef, this can be deduced by looking at its SLSS-ID, which is ``SLSS-ID =
  IMSI * 10`` as explained in :ref:`sec-lte-sl-rrc`. On the other hand, UEs with
  IMSI 2 and 3 selects IMSI 1 as their SyncRef and use the same SLSS-ID, as shown
  in row 5 and 6.

 * pscr-ue-pck.tr

   The information in this file is obtained by using the traces
   "TxWithAddresses" and "RxWithAddresses" of ``UdpEchoClient`` and ``PacketSink``
   application respectively. Following table shows the snippet of data from this
   file for all the three UEs,

  .. tabularcolumns:: |l|l|l|l|l|l|l|l|
  .. list-table:: pscr-ue-pck.tr
   :header-rows: 1

   * - Time (ms)
     - tx/rx
     - NID
     - IMSI
     - UEtype
     - size (bytes)
     - IP[src]
     - IP[dst]
   * - 21485
     - t
     - 7
     - 2
     - resp
     - 40
     - 7.0.0.3:49153
     - 225.0.0.1:8000
   * - 21488
     - r
     - 6
     - 1
     - resp
     - 40
     - 7.0.0.3:49153
     - 7.0.0.2:8000
   * - 21488
     - r
     - 8
     - 3
     - resp
     - 40
     - 7.0.0.3:49153
     - 7.0.0.4:8000

The first row shows that the first UE with node id 4 and IMSI 1 acts as a
transmitter and sends a multicast packet of 40 bytes. Similarly, the other two
UEs act as receiver.

 * TxSlss.txt

   This file stores the information gathered by listening to the trace
   "SendSLSS" of ``LteUeRrc``. Following is the snippet of the information
   stored in this file.

  .. tabularcolumns:: |l|l|l|l|l|l|l|
  .. list-table:: TxSlss.txt
   :header-rows: 1

   * - Time (ms)
     - IMSI
     - SLSSID
     - txOffset (ms)
     - inCoverage
     - FrameNo
     - SframeNo
   * - 20038
     - 2
     - 20
     - 29
     - 0
     - 639
     - 4
   * - 20078
     - 2
     - 20
     - 29
     - 0
     - 643
     - 4
   * - 20118
     - 2
     - 20
     - 29
     - 0
     - 647
     - 4
   * - 20158
     - 2
     - 20
     - 29
     - 0
     - 651
     - 4

It can be observed that UE with IMSI 1 being a SyncRef is transmitting SLSS with
a fixed periodicity of 40 ms. Moreover, at the time when this UE chose to become
a SyncRef it randomly selects one of the two configured offsets, i.e.,
syncOffsetIndicator1 and syncOffsetIndicator2. In this example, it has selected
syncOffsetIndicator2, which is set to 29 ms.

wns3-2017-discovery
+++++++++++++++++++

This example deploys 10 out-of-coverage UEs, distributed randomly within an area
of 100 m x 100 m. This example script also illustrates how an out-of-coverage
Sidelink related simulation can be performed without instantiating the eNB
nodes.

Topology configuration
----------------------

Compared to the topology of the synchronization example that we discussed
before, this example is very simple. For instance, it does not use hexagonal
topology or form groups of UEs, i.e., it does not use
``Lte3gppHexGridEnbTopologyHelper`` and ``LteSidelinkHelper``. The ``LteHelper``
and ``PointToPointEpcHelper`` are initialized in the same way like any other LTE
example with EPC. Also, the call to disable the eNB PHY can also be skipped,
since it does not instantiate the eNB nodes. Moreover, this example only
focuses on simulating ProSe out-of-coverage discovery, therefore, there is no
need to configure the IP of the UEs since discovery is purely a MAC layer
application where the messages are filtered on the basis of ProSeAppCode.

The first Sidelink related configuration in this example is to enable the Sidelink.

* Enabling Sidelink ::

   lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

**Note : Attribute ``UseSidelink`` must be set before installing the UE devices.**

* Work-around to bypass the use of eNB nodes ::

   (1)

     lteHelper->SetAttribute ("PathlossModel", 
                            StringValue ("ns3::FriisPropagationLossModel"));

   (2)

     lteHelper->Initialize ();

   (3)

     double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (23330);
     NS_LOG_LOGIC ("UL freq: " << ulFreq);
     Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
     Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->
                                    GetObject<PropagationLossModel> ();
     NS_ABORT_MSG_IF (lossModel == NULL, "No PathLossModel");
     bool ulFreqOk = uplinkPathlossModel->
            SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));

     if (!ulFreqOk)
       {
         NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
       }


The use of eNB nodes can be bypassed by using the above commands strictly in the
order they are listed. The command "lteHelper->Initialize ()" basically performs
the channel model initialization of all the component carriers. Therefore, it is
necessary to configure any desired pathloss model before issuing this command.
The commands in step 3 are to properly configure the frequency attribute of the
pathloss model used, which is normally done in ``InstallSingleEnb`` method of
``LteHelper``.

* Creating the UE node, fixing their mobility, and installing UE devices : ::

   NodeContainer ues;
   ues.Create (nbUes);

   //Position of the nodes
   Ptr<ListPositionAllocator> positionAllocUe = 
                            CreateObject<ListPositionAllocator> ();

   for (uint32_t u = 0; u < ues.GetN (); ++u)
     {
       Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable> ();
       double x = rand->GetValue (-100,100);
       double y = rand->GetValue (-100,100);
       double z = 1.5;
       positionAllocUe->Add (Vector (x, y, z));
     }

   // Install mobility

   MobilityHelper mobilityUe;
   mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobilityUe.SetPositionAllocator (positionAllocUe);
   mobilityUe.Install (ues);

   NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ues);

The above commands will simply create 10 UEs and position each UE at a random
location in an area of 100 m x 100 m. The UE devices are installed in a usual
way by calling ``InstallUeDevice`` of ``LteHelper`` class.

Service configuration
---------------------

* Configuring discovery applications and starting the discovery process : ::

   std::map<Ptr<NetDevice>, std::list<uint32_t> > announceApps;
   std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorApps;
   for (uint32_t i = 1; i <= nbUes; ++i)
     {
       announceApps[ueDevs.Get (i - 1)].push_back ((uint32_t)i);
       for (uint32_t j = 1; j <= nbUes; ++j)
         {
           if (i != j)
             {
               monitorApps[ueDevs.Get (i - 1)].push_back ((uint32_t)j);
             }
         }
     }



   for (uint32_t i = 0; i < nbUes; ++i)
     {
       // true for announce
       Simulator::Schedule (Seconds (2.0), &SlStartDiscovery, 
                            lteHelper, ueDevs.Get (i),
                            announceApps.find (ueDevs.Get (i))->second, true);

       // false for monitor
       Simulator::Schedule (Seconds (2.0), &SlStartDiscovery, 
                            lteHelper, ueDevs.Get (i),
                            monitorApps.find (ueDevs.Get (i))->second, false);
     }

Given the broadcast nature of the discovery process, the applications are
configured such that each UE will be able to transmit discovery messages with
one ProSeAppCode, i.e., one announce app and will be able to monitor/receive the
discovery messages from all other UEs (Notice the two ``for`` loops at the
beginning). The UE device index in the ``NetDeviceContainer`` is used as a
ProSeAppCode, which is later in ``LteUeRrc`` is converted into a 184 bitset
[TS23003]_. Finally, at 2 sec the method "SlStartDiscovery" implemented in this
example calls the ``StartDiscovery`` of ``LteHelper`` class to start the
discovery process.

Sidelink pool configuration
---------------------------

This example simulates an out-of-coverage scenario, therefore, all the UEs will
be configured with a pre-configured Sidelink discovery pool. As mentioned in
:ref:`sec-lte-sl-rrc`, in this scenario the ``LteSlUeRrc`` class will be
responsible for holding this per-configured pool configuration. The pool
configuration starts by setting a flag in ``LteSlUeRrc`` as an indication that
the Sidelink discovery is enabled. It is configured as follows::

 Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
 ueSidelinkConfiguration->SetDiscEnabled (true);

For configuring Sidelink discovery pre-configured pool parameters, the IE
"SL-Preconfiguration" defined in the standard [TS36331]_ is converted into a C++
structure and similar to the basic LTE layer 3 messages it can be found in
``LteRrcSap`` class. This example uses this structure to configure the Sidelink
discovery pool parameters in the following way, ::

 LteRrcSap::SlPreconfiguration preconfiguration;

 preconfiguration.preconfigGeneral.carrierFreq = 23330;
 preconfiguration.preconfigGeneral.slBandwidth = 50;
 preconfiguration.preconfigDisc.nbPools = 1;

 preconfiguration.preconfigDisc.pools[0].cpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
 preconfiguration.preconfigDisc.pools[0].discPeriod.period = LteRrcSap::
                                                               SlPeriodDisc::rf32;
 preconfiguration.preconfigDisc.pools[0].numRetx = 0;
 preconfiguration.preconfigDisc.pools[0].numRepetition = 1;
 preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbNum = 10;
 preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbStart = 10;
 preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbEnd = 49;
 preconfiguration.preconfigDisc.pools[0].tfResourceConfig.offsetIndicator.offset = 0;
 preconfiguration.preconfigDisc.pools[0].tfResourceConfig.subframeBitmap.
                                                    bitmap = std::bitset<40>(0x11111);
 preconfiguration.preconfigDisc.pools[0].txParameters.txParametersGeneral.alpha = 
                                                      LteRrcSap::SlTxParameters::al09;

 preconfiguration.preconfigDisc.pools[0].txParameters.txParametersGeneral.p0 = -40;

 preconfiguration.preconfigDisc.pools[0].txParameters.txProbability = 
                              SidelinkDiscResourcePool::TxProbabilityFromInt (txProb);

 NS_LOG_INFO ("Install Sidelink discovery configuration in the UEs...");
 ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
 lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

Alternatively, the discovery pool configuration can also be done in a
semi-automatic way by using the ``LteSlDiscResourcePoolFactory``. This would
ease the configuration of the pool by only changing the parameters of interest
and leaving the rest as default. For example, the above configuration can also
be done as follows, ::

 preconfiguration.preconfigGeneral.carrierFreq = 23330;
 preconfiguration.preconfigGeneral.slBandwidth = 50;
 preconfiguration.preconfigComm.nbPools = 1;

 LteSlDiscPreconfigPoolFactory pfactory;
 pfactory.SetDiscPrbEnd (49);

 preconfiguration.preconfigDisc.pools[0] = pfactory.CreatePool ();

 ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
 lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);


Upon the completion of the simulation a user can find the following files
containing the information of the simulated discovery process.

 * discovery_nodes.txt

   This file stores the position of all the UEs in the simulation.

 * SlDchMacStats.txt

   This file logs the discovery message transmissions (announcements)
   by listening to the "SlPsdchScheduling" trace of ``LteUeMac`` class.
   
 * SlRxPhyStats.txt

   This file contains the information about the reception of the discovery
   announcements at the physical layer of a recipient UE. In particular, it is
   obtained by listening to the "SlPhyReception" trace of ``LteSpectrumPhy``
   class. 

By comparing the traces from UE MAC and UE PHY, one can notice that the phy
layer receives a particular discovery messages exactly after 4 ms of the
processing delay between UE MAC and PHY. The CellId is 0 because of
out-of-coverage scenario.

 * SlDchRxRrcStats.txt

   This file contains the information about the reception of discovery messages
   by each UE. The data is obtained by listening to the "DiscoveryMonitoring"
   trace of ``LteUeRrc`` class. Following is snippet from this file,

  .. tabularcolumns:: |l|l|l|l|l|l|l|l|
  .. list-table:: SlDchRxRrcStats.txt
   :header-rows: 1

   * - Time (ms)
     - IMSI
     - CellId
     - RNTI
     - DisType
     - ContentType
     - DiscModel
     - Content
   * - 2240
     - 2
     - 0
     - 2
     - 1
     - 0
     - 1
     - 1
   * - 2240
     - 3
     - 0
     - 3
     - 1
     - 0
     - 1
     - 1
   * - 2240
     - 4
     - 0
     - 4
     - 1
     - 0
     - 1
     - 1
   * - 2240
     - 5
     - 0
     - 5
     - 1
     - 0
     - 1
     - 1

From the above table it can be observed that the UE with IMSI 2, 3, 4, and 5 have
received a discovery message. This is a message of type open discovery 
(indicated by DiscType) used to announce (indicated by ContentType) a discovery
model A (indicated by DiscModel) content. The content in this message is the
ProSeAppCode of IMSI 1. In other words, these UEs have discovered a UE with
ProSeAppCode 1. For more information about the discovery message parameters
please have a look at [TS24334]_.
