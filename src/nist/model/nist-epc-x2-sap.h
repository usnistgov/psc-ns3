/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#ifndef NIST_EPC_X2_SAP_H
#define NIST_EPC_X2_SAP_H

#include "ns3/packet.h"
#include "ns3/nist-eps-bearer.h"
#include "ns3/ipv4-address.h"

#include <bitset>

namespace ns3 {


class Node;

/**
 * The X2 SAP defines the service between the X2 entity and the RRC entity.
 * 
 * The X2 SAP follows the specification 3GPP TS 36.423: "X2 application protocol (X2AP)"
 * 
 * The service primitives corresponds to the X2AP procedures and messages and 
 * the service parameters corresponds to the Information Elements
 * 
 * Note: Any reference in this file refers to the 3GPP TS 36.423 specification
 */

/**
 * \brief Common structures for NistEpcX2SapProvider and NistEpcX2SapUser
 */
class NistEpcX2Sap
{
public:
  virtual ~NistEpcX2Sap ();
  
  /**
   * E-RABs to be setup item as
   * it is used in the HANDOVER REQUEST message.
   * See section 9.1.1.1 for further info about the parameters
   */
  struct NistErabToBeSetupItem
  {
    uint16_t    erabId;
    NistEpsBearer   erabLevelQosParameters;
    bool        dlForwarding;
    Ipv4Address transportLayerAddress;
    uint32_t    gtpTeid;

    NistErabToBeSetupItem ();
  };

  /**
   * E-RABs admitted item as
   * it is used in the HANDOVER REQUEST ACKNOWLEDGE message.
   * See section 9.1.1.2 for further info about the parameters
   */
  struct NistErabAdmittedItem
  {
    uint16_t    erabId;
    uint32_t    ulGtpTeid;
    uint32_t    dlGtpTeid;
  };

  /**
   * E-RABs not admitted item as
   * it is used in the HANDOVER REQUEST ACKNOWLEDGE message.
   * See section 9.1.1.2 for further info about the parameters
   */
  struct NistErabNotAdmittedItem
  {
    uint16_t    erabId;
    uint16_t    cause;
  };

  /**
   * E-RABs subject to status transfer item as
   * it is used in the SN STATUS TRANSFER message.
   * See section 9.1.1.4 for further info about the parameters
   */
  static const uint16_t m_maxPdcpSn = 4096;
  struct NistErabsSubjectToNistStatusTransferItem
  {
    uint16_t            erabId;
    std::bitset<m_maxPdcpSn> receiveNistStatusOfUlPdcpSdus;
    uint16_t            ulPdcpSn;
    uint32_t            ulHfn;
    uint16_t            dlPdcpSn;
    uint32_t            dlHfn;
  };

  /**
   * UL Interference OverloadIndication as
   * it is used in the LOAD INFORMATION message.
   * See section 9.2.17 for further info about the values
   */
  enum UlInterferenceOverloadIndicationItem
  {
    HighInterference,
    MediumInterference,
    LowInterference
  };

  /**
   * UL High Interference Information as
   * it is used in the LOAD INFORMATION message.
   * See section 9.1.2.1 for further info about the parameters
   */
  struct NistUlHighInterferenceInformationItem
  {
    uint16_t    targetCellId;
    std::vector <bool> ulHighInterferenceIndicationList;
  };

  /**
   * Relative Narrowband Tx Power (RNTP) as
   * it is used in the LOAD INFORMATION message.
   * See section 9.2.19 for further info about the parameters
   * 
   * Note: You can use INT16_MIN value for -infinite value
   *       in the rntpThreshold field
   */
  struct NistRelativeNarrowbandTxBand
  {
    std::vector <bool> rntpPerPrbList;
    int16_t     rntpThreshold;
    uint16_t    antennaPorts;
    uint16_t    pB;
    uint16_t    pdcchInterferenceImpact;
  };

  /**
   * Cell Information Item as
   * it is used in the LOAD INFORMATION message.
   * See section 9.1.2.1 for further info about the parameters
   */
  struct NistCellInformationItem
  {
    uint16_t    sourceCellId;
    std::vector <UlInterferenceOverloadIndicationItem> ulInterferenceOverloadIndicationList;
    std::vector <NistUlHighInterferenceInformationItem> ulHighInterferenceInformationList;
    NistRelativeNarrowbandTxBand relativeNarrowbandTxBand;
  };

  /**
   * Load Indicator as
   * it is used in the RESOURCE STATUS UPDATE message.
   * See section 9.2.36 for further info about the value
   */
  enum LoadIndicator
  {
    LowLoad,
    MediumLoad,
    HighLoad,
    Overload
  };

  /**
   * Composite Available Capacity as
   * it is used in the RESOURCE STATUS UPDATE message.
   * See section 9.2.45 for further info about the parameters
   */
  struct NistCompositeAvailCapacity
  {
    uint16_t    cellCapacityClassValue;
    uint16_t    capacityValue;
  };

  /**
   * Cell Measurement Result Item as
   * it is used in the RESOURCE STATUS UPDATE message.
   * See section 9.1.2.14 for further info about the parameters
   */
  struct NistCellMeasurementResultItem
  {
    uint16_t        sourceCellId;

    LoadIndicator   dlHardwareLoadIndicator;
    LoadIndicator   ulHardwareLoadIndicator;

    LoadIndicator   dlS1TnlLoadIndicator;
    LoadIndicator   ulS1TnlLoadIndicator;

    uint16_t        dlGbrPrbUsage;
    uint16_t        ulGbrPrbUsage;
    uint16_t        dlNonGbrPrbUsage;
    uint16_t        ulNonGbrPrbUsage;
    uint16_t        dlTotalPrbUsage;
    uint16_t        ulTotalPrbUsage;

    NistCompositeAvailCapacity  dlCompositeAvailableCapacity;
    NistCompositeAvailCapacity  ulCompositeAvailableCapacity;
  };


  enum IdCause
  {
    HandoverDesirableForRadioReason,
    TimeCriticalHandover
  };


  /**
   * \brief Parameters of the HANDOVER REQUEST message.
   *
   * See section 9.1.1.1 for further info about the parameters
   */
  struct NistHandoverRequestParams
  {
    uint16_t            oldEnbUeX2apId;
    uint16_t            cause;
    uint16_t            sourceCellId;
    uint16_t            targetCellId;
    uint32_t            mmeUeS1apId;
    uint64_t            ueAggregateMaxBitRateDownlink;
    uint64_t            ueAggregateMaxBitRateUplink;
    std::vector <NistErabToBeSetupItem> bearers;
    Ptr<Packet>         rrcContext;
  };

  /**
   * \brief Parameters of the HANDOVER REQUEST ACKNOWLEDGE message.
   *
   * See section 9.1.1.2 for further info about the parameters
   */
  struct NistHandoverRequestAckParams
  {
    uint16_t            oldEnbUeX2apId;
    uint16_t            newEnbUeX2apId;
    uint16_t            sourceCellId;
    uint16_t            targetCellId;
    std::vector <NistErabAdmittedItem> admittedBearers;
    std::vector <NistErabNotAdmittedItem> notAdmittedBearers;
    Ptr<Packet>         rrcContext;
  };

  /**
   * \brief Parameters of the HANDOVER PREPARATION FAILURE message.
   *
   * See section 9.1.1.3 for further info about the parameters
   */
  struct NistHandoverPreparationFailureParams
  {
    uint16_t            oldEnbUeX2apId;
    uint16_t            sourceCellId;
    uint16_t            targetCellId;
    uint16_t            cause;
    uint16_t            criticalityDiagnostics;
  };

  /**
   * \brief Parameters of the SN STATUS TRANSFER message.
   *
   * See section 9.1.1.4 for further info about the parameters
   */
  struct NistSnNistStatusTransferParams
  {
    uint16_t            oldEnbUeX2apId;
    uint16_t            newEnbUeX2apId;
    uint16_t            sourceCellId;
    uint16_t            targetCellId;
    std::vector <NistErabsSubjectToNistStatusTransferItem> erabsSubjectToNistStatusTransferList;
  };

  /**
   * \brief Parameters of the UE CONTEXT RELEASE message.
   *
   * See section 9.1.1.5 for further info about the parameters
   */
  struct NistUeContextReleaseParams
  {
    uint16_t            oldEnbUeX2apId;
    uint16_t            newEnbUeX2apId;
    uint16_t            sourceCellId;
    uint16_t            targetCellId;
  };

  /**
   * \brief Parameters of the LOAD INFORMATION message.
   *
   * See section 9.1.2.1 for further info about the parameters
   */
  struct NistLoadInformationParams
  {
    uint16_t            targetCellId;
    std::vector <NistCellInformationItem> cellInformationList;
  };

  /**
   * \brief Parameters of the RESOURCE STATUS UPDATE message.
   *
   * See section 9.1.2.14 for further info about the parameters
   */
  struct NistResourceNistStatusUpdateParams
  {
    uint16_t            targetCellId;
    uint16_t            enb1MeasurementId;
    uint16_t            enb2MeasurementId;
    std::vector <NistCellMeasurementResultItem> cellMeasurementResultList;
  };

  /**
   * \brief Parameters of the UE DATA primitive
   *
   * Forward UE data during the handover procedure from source eNB (sourceCellId)
   * to target eNB (targetCellId) using a GTP-U tunnel (gtpTeid)
   */
  struct NistUeDataParams
  {
    uint16_t    sourceCellId;
    uint16_t    targetCellId;
    uint32_t    gtpTeid;
    Ptr<Packet> ueData;
  };

};


/**
 * These service primitives of this part of the X2 SAP
 * are provided by the X2 entity and issued by RRC entity
 */
class NistEpcX2SapProvider : public NistEpcX2Sap
{
public:
  virtual ~NistEpcX2SapProvider ();

  /**
   * Service primitives
   */

  virtual void SendHandoverRequest (NistHandoverRequestParams params) = 0;

  virtual void SendHandoverRequestAck (NistHandoverRequestAckParams params) = 0;

  virtual void SendHandoverPreparationFailure (NistHandoverPreparationFailureParams params) = 0;

  virtual void SendSnNistStatusTransfer (NistSnNistStatusTransferParams params) = 0;

  virtual void SendUeContextRelease (NistUeContextReleaseParams params) = 0;

  virtual void SendLoadInformation (NistLoadInformationParams params) = 0;

  virtual void SendResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params) = 0;

  virtual void SendUeData (NistUeDataParams params) = 0;
};


/**
 * These service primitives of this part of the X2 SAP
 * are provided by the RRC entity and issued by the X2 entity
 */
class NistEpcX2SapUser : public NistEpcX2Sap
{
public:
  virtual ~NistEpcX2SapUser ();

  /**
   * Service primitives
   */

  virtual void RecvHandoverRequest (NistHandoverRequestParams params) = 0;

  virtual void RecvHandoverRequestAck (NistHandoverRequestAckParams params) = 0;

  virtual void RecvHandoverPreparationFailure (NistHandoverPreparationFailureParams params) = 0;

  virtual void RecvSnNistStatusTransfer (NistSnNistStatusTransferParams params) = 0;

  virtual void RecvUeContextRelease (NistUeContextReleaseParams params) = 0;

  virtual void RecvLoadInformation (NistLoadInformationParams params) = 0;
  
  virtual void RecvResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params) = 0;

  virtual void RecvUeData (NistUeDataParams params) = 0;
};

///////////////////////////////////////

template <class C>
class NistEpcX2SpecificNistEpcX2SapProvider : public NistEpcX2SapProvider
{
public:
  NistEpcX2SpecificNistEpcX2SapProvider (C* x2);

  //
  // Interface implemented from NistEpcX2SapProvider
  //

  virtual void SendHandoverRequest (NistHandoverRequestParams params);

  virtual void SendHandoverRequestAck (NistHandoverRequestAckParams params);

  virtual void SendHandoverPreparationFailure (NistHandoverPreparationFailureParams params);

  virtual void SendSnNistStatusTransfer (NistSnNistStatusTransferParams params);

  virtual void SendUeContextRelease (NistUeContextReleaseParams params);

  virtual void SendLoadInformation (NistLoadInformationParams params);

  virtual void SendResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params);

  virtual void SendUeData (NistUeDataParams params);

private:
  NistEpcX2SpecificNistEpcX2SapProvider ();
  C* m_x2;
};

template <class C>
NistEpcX2SpecificNistEpcX2SapProvider<C>::NistEpcX2SpecificNistEpcX2SapProvider (C* x2)
  : m_x2 (x2)
{
}

template <class C>
NistEpcX2SpecificNistEpcX2SapProvider<C>::NistEpcX2SpecificNistEpcX2SapProvider ()
{
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendHandoverRequest (NistHandoverRequestParams params)
{
  m_x2->DoSendHandoverRequest (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendHandoverRequestAck (NistHandoverRequestAckParams params)
{
  m_x2->DoSendHandoverRequestAck (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendHandoverPreparationFailure (NistHandoverPreparationFailureParams params)
{
  m_x2->DoSendHandoverPreparationFailure (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendSnNistStatusTransfer (NistSnNistStatusTransferParams params)
{
  m_x2->DoSendSnNistStatusTransfer (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendUeContextRelease (NistUeContextReleaseParams params)
{
  m_x2->DoSendUeContextRelease (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendLoadInformation (NistLoadInformationParams params)
{
  m_x2->DoSendLoadInformation (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params)
{
  m_x2->DoSendResourceNistStatusUpdate (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapProvider<C>::SendUeData (NistUeDataParams params)
{
  m_x2->DoSendUeData (params);
}

///////////////////////////////////////

template <class C>
class NistEpcX2SpecificNistEpcX2SapUser : public NistEpcX2SapUser
{
public:
  NistEpcX2SpecificNistEpcX2SapUser (C* rrc);

  //
  // Interface implemented from NistEpcX2SapUser
  //

  virtual void RecvHandoverRequest (NistHandoverRequestParams params);

  virtual void RecvHandoverRequestAck (NistHandoverRequestAckParams params);

  virtual void RecvHandoverPreparationFailure (NistHandoverPreparationFailureParams params);

  virtual void RecvSnNistStatusTransfer (NistSnNistStatusTransferParams params);

  virtual void RecvUeContextRelease (NistUeContextReleaseParams params);

  virtual void RecvLoadInformation (NistLoadInformationParams params);

  virtual void RecvResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params);

  virtual void RecvUeData (NistUeDataParams params);

private:
  NistEpcX2SpecificNistEpcX2SapUser ();
  C* m_rrc;
};

template <class C>
NistEpcX2SpecificNistEpcX2SapUser<C>::NistEpcX2SpecificNistEpcX2SapUser (C* rrc)
  : m_rrc (rrc)
{
}

template <class C>
NistEpcX2SpecificNistEpcX2SapUser<C>::NistEpcX2SpecificNistEpcX2SapUser ()
{
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvHandoverRequest (NistHandoverRequestParams params)
{
  m_rrc->DoRecvHandoverRequest (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvHandoverRequestAck (NistHandoverRequestAckParams params)
{
  m_rrc->DoRecvHandoverRequestAck (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvHandoverPreparationFailure (NistHandoverPreparationFailureParams params)
{
  m_rrc->DoRecvHandoverPreparationFailure (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvSnNistStatusTransfer (NistSnNistStatusTransferParams params)
{
  m_rrc->DoRecvSnNistStatusTransfer (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvUeContextRelease (NistUeContextReleaseParams params)
{
  m_rrc->DoRecvUeContextRelease (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvLoadInformation (NistLoadInformationParams params)
{
  m_rrc->DoRecvLoadInformation (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvResourceNistStatusUpdate (NistResourceNistStatusUpdateParams params)
{
  m_rrc->DoRecvResourceNistStatusUpdate (params);
}

template <class C>
void
NistEpcX2SpecificNistEpcX2SapUser<C>::RecvUeData (NistUeDataParams params)
{
  m_rrc->DoRecvUeData (params);
}

} // namespace ns3

#endif // NIST_EPC_X2_SAP_H
