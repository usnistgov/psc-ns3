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

SIP is a large protocol, and the |ns3| model only implements a portion.
The focus is on modeling SIP transactions and dialogs involved
in session establishment and release, including the timing and
persistence of retransmission of lost messages, and as perceived
by end users.  Activities that may occur somewhat transparently
to a user, such as messaging related to authorization and charging,
are not in scope.

SIP is part of the 3GPP IP Multimedia Subsystem (IMS) for cellular, and
this model was developed to support Mission-Critical Push-To-Talk
(MCPTT) operation, so those aspects have been emphasized.  The following
usage of SIP in the IMS are described in TS 24.229 [TS24.229]_.

For the procedures at the UE (Section 5.1):

* **Registration and authentication:**  Not implemented.
* **Subscription and notificationn:**  Not implemented.
* **Call initiation:** Supported
* **Session modification:** Not yet supported, but planned.
* **Call release:** Supported
* **Emergency service:** Not yet supported, but planned

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

The main idea in the design is for a client of this model to delegate
the responsibility to manage a SIP dialog, involving multiple SIP transactions, to agents that will call back to the client to inform it of events such
as "Call progressing" or "Call ending".  Responsibility for setting
up communications channels or resolving URIs to IP addresses is outside
the scope of the SIP model.  SDP is also outside of the scope; any SDP
messages are passed into the SIP model by the client.

The |ns3| SIP model is also not an ``ns3::Application``; it is created
and used by an `ns3::Application`` instance (although it is possible
to reuse it across multiple application instances, in most use cases, there
will be only one such user).  As a result, SIP is not started
or stopped like other applications, nor is it initialized automatically
at |ns3| initialization time (such as other members of an ``ns3::Node``
are automatically initialized.  Instead, the lifecycle of the SIP agents
are bound to and managed by the other |ns3| applications that instantiate
and make use of the SIP agent.  So, for instance, in the case of MCPTT,
each MCPTT user application and MCPTT server application will create its
own instance of a ``SipAgent`` and ``SipProxy``, respectively. 

With this background, consider a client that wishes to use the SIP model
to establish a session.  Consider also that it wishes to use SIP to convey
some configuration in the form of notional SDP messages.  The client would
form an ``ns3::Packet`` and push an SDP header onto it, and then would
hand the packet over to the ``SipAgent``.   The client would ask the
``SipAgent`` to send an INVITE to a specific URI, and provide to the 
agent also the From and To fields for the SIP header, the packet (possibly
with SDP header already appended), the address (IPv4 or IPv6) and port
of the proxy, and a callback that can be used to send the resulting
packet down the stack.  The client would also register its own callback
to receive notifications of events such as call in progress (100 Trying),
call establishment (200 OK), or call termination or failure.  Similar
design supports the teardown of a call (sending a SIP BYE instead of
an INVITE) or session modification (re-INVITE).

The following timers are used:

* **T1:**
* **T2:**
* **T4:**
* **Timer D:**

// Register callbacks to desired call events
var eventHandlers = {
  'progress': function(e) {
    console.log('call is in progress');
  },
  'failed': function(e) {
    console.log('call failed with cause: '+ e.data.cause);
  },
  'ended': function(e) {
    console.log('call ended with cause: '+ e.data.cause);
  },
  'confirmed': function(e) {
    console.log('call confirmed');
  }
};


JsSIP:  https://github.com/versatica/JsSIP

Nist JSIP:  https://github.com/usnistgov/jsip

Scope and Limitations
=====================

References
==========

.. [RFC3261] RFC 3261 "SIP: Session Initiation Protocol"

.. [TS24.229] 3GPP TS 24.229 "IP multimedia call control protocol based on Session Initiation Protocol (SIP) and Session Description Protocol (SDP); Stage 3"

Usage
*****

*To be completed.*

Helpers
=======

There are no helper classes for the SIP models.

Attributes
==========

The following attributes exist for class ``ns3::SipAgent``:

The following attributes exist for class ``ns3::SipAgent``:

*To be completed.*

Output
======

|ns3| logging components are defined for SipAgent, SipHeader,
and SipProxy.

The following trace sources exist for class ``ns3::SipAgent``:

The following trace sources exist for class ``ns3::SipProxy``:

*To be completed.*

Examples
========

*To be completed.*

Validation
**********

*To be completed.*
