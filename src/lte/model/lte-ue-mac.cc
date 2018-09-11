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
 * Author: Nicola Baldo  <nbaldo@cttc.es>
 * Author: Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */



#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include <ns3/random-variable-stream.h>
#include <ns3/boolean.h>
#include <ns3/string.h>
#include <ns3/enum.h>

#include "lte-ue-mac.h"
#include "lte-ue-net-device.h"
#include "lte-radio-bearer-tag.h"
#include <ns3/ff-mac-common.h>
#include <ns3/lte-control-messages.h>
#include <ns3/simulator.h>
#include <ns3/lte-common.h>

#include<bitset>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteUeMac");

NS_OBJECT_ENSURE_REGISTERED (LteUeMac);


///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////

/// UeMemberLteUeCmacSapProvider class
class UeMemberLteUeCmacSapProvider : public LteUeCmacSapProvider
{
public:
  /**
   * Constructor
   *
   * \param mac the UE MAC
   */
  UeMemberLteUeCmacSapProvider (LteUeMac* mac);

  //inherited from LteUeCmacSapProvider
  virtual void ConfigureRach (RachConfig rc);
  virtual void StartContentionBasedRandomAccessProcedure ();
  virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask);
  virtual void SetRnti (uint16_t rnti);
  virtual void AddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  virtual void RemoveLc (uint8_t lcId);
  virtual void Reset ();
  virtual void AddSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  virtual void RemoveSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);
  //Sidelink Communication
  virtual void AddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool);
  virtual void RemoveSlCommTxPool (uint32_t dstL2Id);
  virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
  virtual void AddSlDestination (uint32_t destination);
  virtual void RemoveSlDestination (uint32_t destination);
  //Sidelink Discovery
  virtual void SetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool);
  virtual void RemoveSlDiscTxPool ();
  virtual void SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);
  virtual void ModifyDiscTxApps (std::list<uint32_t> apps);
  virtual void ModifyDiscRxApps (std::list<uint32_t> apps);


private:
  LteUeMac* m_mac; ///< the UE MAC
};


UeMemberLteUeCmacSapProvider::UeMemberLteUeCmacSapProvider (LteUeMac* mac)
  : m_mac (mac)
{
}

void 
UeMemberLteUeCmacSapProvider::ConfigureRach (RachConfig rc)
{
  m_mac->DoConfigureRach (rc);
}

  void 
UeMemberLteUeCmacSapProvider::StartContentionBasedRandomAccessProcedure ()
{
  m_mac->DoStartContentionBasedRandomAccessProcedure ();
}

 void 
UeMemberLteUeCmacSapProvider::StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
  m_mac->DoStartNonContentionBasedRandomAccessProcedure (rnti, preambleId, prachMask);
}

 void
 UeMemberLteUeCmacSapProvider::SetRnti (uint16_t rnti)
 {
   m_mac->DoSetRnti (rnti);
 }

void
UeMemberLteUeCmacSapProvider::AddLc (uint8_t lcId, LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  m_mac->DoAddLc (lcId, lcConfig, msu);
}

void
UeMemberLteUeCmacSapProvider::RemoveLc (uint8_t lcid)
{
  m_mac->DoRemoveLc (lcid);
}

void
UeMemberLteUeCmacSapProvider::Reset ()
{
  m_mac->DoReset ();
}

void
UeMemberLteUeCmacSapProvider::AddSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  m_mac->DoAddSlLc (lcId, srcL2Id, dstL2Id, lcConfig, msu);
}

void
UeMemberLteUeCmacSapProvider::RemoveSlLc (uint8_t lcid, uint32_t srcL2Id, uint32_t dstL2Id)
{
  m_mac->DoRemoveSlLc (lcid, srcL2Id, dstL2Id);
}

void
UeMemberLteUeCmacSapProvider::AddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool)
{
  m_mac->DoAddSlCommTxPool (dstL2Id, pool);
}

void
UeMemberLteUeCmacSapProvider::RemoveSlCommTxPool (uint32_t dstL2Id)
{
  m_mac->DoRemoveSlCommTxPool (dstL2Id);
}

void
UeMemberLteUeCmacSapProvider::SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
  m_mac->DoSetSlCommRxPools (pools);
}

void
UeMemberLteUeCmacSapProvider::AddSlDestination (uint32_t destination)
{
  m_mac->DoAddSlDestination (destination);
}

void
UeMemberLteUeCmacSapProvider::RemoveSlDestination (uint32_t destination)
{
  m_mac->DoRemoveSlDestination (destination);
}

void
UeMemberLteUeCmacSapProvider::SetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
  m_mac->DoSetSlDiscTxPool (pool);
}

void
UeMemberLteUeCmacSapProvider::RemoveSlDiscTxPool ()
{
  m_mac->DoRemoveSlDiscTxPool ();
}

void
UeMemberLteUeCmacSapProvider::SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  m_mac->DoSetSlDiscRxPools (pools);
}

void
UeMemberLteUeCmacSapProvider::ModifyDiscTxApps (std::list<uint32_t> apps)
{
  m_mac->DoModifyDiscTxApps (apps);
}

void
UeMemberLteUeCmacSapProvider::ModifyDiscRxApps (std::list<uint32_t> apps)
{
  m_mac->DoModifyDiscRxApps (apps);
}


/// UeMemberLteMacSapProvider class
class UeMemberLteMacSapProvider : public LteMacSapProvider
{
public:
  /**
   * Constructor
   *
   * \param mac the UE MAC
   */
  UeMemberLteMacSapProvider (LteUeMac* mac);

  // inherited from LteMacSapProvider
  virtual void TransmitPdu (TransmitPduParameters params);
  virtual void ReportBufferStatus (ReportBufferStatusParameters params);

private:
  LteUeMac* m_mac; ///< the UE MAC
};


UeMemberLteMacSapProvider::UeMemberLteMacSapProvider (LteUeMac* mac)
  : m_mac (mac)
{
}

void
UeMemberLteMacSapProvider::TransmitPdu (TransmitPduParameters params)
{
  m_mac->DoTransmitPdu (params);
}


void
UeMemberLteMacSapProvider::ReportBufferStatus (ReportBufferStatusParameters params)
{
  m_mac->DoReportBufferStatus (params);
}



/**
 * UeMemberLteUePhySapUser
 */
class UeMemberLteUePhySapUser : public LteUePhySapUser
{
public:
  /**
   * Constructor
   *
   * \param mac the UE MAC
   */
  UeMemberLteUePhySapUser (LteUeMac* mac);

  // inherited from LtePhySapUser
  virtual void ReceivePhyPdu (Ptr<Packet> p);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  virtual void NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo);
  virtual void NotifySidelinkEnabled ();

private:
  LteUeMac* m_mac; ///< the UE MAC
};

UeMemberLteUePhySapUser::UeMemberLteUePhySapUser (LteUeMac* mac) : m_mac (mac)
{

}

void
UeMemberLteUePhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p);
}


void
UeMemberLteUePhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  m_mac->DoSubframeIndication (frameNo, subframeNo);
}

void
UeMemberLteUePhySapUser::ReceiveLteControlMessage (Ptr<LteControlMessage> msg)
{
  m_mac->DoReceiveLteControlMessage (msg);
}

void
UeMemberLteUePhySapUser::NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo)
{
  m_mac->DoNotifyChangeOfTiming (frameNo, subframeNo);
}

void
UeMemberLteUePhySapUser::NotifySidelinkEnabled ()
{
  m_mac->DoNotifySidelinkEnabled ();
}



//////////////////////////////////////////////////////////
// LteUeMac methods
///////////////////////////////////////////////////////////


TypeId
LteUeMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteUeMac")
    .SetParent<Object> ()
    .SetGroupName("Lte")
    .AddConstructor<LteUeMac> ()
    .AddAttribute ("Ktrp",
                   "Number of active subframes used for PSSCH in the TRP",
                   UintegerValue (4),
                   MakeUintegerAccessor (&LteUeMac::m_slKtrp),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("SetTrpIndex",
                   "The TRP index to be used.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&LteUeMac::m_setTrpIndex),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("UseSetTrp",
                   "Indicates if the set TRP index should be used.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&LteUeMac::m_useSetTrpIndex),
                   MakeBooleanChecker ())
    .AddAttribute ("SlGrantMcs",
                   "The MCS of the SL grant, must be [0..20] (default 0)",
                   UintegerValue (10),
                   MakeUintegerAccessor (&LteUeMac::m_slGrantMcs),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("SlGrantSize",
                   "The number of RBs allocated per UE for Sidelink (default 2)",
                   UintegerValue (2),
                   MakeUintegerAccessor (&LteUeMac::m_slGrantSize),
                   MakeUintegerChecker<uint8_t> ())
    .AddTraceSource ("SlPscchScheduling",
                     "Information regarding SL UE scheduling",
                     MakeTraceSourceAccessor (&LteUeMac::m_slPscchScheduling),
                     "ns3::SlUeMacStatParameters::TracedCallback")
    .AddTraceSource ("SlPsschScheduling",
                     "Information regarding SL Shared Channel UE scheduling",
                     MakeTraceSourceAccessor (&LteUeMac::m_slPsschScheduling),
                     "ns3::SlUeMacStatParameters::TracedCallback")
    .AddTraceSource ("DiscoveryAnnouncement",
                     "trace to track the announcement of discovery messages",
                     MakeTraceSourceAccessor (&LteUeMac::m_discoveryAnnouncementTrace),
                     "ns3::LteUeMac::DiscoveryAnnouncementTracedCallback")
    ;
  return tid;
}


LteUeMac::LteUeMac ()
  :  m_bsrPeriodicity (MilliSeconds (1)), // ideal behavior
     m_bsrLast (MilliSeconds (0)),
     m_freshUlBsr (false),
     m_harqProcessId (0),
     m_rnti (0),
     m_rachConfigured (false),
     m_waitingForRaResponse (false),
     m_slBsrPeriodicity (MilliSeconds (1)),
     m_slBsrLast (MilliSeconds (0)),
     m_freshSlBsr (false),
     m_setTrpIndex (0),
     m_useSetTrpIndex (false),
     m_slHasDataToTx (false),
     m_sidelinkEnabled (false)
  
{
  NS_LOG_FUNCTION (this);
  m_miUlHarqProcessesPacket.resize (HARQ_PERIOD);
  for (uint8_t i = 0; i < m_miUlHarqProcessesPacket.size (); i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      m_miUlHarqProcessesPacket.at (i) = pb;
    }
  m_miUlHarqProcessesPacketTimer.resize (HARQ_PERIOD, 0);
   
  m_macSapProvider = new UeMemberLteMacSapProvider (this);
  m_cmacSapProvider = new UeMemberLteUeCmacSapProvider (this);
  m_uePhySapUser = new UeMemberLteUePhySapUser (this);
  m_amc = CreateObject <LteAmc> ();
  m_raPreambleUniformVariable = CreateObject<UniformRandomVariable> ();
  m_ueSelectedUniformVariable = CreateObject<UniformRandomVariable> ();
  m_p1UniformVariable = CreateObject<UniformRandomVariable> ();
  m_resUniformVariable = CreateObject<UniformRandomVariable> ();
  m_componentCarrierId = 0;
  m_discTxPool.m_nextDiscPeriod.frameNo = 0;
  m_discTxPool.m_nextDiscPeriod.subframeNo = 0;
}


LteUeMac::~LteUeMac ()
{
  NS_LOG_FUNCTION (this);
}

void
LteUeMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_miUlHarqProcessesPacket.clear ();
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_uePhySapUser;
  Object::DoDispose ();
}


LteUePhySapUser*
LteUeMac::GetLteUePhySapUser (void)
{
  return m_uePhySapUser;
}

void
LteUeMac::SetLteUePhySapProvider (LteUePhySapProvider* s)
{
  m_uePhySapProvider = s;
}


LteMacSapProvider*
LteUeMac::GetLteMacSapProvider (void)
{
  return m_macSapProvider;
}

void
LteUeMac::SetLteUeCmacSapUser (LteUeCmacSapUser* s)
{
  m_cmacSapUser = s;
}

LteUeCmacSapProvider*
LteUeMac::GetLteUeCmacSapProvider (void)
{
  return m_cmacSapProvider;
}

void
LteUeMac::SetComponentCarrierId (uint8_t index)
{
  m_componentCarrierId = index;
}

void
LteUeMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_rnti == params.rnti, "RNTI mismatch between RLC and MAC");
  if (params.srcL2Id == 0)
    {
      LteRadioBearerTag tag (params.rnti, params.lcid, 0 /* UE works in SISO mode*/);
      params.pdu->AddPacketTag (tag);
      // store pdu in HARQ buffer
      m_miUlHarqProcessesPacket.at (m_harqProcessId)->AddPacket (params.pdu);
      m_miUlHarqProcessesPacketTimer.at (m_harqProcessId) = HARQ_PERIOD;
      m_uePhySapProvider->SendMacPdu (params.pdu);
    }
  else
    {
      NS_LOG_INFO ("Transmitting Sidelink PDU");
      //find transmitting pool
      std::map <uint32_t, PoolInfo>::iterator poolIt = m_sidelinkTxPoolsMap.find (params.dstL2Id);
      NS_ASSERT (poolIt!= m_sidelinkTxPoolsMap.end());

      LteRadioBearerTag tag (params.rnti, params.lcid, params.srcL2Id, params.dstL2Id);
      params.pdu->AddPacketTag (tag);
      // store pdu in HARQ buffer
      poolIt->second.m_miSlHarqProcessPacket->AddPacket (params.pdu);
      m_uePhySapProvider->SendMacPdu (params.pdu);
    }

}

void
LteUeMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this << (uint32_t) params.lcid);
  
  if (params.srcL2Id == 0)
    {
      NS_ASSERT (params.dstL2Id == 0);
      NS_LOG_INFO ("Reporting for uplink");
      std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters>::iterator it;


      it = m_ulBsrReceived.find (params.lcid);
      if (it != m_ulBsrReceived.end ())
        {
          // update entry
          (*it).second = params;
        }
      else
        {
          m_ulBsrReceived.insert (std::pair<uint8_t, LteMacSapProvider::ReportBufferStatusParameters> (params.lcid, params));
        }
      m_freshUlBsr = true;

    }
  else
    {
      NS_LOG_INFO ("Reporting for Sidelink. Tx Queue size = "<<params.txQueueSize);
      //Sidelink BSR
      std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator it;

      SidelinkLcIdentifier slLcId;
      slLcId.lcId = params.lcid;
      slLcId.srcL2Id = params.srcL2Id;
      slLcId.dstL2Id = params.dstL2Id;

      it = m_slBsrReceived.find (slLcId);
      if (it != m_slBsrReceived.end ())
        {
          // update entry
          (*it).second = params;
        }
      else
        {
          m_slBsrReceived.insert (std::pair<SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters> (slLcId, params));
        }
      m_freshSlBsr = true;
    }
}


void
LteUeMac::SendReportBufferStatus (void)
{
  NS_LOG_FUNCTION (this);

  if (m_rnti == 0)
    {
      NS_LOG_INFO ("MAC not initialized, BSR deferred");
      return; 
    }

  if (m_ulBsrReceived.size () == 0)
    {
      NS_LOG_INFO ("No BSR report to transmit");
      return; 
    }
  MacCeListElement_s bsr;
  bsr.m_rnti = m_rnti;
  bsr.m_macCeType = MacCeListElement_s::BSR;

  // BSR is reported for each LCG
  std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters>::iterator it;  
  std::vector<uint32_t> queue (4, 0); // one value per each of the 4 LCGs, initialized to 0
  for (it = m_ulBsrReceived.begin (); it != m_ulBsrReceived.end (); it++)
    {
      uint8_t lcid = it->first;
      std::map <uint8_t, LcInfo>::iterator lcInfoMapIt;
      lcInfoMapIt = m_lcInfoMap.find (lcid);
      NS_ASSERT (lcInfoMapIt !=  m_lcInfoMap.end ());
      NS_ASSERT_MSG ((lcid != 0) || (((*it).second.txQueueSize == 0)
                                     && ((*it).second.retxQueueSize == 0)
                                     && ((*it).second.statusPduSize == 0)),
                     "BSR should not be used for LCID 0");
      uint8_t lcg = lcInfoMapIt->second.lcConfig.logicalChannelGroup;
      queue.at (lcg) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
    }

  // FF API says that all 4 LCGs are always present
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (0)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (1)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (2)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (3)));

  // create the feedback to eNB
  Ptr<BsrLteControlMessage> msg = Create<BsrLteControlMessage> ();
  msg->SetBsr (bsr);
  m_uePhySapProvider->SendLteControlMessage (msg);
}

void
LteUeMac::SendSidelinkReportBufferStatus (void)
{
  NS_LOG_FUNCTION (this);
  if (m_rnti == 0)
    {
      NS_LOG_INFO ("MAC not initialized, BSR deferred");
      return;
    }
  //check if we have at scheduled pools
  bool scheduled = false;
  for (std::map <uint32_t, PoolInfo >::iterator slTxPoolIt = m_sidelinkTxPoolsMap.begin ();
                                                slTxPoolIt != m_sidelinkTxPoolsMap.end () && !scheduled; slTxPoolIt++)
    {
      if (slTxPoolIt->second.m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
        {
          scheduled = true;
        }
    }

  if (m_slBsrReceived.size () == 0 || !scheduled)
    {
      NS_LOG_INFO (this << " No SL BSR report to transmit. SL BSR size=" << m_slBsrReceived.size ());
      return;
    }

  MacCeListElement_s bsr;
  bsr.m_rnti = m_rnti;
  bsr.m_macCeType = MacCeListElement_s::SLBSR;

  // SL BSR is reported for each Group Index

  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator it;
  std::vector<uint32_t> queue (4, 0); //todo: change to create based on the number of destinations, initialized to 0

  for (it = m_slBsrReceived.begin (); it != m_slBsrReceived.end (); it++)
    {
      uint32_t dstL2Id = it->first.dstL2Id;
      std::map <SidelinkLcIdentifier, LcInfo>::iterator slLcInfoMapIt;
      slLcInfoMapIt = m_slLcInfoMap.find (it->first);
      NS_ASSERT (slLcInfoMapIt !=  m_slLcInfoMap.end ());
      //TODO: find the mapping between the destination and the group index (must be provided by RRC)
      std::map <uint32_t, PoolInfo >::iterator slTxPoolIt;
      slTxPoolIt = m_sidelinkTxPoolsMap.find (dstL2Id);
      Ptr<SidelinkTxCommResourcePool> pool = DynamicCast<SidelinkTxCommResourcePool> (slTxPoolIt->second.m_pool);
      NS_ASSERT (slTxPoolIt != m_sidelinkTxPoolsMap.end ());
      if (pool->GetSchedulingType() == SidelinkCommResourcePool::SCHEDULED)
        {
          NS_LOG_DEBUG ("Tx queue size = " << (*it).second.txQueueSize <<
                        " ReTx queue size = " << (*it).second.retxQueueSize <<
                        " Status PDU size = "<< (*it).second.statusPduSize);

          queue.at (pool->GetIndex()) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
        }
    }

  // store
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (0)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (1)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (2)));
  bsr.m_macCeValue.m_bufferStatus.push_back (BufferSizeLevelBsr::BufferSize2BsrId (queue.at (3)));

  // create the feedback to eNB
  Ptr<BsrLteControlMessage> msg = Create<BsrLteControlMessage> ();
  msg->SetBsr (bsr);
  m_uePhySapProvider->SendLteControlMessage (msg);
}

void 
LteUeMac::RandomlySelectAndSendRaPreamble ()
{
  NS_LOG_FUNCTION (this);
  // 3GPP 36.321 5.1.1  
  NS_ASSERT_MSG (m_rachConfigured, "RACH not configured");
  // assume that there is no Random Access Preambles group B
  m_raPreambleId = m_raPreambleUniformVariable->GetInteger (0, m_rachConfig.numberOfRaPreambles - 1);
  bool contention = true;
  SendRaPreamble (contention);
}
   
void
LteUeMac::SendRaPreamble (bool contention)
{
  NS_LOG_FUNCTION (this << (uint32_t) m_raPreambleId << contention);
  // Since regular UL LteControlMessages need m_ulConfigured = true in
  // order to be sent by the UE, the rach preamble needs to be sent
  // with a dedicated primitive (not
  // m_uePhySapProvider->SendLteControlMessage (msg)) so that it can
  // bypass the m_ulConfigured flag. This is reasonable, since In fact
  // the RACH preamble is sent on 6RB bandwidth so the uplink
  // bandwidth does not need to be configured. 
  NS_ASSERT (m_subframeNo > 0); // sanity check for subframe starting at 1
  m_raRnti = m_subframeNo - 1;
  m_uePhySapProvider->SendRachPreamble (m_raPreambleId, m_raRnti);
  NS_LOG_INFO (this << " sent preamble id " << (uint32_t) m_raPreambleId << ", RA-RNTI " << (uint32_t) m_raRnti);
  // 3GPP 36.321 5.1.4 
  Time raWindowBegin = MilliSeconds (3); 
  Time raWindowEnd = MilliSeconds (3 + m_rachConfig.raResponseWindowSize);
  Simulator::Schedule (raWindowBegin, &LteUeMac::StartWaitingForRaResponse, this);
  m_noRaResponseReceivedEvent = Simulator::Schedule (raWindowEnd, &LteUeMac::RaResponseTimeout, this, contention);
}

void 
LteUeMac::StartWaitingForRaResponse ()
{
   NS_LOG_FUNCTION (this);
   m_waitingForRaResponse = true;
}

void 
LteUeMac::RecvRaResponse (BuildRarListElement_s raResponse)
{
  NS_LOG_FUNCTION (this);
  m_waitingForRaResponse = false;
  m_noRaResponseReceivedEvent.Cancel ();
  NS_LOG_INFO ("got RAR for RAPID " << (uint32_t) m_raPreambleId << ", setting T-C-RNTI = " << raResponse.m_rnti);
  m_rnti = raResponse.m_rnti;
  m_cmacSapUser->SetTemporaryCellRnti (m_rnti);
  // in principle we should wait for contention resolution,
  // but in the current LTE model when two or more identical
  // preambles are sent no one is received, so there is no need
  // for contention resolution
  m_cmacSapUser->NotifyRandomAccessSuccessful ();
  // trigger tx opportunity for Message 3 over LC 0
  // this is needed since Message 3's UL GRANT is in the RAR, not in UL-DCIs
  const uint8_t lc0Lcid = 0;
  std::map <uint8_t, LcInfo>::iterator lc0InfoIt = m_lcInfoMap.find (lc0Lcid);
  NS_ASSERT (lc0InfoIt != m_lcInfoMap.end ());
  std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters>::iterator lc0BsrIt
    = m_ulBsrReceived.find (lc0Lcid);
  if ((lc0BsrIt != m_ulBsrReceived.end ())
      && (lc0BsrIt->second.txQueueSize > 0))
    {
      NS_ASSERT_MSG (raResponse.m_grant.m_tbSize > lc0BsrIt->second.txQueueSize, 
                     "segmentation of Message 3 is not allowed");
      // this function can be called only from primary carrier
      if (m_componentCarrierId > 0)
        {
          NS_FATAL_ERROR ("Function called on wrong componentCarrier");
        }
      lc0InfoIt->second.macSapUser->NotifyTxOpportunity (raResponse.m_grant.m_tbSize, 0, 0, m_componentCarrierId, m_rnti, lc0Lcid); 
      lc0BsrIt->second.txQueueSize = 0;
    }
}

void 
LteUeMac::RaResponseTimeout (bool contention)
{
  NS_LOG_FUNCTION (this << contention);
  m_waitingForRaResponse = false;
  // 3GPP 36.321 5.1.4
  ++m_preambleTransmissionCounter;
  if (m_preambleTransmissionCounter == m_rachConfig.preambleTransMax + 1)
    {
      NS_LOG_INFO ("RAR timeout, preambleTransMax reached => giving up");
      m_cmacSapUser->NotifyRandomAccessFailed ();
    }
  else
    {
      NS_LOG_INFO ("RAR timeout, re-send preamble");
      if (contention)
        {
          RandomlySelectAndSendRaPreamble ();
        }
      else
        {
          SendRaPreamble (contention);
        }
    }
}

void 
LteUeMac::DoConfigureRach (LteUeCmacSapProvider::RachConfig rc)
{
  NS_LOG_FUNCTION (this);
  m_rachConfig = rc;
  m_rachConfigured = true;
}

void 
LteUeMac::DoStartContentionBasedRandomAccessProcedure ()
{
  NS_LOG_FUNCTION (this);

  // 3GPP 36.321 5.1.1
  NS_ASSERT_MSG (m_rachConfigured, "RACH not configured");
  m_preambleTransmissionCounter = 0;
  m_backoffParameter = 0;
  RandomlySelectAndSendRaPreamble ();
}

void
LteUeMac::DoSetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  m_rnti = rnti;
}


void 
LteUeMac::DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
  NS_LOG_FUNCTION (this << " rnti" << rnti);
  NS_ASSERT_MSG (prachMask == 0, "requested PRACH MASK = " << (uint32_t) prachMask << ", but only PRACH MASK = 0 is supported");
  m_rnti = rnti;
  m_raPreambleId = preambleId;
  bool contention = false;
  SendRaPreamble (contention);
}

void
LteUeMac::DoAddLc (uint8_t lcId,  LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this << " lcId" << (uint32_t) lcId);
  NS_ASSERT_MSG (m_lcInfoMap.find (lcId) == m_lcInfoMap.end (), "cannot add channel because LCID " << (uint16_t)lcId << " is already present");
  
  LcInfo lcInfo;
  lcInfo.lcConfig = lcConfig;
  lcInfo.macSapUser = msu;
  m_lcInfoMap[lcId] = lcInfo;
}

void
LteUeMac::DoRemoveLc (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << " lcId" << lcId);
  NS_ASSERT_MSG (m_lcInfoMap.find (lcId) != m_lcInfoMap.end (), "could not find LCID " << lcId);
  m_lcInfoMap.erase (lcId);
}

void
LteUeMac::DoAddSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this << (uint32_t) lcId << srcL2Id << dstL2Id);
  SidelinkLcIdentifier slLcId;
  slLcId.lcId = lcId;
  slLcId.srcL2Id = srcL2Id;
  slLcId.dstL2Id = dstL2Id;

  NS_ASSERT_MSG (m_slLcInfoMap.find (slLcId) == m_slLcInfoMap.end (), "cannot add channel because LCID " << lcId
                                    << ", srcL2Id " << srcL2Id << ", dstL2Id " << dstL2Id << " is already present");

  LcInfo lcInfo;
  lcInfo.lcConfig = lcConfig;
  lcInfo.macSapUser = msu;
  m_slLcInfoMap[slLcId] = lcInfo;
}

void
LteUeMac::DoRemoveSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << " lcId" << lcId << ", srcL2Id=" << srcL2Id << ", dstL2Id" << dstL2Id);
  SidelinkLcIdentifier slLcId;
  slLcId.lcId = lcId;
  slLcId.srcL2Id = srcL2Id;
  slLcId.dstL2Id = dstL2Id;
  NS_ASSERT_MSG (m_slLcInfoMap.find (slLcId) != m_slLcInfoMap.end (), "could not find Sidelink LCID " << lcId);
  m_slLcInfoMap.erase (slLcId);
}

void
LteUeMac::DoReset ()
{
  NS_LOG_FUNCTION (this);
  std::map <uint8_t, LcInfo>::iterator it = m_lcInfoMap.begin ();
  while (it != m_lcInfoMap.end ())
    {
      // don't delete CCCH)
      if (it->first == 0)
        {          
          ++it;
        }
      else
        {
          // note: use of postfix operator preserves validity of iterator
          m_lcInfoMap.erase (it++);
        }
    }

  m_noRaResponseReceivedEvent.Cancel ();
  m_rachConfigured = false;
  m_freshUlBsr = false;
  m_ulBsrReceived.clear ();
}

void
LteUeMac::DoSetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
  NS_LOG_FUNCTION (this);
  //NS_ASSERT_MSG (m_discTxPools.m_pool != NULL, "Cannot add discovery transmission pool for " << m_rnti << ". Pool already exist for destination");
  DiscPoolInfo info;
  info.m_pool = pool;
  info.m_npsdch = info.m_pool->GetNPsdch();
  info.m_currentDiscPeriod.frameNo = 0; //init to 0 to make it invalid
  info.m_currentDiscPeriod.subframeNo = 0; //init to 0 to make it invalid
  info.m_nextDiscPeriod = info.m_pool->GetNextDiscPeriod (m_frameNo, m_subframeNo);
  //adjust because scheduler starts with frame/subframe = 1
  info.m_nextDiscPeriod.frameNo++;
  info.m_nextDiscPeriod.subframeNo++;
  NS_ABORT_MSG_IF (info.m_nextDiscPeriod.frameNo > 1024 || info.m_nextDiscPeriod.subframeNo > 10,
                   "Invalid frame or subframe number");

  info.m_grantReceived = false;
  m_discTxPool = info;
}

void
LteUeMac::DoRemoveSlDiscTxPool ()
{
  NS_LOG_FUNCTION (this);
  m_discTxPool.m_pool = NULL;
}

void
LteUeMac::DoSetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  NS_LOG_FUNCTION (this);
  m_discRxPools = pools;
}

void
LteUeMac::DoModifyDiscTxApps (std::list<uint32_t> apps)
{
  NS_LOG_FUNCTION (this);
  m_discTxApps = apps;
  m_uePhySapProvider->AddDiscTxApps (apps);
}

void
LteUeMac::DoModifyDiscRxApps (std::list<uint32_t> apps)
{
  NS_LOG_FUNCTION (this);
  m_discRxApps = apps;
  m_uePhySapProvider->AddDiscRxApps (apps);
}

void
LteUeMac::DoAddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool)
{
  NS_LOG_FUNCTION (this << dstL2Id << pool);
  std::map <uint32_t, PoolInfo >::iterator it;
  it = m_sidelinkTxPoolsMap.find (dstL2Id);
  NS_ASSERT_MSG (it == m_sidelinkTxPoolsMap.end (), "Cannot add Sidelink transmission pool for " << dstL2Id << ". Pool already exist for destination");
  PoolInfo info;
  info.m_pool = pool;
  info.m_npscch = info.m_pool->GetNPscch();
  info.m_currentScPeriod.frameNo = 0; //init to 0 to make it invalid
  info.m_currentScPeriod.subframeNo = 0; //init to 0 to make it invalid
  NS_LOG_DEBUG("frame no : "<< info.m_nextScPeriod.frameNo<<" Subframe no : "<<info.m_nextScPeriod.subframeNo);
  info.m_nextScPeriod = info.m_pool->GetNextScPeriod (m_frameNo, m_subframeNo);
  //adjust because scheduler starts with frame/subframe = 1
  info.m_nextScPeriod.frameNo++;
  info.m_nextScPeriod.subframeNo++;
  NS_ABORT_MSG_IF (info.m_nextScPeriod.frameNo > 1024 || info.m_nextScPeriod.subframeNo > 10,
                   "Invalid frame or subframe number");
  info.m_grantReceived = false;

  m_sidelinkTxPoolsMap.insert (std::pair<uint32_t, PoolInfo > (dstL2Id, info));
}

void
LteUeMac::DoRemoveSlCommTxPool (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);
  std::map <uint32_t, PoolInfo >::iterator it;
  it = m_sidelinkTxPoolsMap.find (dstL2Id);
  NS_ASSERT_MSG (it != m_sidelinkTxPoolsMap.end (), "Cannot remove Sidelink transmission pool for " << dstL2Id << ". Unknown destination");
  m_sidelinkTxPoolsMap.erase (dstL2Id);
}


void
LteUeMac::DoSetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
  NS_LOG_FUNCTION (this);
  m_sidelinkRxPools = pools;
}

void
LteUeMac::DoReceivePhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag;
  p->RemovePacketTag (tag);
  if (tag.GetSourceL2Id () == 0)
    {
      if (tag.GetRnti () == m_rnti)
        {
          // packet is for the current user
          std::map <uint8_t, LcInfo>::const_iterator it = m_lcInfoMap.find (tag.GetLcid ());
          if (it != m_lcInfoMap.end ())
            {
              it->second.macSapUser->ReceivePdu (p, m_rnti, tag.GetLcid ());
            }
          else
            {
              NS_LOG_WARN ("received packet with unknown lcid " << (uint32_t) tag.GetLcid ());
            }
        }
    }
  else
    {
      //Sidelink packet. Perform L2 filtering
      NS_LOG_INFO ("Received Sidelink packet");
      std::list <uint32_t>::iterator dstIt;
      bool found = false;
      for (dstIt = m_sidelinkDestinations.begin (); dstIt != m_sidelinkDestinations.end () ; dstIt++)
        {
          if ((*dstIt) == tag.GetDestinationL2Id ())
          {
            //the destination is a group we want to listen to
            SidelinkLcIdentifier identifier;
            identifier.lcId = tag.GetLcid ();
            identifier.srcL2Id = tag.GetSourceL2Id ();
            identifier.dstL2Id = tag.GetDestinationL2Id ();

            std::map <SidelinkLcIdentifier, LcInfo>::iterator it = m_slLcInfoMap.find (identifier);
            if (it == m_slLcInfoMap.end ())
              {
                //notify RRC to setup bearer
                m_cmacSapUser->NotifySidelinkReception (tag.GetLcid(), tag.GetSourceL2Id (), tag.GetDestinationL2Id ());

                //should be setup now
                it = m_slLcInfoMap.find (identifier);
                if (it == m_slLcInfoMap.end ())
                  {
                    NS_LOG_WARN ("Failure to setup Sidelink radio bearer");
                  }
              }
            it->second.macSapUser->ReceivePdu (p, m_rnti, tag.GetLcid ());
            found = true;
            break;
          }
        }
      if (!found)
        {
          NS_LOG_INFO ("received packet with unknown destination " << tag.GetDestinationL2Id ());
        }
    }
}


void
LteUeMac::DoReceiveLteControlMessage (Ptr<LteControlMessage> msg)
{
  NS_LOG_FUNCTION (this);
  if (msg->GetMessageType () == LteControlMessage::UL_DCI)
    {
      Ptr<UlDciLteControlMessage> msg2 = DynamicCast<UlDciLteControlMessage> (msg);
      UlDciListElement_s dci = msg2->GetDci ();
      if (dci.m_ndi == 1)
        {
          // New transmission -> empty pkt buffer queue (for deleting eventual pkts not acked )
          Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
          m_miUlHarqProcessesPacket.at (m_harqProcessId) = pb;
          // Retrieve data from RLC
          std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
          uint16_t activeLcs = 0;
          uint32_t statusPduMinSize = 0;
          for (itBsr = m_ulBsrReceived.begin (); itBsr != m_ulBsrReceived.end (); itBsr++)
            {
              if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
                {
                  activeLcs++;
                  if (((*itBsr).second.statusPduSize != 0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
                    {
                      statusPduMinSize = (*itBsr).second.statusPduSize;
                    }
                  if (((*itBsr).second.statusPduSize != 0)&&(statusPduMinSize == 0))
                    {
                      statusPduMinSize = (*itBsr).second.statusPduSize;
                    }
                }
            }
          if (activeLcs == 0)
            {
              NS_LOG_ERROR (this << " No active flows for this UL-DCI");
              return;
            }
          std::map <uint8_t, LcInfo>::iterator it;
          uint32_t bytesPerActiveLc = dci.m_tbSize / activeLcs;
          bool statusPduPriority = false;
          if ((statusPduMinSize != 0)&&(bytesPerActiveLc < statusPduMinSize))
            {
              // send only the status PDU which has highest priority
              statusPduPriority = true;
              NS_LOG_DEBUG (this << " Reduced resource -> send only Status, b ytes " << statusPduMinSize);
              if (dci.m_tbSize < statusPduMinSize)
                {
                  NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
                }
            }
          NS_LOG_LOGIC (this << " UE " << m_rnti << ": UL-CQI notified TxOpportunity of " << dci.m_tbSize << " => " << bytesPerActiveLc << " bytes per active LC" << " statusPduMinSize " << statusPduMinSize);
          for (it = m_lcInfoMap.begin (); it != m_lcInfoMap.end (); it++)
            {
              itBsr = m_ulBsrReceived.find ((*it).first);
              NS_LOG_DEBUG (this << " Processing LC " << (uint32_t)(*it).first << " bytesPerActiveLc " << bytesPerActiveLc);
              if ( (itBsr != m_ulBsrReceived.end ())
                   && ( ((*itBsr).second.statusPduSize > 0)
                        || ((*itBsr).second.retxQueueSize > 0)
                        || ((*itBsr).second.txQueueSize > 0)) )
                {
                  if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
                    {
                      (*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0, m_componentCarrierId, m_rnti, (*it).first);
                      NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " send  " << (*itBsr).second.statusPduSize << " status bytes to LC " << (uint32_t)(*it).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
                      (*itBsr).second.statusPduSize = 0;
                      break;
                    }
                  else
                    {
                      uint32_t bytesForThisLc = bytesPerActiveLc;
                      NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " bytes to LC " << (uint32_t)(*it).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
                      if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
                        {
                          (*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0, m_componentCarrierId, m_rnti, (*it).first);
                          bytesForThisLc -= (*itBsr).second.statusPduSize;
                          NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
                          (*itBsr).second.statusPduSize = 0;
                        }
                      else
                        {
                          if ((*itBsr).second.statusPduSize > bytesForThisLc)
                            {
                              NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
                            }
                        }
                        
                      if ((bytesForThisLc > 7)    // 7 is the min TxOpportunity useful for Rlc
                          && (((*itBsr).second.retxQueueSize > 0)
                              || ((*itBsr).second.txQueueSize > 0)))
                        {
                          if ((*itBsr).second.retxQueueSize > 0)
                            {
                              NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);
                              (*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0, m_componentCarrierId, m_rnti, (*it).first);
                              if ((*itBsr).second.retxQueueSize >= bytesForThisLc)
                                {
                                  (*itBsr).second.retxQueueSize -= bytesForThisLc;
                                }
                              else
                                {
                                  (*itBsr).second.retxQueueSize = 0;
                                }
                            }
                          else if ((*itBsr).second.txQueueSize > 0)
                            {
                              uint16_t lcid = (*it).first;
                              uint32_t rlcOverhead;
                              if (lcid == 1)
                                {
                                  // for SRB1 (using RLC AM) it's better to
                                  // overestimate RLC overhead rather than
                                  // underestimate it and risk unneeded
                                  // segmentation which increases delay 
                                  rlcOverhead = 4;                                  
                                }
                              else
                                {
                                  // minimum RLC overhead due to header
                                  rlcOverhead = 2;
                                }
                              NS_LOG_DEBUG (this << " serve tx DATA, bytes " << bytesForThisLc << ", RLC overhead " << rlcOverhead);
                              (*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0, m_componentCarrierId, m_rnti, (*it).first);
                              if ((*itBsr).second.txQueueSize >= bytesForThisLc - rlcOverhead)
                                {
                                  (*itBsr).second.txQueueSize -= bytesForThisLc - rlcOverhead;
                                }
                              else
                                {
                                  (*itBsr).second.txQueueSize = 0;
                                }
                            }
                        }
                      else
                        {
                          if ( ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0)) 
                            {
                              // resend BSR info for updating eNB peer MAC
                              m_freshUlBsr = true;
                            }
                        }
                      NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << "\t new queues " << (uint32_t)(*it).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
                    }

                }
            }
        }
      else
        {
          // HARQ retransmission -> retrieve data from HARQ buffer
          NS_LOG_DEBUG (this << " UE MAC RETX HARQ " << (uint16_t)m_harqProcessId);
          Ptr<PacketBurst> pb = m_miUlHarqProcessesPacket.at (m_harqProcessId);
          for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
            {
              Ptr<Packet> pkt = (*j)->Copy ();
              m_uePhySapProvider->SendMacPdu (pkt);
            }
          m_miUlHarqProcessesPacketTimer.at (m_harqProcessId) = HARQ_PERIOD;          
        }

    }
  else if (msg->GetMessageType () == LteControlMessage::RAR)
    {
      if (m_waitingForRaResponse)
        {
          Ptr<RarLteControlMessage> rarMsg = DynamicCast<RarLteControlMessage> (msg);
          uint16_t raRnti = rarMsg->GetRaRnti ();
          NS_LOG_LOGIC (this << "got RAR with RA-RNTI " << (uint32_t) raRnti << ", expecting " << (uint32_t) m_raRnti);
          if (raRnti == m_raRnti) // RAR corresponds to TX subframe of preamble
            {
              for (std::list<RarLteControlMessage::Rar>::const_iterator it = rarMsg->RarListBegin ();
                   it != rarMsg->RarListEnd ();
                   ++it)
                {
                  if (it->rapId == m_raPreambleId) // RAR is for me
                    {
                      RecvRaResponse (it->rarPayload);
                      /// \todo RRC generates the RecvRaResponse messaged
                      /// for avoiding holes in transmission at PHY layer
                      /// (which produce erroneous UL CQI evaluation)
                    }
                }
            }
        }
    }
  else if (msg->GetMessageType () == LteControlMessage::SL_DCI)
    {
      Ptr<SlDciLteControlMessage> msg2 = DynamicCast<SlDciLteControlMessage> (msg);
      SlDciListElement_s dci = msg2->GetDci ();

      //store the grant for the next SC period
      //TODO: distinguish SL grants for different pools. Right now just assume there is only one pool
      Ptr<SidelinkTxCommResourcePool> pool = DynamicCast<SidelinkTxCommResourcePool> (m_sidelinkTxPoolsMap.begin()->second.m_pool);
      NS_ASSERT (pool->GetSchedulingType() == SidelinkCommResourcePool::SCHEDULED);

      SidelinkGrant grant;
      grant.m_resPscch = dci.m_resPscch;
      grant.m_tpc = dci.m_tpc;
      grant.m_hopping = dci.m_hopping;
      grant.m_rbStart = dci.m_rbStart;
      grant.m_rbLen = dci.m_rbLen;
      grant.m_hoppingInfo = dci.m_hoppingInfo;
      grant.m_iTrp = dci.m_trp;
      grant.m_mcs = pool->GetMcs();
      grant.m_tbSize = 0; //computed later
      m_sidelinkTxPoolsMap.begin()->second.m_nextGrant = grant;
      m_sidelinkTxPoolsMap.begin()->second.m_grantReceived = true;

      NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
    }
  else if (msg->GetMessageType () == LteControlMessage::SL_DISC_MSG)
    {
      NS_LOG_INFO (this << " Received discovery message");
      //notify RRC (pass msg to RRC where we can filter)
      m_cmacSapUser->NotifyDiscoveryReception (msg);
    }
  else
    {
      NS_LOG_WARN (this << " LteControlMessage not recognized");
    }
}

void
LteUeMac::RefreshHarqProcessesPacketBuffer (void)
{
  NS_LOG_FUNCTION (this);

  for (uint16_t i = 0; i < m_miUlHarqProcessesPacketTimer.size (); i++)
    {
      if (m_miUlHarqProcessesPacketTimer.at (i) == 0)
        {
          if (m_miUlHarqProcessesPacket.at (i)->GetSize () > 0)
            {
              // timer expired: drop packets in buffer for this process
              NS_LOG_INFO (this << " HARQ Proc Id " << i << " packets buffer expired");
              Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
              m_miUlHarqProcessesPacket.at (i) = emptyPb;
            }
        }
      else
        {
          m_miUlHarqProcessesPacketTimer.at (i)--;
        }
    }
}


void
LteUeMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this);
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;
  RefreshHarqProcessesPacketBuffer ();
  if ((Simulator::Now () >= m_bsrLast + m_bsrPeriodicity) && (m_freshUlBsr == true))
    {
      if (m_componentCarrierId == 0)
        {
          //Send BSR through primary carrier
          SendReportBufferStatus ();
        }
      m_bsrLast = Simulator::Now ();
      m_freshUlBsr = false;
    }
  m_harqProcessId = (m_harqProcessId + 1) % HARQ_PERIOD;

  if (m_sidelinkEnabled)
    {
      //There is a delay between the MAC scheduling and the transmission so we assume that we are ahead.
      if (subframeNo + UL_PUSCH_TTIS_DELAY > 10)
        {
          frameNo++;
          if (frameNo > 1024)
            {
              frameNo = 1;
            }
          subframeNo = (subframeNo + UL_PUSCH_TTIS_DELAY) % 10;
        }
      else
        {
          subframeNo = subframeNo + UL_PUSCH_TTIS_DELAY;
        }

      NS_LOG_INFO ("Adjusted Frame no. " << frameNo << " Subframe no. " << subframeNo);

      //Sidelink Discovery
      if (m_discTxApps.size () > 0)
        {
          if (frameNo == m_discTxPool.m_nextDiscPeriod.frameNo && subframeNo == m_discTxPool.m_nextDiscPeriod.subframeNo)
            {
              //define periods and frames
              m_discTxPool.m_currentDiscPeriod = m_discTxPool.m_nextDiscPeriod;
              m_discTxPool.m_nextDiscPeriod = m_discTxPool.m_pool->GetNextDiscPeriod (frameNo, subframeNo);
              m_discTxPool.m_nextDiscPeriod.frameNo++;
              m_discTxPool.m_nextDiscPeriod.subframeNo++;
              NS_LOG_INFO ("Starting new discovery period " << ". Next period at " << m_discTxPool.m_nextDiscPeriod.frameNo << "/"
                                                                          << m_discTxPool.m_nextDiscPeriod.subframeNo);
              NS_ABORT_MSG_IF (m_discTxPool.m_nextDiscPeriod.frameNo > 1024 || m_discTxPool.m_nextDiscPeriod.subframeNo > 10,
                               "Invalid frame or subframe number");

              if (m_discTxPool.m_pool->GetSchedulingType () == SidelinkDiscResourcePool::UE_SELECTED)
                {
                  //use txProbability
                  DiscGrant grant;
                  double p1 = m_p1UniformVariable->GetValue (0, 1);
                  double txProbability = m_discTxPool.m_pool->GetTxProbability ();       //calculate txProbability
                  NS_LOG_DEBUG ("txProbability = " << txProbability << " % ");
                  if (p1 <= txProbability / 100.0)
                    {
                      grant.m_resPsdch = m_resUniformVariable->GetInteger (0, m_discTxPool.m_npsdch - 1);
                      grant.m_rnti = m_rnti;
                      m_discTxPool.m_nextGrant = grant;
                      m_discTxPool.m_grantReceived = true;
                      NS_LOG_INFO ("UE selected grant: resource =" << (uint16_t) grant.m_resPsdch << "/" << m_discTxPool.m_npsdch);
                    }
                }
              else   //scheduled
                {
                  //TODO
                  //use defined grant : SL-TF-IndexPair
                }

              //if we received a grant
              if (m_discTxPool.m_grantReceived)
                {
                  m_discTxPool.m_currentGrant = m_discTxPool.m_nextGrant;
                  NS_LOG_INFO ("Discovery grant received resource " << (uint32_t) m_discTxPool.m_currentGrant.m_resPsdch);

                  SidelinkDiscResourcePool::SubframeInfo tmp;
                  tmp.frameNo = m_discTxPool.m_currentDiscPeriod.frameNo - 1;
                  tmp.subframeNo = m_discTxPool.m_currentDiscPeriod.subframeNo - 1;

                  m_discTxPool.m_psdchTx = m_discTxPool.m_pool->GetPsdchTransmissions (m_discTxPool.m_currentGrant.m_resPsdch);

                  for (std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator txIt = m_discTxPool.m_psdchTx.begin ();
                       txIt != m_discTxPool.m_psdchTx.end (); txIt++)
                    {
                      txIt->subframe = txIt->subframe + tmp;
                      //adjust for index starting at 1
                      txIt->subframe.frameNo++;
                      txIt->subframe.subframeNo++;
                      NS_LOG_INFO ("PSDCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo
                                                      << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
                      NS_ABORT_MSG_IF (txIt->subframe.frameNo > 1024 || txIt->subframe.subframeNo > 10,
                                       "Invalid frame or subframe number");
                      //Inform PHY: find a way to inform the PHY layer of the resources
                      m_uePhySapProvider->SetDiscGrantInfo (m_discTxPool.m_currentGrant.m_resPsdch);
                      //clear the grant
                      m_discTxPool.m_grantReceived = false;
                    }
                }
            }
        }

      std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
      //check if we need to transmit PSDCH
      allocIt = m_discTxPool.m_psdchTx.begin ();
      if (allocIt != m_discTxPool.m_psdchTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
        {
          NS_LOG_INFO ("PSDCH transmission");
          for (std::list<uint32_t>::iterator txApp = m_discTxApps.begin (); txApp != m_discTxApps.end (); ++txApp)
            {
              //Create Discovery message for each discovery application announcing
              SlDiscMsg discMsg;
              discMsg.m_rnti = m_rnti;
              discMsg.m_resPsdch = m_discTxPool.m_currentGrant.m_resPsdch;

              discMsg.m_proSeAppCode =  (std::bitset <184>) * txApp;

              Ptr<SlDiscMessage> msg = Create<SlDiscMessage> ();
              msg->SetSlDiscMessage (discMsg);
              NS_LOG_INFO ("discovery message sent by " << m_rnti << ", proSeAppCode = " << *txApp);
              m_discoveryAnnouncementTrace (m_rnti, *txApp);
              m_uePhySapProvider->SendLteControlMessage (msg);
            }
          m_discTxPool.m_psdchTx.erase (allocIt);
        }

      //Sidelink Communication
      if ((Simulator::Now () >= m_slBsrLast + m_slBsrPeriodicity) && (m_freshSlBsr == true))
        {
          SendSidelinkReportBufferStatus ();
          m_slBsrLast = Simulator::Now ();
          m_freshSlBsr = false;
        }
      std::map <uint32_t, PoolInfo>::iterator poolIt;
      for (poolIt = m_sidelinkTxPoolsMap.begin (); poolIt != m_sidelinkTxPoolsMap.end (); poolIt++)
        {
          //Check if this is a new SC period
          if (frameNo == poolIt->second.m_nextScPeriod.frameNo && subframeNo == poolIt->second.m_nextScPeriod.subframeNo)
            {
              poolIt->second.m_currentScPeriod = poolIt->second.m_nextScPeriod;
              poolIt->second.m_nextScPeriod = poolIt->second.m_pool->GetNextScPeriod (frameNo, subframeNo);
              //adjust because scheduler starts with frame/subframe = 1
              poolIt->second.m_nextScPeriod.frameNo++;
              poolIt->second.m_nextScPeriod.subframeNo++;
              NS_LOG_INFO ("Starting new SC period for pool of group " << poolIt->first << ". Next period at "
                                                                       << poolIt->second.m_nextScPeriod.frameNo << "/" << poolIt->second.m_nextScPeriod.subframeNo);
              NS_ABORT_MSG_IF (poolIt->second.m_nextScPeriod.frameNo > 1024 || poolIt->second.m_nextScPeriod.subframeNo > 10,
                               "Invalid frame or subframe number");

              Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
              poolIt->second.m_miSlHarqProcessPacket = emptyPb;

              if (poolIt->second.m_pool->GetSchedulingType () == SidelinkCommResourcePool::UE_SELECTED)
                {
                  //If m_slHasDataToTx is False here (at the beginning of the period), it means
                  //that no transmissions in the PSSCH occurred in the previous SC period.
                  //Notify the RRC for stopping SLSS transmissions if appropriate

                  if (!m_slHasDataToTx)
                    {
                      m_cmacSapUser->NotifyMacHasNoSlDataToSend ();
                    }
                  //Make m_slHasDataToTx = false here (beginning of the period) to detect if transmissions
                  //in the PSSCH are performed in this period
                  m_slHasDataToTx = false;

                  //get the BSR for this pool
                  //if we have data in the queue
                  //find the BSR for that pool (will also give the SidleinkLcIdentifier)
                  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
                  for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
                    {
                      if (itBsr->first.dstL2Id == poolIt->first)
                        {
                          //this is the BSR for the pool
                          NS_LOG_DEBUG ("Found the BSR. Tx Queue size = "<<(*itBsr).second.txQueueSize);
                          break;
                        }
                    }
                  if (itBsr == m_slBsrReceived.end ()
                      || ( (*itBsr).second.txQueueSize == 0
                      && (*itBsr).second.retxQueueSize == 0
                      && (*itBsr).second.statusPduSize == 0 ))
                    {
                      NS_LOG_INFO ("No BSR received. Assume no data to transfer");
                    }
                  else
                    {
                      //we need to pick a random resource from the pool
                      NS_LOG_DEBUG ("SL BSR size =" << m_slBsrReceived.size ());
                      SidelinkGrant grant;
                      //in order to pick a resource that is valid, we compute the number of sub-channels
                      //on the PSSCH
                      uint16_t nbTxOpt = poolIt->second.m_npscch;
                      //Randomly selected Resource in PSCCH.
                      grant.m_resPscch = m_ueSelectedUniformVariable->GetInteger (0, nbTxOpt - 1);
                      grant.m_tpc = 0;
                      std::vector <uint8_t> validRbStarts = poolIt->second.m_pool->GetValidRBstart (m_slGrantSize);
                      NS_ABORT_MSG_IF (validRbStarts.size () == 0, "UE_MAC: No resources available for configured grant size!");
                      grant.m_rbStart = validRbStarts [m_ueSelectedUniformVariable->GetInteger (0, validRbStarts.size () - 1)];
                      grant.m_hoppingInfo = poolIt->second.m_pool->GetDataHoppingConfig ().hoppingInfo;
                      if (grant.m_hoppingInfo < 4)
                        {
                          NS_LOG_DEBUG("Hopping enabled");
                          grant.m_hopping = 1; //Hopping is enabled
                        }
                      else
                        {
                          NS_LOG_DEBUG("Hopping disabled");
                          grant.m_hopping = 0; //Hopping is disabled
                        }

                      switch (m_slKtrp)
                        {
                        case 1:
                          grant.m_iTrp = m_ueSelectedUniformVariable->GetInteger (0, 7);
                          break;
                        case 2:
                          grant.m_iTrp = m_ueSelectedUniformVariable->GetInteger (8, 35);
                          break;
                        case 4:
                          grant.m_iTrp = m_ueSelectedUniformVariable->GetInteger (36, 105);
                          break;
                        case 8:
                          grant.m_iTrp = 106;
                          break;
                        default:
                          NS_FATAL_ERROR ("Invalid KTRP value " << (uint16_t) m_slKtrp << ". Supported values: [1, 2, 4, 8]");
                        }

                      if (this->m_useSetTrpIndex == true)
                        {
                          grant.m_iTrp = this->m_setTrpIndex;
                        }
                      grant.m_rbLen = m_slGrantSize;
                      grant.m_mcs = m_slGrantMcs;
                      grant.m_tbSize = 0; //computed later
                      poolIt->second.m_nextGrant = grant;
                      poolIt->second.m_grantReceived = true;
                      NS_LOG_INFO ("UE selected grant: resource=" << (uint16_t) grant.m_resPscch << "/"
                                                                  << poolIt->second.m_npscch << ", rbStart=" << (uint16_t) grant.m_rbStart
                                                                  << ", rbLen=" << (uint16_t) grant.m_rbLen << ", mcs=" << (uint16_t) grant.m_mcs
                                                                  << ",itrp=" << (uint16_t) grant.m_iTrp);
                    }
                } // end if (UE_SELECTED)

              //if we received a grant, compute the transmission opportunities for PSCCH and PSSCH
              if (poolIt->second.m_grantReceived)
                {
                  if (poolIt->second.m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
                    {
                      std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
                      for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
                        {
                          if (itBsr->first.dstL2Id == poolIt->first)
                            {
                              //this is the BSR for the pool
                              NS_LOG_DEBUG ("Found the BSR. Tx Queue size = "<<(*itBsr).second.txQueueSize);
                              break;
                            }
                        }

                      if (itBsr == m_slBsrReceived.end ()
                          || ( (*itBsr).second.txQueueSize == 0
                          && (*itBsr).second.retxQueueSize == 0
                          && (*itBsr).second.statusPduSize == 0))
                        {
                          NS_LOG_INFO ("We receive the grant from the eNB but the transmission queue size is zero."
                                                                              " Assume no data to transfer for now");
                          continue;
                        }
                    }

                  //make the grant our current grant
                  poolIt->second.m_currentGrant = poolIt->second.m_nextGrant;
                  NS_LOG_INFO ("Sidelink grant received resource " << (uint32_t) poolIt->second.m_currentGrant.m_resPscch);
                  SidelinkCommResourcePool::SubframeInfo subFrameInfo;
                  subFrameInfo.frameNo = poolIt->second.m_currentScPeriod.frameNo - 1;
                  subFrameInfo.subframeNo = poolIt->second.m_currentScPeriod.subframeNo - 1;

                  // Collect statistics for SL UE mac scheduling trace
                  SlUeMacStatParameters stats_params;
                  stats_params.m_frameNo = subFrameInfo.frameNo + 1;
                  stats_params.m_subframeNo = subFrameInfo.subframeNo + 1;
                  stats_params.m_pscchRi = poolIt->second.m_currentGrant.m_resPscch;
                  stats_params.m_cellId = 0;
                  stats_params.m_imsi = 0;
                  stats_params.m_pscchFrame1 = 0;
                  stats_params.m_pscchSubframe1 = 0;
                  stats_params.m_pscchFrame2 = 0;
                  stats_params.m_pscchSubframe2 = 0;
                  stats_params.m_psschFrame = 0;
                  stats_params.m_psschSubframeStart = 0;

                  poolIt->second.m_pscchTx = poolIt->second.m_pool->GetPscchTransmissions (poolIt->second.m_currentGrant.m_resPscch);
                  uint16_t tx_counter = 1;
                  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->second.m_pscchTx.begin ();
                       txIt != poolIt->second.m_pscchTx.end (); txIt++)
                    {
                      txIt->subframe = txIt->subframe + subFrameInfo;
                      //adjust for index starting at 1
                      txIt->subframe.frameNo++;
                      txIt->subframe.subframeNo++;
                      NS_LOG_INFO ("PSCCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo
                                                      << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
                      NS_ABORT_MSG_IF (txIt->subframe.frameNo > 1024 || txIt->subframe.subframeNo > 10,
                                       "Invalid frame or subframe number");
                      switch (tx_counter)
                        {
                        case 1:
                          stats_params.m_pscchFrame1 = txIt->subframe.frameNo;
                          stats_params.m_pscchSubframe1 = txIt->subframe.subframeNo;
                          break;
                        case 2:
                          stats_params.m_pscchFrame2 = txIt->subframe.frameNo;
                          stats_params.m_pscchSubframe2 = txIt->subframe.subframeNo;
                          break;
                        default:
                          NS_FATAL_ERROR ("PSCCH ONLY SUPPORTS 2 TRANSMISSIONS PER UE GRANT!");
                        }
                      tx_counter++;
                    }
                  poolIt->second.m_psschTx = poolIt->second.m_pool->GetPsschTransmissions (subFrameInfo, poolIt->second.m_currentGrant.m_iTrp, poolIt->second.m_currentGrant.m_rbStart, poolIt->second.m_currentGrant.m_rbLen);

                  //adjust PSSCH frame to next period
                  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->second.m_psschTx.begin ();
                       txIt != poolIt->second.m_psschTx.end (); txIt++)
                    {
                      //adjust for index starting at 1
                      txIt->subframe.frameNo++;
                      txIt->subframe.subframeNo++;
                      NS_LOG_INFO ("PSSCH: Subframe " << txIt->subframe.frameNo << "/"
                                                      << txIt->subframe.subframeNo << ": rbStart=" << (uint32_t) txIt->rbStart
                                                      << ", rbLen=" << (uint32_t) txIt->nbRb);
                      NS_ABORT_MSG_IF (txIt->subframe.frameNo > 1024 || txIt->subframe.subframeNo > 10,
                                                             "Invalid frame or subframe number");
                    }

                  //compute the TB size
                  poolIt->second.m_currentGrant.m_tbSize = m_amc->GetUlTbSizeFromMcs (poolIt->second.m_currentGrant.m_mcs, poolIt->second.m_currentGrant.m_rbLen) / 8;
                  NS_LOG_INFO ("Sidelink Tb size = " << poolIt->second.m_currentGrant.m_tbSize << " bytes (mcs=" << (uint32_t) poolIt->second.m_currentGrant.m_mcs << ")");

                  stats_params.m_rnti = m_rnti;
                  stats_params.m_mcs = poolIt->second.m_currentGrant.m_mcs;
                  stats_params.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
                  stats_params.m_psschTxStartRB = poolIt->second.m_currentGrant.m_rbStart;
                  stats_params.m_psschTxLengthRB = poolIt->second.m_currentGrant.m_rbLen;
                  stats_params.m_psschItrp = poolIt->second.m_currentGrant.m_iTrp;
                  stats_params.m_timestamp = Simulator::Now ().GetMilliSeconds ();

                  m_slPscchScheduling (stats_params); //Trace

                  //clear the grant
                  poolIt->second.m_grantReceived = false;
                } //end of if (poolIt->second.m_grantReceived)
            }
          else
            {
              NS_LOG_DEBUG ("Sidelink Control period not started yet");
              NS_LOG_DEBUG("current frame number = " << frameNo <<" Next SC period frame number = "<<poolIt->second.m_nextScPeriod.frameNo);
              NS_LOG_DEBUG("current subframe number = " << subframeNo <<" Next SC period subframe number = "<<poolIt->second.m_nextScPeriod.subframeNo);
            }

          std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
          //check if we need to transmit PSCCH
          allocIt = poolIt->second.m_pscchTx.begin ();
          if (allocIt != poolIt->second.m_pscchTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
            {
              //transmission of PSCCH, no need for HARQ
              if (poolIt->second.m_pscchTx.size () == 2)
                {
                  NS_LOG_INFO ("First PSCCH transmission");
                }
              else
                {
                  NS_LOG_INFO ("Second PSCCH transmission");
                }
              //create SCI message
              SciListElement_s sci;
              sci.m_rnti = m_rnti;
              sci.m_mcs = poolIt->second.m_currentGrant.m_mcs;
              sci.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
              sci.m_resPscch = poolIt->second.m_currentGrant.m_resPscch;
              sci.m_rbLen = poolIt->second.m_currentGrant.m_rbLen;
              sci.m_rbStart = poolIt->second.m_currentGrant.m_rbStart;
              sci.m_trp = poolIt->second.m_currentGrant.m_iTrp;
              sci.m_hopping = poolIt->second.m_currentGrant.m_hopping;
              sci.m_hoppingInfo = poolIt->second.m_currentGrant.m_hoppingInfo;
              sci.m_groupDstId = (poolIt->first & 0xFF);

              Ptr<SciLteControlMessage> msg = Create<SciLteControlMessage> ();
              msg->SetSci (sci);
              m_uePhySapProvider->SendLteControlMessage (msg);

              poolIt->second.m_pscchTx.erase (allocIt);
            }

          //check if we need to transmit PSSCH
          allocIt = poolIt->second.m_psschTx.begin ();
          if (allocIt != poolIt->second.m_psschTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
            {
              // Collect statistics for SL share channel UE MAC scheduling trace
              SlUeMacStatParameters stats_sch_params;
              stats_sch_params.m_frameNo = poolIt->second.m_currentScPeriod.frameNo;
              stats_sch_params.m_subframeNo = poolIt->second.m_currentScPeriod.subframeNo;
              stats_sch_params.m_psschFrame = frameNo;
              stats_sch_params.m_psschSubframe = subframeNo;
              stats_sch_params.m_cellId = 0;
              stats_sch_params.m_imsi = 0;
              stats_sch_params.m_pscchRi = 0;
              stats_sch_params.m_pscchFrame1 = 0;
              stats_sch_params.m_pscchSubframe1 = 0;
              stats_sch_params.m_pscchFrame2 = 0;
              stats_sch_params.m_pscchSubframe2 = 0;
              stats_sch_params.m_psschItrp = 0;
              stats_sch_params.m_psschFrameStart = 0;
              stats_sch_params.m_psschSubframeStart = 0;

              //Get first subframe of PSSCH
              SidelinkCommResourcePool::SubframeInfo currScPeriod;
              currScPeriod.frameNo = poolIt->second.m_currentScPeriod.frameNo - 1;
              currScPeriod.subframeNo = poolIt->second.m_currentScPeriod.subframeNo - 1;
              std::vector<uint32_t> psschSFVector = poolIt->second.m_pool->GetAllPsschSubframes ();
              uint32_t psschStartSubframe = 10 * (currScPeriod.frameNo % 1024) + currScPeriod.subframeNo % 10 + psschSFVector[0];
              stats_sch_params.m_psschFrameStart = psschStartSubframe / 10 + 1;
              stats_sch_params.m_psschSubframeStart = psschStartSubframe % 10 + 1;

              stats_sch_params.m_rnti = m_rnti;
              stats_sch_params.m_mcs = poolIt->second.m_currentGrant.m_mcs;
              stats_sch_params.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
              stats_sch_params.m_psschTxStartRB = (*allocIt).rbStart;
              stats_sch_params.m_psschTxLengthRB = (*allocIt).nbRb;
              stats_sch_params.m_timestamp = Simulator::Now ().GetMilliSeconds ();

              m_slPsschScheduling (stats_sch_params); //Trace

              if (poolIt->second.m_psschTx.size () % 4 == 0)
                {
                  NS_LOG_INFO ("New PSSCH transmission");
                  Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
                  poolIt->second.m_miSlHarqProcessPacket = emptyPb;

                  //get the BSR for this pool
                  //if we have data in the queue
                  //find the BSR for that pool (will also give the SidleinkLcIdentifier)

                  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
                  for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
                    {
                      if (itBsr->first.dstL2Id == poolIt->first)
                        {
                          //this is the BSR for the pool
                          std::map <SidelinkLcIdentifier, LcInfo>::iterator it = m_slLcInfoMap.find (itBsr->first);
                          //for Sidelink we should never have retxQueueSize since it is unacknowledged mode
                          //we still keep the process similar to uplink to be more generic (and maybe handle
                          //future modifications)
                          if ( ((*itBsr).second.statusPduSize > 0)
                               || ((*itBsr).second.retxQueueSize > 0)
                               || ((*itBsr).second.txQueueSize > 0))
                            {
                              //We have data to send in the PSSCH, notify the RRC to start/continue sending SLSS if appropriate
                              m_slHasDataToTx = true;
                              m_cmacSapUser->NotifyMacHasSlDataToSend ();

                              NS_ASSERT ((*itBsr).second.statusPduSize == 0 && (*itBsr).second.retxQueueSize == 0);
                              //similar code as uplink transmission
                              uint32_t bytesForThisLc = poolIt->second.m_currentGrant.m_tbSize;
                              NS_LOG_LOGIC ("RNTI " << m_rnti << " Sidelink Tx " << bytesForThisLc << " bytes to LC "
                                                    << (uint32_t)(*itBsr).first.lcId << " statusQueue " << (*itBsr).second.statusPduSize
                                                    << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue"
                                                    <<  (*itBsr).second.txQueueSize);
                              if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
                                {
                                  (*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0, m_componentCarrierId, m_rnti, (*itBsr).first.lcId);
                                  bytesForThisLc -= (*itBsr).second.statusPduSize; //decrement size available for data
                                  NS_LOG_DEBUG ("Serve STATUS PDU" << (*itBsr).second.statusPduSize);
                                  (*itBsr).second.statusPduSize = 0;
                                }
                              else
                                {
                                  if ((*itBsr).second.statusPduSize > bytesForThisLc)
                                    {
                                      NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
                                    }
                                }
                              // 7 is the min TxOpportunity useful for Rlc
                              if ((bytesForThisLc > 7)
                                  && (((*itBsr).second.retxQueueSize > 0)
                                      || ((*itBsr).second.txQueueSize > 0)))
                                {
                                  if ((*itBsr).second.retxQueueSize > 0)
                                    {
                                      NS_LOG_DEBUG ("Serve retx DATA, bytes " << bytesForThisLc);
                                      (*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0, m_componentCarrierId, m_rnti, (*itBsr).first.lcId);
                                      if ((*itBsr).second.retxQueueSize >= bytesForThisLc)
                                        {
                                          (*itBsr).second.retxQueueSize -= bytesForThisLc;
                                        }
                                      else
                                        {
                                          (*itBsr).second.retxQueueSize = 0;
                                        }
                                    }
                                  else if ((*itBsr).second.txQueueSize > 0)
                                    {
                                      // minimum RLC overhead due to header
                                      uint32_t rlcOverhead = 2;
                                      NS_LOG_DEBUG ("Serve tx DATA, bytes " << bytesForThisLc << ", RLC overhead " << rlcOverhead);
                                      (*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0, m_componentCarrierId, m_rnti, (*itBsr).first.lcId);
                                      if ((*itBsr).second.txQueueSize >= bytesForThisLc - rlcOverhead)
                                        {
                                          (*itBsr).second.txQueueSize -= bytesForThisLc - rlcOverhead;
                                        }
                                      else
                                        {
                                          (*itBsr).second.txQueueSize = 0;
                                        }
                                    }
                                }
                              else
                                {
                                  if ( ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
                                    {
                                      if (poolIt->second.m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
                                        {
                                          // Resend BSR info for updating eNB peer MAC
                                          m_freshSlBsr = true;
                                        }
                                    }
                                }
                              NS_LOG_LOGIC ("RNTI " << m_rnti << " Sidelink Tx " << bytesForThisLc << "\t new queues "
                                                    << (uint32_t)(*it).first.lcId << " statusQueue " << (*itBsr).second.statusPduSize
                                                    << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
                            }
                          break;
                        }
                    } //end of for (itBsr = m_slBsrReceived.begin () ;  itBsr != m_slBsrReceived.end () ; itBsr++)
                }
              else
                {
                  NS_LOG_INFO ("PSSCH retransmission " << (4 - poolIt->second.m_psschTx.size () % 4));
                  Ptr<PacketBurst> pb = poolIt->second.m_miSlHarqProcessPacket;
                  for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                    {
                      Ptr<Packet> pkt = (*j)->Copy ();
                      m_uePhySapProvider->SendMacPdu (pkt);
                    }
                }

              poolIt->second.m_psschTx.erase (allocIt);
            } //end of if //check if we need to transmit PSSCH
        } //end of for (poolIt = m_sidelinkTxPoolsMap.begin() ; poolIt != m_sidelinkTxPoolsMap.end() ; poolIt++)
    }
}

void
LteUeMac::DoAddSlDestination (uint32_t destination)
{
  NS_LOG_FUNCTION (this);
  std::list <uint32_t>::iterator it;
  for (it = m_sidelinkDestinations.begin (); it != m_sidelinkDestinations.end (); it++)
    {
      if ((*it) == destination)
        {
          break;
        }
    }
  if (it == m_sidelinkDestinations.end ())
    {
      //did not find it, so insert
      m_sidelinkDestinations.push_back (destination);
    }
}

void
LteUeMac::DoRemoveSlDestination (uint32_t destination)
{
  NS_LOG_FUNCTION (this);
  std::list <uint32_t>::iterator it = m_sidelinkDestinations.begin ();
  while (it != m_sidelinkDestinations.end ())
    {
      if ((*it) == destination)
        {
          m_sidelinkDestinations.erase (it);
          break;
        }
      it++;
    }
}

void
LteUeMac::DoNotifyChangeOfTiming(uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this);
  //There is a delay between the MAC scheduling and the transmission so we assume that we are ahead.
  if (subframeNo + UL_PUSCH_TTIS_DELAY > 10)
    {
      frameNo++;
      if (frameNo > 1024)
        {
          frameNo = 1;
        }
      subframeNo = (subframeNo + UL_PUSCH_TTIS_DELAY) % 10;
    }
  else
    {
      subframeNo = subframeNo + UL_PUSCH_TTIS_DELAY;
    }

  std::map <uint32_t, PoolInfo>::iterator poolIt;
  for (poolIt = m_sidelinkTxPoolsMap.begin() ; poolIt != m_sidelinkTxPoolsMap.end() ; poolIt++)
    {
      poolIt->second.m_currentScPeriod = poolIt->second.m_pool->GetCurrentScPeriod (frameNo, subframeNo);
      poolIt->second.m_nextScPeriod = poolIt->second.m_pool->GetNextScPeriod (poolIt->second.m_currentScPeriod.frameNo, poolIt->second.m_currentScPeriod.subframeNo);
      //adjust because scheduler starts with frame/subframe = 1
      poolIt->second.m_nextScPeriod.frameNo++;
      poolIt->second.m_nextScPeriod.subframeNo++;
      NS_LOG_INFO ("Adapting the period for pool of group " << poolIt->first << ". Next period at "
                   << poolIt->second.m_nextScPeriod.frameNo << "/" << poolIt->second.m_nextScPeriod.subframeNo);
      NS_ABORT_MSG_IF (poolIt->second.m_nextScPeriod.frameNo > 1024 || poolIt->second.m_nextScPeriod.subframeNo > 10,
                                             "Invalid frame or subframe number");
    }
}

void
LteUeMac::DoNotifySidelinkEnabled()
{
  NS_LOG_FUNCTION (this);
  m_sidelinkEnabled = true;
}

std::list< Ptr<SidelinkRxDiscResourcePool> >
LteUeMac::GetDiscRxPools ()
{
  NS_LOG_FUNCTION (this);
  return m_discRxPools;
}

Ptr<SidelinkTxDiscResourcePool>
LteUeMac::GetDiscTxPool ()
{
  NS_LOG_FUNCTION (this);
  return m_discTxPool.m_pool;
}

int64_t
LteUeMac::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_raPreambleUniformVariable->SetStream (stream);
  m_ueSelectedUniformVariable->SetStream (stream);
  m_p1UniformVariable->SetStream (stream);
  m_resUniformVariable->SetStream (stream);
  return 1;
}

} // namespace ns3
