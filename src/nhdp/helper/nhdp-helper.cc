/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#include "nhdp-helper.h"

#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/nhdp-client.h"

NS_LOG_COMPONENT_DEFINE("NhdpHelper");

namespace ns3
{

namespace nhdp
{

NhdpHelper::NhdpHelper()
{
    NS_LOG_FUNCTION(this);
    m_factory.SetTypeId(NhdpClient::GetTypeId());
}

void
NhdpHelper::SetAttribute(std::string name, const AttributeValue& value)
{
    m_factory.Set(name, value);
}

ApplicationContainer
NhdpHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer
NhdpHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer
NhdpHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

Ptr<Application>
NhdpHelper::InstallPriv(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this << node);
    Ptr<Application> app = m_factory.Create<NhdpClient>();
    node->AddApplication(app);

    return app;
}

int64_t
NhdpHelper::AssignStreams(NodeContainer c, int64_t stream)
{
    auto currentStream = stream;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        auto node = (*i);
        for (uint32_t j = 0; j < node->GetNApplications(); ++j)
        {
            if (auto app = node->GetApplication(j)->GetObject<NhdpClient>())
            {
                currentStream += app->AssignStreams(currentStream);
            }
        }
    }
    return (currentStream - stream);
}

} // namespace nhdp

} // namespace ns3
