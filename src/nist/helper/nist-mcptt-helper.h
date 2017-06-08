/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * 2016 National Institute of Standards and Technology (NIST)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Wesley Garey <wesley.garey@nist.gov>
 */

#ifndef NIST_MCPTT_HELPER_H
#define NIST_MCPTT_HELPER_H

#include <stdint.h>
#include <string>

#include <ns3/address.h>
#include <ns3/application-container.h>
#include <ns3/attribute.h>
#include <ns3/data-rate.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>

namespace ns3 {

/**
 * \ingroup mcptt
 * \brief A helper for creating, configuring, and installing MCPTT apps.
 */
class NistMcpttHelper
{
public:
 /**
  * \brief Creates an instance of the NistMcpttHelper class.
  */
 NistMcpttHelper ();
 /**
  * \brief Destructor of the NistMcpttHelper class.
  */
 virtual ~NistMcpttHelper (void);
 /**
  * \brief Creates an instance of the NistMcpttHelper class.
  * \param peerAddress The address of the node that the peer application is on.
  * \param peerPort The port number of the peer application on the server.
  */
 NistMcpttHelper (const Address& peerAddress, const uint16_t& peerPort);
 /**
  * \brief Install an ns3::NistMcpttPttApp on each node of the input container.
  *
  * \param c NodeContainer of the set of nodes on which an NistMcpttPttApp 
  * will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const NodeContainer& c);
 /**
  * \brief Install an ns3::NistMcpttPttApp on the node. 
  *
  * \param node The node on which an NistMcpttPttApp will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const Ptr<Node>& node);
 /**
  * \brief Install an ns3::NistMcpttPttApp on the node.
  *
  * \param nodeName The node on which an NistMcpttPttApp will be installed.
  * \returns Container of Ptr to the applications installed.
  */
 virtual ApplicationContainer Install (const std::string& nodeName);
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
  * Sets the attribute of the created floor machines.
  * \param name The name of the attribute.
  * \param value The value of the attribute.
  */
 virtual void SetFloorMachineAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the floor machine to create.
  * \param floorTid The type ID of the floor machine.
  */
 virtual void SetFloorMachineTid (const TypeId& floorTid);
 /**
  * Sets the attriubte of the message parsers that will be created.
  * \param name The name of the attribute.
  * \param value The value of the attribute.
  */
 virtual void SetMsgParserAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the message parser to create.
  * \param parserTid The type ID.
  */
 virtual void SetMsgParserTid (const TypeId& parserTid);
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
 virtual void SetRequesterAttribute (const std::string& name, const AttributeValue& value);
 /**
  * Sets the type ID of the requester to create.
  * \param requesterTid The type ID.
  */
 virtual void SetRequesterTid (const TypeId& requesterTid);
protected:
 /**
  * \brief Install an ns3::NistMcpttPttApp on the node.
  *
  * \param node The node on which an NistMcpttPttApp will be installed.
  * \returns Ptr to the application installed.
  */
 virtual Ptr<Application> InstallPriv (const Ptr<Node>& node);
 /**
  * \brief Gets the next MCPTT ID and advances the sequence.
  * \returns The next MCPTT ID.
  */
 virtual uint32_t NextUserId (void);
private:
 ObjectFactory m_appFac; //!< The MCPTT application object factory.
 ObjectFactory m_floorFac; //!< The MCPTT floor machine object factory.
 ObjectFactory m_msgParserFac; //!< The MCPTT message parser object factory.
 uint32_t m_nextUserId; //!< The MCPTT ID that will be assigned to the next application that is created.
 ObjectFactory m_requesterFac; //!< The MCPTT send requester object factory.
 ObjectFactory m_pusherFac; //!< The MCPTT pusher object factory.
protected:
 /**
  * \brief Gets the MCPTT ID that will be assigned to the next application that is created.
  * \returns The MCPTT ID
  */
 virtual uint32_t GetNextUserId (void) const;
 /**
  * \brief Sets the MCPTT ID that will be assigned to the next created application.
  * \param nextUserId The next MCPTT ID.
  */
 virtual void SetNextUserId (const uint32_t& nextUserId);
};

} // namespace ns3

#endif /* NIST_MCPTT_HELPER_H */

