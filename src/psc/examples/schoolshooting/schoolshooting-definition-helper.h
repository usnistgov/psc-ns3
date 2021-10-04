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

#ifndef SCHOOLSCHOOTING_DEFINITION_HELPER_H
#define SCHOOLSCHOOTING_DEFINITION_HELPER_H

#include <stdint.h>
#include <string>
#include <map>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/node-container.h>
#include <ns3/building-container.h>
#include <ns3/traced-callback.h>
#include <ns3/nstime.h>
#include <ns3/psc-scenario-definition.h>
#include "ns3/waypoint-mobility-model.h"


namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class that gathers information about number of the area, buildings,
 * nodes and their associated mobility.
 */
class SchoolShootingDefinitionHelper : public Object
{
public:
  /**
   * Defines the 4 major areas in the incident
   */
  enum Areas: PscScenarioDefinition::AreaId
  {
    OUTER_PERIMETER_AREA,
    INNER_PERIMETER_AREA,
    STAGING_AREA,
    UNIFIED_COMMAND_AREA
  };

  /**
   * Defines the building structures
   */
  enum Structures: PscScenarioDefinition::StructureId
  {
    SCHOOL_BLD,
    CLASSROOMS_BLD
  };

  /**
   * Defines major groups of users and assets
   */
  enum UserGroups: PscScenarioDefinition::GroupId
  {
    SWAT_TEAM_1,       //SWAT team 1 with 4 responders
    SWAT_TEAM_2,       //SWAT team 2 with 4 responders
    SWAT_TEAM_3,       //SWAT team 3 with 4 responders
    SWAT_TEAM_4,       //SWAT team 4 with 4 responders
    UC_TEAM,           //Unified command with 12 responders
    UC_TEAM_VEHICLE,   //Mobile command unit
    TASK_FORCE,        //68 personnel
    TASK_FORCE_VEHICLES, //34 patrol cars
    SA_VEHICLE,        //Mobile command unit for staging area
    EMS,               //6 EMTs
    EMS_VEHICLES,      //3 ambulances
    FIRE,              //6 firefighters
    FIRE_VEHICLES,     //2 fire trucks
    HELICOPTER,        //1 helicopter
    TRAFFIC_CAMERAS,   //4 traffic cameras
    SCHOOL_CAMERAS,    //5 school cameras
    OFFENDER,          //The offender
    DEPLOYABLE_CAMERA //4 Deployable cameras
  };

  /**
   * Defines timeline of major events
   */
  enum KeyEvents: PscScenarioDefinition::KeyEventId
  {
    E1_Incident_Start,
    E2_TaskForce_Arrive,
    E3_SWAT_Arrive,
    E4_FireEMS_Arrive,
    E5_UC_Setup,
    E6_Perimeters_Ready,
    E7_SWAT_Deploy,
    E8_Offender_Found,
    E9_Interdiction,
    E10_Incident_Secured,
    E11_EMS_Departure,
    E12_Incident_End
  };

  /**
   * Default constructor.
   */
  SchoolShootingDefinitionHelper ();

  /**
   * Default destructor.
   */
  ~SchoolShootingDefinitionHelper ();

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Assign fixed stream numbers to random variables. 
   * \param stream The first stream index to use
   * \return The number of stream indices assigned
   */
  int64_t AssignStreams (int64_t stream);

  /**
   * Creates a PscScenarioDefinition based on current settings
   * \param name Name of the scenario (used as prefix for traces)
   */
  void CreateScenario (std::string name);
  
  /**
   * Returns the scenario definition
   * \return The scenario definition
   */
  Ptr<PscScenarioDefinition> GetScenarioDefinition () const;
  
private:
  /**
   * Initialize the areas
   */
  void InitAreas ();

  /**
   * Initialize the buildings
   */
  void InitStructures ();

  /**
   * Initialize the key events occurring in the scenario
   */
  void InitKeyEvents ();

  /**
   * Initialize the nodes and their mobility
   */
  void InitNodesAndMobility ();

  /**
   * Pointer to the scenario definition
   */
  Ptr<PscScenarioDefinition> m_scenarioDefinition;  
    
  /**
   * Move a node to the given position starting at the given time, then wait
   * for the given amount of time.
   *
   * \param waypointMm The way point mobility model associated with the node
   * \param t The time to start moving
   * \param pos The destination
   * \param waitTime Time to wait at the given position
   * \return The time the node should start moving after waiting
  */
  double AddWayPoint (Ptr<WaypointMobilityModel> waypointMm, double t, Vector pos, double waitTime);

  /**
   * Initialize Unified Command center assets and personnel
   */
  void InitializeUC ();

  /**
   * Initialize SWAT teams assets and personnel
   */
  void InitializeSwatTeams ();

  /**
   * Initialize Task Force assets and personnel
   */
  void InitializeTaskForce ();

  /**
   * Initialize Fire and EMS asssets and personnel
   */
  void InitializeFireEms ();

  /**
   * Initialize the Helicopter
   */
  void InitializeHelicopter ();

  /**
   * Initialize school and traffic cameras
   */
  void InitializeSchoolAndTrafficCameras ();

  /**
   * Initialize the offender node
   */
  void InitializeOffender ();

  Ptr<UniformRandomVariable> m_uniformRandomVariable; ///< Provides uniform random variables.

  bool m_createClassrooms; ///< Indicates if classrooms should be created

}; // class PscScenarioHelper

} // namespace psc
} // namespace ns3

#endif // SCHOOLSCHOOTING_DEFINITION_HELPER_H
