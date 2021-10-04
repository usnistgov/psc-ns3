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

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/packet.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"

#include "psc-application-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscApplicationServer");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscApplicationServer);

TypeId
PscApplicationServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscApplicationServer")
    .SetParent<PscApplication> ()
    .AddConstructor<PscApplicationServer> ()
    .AddAttribute ("Port",
                   "Port on which we listen for incoming packets.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PscApplicationServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Packet size for the response packet.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PscApplicationServer::m_packetSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MinResponseInterval",
                   "Minimum interval between response packets sent by this application.",
                   TimeValue (MilliSeconds (1)),
                   MakeTimeAccessor (&PscApplicationServer::m_minTimeBetweenRsps),
                   MakeTimeChecker ())
    .AddAttribute ("IsSink",
                   "Flag to indicate if this server is a sink, or if it sends a response to the packets it receives",
                   BooleanValue (false),
                   MakeBooleanAccessor (&PscApplicationServer::m_isSink),
                   MakeBooleanChecker ())
  ;
  return tid;
}

PscApplicationServer::PscApplicationServer ()
{
  NS_LOG_FUNCTION (this);
  m_timeLastSendScheduled = Time (MilliSeconds (1));

  m_rxBuffer = 0;
  m_txBuffer = Create<Packet> ();
}

PscApplicationServer::~PscApplicationServer ()
{
  NS_LOG_FUNCTION (this);
}

void
PscApplicationServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_rxBuffer = 0;
  m_txBuffer = 0;

  PscApplication::DoDispose ();
}

void
PscApplicationServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      m_socket = ns3::Socket::CreateSocket (GetNode (), m_socketTid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
      m_socket->Listen ();
    }

  m_socket->SetRecvCallback (MakeCallback (&PscApplicationServer::HandleRead, this));
  m_socket->SetAcceptCallback (
    MakeNullCallback<bool, Ptr<ns3::Socket>, const Address &> (),
    MakeCallback (&PscApplicationServer::HandleAccept, this));
  m_socket->SetSendCallback (MakeCallback (&PscApplicationServer::TrySend, this));

  if (m_socket6 == 0)
    {
      m_socket6 = ns3::Socket::CreateSocket (GetNode (), m_socketTid);
      Inet6SocketAddress local = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      m_socket6->Bind (local);
      m_socket6->Listen ();
    }

  m_socket6->SetRecvCallback (MakeCallback (&PscApplicationServer::HandleRead, this));
  m_socket6->SetAcceptCallback (
    MakeNullCallback<bool, Ptr<ns3::Socket>, const Address &> (),
    MakeCallback (&PscApplicationServer::HandleAccept, this));
  m_socket6->SetSendCallback (MakeCallback (&PscApplicationServer::TrySend, this));

  m_startStopTimeTrace (m_appName, m_startTime, (m_stopTime - Seconds (2)));
}

void
PscApplicationServer::StopNow (void)
{
  NS_LOG_FUNCTION (this);

  m_stopEvent.Cancel ();
  m_stopTime = Simulator::Now ();
  StopApplication ();

  m_startStopTimeTrace (m_appName, m_startTime, m_stopTime);
}

void
PscApplicationServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<ns3::Socket> > ());
      m_socket->SetSendCallback (MakeNullCallback<void, Ptr<ns3::Socket>, uint32_t > ());
      m_socket = 0;
    }
  if (m_socket6)
    {
      m_socket6->Close ();
      m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<ns3::Socket> > ());
      m_socket6->SetSendCallback (MakeNullCallback<void, Ptr<ns3::Socket>, uint32_t > ());
      m_socket6 = 0;
    }

}

void
PscApplicationServer::HandleRead (Ptr<ns3::Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<ns3::Packet> recvPkt;
  Address from;
  while ( (recvPkt = socket->RecvFrom (from)) )
    {
      if (Simulator::Now () > m_stopTime)
        {
          break;
        }
      // We may have several packets in this segment, so let's remove them sequentially
      Ptr<ns3::Packet> tmpPacket = recvPkt->Copy ();
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

          SeqTsSizeHeader stsh;

          if (m_rxBuffer->GetSize () >= stsh.GetSerializedSize ())
            {
              // We have the full header. Now check if we also have the data
              SeqTsSizeHeader nbh;
              m_rxBuffer->PeekHeader (nbh);
              if (m_rxBuffer->GetSize () >= nbh.GetSize () + stsh.GetSerializedSize ())
                {
                  // We have the full packet. Let's process it
                  m_rxTrace (m_appName, nbh);
                  if (!m_isSink)
                    {
                      ProcessRecvPkt (socket, from);
                    }
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
PscApplicationServer::ProcessRecvPkt (Ptr<ns3::Socket> socket, const Address& from)
{
  NS_LOG_FUNCTION (this);

  if (m_packetSize == 0)
    {
      NS_FATAL_ERROR_CONT ("Server is not sink but packet size set to 0. Response will not be sent");
      return;
    }

  // 2 seconds grace period
  if (Simulator::Now () <= (m_stopTime - Seconds (2)))
    {
      if ((Simulator::Now () - m_timeLastSendScheduled) >= m_minTimeBetweenRsps)
        {
          m_timeLastSendScheduled = Simulator::Now ();
          Simulator::Schedule (Seconds (0), &PscApplicationServer::DelayedSend, this, socket, from);
        }
      else
        {
          m_timeLastSendScheduled += m_minTimeBetweenRsps;
          Simulator::Schedule (m_timeLastSendScheduled - Simulator::Now (), &PscApplicationServer::DelayedSend, this, socket, from);
        }
    }
  // else time is over. Stop transmitting, but the application will still be alive for those
  // 2 seconds to receive packets in transit
}

void
PscApplicationServer::HandleAccept (Ptr<ns3::Socket> socket, const Address& from)
{
  NS_LOG_FUNCTION (this << socket << from);

  socket->SetRecvCallback (MakeCallback (&PscApplicationServer::HandleRead, this));
  socket->SetSendCallback (MakeCallback (&PscApplicationServer::TrySend, this));
}

void
PscApplicationServer::DelayedSend (Ptr<ns3::Socket> socket, const Address& toAddr)
{
  SeqTsSizeHeader stsh;
  stsh.SetSeq (PscSequenceNumber::GetSequenceNumber ());
  stsh.SetSize (m_packetSize);

  Ptr<Packet> pkt = Create<Packet> (m_packetSize);
  pkt->AddHeader (stsh);

  m_toAddr = toAddr;

  m_txBuffer->AddAtEnd (pkt);

  m_txTrace (m_appName, stsh);

  TrySend (socket, socket->GetTxAvailable ());
}

void
PscApplicationServer::TrySend (Ptr<Socket> socket, uint32_t txSpace)
{
  if (socket->GetTxAvailable () && m_txBuffer->GetSize () > 0)
    {
      uint32_t length = m_txBuffer->GetSize () < socket->GetTxAvailable () ? m_txBuffer->GetSize () : socket->GetTxAvailable ();
      Ptr<Packet> fragment = m_txBuffer->CreateFragment (0, length);
      uint32_t sent = socket->SendTo (fragment, 0, m_toAddr);
      m_txBuffer->RemoveAtStart (sent);
    }
}

} // namespace psc
} //namespace ns3
