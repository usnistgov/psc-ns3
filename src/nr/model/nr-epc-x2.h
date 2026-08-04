// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_EPC_X2_H
#define NR_EPC_X2_H

#include "nr-epc-x2-sap.h"

#include "ns3/callback.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"

#include <map>

namespace ns3
{

/**
 * NrX2IfaceInfo
 */
class NrX2IfaceInfo : public SimpleRefCount<NrX2IfaceInfo>
{
  public:
    /**
     * Constructor
     *
     * \param remoteIpAddr remote IP address
     * \param localCtrlPlaneSocket control plane socket
     * \param localUserPlaneSocket user plane socket
     */
    NrX2IfaceInfo(Ipv4Address remoteIpAddr,
                  Ptr<Socket> localCtrlPlaneSocket,
                  Ptr<Socket> localUserPlaneSocket);
    virtual ~NrX2IfaceInfo();

    /**
     * Assignment operator
     * \param value value to assign
     * \returns NrX2IfaceInfo&
     */
    NrX2IfaceInfo& operator=(const NrX2IfaceInfo& value);

  public:
    Ipv4Address m_remoteIpAddr;         ///< remote IP address
    Ptr<Socket> m_localCtrlPlaneSocket; ///< local control plane socket
    Ptr<Socket> m_localUserPlaneSocket; ///< local user plane socket
};

/**
 * NrX2CellInfo
 */
class NrX2CellInfo : public SimpleRefCount<NrX2CellInfo>
{
  public:
    /**
     * Constructor
     *
     * \param localCellIds local cell IDs
     * \param remoteCellIds remote cell IDs
     */
    NrX2CellInfo(std::vector<uint16_t> localCellIds, std::vector<uint16_t> remoteCellIds);
    virtual ~NrX2CellInfo();

    /**
     * Assignment operator
     * \param value value to assign
     * \returns NrX2CellInfo&
     */
    NrX2CellInfo& operator=(const NrX2CellInfo& value);

  public:
    std::vector<uint16_t> m_localCellIds;  ///< local cell IDs
    std::vector<uint16_t> m_remoteCellIds; ///< remote cell IDs
};

/**
 * \ingroup nr
 *
 * This entity is installed inside an gNB and provides the functionality for the X2 interface
 */
class NrEpcX2 : public Object
{
    /// allow NrEpcX2SpecificEpcX2SapProvider<NrEpcX2> class friend access
    friend class NrEpcX2SpecificEpcX2SapProvider<NrEpcX2>;

  public:
    /**
     * Constructor
     */
    NrEpcX2();

    /**
     * Destructor
     */
    ~NrEpcX2() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * \param s the X2 SAP User to be used by this EPC X2 entity
     */
    void SetEpcX2SapUser(NrEpcX2SapUser* s);

    /**
     * \return the X2 SAP Provider interface offered by this EPC X2 entity
     */
    NrEpcX2SapProvider* GetEpcX2SapProvider();

    /**
     * Add an X2 interface to this EPC X2 entity
     * \param gnb1CellId the cell ID of the current eNodeB
     * \param gnb1X2Address the address of the current eNodeB
     * \param gnb2CellIds the cell IDs of the neighbouring eNodeB
     * \param gnb2X2Address the address of the neighbouring eNodeB
     */
    void AddX2Interface(uint16_t gnb1CellId,
                        Ipv4Address gnb1X2Address,
                        std::vector<uint16_t> gnb2CellIds,
                        Ipv4Address gnb2X2Address);

    /**
     * Method to be assigned to the recv callback of the X2-C (X2 Control Plane) socket.
     * It is called when the gNB receives a packet from the peer gNB of the X2-C interface
     *
     * \param socket socket of the X2-C interface
     */
    void RecvFromX2cSocket(Ptr<Socket> socket);

    /**
     * Method to be assigned to the recv callback of the X2-U (X2 User Plane) socket.
     * It is called when the gNB receives a packet from the peer gNB of the X2-U interface
     *
     * \param socket socket of the X2-U interface
     */
    void RecvFromX2uSocket(Ptr<Socket> socket);

  protected:
    // Interface provided by NrEpcX2SapProvider
    /**
     * Send handover request function
     * \param params the send handover request parameters
     */
    virtual void DoSendHandoverRequest(NrEpcX2SapProvider::HandoverRequestParams params);
    /**
     * Send handover request ack function
     * \param params the send handover request ack parameters
     */
    virtual void DoSendHandoverRequestAck(NrEpcX2SapProvider::HandoverRequestAckParams params);
    /**
     * Send handover preparation failure function
     * \param params the handover preparation failure parameters
     */
    virtual void DoSendHandoverPreparationFailure(
        NrEpcX2SapProvider::HandoverPreparationFailureParams params);
    /**
     * Send SN status transfer function
     * \param params the SN status transfer parameters
     */
    virtual void DoSendSnStatusTransfer(NrEpcX2SapProvider::SnStatusTransferParams params);
    /**
     * Send UE context release function
     * \param params the UE context release parameters
     */
    virtual void DoSendUeContextRelease(NrEpcX2SapProvider::UeContextReleaseParams params);
    /**
     * Send load information function
     * \param params the send load information parameters
     */
    virtual void DoSendLoadInformation(NrEpcX2SapProvider::LoadInformationParams params);
    /**
     * Send resource status update function
     * \param params the send resource status update parameters
     */
    virtual void DoSendResourceStatusUpdate(NrEpcX2SapProvider::ResourceStatusUpdateParams params);
    /**
     * Send UE data function
     *
     * \param params NrEpcX2SapProvider::UeDataParams
     */
    virtual void DoSendUeData(NrEpcX2SapProvider::UeDataParams params);
    /**
     * \brief Send Handover Cancel function
     * \param params the handover cancel parameters
     *
     */
    virtual void DoSendHandoverCancel(NrEpcX2SapProvider::HandoverCancelParams params);

    NrEpcX2SapUser* m_x2SapUser;         ///< X2 SAP user
    NrEpcX2SapProvider* m_x2SapProvider; ///< X2 SAP provider

  private:
    /**
     * Map the targetCellId to the corresponding (sourceSocket, remoteIpAddr) to be used
     * to send the X2 message
     */
    std::map<uint16_t, Ptr<NrX2IfaceInfo>> m_x2InterfaceSockets;

    /**
     * Map the localSocket (the one receiving the X2 message)
     * to the corresponding (sourceCellId, targetCellId) associated with the X2 interface
     */
    std::map<Ptr<Socket>, Ptr<NrX2CellInfo>> m_x2InterfaceCellIds;

    /**
     * UDP ports to be used for the X2-C interface
     */
    uint16_t m_x2cUdpPort;
    /**
     * UDP ports to be used for the X2-U interface
     */
    uint16_t m_x2uUdpPort;
};

} // namespace ns3

#endif // NR_EPC_X2_H
