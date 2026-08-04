// Copyright (c) 2011-2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors:
//   Jaume Nin <jnin@cttc.es>
//   Nicola Baldo <nbaldo@cttc.es>
//   Manuel Requena <manuel.requena@cttc.es>
//   (most of the code refactored to no-backhaul-epc-helper.h)

#ifndef NR_POINT_TO_POINT_EPC_HELPER_H
#define NR_POINT_TO_POINT_EPC_HELPER_H

#include "nr-no-backhaul-epc-helper.h"

namespace ns3
{

/**
 * \ingroup helper
 *
 * \brief Create an EPC network with PointToPoint links
 *
 * The class is based on the LTE version. The usage is, in most of the cases,
 * automatic inside the NrHelper. All the user has to do, is:
 *
\verbatim
  Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper> ();
  ...
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetEpcHelper (nrEpcHelper);
\endverbatim
 *
 * This helper will then used to create the links between the GNBs and the EPC.
 * All links will be point-to-point, with some properties.
 * The user can set the point-to-point links properties by using:
 *
\verbatim
  nrEpcHelper->SetAttribute ("AttributeName", UintegerValue (10));
\endverbatim
 *
 * And these attribute will be valid for all the code that follows the
 * SetAttribute call. The list of attributes can be seen in the class
 * PointToPointEpcHelper in the ns-3 code base.
 *
 * \section p2p_epc_pgw Obtaining the PGW node
 *
 * You can obtain the pointer to the PGW node by doing:
\verbatim
  Ptr<Node> pgw = nrEpcHelper->GetPgwNode ();
\endverbatim
 *
 * After that, you would probably want to setup a network between the PGW and
 * your remote nodes, to create your topology. As example, there is the code
 * that setup a point to point link between the PGW and a single remote node:
 *
\verbatim
  // Create our remote host
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);

  // Install internet stack on the remote host
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // connect a remoteHost to pgw
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (2500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.000)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

  // Here is the routing part.. please note that UEs will always be in the
  // class 7.0.0.0
  Ipv4AddressHelper ipv4h;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting
(remoteHost->GetObject<Ipv4> ()); remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address
("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1); \endverbatim
 *
 * \section p2p_epc_ipv4 Assigning IPV4 addresses
 *
 * Another important thing that this helper can do is assigning automatically
 * the IPv4 addresses to the UE:
 *
\verbatim
  NetDeviceContainer netDeviceContainerForUe = ...;
  Ipv4InterfaceContainer ueLowLatIpIface = nrEpcHelper->AssignUeIpv4Address
(netDeviceContainerForUe);
\endverbatim
 *
 * And, of course, you would like to set the default routing for the UE,
 * which is the address of the EPC:
 *
\verbatim
  // Set the default gateway for the UEs
  for (uint32_t j = 0; j < ueContainer.GetN(); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting
(ueContainer.Get(j)->GetObject<Ipv4> ()); ueStaticRouting->SetDefaultRoute
(nrEpcHelper->GetUeDefaultGatewayAddress (), 1);
    }
\endverbatim
 *
 * For everything else, please see also the NrHelper documentation.
 *
 * \see PointToPointEpcHelper
 */
class NrPointToPointEpcHelper : public NrNoBackhaulEpcHelper
{
  public:
    /**
     * \brief Constructor
     */
    NrPointToPointEpcHelper();

    /**
     * \brief Destructor
     */
    ~NrPointToPointEpcHelper() override;

    // inherited from Object
    /**
     *  \brief Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    // inherited from NrEpcHelper
    void AddGnb(Ptr<Node> gnbNode,
                Ptr<NetDevice> nrGnbNetDevice,
                std::vector<uint16_t> cellIds) override;

  private:
    /**
     * S1-U interfaces
     */

    /**
     * Helper to assign addresses to S1-U NetDevices
     */
    Ipv4AddressHelper m_s1uIpv4AddressHelper;

    /**
     * The data rate to be used for the next S1-U link to be created
     */
    DataRate m_s1uLinkDataRate;

    /**
     * The delay to be used for the next S1-U link to be created
     */
    Time m_s1uLinkDelay;

    /**
     * The MTU of the next S1-U link to be created. Note that,
     * because of the additional GTP/UDP/IP tunneling overhead,
     * you need a MTU larger than the end-to-end MTU that you
     * want to support.
     */
    uint16_t m_s1uLinkMtu;

    /**
     * Helper to assign addresses to S1-MME NetDevices
     */
    Ipv4AddressHelper m_s1apIpv4AddressHelper;

    /**
     * Enable PCAP generation for S1 link
     */
    bool m_s1uLinkEnablePcap;

    /**
     * Prefix for the PCAP file for the S1 link
     */
    std::string m_s1uLinkPcapPrefix;
};

} // namespace ns3

#endif // NR_POINT_TO_POINT_EPC_HELPER_H
