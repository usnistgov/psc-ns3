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
#include <ns3/mcptt-server-app.h>
#include <ns3/mobility-helper.h>
#include <ns3/network-module.h>

#include "mcptt-test-case-config-on-network.h"

namespace ns3 {
namespace psc {
namespace tests {

McpttTestCaseConfigOnNetwork::McpttTestCaseConfigOnNetwork (void)
  : McpttTestCaseConfig ()
{
  m_clientHelper.SetPttApp ("ns3::psc::McpttPttApp",
                            "PushOnStart", BooleanValue (false));
  m_clientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                              "Bytes", UintegerValue (60),
                              "DataRate", DataRateValue (DataRate ("24kb/s")));
  m_clientHelper.SetPusher ("ns3::psc::McpttPusher",
                            "Automatic", BooleanValue (false));

  m_callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                              "AckRequired", BooleanValue (false),
                              "AudioCutIn", BooleanValue (false),
                              "DualFloorSupported", BooleanValue (false),
                              "TxSsrc", UintegerValue (100),
                              "QueueingSupported", BooleanValue (true));
  m_callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                      "ReceiveOnly", BooleanValue (false));
  m_callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                               "AckRequired", BooleanValue (false),
                               "GenMedia", BooleanValue (true));
  m_callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                              "AmbientListening", BooleanValue (false),
                              "TemporaryGroup", BooleanValue (false));
}

McpttTestCaseConfigOnNetwork::~McpttTestCaseConfigOnNetwork (void)
{ }

ApplicationContainer
McpttTestCaseConfigOnNetwork::Configure (void)
{
  uint32_t appCount = GetAppCount ();
  Time start = GetStart ();
  Time stop = GetStop ();

  Config::Reset ();

  NodeContainer nodes;
  NodeContainer servers;
  NodeContainer clients;
  servers.Create (1);
  clients.Create (appCount);
  nodes.Add (servers);
  nodes.Add (clients);

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
  csma.SetChannelAttribute ("Delay", TimeValue (Seconds (1.0e-6)));

  NetDeviceContainer devices;
  devices = csma.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces;
  interfaces = address.Assign (devices);

  ApplicationContainer serverApps;
  serverApps.Add (m_serverHelper.Install (servers.Get (0)));
  serverApps.Start (start);
  serverApps.Stop (stop);

  m_serverApp = DynamicCast<McpttServerApp> (serverApps.Get (0));
  Ipv4Address serverAddress = interfaces.GetAddress (0);
  m_serverApp->SetLocalAddress (serverAddress);

  ApplicationContainer clientApps;
  clientApps.Add (m_clientHelper.Install (clients));
  clientApps.Start (start);
  clientApps.Stop (stop);

  for (uint32_t idx = 0; idx < clientApps.GetN (); idx++)
    {
      Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp> (clientApps.Get (idx));

      Ipv4Address clientAddress = interfaces.GetAddress (idx + servers.GetN ());
      pttApp->SetLocalAddress (clientAddress);
    }

  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  uint32_t groupId = 1;
  m_callHelper.AddCall (clientApps, m_serverApp, groupId, callType, start, stop);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  return clientApps;
}

void
McpttTestCaseConfigOnNetwork::SetCallHelper (const McpttCallHelper& callHelper)
{
  m_callHelper = callHelper;
}

void
McpttTestCaseConfigOnNetwork::SetClientHelper (const McpttHelper& clientHelper)
{
  m_clientHelper = clientHelper;
}

void
McpttTestCaseConfigOnNetwork::SetServerHelper (const McpttServerHelper& serverHelper)
{
  m_serverHelper = serverHelper;
}

Ptr<McpttServerApp>
McpttTestCaseConfigOnNetwork::GetServerApp (void) const
{
  return m_serverApp;
}

} // namespace tests
} // namespace psc
} // namespace ns3

