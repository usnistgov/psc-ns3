
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

#ifndef MCPTT_SERVER_HELPER_H
#define MCPTT_SERVER_HELPER_H

#include <string>

#include <ns3/application-container.h>
#include <ns3/attribute.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

namespace ns3 {
namespace psc {

class McpttServerApp;
/**
 * \ingroup psc
 * \brief A helper for creating, configuring, and installing MCPTT apps.
 */
class McpttServerHelper
{
public:
  /**
   * \brief Creates an instance of the McpttServerHelper class.
   */
  McpttServerHelper (void);
  /**
   * \brief Destructor of the McpttServerHelper class.
   */
  virtual ~McpttServerHelper (void);
  /**
   * \brief Install an ns3::psc::McpttServerApp on the node.
   *
   * \param node The node on which an McpttServerApp will be installed.
   * \returns Ptr to the McpttServerApp app
   */
  Ptr<McpttServerApp> Install (Ptr<Node> node);
  /**
   * \brief Install an ns3::psc::McpttServerApp on the node.
   *
   * \param nodeName The node on which an McpttServerApp will be installed.
   * \returns Ptr to the McpttServerApp app
   */
  Ptr<McpttServerApp> Install (const std::string& nodeName);
  /**
   * Configures the floor control server app.
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   */
  void SetServerAttributes (
    std::string n0 = "", const AttributeValue& v0 = EmptyAttributeValue (),
    std::string n1 = "", const AttributeValue& v1 = EmptyAttributeValue (),
    std::string n2 = "", const AttributeValue& v2 = EmptyAttributeValue (),
    std::string n3 = "", const AttributeValue& v3 = EmptyAttributeValue (),
    std::string n4 = "", const AttributeValue& v4 = EmptyAttributeValue (),
    std::string n5 = "", const AttributeValue& v5 = EmptyAttributeValue (),
    std::string n6 = "", const AttributeValue& v6 = EmptyAttributeValue (),
    std::string n7 = "", const AttributeValue& v7 = EmptyAttributeValue ());

protected:
  /**
   * \brief Install an ns3::psc::McpttServerApp on the node.
   *
   * \param node The node on which an McpttServerApp will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<McpttServerApp> InstallPriv (Ptr<Node> node);

private:
  ObjectFactory m_serverFactory; //!< The MCPTT server object factory.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_SERVER_HELPER */

