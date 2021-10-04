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

#ifndef PSC_APPLICATION_HELPER_H
#define PSC_APPLICATION_HELPER_H

#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/eps-bearer.h"
#include "ns3/lte-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"

#include "ns3/psc-application.h"
#include "ns3/psc-application-configuration.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * \brief Class that helps to configure and deploy applications for incident scenarios.
 *
 * This helper initializes a list with the applications defined in the Televate
 * report, provides methods to deploy them in nodes (including bearer setup for LTE nodes),
 * and provides trace sinks for packets sent and received.
 *
 * For the purposes of the documentation of this class, we will refer to the pair of
 * client and server applications as a single application, unless otherwise especified.
 *
 * Note that the application definitions (i.e. throughput, packet interval) is hardcoded and
 * cannot be modified from the scenario using this helper. They can, however, be modified using
 * the individual application accessors for the variables.
 *
 * Only one instance of this class is needed for the whole simulation, as the trace sink uses
 * static file writers (default trace file: 'incidentAppTrace.txt').
 */
class PscApplicationHelper : public Object
{
public:
  /**
   * Enumeration with the different types of traces we may request for a node.
   */
  enum TraceType_t
  {
    TX,                 ///< Trace the transmission of packets
    RX                 ///< Trace the reception of packets
  };

  /**
   * Enumeration used to tell the helper which of the nodes in the application is
   * an LTE node.
   */
  enum LteNode_t
  {
    CLIENT,               ///< The client node is an LTE node.
    SERVER,               ///< The server node is an LTE node.
    BOTH                  ///< Both the client and server nodes are LTE nodes
  };

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Default constructor, where InitAppList is invoked.
   */
  PscApplicationHelper ();
  /**
   * Default destructor, where all the smart pointers are set to null.
   */
  ~PscApplicationHelper ();

  /**
   * \brief Method that installs the specified application in the provided nodes,
   * with the specified start and stop times.
   *
   * This method installs an application in the nodes provided with the
   * parameters stored in the PscApplication::m_appConfigList .
   *
   * After configuring the packet size and remote address attributes in the client and server
   * ObjectFactory, this method invokes PscApplication::InstallClientPriv and
   * PscApplication::InstallServerPriv.
   *
   * \param config The application configuration
   * \param client The node where to deploy the client application (assuming REQs go UL; if not, client and server are reversed from these names).
   * \param server The node where to deploy the server application (assuming REQs go UL; if not, client and server are reversed from these names).
   * \param serverAddress The address to use by the client when initiating the connection.
   * \param serverResponse Indicates if the server should send a response or not.
   * \param startTime Time at which the client application will start (the server application starts one second earlier to make sure it is listening and ready when the first packet arrives).
   * \param stopTime Time at which the client and server applications will stop.
   *
   * \return An ApplicationContainer with the client application and the server application, stored in that order.
   */
  ApplicationContainer Install (Ptr<PscApplicationConfiguration> config, Ptr<Node> client, Ptr<Node> server,
                                Ipv6Address serverAddress, bool serverResponse,
                                Time startTime, Time stopTime);

private:
  /**
   * Method that creates a client application and deploys it in the specified node.
   *
   * We need to pass the type of application to configure the members that are instances of RandomStreamVariable (we get those parameters from m_appConfigList and 'name' serves as the index of that array).
   *
   * \param config Configuration of the application to deploy.
   * \param client Node where to deploy the application.
   * \return A pointer to the application created and deployed.
   */
  Ptr<Application> InstallClientPriv (Ptr<PscApplicationConfiguration> config, Ptr<Node> client);

  /**
   * Method that creates a server application and deploys it in the specified node.
   *
   * \param server Node where to deploy the application.
   * \return A pointer to the application created and deployed.
   */
  Ptr<Application> InstallServerPriv (Ptr<Node> server);

  /**
   * Object factory for the client applications.
   */
  ObjectFactory m_clientFactory;
  /**
   * Object factory for the server applications.
   */
  ObjectFactory m_serverFactory;

  /**
   * Container with all the client applications created through this helper.
   * Used to trace the demand over time.
   */
  ApplicationContainer m_incidentClientAppsCreated;

  /**
   * Temporary storage of the RandomStreamVariable for the number of packets in a session.
   */
  Ptr<RandomVariableStream> m_tmpPktsSession;
  /**
   * Temporary storage of the RandomStreamVariable for the interval between packets.
   */
  Ptr<RandomVariableStream> m_tmpPktInterval;
  /**
   * Temporary storage of the RandomStreamVariable for the interval between sessions.
   */
  Ptr<RandomVariableStream> m_tmpSessionInterval;

  /**
   * Minimum interval between sever response packets.
   */
  Time m_minTimeBetweenServerRsps;

}; //class PscApplicationHelper

} // namespace psc

} // namespace ns3

#endif // PSC_APPLICATION_HELPER_H
