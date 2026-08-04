// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_STATS_HELPER_H
#define NR_SL_STATS_HELPER_H

#include <ns3/address.h>
#include <ns3/nr-sl-phy-mac-common.h>
#include <ns3/sqlite-output.h>

#include <limits>
#include <string>
#include <vector>

namespace ns3
{

/**
 * \brief Class to listen and store the application level traces of type
 *        TxWithAddresses and RxWithAddresses into a database from an NR
 *        V2X simulation. This class expects both TX and RX nodes to be
 *        a UE, hence, apart from the parameters of the trace source, it methods
 *        require some additional parameters, .e.g., IMSI of the UEs.
 *
 * \see SetDb
 * \see Save
 */
class UeToUePktTxRxOutputStats
{
  public:
    /**
     * \brief UeToUePktTxRxOutputStats constructor
     */
    UeToUePktTxRxOutputStats();

    /**
     * \brief Install the output database for packet TX and RX traces from ns-3
     *        apps. In particular, the traces TxWithAddresses and RxWithAddresses.
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeSec DOUBLE NOT NULL, "
     * - "txRx TEXT NOT NULL,"
     * - "nodeId INTEGER NOT NULL,"
     * - "imsi INTEGER NOT NULL,"
     * - "pktSizeBytes INTEGER NOT NULL,"
     * - "srcIp TEXT NOT NULL,"
     * - "srcPort TEXT NOT NULL,"
     * - "dstIp TEXT NOT NULL,"
     * - "dstPort TEXT NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the packet transmissions and receptions from the application
     *        layer in the database.
     *
     * The parameter 'localAddrs' is passed to this trace in case the
     * address passed by the trace is not set (i.e., is '0.0.0.0' or '::').
     *
     * \param txRx The string indicating the type of node, i.e., TX or RX
     * \param localAddrs The local IPV4 address of the node
     * \param nodeId The node id
     * \param imsi The IMSI
     * \param pktSize The packet size
     * \param srcAddrs The source address from the trace
     * \param dstAddrs The destination address from the trace
     * \param seq The packet sequence number
     */
    void Save(const std::string txRx,
              const Address& localAddrs,
              uint32_t nodeId,
              uint64_t imsi,
              uint32_t pktSize,
              const Address& srcAddrs,
              const Address& dstAddrs,
              uint32_t seq);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \ingroup nr
     * \brief UePacketResultCache struct to cache the information communicated
     *        by TX/RX application layer traces.
     */
    struct UePacketResultCache
    {
        /**
         * \brief UePacketResultCache constructor
         * \param timeSec The time in seconds
         * \param txRx The string indicating the type of node, i.e., TX or RX
         * \param localAddrs The local IPV4 address of the node
         * \param nodeId The node id of the TX or RX node
         * \param imsi The IMSI of the UE
         * \param pktSize The packet size
         * \param srcAddrs The source address from the trace
         * \param dstAddrs The destination address from the trace
         * \param seq The sequence number of the packet
         */
        UePacketResultCache(double timeSec,
                            std::string txRx,
                            Address localAddrs,
                            uint32_t nodeId,
                            uint64_t imsi,
                            uint32_t pktSize,
                            Address srcAddrs,
                            Address dstAddrs,
                            uint32_t seq)
            : timeSec(timeSec),
              txRx(txRx),
              localAddrs(localAddrs),
              nodeId(nodeId),
              imsi(imsi),
              pktSize(pktSize),
              srcAddrs(srcAddrs),
              dstAddrs(dstAddrs),
              seq(seq)
        {
        }

        double timeSec{0.0};  //!< The time in seconds
        std::string txRx{""}; //!< The string indicating the type of node, i.e., TX or RX
        Address localAddrs;   //!< The local IPV4 address of the node
        uint32_t nodeId{std::numeric_limits<uint32_t>::max()}; //!< The node id of the TX or RX node
        uint64_t imsi{std::numeric_limits<uint64_t>::max()};   //!< The IMSI of the UE
        uint32_t pktSize;                                      //!< The packet size
        Address srcAddrs; //!< The source address from the trace
        Address dstAddrs; //!< The destination address from the trace
        uint32_t seq{std::numeric_limits<uint32_t>::max()}; //!< The sequence number of the packet
    };

    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                 //!< DB pointer
    std::string m_tableName{"InvalidTableName"}; //!< table name
    std::vector<UePacketResultCache> m_pktCache; //!< Result cache
};

/**
 * \brief Class to listen and store the RxRlcPduWithTxRnti trace of NrUeMac
 *        into a database.
 *
 * We added this trace in NR UE MAC because we were unable to figure out how to use
 * ObjectMapValue for nested SL DRB maps in NrSlUeRrc class. This map should be
 * an attribute to hook functions to the RLC and PDCP traces.
 *
 * \see SetDb
 * \see Save
 */
class UeRlcRxOutputStats
{
  public:
    /**
     * \brief UeRlcRxOutputStats constructor
     */
    UeRlcRxOutputStats();

    /**
     * \brief Install the output database for RxRlcPduWithTxRnti trace from NrUeMac.
     *
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeMs DOUBLE NOT NULL,"
     * - "imsi INTEGER NOT NULL,"
     * - "rnti INTEGER NOT NULL,"
     * - "txRnti INTEGER NOT NULL,"
     * - "lcid INTEGER NOT NULL,"
     * - "rxPdueSize INTEGER NOT NULL,"
     * - "delayNsec INTEGER NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the RxRlcPduWithTxRnti trace parameters into a local vector, which
     *        acts as a cache.
     *
     * \param imsi
     * \param rnti
     * \param txRnti
     * \param lcid
     * \param rxPduSize
     * \param delaySeconds
     */
    void Save(uint64_t imsi,
              uint16_t rnti,
              uint16_t txRnti,
              uint8_t lcid,
              uint32_t rxPduSize,
              double delaySeconds);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \brief The UeRlcRxData struct to cache the information communicated by
     *        RxRlcPduWithTxRnti trace in NrUeMac
     */
    struct UeRlcRxData
    {
        /**
         * \brief UeRlcRxData constructor
         * \param timeMs The time in milliseconds
         * \param imsi The IMSI of the UE
         * \param rnti The RNTI of the UE
         * \param txRnti The RNTI of the TX UE
         * \param lcid The logical channel id
         * \param rxPduSize The received PDU size in bytes
         * \param delayMicroSeconds The end-to-end, i.e., from TX RLC entity to RX
         *        RLC entity, delay in microseconds
         */
        UeRlcRxData(double timeMs,
                    uint64_t imsi,
                    uint16_t rnti,
                    uint16_t txRnti,
                    uint8_t lcid,
                    uint32_t rxPduSize,
                    int64_t delayMicroSeconds)
            : timeMs(timeMs),
              imsi(imsi),
              rnti(rnti),
              txRnti(txRnti),
              lcid(lcid),
              rxPduSize(rxPduSize),
              delayMicroSeconds(delayMicroSeconds)
        {
        }

        double timeMs{0.0};                                    //!< timeMs The time in milliseconds
        uint64_t imsi{std::numeric_limits<uint64_t>::max()};   //!< The IMSI of the UE
        uint16_t rnti{std::numeric_limits<uint16_t>::max()};   //!< The RNTI of the UE
        uint16_t txRnti{std::numeric_limits<uint16_t>::max()}; //!< The RNTI of the TX UE
        uint8_t lcid{std::numeric_limits<uint8_t>::max()};     //!< The logical channel id
        uint32_t rxPduSize{
            std::numeric_limits<uint32_t>::max()}; //!< The received PDU size in bytes
        double delayMicroSeconds{0}; //!< The end-to-end, i.e., from TX RLC entity to RX RLC entity,
                                     //!< delay in microseconds
    };

    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                 //!< DB pointer
    std::string m_tableName{"InvalidTableName"}; //!< table name
    std::vector<UeRlcRxData> m_rlcRxDataCache;   //!< Result cache
};

/**
 * \brief Class to listen and store the SlPscchScheduling trace of NrUeMac
 *        into a database.
 *
 * \see SetDb
 * \see Save
 */
class UeMacPscchTxOutputStats
{
  public:
    /**
     * \brief UeMacPscchTxOutputStats constructor
     */
    UeMacPscchTxOutputStats();

    /**
     * \brief Install the output database for PSCCH trace from NrUeMac.
     *
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeMs DOUBLE NOT NULL, "
     * - "imsi INTEGER NOT NULL,"
     * - "rnti INTEGER NOT NULL,"
     * - "frame INTEGER NOT NULL,"
     * - "subFrame INTEGER NOT NULL,"
     * - "slot INTEGER NOT NULL,"
     * - "symStart INTEGER NOT NULL,"
     * - "symLen INTEGER NOT NULL,"
     * - "rbStart INTEGER NOT NULL,"
     * - "rbLen INTEGER NOT NULL,"
     * - "priority INTEGER NOT NULL,"
     * - "mcs INTEGER NOT NULL,"
     * - "tbSize INTEGER NOT NULL,"
     * - "rsvpMs INTEGER NOT NULL,"
     * - "totSbCh INTEGER NOT NULL,"
     * - "sbChStart INTEGER NOT NULL,"
     * - "sbChLen INTEGER NOT NULL,"
     * - "maxNumPerReserve INTEGER NOT NULL,"
     * - "gapReTx1 INTEGER NOT NULL,"
     * - "gapReTx2 INTEGER NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the PSCCH stats parameters into a local vector, which
     *        acts as a cache.
     *
     * \param pscchStatsParams The PSCCH stats parameters
     */
    void Save(const SlPscchUeMacStatParameters pscchStatsParams);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                          //!< DB pointer
    std::string m_tableName{"InvalidTableName"};          //!< table name
    std::vector<SlPscchUeMacStatParameters> m_pscchCache; //!< Result cache
};

/**
 * \brief Class to listen and store the SlPsschScheduling trace of NrUeMac
 *        into a database.
 *
 * \see SetDb
 * \see Save
 */
class UeMacPsschTxOutputStats
{
  public:
    /**
     * \brief UeMacPsschTxOutputStats constructor
     */
    UeMacPsschTxOutputStats();

    /**
     * \brief Install the output database for PSSCH trace from NrUeMac.
     *
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeMs DOUBLE NOT NULL,"
     * - "imsi INTEGER NOT NULL,"
     * - "rnti INTEGER NOT NULL,"
     * - "srcL2Id INTEGER NOT NULL,"
     * - "dstL2Id INTEGER NOT NULL,"
     * - "frame INTEGER NOT NULL,"
     * - "subFrame INTEGER NOT NULL,"
     * - "slot INTEGER NOT NULL,"
     * - "symStart INTEGER NOT NULL,"
     * - "symLen INTEGER NOT NULL,"
     * - "sbChSize INTEGER NOT NULL,"
     * - "rbStart INTEGER NOT NULL,"
     * - "rbLen INTEGER NOT NULL,"
     * - "harqId INTEGER NOT NULL,"
     * - "ndi INTEGER NOT NULL,"
     * - "rv INTEGER NOT NULL,"
     * - "reselCounter INTEGER NOT NULL,"
     * - "cReselCounter INTEGER NOT NULL,"
     * - "csiReq INTEGER NOT NULL,"
     * - "castType INTEGER NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the PSSCH stats parameters into a local vector, which
     *        acts as a cache.
     *
     * \param psschStatsParams The PSSCH stats parameters
     */
    void Save(const SlPsschUeMacStatParameters psschStatsParams);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                          //!< DB pointer
    std::string m_tableName{"InvalidTableName"};          //!< table name
    std::vector<SlPsschUeMacStatParameters> m_psschCache; //!< Result cache
};

/**
 * \brief Class to listen and store the RxPscchTraceUe trace of NrSpectrumPhy
 *        into a database.
 *
 * \see SetDb
 * \see Save
 */
class UePhyPscchRxOutputStats
{
  public:
    /**
     * \brief UePhyPscchRxOutputStats constructor
     */
    UePhyPscchRxOutputStats();

    /**
     * \brief Install the output database for PSCCH RX trace from NrSpectrumPhy.
     *
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeMs DOUBLE NOT NULL, "
     * - "cellId INTEGER NOT NULL,"
     * - "rnti INTEGER NOT NULL,"
     * - "bwpId INTEGER NOT NULL,"
     * - "frame INTEGER NOT NULL,"
     * - "subFrame INTEGER NOT NULL,"
     * - "slot INTEGER NOT NULL,"
     * - "txRnti INTEGER NOT NULL,"
     * - "dstL2Id INTEGER NOT NULL,"
     * - "pscchRbStart INTEGER NOT NULL,"
     * - "pscchRbLen INTEGER NOT NULL,"   *
     * - "pscchMcs INTEGER NOT NULL,"
     * - "avrgSinr INTEGER NOT NULL,"
     * - "minSinr INTEGER NOT NULL,"
     * - "tbler INTEGER NOT NULL,"
     * - "corrupt INTEGER NOT NULL,"
     * - "psschStartSbCh INTEGER NOT NULL,"
     * - "psschLenSbCh INTEGER NOT NULL,"
     * - "maxNumPerReserve INTEGER NOT NULL,"
     * - "rsvpMs INTEGER NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the PSCCH stats parameters into a local vector, which
     *        acts as a cache.
     *
     * \param pscchStatsParams The PSCCH stats parameters
     */
    void Save(const SlRxCtrlPacketTraceParams pscchStatsParams);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                         //!< DB pointer
    std::string m_tableName{"InvalidTableName"};         //!< table name
    std::vector<SlRxCtrlPacketTraceParams> m_pscchCache; //!< Result cache
};

/**
 * \brief Class to listen and store the RxPsschTraceUe trace of NrSpectrumPhy
 *        into a database.
 *
 * \see SetDb
 * \see Save
 */
class UePhyPsschRxOutputStats
{
  public:
    /**
     * \brief UePhyPsschRxOutputStats constructor
     */
    UePhyPsschRxOutputStats();

    /**
     * \brief Install the output database for PSSCH RX trace from NrSpectrumPhy.
     *
     * \param db database pointer
     * \param tableName name of the table where the values will be stored
     *
     * The db pointer must be valid through all the lifespan of the class. The
     * method creates, if not exists, a table for storing the values. The table
     * will contain the following columns:
     *
     * - "timeMs DOUBLE NOT NULL, "
     * - "cellId INTEGER NOT NULL,"
     * - "rnti INTEGER NOT NULL,"
     * - "bwpId INTEGER NOT NULL,"
     * - "frame INTEGER NOT NULL,"
     * - "subFrame INTEGER NOT NULL,"
     * - "slot INTEGER NOT NULL,"
     * - "txRnti INTEGER NOT NULL,"
     * - "srcL2Id INTEGER NOT NULL,"
     * - "dstL2Id INTEGER NOT NULL,"
     * - "psschRbStart INTEGER NOT NULL,"
     * - "psschRbLen INTEGER NOT NULL,"
     * - "psschSymStart INTEGER NOT NULL,"
     * - "psschSymLen INTEGER NOT NULL,"
     * - "psschMcs INTEGER NOT NULL,"
     * - "ndi INTEGER NOT NULL,"
     * - "rv INTEGER NOT NULL,"
     * - "tbSizeBytes INTEGER NOT NULL,"
     * - "avrgSinr INTEGER NOT NULL,"
     * - "minSinr INTEGER NOT NULL,"
     * - "psschTbler INTEGER NOT NULL,"
     * - "psschCorrupt INTEGER NOT NULL,"
     * - "sci2Tbler INTEGER NOT NULL,"
     * - "sci2Corrupt INTEGER NOT NULL,"
     * - "SEED INTEGER NOT NULL,"
     * - "RUN INTEGER NOT NULL"
     *
     * Please note that this method, if the db already contains a table with
     * the same name, also clean existing values that has the same
     * Seed/Run pair.
     */
    void SetDb(SQLiteOutput* db, const std::string& tableName);

    /**
     * \brief Store the PSSCH stats parameters into a local vector, which
     *        acts as a cache.
     *
     * \param psschStatsParams The PSSCH stats parameters
     */
    void Save(const SlRxDataPacketTraceParams psschStatsParams);

    /**
     * \brief Force the cache write to disk, emptying the cache itself.
     */
    void EmptyCache();

  private:
    /**
     * \brief Delete the table if it already exists with same seed and run number
     * \param p The pointer to the DB
     * \param seed The seed index
     * \param run The run index
     * \param table The name of the table
     */
    static void DeleteWhere(SQLiteOutput* p, uint32_t seed, uint32_t run, const std::string& table);
    /**
     * \brief Write the data stored in our local cache into the DB
     */
    void WriteCache();

    SQLiteOutput* m_db{nullptr};                         //!< DB pointer
    std::string m_tableName{"InvalidTableName"};         //!< table name
    std::vector<SlRxDataPacketTraceParams> m_psschCache; //!< Result cache
};

} // namespace ns3

#endif // NR_SL_STATS_HELPER_H
