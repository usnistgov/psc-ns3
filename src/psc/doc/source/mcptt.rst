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

Model Description
=================

The purpose of the MCPTT model is to enable research into the performance
of MCPTT protocols in both off-network and on-network public safety scenarios. 
Users are expected to be interested in studying Key Performance Indicators
(KPIs) such as the push-to-talk access time (KPI1) or the mouth-to-ear
latency (KPI3) metrics defined in Section 6.15.3 of [TS22179]_.

Design
######

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

In the specifications, on-network calls are described not only by their 
call type (basic, broadcast, and private) but also by how the supporting 
session is established (on-demand or pre-established), the group call
model (pre-arranged or chat group (restricted)), and whether the call
requires elevated access privileges to network resources (emergency
or imminent peril calls).  The mode of call commencement (automatic or
manually with user intervention) also can be defined.  The difference 
between an on-demand and pre-established session is that in the 
pre-established case, certain network setup steps are completed before 
a user initiates the call; these steps include negotiation of IP 
addresses and ports (e.g., Interactive Connectivity Establishment (ICE) procedures) 
and coordination of bearers in the IP Media Subsystem (IMS) core.  In a 
prearranged group call, the group members will be 
defined a priori, while the chat group (restricted) call model allows 
members to join calls without being invited.  The initial support in 
|ns3| for on-network call types is the 'prearranged group call, using 
on-demand session, with automatic commencement,' 
defined in TS 24.379 [TS24379]_.

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
at a time (with the exception of the dual-speaker feature of on-network), and
during this time all other group members need to listen. This is facilitated
by the floor control protocol. The protocol consists of floor participants and
a floor control server. The floor participants make requests to the floor
control server, while the floor control server receives and handles these
requests. In the on-network version there is a centralized MCPTT server that
acts as the floor control server, but in the off-network case, the UE of the
current speaker is the one acting as the floor control server (also known as
the current arbitrator). It is optional to use floor control for a private
call.

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
  MCPTT state machines (i.e., protocols)
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

.. note::
   MCPTT was created specifically for LTE, and the main component that ties
   off-network MCPTT to LTE is the use of PPPP, which is provided to lower
   layers of LTE  when sending messages for physical resource selection, but
   the current implementation of the off-network model does not make use of
   this feature since there is currently no support for it in the current
   ns-3 implementation of ProSe. This means that the current MCPTT model is
   capable of being used over more technologies than just LTE since it is
   just an application in |ns3|.

Design
######

In |ns3|, the ``ns3::psc::McpttHelper`` is the main class that a user should use to
initialize and install applications to an ``ns3::Node``. This helper can be used
to initially configure some of the components that are associated with an MCPTT
application, such as the application's data rate when generating media,
the random variable that should be used to simulate a PTT, the IP address used
by the application when addressing the group, etc. Once created and installed,
it is up to the user to setup calls. This can be done before or after the
application is started but not "dynamically" by the application itself. So if
a user wanted a scenario with three UE's that will eventually take part in the
same on-going group call, the user will have to first query the
``ns3::psc::McpttPttApplication`` objects of the three ``ns3::Node``'s to create
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
  floorFac.SetTypeId (McpttOffNetworkFloorParticipant::GetTypeId ());

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

Identifiers
~~~~~~~~~~~

Several MCPTT identifiers are used throughout the code, as follows.

  * ``MCPTT User ID`` The MCPTT User ID is, in practice, a SIP URI (i.e.,
    a string value).  In this ns-3 model, for simplicity, it is stored as a
    32-bit unsigned integer.  Clients for which the ``ns3::psc::McpttHelper``
    installs an instance of the ``ns3::psc::McpttPttApp`` will be assigned a
    unique user ID value, starting from the initial value of 1.  In addition,
    the MCPTT server is separately assigned a user ID, stored in the
    ``ns3::psc::McpttServerCallMachine`` instance.  This is initialized by default
    to 0, and typically left at the default value.
 
  * ``MCPTT Group ID`` The MCPTT Group ID pertains to group calls; it is also,
    in practice, a SIP URI (i.e., a string value).  In this ns-3 model, for
    simplicity, it is stored as a 32-bit unsigned integer.  On clients,
    the value is stored in the base class ``ns3::psc::McpttCallMachineGrp`` as an
    attribute, and similarly, on the server, it is stored in an attribute
    in the base class ``ns3::psc::McpttServerCallMachineGrp``.  The group ID
    is usually assigned explicitly by the simulation user as part of
    call definition.

  * ``SSRC (synchronization source)`` This 32-bit identifier is defined
    for RTP (see RFC 3550) and is, in practice, supposed to be assigned
    randomly so as to avoid collisions.  The SSRC is associated with a
    media stream component, and the identifier is carried in RTP and
    floor control messages.  In on-network operation, an SSRC is also
    assigned to the server (included in server-originated floor control
    messages).  The on-network arbitrator,
    ``ns3::psc::McpttOnNetworkFloorArbitrator``, contains an attribute 
    ``TxSsrc`` that defaults to the value 0.
    For simplicity, the clients reuse the MCPTT User ID value for SSRC; 
    i.e., all media streams are identified as being from the same source,
    in the current version of this model.

MCPTT Application
~~~~~~~~~~~~~~~~~

The ``ns3::psc::McpttPttApp`` is the core component of the MCPTT model. It is the
object used to manage calls and provide an API with functions that would be
available to a user like, starting a call, releasing a call, entering an
emergency alert, etc. It also houses a few entities to help simulate the
behavior of how an MCPTT application may be used. This class also implements
the ``ns3::psc::McpttMediaSink`` interface, which allows for an instance of the
``ns3::psc::McpttMediaSrc`` class to generate and pass media messages for
transmission.

The ``ns3::psc::McpttMediaSrc`` is simply a class that generates RTP media messages
to be sent. It is used by the application to help model data traffic in the
network. It is a rather simple traffic model but can be configured to alter
data rate and message size.

The ``ns3::psc::McpttMsgParser`` is a class that reads the header of a packet to
determine which message is being received. This class is used by the
application as well as in some test cases.

The ``ns3::psc::McpttPusher`` is class used to simulate the pushing of a button.
The application uses this class to simulate a user that is using the PTT
button. This model is also simple but allows the user to configure it with
a random variable to reflect how often a user may push and release the PTT
button.

The ``ns3::psc::McpttCall`` class simply aggregates the components needed to have an
MCPTT call. This includes a channel for both floor control and media messages,
and state machines for call control and floor control.

The ``ns3::psc::McpttChannel`` class is a wrapper around the ``ns3:Socket`` class to
provide an interface similar to the functions required by the specifications,
as well as handle opening and closing sockets on the fly. This class is used
by the ``ns3::psc::McpttCall`` and ``ns3::psc::McpttPttApp`` classes.

The ``ns3::psc::McpttCallMachine`` is an interface created to handle the different
types of calls that are available. This includes functions for starting and
leaving calls. There are many subclasses:

* ``ns3::psc::McpttCallMachineGrpBasic`` for basic group calls
* ``ns3::psc::McpttCallMachineGrpBroadcast`` for broadcast group calls
* ``ns3::psc::McpttCallMachinePrivate`` for private calls, and
* ``ns3::psc::McpttCallMachineNull`` to "turn-off" call control.

Each of the state machines except for the null state machine have several
classes associated with each to represent the different states of the state
machine. For example, the class ``ns3::psc::McpttCallMachineGrpBroadcastStateB1``
is a model of the "B1: start-stop" state of the Broadcast call control state
machine from the 3GPP standard describing call control.

The ``ns3::psc::McpttFloorParticipant`` is an interface created to represent the floor
control protocol. There are two subclasses:

- ``ns3::psc::McpttOffNetworkFloorParticipant`` for the floor control protocol
- ``ns3::psc::McpttFloorParticipantNull`` to "turn-off" floor control

Just like the state machines for call control, the
``ns3:McpttOffNetworkFloorParticipant`` class has an
``ns3::psc::McpttOffNetworkFloorParticipantState`` member which is derived by many classes
such as ``ns3:McpttOffNetworkFloorParticipantStateHasPerm`` to represent the different
states of the floor control state machine.

The ``ns3::psc::McpttEmergAlertMachine`` is an interface to represent the state
machine used to maintain a user's emergency alert status. This can be found in
TS 24.379 [TS24379]_. There is currently one subclass:

- ``ns3::psc::McpttEmergAlertMachineBasic`` for the emergency alert protocol

In the standard, there is one emergency alert machine associated with one user,
but in the current implementation there is an emergency alert machine
associated with each ``ns3::psc::McpttCallMachineGrpBasic`` instance. Unlike the
previous state machines, there is no "state" class associated with this machine,
all the logic is self-contained.

The ``ns3::psc::McpttCallTypeMachine`` is an interface for the call type machines
described in the standard. These state machines exist to maintain the call type
(e.g., basic, emergency, etc.) of a call. There are two subclasses:

* ``ns3::psc::McpttCallTypeMachineGrpBasic`` for a basic group call, and
* ``ns3::psc::McpttCallTypeMachinePriv`` for a private call.
    
These state machines are also self-contained and are not associated with any
"state" classes.

Most state machines, if not all, have many members that are of the types
``ns3::psc::McpttCounter`` and ``ns3::psc::McpttTimer``. These classes are used to
provide an API to mirror the actions that can be requested of counters and
timers that are defined throughout the standards. The ``ns3::psc::McpttCounter``
class provides an interface that can be used as counters are described in the
standard with functions like, "Increment" and "Reset". The ``ns3::psc::McpttTimer``
class does the same for timers described in the standard with functions like
"Start", "Stop", and "Restart". The ``ns3::psc::McpttTimer`` is simply a wrapper
around the ``ns3::Timer`` class.

The ``ns3::psc::McpttMsg`` class is a base class for all MCPTT off-network messages.
This class is derived from by the ``ns3::psc::McpttCallMsg`` class for call control
messages, the ``ns3::psc::McpttFloorMsg`` for floor control, and the
``ns3::psc::McpttMediaMsg`` for RTP media messages. The ``ns3::psc::McpttCallMsg`` and
``ns3::psc::McpttFloorMsg`` classes both have many subclasses to represent each
floor control or call control message used in the off-network portion of the
standard. For example, the ``ns3::psc::McpttFloorMsgRequest`` class represents the
"Floor Request" message described in TS 24.380 [TS24380]_. These classes are 
sent between
the MCPTT applications and are consumed by the appropriate state machines
throughout the simulation.

As mentioned above, the ``ns3::psc::McpttMediaMsg`` class is used to represent an
RTP media message, but the actual header for an RTP packet is modeled by the
``ns3::psc::McpttRtpHeader`` class. This class just defines the fields needed for
a basic RTP header which is used by the media message.

The ``ns3::psc::McpttCallMsgField`` and ``ns3::psc::McpttFloorMsgField`` classes
represent call control and floor control message fields, respectively. These
classes are also just used as a base and have many child classes. For example,
the ``ns3::psc::McpttFloorMsgRequest`` class, which represents a "Floor Request"
message contains a member of type ``ns3::psc::McpttFloorMsgFieldUserId`` that
describes the ID of the MCPTT user making the floor request.

The ``ns3::psc::McpttEntityId`` class only exist for simulation to associate an ID
with various entities described in the standard. One may find that counters,
timers, and various states have an ID, and this ID just helps distinguish
between multiple objects of the same type.

The ``ns3::psc::McpttFloorQueue`` class is a wrapper around the ``std::vector``
class that provides an interface for the floor queue that is described in TS
24.380 [TS24380]_. This class is only associated with a floor machine for 
use when
queuing is enabled in floor control. One can enable queuing in floor control
for a particular call, simply by setting the capacity of this queue to a size
greater than zero.

The ``ns3::psc::McpttQueuedUserInfo`` class is used by the ``ns3::psc::McpttFloorQueue``
class to represent the structure of information needed to store for a user
when they are placed in the queue, and made to wait to transmit during floor
control.

Pusher Behavior
~~~~~~~~~~~~~~~

Supplement to the MCPTT application, a pusher model exists to simulate the
PTT behavior for a group of MCPTT users on a call. This model consists of a
centralized entity derived from the ``ns3::psc::McpttPusherOrchestrator`` class
that determines the interarrival time (IAT) and duration of PTT events for
a set of ``ns3::psc::McpttPusher`` objects. The value of this model comes from its
ability to simulate the PTT behavior for a group based on data that was
collected and generalized from real public safety call logs. Using the data
from the call logs two main elements were extracted and realized by the model,
and they are, PTT and session durations. In essence, a session is a time span
when users (i.e., instances of the ``ns3::psc::McpttPusher`` class) are actively
being scheduled to push and release the PTT button, while the PTT duration
determines how long a PTT push will last. These durations are generated using
instances of the ``ns3::EmpiricalRandomVariable`` class that interpolate bins
taken from the Cumulative Distribution Function (CDF) curves that were
computed after parsing the call log data to determine PTT and session
durations. While the durations of PTTs and sessions will resemble the actual
data more precisely, the IATs of both PTTs and sessions are based on an
activity factor that is set by the user. They are computed using instances of
the ``ns3::ExponentialRandomVariable``, which is assumed to be the
distribution of IATs. This means that the following sequence of
non-overlapping steps is typical for generating PTT and session events: IAT,
duration, IAT, duration, and so on.

The ``ns3::psc::McpttPusherOrchestratorInterface`` is a base class for the subclasses:

* ``ns3::psc::McpttPusherOrchestrator``,
* ``ns3::psc::McpttPusherOrchestratorSpurtCdf``,
* ``ns3::psc::McpttPusherOrchestratorSessionCdf``, and
* ``ns3::psc::McpttPusherOrchestratorContention``. 

The class
``ns3::psc::McpttPusherOrchestrator`` simply selects a random pusher from
the set of pushers being orchestrated and uses two instances of an
``ns3::RandomVariableStream``. One for deciding push durations and another for
deciding the IAT of push events. The ``ns3::psc::McpttPusherOrchestratorSpurtCdf``
is a wrapper around the ``ns3::psc::McpttPusherOrchestrator`` class that
initializes the random variables used by ``ns3::psc::McpttPusherOrchestrator``
to match CDFs from the call logs based on the activity factor that was
provided. The ``ns3::psc::McpttPusherOrchestratorSessionCdf`` class is a decorator
that simply starts and stops an underlying orchestrator to create sessions
based on the CDFs from the call log and the activity factor set by the user.
Finally, the ``ns3::psc::McpttPusherOrchestratorContention`` class is also a
decorator that can be used to introduce contention, by using a threshold and
an instance of the ``ns3::UniformRandomVariable`` class to determine if and
when an additional PTT event should occur to collide with another active PTT
event.

.. _fig-mcptt-ptt-duration-cdf:

.. figure:: figures/mcptt-ptt-duration-cdf.*
   :scale: 75 %

   PTT Duration CDF

.. _fig-mcptt-session-duration-cdf:

.. figure:: figures/mcptt-session-duration-cdf.*
   :scale: 75 %

   Session Duration CDF

For ``ns3::psc::McpttPusherOrchestratorSpurtCdf`` the PTT duration is based on the CDF
of PTT durations in figure :ref:`fig-mcptt-ptt-duration-cdf`, while the mean
value given to the exponential random variable responsible for generating the
IAT is computed as:

Average PTT IAT = (Average PTT Duration) * ((1 / Voice Activity Factor) - 1)

Here, "Average PTT IAT" is the mean value given to the exponential random
variable, "Average PTT Duration" is 4.69 (which is the average PTT duration
computed from the CDF data), and "Voice Activity Factory" is a value between
(0, 1] that is set by the user.

For ``ns3::psc::McpttPusherOrchestratorSessionCdf`` the session duration is based
on the CDF in figure :ref:`fig-mcptt-session-duration-cdf`, while the mean
value given to the exponential random variable responsible for generating
session IAT is computed as:

Average Session IAT = (Average Session Duration) * ((1 / Session Activity Factor) - 1)

Here, "Average Session IAT" is the mean value given to the exponential random
variable, "Average Session Duration" is 8.58 (which is the average session
duration computed from the CDF data), and "Session Activity Factory" is a
value between (0, 1] that is set by the user.

Note that the ``ns3::psc::McpttPusherOrchestratorContention`` class can affect the
overall activity factor since it can create additional PTT requests, but how
it will affect the activity factor depends on the protocol configuration
(e.g., queuing, preemption, etc.). However, based on a configurable Contention
Probability (CP) threshold, the probability that a single pusher's request
will occur at the same time as another pusher's request is:

Probability of Experiencing Contention = 2 * CP / (1 + CP)

The following code snippet gives an example of how the pusher model is
configured and attached to a set of applications. In this case the voice
activity factor during a session is 0.5, which means that there will be a PTT
event about 50 % of the time in each session, while the session activity
factor is 1.0, which means that there will be active sessions during the
entire simulation. The timeline in figure
:ref:`fig-mcptt-pusher-model-example` captures the activity of both pushers
and sessions for this configuration.

.. sourcecode:: cpp

  Ptr<McpttPusherOrchestratorSpurtCdf> spurtOrchestrator = CreateObject<McpttPusherOrchestratorSpurtCdf> ();
  spurtOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (1.0));

  Ptr<McpttPusherOrchestratorSessionCdf> sessionOrchestrator = CreateObject<McpttPusherOrchestratorSessionCdf> ();
  sessionOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (0.5));
  sessionOrchestrator->SetAttribute ("Orchestrator", PointerValue (spurtOrchestrator));

  sessionOrchestrator->StartAt (Seconds (2.0));
  sessionOrchestrator->StopAt (Seconds (82.0);

  mcpttHelper.Associate (sessionOrchestrator, clientApps);

It is also worth mentioning that if the Voice Activity Factor is less than
1.0, then depending on the combination of Voice Activity Factor and session
duration, it is possible to have a session that does not have any PTT events.
This is due to the fact that an active session only determines when PTT events
are possible but it does not guarantee that a PTT event will occur.

.. _fig-mcptt-pusher-model-example:

.. figure:: figures/mcptt-pusher-model-example.*
   :scale: 75 %

   Pusher Model Example

Helpers
~~~~~~~

There are five helpers:
  * ``ns3::psc::McpttHelper`` for deploying MCPTT applications,
  * ``ns3::psc::McpttMsgStats`` for tracing transmitted MCPTT application messages,
  * ``ns3::psc::McpttProseCollisionDetector`` for examining ProSe operation, and
  * ``ns3::psc::McpttStateMachineStats`` for tracing state machine state transitions.
  * ``ns3::psc::ImsHelper`` for adding an optional IMS;
      the IMS is modeled as a single node connected to the PGW.

As stated previously, the ``ns3::psc::McpttHelper`` is used to configure and deploy
MCPTT applications. This is the class that a user should use to configure
settings that will be common across the applications and use it to create
several copies of the same configuration.

The ``ns3::psc::McpttMsgStats`` class has the necessary sink functions to connect
to and trace messages sent from app to app. One can set the name of the output
file by setting the ``ns3::psc::McpttMsgStats::OutputFileName`` attribute. The user
can also specify which types of messages should be captured. So if a user
wanted to capture all message types the user should then set,
``ns3::psc::McpttMsgStats::CallControl``, ``ns3::psc::McpttMsgStats::FloorControl``, and
``ns3::psc::McpttMsgStats::Media`` attributes to "true". If the user also wishes to
include not just the type of message in the trace but also the contents of
the message the user can set ``ns3::psc::McpttMsgStats::IncludeMessageContent`` to
"true" as well.

The ``ns3::psc::McpttStateMachineStats`` class has the necessary functions to act as
a sink to trace state transitions of the various state machines throughout a
simulation. One can set the name of the output file by setting the
``ns3::psc::McpttStateMachineStats::OutputFileName`` attribute.

Scope and Limitations
#####################

Only the logic of the protocols described for call control and floor control
were meant to be captured by the current MCPTT model. There is a feature
described throughout the call control document called PPPP or Prose Per-Packet
Priority that is taken into consideration at the application but is not
propagated to the lower layers as specified because currently this feature is
not supported by the ProSe model. On-network currently only supports
pre-arranged group call, using an on-demand session, with automatic
commencement. Also, elevated call types, such as 'Emergency,' and the use of
Guaranteed Bit-Rate (GBR) bearers' are not modeled in on-network as this would
require additional features in the LTE model.

Usage
=====

Helpers
#######

Adding IP Multimedia Subsystem (IMS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An IMS node can be added to a simulation in a manner similar to the optional
configuration of an EPC network.  In the below snippet, the ImsHelper object
is LteHelper, and the ImsHelper also is connected to
the PGW node::

   Ptr<ImsHelper> imsHelper = CreateObject<ImsHelper> ();
   imsHelper->ConnectPgw (epcHelper->GetPgwNode ());

The IMS node itself can be fetched as follows::

   Ptr<Node> ims = imsHelper->GetImsNode ();

The ConnectPgw() method creates the IMS node, adds an IP (internet) stack to
it, and then adds a point-to-point link between it and the PGW node.  The
SGi interface is assigned the 15.0.0.0/8 network by default, although this
can be changed to another network.  Finally, a static route
towards the UE subnetwork (7.0.0.0/8) is inserted on the IMS.

Examples
########

There are two off-network MCPTT examples in the 'psc/examples' folder:
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

There are several on-network MCTT examples in the 'psc/examples' folder:
  * ``example-mcptt-on-network-floor-control-csma.cc`` demonstrates the
    operation of the on-network floor control with a single call on a CSMA network.
  * ``example-mcptt-on-network-floor-control-lte.cc`` demonstrates the
    operation of the on-network floor control with a single call on an LTE network.
  * ``example-mcptt-on-network-two-calls.cc`` demonstrates two sequential calls
    executing sequentially in the same LTE scenario.
  * ``example-mcptt-on-network-two-simultaneous-calls.cc`` demonstrates two
    concurrent calls in the same LTE network, each with different MCPTT
    server nodes, and an MCPTT user participating in both calls
  * ``mcptt-operational-modes-static.cc`` demonstrates how different modes of
    MCPTT (on-network, off-network, and on-network using UE-to-network relay)
    operate in parallel, with some support for plotting access time and
    mouth-to-ear latency KPIs.
  * ``mcptt-operational-modes-mobility.cc`` is similar in initial configuration
    to the static program above, but demonstrates mobility of three teams
    of four UEs into a building, and the relative performance impact of the
    three different modes of operation (on-network, off-network, and on-network
    with UE-to-network relay).

mcptt-lte-sl-out-of-covrg-comm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The program ``mcptt-lte-sl-out-of-covrg-comm.cc`` is an adaptation of the
LTE Sidelink example ``lte-sl-out-of-covrg-comm.cc`` documented in the
LTE module.  The example was adapted to replace the simple UDP application
with MCPTT.  The following code excerpts highlight the main aspects of
configuring MCPTT in a program.

The first block of code below highlights the use of the class
``ns3::psc::McpttHelper`` to encapsulate configuration statements on the key
objects involved in the MCPTT service.  The helper exposes some methods
that allow for custom configuration of the PttApp class and attributes,
MediaSrc class and attributes, and Pusher class and attributes.  In the
below, the configuration of ``ns3::psc::McpttPusher`` to Automatic operation
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
   mcpttHelper.SetPttApp ("ns3::psc::McpttPttApp",
                          "PeerAddress", Ipv4AddressValue (peerAddress),
                          "PushOnStart", BooleanValue (true));
   mcpttHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                          "Bytes", UintegerValue (msgSize),
                          "DataRate", DataRateValue (dataRate));
   mcpttHelper.SetPusher ("ns3::psc::McpttPusher",
                          "Automatic", BooleanValue (true));
   mcpttHelper.SetPusherPttInterarrivalTimeVariable ("ns3::NormalRandomVariable",
                          "Mean", DoubleValue (pushTimeMean),
                          "Variance", DoubleValue (pushTimeVariance));
   mcpttHelper.SetPusherPttDurationVariable ("ns3::NormalRandomVariable",
                          "Mean", DoubleValue (releaseTimeMean),
                          "Variance", DoubleValue (releaseTimeVariance));
 
   clientApps.Add (mcpttHelper.Install (ueNodes));
   clientApps.Start (startTime);
   clientApps.Stop (stopTime);

The above will prepare each UE for the service, but call configuration
remains to be configured.  For a basic group call type, using the
basic floor machine, the helper provides a single statement to configure
the call, as follows.
 
One could also use the following snippet to configure an
``ns3::psc::McpttPusherOrchestrator`` to control when push and release events
occur for a set of ``ns3::psc::McpttPttApp`` applications.

.. sourcecode:: cpp

   ApplicationContainer clientApps;
   McpttHelper mcpttHelper;
   if (enableNsLogs)
     {
       mcpttHelper.EnableLogComponents ();
     }
   mcpttHelper.SetPttApp ("ns3::psc::McpttPttApp",
                          "PeerAddress", Ipv4AddressValue (peerAddress),
                          "PushOnStart", BooleanValue (true));
   mcpttHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                          "Bytes", UintegerValue (msgSize),
                          "DataRate", DataRateValue (dataRate));
   mcpttHelper.SetPusher ("ns3::psc::McpttPusher",
                          "Automatic", BooleanValue (false));
 
   clientApps.Add (mcpttHelper.Install (ueNodes));
   clientApps.Start (startTime);
   clientApps.Stop (stopTime);

   Ptr<McpttPusherOrchestratorSpurtCdf> orchestrator = CreateObject<McpttPusherOrchestratorSpurtCdf> ();
   orchestrator->SetAttribute ("ActivityFactor", DoubleValue (0.5));
   orchestrator->StartAt (startTime);

   mcpttHelper.Associate (orchestrator, clientApps);

The following method:

.. sourcecode:: cpp

   mcpttHelper.ConfigureBasicGrpCall (clientApps, usersPerGroup);

encapsulates the following operations:

*  sets the call control state machine type to ``ns3::psc::McpttCallMachineGrpBasic``
*  sets the floor control state machine type to ``ns3::psc::McpttOffNetworkFloorParticipant``
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

Finally, we note four tracing statements inserted near the bottom of the
program: 
 
.. sourcecode:: cpp

   NS_LOG_INFO ("Enabling MCPTT traces...");
   mcpttHelper.EnableMsgTraces ();
   mcpttHelper.EnableStateMachineTraces ();
   mcpttHelper.EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
   mcpttHelper.EnableAccessTimeTrace ("mcptt-access-time.txt");

These statements are explained in the next section.  Some other aspects
of LTE tracing are omitted in this modified example, in order to focus on
the MCPTT configuration.

mcptt-on-network-two-calls
~~~~~~~~~~~~~~~~~~~~~~~~~~

The program ``mcptt-on-network-two-calls.cc`` is an adaptation of the LTE
example ``lena-simple-epc.cc`` to experiment with two MCPTT on-network calls
involving two different pairs of UEs to a single MCPTT server managing 
both calls.  The first group call (between the first pair of UEs) starts
when the McpttPttApps start at time 2 seconds and runs until simulation
time 16 seconds.  The second call starts at time 18 seconds and runs
until time 34 seconds.

This example provides examples of typical statements necessary to
configure on-network calls.  The first necessary configuration is to
add the MCPTT server to the EPC (via the notional IMS) with an ImsHelper.

.. sourcecode:: cpp

   Ptr<ImsHelper> imsHelper = CreateObject<ImsHelper> ();
   imsHelper->ConnectPgw (epcHelper->GetPgwNode ());

MCPTT clients are added in a manner similar to off-network configuration; e.g.:

.. sourcecode:: cpp

   McpttHelper mcpttClientHelper;
   mcpttClientHelper.SetPttApp ("ns3::psc::McpttPttApp");
   ...
   clientAppContainer1.Add (mcpttClientHelper.Install (ueNodePair1));
   clientAppContainer1.Start (start);
   clientAppContainer1.Stop (stop);

However, in addition, an MCPTT server app must be configured:

.. sourcecode:: cpp

   Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverAppContainer.Get (0));
   Ipv4Address serverAddress = Ipv4Address::ConvertFrom (imsHelper->GetImsGmAddress ());
   serverApp->SetLocalAddress (serverAddress);

and in a similar manner, the local IP address of the UEs is configured:

.. sourcecode:: cpp

   for (uint32_t index = 0; index < ueIpIface.GetN (); index++)
     {
       Ptr<McpttPttApp> pttApp = clientAppContainer.Get (index)->GetObject<McpttPttApp> ();
       Ipv4Address clientAddress = ueIpIface.GetAddress (index);
       pttApp->SetLocalAddress (clientAddress);
       NS_LOG_INFO ("client " << index << " ip address = " << clientAddress);
     }

The next few statements show how to use a McpttCallHelper to configure
the on-network components of floor arbitrator, the 'towards participant'
state machine (the component of the floor control at the server that
maintains state for each participant), the participant state machine,
and properties of the call configurations on the server.

.. sourcecode:: cpp

   McpttCallHelper callHelper;
   // Optional statements to tailor the configurable attributes
   callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                          "AckRequired", BooleanValue (false),
                          "AudioCutIn", BooleanValue (false),
                          "DualFloorSupported", BooleanValue (false),
                          "TxSsrc", UintegerValue (100),
                          "QueueingSupported", BooleanValue (true));
   callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                          "ReceiveOnly", BooleanValue (false));
   callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                          "AckRequired", BooleanValue (false),
                          "GenMedia", BooleanValue (true));
   callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                          "AmbientListening", BooleanValue (false),
                          "TemporaryGroup", BooleanValue (false));

The next few statements configure the calls themselves, using the call helper.
The user must specify the call type and group ID, the client (McpttPttAtt)
applications to configure, the server application, and then the start and
stop times of the call.

.. sourcecode:: cpp

   McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
   // Add first call, to start at time 2 and stop at time 10
   // Call will involve two nodes (7 and 8) and the MCPTT server (node 3)
   uint32_t groupId = 1;
   callHelper.AddCall (clientAppContainer1, serverApp, groupId, callType, Seconds (2), Seconds (16));

   // Add second call, on new groupId, to start at time 8 and stop at time 15
   // Call will involve two nodes (9 and 10) and the MCPTT server (node 3)
   groupId = 2;
   callHelper.AddCall (clientAppContainer2, serverApp, groupId, callType, Seconds (18), Seconds (34));

Finally, the MCPTT tracing can be enabled to trace messages, state machine
transitions, and statistics such as mouth-to-ear latency and access time.

.. sourcecode:: cpp

   NS_LOG_INFO ("Enabling MCPTT traces...");
   McpttTraceHelper traceHelper;
   traceHelper.EnableMsgTraces ();
   traceHelper.EnableStateMachineTraces ();
   traceHelper.EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
   traceHelper.EnableAccessTimeTrace ("mcptt-access-time.txt");

mcptt-on-network-two-simultaneous-calls
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The program ``mcptt-on-network-two-simultaneous-calls.cc`` differs from
the previously introduced ``mcptt-on-network-two-calls.cc`` example as
follows.  First, two calls between three UEs are configured to run
concurrently, rather than sequentially, for a duration of nearly 60 seconds.
Second, each call has a different MCPTT server.  Third, one of the MCPTT
users is configured to participate in both calls, and to switch between
the two calls every second.

The main change in configuration is to configure a second IMS network
(essentially, a second MCPTT server) and to configure its network address
to another subnet instead of the default 15.0.0.0:

.. sourcecode:: cpp

   Ptr<ImsHelper> imsHelper2 = CreateObject<ImsHelper> ();
   imsHelper2->SetImsIpv4Network (Ipv4Address ("16.0.0.0"), Ipv4Mask ("255.0.0.0"));
   imsHelper2->ConnectPgw (epcHelper->GetPgwNode ());

The first user is added to both the first and second call groups:

.. sourcecode:: cpp

   ApplicationContainer callContainer1;
   callContainer1.Add (clientAppContainer1.Get (0));
   ...
   ApplicationContainer callContainer2;
   ...
   // Add the first user to the second call also
   callContainer2.Add (clientAppContainer1.Get (0));

and the two calls configured in the call helper have different server apps:

.. sourcecode:: cpp

   McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
   uint32_t groupId = 1;
   callHelper.AddCall (callContainer1, serverApp, groupId, callType, callStartTime, callStopTime);

   groupId = 2;
   callHelper.AddCall (callContainer2, serverApp2, groupId, callType, callStartTime, callStopTime);


Finally, some events must be added to cause the first MCPTT user to switch
calls at specified times.  This is done with the ``McpttPttApp::SelectCall``
method, such as follows:

.. sourcecode:: cpp

   // schedule events to cause the user (McpttPusher) to switch calls at the
   // configured times in the simulation
   Simulator::Schedule (callStartTime, &McpttPttApp::SelectCall, pttApp, 1, true);
   Simulator::Schedule (firstSwitchTime, &McpttPttApp::SelectCall, pttApp, 2, true);
   Simulator::Schedule (secondSwitchTime, &McpttPttApp::SelectCall, pttApp, 1, true);

The program output of this program shows application and floor control
events traced on userId 1, illustrating that as one call is selected
(sent to the foreground), the other call machine still evolves in
the background.

mcptt-operational-modes-static
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The program ``mcptt-operational-modes-static.cc`` demonstrates how
different modes of MCPTT (on-network, off-network, and on-network using
UE-to-network relay) operate in parallel, with some support for plotting
access time and mouth-to-ear latency KPIs from the resulting traces.

The program creates three teams of UEs (default of 4 UEs per team)
with one team in each of the three modes.  The off-network and
on-network using relay teams are positioned within a building, preventing
full on-network operation for those teams.  Each team creates an MCPTT
call and uses the automatic pusher model to drive an MCPTT call for the
duration of the call (default of 1000 seconds duration).  

.. _fig-mcptt-operational-modes-scenario:

.. figure:: figures/mcptt-operational-modes-scenario.*
   :scale: 75 %

   MCPTT operational modes scenario

A number of traces are generated:

  * ``mcptt-operational-modes-static-access-time.dat``:  Floor access time trace
  * ``mcptt-operational-modes-static-m2e-latency.dat``:  Talk-spurt mouth-to-ear latency tracing
  * ``mcptt-operational-modes-static-msg-stats.dat``:  MCPTT message trace
  * ``mcptt-operational-modes-static-pc5-signaling.dat``:  Relay PC5 interface trace
  * ``mcptt-operational-modes-static-positions.dat``:  UE position trace
  * ``mcptt-operational-modes-static-rsrp-measurement.dat``:  LTE RSRP measurement trace
  * ``mcptt-operational-modes-static-state-machine-stats.dat``:  MCPTT state machine trace

At the end of the program execution, some output is printed on the terminal
regarding the number of packets sent and received by each node.

The call duration, whether floor control queueing is enabled or not (disabled
by default), and the number of UEs per team are configurable as command-line
options.  A simulation progress display is also optionally enabled by 
the ``--showProgress`` option.

Finally, some plotting programs (requiring Python Matplotlib) are available
to create CDF plots of the access time and mouth-to-ear latencies of the three
teams.  The programs are named ``mcptt-operational-modes-plot-access-time.py``
and ``mcptt-operational-modes-plot-m2e-latency.py``.  The programs output,
by default, an EPS file showing the CDF of access time and mouth-to-ear
latency, respectively.  They should be run from the same directory
as the traces (or the optional program argument can be used to point
to the applicable trace file).  The following figures show the output produced
using the default configuration of ``mcptt-operational-modes-static.cc``.

.. _fig-mcptt-operational-modes-static-access-cdf:

.. figure:: figures/mcptt-operational-modes-static-access-cdf.*
   :scale: 75 %

   MCPTT operational modes static access time

.. _fig-mcptt-operational-modes-static-m2e-cdf:

.. figure:: figures/mcptt-operational-modes-static-m2e-cdf.*
   :scale: 75 %

   MCPTT operational modes static mouth-to-ear latency

mcptt-operational-modes-mobility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  * ``mcptt-operational-modes-mobility.cc`` is similar in initial configuration
    to the static program above, but demonstrates mobility of three teams
    of four UEs into a building, and the relative performance impact of the
    three different modes of operation (on-network, off-network, and on-network
    with UE-to-network relay).

Traces
######

There are currently four traces that can be activated by using the
``ns3::psc::McpttHelper``, and this can be done by following the example given
below, after all the applications have been created.

.. sourcecode:: cpp

  NS_LOG_INFO ("Enabling MCPTT traces...");
  mcpttHelper.EnableMsgTraces (); // Enable message trace
  mcpttHelper.EnableStateMachineTraces (); // Enable state trace
  mcpttHelper.EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  mcpttHelper.EnableAccessTimeTrace ("mcptt-access-time.txt");

The ``ns3::psc::McpttMsgStats`` class is used for tracing MCPTT messages at
the application layer and produces a file with the default name
"mcptt-msg-stats.txt" with the following file format.

.. sourcecode:: text

  time(s) nodeid callid ssrc selected  rx/tx bytes  message

There may also be an additional field at the end of the row called "message"
that will be included if the ``ns3::psc::McpttMsgStats::IncludeMessageContent``
attribute is set to "true". The "time(s)" column describes the time (in
seconds) at which a message was sent/received. The "nodeid" column
contains the ns3::Node ID value of the sender or receiver, and the 
"callid" column contains the call ID for this message.  The "ssrc" field
prints the RTP synchronization source (SSRC) field if available.  The
"selected" column indicates whether the message was sent or received for
the currently selected call on the client, or "N/A" for server messages. The  
"rx/tx" column indicates if the message was sent or received, i.e., if "rx"
is the value in the column then that means the message was received, while a
value of "tx" indicates that the message was sent. The "bytes" columns
indicates the size (in bytes) of the message that was generated at the
application layer. The "message" column, if present, includes the string
representation of the message that was sent and includes message field names
and values.

The ``ns3::psc::McpttStateMachineStats`` is used for tracing state machine state
transitions and produces a file with the default name,
"mcptt-state-machine-stats.txt", with the following format.

.. sourcecode:: text

  time(s) userid callid selected     typeid     oldstate        newstate

The "time(s)" column describes the time (in seconds) at which the state
transition took place. The "userid" column contains the user ID of the MCPTT
user that the state machine is associated with. The "callid" column contains
the ID of the call the state machine is associated with.  The 
"selected" column indicates whether the message was sent or received for
the currently selected call on the client, or "N/A" for server messages. The  
"typeid" column
contains the string representation of the state machine's ``ns3::TypeId``.
The "oldstate" column contains the string representation of the
``ns3::psc::McpttEntityId`` which gives the name of the state that the state
machine was in BEFORE the transition took place. And finally, the "newstate"
column contains the string representation of the ``ns3::psc::McpttEntityId`` which
gives the name of the state that the state machine was in AFTER the transition
took place.

One of the key performance indicators (KPI) defined for MCPTT is
the mouth-to-ear latency.  More information about this statistic can be
found in NIST technical report NISTIR 8206 [NIST.IR.8206]_.  When the
|ns3| model generates the first RTP packet of a talk spurt, the timestamp
is encoded into the payload of that RTP packet and every subsequent RTP
packet of the talk spurt.  Upon receipt of an ``ns3::psc::McpttMediaMsg``, the 
receiving application will check whether the packet contains a newer
'start-of-talkspurt' timestamp.  If so, this indicates the reception of the
first RTP packet of a new talk spurt from a sender.  The latency of all
such talk spurts, traced from the perspective of each receiving application,
can be traced using the helper method 
``ns3::psc::McpttHelper::EnableMouthToEarLatencyTrace()``. This method can be
called with no arguments so that mouth-to-ear latency samples can be
captured in the simulation via the
``ns3::psc::McpttHelper::MouthToEarLatencyTrace`` traced callback, or a single
argument, the trace filename, can be provided so that the collected samples
are also written to a file. The output file is formatted as follows.

.. sourcecode:: text

    time(s) ssrc nodeid callid latency(s)
  3.727000    3      8      1   0.024010
  3.727000    3      9      1   0.024010

In the example, node ID 8 started to receive a talk spurt at time 3.727 s, 
on call ID 1, with a mouth-to-ear latency of 24 ms (see Figure 1 of 
[NIST.IR.8206]_).

The MCPTT Access time is defined as KPI1 in [TS22179]_, and measures the time
between a request to speak (normally by pushing to talk) and when the user
receives an indication from floor control to start speaking.  In the ns-3
model, this is measured by state transitions of the floor machinei; the
initial request to speak is marked by a change to a Pending Request state in the
MCPTT user's floor control machine, and permission is marked by the transition
to the Has Permission state.  There can be a few possible state transition
outcomes.  The first is that the floor control server immediately grants
permission.  The second is that the request is initially queued but later
granted.  The third is that the request is denied (perhaps because queuing
is disabled or the queue is full).  the fourth is that the request ultimately
fails, either due to the user (pusher) abandoning the request, or the end
of the call, or some other reason.

The latency and outcomes of access requests can be traced using the helper
method ``ns3::psc::McpttHelper::EnableAccessTimeTrace()``. If no arguments
are provided to this method, then samples can be captured in the simulation
via the ``ns3::psc::McpttHelper::AccessTimeTrace`` traced callback, or if a
single argument, the trace filename, is provided then the collected samples
will also be written to a file. The output file is formatted as follows.

.. sourcecode:: text

    time(s) userid callid result latency(s)
   7.246000      1      0     I   0.024115
  11.251000      2      0     Q   1.302355
  22.689000      4      1     I   0.024172
  25.907000      3      1     Q   2.652518
  28.952000      4      1     I   0.024378
  34.017000      3      1     F   0.651460

The above example illustrates that at time 7.246 s, user ID 1 from call ID 0
was able to immediately request the floor in a transaction that took a bit
more than 24 ms.  The timestamp in the first column indicates the time at
which the outcome is decided, not when the floor request was initiated.
The five possible results described above can be filtered based on the
fourth column above, with "I" denoting "immediate", "Q" denoting "queued",
"D" denoting "denied", "F" denoting "failed" (such as due to message loss),
and "A" denoting "abandoned" (when the PTT button is released while the
request is pending).

When measuring access time in ns-3, we recommend to count the "I" and "Q"
outcomes and filter out the "D", "F", and "A" outcomes.  The [TS22179]_ standard
suggests that access time should be less than 300 ms for 99 % of all MCPTT
requests, but suggests that the system should have negligible backhaul
delay and not be overloaded (less than 70 % load per node) when comparing
against this threshold.  In simulations, the access time may rise above
300 ms due to non-negligible backhaul delay, the pusher model (if too many
MCPTT users are contending for the floor), or congestion or transmission
losses in the LTE network.

Testing and Validation
======================

NIST used the 3GPP
standards to create over 50 test cases for off-network MCPTT operation.
More information about those test cases
can be found in NIST technical report NISTIR 8236 [NIST.IR.8236]_.

On-network MCPTT call control has been tested against the protocol
conformance specification 3GPP TS 36.579-2 [TS365792]_.  The current
test suite checks steps 1-11 in Table 6.1.1.1.3.2-1 for the on-network,
on-demand pre-arranged group call with automatic commencement mode.
The underlying SIP module supporting the on-network MCPTT call control
has also been tested against the SIP RFCs.

On-network MCPTT floor control has been tested for several floor control
scenarios, including:

* UE floor release after initiating the call
* UE floor grant after another UE initiates the call and then releases the floor
* UE floor revoked after higher priority UE requests floor
* UE floor denied due to another UE requesting the floor
* UE floor queued and later cancelled by the requesting UE
* UE floor queued and later granted after another UE releases the floor
* Floor control with dual floor control enabled (two UEs holding the floor
  at the same time

.. References below will be placed in separate Bibliography section

.. [TS22179] 3GPP TS 22.179 `"Mission Critical Push To Talk (MCPTT) Mission Critical Push to Talk (MCPTT) over LTE; Stage 1"
   <http://www.3gpp.org/ftp//Specs/archive/22_series/22.179/22179-e30.zip>`_

.. [TS24379] 3GPP TS 24.379 `"Mission Critical Push To Talk (MCPTT) call control; Protocol specification"
   <http://www.3gpp.org/ftp//Specs/archive/24_series/24.379/24379-e40.zip>`_

.. [TS24380] 3GPP TS 24.380 `"Mission Critical Push To Talk (MCPTT) media plane control; Protocol specification"
   <http://www.3gpp.org/ftp//Specs/archive/24_series/24.380/24380-e40.zip>`_

.. [TS365792] 3GPP TS 36.579-2 `"Mission Critical Push To Talk (MCPTT) User Equpment (UE) Protocol conformance specification"
   <https://www.3gpp.org/ftp//Specs/archive/36_series/36.579-2/36579-2-e50.zip>`_

.. [NIST.IR.8206] Frey, J., Pieper, J., and Thompson, T., `"Mission Critical Voice QoE Mouth-to-Ear Latency Measurement Methods" <https://doi.org/10.6028/NIST.IR.8206>`_, February 2018.

.. [NIST.IR.8236] Varin, P., Sun, Y., and Garey, W., `"Test Scenarios for Mission Critical Push-To-Talk (MCPTT) Off-Network Mode Protocols Implementation" <https://doi.org/10.6028/NIST.IR.8236>`_, October 2018.
