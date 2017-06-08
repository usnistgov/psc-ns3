/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#include "nist-lte-fr-no-op-algorithm.h"
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteFrNoOpAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (NistLteFrNoOpAlgorithm);


NistLteFrNoOpAlgorithm::NistLteFrNoOpAlgorithm ()
  : m_ffrSapUser (0), m_ffrRrcSapUser (0)
{
  NS_LOG_FUNCTION (this);
  m_ffrSapProvider = new MemberNistLteFfrSapProvider<NistLteFrNoOpAlgorithm> (this);
  m_ffrRrcSapProvider = new MemberNistLteFfrRrcSapProvider<NistLteFrNoOpAlgorithm> (this);
}


NistLteFrNoOpAlgorithm::~NistLteFrNoOpAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}


void
NistLteFrNoOpAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ffrSapProvider;
  delete m_ffrRrcSapProvider;
}


TypeId
NistLteFrNoOpAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NistLteFrNoOpAlgorithm")
    .SetParent<NistLteFfrAlgorithm> ()
    .AddConstructor<NistLteFrNoOpAlgorithm> ()
  ;
  return tid;
}


void
NistLteFrNoOpAlgorithm::SetNistLteFfrSapUser (NistLteFfrSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrSapUser = s;
}


NistLteFfrSapProvider*
NistLteFrNoOpAlgorithm::GetNistLteFfrSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrSapProvider;
}

void
NistLteFrNoOpAlgorithm::SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrRrcSapUser = s;
}


NistLteFfrRrcSapProvider*
NistLteFrNoOpAlgorithm::GetNistLteFfrRrcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrRrcSapProvider;
}


void
NistLteFrNoOpAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  NistLteFfrAlgorithm::DoInitialize ();
}

void
NistLteFrNoOpAlgorithm::Reconfigure ()
{
  NS_LOG_FUNCTION (this);
}

std::vector <bool>
NistLteFrNoOpAlgorithm::DoGetAvailableDlRbg ()
{
  NS_LOG_FUNCTION (this);
  std::vector <bool> rbgMap;
  int rbgSize = GetRbgSize (m_dlBandwidth);
  rbgMap.resize (m_dlBandwidth/rbgSize, false);
  return rbgMap;
}

bool
NistLteFrNoOpAlgorithm::DoIsDlRbgAvailableForUe (int i, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  return true;
}

std::vector <bool>
NistLteFrNoOpAlgorithm::DoGetAvailableUlRbg ()
{
  NS_LOG_FUNCTION (this);
  std::vector <bool> rbgMap;
  rbgMap.resize (m_ulBandwidth, false);
  return rbgMap;
}

bool
NistLteFrNoOpAlgorithm::DoIsUlRbgAvailableForUe (int i, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  return true;
}

void
NistLteFrNoOpAlgorithm::DoReportDlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedDlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

void
NistLteFrNoOpAlgorithm::DoReportUlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

void
NistLteFrNoOpAlgorithm::DoReportUlCqiInfo (std::map <uint16_t, std::vector <double> > ulCqiMap)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

uint8_t
NistLteFrNoOpAlgorithm::DoGetTpc (uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  return 1; // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS36.213 Table 5.1.1.1-2
}

uint8_t
NistLteFrNoOpAlgorithm::DoGetMinContinuousUlBandwidth ()
{
  NS_LOG_FUNCTION (this);
  return m_ulBandwidth;
}

void
NistLteFrNoOpAlgorithm::DoReportUeMeas (uint16_t rnti,
                                    NistLteRrcSap::NistMeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

void
NistLteFrNoOpAlgorithm::DoRecvLoadInformation (NistEpcX2Sap::NistLoadInformationParams params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

} // end of namespace ns3
