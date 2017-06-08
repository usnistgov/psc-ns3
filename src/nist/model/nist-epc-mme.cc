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
 */

#include <ns3/fatal-error.h>
#include <ns3/log.h>

#include "nist-epc-s1ap-sap.h"
#include "nist-epc-s11-sap.h"

#include "nist-epc-mme.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcMme");

NS_OBJECT_ENSURE_REGISTERED (NistEpcMme);

NistEpcMme::NistEpcMme ()
  : m_s11SapSgw (0)
{
  NS_LOG_FUNCTION (this);
  m_s1apSapMme = new MemberNistEpcS1apSapMme<NistEpcMme> (this);
  m_s11SapMme = new MemberNistEpcS11SapMme<NistEpcMme> (this);
}


NistEpcMme::~NistEpcMme ()
{
  NS_LOG_FUNCTION (this);
}

void
NistEpcMme::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_s1apSapMme;
  delete m_s11SapMme;
}

TypeId
NistEpcMme::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcMme")
    .SetParent<Object> ()
    .AddConstructor<NistEpcMme> ()
    ;
  return tid;
}

NistEpcS1apSapMme* 
NistEpcMme::GetS1apSapMme ()
{
  return m_s1apSapMme;
}

void 
NistEpcMme::SetS11SapSgw (NistEpcS11SapSgw * s)
{
  m_s11SapSgw = s;
}

NistEpcS11SapMme* 
NistEpcMme::GetS11SapMme ()
{
  return m_s11SapMme;
}

void 
NistEpcMme::AddEnb (uint16_t gci, Ipv4Address enbS1uAddr, NistEpcS1apSapEnb* enbS1apSap)
{
  NS_LOG_FUNCTION (this << gci << enbS1uAddr);
  Ptr<NistEnbInfo> enbInfo = Create<NistEnbInfo> ();
  enbInfo->gci = gci;
  enbInfo->s1uAddr = enbS1uAddr;
  enbInfo->s1apSapEnb = enbS1apSap;
  m_enbInfoMap[gci] = enbInfo;
}

void 
NistEpcMme::AddUe (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  Ptr<NistUeInfo> ueInfo = Create<NistUeInfo> ();
  ueInfo->imsi = imsi;
  ueInfo->mmeUeS1Id = imsi;
  m_ueInfoMap[imsi] = ueInfo;
  ueInfo->bearerCounter = 0;
}

uint8_t
NistEpcMme::AddBearer (uint64_t imsi, Ptr<NistEpcTft> tft, NistEpsBearer bearer)
{
  NS_LOG_FUNCTION (this << imsi);
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  NS_ASSERT_MSG (it->second->bearerCounter < 11, "too many bearers already! " << it->second->bearerCounter);
  NistBearerInfo bearerInfo;
  bearerInfo.bearerId = ++(it->second->bearerCounter);
  bearerInfo.tft = tft;
  bearerInfo.bearer = bearer;  
  it->second->bearersToBeActivated.push_back (bearerInfo);
  return bearerInfo.bearerId;
}


// S1-AP SAP MME forwarded methods

void 
NistEpcMme::DoInitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t gci)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id << imsi << gci);
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  it->second->cellId = gci;
  NistEpcS11SapSgw::NistCreateSessionRequestMessage msg;
  msg.imsi = imsi;
  msg. uli.gci = gci;
  for (std::list<NistBearerInfo>::iterator bit = it->second->bearersToBeActivated.begin ();
       bit != it->second->bearersToBeActivated.end ();
       ++bit)
    {
      NistEpcS11SapSgw::NistBearerContextToBeCreated bearerContext;
      bearerContext.epsBearerId =  bit->bearerId; 
      bearerContext.bearerLevelQos = bit->bearer; 
      bearerContext.tft = bit->tft;
      msg.bearerContextsToBeCreated.push_back (bearerContext);
    }
  m_s11SapSgw->CreateSessionRequest (msg);
}

void 
NistEpcMme::DoInitialContextSetupResponse (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistEpcS1apSapMme::NistErabSetupItem> erabSetupList)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id);
  NS_FATAL_ERROR ("unimplemented");
}

void 
NistEpcMme::DoPathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t gci, std::list<NistEpcS1apSapMme::NistErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id << gci);

  uint64_t imsi = mmeUeS1Id; 
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  NS_LOG_INFO ("IMSI " << imsi << " old eNB: " << it->second->cellId << ", new eNB: " << gci);
  it->second->cellId = gci;
  it->second->enbUeS1Id = enbUeS1Id;

  NistEpcS11SapSgw::NistModifyBearerRequestMessage msg;
  msg.teid = imsi; // trick to avoid the need for allocating TEIDs on the S11 interface
  msg.uli.gci = gci;
  // bearer modification is not supported for now
  m_s11SapSgw->ModifyBearerRequest (msg);
}


// S11 SAP MME forwarded methods

void 
NistEpcMme::DoCreateSessionResponse (NistEpcS11SapMme::NistCreateSessionResponseMessage msg)
{
  NS_LOG_FUNCTION (this << msg.teid);
  uint64_t imsi = msg.teid;
  std::list<NistEpcS1apSapEnb::NistErabToBeSetupItem> erabToBeSetupList;
  for (std::list<NistEpcS11SapMme::NistBearerContextCreated>::iterator bit = msg.bearerContextsCreated.begin ();
       bit != msg.bearerContextsCreated.end ();
       ++bit)
    {
      NistEpcS1apSapEnb::NistErabToBeSetupItem erab;
      erab.erabId = bit->epsBearerId;
      erab.erabLevelQosParameters = bit->bearerLevelQos;
      erab.transportLayerAddress = bit->sgwNistFteid.address;
      erab.sgwTeid = bit->sgwNistFteid.teid;      
      erabToBeSetupList.push_back (erab);
    }
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  uint16_t cellId = it->second->cellId;
  uint16_t enbUeS1Id = it->second->enbUeS1Id;
  uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
  std::map<uint16_t, Ptr<NistEnbInfo> >::iterator jt = m_enbInfoMap.find (cellId);
  NS_ASSERT_MSG (jt != m_enbInfoMap.end (), "could not find any eNB with CellId " << cellId);
  jt->second->s1apSapEnb->InitialContextSetupRequest (mmeUeS1Id, enbUeS1Id, erabToBeSetupList);
}


void 
NistEpcMme::DoModifyBearerResponse (NistEpcS11SapMme::NistModifyBearerResponseMessage msg)
{
  NS_LOG_FUNCTION (this << msg.teid);
  NS_ASSERT (msg.cause == NistEpcS11SapMme::NistModifyBearerResponseMessage::REQUEST_ACCEPTED);
  uint64_t imsi = msg.teid;
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  uint64_t enbUeS1Id = it->second->enbUeS1Id;
  uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
  uint16_t cgi = it->second->cellId;
  std::list<NistEpcS1apSapEnb::NistErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList; // unused for now
  std::map<uint16_t, Ptr<NistEnbInfo> >::iterator jt = m_enbInfoMap.find (it->second->cellId);
  NS_ASSERT_MSG (jt != m_enbInfoMap.end (), "could not find any eNB with CellId " << it->second->cellId);
  jt->second->s1apSapEnb->PathSwitchRequestAcknowledge (enbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInUplinkList);
}

void
NistEpcMme::DoErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistEpcS1apSapMme::NistErabToBeReleasedIndication> erabToBeReleaseIndication)
{
  NS_LOG_FUNCTION (this << mmeUeS1Id << enbUeS1Id);
  uint64_t imsi = mmeUeS1Id;
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);

  NistEpcS11SapSgw::NistDeleteBearerCommandMessage msg;
  // trick to avoid the need for allocating TEIDs on the S11 interface
  msg.teid = imsi;

  for (std::list<NistEpcS1apSapMme::NistErabToBeReleasedIndication>::iterator bit = erabToBeReleaseIndication.begin (); bit != erabToBeReleaseIndication.end (); ++bit)
    {
      NistEpcS11SapSgw::NistBearerContextToBeRemoved bearerContext;
      bearerContext.epsBearerId =  bit->erabId;
      msg.bearerContextsToBeRemoved.push_back (bearerContext);
    }
  //Delete Bearer command towards epc-sgw-pgw-application
  m_s11SapSgw->DeleteBearerCommand (msg);
}

void
NistEpcMme::DoDeleteBearerRequest (NistEpcS11SapMme::NistDeleteBearerRequestMessage msg)
{
  NS_LOG_FUNCTION (this);
  uint64_t imsi = msg.teid;
  std::map<uint64_t, Ptr<NistUeInfo> >::iterator it = m_ueInfoMap.find (imsi);
  NS_ASSERT_MSG (it != m_ueInfoMap.end (), "could not find any UE with IMSI " << imsi);
  NistEpcS11SapSgw::NistDeleteBearerResponseMessage res;

  res.teid = imsi;

  for (std::list<NistEpcS11SapMme::NistBearerContextRemoved>::iterator bit = msg.bearerContextsRemoved.begin ();
       bit != msg.bearerContextsRemoved.end ();
       ++bit)
    {
      NistEpcS11SapSgw::NistBearerContextRemovedSgwPgw bearerContext;
      bearerContext.epsBearerId = bit->epsBearerId;
      res.bearerContextsRemoved.push_back (bearerContext);

      RemoveBearer (it->second, bearerContext.epsBearerId); //schedules function to erase, context of de-activated bearer
    }
  //schedules Delete Bearer Response towards epc-sgw-pgw-application
  m_s11SapSgw->DeleteBearerResponse (res);
}

void NistEpcMme::RemoveBearer (Ptr<NistUeInfo> ueInfo, uint8_t epsBearerId)
{
  NS_LOG_FUNCTION (this << epsBearerId);
  for (std::list<NistBearerInfo>::iterator bit = ueInfo->bearersToBeActivated.begin ();
       bit != ueInfo->bearersToBeActivated.end ();
       ++bit)
    {
      if (bit->bearerId == epsBearerId)
        {
          ueInfo->bearersToBeActivated.erase (bit);
          break;
        }
    }
}

} // namespace ns3
