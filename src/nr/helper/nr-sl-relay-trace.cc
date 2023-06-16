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


#include "nr-sl-relay-trace.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {
 
NS_LOG_COMPONENT_DEFINE ("NrSlRelayTrace");

NS_OBJECT_ENSURE_REGISTERED (NrSlRelayTrace);

NrSlRelayTrace::NrSlRelayTrace ()
{
  NS_LOG_FUNCTION (this);
  m_relayDiscoveryFirstWrite = true;
  m_relaySelectionFirstWrite = true;
  m_relayRsrpFirstWrite = true;
}

NrSlRelayTrace::~NrSlRelayTrace ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NrSlRelayTrace::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrSlRelayTrace")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName ("nr")
    .AddConstructor<NrSlRelayTrace> ()
    .AddAttribute ("NrSlRelayDiscoveryOutputFilename",
                   "Name of the file where the NR SL relay discovery will be saved.",
                   StringValue ("NrSlRelayDiscoveryTrace.txt"),
                   MakeStringAccessor (&NrSlRelayTrace::m_nrSlRelayDiscoveryFilename),
                   MakeStringChecker ())
    .AddAttribute ("NrSlRelaySelectionOutputFilename",
                   "Name of the file where the NR SL relay selection will be saved.",
                   StringValue ("NrSlRelaySelectionTrace.txt"),
                   MakeStringAccessor (&NrSlRelayTrace::m_nrSlRelaySelectionFilename),
                   MakeStringChecker ())
    .AddAttribute ("NrSlRelayRsrpOutputFilename",
                   "Name of the file where the NR SL RSRP measurements between a relay and remote will be saved.",
                   StringValue ("NrSlRelayRsrpTrace.txt"),
                   MakeStringAccessor (&NrSlRelayTrace::m_nrSlRelayRsrpFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
NrSlRelayTrace::RelayDiscoveryTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t relayL2Id, uint32_t relayCode, double rsrp) 
{
  NS_LOG_FUNCTION (relayTrace << path);
  relayTrace->RelayDiscoveryTrace (remoteL2Id, relayL2Id, relayCode, rsrp);
}

void
NrSlRelayTrace::RelayDiscoveryTrace (uint32_t remoteL2Id, uint32_t relayL2Id, uint32_t relayCode, double rsrp)
{
  NS_LOG_INFO ("Writing Relay Discovery Stats in "<< m_nrSlRelayDiscoveryFilename);

  std::ofstream outFile;
  outFile.precision (10);
  outFile << std::fixed;
  if (m_relayDiscoveryFirstWrite == true)
    {
      outFile.open (m_nrSlRelayDiscoveryFilename);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelayDiscoveryFilename);
          return;
        }
      m_relayDiscoveryFirstWrite = false;
      outFile << "Time (s)\tRemoteL2ID\tDiscoveredRelayL2ID\tRelayCode\tRSRP" << std::endl;
    }
  else
    {
      outFile.open (m_nrSlRelayDiscoveryFilename,  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelayDiscoveryFilename);
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds ()/ (double) 1e9 << "\t" << remoteL2Id << "\t" << relayL2Id << "\t" << relayCode << "\t" << rsrp << std::endl;
}

void
NrSlRelayTrace::RelaySelectionTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t currentRelayL2Id, uint32_t selectedRelayL2Id, uint32_t relayCode, double rsrpValue) 
{
  NS_LOG_FUNCTION (relayTrace << path);
  relayTrace->RelaySelectionTrace (remoteL2Id, currentRelayL2Id, selectedRelayL2Id, relayCode, rsrpValue);
}

void
NrSlRelayTrace::RelaySelectionTrace (uint32_t remoteL2Id, uint32_t currentRelayL2Id, uint32_t selectedRelayL2Id, uint32_t relayCode, double rsrpValue)
{
  NS_LOG_INFO ("Writing Relay Selection Stats in "<< m_nrSlRelaySelectionFilename);

  std::ofstream outFile;
  outFile.precision (10);
  outFile << std::fixed;
  if (m_relaySelectionFirstWrite == true)
    {
      outFile.open (m_nrSlRelaySelectionFilename);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelaySelectionFilename);
          return;
        }
      m_relaySelectionFirstWrite = false;
      outFile << "Time (s)\tRemoteL2ID\tCurrentRelayL2ID\tNewRelayL2ID\tNewRelayCode\tNewRSRP" << std::endl;
    }
  else
    {
      outFile.open (m_nrSlRelaySelectionFilename,  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelaySelectionFilename);
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds ()/ (double) 1e9 << "\t" << remoteL2Id << "\t" << currentRelayL2Id << "\t" << selectedRelayL2Id << "\t" << relayCode << "\t" << rsrpValue << std::endl;
}

void
NrSlRelayTrace::RelayRsrpTraceCallback (Ptr<NrSlRelayTrace> relayTrace, std::string path, uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue) 
{
  NS_LOG_FUNCTION (relayTrace << path);
  relayTrace->RelayRsrpTrace (remoteL2Id, relayL2Id, rsrpValue);
}

void
NrSlRelayTrace::RelayRsrpTrace (uint32_t remoteL2Id, uint32_t relayL2Id, double rsrpValue)
{
  NS_LOG_INFO ("Writing Relay Selection Stats in "<< m_nrSlRelayRsrpFilename);

  std::ofstream outFile;
  outFile.precision (10);
  outFile << std::fixed;
  if (m_relayRsrpFirstWrite == true)
    {
      outFile.open (m_nrSlRelayRsrpFilename);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelayRsrpFilename);
          return;
        }
      m_relayRsrpFirstWrite = false;
      outFile << "Time (s)\tRemoteL2ID\tRelayL2ID\tRSRP" << std::endl;
    }
  else
    {
      outFile.open (m_nrSlRelayRsrpFilename,  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << m_nrSlRelayRsrpFilename);
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds ()/ (double) 1e9 << "\t" << remoteL2Id << "\t" << relayL2Id << "\t" << rsrpValue << std::endl;
}


} // namespace ns3
