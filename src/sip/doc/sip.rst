SIP Module
----------

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

This document describes an ns-3 model for the Session Initiation
Protocol (SIP) [RFC3261]_.
The source code for the new module lives in the directory ``src/sip``.

Model Description
*****************

SIP is a large protocol, and the |ns3| model only implements a portion,
focusing on the most typical aspects from [RFC3261]_.
The focus is on modeling SIP transactions and dialogs involved
in session establishment and release, including the timing and
persistence of retransmission of lost messages, and as perceived
by end users.  Network activities that may occur somewhat transparently
to a user (outside of increased message processing delays), such as
messaging related to authorization and charging, are not in scope.

SIP is part of the 3GPP IP Multimedia Subsystem (IMS) for cellular, and
this model was developed to support Mission-Critical Push-To-Talk
(MCPTT) operation, so those aspects have been emphasized.  The following
usage of SIP in the IMS are described in TS 24.229 [TS24.229]_.

For the procedures at the UE (Section 5.1):

* **Registration and authentication:**  Not implemented.
* **Subscription and notification:**  Not implemented.
* **Call initiation:** Supported
* **Session modification:** Not yet supported, but possible for future.
* **Call release:** Supported
* **Emergency service:** Not yet supported, but possible for future.

The |ns3| model doesn't distinguish between different variants of
the CSCF (I-CSCF, P-CSCF, S-CSCF); they are all abstracted as
a server 'proxy' function.  The procedures supported in network
(at the proxy) match those of the UE listed above.

Design
======

SIP is a text-based protocol, and fields like addresses and URIs are usually
defined as strings.  For simplicity, we use four-byte unsigned integers
to represent URIs and addresses, although the model could be extended or
changed to use strings in the future.

The main idea in the |ns3| SIP design is for a client (user) of this model 
to delegate
the responsibility to manage a SIP dialog, involving multiple SIP transactions, to SIP agents that will call back to the client to inform it of events such
as "Call progressing" or "Call ending".  Responsibility for setting
up communications channels or resolving URIs to IP addresses is outside
the scope of the SIP model.  SDP is also outside of the scope; any SDP
messages are passed into the SIP model by the client, and the SIP model
does not interpret any message payloads.

The |ns3| SIP model is also not an ``ns3::Application``; it is created and
used by an `ns3::Application`` instance as a service (although it is possible
to reuse it across multiple application instances, in most use cases, there
will be only one such user).  As a result, SIP is not started
or stopped like other applications, nor is it initialized automatically
at |ns3| initialization time (such as other members of an ``ns3::Node``
are automatically initialized).  Instead, the lifecycle of the SIP agents
are bound to and managed by the other |ns3| applications that instantiate
and make use of the SIP agent.  So, for instance, in the case of MCPTT,
each MCPTT user application and MCPTT server application will create its
own instance of a ``SipAgent`` and ``SipProxy``, respectively, and will
release its pointer to the SIP element upon the application deletion
(usually at the end of simulation). 

The base class for ``SipAgent`` and ``SipProxy`` is the ``SipElement``
class.  Most of the logic is implemented in this base class because
there is not much distinction in the |ns3| model between the agent
and proxy roles, in terms of how transactions are managed.

With this background, consider a client that wishes to use the SIP model
to establish a session.  Consider also that it wishes to use SIP to convey
some configuration in the form of notional SDP messages.  The client would
form an ``ns3::Packet`` and push an SDP header onto it, and then would
hand the packet over to the ``SipAgent``.   The client would ask the
``SipAgent`` to send an INVITE request with this packet to a specific URI,
and provide to the 
agent the following:  the From and To fields for the SIP header,
the packet (possibly with SDP header already appended), 
the address (IPv4 or IPv6) and port
of the proxy, and a callback that can be used by SIP to send the resulting
packet down the network stack.  The client would also register its own callback
to receive notifications of events such as call in progress (100 Trying),
call establishment (200 OK), or call termination or failure.  A similar
process supports the teardown of a call (sending a SIP BYE instead of
an INVITE) or session modification (re-INVITE).

The implementation contains classes to hold state for SIP transactions
and dialogs, including timers to protect against loss or lack of response.  
The following timers are used:

* **Timer A:** INVITE request retransmit interval (UDP only); doubles upon retransmission.
* **Timer B:** INVITE transaction timeout timer; report failure upon expiry.
* **Timer C:** proxy INVITE transaction timeout timer; doubles upon retransmission.  If provisional 100 was sent, then the server needs to ensure that final response gets sent, or generate CANCEL request if not.
* **Timer E:** non-INVITE request retransmit interval (UDP only); doubles upon retransmission.  Similar to Timer A.
* **Timer F:** non-INVITE transaction timeout interval (UDP only).  Similar to Timer B.
* **Timer I:** Wait time for ACK retransmits (server absorb ACKs for a time before moving to Terminated).  This is part of INVITE server transaction.
* **Timer J:** Wait time for non-INVITE request retransmits (server Completed to Terminated transition, to respond to duplicate requests).
* **Timer K:** Wait time for response retransmits (client absorbs any retransmitted responses for a time before moving from Completed to Terminated).  This is part of non-INVITE client transaction.

The following timers from [RFC3261]_ are not supported:

* **Timer D:** time that client will wait in Completed state, after a 300-699
  ACK response has been sent.  Since the model does not send these responses,
  Timer D (and transition from Completed to Terminated) is not needed.
* **Timer G:** INVITE response retransmit interval; doubles upon retransmission
   Timer G pertains to non-200 response from server
* **Timer H:** Wait time for ACK receipt (server waits for ACK to its response)
   Timer H pertains to non-200 response from server

Scope and Limitations
=====================

Only a few possible requests and responses are handled.  The SIP INVITE
request, BYE request, and CANCEL request are implemented, along with the
'100 Trying' and '200 OK responses'.  Failure responses such as '404 Not Found'
are not implemented.  Unknown requests and responses will cause a
simulation error.

[RFC4028]_ describes a mechanism to send keepalive INVITE and/or UPDATE
messages for SIP sessions.  This model does not support [RFC4028]_.

References
==========

If references are not rendered here, they can be found in the bibliography
section at the end of the document.

.. [RFC3261] RFC 3261 "SIP: Session Initiation Protocol"

.. [RFC4028] RFC 4028 "Session Timers in the Session Initiation Protocol (SIP)"

.. [TS24.229] 3GPP TS 24.229 "IP multimedia call control protocol based on Session Initiation Protocol (SIP) and Session Description Protocol (SDP); Stage 3"

Usage
*****

Helpers
=======

There are no helper classes for the SIP models.

Attributes
==========

The following attributes exist for class ``ns3::SipElement``:

* **ReliableTransport** Whether the transport is reliable (TCP, SCTP) or unreliable (UDP)
* **T1** RTT estimate timer
* **T2** Maximum retransmit interval for non-INVITE requests and INVITE response
* **T4** Maximum duration a message will remain in the network

The following attributes exist for class ``ns3::SipProxy``:

* **ProxyInviteTransactionTimeout** Timer C timeout value

No attributes exist for class ``ns3::SipAgent``.

Output
======

|ns3| logging components are defined for SipElement, SipAgent, SipHeader,
and SipProxy.

The following trace sources exist for class ``ns3::SipElement``:

* **TxTrace** The trace for capturing transmitted messages
* **RxTrace** The trace for capturing received messages
* **DialogState** Trace of Dialog state changes
* **TransactionState** Trace of Transaction state changes",

No trace sources exist for classes ``ns3::SipProxy`` and ``ns3::SipAgent``:

Examples
========

No standalone examples are presently written, although one could be
created from the test suite, initial test case (SIP dialog).

Validation
**********

This model has been validated by comparing messages with published MCPTT
traces from Nemergent and with traces from experiments conducted with
Nemergent equipment by NIST Boulder.  The tests also serve as regression
tests.

The main objective of the tests has been to first confirm the normal
operation of the protocol in a MCPTT group call configuration, without
any message loss, and then to force various message losses to confirm
that different SIP timers are operating as expected to protect against
message loss and to notify users about transaction failures.

The test configuration creates three SIP users and one SIP proxy in a
group call context; one user initiates a call to the other two users,
and later (in some cases) terminates the call.
The call causes multiple SIP dialogs and transactions to be established
between the entities.  In a simulation, the protocol that is making use
of SIP for call control (such as an MCPTT application) would register
callbacks to learn of event notifications, as well as to receive SIP
messages.  The test suite registers methods to call as callbacks and
to receive messages and checks that the expected events occur at the
expected times.

The following tests are written:

1. **SipDialogTest:** Test a full dialog with no message losses (normal operation).  This also tests operation with UDP of Timer I (INVITE), J (BYE), K(BYE client).

2. **SipInitiatorInviteLossTest:** Test the recovery from the loss of initial INVITE from Client 1 (towards the Proxy).  This tests the operation of Timer A.

3. **SipInitiatorInviteFailureTest:** Test the outcome from the failure of initial INVITE (all retransmissions) from Client 1.  This tests the operation of Timer B.

4. **SipInitiatorByeLossTest:** Test the recovery from the loss of initial BYE from Client 1 (towards the Proxy).  This tests the operation of Timer E.

5. **SipInitiatorByeFailureTest:** Test the outcome from the failure of BYE (all retransmissions) from Client 1.  This tests the operation of Timer F.

6. **SipProxyInviteLossTest:** Test the outcome from the loss of first INVITEs from proxy to Clients 2 and 3, testing the proxy Timer A handling.

7. **SipProxyInviteFailureTest:** Test the outcome from the failure of INVITEs from proxy to Clients 2 and 3.  This generates a 408 Request Timeout back to the Client 1.
