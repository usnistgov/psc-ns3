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
 * Author: Lluis Parcerisa <lparcerisa@cttc.cat>
 * Modified by: NIST
 */

#ifndef NIST_RRC_HEADER_H
#define NIST_RRC_HEADER_H

#include "ns3/header.h"

#include <bitset>
#include <string>

#include "ns3/nist-lte-rrc-sap.h"
#include "ns3/nist-lte-asn1-header.h"

namespace ns3 {

/**
 * This class extends NistAsn1Header functions, adding serialization/deserialization
 * of some Information elements defined in 3GPP TS 36.331
 */
class NistRrcAsn1Header : public NistAsn1Header
{
public:
  NistRrcAsn1Header ();
  int GetMessageType ();

protected:
  // Inherited from NistAsn1Header
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  uint32_t Deserialize (Buffer::Iterator bIterator) = 0;
  virtual void PreSerialize (void) const = 0;

  // Serialization functions
  void SerializeNistSrbToAddModList (std::list<NistLteRrcSap::NistSrbToAddMod> srbToAddModList) const;
  void SerializeNistDrbToAddModList (std::list<NistLteRrcSap::NistDrbToAddMod> drbToAddModList) const;
  void SerializeNistLogicalChannelConfig (NistLteRrcSap::NistLogicalChannelConfig logicalChannelConfig) const;
  void SerializeNistRadioResourceConfigDedicated (NistLteRrcSap::NistRadioResourceConfigDedicated radioResourceConfigDedicated) const;
  void SerializeNistPhysicalConfigDedicated (NistLteRrcSap::NistPhysicalConfigDedicated physicalConfigDedicated) const;
  void SerializeNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 systemInformationBlockType1) const;
  void SerializeNistSystemInformationBlockType2 (NistLteRrcSap::NistSystemInformationBlockType2 systemInformationBlockType2) const;
  void SerializeNistRadioResourceConfigCommon (NistLteRrcSap::NistRadioResourceConfigCommon radioResourceConfigCommon) const;
  void SerializeNistRadioResourceConfigCommonSib (NistLteRrcSap::NistRadioResourceConfigCommonSib radioResourceConfigCommonSib) const;
  void SerializeNistMeasResults (NistLteRrcSap::NistMeasResults measResults) const;
  void SerializePlmnIdentity (uint32_t plmnId) const;
  void SerializeNistRachConfigCommon (NistLteRrcSap::NistRachConfigCommon rachConfigCommon) const;
  void SerializeNistMeasConfig (NistLteRrcSap::NistMeasConfig measConfig) const;
  void SerializeQoffsetRange (int8_t qOffsetRange) const;
  void SerializeNistThresholdEutra (NistLteRrcSap::NistThresholdEutra thresholdEutra) const;
  void SerializeSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation slUeInfo) const;
  
  // Deserialization functions
  Buffer::Iterator DeserializeNistDrbToAddModList (std::list<NistLteRrcSap::NistDrbToAddMod> *drbToAddModLis, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistSrbToAddModList (std::list<NistLteRrcSap::NistSrbToAddMod> *srbToAddModList, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistLogicalChannelConfig (NistLteRrcSap::NistLogicalChannelConfig *logicalChannelConfig, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistRadioResourceConfigDedicated (NistLteRrcSap::NistRadioResourceConfigDedicated *radioResourceConfigDedicated, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistPhysicalConfigDedicated (NistLteRrcSap::NistPhysicalConfigDedicated *physicalConfigDedicated, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 *systemInformationBlockType1, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistSystemInformationBlockType2 (NistLteRrcSap::NistSystemInformationBlockType2 *systemInformationBlockType2, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistRadioResourceConfigCommon (NistLteRrcSap::NistRadioResourceConfigCommon *radioResourceConfigCommon, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistRadioResourceConfigCommonSib (NistLteRrcSap::NistRadioResourceConfigCommonSib *radioResourceConfigCommonSib, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistMeasResults (NistLteRrcSap::NistMeasResults *measResults, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializePlmnIdentity (uint32_t *plmnId, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistRachConfigCommon (NistLteRrcSap::NistRachConfigCommon * rachConfigCommon, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistMeasConfig (NistLteRrcSap::NistMeasConfig * measConfig, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeQoffsetRange (int8_t * qOffsetRange, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeNistThresholdEutra (NistLteRrcSap::NistThresholdEutra * thresholdEutra, Buffer::Iterator bIterator);
  Buffer::Iterator DeserializeSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation *slInfo, Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;
  /**
   * This function prints NistRadioResourceConfigDedicated IE, for debugging purposes.
   * @param os The output stream to use (i.e. std::cout)
   * @param radioResourceConfigDedicated The information element to be printed
   */
  void Print (std::ostream &os, NistLteRrcSap::NistRadioResourceConfigDedicated radioResourceConfigDedicated) const;

  /// Stores RRC message type, according to 3GPP TS 36.331
  int m_messageType;
};


/**
 * This class only serves to discriminate which message type has been received
 * in uplink (ue to eNb) for channel DCCH
 */
class NistRrcUlDcchMessage : public NistRrcAsn1Header
{
public:
  NistRrcUlDcchMessage ();
  ~NistRrcUlDcchMessage ();

  // Inherited from NistRrcAsn1Header
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;
  void PreSerialize () const;

protected:
  void SerializeUlDcchMessage (int msgType) const;
  Buffer::Iterator DeserializeUlDcchMessage (Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in downlink (eNb to ue) for channel DCCH
 */
class NistRrcDlDcchMessage : public NistRrcAsn1Header
{
public:
  NistRrcDlDcchMessage ();
  ~NistRrcDlDcchMessage ();

  // Inherited from NistRrcAsn1Header
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;
  void PreSerialize () const;

protected:
  void SerializeDlDcchMessage (int msgType) const;
  Buffer::Iterator DeserializeDlDcchMessage (Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in uplink (ue to eNb) for channel CCCH
 */
class NistRrcUlCcchMessage : public NistRrcAsn1Header
{
public:
  NistRrcUlCcchMessage ();
  ~NistRrcUlCcchMessage ();

  // Inherited from NistRrcAsn1Header 
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;
  void PreSerialize () const;

protected:
  void SerializeUlCcchMessage (int msgType) const;
  Buffer::Iterator DeserializeUlCcchMessage (Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in downlink (eNb to ue) for channel CCCH
 */
class NistRrcDlCcchMessage : public NistRrcAsn1Header
{
public:
  NistRrcDlCcchMessage ();
  ~NistRrcDlCcchMessage ();

  // Inherited from NistRrcAsn1Header 
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;
  void PreSerialize () const;

protected:
  void SerializeDlCcchMessage (int msgType) const;
  Buffer::Iterator DeserializeDlCcchMessage (Buffer::Iterator bIterator);
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionRequest IE
*/
class NistRrcConnectionRequestHeader : public NistRrcUlCcchMessage
{
public:
  NistRrcConnectionRequestHeader ();
  ~NistRrcConnectionRequestHeader ();

  // Inherited from NistRrcAsn1Header 
  static TypeId GetTypeId (void);
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
   * Receives a NistRrcConnectionRequest IE and stores the contents into the class attributes
   * @param msg The information element to parse
   */
  void SetMessage (NistLteRrcSap::NistRrcConnectionRequest msg);

  /**
   * Returns a NistRrcConnectionRequest IE from the values in the class attributes
   * @return A NistRrcConnectionRequest, as defined in NistLteRrcSap
   */
  NistLteRrcSap::NistRrcConnectionRequest GetMessage () const;

  /**
   * Get MMEC attribute
   * @return m_mmec attribute
   */
  std::bitset<8> GetMmec () const;

  /**
   * Get M-TMSI attribute
   * @return m_tmsi attribute
   */
  std::bitset<32> GetMtmsi () const;

private:
  std::bitset<8> m_mmec;
  std::bitset<32> m_mTmsi;
  enum
  {
    EMERGENCY = 0, HIGHPRIORITYACCESS, MT_ACCESS,
    MO_SIGNALLING, MO_DATA, SPARE3, SPARE2, SPARE1
  } m_establishmentCause;
  std::bitset<1> m_spare;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionSetup IE
*/
class NistRrcConnectionSetupHeader : public NistRrcDlCcchMessage
{
public:
  NistRrcConnectionSetupHeader ();
  ~NistRrcConnectionSetupHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionSetup IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */
  void SetMessage (NistLteRrcSap::NistRrcConnectionSetup msg);

  /**
  * Returns a NistRrcConnectionSetup IE from the values in the class attributes
  * @return A NistRrcConnectionSetup, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionSetup GetMessage () const;

  /**
  * Getter for m_rrcTransactionIdentifier
  * @return m_rrcTransactionIdentifier
  */
  uint8_t GetRrcTransactionIdentifier () const;

  /**
  * Getter for m_radioResourceConfigDedicated
  * @return m_radioResourceConfigDedicated
  */
  NistLteRrcSap::NistRadioResourceConfigDedicated GetNistRadioResourceConfigDedicated () const; 

  /**
  * Gets m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated
  * @return m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated
  */
  bool HaveNistPhysicalConfigDedicated () const;

  /**
  * Gets m_radioResourceConfigDedicated.physicalConfigDedicated
  * @return m_radioResourceConfigDedicated.physicalConfigDedicated
  */
  NistLteRrcSap::NistPhysicalConfigDedicated GetNistPhysicalConfigDedicated () const;

  /**
  * Gets m_radioResourceConfigDedicated.srbToAddModList
  * @return m_radioResourceConfigDedicated.srbToAddModList
  */
  std::list<NistLteRrcSap::NistSrbToAddMod> GetNistSrbToAddModList () const;

  /**
  * Gets m_radioResourceConfigDedicated.drbToAddModList
  * @return m_radioResourceConfigDedicated.drbToAddModList
  */
  std::list<NistLteRrcSap::NistDrbToAddMod> GetNistDrbToAddModList () const;

  /**
  * Gets m_radioResourceConfigDedicated.drbToReleaseList
  * @return m_radioResourceConfigDedicated.drbToReleaseList
  */
  std::list<uint8_t> GetDrbToReleaseList () const;

private:
  uint8_t m_rrcTransactionIdentifier;
  mutable NistLteRrcSap::NistRadioResourceConfigDedicated m_radioResourceConfigDedicated;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionSetupComplete IE
*/
class NistRrcConnectionSetupCompleteHeader : public NistRrcUlDcchMessage
{
public:
  NistRrcConnectionSetupCompleteHeader ();
  ~NistRrcConnectionSetupCompleteHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionSetupCompleted IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */
  void SetMessage (NistLteRrcSap::NistRrcConnectionSetupCompleted msg);

  /**
  * Returns a NistRrcConnectionSetupCompleted IE from the values in the class attributes
  * @return A NistRrcConnectionSetupCompleted, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionSetupCompleted GetMessage () const;

  /**
  * Getter for m_rrcTransactionIdentifier
  * @return m_rrcTransactionIdentifier
  */
  uint8_t GetRrcTransactionIdentifier () const;

private:
  uint8_t m_rrcTransactionIdentifier;

};

/**
* This class manages the serialization/deserialization of NistRrcConnectionSetupComplete IE
*/
class NistRrcConnectionReconfigurationCompleteHeader : public NistRrcUlDcchMessage
{
public:
  NistRrcConnectionReconfigurationCompleteHeader ();
  ~NistRrcConnectionReconfigurationCompleteHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReconfigurationCompleted IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */
  void SetMessage (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg);

  /**
  * Returns a NistRrcConnectionReconfigurationCompleted IE from the values in the class attributes
  * @return A NistRrcConnectionReconfigurationCompleted, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReconfigurationCompleted GetMessage () const;

  /**
  * Getter for m_rrcTransactionIdentifier
  * @return m_rrcTransactionIdentifier
  */ 
  uint8_t GetRrcTransactionIdentifier () const;

private:
  uint8_t m_rrcTransactionIdentifier;

};

/**
* This class manages the serialization/deserialization of NistRrcConnectionReconfiguration IE
*/
class NistRrcConnectionReconfigurationHeader : public NistRrcDlDcchMessage
{
public:
  NistRrcConnectionReconfigurationHeader ();
  ~NistRrcConnectionReconfigurationHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReconfiguration IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReconfiguration msg);

  /**
  * Returns a NistRrcConnectionReconfiguration IE from the values in the class attributes
  * @return A NistRrcConnectionReconfiguration, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReconfiguration GetMessage () const; 

  /**
  * Getter for m_haveNistMeasConfig
  * @return m_haveNistMeasConfig
  */
  bool GetHaveNistMeasConfig ();

  /**
  * Getter for m_measConfig
  * @return m_measConfig
  */
  NistLteRrcSap::NistMeasConfig GetNistMeasConfig ();

  /**
  * Getter for m_haveNistMobilityControlInfo
  * @return m_haveNistMobilityControlInfo
  */
  bool GetHaveNistMobilityControlInfo ();

  /**
  * Getter for m_mobilityControlInfo
  * @return m_mobilityControlInfo
  */
  NistLteRrcSap::NistMobilityControlInfo GetNistMobilityControlInfo ();

  /**
  * Getter for m_haveNistRadioResourceConfigDedicated
  * @return m_haveNistRadioResourceConfigDedicated
  */
  bool GetHaveNistRadioResourceConfigDedicated ();

  /**
  * Getter for m_radioResourceConfigDedicated
  * @return m_radioResourceConfigDedicated
  */
  NistLteRrcSap::NistRadioResourceConfigDedicated GetNistRadioResourceConfigDedicated ();

  /**
  * Getter for m_rrcTransactionIdentifier
  * @return m_rrcTransactionIdentifier
  */
  uint8_t GetRrcTransactionIdentifier () const;

  /**
  * Getter for m_radioResourceConfigDedicated
  * @return m_radioResourceConfigDedicated
  */
  NistLteRrcSap::NistRadioResourceConfigDedicated GetNistRadioResourceConfigDedicated () const; 

  /**
  * Gets m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated
  * @return m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated
  */
  bool HaveNistPhysicalConfigDedicated () const;

  /**
  * Gets m_radioResourceConfigDedicated.physicalConfigDedicated
  * @return m_radioResourceConfigDedicated.physicalConfigDedicated
  */
  NistLteRrcSap::NistPhysicalConfigDedicated GetNistPhysicalConfigDedicated () const;

  /**
  * Gets m_radioResourceConfigDedicated.srbToAddModList
  * @return m_radioResourceConfigDedicated.srbToAddModList
  */
  std::list<NistLteRrcSap::NistSrbToAddMod> GetNistSrbToAddModList () const;

  /**
  * Gets m_radioResourceConfigDedicated.drbToAddModList
  * @return m_radioResourceConfigDedicated.drbToAddModList
  */
  std::list<NistLteRrcSap::NistDrbToAddMod> GetNistDrbToAddModList () const;

  /**
  * Gets m_radioResourceConfigDedicated.drbToReleaseList
  * @return m_radioResourceConfigDedicated.drbToReleaseList
  */
  std::list<uint8_t> GetDrbToReleaseList () const;

private:
  uint8_t m_rrcTransactionIdentifier;
  bool m_haveNistMeasConfig;
  NistLteRrcSap::NistMeasConfig m_measConfig;
  bool m_haveNistMobilityControlInfo;
  NistLteRrcSap::NistMobilityControlInfo m_mobilityControlInfo;
  bool m_haveNistRadioResourceConfigDedicated;
  NistLteRrcSap::NistRadioResourceConfigDedicated m_radioResourceConfigDedicated;
};

/**
* This class manages the serialization/deserialization of NistHandoverPreparationInfo IE
*/
class NistHandoverPreparationInfoHeader : public NistRrcAsn1Header
{
public:
  NistHandoverPreparationInfoHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistHandoverPreparationInfo IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistHandoverPreparationInfo msg);

  /**
  * Returns a NistHandoverPreparationInfo IE from the values in the class attributes
  * @return A NistHandoverPreparationInfo, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistHandoverPreparationInfo GetMessage () const;

  /**
  * Getter for m_asConfig
  * @return m_asConfig
  */ 
  NistLteRrcSap::NistAsConfig GetNistAsConfig () const;

private:
  NistLteRrcSap::NistAsConfig m_asConfig;
};

/**
* This class manages the serialization/deserialization of RRCConnectionReestablishmentRequest IE
*/
class NistRrcConnectionReestablishmentRequestHeader : public NistRrcUlCcchMessage
{
public:
  NistRrcConnectionReestablishmentRequestHeader ();
  ~NistRrcConnectionReestablishmentRequestHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReestablishmentRequest IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg);

  /**
  * Returns a NistRrcConnectionReestablishmentRequest IE from the values in the class attributes
  * @return A NistRrcConnectionReestablishmentRequest, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReestablishmentRequest GetMessage () const;

  /**
  * Getter for m_ueIdentity
  * @return m_ueIdentity
  */
  NistLteRrcSap::NistReestabUeIdentity GetUeIdentity () const;

  /**
  * Getter for m_reestablishmentCause
  * @return m_reestablishmentCause
  */
  NistLteRrcSap::ReestablishmentCause GetReestablishmentCause () const;

private:
  NistLteRrcSap::NistReestabUeIdentity m_ueIdentity;
  NistLteRrcSap::ReestablishmentCause m_reestablishmentCause;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionReestablishment IE
*/
class NistRrcConnectionReestablishmentHeader : public NistRrcDlCcchMessage
{
public:
  NistRrcConnectionReestablishmentHeader ();
  ~NistRrcConnectionReestablishmentHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReestablishment IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReestablishment msg);

  /**
  * Returns a NistRrcConnectionReestablishment IE from the values in the class attributes
  * @return A NistRrcConnectionReestablishment, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReestablishment GetMessage () const;

  /**
  * Getter for m_rrcTransactionIdentifier attribute
  * @return m_rrcTransactionIdentifier
  */
  uint8_t GetRrcTransactionIdentifier () const;

  /**
  * Getter for m_radioResourceConfigDedicated attribute
  * @return m_radioResourceConfigDedicated
  */
  NistLteRrcSap::NistRadioResourceConfigDedicated GetNistRadioResourceConfigDedicated () const;

private:
  uint8_t m_rrcTransactionIdentifier;
  NistLteRrcSap::NistRadioResourceConfigDedicated m_radioResourceConfigDedicated;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionReestablishmentComplete IE
*/
class NistRrcConnectionReestablishmentCompleteHeader : public NistRrcUlDcchMessage
{
public:
  NistRrcConnectionReestablishmentCompleteHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReestablishmentComplete IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg);

  /**
  * Returns a NistRrcConnectionReestablishmentComplete IE from the values in the class attributes
  * @return A NistRrcConnectionReestablishmentComplete, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReestablishmentComplete GetMessage () const;

  /**
  * Getter for m_rrcTransactionIdentifier attribute
  * @return m_rrcTransactionIdentifier
  */
  uint8_t GetRrcTransactionIdentifier () const;

private:
  uint8_t m_rrcTransactionIdentifier;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionReestablishmentReject IE
*/
class NistRrcConnectionReestablishmentRejectHeader : public NistRrcDlCcchMessage
{
public:
  NistRrcConnectionReestablishmentRejectHeader ();
  ~NistRrcConnectionReestablishmentRejectHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReestablishmentReject IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentReject msg);

  /**
  * Returns a NistRrcConnectionReestablishmentReject IE from the values in the class attributes
  * @return A NistRrcConnectionReestablishmentReject, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReestablishmentReject GetMessage () const;

private:
  NistLteRrcSap::NistRrcConnectionReestablishmentReject m_rrcConnectionReestablishmentReject;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionRelease IE
*/
class NistRrcConnectionReleaseHeader : public NistRrcDlDcchMessage
{
public:
  NistRrcConnectionReleaseHeader ();
  ~NistRrcConnectionReleaseHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionRelease IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionRelease msg);

  /**
  * Returns a NistRrcConnectionRelease IE from the values in the class attributes
  * @return A NistRrcConnectionRelease, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionRelease GetMessage () const;

private:
  NistLteRrcSap::NistRrcConnectionRelease m_rrcConnectionRelease;
};

/**
* This class manages the serialization/deserialization of NistRrcConnectionReject IE
*/
class NistRrcConnectionRejectHeader : public NistRrcDlCcchMessage
{
public:
  NistRrcConnectionRejectHeader ();
  ~NistRrcConnectionRejectHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistRrcConnectionReject IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */ 
  void SetMessage (NistLteRrcSap::NistRrcConnectionReject msg);

  /**
  * Returns a NistRrcConnectionReject IE from the values in the class attributes
  * @return A NistRrcConnectionReject, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistRrcConnectionReject GetMessage () const;

private:
  NistLteRrcSap::NistRrcConnectionReject m_rrcConnectionReject;
};

/**
* This class manages the serialization/deserialization of NistMeasurementReport IE
*/
class NistMeasurementReportHeader : public NistRrcUlDcchMessage
{
public:
  NistMeasurementReportHeader ();
  ~NistMeasurementReportHeader ();

  // Inherited from NistRrcAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a NistMeasurementReport IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */
  void SetMessage (NistLteRrcSap::NistMeasurementReport msg);

  /**
  * Returns a NistMeasurementReport IE from the values in the class attributes
  * @return A NistMeasurementReport, as defined in NistLteRrcSap
  */
  NistLteRrcSap::NistMeasurementReport GetMessage () const;

private:
  NistLteRrcSap::NistMeasurementReport m_measurementReport;

};

/**
* This class manages the serialization/deserialization of NistMeasurementReport IE
*/
class SidelinkUeInformationHeader : public NistRrcUlDcchMessage
{
public:
  SidelinkUeInformationHeader ();
  ~SidelinkUeInformationHeader ();

  // Inherited from RrcNistAsn1Header 
  void PreSerialize () const;
  uint32_t Deserialize (Buffer::Iterator bIterator);
  void Print (std::ostream &os) const;

  /**
  * Receives a SidelinkUeInformation IE and stores the contents into the class attributes
  * @param msg The information element to parse
  */
  void SetMessage (NistLteRrcSap::SidelinkUeInformation msg);

  /**
  * Returns a SidelinkUeInformation IE from the values in the class attributes
  * @return A SidelinkUeInformation, as defined in NistLteRrcSap
  */
  NistLteRrcSap::SidelinkUeInformation GetMessage () const;

private:
  NistLteRrcSap::SidelinkUeInformation m_sidelinkUeInformation;

};
  

} // namespace ns3

#endif // NIST_RRC_HEADER_H

