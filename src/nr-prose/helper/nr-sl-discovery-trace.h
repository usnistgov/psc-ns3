
#ifndef NR_SL_DISCOVERY_TRACE_STATS_H
#define NR_SL_DISCOVERY_TRACE_STATS_H

#include "ns3/nr-sl-discovery-header.h"
#include <ns3/nr-stats-calculator.h>

#include <fstream>
#include <string>

namespace ns3
{

class NrSlDiscoveryTrace : public NrStatsCalculator
{
  public:
    /**
     * Constructor
     */
    NrSlDiscoveryTrace();

    /**
     * Destructor
     */
    virtual ~NrSlDiscoveryTrace();

    // Inherited from ns3::Object
    /**
     *  Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId(void);

    /**
     * Set the name of the file where the NR SL discovery statistics will be stored.
     *
     * \param outputFilename string with the name of the file
     */
    void SetSlDiscoveryOutputFilename(std::string outputFilename);

    /**
     * Get the name of the file where the NR SL discovery statistics will be stored.
     *
     * \return the outputFilename string with the name of the file
     */
    std::string GetSlDiscoveryOutputFilename();

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
    static void DiscoveryTraceCallback(Ptr<NrSlDiscoveryTrace> discoveryTrace,
                                       std::string path,
                                       uint32_t senderL2Id,
                                       uint32_t receiverL2Id,
                                       bool isTx,
                                       NrSlDiscoveryHeader discMsg);

    /**
     * Notifies the stats that a discovery message was sent.
     * \param senderL2Id sender L2 ID
     * \param receiverL2Id receiver L2 ID
     * \param isTx True if the UE is transmitting and False receiving a discovery message
     * \param discMsg the discovery header storing the NR SL discovery header information
     */
    void DiscoveryTrace(uint32_t senderL2Id,
                        uint32_t receiverL2Id,
                        bool isTx,
                        NrSlDiscoveryHeader discMsg);

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
