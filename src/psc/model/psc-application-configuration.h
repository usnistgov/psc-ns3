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

#ifndef PSC_APPLICATION_CONFIGURATION_H
#define PSC_APPLICATION_CONFIGURATION_H

#include "ns3/random-variable-stream.h"
#include "ns3/psc-application.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Class that defines a set of configuration parameters for applications
 * used in incident modeling. This class stores the amount of instances of
 * each application, the socket type (TCP or UDP), the start and stop
 * RandomVariableStream and the default QCI (for LTE use).
 */
class PscApplicationConfiguration : public Object
{
public:
  /**
   * Constructor that takes all the parameters to store for an application.
   * \param name The application name
   * \param sType Socket type (TCP or UDP), as an string with the ns3 path to the appropriate socket factory.
   * \param basePort The initial port for deploying the application
   */
  PscApplicationConfiguration (std::string name,
                               TypeId sType,
                               uint16_t basePort);

  /**
   * Default destructor
   */
  ~PscApplicationConfiguration ();

  /**
   * Set the application pattern
   * \param pktsPerSessionRandomVariable RandomVariableStream that will tell the number of packets per session.
   * \param pktsIntervalRandomVariable RandomVariableStream that will tell the interval between packets in a session.
   * \param sessionIntervalRandomVariable RandomVariableStream that will tell the interval between session.
   * \param pSizeClient Size of the packets the client sends.
   * \param pSizeServer Size of the packets the server sends.
   */
  void SetApplicationPattern (
    Ptr<RandomVariableStream> pktsPerSessionRandomVariable,
    Ptr<RandomVariableStream> pktsIntervalRandomVariable,
    Ptr<RandomVariableStream> sessionIntervalRandomVariable,
    uint32_t pSizeClient, uint32_t pSizeServer = 0);

  /**
   * Get the name of the application
   * @return the name of the application
   */
  std::string GetName (void) const;

  /**
   * Get the socket type to use, as a string with the ns3 path to the appropriate socket factory.
   *
   * \return The socket type to use, as a string with the ns3 path to the appropriate socket factory.
   */
  TypeId GetSocketType (void) const;
  
  /**
   * Getter for the next port number to use
   *
   * \return The port number to use for the next instantiation of the application
   */
  uint16_t GetNextPort (void);
  
  /**
   * Getter for the packet size for the client.
   *
   * \return The packet size for the client.
   */
  uint32_t GetPacketSizeClient (void);
  
  /**
   * Getter for the packet size for the server.
   *
   * \return The packet size for the server.
   */
  uint32_t GetPacketSizeServer (void);
  
  /**
   * Getter for the RandomVariableStream that generates the number of packets per session.
   *
   * \return The RandomVariableStream that generates the number of packets per session.
   */
  Ptr<RandomVariableStream> GetPacketsPerSession (void);
  
  /**
   * Getter for the RandomVariableStream that generates the interval between packets.
   *
   * \return The RandomVariableStream that generates the interval between packets.
   */
  Ptr<RandomVariableStream> GetPacketInterval (void);
  
  /**
   * Getter for the RandomVariableStream that generates the interval between session.
   *
   * \return The RandomVariableStream that generates the interval between session.
   */
  Ptr<RandomVariableStream> GetSessionInterval (void);

  /**
   * Increment the number of instances created and return count before increment.
   * 
   * \return The number of instances before it was incremented
   */
  uint32_t IncrementInstances (void);
  
private:
  /**
   * The name of the application
   */
  std::string m_name;
  /**
   * Socket type to use, as a string with the ns3 path to the appropriate socket factory.
   */
  TypeId m_socketType;
  /**
 * Packet size for the client, in bytes.
 */
  uint32_t m_packetSizeClient;
  /**
   * Packet size for the server, in bytes.
   */
  uint32_t m_packetSizeServer;
  /**
   * RandomStreamVariable that generates the number of packets per session.
   */
  Ptr<RandomVariableStream> m_packetsPerSessionRandomVariable;
  /**
   * RandomStreamVariable that generates the interval between packets, in seconds.
   */
  Ptr<RandomVariableStream> m_packetIntervalRandomVariable;
  /**
   * RandomStreamVariable that generates the interval between session, in seconds.
   */
  Ptr<RandomVariableStream> m_sessionIntervalRandomVariable;
  /**
   * Port for instantiating the application
   */
  uint16_t m_port;
  /**
   * Counter for the number of instances created
   */
  uint32_t m_instances;

}; // class PscApplicationConfiguration

} // namespace psc
} // namespace ns3

#endif // PSC_INCIDENT_APPLICATION_CONFIGURATION_INFO_H
