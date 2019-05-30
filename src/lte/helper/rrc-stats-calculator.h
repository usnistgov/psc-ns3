/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in 
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef RRC_STATS_CALCULATOR_H_
#define RRC_STATS_CALCULATOR_H_

#include "ns3/lte-common.h"
#include "ns3/lte-stats-calculator.h"
#include "ns3/lte-sl-header.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include <string>
#include <fstream>
#include <bitset>
#include <ns3/ff-mac-common.h>
#include <ns3/config.h>

namespace ns3 {


class RrcStatsCalculator : public LteStatsCalculator
{
public:
  /**
   * Constructor
   */
  RrcStatsCalculator ();

  /**
   * Destructor
   */
  virtual ~RrcStatsCalculator ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /** 
   * Trace sink for the ns3::LteUeRrc::DiscoveryMonitoring trace source
   *
   * \param rrcStats
   * \param path
   * \param imsi
   * \param cellId
   * \param rnti
   * \param discMsg
   */
  static void DiscoveryMonitoringRrcTraceCallback (Ptr<RrcStatsCalculator> rrcStats, std::string path, uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg);

  /**
   * Notifies the stats calculator that a RRC has received a Sidelink discovery message.
   * \param imsi The IMSI of the UE
   * \param cellId The cell id
   * \param rnti The RNTI of the UE
   * \param discMsg The LteSlDiscHeader storing the Sidelink discovery header information
   */
  void RrcDiscoveryMonitoring (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg);

  /**
   * Set the name of the file where the Sidelink RRC discovery
   * monitoring statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetSlDiscRrcOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the Sidelink RRC discovery
   * monitoring statistics will be stored.
   *
   * \return the outputFilename string with the name of the file
   */
  std::string GetSlDiscRrcOutputFilename ();

private:
  /**
   * When writing Discovery Monitoring messages first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_discoveryMonitoringRrcFirstWrite;

};

} // namespace ns3

#endif /* LTE_RRC_STATS_CALCULATOR_H_ */
