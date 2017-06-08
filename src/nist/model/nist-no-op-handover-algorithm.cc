/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Budiarto Herman
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
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#include "nist-no-op-handover-algorithm.h"
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistNoOpHandoverAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (NistNoOpHandoverAlgorithm);


NistNoOpHandoverAlgorithm::NistNoOpHandoverAlgorithm ()
  : m_handoverManagementSapUser (0)
{
  NS_LOG_FUNCTION (this);
  m_handoverManagementSapProvider = new MemberNistLteHandoverManagementSapProvider<NistNoOpHandoverAlgorithm> (this);
}


NistNoOpHandoverAlgorithm::~NistNoOpHandoverAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}


void
NistNoOpHandoverAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_handoverManagementSapProvider;
}


TypeId
NistNoOpHandoverAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NistNoOpHandoverAlgorithm")
    .SetParent<NistLteHandoverAlgorithm> ()
    .AddConstructor<NistNoOpHandoverAlgorithm> ()
  ;
  return tid;
}


void
NistNoOpHandoverAlgorithm::SetNistLteHandoverManagementSapUser (NistLteHandoverManagementSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_handoverManagementSapUser = s;
}


NistLteHandoverManagementSapProvider*
NistNoOpHandoverAlgorithm::GetNistLteHandoverManagementSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_handoverManagementSapProvider;
}


void
NistNoOpHandoverAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  NistLteHandoverAlgorithm::DoInitialize ();
}


void
NistNoOpHandoverAlgorithm::DoReportUeMeas (uint16_t rnti,
                                       NistLteRrcSap::NistMeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);
}


} // end of namespace ns3
