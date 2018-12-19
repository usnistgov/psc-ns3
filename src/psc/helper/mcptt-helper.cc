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

#include <ns3/address.h>
#include <ns3/callback.h>
#include <ns3/config.h>
#include <ns3/data-rate.h>
#include <ns3/double.h>
#include <ns3/names.h>
#include <ns3/log.h>
#include <ns3/mcptt-floor-machine.h>
#include <ns3/mcptt-media-src.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-pusher.h>
#include <ns3/packet-socket-address.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/string.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/uinteger.h>

#include "mcptt-msg-stats.h"
#include "mcptt-state-machine-stats.h"

#include "mcptt-helper.h"


namespace ns3 {

void
McpttHelper::EnableLogComponents (void)
{
  LogComponentEnableAll (LOG_PREFIX_TIME);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnable ("McpttCall", LOG_LEVEL_ALL);
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
  LogComponentEnable ("McpttChan", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCounter", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttEmergAlertMachineBasic", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttEmergAlertMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMachineBasic", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMachineBasicState", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorMsgField", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttFloorQueue", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMediaMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMediaSrc", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttPttApp", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttPusher", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttQueuedUserInfo", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttRtpHeader", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttTimer", LOG_LEVEL_ALL);
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
    m_releaseConfigured (false),
    m_msgTracer (0),
    m_stateMachineTracer (0)
{ 
  m_appFac.SetTypeId (McpttPttApp::GetTypeId ());
  m_pusherFac.SetTypeId (McpttPusher::GetTypeId ());
  m_mediaSrcFac.SetTypeId (McpttMediaSrc::GetTypeId ());
}

McpttHelper::~McpttHelper ()
{ }

void
McpttHelper::ConfigureBasicGrpCall (ApplicationContainer& apps, uint32_t usersPerGroup, uint32_t baseGroupId)
{
  uint32_t groupId = baseGroupId;

  ObjectFactory callFac;
  callFac.SetTypeId ("ns3::McpttCallMachineGrpBasic");

  ObjectFactory floorFac;
  floorFac.SetTypeId ("ns3::McpttFloorMachineBasic");

  for (uint32_t idx = 0; idx < apps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp, Application> (apps.Get (idx));

      callFac.Set ("GroupId", UintegerValue (groupId));

      pttApp->CreateCall (callFac, floorFac);
      pttApp->SelectLastCall ();
 
      if ((idx + 1) % usersPerGroup == 0)
        {
          groupId += 1;
        }
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
McpttHelper::EnableMsgTraces (void)
{
  if (m_msgTracer == 0)
    {
      m_msgTracer = CreateObject<McpttMsgStats> ();
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
    }
}

void
McpttHelper::EnableStateMachineTraces (void)
{
  if (m_stateMachineTracer == 0)
    {
      m_stateMachineTracer = CreateObject<McpttStateMachineStats> ();
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/CallTypeMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/EmergAlertMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
    }
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
McpttHelper::SetPusherPushVariable (std::string name,
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
McpttHelper::SetPusherReleaseVariable (std::string name,
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
      pusher->SetAttribute ("PushVariable", PointerValue (m_pusherPushFac.Create<RandomVariableStream> ()));
    }

  if (m_releaseConfigured == true)
    {
      pusher->SetAttribute ("ReleaseVariable", PointerValue (m_pusherReleaseFac.Create<RandomVariableStream> ()));
    }

  app->SetUserId (userId);
  app->SetMediaSrc (requester);
  app->SetPusher (pusher);

  node->AddApplication (app);

  return app;
}

} // namespace ns3

