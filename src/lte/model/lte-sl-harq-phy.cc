/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 CTTC
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
 * This class, authored by NIST, is derived from lte-harq-phy.cc
 * originally authored by Marco Miozzo  <marco.miozzo@cttc.es>.
 */

#include <ns3/lte-sl-harq-phy.h>
#include <ns3/log.h>
#include <ns3/assert.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlHarqPhy");

//NS_OBJECT_ENSURE_REGISTERED (LteSlHarqPhy)
//  ;


LteSlHarqPhy::LteSlHarqPhy (void)
{
}

LteSlHarqPhy::~LteSlHarqPhy ()
{
  m_miSlHarqProcessesInfoMap.clear ();
}

uint32_t
LteSlHarqPhy::GetTbIdx (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t idx = 0;
  uint32_t tbIdxId = (rnti << 8) + l1dst;
  std::map<uint32_t, uint32_t>::iterator it = this->m_tbIdx.find (tbIdxId);

  if (it != this->m_tbIdx.end ())
    {
      idx = it->second;
    }

  return idx;
}

void
LteSlHarqPhy::IncreaseTbIdx (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t tbIdxId = (rnti << 8) + l1dst;

  if (this->m_tbIdx.find (tbIdxId) == this->m_tbIdx.end ())
    {
      this->m_tbIdx[tbIdxId] = 0;
    }
  else
    {
      this->m_tbIdx[tbIdxId] = this->m_tbIdx[tbIdxId] + 1;
    }
}

void
LteSlHarqPhy::IndicatePrevDecoded (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t decodedId = (rnti << 8) + l1dst;

  if (this->m_decoded.find (decodedId) == this->m_decoded.end ())
    {
      this->m_decoded.insert (decodedId);
    }
}

bool
LteSlHarqPhy::IsPrevDecoded (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t decodedId = (rnti << 8) + l1dst;

  std::set<uint32_t>::iterator it = this->m_decoded.find (decodedId);

  bool prevDecoded = (it != this->m_decoded.end ());

  return prevDecoded;
}

void LteSlHarqPhy::ResetPrevDecoded (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t decodedId = (rnti << 8) + l1dst;

  std::set<uint32_t>::iterator it = this->m_decoded.find (decodedId);

  if (it != this->m_decoded.end ())
    {
      this->m_decoded.erase (it);
    }
}

void
LteSlHarqPhy::IndicateDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)resPsdch);

  uint32_t decodedId = (rnti << 8) + resPsdch;

  if (this->m_discDecoded.find (decodedId) == this->m_discDecoded.end ())
    {
      this->m_discDecoded.insert (decodedId);
    }
}

bool
LteSlHarqPhy::IsDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)resPsdch);

  uint32_t decodedId = (rnti << 8) + resPsdch;

  std::set<uint32_t>::iterator it = this->m_discDecoded.find (decodedId);

  bool prevDecoded = (it != this->m_discDecoded.end ());

  return prevDecoded;
}

void LteSlHarqPhy::ResetDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)resPsdch);

  uint32_t decodedId = (rnti << 8) + resPsdch;

  std::set<uint32_t>::iterator it = this->m_discDecoded.find (decodedId);

  if (it != this->m_discDecoded.end ())
    {
      this->m_discDecoded.erase (it);
    }
}

void
LteSlHarqPhy::ResetTbIdx (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << (uint32_t)rnti << (uint32_t)l1dst);

  uint32_t tbIdxId = (rnti << 8) + l1dst;
  std::map<uint32_t, uint32_t>::iterator it = this->m_tbIdx.find (tbIdxId);

  if (it != this->m_tbIdx.end ())
    {
      this->m_tbIdx.erase (it);
    }
}

double
LteSlHarqPhy::GetAccumulatedMiSl (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << rnti);

  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  it = m_miSlHarqProcessesInfoMap.find (rnti);
  NS_ASSERT_MSG (it != m_miSlHarqProcessesInfoMap.end (), " Does not find MI for RNTI");
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;
  it2 = it->second.find (l1dst);
  NS_ASSERT_MSG (it2 != it->second.end (), " Does not find MI for l1dst ");
  HarqProcessInfoList_t list = (*it2).second;
  double mi = 0.0;
  for (uint8_t i = 0; i < list.size (); i++)
    {
      mi += list.at (i).m_mi;
    }
  return (mi);
}

HarqProcessInfoList_t
LteSlHarqPhy::GetHarqProcessInfoSl (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t)l1dst);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  it = m_miSlHarqProcessesInfoMap.find (rnti);
  if (it == m_miSlHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
      m_miSlHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
      return (harqInfo);
    }
  else
    {
      //entry here, check ls1dst
      std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;
      it2 = it->second.find (l1dst);
      if (it2 == it->second.end ())
        {
          HarqProcessInfoList_t harqInfo;
          std::map <uint8_t, HarqProcessInfoList_t> map;
          it->second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
          return (harqInfo);
        }
      else
        {
          return ((*it2).second);
        }
    }
}

HarqProcessInfoList_t
LteSlHarqPhy::GetHarqProcessInfoDisc (uint16_t rnti, uint8_t resPsdch)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t)resPsdch);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  it = m_miDiscHarqProcessesInfoMap.find (rnti);
  if (it == m_miDiscHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
      m_miDiscHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
      return (harqInfo);
    }
  else
    {
      //entry here, check ls1dst
      std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;
      it2 = it->second.find (resPsdch);
      if (it2 == it->second.end ())
        {
          HarqProcessInfoList_t harqInfo;
          std::map <uint8_t, HarqProcessInfoList_t> map;
          it->second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
          return (harqInfo);
        }
      else
        {
          return ((*it2).second);
        }
    }
}

void
LteSlHarqPhy::UpdateSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst, double mi, uint16_t infoBytes, uint16_t codeBytes)
{
  NS_LOG_FUNCTION (this << rnti << mi);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;

  it = m_miSlHarqProcessesInfoMap.find (rnti);
  if (it == m_miSlHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      HarqProcessInfoElement_t el;
      el.m_mi = mi;
      el.m_infoBits = infoBytes * 8;
      el.m_codeBits = codeBytes * 8;
      harqInfo.push_back (el);
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
      m_miSlHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
    }
  else
    {
      it2 = it->second.find (l1dst);
      if (it2 == it->second.end ())
        {
          // new entry
          HarqProcessInfoList_t harqInfo;
          HarqProcessInfoElement_t el;
          el.m_mi = mi;
          el.m_infoBits = infoBytes * 8;
          el.m_codeBits = codeBytes * 8;
          harqInfo.push_back (el);
          (*it).second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
        }
      else
        {
          if ((*it2).second.size () == 3) // MAX HARQ RETX
            {
              NS_LOG_DEBUG ("numRetx = " << (*it2).second.size () << " discard info");
              // HARQ should be disabled -> discard info
              return;
            }
          HarqProcessInfoElement_t el;
          el.m_mi = mi;
          el.m_infoBits = infoBytes * 8;
          el.m_codeBits = codeBytes * 8;
          (*it2).second.push_back (el);
        }
    }
}

void
LteSlHarqPhy::UpdateSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst, double sinr)
{
  NS_LOG_FUNCTION (this << rnti << sinr);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;

  it = m_miSlHarqProcessesInfoMap.find (rnti);
  if (it == m_miSlHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      HarqProcessInfoElement_t el;
      el.m_mi = 0;
      el.m_infoBits = 0;
      el.m_codeBits = 0;
      el.m_sinr = sinr;
      harqInfo.push_back (el);
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
      m_miSlHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
    }
  else
    {
      it2 = it->second.find (l1dst);
      if (it2 == it->second.end ())
        {
          // new entry
          HarqProcessInfoList_t harqInfo;
          HarqProcessInfoElement_t el;
          el.m_mi = 0;
          el.m_infoBits = 0;
          el.m_codeBits = 0;
          el.m_sinr = sinr;
          harqInfo.push_back (el);
          (*it).second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
        }
      else
        {
          if ((*it2).second.size () == 3) // MAX HARQ RETX
            {
              NS_LOG_DEBUG ("numRetx = " << (*it2).second.size () << " discard info");
              // HARQ should be disabled -> discard info
              return;
            }
          HarqProcessInfoElement_t el;
          el.m_mi = 0;
          el.m_infoBits = 0;
          el.m_codeBits = 0;
          el.m_sinr = sinr;
          (*it2).second.push_back (el);
        }
    }
}

void
LteSlHarqPhy::UpdateDiscHarqProcessStatus (uint16_t rnti, uint8_t resPsdch, double sinr )
{
  NS_LOG_FUNCTION (this << rnti << sinr);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;

  it = m_miDiscHarqProcessesInfoMap.find (rnti);
  if (it == m_miDiscHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      HarqProcessInfoElement_t el;
      el.m_mi = 0;
      el.m_infoBits = 0;
      el.m_codeBits = 0;
      el.m_sinr = sinr;
      harqInfo.push_back (el);
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
      m_miDiscHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
    }
  else
    {
      it2 = it->second.find (resPsdch);
      if (it2 == it->second.end ())
        {
          // new entry
          HarqProcessInfoList_t harqInfo;
          HarqProcessInfoElement_t el;
          el.m_mi = 0;
          el.m_infoBits = 0;
          el.m_codeBits = 0;
          el.m_sinr = sinr;
          harqInfo.push_back (el);
          (*it).second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
        }
      else
        {
          if ((*it2).second.size () == m_discNumRetx) // MAX HARQ RETX
            {
              NS_LOG_DEBUG ("numRetx = " << (uint16_t)m_discNumRetx << " discard info");
              // HARQ should be disabled -> discard info
              return;
            }
          HarqProcessInfoElement_t el;
          el.m_mi = 0;
          el.m_infoBits = 0;
          el.m_codeBits = 0;
          el.m_sinr = sinr;
          (*it2).second.push_back (el);
        }
    }
}


void
LteSlHarqPhy::ResetSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t)l1dst);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;
  it = m_miSlHarqProcessesInfoMap.find (rnti);
  if (it == m_miSlHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
      m_miSlHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
    }
  else
    {
      it2 = (*it).second.find (l1dst);
      if (it2 == (*it).second.end ())
        {
          HarqProcessInfoList_t harqInfo;
          std::map <uint8_t, HarqProcessInfoList_t> map;
          it->second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (l1dst, harqInfo));
        }
      else
        {
          (*it2).second.clear ();
        }
    }
}

void
LteSlHarqPhy::ResetDiscHarqProcessStatus (uint16_t rnti, uint8_t resPsdch)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t)resPsdch);
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> >::iterator it;
  std::map <uint8_t, HarqProcessInfoList_t>::iterator it2;
  it = m_miDiscHarqProcessesInfoMap.find (rnti);
  if (it == m_miDiscHarqProcessesInfoMap.end ())
    {
      // new entry
      HarqProcessInfoList_t harqInfo;
      std::map <uint8_t, HarqProcessInfoList_t> map;
      map.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
      m_miDiscHarqProcessesInfoMap.insert (std::pair <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > (rnti, map));
    }
  else
    {
      it2 = (*it).second.find (resPsdch);
      if (it2 == (*it).second.end ())
        {
          HarqProcessInfoList_t harqInfo;
          std::map <uint8_t, HarqProcessInfoList_t> map;
          it->second.insert (std::pair <uint8_t, HarqProcessInfoList_t> (resPsdch, harqInfo));
        }
      else
        {
          (*it2).second.clear ();
        }
    }
}


void
LteSlHarqPhy::SetDiscNumRetx (uint8_t retx)
{
  NS_LOG_FUNCTION (this << retx);
  m_discNumRetx = retx;
}

} // end namespace
