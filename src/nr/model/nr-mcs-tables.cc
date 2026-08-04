// Copyright (c) 2025 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mcs-tables.h"

#include "ns3/abort.h"

namespace ns3
{

/**
 * @brief Table of modulation order (Q_m) for MCS Table 1
 *
 * 29 MCS values (0-28) as per TS 38.214 Table 5.1.3.1-1
 */
const std::vector<uint8_t> NrMcsTables::m_mcsMTable1 = {
    // QPSK (Q_m = 2)
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    // 16QAM (Q_m = 4)
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    // 64QAM (Q_m = 6)
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6};

/**
 * @brief Table of Effective Code Rate (ECR) for MCS Table 1
 *
 * Code rates R * 1024 from TS 38.214 Table 5.1.3.1-1.
 */
const std::vector<double> NrMcsTables::m_mcsEcrTable1 = {
    // QPSK (Q_m = 2): MCS 0-9
    120.0 / 1024.0, // MCS 0
    157.0 / 1024.0, // MCS 1
    193.0 / 1024.0, // MCS 2
    251.0 / 1024.0, // MCS 3
    308.0 / 1024.0, // MCS 4
    379.0 / 1024.0, // MCS 5
    449.0 / 1024.0, // MCS 6
    526.0 / 1024.0, // MCS 7
    602.0 / 1024.0, // MCS 8
    679.0 / 1024.0, // MCS 9
    // 16QAM (Q_m = 4): MCS 10-16
    340.0 / 1024.0, // MCS 10
    378.0 / 1024.0, // MCS 11
    434.0 / 1024.0, // MCS 12
    490.0 / 1024.0, // MCS 13
    553.0 / 1024.0, // MCS 14
    616.0 / 1024.0, // MCS 15
    658.0 / 1024.0, // MCS 16
    // 64QAM (Q_m = 6): MCS 17-28
    438.0 / 1024.0, // MCS 17
    466.0 / 1024.0, // MCS 18
    517.0 / 1024.0, // MCS 19
    567.0 / 1024.0, // MCS 20
    616.0 / 1024.0, // MCS 21
    666.0 / 1024.0, // MCS 22
    719.0 / 1024.0, // MCS 23
    772.0 / 1024.0, // MCS 24
    822.0 / 1024.0, // MCS 25
    873.0 / 1024.0, // MCS 26
    910.0 / 1024.0, // MCS 27
    948.0 / 1024.0  // MCS 28
};

/**
 * @brief Table of spectral efficiency for MCS Table 1
 *
 * Spectral efficiency = Q_m * R for each MCS index.
 * 29 MCS values (0-28) as per TS 38.214 Table 5.1.3.1-1
 */
const std::vector<double> NrMcsTables::m_spectralEfficiencyForMcs1 = {
    // QPSK (Q_m = 2): MCS 0-9
    0.2344,
    0.3066,
    0.3770,
    0.4902,
    0.6016,
    0.7402,
    0.8770,
    1.0273,
    1.1758,
    1.3262,
    // 16QAM (Q_m = 4): MCS 10-16
    1.3281,
    1.4766,
    1.6953,
    1.9141,
    2.1602,
    2.4063,
    2.5703,
    // 64QAM (Q_m = 6): MCS 17-28
    2.5664,
    2.7305,
    3.0293,
    3.3223,
    3.6094,
    3.9023,
    4.2129,
    4.5234,
    4.8164,
    5.1152,
    5.3320,
    5.5547};

/**
 * @brief Table of spectral efficiency for CQI Table 1
 *
 * 16 CQI values (0-15) as per TS 38.214 Table 5.2.2.1-2
 */
const std::vector<double> NrMcsTables::m_spectralEfficiencyForCqi1 = {
    0.0,    // CQI 0: out of range
    0.1523, // CQI 1
    0.2344, // CQI 2
    0.3770, // CQI 3
    0.6016, // CQI 4
    0.8770, // CQI 5
    1.1758, // CQI 6
    1.4766, // CQI 7
    1.9141, // CQI 8
    2.4063, // CQI 9
    2.7305, // CQI 10
    3.3223, // CQI 11
    3.9023, // CQI 12
    4.5234, // CQI 13
    5.1152, // CQI 14
    5.5547  // CQI 15
};

/**
 * @brief Table of modulation order (Q_m) for MCS Table 2
 *
 * 28 MCS values (0-27) as per TS 38.214 Table 5.1.3.1-2
 */
const std::vector<uint8_t> NrMcsTables::m_mcsMTable2 = {
    // QPSK (Q_m = 2): MCS 0-4
    2,
    2,
    2,
    2,
    2,
    // 16QAM (Q_m = 4): MCS 5-10
    4,
    4,
    4,
    4,
    4,
    4,
    // 64QAM (Q_m = 6): MCS 11-19
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    // 256QAM (Q_m = 8): MCS 20-27
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8};

/**
 * @brief Table of Effective Code Rate (ECR) for MCS Table 2
 *
 * Code rates from TS 38.214 Table 5.1.3.1-2.
 */
const std::vector<double> NrMcsTables::m_mcsEcrTable2 = {
    // QPSK (Q_m = 2): MCS 0-4
    120.0 / 1024.0, // MCS 0
    193.0 / 1024.0, // MCS 1
    308.0 / 1024.0, // MCS 2
    449.0 / 1024.0, // MCS 3
    602.0 / 1024.0, // MCS 4
    // 16QAM (Q_m = 4): MCS 5-10
    378.0 / 1024.0, // MCS 5
    434.0 / 1024.0, // MCS 6
    490.0 / 1024.0, // MCS 7
    553.0 / 1024.0, // MCS 8
    616.0 / 1024.0, // MCS 9
    658.0 / 1024.0, // MCS 10
    // 64QAM (Q_m = 6): MCS 11-19
    466.0 / 1024.0, // MCS 11
    517.0 / 1024.0, // MCS 12
    567.0 / 1024.0, // MCS 13
    616.0 / 1024.0, // MCS 14
    666.0 / 1024.0, // MCS 15
    719.0 / 1024.0, // MCS 16
    772.0 / 1024.0, // MCS 17
    822.0 / 1024.0, // MCS 18
    873.0 / 1024.0, // MCS 19
    // 256QAM (Q_m = 8): MCS 20-27
    682.5 / 1024.0, // MCS 20
    711.0 / 1024.0, // MCS 21
    754.0 / 1024.0, // MCS 22
    797.0 / 1024.0, // MCS 23
    841.0 / 1024.0, // MCS 24
    885.0 / 1024.0, // MCS 25
    916.5 / 1024.0, // MCS 26
    948.0 / 1024.0  // MCS 27
};

/**
 * @brief Table of spectral efficiency for MCS Table 2
 *
 * Spectral efficiency = Q_m * R for each MCS index.
 * 28 MCS values (0-27) as per TS 38.214 Table 5.1.3.1-2
 */
const std::vector<double> NrMcsTables::m_spectralEfficiencyForMcs2 = {
    // QPSK (Q_m = 2): MCS 0-4
    0.2344,
    0.3770,
    0.6016,
    0.8770,
    1.1758,
    // 16QAM (Q_m = 4): MCS 5-10
    1.4766,
    1.6953,
    1.9141,
    2.1602,
    2.4063,
    2.5703,
    // 64QAM (Q_m = 6): MCS 11-19
    2.7305,
    3.0293,
    3.3223,
    3.6094,
    3.9023,
    4.2129,
    4.5234,
    4.8164,
    5.1152,
    // 256QAM (Q_m = 8): MCS 20-27
    5.3320,
    5.5547,
    5.8906,
    6.2266,
    6.5703,
    6.9141,
    7.1602,
    7.4063};

/**
 * @brief Table of spectral efficiency for CQI Table 2
 *
 * 16 CQI values (0-15) as per TS 38.214 Table 5.2.2.1-3
 */
const std::vector<double> NrMcsTables::m_spectralEfficiencyForCqi2 = {
    0.0,    // CQI 0: out of range
    0.1523, // CQI 1
    0.3770, // CQI 2
    0.8770, // CQI 3
    1.4766, // CQI 4
    1.9141, // CQI 5
    2.4063, // CQI 6
    2.7305, // CQI 7
    3.3223, // CQI 8
    3.9023, // CQI 9
    4.5234, // CQI 10
    5.1152, // CQI 11
    5.5547, // CQI 12
    6.2266, // CQI 13
    6.9141, // CQI 14
    7.4063  // CQI 15
};

uint8_t
NrMcsTables::GetModulationOrder(uint8_t mcs, uint8_t table)
{
    const auto& mcsMTable = GetMcsMTableRef(table);
    NS_ABORT_MSG_IF(mcs >= mcsMTable.size(),
                    "MCS " << +mcs << " out of range for Table " << +table);
    return mcsMTable[mcs];
}

double
NrMcsTables::GetCodeRate(uint8_t mcs, uint8_t table)
{
    const auto& ecrTable = GetMcsEcrTableRef(table);
    NS_ABORT_MSG_IF(mcs >= ecrTable.size(), "MCS " << +mcs << " out of range for Table " << +table);
    return ecrTable[mcs];
}

double
NrMcsTables::GetSpectralEfficiencyForMcs(uint8_t mcs, uint8_t table)
{
    const auto& seTable = GetSpectralEfficiencyForMcsRef(table);
    NS_ABORT_MSG_IF(mcs >= seTable.size(), "MCS " << +mcs << " out of range for Table " << +table);
    return seTable[mcs];
}

double
NrMcsTables::GetSpectralEfficiencyForCqi(uint8_t cqi, uint8_t table)
{
    const auto& seTable = GetSpectralEfficiencyForCqiRef(table);
    NS_ABORT_MSG_IF(cqi >= seTable.size(), "CQI " << +cqi << " out of range for Table " << +table);
    return seTable[cqi];
}

uint8_t
NrMcsTables::GetMaxMcs(uint8_t table)
{
    const auto& mcsMTable = GetMcsMTableRef(table);
    return static_cast<uint8_t>(mcsMTable.size() - 1);
}

const std::vector<uint8_t>&
NrMcsTables::GetMcsMTableRef(uint8_t table)
{
    NS_ABORT_MSG_IF(table != 1 && table != 2, "Invalid MCS table: " << +table);
    return (table == 1) ? m_mcsMTable1 : m_mcsMTable2;
}

const std::vector<double>&
NrMcsTables::GetMcsEcrTableRef(uint8_t table)
{
    NS_ABORT_MSG_IF(table != 1 && table != 2, "Invalid MCS table: " << +table);
    return (table == 1) ? m_mcsEcrTable1 : m_mcsEcrTable2;
}

const std::vector<double>&
NrMcsTables::GetSpectralEfficiencyForMcsRef(uint8_t table)
{
    NS_ABORT_MSG_IF(table != 1 && table != 2, "Invalid MCS table: " << +table);
    return (table == 1) ? m_spectralEfficiencyForMcs1 : m_spectralEfficiencyForMcs2;
}

const std::vector<double>&
NrMcsTables::GetSpectralEfficiencyForCqiRef(uint8_t table)
{
    NS_ABORT_MSG_IF(table != 1 && table != 2, "Invalid CQI table: " << +table);
    return (table == 1) ? m_spectralEfficiencyForCqi1 : m_spectralEfficiencyForCqi2;
}

} // namespace ns3
