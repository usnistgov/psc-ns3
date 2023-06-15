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

#ifndef NR_SL_RELAY_TRACE_STATS_H
#define NR_SL_RELAY_TRACE_STATS_H

#include <ns3/lte-stats-calculator.h>

#include <string>
#include <fstream>

namespace ns3 {

class NrSlRelayTrace  : public LteStatsCalculator  
{

public:
  /**
   * Constructor
   */
  NrSlRelayTrace ();

  /**
   * Destructor
   */
  virtual ~NrSlRelayTrace ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /** 
   * Trace sink for the ns3::NrSlUeProse::RelayDiscoveryTrace trace source
   *
   * \param relayTrace
   * \param path trace path
   * \param remoteL2Id remote L2 ID
   * \param relayL2Id relay L2 ID
   * \param relayCode relay service code
   * \param rsrp RSRP measurement corresponding to the discovered relay
   */
  static void RelayDiscoveryTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t relayL2Id, uint32_t relayCode, double rsrp);
 
  /**
   * Notifies the stats that a relay is discovered.
   * \param remoteL2Id remote L2 ID
   * \param relayL2Id current relay L2 ID
   * \param relayCode the relay service code
   * \param rsrp RSRP measurement corresponding to the discovered relay
   */
  void RelayDiscoveryTrace (uint32_t remoteL2Id, uint32_t relayL2Id, uint32_t relayCode, double rsrp);

  /** 
   * Trace sink for the ns3::NrSlUeProse::RelaySelectionTrace trace source
   *
   * \param relayTrace a pointer to NrSlRelayTrace class
   * \param path trace path
   * \param remoteL2Id remote L2 ID
   * \param currentRelayL2Id current relay L2 ID
   * \param selectedRelayL2id selected relay L2 ID
   * \param relayCode relay service code
   * \param rsrpValue RSRP value
   */
  static void RelaySelectionTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t currentRelayL2Id, uint32_t selectedRelayL2Id, uint32_t relayCode, double rsrpValue);
 
  /**
   * Notifies the stats that a relay is selected.
   * \param remoteL2Id remote L2 ID
   * \param currentRelayL2Id current relay L2 ID
   * \param selectedRelayL2Id selected relay L2 ID
   * \param relayCode the relay service code
   * \param rsrpValue RSRP value
   */
  void RelaySelectionTrace (uint32_t remoteL2Id, uint32_t currentRelayL2Id, uint32_t selectedRelayL2Id, uint32_t relayCode, double rsrpValue);

    /** 
   * Trace sink for the ns3::NrSlUeProse::RelayRsrpTrace trace source
   *
   * \param relayTrace a pointer to NrSlRelayTrace class
   * \param path trace path
   * \param remoteL2Id remote L2 ID
   * \param relayL2Id relay L2 ID
   * \param rsrpValue RSRP value
   */
  static void RelayRsrpTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue);
 
  /**
   * Notifies the stats that a relay is selected.
   * \param remoteL2Id remote L2 ID
   * \param relayL2Id relay L2 ID
   * \param rsrpValue RSRP value
   */
  void RelayRsrpTrace (uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue);



private:
  /**
   * Name of the file where the relay discovery results will be saved
   */
  std::string m_nrSlRelayDiscoveryFilename;

  /**
   * When writing Relay discovery information first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_relayDiscoveryFirstWrite;

  /**
   * Name of the file where the relay selection results will be saved
   */
  std::string m_nrSlRelaySelectionFilename;

  /**
   * When writing Relay selection information first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_relaySelectionFirstWrite;

  /**
   * Name of the file where the relay RSRP results will be saved
   */
  std::string m_nrSlRelayRsrpFilename;

  /**
   * When writing Relay RSRP information first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_relayRsrpFirstWrite;


};

} // namespace ns3

#endif /* NR_SL_RELAY_TRACE_H */
