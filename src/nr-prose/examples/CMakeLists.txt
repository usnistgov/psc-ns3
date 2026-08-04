set(nr-prose-examples_examples
    nr-prose-unicast-single-link
    nr-prose-discovery
    nr-prose-discovery-l3-relay
    nr-prose-discovery-l3-relay-selection
)
set(nr-prose-examples_flowmon_examples
    nr-prose-unicast-multi-link
    nr-prose-unicast-l3-relay
    nr-prose-network-coex
    nr-prose-l3-relay
)
foreach(
  example
  ${nr-prose-examples_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnr}
                      ${libnr-prose}
  )
endforeach()
foreach(
  example
  ${nr-prose-examples_flowmon_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnr}
                      ${libnr-prose}
                      ${libflow-monitor}
  )
endforeach()

set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${libnr} ${libnr-prose} ${libflow-monitor})
if(${NS3_NETSIMULYZER})
  set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${PROSE_L3_RELAY_ON_OFF_LIBRARIES} ${libnetsimulyzer})
endif()

build_lib_example(
  NAME nr-prose-l3-relay-on-off
  SOURCE_FILES nr-prose-l3-relay-on-off.cc
  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
)

# Also requires psc and sip modules; uncomment the below if they are present
#build_lib_example(
#  NAME nr-prose-l3-relay-mcptt
#  SOURCE_FILES nr-prose-l3-relay-mcptt.cc
#  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
#                    ${libnr-prose}
#                    ${libpsc}
#                    ${libsip}
#)

build_lib_example(
  NAME nr-prose-olsrv2
  SOURCE_FILES nr-prose-olsrv2.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}
)

build_lib_example(
  NAME multihop-grid
  SOURCE_FILES multihop-grid.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}
)

build_lib_example(
  NAME nr-prose-olsrv2-rlf
  SOURCE_FILES nr-prose-olsrv2-rlf.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}
)

build_lib_example(
  NAME multihop-random-waypoint
  SOURCE_FILES multihop-random-waypoint.cc
  LIBRARIES_TO_LINK ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpropagation}
                    ${libspectrum}
)

build_lib_example(
  NAME nr-prose-multihop-linear
  SOURCE_FILES nr-prose-multihop-linear.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}
)

build_lib_example(
  NAME nr-prose-multihop-advanced
  SOURCE_FILES nr-prose-multihop-advanced.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnhdp}
                    ${libnr}
                    ${libnr-prose}
                    ${libolsrv2}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}

)
build_lib_example(
  NAME nr-prose-adaptive-mcs
  SOURCE_FILES nr-prose-adaptive-mcs.cc
  LIBRARIES_TO_LINK ${libantenna}
                    ${libapplications}
                    ${libcore}
                    ${libinternet}
                    ${libmobility}
                    ${libnetwork}
                    ${libnr}
                    ${libnr-prose}
                    ${libpoint-to-point}
                    ${libpropagation}
                    ${libspectrum}
                    ${libstats}
                    ${libpsc}
                    ${libsip}

)

build_lib_example(
  NAME nr-prose-mcs-pareto-sweep
  SOURCE_FILES nr-prose-mcs-pareto-sweep.cc
  LIBRARIES_TO_LINK ${libcore}
                    ${libnr}
)
