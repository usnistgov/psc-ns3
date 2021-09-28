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

#ifndef MCPTT_SERVER_APP_H
#define MCPTT_SERVER_APP_H

#include <vector>
#include <map>

#include <ns3/application.h>
#include <ns3/header.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>
#include <ns3/sip-proxy.h>
#include <ns3/sip-header.h>

namespace ns3 {

namespace psc {

class McpttServerCall;
class McpttCallMsg;
class McpttChannel;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT server application.  It holds state for
 * a number of MCPTT calls.
 */
class McpttServerApp : public Application
{
public:
  /**
   * Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttServerApp class.
   */
  McpttServerApp (void);
  /**
   * The destructor of the McpttServerApp class.
   */
  virtual ~McpttServerApp (void);
  /**
   * Get the SipProxy associated with this application.
   * \return pointer to SipProxy
   */
  Ptr<sip::SipProxy> GetSipProxy (void) const;
  /**
   * Return a new call ID value
   * \return new call ID value
   */
  uint16_t AllocateCallId (void);
  /**
   * Add a call definition to the server
   * \param call a pointer to the call object
   */
  void AddCall (Ptr<McpttServerCall> call);
  /**
   * Get a call definition from the server
   * \param callId The ID of the call
   */
  Ptr<McpttServerCall> GetCall (uint16_t callId);
  /**
   * Sends a call control packet.
   * \param pkt The packet to send.
   * \param toAddr The address to send to
   */
  virtual void SendCallControlPacket (Ptr<Packet> pkt, const Address& toAddr);
  /**
   * Gets the address of the host.
   * \returns The address.
   */
  Address GetLocalAddress (void) const;
  /**
   * Sets the address of the host.
   * \param localAddress The address.
   */
  void SetLocalAddress (const Address& localAddress);
  /**
   * Indicates whether or not the application is running
   * \returns True, if the application is running.
   */
  virtual bool IsRunning (void) const;
  /**
   * Trace the transmission of a message for a given callId
   * \param callId The call that the message is for
   * \param pkt Packet with serialized header
   * \param headerType TypeId of outer header in pack
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
   * Disposes of the McpttServerApp instance.
   */
  virtual void DoDispose (void);
  /**
   * Starts the McpttServerApp application.
   */
  virtual void StartApplication (void);
  /**
   * Stops the McpttServerApp application.
   */
  virtual void StopApplication (void);

  /**
   * TracedCallback signature for McpttMsg transmission or reception events
   * \param [in] app Ptr<Application>
   * \param [in] callId Call ID
   * \param [in] pkt The packet sent or received
   * \param [in] headerType TypeId of the first header in the packet
   */
  typedef void (* TxRxTracedCallback) (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  static uint16_t s_callId; //!< Call ID counter
  std::map<uint16_t, Ptr<McpttServerCall> > m_calls; //!< Call container keyed by callId
  Address m_localAddress; //!< The local IP address.
  Address m_peerAddress; //!< The peer IP address.
  uint16_t m_callPort; //!< The port on which call control messages will flow.
  Ptr<McpttChannel> m_callChannel; //!< The channel for call control messages.
  Ptr<sip::SipProxy> m_sipProxy; //!< The SIP proxy agent
  TracedCallback<Ptr<const Application>, uint16_t, Ptr<const Packet>, const TypeId&> m_rxTrace; //!< The Rx trace.
  TracedCallback<Ptr<const Application>, uint16_t, Ptr<const Packet>, const TypeId&> m_txTrace; //!< The Tx trace.
  bool m_isRunning; //!< Flag to mark if the application is running
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_SERVER_APP_H */

