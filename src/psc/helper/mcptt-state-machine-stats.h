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

#ifndef MCPTT_STATE_MACHINE_STATS_H
#define MCPTT_STATE_MACHINE_STATS_H

#include <fstream>
#include <ns3/object.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup psc
 *
 * A class used to capture MCPTT messages.
 */
class McpttStateMachineStats : public Object
{
public:
  /**
   * Gets the type ID of the McpttStateMachineStats class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttStateMachineStats class.
   */
  McpttStateMachineStats (void);
  /**
   * \brief The destructor of the McpttStateMachineStats class.
   */
  virtual ~McpttStateMachineStats (void);
  /**
   * Gets the type ID of this  instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * The callback for capturing state changes
   * \param userId The user ID of the state machine.
   * \param callId The call ID of the state machine.
   * \param selected Indicator whether the call is selected on the client
   * \param typeId The type ID of the state machine.
   * \param oldStateName The name of the previous state.
   * \param newStateName THe name of the current state.
   */
  virtual void StateChangeCb (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName);

private:
  bool m_firstCb; //!< Flag that indicates if the callback has been fired yet.
  std::string m_outputFileName; //!< The file name of the trace file.
  std::ofstream m_outputFile; //!< The file stream object of trace file
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_STATE_MACHINE_STATS_H */

