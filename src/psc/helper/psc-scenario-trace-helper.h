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

#ifndef PSC_SCENARIO_TRACE_HELPER_H
#define PSC_SCENARIO_TRACE_HELPER_H

#include "ns3/packet.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/output-stream-wrapper.h"

#include "ns3/psc-scenario-definition.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class to provide the trace sinks for the incident modeling
 * in a single place.
 */

class PscScenarioTraceHelper : public Object
{
public:
  /**
   * Default constructor
   */
  PscScenarioTraceHelper ();

  /**
   * Constructor for a given scenario
   * \param scenario The scenario definition
   */
  PscScenarioTraceHelper (Ptr<PscScenarioDefinition> scenarioDefinition);

  /**
   * Default destructor
   */
  ~PscScenarioTraceHelper ();

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Enable all the traces of the PscScenarioDefinition
   */
  void EnableScenarioTraces ();

  /**
   * Print the information about areas defined in the incident
   */
  void EnableAreasTrace ();

  /**
   * Print the information about the structures defined in the incident
   */
  void EnableStructuresTrace ();

  /**
   * Print the information about the nodes defined in the incident
   */
  void EnableNodesTrace ();

  /**
   * Print the information about the events defined in the incident
   */
  void EnableEventsTrace ();

  /**
   * Print the information about the simulation time.
   *
   * \param reportTime The interval at which to report the time.
   */
  void EnableTimeTrace (const Time& reportTime);

protected:
  /**
   * Traces the simulation time.
   *
   * \param reportTime The interval at which to report the time.
   */
  void TraceTime (const Time& reportTime);


private:
  /**
   * The scenario helper
   * @param outputPrecision
   */
  Ptr<PscScenarioDefinition> m_scenarioDefinition;

  /**
  * OutputStreamWrapper for writing the areas' coordinates.
  */
  Ptr<OutputStreamWrapper> m_areasOutputStreamWrapper;
  /**
   * OutputStreamWrapper for writing the structures' coordinates.
   */
  Ptr<OutputStreamWrapper> m_structuresOutputStreamWrapper;
  /**
   * OutputStreamWrapper for writing the nodes' information.
   */
  Ptr<OutputStreamWrapper> m_nodesOutputStreamWrapper;
  /**
   * OutputStreamWrapper for writing the events' information.
   */
  Ptr<OutputStreamWrapper> m_eventsOutputStreamWrapper;
  /**
   * OutputStreamWrapper for writing the scenario configuration.
   */
  Ptr<OutputStreamWrapper> m_configOutputStreamWrapper;
  /**
   * OutputStreamWrapper for writing the current second of the simulation.
   */
  Ptr<OutputStreamWrapper> m_timeOutputStreamWrapper;

  /**
   * Set the precision to use when printing double numbers.
   *
   * \param outputPrecision The precision to use
   */
  void SetOutputPrecision (uint16_t outputPrecision);
  /**
   * Get the precision to use when printing double numbers.
   *
   * \return The precision.
   */
  uint16_t GetOutputPrecision (void) const;

/**
 * Precision to use when printing decimal numbers. This value includes the digits
 * of the integer part.
 */
  static uint16_t m_outputPrecision;

}; // PscScenarioTraceHelper

} // namespace psc
} // namespace ns3

#endif // PSC_SCENARIO_TRACE_HELPER_H

