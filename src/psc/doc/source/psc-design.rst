.. include:: replace.txt
.. highlight:: cpp

+++++++++++++++++++++++++++++++++++++
Public Safety Communications Overview
+++++++++++++++++++++++++++++++++++++

|ns3| support for public safety communications (PSC) is based on 
relatively new capabilities for 4G LTE systems introduced in 3GPP Release 12
and later releases. This includes Device-to-Device (D2D) communications over sidelink
and mission-critical push-to-talk (MCPTT) application.

Support for public safety communications is distributed among the following
five |ns3| modules:

1) ``psc``:  (this module)  Support for models and scenarios that are specific to public safety communications.
2) ``lte``:  Support for ProSe (sidelink communications).
3) ``buildings``:  Support for pathloss models including building effects, as defined by 3GPP with relevance to public safety scenarios.
4) ``antenna``: Parabolic antenna model as described in 3GPP document TR 36.814.
5) ``sip``: Session Initiation Protocol (SIP) model.

Documentation for the PSC features implemented in the |ns3| ``lte``, ``buildings``, ``antenna``, and ``sip`` modules is provided in the respective 
module documentation.  This chapter documents the |ns3| ``psc`` module.

At present, a large portion of the code related to public safety communications is
found in the ProSe and UE-to-Network relay implementation in the ``lte`` module.  This is because
the ProSe services of sidelink communications, discovery, and synchronization
are deeply connected to the LTE models and difficult to factor into a
separate module. Features intrinsic to the low-level operation of ProSe in 
LTE are found in the ``lte`` module.

The ``psc`` module also includes:

1) an extensive model of mission-critical push-to-talk (MCPTT) for use in off-network or on-network scenarios.
2) a UDP-based application to generate many-to-many traffic models in a scenario.
3) an energy consumption model for flying a (single/multi)rotor unmanned aerial vehicle (UAV).
4) a new HTTP application.  
5) a video streaming model.
6) a configurable generic client/server application
7) a framework to develop incident scenarios and a detailed large scale example

There is no support for legacy public safety communications such as
land mobile radio system (LMRS).  While LTE sidelink supports both
IPv4 and IPv6, the UE-to-Network Relay feature only supports IPv6.

---------------
Acknowledgments
---------------

Public safety communications features are based on development led by the
Wireless Networks Division of the U.S. National Institute of Standards
and Technology (NIST), described in publications ([NIST2016]_, [NIST2017]_, [NIST2019]_, and [NIST2021]_).
Users of the D2D features of |ns3| are requested to cite [NIST2017]_
in academic publications based on these models. Users of the MCPTT
features of |ns3| are requested to cite [NIST2021]_
in academic publications based on these models. 

The integration of the initial ProSe module with the LTE and buildings module,
as well as the creation of additional examples and tests, and contributions
to on-network MCPTT support, was assisted  by CTTC and the 
University of Washington. This work was performed under the financial 
assistance award 70NANB17H170 and 70NANB20H179 from U.S. Department of
Commerce, National Institute of Standards and Technology.

The following individuals are authors of the public safety communications
extensions:

* Zoraze Ali (CTTC)
* Aziza Ben-Mosbah (NIST)
* Evan Black (NIST)
* Fernando J. Cintron (NIST)
* Samantha Gamboa (NIST)
* Wesley Garey (NIST)
* Tom Henderson (Univ. of Washington)
* Antonio Izquierdo Manzanares (NIST)
* Manuel Requena (CTTC)
* Richard Rouil (NIST)
* Raghav Thanigaivel (NIST)

