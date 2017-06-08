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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */


#include "nist-lte-phy-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (NistLtePhyTag);

TypeId
NistLtePhyTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLtePhyTag")
    .SetParent<Tag> ()
    .AddConstructor<NistLtePhyTag> ()
  ;
  return tid;
}

TypeId
NistLtePhyTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

NistLtePhyTag::NistLtePhyTag ()
{
}

NistLtePhyTag::NistLtePhyTag (uint16_t cellId)
  : m_cellId (cellId)
{
}

NistLtePhyTag::~NistLtePhyTag ()
{
}

uint32_t
NistLtePhyTag::GetSerializedSize (void) const
{
  return 2;
}

void
NistLtePhyTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_cellId);
}

void
NistLtePhyTag::Deserialize (TagBuffer i)
{
  m_cellId = i.ReadU16 ();
}

void
NistLtePhyTag::Print (std::ostream &os) const
{
  os << m_cellId;
}

uint16_t
NistLtePhyTag::GetCellId () const
{
  return m_cellId;
}

} // namespace ns3
