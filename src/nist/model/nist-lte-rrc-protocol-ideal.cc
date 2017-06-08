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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by: NIST
 */

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/simulator.h>

#include "nist-lte-rrc-protocol-ideal.h"
#include "nist-lte-ue-rrc.h"
#include "nist-lte-enb-rrc.h"
#include "nist-lte-enb-net-device.h"
#include "nist-lte-ue-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteRrcProtocolIdeal");

static const Time RRC_IDEAL_MSG_DELAY = MilliSeconds (0);

NS_OBJECT_ENSURE_REGISTERED (NistLteUeRrcProtocolIdeal);

NistLteUeRrcProtocolIdeal::NistLteUeRrcProtocolIdeal ()
  :  m_ueRrcSapProvider (0),
     m_enbRrcSapProvider (0)
{
  m_ueRrcSapUser = new NistMemberLteUeRrcSapUser<NistLteUeRrcProtocolIdeal> (this);
}

NistLteUeRrcProtocolIdeal::~NistLteUeRrcProtocolIdeal ()
{
}

void
NistLteUeRrcProtocolIdeal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ueRrcSapUser;
  m_rrc = 0;
}

TypeId
NistLteUeRrcProtocolIdeal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteUeRrcProtocolIdeal")
    .SetParent<Object> ()
    .AddConstructor<NistLteUeRrcProtocolIdeal> ()
    ;
  return tid;
}

void 
NistLteUeRrcProtocolIdeal::SetNistLteUeRrcSapProvider (NistLteUeRrcSapProvider* p)
{
  m_ueRrcSapProvider = p;
}

NistLteUeRrcSapUser* 
NistLteUeRrcProtocolIdeal::GetNistLteUeRrcSapUser ()
{
  return m_ueRrcSapUser;
}

void 
NistLteUeRrcProtocolIdeal::SetUeRrc (Ptr<NistLteUeRrc> rrc)
{
  m_rrc = rrc;
}

void 
NistLteUeRrcProtocolIdeal::DoSetup (NistLteUeRrcSapUser::NistSetupParameters params)
{
  NS_LOG_FUNCTION (this);
  // We don't care about SRB0/SRB1 since we use ideal RRC messages.
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistRrcConnectionRequest (NistLteRrcSap::NistRrcConnectionRequest msg)
{
  // initialize the RNTI and get the EnbNistLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();
    
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
                       &NistLteEnbRrcSapProvider::RecvNistRrcConnectionRequest,
                       m_enbRrcSapProvider,
                       m_rnti, 
                       msg);
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistRrcConnectionSetupCompleted (NistLteRrcSap::NistRrcConnectionSetupCompleted msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteEnbRrcSapProvider::RecvNistRrcConnectionSetupCompleted,
                       m_enbRrcSapProvider,
		       m_rnti, 
		       msg);
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistRrcConnectionReconfigurationCompleted (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg)
{
  // re-initialize the RNTI and get the EnbNistLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();
    
   Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
                        &NistLteEnbRrcSapProvider::RecvNistRrcConnectionReconfigurationCompleted,
                        m_enbRrcSapProvider,
                        m_rnti, 
                        msg);
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistRrcConnectionReestablishmentRequest (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg)
{
   Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentRequest,
                       m_enbRrcSapProvider,
		       m_rnti, 
                        msg);
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistRrcConnectionReestablishmentComplete (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg)
{
   Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentComplete,
                       m_enbRrcSapProvider,
		       m_rnti, 
msg);
}

void 
NistLteUeRrcProtocolIdeal::DoSendNistMeasurementReport (NistLteRrcSap::NistMeasurementReport msg)
{
   Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
                        &NistLteEnbRrcSapProvider::RecvNistMeasurementReport,
                        m_enbRrcSapProvider,
                        m_rnti, 
                        msg);
}

void
NistLteUeRrcProtocolIdeal::DoSendSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
                        &NistLteEnbRrcSapProvider::RecvSidelinkUeInformation,
                        m_enbRrcSapProvider,
                        m_rnti, 
                        msg);
}
  
  
void 
NistLteUeRrcProtocolIdeal::SetEnbRrcSapProvider ()
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
  Ptr<NistLteEnbRrcProtocolIdeal> enbRrcProtocolIdeal = enbDev->GetRrc ()->GetObject<NistLteEnbRrcProtocolIdeal> ();
  enbRrcProtocolIdeal->SetUeRrcSapProvider (m_rnti, m_ueRrcSapProvider);
}


NS_OBJECT_ENSURE_REGISTERED (NistLteEnbRrcProtocolIdeal);

NistLteEnbRrcProtocolIdeal::NistLteEnbRrcProtocolIdeal ()
  :  m_enbRrcSapProvider (0)
{
  NS_LOG_FUNCTION (this);
  m_enbRrcSapUser = new NistMemberLteEnbRrcSapUser<NistLteEnbRrcProtocolIdeal> (this);
}

NistLteEnbRrcProtocolIdeal::~NistLteEnbRrcProtocolIdeal ()
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbRrcProtocolIdeal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_enbRrcSapUser;  
}

TypeId
NistLteEnbRrcProtocolIdeal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteEnbRrcProtocolIdeal")
    .SetParent<Object> ()
    .AddConstructor<NistLteEnbRrcProtocolIdeal> ()
    ;
  return tid;
}

void 
NistLteEnbRrcProtocolIdeal::SetNistLteEnbRrcSapProvider (NistLteEnbRrcSapProvider* p)
{
  m_enbRrcSapProvider = p;
}

NistLteEnbRrcSapUser* 
NistLteEnbRrcProtocolIdeal::GetNistLteEnbRrcSapUser ()
{
  return m_enbRrcSapUser;
}

void 
NistLteEnbRrcProtocolIdeal::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;
}

NistLteUeRrcSapProvider* 
NistLteEnbRrcProtocolIdeal::GetUeRrcSapProvider (uint16_t rnti)
{
  std::map<uint16_t, NistLteUeRrcSapProvider*>::const_iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  return it->second;
}

void 
NistLteEnbRrcProtocolIdeal::SetUeRrcSapProvider (uint16_t rnti, NistLteUeRrcSapProvider* p)
{
  std::map<uint16_t, NistLteUeRrcSapProvider*>::iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  it->second = p;
}

void 
NistLteEnbRrcProtocolIdeal::DoSetupUe (uint16_t rnti, NistLteEnbRrcSapUser::NistSetupUeParameters params)
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
  //       	  found = true;
  //       	  break;
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

}

void 
NistLteEnbRrcProtocolIdeal::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  m_enbRrcSapProviderMap.erase (rnti);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistSystemInformation (NistLteRrcSap::NistSystemInformation msg)
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
                  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
                                       &NistLteUeRrcSapProvider::RecvNistSystemInformation,
                                       ueRrc->GetNistLteUeRrcSapProvider (), 
                                       msg);          
                }             
            }
        }
    } 
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionSetup (uint16_t rnti, NistLteRrcSap::NistRrcConnectionSetup msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionSetup,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionReconfiguration (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReconfiguration msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionReconfiguration,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionReestablishment (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishment msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionReestablishment,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentReject msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionReestablishmentReject,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionRelease (uint16_t rnti, NistLteRrcSap::NistRrcConnectionRelease msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionRelease,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

void 
NistLteEnbRrcProtocolIdeal::DoSendNistRrcConnectionReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReject msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY, 
		       &NistLteUeRrcSapProvider::RecvNistRrcConnectionReject,
		       GetUeRrcSapProvider (rnti), 
		       msg);
}

/*
 * The purpose of NistLteEnbRrcProtocolIdeal is to avoid encoding
 * messages. In order to do so, we need to have some form of encoding for
 * inter-node RRC messages like NistHandoverPreparationInfo and HandoverCommand. Doing so
 * directly is not practical (these messages includes a lot of
 * information elements, so encoding all of them would defeat the
 * purpose of NistLteEnbRrcProtocolIdeal. The workaround is to store the
 * actual message in a global map, so that then we can just encode the
 * key in a header and send that between eNBs over X2.
 * 
 */

static std::map<uint32_t, NistLteRrcSap::NistHandoverPreparationInfo> g_handoverPreparationInfoMsgMap;
static uint32_t g_handoverPreparationInfoMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 * 
 */
class NistIdealHandoverPreparationInfoHeader : public Header
{
public:
  uint32_t GetMsgId ();
  void SetMsgId (uint32_t id);
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint32_t m_msgId;
};

uint32_t 
NistIdealHandoverPreparationInfoHeader::GetMsgId ()
{
  return m_msgId;
}  

void 
NistIdealHandoverPreparationInfoHeader::SetMsgId (uint32_t id)
{
  m_msgId = id;
}  


TypeId
NistIdealHandoverPreparationInfoHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistIdealHandoverPreparationInfoHeader")
    .SetParent<Header> ()
    .AddConstructor<NistIdealHandoverPreparationInfoHeader> ()
  ;
  return tid;
}

TypeId
NistIdealHandoverPreparationInfoHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void NistIdealHandoverPreparationInfoHeader::Print (std::ostream &os)  const
{
  os << " msgId=" << m_msgId;
}

uint32_t NistIdealHandoverPreparationInfoHeader::GetSerializedSize (void) const
{
  return 4;
}

void NistIdealHandoverPreparationInfoHeader::Serialize (Buffer::Iterator start) const
{  
  start.WriteU32 (m_msgId);
}

uint32_t NistIdealHandoverPreparationInfoHeader::Deserialize (Buffer::Iterator start)
{
  m_msgId = start.ReadU32 ();
  return GetSerializedSize ();
}



Ptr<Packet> 
NistLteEnbRrcProtocolIdeal::DoEncodeNistHandoverPreparationInformation (NistLteRrcSap::NistHandoverPreparationInfo msg)
{
  uint32_t msgId = ++g_handoverPreparationInfoMsgIdCounter;
  NS_ASSERT_MSG (g_handoverPreparationInfoMsgMap.find (msgId) == g_handoverPreparationInfoMsgMap.end (), "msgId " << msgId << " already in use");
  NS_LOG_INFO (" encoding msgId = " << msgId);
  g_handoverPreparationInfoMsgMap.insert (std::pair<uint32_t, NistLteRrcSap::NistHandoverPreparationInfo> (msgId, msg));
  NistIdealHandoverPreparationInfoHeader h;
  h.SetMsgId (msgId);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

NistLteRrcSap::NistHandoverPreparationInfo 
NistLteEnbRrcProtocolIdeal::DoDecodeNistHandoverPreparationInformation (Ptr<Packet> p)
{
  NistIdealHandoverPreparationInfoHeader h;
  p->RemoveHeader (h);
  uint32_t msgId = h.GetMsgId ();
  NS_LOG_INFO (" decoding msgId = " << msgId);
  std::map<uint32_t, NistLteRrcSap::NistHandoverPreparationInfo>::iterator it = g_handoverPreparationInfoMsgMap.find (msgId);
  NS_ASSERT_MSG (it != g_handoverPreparationInfoMsgMap.end (), "msgId " << msgId << " not found");
  NistLteRrcSap::NistHandoverPreparationInfo msg = it->second;
  g_handoverPreparationInfoMsgMap.erase (it);
  return msg;
}



static std::map<uint32_t, NistLteRrcSap::NistRrcConnectionReconfiguration> g_handoverCommandMsgMap;
static uint32_t g_handoverCommandMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 * 
 */
class NistIdealHandoverCommandHeader : public Header
{
public:
  uint32_t GetMsgId ();
  void SetMsgId (uint32_t id);
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint32_t m_msgId;
};

uint32_t 
NistIdealHandoverCommandHeader::GetMsgId ()
{
  return m_msgId;
}  

void 
NistIdealHandoverCommandHeader::SetMsgId (uint32_t id)
{
  m_msgId = id;
}  


TypeId
NistIdealHandoverCommandHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistIdealHandoverCommandHeader")
    .SetParent<Header> ()
    .AddConstructor<NistIdealHandoverCommandHeader> ()
  ;
  return tid;
}

TypeId
NistIdealHandoverCommandHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void NistIdealHandoverCommandHeader::Print (std::ostream &os)  const
{
  os << " msgId=" << m_msgId;
}

uint32_t NistIdealHandoverCommandHeader::GetSerializedSize (void) const
{
  return 4;
}

void NistIdealHandoverCommandHeader::Serialize (Buffer::Iterator start) const
{  
  start.WriteU32 (m_msgId);
}

uint32_t NistIdealHandoverCommandHeader::Deserialize (Buffer::Iterator start)
{
  m_msgId = start.ReadU32 ();
  return GetSerializedSize ();
}



Ptr<Packet> 
NistLteEnbRrcProtocolIdeal::DoEncodeHandoverCommand (NistLteRrcSap::NistRrcConnectionReconfiguration msg)
{
  uint32_t msgId = ++g_handoverCommandMsgIdCounter;
  NS_ASSERT_MSG (g_handoverCommandMsgMap.find (msgId) == g_handoverCommandMsgMap.end (), "msgId " << msgId << " already in use");
  NS_LOG_INFO (" encoding msgId = " << msgId);
  g_handoverCommandMsgMap.insert (std::pair<uint32_t, NistLteRrcSap::NistRrcConnectionReconfiguration> (msgId, msg));
  NistIdealHandoverCommandHeader h;
  h.SetMsgId (msgId);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

NistLteRrcSap::NistRrcConnectionReconfiguration
NistLteEnbRrcProtocolIdeal::DoDecodeHandoverCommand (Ptr<Packet> p)
{
  NistIdealHandoverCommandHeader h;
  p->RemoveHeader (h);
  uint32_t msgId = h.GetMsgId ();
  NS_LOG_INFO (" decoding msgId = " << msgId);
  std::map<uint32_t, NistLteRrcSap::NistRrcConnectionReconfiguration>::iterator it = g_handoverCommandMsgMap.find (msgId);
  NS_ASSERT_MSG (it != g_handoverCommandMsgMap.end (), "msgId " << msgId << " not found");
  NistLteRrcSap::NistRrcConnectionReconfiguration msg = it->second;
  g_handoverCommandMsgMap.erase (it);
  return msg;
}





} // namespace ns3
