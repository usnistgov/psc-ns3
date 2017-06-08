/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * modified by: Marco Miozzo <mmiozzo@cttc.es>
 *        Convert NistMacStatsCalculator in NistPhyRxStatsCalculator
 * Modified by: NIST
 */

#ifndef NIST_PHY_RX_STATS_CALCULATOR_H_
#define NIST_PHY_RX_STATS_CALCULATOR_H_

#include "ns3/nist-lte-stats-calculator.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include <string>
#include <fstream>
#include <ns3/nist-lte-common.h>

namespace ns3 {

/**
 * \ingroup lte
 *
 * Takes care of storing the information generated at PHY layer regarding 
 * reception. Metrics saved are:
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
class NistPhyRxStatsCalculator : public NistLteStatsCalculator
{
public:
  /**
   * Constructor
   */
  NistPhyRxStatsCalculator ();

  /**
   * Destructor
   */
  virtual ~NistPhyRxStatsCalculator ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /**
   * Set the name of the file where the UL Rx PHY statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetUlRxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the UL RX PHY statistics will be stored.
   * @return the name of the file where the UL RX PHY statistics will be stored
   */
  std::string GetUlRxOutputFilename (void);

  /**
   * Set the name of the file where the DL RX PHY statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetDlRxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the DL RX PHY statistics will be stored.
   * @return the name of the file where the DL RX PHY statistics will be stored
   */
  std::string GetDlRxOutputFilename (void);

  /**
   * Set the name of the file where the SL RX PHY statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetSlRxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the SL RX PHY statistics will be stored.
   * @return the name of the file where the SL RX PHY statistics will be stored
   */
  std::string GetSlRxOutputFilename (void);

  /**
   * Set the name of the file where the SL RX PSCCH statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetSlPscchRxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the SL RX PSCCH statistics will be stored.
   * @return the name of the file where the SL RX PHY statistics will be stored
   */
  std::string GetSlPscchRxOutputFilename (void);

  /**
   * Notifies the stats calculator that an downlink reception has occurred.
   * @param params Trace information regarding PHY reception stats
   */
  void DlPhyReception (NistPhyReceptionStatParameters params);

  /**
   * Notifies the stats calculator that an uplink reception has occurred.
   * @param params Trace information regarding PHY reception stats
   */
  void UlPhyReception (NistPhyReceptionStatParameters params);

  /**
   * Notifies the stats calculator that an sidelink reception has occurred.
   * @param params Trace information regarding PHY reception stats
   */
  void SlPhyReception (NistPhyReceptionStatParameters params);
  
  /**
   * Notifies the stats calculator that an sidelink reception has occurred.
   * @param params Trace information regarding PHY reception stats
   */
  void SlPscchReception (NistPhyReceptionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyRxStats 
   * \param path 
   * \param params 
   */
  static void DlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                               std::string path, NistPhyReceptionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyRxStats 
   * \param path 
   * \param params 
   */
  static void UlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                               std::string path, NistPhyReceptionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyRxStats 
   * \param path 
   * \param params 
   */
  static void SlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                               std::string path, NistPhyReceptionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyRxStats 
   * \param path 
   * \param params 
   */
  static void SlPscchReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                               std::string path, NistPhyReceptionStatParameters params);
private:

  /**
   * When writing DL RX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_dlRxFirstWrite;

  /**
   * When writing UL RX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_ulRxFirstWrite;

  /**
   * When writing SL RX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slRxFirstWrite;
  
  /**
   * When writing SL RX PSCCH statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slPscchRxFirstWrite;

};

} // namespace ns3

#endif /* NIST_PHY_RX_STATS_CALCULATOR_H_ */
