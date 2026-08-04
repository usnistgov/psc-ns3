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

#ifndef MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_H
#define MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_H

#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-participant.h"
#include "mcptt-media-msg.h"
#include "mcptt-sdp-fmtp-header.h"
#include "mcptt-timer.h"

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

class McpttCall;
class McpttOnNetworkFloorParticipantState;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT floor control state machine described in TS
 * 24.380 v14.8.0.
 */
class McpttOnNetworkFloorParticipant : public McpttFloorParticipant
{
  public:
    /**
     * \brief Gets the ID of the McpttOnNetworkFloorParticipant type.
     * \returns The type ID.
     */
    static TypeId GetTypeId();
    /**
     * \brief Creates an instance of the McpttOnNetworkFloorParticipant class.
     */
    McpttOnNetworkFloorParticipant();
    /**
     * \brief The destructor of the McpttOnNetworkFloorParticipant class.
     */
    ~McpttOnNetworkFloorParticipant() override;
    /**
     * Accepts the floor grant.
     */
    void AcceptGrant() override;
    /**
     * Notifies the floor machine that the call has been initiated
     * (originating MCPTT user).
     */
    virtual void CallInitiated();
    /**
     * Notifies the floor machine that the call has been established
     * \param sdpHeader The SDP header.
     */
    virtual void CallEstablished(const McpttSdpFmtpHeader& sdpHeader);
    /**
     * Notifies the floor machine that the call has been released (part I).
     */
    virtual void CallRelease1();
    /**
     * Notifies the floor machine that the call has been released (part II).
     */
    virtual void CallRelease2();
    /**
     * Changes the state of the floor machine.
     * \param state The state to change to.
     */
    virtual void ChangeState(Ptr<McpttOnNetworkFloorParticipantState> state);
    /**
     * Notifies the floor machine that it has been granted the floor.
     */
    virtual void FloorGranted();
    /**
     * Gets the ID of the current call type.
     * \returns The call type ID.
     */
    virtual uint8_t GetCallTypeId() const;
    /**
     * Gets the floor indicator to use when sending a message.
     * \returns The floor indicator field.
     */
    virtual McpttFloorMsgFieldIndic GetIndicator() const;
    /**
     * Gets the type ID of this McpttFloorMachine instance.
     * \returns The type ID.
     */
    TypeId GetInstanceTypeId() const override;
    /**
     * Gets the ID of the state.
     * \returns The state ID.
     */
    virtual McpttEntityId GetStateId() const;
    /**
     * Gets the state of the floor machine.
     * \returns The state.
     */
    virtual Ptr<McpttOnNetworkFloorParticipantState> GetState() const;
    /**
     * Gets the SSRC to use when sending a message.
     * \returns The SSRC to use when sending a message.
     */
    virtual uint32_t GetTxSsrc() const;
    /**
     * Indicates whether or not the current participant is the acting arbitrator.
     * \returns True, if the current participant is the acting arbitrator.
     */
    bool HasFloor() const override;
    /**
     * Indicates if acknowledgement is required.
     * \returns True, if acknowledgement is required; otherwise, false.
     */
    virtual bool IsAckRequired() const;
    /**
     * Indicates whether or not dual floor has been indicated.
     * \returns True, if the dual floor has been indicated; otherwise, false.
     */
    virtual bool IsDualFloor() const;
    /**
     * Indicates if the SIP INVITE SDP offer should include an implicit floor request.
     * \returns True, if the implicit floor request is included; otherwise, false.
     */
    virtual bool IsImplicitRequest() const;
    /**
     * Indicates if the SIP response included an implicit floor request.
     * \returns True, if the implicit floor request was included; otherwise, false.
     */
    virtual bool IsGranted() const;
    /**
     * Indicates whether or not this participant is the originator.
     * \returns True, if this participant is the originator, or false otherwise.
     */
    virtual bool IsOriginator() const;
    /**
     * Indicates whether or not this participant is overridden without revoke.
     * \returns True, if this participant is overridden without revoke, or false otherwise.
     */
    virtual bool IsOverridden() const;
    /**
     * Indicates whether or not this participant is overriding without revoke.
     * \returns True, if this participant is overriding without revoke, or false otherwise.
     */
    virtual bool IsOverriding() const;
    /**
     * Indicates whether or not the floor machine has been started.
     * \returns True, if the floor machine has been started.
     */
    bool IsStarted() const override;
    /**
     * Notifies the floor machine that RTP media is ready to be sent.
     * \param msg The message ready to be sent.
     */
    void MediaReady(McpttMediaMsg& msg) override;
    /**
     * \brief Receives a message.
     * \param msg The message that was received.
     */
    void Receive(const McpttFloorMsg& msg) override;
    /**
     * \brief Receives a message.
     * \param msg The message that was received.
     */
    void Receive(const McpttMediaMsg& msg) override;
    /**
     * Receives a floor acknowledgement message.
     * \param msg The received message.
     */
    void ReceiveFloorAck(const McpttFloorMsgAck& msg) override;
    /**
     * Receives a floor deny message.
     * \param msg The received message.
     */
    void ReceiveFloorDeny(const McpttFloorMsgDeny& msg) override;
    /**
     * Receives a floor granted message.
     * \param msg The received message.
     */
    void ReceiveFloorGranted(const McpttFloorMsgGranted& msg) override;
    /**
     * Receives a floor idle message.
     * \param msg The received message.
     */
    void ReceiveFloorIdle(const McpttFloorMsgIdle& msg) override;
    /**
     * Receives a floor queue position info message.
     * \param msg The received message.
     */
    void ReceiveFloorQueuePositionInfo(const McpttFloorMsgQueuePositionInfo& msg) override;
    /**
     * Receives a floor revoke message.
     * \param msg The received message.
     */
    void ReceiveFloorRevoke(const McpttFloorMsgRevoke& msg) override;
    /**
     * Receives a floor taken message.
     * \param msg The received message.
     */
    void ReceiveFloorTaken(const McpttFloorMsgTaken& msg) override;
    /**
     * Receives an RTP media message.
     * \param msg The received message.
     */
    void ReceiveMedia(const McpttMediaMsg& msg) override;
    /**
     * Releases a request.
     */
    void ReleaseRequest() override;
    /**
     * \brief Sends a message using the message sender.
     * \param msg The message to send.
     */
    virtual void Send(const McpttFloorMsg& msg);
    /**
     * Sends a floor queue position request message.
     */
    void SendFloorQueuePositionRequest() override;
    /**
     * Sets the delay for timer T100.
     * \param delayT100 The delay to use.
     */
    virtual void SetDelayT100(const Time& delayT100);
    /**
     * Sets the delay for timer T101.
     * \param delayT101 The delay to use.
     */
    virtual void SetDelayT101(const Time& delayT101);
    /**
     * Sets the delay for timer T103.
     * \param delayT103 The delay to use.
     */
    virtual void SetDelayT103(const Time& delayT103);
    /**
     * Sets the delay for timer T104.
     * \param delayT104 The delay to use.
     */
    virtual void SetDelayT104(const Time& delayT104);
    /**
     * Sets the delay for timer T132.
     * \param delayT132 The delay to use.
     */
    virtual void SetDelayT132(const Time& delayT132);
    /**
     * Sets the limit for counter C100.
     * \param limitC100 The limit to use.
     */
    virtual void SetLimitC100(uint32_t limitC100);
    /**
     * Sets the limit for counter C101.
     * \param limitC101 The limit to use.
     */
    virtual void SetLimitC101(uint32_t limitC101);
    /**
     * Sets the limit for counter C104.
     * \param limitC104 The limit to use.
     */
    virtual void SetLimitC104(uint32_t limitC104);
    /**
     * Indicates if media should automatically be generated in the 'O: has permission' state.
     * \returns True, if media should be generated; false, otherwise.
     */
    virtual bool ShouldGenMedia() const;
    /**
     * Notifies this machine that the button has been pushed.
     */
    void PttPush() override;
    /**
     * Notifies this machine that the button has been released.
     */
    void PttRelease() override;
    /**
     * Starts the McpttOnNetworkFloorParticipant state machine.
     */
    void Start() override;
    /**
     * Stop the McpttOnNetworkFloorParticipant state machine.
     */
    void Stop() override;
    /**
     * \brief Report an event to the McpttPttApp.
     * \param reason the reason to report.
     */
    virtual void ReportEvent(const char* reason) const;

  protected:
    /**
     * \brief Disposes of the McpttFloorMachine.
     */
    void DoDispose() override;
    /**
     * Notifies the floor machine that timer T100 has expired.
     */
    virtual void ExpiryOfT100();
    /**
     * Notifies the floor machine that timer T101 has expired.
     */
    virtual void ExpiryOfT101();
    /**
     * Notifies the floor machine that timer T103 has expired.
     */
    virtual void ExpiryOfT103();
    /**
     * Notifies the floor machine that timer T104 has expired.
     */
    virtual void ExpiryOfT104();
    /**
     * Notifies the floor machine that timer T132 has expired.
     */
    virtual void ExpiryOfT132();

  private:
    bool m_ackRequired;              //!< A flag that indicates if acknowledgement is required.
    Ptr<McpttCounter> m_c100;        //!< The counter associated with T100.
    Ptr<McpttCounter> m_c101;        //!< The counter associated with T101.
    Ptr<McpttCounter> m_c104;        //!< The counter associated with T104.
    bool m_dualFloor;                //!< A flag that to indicate dual floor indication.
    Callback<void> m_floorGrantedCb; //!< The floor granted callback.
    bool m_genMedia;                 //!< The flag that indicates if media should be generated.
    bool m_implicitRequest; //!< The flag that indicates if INVITE contains implicit floor request.
    bool m_granted;         //!< The flag that indicates if implicit request was granted.
    bool m_originator;      //!< A flag that indicates if this floor machine is the call originator.
    bool m_overridden;     //!< A flag that indicates whether the user is overridden without revoke.
    bool m_overriding;     //!< A flag that indicates whether the user is overriding without revoke.
    Ptr<McpttCall> m_call; //!< The McpttCall that owns this floor machine.
    uint8_t m_priority;    //!< The priority of this floor machine.
    Ptr<McpttOnNetworkFloorParticipantState> m_state; //!< The state of the floor machine.
    Callback<void, const McpttEntityId&, const McpttEntityId&>
        m_stateChangeCb; //!< The state change callback.
    TracedCallback<uint32_t,
                   uint16_t,
                   const std::string&,
                   const std::string&,
                   const std::string&,
                   const std::string&>
        m_stateChangeTrace;   //!< The state change traced callback.
    Ptr<Packet> m_storedMsgs; //!< The collection of stored messages.
    Ptr<McpttTimer> m_t100;   //!< The timer T100.
    Ptr<McpttTimer> m_t101;   //!< The timer T101.
    Ptr<McpttTimer> m_t103;   //!< The timer T103.
    Ptr<McpttTimer> m_t104;   //!< The timer T104.
    Ptr<McpttTimer> m_t132;   //!< The timer T132.

  public:
    /**
     * Gets the counter C100
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC100() const;
    /**
     * Gets the counter C101
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC101() const;
    /**
     * Gets the counter C104
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC104() const;
    /**
     * Gets the McpttCall for the state machine.
     * \returns The call object
     */
    virtual Ptr<McpttCall> GetCall() const;
    /**
     * Gets the priority of the state machine.
     * \returns The priority.
     */
    virtual uint8_t GetPriority() const;
    /**
     * Gets the collection of stored messages.
     * \returns A packet with that includes the messages.
     */
    virtual Ptr<Packet> GetStoredMsgs() const;
    /**
     * Gets the timer T100.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT100() const;
    /**
     * Gets the timer T101.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT101() const;
    /**
     * Gets the timer T103.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT103() const;
    /**
     * Gets the timer T104.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT104() const;
    /**
     * Gets the timer T132.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT132() const;
    /**
     * A flag that indicates dual floor or not.
     * \param dualFloor The flag.
     */
    virtual void SetDualFloor(const bool& dualFloor);
    /**
     * Sets the callback used to notify a floor granted.
     * \param floorGrantedCb The callback.
     */
    void SetFloorGrantedCb(const Callback<void> floorGrantedCb) override;
    /**
     * Sets the flag that indicates if this floor machine is the originator of the call.
     * \param originator True, if this participant is the originator of the call.
     */
    void SetOriginator(const bool& originator) override;
    /**
     * Sets the flag that indicates if the participant is overridden without revoke.
     * \param overridden The flag.
     */
    virtual void SetOverridden(const bool& overridden);
    /**
     * Sets the flag that indicates if the participant is overriding without revoke.
     * \param overriding The flag.
     */
    virtual void SetOverriding(const bool& overriding);
    /**
     * Sets the McpttCall associated with the floor machine.
     * \param call The call.
     */
    void SetCall(Ptr<McpttCall> call) override;
    /**
     * Sets the current priority of the floor.
     * \param priority The current priority.
     */
    void SetPriority(uint8_t priority) override;
    /**
     * Sets the state of the floor machine.
     * \param state The state.
     */
    virtual void SetState(Ptr<McpttOnNetworkFloorParticipantState> state);
    /**
     * Sets the state change call back.
     * \param stateChangeCb The setup delay callback.
     */
    virtual void SetStateChangeCb(
        const Callback<void, const McpttEntityId&, const McpttEntityId&> stateChangeCb);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_H */
