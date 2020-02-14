
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

#ifndef MCPTT_TRACE_HELPER_H
#define MCPTT_TRACE_HELPER_H

#include <stdint.h>
#include <string>
#include <fstream>
#include <utility>
#include <map>

#include <ns3/ptr.h>
#include <ns3/nstime.h>

namespace ns3 {

class McpttMsg;
class McpttMsgStats;
class McpttStateMachineStats;
class ApplicationContainer;

/**
 * \ingroup psc
 * \brief A helper for creating, configuring, and installing MCPTT apps.
 */
class McpttTraceHelper
{
public:
 /**
  * \brief Creates an instance of the McpttHelper class.
  */
 McpttTraceHelper (void);
 /**
  * \brief Destructor of the McpttHelper class.
  */
 virtual ~McpttTraceHelper (void);
 /**
  * Enables the MCPTT message trace at the application layer.
  */
 virtual void EnableMsgTraces (void);
 /**
  * Disables the MCPTT message trace at the application layer.
  */
 virtual void DisableMsgTraces (void);
 /**
  * Enables the MCPTT state machine traces.
  */
 virtual void EnableStateMachineTraces (void);
 /**
  * Disables the MCPTT state machine traces.
  */
 virtual void DisableStateMachineTraces (void);
 /**
  * Enables a trace for MCPTT mouth-to-ear latency statistics
  * \param filename Filename to open for writing the trace
  */
 virtual void EnableMouthToEarLatencyTrace (std::string filename);
 /**
  * Disables any traces for MCPTT mouth-to-ear latency statistics
  */
 virtual void DisableMouthToEarLatencyTrace (void);
private:
 Ptr<McpttMsgStats> m_msgTracer; //!< The object used to trace MCPTT messages.
 Ptr<McpttStateMachineStats> m_stateMachineTracer; //!< The object used to trace MCPTT state machine traces.
 std::map<std::pair<uint32_t, uint16_t>, Time> m_mouthToEarLatencyMap;
 std::ofstream m_mouthToEarLatencyTraceFile; //!< file stream for latency trace

  void TraceMcpttMediaMsg (Ptr<const Application> app, uint16_t callId, const McpttMsg& msg);
};

} // namespace ns3

#endif /* MCPTT_TRACE_HELPER_H */

