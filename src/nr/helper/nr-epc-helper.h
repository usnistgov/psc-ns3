// Copyright (c) 2011-2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors:
//   Jaume Nin <jnin@cttc.es>
//   Nicola Baldo <nbaldo@cttc.es>
//   Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_EPC_HELPER_H
#define NR_EPC_HELPER_H

#include <ns3/data-rate.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv6-address-helper.h>
#include <ns3/nr-epc-tft.h>
#include <ns3/nr-eps-bearer.h>
#include <ns3/object.h>

namespace ns3
{

class Node;
class NetDevice;
class VirtualNetDevice;
class NrEpcX2;

/**
 * \ingroup nr
 *
 * \brief Base helper class to handle the creation of the EPC entities.
 *
 * This class provides the API for the implementation of helpers that
 * allow to create EPC entities and the nodes and interfaces that host
 * and connect them.
 */
class NrEpcHelper : public Object
{
  public:
    /**
     * Constructor
     */
    NrEpcHelper();

    /**
     * Destructor
     */
    ~NrEpcHelper() override;

    // inherited from Object
    /**
     * Register this type.
     * \return The object TypeId.
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * Add an gNB to the EPC
     *
     * \param gnbNode the previously created gNB node which is to be
     * added to the EPC
     * \param nrGnbNetDevice the NrGnbNetDevice of the gNB node
     * \param cellIds IDs of cells served by this gNB
     */
    virtual void AddGnb(Ptr<Node> gnbNode,
                        Ptr<NetDevice> nrGnbNetDevice,
                        std::vector<uint16_t> cellIds) = 0;

    /**
     * Notify the EPC of the existence of a new UE which might attach at a later time
     *
     * \param ueNrDevice the UE device to be attached
     * \param imsi the unique identifier of the UE
     */
    virtual void AddUe(Ptr<NetDevice> ueNrDevice, uint64_t imsi) = 0;

    /**
     * Add an X2 interface between two gNB
     *
     * \param gnbNode1 one gNB peer of the X2 interface
     * \param gnbNode2 the other gNB peer of the X2 interface
     */
    virtual void AddX2Interface(Ptr<Node> gnbNode1, Ptr<Node> gnbNode2) = 0;

    /**
     * Add an S1 interface between an gNB and a SGW
     *
     * \param gnb gNB peer of the S1 interface
     * \param gnbAddress gNB IPv4 address of the S1 interface
     * \param sgwAddress SGW IPv4 address of the S1 interface
     * \param cellIds cellIds of the gNB
     */
    virtual void AddS1Interface(Ptr<Node> gnb,
                                Ipv4Address gnbAddress,
                                Ipv4Address sgwAddress,
                                std::vector<uint16_t> cellIds) = 0;

    /**
     * Activate an EPS bearer, setting up the corresponding S1-U tunnel.
     *
     *
     *
     * \param ueNrDevice the Ipv4-enabled device of the UE, normally
     * connected via the NR radio interface
     * \param imsi the unique identifier of the UE
     * \param tft the Traffic Flow Template of the new bearer
     * \param bearer struct describing the characteristics of the EPS bearer to be activated
     * \return bearer ID
     */
    virtual uint8_t ActivateEpsBearer(Ptr<NetDevice> ueNrDevice,
                                      uint64_t imsi,
                                      Ptr<NrEpcTft> tft,
                                      NrEpsBearer bearer) = 0;

    /**
     * Get the SGW node
     *
     * \return a pointer to the SGW
     */
    virtual Ptr<Node> GetSgwNode() const = 0;

    /**
     * Get the PGW node
     *
     * \return a pointer to the PGW node
     * The primary use intended for this method is to allow the user to configure the SGi
     * interface of the PGW, i.e., to connect the PGW to the internet.
     */
    virtual Ptr<Node> GetPgwNode() const = 0;

    /**
     * Assign IPv4 addresses to UE devices
     *
     * \param ueDevices the set of UE devices
     *
     * \return the interface container, \see Ipv4AddressHelper::Assign() which has similar semantics
     */
    virtual Ipv4InterfaceContainer AssignUeIpv4Address(NetDeviceContainer ueDevices) = 0;

    /**
     * Assign IPv6 addresses to UE devices
     *
     * \param ueDevices the set of UE devices
     *
     * \return the interface container, \see Ipv6AddressHelper::Assign() which has similar semantics
     */
    virtual Ipv6InterfaceContainer AssignUeIpv6Address(NetDeviceContainer ueDevices) = 0;

    /**
     *
     * \return the IPv4 address of the Default Gateway to be used by UEs to reach the internet
     */
    virtual Ipv4Address GetUeDefaultGatewayAddress() = 0;

    /**
     *
     * \return the IPv6 address of the Default Gateway to be used by UEs to reach the internet
     */
    virtual Ipv6Address GetUeDefaultGatewayAddress6() = 0;

    /**
     * Assign a fixed random variable stream number to the random variables used.
     *
     * \param stream first stream index to use
     * \return the number of stream indices (possibly zero) that have been assigned
     */
    virtual int64_t AssignStreams(int64_t stream) = 0;

    /**
     * Informs the PGW/SGW of a new remote UE connected to a L3 UE-to-Network relay UE
     *
     * \param relayImsi The IMSI of the relay node
     * \param ueAddr The IPv4 address of the remote UE
     */
    virtual void AddRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr) = 0;

    /**
     * Deregister a remote UE that previously connected to a L3 UE-to-Network relay UE
     * \param relayImsi IMSI of the relay UE
     * \param ueAddr The IPv4 address of the remote UE
     */
    virtual void RemoveRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr) = 0;
};

} // namespace ns3

#endif // NR_EPC_HELPER_H
