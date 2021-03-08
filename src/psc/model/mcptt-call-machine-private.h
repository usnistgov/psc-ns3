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

#ifndef MCPTT_CALL_MACHINE_PRIVATE_H
#define MCPTT_CALL_MACHINE_PRIVATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/random-variable-stream.h>

#include "mcptt-call.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-machine-private-state.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine-private.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine-basic.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT private call control state machine as
 * described in TS.379 version 14.4.0.
 */
class McpttCallMachinePrivate : public McpttCallMachineGrp
{
public:
  /**
   * Gets the type ID of the McpttCallMachinePrivate class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the  class.
   * \param owner The owner of this machine
   */
  McpttCallMachinePrivate (Ptr<McpttCall> owner = 0);
  /**
   * \brief The destructor of the McpttCallMachinePrivate class.
   */
  virtual ~McpttCallMachinePrivate (void);
  /**
   * Indicates that the call should be accepted.
   */
  virtual void AcceptCall (void);
  /**
   * Starts an emergency alert.
   */
  virtual void BeginEmergAlert (void);
  /**
   * Stops an emergency alert.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Changes the state of the call machine.
   * \param state The state to change to.
   */
  virtual void ChangeState (Ptr<McpttCallMachinePrivateState>  state);
  /**
   * Downgrades the call from imminent peril.
   */
  virtual void DowngradeCallType (void);
  /**
   * Gets the user ID of the user that started the call.
   * \returns The MCPTT user ID.
   */
  virtual uint32_t GetCallerUserId (void) const;
  /**
   * Gets the current call type.
   * \returns The call type.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the group ID.
   * \returns The group ID.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the type ID of this McpttCallMachinePrivate instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the ID of the current state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Gets the call type machine.
   * \returns The call type machine.
   */
  virtual Ptr<McpttCallTypeMachine> GetCallTypeMachine (void) const;
  /**
   * Indicates that a private call should be initiated.
   */
  virtual void InitiateCall (void);
  /**
   * Indicates if automatic commencement mode should be indicated.
   * \returns True, if automatic commencement mode should be indicated.
   */
  virtual bool IsAutoCommencement (void) const;
  /**
   * Indicates if confirm mode is indicated.
   * \returns True, if confirm mode is indicated; otherwise, false.
   */
  virtual bool IsCallOngoing (void) const;
  /**
   * Indicates if the call is a failure.
   * \returns True, if the setup should fail.
   */
  virtual bool IsFailure (void) const;
  /**
   * Indicates if the call control machine has been started.
   * \returns True, if the call control machine has been started, or false otherwise.
   */
  virtual bool IsStarted (void) const;
  /**
   * Notifes the call machine that a new call has been started.
   * \param callId The ID of the new call.
   */
  virtual void NotifyNewCall (uint16_t callId);
  /**
   * Receives a call control message.
   * \param msg The call control message.
   */
  virtual void Receive (const McpttCallMsg& msg);
  /**
   * Receives a media message.
   * \param msg The message to receive.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL" message.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg);
  /**
   * Receive a "GROUP EMERGENCY ALERT" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg);
  /**
   * Receive a "GROUP EMERGENCY ALERT CANCEL" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg);
  /**
   * Reception of a media message.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (const McpttMediaMsg& msg);
  /**
   * Reception of a "PRIVATE CALL REJECT" message.
   * \param msg The received message.
   */
  virtual void ReceiveReject (const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (const McpttCallMsgPrivateRelease& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveReleaseAck (const McpttCallMsgPrivateReleaseAck& msg);
  /**
   * Reception of a "PRIVATE CALL RINGING" message.
   * \param msg The received message.
   */
  virtual void ReceiveRinging (const McpttCallMsgPrivateRinging& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Indicates that the call should be rejected.
   */
  virtual void RejectCall (void);
  /**
   * Indicates that the call should be released.
   */
  virtual void ReleaseCall (void);
  /**
   * \brief Report an event to the McpttPttApp.
   * \param reason the reason to report.
   */
  virtual void ReportEvent (const char* reason) const;
  /**
   * Sends a call control message.
   * \param msg The message to send.
   */
  virtual void Send (const McpttCallMsg& msg);
  /**
   * Sets the delay for timer TFP1.
   * \param delayTfp1 The delay to use.
   */
  virtual void SetDelayTfp1 (const Time& delayTfp1);
  /**
   * Sets the delay for timer TFP2.
   * \param delayTfp2 The delay to use.
   */
  virtual void SetDelayTfp2 (const Time& delayTfp2);
  /**
   * Sets the delay for timer TFP3.
   * \param delayTfp3 The delay to use.
   */
  virtual void SetDelayTfp3 (const Time& delayTfp3);
  /**
   * Sets the delay for timer TFP4.
   * \param delayTfp4 The delay to use.
   */
  virtual void SetDelayTfp4 (const Time& delayTfp4);
  /**
   * Sets the delay for timer TFP5.
   * \param delayTfp5 The delay to use.
   */
  virtual void SetDelayTfp5 (const Time& delayTfp5);
  /**
   * Sets the delay for timer TFP6.
   * \param delayTfp6 The delay to use.
   */
  virtual void SetDelayTfp6 (const Time& delayTfp6);
  /**
   * Sets the delay for timer TFP7.
   * \param delayTfp7 The delay to use.
   */
  virtual void SetDelayTfp7 (const Time& delayTfp7);
  /**
   * Sets the delay for timer TFP8.
   * \param delayTfp8 The delay to use.
   */
  virtual void SetDelayTfp8 (const Time& delayTfp8);
  /**
   * Sets the group ID.
   * \param grpId The group ID.
   */
  virtual void SetGrpId (uint32_t grpId);
  /**
   * Sets the limit for counter CFP1.
   * \param limitCfp1 The limit to use.
   */
  virtual void SetLimitCfp1 (uint32_t limitCfp1);
  /**
   * Sets the limit for counter CFP3.
   * \param limitCfp3 The limit to use.
   */
  virtual void SetLimitCfp3 (uint32_t limitCfp3);
  /**
   * Sets the limit for counter CFP4.
   * \param limitCfp4 The limit to use.
   */
  virtual void SetLimitCfp4 (uint32_t limitCfp4);
  /**
   * Sets the limit for counter CFP6.
   * \param limitCfp6 The limit to use.
   */
  virtual void SetLimitCfp6 (uint32_t limitCfp6);
  /**
   * Starts the call machine.
   */
  virtual void Start (void);
  /**
   * Stops the call machine.
   */
  virtual void Stop (void);
  /**
   * Upgrades the call type.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeCallType (uint8_t callType);

protected:
  /**
   * Disposes of the McpttCallMachinePrivate.
   */
  virtual void DoDispose (void);
  /**
   * Indicates that the timer TFP1 has expired.
   */
  virtual void ExpiryOfTfp1 (void);
  /**
   * Indicates that the timer TFP2 has expired.
   */
  virtual void ExpiryOfTfp2 (void);
  /**
   * Indicates that the timer TFP3 has expired.
   */
  virtual void ExpiryOfTfp3 (void);
  /**
   * Indicates that the timer TFP4 has expired.
   */
  virtual void ExpiryOfTfp4 (void);
  /**
   * Indicates that the timer TFP5 has expired.
   */
  virtual void ExpiryOfTfp5 (void);
  /**
   * Indicates that the timer TFP6 has expired.
   */
  virtual void ExpiryOfTfp6 (void);
  /**
   * Indicates that the timer TFP7 has expired.
   */
  virtual void ExpiryOfTfp7 (void);
  /**
   * Indicates that the timer TFP8 has expired.
   */
  virtual void ExpiryOfTfp8 (void);
  /**
   * Gets the flag that indicates if the call machine has been started.
   * \returns The flag.
   */
  virtual bool GetStarted (void) const;
  /**
   * Sets the flag that indicates if the call machine has been started.
   * \param started The flag.
   */
  virtual void SetStarted (const bool& started);

private:
  bool m_autoCommencement; //!< The flag that indicates if auto commencement mode should be indicated.
  McpttCallMsgFieldCallId m_callId; //!< The ID of the MCPTT call.
  McpttCallMsgFieldUserId m_calleeId; //!< The MCPTT user ID of the callee.
  McpttCallMsgFieldUserId m_callerId; //!< The MCPTT user ID of the caller.
  Ptr<McpttCounter> m_cfp1; //!< The counter CFP1
  Ptr<McpttCounter> m_cfp3; //!< The counter CFP3
  Ptr<McpttCounter> m_cfp4; //!< The counter CFP4
  Ptr<McpttCounter> m_cfp6; //!< The counter CFP6
  McpttCallMsgFieldCommMode m_commMode; //!< The current commencement mode.
  Ptr<McpttEmergAlertMachineBasic> m_emergMachine; //!< The emergency alert state machine.
  bool m_failure; //!< Indicates if the call should fail.
  McpttCallMsgFieldGrpId m_grpId; //!< TODO: Not in standard - private call use of emergency alert machine
  Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
  Ptr<McpttCall> m_call; //!< The call under which this MCPTT call control machine is under.
  Ptr<UniformRandomVariable> m_randomCallIdGenerator; //!< The RNG used for call ID generation.
  McpttCallMsgFieldSdp m_sdp; //!< SDP information.
  McpttCallMsgFieldSdp m_sdpEmerg; //!< Emergency SDP information.
  bool m_started; //!< The flag that indicates if the machine has been started.
  Ptr<McpttCallMachinePrivateState> m_state; //!< The current state of the machine.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  uint32_t m_targetId; //!< The MCPTT user ID of the user to call.
  Ptr<McpttTimer> m_tfp1; //!< The timer TFP1.
  Ptr<McpttTimer> m_tfp2; //!< The timer TFP2.
  Ptr<McpttTimer> m_tfp3; //!< The timer TFP3.
  Ptr<McpttTimer> m_tfp4; //!< The timer TFP4.
  Ptr<McpttTimer> m_tfp5; //!< The timer TFP5.
  Ptr<McpttTimer> m_tfp6; //!< The timer TFP6.
  Ptr<McpttTimer> m_tfp7; //!< The timer TFP7.
  Ptr<McpttTimer> m_tfp8; //!< The timer TFP8.
  Ptr<McpttCallTypeMachinePrivate> m_typeMachine; //!< The call type state machine.

public:
  /**
   * Gets the ID of the MCPTT call.
   * \returns The ID.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the MCPTT user ID of the callee.
   * \returns The user ID.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the MCPTT user ID of the caller.
   * \returns The user ID.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Gets the counter CFP1.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfp1 (void) const;
  /**
   * Gets the counter CFP3.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfp3 (void) const;
  /**
   * Gets the counter CFP4.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfp4 (void) const;
  /**
   * Gets the counter CFP6.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfp6 (void) const;
  /**
   * Gets the stored commencement mode.
   * \returns The stored commencement mode.
   */
  virtual McpttCallMsgFieldCommMode GetCommMode (void) const;
  /**
   * Gets the emergency alert state machine.
   * \returns The state machine.
   */
  virtual Ptr<McpttEmergAlertMachineBasic> GetEmergMachine (void) const;
  /**
   * Gets the McpttCall associated with the call machine.
   * \returns The call.
   */
  virtual Ptr<McpttCall> GetCall (void) const;
  /**
   * Generate and return a randomly generated call ID
   * \returns The random call ID generated.
   */
  virtual uint16_t GenerateRandomCallId (void) const;
  /**
   * Gets the SDP information.
   * \returns The SDP information.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Gets the emergency SDP information.
   * \returns The SDP information.
   */
  virtual McpttCallMsgFieldSdp GetSdpEmerg (void) const;
  /**
   * Gets the current state of the call machine.
   * \returns The current state.
   */
  virtual Ptr<McpttCallMachinePrivateState> GetState (void) const;
  /**
   * Gets the timer TFP1.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp1 (void) const;
  /**
   * Gets the timer TFP2.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp2 (void) const;
  /**
   * Gets the timer TFP3.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp3 (void) const;
  /**
   * Gets the timer TFP4.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp4 (void) const;
  /**
   * Gets the timer TFP5.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp5 (void) const;
  /**
   * Gets the timer TFP6.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp6 (void) const;
  /**
   * Gets the timer TFP7.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp7 (void) const;
  /**
   * Gets the timer TFP8.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfp8 (void) const;
  /**
   * Gets the call type machine.
   * \returns The call type machine.
   */
  virtual Ptr<McpttCallTypeMachinePrivate> GetTypeMachine (void) const;
  /**
   * Sets the ID of the MCPTT call.
   * \param callId The ID of the MCPTT call.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the MCPTT user ID of the callee.
   * \param calleeId The MCPTT user ID.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeId);
  /**
   * Sets the MCPTT user ID of the caller.
   * \param callerId The MCPTT user ID.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerId);
  /**
   * Sets the counter CFP1.
   * \param cfp1 The counter.
   */
  virtual void SetCfp1 (Ptr<McpttCounter>  cfp1);
  /**
   * Sets the counter CFP3.
   * \param cfp3 The counter.
   */
  virtual void SetCfp3 (Ptr<McpttCounter>  cfp3);
  /**
   * Sets the counter CFP4.
   * \param cfp4 The counter.
   */
  virtual void SetCfp4 (Ptr<McpttCounter>  cfp4);
  /**
   * Sets the counter CFP6.
   * \param cfp6 The counter.
   */
  virtual void SetCfp6 (Ptr<McpttCounter>  cfp6);
  /**
   * Sets the stored commencement mode.
   * \param commMode The commencement mode.
   */
  virtual void SetCommMode (const McpttCallMsgFieldCommMode& commMode);
  /**
   * Sets the emergency alert state machine.
   * \param emergMachine The emergency alert state machine.
   */
  virtual void SetEmergMachine (Ptr<McpttEmergAlertMachineBasic>  emergMachine);
  /**
   * Sets the callback used to notify a new call.
   * \param newCallCb The callback
   */
  virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb);
  /**
   * Sets the McpttCall associated with the call machine.
   * \param call The call.
   */
  virtual void SetCall (Ptr<McpttCall> call);
  /**
   * Sets the SDP inforamtion.
   * \param sdp The SDP information.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdp);
  /**
   * Sets the emergency SDP information.
   * \param sdpEmerg The emergency SDP information.
   */
  virtual void SetSdpEmerg (const McpttCallMsgFieldSdp& sdpEmerg);
  /**
   * Sets the current state of the call machine.
   * \param state The current state.
   */
  virtual void SetState (Ptr<McpttCallMachinePrivateState>  state);
  /**
   * Sets the state change callback.
   * \param stateChangeCb The callback.
   */
  virtual void SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb);
  /**
   * Sets the timer TFP1.
   * \param tfp1 The timer.
   */
  virtual void SetTfp1 (Ptr<McpttTimer>  tfp1);
  /**
   * Sets the timer TFP2.
   * \param tfp2 The timer.
   */
  virtual void SetTfp2 (Ptr<McpttTimer>  tfp2);
  /**
   * Sets the timer TFP3.
   * \param tfp3 The timer.
   */
  virtual void SetTfp3 (Ptr<McpttTimer>  tfp3);
  /**
   * Sets the timer TFP4.
   * \param tfp4 The timer.
   */
  virtual void SetTfp4 (Ptr<McpttTimer>  tfp4);
  /**
   * Sets the timer TFP5.
   * \param tfp5 The timer.
   */
  virtual void SetTfp5 (Ptr<McpttTimer>  tfp5);
  /**
   * Sets the timer TFP6.
   * \param tfp6 The timer.
   */
  virtual void SetTfp6 (Ptr<McpttTimer>  tfp6);
  /**
   * Sets the timer TFP7.
   * \param tfp7 The timer.
   */
  virtual void SetTfp7 (Ptr<McpttTimer>  tfp7);
  /**
   * Sets the timer TFP8.
   * \param tfp8 The timer.
   */
  virtual void SetTfp8 (Ptr<McpttTimer>  tfp8);
  /**
   * Sets the call type machine.
   * \param typeMachine The call type machine.
   */
  virtual void SetTypeMachine (Ptr<McpttCallTypeMachinePrivate>  typeMachine);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_PRIVATE_H */

