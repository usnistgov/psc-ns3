/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 * Author: Marco Miozzo <mmiozzo@cttc.es> : Update to FF API Architecture
 * Author: Nicola Baldo <nbaldo@cttc.es>  : Integrated with new RRC and MAC architecture
 */

#include <ns3/llc-snap-header.h>
#include <ns3/simulator.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/nist-lte-net-device.h>
#include <ns3/packet-burst.h>
#include <ns3/uinteger.h>
#include <ns3/trace-source-accessor.h>
#include <ns3/pointer.h>
#include <ns3/enum.h>
#include <ns3/nist-lte-amc.h>
#include <ns3/nist-lte-enb-mac.h>
#include <ns3/nist-lte-enb-net-device.h>
#include <ns3/nist-lte-enb-rrc.h>
#include <ns3/nist-lte-ue-net-device.h>
#include <ns3/nist-lte-enb-phy.h>
#include <ns3/nist-ff-mac-scheduler.h>
#include <ns3/nist-lte-handover-algorithm.h>
#include <ns3/nist-lte-anr.h>
#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/abort.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteEnbNetDevice");

NS_OBJECT_ENSURE_REGISTERED ( NistLteEnbNetDevice);

TypeId NistLteEnbNetDevice::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NistLteEnbNetDevice")
    .SetParent<NistLteNetDevice> ()
    .AddConstructor<NistLteEnbNetDevice> ()
    .AddAttribute ("NistLteEnbRrc",
                   "The RRC associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_rrc),
                   MakePointerChecker <NistLteEnbRrc> ())
    .AddAttribute ("NistLteHandoverAlgorithm",
                   "The handover algorithm associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_handoverAlgorithm),
                   MakePointerChecker <NistLteHandoverAlgorithm> ())
    .AddAttribute ("NistLteAnr",
                   "The automatic neighbour relation function associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_anr),
                   MakePointerChecker <NistLteAnr> ())
    .AddAttribute ("NistLteFfrAlgorithm",
                   "The FFR algorithm associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_ffrAlgorithm),
                   MakePointerChecker <NistLteFfrAlgorithm> ())
    .AddAttribute ("NistLteEnbMac",
                   "The MAC associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_mac),
                   MakePointerChecker <NistLteEnbMac> ())
    .AddAttribute ("NistFfMacScheduler",
                   "The scheduler associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_scheduler),
                   MakePointerChecker <NistFfMacScheduler> ())
    .AddAttribute ("NistLteEnbPhy",
                   "The PHY associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbNetDevice::m_phy),
                   MakePointerChecker <NistLteEnbPhy> ())
    .AddAttribute ("UlBandwidth",
                   "Uplink Transmission Bandwidth Configuration in number of Resource Blocks",
                   UintegerValue (25),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::SetUlBandwidth, 
                                         &NistLteEnbNetDevice::GetUlBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlBandwidth",
                   "Downlink Transmission Bandwidth Configuration in number of Resource Blocks",
                   UintegerValue (25),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::SetDlBandwidth, 
                                         &NistLteEnbNetDevice::GetDlBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("CellId",
                   "Cell Identifier",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::m_cellId),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DlEarfcn",
                   "Downlink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) "
                   "as per 3GPP 36.101 Section 5.7.3. ",
                   UintegerValue (100),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::m_dlEarfcn),
                   MakeUintegerChecker<uint16_t> (0, 6599))
    .AddAttribute ("UlEarfcn",
                   "Uplink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) "
                   "as per 3GPP 36.101 Section 5.7.3. ",
                   UintegerValue (18100),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::m_ulEarfcn),
                   MakeUintegerChecker<uint16_t> (18000, 24599))
    .AddAttribute ("CsgId",
                   "The Closed Subscriber Group (CSG) identity that this eNodeB belongs to",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteEnbNetDevice::SetCsgId,
                                         &NistLteEnbNetDevice::GetCsgId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CsgIndication",
                   "If true, only UEs which are members of the CSG (i.e. same CSG ID) "
                   "can gain access to the eNodeB, therefore enforcing closed access mode. "
                   "Otherwise, the eNodeB operates as a non-CSG cell and implements open access mode.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&NistLteEnbNetDevice::SetCsgIndication,
                                        &NistLteEnbNetDevice::GetCsgIndication),
                   MakeBooleanChecker ())
  ;
  return tid;
}

NistLteEnbNetDevice::NistLteEnbNetDevice ()
  : m_isConstructed (false),
    m_isConfigured (false),
    m_anr (0)
{
  NS_LOG_FUNCTION (this);
}

NistLteEnbNetDevice::~NistLteEnbNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  m_mac->Dispose ();
  m_mac = 0;

  m_scheduler->Dispose ();
  m_scheduler = 0;

  m_rrc->Dispose ();
  m_rrc = 0;

  m_handoverAlgorithm->Dispose ();
  m_handoverAlgorithm = 0;

  if (m_anr != 0)
    {
      m_anr->Dispose ();
      m_anr = 0;
    }

  m_ffrAlgorithm->Dispose ();
  m_ffrAlgorithm = 0;

  m_phy->Dispose ();
  m_phy = 0;

  NistLteNetDevice::DoDispose ();
}



Ptr<NistLteEnbMac>
NistLteEnbNetDevice::GetMac () const
{
  return m_mac;
}

Ptr<NistLteEnbPhy>
NistLteEnbNetDevice::GetPhy () const
{
  return m_phy;
}

Ptr<NistLteEnbRrc>
NistLteEnbNetDevice::GetRrc () const
{
  return m_rrc;
}

uint16_t
NistLteEnbNetDevice::GetCellId () const
{
  return m_cellId;
}

uint8_t 
NistLteEnbNetDevice::GetUlBandwidth () const
{
  return m_ulBandwidth;
}

void 
NistLteEnbNetDevice::SetUlBandwidth (uint8_t bw)
{ 
  NS_LOG_FUNCTION (this << uint16_t (bw));
  switch (bw)
    { 
    case 6:
    case 15:
    case 25:
    case 50:
    case 75:
    case 100:
      m_ulBandwidth = bw;
      break;

    default:
      NS_FATAL_ERROR ("invalid bandwidth value " << (uint16_t) bw);
      break;
    }
}

uint8_t 
NistLteEnbNetDevice::GetDlBandwidth () const
{
  return m_dlBandwidth;
}

void 
NistLteEnbNetDevice::SetDlBandwidth (uint8_t bw)
{
  NS_LOG_FUNCTION (this << uint16_t (bw));
  switch (bw)
    { 
    case 6:
    case 15:
    case 25:
    case 50:
    case 75:
    case 100:
      m_dlBandwidth = bw;
      break;

    default:
      NS_FATAL_ERROR ("invalid bandwidth value " << (uint16_t) bw);
      break;
    }
}

uint16_t 
NistLteEnbNetDevice::GetDlEarfcn () const
{
  return m_dlEarfcn;
}

void 
NistLteEnbNetDevice::SetDlEarfcn (uint16_t earfcn)
{ 
  NS_LOG_FUNCTION (this << earfcn);
  m_dlEarfcn = earfcn;
}

uint16_t 
NistLteEnbNetDevice::GetUlEarfcn () const
{
  return m_ulEarfcn;
}

void 
NistLteEnbNetDevice::SetUlEarfcn (uint16_t earfcn)
{ 
  NS_LOG_FUNCTION (this << earfcn);
  m_ulEarfcn = earfcn;
}

uint32_t
NistLteEnbNetDevice::GetCsgId () const
{
  return m_csgId;
}

void
NistLteEnbNetDevice::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  UpdateConfig (); // propagate the change to RRC level
}

bool
NistLteEnbNetDevice::GetCsgIndication () const
{
  return m_csgIndication;
}

void
NistLteEnbNetDevice::SetCsgIndication (bool csgIndication)
{
  NS_LOG_FUNCTION (this << csgIndication);
  m_csgIndication = csgIndication;
  UpdateConfig (); // propagate the change to RRC level
}


void 
NistLteEnbNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();
  m_phy->Initialize ();
  m_mac->Initialize ();
  m_rrc->Initialize ();
  m_handoverAlgorithm->Initialize ();

  if (m_anr != 0)
    {
      m_anr->Initialize ();
    }

  m_ffrAlgorithm->Initialize ();
}


bool
NistLteEnbNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet   << dest << protocolNumber);
  NS_ASSERT_MSG (protocolNumber == Ipv4L3Protocol::PROT_NUMBER, "unsupported protocol " << protocolNumber << ", only IPv4 is supported");
  return m_rrc->SendData (packet);
}


void
NistLteEnbNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      if (!m_isConfigured)
        {
          NS_LOG_LOGIC (this << " Configure cell " << m_cellId);
          // we have to make sure that this function is called only once
          m_rrc->ConfigureCell (m_ulBandwidth, m_dlBandwidth, m_ulEarfcn, m_dlEarfcn, m_cellId);
          m_isConfigured = true;
        }

      NS_LOG_LOGIC (this << " Updating SIB1 of cell " << m_cellId
                         << " with CSG ID " << m_csgId
                         << " and CSG indication " << m_csgIndication);
      m_rrc->SetCsgId (m_csgId, m_csgIndication);
    }
  else
    {
      /*
       * Lower layers are not ready yet, so do nothing now and expect
       * ``DoInitialize`` to re-invoke this function.
       */
    }
}


} // namespace ns3
