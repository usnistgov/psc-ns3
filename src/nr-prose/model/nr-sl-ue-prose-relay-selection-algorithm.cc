//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-ue-prose-relay-selection-algorithm.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeProseRelaySelectionAlgorithm");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeProseRelaySelectionAlgorithm);

TypeId
NrSlUeProseRelaySelectionAlgorithm::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlUeProseRelaySelectionAlgorithm").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

NrSlUeProseRelaySelectionAlgorithm::NrSlUeProseRelaySelectionAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeProseRelaySelectionAlgorithm::~NrSlUeProseRelaySelectionAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NS_OBJECT_ENSURE_REGISTERED(NrSlUeProseRelaySelectionAlgorithmFirstAvailable);

TypeId
NrSlUeProseRelaySelectionAlgorithmFirstAvailable::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlUeProseRelaySelectionAlgorithmFirstAvailable")
                            .SetParent<NrSlUeProseRelaySelectionAlgorithm>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlUeProseRelaySelectionAlgorithmFirstAvailable>();
    return tid;
}

NrSlUeProseRelaySelectionAlgorithmFirstAvailable::NrSlUeProseRelaySelectionAlgorithmFirstAvailable()
    : NrSlUeProseRelaySelectionAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeProseRelaySelectionAlgorithmFirstAvailable::
    ~NrSlUeProseRelaySelectionAlgorithmFirstAvailable()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmFirstAvailable::SelectRelay(
    std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
    NS_LOG_FUNCTION(this << discoveredRelays.size());

    if (!discoveredRelays.empty())
    {
        NS_LOG_INFO(
            "Selection algorithm: first available relay L2Id: " << discoveredRelays.at(0).l2Id);
        return discoveredRelays.at(0);
    }
    else
    {
        NS_LOG_INFO("Selection algorithm: no available relays");
        return NrSlUeProse::RelayInfo();
    }
}

NS_OBJECT_ENSURE_REGISTERED(NrSlUeProseRelaySelectionAlgorithmRandom);

TypeId
NrSlUeProseRelaySelectionAlgorithmRandom::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlUeProseRelaySelectionAlgorithmRandom")
                            .SetParent<NrSlUeProseRelaySelectionAlgorithm>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlUeProseRelaySelectionAlgorithmRandom>();
    return tid;
}

NrSlUeProseRelaySelectionAlgorithmRandom::NrSlUeProseRelaySelectionAlgorithmRandom()
    : NrSlUeProseRelaySelectionAlgorithm()
{
    NS_LOG_FUNCTION(this);
    m_rand = CreateObject<UniformRandomVariable>();
}

NrSlUeProseRelaySelectionAlgorithmRandom::~NrSlUeProseRelaySelectionAlgorithmRandom()
{
    NS_LOG_FUNCTION(this);
}

int64_t
NrSlUeProseRelaySelectionAlgorithmRandom::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_rand->SetStream(stream);
    return 1;
}

NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmRandom::SelectRelay(
    std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
    NS_LOG_FUNCTION(this << discoveredRelays.size());

    if (!discoveredRelays.empty())
    {
        uint32_t i = m_rand->GetInteger(0, discoveredRelays.size() - 1);
        NS_LOG_INFO("Selection algorithm: random relay L2Id: " << discoveredRelays.at(i).l2Id);
        return discoveredRelays.at(i);
    }
    else
    {
        NS_LOG_INFO("Selection algorithm: no available relays");
        return NrSlUeProse::RelayInfo();
    }
}

void
NrSlUeProseRelaySelectionAlgorithmRandom::DoDispose()
{
}

NS_OBJECT_ENSURE_REGISTERED(NrSlUeProseRelaySelectionAlgorithmMaxRsrp);

TypeId
NrSlUeProseRelaySelectionAlgorithmMaxRsrp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlUeProseRelaySelectionAlgorithmMaxRsrp")
                            .SetParent<NrSlUeProseRelaySelectionAlgorithm>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlUeProseRelaySelectionAlgorithmMaxRsrp>();
    return tid;
}

NrSlUeProseRelaySelectionAlgorithmMaxRsrp::NrSlUeProseRelaySelectionAlgorithmMaxRsrp()
    : NrSlUeProseRelaySelectionAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeProseRelaySelectionAlgorithmMaxRsrp::~NrSlUeProseRelaySelectionAlgorithmMaxRsrp()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeProse::RelayInfo
NrSlUeProseRelaySelectionAlgorithmMaxRsrp::SelectRelay(
    std::vector<NrSlUeProse::RelayInfo> discoveredRelays)
{
    NS_LOG_FUNCTION(this << discoveredRelays.size());

    NrSlUeProse::RelayInfo relay;
    bool found = false;
    for (uint32_t i = 0; i < discoveredRelays.size(); ++i)
    {
        NrSlUeProse::RelayInfo relayIt = discoveredRelays.at(i);
        if ((relayIt.rsrp > relay.rsrp) && (relayIt.eligible == true))
        {
            relay.l2Id = relayIt.l2Id;
            relay.relayCode = relayIt.relayCode;
            relay.rsrp = relayIt.rsrp;
            relay.eligible = relayIt.eligible;
            found = true;
            NS_LOG_DEBUG("Selection algorithm: found candidate L2Id " << relay.l2Id << " with RSRP "
                                                                      << relay.rsrp);
        }
    }
    if (!found)
    {
        NS_LOG_INFO("Selection algorithm: no eligible relay was found");
    }
    else
    {
        NS_LOG_INFO("Selection algorithm: selected candidate L2Id " << relay.l2Id << " with RSRP "
                                                                    << relay.rsrp);
    }
    return relay;
}

} // namespace ns3
