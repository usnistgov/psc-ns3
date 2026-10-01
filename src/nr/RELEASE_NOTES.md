<!--
Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)

SPDX-License-Identifier: GPL-2.0-only
-->

5G-LENA Release Notes                         {#releasenotes}
=====================

This file contains release notes for the NR module (most recent releases first).

All of the ns-3 documentation is accessible from the ns-3 website:
http://www.nsnam.org including tutorials: https://www.nsnam.org/documentation/

Consult the file CHANGES.md for more detailed information about changed
API and behavior across releases.

Release NR-v3.4
---------------

Availability
------------

Cite this version
-----------------

Supported platforms
-------------------
This release has been tested on the following platforms:
- Arch Linux with g++-13 and clang-16.
- Ubuntu 20.04 with g++10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.
- Ubuntu 23.04 with g++13.
- Ubuntu 23.10 (Mantic Minotaur) with clang-16.

This release will be compatible with ns-3.4x.

Important news
--------------
- We introduced a new attachment algorithm `AttachToMaxRsrpGnb` which follows 3GPP configuration to attach UE to the gNB with highest RSRP within indicates handoff margin.
- We introduced a new beamforming algorithms, `KroneckerBeamforming` and `KroneckerQuasiOmniBeamforming` which provides Kronecker based beamforming whereas in the `KroneckerBeamforming` both gNB and UE use Kronecker, and in the `KroneckerQuasiOmniBeamforming` gNB use Kronecker vector and UE used Omni direction to do Beamforming.
- We introduced a new helper, `NrChannelHelper`, which provides a simple interface for configuring spectrum channels with various channel models, including NYUSIM, Fluctuating Two-Ray (FTR), and 3GPP.

- A new example, `gsoc-nr-channel-models.cc`, demonstrates how to configure different spectrum channels for end-to-end simulations. These channels can be 'legacy' (e.g., Friis) or not (e.g., NYUSIM, 3GPP, FTR).

- A new test file, `nr-channel-setup-test.cc`, tests if the NrChannelHelper API can correctly create a specified
channel, which will be created using well-defined combinations of scenarios, channel models, and channel conditions.

- The ns-3 implementation of NYUSIM channel model is added as a feature of the NR module. The original files are installed in utils directory,
with some version updates. Acknowledgments to Hitesh Poddar (NYU WIRELESS) and Tomoki Yoshimura (Sharp Laboratories of America),
authors of the NYUSIM channel model code implementation in ns-3. (Original code: https://github.com/hiteshPoddar/NYUSIM_in_ns3/tree/main)

- A method has been implemented to ensure that non-SU-MIMO channel models can also be utilized. This is achieved by converting the receiver's PSD (Power Spectral Density) into a spectrum channel matrix used in the module's features.

- Remember to follow the instructions from the README.md file, i.e., to checkout
  the correct release branch of both ns-3 and the NR module. The information about
  compatibility with the corresponding ns-3 release branch is stated in the
  `README.md` file.

- Previously, CSI feedback was based only on PDSCH. Now, the module is extended to support period CSI-RS and CSI-IM based CSI-feedback.
Additional combinations of these modes are available for testing purposes and can be configured through `NrHelper::CsiFeedbackFlags` attribute.
Example `cttc-nr-mimo-demo.cc` is extended to show how to configure different types of the feedback.

- It is no longer necessary to call NrHelper::UpdateDeviceConfigs(); please remove from any examples or tests.

New user-visible features
-------------------------
In the new version of the module, we have created a simple interface for configuring spectrum channels for use in simulations. Previously, channel configuration was done along with band creation, where the user selected the scenario for use in the 3GPP model as an argument. In addition to extending the module to support NYUSIM and FTR channels, we have also created a helper class to assist with channel configuration `NrChannelHelper`. Users can now use this helper class to configure channels.

Bugs fixed
----------


Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues


Release NR-v3.3
----------------

Availability
------------
Available since October 15, 2024

Cite this version
-----------------
DOI: 10.5281/zenodo.13929095

Supported platforms
-------------------
This release has been tested on the following platforms:
- Arch Linux with g++-14 and clang-18 (with GNU stdlibc++ and LLVM libc++).
- Ubuntu 20.04 with g++10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.
- Ubuntu 23.04 with g++13.
- Ubuntu 23.10 (Mantic Minotaur) with clang-16.

This release is compatible with ns-3.42.

Important news
--------------
- This release includes Fronthaul Control mechanisms, that allow
  to simulate a limited-capacity fronthaul (FH) link based on the
  Fronthaul Capacity (m_fhCapacity) set by the user in the example
  script, and to apply FH Control methods (m_fhControlMethod) in order
  to restrict user allocations, if these they do not fit in the
  available FH capacity. O-RAN 7.2x functional split is assumed.
  The methods supported are the Dropping, Postponing, Optimize MCS
  and Optimize RBs, however it can be easily extended to apply
  additional methods. The Fronthaul Control takes decisions based
  on the model applied, the available fronthaul capacity, the applied
  modulation compression and the number of active UEs in each cell
  (either with new or HARQ data). This last information is updated
  after each scheduling process is finalized, through the MAC layer.
  The interaction of the Fronthaul Control with the MAC and high-PHY
  layers takes place through a set of SAP interfaces that allow the
  bidirectional exchange of information.

- Remember to follow the instructions from the README.md file, i.e., to checkout
  the correct release branch of both ns-3 and the NR module. The information about
  compatibility with the corresponding ns-3 release branch is stated in the
  `README.md` file.

New user-visible features
-------------------------
- Fronthaul Control mechanisms are available in the NR module.
  The ``cttc-nr-fh-xr`` example shows how to use and configure the
  Fronthaul Control in a scenario employing XR, CG and VoIP traffic.
  For a more detailed description see the NR manual and Doxygen.

Bugs fixed
----------
- (2b84532c) Set the correct UL output file name for the ``NrMacSchedulingStats``.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues


Release NR-v3.2
---------------

Availability
------------
Available since September 25, 2024

Cite this version
-----------------
DOI: 10.5281/zenodo.13837253

Supported platforms
-------------------
This release has been tested on the following platforms:

- Arch Linux with g++-14 and clang-18 (with GNU stdlibc++ and LLVM libc++).
- Ubuntu 20.04 with g++10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.
- Ubuntu 23.04 with g++13.
- Ubuntu 23.10 (Mantic Minotaur) with clang-16.

This release will be compatible with ns-3.42.

Important news
--------------

- NR is now independent from the upstream LTE module from ns-3.
  To simplify the porting process, we migrated most source files from LTE, including RLC, RRC, PDCP, EPC.
  However, keep in mind we are not adding supported features at this time.
  We hope to enable more features in future releases.

- We introduced three new CI jobs with additional tests.
  The first CI job checks for now deprecated emacs lines.
  The second job checks if the nr works normally without eigen (which is required by MIMO).
  The third job tests nr with LLVM's libc++. This helps to detect defects
  caused by relying on implementation specific behavior of different C++ standard libraries.

- The ``NrHelper::EnableHarq`` attribute was removed for being misleading and causing bugs.
  If you want to avoid retransmissions, use ``NrMacSchedulerNs3::EnableHarqReTx`` instead.

- Remember to follow the instructions from the README.md file, i.e., to checkout
  the correct release branch of both ns-3 and the NR module. The information about
  compatibility with the corresponding ns-3 release branch is stated in the
  `README.md` file.

New user-visible features
-------------------------

Bugs fixed
----------

- (5b5f2d27) Fix Codebook type configuration using the NrHelper::SetupMimoPmi()
- (e1272b43) Print missing CQI field and remove extra tabulation
- (3617c735) Do not account for CRC when estimating potential throughput
- (3f93030d) Remove duplicate multiplication of RBGs and symbols per beam
- (c2ea8533) Remove misleading HarqEnabled flag from NrHelper and add test case
- (3618a80a) Sort control message lists before merging
- (a68f143c) Update type of rbNum to prohibit overflow and wrong SINR trace
- (738b08bc) Prevent out-of-bounds access when rntiIt == m_rlcAttached.end()
- (ed645749) Prevent negative values of effective SINR
- (63c863ce) examples: Disable cttc-nr-mimo-demo when eigen is not found

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues


Release NR-v3.1
----------------

Availability
------------
Available since July 19, 2024

Cite this version
-----------------
DOI: 10.5281/zenodo.12773723

Supported platforms
-------------------
This release has been tested on the following platforms:
- Arch Linux with g++-13 and clang-16.
- Ubuntu 20.04 with g++10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.
- Ubuntu 23.04 with g++13.
- Ubuntu 23.10 (Mantic Minotaur) with clang-16.

This release is compatible with ns-3.42.

Important news
--------------
- This release comes with a completely new PMI Type-I Single Panel codebook for MIMO.

- Performance optimizations were included for faster simulations.

- The code base is updated for C++ conformance and aligned with ns-3.42.

- Many bugs were fixed. One of them is significant, however it is limited to NR-U users.
  The interference events were counted incorrectly after a refactoring merged in nr-3.0.
  This caused the NrSpectrumPhy to change state prematurely due to sensing the channel
  as empty when there was a transmission with received power bigger than the threshold.

- We introduced two new CI jobs. The first to improve uniformization of commit messages.
  The second catches memory related bugs, allowing for us or contributors to fix them
  before finding them on the wild, crashing simulations or generating bogus results.

- Remember to follow the instructions from the README.md file, i.e., to checkout
  the correct release branch of both ns-3 and the NR module. The information about
  compatibility with the corresponding ns-3 release branch is stated in the
  `README.md` file.

New user-visible features
-------------------------
- A new MIMO Type-I Single Panel codebook is available in the NR module.
  The ``NrCbTypeOneSp`` codebook allows the MIMO model introduced in
  nr-3.0 to support up to 32 antenna ports and up to rank 4. See the
  ``cttc-nr-mimo-demo.cc`` file for an example on how to configure
  the new codebook. More details are available at the nr-manual and Doxygen.

- Sub-band downsampling/upsampling techniques were implemented, allowing
  for much faster simulations using large antenna panels with many ports.

Bugs fixed
----------
- (edd72f85) Fixed the number of rows when creating a dummy precoding matrix
- (2c98da82) Abort when size of packet isn't supported by UDP
- (d827c1df) Detect overflow in active tx count
- (a9c6af8b) Assigned random streams to fix traffic-generator-test
- (1092d90d) Reverted changes to NrInterference accounting of events
- (88973614) Fixed serialized size of NrRadioBearerTag
- (884b811e) Extend 16-bit numbers to 32-bit, to prevent overflows in Cantor computation
- (9b59d43c) Prevent memory leak by freeing up SpectrumValue globals at the end of the simulation
- (1df0862b) Prevent memory leak by explicitly disposing Phys not aggregated to a Node
- (3cddaa57) Removed the deprecated attribute 'UseFixedRi' leftovers
- (9da24622) Removed the deprecated attribute 'NrSpectrumPhyList' leftovers
- (6b182dca) Fix config paths used by DlCtrlPathloss/DlDataPathloss
- (1e47feb1) Fix static and monolib build of nr
- (0fda3f52) Use sqlite prepare statement to prevent leak
- (0989a1eb) Fix NrSpectrumPhy disposal to include m_rxSpectrumModel and m_txPsd
- (69a76a94) Fix RealisticBeamformingHelper disposal

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues

Release NR-v3.0
----------------

Availability
------------
Available since February 16, 2024

Cite this version
-----------------
DOI: 10.5281/zenodo.10670856

Supported platforms
-------------------
This release has been tested on the following platforms:
- Arch Linux with g++-13 and clang-16.
- Ubuntu 20.04 with g++-9 and 10, and clang-10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.
- Ubuntu 23.04 with g++13.
- Ubuntu 23.10 (Mantic Minotaur) with clang-16.

This release is compatible with ns-3.41.

Important news
--------------
- This release comes with a completely new MIMO model implementation.
Comparing to the previous dual-polarized implementation, this new model offers a
much more general approach and can be easily extended to support different types of MIMO,
which was not the case with the previous implementation which was limited to a
particular case of MIMO, i.e., DualPolarized-MIMO (DP-MIMO), supporting
only 2 streams with no possibility for extension. Old DP-MIMO implementation
used some assumptions related to the mapping of the streams to ports that were
limiting its application, i.e., each stream was mapped to the antenna elements
with a specific polarization. This means that there was a hard limitation on the
assignment of streams to a specific port and to the specific antenna elements of
one polarization. Also, it used a not realistic inter-stream interference rejection
model. Instead, the new MIMO implementation will support general MIMO, through
the inclusion of 3GPP-compliant PMI (precoding matrix indicator) and RI (rank indicator),
assuming MMSE-IRC (interference rejection combining) receiver for inter-stream
interference suppression, as adopted in 3GPP. Such general MIMO includes as particular
case the old DP-MIMO, but with a more generic and realistic model for DP-MIMO in
which the streams/port/antennas mapping is not predefined but defined through
digital precoding and in which a more accurate model for the inter-stream
interference calculation is considered. The new MIMO is flexible and can be easily
extended for more streams/ranks/ports.

- The current MIMO implementation requires Eigen3 library (https://eigen.tuxfamily.org/).

- When creating an MR, 5G-LENA users will be able to use CI/CD minutes belonging
to the NR module project.

- The code base is updated and a bit modernized, e.g. to use standard integer types,
simpler bool checks, spelling errors are fixed, automatic clang-tidy fixes are
applied, for loops are modernized, python formatter settings were added, etc.

- Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both ns-3 and the NR module. The information about
compatibility with the corresponding ns-3 release branch is stated in the
`README.md` file.


New user-visible features
-------------------------
- A new MIMO model is available in the NR module. The ``cttc-nr-mimo-demo`` example
shows how to use and configure the new MIMO model. See the NR manual and Doxygen
for the full description of the new MIMO model and APIs. The new APIs are
also listed CHANGES.md.

- The NR module pipelines have been improved in multiple aspects:
  - GCC 13 CI jobs are added.
  - Ubuntu rolling CI jobs are added to weekly tests.
  - Cmake format CI job is added.
  - Black and isort formatter settings for python were added and applied.
  - Added python format CI job.
  - Added spell-checking CI job.
  - Clang-tidy CI job is added.
  - The documentation build and deployment are split now, and the deployment is limited to master.
  - Merge request pipelines are enabled.

Bugs fixed
----------
None.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues


Release NR-v2.6
----------------

Availability
------------
Available since November 30, 2023

Cite this version
-----------------
DOI: 10.5281/zenodo.10246105

Supported platforms
-------------------
This release has been tested on the following platforms:
- Arch Linux with g++-13 and clang-16.
- Ubuntu 20.04 with g++-9 and 10, and clang-10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.

Recommended ns-3 release: ns-3.40

Important news
--------------
This release is compatible with ns-3.40.

Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both, ns-3 and the NR module. The information
about compatibility with the corresponding ns-3 release branch is stated in the
`README.md` file.

New user-visible features
-------------------------
- This NR release comes with the first NR module tutorial created by Giovanni
Grieco in the scope of GSoC 2023 project `IUNS-3 5G NR: Improving the Usability of ns-3's 5G NR Module`
mentored by Tom Henderson, Katerina Koutlia, and Biljana Bojovic. This tutorial
focuses on a `cttc-nr-demo` example and explains the internal functionality of the
NR RAN by providing a detailed, layer-by-layer insights on the packet lifecycle
as they traverse the RAN. The tutorial highlights all the points in the NR protocol
stack where packets may be delayed or dropped, and how to log and trace such events.

- Overall enhancement of the logging in the NR module. For many log messages
the log level is refactored to provide easier overall logging, which allows to
focus on the main events in the RAN protocol stack. In many places the logging
information is expanded and improved its comprehension.

- Added `Tx` and `Rx` trace sources in NrNetDevice to allow easier tracing of
the events when the packet is transmitted or received.

- The references in the examples are updated to point to the latest 3GPP documents.

Bugs fixed
----------
- #157 Changed default value of NumHarqProcess to 16.
- #166 Avoid Time overflow in nr-ch-access-manager.cc.
- Fixed to use NS_LOG_FUNCTION instead of NS_LOG_INFO for function calls.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues



Release NR-v2.5
----------------

Availability
------------
Available since July 26, 2023

Cite this version
-----------------
DOI: 10.5281/zenodo.8188631

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-9 or later
- clang-6 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- Arch Linux with g++-13 and clang-13.
- Ubuntu 20.04 with g++-9 and 10.
- Ubuntu 22.04 with g++11 and 12 and clang-11 and 14.

Recommended ns-3 release: ns-3.39

Important news
--------------
This release is compatible with ns-3.39.

Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both, ns-3 and the NR module. The information
about compatibility with the corresponding ns-3 release branch is stated in the
`README.md` file.

New user-visible features
-------------------------
- New QoS schedulers are included that perform scheduling by taking into account the
QoS requirements of QoS flows. `NrMacSchedulerTdmaQos` and `NrMacSchedulerOfdmaQos`
classes are responsible for setting the scheduler and access mode types when desired
by the user and updating the DL and UL metrics of each UE. `NrMacSchedulerUeInfoQos`
performs the sorting of the UEs (based on DL and UL metrics).

- New design for LC bytes assignment that allows the implementation of various
algorithms. A new base class is added, known as `NrMacSchedulerLcAlgorithm` that
allows the implementation of various algorithms for the LC byte assignment. Two
algorithms are implemented. `NrMacSchedulerLcRR` that includes the original
implementation of assigning bytes to LCs in RR fashion and `NrMacSchedulerLcQos`
that shares bytes among the active LCs by taking into account the resource type
and the e_rabGuaranteedBitRate of a flow.

Bugs fixed
----------
None.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.com/cttc-lena/nr/-/issues



Release NR-v2.4
----------------

Availability
------------
Available since April 5, 2023

Cite this version
-----------------
DOI: 10.5281/zenodo.7807983

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-9 or later
- clang-8 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- ArchLinux with g++-9, 10 and 11, and clang-8, 9, 10, 11 and 12.

Recommended ns-3 release: ns-3.38

Important news
--------------
Starting with this NR release, the `NR` module is compatible with REUSE software
(https://reuse.software/).
The old way of licensing the code is replaced by REUSE format and syntax.
The `NR` module pipelines have been extended to include `REUSE` job.

This release adds new traffic models to the NR module, NGMN and 3GPP Extended
Reality (XR) models.

With this release we have started to test the reproducibility of the examples,
e.g. the examples that are part of this reproducibility testing will fail if
the result is not as expected for the specific parameters configuration.

The `NR` module now has its `DOI` number which can be found in `RELEASE_NOTES.md`.

The `NR` module has a new pipeline job that checks the compatibility with the
latest ns-3 clang-format.

Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both, ns-3 and the NR module. E.g., the current
NR module Release 2.4 is compatible with the ns-3.38 release branch. The
information about compatibility with the corresponding ns-3 release branch
is stated in the `README.md` file.

New user-visible features (old first)
-------------------------
- The testing of the `NR` module has been extended to test the reproducibility of
the NR module examples results. To achieve this we have extended some of the `NR`
examples to include the verification of the produced results in terms of KPIs,
such as throughput and/or delay. This verification can help us
detect when some of the changes either in ns-3 modules or in the NR
features implementation or configurations affects and changes the original
performance of the `NR` module. The goal of this testing is not only to
guarantee the reproducibility, but also to allow an early detection of the
bugs in the code or in the configuration. The examples that are currently
covered to some extent with this reproducibility check are:
`cttc-nr-notching.cc`, `cttc-3gpp-channel-example`, `cttc-3gpp-channel-nums-fdm`,
`cttc-3gpp-channel-nums`, and `cttc-nr-demo`.

- The `NR` module now includes a new traffic generators framework that allows to
simulate NGMN traffic applications for mixed traffic scenarios and advanced
and multi-flow 3GPP XR traffic applications, such as Virtual Reality (VR),
Augmented Reality (AR), and Cloud Gaming (CG) applications.
The traffic models are included in `nr/utils/traffic-generators`,
with a goal to port them in the future to the ns-3 applications
module. To do that, we currently lack of tests for the 3GPP traffic generators.
This NR traffic generators framework adds the following traffic models to the
`NR` module: NGMN FTP, NGMN video streaming, NGMN gaming, NGMN VoIP, 3GPP AR Model
3A 3 streams: pose/control, scene/video and audio/data, 3GPP VR downlink 1 stream:
scene/video, 3GPP VR downlink 2 streams: scene/video and audio/data, 3GPP VR
uplink: pose/control, 3GPP CG downlink 1 stream: scene/video, 3GPP CG downlink
2 streams: scene/video and audio/data, and 3GPP CG uplink: pose/control.
The traffic generator framework can be easily extended to include more traffic
types. For more information about these models, please take a look in the NR
manual sections `NGMN mixed and 3GPP XR traffic models`, `Examples` and
`Test for NGMN traffic models`. New examples are added to demonstrate the usage
of the new traffic models. These are: `traffic-generator-example.cc`,
`cttc-nr-traffic-ngmn-mixed.cc`, and `cttc-nr-traffic-generator-3gpp-xr.cc`. Also,
`traffic-generator-test` is added to test traffic generator framework, and it
currently supports testing of NGMN traffic types.

Bugs fixed
----------
- Detected and fixed a bug when postponing transmissions in NR-U simulations.

- Fixed an error in PointInFTPlane constructor, i.e., m_rbg was defined
as `uint32_t`, but the constructor parameter was using `uint8_t`.

* Fixed an issue with `S` slots in `NrGnbPhy` and `NrPhy` to treat correctly `S`
slots when evaluating if they can be used for DL or UL transmissions.

* Fixed a bug in the `NrMacSchedulerNs3` in functions `DoScheduleDl` and
`DoScheduleUl` when updating the active DL and UL users list.

* Fixed a bug in `NrGnbPhy::StartSlot` that was not allowing to use a flexible
and configurable number of `CTRL` symbols per slot.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr



Release NR-v2.3
----------------

Availability
------------
Available since November 23, 2022

Cite this version
-----------------
DOI: 10.5281/zenodo.7780747

Supported platforms
-------------------
The supported platforms are the same as for the NR-v2.1 release, except that
the recommended ns-3 release is ns-3.37.

This release has been tested on the following platforms:
- ArchLinux with g++-9, 10 and 11, and clang-8, 9, 10, 11 and 12.

Important news
--------------
The module follows now the clang-format C++ code style. Clang-format can be easily
integrated with modern IDEs or run manually on the command-line.
Supported versions are:
- Clang-format-14
- Clang-format-15
- Clang-format-16

For more information please refer to
https://gitlab.com/nsnam/ns-3-dev/-/blob/master/doc/contributing/source/coding-style.rst

Whitespaces are also checked with:

```
$ python3 ns-3-dev/utils/trim-trailing-whitespace.py --check nr
```

This module can be updated with the usual:

```
$ git pull
```

command.

Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both, ns-3 and the NR module. E.g., the NR module
Release 2.2 is compatible with the ns-3.36.1 release branch, while the NR module
Release 2.3 is compatible with the ns-3.37 release branch.

The information about compatibility with the corresponding ns-3 release branch
is stated in this (RELEASE_NOTES.md) document in the "Supported platforms"
section for each NR release (starting from the NR Release 1.3).

New user-visible features (old first)
-------------------------
- Upgrade nr to `ThreeGppSpectrumPropagationLossModel::DoCalcRxPowerSpectralDensity`
changes.
- Added new example called `cttc-nr-3gpp-calibration` used for the calibration
of the simulator under 3GPP outdoor reference scenarios.
- Added `DlDataSnrTrace`, `DlCtrlPathloss` and `DlDataPathloss` trace sources in
NrSpectrumPhy.
- `NrUePhy` now includes the RSRP measurements of a UE.
- PHY traces are extended with a function to set the results folder path.
- `HexagonalGridScenarioHelper` is extended to define the max UE distance from the
closest site for the `HexagonalGridScenarioHelper::CreateScenarioWithMobility`
function. Moreover, the simTag and the results folder can now be set.
- The antenna orientation in the `NodeDistributionScenarioInterface::GetAntennaOrientationDegrees`
is changed from 60, 180, 300 degrees to 30, 120, 270.
- `GridScenarioHelper` includes now a function to set the starting position of the grid.
- Included some performance enhancements, such as to remove from `NrEesmErrorModel`
unnecessary copy, to allow the `NrErrorModel` to be passed and fetched as an object
and to reduce the execution of tests and examples_to_run.py.

Bugs fixed
----------
- Sfnsf frame number is expanded to 32-bit to prevent rollover
- Fixed how the HARQ feedback from multiple streams is combined in `NrUePhy`.
- Fixed and modified the code for MAC UL/DL RLC TX/RX/PDU queues.
- Included a fix in NrHelper to avoid reassigning a stream due to incorrect pointer.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr



Release NR-v2.2
----------------

Availability
------------
Available since June 03, 2022

Supported platforms
-------------------
The supported platforms are the same as for the NR-v2.1 release, except that
the recommended ns-3 release is ns-3.36.1.

Important news
--------------
This module can be updated with the usual

```
$ git pull
```

command.

Remember to follow the instructions from the README.md file, i.e., to checkout
the correct release branch of both, ns-3 and the NR module. E.g., the NR module
Release 2.1 is compatible with the ns-3.36 release branch, while the NR module
Release 2.2 is compatible with the ns-3.36.1 release branch.

The information about compatibility with the corresponding ns-3 release branch
is stated in this (RELEASE_NOTES.md) document in the "Supported platforms"
section for each NR release (starting from the NR Release 1.3).

New user-visible features (old first)
-------------------------

None.

Bugs fixed
----------

None.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr



Release NR-v2.1
---------------

Availability
------------
Available since May 06, 2022

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-9 or later
- clang-8 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- ArchLinux with g++-9 and 10 clang-8, 9, 10, and 11

Recommended ns-3 release: ns-3.36.

Important news
--------------
This module can be updated with the usual

```
$ git pull
```

command. We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------
- Added new distance-based 3GPP spectrum propagation loss model
- Added the Get function to obtain the pointer to PHY traces
- Added scenario with UE mobility in `HexagonalGridScenarioHelper` class
- Added option to set random antenna height in centrain percentage of UEs in
`HexagonalGriScenarioHelper`
- Extended `HexagonalGridScenarioHelper` to allow installing the hexagonal scenario
with the 4th and the 5th ring (needed for the wrap around calibration)
- Added new attribute to `NrMacSchedulerNs` to allow enabling or disabling HARQ ReTx
- `NrRadioEnvironmentMapHelper` is extended to provide the progress report at std::
cout, i.e., 1%, 10%, ..., 100%, and provides the estimation of the time left
- Added CQI column in RxPacketTraceUe
- Added SIR calculation and plot in `NrRadioEnvironmentMapHelper`
- Added CellScan algorithm based on azimuth and zenith in class called
`CellScanBeamformingAzimuthZenith` in `ideal-beamforming-algorithm.h/cc`
- Added new trace for reporting DL SINR CTRL
- Extended and improved RLC and PDCP traces to include simple traces per RX/TX
side, and combined/merged end-to-end traces.



Bugs fixed
----------
- Fixed how to consider RLC overhead when updating the TX queues in MAC scheduler
- Fixed attachment for loop in `lena-lte-comparison` example
- Fixed wrong assignation of NetDeviceContainer vector for REM in `lena-lte-comparison`
example
- Fixed REM sector index bug in lena-lte-comparison.cc example
- Fix the buffer size calculation in `NrMacSchedulerLcg` to consider correctly the
RLC overhead
- Improved formatting of the RLC/PDCP traces

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr



Release NR-v2.0
--------------

Availability
------------
Available since April 21, 2022

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-9 or later
- clang-8 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- ArchLinux with g++-9 and 10 clang-8, 9, 10, and 11

Recommended ns-3 release: ns-3.36. (If ns-3.36 is not available yet, use ns-3
master branch.)

Important news
--------------
This module can be updated with the usual

```
$ git pull
```

command. We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------
- The NR module now supports DP-MIMO spatial multiplexing. The NR DP-MIMO model
currently supports MIMO with 2 streams for the downlink, however, in the future,
the MIMO model will be extended to support more streams and the operation in the
uplink.
The current, NR DP-MIMO model exploits dual-polarized antennas and their
orthogonality to send the two data streams, by exploiting polarization diversity.
The model does not rely on abstraction, as is it case with LTE MIMO,
and thus can more properly model the propagation differences between the two
streams and account for rank adaptation. So, one of the important new features
that enters with this release along the NR DP-MIMO is the rank adaptation.
The major modifications to support the DP-MIMO feature are inside the PHY and
MAC layers of the NR module.


Bugs fixed
----------


Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v1.3
---------------

Availability
------------
Available since April 7, 2022.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-9 or later
- clang-8 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- ArchLinux with g++-9 and 10 clang-8, 9, 10, and 11

Recommended ns-3 release: ns-3.36.

Important news
--------------
In past, the NR module releases were not bound to any particular ns-3 release
because we were trying to keep in sync with the latest advancements in ns-3-dev
(master). Since ns-3 has had many disruptive API changes since its release 3.35
towards the upcoming release 3.36, and it also changed the build system from waf to
cmake, many 5G-LENA users have reported issues related to the unsuccessful
compilation of NR v1.2 with the latest ns-3 master.

Hence, starting from this NR release we decided to bind each NR release to
the specific ns-3 release by recommending the ns-3 release with which has been
tested the specific NR release.

Additionally, due to many accumulated changes in the NR module, that were waiting
for the next ns-3 release, we have decided to split all these upcoming changes into
several incremental releases to facilitate the 5G-LENA users to upgrade
their code gradually.

As a first step towards that goal, we have created this NR release v1.3 to
provide to the 5G-LENA users a version of the NR module that is compatible with
the upcoming ns.3-36.

(This release has been tested with ns-3-dev master with commit 4a98f050, and is
expected to be fully compatible with ns-3.36).

This module can be updated with the usual

```
$ git pull
```

command.

We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------
- Switch to cmake build system
- Upgrade nr to spectrum changes that entered with the MR 686
- Upgrade nr to lte changes that entered with the MR 810

Bugs fixed
----------

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr



Release NR-v1.2
--------------

Availability
------------
Available since June 4, 2021.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note: not all features available on all platforms):
- g++-7 or later
- clang-8 or later
- (macOS only) Xcode 10.1 or later

This release has been tested on the following platforms:
- ArchLinux with g++-7, 8, 9, and 10 clang-8, 9, 10, and 11

Important news
--------------
This module can be updated with the usual

```
$ git pull
```

command. We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------
- File Transfer Protocol (FTP) model 1 traffic model is included.

Bugs fixed
----------
- The computation of the effective SINR for the error modeling under HARQ-IR uses
now an updated formula that accounts for pure IR with no repetition of coding bits.
- There were cases in which multiple UEs could be assigned the same SRS offset value,
because the generation of the possible SRS offset was including multiple 0 values.
Now, the generation of the available values for the SRS offsets has been updated
to not contain multiple 0 values.
- Realistic beamforming algorithm with trigger event configured as delay update
uses the actual channel at SRS reception moment for real BF update with delay.
- There have been reported cases where an assert in PHY was triggered due to the
fact that the Allocation Statistics were not in accordance with the real allocation.
This happened because the `SlotAllocInfo` structure, and in particular the
`m_numSymAlloc` field, was not updated accurately when a UE didn’t get a
DCI (i.e., when the TBS is less than 7 bytes). Scheduler, now correctly updates
the `m_numSymAlloc` field and the `usedSym` variable in `NrMacSchedulerNs3::DoScheduleDlData`
and `NrMacSchedulerNs3::DoScheduleUlData` when DCI is not created.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v1.1
--------------

Availability
------------
Available since March 2, 2021.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note:  not all features available on all platforms):

- g++-4.9 or later
- Apple LLVM version 7.0.2 or later
- clang-3.3 or later

In addition, Python 2.7 (Python 2 series) or Python 3.4-3.8 (Python 3 series)

This release has been tested on the following platforms:
- ArchLinux with g++-9.2.1, clang-9.0.1, and Python 3.8.1
- Ubuntu 18.04 (64 bit) with g++-8.3.0 and Python 2.7.12/3.5.2

Important news
--------------
This module can be updated with the usual

```
$ git pull
```

command. We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------
- The scheduler can selectively leave particular RBG empty. This feature is
  called notching, and is used when multiple gNBs are collaborating to avoid
  interferences over a spectrum part.
- Added SRS allocation, transmission, and reception. Added an SRS message that
  takes 4 symbols (in the default configuration) within some periodicity (default
  at 80 slots). SRS are dynamically scheduled by the gNB (with an interface and
  an example specialized scheduler for it, `NrMacSchedulerSrsDefault`), and its
  allocation is signaled to the UE through a DCI. This is used by the UE to
  transmit SRS.
- `RealisticBeamformingAlgorithm` class is added. It implements a
  beamforming algorithm that determines the beamforming vectors of the transmitter
  and the receiver based on the SINR SRS.
- Uplink power control functionality implemented through the `NrUePowerControl`
  class, supporting UL power control for PUSCH, PUCCH, and SRS.
- IPV6 is now supported. That is, the end-to-end connections between the UEs
  and the remote hosts can be IPv4 or IPv6.

Bugs fixed
----------
- BeamManager called the function with the name "ChangeToOmniTx" of 3gpp
  antenna. This was causing that the CTRL was not being passed through 3gpp
  spectrum propagation model, but only through the propagation loss model.
- `GridScenarioHelper` fixes to correctly place nodes.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v1.0
--------------

Availability
------------
Available since Sept. 16, 2020.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note:  not all features available on all platforms):

- g++-4.9 or later
- Apple LLVM version 7.0.2 or later
- clang-3.3 or later

In addition, Python 2.7 (Python 2 series) or Python 3.4-3.8 (Python 3 series)

This release has been tested on the following platforms:
- ArchLinux with g++-9.2.1, clang-9.0.1, and Python 3.8.1
- Ubuntu 18.04 (64 bit) with g++-8.3.0 and Python 2.7.12/3.5.2

Important news
--------------
The v1.0 can now be installed in the ns-3-dev repository, or any ns-3 version
starting from ns-3.31. This module can be updated with the usual

```
$ git pull
```

command. We hope you will have fun and good times in using our module!

New user-visible features (old first)
-------------------------

- Renamed all mmwave- classes, tests, examples, helpers, to nr-. Renamed all
  the classes by replacing the prefix `MmWave` with `Nr`.
- Renamed the `Enb` part in `Gnb` (e.g., `NrEnbPhy` -> `NrGnbPhy`)
- Processing delays N0, N1, N2 are introduced as attributes of NrEnbPhy,
  respectively for the DL DCI delay, DL HARQ feedback delay, and UL DCI delay.
  The values K0, K1, K2 (definition in the 3GPP standard) are then calculated
  and communicated to the UE in the DCI. The N2Delay parameter replaces the old
  UlSchedDelay parameter.
- Removed PhyMacCommon. Its attributes are now divided among different
  classes. Please check CHANGES.md for the list.
- Separated NrEesmErrorModel in four different classes: NrEesmIrT1, NrEesmIrT2,
  NrEesmCcT1, NrEesmCcT2. These classes encapsulate the properties (harq method,
  table) that were an attribute of NrEesmErrorModel.
- Added the LENA error model. To be used only in conjunction with a OFDMA
  scheduler, and without beams. This error model has the same performance and
  values as the error model used in the lte/ module. The reference file is
  `lena-error-model.h`.
- Added the attribute `RbOverhead` to the NrGnbPhy and NrUePhy to set the
  bandwidth overhead to keep in consideration when calculating the number of
  usable RB. By default, it is set to 0.04, while in the previous versions the
  effect was like a value set to 0.0 (0.0 means that there are no guard bands,
  and the entire bandwidth is usable).
- Starting with this release the simulator is using new ns-3-dev 3ggp
  channel, spectrum, propagation, channel condition and antenna models
  that are implemented in spectrum, propagation and antenna modules of
  ns-3-dev. To allow usage of this new channel and antenna models, we have
  introduced a new BeamManager class which is responsible configuration of
  beamforming vectors of antenna arrays. BeamManager class is also responsible
  of configuring quasi-omni beamforming vector for omni transmissions.
  Since real beamforming management methods are still not implemented
  in our module, there are available ideal beamforming methods: cell scan
  and direct path. User can configure ideal beamforming method by using
  attribute of IdealBeamformingHelper which is in charge of creating
  the corresponding beamforming algorithm and calling it with configured
  periodicity to generate beamforming vectors for pairs of gNBs and UEs.
  BeamManager class is then responsible to cache beamforming vectors for
  antenna. For example, at gNB BeamManager for each connected UE device
  there will be cached the beamforming vector that will be used for
  communication with that UE. In the same way, the BeamManager at UE
  serves the same purpose, with the difference that it will be normally just one
  element in the map and that is toward its own gNB.
- Starting with this release the default behaviour will be to calculate
  interference for all the links, and will not be any more possible to exclude
  UE->UE and GNB->GNB interference calculations.
- NrHelper is now refactored to take into account Multi-Cell Configurations
  (i.e., different configuration for different cells).
- Introduced the TDD pattern: every gNb can use a different pattern, specified
  by a string that identify a sequence of slot types (e.g.,
  "DL|UL|UL|DL|DL|DL|").
- Introduced FDD operational mode for a Bandwidth Part.
- A new Component Carrier/Bandwidth Part helper has been added in file
  `cc-bwp-helper.h`. With this class, it is easy to divide the spectrum in
  different regions.
- Added the support for NR MAC sub-headers and subPDU.
- Added SHORT_BSR as MAC CE element, that goes with MAC data, and is evaluated
  by the error model upon delivery.
- A new NrRadioEnvironmentMapHelper has been implemented for the generation of
  DL and UL REM maps.

Bugs fixed
----------

- Removed legacy and invalid mmwave-* examples, that were inherited from mmWave codebase.
- The code performing LBT at UE side always assumed a DL CTRL symbol inside a slot.
  With TDD, that may not happen, and the code has been updated to not
  always assume a DL CTRL.
- TDMA Scheduler was assuming that there are always UE to schedule. That may not be true
  if these UEs have scheduled an HARQ retransmission (and hence unable to
  receive a new data DCI). Code updated to remove this assumption.
- gNb was scheduling first DL and then UL, so when it was calculating the symbols to be
  assigned to DL, it was not taking into account the UL symbols. Code has been updated to
  schedule first UL and then DL.
- N1 (used to schedule DL HARQ Feedback) could not be set to zero, because the way the
  calculation was carried out was resulting in negative delay. Calculation has been updated
  to take into account the case of N1=0.
- The Harq timer is now set to 0 every time a retransmission is scheduled and
  transmitted (before, it was just incremented)
- The BwpManagerGnb now redirects BSR and SR to the source bandwidth part, as
  the UE has already done the selection based on the configured QCI.
- LBT at the gNb side is now scheduled (and done) every time there is a DL CTRL
  to transmit, as in FDD configuration, other BWP can inject messages after
  the start of the slot.
- NrLteErrorModel contained a bug that prevented the calculation of the right
  error value.
- Scheduler contained a bug that was forcing, in some situation, a double SR
  scheduling, stopped by a FATAL_ERROR.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v0.4
--------------

Availability
------------
Available since Thu Feb 13 2020.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note:  not all features available on all platforms):

- g++-4.9 or later
- Apple LLVM version 7.0.2 or later
- clang-3.3 or later

In addition, Python 2.7 (Python 2 series) or Python 3.4-3.7 (Python 3 series)

This release has been tested on the following platforms:
- ArchLinux with g++-9.2.1, clang-9.0.1, and Python 3.8.1
- Ubuntu 18.04 (64 bit) with g++-8.3.0 and Python 2.7.12/3.5.2

Important news
--------------
- This release is aligned with CTTC's ns-3-dev commit id
217b410c lte-rlc: TM is now sending more than one packet per transmission opportunity
that is on top of the nsnam ns-3-dev master commit id
578c107eb internet: fix packet deduplication test .
To upgrade CTTC's ns-3-dev, please run the following (save any non-official
commit, as they will be deleted):

$ cd /path/to/cttc/ns-3-dev
$ git reset --hard HEAD~200
$ git pull

This module can be updated with the usual

$ git pull


New user-visible features (old first)
-------------------------------------

- (error model) new BLER-SINR tables for MCS table1 and table2
- (performance) Various performance improvements


Bugs fixed
----------
- (scheduler) Correctly schedule beams of users that got a HARQ retx space.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v0.3
--------------

Availability
------------
Available since Tue Aug 27 2019.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note:  not all features available on all platforms):

- g++-4.9 or later
- Apple LLVM version 7.0.2 or later
- clang-3.3 or later

In addition, Python 2.7 (Python 2 series) or Python 3.4-3.7 (Python 3 series)

This release has been tested on the following platforms:
- ArchLinux 2018.10.08 with g++-8.2.1 and Python 3.7.1
- Ubuntu 16.04 (64 bit) with g++-5.4.0 and Python 2.7.12/3.5.2

New user-visible features (old first)
-------------------------

- (error model) Added the NrEesmErrorModel class. It models the NR PHY
abstraction according to LDPC coding, block segmentation, and including
MCS/CQI table 1 and 2. The user can select the HARQ method (HarqCc or HarqIr)
as well as the MCS/CQI table of NR to be used (McsTable1 or McsTable2), through
new attributes. In this release, the BLER-SINR tables are not completed yet,
and so it is recommended not to use this error model.

- (3gpp channel model) 3gppChannelModel can now be used by any other module,
i.e. it is not any more mmwave specific spectrum propagation model. This means
that any subclass of NetDevice can be attached to a channel using this
SpectrumPropagationModel. An additional requirement is that the technology uses
AntennaModel that is implementing AntennaArrayBasicModel interface. This
feature is a basic prerequisite for simulating co-existence of different
technologies that are using the 3gpp channel model implementation.

- (3gpp channel model) The beamforming phase has been extracted from the model,
and it is now a duty of the NetDevice. The gNB phy has now a new attribute to
configure the periodicity of the beamforming. Please note that it is still ideal,
i.e., it does not require any simulated time to be performed.

- (3gpp channel model) gNB-gNB and UE-UE pathloss and channel computation can be
allowed (through a new attribute) for Indoor Hotspot scenarios.

- (spectrum phy) gNB-gNB and UE-UE interferences can be enabled (through a new
attribute).

- (RRC) Now all carriers are registered to RRC, to transmit system information
through all the bandwidth parts.

- (SCHED) The scheduler now is informed of RACH preamble messages, and reserve
some space in the DL CTRL symbol to send the RAR messages.

- Added traces that indicate the transmission or the reception of CTRL
messages. For instance, take a look to *EnbMacRxedCtrlMsgsTrace* or
*EnbMacTxedCtrlMsgsTrace* in the Gnb MAC file.

Bugs fixed
----------
- (scheduler) Fixed the use of a static MCS value in the schedulers
- (spectrum-phy) While looping the packets in the packet burst,
  to send the feedback, extract the RNTI for each loop. Before, it was
  asserting when the RNTI changed. The code does not depend on the
  RNTI previous values, so it should be safe.
- (phy) At the beginning, fill some slot (number configured by UL sched delay
  param) with UL CTRL symbol.

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr


Release NR-v0.2
---------------

Availability
------------
Available since Fri Feb 1 2019.

Supported platforms
-------------------
This release is intended to work on systems with the following minimal
requirements (Note:  not all features available on all platforms):

- g++-4.9 or later
- Apple LLVM version 7.0.2 or later
- clang-3.3 or later

In addition, Python 2.7 (Python 2 series) or Python 3.4-3.7 (Python 3 series)

This release has been tested on the following platforms:
- ArchLinux 2018.10.08 with g++-8.2.1 and Python 3.7.1
- Ubuntu 16.04 (64 bit) with g++-5.4.0 and Python 2.7.12/3.5.2

New user-visible features (old first)
-------------------------

- (mmwave) Removed any reference to Rlc Low Latency
- (mmwave) The code that was previously under the directory src/mmwave has been
  moved to src/nr.
- (nr) Aligned ComponentCarrierGnb, MmWaveEnbMac, MmWaveUePhy to ns-3-dev
- (nr) Aligned the BwpManager and the various helper/example to the bearer
  definitions in Rel. 15
- (nr) Removed unsupported MmWaveBeamforming and MmWaveChannelMatrix classes.
- (nr) Added a 3GPP-compliant antenna class.
- (nr) Added a 3GPP-compliant UL scheduling request feature.

Bugs fixed
----------

Known issues
------------
In general, known issues are tracked on the project tracker available
at https://gitlab.cttc.es/ns3-new-radio/nr
