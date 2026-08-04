/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NR_TEST_ENTITIES_H
#define NR_TEST_ENTITIES_H

#include "ns3/net-device.h"
#include "ns3/nr-mac-sap.h"
#include "ns3/nr-pdcp-sap.h"
#include "ns3/nr-rlc-sap.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include <ns3/nr-epc-gnb-s1-sap.h>

namespace ns3
{

/**
 * \ingroup nr-test
 *
 * \brief This class implements a testing RRC entity
 */
class NrTestRrc : public Object
{
    /// allow NrPdcpSpecificNrPdcpSapUser<NrTestRrc> class friend access
    friend class NrPdcpSpecificNrPdcpSapUser<NrTestRrc>;
    //   friend class GnbMacMemberNrGnbCmacSapProvider;
    //   friend class GnbMacMemberNrMacSapProvider<NrTestMac>;
    //   friend class GnbMacMemberFfMacSchedSapUser;
    //   friend class GnbMacMemberFfMacCschedSapUser;
    //   friend class GnbMacMemberNrGnbPhySapUser;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrTestRrc();
    ~NrTestRrc() override;
    void DoDispose() override;

    /**
     * \brief Set the PDCP SAP provider
     * \param s a pointer to the PDCP SAP provider
     */
    void SetNrPdcpSapProvider(NrPdcpSapProvider* s);
    /**
     * \brief Get the PDCP SAP user
     * \return a pointer to the SAP user of the RLC
     */
    NrPdcpSapUser* GetNrPdcpSapUser();

    /// Start function
    void Start();
    /// Stop function
    void Stop();

    /**
     * \brief Send data function
     * \param at the time to send
     * \param dataToSend the data to send
     */
    void SendData(Time at, std::string dataToSend);
    /**
     * \brief Get data received function
     * \returns the received data string
     */
    std::string GetDataReceived();

    // Stats
    /**
     * \brief Get the transmit PDUs
     * \return the number of transmit PDUS
     */
    uint32_t GetTxPdus();
    /**
     * \brief Get the transmit bytes
     * \return the number of bytes transmitted
     */
    uint32_t GetTxBytes();
    /**
     * \brief Get the receive PDUs
     * \return the number of receive PDUS
     */
    uint32_t GetRxPdus();
    /**
     * \brief Get the receive bytes
     * \return the number of bytes received
     */
    uint32_t GetRxBytes();

    /**
     * \brief Get the last transmit time
     * \return the time of the last transmit
     */
    Time GetTxLastTime();
    /**
     * \brief Get the last receive time
     * \return the time of the last receive
     */
    Time GetRxLastTime();

    /**
     * \brief Set the arrival time
     * \param arrivalTime the arrival time
     */
    void SetArrivalTime(Time arrivalTime);
    /**
     * \brief Set the PDU size
     * \param pduSize the PDU size
     */
    void SetPduSize(uint32_t pduSize);

    /**
     * \brief Set the device
     * \param device the device
     */
    void SetDevice(Ptr<NetDevice> device);

  private:
    /**
     * Interface forwarded by NrPdcpSapUser
     * \param params the NrPdcpSapUser::ReceivePdcpSduParameters
     */
    virtual void DoReceivePdcpSdu(NrPdcpSapUser::ReceivePdcpSduParameters params);

    NrPdcpSapUser* m_pdcpSapUser;         ///< PDCP SAP user
    NrPdcpSapProvider* m_pdcpSapProvider; ///< PDCP SAP provider

    std::string m_receivedData; ///< the received data

    uint32_t m_txPdus;  ///< number of transmit PDUs
    uint32_t m_txBytes; ///< number of transmit bytes
    uint32_t m_rxPdus;  ///< number of receive PDUs
    uint32_t m_rxBytes; ///< number of receive bytes
    Time m_txLastTime;  ///< last transmit time
    Time m_rxLastTime;  ///< last reeive time

    EventId m_nextPdu;  ///< next PDU event
    Time m_arrivalTime; ///< next arrival time
    uint32_t m_pduSize; ///< PDU size

    Ptr<NetDevice> m_device; ///< the device
};

/////////////////////////////////////////////////////////////////////

/**
 * \ingroup nr-test
 *
 * \brief This class implements a testing PDCP entity
 */
class NrTestPdcp : public Object
{
    /// allow NrRlcSpecificNrRlcSapUser<NrTestPdcp> class friend access
    friend class NrRlcSpecificNrRlcSapUser<NrTestPdcp>;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrTestPdcp();
    ~NrTestPdcp() override;
    void DoDispose() override;

    /**
     * \brief Set the RLC SAP provider
     * \param s a pointer to the RLC SAP provider
     */
    void SetNrRlcSapProvider(NrRlcSapProvider* s);
    /**
     * \brief Get the RLC SAP user
     * \return a pointer to the SAP user of the RLC
     */
    NrRlcSapUser* GetNrRlcSapUser();

    /// Start function
    void Start();

    /**
     * \brief Send data function
     * \param time the time to send
     * \param dataToSend the data to send
     */
    void SendData(Time time, std::string dataToSend);
    /**
     * \brief Get data received function
     * \returns the received data string
     */
    std::string GetDataReceived();

  private:
    /**
     * Interface forwarded by NrRlcSapUser
     * \param p the PDCP PDU packet received
     */
    virtual void DoReceivePdcpPdu(Ptr<Packet> p);

    NrRlcSapUser* m_rlcSapUser;         ///< RLC SAP user
    NrRlcSapProvider* m_rlcSapProvider; ///< RLC SAP provider

    std::string m_receivedData; ///< the received data
};

/////////////////////////////////////////////////////////////////////

/**
 * \ingroup nr-test
 *
 * \brief This class implements a testing loopback MAC layer
 */
class NrTestMac : public Object
{
    //   friend class GnbMacMemberNrGnbCmacSapProvider;
    /// allow GnbMacMemberNrMacSapProvider<NrTestMac> class friend access
    friend class GnbMacMemberNrMacSapProvider<NrTestMac>;
    //   friend class GnbMacMemberFfMacSchedSapUser;
    //   friend class GnbMacMemberFfMacCschedSapUser;
    //   friend class GnbMacMemberNrGnbPhySapUser;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrTestMac();
    ~NrTestMac() override;
    void DoDispose() override;

    /**
     * \brief Set the device function
     * \param device the device
     */
    void SetDevice(Ptr<NetDevice> device);

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
     * \brief the Receive function
     * \param nd the device
     * \param p the packet
     * \param protocol the protocol
     * \param addr the address
     * \returns true if successful
     */
    bool Receive(Ptr<NetDevice> nd, Ptr<const Packet> p, uint16_t protocol, const Address& addr);

    /**
     * \brief Set the MAC SAP user
     * \param s a pointer to the MAC SAP user
     */
    void SetNrMacSapUser(NrMacSapUser* s);
    /**
     * \brief Get the MAC SAP provider
     * \return a pointer to the SAP provider of the MAC
     */
    NrMacSapProvider* GetNrMacSapProvider();

    /**
     * \brief Set the other side of the MAC Loopback
     * \param s a pointer to the other side of the MAC loopback
     */
    void SetNrMacLoopback(Ptr<NrTestMac> s);

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

    /**
     * Set transmit opportunity mode
     * \param mode the transmit opportunity mode
     */
    void SetTxOpportunityMode(uint8_t mode);

    /// Transmit opportunity mode enumeration
    enum TxOpportunityMode_t
    {
        MANUAL_MODE = 0,
        AUTOMATIC_MODE = 1,
        RANDOM_MODE = 2
    };

    /**
     * Set transmit opportunity time
     * \param txOppTime the transmit opportunity time
     */
    void SetTxOppTime(Time txOppTime);
    /**
     * Set transmit opportunity time
     * \param txOppSize the transmit opportunity size
     */
    void SetTxOppSize(uint32_t txOppSize);

    // Stats
    /**
     * \brief Get the transmit PDUs
     * \return the number of transmit PDUS
     */
    uint32_t GetTxPdus();
    /**
     * \brief Get the transmit bytes
     * \return the number of bytes transmitted
     */
    uint32_t GetTxBytes();
    /**
     * \brief Get the receive PDUs
     * \return the number of receive PDUS
     */
    uint32_t GetRxPdus();
    /**
     * \brief Get the receive bytes
     * \return the number of bytes received
     */
    uint32_t GetRxBytes();

  private:
    // forwarded from NrMacSapProvider
    /**
     * Transmit PDU
     * \param params NrMacSapProvider::TransmitPduParameters
     */
    void DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params);
    /**
     * Report buffer status function
     * \param params NrMacSapProvider::ReportBufferStatusParameters
     */
    void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params);

    NrMacSapProvider* m_macSapProvider; ///< MAC SAP provider
    NrMacSapUser* m_macSapUser;         ///< MAC SAP user
    Ptr<NrTestMac> m_macLoopback;       ///< MAC loopback

    std::string m_receivedData; ///< the received data string

    uint8_t m_rlcHeaderType;     ///< RLC header type
    bool m_pdcpHeaderPresent;    ///< PDCP header present?
    uint8_t m_txOpportunityMode; ///< transmit opportunity mode

    Ptr<NetDevice> m_device; ///< the device

    // TxOpportunity configuration
    EventId m_nextTxOpp;                ///< next transmit opportunity event
    Time m_txOppTime;                   ///< transmit opportunity time
    uint32_t m_txOppSize;               ///< transmit opportunity size
    std::list<EventId> m_nextTxOppList; ///< next transmit opportunity list

    // Stats
    uint32_t m_txPdus;  ///< the number of transmit PDUs
    uint32_t m_txBytes; ///< the number of transmit bytes
    uint32_t m_rxPdus;  ///< the number of receive PDUs
    uint32_t m_rxBytes; ///< the number of receive bytes
};

/**
 * \ingroup nr-test
 *
 * \brief RRC stub providing a testing S1 SAP user to be used with the NrEpcGnbApplication
 */
class NrEpcTestRrc : public Object
{
    /// allow NrMemberEpcGnbS1SapUser<NrEpcTestRrc> class friend access
    friend class NrMemberEpcGnbS1SapUser<NrEpcTestRrc>;

  public:
    NrEpcTestRrc();
    ~NrEpcTestRrc() override;

    // inherited from Object
    void DoDispose() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set the S1 SAP Provider
     *
     * \param s the S1 SAP Provider
     */
    void SetS1SapProvider(NrEpcGnbS1SapProvider* s);

    /**
     *
     * \return the S1 SAP user
     */
    NrEpcGnbS1SapUser* GetS1SapUser();

  private:
    // S1 SAP methods
    /**
     * Initial context setup request
     * \param params NrEpcGnbS1SapUser::InitialContextSetupRequestParameters
     */
    void DoInitialContextSetupRequest(
        NrEpcGnbS1SapUser::InitialContextSetupRequestParameters params);
    /**
     * Data radio bearer setup request
     * \param params NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters
     */
    void DoDataRadioBearerSetupRequest(
        NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters params);
    /**
     * Path switch request acknowledge function
     * \param params NrEpcGnbS1SapUser::PathSwitchRequestAcknowledgeParameters
     */
    void DoPathSwitchRequestAcknowledge(
        NrEpcGnbS1SapUser::PathSwitchRequestAcknowledgeParameters params);

    NrEpcGnbS1SapProvider* m_s1SapProvider; ///< S1 SAP provider
    NrEpcGnbS1SapUser* m_s1SapUser;         ///< S1 SAP user
};

} // namespace ns3

#endif /* NR_TEST_MAC_H */
