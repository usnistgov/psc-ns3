/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include <string>

#include <ns3/names.h>
#include <ns3/mcptt-call.h>
#include <ns3/mcptt-on-network-floor-arbitrator.h>
#include <ns3/mcptt-on-network-floor-dual-control.h>
#include <ns3/mcptt-on-network-floor-participant.h>
#include <ns3/mcptt-server-app.h>
#include <ns3/mcptt-on-network-floor-towards-participant.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-server-helper.h"

namespace ns3 {
namespace psc {

McpttServerHelper::McpttServerHelper (void)
{
  m_serverFactory.SetTypeId (McpttServerApp::GetTypeId ());
}

McpttServerHelper::~McpttServerHelper ()
{ }

Ptr<McpttServerApp>
McpttServerHelper::Install (Ptr<Node> node)
{
  return (InstallPriv (node));
}

Ptr<McpttServerApp>
McpttServerHelper::Install (const std::string& nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);

  return (InstallPriv (node));
}

void
McpttServerHelper::SetServerAttributes (
  std::string n0, const AttributeValue& v0,
  std::string n1, const AttributeValue& v1,
  std::string n2, const AttributeValue& v2,
  std::string n3, const AttributeValue& v3,
  std::string n4, const AttributeValue& v4,
  std::string n5, const AttributeValue& v5,
  std::string n6, const AttributeValue& v6,
  std::string n7, const AttributeValue& v7)
{
  m_serverFactory.Set (n0, v0);
  m_serverFactory.Set (n1, v1);
  m_serverFactory.Set (n2, v2);
  m_serverFactory.Set (n3, v3);
  m_serverFactory.Set (n4, v4);
  m_serverFactory.Set (n5, v5);
  m_serverFactory.Set (n6, v6);
  m_serverFactory.Set (n7, v7);
}

Ptr<McpttServerApp>
McpttServerHelper::InstallPriv (Ptr<Node> node)
{
  Ptr<McpttServerApp> app = m_serverFactory.Create<McpttServerApp> ();
  node->AddApplication (app);
  return app;
}

} // namespace psc
} // namespace ns3

