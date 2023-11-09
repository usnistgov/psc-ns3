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

#include <ns3/log.h>
#include <ns3/packet.h>
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/tcp-socket-factory.h>
#include <ns3/abort.h>
#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/string.h>

#include "intel-http-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("IntelHttpClient");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (IntelHttpClient);

TypeId
IntelHttpClient::GetTypeId()
{
  static TypeId tid =
      TypeId ("ns3::psc::IntelHttpClient")
          .SetParent<Application> ()
          .AddConstructor<IntelHttpClient> ()
          .AddAttribute ("RemoteAddress", "The server Address", AddressValue (),
                         MakeAddressAccessor (&IntelHttpClient::m_peerAddress),
                         MakeAddressChecker ())
          .AddAttribute ("RemotePort", "The server port", UintegerValue (80),
                         MakeUintegerAccessor (&IntelHttpClient::m_peerPort),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("CacheRvs",
                         "Stream that generates the value compared with the CacheThreshold to "
                         "decide if an embedded object is cached or not. "
                         "Should generate a value between 0 and 1",
                         StringValue ("ns3::UniformRandomVariable[Min=0|Max=1]"),
                         MakePointerAccessor (&IntelHttpClient::m_cacheRvs),
                         MakePointerChecker<RandomVariableStream> ())
          .AddAttribute ("CacheThreshold",
                         "Threshold to decide if an embedded object is cached or not. If the value "
                         "generated by CacheRvs is less than or equal to this value, then the "
                         "object was cached and will not be requested",
                         DoubleValue (0.16),
                         MakeDoubleAccessor (&IntelHttpClient::m_cacheThreshold),
                         MakeDoubleChecker<double> (0, 1))
          .AddAttribute ("RequestSizeRvs",
                         "Random Variable Stream to generate the size (in Bytes) of the requests",
                         StringValue ("ns3::UniformRandomVariable[Min=7|Max=630]"),
                         MakePointerAccessor (&IntelHttpClient::m_requestSizeRvs),
                         MakePointerChecker<RandomVariableStream> ())
          .AddAttribute ("ParseTimeRvs",
                         "Random Variable Stream to generate the time (in seconds) required to parse "
                         "a downloaded file",
                         StringValue ("ns3::LogNormalRandomVariable[Mu=-1.24892|Sigma=2.08427]"),
                         MakePointerAccessor (&IntelHttpClient::m_parseTimeRvs),
                         MakePointerChecker<RandomVariableStream> ())
          .AddAttribute ("ParseTimeLowBound",
                         "Low bound (in seconds) to the values generated by the parse time "
                         "random variable stream, in seconds",
                         DoubleValue (0),
                         MakeDoubleAccessor (&IntelHttpClient::m_parseTimeLowBound),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("ParseTimeHighBound",
                         "High bound (in seconds) to the values generated by the parse time "
                         "random variable stream, in seconds",
                         DoubleValue (300),
                         MakeDoubleAccessor (&IntelHttpClient::m_parseTimeHighBound),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("ReadTimeRvs",
                         "Random Variable Stream to generate the time (in seconds) spent reading "
                         "a page after it has been downloaded",
                         StringValue ("ns3::LogNormalRandomVariable[Mu=-0.495204|Sigma=2.7731]"),
                         MakePointerAccessor (&IntelHttpClient::m_readTimeRvs),
                         MakePointerChecker<RandomVariableStream> ())
          .AddAttribute ("ReadTimeLowBound",
                         "Low bound (in seconds) to the values generated by the reading time "
                         "random variable stream, in seconds",
                         DoubleValue (0), MakeDoubleAccessor (&IntelHttpClient::m_readTimeLowBound),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("ReadTimeHighBound",
                         "High bound (in seconds) to the values generated by the reading time "
                         "random variable stream, in seconds",
                         DoubleValue (10000),
                         MakeDoubleAccessor (&IntelHttpClient::m_readTimeHighBound),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SocketBufferSize", "Size in Bytes of the socket send and receive buffers",
                         UintegerValue (0x7fffffff),
                         MakeUintegerAccessor (&IntelHttpClient::m_socketBufferSize),
                         MakeUintegerChecker<uint32_t> ())
          .AddTraceSource ("Rx", "General trace for receiving a packet of any kind.",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_rxTrace),
                           "ns3::Packet::AddressTracedCallback")
          .AddTraceSource ("RxMainObject",
                           "Trace for when a packet containing a main object is received",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_rxMainObjectTrace),
                           "ns3::Packet::AddressTracedCallback")
          .AddTraceSource ("RxEmbeddedObject",
                           "Trace for when a packet containing an embedded object is received",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_rxEmbeddedObjectTrace),
                           "ns3::Packet::AddressTracedCallback")
          .AddTraceSource ("Tx", "General trace for sending a packet of any kind.",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_txTrace),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("TxRequestMainObject",
                           "Trace called before a request for a main object is transmitted.",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_txRequestMainObjectTrace),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("TxRequestEmbeddedObject",
                           "Trace called before a request for an embedded object is transmitted.",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_txRequestEmbeddedObjectTrace),
                           "ns3::Packet::TracedCallback")
          .AddTraceSource ("CacheHit",
                           "Trace called when object on a page was "
                           "cached and does not need to be requested",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_cacheHit),
                           "ns3::psc::IntelHttpClient::CacheCallback")
          .AddTraceSource ("CacheMiss",
                           "Trace called when object on a page was "
                           "not cached and needs to be requested",
                           MakeTraceSourceAccessor (&IntelHttpClient::m_cacheMiss),
                           "ns3::psc::IntelHttpClient::CacheCallback");
  return tid;
}

IntelHttpClient::IntelHttpClient ()
{
  NS_LOG_FUNCTION (this);
}

IntelHttpClient::~IntelHttpClient ()
{
  NS_LOG_FUNCTION (this);
}

void
IntelHttpClient::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
IntelHttpClient::DoDispose()
{
  NS_LOG_FUNCTION (this);
  m_socket = nullptr;
  Application::DoDispose ();
}

void
IntelHttpClient::StartApplication()
{
  NS_LOG_FUNCTION (this);
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());
      m_socket->SetAttribute ("SndBufSize", UintegerValue (m_socketBufferSize));
      m_socket->SetAttribute ("RcvBufSize", UintegerValue (m_socketBufferSize));

      if (Ipv4Address::IsMatchingType (m_peerAddress))
        {
          m_socket->Bind ();
          m_socket->Connect (
              InetSocketAddress (Ipv4Address::ConvertFrom (m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType (m_peerAddress))
        {
          m_socket->Bind6 ();
          m_socket->Connect (
              Inet6SocketAddress (Ipv6Address::ConvertFrom (m_peerAddress), m_peerPort));
        }
    }
  m_socket->SetRecvCallback (MakeCallback (&IntelHttpClient::HandleRead, this));

  m_sendEvent = Simulator::ScheduleNow (&IntelHttpClient::SendRequestPage, this);
}

void
IntelHttpClient::StopApplication()
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
  Simulator::Cancel (m_parseEvent);

  if (m_socket)
    {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
      m_socket->Close ();
      m_socket = nullptr;
    }
}

void
IntelHttpClient::SendRequestPage()
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_IF (!m_sendEvent.IsExpired (),
                   "ERROR! Attempting to transmit a request while another one is scheduled!");

  uint16_t size = m_requestSizeRvs->GetInteger () - IntelHttpHeader::GetHeaderSize ();
  Ptr<Packet> p = Create<Packet> (size);

  IntelHttpHeader httpHeader;
  httpHeader.SetRequestType (IntelHttpHeader::Type::Main);
  httpHeader.SetNumberEmbeddedObjects (0);
  httpHeader.SetPayloadSize (p->GetSize ());
  p->AddHeader (httpHeader);

  NS_LOG_DEBUG ("Send Main Object Request of size " << size << " Bytes");
  m_txRequestMainObjectTrace (p);
  m_txTrace (p);
  m_socket->Send (p);
  // Now wait for the response
}

void
IntelHttpClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> p;
  Address from;
  while ((p = socket->RecvFrom (from)))
    {
      NS_LOG_DEBUG ("Received packet from socket");
      if (Simulator::Now () > m_stopTime)
        {
          NS_LOG_DEBUG ("Application is stopped. Discard the packet");
          break;
        }
      // We may have several packets in this segment, so let's remove them sequentially
      Ptr<Packet> tmpPacket = p->Copy ();
      while (tmpPacket)
        {
          NS_LOG_DEBUG ("Checking if we have enough Bytes to make a packet");
          if (m_partialPacket)
            {
              NS_LOG_DEBUG ("Appending the Bytes received to the ones pending from previous receptions");
              m_partialPacket->AddAtEnd (tmpPacket);
            }
          else
            {
              NS_LOG_DEBUG ("The Bytes received are all we have");
              m_partialPacket = tmpPacket;
            }
          NS_LOG_DEBUG ("We have " << m_partialPacket->GetSize () << " Bytes");

          if (m_partialPacket->GetSize () >= IntelHttpHeader::GetHeaderSize ())
            {
              NS_LOG_DEBUG ("Enough Bytes to make a header "
                  "(Have " << m_partialPacket->GetSize() << ", "
                  "needed " << IntelHttpHeader::GetHeaderSize () << ")");

              // We have the full header. Now check if we also have the data
              IntelHttpHeader header;
              m_partialPacket->PeekHeader (header);
              if (m_partialPacket->GetSize () >=
                  header.GetPayloadSize () + header.GetSerializedSize ())
                {
                  NS_LOG_DEBUG ("Enough Bytes to make a full packet ("
                      "Header is " << IntelHttpHeader::GetHeaderSize () << ", "
                      "Payload is " << header.GetPayloadSize () << ")");

                  // We have the full packet. Let's process it

                  // Make a copy containing only our current item
                  // for our traces
                  auto packet = m_partialPacket->Copy ();
                  packet->RemoveAtEnd (packet->GetSize () -
                                       (header.GetPayloadSize () + header.GetSerializedSize ()));
                  m_rxTrace (packet, from);
                  if (header.GetRequestType () == IntelHttpHeader::Type::Main)
                    {
                      NS_LOG_DEBUG ("Main Object Response");
                      m_rxMainObjectTrace (packet, from);
                    }
                  else
                    {
                      NS_LOG_DEBUG ("Embedded Object Response");
                      m_rxEmbeddedObjectTrace (packet, from);
                    }

                  double parseValue = m_parseTimeRvs->GetValue ();
                  if (parseValue < m_parseTimeLowBound)
                    {
                      parseValue = m_parseTimeLowBound;
                    }
                  if (parseValue > m_parseTimeHighBound)
                    {
                      parseValue = m_parseTimeHighBound;
                    }

                  NS_LOG_DEBUG ("Process received packet in " << parseValue << " seconds");
                  m_parseEvent = Simulator::Schedule (
                      Seconds (parseValue), &IntelHttpClient::ProcessRecvPkt, this, from, header);

                  // Actually remove the header
                  // since we really don't need the
                  // removed header, just discard it
                  {
                    IntelHttpHeader httpHeader;
                    m_partialPacket->RemoveHeader (httpHeader);
                  }
                  // Get how many bytes we need to remove from the packet we just got
                  NS_LOG_DEBUG ("Remove the Bytes 'used' for the previous packet and save "
                      "the remaining Bytes for the next packet");
                  uint32_t bytesToRemove = tmpPacket->GetSize () -
                                           (m_partialPacket->GetSize () - header.GetPayloadSize ());
                  m_partialPacket->RemoveAllPacketTags ();
                  m_partialPacket->RemoveAllByteTags ();
                  m_partialPacket = nullptr;

                  // Now let's see if there is anything else in the segment
                  NS_LOG_DEBUG ("Removing " << bytesToRemove << " Bytes");
                  tmpPacket->RemoveAtStart (bytesToRemove);
                  if (tmpPacket->GetSize () == 0)
                    {
                      tmpPacket = nullptr;
                    }
                }
              else
                {
                  // We don't have the full packet. Let's wait.
                  tmpPacket = nullptr;
                }
            }
          else
            {
              // We still need to keep receiving data (we don't have the full header).
              tmpPacket = nullptr;
            }
        }
    }
}

void
IntelHttpClient::ProcessRecvPkt (const Address &from, const IntelHttpHeader &recvHttpHeader)
{
  NS_LOG_FUNCTION (this << from << recvHttpHeader);

  if (!m_socket)
    {
      return;
    }

  if (recvHttpHeader.GetRequestType () == IntelHttpHeader::Type::Main)
    {
      NS_LOG_DEBUG ("Received Main Object with " << recvHttpHeader.GetNumberEmbeddedObjects () << 
          " embedded objects");
      m_objectsLeft = recvHttpHeader.GetNumberEmbeddedObjects ();
      if (m_objectsLeft > 0)
        {
          for (int i = 0; i < recvHttpHeader.GetNumberEmbeddedObjects (); i++)
            {
              NS_LOG_DEBUG ("Checking embedded object #" << i);
              if (m_cacheRvs->GetValue () > m_cacheThreshold)
                {
                  NS_LOG_DEBUG ("Embedded Object #" << i << " is not cached");
                  m_cacheMiss (i);
                  uint32_t randomSizeVal = m_requestSizeRvs->GetInteger ();
                  NS_ABORT_MSG_IF (randomSizeVal <= IntelHttpHeader::GetHeaderSize (),
                      "Packet size generated (" << randomSizeVal << " Bytes) is not large enough "
                      "to hold the header (" << IntelHttpHeader::GetHeaderSize () << " Bytes) and "
                      "at least 1 Bytes of payload");
                  uint32_t size = randomSizeVal - IntelHttpHeader::GetHeaderSize ();
                  Ptr<Packet> p = Create<Packet> (size);

                  IntelHttpHeader httpHeader;
                  httpHeader.SetRequestType (IntelHttpHeader::Type::Embedded);
                  httpHeader.SetNumberEmbeddedObjects (i);
                  httpHeader.SetPayloadSize (p->GetSize ());
                  p->AddHeader (httpHeader);
      
                  NS_LOG_DEBUG ("Sending Embedded Object Request of size " << size << 
                      " for Embedded Object #" << i);

                  m_txRequestEmbeddedObjectTrace (p);
                  m_txTrace (p);
                  m_socket->Send (p);
                }
              else
                {
                  NS_LOG_DEBUG ("Embedded Object #" << i << " is cached");
                  m_cacheHit (i);
                  m_objectsLeft--;
                }
            }
        }
      if (m_objectsLeft == 0)
        {
          NS_LOG_DEBUG ("No Embedded Object left to receive.");
          double readValue = m_readTimeRvs->GetValue ();
          if (readValue < m_readTimeLowBound)
            {
              readValue = m_readTimeLowBound;
            }
          if (readValue > m_readTimeHighBound)
            {
              readValue = m_readTimeHighBound;
            }
          m_sendEvent =
              Simulator::Schedule (Seconds (readValue), &IntelHttpClient::SendRequestPage, this);
        }
    }
  else
    {
      NS_LOG_DEBUG ("Received Embedded Object #" << recvHttpHeader.GetNumberEmbeddedObjects () << 
          " of size " << recvHttpHeader.GetPayloadSize () << " Bytes");

      m_objectsLeft--;
      NS_LOG_DEBUG (m_objectsLeft << " Embedded Objects pending");
      if (m_objectsLeft == 0)
        {
          NS_LOG_DEBUG ("No Embedded Object left to receive.");
          double readValue = m_readTimeRvs->GetValue ();
          if (readValue < m_readTimeLowBound)
            {
              readValue = m_readTimeLowBound;
            }
          if (readValue > m_readTimeHighBound)
            {
              readValue = m_readTimeHighBound;
            }
          m_sendEvent =
              Simulator::Schedule (Seconds (readValue), &IntelHttpClient::SendRequestPage, this);
        }
    }
}

} // namespace psc
} // namespace ns3
