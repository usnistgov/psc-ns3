/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "nr-sl-ue-mac-harq.h"
#include <ns3/packet-burst.h>
#include <ns3/packet.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeMacHarq");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeMacHarq);

TypeId
NrSlUeMacHarq::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrSlUeMacHarq")
    .SetParent<Object> ()
    .AddConstructor <NrSlUeMacHarq> ()
    .SetGroupName ("nr")
    .AddTraceSource ("RxHarqFeedback",
                     "Receive HARQ feedback trace",
                     MakeTraceSourceAccessor (&NrSlUeMacHarq::m_rxHarqFeedback),
                     "ns3::NrSlUeMacHarq::RxHarqFeedbackTracedCallback")
    .AddTraceSource ("Allocate",
                     "Trace HARQ process ID allocation",
                     MakeTraceSourceAccessor (&NrSlUeMacHarq::m_allocateTrace),
                     "ns3::NrSlUeMacHarq::AllocateTracedCallback")
    .AddTraceSource ("Deallocate",
                     "Trace HARQ process ID deallocation",
                     MakeTraceSourceAccessor (&NrSlUeMacHarq::m_deallocateTrace),
                     "ns3::NrSlUeMacHarq::DeallocateTracedCallback")
    .AddTraceSource ("RequestPacketBurst",
                     "Trace requests for packet bursts (retransmissions)",
                     MakeTraceSourceAccessor (&NrSlUeMacHarq::m_packetBurstTrace),
                     "ns3::NrSlUeMacHarq::PacketBurstTracedCallback")
    .AddTraceSource ("Timeout",
                     "Trace HARQ process timer expiry",
                     MakeTraceSourceAccessor (&NrSlUeMacHarq::m_timeoutTrace),
                     "ns3::NrSlUeMacHarq::TimeoutTracedCallback")
  ;
  return tid;
}

NrSlUeMacHarq::NrSlUeMacHarq ()
{
  NS_LOG_FUNCTION (this);
}

NrSlUeMacHarq::~NrSlUeMacHarq ()
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeMacHarq::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  for (auto it:m_nrSlHarqPktBuffer)
    {
      it.pktBurst = nullptr;
    }
  m_nrSlHarqPktBuffer.clear ();
}

void
NrSlUeMacHarq::InitHarqBuffer (uint8_t maxSlProcesses)
{
  NS_LOG_FUNCTION (this << +maxSlProcesses);

  m_nrSlHarqPktBuffer.resize (maxSlProcesses);
  for (uint8_t i = 0; i < maxSlProcesses; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      m_nrSlHarqPktBuffer.at (i).pktBurst = pb;
    }
}

uint8_t
NrSlUeMacHarq::AssignNrSlHarqProcessId (uint8_t harqId, uint32_t dstL2Id, Time timeout)
{
  NS_LOG_FUNCTION (this << +harqId << dstL2Id << timeout);
  NS_ABORT_MSG_IF (GetNumAvailableHarqIds () == 0, "All the Sidelink processes are busy");
  NS_LOG_DEBUG ("Calling allocate trace for ID " << +harqId << " dstL2Id " << dstL2Id
    << " timeout " << timeout.As (Time::MS) << " size " << m_nrSlHarqIdBuffer.size ());
  m_allocateTrace (harqId, dstL2Id, timeout, m_nrSlHarqIdBuffer.size ());
  //set the given destination in m_nrSlHarqPktBuffer at the index equal to
  //availableHarqId so we can check it while adding the packet.
  m_nrSlHarqPktBuffer.at (harqId).dstL2Id = dstL2Id;
  m_nrSlHarqPktBuffer.at (harqId).timer = Simulator::Schedule (timeout,
                                            &NrSlUeMacHarq::HarqProcessTimerExpiry, this, harqId);
  return harqId;
}

uint8_t
NrSlUeMacHarq::GetNumAvailableHarqIds () const
{
  uint8_t count = 0;
  for (uint32_t i = 0; i < m_nrSlHarqPktBuffer.size (); i++)
    {
      if (m_nrSlHarqPktBuffer.at (i).dstL2Id == std::numeric_limits <uint32_t>::max ())
        {
          count++;
        }
    }
  return count;
}

std::deque<uint8_t>
NrSlUeMacHarq::GetAvailableHarqIds () const
{
  std::deque<uint8_t> dq;
  for (uint32_t i = 0; i < m_nrSlHarqPktBuffer.size (); i++)
    {
      if (m_nrSlHarqPktBuffer.at (i).dstL2Id == std::numeric_limits <uint32_t>::max ())
        {
          dq.push_back (i);
        }
    }
  return dq;
}


bool
NrSlUeMacHarq::IsHarqIdAvailable (uint8_t harqId) const
{
  return (m_nrSlHarqPktBuffer.at (harqId).dstL2Id == std::numeric_limits <uint32_t>::max () ? true : false);
}

void
NrSlUeMacHarq::AddPacket (uint32_t dstL2Id, uint8_t lcId, uint8_t harqId, Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION (this << dstL2Id << +lcId << +harqId);
  NS_ABORT_MSG_IF (m_nrSlHarqPktBuffer.at (harqId).dstL2Id != dstL2Id, "the HARQ id " << +harqId << " does not belongs to the destination " << dstL2Id);
  m_nrSlHarqPktBuffer.at (harqId).lcidList.insert (lcId);
  NS_ASSERT_MSG (m_nrSlHarqPktBuffer.at (harqId).pktBurst != nullptr, " Packet burst not initialized for HARQ id " << +harqId);
  m_nrSlHarqPktBuffer.at (harqId).pktBurst->AddPacket (pkt);
  //Each LC have one MAC PDU in a TB. Packet burst here, imitates a TB, therefore,
  //the number of LCs inside lcidList and the packets inside the packet burst
  //must be equal.
  NS_ABORT_MSG_IF (m_nrSlHarqPktBuffer.at (harqId).lcidList.size () != m_nrSlHarqPktBuffer.at (harqId).pktBurst->GetNPackets (),
                   "Mismatch in number of LCIDs and the number of packets for SL HARQ ID " << +harqId << " dest " << dstL2Id);
}

void
NrSlUeMacHarq::RecvNrSlHarqFeedback (SlHarqInfo harqInfo)
{
  NS_LOG_FUNCTION (this << harqInfo.m_dstL2Id << +harqInfo.m_harqProcessId << harqInfo.IsReceivedOk ());
  m_rxHarqFeedback (harqInfo);
  if (IsHarqIdAvailable (harqInfo.m_harqProcessId))
    {
      NS_LOG_DEBUG ("Feedback (possibly stale) received for unused HARQ ID " << +harqInfo.m_harqProcessId);
      return;
    }
  if (harqInfo.IsReceivedOk () && (m_nrSlHarqPktBuffer.at (harqInfo.m_harqProcessId).dstL2Id == harqInfo.m_dstL2Id))
    {
      FlushNrSlHarqBuffer (harqInfo.m_harqProcessId);
    }
}

void
NrSlUeMacHarq::FlushNrSlHarqBuffer (uint8_t harqId)
{
  NS_LOG_FUNCTION (this << harqId);
  // Check that the given HARQ process ID is currently active before adding back to the available pool
  if (!IsHarqIdAvailable (harqId))
    {
      m_nrSlHarqIdBuffer.push_back (harqId);
      NS_LOG_DEBUG ("Calling deallocate trace for ID " << +harqId << " size " << m_nrSlHarqIdBuffer.size ());
      m_deallocateTrace (harqId, m_nrSlHarqIdBuffer.size ());
    }
  if (m_nrSlHarqPktBuffer.at (harqId).timer.IsRunning ()) 
    {
      m_nrSlHarqPktBuffer.at (harqId).timer.Cancel ();
    }
  // Reinitialize HARQ packet buffer
  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
  m_nrSlHarqPktBuffer.at (harqId).pktBurst = pb;
  m_nrSlHarqPktBuffer.at (harqId).lcidList.clear ();
  m_nrSlHarqPktBuffer.at (harqId).dstL2Id = std::numeric_limits <uint32_t>::max ();
}

Ptr<PacketBurst>
NrSlUeMacHarq::GetPacketBurst (uint32_t dstL2Id, uint8_t harqId) const
{
  NS_LOG_FUNCTION (this << dstL2Id << +harqId);
  if (m_nrSlHarqPktBuffer.at (harqId).dstL2Id != dstL2Id)
    {
      // This operation can fail to return a packet burst if retransmissions
      // have been completed on this HARQ Process ID
      NS_LOG_DEBUG ("No packet to return");
      return nullptr;
    }
  m_packetBurstTrace (dstL2Id, harqId);
  return m_nrSlHarqPktBuffer.at (harqId).pktBurst;
}

void
NrSlUeMacHarq::HarqProcessTimerExpiry (uint8_t harqId)
{
  NS_LOG_FUNCTION (this << +harqId);
  m_timeoutTrace (harqId);
  FlushNrSlHarqBuffer (harqId);
  return;
}

} // namespace ns3
