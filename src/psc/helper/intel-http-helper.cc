/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include <ns3/log.h>
#include <ns3/names.h>

#include "intel-http-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("IntelHttpHelper");

namespace psc {

// -- IntelHttpClientHelper --

IntelHttpClientHelper::IntelHttpClientHelper (const Address &remoteAddress)
{
  NS_LOG_FUNCTION (this << remoteAddress);
  m_factory.SetTypeId ("ns3::psc::IntelHttpClient");
  m_factory.Set ("RemoteAddress", AddressValue (remoteAddress));
}

void
IntelHttpClientHelper::Set (const std::string &name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this);
  m_factory.Set (name, value);
}

ApplicationContainer
IntelHttpClientHelper::Install (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);
  return ApplicationContainer (DoInstall (node));
}

ApplicationContainer
IntelHttpClientHelper::Install (NodeContainer &nodes) const
{
  NS_LOG_FUNCTION (this);
  ApplicationContainer applications;
  for (auto node = nodes.Begin (); node != nodes.End (); node++)
    {
      applications.Add (DoInstall (*node));
    }

  return applications;
}

ApplicationContainer
IntelHttpClientHelper::Install (const std::string &nodeName) const
{
  NS_LOG_FUNCTION (this << nodeName);
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (DoInstall (node));
}

Ptr<Application>
IntelHttpClientHelper::DoInstall (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);
  NS_ABORT_MSG_IF (node == nullptr, "Cannot install on a NULL node");

  auto app = m_factory.Create<Application> ();
  node->AddApplication (app);
  return app;
}

// -- IntelHttpServerHelper --

IntelHttpServerHelper::IntelHttpServerHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_factory.SetTypeId ("ns3::psc::IntelHttpServer");
}

void
IntelHttpServerHelper::Set (const std::string &name, const AttributeValue &value)
{
  NS_LOG_FUNCTION (this);
  m_factory.Set (name, value);
}

ApplicationContainer
IntelHttpServerHelper::Install (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);
  return ApplicationContainer (DoInstall (node));
}

ApplicationContainer
IntelHttpServerHelper::Install (NodeContainer &nodes) const
{
  NS_LOG_FUNCTION (this);
  ApplicationContainer applications;
  for (auto node = nodes.Begin (); node != nodes.End (); node++)
    {
      applications.Add (DoInstall (*node));
    }

  return applications;
}

ApplicationContainer
IntelHttpServerHelper::Install (const std::string &nodeName) const
{
  NS_LOG_FUNCTION (this << nodeName);
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (DoInstall (node));
}

Ptr<Application>
IntelHttpServerHelper::DoInstall (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);
  NS_ABORT_MSG_IF (node == nullptr, "Cannot install on a NULL node");

  auto app = m_factory.Create<Application> ();
  node->AddApplication (app);
  return app;
}

} // namespace psc
} // namespace ns3
