/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jnin@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 *         Manuel Requena <manuel.requena@cttc.es>
 * Modified by: NIST
 */

#include <ns3/nist-epc-helper.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/ipv4-address.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcHelper");

NS_OBJECT_ENSURE_REGISTERED (NistEpcHelper);


NistEpcHelper::NistEpcHelper () 
{
  NS_LOG_FUNCTION (this);
}

NistEpcHelper::~NistEpcHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistEpcHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcHelper")
    .SetParent<Object> ()
    ;
  return tid;
}

void
NistEpcHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}



} // namespace ns3
