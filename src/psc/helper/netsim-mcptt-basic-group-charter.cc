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

#ifdef HAS_NETSIMULYZER

#include "netsim-mcptt-basic-group-charter.h"

#include <ns3/boolean.h>

namespace ns3
{

namespace psc
{

NS_LOG_COMPONENT_DEFINE("NetSimMcpttPttBasicGroupCharter");

using namespace std;

int
NetSimMcpttPttBasicGroupCharter::SetUp(Ptr<netsimulyzer::Orchestrator> orchestrator)
{
    // Setup already occured
    if (m_isSetUp)
    {
        return 0;
    }

    if (orchestrator == nullptr)
    {
        m_isSetUp = false;
        return -1;
    }

    m_orchestrator = orchestrator;
    LinkTraces();
    m_isSetUp = true;
    return 1;
}

void
NetSimMcpttPttBasicGroupCharter::SetUpGraphCollections()
{
    // Method already called once. Skip collection setup
    if (m_areGraphsSetUp)
    {
        return;
    }

    PointerValue xAxis;
    PointerValue yAxis;

    // Collection of AccessTime Ecdf graphs over all groups
    m_accessTimeEcdfCollection = CreateObject<netsimulyzer::SeriesCollection>(m_orchestrator);
    m_accessTimeEcdfCollection->SetAttribute("Name",
                                             StringValue("Access time - eCDF - All groups"));
    m_accessTimeEcdfCollection->GetAttribute("XAxis", xAxis);
    xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Access time (ms)"));
    m_accessTimeEcdfCollection->GetAttribute("YAxis", yAxis);
    yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Empirical CDF"));
    m_accessTimeEcdfCollection->SetAttribute("HideAddedSeries", BooleanValue(false));

    // Collection of M2eLatency Ecdf graphs over all groups
    m_m2eLatencyEcdfCollection = CreateObject<netsimulyzer::SeriesCollection>(m_orchestrator);
    m_m2eLatencyEcdfCollection->SetAttribute("Name",
                                             StringValue("M2E latency - eCDF - All groups"));
    m_m2eLatencyEcdfCollection->GetAttribute("XAxis", xAxis);
    xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("M2E latency (ms)"));
    m_m2eLatencyEcdfCollection->GetAttribute("YAxis", yAxis);
    yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Empirical CDF"));
    m_m2eLatencyEcdfCollection->SetAttribute("HideAddedSeries", BooleanValue(false));

    // Collection of AccessTime Timelin graphs over all groups
    m_accessTimeTimelineCollection = CreateObject<netsimulyzer::SeriesCollection>(m_orchestrator);
    m_accessTimeTimelineCollection->SetAttribute(
        "Name",
        StringValue("Access time - Timeline - All groups"));
    m_accessTimeTimelineCollection->GetAttribute("XAxis", xAxis);
    xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Time (s)"));
    m_accessTimeTimelineCollection->GetAttribute("YAxis", yAxis);
    yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Access time (ms)"));
    m_accessTimeTimelineCollection->SetAttribute("HideAddedSeries", BooleanValue(false));

    // Collection of M2eLatency Timelin graphs over all groups
    m_m2eLatencyTimelineCollection = CreateObject<netsimulyzer::SeriesCollection>(m_orchestrator);
    m_m2eLatencyTimelineCollection->SetAttribute(
        "Name",
        StringValue("M2E latency - Timeline - All groups"));
    m_m2eLatencyTimelineCollection->GetAttribute("XAxis", xAxis);
    xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Time (s)"));
    m_m2eLatencyTimelineCollection->GetAttribute("YAxis", yAxis);
    yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("M2E latency (ms)"));
    m_m2eLatencyTimelineCollection->SetAttribute("HideAddedSeries", BooleanValue(false));

    m_areGraphsSetUp = true;
}

void
NetSimMcpttPttBasicGroupCharter::LinkTraces()
{
    m_mcpttTraceHelper = CreateObject<McpttTraceHelper>();
    m_mcpttTraceHelper->EnableAccessTimeTrace();
    m_mcpttTraceHelper->EnableMouthToEarLatencyTrace();

    m_mcpttTraceHelper->TraceConnectWithoutContext(
        "AccessTimeTrace",
        MakeCallback(&NetSimMcpttPttBasicGroupCharter::AccessTimeTrace, this));
    m_mcpttTraceHelper->TraceConnectWithoutContext(
        "MouthToEarLatencyTrace",
        MakeCallback(&NetSimMcpttPttBasicGroupCharter::M2eLatencyTrace, this));
}

int
NetSimMcpttPttBasicGroupCharter::AddGroup(ApplicationContainer pttApps,
                                          uint32_t grpId,
                                          netsimulyzer::Color3Value nextColor)
{
    if ((pttApps.GetN() < 2) || m_allGrpIds.contains(grpId) || (!m_isSetUp))
    {
        return -1;
    }

    // Set-up collection graphs if not done so already
    // Called when adding the first group to be tracked
    if (!m_areGraphsSetUp)
    {
        SetUpGraphCollections();
        m_areGraphsSetUp = true;
    }

    // ECDFs for Access time
    auto groupAccessTimeEcdf =
        CreateObject<netsimulyzer::EcdfSink>(m_orchestrator,
                                             "Access time - eCDF - " + std::to_string(grpId));
    groupAccessTimeEcdf->GetXAxis()->SetAttribute("Name", StringValue("Access time (ms)"));
    groupAccessTimeEcdf->GetYAxis()->SetAttribute("Name", StringValue("Emperical CDF"));
    groupAccessTimeEcdf->GetSeries()->SetAttribute("Color", nextColor);
    // groupAccessTimeEcdf->SetConnectionType(netsimulyzer::XYSeries::ConnectionType::None);
    groupAccessTimeEcdf->GetSeries()->SetAttribute("Visible", BooleanValue(false));
    m_accessTimeEcdfCollection->Add(groupAccessTimeEcdf->GetSeries());

    // ECDFs for M2E latency
    auto groupM2eLatencyEcdfs =
        CreateObject<netsimulyzer::EcdfSink>(m_orchestrator,
                                             "M2E latency - eCDF - " + std::to_string(grpId));
    groupM2eLatencyEcdfs->GetXAxis()->SetAttribute("Name", StringValue("M2E latency (ms)"));
    groupM2eLatencyEcdfs->GetYAxis()->SetAttribute("Name", StringValue("Emperical CDF"));
    groupM2eLatencyEcdfs->GetSeries()->SetAttribute("Color", nextColor);
    // groupM2eLatencyEcdfs->SetConnectionType(netsimulyzer::XYSeries::ConnectionType::None);
    groupM2eLatencyEcdfs->GetSeries()->SetAttribute("Visible", BooleanValue(false));
    m_m2eLatencyEcdfCollection->Add(groupM2eLatencyEcdfs->GetSeries());

    // Timeline for Access time
    auto groupAccessTimeTimeline = CreateObject<netsimulyzer::XYSeries>(m_orchestrator);
    groupAccessTimeTimeline->SetAttribute(
        "Name",
        StringValue("Access time - Timeline - " + std::to_string(grpId)));
    groupAccessTimeTimeline->SetAttribute("Color", nextColor);
    groupAccessTimeTimeline->SetAttribute("Connection", StringValue("None"));
    groupAccessTimeTimeline->GetXAxis()->SetAttribute("Name", StringValue("Time (s)"));
    groupAccessTimeTimeline->GetYAxis()->SetAttribute("Name", StringValue("Access time (ms)"));
    groupAccessTimeTimeline->SetAttribute("Visible", BooleanValue(false));
    m_accessTimeTimelineCollection->Add(groupAccessTimeTimeline);

    // Timeline for M2E latency
    auto groupM2eLatencyTimeline = CreateObject<netsimulyzer::XYSeries>(m_orchestrator);
    groupM2eLatencyTimeline->SetAttribute(
        "Name",
        StringValue("M2E latency - Timeline - " + std::to_string(grpId)));
    groupM2eLatencyTimeline->SetAttribute("Color", nextColor);
    groupM2eLatencyTimeline->SetAttribute("Connection", StringValue("None"));
    groupM2eLatencyTimeline->GetXAxis()->SetAttribute("Name", StringValue("Time (s)"));
    groupM2eLatencyTimeline->GetYAxis()->SetAttribute("Name", StringValue("M2E latency (ms)"));
    groupM2eLatencyTimeline->SetAttribute("Visible", BooleanValue(false));
    m_m2eLatencyTimelineCollection->Add(groupM2eLatencyTimeline);

    m_grpIdToAccessTimeTimelineSeries.insert(
        std::pair<uint32_t, Ptr<netsimulyzer::XYSeries>>(grpId, groupAccessTimeTimeline));
    m_grpIdToAccessTimeEcdf.insert(
        std::pair<uint32_t, Ptr<netsimulyzer::EcdfSink>>(grpId, groupAccessTimeEcdf));
    m_grpIdToM2eLatencyTimelineSeries.insert(
        std::pair<uint32_t, Ptr<netsimulyzer::XYSeries>>(grpId, groupM2eLatencyTimeline));
    m_grpIdToM2eLatencyEcdf.insert(
        std::pair<uint32_t, Ptr<netsimulyzer::EcdfSink>>(grpId, groupM2eLatencyEcdfs));

    for (uint32_t i = 0; i < pttApps.GetN(); i++)
    {
        auto app = DynamicCast<McpttPttApp>(pttApps.Get(i));
        uint32_t userId = app->GetUserId();
        uint32_t nodeId = app->GetNode()->GetId();

        m_idsToAppPtr.insert(std::pair<uint32_t, Ptr<McpttPttApp>>(nodeId, app));
        m_idsToAppPtr.insert(std::pair<uint32_t, Ptr<McpttPttApp>>(userId, app));
    }

    // Keeping track of all added grpIds
    // Prevent user from reusing grpIds
    m_allGrpIds.insert(grpId);
    return 0;
}

void
NetSimMcpttPttBasicGroupCharter::AccessTimeTrace(Time t,
                                                 uint32_t userId,
                                                 uint16_t callId,
                                                 std::string result,
                                                 Time latency)
{
    if ((!m_isSetUp) || (result != "I" && result != "Q"))
    {
        return;
    }

    int grpId = GetGroupId(userId, callId);
    if (grpId != -1)
    {
        m_grpIdToAccessTimeTimelineSeries[grpId]->Append(Simulator::Now().GetSeconds(),
                                                         latency.GetMilliSeconds());
        m_grpIdToAccessTimeEcdf[grpId]->Append(latency.GetMilliSeconds());
    }
}

void
NetSimMcpttPttBasicGroupCharter::M2eLatencyTrace(Time t,
                                                 uint32_t ssrc,
                                                 uint64_t nodeId,
                                                 uint16_t callId,
                                                 Time latency)
{
    if (!m_isSetUp)
    {
        return;
    }

    int grpId = GetGroupId(nodeId, callId);
    if (grpId != -1)
    {
        m_grpIdToM2eLatencyTimelineSeries[grpId]->Append(Simulator::Now().GetSeconds(),
                                                         latency.GetMilliSeconds());
        m_grpIdToM2eLatencyEcdf[grpId]->Append(latency.GetMilliSeconds());
    }
}

int
NetSimMcpttPttBasicGroupCharter::GetGroupId(uint32_t id, uint16_t callId)
{
    auto appPtrIt = m_idsToAppPtr.find(id);
    if (appPtrIt == m_idsToAppPtr.end())
    {
        // Fetching grp id for a node whose app was not added through AddGroup
        // Trace sinks are linked for all nodes in simulation and so is an expected condition to be
        // ignored
        NS_LOG_WARN("Fetching grp id for unadded app with userId/nodeId " + std::to_string(id) +
                    "for call " + std::to_string(callId));
        return -1;
    }

    ObjectMapValue callMap;
    appPtrIt->second->GetAttribute("Calls", callMap);
    for (auto callIt = callMap.Begin(); callIt != callMap.End(); callIt++)
    {
        auto call = DynamicCast<McpttCall>(callIt->second);
        if (call->GetCallId() == callId)
        {
            auto callMachine = DynamicCast<McpttCallMachineGrp>(call->GetCallMachine());
            uint32_t grpId = callMachine->GetGrpId().GetGrpId();
            // Limit to groups that the user has added
            if (m_allGrpIds.contains(grpId))
            {
                return grpId;
            }

            // Fetching grp id for a node whose app was not added through AddGroup
            // Trace sinks are linked for all nodes in simulation and so is an expected condition to
            // be ignored
            NS_LOG_WARN("Fetching grp id for unadded app with userId/nodeId " + std::to_string(id) +
                        "for call " + std::to_string(callId));
            return -1;
        }
    }

    NS_FATAL_ERROR("Grp id not found for call " + std::to_string(+callId) + "for userId/nodeId " +
                   std::to_string(id));
}

} /* namespace psc */

} /* namespace ns3 */

#endif /* HAS_NETSIMULYZER */
