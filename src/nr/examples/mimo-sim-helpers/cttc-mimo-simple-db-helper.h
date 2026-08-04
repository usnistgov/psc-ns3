// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author: Biljana Bojovic <bbojovic@cttc.es>

#include <ns3/sqlite-output.h>

const uint32_t DB_ATTEMPT_LIMIT = 500;

// how many times to try to perform DB query before giving up,
// we dont want to enter into an infinite loop

namespace ns3
{

struct CttcMimoSimpleResults
{
    double simTime;
    bool enableMimoFeedback;
    double gnbUeDistance;
    uint32_t rngRun;
    std::string pmSearchMethod;
    std::string fullSearchCb;
    uint32_t rankLimit;
    size_t numRowsGnb;
    size_t numRowsUe;
    size_t numColumnsGnb;
    size_t numColumnsUe;
    size_t numVPortsGnb;
    size_t numVPortsUe;
    size_t numHPortsGnb;
    size_t numHPortsUe;
    std::string schedulerType;
    bool isXPolGnb;
    bool isXPolUe;
    double delayMs{0};
    double jitterMs{0};
    double throughputMbps{0};
    uint32_t bytesReceived{0};
    uint32_t bytesTransmitted{0};
    double packetLoss{0};
    double execTimeSec{0};
    double rank{0};
    double mcs{0};
    uint32_t sbPmiUpdateIntervalMs;
    uint32_t wbPmiUpdateIntervalMs;
    bool enableInterfNode;
    uint8_t csiFlags;
    std::string trafficType;
    double xyVelocity;
};

/**
 * \brief The helper class that creates the specific tables in the database,
 * write results, checks if the results exits, etc.
 */
class CttcMimoSimpleDbHelper
{
  public:
    /**
     * \brief Destructror that should close the database if not closed
     */
    ~CttcMimoSimpleDbHelper();
    /**
     * \brief Sets the results dir path
     */
    void SetResultsDirPath(std::string resultsDir);
    /**
     * \brief Sets the DB name
     */
    void SetDbName(std::string dbName);
    /**
     * \brief Prepare the database, i.e., open the database it,
     * and create the table if it does not exist.
     */
    void PrepareTable();
    /**
     * \brief Insert results to the table in database.
     */
    void InsertResults(CttcMimoSimpleResults& results);
    /**
     * \brief Delete results entry from table if already exist
     */
    void DeleteFromTableIfAlreadyExist(CttcMimoSimpleResults& results);

  private:
    // database related attributes
    sqlite3* m_db{nullptr};                      //!< DB pointer
    std::string m_dbName{"cttc-mimo-simple.db"}; //!< Database name
    std::string m_tableName{"e2e"};              //!< Table name
    std::string m_resultsDirPath{""}; //!< The directory in which will be created the database
};

} // namespace ns3
