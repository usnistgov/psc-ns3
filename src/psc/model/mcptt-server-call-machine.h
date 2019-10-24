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

#ifndef MCPTT_SERVER_CALL_MACHINE_H
#define MCPTT_SERVER_CALL_MACHINE_H

#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-media-msg.h"

namespace ns3 {

class McpttServerCall;

/**
 * \ingroup mcptt
 * 
 * This interface is used to construct a common API for the MCPTT server
 * entities performing server call control described in TS 24.379.
 */
class McpttServerCallMachine : public Object
{
public:
 /**
  * Gets the type ID of the McpttServerCallMachine class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttServerCallMachine class.
  */
 McpttServerCallMachine (void);
 /**
  * The destructor of the McpttServerCallMachine class.
  */
 virtual ~McpttServerCallMachine (void);
 /**
  * Accepts the call.
  */
 virtual void AcceptCall (void) = 0;
 /**
  * Begins an emergency alert.
  */
 virtual void BeginEmergAlert (void) = 0;
 /**
  * Cancels an emergency alert.
  */
 virtual void CancelEmergAlert (void) = 0;
 /**
  * Downgrades the call type.
  */
 virtual void DowngradeCallType (void) = 0;
 /**
  * Gets the ID of the user that started the call.
  * \returns The MCPTT user ID.
  */
 virtual uint32_t GetCallerUserId (void) const = 0;
 /**
  * Gets the ID of the call.
  * \returns The call ID.
  */
 virtual McpttCallMsgFieldCallId GetCallId (void) const = 0;
 /**
  * Gets the call type.
  * \returns The call type.
  */
 virtual McpttCallMsgFieldCallType GetCallType (void) const = 0;
 /**
  * Gets the type ID of this McpttServerCallMachine instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Gets the owner of the call machine.
  * \returns The owner.
  */
 virtual Ptr<McpttServerCall> GetOwner (void) const = 0;
 /**
  * Gets the ID of the current state.
  * \returns The state ID.
  */
 virtual McpttEntityId GetStateId (void) const = 0;
 /**
  * Initiates a call.
  */
 virtual void InitiateCall (void) = 0;
 /**
  * Indicates if the call is active.
  * \returns True, if the call is active.
  */
 virtual bool IsCallOngoing (void) const = 0;
 /**
  * Indicates if this is a group call.
  * \param grpId The particular group ID to check for when greater than 0.
  * \returns True, if this call is a group call; otherwise, false.
  */
 virtual bool IsGrpCall (uint32_t grpId = 0) const = 0;
 /**
  * Indicates if this is a private call.
  * \param userId The particular user ID to check for when greater than 0.
  * \returns True, if this call is a private call; otherwise, false.
  */
 virtual bool IsPrivateCall (uint32_t userId = 0) const = 0;
 /**
  * Receives a call control message.
  * \param msg The message to receive.
  */
 virtual void Receive (const McpttCallMsg& msg) = 0;
 /**
  * Receives a media message.
  * \param msg The message to receive.
  */
 virtual void Receive (const McpttMediaMsg& msg) = 0;
 /**
  * Releases the call.
  */
 virtual void ReleaseCall (void) = 0;
 /**
  * Rejects the call.
  */
 virtual void RejectCall (void) = 0;
 /**
  * Sends a call control message.
  * \param msg The message to send.
  */
 virtual void Send (const McpttCallMsg& msg) = 0;
 /**
  * Sets the callback used to indicate that a new call exists.
  * \param newCallCb The callback.
  */
 virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb) = 0;
 /**
  * Sets the owner of the call machine.
  * \param owner The owner.
  */
 virtual void SetOwner (Ptr<McpttServerCall> owner) = 0;
 /**
  * Starts the state machine.
  */
 virtual void Start (void) = 0;
 /**
  * Stops the state machine.
  */
 virtual void Stop (void) = 0;
 /**
  * Upgrades the call type.
  * \param callType The call type value.
  */
 virtual void UpgradeCallType (uint8_t callType) = 0;
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a group call call machine.
 */
class McpttServerCallMachineGrp : public McpttServerCallMachine
{
public:
 /**
  * Gets the type Id of the McpttServerCallMachineGrp class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttServerCallMachineGrp class.
  */
 McpttServerCallMachineGrp (void);
 /**
  * The destructor of the McpttServerCallMachineGrp class.
  */
 virtual ~McpttServerCallMachineGrp (void);
 /**
  * Gets the group ID.
  * \returns The group ID.
  */
 virtual McpttCallMsgFieldGrpId GetGrpId (void) const = 0;
 /**
  * Gets the type ID of this instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Indicates if this is a group call.
  * \param grpId The particular group ID to check for when greater than 0.
  * \returns True, if this a group call; otherwise, false.
  */
 virtual bool IsGrpCall (uint32_t grpId = 0) const;
 /**
  * Indicates if this is a private call.
  * \param userId The particular user ID to check for when greater than 0.
  * \returns True, if this is a private call; otherwise, false.
  */
 virtual bool IsPrivateCall (uint32_t userId = 0) const;
 /**
  * Sets the group ID.
  * \param grpId The group ID.
  */
 virtual void SetGrpId (uint32_t grpId) = 0;
};
/**
 * \ingroup mcptt
 * 
 * A class used to represent a null MCPTT call machine.
 */
class McpttServerCallMachineNull : public McpttServerCallMachine
{
public:
 /**
  * Gets The null state ID.
  * \returns The state ID.
  */
 static McpttEntityId GetNullStateId (void);
 /**
  * Gets the type ID of the McpttServerCallMachineNull class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttServerCallMachineNull class.
  */
 McpttServerCallMachineNull (void);
 /**
  * The destructor of the McpttServerCallMachineNull class.
  */
 virtual ~McpttServerCallMachineNull (void);
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
  * Gets the type ID of this McpttServerCallMachineNull instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Gets the owner of the call machine.
  * \returns The owner.
  */
 virtual Ptr<McpttServerCall> GetOwner (void) const;
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
  * Receives a call control message.
  * \param msg The message to receive.
  */
 virtual void Receive (const McpttCallMsg& msg);
 /**
  * Receives a media message.
  * \param msg The message to receive.
  */
 virtual void Receive (const McpttMediaMsg& msg);
 /**
  * Releases the call.
  */
 virtual void ReleaseCall (void);
 /**
  * Rejects the call.
  */
 virtual void RejectCall (void);
 /**
  * Sends a call control message.
  * \param msg The message to send.
  */
 virtual void Send (const McpttCallMsg& msg);
 /**
  * Sets the callback used to indicate that a new call exists.
  * \param newCallCb The callback.
  */
 virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb);
 /**
  * Sets the owner of the call machine.
  * \param owner The owner.
  */
 virtual void SetOwner (Ptr<McpttServerCall> owner);
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
protected:
 /**
  * Disposes of this McpttServerCallMachineNull instance.
  */
 virtual void DoDispose (void);
private:
 Ptr<McpttServerCall> m_owner; //!< The owner.
 uint16_t m_floorPort; //!< The port number to use for floor control.
 uint16_t m_mediaPort; //!< The port number to use for media.
 uint8_t m_callType; //!< The call type.
};

} // namespace ns3

#endif /* MCPTT_SERVER_CALL_MACHINE_H */

