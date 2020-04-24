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

#include "rrc-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RrcStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (RrcStatsCalculator);

RrcStatsCalculator::RrcStatsCalculator ()
  : m_discoveryMonitoringRrcFirstWrite (true)
{
  NS_LOG_FUNCTION (this);
}

RrcStatsCalculator::~RrcStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
RrcStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RrcStatsCalculator")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName("LTE")
    .AddConstructor<RrcStatsCalculator> ()
    .AddAttribute ("SlDchRxRrcOutputFilename",
                   "Name of the file where the Sidelink RRC discovery "
                   "monitoring statistics will be saved.",
                   StringValue ("SlDchRxRrcStats.txt"),
                   MakeStringAccessor (&RrcStatsCalculator::SetSlDiscRrcOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
RrcStatsCalculator::SetSlDiscRrcOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlOutputFilename (outputFilename);
}

std::string
RrcStatsCalculator::GetSlDiscRrcOutputFilename (void)
{
  return LteStatsCalculator::GetSlOutputFilename ();
}

void
RrcStatsCalculator::RrcDiscoveryMonitoring (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg)
{
  NS_LOG_INFO ("Writing Discovery Monitoring Stats in "<< GetSlDiscRrcOutputFilename ().c_str ());

  std::ofstream outFile;
  if (m_discoveryMonitoringRrcFirstWrite == true )
    {
      outFile.open (GetSlDiscRrcOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlDiscRrcOutputFilename ().c_str ());
          return;
        }
      m_discoveryMonitoringRrcFirstWrite = false;
      outFile << "Time\tIMSI\tCellId\tRNTI\tDiscType\tContentType\tDiscModel\tContent" << std::endl;
    }
  else
    {
      outFile.open (GetSlDiscRrcOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlDiscRrcOutputFilename ().c_str ());
          return;
        }
    }


  outFile << Simulator::Now ().GetMilliSeconds () << "\t" << imsi << "\t" << cellId << "\t" << rnti << "\t";

  uint8_t msgType = discMsg.GetDiscoveryMsgType ();

  outFile << (uint16_t) discMsg.GetDiscoveryType ()  << "\t" << (uint16_t) discMsg.GetDiscoveryContentType ()
                                                             << "\t" << (uint16_t) discMsg.GetDiscoveryModel ()<< "\t";

  switch (msgType)
    {
    case LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT://UE-to-Network Relay Discovery Announcement in model A
    case LteSlDiscHeader::DISC_RELAY_RESPONSE://UE-to-Network Relay Discovery Response in model B
      {
        //write fields, include spare (0) as the last field
        outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << discMsg.GetRelayUeId () << ";" << (uint16_t) discMsg.GetStatusIndicator () << ";0" << std::endl;
      }
      break;
    case LteSlDiscHeader::DISC_RELAY_SOLICITATION:
      {
        //write fields, include spare (0) as the last field
        outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << (uint16_t) discMsg.GetURDSComposition () << ";" << discMsg.GetRelayUeId () << ";0" << std::endl;
      }
      break;
    case LteSlDiscHeader::DISC_OPEN_ANNOUNCEMENT:
    case LteSlDiscHeader::DISC_RESTRICTED_QUERY:
    case LteSlDiscHeader::DISC_RESTRICTED_RESPONSE:
      { //open or restricted announcement
        outFile << discMsg.GetApplicationCode () << std::endl;
      }
      break;
    default:
      NS_FATAL_ERROR ("Invalid discovery message type " << msgType);
    }
}

void
RrcStatsCalculator::DiscoveryMonitoringRrcTraceCallback (Ptr<RrcStatsCalculator> rrcStats, std::string path, uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg)
{
  NS_LOG_FUNCTION (rrcStats << path);

  rrcStats->RrcDiscoveryMonitoring (imsi, cellId, rnti, discMsg);
}


} // namespace ns3
