//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H
#define NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H

#include "nr-sl-ue-prose.h"

#include <ns3/object.h>
#include <ns3/random-variable-stream.h>

#include <vector>

namespace ns3
{

/**
 * \ingroup nr-prose
 *
 * \brief Base class for NR ProSe Relay Selection Algorithms
 *
 * This class defines the API necessary to perform relay selection for NR SL
 * UEs. To add a difference selection algorithm (or flavor), simply override
 * this class, implement the SelectRelay function, and provide an instance
 * of your class to the entity housing the selection algorithm to be used.
 */
class NrSlUeProseRelaySelectionAlgorithm : public Object
{
  public:
    NrSlUeProseRelaySelectionAlgorithm();
    ~NrSlUeProseRelaySelectionAlgorithm() override;
    static TypeId GetTypeId();

    /**
     * \brief Selects a relay from the available list.
     *
     * \param discoveredRelays List of discovered relays
     *
     * \returns The newly selected relay
     */
    virtual NrSlUeProse::RelayInfo SelectRelay(
        std::vector<NrSlUeProse::RelayInfo> discoveredRelays) = 0;

}; // end of NrSlUeProseRelaySelectionAlgorithm

/**
 * \ingroup nr-prose
 *
 * \brief Implements the first available relay selection algorithm
 */
class NrSlUeProseRelaySelectionAlgorithmFirstAvailable : public NrSlUeProseRelaySelectionAlgorithm
{
  public:
    NrSlUeProseRelaySelectionAlgorithmFirstAvailable();
    ~NrSlUeProseRelaySelectionAlgorithmFirstAvailable() override;
    static TypeId GetTypeId();

    NrSlUeProse::RelayInfo SelectRelay(
        std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;

}; // end of NrSlUeProseRelaySelectionAlgorithmFirstAvailable

/**
 * \ingroup nr-prose
 *
 * \brief Implements the random relay selection algorithm
 */
class NrSlUeProseRelaySelectionAlgorithmRandom : public NrSlUeProseRelaySelectionAlgorithm
{
  public:
    NrSlUeProseRelaySelectionAlgorithmRandom();
    ~NrSlUeProseRelaySelectionAlgorithmRandom() override;
    static TypeId GetTypeId();
    virtual int64_t AssignStreams(int64_t stream);

    NrSlUeProse::RelayInfo SelectRelay(
        std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;

  protected:
    void DoDispose() override;

  private:
    Ptr<UniformRandomVariable> m_rand; //!< The uniform random variable

}; // end of NrSlUeProseRelaySelectionAlgorithmRandom

/**
 * \ingroup nr-prose
 *
 * \brief Implements the max RSRP relay selection algorithm
 *
 * The RelayInfo with the maximum RSRP value, considering only those that
 * are set to 'eligible', will be returned by SelectRelay().  If no eligible
 * relays are found, SelectRelay() will return an uninitialized RelayInfo.
 */
class NrSlUeProseRelaySelectionAlgorithmMaxRsrp : public NrSlUeProseRelaySelectionAlgorithm
{
  public:
    NrSlUeProseRelaySelectionAlgorithmMaxRsrp();
    ~NrSlUeProseRelaySelectionAlgorithmMaxRsrp() override;
    static TypeId GetTypeId();

    NrSlUeProse::RelayInfo SelectRelay(
        std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;

}; // end of NrSlUeProseRelaySelectionAlgorithmMaxRsrp

} // namespace ns3

#endif // NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H
