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

#include "psc-scenario-definition.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscScenarioDefinition");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscScenarioDefinition);

PscScenarioDefinition::PscScenarioDefinition ()
  : m_name ("undefined")
{
  NS_LOG_FUNCTION (this);

}

PscScenarioDefinition::~PscScenarioDefinition ()
{
  NS_LOG_FUNCTION (this);
  m_areas.clear ();
  m_structures.clear ();
  m_nodeGroups.clear ();
  m_keyEvents.clear ();
  m_applications.clear ();

}

TypeId
PscScenarioDefinition::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscScenarioDefinition")
    .SetParent<Object> ()
    .AddConstructor<PscScenarioDefinition> ()
  ;

  return tid;
}

void
PscScenarioDefinition::SetName (std::string name)
{
  NS_LOG_FUNCTION (this << name);
  m_name = name;
}

std::string
PscScenarioDefinition::GetName () const
{
  NS_LOG_FUNCTION (this);
  return m_name;
}

void
PscScenarioDefinition::AddArea (AreaId id, AreaInfo area)
{
  NS_LOG_FUNCTION (this << id);
  NS_ASSERT_MSG (m_areas.find (id) == m_areas.end (), "Area " << id << " already defined");
  m_areas.insert (std::pair<AreaId, AreaInfo> (id, area));
}

PscScenarioDefinition::AreaInfo
PscScenarioDefinition::GetArea (AreaId id) const
{
  NS_LOG_FUNCTION (this << id);

  auto it = m_areas.find (id);
  NS_ASSERT_MSG (it != m_areas.end (), "Area " << id << " not found in scenario");
  return it->second;
}

std::map<PscScenarioDefinition::AreaId, PscScenarioDefinition::AreaInfo>
PscScenarioDefinition::GetAreas () const
{
  NS_LOG_FUNCTION (this);

  return m_areas;
}

void
PscScenarioDefinition::AddStructure (StructureId id, StructureInfo buildings)
{
  NS_LOG_FUNCTION (this << id);
  NS_ASSERT_MSG (m_structures.find (id) == m_structures.end (), "Structure " << id << " already defined");
  m_structures.insert (std::pair<StructureId, StructureInfo> (id, buildings));
}

PscScenarioDefinition::StructureInfo
PscScenarioDefinition::GetStructure (StructureId id) const
{
  NS_LOG_FUNCTION (this << id);

  auto it = m_structures.find (id);
  NS_ASSERT_MSG (it != m_structures.end (), "Structure " << id << " not found in scenario");
  return it->second;
}

std::map<PscScenarioDefinition::StructureId, PscScenarioDefinition::StructureInfo>
PscScenarioDefinition::GetStructures () const
{
  NS_LOG_FUNCTION (this);

  return m_structures;
}

void
PscScenarioDefinition::AddNodes (GroupId id, NodeGroupInfo nodes)
{
  NS_LOG_FUNCTION (this << id);
  NS_ASSERT_MSG (m_nodeGroups.find (id) == m_nodeGroups.end (), "Group " << id << " already defined");
  m_nodeGroups.insert (std::pair<GroupId, NodeGroupInfo> (id, nodes));
}

PscScenarioDefinition::NodeGroupInfo
PscScenarioDefinition::GetNodesForGroup (GroupId id) const
{
  NS_LOG_FUNCTION (this << id);

  auto it = m_nodeGroups.find (id);
  NS_ASSERT_MSG (it != m_nodeGroups.end (), "Group " << id << " not found in scenario");
  return it->second;
}

std::map<PscScenarioDefinition::GroupId, PscScenarioDefinition::NodeGroupInfo>
PscScenarioDefinition::GetNodes () const
{
  NS_LOG_FUNCTION (this);

  return m_nodeGroups;
}

void
PscScenarioDefinition::AddKeyEvent (KeyEventId id, KeyEventInfo event)
{
  NS_LOG_FUNCTION (this << id);
  NS_ASSERT_MSG (m_keyEvents.find (id) == m_keyEvents.end (), "Event " << id << " already defined");
  m_keyEvents.insert (std::pair<KeyEventId, KeyEventInfo> (id, event));
}

PscScenarioDefinition::KeyEventInfo
PscScenarioDefinition::GetKeyEventTime (KeyEventId id) const
{
  NS_LOG_FUNCTION (this << id);

  auto it = m_keyEvents.find (id);
  NS_ASSERT_MSG (it != m_keyEvents.end (), "Event " << id << " not found in scenario");
  return it->second;
}

std::map<PscScenarioDefinition::KeyEventId, PscScenarioDefinition::KeyEventInfo>
PscScenarioDefinition::GetKeyEvents () const
{
  NS_LOG_FUNCTION (this);

  return m_keyEvents;
}

void
PscScenarioDefinition::AddApplication (ApplicationId id, ApplicationInfo application)
{
  NS_LOG_FUNCTION (this << id);
  NS_ASSERT_MSG (m_applications.find (id) == m_applications.end (), "Application " << id << " already defined");
  m_applications.insert (std::pair<ApplicationId, ApplicationInfo> (id, application));
}

PscScenarioDefinition::ApplicationInfo
PscScenarioDefinition::GetApplication (ApplicationId id) const
{
  NS_LOG_FUNCTION (this << id);

  auto it = m_applications.find (id);
  NS_ASSERT_MSG (it != m_applications.end (), "Application " << id << " not found in scenario");
  return it->second;
}

std::map<PscScenarioDefinition::ApplicationId, PscScenarioDefinition::ApplicationInfo>
PscScenarioDefinition::GetApplications () const
{
  NS_LOG_FUNCTION (this);

  return m_applications;
}

} // namespace psc
} // namespace ns3
