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
#include <ns3/mcptt-call-control-info.h>
#include <ns3/mcptt-on-network-floor-arbitrator.h>
#include <ns3/mcptt-on-network-floor-dual-control.h>
#include <ns3/mcptt-on-network-floor-participant.h>
#include <ns3/mcptt-server-app.h>
#include <ns3/mcptt-on-network-floor-towards-participant.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-on-network-floor-control-helper.h"

namespace ns3 {

McpttOnNetworkFloorControlHelper::McpttOnNetworkFloorControlHelper (void)
{ 
  m_appFac.SetTypeId (McpttServerApp::GetTypeId ());
  m_arbitratorFac.SetTypeId (McpttOnNetworkFloorArbitrator::GetTypeId ());
  m_dualControlFac.SetTypeId (McpttOnNetworkFloorDualControl::GetTypeId ());
  m_participantFac.SetTypeId (McpttOnNetworkFloorTowardsParticipant::GetTypeId ());
  m_callInfoFac.SetTypeId (McpttCallControlInfo::GetTypeId ());
}

McpttOnNetworkFloorControlHelper::~McpttOnNetworkFloorControlHelper ()
{ }

ApplicationContainer
McpttOnNetworkFloorControlHelper::Install (const Ptr<Node>& node)
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
McpttOnNetworkFloorControlHelper::Install (const std::string& nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
McpttOnNetworkFloorControlHelper::Install (const NodeContainer& c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

void
McpttOnNetworkFloorControlHelper::SetApp (std::string name,
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
McpttOnNetworkFloorControlHelper::SetArbitrator (std::string name,
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
  m_arbitratorFac = factory;
}

void
McpttOnNetworkFloorControlHelper::SetDualControl (std::string name,
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
  m_dualControlFac = factory;
}

void
McpttOnNetworkFloorControlHelper::SetParticipant (std::string name,
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
  m_participantFac = factory;
}

void
McpttOnNetworkFloorControlHelper::SetCallInfo (std::string name,
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
  m_callInfoFac = factory;
}

void
McpttOnNetworkFloorControlHelper::SetupFloorControl (const ApplicationContainer& server, const ApplicationContainer& clients)
{
  Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp, Application> (server.Get (0));
  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = serverApp->GetArbitrator ();

  for (uint32_t pidx = 0; pidx < clients.GetN (); pidx++)
    {
      BooleanValue mcImplicitRequest;
      uint32_t callPort = McpttPttApp::AllocateNextPortNumber ();
      uint32_t floorPort = McpttPttApp::AllocateNextPortNumber ();
      uint32_t mediaPort = McpttPttApp::AllocateNextPortNumber ();
      Ptr<McpttPttApp> clientApp = DynamicCast<McpttPttApp, Application> (clients.Get (pidx));
      Ptr<McpttOnNetworkFloorTowardsParticipant> participant = m_participantFac.Create<McpttOnNetworkFloorTowardsParticipant> ();

      Ptr<McpttCall> selectedCall = clientApp->GetSelectedCall ();
      Ptr<McpttCallMachineNull> clientCallControl = DynamicCast<McpttCallMachineNull, McpttCallMachine> (selectedCall->GetCallMachine ());
      Ptr<McpttOnNetworkFloorParticipant> clientFloorControl = DynamicCast<McpttOnNetworkFloorParticipant, McpttFloorParticipant> (selectedCall->GetFloorMachine ());

      clientFloorControl->GetAttribute ("McImplicitRequest", mcImplicitRequest);

      clientApp->SetAttribute ("PeerAddress", AddressValue (serverApp->GetLocalAddress ()));
      clientApp->SetAttribute ("CallPort", UintegerValue (callPort));
      clientCallControl->SetAttribute ("FloorPort", UintegerValue (floorPort));
      clientCallControl->SetAttribute ("MediaPort", UintegerValue (mediaPort));
      participant->SetAttribute ("PeerAddress", AddressValue (clientApp->GetLocalAddress ()));
      participant->SetAttribute ("FloorPort", UintegerValue (floorPort));
      participant->SetAttribute ("MediaPort", UintegerValue (mediaPort));
      participant->SetAttribute ("McImplicitRequest", mcImplicitRequest);
      participant->SetOriginator (clientFloorControl->IsOriginator ());
      participant->SetStoredSsrc (clientFloorControl->GetTxSsrc ());
      participant->SetStoredPriority (clientFloorControl->GetPriority ());

      arbitrator->AddParticipant (participant);
    }
}

Ptr<Application>
McpttOnNetworkFloorControlHelper::InstallPriv (const Ptr<Node>& node)
{
  Ptr<McpttServerApp> app = m_appFac.Create<McpttServerApp> ();
  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = m_arbitratorFac.Create<McpttOnNetworkFloorArbitrator> ();
  Ptr<McpttOnNetworkFloorDualControl> dualControl = m_dualControlFac.Create<McpttOnNetworkFloorDualControl> ();
  Ptr<McpttCallControlInfo> callInfo = m_callInfoFac.Create<McpttCallControlInfo> ();

  app->SetArbitrator (arbitrator);
  arbitrator->SetCallInfo (callInfo);
  arbitrator->SetDualControl (dualControl);

  node->AddApplication (app);

  return app;
}

} // namespace ns3

