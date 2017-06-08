/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 *         Nicola Baldo <nbaldo@cttc.es>
 *         Marco Miozzo <mmiozzo@cttc.es>
 */

#include "ns3/llc-snap-header.h"
#include "ns3/simulator.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "nist-lte-net-device.h"
#include "ns3/packet-burst.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/enum.h"
#include "ns3/nist-lte-enb-net-device.h"
#include "nist-lte-ue-net-device.h"
#include "nist-lte-ue-mac.h"
#include "nist-lte-ue-rrc.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4.h"
#include "nist-lte-amc.h"
#include "nist-lte-ue-phy.h"
#include "nist-epc-ue-nas.h"
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/log.h>
#include "nist-epc-tft.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteUeNetDevice");

NS_OBJECT_ENSURE_REGISTERED ( NistLteUeNetDevice);


TypeId NistLteUeNetDevice::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NistLteUeNetDevice")
    .SetParent<NistLteNetDevice> ()
    .AddConstructor<NistLteUeNetDevice> ()
    .AddAttribute ("NistEpcUeNas",
                   "The NAS associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteUeNetDevice::m_nas),
                   MakePointerChecker <NistEpcUeNas> ())
    .AddAttribute ("NistLteUeRrc",
                   "The RRC associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteUeNetDevice::m_rrc),
                   MakePointerChecker <NistLteUeRrc> ())
    .AddAttribute ("NistLteUeMac",
                   "The MAC associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteUeNetDevice::m_mac),
                   MakePointerChecker <NistLteUeMac> ())
    .AddAttribute ("NistLteUePhy",
                   "The PHY associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&NistLteUeNetDevice::m_phy),
                   MakePointerChecker <NistLteUePhy> ())
    .AddAttribute ("Imsi",
                   "International Mobile Subscriber Identity assigned to this UE",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteUeNetDevice::m_imsi),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("DlEarfcn",
                   "Downlink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) "
                   "as per 3GPP 36.101 Section 5.7.3. ",
                   UintegerValue (100),
                   MakeUintegerAccessor (&NistLteUeNetDevice::SetDlEarfcn,
                                         &NistLteUeNetDevice::GetDlEarfcn),
                   MakeUintegerChecker<uint16_t> (0, 6149))
    .AddAttribute ("CsgId",
                   "The Closed Subscriber Group (CSG) identity that this UE is associated with, "
                   "i.e., giving the UE access to cells which belong to this particular CSG. "
                   "This restriction only applies to initial cell selection and EPC-enabled simulation. "
                   "This does not revoke the UE's access to non-CSG cells. ",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteUeNetDevice::SetCsgId,
                                         &NistLteUeNetDevice::GetCsgId),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}


NistLteUeNetDevice::NistLteUeNetDevice (void)
  : m_isConstructed (false)
{
  NS_LOG_FUNCTION (this);
}

NistLteUeNetDevice::~NistLteUeNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteUeNetDevice::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_targetEnb = 0;
  m_mac->Dispose ();
  m_mac = 0;
  m_rrc->Dispose ();
  m_rrc = 0;
  m_phy->Dispose ();
  m_phy = 0;
  m_nas->Dispose ();
  m_nas = 0;
  NistLteNetDevice::DoDispose ();
}

void
NistLteUeNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      NS_LOG_LOGIC (this << " Updating configuration: IMSI " << m_imsi
                         << " CSG ID " << m_csgId);
      m_nas->SetImsi (m_imsi);
      m_rrc->SetImsi (m_imsi);
      m_nas->SetCsgId (m_csgId); // this also handles propagation to RRC
    }
  else
    {
      /*
       * NAS and RRC instances are not be ready yet, so do nothing now and
       * expect ``DoInitialize`` to re-invoke this function.
       */
    }
}



Ptr<NistLteUeMac>
NistLteUeNetDevice::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}


Ptr<NistLteUeRrc>
NistLteUeNetDevice::GetRrc (void) const
{
  NS_LOG_FUNCTION (this);
  return m_rrc;
}


Ptr<NistLteUePhy>
NistLteUeNetDevice::GetPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

Ptr<NistEpcUeNas>
NistLteUeNetDevice::GetNas (void) const
{
  NS_LOG_FUNCTION (this);
  return m_nas;
}

uint64_t
NistLteUeNetDevice::GetImsi () const
{
  NS_LOG_FUNCTION (this);
  return m_imsi;
}

uint16_t
NistLteUeNetDevice::GetDlEarfcn () const
{
  NS_LOG_FUNCTION (this);
  return m_dlEarfcn;
}

void
NistLteUeNetDevice::SetDlEarfcn (uint16_t earfcn)
{
  NS_LOG_FUNCTION (this << earfcn);
  m_dlEarfcn = earfcn;
}

uint32_t
NistLteUeNetDevice::GetCsgId () const
{
  NS_LOG_FUNCTION (this);
  return m_csgId;
}

void
NistLteUeNetDevice::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  UpdateConfig (); // propagate the change down to NAS and RRC
}

void
NistLteUeNetDevice::SetTargetEnb (Ptr<NistLteEnbNetDevice> enb)
{
  NS_LOG_FUNCTION (this << enb);
  m_targetEnb = enb;
}


Ptr<NistLteEnbNetDevice>
NistLteUeNetDevice::GetTargetEnb (void)
{
  NS_LOG_FUNCTION (this);
  return m_targetEnb;
}

void 
NistLteUeNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();
  m_phy->Initialize ();
  m_mac->Initialize ();
  m_rrc->Initialize ();
}

bool
NistLteUeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << dest << protocolNumber);
  if (protocolNumber != Ipv4L3Protocol::PROT_NUMBER)
    {
      NS_LOG_INFO("unsupported protocol " << protocolNumber << ", only IPv4 is supported");
      return true;
    }  
  return m_nas->Send (packet);
}


} // namespace ns3
