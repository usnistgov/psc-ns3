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

#ifndef INTEL_HTTP_CLIENT_H
#define INTEL_HTTP_CLIENT_H

#include <ns3/address.h>
#include <ns3/application.h>
#include <ns3/event-id.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/packet.h>
#include <ns3/random-variable-stream.h>
#include <ns3/socket.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "intel-http-header.h"

namespace ns3 {
namespace psc {
/**
 * \ingroup psc
 * \brief HTTP client based off of the Intel HTTP model
 *
 * HTTP client based on the Intel model.
 * Intended to communicate with IntelHttpServer
 *
 * See the model documentation for details about this class
 */
class IntelHttpClient : public Application
{
public:
  /**
   * Signature for a callback triggered when an object on a page was
   * cached or not
   *
   * \param currentObject The sequence number of the object for
   * for the cache event
   */
  typedef void (*CacheCallback) (uint16_t currentObject);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Default constructor.
   */
  IntelHttpClient ();

  /**
   * Default destructor.
   */
  ~IntelHttpClient () override;

  /**
   * Setter for the address of the server for this client.
   *
   * \param ip The address at which the server will be listening.
   * \param port The port at which the server will be listening.
   */
  void SetRemote (Address ip, uint16_t port);

protected:
  /**
   * Trace called right before a Request of any type is transmitted.
   */
  TracedCallback<Ptr<const Packet> > m_txTrace;

  /**
   * Trace called before a Request for a main object is transmitted.
   */
  TracedCallback<Ptr<const Packet> > m_txRequestMainObjectTrace;

  /**
   * Trace called before a Request for an embedded object is transmitted.
   */
  TracedCallback<Ptr<const Packet> > m_txRequestEmbeddedObjectTrace;

  /**
   * Trace called after a Response of any type is received.
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

  /**
   * Trace called after a Response containing a main object is received
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxMainObjectTrace;

  /**
   * Trace called after a Response containing an embedded object is received
   */
  TracedCallback<Ptr<const Packet>, const Address &> m_rxEmbeddedObjectTrace;

  /**
   * Trace called when an object was cached
   */
  TracedCallback<uint16_t> m_cacheHit;

  /**
   * Trace called when an object was not cached
   */
  TracedCallback<uint16_t> m_cacheMiss;

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
   * Method that actually sends a request for an HTML page to the server.
   */
  void SendRequestPage (void);

  /**
   * Method that processes a received packet.
   *
   * \param socket Socket that received the packet.
   */
  void HandleRead (Ptr<Socket> socket);

  /**
   * Process a received application packet
   *
   * \param from The address the packet was sent from
   * \param recvHttpHeader The received HTTP header
   */
  void ProcessRecvPkt (const Address &from, const IntelHttpHeader &recvHttpHeader);

  /**
   * Address at which to send the packets to.
   */
  Address m_peerAddress;
  /**
   * Port at which to send the packets to.
   */
  uint16_t m_peerPort;
  /**
   * Event ID for tracking the sending of packets.
   */
  EventId m_sendEvent;
  /**
   * Event ID for tracking the processing time for parsing packets.
   */
  EventId m_parseEvent;
  /**
   * Socket to send and receive data.
   */
  Ptr<Socket> m_socket;

  /**
   * Packet where we "accumulate" the bytes that we receive in
   * TCP segments until we have a full packet
   */
  Ptr<Packet> m_partialPacket;

  /**
   * Threshold to decide if an embedded object is cached or not
   */
  double m_cacheThreshold;

  /**
   * Number of embedded objects in a page left to download
   */
  uint16_t m_objectsLeft;

  /**
   * Low bound for the parse time, in seconds
   */
  double m_parseTimeLowBound;

  /**
   * High bound for the parse time, in seconds
   */
  double m_parseTimeHighBound;

  /**
   * Low bound for the reading time, in seconds
   */
  double m_readTimeLowBound;

  /**
   * High bound for the reading time, in seconds
   */
  double m_readTimeHighBound;

  /**
   * Size of the send and receive socket buffers
   */
  uint32_t m_socketBufferSize;

  /**
   * Random variable stream to generate the size of the requests
   */
  Ptr<RandomVariableStream> m_requestSizeRvs;
  /**
   * Random variable stream to generate the time spent in parsing
   */
  Ptr<RandomVariableStream> m_parseTimeRvs;
  /**
   * Random variable stream to decide if an embedded object is cached or not
   */
  Ptr<RandomVariableStream> m_cacheRvs;
  /**
   * Random variable stream to generate the time spent reading
   */
  Ptr<RandomVariableStream> m_readTimeRvs;

}; //class NistIncidentAppHttpClient

} // namespace psc
} // namespace ns3

#endif // INTEL_HTTP_CLIENT_H
