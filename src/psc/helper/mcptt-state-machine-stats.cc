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

#include <fstream>
#include <iomanip>
#include <string>

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/string.h>

#include "mcptt-state-machine-stats.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttStateMachineStats");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttStateMachineStats);

TypeId
McpttStateMachineStats::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttStateMachineStats")
    .SetParent<Object> ()
    .AddConstructor<McpttStateMachineStats> ()
    .AddAttribute ("OutputFileName",
                   "The name to use for the trace file.",
                   StringValue ("mcptt-state-machine-stats.txt"),
                   MakeStringAccessor (&McpttStateMachineStats::m_outputFileName),
                   MakeStringChecker ())
  ;

  return tid;
}

McpttStateMachineStats::McpttStateMachineStats (void)
  : Object (),
    m_firstCb (true)
{
  NS_LOG_FUNCTION (this);
}

McpttStateMachineStats::~McpttStateMachineStats (void)
{
  NS_LOG_FUNCTION (this);
  if (m_outputFile.is_open ())
    {
      m_outputFile.close ();
    }
}

TypeId
McpttStateMachineStats::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttStateMachineStats::GetTypeId ();
}

void
McpttStateMachineStats::StateChangeCb (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName)
{
  NS_LOG_FUNCTION (this << userId << callId << selected << typeId << oldStateName << newStateName);
  if (m_firstCb == true)
    {
      m_firstCb = false;
      m_outputFile.open (m_outputFileName.c_str ());
      m_outputFile << "#";
      m_outputFile << std::setw (9) << "time(s)";
      m_outputFile << std::setw (7) << "userid";
      m_outputFile << std::setw (7) << "callid";
      m_outputFile << std::setw (9) << "selected";
      m_outputFile << std::setw (38) << "typeid";
      m_outputFile << std::setw (36) << "oldstate";
      m_outputFile << std::setw (36) << "newstate";
      m_outputFile << std::endl;
    }
  m_outputFile << std::fixed << std::setw (10) << Simulator::Now ().GetSeconds ();
  m_outputFile << std::setw (6) << userId;
  m_outputFile << std::setw (6) << callId;
  m_outputFile << std::setw (9) << selected;
  m_outputFile << std::setw (40) << typeId.substr (10); // trim leading 'ns3::psc::'
  m_outputFile << std::setw (36) << oldStateName;
  m_outputFile << std::setw (36)  << newStateName;
  m_outputFile << std::endl;
}

} // namespace psc
} // namespace ns3

