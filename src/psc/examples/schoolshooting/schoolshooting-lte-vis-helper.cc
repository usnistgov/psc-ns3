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

#include <algorithm>
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/vector.h"
#include "ns3/mobility-model.h"
#include "ns3/simulator.h"
#include "ns3/object-map.h"
#include "ns3/lte-enb-net-device.h"

#include "schoolshooting-lte-helper.h"
#include "schoolshooting-lte-vis-helper.h"

#ifdef HAS_NETSIMULYZER

namespace ns3 {
namespace psc {

NS_LOG_COMPONENT_DEFINE ("SchoolShootingLteVisHelper");
NS_OBJECT_ENSURE_REGISTERED (SchoolShootingLteVisHelper);

using netsimulyzer::OptionalValue;
using netsimulyzer::Color3;

SchoolShootingLteVisHelper::SchoolShootingLteVisHelper () :
  m_scenarioHelper (nullptr),
  m_accessTimesCollection (nullptr)
{
  NS_LOG_FUNCTION (this);
}

SchoolShootingLteVisHelper::SchoolShootingLteVisHelper (Ptr<PscScenarioDefinition> scenarioDefinition) :
m_scenarioHelper (nullptr),
m_accessTimesCollection (nullptr)
{
  NS_LOG_FUNCTION (this << scenarioDefinition);
  m_scenarioDefinition = scenarioDefinition;

  m_orchestrator = CreateObject<netsimulyzer::Orchestrator> (m_scenarioDefinition->GetName () + ".json");

  InitBuildings ();
  InitAreas ();
  InitNodeModels ();
}

SchoolShootingLteVisHelper::SchoolShootingLteVisHelper (Ptr<SchoolShootingApplicationHelper> scenarioHelper) :
m_accessTimesCollection (nullptr)
{
  NS_LOG_FUNCTION (this << scenarioHelper);

  m_scenarioDefinition = scenarioHelper->GetScenarioDefinition ();
  m_scenarioHelper = scenarioHelper;
  m_orchestrator = CreateObject<netsimulyzer::Orchestrator> (m_scenarioDefinition->GetName () + ".json");

  InitBuildings ();
  InitAreas ();
  InitNodeModels ();
}


SchoolShootingLteVisHelper::~SchoolShootingLteVisHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_scenarioHelper = nullptr;
  m_orchestrator = nullptr;
}

TypeId
SchoolShootingLteVisHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::SchoolShootingLteVisHelper")
    .SetParent<Object> ()
    .AddConstructor<SchoolShootingLteVisHelper> ()
    .AddAttribute ("Interval", "Time between updates", TimeValue (Seconds (1)),
                   MakeTimeAccessor (&SchoolShootingLteVisHelper::SetInterval), MakeTimeChecker ())
  ;

  return tid;
}

Ptr<netsimulyzer::Orchestrator>
SchoolShootingLteVisHelper::GetOrchestrator ()
{
  return m_orchestrator;
}

void
SchoolShootingLteVisHelper::SetOrchestratorAttribute (std::string n, const AttributeValue &v)
{
  m_orchestrator->SetAttribute (n, v);
}

void
SchoolShootingLteVisHelper::SetInterval (Time interval)
{
  NS_ABORT_MSG_UNLESS (interval.GetSeconds () > 0, "Needs interval greater than 0");
  m_interval = interval;
}

void
SchoolShootingLteVisHelper::InitBuildings ()
{
  // Only explicitly configured items will be shown
  // so, even if we don't have options to set
  // the buildings must be configured
  netsimulyzer::BuildingConfigurationHelper buildingConfigHelper (m_orchestrator);

  std::map<PscScenarioDefinition::StructureId, PscScenarioDefinition::StructureInfo> structures = m_scenarioDefinition->GetStructures ();
  std::map<PscScenarioDefinition::StructureId, PscScenarioDefinition::StructureInfo>::iterator it;

  for (it = structures.begin (); it != structures.end (); it++)
    {
      PscScenarioDefinition::StructureInfo info = it->second;
      switch (it->first)
        {
        case SchoolShootingDefinitionHelper::SCHOOL_BLD:
          buildingConfigHelper.Set ("Color", netsimulyzer::GRAY_20_VALUE);
          break;
        case SchoolShootingDefinitionHelper::CLASSROOMS_BLD:
          buildingConfigHelper.Set ("Color", netsimulyzer::BLUE_VALUE);
          break;
        default:
          NS_ABORT_MSG ("Unknown building type");
        }
      buildingConfigHelper.Install (info.buildings);
    }

}

void
SchoolShootingLteVisHelper::InitAreas ()
{
  std::map<PscScenarioDefinition::AreaId, PscScenarioDefinition::AreaInfo> areas = m_scenarioDefinition->GetAreas ();
  std::map<PscScenarioDefinition::AreaId, PscScenarioDefinition::AreaInfo>::iterator it;


for (it = areas.begin (); it != areas.end (); it++)
    {
      PscScenarioDefinition::AreaInfo info = it->second;
      auto areaLocation = CreateObject<netsimulyzer::RectangularArea> (m_orchestrator, info.area);

      // Identify the area
      areaLocation->SetAttribute ("Name", StringValue (info.name));

      // Move the area indicator slightly below the feet of the Nodes
      areaLocation->SetAttribute ("Height", DoubleValue (0));

      // Border
      areaLocation->SetAttribute ("Fill", EnumValue(netsimulyzer::RectangularArea::DrawMode::Hidden));

      switch (it->first) 
      {
        case SchoolShootingDefinitionHelper::OUTER_PERIMETER_AREA:
          //brown-ish
          areaLocation->SetAttribute ("BorderColor", netsimulyzer::ORANGE_VALUE);
          break;
        case SchoolShootingDefinitionHelper::INNER_PERIMETER_AREA:
          //red-ish
          areaLocation->SetAttribute ("BorderColor", netsimulyzer::RED_VALUE);
          break;
        case SchoolShootingDefinitionHelper::STAGING_AREA:
          //green-ish
          areaLocation->SetAttribute ("BorderColor", netsimulyzer::GREEN_VALUE);
          break;
        case SchoolShootingDefinitionHelper::UNIFIED_COMMAND_AREA:
          //blue-ish
          areaLocation->SetAttribute ("BorderColor", netsimulyzer::BLUE_VALUE);
          break;
        default:
          //leave default color
          break;
      }
    }
}

void
SchoolShootingLteVisHelper::InitNodeModels ()
{
  netsimulyzer::NodeConfigurationHelper nodeConfigHelper (m_orchestrator);
  PscScenarioDefinition::NodeGroupInfo nodeInfo;
  
  nodeConfigHelper.Set ("Scale", DoubleValue (1.0));

  // NOTE: When setting a "Model", if the object is not found on the
  // filesystem, then there is a fallback model loaded that looks like an
  // exclamation mark.
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Swat_Male.obj"));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (1.8));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, -1.5)));
  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SWAT_TEAM_1);
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);
  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SWAT_TEAM_2);
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);
  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SWAT_TEAM_3);
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);
  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SWAT_TEAM_4);
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::TASK_FORCE);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Police_Male.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::UC_TEAM);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Firefighter.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::OFFENDER);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Gangster.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::EMS);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Paramedic.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, 0)));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SCHOOL_CAMERAS);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Security_Camera.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (0.5));
  NodeContainer schoolcameras = nodeInfo.nodes;
  nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 180)));
  nodeConfigHelper.Install (schoolcameras.Get(0));
  nodeConfigHelper.Install (schoolcameras.Get(1));
  nodeConfigHelper.Install (schoolcameras.Get(3));
  nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 90)));
  nodeConfigHelper.Install (schoolcameras.Get(2));
  nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, -90)));
  nodeConfigHelper.Install (schoolcameras.Get(4));
  //reset orientation
  nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 0)));

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::TRAFFIC_CAMERAS);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Security_Camera.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (0.5));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::HELICOPTER);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Police_Helicopter.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (3.0));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::TASK_FORCE_VEHICLES);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Police_Car_Heist.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (1.5));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, -1)));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::UC_TEAM_VEHICLE);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Bus.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (3.0));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, -3)));
  nodeConfigHelper.Install (nodeInfo.nodes);


  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::SA_VEHICLE);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Swat_Van.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (3.0));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, -3)));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::FIRE_VEHICLES);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Firetruck.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (3.0));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, -3)));
  nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 90)));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::EMS_VEHICLES);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/vehicle/Ambulance.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (2.7));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D(0, 0, 0)));
  nodeConfigHelper.Install (nodeInfo.nodes);

  nodeInfo = m_scenarioDefinition->GetNodesForGroup (SchoolShootingDefinitionHelper::DEPLOYABLE_CAMERA);
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Security_Camera.obj"));
  nodeConfigHelper.Set ("Name", StringValue (nodeInfo.name));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (0.5));
  nodeConfigHelper.Install (nodeInfo.nodes);

  //networking elements
  if (!m_scenarioHelper)
    return;
  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Game_Console.obj"));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (1.0));
  nodeConfigHelper.Set ("Name", StringValue ("PS application server"));
  nodeConfigHelper.Install (m_scenarioHelper->GetTechnologyHelper ()->GetPsServer ());
  nodeConfigHelper.Set ("Name", StringValue ("Public server"));
  nodeConfigHelper.Install (m_scenarioHelper->GetTechnologyHelper ()->GetPublicServer ());
  nodeConfigHelper.Set ("Name", StringValue ("School server"));
  nodeConfigHelper.Install (m_scenarioHelper->GetTechnologyHelper ()->GetSchoolServer ());
  nodeConfigHelper.Set ("Name", StringValue ("City server"));
  nodeConfigHelper.Install (m_scenarioHelper->GetTechnologyHelper ()->GetCityServer ());

  Ptr<SchoolShootingLteHelper> lteHelper =
      m_scenarioHelper->GetTechnologyHelper ()->GetObject<SchoolShootingLteHelper> ();

  if (!lteHelper)
    return;
  NodeContainer tmp = lteHelper->GetEnodeBs ();

  Ptr<netsimulyzer::Decoration> pole = CreateObject<netsimulyzer::Decoration> (m_orchestrator);
  pole->SetAttribute ("Model", StringValue ("non-distributable/models/props/Billboard_pole.obj"));
  pole->SetAttribute ("Height", OptionalValue<double> (32.0));
  const auto mobility = tmp.Get (0)->GetObject<MobilityModel> ();
  auto position = mobility->GetPosition ();
  position.z = 0;
  pole->SetAttribute ("Position", Vector3DValue (position));

  nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Cylinder.obj"));
  nodeConfigHelper.Set ("Name", StringValue ("eNodeB antenna"));
  nodeConfigHelper.Set ("Height", OptionalValue<double> (3.0));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D (0, 2, 0)));
  //nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 0)));
  nodeConfigHelper.Install (tmp.Get (0));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D (2 * 0.886, 2 * -0.5, 0)));
  //nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 120)));
  nodeConfigHelper.Install (tmp.Get (1));
  nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D (2 * -0.886, 2 * -0.5, 0)));
  //nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D(0, 0, 240)));
  nodeConfigHelper.Install (tmp.Get (2));

  nodeConfigHelper.Set ("Model", StringValue ("models/simple/diamond.obj"));
  nodeConfigHelper.Set ("BaseColor", OptionalValue<Color3> (ns3::netsimulyzer::BLUE));
  nodeConfigHelper.Set ("Name", StringValue ("SGW"));
  nodeConfigHelper.Install (lteHelper->GetSgw ());

  nodeConfigHelper.Set ("BaseColor", OptionalValue<Color3> (ns3::netsimulyzer::ORANGE));
  nodeConfigHelper.Set ("Name", StringValue ("PGW"));
  nodeConfigHelper.Install (lteHelper->GetPgw ());
  
}

void
SchoolShootingLteVisHelper::EnableApplicationGraphs ()
{
  NS_LOG_FUNCTION (this);
  std::set <SchoolShootingApplicationHelper::SchoolSchootingApplicationId> enabledApps = m_scenarioHelper->GetEnabledApplications ();

  if (enabledApps.size() == 0)
  {
    //all apps are enabled
    for (uint16_t appId = SchoolShootingApplicationHelper::SCHOOL_SHOOTING_MCPTT; appId < SchoolShootingApplicationHelper::SCHOOL_SHOOTING_LAST; appId++)
    {
      EnableApplicationGraphs ((SchoolShootingApplicationHelper::SchoolSchootingApplicationId) appId);
    }
  } 
  else
  {
    std::set <SchoolShootingApplicationHelper::SchoolSchootingApplicationId>::iterator enabledAppsIt;
    for (enabledAppsIt = enabledApps.begin () ; enabledAppsIt != enabledApps.end () ; enabledAppsIt++)
    {
      EnableApplicationGraphs (*enabledAppsIt);
    }
  }
}

void
SchoolShootingLteVisHelper::EnableApplicationGraphs (SchoolShootingApplicationHelper::SchoolSchootingApplicationId appName)
{
  NS_LOG_FUNCTION (this << appName);
  NS_ABORT_MSG_UNLESS (m_scenarioHelper !=nullptr, "Cannot enable application trace if network not deployed");

  switch (appName)
    {
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_MCPTT:
      EnableMcpttGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TACTICAL_TELEMETRY:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BIOMETRICS:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA:
      EnableSwatHelmetCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA:
      EnableDeployableCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE:
      EnablePscAppsTrafficGraphs (appName);
      EnableThrowPhoneCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER:
      EnableHelicopterCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES:
      EnableEmsVehicleCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE:
      EnablePscAppsTrafficGraphs (appName);
      EnableVideoConferenceVideoGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_AVL:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB:
      EnableWebGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_RMS_BCA:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SATELLITE:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_BUILDING_PLANS:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_INCIDENT_MANAGEMENT:
      EnablePscAppsTrafficGraphs (appName);
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911:
      EnableNg911Graphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO:
      EnableSchoolCameraGraphs ();
      break;
    case SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO:
      EnableTrafficCameraGraphs ();
      break;
    default:
      NS_ABORT_MSG ("Invalid application name " << appName);
    }
}

void
SchoolShootingLteVisHelper::EnableKeyEventLog ()
{
  NS_LOG_FUNCTION (this);

  auto eventLog = CreateObject<netsimulyzer::LogStream> (m_orchestrator);
  eventLog->SetAttribute ("Name", StringValue ("Key events"));

  //customize color
  eventLog->SetAttribute("Color", netsimulyzer::OptionalValue<Color3>{100u, 150u, 100u});

  //Retrieve the list of events and schedule the log
  std::map<PscScenarioDefinition::KeyEventId, PscScenarioDefinition::KeyEventInfo> events = m_scenarioDefinition->GetKeyEvents ();
  std::map<PscScenarioDefinition::KeyEventId, PscScenarioDefinition::KeyEventInfo>::iterator eventIt;
  for (eventIt = events.begin (); eventIt != events.end (); eventIt++)
  {
    std::string text = std::to_string (eventIt->second.time.GetMinutes ());
    text += " min: " + eventIt->second.name + "\n";
    Simulator::Schedule (eventIt->second.time, &netsimulyzer::LogStream::Write, eventLog, text);
  }

}

void
SchoolShootingLteVisHelper::EnableSwatHelmetCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetSwatHelmetCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetSwatHelmetCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid Helmet camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("SWAT Helmet video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "SWAT Helmet Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("Color", netsimulyzer::RED_VALUE);
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxSwatHelmetCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "SWAT Helmet Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          if (j==0)
          {
            txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("Color", netsimulyzer::GREEN_VALUE);
          } 
          else
          {
            txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("Color", netsimulyzer::BLUE_VALUE);
          }
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxSwatHelmetCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_helmetCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxSwatHelmetCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxSwatHelmetCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableDeployableCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetDeployableCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetDeployableCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid deployable camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Deployable Camera video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Deployable Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxDeployableCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Deployable Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxDeployableCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_deployableCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxDeployableCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxDeployableCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableThrowPhoneCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetThrowPhoneCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetThrowPhoneCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid throw phone camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Throw phone video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Throw Phone Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxThrowPhoneCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Throw Phone Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxThrowPhoneCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_throwPhoneCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxThrowPhoneCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxThrowPhoneCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableHelicopterCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetHelicopterCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetHelicopterCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid Helicopterera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Helicopter video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Helicopter Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxHelicopterCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Helicopter Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxHelicopterCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_helicopterCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxHelicopterCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxHelicopterCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableEmsVehicleCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetEmsVehicleCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetEmsVehicleCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid EMS vehicle camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("EMS vehicle video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "EMS Vehicle Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxEmsVehicleCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "EMS Vehicle Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxEmsVehicleCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_emsVehicleCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxEmsVehicleCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxEmsVehicleCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableVideoConferenceVideoGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetVideoConferenceVideoTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetVideoConferenceVideoRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () == 2 && rxApps.GetN () == 2, "Invalid video conference video configuration. Expecting 1 receiver and 1 transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and Rx together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Video conference video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Video Conference Video TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      txSink->SetAttribute ("Unit", StringValue ("kb/s"));
      txSink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      txSink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxVideoConferenceVideo, txSink));
      collection->Add (txSink->GetSeries ());

      //Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Video Conference Video RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get ((i + 1) % txApps.GetN ())->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get ((i + 1) % txApps.GetN ())->GetNode ()->GetId ()));
      rxSink->SetAttribute ("Unit", StringValue ("kb/s"));
      rxSink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue rxXySeries;
      rxSink->GetAttribute ("XYSeries", rxXySeries);
      rxXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      rxApps.Get ((i + 1) % txApps.GetN ())->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxVideoConferenceVideo, rxSink));
      collection->Add (rxSink->GetSeries ());
      //Add to list of collections
      m_videoConferenceVideoCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxVideoConferenceVideo (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxVideoConferenceVideo (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableSchoolCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetSchoolCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetSchoolCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid school camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("School camera video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "School Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxSchoolCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "School Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxSchoolCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_schoolCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxSchoolCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxSchoolCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableTrafficCameraGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetTrafficCamerasTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetTrafficCamerasRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () * 2 == rxApps.GetN (), "Invalid traffic camera configuration. Expecting 2 receivers per transmitter");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Traffic camera video (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Traffic Camera TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxTrafficCamera, sink));
      collection->Add (sink->GetSeries ());

      for (uint16_t j = 0; j < 2; j++)
        {
          //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Traffic Camera RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()) + ")");
          Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (2*i+j)->GetNode ()->GetId ()));
          sink->SetAttribute ("Unit", StringValue ("kb/s"));
          sink->SetAttribute ("Interval", TimeValue (m_interval));
          PointerValue txXySeries;
          sink->GetAttribute ("XYSeries", txXySeries);
          txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
          rxApps.Get (2*i+j)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxTrafficCamera, sink));
          collection->Add (sink->GetSeries ());
        }
      //Add to list of collections
      m_trafficCameraCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxTrafficCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxTrafficCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableWebGraphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetWebTxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetWebRxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () == 4 && rxApps.GetN () == 4, "Invalid web configuration. Expecting 8 receivers and 8 transmitters");
  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue("Web (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Web TX (node " + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      txSink->SetAttribute ("Unit", StringValue ("kb/s"));
      txSink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      txSink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxWeb, txSink));
      collection->Add (txSink->GetSeries ());

      //Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "Web RX (" + std::to_string (txApps.Get (i)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get ((i + 2) % txApps.GetN ())->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get ((i + 2) % txApps.GetN ())->GetNode ()->GetId ()));
      rxSink->SetAttribute ("Unit", StringValue ("kb/s"));
      rxSink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue rxXySeries;
      rxSink->GetAttribute ("XYSeries", rxXySeries);
      rxXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      rxApps.Get ((i + 2) % txApps.GetN ())->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxWeb, rxSink));
      collection->Add (rxSink->GetSeries ());
      //Add to list of collections
      m_webCollections.push_back (collection);
    }
}

void SchoolShootingLteVisHelper::TraceTxWeb (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxWeb (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnableNg911Graphs ()
{
  NS_LOG_FUNCTION (this);
  
  ApplicationContainer txApps = m_scenarioHelper->GetNg911TxApplications ();
  ApplicationContainer rxApps = m_scenarioHelper->GetNg911RxApplications ();
  NS_ABORT_MSG_UNLESS (txApps.GetN () == 1 && rxApps.GetN () == 1, "Invalid NG911 configuration. Expecting 1 receiver and 1 transmitter");
  //Create a collection with the Tx and  Rx together
  PointerValue xAxis; 
  PointerValue yAxis;
  Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
  collection->SetAttribute ("AutoColor", BooleanValue (true));
  collection->SetAttribute ("Name", StringValue("NG911 video (node " + std::to_string (txApps.Get (0)->GetNode ()->GetId ()) + ")"));
  collection->GetAttribute ("XAxis", xAxis);
  xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
  collection->GetAttribute ("YAxis", yAxis);
  yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

  //Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "NG911 TX (node " + std::to_string (txApps.Get (0)->GetNode ()->GetId ()) + ")");
  Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
  txSink->SetAttribute ("Unit", StringValue ("kb/s"));
  txSink->SetAttribute ("Interval", TimeValue (m_interval));
  PointerValue txXySeries;
  txSink->GetAttribute ("XYSeries", txXySeries);
  txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
  txApps.Get (0)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxNg911, txSink));
  collection->Add (txSink->GetSeries ());

  //Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "NG911 RX (" + std::to_string (txApps.Get (0)->GetNode ()->GetId ()) + "->" + std::to_string (rxApps.Get (0)->GetNode ()->GetId ()) + ")");
  Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX " + std::to_string (rxApps.Get (0)->GetNode ()->GetId ()));
  rxSink->SetAttribute ("Unit", StringValue ("kb/s"));
  rxSink->SetAttribute ("Interval", TimeValue (m_interval));
  PointerValue rxXySeries;
  rxSink->GetAttribute ("XYSeries", rxXySeries);
  rxXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
  rxApps.Get (0)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxNg911, rxSink));
  collection->Add (rxSink->GetSeries ());
  //Add to list of collections
  m_ng911Collections.push_back (collection);
}

void SchoolShootingLteVisHelper::TraceTxNg911 (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void SchoolShootingLteVisHelper::TraceRxNg911 (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (sink);
  sink->AddPacketSize (p->GetSize ());
}

void
SchoolShootingLteVisHelper::EnablePscAppsTrafficGraphs (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name)
{
  NS_LOG_FUNCTION (this);

  PscScenarioDefinition::ApplicationInfo ai = m_scenarioDefinition->GetApplication (name);
  ApplicationContainer apps = m_scenarioHelper->GetPscApplications (name);

  if (apps.GetN() == 0)
  {
    NS_LOG_DEBUG ("No application " + ai.name + " found. It was either not enabled or it is not a PscApplication");
  }
  for (uint32_t i = 0; i < apps.GetN (); i++)
    {
      //Create a collection with the Tx and 2 Rxs together
      PointerValue xAxis; 
      PointerValue yAxis;
      Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
      collection->SetAttribute ("AutoColor", BooleanValue (true));
      collection->SetAttribute ("Name", StringValue(ai.name + " (node " + std::to_string (apps.Get (i)->GetNode ()->GetId ()) + ")"));
      collection->GetAttribute ("XAxis", xAxis);
      xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
      collection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

      //Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, ai.name + " TX (node " + std::to_string (apps.Get (i)->GetNode ()->GetId ()) + ")");
      Ptr<netsimulyzer::ThroughputSink> sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue txXySeries;
      sink->GetAttribute ("XYSeries", txXySeries);
      txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceAppRxTraffic, sink));
      collection->Add (sink->GetSeries ());

      //sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, ai.name + " RX (node " + std::to_string (apps.Get (i)->GetNode ()->GetId ()) + ")");
      sink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX");
      sink->SetAttribute ("Unit", StringValue ("kb/s"));
      sink->SetAttribute ("Interval", TimeValue (m_interval));
      PointerValue rxXySeries;
      sink->GetAttribute ("XYSeries", rxXySeries);
      rxXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceAppTxTraffic, sink));
      collection->Add (sink->GetSeries ());
      //Add to list of collections
      m_pscApplicationCollections.push_back (collection);
    }
}

void
SchoolShootingLteVisHelper::TraceAppTxTraffic (Ptr<netsimulyzer::ThroughputSink> sink, std::string appName, SeqTsSizeHeader header)
{
  NS_LOG_FUNCTION (sink << appName << header);
  sink->AddPacketSize (header.GetSize ());
}

void
SchoolShootingLteVisHelper::TraceAppRxTraffic (Ptr<netsimulyzer::ThroughputSink> sink, std::string appName, SeqTsSizeHeader header)
{
  NS_LOG_FUNCTION (sink << appName << header);
  sink->AddPacketSize (header.GetSize ());
}

void
SchoolShootingLteVisHelper::EnableMcpttGraphs ()
{
  NS_LOG_FUNCTION (this);

  PointerValue xAxis; 
  PointerValue yAxis;

  //Access time
  m_accessTimesCollection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
  m_accessTimesCollection->SetAttribute ("AutoColor", BooleanValue (true));
  m_accessTimesCollection->SetAttribute ("Name", StringValue("Access Time"));
  m_accessTimesCollection->GetAttribute ("XAxis", xAxis);
  xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
  m_accessTimesCollection->GetAttribute ("YAxis", yAxis);
  yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Access Time (ms)"));

  //Mouth-to-ear latency
  m_m2eLatencyCollection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
  m_m2eLatencyCollection->SetAttribute ("AutoColor", BooleanValue (true));
  m_m2eLatencyCollection->SetAttribute ("Name", StringValue("M2E Latency"));
  m_m2eLatencyCollection->GetAttribute ("XAxis", xAxis);
  xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
  m_m2eLatencyCollection->GetAttribute ("YAxis", yAxis);
  yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("M2E Latency (ms)"));

  std::map<uint16_t,  McpttCallInfo>::iterator callIt;
  for (uint16_t callId = 1; callId <= 6; callId++)
  {
    //Create new call info
    McpttCallInfo callInfo;
    callInfo.callId = callId;
    std::pair<std::map<uint16_t,McpttCallInfo>::iterator,bool> ret;
    ret = m_callInfoByCallId.insert (std::make_pair(callId, callInfo));
    callIt = ret.first;

    //Access time
    callIt->second.accessTimeGraph = CreateObject <netsimulyzer::XYSeries>(m_orchestrator);
    //callIt->second.accessTimeGraph->SetAttribute ("Name", StringValue("Call ID " + std::to_string(callId) + " Access time"));
    callIt->second.accessTimeGraph->SetAttribute ("Name", StringValue("Call ID " + std::to_string(callId)));
    callIt->second.accessTimeGraph->SetAttribute ("LabelMode", StringValue("Hidden"));
    callIt->second.accessTimeGraph->SetAttribute ("Connection", StringValue("None"));

    //Mouth-to-ear latency
    callIt->second.m2eLatencyGraph = CreateObject <netsimulyzer::XYSeries>(m_orchestrator);
    //callIt->second.m2eLatencyGraph->SetAttribute ("Name", StringValue("Call ID " + std::to_string(callId) + " M2E Latency"));
    callIt->second.m2eLatencyGraph->SetAttribute ("Name", StringValue("Call ID " + std::to_string(callId)));
    callIt->second.m2eLatencyGraph->SetAttribute ("LabelMode", StringValue("Hidden"));
    callIt->second.m2eLatencyGraph->SetAttribute ("Connection", StringValue("None"));

    //Add graph to series
    m_accessTimesCollection->Add (callIt->second.accessTimeGraph);
    m_m2eLatencyCollection->Add (callIt->second.m2eLatencyGraph);
  }

  //Connect to trace sources
  Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&SchoolShootingLteVisHelper::TraceAccessTime, this));
  Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/RxTrace", MakeCallback (&SchoolShootingLteVisHelper::TraceMouthToEarLatency, this));
}

void
SchoolShootingLteVisHelper::TraceAccessTime (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName)
{
  NS_LOG_FUNCTION (this << userId << callId);

  std::map<uint16_t,  McpttCallInfo>::iterator callIt;
  callIt = m_callInfoByCallId.find (callId);

  if (callIt != m_callInfoByCallId.end ())
    {
      auto it = callIt->second.accessTimeMap.find (userId);

      if (it == callIt->second.accessTimeMap.end ())
        {
          if ((oldStateName == "'Start-stop'" || oldStateName == "'U: has no permission'") &&
              newStateName == "'U: pending Request'")
            {
              std::pair<Time, std::string> item = std::make_pair (Simulator::Now (), newStateName);
              callIt->second.accessTimeMap.insert (std::make_pair (userId, item));
            }
        }
      else
        {
          if (it->second.second == "'U: pending Request'")
            {
              if (newStateName == "'U: queued'")
                {
                  it->second.second = newStateName;
                  return;
                }
              else if (newStateName == "'U: has permission'")
                {
                  callIt->second.accessTimeGraph->Append (Simulator::Now ().GetSeconds(), (Simulator::Now () - it->second.first).GetMilliSeconds ());
                  callIt->second.accessTimeMap.erase (userId);
                  return;
                }
              else if (newStateName == "'U: has no permission'" || newStateName == "'Releasing'" || newStateName == "'U: pending Release'")
                {
                  callIt->second.accessTimeMap.erase (userId);
                  return;
                }
            }
          else if (it->second.second == "'U: queued'")
            {
              if (newStateName == "'U: has permission'")
                {
                  callIt->second.accessTimeGraph->Append (Simulator::Now ().GetSeconds(), (Simulator::Now () - it->second.first).GetMilliSeconds ());
                  callIt->second.accessTimeMap.erase (userId);
                  return;
                }
              else if (newStateName == "'U: has no permission'" || newStateName == "'Releasing'" || newStateName == "'U: pending Release'")
                {
                  callIt->second.accessTimeMap.erase (userId);
                  return;
                }
            }
        }
    }
}

void
SchoolShootingLteVisHelper::TraceMouthToEarLatency (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << app << callId);

  std::map<uint16_t,  McpttCallInfo>::iterator callIt;
  callIt = m_callInfoByCallId.find (callId);

  if (callIt == m_callInfoByCallId.end ())
    return;

  if (headerType == McpttMediaMsg::GetTypeId ())
    {
      McpttMediaMsg msg;
      pkt->PeekHeader (msg);
      Time talkSpurtStart = msg.GetTalkSpurtStart ();
      auto it = callIt->second.m2eLatencyMap.find (app->GetNode ()->GetId ());
      if (it == callIt->second.m2eLatencyMap.end ())
        {
          callIt->second.m2eLatencyMap.insert (std::make_pair (app->GetNode ()->GetId (), talkSpurtStart));
          callIt->second.m2eLatencyGraph->Append (Simulator::Now ().GetSeconds(), (Simulator::Now () - talkSpurtStart).GetMilliSeconds ());
        }
      else
        {
          if (it->second < talkSpurtStart)
            {
              it->second = talkSpurtStart;
              callIt->second.m2eLatencyGraph->Append (Simulator::Now ().GetSeconds(), (Simulator::Now () - talkSpurtStart).GetMilliSeconds ());
            }
        }
    }

}

void
SchoolShootingLteVisHelper::EnableEnbThroughputGraphs ()
{
  NS_LOG_FUNCTION (this);
  ///NodeList/[i]/DeviceList/[i]/$ns3::LteNetDevice/$ns3::LteEnbNetDevice/LteEnbRrc/UeMap/[i]/DataRadioBearerMap/[i]/LteRlc

  NodeContainer enbs = m_scenarioHelper->GetTechnologyHelper ()->GetObject<SchoolShootingLteHelper> ()->GetEnodeBs ();

  for (auto enbIt = enbs.Begin (); enbIt != enbs.End (); enbIt++)
    {
      Ptr<Node> node = *enbIt;
      for (uint32_t netDevIdx = 0; netDevIdx < node->GetNDevices (); netDevIdx++)
        {
          Ptr<LteEnbNetDevice> netDev = node->GetDevice (netDevIdx)->GetObject<LteEnbNetDevice> ();

          if (netDev != nullptr)
            {
              Ptr<LteEnbRrc> rrc = netDev->GetRrc ();

              //Create a collection with the Tx and 2 Rxs together
              PointerValue xAxis; 
              PointerValue yAxis;
              Ptr<netsimulyzer::SeriesCollection> collection = CreateObject <netsimulyzer::SeriesCollection>(m_orchestrator);
              collection->SetAttribute ("AutoColor", BooleanValue (true));
              collection->SetAttribute ("Name", StringValue("eNodeB Cell Id " + std::to_string (netDev->GetCellId ()) + " (Node " + std::to_string (node->GetId ()) + ")"));
              collection->GetAttribute ("XAxis", xAxis);
              xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));
              collection->GetAttribute ("YAxis", yAxis);
              yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("Throughput (kb/s)"));

              Ptr<netsimulyzer::ThroughputSink> txSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "TX");
              txSink->SetAttribute ("Unit", StringValue ("kb/s"));
              txSink->SetAttribute ("Interval", TimeValue (m_interval));
              PointerValue txXySeries;
              txSink->GetAttribute ("XYSeries", txXySeries);
              txXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
              collection->Add (txSink->GetSeries ());

              Ptr<netsimulyzer::ThroughputSink> rxSink = CreateObject<netsimulyzer::ThroughputSink> (m_orchestrator, "RX");
              rxSink->SetAttribute ("Unit", StringValue ("kb/s"));
              rxSink->SetAttribute ("Interval", TimeValue (m_interval));
              PointerValue rxXySeries;
              rxSink->GetAttribute ("XYSeries", rxXySeries);
              rxXySeries.Get<netsimulyzer::XYSeries> ()->SetAttribute ("LabelMode", EnumValue (netsimulyzer::XYSeries::LabelMode::Hidden));
              collection->Add (rxSink->GetSeries ());

              m_enbThroughputCollections.push_back (collection);

              ObjectPtrContainerValue ueManagers;
              rrc->GetAttribute ("UeMap", ueManagers);

              for (auto ueManagerIt = ueManagers.Begin (); ueManagerIt != ueManagers.End (); ueManagerIt++)
                {
                  ObjectMapValue drbMap;
                  ueManagerIt->second->GetAttribute ("DataRadioBearerMap", drbMap);

                  for (auto drbIt = drbMap.Begin (); drbIt != drbMap.End (); drbIt++)
                    {
                      PointerValue rlc;
                      drbIt->second->GetAttribute ("LteRlc", rlc);

                      rlc.Get<LteRlc> ()->TraceConnectWithoutContext ("TxPDU", MakeBoundCallback (&TraceEnbThroughputTx, txSink));
                      rlc.Get<LteRlc> ()->TraceConnectWithoutContext ("RxPDU", MakeBoundCallback (&TraceEnbThroughputRx, rxSink));
                    }
                }
            }
        }
    }
}

void
SchoolShootingLteVisHelper::TraceEnbThroughputTx (Ptr<netsimulyzer::ThroughputSink> sink, uint16_t rnti, uint8_t lcid, uint32_t bytes)
{
  NS_LOG_FUNCTION (sink << rnti << lcid << bytes);

  sink->AddPacketSize (bytes);
}

void
SchoolShootingLteVisHelper::TraceEnbThroughputRx (Ptr<netsimulyzer::ThroughputSink> sink, uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t delay)
{
  NS_LOG_FUNCTION (sink << rnti << lcid << bytes << delay);

  sink->AddPacketSize (bytes);
}

} // namespace psc
} // namespace ns3

#endif // HAS_NETSIMULYZER
