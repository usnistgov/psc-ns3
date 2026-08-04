// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Lluis Parcerisa <lparcerisa@cttc.cat>
// Modified by:
//          Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
//          Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)

#ifndef NR_RRC_HEADER_H
#define NR_RRC_HEADER_H

#include "nr-asn1-header.h"
#include "nr-rrc-sap.h"

#include "ns3/header.h"

#include <bitset>
#include <string>

namespace ns3
{

/**
 * \ingroup nr
 */

/**
 * This class extends NrAsn1Header functions, adding serialization/deserialization
 * of some Information elements defined in 3GPP TS 36.331
 */
class NrRrcAsn1Header : public NrAsn1Header
{
  public:
    NrRrcAsn1Header();
    /**
     * Get message type
     *
     * \returns the message type
     */
    int GetMessageType() const;

  protected:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    // Inherited from NrAsn1Header
    TypeId GetInstanceTypeId() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override = 0;
    void PreSerialize() const override = 0;

    // Auxiliary functions
    /**
     * Convert from bandwidth (in RBs) to ENUMERATED value
     *
     * \param bandwidth Bandwidth in RBs: 6, 15, 25, 50, 75, 100
     * \returns ENUMERATED value: 0, 1, 2, 3, 4, 5
     */
    int BandwidthToEnum(uint16_t bandwidth) const;
    /**
     * Convert from ENUMERATED value to bandwidth (in RBs)
     *
     * \param n ENUMERATED value: 0, 1, 2, 3, 4, 5
     * \returns bandwidth Bandwidth in RBs: 6, 15, 25, 50, 75, 100
     */
    uint16_t EnumToBandwidth(int n) const;

    // Serialization functions
    /**
     * Serialize SRB to add mod list function
     *
     * \param srbToAddModList std::list<NrRrcSap::SrbToAddMod>
     */
    void SerializeSrbToAddModList(std::list<NrRrcSap::SrbToAddMod> srbToAddModList) const;
    /**
     * Serialize DRB to add mod list function
     *
     * \param drbToAddModList std::list<NrRrcSap::SrbToAddMod>
     */
    void SerializeDrbToAddModList(std::list<NrRrcSap::DrbToAddMod> drbToAddModList) const;
    /**
     * Serialize logicala channel config function
     *
     * \param logicalChannelConfig NrRrcSap::LogicalChannelConfig
     */
    void SerializeLogicalChannelConfig(NrRrcSap::LogicalChannelConfig logicalChannelConfig) const;
    /**
     * Serialize radio resource config function
     *
     * \param radioResourceConfigDedicated NrRrcSap::RadioResourceConfigDedicated
     */
    void SerializeRadioResourceConfigDedicated(
        NrRrcSap::RadioResourceConfigDedicated radioResourceConfigDedicated) const;
    /**
     * Serialize physical config dedicated function
     *
     * \param physicalConfigDedicated NrRrcSap::PhysicalConfigDedicated
     */
    void SerializePhysicalConfigDedicated(
        NrRrcSap::PhysicalConfigDedicated physicalConfigDedicated) const;
    /**
     * Serialize physical config dedicated function
     *
     * \param pcdsc NrRrcSap::PhysicalConfigDedicatedSCell
     */
    void SerializePhysicalConfigDedicatedSCell(NrRrcSap::PhysicalConfigDedicatedSCell pcdsc) const;
    /**
     * Serialize system information block type 1 function
     *
     * \param systemInformationBlockType1 NrRrcSap::SystemInformationBlockType1
     */
    void SerializeSystemInformationBlockType1(
        NrRrcSap::SystemInformationBlockType1 systemInformationBlockType1) const;
    /**
     * Serialize system information block type 2 function
     *
     * \param systemInformationBlockType2 NrRrcSap::SystemInformationBlockType2
     */
    void SerializeSystemInformationBlockType2(
        NrRrcSap::SystemInformationBlockType2 systemInformationBlockType2) const;
    /**
     * Serialize system information block type 2 function
     *
     * \param radioResourceConfigCommon NrRrcSap::RadioResourceConfigCommon
     */
    void SerializeRadioResourceConfigCommon(
        NrRrcSap::RadioResourceConfigCommon radioResourceConfigCommon) const;
    /**
     * Serialize radio resource config common SIB function
     *
     * \param radioResourceConfigCommonSib NrRrcSap::RadioResourceConfigCommonSib
     */
    void SerializeRadioResourceConfigCommonSib(
        NrRrcSap::RadioResourceConfigCommonSib radioResourceConfigCommonSib) const;
    /**
     * Serialize measure results function
     *
     * \param measResults NrRrcSap::MeasResults
     */
    void SerializeMeasResults(NrRrcSap::MeasResults measResults) const;
    /**
     * Serialize PLMN identity function
     *
     * \param plmnId the PLMN ID
     */
    void SerializePlmnIdentity(uint32_t plmnId) const;
    /**
     * Serialize RACH config common function
     *
     * \param rachConfigCommon NrRrcSap::RachConfigCommon
     */
    void SerializeRachConfigCommon(NrRrcSap::RachConfigCommon rachConfigCommon) const;
    /**
     * Serialize measure config function
     *
     * \param measConfig NrRrcSap::MeasConfig
     */
    void SerializeMeasConfig(NrRrcSap::MeasConfig measConfig) const;
    /**
     * Serialize non critical extension config function
     *
     * \param nonCriticalExtensionConfiguration NrRrcSap::NonCriticalExtensionConfiguration
     */
    void SerializeNonCriticalExtensionConfiguration(
        NrRrcSap::NonCriticalExtensionConfiguration nonCriticalExtensionConfiguration) const;
    /**
     * Serialize radio resource config common SCell function
     *
     * \param rrccsc NrRrcSap::RadioResourceConfigCommonSCell
     */
    void SerializeRadioResourceConfigCommonSCell(
        NrRrcSap::RadioResourceConfigCommonSCell rrccsc) const;
    /**
     * Serialize radio resource dedicated SCell function
     *
     * \param rrcdsc NrRrcSap::RadioResourceConfigDedicatedSCell
     */
    void SerializeRadioResourceDedicatedSCell(
        NrRrcSap::RadioResourceConfigDedicatedSCell rrcdsc) const;
    /**
     * Serialize Q offset range function
     *
     * \param qOffsetRange q offset range
     */
    void SerializeQoffsetRange(int8_t qOffsetRange) const;
    /**
     * Serialize threshold eutra function
     *
     * \param thresholdEutra NrRrcSap::ThresholdEutra
     */
    void SerializeThresholdEutra(NrRrcSap::ThresholdEutra thresholdEutra) const;

    // Deserialization functions
    /**
     * Deserialize DRB to add mod list function
     *
     * \param drbToAddModLis std::list<NrRrcSap::DrbToAddMod> *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeDrbToAddModList(std::list<NrRrcSap::DrbToAddMod>* drbToAddModLis,
                                                Buffer::Iterator bIterator);
    /**
     * Deserialize SRB to add mod list function
     *
     * \param srbToAddModList std::list<NrRrcSap::SrbToAddMod> *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeSrbToAddModList(std::list<NrRrcSap::SrbToAddMod>* srbToAddModList,
                                                Buffer::Iterator bIterator);
    /**
     * Deserialize logical channel config function
     *
     * \param logicalChannelConfig NrRrcSap::LogicalChannelConfig *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeLogicalChannelConfig(
        NrRrcSap::LogicalChannelConfig* logicalChannelConfig,
        Buffer::Iterator bIterator);
    /**
     * Deserialize radio resource config dedicated function
     *
     * \param radioResourceConfigDedicated NrRrcSap::RadioResourceConfigDedicated *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRadioResourceConfigDedicated(
        NrRrcSap::RadioResourceConfigDedicated* radioResourceConfigDedicated,
        Buffer::Iterator bIterator);
    /**
     * Deserialize physical config dedicated function
     *
     * \param physicalConfigDedicated NrRrcSap::PhysicalConfigDedicated *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializePhysicalConfigDedicated(
        NrRrcSap::PhysicalConfigDedicated* physicalConfigDedicated,
        Buffer::Iterator bIterator);
    /**
     * Deserialize system information block type 1 function
     *
     * \param systemInformationBlockType1 NrRrcSap::SystemInformationBlockType1 *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeSystemInformationBlockType1(
        NrRrcSap::SystemInformationBlockType1* systemInformationBlockType1,
        Buffer::Iterator bIterator);
    /**
     * Deserialize system information block type 2 function
     *
     * \param systemInformationBlockType2 NrRrcSap::SystemInformationBlockType2 *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeSystemInformationBlockType2(
        NrRrcSap::SystemInformationBlockType2* systemInformationBlockType2,
        Buffer::Iterator bIterator);
    /**
     * Deserialize radio resource config common function
     *
     * \param radioResourceConfigCommon NrRrcSap::RadioResourceConfigCommon *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRadioResourceConfigCommon(
        NrRrcSap::RadioResourceConfigCommon* radioResourceConfigCommon,
        Buffer::Iterator bIterator);
    /**
     * Deserialize radio resource config common SIB function
     *
     * \param radioResourceConfigCommonSib NrRrcSap::RadioResourceConfigCommonSib *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRadioResourceConfigCommonSib(
        NrRrcSap::RadioResourceConfigCommonSib* radioResourceConfigCommonSib,
        Buffer::Iterator bIterator);
    /**
     * Deserialize measure results function
     *
     * \param measResults NrRrcSap::MeasResults *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeMeasResults(NrRrcSap::MeasResults* measResults,
                                            Buffer::Iterator bIterator);
    /**
     * Deserialize PLMN identity function
     *
     * \param plmnId the PLMN ID
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializePlmnIdentity(uint32_t* plmnId, Buffer::Iterator bIterator);
    /**
     * Deserialize RACH config common function
     *
     * \param rachConfigCommon NrRrcSap::RachConfigCommon *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRachConfigCommon(NrRrcSap::RachConfigCommon* rachConfigCommon,
                                                 Buffer::Iterator bIterator);
    /**
     * Deserialize measure config function
     *
     * \param measConfig NrRrcSap::MeasConfig *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeMeasConfig(NrRrcSap::MeasConfig* measConfig,
                                           Buffer::Iterator bIterator);
    /**
     * Deserialize Qoffset range function
     *
     * \param qOffsetRange Qoffset range
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeQoffsetRange(int8_t* qOffsetRange, Buffer::Iterator bIterator);
    /**
     * Deserialize threshold eutra function
     *
     * \param thresholdEutra NrRrcSap::ThresholdEutra *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeThresholdEutra(NrRrcSap::ThresholdEutra* thresholdEutra,
                                               Buffer::Iterator bIterator);
    /**
     * Deserialize non critical extension config function
     *
     * \param nonCriticalExtension NrRrcSap::NonCriticalExtensionConfiguration *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeNonCriticalExtensionConfig(
        NrRrcSap::NonCriticalExtensionConfiguration* nonCriticalExtension,
        Buffer::Iterator bIterator);
    /**
     * Deserialize cell identification function
     *
     * \param ci NrRrcSap::CellIdentification *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeCellIdentification(NrRrcSap::CellIdentification* ci,
                                                   Buffer::Iterator bIterator);
    /**
     * Deserialize radio resource config common SCell function
     *
     * \param rrccsc NrRrcSap::RadioResourceConfigCommonSCell *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRadioResourceConfigCommonSCell(
        NrRrcSap::RadioResourceConfigCommonSCell* rrccsc,
        Buffer::Iterator bIterator);
    /**
     * Deserialize radio resource config dedicated SCell function
     *
     * \param rrcdsc NrRrcSap::RadioResourceConfigDedicatedSCell *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeRadioResourceConfigDedicatedSCell(
        NrRrcSap::RadioResourceConfigDedicatedSCell* rrcdsc,
        Buffer::Iterator bIterator);
    /**
     * Deserialize physical config dedicated SCell function
     *
     * \param pcdsc NrRrcSap::PhysicalConfigDedicatedSCell *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializePhysicalConfigDedicatedSCell(
        NrRrcSap::PhysicalConfigDedicatedSCell* pcdsc,
        Buffer::Iterator bIterator);

    /**
     * This function prints the object, for debugging purposes.
     * @param os The output stream to use (i.e. std::cout)
     */
    void Print(std::ostream& os) const override;
    /**
     * This function prints RadioResourceConfigDedicated IE, for debugging purposes.
     * @param os The output stream to use (i.e. std::cout)
     * @param radioResourceConfigDedicated The information element to be printed
     */
    void Print(std::ostream& os,
               NrRrcSap::RadioResourceConfigDedicated radioResourceConfigDedicated) const;

    /// Stores RRC message type, according to 3GPP TS 36.331
    int m_messageType;
};

/**
 * This class only serves to discriminate which message type has been received
 * in uplink (ue to eNb) for channel DCCH
 */
class NrRrcUlDcchMessage : public NrRrcAsn1Header
{
  public:
    NrRrcUlDcchMessage();
    ~NrRrcUlDcchMessage() override;

    // Inherited from NrRrcAsn1Header
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;
    void PreSerialize() const override;

  protected:
    /**
     * Serialize UL DCCH message function
     *
     * \param msgType message type
     */
    void SerializeUlDcchMessage(int msgType) const;
    /**
     * Deserialize UL DCCH message function
     *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeUlDcchMessage(Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in downlink (eNb to ue) for channel DCCH
 */
class NrRrcDlDcchMessage : public NrRrcAsn1Header
{
  public:
    NrRrcDlDcchMessage();
    ~NrRrcDlDcchMessage() override;

    // Inherited from NrRrcAsn1Header
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;
    void PreSerialize() const override;

  protected:
    /**
     * Serialize DL DCCH message function
     *
     * \param msgType message type
     */
    void SerializeDlDcchMessage(int msgType) const;
    /**
     * Deserialize DL DCCH message function
     *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeDlDcchMessage(Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in uplink (ue to eNb) for channel CCCH
 */
class NrRrcUlCcchMessage : public NrRrcAsn1Header
{
  public:
    NrRrcUlCcchMessage();
    ~NrRrcUlCcchMessage() override;

    // Inherited from NrRrcAsn1Header
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;
    void PreSerialize() const override;

  protected:
    /**
     * Serialize UL CCCH message function
     *
     * \param msgType message type
     */
    void SerializeUlCcchMessage(int msgType) const;
    /**
     * Deserialize DL CCCH message function
     *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeUlCcchMessage(Buffer::Iterator bIterator);
};

/**
 * This class only serves to discriminate which message type has been received
 * in downlink (eNb to ue) for channel CCCH
 */
class NrRrcDlCcchMessage : public NrRrcAsn1Header
{
  public:
    NrRrcDlCcchMessage();
    ~NrRrcDlCcchMessage() override;

    // Inherited from NrRrcAsn1Header
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;
    void PreSerialize() const override;

  protected:
    /**
     * Serialize DL CCCH message function
     *
     * \param msgType message type
     */
    void SerializeDlCcchMessage(int msgType) const;
    /**
     * Deserialize DL CCCH message function
     *
     * \param bIterator buffer iterator
     * \returns buffer iterator
     */
    Buffer::Iterator DeserializeDlCcchMessage(Buffer::Iterator bIterator);
};

/**
 * This class manages the serialization/deserialization of RrcConnectionRequest IE
 */
class NrRrcConnectionRequestHeader : public NrRrcUlCcchMessage
{
  public:
    NrRrcConnectionRequestHeader();
    ~NrRrcConnectionRequestHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionRequest IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionRequest msg);

    /**
     * Returns a RrcConnectionRequest IE from the values in the class attributes
     * @return A RrcConnectionRequest, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionRequest GetMessage() const;

    /**
     * Get MMEC attribute
     * @return m_mmec attribute
     */
    std::bitset<8> GetMmec() const;

    /**
     * Get M-TMSI attribute
     * @return m_tmsi attribute
     */
    std::bitset<32> GetMtmsi() const;

  private:
    std::bitset<8> m_mmec;   ///< MMEC
    std::bitset<32> m_mTmsi; ///< TMSI

    /// EstablishmentCause enumeration
    enum
    {
        EMERGENCY = 0,
        HIGHPRIORITYACCESS,
        MT_ACCESS,
        MO_SIGNALLING,
        MO_DATA,
        SPARE3,
        SPARE2,
        SPARE1
    } m_establishmentCause; ///< the establishent cause

    std::bitset<1> m_spare; ///< spare bit
};

/**
 * This class manages the serialization/deserialization of RrcConnectionSetup IE
 */
class NrRrcConnectionSetupHeader : public NrRrcDlCcchMessage
{
  public:
    NrRrcConnectionSetupHeader();
    ~NrRrcConnectionSetupHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionSetup IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionSetup msg);

    /**
     * Returns a RrcConnectionSetup IE from the values in the class attributes
     * @return A RrcConnectionSetup, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionSetup GetMessage() const;

    /**
     * Getter for m_rrcTransactionIdentifier
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

    /**
     * Getter for m_radioResourceConfigDedicated
     * @return m_radioResourceConfigDedicated
     */
    NrRrcSap::RadioResourceConfigDedicated GetRadioResourceConfigDedicated() const;

    /**
     * Gets m_radioResourceConfigDedicated.havePhysicalConfigDedicated
     * @return m_radioResourceConfigDedicated.havePhysicalConfigDedicated
     */
    bool HavePhysicalConfigDedicated() const;

    /**
     * Gets m_radioResourceConfigDedicated.physicalConfigDedicated
     * @return m_radioResourceConfigDedicated.physicalConfigDedicated
     */
    NrRrcSap::PhysicalConfigDedicated GetPhysicalConfigDedicated() const;

    /**
     * Gets m_radioResourceConfigDedicated.srbToAddModList
     * @return m_radioResourceConfigDedicated.srbToAddModList
     */
    std::list<NrRrcSap::SrbToAddMod> GetSrbToAddModList() const;

    /**
     * Gets m_radioResourceConfigDedicated.drbToAddModList
     * @return m_radioResourceConfigDedicated.drbToAddModList
     */
    std::list<NrRrcSap::DrbToAddMod> GetDrbToAddModList() const;

    /**
     * Gets m_radioResourceConfigDedicated.drbToReleaseList
     * @return m_radioResourceConfigDedicated.drbToReleaseList
     */
    std::list<uint8_t> GetDrbToReleaseList() const;

  private:
    uint8_t m_rrcTransactionIdentifier; ///< RRC transaction identifier
    mutable NrRrcSap::RadioResourceConfigDedicated
        m_radioResourceConfigDedicated; ///< radio resource config dedicated
};

/**
 * This class manages the serialization/deserialization of RrcConnectionSetupComplete IE
 */
class NrRrcConnectionSetupCompleteHeader : public NrRrcUlDcchMessage
{
  public:
    NrRrcConnectionSetupCompleteHeader();
    ~NrRrcConnectionSetupCompleteHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionSetupCompleted IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionSetupCompleted msg);

    /**
     * Returns a RrcConnectionSetupCompleted IE from the values in the class attributes
     * @return A RrcConnectionSetupCompleted, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionSetupCompleted GetMessage() const;

    /**
     * Getter for m_rrcTransactionIdentifier
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

  private:
    uint8_t m_rrcTransactionIdentifier; ///< RRC transaction identifier
};

/**
 * This class manages the serialization/deserialization of RrcConnectionSetupComplete IE
 */
class NrRrcConnectionReconfigurationCompleteHeader : public NrRrcUlDcchMessage
{
  public:
    NrRrcConnectionReconfigurationCompleteHeader();
    ~NrRrcConnectionReconfigurationCompleteHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReconfigurationCompleted IE and stores the contents into the class
     * attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReconfigurationCompleted msg);

    /**
     * Returns a RrcConnectionReconfigurationCompleted IE from the values in the class attributes
     * @return A RrcConnectionReconfigurationCompleted, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReconfigurationCompleted GetMessage() const;

    /**
     * Getter for m_rrcTransactionIdentifier
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

  private:
    uint8_t m_rrcTransactionIdentifier; ///< RRC transaction identifier
};

/**
 * This class manages the serialization/deserialization of RrcConnectionReconfiguration IE
 */
class NrRrcConnectionReconfigurationHeader : public NrRrcDlDcchMessage
{
  public:
    NrRrcConnectionReconfigurationHeader();
    ~NrRrcConnectionReconfigurationHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReconfiguration IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReconfiguration msg);

    /**
     * Returns a RrcConnectionReconfiguration IE from the values in the class attributes
     * @return A RrcConnectionReconfiguration, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReconfiguration GetMessage() const;

    /**
     * Getter for m_haveMeasConfig
     * @return m_haveMeasConfig
     */
    bool GetHaveMeasConfig() const;

    /**
     * Getter for m_measConfig
     * @return m_measConfig
     */
    NrRrcSap::MeasConfig GetMeasConfig();

    /**
     * Getter for m_haveMobilityControlInfo
     * @return m_haveMobilityControlInfo
     */
    bool GetHaveMobilityControlInfo() const;

    /**
     * Getter for m_mobilityControlInfo
     * @return m_mobilityControlInfo
     */
    NrRrcSap::MobilityControlInfo GetMobilityControlInfo();

    /**
     * Getter for m_haveRadioResourceConfigDedicated
     * @return m_haveRadioResourceConfigDedicated
     */
    bool GetHaveRadioResourceConfigDedicated() const;

    /**
     * Getter for m_radioResourceConfigDedicated
     * @return m_radioResourceConfigDedicated
     */
    NrRrcSap::RadioResourceConfigDedicated GetRadioResourceConfigDedicated();

    /**
     * Getter for m_rrcTransactionIdentifier
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

    /**
     * Getter for m_radioResourceConfigDedicated
     * @return m_radioResourceConfigDedicated
     */
    NrRrcSap::RadioResourceConfigDedicated GetRadioResourceConfigDedicated() const;

    /**
     * Getter for m_haveNonCriticalExtension
     * @return m_haveNonCriticalExtension
     */
    bool GetHaveNonCriticalExtensionConfig() const;

    /**
     * Getter for m_nonCriticalExtension
     * @return m_nonCriticalExtension
     */
    NrRrcSap::NonCriticalExtensionConfiguration GetNonCriticalExtensionConfig();

    /**
     * Gets m_radioResourceConfigDedicated.havePhysicalConfigDedicated
     * @return m_radioResourceConfigDedicated.havePhysicalConfigDedicated
     */
    bool HavePhysicalConfigDedicated() const;

    /**
     * Gets m_radioResourceConfigDedicated.physicalConfigDedicated
     * @return m_radioResourceConfigDedicated.physicalConfigDedicated
     */
    NrRrcSap::PhysicalConfigDedicated GetPhysicalConfigDedicated() const;

    /**
     * Gets m_radioResourceConfigDedicated.srbToAddModList
     * @return m_radioResourceConfigDedicated.srbToAddModList
     */
    std::list<NrRrcSap::SrbToAddMod> GetSrbToAddModList() const;

    /**
     * Gets m_radioResourceConfigDedicated.drbToAddModList
     * @return m_radioResourceConfigDedicated.drbToAddModList
     */
    std::list<NrRrcSap::DrbToAddMod> GetDrbToAddModList() const;

    /**
     * Gets m_radioResourceConfigDedicated.drbToReleaseList
     * @return m_radioResourceConfigDedicated.drbToReleaseList
     */
    std::list<uint8_t> GetDrbToReleaseList() const;

  private:
    uint8_t m_rrcTransactionIdentifier;                  ///< RRC transaction identifier
    bool m_haveMeasConfig;                               ///< have measure config?
    NrRrcSap::MeasConfig m_measConfig;                   ///< the measure config
    bool m_haveMobilityControlInfo;                      ///< have mobility control info?
    NrRrcSap::MobilityControlInfo m_mobilityControlInfo; ///< the modility control info
    bool m_haveRadioResourceConfigDedicated;             ///< have radio resource config dedicated?
    NrRrcSap::RadioResourceConfigDedicated
        m_radioResourceConfigDedicated; ///< the radio resource config dedicated
    bool m_haveNonCriticalExtension;    ///< Have non-critical extension
    NrRrcSap::NonCriticalExtensionConfiguration
        m_nonCriticalExtension; ///< the non-critical extension
};

/**
 * This class manages the serialization/deserialization of HandoverPreparationInfo IE
 */
class NrHandoverPreparationInfoHeader : public NrRrcAsn1Header
{
  public:
    NrHandoverPreparationInfoHeader();

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a HandoverPreparationInfo IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::HandoverPreparationInfo msg);

    /**
     * Returns a HandoverPreparationInfo IE from the values in the class attributes
     * @return A HandoverPreparationInfo, as defined in NrRrcSap
     */
    NrRrcSap::HandoverPreparationInfo GetMessage() const;

    /**
     * Getter for m_asConfig
     * @return m_asConfig
     */
    NrRrcSap::AsConfig GetAsConfig() const;

  private:
    NrRrcSap::AsConfig m_asConfig; ///< AS config
};

/**
 * This class manages the serialization/deserialization of RRCConnectionReestablishmentRequest IE
 */
class NrRrcConnectionReestablishmentRequestHeader : public NrRrcUlCcchMessage
{
  public:
    NrRrcConnectionReestablishmentRequestHeader();
    ~NrRrcConnectionReestablishmentRequestHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReestablishmentRequest IE and stores the contents into the class
     * attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReestablishmentRequest msg);

    /**
     * Returns a RrcConnectionReestablishmentRequest IE from the values in the class attributes
     * @return A RrcConnectionReestablishmentRequest, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReestablishmentRequest GetMessage() const;

    /**
     * Getter for m_ueIdentity
     * @return m_ueIdentity
     */
    NrRrcSap::ReestabUeIdentity GetUeIdentity() const;

    /**
     * Getter for m_reestablishmentCause
     * @return m_reestablishmentCause
     */
    NrRrcSap::ReestablishmentCause GetReestablishmentCause() const;

  private:
    NrRrcSap::ReestabUeIdentity m_ueIdentity;              ///< UE identity
    NrRrcSap::ReestablishmentCause m_reestablishmentCause; ///< reestablishment cause
};

/**
 * This class manages the serialization/deserialization of RrcConnectionReestablishment IE
 */
class NrRrcConnectionReestablishmentHeader : public NrRrcDlCcchMessage
{
  public:
    NrRrcConnectionReestablishmentHeader();
    ~NrRrcConnectionReestablishmentHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReestablishment IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReestablishment msg);

    /**
     * Returns a RrcConnectionReestablishment IE from the values in the class attributes
     * @return A RrcConnectionReestablishment, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReestablishment GetMessage() const;

    /**
     * Getter for m_rrcTransactionIdentifier attribute
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

    /**
     * Getter for m_radioResourceConfigDedicated attribute
     * @return m_radioResourceConfigDedicated
     */
    NrRrcSap::RadioResourceConfigDedicated GetRadioResourceConfigDedicated() const;

  private:
    uint8_t m_rrcTransactionIdentifier; ///< RRC transaction identifier
    NrRrcSap::RadioResourceConfigDedicated
        m_radioResourceConfigDedicated; ///< radio resource config dedicated
};

/**
 * This class manages the serialization/deserialization of RrcConnectionReestablishmentComplete IE
 */
class NrRrcConnectionReestablishmentCompleteHeader : public NrRrcUlDcchMessage
{
  public:
    NrRrcConnectionReestablishmentCompleteHeader();

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReestablishmentComplete IE and stores the contents into the class
     * attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReestablishmentComplete msg);

    /**
     * Returns a RrcConnectionReestablishmentComplete IE from the values in the class attributes
     * @return A RrcConnectionReestablishmentComplete, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReestablishmentComplete GetMessage() const;

    /**
     * Getter for m_rrcTransactionIdentifier attribute
     * @return m_rrcTransactionIdentifier
     */
    uint8_t GetRrcTransactionIdentifier() const;

  private:
    uint8_t m_rrcTransactionIdentifier; ///< RRC transaction identifier
};

/**
 * This class manages the serialization/deserialization of RrcConnectionReestablishmentReject IE
 */
class NrRrcConnectionReestablishmentRejectHeader : public NrRrcDlCcchMessage
{
  public:
    NrRrcConnectionReestablishmentRejectHeader();
    ~NrRrcConnectionReestablishmentRejectHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReestablishmentReject IE and stores the contents into the class
     * attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReestablishmentReject msg);

    /**
     * Returns a RrcConnectionReestablishmentReject IE from the values in the class attributes
     * @return A RrcConnectionReestablishmentReject, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReestablishmentReject GetMessage() const;

  private:
    NrRrcSap::RrcConnectionReestablishmentReject
        m_rrcConnectionReestablishmentReject; ///< RRC connection reestablishmnet reject
};

/**
 * This class manages the serialization/deserialization of RrcConnectionRelease IE
 */
class NrRrcConnectionReleaseHeader : public NrRrcDlDcchMessage
{
  public:
    NrRrcConnectionReleaseHeader();
    ~NrRrcConnectionReleaseHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionRelease IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionRelease msg);

    /**
     * Returns a RrcConnectionRelease IE from the values in the class attributes
     * @return A RrcConnectionRelease, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionRelease GetMessage() const;

  private:
    NrRrcSap::RrcConnectionRelease m_rrcConnectionRelease; ///< RRC connection release
};

/**
 * This class manages the serialization/deserialization of RrcConnectionReject IE
 */
class NrRrcConnectionRejectHeader : public NrRrcDlCcchMessage
{
  public:
    NrRrcConnectionRejectHeader();
    ~NrRrcConnectionRejectHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a RrcConnectionReject IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::RrcConnectionReject msg);

    /**
     * Returns a RrcConnectionReject IE from the values in the class attributes
     * @return A RrcConnectionReject, as defined in NrRrcSap
     */
    NrRrcSap::RrcConnectionReject GetMessage() const;

  private:
    NrRrcSap::RrcConnectionReject m_rrcConnectionReject; ///< RRC connection reject
};

/**
 * This class manages the serialization/deserialization of MeasurementReport IE
 */
class NrMeasurementReportHeader : public NrRrcUlDcchMessage
{
  public:
    NrMeasurementReportHeader();
    ~NrMeasurementReportHeader() override;

    // Inherited from NrRrcAsn1Header
    void PreSerialize() const override;
    uint32_t Deserialize(Buffer::Iterator bIterator) override;
    void Print(std::ostream& os) const override;

    /**
     * Receives a MeasurementReport IE and stores the contents into the class attributes
     * @param msg The information element to parse
     */
    void SetMessage(NrRrcSap::MeasurementReport msg);

    /**
     * Returns a MeasurementReport IE from the values in the class attributes
     * @return A MeasurementReport, as defined in NrRrcSap
     */
    NrRrcSap::MeasurementReport GetMessage() const;

  private:
    NrRrcSap::MeasurementReport m_measurementReport; ///< measurement report
};

} // namespace ns3

#endif // RRC_HEADER_H
