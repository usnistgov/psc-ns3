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

#include "netsim-mcptt-basic-group-visualizer.h"

#include "ns3/mcptt-floor-msg.h"
#include "ns3/mcptt-media-msg.h"
#include "ns3/mcptt-ptt-app.h"

#include <ns3/node-configuration.h>

namespace ns3
{

namespace psc
{

NS_LOG_COMPONENT_DEFINE("NetSimMcpttPttBasicGroupVisualizer");

using namespace std;

int
NetSimMcpttPttBasicGroupVisualizer::EnableVisualizations(
    Ptr<netsimulyzer::Orchestrator> orchestrator,
    ApplicationContainer pttApps,
    netsimulyzer::Color3 highlightColor)
{
    if (m_isSetUp)
    {
        return 0;
    }

    bool areParamsWrong = (!orchestrator || !pttApps.GetN());
    NS_ABORT_MSG_IF(areParamsWrong, "orchestrator null or #apps = 0");

    m_orchestrator = orchestrator;
    m_highlightColor = highlightColor;
    LinkTraces(pttApps);
    m_isSetUp = true;
    return 1;
}

void
NetSimMcpttPttBasicGroupVisualizer::LinkTraces(ApplicationContainer pttApps)
{
    for (uint32_t i = 0; i < pttApps.GetN(); i++)
    {
        auto node = pttApps.Get(i)->GetNode();
        auto nodeConfig = node->GetObject<netsimulyzer::NodeConfiguration>();

        m_nodeToColor.insert(
            std::pair<uint32_t, netsimulyzer::Color3>{node->GetId(),
                                                      nodeConfig->GetHighlightColor().value()});

        Ptr<McpttPttApp> app = DynamicCast<McpttPttApp>(pttApps.Get(i));
        app->TraceConnectWithoutContext(
            "TxTrace",
            MakeCallback(&NetSimMcpttPttBasicGroupVisualizer::McpttPttAppTxRxTraceCallback,
                         this,
                         true));
    }
}

void
NetSimMcpttPttBasicGroupVisualizer::McpttPttAppTxRxTraceCallback(bool isTx,
                                                                 Ptr<const Application> app,
                                                                 uint16_t callId,
                                                                 Ptr<const Packet> packet,
                                                                 const TypeId& headerType)
{
    if (!isTx)
    {
        return;
    }

    auto node = app->GetNode();
    auto nodeConfig = node->GetObject<netsimulyzer::NodeConfiguration>();
    auto it = m_nodeToColor.find(node->GetId());
    NS_ABORT_MSG_IF((it == m_nodeToColor.end()),
                    "Node with id" + std::to_string(node->GetId()) +
                        "not found. Possibly due to unadded app during setup.");

    // TODO: Support additional cases (e.g.: floor revoke)
    // Node has relinquished the floor - highlight in original color
    if ((headerType.IsChildOf(McpttFloorMsg::GetTypeId())) &&
        (headerType.GetName().find("McpttFloorMsgRelease") != std::string::npos))
    {
        nodeConfig->SetHighlightColor(it->second);
    }
    // Node took the floor/started transmitting messages - turn on highlight
    else if ((headerType == McpttMediaMsg::GetTypeId()) ||
             ((headerType.IsChildOf(McpttFloorMsg::GetTypeId())) &&
              (headerType.GetName().find("McpttFloorMsgTaken") != std::string::npos)))
    {
        nodeConfig->SetHighlightColor(m_highlightColor);
    }
}

} /* namespace psc */

} /* namespace ns3 */

#endif /* HAS_NETSIMULYZER */
