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

#ifndef PSC_SCENARIO_DEFINITION_H
#define PSC_SCENARIO_DEFINITION_H

#include <stdint.h>
#include <string>
#include <map>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/node-container.h>
#include <ns3/building-container.h>
#include <ns3/application-container.h>
#include <ns3/rectangle.h>
#include <ns3/traced-callback.h>
#include <ns3/nstime.h>
#include <ns3/psc-application.h>

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class that gathers information about number of the area, buildings,
 * nodes and their associated mobility.
 */
class PscScenarioDefinition : public Object
{
public:
  /**
   * Type for identifying the various areas in the scenario
   */
  typedef uint16_t AreaId;

  /**
   * Type for identifying the various structures in the scenario
   */
  typedef uint16_t StructureId;

  /**
   * Type for identifying the various groups of nodes in the scenario
   */
  typedef uint16_t GroupId;

  /**
  * Type for identifying key events occurring in the scenario
  */
  typedef uint16_t KeyEventId;

  /**
   * Type for identifying the various applications in the scenario
   */
  typedef uint16_t ApplicationId;

  /**
   * Defines the structure of an area
   */
  struct AreaInfo
  {
    std::string name; ///< The name of the area
    Rectangle area; ///< The coordinates of the area
  };

  /**
   * Defines the structure of a Structure
   */
  struct StructureInfo
  {
    std::string name; ///< The name of the structure
    BuildingContainer buildings; ///< The buildings making up the structure
  };

  /**
   * Defines the structure of a group of nodes
   */
  struct NodeGroupInfo
  {
    std::string name; ///< The name of the group
    NodeContainer nodes; ///< The list of nodes that are part of the group
  };

  /**
   * Defines the structure of a major event happening in the scenario
   */
  struct KeyEventInfo
  {
    std::string name; ///< The name of the event
    Time time;     ///< The time of the event
  };

  /**
   * Defines the structure for an application information
   */
  struct ApplicationInfo
  {
    std::string name; ///< The name of the application
  };

  /**
   * Default constructor.
   */
  PscScenarioDefinition ();

  /**
   * Default destructor.
   */
  virtual ~PscScenarioDefinition ();

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Sets the scenario name, which is used for naming the trace files.
   * This function should be called before the scenario is initialized
   * \param name The scenario name
   */
  void SetName (std::string name);

  /**
   * Gets the name of the scenario, which is used for naming the trace files
   * \return The name of the scenario
   */
  std::string GetName () const;

  /**
   * Add a new area
   * @param id The area ID
   * @param area The area information (name and location)
   */
  void AddArea (AreaId id, AreaInfo area);

  /**
   * Gets the area with the given name
   * \param id The ID of the area
   * \return The area
   */
  AreaInfo GetArea (AreaId id) const;

  /**
   * Gets all the areas defined in the scenario
   * \return All the areas defined
   */
  std::map<AreaId, AreaInfo> GetAreas () const;
  
  /**
   * Add a new structure
   * @param id The structure ID
   * @param buildings The building information
   */
  void AddStructure (StructureId id, StructureInfo buildings);

  /**
   * Gets the buildings with the given name
   * \param id The ID of the building
   * \return The building
   */
  StructureInfo GetStructure (StructureId id) const;

  /**
   * Gets all the structures defined in the scenario
   * \return All the structures defined
   */
  std::map<StructureId, StructureInfo> GetStructures () const;
  
  /**
   * Add a new group of nodes
   * @param id The group ID
   * @param nodes The group information
   */
  void AddNodes (GroupId id, NodeGroupInfo nodes);

  /**
   * Gets the nodes belonging to the given group
   * \param id The ID of the group
   * \return The node container
   */
  NodeGroupInfo GetNodesForGroup (GroupId id) const;

  /**
   * Gets all the nodes defined in the scenario
   * \return All the nodes defined
   */
  std::map<GroupId, NodeGroupInfo> GetNodes () const;
  
  /**
   * Add a new key event
   * @param id The event ID
   * @param event The event information
   */
  void AddKeyEvent (KeyEventId id, KeyEventInfo event);

  /**
   * Gets the time of the given key event
   * \param id The ID of the KeyEvent
   * \return The time of the event
   */
  KeyEventInfo GetKeyEventTime (KeyEventId id) const;

  /**
   * Gets all the key events defined in the scenario
   * \return All the key events defined
   */
  std::map<KeyEventId, KeyEventInfo> GetKeyEvents () const;
  
  /**
   * Add a new application
   * @param id The application ID
   * @param application The application information
   */
  void AddApplication (ApplicationId id, ApplicationInfo application);

  /**
   * Gets the application with the given name
   * \param id The ID of the application
   * \return The application information
   */
  ApplicationInfo GetApplication (ApplicationId id) const;
  
  /**
   * Gets all the applications defined in the scenario
   * \return All the areas defined
   */
  std::map<ApplicationId, ApplicationInfo> GetApplications () const;
  
private:
  /**
   * The name of the scenario
   */
  std::string m_name;

  /**
    * Areas.
    */
  std::map<AreaId, AreaInfo> m_areas;

  /**
   * Structures. Grouped by name so that an actual building can be made up of
   * multiple ns3 buildings if necessary
   */
  std::map<StructureId, StructureInfo> m_structures;

  /**
   * Map with the NodeContainer for each group in the scenario.
   */
  std::map<GroupId, NodeGroupInfo> m_nodeGroups;

  /**
   * Key events
   */
  std::map<KeyEventId, KeyEventInfo> m_keyEvents;

  /**
    * Applications.
    */
  std::map<ApplicationId, ApplicationInfo> m_applications;

}; // class PscScenarioDefinition

} // namespace psc
} // namespace ns3

#endif // PSC_SCENARIO_DEFINITION_H
