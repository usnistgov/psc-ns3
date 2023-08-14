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

#ifndef MCPTT_FLOOR_MSG_SINK_H
#define MCPTT_FLOOR_MSG_SINK_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This interface exists to create a common API for the MCPTT floor control
 * messages to be received.
 */
class McpttFloorMsgSink
{
public:
  /**
   * \brief Creates an instance of the McpttFloorMsgSink class.
   */
  McpttFloorMsgSink (void);
  /**
   * \brief The destructor of the McpttFloorMsgSink class.
   */
  virtual ~McpttFloorMsgSink (void);
  /**
   * Receives a message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttFloorMsg& msg);
  /**
   * \brief Receives a message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Receives a floor ack message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorAck (const McpttFloorMsgAck& msg);
  /**
   * Receives a floor deny message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (const McpttFloorMsgDeny& msg);
  /**
   * Receives a floor granted message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (const McpttFloorMsgGranted& msg);
  /**
   * Receives a floor idle message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (const McpttFloorMsgIdle& msg);
  /**
   * Receives Floor Queue Position Request message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (const McpttFloorMsgQueuePositionRequest& msg);
  /**
   * Receives a floor queue position info message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (const McpttFloorMsgQueuePositionInfo& msg);
  /**
   * Receives a floor release message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (const McpttFloorMsgRelease& msg);
  /**
   * Receives a floor request message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (const McpttFloorMsgRequest& msg);
  /**
   * Receives a floor revoke message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (const McpttFloorMsgRevoke& msg);
  /**
   * Receives a floor taken message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (const McpttFloorMsgTaken& msg);
  /**
   * Receives an RTP media message.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (const McpttMediaMsg& msg);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_FLOOR_MSG_SINK_H */

