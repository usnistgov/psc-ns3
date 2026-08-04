/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#include "nhdp-helper.h"

#include "ns3/callback.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/trace-helper.h"

#include <iomanip>
#include <iostream>

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

void
NhdpHelper::EnableLinkChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream)
{
    NS_LOG_FUNCTION(this);
    if (!stream && !m_defaultLinkChangeStream)
    {
        AsciiTraceHelper asciiTraceHelper;
        stream = asciiTraceHelper.CreateFileStream("nhdp-link-change-trace.txt");
        m_defaultLinkChangeStream = stream;
    }
    else if (!stream)
    {
        // Avoid creating multiple std::ostream file descriptors to the same output file
        stream = m_defaultLinkChangeStream;
    }
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        for (uint32_t j = 0; j < nodes.Get(i)->GetNApplications(); j++)
        {
            Ptr<NhdpClient> client = DynamicCast<NhdpClient>(nodes.Get(i)->GetApplication(j));
            if (client)
            {
                NS_LOG_DEBUG("Connecting LinkChange trace on " << client->GetNode()->GetId());
                client->TraceConnect("LinkChange",
                                     std::to_string(client->GetNode()->GetId()),
                                     MakeBoundCallback(&NhdpHelper::LinkChangeTrace, stream));
            }
        }
    }
}

void
NhdpHelper::EnableNeighborChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream)
{
    NS_LOG_FUNCTION(this);
    if (!stream && !m_defaultNeighborChangeStream)
    {
        AsciiTraceHelper asciiTraceHelper;
        stream = asciiTraceHelper.CreateFileStream("nhdp-neighbor-change-trace.txt");
        m_defaultNeighborChangeStream = stream;
    }
    else if (!stream)
    {
        // Avoid creating multiple std::ostream file descriptors to the same output file
        stream = m_defaultNeighborChangeStream;
    }
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        for (uint32_t j = 0; j < nodes.Get(i)->GetNApplications(); j++)
        {
            Ptr<NhdpClient> client = DynamicCast<NhdpClient>(nodes.Get(i)->GetApplication(j));
            if (client)
            {
                NS_LOG_DEBUG("Connecting NeighborChange trace on " << client->GetNode()->GetId());
                client->TraceConnect("NeighborChange",
                                     std::to_string(client->GetNode()->GetId()),
                                     MakeBoundCallback(&NhdpHelper::NeighborChangeTrace, stream));
            }
        }
    }
}

void
NhdpHelper::EnableTwoHopChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream)
{
    NS_LOG_FUNCTION(this);
    if (!stream && !m_defaultTwoHopChangeStream)
    {
        AsciiTraceHelper asciiTraceHelper;
        stream = asciiTraceHelper.CreateFileStream("nhdp-two-hop-change-trace.txt");
        m_defaultTwoHopChangeStream = stream;
    }
    else if (!stream)
    {
        // Avoid creating multiple std::ostream file descriptors to the same output file
        stream = m_defaultTwoHopChangeStream;
    }
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        for (uint32_t j = 0; j < nodes.Get(i)->GetNApplications(); j++)
        {
            Ptr<NhdpClient> client = DynamicCast<NhdpClient>(nodes.Get(i)->GetApplication(j));
            if (client)
            {
                NS_LOG_DEBUG("Connecting TwoHopChange trace on " << client->GetNode()->GetId());
                client->TraceConnect("TwoHopChange",
                                     std::to_string(client->GetNode()->GetId()),
                                     MakeBoundCallback(&NhdpHelper::TwoHopChangeTrace, stream));
            }
        }
    }
}

void
NhdpHelper::EnableLostNeighborChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream)
{
    NS_LOG_FUNCTION(this);
    if (!stream && !m_defaultLostNeighborChangeStream)
    {
        AsciiTraceHelper asciiTraceHelper;
        stream = asciiTraceHelper.CreateFileStream("nhdp-lost-neighbor-change-trace.txt");
        m_defaultLostNeighborChangeStream = stream;
    }
    else if (!stream)
    {
        // Avoid creating multiple std::ostream file descriptors to the same output file
        stream = m_defaultLostNeighborChangeStream;
    }
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        for (uint32_t j = 0; j < nodes.Get(i)->GetNApplications(); j++)
        {
            Ptr<NhdpClient> client = DynamicCast<NhdpClient>(nodes.Get(i)->GetApplication(j));
            if (client)
            {
                NS_LOG_DEBUG("Connecting LostNeighborChange trace on "
                             << client->GetNode()->GetId());
                client->TraceConnect(
                    "LostNeighborChange",
                    std::to_string(client->GetNode()->GetId()),
                    MakeBoundCallback(&NhdpHelper::LostNeighborChangeTrace, stream));
            }
        }
    }
}

void
NhdpHelper::EnableLinkFailureTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream)
{
    NS_LOG_FUNCTION(this);
    if (!stream && !m_defaultLinkFailureStream)
    {
        AsciiTraceHelper asciiTraceHelper;
        stream = asciiTraceHelper.CreateFileStream("nhdp-link-failure-trace.txt");
        m_defaultLinkFailureStream = stream;
    }
    else if (!stream)
    {
        // Avoid creating multiple std::ostream file descriptors to the same output file
        stream = m_defaultLinkFailureStream;
    }
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        for (uint32_t j = 0; j < nodes.Get(i)->GetNApplications(); j++)
        {
            Ptr<NhdpClient> client = DynamicCast<NhdpClient>(nodes.Get(i)->GetApplication(j));
            if (client)
            {
                NS_LOG_DEBUG("Connecting LinkFailuree trace on " << client->GetNode()->GetId());
                client->TraceConnect("LinkFailure",
                                     std::to_string(client->GetNode()->GetId()),
                                     MakeBoundCallback(&NhdpHelper::LinkFailureTrace, stream));
            }
        }
    }
}

void
NhdpHelper::LinkChangeTrace(Ptr<OutputStreamWrapper> stream,
                            std::string context,
                            Action action,
                            const LinkTuple& oldTuple,
                            const LinkTuple& newTuple)
{
    NS_LOG_FUNCTION_NOARGS();
    *stream->GetStream() << std::fixed << std::setprecision(9) << std::setw(13)
                         << Now().GetSeconds() << std::setw(3) << context << " Link" << std::setw(8)
                         << action << std::setw(2) << newTuple.m_neighborAddrList[0]
                         << std::setw(10) << newTuple.GetLinkStatus()
                         << " heard: " << std::setprecision(3) << std::setw(6)
                         << newTuple.m_heardTime.GetSeconds() << " sym: " << std::setw(6)
                         << newTuple.m_symTime.GetSeconds() << " exp: " << std::setw(6)
                         << newTuple.m_expirationTime.GetSeconds() << " qual: " << std::setw(3)
                         << newTuple.m_quality << " pend: " << std::boolalpha << std::setw(5)
                         << newTuple.m_pending << " lost: " << std::boolalpha << std::setw(5)
                         << newTuple.m_lost << std::endl;
}

void
NhdpHelper::NeighborChangeTrace(Ptr<OutputStreamWrapper> stream,
                                std::string context,
                                Action action,
                                const NeighborTuple& oldTuple,
                                const NeighborTuple& newTuple)
{
    NS_LOG_FUNCTION_NOARGS();
    *stream->GetStream() << std::fixed << std::setprecision(9) << std::setw(13)
                         << Now().GetSeconds() << std::setw(3) << context << " Neighbor"
                         << std::setw(8) << action << std::setw(2) << newTuple.m_neighborAddrList[0]
                         << " sym: " << std::boolalpha << std::setw(5) << newTuple.m_symmetric
                         << std::endl;
}

void
NhdpHelper::TwoHopChangeTrace(Ptr<OutputStreamWrapper> stream,
                              std::string context,
                              Action action,
                              const TwoHopTuple& oldTuple,
                              const TwoHopTuple& newTuple)
{
    NS_LOG_FUNCTION_NOARGS();
    *stream->GetStream() << std::fixed << std::setprecision(9) << std::setw(13)
                         << Now().GetSeconds() << std::setw(3) << context << " Neighbor"
                         << std::setw(8) << action << std::setw(2) << newTuple.m_neighborAddrList[0]
                         << "  2-Hop Neighbor: " << std::setw(2) << newTuple.m_twoHopAddr
                         << std::endl;
}

void
NhdpHelper::LostNeighborChangeTrace(Ptr<OutputStreamWrapper> stream,
                                    std::string context,
                                    Action action,
                                    const LostNeighborTuple& oldTuple,
                                    const LostNeighborTuple& newTuple)
{
    NS_LOG_FUNCTION_NOARGS();
    *stream->GetStream() << std::fixed << std::setprecision(9) << std::setw(13)
                         << Now().GetSeconds() << std::setw(3) << context << " Neighbor"
                         << std::setw(8) << action << std::setw(2) << newTuple.m_neighborAddr
                         << " expiration: " << std::setw(2)
                         << newTuple.m_expirationTime.GetSeconds() << std::endl;
}

void
NhdpHelper::LinkFailureTrace(Ptr<OutputStreamWrapper> stream,
                             std::string context,
                             const Ipv4Address& address)
{
    NS_LOG_FUNCTION_NOARGS();
    *stream->GetStream() << std::fixed << std::setprecision(9) << std::setw(13)
                         << Now().GetSeconds() << std::setw(3) << context << " Neighbor"
                         << std::setw(2) << address << std::endl;
}

} // namespace nhdp

} // namespace ns3
