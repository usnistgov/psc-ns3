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

#include <vector>

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
 * This class is the main component of the MCPTT model, and it is what ties all
 * of the MCPTT components together. The application is the object used to
 * create and manage calls, as well as definining the functions for expected
 * user actions like starting an emergency alert or chaning the call type. When
 * creating calls the user must give two object factories, one factory for
 * creating the approprate call machine and another for the appropriate floor
 * machine depending on which call control and floor control protocols are
 * desired for a call. Once created, the user must use the "SelectCall"
 * function to select a call based on the call's index that will then link the
 * application to receive notifications and to have user actions applied to a
 * particular call.
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
  * Creates a call.
  * \param callFac The factory used to create an ns3::McpttCallMachine.
  * \param floorFac The factory used to create an ns3::McpttFloorMachine.
  */
 virtual void CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac);
 /**
  * Downgrades the call type.
  */
 virtual void DowngradeCallType (void);
 /**
  * Gets the type ID fo this McpttPttApp instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
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
  * Indicates whether or not the PTT button is pushed.
  * \returns True, if the PTT button is pushed.
  */
 virtual bool IsPushed (void) const;
 /**
  * The push-to-talk button pressed event handler.
  */
 virtual void Pushed (void);
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
  * The push-to-talk button released event handler.
  */
 virtual void Released (void);
 /**
  * Receives a call control message.
  * \param msg The message that was received.
  */
 virtual void Receive (const McpttCallMsg& msg);
 /**
  * Selects a call.
  * \param callIdx The index of the call.
  */
 virtual void SelectCall (uint32_t callIdx);
 /**
  * Selects the last call in the list of calls.
  */
 virtual void SelectLastCall (void);
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
 virtual void RxCb (const McpttCall& call, const McpttMsg& msg);
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
 virtual void TxCb (const McpttCall& call, const McpttMsg& msg);
private:
 static uint16_t s_portNumber; //!< A port number.
 uint16_t m_callPort; //!< The port on which call control messages will flow.
 Ptr<McpttChan> m_callChan; //!< The channel for call control messages.
 std::vector<Ptr<McpttCall> > m_calls; //!< The collection of calls.
 Callback<void> m_floorGrantedCb; //!< The floor granted callback.
 Ipv4Address m_localAddress; //!< The local IP address.
 Ptr<McpttMediaSrc> m_mediaSrc; //!< The media source.
 Callback<void, uint16_t> m_newCallCb; //!< The new call callback.
 Ipv4Address m_peerAddress; //!< The address of the node that the peer application is on.
 Ptr<McpttPusher> m_pusher; //!< The object that randomly calls the Pushed() function.
 bool m_pushOnStart; //!< The flag that indicates if the pusher should be started when the application starts.
 TracedCallback<const McpttPttApp&, const McpttMsg&> m_rxTrace; //!< The Tx trace.
 Ptr<McpttCall> m_selectedCall; //!< The currenctly selected call.
 Callback<void, Ptr<McpttCall> , Ptr<McpttCall> > m_selectedCallChangeCb; //!< The selected call change CB.
 uint32_t m_userId; //!< The MCPTT user ID.
 TracedCallback<const McpttPttApp&, const McpttMsg&> m_txTrace; //!< The Rx trace.
protected:
 /**
  * Sets the local IP address.
  * \param localAddress The local IP address.
  */
 virtual void SetLocalAddress (const Ipv4Address& localAddress);
public:
 /**
  * Gets the channel used for call control messages.
  * \returns The channel.
  */
 virtual Ptr<McpttChan> GetCallChan (void) const;
 /**
  * Gets the collection of calls.
  * \returns The collection of calls.
  */
 virtual std::vector<Ptr<McpttCall> > GetCalls (void) const;
 /**
  * Gets the local address.
  * \returns The local address.
  */
 virtual Ipv4Address GetLocalAddress (void) const;
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
  * Sets the collection of calls.
  * \param calls The collection of calls.
  */
 virtual void SetCalls (const std::vector<Ptr<McpttCall> >  calls);
 /**
  * Sets the callback used to notify a floor granted.
  * \param floorGrantedCb The callback.
  */
 virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb);
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
  * Sets the currently selected call.
  * \param selectedCall The call.
  */
 virtual void SetSelectedCall (Ptr<McpttCall>  selectedCall);
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

