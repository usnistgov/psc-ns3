/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef PSC_APPLICATION_CLIENT_H
#define PSC_APPLICATION_CLIENT_H

#include "ns3/random-variable-stream.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/seq-ts-size-header.h"

#include "ns3/psc-application.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 * \brief Client application for incident scenarios.
 *
 * Client application for the incident scenarios.
 */
class PscApplicationClient : public PscApplication
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
  PscApplicationClient ();

  /**
   * Default destructor.
   */
  virtual ~PscApplicationClient ();

  /**
   * Setter for the address of the server for this client.
   *
   * \param ip The address at which the server will be listening.
   * \param port The port at which the server will be listening.
   */
  void SetRemote (Address ip, uint16_t port);
  /**
   * Setter for the RandomStreamVariable for the packets per session.
   *
   * \param pktsPerSession RandomStreamVariable to use to get the packets per session.
   */
  void SetPacketsPerSession (Ptr<RandomVariableStream> pktsPerSession);
  /**
   * Setter for the RandomStreamVariable for the packet interval in a session.
   *
   * \param pktInterval RandomStreamVariable to use to get the interval between packets in a session.
   */
  void SetPacketInterval (Ptr<RandomVariableStream> pktInterval);
  /**
   * Setter for the RandomStreamVariable for the interval between sessions.
   *
   * \param sessionInterval RandomStreamVariable to use to get the interval between sessions.
   */
  void SetSessionInterval (Ptr<RandomVariableStream> sessionInterval);

  /**
   * Stop the application. This cancels the stop event at the original stop time
   * and invokes the StopApplication method
   */
  virtual void StopNow ();

protected:
  /**
   * Method to orderly clean up the instance before destroying it.
   */
  virtual void DoDispose (void);

private:
  /**
   * Method that starts the operation of this application.
   */
  virtual void StartApplication (void);
  /**
   * Method that stops the operation of this application.
   */
  virtual void StopApplication (void);

  /**
   * Method that schedules the transmission of a packet at a given time.
   *
   * \param t The time at which to schedule the transmission.
   */
  void ScheduleTransmit (Time t);

  /**
   * Method that actually sends a packet to the server.
   */
  void Send (void);

  /**
   * Method that processes a received packet.
   *
   * \param socket Socket that received the packet.
   */
  void HandleRead (Ptr<ns3::Socket> socket);

  /**
   * Tries to send data in the transmit buffer.
   *
   * \param socket The socket to try to send data with.
   * \param txSpace The amount of space left in the socket buffer.
   */
  void TrySend (Ptr<ns3::Socket> socket, uint32_t txSpace);

  /**
   * Counter for the packets pending to transmit in the current session.
   */
  uint32_t m_packetsLeftThisSession;
  /**
   * RandomStreamVariable for the number of packets per session.
   */
  Ptr<RandomVariableStream> m_packetsPerSessionRandomVariable;
  /**
   * RandomStreamVariable for the interval between packets.
   */
  Ptr<RandomVariableStream> m_packetIntervalRandomVariable;
  /**
   * RandomStreamVariable for the interval between sessions.
   */
  Ptr<RandomVariableStream> m_sessionIntervalRandomVariable;

  /**
   * Size of the packets to transmit.
   */
  uint32_t m_packetSize;
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
   * Socket to send and receive data.
   */
  Ptr<ns3::Socket> m_socket;
  /**
   * Packet where we "accumulate" the bytes that we receive.
   */
  Ptr<ns3::Packet> m_rxBuffer;
  /**
   * Packet where we "accumulate" the bytes that we send.
   */
  Ptr<ns3::Packet> m_txBuffer;
}; //class PscApplicationClient

} // namespace psc
} // namespace ns3

#endif // PSC_APPLICATION_CLIENT_H
