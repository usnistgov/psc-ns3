/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/socket.h"
#include <ns3/pointer.h>
#include "ns3/simulator.h"

#include "psc-application-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscApplicationClient");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscApplicationClient);

TypeId
PscApplicationClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscApplicationClient")
    .SetParent<PscApplication> ()
    .AddConstructor<PscApplicationClient> ()
    .AddAttribute ("PacketSize",
                   "The packet size for this application",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PscApplicationClient::m_packetSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("RemoteAddress",
                   "The server Address",
                   AddressValue (),
                   MakeAddressAccessor (&PscApplicationClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort",
                   "The server port",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PscApplicationClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketsPerSession",
                   "Stream that generates the number of packets per session",
                   StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"),
                   MakePointerAccessor (&PscApplicationClient::m_packetsPerSessionRandomVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("PacketInterval",
                   "Stream that generates time between packets",
                   StringValue ("ns3::UniformRandomVariable[Min=10000|Max=10000]"),
                   MakePointerAccessor (&PscApplicationClient::m_packetIntervalRandomVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("SessionInterval",
                   "Stream that generates the interval between sessions",
                   StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"),
                   MakePointerAccessor (&PscApplicationClient::m_sessionIntervalRandomVariable),
                   MakePointerChecker<RandomVariableStream> ())
  ;
  return tid;
}

PscApplicationClient::PscApplicationClient ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_sendEvent = EventId ();
  m_packetsLeftThisSession = 0;
  m_txBuffer = Create<Packet> ();
}

PscApplicationClient::~PscApplicationClient ()
{
  NS_LOG_FUNCTION (this);
}

void
PscApplicationClient::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
PscApplicationClient::SetPacketsPerSession (Ptr<RandomVariableStream> pktsPerSession)
{
  NS_LOG_FUNCTION (this << pktsPerSession);
  m_packetsPerSessionRandomVariable = pktsPerSession;
}

void
PscApplicationClient::SetPacketInterval (Ptr<RandomVariableStream> pktInterval)
{
  NS_LOG_FUNCTION (this << pktInterval);
  m_packetIntervalRandomVariable = pktInterval;
}

void
PscApplicationClient::SetSessionInterval (Ptr<RandomVariableStream> sessiontInterval)
{
  NS_LOG_FUNCTION (this << sessiontInterval);
  m_sessionIntervalRandomVariable = sessiontInterval;
}

void
PscApplicationClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_packetsPerSessionRandomVariable = 0;
  m_packetIntervalRandomVariable    = 0;
  m_sessionIntervalRandomVariable   = 0;
  m_rxBuffer = 0;
  m_txBuffer = 0;
  m_socket = 0;
  PscApplication::DoDispose ();
  m_packetsLeftThisSession = -1;
}

void PscApplicationClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      m_socket = ns3::Socket::CreateSocket (GetNode (), m_socketTid);
      m_socket->SetSendCallback (MakeCallback (&PscApplicationClient::TrySend, this));
      if (Ipv4Address::IsMatchingType (m_peerAddress))
        {
          m_socket->Bind ();
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom (m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType (m_peerAddress))
        {
          m_socket->Bind ();
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom (m_peerAddress), m_peerPort));
        }
      else
        {
          NS_ABORT_MSG ("Invalid address type");
        }
    }
  m_socket->SetRecvCallback (MakeCallback (&PscApplicationClient::HandleRead, this));

  // Potential for infinite loop! Be careful defining the RandomStreamVariable!
  while (m_packetsLeftThisSession <= 0)
    {
      m_packetsLeftThisSession = m_packetsPerSessionRandomVariable->GetInteger ();
    }

  m_startStopTimeTrace (m_appName, m_startTime, (m_stopTime - Seconds (2)));

  ScheduleTransmit (Seconds (0));
}

void
PscApplicationClient::StopNow (void)
{
  NS_LOG_FUNCTION (this);

  m_stopEvent.Cancel ();
  m_stopTime = Simulator::Now ();
  StopApplication ();

  m_startStopTimeTrace (m_appName, m_startTime, m_stopTime);
}

void PscApplicationClient::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);

  if (m_socket)
    {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<ns3::Socket> > ());
      m_socket->SetSendCallback (MakeNullCallback<void, Ptr<ns3::Socket>, uint32_t> ());
      m_socket->Close ();
      m_socket = 0;
    }
}

void
PscApplicationClient::ScheduleTransmit (Time t)
{
  NS_LOG_FUNCTION (this << t);
  m_sendEvent = Simulator::Schedule (t, &PscApplicationClient::Send, this);
}

void
PscApplicationClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  SeqTsSizeHeader stsh;
  stsh.SetSeq (PscSequenceNumber::GetSequenceNumber ());
  stsh.SetSize (m_packetSize);

  Ptr<Packet> pkt = Create<Packet> (m_packetSize);
  pkt->AddHeader (stsh);

  m_txBuffer->AddAtEnd (pkt);

  m_txTrace (m_appName, stsh);

  TrySend (m_socket, m_socket->GetTxAvailable ());

  // 2 seconds grace period
  if (Simulator::Now () <= (m_stopTime - Seconds (2)) )
    {
      Time txDelay = Seconds (0);

      --m_packetsLeftThisSession;

      if (m_packetsLeftThisSession == 0)
        {
          // Potential for infinite loop! Be careful defining the RandomStreamVariable!
          while (m_packetsLeftThisSession <= 0)
            {
              m_packetsLeftThisSession = m_packetsPerSessionRandomVariable->GetInteger ();
            }

          txDelay = Seconds (m_sessionIntervalRandomVariable->GetValue ());
        }

      if (txDelay == Seconds (0))
        {
          txDelay = Seconds (m_packetIntervalRandomVariable->GetValue ());
        }

      ScheduleTransmit (txDelay);
    }
  // else time is over. Stop transmitting, but the application will still be alive for the grace period to receive packets in transit
}

void
PscApplicationClient::HandleRead (Ptr<ns3::Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  SeqTsSizeHeader stsh;
  Ptr<ns3::Packet> p;
  Address from;
  while ( (p = socket->RecvFrom (from)) )
    {
      if (Simulator::Now () > m_stopTime)
        {
          break;
        }
      // We may have several packets in this segment, so let's remove them sequentially
      Ptr<ns3::Packet> tmpPacket = p->Copy ();
      while (tmpPacket)
        {
          if (m_rxBuffer)
            {
              m_rxBuffer->AddAtEnd (tmpPacket);
            }
          else
            {
              m_rxBuffer = tmpPacket;
            }

          if (m_rxBuffer->GetSize () >= stsh.GetSerializedSize ())
            {
              // We have the full header. Now check if we also have the data
              SeqTsSizeHeader nbh;
              m_rxBuffer->PeekHeader (nbh);
              if (m_rxBuffer->GetSize () >= nbh.GetSize () + stsh.GetSerializedSize ())
                {
                  // We have the full packet. Let's process it
                  m_rxTrace (m_appName, nbh);

                  // Get how many bytes we need to remove from the packet we just got
                  // bytesUsed = receivedPacketSize - (TotalSize - PayloadSize - HeaderSize)
                  uint32_t bytesToRemove = tmpPacket->GetSize () - (m_rxBuffer->GetSize () - nbh.GetSize () - stsh.GetSerializedSize ());
                  m_rxBuffer->RemoveAllPacketTags ();
                  m_rxBuffer->RemoveAllByteTags ();
                  m_rxBuffer = 0;

                  // Now let's see if there is anything else in the segment
                  tmpPacket->RemoveAtStart (bytesToRemove);
                  if (tmpPacket->GetSize () == 0)
                    {
                      tmpPacket = 0;
                    }
                }
              else
                {
                  // We don't have the full packet. Let's wait.
                  tmpPacket = 0;
                }
            }
          else
            {
              // We still need to keep receiving data (we don't have the full header).
              tmpPacket = 0;
            }
        }
    }
}

void
PscApplicationClient::TrySend (Ptr<Socket> socket, uint32_t txSpace)
{
  uint32_t txPending = m_txBuffer->GetSize ();

  if (txSpace > 0 && txPending > 0)
    {
      uint32_t length = txPending < txSpace ? txPending : txSpace;
      Ptr<Packet> fragment = m_txBuffer->CreateFragment (0, length);
      uint32_t sent = socket->Send (fragment);
      m_txBuffer->RemoveAtStart (sent);
    }
}

} // namespace psc
} // namespace ns3
