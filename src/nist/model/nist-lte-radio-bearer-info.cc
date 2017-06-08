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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by: NIST
 */

#include "nist-lte-radio-bearer-info.h"
#include "nist-lte-ue-rrc.h"
#include "nist-lte-rlc.h"
#include "nist-lte-pdcp.h"

#include <ns3/log.h>



namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (NistLteRadioBearerInfo);

NistLteRadioBearerInfo::NistLteRadioBearerInfo (void)
{
}

NistLteRadioBearerInfo::~NistLteRadioBearerInfo (void)
{
}
  
TypeId 
NistLteRadioBearerInfo::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NistLteRadioBearerInfo")
    .SetParent<Object> ()
    .AddConstructor<NistLteRadioBearerInfo> ()
    ;
  return tid;
}
  
  
TypeId 
LteDataRadioNistBearerInfo::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::LteDataRadioNistBearerInfo")
    .SetParent<NistLteRadioBearerInfo> ()
    .AddConstructor<LteDataRadioNistBearerInfo> ()
    .AddAttribute ("DrbIdentity", "The id of this Data Radio Bearer",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteDataRadioNistBearerInfo::m_drbIdentity),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("NistEpsBearerIdentity", "The id of the EPS bearer corresponding to this Data Radio Bearer",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteDataRadioNistBearerInfo::m_epsBearerIdentity),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("logicalChannelIdentity", "The id of the Logical Channel corresponding to this Data Radio Bearer",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteDataRadioNistBearerInfo::m_logicalChannelIdentity),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("NistLteRlc", "RLC instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_rlc),
                   MakePointerChecker<NistLteRlc> ())
    .AddAttribute ("NistLtePdcp", "PDCP instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_pdcp),
                   MakePointerChecker<NistLtePdcp> ())
    ;
  return tid;
}


TypeId 
LteSignalingRadioNistBearerInfo::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::LteSignalingRadioNistBearerInfo")
    .SetParent<NistLteRadioBearerInfo> ()
    .AddConstructor<LteSignalingRadioNistBearerInfo> ()
    .AddAttribute ("SrbIdentity", "The id of this Signaling Radio Bearer",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteSignalingRadioNistBearerInfo::m_srbIdentity),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("NistLteRlc", "RLC instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_rlc),
                   MakePointerChecker<NistLteRlc> ())
    .AddAttribute ("NistLtePdcp", "PDCP instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_pdcp),
                   MakePointerChecker<NistLtePdcp> ())
    ;
  return tid;
}

TypeId 
LteSidelinkRadioBearerInfo::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::LteSidelinkRadioBearerInfo")
    .SetParent<NistLteRadioBearerInfo> ()
    .AddConstructor<LteSidelinkRadioBearerInfo> ()
    .AddAttribute ("DestinationL2Id", "The destination identifier for the communication",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteSidelinkRadioBearerInfo::m_destinationL2Id),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("logicalChannelIdentity", "The id of the Logical Channel corresponding to this Data Radio Bearer",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0), // unused (attribute is read-only
                   MakeUintegerAccessor (&LteSidelinkRadioBearerInfo::m_logicalChannelIdentity),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("NistLteRlc", "RLC instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_rlc),
                   MakePointerChecker<NistLteRlc> ())
    .AddAttribute ("NistLtePdcp", "PDCP instance of the radio bearer.",
                   PointerValue (),
                   MakePointerAccessor (&NistLteRadioBearerInfo::m_pdcp),
                   MakePointerChecker<NistLtePdcp> ())
    // .AddAttribute ("Tx", "If true, indicates that the UE is interesting in transmitting to the group.",
    //                TypeId::ATTR_GET, // allow only getting it.
    //                BooleanValue (false),
    //                MakeBooleanAccessor (&LteSidelinkRadioBearerInfo::m_tx),
    //                MakeBooleanChecker ())
    // .AddAttribute ("Rx", "If true, indicates that the UE is interesting in receiving from the group.",
    //                TypeId::ATTR_GET, // allow only getting it.
    //                BooleanValue (false),
    //                MakeBooleanAccessor (&LteSidelinkRadioBearerInfo::m_rx),
    //                MakeBooleanChecker ())
    ;
  return tid;
}

} // namespace ns3
