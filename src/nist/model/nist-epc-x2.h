/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NIST_EPC_X2_H
#define NIST_EPC_X2_H

#include "ns3/socket.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/object.h"

#include "ns3/nist-epc-x2-sap.h"

#include <map>

namespace ns3 {


class NistX2IfaceInfo : public SimpleRefCount<NistX2IfaceInfo>
{
public:
  NistX2IfaceInfo (Ipv4Address remoteIpAddr, Ptr<Socket> localCtrlPlaneSocket, Ptr<Socket> localUserPlaneSocket);
  virtual ~NistX2IfaceInfo (void);

  NistX2IfaceInfo& operator= (const NistX2IfaceInfo &);

public:
  Ipv4Address   m_remoteIpAddr;
  Ptr<Socket>   m_localCtrlPlaneSocket;
  Ptr<Socket>   m_localUserPlaneSocket;
};


class NistX2CellInfo : public SimpleRefCount<NistX2CellInfo>
{
public:
  NistX2CellInfo (uint16_t localCellId, uint16_t remoteCellId);
  virtual ~NistX2CellInfo (void);

  NistX2CellInfo& operator= (const NistX2CellInfo &);

public:
  uint16_t m_localCellId;
  uint16_t m_remoteCellId;
};


/**
 * \ingroup lte
 *
 * This entity is installed inside an eNB and provides the functionality for the X2 interface
 */
class NistEpcX2 : public Object
{
  friend class NistEpcX2SpecificNistEpcX2SapProvider<NistEpcX2>;

public:
  /** 
   * Constructor
   */
  NistEpcX2 ();

  /**
   * Destructor
   */
  virtual ~NistEpcX2 (void);

  static TypeId GetTypeId (void);
  virtual void DoDispose (void);


  /**
   * \param s the X2 SAP User to be used by this EPC X2 entity
   */
  void SetNistEpcX2SapUser (NistEpcX2SapUser * s);

  /**
   * \return the X2 SAP Provider interface offered by this EPC X2 entity
   */
  NistEpcX2SapProvider* GetNistEpcX2SapProvider ();


  /**
   * Add an X2 interface to this EPC X2 entity
   * \param enb1CellId the cell ID of the current eNodeB
   * \param enb1X2Address the address of the current eNodeB
   * \param enb2CellId the cell ID of the neighbouring eNodeB
   * \param enb2X2Address the address of the neighbouring eNodeB
   */
  void AddX2Interface (uint16_t enb1CellId, Ipv4Address enb1X2Address,
                       uint16_t enb2CellId, Ipv4Address enb2X2Address);


  /** 
   * Method to be assigned to the recv callback of the X2-C (X2 Control Plane) socket.
   * It is called when the eNB receives a packet from the peer eNB of the X2-C interface
   * 
   * \param socket socket of the X2-C interface
   */
  void RecvFromX2cSocket (Ptr<Socket> socket);

  /** 
   * Method to be assigned to the recv callback of the X2-U (X2 User Plane) socket.
   * It is called when the eNB receives a packet from the peer eNB of the X2-U interface
   * 
   * \param socket socket of the X2-U interface
   */
  void RecvFromX2uSocket (Ptr<Socket> socket);


protected:
  // Interface provided by NistEpcX2SapProvider
  virtual void DoSendHandoverRequest (NistEpcX2SapProvider::NistHandoverRequestParams params);
  virtual void DoSendHandoverRequestAck (NistEpcX2SapProvider::NistHandoverRequestAckParams params);
  virtual void DoSendHandoverPreparationFailure (NistEpcX2SapProvider::NistHandoverPreparationFailureParams params);
  virtual void DoSendSnNistStatusTransfer (NistEpcX2SapProvider::NistSnNistStatusTransferParams params);
  virtual void DoSendUeContextRelease (NistEpcX2SapProvider::NistUeContextReleaseParams params);
  virtual void DoSendLoadInformation (NistEpcX2SapProvider::NistLoadInformationParams params);
  virtual void DoSendResourceNistStatusUpdate (NistEpcX2SapProvider::NistResourceNistStatusUpdateParams params);
  virtual void DoSendUeData (NistEpcX2SapProvider::NistUeDataParams params);

  NistEpcX2SapUser* m_x2SapUser;
  NistEpcX2SapProvider* m_x2SapProvider;


private:

  /**
   * Map the targetCellId to the corresponding (sourceSocket, remoteIpAddr) to be used
   * to send the X2 message
   */
  std::map < uint16_t, Ptr<NistX2IfaceInfo> > m_x2InterfaceSockets;

  /**
   * Map the localSocket (the one receiving the X2 message) 
   * to the corresponding (sourceCellId, targetCellId) associated with the X2 interface
   */
  std::map < Ptr<Socket>, Ptr<NistX2CellInfo> > m_x2InterfaceCellIds;

  /**
   * UDP ports to be used for the X2 interfaces: X2-C and X2-U
   */
  uint16_t m_x2cUdpPort;
  uint16_t m_x2uUdpPort;

};

} //namespace ns3

#endif // NIST_EPC_X2_H
