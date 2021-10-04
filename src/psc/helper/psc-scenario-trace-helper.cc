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

#include "psc-scenario-trace-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscScenarioTraceHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscScenarioTraceHelper);

uint16_t PscScenarioTraceHelper::m_outputPrecision = 11;

PscScenarioTraceHelper::PscScenarioTraceHelper () :
  m_scenarioDefinition (nullptr),
  m_areasOutputStreamWrapper (nullptr),
  m_structuresOutputStreamWrapper (nullptr),
  m_nodesOutputStreamWrapper (nullptr),
  m_eventsOutputStreamWrapper (nullptr),
  m_configOutputStreamWrapper (nullptr)
{
  NS_LOG_FUNCTION (this);
}

PscScenarioTraceHelper::PscScenarioTraceHelper (Ptr<PscScenarioDefinition> scenarioDefinition)
{
  NS_LOG_FUNCTION (this << scenarioDefinition);
  m_scenarioDefinition = scenarioDefinition;
}


PscScenarioTraceHelper::~PscScenarioTraceHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_scenarioDefinition = nullptr;
  m_areasOutputStreamWrapper = nullptr;
  m_structuresOutputStreamWrapper = nullptr;
  m_nodesOutputStreamWrapper = nullptr;
  m_eventsOutputStreamWrapper = nullptr;
  m_configOutputStreamWrapper = nullptr;
}

TypeId
PscScenarioTraceHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscScenarioTraceHelper")
    .SetParent<Object> ()
    .AddConstructor<PscScenarioTraceHelper> ()
    .AddAttribute ("OutputPrecision",
                   "Precision to use when printing decimal numbers (i.e., the timestamp), including the digits for the integer part",
                   UintegerValue (11),
                   MakeUintegerAccessor (
                     &PscScenarioTraceHelper::SetOutputPrecision,
                     &PscScenarioTraceHelper::GetOutputPrecision),
                   MakeUintegerChecker<uint16_t> ())
  ;

  return tid;
}

void
PscScenarioTraceHelper::EnableScenarioTraces ()
{
  NS_LOG_FUNCTION (this);
  EnableAreasTrace ();
  EnableStructuresTrace ();
  EnableNodesTrace ();
  EnableEventsTrace ();
}

void
PscScenarioTraceHelper::EnableAreasTrace ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (!m_areasOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableAreasTrace () only once");

  m_areasOutputStreamWrapper = Create<OutputStreamWrapper> (m_scenarioDefinition->GetName () + "-areas.txt", std::ios::out);

  (*m_areasOutputStreamWrapper->GetStream ()).precision (m_outputPrecision);

  std::map<PscScenarioDefinition::AreaId, PscScenarioDefinition::AreaInfo> areas = m_scenarioDefinition->GetAreas ();
  std::map<PscScenarioDefinition::AreaId, PscScenarioDefinition::AreaInfo>::iterator it;

  //print header line
  *m_areasOutputStreamWrapper->GetStream () << "ID\tName\txMin\txMax\tyMin\tyMax" << std::endl;

  for (it = areas.begin (); it != areas.end (); it++)
    {
      //Fix the names in case there are spaces for easier parsing of the trace file
      std::string nameFixed;
      std::replace_copy (it->second.name.begin (), it->second.name.end (), std::back_inserter<std::string> (nameFixed), ' ', '_');
      *m_areasOutputStreamWrapper->GetStream () << it->first
                                                << "\t" << nameFixed
                                                << "\t" << it->second.area.xMin
                                                << "\t" << it->second.area.xMax
                                                << "\t" << it->second.area.yMin
                                                << "\t" << it->second.area.yMax << std::endl;
    }

  m_areasOutputStreamWrapper->GetStream ()->flush ();
}

void
PscScenarioTraceHelper::EnableStructuresTrace ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (!m_structuresOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableStructuresTrace () only once");

  m_structuresOutputStreamWrapper = Create<OutputStreamWrapper> (m_scenarioDefinition->GetName () + "-structures.txt", std::ios::out);

  (*m_structuresOutputStreamWrapper->GetStream ()).precision (m_outputPrecision);

  //print header line
  *m_structuresOutputStreamWrapper->GetStream () << "ID\tName\txMin\txMax\tyMin\tyMax\tzMin\tzMax" << std::endl;

  //loop through the structures
  std::map<PscScenarioDefinition::StructureId, PscScenarioDefinition::StructureInfo> structures = m_scenarioDefinition->GetStructures ();
  std::map<PscScenarioDefinition::StructureId, PscScenarioDefinition::StructureInfo>::iterator it;
  for (it = structures.begin (); it != structures.end (); it++)
    {
      PscScenarioDefinition::StructureInfo info = it->second;
      //loop through the buildings
      for (uint32_t i = 0; i < info.buildings.GetN (); i++)
        {
          //Fix the names in case there are spaces for easier parsing of the trace file
          std::string nameFixed;
          std::replace_copy (it->second.name.begin (), it->second.name.end (), std::back_inserter<std::string> (nameFixed), ' ', '_');
          *m_structuresOutputStreamWrapper->GetStream () << it->first
                                                         << " " << nameFixed
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().xMin
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().xMax
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().yMin
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().yMax
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().zMin
                                                         << "\t" << info.buildings.Get (i)->GetBoundaries ().zMax
                                                         << std::endl;
        }
    }

  m_structuresOutputStreamWrapper->GetStream ()->flush ();
}


void
PscScenarioTraceHelper::EnableNodesTrace ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (!m_nodesOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableNodesTrace () only once");

  m_nodesOutputStreamWrapper = Create<OutputStreamWrapper> (m_scenarioDefinition->GetName () + "-nodes.txt", std::ios::out);

  (*m_nodesOutputStreamWrapper->GetStream ()).precision (m_outputPrecision);

  //print header line
  *m_nodesOutputStreamWrapper->GetStream () << "GroupID\tGroupName\tUID\tX\tY\tZ" << std::endl;

  //loop through the groups
  std::map<PscScenarioDefinition::GroupId, PscScenarioDefinition::NodeGroupInfo> nodesInfo = m_scenarioDefinition->GetNodes ();
  std::map<PscScenarioDefinition::GroupId, PscScenarioDefinition::NodeGroupInfo>::iterator it;
  for (it = nodesInfo.begin (); it != nodesInfo.end (); it++)
    {
      PscScenarioDefinition::NodeGroupInfo info = it->second;
      //loop through the nodes
      for (uint32_t i = 0; i < info.nodes.GetN (); i++)
        {
          //Fix the names in case there are spaces for easier parsing of the trace file
          std::string nameFixed;
          std::replace_copy (info.name.begin (), info.name.end (), std::back_inserter<std::string> (nameFixed), ' ', '_');
          Vector position =  info.nodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ();
          *m_nodesOutputStreamWrapper->GetStream () << it->first
                                                    << "\t" << nameFixed
                                                    << "\t" << info.nodes.Get (i)->GetId ()
                                                    << "\t" << position.x
                                                    << "\t" << position.y
                                                    << "\t" << position.z << std::endl;
        }
    }

  m_nodesOutputStreamWrapper->GetStream ()->flush ();
}

void
PscScenarioTraceHelper::EnableEventsTrace ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (!m_eventsOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableEventsTrace () only once");

  m_eventsOutputStreamWrapper = Create<OutputStreamWrapper> (m_scenarioDefinition->GetName () + "-events.txt", std::ios::out);

  (*m_eventsOutputStreamWrapper->GetStream ()).precision (m_outputPrecision);

  //print header line
  *m_eventsOutputStreamWrapper->GetStream () << "ID\tName\tTime" << std::endl;

  std::map<PscScenarioDefinition::KeyEventId, PscScenarioDefinition::KeyEventInfo> events = m_scenarioDefinition->GetKeyEvents ();
  std::map<PscScenarioDefinition::KeyEventId, PscScenarioDefinition::KeyEventInfo>::iterator it;
  for (it = events.begin (); it != events.end (); it++)
    {
      //Fix the names in case there are spaces for easier parsing of the trace file
      std::string nameFixed;
      std::replace_copy (it->second.name.begin (), it->second.name.end (), std::back_inserter<std::string> (nameFixed), ' ', '_');
      *m_eventsOutputStreamWrapper->GetStream () << it->first
                                                 << "\t" << nameFixed
                                                 << "\t" << it->second.time.GetSeconds () << std::endl;
    }

  m_eventsOutputStreamWrapper->GetStream ()->flush ();
}

void
PscScenarioTraceHelper::EnableTimeTrace (const Time& reportTime)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (!m_timeOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableTimeTrace () only once");

  m_eventsOutputStreamWrapper = Create<OutputStreamWrapper> (m_scenarioDefinition->GetName () + "-time.txt", std::ios::out);

  (*m_eventsOutputStreamWrapper->GetStream ()).precision (m_outputPrecision);

  //print header line
  *m_eventsOutputStreamWrapper->GetStream () << "Time(s)" << std::endl;

  Simulator::Schedule (reportTime, &PscScenarioTraceHelper::TraceTime, this, reportTime);

  m_eventsOutputStreamWrapper->GetStream ()->flush ();
}

void
PscScenarioTraceHelper::TraceTime (const Time& reportTime)
{
  NS_ASSERT_MSG (!m_timeOutputStreamWrapper, "Make sure to call PscScenarioTraceHelper::EnableTimeTrace ()");

  *m_eventsOutputStreamWrapper->GetStream () << Simulator::Now ().GetSeconds () << std::endl;

  m_eventsOutputStreamWrapper->GetStream ()->flush ();

  Simulator::Schedule (reportTime, &PscScenarioTraceHelper::TraceTime, this, reportTime);
}

void
PscScenarioTraceHelper::SetOutputPrecision (uint16_t outputPrecision)
{
  NS_LOG_FUNCTION (this << outputPrecision);

  m_outputPrecision = outputPrecision;
}

uint16_t
PscScenarioTraceHelper::GetOutputPrecision (void) const
{
  NS_LOG_FUNCTION (this);

  return m_outputPrecision;
}

} // namespace psc
} // namespace ns3

