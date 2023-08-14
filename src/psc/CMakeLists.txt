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

set(source_files
    helper/ims-helper.cc
    helper/intel-http-helper.cc
    helper/mcptt-call-helper.cc
    helper/mcptt-helper.cc
    helper/mcptt-msg-stats.cc
    helper/mcptt-server-helper.cc
    helper/mcptt-state-machine-stats.cc
    helper/mcptt-trace-helper.cc
    helper/psc-application-helper.cc
    helper/psc-scenario-trace-helper.cc
    helper/uav-mobility-energy-model-helper.cc
    helper/udp-group-echo-helper.cc
    model/intel-http-client.cc
    model/intel-http-header.cc
    model/intel-http-server.cc
    model/mcptt-call.cc
    model/mcptt-call-machine.cc
    model/mcptt-call-machine-grp-basic.cc
    model/mcptt-call-machine-grp-basic-state.cc
    model/mcptt-call-machine-grp-broadcast.cc
    model/mcptt-call-machine-grp-broadcast-state.cc
    model/mcptt-call-machine-private.cc
    model/mcptt-call-machine-private-state.cc
    model/mcptt-call-msg.cc
    model/mcptt-call-msg-field.cc
    model/mcptt-call-type-machine.cc
    model/mcptt-call-type-machine-grp-basic.cc
    model/mcptt-call-type-machine-private.cc
    model/mcptt-call-type-machine-private-state.cc
    model/mcptt-channel.cc
    model/mcptt-counter.cc
    model/mcptt-emerg-alert-machine.cc
    model/mcptt-emerg-alert-machine-basic.cc
    model/mcptt-entity-id.cc
    model/mcptt-floor-msg.cc
    model/mcptt-floor-msg-field.cc
    model/mcptt-floor-msg-sink.cc
    model/mcptt-floor-participant.cc
    model/mcptt-floor-queue.cc
    model/mcptt-media-msg.cc
    model/mcptt-media-src.cc
    model/mcptt-msg.cc
    model/mcptt-off-network-floor-participant.cc
    model/mcptt-off-network-floor-participant-state.cc
    model/mcptt-on-network-call-machine-client.cc
    model/mcptt-on-network-call-machine-client-state.cc
    model/mcptt-on-network-floor-arbitrator.cc
    model/mcptt-on-network-floor-arbitrator-state.cc
    model/mcptt-on-network-floor-dual-control.cc
    model/mcptt-on-network-floor-dual-control-state.cc
    model/mcptt-on-network-floor-participant.cc
    model/mcptt-on-network-floor-participant-state.cc
    model/mcptt-on-network-floor-towards-participant.cc
    model/mcptt-on-network-floor-towards-participant-state.cc
    model/mcptt-ptt-app.cc
    model/mcptt-pusher.cc
    model/mcptt-pusher-orchestrator.cc
    model/mcptt-pusher-orchestrator-contention.cc
    model/mcptt-pusher-orchestrator-interface.cc
    model/mcptt-pusher-orchestrator-session-cdf.cc
    model/mcptt-pusher-orchestrator-spurt-cdf.cc
    model/mcptt-queued-user-info.cc
    model/mcptt-rtp-header.cc
    model/mcptt-sdp-fmtp-header.cc
    model/mcptt-server-app.cc
    model/mcptt-server-call.cc
    model/mcptt-server-call-machine.cc
    model/mcptt-server-call-machine-group-prearranged.cc
    model/mcptt-server-call-machine-group-prearranged-state.cc
    model/mcptt-timer.cc
    model/psc-application.cc
    model/psc-application-client.cc
    model/psc-application-configuration.cc
    model/psc-application-server.cc
    model/psc-scenario-definition.cc
    model/psc-video-streaming.cc
    model/psc-video-streaming-distributions.cc
    model/uav-mobility-energy-model.cc
    model/udp-group-echo-server.cc
    )

set(header_files
    helper/ims-helper.h
    helper/intel-http-helper.h
    helper/mcptt-call-helper.h
    helper/mcptt-helper.h
    helper/mcptt-msg-stats.h
    helper/mcptt-server-helper.h
    helper/mcptt-state-machine-stats.h
    helper/mcptt-trace-helper.h
    helper/psc-application-helper.h
    helper/psc-scenario-trace-helper.h
    helper/uav-mobility-energy-model-helper.h
    helper/udp-group-echo-helper.h
    model/intel-http-client.h
    model/intel-http-header.h
    model/intel-http-server.h
    model/mcptt-call.h
    model/mcptt-call-machine.h
    model/mcptt-call-machine-grp-basic.h
    model/mcptt-call-machine-grp-basic-state.h
    model/mcptt-call-machine-grp-broadcast.h
    model/mcptt-call-machine-grp-broadcast-state.h
    model/mcptt-call-machine-private.h
    model/mcptt-call-machine-private-state.h
    model/mcptt-call-msg.h
    model/mcptt-call-msg-field.h
    model/mcptt-call-type-machine.h
    model/mcptt-call-type-machine-grp-basic.h
    model/mcptt-call-type-machine-private.h
    model/mcptt-call-type-machine-private-state.h
    model/mcptt-channel.h
    model/mcptt-counter.h
    model/mcptt-emerg-alert-machine.h
    model/mcptt-emerg-alert-machine-basic.h
    model/mcptt-entity-id.h
    model/mcptt-floor-msg.h
    model/mcptt-floor-msg-field.h
    model/mcptt-floor-msg-sink.h
    model/mcptt-floor-participant.h
    model/mcptt-floor-queue.h
    model/mcptt-media-msg.h
    model/mcptt-media-sink.h
    model/mcptt-media-src.h
    model/mcptt-msg.h
    model/mcptt-off-network-floor-participant.h
    model/mcptt-off-network-floor-participant-state.h
    model/mcptt-on-network-call-machine-client.h
    model/mcptt-on-network-call-machine-client-state.h
    model/mcptt-on-network-floor-arbitrator.h
    model/mcptt-on-network-floor-arbitrator-state.h
    model/mcptt-on-network-floor-dual-control.h
    model/mcptt-on-network-floor-dual-control-state.h
    model/mcptt-on-network-floor-participant.h
    model/mcptt-on-network-floor-participant-state.h
    model/mcptt-on-network-floor-towards-participant.h
    model/mcptt-on-network-floor-towards-participant-state.h
    model/mcptt-ptt-app.h
    model/mcptt-pusher.h
    model/mcptt-pusher-orchestrator.h
    model/mcptt-pusher-orchestrator-contention.h
    model/mcptt-pusher-orchestrator-interface.h
    model/mcptt-pusher-orchestrator-session-cdf.h
    model/mcptt-pusher-orchestrator-spurt-cdf.h
    model/mcptt-queued-user-info.h
    model/mcptt-rtp-header.h
    model/mcptt-sdp-fmtp-header.h
    model/mcptt-server-app.h
    model/mcptt-server-call.h
    model/mcptt-server-call-machine.h
    model/mcptt-server-call-machine-group-prearranged.h
    model/mcptt-server-call-machine-group-prearranged-state.h
    model/mcptt-timer.h
    model/psc-application.h
    model/psc-application-client.h
    model/psc-application-configuration.h
    model/psc-application-server.h
    model/psc-scenario-definition.h
    model/psc-video-streaming.h
    model/psc-video-streaming-distributions.h
    model/uav-mobility-energy-model.h
    model/udp-group-echo-server.h
    )

set(test_sources
    test/intel-http-model-test.cc
    test/mcptt-call-control.cc
    test/mcptt-call-control-broadcast.cc
    test/mcptt-call-control-msg.cc
    test/mcptt-call-control-on-network.cc
    test/mcptt-call-control-private.cc
    test/mcptt-call-type-control.cc
    test/mcptt-call-type-control-private.cc
    test/mcptt-floor-control.cc
    test/mcptt-floor-control-msg.cc
    test/mcptt-floor-control-on-network.cc
    test/mcptt-msg-dropper.cc
    test/mcptt-msg-dropper.h
    test/mcptt-test-call.cc
    test/mcptt-test-call.h
    test/mcptt-test-case.cc
    test/mcptt-test-case.h
    test/mcptt-test-case-config.cc
    test/mcptt-test-case-config.h
    test/mcptt-test-case-config-on-network.cc
    test/mcptt-test-case-config-on-network.h
    test/uav-mobility-energy-model-helper-test.cc
    test/uav-mobility-energy-model-test.cc
    )

build_lib(
    LIBNAME psc
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    TEST_SOURCES ${test_sources}
    LIBRARIES_TO_LINK
    ${libcore}
    ${libenergy}
    ${libmobility}
    ${libnetwork}
    ${libantenna}
    ${libbuildings}
    ${liblte}
    ${libpoint-to-point}
    ${libsip}
)
