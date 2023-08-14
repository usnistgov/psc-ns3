
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

#ifndef MCPTT_HELPER_H
#define MCPTT_HELPER_H

#include <stdint.h>
#include <string>
#include <fstream>
#include <utility>
#include <map>

#include <ns3/application-container.h>
#include <ns3/attribute.h>
#include <ns3/data-rate.h>
#include <ns3/ipv4-address.h>
#include <ns3/mcptt-pusher-orchestrator.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

#include "mcptt-msg-stats.h"
#include "mcptt-state-machine-stats.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup psc
 * \brief A helper for creating, configuring, and installing MCPTT apps.
 */
class McpttHelper
{
public:
  /**
   * Helper to enable all MCPTT log components with one statement
   */
  static void EnableLogComponents (void);
  /**
   * \brief Gets the MCPTT ID that will be assigned to the next application that is created.
   * \returns The MCPTT ID
   */
  static uint32_t GetNextUserId (void);
  /**
   * \brief Creates an instance of the McpttHelper class.
   */
  McpttHelper (void);
  /**
   * \brief Destructor of the McpttHelper class.
   */
  virtual ~McpttHelper (void);
  /**
   * \brief Associate an ns3::psc::McpttPusherOrchestratorInterface with the pushers from a set of ns3::psc::McpttPttApp objects.
   * \param orchestrator The orchestrator.
   * \param apps The set of applications.
   */
  virtual void AddPushersToOrchestrator (Ptr<McpttPusherOrchestratorInterface> orchestrator, const ApplicationContainer& apps);
  /**
   * \brief Install an ns3::psc::McpttPttApp on each node of the input container.
   *
   * \param c NodeContainer of the set of nodes on which an McpttPttApp
   * will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  virtual ApplicationContainer Install (const NodeContainer& c);
  /**
   * \brief Install an ns3::psc::McpttPttApp on the node.
   *
   * \param node The node on which an McpttPttApp will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  virtual ApplicationContainer Install (const Ptr<Node>& node);
  /**
   * \brief Install an ns3::psc::McpttPttApp on the node.
   *
   * \param nodeName The node on which an McpttPttApp will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  virtual ApplicationContainer Install (const std::string& nodeName);
  /**
   * Configures the MCPTT PTT app.
   * \param tid the string representation of the ns3::TypeId associated with the model to set
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
  virtual void SetPttApp (std::string tid,
                          std::string n0 = "", const AttributeValue& v0 = EmptyAttributeValue (),
                          std::string n1 = "", const AttributeValue& v1 = EmptyAttributeValue (),
                          std::string n2 = "", const AttributeValue& v2 = EmptyAttributeValue (),
                          std::string n3 = "", const AttributeValue& v3 = EmptyAttributeValue (),
                          std::string n4 = "", const AttributeValue& v4 = EmptyAttributeValue (),
                          std::string n5 = "", const AttributeValue& v5 = EmptyAttributeValue (),
                          std::string n6 = "", const AttributeValue& v6 = EmptyAttributeValue (),
                          std::string n7 = "", const AttributeValue& v7 = EmptyAttributeValue ());
  /**
   * Configures the media source model.
   * \param tid the string representation of the ns3::TypeId associated with the model to set
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
  virtual void SetMediaSrc (std::string tid,
                            std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                            std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                            std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                            std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                            std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                            std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                            std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                            std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Configures the pusher model.
   * \param tid the string representation of the ns3::TypeId associated with the model to set
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
  virtual void SetPusher (std::string tid,
                          std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                          std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                          std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                          std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                          std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                          std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                          std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                          std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Configures the MCPTT pusher's push variable.
   * \param tid the string representation of the ns3::TypeId associated with the model to set
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
  virtual void SetPusherPttInterarrivalTimeVariable (std::string tid,
                                                     std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                                                     std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                                                     std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                                                     std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                                                     std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                                                     std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                                                     std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                                                     std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Configures the MCPTT pusher's release variable.
   * \param tid the string representation of the ns3::TypeId associated with the model to set
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
  virtual void SetPusherPttDurationVariable (std::string tid,
                                             std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                                             std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                                             std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                                             std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                                             std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                                             std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                                             std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                                             std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());

protected:
  /**
   * \brief Install an ns3::psc::McpttPttApp on the node.
   *
   * \param node The node on which an McpttPttApp will be installed.
   * \returns Ptr to the application installed.
   */
  virtual Ptr<Application> InstallPriv (const Ptr<Node>& node);

private:
  bool m_pushConfigured; //!< Indicates if the pusher's push variable was explicity configured.
  bool m_releaseConfigured; //!< Indicates if the pusher's release variable was explicity configured.
  ObjectFactory m_appFac; //!< The MCPTT application object factory.
  ObjectFactory m_pusherFac; //!< The MCPTT pusher object factory.
  ObjectFactory m_pusherPushFac; //!< The MCPTT pusher's push variable object factory.
  ObjectFactory m_pusherReleaseFac; //!< The MCPTT pusher's relase variable object factory.
  ObjectFactory m_mediaSrcFac; //!< The MCPTT send requester object factory.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_HELPER_H */

