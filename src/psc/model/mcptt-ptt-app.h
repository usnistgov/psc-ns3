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
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

#include "mcptt-call.h"
#include "mcptt-call-msg.h"
#include "mcptt-chan.h"
#include "mcptt-media-sink.h"
#include "mcptt-pushable.h"
#include "mcptt-pusher.h"
#include "mcptt-media-src.h"

namespace ns3 {

/**
 * \defgroup psc  
 *
 * This section documents the API of the ns-3 psc module.The module contains
 * models specific to Public Safety Communications (PSC) research. This
 * includes Mission Critical Push-to-Talk over LTE (MCPTT).
 */
/**
 * \defgroup mcptt
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
class McpttPttApp : public Application, public McpttMediaSink, public McpttPushable
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
  * \param callFac The factory used to create an ns3::McpttCallMachine.
  * \param floorFac The factory used to create an ns3::McpttFloorParticipant.
  * \return pointer to newly created call
  */
 virtual Ptr<McpttCall> CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac);
 /**
  * Creates a call using factory objects and a provided call ID, and add
  * to the container of calls on this application instance.
  * \param callFac The factory used to create an ns3::McpttCallMachine.
  * \param floorFac The factory used to create an ns3::McpttFloorParticipant.
  * \param callId The call ID to use
  * \return pointer to newly created call
  */
 virtual Ptr<McpttCall> CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, uint16_t callId);
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
  * Releases a call.
  */
 virtual void ReleaseCall (void);
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
  * Receives a call control message.
  * \param msg The message that was received.
  */
 virtual void Receive (const McpttCallMsg& msg);
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
  * Sends a call control packet.
  * \param pkt The packet to send.
  */
 virtual void SendCallControlPacket (Ptr<Packet> pkt);
 /**
  * Sends a call control message.
  * \param msg The message to send.
  */
 virtual void Send (const McpttCallMsg& msg);
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
  * Receives a call control packet.
  * \param pkt The call control packet that was received.
  */
 virtual void ReceiveCallPkt (Ptr<Packet>  pkt);
 /**
  * The callback to fire when a message is received.
  * \param call The call that the message is for.
  * \param msg The message that was received.
  */
 virtual void RxCb (Ptr<const McpttCall> call, const McpttMsg& msg);
 /**
  * Starts the McpttPttApp application.
  */
 virtual void StartApplication (void);
 /**
  * Stops the McpttPttApp application.
  */
 virtual void StopApplication (void);
 /**
  * The callback to fire when a message is sent.
  * \param call The call that the message is for.
  * \param msg The message that was sent.
  */
 virtual void TxCb (Ptr<const McpttCall> call, const McpttMsg& msg);
 /**
  * TracedCallback signature for McpttMsg transmission or reception events
  * \param [in] app Ptr<Application>
  * \param [in] callId Call ID
  * \param [in] msg McpttMsg
  */ 
 typedef void (* TxRxTracedCallback) (Ptr<const Application> app, uint16_t callId, const McpttMsg& msg);
private:
 static uint16_t s_portNumber; //!< A port number.
 bool m_isRunning; //!< Whether application is running or not
 uint16_t m_callIdAllocator; //!< Counter to allocate call IDs
 uint16_t m_callPort; //!< The port on which call control messages will flow.
 Ptr<McpttChan> m_callChan; //!< The channel for call control messages.
 std::map<uint16_t, Ptr<McpttCall> > m_calls; //!< The container of calls.
 Callback<void> m_floorGrantedCb; //!< The floor granted callback.
 Address m_localAddress; //!< The local IP address.
 Ptr<McpttMediaSrc> m_mediaSrc; //!< The media source.
 Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
 Address m_peerAddress; //!< The address of the node that the peer application is on.
 bool m_pushOnStart; //!< The flag that indicates if the pusher should be started when the application starts.
 Ptr<McpttPusher> m_pusher; //!< The object that calls the Pushed() function.
 TracedCallback<Ptr<const Application>, uint16_t, const McpttMsg&> m_rxTrace; //!< The Rx trace.
 Ptr<McpttCall> m_selectedCall; //!< The currently selected call.
 Callback<void, Ptr<McpttCall> , Ptr<McpttCall> > m_selectedCallChangeCb; //!< The selected call change CB.
 uint32_t m_userId; //!< The MCPTT user ID.
 TracedCallback<Ptr<const Application>, uint16_t, const McpttMsg&> m_txTrace; //!< The Tx trace.
public:
 /**
  * Gets the channel used for call control messages.
  * \returns The channel.
  */
 virtual Ptr<McpttChan> GetCallChan (void) const;
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
  * Sets the channel used for call control messages.
  * \param callChan The channel.
  */
 virtual void SetCallChan (Ptr<McpttChan>  callChan);
 /**
  * Sets the container of calls.
  * \param calls The container of calls.
  */
 virtual void SetCalls (const std::map<uint16_t, Ptr<McpttCall> >  calls);
 /**
  * Sets the callback used to notify a floor granted.
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
 virtual void SetSelectedCallChangeCb (const Callback<void, Ptr<McpttCall> , Ptr<McpttCall> >  selectedCallChangeCb);
 /**
  * Sets the ID of the MCPTT user.
  * \param userId The MCPTT user ID.
  */
 virtual void SetUserId (uint32_t userId);
};

} // namespace ns3

#endif /* MCPTT_PTT_APP_H */
