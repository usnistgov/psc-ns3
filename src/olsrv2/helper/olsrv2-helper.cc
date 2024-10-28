/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "olsrv2-helper.h"

#include "ns3/ipv4-list-routing.h"
#include "ns3/names.h"
#include "ns3/node-list.h"
#include "ns3/olsrv2-routing-protocol.h"
#include "ns3/ptr.h"

namespace ns3
{

OlsrHelper::OlsrHelper()
{
    m_agentFactory.SetTypeId("ns3::olsrv2::RoutingProtocol");
}

OlsrHelper::OlsrHelper(const OlsrHelper& o)
    : m_agentFactory(o.m_agentFactory)
{
    m_interfaceExclusions = o.m_interfaceExclusions;
}

OlsrHelper*
OlsrHelper::Copy() const
{
    return new OlsrHelper(*this);
}

void
OlsrHelper::ExcludeInterface(Ptr<Node> node, uint32_t interface)
{
    auto it = m_interfaceExclusions.find(node);

    if (it == m_interfaceExclusions.end())
    {
        std::set<uint32_t> interfaces;
        interfaces.insert(interface);

        m_interfaceExclusions.insert(std::make_pair(node, std::set<uint32_t>(interfaces)));
    }
    else
    {
        it->second.insert(interface);
    }
}

Ptr<Ipv4RoutingProtocol>
OlsrHelper::Create(Ptr<Node> node) const
{
    Ptr<olsrv2::RoutingProtocol> agent = m_agentFactory.Create<olsrv2::RoutingProtocol>();

    auto it = m_interfaceExclusions.find(node);

    if (it != m_interfaceExclusions.end())
    {
        agent->SetInterfaceExclusions(it->second);
    }

    node->AggregateObject(agent);
    return agent;
}

void
OlsrHelper::Set(std::string name, const AttributeValue& value)
{
    m_agentFactory.Set(name, value);
}

int64_t
OlsrHelper::AssignStreams(NodeContainer c, int64_t stream)
{
    int64_t currentStream = stream;
    Ptr<Node> node;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        node = (*i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        NS_ASSERT_MSG(ipv4, "Ipv4 not installed on node");
        Ptr<Ipv4RoutingProtocol> proto = ipv4->GetRoutingProtocol();
        NS_ASSERT_MSG(proto, "Ipv4 routing not installed on node");
        Ptr<olsrv2::RoutingProtocol> olsr = DynamicCast<olsrv2::RoutingProtocol>(proto);
        if (olsr)
        {
            currentStream += olsr->AssignStreams(currentStream);
            continue;
        }
        // Olsr may also be in a list
        Ptr<Ipv4ListRouting> list = DynamicCast<Ipv4ListRouting>(proto);
        if (list)
        {
            int16_t priority;
            Ptr<Ipv4RoutingProtocol> listProto;
            Ptr<olsrv2::RoutingProtocol> listOlsr;
            for (uint32_t i = 0; i < list->GetNRoutingProtocols(); i++)
            {
                listProto = list->GetRoutingProtocol(i, priority);
                listOlsr = DynamicCast<olsrv2::RoutingProtocol>(listProto);
                if (listOlsr)
                {
                    currentStream += listOlsr->AssignStreams(currentStream);
                    break;
                }
            }
        }
    }
    return (currentStream - stream);
}

} // namespace ns3
