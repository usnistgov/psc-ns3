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
 * Modified by: Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
 *              Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef MAC_STATS_CALCULATOR_H_
#define MAC_STATS_CALCULATOR_H_

#include "ns3/lte-stats-calculator.h"
#include "ns3/lte-common.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include <string>
#include <fstream>
#include "ns3/lte-enb-mac.h"
#include <bitset>
#include <ns3/ff-mac-common.h>
#include <ns3/config.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/lte-sl-header.h>

namespace ns3 {

/**
 * \ingroup lte
 *
 * Takes care of storing the information generated at MAC layer. Metrics saved are:
 *   - Timestamp (in seconds)
 *   - Frame index
 *   - Subframe index
 *   - C-RNTI
 *   - MCS for transport block 1
 *   - Size of transport block 1
 *   - MCS for transport block 2 (0 if not used)
 *   - Size of transport block 2 (0 if not used)
 */
class MacStatsCalculator : public LteStatsCalculator
{
public:
  /**
   * Constructor
   */
  MacStatsCalculator ();

  /**
   * Destructor
   */
  virtual ~MacStatsCalculator ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /**
   * Set the name of the file where the uplink statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetUlOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the uplink statistics will be stored.
   * @return the name of the file where the uplink statistics will be stored
   */
  std::string GetUlOutputFilename (void);

  /**
   * Set the name of the file where the downlink statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetDlOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the downlink statistics will be stored.
   * @return the name of the file where the downlink statistics will be stored
   */
  std::string GetDlOutputFilename (void);

  //Sidelink

  /**
   * Set the name of the file where the Sidelink PSCCH UE MAC statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetSlUeCchOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the Sidelink PSCCH UE MAC statistics will be stored.
   * @return the name of the file where the Sidelink statistics will be stored
   */
  std::string GetSlUeCchOutputFilename (void);

  /**
   * Set the name of the file where the Sidelink PSSCH UE MAC statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetSlUeSchOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the Sidelink PSSCH UE MAC statistics will be stored.
   * @return the name of the file where the Sidelink statistics will be stored
   */
  std::string GetSlUeSchOutputFilename (void);

  /**
   * Set the name of the file where the Sidelink PSDCH UE MAC statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetSlUeDchOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the Sidelink PSDCH UE MAC statistics will be stored.
   * @return the name of the file where the Sidelink statistics will be stored
   */
  std::string GetSlUeDchOutputFilename (void);


  /**
   * Notifies the stats calculator that an downlink scheduling has occurred.
   * @param cellId Cell ID of the attached Enb
   * @param imsi IMSI of the scheduled UE
   * @param dlSchedulingCallbackInfo the structure that contains downlink scheduling fields:
   * frameNo Frame number
   * subframeNo Subframe number
   * rnti C-RNTI scheduled
   * mcsTb1 MCS for transport block 1
   * sizeTb1 Size of transport block 1
   * mcsTb2 MCS for transport block 2 (0 if not used)
   * sizeTb2 Size of transport block 2 (0 if not used)
   * componentCarrierId component carrier ID
   */

  void DlScheduling (uint16_t cellId, uint64_t imsi, DlSchedulingCallbackInfo dlSchedulingCallbackInfo);

  /**
   * Notifies the stats calculator that an uplink scheduling has occurred.
   * @param cellId Cell ID of the attached Enb
   * @param imsi IMSI of the scheduled UE
   * @param frameNo Frame number
   * @param subframeNo Subframe number
   * @param rnti C-RNTI scheduled
   * @param mcsTb MCS for transport block
   * @param sizeTb Size of transport block
   * @param componentCarrierId component carrier ID
   */
  void UlScheduling (uint16_t cellId, uint64_t imsi,uint32_t frameNo, uint32_t subframeNo,
                     uint16_t rnti, uint8_t mcsTb, uint16_t sizeTb, uint8_t componentCarrierId);


  /**
   * Trace sink for the ns3::LteEnbMac::DlScheduling trace source
   *
   * \param macStats
   * \param path
   * \param dlSchedulingCallbackInfo DlSchedulingCallbackInfo structure containing all downlink information that is generated what DlScheduling traces is fired
   */
  static void DlSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, DlSchedulingCallbackInfo dlSchedulingCallbackInfo);

  /**
   * Trace sink for the ns3::LteEnbMac::UlScheduling trace source
   *
   * \param macStats
   * \param path
   * \param frameNo
   * \param subframeNo
   * \param rnti
   * \param mcs
   * \param size
   * \param componentCarrierId
   */
  static void UlSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path,
                                    uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                                    uint8_t mcs, uint16_t size, uint8_t componentCarrierId);
  //Sidelink

  /**
   * Trace sink for the ns3::LteUeMac::SlPscchScheduling trace source
   * \param macStats
   * \param path
   * \param params The SlUeMacStatParameters
   */
  static void SlUeCchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params);

  /**
   * Trace sink for the ns3::LteUeMac::SlPsschScheduling trace source
   * \param macStats
   * \param path
   * \param params The SlUeMacStatParameters
   */
  static void SlUeSchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params);

  /**
   * Notifies the stats calculator that a Sidelink PSDCH UE MAC transmission has occurred.
   * \param macStats
   * \param path
   * \param params
   * \param discMsg The LteSlDiscHeader
   */
  static void SlUeDchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params, LteSlDiscHeader discMsg);

  /**
   * Notifies the stats calculator that a Sidelink PSCCH UE MAC scheduling has occurred.
   * \param params The SlUeMacStatParameters
   */
  void SlUeCchScheduling (SlUeMacStatParameters params);

  /**
  * Notifies the stats calculator that a Sidelink PSSCH UE MAC scheduling has occurred.
  * \param params The SlUeMacStatParameters
  */
  void SlUeSchScheduling (SlUeMacStatParameters params);

  /**
   * Notifies the stats calculator that a Sidelink PSDCH UE MAC scheduling has occurred.
   * \param params The SlUeMacStatParameters
   * \param discMsg The LteSlDiscHeader
   */
  void SlUeDchScheduling (SlUeMacStatParameters params, LteSlDiscHeader discMsg);

private:
  /**
   * When writing DL MAC statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_dlFirstWrite;

  /**
   * When writing UL MAC statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_ulFirstWrite;

  /**
   * When writing Sidelink PSCCH UE MAC statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slUeCchFirstWrite;

  /**
   * When writing Sidelink PSSCH UE MAC statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slUeSchFirstWrite;

  /**
   * When writing Discovery Announcement messages first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slUeDchFirstWrite;

  /**
   * Downlink output trace file
   */
  std::ofstream m_dlOutFile;

  /**
   * Uplink output trace file
   */
  std::ofstream m_ulOutFile;
};

} // namespace ns3

#endif /* MAC_STATS_CALCULATOR_H_ */
