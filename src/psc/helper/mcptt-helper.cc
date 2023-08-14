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

#include <stdint.h>
#include <string>

#include <ns3/callback.h>
#include <ns3/config.h>
#include <ns3/names.h>
#include <ns3/log.h>
#include <ns3/mcptt-media-src.h>
#include <ns3/mcptt-media-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-pusher.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/string.h>
#include "mcptt-msg-stats.h"
#include "mcptt-state-machine-stats.h"
#include "mcptt-helper.h"

namespace ns3 {
namespace psc {

NS_LOG_COMPONENT_DEFINE ("McpttHelper");

void
McpttHelper::EnableLogComponents (void)
{
  LogComponentEnableAll (LOG_PREFIX_TIME);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnable ("ImsHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCall", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachineGrpBasic", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachineGrpBasicState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachineGrpBroadcast", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachineGrpBroadcastState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachinePrivate", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMachinePrivateState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMsgField", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachineGrpBasic", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachinePrivate", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachinePrivateState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttChannel", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCounter", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttEmergAlertMachineBasic", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttEmergAlertMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOffNetworkFloorParticipant", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOffNetworkFloorParticipantState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorParticipant", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMsgField", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorQueue", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMediaMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMediaSrc", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMsgStats", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttPttApp", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttPusher", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttQueuedUserInfo", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttRtpHeader", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttStateMachineStats", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttTimer", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttTraceHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttServerApp", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttServerCall", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttServerCallMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorParticipant", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorParticipant", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorParticipantState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorArbitrator", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorArbitratorState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorDualControl", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorDualControlState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorTowardsParticipant", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkFloorTowardsParticipantState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkCallMachineClient", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttOnNetworkCallMachineClientState", LOG_LEVEL_ALL);
}

uint32_t
McpttHelper::GetNextUserId (void)
{
  static uint32_t s_nextUserId = 0;

  s_nextUserId += 1;

  return s_nextUserId;
}

McpttHelper::McpttHelper (void)
  : m_pushConfigured (false),
    m_releaseConfigured (false)
{
  m_appFac.SetTypeId (McpttPttApp::GetTypeId ());
  m_pusherFac.SetTypeId (McpttPusher::GetTypeId ());
  m_mediaSrcFac.SetTypeId (McpttMediaSrc::GetTypeId ());
}

McpttHelper::~McpttHelper ()
{}

void
McpttHelper::AddPushersToOrchestrator (Ptr<McpttPusherOrchestratorInterface> orchestrator, const ApplicationContainer& apps)
{
  for (uint32_t i = 0; i < apps.GetN (); i++)
    {
      orchestrator->AddPusher (apps.Get (i)->GetObject<McpttPttApp> ()->GetPusher ());
    }
}

ApplicationContainer
McpttHelper::Install (const Ptr<Node>& node)
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
McpttHelper::Install (const std::string& nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);

  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
McpttHelper::Install (const NodeContainer& c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

void
McpttHelper::SetPttApp (std::string name,
                        std::string n0, const AttributeValue& v0,
                        std::string n1, const AttributeValue& v1,
                        std::string n2, const AttributeValue& v2,
                        std::string n3, const AttributeValue& v3,
                        std::string n4, const AttributeValue& v4,
                        std::string n5, const AttributeValue& v5,
                        std::string n6, const AttributeValue& v6,
                        std::string n7, const AttributeValue& v7)
{
  ObjectFactory factory;
  factory.SetTypeId (name);
  factory.Set (n0, v0);
  factory.Set (n1, v1);
  factory.Set (n2, v2);
  factory.Set (n3, v3);
  factory.Set (n4, v4);
  factory.Set (n5, v5);
  factory.Set (n6, v6);
  factory.Set (n7, v7);
  m_appFac = factory;
}

void
McpttHelper::SetMediaSrc (std::string name,
                          std::string n0, const AttributeValue& v0,
                          std::string n1, const AttributeValue& v1,
                          std::string n2, const AttributeValue& v2,
                          std::string n3, const AttributeValue& v3,
                          std::string n4, const AttributeValue& v4,
                          std::string n5, const AttributeValue& v5,
                          std::string n6, const AttributeValue& v6,
                          std::string n7, const AttributeValue& v7)
{
  ObjectFactory factory;
  factory.SetTypeId (name);
  factory.Set (n0, v0);
  factory.Set (n1, v1);
  factory.Set (n2, v2);
  factory.Set (n3, v3);
  factory.Set (n4, v4);
  factory.Set (n5, v5);
  factory.Set (n6, v6);
  factory.Set (n7, v7);
  m_mediaSrcFac = factory;
}

void
McpttHelper::SetPusher (std::string name,
                        std::string n0, const AttributeValue& v0,
                        std::string n1, const AttributeValue& v1,
                        std::string n2, const AttributeValue& v2,
                        std::string n3, const AttributeValue& v3,
                        std::string n4, const AttributeValue& v4,
                        std::string n5, const AttributeValue& v5,
                        std::string n6, const AttributeValue& v6,
                        std::string n7, const AttributeValue& v7)
{
  ObjectFactory factory;
  factory.SetTypeId (name);
  factory.Set (n0, v0);
  factory.Set (n1, v1);
  factory.Set (n2, v2);
  factory.Set (n3, v3);
  factory.Set (n4, v4);
  factory.Set (n5, v5);
  factory.Set (n6, v6);
  factory.Set (n7, v7);
  m_pusherFac = factory;
}

void
McpttHelper::SetPusherPttInterarrivalTimeVariable (std::string name,
                                                   std::string n0, const AttributeValue& v0,
                                                   std::string n1, const AttributeValue& v1,
                                                   std::string n2, const AttributeValue& v2,
                                                   std::string n3, const AttributeValue& v3,
                                                   std::string n4, const AttributeValue& v4,
                                                   std::string n5, const AttributeValue& v5,
                                                   std::string n6, const AttributeValue& v6,
                                                   std::string n7, const AttributeValue& v7)
{
  m_pushConfigured = true;
  ObjectFactory factory;
  factory.SetTypeId (name);
  factory.Set (n0, v0);
  factory.Set (n1, v1);
  factory.Set (n2, v2);
  factory.Set (n3, v3);
  factory.Set (n4, v4);
  factory.Set (n5, v5);
  factory.Set (n6, v6);
  factory.Set (n7, v7);
  m_pusherPushFac = factory;
}

void
McpttHelper::SetPusherPttDurationVariable (std::string name,
                                           std::string n0, const AttributeValue& v0,
                                           std::string n1, const AttributeValue& v1,
                                           std::string n2, const AttributeValue& v2,
                                           std::string n3, const AttributeValue& v3,
                                           std::string n4, const AttributeValue& v4,
                                           std::string n5, const AttributeValue& v5,
                                           std::string n6, const AttributeValue& v6,
                                           std::string n7, const AttributeValue& v7)
{
  m_releaseConfigured = true;
  ObjectFactory factory;
  factory.SetTypeId (name);
  factory.Set (n0, v0);
  factory.Set (n1, v1);
  factory.Set (n2, v2);
  factory.Set (n3, v3);
  factory.Set (n4, v4);
  factory.Set (n5, v5);
  factory.Set (n6, v6);
  factory.Set (n7, v7);
  m_pusherReleaseFac = factory;
}

Ptr<Application>
McpttHelper::InstallPriv (const Ptr<Node>& node)
{
  uint32_t userId = McpttHelper::GetNextUserId ();

  Ptr<McpttPttApp> app = m_appFac.Create<McpttPttApp> ();
  Ptr<McpttMediaSrc> requester = m_mediaSrcFac.Create<McpttMediaSrc> ();
  Ptr<McpttPusher> pusher = m_pusherFac.Create<McpttPusher> ();

  if (m_pushConfigured == true)
    {
      pusher->SetAttribute ("PttInterarrivalTimeVariable", PointerValue (m_pusherPushFac.Create<RandomVariableStream> ()));
    }

  if (m_releaseConfigured == true)
    {
      pusher->SetAttribute ("PttDurationVariable", PointerValue (m_pusherReleaseFac.Create<RandomVariableStream> ()));
    }

  app->SetUserId (userId);
  app->SetMediaSrc (requester);
  app->SetPusher (pusher);

  node->AddApplication (app);

  return app;
}

} // namespace psc
} // namespace ns3

