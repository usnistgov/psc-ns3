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
 * Modified by:
 *          NIST // Contributions may not be subject to US copyright. (D2D extensions)
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
#include "lte-sl-header.h"
#include "lte-sl-tag.h"
#include <ns3/ff-mac-common.h>
#include <ns3/lte-control-messages.h>
#include <ns3/simulator.h>
#include <ns3/lte-common.h>

#include <bitset>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteUeMac");

NS_OBJECT_ENSURE_REGISTERED (LteUeMac);

/**
 * Delay from subframe start to perform the SL subframe indication
 * Arbitrary number chosen to be larger than the DL_CTRL_DURATION to ensure
 * UL_DCI has been received, and PHY has generated the DL CQI if needed.
 */
static const Time SL_SF_INDICATION_DELAY = NanoSeconds (3e5);

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
  virtual void NotifyConnectionSuccessful ();
  virtual void SetImsi (uint64_t imsi);
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
UeMemberLteUeCmacSapProvider::NotifyConnectionSuccessful ()
{
  m_mac->DoNotifyConnectionSuccessful ();
}

void
UeMemberLteUeCmacSapProvider::SetImsi (uint64_t imsi)
{
  m_mac->DoSetImsi (imsi);
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
  virtual void ReceiveSlDiscPhyPdu (Ptr<Packet> p);
  virtual void ReceiveSlSciPhyPdu (Ptr<Packet> p);
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  virtual void NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo);
  virtual void NotifySidelinkEnabled ();
  virtual void NotifyUlTransmission ();

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
UeMemberLteUePhySapUser::ReceiveSlDiscPhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceiveSlDiscPhyPdu (p);
}

void
UeMemberLteUePhySapUser::ReceiveSlSciPhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceiveSlSciPhyPdu (p);
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

void
UeMemberLteUePhySapUser::NotifyUlTransmission ()
{
  m_mac->DoNotifyUlTransmission ();
}


//////////////////////////////////////////////////////////
// LteUeMac methods
///////////////////////////////////////////////////////////


TypeId
LteUeMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteUeMac")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteUeMac> ()
    .AddTraceSource ("RaResponseTimeout",
                     "trace fired upon RA response timeout",
                     MakeTraceSourceAccessor (&LteUeMac::m_raResponseTimeoutTrace),
                     "ns3::LteUeMac::RaResponseTimeoutTracedCallback")
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
    .AddAttribute ("SlScheduler",
                   "Algorithm for assigning resources for sidelink",
                   EnumValue (LteUeMac::FIXED),
                   MakeEnumAccessor (&LteUeMac::m_schedulingGrantMetric),
                   MakeEnumChecker (LteUeMac::FIXED, "Fixed",
                                    LteUeMac::RANDOM, "Random",
                                    LteUeMac::MIN_PRB, "MinPrb",
                                    LteUeMac::MAX_COVERAGE, "MaxCoverage"))
    .AddTraceSource ("SlPscchScheduling",
                     "Information regarding SL UE scheduling",
                     MakeTraceSourceAccessor (&LteUeMac::m_slPscchScheduling),
                     "ns3::SlUeMacStatParameters::TracedCallback")
    .AddTraceSource ("SlPsschScheduling",
                     "Information regarding SL Shared Channel UE scheduling",
                     MakeTraceSourceAccessor (&LteUeMac::m_slPsschScheduling),
                     "ns3::SlUeMacStatParameters::TracedCallback")
    .AddTraceSource ("SlPsdchScheduling",
                     "Information regarding SL discovery message transmissions",
                     MakeTraceSourceAccessor (&LteUeMac::m_slPsdchScheduling),
                     "ns3::SlUeMacStatParameters::TracedCallback")
  ;
  return tid;
}


LteUeMac::LteUeMac ()
  :  m_bsrPeriodicity (MilliSeconds (1)),
  // ideal behavior
  m_bsrLast (MilliSeconds (0)),
  m_freshUlBsr (false),
  m_harqProcessId (0),
  m_rnti (0),
  m_imsi (0),
  m_rachConfigured (false),
  m_waitingForRaResponse (false),
  m_slBsrPeriodicity (MilliSeconds (1)),
  m_slBsrLast (MilliSeconds (0)),
  m_freshSlBsr (false),
  m_setTrpIndex (0),
  m_useSetTrpIndex (false),
  m_slSynchPendingTxMsg (0),
  m_slHasDataToTxInPeriod (false),
  m_sidelinkEnabled (false),
  m_hasUlToTx (false),
  m_hasSlMibToTx (false),
  m_hasSlCommToTx (false),
  m_hasSlCommToRx (false),
  m_schedulingGrantMetric (LteUeMac::SlSchedulingGrantMetric::FIXED)
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
  m_discTxPool.m_pool = nullptr;
  m_discTxPool.m_nextDiscPeriod.frameNo = 0;
  m_discTxPool.m_nextDiscPeriod.subframeNo = 0;
  m_slSchedTime.frameNo = 0;
  m_slSchedTime.subframeNo = 0;

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

  if (params.discMsg)
    {
      LteRadioBearerTag tag;
      tag.SetRnti (params.rnti);
      params.pdu->AddPacketTag (tag);
      //queue message until next discovery period
      m_discPendingTxMsgs.push_back (params.pdu);
    }
  else if (params.mibslMsg)
    {
      NS_ASSERT_MSG (m_slSynchPendingTxMsg == 0, "Trying to transmit multiple synchronization messages");
      m_slSynchPendingTxMsg = params.pdu;
    }
  else
    {
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
          std::list <PoolInfo>::iterator poolIt = GetPoolForDestination (params.dstL2Id);
          NS_ASSERT (poolIt != m_sidelinkTxPools.end ());
          std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator allocIt = poolIt->m_psschTx.begin ();

          //For sanity check, but to be removed once the code is checked

          uint32_t frameNo = m_slSchedTime.frameNo;
          uint32_t subframeNo = m_slSchedTime.subframeNo;

          //Following conversion is not needed because we are now
          //using the frame and subframe number, which is 4ms ahead of PHY
          /*
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
          */
          NS_LOG_DEBUG ("Before assert: Sidelink MAC current frame/subframe number = " << frameNo << "/" << subframeNo);
          NS_LOG_DEBUG  ("Allocated frame/subframe number = " << (*allocIt).subframe.frameNo << "/" << (*allocIt).subframe.subframeNo);

          NS_ASSERT ((*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo);

          LteRadioBearerTag tag (params.rnti, params.lcid, params.srcL2Id, params.dstL2Id);
          params.pdu->AddPacketTag (tag);
          // store pdu in HARQ buffer
          poolIt->m_miSlHarqProcessPacket->AddPacket (params.pdu);

          LteUePhySapProvider::TransmitSlPhySduParameters phyParams;
          phyParams.channel = LteUePhySapProvider::TransmitSlPhySduParameters::PSSCH;
          phyParams.rbStart = (*allocIt).rbStart;
          phyParams.rbLen = (*allocIt).nbRb;
          uint8_t rv = poolIt->m_psschTx.size () % 4;
          NS_ASSERT (rv == 0); //to be removed once confirmed
          phyParams.rv = rv == 0 ? rv : 4 - rv;

          if (!m_hasUlToTx && !m_hasSlMibToTx)
            {
              m_uePhySapProvider->SendSlMacPdu (params.pdu, phyParams);
            }
          else
            {
              NS_LOG_DEBUG ("PSSCH message not sent because of uplink or SL-MIB transmission (UL=" << m_hasUlToTx << ", Sl-MIB=" << m_hasSlMibToTx << ")");
            }

        }
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
      NS_LOG_INFO ("Reporting for Sidelink. Tx Queue size = " << params.txQueueSize);
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
  m_hasUlToTx = true;
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
  for (std::list <PoolInfo >::iterator slTxPoolIt = m_sidelinkTxPools.begin ();
       slTxPoolIt != m_sidelinkTxPools.end () && !scheduled; slTxPoolIt++)
    {
      if (slTxPoolIt->m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
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
      std::list <PoolInfo >::iterator slTxPoolIt = GetPoolForDestination (dstL2Id);
      Ptr<SidelinkTxCommResourcePool> pool = DynamicCast<SidelinkTxCommResourcePool> (slTxPoolIt->m_pool);
      NS_ASSERT (slTxPoolIt != m_sidelinkTxPools.end ());
      if (pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
        {
          NS_LOG_DEBUG ("Tx queue size = " << (*it).second.txQueueSize <<
                        " ReTx queue size = " << (*it).second.retxQueueSize <<
                        " Status PDU size = " << (*it).second.statusPduSize);

          queue.at (pool->GetIndex ()) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
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
  m_hasUlToTx = true;
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
      LteMacSapUser::TxOpportunityParameters txOpParams;
      txOpParams.bytes = raResponse.m_grant.m_tbSize;
      txOpParams.layer = 0;
      txOpParams.harqId = 0;
      txOpParams.componentCarrierId = m_componentCarrierId;
      txOpParams.rnti = m_rnti;
      txOpParams.lcid = lc0Lcid;
      txOpParams.srcL2Id = 0;
      txOpParams.dstL2Id = 0;
      lc0InfoIt->second.macSapUser->NotifyTxOpportunity (txOpParams);
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
  //fire RA response timeout trace
  m_raResponseTimeoutTrace (m_imsi, contention, m_preambleTransmissionCounter,
                            m_rachConfig.preambleTransMax + 1);
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
LteUeMac::DoSetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this);
  m_imsi = imsi;
}


void
LteUeMac::DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) preambleId << (uint16_t) prachMask);
  NS_ASSERT_MSG (prachMask == 0, "requested PRACH MASK = " << (uint32_t) prachMask << ", but only PRACH MASK = 0 is supported");
  m_rnti = rnti;
  m_raPreambleId = preambleId;
  m_preambleTransmissionCounter = 0;
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
  m_ulBsrReceived.erase(lcId); //empty BSR buffer for this lcId
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
  // note: rnti will be assigned by the eNB using RA response message
  m_rnti = 0;
  m_noRaResponseReceivedEvent.Cancel ();
  m_rachConfigured = false;
  m_freshUlBsr = false;
  m_ulBsrReceived.clear ();
}

void
LteUeMac::DoNotifyConnectionSuccessful ()
{
  NS_LOG_FUNCTION (this);
  m_uePhySapProvider->NotifyConnectionSuccessful ();
}

void
LteUeMac::DoSetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
  NS_LOG_FUNCTION (this);
  //NS_ASSERT_MSG (m_discTxPool.m_pool != nullptr, "Cannot add discovery transmission pool for " << m_rnti << ". Pool already exist for destination");
  DiscPoolInfo info;
  info.m_pool = pool;
  info.m_npsdch = info.m_pool->GetNPsdch ();
  info.m_currentDiscPeriod.frameNo = 0; //init to 0 to make it invalid
  info.m_currentDiscPeriod.subframeNo = 0; //init to 0 to make it invalid

  /**
   * After learning the correct usage of GetNextDiscPeriod, i.e, subtracting 1
   * from a frame and a subframe number before calling this method and adding 1
   * after, we should use the frame and subframe number, which is 4 ms ahead of
   * PHY.
   */

  info.m_nextDiscPeriod = info.m_pool->GetNextDiscPeriod (m_slSchedTime.frameNo - 1, m_slSchedTime.subframeNo - 1);
  //adjust because scheduler starts with frame/subframe = 1
  info.m_nextDiscPeriod.frameNo++;
  info.m_nextDiscPeriod.subframeNo++;
  NS_ABORT_MSG_IF (info.m_nextDiscPeriod.frameNo > 1024 || info.m_nextDiscPeriod.subframeNo > 10,
                   "Invalid frame or subframe number");

  info.m_nextGrants.clear ();
  m_discTxPool = info;
}

void
LteUeMac::DoRemoveSlDiscTxPool ()
{
  NS_LOG_FUNCTION (this);
  m_discTxPool.m_pool = nullptr;
}

void
LteUeMac::DoSetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  NS_LOG_FUNCTION (this);
  m_discRxPools = pools;
}

void
LteUeMac::DoAddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool)
{
  NS_LOG_FUNCTION (this << dstL2Id << pool);

  //check if the pool is already known
  std::list <PoolInfo>::iterator itPool;
  for (itPool = m_sidelinkTxPools.begin (); itPool != m_sidelinkTxPools.end (); itPool++)
    {
      if (*(itPool->m_pool) == *pool)
        {
          break;
        }
    }

  if (itPool == m_sidelinkTxPools.end ())
    {
      //create new pool information
      PoolInfo info;
      info.m_pool = pool;
      info.m_destinations.clear ();
      info.m_npscch = info.m_pool->GetNPscch ();
      info.m_currentScPeriod.frameNo = 0; //init to 0 to make it invalid
      info.m_currentScPeriod.subframeNo = 0; //init to 0 to make it invalid
      /**
      * After learning the correct usage of GetNextScPeriod, i.e, subtracting 1
      * from a frame and a subframe number before calling this method and adding 1
      * after, we should use the frame and subframe number, which is 4 ms ahead of
      * PHY.
      */

      NS_LOG_DEBUG ("sidelink current MAC frame/subframe number : " << m_slSchedTime.frameNo << "/" << m_slSchedTime.subframeNo);
      info.m_nextScPeriod = info.m_pool->GetNextScPeriod (m_slSchedTime.frameNo - 1, m_slSchedTime.subframeNo - 1);
      //adjust because scheduler starts with frame/subframe = 1
      info.m_nextScPeriod.frameNo++;
      info.m_nextScPeriod.subframeNo++;

      NS_LOG_DEBUG ("Next SC frame/subframe number : " << info.m_nextScPeriod.frameNo << "/" << info.m_nextScPeriod.subframeNo);

      NS_ABORT_MSG_IF (info.m_nextScPeriod.frameNo > 1024 || info.m_nextScPeriod.subframeNo > 10,
                       "Invalid frame or subframe number");
      info.m_grantReceived = false;
      info.m_currentGrant.m_dst = 0;

      itPool = m_sidelinkTxPools.insert (m_sidelinkTxPools.begin (), info);
    }

  itPool->m_destinations.insert (dstL2Id);

}

void
LteUeMac::DoRemoveSlCommTxPool (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);
  std::list <PoolInfo>::iterator it = GetPoolForDestination (dstL2Id);
  NS_ASSERT_MSG (it != m_sidelinkTxPools.end (), "Cannot remove Sidelink transmission pool for " << dstL2Id << ". Unknown destination");
  it->m_destinations.erase (it->m_destinations.find (dstL2Id));
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
              LteMacSapUser::ReceivePduParameters rxPduParams;
              rxPduParams.p = p;
              rxPduParams.rnti = m_rnti;
              rxPduParams.lcid = tag.GetLcid ();
              rxPduParams.srcL2Id = 0;
              rxPduParams.dstL2Id = 0;
              it->second.macSapUser->ReceivePdu (rxPduParams);
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
      for (dstIt = m_sidelinkDestinations.begin (); dstIt != m_sidelinkDestinations.end (); dstIt++)
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
                  m_cmacSapUser->NotifySidelinkReception (tag.GetLcid (), tag.GetSourceL2Id (), tag.GetDestinationL2Id ());

                  //should be setup now
                  it = m_slLcInfoMap.find (identifier);
                  if (it == m_slLcInfoMap.end ())
                    {
                      NS_LOG_WARN ("Failure to setup Sidelink radio bearer");
                    }
                }
              LteMacSapUser::ReceivePduParameters rxPduParams;
              rxPduParams.p = p;
              rxPduParams.rnti = m_rnti;
              rxPduParams.lcid = tag.GetLcid ();
              rxPduParams.srcL2Id = tag.GetSourceL2Id ();
              rxPduParams.dstL2Id = tag.GetDestinationL2Id ();
              it->second.macSapUser->ReceivePdu (rxPduParams);
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
LteUeMac::DoReceiveSlDiscPhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO (this << " Received discovery message");
  //notify RRC (pass msg to RRC where we can filter)
  m_cmacSapUser->NotifyDiscoveryReception (p);
}

void
LteUeMac::DoReceiveSlSciPhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);

  LteSlSciHeader sciHeader;
  p->PeekHeader (sciHeader);
  LteSlSciTag tag;
  p->PeekPacketTag (tag);

  //Keep track of the subframes in which PSSCH receptions can occur so we don't
  //transmit discovery during that time
  std::list <uint32_t>::iterator it;
  for (it = m_sidelinkDestinations.begin (); it != m_sidelinkDestinations.end (); it++)
    {
      if (sciHeader.GetGroupDstId () == ((*it) & 0xFF))
        {
          NS_LOG_INFO ("received SCI for group " << (uint32_t)((*it) & 0xFF) << " from rnti " << tag.GetRnti ());

          std::list <Ptr<SidelinkRxCommResourcePool> >::iterator poolIt = m_sidelinkRxPools.begin ();

          NS_ASSERT_MSG (poolIt != m_sidelinkRxPools.end (), "No receiving pools configured");

          /**
           * After learning the correct usage of GetCurrentScPeriod, i.e,
           * subtracting 1 from a frame and a subframe number before calling
           * this method and adding 1 after, we should use the frame and
           * subframe number, which is 4 ms ahead of PHY.
           */

          SidelinkCommResourcePool::SubframeInfo tmp = (*poolIt)->GetCurrentScPeriod (m_slSchedTime.frameNo - 1, m_slSchedTime.subframeNo - 1);

          std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> psschTx = (*poolIt)->GetPsschTransmissions (tmp, sciHeader.GetTrp (), sciHeader.GetRbStart (), sciHeader.GetRbLen ());
          std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator rxIt;
          for (rxIt = psschTx.begin (); rxIt != psschTx.end (); rxIt++)
            {
              //adjust for index starting at 1
              rxIt->subframe.frameNo++;
              rxIt->subframe.subframeNo++;
              NS_LOG_INFO ("Subframe Rx " << rxIt->subframe.frameNo << "/" << rxIt->subframe.subframeNo);
              if (m_slSchedTime < rxIt->subframe)
                {
                  m_psschRxSet.insert (rxIt->subframe);
                }
              else
                {
                  //MAC is 4 ms ahead so we do not care about the PSSCH reception, which may occur on the frame/subframe
                  //number less than the current frame/subframe at MAC.
                  //This can happen when the number of subframes between the reception of the SCI message in the PSCCH and
                  //the computed subframe to receive data on the PSSCH is less than 4.
                  NS_LOG_INFO ("MAC frame/subframe > frame/subframe number of the expected PSSCH reception."
                               "Do not store the subframe info of this reception");
                }
            }
        }
    }
}

void
LteUeMac::DoReceiveLteControlMessage (Ptr<LteControlMessage> msg)
{
  NS_LOG_FUNCTION (this);
  if (msg->GetMessageType () == LteControlMessage::UL_DCI)
    {
      //Because we received an uplink allocation, we mark that there is an uplink transmission
      m_hasUlToTx = true;
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
              NS_LOG_DEBUG (this << " Reduced resource -> send only Status, bytes " << statusPduMinSize);
              if (dci.m_tbSize < statusPduMinSize)
                {
                  NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
                }
            }
          NS_LOG_LOGIC (this << " UE " << m_rnti << ": UL-CQI notified TxOpportunity of " << dci.m_tbSize << " => " << bytesPerActiveLc << " bytes per active LC" << " statusPduMinSize " << statusPduMinSize);

          LteMacSapUser::TxOpportunityParameters txOpParams;

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
                      txOpParams.bytes = (*itBsr).second.statusPduSize;
                      txOpParams.layer = 0;
                      txOpParams.harqId = 0;
                      txOpParams.componentCarrierId = m_componentCarrierId;
                      txOpParams.rnti = m_rnti;
                      txOpParams.lcid = (*it).first;
                      txOpParams.srcL2Id = 0;
                      txOpParams.dstL2Id = 0;
                      (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
                          txOpParams.bytes = (*itBsr).second.statusPduSize;
                          txOpParams.layer = 0;
                          txOpParams.harqId = 0;
                          txOpParams.componentCarrierId = m_componentCarrierId;
                          txOpParams.rnti = m_rnti;
                          txOpParams.lcid = (*it).first;
                          txOpParams.srcL2Id = 0;
                          txOpParams.dstL2Id = 0;
                          (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
                              txOpParams.bytes = bytesForThisLc;
                              txOpParams.layer = 0;
                              txOpParams.harqId = 0;
                              txOpParams.componentCarrierId = m_componentCarrierId;
                              txOpParams.rnti = m_rnti;
                              txOpParams.lcid = (*it).first;
                              txOpParams.srcL2Id = 0;
                              txOpParams.dstL2Id = 0;
                              (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
                              txOpParams.bytes = bytesForThisLc;
                              txOpParams.layer = 0;
                              txOpParams.harqId = 0;
                              txOpParams.componentCarrierId = m_componentCarrierId;
                              txOpParams.rnti = m_rnti;
                              txOpParams.lcid = (*it).first;
                              txOpParams.srcL2Id = 0;
                              txOpParams.dstL2Id = 0;
                              (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
      Ptr<SidelinkTxCommResourcePool> pool = DynamicCast<SidelinkTxCommResourcePool> (m_sidelinkTxPools.begin ()->m_pool);
      NS_ASSERT (pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED);

      SidelinkGrant grant;
      grant.m_resPscch = dci.m_resPscch;
      grant.m_tpc = dci.m_tpc;
      grant.m_hopping = dci.m_hopping;
      grant.m_rbStart = dci.m_rbStart;
      grant.m_rbLen = dci.m_rbLen;
      grant.m_hoppingInfo = dci.m_hoppingInfo;
      grant.m_iTrp = dci.m_trp;
      grant.m_mcs = pool->GetMcs ();
      grant.m_tbSize = 0; //computed later
      m_sidelinkTxPools.begin ()->m_nextGrant = grant;
      m_sidelinkTxPools.begin ()->m_grantReceived = true;

      NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
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
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  m_frameNo = frameNo;
  m_subframeNo = subframeNo;
  RefreshHarqProcessesPacketBuffer ();
  m_hasUlToTx = false;
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
      Simulator::Schedule (SL_SF_INDICATION_DELAY, &LteUeMac::DoSlDelayedSubframeIndication, this, frameNo, subframeNo);
    }
}

void
LteUeMac::DoSlDelayedSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
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

  NS_LOG_INFO (this << " Adjusted Frame/Subframe number from " << m_frameNo << "/" << m_subframeNo << " to " << frameNo << "/" << subframeNo);
  m_slSchedTime.frameNo = frameNo;
  m_slSchedTime.subframeNo = subframeNo;

  m_hasSlMibToTx = false;
  m_hasSlCommToTx = false;
  m_hasSlCommToRx = false;

  //Sidelink Synchronization
  if (m_slSynchPendingTxMsg)
    {
      LteUePhySapProvider::TransmitSlPhySduParameters phyParams;
      phyParams.channel = LteUePhySapProvider::TransmitSlPhySduParameters::PSBCH;
      phyParams.rbStart = 22;
      phyParams.rbLen = 6;
      phyParams.rv = 0;
      m_hasSlMibToTx = true;
      if (!m_hasUlToTx)
        {
          NS_LOG_DEBUG ("Transmitting MIB-SL message");
          m_uePhySapProvider->SendSlMacPdu (m_slSynchPendingTxMsg, phyParams);
        }
      else
        {
          NS_LOG_DEBUG ("MIB-SL message not sent because of uplink transmission");
        }
      //clear pending message
      m_slSynchPendingTxMsg = nullptr;
    }

  //Sidelink Communication
  if ((Simulator::Now () >= m_slBsrLast + m_slBsrPeriodicity) && (m_freshSlBsr == true))
    {
      SendSidelinkReportBufferStatus ();
      m_slBsrLast = Simulator::Now ();
      m_freshSlBsr = false;
    }

  SidelinkCommResourcePool::SubframeInfo currentSubframe;
  currentSubframe.frameNo = frameNo;
  currentSubframe.subframeNo = subframeNo;
  std::set <SidelinkCommResourcePool::SubframeInfo>::iterator subIt = m_psschRxSet.find (currentSubframe);
  if (subIt != m_psschRxSet.end ())
    {
      m_hasSlCommToRx = true;
      NS_LOG_DEBUG ("Expected PSSCH reception in this subframe ");
      m_psschRxSet.erase (subIt);
    }

  std::list <PoolInfo>::iterator poolIt;
  for (poolIt = m_sidelinkTxPools.begin (); poolIt != m_sidelinkTxPools.end (); poolIt++)
    {
      //Check if this is a new SC period
      if (frameNo == poolIt->m_nextScPeriod.frameNo && subframeNo == poolIt->m_nextScPeriod.subframeNo)
        {
          poolIt->m_currentScPeriod = poolIt->m_nextScPeriod;
          poolIt->m_nextScPeriod = poolIt->m_pool->GetNextScPeriod (frameNo - 1, subframeNo - 1);
          //adjust because scheduler starts with frame/subframe = 1
          poolIt->m_nextScPeriod.frameNo++;
          poolIt->m_nextScPeriod.subframeNo++;
          NS_LOG_INFO ("Starting new SC period. Next period at "
                       << poolIt->m_nextScPeriod.frameNo << "/" << poolIt->m_nextScPeriod.subframeNo);
          NS_ABORT_MSG_IF (poolIt->m_nextScPeriod.frameNo > 1024 || poolIt->m_nextScPeriod.subframeNo > 10,
                           "Invalid frame or subframe number");

          Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
          poolIt->m_miSlHarqProcessPacket = emptyPb;

          if (poolIt->m_pool->GetSchedulingType () == SidelinkCommResourcePool::UE_SELECTED)
            {
              //If m_slHasDataToTx is False here (at the beginning of the period), it means
              //that no transmissions in the PSSCH occurred in the previous SC period.
              //Notify the RRC for stopping SLSS transmissions if appropriate

              if (!m_slHasDataToTxInPeriod)
                {
                  m_cmacSapUser->NotifyMacHasNoSlDataToSend ();
                }
              //Make m_slHasDataToTx = false here (beginning of the period) to detect if transmissions
              //in the PSSCH are performed in this period
              m_slHasDataToTxInPeriod = false;

              poolIt->m_nextGrant = GetSlUeSelectedGrant (poolIt);
              poolIt->m_grantReceived = poolIt->m_nextGrant.m_dst != 0;
            }     // end if (UE_SELECTED)

          //if we received a grant, compute the transmission opportunities for PSCCH and PSSCH
          if (poolIt->m_grantReceived)
            {
              if (poolIt->m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
                {
                  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
                  for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
                    {
                      if (itBsr->first.dstL2Id == *(poolIt->m_destinations.begin ()))
                        {
                          //this is the BSR for the pool
                          NS_LOG_DEBUG ("Found the BSR. Tx Queue size = " << (*itBsr).second.txQueueSize);
                          poolIt->m_nextGrant.m_dst = itBsr->first.dstL2Id;
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
              poolIt->m_currentGrant = poolIt->m_nextGrant;
              NS_LOG_INFO ("Sidelink grant received resource " << (uint32_t) poolIt->m_currentGrant.m_resPscch);
              SidelinkCommResourcePool::SubframeInfo subFrameInfo;
              subFrameInfo.frameNo = poolIt->m_currentScPeriod.frameNo - 1;
              subFrameInfo.subframeNo = poolIt->m_currentScPeriod.subframeNo - 1;

              poolIt->m_pscchTx = poolIt->m_pool->GetPscchTransmissions (poolIt->m_currentGrant.m_resPscch);
              NS_ABORT_MSG_IF (poolIt->m_pscchTx.size () > 2, "PSCCH ONLY SUPPORTS 2 TRANSMISSIONS PER UE GRANT!");
              for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->m_pscchTx.begin ();
                   txIt != poolIt->m_pscchTx.end (); txIt++)
                {
                  txIt->subframe = txIt->subframe + subFrameInfo;
                  //adjust for index starting at 1
                  txIt->subframe.frameNo++;
                  txIt->subframe.subframeNo++;
                  NS_LOG_INFO ("PSCCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo
                                                  << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
                  NS_ABORT_MSG_IF (txIt->subframe.frameNo > 1024 || txIt->subframe.subframeNo > 10,
                                   "Invalid frame or subframe number");
                }
              poolIt->m_psschTx = poolIt->m_pool->GetPsschTransmissions (subFrameInfo, poolIt->m_currentGrant.m_iTrp, poolIt->m_currentGrant.m_rbStart, poolIt->m_currentGrant.m_rbLen);

              //adjust PSSCH frame to next period
              for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->m_psschTx.begin ();
                   txIt != poolIt->m_psschTx.end (); txIt++)
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
              poolIt->m_currentGrant.m_tbSize = m_amc->GetUlTbSizeFromMcs (poolIt->m_currentGrant.m_mcs, poolIt->m_currentGrant.m_rbLen) / 8;
              NS_LOG_INFO ("Sidelink Tb size = " << poolIt->m_currentGrant.m_tbSize << " bytes (mcs=" << (uint32_t) poolIt->m_currentGrant.m_mcs << ")");

              //clear the grant
              poolIt->m_grantReceived = false;
            }     //end of if (poolIt->second.m_grantReceived)
        }
      else
        {
          NS_LOG_DEBUG ("Sidelink Control period not started yet");
          NS_LOG_DEBUG ("current frame number = " << frameNo << " Next SC period frame number = " << poolIt->m_nextScPeriod.frameNo);
          NS_LOG_DEBUG ("current subframe number = " << subframeNo << " Next SC period subframe number = " << poolIt->m_nextScPeriod.subframeNo);
        }

      std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
      //check if we need to transmit PSCCH
      allocIt = poolIt->m_pscchTx.begin ();
      if (allocIt != poolIt->m_pscchTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
        {
          //transmission of PSCCH, no need for HARQ
          if (poolIt->m_pscchTx.size () == 2)
            {
              NS_LOG_INFO ("First PSCCH transmission");
            }
          else
            {
              NS_LOG_INFO ("Second PSCCH transmission");
            }
          //create SCI message
          LteSlSciHeader sciHeader;
          sciHeader.SetSciFormat0Params (poolIt->m_currentGrant.m_hopping,
                                         poolIt->m_currentGrant.m_rbStart,
                                         poolIt->m_currentGrant.m_rbLen,
                                         poolIt->m_currentGrant.m_hoppingInfo,
                                         poolIt->m_currentGrant.m_iTrp,
                                         poolIt->m_currentGrant.m_mcs,
                                         poolIt->m_currentGrant.m_dst & 0xFF);
          Ptr<Packet> p = Create<Packet> ();
          p->AddHeader (sciHeader);
          LteSlSciTag tag (m_rnti, poolIt->m_currentGrant.m_resPscch, poolIt->m_currentGrant.m_tbSize);
          p->AddPacketTag (tag);

          LteUePhySapProvider::TransmitSlPhySduParameters phyParams;
          phyParams.channel = LteUePhySapProvider::TransmitSlPhySduParameters::PSCCH;
          phyParams.rbStart = (*allocIt).rbStart;
          phyParams.rbLen = (*allocIt).nbRb;
          phyParams.resNo = poolIt->m_currentGrant.m_resPscch;

          m_hasSlCommToTx = true;

          if (!m_hasUlToTx && !m_hasSlMibToTx)
            {
              m_uePhySapProvider->SendSlMacPdu (p, phyParams);
            }
          else
            {
              NS_LOG_DEBUG ("SCI message not sent because of uplink or SL-MIB transmission (UL=" << m_hasUlToTx << ", Sl-MIB=" << m_hasSlMibToTx << ")");
            }

          // Collect statistics for SL PSCCH UE MAC scheduling trace
          SlUeMacStatParameters pscchStatsParams;
          pscchStatsParams.m_timestamp = Simulator::Now ().GetMilliSeconds ();
          pscchStatsParams.m_cellId = 0;
          pscchStatsParams.m_imsi = 0;
          pscchStatsParams.m_rnti = m_rnti;
          pscchStatsParams.m_frameNo = frameNo;
          pscchStatsParams.m_subframeNo = subframeNo;
          pscchStatsParams.m_periodStartFrame = poolIt->m_currentScPeriod.frameNo;
          pscchStatsParams.m_periodStartSubframe = poolIt->m_currentScPeriod.subframeNo;
          pscchStatsParams.m_resIndex = poolIt->m_currentGrant.m_resPscch;
          pscchStatsParams.m_tbSize = p->GetSize ();
          pscchStatsParams.m_pscchTxLengthRB = (*allocIt).nbRb;
          pscchStatsParams.m_pscchTxStartRB = (*allocIt).rbStart;
          pscchStatsParams.m_hopping = poolIt->m_currentGrant.m_hopping;
          pscchStatsParams.m_hoppingInfo = poolIt->m_currentGrant.m_hoppingInfo;
          pscchStatsParams.m_txLengthRB = poolIt->m_currentGrant.m_rbLen;
          pscchStatsParams.m_txStartRB = poolIt->m_currentGrant.m_rbStart;
          pscchStatsParams.m_psschItrp = poolIt->m_currentGrant.m_iTrp;
          pscchStatsParams.m_mcs = poolIt->m_currentGrant.m_mcs;
          pscchStatsParams.m_groupDstId = poolIt->m_currentGrant.m_dst & 0xFF;
          pscchStatsParams.m_sidelinkDropped = ((m_hasUlToTx || m_hasSlMibToTx) == 1 ? 1 : 0);
          m_slPscchScheduling (pscchStatsParams); //Trace

          poolIt->m_pscchTx.erase (allocIt);
        }

      //check if we need to transmit PSSCH
      allocIt = poolIt->m_psschTx.begin ();
      if (allocIt != poolIt->m_psschTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
        {
          m_hasSlCommToTx = true;

          if (poolIt->m_psschTx.size () % 4 == 0)
            {
              NS_LOG_INFO ("New PSSCH transmission");
              Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
              poolIt->m_miSlHarqProcessPacket = emptyPb;

              //get the BSR for this pool
              //if we have data in the queue
              //find the BSR for that pool (will also give the SidleinkLcIdentifier)

              std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
              for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
                {
                  if (itBsr->first.dstL2Id == poolIt->m_currentGrant.m_dst)
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
                          m_slHasDataToTxInPeriod = true;
                          m_cmacSapUser->NotifyMacHasSlDataToSend ();

                          NS_ASSERT ((*itBsr).second.statusPduSize == 0 && (*itBsr).second.retxQueueSize == 0);
                          //similar code as uplink transmission
                          uint32_t bytesForThisLc = poolIt->m_currentGrant.m_tbSize;
                          NS_LOG_LOGIC ("RNTI " << m_rnti << " Sidelink Tx " << bytesForThisLc << " bytes to LC "
                                                << (uint32_t)(*itBsr).first.lcId << " statusQueue " << (*itBsr).second.statusPduSize
                                                << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue"
                                                <<  (*itBsr).second.txQueueSize);
                          if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
                            {
                              LteMacSapUser::TxOpportunityParameters txOpParams;
                              txOpParams.bytes = (*itBsr).second.statusPduSize;
                              txOpParams.layer = 0;
                              txOpParams.harqId = 0;
                              txOpParams.componentCarrierId = m_componentCarrierId;
                              txOpParams.rnti = m_rnti;
                              txOpParams.lcid = (*itBsr).first.lcId;
                              txOpParams.srcL2Id = (*itBsr).first.srcL2Id;
                              txOpParams.dstL2Id = (*itBsr).first.dstL2Id;
                              (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
                              bytesForThisLc -= (*itBsr).second.statusPduSize;     //decrement size available for data
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
                                  LteMacSapUser::TxOpportunityParameters txOpParams;
                                  txOpParams.bytes = bytesForThisLc;
                                  txOpParams.layer = 0;
                                  txOpParams.harqId = 0;
                                  txOpParams.componentCarrierId = m_componentCarrierId;
                                  txOpParams.rnti = m_rnti;
                                  txOpParams.lcid = (*itBsr).first.lcId;
                                  txOpParams.srcL2Id = (*itBsr).first.srcL2Id;
                                  txOpParams.dstL2Id = (*itBsr).first.dstL2Id;
                                  (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
                                  LteMacSapUser::TxOpportunityParameters txOpParams;
                                  txOpParams.bytes = bytesForThisLc;
                                  txOpParams.layer = 0;
                                  txOpParams.harqId = 0;
                                  txOpParams.componentCarrierId = m_componentCarrierId;
                                  txOpParams.rnti = m_rnti;
                                  txOpParams.lcid = (*itBsr).first.lcId;
                                  txOpParams.srcL2Id = (*itBsr).first.srcL2Id;
                                  txOpParams.dstL2Id = (*itBsr).first.dstL2Id;
                                  (*it).second.macSapUser->NotifyTxOpportunity (txOpParams);
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
                                  if (poolIt->m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
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
                }     //end of for (itBsr = m_slBsrReceived.begin () ;  itBsr != m_slBsrReceived.end () ; itBsr++)
            }
          else
            {
              NS_LOG_INFO ("PSSCH retransmission " << (4 - poolIt->m_psschTx.size () % 4));
              Ptr<PacketBurst> pb = poolIt->m_miSlHarqProcessPacket;
              for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                {
                  Ptr<Packet> pkt = (*j)->Copy ();
                  LteUePhySapProvider::TransmitSlPhySduParameters phyParams;
                  phyParams.channel = LteUePhySapProvider::TransmitSlPhySduParameters::PSSCH;
                  phyParams.rbStart = (*allocIt).rbStart;
                  phyParams.rbLen = (*allocIt).nbRb;
                  uint8_t rv = poolIt->m_psschTx.size () % 4;
                  phyParams.rv = rv == 0 ? 0 : 4 - rv;
                  if (!m_hasUlToTx && !m_hasSlMibToTx)
                    {
                      m_uePhySapProvider->SendSlMacPdu (pkt, phyParams);
                    }
                  else
                    {
                      NS_LOG_DEBUG ("PSSCH message not sent because of uplink or SL-MIB transmission (UL=" << m_hasUlToTx << ", Sl-MIB=" << m_hasSlMibToTx << ")");
                    }
                }
            }
          // Collect statistics for SL PSSCH UE MAC scheduling trace
          SlUeMacStatParameters psschStatsParams;
          psschStatsParams.m_timestamp = Simulator::Now ().GetMilliSeconds ();
          psschStatsParams.m_cellId = 0;
          psschStatsParams.m_imsi = 0;
          psschStatsParams.m_rnti = m_rnti;
          psschStatsParams.m_frameNo = frameNo;
          psschStatsParams.m_subframeNo = subframeNo;
          psschStatsParams.m_periodStartFrame = poolIt->m_currentScPeriod.frameNo;
          psschStatsParams.m_periodStartSubframe = poolIt->m_currentScPeriod.subframeNo;
          psschStatsParams.m_txLengthRB = (*allocIt).nbRb;
          psschStatsParams.m_txStartRB = (*allocIt).rbStart;
          psschStatsParams.m_mcs = poolIt->m_currentGrant.m_mcs;
          psschStatsParams.m_tbSize = poolIt->m_currentGrant.m_tbSize;
          uint8_t rv = poolIt->m_psschTx.size () % 4;
          psschStatsParams.m_rv = rv == 0 ? 0 : 4 - rv;
          psschStatsParams.m_sidelinkDropped = ((m_hasUlToTx || m_hasSlMibToTx) == 1 ? 1 : 0);
          m_slPsschScheduling (psschStatsParams); //Trace

          poolIt->m_psschTx.erase (allocIt);
        }     //end of if //check if we need to transmit PSSCH
    }     //end of for (poolIt = m_sidelinkTxPoolsMap.begin() ; poolIt != m_sidelinkTxPoolsMap.end() ; poolIt++)

  //Sidelink Discovery
  if (m_discTxPool.m_pool != nullptr)
    {
      if (frameNo == m_discTxPool.m_nextDiscPeriod.frameNo && subframeNo == m_discTxPool.m_nextDiscPeriod.subframeNo)
        {
          //define periods and frames
          m_discTxPool.m_currentDiscPeriod = m_discTxPool.m_nextDiscPeriod;
          m_discTxPool.m_nextDiscPeriod = m_discTxPool.m_pool->GetNextDiscPeriod (frameNo - 1, subframeNo - 1);
          m_discTxPool.m_nextDiscPeriod.frameNo++;
          m_discTxPool.m_nextDiscPeriod.subframeNo++;
          NS_LOG_INFO ("Starting new discovery period " << ". Next period at " << m_discTxPool.m_nextDiscPeriod.frameNo << "/"
                                                        << m_discTxPool.m_nextDiscPeriod.subframeNo);
          NS_ABORT_MSG_IF (m_discTxPool.m_nextDiscPeriod.frameNo > 1024 || m_discTxPool.m_nextDiscPeriod.subframeNo > 10,
                           "Invalid frame or subframe number");

          if (m_discTxPool.m_pool->GetSchedulingType () == SidelinkDiscResourcePool::UE_SELECTED)
            {
              //Section 5.15.1.1
              //for each MAC PDU, pick a random number to see if the packet will be transmitted
              //then pick a random resource amongst those available

              //set of resources available
              std::set <uint32_t> resourcesAvailable;
              for (uint32_t i = 0; i < m_discTxPool.m_npsdch; i++)
                {
                  resourcesAvailable.insert (i);
                }

              std::list<Ptr<Packet> >::iterator pktIt;
              for (pktIt = m_discPendingTxMsgs.begin (); pktIt != m_discPendingTxMsgs.end (); pktIt++)
                {
                  //use txProbability
                  double p1 = m_p1UniformVariable->GetValue (0, 1);
                  double txProbability = m_discTxPool.m_pool->GetTxProbability ();       //calculate txProbability
                  NS_LOG_DEBUG ("txProbability = " << txProbability << " % " << " selected " << (100 * p1));
                  if (p1 <= txProbability / 100.0)
                    {
                      uint32_t randVar = m_resUniformVariable->GetInteger (0, resourcesAvailable.size () - 1);
                      DiscGrant grant;
                      std::set <uint32_t>::iterator selectResIt = resourcesAvailable.begin ();
                      std::advance (selectResIt,randVar);
                      grant.m_resPsdch = *selectResIt;
                      grant.m_discMsg = *pktIt;
                      m_discTxPool.m_nextGrants.push_back (grant);
                      NS_LOG_INFO ("UE selected grant: resource =" << (uint16_t) grant.m_resPsdch << "/" << m_discTxPool.m_npsdch);
                      //remove overlapping resources
                      std::set <uint32_t> conflictingResources = m_discTxPool.m_pool->GetConflictingResources (grant.m_resPsdch);
                      std::set <uint32_t>::iterator resIt;
                      for (resIt = conflictingResources.begin (); resIt != conflictingResources.end (); resIt++)
                        {
                          resourcesAvailable.erase (*resIt);
                        }
                      if (resourcesAvailable.size () == 0)
                        {
                          NS_LOG_INFO ("No more resources available for sending additional discovery messages");
                          break;
                        }
                    }
                  else
                    {
                      NS_LOG_INFO ("Discovery packet will not be transmitted due to transmit probability");
                    }
                }
            }
          else       //scheduled
            {
              //TODO
              //use defined grant : SL-TF-IndexPair
            }

          //Move new grants to current grants
          m_discTxPool.m_currentGrants = m_discTxPool.m_nextGrants;
          m_discTxPool.m_nextGrants.clear ();
          //Clear list of discovery messages to be sent, including those that did not receive a grant
          m_discPendingTxMsgs.clear ();


          //if we received a grant
          if (m_discTxPool.m_currentGrants.size () > 0)
            {
              SidelinkDiscResourcePool::SubframeInfo tmp;
              tmp.frameNo = m_discTxPool.m_currentDiscPeriod.frameNo - 1;
              tmp.subframeNo = m_discTxPool.m_currentDiscPeriod.subframeNo - 1;

              std::list <DiscGrant>::iterator grantIt;
              for (grantIt = m_discTxPool.m_currentGrants.begin (); grantIt != m_discTxPool.m_currentGrants.end (); grantIt++)
                {

                  grantIt->m_psdchTx = m_discTxPool.m_pool->GetPsdchTransmissions (grantIt->m_resPsdch);

                  for (std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator txIt = grantIt->m_psdchTx.begin ();
                       txIt != grantIt->m_psdchTx.end (); txIt++)
                    {
                      txIt->subframe = txIt->subframe + tmp;
                      //adjust for index starting at 1
                      txIt->subframe.frameNo++;
                      txIt->subframe.subframeNo++;
                      NS_LOG_INFO ("PSDCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo
                                                      << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
                      NS_ABORT_MSG_IF (txIt->subframe.frameNo > 1024 || txIt->subframe.subframeNo > 10,
                                       "Invalid frame or subframe number");
                    }
                }
            }
        }

      //check if we need to transmit PSDCH
      std::list <DiscGrant>::iterator grantIt;
      for (grantIt = m_discTxPool.m_currentGrants.begin (); grantIt != m_discTxPool.m_currentGrants.end (); grantIt++)
        {
          std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
          allocIt = grantIt->m_psdchTx.begin ();
          if (allocIt != grantIt->m_psdchTx.end () && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
            {
              NS_LOG_INFO (this << "PSDCH transmission");

              //Create Discovery message for each discovery payload announcing
              NS_LOG_INFO ("discovery message sent by " << m_rnti);

              LteSlDiscHeader discHeader;
              grantIt->m_discMsg->PeekHeader (discHeader);

              LteUePhySapProvider::TransmitSlPhySduParameters phyParams;
              phyParams.channel = LteUePhySapProvider::TransmitSlPhySduParameters::PSDCH;
              phyParams.rbStart = (*allocIt).rbStart;
              phyParams.rbLen = (*allocIt).nbRb;
              phyParams.resNo =  grantIt->m_resPsdch;
              uint8_t rv = grantIt->m_psdchTx.size () % (m_discTxPool.m_pool->GetNumRetx () + 1);
              phyParams.rv = rv == 0 ? rv : m_discTxPool.m_pool->GetNumRetx () + 1 - rv;

              if (!m_hasUlToTx && !m_hasSlMibToTx && !m_hasSlCommToTx && !m_hasSlCommToRx)
                {
                  m_uePhySapProvider->SendSlMacPdu (grantIt->m_discMsg, phyParams);
                }
              else
                {
                  NS_LOG_DEBUG ("Discovery message not sent because of uplink or SL-MIB transmission, or sidelink communication  transmission or reception (UL=" << m_hasUlToTx << ", Sl-MIB=" << m_hasSlMibToTx << ", COMMTx=" << m_hasSlCommToTx << ", COMMRx=" << m_hasSlCommToRx << ")");
                }

              // Collect statistics for SL PSDCH UE MAC scheduling trace
              SlUeMacStatParameters psdchStatsParams;
              psdchStatsParams.m_timestamp = Simulator::Now ().GetMilliSeconds ();
              psdchStatsParams.m_cellId = 0;
              psdchStatsParams.m_imsi = 0;
              psdchStatsParams.m_rnti = m_rnti;
              psdchStatsParams.m_frameNo = frameNo;
              psdchStatsParams.m_subframeNo = subframeNo;
              psdchStatsParams.m_periodStartFrame = m_discTxPool.m_currentDiscPeriod.frameNo;
              psdchStatsParams.m_periodStartSubframe = m_discTxPool.m_currentDiscPeriod.subframeNo;
              psdchStatsParams.m_resIndex = grantIt->m_resPsdch;
              psdchStatsParams.m_txStartRB = (*allocIt).rbStart;
              psdchStatsParams.m_txLengthRB = (*allocIt).nbRb;
              psdchStatsParams.m_mcs = 8; //for discovery, we use a fixed QPSK modulation
              psdchStatsParams.m_tbSize = grantIt->m_discMsg->GetSize (); //bytes
              psdchStatsParams.m_rv = phyParams.rv;
              psdchStatsParams.m_sidelinkDropped = ((m_hasUlToTx || m_hasSlMibToTx || m_hasSlCommToTx || m_hasSlCommToRx) == 1 ? 1 : 0);
              m_slPsdchScheduling (psdchStatsParams, discHeader); //Trace

              grantIt->m_psdchTx.erase (allocIt);
              if (grantIt->m_psdchTx.empty ())
                {
                  NS_LOG_INFO ("was last transmission for this discovery message.");
                  grantIt = m_discTxPool.m_currentGrants.erase (grantIt);
                }
            }

        }     //end loop through discovery grants
    }
}

void
LteUeMac::DoAddSlDestination (uint32_t destination)
{
  NS_LOG_FUNCTION (this << destination);
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
  NS_LOG_FUNCTION (this << destination);
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
LteUeMac::DoNotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo)
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

  std::list <PoolInfo>::iterator poolIt;
  for (poolIt = m_sidelinkTxPools.begin (); poolIt != m_sidelinkTxPools.end (); poolIt++)
    {
      poolIt->m_currentScPeriod = poolIt->m_pool->GetCurrentScPeriod (frameNo - 1, subframeNo - 1);
      poolIt->m_nextScPeriod = poolIt->m_pool->GetNextScPeriod (poolIt->m_currentScPeriod.frameNo, poolIt->m_currentScPeriod.subframeNo);
      //adjust because scheduler starts with frame/subframe = 1
      poolIt->m_nextScPeriod.frameNo++;
      poolIt->m_nextScPeriod.subframeNo++;
      NS_LOG_INFO ("Adapting the period for pool. Next period at "
                   << poolIt->m_nextScPeriod.frameNo << "/" << poolIt->m_nextScPeriod.subframeNo);
      NS_ABORT_MSG_IF (poolIt->m_nextScPeriod.frameNo > 1024 || poolIt->m_nextScPeriod.subframeNo > 10,
                       "Invalid frame or subframe number");
    }
}

void
LteUeMac::DoNotifySidelinkEnabled ()
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
  m_ueSelectedUniformVariable->SetStream (stream + 1);
  m_p1UniformVariable->SetStream (stream + 2);
  m_resUniformVariable->SetStream (stream + 3);
  return 4;
}

void
LteUeMac::DoNotifyUlTransmission ()
{
  NS_LOG_FUNCTION (this);
  //toggle value to indicate that there will be an uplink transmission in this
  //subframe
  m_hasUlToTx = true;
}

LteUeMac::SidelinkGrant
LteUeMac::GetSlUeSelectedGrant (std::list<PoolInfo>::iterator poolIt)
{
  NS_LOG_FUNCTION (this);
  SidelinkGrant grant;
  grant.m_dst = 0;

  //First lets find the destination for the grant
  //For now, we use round robin and one grant per pool

  std::set<uint32_t>::iterator dstIt;
  bool foundCurrentDst = false;
  uint32_t firstDstWithData = 0;
  for (dstIt = poolIt->m_destinations.begin (); dstIt != poolIt->m_destinations.end (); dstIt++)
    {
      if (firstDstWithData == 0 && GetQueueSize (*dstIt) > 0)
        {
          //Mark first destination in case we don't find any destination after
          //the current destination
          firstDstWithData = *dstIt;
        }
      if (poolIt->m_currentGrant.m_dst == 0 && GetQueueSize (*dstIt) > 0)
        {
          //No previous destination, pick first one
          grant.m_dst = *dstIt;
          break;
        }
      else if (poolIt->m_currentGrant.m_dst == *dstIt)
        {
          //found current destination, mark that we need to pick the next one
          foundCurrentDst = true;
        }
      else if (foundCurrentDst && GetQueueSize (*dstIt) > 0)
        {
          //found next destination
          grant.m_dst = *dstIt;
          break;
        }
    }
  //if we did not find any destination with data to send after the current destination
  //loop back and assign to the first one found (it any)
  if (grant.m_dst == 0 && firstDstWithData != 0)
    {
      grant.m_dst = firstDstWithData;
    }

  if (grant.m_dst != 0)
    {
      NS_LOG_DEBUG ("Grant destination " << grant.m_dst);

      //Randomly selected Resource in PSCCH.
      uint16_t nbTxOpt = poolIt->m_npscch;
      grant.m_resPscch = m_ueSelectedUniformVariable->GetInteger (0, nbTxOpt - 1);
      grant.m_tpc = 0;

      //Schedule resources in PSSCH
      uint8_t slKtrp = m_slKtrp;                 //Initialize value to default, it will change according to the SlSchedulingGrantMetric
      if (m_schedulingGrantMetric == LteUeMac::SlSchedulingGrantMetric::FIXED)
        {
          NS_LOG_INFO ("PSSCH FIXED grant scheduling");
          grant.m_rbLen = m_slGrantSize;
          grant.m_mcs = m_slGrantMcs;
          grant.m_tbSize = m_amc->GetUlTbSizeFromMcs (grant.m_mcs, grant.m_rbLen);
        }
      else
        {               //Dynamic SlSchedulingGrantMetric
                        //Compute number of TB per SL period for each enabled Ktrp
          NS_LOG_INFO ("PSSCH dynamic grant scheduling");
          std::vector <uint32_t> tbPerKtrpVector = poolIt->m_pool->GetTbPerSlPeriod ();
          std::list <std::pair <uint8_t, std::vector <LteAmc::McsPrbInfo> > > kMcsPrbPairList;
          bool noAvailablePairForTbs = true;
          uint32_t tbs_bytes;
          uint8_t kValue;
          for (uint8_t i = 0; i < tbPerKtrpVector.size (); i++)
            {
              if (tbPerKtrpVector[i] > 0)
                {
                  //Compute TBS considering RLC overhead
                  tbs_bytes = (uint32_t) std::ceil (GetQueueSize (grant.m_dst) * 1.0 / tbPerKtrpVector[i]) + 2;             //RLC overhead = 2
                  tbs_bytes = std::max ((uint32_t)8, tbs_bytes); //need at least 7 bytes for RLC
                  kValue = (uint8_t) std::pow (2, double(i));
                  NS_LOG_DEBUG ("k: " << (uint16_t) kValue << ", TBnumber: " << tbPerKtrpVector[i] << ", TBS: " << tbs_bytes);
                  std::pair <uint8_t, std::vector <LteAmc::McsPrbInfo> > ktrpMcsPrbInfoPair (kValue, m_amc->GetUlMcsNprbInfoFromTbs (tbs_bytes * 8,poolIt->m_pool->GetPsschBandwidth (), 20));
                  for (uint32_t vi = 0; vi < ktrpMcsPrbInfoPair.second.size (); vi++)
                    {
                      NS_LOG_DEBUG ("MCS:" << (uint16_t) ktrpMcsPrbInfoPair.second[vi].mcs << ", nbRB:" << (uint16_t) ktrpMcsPrbInfoPair.second[vi].nbRb);
                    }
                  if (ktrpMcsPrbInfoPair.second.size () > 0)
                    {
                      noAvailablePairForTbs = false;
                      kMcsPrbPairList.push_back (ktrpMcsPrbInfoPair);
                    }
                }
            }
          //Check if no <MCS,PRB> pair can support TBS
          if (noAvailablePairForTbs)
            {
              NS_LOG_DEBUG ("Warning: TBS less than buffer request! Using FIXED grant scheduling");
              //Use default values
              grant.m_rbLen = m_slGrantSize;
              grant.m_mcs = m_slGrantMcs;
              grant.m_tbSize = m_amc->GetUlTbSizeFromMcs (grant.m_mcs, grant.m_rbLen);
            }
          else
            {
              //Configure based on scheduling goal.
              uint32_t randomPairIndex = 0;
              uint32_t pairIndexCounter = 0;
              uint32_t kPairIndex = 0;
              uint32_t totalMcsPrbPairs = 0;
              double pairScore = 0.0;
              double bestPairScore = 0.0;
              double sinrFromBler = 0.0;
              std::list <std::pair <uint8_t, std::vector <LteAmc::McsPrbInfo> > >::iterator itPair;
              std::list <std::pair <uint8_t, std::vector <LteAmc::McsPrbInfo> > >::reverse_iterator ritPair;
              switch (m_schedulingGrantMetric)
                {
                case LteUeMac::SlSchedulingGrantMetric::MIN_PRB:
                  // - Minimum number of RBs. Uses more number of transmissions and high MCS.
                  /*itPair = --(kMcsPrbPairList.end ());
                  slKtrp = (*itPair).first; //m_slKtrp
                  grant.m_rbLen = (*itPair).second.front ().nbRb; //m_slGrantSize;
                  grant.m_mcs = (*itPair).second.front ().mcs; //m_slGrantMcs;
                  grant.m_tbSize = (*itPair).second.front ().tbs; //transport block size
                  */
                  NS_LOG_INFO ("PSSCH MIN_PRB grant scheduling");
                  ritPair = kMcsPrbPairList.rbegin ();
                  slKtrp = (*ritPair).first;               //m_slKtrp
                  grant.m_rbLen = (*ritPair).second.front ().nbRb;               //m_slGrantSize;
                  grant.m_mcs = (*ritPair).second.front ().mcs;               //m_slGrantMcs;
                  grant.m_tbSize = (*ritPair).second.front ().tbs;               //transport block size
                  ritPair++;
                  for (; ritPair != kMcsPrbPairList.rend (); ritPair++)
                    {
                      //Evaluate each first pair
                      if ((*ritPair).second.front ().nbRb <= grant.m_rbLen)
                        {
                          slKtrp = (*ritPair).first;               //m_slKtrp
                          grant.m_rbLen = (*ritPair).second.front ().nbRb;
                          grant.m_mcs = (*ritPair).second.front ().mcs;
                          grant.m_tbSize = (*ritPair).second.front ().tbs;
                        }
                    }
                  break;
                case LteUeMac::SlSchedulingGrantMetric::RANDOM:
                  NS_LOG_INFO ("PSSCH RANDOM grant scheduling");
                  //Compute the total number of (MCS,PRBs) pairs
                  totalMcsPrbPairs = 0; //Reset
                  for (itPair = kMcsPrbPairList.begin (); itPair != kMcsPrbPairList.end (); itPair++)
                    {
                      totalMcsPrbPairs += (*itPair).second.size ();
                      NS_LOG_DEBUG ("K= " << (uint16_t)(*itPair).first << " , Available <MCS,PRB> pairs = " << (*itPair).second.size ());
                    }
                  NS_LOG_DEBUG ("Total pair indexes " << totalMcsPrbPairs);

                  randomPairIndex = m_ueSelectedUniformVariable->GetInteger (0, totalMcsPrbPairs - 1);
                  NS_LOG_DEBUG ("randomPairIndex= " << randomPairIndex );
                  //Get the pair
                  pairIndexCounter = 0; //Reset
                  for (itPair = kMcsPrbPairList.begin (); itPair != kMcsPrbPairList.end (); itPair++)
                    {
                      if (randomPairIndex < (pairIndexCounter + (*itPair).second.size ()))
                        {
                          break;
                        }
                      pairIndexCounter += (*itPair).second.size ();
                    }
                  if (itPair == kMcsPrbPairList.end ())
                    {
                      NS_FATAL_ERROR ("CORRUPTED <MCS,PRB> INDEXES LIST: ");
                    }

                  slKtrp = (*itPair).first; //m_slKtrp
                  //uint32_t vectorSize = (*itPair).second.size ();
                  kPairIndex = randomPairIndex - pairIndexCounter;
                  grant.m_rbLen = (*itPair).second[kPairIndex].nbRb;
                  grant.m_mcs = (*itPair).second[kPairIndex].mcs;
                  grant.m_tbSize = (*itPair).second[kPairIndex].tbs;
                  NS_LOG_DEBUG ("Total pair indexes " << totalMcsPrbPairs << ", randomPairIndex= " << randomPairIndex);
                  NS_LOG_DEBUG ("kPairIndex= " << kPairIndex << ", <MCS, PRBs>= <" << (uint16_t)grant.m_mcs << ", " << (uint16_t)grant.m_rbLen << ">, TBS= " << grant.m_tbSize);

                  break;
                case LteUeMac::SlSchedulingGrantMetric::MAX_COVERAGE:
                  NS_LOG_INFO ("PSSCH MAX_COVERAGE grant scheduling");
                  //Find pair with lowest score
                  bestPairScore = 100;              //Set it high to start
                  //Iterate kMcsPrbPairList, this contains <MCS,PRB> pairs for each enabled Ktrp
                  for (itPair = kMcsPrbPairList.begin (); itPair != kMcsPrbPairList.end (); itPair++)
                    {
                      //Evaluate each pair using utility function
                      for (std::vector <LteAmc::McsPrbInfo>::iterator itVector = (*itPair).second.begin (); itVector != (*itPair).second.end (); itVector++)
                        {
                          sinrFromBler = 10 * std::log10 (LteNistErrorModel::GetPsschSinrFromBler (LteNistErrorModel::AWGN, LteNistErrorModel::SISO, (*itVector).mcs, 3, 0.01));
                          pairScore = 10 * std::log10 ((*itVector).nbRb) + sinrFromBler;               // + getSnrFromBler();
                          NS_LOG_DEBUG ("MCS: " << (uint16_t) (*itVector).mcs << ", PRB: " << (uint16_t) (*itVector).nbRb << ", SINR: " << sinrFromBler << ", Dscore: " << pairScore);
                          if (pairScore < bestPairScore)
                            {
                              bestPairScore = pairScore;
                              slKtrp = (*itPair).first;                //m_slKtrp
                              grant.m_rbLen = (*itVector).nbRb;
                              grant.m_mcs = (*itVector).mcs;
                              grant.m_tbSize = (*itVector).tbs;
                            }
                        }
                    }
                  break;
                default:
                  NS_FATAL_ERROR ("SCHEDULING METRIC NOT IMPLEMENTED: ");

                }
            }
        }
      std::vector <uint8_t> validRbStarts = poolIt->m_pool->GetValidRBstart (grant.m_rbLen);
      NS_ABORT_MSG_IF (validRbStarts.size () == 0, "UE_MAC: No resources available for configured grant size!");
      grant.m_rbStart = validRbStarts [m_ueSelectedUniformVariable->GetInteger (0, validRbStarts.size () - 1)];
      grant.m_hoppingInfo = poolIt->m_pool->GetDataHoppingConfig ().hoppingInfo;
      if (grant.m_hoppingInfo < 4)
        {
          NS_LOG_DEBUG ("Hopping enabled");
          grant.m_hopping = 1;                 //Hopping is enabled
        }
      else
        {
          NS_LOG_DEBUG ("Hopping disabled");
          grant.m_hopping = 0;                 //Hopping is disabled
        }

      switch (slKtrp)
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
          NS_FATAL_ERROR ("Invalid KTRP value " << (uint16_t) slKtrp << ". Supported values: [1, 2, 4, 8]");
        }

      if (this->m_useSetTrpIndex == true)
        {
          grant.m_iTrp = this->m_setTrpIndex;
        }

      NS_LOG_INFO ("UE selected grant: resource=" << (uint16_t) grant.m_resPscch << "/"
                                                  << poolIt->m_npscch << ", rbStart=" << (uint16_t) grant.m_rbStart
                                                  << ", rbLen=" << (uint16_t) grant.m_rbLen << ", mcs=" << (uint16_t) grant.m_mcs
                                                  << ",itrp=" << (uint16_t) grant.m_iTrp
                                                  << ", dst=" << grant.m_dst);
    }

  else
    {
      NS_LOG_DEBUG ("No sidelink grant this period");
    }
  return grant;
}

uint32_t
LteUeMac::GetQueueSize (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);
  uint32_t size = 0;
  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters>::iterator itBsr;
  for (itBsr = m_slBsrReceived.begin ();  itBsr != m_slBsrReceived.end (); itBsr++)
    {
      if (itBsr->first.dstL2Id == dstL2Id)
        {
          //this is the BSR for the pool
          NS_LOG_DEBUG ("Found the BSR. Tx Queue size = " << (*itBsr).second.txQueueSize);
          size += (*itBsr).second.txQueueSize + (*itBsr).second.retxQueueSize + (*itBsr).second.statusPduSize;
        }
    }
  return size;
}

/**
   * Get an iterator to the pool that must be used for a given destination
   * \param dstL2Id
   * \return an iterator to the pool that must be used for a given destination
   */
std::list <LteUeMac::PoolInfo>::iterator
LteUeMac::GetPoolForDestination (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);
  std::list <PoolInfo>::iterator itPool;
  for (itPool = m_sidelinkTxPools.begin (); itPool != m_sidelinkTxPools.end (); itPool++)
    {
      if (itPool->m_destinations.find (dstL2Id) != itPool->m_destinations.end ())
        {
          break;
        }
    }
  return itPool;
}

} // namespace ns3
