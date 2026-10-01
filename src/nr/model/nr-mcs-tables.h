// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_MCS_TABLES_H
#define NR_MCS_TABLES_H

#include <cstdint>
#include <vector>

namespace ns3
{

/**
 * @ingroup error-models
 * @brief MCS tables per TS 38.214
 *
 * Provides static access to MCS-to-modulation-order and MCS-to-code-rate
 * mappings for 5G NR. Used by both TBS calculation and error models.
 *
 * Two MCS tables are supported:
 * - Table 1: TS 38.214 Table 5.1.3.1-1 (up to 64QAM, 29 MCS values 0-28)
 * - Table 2: TS 38.214 Table 5.1.3.1-2 (up to 256QAM, 28 MCS values 0-27)
 *
 * CQI tables are also provided:
 * - CQI Table 1: TS 38.214 Table 5.2.2.1-2
 * - CQI Table 2: TS 38.214 Table 5.2.2.1-3
 */
class NrMcsTables
{
  public:
    /**
     * @brief Get modulation order (Q_m) for MCS index
     * @param mcs MCS index (0-28 for Table 1, 0-27 for Table 2)
     * @param table MCS table (1 or 2, per TS 38.214 Tables 5.1.3.1-1/2)
     * @return Modulation order (2=QPSK, 4=16QAM, 6=64QAM, 8=256QAM)
     */
    static uint8_t GetModulationOrder(uint8_t mcs, uint8_t table);

    /**
     * @brief Get effective code rate (R) for MCS index
     * @param mcs MCS index
     * @param table MCS table (1 or 2)
     * @return Effective code rate (a value between 0 and 1)
     */
    static double GetCodeRate(uint8_t mcs, uint8_t table);

    /**
     * @brief Get spectral efficiency for MCS index
     * @param mcs MCS index
     * @param table MCS table (1 or 2)
     * @return Spectral efficiency (Q_m * R)
     */
    static double GetSpectralEfficiencyForMcs(uint8_t mcs, uint8_t table);

    /**
     * @brief Get spectral efficiency for CQI index
     * @param cqi CQI index (0-15)
     * @param table CQI table (1 or 2)
     * @return Spectral efficiency
     */
    static double GetSpectralEfficiencyForCqi(uint8_t cqi, uint8_t table);

    /**
     * @brief Get maximum MCS index for table
     * @param table MCS table (1 or 2)
     * @return Maximum valid MCS index (28 for Table 1, 27 for Table 2)
     */
    static uint8_t GetMaxMcs(uint8_t table);

    /**
     * @brief Get reference to the modulation order table
     * @param table MCS table (1 or 2)
     * @return Const reference to the modulation order vector
     */
    static const std::vector<uint8_t>& GetMcsMTableRef(uint8_t table);

    /**
     * @brief Get reference to the code rate (ECR) table
     * @param table MCS table (1 or 2)
     * @return Const reference to the code rate vector
     */
    static const std::vector<double>& GetMcsEcrTableRef(uint8_t table);

    /**
     * @brief Get reference to the spectral efficiency for MCS table
     * @param table MCS table (1 or 2)
     * @return Const reference to the spectral efficiency vector
     */
    static const std::vector<double>& GetSpectralEfficiencyForMcsRef(uint8_t table);

    /**
     * @brief Get reference to the spectral efficiency for CQI table
     * @param table CQI table (1 or 2)
     * @return Const reference to the spectral efficiency vector
     */
    static const std::vector<double>& GetSpectralEfficiencyForCqiRef(uint8_t table);

  private:
    /// MCS Table 1 modulation orders (TS 38.214 Table 5.1.3.1-1)
    static const std::vector<uint8_t> m_mcsMTable1;
    /// MCS Table 1 code rates
    static const std::vector<double> m_mcsEcrTable1;
    /// MCS Table 1 spectral efficiencies
    static const std::vector<double> m_spectralEfficiencyForMcs1;
    /// CQI Table 1 spectral efficiencies (TS 38.214 Table 5.2.2.1-2)
    static const std::vector<double> m_spectralEfficiencyForCqi1;

    /// MCS Table 2 modulation orders (TS 38.214 Table 5.1.3.1-2)
    static const std::vector<uint8_t> m_mcsMTable2;
    /// MCS Table 2 code rates
    static const std::vector<double> m_mcsEcrTable2;
    /// MCS Table 2 spectral efficiencies
    static const std::vector<double> m_spectralEfficiencyForMcs2;
    /// CQI Table 2 spectral efficiencies (TS 38.214 Table 5.2.2.1-3)
    static const std::vector<double> m_spectralEfficiencyForCqi2;
};

} // namespace ns3

#endif // NR_MCS_TABLES_H
