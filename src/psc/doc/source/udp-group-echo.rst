.. include:: replace.txt

+++++++++++++++++++++
UDP Group Echo Server
+++++++++++++++++++++

The class ``ns3::psc::UdpGroupEchoServer`` implements a group echo server that
echoes received UDP datagrams to a set of clients.  The policy
of generating replies can be tailored through the use of attributes.

The relevance of this model to public safety communications is that such
scenarios often require a many-to-many group communications application, and
existing |ns3| applications are not suitable to generate such traffic.

The UDP group echo implementation is authored by 
Fernando J. Cintron (fernando.cintron@nist.gov) and is derived from the
UdpEchoServer found in the |ns3| applications module.

-----------------
Model Description
-----------------

The implementation is provided in the following files:

* ``src/psc/model/udp-group-echo-server.{h,cc}``   The model itself
* ``src/psc/helper/udp-group-echo-helper.{h,cc}`` Helper code for configuration

Additionally, a simple example is provided at ``src/psc/examples/example-udp-group-echo.cc``.

The model is based on the ``ns3::psc::UdpEchoServer``, but differs in that the
existing server only handles one client, while the group echo server handles
one or more clients.  The behavior of the ``ns3::psc::UdpEchoServer`` can be
reproduced (i.e., it is a special case of this object).

The model works as follows.  The UdpGroupEchoServer is an |ns3| application,
listening for UDP datagrams on a configured UDP port.  Upon receipt of a 
datagram for the first time from a client, the server records the client
and a timestamp for when the packet was received.  The server then decides
whether to forward the packet back to one or more clients, on a pre-configured
'EchoPort'.  The set of possible clients is built dynamically based on 
received packets.

There are a few configurable policies:

1.  The server may be configured to echo only to the client that originated
    the packet (similar to the ``UdpEchoServer``).

2.  The server may be configured to echo to a group of clients including the
    sender.  Furthermore, the sending client may be excluded from the
    response. 

3.  The server may be configured to echo to a group of clients including the
    sender, so long as the server has heard from each client within a
    configurable timeout period.  Furthermore, the sending client may 
    be excluded from the response. 

Attributes
++++++++++

The following is the list of attributes:

* ``Port``: Port on which the server listens for incoming packets, default value of 9.
* ``EchoPort``: Port on which the server echoes packets to client, default value of 0.
* ``Mode``: Mode of operation, either no group session (reply to sender only), timeout limited session (replicate to all clients for which a packet has been received from them within the configured timeout period), and infinite session (reply to all known clients).  The default is no group session.
* ``Timeout``: Inactive client session expiration time, default of zero seconds.
* ``EchoClient``: Whether the server echoes back to the sending client, default value of true.,

Trace sources
+++++++++++++

The model also provides a ``Rx`` trace source for all received datagrams.

-----
Usage
-----

A simple example based on CSMA links is provided in the file ``src/psc/examples/example-udp-group-echo.cc``.

::

  //  *    *
  // n0   n1   ... n(nExtra)  n(1+nExtra)
  //  |    |    |    |         |
  //  ==========================
  //     LAN 10.1.2.0

By default, node n0 is the client and node n1 is the server, although 
additional nodes can be added with the ``--nExtra`` argument.  If there
are more nodes, the highest numbered node is the server. 

Each client is configured with an on-off traffic generator that sends
traffic at random times to the server.  The following program options exercise
some of the configuration of the server:

::

  $ ./ns3 run 'example-udp-group-echo --PrintHelp'

  Program Options:
    --nExtra:      Number of "extra" CSMA nodes/devices [0]
    --echoClient:  Set EchoClient attribute [true]
    --mode:        Set Mode attribute (InfSession|NoGroupSession|TimeoutLimited) [InfSession]
    --timeout:     Set Timeout attribute [+0.0ns]
    --verbose:     Tell echo applications to log if true [true]
    --enablePcap:  Enable PCAP file output [false]
    --time:        Simulation time [10]

By default, the number of extra CSMA nodes/devices is zero, so there will
be only one client and one server.  By running with the existing defaults,
the program will configure the server to echo back to the client, use
the 'InfSession' mode of operation (to echo without considering timeout
value) and a timeout value of 0 (which is not used in this mode).  Logging
is also enabled, and running the program with defaults yields output such
as:

::

  $ ./ns3 run 'example-udp-group-echo'
  ...

  8.7006 server received 41 bytes from 10.1.2.1 port 49153
  Client found; old timestamp: 8.6806
  New timestamp: 8.7006
  8.7006 number of clients: 1
  ---------------------------------------------------------
                 Client     Session
  ---------------------------------------------------------
           10.1.2.1:49153          0
  =========================================================
  8.7006 server sent 41 bytes to 10.1.2.1 port 49153

This shows that node 10.1.2.1 sent data at time 8.7006 that was echoed
back to the client.  Running with the option ``--nExtra=2`` shows:

::

  $ ./ns3 run 'example-udp-group-echo --nExtra=2'
  
  ...

  9.99949 server received 41 bytes from 10.1.2.1 port 49153
  Client found; old timestamp: 9.97949
  New timestamp: 9.99949
  9.99949 number of clients: 3
  ---------------------------------------------------------
                 Client     Session
  ---------------------------------------------------------
           10.1.2.1:49153          0
           10.1.2.2:49153 0.00647334
           10.1.2.3:49153 0.00426833
  =========================================================
  9.99949 server sent 41 bytes to 10.1.2.1 port 49153
  9.99949 server sent 41 bytes to 10.1.2.2 port 49153
  9.99949 server sent 41 bytes to 10.1.2.3 port 49153

Here, one packet sent leads to three packets echoed, and the data under the
'Session' column shows the time in seconds since the last packet was
received from each client.  In mode 'InfSession', there is no explicit
timeout, but if we set a timeout to be something small, such as 5ms, and
set the mode to 'TimeoutLimited', we can suppress the response to node
10.1.2.2 because it has last polled the server over 6ms ago.

::

  $ ./ns3 run 'example-udp-group-echo --nExtra=2 --timeout=5ms --mode=TimeoutLimited'

  ...

  9.99949 server received 41 bytes from 10.1.2.1 port 49153
  9.99949 number of clients: 3
  ---------------------------------------------------------
                 Client     Session
  ---------------------------------------------------------
           10.1.2.1:49153          0
           10.1.2.2:49153 0.00647334 **Session Expired!**
           10.1.2.3:49153 0.00426833
  =========================================================
  9.99949 server sent 41 bytes to 10.1.2.1 port 49153
  9.99949 server sent 41 bytes to 10.1.2.3 port 49153

As seen above, the session has expired to 10.1.2.2 with this timeout and
mode setting, so the echo response is suppressed.

The C++ code for setting the server is fairly standard |ns3| syntax
and container/helper-based code, as exemplified below:

::

  uint16_t serverPort = 9;
  // Attributes 'timeout', 'mode', 'echoClient' may be set above
  UdpGroupEchoServerHelper echoServer (serverPort);
  echoServer.SetAttribute ("Timeout", TimeValue (timeout));
  echoServer.SetAttribute ("Mode", StringValue (mode));
  echoServer.SetAttribute ("EchoClient", BooleanValue (echoClient));
  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (simTime));

