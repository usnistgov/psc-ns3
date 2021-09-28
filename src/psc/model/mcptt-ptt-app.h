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

#ifndef MCPTT_PTT_APP_H
#define MCPTT_PTT_APP_H

#include <map>

#include <ns3/application.h>
#include <ns3/callback.h>
#include <ns3/header.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>
#include <ns3/sip-agent.h>

#include "mcptt-call.h"
#include "mcptt-call-msg.h"
#include "mcptt-channel.h"
#include "mcptt-media-sink.h"
#include "mcptt-media-src.h"

namespace ns3 {

namespace psc {

class SipHeader;
class McpttPusher;

/**
 * \defgroup psc Public Safety Communications
 *
 * This section documents the API of the ns-3 psc module.The module contains
 * models specific to Public Safety Communications (PSC) research.
 */
/**
 * \defgroup mcptt Mission Critical Push-To-Talk
 *
 *  This sections documents the Mission Critical Push-to-Talk API.
 */
/**
 * \ingroup mcptt
 *
 * This class is the main component of the MCPTT off-network model, and it
 * is what ties all of the MCPTT components together (MCPTT on-network relies
 * on additional server components). The application is the object used to
 * create and manage calls, as well as defining the functions for expected
 * user actions like starting an emergency alert or changing the call type. When
 * creating calls the user must give two object factories, one factory for
 * creating the desired call control machine and another for the desired floor
 * control machine. Once started, the user must use the "SelectCall"
 * function to select a call based on the call's index.  This will result in
 * a call notionally being in the foreground of the user's app and will allow
 * user actions such as push-to-talk to be applied to a particular call.
 */
class McpttPttApp : public Application,
                    public McpttMediaSink
{
public:
  /**
   * Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttPtt class.
   */
  McpttPttApp (void);
  /**
   * The destructor of the McpttPttApp class.
   */
  virtual ~McpttPttApp (void);
  /**
   * Get the SipAgent associated with this application.
   * \return pointer to SipAgent
   */
  Ptr<sip::SipAgent> GetSipAgent (void) const;
  /**
   * Gets the current port number
   * \returns The current port number.
   */
  static uint16_t GetCurrentPortNumber (void);
  /**
   * Gets the next port number to use.
   * \returns The next port number.
   */
  static uint16_t AllocateNextPortNumber (void);
  /**
   * Accepts a call.
   */
  virtual void AcceptCall (void);
  /**
   * Accepts a floor grant.
   */
  virtual void AcceptFloorGrant (void);
  /**
   * Begins an emergency alert.
   */
  virtual void BeginEmergAlert (void);
  /**
   * Cancels an emergency alert.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Creates a call using factory objects, and add to the container of calls
   * on this application instance.
   * \param callFac The factory used to create an McpttCallMachine.
   * \param floorFac The factory used to create an McpttFloorParticipant.
   * \param callType The type of network call (e.g. on-network)
   * \return pointer to newly created call
   */
  virtual Ptr<McpttCall> CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, McpttCall::NetworkCallType callType);
  /**
   * Creates a call using factory objects and a provided call ID, and add
   * to the container of calls on this application instance.
   * \param callFac The factory used to create an McpttCallMachine.
   * \param floorFac The factory used to create an McpttFloorParticipant.
   * \param callType The type of network call (e.g. on-network)
   * \param callId The call ID to use
   * \return pointer to newly created call
   */
  virtual Ptr<McpttCall> CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, McpttCall::NetworkCallType callType, uint16_t callId);
  /**
   * Adds a previously created call to the container of calls on this
   * application instance.
   * \param call The call to add
   */
  void AddCall (Ptr<McpttCall> call);
  /**
   * Downgrades the call type.
   */
  virtual void DowngradeCallType (void);
  /**
   * Gets the location of the node that this application is installed on.
   * \return The location of the node.
   */
  virtual Vector GetNodeLoc (void) const;
  /**
   * Indicates if there is an existing group call.
   * \param grpId The group ID of the call to check for.
   * \returns True, if there is an existing group call; otherwise, false.
   */
  virtual bool HasGrpCallFor (uint32_t grpId);
  /**
   * Indicates if there is an existing private call.
   * \param userId The user ID of the call to check for.
   * \returns True, if there is an existing private call; otherwise, false.
   */
  virtual bool HasPrivateCallFor (uint32_t userId);
  /**
   * Initiates a call.
   */
  virtual void InitiateCall (void);
  /**
   * Process an MCPTT session initiate request received from call control.
   */
  virtual void SessionInitiateRequest (void);
  /**
   * Indicates whether or not the application is running
   * \returns True, if the application is running.
   */
  virtual bool IsRunning (void) const;
  /**
   * Indicates whether or not the PTT button is pushed.
   * \returns True, if the PTT button is pushed.
   */
  virtual bool IsPushed (void) const;
  /**
   * Notify the pusher that a push event has been generated by the app.
   */
  virtual void NotifyPushed (void);
  /**
   * Rejects a call.
   */
  virtual void RejectCall (void);
  /**
   * Release the selected call.
   */
  virtual void ReleaseCall (void);
  /**
   * Release the call specified by call ID.
   * \param callId The ID of the call.
   */
  virtual void ReleaseCallByCallId (uint32_t callId);
  /**
   * Releases a floor request.
   */
  virtual void ReleaseRequest (void);
  /**
   * Notify the pusher that a release event has been generated by the app.
   */
  virtual void NotifyReleased (void);
  /**
   * Process an MCPTT session release request received from call control.
   */
  virtual void SessionReleaseRequest (void);
  /**
   * Selects a call.  An existing active call will be released.  If the
   * pushOnSelect option is false, the user must separately restart the
   * pusher object.  If true, the pusher process will be restarted upon
   * SelectCall if the app is currently running.
   *
   * \param callId The ID of the call.
   * \param pushOnSelect Whether to start the pusher upon call selection
   */
  virtual void SelectCall (uint32_t callId, bool pushOnSelect = false);
  /**
   * Notifies the application that the button has been pushed.
   */
  virtual void TakePushNotification (void);
  /**
   * Notifies the application that the button has been released.
   */
  virtual void TakeReleaseNotification (void);
  /**
   * Takes a request to send a media message.
   * \param msg The message to send.
   * \returns True, if the message was sent.
   */
  virtual bool TakeSendReq (McpttMediaMsg& msg);
  /**
   * Upgrades the call type.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeCallType (uint8_t callType);
  /**
   * Report an event to the Event trace source.
   * \param callId Call ID for the event.
   * \param reason Description of the event.
   */
  virtual void ReportEvent (uint16_t callId, const char* reason);

  // Events to report
  static constexpr const char* CALL_SELECTED  = "Call selected";

  /**
   * Trace the transmission of a message for a given callId
   * \param callId The call that the message is for
   * \param pkt Packet with serialized header
   * \param headerType TypeId of outer header in packet
   */
  void TraceMessageSend (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

  /**
   * Trace the reception of a message for a given callId
   * \param callId The call that the message is for
   * \param pkt Packet with serialized header
   * \param headerType TypeId of outer header in pack
   */
  void TraceMessageReceive (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

protected:
  /**
   * Disposes of the McpttPttApp instance.
   */
  virtual void DoDispose (void);
  /**
   * Indicates that the floor has been granted to me.
   */
  virtual void FloorGrantedCb (void);
  /**
   * Indicates that a new call has been received.
   * \param callId The ID of the new call.
   */
  virtual void NewCallCb (uint16_t callId);
  /**
   * Receives an off-network call control packet.
   * \param pkt The call control packet that was received.
   * \param from The source address of the packet.
   */
  virtual void ReceiveOffNetworkCallPacket (Ptr<Packet>  pkt, Address from);
  /**
   * Distributes a received off-network call control packet to all off network calls.
   * \param pkt The call control packet that was received.
   * \param msg The message that was received.
   */
  virtual void Receive (Ptr<Packet> pkt, const McpttCallMsg& msg);
  /**
   * Starts the McpttPttApp application.
   */
  virtual void StartApplication (void);
  /**
   * Stops the McpttPttApp application.
   */
  virtual void StopApplication (void);
  /**
   * TracedCallback signature for message transmission or reception events
   * \param [in] app pointer to the MCPTT application involved
   * \param [in] callId Call ID
   * \param [in] pkt The packet sent or received
   * \param [in] headerType TypeId of the first header in the packet
   */
  typedef void (* TxRxTracedCallback) (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  /**
   * TracedCallback signature for event reporting
   * \param [in] userId MCPTT user ID
   * \param [in] callId MCPTT call ID
   * \param [in] selected Indicator of Whether the reported event is for the selected call
   * \param [in] event event description
   */
  typedef void (* EventTracedCallback) (uint32_t, uint16_t callId, const std::string& selected, const char* event);

private:
  static uint16_t s_portNumber; //!< A port number.
  bool m_isRunning; //!< Whether application is running or not
  uint16_t m_callIdAllocator; //!< Counter to allocate call IDs
  std::map<uint16_t, Ptr<McpttChannel> > m_callChannels; //!< Map of call channels
  std::map<Ptr<McpttChannel>, uint32_t> m_callChannelReferenceCount; //!< Reference count for call channels
  std::map<uint16_t, Ptr<McpttCall> > m_calls; //!< The container of all calls.
  std::map<uint16_t, Ptr<McpttCall> > m_onNetworkCalls; //!< The container of on-network calls.
  std::map<uint16_t, Ptr<McpttCall> > m_offNetworkCalls; //!< The container of off-network calls.
  Callback<void> m_floorGrantedCb; //!< The floor granted callback.
  Address m_localAddress; //!< The local IP address.
  Ptr<sip::SipAgent> m_sipAgent; //!< SIP user agent
  Ptr<McpttMediaSrc> m_mediaSrc; //!< The media source.
  Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
  bool m_pushOnStart; //!< The flag that indicates if the pusher should be started when the application starts.
  Ptr<McpttPusher> m_pusher; //!< The object that calls the Pushed() function.
  TracedCallback<Ptr<const Application>, uint16_t, Ptr<const Packet>, const TypeId&> m_rxTrace; //!< The Rx trace
  Ptr<McpttCall> m_selectedCall; //!< The currently selected call.
  Callback<void, Ptr<McpttCall>, Ptr<McpttCall> > m_selectedCallChangeCb; //!< The selected call change CB.
  uint32_t m_userId; //!< The MCPTT user ID.
  TracedCallback<Ptr<const Application>, uint16_t, Ptr<const Packet>, const TypeId&> m_txTrace; //!< The Tx trace.
  TracedCallback<uint32_t, uint16_t, const std::string&, const char* > m_eventTrace; //!< Event trace

public:
  /**
   * Open a call control channel to listen on the provided port, if not already open
   * \param port local port for the socket
   * \param call pointer to McpttCall object
   * \param callType network type for the call
   */
  void OpenCallChannel (uint16_t port, Ptr<McpttCall> call, McpttCall::NetworkCallType callType);
  /**
   * Close the call control channel for the provided local port.  If other calls
   * are using the same channel, the channel will remain open but the call will
   * become disassociated from it.
   * \param port local port for the socket
   * \param call pointer to McpttCall object
   * \param callType network type for the call
   */
  void CloseCallChannel (uint16_t port, Ptr<McpttCall> call, McpttCall::NetworkCallType callType);
  /**
   * Gets the call channel bound to the provided port
   * \param port local port for the socket
   * \returns The channel.
   */
  virtual Ptr<McpttChannel> GetCallChannel (uint16_t port) const;
  /**
   * Gets the container of calls.
   * \returns The container of calls.
   */
  virtual std::map<uint16_t, Ptr<McpttCall> > GetCalls (void) const;
  /**
   * Gets the local address.
   * \returns The local address.
   */
  virtual Address GetLocalAddress (void) const;
  /**
   * Gets the media source.
   * \returns The media source.
   */
  virtual Ptr<McpttMediaSrc> GetMediaSrc (void) const;
  /**
   * Gets the pusher.
   * \returns The pusher.
   */
  virtual Ptr<McpttPusher> GetPusher (void) const;
  /**
   * Gets the currently selected call.
   * \returns The call.
   */
  virtual Ptr<McpttCall> GetSelectedCall (void) const;
  /**
   * Gets the ID of the MCPTT user.
   * \returns The MCPTT user ID.
   */
  virtual uint32_t GetUserId (void) const;
  /**
   * Sets the callback used to notify a floor granted.  Use of this callback
   * is optional, depending on the pusher model.  In real-world operation,
   * the MCPTT user would request talk permission by pushing the button.  If
   * immediately granted, the pusher would continue to push and to generate
   * media.  If queued, the pusher would, upon notification of a queue position,
   * release the button, and wait for a floor grant to arrive through this
   * callback.  When it arrives, this app model expects the user to accept the
   * grant by again starting a push and generating media within the T132
   * timer interval.  Failure to do so will trigger a T132 timeout.
   *
   * If this callback is not set, the McpttPttApp will stand in for the
   * user and automatically accept any floor grants, and the pusher will
   * not be notified except possibly through the event trace.
   *
   * \param floorGrantedCb The callback.
   */
  virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb);
  /**
   * Sets the local IP address.
   * \param localAddress The local IP address.
   */
  virtual void SetLocalAddress (const Address& localAddress);
  /**
   * Sets the media source.
   * \param mediaSrc The media source.
   */
  virtual void SetMediaSrc (Ptr<McpttMediaSrc>  mediaSrc);
  /**
   * Sets the callback used to notify a new call.
   * \param newCallCb The callback
   */
  virtual void SetNewCallCb (const Callback<void, uint16_t>  newCallCb);
  /**
   * Sets the pusher.
   * \param pusher The pusher.
   */
  virtual void SetPusher (Ptr<McpttPusher>  pusher);
  /**
   * Sets the selected call change callback.
   * \param selectedCallChangeCb The callback.
   */
  virtual void SetSelectedCallChangeCb (const Callback<void, Ptr<McpttCall>, Ptr<McpttCall> >  selectedCallChangeCb);
  /**
   * Sets the ID of the MCPTT user.
   * \param userId The MCPTT user ID.
   */
  virtual void SetUserId (uint32_t userId);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PTT_APP_H */

