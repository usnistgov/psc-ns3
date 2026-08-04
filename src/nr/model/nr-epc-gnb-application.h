// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors:
//   Jaume Nin <jnin@cttc.cat>
//   Nicola Baldo <nbaldo@cttc.cat>

#ifndef NR_EPC_GNB_APPLICATION_H
#define NR_EPC_GNB_APPLICATION_H

#include "nr-epc-gnb-s1-sap.h"
#include "nr-epc-s1ap-sap.h"

#include <ns3/address.h>
#include <ns3/application.h>
#include <ns3/callback.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/socket.h>
#include <ns3/traced-callback.h>
#include <ns3/virtual-net-device.h>

#include <map>

namespace ns3
{
class NrEpcGnbS1SapUser;
class NrEpcGnbS1SapProvider;

/**
 * \ingroup nr
 *
 * This application is installed inside gNBs and provides the bridge functionality for user data
 * plane packets between the radio interface and the S1-U interface.
 */
class NrEpcGnbApplication : public Application
{
    /// allow NrMemberEpcGnbS1SapProvider<NrEpcGnbApplication> class friend access
    friend class NrMemberEpcGnbS1SapProvider<NrEpcGnbApplication>;
    /// allow NrMemberEpcS1apSapGnb<NrEpcGnbApplication> class friend access
    friend class NrMemberEpcS1apSapGnb<NrEpcGnbApplication>;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

  protected:
    void DoDispose() override;

  public:
    /**
     * Constructor
     *
     * \param nrSocket the socket to be used to send/receive IPv4 packets to/from the
     * NR radio interface
     * \param nrSocket6 the socket to be used to send/receive IPv6 packets to/from the
     * NR radio interface
     * \param cellId the identifier of the gNB
     */
    NrEpcGnbApplication(Ptr<Socket> nrSocket, Ptr<Socket> nrSocket6, uint16_t cellId);

    /**
     * Add a S1-U interface to the gNB
     *
     * \param s1uSocket the socket to be used to send/receive packets to/from the S1-U interface
     * connected with the SGW
     * \param gnbS1uAddress the IPv4 address of the S1-U interface of this gNB
     * \param sgwS1uAddress the IPv4 address at which this gNB will be able to reach its SGW for
     * S1-U communications
     */
    void AddS1Interface(Ptr<Socket> s1uSocket,
                        Ipv4Address gnbS1uAddress,
                        Ipv4Address sgwS1uAddress);

    /**
     * Destructor
     *
     */
    ~NrEpcGnbApplication() override;

    /**
     * Set the S1 SAP User
     *
     * \param s the S1 SAP User
     */
    void SetS1SapUser(NrEpcGnbS1SapUser* s);

    /**
     *
     * \return the S1 SAP Provider
     */
    NrEpcGnbS1SapProvider* GetS1SapProvider();

    /**
     * Set the MME side of the S1-AP SAP
     *
     * \param s the MME side of the S1-AP SAP
     */
    void SetS1apSapMme(NrEpcS1apSapMme* s);

    /**
     *
     * \return the gNB side of the S1-AP SAP
     */
    NrEpcS1apSapGnb* GetS1apSapGnb();

    /**
     * Method to be assigned to the recv callback of the NR socket. It is called when the gNB
     * receives a data packet from the radio interface that is to be forwarded to the SGW.
     *
     * \param socket pointer to the NR socket
     */
    void RecvFromNrSocket(Ptr<Socket> socket);

    /**
     * Method to be assigned to the recv callback of the S1-U socket. It is called when the gNB
     * receives a data packet from the SGW that is to be forwarded to the UE.
     *
     * \param socket pointer to the S1-U socket
     */
    void RecvFromS1uSocket(Ptr<Socket> socket);

    /**
     * TracedCallback signature for data Packet reception event.
     *
     * \param [in] packet The data packet sent from the internet.
     */
    typedef void (*RxTracedCallback)(Ptr<Packet> packet);

    /**
     * EPS flow ID structure
     */
    struct EpsFlowId_t
    {
        uint16_t m_rnti; ///< RNTI
        uint8_t m_bid;   ///< Bid, the EPS Bearer Identifier

      public:
        EpsFlowId_t();
        /**
         * Constructor
         *
         * \param a RNTI
         * \param b bid
         */
        EpsFlowId_t(const uint16_t a, const uint8_t b);

        /**
         * Comparison operator
         *
         * \param a first application
         * \param b second application
         * \returns true is the applications are "equal"
         */
        friend bool operator==(const EpsFlowId_t& a, const EpsFlowId_t& b);
        /**
         * Less than operator
         *
         * \param a first application
         * \param b second application
         * \returns true is the applications are "equal"
         */
        friend bool operator<(const EpsFlowId_t& a, const EpsFlowId_t& b);
    };

  private:
    // gNB S1 SAP provider methods
    /**
     * Initial UE message function
     * \param imsi the IMSI
     * \param rnti the RNTI
     */
    void DoInitialUeMessage(uint64_t imsi, uint16_t rnti);
    /**
     * Path switch request function
     * \param params PathSwitchRequestParameters
     */
    void DoPathSwitchRequest(NrEpcGnbS1SapProvider::PathSwitchRequestParameters params);
    /**
     * UE Context Release function
     * \param rnti the RNTI
     */
    void DoUeContextRelease(uint16_t rnti);

    // S1-AP SAP gNB methods
    /**
     * Initial Context Setup Request
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param erabToBeSetupList the ERAB setup list
     */
    void DoInitialContextSetupRequest(
        uint64_t mmeUeS1Id,
        uint16_t gnbUeS1Id,
        std::list<NrEpcS1apSapGnb::ErabToBeSetupItem> erabToBeSetupList);
    /**
     * Path Switch Request Acknowledge
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param cgi the CGI
     * \param erabToBeSwitchedInUplinkList the ERAB switched in uplink list
     */
    void DoPathSwitchRequestAcknowledge(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t cgi,
        std::list<NrEpcS1apSapGnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList);

    /**
     * \brief This function accepts bearer id corresponding to a particular UE and schedules
     * indication of bearer release towards MME
     * \param imsi maps to mmeUeS1Id
     * \param rnti maps to gnbUeS1Id
     * \param bearerId Bearer Identity which is to be de-activated
     */
    void DoReleaseIndication(uint64_t imsi, uint16_t rnti, uint8_t bearerId);

    /**
     * Send a packet to the UE via the NR radio interface of the gNB
     *
     * \param packet t
     * \param rnti maps to gnbUeS1Id
     * \param bid the EPS Bearer Identifier
     */
    void SendToNrSocket(Ptr<Packet> packet, uint16_t rnti, uint8_t bid);

    /**
     * Send a packet to the SGW via the S1-U interface
     *
     * \param packet packet to be sent
     * \param teid the Tunnel Endpoint Identifier
     */
    void SendToS1uSocket(Ptr<Packet> packet, uint32_t teid);

    /**
     * internal method used for the actual setup of the S1 Bearer
     *
     * \param teid the Tunnel Endpoint Identifier
     * \param rnti maps to gnbUeS1Id
     * \param bid the S1-U Bearer Identifier
     */
    void SetupS1Bearer(uint32_t teid, uint16_t rnti, uint8_t bid);

    /**
     * raw packet socket to send and receive the packets to and from the NR radio interface
     */
    Ptr<Socket> m_nrSocket;

    /**
     * raw packet socket to send and receive the packets to and from the NR radio interface
     */
    Ptr<Socket> m_nrSocket6;

    /**
     * UDP socket to send and receive GTP-U the packets to and from the S1-U interface
     */
    Ptr<Socket> m_s1uSocket;

    /**
     * address of the gNB for S1-U communications
     */
    Ipv4Address m_gnbS1uAddress;

    /**
     * address of the SGW which terminates all S1-U tunnels
     */
    Ipv4Address m_sgwS1uAddress;

    /**
     * map of maps telling for each RNTI and BID the corresponding  S1-U TEID
     *
     */
    std::map<uint16_t, std::map<uint8_t, uint32_t>> m_rbidTeidMap;

    /**
     * map telling for each S1-U TEID the corresponding RNTI,BID
     *
     */
    std::map<uint32_t, EpsFlowId_t> m_teidRbidMap;

    /**
     * UDP port to be used for GTP
     */
    uint16_t m_gtpuUdpPort;

    /**
     * Provider for the S1 SAP
     */
    NrEpcGnbS1SapProvider* m_s1SapProvider;

    /**
     * User for the S1 SAP
     */
    NrEpcGnbS1SapUser* m_s1SapUser;

    /**
     * MME side of the S1-AP SAP
     *
     */
    NrEpcS1apSapMme* m_s1apSapMme;

    /**
     * gNB side of the S1-AP SAP
     *
     */
    NrEpcS1apSapGnb* m_s1apSapGnb;

    /**
     * UE context info
     *
     */
    std::map<uint64_t, uint16_t> m_imsiRntiMap;

    uint16_t m_cellId; ///< cell ID

    /**
     * \brief Callback to trace RX (reception) data packets from NR socket.
     */
    TracedCallback<Ptr<Packet>> m_rxNrSocketPktTrace;

    /**
     * \brief Callback to trace RX (reception) data packets from S1-U Socket.
     */
    TracedCallback<Ptr<Packet>> m_rxS1uSocketPktTrace;
};

} // namespace ns3

#endif /* NR_EPC_GNB_APPLICATION_H */
