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
#include "ns3/nhdp-client.h"
#include "ns3/nhdp-info-base.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/output-stream-wrapper.h"

#include <string>

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

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.  The Install() method of the InternetStackHelper
     * should have previously been called by the user.
     *
     * @param stream first stream index to use
     * @param c NodeContainer of the set of nodes for which the OlsrRoutingProtocol
     *          should be modified to use a fixed stream
     * @return the number of stream indices assigned by this helper
     */
    int64_t AssignStreams(NodeContainer c, int64_t stream);

    /**
     * Write trace of NhdpClient::LinkChange into output stream for all nodes in the container
     *
     * Ptr<Node> can also be used as the first argument (automatically converts to a NodeContainer).
     * If the stream argument is omitted, traces will be written to 'nhdp-link-change-trace.txt'.
     *
     * @param nodes The NodeContainer of nodes to trace
     * @param stream The (optional) output stream to write to
     */
    void EnableLinkChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream = nullptr);

    /**
     * Write trace of NhdpClient::NeighborChange into output stream for all nodes in the container
     *
     * Ptr<Node> can also be used as the first argument (automatically converts to a NodeContainer).
     * If the stream argument is omitted, traces will be written to
     * 'nhdp-neighbor-change-trace.txt'.
     *
     * @param nodes The NodeContainer of nodes to trace
     * @param stream The (optional) output stream to write to
     */
    void EnableNeighborChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream = nullptr);

    /**
     * Write trace of NhdpClient::TwoHopChange into output stream for all nodes in the container
     *
     * Ptr<Node> can also be used as the first argument (automatically converts to a NodeContainer).
     * If the stream argument is omitted, traces will be written to
     * 'nhdp-two-hop-change-trace.txt'.
     *
     * @param nodes The NodeContainer of nodes to trace
     * @param stream The (optional) output stream to write to
     */
    void EnableTwoHopChangeTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream = nullptr);

    /**
     * Write trace of NhdpClient::LostNeighborChange into output stream for all nodes in the
     * container
     *
     * Ptr<Node> can also be used as the first argument (automatically converts to a NodeContainer).
     * If the stream argument is omitted, traces will be written to
     * 'nhdp-lost-neighbor-change-trace.txt'.
     *
     * @param nodes The NodeContainer of nodes to trace
     * @param stream The (optional) output stream to write to
     */
    void EnableLostNeighborChangeTrace(NodeContainer nodes,
                                       Ptr<OutputStreamWrapper> stream = nullptr);

    /**
     * Write trace of NhdpClient::LinkFailureinto output stream for all nodes in the
     * container
     *
     * Ptr<Node> can also be used as the first argument (automatically converts to a NodeContainer).
     * If the stream argument is omitted, traces will be written to
     * 'nhdp-link-failure-trace.txt'.
     *
     * @param nodes The NodeContainer of nodes to trace
     * @param stream The (optional) output stream to write to
     */
    void EnableLinkFailureTrace(NodeContainer nodes, Ptr<OutputStreamWrapper> stream = nullptr);

  private:
    Ptr<Application> InstallPriv(Ptr<Node> node) const;

    /**
     * Write trace of NhdpClient::LinkChange into output stream
     *
     * @param stream output stream to write to
     * @param context trace context
     * @param action Type of change to report (added, changed, removed)
     * @param oldTuple The old value of the LinkTuple
     * @param newTuple The new value of the LinkTuple
     */
    static void LinkChangeTrace(Ptr<OutputStreamWrapper> stream,
                                std::string context,
                                Action action,
                                const LinkTuple& oldTuple,
                                const LinkTuple& newTuple);

    /**
     * Write trace of NhdpClient::NeighborChange into output stream
     *
     * @param stream output stream to write to
     * @param context trace context
     * @param action Type of change to report (added, changed, removed)
     * @param oldTuple The old value of the NeighborTuple
     * @param newTuple The new value of the NeighborTuple
     */
    static void NeighborChangeTrace(Ptr<OutputStreamWrapper> stream,
                                    std::string context,
                                    Action action,
                                    const NeighborTuple& olTuple,
                                    const NeighborTuple& newTuple);

    /**
     * Write trace of NhdpClient::TwoHopChange into output stream
     *
     * @param stream output stream to write to
     * @param context trace context
     * @param action Type of change to report (added, changed, removed)
     * @param oldTuple The old value of the TwoHopTuple
     * @param newTuple The new value of the TwoHopTuple
     */
    static void TwoHopChangeTrace(Ptr<OutputStreamWrapper> stream,
                                  std::string context,
                                  Action action,
                                  const TwoHopTuple& oldTuple,
                                  const TwoHopTuple& newTuple);

    /**
     * Write trace of NhdpClient::LostNeighborChange into output stream
     *
     * @param stream output stream to write to
     * @param context trace context
     * @param action Type of change to report (added, changed, removed)
     * @param oldTuple The old value of the LostNeighborTuple
     * @param newTuple The new value of the LostNeighborTuple
     */
    static void LostNeighborChangeTrace(Ptr<OutputStreamWrapper> stream,
                                        std::string context,
                                        Action action,
                                        const LostNeighborTuple& oldTuple,
                                        const LostNeighborTuple& newTuple);

    /**
     * Write trace of NhdpClient::LinkFailure into output stream
     *
     * @param stream output stream to write to
     * @param context trace context
     * @param address The peer IPv4 address
     */
    static void LinkFailureTrace(Ptr<OutputStreamWrapper> stream,
                                 std::string context,
                                 const Ipv4Address& address);

    ObjectFactory m_factory;

    Ptr<OutputStreamWrapper> m_defaultLinkChangeStream{nullptr};
    Ptr<OutputStreamWrapper> m_defaultNeighborChangeStream{nullptr};
    Ptr<OutputStreamWrapper> m_defaultTwoHopChangeStream{nullptr};
    Ptr<OutputStreamWrapper> m_defaultLostNeighborChangeStream{nullptr};
    Ptr<OutputStreamWrapper> m_defaultLinkFailureStream{nullptr};
};

} // namespace nhdp

} // namespace ns3
#endif /* NHDP_HELPER_H */
