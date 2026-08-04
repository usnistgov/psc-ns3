/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 *
 * Author: Nihar Kapasi <niharkkapasi@gmail.com>
 */

#ifndef NETSIM_MCPTT_BASIC_GROUP_CHARTER
#define NETSIM_MCPTT_BASIC_GROUP_CHARTER

#ifdef HAS_NETSIMULYZER

#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-trace-helper.h>

#include <ns3/application-container.h>
#include <ns3/color.h>
#include <ns3/ecdf-sink.h>
#include <ns3/object-map.h>
#include <ns3/orchestrator.h>
#include <ns3/packet.h>
#include <ns3/series-collection.h>
#include <ns3/type-id.h>
#include <ns3/xy-series.h>

namespace ns3
{

namespace psc
{

using namespace std;

/*
 * Create Timeline and Ecdf graphs for (displayed in SeriesCollection as curves per grp):
 * - M2eLatency
 * - AccessTime (only limits to result of I/Q)
 *
 * Usage: First call SetUp()
 *        then AddGroup for each basic group you wish to enable tracing for
 * ASSUMPTION: Only one McpttPttApp per node (to change could have a list of McpttPttApps per node
 *                                            and iterate through that in GetGrpId() method)
 * ASSUMPTION: call ids are globally unique (ensured by McpttServerApp::AllocateId in
 * McpttCallHelper) ASSUMPTION: user ids are unique per app (ensured by McpttHelper)
 */
class NetSimMcpttPttBasicGroupCharter
{
  public:
    NetSimMcpttPttBasicGroupCharter() = default;

    /*
     *
     * \returns 1 on success
     *          0 (returns immediately) on recall after succesfull setup
     *          -1 unsuccessful setup (orchestrator = nullptr)
     */
    int SetUp(Ptr<netsimulyzer::Orchestrator> orchestrator);

    /*
     * Method to add group for tracing
     * ASSUMPTION: pttApps are all part of the same group
     * \param grpId - the group id used in the McpttCallHelper::AddCall/AddCallOffNetwork
     * \param nextColor - the color to be used for the graphs associated with this group
     * \returns 1 on success
     *          -1 on failure (#pttApps < 2 | grpId < 0 | reusing a grpId)
     */
    int AddGroup(ApplicationContainer pttApps, uint32_t grpId, netsimulyzer::Color3Value nextColor);

  private:
    /*
     * Links to AccessTime and M2eLatency trace sources of McpttTraceSources
     * This links to all nodes in the simulation
     * As a result other methods will check again other private maps to ensure that
     * data being fetched is only for nodes/apps added via AddGroup
     */
    void LinkTraces();

    /*
     * Sets up the series collections for time series and ecdf graphs
     */
    void SetUpGraphCollections();

    /*
     * \param id The globally unique user id or node id
     * \return the group id associated with the particular id, callId pair
     */
    int GetGroupId(uint32_t id, uint16_t callId);

    /*
     * Trace sink to update AccessTime related graphs
     */
    void AccessTimeTrace(Time t,
                         uint32_t userId,
                         uint16_t callId,
                         std::string result,
                         Time latency);

    /*
     * Trace sink to update M2eLatency related graphs
     */
    void M2eLatencyTrace(Time t, uint32_t ssrc, uint64_t nodeId, uint16_t callId, Time latency);

    /*
     * Boolean flag to denote successful setup
     */
    bool m_isSetUp = false;
    /*
     * Boolean flag to denote SeriesCollections are made
     */
    bool m_areGraphsSetUp = false;
    /*
     * Orchestrator to be used to make graphs
     */
    Ptr<netsimulyzer::Orchestrator> m_orchestrator;
    /*
     * Trace helper to hook to the M2e and AccessTime trace sources
     */
    Ptr<McpttTraceHelper> m_mcpttTraceHelper;
    /*
     * Maps McpttPttApp user ids and node ids to the corresponding McpttPttApp
     */
    std::map<uint32_t, Ptr<McpttPttApp>> m_idsToAppPtr;
    /*
     * Set of all group ids
     * Prevent users fo=rom reusing group id
     */
    std::set<uint32_t> m_allGrpIds;
    /*
     * Maps a grpId to its AccessTime Timeline graph
     */
    std::map<uint32_t, Ptr<netsimulyzer::XYSeries>> m_grpIdToAccessTimeTimelineSeries;
    /*
     * Maps a grpId to its AccessTime Ecdf graph
     */
    std::map<uint32_t, Ptr<netsimulyzer::EcdfSink>> m_grpIdToAccessTimeEcdf;
    /*
     * Maps a grpId to its M2eLatency Timeline graph
     */
    std::map<uint32_t, Ptr<netsimulyzer::XYSeries>> m_grpIdToM2eLatencyTimelineSeries;
    /*
     * Maps a grpId to its M2eLatency Ecdf graph
     */
    std::map<uint32_t, Ptr<netsimulyzer::EcdfSink>> m_grpIdToM2eLatencyEcdf;
    /*
     * Collection of AccessTime Ecdf graphs for each grp
     */
    Ptr<netsimulyzer::SeriesCollection> m_accessTimeEcdfCollection;
    /*
     * Collection of AccessTime Timeline graphs for each grp
     */
    Ptr<netsimulyzer::SeriesCollection> m_accessTimeTimelineCollection;
    /*
     * Collection of M2eLatency Ecdf graphs for each grp
     */
    Ptr<netsimulyzer::SeriesCollection> m_m2eLatencyEcdfCollection;
    /*
     * Collection of M2eLatency Timeline graphs for each grp
     */
    Ptr<netsimulyzer::SeriesCollection> m_m2eLatencyTimelineCollection;
};

} // namespace psc

} // namespace ns3

#endif /* HAS_NETSIMULYZER */

#endif /* NETSIM_MCPTT_BASIC_GROUP_CHARTER */
