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

#ifndef INTEL_HTTP_HELPER_H
#define INTEL_HTTP_HELPER_H

#include <ns3/address.h>
#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>

#include <ns3/intel-http-client.h>

namespace ns3 {
namespace psc {

// -- IntelHttpClientHelper --

/**
 * \ingroup psc
 * \brief Creates IntelHttpClients
 * 
 */
class IntelHttpClientHelper
{
public:
  /**
   * Create a new helper used to configure, create, & install
   * one to many IntelHttpClient applications
   * 
   * \param remoteAddress
   * The address of the node bearing the IntelHttpServer
   * that we wish to communicate with
   */
  IntelHttpClientHelper (const Address &remoteAddress);

  /**
   * Sets one of the attributes of underlying IntelHttpClient
   * 
   * \param name
   * Name of attribute to set.
   * \param value
   * Value of the attribute.
   */
  void Set (const std::string &name, const AttributeValue &value);

  /**
   * \brief Install the configured IntelHttpClient on a node
   * 
   * Installs and adds the configured application to a single node
   * 
   * \param node
   * Pointer to the node to install the application on.
   * \return ApplicationContainer
   * A container with the newly added application
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * \brief Install the configured application on many nodes
   * 
   * Installs & adds the configured application to many nodes.
   * 
   * \param nodes
   * The collection of nodes to add the model to
   * 
   * \return ApplicationContainer
   * A container with the new applications.
   * In order in which they were installed
   */
  ApplicationContainer Install (NodeContainer &nodes) const;

  /**
   * \brief Install the configured application on a node with a given name
   * 
   * \param nodeName
   * The name of the node to search for and install the application on
   * 
   * \return ApplicationContainer 
   * A container with the newly installed application
   */
  ApplicationContainer Install (const std::string &nodeName) const;

private:
  ObjectFactory m_factory; //!< object factory for creating the application model

  /**
   * \brief Creates an application, installs and adds that application on the passed node
   * 
   * \param node
   * The node to install the new application on.
   * 
   * \return Ptr<Application>
   * A pointer ot the newly created model
   */
  Ptr<Application> DoInstall (Ptr<Node> node) const;
};

// -- IntelHttpServerHelper --

/**
 * \ingroup psc
 * \brief Creates IntelHttpServers
 */
class IntelHttpServerHelper
{
public:
  /**
   * Create a new helper used to configure, create, & install
   * one to many IntelHttpServer applications
   */
  IntelHttpServerHelper (void);

  /**
   * Sets one of the attributes of underlying IntelHttpServer
   * 
   * \param name
   * Name of attribute to set.
   * \param value
   * Value of the attribute.
   */
  void Set (const std::string &name, const AttributeValue &value);

  /**
   * \brief Install the configured IntelHttpServerHelper on a node
   * 
   * Installs and adds the configured application to a single node
   * 
   * \param node
   * Pointer to the node to install the application on.
   * \return ApplicationContainer
   * A container with the newly added application
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * \brief Install the configured application on many nodes
   * 
   * Installs & adds the configured application to many nodes.
   * 
   * \param nodes
   * The collection of nodes to add the model to
   * 
   * \return ApplicationContainer
   * A container with the new applications.
   * In order in which they were installed
   */
  ApplicationContainer Install (NodeContainer &nodes) const;

  /**
   * \brief Install the configured application on a node with a given name
   * 
   * \param nodeName
   * The name of the node to search for and install the application on
   * 
   * \return ApplicationContainer 
   * A container with the newly installed application
   */
  ApplicationContainer Install (const std::string &nodeName) const;

private:
  ObjectFactory m_factory; //!< object factory for creating the application model

  /**
   * \brief Creates an application, installs and adds that application on the passed node
   * 
   * \param node
   * The node to install the new application on.
   * 
   * \return Ptr<Application>
   * A pointer ot the newly created model
   */
  Ptr<Application> DoInstall (Ptr<Node> node) const;
};

} // namespace psc
} // namespace ns3

#endif // INTEL_HTTP_HELPER_H
