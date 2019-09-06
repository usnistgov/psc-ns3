.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

Mission Critical Push-to-Talk (MCPTT)
*************************************

An MCPTT service provides a reliable, always-on, always-available means of
communication for first responders. This communication can be one-to-one or
one-to-many. There are two modes of operation: on-network and off-network.
In on-network mode, communication takes place via a client/server setup, which
means that, in LTE, a UE sends signals to an eNodeB, and talks with an MCPTT
server via the core-network. But in off-network mode, communication is
supported by UE devices in a peer-to-peer like fashion, where signals are only
sent from UE to UE.

A call is the logical channel between MCPTT applications. Call control is the
protocol used to create and manage these logical channels. There are three
types of calls that can take place between a set of users: basic, broadcast,
and private. A basic group call is one where a logical channel is setup for a
group of users associated with a particular group ID so that members of the
group can contend to talk in order to communicate with the other members of
the group. A broadcast group call is like a basic group call, but only the
initiator is allowed to speak, and once the initiator is done speaking the call
is terminated. A private call exists when there is a logical channel between
two applications for two users to communicate. Each type of call may be basic,
imminent peril, or emergency, and this "call status" dictates which physical
resources are used by this call.

When off-network, all call control messages are sent to all other applications
using the same socket. When a user wants to start or join a group call for a
particular group ID, the UE sends out periodic "GROUP CALL PROBE" messages. If
a UE with same group ID is already part of an on-going call, the UE that
received the probe will respond with a "GROUP CALL ANNOUNCEMENT" message. When
a UE who is not part of an on-going call receives this "GROUP CALL
ANNOUNCEMENT" message, the UE then automatically joins the call or waits for
the user to choose between joining the call or ignoring the call. This
announcement message is also sent periodically by members of an on-going call.
While part of a group call, floor control (described in the following
paragraph) is used to control which UE has permission to send media, and thus,
which user is allowed to talk, at any given time during a call. A user may also
change the "status" of a call at any point and this is communicated via the
"GROUP CALL ANNOUNCEMENT" message.

In an MCPTT group call, only a single member of the group is allowed to talk
at a time, and during this time all other group members need to listen. This
is facilitated by the floor control protocol. The protocol consists of floor
participants and a floor control server. The floor participants make requests
to the floor control server, while the floor control server receives and
handles these requests. In the on-network version there is a centralized MCPTT
server that acts as the floor control server, but in the off-network case, the
UE of the current speaker is the one acting as the floor control server (also
known as the current arbitrator). It is optional to use floor control for a
private call.

When off-network, all floor control messages are sent to all members of the
group. When a user wants to talk, which is indicated by a PTT press during a
call, the UE requests permission to send data by sending a "Floor Request"
message. The current arbitrator of the call, which is the device of the user
who is currently allowed to speak, will send a "Floor Granted" message to give
permission to another UE to transmit media, allowing the requesting user to
speak to the other members of the group, a "Floor Deny" message to deny the UE
permission to send media, or a "Floor Queue Info" message if the UE must wait
to transmit media. After sending a request, if the UE does not receive an
appropriate message after a given amount of time, the UE will assume that there
is currently no arbitrator and will send a "Floor Taken" message, giving itself
permission to transmit media.

Design Documentation
====================

The MCPTT model was implemented based on the 3GPP specifications 
TS 24.379 [TS24379]_ and TS 24.380 [TS24380]_ release 14.4.0. 
This includes off-network models for floor control,
basic group calls, broadcast group calls, and private calls. There are also
several test cases that were invented by NIST that are also included with this
model. This model was created and is intended to be used with the Proximity
Services (ProSe) which is provided by LTE.

The MCPTT model includes:

- A set of classes that can be used to model an MCPTT application using call
  control and floor control in the off-network case
- Helper classes to act as a sink for time-sensitive message and state change
  traces that can be used to capture the behavior of the different off-network
  MCPTT state machines (i.e. protocols)
- A helper class that allows users to configure and deploy MCPTT applications
  in a scenario with nodes
- A few examples that show how the model can be used
- Test cases produced by NIST to check state machine behavior

The MCPTT model does not take into account security. The model only goes as
far as including dummy variables in appropriate messages that would carry
security fields. Also, the MCPTT model currently does not make use of ProSe
Per-Packet Priority (PPPP). Even though the MCPTT model includes and
appropriately maintains PPPP as specified in the standards, there currently
is no support in ProSe for this feature and, thus, it is not used.

Note: MCPTT was created specifically for LTE, and the main component that ties
      MCPTT to LTE is the use of PPPP, which is provided to lower layers of LTE
      when sending messages for physical resource selection, but the current
      implementation of the off-network model does not make use of this feature
      since there is currently no support for it in the current ns-3
      implementation of ProSe. This means that the current MCPTT model is
      capable of being used over more technologies than just LTE since it is
      just an application in |ns3|.

Design
######

In |ns3|, the ``ns3::McpttHelper`` is the main class that a user should use to
initialize and install applications to an ``ns3::Node``. This helper can be used
to initially configure some of the components that are associated with an MCPTT
application, such as the application's data rate when generating media,
the random variable that should be used to simulate a PTT, the IP address used
by the application when addressing the group, etc. Once created and installed,
it is up to the user to setup calls. This can be done before or after the
application is started but not "dynamically" by the application itself. So if
a user wanted a scenario with three UE's that will eventually take part in the
same on-going group call, the user will have to first query the
``ns3::McpttPttApplication`` objects of the three ``ns3::Node``'s to create
those calls. Once the calls are created, the user will also have to query the
application to select the current call to reflect things such as a PTT or call
release at any given time throughout the simulation.

In the following code snippet, a basic group call is created for N users with
group ID 1. One second after the applications are started, the first user will
push the PTT button.

.. sourcecode:: cpp

  ObjectFactory callFac;
  callFac.SetTypeId (McpttCallMachineGrpBasic::GetTypeId ());
  callFac.Set ("GroupId", UintegerValue (1));
  ObjectFactory floorFac;
  floorFac.SetTypeId (McpttFloorMachineBasic::GetTypeId ());

  for (uint32_t idx = 0; idx < clientApps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = 
        DynamicCast<McpttPttApp, Application> (clientApps.Get (idx));
      pttApp->CreateCall (callFac, floorFac);
      pttApp->SelectCall (0);

      if (idx == 0)
        {
          Simulator::Schedule (Seconds (start.GetSeconds () + 1.0), 
            &McpttPttApp::TakePushNotification, pttApp);
        }
    }


MCPTT Application
~~~~~~~~~~~~~~~~~

The ``ns3::McpttPttApp`` is the core component of the MCPTT model. It is the
object used to manages calls and provide an API with functions that would be
available to a user like, starting a call, releasing a call, entering an
emergency alert, etc. It also houses a few entities to help simulate the
behavior of how an MCPTT application may be used. This class also implements
the ``ns3::McpttMediaSink`` interface, which allows for an instance of the
``ns3::McpttMediaSrc`` class to generate and pass media messages for
transmission, and the ``ns3::McpttPushable`` interface, which can be attached
to by an ``ns3::McpttPusher`` to simulate button push and release events.

The ``ns3::McpttMediaSrc`` is simply a class that generates RTP media messages
to be sent. It is used by the application to help model data traffic in the
network. It is a rather simple traffic model but can be configured to alter
data rate and message size.

The ``ns3::McpttMsgParser`` is a class that reads the header of a packet to
determine which message is being received. This class is used by the
application as well as in some test cases.

The ``ns3::McpttPusher`` is class used to simulate the pushing of a button.
The application uses this class to simulate a user that is using the PTT
button. This model is also simple but allows the user to configure it with
a random variable to reflect how often a user may push and release the PTT
button.

The ``ns3::McpttCall`` class simply aggregates the components needed to have an
MCPTT call. This includes a channel for both floor control and media messages,
and state machines for call control and floor control.

The ``ns3::McpttChan`` class is a wrapper around the ``ns3:Socket`` class to
provide an interface similar to the functions required by the specifications,
as well as handle opening and closing sockets on the fly. This class is used
by the ``ns3::McpttCall`` and ``ns3::McpttPttApp`` classes.

The ``ns3::McpttCallMachine`` is an interface created to handle the different
types of calls that are available. This includes functions for starting and
leaving calls. There are many subclasses:
* ``ns3::McpttCallMachineGrpBasic`` for basic group calls
* ``ns3::McpttCallMachineGrpBroadcast`` for broadcast group calls
* ``ns3::McpttCallMachinePrivate`` for private calls, and
* ``ns3::McpttCallMachineNull`` to "turn-off" call control.
Each of the state machines except for the null state machine have several
classes associated with each to represent the different states of the state
machine. For example, the class ``ns3::McpttCallMachineGrpBroadcastStateB1``
is a model of the "B1: start-stop" state of the Broadcast call control state
machine from the 3GPP standard describing call control.

The ``ns3::McpttFloorMachine`` is an interface created to represent the floor
control protocol. There are two subclasses:
- ``ns3::McpttFloorMachineBasic`` for the floor control protocol
- ``ns3::McpttFloorMachineNull`` to "turn-off" floor control

Just like the state machines for call control, the
``ns3:McpttFloorMachineBasic`` class has an
``ns3::McpttFloorMachineBasicState`` member which is derived by many classes
such as ``ns3:McpttFloorMachineBasicStateHasPerm`` to represent the different
states of the floor control state machine.

The ``ns3::McpttEmergAlertMachine`` is an interface to represent the state
machine used to maintain a user's emergency alert status. This can be found in
TS 24.379 [TS24379]_. There is currently one subclass:
- ``ns3::McpttEmergAlertMachineBasic`` for the emergency alert protocol

In the standard, there is one emergency alert machine associated with one user,
but in the current implementation there is an emergency alert machine
associated with each ``ns3::McpttCallMachineGrpBasic`` instance. Unlike the
previous state machines, there is no "state" class associated with this machine,
all the logic is self-contained.

The ``ns3::McpttCallTypeMachine`` is an interface for the call type machines
described in the standard. These state machines exist to maintain the call type
(e.g. basic, emergency, etc.) of a call. There are two subclasses:
* ``ns3::McpttCallTypeMachineGrpBasic`` for a basic group call, and
* ``ns3::McpttCallTypeMachinePriv`` for a private call.
    
These state machines are also self-contained and are not associated with any
"state" classes.

Most state machines, if not all, have many members that are of the types
``ns3::McpttCounter`` and ``ns3::McpttTimer``. These classes are used to
provide an API to mirror the actions that can be requested of counters and
timers that are defined throughout the standards. The ``ns3::McpttCounter``
class provides an interface that can be used as counters are described in the
standard with functions like, "Increment" and "Reset". The ``ns3::McpttTimer``
class does the same for timers described in the standard with functions like
"Start", "Stop", and "Restart". The ``ns3::McpttTimer`` is simply a wrapper
around the ``ns3::Timer`` class.

The ``ns3::McpttMsg`` class is a base class for all MCPTT off-network messages.
This class is derived from by the ``ns3::McpttCallMsg`` class for call control
messages, the ``ns3::McpttFloorMsg`` for floor control, and the
``ns3::McpttMediaMsg`` for RTP media messages. The ``ns3::McpttCallMsg`` and
``ns3::McpttFloorMsg`` classes both have many subclasses to represent each
floor control or call control message used in the off-network portion of the
standard. For example, the ``ns3::McpttFloorMsgRequest`` class represents the
"Floor Request" message described in TS 24.380 [TS24380]_. These classes are 
sent between
the MCPTT applications and are consumed by the appropriate state machines
throughout the simulation.

As mentioned above, the ``ns3::McpttMediaMsg`` class is used to represent an
RTP media message, but the actual header for an RTP packet is modeled by the
``ns3::McpttRtpHeader`` class. This class just defines the fields needed for
a basic RTP header which is used by the media message.

The ``ns3::McpttCallMsgField`` and ``ns3::McpttFloorMsgField`` classes
represent call control and floor control message fields, respectively. These
classes are also just used as a base and have many child classes. For example,
the ``ns3::McpttFloorMsgRequest`` class, which represents a "Floor Request"
message contains a member of type ``ns3::McpttFloorMsgFieldUserId`` that
describes the ID of the MCPTT user making the floor request.

The ``ns3::McpttEntityId`` class only exist for simulation to associate an ID
with various entities described in the standard. One may find that counters,
timers, and various states have an ID, and this ID just helps distinguish
between multiple objects of the same type.

The ``ns3::McpttFloorQueue`` class is a wrapper around the ``std::vector``
class that provides an interface for the floor queue that is described in TS
24.380 [TS24380]_. This class is only associated with a floor machine for 
use when
queueing is enabled in floor control. One can enable queueing in floor control
for a particular call, simply by setting the capacity of this queue to a size
greater than zero.

The ``ns3::McpttQueuedUserInfo`` class is used by the ``ns3::McpttFloorQueue``
class to represent the structure of information needed to store for a user
when they are placed in the queue, and made to wait to transmit during floor
control.

Helpers
~~~~~~~

There are four helpers:
  * ``ns3::McpttHelper`` for deploying MCPTT applications,
  * ``ns3::McpttMsgStats`` for tracing transmitted MCPTT application messages,
  * ``ns3::McpttProseCollisionDetector`` for examining ProSe operation, and
  * ``ns3::McpttStateMachineStats`` for tracing state machine state transitions.

As stated previously, the ``ns3::McpttHelper`` is used to configure and deploy
MCPTT applications. This is the class that a user should use to configure
settings that will be common across the applications and use it to create
several copies of the same configuration.

The ``ns3::McpttMsgStats`` class has the necessary sink functions to connect
to and trace messages sent from app to app. One can set the name of the output
file by setting the ``ns3::McpttMsgStats::OutputFileName`` attribute. The user
can also specify which types of messages should be captured. So if a user
wanted to capture all message types the user should then set,
``ns3::McpttMsgStats::CallControl``, ``ns3::McpttMsgStats::FloorControl``, and
``ns3::McpttMsgStats::Media`` attributes to "true". If the user also wishes to
include not just the type of message in the trace but also the contents of
the message the user can set ``ns3::McpttMsgStats::IncludeMessageContent`` to
"true" as well.

The ``McpttProseCollisionDetector`` class is used in the setup delay example
for detecting when collisions occur in the PSCCH and PSSCH of the ProSe
Sidelink channel.

The ``ns3::McpttStateMachineStats`` class has the necessary functions to act as
a sink to trace state transitions of the various state machines throughout a
simulation. One can set the name of the output file by setting the
``ns3::McpttStateMachineStats::OutputFileName`` attribute.

Scope and Limitations
#####################

Only the logic of the protocols described for call control and floor control
were meant to be captured by the current MCPTT model. There is a feature
described throughout the call control document called PPPP or Prose Per-Packet
Priority that is taken into consideration at the application but is not
propagated to the lower layers as specified because currently this feature is
not supported by the ProSe model.

User Documentation
==================

Examples
########

There are two MCPTT examples in the 'psc/examples' folder:
  * ``example-mcptt.cc`` is a basic scenario with two users deployed randomly
    using the |ns3| WiFi module in Adhoc mode
  * ``mcptt-lte-sl-out-of-covrg-comm.cc`` is an adaptation of the LTE Sidelink
    example ``lte-sl-out-of-covrg-comm`` Mode 2 ProSe example
..
   Note: the below is commented out until this program is made available

   * ``mcptt-setup-delay.cc`` is a scenario configured specifically for
     simulating a single PTT to determine how long it would take a user to
     join a call or request the floor while communicating over ProSe.

   For the setup delay example, one can use the message and state machine traces
   to compute there own metrics or if the log component ``McpttProseExample`` is
   enabled at the "logic" level, the user will see the computed setup delays and
   reported error cases.

mcptt-lte-sl-out-of-covrg-comm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The program ``mcptt-lte-sl-out-of-covrg-comm.cc`` is an adaptation of the
LTE Sidelink example ``lte-sl-out-of-covrg-comm.cc`` documented in the
LTE module.  The example was adapted to replace the simple UDP application
with MCPTT.  The following code excerpts highlight the main aspects of
configuring MCPTT in a program.

The first block of code below highlights the use of the class
``ns3::McpttHelper`` to encapsulate configuration statements on the key
objects involved in the MCPTT service.  The helper exposes some methods
that allow for custom configuration of the PttApp class and attributes,
MediaSrc class and attributes, and Pusher class and attributes.  In the
below, the configuration of ``ns3::McpttPusher`` to Automatic operation
means that the pushing and releasing times will be driven by random
variables, and additional methods to easily configure these random variables
are provided.  Following the configuration of these objects, the usual
pattern in |ns3| for using the application helper to install onto the
set of ueNodes, configure a start time, and configure a stop time are
followed.

.. sourcecode:: cpp

   ApplicationContainer clientApps;
   McpttHelper mcpttHelper;
   if (enableNsLogs)
     {
       mcpttHelper.EnableLogComponents ();
     }
   mcpttHelper.SetPttApp ("ns3::McpttPttApp",
                          "PeerAddress", Ipv4AddressValue (peerAddress),
                          "PushOnStart", BooleanValue (true));
   mcpttHelper.SetMediaSrc ("ns3::McpttMediaSrc",
                          "Bytes", UintegerValue (msgSize),
                          "DataRate", DataRateValue (dataRate));
   mcpttHelper.SetPusher ("ns3::McpttPusher",
                          "Automatic", BooleanValue (true));
   mcpttHelper.SetPusherPushVariable ("ns3::NormalRandomVariable",
                          "Mean", DoubleValue (pushTimeMean),
                          "Variance", DoubleValue (pushTimeVariance));
   mcpttHelper.SetPusherReleaseVariable ("ns3::NormalRandomVariable",
                          "Mean", DoubleValue (releaseTimeMean),
                          "Variance", DoubleValue (releaseTimeVariance));
 
   clientApps.Add (mcpttHelper.Install (ueNodes));
   clientApps.Start (startTime);
   clientApps.Stop (stopTime);

The above will prepare each UE for the service, but call configuration
remains to be configured.  For a basic group call type, using the
basic floor machine, the helper provides a single statement to configure
the call, as follows.
 
.. sourcecode:: cpp

   mcpttHelper.ConfigureBasicGrpCall (clientApps, usersPerGroup);

This method encapsulates the following operations:

*  sets the call control state machine type to ``ns3::McpttCallMachineGrpBasic``
*  sets the floor control state machine type to ``ns3::McpttFloorMachineBasic``
*  iterates across the clientApps in the provided application container.  If
   the provided ``usersPerGroup`` value is equal to or greater than the size
   of the ``clientApps`` container, all instances of ``McpttPttApp`` will 
   be included in a call with the same GroupId.  If ``usersPerGroup`` is
   less than the size of ``clientApps``, the first ``usersPerGroup`` will
   be placed into a call with the first GroupId, the second ``usersPerGroup``
   will be placed into a call with the second GroupId, and so on until the
   ``clientApps`` have all been handled.  The base GroupId is an optional
   argument to this method, but defaults to the value of 1 if not provided.
*  creates the call instance on each ``McpttPttApp``

The various configuration variables used in the above are set near the top
of the main program, as follows.

.. sourcecode:: cpp

   // MCPTT configuration
   uint32_t usersPerGroup = 2;
   DataRate dataRate = DataRate ("24kb/s");
   uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
   double pushTimeMean = 5.0; // seconds
   double pushTimeVariance = 2.0; // seconds
   double releaseTimeMean = 5.0; // seconds
   double releaseTimeVariance = 2.0; // seconds
   Ipv4Address peerAddress = Ipv4Address ("225.0.0.0");
   Time startTime = Seconds (2);
   Time stopTime = simTime;

Here, it is worth noting that the configuration sets two users per group,
and the example only has two UEs, so both UEs will belong to the same GroupId.
Also, the peerAddress value is set to an IPv4 multicast address.  This value
should not be changed without similarly changing the IPv4 group address of
the TFT configuration further down in the LTE configuration part of
the program.  The TFT controls the mapping of IP packets to sidelink
bearers.  If the packets are sent to a peer address for which a suitable
TFT is not configured, they will be dropped in the sending UE's stack. 

Another difference with respect to the D2D example is that, presently,
only IPv4 addressing is supported, so there is no command-line option to 
use IPv6.

A further difference with respect to the D2D example is that the command-line
option to enable |ns3| logging, ``--enableNsLogs``, will not enable LTE
logging as in the LTE D2D example, but will instead enable all MCPTT logs,
as shown above.

Finally, we note two tracing statements inserted near the bottom of the
program: 
 
.. sourcecode:: cpp

   NS_LOG_INFO ("Enabling MCPTT traces...");
   mcpttHelper.EnableMsgTraces ();
   mcpttHelper.EnableStateMachineTraces ();

These statements are explained in the next section.  Some other aspects
of LTE tracing are omitted in this modified example, in order to focus on
the MCPTT configuration.

Traces
######

There are currently only two traces that can be activated by using the
``ns3::McpttHelper``, and this can be done by following the example given
below, after all the applications have been created.

.. sourcecode:: cpp

  NS_LOG_INFO ("Enabling MCPTT traces...");
  mcpttHelper.EnableMsgTraces (); // Enable message trace
  mcpttHelper.EnableStateMachineTraces (); // Enable state trace

The ``ns3::McpttMsgStats`` class is used for tracing MCPTT messages at
the application layer and produces a file with the default name
"mcptt_msg_stats.txt" with the following file format.

.. sourcecode:: text

  time(ms)	userid	rx/tx	bytes

There may also be an additional field at the end of the row called "message"
that will be included if the ``ns3::McpttMsgStats::IncludeMessageContent``
attribute is set to "true". The "time(ms)" column describes the time (in
milliseconds) at which a message was sent/received. The "userid" column
contains the user ID of the MCPTT user that sent/received the message. The
"rx/tx" column indicates if the message was sent or received, i.e. if "rx"
is the value in the column then that means the message was received, while a
value of "tx" indicates that the message was sent. The "bytes" columns
indicates the size (in bytes) of the message that was generated at the
application layer. The "message" column, if present, includes the string
representation of the message that was sent and includes message field names
and values.

The ``ns3::McpttStateMachineStats`` is used for tracing state machine state
transitions and produces a file with the default name,
"mcptt_state_machine_stats.txt", with the following format.

.. sourcecode:: text

  time(ms)	userid	callid	typeid	oldstate	newstate

The "time(ms)" column describes the time (in milliseconds) at which the state
transition took place. The "userid" column contains the user ID of the MCPTT
user that the state machine is associated with. The "callid" column contains
the ID of the call the state machine is associated with. The "typeid" column
contains the string representation of the state machine's ``ns3::TypeId``.
The "oldstate" column contains the string representation of the
``ns3::McpttEntityId`` which gives the name of the state that the state
machine was in BEFORE the transition took place. And finally, the "newstate"
column contains the string representation of the ``ns3::McpttEntityId`` which
gives the name of the state that the state machine was in AFTER the transition
took place.

Testing Documentation
=====================

The National Institute of Standards and Technology (NIST) used the 3GPP
standards to create over 50 test cases. More information about those test cases
can be found in NIST technical report NISTIR 8236 [NIST.IR.8236]_.

.. References below will be placed in separate Bibliography section

.. [TS24379] 3GPP TS 24.379 `"Mission Critical Push To Talk (MCPTT) call control; Protocol specification"
   <http://www.3gpp.org/ftp//Specs/archive/24_series/24.379/24379-e40.zip>`_

.. [TS24380] 3GPP TS 24.380 `"Mission Critical Push To Talk (MCPTT) media plane control; Protocol specification"
   <http://www.3gpp.org/ftp//Specs/archive/24_series/24.380/24380-e40.zip>`_

.. [NIST.IR.8236] Varin, P., Sun, Y., and Garey, W., `"Test Scenarios for Mission Critical Push-To-Talk (MCPTT) Off-Network Mode Protocols Implementation" <https://doi.org/10.6028/NIST.IR.8236>`_, October 2018.
