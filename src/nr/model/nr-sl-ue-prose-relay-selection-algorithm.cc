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

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include "nr-sl-ue-prose-relay-selection-algorithm.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeProseRelaySelectionAlgorithm");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeProseRelaySelectionAlgorithm);

TypeId
NrSlUeProseRelaySelectionAlgorithm::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProseRelaySelectionAlgorithm")
    .SetParent<Object> ()
    .SetGroupName ("Nr")
  ;
  return tid;
}

NrSlUeProseRelaySelectionAlgorithm::NrSlUeProseRelaySelectionAlgorithm (void)
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProseRelaySelectionAlgorithm::~NrSlUeProseRelaySelectionAlgorithm (void)
{
  NS_LOG_FUNCTION (this);
}

NS_OBJECT_ENSURE_REGISTERED (NrSlUeProseRelaySelectionAlgorithmFirstAvailable);

TypeId
NrSlUeProseRelaySelectionAlgorithmFirstAvailable::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProseRelaySelectionAlgorithmFirstAvailable")
    .SetParent<NrSlUeProseRelaySelectionAlgorithm> ()
    .SetGroupName ("Nr")
    .AddConstructor<NrSlUeProseRelaySelectionAlgorithmFirstAvailable> ()
  ;
  return tid;
}

NrSlUeProseRelaySelectionAlgorithmFirstAvailable::NrSlUeProseRelaySelectionAlgorithmFirstAvailable (void)
  : NrSlUeProseRelaySelectionAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProseRelaySelectionAlgorithmFirstAvailable::~NrSlUeProseRelaySelectionAlgorithmFirstAvailable (void)
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmFirstAvailable::SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
  NS_LOG_FUNCTION (this << discoveredRelays.size ());

  NS_LOG_DEBUG ("Selection algorithm: first available relay");
  return discoveredRelays.at (0);
}


NS_OBJECT_ENSURE_REGISTERED (NrSlUeProseRelaySelectionAlgorithmRandom);

TypeId
NrSlUeProseRelaySelectionAlgorithmRandom::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProseRelaySelectionAlgorithmRandom")
    .SetParent<NrSlUeProseRelaySelectionAlgorithm> ()
    .SetGroupName ("Nr")
    .AddConstructor<NrSlUeProseRelaySelectionAlgorithmRandom> ()
  ;
  return tid;
}

NrSlUeProseRelaySelectionAlgorithmRandom::NrSlUeProseRelaySelectionAlgorithmRandom (void)
  : NrSlUeProseRelaySelectionAlgorithm (),
    m_rand (CreateObject<UniformRandomVariable> ())
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProseRelaySelectionAlgorithmRandom::~NrSlUeProseRelaySelectionAlgorithmRandom (void)
{
  NS_LOG_FUNCTION (this);
}


int64_t
NrSlUeProseRelaySelectionAlgorithmRandom::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_rand->SetStream (stream);
  return 1;
}


NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmRandom::SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
  NS_LOG_FUNCTION (this << discoveredRelays.size ());

  NS_LOG_DEBUG ("Selection algorithm: random relay");
  uint32_t i = m_rand->GetInteger (0, discoveredRelays.size() - 1);

  return discoveredRelays.at (i);
}

void
NrSlUeProseRelaySelectionAlgorithmRandom::DoDispose (void)
{
  m_rand = nullptr;
}

NS_OBJECT_ENSURE_REGISTERED (NrSlUeProseRelaySelectionAlgorithmMaxRsrp);

TypeId
NrSlUeProseRelaySelectionAlgorithmMaxRsrp::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProseRelaySelectionAlgorithmMaxRsrp")
    .SetParent<NrSlUeProseRelaySelectionAlgorithm> ()
    .SetGroupName ("Nr")
    .AddConstructor<NrSlUeProseRelaySelectionAlgorithmMaxRsrp> ()
  ;
  return tid;
}

NrSlUeProseRelaySelectionAlgorithmMaxRsrp::NrSlUeProseRelaySelectionAlgorithmMaxRsrp (void)
  : NrSlUeProseRelaySelectionAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProseRelaySelectionAlgorithmMaxRsrp::~NrSlUeProseRelaySelectionAlgorithmMaxRsrp (void)
{
  NS_LOG_FUNCTION (this);
}

NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmMaxRsrp::SelectRelay (std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
  NS_LOG_FUNCTION (this << discoveredRelays.size ());

  NS_LOG_DEBUG ("Selection algorithm: relay with best RSRP value");
  
  NrSlUeProse::RelayInfo relay;
  relay.rsrp = (-1) * std::numeric_limits<double>::infinity();
  bool found = false;
  for (uint32_t i = 0; i < discoveredRelays.size (); ++i)
    {
      NrSlUeProse::RelayInfo relayIt = discoveredRelays.at (i);
      if ((relayIt.rsrp > relay.rsrp) && (relayIt.eligible == true))
      {
        NS_LOG_DEBUG ("Found at least one eligible relay!");
        relay.l2Id = relayIt.l2Id;
        relay.relayCode = relayIt.relayCode;
        relay.rsrp = relayIt.rsrp;
        relay.eligible = relayIt.eligible;
        found = true;
      }
    }
  if (!found)
    {
      NS_LOG_DEBUG ("No eligible relay is found!");
      relay.l2Id = 0;
      relay.relayCode = 0;
      double inf = std::numeric_limits<double>::infinity();
      relay.rsrp = (-1) * inf;
      relay.eligible = false;
    }

  return relay;
}

} // namespace ns3
