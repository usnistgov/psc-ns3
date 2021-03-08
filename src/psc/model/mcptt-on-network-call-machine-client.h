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

#ifndef MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_H
#define MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_H

#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-media-msg.h"
#include "mcptt-call-machine.h"

namespace ns3 {

class RandomVariableStream;

namespace psc {

class McpttCall;
class McpttOnNetworkCallMachineClientState;

/**
 * \ingroup mcptt
 *
 * A class used to represent an on-network client MCPTT call machine.
 */
class McpttOnNetworkCallMachineClient : public McpttCallMachineGrp
{
public:
  /**
   * Gets the type ID of the McpttOnNetworkCallMachineClient class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClient class.
   */
  McpttOnNetworkCallMachineClient (void);
  /**
   * The destructor of the McpttOnNetworkCallMachineClient class.
   */
  virtual ~McpttOnNetworkCallMachineClient (void);
  /**
   * Sets the current state of the call machine.
   * \param state The current state.
   */
  virtual void SetState (Ptr<McpttOnNetworkCallMachineClientState> state);
  /**
   * Gets the current state of the call machine.
   * \returns The current state.
   */
  virtual Ptr<McpttOnNetworkCallMachineClientState> GetState (void) const;
  /**
   * Sets the state change callback.
   * \param stateChangeCb The callback.
   */
  virtual void SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb);
  /**
   * Accepts the call.
   */
  virtual void AcceptCall (void);
  /**
   * Begins an emergency alert.
   */
  virtual void BeginEmergAlert (void);
  /**
   * Cancels an emergency alert.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Downgrades the call type.
   */
  virtual void DowngradeCallType (void);
  /**
   * Gets the ID of the user that started the call.
   * \returns The MCPTT user ID.
   */
  virtual uint32_t GetCallerUserId (void) const;
  /**
   * Gets the ID of the call.
   * \returns The call ID.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the call type.
   * \returns The call type.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the type ID of this McpttOnNetworkCallMachineClient instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the McpttCall for the call machine.
   * \returns The call object
   */
  virtual Ptr<McpttCall> GetCall (void) const;
  /**
   * Gets the ID of the current state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Initiates a call.
   */
  virtual void InitiateCall (void);
  /**
   * Indicates if the call is active.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (void) const;
  /**
   * Indicates if this is a group call.
   * \param grpId The particular group ID to check for when greater than 0.
   * \returns True, if this a group call; otherwise, false.
   */
  virtual bool IsGrpCall (uint32_t grpId = 0) const;
  /**
   * Indicates if this is a private call.
   * \param userId The particular user ID to check for when greater than 0.
   * \returns True, if this a private call; otherwise, false.
   */
  virtual bool IsPrivateCall (uint32_t userId = 0) const;
  /**
   * Receives a call control packet
   * \param pkt The packet received.
   * \param hdr The SIP header received.
   */
  virtual void ReceiveCallPacket (Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Receives a media message.
   * \param msg The message to receive.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Receives a call message.
   * \param msg The message to receive.
   */
  virtual void Receive (const McpttCallMsg& msg);
  /**
   * Releases the call.
   */
  virtual void ReleaseCall (void);
  /**
   * Rejects the call.
   */
  virtual void RejectCall (void);
  /**
   * Sends an off-network call control packet.  Should be unused but is
   * necessary to implement; it generates a fatal error.
   * \param hdr The McpttCallMsg header to send.
   */
  virtual void Send (const McpttCallMsg& hdr);
  /**
   * Sets the ID of the MCPTT call.
   * \param callId The ID of the MCPTT call.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the callback used to indicate that a new call exists.
   * \param newCallCb The callback.
   */
  virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb);
  /**
   * Sets the McpttCall associated with the call machine.
   * \param call The call.
   */
  virtual void SetCall (Ptr<McpttCall> call);
  /**
   * Sets the group ID
   * \param grpId The group ID
   */
  virtual void SetGrpId (uint32_t grpId);
  /**
   * Gets the group ID
   * \return The group ID
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Starts the state machine.
   */
  virtual void Start (void);
  /**
   * Stops the state machine.
   */
  virtual void Stop (void);
  /**
   * Upgrades the call type.
   * \param callType The call type value.
   */
  virtual void UpgradeCallType (uint8_t callType);
  /**
   * Get the notional SIP INVITE payload size
   * \return The notional payload size, in bytes
   */
  uint32_t GetInvitePayloadSize (void);
  /**
   * Get the notional SIP BYE payload size
   * \return The notional payload size, in bytes
   */
  uint32_t GetByePayloadSize (void);
  /**
   * Get the notional SIP 200 OK payload size
   * \return The notional payload size, in bytes
   */
  uint32_t GetResponsePayloadSize (void);
  /**
   * \brief Set the stream for each random variable.
   * \param stream The starting stream number.
   * \returns The number of streams that were set.
   */
  virtual int64_t AssignStreams (int64_t stream);

protected:
  /**
   * Disposes of this McpttOnNetworkCallMachineClient instance.
   */
  virtual void DoDispose (void);

private:
  Ptr<McpttCall> m_call; //!< The call object associated with this machine.
  uint16_t m_callId;   //!< The call ID
  uint16_t m_floorPort; //!< The port number to use for floor control.
  uint16_t m_mediaPort; //!< The port number to use for media.
  uint8_t m_callType; //!< The call type.
  bool m_started;   //!< Whether the call is started
  Ptr<RandomVariableStream> m_invitePayloadSize; //!< Used for padding the notional SIP INVITE
  Ptr<RandomVariableStream> m_byePayloadSize; //!< Used for padding the notional SIP BYE
  Ptr<RandomVariableStream> m_responsePayloadSize; //!< Used for padding the notional SIP 200 OK
  Ptr<McpttOnNetworkCallMachineClientState> m_state; //!< The current state of the machine.
  McpttCallMsgFieldGrpId m_grpId; //!< The ID of the MCPTT group.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_H */

