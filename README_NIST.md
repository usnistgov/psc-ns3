# Project Overview

This project has been extended by the National Institute of Standards and
Technology (NIST) Communications Technology Lab (CTL) Wireless
Networks Division (WND).

Certain equipment, instruments, software, or materials, commercial or
non-commercial, are used in this project. Such usage is not intended to imply
recommendation or endorsement of any product or service by NIST, nor is it
intended to imply that the software, materials, or equipment identified are
necessarily the best available for the purpose.

This project is considered feature complete, and will be maintained on a
'best effort' basis.

## Contact

To report a bug, please open a [GitHub Issue](https://github.com/usnistgov/pscr-net-sim/issues/new).
The points of contact for this project are:
  * Aziza Ben Mosbah ([aziza.benmosbah@nist.gov](mailto:aziza.benmosbah@nist.gov))
  * Samantha Gamboa Quintiliani ([samantha.gamboaquintiliani@nist.gov](mailto:samantha.gamboaquintiliani@nist.gov))

# Model Overview

This model is an extension of the 5G New Radio (NR)
Vehicle-to-Everything (V2X) ns-3 model described
[here](https://doi.org/10.1109/VNC52810.2021.9644618). The original model
provides a Sidelink (SL) implementation based on 3GPP Release 16. The SL is
comprised of a set of physical channels and a protocol stack that allow
User Equipment (UE) of a 5G NR system to communicate directly between them
without routing the data through the network infrastructure. The first
services defined to use the NR SL were the NR V2X services in Release 16,
particularly the V2X communication over PC5 functionality.

Our contribution is an extension to this model that makes use of the SL
implemenation to provide NR Proximity Services (ProSe). 5G NR ProSe is the
term used by 3GPP to define a set of services that use direct transmissions
between UEs in close proximity on a 5G NR system.

At the time of development, three main ProSe functionalities were defined to
enable these services in 3GPP Release 17:
  * 5G NR ProSe Direct Discovery, including direct, group member, and relay
    UE discovery
  * 5G NR ProSe Direct Communication, including unicast, broadcast, and
    groupcast communication
  * 5G NR ProSe UE-to-Network (U2N) relay, including Layer 2 and Layer 3
    architectures

Our extension expands over two ns-3 modules:
* lte: includes the extensions made on layers above the MAC layer, including
  the new ProSe layer and the different entities to support its operation.
* nr: includes extensions made on the MAC layer and below, a new helper to
  support ProSe configuration, and a set of  example scenarios of the different
  implemented functionalities.

