# SIP ns-3 Module

# About
This is an [ns-3](https://www.nsnam.org/) extension module for the
[Session Initiation Protocol (SIP)](https://www.ietf.org/rfc/rfc3261.txt),
an Internet-based call control protocol.

This can be used to model protocol effects such as latency and robustness
for other application protocols that may use SIP as a call control protocol.

# Scope and Limitations

SIP is a large protocol, and the ns-3 model only implements a portion,
focusing on the most typical aspects from RFC3261.
The focus is on modeling SIP transactions and dialogs involved
in session establishment and release, including the timing and
persistence of retransmission of lost messages, and as perceived
by end users.  Network activities that may occur somewhat transparently
to a user (outside of increased message processing delays), such as
messaging related to authorization and charging, are not in scope.

Only a few possible requests and responses are handled.  The SIP INVITE
request, BYE request, and CANCEL request are implemented, along with the
'100 Trying' and '200 OK responses'.  Failure responses such as '404 Not Found'
are not implemented.  Unknown requests and responses will cause a
simulation error.

[RFC 4028](https://www.ietf.org/rfc/rfc4028.txt)
describes a mechanism to send keepalive INVITE and/or UPDATE
messages for SIP sessions.  This model does not support RFC4028.

# Installation

Installation is performed in the usual way for ns-3 contributed modules
(clone or download this module into the 'contrib' directory of ns-3.

There are no special dependencies or dependencies on newer C++ compilers.

# Documentation

See the 'doc' directory for more documentation.

