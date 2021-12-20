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

#include "ns3/log.h"
#include "ns3/abort.h"

#include "ns3/point-to-point-epc-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/buildings-helper.h"
#include "ns3/applications-module.h"
#include "ns3/lte-module.h"
#include "ns3/psc-module.h"
#include "ns3/psc-application-configuration.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/udp-group-echo-helper.h"
#include "ns3/psc-video-streaming.h"

#include "schoolshooting-application-helper.h"
#include "schoolshooting-application-trace-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SchoolShootingApplicationHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (SchoolShootingApplicationHelper);

SchoolShootingApplicationHelper::SchoolShootingApplicationHelper ()
  : m_initialized (false)
{
  NS_LOG_FUNCTION (this);

  m_genericApplicationHelper = CreateObject<PscApplicationHelper> ();
  m_traceHelper = CreateObject<SchoolShootingApplicationTraceHelper> (this);
  m_mcpttTraceHelper = CreateObject<McpttTraceHelper> ();
}

SchoolShootingApplicationHelper::~SchoolShootingApplicationHelper ()
{
  NS_LOG_FUNCTION (this);

  m_genericApplicationHelper = nullptr;
  m_enabledApps.clear ();
  if (m_mcpttTracesEnabled)
    {
      m_mcpttTraceHelper->DisableMsgTraces ();
      m_mcpttTraceHelper->DisableStateMachineTraces ();
      m_mcpttTraceHelper->DisableMouthToEarLatencyTrace ();
      m_mcpttTraceHelper->DisableAccessTimeTrace ();
    }
}

TypeId
SchoolShootingApplicationHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::SchoolShootingApplicationHelper")
    .SetParent<Object> ()
    .AddConstructor<SchoolShootingApplicationHelper> ()
    .AddAttribute ("Testing",
                   "True to run configuration with testing values for application start/stop",
                   BooleanValue (false),
                   MakeBooleanAccessor (&SchoolShootingApplicationHelper::m_testing),
                   MakeBooleanChecker ())
  ;

  return tid;
}

  
void 
SchoolShootingApplicationHelper::Initialize ()
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_UNLESS (!m_initialized, "Error: The scenario has already been initialized");

  InitApplications ();
}


void
SchoolShootingApplicationHelper::SetTechnologyHelper (Ptr<SchoolShootingNetworkTechnologyHelper> techHelper)
{
  NS_LOG_FUNCTION (this << techHelper);
  m_techHelper = techHelper;
}

Ptr<SchoolShootingNetworkTechnologyHelper> 
SchoolShootingApplicationHelper::GetTechnologyHelper ()
{
  return m_techHelper;
}

void
SchoolShootingApplicationHelper::SetScenarioDefinitionHelper (Ptr<SchoolShootingDefinitionHelper> definitionHelper)
{
  NS_LOG_FUNCTION (this << definitionHelper);
  m_scenarioDefinitionHelper = definitionHelper;
}

Ptr<PscScenarioDefinition> 
SchoolShootingApplicationHelper::GetScenarioDefinition ()
{
  Ptr<PscScenarioDefinition> scenarioDef = nullptr;
  if (m_scenarioDefinitionHelper != nullptr)
   {
    scenarioDef = m_scenarioDefinitionHelper->GetScenarioDefinition();
   }
  return scenarioDef;
}

void SchoolShootingApplicationHelper::InitApplications ()
{
  NS_LOG_FUNCTION (this);

  //Defines the port number to be used for deploying instances of each application
  //The default values allow for up to 200 instances of the same application
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_MCPTT] = 1000;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY] = 1200;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS] = 1400;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA] = 1600;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE] = 1800;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL] = 2000;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB] = 2200;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA] = 2400;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE] = 2600;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS] = 2800;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT] = 3000;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911] = 3200;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO] = 3400;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO] = 3600;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA] = 3800;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER] = 4000;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES] = 4200;
  m_basePortsMap[SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE] = 4400;

  // Set TCP flavor
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName ("ns3::TcpCubic")));

  // Set sender and receiver buffer sizes
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));

  // Set the default segment size of TCP packet
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (536));

  //Configure the different applications used in the scenario
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_MCPTT) != m_enabledApps.end ())
    {
      InitMcpttApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY) != m_enabledApps.end ())
    {
      InitTacticalTelemetryApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS) != m_enabledApps.end ())
    {
      InitBiometricApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA) != m_enabledApps.end ())
    {
      InitSwatHelmetCameraApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA) != m_enabledApps.end ())
    {
      InitDeployableCameraApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE) != m_enabledApps.end ())
    {
      InitThrowPhoneApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER) != m_enabledApps.end ())
    {
      InitHelicopterApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES) != m_enabledApps.end ())
    {
      InitEmsVehicleApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE) != m_enabledApps.end ())
    {
      InitVideoConferenceApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL) != m_enabledApps.end ())
    {
      InitAvlApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB) != m_enabledApps.end ())
    {
      InitWebApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA) != m_enabledApps.end ())
    {
      InitRmsBcaApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE) != m_enabledApps.end ())
    {
      InitSatelliteApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS) != m_enabledApps.end ())
    {
      InitBuildingPlanApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT) != m_enabledApps.end ())
    {
      InitIncidentManagementApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911) != m_enabledApps.end ())
    {
      InitNg911App ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO) != m_enabledApps.end ())
    {
      InitSchoolVideoApp ();
    }
  if (m_enabledApps.empty () || m_enabledApps.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO) != m_enabledApps.end ())
    {
      InitTrafficVideoApp ();
    }
}

void SchoolShootingApplicationHelper::InitMcpttApp ()
{
  NS_LOG_FUNCTION (this);

  // Disable media message traces to keep trace file size down
  Config::SetDefault ("ns3::psc::McpttMsgStats::Media", BooleanValue (false));

  // Register MCPTT application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "McpttPttApp";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_MCPTT, appInfo);

  // Get the address of the PS MCPTT server
  // Interface 0 is localhost, 1 is the p2p device
  Ipv6Address remoteHostAddr = GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ();

  // Install and start applications on server
  McpttServerHelper mcpttServerHelper;
  Ptr<McpttServerApp> mcpttServerApp = mcpttServerHelper.Install (GetTechnologyHelper ()->GetPsServer ());
  mcpttServerApp->SetLocalAddress (remoteHostAddr);
  if (m_testing)
    {
      mcpttServerApp->SetStartTime (Seconds (5));
      mcpttServerApp->SetStopTime (Seconds (95));
    }
  else
    {
      mcpttServerApp->SetStartTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E2_TaskForce_Arrive).time);
      mcpttServerApp->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);    
    }

  // Configure UE applications
  McpttHelper mcpttClientHelper;
  mcpttClientHelper.SetPttApp ("ns3::psc::McpttPttApp");
  mcpttClientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                         "Bytes", UintegerValue (60),
                         "DataRate", DataRateValue (DataRate ("12kb/s")));
  mcpttClientHelper.SetPusher ("ns3::psc::McpttPusher",
                         "Automatic", BooleanValue (false));

  // Create and install UE applications
  ApplicationContainer mcpttClientAppsSwatTeam1 = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1).nodes);
  ApplicationContainer mcpttClientAppsSwatTeam2 = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2).nodes);
  ApplicationContainer mcpttClientAppsSwatTeam3 = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3).nodes);
  ApplicationContainer mcpttClientAppsSwatTeam4 = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4).nodes);

  ApplicationContainer mcpttClientAppsFireEms = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE).nodes);
  mcpttClientAppsFireEms.Add (mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS).nodes));

  ApplicationContainer mcpttClientAppsTaskForce = mcpttClientHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE).nodes);

  // Setup starting pusher activity
  Ptr<McpttPusherOrchestratorSessionCdf> swatTeam1PttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.1), // Initial SWAT team actitivy factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));

  Ptr<McpttPusherOrchestratorSessionCdf> swatTeam2PttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.1), // Initial SWAT team actitivy factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));

  Ptr<McpttPusherOrchestratorSessionCdf> swatTeam3PttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.1), // Initial SWAT team actitivy factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));

  Ptr<McpttPusherOrchestratorSessionCdf> swatTeam4PttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.1), // Initial SWAT team actitivy factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));


  Ptr<McpttPusherOrchestratorSessionCdf> fireEmsPttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.3), // Initial fire/EMS activity factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));

  Ptr<McpttPusherOrchestratorSessionCdf> taskForcePttActivity = CreateObjectWithAttributes<McpttPusherOrchestratorSessionCdf> (
      "ActivityFactor", DoubleValue (0.05), // Task force activity factor
      "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorContention> (
          "ContentionProbability", DoubleValue (0.0),
          "Orchestrator", PointerValue (CreateObjectWithAttributes<McpttPusherOrchestratorSpurtCdf> (
              "ActivityFactor", DoubleValue (1.0))))));

  // Assocate pusher orchestrators to their respective groups
  mcpttClientHelper.AddPushersToOrchestrator (swatTeam1PttActivity, mcpttClientAppsSwatTeam1);
  mcpttClientHelper.AddPushersToOrchestrator (swatTeam2PttActivity, mcpttClientAppsSwatTeam2);
  mcpttClientHelper.AddPushersToOrchestrator (swatTeam3PttActivity, mcpttClientAppsSwatTeam3);
  mcpttClientHelper.AddPushersToOrchestrator (swatTeam4PttActivity, mcpttClientAppsSwatTeam4);

  mcpttClientHelper.AddPushersToOrchestrator (fireEmsPttActivity, mcpttClientAppsFireEms);
  mcpttClientHelper.AddPushersToOrchestrator (taskForcePttActivity, mcpttClientAppsTaskForce);

  // Make it so that the pushers persist beyond the scope of this function
  m_pttOrchestrators.push_back (swatTeam1PttActivity);
  m_pttOrchestrators.push_back (swatTeam2PttActivity);
  m_pttOrchestrators.push_back (swatTeam3PttActivity);
  m_pttOrchestrators.push_back (swatTeam4PttActivity);

  m_pttOrchestrators.push_back (fireEmsPttActivity);
  m_pttOrchestrators.push_back (taskForcePttActivity);

  // Collect all client applications into one container
  ApplicationContainer mcpttClientApps;
  mcpttClientApps.Add (mcpttClientAppsSwatTeam1);
  mcpttClientApps.Add (mcpttClientAppsSwatTeam2);
  mcpttClientApps.Add (mcpttClientAppsSwatTeam3);
  mcpttClientApps.Add (mcpttClientAppsSwatTeam4);
  mcpttClientApps.Add (mcpttClientAppsFireEms);
  mcpttClientApps.Add (mcpttClientAppsTaskForce);

  // Configure start/stop times of client applications
  if (m_testing)
    {
      mcpttClientApps.Start (Seconds (5));
      mcpttClientApps.Stop (Seconds (95));
    }
  else
    {
      mcpttClientApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E2_TaskForce_Arrive).time);
      mcpttClientApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);    
    }
  
  // Get all client net devices so that we can access their IP address
  NetDeviceContainer clientNetDevices;
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS));
  clientNetDevices.Add (GetTechnologyHelper ()->GetNetDevicesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE));

  // Make application aware of UE's IP address
  for (uint32_t netDevIndex = 0; netDevIndex < clientNetDevices.GetN (); netDevIndex++)
    {
      bool found = false;
      uint32_t appIndex = 0;
      Ptr<NetDevice> ueDevice = clientNetDevices.Get (netDevIndex);
      Ptr<Node> ueNode = ueDevice->GetNode (); 
      Ptr<Ipv6> ueIpv6 = ueNode->GetObject<Ipv6> ();
      while (appIndex < mcpttClientApps.GetN () && !found)
        {
          Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp> (mcpttClientApps.Get (appIndex));
          found = (pttApp->GetNode ()->GetId () == ueNode->GetId ());
          if (found)
            {
              uint32_t interface6 =  ueIpv6->GetInterfaceForDevice (ueDevice);
              Ipv6Address ueAddr6 = ueIpv6->GetAddress (interface6, 1).GetAddress ();
              pttApp->SetLocalAddress (ueAddr6);
            }
          appIndex++;
        }
    }

  // Configuration for all the different group calls
  McpttCallHelper callHelper;
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                         "AckRequired", BooleanValue (false),
                         "AudioCutIn", BooleanValue (false),
                         "DualFloorSupported", BooleanValue (false),
                         "TxSsrc", UintegerValue (100));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                         "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                         "AckRequired", BooleanValue (false),
                         "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                         "AmbientListening", BooleanValue (false),
                         "TemporaryGroup", BooleanValue (false));

  // Create and install calls for each group
  // This is also where we set when/how PTT activity should change over the course of the simulation
  uint32_t groupId = 0;
  McpttCallMsgFieldCallType callType;

  callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  if (m_testing)
    {
      callHelper.AddCall (mcpttClientAppsTaskForce, mcpttServerApp, groupId++, callType, Seconds (10), Seconds (90));
      taskForcePttActivity->StartAt (Seconds (10));
      taskForcePttActivity->StopAt (Seconds (90));
    }
  else
    {
      callHelper.AddCall (mcpttClientAppsTaskForce, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E2_TaskForce_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);    
      taskForcePttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E2_TaskForce_Arrive).time);
      taskForcePttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }
 
  callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  if (m_testing)
    {
      callHelper.AddCall (mcpttClientAppsSwatTeam1, mcpttServerApp, groupId++, callType, Seconds (15), Seconds (85));
      callHelper.AddCall (mcpttClientAppsSwatTeam2, mcpttServerApp, groupId++, callType, Seconds (15), Seconds (85));
      callHelper.AddCall (mcpttClientAppsSwatTeam3, mcpttServerApp, groupId++, callType, Seconds (15), Seconds (85));
      callHelper.AddCall (mcpttClientAppsSwatTeam4, mcpttServerApp, groupId++, callType, Seconds (15), Seconds (85));
      swatTeam1PttActivity->StartAt (Seconds (10));
      swatTeam1PttActivity->StopAt (Seconds (85));
      swatTeam2PttActivity->StartAt (Seconds (10));
      swatTeam2PttActivity->StopAt (Seconds (85));
      swatTeam3PttActivity->StartAt (Seconds (10));
      swatTeam3PttActivity->StopAt (Seconds (85));
      swatTeam4PttActivity->StartAt (Seconds (10));
      swatTeam4PttActivity->StopAt (Seconds (85));

      Simulator::Schedule (Seconds (38),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam1PttActivity,
          0.5);

      Simulator::Schedule (Seconds (51),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam1PttActivity,
          0.1);

      Simulator::Schedule (Seconds (38),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam2PttActivity,
          0.5);

      Simulator::Schedule (Seconds (51),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam2PttActivity,
          0.1);

      Simulator::Schedule (Seconds (38),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam3PttActivity,
          0.5);

      Simulator::Schedule (Seconds (51),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam3PttActivity,
          0.1);

      Simulator::Schedule (Seconds (38),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam4PttActivity,
          0.5);

      Simulator::Schedule (Seconds (51),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam4PttActivity,
          0.1);
    }
  else
    {
      callHelper.AddCall (mcpttClientAppsSwatTeam1, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time); 
      callHelper.AddCall (mcpttClientAppsSwatTeam2, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time); 
      callHelper.AddCall (mcpttClientAppsSwatTeam3, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time); 
      callHelper.AddCall (mcpttClientAppsSwatTeam4, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time); 
      swatTeam1PttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
      swatTeam1PttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
      swatTeam2PttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
      swatTeam2PttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
      swatTeam3PttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
      swatTeam3PttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
      swatTeam4PttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
      swatTeam4PttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam1PttActivity,
          0.5);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam1PttActivity,
          0.1);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam2PttActivity,
          0.5);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam2PttActivity,
          0.1);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam3PttActivity,
          0.5);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam3PttActivity,
          0.1);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam4PttActivity,
          0.5);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          swatTeam4PttActivity,
          0.1);
    }

  callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  if (m_testing)
    {
      callHelper.AddCall (mcpttClientAppsFireEms, mcpttServerApp, groupId++, callType, Seconds (20), Seconds (80));

      Simulator::Schedule (Seconds (40),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          fireEmsPttActivity,
          0.4);

      Simulator::Schedule (Seconds (60),
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          fireEmsPttActivity,
          0.1);
    }
  else
    {
      callHelper.AddCall (mcpttClientAppsFireEms, mcpttServerApp, groupId++, callType, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E4_FireEMS_Arrive).time, m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);    
      fireEmsPttActivity->StartAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E4_FireEMS_Arrive).time);
      fireEmsPttActivity->StopAt (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          fireEmsPttActivity,
          0.4);

      Simulator::Schedule (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time,
          &McpttPusherOrchestratorSessionCdf::SetActivityFactor,
          fireEmsPttActivity,
          0.1);
    }
}  

void SchoolShootingApplicationHelper::InitTacticalTelemetryApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "TacticalTelemetry";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY]
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (60)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    88,        //Client packet size (bytes)
    8);       //Server packet size (bytes)

  // Set start/stop time of client applications.
  Time startTime;
  Time stopTime;
  Ptr<NormalRandomVariable> startSwatTeam = nullptr;
  Ptr<NormalRandomVariable> endSwatTeam = nullptr;

  if (m_testing)
  {
    startSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (10),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (2690328116));
    endSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (90),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (387351077));
  }
  else
  {
    startSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E7_SWAT_Deploy).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (1302947634));
    endSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
     "Variance", DoubleValue (12.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (4118718303));
  }

  // Collect all participating nodes into one container
  NodeContainer allSwats;
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4).nodes);

  //Configure tactical telemetry applications
  for (uint16_t i = 0; i < allSwats.GetN (); i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (startSwatTeam->GetValue ());
      stopTime = Seconds (endSwatTeam->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY,
                          m_genericApplicationHelper->Install (appConfig, allSwats.Get (i),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitBiometricApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "Biometrics";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS]
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (6)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (10)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    7);       //Client packet size (bytes), no packet from server


  //QoS Parameters associated with the application
//  EpsBearer::Qci qci = EpsBearer::NGBR_MC_DATA;
//  AllocationRetentionPriority arp;
//  arp.preemptionCapability = true;
//  arp.preemptionVulnerability = true;
//  arp.priorityLevel = 10;
//  GbrQosInformation qosInfoClient;
//  GbrQosInformation qosInfoServer;

  // Configure start/stop times for the client applications
  Time startTime;
  Time stopTime;
  Ptr<NormalRandomVariable> startSwatTeam = nullptr;
  Ptr<NormalRandomVariable> endSwatTeam = nullptr;
  Ptr<NormalRandomVariable> startPatients = nullptr;
  Ptr<NormalRandomVariable> endPatients = nullptr;

  if (m_testing)
    {
      startSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (10),
          "Variance", DoubleValue (1),
          "Bound", DoubleValue (1),
          "Stream", IntegerValue (4199179370));
      endSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (90),
          "Variance", DoubleValue (1),
          "Bound", DoubleValue (1),
          "Stream", IntegerValue (3155653880));

      startPatients = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (10),
          "Variance", DoubleValue (1),
          "Bound", DoubleValue (1),
          "Stream", IntegerValue (1547664542));
      endPatients = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (90),
          "Variance", DoubleValue (1),
          "Bound", DoubleValue (1),
          "Stream", IntegerValue (2148173542));
    }
  else
    {
      startSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E7_SWAT_Deploy).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3597260308));
      endSwatTeam = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (686870781));

      startPatients = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (1811505450));
      endPatients = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E11_EMS_Departure).time.GetSeconds ()),
          "Variance", DoubleValue (0),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (2542779624));
  }

  // Collect all nodes
  NodeContainer allSwats;
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3).nodes);
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4).nodes);

  // Configure the applications
  for (uint16_t i = 0; i < allSwats.GetN (); i++)
    {
      //Deploy app
      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS,
                          m_genericApplicationHelper->Install (appConfig, allSwats.Get (i),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               false,
                                                               Seconds (startSwatTeam->GetValue ()),
                                                               Seconds (endSwatTeam->GetValue ())));
    }

  NodeContainer emsVehicles = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES).nodes;

  for (uint32_t i = 0; i < emsVehicles.GetN (); i ++)
    {
      //Deploy app
      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS,
                          m_genericApplicationHelper->Install (appConfig, emsVehicles.Get (i),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               false,
                                                               Seconds (startPatients->GetValue ()),
                                                               Seconds (endPatients->GetValue ())));
    }
}

void SchoolShootingApplicationHelper::InitSwatHelmetCameraApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "SwatHelmetCamera";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from a first responder to a UDP group echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. Unified Command and Staging Area)
  //
  
  //Get all the sender nodes. One member in each team
  NodeContainer allSwats;
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1).nodes.Get (0));
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2).nodes.Get (0));
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3).nodes.Get (0));
  allSwats.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4).nodes.Get (0));

  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (3565260182));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_SWAT_HELMET_CAMERA];

  for (uint16_t i = 0; i < allSwats.GetN (); i++)
    {
      //Configure the UDP echo server acting as media server
      psc::UdpGroupEchoServerHelper echoServer (serverPort);
      echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
      echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
      echoServer.SetAttribute ("EchoClient", BooleanValue (false));
      echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

      ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());
      if (m_testing)
        {
          serverApps.Start (Seconds (5)); //start before client
          serverApps.Stop (Seconds (100));  //end after client
        }
      else
        {
          serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
          serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      //Configure video sender
      Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
      // Attributes
      streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
      streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
      streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
      streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
          std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

      // Add application / streaming source to the node
      allSwats.Get (i)->AddApplication (streamSrc);

      // Configure the start/stop times
      if (m_testing)
        {
          streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
          streamSrc->SetStopTime (Seconds (95));
        }
      else
        {
          streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E7_SWAT_Deploy).time.GetSeconds () + startVar->GetValue ()));
          streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
        }    

      //Add receiver nodes (UC and SA)
      ApplicationContainer streamSink;
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

      if (m_testing)
        {
          streamSink.Start (Seconds (5));
          streamSink.Stop (Seconds (95));
        } 
      else
        {
          streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      // Tell echo server where to foward received packets
      Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
      Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (ucSocket);
      Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (saSocket);

      // So now we have stream that goes [Helement Camera] -> [Echo Server] -> [UC/SA]

      // Add traces sources for senders and receivers
      m_swatHelmetCamerasTx.Add (streamSrc);
      m_swatHelmetCamerasRx.Add (streamSink);

      //Increment port for next instances
      serverPort++;
    }
 
}

void SchoolShootingApplicationHelper::InitDeployableCameraApp ()
{
  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "DeployableCameras";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from a first responder to a UDP group echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. Unified Command and Staging Area)
  //

  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (661635272));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_DEPLOYABLE_CAMERA];

  //Get all the sender nodes. One member in each team
  NodeContainer deployableCameras = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::DEPLOYABLE_CAMERA).nodes;

  for (uint16_t i = 0; i < deployableCameras.GetN (); i++)
    {
      //Configure the UDP echo server acting as media server
      psc::UdpGroupEchoServerHelper echoServer (serverPort);
      echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
      echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
      echoServer.SetAttribute ("EchoClient", BooleanValue (false));
      echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

      ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());
      if (m_testing)
        {
          serverApps.Start (Seconds (5)); //start before client
          serverApps.Stop (Seconds (100));  //end after client
        }
      else
        {
          serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
          serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      //Configure video sender
      Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
      // Attributes
      streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
      streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
      streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
      streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
          std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

      deployableCameras.Get (i)->AddApplication (streamSrc);

      if (m_testing)
        {
          streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
          streamSrc->SetStopTime (Seconds (95));
        }
      else
        {
          streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E8_Offender_Found).time.GetSeconds () + startVar->GetValue ()));
          streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
        }    

      //Add receiver nodes (UC and SA)
      ApplicationContainer streamSink;
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

      if (m_testing)
        {
          streamSink.Start (Seconds (5));
          streamSink.Stop (Seconds (95));
        } 
      else
        {
          streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      // Tell echo server where to send the received video
      Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
      Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (ucSocket);
      Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (saSocket);

      // So now we have stream that goes [Deployable Camera] -> [Echo Server] -> [UC/SA]
      
      // Add traces sources for senders and receivers
      m_deployableCamerasTx.Add (streamSrc);
      m_deployableCamerasRx.Add (streamSink);

      //Increment port for next instances
      serverPort++;
    }
}

void SchoolShootingApplicationHelper::InitThrowPhoneApp ()
{
  NS_LOG_FUNCTION (this);
  
  //Get all the sender nodes. One member in each team
  NodeContainer offenders = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::OFFENDER).nodes;

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "ThrowPhone";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from a first responder to a UDP group echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. Unified Command and Staging Area)
  
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (734255024));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_THROW_PHONE];

  //Configure the UDP echo server acting as media server
  psc::UdpGroupEchoServerHelper echoServer (serverPort);
  echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
  echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
  echoServer.SetAttribute ("EchoClient", BooleanValue (false));
  echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

  ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());
  if (m_testing)
    {
      serverApps.Start (Seconds (5)); //start before client
      serverApps.Stop (Seconds (100));  //end after client
    }
  else
    {
      serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
      serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  //Configure video sender
  Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
  // Attributes
  streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
  streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
  streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
  streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
      std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

  offenders.Get (0)->AddApplication (streamSrc);

  if (m_testing)
    {
      streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
      streamSrc->SetStopTime (Seconds (95));
    }
  else
    {
      streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E8_Offender_Found).time.GetSeconds () + startVar->GetValue ()));
      streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time);
    }    

  //Add receiver nodes (UC and SA)
  ApplicationContainer streamSink;
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
  streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
  streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

  if (m_testing)
    {
      streamSink.Start (Seconds (5));
      streamSink.Stop (Seconds (95));
    } 
  else
    {
      streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
      streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  // Tell echo server where to send the received video
  Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
  Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
  echoServerApp->AddClient (ucSocket);
  Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
  echoServerApp->AddClient (saSocket);

  serverPort++;

  // So now we have a video stream that goes [Throw Phone] -> [Echo Server] -> [UC/SA]

  // Add traces sources for senders and receivers
  m_throwPhoneCamerasTx.Add (streamSrc);
  m_throwPhoneCamerasRx.Add (streamSink);

  //
  // Configure voice service
  //

  // Configure start/stop times
  Time startTime;
  Time stopTime;
  if (m_testing)
    {
      startTime = Seconds (6);
      stopTime = Seconds (95);
    }
  else
    {
      startTime = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E8_Offender_Found).time;
      stopTime = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E9_Interdiction).time;
    }

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      UdpSocketFactory::GetTypeId (),     //Socket type
      serverPort
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1296)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0.1111)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (216)),   //Time between sessions
    384,        //Client packet size (bytes)
    384);       //Server packet size (bytes)

  for (uint16_t i = 0; i < offenders.GetN (); i++)
    {
      //Deploy app
      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE,
                          m_genericApplicationHelper->Install (appConfig, offenders.Get (i),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitHelicopterApp ()
{
  NS_LOG_FUNCTION (this);
  
  //Get all the sender nodes. One member in each team
  NodeContainer helicopter = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::HELICOPTER).nodes;

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "HelicopterVideoStream";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from a first responder to a UDP group echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. Unified Command and Staging Area)
  
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (1730602846));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_HELICOPTER];

  //Configure the UDP echo server acting as media server
  psc::UdpGroupEchoServerHelper echoServer (serverPort);
  echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
  echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
  echoServer.SetAttribute ("EchoClient", BooleanValue (false));
  echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

  ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());
  if (m_testing)
    {
      serverApps.Start (Seconds (5)); //start before client
      serverApps.Stop (Seconds (100));  //end after client
    }
  else
    {
      serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
      serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  //Configure video sender
  Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
  // Attributes
  streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
  streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
  streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
  streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
      std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

  helicopter.Get (0)->AddApplication (streamSrc);

  if (m_testing)
    {
      streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
      streamSrc->SetStopTime (Seconds (7));
    }
  else
    {
      streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time.GetSeconds () + startVar->GetValue ()));
      streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time);
    }    

  //Add receiver nodes (UC and SA)
  ApplicationContainer streamSink;
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
  streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
  streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

  if (m_testing)
    {
      streamSink.Start (Seconds (5));
      streamSink.Stop (Seconds (95));
    } 
  else
    {
      streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
      streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  // Add stream source socket to UDP echo server
  Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
  Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
  echoServerApp->AddClient (ucSocket);
  Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
  echoServerApp->AddClient (saSocket);
  
  // Add traces sources for senders and receivers
  m_helicopterCamerasTx.Add (streamSrc);
  m_helicopterCamerasRx.Add (streamSink);

  serverPort++;
}

void SchoolShootingApplicationHelper::InitEmsVehicleApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "EmsVehicle";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from a first responder to a UDP group echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. Unified Command and Staging Area)
  //
  
  //Get all the sender nodes. One member in each team
  NodeContainer emsVehicles = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES).nodes;
  //
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (2776766018));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_EMS_VEHICLES];

  for (uint16_t i = 0; i < emsVehicles.GetN (); i++)
    {
      //Configure the UDP echo server acting as media server
      psc::UdpGroupEchoServerHelper echoServer (serverPort);
      echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
      echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
      echoServer.SetAttribute ("EchoClient", BooleanValue (false));
      echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

      ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());
      if (m_testing)
        {
          serverApps.Start (Seconds (5)); //start before client
          serverApps.Stop (Seconds (100));  //end after client
        }
      else
        {
          serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E3_SWAT_Arrive).time);
          serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      //Configure video sender
      Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
      // Attributes
      streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
      streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
      streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
      streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
          std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

      emsVehicles.Get (i)->AddApplication (streamSrc);

      if (m_testing)
        {
          streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
          streamSrc->SetStopTime (Seconds (95));
        }
      else
        {
          streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E10_Incident_Secured).time.GetSeconds () + startVar->GetValue ()));
          streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E11_EMS_Departure).time);
        }    

      //Add receiver nodes (UC and SA)
      ApplicationContainer streamSink;
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
      streamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

      if (m_testing)
        {
          streamSink.Start (Seconds (5));
          streamSink.Stop (Seconds (95));
        } 
      else
        {
          streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      // Tell echo server where to forward packets to
      Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
      Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (ucSocket);
      Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (saSocket);

      // So now we have a video stream that goes [EMS vehicle] -> [Echo Server] -> [UC/SA]

      // Add traces sources for senders and receivers
      m_emsVehicleCamerasTx.Add (streamSrc);
      m_emsVehicleCamerasRx.Add (streamSink);

      //Increment port for next instances
      serverPort++;
    }
}

void SchoolShootingApplicationHelper::InitVideoConferenceApp ()
{
  NS_LOG_FUNCTION (this);
  
  // Get the nodes participating in the video conference
  NodeContainer ucVehicles = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;
  NodeContainer saVehicles = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes;

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "VideoConference";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE, appInfo);

  //Video applications are configured as follows:
  //- An video stream is sent from UC / SA to the echo server
  //- The UDP group echo server sends a copy of the packets to each registered
  //  UDP echo clients (e.g. UC and SA)
  
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (2964008164));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_VIDEO_CONFERENCE];

  //Configure the UDP echo server acting as media server
  psc::UdpGroupEchoServerHelper echoServer (serverPort);
  echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
  echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
  echoServer.SetAttribute ("EchoClient", BooleanValue (false));
  echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

  ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPsServer ());

  echoServer.SetAttribute ("Port", UintegerValue (serverPort + 1));
  echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort + 1));

  serverApps.Add (echoServer.Install (GetTechnologyHelper ()->GetPsServer ()));

  // Configure server start/stop times
  if (m_testing)
    {
      serverApps.Start (Seconds (5)); //start before client
      serverApps.Stop (Seconds (100));  //end after client
    }
  else
    {
      serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time);
      serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  //Configure UC video stream
  Ptr <PscVideoStreaming> ucStreamSrc = CreateObject <PscVideoStreaming> ();
  // Attributes
  ucStreamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
  ucStreamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
  ucStreamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
  ucStreamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1920x1080-30-25-true_packetsize.txt_90_90"),
      std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1920x1080-30-25-true_packetinterval.txt_90_90"));

  // Install the UC stream source
  ucVehicles.Get (0)->AddApplication (ucStreamSrc);

  // Configure SA videostream
  Ptr <PscVideoStreaming> saStreamSrc = CreateObject <PscVideoStreaming> ();
  // Attributes
  saStreamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
  saStreamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort + 1));
  saStreamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
  saStreamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1920x1080-30-25-true_packetsize.txt_90_90"),
      std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1920x1080-30-25-true_packetinterval.txt_90_90"));

  // Install the SA stream source
  saVehicles.Get (0)->AddApplication (saStreamSrc);

  // Configure start/stop times for the stream sources
  Time startTime;
  Time stopTime;

  if (m_testing)
    {
      startTime = Seconds (6 + startVar->GetValue ());
      stopTime = Seconds (95);
    }
  else
    {
      startTime = Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time.GetSeconds () + startVar->GetValue ());
      stopTime = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time;
    }    

  // Set the start/stop times.
  ucStreamSrc->SetStartTime (startTime);
  ucStreamSrc->SetStopTime (stopTime);
  saStreamSrc->SetStartTime (startTime);
  saStreamSrc->SetStopTime (stopTime);

  //Add receiver sockets for UC and SA
  ApplicationContainer streamSink;
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort + 1));
  streamSink.Add (packetSinkHelper.Install (ucVehicles.Get (0)));

  packetSinkHelper.SetAttribute ("Local", AddressValue (Inet6SocketAddress (Ipv6Address::GetAny (), serverPort)));
  streamSink.Add (packetSinkHelper.Install (saVehicles.Get (0)));

  if (m_testing)
    {
      streamSink.Start (Seconds (5));
      streamSink.Stop (Seconds (95));
    } 
  else
    {

      streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time);
      streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  // Tell echo server where to forward received streams
  Inet6SocketAddress ucSocket (ucVehicles.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort + 1);
  serverApps.Get (1)->GetObject <UdpGroupEchoServer> ()->AddClient (ucSocket);

  Inet6SocketAddress saSocket (saVehicles.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
  serverApps.Get (0)->GetObject <UdpGroupEchoServer> ()->AddClient (saSocket);

  // So now we have a video stream that goes [UC] <-> [Echo Server] <-> [SA]

  // Add traces sources for senders and receivers
  m_videoConferenceVideoTx.Add (ucStreamSrc);
  m_videoConferenceVideoTx.Add (saStreamSrc);
  m_videoConferenceVideoRx.Add (streamSink);

  serverPort += 2;

  //
  // Configure voice service
  //

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      UdpSocketFactory::GetTypeId (),     //Socket type
      serverPort
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1296)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0.1111)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    384,        //Client packet size (bytes)
    384);       //Server packet size (bytes)

  //Deploy apps
  AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE,
                      m_genericApplicationHelper->Install (appConfig, ucVehicles.Get (0),
                                                            GetTechnologyHelper ()->GetPsServer (),
                                                            GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                            true,
                                                            startTime,
                                                            stopTime));

  AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE,
                      m_genericApplicationHelper->Install (appConfig, saVehicles.Get (0),
                                                            GetTechnologyHelper ()->GetPsServer (),
                                                            GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                            true,
                                                            startTime,
                                                            stopTime));

}


void SchoolShootingApplicationHelper::InitAvlApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "AutomaticVehicleLocation";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      UdpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL]
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (4)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (15)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    1408,        //Client packet size (bytes)
    128);       //Server packet size (bytes)

  //Configure SWAT team applications
  NodeContainer nodes;

  Ptr<NormalRandomVariable> startRndVar = nullptr;
  Ptr<NormalRandomVariable> endRndVar = nullptr;

  if (m_testing)
    {
      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
        "Mean", DoubleValue (5),
        "Variance", DoubleValue (1),
        "Bound", DoubleValue (1),
        "Stream", IntegerValue (226200624));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
        "Mean", DoubleValue (90),
        "Variance", DoubleValue (1),
        "Bound", DoubleValue (1),
        "Stream", IntegerValue (3302719730));

      // 34 instances in permiter patrol cars (TASK_FORCE_VEHCILES)
      // 2 fire engines (FIRE_VEHCILES)
      // 3 ambulances (EMS_VEHCILES)
      // 1 mobile command center (UC_VEHCILE)
      // 1 SA mobile unit (SA_VEHCILE)
      // 1 helicopter (HELICOPTER)
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE_VEHICLES).nodes);
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE_VEHICLES).nodes);
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES).nodes);
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes);
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes);
      nodes.Add (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::HELICOPTER).nodes);

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }
    }
  else
    {
      // 34 instances in permiter patrol cars (TASK_FORCE_VEHCILES)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE_VEHICLES).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E2_TaskForce_Arrive).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (2353855280));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3752123142));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
            m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
              GetTechnologyHelper ()->GetPsServer (),
              GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
              true,
              Seconds (startRndVar->GetValue ()),
              Seconds (endRndVar->GetValue ())));
        }

      // 2 fire engines (FIRE_VEHCILES)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE_VEHICLES).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E4_FireEMS_Arrive).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (1864306809));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3945724599));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }

      // 3 ambulances (EMS_VEHCILES)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E4_FireEMS_Arrive).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3469341645));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E11_EMS_Departure).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3034262386));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }

      // 1 mobile command center (UC_VEHCILE)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (4019168555));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3543992080));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }

      // 1 SA mobile unit (SA_VEHCILE)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E6_Perimeters_Ready).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (2642586946));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (2875877323));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }

      // 1 helicopter (HELICOPTER)
      nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::HELICOPTER).nodes;

      startRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
          "Variance", DoubleValue (6.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (3020755717));
      endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
          "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time.GetSeconds ()),
          "Variance", DoubleValue (12.25 * 60),
          "Bound", DoubleValue (10 * 60),
          "Stream", IntegerValue (2231723316));

      for (uint16_t i = 0; i < nodes.GetN (); i++)
        {
          AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL,
              m_genericApplicationHelper->Install (appConfig, nodes.Get (i),
                GetTechnologyHelper ()->GetPsServer (),
                GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                true,
                Seconds (startRndVar->GetValue ()),
                Seconds (endRndVar->GetValue ())));
        }
    }
}

void SchoolShootingApplicationHelper::InitWebApp ()
{
  NS_LOG_FUNCTION (this);


  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "Web";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB, appInfo);

  //Some information of each instance of the application
  Time startTime;
  Time stopTime;

  //Configure the start/stop times of the applications
  Ptr<UniformRandomVariable> startRndVar = nullptr;
  Ptr<UniformRandomVariable> endRndVar = nullptr;

  if (m_testing)
    {
      startRndVar = CreateObjectWithAttributes<UniformRandomVariable> (
        "Min", DoubleValue (0),
        "Max", DoubleValue (1),
        "Stream", IntegerValue (2353680874));
      endRndVar = CreateObjectWithAttributes<UniformRandomVariable> (
        "Min", DoubleValue (0),
        "Max", DoubleValue (1),
        "Stream", IntegerValue (1692196905));
    }
  else
    {
      startRndVar = CreateObjectWithAttributes<UniformRandomVariable> (
        "Min", DoubleValue (0),
        "Max", DoubleValue (600),
        "Stream", IntegerValue (3439410251));
      endRndVar = CreateObjectWithAttributes<UniformRandomVariable> (
        "Min", DoubleValue (0),
        "Max", DoubleValue (600),
        "Stream", IntegerValue (13912914));
    }

  // Configure the client and server apps
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  IntelHttpServerHelper graphicalServerHelper;
  IntelHttpServerHelper nonGraphicalServerHelper;
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_WEB];
  IntelHttpClientHelper graphicalClientHelper (GetTechnologyHelper ()->GetPublicServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ());
  IntelHttpClientHelper nonGraphicalClientHelper (GetTechnologyHelper ()->GetPublicServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ());
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;

  // Configure one instance of non-graphical random variables for 2x the non-graphical activity
  Ptr<ConstantRandomVariable> nonGraphicalEmbeddedObjectAmountRvs = CreateObject<ConstantRandomVariable> ();
  Ptr<NormalRandomVariable> nonGraphicalHtmlSizeRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> nonGraphicalReadTimeRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> nonGraphicalParseTimeRvs = CreateObject<NormalRandomVariable> ();

  nonGraphicalEmbeddedObjectAmountRvs->SetAttribute ("Constant", DoubleValue (0));
  nonGraphicalHtmlSizeRvs->SetAttribute ("Mean", DoubleValue (632 * 1024));
  nonGraphicalHtmlSizeRvs->SetAttribute ("Variance", DoubleValue ((632 * 1024 * 0.05) * (632 * 1024 * 0.05)));
  nonGraphicalHtmlSizeRvs->SetAttribute ("Stream", IntegerValue (3937459790));
  // Read time should be about two minutes (i.e. 120 s).
  nonGraphicalReadTimeRvs->SetAttribute ("Mean", DoubleValue (120));
  nonGraphicalReadTimeRvs->SetAttribute ("Variance", DoubleValue ((120 * 0.05) * (120 * 0.05)));
  nonGraphicalReadTimeRvs->SetAttribute ("Stream", IntegerValue (2893591180));

  nonGraphicalParseTimeRvs->SetAttribute ("Mean", DoubleValue (0.5));
  nonGraphicalParseTimeRvs->SetAttribute ("Variance", DoubleValue ((0.5 * 0.05) * (0.5 * 0.05)));
  nonGraphicalParseTimeRvs->SetAttribute ("Stream", IntegerValue (2198857611));

  // Install instance of the non-graphical application
  nonGraphicalServerHelper.Set ("Port", UintegerValue (serverPort));
  nonGraphicalServerHelper.Set ("EmbeddedObjectAmountRvs", PointerValue (nonGraphicalEmbeddedObjectAmountRvs));
  nonGraphicalServerHelper.Set ("HtmlSizeRvs", PointerValue (nonGraphicalHtmlSizeRvs));
  serverApps.Add (nonGraphicalServerHelper.Install (GetTechnologyHelper ()->GetPublicServer ()));

  nonGraphicalClientHelper.Set ("RemotePort", UintegerValue (serverPort));
  nonGraphicalClientHelper.Set ("ReadTimeRvs", PointerValue (nonGraphicalReadTimeRvs));
  nonGraphicalClientHelper.Set ("ParseTimeRvs", PointerValue (nonGraphicalReadTimeRvs));
  clientApps.Add (nonGraphicalClientHelper.Install (ucNodes));

  serverPort++;

  // Configure one instance of graphical random variables for 2x the graphical activity

  Ptr<NormalRandomVariable> graphicalEmbeddedObjectIatRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> graphicalEmbeddedObjectSizeRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> graphicalEmbeddedObjectAmountRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> graphicalReadTimeRvs = CreateObject<NormalRandomVariable> ();
  Ptr<NormalRandomVariable> graphicalParseTimeRvs = CreateObject<NormalRandomVariable> ();

  graphicalEmbeddedObjectIatRvs->SetAttribute ("Mean", DoubleValue (0.2));
  graphicalEmbeddedObjectIatRvs->SetAttribute ("Variance", DoubleValue ((0.2 * 0.2) * (0.2 * 0.2)));
  graphicalEmbeddedObjectIatRvs->SetAttribute ("Stream", IntegerValue (3425821638));
  // On average embbedd objects should be 152,371.2 B
  graphicalEmbeddedObjectSizeRvs->SetAttribute ("Mean", DoubleValue (152371.2));
  graphicalEmbeddedObjectSizeRvs->SetAttribute ("Variance", DoubleValue ((152371.2 * 0.05) * (152371.2 * 0.05)));
  graphicalEmbeddedObjectSizeRvs->SetAttribute ("Stream", IntegerValue (237284994));
  // Send about 15 embedded objects per request
  graphicalEmbeddedObjectAmountRvs->SetAttribute ("Mean", DoubleValue (15));
  graphicalEmbeddedObjectAmountRvs->SetAttribute ("Variance", DoubleValue ((15 * 0.05) *(15 * 0.05)));
  graphicalEmbeddedObjectAmountRvs->SetAttribute ("Stream", IntegerValue (3116224981));
  // Read time should be about one minute (i.e. 60 s).
  graphicalReadTimeRvs->SetAttribute ("Mean", DoubleValue (60));
  graphicalReadTimeRvs->SetAttribute ("Variance", DoubleValue ((60 * 0.05) * (60 * 0.05)));
  graphicalReadTimeRvs->SetAttribute ("Stream", IntegerValue (2397024192));
  // Amount of time it takes to parse HTML response
  graphicalParseTimeRvs->SetAttribute ("Mean", DoubleValue (0.5));
  graphicalParseTimeRvs->SetAttribute ("Variance", DoubleValue ((0.5 * 0.05) * (0.5 * 0.05)));
  graphicalParseTimeRvs->SetAttribute ("Stream", IntegerValue (2520801602));

  // Install instance of the graphical application
  graphicalServerHelper.Set ("Port", UintegerValue (serverPort));
  graphicalServerHelper.Set ("EmbeddedObjectIatRvs", PointerValue (graphicalEmbeddedObjectIatRvs));
  graphicalServerHelper.Set ("EmbeddedObjectSizeRvs", PointerValue (graphicalEmbeddedObjectSizeRvs));
  graphicalServerHelper.Set ("EmbeddedObjectAmountRvs", PointerValue (graphicalEmbeddedObjectAmountRvs));
  serverApps.Add (graphicalServerHelper.Install (GetTechnologyHelper ()->GetPublicServer ()));

  graphicalClientHelper.Set ("RemotePort", UintegerValue (serverPort));
  graphicalClientHelper.Set ("ReadTimeRvs", PointerValue (graphicalReadTimeRvs));
  graphicalClientHelper.Set ("ParseTimeRvs", PointerValue (graphicalParseTimeRvs));
  clientApps.Add (graphicalClientHelper.Install (ucNodes));

  serverPort++;

  for (uint32_t i = 0; i < clientApps.GetN (); i++)
    {
      if (m_testing)
        {
          clientApps.Get (i)->SetStartTime (Seconds (6 + startRndVar->GetValue ()));
          clientApps.Get (i)->SetStopTime (Seconds (95 - endRndVar->GetValue ()));
        }
      else
        {
          clientApps.Get (i)->SetStartTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time + Seconds (startRndVar->GetValue ()));
          clientApps.Get (i)->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E12_Incident_End).time - Seconds (endRndVar->GetValue ()));
        }
    }

  if (m_testing)
    {
      serverApps.Start (Seconds (5));
      serverApps.Stop (Seconds (100));
    }
  else
    {
      serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
      serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  // Add traces sources for senders and receivers
  m_webTx.Add (clientApps);
  m_webTx.Add (serverApps);
  m_webRx.Add (clientApps);
  m_webRx.Add (serverApps);
}

void SchoolShootingApplicationHelper::InitRmsBcaApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "RmsBca";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA]
      );

  // Send one packet every six seconds so that each computer performs 10 16 kB reads/writes per minute
  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (10)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (6)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    16384,
    16384);

  //Some information of each instance of the application
  Time startTime;
  Time stopTime;

  //Configure SWAT team applications
  Ptr<NormalRandomVariable> ucStartRndVar = nullptr;
  Ptr<NormalRandomVariable> saStartRndVar = nullptr;
  Ptr<NormalRandomVariable> endRndVar = nullptr;

  // Configure the start/stop times of the application
  if (m_testing)
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (3824051190));
    saStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (3802859310));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (90),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (961481328));
  }
  else
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3249174580));
    saStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E6_Perimeters_Ready).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3862066749));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
     "Variance", DoubleValue (12.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (2057014458));
  }

  // Get the required nodes
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;
  NodeContainer saNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes;

  // Install 5 instances of the application on the UC node
  for (uint16_t i = 0; i < 5; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (ucStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA,
                          m_genericApplicationHelper->Install (appConfig, ucNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }

  // Install 3 instances of the application on the SA node
  for (uint16_t i = 0; i < 3; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (saStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA,
                          m_genericApplicationHelper->Install (appConfig, saNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitSatelliteApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "Satellite";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE]
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (300)),   //Time between sessions
    4608,        //Client packet size (4.5 kB)
    4194304);       //Server packet size (4 MB)

  // Configure the start/stop times of the applcations.
  Time startTime;
  Time stopTime;
  Ptr<NormalRandomVariable> ucStartRndVar = nullptr;
  Ptr<NormalRandomVariable> endRndVar = nullptr;
  if (m_testing)
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (905420626));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (90),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (2228332951));
  }
  else
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3335729391));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
     "Variance", DoubleValue (12.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3383349015));
  }

  // Get the required nodes
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;

  // Install 2 instances on the UC node
  for (uint16_t i = 0; i < 2; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (ucStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());
      

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE,
                          m_genericApplicationHelper->Install (appConfig, ucNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPublicServer (),
                                                               GetTechnologyHelper ()->GetPublicServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitBuildingPlanApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "BuildingPlan";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS]
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (14700)),   //Time between sessions
    4608,        //Client packet size (4.5 kB)
    10485760);       //Server packet size (10 MB)

  // Configure the start/stop time of the application
  Time startTime;
  Time stopTime;
  Ptr<NormalRandomVariable> ucStartRndVar = nullptr;
  Ptr<NormalRandomVariable> endRndVar = nullptr;
  if (m_testing)
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (632905471));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (90),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (2354377938));
  }
  else
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (2195027868));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
     "Variance", DoubleValue (12.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (1995676912));
  }

  // Get the required nodes
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;

  // Install 2 instances on the UC node.
  for (uint16_t i = 0; i < 2; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (ucStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS,
                          m_genericApplicationHelper->Install (appConfig, ucNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPublicServer (),
                                                               GetTechnologyHelper ()->GetPublicServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitIncidentManagementApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "IncidentManagement";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT, appInfo);

  //Configure generic application to send updates to public safety server
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      appInfo.name,
      TcpSocketFactory::GetTypeId (),     //Socket type
      m_basePortsMap [SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT]
      );

  // Try to achieve 40 kbits/s UL and 128 kbits/s DL
  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (60)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0.05)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (0)),   //Time between sessions
    250,        //Client packet size (bytes)
    800);       //Server packet size (bytes)

  // Configure the start/stop time of the applications
  Time startTime;
  Time stopTime;
  Ptr<NormalRandomVariable> ucStartRndVar = nullptr;
  Ptr<NormalRandomVariable> saStartRndVar = nullptr;
  Ptr<NormalRandomVariable> endRndVar = nullptr;
  if (m_testing)
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (1217980281));
    saStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (5),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (3183181063));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
      "Mean", DoubleValue (90),
      "Variance", DoubleValue (1),
      "Bound", DoubleValue (1),
      "Stream", IntegerValue (2484113602));
  }
  else
  {
    ucStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E5_UC_Setup).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (2326917846));
    saStartRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E6_Perimeters_Ready).time.GetSeconds ()),
     "Variance", DoubleValue (6.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3441102793));
    endRndVar = CreateObjectWithAttributes<NormalRandomVariable> (
     "Mean", DoubleValue (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::E10_Incident_Secured).time.GetSeconds ()),
     "Variance", DoubleValue (12.25 * 60),
     "Bound", DoubleValue (10 * 60),
     "Stream", IntegerValue (3077454812));
  }

  // Get the required nodes
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;
  NodeContainer saNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes;

  // Install 1 instances on the UC node
  for (uint16_t i = 0; i < 1; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (ucStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT,
                          m_genericApplicationHelper->Install (appConfig, ucNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }

  // Install 1 instances on the SA node
  for (uint16_t i = 0; i < 1; i++)
    {
      //Get start/stop time of the instance
      //randomize start time so applications are not synchronized
      startTime = Seconds (saStartRndVar->GetValue ());
      stopTime = Seconds (endRndVar->GetValue ());

      AddPscApplications (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT,
                          m_genericApplicationHelper->Install (appConfig, saNodes.Get (0),
                                                               GetTechnologyHelper ()->GetPsServer (),
                                                               GetTechnologyHelper ()->GetPsServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                                               true,
                                                               startTime,
                                                               stopTime));
    }
}

void SchoolShootingApplicationHelper::InitNg911App ()
{
  NS_LOG_FUNCTION (this);
  
  NodeContainer ucNodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "NG911";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911, appInfo);

  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (3475554342));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_NG911];

  //Configure video sender
  Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
  // Attributes
  streamSrc->SetAttribute ("ReceiverAddress", AddressValue (ucNodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
  streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
  streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
  streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
      std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

  // Install the stream source on the server
  GetTechnologyHelper ()->GetPublicServer ()->AddApplication (streamSrc);

  // Configure the start/stop time of the stream sources
  if (m_testing)
    {
      streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
      streamSrc->SetStopTime (Seconds (95));
    }
  else
    {
      streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time.GetSeconds () + startVar->GetValue ()));
      streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }    

  // Setup a packet sink for the server to send packets to.
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
  ApplicationContainer streamSink = packetSinkHelper.Install (ucNodes.Get (0));

  // Set the start/stop time of the receiver node.
  if (m_testing)
    {
      streamSink.Start (Seconds (5));
      streamSink.Stop (Seconds (100));
    } 
  else
    {
      streamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
      streamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
    }

  // Add traces sources for senders and receivers for this application
  m_ng911Tx.Add (streamSrc);
  m_ng911Rx.Add (streamSink);
}

void SchoolShootingApplicationHelper::InitSchoolVideoApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "SchoolVideo";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO, appInfo);

  //Get all the sender nodes
  NodeContainer schoolCameras = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SCHOOL_CAMERAS).nodes;
  //
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (680931034));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_SCHOOL_VIDEO];

  for (uint16_t i = 0; i < schoolCameras.GetN (); i++)
    {
      //Configure the UDP echo server acting as media server
      psc::UdpGroupEchoServerHelper echoServer (serverPort);
      echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
      echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
      echoServer.SetAttribute ("EchoClient", BooleanValue (false));
      echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

      ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetSchoolServer ());

      // Set the start/stop time of the server application
      if (m_testing)
        {
          serverApps.Start (Seconds (5)); //start before client
          serverApps.Stop (Seconds (100));  //end after client
        }
      else
        {
          serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      //Configure video sender
      Ptr <PscVideoStreaming> streamSrc = CreateObject <PscVideoStreaming> ();
      // Attributes
      streamSrc->SetAttribute ("ReceiverAddress", AddressValue (GetTechnologyHelper ()->GetSchoolServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ()));
      streamSrc->SetAttribute ("ReceiverPort", UintegerValue (serverPort));
      streamSrc->SetAttribute ("MaxUdpPayloadSize", UintegerValue (1500));
      streamSrc->ReadCustomDistribution (std::string ("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetsize.txt_90_90"),
          std::string("src/psc/examples/video_cdf/CdfVideoStreaming-h264-1280x720-30-25-true_packetinterval.txt_90_90"));

      // Install the stream source on each camera
      schoolCameras.Get (i)->AddApplication (streamSrc);

      // Set the start/stop time of the receiver applications
      if (m_testing)
        {
          streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
          streamSrc->SetStopTime (Seconds (95));
        }
      else
        {
          streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time.GetSeconds () + startVar->GetValue ()));
          streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }    

      //Configure UC and SA as receiver nodes of the stream
      ApplicationContainer ucStreamSink;
      ApplicationContainer saStreamSink;
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
      ucStreamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
      saStreamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

      // Set the start/stop time of the receiver applications
      if (m_testing)
        {
          ucStreamSink.Start (Seconds (5));
          ucStreamSink.Stop (Seconds (100));
          saStreamSink.Start (Seconds (5));
          saStreamSink.Stop (Seconds (100));
        } 
      else
        {
          ucStreamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          ucStreamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
          saStreamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time);
          saStreamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      // Tell the group echo server where to forward the packets
      Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
      Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (ucSocket);
      Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (saSocket);

      // Add traces sources for senders and receivers for this application
      m_schoolCamerasTx.Add (streamSrc);
      m_schoolCamerasRx.Add (ucStreamSink);
      m_schoolCamerasRx.Add (saStreamSink);

      //Increment port for next instances
      serverPort++;
    }
}

void SchoolShootingApplicationHelper::InitTrafficVideoApp ()
{
  NS_LOG_FUNCTION (this);

  //Register application
  PscScenarioDefinition::ApplicationInfo appInfo;
  appInfo.name = "TrafficVideo";
  m_scenarioDefinitionHelper->GetScenarioDefinition()->AddApplication (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO, appInfo);

  //Get all the sender nodes
  NodeContainer trafficCameras = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TRAFFIC_CAMERAS).nodes;
  //
  //uniform random variable for scattering start of applications
  Ptr<UniformRandomVariable> startVar = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0.0),
      "Max", DoubleValue (0.1),
      "Stream", IntegerValue (2502803685));

  // Base port to use by applications
  uint16_t serverPort = m_basePortsMap[SCHOOL_SHOOTING_TRAFFIC_VIDEO];
  Address serverAddress = GetTechnologyHelper ()->GetPublicServer ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress ();
  for (uint16_t i = 0; i < trafficCameras.GetN (); i++)
    {
      //Configure the UDP echo server acting as media server
      psc::UdpGroupEchoServerHelper echoServer (serverPort);
      echoServer.SetAttribute ("Timeout", TimeValue (Seconds (0)));
      echoServer.SetAttribute ("Mode", StringValue ("InfSession"));
      echoServer.SetAttribute ("EchoClient", BooleanValue (false));
      echoServer.SetAttribute ("EchoPort", UintegerValue (serverPort));

      // Deploy the server application
      ApplicationContainer serverApps = echoServer.Install (GetTechnologyHelper ()->GetPublicServer ());

      // Set the start/stop time of the server applications
      if (m_testing)
        {
          serverApps.Start (Seconds (5)); //start before client
          serverApps.Stop (Seconds (100));  //end after client
        }
      else
        {
          serverApps.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          serverApps.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      Ptr<ConstantRandomVariable> onTimeRv = CreateObject<ConstantRandomVariable> ();
      Ptr<ConstantRandomVariable> offTimeRv = CreateObject<ConstantRandomVariable> ();

      onTimeRv->SetAttribute ("Constant", DoubleValue (1.0));
      offTimeRv->SetAttribute ("Constant", DoubleValue (0.0));

      //Configure video sender
      Ptr<OnOffApplication> streamSrc = CreateObject<OnOffApplication> ();
      // Attributes
      // 1152 bytes at 38.4 Kb/s acheives the inter packet arrival time of 0.24 s
      streamSrc->SetAttribute ("DataRate", StringValue ("38.4Kb/s"));
      streamSrc->SetAttribute ("PacketSize", UintegerValue (1152));
      streamSrc->SetAttribute ("Remote", AddressValue (Inet6SocketAddress (Ipv6Address::ConvertFrom (serverAddress), serverPort)));
      streamSrc->SetAttribute ("OnTime", PointerValue (onTimeRv));
      streamSrc->SetAttribute ("OffTime", PointerValue (offTimeRv));
      streamSrc->SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));

      // Install a video stream on each traffic camera
      trafficCameras.Get (i)->AddApplication (streamSrc);

      // Set the start/stop time of the sender applications
      if (m_testing)
        {
          streamSrc->SetStartTime (Seconds (6 + startVar->GetValue ()));
          streamSrc->SetStopTime (Seconds (95));
        }
      else
        {
          streamSrc->SetStartTime (Seconds (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time.GetSeconds () + startVar->GetValue ()));
          streamSrc->SetStopTime (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }    

      //Add receiver nodes (UC and SA)
      ApplicationContainer ucStreamSink;
      ApplicationContainer saStreamSink;
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", Inet6SocketAddress (Ipv6Address::GetAny (), serverPort));
      ucStreamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)));
      saStreamSink.Add (packetSinkHelper.Install (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)));

      // Set the start/stop time of the receiver applications.
      if (m_testing)
        {
          ucStreamSink.Start (Seconds (5));
          ucStreamSink.Stop (Seconds (100));
          saStreamSink.Start (Seconds (5));
          saStreamSink.Stop (Seconds (100));
        } 
      else
        {
          ucStreamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E5_UC_Setup).time);
          ucStreamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
          saStreamSink.Start (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E6_Perimeters_Ready).time);
          saStreamSink.Stop (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetKeyEventTime (SchoolShootingDefinitionHelper::KeyEvents::E12_Incident_End).time);
        }

      // Tell the group echo server where to forward the stream
      Ptr<UdpGroupEchoServer> echoServerApp = serverApps.Get (0)->GetObject <UdpGroupEchoServer>();
      Inet6SocketAddress ucSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (ucSocket);
      Inet6SocketAddress saSocket (m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes.Get (0)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (), serverPort);
      echoServerApp->AddClient (saSocket);

      // Add traces sources for senders and receivers for this application
      m_trafficCamerasTx.Add (streamSrc);
      m_trafficCamerasRx.Add (ucStreamSink);
      m_trafficCamerasRx.Add (saStreamSink);

      //Increment port for next instances
      serverPort++;
    }
}

void
SchoolShootingApplicationHelper::EnableApplication (SchoolSchootingApplicationId appName)
{
  NS_LOG_FUNCTION (this << appName);

  m_enabledApps.insert (appName);
}

void
SchoolShootingApplicationHelper::EnableApplicationTraces ()
{
  NS_LOG_FUNCTION (this);

  if (m_enabledApps.empty ())
    {
      EnableApplicationTraces (SCHOOL_SHOOTING_MCPTT);
      EnableApplicationTraces (SCHOOL_SHOOTING_TACTICAL_TELEMETRY);
      EnableApplicationTraces (SCHOOL_SHOOTING_BIOMETRICS);
      EnableApplicationTraces (SCHOOL_SHOOTING_SWAT_HELMET_CAMERA);
      EnableApplicationTraces (SCHOOL_SHOOTING_DEPLOYABLE_CAMERA);
      EnableApplicationTraces (SCHOOL_SHOOTING_THROW_PHONE);
      EnableApplicationTraces (SCHOOL_SHOOTING_HELICOPTER);
      EnableApplicationTraces (SCHOOL_SHOOTING_EMS_VEHICLES);
      EnableApplicationTraces (SCHOOL_SHOOTING_VIDEO_CONFERENCE);
      EnableApplicationTraces (SCHOOL_SHOOTING_AVL);
      EnableApplicationTraces (SCHOOL_SHOOTING_WEB);
      EnableApplicationTraces (SCHOOL_SHOOTING_RMS_BCA);
      EnableApplicationTraces (SCHOOL_SHOOTING_SATELLITE);
      EnableApplicationTraces (SCHOOL_SHOOTING_BUILDING_PLANS);
      EnableApplicationTraces (SCHOOL_SHOOTING_INCIDENT_MANAGEMENT);
      EnableApplicationTraces (SCHOOL_SHOOTING_NG911);
      EnableApplicationTraces (SCHOOL_SHOOTING_SCHOOL_VIDEO);
      EnableApplicationTraces (SCHOOL_SHOOTING_TRAFFIC_VIDEO);
    }
  else
    {
      for (auto enabledAppsIt = m_enabledApps.begin (); enabledAppsIt != m_enabledApps.end (); enabledAppsIt++)
        {
          EnableApplicationTraces (*enabledAppsIt);
        }
    }
}

void
SchoolShootingApplicationHelper::EnableApplicationTraces (SchoolSchootingApplicationId appName)
{
  NS_LOG_FUNCTION (this << appName);

  switch (appName)
    {
    case SCHOOL_SHOOTING_MCPTT:
      EnableMcpttTraces ();
      break;
    case SCHOOL_SHOOTING_TACTICAL_TELEMETRY:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_BIOMETRICS:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_SWAT_HELMET_CAMERA:
      m_traceHelper->EnableSwatHelmetCameraTraces (m_swatHelmetCamerasTx, m_swatHelmetCamerasRx);
      break;
    case SCHOOL_SHOOTING_DEPLOYABLE_CAMERA:
      m_traceHelper->EnableDeployableCameraTraces (m_deployableCamerasTx, m_deployableCamerasRx);
      break;
    case SCHOOL_SHOOTING_THROW_PHONE:
      m_traceHelper->EnablePscAppsTimeTrace (SCHOOL_SHOOTING_THROW_PHONE);
      m_traceHelper->EnablePscAppsTrafficTrace (SCHOOL_SHOOTING_THROW_PHONE);
      m_traceHelper->EnableThrowPhoneCameraTraces (m_throwPhoneCamerasTx, m_throwPhoneCamerasRx);
      break;
    case SCHOOL_SHOOTING_HELICOPTER:
      m_traceHelper->EnableHelicopterCameraTraces (m_helicopterCamerasTx, m_helicopterCamerasRx);
      break;
    case SCHOOL_SHOOTING_EMS_VEHICLES:
      m_traceHelper->EnableEmsVehicleCameraTraces (m_emsVehicleCamerasTx, m_emsVehicleCamerasRx);
      break;
    case SCHOOL_SHOOTING_VIDEO_CONFERENCE:
      m_traceHelper->EnablePscAppsTimeTrace (SCHOOL_SHOOTING_VIDEO_CONFERENCE);
      m_traceHelper->EnablePscAppsTrafficTrace (SCHOOL_SHOOTING_VIDEO_CONFERENCE);
      m_traceHelper->EnableVideoConferenceVideoTraces (m_videoConferenceVideoTx, m_videoConferenceVideoRx);
      break;
    case SCHOOL_SHOOTING_AVL:
      m_traceHelper->EnablePscAppsTimeTrace (SCHOOL_SHOOTING_AVL);
      m_traceHelper->EnablePscAppsTrafficTrace (SCHOOL_SHOOTING_AVL);
      break;
    case SCHOOL_SHOOTING_WEB:
      m_traceHelper->EnableWebTraces (m_webTx, m_webRx);
      break;
    case SCHOOL_SHOOTING_RMS_BCA:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_SATELLITE:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_BUILDING_PLANS:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_INCIDENT_MANAGEMENT:
      m_traceHelper->EnablePscAppsTimeTrace (appName);
      m_traceHelper->EnablePscAppsTrafficTrace (appName);
      break;
    case SCHOOL_SHOOTING_NG911:
      m_traceHelper->EnableNg911Traces (m_ng911Tx, m_ng911Rx);
      break;
    case SCHOOL_SHOOTING_SCHOOL_VIDEO:
      m_traceHelper->EnableSchoolCameraTraces (m_schoolCamerasTx, m_schoolCamerasRx);
      break;
    case SCHOOL_SHOOTING_TRAFFIC_VIDEO:
      m_traceHelper->EnableTrafficCameraTraces (m_trafficCamerasTx, m_trafficCamerasRx);
      break;
    default:
      NS_ABORT_MSG ("Invalid application name " << appName);
    }
}

void
SchoolShootingApplicationHelper::EnableMcpttTraces (void)
{
  m_mcpttTraceHelper->EnableMsgTraces ();
  m_mcpttTraceHelper->EnableStateMachineTraces ();
  m_mcpttTraceHelper->EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  m_mcpttTraceHelper->EnableAccessTimeTrace ("mcptt-access-time.txt");
  m_mcpttTracesEnabled = true;
}

void
SchoolShootingApplicationHelper::AddPscApplication (SchoolSchootingApplicationId name, Ptr<PscApplication> application)
{
  NS_LOG_FUNCTION (this);
  ApplicationContainer c;
  c.Add (application);
  AddPscApplications (name, c);
}

void
SchoolShootingApplicationHelper::AddPscApplications (SchoolSchootingApplicationId name, ApplicationContainer applications)
{
  NS_LOG_FUNCTION (this);
  std::map <SchoolSchootingApplicationId, ApplicationContainer>::iterator it = m_pscApplicationsMap.find (name);
  if (it == m_pscApplicationsMap.end ())
    {
      //new type of application, just insert the container
      m_pscApplicationsMap.insert (std::pair<SchoolSchootingApplicationId, ApplicationContainer> (name, applications));
    }
  else
    {
      it->second.Add (applications);
    }
}

std::set <SchoolShootingApplicationHelper::SchoolSchootingApplicationId> 
SchoolShootingApplicationHelper::GetEnabledApplications ()
{
  return m_enabledApps;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetPscApplications (SchoolSchootingApplicationId name)
{
  ApplicationContainer c;
  std::map <SchoolSchootingApplicationId, ApplicationContainer>::iterator it = m_pscApplicationsMap.find (name);
  if (it != m_pscApplicationsMap.end ())
    {
      c = it->second;
    }
  return c;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetSwatHelmetCamerasTxApplications ()
{
  return m_swatHelmetCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetSwatHelmetCamerasRxApplications ()
{
  return m_swatHelmetCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetDeployableCamerasTxApplications ()
{
  return m_deployableCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetDeployableCamerasRxApplications ()
{
  return m_deployableCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetThrowPhoneCamerasTxApplications ()
{
  return m_throwPhoneCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetThrowPhoneCamerasRxApplications ()
{
  return m_throwPhoneCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetHelicopterCamerasTxApplications ()
{
  return m_helicopterCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetHelicopterCamerasRxApplications ()
{
  return m_helicopterCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetEmsVehicleCamerasTxApplications ()
{
  return m_emsVehicleCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetEmsVehicleCamerasRxApplications ()
{
  return m_emsVehicleCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetVideoConferenceVideoTxApplications ()
{
  return m_videoConferenceVideoTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetVideoConferenceVideoRxApplications ()
{
  return m_videoConferenceVideoRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetSchoolCamerasTxApplications ()
{
  return m_schoolCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetSchoolCamerasRxApplications ()
{
  return m_schoolCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetTrafficCamerasTxApplications ()
{
  return m_trafficCamerasTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetTrafficCamerasRxApplications ()
{
  return m_trafficCamerasRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetWebTxApplications ()
{
  return m_webTx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetWebRxApplications ()
{
  return m_webRx;
}

ApplicationContainer
SchoolShootingApplicationHelper::GetNg911TxApplications ()
{
  return m_ng911Tx;
}


ApplicationContainer
SchoolShootingApplicationHelper::GetNg911RxApplications ()
{
  return m_ng911Rx;
}

} // namespace psc
} // namespace ns3
