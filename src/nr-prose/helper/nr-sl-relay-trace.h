//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_RELAY_TRACE_STATS_H
#define NR_SL_RELAY_TRACE_STATS_H

#include <ns3/nr-stats-calculator.h>

#include <fstream>
#include <string>

namespace ns3
{

class NrSlRelayTrace : public NrStatsCalculator
{
  public:
    /**
     * Constructor
     */
    NrSlRelayTrace();

    /**
     * Destructor
     */
    virtual ~NrSlRelayTrace();

    // Inherited from ns3::Object
    /**
     *  Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId(void);

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
    static void RelayDiscoveryTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                            std::string path,
                                            uint32_t remoteL2Id,
                                            uint32_t relayL2Id,
                                            uint32_t relayCode,
                                            double rsrp);

    /**
     * Notifies the stats that a relay is discovered.
     * \param remoteL2Id remote L2 ID
     * \param relayL2Id current relay L2 ID
     * \param relayCode the relay service code
     * \param rsrp RSRP measurement corresponding to the discovered relay
     */
    void RelayDiscoveryTrace(uint32_t remoteL2Id,
                             uint32_t relayL2Id,
                             uint32_t relayCode,
                             double rsrp);

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
    static void RelaySelectionTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                            std::string path,
                                            uint32_t remoteL2Id,
                                            uint32_t currentRelayL2Id,
                                            uint32_t selectedRelayL2Id,
                                            uint32_t relayCode,
                                            double rsrpValue);

    /**
     * Notifies the stats that a relay is selected.
     * \param remoteL2Id remote L2 ID
     * \param currentRelayL2Id current relay L2 ID
     * \param selectedRelayL2Id selected relay L2 ID
     * \param relayCode the relay service code
     * \param rsrpValue RSRP value
     */
    void RelaySelectionTrace(uint32_t remoteL2Id,
                             uint32_t currentRelayL2Id,
                             uint32_t selectedRelayL2Id,
                             uint32_t relayCode,
                             double rsrpValue);

    /**
     * Trace sink for the ns3::NrSlUeProse::RelayRsrpTrace trace source
     *
     * \param relayTrace a pointer to NrSlRelayTrace class
     * \param path trace path
     * \param remoteL2Id remote L2 ID
     * \param relayL2Id relay L2 ID
     * \param rsrpValue RSRP value
     */
    static void RelayRsrpTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                       std::string path,
                                       uint32_t remoteL2Id,
                                       uint32_t relayL2Id,
                                       double rsrpValue);

    /**
     * Notifies the stats that a relay is selected.
     * \param remoteL2Id remote L2 ID
     * \param relayL2Id relay L2 ID
     * \param rsrpValue RSRP value
     */
    void RelayRsrpTrace(uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue);

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
