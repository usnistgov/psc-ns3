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

#include "intel-http-server.h"

#include "intel-http-header.h"

#include <ns3/double.h>
#include <ns3/inet6-socket-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/log.h>
#include <ns3/packet.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/socket.h>
#include <ns3/string.h>
#include <ns3/tcp-socket-factory.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IntelHttpServer");

namespace psc
{

NS_OBJECT_ENSURE_REGISTERED(IntelHttpServer);

TypeId
IntelHttpServer::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::psc::IntelHttpServer")
            .SetParent<Application>()
            .AddConstructor<IntelHttpServer>()
            .AddAttribute("Port",
                          "Port on which we listen for incoming packets.",
                          UintegerValue(80),
                          MakeUintegerAccessor(&IntelHttpServer::m_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "HtmlSizeRvs",
                "Random Variable Stream to generate the size (in Bytes) of the HTML pages",
                StringValue("ns3::LogNormalRandomVariable[Mu=7.90272|Sigma=1.7643]"),
                MakePointerAccessor(&IntelHttpServer::m_htmlSizeRvs),
                MakePointerChecker<RandomVariableStream>())
            .AddAttribute("HtmlSizeLowBound",
                          "Low bound (in Bytes) to the values generated by the HTML Size "
                          "random variable stream",
                          UintegerValue(1),
                          MakeUintegerAccessor(&IntelHttpServer::m_htmlSizeLowBound),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("HtmlSizeHighBound",
                          "High bound (in Bytes) to the values generated by the HTML Size "
                          "random variable stream",
                          UintegerValue(2097152),
                          MakeUintegerAccessor(&IntelHttpServer::m_htmlSizeHighBound),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("EmbeddedObjectAmountRvs",
                          "Random Variable Stream to generate the amount of embedded objects",
                          StringValue("ns3::GammaRandomVariable[Alpha=0.141385|Beta=40.3257]"),
                          MakePointerAccessor(&IntelHttpServer::m_embeddedObjectAmountRvs),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("EmbeddedObjectAmountLowBound",
                          "Low bound to the values generated by the Embedded Object Amount random "
                          "variable stream",
                          UintegerValue(0),
                          MakeUintegerAccessor(&IntelHttpServer::m_embeddedObjectAmountLowBound),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("EmbeddedObjectAmountHighBound",
                          "High bound to the values generated by the Embedded Object Amount random "
                          "variable stream",
                          UintegerValue(300),
                          MakeUintegerAccessor(&IntelHttpServer::m_embeddedObjectAmountHighBound),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("EmbeddedObjectIatRvs",
                          "Random Variable Stream to generate the Inter Arrival Time (in seconds) "
                          "of embedded objects",
                          StringValue("ns3::WeibullRandomVariable[Scale=0.2089|Shape=0.376]"),
                          MakePointerAccessor(&IntelHttpServer::m_embeddedObjectIatRvs),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("EmbeddedObjectIatLowBound",
                          "Low bound (in seconds) to the values generated by the Embedded Object "
                          "IAT random variable stream",
                          DoubleValue(0),
                          MakeDoubleAccessor(&IntelHttpServer::m_embeddedObjectIatLowBound),
                          MakeDoubleChecker<double>(0))
            .AddAttribute("EmbeddedObjectIatHighBound",
                          "High bound (in seconds) to the values generated by the Embedded Object "
                          "IAT random variable stream",
                          DoubleValue(30),
                          MakeDoubleAccessor(&IntelHttpServer::m_embeddedObjectIatHighBound),
                          MakeDoubleChecker<double>(0))
            .AddAttribute("EmbeddedObjectSizeRvs",
                          "Random Variable Stream to generate the size of the embedded objects",
                          StringValue("ns3::LogNormalRandomVariable[Mu=7.51384|Sigma=2.17454]"),
                          MakePointerAccessor(&IntelHttpServer::m_embeddedObjectSizeRvs),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("EmbeddedObjectSizeLowBound",
                          "Low bound to the values generated by the Embedded Object Size random "
                          "variable stream",
                          UintegerValue(1),
                          MakeUintegerAccessor(&IntelHttpServer::m_embeddedObjectSizeLowBound),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("EmbeddedObjectSizeHighBound",
                          "High bound to the values generated by the Embedded Object Size random "
                          "variable stream",
                          UintegerValue(6291456),
                          MakeUintegerAccessor(&IntelHttpServer::m_embeddedObjectSizeHighBound),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("SocketBufferSize",
                          "Size in Bytes of the socket send and receive buffers",
                          UintegerValue(0x7fffffff),
                          MakeUintegerAccessor(&IntelHttpServer::m_socketBufferSize),
                          MakeUintegerChecker<uint32_t>())
            .AddTraceSource("Rx",
                            "General trace for receiving a packet of any kind.",
                            MakeTraceSourceAccessor(&IntelHttpServer::m_rxTrace),
                            "ns3::Packet::AddressTracedCallback")
            .AddTraceSource("RxRequestMainObject",
                            "Trace called after a request for a main object is received.",
                            MakeTraceSourceAccessor(&IntelHttpServer::m_rxMainObjectRequestTrace),
                            "ns3::Packet::AddressTracedCallback")
            .AddTraceSource(
                "RxRequestEmbeddedObject",
                "Trace called after a request for an embedded object is received.",
                MakeTraceSourceAccessor(&IntelHttpServer::m_rxEmbeddedObjectRequestTrace),
                "ns3::Packet::AddressTracedCallback")
            .AddTraceSource("Tx",
                            "General trace for sending a packet of any kind.",
                            MakeTraceSourceAccessor(&IntelHttpServer::m_txTrace),
                            "ns3::Packet::TracedCallback")
            .AddTraceSource("TxMainObject",
                            "Trace called before a main object is transmitted.",
                            MakeTraceSourceAccessor(&IntelHttpServer::m_txMainObjectTrace),
                            "ns3::Packet::TracedCallback")
            .AddTraceSource("TxEmbeddedObject",
                            "Trace called before an embedded object is transmitted.",
                            MakeTraceSourceAccessor(&IntelHttpServer::m_txEmbeddedObjectTrace),
                            "ns3::Packet::TracedCallback");
    return tid;
}

IntelHttpServer::IntelHttpServer()
{
    NS_LOG_FUNCTION(this);
}

IntelHttpServer::~IntelHttpServer()
{
    NS_LOG_FUNCTION(this);
}

void
IntelHttpServer::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_socket = nullptr;
    Application::DoDispose();
}

void
IntelHttpServer::StartApplication()
{
    NS_LOG_FUNCTION(this);

    if (!m_socket)
    {
        m_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_socket->SetAttribute("SndBufSize", UintegerValue(m_socketBufferSize));
        m_socket->SetAttribute("RcvBufSize", UintegerValue(m_socketBufferSize));

        m_socket->Bind(Inet6SocketAddress(Ipv6Address::GetAny(), m_port));
        m_socket->Listen();
    }

    m_socket->SetRecvCallback(MakeCallback(&IntelHttpServer::HandleRead, this));
    m_socket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address&>(),
                                MakeCallback(&IntelHttpServer::HandleAccept, this));
}

void
IntelHttpServer::StopApplication()
{
    NS_LOG_FUNCTION(this);

    if (m_socket)
    {
        m_socket->Close();
        m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
        m_socket = nullptr;
    }
}

void
IntelHttpServer::HandleRead(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);

    Ptr<Packet> recvPkt;
    Address from;
    while ((recvPkt = socket->RecvFrom(from)))
    {
        NS_LOG_DEBUG("Received packet from socket");
        if (Simulator::Now() > m_stopTime)
        {
            NS_LOG_DEBUG("Application is stopped. Discard the packet");
            break;
        }
        // We may have several packets in this segment, so let's remove them sequentially
        Ptr<Packet> tmpPacket = recvPkt->Copy();
        while (tmpPacket)
        {
            NS_LOG_DEBUG("Checking if we have enough Bytes to make a packet");
            if (m_partialPacket)
            {
                NS_LOG_DEBUG(
                    "Appending the Bytes received to the ones pending from previous receptions");
                m_partialPacket->AddAtEnd(tmpPacket);
            }
            else
            {
                NS_LOG_DEBUG("The Bytes received are all we have");
                m_partialPacket = tmpPacket;
            }
            NS_LOG_DEBUG("We have " << m_partialPacket->GetSize() << " Bytes");

            if (m_partialPacket->GetSize() >= IntelHttpHeader::GetHeaderSize())
            {
                NS_LOG_DEBUG("Enough Bytes to make a header "
                             "(Have "
                             << m_partialPacket->GetSize()
                             << ", "
                                "needed "
                             << IntelHttpHeader::GetHeaderSize() << ")");

                // We have the full header. Now check if we also have the data
                IntelHttpHeader header;
                m_partialPacket->PeekHeader(header);
                if (m_partialPacket->GetSize() >=
                    header.GetPayloadSize() + header.GetSerializedSize())
                {
                    NS_LOG_DEBUG("Enough Bytes to make a full packet ("
                                 "Header is "
                                 << IntelHttpHeader::GetHeaderSize()
                                 << ", "
                                    "Payload is "
                                 << header.GetPayloadSize() << ")");

                    // We have the full packet. Let's process it

                    // Make a copy containing only our current item
                    // for our traces
                    auto packet = m_partialPacket->Copy();
                    packet->RemoveAtEnd(packet->GetSize() -
                                        (header.GetPayloadSize() + header.GetSerializedSize()));
                    m_rxTrace(packet, from);
                    if (header.GetRequestType() == IntelHttpHeader::Type::Main)
                    {
                        NS_LOG_DEBUG("Main Object Request");
                        m_rxMainObjectRequestTrace(packet, from);
                    }
                    else
                    {
                        NS_LOG_DEBUG("Embedded Object Request");
                        m_rxEmbeddedObjectRequestTrace(packet, from);
                    }

                    NS_LOG_DEBUG("Process received packet");
                    ProcessRecvPkt(socket, from, header);

                    // Actually remove the header
                    // since we really don't need the
                    // removed header, just discard it
                    {
                        IntelHttpHeader httpHeader;
                        m_partialPacket->RemoveHeader(httpHeader);
                    }
                    // Get how many bytes we need to remove from the packet we just got
                    NS_LOG_DEBUG("Remove the Bytes 'used' for the previous packet and save "
                                 "the remaining Bytes for the next packet");
                    uint32_t bytesToRemove = tmpPacket->GetSize() -
                                             (m_partialPacket->GetSize() - header.GetPayloadSize());
                    m_partialPacket->RemoveAllPacketTags();
                    m_partialPacket->RemoveAllByteTags();
                    m_partialPacket = nullptr;

                    // Now let's see if there is anything else in the segment
                    NS_LOG_DEBUG("Removing " << bytesToRemove << " Bytes");
                    tmpPacket->RemoveAtStart(bytesToRemove);
                    if (tmpPacket->GetSize() == 0)
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
IntelHttpServer::ProcessRecvPkt(Ptr<Socket> socket,
                                const Address& from,
                                const IntelHttpHeader& httpHeader)
{
    NS_LOG_FUNCTION(this << socket << from << httpHeader);

    if (Simulator::Now() > m_stopTime)
    {
        return;
    }

    Ptr<Packet> responsePkt;
    Time waitTime;
    IntelHttpHeader header;

    if (httpHeader.GetRequestType() == IntelHttpHeader::Type::Main)
    {
        // HTML page request. We have to generate the payload size and the number of embedded
        // objects
        uint32_t size = m_htmlSizeRvs->GetInteger() - IntelHttpHeader::GetHeaderSize();
        if (size < m_htmlSizeLowBound)
        {
            size = m_htmlSizeLowBound;
        }
        else if (size > m_htmlSizeHighBound)
        {
            size = m_htmlSizeHighBound;
        }
        responsePkt = Create<Packet>(size);

        uint32_t numObjects = m_embeddedObjectAmountRvs->GetInteger();
        if (numObjects < m_embeddedObjectAmountLowBound)
        {
            numObjects = m_embeddedObjectAmountLowBound;
        }
        else if (numObjects > m_embeddedObjectAmountHighBound)
        {
            numObjects = m_embeddedObjectAmountHighBound;
        }

        header.SetRequestType(IntelHttpHeader::Type::Main);
        header.SetNumberEmbeddedObjects(numObjects);
        header.SetPayloadSize(responsePkt->GetSize());
        responsePkt->AddHeader(header);

        NS_LOG_DEBUG("Send Main Object Response of size " << size
                                                          << " Bytes "
                                                             "listing "
                                                          << numObjects << " embedded objects");
    }
    else
    {
        // Embedded object request. We wait the embedded object inter-arrival time, and generate the
        // embedded object size
        uint32_t size = m_embeddedObjectSizeRvs->GetInteger() - IntelHttpHeader::GetHeaderSize();
        if (size < m_embeddedObjectSizeLowBound)
        {
            size = m_embeddedObjectSizeLowBound;
        }
        if (size > m_embeddedObjectSizeHighBound)
        {
            size = m_embeddedObjectSizeHighBound;
        }
        responsePkt = Create<Packet>(size);

        double iatValue = m_embeddedObjectIatRvs->GetValue();
        if (iatValue < m_embeddedObjectIatLowBound)
        {
            iatValue = m_embeddedObjectIatLowBound;
        }
        if (iatValue > m_embeddedObjectIatHighBound)
        {
            iatValue = m_embeddedObjectIatHighBound;
        }
        waitTime = Seconds(iatValue);

        header.SetRequestType(IntelHttpHeader::Type::Embedded);
        header.SetNumberEmbeddedObjects(httpHeader.GetNumberEmbeddedObjects());
        header.SetPayloadSize(responsePkt->GetSize());
        responsePkt->AddHeader(header);
        NS_LOG_DEBUG("Send Embedded Object Response of size "
                     << size
                     << " Bytes "
                        "(embedded object #"
                     << httpHeader.GetNumberEmbeddedObjects() << ")");
    }

    // Send the response
    NS_LOG_DEBUG("Send Response after " << waitTime.GetSeconds() << " seconds");
    Simulator::Schedule(waitTime, &IntelHttpServer::DelayedSend, this, socket, responsePkt, from);
}

void
IntelHttpServer::HandleAccept(Ptr<Socket> socket, const Address& from)
{
    NS_LOG_FUNCTION(this << socket << from);

    socket->SetRecvCallback(MakeCallback(&IntelHttpServer::HandleRead, this));
}

void
IntelHttpServer::DelayedSend(Ptr<Socket> socket, Ptr<Packet> responsePkt, const Address& toAddr)
{
    NS_LOG_FUNCTION(this << socket << responsePkt << toAddr);

    if (!m_socket)
    {
        return;
    }
    m_txTrace(responsePkt);

    IntelHttpHeader header;
    responsePkt->PeekHeader(header);
    if (header.GetRequestType() == IntelHttpHeader::Type::Main)
    {
        m_txMainObjectTrace(responsePkt);
    }
    else
    {
        m_txEmbeddedObjectTrace(responsePkt);
    }

    socket->SendTo(responsePkt, 0, toAddr);
}

} // namespace psc
} // namespace ns3
