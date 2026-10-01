// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-bearer-stats-connector.h"

#include "nr-bearer-stats-calculator.h"

#include <ns3/config.h>
#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrBearerStatsConnector");

/**
 * Less than operator for CellIdRnti, because it is used as key in map
 */
bool
operator<(const NrBearerStatsConnector::CellIdRnti& a, const NrBearerStatsConnector::CellIdRnti& b)
{
    return ((a.cellId < b.cellId) || ((a.cellId == b.cellId) && (a.rnti < b.rnti)));
}

/**
 * This structure is used as interface between trace
 * sources and NrBearerStatsCalculator. It stores
 * and provides calculators with cellId and IMSI,
 * because most trace sources do not provide it.
 */
struct NrBoundCallbackArgument : public SimpleRefCount<NrBoundCallbackArgument>
{
  public:
    Ptr<NrBearerStatsBase> stats; //!< statistics calculator
    uint64_t imsi;                //!< imsi
    uint16_t cellId;              //!< cellId
};

/**
 * Callback function for DL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
DlTxPduCallback(Ptr<NrBoundCallbackArgument> arg,
                std::string path,
                uint16_t rnti,
                uint8_t lcid,
                uint32_t packetSize)
{
    NS_LOG_FUNCTION(path << rnti << (uint16_t)lcid << packetSize);
    arg->stats->DlTxPdu(arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for DL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
DlRxPduCallback(Ptr<NrBoundCallbackArgument> arg,
                std::string path,
                uint16_t rnti,
                uint8_t lcid,
                uint32_t packetSize,
                uint64_t delay)
{
    NS_LOG_FUNCTION(path << rnti << (uint16_t)lcid << packetSize << delay);
    arg->stats->DlRxPdu(arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}

/**
 * Callback function for UL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
UlTxPduCallback(Ptr<NrBoundCallbackArgument> arg,
                std::string path,
                uint16_t rnti,
                uint8_t lcid,
                uint32_t packetSize)
{
    NS_LOG_FUNCTION(path << rnti << (uint16_t)lcid << packetSize);

    arg->stats->UlTxPdu(arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for UL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
UlRxPduCallback(Ptr<NrBoundCallbackArgument> arg,
                std::string path,
                uint16_t rnti,
                uint8_t lcid,
                uint32_t packetSize,
                uint64_t delay)
{
    NS_LOG_FUNCTION(path << rnti << (uint16_t)lcid << packetSize << delay);

    arg->stats->UlRxPdu(arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}

NrBearerStatsConnector::NrBearerStatsConnector()
    : m_connected(false)
{
}

void
NrBearerStatsConnector::EnableRlcStats(Ptr<NrBearerStatsBase> rlcStats)
{
    m_rlcStats = rlcStats;
    EnsureConnected();
}

void
NrBearerStatsConnector::EnablePdcpStats(Ptr<NrBearerStatsBase> pdcpStats)
{
    m_pdcpStats = pdcpStats;
    EnsureConnected();
}

void
NrBearerStatsConnector::EnsureConnected()
{
    NS_LOG_FUNCTION(this);
    if (!m_connected)
    {
        Config::Connect("/NodeList/*/DeviceList/*/NrGnbRrc/NewUeContext",
                        MakeBoundCallback(&NrBearerStatsConnector::NotifyNewUeContextGnb, this));
        Config::Connect(
            "/NodeList/*/DeviceList/*/NrUeRrc/RandomAccessSuccessful",
            MakeBoundCallback(&NrBearerStatsConnector::NotifyRandomAccessSuccessfulUe, this));
        Config::Connect(
            "/NodeList/*/DeviceList/*/NrGnbRrc/ConnectionReconfiguration",
            MakeBoundCallback(&NrBearerStatsConnector::NotifyConnectionReconfigurationGnb, this));
        Config::Connect(
            "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionReconfiguration",
            MakeBoundCallback(&NrBearerStatsConnector::NotifyConnectionReconfigurationUe, this));
        Config::Connect("/NodeList/*/DeviceList/*/NrGnbRrc/HandoverStart",
                        MakeBoundCallback(&NrBearerStatsConnector::NotifyHandoverStartGnb, this));
        Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/HandoverStart",
                        MakeBoundCallback(&NrBearerStatsConnector::NotifyHandoverStartUe, this));
        Config::Connect("/NodeList/*/DeviceList/*/NrGnbRrc/HandoverEndOk",
                        MakeBoundCallback(&NrBearerStatsConnector::NotifyHandoverEndOkGnb, this));
        Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/HandoverEndOk",
                        MakeBoundCallback(&NrBearerStatsConnector::NotifyHandoverEndOkUe, this));
        m_connected = true;
    }
}

void
NrBearerStatsConnector::NotifyRandomAccessSuccessfulUe(NrBearerStatsConnector* c,
                                                       std::string context,
                                                       uint64_t imsi,
                                                       uint16_t cellId,
                                                       uint16_t rnti)
{
    c->ConnectSrb0Traces(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyConnectionSetupUe(NrBearerStatsConnector* c,
                                                std::string context,
                                                uint64_t imsi,
                                                uint16_t cellId,
                                                uint16_t rnti)
{
    c->ConnectSrb1TracesUe(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyConnectionReconfigurationUe(NrBearerStatsConnector* c,
                                                          std::string context,
                                                          uint64_t imsi,
                                                          uint16_t cellId,
                                                          uint16_t rnti)
{
    c->ConnectTracesUeIfFirstTime(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyHandoverStartUe(NrBearerStatsConnector* c,
                                              std::string context,
                                              uint64_t imsi,
                                              uint16_t cellId,
                                              uint16_t rnti,
                                              uint16_t targetCellId)
{
    c->DisconnectTracesUe(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyHandoverEndOkUe(NrBearerStatsConnector* c,
                                              std::string context,
                                              uint64_t imsi,
                                              uint16_t cellId,
                                              uint16_t rnti)
{
    c->ConnectTracesUe(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyNewUeContextGnb(NrBearerStatsConnector* c,
                                              std::string context,
                                              uint16_t cellId,
                                              uint16_t rnti)
{
    c->StoreUeManagerPath(context, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyConnectionReconfigurationGnb(NrBearerStatsConnector* c,
                                                           std::string context,
                                                           uint64_t imsi,
                                                           uint16_t cellId,
                                                           uint16_t rnti)
{
    c->ConnectTracesGnbIfFirstTime(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyHandoverStartGnb(NrBearerStatsConnector* c,
                                               std::string context,
                                               uint64_t imsi,
                                               uint16_t cellId,
                                               uint16_t rnti,
                                               uint16_t targetCellId)
{
    c->DisconnectTracesGnb(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::NotifyHandoverEndOkGnb(NrBearerStatsConnector* c,
                                               std::string context,
                                               uint64_t imsi,
                                               uint16_t cellId,
                                               uint16_t rnti)
{
    c->ConnectTracesGnb(context, imsi, cellId, rnti);
}

void
NrBearerStatsConnector::StoreUeManagerPath(std::string context, uint16_t cellId, uint16_t rnti)
{
    NS_LOG_FUNCTION(this << context << cellId << rnti);
    std::ostringstream ueManagerPath;
    ueManagerPath << context.substr(0, context.rfind('/')) << "/UeMap/" << (uint32_t)rnti;
    CellIdRnti key;
    key.cellId = cellId;
    key.rnti = rnti;
    m_ueManagerPathByCellIdRnti[key] = ueManagerPath.str();
}

void
NrBearerStatsConnector::ConnectSrb0Traces(std::string context,
                                          uint64_t imsi,
                                          uint16_t cellId,
                                          uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
    std::string ueRrcPath = context.substr(0, context.rfind('/'));
    CellIdRnti key;
    key.cellId = cellId;
    key.rnti = rnti;
    std::map<CellIdRnti, std::string>::iterator it = m_ueManagerPathByCellIdRnti.find(key);
    NS_ASSERT(it != m_ueManagerPathByCellIdRnti.end());
    std::string ueManagerPath = it->second;
    NS_LOG_LOGIC(this << " ueManagerPath: " << ueManagerPath);
    m_ueManagerPathByCellIdRnti.erase(it);

    if (m_rlcStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_rlcStats;

        // disconnect eventually previously connected SRB0 both at UE and gNB
        Config::Disconnect(ueRrcPath + "/Srb0/NrRlc/TxPDU",
                           MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Disconnect(ueRrcPath + "/Srb0/NrRlc/RxPDU",
                           MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Disconnect(ueManagerPath + "/Srb0/NrRlc/TxPDU",
                           MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Disconnect(ueManagerPath + "/Srb0/NrRlc/RxPDU",
                           MakeBoundCallback(&UlRxPduCallback, arg));

        // connect SRB0 both at UE and gNB
        Config::Connect(ueRrcPath + "/Srb0/NrRlc/TxPDU", MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Connect(ueRrcPath + "/Srb0/NrRlc/RxPDU", MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Connect(ueManagerPath + "/Srb0/NrRlc/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(ueManagerPath + "/Srb0/NrRlc/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));

        // connect SRB1 at gNB only (at UE SRB1 will be setup later)
        Config::Connect(ueManagerPath + "/Srb1/NrRlc/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(ueManagerPath + "/Srb1/NrRlc/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
    }
    if (m_pdcpStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_pdcpStats;

        // connect SRB1 at gNB only (at UE SRB1 will be setup later)
        Config::Connect(ueManagerPath + "/Srb1/NrPdcp/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
        Config::Connect(ueManagerPath + "/Srb1/NrPdcp/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
    }
}

void
NrBearerStatsConnector::ConnectSrb1TracesUe(std::string ueRrcPath,
                                            uint64_t imsi,
                                            uint16_t cellId,
                                            uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
    if (m_rlcStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_rlcStats;
        Config::Connect(ueRrcPath + "/Srb1/NrRlc/TxPDU", MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Connect(ueRrcPath + "/Srb1/NrRlc/RxPDU", MakeBoundCallback(&DlRxPduCallback, arg));
    }
    if (m_pdcpStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_pdcpStats;
        Config::Connect(ueRrcPath + "/Srb1/NrPdcp/RxPDU", MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Connect(ueRrcPath + "/Srb1/NrPdcp/TxPDU", MakeBoundCallback(&UlTxPduCallback, arg));
    }
}

void
NrBearerStatsConnector::ConnectTracesUeIfFirstTime(std::string context,
                                                   uint64_t imsi,
                                                   uint16_t cellId,
                                                   uint16_t rnti)
{
    NS_LOG_FUNCTION(this << context);
    if (m_imsiSeenUe.find(imsi) == m_imsiSeenUe.end())
    {
        m_imsiSeenUe.insert(imsi);
        ConnectTracesUe(context, imsi, cellId, rnti);
    }
}

void
NrBearerStatsConnector::ConnectTracesGnbIfFirstTime(std::string context,
                                                    uint64_t imsi,
                                                    uint16_t cellId,
                                                    uint16_t rnti)
{
    NS_LOG_FUNCTION(this << context);
    if (m_imsiSeenGnb.find(imsi) == m_imsiSeenGnb.end())
    {
        m_imsiSeenGnb.insert(imsi);
        ConnectTracesGnb(context, imsi, cellId, rnti);
    }
}

void
NrBearerStatsConnector::ConnectTracesUe(std::string context,
                                        uint64_t imsi,
                                        uint16_t cellId,
                                        uint16_t rnti)
{
    NS_LOG_FUNCTION(this << context);
    NS_LOG_LOGIC(this << "expected context should match /NodeList/*/DeviceList/*/NrUeRrc/");
    std::string basePath = context.substr(0, context.rfind('/'));
    if (m_rlcStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_rlcStats;
        Config::Connect(basePath + "/DataRadioBearerMap/*/NrRlc/TxPDU",
                        MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Connect(basePath + "/DataRadioBearerMap/*/NrRlc/RxPDU",
                        MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Connect(basePath + "/Srb1/NrRlc/TxPDU", MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Connect(basePath + "/Srb1/NrRlc/RxPDU", MakeBoundCallback(&DlRxPduCallback, arg));
    }
    if (m_pdcpStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_pdcpStats;
        Config::Connect(basePath + "/DataRadioBearerMap/*/NrPdcp/RxPDU",
                        MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Connect(basePath + "/DataRadioBearerMap/*/NrPdcp/TxPDU",
                        MakeBoundCallback(&UlTxPduCallback, arg));
        Config::Connect(basePath + "/Srb1/NrPdcp/RxPDU", MakeBoundCallback(&DlRxPduCallback, arg));
        Config::Connect(basePath + "/Srb1/NrPdcp/TxPDU", MakeBoundCallback(&UlTxPduCallback, arg));
    }
}

void
NrBearerStatsConnector::ConnectTracesGnb(std::string context,
                                         uint64_t imsi,
                                         uint16_t cellId,
                                         uint16_t rnti)
{
    NS_LOG_FUNCTION(this << context);
    NS_LOG_LOGIC(this << "expected context  should match /NodeList/*/DeviceList/*/NrGnbRrc/");
    std::ostringstream basePath;
    basePath << context.substr(0, context.rfind('/')) << "/UeMap/" << (uint32_t)rnti;
    if (m_rlcStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_rlcStats;
        Config::Connect(basePath.str() + "/DataRadioBearerMap/*/NrRlc/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
        Config::Connect(basePath.str() + "/DataRadioBearerMap/*/NrRlc/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb0/NrRlc/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb0/NrRlc/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb1/NrRlc/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb1/NrRlc/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
    }
    if (m_pdcpStats)
    {
        Ptr<NrBoundCallbackArgument> arg = Create<NrBoundCallbackArgument>();
        arg->imsi = imsi;
        arg->cellId = cellId;
        arg->stats = m_pdcpStats;
        Config::Connect(basePath.str() + "/DataRadioBearerMap/*/NrPdcp/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(basePath.str() + "/DataRadioBearerMap/*/NrPdcp/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb1/NrPdcp/TxPDU",
                        MakeBoundCallback(&DlTxPduCallback, arg));
        Config::Connect(basePath.str() + "/Srb1/NrPdcp/RxPDU",
                        MakeBoundCallback(&UlRxPduCallback, arg));
    }
}

void
NrBearerStatsConnector::DisconnectTracesUe(std::string context,
                                           uint64_t imsi,
                                           uint16_t cellId,
                                           uint16_t rnti)
{
    NS_LOG_FUNCTION(this);
}

void
NrBearerStatsConnector::DisconnectTracesGnb(std::string context,
                                            uint64_t imsi,
                                            uint16_t cellId,
                                            uint16_t rnti)
{
    NS_LOG_FUNCTION(this);
}

Ptr<NrBearerStatsBase>
NrBearerStatsConnector::GetRlcStats()
{
    return m_rlcStats;
}

Ptr<NrBearerStatsBase>
NrBearerStatsConnector::GetPdcpStats()
{
    return m_pdcpStats;
}

} // namespace ns3
