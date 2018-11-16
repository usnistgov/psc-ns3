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
  LogComponentEnable ("McpttCallMsg", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallMsgField", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachine", LOG_LEVEL_ALL);
  LogComponentEnable ("McpttCallTypeMachineGrpBasic", LOG_LEVEL_ALL);
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
  LogComponentEnable ("McpttInfo", LOG_LEVEL_ALL);
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
  : m_appFac (ObjectFactory ()),
    m_pusherFac (ObjectFactory ()),
    m_mediaSrcFac (ObjectFactory ()),
    m_msgTracer (0),
    m_stateMachineTracer (0)
{ 
  m_appFac.SetTypeId (McpttPttApp::GetTypeId ());
  m_pusherFac.SetTypeId (McpttPusher::GetTypeId ());
  m_mediaSrcFac.SetTypeId (McpttMediaSrc::GetTypeId ());

  m_pusherFac.Set ("Rnd", PointerValue (CreateObjectWithAttributes<NormalRandomVariable> ("Mean", DoubleValue(5.0), "Variance", DoubleValue (2.0))));
  m_mediaSrcFac.Set ("Bytes", UintegerValue (1));
  m_mediaSrcFac.Set ("DataRate", DataRateValue (DataRate ("8b/s")));
}

McpttHelper::~McpttHelper ()
{ }

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
McpttHelper::SetAppAttribute (const std::string& name, const AttributeValue& value)
{
  m_appFac.Set (name, value);
}

void
McpttHelper::SetAppTid (const TypeId& appTid)
{
  TypeId pttAppTid = McpttPttApp::GetTypeId ();

  NS_ASSERT_MSG ((appTid == pttAppTid || appTid.IsChildOf (pttAppTid)), "The given type ID is not that of a " << pttAppTid.GetName () << ".");

  m_appFac.SetTypeId (appTid);
}

void
McpttHelper::SetPusherAttribute (const std::string& name, const AttributeValue& value)
{
  m_pusherFac.Set (name, value);
}

void
McpttHelper::SetPusherTid (const TypeId& pusherTid)
{
  TypeId mcpttPusherTid = McpttPusher::GetTypeId ();

  NS_ASSERT_MSG ((pusherTid == mcpttPusherTid || pusherTid.IsChildOf (mcpttPusherTid)), "The given type ID is not that of a " << mcpttPusherTid.GetName () << ".");

  m_pusherFac.SetTypeId (pusherTid);
}

void
McpttHelper::SetMediaSrcAttribute (const std::string& name, const AttributeValue& value)
{
  m_mediaSrcFac.Set (name, value);
}

void
McpttHelper::SetMediaSrcTid (const TypeId& requesterTid)
{
  TypeId mcpttMediaSrcTid = McpttMediaSrc::GetTypeId ();

  NS_ASSERT_MSG ((mcpttMediaSrcTid == requesterTid || requesterTid.IsChildOf (mcpttMediaSrcTid)), "The given type ID is not that of a " << mcpttMediaSrcTid.GetName () << ".");

  m_mediaSrcFac.SetTypeId (requesterTid);
}

Ptr<Application>
McpttHelper::InstallPriv (const Ptr<Node>& node)
{
  uint32_t userId = McpttHelper::GetNextUserId ();

  Ptr<McpttPttApp> app = m_appFac.Create<McpttPttApp> ();
  Ptr<McpttMediaSrc> requester = m_mediaSrcFac.Create<McpttMediaSrc> ();
  Ptr<McpttPusher> pusher = m_pusherFac.Create<McpttPusher> ();

  app->SetUserId (userId);
  app->SetMediaSrc (requester);
  app->SetPusher (pusher);

  node->AddApplication (app);

  return app;
}

} // namespace ns3

