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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo  <nbaldo@cttc.es>
 * Modified by: NIST
 */


#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>

#include "nist-lte-amc.h"
#include "nist-lte-control-messages.h"
#include "nist-lte-enb-net-device.h"
#include "nist-lte-ue-net-device.h"

#include <ns3/nist-lte-enb-mac.h>
#include <ns3/nist-lte-radio-bearer-tag.h>
#include <ns3/nist-lte-ue-phy.h>

#include "ns3/nist-lte-mac-sap.h"
#include <ns3/nist-lte-common.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteEnbMac");

NS_OBJECT_ENSURE_REGISTERED (NistLteEnbMac);



// //////////////////////////////////////
// member SAP forwarders
// //////////////////////////////////////


class NistEnbMacMemberLteEnbCmacSapProvider : public NistLteEnbCmacSapProvider
{
public:
  NistEnbMacMemberLteEnbCmacSapProvider (NistLteEnbMac* mac);

  // inherited from NistLteEnbCmacSapProvider
  virtual void ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  virtual void AddUe (uint16_t rnti);
  virtual void RemoveUe (uint16_t rnti);
  virtual void AddLc (NistLcInfo lcinfo, NistLteMacSapUser* msu);
  virtual void ReconfigureLc (NistLcInfo lcinfo);
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid);
  virtual void UeUpdateConfigurationReq (NistUeConfig params);
  virtual void AddPool (uint32_t group, Ptr<SidelinkCommResourcePool> pool);
  virtual void RemovePool (uint32_t group);
  virtual NistRachConfig GetNistRachConfig ();
  virtual NistAllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti);
  

private:
  NistLteEnbMac* m_mac;
};


NistEnbMacMemberLteEnbCmacSapProvider::NistEnbMacMemberLteEnbCmacSapProvider (NistLteEnbMac* mac)
  : m_mac (mac)
{
}

void
NistEnbMacMemberLteEnbCmacSapProvider::ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  m_mac->DoConfigureMac (ulBandwidth, dlBandwidth);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::AddUe (uint16_t rnti)
{
  m_mac->DoAddUe (rnti);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::RemoveUe (uint16_t rnti)
{
  m_mac->DoRemoveUe (rnti);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::AddLc (NistLcInfo lcinfo, NistLteMacSapUser* msu)
{
  m_mac->DoAddLc (lcinfo, msu);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::ReconfigureLc (NistLcInfo lcinfo)
{
  m_mac->DoReconfigureLc (lcinfo);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::ReleaseLc (uint16_t rnti, uint8_t lcid)
{
  m_mac->DoReleaseLc (rnti, lcid);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::UeUpdateConfigurationReq (NistUeConfig params)
{
  m_mac->DoUeUpdateConfigurationReq (params);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::AddPool (uint32_t group, Ptr<SidelinkCommResourcePool> pool)
{
  m_mac->DoAddPool (group, pool);
}

void
NistEnbMacMemberLteEnbCmacSapProvider::RemovePool (uint32_t group)
{
  m_mac->DoRemovePool (group);
}
  
NistLteEnbCmacSapProvider::NistRachConfig 
NistEnbMacMemberLteEnbCmacSapProvider::GetNistRachConfig ()
{
  return m_mac->DoGetNistRachConfig ();
}
 
NistLteEnbCmacSapProvider::NistAllocateNcRaPreambleReturnValue 
NistEnbMacMemberLteEnbCmacSapProvider::AllocateNcRaPreamble (uint16_t rnti)
{
  return m_mac->DoAllocateNcRaPreamble (rnti);
}


class NistEnbMacMemberFfMacSchedSapUser : public NistFfMacSchedSapUser
{
public:
  NistEnbMacMemberFfMacSchedSapUser (NistLteEnbMac* mac);


  virtual void SchedDlConfigInd (const struct NistSchedDlConfigIndParameters& params);
  virtual void SchedUlConfigInd (const struct NistSchedUlConfigIndParameters& params);
private:
  NistLteEnbMac* m_mac;
};


NistEnbMacMemberFfMacSchedSapUser::NistEnbMacMemberFfMacSchedSapUser (NistLteEnbMac* mac)
  : m_mac (mac)
{
}


void
NistEnbMacMemberFfMacSchedSapUser::SchedDlConfigInd (const struct NistSchedDlConfigIndParameters& params)
{
  m_mac->DoSchedDlConfigInd (params);
}



void
NistEnbMacMemberFfMacSchedSapUser::SchedUlConfigInd (const struct NistSchedUlConfigIndParameters& params)
{
  m_mac->DoSchedUlConfigInd (params);
}



class NistEnbMacMemberFfMacCschedSapUser : public NistFfMacCschedSapUser
{
public:
  NistEnbMacMemberFfMacCschedSapUser (NistLteEnbMac* mac);

  virtual void CschedCellConfigCnf (const struct NistCschedCellConfigCnfParameters& params);
  virtual void CschedNistUeConfigCnf (const struct NistCschedNistUeConfigCnfParameters& params);
  virtual void CschedLcConfigCnf (const struct NistCschedLcConfigCnfParameters& params);
  virtual void CschedLcReleaseCnf (const struct NistCschedLcReleaseCnfParameters& params);
  virtual void CschedUeReleaseCnf (const struct NistCschedUeReleaseCnfParameters& params);
  virtual void CschedNistUeConfigUpdateInd (const struct NistCschedNistUeConfigUpdateIndParameters& params);
  virtual void CschedCellConfigUpdateInd (const struct NistCschedCellConfigUpdateIndParameters& params);

private:
  NistLteEnbMac* m_mac;
};


NistEnbMacMemberFfMacCschedSapUser::NistEnbMacMemberFfMacCschedSapUser (NistLteEnbMac* mac)
  : m_mac (mac)
{
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedCellConfigCnf (const struct NistCschedCellConfigCnfParameters& params)
{
  m_mac->DoCschedCellConfigCnf (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedNistUeConfigCnf (const struct NistCschedNistUeConfigCnfParameters& params)
{
  m_mac->DoCschedNistUeConfigCnf (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedLcConfigCnf (const struct NistCschedLcConfigCnfParameters& params)
{
  m_mac->DoCschedLcConfigCnf (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedLcReleaseCnf (const struct NistCschedLcReleaseCnfParameters& params)
{
  m_mac->DoCschedLcReleaseCnf (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedUeReleaseCnf (const struct NistCschedUeReleaseCnfParameters& params)
{
  m_mac->DoCschedUeReleaseCnf (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedNistUeConfigUpdateInd (const struct NistCschedNistUeConfigUpdateIndParameters& params)
{
  m_mac->DoCschedNistUeConfigUpdateInd (params);
}

void
NistEnbMacMemberFfMacCschedSapUser::CschedCellConfigUpdateInd (const struct NistCschedCellConfigUpdateIndParameters& params)
{
  m_mac->DoCschedCellConfigUpdateInd (params);
}



// ---------- PHY-SAP


class NistEnbMacMemberLteEnbPhySapUser : public NistLteEnbPhySapUser
{
public:
  NistEnbMacMemberLteEnbPhySapUser (NistLteEnbMac* mac);

  // inherited from NistLteEnbPhySapUser
  virtual void ReceivePhyPdu (Ptr<Packet> p);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg);
  virtual void ReceiveRachPreamble (uint32_t prachId);
  virtual void UlCqiReport (NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi);
  virtual void UlInfoListElementHarqFeeback (NistUlInfoListElement_s params);
  virtual void DlInfoListElementHarqFeeback (NistDlInfoListElement_s params);

private:
  NistLteEnbMac* m_mac;
};

NistEnbMacMemberLteEnbPhySapUser::NistEnbMacMemberLteEnbPhySapUser (NistLteEnbMac* mac) : m_mac (mac)
{
}


void
NistEnbMacMemberLteEnbPhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p);
}

void
NistEnbMacMemberLteEnbPhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  m_mac->DoSubframeIndication (frameNo, subframeNo);
}

void
NistEnbMacMemberLteEnbPhySapUser::ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
  m_mac->DoReceiveNistLteControlMessage (msg);
}

void
NistEnbMacMemberLteEnbPhySapUser::ReceiveRachPreamble (uint32_t prachId)
{
  m_mac->DoReceiveRachPreamble (prachId);
}

void
NistEnbMacMemberLteEnbPhySapUser::UlCqiReport (NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi)
{
  m_mac->DoUlCqiReport (ulcqi);
}

void
NistEnbMacMemberLteEnbPhySapUser::UlInfoListElementHarqFeeback (NistUlInfoListElement_s params)
{
  m_mac->DoUlInfoListElementHarqFeeback (params);
}

void
NistEnbMacMemberLteEnbPhySapUser::DlInfoListElementHarqFeeback (NistDlInfoListElement_s params)
{
  m_mac->DoDlInfoListElementHarqFeeback (params);
}


// //////////////////////////////////////
// generic NistLteEnbMac methods
// //////////////////////////////////////


TypeId
NistLteEnbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteEnbMac")
    .SetParent<Object> ()
    .AddConstructor<NistLteEnbMac> ()
    .AddAttribute ("NumberOfRaPreambles",
                   "how many random access preambles are available for the contention based RACH process",
                   UintegerValue (50),
                   MakeUintegerAccessor (&NistLteEnbMac::m_numberOfRaPreambles),
                   MakeUintegerChecker<uint8_t> (4, 64))
    .AddAttribute ("PreambleTransMax",
                   "Maximum number of random access preamble transmissions",
                   UintegerValue (50),
                   MakeUintegerAccessor (&NistLteEnbMac::m_preambleTransMax),
                   MakeUintegerChecker<uint8_t> (3, 200))
    .AddAttribute ("RaResponseWindowSize",
                   "length of the window (in TTIs) for the reception of the random access response (RAR); the resulting RAR timeout is this value + 3 ms",
                   UintegerValue (3),
                   MakeUintegerAccessor (&NistLteEnbMac::m_raResponseWindowSize),
                   MakeUintegerChecker<uint8_t> (2, 10))
    .AddTraceSource ("DlScheduling",
                     "Information regarding DL scheduling.",
                     MakeTraceSourceAccessor (&NistLteEnbMac::m_dlScheduling),
                     "ns3::NistLteEnbMac::DlSchedulingTracedCallback")
    .AddTraceSource ("UlScheduling",
                     "Information regarding UL scheduling.",
                     MakeTraceSourceAccessor (&NistLteEnbMac::m_ulScheduling),
                     "ns3::NistLteEnbMac::UlSchedulingTracedCallback")
  ;

  return tid;
}


NistLteEnbMac::NistLteEnbMac ()
{
  NS_LOG_FUNCTION (this);
  m_macSapProvider = new NistEnbMacMemberLteMacSapProvider<NistLteEnbMac> (this);
  m_cmacSapProvider = new NistEnbMacMemberLteEnbCmacSapProvider (this);
  m_schedSapUser = new NistEnbMacMemberFfMacSchedSapUser (this);
  m_cschedSapUser = new NistEnbMacMemberFfMacCschedSapUser (this);
  m_enbPhySapUser = new NistEnbMacMemberLteEnbPhySapUser (this);
}


NistLteEnbMac::~NistLteEnbMac ()
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_dlCqiReceived.clear ();
  m_ulCqiReceived.clear ();
  m_ulCeReceived.clear ();
  m_dlInfoListReceived.clear ();
  m_ulInfoListReceived.clear ();
  m_miDlHarqProcessesPackets.clear ();
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_schedSapUser;
  delete m_cschedSapUser;
  delete m_enbPhySapUser;
}


void
NistLteEnbMac::SetNistFfMacSchedSapProvider (NistFfMacSchedSapProvider* s)
{
  m_schedSapProvider = s;
}

NistFfMacSchedSapUser*
NistLteEnbMac::GetNistFfMacSchedSapUser (void)
{
  return m_schedSapUser;
}

void
NistLteEnbMac::SetNistFfMacCschedSapProvider (NistFfMacCschedSapProvider* s)
{
  m_cschedSapProvider = s;
}

NistFfMacCschedSapUser*
NistLteEnbMac::GetNistFfMacCschedSapUser (void)
{
  return m_cschedSapUser;
}



void
NistLteEnbMac::SetNistLteMacSapUser (NistLteMacSapUser* s)
{
  m_macSapUser = s;
}

NistLteMacSapProvider*
NistLteEnbMac::GetNistLteMacSapProvider (void)
{
  return m_macSapProvider;
}

void
NistLteEnbMac::SetNistLteEnbCmacSapUser (NistLteEnbCmacSapUser* s)
{
  m_cmacSapUser = s;
}

NistLteEnbCmacSapProvider*
NistLteEnbMac::GetNistLteEnbCmacSapProvider (void)
{
  return m_cmacSapProvider;
}

void
NistLteEnbMac::SetNistLteEnbPhySapProvider (NistLteEnbPhySapProvider* s)
{
  m_enbPhySapProvider = s;
}


NistLteEnbPhySapUser*
NistLteEnbMac::GetNistLteEnbPhySapUser ()
{
  return m_enbPhySapUser;
}



void
NistLteEnbMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << " EnbMac - frame " << frameNo << " subframe " << subframeNo);

  // Store current frame / subframe number
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;


  // --- DOWNLINK ---
  // Send Dl-CQI info to the scheduler
  if (m_dlCqiReceived.size () > 0)
    {
      NistFfMacSchedSapProvider::NistSchedDlCqiInfoReqParameters dlcqiInfoReq;
      dlcqiInfoReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);

      int cqiNum = m_dlCqiReceived.size ();
      if (cqiNum > MAX_CQI_LIST)
        {
          cqiNum = MAX_CQI_LIST;
        }
      dlcqiInfoReq.m_cqiList.insert (dlcqiInfoReq.m_cqiList.begin (), m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
      m_dlCqiReceived.erase (m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
      m_schedSapProvider->SchedDlCqiInfoReq (dlcqiInfoReq);
    }

  if (!m_receivedRachPreambleCount.empty ())
    {
      // process received RACH preambles and notify the scheduler
      NistFfMacSchedSapProvider::NistSchedDlRachInfoReqParameters rachInfoReqParams;
      NS_ASSERT (subframeNo > 0 && subframeNo <= 10); // subframe in 1..10
      for (std::map<uint8_t, uint32_t>::const_iterator it = m_receivedRachPreambleCount.begin ();
           it != m_receivedRachPreambleCount.end ();
           ++it)
        {
          NS_LOG_INFO (this << " preambleId " << (uint32_t) it->first << ": " << it->second << " received");
          NS_ASSERT (it->second != 0);
          if (it->second > 1)
            {
              NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": collision");
              // in case of collision we assume that no preamble is
              // successfully received, hence no RAR is sent 
            }
          else
            {
              uint16_t rnti;
              std::map<uint8_t, NistNcRaNistPreambleInfo>::iterator jt = m_allocatedNcRaPreambleMap.find (it->first);
              if (jt != m_allocatedNcRaPreambleMap.end ())
                {
                  rnti = jt->second.rnti;
                  NS_LOG_INFO ("preambleId previously allocated for NC based RA, RNTI =" << (uint32_t) rnti << ", sending RAR");
                  
                }
              else
                {
                  rnti = m_cmacSapUser->AllocateTemporaryCellRnti ();
                  NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": allocated T-C-RNTI " << (uint32_t) rnti << ", sending RAR");
                }

              NistRachListElement_s rachLe;
              rachLe.m_rnti = rnti;
              rachLe.m_estimatedSize = 144; // to be confirmed
              rachInfoReqParams.m_rachList.push_back (rachLe);
              m_rapIdRntiMap.insert (std::pair <uint16_t, uint32_t> (rnti, it->first));
            }
        }
      m_schedSapProvider->SchedDlRachInfoReq (rachInfoReqParams);
      m_receivedRachPreambleCount.clear ();
    }
  // Get downlink transmission opportunities
  uint32_t dlSchedFrameNo = m_frameNo;
  uint32_t dlSchedSubframeNo = m_subframeNo;
  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
  if (dlSchedSubframeNo + m_macChTtiDelay > 10)
    {
      dlSchedFrameNo++;
      dlSchedSubframeNo = (dlSchedSubframeNo + m_macChTtiDelay) % 10;
    }
  else
    {
      dlSchedSubframeNo = dlSchedSubframeNo + m_macChTtiDelay;
    }
  NistFfMacSchedSapProvider::NistSchedDlTriggerReqParameters dlparams;
  dlparams.m_sfnSf = ((0x3FF & dlSchedFrameNo) << 4) | (0xF & dlSchedSubframeNo);

  // Forward DL HARQ feebacks collected during last TTI
  if (m_dlInfoListReceived.size () > 0)
    {
      dlparams.m_dlInfoList = m_dlInfoListReceived;
      // empty local buffer
      m_dlInfoListReceived.clear ();
    }

  m_schedSapProvider->SchedDlTriggerReq (dlparams);


  // --- UPLINK ---
  // Send UL-CQI info to the scheduler
  std::vector <NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters>::iterator itCqi;
  for (uint16_t i = 0; i < m_ulCqiReceived.size (); i++)
    {
      if (subframeNo > 1)
        {        
          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & (subframeNo - 1));
        }
      else
        {
          m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & (frameNo - 1)) << 4) | (0xF & 10);
        }
      m_schedSapProvider->SchedUlCqiInfoReq (m_ulCqiReceived.at (i));
    }
    m_ulCqiReceived.clear ();
  
  // Send BSR reports to the scheduler
  if (m_ulCeReceived.size () > 0)
    {
      NistFfMacSchedSapProvider::NistSchedUlMacCtrlInfoReqParameters ulMacReq;
      ulMacReq.m_sfnSf = ((0x3FF & frameNo) << 4) | (0xF & subframeNo);
      ulMacReq.m_macCeList.insert (ulMacReq.m_macCeList.begin (), m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_ulCeReceived.erase (m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_schedSapProvider->SchedUlMacCtrlInfoReq (ulMacReq);
    }


  // Get uplink transmission opportunities
  uint32_t ulSchedFrameNo = m_frameNo;
  uint32_t ulSchedSubframeNo = m_subframeNo;
  //   NS_LOG_DEBUG (this << " sfn " << frameNo << " sbfn " << subframeNo);
  if (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY) > 10)
    {
      ulSchedFrameNo++;
      ulSchedSubframeNo = (ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY)) % 10;
    }
  else
    {
      ulSchedSubframeNo = ulSchedSubframeNo + (m_macChTtiDelay + UL_PUSCH_TTIS_DELAY);
    }
  NistFfMacSchedSapProvider::NistSchedUlTriggerReqParameters ulparams;
  ulparams.m_sfnSf = ((0x3FF & ulSchedFrameNo) << 4) | (0xF & ulSchedSubframeNo);

  // Forward DL HARQ feebacks collected during last TTI
  if (m_ulInfoListReceived.size () > 0)
    {
     ulparams.m_ulInfoList = m_ulInfoListReceived;
      // empty local buffer
      m_ulInfoListReceived.clear ();
    }

  m_schedSapProvider->SchedUlTriggerReq (ulparams);

}


void
NistLteEnbMac::DoReceiveNistLteControlMessage  (Ptr<NistLteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);
  if (msg->GetMessageType () == NistLteControlMessage::DL_CQI)
    {
      Ptr<NistDlCqiLteControlMessage> dlcqi = DynamicCast<NistDlCqiLteControlMessage> (msg);
      ReceiveNistDlCqiLteControlMessage (dlcqi);
    }
  else if (msg->GetMessageType () == NistLteControlMessage::BSR)
    {
      Ptr<NistBsrLteControlMessage> bsr = DynamicCast<NistBsrLteControlMessage> (msg);
      ReceiveBsrMessage (bsr->GetBsr ());
    }
  else if (msg->GetMessageType () == NistLteControlMessage::DL_HARQ)
    {
      Ptr<NistDlHarqFeedbackLteControlMessage> dlharq = DynamicCast<NistDlHarqFeedbackLteControlMessage> (msg);
      DoDlInfoListElementHarqFeeback (dlharq->GetDlHarqFeedback ());
    }
  else
    {
      NS_LOG_LOGIC (this << " NistLteControlMessage type " << msg->GetMessageType () << " not recognized");
    }
}

void
NistLteEnbMac::DoReceiveRachPreamble  (uint8_t rapId)
{
  NS_LOG_FUNCTION (this << (uint32_t) rapId);
  // just record that the preamble has been received; it will be processed later
  ++m_receivedRachPreambleCount[rapId]; // will create entry if not exists
}

void
NistLteEnbMac::DoUlCqiReport (NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi)
{ 
  if (ulcqi.m_ulCqi.m_type == NistUlCqi_s::PUSCH)
    {
      NS_LOG_DEBUG (this << " eNB rxed an PUSCH UL-CQI");
    }
  else if (ulcqi.m_ulCqi.m_type == NistUlCqi_s::SRS)
    {
      NS_LOG_DEBUG (this << " eNB rxed an SRS UL-CQI");
    }
  m_ulCqiReceived.push_back (ulcqi);
}


void
NistLteEnbMac::ReceiveNistDlCqiLteControlMessage  (Ptr<NistDlCqiLteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);

  NistCqiListElement_s dlcqi = msg->GetDlCqi ();
  NS_LOG_LOGIC (this << "Enb Received DL-CQI rnti" << dlcqi.m_rnti);
  NS_ASSERT (dlcqi.m_rnti != 0);
  m_dlCqiReceived.push_back (dlcqi);

}


void
NistLteEnbMac::ReceiveBsrMessage  (NistMacCeListElement_s bsr)
{
  NS_LOG_FUNCTION (this);

  //store BSR, both regular and sidelink
  m_ulCeReceived.push_back (bsr);
}


void
NistLteEnbMac::DoReceivePhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  NistLteRadioBearerTag tag;
  p->RemovePacketTag (tag);

  // store info of the packet received

//   std::map <uint16_t,NistUlInfoListElement_s>::iterator it;
//   u_int rnti = tag.GetRnti ();
//  u_int lcid = tag.GetLcid ();
//   it = m_ulInfoListElements.find (tag.GetRnti ());
//   if (it == m_ulInfoListElements.end ())
//     {
//       // new RNTI
//       NistUlInfoListElement_s ulinfonew;
//       ulinfonew.m_rnti = tag.GetRnti ();
//       // always allocate full size of ulReception vector, initializing all elements to 0
//       ulinfonew.m_ulReception.assign (MAX_LC_LIST+1, 0);
//       // set the element for the current LCID
//       ulinfonew.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       ulinfonew.m_receptionNistStatus = NistUlInfoListElement_s::Ok;
//       ulinfonew.m_tpc = 0; // Tx power control not implemented at this stage
//       m_ulInfoListElements.insert (std::pair<uint16_t, NistUlInfoListElement_s > (tag.GetRnti (), ulinfonew));
// 
//     }
//   else
//     {
//       // existing RNTI: we just set the value for the current
//       // LCID. Note that the corresponding element had already been
//       // allocated previously.
//       NS_ASSERT_MSG ((*it).second.m_ulReception.at (tag.GetLcid ()) == 0, "would overwrite previously written ulReception element");
//       (*it).second.m_ulReception.at (tag.GetLcid ()) = p->GetSize ();
//       (*it).second.m_receptionNistStatus = NistUlInfoListElement_s::Ok;
//     }



  // forward the packet to the correspondent RLC
  uint16_t rnti = tag.GetRnti ();
  uint8_t lcid = tag.GetLcid ();
  std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
  std::map<uint8_t, NistLteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
  //NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);

  //Receive PDU only if LCID is found
  if (lcidIt != rntiIt->second.end ())
    {
      (*lcidIt).second->ReceivePdu (p);
    }
}



// ////////////////////////////////////////////
// CMAC SAP
// ////////////////////////////////////////////

void
NistLteEnbMac::DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
  NistFfMacCschedSapProvider::NistCschedCellConfigReqParameters params;
  // Configure the subset of parameters used by NistFfMacScheduler
  params.m_ulBandwidth = ulBandwidth;
  params.m_dlBandwidth = dlBandwidth;
  m_macChTtiDelay = m_enbPhySapProvider->GetMacChTtiDelay ();
  // ...more parameters can be configured
  m_cschedSapProvider->CschedCellConfigReq (params);
}


void
NistLteEnbMac::DoAddUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  std::map<uint8_t, NistLteMacSapUser*> empty;
  std::pair <std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> >::iterator, bool> 
    ret = m_rlcAttached.insert (std::pair <uint16_t,  std::map<uint8_t, NistLteMacSapUser*> > 
                                (rnti, empty));
  NS_ASSERT_MSG (ret.second, "element already present, RNTI already existed");

  NistFfMacCschedSapProvider::NistCschedNistUeConfigReqParameters params;
  params.m_rnti = rnti;
  params.m_transmissionMode = 0; // set to default value (SISO) for avoiding random initialization (valgrind error)
  m_cschedSapProvider->CschedNistUeConfigReq (params);

  // Create DL trasmission HARQ buffers
  std::vector < Ptr<PacketBurst> > dlHarqLayer0pkt;
  dlHarqLayer0pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer0pkt.at (i) = pb;
    }
  std::vector < Ptr<PacketBurst> > dlHarqLayer1pkt;
  dlHarqLayer1pkt.resize (8);
  for (uint8_t i = 0; i < 8; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      dlHarqLayer1pkt.at (i) = pb;
    }
  DlHarqProcessesBuffer_t buf;
  buf.push_back (dlHarqLayer0pkt);
  buf.push_back (dlHarqLayer1pkt);
  m_miDlHarqProcessesPackets.insert (std::pair <uint16_t, DlHarqProcessesBuffer_t> (rnti, buf));
}

void
NistLteEnbMac::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  NistFfMacCschedSapProvider::NistCschedUeReleaseReqParameters params;
  params.m_rnti = rnti;
  m_cschedSapProvider->CschedUeReleaseReq (params);
  m_rlcAttached.erase (rnti);
  m_miDlHarqProcessesPackets.erase (rnti);
}

void
NistLteEnbMac::DoAddLc (NistLteEnbCmacSapProvider::NistLcInfo lcinfo, NistLteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);

  std::map <NistLteFlowId_t, NistLteMacSapUser* >::iterator it;
  
  NistLteFlowId_t flow (lcinfo.rnti, lcinfo.lcId);
  
  std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (lcinfo.rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "RNTI not found");
  std::map<uint8_t, NistLteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcinfo.lcId);
  if (lcidIt == rntiIt->second.end ())
    {
      rntiIt->second.insert (std::pair<uint8_t, NistLteMacSapUser*> (lcinfo.lcId, msu));
    }
  else
    {
      NS_LOG_ERROR ("LC already exists");
    }

  // CCCH (LCID 0) is pre-configured 
  // see FF LTE MAC Scheduler
  // Interface Specification v1.11, 
  // 4.3.4 logicalChannelConfigListElement
  if (lcinfo.lcId != 0)
    {
      struct NistFfMacCschedSapProvider::NistCschedLcConfigReqParameters params;
      params.m_rnti = lcinfo.rnti;
      params.m_reconfigureFlag = false;

      struct NistLogicalChannelConfigListElement_s lccle;
      lccle.m_logicalChannelIdentity = lcinfo.lcId;
      lccle.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle.m_direction = NistLogicalChannelConfigListElement_s::DIR_BOTH;
      lccle.m_qosBearerType = lcinfo.isGbr ? NistLogicalChannelConfigListElement_s::QBT_GBR : NistLogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle.m_qci = lcinfo.qci;
      lccle.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params.m_logicalChannelConfigList.push_back (lccle);

      m_cschedSapProvider->CschedLcConfigReq (params);
    }
}

void
NistLteEnbMac::DoReconfigureLc (NistLteEnbCmacSapProvider::NistLcInfo lcinfo)
{
  NS_FATAL_ERROR ("not implemented");
}

void
NistLteEnbMac::DoReleaseLc (uint16_t rnti, uint8_t lcid)
{
  NS_LOG_FUNCTION (this);

  //Find user based on rnti and then erase lcid stored against the same
  std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  rntiIt->second.erase (lcid);

  struct NistFfMacCschedSapProvider::NistCschedLcReleaseReqParameters params;
  params.m_rnti = rnti;
  params.m_logicalChannelIdentity.push_back (lcid);
  m_cschedSapProvider->CschedLcReleaseReq (params);
}

void
NistLteEnbMac::DoUeUpdateConfigurationReq (NistLteEnbCmacSapProvider::NistUeConfig params)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  NistFfMacCschedSapProvider::NistCschedNistUeConfigReqParameters req;
  req.m_rnti = params.m_rnti;
  req.m_transmissionMode = params.m_transmissionMode;
  req.m_slDestinations = params.m_slDestinations;
  NS_LOG_DEBUG ("sidelink: adding " << params.m_slDestinations.size () << " destinations for UE with RNTI " << params.m_rnti);
  req.m_reconfigureFlag = true;
  m_cschedSapProvider->CschedNistUeConfigReq (req);
}

void
NistLteEnbMac::DoAddPool (uint32_t group, Ptr<SidelinkCommResourcePool> pool)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  NistFfMacCschedSapProvider::NistCschedPoolConfigReqParameters req;
  req.m_group = group;
  req.m_pool = pool;
  m_cschedSapProvider->CschedPoolConfigReq (req);
  
}

void
NistLteEnbMac::DoRemovePool (uint32_t group)
{
  NS_LOG_FUNCTION (this);

  // propagates to scheduler
  NistFfMacCschedSapProvider::NistCschedPoolReleaseReqParameters req;
  req.m_group = group;
  m_cschedSapProvider->CschedPoolReleaseReq (req);
}

  

NistLteEnbCmacSapProvider::NistRachConfig 
NistLteEnbMac::DoGetNistRachConfig ()
{
  struct NistLteEnbCmacSapProvider::NistRachConfig rc;
  rc.numberOfRaPreambles = m_numberOfRaPreambles;
  rc.preambleTransMax = m_preambleTransMax;
  rc.raResponseWindowSize = m_raResponseWindowSize;
  return rc;
}
 
NistLteEnbCmacSapProvider::NistAllocateNcRaPreambleReturnValue 
NistLteEnbMac::DoAllocateNcRaPreamble (uint16_t rnti)
{
  bool found = false;
  uint8_t preambleId;
  for (preambleId = m_numberOfRaPreambles; preambleId < 64; ++preambleId)
    {
      std::map<uint8_t, NistNcRaNistPreambleInfo>::iterator it = m_allocatedNcRaPreambleMap.find (preambleId);
      if ((it ==  m_allocatedNcRaPreambleMap.end ())
          || (it->second.expiryTime < Simulator::Now ()))
        {
          found = true;
          NistNcRaNistPreambleInfo preambleInfo;
          uint32_t expiryIntervalMs = (uint32_t) m_preambleTransMax * ((uint32_t) m_raResponseWindowSize + 5); 
          
          preambleInfo.expiryTime = Simulator::Now () + MilliSeconds (expiryIntervalMs);
          preambleInfo.rnti = rnti;
          NS_LOG_INFO ("allocated preamble for NC based RA: preamble " << preambleId << ", RNTI " << preambleInfo.rnti << ", exiryTime " << preambleInfo.expiryTime);
          m_allocatedNcRaPreambleMap[preambleId] = preambleInfo; // create if not exist, update otherwise
          break;
        }
    }
  NistLteEnbCmacSapProvider::NistAllocateNcRaPreambleReturnValue ret;
  if (found)
    {
      ret.valid = true;
      ret.raPreambleId = preambleId;
      ret.raPrachMaskIndex = 0;
    }
  else
    {
      ret.valid = false;
      ret.raPreambleId = 0;
      ret.raPrachMaskIndex = 0;
    }
  return ret;
}



// ////////////////////////////////////////////
// MAC SAP
// ////////////////////////////////////////////


void
NistLteEnbMac::DoTransmitPdu (NistLteMacSapProvider::NistTransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);
  NistLteRadioBearerTag tag (params.rnti, params.lcid, params.layer);
  params.pdu->AddPacketTag (tag);
  // Store pkt in HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  NS_LOG_DEBUG (this << " LAYER " << (uint16_t)tag.GetLayer () << " HARQ ID " << (uint16_t)params.harqProcessId);
  
  //(*it).second.at (params.layer).at (params.harqProcessId) = params.pdu;//->Copy ();
  (*it).second.at (params.layer).at (params.harqProcessId)->AddPacket (params.pdu);
  m_enbPhySapProvider->SendMacPdu (params.pdu);
}

void
NistLteEnbMac::DoReportBufferNistStatus (NistLteMacSapProvider::NistReportBufferNistStatusParameters params)
{
  NS_LOG_FUNCTION (this);
  NistFfMacSchedSapProvider::NistSchedDlRlcBufferReqParameters req;
  req.m_rnti = params.rnti;
  req.m_logicalChannelIdentity = params.lcid;
  req.m_rlcTransmissionQueueSize = params.txQueueSize;
  req.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  req.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  req.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  req.m_rlcNistStatusPduSize = params.statusPduSize;
  m_schedSapProvider->SchedDlRlcBufferReq (req);
}



// ////////////////////////////////////////////
// SCHED SAP
// ////////////////////////////////////////////



void
NistLteEnbMac::DoSchedDlConfigInd (NistFfMacSchedSapUser::NistSchedDlConfigIndParameters ind)
{
  NS_LOG_FUNCTION (this);
  // Create DL PHY PDU
  Ptr<PacketBurst> pb = CreateObject<PacketBurst> ();
  std::map <NistLteFlowId_t, NistLteMacSapUser* >::iterator it;

  for (unsigned int i = 0; i < ind.m_buildDataList.size (); i++)
    {
      for (uint16_t layer = 0; layer < ind.m_buildDataList.at (i).m_dci.m_ndi.size (); layer++)
        {
          if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (layer) == 1)
            {
              // new data -> force emptying correspondent harq pkt buffer
              std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
              NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
              for (uint16_t lcId = 0; lcId < (*it).second.size (); lcId++)
                {
                  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                  (*it).second.at (lcId).at (ind.m_buildDataList.at (i).m_dci.m_harqProcess) = pb;
                }
            }
        }
      for (unsigned int j = 0; j < ind.m_buildDataList.at (i).m_rlcPduList.size (); j++)
        {
          for (uint16_t k = 0; k < ind.m_buildDataList.at (i).m_rlcPduList.at (j).size (); k++)
            {
              if (ind.m_buildDataList.at (i).m_dci.m_ndi.at (k) == 1)
                {
                  // New Data -> retrieve it from RLC
                  uint16_t rnti = ind.m_buildDataList.at (i).m_rnti;
                  uint8_t lcid = ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_logicalChannelIdentity;
                  std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
                  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
                  std::map<uint8_t, NistLteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcid);
                  NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << lcid);
                  NS_LOG_DEBUG (this << " rnti= " << rnti << " lcid= " << (uint32_t) lcid << " layer= " << k);
                  (*lcidIt).second->NotifyTxOpportunity (ind.m_buildDataList.at (i).m_rlcPduList.at (j).at (k).m_size, k, ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                }
              else
                {
                  if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (k) > 0)
                    {
                      // HARQ retransmission -> retrieve TB from HARQ buffer
                      std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (ind.m_buildDataList.at (i).m_rnti);
                      NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
                      Ptr<PacketBurst> pb = (*it).second.at (k).at ( ind.m_buildDataList.at (i).m_dci.m_harqProcess);
                      for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                        {
                          Ptr<Packet> pkt = (*j)->Copy ();
                          m_enbPhySapProvider->SendMacPdu (pkt);
                        }
                    }
                }
            }
        }
      // send the relative DCI
      Ptr<NistDlDciLteControlMessage> msg = Create<NistDlDciLteControlMessage> ();
      msg->SetDci (ind.m_buildDataList.at (i).m_dci);
      m_enbPhySapProvider->SendNistLteControlMessage (msg);
    }

  // Fire the trace with the DL information
  for (  uint32_t i  = 0; i < ind.m_buildDataList.size (); i++ )
    {
      // Only one TB used
      if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 1)
        {
          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                          0, 0
                          );

        }
      // Two TBs used
      else if (ind.m_buildDataList.at (i).m_dci.m_tbsSize.size () == 2)
        {
          m_dlScheduling (m_frameNo, m_subframeNo, ind.m_buildDataList.at (i).m_dci.m_rnti,
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (0),
                          ind.m_buildDataList.at (i).m_dci.m_mcs.at (1),
                          ind.m_buildDataList.at (i).m_dci.m_tbsSize.at (1)
                          );
        }
      else
        {
          NS_FATAL_ERROR ("Found element with more than two transport blocks");
        }
    }

  // Random Access procedure: send RARs
  Ptr<NistRarLteControlMessage> rarMsg = Create<NistRarLteControlMessage> ();
  // see TS 36.321 5.1.4;  preambles were sent two frames ago
  // (plus 3GPP counts subframes from 0, not 1)
  uint16_t raRnti;
  if (m_subframeNo < 3)
    {
      raRnti = m_subframeNo + 7; // equivalent to +10-3
    }
  else
    {
      raRnti = m_subframeNo - 3;
    }
  rarMsg->SetRaRnti (raRnti);
  for (unsigned int i = 0; i < ind.m_buildNistRarList.size (); i++)
    {
      std::map <uint8_t, uint32_t>::iterator itRapId = m_rapIdRntiMap.find (ind.m_buildNistRarList.at (i).m_rnti);
      if (itRapId == m_rapIdRntiMap.end ())
        {
          NS_FATAL_ERROR ("Unable to find rapId of RNTI " << ind.m_buildNistRarList.at (i).m_rnti);
        }
      NistRarLteControlMessage::NistRar rar;
      rar.rapId = itRapId->second;
      rar.rarPayload = ind.m_buildNistRarList.at (i);
      rarMsg->AddNistRar (rar);
      NS_LOG_INFO (this << " Send RAR message to RNTI " << ind.m_buildNistRarList.at (i).m_rnti << " rapId " << itRapId->second);
    }
  if (ind.m_buildNistRarList.size () > 0)
    {
      m_enbPhySapProvider->SendNistLteControlMessage (rarMsg);
    }
  m_rapIdRntiMap.clear ();
}


void
NistLteEnbMac::DoSchedUlConfigInd (NistFfMacSchedSapUser::NistSchedUlConfigIndParameters ind)
{
  NS_LOG_FUNCTION (this);

  for (unsigned int i = 0; i < ind.m_dciList.size (); i++)
    {
      // send the correspondent ul dci
      Ptr<NistUlDciLteControlMessage> msg = Create<NistUlDciLteControlMessage> ();
      msg->SetDci (ind.m_dciList.at (i));
      m_enbPhySapProvider->SendNistLteControlMessage (msg);
    }
  
  // Fire the trace with the UL information
  for (  uint32_t i  = 0; i < ind.m_dciList.size (); i++ )
    {
      m_ulScheduling (m_frameNo, m_subframeNo, ind.m_dciList.at (i).m_rnti,
                      ind.m_dciList.at (i).m_mcs, ind.m_dciList.at (i).m_tbSize);
    }

  if (ind.m_sldciList.size () > 0) {
    NS_LOG_DEBUG ("Sending " << ind.m_sldciList.size () << " SL_DCI messages");
  }
  for (unsigned int i = 0; i < ind.m_sldciList.size (); i++)
    {
      NS_LOG_DEBUG ("i=" << i << " rnti=" << (uint32_t) (ind.m_sldciList.at (i).m_rnti));
      // send the correspondent sl dci
      Ptr<NistSlDciLteControlMessage> msg = Create<NistSlDciLteControlMessage> ();
      msg->SetDci (ind.m_sldciList.at (i));
      m_enbPhySapProvider->SendNistLteControlMessage (msg);
    }

}




// ////////////////////////////////////////////
// CSCHED SAP
// ////////////////////////////////////////////


void
NistLteEnbMac::DoCschedCellConfigCnf (NistFfMacCschedSapUser::NistCschedCellConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoCschedNistUeConfigCnf (NistFfMacCschedSapUser::NistCschedNistUeConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoCschedLcConfigCnf (NistFfMacCschedSapUser::NistCschedLcConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
  // Call the CSCHED primitive
  // m_cschedSap->LcConfigCompleted();
}

void
NistLteEnbMac::DoCschedLcReleaseCnf (NistFfMacCschedSapUser::NistCschedLcReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoCschedUeReleaseCnf (NistFfMacCschedSapUser::NistCschedUeReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoCschedNistUeConfigUpdateInd (NistFfMacCschedSapUser::NistCschedNistUeConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
  // propagates to RRC
  NistLteEnbCmacSapUser::NistUeConfig ueConfigUpdate;
  ueConfigUpdate.m_rnti = params.m_rnti;
  ueConfigUpdate.m_transmissionMode = params.m_transmissionMode;
  m_cmacSapUser->RrcConfigurationUpdateInd (ueConfigUpdate);
}

void
NistLteEnbMac::DoCschedCellConfigUpdateInd (NistFfMacCschedSapUser::NistCschedCellConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
NistLteEnbMac::DoUlInfoListElementHarqFeeback (NistUlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this);
  m_ulInfoListReceived.push_back (params);
}

void
NistLteEnbMac::DoDlInfoListElementHarqFeeback (NistDlInfoListElement_s params)
{
  NS_LOG_FUNCTION (this);
  // Update HARQ buffer
  std::map <uint16_t, DlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.m_rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
  for (uint8_t layer = 0; layer < params.m_harqNistStatus.size (); layer++)
    {
      if (params.m_harqNistStatus.at (layer) == NistDlInfoListElement_s::ACK)
        {
          // discard buffer
          Ptr<PacketBurst> emptyBuf = CreateObject <PacketBurst> ();
          (*it).second.at (layer).at (params.m_harqProcessId) = emptyBuf;
          NS_LOG_DEBUG (this << " HARQ-ACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else if (params.m_harqNistStatus.at (layer) == NistDlInfoListElement_s::NACK)
        {
          NS_LOG_DEBUG (this << " HARQ-NACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId << " layer " << (uint16_t)layer);
        }
      else
        {
          NS_FATAL_ERROR (" HARQ functionality not implemented");
        }
    }
  m_dlInfoListReceived.push_back (params);
}


} // namespace ns3
