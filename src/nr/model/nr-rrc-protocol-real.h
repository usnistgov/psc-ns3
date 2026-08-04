// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors: Nicola Baldo <nbaldo@cttc.es>
//          Lluis Parcerisa <lparcerisa@cttc.cat>

#ifndef NR_RRC_PROTOCOL_REAL_H
#define NR_RRC_PROTOCOL_REAL_H

#include "nr-pdcp-sap.h"
#include "nr-rlc-sap.h"
#include "nr-rrc-sap.h"

#include <ns3/object.h>
#include <ns3/ptr.h>

#include <map>
#include <stdint.h>

namespace ns3
{

class NrUeRrcSapProvider;
class NrUeRrcSapUser;
class NrGnbRrcSapProvider;
class NrUeRrc;
class NrGnbNetDevice;

namespace nr
{
/**
 * \ingroup nr
 *
 * Models the transmission of RRC messages from the UE to the gNB in
 * a real fashion, by creating real RRC PDUs and transmitting them
 * over Signaling Radio Bearers using radio resources allocated by the
 * NR MAC scheduler.
 *
 */
class UeRrcProtocolReal : public Object
{
    /// allow MemberNrUeRrcSapUser<UeRrcProtocolReal> class friend access
    friend class MemberNrUeRrcSapUser<UeRrcProtocolReal>;
    /// allow NrRlcSpecificNrRlcSapUser<UeRrcProtocolReal> class friend access
    friend class NrRlcSpecificNrRlcSapUser<UeRrcProtocolReal>;
    /// allow NrPdcpSpecificNrPdcpSapUser<UeRrcProtocolReal> class friend access
    friend class NrPdcpSpecificNrPdcpSapUser<UeRrcProtocolReal>;

  public:
    UeRrcProtocolReal();
    ~UeRrcProtocolReal() override;

    // inherited from Object
    void DoDispose() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set NR UE RRC SAP provider function
     *
     * \param p the NR UE RRC SAP provider
     */
    void SetNrUeRrcSapProvider(NrUeRrcSapProvider* p);
    /**
     * Get NR UE RRC SAP user function
     *
     * \returns NR UE RRC SAP user
     */
    NrUeRrcSapUser* GetNrUeRrcSapUser();

    /**
     * Set UE RRC function
     *
     * \param rrc the NR UE RRC
     */
    void SetUeRrc(Ptr<NrUeRrc> rrc);

  private:
    // methods forwarded from NrUeRrcSapUser
    /**
     * Setup function
     *
     * \param params NrUeRrcSapUser::SetupParameters
     */
    void DoSetup(NrUeRrcSapUser::SetupParameters params);
    /**
     * Send RRC connection request function
     *
     * \param msg NrRrcSap::RrcConnectionRequest
     */
    void DoSendRrcConnectionRequest(NrRrcSap::RrcConnectionRequest msg);
    /**
     * Send RRC connection setup completed function
     *
     * \param msg NrRrcSap::RrcConnectionSetupCompleted
     */
    void DoSendRrcConnectionSetupCompleted(NrRrcSap::RrcConnectionSetupCompleted msg) const;
    /**
     * Send RRC connection reconfiguration setup completed function
     *
     * \param msg NrRrcSap::RrcConnectionReconfigurationCompleted
     */
    void DoSendRrcConnectionReconfigurationCompleted(
        NrRrcSap::RrcConnectionReconfigurationCompleted msg);
    /**
     * Send RRC connection reestablishment request function
     *
     * \param msg NrRrcSap::RrcConnectionReestablishmentRequest
     */
    void DoSendRrcConnectionReestablishmentRequest(
        NrRrcSap::RrcConnectionReestablishmentRequest msg) const;
    /**
     * Send RRC connection reestablishment complete function
     *
     * \param msg NrRrcSap::RrcConnectionReestablishmentComplete
     */
    void DoSendRrcConnectionReestablishmentComplete(
        NrRrcSap::RrcConnectionReestablishmentComplete msg) const;
    /**
     * Send measurement report function
     *
     * \param msg NrRrcSap::MeasurementReport
     */
    void DoSendMeasurementReport(NrRrcSap::MeasurementReport msg);
    /**
     * \brief Send ideal UE context remove request function
     *
     * Notify eNodeB to release UE context once radio link failure
     * or random access failure is detected. It is needed since no
     * RLF detection mechanism at eNodeB is implemented
     *
     * \param rnti the RNTI of the UE
     */
    void DoSendIdealUeContextRemoveRequest(uint16_t rnti);

    /// Set gNB RRC SAP provider
    void SetGnbRrcSapProvider();
    /**
     * Receive PDCP PDU function
     *
     * \param p the packet
     */
    void DoReceivePdcpPdu(Ptr<Packet> p);
    /**
     * Receive PDCP SDU function
     *
     * \param params NrPdcpSapUser::ReceivePdcpSduParameters
     */
    void DoReceivePdcpSdu(NrPdcpSapUser::ReceivePdcpSduParameters params);

    Ptr<NrUeRrc> m_rrc;                       ///< the RRC
    uint16_t m_rnti;                          ///< the RNTI
    NrUeRrcSapProvider* m_ueRrcSapProvider;   ///< UE RRC SAP provider
    NrUeRrcSapUser* m_ueRrcSapUser;           ///< UE RRC SAP user
    NrGnbRrcSapProvider* m_gnbRrcSapProvider; ///< gNB RRC SAP provider

    NrUeRrcSapUser::SetupParameters m_setupParameters; ///< setup parameters
    NrUeRrcSapProvider::CompleteSetupParameters
        m_completeSetupParameters; ///< complete setup parameters
    std::unordered_map<uint16_t, Ptr<NrGnbNetDevice>> m_knownGnb;
};

/**
 * Models the transmission of RRC messages from the UE to the gNB in
 * a real fashion, by creating real RRC PDUs and transmitting them
 * over Signaling Radio Bearers using radio resources allocated by the
 * NR MAC scheduler.
 *
 */
class NrGnbRrcProtocolReal : public Object
{
    /// allow MemberNrGnbRrcSapUser<NrGnbRrcProtocolReal> class friend access
    friend class MemberNrGnbRrcSapUser<NrGnbRrcProtocolReal>;
    /// allow NrPdcpSpecificNrPdcpSapUser<NrGnbRrcProtocolReal> class friend access
    friend class NrPdcpSpecificNrPdcpSapUser<NrGnbRrcProtocolReal>;
    /// allow NrRlcSpecificNrRlcSapUser<NrGnbRrcProtocolReal> class friend access
    friend class NrRlcSpecificNrRlcSapUser<NrGnbRrcProtocolReal>;
    /// allow RealProtocolRlcSapUser class friend access
    friend class RealProtocolRlcSapUser;

  public:
    NrGnbRrcProtocolReal();
    ~NrGnbRrcProtocolReal() override;

    // inherited from Object
    void DoDispose() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set NR gNB RRC SAP provider function
     *
     * \param p NrGnbRrcSapProvider *
     */
    void SetNrGnbRrcSapProvider(NrGnbRrcSapProvider* p);
    /**
     * Get NR gNB RRC SAP user function
     *
     * \returns NrGnbRrcSapUser *
     */
    NrGnbRrcSapUser* GetNrGnbRrcSapUser();

    /**
     * Set cell ID function
     *
     * \param cellId the cell ID
     */
    void SetCellId(uint16_t cellId);

    /**
     * Get UE RRC SAP provider function
     *
     * \param rnti the RNTI
     * \returns NrUeRrcSapProvider *
     */
    NrUeRrcSapProvider* GetUeRrcSapProvider(uint16_t rnti);
    /**
     * Set UE RRC SAP provider function
     *
     * \param rnti the RNTI
     * \param p NrUeRrcSapProvider *
     */
    void SetUeRrcSapProvider(uint16_t rnti, NrUeRrcSapProvider* p);

  private:
    // methods forwarded from NrGnbRrcSapUser
    /**
     * Setup UE function
     *
     * \param rnti the RNTI
     * \param params NrGnbRrcSapUser::SetupUeParameters
     */
    void DoSetupUe(uint16_t rnti, NrGnbRrcSapUser::SetupUeParameters params);
    /**
     * Remove UE function
     *
     * \param rnti the RNTI
     */
    void DoRemoveUe(uint16_t rnti);
    /**
     * Send system information function
     *
     * \param cellId cell ID
     * \param msg NrRrcSap::SystemInformation
     */
    void DoSendSystemInformation(uint16_t cellId, NrRrcSap::SystemInformation msg);
    /**
     * Send system information function
     *
     * \param cellId cell ID
     * \param msg NrRrcSap::SystemInformation
     */
    void SendSystemInformation(uint16_t cellId, NrRrcSap::SystemInformation msg);
    /**
     * Send RRC connection setup function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionSetup
     */
    void DoSendRrcConnectionSetup(uint16_t rnti, NrRrcSap::RrcConnectionSetup msg);
    /**
     * Send RRC connection reconfiguration function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionReconfiguration
     */
    void DoSendRrcConnectionReconfiguration(uint16_t rnti,
                                            NrRrcSap::RrcConnectionReconfiguration msg);
    /**
     * Send RRC connection reestabishment function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionReestablishment
     */
    void DoSendRrcConnectionReestablishment(uint16_t rnti,
                                            NrRrcSap::RrcConnectionReestablishment msg);
    /**
     * Send RRC connection reestabishment reject function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionReestablishmentReject
     */
    void DoSendRrcConnectionReestablishmentReject(uint16_t rnti,
                                                  NrRrcSap::RrcConnectionReestablishmentReject msg);
    /**
     * Send RRC connection release function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionRelease
     */
    void DoSendRrcConnectionRelease(uint16_t rnti, NrRrcSap::RrcConnectionRelease msg);
    /**
     * Send RRC connection reject function
     *
     * \param rnti the RNTI
     * \param msg NrRrcSap::RrcConnectionReject
     */
    void DoSendRrcConnectionReject(uint16_t rnti, NrRrcSap::RrcConnectionReject msg);
    /**
     * Encode handover preparation information function
     *
     * \param msg NrRrcSap::HandoverPreparationInfo
     * \returns the packet
     */
    Ptr<Packet> DoEncodeHandoverPreparationInformation(NrRrcSap::HandoverPreparationInfo msg);
    /**
     * Decode handover preparation information function
     *
     * \param p the packet
     * \returns NrRrcSap::HandoverPreparationInfo
     */
    NrRrcSap::HandoverPreparationInfo DoDecodeHandoverPreparationInformation(Ptr<Packet> p);
    /**
     * Encode handover command function
     *
     * \param msg NrRrcSap::RrcConnectionReconfiguration
     * \returns the packet
     */
    Ptr<Packet> DoEncodeHandoverCommand(NrRrcSap::RrcConnectionReconfiguration msg);
    /**
     * Decode handover command function
     *
     * \param p the packet
     * \returns NrRrcSap::RrcConnectionReconfiguration
     */
    NrRrcSap::RrcConnectionReconfiguration DoDecodeHandoverCommand(Ptr<Packet> p);

    /**
     * Receive PDCP SDU function
     *
     * \param params NrPdcpSapUser::ReceivePdcpSduParameters
     */
    void DoReceivePdcpSdu(NrPdcpSapUser::ReceivePdcpSduParameters params);
    /**
     * Receive PDCP PDU function
     *
     * \param rnti the RNTI
     * \param p the packet
     */
    void DoReceivePdcpPdu(uint16_t rnti, Ptr<Packet> p);

    uint16_t m_rnti;                                                ///< the RNTI
    uint16_t m_cellId;                                              ///< the cell ID
    NrGnbRrcSapProvider* m_gnbRrcSapProvider;                       ///< gNB RRC SAP provider
    NrGnbRrcSapUser* m_gnbRrcSapUser;                               ///< gNB RRC SAP user
    std::map<uint16_t, NrUeRrcSapProvider*> m_gnbRrcSapProviderMap; ///< gNB RRC SAP provider map
    std::map<uint16_t, NrGnbRrcSapUser::SetupUeParameters>
        m_setupUeParametersMap; ///< setup UE parameters map
    std::map<uint16_t, NrGnbRrcSapProvider::CompleteSetupUeParameters>
        m_completeSetupUeParametersMap; ///< complete setup UE parameters map
};

/// RealProtocolRlcSapUser class
class RealProtocolRlcSapUser : public NrRlcSapUser
{
  public:
    /**
     * Real protocol RC SAP user
     *
     * \param pdcp NrGnbRrcProtocolReal *
     * \param rnti the RNTI
     */
    RealProtocolRlcSapUser(NrGnbRrcProtocolReal* pdcp, uint16_t rnti);

    // Interface implemented from NrRlcSapUser
    void ReceivePdcpPdu(Ptr<Packet> p) override;

  private:
    RealProtocolRlcSapUser();
    NrGnbRrcProtocolReal* m_pdcp; ///< PDCP
    uint16_t m_rnti;              ///< RNTI
};

} // namespace nr
} // namespace ns3

#endif // NR_RRC_PROTOCOL_REAL_H
