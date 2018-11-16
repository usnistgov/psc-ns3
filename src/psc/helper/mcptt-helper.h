
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

#include <ns3/application-container.h>
#include <ns3/attribute.h>
#include <ns3/data-rate.h>
#include <ns3/ipv4-address.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

#include "mcptt-msg-stats.h"
#include "mcptt-state-machine-stats.h"

namespace ns3 {

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
  * Enables the MCPTT message trace at the application layer.
  */
 virtual void EnableMsgTraces (void);
 /**
  * Enables the MCPTT state machine traces.
  */
 virtual void EnableStateMachineTraces (void);
 /**
  * Sets the attribute of the created MCPTT applications.
  * \param name The name of the attribute.
  * \param value The value of the attribute.
  */
 virtual void SetAppAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the MCPTT applicaton to create.
  * \param appTid The type ID of the MCPTT application.
  */
 virtual void SetAppTid (const TypeId& appTid);
 /**
  * Sets the attribute of the pushers that will be created.
  * \param name The name of the attribute.
  * \param value The value of the attribute.
  */
 virtual void SetPusherAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the pusher to create.
  * \param pusherTid The typeId.
  */
 virtual void SetPusherTid (const TypeId& pusherTid);
 /**
  * Sets the attribute of the requesters that will be created.
  * \param name The name of the attribute.
  * \param value The value of the attribute.
  */
 virtual void SetMediaSrcAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the requester to create.
  * \param requesterTid The type ID.
  */
 virtual void SetMediaSrcTid (const TypeId& requesterTid);
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
 ObjectFactory m_pusherFac; //!< The MCPTT pusher object factory.
 ObjectFactory m_mediaSrcFac; //!< The MCPTT send requester object factory.
 Ptr<McpttMsgStats> m_msgTracer; //!< The object used to trace MCPTT messages.
 Ptr<McpttStateMachineStats> m_stateMachineTracer; //!< The object used to trace MCPTT state machine traces.
};

} // namespace ns3

#endif /* MCPTT_HELPER_H */

