set(base_examples
    lena-cc-helper
    lena-cqi-threshold
    lena-deactivate-bearer
    lena-distributed-ffr
    lena-dual-stripe
    lena-fading
    lena-frequency-reuse
    lena-intercell-interference
    lena-ipv6-addr-conf
    lena-ipv6-ue-rh
    lena-ipv6-ue-ue
    lena-pathloss-traces
    lena-profiling
    lena-radio-link-failure
    lena-rem
    lena-rem-sector-antenna
    lena-rlc-traces
    lena-simple
    lena-simple-epc
    lena-simple-epc-backhaul
    lena-uplink-power-control
    lena-x2-handover
    lena-x2-handover-measures
    lte-3gpp-hex-topology
    lte-wrap-around-hex-topology
)

set(d2d_examples
    lte-sl-in-covrg-comm-mode1
    lte-sl-in-covrg-comm-mode1-multicell
    lte-sl-in-covrg-comm-mode2
    lte-sl-in-covrg-comm-mode2-multicell
    lte-sl-in-covrg-discovery
    lte-sl-in-covrg-discovery-collision
    lte-sl-in-covrg-discovery-multi
    lte-sl-in-covrg-discovery-multicell
    lte-sl-in-covrg-relay-building
    lte-sl-in-covrg-relay
    lte-sl-in-covrg-relay-mode2-multicell
    lte-sl-out-of-covrg-comm
    lte-sl-out-of-covrg-discovery-collision
    lte-sl-out-of-covrg-synch
    lte-sl-out-of-covrg-synch-twotx
    lte-sl-relay-cluster
    lte-sl-relay-cluster-selection
    wns3-2017-discovery
    wns3-2017-pscch
    wns3-2017-pssch
    wns3-2017-synch
)

foreach(
  example
  ${base_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${liblte}
  )
endforeach()

foreach(
  example
  ${d2d_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES d2d-examples/${example}.cc
    LIBRARIES_TO_LINK ${liblte}
  )
endforeach()

if(${ENABLE_EMU})
  build_lib_example(
    NAME lena-simple-epc-emu
    SOURCE_FILES lena-simple-epc-emu.cc
    LIBRARIES_TO_LINK ${liblte}
                      ${fd-net-device}
  )
endif()
