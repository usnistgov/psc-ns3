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

#ifndef MCPTT_FLOOR_PARTICIPANT_H
#define MCPTT_FLOOR_PARTICIPANT_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-floor-msg-sink.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This interface exists to create a common API for the MCPTT floor participants
 * described in TS 24.380.
 */
class McpttFloorParticipant : public Object,
                              public McpttFloorMsgSink
{
public:
  /**
   * \brief Gets the ID of the McpttFloorParticipant type.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttFloorParticipant class.
   */
  McpttFloorParticipant (void);
  /**
   * \brief The destructor of the McpttFloorParticipant class.
   */
  virtual ~McpttFloorParticipant (void);
  /**
   * Accepts the floor grant.
   */
  virtual void AcceptGrant (void) = 0;
  /**
   * Gets the type ID of this McpttFloorParticipant instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates whether or not the current participant is the acting arbitrator.
   * \returns True, if the current participant is the acting arbitrator.
   */
  virtual bool HasFloor (void) const = 0;
  /**
   * Indicates whether or not the floor machine has been started.
   * \returns True, if the floor machine has been started.
   */
  virtual bool IsStarted (void) const = 0;
  /**
   * Indicates that media is ready to be sent.
   * \param msg The media that is ready to be sent.
   */
  virtual void MediaReady (McpttMediaMsg& msg) = 0;
  /**
  * Releases a request.
  */
  virtual void ReleaseRequest (void) = 0;
  /**
   * Sends a floor queue position request message.
   */
  virtual void SendFloorQueuePositionRequest (void) = 0;
  /**
   * Sets the callback used to indicate that the floor has been granted.
   * \param floorGrantedCb The callback to set
   */
  virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb) = 0;
  /**
   * Sets the flag that indicates if this floor machine is the originator of the call.
   * \param originator True, if this participant is the originator of the call.
   */
  virtual void SetOriginator (const bool& originator) = 0;
  /**
   * Sets the McpttCall associated with the floor machine.
   * \param call The call.
   */
  virtual void SetCall (Ptr<McpttCall> call) = 0;
  /**
  * Sets the current priority of the floor.
  * \param priority The current priority.
  */
  virtual void SetPriority (uint8_t priority) = 0;
  /**
   * Starts the floor machine.
   */
  virtual void Start (void) = 0;
  /**
   * Stops the floor machine.
   */
  virtual void Stop (void) = 0;
  /**
   * Notifies this machine that the button has been pushed.
   */
  virtual void PttPush (void) = 0;
  /**
   * Notifies this machine that the button has been released.
   */
  virtual void PttRelease (void) = 0;

  /**
   * TracedCallback signature for state change traces
   * \param [in] userId User ID
   * \param [in] callId Call ID
   * \param [in] selected Indicator whether the call is selected by the client
   * \param [in] typeId Type ID of the class reporting the change
   * \param [in] oldStateName Name of the old state
   * \param [in] newStateName Name of the new state
   */
  typedef void (* StateChangeTracedCallback) (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName);

  // Events to report
  static constexpr const char* PTT_BUTTON_PUSHED = "PTT button pushed";
  static constexpr const char* PTT_BUTTON_RELEASED = "PTT button released";
  static constexpr const char* RECEIVED_FLOOR_DENY = "Received Floor Deny";
  static constexpr const char* RECEIVED_FLOOR_GRANTED = "Received Floor Granted";
  static constexpr const char* RECEIVED_FLOOR_QUEUE = "Received Floor Queue Position Info";
  static constexpr const char* RECEIVED_FLOOR_IDLE = "Received Floor Idle";
  static constexpr const char* RECEIVED_FLOOR_REVOKED = "Received Floor Revoked";
  static constexpr const char* RECEIVED_FLOOR_TAKEN = "Received Floor Taken";
  static constexpr const char* TIMER_T100_EXPIRED = "Timer T100 expired";
  static constexpr const char* TIMER_T100_EXPIRED_N_TIMES = "Timer T100 expired N times";
  static constexpr const char* TIMER_T101_EXPIRED = "Timer T101 expired";
  static constexpr const char* TIMER_T101_EXPIRED_N_TIMES = "Timer T101 expired N times";
  static constexpr const char* TIMER_T103_EXPIRED = "Timer T103 expired";
  static constexpr const char* TIMER_T203_EXPIRED = "Timer T203 expired";
  static constexpr const char* TIMER_T104_EXPIRED = "Timer T104 expired";
  static constexpr const char* TIMER_T104_EXPIRED_N_TIMES = "Timer T104 expired N times";
  static constexpr const char* TIMER_T204_EXPIRED = "Timer T204 expired";
  static constexpr const char* TIMER_T204_EXPIRED_N_TIMES = "Timer T204 expired N times";
  static constexpr const char* TIMER_T132_EXPIRED = "Timer T132 expired";
  static constexpr const char* TIMER_T233_EXPIRED = "Timer T233 expired";
};

/**
 * \ingroup mcptt
 * \brief A class that represents an MCPTT floor machine.
 */
class McpttFloorParticipantNull : public McpttFloorParticipant
{
public:
  /**
   * \brief Gets the ID of the McpttFloorParticipant type.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttFloorParticipantNull class.
   */
  McpttFloorParticipantNull (void);
  /**
   * \brief The destructor of the McpttFloorParticipantNull class.
   */
  virtual ~McpttFloorParticipantNull (void);
  /**
   * Accepts the floor grant.
   */
  virtual void AcceptGrant (void);
  /**
   * Gets the type ID of this McpttFloorParticipant instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates whether or not the current participant is the acting arbitrator.
   * \returns True, if the current participant is the acting arbitrator.
   */
  virtual bool HasFloor (void) const;
  /**
   * Indicates whether or not the floor machine has been started.
   * \returns True, if the floor machine has been started.
   */
  virtual bool IsStarted (void) const;
  /**
   * Indicates that media is ready to be sent.
   * \param msg The media that is ready to be sent.
   */
  virtual void MediaReady (McpttMediaMsg& msg);
  /**
   * Releases a request.
   */
  virtual void ReleaseRequest (void);
  /**
   * Sends a floor queue position request message.
   */
  virtual void SendFloorQueuePositionRequest (void);
  /**
   * Sets the callback used to indicate that the floor has been granted.
   * \param floorGrantedCb The callback to set
   */
  virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb);
  /**
  * Sets the flag that indicates if this floor machine is the originator of the call.
  * \param originator True, if this participant is the originator of the call.
  */
  virtual void SetOriginator (const bool& originator);
  /**
   * Sets the McpttCall associated with the machine.
   * \param call The call.
   */
  virtual void SetCall (Ptr<McpttCall> call);
  /**
   * Sets the current priority of the floor.
   * \param priority The current priority.
   */
  virtual void SetPriority (uint8_t priority);
  /**
   * Starts the floor machine.
   */
  virtual void Start (void);
  /**
   * Stops the floor machine.
   */
  virtual void Stop (void);
  /**
   * Notifies this machine that the button has been pushed.
   */
  virtual void PttPush (void);
  /**
   * Notifies this machine that the button has been released.
   */
  virtual void PttRelease (void);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_FLOOR_PARTICIPANT_H */

