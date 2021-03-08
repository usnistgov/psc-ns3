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

#ifndef MCPTT_CALL_TYPE_MACHINE_GRP_BASIC_H
#define MCPTT_CALL_TYPE_MACHINE_GRP_BASIC_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttCallMachineGrpBasic;

/**
 * \ingroup mcptt
 *
 * This class represents the basic MCPTT call control call type state machine
 * described in TS 24.379 v14.4.0.
 *
 * T0: waiting for call to establish
 *  - This state is the start state of the the state machine
 *
 * T1: in-progress emergency group call
 *  - This state exists for UE, when the UE is part of an in-progress emergency group call
 *
 * T2: in-progress basic group call
 *  - This state exists for UE, when the UE is part of an in-progress basic group call
 *
 * T3: in-progress imminent peril group call
 *  - This state exists for UE, when the UE is part of an in-progress imminent peril group call
 *
 * CFG11 (emergency end retransmission)
 *  - Limit: 5
 *  - Associated timer: TFG11
 *  - On expiry: retransmit "GROUP CALL EMERGENCY END" message.
 *  - Upon limit: Stop timer TFG11
 *
 * CFG12 (imminent peril end retransmission)
 *  - Limit: 5
 *  - Associated timer: TFG12
 *  - On expiry: retransmit "GROUP CALL IMMINENT PERIL END" message.
 *  - Upon limit: Stop timer TFG12
 *
 * TFG11 (emergency end retransmission)
 *  - Default value: 1 second
 *  - Cause of start: When client sends "GROUP CALL EMERGENCY END" message
 *  - Normal Stop: N/A
 *  - On expiry: Send a "GROUP CALL EMERGENCY END" message and increment counter by 1
 *
 * TFG12 (imminent peril end retransmission)
 *  - Default value: 1 second
 *  - Cause of start: When client sends "GROUP CALL IMMINENT PERIL END" message
 *  - Normal stop:
 *  - On expiry: Send a "GROUP CALL IMMINENT PERIL END" message and increment counter by 1
 *
 * TFG13 (implicit downgrade emergency)
 *  - Default value: Calculated
 *  - Cause of start: Upgrade of the call to emergency group call
 *  - Normal stop: downgrade of the call
 *  - On expiry: Downgrade the call
 *
 * TFG14 (implicit downgrade imminent peril)
 *  - Default value: Calculated
 *  - Cause of start: Upgrade of the call to imminent peril call
 *  - Normal stop: downgrade of the call
 *  - On expiry: Downgrade the call
 */
class McpttCallTypeMachineGrpBasic : public McpttCallTypeMachine
{
public:
  static const McpttEntityId T0; //!< The ID of state T0.
  static const McpttEntityId T1; //!< The ID of state T1.
  static const McpttEntityId T2; //!< The ID of state T2.
  static const McpttEntityId T3; //!< The ID of state T3.
  /**
   * Gets the type ID of the McpttCallTypeMachineGrpBasic class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallTypeMachineGrpBasic class.
   * \param owner The owner of this machine.
   */
  McpttCallTypeMachineGrpBasic (Ptr<McpttCallMachineGrpBasic> owner = 0);
  /**
   * \brief The destructor of the McpttCallTypeMachineGrpBasic class.
   */
  virtual ~McpttCallTypeMachineGrpBasic (void);
  /**
   * Calculate the delay to use for timer TFG13.
   * \returns The delay.
   */
  virtual Time CalcDelayForTfg13 (void) const;
  /**
   * Calculate the delay to use for timer TFG14.
   * \returns The delay.
   */
  virtual Time CalcDelayForTfg14 (void) const;
  /**
   * Indicates that a call has started.
   */
  virtual void CallStarted (void);
  /**
   * Changes the current state.
   * \param stateId The ID of the state to change to.
   */
  virtual void ChangeState (const McpttEntityId& stateId);
  /**
   * Downgrade the current call type.
   */
  virtual void Downgrade (void);
  /**
   * The timer Tfg1 expired.
   */
  virtual void ExpiryOfTfg1 (void);
  /**
   * Gets the type ID of this McpttCallTypeMachineGrpBasic instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Initiate a group call.
   * \param callType The type of group call to initiate.
   */
  virtual void InitiateCall (uint8_t callType = McpttCallMsgFieldCallType::BASIC_GROUP);
  /**
   * Indicates if the call type machine has been started.
   * \returns True, if the call type machine has been started, otherwise false.
   */
  virtual bool IsStarted (void) const;
  /**
   * Receive a "GROUP CALL ANNOUNCEMENT" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg);
  /**
   * Receive a "GROUP CALL EMERGENCY END" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg);
  /**
   * Receive a "GROUP CALL IMMINENT PERIL END" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg);
  /**
   * Release a group call.
   */
  virtual void ReleaseCall (void);
  /**
   * Reject a group call.
   */
  virtual void RejectCall (void);
  /**
   * Sends a call control message.
   * \param msg The message to send.
   */
  virtual void Send (const McpttCallMsg& msg);
  /**
   * Sets the delay for timer TFG11.
   * \param delayTfg11 The delay to use.
   */
  virtual void SetDelayTfg11 (const Time& delayTfg11);
  /**
   * Sets the delay for timer TFG12.
   * \param delayTfg12 The delay to use.
   */
  virtual void SetDelayTfg12 (const Time& delayTfg12);
  /**
   * Sets the limit for counter CFG11.
   * \param limitCfg11 The limit to use.
   */
  virtual void SetLimitCfg11 (uint32_t limitCfg11);
  /**
   * Sets the limit for counter CFG12.
   * \param limitCfg12 The limit to use.
   */
  virtual void SetLimitCfg12 (uint32_t limitCfg12);
  /**
   * Starts the call type machine.
   */
  virtual void Start (void);
  /**
   * Stops the call type machine.
   */
  virtual void Stop (void);
  /**
   * Upgrade the current call type.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeTo (const McpttCallMsgFieldCallType& callType);

protected:
  /**
   * Disposes of the McpttCallTypeMachineGrpBasic.
   */
  virtual void DoDispose (void);
  /**
   * The timer Tfg11 expired.
   */
  virtual void ExpiryOfTfg11 (void);
  /**
   * The timer Tfg12 expired.
   */
  virtual void ExpiryOfTfg12 (void);
  /**
   * The timer Tfg13 expired.
   */
  virtual void ExpiryOfTfg13 (void);
  /**
   * The timer Tfg14 expired.
   */
  virtual void ExpiryOfTfg14 (void);

private:
  McpttCallMsgFieldCallType m_callType; //!< The current call type.
  Ptr<McpttCounter> m_cfg11; //!< The counter CFG11.
  Ptr<McpttCounter> m_cfg12; //!< The counter CFG12.
  Time m_delayTfg13X; //!< The X value to use in the delay calculation for timer TFG13.
  Time m_delayTfg14X; //!< The X value to use in the delay calculation for timer TFG14.
  Callback<void> m_downgradeCb; //!< The downgrade callback.
  McpttCallMsgFieldLastChgTime m_lastChgTime; //!< The last call type change time.
  McpttCallMsgFieldUserId m_lastChgUserId; //!< The ID of the last user to change the call type.
  /* NOTE:
   * Using a raw pointer here because this type represents
   * an interface that does not inheret from any of the ns3
   * base classes. Therefore, this class should not allocate
   * or dispose of the memory referenced by this pointer, and
   * it is up to the user to provide a valid object. This
   * pointer's sole purpose is to provide access to the
   * object that the user has provided.
   */
  Ptr<McpttCallMachineGrpBasic> m_owner; //!< The owner of this call type machine
  uint8_t m_priority; //!< The ProSe per-packet priority.
  bool m_started; //!< The flag that indicates if the machine has been started.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  McpttEntityId m_stateId; //!< The current state ID.
  Ptr<McpttTimer> m_tfg11; //!< The timer TFG11.
  Ptr<McpttTimer> m_tfg12; //!< The timer TFG12.
  Ptr<McpttTimer> m_tfg13; //!< The timer TFG13.
  Ptr<McpttTimer> m_tfg14; //!< The timer TFG14.
  Callback<void, uint8_t> m_upgradeCb; //!< The downgrade callback.

public:
  /**
   * Gets the current call type.
   * \returns The call type.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the counter CFG11.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfg11 (void) const;
  /**
   * Gets the counter CFG12.
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetCfg12 (void) const;
  /**
   * Gets the last call type change time.
   * \returns The last change time.
   */
  virtual McpttCallMsgFieldLastChgTime GetLastChgTime (void) const;
  /**
   * Gets the ID of the last user to change the call type.
   * \returns The user ID.
   */
  virtual McpttCallMsgFieldUserId GetLastChgUserId (void) const;
  /**
   * Gets the owner of this call type machine.
   * \returns The owner.
   */
  virtual Ptr<McpttCallMachineGrpBasic> GetOwner (void) const;
  /**
   * Gets the ProSe per-packet priority.
   * \returns The ProSe per-packet priority.
   */
  virtual uint8_t GetPriority (void) const;
  /**
   * Gets the flag that indicates if the call type machine has been started.
   * \returns The flag.
   */
  virtual bool GetStarted (void) const;
  /**
   * Gets the current state ID.
   * \returns The current state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Gets the timer TFG11.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg11 (void) const;
  /**
   * Gets the timer TFG12.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg12 (void) const;
  /**
   * Gets the timer TFG13.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg13 (void) const;
  /**
   * Gets the timer TFG14.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetTfg14 (void) const;
  /**
   * Sets the downgrade callback.
   * \param downgradeCb The downgrade callback.
   */
  virtual void SetDowngradeCb (const Callback<void>  downgradeCb);
  /**
   * Sets the current call type.
   * \param callType The current call type.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType);
  /**
   * Sets the counter CFG11.
   * \param cfg11 The counter.
   */
  virtual void SetCfg11 (Ptr<McpttCounter>  cfg11);
  /**
   * Sets the counter CFG12.
   * \param cfg12 The counter.
   */
  virtual void SetCfg12 (Ptr<McpttCounter>  cfg12);
  /**
   * Sets the last call type change time.
   * \param lastChgTime The last change time.
   */
  virtual void SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime);
  /**
   * Sets the ID of the last user to change the call type.
   * \param lastChgUserId The ID of the last user to change the call type.
   */
  virtual void SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId);
  /**
   * Sets the owner of this call type machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttCallMachineGrpBasic> owner);
  /**
   * Sets the ProSe per-packet priority.
   * \param priority The ProSe per-packet priority.
   */
  virtual void SetPriority (uint8_t priority);
  /**
   * Sets the flag that indicates if the call type machine has been started.
   * \param started The flag.
   */
  virtual void SetStarted (const bool& started);
  /**
   * Gets the current state ID.
   * \param state The state ID.
   */
  virtual void SetStateId (const McpttEntityId& state);
  /**
   * Sets the timer TFG11.
   * \param tfg11 The timer.
   */
  virtual void SetTfg11 (Ptr<McpttTimer>  tfg11);
  /**
   * Sets the timer TFG12.
   * \param tfg12 The timer.
   */
  virtual void SetTfg12 (Ptr<McpttTimer>  tfg12);
  /**
   * Sets the timer TFG13.
   * \param tfg13 The timer.
   */
  virtual void SetTfg13 (Ptr<McpttTimer>  tfg13);
  /**
   * Sets the timer TFG14.
   * \param tfg14 The timer.
   */
  virtual void SetTfg14 (Ptr<McpttTimer>  tfg14);
  /**
   * Sets the upgrade callback.
   * \param upgradeCb The upgrade callback.
   */
  virtual void SetUpgradeCb (const Callback<void, uint8_t>  upgradeCb);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_TYPE_MACHINE_GRP_BASIC_H */

