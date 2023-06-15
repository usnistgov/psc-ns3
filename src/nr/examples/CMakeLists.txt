set(base_examples
    cttc-3gpp-channel-example
    cttc-3gpp-channel-simple-ran
    cttc-3gpp-channel-simple-fdm
    cttc-3gpp-indoor-calibration
    cttc-3gpp-channel-nums
    cttc-3gpp-channel-nums-fdm
    cttc-nr-cc-bwp-demo
    cttc-lte-ca-demo
    cttc-nr-demo
    cttc-error-model
    cttc-error-model-amc
    cttc-error-model-comparison
    cttc-channel-randomness
    rem-example
    rem-beam-example
    cttc-fh-compression
    cttc-nr-notching
    cttc-nr-mimo-demo
)

foreach(
  example
  ${base_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnr}
                      ${libflow-monitor}
  )
endforeach()

set(nr-v2x-examples_examples
    cttc-nr-v2x-demo-simple
    nr-v2x-west-to-east-highway
)
set(nr-v2x-examples_source_files
    nr-v2x-examples/ue-mac-pscch-tx-output-stats.cc
    nr-v2x-examples/ue-mac-pssch-tx-output-stats.cc
    nr-v2x-examples/ue-phy-pscch-rx-output-stats.cc
    nr-v2x-examples/ue-phy-pssch-rx-output-stats.cc
    nr-v2x-examples/ue-rlc-rx-output-stats.cc
    nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.cc
    nr-v2x-examples/v2x-kpi.cc
)
foreach(
  example
  ${nr-v2x-examples_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES nr-v2x-examples/${example}.cc
                 ${nr-v2x-examples_source_files}
    LIBRARIES_TO_LINK ${libnr}
                      ${libflow-monitor}
                      ${libstats}
  )
endforeach()

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
set(nr-prose-examples_source_files
    nr-v2x-examples/ue-mac-pscch-tx-output-stats.cc
    nr-v2x-examples/ue-mac-pssch-tx-output-stats.cc
    nr-v2x-examples/ue-phy-pscch-rx-output-stats.cc
    nr-v2x-examples/ue-phy-pssch-rx-output-stats.cc
    nr-v2x-examples/ue-rlc-rx-output-stats.cc
    nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.cc
    nr-v2x-examples/v2x-kpi.cc
)
foreach(
  example
  ${nr-prose-examples_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES nr-prose-examples/${example}.cc
                 ${nr-prose-examples_source_files}
    LIBRARIES_TO_LINK ${libnr}
  )
endforeach()
foreach(
  example
  ${nr-prose-examples_flowmon_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES nr-prose-examples/${example}.cc
                 ${nr-prose-examples_source_files}
    LIBRARIES_TO_LINK ${libnr}
                      ${libflow-monitor}
  )
endforeach()

set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${libnr} ${libflow-monitor})
if(${NS3_NETSIMULYZER})
  set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${PROSE_L3_RELAY_ON_OFF_LIBRARIES} ${libnetsimulyzer})
endif()

build_lib_example(
  NAME nr-prose-l3-relay-on-off
  SOURCE_FILES nr-prose-examples/nr-prose-l3-relay-on-off.cc
               ${nr-prose-examples_source_files}
  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
)

# To be enabled when psc and sip modules are added
#build_lib_example(
#  NAME nr-prose-l3-relay-mcptt
#  SOURCE_FILES nr-prose-examples/nr-prose-l3-relay-mcptt.cc
#               ${nr-prose-examples_source_files}
#  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
#                    ${libpsc}
#                    ${libsip}
#)

set(lena-lte-comparison_examples
    lena-lte-comparison-user
    lena-lte-comparison-campaign
)
set(lena-lte-comparison_source_files 
    lena-lte-comparison/lena-lte-comparison.cc
    lena-lte-comparison/sinr-output-stats.cc
    lena-lte-comparison/flow-monitor-output-stats.cc
    lena-lte-comparison/power-output-stats.cc
    lena-lte-comparison/slot-output-stats.cc
    lena-lte-comparison/lena-v1-utils.cc
    lena-lte-comparison/lena-v2-utils.cc
    lena-lte-comparison/rb-output-stats.cc
)
foreach(
  example
  ${lena-lte-comparison_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES lena-lte-comparison/${example}.cc
                 ${lena-lte-comparison_source_files}
    LIBRARIES_TO_LINK ${libnr}
                      ${libflow-monitor}
                      ${libstats}
  )
endforeach()

set(example cttc-realistic-beamforming)
set(source_files ${example}.cc)
set(libraries_to_link ${libnr} ${libflow-monitor} ${SQLite3_LIBRARIES})
build_lib_example(
  NAME ${example} 
  SOURCE_FILES ${source_files}
  LIBRARIES_TO_LINK ${libraries_to_link}
)
