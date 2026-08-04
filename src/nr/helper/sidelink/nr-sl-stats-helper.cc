// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-stats-helper.h"

#include <ns3/abort.h>
#include <ns3/inet-socket-address.h>
#include <ns3/inet6-socket-address.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/simulator.h>

namespace ns3
{

UeToUePktTxRxOutputStats::UeToUePktTxRxOutputStats()
{
}

void
UeToUePktTxRxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeSec DOUBLE NOT NULL, "
                       "txRx TEXT NOT NULL,"
                       "nodeId INTEGER NOT NULL,"
                       "imsi INTEGER NOT NULL,"
                       "pktSizeBytes INTEGER NOT NULL,"
                       "srcIp TEXT NOT NULL,"
                       "srcPort INTEGER NOT NULL,"
                       "dstIp TEXT NOT NULL,"
                       "dstPort INTEGER NOT NULL,"
                       "pktSeqNum INTEGER NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UeToUePktTxRxOutputStats::DeleteWhere(m_db,
                                          RngSeedManager::GetSeed(),
                                          RngSeedManager::GetRun(),
                                          tableName);
}

void
UeToUePktTxRxOutputStats::Save(const std::string txRx,
                               const Address& localAddrs,
                               uint32_t nodeId,
                               uint64_t imsi,
                               uint32_t pktSize,
                               const Address& srcAddrs,
                               const Address& dstAddrs,
                               uint32_t seq)
{
    m_pktCache.emplace_back(Simulator::Now().GetNanoSeconds() / (double)1e9,
                            txRx,
                            localAddrs,
                            nodeId,
                            imsi,
                            pktSize,
                            srcAddrs,
                            dstAddrs,
                            seq);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_pktCache.size() * sizeof(UePacketResultCache) > 1000000)
    {
        WriteCache();
    }
}

void
UeToUePktTxRxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UeToUePktTxRxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    std::ostringstream oss;

    for (const auto& v : m_pktCache)
    {
        std::string srcStr;
        std::string dstStr;
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt,
                          "INSERT INTO " + m_tableName + " VALUES (?,?,?,?,?,?,?,?,?,?,?,?);");
        oss.str("");
        ret = m_db->Bind(stmt, 1, v.timeSec);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, v.txRx);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.nodeId);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, static_cast<uint32_t>(v.imsi));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, v.pktSize);
        NS_ABORT_UNLESS(ret);
        if (InetSocketAddress::IsMatchingType(v.srcAddrs))
        {
            oss << InetSocketAddress::ConvertFrom(v.srcAddrs).GetIpv4();
            if (oss.str() == "0.0.0.0")
            {
                // srcAddr is not set (is "0.0.0.0")-- most likely a TX packet
                std::ostringstream ip;
                ip << Ipv4Address::ConvertFrom(v.localAddrs);
                srcStr = ip.str();
                ret = m_db->Bind(stmt, 6, srcStr);
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 7, InetSocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                NS_ABORT_UNLESS(ret);
                ip.str("");
                ip << InetSocketAddress::ConvertFrom(v.dstAddrs).GetIpv4();
                dstStr = ip.str();
                ret = m_db->Bind(stmt, 8, dstStr);
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 9, InetSocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 10, v.seq);
                NS_ABORT_UNLESS(ret);
            }
            else
            {
                oss.str("");
                oss << InetSocketAddress::ConvertFrom(v.dstAddrs).GetIpv4();
                if (oss.str() == "0.0.0.0")
                {
                    // dstAddr is not set (is "0.0.0.0")
                    std::ostringstream ip;
                    ip << InetSocketAddress::ConvertFrom(v.srcAddrs).GetIpv4();
                    srcStr = ip.str();
                    ret = m_db->Bind(stmt, 6, srcStr);
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 7, InetSocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ip.str("");
                    ip << Ipv4Address::ConvertFrom(v.localAddrs);
                    dstStr = ip.str();
                    ret = m_db->Bind(stmt, 8, dstStr);
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 9, InetSocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 10, v.seq);
                    NS_ABORT_UNLESS(ret);
                }
                else
                {
                    std::ostringstream ip;
                    ip << InetSocketAddress::ConvertFrom(v.srcAddrs).GetIpv4();
                    srcStr = ip.str();
                    ret = m_db->Bind(stmt, 6, srcStr);
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 7, InetSocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    Ipv4Address dstIpv4Address =
                        InetSocketAddress::ConvertFrom(v.dstAddrs).GetIpv4();
                    if (dstIpv4Address.IsMulticast() || dstIpv4Address.IsBroadcast())
                    {
                        // Use local address as destination address
                        dstIpv4Address = Ipv4Address::ConvertFrom(v.localAddrs);
                    }
                    ip.str("");
                    ip << dstIpv4Address;
                    dstStr = ip.str();
                    ret = m_db->Bind(stmt, 8, dstStr);
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 9, InetSocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 10, v.seq);
                    NS_ABORT_UNLESS(ret);
                }
            }
        }
        else if (Inet6SocketAddress::IsMatchingType(v.srcAddrs))
        {
            oss << Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetIpv6();
            if (oss.str() == "::") // srcAddrs not set
            {
                std::ostringstream ip;
                ip << Ipv6Address::ConvertFrom(v.localAddrs);
                srcStr = ip.str();
                ret = m_db->Bind(stmt, 6, srcStr);
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 7, Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                NS_ABORT_UNLESS(ret);
                ip.str("");
                ip << Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetIpv6();
                dstStr = ip.str();
                ret = m_db->Bind(stmt, 8, dstStr);
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 9, Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                NS_ABORT_UNLESS(ret);
                ret = m_db->Bind(stmt, 10, v.seq);
                NS_ABORT_UNLESS(ret);
            }
            else
            {
                oss.str("");
                oss << Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetIpv6();
                if (oss.str() == "::") // dstAddrs not set
                {
                    std::ostringstream ip;
                    ip << Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetIpv6();
                    srcStr = ip.str();
                    ret = m_db->Bind(stmt, 6, srcStr);
                    NS_ABORT_UNLESS(ret);
                    ret =
                        m_db->Bind(stmt, 7, Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ip.str("");
                    ip << Ipv6Address::ConvertFrom(v.localAddrs);
                    dstStr = ip.str();
                    ret = m_db->Bind(stmt, 8, dstStr);
                    NS_ABORT_UNLESS(ret);
                    ret =
                        m_db->Bind(stmt, 9, Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 10, v.seq);
                    NS_ABORT_UNLESS(ret);
                }
                else
                {
                    std::ostringstream ip;
                    ip << Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetIpv6();
                    srcStr = ip.str();
                    ret = m_db->Bind(stmt, 6, srcStr);
                    NS_ABORT_UNLESS(ret);
                    ret =
                        m_db->Bind(stmt, 7, Inet6SocketAddress::ConvertFrom(v.srcAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ip.str("");
                    ip << Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetIpv6();
                    dstStr = ip.str();
                    ret = m_db->Bind(stmt, 8, dstStr);
                    NS_ABORT_UNLESS(ret);
                    ret =
                        m_db->Bind(stmt, 9, Inet6SocketAddress::ConvertFrom(v.dstAddrs).GetPort());
                    NS_ABORT_UNLESS(ret);
                    ret = m_db->Bind(stmt, 10, v.seq);
                    NS_ABORT_UNLESS(ret);
                }
            }
        }
        else
        {
            NS_FATAL_ERROR("Unknown address type!");
        }

        ret = m_db->Bind(stmt, 11, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 12, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_pktCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UeToUePktTxRxOutputStats::DeleteWhere(SQLiteOutput* p,
                                      uint32_t seed,
                                      uint32_t run,
                                      const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

UeRlcRxOutputStats::UeRlcRxOutputStats()
{
}

void
UeRlcRxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeMs DOUBLE NOT NULL,"
                       "imsi INTEGER NOT NULL,"
                       "rnti INTEGER NOT NULL,"
                       "txRnti INTEGER NOT NULL,"
                       "lcid INTEGER NOT NULL,"
                       "rxPdueSize INTEGER NOT NULL,"
                       "delayMicroSec DOUBLE NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UeRlcRxOutputStats::DeleteWhere(m_db,
                                    RngSeedManager::GetSeed(),
                                    RngSeedManager::GetRun(),
                                    tableName);
}

void
UeRlcRxOutputStats::Save(uint64_t imsi,
                         uint16_t rnti,
                         uint16_t txRnti,
                         uint8_t lcid,
                         uint32_t rxPduSize,
                         double delaySeconds)
{
    UeRlcRxData data(Simulator::Now().GetSeconds() * 1000.0,
                     imsi,
                     rnti,
                     txRnti,
                     lcid,
                     rxPduSize,
                     delaySeconds * 1e6);
    m_rlcRxDataCache.emplace_back(data);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_rlcRxDataCache.size() * sizeof(SlPscchUeMacStatParameters) > 1000000)
    {
        WriteCache();
    }
}

void
UeRlcRxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UeRlcRxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_rlcRxDataCache)
    {
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt, "INSERT INTO " + m_tableName + " VALUES (?,?,?,?,?,?,?,?,?);");
        ret = m_db->Bind(stmt, 1, v.timeMs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, static_cast<uint32_t>(v.imsi));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.rnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, v.txRnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, static_cast<uint16_t>(v.lcid));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 6, v.rxPduSize);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 7, v.delayMicroSeconds);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 8, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 9, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_rlcRxDataCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UeRlcRxOutputStats::DeleteWhere(SQLiteOutput* p,
                                uint32_t seed,
                                uint32_t run,
                                const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);
    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

UeMacPscchTxOutputStats::UeMacPscchTxOutputStats()
{
}

void
UeMacPscchTxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeMs DOUBLE NOT NULL, "
                       "imsi INTEGER NOT NULL,"
                       "rnti INTEGER NOT NULL,"
                       "frame INTEGER NOT NULL,"
                       "subFrame INTEGER NOT NULL,"
                       "slot INTEGER NOT NULL,"
                       "symStart INTEGER NOT NULL,"
                       "symLen INTEGER NOT NULL,"
                       "rbStart INTEGER NOT NULL,"
                       "rbLen INTEGER NOT NULL,"
                       "priority INTEGER NOT NULL,"
                       "mcs INTEGER NOT NULL,"
                       "tbSize INTEGER NOT NULL,"
                       "rsvpMs INTEGER NOT NULL,"
                       "totSbCh INTEGER NOT NULL,"
                       "sbChStart INTEGER NOT NULL,"
                       "sbChLen INTEGER NOT NULL,"
                       "maxNumPerReserve INTEGER NOT NULL,"
                       "gapReTx1 INTEGER NOT NULL,"
                       "gapReTx2 INTEGER NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UeMacPscchTxOutputStats::DeleteWhere(m_db,
                                         RngSeedManager::GetSeed(),
                                         RngSeedManager::GetRun(),
                                         tableName);
}

void
UeMacPscchTxOutputStats::Save(const SlPscchUeMacStatParameters pscchStatsParams)
{
    m_pscchCache.emplace_back(pscchStatsParams);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_pscchCache.size() * sizeof(SlPscchUeMacStatParameters) > 1000000)
    {
        WriteCache();
    }
}

void
UeMacPscchTxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UeMacPscchTxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_pscchCache)
    {
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt,
                          "INSERT INTO " + m_tableName +
                              " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
        ret = m_db->Bind(stmt, 1, v.timeMs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, static_cast<uint32_t>(v.imsi));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.rnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, v.frameNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, v.subframeNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 6, v.slotNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 7, v.symStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 8, v.symLength);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 9, v.rbStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 10, v.rbLength);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 11, v.priority);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 12, v.mcs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 13, v.tbSize);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 14, v.slResourceReservePeriod);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 15, v.totalSubChannels);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 16, v.slPsschSubChStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 17, v.slPsschSubChLength);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 18, v.slMaxNumPerReserve);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 19, v.gapReTx1);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 20, v.gapReTx2);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 21, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 22, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_pscchCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UeMacPscchTxOutputStats::DeleteWhere(SQLiteOutput* p,
                                     uint32_t seed,
                                     uint32_t run,
                                     const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

UeMacPsschTxOutputStats::UeMacPsschTxOutputStats()
{
}

void
UeMacPsschTxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeMs DOUBLE NOT NULL,"
                       "imsi INTEGER NOT NULL,"
                       "rnti INTEGER NOT NULL,"
                       "srcL2Id INTEGER NOT NULL,"
                       "dstL2Id INTEGER NOT NULL,"
                       "frame INTEGER NOT NULL,"
                       "subFrame INTEGER NOT NULL,"
                       "slot INTEGER NOT NULL,"
                       "symStart INTEGER NOT NULL,"
                       "symLen INTEGER NOT NULL,"
                       "sbChSize INTEGER NOT NULL,"
                       "rbStart INTEGER NOT NULL,"
                       "rbLen INTEGER NOT NULL,"
                       "harqId INTEGER NOT NULL,"
                       "ndi INTEGER NOT NULL,"
                       "rv INTEGER NOT NULL,"
                       "reselCounter INTEGER NOT NULL,"
                       "cReselCounter INTEGER NOT NULL,"
                       "csiReq INTEGER NOT NULL,"
                       "castType INTEGER NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UeMacPsschTxOutputStats::DeleteWhere(m_db,
                                         RngSeedManager::GetSeed(),
                                         RngSeedManager::GetRun(),
                                         tableName);
}

void
UeMacPsschTxOutputStats::Save(const SlPsschUeMacStatParameters psschStatsParams)
{
    m_psschCache.emplace_back(psschStatsParams);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_psschCache.size() * sizeof(SlPsschUeMacStatParameters) > 1000000)
    {
        WriteCache();
    }
}

void
UeMacPsschTxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UeMacPsschTxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_psschCache)
    {
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt,
                          "INSERT INTO " + m_tableName +
                              " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
        ret = m_db->Bind(stmt, 1, v.timeMs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, static_cast<uint32_t>(v.imsi));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.rnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, v.srcL2Id);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, v.dstL2Id);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 6, v.frameNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 7, v.subframeNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 8, v.slotNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 9, v.symStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 10, v.symLength);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 11, v.subChannelSize);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 12, v.rbStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 13, v.rbLength);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 14, v.harqId);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 15, v.ndi);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 16, v.rv);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 17, v.resoReselCounter);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 18, v.cReselCounter);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 19, v.csiReq);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 20, v.castType);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 21, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 22, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_psschCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UeMacPsschTxOutputStats::DeleteWhere(SQLiteOutput* p,
                                     uint32_t seed,
                                     uint32_t run,
                                     const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

UePhyPscchRxOutputStats::UePhyPscchRxOutputStats()
{
}

void
UePhyPscchRxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeMs DOUBLE NOT NULL,"
                       "cellId INTEGER NOT NULL,"
                       "rnti INTEGER NOT NULL,"
                       "bwpId INTEGER NOT NULL,"
                       "frame INTEGER NOT NULL,"
                       "subFrame INTEGER NOT NULL,"
                       "slot INTEGER NOT NULL,"
                       "txRnti INTEGER NOT NULL,"
                       "dstL2Id INTEGER NOT NULL,"
                       "pscchRbStart INTEGER NOT NULL,"
                       "pscchRbLen INTEGER NOT NULL,"
                       "pscchMcs INTEGER NOT NULL,"
                       "avrgSinr DOUBLE NOT NULL,"
                       "minSinr DOUBLE NOT NULL,"
                       "tbler INTEGER NOT NULL,"
                       "corrupt INTEGER NOT NULL,"
                       "psschStartSbCh INTEGER NOT NULL,"
                       "psschLenSbCh INTEGER NOT NULL,"
                       "maxNumPerReserve INTEGER NOT NULL,"
                       "rsvpMs INTEGER NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UePhyPscchRxOutputStats::DeleteWhere(m_db,
                                         RngSeedManager::GetSeed(),
                                         RngSeedManager::GetRun(),
                                         tableName);
}

void
UePhyPscchRxOutputStats::Save(const SlRxCtrlPacketTraceParams pscchStatsParams)
{
    m_pscchCache.emplace_back(pscchStatsParams);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_pscchCache.size() * sizeof(SlRxCtrlPacketTraceParams) > 1000000)
    {
        WriteCache();
    }
}

void
UePhyPscchRxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UePhyPscchRxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_pscchCache)
    {
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt,
                          "INSERT INTO " + m_tableName +
                              " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
        ret = m_db->Bind(stmt, 1, v.m_timeMs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, static_cast<uint32_t>(v.m_cellId));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.m_rnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, v.m_bwpId);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, v.m_frameNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 6, v.m_subframeNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 7, v.m_slotNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 8, v.m_txRnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 9, v.m_dstL2Id);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 10, v.m_rbStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 11, v.m_rbAssignedNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 12, v.m_mcs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 13, v.m_sinr);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 14, v.m_sinrMin);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 15, v.m_tbler);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 16, (v.m_corrupt) ? 1 : 0);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 17, v.m_indexStartSubChannel);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 18, v.m_lengthSubChannel);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 19, v.m_maxNumPerReserve);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 20, v.m_slResourceReservePeriod);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 21, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 22, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_pscchCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UePhyPscchRxOutputStats::DeleteWhere(SQLiteOutput* p,
                                     uint32_t seed,
                                     uint32_t run,
                                     const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

UePhyPsschRxOutputStats::UePhyPsschRxOutputStats()
{
}

void
UePhyPsschRxOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "timeMs DOUBLE NOT NULL, "
                       "cellId INTEGER NOT NULL,"
                       "rnti INTEGER NOT NULL,"
                       "bwpId INTEGER NOT NULL,"
                       "frame INTEGER NOT NULL,"
                       "subFrame INTEGER NOT NULL,"
                       "slot INTEGER NOT NULL,"
                       "txRnti INTEGER NOT NULL,"
                       "srcL2Id INTEGER NOT NULL,"
                       "dstL2Id INTEGER NOT NULL,"
                       "psschRbStart INTEGER NOT NULL,"
                       "psschRbLen INTEGER NOT NULL,"
                       "psschSymStart INTEGER NOT NULL,"
                       "psschSymLen INTEGER NOT NULL,"
                       "psschMcs INTEGER NOT NULL,"
                       "ndi INTEGER NOT NULL,"
                       "rv INTEGER NOT NULL,"
                       "tbSizeBytes INTEGER NOT NULL,"
                       "avrgSinr INTEGER NOT NULL,"
                       "minSinr INTEGER NOT NULL,"
                       "psschTbler INTEGER NOT NULL,"
                       "psschCorrupt INTEGER NOT NULL,"
                       "sci2Tbler INTEGER NOT NULL,"
                       "sci2Corrupt INTEGER NOT NULL,"
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL"
                       ");");

    NS_ABORT_UNLESS(ret);

    UePhyPsschRxOutputStats::DeleteWhere(m_db,
                                         RngSeedManager::GetSeed(),
                                         RngSeedManager::GetRun(),
                                         tableName);
}

void
UePhyPsschRxOutputStats::Save(const SlRxDataPacketTraceParams psschStatsParams)
{
    m_psschCache.emplace_back(psschStatsParams);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_psschCache.size() * sizeof(SlRxDataPacketTraceParams) > 1000000)
    {
        WriteCache();
    }
}

void
UePhyPsschRxOutputStats::EmptyCache()
{
    WriteCache();
}

void
UePhyPsschRxOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_psschCache)
    {
        sqlite3_stmt* stmt;
        m_db->SpinPrepare(&stmt,
                          "INSERT INTO " + m_tableName +
                              " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
        ret = m_db->Bind(stmt, 1, v.m_timeMs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, static_cast<uint32_t>(v.m_cellId));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, v.m_rnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, v.m_bwpId);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, v.m_frameNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 6, v.m_subframeNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 7, v.m_slotNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 8, v.m_txRnti);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 9, v.m_srcL2Id);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 10, v.m_dstL2Id);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 11, v.m_rbStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 12, v.m_rbAssignedNum);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 13, v.m_symStart);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 14, v.m_numSym);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 15, v.m_mcs);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 16, v.m_ndi);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 17, v.m_rv);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 18, v.m_tbSize);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 19, v.m_sinr);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 20, v.m_sinrMin);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 21, v.m_tbler);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 22, (v.m_corrupt) ? 1 : 0);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 23, v.m_tblerSci2);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 24, (v.m_sci2Corrupted) ? 1 : 0);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 25, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 26, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);

        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    m_psschCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ABORT_UNLESS(ret);
}

void
UePhyPsschRxOutputStats::DeleteWhere(SQLiteOutput* p,
                                     uint32_t seed,
                                     uint32_t run,
                                     const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

} // namespace ns3
