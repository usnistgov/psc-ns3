// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_EESM_T2_H
#define NR_EESM_T2_H

#include "nr-eesm-error-model.h"

#include <vector>

namespace ns3
{

/**
 * \ingroup error-models
 * \brief The NrEesmT2 struct
 *
 * This class provides the NR Tables to be used for MCS/CQI Table2, corresponding
 * to tables  5.1.3.1-2 and 5.2.2.1-3 in TS 38.214. It includes the beta values
 * (obtained from link-to-system mapping techniques) for EESM under MCS Table2, the
 * Effective Code Rate (ECR) corresponding to MCS Table2, the BLER-SINR tables
 * for different MCS, Block Sizes and LDPC base graph types, the M tables
 * (modulation order for each MCS index in MCS Table2), the Spectral Efficiency
 * (SE) for MCS indexes in MCS Table2, and the SE for CQI indexes in CQI Table2.
 *
 * Values used inside NrEesmIrT2 and NrEesmCcT2 classes
 *
 * \see NrEesmIrT2
 * \see NrEesmCcT2
 */
struct NrEesmT2
{
    /**
     * \brief NrEesmT2 constructor. Initialize the pointers
     */
    NrEesmT2();

    const std::vector<double>* m_betaTable{nullptr};   //!< Beta table
    const std::vector<double>* m_mcsEcrTable{nullptr}; //!< MCS-ECR table
    const NrEesmErrorModel::SimulatedBlerFromSINR* m_simulatedBlerFromSINR{
        nullptr};                                                   //!< BLER from SINR table
    const std::vector<uint8_t>* m_mcsMTable{nullptr};               //!< MCS-M table
    const std::vector<double>* m_spectralEfficiencyForMcs{nullptr}; //!< Spectral-efficiency for MCS
    const std::vector<double>* m_spectralEfficiencyForCqi{nullptr}; //!< Spectral-efficiency for CQI
};

} // namespace ns3

#endif // NR_EESM_T2_H
