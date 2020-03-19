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

#include "nr-sl-ue-rrc.h"

#include <ns3/lte-ue-rrc.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>

#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <ns3/lte-rrc-sap.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeRrc");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeRrc);

TypeId NrSlUeRrc::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeRrc")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<NrSlUeRrc> ()
  ;
  return tid;
}
NrSlUeRrc::NrSlUeRrc ()
{
  NS_LOG_FUNCTION (this);
  m_nrSlRrcSapUser = new MemberNrSlUeRrcSapUser<NrSlUeRrc> (this);
}

NrSlUeRrc::~NrSlUeRrc (void)
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeRrc::SetNrSlUeRrcSapProvider (NrSlUeRrcSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeRrcSapProvider = s;
}

NrSlUeRrcSapUser*
NrSlUeRrc::GetNrSlUeRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlRrcSapUser;
}

void
NrSlUeRrc::SetNrSlEnabled (bool status)
{
  NS_LOG_FUNCTION (this);
  m_slEnabled = status;
}

bool
NrSlUeRrc::IsNrSlEnabled ()
{
  NS_LOG_FUNCTION (this);
  return m_slEnabled;
}

void
NrSlUeRrc::SetNrSlPreconfiguration (const LteRrcSap::SidelinkPreconfigNr &preconfiguration)
{
  NS_LOG_FUNCTION (this);
  m_preconfiguration = preconfiguration;
}

const LteRrcSap::SidelinkPreconfigNr
NrSlUeRrc::DoGetNrSlPreconfiguration () const
{
  NS_LOG_FUNCTION (this);
  return m_preconfiguration;
}


} // namespace ns3
