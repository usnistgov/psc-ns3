// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "sinr-output-stats.h"

#include <ns3/abort.h>
#include <ns3/rng-seed-manager.h>

namespace ns3
{

SinrOutputStats::SinrOutputStats()
{
}

void
SinrOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = m_db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                         " ("
                         "CellId INTEGER NOT NULL, "
                         "BwpId INTEGER NOT NULL,"
                         "Rnti INTEGER NOT NULL,"
                         "AvgSinr DOUBLE NOT NULL,"
                         "Seed INTEGER NOT NULL,"
                         "Run INTEGER NOT NULL);");
    NS_ASSERT(ret);

    SinrOutputStats::DeleteWhere(m_db,
                                 RngSeedManager::GetSeed(),
                                 RngSeedManager::GetRun(),
                                 tableName);
}

void
SinrOutputStats::SaveSinr(uint16_t cellId, uint16_t rnti, double avgSinr, uint16_t bwpId)
{
    m_sinrCache.emplace_back(cellId, bwpId, rnti, avgSinr);

    // Let's wait until ~1MB of entries before storing it in the database
    if (m_sinrCache.size() * sizeof(SinrResultCache) > 1000000)
    {
        WriteCache();
    }
}

void
SinrOutputStats::EmptyCache()
{
    WriteCache();
}

void
SinrOutputStats::DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table)
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

void
SinrOutputStats::WriteCache()
{
    bool ret = m_db->SpinExec("BEGIN TRANSACTION;");
    for (const auto& v : m_sinrCache)
    {
        sqlite3_stmt* stmt;
        ret = m_db->SpinPrepare(&stmt, "INSERT INTO " + m_tableName + " VALUES (?,?,?,?,?,?);");
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 1, v.cellId);
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 2, v.bwpId);
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 3, v.rnti);
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 4, v.avgSinr);
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 5, RngSeedManager::GetSeed());
        NS_ASSERT(ret);
        ret = m_db->Bind(stmt, 6, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ASSERT(ret);

        ret = m_db->SpinExec(stmt);
        NS_ASSERT(ret);
    }
    m_sinrCache.clear();
    ret = m_db->SpinExec("END TRANSACTION;");
    NS_ASSERT(ret);
}

} // namespace ns3
