# NIST-developed software is provided by NIST as a public service. You may use,
# copy and distribute copies of the software in any medium, provided that you
# keep intact this entire notice. You may improve,modify and create derivative
# works of the software or any portion of the software, and you may copy and
# distribute such modifications or works. Modified works should carry a notice
# stating that you changed the software and should note the date and nature of
# any such change. Please explicitly acknowledge the National Institute of
# Standards and Technology as the source of the software.
#
# NIST-developed software is expressly provided "AS IS." NIST MAKES NO
# WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
# LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
# AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
# OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
# ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
# REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
# INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
# OR USEFULNESS OF THE SOFTWARE.
#
# You are solely responsible for determining the appropriateness of using and
# distributing the software and you assume all risks associated with its use,
# including but not limited to the risks and costs of program errors,
# compliance with applicable laws, damage to or loss of data, programs or
# equipment, and the unavailability or interruption of operation. This
# software is not intended to be used in any situation where a failure could
# cause risk of injury or damage to property. The software developed by NIST
# employees is not subject to copyright protection within the United States.

build_lib_example(
    NAME example-udp-group-echo
    SOURCE_FILES example-udp-group-echo.cc
    LIBRARIES_TO_LINK
    ${libpsc}
)

build_lib_example(
    NAME uav-mobility-energy-example
    SOURCE_FILES example-udp-group-echo.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${libenergy}
    ${libmobility}
    ${libpsc}
)

build_lib_example(
    NAME example-intel-http
    SOURCE_FILES example-intel-http.cc
    LIBRARIES_TO_LINK
    ${libpsc}
)

build_lib_example(
    NAME example-psc-application
    SOURCE_FILES example-psc-application.cc
    LIBRARIES_TO_LINK
    ${libpsc}
)

build_lib_example(
    NAME camad-2019-communication
    SOURCE_FILES
    camad-2019/camad-2019-communication.cc
    camad-2019/lte-sl-ue-controller-camad-2019.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME camad-2019-connection
    SOURCE_FILES
    camad-2019/camad-2019-connection.cc
    camad-2019/lte-sl-ue-controller-camad-2019.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME camad-2019-discovery
    SOURCE_FILES
    camad-2019/camad-2019-discovery.cc
    camad-2019/lte-sl-ue-controller-camad-2019.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

set(school_shooting_libs
    ${libnetanim}
    ${libpsc}
    )

if ("netsimulyzer" IN_LIST ns3-all-enabled-modules)
  list(APPEND school_shooting_libs ${libnetsimulyzer})
endif ()

build_lib_example(
    NAME psc-schoolshooting
    SOURCE_FILES
    schoolshooting/psc-schoolshooting.cc
    schoolshooting/schoolshooting-definition-helper.cc
    schoolshooting/schoolshooting-lte-helper.cc
    schoolshooting/schoolshooting-network-technology-helper.cc
    schoolshooting/schoolshooting-application-helper.cc
    schoolshooting/schoolshooting-application-trace-helper.cc
    schoolshooting/schoolshooting-lte-vis-helper.cc
    LIBRARIES_TO_LINK
    ${school_shooting_libs}
)

build_lib_example(
    NAME example-video-streaming
    SOURCE_FILES example-video-streaming.cc
    LIBRARIES_TO_LINK
    ${libpsc}
)

build_lib_example(
    NAME example-mcptt-wifi-adhoc
    SOURCE_FILES example-mcptt-wifi-adhoc.cc
    LIBRARIES_TO_LINK
    ${libpsc}
    ${libwifi}
)

build_lib_example(
    NAME example-mcptt-pusher-orchestrator
    SOURCE_FILES example-mcptt-pusher-orchestrator.cc
    LIBRARIES_TO_LINK
    ${libpsc}
    ${libwifi}
)

build_lib_example(
    NAME mcptt-lte-sl-out-of-covrg-comm
    SOURCE_FILES mcptt-lte-sl-out-of-covrg-comm.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME example-mcptt-on-network-floor-control-csma
    SOURCE_FILES example-mcptt-on-network-floor-control-csma.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${libcsma}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME example-mcptt-on-network-floor-control-lte
    SOURCE_FILES example-mcptt-on-network-floor-control-lte.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME example-mcptt-on-network-two-calls
    SOURCE_FILES example-mcptt-on-network-floor-control-lte.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${libpoint-to-point}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME example-mcptt-on-network-two-simultaneous-calls
    SOURCE_FILES example-mcptt-on-network-two-simultaneous-calls.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${libpoint-to-point}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
)

build_lib_example(
    NAME mcptt-operational-modes-static
    SOURCE_FILES example-mcptt-on-network-two-simultaneous-calls.cc
    LIBRARIES_TO_LINK
    ${libcore}
    ${libpoint-to-point}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
    ${libbuildings}
)

set(mcptt_operational_modes_mobility_libs
    ${libcore}
    ${libpoint-to-point}
    ${liblte}
    ${libnetwork}
    ${libmobility}
    ${libinternet}
    ${libapplications}
    ${libpsc}
    ${libbuildings}
    )

if ("netsimulyzer" IN_LIST ns3-all-enabled-modules)
  list(APPEND mcptt_operational_modes_mobility_libs ${libnetsimulyzer})
endif ()

build_lib_example(
    NAME mcptt-operational-modes-mobility
    SOURCE_FILES mcptt-operational-modes-mobility.cc
    LIBRARIES_TO_LINK
    ${mcptt_operational_modes_mobility_libs}
)
