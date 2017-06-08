/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Authors: Nicola Baldo <nbaldo@cttc.es>
 *          Lluis Parcerisa <lparcerisa@cttc.cat>
 * Modified by: NIST
 */

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/simulator.h>

#include "nist-lte-rrc-protocol-real.h"
#include "nist-lte-ue-rrc.h"
#include "nist-lte-enb-rrc.h"
#include "nist-lte-enb-net-device.h"
#include "nist-lte-ue-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteRrcProtocolReal");

const Time RRC_REAL_MSG_DELAY = MilliSeconds (0); 

NS_OBJECT_ENSURE_REGISTERED (NistLteUeRrcProtocolReal);

NistLteUeRrcProtocolReal::NistLteUeRrcProtocolReal ()
  :  m_ueRrcSapProvider (0),
    m_enbRrcSapProvider (0)
{
  m_ueRrcSapUser = new NistMemberLteUeRrcSapUser<NistLteUeRrcProtocolReal> (this);
  m_completeNistSetupParameters.srb0SapUser = new NistLteRlcSpecificLteRlcSapUser<NistLteUeRrcProtocolReal> (this);
  m_completeNistSetupParameters.srb1SapUser = new NistLtePdcpSpecificLtePdcpSapUser<NistLteUeRrcProtocolReal> (this);    
}

NistLteUeRrcProtocolReal::~NistLteUeRrcProtocolReal ()
{
}

void
NistLteUeRrcProtocolReal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ueRrcSapUser;
  delete m_completeNistSetupParameters.srb0SapUser;
  delete m_completeNistSetupParameters.srb1SapUser;
  m_rrc = 0;
}

TypeId
NistLteUeRrcProtocolReal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteUeRrcProtocolReal")
    .SetParent<Object> ()
    .AddConstructor<NistLteUeRrcProtocolReal> ()
  ;
  return tid;
}

void 
NistLteUeRrcProtocolReal::SetNistLteUeRrcSapProvider (NistLteUeRrcSapProvider* p)
{
  m_ueRrcSapProvider = p;
}

NistLteUeRrcSapUser* 
NistLteUeRrcProtocolReal::GetNistLteUeRrcSapUser ()
{
  return m_ueRrcSapUser;
}

void 
NistLteUeRrcProtocolReal::SetUeRrc (Ptr<NistLteUeRrc> rrc)
{
  m_rrc = rrc;
}

void 
NistLteUeRrcProtocolReal::DoSetup (NistLteUeRrcSapUser::NistSetupParameters params)
{
  NS_LOG_FUNCTION (this);

  m_setupParameters.srb0SapProvider = params.srb0SapProvider;
  m_setupParameters.srb1SapProvider = params.srb1SapProvider; 
  m_ueRrcSapProvider->CompleteSetup (m_completeNistSetupParameters);
}

void 
NistLteUeRrcProtocolReal::DoSendNistRrcConnectionRequest (NistLteRrcSap::NistRrcConnectionRequest msg)
{
  // initialize the RNTI and get the EnbNistLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();

  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionRequestHeader rrcConnectionRequestHeader;
  rrcConnectionRequestHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionRequestHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = m_rnti;
  transmitPdcpPduParameters.lcid = 0;

  m_setupParameters.srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
}

void 
NistLteUeRrcProtocolReal::DoSendNistRrcConnectionSetupCompleted (NistLteRrcSap::NistRrcConnectionSetupCompleted msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionSetupCompleteHeader rrcConnectionSetupCompleteHeader;
  rrcConnectionSetupCompleteHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionSetupCompleteHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = m_rnti;
  transmitPdcpSduParameters.lcid = 1;

  if (m_setupParameters.srb1SapProvider)
    {
      m_setupParameters.srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
    }
}

void 
NistLteUeRrcProtocolReal::DoSendNistRrcConnectionReconfigurationCompleted (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg)
{
  // re-initialize the RNTI and get the EnbNistLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();

  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReconfigurationCompleteHeader rrcConnectionReconfigurationCompleteHeader;
  rrcConnectionReconfigurationCompleteHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReconfigurationCompleteHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = m_rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupParameters.srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}

void 
NistLteUeRrcProtocolReal::DoSendNistMeasurementReport (NistLteRrcSap::NistMeasurementReport msg)
{
  // re-initialize the RNTI and get the EnbNistLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();

  Ptr<Packet> packet = Create<Packet> ();

  NistMeasurementReportHeader measurementReportHeader;
  measurementReportHeader.SetMessage (msg);

  packet->AddHeader (measurementReportHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = m_rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupParameters.srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}

void 
NistLteUeRrcProtocolReal::DoSendNistRrcConnectionReestablishmentRequest (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReestablishmentRequestHeader rrcConnectionReestablishmentRequestHeader;
  rrcConnectionReestablishmentRequestHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReestablishmentRequestHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = m_rnti;
  transmitPdcpPduParameters.lcid = 0;

  m_setupParameters.srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
}

void 
NistLteUeRrcProtocolReal::DoSendNistRrcConnectionReestablishmentComplete (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReestablishmentCompleteHeader rrcConnectionReestablishmentCompleteHeader;
  rrcConnectionReestablishmentCompleteHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReestablishmentCompleteHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = m_rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupParameters.srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}

void
NistLteUeRrcProtocolReal::DoSendSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  //todo: define header
  SidelinkUeInformationHeader sidelinkUeInformationHeader;
  sidelinkUeInformationHeader.SetMessage (msg);

  packet->AddHeader (sidelinkUeInformationHeader);
  
  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = m_rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupParameters.srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}
  

void 
NistLteUeRrcProtocolReal::SetEnbRrcSapProvider ()
{
  uint16_t cellId = m_rrc->GetCellId ();

  // walk list of all nodes to get the peer eNB
  Ptr<NistLteEnbNetDevice> enbDev;
  NodeList::Iterator listEnd = NodeList::End ();
  bool found = false;
  for (NodeList::Iterator i = NodeList::Begin (); 
       (i != listEnd) && (!found); 
       ++i)
    {
      Ptr<Node> node = *i;
      int nDevs = node->GetNDevices ();
      for (int j = 0; 
           (j < nDevs) && (!found);
           j++)
        {
          enbDev = node->GetDevice (j)->GetObject <NistLteEnbNetDevice> ();
          if (enbDev == 0)
            {
              continue;
            }
          else
            {
              if (enbDev->GetCellId () == cellId)
                {
                  found = true;
                  break;
                }
            }
        }
    }
  NS_ASSERT_MSG (found, " Unable to find eNB with CellId =" << cellId);
  m_enbRrcSapProvider = enbDev->GetRrc ()->GetNistLteEnbRrcSapProvider ();
  Ptr<NistLteEnbRrcProtocolReal> enbRrcProtocolReal = enbDev->GetRrc ()->GetObject<NistLteEnbRrcProtocolReal> ();
  enbRrcProtocolReal->SetUeRrcSapProvider (m_rnti, m_ueRrcSapProvider);
}

void
NistLteUeRrcProtocolReal::DoReceivePdcpPdu (Ptr<Packet> p)
{
  // Get type of message received
  NistRrcDlCcchMessage rrcDlCcchMessage;
  p->PeekHeader (rrcDlCcchMessage);

  // Declare possible headers to receive
  NistRrcConnectionReestablishmentHeader rrcConnectionReestablishmentHeader;
  NistRrcConnectionReestablishmentRejectHeader rrcConnectionReestablishmentRejectHeader;
  NistRrcConnectionSetupHeader rrcConnectionSetupHeader;
  NistRrcConnectionRejectHeader rrcConnectionRejectHeader;

  // Declare possible messages
  NistLteRrcSap::NistRrcConnectionReestablishment rrcConnectionReestablishmentMsg;
  NistLteRrcSap::NistRrcConnectionReestablishmentReject rrcConnectionReestablishmentRejectMsg;
  NistLteRrcSap::NistRrcConnectionSetup rrcConnectionSetupMsg;
  NistLteRrcSap::NistRrcConnectionReject rrcConnectionRejectMsg;

  // Deserialize packet and call member recv function with appropiate structure
  switch ( rrcDlCcchMessage.GetMessageType () )
    {
    case 0:
      // NistRrcConnectionReestablishment
      p->RemoveHeader (rrcConnectionReestablishmentHeader);
      rrcConnectionReestablishmentMsg = rrcConnectionReestablishmentHeader.GetMessage ();
      m_ueRrcSapProvider->RecvNistRrcConnectionReestablishment (rrcConnectionReestablishmentMsg);
      break;
    case 1:
      // NistRrcConnectionReestablishmentReject
      p->RemoveHeader (rrcConnectionReestablishmentRejectHeader);
      rrcConnectionReestablishmentRejectMsg = rrcConnectionReestablishmentRejectHeader.GetMessage ();
      // m_ueRrcSapProvider->RecvNistRrcConnectionReestablishmentReject (rrcConnectionReestablishmentRejectMsg);
      break;
    case 2:
      // NistRrcConnectionReject
      p->RemoveHeader (rrcConnectionRejectHeader);
      rrcConnectionRejectMsg = rrcConnectionRejectHeader.GetMessage ();
      m_ueRrcSapProvider->RecvNistRrcConnectionReject (rrcConnectionRejectMsg);
      break;
    case 3:
      // NistRrcConnectionSetup
      p->RemoveHeader (rrcConnectionSetupHeader);
      rrcConnectionSetupMsg = rrcConnectionSetupHeader.GetMessage ();
      m_ueRrcSapProvider->RecvNistRrcConnectionSetup (rrcConnectionSetupMsg);
      break;
    }
}

void
NistLteUeRrcProtocolReal::DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params)
{
  // Get type of message received
  NistRrcDlDcchMessage rrcDlDcchMessage;
  params.pdcpSdu->PeekHeader (rrcDlDcchMessage);

  // Declare possible headers to receive
  NistRrcConnectionReconfigurationHeader rrcConnectionReconfigurationHeader;
  NistRrcConnectionReleaseHeader rrcConnectionReleaseHeader;

  // Declare possible messages to receive
  NistLteRrcSap::NistRrcConnectionReconfiguration rrcConnectionReconfigurationMsg;
  NistLteRrcSap::NistRrcConnectionRelease rrcConnectionReleaseMsg;

  // Deserialize packet and call member recv function with appropiate structure
  switch ( rrcDlDcchMessage.GetMessageType () )
    {
    case 4:
      params.pdcpSdu->RemoveHeader (rrcConnectionReconfigurationHeader);
      rrcConnectionReconfigurationMsg = rrcConnectionReconfigurationHeader.GetMessage ();
      m_ueRrcSapProvider->RecvNistRrcConnectionReconfiguration (rrcConnectionReconfigurationMsg);
      break;
    case 5:
      params.pdcpSdu->RemoveHeader (rrcConnectionReleaseHeader);
      rrcConnectionReleaseMsg = rrcConnectionReleaseHeader.GetMessage ();
      //m_ueRrcSapProvider->RecvNistRrcConnectionRelease (rrcConnectionReleaseMsg);
      break;
    }
}

NS_OBJECT_ENSURE_REGISTERED (NistLteEnbRrcProtocolReal);

NistLteEnbRrcProtocolReal::NistLteEnbRrcProtocolReal ()
  :  m_enbRrcSapProvider (0)
{
  NS_LOG_FUNCTION (this);
  m_enbRrcSapUser = new NistMemberLteEnbRrcSapUser<NistLteEnbRrcProtocolReal> (this);
}

NistLteEnbRrcProtocolReal::~NistLteEnbRrcProtocolReal ()
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbRrcProtocolReal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_enbRrcSapUser;
  for (std::map<uint16_t, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters>::iterator 
         it = m_completeNistSetupUeParametersMap.begin ();
       it != m_completeNistSetupUeParametersMap.end ();
       ++it)
    {     
      delete it->second.srb0SapUser;
      delete it->second.srb1SapUser;
    }
  m_completeNistSetupUeParametersMap.clear ();
}

TypeId
NistLteEnbRrcProtocolReal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteEnbRrcProtocolReal")
    .SetParent<Object> ()
    .AddConstructor<NistLteEnbRrcProtocolReal> ()
  ;
  return tid;
}

void 
NistLteEnbRrcProtocolReal::SetNistLteEnbRrcSapProvider (NistLteEnbRrcSapProvider* p)
{
  m_enbRrcSapProvider = p;
}

NistLteEnbRrcSapUser* 
NistLteEnbRrcProtocolReal::GetNistLteEnbRrcSapUser ()
{
  return m_enbRrcSapUser;
}

void 
NistLteEnbRrcProtocolReal::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;
}

NistLteUeRrcSapProvider* 
NistLteEnbRrcProtocolReal::GetUeRrcSapProvider (uint16_t rnti)
{
  std::map<uint16_t, NistLteUeRrcSapProvider*>::const_iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  return it->second;
}

void 
NistLteEnbRrcProtocolReal::SetUeRrcSapProvider (uint16_t rnti, NistLteUeRrcSapProvider* p)
{
  std::map<uint16_t, NistLteUeRrcSapProvider*>::iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  it->second = p;
}

void 
NistLteEnbRrcProtocolReal::DoSetupUe (uint16_t rnti, NistLteEnbRrcSapUser::NistSetupUeParameters params)
{
  NS_LOG_FUNCTION (this << rnti);

  // // walk list of all nodes to get the peer UE RRC SAP Provider
  // Ptr<NistLteUeRrc> ueRrc;
  // NodeList::Iterator listEnd = NodeList::End ();
  // bool found = false;
  // for (NodeList::Iterator i = NodeList::Begin (); (i != listEnd) && (found == false); i++)
  //   {
  //     Ptr<Node> node = *i;
  //     int nDevs = node->GetNDevices ();
  //     for (int j = 0; j < nDevs; j++)
  //       {
  //         Ptr<NistLteUeNetDevice> ueDev = node->GetDevice (j)->GetObject <NistLteUeNetDevice> ();
  //         if (!ueDev)
  //           {
  //             continue;
  //           }
  //         else
  //           {
  //             ueRrc = ueDev->GetRrc ();
  //             if ((ueRrc->GetRnti () == rnti) && (ueRrc->GetCellId () == m_cellId))
  //               {
  //              found = true;
  //              break;
  //               }
  //           }
  //       }
  //   }
  // NS_ASSERT_MSG (found , " Unable to find UE with RNTI=" << rnti << " cellId=" << m_cellId);
  // m_enbRrcSapProviderMap[rnti] = ueRrc->GetNistLteUeRrcSapProvider ();

  // just create empty entry, the UeRrcSapProvider will be set by the
  // ue upon connection request or connection reconfiguration
  // completed 
  m_enbRrcSapProviderMap[rnti] = 0;

  // Store NistSetupUeParameters
  m_setupUeParametersMap[rnti] = params;

  NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters completeNistSetupUeParameters;
  std::map<uint16_t, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters>::iterator 
    csupIt = m_completeNistSetupUeParametersMap.find (rnti);
  if (csupIt == m_completeNistSetupUeParametersMap.end ())
    {
      // Create NistLteRlcSapUser, NistLtePdcpSapUser
      NistLteRlcSapUser* srb0SapUser = new NistRealProtocolRlcSapUser (this,rnti);
      NistLtePdcpSapUser* srb1SapUser = new NistLtePdcpSpecificLtePdcpSapUser<NistLteEnbRrcProtocolReal> (this);
      completeNistSetupUeParameters.srb0SapUser = srb0SapUser;
      completeNistSetupUeParameters.srb1SapUser = srb1SapUser;
      // Store NistLteRlcSapUser, NistLtePdcpSapUser
      m_completeNistSetupUeParametersMap[rnti] = completeNistSetupUeParameters;      
    }
  else
    {
      completeNistSetupUeParameters = csupIt->second;
    }
  m_enbRrcSapProvider->CompleteSetupUe (rnti, completeNistSetupUeParameters);
}

void 
NistLteEnbRrcProtocolReal::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  std::map<uint16_t, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters>::iterator 
    it = m_completeNistSetupUeParametersMap.find (rnti);
  NS_ASSERT (it != m_completeNistSetupUeParametersMap.end ());
  delete it->second.srb0SapUser;
  delete it->second.srb1SapUser;
  m_completeNistSetupUeParametersMap.erase (it);
  m_enbRrcSapProviderMap.erase (rnti);
  m_setupUeParametersMap.erase (rnti);
}

void 
NistLteEnbRrcProtocolReal::DoSendNistSystemInformation (NistLteRrcSap::NistSystemInformation msg)
{
  NS_LOG_FUNCTION (this << m_cellId);
  // walk list of all nodes to get UEs with this cellId
  Ptr<NistLteUeRrc> ueRrc;
  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
    {
      Ptr<Node> node = *i;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; ++j)
        {
          Ptr<NistLteUeNetDevice> ueDev = node->GetDevice (j)->GetObject <NistLteUeNetDevice> ();
          if (ueDev != 0)
            {
              Ptr<NistLteUeRrc> ueRrc = ueDev->GetRrc ();
              NS_LOG_LOGIC ("considering UE IMSI " << ueDev->GetImsi () << " that has cellId " << ueRrc->GetCellId ());
              if (ueRrc->GetCellId () == m_cellId)
                {
                  NS_LOG_LOGIC ("sending SI to IMSI " << ueDev->GetImsi ());
                  //ueRrc->GetNistLteUeRrcSapProvider ()->RecvNistSystemInformation (msg);
                  Simulator::Schedule (RRC_REAL_MSG_DELAY, 
                                       &NistLteUeRrcSapProvider::RecvNistSystemInformation,
                                       ueRrc->GetNistLteUeRrcSapProvider (), 
                                       msg);
                }
            }
        }
    } 
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionSetup (uint16_t rnti, NistLteRrcSap::NistRrcConnectionSetup msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionSetupHeader rrcConnectionSetupHeader;
  rrcConnectionSetupHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionSetupHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = rnti;
  transmitPdcpPduParameters.lcid = 0;

  if (m_setupUeParametersMap.find (rnti) == m_setupUeParametersMap.end () )
    {
      std::cout << "RNTI not found in Enb setup parameters Map!" << std::endl;
    }
  else
    {
      m_setupUeParametersMap[rnti].srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
    }
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReject msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionRejectHeader rrcConnectionRejectHeader;
  rrcConnectionRejectHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionRejectHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = rnti;
  transmitPdcpPduParameters.lcid = 0;

  m_setupUeParametersMap[rnti].srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionReconfiguration (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReconfiguration msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReconfigurationHeader rrcConnectionReconfigurationHeader;
  rrcConnectionReconfigurationHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReconfigurationHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupUeParametersMap[rnti].srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionReestablishment (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishment msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReestablishmentHeader rrcConnectionReestablishmentHeader;
  rrcConnectionReestablishmentHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReestablishmentHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = rnti;
  transmitPdcpPduParameters.lcid = 0;

  m_setupUeParametersMap[rnti].srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentReject msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReestablishmentRejectHeader rrcConnectionReestablishmentRejectHeader;
  rrcConnectionReestablishmentRejectHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReestablishmentRejectHeader);

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters transmitPdcpPduParameters;
  transmitPdcpPduParameters.pdcpPdu = packet;
  transmitPdcpPduParameters.rnti = rnti;
  transmitPdcpPduParameters.lcid = 0;

  m_setupUeParametersMap[rnti].srb0SapProvider->TransmitPdcpPdu (transmitPdcpPduParameters);
}

void 
NistLteEnbRrcProtocolReal::DoSendNistRrcConnectionRelease (uint16_t rnti, NistLteRrcSap::NistRrcConnectionRelease msg)
{
  Ptr<Packet> packet = Create<Packet> ();

  NistRrcConnectionReleaseHeader rrcConnectionReleaseHeader;
  rrcConnectionReleaseHeader.SetMessage (msg);

  packet->AddHeader (rrcConnectionReleaseHeader);

  NistLtePdcpSapProvider::NistTransmitPdcpSduParameters transmitPdcpSduParameters;
  transmitPdcpSduParameters.pdcpSdu = packet;
  transmitPdcpSduParameters.rnti = rnti;
  transmitPdcpSduParameters.lcid = 1;

  m_setupUeParametersMap[rnti].srb1SapProvider->TransmitPdcpSdu (transmitPdcpSduParameters);
}

void
NistLteEnbRrcProtocolReal::DoReceivePdcpPdu (uint16_t rnti, Ptr<Packet> p)
{
  // Get type of message received
  NistRrcUlCcchMessage rrcUlCcchMessage;
  p->PeekHeader (rrcUlCcchMessage);

  // Declare possible headers to receive
  NistRrcConnectionReestablishmentRequestHeader rrcConnectionReestablishmentRequestHeader;
  NistRrcConnectionRequestHeader rrcConnectionRequestHeader;

  // Deserialize packet and call member recv function with appropiate structure
  switch ( rrcUlCcchMessage.GetMessageType () )
    {
    case 0:
      p->RemoveHeader (rrcConnectionReestablishmentRequestHeader);
      NistLteRrcSap::NistRrcConnectionReestablishmentRequest rrcConnectionReestablishmentRequestMsg;
      rrcConnectionReestablishmentRequestMsg = rrcConnectionReestablishmentRequestHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistRrcConnectionReestablishmentRequest (rnti,rrcConnectionReestablishmentRequestMsg);
      break;
    case 1:
      p->RemoveHeader (rrcConnectionRequestHeader);
      NistLteRrcSap::NistRrcConnectionRequest rrcConnectionRequestMsg;
      rrcConnectionRequestMsg = rrcConnectionRequestHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistRrcConnectionRequest (rnti,rrcConnectionRequestMsg);
      break;
    }
}

void
NistLteEnbRrcProtocolReal::DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params)
{
  // Get type of message received
  NistRrcUlDcchMessage rrcUlDcchMessage;
  params.pdcpSdu->PeekHeader (rrcUlDcchMessage);

  // Declare possible headers to receive
  NistMeasurementReportHeader measurementReportHeader;
  NistRrcConnectionReconfigurationCompleteHeader rrcConnectionReconfigurationCompleteHeader;
  NistRrcConnectionReestablishmentCompleteHeader rrcConnectionReestablishmentCompleteHeader;
  NistRrcConnectionSetupCompleteHeader rrcConnectionSetupCompleteHeader;
  SidelinkUeInformationHeader sidelinkUeInformationHeader; 
  
  // Declare possible messages to receive
  NistLteRrcSap::NistMeasurementReport measurementReportMsg;
  NistLteRrcSap::NistRrcConnectionReconfigurationCompleted rrcConnectionReconfigurationCompleteMsg;
  NistLteRrcSap::NistRrcConnectionReestablishmentComplete rrcConnectionReestablishmentCompleteMsg;
  NistLteRrcSap::NistRrcConnectionSetupCompleted rrcConnectionSetupCompletedMsg;
  NistLteRrcSap::SidelinkUeInformation sidelinkUeInformationMsg;
  
  // Deserialize packet and call member recv function with appropiate structure
  switch ( rrcUlDcchMessage.GetMessageType () )
    {
    case 1:
      params.pdcpSdu->RemoveHeader (measurementReportHeader);
      measurementReportMsg = measurementReportHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistMeasurementReport (params.rnti,measurementReportMsg);
      break;
    case 2:
      params.pdcpSdu->RemoveHeader (rrcConnectionReconfigurationCompleteHeader);
      rrcConnectionReconfigurationCompleteMsg = rrcConnectionReconfigurationCompleteHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistRrcConnectionReconfigurationCompleted (params.rnti,rrcConnectionReconfigurationCompleteMsg);
      break;
    case 3:
      params.pdcpSdu->RemoveHeader (rrcConnectionReestablishmentCompleteHeader);
      rrcConnectionReestablishmentCompleteMsg = rrcConnectionReestablishmentCompleteHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistRrcConnectionReestablishmentComplete (params.rnti,rrcConnectionReestablishmentCompleteMsg);
      break;
    case 4:
      params.pdcpSdu->RemoveHeader (rrcConnectionSetupCompleteHeader);
      rrcConnectionSetupCompletedMsg = rrcConnectionSetupCompleteHeader.GetMessage ();
      m_enbRrcSapProvider->RecvNistRrcConnectionSetupCompleted (params.rnti, rrcConnectionSetupCompletedMsg);
      break;
    case 20:
      params.pdcpSdu->RemoveHeader (sidelinkUeInformationHeader);
      sidelinkUeInformationMsg = sidelinkUeInformationHeader.GetMessage ();
      m_enbRrcSapProvider->RecvSidelinkUeInformation (params.rnti,sidelinkUeInformationMsg);
      break;
    }
}

Ptr<Packet> 
NistLteEnbRrcProtocolReal::DoEncodeNistHandoverPreparationInformation (NistLteRrcSap::NistHandoverPreparationInfo msg)
{
  NistHandoverPreparationInfoHeader h;
  h.SetMessage (msg);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

NistLteRrcSap::NistHandoverPreparationInfo 
NistLteEnbRrcProtocolReal::DoDecodeNistHandoverPreparationInformation (Ptr<Packet> p)
{
  NistHandoverPreparationInfoHeader h;
  p->RemoveHeader (h);
  NistLteRrcSap::NistHandoverPreparationInfo msg = h.GetMessage ();
  return msg;
}

Ptr<Packet> 
NistLteEnbRrcProtocolReal::DoEncodeHandoverCommand (NistLteRrcSap::NistRrcConnectionReconfiguration msg)
{
  NistRrcConnectionReconfigurationHeader h;
  h.SetMessage (msg);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

NistLteRrcSap::NistRrcConnectionReconfiguration
NistLteEnbRrcProtocolReal::DoDecodeHandoverCommand (Ptr<Packet> p)
{
  NistRrcConnectionReconfigurationHeader h;
  p->RemoveHeader (h);
  NistLteRrcSap::NistRrcConnectionReconfiguration msg = h.GetMessage ();
  return msg;
}

//////////////////////////////////////////////////////

NistRealProtocolRlcSapUser::NistRealProtocolRlcSapUser (NistLteEnbRrcProtocolReal* pdcp, uint16_t rnti)
  : m_pdcp (pdcp),
    m_rnti (rnti)
{
}

NistRealProtocolRlcSapUser::NistRealProtocolRlcSapUser ()
{
}

void
NistRealProtocolRlcSapUser::ReceivePdcpPdu (Ptr<Packet> p)
{
  m_pdcp->DoReceivePdcpPdu (m_rnti, p);
}

} // namespace ns3
