/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
 */

#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/abort.h"

#include "ns3/psc-application-server.h"
#include "ns3/psc-application-client.h"
#include "psc-application-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscApplicationHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscApplicationHelper);

TypeId
PscApplicationHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscApplicationHelper")
    .SetParent<Object> ()
    .AddConstructor<PscApplicationHelper> ()
    .AddAttribute ("MinimumServerResponseInterval",
                   "Minimum interval between response packets sent by the servers.",
                   TimeValue (MilliSeconds (1)),
                   MakeTimeAccessor (&PscApplicationHelper::m_minTimeBetweenServerRsps),
                   MakeTimeChecker ())
  ;

  return tid;
}

PscApplicationHelper::PscApplicationHelper ()
{
  NS_LOG_FUNCTION (this);

  m_clientFactory.SetTypeId (PscApplicationClient::GetTypeId ());
  m_serverFactory.SetTypeId (PscApplicationServer::GetTypeId ());
}

PscApplicationHelper::~PscApplicationHelper ()
{
  NS_LOG_FUNCTION (this);
}

ApplicationContainer
PscApplicationHelper::Install (
  Ptr<PscApplicationConfiguration> config,
  Ptr<Node> client,
  Ptr<Node> server,
  Ipv6Address serverAddress,
  bool serverResponse,
  Time startTime,
  Time stopTime)
{
  NS_LOG_FUNCTION (this << config << client << server << serverAddress << serverResponse <<
                   startTime << stopTime);

  // Check that the start time is at least 1.
  NS_ABORT_MSG_IF (startTime < Seconds (1), "startTime MUST be equal or greater than 1 second");

  ApplicationContainer apps;

  uint32_t port = config->GetNextPort ();
  uint32_t instanceNo = config->IncrementInstances ();

  std::stringstream sstm;
  sstm << config->GetName () << "_" << instanceNo << "_";


  // Note that we add "Client" and "Server" to the application name provided.
  // This helps to process the trace file, but may lead to applications not being added to the
  // trace if the EnableSingleTrace (appName, trace) method is called with the same application
  // name that was used when calling this method.
  m_clientFactory.Set ("AppName",  StringValue (sstm.str () + "Client"));
  m_serverFactory.Set ("AppName",  StringValue (sstm.str () + "Server"));

  m_clientFactory.Set ("Protocol", TypeIdValue (config->GetSocketType ()));
  m_serverFactory.Set ("Protocol", TypeIdValue (config->GetSocketType ()));

  m_clientFactory.Set ("RemotePort", UintegerValue (port));
  m_serverFactory.Set ("Port", UintegerValue (port));

  // First the client application
  m_clientFactory.Set ("PacketSize", UintegerValue (config->GetPacketSizeClient ()));
  m_clientFactory.Set ("RemoteAddress", AddressValue (Address (serverAddress)));
  m_clientFactory.Set ("PacketsPerSession", PointerValue (config->GetPacketsPerSession ()));
  m_clientFactory.Set ("PacketInterval", PointerValue (config->GetPacketInterval ()));
  m_clientFactory.Set ("SessionInterval", PointerValue (config->GetSessionInterval ()));

  apps.Add (InstallClientPriv (config, client));

  // Server application
  m_serverFactory.Set ("PacketSize", UintegerValue (config->GetPacketSizeServer ()));
  m_serverFactory.Set ("IsSink", BooleanValue (!serverResponse));
  m_serverFactory.Set ("MinResponseInterval", TimeValue (m_minTimeBetweenServerRsps));

  apps.Add (InstallServerPriv (server));

  // Start the client at the specified time, and the server one second earlier
  // to make sure that it is ready by the time the first packet arrives.
  // This will break if startTime is less than 1
  // (that's the cause of the abort at the beginning of the method)
  apps.Get (0)->SetStartTime (startTime);
  apps.Get (1)->SetStartTime (startTime - Seconds (1));
  apps.Get (0)->SetStopTime (stopTime);
  apps.Get (1)->SetStopTime (stopTime + Seconds (1));

  return apps;
}

Ptr<Application>
PscApplicationHelper::InstallClientPriv (
  Ptr<PscApplicationConfiguration> config,
  Ptr<Node> client)
{
  NS_LOG_FUNCTION (this << config << client);

  Ptr<Application> app = m_clientFactory.Create<PscApplicationClient> ();
  client->AddApplication (app);

  m_incidentClientAppsCreated.Add (app);

  return app;
}

Ptr<Application>
PscApplicationHelper::InstallServerPriv (
  Ptr<Node> server)
{
  NS_LOG_FUNCTION (this << server);

  Ptr<Application> app = m_serverFactory.Create<PscApplicationServer> ();
  server->AddApplication (app);

  return app;
}

} // namespace psc
} // namespace ns3
