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

#ifndef MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_H
#define MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_H

#include "mcptt-counter.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-participant.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"
#include "mcptt-off-network-floor-participant-state.h"
#include "mcptt-timer.h"

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT floor control state mahine described in TS
 * 24.380 v14.4.0.
 */
class McpttOffNetworkFloorParticipant : public McpttFloorParticipant
{
  public:
    /**
     * \brief Gets the ID of the McpttOffNetworkFloorParticipant type.
     * \returns The type ID.
     */
    static TypeId GetTypeId();
    /**
     * \brief Creates an instance of the McpttOffNetworkFloorParticipant class.
     */
    McpttOffNetworkFloorParticipant();
    /**
     * \brief The destructor of the McpttOffNetworkFloorParticipant class.
     */
    ~McpttOffNetworkFloorParticipant() override;
    /**
     * Accepts the floor grant.
     */
    void AcceptGrant() override;
    /**
     * Changes the state of the floor machine.
     * \param state The state to change to.
     */
    virtual void ChangeState(Ptr<McpttOffNetworkFloorParticipantState> state);
    /**
     * Clears the SSRC stored for the candidate arbitrator.
     */
    virtual void ClearCandidateSsrc();
    /**
     * Clears the SSRC stored for the current arbitrator.
     */
    virtual void ClearCurrentSsrc();
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
     * Gets the type ID of this McpttOffNetworkFloorParticipant instance.
     * \returns The type ID.
     */
    TypeId GetInstanceTypeId() const override;
    /**
     * Gets the ID of the state.
     * \returns The state ID.
     */
    virtual McpttEntityId GetStateId() const;
    /**
     * Gets the SSRC to use when sending a message.
     * \returns The SSRC to use when sending a message.
     */
    virtual uint32_t GetTxSsrc() const;
    /**
     * Indicates if an SSRC is stored for the candidate arbitrator.
     * \returns True, if the an SSRC is stored; otherwise false.
     */
    virtual bool HasCandidateSsrc() const;
    /**
     * Indicates if an SSRC is stored for the current arbitrator.
     * \returns True, if an SSRC is stored; otherwise false.
     */
    virtual bool HasCurrentSsrc() const;
    /**
     * Indicates whether or not the current participant is the acting arbitrator.
     * \returns True, if the current participant is the acting arbitrator.
     */
    bool HasFloor() const override;
    /**
     * Indicates whether or not this participant is the origninator.
     * \returns True, if this participant is the orignator, or false otherwise.
     */
    virtual bool IsOriginator() const;
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
     * Notifes the floor machine that it has been granted the floor.
     */
    virtual void NotifyFloorGranted();
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
     * Receives a floor deny message.
     * \param msg The received message.
     */
    void ReceiveFloorDeny(const McpttFloorMsgDeny& msg) override;
    /**
     * Receives a floor acknowledgement message.
     * \param msg The received message.
     */
    void ReceiveFloorAck(const McpttFloorMsgAck& msg) override;
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
     * Receives Floor Queue Position Request message.
     * \param msg The received message.
     */
    void ReceiveFloorQueuePositionRequest(const McpttFloorMsgQueuePositionRequest& msg) override;
    /**
     * Receives a floor queue position info message.
     * \param msg The received message.
     */
    void ReceiveFloorQueuePositionInfo(const McpttFloorMsgQueuePositionInfo& msg) override;
    /**
     * Receives a floor release message.
     * \param msg The received message.
     */
    void ReceiveFloorRelease(const McpttFloorMsgRelease& msg) override;
    /**
     * Receives a floor request message.
     * \param msg The received message.
     */
    void ReceiveFloorRequest(const McpttFloorMsgRequest& msg) override;
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
     * Resets all counters.
     */
    virtual void ResetCounters();
    /**
     * Reports an MCPTT event.
     */
    virtual void ReportEvent(const char* reason) const;
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
     * Sets the delay for timer T201.
     * \param delayT201 The delay to use.
     */
    virtual void SetDelayT201(const Time& delayT201);
    /**
     * Sets the delay for timer T203.
     * \param delayT203 The delay to use.
     */
    virtual void SetDelayT203(const Time& delayT203);
    /**
     * Sets the delay for timer T204.
     * \param delayT204 The delay to use.
     */
    virtual void SetDelayT204(const Time& delayT204);
    /**
     * Sets the delay for timer T205.
     * \param delayT205 The delay to use.
     */
    virtual void SetDelayT205(const Time& delayT205);
    /**
     * Sets the delay for timer T206.
     * \param delayT206 The delay to use.
     */
    virtual void SetDelayT206(const Time& delayT206);
    /**
     * Sets the delay for timer T207.
     * \param delayT207 The delay to use.
     */
    virtual void SetDelayT207(const Time& delayT207);
    /**
     * Sets the delay for timer T230.
     * \param delayT230 The delay to use.
     */
    virtual void SetDelayT230(const Time& delayT230);
    /**
     * Sets the delay for timer T233.
     * \param delayT233 The delay to use.
     */
    virtual void SetDelayT233(const Time& delayT233);
    /**
     * Sets the limit for counter C201.
     * \param limitC201 The limit to use.
     */
    virtual void SetLimitC201(uint32_t limitC201);
    /**
     * Sets the limit for counter C204.
     * \param limitC204 The limit to use.
     */
    virtual void SetLimitC204(uint32_t limitC204);
    /**
     * Sets the limit for counter C205.
     * \param limitC205 The limit to use.
     */
    virtual void SetLimitC205(uint32_t limitC205);
    /**
     * Indicates if media should automatically be generated in the 'O: has permission' state.
     * \returns True, if media should be generated; false, otherwise.
     */
    virtual bool ShouldGenMedia() const;
    /**
     * Starts the floor machine.
     */
    void Start() override;
    /**
     * Stops the floor machine.
     */
    void Stop() override;
    /**
     * Stops all timers.
     */
    virtual void StopTimers();
    /**
     * Notifies this machine that the button has been pushed.
     */
    void PttPush() override;
    /**
     * Notifies this machine that the button has been released.
     */
    void PttRelease() override;

  protected:
    /**
     * \brief Disposes of the McpttLfloorMachine.
     */
    void DoDispose() override;
    /**
     * Notifies the floor machine that timer T201 has expired.
     */
    virtual void ExpiryOfT201();
    /**
     * Notifies the floor machine that timer T203 has expired.
     */
    virtual void ExpiryOfT203();
    /**
     * Notifies the floor machine that timer T204 has expired.
     */
    virtual void ExpiryOfT204();
    /**
     * Notifies the floor machine that timer T205 has expired.
     */
    virtual void ExpiryOfT205();
    /**
     * Notifies the floor machine that timer T206 has expired.
     */
    virtual void ExpiryOfT206();
    /**
     * Notifies the floor machine that timer T207 has expired.
     */
    virtual void ExpiryOfT207();
    /**
     * Notifies the floor machine that timer T230 has expired.
     */
    virtual void ExpiryOfT230();
    /**
     * Notifies the floor machine that timer T233 has expired.
     */
    virtual void ExpiryOfT233();

  private:
    Ptr<McpttCounter> m_c201;            //!< The counter associated with T201.
    Ptr<McpttCounter> m_c204;            //!< The counter associated with T204.
    Ptr<McpttCounter> m_c205;            //!< The counter associated with T205.
    Callback<void> m_floorGrantedCb;     //!< The floor granted callback.
    bool m_genMedia;                     //!< The flag that indicates if media should be generated.
    McpttFloorMsgGranted m_lastGrantMsg; //!< The last floor granted message that was sent.
    bool m_originator; //!< A flag that indicates if this floor machine is the call originator.
    /* NOTE:
     * Using a raw pointer here because this type represents
     * an interface that does not inheret from any of the ns3
     * base classes. Therefore, this class should not allocate
     * or dispose of the memory referenced by this pointer, and
     * it is up to the user to provide a valid object. This
     * pointer's sole purpose is to provide access to the
     * object that the user has provided.
     */
    Ptr<McpttCall> m_call;                      //!< The McpttCall that owns this floor machine.
    uint8_t m_priority;                         //!< The priority of this floor machine.
    Ptr<McpttFloorQueue> m_queue;               //!< The floor queue.
    Time m_setupDelayStartTime;                 //!< The setup delay time.
    Callback<void, const Time&> m_setupDelayCb; //!< The setup delay call back.
    bool m_started; //!< A flag that indicates whether or not the floor machine has started.
    Ptr<McpttOffNetworkFloorParticipantState> m_state; //!< The state of the floor machine.
    Callback<void, const McpttEntityId&, const McpttEntityId&>
        m_stateChangeCb; //!< The state change call back.
    TracedCallback<uint32_t,
                   uint16_t,
                   const std::string&,
                   const std::string&,
                   const std::string&,
                   const std::string&>
        m_stateChangeTrace;   //!< The state change traced callback.
    uint32_t m_candidateSsrc; //!< The SSRC of the candidate arbitrator.
    uint32_t m_currentSsrc;   //!< The SSRC of the current arbitrator.
    Ptr<McpttTimer> m_t201;   //!< The timer T201.
    Ptr<McpttTimer> m_t203;   //!< The timer T203.
    Ptr<McpttTimer> m_t204;   //!< The timer T204.
    Ptr<McpttTimer> m_t205;   //!< The timer T205.
    Ptr<McpttTimer> m_t206;   //!< The timer T206.
    Ptr<McpttTimer> m_t207;   //!< The timer T207.
    Ptr<McpttTimer> m_t230;   //!< The timer T230.
    Ptr<McpttTimer> m_t233;   //!< The timer T233.

  protected:
    /**
     * Sets the flag that indicates if the floor machine has been started.
     * \param started The flag.
     */
    virtual void SetStarted(const bool& started);

  public:
    /**
     * Gets the counter associated with timer T201.
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC201() const;
    /**
     * Gets the counter associated with timer T204.
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC204() const;
    /**
     * Gets the counter associated with timer T205.
     * \returns The counter.
     */
    virtual Ptr<McpttCounter> GetC205() const;
    /**
     * Gets the last floor granted message that was sent.
     * \returns The message.
     */
    virtual McpttFloorMsgGranted GetLastGrantMsg() const;
    /**
     * Gets the flag that is used to indicate if this floor machine is the call originator.
     * \returns True, if this participant is the originator, or false otherwise.
     */
    virtual bool GetOriginator() const;
    /**
     * Gets the McpttCall for the state machine.
     * \returns The call object
     */
    virtual Ptr<McpttCall> GetCall() const;
    /**
     * Gets the current priority of the floor.
     * \returns The current priority.
     */
    virtual uint8_t GetPriority() const;
    /**
     * Gets the queue.
     * \returns The queue.
     */
    virtual Ptr<McpttFloorQueue> GetQueue() const;
    /**
     * Gets the setup delay.
     * \returns The setup delay time.
     */
    virtual Time GetSetupDelayStartTime() const;
    /**
     * Gets the flag that indicates whether or not the floor machine has been started.
     * \returns The flag.
     */
    virtual bool GetStarted() const;
    /**
     * Gets the state of the floor machine.
     * \returns The state.
     */
    virtual Ptr<McpttOffNetworkFloorParticipantState> GetState() const;
    /**
     * Gets the SSRC stored for the candidate arbitrator.
     * \returns The SSRC.
     */
    virtual uint32_t GetCandidateSsrc() const;
    /**
     * Gets the SSRC stored for the current arbitrator.
     * \returns The SSRC.
     */
    virtual uint32_t GetCurrentSsrc() const;
    /**
     * Gets the timer T201.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT201() const;
    /**
     * Gets the timer T203.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT203() const;
    /**
     * Gets the timer T204.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT204() const;
    /**
     * Gets the timer T205.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT205() const;
    /**
     * Gets the timer T206.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT206() const;
    /**
     * Gets the timer T207.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT207() const;
    /**
     * Gets the timer T230.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT230() const;
    /**
     * Gets the timer T233.
     * \returns The timer.
     */
    virtual Ptr<McpttTimer> GetT233() const;
    /**
     * Sets the counter associated with timer T201.
     * \param c201 The counter.
     */
    virtual void SetC201(Ptr<McpttCounter> c201);
    /**
     * Sets the counter associated with timer T204.
     * \param c204 The counter.
     */
    virtual void SetC204(Ptr<McpttCounter> c204);
    /**
     * Sets the counter associated with timer T205.
     * \param c205 The counter.
     */
    virtual void SetC205(Ptr<McpttCounter> c205);
    /**
     * Sets the callback used to notify a floor granted.
     * \param floorGrantedCb The callback.
     */
    void SetFloorGrantedCb(const Callback<void> floorGrantedCb) override;
    /**
     * Sets the last floor granted message that was sent.
     * \param msg The last sent floor granted message.
     */
    virtual void SetLastGrantMsg(const McpttFloorMsgGranted& msg);
    /**
     * Sets the flag that indicates if this floor machine is the originator of the call.
     * \param originator True, if this participant is the originator of the call.
     */
    void SetOriginator(const bool& originator) override;
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
     * Sets the queue.
     * \param queue The queue.
     */
    virtual void SetQueue(Ptr<McpttFloorQueue> queue);
    /**
     * Sets the setup delay.
     * \param startTime The setup delay time.
     */
    virtual void SetSetupDelayStartTime(const Time& startTime);
    /**
     * Sets the setup delay call back.
     * \param setupDelayCb The setup delay callback.
     */
    virtual void SetSetupDelayCb(const Callback<void, const Time&> setupDelayCb);
    /**
     * Sets the state of the floor machine.
     * \param state The state.
     */
    virtual void SetState(Ptr<McpttOffNetworkFloorParticipantState> state);
    /**
     * Sets the state change call back.
     * \param stateChangeCb The setup delay callback.
     */
    virtual void SetStateChangeCb(
        const Callback<void, const McpttEntityId&, const McpttEntityId&> stateChangeCb);
    /**
     * Sets the SSRC stored for the candidate arbitrator.
     * \param candidateSsrc The SSRC.
     */
    virtual void SetCandidateSsrc(uint32_t candidateSsrc);
    /**
     * Sets the SSRC stored for the current arbitrator.
     * \param currentSsrc The SSRC.
     */
    virtual void SetCurrentSsrc(uint32_t currentSsrc);
    /**
     * Sets the timer T201.
     * \param t201 The timer.
     */
    virtual void SetT201(Ptr<McpttTimer> t201);
    /**
     * Sets the timer T203.
     * \param t203 The timer.
     */
    virtual void SetT203(Ptr<McpttTimer> t203);
    /**
     * Sets the timer T204.
     * \param t204 The timer.
     */
    virtual void SetT204(Ptr<McpttTimer> t204);
    /**
     * Sets the timer T205.
     * \param t205 The timer.
     */
    virtual void SetT205(Ptr<McpttTimer> t205);
    /**
     * Sets the timer T206.
     * \param t206 The timer.
     */
    virtual void SetT206(Ptr<McpttTimer> t206);
    /**
     * Sets the timer T207.
     * \param t207 The timer.
     */
    virtual void SetT207(Ptr<McpttTimer> t207);
    /**
     * Sets the timer T230.
     * \param t230 The timer.
     */
    virtual void SetT230(Ptr<McpttTimer> t230);
    /**
     * Sets the timer T233.
     * \param t233 The timer.
     */
    virtual void SetT233(Ptr<McpttTimer> t233);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_H */
