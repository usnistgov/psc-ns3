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

#ifndef NETSIM_MCPTT_BASIC_GROUP_VISUALIZER
#define NETSIM_MCPTT_BASIC_GROUP_VISUALIZER

#ifdef HAS_NETSIMULYZER

#include <ns3/application-container.h>
#include <ns3/color.h>
#include <ns3/orchestrator.h>
#include <ns3/packet.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

using namespace std;

/*
 * Mcptt visualizer that highlights user when transmitting/taken the floor
 * Currently does not support different highlight colors per group
 * Multiple groups can be supported by either making an object per group or just passing in all apps
 * in setup (TODO: change node to color map to use node Id, group Id pairs as keys and change
 * m_highlightColor to map of grpIds to highlight color if want better support for group-specific
 * colors)
 *
 * Usage: Only need to call EnableVisualizations
 */
class NetSimMcpttPttBasicGroupVisualizer
{
  public:
    NetSimMcpttPttBasicGroupVisualizer() = default;

    /*
     * Method to enable netsimulyzer visualization features
     * \param pttApps the apps to add visualization to
     * \param highlightColor the color to highlight nodes that have taken floor/transmitting
     * messages
     * \return 1 on successful setup, 0 (immediately) on recall after successfull setup,
     * -1 on unsuccessful setup (orchestrator = nullptr | #pttApps = 0)
     *
     * ASSUMPTION: pttApps does not include the server node
     */
    int EnableVisualizations(Ptr<netsimulyzer::Orchestrator> orchestrator,
                             ApplicationContainer pttApps,
                             netsimulyzer::Color3 highlightColor = netsimulyzer::RED);

  private:
    /*
     * Method to link to rx/tx traces of the mcptt-ptt app to keep track of mcptt msgs
     * Msgs used to toggle highlighting of nodes
     */
    void LinkTraces(ApplicationContainer pttApps);

    /*
     * Trace sink for McptPttApp tx trace to toggle logical links
     * McpttPttApp have same callback signature for Tx and Rx (first parameter used to
     * differentiate)
     */
    void McpttPttAppTxRxTraceCallback(bool isTx,
                                      Ptr<const Application> app,
                                      uint16_t callId,
                                      Ptr<const Packet> packet,
                                      const TypeId& headerType);

    /*
     * Boolean flag to denote successful setup
     */
    bool m_isSetUp = false;
    /*
     * Orchestrator used to create logical links
     */
    Ptr<netsimulyzer::Orchestrator> m_orchestrator;
    /*
     * Maps a node Id to its original highlight color
     */
    std::map<uint32_t, netsimulyzer::Color3> m_nodeToColor;
    /*
     * Highlight color to denote node as transmitting/taken floor
     */
    netsimulyzer::Color3 m_highlightColor;
};

} /* namespace psc */

} /* namespace ns3 */

#endif /* HAS_NETSIMULYZER */

#endif /* NETSIM_MCPTT_BASIC_GROUP_VISUALIZER */
