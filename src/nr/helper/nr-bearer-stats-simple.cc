// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-bearer-stats-simple.h"

#include "ns3/nstime.h"
#include "ns3/string.h"
#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrBearerStatsSimple");
NS_OBJECT_ENSURE_REGISTERED(NrBearerStatsBase);
NS_OBJECT_ENSURE_REGISTERED(NrBearerStatsSimple);

TypeId
NrBearerStatsBase::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrBearerStatsBase").SetParent<Object>().SetGroupName("nr");
    return tid;
}

void
NrBearerStatsBase::DoDispose()
{
    NS_LOG_FUNCTION(this);
    Object::DoDispose();
}

NrBearerStatsSimple::NrBearerStatsSimple()
    : m_protocolType("RLC")
{
    NS_LOG_FUNCTION(this);
}

NrBearerStatsSimple::NrBearerStatsSimple(std::string protocolType)
{
    NS_LOG_FUNCTION(this);
    m_protocolType = protocolType;
}

NrBearerStatsSimple::~NrBearerStatsSimple()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrBearerStatsSimple::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrBearerStatsSimple")
            .SetParent<NrBearerStatsBase>()
            .AddConstructor<NrBearerStatsSimple>()
            .SetGroupName("nr")
            .AddAttribute("DlRlcTxOutputFilename",
                          "Name of the file where the RLC downlink TX results will be saved.",
                          StringValue("NrDlTxRlcStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_dlRlcTxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("DlRlcRxOutputFilename",
                          "Name of the file where the RLC downlink RX results will be saved.",
                          StringValue("NrDlRxRlcStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_dlRlcRxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("UlRlcTxOutputFilename",
                          "Name of the file where the RLC uplink RX results will be saved.",
                          StringValue("NrUlRlcTxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_ulRlcTxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("UlRlcRxOutputFilename",
                          "Name of the file where the RLC uplink TX results will be saved.",
                          StringValue("NrUlRlcRxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_ulRlcRxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("DlPdcpTxOutputFilename",
                          "Name of the file where the downlink PDCP TX results will be saved.",
                          StringValue("NrDlPdcpTxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_dlPdcpTxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("DlPdcpRxOutputFilename",
                          "Name of the file where the downlink PDCP RX results will be saved.",
                          StringValue("NrDlPdcpRxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_dlPdcpRxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("UlPdcpTxOutputFilename",
                          "Name of the file where the uplink PDCP TX results will be saved.",
                          StringValue("NrUlPdcpTxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_ulPdcpTxOutputFilename),
                          MakeStringChecker())
            .AddAttribute("UlPdcpRxOutputFilename",
                          "Name of the file where the uplink PDCP RX results will be saved.",
                          StringValue("NrUlPdcpRxStats.txt"),
                          MakeStringAccessor(&NrBearerStatsSimple::m_ulPdcpRxOutputFilename),
                          MakeStringChecker());
    return tid;
}

void
NrBearerStatsSimple::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_dlTxOutFile.close(); //!< Output file stream to which DL RLC TX stats will be written
    m_dlRxOutFile.close(); //!< Output file stream to which DL RLC RX stats will be written
    m_ulTxOutFile.close(); //!< Output file stream to which UL RLC TX stats will be written
    m_ulRxOutFile.close(); //!< Output file stream to which UL RLC RX stats will be written
    NrBearerStatsBase::DoDispose();
}

void
NrBearerStatsSimple::UlTxPdu(uint16_t cellId,
                             uint64_t imsi,
                             uint16_t rnti,
                             uint8_t lcid,
                             uint32_t packetSize)
{
    NS_LOG_FUNCTION(this << cellId << imsi << rnti << (uint32_t)lcid << packetSize);

    if (!m_ulTxOutFile.is_open())
    {
        m_ulTxOutFile.open(GetUlTxOutputFilename().c_str());
        m_ulTxOutFile << "time(s)"
                      << "\t"
                      << "cellId"
                      << "\t"
                      << "rnti"
                      << "\t"
                      << "lcid"
                      << "\t"
                      << "packetSize" << std::endl;
    }
    m_ulTxOutFile << Simulator::Now().GetSeconds() << "\t" << cellId << "\t" << rnti << "\t"
                  << (uint32_t)lcid << "\t" << packetSize << std::endl;
}

void
NrBearerStatsSimple::DlTxPdu(uint16_t cellId,
                             uint64_t imsi,
                             uint16_t rnti,
                             uint8_t lcid,
                             uint32_t packetSize)
{
    NS_LOG_FUNCTION(this << cellId << imsi << rnti << (uint32_t)lcid << packetSize);

    if (!m_dlTxOutFile.is_open())
    {
        m_dlTxOutFile.open(GetDlTxOutputFilename().c_str());
        m_dlTxOutFile << "time(s)"
                      << "\t"
                      << "cellId"
                      << "\t"
                      << "rnti"
                      << "\t"
                      << "lcid"
                      << "\t"
                      << "packetSize" << std::endl;
    }

    m_dlTxOutFile << Simulator::Now().GetSeconds() << "\t" << cellId << "\t" << rnti << "\t"
                  << (uint32_t)lcid << "\t" << packetSize << std::endl;
}

void
NrBearerStatsSimple::UlRxPdu(uint16_t cellId,
                             uint64_t imsi,
                             uint16_t rnti,
                             uint8_t lcid,
                             uint32_t packetSize,
                             uint64_t delay)
{
    NS_LOG_FUNCTION(this << cellId << imsi << rnti << (uint32_t)lcid << packetSize << delay);

    if (!m_ulRxOutFile.is_open())
    {
        m_ulRxOutFile.open(GetUlRxOutputFilename().c_str());
        m_ulRxOutFile << "time(s)"
                      << "\t"
                      << "cellId"
                      << "\t"
                      << "rnti"
                      << "\t"
                      << "lcid"
                      << "\t"
                      << "packetSize"
                      << "\t"
                      << "delay(s)" << std::endl;
    }

    m_ulRxOutFile << Simulator::Now().GetSeconds() << "\t" << cellId << "\t" << rnti << "\t"
                  << (uint32_t)lcid << "\t" << packetSize << "\t" << delay * 1e-9 << std::endl;
}

void
NrBearerStatsSimple::DlRxPdu(uint16_t cellId,
                             uint64_t imsi,
                             uint16_t rnti,
                             uint8_t lcid,
                             uint32_t packetSize,
                             uint64_t delay)
{
    NS_LOG_FUNCTION(this << cellId << imsi << rnti << (uint32_t)lcid << packetSize << delay);

    if (!m_dlRxOutFile.is_open())
    {
        m_dlRxOutFile.open(GetDlRxOutputFilename().c_str());
        m_dlRxOutFile << "time(s)"
                      << "\t"
                      << "cellId"
                      << "\t"
                      << "rnti"
                      << "\t"
                      << "lcid"
                      << "\t"
                      << "packetSize"
                      << "\t"
                      << "delay(s)" << std::endl;
    }

    m_dlRxOutFile << Simulator::Now().GetSeconds() << "\t" << cellId << "\t" << rnti << "\t"
                  << (uint32_t)lcid << "\t" << packetSize << "\t" << delay * 1e-9 << std::endl;
}

std::string
NrBearerStatsSimple::GetUlTxOutputFilename()
{
    if (m_protocolType == "RLC")
    {
        return m_ulRlcTxOutputFilename;
    }
    else
    {
        return m_ulPdcpTxOutputFilename;
    }
}

std::string
NrBearerStatsSimple::GetUlRxOutputFilename()
{
    if (m_protocolType == "RLC")
    {
        return m_ulRlcRxOutputFilename;
    }
    else
    {
        return m_ulPdcpRxOutputFilename;
    }
}

std::string
NrBearerStatsSimple::GetDlTxOutputFilename()
{
    if (m_protocolType == "RLC")
    {
        return m_dlRlcTxOutputFilename;
    }
    else
    {
        return m_dlPdcpTxOutputFilename;
    }
}

std::string
NrBearerStatsSimple::GetDlRxOutputFilename()
{
    if (m_protocolType == "RLC")
    {
        return m_dlRlcRxOutputFilename;
    }
    else
    {
        return m_dlPdcpRxOutputFilename;
    }
}

} // namespace ns3
