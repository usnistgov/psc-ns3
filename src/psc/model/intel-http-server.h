/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
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

#ifndef INTEL_HTTP_SERVER_H
#define INTEL_HTTP_SERVER_H

#include <ns3/address.h>
#include <ns3/application.h>
#include <ns3/packet.h>
#include <ns3/random-variable-stream.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "intel-http-header.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup psc
 * \brief HTTP client based off of the Intel HTTP model
 *
 * HTTP server based on the Intel HTTP model.
 * Intended to communicate with IntelHttpClient
 *
 * See the model documentation for details about this class
 */
class IntelHttpServer : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Default constructor.
   */
  IntelHttpServer ();
  /**
   * Default destructor.
   */
  ~IntelHttpServer () override;

protected:
  /**
   * Trace called right before a Response of any type is transmitted
   */
  TracedCallback<Ptr<const Packet> > m_txTrace;

  /**
   * Trace called before a packet containing a main object is transmitted
   */
  TracedCallback<Ptr<const Packet> > m_txMainObjectTrace;

  /**
   * Trace called before a packet containing an embedded object is transmitted
   */
  TracedCallback<Ptr<const Packet> > m_txEmbeddedObjectTrace;

  /**
   * Trace called after a Request is received
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

  /**
   * Trace called when a packet containing a request for a page is received
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxMainObjectRequestTrace;

  /**
   * Trace called when a packet containing a request for an embedded object is received
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxEmbeddedObjectRequestTrace;

  /**
   * Method to orderly clean up the instance before destroying it.
   */
  void DoDispose (void) override;

private:
  /**
   * Method that starts the operation of this application.
   */
  void StartApplication (void) override;
  /**
   * Method that stops the operation of this application.
   */
  void StopApplication (void) override;

  /**
   * Method that processes a received packet. Note that, as we are using TCP, this packet
   * may not be an application packet, but a TCP segment. When we get a full application
   * packet, it will be passed to ProcessRecvPacket.
   *
   * \param socket Socket that received the packet.
   */
  void HandleRead (Ptr<Socket> socket);
  /**
   * Process a received application packet
   *
   * \param socket The socket the packet was received from.
   * \param from The address the packet was sent from
   * \param recvHttpHeader The received HTTP header
   */
  void ProcessRecvPkt (Ptr<Socket> socket, const Address &from,
                       const IntelHttpHeader &recvHttpHeader);

  /**
   * Method that processes a new connection from a client.
   *
   * \param socket Socket that received the connection.
   * \param from Client address.
   */
  void HandleAccept (Ptr<Socket> socket, const Address &from);
  /**
   * Method to handle the scheduling of sending event. It just sends and traces the response packet, but being a separate methods allows us to call Simulator::Schedule () on it.
   *
   * \param socket The socket to send the packet through.
   * \param responsePkt The response packet to send.
   * \param toAddr Address to send the packet to
   */
  void DelayedSend (Ptr<Socket> socket, Ptr<Packet> responsePkt, const Address &toAddr);

  /**
   * Port to listen at.
   */
  uint16_t m_port;
  /**
   * Socket to listen to.
   */
  Ptr<Socket> m_socket;

  /**
   * Packet where we "accumulate" the bytes that we receive in
   * TCP segments until we have a full packet
   */
  Ptr<Packet> m_partialPacket;

  /**
   * Size of the send and receive socket buffers
   */
  uint32_t m_socketBufferSize;

  /**
   * Low bound for the html size
   */
  uint32_t m_htmlSizeLowBound;

  /**
   * High bound for the html size
   */
  uint32_t m_htmlSizeHighBound;

  /**
   * Low bound for the number of embedded objects
   */
  uint32_t m_embeddedObjectAmountLowBound;

  /**
   * High bound for the number of embedded objects
   */
  uint32_t m_embeddedObjectAmountHighBound;

  /**
   * Low bound for the size of embedded objects
   */
  uint32_t m_embeddedObjectSizeLowBound;

  /**
   * High bound for the size of embedded objects
   */
  uint32_t m_embeddedObjectSizeHighBound;

  /**
   * Low bound for the AIT of embedded objects, in seconds
   */
  uint32_t m_embeddedObjectIatLowBound;

  /**
   * High bound for the IAT of embedded objects, in seconds
   */
  uint32_t m_embeddedObjectIatHighBound;

  /**
   * Random variable stream to generate the size of the HTML file
   */
  Ptr<RandomVariableStream> m_htmlSizeRvs;
  /**
   * Random variable stream to generate the amount of embedded objects in an HTML file
   */
  Ptr<RandomVariableStream> m_embeddedObjectAmountRvs;
  /**
   * Random variable stream to generate the IAT to wait for embedded objects
   */
  Ptr<RandomVariableStream> m_embeddedObjectIatRvs;
  /**
   * Random variable stream to generate the size of the embedded objects
   */
  Ptr<RandomVariableStream> m_embeddedObjectSizeRvs;

}; // class IntelHttpServer

} // namespace psc
} // namespace ns3

#endif /* INTEL_HTTP_SERVER_H */
