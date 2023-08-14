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

#include <ns3/config.h>
#include <ns3/application-container.h>
#include <ns3/core-module.h>
#include <ns3/config-store.h>
#include <ns3/csma-module.h>
#include <ns3/internet-module.h>
#include <ns3/mcptt-helper.h>
#include <ns3/mcptt-call-machine.h>
#include <ns3/mcptt-floor-participant.h>
#include <ns3/mobility-helper.h>
#include <ns3/network-module.h>

#include "mcptt-test-case-config.h"
#include "mcptt-test-call.h"

namespace ns3 {
namespace psc {
namespace tests {

McpttTestCaseConfig::McpttTestCaseConfig (void)
  : m_appCount (2),
    m_appTid (McpttPttApp::GetTypeId ()),
    m_callTid (McpttCallMachineNull::GetTypeId ()),
    m_floorTid (McpttFloorParticipantNull::GetTypeId ()),
    m_start (Seconds (2)),
    m_stop (Seconds (20))
{ }

McpttTestCaseConfig::McpttTestCaseConfig (const uint32_t& appCount, const TypeId& appTid, const TypeId& callTid, const TypeId& floorTid, const Time& start, const Time& stop)
  : m_appCount (appCount),
    m_appTid (appTid),
    m_callTid (callTid),
    m_floorTid (floorTid),
    m_start (start),
    m_stop (stop)
{ }

McpttTestCaseConfig::~McpttTestCaseConfig (void)
{ }

ApplicationContainer
McpttTestCaseConfig::Configure (void)
{
  uint32_t appCount = GetAppCount ();
  TypeId appTid = GetAppTid ();
  TypeId callTid = GetCallTid ();
  TypeId floorTid = GetFloorTid ();
  Time start = GetStart ();
  Time stop = GetStop ();

  Config::Reset ();

  NodeContainer nodes;
  nodes.Create (appCount);


  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (5.0),
                                 "GridWidth", UintegerValue (2),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (Seconds (0.05193)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  ApplicationContainer clientApps;
  McpttHelper mcpttHelper;
  mcpttHelper.SetPttApp (appTid.GetName ());

  clientApps.Add (mcpttHelper.Install (nodes));

  ObjectFactory callFac;
  callFac.SetTypeId (callTid);

  ObjectFactory floorFac;
  floorFac.SetTypeId (floorTid);

  uint16_t callId = 0;
  for (uint32_t idx = 0; idx < clientApps.GetN (); idx++)
    {
      Ptr<McpttPttApp> app = DynamicCast<McpttPttApp, Application> (clientApps.Get (idx));
      Ptr<McpttTestCall> call = CreateTestCall (callFac, floorFac, callId);
      app->AddCall (call);
      app->SelectCall (0);
      app->GetSelectedCall ()->SetAttribute ("PeerAddress", AddressValue (Ipv4Address ("255.255.255.255")));
    }

  clientApps.Start (start);
  clientApps.Stop (stop);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  return clientApps;
}

uint32_t
McpttTestCaseConfig::GetAppCount (void) const
{
  return m_appCount;
}

TypeId
McpttTestCaseConfig::GetAppTid (void) const
{
  return m_appTid;
}

TypeId
McpttTestCaseConfig::GetCallTid (void) const
{
  return m_callTid;
}

TypeId
McpttTestCaseConfig::GetFloorTid (void) const
{
  return m_floorTid;
}

Time
McpttTestCaseConfig::GetStart (void) const
{
  return m_start;
}

Time
McpttTestCaseConfig::GetStop (void) const
{
  return m_stop;
}

void
McpttTestCaseConfig::SetAppCount (const uint32_t& appCount)
{
  m_appCount = appCount;
}

void
McpttTestCaseConfig::SetAppTid (const TypeId& appTid)
{
  m_appTid = appTid;
}

void
McpttTestCaseConfig::SetCallTid (const TypeId& callTid)
{
  m_callTid = callTid;
}

void
McpttTestCaseConfig::SetFloorTid (const TypeId& floorTid)
{
  m_floorTid = floorTid;
}

void
McpttTestCaseConfig::SetStart (const Time& start)
{
  m_start = start;
}

void
McpttTestCaseConfig::SetStop (const Time& stop)
{
  m_stop = stop;
}

Ptr<McpttTestCall>
McpttTestCaseConfig::CreateTestCall (ObjectFactory& callFac, ObjectFactory& floorFac, uint16_t callId) const
{
  Ptr<McpttTestCall> call = CreateObject<McpttTestCall> (McpttCall::NetworkCallType::OFF_NETWORK);
  Ptr<McpttChannel> floorChannel = CreateObject<McpttChannel> ();
  Ptr<McpttChannel> mediaChannel = CreateObject<McpttChannel> ();
  Ptr<McpttCallMachine> callMachine = callFac.Create<McpttCallMachine> ();
  Ptr<McpttFloorParticipant> floorMachine = floorFac.Create<McpttFloorParticipant> ();

  call->SetCallMachine (callMachine);
  call->SetFloorChannel (floorChannel);
  call->SetFloorMachine (floorMachine);
  call->SetMediaChannel (mediaChannel);
  call->SetCallId (callId);
  return call;
}

} // namespace tests
} // namespace psc
} // namespace ns3

