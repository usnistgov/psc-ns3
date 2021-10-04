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
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/integer.h"

#include "schoolshooting-definition-helper.h"
#include "ns3/group-mobility-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/event-id.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SchoolShootingDefinitionHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (SchoolShootingDefinitionHelper);

SchoolShootingDefinitionHelper::SchoolShootingDefinitionHelper ()
  : m_scenarioDefinition (nullptr), m_createClassrooms (false)
{
  NS_LOG_FUNCTION (this);

  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
  m_uniformRandomVariable->SetAttribute ("Stream", IntegerValue (4118918303));
}

SchoolShootingDefinitionHelper::~SchoolShootingDefinitionHelper ()
{
  NS_LOG_FUNCTION (this);

}

TypeId
SchoolShootingDefinitionHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::SchoolShootingDefinitionHelper")
    .SetParent<Object> ()
    .AddConstructor<SchoolShootingDefinitionHelper> ()
    .AddAttribute ("CreateClassrooms",
                   "Flag to indicate whether classrooms layout should be created using buildings",
                   BooleanValue (false),
                   MakeBooleanAccessor (&SchoolShootingDefinitionHelper::m_createClassrooms),
                   MakeBooleanChecker ())
  ;

  return tid;
}

int64_t
SchoolShootingDefinitionHelper::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  return 1;
}

void
SchoolShootingDefinitionHelper::CreateScenario (std::string name)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_scenarioDefinition != nullptr, "Scenario definition already created.");

  m_scenarioDefinition = CreateObject <PscScenarioDefinition> ();
  m_scenarioDefinition->SetName (name);
  //Call initialization functions
  InitAreas ();
  InitStructures ();
  InitKeyEvents ();
  InitNodesAndMobility (); //make sure this is done after populating the key events in case mobility depends on the events
}

Ptr<PscScenarioDefinition>
SchoolShootingDefinitionHelper::GetScenarioDefinition () const
{
  return m_scenarioDefinition;
}

void SchoolShootingDefinitionHelper::InitAreas ()
{
  NS_LOG_FUNCTION (this);

  PscScenarioDefinition::AreaInfo area;

  // Outer perimeter center is always in (0,0)
  area.name = "Outer perimeter";
  area.area = Rectangle (-300, 300, -300, 300);
  m_scenarioDefinition->AddArea (OUTER_PERIMETER_AREA, area);

  // Inner perimeter
  area.name = "Inner perimeter";
  area.area = Rectangle (-50, 250,  0, 200);
  m_scenarioDefinition->AddArea (INNER_PERIMETER_AREA, area);

  // Staging Area
  area.name = "Staging area";
  area.area = Rectangle (135, 165, -95, -55);
  m_scenarioDefinition->AddArea (STAGING_AREA, area);

  // Unified Command
  area.name = "Unified command";
  area.area = Rectangle (-80, -70, -55, -45);
  m_scenarioDefinition->AddArea (UNIFIED_COMMAND_AREA, area);

}

void SchoolShootingDefinitionHelper::InitStructures ()
{
  NS_LOG_FUNCTION (this);
  //Due to the shape of the school building, we use 5 ns-3 building
  //to represent its entire shape
  PscScenarioDefinition::StructureInfo building;
  building.name = "School";

  Ptr<Building> buildingA = CreateObject<Building> ();
  Ptr<Building> buildingB = CreateObject<Building> ();
  Ptr<Building> buildingC = CreateObject<Building> ();
  Ptr<Building> buildingD = CreateObject<Building> ();
  Ptr<Building> buildingE = CreateObject<Building> ();

  buildingA->SetBoundaries ({-38,37,12.5,187.5,0,6});
  buildingB->SetBoundaries ({37,77,62.5,112.5,0,6});
  buildingC->SetBoundaries ({77,157,37.5,137.5,0,6});
  buildingD->SetBoundaries ({157,182,62.5,112.5,0,6});
  buildingE->SetBoundaries ({182,238,17.5,157.5,0,6});

  building.buildings.Add (buildingA);
  building.buildings.Add (buildingB);
  building.buildings.Add (buildingC);
  building.buildings.Add (buildingD);
  building.buildings.Add (buildingE);

  m_scenarioDefinition->AddStructure (SCHOOL_BLD, building);

  //The creation of classes is optional, it could be used for advanced propagation
  //models that could use this information or for visualization purposes
  if (m_createClassrooms)
    { //Create buildings to form classrooms
      PscScenarioDefinition::StructureInfo classes;
      classes.name = "Classes";

      Ptr<Building> room;

      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,12.5,22.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,22.5,32.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,32.5,42.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,42.5,52.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,58.5,68.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,68.5,78.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,78.5,88.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,88.5,98.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,101.5,111.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,111.5,121.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,121.5,131.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,131.5,141.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,157.5,167.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,167.5,177.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-38,-23,177.5,187.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15.5,14.5,12.5,42.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,57.5,67.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,67.5,77.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,77.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,87.5,97.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,97.5,107.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,107.5,117.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,157.5,167.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,167.5,177.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({-15,0,177.5,187.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,57.5,67.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,67.5,77.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,77.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,87.5,97.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,97.5,107.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,107.5,117.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,157.5,167.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,167.5,177.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({0,15,177.5,187.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,12.5,22.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,22.5,32.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,32.5,42.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,42.5,52.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,157.5,167.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,167.5,177.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({22,37,177.5,187.5,0,5});
      classes.buildings.Add (room);

      room = CreateObject<Building> ();
      room->SetBoundaries ({37,47,72.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({37,47,87.5,102.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({47,57,72.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({47,57,87.5,102.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({57,67,72.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({57,67,87.5,102.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({67,77,72.5,87.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({67,77,87.5,102.5,0,5});
      classes.buildings.Add (room);

      room = CreateObject<Building> ();
      room->SetBoundaries ({77,87,37.5,47.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({77,87,47.5,57.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({77,87,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({77,87,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,112,37.5,47.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,112,47.5,57.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,112,57.5,67.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,117,97.5,107.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,117,107.5,117.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,117,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({102,117,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({112,127,37.5,67.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({127,157,37.5,67.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({122,137,97.5,107.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({122,137,107.5,117.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({122,137,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({122,137,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({137,157,107.5,137.5,0,5});
      classes.buildings.Add (room);

      room = CreateObject<Building> ();
      room->SetBoundaries ({167,182,65.5,75.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({167,182,75.5,85.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({167,182,85.5,95.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({167,182,95.5,105.5,0,5});
      classes.buildings.Add (room);

      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,17.5,27.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,27.5,37.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,37.5,47.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,47.5,57.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,65.5,75.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,75.5,85.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,85.5,95.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,95.5,105.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({182,197,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({200,220,17.5,47.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({200,210,65.5,75.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({200,210,75.5,85.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({200,210,85.5,95.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({200,210,95.5,105.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({210,220,65.5,75.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({210,220,75.5,85.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({210,220,85.5,95.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({210,220,95.5,105.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({202,217,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({202,217,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({202,217,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({202,217,147.5,157.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,17.5,27.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,27.5,37.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,37.5,47.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,47.5,57.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,65.5,75.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,75.5,85.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,85.5,95.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,95.5,105.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,117.5,127.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,127.5,137.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,137.5,147.5,0,5});
      classes.buildings.Add (room);
      room = CreateObject<Building> ();
      room->SetBoundaries ({223,238,147.5,157.5,0,5});
      classes.buildings.Add (room);

      m_scenarioDefinition->AddStructure (CLASSROOMS_BLD, classes);
    }
}

void SchoolShootingDefinitionHelper::InitKeyEvents ()
{
  NS_LOG_FUNCTION (this);

  Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();
  uniformRandomVariable->SetAttribute ("Stream", IntegerValue (3128918313));

  PscScenarioDefinition::KeyEventInfo event;

  event.name = "E1_Incident_Start";
  event.time = Minutes (0);
  m_scenarioDefinition->AddKeyEvent (E1_Incident_Start, event);

  event.name = "E2_TaskForce_Arrive";
  event.time = Minutes (uniformRandomVariable->GetValue (3, 5));
  m_scenarioDefinition->AddKeyEvent (E2_TaskForce_Arrive, event);

  event.name = "E3_SWAT_Arrive";
  event.time = Minutes (uniformRandomVariable->GetValue (10, 30));
  m_scenarioDefinition->AddKeyEvent (E3_SWAT_Arrive, event);

  event.name = "E4_FireEMS_Arrive";
  event.time = Minutes (uniformRandomVariable->GetValue (10, 30));
  m_scenarioDefinition->AddKeyEvent (E4_FireEMS_Arrive, event);

  event.name = "E5_UC_Setup";
  event.time = Minutes (uniformRandomVariable->GetValue (3, 30));
  m_scenarioDefinition->AddKeyEvent (E5_UC_Setup, event);

  event.name = "E6_Perimeters_Ready";
  event.time = Minutes (30);
  m_scenarioDefinition->AddKeyEvent (E6_Perimeters_Ready, event);

  event.name = "E7_SWAT_Deploy";
  event.time = Minutes (60);
  m_scenarioDefinition->AddKeyEvent (E7_SWAT_Deploy, event);

  event.name = "E8_Offender_Found";
  event.time = Minutes (120);
  m_scenarioDefinition->AddKeyEvent (E8_Offender_Found, event);

  event.name = "E9_Interdiction";
  event.time = Minutes (180);
  m_scenarioDefinition->AddKeyEvent (E9_Interdiction, event);

  event.name = "E10_Incident_Secured";
  event.time = Minutes (185);
  m_scenarioDefinition->AddKeyEvent (E10_Incident_Secured, event);

  event.name = "E11_EMS_Departure";
  event.time = Minutes (195);
  m_scenarioDefinition->AddKeyEvent (E11_EMS_Departure, event);

  event.name = "E12_Incident_End";
  event.time = Minutes (245);
  m_scenarioDefinition->AddKeyEvent (E12_Incident_End, event);

}

void SchoolShootingDefinitionHelper::InitNodesAndMobility ()
{
  NS_LOG_FUNCTION (this);


  //Since this is a large scenario, we break down the configuration of each group
  //in separate functions.

  InitializeUC ();
  InitializeSwatTeams ();
  InitializeTaskForce ();
  InitializeFireEms ();
  InitializeHelicopter ();
  InitializeSchoolAndTrafficCameras ();
  InitializeOffender ();
}

void SchoolShootingDefinitionHelper::InitializeUC ()
{
  NS_LOG_FUNCTION (this);

  //Configure position of UC vehicle
  NodeContainer uc;
  uc.Create (12);
  PscScenarioDefinition::NodeGroupInfo ucInfo;
  ucInfo.name = "UnifiedCommand";
  ucInfo.nodes = uc;
  m_scenarioDefinition->AddNodes (UC_TEAM, ucInfo);
  Ptr<ListPositionAllocator> ucPosAllocator;
  ucPosAllocator = CreateObject<ListPositionAllocator> ();
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));
  ucPosAllocator->Add (Vector (-75, -50, 1.5));

  MobilityHelper ucMobilityHelper;
  ucMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  ucMobilityHelper.SetPositionAllocator (ucPosAllocator);
  ucMobilityHelper.Install (uc);

  //Configure position of school cameras
  NodeContainer ucv;
  ucv.Create (1);
  PscScenarioDefinition::NodeGroupInfo ucvInfo;
  ucvInfo.name = "MobileUnitCommand";
  ucvInfo.nodes = ucv;
  m_scenarioDefinition->AddNodes (UC_TEAM_VEHICLE, ucvInfo);
  Ptr<ListPositionAllocator> ucvPosAllocator;
  ucvPosAllocator = CreateObject<ListPositionAllocator> ();
  ucvPosAllocator->Add (Vector (-75, -50, 2.5));

  MobilityHelper ucvMobilityHelper;
  ucvMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  ucvMobilityHelper.SetPositionAllocator (ucvPosAllocator);
  ucvMobilityHelper.Install (ucv);
}

void SchoolShootingDefinitionHelper::InitializeSwatTeams ()
{
  NS_LOG_FUNCTION (this);

  //Each SWAT team has a team leader and three other officers
  //To configure the mobility of each team, we use WaypointMobilityModel mobility
  //model to move the team leader, and use group mobility
  //model to have the other officers move next to him (without having to manually configure
  //each of them
  double t = 0;

  //Deployable Cameras (one DC for each team)
  NodeContainer deployableCameras;
  deployableCameras.Create (4);
  PscScenarioDefinition::NodeGroupInfo deployableCamerasInfo;
  deployableCamerasInfo.name = "DeployableCameras";
  deployableCamerasInfo.nodes = deployableCameras;
  m_scenarioDefinition->AddNodes (DEPLOYABLE_CAMERA, deployableCamerasInfo);
  MobilityHelper deployableCamerasMobilityHelper;
  deployableCamerasMobilityHelper.SetMobilityModel ("ns3::WaypointMobilityModel");
  deployableCamerasMobilityHelper.Install (deployableCameras);

  //DC 1
  Ptr<WaypointMobilityModel> waypointMmDC1 = deployableCameras.Get (0)->GetObject<WaypointMobilityModel> ();
  waypointMmDC1->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t = AddWayPoint (waypointMmDC1, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 209.371082052895, Vector (-42, 8.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 136, Vector (-42, 144.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 4, Vector (-40, 144.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (waypointMmDC1, t + 0, Vector (-40, 144.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 16, Vector (-22, 144.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 8, Vector (-22, 152.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-22, 162.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-22, 172.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-22, 182.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 6, Vector (-16, 182.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 172.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 162.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 152.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 142.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 132.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 122.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 112.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 102.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 92.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 82.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 72.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (-16, 62.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 6, Vector (-16, 56.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 32, Vector (16, 56.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC1, t + 6, Vector (16, 62.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 72.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 82.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 92.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 102.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 112.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 122.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 132.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 142.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC1, t + 10, Vector (16, 152.5, 1.5), 120);

  //DC 2
  Ptr<WaypointMobilityModel> waypointMmDC2 = deployableCameras.Get (1)->GetObject<WaypointMobilityModel> ();
  waypointMmDC2->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 10; //move team 2 after team 1 starts moving
  t = AddWayPoint (waypointMmDC2, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (waypointMmDC2, t + 209.371082052895, Vector (-42, 8.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC2, t + 47, Vector (-42, 55.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC2, t + 4, Vector (-40, 55.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (waypointMmDC2, t + 0, Vector (-40, 55.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC2, t + 16, Vector (-22, 55.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC2, t + 8, Vector (-22, 63.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 73.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 83.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 93.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 13, Vector (-22, 106.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 116.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 126.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 136.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 87, Vector (-22, 49.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 39.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 29.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (-22, 19.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 8, Vector (-14, 19.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 24, Vector (-14, 43.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 28, Vector (14, 43.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 24, Vector (14, 19.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 8, Vector (22, 19.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (22, 29.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (22, 39.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 10, Vector (22, 49.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC2, t + 6, Vector (22, 55.5, 1.5), 0);
 
  //DC 3
  Ptr<WaypointMobilityModel> waypointMmDC3 = deployableCameras.Get (2)->GetObject<WaypointMobilityModel> ();
  waypointMmDC3->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 20; //move team 3 after team 2
  t = AddWayPoint (waypointMmDC3, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 165.997740948484, Vector (57, 60.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (waypointMmDC3, t + 0, Vector (57, 60.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 1, Vector (57, 63.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 31, Vector (88, 63.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 11, Vector (88, 52.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 10, Vector (88, 42.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 13, Vector (101, 42.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 10, Vector (101, 52.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 10, Vector (101, 62.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 6, Vector (101, 68.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 14, Vector (115, 68.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 26, Vector (115, 42.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 26, Vector (115, 68.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 13, Vector (128, 68.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC3, t + 26, Vector (128, 42.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 28, Vector (156, 42.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 90, Vector (156, 132.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 18, Vector (138, 132.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC3, t + 36, Vector (138, 96.5, 1.5), 0);

  //DC 4
  Ptr<WaypointMobilityModel> waypointMmDC4 = deployableCameras.Get (3)->GetObject<WaypointMobilityModel> ();
  waypointMmDC4->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 40; //move team 4 after team 3
  t = AddWayPoint (waypointMmDC4, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 124.242706023332, Vector (242, 8.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 102, Vector (242, 110.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 4, Vector (240, 110.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (waypointMmDC4, t + 0, Vector (240, 110.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 16, Vector (222, 110.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 11, Vector (222, 121.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (222, 131.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (222, 141.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (222, 151.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 2, Vector (220, 151.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 36, Vector (220, 115.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 17, Vector (203, 115.5, 1.5), 0);
  t = AddWayPoint (waypointMmDC4, t + 6, Vector (203, 121.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (203, 131.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (203, 141.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (203, 151.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 5, Vector (198, 151.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (198, 141.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (198, 131.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 10, Vector (198, 121.5, 1.5), 120);
  t = AddWayPoint (waypointMmDC4, t + 16, Vector (198, 105.5, 1.5), 0);
 
  //TEAM 1
  NodeContainer team1;
  team1.Create (4);
  PscScenarioDefinition::NodeGroupInfo team1Info;
  team1Info.name = "SWATteam1";
  team1Info.nodes = team1;
  m_scenarioDefinition->AddNodes (SWAT_TEAM_1, team1Info);

  Ptr<WaypointMobilityModel> refTeam1MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam1MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));

  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t = AddWayPoint (refTeam1MobilityModel, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 209.371082052895, Vector (-42, 8.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 136, Vector (-42, 144.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 4, Vector (-40, 144.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (refTeam1MobilityModel, t + 0, Vector (-40, 144.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 16, Vector (-22, 144.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 8, Vector (-22, 152.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-22, 162.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-22, 172.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-22, 182.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 6, Vector (-16, 182.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 172.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 162.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 152.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 142.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 122.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 112.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 102.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 92.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 82.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 72.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (-16, 62.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 6, Vector (-16, 56.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 32, Vector (16, 56.5, 1.5), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 6, Vector (16, 62.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 72.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 82.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 92.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 102.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 112.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 122.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 142.5, 1.5), 120);
  t = AddWayPoint (refTeam1MobilityModel, t + 10, Vector (16, 152.5, 1.5), 120);

  GroupMobilityHelper team1MobilityHelper;
  team1MobilityHelper.SetReferenceMobilityModel (refTeam1MobilityModel);

  team1MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team1MobilityHelper.Install (team1);

  //TEAM 2
  NodeContainer team2;
  team2.Create (4);
  PscScenarioDefinition::NodeGroupInfo team2Info;
  team2Info.name = "SWATteam2";
  team2Info.nodes = team2;
  m_scenarioDefinition->AddNodes (SWAT_TEAM_2, team2Info);

  Ptr<WaypointMobilityModel> refTeam2MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam2MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));

  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 10; //move team 2 after team 1 starts moving
  t = AddWayPoint (refTeam2MobilityModel, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 209.371082052895, Vector (-42, 8.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 47, Vector (-42, 55.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 4, Vector (-40, 55.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (refTeam2MobilityModel, t + 0, Vector (-40, 55.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 16, Vector (-22, 55.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 8, Vector (-22, 63.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 73.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 83.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 93.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 13, Vector (-22, 106.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 116.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 126.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 136.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 87, Vector (-22, 49.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 39.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 29.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (-22, 19.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 8, Vector (-14, 19.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 24, Vector (-14, 43.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 28, Vector (14, 43.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 24, Vector (14, 19.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 8, Vector (22, 19.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (22, 29.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (22, 39.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (22, 49.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 6, Vector (22, 55.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 15, Vector (37, 55.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 20, Vector (37, 75.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 6, Vector (43, 75.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (53, 75.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (63, 75.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (73, 75.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 6, Vector (79, 75.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 32, Vector (79, 107.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 6, Vector (73, 107.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (63, 107.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (53, 107.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 10, Vector (43, 107.5, 1.5), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 21, Vector (22, 107.5, 1.5), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 45, Vector (22, 152.5, 1.5), 0);

  GroupMobilityHelper team2MobilityHelper;
  team2MobilityHelper.SetReferenceMobilityModel (refTeam2MobilityModel);

  team2MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team2MobilityHelper.Install (team2);

  //TEAM 3
  NodeContainer team3;
  team3.Create (4);
  PscScenarioDefinition::NodeGroupInfo team3Info;
  team3Info.name = "SWATteam3";
  team3Info.nodes = team3;
  m_scenarioDefinition->AddNodes (SWAT_TEAM_3, team3Info);

  Ptr<WaypointMobilityModel> refTeam3MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam3MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));

  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 20; //move team 3 after team 2
  t = AddWayPoint (refTeam3MobilityModel, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 165.997740948484, Vector (57, 60.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (refTeam3MobilityModel, t + 0, Vector (57, 60.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 1, Vector (57, 63.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 31, Vector (88, 63.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 11, Vector (88, 52.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (88, 42.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 13, Vector (101, 42.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (101, 52.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (101, 62.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 6, Vector (101, 68.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 14, Vector (115, 68.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 26, Vector (115, 42.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 26, Vector (115, 68.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 13, Vector (128, 68.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 26, Vector (128, 42.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 28, Vector (156, 42.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 90, Vector (156, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 18, Vector (138, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 36, Vector (138, 96.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 17, Vector (121, 96.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 6, Vector (121, 102.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (121, 112.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (121, 122.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (121, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 3, Vector (118, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (118, 122.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (118, 112.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (118, 102.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 6, Vector (118, 96.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 17, Vector (101, 96.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 36, Vector (101, 132.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 13, Vector (88, 132.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 10, Vector (88, 122.5, 1.5), 120);
  t = AddWayPoint (refTeam3MobilityModel, t + 6, Vector (88, 116.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 11.180339887499, Vector (78, 111.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 57, Vector (21, 111.5, 1.5), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 41, Vector (21, 152.5, 1.5), 0);

  GroupMobilityHelper team3MobilityHelper;
  team3MobilityHelper.SetReferenceMobilityModel (refTeam3MobilityModel);

  team3MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team3MobilityHelper.Install (team3);


//TEAM 4
  NodeContainer team4;
  team4.Create (4);
  PscScenarioDefinition::NodeGroupInfo team4Info;
  team4Info.name = "SWATteam4";
  team4Info.nodes = team4;
  m_scenarioDefinition->AddNodes (SWAT_TEAM_4, team4Info);

  Ptr<WaypointMobilityModel> refTeam4MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam4MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));

  t = m_scenarioDefinition->GetKeyEventTime (E3_SWAT_Arrive).time.GetSeconds ();
  t += 40; //move team 4 after team 3
  t = AddWayPoint (refTeam4MobilityModel, t + 0, Vector (150, -75, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 124.242706023332, Vector (242, 8.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 102, Vector (242, 110.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 4, Vector (240, 110.5, 1.5), 0);

  t = m_scenarioDefinition->GetKeyEventTime (E7_SWAT_Deploy).time.GetSeconds ();
  t = AddWayPoint (refTeam4MobilityModel, t + 0, Vector (240, 110.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 16, Vector (222, 110.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 11, Vector (222, 121.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (222, 131.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (222, 141.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (222, 151.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 2, Vector (220, 151.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 36, Vector (220, 115.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 17, Vector (203, 115.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 6, Vector (203, 121.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (203, 131.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (203, 141.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (203, 151.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 5, Vector (198, 151.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (198, 141.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (198, 131.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (198, 121.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 16, Vector (198, 105.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 22, Vector (220, 105.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 6, Vector (220, 99.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 89.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 79.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 69.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 18, Vector (220, 51.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 41.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 31.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 21.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (220, 31.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 23, Vector (197, 31.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (197, 21.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 20, Vector (197, 41.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (197, 51.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 18, Vector (197, 69.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (197, 79.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (197, 89.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (197, 99.5, 1.5), 240);
  t = AddWayPoint (refTeam4MobilityModel, t + 6, Vector (197, 105.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 34, Vector (163, 105.5, 1.5), 0);
  t = AddWayPoint (refTeam4MobilityModel, t + 6, Vector (163, 99.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (163, 89.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (163, 79.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (163, 69.5, 1.5), 120);
  t = AddWayPoint (refTeam4MobilityModel, t + 10, Vector (163, 59.5, 1.5), 0);

  GroupMobilityHelper team4MobilityHelper;
  team4MobilityHelper.SetReferenceMobilityModel (refTeam4MobilityModel);

  team4MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team4MobilityHelper.Install (team4);
}

void SchoolShootingDefinitionHelper::InitializeTaskForce ()
{
  NS_LOG_FUNCTION (this);

  // Configure the 12 roadblocks with two vehicles per roadblock and 10 more
  // vehicles in the staging area
  NodeContainer tfv;
  tfv.Create (34);
  PscScenarioDefinition::NodeGroupInfo tfvInfo;
  tfvInfo.name = "TaskForceVehicles";
  tfvInfo.nodes = tfv;
  m_scenarioDefinition->AddNodes (TASK_FORCE_VEHICLES, tfvInfo);

  Ptr<ListPositionAllocator> tfvPosAllocator;
  tfvPosAllocator = CreateObject<ListPositionAllocator> ();
  // 24 cars around each perimeter
  tfvPosAllocator->Add (Vector (-100, 300, 1.0));
  tfvPosAllocator->Add (Vector (-95, 300, 1.0));
  tfvPosAllocator->Add (Vector (100, 300, 1.0));
  tfvPosAllocator->Add (Vector (105, 300, 1.0));
  tfvPosAllocator->Add (Vector (300, -100, 1.0));
  tfvPosAllocator->Add (Vector (300, -95, 1.0));
  tfvPosAllocator->Add (Vector (300, 100, 1.0));
  tfvPosAllocator->Add (Vector (300, 105, 1.0));
  tfvPosAllocator->Add (Vector (100, -300, 1.0));
  tfvPosAllocator->Add (Vector (105, -300, 1.0));
  tfvPosAllocator->Add (Vector (-100, -300, 1.0));
  tfvPosAllocator->Add (Vector (-95, -300, 1.0));
  tfvPosAllocator->Add (Vector (-300, -100, 1.0));
  tfvPosAllocator->Add (Vector (-300, -95, 1.0));
  tfvPosAllocator->Add (Vector (-300, 100, 1.0));
  tfvPosAllocator->Add (Vector (-300, 105, 1.0));
  tfvPosAllocator->Add (Vector (97, 200, 1.0));
  tfvPosAllocator->Add (Vector (103, 200, 1.0));
  tfvPosAllocator->Add (Vector (250, 103, 1.0));
  tfvPosAllocator->Add (Vector (250, 97, 1.0));
  tfvPosAllocator->Add (Vector (103, 0, 1.0));
  tfvPosAllocator->Add (Vector (97, 0, 1.0));
  tfvPosAllocator->Add (Vector (-50, 97, 1.0));
  tfvPosAllocator->Add (Vector (-50, 103, 1.0));
  // 10 cars in the staging area
  tfvPosAllocator->Add (Vector (137, -79, 1.0));
  tfvPosAllocator->Add (Vector (145, -79, 1.0));
  tfvPosAllocator->Add (Vector (137, -82, 1.0));
  tfvPosAllocator->Add (Vector (145, -82, 1.0));
  tfvPosAllocator->Add (Vector (137, -85, 1.0));
  tfvPosAllocator->Add (Vector (145, -85, 1.0));
  tfvPosAllocator->Add (Vector (137, -88, 1.0));
  tfvPosAllocator->Add (Vector (145, -88, 1.0));
  tfvPosAllocator->Add (Vector (137, -91, 1.0));
  tfvPosAllocator->Add (Vector (145, -91, 1.0));


  MobilityHelper tfvMobilityHelper;
  tfvMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  tfvMobilityHelper.SetPositionAllocator (tfvPosAllocator);
  tfvMobilityHelper.Install (tfv);

  NodeContainer tf;
  tf.Create (68); //48 at road blocks + 20 for patrol
  PscScenarioDefinition::NodeGroupInfo tfInfo;
  tfInfo.name = "TaskForce";
  tfInfo.nodes = tf;
  m_scenarioDefinition->AddNodes (TASK_FORCE, tfInfo);

  Ptr<ListPositionAllocator> tfPosAllocator;
  tfPosAllocator = CreateObject<ListPositionAllocator> ();
  for (uint32_t i = 0; i < 24; i++)
    {
      Vector v = tfvPosAllocator->GetNext ();
      v.z = 1.5; //height of personnel
      tfPosAllocator->Add (v); //officer 1
      tfPosAllocator->Add (v); //officer 2
    }

  //Remaining 20 patrol units (precomputed random locations)
  tfPosAllocator->Add (Vector (-3, -71, 1.5));
  tfPosAllocator->Add (Vector (-79, 184, 1.5));
  tfPosAllocator->Add (Vector (-239, 51, 1.5));
  tfPosAllocator->Add (Vector (263, 76, 1.5));
  tfPosAllocator->Add (Vector (-153, 202, 1.5));
  tfPosAllocator->Add (Vector (-288, 203, 1.5));
  tfPosAllocator->Add (Vector (266, 201, 1.5));
  tfPosAllocator->Add (Vector (-263, 9, 1.5));
  tfPosAllocator->Add (Vector (-167, -95, 1.5));
  tfPosAllocator->Add (Vector (-74, -81, 1.5));
  tfPosAllocator->Add (Vector (275, -7, 1.5));
  tfPosAllocator->Add (Vector (-89, -118, 1.5));
  tfPosAllocator->Add (Vector (253, 120, 1.5));
  tfPosAllocator->Add (Vector (-16, -120, 1.5));
  tfPosAllocator->Add (Vector (-24, 232, 1.5));
  tfPosAllocator->Add (Vector (156, -297, 1.5));
  tfPosAllocator->Add (Vector (-196, -94, 1.5));
  tfPosAllocator->Add (Vector (224, -206, 1.5));
  tfPosAllocator->Add (Vector (-77, -72, 1.5));
  tfPosAllocator->Add (Vector (131, -139, 1.5));

  MobilityHelper tfMobilityHelper;
  tfMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  tfMobilityHelper.SetPositionAllocator (tfPosAllocator);
  tfMobilityHelper.Install (tf);
}

void SchoolShootingDefinitionHelper::InitializeFireEms ()
{
  //Configure SA mobile unit
  NodeContainer sav;
  sav.Create (1);
  PscScenarioDefinition::NodeGroupInfo savInfo;
  savInfo.name = "SAVehicle";
  savInfo.nodes = sav;
  m_scenarioDefinition->AddNodes (SA_VEHICLE, savInfo);
  Ptr<ListPositionAllocator> savPosAllocator;
  savPosAllocator = CreateObject<ListPositionAllocator> ();
  savPosAllocator->Add (Vector (157, -65, 2.5));

  MobilityHelper savMobilityHelper;
  savMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  savMobilityHelper.SetPositionAllocator (savPosAllocator);
  savMobilityHelper.Install (sav);

  //Configure position of fire trucks and firefighters
  NodeContainer fv;
  fv.Create (2);
  PscScenarioDefinition::NodeGroupInfo fvInfo;
  fvInfo.name = "FireVehicles";
  fvInfo.nodes = fv;
  m_scenarioDefinition->AddNodes (FIRE_VEHICLES, fvInfo);
  Ptr<ListPositionAllocator> fvPosAllocator;
  fvPosAllocator = CreateObject<ListPositionAllocator> ();
  fvPosAllocator->Add (Vector (144, -58, 2.8));
  fvPosAllocator->Add (Vector (144, -63, 2.8));

  MobilityHelper fvMobilityHelper;
  fvMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  fvMobilityHelper.SetPositionAllocator (fvPosAllocator);
  fvMobilityHelper.Install (fv);

  NodeContainer f;
  f.Create (6);
  PscScenarioDefinition::NodeGroupInfo fInfo;
  fInfo.name = "Firefighters";
  fInfo.nodes = f;
  m_scenarioDefinition->AddNodes (FIRE, fInfo);
  Ptr<ListPositionAllocator> fPosAllocator;
  fPosAllocator = CreateObject<ListPositionAllocator> ();
  fPosAllocator->Add (Vector (144, -58, 1.5));
  fPosAllocator->Add (Vector (144, -58, 1.5));
  fPosAllocator->Add (Vector (144, -58, 1.5));
  fPosAllocator->Add (Vector (144, -63, 1.5));
  fPosAllocator->Add (Vector (144, -63, 1.5));
  fPosAllocator->Add (Vector (144, -63, 1.5));

  MobilityHelper fMobilityHelper;
  fMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  fMobilityHelper.SetPositionAllocator (fPosAllocator);
  fMobilityHelper.Install (f);

  //Configure position of ambulances and EMTs
  NodeContainer ev;
  ev.Create (3);
  PscScenarioDefinition::NodeGroupInfo evInfo;
  evInfo.name = "EMSVehicles";
  evInfo.nodes = ev;
  m_scenarioDefinition->AddNodes (EMS_VEHICLES, evInfo);

  MobilityHelper evMobilityHelper;
  evMobilityHelper.SetMobilityModel ("ns3::WaypointMobilityModel");
  evMobilityHelper.Install (ev);

  Ptr<Node> ambulance1 = ev.Get (0);
  Ptr<WaypointMobilityModel> amb1MobilityModel = ambulance1->GetObject<WaypointMobilityModel> ();
  amb1MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  double t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (amb1MobilityModel, t + 0, Vector (137, -68, 0.0), 0);
  t = AddWayPoint (amb1MobilityModel, t + 11.52, Vector (44, 0, 0.0), 600);
  t = AddWayPoint (amb1MobilityModel, t + 71.20, Vector (400, 0, 0.0), 0);

  Ptr<Node> ambulance2 = ev.Get (1);
  Ptr<WaypointMobilityModel> amb2MobilityModel = ambulance2->GetObject<WaypointMobilityModel> ();
  amb2MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (amb2MobilityModel, t + 0, Vector (145, -68, 0.0), 0);
  t = AddWayPoint (amb2MobilityModel, t + 11.52, Vector (52, 0, 0.0), 600);
  t = AddWayPoint (amb2MobilityModel, t + 69.60, Vector (400, 0, 0.0), 0);

  Ptr<Node> ambulance3 = ev.Get (2);
  Ptr<WaypointMobilityModel> amb3MobilityModel = ambulance3->GetObject<WaypointMobilityModel> ();
  amb3MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (amb3MobilityModel, t + 0, Vector (137, -72, 0.0), 0);
  t = AddWayPoint (amb3MobilityModel, t + 11.45, Vector (48, 0, 0.0), 600);
  t = AddWayPoint (amb3MobilityModel, t + 70.40, Vector (400, 0, 0.0), 0);

  NodeContainer e;
  e.Create (6);
  PscScenarioDefinition::NodeGroupInfo eInfo;
  eInfo.name = "EMTs";
  eInfo.nodes = e;
  m_scenarioDefinition->AddNodes (EMS, eInfo);

  NodeContainer ems1; //ems 1 is 2 EMTs
  ems1.Add (e.Get (0));
  ems1.Add (e.Get (1));
  Ptr<WaypointMobilityModel> refTeam1MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam1MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (refTeam1MobilityModel, t + 0, Vector (137, -68, 0.0), 0);
  t = AddWayPoint (refTeam1MobilityModel, t + 11.52, Vector (44, 0, 0.0), 60);
  t = AddWayPoint (refTeam1MobilityModel, t + 35.00, Vector (44, 70, 0.0), 370);
  t = AddWayPoint (refTeam1MobilityModel, t + 35.00, Vector (44, 0, 0.0), 100);
  t = AddWayPoint (refTeam1MobilityModel, t + 71.20, Vector (400, 0, 0.0), 0);
  GroupMobilityHelper team1MobilityHelper;
  team1MobilityHelper.SetReferenceMobilityModel (refTeam1MobilityModel);
  team1MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team1MobilityHelper.Install (ems1);

  NodeContainer ems2; //ems 2 is 2 EMTs
  ems2.Add (e.Get (2));
  ems2.Add (e.Get (3));
  Ptr<WaypointMobilityModel> refTeam2MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam2MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (refTeam2MobilityModel, t + 0, Vector (145, -68, 0.0), 0);
  t = AddWayPoint (refTeam2MobilityModel, t + 11.52, Vector (52, 0, 0.0), 60);
  t = AddWayPoint (refTeam2MobilityModel, t + 35.00, Vector (52, 70, 0.0), 350);
  t = AddWayPoint (refTeam2MobilityModel, t + 35.00, Vector (52, 0, 0.0), 120);
  t = AddWayPoint (refTeam2MobilityModel, t + 69.60, Vector (400, 0, 0.0), 0);
  GroupMobilityHelper team2MobilityHelper;
  team2MobilityHelper.SetReferenceMobilityModel (refTeam2MobilityModel);
  team2MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team2MobilityHelper.Install (ems2);

  NodeContainer ems3; //ems 3 is 2 EMTs
  ems3.Add (e.Get (4));
  ems3.Add (e.Get (5));
  Ptr<WaypointMobilityModel> refTeam3MobilityModel = CreateObject<WaypointMobilityModel> ();
  refTeam3MobilityModel->SetAttribute ("InitialPositionIsWaypoint", BooleanValue (true));
  t = m_scenarioDefinition->GetKeyEventTime (E10_Incident_Secured).time.GetSeconds ();
  t = AddWayPoint (refTeam3MobilityModel, t + 0, Vector (137, -72, 0.0), 0);
  t = AddWayPoint (refTeam3MobilityModel, t + 11.45, Vector (48, 0, 0.0), 60);
  t = AddWayPoint (refTeam3MobilityModel, t + 35.00, Vector (48, 70, 0.0), 300);
  t = AddWayPoint (refTeam3MobilityModel, t + 35.00, Vector (48, 0, 0.0), 170);
  t = AddWayPoint (refTeam3MobilityModel, t + 70.40, Vector (400, 0, 0.0), 0);
  GroupMobilityHelper team3MobilityHelper;
  team3MobilityHelper.SetReferenceMobilityModel (refTeam3MobilityModel);
  team3MobilityHelper.SetMemberMobilityModel ("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue (Rectangle (-2, 2, -2, 2)),
        "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
  team3MobilityHelper.Install (ems3);

}

void SchoolShootingDefinitionHelper::InitializeHelicopter ()
{
  NS_LOG_FUNCTION (this);

  //Configure position of school cameras
  NodeContainer hc;
  hc.Create (1);
  PscScenarioDefinition::NodeGroupInfo hcInfo;
  hcInfo.name = "Helicopter";
  hcInfo.nodes = hc;
  m_scenarioDefinition->AddNodes (HELICOPTER, hcInfo);
  Ptr<ListPositionAllocator> hcPosAllocator;
  hcPosAllocator = CreateObject<ListPositionAllocator> ();
  hcPosAllocator->Add (Vector (-150, 200, 150));

  MobilityHelper hcMobilityHelper;
  hcMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  hcMobilityHelper.SetPositionAllocator (hcPosAllocator);
  hcMobilityHelper.Install (hc);
}

void SchoolShootingDefinitionHelper::InitializeSchoolAndTrafficCameras ()
{
  NS_LOG_FUNCTION (this);

  //Configure position of school cameras
  NodeContainer sc;
  sc.Create (5);
  PscScenarioDefinition::NodeGroupInfo scInfo;
  scInfo.name = "SchoolCameras";
  scInfo.nodes = sc;
  m_scenarioDefinition->AddNodes (SCHOOL_CAMERAS, scInfo);
  Ptr<ListPositionAllocator> scPosAllocator;
  scPosAllocator = CreateObject<ListPositionAllocator> ();
  scPosAllocator->Add (Vector (47, 72.5, 2.5));
  scPosAllocator->Add (Vector (-23, 187.5, 2.5));
  scPosAllocator->Add (Vector (-23, 42.5, 2.5));
  scPosAllocator->Add (Vector (222, 157.5, 2.5));
  scPosAllocator->Add (Vector (200, 42.5, 2.5));

  MobilityHelper scMobilityHelper;
  scMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  scMobilityHelper.SetPositionAllocator (scPosAllocator);
  scMobilityHelper.Install (sc);

  //Configure position of traffic cameras
  NodeContainer tc;
  tc.Create (4);
  PscScenarioDefinition::NodeGroupInfo tcInfo;
  tcInfo.name = "TrafficCameras";
  tcInfo.nodes = tc;
  m_scenarioDefinition->AddNodes (TRAFFIC_CAMERAS, tcInfo);
  Ptr<ListPositionAllocator> tcPosAllocator;
  tcPosAllocator = CreateObject<ListPositionAllocator> ();
  tcPosAllocator->Add (Vector (-290, 60, 3.6));
  tcPosAllocator->Add (Vector (-200, 40, 3.6));
  tcPosAllocator->Add (Vector (275, 300, 3.6));
  tcPosAllocator->Add (Vector (-40, -200, 3.6));

  MobilityHelper tcMobilityHelper;
  tcMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  tcMobilityHelper.SetPositionAllocator (tcPosAllocator);
  tcMobilityHelper.Install (tc);
}

void SchoolShootingDefinitionHelper::InitializeOffender ()
{
  NS_LOG_FUNCTION (this);

  //Configure position of school cameras
  NodeContainer oc;
  oc.Create (1);
  PscScenarioDefinition::NodeGroupInfo ocInfo;
  ocInfo.name = "Offender";
  ocInfo.nodes = oc;
  m_scenarioDefinition->AddNodes (OFFENDER, ocInfo);
  Ptr<ListPositionAllocator> ocPosAllocator;
  ocPosAllocator = CreateObject<ListPositionAllocator> ();
  ocPosAllocator->Add (Vector (5, 152.5, 1.5));

  MobilityHelper ocMobilityHelper;
  ocMobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  ocMobilityHelper.SetPositionAllocator (ocPosAllocator);
  ocMobilityHelper.Install (oc);
}


double SchoolShootingDefinitionHelper::AddWayPoint (Ptr<WaypointMobilityModel> waypointMm, double t, Vector pos, double waitTime)
{
  NS_LOG_FUNCTION (this << waypointMm << t << pos << waitTime);
  //Add the new destination
  Waypoint wpt (Seconds (t), pos);
  waypointMm->AddWaypoint (wpt);
  //Make sure the node stays at the same location if it has to wait
  if (waitTime > 0)
    {
      Waypoint wpt2 (Seconds (t + waitTime), pos);
      waypointMm->AddWaypoint (wpt2);
    }
  //Return the new time at which the node can move again
  return t + waitTime;
}

} // namespace psc

} // namespace ns3
