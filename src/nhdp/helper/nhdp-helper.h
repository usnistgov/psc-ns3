/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#ifndef NHDP_HELPER_H
#define NHDP_HELPER_H

#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

namespace ns3
{

namespace nhdp
{

class NhdpHelper
{
  public:
    NhdpHelper();

    void SetAttribute(std::string name, const AttributeValue& value);

    ApplicationContainer Install(Ptr<Node> node) const;
    ApplicationContainer Install(std::string nodeName) const;
    ApplicationContainer Install(NodeContainer c) const;

  private:
    Ptr<Application> InstallPriv(Ptr<Node> node) const;

    ObjectFactory m_factory;
};

} // namespace nhdp

} // namespace ns3
#endif /* NHDP_HELPER_H */
