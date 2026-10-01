// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_EPC_X2_HEADER_H
#define NR_EPC_X2_HEADER_H

#include "nr-epc-x2-sap.h"

#include "ns3/header.h"

#include <vector>

namespace ns3
{
class NrEpcX2Header : public Header
{
  public:
    NrEpcX2Header();
    ~NrEpcX2Header() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get message type function
     * \returns the message type
     */
    uint8_t GetMessageType() const;
    /**
     * Set message type function
     * \param messageType the message type
     */
    void SetMessageType(uint8_t messageType);

    /**
     * Get procedure code function
     * \returns the procedure code
     */
    uint8_t GetProcedureCode() const;
    /**
     * Set procedure code function
     * \param procedureCode the procedure code
     */
    void SetProcedureCode(uint8_t procedureCode);

    /**
     * Set length of IEs function
     * \param lengthOfIes the length of IEs
     */
    void SetLengthOfIes(uint32_t lengthOfIes);
    /**
     * Set number of IEs function
     * \param numberOfIes the number of IEs
     */
    void SetNumberOfIes(uint32_t numberOfIes);

    /// Procedure code enumeration 9.3.7
    enum ProcedureCode_t
    {
        HandoverPreparation = 0,
        HandoverCancel = 1,
        LoadIndication = 2,
        SnStatusTransfer = 4,
        UeContextRelease = 5,
        ResourceStatusReporting = 10
    };

    /// Type of message enumeration
    enum TypeOfMessage_t
    {
        InitiatingMessage = 0,
        SuccessfulOutcome = 1,
        UnsuccessfulOutcome = 2
    };

  private:
    uint8_t m_messageType;   ///< message type
    uint8_t m_procedureCode; ///< procedure code

    uint32_t m_lengthOfIes; ///< length of IEs
    uint32_t m_numberOfIes; ///< number of IEs
};

/**
 * NrEpcX2HandoverRequestHeader
 */
class NrEpcX2HandoverRequestHeader : public Header
{
  public:
    NrEpcX2HandoverRequestHeader();
    ~NrEpcX2HandoverRequestHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get old gNB X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * Set old gNB X2 AP ID function
     * \param x2apId the X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * Get cause function
     * \returns the cause
     */
    uint16_t GetCause() const;
    /**
     * Set cause function
     * \param cause
     */
    void SetCause(uint16_t cause);

    /**
     * Get target cell id function
     * \returns the target cell ID
     */
    uint16_t GetTargetCellId() const;
    /**
     * Set target cell id function
     * \param targetCellId the target cell ID
     */
    void SetTargetCellId(uint16_t targetCellId);

    /**
     * Get MME UE S1 AP ID function
     * \returns the MME UE S1 AP ID
     */
    uint32_t GetMmeUeS1apId() const;
    /**
     * Set MME UE S1 AP ID function
     * \param mmeUeS1apId the MME UE S1 AP ID
     */
    void SetMmeUeS1apId(uint32_t mmeUeS1apId);

    /**
     * Get bearers function
     * \returns <NrEpcX2Sap::ErabToBeSetupItem>
     */
    std::vector<NrEpcX2Sap::ErabToBeSetupItem> GetBearers() const;
    /**
     * Set bearers function
     * \param bearers std::vector <NrEpcX2Sap::ErabToBeSetupItem>
     */
    void SetBearers(std::vector<NrEpcX2Sap::ErabToBeSetupItem> bearers);

    /**
     * Get UE Aggregate Max Bit Rate Downlink function
     * \returns the UE aggregate max bit rate downlink
     */
    uint64_t GetUeAggregateMaxBitRateDownlink() const;
    /**
     * Set UE Aggregate Max Bit Rate Downlink function
     * \param bitRate the bit rate
     */
    void SetUeAggregateMaxBitRateDownlink(uint64_t bitRate);

    /**
     * Get UE Aggregate Max Bit Rate Uplik function
     * \returns the UE aggregate max bit rate uplink
     */
    uint64_t GetUeAggregateMaxBitRateUplink() const;
    /**
     * Set UE Aggregate Max Bit Rate Uplik function
     * \param bitRate the bit rate
     */
    void SetUeAggregateMaxBitRateUplink(uint64_t bitRate);

    /**
     * Get length of IEs
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId;                ///< old gNB UE X1 AP ID
    uint16_t m_cause;                         ///< cause
    uint16_t m_targetCellId;                  ///< target cell ID
    uint32_t m_mmeUeS1apId;                   ///< MME UE S1 AP ID
    uint64_t m_ueAggregateMaxBitRateDownlink; ///< aggregate max bit rate downlink
    uint64_t m_ueAggregateMaxBitRateUplink;   ///< aggregate max bit rate uplink
    std::vector<NrEpcX2Sap::ErabToBeSetupItem> m_erabsToBeSetupList; ///< ERAB to be setup list
};

/**
 * NrEpcX2HandoverRequestAckHeader
 */
class NrEpcX2HandoverRequestAckHeader : public Header
{
  public:
    NrEpcX2HandoverRequestAckHeader();
    ~NrEpcX2HandoverRequestAckHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get old gNB UE X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * Set old gNB UE X2 AP ID function
     * \param x2apId the old gNB UE X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * Get new gNB UE X2 AP ID function
     * \returns the new gNB UE X2 AP ID
     */
    uint16_t GetNewGnbUeX2apId() const;
    /**
     * Set new gNB UE X2 AP ID function
     * \param x2apId the new gNB UE X2 AP ID
     */
    void SetNewGnbUeX2apId(uint16_t x2apId);

    /**
     * Get admittied bearers function
     * \returns <NrEpcX2Sap::ErabAdmittedItem>
     */
    std::vector<NrEpcX2Sap::ErabAdmittedItem> GetAdmittedBearers() const;
    /**
     * Set admitted bearers function
     * \param bearers the admitted bearers
     */
    void SetAdmittedBearers(std::vector<NrEpcX2Sap::ErabAdmittedItem> bearers);

    /**
     * Get not admitted bearers function
     * \returns the not admitted bearers
     */
    std::vector<NrEpcX2Sap::ErabNotAdmittedItem> GetNotAdmittedBearers() const;
    /**
     * Set not admitted bearers function
     * \param bearers the not admitted bearers
     */
    void SetNotAdmittedBearers(std::vector<NrEpcX2Sap::ErabNotAdmittedItem> bearers);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId;                                     ///< old gNB UE X2 AP ID
    uint16_t m_newGnbUeX2apId;                                     ///< new gNB UE X2 AP ID
    std::vector<NrEpcX2Sap::ErabAdmittedItem> m_erabsAdmittedList; ///< ERABs admitted list
    std::vector<NrEpcX2Sap::ErabNotAdmittedItem>
        m_erabsNotAdmittedList; ///< ERABs not admitted list
};

/**
 * NrEpcX2HandoverPreparationFailureHeader
 */
class NrEpcX2HandoverPreparationFailureHeader : public Header
{
  public:
    NrEpcX2HandoverPreparationFailureHeader();
    ~NrEpcX2HandoverPreparationFailureHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get old gNB UE X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * Set old gNB UE X2 AP ID function
     * \param x2apId the old gNB UE X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * Get cause function
     * \returns the cause
     */
    uint16_t GetCause() const;
    /**
     * Set cause function
     * \param cause
     */
    void SetCause(uint16_t cause);

    /**
     * Get criticality diagnostics function
     * \returns the criticality diagnostics
     */
    uint16_t GetCriticalityDiagnostics() const;
    /**
     * Set criticality diagnostics function
     * \param criticalityDiagnostics the criticality diagnostics
     */
    void SetCriticalityDiagnostics(uint16_t criticalityDiagnostics);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId;         ///< old gNB UE X2 AP ID
    uint16_t m_cause;                  ///< cause
    uint16_t m_criticalityDiagnostics; ///< criticality diagnostics
};

/**
 * NrEpcX2SnStatusTransferHeader
 */
class NrEpcX2SnStatusTransferHeader : public Header
{
  public:
    NrEpcX2SnStatusTransferHeader();
    ~NrEpcX2SnStatusTransferHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get old gNB UE X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * Set old gNB UE X2 AP ID function
     * \param x2apId the old gNB UE X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * Get new gNB UE X2 AP ID function
     * \returns the new gNB UE X2AP ID
     */
    uint16_t GetNewGnbUeX2apId() const;
    /**
     * Set new gNB UE X2 AP ID function
     * \param x2apId the new gNB UE X2AP ID
     */
    void SetNewGnbUeX2apId(uint16_t x2apId);

    /**
     * Get ERABs subject to status transfer list function
     * \returns std::vector <NrEpcX2Sap::ErabsSubjectToStatusTransferItem>
     */
    std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem> GetErabsSubjectToStatusTransferList()
        const;
    /**
     * Set ERABs subject to status transfer list function
     * \param erabs std::vector <NrEpcX2Sap::ErabsSubjectToStatusTransferItem>
     */
    void SetErabsSubjectToStatusTransferList(
        std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem> erabs);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId; ///< old gNB UE X2 AP ID
    uint16_t m_newGnbUeX2apId; ///< new gNB UE X2 AP ID
    std::vector<NrEpcX2Sap::ErabsSubjectToStatusTransferItem>
        m_erabsSubjectToStatusTransferList; ///< ERABs subject to status transfer list
};

/**
 * NrEpcX2UeContextReleaseHeader
 */
class NrEpcX2UeContextReleaseHeader : public Header
{
  public:
    NrEpcX2UeContextReleaseHeader();
    ~NrEpcX2UeContextReleaseHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get old gNB UE X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * Set old gNB UE X2 AP ID function
     * \param x2apId the old gNB UE X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * Get new gNB UE X2 AP ID function
     * \returns the new gNB UE X2 AP ID
     */
    uint16_t GetNewGnbUeX2apId() const;
    /**
     * Set new gNB UE X2 AP ID function
     * \param x2apId the new gNB UE X2 AP ID
     */
    void SetNewGnbUeX2apId(uint16_t x2apId);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Set length of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId; ///< old gNB UE X2 AP ID
    uint16_t m_newGnbUeX2apId; ///< new gNB UE X2 AP ID
};

/**
 * NrEpcX2LoadInformationHeader
 */
class NrEpcX2LoadInformationHeader : public Header
{
  public:
    NrEpcX2LoadInformationHeader();
    ~NrEpcX2LoadInformationHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get cell information list function
     * \returns std::vector <NrEpcX2Sap::CellInformationItem>
     */
    std::vector<NrEpcX2Sap::CellInformationItem> GetCellInformationList() const;
    /**
     * Set cell information list function
     * \param cellInformationList std::vector <NrEpcX2Sap::CellInformationItem>
     */
    void SetCellInformationList(std::vector<NrEpcX2Sap::CellInformationItem> cellInformationList);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< length of IEs

    std::vector<NrEpcX2Sap::CellInformationItem> m_cellInformationList; ///< cell information list
};

/**
 * NrEpcX2ResourceStatusUpdateHeader
 */
class NrEpcX2ResourceStatusUpdateHeader : public Header
{
  public:
    NrEpcX2ResourceStatusUpdateHeader();
    ~NrEpcX2ResourceStatusUpdateHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * Get gNB1 measurement ID function
     * \returns the gNB1 measurement ID
     */
    uint16_t GetGnb1MeasurementId() const;
    /**
     * Set gNB1 measurement ID function
     * \param gnb1MeasurementId the gNB1 measurement ID
     */
    void SetGnb1MeasurementId(uint16_t gnb1MeasurementId);

    /**
     * Get gNB2 measurement ID function
     * \returns the gNB2 measurement ID
     */
    uint16_t GetGnb2MeasurementId() const;
    /**
     * Set gNB2 measurement ID function
     * \param gnb2MeasurementId gNB2 measruement ID
     */
    void SetGnb2MeasurementId(uint16_t gnb2MeasurementId);

    /**
     * Get cell measurement results list function
     * \returns the cell measurement results list
     */
    std::vector<NrEpcX2Sap::CellMeasurementResultItem> GetCellMeasurementResultList() const;
    /**
     * Set cell measurement results list function
     * \param cellMeasurementResultList the cell measurement results list
     */
    void SetCellMeasurementResultList(
        std::vector<NrEpcX2Sap::CellMeasurementResultItem> cellMeasurementResultList);

    /**
     * Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_gnb1MeasurementId; ///< gNB1 measurement
    uint16_t m_gnb2MeasurementId; ///< gNB2 measurement
    std::vector<NrEpcX2Sap::CellMeasurementResultItem>
        m_cellMeasurementResultList; ///< cell measurement result list
};

/**
 * NrEpcX2HandoverCancelHeader
 */
class NrEpcX2HandoverCancelHeader : public Header
{
  public:
    NrEpcX2HandoverCancelHeader();
    ~NrEpcX2HandoverCancelHeader() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;
    void Print(std::ostream& os) const override;

    /**
     * \brief Get old gNB UE X2 AP ID function
     * \returns the old gNB UE X2 AP ID
     */
    uint16_t GetOldGnbUeX2apId() const;
    /**
     * \brief Set old gNB UE X2 AP ID function
     * \param x2apId the old gNB UE X2 AP ID
     */
    void SetOldGnbUeX2apId(uint16_t x2apId);

    /**
     * \brief Get new gNB UE X2 AP ID function
     * \returns the new gNB UE X2 AP ID
     */
    uint16_t GetNewGnbUeX2apId() const;
    /**
     * \brief Set new gNB UE X2 AP ID function
     * \param x2apId the new gNB UE X2 AP ID
     */
    void SetNewGnbUeX2apId(uint16_t x2apId);

    /**
     * \brief Get cause function
     * \returns the cause
     */
    uint16_t GetCause() const;
    /**
     * \brief Set cause function
     * \param cause
     */
    void SetCause(uint16_t cause);

    /**
     * \brief Get length of IEs function
     * \returns the length of IEs
     */
    uint32_t GetLengthOfIes() const;
    /**
     * \brief Get number of IEs function
     * \returns the number of IEs
     */
    uint32_t GetNumberOfIes() const;

  private:
    uint32_t m_numberOfIes;  ///< number of IEs
    uint32_t m_headerLength; ///< header length

    uint16_t m_oldGnbUeX2apId; ///< old gNB UE X2 AP ID
    uint16_t m_newGnbUeX2apId; ///< new gNB UE X2 AP ID
    uint16_t m_cause;          ///< cause
};

} // namespace ns3

#endif // NR_EPC_X2_HEADER_H
