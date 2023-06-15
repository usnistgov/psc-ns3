/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
#include "sfnsf.h"
#include <math.h>
#include "ns3/abort.h"

namespace ns3 {

SfnSf::SfnSf (uint32_t frameNum, uint8_t sfNum, uint8_t slotNum, uint8_t numerology)
  : m_frameNum (frameNum),
    m_subframeNum (sfNum),
    m_slotNum (slotNum),
    m_numerology (numerology)
{
  // Numerology > 5 unsupported; if you want to define a new one,
  // relax this constraint
  NS_ABORT_MSG_IF (numerology > 5, "Numerology > 5 unsupported");
}

uint64_t
SfnSf::GetEncoding () const
{
  NS_ASSERT (m_numerology <= 5);
  uint64_t ret = 0ULL;
  ret =
    (static_cast<uint64_t> (m_frameNum) << 32) |
    (static_cast<uint64_t> (m_subframeNum) << 24) |
    (static_cast<uint64_t> (m_slotNum) << 16) |
    (static_cast<uint64_t> (m_numerology) << 8);
  return ret;
}

uint64_t
SfnSf::GetEncodingWithSymStart (uint8_t symStart) const
{
  NS_ASSERT (m_numerology <= 5);
  uint64_t ret = 0ULL;
  ret =
    (static_cast<uint64_t> (m_frameNum) << 32) |
    (static_cast<uint64_t> (m_subframeNum) << 24) |
    (static_cast<uint64_t> (m_slotNum) << 16) |
    (static_cast<uint64_t> (m_numerology) << 8) |
    (static_cast<uint64_t> (symStart));
  return ret;
}

uint64_t
SfnSf::GetEncForStreamWithSymStart (uint8_t streamId, uint8_t symStart) const
{
  NS_ASSERT (m_numerology >= 0);
  uint64_t ret = 0ULL;
  ret =
    (static_cast<uint64_t> (m_numerology) << 56) |
    (static_cast<uint64_t> (m_frameNum) << 40 ) |
    (static_cast<uint64_t> (m_subframeNum) << 32) |
    (static_cast<uint64_t> (m_slotNum) << 16) |
    (static_cast<uint64_t> (symStart) << 8) |
    (static_cast<uint64_t> (streamId));
  return ret;
}

void
SfnSf::FromEncoding (uint64_t sfn)
{
  m_frameNum    =     (sfn & 0xFFFFFFFF00000000) >> 32;
  m_subframeNum =     (sfn & 0x00000000FF000000) >> 24;
  m_slotNum     =     (sfn & 0x0000000000FF0000) >> 16;
  m_numerology =      (sfn & 0x000000000000FF00) >> 8;
}

// Static functions
uint32_t
SfnSf::GetSubframesPerFrame ()
{
  return 10;
}

uint32_t
SfnSf::GetSlotPerSubframe () const
{
  return 1 << m_numerology;
}

uint64_t
SfnSf::Encode (const SfnSf &p)
{
  return p.GetEncoding ();
}

SfnSf
SfnSf::Decode (uint64_t sfn)
{
  SfnSf ret;
  ret.FromEncoding (sfn);
  return ret;
}

uint64_t
SfnSf::Normalize () const
{
  uint64_t ret = 0;
  ret += m_slotNum;
  ret += m_subframeNum * GetSlotPerSubframe ();
  ret += m_frameNum * GetSubframesPerFrame () * GetSlotPerSubframe ();
  return ret;
}

SfnSf
SfnSf::GetFutureSfnSf (uint32_t slotN) const
{
  SfnSf ret = *this;
  ret.Add (slotN);
  return ret;
}

void
SfnSf::Add (uint32_t slotN)
{
  NS_ASSERT_MSG (m_numerology <= 5, "Numerology " << m_numerology << " invalid");
  m_frameNum += (m_subframeNum + (m_slotNum + slotN) / GetSlotPerSubframe ()) / GetSubframesPerFrame ();
  m_subframeNum = (m_subframeNum + (m_slotNum + slotN) / GetSlotPerSubframe ()) % GetSubframesPerFrame ();
  m_slotNum = (m_slotNum + slotN) % GetSlotPerSubframe ();
}

bool
SfnSf::operator < (const SfnSf &rhs) const
{
  NS_ASSERT_MSG (rhs.m_numerology == m_numerology, "Numerology does not match");
  if (m_frameNum < rhs.m_frameNum)
    {
      return true;
    }
  else if ((m_frameNum == rhs.m_frameNum ) && (m_subframeNum < rhs.m_subframeNum))
    {
      return true;
    }
  else if (((m_frameNum == rhs.m_frameNum ) && (m_subframeNum == rhs.m_subframeNum)) && (m_slotNum < rhs.m_slotNum))
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool
SfnSf::operator == (const SfnSf &o) const
{
  NS_ASSERT_MSG (o.m_numerology == m_numerology, "Numerology does not match");
  return (m_frameNum == o.m_frameNum) && (m_subframeNum == o.m_subframeNum)
         && (m_slotNum == o.m_slotNum);
}

uint32_t
SfnSf::GetFrame () const
{
  return m_frameNum;
}

uint8_t
SfnSf::GetSubframe () const
{
  return m_subframeNum;
}

uint8_t
SfnSf::GetSlot () const
{
  return m_slotNum;
}

uint8_t
SfnSf::GetNumerology () const
{
  NS_ASSERT_MSG (m_numerology <= 5, "Numerology " << m_numerology << " invalid");
  return m_numerology;
}

} // namespace ns3
