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

#ifndef NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H
#define NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H

#include <ns3/object.h>
#include <ns3/timer.h>
#include <ns3/tag.h>
#include <vector>
#include <ns3/random-variable-stream.h>

#include <ns3/nr-sl-ue-prose.h>

namespace ns3 {

/**
 * \ingroup nr
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
  NrSlUeProseRelaySelectionAlgorithm (void);
  virtual ~NrSlUeProseRelaySelectionAlgorithm (void);
  static TypeId GetTypeId (void);

  /**
   * \brief Selects a relay from the available list.
   *
   * \param discoveredRelays List of discovered relays 
   * 
   * \returns The newly selected relay
   */
  virtual NrSlUeProse::RelayInfo SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays) = 0;

};//end of NrSlUeProseRelaySelectionAlgorithm

/**
 * \ingroup nr
 *
 * \brief Implements the first available relay selection algorithm
 */
class NrSlUeProseRelaySelectionAlgorithmFirstAvailable : public NrSlUeProseRelaySelectionAlgorithm
{

public:
  NrSlUeProseRelaySelectionAlgorithmFirstAvailable (void);
  virtual ~NrSlUeProseRelaySelectionAlgorithmFirstAvailable (void);
  static TypeId GetTypeId (void);

  /**
   * \brief Select first available relay.
   *
   * \param discoveredRelays List of discovered relays 
   *
   * \returns The newly selected relay.
   */
  virtual NrSlUeProse::RelayInfo SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;

};//end of NrSlUeProseRelaySelectionAlgorithmFirstAvailable


/**
 * \ingroup nr
 *
 * \brief Implements the random relay selection algorithm
 */
class NrSlUeProseRelaySelectionAlgorithmRandom : public NrSlUeProseRelaySelectionAlgorithm
{

public:
  NrSlUeProseRelaySelectionAlgorithmRandom (void);
  virtual ~NrSlUeProseRelaySelectionAlgorithmRandom (void);
  static TypeId GetTypeId (void);
  virtual int64_t AssignStreams (int64_t stream);

  /**
   * \brief Select random relay.
   *
   * \param discoveredRelays List of discovered relays 
   *
   * \returns The newly selected relay.
   */
  virtual NrSlUeProse::RelayInfo SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;
protected:
  virtual void DoDispose (void) override;
private:
  Ptr<UniformRandomVariable> m_rand; //!< The uniform random variable

};//end of NrSlUeProseRelaySelectionAlgorithmRandom

/**
 * \ingroup nr
 *
 * \brief Implements the max RSRP relay selection algorithm
 */
class NrSlUeProseRelaySelectionAlgorithmMaxRsrp : public NrSlUeProseRelaySelectionAlgorithm
{

public:
  NrSlUeProseRelaySelectionAlgorithmMaxRsrp (void);
  virtual ~NrSlUeProseRelaySelectionAlgorithmMaxRsrp (void);
  static TypeId GetTypeId (void);

  /**
   * \brief Select the relay with the max RSRP.
   *
   * \param discoveredRelays List of discovered relays 
   *
   * \returns The newly selected relay.
   */
  virtual NrSlUeProse::RelayInfo SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays) override;

};//end of NrSlUeProseRelaySelectionAlgorithmMaxRsrp

} // namespace ns3

#endif // NR_SL_UE_PROSE_RELAY_SELECTION_ALGORITHM_H
