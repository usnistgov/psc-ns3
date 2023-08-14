/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2019 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/application-container.h"
#include "ns3/nstime.h"
#include "ns3/address.h"
#include "ns3/log.h"
#include "ns3/object-factory.h"
#include "ns3/mcptt-call-helper.h"
#include "ns3/mcptt-call-msg-field.h"
#include "ns3/mcptt-server-app.h"
#include "ns3/mcptt-call.h"
#include "ns3/mcptt-server-call.h"
#include "ns3/mcptt-ptt-app.h"
#include "ns3/mcptt-channel.h"
#include "ns3/mcptt-server-call-machine.h"
#include "ns3/mcptt-server-call-machine-group-prearranged.h"
#include "ns3/mcptt-on-network-call-machine-client.h"
#include "ns3/mcptt-on-network-floor-participant.h"
#include "ns3/mcptt-on-network-floor-towards-participant.h"
#include "ns3/mcptt-on-network-floor-arbitrator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallHelper");

namespace psc {

McpttCallHelper::McpttCallHelper (void)
{
  m_arbitratorFactory.SetTypeId (McpttOnNetworkFloorArbitrator::GetTypeId ());
  m_towardsParticipantFactory.SetTypeId (McpttOnNetworkFloorTowardsParticipant::GetTypeId ());
  m_participantFactory.SetTypeId (McpttOnNetworkFloorParticipant::GetTypeId ());
  m_serverCallFactory.SetTypeId (McpttServerCall::GetTypeId ());
}

McpttCallHelper::~McpttCallHelper ()
{}

void
McpttCallHelper::SetArbitrator (std::string name,
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
  m_arbitratorFactory = factory;
}

void
McpttCallHelper::SetTowardsParticipant (std::string name,
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
  m_towardsParticipantFactory = factory;
}

void
McpttCallHelper::SetParticipant (std::string name,
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
  m_participantFactory = factory;
}

void
McpttCallHelper::SetServerCall (std::string name,
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
  m_serverCallFactory = factory;
}

uint16_t
McpttCallHelper::AddCall (ApplicationContainer clients, Ptr<McpttServerApp> server,
                          uint32_t groupId, McpttCallMsgFieldCallType callType, Time startTime, Time stopTime)
{
  NS_LOG_FUNCTION (this << clients.GetN () << server->GetNode ()->GetId ()
                        << groupId << +callType.GetType () << startTime.GetSeconds () << stopTime.GetSeconds ());
  // callType not presently used
  uint16_t callId = server->AllocateCallId ();
  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = m_arbitratorFactory.Create<McpttOnNetworkFloorArbitrator> ();
  NS_LOG_DEBUG ("Creating call with callID " << callId);
  std::vector<uint32_t> clientUserIds;

  for (uint32_t i = 0; i < clients.GetN (); i++)
    {
      Ptr<McpttPttApp> app = clients.Get (i)->GetObject<McpttPttApp> ();
      clientUserIds.push_back (app->GetUserId ());
      // McpttPttApp uses a static integer for allocating unique port numbers
      uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
      uint16_t mediaPort = McpttPttApp::AllocateNextPortNumber ();
      NS_LOG_DEBUG ("Port from " << app->GetNode ()->GetId ()
                                 << " to server:  floor " << floorPort
                                 << " media " << mediaPort);
      // Each application gets its own instance of a McpttCall object
      Ptr<McpttCall> call = CreateObject<McpttCall> (McpttCall::NetworkCallType::ON_NETWORK);
      call->SetAttribute ("PeerAddress", AddressValue (server->GetLocalAddress ()));
      call->SetStartTime (startTime);
      call->SetStopTime (stopTime);
      // XXX TODO: configure call machine type from call type
      Ptr<McpttCallMachine> callMachine =
        CreateObjectWithAttributes<McpttOnNetworkCallMachineClient> ("GroupId", UintegerValue (groupId));
      call->SetCallMachine (callMachine);
      call->SetCallId (callId);
      Ptr<McpttChannel> floorChannel = CreateObject<McpttChannel> ();
      call->SetFloorChannel (floorChannel);
      call->GetCallMachine ()->SetAttribute ("FloorPort", UintegerValue (floorPort));
      Ptr<McpttChannel> mediaChannel = CreateObject<McpttChannel> ();
      call->SetMediaChannel (mediaChannel);
      call->GetCallMachine ()->SetAttribute ("MediaPort", UintegerValue (mediaPort));
      Ptr<McpttOnNetworkFloorParticipant> clientFloorControl =
        m_participantFactory.Create<McpttOnNetworkFloorParticipant> ();
      call->SetFloorMachine (clientFloorControl);
      call->SetPushOnSelect (true);
      app->AddCall (call);

      Ptr<McpttOnNetworkFloorTowardsParticipant> serverTowardsParticipant =
        m_towardsParticipantFactory.Create<McpttOnNetworkFloorTowardsParticipant> ();
      serverTowardsParticipant->SetPeerAddress (app->GetLocalAddress ());
      serverTowardsParticipant->SetPeerUserId (app->GetUserId ());
      serverTowardsParticipant->SetFloorPort (floorPort);
      serverTowardsParticipant->SetMediaPort (mediaPort);
      serverTowardsParticipant->SetOriginator (clientFloorControl->IsOriginator ());
      serverTowardsParticipant->SetStoredSsrc (clientFloorControl->GetTxSsrc ());
      serverTowardsParticipant->SetStoredPriority (clientFloorControl->GetPriority ());
      arbitrator->AddParticipant (serverTowardsParticipant);
    }
  Ptr<McpttOnNetworkFloorDualControl> dualControl = CreateObject<McpttOnNetworkFloorDualControl> ();
  // XXX TODO Introduce another factory pattern for dual control
  arbitrator->SetDualControl (dualControl);
  // Each server application gets an instance of a McpttServerCall object
  Ptr<McpttServerCall> call = m_serverCallFactory.Create<McpttServerCall> ();
  // XXX TODO configure call machine type from call type
  Ptr<McpttServerCallMachine> callMachine =
    CreateObjectWithAttributes<McpttServerCallMachineGroupPrearranged> ("GroupId", UintegerValue (groupId));
  call->SetCallMachine (callMachine);
  call->SetCallId (callId);
  call->SetClientUserIds (clientUserIds);
  arbitrator->SetOwner (call);
  call->SetArbitrator (arbitrator);

  server->AddCall (call);
  return callId;
}

void
McpttCallHelper::AddCallOffNetwork (ApplicationContainer clients, uint16_t callId, Address peerAddress, uint32_t groupId, McpttCallMsgFieldCallType callType, Time startTime, Time stopTime)
{
  NS_LOG_FUNCTION (this << clients.GetN () << +callId << peerAddress
                        << groupId << +callType.GetType () << startTime.GetSeconds () << stopTime.GetSeconds ());

  ObjectFactory callFac;

  ObjectFactory floorFac;

  if (callType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      callFac.SetTypeId ("ns3::psc::McpttCallMachineGrpBasic");
      floorFac.SetTypeId ("ns3::psc::McpttOffNetworkFloorParticipant");
    }
  else
    {
      NS_ABORT_MSG ("Call type not supported.");
    }


  for (uint32_t idx = 0; idx < clients.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp> (clients.Get (idx));

      Ptr<McpttCall> call = CreateObject<McpttCall> (McpttCall::NetworkCallType::OFF_NETWORK);
      Ptr<McpttChannel> floorChannel = CreateObject<McpttChannel> ();
      Ptr<McpttChannel> mediaChannel = CreateObject<McpttChannel> ();
      Ptr<McpttCallMachine> callMachine = callFac.Create<McpttCallMachine> ();
      Ptr<McpttFloorParticipant> floorMachine = floorFac.Create<McpttFloorParticipant> ();

      callMachine->SetAttribute ("GroupId", UintegerValue (groupId));

      call->SetAttribute ("PeerAddress", AddressValue (peerAddress));
      call->SetCallMachine (callMachine);
      call->SetFloorChannel (floorChannel);
      call->SetFloorMachine (floorMachine);
      call->SetMediaChannel (mediaChannel);
      call->SetCallId (callId);
      call->SetStartTime (startTime);
      call->SetStopTime (stopTime);
      call->SetPushOnSelect (true);

      pttApp->AddCall (call);
    }
}

void
McpttCallHelper::ConfigureOffNetworkBasicGrpCall (ApplicationContainer& apps, Address peerAddress, uint32_t usersPerGroup, uint32_t baseGroupId)
{
  uint32_t groupId = baseGroupId;

  ObjectFactory callFac;
  callFac.SetTypeId ("ns3::psc::McpttCallMachineGrpBasic");

  ObjectFactory floorFac;
  floorFac.SetTypeId ("ns3::psc::McpttOffNetworkFloorParticipant");

  for (uint32_t idx = 0; idx < apps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp> (apps.Get (idx));

      callFac.Set ("GroupId", UintegerValue (groupId));

      Ptr<McpttCall> call = pttApp->CreateCall (callFac, floorFac, McpttCall::NetworkCallType::OFF_NETWORK);
      call->SetAttribute ("PeerAddress", AddressValue (peerAddress));
      pttApp->SelectCall (call->GetCallId ());

      if ((idx + 1) % usersPerGroup == 0)
        {
          groupId += 1;
        }
    }
}

} // namespace psc
} // namespace ns3

