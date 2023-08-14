/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/socket.h>
#include <ns3/packet.h>
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/boolean.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/seq-ts-size-header.h>
#include "psc-video-streaming.h"
#include "psc-video-streaming-distributions.h"

#include <fstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscVideoStreaming");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscVideoStreaming);

TypeId
PscVideoStreaming::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscVideoStreaming")
    .SetParent <Application> ()
    .AddConstructor <PscVideoStreaming> ()
    .AddAttribute ("Distribution",
                   "Video Streaming Model data distribution",
                   StringValue ("1080p-bright"),
                   MakeStringAccessor (&PscVideoStreaming::SetDistributionName,
                                       &PscVideoStreaming::GetDistributionName),
                   MakeStringChecker ())
    .AddAttribute ("ReceiverAddress",
                   "The receiver Address",
                   AddressValue (),
                   MakeAddressAccessor (&PscVideoStreaming::m_receiverAddress),
                   MakeAddressChecker ())
    .AddAttribute ("ReceiverPort",
                   "The receiver port",
                   UintegerValue (5554),
                   MakeUintegerAccessor (&PscVideoStreaming::m_receiverPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("BoostLengthPacketCount",
                   "Length of boost time, in packets",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PscVideoStreaming::m_boostPacketCount),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("BoostPercentile",
                   "CDF percentile from which size and interval values are generated during boost time",
                   DoubleValue (0),
                   MakeDoubleAccessor (&PscVideoStreaming::m_boostPercentile),
                   MakeDoubleChecker<double> (0, 100))
    .AddAttribute ("MaxUdpPayloadSize",
                   "Maximum size of the generated UDP payloads",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&PscVideoStreaming::m_maxUdpPayloadSize),
                   MakeUintegerChecker<uint16_t> (12, 65500))
    .AddTraceSource ("Tx",
                     "Trace with packets transmitted",
                     MakeTraceSourceAccessor (&PscVideoStreaming::m_txTrace),
                     "ns3::psc::PscVideoStreaming::TxTracedCallback")
  ;

  return tid;
}

PscVideoStreaming::PscVideoStreaming ()
{
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
}

PscVideoStreaming::~PscVideoStreaming ()
{
  NS_LOG_FUNCTION (this);
}

void
PscVideoStreaming::SetReceiver (Address recvAddress, uint16_t recvPort)
{
  NS_LOG_FUNCTION (this << recvAddress << recvPort);
  m_receiverAddress = recvAddress;
  m_receiverPort = recvPort;
}

void
PscVideoStreaming::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
  Application::DoDispose ();
}

void
PscVideoStreaming::ReadCustomDistribution (std::string sizeCdfFilePath, std::string intervalCdfFilePath)
{
  NS_LOG_FUNCTION (this << sizeCdfFilePath << intervalCdfFilePath);

  // Clear the distribution maps
  m_sizeDistribution.clear ();
  m_intervalDistribution.clear ();


  double increment;
  double currProb;
  uint32_t readSize;
  double readInterval;

  // Read the file with the distribution for the size
  std::ifstream finSize (sizeCdfFilePath.c_str ());
  NS_ABORT_MSG_IF (!finSize.is_open (), "Cannot open file " + sizeCdfFilePath);

  finSize >> increment;
  currProb = 0;
  while (finSize.good () && finSize >> readSize)
    {
      if (currProb > 1)
        {
          currProb = 1;
        }
      m_sizeDistribution [currProb] = readSize;
      currProb += increment;
    }
  m_sizeDistribution [1] = readSize;

  // Read the file with the distribution for the interval
  std::ifstream finInterval (intervalCdfFilePath.c_str ());
  NS_ABORT_MSG_IF (!finInterval.is_open (), "Cannot open file " + intervalCdfFilePath);

  finInterval >> increment;
  currProb = 0;
  while (finInterval.good () && finInterval >> readInterval)
    {
      if (currProb > 1)
        {
          currProb = 1;
        }
      m_intervalDistribution [currProb] = readInterval;
      currProb += increment;
    }
  m_intervalDistribution [1] = readInterval;
}

void
PscVideoStreaming::LoadCdfs (void)
{
  NS_LOG_FUNCTION (this);

  double boostPctSize = (1 - m_boostPercentile / 100);

  m_sizeErv = CreateObjectWithAttributes<EmpiricalRandomVariable> ("Interpolate", BooleanValue (true));
  m_intervalErv = CreateObjectWithAttributes<EmpiricalRandomVariable> ("Interpolate", BooleanValue (true));
  m_sizeErvBoost = CreateObjectWithAttributes<EmpiricalRandomVariable> ("Interpolate", BooleanValue (true));
  m_intervalErvBoost = CreateObjectWithAttributes<EmpiricalRandomVariable> ("Interpolate", BooleanValue (true));


  // Size
  double oldProb = 0;
  uint32_t oldSize = 0;
  bool boostCdfStarted = false;

  for (auto it = m_sizeDistribution.begin (); it != m_sizeDistribution.end (); it++)
    {
      m_sizeErv->CDF (it->second, it->first);

      // Check if we are in the probability range of the boost
      if (it->first >= (m_boostPercentile / 100))
        {
          if (!boostCdfStarted)
            {
              uint32_t interpolatedVal = static_cast<uint32_t> (oldSize + (((m_boostPercentile / 100) - oldProb) / (it->first - oldProb)) * (it->second - oldSize));
              m_sizeErvBoost->CDF (interpolatedVal, 0);
              boostCdfStarted = true;
            }
          m_sizeErvBoost->CDF (it->second, (it->first - (m_boostPercentile / 100)) / boostPctSize);
        }
      oldProb = it->first;
      oldSize = it->second;
    }
  m_sizeErv->CDF (oldSize, 1);
  m_sizeErvBoost->CDF (oldSize, 1);


  // Interval
  double oldInterval = 0;
  bool boostCdfFinished = false;

  for (auto it = m_intervalDistribution.begin (); it != m_intervalDistribution.end (); it++)
    {
      m_intervalErv->CDF (it->second, it->first);

      // Check if we are in the probability range of the boost
      if (it->first < boostPctSize)
        {
          m_intervalErvBoost->CDF (it->second, it->first / boostPctSize);
        }
      else
        {
          if (!boostCdfFinished)
            {
              m_intervalErvBoost->CDF (it->second, 1);
              boostCdfFinished = true;
            }
        }
      oldInterval = it->second;
    }

  m_intervalErv->CDF (oldInterval, 1);
}

void
PscVideoStreaming::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  LoadCdfs ();

  m_sequenceNumber = 0;
  m_boostPacketsLeft = m_boostPacketCount;

  if (m_socket == nullptr)
    {
      m_socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
      if (Ipv4Address::IsMatchingType (m_receiverAddress))
        {
          m_socket->Bind ();
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom (m_receiverAddress), m_receiverPort));
        }
      else if (Ipv6Address::IsMatchingType (m_receiverAddress))
        {
          m_socket->Bind6 ();
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom (m_receiverAddress), m_receiverPort));
        }
      else
        {
          NS_ABORT_MSG ("Invalid receiver address type");
        }
    }

  m_socket->SetRecvCallback (MakeNullCallback <void, Ptr <Socket> > ());

  Send ();
}

void
PscVideoStreaming::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  m_socket->Close ();
  m_socket = nullptr;
}

void
PscVideoStreaming::Send (void)
{
  NS_LOG_FUNCTION (this);

  if (Simulator::Now () < m_stopTime && m_socket)
    {
      uint32_t pending = m_boostPacketsLeft > 0 ? m_sizeErvBoost->GetInteger () : m_sizeErv->GetInteger ();
      Ptr<Packet> p;

      while (pending > 0)
        {
          SeqTsSizeHeader stsh;
          if (pending <= stsh.GetSerializedSize ())
            {
              p = Create <Packet> (0); //minimum packet size will be header size
            }
          else if (pending < m_maxUdpPayloadSize)
            {
              p = Create <Packet> (pending - stsh.GetSerializedSize ());
            }
          else
            {
              p = Create <Packet> (m_maxUdpPayloadSize - stsh.GetSerializedSize ());
            }

          stsh.SetSeq (++m_sequenceNumber);
          stsh.SetSize (p->GetSize ());
          p->AddHeader (stsh);

          m_socket->Send (p);
          m_txTrace (p);
          NS_LOG_DEBUG ("Sending packet with size " << p->GetSize () << " Bytes");

          if (pending > p->GetSize ())
            {
              pending -= p->GetSize ();
            }
          else
            {
              pending = 0;
            }
        }

      Time interval;
      if (m_boostPacketsLeft > 0)
        {
          interval = Time::FromDouble (m_intervalErvBoost->GetValue (), Time::MS);
          m_boostPacketsLeft--;
        }
      else
        {
          interval = Time::FromDouble (m_intervalErv->GetValue (), Time::MS);
        }

      if (Simulator::Now () + interval < m_stopTime)
        {
          Simulator::Schedule (interval, &PscVideoStreaming::Send, this);
        }
    }
}

void
PscVideoStreaming::SetDistributionName (std::string distributionName)
{
  NS_LOG_FUNCTION (this << distributionName);

  m_distributionName = distributionName;

  m_sizeDistribution = PscVideoStreamingDistributions::GetSizeDistribution (m_distributionName);
  m_intervalDistribution = PscVideoStreamingDistributions::GetIntervalDistribution (m_distributionName);
}

std::string
PscVideoStreaming::GetDistributionName (void) const
{
  NS_LOG_FUNCTION (this);

  return m_distributionName;
}

} // namespace psc
} // namespace ns3
