/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.es>
 * Modified by: Marco Miozzo <mmiozzo@cttc.es>
 *        Convert MacStatsCalculator in PhyTxStatsCalculator
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef PHY_TX_STATS_CALCULATOR_H_
#define PHY_TX_STATS_CALCULATOR_H_

#include "lte-stats-calculator.h"

#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include <ns3/lte-common.h>

#include <fstream>
#include <string>

namespace ns3
{

/**
 * \ingroup lte
 *
 * Takes care of storing the information generated at PHY layer regarding
 * transmission. Metrics saved are:
 *
 *   - Timestamp (in seconds)
 *   - Frame index
 *   - Subframe index
 *   - C-RNTI
 *   - MCS for transport block 1
 *   - Size of transport block 1
 *   - MCS for transport block 2 (0 if not used)
 *   - Size of transport block 2 (0 if not used)
 */
class PhyTxStatsCalculator : public LteStatsCalculator
{
  public:
    /**
     * Constructor
     */
    PhyTxStatsCalculator();

    /**
     * Destructor
     */
    ~PhyTxStatsCalculator() override;

    // Inherited from ns3::Object
    /**
     * Register this type.
     * \return The object TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Set the name of the file where the UL Tx PHY statistics will be stored.
     *
     * \param outputFilename The string with the name of the file
     */
    void SetUlTxOutputFilename(std::string outputFilename);

    /**
     * Get the name of the file where the UL RX PHY statistics will be stored.
     * \return The name of the file where the UL RX PHY statistics will be stored
     */
    std::string GetUlTxOutputFilename();

    /**
     * Set the name of the file where the DL TX PHY statistics will be stored.
     *
     * \param outputFilename The string with the name of the file
     */
    void SetDlTxOutputFilename(std::string outputFilename);

    /**
     * Get the name of the file where the DL TX PHY statistics will be stored.
     * \return The name of the file where the DL TX PHY statistics will be stored
     */
    std::string GetDlTxOutputFilename();

    /**
     * Notifies the stats calculator that an downlink transmission has occurred.
     * \param params The trace information regarding PHY transmission stats
     */
    void DlPhyTransmission(PhyTransmissionStatParameters params);

    /**
     * Notifies the stats calculator that an uplink transmission has occurred.
     * \param params The trace information regarding PHY transmission stats
     */
    void UlPhyTransmission(PhyTransmissionStatParameters params);

    /**
     * trace sink
     *
     * \param phyTxStats
     * \param path
     * \param params
     */
    static void DlPhyTransmissionCallback(Ptr<PhyTxStatsCalculator> phyTxStats,
                                          std::string path,
                                          PhyTransmissionStatParameters params);

    /**
     * trace sink
     *
     * \param phyTxStats
     * \param path
     * \param params
     */
    static void UlPhyTransmissionCallback(Ptr<PhyTxStatsCalculator> phyTxStats,
                                          std::string path,
                                          PhyTransmissionStatParameters params);

  private:
    /**
     * When writing DL TX PHY statistics first time to file,
     * columns description is added. Then next lines are
     * appended to file. This value is true if output
     * files have not been opened yet
     */
    bool m_dlTxFirstWrite;

    /**
     * When writing UL TX PHY statistics first time to file,
     * columns description is added. Then next lines are
     * appended to file. This value is true if output
     * files have not been opened yet
     */
    bool m_ulTxFirstWrite;

    /**
     * DL TX PHY statistics output trace file
     */
    std::ofstream m_dlTxOutFile;

    /**
     * UL TX PHY statistics output trace file
     */
    std::ofstream m_ulTxOutFile;
};

} // namespace ns3

#endif /* PHY_TX_STATS_CALCULATOR_H_ */
