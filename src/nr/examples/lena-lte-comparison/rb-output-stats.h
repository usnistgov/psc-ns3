// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef RB_OUTPUT_STATS_H
#define RB_OUTPUT_STATS_H

#include <ns3/sfnsf.h>
#include <ns3/sqlite-output.h>

#include <vector>

namespace ns3
{

/**
 * \brief Class to collect and store the Resource Block statistics from a simulation
 *
 * \see SetDb
 * \see SaveSinr
 * \see EmptyCache
 */
class RbOutputStats
{
  public:
    /**
     * \brief Constructor
     */
    RbOutputStats();

    /**
     * \brief Install the output database.
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     *  The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "(Frame INTEGER NOT NULL, "
     * - "SubFrame INTEGER NOT NULL,"
     * - "Slot INTEGER NOT NULL,"
     * - "Symbol INTEGER NOT NULL,"
     * - "RBIndexActive INTEGER NOT NULL,"
     * - "BwpId INTEGER NOT NULL,"
     * - "CellId INTEGER NOT NULL,"
     * - "Seed INTEGER NOT NULL,"
     * - "Run INTEGER NOT NULL);"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName = "rbStats");

    /**
     * \brief Save the slot statistics
     * \param [in] sfnSf Slot number
     * \param [in] sym Symbol
     * \param [in] rbUsed RB used
     */
    void SaveRbStats(const SfnSf& sfnSf,
                     uint8_t sym,
                     const std::vector<int> rbUsed,
                     uint16_t bwpId,
                     uint16_t cellId);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);

    void WriteCache();

    struct RbCache
    {
        SfnSf sfnSf;
        uint8_t sym;
        std::vector<int> rbUsed;
        uint16_t cellId;
        uint16_t bwpId;

        uint32_t GetSize() const
        {
            return sizeof(RbCache) + (rbUsed.size() * sizeof(int));
        }
    };

    SQLiteOutput* m_db;               //!< DB pointer
    std::vector<RbCache> m_slotCache; //!< Result cache
    std::string m_tableName;          //!< Table name
};

} // namespace ns3

#endif // RB_OUTPUT_STATS_H
