Public Safety Communications Overview
*************************************

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

|ns3| support for public safety communications (PSC) is based on 
relatively new capabilities for 4G LTE systems introduced in 3GPP Release 12
and later releases.  Device-to-Device (D2D) Proximity Services (ProSe)
communications are terms relating to features allowing for UEs to discover
each other, synchronize, and communicate with each other, with or without
the use of an eNodeB.  These services make use of a so-called `sidelink`
channel between UEs.

Support for public safety communications is distributed among the following
four |ns3| modules:

1) ``psc``:  (this module)  Support for models and scenarios that are specific to public safety communications.
2) ``lte``:  Support for ProSe (sidelink communications).
3) ``buildings``:  Support for pathloss models including building effects, as defined by 3GPP with relevance to public safety scenarios.
4) ``antenna``: Parabolic antenna model as described in 3GPP document TR 36.814

All of the above mentioned features are based on development led by the
Wireless Networks Division of the U.S. National Institute of Standards
and Technology, described in publications ([Rou16]_ and  [Rou17]_).  

Documentation for the PSC features implemented in the |ns3| ``lte``, ``buildings``, and ``antenna`` modules is provided in the respective 
module documentation.  This chapter documents the |ns3| ``psc`` module.

At present, the majority of the code related to public safety communications is
found in the ProSe implementation in the ``lte`` module.  This is because
the ProSe services of sidelink communications, discovery, and synchronization
are deeply connected to the LTE models and difficult to factor into a
separate module.   Features intrisic to the low-level operation of ProSe in 
LTE are found in the ``lte`` module. 

There is no support for legacy public safety communications such as land
mobile radio system (LMRS), and only IPv4 (and not IPv6) is presently
supported.

Future extensions to this module or other related modules such as LTE are planned for the following features:

1) mission-critical push-to-talk (MCPTT) and possibly other public safety applications
2) UE-to-network relay
3) public safety scenario support code and example programs


