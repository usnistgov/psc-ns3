.. include:: replace.txt
.. highlight:: cpp

+++++++++++++++++++++++++++++++++++++
Public Safety Communications Overview
+++++++++++++++++++++++++++++++++++++

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
4) ``antenna``: Parabolic antenna model as described in 3GPP document TR 36.814.

Documentation for the PSC features implemented in the |ns3| ``lte``, ``buildings``, and ``antenna`` modules is provided in the respective 
module documentation.  This chapter documents the |ns3| ``psc`` module.

At present, a large portion of the code related to public safety communications is
found in the ProSe implementation in the ``lte`` module.  This is because
the ProSe services of sidelink communications, discovery, and synchronization
are deeply connected to the LTE models and difficult to factor into a
separate module.   Features intrisic to the low-level operation of ProSe in 
LTE are found in the ``lte`` module.

The ``psc`` module also includes:

1) an extensive model of mission-critical push-to-talk (MCPTT) for use in off-network scenarios.
2) a UDP-based application to generate many-to-many traffic models in a scenario.
3) an energy consumption model for flying a (single/multi)rotor unmanned aerial vehicle (UAV).
4) a new HTTP application.  

There is no support for legacy public safety communications such as land
mobile radio system (LMRS).  While IPv4 has been the main focus of development,
some of the LTE sidelink example programs now feature an option to use IPv6.

Future extensions to this module or other related modules such as LTE are planned for the following features:

1) support for on-network modes of MCPTT, and possibly other public safety applications.
2) UE-to-network relay.
3) public safety scenario support code and example programs.

---------------
Acknowledgments
---------------

Public safety communications features are based on development led by the
Wireless Networks Division of the U.S. National Institute of Standards
and Technology, described in publications ([NIST2016]_ and  [NIST2017]_).   Users
of the D2D features of |ns3| are requested to cite [NIST2017]_
in academic publications based on these models. 

The following individuals are authors of the public safety communications
extensions:

* Aziza Ben-Mosbah (aziza.ben.mosbah@gmail.com)
* Evan Black (evan.black@nist.gov)
* Fernando J. Cintron (fernando.cintron@nist.gov)
* Samantha Gamboa (samantha.gamboa@nist.gov)
* Wesley Garey (wesley.garey@nist.gov)
* Antonio Izquierdo Manzanares (antonio.izquierdomanzanares@nist.gov)
* Richard Rouil (richard.rouil@nist.gov)

This release was ported from an earlier release, based on ns-3.22, to the
ns-3.30 release, and was integrated with the LTE and buildings module, and
extended to include additional example programs and tests, by CTTC and the 
University of Washington. This work was performed under the financial 
assistance award 70NANB17H170 from U.S. Department of Commerce, National 
Institute of Standards and Technology.
