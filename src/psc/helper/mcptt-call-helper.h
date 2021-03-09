/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2019 University of Washington
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
 */

#ifndef MCPTT_CALL_HELPER_H
#define MCPTT_CALL_HELPER_H

#include <string>

#include "ns3/ptr.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/application-container.h"

namespace ns3 {

class Time;
class ApplicationContainer;

namespace psc {

class McpttServerApp;
class McpttPttApp;
class McpttCallMsgFieldCallType;

/**
 * \ingroup psc
 * \brief A helper for creating MCPTT calls
 */
class McpttCallHelper
{
public:
  /**
   * \brief Creates an instance of the McpttCallHelper class.
   */
  McpttCallHelper (void);
  /**
   * \brief Destructor of the McpttCallHelper class.
   */
  virtual ~McpttCallHelper (void);

  /**
   * Configure a call for a set of on-network clients and a server.
   * \param clients container of client applications
   * \param server pointer to the server application
   * \param groupId GroupId to assign
   * \param callType type of call to configure
   * \param startTime start time of the call
   * \param stopTime stop time of the call
   * \return the call ID of the call
   */
  uint16_t AddCall (ApplicationContainer clients, Ptr<McpttServerApp> server, uint32_t groupId, McpttCallMsgFieldCallType callType, Time startTime, Time stopTime);
  /**
   * Configure a call for a set of off-network clients.
   * \param clients container of client applications
   * \param callId The ID of the call.
   * \param peerAddress The address of the off-network group.
   * \param groupId GroupId to assign
   * \param callType type of call to configure
   * \param startTime start time of the call
   * \param stopTime stop time of the call
   */
  void AddCallOffNetwork (ApplicationContainer clients, uint16_t callId, Address peerAddress, uint32_t groupId, McpttCallMsgFieldCallType callType, Time startTime, Time stopTime);
  /**
   * Configures and selects a call for a set of MCPTT applications.
   * \param apps The application container with the set of apps to configure.
   * \param peerAddress The peer address.
   * \param usersPerGroup The number of users to put in each group.
   * \param baseGroupId The starting group ID.
   */
  virtual void ConfigureOffNetworkBasicGrpCall (ApplicationContainer& apps, Address peerAddress, uint32_t usersPerGroup, uint32_t baseGroupId = 1);
  /**
   * Configures the default floor control arbitrator object.
   * \param tid the name of the model to set
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
   * Configures the default floor control towards participant object.
   * \param tid the name of the model to set
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
  virtual void SetTowardsParticipant (std::string tid,
                                      std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                                      std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                                      std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                                      std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                                      std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                                      std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                                      std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                                      std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Configures the default floor control participant object.
   * \param tid the name of the model to set
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
   * Configures the default server call object.
   * \param tid the name of the model to set
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
  virtual void SetServerCall (std::string tid,
                              std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                              std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                              std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                              std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                              std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                              std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                              std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                              std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());

private:
  ObjectFactory m_arbitratorFactory; //!< The floor arbitrator factory
  ObjectFactory m_towardsParticipantFactory; //!< The towards participant factory
  ObjectFactory m_participantFactory; //!< The participant factory
  ObjectFactory m_serverCallFactory; //!< The server call factory
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_HELPER */
