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

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/string.h>

#include "mcptt-state-machine-stats.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("McpttStateMachineStats");

NS_OBJECT_ENSURE_REGISTERED (McpttStateMachineStats);

TypeId
McpttStateMachineStats::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttStateMachineStats")
    .SetParent<Object> ()
    .AddConstructor<McpttStateMachineStats> ()
    .AddAttribute ("OutputFileName",
                   "The name to use for the trace file.",
                   StringValue ("mcptt_state_machine_stats.txt"),
                   MakeStringAccessor (&McpttStateMachineStats::m_outputFileName),
                   MakeStringChecker ())
    ;

  return tid;
}

McpttStateMachineStats::McpttStateMachineStats (void)
  : Object (),
    m_firstCb (true),
    m_outputFileName ("mcptt_state_machine_stats.txt")
{
  NS_LOG_FUNCTION (this);
}

McpttStateMachineStats::~McpttStateMachineStats (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttStateMachineStats::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttStateMachineStats::GetTypeId ();
}

void
McpttStateMachineStats::StateChangeCb (uint32_t userId, uint32_t callId, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName)
{
  NS_LOG_FUNCTION (this << userId << callId << typeId << oldStateName << newStateName);
  std::ofstream outFile;
  if (m_firstCb == true)
    {
      m_firstCb = false;
      outFile.open (m_outputFileName.c_str ());
      outFile << "time(ms)\tuserid\tcallid\ttypeid\toldstate\tnewstate";
      outFile << std::endl;
    }
  else
    {
      outFile.open (m_outputFileName.c_str (), std::ios_base::app);
    }
  outFile << Simulator::Now ().GetMilliSeconds ();
  outFile << "\t" << userId;
  outFile << "\t" << callId;
  outFile << "\t" << typeId;
  outFile << "\t" << oldStateName;
  outFile << "\t" << newStateName;
  outFile << std::endl;
  outFile.close ();
}

} // namespace ns3

