// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author: Biljana Bojovic <bbojovic@cttc.es>

#include "cttc-mimo-simple-db-helper.h"

#include "ns3/nr-module.h"
#include "ns3/stats-module.h"
#include <ns3/sqlite-output.h>

#include <chrono>
#include <thread>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CttcMimoSimpleDbHelper");

void
CttcMimoSimpleDbHelper::SetResultsDirPath(std::string resultsDir)
{
    m_resultsDirPath = resultsDir;
}

void
CttcMimoSimpleDbHelper::SetDbName(std::string dbName)
{
    m_dbName = dbName;
}

void
CttcMimoSimpleDbHelper::PrepareTable()
{
    NS_LOG_FUNCTION(this);

    int rc = sqlite3_open((m_resultsDirPath + "/" + m_dbName).c_str(), &m_db);
    NS_ABORT_MSG_UNLESS(rc == SQLITE_OK, "Failed to open DB");

    std::string cmd = "CREATE TABLE IF NOT EXISTS " + m_tableName +
                      " ("
                      "SimTime            DOUBLE NOT NULL,"
                      "EnableMimoFeedback INTEGER NOT NULL,"
                      "GnbUeDistance      DOUBLE NOT NULL,"
                      "RngRun             INTEGER NOT NULL,"
                      "PmSearchMethod     TEXT NOT NULL,"
                      "FullSearchCb       TEXT NOT NULL,"
                      "RankLimit          INTEGER NOT NULL,"
                      "NumRowsGnb         INTEGER NOT NULL,"
                      "NumRowsUe          INTEGER NOT NULL,"
                      "NumColumnsGnb      INTEGER NOT NULL,"
                      "NumColumnsUe       INTEGER NOT NULL,"
                      "NumVPortsGnb       INTEGER NOT NULL,"
                      "NumVPortsUe        INTEGER NOT NULL,"
                      "NumHPortsGnb       INTEGER NOT NULL,"
                      "NumHPortsUe        INTEGER NOT NULL,"
                      "IsXPolGnb          INTEGER NOT NULL,"
                      "IsXPolUe           INTEGER NOT NULL,"
                      "SchedulerType      TEXT NOT NULL,"
                      "SbPmiInterval      INTEGER NOT NULL,"
                      "WbPmiInterval      INTEGER NOT NULL,"
                      "EnableInterfNode   INTEGER NOT NULL,"
                      "CsiFlags           INTEGER NOT NULL,"
                      "TrafficType        TEXT NOT NULL,"
                      "XyVelocity         DOUBLE NOT NULL,"
                      "DelayMs            DOUBLE NOT NULL,"
                      "JitterMs           DOUBLE NOT NULL,"
                      "ThroughputMbps     DOUBLE NOT NULL,"
                      "BytesReceived      INTEGER NOT NULL,"
                      "BytesTransmitted   INTEGER NOT NULL,"
                      "PacketLoss         DOUBLE NOT NULL,"
                      "Rank               DOUBLE NOT NULL,"
                      "Mcs                DOUBLE NOT NULL,"
                      "ExecTimeSec        DOUBLE NOT NULL"
                      ");";

    sqlite3_stmt* stmt;

    // prepare the statement for creating the table
    uint32_t attemptCount = 0;
    do
    {
        rc = sqlite3_prepare_v2(m_db, cmd.c_str(), static_cast<int>(cmd.size()), &stmt, nullptr);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not prepare correctly the statement for creating the table. Db error:"
            << sqlite3_errmsg(m_db) << "full command is: \n"
            << cmd);

    // execute a step operation on a statement until the result is ok or an error
    attemptCount = 0;
    do
    {
        rc = sqlite3_step(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement for creating the table. Db error:"
            << sqlite3_errmsg(m_db));

    // finalize the statement until the result is ok or an error occurs
    attemptCount = 0;
    do
    {
        rc = sqlite3_finalize(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement for creating the table. Db error:"
            << sqlite3_errmsg(m_db));
}

void
CttcMimoSimpleDbHelper::InsertResults(CttcMimoSimpleResults& results)
{
    NS_LOG_FUNCTION(this);

    DeleteFromTableIfAlreadyExist(results);
    sqlite3_stmt* stmt;
    std::string cmd = "INSERT INTO " + m_tableName +
                      " VALUES ("
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?, ?, ?, "
                      "?, ?, ?);";
    int rc;
    // prepare the statement for creating the table
    uint32_t attemptCount = 0;
    do
    {
        rc = sqlite3_prepare_v2(m_db, cmd.c_str(), static_cast<int>(cmd.size()), &stmt, nullptr);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(rc == SQLITE_OK || rc == SQLITE_DONE,
                        "Could not prepare correctly the insert into the table statement. "
                        " Db error:"
                            << sqlite3_errmsg(m_db) << ". The full command is: \n"
                            << cmd);

    // add all parameters to the command
    int i = 1;
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.simTime) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.enableMimoFeedback) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.gnbUeDistance) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.rngRun) == SQLITE_OK);
    NS_ABORT_UNLESS(
        sqlite3_bind_text(stmt, i++, results.pmSearchMethod.c_str(), -1, SQLITE_STATIC) ==
        SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_text(stmt, i++, results.fullSearchCb.c_str(), -1, SQLITE_STATIC) ==
                    SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.rankLimit) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numRowsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numRowsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numColumnsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numColumnsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numVPortsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numVPortsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numHPortsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numHPortsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.isXPolGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.isXPolUe) == SQLITE_OK);
    NS_ABORT_UNLESS(
        sqlite3_bind_text(stmt, i++, results.schedulerType.c_str(), -1, SQLITE_STATIC) ==
        SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.sbPmiUpdateIntervalMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.wbPmiUpdateIntervalMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.enableInterfNode) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.csiFlags) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_text(stmt, i++, results.trafficType.c_str(), -1, SQLITE_STATIC) ==
                    SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.xyVelocity) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.delayMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.jitterMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.throughputMbps) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.bytesReceived) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.bytesTransmitted) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.packetLoss) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.rank) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.mcs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.execTimeSec) == SQLITE_OK);

    // finalize the command
    attemptCount = 0;
    do
    {
        rc = sqlite3_step(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement. Db error:" << sqlite3_errmsg(m_db));
    attemptCount = 0;
    do
    {
        rc = sqlite3_finalize(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement. Db error:" << sqlite3_errmsg(m_db));
}

void
CttcMimoSimpleDbHelper::DeleteFromTableIfAlreadyExist(CttcMimoSimpleResults& results)
{
    sqlite3_stmt* stmt;
    std::string cmd = "DELETE FROM \"" + m_tableName +
                      "\" WHERE "
                      "SimTime            == ? AND " // 1
                      "EnableMimoFeedback == ? AND " // 2
                      "GnbUeDistance      == ? AND " // 3
                      "RngRun             == ? AND " // 4
                      "PmSearchMethod     == ? AND " // 5
                      "FullSearchCb       == ? AND " // 6
                      "RankLimit          == ? AND " // 7
                      "NumRowsGnb         == ? AND " // 8
                      "NumRowsUe          == ? AND " // 9
                      "NumColumnsGnb      == ? AND " // 10
                      "NumColumnsUe       == ? AND " // 11
                      "NumVPortsGnb       == ? AND " // 12
                      "NumVPortsUe        == ? AND " // 13
                      "NumHPortsGnb       == ? AND " // 14
                      "NumHPortsUe        == ? AND " // 15
                      "IsXPolGnb          == ? AND " // 16
                      "IsXPolUe           == ? AND " // 17
                      "SchedulerType      == ? AND " // 18
                      "SbPmiInterval      == ? AND " // 19
                      "WbPmiInterval      == ? AND " // 20
                      "EnableInterfNode   == ? AND " // 21
                      "CsiFlags           == ? AND " // 22
                      "TrafficType        == ? AND " // 23
                      "XyVelocity         == ? ;";   // 24

    int rc;

    // prepare the statement for creating the table
    uint32_t attemptCount = 0;
    do
    {
        rc = sqlite3_prepare_v2(m_db, cmd.c_str(), static_cast<int>(cmd.size()), &stmt, nullptr);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(rc == SQLITE_OK || rc == SQLITE_DONE,
                        "Could not prepare correctly the delete statement. "
                        " Db error:"
                            << sqlite3_errmsg(m_db) << ". The full command is: \n"
                            << cmd);

    // add all parameters to the command
    int i = 1;
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.simTime) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.enableMimoFeedback) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.gnbUeDistance) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.rngRun) == SQLITE_OK);
    NS_ABORT_UNLESS(
        sqlite3_bind_text(stmt, i++, results.pmSearchMethod.c_str(), -1, SQLITE_STATIC) ==
        SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_text(stmt, i++, results.fullSearchCb.c_str(), -1, SQLITE_STATIC) ==
                    SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.rankLimit) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numRowsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numRowsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numColumnsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numColumnsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numVPortsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numVPortsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numHPortsGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.numHPortsUe) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.isXPolGnb) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.isXPolUe) == SQLITE_OK);
    NS_ABORT_UNLESS(
        sqlite3_bind_text(stmt, i++, results.schedulerType.c_str(), -1, SQLITE_STATIC) ==
        SQLITE_OK);

    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.sbPmiUpdateIntervalMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.wbPmiUpdateIntervalMs) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.enableInterfNode) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_int(stmt, i++, results.csiFlags) == SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_text(stmt, i++, results.trafficType.c_str(), -1, SQLITE_STATIC) ==
                    SQLITE_OK);
    NS_ABORT_UNLESS(sqlite3_bind_double(stmt, i++, results.xyVelocity) == SQLITE_OK);

    // finalize the command
    attemptCount = 0;
    do
    {
        rc = sqlite3_step(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // check if it went correctly
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement. Db error:" << sqlite3_errmsg(m_db));
    attemptCount = 0;
    do
    {
        rc = sqlite3_finalize(stmt);
        NS_ABORT_MSG_IF(++attemptCount == DB_ATTEMPT_LIMIT,
                        "Waiting too much for sqlite3 database to be ready. "
                        "Check if you have the database/table open with another program. "
                        "If yes, close it before running again your program.\n\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    NS_ABORT_MSG_UNLESS(
        rc == SQLITE_OK || rc == SQLITE_DONE,
        "Could not correctly execute the statement. Db error:" << sqlite3_errmsg(m_db));
}

CttcMimoSimpleDbHelper::~CttcMimoSimpleDbHelper()
{
    // Failed to close the database
    int rc = SQLITE_FAIL;
    rc = sqlite3_close_v2(m_db);
    NS_ABORT_MSG_UNLESS(rc == SQLITE_OK, "Failed to close DB");
}
