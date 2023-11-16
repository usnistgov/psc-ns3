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

#ifndef NR_SL_DISCOVERY_TRACE_STATS_H
#define NR_SL_DISCOVERY_TRACE_STATS_H

#include <ns3/lte-stats-calculator.h>

#include <string>
#include <fstream>

#include "ns3/nr-sl-discovery-header.h"

namespace ns3 {

class NrSlDiscoveryTrace  : public LteStatsCalculator 
{
public:
  /**
   * Constructor
   */
  NrSlDiscoveryTrace ();

  /**
   * Destructor
   */
  virtual ~NrSlDiscoveryTrace ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  
  /**
   * Set the name of the file where the NR SL discovery statistics will be stored.
   *
   * \param outputFilename string with the name of the file
   */
  void SetSlDiscoveryOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the NR SL discovery statistics will be stored.
   *
   * \return the outputFilename string with the name of the file
   */
  std::string GetSlDiscoveryOutputFilename ();

  /** 
   * Trace sink for the ns3::NrSlUeProse::DiscoveryTrace trace source
   *
   * \param discoveryTrace
   * \param path trace path
   * \param SenderL2Id sender L2 ID
   * \param ReceiverL2Id receiver L2 ID
   * \param isTx True if the UE is transmitting and False receiving a discovery message
   * \param discMsg the discovery header storing the NR SL discovery header information
   */
  static void DiscoveryTraceCallback (Ptr<NrSlDiscoveryTrace> discoveryTrace, std::string path, uint32_t senderL2Id, uint32_t receiverL2Id, bool isTx, NrSlDiscoveryHeader discMsg);
 
  /**
   * Notifies the stats that a discovery message was sent.
   * \param senderL2Id sender L2 ID
   * \param receiverL2Id receiver L2 ID
   * \param isTx True if the UE is transmitting and False receiving a discovery message
   * \param discMsg the discovery header storing the NR SL discovery header information
   */
  void DiscoveryTrace (uint32_t senderL2Id, uint32_t receiverL2Id, bool isTx, NrSlDiscoveryHeader discMsg);


private:
  /**
   * Name of the file where the discovery results will be saved
   */
  std::string m_nrSlDiscoveryFilename;

  /**
   * When writing Discovery messages first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_discoveryFirstWrite;

};

} // namespace ns3

#endif /* NR_SL_DISCOVERY_TRACE_H */
