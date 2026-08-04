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

#ifndef MCPTT_CALL_MACHINE_GRP_BROADCAST_H
#define MCPTT_CALL_MACHINE_GRP_BROADCAST_H

#include "mcptt-call-machine-grp-broadcast-state.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-msg.h"
#include "mcptt-call.h"
#include "mcptt-timer.h"

#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT broadcast group call control state machine
 * as described in TS.379 version 14.4.0.
 */
class McpttCallMachineGrpBroadcast : public McpttCallMachineGrp
{
  public:
    /**
     * Gets the type ID of the McpttCallMachineGrpBroadcast class.
     * \returns The type ID.
     */
    static TypeId GetTypeId();
    /**
     * Creates an instance of the McpttCallMachineGrpBroadcast class.
     * \param owner The owner of the call machine.
     */
    McpttCallMachineGrpBroadcast(Ptr<McpttCall> owner = nullptr);
    /**
     * \brief The destructor of the McpttCallMachineGrpBroadcast class.
     */
    ~McpttCallMachineGrpBroadcast() override;
    /**
     * Accepts the call.
     */
    void AcceptCall() override;
    /**
     * Begins an emergency alert.
     */
    void BeginEmergAlert() override;
    /**
     * Cancels an emergency alert.
     */
    void CancelEmergAlert() override;
    /**
     * Changes the state of the state machine.
     * \param state The state to change to.
     */
    virtual void ChangeState(Ptr<McpttCallMachineGrpBroadcastState> state);
    /**
     * Downgrades the call type.
     */
    void DowngradeCallType() override;
    /**
     * Gets the ID of the user that started the call.
     * \returns The MCPTT user ID.
     */
    uint32_t GetCallerUserId() const override;
    /**
     * Gets the type ID of this McpttCallMachineGrpBroadcast instance.
     * \returns The type ID.
     */
    TypeId GetInstanceTypeId() const override;
    /**
     * Gets the ID of the current state.
     * \returns The state ID.
     */
    McpttEntityId GetStateId() const override;
    /**
     * Initiates a call.
     */
    void InitiateCall() override;
    /**
     * Indicates if the call is active.
     * \returns True, if the call is active.
     */
    bool IsCallOngoing() const override;
    /**
     * Indicates if the call control machine has been started.
     * \returns True, if the call control machine has been started, or false otherwise.
     */
    virtual bool IsStarted() const;
    /**
     * Indicates if user acknowledgements are required.
     * \returns True, if user acknowledgements are required; otherwise, false.
     */
    virtual bool IsUserAckReq() const;
    /**
     * Receives a call control message.
     * \param msg The message to receive.
     */
    void Receive(const McpttCallMsg& msg) override;
    /**
     * Receives a media message.
     * \param msg The message to receive.
     */
    void Receive(const McpttMediaMsg& msg) override;
    /**
     * Receives a "GROUP CALL BROADCAST" message.
     * \param msg The message to receive.
     */
    virtual void ReceiveGrpCallBroadcast(const McpttCallMsgGrpBroadcast& msg);
    /**
     * Receives a "GROUP CALL BROADCAST END" message.
     * \param msg The message to receive.
     */
    virtual void ReceiveGrpCallBroadcastEnd(const McpttCallMsgGrpBroadcastEnd& msg);
    /**
     * Releases the call.
     */
    void ReleaseCall() override;
    /**
     * Rejects the call.
     */
    void RejectCall() override;
    /**
     * \brief Report an event to the McpttPttApp.
     * \param reason the reason to report.
     */
    virtual void ReportEvent(const char* reason) const;
    /**
     * Sends a call control message.
     * \param msg The message to send.
     */
    void Send(const McpttCallMsg& msg) override;
    /**
     * Sets the delay for timer TFB1.
     * \param delayTfb1 The delay to use.
     */
    virtual void SetDelayTfb1(const Time& delayTfb1);
    /**
     * Sets the delay for timer TFB2.
     * \param delayTfb2 The delay to use.
     */
    virtual void SetDelayTfb2(const Time& delayTfb2);
    /**
     * Sets the delay for timer TFB3.
     * \param delayTfb3 The delay to use.
     */
    virtual void SetDelayTfb3(const Time& delayTfb3);
    /**
     * Sets the group ID.
     * \param grpId The group ID.
     */
    void SetGrpId(uint32_t grpId) override;
    /**
     * Starts the state machine.
     */
    void Start() override;
    /**
     * Stops the state machine.
     */
    void Stop() override;
    /**
     * Takes a new call notification.
     * \param callId The ID of the new call.
     */
    virtual void TakeNewCallNotification(uint16_t callId);
    /**
     * Upgrades the call type.
     * \param callType The call type value.
     */
    void UpgradeCallType(uint8_t callType) override;

  protected:
    /**
     * Disposes of the McpttCallMachineGrpBroadcast.
     */
    void DoDispose() override;
    /**
     * Expiry of TFB1.
     */
    virtual void ExpiryOfTfb1();
    /**
     * Expiry of TFB2.
     */
    virtual void ExpiryOfTfb2();
    /**
     * Expiry of TFB3.
     */
    virtual void ExpiryOfTfb3();
    /**
     * Gets the flag that indicates if the call machine has been started.
     * \returns The flag.
     */
    virtual bool GetStarted() const;
    /**
     * Sets the flag that indicates if the call machine has been started.
     * \param started The flag.
     */
    virtual void SetStarted(const bool& started);

  private:
    McpttCallMsgFieldCallId m_callId;     //!< The ID of the call.
    McpttCallMsgFieldCallType m_callType; //!< The current call type.
    McpttCallMsgFieldGrpId m_grpId;       //!< The group ID of the call.
    Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
    McpttCallMsgFieldUserId m_origId;     //!< The originating MCPTT user's ID.
    Ptr<McpttCall> m_call;                //!< The owner under which the call machine resides.
    uint8_t m_priority;                   //!< The ProSe per-packet priority.
    Ptr<UniformRandomVariable> m_randomCallIdGenerator; //!< The RNG used for call ID generation.
    McpttCallMsgFieldSdp m_sdp;                         //!< SDP information.
    bool m_started; //!< The flag that indicates if the state machine has been started.
    Ptr<McpttCallMachineGrpBroadcastState> m_state; //!< The current state.
    Callback<void, const McpttEntityId&, const McpttEntityId&>
        m_stateChangeCb; //!< The state changed callback.
    TracedCallback<uint32_t,
                   uint16_t,
                   const std::string&,
                   const std::string&,
                   const std::string&,
                   const std::string&>
        m_stateChangeTrace; //!< The state change traced callback.
    Ptr<McpttTimer> m_tfb1; //!< The timer TFB1.
    Ptr<McpttTimer> m_tfb2; //!< The timer TFB2.
    Ptr<McpttTimer> m_tfb3; //!< The timer TFB3.
    bool m_userAckReq;      //!< Indicates if user acknowledgments are required.

  public:
    /**
     * Gets the ID of the MCPTT call.
     * \returns The ID.
     */
    McpttCallMsgFieldCallId GetCallId() const override;
    /**
     * Gets the current call type.
     * \returns The call type.
     */
    McpttCallMsgFieldCallType GetCallType() const override;
    /**
     * Gets the ID of the MCPTT group.
     * \returns The ID of the MCPTT group.
     */
    McpttCallMsgFieldGrpId GetGrpId() const override;
    /**
     * Gets the ID of the MCPTT user that the call originated from.
     * \returns The ID of the MCPTT user.
     */
    virtual McpttCallMsgFieldUserId GetOrigId() const;
    /**
     * Gets the McpttCall associated with the call machine.
     * \returns The call.
     */
    Ptr<McpttCall> GetCall() const override;
    /**
     * Gets the ProSe per-packet priority.
     * \returns The ProSe per-packet priority.
     */
    virtual uint8_t GetPriority() const;
    /**
     * Generate and return a randomly generated call ID
     * \returns The random call ID generated.
     */
    virtual uint16_t GenerateRandomCallId() const;
    /**
     * Gets the SDP information.
     * \returns The SDP information.
     */
    virtual McpttCallMsgFieldSdp GetSdp() const;
    /**
     * Gets the state.
     * \returns The state.
     */
    virtual Ptr<McpttCallMachineGrpBroadcastState> GetState() const;
    /**
     * Gets the timer TFB1.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetTfb1() const;
    /**
     * Gets the timer TFB2.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetTfb2() const;
    /**
     * Gets the timer TFB3.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetTfb3() const;
    /**
     * Sets the ID of the MCPTT call.
     * \param callId The ID of the MCPTT call.
     */
    void SetCallId(const McpttCallMsgFieldCallId& callId) override;
    /**
     * Sets the current call type.
     * \param callType The current call type.
     */
    virtual void SetCallType(const McpttCallMsgFieldCallType& callType);
    /**
     * Sets the ID of the MCPTT group.
     * \param grpId The ID of the MCPTT group.
     */
    virtual void SetGrpId(const McpttCallMsgFieldGrpId& grpId);
    /**
     * Sets the callback used to notify a new call.
     * \param newCallCb The callback to set
     */
    void SetNewCallCb(const Callback<void, uint16_t> newCallCb) override;
    /**
     * Sets the ID of the MCPTT user that the call orignated from.
     * \param origId The ID of the MCPTT user.
     */
    virtual void SetOrigId(const McpttCallMsgFieldUserId& origId);
    /**
     * Sets the McpttCall associated with the call machine.
     * \param call The call.
     */
    void SetCall(Ptr<McpttCall> call) override;
    /**
     * Sets the ProSe per-packet priority.
     * \param priority The ProSe per-packet priority.
     */
    virtual void SetPriority(uint8_t priority);
    /**
     * Sets the SDP inforamtion.
     * \param sdp The SDP information.
     */
    virtual void SetSdp(const McpttCallMsgFieldSdp& sdp);
    /**
     * Sets the state.
     * \param state The state.
     */
    virtual void SetState(Ptr<McpttCallMachineGrpBroadcastState> state);
    /**
     * Sets the state change callback.
     * \param stateChangeCb The callback.
     */
    virtual void SetStateChangeCb(
        const Callback<void, const McpttEntityId&, const McpttEntityId&> stateChangeCb);
    /**
     * Sets the timer TFB1
     * \param tfb1 The timer.
     */
    virtual void SetTfb1(Ptr<McpttTimer> tfb1);
    /**
     * Sets the timer TFB2.
     * \param tfb2 The timer.
     */
    virtual void SetTfb2(Ptr<McpttTimer> tfb2);
    /**
     * Sets the timer TFB3.
     * \param tfb3 The timer.
     */
    virtual void SetTfb3(Ptr<McpttTimer> tfb3);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_GRP_BROADCAST_H */
