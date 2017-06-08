/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.cat>
 *         Nicola Baldo <nbaldo@cttc.cat>
 */


#include "nist-epc-sgw-pgw-application.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4.h"
#include "ns3/inet-socket-address.h"
#include "ns3/nist-epc-gtpu-header.h"
#include "ns3/abort.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcSgwPgwApplication");

/////////////////////////
// NistUeInfo
/////////////////////////


NistEpcSgwPgwApplication::NistUeInfo::NistUeInfo ()
{
  NS_LOG_FUNCTION (this);
}

void
NistEpcSgwPgwApplication::NistUeInfo::AddBearer (Ptr<NistEpcTft> tft, uint8_t bearerId, uint32_t teid)
{
  NS_LOG_FUNCTION (this << tft << teid);
  m_teidByBearerIdMap[bearerId] = teid;
  return m_tftClassifier.Add (tft, teid);
}

void
NistEpcSgwPgwApplication::NistUeInfo::RemoveBearer (uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << bearerId);
  m_teidByBearerIdMap.erase (bearerId);
}

uint32_t
NistEpcSgwPgwApplication::NistUeInfo::Classify (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  // we hardcode DOWNLINK direction since the PGW is espected to
  // classify only downlink packets (uplink packets will go to the
  // internet without any classification). 
  return m_tftClassifier.Classify (p, NistEpcTft::DOWNLINK);
}

Ipv4Address 
NistEpcSgwPgwApplication::NistUeInfo::GetEnbAddr ()
{
  return m_enbAddr;
}

void
NistEpcSgwPgwApplication::NistUeInfo::SetEnbAddr (Ipv4Address enbAddr)
{
  m_enbAddr = enbAddr;
}

Ipv4Address 
NistEpcSgwPgwApplication::NistUeInfo::GetUeAddr ()
{
  return m_ueAddr;
}

void
NistEpcSgwPgwApplication::NistUeInfo::SetUeAddr (Ipv4Address ueAddr)
{
  m_ueAddr = ueAddr;
}

/////////////////////////
// NistEpcSgwPgwApplication
/////////////////////////


TypeId
NistEpcSgwPgwApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcSgwPgwApplication")
    .SetParent<Object> ();
  return tid;
}

void
NistEpcSgwPgwApplication::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_s1uSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_s1uSocket = 0;
  delete (m_s11SapSgw);
}

  

NistEpcSgwPgwApplication::NistEpcSgwPgwApplication (const Ptr<VirtualNetDevice> tunDevice, const Ptr<Socket> s1uSocket)
  : m_s1uSocket (s1uSocket),
    m_tunDevice (tunDevice),
    m_gtpuUdpPort (2152), // fixed by the standard
    m_teidCount (0),
    m_s11SapMme (0)
{
  NS_LOG_FUNCTION (this << tunDevice << s1uSocket);
  m_s1uSocket->SetRecvCallback (MakeCallback (&NistEpcSgwPgwApplication::RecvFromS1uSocket, this));
  m_s11SapSgw = new MemberNistEpcS11SapSgw<NistEpcSgwPgwApplication> (this);
}

  
NistEpcSgwPgwApplication::~NistEpcSgwPgwApplication ()
{
  NS_LOG_FUNCTION (this);
}


bool
NistEpcSgwPgwApplication::RecvFromTunDevice (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << source << dest << packet << packet->GetSize ());

  // get IP address of UE
  Ptr<Packet> pCopy = packet->Copy ();
  Ipv4Header ipv4Header;
  pCopy->RemoveHeader (ipv4Header);
  Ipv4Address ueAddr =  ipv4Header.GetDestination ();
  NS_LOG_LOGIC ("packet addressed to UE " << ueAddr);

  // find corresponding NistUeInfo address
  std::map<Ipv4Address, Ptr<NistUeInfo> >::iterator it = m_ueInfoByAddrMap.find (ueAddr);
  if (it == m_ueInfoByAddrMap.end ())
    {        
      NS_LOG_WARN ("unknown UE address " << ueAddr);
    }
  else
    {
      Ipv4Address enbAddr = it->second->GetEnbAddr ();      
      uint32_t teid = it->second->Classify (packet);   
      if (teid == 0)
        {
          NS_LOG_WARN ("no matching bearer for this packet");                   
        }
      else
        {
          SendToS1uSocket (packet, enbAddr, teid);
        }
    }
  // there is no reason why we should notify the TUN
  // VirtualNetDevice that he failed to send the packet: if we receive
  // any bogus packet, it will just be silently discarded.
  const bool succeeded = true;
  return succeeded;
}

void 
NistEpcSgwPgwApplication::RecvFromS1uSocket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);  
  NS_ASSERT (socket == m_s1uSocket);
  Ptr<Packet> packet = socket->Recv ();
  NistGtpuHeader gtpu;
  packet->RemoveHeader (gtpu);
  uint32_t teid = gtpu.GetTeid ();

  /// \internal
  /// Workaround for \bugid{231}
  SocketAddressTag tag;
  packet->RemovePacketTag (tag);

  SendToTunDevice (packet, teid);
}

void 
NistEpcSgwPgwApplication::SendToTunDevice (Ptr<Packet> packet, uint32_t teid)
{
  NS_LOG_FUNCTION (this << packet << teid);
  NS_LOG_LOGIC (" packet size: " << packet->GetSize () << " bytes");
  m_tunDevice->Receive (packet, 0x0800, m_tunDevice->GetAddress (), m_tunDevice->GetAddress (), NetDevice::PACKET_HOST);
}

void 
NistEpcSgwPgwApplication::SendToS1uSocket (Ptr<Packet> packet, Ipv4Address enbAddr, uint32_t teid)
{
  NS_LOG_FUNCTION (this << packet << enbAddr << teid);

  NistGtpuHeader gtpu;
  gtpu.SetTeid (teid);
  // From 3GPP TS 29.281 v10.0.0 Section 5.1
  // Length of the payload + the non obligatory GTP-U header
  gtpu.SetLength (packet->GetSize () + gtpu.GetSerializedSize () - 8);  
  packet->AddHeader (gtpu);
  uint32_t flags = 0;
  m_s1uSocket->SendTo (packet, flags, InetSocketAddress (enbAddr, m_gtpuUdpPort));
}


void 
NistEpcSgwPgwApplication::SetS11SapMme (NistEpcS11SapMme * s)
{
  m_s11SapMme = s;
}

NistEpcS11SapSgw* 
NistEpcSgwPgwApplication::GetS11SapSgw ()
{
  return m_s11SapSgw;
}

void 
NistEpcSgwPgwApplication::AddEnb (uint16_t cellId, Ipv4Address enbAddr, Ipv4Address sgwAddr)
{
  NS_LOG_FUNCTION (this << cellId << enbAddr << sgwAddr);
  NistEnbInfo enbInfo;
  enbInfo.enbAddr = enbAddr;
  enbInfo.sgwAddr = sgwAddr;
  m_enbInfoByCellId[cellId] = enbInfo;
}

void 
NistEpcSgwPgwApplication::AddUe (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  Ptr<NistUeInfo> ueInfo = Create<NistUeInfo> ();
  m_ueInfoByImsiMap[imsi] = ueInfo;
}

void 
NistEpcSgwPgwApplication::SetUeAddress (uint64_t imsi, Ipv4Address ueAddr)
{
  NS_LOG_FUNCTION (this << imsi << ueAddr);
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi); 
  m_ueInfoByAddrMap[ueAddr] = ueit->second;
  ueit->second->SetUeAddr (ueAddr);
}

void 
NistEpcSgwPgwApplication::DoCreateSessionRequest (NistEpcS11SapSgw::NistCreateSessionRequestMessage req)
{
  NS_LOG_FUNCTION (this << req.imsi);
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (req.imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << req.imsi); 
  uint16_t cellId = req.uli.gci;
  std::map<uint16_t, NistEnbInfo>::iterator enbit = m_enbInfoByCellId.find (cellId);
  NS_ASSERT_MSG (enbit != m_enbInfoByCellId.end (), "unknown CellId " << cellId); 
  Ipv4Address enbAddr = enbit->second.enbAddr;
  ueit->second->SetEnbAddr (enbAddr);

  NistEpcS11SapMme::NistCreateSessionResponseMessage res;
  res.teid = req.imsi; // trick to avoid the need for allocating TEIDs on the S11 interface

  for (std::list<NistEpcS11SapSgw::NistBearerContextToBeCreated>::iterator bit = req.bearerContextsToBeCreated.begin ();
       bit != req.bearerContextsToBeCreated.end ();
       ++bit)
    {
      // simple sanity check. If you ever need more than 4M teids
      // throughout your simulation, you'll need to implement a smarter teid
      // management algorithm. 
      NS_ABORT_IF (m_teidCount == 0xFFFFFFFF);
      uint32_t teid = ++m_teidCount;  
      ueit->second->AddBearer (bit->tft, bit->epsBearerId, teid);

      NistEpcS11SapMme::NistBearerContextCreated bearerContext;
      bearerContext.sgwNistFteid.teid = teid;
      bearerContext.sgwNistFteid.address = enbit->second.sgwAddr;
      bearerContext.epsBearerId =  bit->epsBearerId; 
      bearerContext.bearerLevelQos = bit->bearerLevelQos; 
      bearerContext.tft = bit->tft;
      res.bearerContextsCreated.push_back (bearerContext);
    }
  m_s11SapMme->CreateSessionResponse (res);
  
}

void 
NistEpcSgwPgwApplication::DoModifyBearerRequest (NistEpcS11SapSgw::NistModifyBearerRequestMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi); 
  uint16_t cellId = req.uli.gci;
  std::map<uint16_t, NistEnbInfo>::iterator enbit = m_enbInfoByCellId.find (cellId);
  NS_ASSERT_MSG (enbit != m_enbInfoByCellId.end (), "unknown CellId " << cellId); 
  Ipv4Address enbAddr = enbit->second.enbAddr;
  ueit->second->SetEnbAddr (enbAddr);
  // no actual bearer modification: for now we just support the minimum needed for path switch request (handover)
  NistEpcS11SapMme::NistModifyBearerResponseMessage res;
  res.teid = imsi; // trick to avoid the need for allocating TEIDs on the S11 interface
  res.cause = NistEpcS11SapMme::NistModifyBearerResponseMessage::REQUEST_ACCEPTED;
  m_s11SapMme->ModifyBearerResponse (res);
}
 
void
NistEpcSgwPgwApplication::DoDeleteBearerCommand (NistEpcS11SapSgw::NistDeleteBearerCommandMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi);

  NistEpcS11SapMme::NistDeleteBearerRequestMessage res;
  res.teid = imsi;

  for (std::list<NistEpcS11SapSgw::NistBearerContextToBeRemoved>::iterator bit = req.bearerContextsToBeRemoved.begin ();
       bit != req.bearerContextsToBeRemoved.end ();
       ++bit)
    {
      NistEpcS11SapMme::NistBearerContextRemoved bearerContext;
      bearerContext.epsBearerId =  bit->epsBearerId;
      res.bearerContextsRemoved.push_back (bearerContext);
    }
  //schedules Delete Bearer Request towards MME
  m_s11SapMme->DeleteBearerRequest (res);
}

void
NistEpcSgwPgwApplication::DoDeleteBearerResponse (NistEpcS11SapSgw::NistDeleteBearerResponseMessage req)
{
  NS_LOG_FUNCTION (this << req.teid);
  uint64_t imsi = req.teid; // trick to avoid the need for allocating TEIDs on the S11 interface
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator ueit = m_ueInfoByImsiMap.find (imsi);
  NS_ASSERT_MSG (ueit != m_ueInfoByImsiMap.end (), "unknown IMSI " << imsi);

  for (std::list<NistEpcS11SapSgw::NistBearerContextRemovedSgwPgw>::iterator bit = req.bearerContextsRemoved.begin ();
       bit != req.bearerContextsRemoved.end ();
       ++bit)
    {
      //Function to remove de-activated bearer contexts from S-Gw and P-Gw side
      ueit->second->RemoveBearer (bit->epsBearerId);
    }
}

}  // namespace ns3
