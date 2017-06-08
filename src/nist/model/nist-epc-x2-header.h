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

#ifndef NIST_EPC_X2_HEADER_H
#define NIST_EPC_X2_HEADER_H

#include "ns3/nist-epc-x2-sap.h"
#include "ns3/header.h"

#include <vector>


namespace ns3 {


class NistEpcX2Header : public Header
{
public:
  NistEpcX2Header ();
  virtual ~NistEpcX2Header ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint8_t GetMessageType () const;
  void SetMessageType (uint8_t messageType);

  uint8_t GetProcedureCode () const;
  void SetProcedureCode (uint8_t procedureCode);

  void SetLengthOfIes (uint32_t lengthOfIes);
  void SetNumberOfIes (uint32_t numberOfIes);


  enum ProcedureCode_t {
    HandoverPreparation     = 0,
    LoadIndication          = 2,
    SnNistStatusTransfer        = 4,
    UeContextRelease        = 5,
    ResourceNistStatusReporting = 10
  };

  enum TypeOfMessage_t {
    InitiatingMessage       = 0,
    SuccessfulOutcome       = 1,
    UnsuccessfulOutcome     = 2
  };

private:
  uint8_t m_messageType;
  uint8_t m_procedureCode;

  uint32_t m_lengthOfIes;
  uint32_t m_numberOfIes;
};


class NistEpcX2HandoverRequestHeader : public Header
{
public:
  NistEpcX2HandoverRequestHeader ();
  virtual ~NistEpcX2HandoverRequestHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetOldEnbUeX2apId () const;
  void SetOldEnbUeX2apId (uint16_t x2apId);

  uint16_t GetCause () const;
  void SetCause (uint16_t cause);

  uint16_t GetTargetCellId () const;
  void SetTargetCellId (uint16_t targetCellId);

  uint32_t GetMmeUeS1apId () const;
  void SetMmeUeS1apId (uint32_t mmeUeS1apId);

  std::vector <NistEpcX2Sap::NistErabToBeSetupItem> GetBearers () const;
  void SetBearers (std::vector <NistEpcX2Sap::NistErabToBeSetupItem> bearers);

  uint64_t GetUeAggregateMaxBitRateDownlink () const;
  void SetUeAggregateMaxBitRateDownlink (uint64_t bitRate);

  uint64_t GetUeAggregateMaxBitRateUplink () const;
  void SetUeAggregateMaxBitRateUplink (uint64_t bitRate);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_oldEnbUeX2apId;
  uint16_t          m_cause;
  uint16_t          m_targetCellId;
  uint32_t          m_mmeUeS1apId;
  uint64_t          m_ueAggregateMaxBitRateDownlink;
  uint64_t          m_ueAggregateMaxBitRateUplink;
  std::vector <NistEpcX2Sap::NistErabToBeSetupItem> m_erabsToBeSetupList;
};


class NistEpcX2HandoverRequestAckHeader : public Header
{
public:
  NistEpcX2HandoverRequestAckHeader ();
  virtual ~NistEpcX2HandoverRequestAckHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetOldEnbUeX2apId () const;
  void SetOldEnbUeX2apId (uint16_t x2apId);

  uint16_t GetNewEnbUeX2apId () const;
  void SetNewEnbUeX2apId (uint16_t x2apId);

  std::vector <NistEpcX2Sap::NistErabAdmittedItem> GetAdmittedBearers () const;
  void SetAdmittedBearers (std::vector <NistEpcX2Sap::NistErabAdmittedItem> bearers);

  std::vector <NistEpcX2Sap::NistErabNotAdmittedItem> GetNotAdmittedBearers () const;
  void SetNotAdmittedBearers (std::vector <NistEpcX2Sap::NistErabNotAdmittedItem> bearers);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_oldEnbUeX2apId;
  uint16_t          m_newEnbUeX2apId;
  std::vector <NistEpcX2Sap::NistErabAdmittedItem>     m_erabsAdmittedList;
  std::vector <NistEpcX2Sap::NistErabNotAdmittedItem>  m_erabsNotAdmittedList;
};


class NistEpcX2HandoverPreparationFailureHeader : public Header
{
public:
  NistEpcX2HandoverPreparationFailureHeader ();
  virtual ~NistEpcX2HandoverPreparationFailureHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetOldEnbUeX2apId () const;
  void SetOldEnbUeX2apId (uint16_t x2apId);

  uint16_t GetCause () const;
  void SetCause (uint16_t cause);

  uint16_t GetCriticalityDiagnostics () const;
  void SetCriticalityDiagnostics (uint16_t criticalityDiagnostics);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_oldEnbUeX2apId;
  uint16_t          m_cause;
  uint16_t          m_criticalityDiagnostics;
};


class NistEpcX2SnNistStatusTransferHeader : public Header
{
public:
  NistEpcX2SnNistStatusTransferHeader ();
  virtual ~NistEpcX2SnNistStatusTransferHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetOldEnbUeX2apId () const;
  void SetOldEnbUeX2apId (uint16_t x2apId);

  uint16_t GetNewEnbUeX2apId () const;
  void SetNewEnbUeX2apId (uint16_t x2apId);

  std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem> GetErabsSubjectToNistStatusTransferList () const;
  void SetErabsSubjectToNistStatusTransferList (std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem> erabs);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_oldEnbUeX2apId;
  uint16_t          m_newEnbUeX2apId;
  std::vector <NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem> m_erabsSubjectToNistStatusTransferList;
};


class NistEpcX2UeContextReleaseHeader : public Header
{
public:
  NistEpcX2UeContextReleaseHeader ();
  virtual ~NistEpcX2UeContextReleaseHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetOldEnbUeX2apId () const;
  void SetOldEnbUeX2apId (uint16_t x2apId);

  uint16_t GetNewEnbUeX2apId () const;
  void SetNewEnbUeX2apId (uint16_t x2apId);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_oldEnbUeX2apId;
  uint16_t          m_newEnbUeX2apId;
};


class NistEpcX2LoadInformationHeader : public Header
{
public:
  NistEpcX2LoadInformationHeader ();
  virtual ~NistEpcX2LoadInformationHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  std::vector <NistEpcX2Sap::NistCellInformationItem> GetCellInformationList () const;
  void SetCellInformationList (std::vector <NistEpcX2Sap::NistCellInformationItem> cellInformationList);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  std::vector <NistEpcX2Sap::NistCellInformationItem> m_cellInformationList;
};


class NistEpcX2ResourceNistStatusUpdateHeader : public Header
{
public:
  NistEpcX2ResourceNistStatusUpdateHeader ();
  virtual ~NistEpcX2ResourceNistStatusUpdateHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;


  uint16_t GetEnb1MeasurementId () const;
  void SetEnb1MeasurementId (uint16_t enb1MeasurementId);

  uint16_t GetEnb2MeasurementId () const;
  void SetEnb2MeasurementId (uint16_t enb2MeasurementId);

  std::vector <NistEpcX2Sap::NistCellMeasurementResultItem> GetCellMeasurementResultList () const;
  void SetCellMeasurementResultList (std::vector <NistEpcX2Sap::NistCellMeasurementResultItem> cellMeasurementResultList);

  uint32_t GetLengthOfIes () const;
  uint32_t GetNumberOfIes () const;

private:
  uint32_t          m_numberOfIes;
  uint32_t          m_headerLength;

  uint16_t          m_enb1MeasurementId;
  uint16_t          m_enb2MeasurementId;
  std::vector <NistEpcX2Sap::NistCellMeasurementResultItem> m_cellMeasurementResultList;
};


} // namespace ns3

#endif // NIST_EPC_X2_HEADER_H
