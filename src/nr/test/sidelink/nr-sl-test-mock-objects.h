//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_TEST_MOCK_OBJECTS_H
#define NR_SL_TEST_MOCK_OBJECTS_H

#include "ns3/net-device.h"
#include "ns3/nr-mac-sap.h"
#include "ns3/nr-sl-mac-sap.h"
#include "ns3/nr-sl-pdcp-sap.h"
#include "ns3/nr-sl-rlc-sap.h"
#include "ns3/test.h"
#include <ns3/nstime.h>
#include <ns3/object.h>

using namespace ns3;

/**
 * \ingroup nr-test
 *
 * \brief This class implements a testing PDCP entity
 */
class NrSlTestPdcp : public Object
{
    friend class MemberNrSlPdcpSapProvider<NrSlTestPdcp>;
    friend class MemberNrSlRlcSapUser<NrSlTestPdcp>;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrSlTestPdcp();
    ~NrSlTestPdcp() override;

    /**
     * \brief Set the NR Sidelik SAP offered by RLC to PDCP
     *
     * \param s the NR Sidelink RLC SAP Provider interface offered by RLC to PDCP
     */
    void SetNrSlRlcSapProvider(NrSlRlcSapProvider* s);

    /**
     * \brief Get the NR Sidelik SAP offered by PDCP to RLC
     *
     * \return the NR Sidelink SAP user interface offered by PDCP to RLC
     */
    NrSlRlcSapUser* GetNrSlRlcSapUser();

    /**
     * \brief Send data function
     * \param at the time to send
     * \param dataToSend the data to send
     */
    void SendData(Time at, std::string dataToSend);

    /**
     * \brief Send a RRC PDU to the RDCP for transmission
     *
     * This method is to be called when upper RRC entity has a NR SL RRC PDU
     * ready to send.
     *
     * \param params
     */
    void DoTransmitNrSlPdcpSdu(const NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters& params);

    /**
     * \brief Called by the RLC entity to notify the PDCP entity of the reception
     * of a new NR sidelink PDCP PDU
     *
     * \param p the PDCP PDU
     */
    void DoReceiveNrSlPdcpPdu(Ptr<Packet> p);

  private:
    uint16_t m_sn{0};               /// The sequence number
    std::string m_receivedData{""}; /// The received data
    NrSlRlcSapProvider* m_nrSlRlcSapProvider{
        nullptr};                              /// SAP interface to call the methods of RLC instance
    NrSlRlcSapUser* m_nrSlRlcSapUser{nullptr}; /// SAP interface to receive calls from RLC instance
};

/**
 * \ingroup nr-test
 *
 * \brief This class implements a testing loopback MAC layer
 */
class NrSlTestMac : public Object
{
    friend class MemberNrSlMacSapProvider<NrSlTestMac>;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrSlTestMac();
    ~NrSlTestMac() override;

    /**
     * \brief Send transmit opportunity function
     * \param time the time
     * \param bytes the number of bytes
     */
    void SendTxOpportunity(Time time, uint32_t bytes);
    /**
     * \brief Get data received function
     * \returns the received data string
     */
    std::string GetDataReceived();

    /**
     * \brief Set the MAC SAP user
     * \param s a pointer to the MAC SAP user
     */
    void SetNrSlMacSapUser(NrSlMacSapUser* s);
    /**
     * \brief Get the MAC SAP provider
     * \return a pointer to the SAP provider of the MAC
     */
    NrSlMacSapProvider* GetNrSlMacSapProvider();

    /**
     * \brief Set PDCP header present function
     * \param present true if PDCP header present
     */
    void SetPdcpHeaderPresent(bool present);

    /**
     * \brief Set RLC header type
     * \param rlcHeaderType the RLC header type
     */
    void SetRlcHeaderType(uint8_t rlcHeaderType);

    /// RCL Header Type enumeration
    enum RlcHeaderType_t
    {
        UM_RLC_HEADER = 0,
        AM_RLC_HEADER = 1,
    };

  private:
    /**
     * \brief send an NR SL RLC PDU to the MAC for transmission. This method is
     * to be called as a response to NrSlMacSapUser::NotifyNrSlTxOpportunity
     *
     * \param params NrSlRlcPduParameters
     */
    void DoTransmitNrSlRlcPdu(const NrSlMacSapProvider::NrSlRlcPduParameters& params);
    /**
     * \brief Report the RLC buffer status to the MAC
     *
     * \param params NrSlReportBufferStatusParameters
     */
    void DoReportNrSlBufferStatus(
        const NrSlMacSapProvider::NrSlReportBufferStatusParameters& params);

    NrSlMacSapUser* m_nrSlMacSapUser{nullptr};         /// MAC SAP user
    NrSlMacSapProvider* m_nrSlMacSapProvider{nullptr}; /// MAC SAP provider
    std::string m_receivedData{""};                    /// The received data string
    uint8_t m_rlcHeaderType{UM_RLC_HEADER};            /// RLC header type
    bool m_pdcpHeaderPresent{false};                   /// PDCP header present flag
};

#endif /* NR_SL_TEST_MOCK_OBJECTS_H */
