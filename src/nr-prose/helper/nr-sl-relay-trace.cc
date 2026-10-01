//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-relay-trace.h"

#include "ns3/string.h"
#include <ns3/log.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlRelayTrace");

NS_OBJECT_ENSURE_REGISTERED(NrSlRelayTrace);

NrSlRelayTrace::NrSlRelayTrace()
{
    NS_LOG_FUNCTION(this);
    m_relayDiscoveryFirstWrite = true;
    m_relaySelectionFirstWrite = true;
    m_relayRsrpFirstWrite = true;
}

NrSlRelayTrace::~NrSlRelayTrace()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlRelayTrace::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlRelayTrace")
            .SetParent<NrStatsCalculator>()
            .SetGroupName("nr")
            .AddConstructor<NrSlRelayTrace>()
            .AddAttribute("NrSlRelayDiscoveryOutputFilename",
                          "Name of the file where the NR SL relay discovery will be saved.",
                          StringValue("NrSlRelayDiscoveryTrace.txt"),
                          MakeStringAccessor(&NrSlRelayTrace::m_nrSlRelayDiscoveryFilename),
                          MakeStringChecker())
            .AddAttribute("NrSlRelaySelectionOutputFilename",
                          "Name of the file where the NR SL relay selection will be saved.",
                          StringValue("NrSlRelaySelectionTrace.txt"),
                          MakeStringAccessor(&NrSlRelayTrace::m_nrSlRelaySelectionFilename),
                          MakeStringChecker())
            .AddAttribute("NrSlRelayRsrpOutputFilename",
                          "Name of the file where the NR SL RSRP measurements between a relay and "
                          "remote will be saved.",
                          StringValue("NrSlRelayRsrpTrace.txt"),
                          MakeStringAccessor(&NrSlRelayTrace::m_nrSlRelayRsrpFilename),
                          MakeStringChecker());
    return tid;
}

void
NrSlRelayTrace::RelayDiscoveryTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                            std::string path,
                                            uint32_t remoteL2Id,
                                            uint32_t relayL2Id,
                                            uint32_t relayCode,
                                            double rsrp)
{
    NS_LOG_FUNCTION(relayTrace << path);
    relayTrace->RelayDiscoveryTrace(remoteL2Id, relayL2Id, relayCode, rsrp);
}

void
NrSlRelayTrace::RelayDiscoveryTrace(uint32_t remoteL2Id,
                                    uint32_t relayL2Id,
                                    uint32_t relayCode,
                                    double rsrp)
{
    NS_LOG_INFO("Writing Relay Discovery Stats in " << m_nrSlRelayDiscoveryFilename);

    std::ofstream outFile;
    outFile.precision(10);
    if (m_relayDiscoveryFirstWrite == true)
    {
        outFile.open(m_nrSlRelayDiscoveryFilename);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelayDiscoveryFilename);
            return;
        }
        m_relayDiscoveryFirstWrite = false;
        outFile << "Time (s)\tRemoteL2ID\tDiscoveredRelayL2ID\tRelayCode\tRSRP" << std::endl;
    }
    else
    {
        outFile.open(m_nrSlRelayDiscoveryFilename, std::ios_base::app);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelayDiscoveryFilename);
            return;
        }
    }

    outFile << Simulator::Now().GetNanoSeconds() / (double)1e9 << "\t" << remoteL2Id << "\t"
            << relayL2Id << "\t" << relayCode << "\t" << rsrp << std::endl;
}

void
NrSlRelayTrace::RelaySelectionTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                            std::string path,
                                            uint32_t remoteL2Id,
                                            uint32_t currentRelayL2Id,
                                            uint32_t selectedRelayL2Id,
                                            uint32_t relayCode,
                                            double rsrpValue)
{
    NS_LOG_FUNCTION(relayTrace << path);
    relayTrace->RelaySelectionTrace(remoteL2Id,
                                    currentRelayL2Id,
                                    selectedRelayL2Id,
                                    relayCode,
                                    rsrpValue);
}

void
NrSlRelayTrace::RelaySelectionTrace(uint32_t remoteL2Id,
                                    uint32_t currentRelayL2Id,
                                    uint32_t selectedRelayL2Id,
                                    uint32_t relayCode,
                                    double rsrpValue)
{
    NS_LOG_INFO("Writing Relay Selection Stats in " << m_nrSlRelaySelectionFilename);

    std::ofstream outFile;
    outFile.precision(10);
    if (m_relaySelectionFirstWrite == true)
    {
        outFile.open(m_nrSlRelaySelectionFilename);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelaySelectionFilename);
            return;
        }
        m_relaySelectionFirstWrite = false;
        outFile << "Time (s)\tRemoteL2ID\tCurrentRelayL2ID\tNewRelayL2ID\tNewRelayCode\tNewRSRP"
                << std::endl;
    }
    else
    {
        outFile.open(m_nrSlRelaySelectionFilename, std::ios_base::app);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelaySelectionFilename);
            return;
        }
    }

    outFile << Simulator::Now().GetNanoSeconds() / (double)1e9 << "\t" << remoteL2Id << "\t"
            << currentRelayL2Id << "\t" << selectedRelayL2Id << "\t" << relayCode << "\t"
            << rsrpValue << std::endl;
}

void
NrSlRelayTrace::RelayRsrpTraceCallback(Ptr<NrSlRelayTrace> relayTrace,
                                       std::string path,
                                       uint32_t remoteL2Id,
                                       uint32_t relayL2Id,
                                       double rsrpValue)
{
    NS_LOG_FUNCTION(relayTrace << path);
    relayTrace->RelayRsrpTrace(remoteL2Id, relayL2Id, rsrpValue);
}

void
NrSlRelayTrace::RelayRsrpTrace(uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue)
{
    NS_LOG_INFO("Writing Relay Selection Stats in " << m_nrSlRelayRsrpFilename);

    std::ofstream outFile;
    outFile.precision(10);
    if (m_relayRsrpFirstWrite == true)
    {
        outFile.open(m_nrSlRelayRsrpFilename);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelayRsrpFilename);
            return;
        }
        m_relayRsrpFirstWrite = false;
        outFile << "Time (s)\tRemoteL2ID\tRelayL2ID\tRSRP" << std::endl;
    }
    else
    {
        outFile.open(m_nrSlRelayRsrpFilename, std::ios_base::app);
        if (!outFile.is_open())
        {
            NS_LOG_ERROR("Can't open file " << m_nrSlRelayRsrpFilename);
            return;
        }
    }

    outFile << Simulator::Now().GetNanoSeconds() / (double)1e9 << "\t" << remoteL2Id << "\t"
            << relayL2Id << "\t" << rsrpValue << std::endl;
}

} // namespace ns3
