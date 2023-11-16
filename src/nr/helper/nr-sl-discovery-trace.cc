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


#include "nr-sl-discovery-trace.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {
 
NS_LOG_COMPONENT_DEFINE ("NrSlDiscoveryTrace");

NS_OBJECT_ENSURE_REGISTERED (NrSlDiscoveryTrace);

NrSlDiscoveryTrace::NrSlDiscoveryTrace ()
  : 
    m_discoveryFirstWrite (true)
{
  NS_LOG_FUNCTION (this);
}

NrSlDiscoveryTrace::~NrSlDiscoveryTrace ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NrSlDiscoveryTrace::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrSlDiscoveryTrace")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName ("nr")
    .AddConstructor<NrSlDiscoveryTrace> ()
    .AddAttribute ("NrSlDiscoveryOutputFilename",
                   "Name of the file where the NR SL discovery "
                   "transmission/reception statistics will be saved.",
                   StringValue ("NrSlDiscoveryTrace.txt"),
                   MakeStringAccessor (&NrSlDiscoveryTrace::SetSlDiscoveryOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
NrSlDiscoveryTrace::SetSlDiscoveryOutputFilename (std::string outputFilename)
{
   m_nrSlDiscoveryFilename = outputFilename;
}

std::string
NrSlDiscoveryTrace::GetSlDiscoveryOutputFilename ()
{
  return m_nrSlDiscoveryFilename;
}

void
NrSlDiscoveryTrace::DiscoveryTraceCallback (Ptr<NrSlDiscoveryTrace> discoveryTrace, std::string path, uint32_t senderL2Id, uint32_t receiverL2Id, bool isTx, NrSlDiscoveryHeader discMsg)
{
  NS_LOG_FUNCTION (discoveryTrace << path);
  discoveryTrace->DiscoveryTrace (senderL2Id, receiverL2Id, isTx, discMsg);
}

void
NrSlDiscoveryTrace::DiscoveryTrace (uint32_t senderL2Id, uint32_t receiverL2Id, bool isTx, NrSlDiscoveryHeader discMsg)
{
  NS_LOG_INFO ("Writing Discovery Transmission/Reception Stats in "<< GetSlDiscoveryOutputFilename ().c_str ());

  std::ofstream outFile;
  if (m_discoveryFirstWrite == true)
    {
      outFile.open (GetSlDiscoveryOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlDiscoveryOutputFilename ().c_str ());
          return;
        }
      m_discoveryFirstWrite = false;
      outFile << "Time (ns)\tTX/RX\tsenderL2Id\treceiverL2Id\tDiscType\tDiscModel\tContentType\tContent" << std::endl;
    }
  else
    {
      outFile.open (GetSlDiscoveryOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlDiscoveryOutputFilename ().c_str ());
          return;
        }
    }


  outFile << Simulator::Now ().GetNanoSeconds () << "\t";
  if (isTx)
  {
    outFile << "TX" << "\t";
  }
  else
  {
    outFile << "RX" << "\t";
  }
  outFile << senderL2Id << "\t" << receiverL2Id << "\t";
  
  if (discMsg.GetDiscoveryType () == 1)
  {
    outFile << "Open" << "\t";
  }
  else if (discMsg.GetDiscoveryType () == 2)
  {
    outFile << "Restricted" << "\t";
  }
  else
  {
    NS_FATAL_ERROR ("Invalid discovery type " << discMsg.GetDiscoveryType ());
  }

  if (discMsg.GetDiscoveryModel () == 1)
  {
    outFile << "ModelA" << "\t";
  }
  else if (discMsg.GetDiscoveryModel () == 2)
  {
    outFile << "ModelB" << "\t";
  }
  else
  {
    NS_FATAL_ERROR ("Invalid discovery model " << discMsg.GetDiscoveryModel ());
  }

  if (discMsg.GetDiscoveryContentType () == 0 and discMsg.GetDiscoveryModel () == 1)
  {
    outFile << "Announcement" << "\t";
  }
  else if (discMsg.GetDiscoveryContentType () == 0 and discMsg.GetDiscoveryModel () == 2)
  {
    outFile << "Response" << "\t";
  }
  else if (discMsg.GetDiscoveryContentType () == 1)
  {
    outFile << "Request" << "\t";
  }
  else if (discMsg.GetDiscoveryContentType () == 4 and discMsg.GetDiscoveryModel () == 1)
  {
    outFile << "RelayAnnouncement" << "\t";
  }
  else if (discMsg.GetDiscoveryContentType () == 4 and discMsg.GetDiscoveryModel () == 2)
  {
    outFile << "RelayResponse" << "\t";
  }
  else if (discMsg.GetDiscoveryContentType () == 5)
  {
    outFile << "RelaySolicitation" << "\t";
  }
  else
  {
    NS_FATAL_ERROR ("Invalid discovery content type " << discMsg.GetDiscoveryContentType ());
  }
  
  switch (discMsg.GetDiscoveryMsgType ())
    {
    case NrSlDiscoveryHeader::DISC_RELAY_ANNOUNCEMENT://UE-to-Network Relay Discovery Announcement in model A
    case NrSlDiscoveryHeader::DISC_RELAY_RESPONSE://UE-to-Network Relay Discovery Response in model B
      {
        //write fields, include spare (0) as the last field
        outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << discMsg.GetRelayUeId () << ";" << (uint16_t) discMsg.GetStatusIndicator () << ";0" << std::endl;
      }
      break;
    case NrSlDiscoveryHeader::DISC_RELAY_SOLICITATION:
      {
        //write fields, include spare (0) as the last field
        outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << (uint16_t) discMsg.GetURDSComposition () << ";" << discMsg.GetRelayUeId () << ";0" << std::endl;
      }
      break;
    case NrSlDiscoveryHeader::DISC_OPEN_ANNOUNCEMENT:
    case NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY:
    case NrSlDiscoveryHeader::DISC_RESTRICTED_RESPONSE:
      { //open or restricted announcement
        outFile << discMsg.GetApplicationCode () << std::endl;
      }
      break;
    default:
      NS_FATAL_ERROR ("Invalid discovery message type " << discMsg.GetDiscoveryMsgType ());
    }
}


} // namespace ns3
