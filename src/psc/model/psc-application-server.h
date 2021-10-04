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

#ifndef PSC_APPLICATION_SERVER_H
#define PSC_APPLICATION_SERVER_H

#include "ns3/address.h"
#include "ns3/psc-application.h"
#include "ns3/socket.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 * \brief Server application for incident applications.
 *
 * Server application for the incident request / response applications.
 */
class PscApplicationServer : public PscApplication
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
  PscApplicationServer ();
  /**
   * Default destructor.
   */
  virtual ~PscApplicationServer ();

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
   * Method that processes received data. Note that if we are using TCP, this
   * data may not be an application packet, but a byte stream. When we get
   * a full application packet, it will RecvPacket the method that will
   * process it.
   *
   * \param socket Socket that received the data.
   */
  void HandleRead (Ptr<ns3::Socket> socket);
  /**
   * Process a received application packet
   *
   * \param socket The socket the packet was received from.
   * \param from The address the packet was sent from
   */
  void ProcessRecvPkt (Ptr<ns3::Socket> socket, const Address& from);

  /**
   * Method that processes a new connection from a client.
   *
   * \param socket Socket that received the connection.
   * \param from Client address.
   */
  void HandleAccept (Ptr<ns3::Socket> socket, const Address& from);
  /**
   * Method to handle the scheduling of sending event. It just sends and traces the response packet, but being a separate methods allows us to call Simulator::Schedule () on it.
   *
   * \param socket The socket to send the packet through.
   * \param toAddr Address to send the packet to
   */
  void DelayedSend (Ptr<ns3::Socket> socket, const Address& toAddr);

  /**
   * Tries to send data in the transmit buffer.
   *
   * \param socket The socket to try to send data with.
   * \param txSpace The amount of space left in the socket buffer.
   */
  void TrySend (Ptr<ns3::Socket> socket, uint32_t txSpace);

  /**
   * Port to listen at.
   */
  uint16_t    m_port;
  /**
   * Address to listen at.
   */
  Address     m_local;
  /**
   * Socket to listen to.
   */
  Ptr<ns3::Socket> m_socket;
  /**
   * Socket to listen to for IPv6 packets.
   */
  Ptr<ns3::Socket> m_socket6;
  /**
   * Size of the packets to send.
   */
  uint32_t    m_packetSize;
  /**
   * Minimum interval between sent response packets.
   */
  Time m_minTimeBetweenRsps;
  /**
   * Time at which the last response was sent
   */
  Time m_timeLastSendScheduled;
  /**
   * Flag to indicate if this server is a sink, or if it will send a response to the
   * packets received
   */
  bool m_isSink;
  /**
   * Packet where we "accumulate" the bytes that we receive.
   */
  Ptr<ns3::Packet> m_rxBuffer;
  /**
   * Packet where we "accumulate" the bytes that we send.
   */
  Ptr<ns3::Packet> m_txBuffer;
  /**
   * The address to send packets to.
   */
  Address m_toAddr;
}; // class PscApplicationServer

} // namespace psc
} // namespace ns3

#endif // PSC_APPLICATION_SERVER_H
