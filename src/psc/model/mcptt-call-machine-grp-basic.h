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

#ifndef MCPTT_CALL_MACHINE_GRP_BASIC_H
#define MCPTT_CALL_MACHINE_GRP_BASIC_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/random-variable-stream.h>

#include "mcptt-call.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-machine-grp-basic-state.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine-grp-basic.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine-basic.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT basic group call control state machine as
 * described in TS.379 version 14.4.0.
 *
 * Refresh Interval
 *  - Fixed: 10000 milliseconds
 *
 * TFG1 (wait for call announcement)
 *  - Default: 150 milliseconds
 *  - Depends on sidelink configuration
 *  - Start: When client sends a "CALL PROBE" message
 *  - Stop: Reception of a "CALL ANNOUNCEMENT" message
 *  - Expiration: Send a "CALL ANOUNCEMENT" message
 *
 * TFG2 (call announcement)
 *  - Default: calculated
 *  - Start: Commencement of group call
 *  - Restart: Reception of "CALL PROBE" or "CALL ANNOUNCEMENT" message, or
 *             transmission of "CALL ANNOUNCEMENT" message
 *  - Stop: Termintation of call
 *  - Expiration: Send "CALL ANNOUNCEMENT" message
 *
 * TFG3 (call probe retransmission)
 *  - Default: 40 milliseconds
 *  - Start: When the client sends a "CALL PROBE" message.
 *  - Stop: Reception of a "CALL ANNOUNCEMENT" message, or user releases call,
 *          or TFG1 expires
 *
 * TFG4 (waiting for user)
 *  - Default: 30 seconds
 *  - Maximum: 60 seconds
 *  - Start: Reception of "CALL ANNOUNCEMENT" message when not on call
 *  - Stop: Reception of user action (accept or reject)
 *  - Expiration: Stop incoming call notification
 *
 * TFG5 (not present incomming call announcements)
 *  - Default: 30 seconds
 *  - Start: Expiration of TFG4, or user rejects call, or user releases call
 *  - Stop: N/A
 *  - Expiration: Reset group call state machine
 *
 * TFG6 (max duration)
 *  - Default: calculated
 *  - Start: Commencement of group call
 *  - Stop: Termintation of group call
 *  - Expiration: Release the group call
 */
class McpttCallMachineGrpBasic : public McpttCallMachineGrp
{
public:
  /**
   * Gets the type ID of the McpttCallMachineGrpBasic class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the  class.
   * \param owner The owner of this machine
   */
  McpttCallMachineGrpBasic (Ptr<McpttCall> owner = 0);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasic class.
   */
  virtual ~McpttCallMachineGrpBasic (void);
  /**
   * Indicates that the call should be accepted.
   */
  virtual void AcceptCall (void);
  /**
   * Starts an emergency alert.
   */
  virtual void BeginEmergAlert (void);
  /**
   * Calculates the delay to use for TFG2 after receiving a "CALL PROBE" message.
   * \returns The delay.
   */
  virtual Time CalcDelayForAfterCallProbe (void);
  /**
   * Calculates the delay to use for TFG2 for periodic call announcements.
   * \returns The delay.
   */
  virtual Time CalcDelayForPeriodicAnnoun (void);
  /**
   * Calculates the delay to use for TFG6.
   * \returns The delay.
   */
  virtual Time CalcDelayForMaxDuration (void) const;
  /**
   * Stops an emergency alert.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Changes the state of the call machine.
   * \param state The state to change to.
   */
  virtual void ChangeState (Ptr<McpttCallMachineGrpBasicState>  state);
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
   * Gets the type ID of this McpttCallMachineGrpBasic instance.
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
   * Indicates that a group call should be initiated.
   */
  virtual void InitiateCall (void);
  /**
   * Indicates if the call is active.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (void) const;
  /**
   * Indicates if confirm mode is indicated.
   * \returns True, if confirm mode is indicated; otherwise, false.
   */
  virtual bool IsConfirmMode (void) const;
  /**
   * Indicates if the call control machine has been started.
   * \returns True, if the call control machine has been started, or false otherwise.
   */
  virtual bool IsStarted (void) const;
  /**
   * Indicates if user acknowledgements are required.
   * \returns True, if user acknowledgements are required; otherwise, false.
   */
  virtual bool IsUserAckReq (void) const;
  /**
   * Notifies the call machine that a call merge is taking place.
   * \param callId The ID of the call being merged to.
   * \todo Not in standard - just used get an indication.
   */
  virtual void NotifyMerge (uint16_t callId);
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
   * Reception of a "GROUP CALL ACCEPT" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAccept (const McpttCallMsgGrpAccept& msg);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg);
  /**
   * Reception of a "GROUP CALL EMERGENCY ALERT" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg);
  /**
   * Reception of a "GROUP CALL EMERGENCY ALERT ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallEmergAlertAck (const McpttCallMsgGrpEmergAlertAck& msg);
  /**
   * Reception of a "GROUP CALL EMERGENCY ALERT CANCEL" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg);
  /**
   * Reception of a "GROUP CALL EMERGENCY ALERT CANCEL ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallEmergAlertCancelAck (const McpttCallMsgGrpEmergAlertCancelAck& msg);
  /**
   * Reception of a "GROUP CALL EMERGENCY END" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg);
  /**
   * Reception of a "GROUP CALL IMMINENT PERIL END" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg);
  /**
   * Reception of a "GROUP CALL PROBE" message.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallProbe (const McpttCallMsgGrpProbe& msg);
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
   * Sets the delay for timer TFG1.
   * \param delayTfg1 The delay to use.
   */
  virtual void SetDelayTfg1 (const Time& delayTfg1);
  /**
   * Sets the delay for timer TFG3.
   * \param delayTfg3 The delay to use.
   */
  virtual void SetDelayTfg3 (const Time& delayTfg3);
  /**
   * Sets the delay for timer TFG4.
   * \param delayTfg4 The delay to use.
   */
  virtual void SetDelayTfg4 (const Time& delayTfg4);
  /**
   * Sets the delay for timer TFG5.
   * \param delayTfg5 The delay to use.
   */
  virtual void SetDelayTfg5 (const Time& delayTfg5);
  /**
   * Sets the group ID.
   * \param grpId The group ID.
   */
  virtual void SetGrpId (uint32_t grpId);
  /**
   * Sets the limit for counter CFG3.
   * \param limitCfg3 The limit to use.
   */
  virtual void SetLimitCfg3 (uint32_t limitCfg3);
  /**
   * Sets the minimum time interval of periodic announcements.
   * \param refIntVal The minimum interval.
   */
  virtual void SetRefIntVal (uint16_t refIntVal);
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
   * \param callType The call type value.
   */
  virtual void UpgradeCallType (uint8_t callType);

protected:
  /**
   * Disposes of the McpttCallMachineGrpBasic.
   */
  virtual void DoDispose (void);
  /**
   * Indicates that the timer TFG1 has expired.
   */
  virtual void ExpiryOfTfg1 (void);
  /**
   * Indicates that the timer TFG2 has expired.
   */
  virtual void ExpiryOfTfg2 (void);
  /**
   * Indicates that the timer TFG3 has expired.
   */
  virtual void ExpiryOfTfg3 (void);
  /**
   * Indicates that the timer TFG4 has expired.
   */
  virtual void ExpiryOfTfg4 (void);
  /**
   * Indicates that the timer TFG5 has expired.
   */
  virtual void ExpiryOfTfg5 (void);
  /**
   * Indicates that the timer TFG6 has expired.
   */
  virtual void ExpiryOfTfg6 (void);
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
  McpttCallMsgFieldCallId m_callId; //!< The ID of the MCPTT call.
  //TODO: Not in standard - CFG3 added for call control setup delay testing
  Ptr<McpttCounter> m_cfg3; //!< The counter CFG3.
  bool m_confirmMode; //!< Indicates if confirm mode is used.
  Time m_delayTfg6X; //!< The X value to use when calculating the delay for TFG6.
  Ptr<McpttEmergAlertMachineBasic> m_emergMachine; //!< The emergency alert state machine.
  McpttCallMsgFieldGrpId m_grpId; //!< The ID of the MCPTT group.
  Callback<void, uint16_t> m_mergeCb; //!< The merge callback.
  Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
  McpttCallMsgFieldUserId m_origId; //!< The ID of MCPTT user that the call originated from.
  Ptr<McpttCall> m_call {nullptr}; //!< The call under which this MCPTT call control machine is under.
  bool m_probeRespVal; //!< The probe response value flag.
  McpttCallMsgFieldRefreshInterval m_refInt; //!< The minimum time interval of periodic announcements.
  Ptr<UniformRandomVariable> m_randomCallIdGenerator; //!< The RNG used for call ID generation.
  Ptr<RandomVariableStream> m_rndDelays; //!< The random number generator used for timer delay calculations.
  McpttCallMsgFieldSdp m_sdp; //!< SDP information.
  bool m_started; //!< The flag that indicates if the machine has been started.
  McpttCallMsgFieldStartTime m_startTime; //!< The time at which the call started.
  Ptr<McpttCallMachineGrpBasicState> m_state; //!< The current state of the machine.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  Ptr<McpttTimer> m_tfg1; //!< The timer TFG1.
  Ptr<McpttTimer> m_tfg2; //!< The timer TFG2.
  Ptr<McpttTimer> m_tfg3; //!< The timer TFG3.
  Ptr<McpttTimer> m_tfg4; //!< The timer TFG4.
  Ptr<McpttTimer> m_tfg5; //!< The timer TFG5.
  Ptr<McpttTimer> m_tfg6; //!< The timer TFG6.
  Ptr<McpttCallTypeMachineGrpBasic> m_typeMachine; //!< The call type state machine.
  bool m_userAckReq; //!< Indicates if user acknowledgments are required.

public:
  /**
   * Gets the ID of the MCPTT call.
   * \returns The ID.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the counter CFG3.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfg3 (void) const;
  /**
   * Gets the emergency alert state machine.
   * \returns The state machine.
   */
  virtual Ptr<McpttEmergAlertMachineBasic> GetEmergMachine (void) const;
  /**
   * Gets the ID of the MCPTT group.
   * \returns The ID of the MCPTT group.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the ID of the MCPTT user that the call originated from.
   * \returns The ID of the MCPTT user.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the McpttCall associated with the call machine.
   * \returns The call.
   */
  virtual Ptr<McpttCall> GetCall (void) const;
  /**
   * Gets the probe response value flag.
   * \returns The flag.
   */
  virtual bool GetProbeRespVal (void) const;
  /**
   * Gets the minimum time interval of periodic announcements.
   * \returns The minimum time interval.
   */
  virtual McpttCallMsgFieldRefreshInterval GetRefInt (void) const;
  /**
   * Generate and return a randomly generated call ID
   * \returns The random call ID generated.
   */
  virtual uint16_t GenerateRandomCallId (void) const;
  /**
   * Gets the random number generator used for timer delay calculations.
   * \returns The random number generator.
   */
  virtual Ptr<RandomVariableStream> GetRndDelays (void) const;
  /**
   * Gets the SDP information.
   * \returns The SDP information.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Gets the current state of the call machine.
   * \returns The current state.
   */
  virtual Ptr<McpttCallMachineGrpBasicState> GetState (void) const;
  /**
   * Gets the time at which the call started.
   * \returns The time.
   */
  virtual McpttCallMsgFieldStartTime GetStartTime (void) const;
  /**
   * Gets the timer TFG1.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg1 (void) const;
  /**
   * Gets the timer TFG2.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg2 (void) const;
  /**
   * Gets the timer TFG3.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg3 (void) const;
  /**
   * Gets the timer TFG4.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg4 (void) const;
  /**
   * Gets the timer TFG5.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg5 (void) const;
  /**
   * Gets the timer TFG6.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg6 (void) const;
  /**
   * Gets the call type machine.
   * \returns The call type machine.
   */
  virtual Ptr<McpttCallTypeMachineGrpBasic> GetTypeMachine (void) const;
  /**
   * Sets the ID of the MCPTT call.
   * \param callId The ID of the MCPTT call.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the timer CFG3.
   * \param cfg3 The timer.
   */
  virtual void SetCfg3 (Ptr<McpttCounter>  cfg3);
  /**
   * Sets the emergency alert state machine.
   * \param emergMachine The emergency alert state machine.
   */
  virtual void SetEmergMachine (Ptr<McpttEmergAlertMachineBasic>  emergMachine);
  /**
   * Sets the ID of the MCPTT group.
   * \param grpId The ID of the MCPTT group.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the callback used to notify a new call.
   * \param newCallCb The callback
   */
  virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb);
  /**
  * Sets the merge callback.
  * \param mergeCb The merge callback.
  */
  virtual void SetMergeCb (const Callback<void, uint16_t>  mergeCb);
  /**
   * Sets the ID of the MCPTT user that the call orignated from.
   * \param origId The ID of the MCPTT user.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origId);
  /**
   * Sets the McpttCall associated with the call machine.
   * \param call The call.
   */
  virtual void SetCall (Ptr<McpttCall> call);
  /**
   * Sets the probe response value flag.
   * \param probeRespVal The flag.
   */
  virtual void SetProbeRespVal (const bool& probeRespVal);
  /**
   * Sets the minimum time interval of periodic announcements.
   * \param refInt The minimum interval.
   */
  virtual void SetRefInt (const McpttCallMsgFieldRefreshInterval& refInt);
  /**
   * Sets the random number generator used for timer delay calculations.
   * \param rndDelays The random number generator.
   */
  virtual void SetRndDelays (Ptr<RandomVariableStream>  rndDelays);
  /**
   * Sets the SDP inforamtion.
   * \param sdp The SDP information.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdp);
  /**
   * Sets the time at which the call started.
   * \param startTime The time.
   */
  virtual void SetStartTime (const McpttCallMsgFieldStartTime& startTime);
  /**
   * Sets the current state of the call machine.
   * \param state The current state.
   */
  virtual void SetState (Ptr<McpttCallMachineGrpBasicState>  state);
  /**
   * Sets the state change callback.
   * \param stateChangeCb The callback.
   */
  virtual void SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb);
  /**
   * Sets the timer TFG1.
   * \param tfg1 The timer.
   */
  virtual void SetTfg1 (Ptr<McpttTimer>  tfg1);
  /**
   * Sets the timer TFG2.
   * \param tfg2 The timer.
   */
  virtual void SetTfg2 (Ptr<McpttTimer>  tfg2);
  /**
   * Sets the timer TFG3.
   * \param tfg3 The timer.
   */
  virtual void SetTfg3 (Ptr<McpttTimer>  tfg3);
  /**
   * Sets the timer TFG4.
   * \param tfg4 The timer.
   */
  virtual void SetTfg4 (Ptr<McpttTimer>  tfg4);
  /**
   * Sets the timer TFG5.
   * \param tfg5 The timer.
   */
  virtual void SetTfg5 (Ptr<McpttTimer>  tfg5);
  /**
   * Sets the timer TFG6.
   * \param tfg6 The timer.
   */
  virtual void SetTfg6 (Ptr<McpttTimer>  tfg6);
  /**
   * Sets the call type machine.
   * \param typeMachine The call type machine.
   */
  virtual void SetTypeMachine (Ptr<McpttCallTypeMachineGrpBasic>  typeMachine);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_GRP_BASIC_H */

