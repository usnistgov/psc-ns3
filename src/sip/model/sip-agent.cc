/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2020 University of Washington
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
 */

#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "sip-agent.h"
#include "sip-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SipAgent");

namespace sip {

NS_OBJECT_ENSURE_REGISTERED (SipAgent);

TypeId
SipAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::sip::SipAgent")
    .SetParent<SipElement> ()
    .SetGroupName ("Sip")
    .AddConstructor<SipAgent> ()
  ;
  return tid;
}

SipAgent::SipAgent ()
  : SipElement ()
{
  NS_LOG_FUNCTION (this);
}

SipAgent::~SipAgent ()
{
  NS_LOG_FUNCTION (this);
}

void
SipAgent::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  SipElement::DoDispose ();
}

} // namespace sip

} // namespace ns3
