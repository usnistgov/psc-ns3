// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>
//         (based on epc-sgw-pgw-application.h)

#ifndef NR_EPC_PGW_APPLICATION_H
#define NR_EPC_PGW_APPLICATION_H

#include "nr-epc-gtpc-header.h"
#include "nr-epc-tft-classifier.h"

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/virtual-net-device.h"

namespace ns3
{

/**
 * \ingroup nr
 *
 * This application implements the Packet Data Network (PDN) Gateway Entity (PGW)
 * according to the 3GPP TS 23.401 document.
 *
 * This Application implements the PGW side of the S5 interface between
 * the PGW node and the SGW nodes and the PGW side of the SGi interface between
 * the PGW node and the internet hosts. It supports the following functions and messages:
 *
 *  - S5 connectivity (i.e. GTPv2-C signalling and GTP-U data plane)
 *  - Bearer management functions including dedicated bearer establishment
 *  - Per-user based packet filtering
 *  - UL and DL bearer binding
 *  - Tunnel Management messages
 *
 * Others functions enumerated in section 4.4.3.3 of 3GPP TS 23.401 are not supported.
 */
class NrEpcPgwApplication : public Application
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * Constructor that binds the tap device to the callback methods.
     *
     * \param tunDevice TUN VirtualNetDevice used to tunnel IP packets from
     * the SGi interface of the PGW in the internet
     * over GTP-U/UDP/IP on the S5 interface
     * \param s5Addr IP address of the PGW S5 interface
     * \param s5uSocket socket used to send GTP-U packets to the peer SGW
     * \param s5cSocket socket used to send GTP-C packets to the peer SGW
     */
    NrEpcPgwApplication(const Ptr<VirtualNetDevice> tunDevice,
                        Ipv4Address s5Addr,
                        const Ptr<Socket> s5uSocket,
                        const Ptr<Socket> s5cSocket);

    /** Destructor */
    ~NrEpcPgwApplication() override;

    /**
     * Method to be assigned to the callback of the SGi TUN VirtualNetDevice.
     * It is called when the PGW receives a data packet from the
     * internet (including IP headers) that is to be sent to the UE via
     * its associated SGW and gNB, tunneling IP over GTP-U/UDP/IP.
     *
     * \param packet
     * \param source
     * \param dest
     * \param protocolNumber
     * \return true always
     */
    bool RecvFromTunDevice(Ptr<Packet> packet,
                           const Address& source,
                           const Address& dest,
                           uint16_t protocolNumber);

    /**
     * Method to be assigned to the receiver callback of the S5-U socket.
     * It is called when the PGW receives a data packet from the SGW
     * that is to be forwarded to the internet.
     *
     * \param socket pointer to the S5-U socket
     */
    void RecvFromS5uSocket(Ptr<Socket> socket);

    /**
     * Method to be assigned to the receiver callback of the S5-C socket.
     * It is called when the PGW receives a control packet from the SGW.
     *
     * \param socket pointer to the S5-C socket
     */
    void RecvFromS5cSocket(Ptr<Socket> socket);

    /**
     * Send a data packet to the internet via the SGi interface of the PGW
     *
     * \param packet packet to be sent
     * \param teid the Tunnel Endpoint Identifier
     */
    void SendToTunDevice(Ptr<Packet> packet, uint32_t teid);

    /**
     * Send a data packet to the SGW via the S5-U interface
     *
     * \param packet packet to be sent
     * \param sgwS5uAddress the address of the SGW
     * \param teid the Tunnel Endpoint Identifier
     */
    void SendToS5uSocket(Ptr<Packet> packet, Ipv4Address sgwS5uAddress, uint32_t teid);

    /**
     * Let the PGW be aware of a new SGW
     *
     * \param sgwS5Addr the address of the SGW S5 interface
     */
    void AddSgw(Ipv4Address sgwS5Addr);

    /**
     * Let the PGW be aware of a new UE
     *
     * \param imsi the unique identifier of the UE
     */
    void AddUe(uint64_t imsi);

    /**
     * Set the address of a previously added UE
     *
     * \param imsi the unique identifier of the UE
     * \param ueAddr the IPv4 address of the UE
     */
    void SetUeAddress(uint64_t imsi, Ipv4Address ueAddr);

    /**
     * set the address of a previously added UE
     *
     * \param imsi the unique identifier of the UE
     * \param ueAddr the IPv6 address of the UE
     */
    void SetUeAddress6(uint64_t imsi, Ipv6Address ueAddr);

    /**
     * Register a remote UE that connected to an L3 UE-to-Network relay UE
     *
     * \param relayImsi IMSI of the relay UE
     * \param ueAddr The IPv4 address of the remote UE
     */
    void AddRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr);

    /**
     * Deregister a remote UE that previously connected to an L3 UE-to-Network relay UE
     *
     * \param relayImsi IMSI of the relay UE
     * \param ueAddr The IPv4 address of the remote UE
     */
    void RemoveRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr);

    /**
     * TracedCallback signature for data Packet reception event.
     *
     * \param [in] packet The data packet sent from the internet.
     */
    typedef void (*RxTracedCallback)(Ptr<Packet> packet);

  private:
    /**
     * Process Create Session Request message
     * \param packet GTPv2-C Create Session Request message
     */
    void DoRecvCreateSessionRequest(Ptr<Packet> packet);

    /**
     * Process Modify Bearer Request message
     * \param packet GTPv2-C Modify Bearer Request message
     */
    void DoRecvModifyBearerRequest(Ptr<Packet> packet);

    /**
     * Process Delete Bearer Command message
     * \param packet GTPv2-C Delete Bearer Command message
     */
    void DoRecvDeleteBearerCommand(Ptr<Packet> packet);

    /**
     * Process Delete Bearer Response message
     * \param packet GTPv2-C Delete Bearer Response message
     */
    void DoRecvDeleteBearerResponse(Ptr<Packet> packet);

    /**
     * store info for each UE connected to this PGW
     */
    class NrUeInfo : public SimpleRefCount<NrUeInfo>
    {
      public:
        NrUeInfo();

        /**
         * Add a bearer for this UE on PGW side
         *
         * \param bearerId the ID of the EPS Bearer to be activated
         * \param teid  the TEID of the new bearer
         * \param tft the Traffic Flow Template of the new bearer to be added
         */
        void AddBearer(uint8_t bearerId, uint32_t teid, Ptr<NrEpcTft> tft);

        /**
         * Delete context of bearer for this UE on PGW side
         *
         * \param bearerId the ID of the EPS Bearer whose contexts is to be removed
         */
        void RemoveBearer(uint8_t bearerId);

        /**
         * Classify the packet according to TFTs of this UE
         *
         * \param p the IPv4 or IPv6 packet from the internet to be classified
         * \param protocolNumber identifies the type of packet.
         *        Only IPv4 and IPv6 packets are allowed.
         *
         * \return the corresponding bearer ID > 0 identifying the bearer
         * among all the bearers of this UE;  returns 0 if no bearers
         * matches with the previously declared TFTs
         */
        uint32_t Classify(Ptr<Packet> p, uint16_t protocolNumber);

        /**
         * Get the address of the SGW to which the UE is connected
         *
         * \return the address of the SGW
         */
        Ipv4Address GetSgwAddr();

        /**
         * Set the address of the gNB to which the UE is connected
         *
         * \param addr the address of the SGW
         */
        void SetSgwAddr(Ipv4Address addr);

        /**
         * Get the IPv4 address of the UE
         *
         * \return the IPv4 address of the UE
         */
        Ipv4Address GetUeAddr();

        /**
         * Set the IPv4 address of the UE
         *
         * \param addr the IPv4 address of the UE
         */
        void SetUeAddr(Ipv4Address addr);

        /**
         * Get the IPv6 address of the UE
         *
         * \return the IPv6 address of the UE
         */
        Ipv6Address GetUeAddr6();

        /**
         * Set the IPv6 address of the UE
         *
         * \param addr the IPv6 address of the UE
         */
        void SetUeAddr6(Ipv6Address addr);

      private:
        Ipv4Address m_ueAddr;                            ///< UE IPv4 address
        Ipv6Address m_ueAddr6;                           ///< UE IPv6 address
        Ipv4Address m_sgwAddr;                           ///< SGW IPv4 address
        NrEpcTftClassifier m_tftClassifier;              ///< TFT classifier
        std::map<uint8_t, uint32_t> m_teidByBearerIdMap; ///< TEID By bearer ID Map
    };

    /**
     * PGW address of the S5 interface
     */
    Ipv4Address m_pgwS5Addr;

    /**
     * UDP socket to send/receive GTP-U packets to/from the S5 interface
     */
    Ptr<Socket> m_s5uSocket;

    /**
     * UDP socket to send/receive GTPv2-C packets to/from the S5 interface
     */
    Ptr<Socket> m_s5cSocket;

    /**
     * TUN VirtualNetDevice used for tunneling/detunneling IP packets
     * from/to the internet over GTP-U/UDP/IP on the S5 interface
     */
    Ptr<VirtualNetDevice> m_tunDevice;

    /**
     * NrUeInfo stored by UE IPv4 address
     */
    std::map<Ipv4Address, Ptr<NrUeInfo>> m_ueInfoByAddrMap;

    /**
     * NrUeInfo stored by UE IPv6 address
     */
    std::map<Ipv6Address, Ptr<NrUeInfo>> m_ueInfoByAddrMap6;

    /**
     * NrUeInfo stored by IMSI
     */
    std::map<uint64_t, Ptr<NrUeInfo>> m_ueInfoByImsiMap;

    /**
     * L3 UE-to-Network relay UE info indexed by remote UE IPv4 address
     */
    std::map<Ipv4Address, Ptr<NrUeInfo>> m_ueInfoByRemoteUeAddrMap;

    /**
     * UDP port to be used for GTP-U
     */
    uint16_t m_gtpuUdpPort;

    /**
     * UDP port to be used for GTPv2-C
     */
    uint16_t m_gtpcUdpPort;

    /**
     * SGW address of the S5 interface
     */
    Ipv4Address m_sgwS5Addr;

    /**
     * \brief Callback to trace received data packets at Tun NetDevice from internet.
     */
    TracedCallback<Ptr<Packet>> m_rxTunPktTrace;

    /**
     * \brief Callback to trace received data packets from S5 socket.
     */
    TracedCallback<Ptr<Packet>> m_rxS5PktTrace;
};

} // namespace ns3

#endif // NR_EPC_PGW_APPLICATION_H
