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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/nist-lte-rlc-sdu-status-tag.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (NistLteRlcSduNistStatusTag);

NistLteRlcSduNistStatusTag::NistLteRlcSduNistStatusTag ()
{
}

void
NistLteRlcSduNistStatusTag::SetNistStatus (uint8_t status)
{
  m_sduNistStatus = status;
}

uint8_t
NistLteRlcSduNistStatusTag::GetNistStatus (void) const
{
  return m_sduNistStatus;
}

TypeId
NistLteRlcSduNistStatusTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteRlcSduNistStatusTag")
    .SetParent<Tag> ()
    .AddConstructor<NistLteRlcSduNistStatusTag> ()
  ;
  return tid;
}
TypeId
NistLteRlcSduNistStatusTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NistLteRlcSduNistStatusTag::GetSerializedSize (void) const
{
  return 1;
}
void
NistLteRlcSduNistStatusTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_sduNistStatus);
}
void
NistLteRlcSduNistStatusTag::Deserialize (TagBuffer i)
{
  m_sduNistStatus = i.ReadU8 ();
}
void
NistLteRlcSduNistStatusTag::Print (std::ostream &os) const
{
  os << "SDU NistStatus=" << (uint32_t) m_sduNistStatus;
}

}; // namespace ns3
