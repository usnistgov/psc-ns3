
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

#ifndef MCPTT_ON_NETWORK_FLOOR_CONTROL_HELPER_H
#define MCPTT_ON_NETWORK_FLOOR_CONTROL_HELPER_H

#include <string>

#include <ns3/application-container.h>
#include <ns3/attribute.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

namespace ns3 {

/**
 * \ingroup psc
 * \brief A helper for creating, configuring, and installing MCPTT apps.
 */
class McpttOnNetworkFloorControlHelper
{
public:
 /**
  * \brief Creates an instance of the McpttOnNetworkFloorControlHelper class.
  */
 McpttOnNetworkFloorControlHelper (void);
 /**
  * \brief Destructor of the McpttOnNetworkFloorControlHelper class.
  */
 virtual ~McpttOnNetworkFloorControlHelper (void);
 /**
  * \brief Install an ns3::McpttPttApp on each node of the input container.
  *
  * \param c NodeContainer of the set of nodes on which an McpttPttApp 
  * will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const NodeContainer& c);
 /**
  * \brief Install an ns3::McpttPttApp on the node. 
  *
  * \param node The node on which an McpttPttApp will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const Ptr<Node>& node);
 /**
  * \brief Install an ns3::McpttPttApp on the node.
  *
  * \param nodeName The node on which an McpttPttApp will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const std::string& nodeName);
 /**
  * Configures the floor control server app.
  * \param name the name of the model to set
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
 virtual void SetApp (std::string tid,
                      std::string n0 = "", const AttributeValue& v0 = EmptyAttributeValue (),
                      std::string n1 = "", const AttributeValue& v1 = EmptyAttributeValue (),
                      std::string n2 = "", const AttributeValue& v2 = EmptyAttributeValue (),
                      std::string n3 = "", const AttributeValue& v3 = EmptyAttributeValue (),
                      std::string n4 = "", const AttributeValue& v4 = EmptyAttributeValue (),
                      std::string n5 = "", const AttributeValue& v5 = EmptyAttributeValue (),
                      std::string n6 = "", const AttributeValue& v6 = EmptyAttributeValue (),
                      std::string n7 = "", const AttributeValue& v7 = EmptyAttributeValue ());
 /**
  * Configures the floor control arbitrator object.
  * \param name the name of the model to set
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
 virtual void SetArbitrator (std::string tid,
                             std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                             std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                             std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                             std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                             std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                             std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                             std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                             std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
 /**
  * Configures the dual floor control object.
  * \param name the name of the model to set
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
 virtual void SetDualControl (std::string tid,
                              std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                              std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                              std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                              std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                              std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                              std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                              std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                              std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
 /**
  * Configures the floor towards participant object.
  * \param name the name of the model to set
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
 virtual void SetParticipant (std::string tid,
                              std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                              std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                              std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                              std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                              std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                              std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                              std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                              std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
 /**
  * Configures the call control information object.
  * \param name the name of the model to set
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
 virtual void SetCallInfo (std::string tid,
                           std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                           std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                           std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                           std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                           std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                           std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                           std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                           std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
 /**
  * Sets the server clients up for communication without the use of call control.
  * \param server The server application.
  * \param client The client applications.
  */
 virtual void SetupFloorControl (const ApplicationContainer& server, const ApplicationContainer& clients);
protected:
 /**
  * \brief Install an ns3::McpttPttApp on the node.
  *
  * \param node The node on which an McpttPttApp will be installed.
  * \returns Ptr to the application installed.
  */
 virtual Ptr<Application> InstallPriv (const Ptr<Node>& node);
private:
 ObjectFactory m_appFac; //!< The MCPTT application object factory.
 ObjectFactory m_arbitratorFac; //!< The floor arbitrator factory.
 ObjectFactory m_dualControlFac; //!< The dual floor control factory
 ObjectFactory m_participantFac; //!< The floor towards participant factory.
 ObjectFactory m_callInfoFac; //!< The floor towards participant factory.
};

} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_CONTROL_HELPER_H */

