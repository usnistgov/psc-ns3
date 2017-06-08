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
 * Modified by: NIST
 */



#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include <ns3/random-variable-stream.h>

#include "nist-lte-ue-mac.h"
#include "nist-lte-ue-net-device.h"
#include "nist-lte-radio-bearer-tag.h"
#include <ns3/nist-ff-mac-common.h>
#include <ns3/nist-lte-control-messages.h>
#include <ns3/simulator.h>
#include <ns3/nist-lte-common.h>
#include <ns3/string.h>
#include <ns3/enum.h>

#include <ns3/boolean.h>
#include<bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteUeMac");

NS_OBJECT_ENSURE_REGISTERED (NistLteUeMac);


///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////


class NistUeMemberLteUeCmacSapProvider : public NistLteUeCmacSapProvider
{
public:
	NistUeMemberLteUeCmacSapProvider (NistLteUeMac* mac);

	// inherited from NistLteUeCmacSapProvider
	virtual void ConfigureRach (NistRachConfig rc);
	virtual void StartContentionBasedRandomAccessProcedure ();
	virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask);
	virtual void AddLc (uint8_t lcId, NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu);
	virtual void AddLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu);
	virtual void RemoveLc (uint8_t lcId);
	virtual void RemoveLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);
	virtual void Reset ();
  //communication
	virtual void AddSlTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool);
	virtual void RemoveSlTxPool (uint32_t dstL2Id);
	virtual void SetSlRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
	virtual void AddSlDestination (uint32_t destination);
	virtual void RemoveSlDestination (uint32_t destination);
  //Discovery
  virtual void AddSlTxPool (Ptr<SidelinkTxDiscResourcePool> pool);
  virtual void RemoveSlTxPool ();
  virtual void SetSlRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);
  virtual void ModifyDiscTxApps (std::list<uint32_t> apps);
  virtual void ModifyDiscRxApps (std::list<uint32_t> apps);
  
	virtual void SetRnti (uint16_t rnti);
	// added to handle LC priority
	virtual void AddLCPriority ( uint8_t rnti, uint8_t lcid, uint8_t priority);

private:
	NistLteUeMac* m_mac;
};


NistUeMemberLteUeCmacSapProvider::NistUeMemberLteUeCmacSapProvider (NistLteUeMac* mac)
: m_mac (mac)
{
}

void 
NistUeMemberLteUeCmacSapProvider::ConfigureRach (NistRachConfig rc)
{
	m_mac->DoConfigureRach (rc);
}

void
NistUeMemberLteUeCmacSapProvider::StartContentionBasedRandomAccessProcedure ()
{
	m_mac->DoStartContentionBasedRandomAccessProcedure ();
}

void
NistUeMemberLteUeCmacSapProvider::StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
	m_mac->DoStartNonContentionBasedRandomAccessProcedure (rnti, preambleId, prachMask);
}


void
NistUeMemberLteUeCmacSapProvider::AddLc (uint8_t lcId, NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu)
{
	m_mac->DoAddLc (lcId, lcConfig, msu);
}

void
NistUeMemberLteUeCmacSapProvider::AddLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu)
{
	m_mac->DoAddLc (lcId, srcL2Id, dstL2Id, lcConfig, msu);
}

void
NistUeMemberLteUeCmacSapProvider::RemoveLc (uint8_t lcid)
{
	m_mac->DoRemoveLc (lcid);
}

void
NistUeMemberLteUeCmacSapProvider::RemoveLc (uint8_t lcid, uint32_t srcL2Id, uint32_t dstL2Id)
{
	m_mac->DoRemoveLc (lcid, srcL2Id, dstL2Id);
}

void
NistUeMemberLteUeCmacSapProvider::Reset ()
{
	m_mac->DoReset ();
}

void
NistUeMemberLteUeCmacSapProvider::AddSlTxPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
  m_mac->DoAddSlTxPool (pool);
}

void
NistUeMemberLteUeCmacSapProvider::RemoveSlTxPool ()
{
  m_mac->DoRemoveSlTxPool ();
}
  
void
NistUeMemberLteUeCmacSapProvider::SetSlRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  m_mac->DoSetSlRxPools (pools);
}

void
NistUeMemberLteUeCmacSapProvider::AddSlTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool)
{
	m_mac->DoAddSlTxPool (dstL2Id, pool);
}

void
NistUeMemberLteUeCmacSapProvider::RemoveSlTxPool (uint32_t dstL2Id)
{
	m_mac->DoRemoveSlTxPool (dstL2Id);
}

void
NistUeMemberLteUeCmacSapProvider::SetSlRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
	m_mac->DoSetSlRxPools (pools);
}

void
NistUeMemberLteUeCmacSapProvider::AddSlDestination (uint32_t destination)
{
	m_mac->DoAddSlDestination (destination);
}

void
NistUeMemberLteUeCmacSapProvider::RemoveSlDestination (uint32_t destination)
{
	m_mac->DoRemoveSlDestination (destination);
}

void
NistUeMemberLteUeCmacSapProvider::SetRnti (uint16_t rnti)
{
	m_mac->DoSetRnti (rnti);
}

void
NistUeMemberLteUeCmacSapProvider::ModifyDiscTxApps (std::list<uint32_t> apps)
{
  m_mac->DoModifyDiscTxApps (apps);
}

void
NistUeMemberLteUeCmacSapProvider::ModifyDiscRxApps (std::list<uint32_t> apps)
{
  m_mac->DoModifyDiscRxApps (apps);
}

// added fucntion to handle priority for LC 
void 
NistUeMemberLteUeCmacSapProvider::AddLCPriority ( uint8_t rnti, uint8_t lcid, uint8_t priority)
{
	m_mac->DoAddLCPriority (rnti, lcid, priority);
}  

class NistUeMemberLteMacSapProvider : public NistLteMacSapProvider
{
public:
	NistUeMemberLteMacSapProvider (NistLteUeMac* mac);

	// inherited from NistLteMacSapProvider
	virtual void TransmitPdu (NistTransmitPduParameters params);
	virtual void ReportBufferNistStatus (NistReportBufferNistStatusParameters params);

private:
	NistLteUeMac* m_mac;
};


NistUeMemberLteMacSapProvider::NistUeMemberLteMacSapProvider (NistLteUeMac* mac)
: m_mac (mac)
{
}

void
NistUeMemberLteMacSapProvider::TransmitPdu (NistTransmitPduParameters params)
{
	m_mac->DoTransmitPdu (params);
}


void
NistUeMemberLteMacSapProvider::ReportBufferNistStatus (NistReportBufferNistStatusParameters params)
{
	m_mac->DoReportBufferNistStatus (params);
}




class NistUeMemberLteUePhySapUser : public NistLteUePhySapUser
{
public:
	NistUeMemberLteUePhySapUser (NistLteUeMac* mac);

	// inherited from NistLtePhySapUser
	virtual void ReceivePhyPdu (Ptr<Packet> p);
	virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);
	virtual void ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg);
	virtual void NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo);

private:
	NistLteUeMac* m_mac;
};

NistUeMemberLteUePhySapUser::NistUeMemberLteUePhySapUser (NistLteUeMac* mac) : m_mac (mac)
{

}

void
NistUeMemberLteUePhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
	m_mac->DoReceivePhyPdu (p);
}


void
NistUeMemberLteUePhySapUser::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
	m_mac->DoSubframeIndication (frameNo, subframeNo);
}

void
NistUeMemberLteUePhySapUser::ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
	m_mac->DoReceiveNistLteControlMessage (msg);
}

void
NistUeMemberLteUePhySapUser::NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo)
{
	m_mac->DoNotifyChangeOfTiming (frameNo, subframeNo);
}




//////////////////////////////////////////////////////////
// NistLteUeMac methods
///////////////////////////////////////////////////////////


TypeId
NistLteUeMac::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::NistLteUeMac")
		.SetParent<Object> ()
		.AddConstructor<NistLteUeMac> ()
		.AddAttribute ("NistUlScheduler",
					"Type of Scheduler in the UE",
					StringValue ("ns3::NistRrFfMacScheduler"),
					MakeStringAccessor (&NistLteUeMac::SetNistUlScheduler,
					&NistLteUeMac::GetNistUlScheduler),
					MakeStringChecker ())
		.AddAttribute ("Ktrp",
					"The repetition for PSSCH. Default = 0",
					UintegerValue (0),
					MakeUintegerAccessor (&NistLteUeMac::m_slKtrp),
					MakeUintegerChecker<uint8_t> ())
		.AddAttribute ("SlGrantMcs",
					"The MCS of the SL grant, must be [0..15] (default 0)",
					UintegerValue (0),
					MakeUintegerAccessor (&NistLteUeMac::m_slGrantMcs),
					MakeUintegerChecker<uint8_t> ())
		.AddAttribute ("SlGrantSize",
					"The number of RBs allocated per UE for sidelink (default 1)",
					UintegerValue (1),
					MakeUintegerAccessor (&NistLteUeMac::m_slGrantSize),
					MakeUintegerChecker<uint8_t> ())
		.AddAttribute ("PucchSize",
					"Number of RBs reserved for PUCCH (default 0)",
					UintegerValue (0),
					MakeUintegerAccessor (&NistLteUeMac::m_pucchSize),
					MakeUintegerChecker<uint8_t> ())
		.AddTraceSource ("SlUeScheduling",
					"Information regarding SL UE scheduling",
					MakeTraceSourceAccessor (&NistLteUeMac::m_slUeScheduling),
					"ns3::NistSlUeMacStatParameters::TracedCallback")
		.AddTraceSource ("SlSharedChUeScheduling",
					"Information regarding SL Shared Channel UE scheduling",
					MakeTraceSourceAccessor (&NistLteUeMac::m_slSharedChUeScheduling),
					"ns3::NistSlUeMacStatParameters::TracedCallback")
    // Added to trace the transmission of discovery message
    .AddTraceSource ("DiscoveryAnnouncement",
                     "trace to track the announcement of discovery messages",
                     MakeTraceSourceAccessor (&NistLteUeMac::m_discoveryAnnouncementTrace),
                     "ns3::NistLteUeMac::DiscoveryAnnouncementTracedCallback")
																									;
	return tid;
}


NistLteUeMac::NistLteUeMac ()
:  
   m_cphySapProvider (0),
   m_bsrPeriodicity (MilliSeconds (1)), // ideal behavior
   m_bsrLast (MilliSeconds (0)),
   m_freshUlBsr (false),
   m_harqProcessId (0),
   m_rnti (0),
   m_rachConfigured (false),
   m_waitingForRaResponse (false),
   m_slBsrPeriodicity (MilliSeconds (1)),
   m_slBsrLast (MilliSeconds (0)),
   m_freshSlBsr (false),
   m_NistUlScheduler ("ns3::NistRrFfMacScheduler")    //UE scheduler initialization
{
	NS_LOG_FUNCTION (this);
	m_miUlHarqProcessesPacket.resize (HARQ_PERIOD);
	for (uint8_t i = 0; i < m_miUlHarqProcessesPacket.size (); i++)
	{
		Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
		m_miUlHarqProcessesPacket.at (i) = pb;
	}
	m_miUlHarqProcessesPacketTimer.resize (HARQ_PERIOD, 0);

	m_macSapProvider = new NistUeMemberLteMacSapProvider (this);
	m_cmacSapProvider = new NistUeMemberLteUeCmacSapProvider (this);
	m_uePhySapUser = new NistUeMemberLteUePhySapUser (this);
	m_raPreambleUniformVariable = CreateObject<UniformRandomVariable> ();

	m_amc = CreateObject <NistLteAmc> ();
	m_ueSelectedUniformVariable = CreateObject<UniformRandomVariable> ();
	//m_slDiversity.status = SlDiversity::disabled;//enabled should be default!
  
  m_p1UniformVariable = CreateObject<UniformRandomVariable> ();
  m_resUniformVariable = CreateObject<UniformRandomVariable> ();
}


void
NistLteUeMac::SetNistUlScheduler (std::string UeSched)
{
	m_NistUlScheduler = UeSched;
}

std::string 
NistLteUeMac::GetNistUlScheduler (void) const
{
	return m_NistUlScheduler;
}

NistLteUeMac::~NistLteUeMac ()
{
	NS_LOG_FUNCTION (this);
}

void
NistLteUeMac::DoDispose ()
{
	NS_LOG_FUNCTION (this);
	m_miUlHarqProcessesPacket.clear ();
	delete m_macSapProvider;
	delete m_cmacSapProvider;
	delete m_uePhySapUser;
	Object::DoDispose ();
}

void
NistLteUeMac::SetNistLteUeCphySapProvider (NistLteUeCphySapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_cphySapProvider = s;
}

NistLteUePhySapUser*
NistLteUeMac::GetNistLteUePhySapUser (void)
{
	return m_uePhySapUser;
}

void
NistLteUeMac::SetNistLteUePhySapProvider (NistLteUePhySapProvider* s)
{
	m_uePhySapProvider = s;
}


NistLteMacSapProvider*
NistLteUeMac::GetNistLteMacSapProvider (void)
{
	return m_macSapProvider;
}

void
NistLteUeMac::SetNistLteUeCmacSapUser (NistLteUeCmacSapUser* s)
{
	m_cmacSapUser = s;
}

NistLteUeCmacSapProvider*
NistLteUeMac::GetNistLteUeCmacSapProvider (void)
{
	return m_cmacSapProvider;
}


void
NistLteUeMac::DoTransmitPdu (NistLteMacSapProvider::NistTransmitPduParameters params)
{
	NS_LOG_FUNCTION (this);
	NS_ASSERT_MSG (m_rnti == params.rnti, "RNTI mismatch between RLC and MAC");

	if (params.srcL2Id == 0)
	{
		NistLteRadioBearerTag tag (params.rnti, params.lcid, 0 /* UE works in SISO mode*/);
		params.pdu->AddPacketTag (tag);
		// store pdu in HARQ buffer
		m_miUlHarqProcessesPacket.at (m_harqProcessId)->AddPacket (params.pdu);
		m_miUlHarqProcessesPacketTimer.at (m_harqProcessId) = HARQ_PERIOD;
		m_uePhySapProvider->SendMacPdu (params.pdu);
	}
	else
	{
		NS_LOG_INFO ("Transmitting sidelink PDU");
		//find transmitting pool
		std::map <uint32_t, PoolInfo>::iterator poolIt = m_sidelinkTxPoolsMap.find (params.dstL2Id);
		NS_ASSERT (poolIt!= m_sidelinkTxPoolsMap.end());

		NistLteRadioBearerTag tag (params.rnti, params.lcid, params.srcL2Id, params.dstL2Id);
		params.pdu->AddPacketTag (tag);
		// store pdu in HARQ buffer
		poolIt->second.m_miSlHarqProcessPacket->AddPacket (params.pdu);
		m_uePhySapProvider->SendMacPdu (params.pdu);
	}
}

void
NistLteUeMac::DoReportBufferNistStatus (NistLteMacSapProvider::NistReportBufferNistStatusParameters params)
{
	NS_LOG_FUNCTION (this << (uint32_t) params.lcid);

	if (params.srcL2Id == 0) {
		NS_ASSERT (params.dstL2Id == 0);
		NS_LOG_INFO ("Reporting for uplink");
		//regular uplink BSR
		if ( ( m_NistUlScheduler =="ns3::NistPriorityFfMacScheduler") || ( m_NistUlScheduler == "ns3::NistPfFfMacScheduler") || ( m_NistUlScheduler == "ns3::NistMtFfMacScheduler" ) || ( m_NistUlScheduler == "ns3::NistRrSlFfMacScheduler"))
		{
			//NIST new iterator since Nist_m_ulBsrReceived is modified
			std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> >::iterator itNist;
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itTempMap;
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> TempMap;

			itNist = Nist_m_ulBsrReceived.find (m_rnti);
			if (itNist!=Nist_m_ulBsrReceived.end ())
			{
				// update entry

				TempMap=itNist->second;
				itTempMap=TempMap.find ((uint8_t)params.lcid);
				if (itTempMap == TempMap.end ())
				{
					itNist->second.insert (std::pair<uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> (params.lcid, params));
					m_freshUlBsr = true;
					return;
				}
				else
				{
					(*itTempMap).second = params;
					itNist->second = TempMap;
					m_freshUlBsr = true;
					return;
				}
			}

			else
			{

				std::map<uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> tempMap;
				tempMap.insert (std::pair<uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> ((uint8_t)params.lcid, params));
				Nist_m_ulBsrReceived.insert (std::pair <uint8_t, std::map<uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > > (m_rnti, tempMap));
				m_freshUlBsr = true;
				return;
			}
		}

		else if ( ( m_NistUlScheduler =="ns3::NistRrFfMacScheduler") ||  ( m_NistUlScheduler =="ns3::Nist3GPPcalMacScheduler"))
		{
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator it;
			it = m_ulBsrReceived.find (params.lcid);
			if (it!=m_ulBsrReceived.end ())
			{
				// update entry
				(*it).second = params;
			}
			else
			{
				m_ulBsrReceived.insert (std::pair<uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> (params.lcid, params));
			}
			m_freshUlBsr = true;
		}
	} else {
		NS_LOG_INFO ("Reporting for sidelink");
		//sidelink BSR
		std::map <SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator it;

		SidelinkLcIdentifier sllcid;
		sllcid.lcId = params.lcid;
		sllcid.srcL2Id = params.srcL2Id;
		sllcid.dstL2Id = params.dstL2Id;

		it = m_slBsrReceived.find (sllcid);
		if (it != m_slBsrReceived.end ())
		{
			// update entry
			(*it).second = params;
		}
		else
		{
			m_slBsrReceived.insert (std::pair<SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters> (sllcid, params));
		}
		m_freshSlBsr = true;

	}
}


void
NistLteUeMac::SendReportBufferNistStatus (void)
{
	NS_LOG_FUNCTION (this);

	if (m_rnti == 0)
	{
		NS_LOG_INFO ("MAC not initialized, BSR deferred");
		return;
	}

	if (Nist_m_ulBsrReceived.size () == 0)
	{
		NS_LOG_INFO ("No NIST BSR report to transmit");

		if (m_ulBsrReceived.size () == 0)
		{
			NS_LOG_INFO ("No BSR report to transmit");
			return;
		}
		NistMacCeListElement_s bsr;
		bsr.m_rnti = m_rnti;
		bsr.m_macCeType = NistMacCeListElement_s::BSR;

		// BSR is reported for each LCG
		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator it;
		std::vector<uint32_t> queue (4, 0); // one value per each of the 4 LCGs, initialized to 0
		for (it = m_ulBsrReceived.begin (); it != m_ulBsrReceived.end (); it++)
		{
			uint8_t lcid = it->first;
			std::map <uint8_t, NistLcInfo>::iterator lcInfoMapIt;
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
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (0)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (1)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (2)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (3)));

		// create the feedback to eNB
		Ptr<NistBsrLteControlMessage> msg = Create<NistBsrLteControlMessage> ();
		msg->SetBsr (bsr);
		m_uePhySapProvider->SendNistLteControlMessage (msg);
	}

	NistMacCeListElement_s bsr;
	bsr.m_rnti = m_rnti;
	bsr.m_macCeType = NistMacCeListElement_s::BSR;

	// BSR is reported for each LCG

	// NIST new iterator for m_ulBsrReceived
	std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
	it=Nist_m_ulBsrReceived.find(m_rnti);

	if (it!=Nist_m_ulBsrReceived.end())
	{
		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters >::iterator it1=mapLC.begin();

		std::vector<uint32_t> queue (4, 0); // one value per each of the 4 LCGs, initialized to 0
		std::vector<uint32_t> queueLCG (4, 0);                                                                                  // this queue is used to fill in the ns3 structure
		for (it1= mapLC.begin (); it1 != mapLC.end (); it1++)
		{
			uint8_t lcid = it1->first;
			std::map <uint8_t, NistLcInfo>::iterator lcInfoMapIt;
			lcInfoMapIt = m_lcInfoMap.find (lcid);
			NS_ASSERT (lcInfoMapIt !=  m_lcInfoMap.end ());
			NS_ASSERT_MSG ((lcid != 0) || (((*it1).second.txQueueSize == 0)
					&& ((*it1).second.retxQueueSize == 0)
					&& ((*it1).second.statusPduSize == 0)),
					"BSR should not be used for LCID 0");
			uint8_t lcg = lcInfoMapIt->second.lcConfig.logicalChannelGroup;
			queue.at (lcg) = ((*it1).second.txQueueSize + (*it1).second.retxQueueSize + (*it1).second.statusPduSize);
			queueLCG.at (lcg) += ((*it1).second.txQueueSize + (*it1).second.retxQueueSize + (*it1).second.statusPduSize);       // this queue is used to fill in the ns3 structure
			std::vector <uint8_t>  bufferStatus (4,0);
			bufferStatus[lcg]=NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (lcg));
			bsr.m_macCeValue.Nist_m_bufferNistStatus.insert (std::pair <uint8_t, std::vector <uint8_t> > (lcid, bufferStatus));
		}


		//filling in the structure of bsr buffer implemented by ns3 beacause if UE scheduler is RR , it will check this structure
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queueLCG.at (0)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queueLCG.at (1)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queueLCG.at (2)));
		bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queueLCG.at (3)));
		// end added

		// create the feedback to eNB
		Ptr<NistBsrLteControlMessage> msg = Create<NistBsrLteControlMessage> ();
		msg->SetBsr (bsr);
		m_uePhySapProvider->SendNistLteControlMessage (msg);
	}

}

void
NistLteUeMac::SendSidelinkReportBufferStatus (void)
{
	NS_LOG_FUNCTION (this);

	if (m_rnti == 0)
	{
		NS_LOG_INFO ("MAC not initialized, BSR deferred");
		return;
	}

	//check if we have at scheduled pools
	bool scheduled = false;
	for (std::map <uint32_t, PoolInfo >::iterator slTxPoolIt = m_sidelinkTxPoolsMap.begin (); slTxPoolIt != m_sidelinkTxPoolsMap.end () && !scheduled; slTxPoolIt++)
	{
		if (slTxPoolIt->second.m_pool->GetSchedulingType () == SidelinkCommResourcePool::SCHEDULED)
			scheduled = true;
	}



	if (m_slBsrReceived.size () == 0 || !scheduled)
	{
		NS_LOG_INFO (this << " No SL BSR report to transmit. SL BSR size=" << m_slBsrReceived.size ());
		return;
	}
	NistMacCeListElement_s bsr;
	bsr.m_rnti = m_rnti;
	bsr.m_macCeType = NistMacCeListElement_s::SLBSR;

	// SL BSR is reported for each Group Index

	std::map <SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator it;

	std::vector<uint32_t> queue (4, 0); //todo: change to create based on the number of destinations, initialized to 0
	for (it = m_slBsrReceived.begin (); it != m_slBsrReceived.end (); it++)
	{
		//uint8_t lcid = it->first.lcId;
		uint32_t dstL2Id = it->first.dstL2Id;

		std::map <SidelinkLcIdentifier, NistLcInfo>::iterator slLcInfoMapIt;
		slLcInfoMapIt = m_slLcInfoMap.find (it->first);
		NS_ASSERT (slLcInfoMapIt !=  m_slLcInfoMap.end ());
		//TODO: find the mapping between the destination and the group index (must be provided by RRC)
		//uint8_t lcg = slLcInfoMapIt->second.lcConfig.logicalChannelGroup;
		//queue.at (lcg) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
		std::map <uint32_t, PoolInfo >::iterator slTxPoolIt;
		slTxPoolIt = m_sidelinkTxPoolsMap.find (dstL2Id);
		Ptr<SidelinkTxCommResourcePool> pool = DynamicCast<SidelinkTxCommResourcePool> (slTxPoolIt->second.m_pool);
		NS_ASSERT (slTxPoolIt != m_sidelinkTxPoolsMap.end ());
		if (pool->GetSchedulingType() == SidelinkCommResourcePool::SCHEDULED)
		{
			queue.at (pool->GetIndex()) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
		}
	}

	// store
	bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (0)));
	bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (1)));
	bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (2)));
	bsr.m_macCeValue.m_bufferNistStatus.push_back (NistBufferSizeLevelBsr::BufferSize2BsrId (queue.at (3)));

	// create the feedback to eNB
	Ptr<NistBsrLteControlMessage> msg = Create<NistBsrLteControlMessage> ();
	msg->SetBsr (bsr);
	m_uePhySapProvider->SendNistLteControlMessage (msg);

}


void 
NistLteUeMac::RandomlySelectAndSendRaPreamble ()
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
NistLteUeMac::SendRaPreamble (bool contention)
{
	NS_LOG_FUNCTION (this << (uint32_t) m_raPreambleId << contention);
	// Since regular UL NistLteControlMessages need m_ulConfigured = true in
	// order to be sent by the UE, the rach preamble needs to be sent
	// with a dedicated primitive (not
	// m_uePhySapProvider->SendNistLteControlMessage (msg)) so that it can
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
	Simulator::Schedule (raWindowBegin, &NistLteUeMac::StartWaitingForRaResponse, this);
	m_noRaResponseReceivedEvent = Simulator::Schedule (raWindowEnd, &NistLteUeMac::RaResponseTimeout, this, contention);
}

void 
NistLteUeMac::StartWaitingForRaResponse ()
{
	NS_LOG_FUNCTION (this);
	m_waitingForRaResponse = true;
}

void 
NistLteUeMac::RecvRaResponse (NistBuildNistRarListElement_s raResponse)
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
	std::map <uint8_t, NistLcInfo>::iterator lc0InfoIt = m_lcInfoMap.find (lc0Lcid);
	NS_ASSERT (lc0InfoIt != m_lcInfoMap.end ());

	//added
	std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
	it=Nist_m_ulBsrReceived.find(m_rnti);
	if (it!=Nist_m_ulBsrReceived.end())
	{
		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters >::iterator lc0BsrIt;
		lc0BsrIt=mapLC.find(lc0Lcid);

		if ((lc0BsrIt != mapLC.end ())
				&& (lc0BsrIt->second.txQueueSize > 0))
		{
			NS_ASSERT_MSG (raResponse.m_grant.m_tbSize > lc0BsrIt->second.txQueueSize,
					"segmentation of Message 3 is not allowed");
			lc0InfoIt->second.macSapUser->NotifyTxOpportunity (raResponse.m_grant.m_tbSize, 0, 0);
			lc0BsrIt->second.txQueueSize = 0;
		}
	}

	else
	{

		std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator lc0BsrIt= m_ulBsrReceived.find (lc0Lcid);
		if ((lc0BsrIt != m_ulBsrReceived.end ())
				&& (lc0BsrIt->second.txQueueSize > 0))
		{
			NS_ASSERT_MSG (raResponse.m_grant.m_tbSize > lc0BsrIt->second.txQueueSize,
					"segmentation of Message 3 is not allowed");
			lc0InfoIt->second.macSapUser->NotifyTxOpportunity (raResponse.m_grant.m_tbSize, 0, 0);
			lc0BsrIt->second.txQueueSize = 0;
		}
	}
}

void 
NistLteUeMac::RaResponseTimeout (bool contention)
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
NistLteUeMac::DoConfigureRach (NistLteUeCmacSapProvider::NistRachConfig rc)
{
	NS_LOG_FUNCTION (this);
	m_rachConfig = rc;
	m_rachConfigured = true;
}

void 
NistLteUeMac::DoStartContentionBasedRandomAccessProcedure ()
{
	NS_LOG_FUNCTION (this);

	// 3GPP 36.321 5.1.1
	NS_ASSERT_MSG (m_rachConfigured, "RACH not configured");
	m_preambleTransmissionCounter = 0;
	m_backoffParameter = 0;
	RandomlySelectAndSendRaPreamble ();
}

void 
NistLteUeMac::DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
	NS_LOG_FUNCTION (this << " rnti" << rnti);
	NS_ASSERT_MSG (prachMask == 0, "requested PRACH MASK = " << (uint32_t) prachMask << ", but only PRACH MASK = 0 is supported");
	m_rnti = rnti;
	m_raPreambleId = preambleId;
	bool contention = false;
	SendRaPreamble (contention);
}

void
NistLteUeMac::DoAddLc (uint8_t lcId,  NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu)
{
	NS_LOG_FUNCTION (this << " lcId" << (uint32_t) lcId);
	NS_ASSERT_MSG (m_lcInfoMap.find (lcId) == m_lcInfoMap.end (), "cannot add channel because LCID " << lcId << " is already present");

	NistLcInfo lcInfo;
	lcInfo.lcConfig = lcConfig;
	lcInfo.macSapUser = msu;
	m_lcInfoMap[lcId] = lcInfo;
}

// added function to handle LC priority 
void
NistLteUeMac::DoAddLCPriority ( uint8_t rnti, uint8_t lcid, uint8_t priority)
{
	std::map <uint8_t, std::map <uint8_t, uint8_t> >::iterator it;
	it = PriorityMap.find(rnti);
	if (it == PriorityMap.end())
	{
		// insert new rnti in the map
		std::map<uint8_t, uint8_t> tempMap;
		tempMap.insert (std::pair<uint8_t, uint8_t> (lcid, priority));
		PriorityMap.insert (std::pair <uint8_t, std::map<uint8_t, uint8_t > > (rnti, tempMap));

	}
	else
	{
		// check if LC exists already or not
		std::map <uint8_t, uint8_t> mapLC=it->second;
		std::map <uint8_t, uint8_t>::iterator itLC;
		itLC=mapLC.find(lcid);
		if (itLC==mapLC.end())
		{
			// LC doesn't exist in the map
			it->second.insert (std::pair<uint8_t, uint8_t> (lcid, priority));
		}
	}
	return;
}

void
NistLteUeMac::DoAddLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu)
{
	NS_LOG_FUNCTION (this << (uint32_t) lcId << srcL2Id << dstL2Id);
	SidelinkLcIdentifier sllcid;
	sllcid.lcId = lcId;
	sllcid.srcL2Id = srcL2Id;
	sllcid.dstL2Id = dstL2Id;

	NS_ASSERT_MSG (m_slLcInfoMap.find (sllcid) == m_slLcInfoMap.end (), "cannot add channel because LCID " << lcId << ", srcL2Id " << srcL2Id << ", dstL2Id " << dstL2Id << " is already present");

	NistLcInfo lcInfo;
	lcInfo.lcConfig = lcConfig;
	lcInfo.macSapUser = msu;
	m_slLcInfoMap[sllcid] = lcInfo;
}

void
NistLteUeMac::DoRemoveLc (uint8_t lcId)
{
	NS_LOG_FUNCTION (this << " lcId" << lcId);
	NS_ASSERT_MSG (m_lcInfoMap.find (lcId) != m_lcInfoMap.end (), "could not find LCID " << lcId);
	m_lcInfoMap.erase (lcId);

	// added code to remove the LC from the LC priority map
	std::map <uint8_t, std::map <uint8_t, uint8_t> >::iterator it;
	it=PriorityMap.find(m_rnti);
	it->second.erase(lcId);
}

void
NistLteUeMac::DoRemoveLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
	NS_LOG_FUNCTION (this << " lcId" << lcId << ", srcL2Id=" << srcL2Id << ", dstL2Id" << dstL2Id);
	//    NS_ASSERT_MSG (m_lcInfoMap.find (lcId) != m_lcInfoMap.end (), "could not find LCID " << lcId);
	//    m_lcInfoMap.erase (lcId);
}

void
NistLteUeMac::DoReset ()
{
	NS_LOG_FUNCTION (this);
	std::map <uint8_t, NistLcInfo>::iterator it = m_lcInfoMap.begin ();
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
NistLteUeMac::DoAddSlTxPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
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
  info.m_grant_received = false; 
  m_discTxPools = info;
}

void
NistLteUeMac::DoRemoveSlTxPool ()
{
  m_discTxPools.m_pool = NULL;
}

void
NistLteUeMac::DoSetSlRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  m_discRxPools = pools;
}

void 
NistLteUeMac::DoModifyDiscTxApps (std::list<uint32_t> apps)
{
  m_discTxApps = apps;
  m_cphySapProvider->AddDiscTxApps (apps);
}

void 
NistLteUeMac::DoModifyDiscRxApps (std::list<uint32_t> apps)
{
  m_discRxApps = apps;
  m_cphySapProvider->AddDiscRxApps (apps);
}

void
NistLteUeMac::DoAddSlTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool)
{
	std::map <uint32_t, PoolInfo >::iterator it;
	it = m_sidelinkTxPoolsMap.find (dstL2Id);
	NS_ASSERT_MSG (it == m_sidelinkTxPoolsMap.end (), "Cannot add sidelink transmission pool for " << dstL2Id << ". Pool already exist for destination");
	PoolInfo info;
	info.m_pool = pool;
	info.m_npscch = info.m_pool->GetNPscch();
	info.m_currentScPeriod.frameNo = 0; //init to 0 to make it invalid
	info.m_currentScPeriod.subframeNo = 0; //init to 0 to make it invalid
	info.m_nextScPeriod = info.m_pool->GetNextScPeriod (m_frameNo, m_subframeNo);
	//adjust because scheduler starts with frame/subframe = 1
	info.m_nextScPeriod.frameNo++;
	info.m_nextScPeriod.subframeNo++;
	info.m_grant_received = false;

	m_sidelinkTxPoolsMap.insert (std::pair<uint32_t, PoolInfo > (dstL2Id, info));
}

void
NistLteUeMac::DoRemoveSlTxPool (uint32_t dstL2Id)
{
	std::map <uint32_t, PoolInfo >::iterator it;
	it = m_sidelinkTxPoolsMap.find (dstL2Id);
	NS_ASSERT_MSG (it != m_sidelinkTxPoolsMap.end (), "Cannot remove sidelink transmission pool for " << dstL2Id << ". Unknown destination");
	m_sidelinkTxPoolsMap.erase (dstL2Id);
}


void
NistLteUeMac::DoSetSlRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
	m_sidelinkRxPools = pools;
}

void
NistLteUeMac::DoSetRnti (uint16_t rnti)
{
	NS_LOG_FUNCTION (this << rnti);
	NS_ASSERT_MSG (m_rnti == 0, "Cannot manually set RNTI if already configured");
	m_rnti = rnti;
}

void
NistLteUeMac::DoReceivePhyPdu (Ptr<Packet> p)
{
	NS_LOG_FUNCTION (this);
	NistLteRadioBearerTag tag;
	p->RemovePacketTag (tag);
	if (tag.GetSourceL2Id () == 0)
	{
		if (tag.GetRnti () == m_rnti)
		{
			NS_LOG_INFO ("Received dowlink packet");
			//regular downlink packet
			// packet is for the current user
			std::map <uint8_t, NistLcInfo>::const_iterator it = m_lcInfoMap.find (tag.GetLcid ());
			if (it != m_lcInfoMap.end ())
			{
				it->second.macSapUser->ReceivePdu (p);
			}
			else
			{
				NS_LOG_WARN ("received packet with unknown lcid " << (uint32_t) tag.GetLcid ());
			}
		}
	}
	else
	{
		//sidelink packet. Perform L2 filtering
		NS_LOG_INFO ("Received sidelink packet");
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

				std::map <SidelinkLcIdentifier, NistLcInfo>::iterator it = m_slLcInfoMap.find (identifier);
				if (it == m_slLcInfoMap.end ())
				{
					//notify RRC to setup bearer
					m_cmacSapUser->NotifySidelinkReception (tag.GetLcid(), tag.GetSourceL2Id (), tag.GetDestinationL2Id ());

					//should be setup now
					it = m_slLcInfoMap.find (identifier);
					if (it == m_slLcInfoMap.end ())
					{
						NS_LOG_WARN ("Failure to setup sidelink radio bearer");
					}
				}
				it->second.macSapUser->ReceivePdu (p);

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
NistLteUeMac::DoReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
	if ( m_NistUlScheduler == "ns3::NistPfFfMacScheduler")
	{
		DoReceiveNistPFLteControlMessage (msg);
	}
	else if ( m_NistUlScheduler == "ns3::NistMtFfMacScheduler" )
	{
		DoReceiveNistMTLteControlMessage (msg);
	}
	else if ( m_NistUlScheduler == "ns3::NistPriorityFfMacScheduler" )
	{
		DoReceiveNistPrLteControlMessage (msg);
	}
	else if (m_NistUlScheduler == "ns3::NistRrSlFfMacScheduler")
	{
		DoReceiveNistRrLteControlMessage (msg);
	}
	else
	{
		DoReceiveNistRrLteControlMessage (msg);
		//std::cout<<" UE RR SCHEDULER "<<std::endl;
	}
}

void
NistLteUeMac::DoReceiveNistRrLteControlMessage (Ptr<NistLteControlMessage> msg)
{
	//std::cout<<" ENTER DoReceiveNistRrLteControlMessage "<<std::endl;
	NS_LOG_FUNCTION (this);
	if (msg->GetMessageType () == NistLteControlMessage::UL_DCI)
	{
		Ptr<NistUlDciLteControlMessage> msg2 = DynamicCast<NistUlDciLteControlMessage> (msg);
		NistUlDciListElement_s dci = msg2->GetDci ();
		if (dci.m_ndi == 1)
		{
			// New transmission -> emtpy pkt buffer queue (for deleting eventual pkts not acked )
			Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
			m_miUlHarqProcessesPacket.at (m_harqProcessId) = pb;
			// Retrieve data from RLC
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
			uint16_t activeLcs = 0;
			uint32_t statusPduMinSize = 0;
			//added code
			if (m_ulBsrReceived.size()!=0)
			{
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
				std::map <uint8_t, NistLcInfo>::iterator it;
				uint32_t bytesPerActiveLc = dci.m_tbSize / activeLcs;
				bool statusPduPriority = false;
				if ((statusPduMinSize != 0)&&(bytesPerActiveLc < statusPduMinSize))
				{
					// send only the status PDU which has highest priority
					statusPduPriority = true;
					NS_LOG_DEBUG (this << " Reduced resource -> send only NistStatus, b ytes " << statusPduMinSize);
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
							(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
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
								(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
			} // end if m_ulBsrReceived.size()!=0
			else
			{
				// NIST IMPLEMENTATION OF Nist_m_ulBsrReceived
				uint16_t activeLcs = 0;
				uint32_t statusPduMinSize = 0;
				std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
				it=Nist_m_ulBsrReceived.find(m_rnti);
				if (it!=Nist_m_ulBsrReceived.end())
				{
					// Retrieve data from RLC
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;

					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
						{
							activeLcs++;
							if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
							if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
						}// end if
					}// end for

					if (activeLcs == 0)
					{
						NS_LOG_ERROR (this << " No active flows for this UL-DCI");
						return;
					}
					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						// compute tb size for this lc
						uint32_t bytesPerActiveLc= dci.m_tbSize / activeLcs ;
						std::map <uint8_t, NistLcInfo>::iterator itLcInfo;
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

						if ( (itBsr!=mapLC.end ()) &&
								( ((*itBsr).second.statusPduSize > 0) ||
										((*itBsr).second.retxQueueSize > 0) ||
										((*itBsr).second.txQueueSize > 0)) )
						{
							itLcInfo=m_lcInfoMap.find((*itBsr).first);

							if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
							{

								(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);

								(*itBsr).second.statusPduSize = 0;
							}
							else
							{

								uint32_t bytesForThisLc = bytesPerActiveLc;
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " bytes to LC " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
								if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
								{
									(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
									bytesForThisLc -= (*itBsr).second.statusPduSize;
									NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
									(*itBsr).second.statusPduSize = 0;

								}
								else
								{
									if ((*itBsr).second.statusPduSize>bytesForThisLc)
									{
										NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
									}
								}

								if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
										(((*itBsr).second.retxQueueSize > 0) ||
												((*itBsr).second.txQueueSize > 0)))
								{
									if ((*itBsr).second.retxQueueSize > 0)
									{
										NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);

										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
										uint16_t lcid = (*itLcInfo).first;
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
										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << "\t new queues " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
							}

						}
					}
				}// end for p
			} // end it!=Nist_m_ulBsrReceived.end()

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
	else if (msg->GetMessageType () == NistLteControlMessage::RAR)
	{
		if (m_waitingForRaResponse)
		{
			Ptr<NistRarLteControlMessage> rarMsg = DynamicCast<NistRarLteControlMessage> (msg);
			uint16_t raRnti = rarMsg->GetRaRnti ();
			NS_LOG_LOGIC (this << "got RAR with RA-RNTI " << (uint32_t) raRnti << ", expecting " << (uint32_t) m_raRnti);
			if (raRnti == m_raRnti) // RAR corresponds to TX subframe of preamble
			{
				for (std::list<NistRarLteControlMessage::NistRar>::const_iterator it = rarMsg->NistRarListBegin ();
						it != rarMsg->NistRarListEnd ();
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
	else if (msg->GetMessageType () == NistLteControlMessage::SL_DCI)
	{
		Ptr<NistSlDciLteControlMessage> msg2 = DynamicCast<NistSlDciLteControlMessage> (msg);
		NistSlDciListElement_s dci = msg2->GetDci ();

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
		grant.m_trp = dci.m_trp;
		grant.m_mcs = pool->GetMcs();
		grant.m_tbSize = 0; //computed later
		m_sidelinkTxPoolsMap.begin()->second.m_nextGrant = grant;
		m_sidelinkTxPoolsMap.begin()->second.m_grant_received = true;

		NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
	}

  else if (msg->GetMessageType () == NistLteControlMessage::SL_DISC_MSG)
  {
    NS_LOG_INFO (this << " Received discovery message");
    //notify RRC (pass msg to RRC where we can filter)
    m_cmacSapUser->NotifyDiscoveryReception (msg);
  }

	else
	{
		NS_LOG_WARN (this << " NistLteControlMessage not recognized");
	}
}

void
NistLteUeMac::DoReceiveNistPFLteControlMessage (Ptr<NistLteControlMessage> msg)
{

	NS_LOG_FUNCTION (this);
	if (msg->GetMessageType () == NistLteControlMessage::UL_DCI)
	{
		Ptr<NistUlDciLteControlMessage> msg2 = DynamicCast<NistUlDciLteControlMessage> (msg);
		NistUlDciListElement_s dci = msg2->GetDci ();
		if (dci.m_ndi==1)
		{
			// New transmission -> emtpy pkt buffer queue (for deleting eventual pkts not acked )
			Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
			m_miUlHarqProcessesPacket.at (m_harqProcessId) = pb;
			// Retrieve data from RLC
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
			uint16_t activeLcs = 0;
			uint32_t statusPduMinSize = 0;
			//added code
			if (m_ulBsrReceived.size()!=0)
			{
				for (itBsr = m_ulBsrReceived.begin (); itBsr != m_ulBsrReceived.end (); itBsr++)
				{
					if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
					{
						activeLcs++;
						if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
						{
							statusPduMinSize = (*itBsr).second.statusPduSize;
						}
						if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
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
				std::map <uint8_t, NistLcInfo>::iterator it;
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
				for (it = m_lcInfoMap.begin (); it!=m_lcInfoMap.end (); it++)
				{
					itBsr = m_ulBsrReceived.find ((*it).first);
					NS_LOG_DEBUG (this << " Processing LC " << (uint32_t)(*it).first << " bytesPerActiveLc " << bytesPerActiveLc);
					if ( (itBsr!=m_ulBsrReceived.end ()) &&
							( ((*itBsr).second.statusPduSize > 0) ||
									((*itBsr).second.retxQueueSize > 0) ||
									((*itBsr).second.txQueueSize > 0)) )
					{
						if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
						{
							(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
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
								(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
								bytesForThisLc -= (*itBsr).second.statusPduSize;
								NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
								(*itBsr).second.statusPduSize = 0;
							}
							else
							{
								if ((*itBsr).second.statusPduSize>bytesForThisLc)
								{
									NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
								}
							}

							if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
									(((*itBsr).second.retxQueueSize > 0) ||
											((*itBsr).second.txQueueSize > 0)))
							{
								if ((*itBsr).second.retxQueueSize > 0)
								{
									NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
			} // end if m_ulBsrReceived.size()!=0
			else
			{
				// NIST IMPLEMENTATION OF Nist_m_ulBsrReceived
				uint16_t activeLcs = 0;
				uint32_t statusPduMinSize = 0;
				std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
				it=Nist_m_ulBsrReceived.find(m_rnti);
				if (it!=Nist_m_ulBsrReceived.end())
				{
					// Retrieve data from RLC
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;

					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
						{
							activeLcs++;
							if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
							if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
						}// end if
					}// end for

					if (activeLcs == 0)
					{
						NS_LOG_ERROR (this << " No active flows for this UL-DCI");
						return;
					}

					uint32_t totalQueue =0;
					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						totalQueue=totalQueue + (*itBsr).second.txQueueSize;
					}
					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						// compute tb size for this lc
						uint32_t bytesPerActiveLc= dci.m_tbSize / activeLcs ;
						if (totalQueue > dci.m_tbSize)
						{
							double pfCoef= (double) (*itBsr).second.txQueueSize / (double)totalQueue;
							bytesPerActiveLc = floor (pfCoef * (double)dci.m_tbSize);
						}
						std::map <uint8_t, NistLcInfo>::iterator itLcInfo;
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

						if ( (itBsr!=mapLC.end ()) &&
								( ((*itBsr).second.statusPduSize > 0) ||
										((*itBsr).second.retxQueueSize > 0) ||
										((*itBsr).second.txQueueSize > 0)) )
						{
							itLcInfo=m_lcInfoMap.find((*itBsr).first);

							if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
							{

								(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);

								(*itBsr).second.statusPduSize = 0;
							}
							else
							{

								uint32_t bytesForThisLc = bytesPerActiveLc;
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " bytes to LC " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
								if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
								{
									(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
									bytesForThisLc -= (*itBsr).second.statusPduSize;
									NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
									(*itBsr).second.statusPduSize = 0;

								}
								else
								{
									if ((*itBsr).second.statusPduSize>bytesForThisLc)
									{
										NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
									}
								}

								if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
										(((*itBsr).second.retxQueueSize > 0) ||
												((*itBsr).second.txQueueSize > 0)))
								{
									if ((*itBsr).second.retxQueueSize > 0)
									{
										NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);

										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
										uint16_t lcid = (*itLcInfo).first;
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
										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << "\t new queues " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
							}

						}
					}
				}// end for p
			} // end it!=Nist_m_ulBsrReceived.end()
		} //end if ndi
		else  //else ndi
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
	else if (msg->GetMessageType () == NistLteControlMessage::RAR)
	{
		if (m_waitingForRaResponse)
		{
			Ptr<NistRarLteControlMessage> rarMsg = DynamicCast<NistRarLteControlMessage> (msg);
			uint16_t raRnti = rarMsg->GetRaRnti ();
			NS_LOG_LOGIC (this << "got RAR with RA-RNTI " << (uint32_t) raRnti << ", expecting " << (uint32_t) m_raRnti);
			if (raRnti == m_raRnti) // RAR corresponds to TX subframe of preamble
			{
				for (std::list<NistRarLteControlMessage::NistRar>::const_iterator it = rarMsg->NistRarListBegin ();
						it != rarMsg->NistRarListEnd ();
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
	else if (msg->GetMessageType () == NistLteControlMessage::SL_DCI)
	{
		Ptr<NistSlDciLteControlMessage> msg2 = DynamicCast<NistSlDciLteControlMessage> (msg);
		NistSlDciListElement_s dci = msg2->GetDci ();

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
		grant.m_trp = dci.m_trp;
		grant.m_mcs = pool->GetMcs();
		grant.m_tbSize = 0; //computed later
		m_sidelinkTxPoolsMap.begin()->second.m_nextGrant = grant;
		m_sidelinkTxPoolsMap.begin()->second.m_grant_received = true;

		NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
	}

 else if (msg->GetMessageType () == NistLteControlMessage::SL_DISC_MSG)
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
NistLteUeMac::DoReceiveNistMTLteControlMessage (Ptr<NistLteControlMessage> msg)
{
	NS_LOG_FUNCTION (this);
	if (msg->GetMessageType () == NistLteControlMessage::UL_DCI)
	{
		Ptr<NistUlDciLteControlMessage> msg2 = DynamicCast<NistUlDciLteControlMessage> (msg);
		NistUlDciListElement_s dci = msg2->GetDci ();
		if (dci.m_ndi==1)
		{
			// New transmission -> emtpy pkt buffer queue (for deleting eventual pkts not acked )
			Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
			m_miUlHarqProcessesPacket.at (m_harqProcessId) = pb;
			// Retrieve data from RLC
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
			uint16_t activeLcs = 0;
			uint32_t statusPduMinSize = 0;
			//added code
			if (m_ulBsrReceived.size()!=0)
			{
				for (itBsr = m_ulBsrReceived.begin (); itBsr != m_ulBsrReceived.end (); itBsr++)
				{
					if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
					{
						activeLcs++;
						if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
						{
							statusPduMinSize = (*itBsr).second.statusPduSize;
						}
						if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
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
				std::map <uint8_t, NistLcInfo>::iterator it;
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
				for (it = m_lcInfoMap.begin (); it!=m_lcInfoMap.end (); it++)
				{
					itBsr = m_ulBsrReceived.find ((*it).first);
					NS_LOG_DEBUG (this << " Processing LC " << (uint32_t)(*it).first << " bytesPerActiveLc " << bytesPerActiveLc);
					if ( (itBsr!=m_ulBsrReceived.end ()) &&
							( ((*itBsr).second.statusPduSize > 0) ||
									((*itBsr).second.retxQueueSize > 0) ||
									((*itBsr).second.txQueueSize > 0)) )
					{
						if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
						{
							(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
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
								(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
								bytesForThisLc -= (*itBsr).second.statusPduSize;
								NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
								(*itBsr).second.statusPduSize = 0;
							}
							else
							{
								if ((*itBsr).second.statusPduSize>bytesForThisLc)
								{
									NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
								}
							}

							if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
									(((*itBsr).second.retxQueueSize > 0) ||
											((*itBsr).second.txQueueSize > 0)))
							{
								if ((*itBsr).second.retxQueueSize > 0)
								{
									NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
			} // end if m_ulBsrReceived.size()!=0
			else
			{
				// NIST IMPLEMENTATION OF Nist_m_ulBsrReceived
				uint16_t activeLcs = 0;
				uint32_t statusPduMinSize = 0;
				std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
				it=Nist_m_ulBsrReceived.find(m_rnti);
				if (it!=Nist_m_ulBsrReceived.end())
				{
					// Retrieve data from RLC
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;

					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
						{
							activeLcs++;
							if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
							if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
						}// end if
					}// end for

					if (activeLcs == 0)
					{
						NS_LOG_ERROR (this << " No active flows for this UL-DCI");
						return;
					}

					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						// compute tb size for this lc
						uint32_t bytesPerActiveLc= dci.m_tbSize / activeLcs ;
						std::map <uint8_t, NistLcInfo>::iterator itLcInfo;
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

						if ( (itBsr!=mapLC.end ()) &&
								( ((*itBsr).second.statusPduSize > 0) ||
										((*itBsr).second.retxQueueSize > 0) ||
										((*itBsr).second.txQueueSize > 0)) )
						{
							itLcInfo=m_lcInfoMap.find((*itBsr).first);

							if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
							{

								(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);

								(*itBsr).second.statusPduSize = 0;
							}
							else
							{

								uint32_t bytesForThisLc = bytesPerActiveLc;
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " bytes to LC " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
								if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
								{
									(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
									bytesForThisLc -= (*itBsr).second.statusPduSize;
									NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
									(*itBsr).second.statusPduSize = 0;

								}
								else
								{
									if ((*itBsr).second.statusPduSize>bytesForThisLc)
									{
										NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
									}
								}

								if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
										(((*itBsr).second.retxQueueSize > 0) ||
												((*itBsr).second.txQueueSize > 0)))
								{
									if ((*itBsr).second.retxQueueSize > 0)
									{
										NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);

										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
										uint16_t lcid = (*itLcInfo).first;
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
										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << "\t new queues " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
							}

						}
					}
				}// end for p
			} // end it!=Nist_m_ulBsrReceived.end()
		} //end if ndi
		else  //else ndi
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
	else if (msg->GetMessageType () == NistLteControlMessage::RAR)
	{
		if (m_waitingForRaResponse)
		{
			Ptr<NistRarLteControlMessage> rarMsg = DynamicCast<NistRarLteControlMessage> (msg);
			uint16_t raRnti = rarMsg->GetRaRnti ();
			NS_LOG_LOGIC (this << "got RAR with RA-RNTI " << (uint32_t) raRnti << ", expecting " << (uint32_t) m_raRnti);
			if (raRnti == m_raRnti) // RAR corresponds to TX subframe of preamble
			{
				for (std::list<NistRarLteControlMessage::NistRar>::const_iterator it = rarMsg->NistRarListBegin ();
						it != rarMsg->NistRarListEnd ();
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
	else if (msg->GetMessageType () == NistLteControlMessage::SL_DCI)
	{
		Ptr<NistSlDciLteControlMessage> msg2 = DynamicCast<NistSlDciLteControlMessage> (msg);
		NistSlDciListElement_s dci = msg2->GetDci ();

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
		grant.m_trp = dci.m_trp;
		grant.m_mcs = pool->GetMcs();
		grant.m_tbSize = 0; //computed later
		m_sidelinkTxPoolsMap.begin()->second.m_nextGrant = grant;
		m_sidelinkTxPoolsMap.begin()->second.m_grant_received = true;

		NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
	}

  else if (msg->GetMessageType () == NistLteControlMessage::SL_DISC_MSG)
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
NistLteUeMac::DoReceiveNistPrLteControlMessage (Ptr<NistLteControlMessage> msg)
{
	NS_LOG_FUNCTION (this);
	if (msg->GetMessageType () == NistLteControlMessage::UL_DCI)
	{
		Ptr<NistUlDciLteControlMessage> msg2 = DynamicCast<NistUlDciLteControlMessage> (msg);
		NistUlDciListElement_s dci = msg2->GetDci ();
		if (dci.m_ndi==1)
		{
			// New transmission -> emtpy pkt buffer queue (for deleting eventual pkts not acked )
			Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
			m_miUlHarqProcessesPacket.at (m_harqProcessId) = pb;
			// Retrieve data from RLC
			std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
			uint16_t activeLcs = 0;
			uint32_t statusPduMinSize = 0;

			if (m_ulBsrReceived.size()!=0)
			{
				for (itBsr = m_ulBsrReceived.begin (); itBsr != m_ulBsrReceived.end (); itBsr++)
				{
					if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
					{
						activeLcs++;
						if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
						{
							statusPduMinSize = (*itBsr).second.statusPduSize;
						}
						if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
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
				std::map <uint8_t, NistLcInfo>::iterator it;
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
				for (it = m_lcInfoMap.begin (); it!=m_lcInfoMap.end (); it++)
				{
					itBsr = m_ulBsrReceived.find ((*it).first);
					NS_LOG_DEBUG (this << " Processing LC " << (uint32_t)(*it).first << " bytesPerActiveLc " << bytesPerActiveLc);
					if ( (itBsr!=m_ulBsrReceived.end ()) &&
							( ((*itBsr).second.statusPduSize > 0) ||
									((*itBsr).second.retxQueueSize > 0) ||
									((*itBsr).second.txQueueSize > 0)) )
					{
						if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
						{
							(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
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
								(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
								bytesForThisLc -= (*itBsr).second.statusPduSize;
								NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
								(*itBsr).second.statusPduSize = 0;
							}
							else
							{
								if ((*itBsr).second.statusPduSize>bytesForThisLc)
								{
									NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
								}
							}

							if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
									(((*itBsr).second.retxQueueSize > 0) ||
											((*itBsr).second.txQueueSize > 0)))
							{
								if ((*itBsr).second.retxQueueSize > 0)
								{
									NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
			} // end if m_ulBsrReceived.size()!=0
			else
			{
				// NIST IMPLEMENTATION OF Nist_m_ulBsrReceived
				uint16_t activeLcs = 0;
				uint32_t statusPduMinSize = 0;
				std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > >:: iterator it;
				it=Nist_m_ulBsrReceived.find(m_rnti);
				if (it!=Nist_m_ulBsrReceived.end())
				{
					// Retrieve data from RLC
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > mapLC=it->second;
					std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;

					for (itBsr = mapLC.begin (); itBsr != mapLC.end (); itBsr++)
					{
						if (((*itBsr).second.statusPduSize > 0) || ((*itBsr).second.retxQueueSize > 0) || ((*itBsr).second.txQueueSize > 0))
						{
							activeLcs++;
							if (((*itBsr).second.statusPduSize!=0)&&((*itBsr).second.statusPduSize < statusPduMinSize))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
							if (((*itBsr).second.statusPduSize!=0)&&(statusPduMinSize == 0))
							{
								statusPduMinSize = (*itBsr).second.statusPduSize;
							}
						}// end if
					}// end for

					if (activeLcs == 0)
					{
						NS_LOG_ERROR (this << " No active flows for this UL-DCI");
						return;
					}

					std::vector<uint8_t> TreatedLCs;

					std::map <uint8_t, std::map <uint8_t, uint8_t> >::iterator itP;
					std::map <uint8_t, uint8_t> mapLCP;
					std::map <uint8_t, uint8_t>::iterator itLCP;
					itP= PriorityMap.find(m_rnti);
					mapLCP=itP->second;
					uint16_t TbTemp=dci.m_tbSize;
					for (uint16_t p=0; p < activeLcs; p++)
					{
						uint8_t MinPriority=10;
						uint8_t lcidMin=0;

						for (itLCP= mapLCP.begin (); itLCP != mapLCP.end (); itLCP++)
						{
							//CHECK IF THIS LCID IS ALREADY TREATED
							std::vector <uint8_t>::iterator Ft;
							bool findF=false;
							for(Ft=TreatedLCs.begin();Ft!=TreatedLCs.end();Ft++)
							{
								if((*Ft)==(*itLCP).first)
								{
									findF=true;

									break;
								}
							}
							if (findF==true)
							{
								MinPriority=10;
								continue;
							}
							if (findF==false)
							{
								//this LC doesn't exist in Treated LC
								if ((*itLCP).second < MinPriority )
								{
									lcidMin=(*itLCP).first;

									MinPriority=(*itLCP).second;

								}
							}
						}//end for

						TreatedLCs.push_back (lcidMin);
						// compute tb size for this lc
						itBsr=mapLC.find (lcidMin);
						uint32_t bytesPerActiveLc;
						if ( TbTemp >= (*itBsr).second.txQueueSize )
						{
							bytesPerActiveLc=(*itBsr).second.txQueueSize;
							TbTemp = TbTemp - (*itBsr).second.txQueueSize;
						}
						else
						{
							bytesPerActiveLc = TbTemp;
							TbTemp=0;
						}
						std::map <uint8_t, NistLcInfo>::iterator itLcInfo;
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

						itBsr=mapLC.begin();
						itBsr = mapLC.find (lcidMin);

						if ( (itBsr!=mapLC.end ()) &&
								( ((*itBsr).second.statusPduSize > 0) ||
										((*itBsr).second.retxQueueSize > 0) ||
										((*itBsr).second.txQueueSize > 0)) )
						{
							itLcInfo=m_lcInfoMap.find((*itBsr).first);

							if ((statusPduPriority) && ((*itBsr).second.statusPduSize == statusPduMinSize))
							{

								(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);

								(*itBsr).second.statusPduSize = 0;
							}
							else
							{

								uint32_t bytesForThisLc = bytesPerActiveLc;
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << " bytes to LC " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
								if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
								{
									(*itLcInfo).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
									bytesForThisLc -= (*itBsr).second.statusPduSize;
									NS_LOG_DEBUG (this << " serve STATUS " << (*itBsr).second.statusPduSize);
									(*itBsr).second.statusPduSize = 0;

								}
								else
								{
									if ((*itBsr).second.statusPduSize>bytesForThisLc)
									{
										NS_FATAL_ERROR ("Insufficient Tx Opportunity for sending a status message");
									}
								}

								if ((bytesForThisLc > 7) && // 7 is the min TxOpportunity useful for Rlc
										(((*itBsr).second.retxQueueSize > 0) ||
												((*itBsr).second.txQueueSize > 0)))
								{
									if ((*itBsr).second.retxQueueSize > 0)
									{
										NS_LOG_DEBUG (this << " serve retx DATA, bytes " << bytesForThisLc);

										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
										uint16_t lcid = (*itLcInfo).first;
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
										(*itLcInfo).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
								NS_LOG_LOGIC (this << "\t" << bytesPerActiveLc << "\t new queues " << (uint32_t)(*itLcInfo).first << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
							}

						}
					}
				}// end for p
			} // end it!=Nist_m_ulBsrReceived.end()
		} //end if ndi
		else  //else ndi
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
	else if (msg->GetMessageType () == NistLteControlMessage::RAR)
	{
		if (m_waitingForRaResponse)
		{
			Ptr<NistRarLteControlMessage> rarMsg = DynamicCast<NistRarLteControlMessage> (msg);
			uint16_t raRnti = rarMsg->GetRaRnti ();
			NS_LOG_LOGIC (this << "got RAR with RA-RNTI " << (uint32_t) raRnti << ", expecting " << (uint32_t) m_raRnti);
			if (raRnti == m_raRnti) // RAR corresponds to TX subframe of preamble
			{
				for (std::list<NistRarLteControlMessage::NistRar>::const_iterator it = rarMsg->NistRarListBegin ();
						it != rarMsg->NistRarListEnd ();
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
	else if (msg->GetMessageType () == NistLteControlMessage::SL_DCI)
	{
		Ptr<NistSlDciLteControlMessage> msg2 = DynamicCast<NistSlDciLteControlMessage> (msg);
		NistSlDciListElement_s dci = msg2->GetDci ();

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
		grant.m_trp = dci.m_trp;
		grant.m_mcs = pool->GetMcs();
		grant.m_tbSize = 0; //computed later
		m_sidelinkTxPoolsMap.begin()->second.m_nextGrant = grant;
		m_sidelinkTxPoolsMap.begin()->second.m_grant_received = true;

		NS_LOG_INFO (this << "Received SL_DCI message rnti=" << m_rnti << " res=" << (uint16_t) dci.m_resPscch);
	}

  else if (msg->GetMessageType () == NistLteControlMessage::SL_DISC_MSG)
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
NistLteUeMac::RefreshHarqProcessesPacketBuffer (void)
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
NistLteUeMac::DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
	NS_LOG_FUNCTION (this << " Frame no. " << frameNo << " subframe no. " << subframeNo);
	m_frameNo = frameNo;
	m_subframeNo = subframeNo;


	RefreshHarqProcessesPacketBuffer ();
	if ((Simulator::Now () >= m_bsrLast + m_bsrPeriodicity) && (m_freshUlBsr == true))
	{
		SendReportBufferNistStatus ();
		m_bsrLast = Simulator::Now ();
		m_freshUlBsr = false;
		m_harqProcessId = (m_harqProcessId + 1) % HARQ_PERIOD;
	}

	//sidelink processes

	//there is a delay between the MAC scheduling and the transmission so we assume that we are ahead
	subframeNo += 4;
	if (subframeNo > 10)
	{
		++frameNo;
		if (frameNo > 1024)
			frameNo = 1;
		subframeNo -= 10;
	}
	NS_LOG_INFO (this << " Adjusted Frame no. " << frameNo << " subframe no. " << subframeNo);


  //discovery
  //Check if this is a new disc period
  if (frameNo == m_discTxPools.m_nextDiscPeriod.frameNo && subframeNo == m_discTxPools.m_nextDiscPeriod.subframeNo)
  {
    //define periods and frames
    m_discTxPools.m_currentDiscPeriod = m_discTxPools.m_nextDiscPeriod;
    m_discTxPools.m_nextDiscPeriod = m_discTxPools.m_pool->GetNextDiscPeriod (frameNo, subframeNo);
    m_discTxPools.m_nextDiscPeriod.frameNo++;
    m_discTxPools.m_nextDiscPeriod.subframeNo++;
    NS_LOG_INFO (this << " starting new discovery period " << ". Next period at " << m_discTxPools.m_nextDiscPeriod.frameNo << "/" << m_discTxPools.m_nextDiscPeriod.subframeNo);
   
    if (m_discTxPools.m_pool->GetSchedulingType() == SidelinkDiscResourcePool::UE_SELECTED) 
    {
        //use txProbability
        DiscGrant grant;
        double p1 = m_p1UniformVariable->GetValue (0, 1);
        
        double txProbability = m_discTxPools.m_pool->GetTxProbability (); //calculate txProbability
        if (p1 <= txProbability/100)
        {
          grant.m_resPsdch = m_resUniformVariable->GetInteger (0, m_discTxPools.m_npsdch-1);
          grant.m_rnti = m_rnti;
          m_discTxPools.m_nextGrant = grant;
          m_discTxPools.m_grant_received = true;
          NS_LOG_INFO (this << " UE selected grant: resource=" << (uint16_t) grant.m_resPsdch << "/" << m_discTxPools.m_npsdch);
        }
    }
    else //scheduled
    {
      //TODO
      //use defined grant : SL-TF-IndexPair
    } 

    //if we received a grant
    if (m_discTxPools.m_grant_received)
    {
      m_discTxPools.m_currentGrant = m_discTxPools.m_nextGrant;
      NS_LOG_INFO (this << " Discovery grant received resource " << (uint32_t) m_discTxPools.m_currentGrant.m_resPsdch);  

      SidelinkDiscResourcePool::SubframeInfo tmp;
      tmp.frameNo = m_discTxPools.m_currentDiscPeriod.frameNo-1;
      tmp.subframeNo = m_discTxPools.m_currentDiscPeriod.subframeNo-1;
            
      m_discTxPools.m_psdchTx = m_discTxPools.m_pool->GetPsdchTransmissions (m_discTxPools.m_currentGrant.m_resPsdch);
      for (std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator txIt = m_discTxPools.m_psdchTx.begin (); txIt != m_discTxPools.m_psdchTx.end (); txIt++)
      {
        txIt->subframe = txIt->subframe + tmp;
        //adjust for index starting at 1
        txIt->subframe.frameNo++;
        txIt->subframe.subframeNo++;
        NS_LOG_INFO (this << " PSDCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
        //std::cout <<  " PSDCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb << std::endl;
      }
        
      //Inform PHY: find a way to inform the PHY layer of the resources
      m_cphySapProvider->SetDiscGrantInfo (m_discTxPools.m_currentGrant.m_resPsdch);   
      //clear the grant
      m_discTxPools.m_grant_received = false;
    }
  }
  std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
  //check if we need to transmit PSDCH
  allocIt = m_discTxPools.m_psdchTx.begin();
  if (allocIt != m_discTxPools.m_psdchTx.end() && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
  {
    NS_LOG_INFO (this << "PSDCH transmission");
    for (std::list<uint32_t>::iterator txApp = m_discTxApps.begin (); txApp != m_discTxApps.end (); ++txApp)
    {
           
      //Create Discovery message for each discovery application announcing
      NistSlDiscMsg discMsg;
      discMsg.m_rnti = m_rnti;
      discMsg.m_resPsdch = m_discTxPools.m_currentGrant.m_resPsdch;

      discMsg.m_proSeAppCode =  (std::bitset <184>)*txApp;

      Ptr<NistSlDiscMessage> msg = Create<NistSlDiscMessage> ();
      msg->SetSlDiscMessage (discMsg);
      NS_LOG_INFO ("discovery message sent by " << m_rnti << ", proSeAppCode = " << *txApp);
      m_discoveryAnnouncementTrace (m_rnti, *txApp);
      m_uePhySapProvider->SendNistLteControlMessage (msg);
     
    }
    m_discTxPools.m_psdchTx.erase (allocIt);
  } 

  //communication
	if ((Simulator::Now () >= m_slBsrLast + m_slBsrPeriodicity) && (m_freshSlBsr == true))
	{
		SendSidelinkReportBufferStatus ();
		m_slBsrLast = Simulator::Now ();
		m_freshSlBsr = false;
		//m_harqProcessId = (m_harqProcessId + 1) % HARQ_PERIOD; //is this true?
	}

	std::map <uint32_t, PoolInfo>::iterator poolIt;
	for (poolIt = m_sidelinkTxPoolsMap.begin() ; poolIt != m_sidelinkTxPoolsMap.end() ; poolIt++)
	{
		//Check if this is a new SC period
		if (frameNo == poolIt->second.m_nextScPeriod.frameNo && subframeNo == poolIt->second.m_nextScPeriod.subframeNo)
		{
			poolIt->second.m_currentScPeriod = poolIt->second.m_nextScPeriod;
			poolIt->second.m_nextScPeriod = poolIt->second.m_pool->GetNextScPeriod (frameNo, subframeNo);
			//adjust because scheduler starts with frame/subframe = 1
			poolIt->second.m_nextScPeriod.frameNo++;
			poolIt->second.m_nextScPeriod.subframeNo++;
			NS_LOG_INFO (this << " Starting new SC period for pool of group " << poolIt->first << ". Next period at " << poolIt->second.m_nextScPeriod.frameNo << "/" << poolIt->second.m_nextScPeriod.subframeNo);

			Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
			poolIt->second.m_miSlHarqProcessPacket = emptyPb;

			if (poolIt->second.m_pool->GetSchedulingType() == SidelinkCommResourcePool::UE_SELECTED)
			{

				//If m_slHasDataToTx is False here (at the beginning of the period), it means
				//that no transmissions in the PSSCH occurred in the previous SC period.
				//Notify the RRC for stopping SLSS transmissions if appropriate
				if (!m_slHasDataToTx){
					m_cmacSapUser->NotifyMacHasNotSlDataToSend();
				}
				//Make m_slHasDataToTx = false here (beginning of the period) to detect if transmissions
				//in the PSSCH are performed in this period
				m_slHasDataToTx=false;

				//get the BSR for this pool
				//if we have data in the queue
				//find the BSR for that pool (will also give the SidleinkLcIdentifier)
				std::map <SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
				for (itBsr = m_slBsrReceived.begin () ;  itBsr != m_slBsrReceived.end () ; itBsr++)
				{
					if (itBsr->first.dstL2Id == poolIt->first)
					{
						//this is the BSR for the pool
						break;
					}

				}

				if (itBsr == m_slBsrReceived.end () || (*itBsr).second.txQueueSize == 0)
				{
					NS_LOG_INFO (this << " no BSR received. Assume no data to transfer");

				}
				else
				{
					//we need to pick a random resource from the pool
					//NS_ASSERT_MSG (0, "UE_SELECTED pools not implemented");
					NS_LOG_DEBUG (this << "SL BSR size=" << m_slBsrReceived.size ());
					SidelinkGrant grant;
					//in order to pick a resource that is valid, we compute the number of suchannels
					//on the PSSCH
					NS_ASSERT_MSG (m_pucchSize % 2 == 0, "Number of RBs for PUCCH must be multiple of 2");
					//TODO: add function so the RRC tells the MAC what the UL bandwidth is.
					//currently only the phy has it
					uint16_t nbSubchannels = std::floor ((50 - m_pucchSize) / m_slGrantSize);
					uint16_t nbTxOpt = poolIt->second.m_npscch;//before was (-1)

					grant.m_resPscch = m_ueSelectedUniformVariable->GetInteger (0, nbTxOpt-1); //Randomly selected Resource in PSCCH.
					grant.m_tpc = 0;
					grant.m_hopping = 0;
					uint16_t subCh = 0;
					subCh = m_ueSelectedUniformVariable->GetInteger (0, nbSubchannels-1);
					switch (m_slKtrp)
					{
					case 1:
						grant.m_trp = m_ueSelectedUniformVariable->GetInteger (0, 7);
						break;
					case 2:
						grant.m_trp = m_ueSelectedUniformVariable->GetInteger (8, 35);
						break;
					case 4:
						grant.m_trp = m_ueSelectedUniformVariable->GetInteger (36, 105);
						break;
					case 8:
						grant.m_trp = 106;
						break;
					default:
						NS_FATAL_ERROR ("Invalid KTRP value " << (uint16_t) m_slKtrp << ". Supported values: [1, 2, 4, 8]");
					}

					grant.m_rbStart = m_pucchSize / 2 + m_slGrantSize * subCh;
					grant.m_rbLen = m_slGrantSize;


					//grant.m_trp = (uint16_t) std::floor (grant.m_resPscch / nbSubchannels)/*m_slItrp*/;
					grant.m_mcs = m_slGrantMcs;
					grant.m_tbSize = 0; //computed later
					poolIt->second.m_nextGrant = grant;
					poolIt->second.m_grant_received = true;
					NS_LOG_INFO (this << " UE selected grant: resource=" << (uint16_t) grant.m_resPscch << "/" << poolIt->second.m_npscch << ", rbStart=" << (uint16_t) grant.m_rbStart << ", rbLen=" << (uint16_t) grant.m_rbLen << ", mcs=" << (uint16_t) grant.m_mcs << ", ch=" << subCh << ",itrp=" << (uint16_t) grant.m_trp);

					/* // Trace SL UE mac scheduling
		                NistSlUeMacStatParameters stats_params;
		                stats_params.m_timestamp = Simulator::Now ().GetMilliSeconds ();
		                stats_params.m_frameNo = frameNo;
		                stats_params.m_subframeNo = subframeNo;
		                stats_params.m_rnti = m_rnti;
		                stats_params.m_mcs = grant.m_mcs;
		                stats_params.m_pscchRi = grant.m_resPscch;
		                stats_params.m_pscchTx1 = 1; //NEED to obtain SF of first Tx in PSCCH!!!!!!!!!!!!!!
		                stats_params.m_pscchTx2 = 2; //NEED to obtain SF of second Tx in PSCCH!!!!!!!!!!!!!!
		                stats_params.m_psschTxStartRB = grant.m_rbStart;
		                stats_params.m_psschTxLengthRB = grant.m_rbLen;
		                stats_params.m_psschItrp = grant.m_trp;

		                m_slUeScheduling (stats_params);
					 */
				}
			}

			//if we received a grant, compute the transmission opportunities for PSCCH and PSSCH
			if (poolIt->second.m_grant_received) {
				//make the grant our current grant
				poolIt->second.m_currentGrant = poolIt->second.m_nextGrant;

				NS_LOG_INFO (this << " Sidelink grant received resource " << (uint32_t) poolIt->second.m_currentGrant.m_resPscch);

				SidelinkCommResourcePool::SubframeInfo tmp;
				tmp.frameNo = poolIt->second.m_currentScPeriod.frameNo-1;
				tmp.subframeNo = poolIt->second.m_currentScPeriod.subframeNo-1;

				// Collect statistics for SL UE mac scheduling trace
				NistSlUeMacStatParameters stats_params;
				stats_params.m_frameNo = tmp.frameNo+1;
				stats_params.m_subframeNo = tmp.subframeNo+1;
				stats_params.m_pscchRi = poolIt->second.m_currentGrant.m_resPscch;
				stats_params.m_cellId = 0;
				stats_params.m_imsi = 0 ;
				stats_params.m_pscchFrame1 = 0;
				stats_params.m_pscchSubframe1 = 0;
				stats_params.m_pscchFrame2 = 0;
				stats_params.m_pscchSubframe2 = 0;
				stats_params.m_psschFrame = 0;
				stats_params.m_psschSubframeStart = 0;


				poolIt->second.m_pscchTx = poolIt->second.m_pool->GetPscchTransmissions (poolIt->second.m_currentGrant.m_resPscch);
				uint16_t tx_counter = 1;
				for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->second.m_pscchTx.begin (); txIt != poolIt->second.m_pscchTx.end (); txIt++)
				{
					txIt->subframe = txIt->subframe + tmp;
					//adjust for index starting at 1
					txIt->subframe.frameNo++;
					txIt->subframe.subframeNo++;
					NS_LOG_INFO (this << " PSCCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
					switch (tx_counter){
					case 1:
						stats_params.m_pscchFrame1 = txIt->subframe.frameNo;
						stats_params.m_pscchSubframe1 = txIt->subframe.subframeNo;
						break;
					case 2:
						stats_params.m_pscchFrame2 = txIt->subframe.frameNo;
						stats_params.m_pscchSubframe2 = txIt->subframe.subframeNo;
						break;
					default:
						NS_FATAL_ERROR(this << "PSCCH ONLY SUPPORTS 2 TRANSMISSIONS PER UE GRANT!");
					}
					tx_counter++;

				}

				poolIt->second.m_psschTx = poolIt->second.m_pool->GetPsschTransmissions (tmp, poolIt->second.m_currentGrant.m_trp, poolIt->second.m_currentGrant.m_rbStart, poolIt->second.m_currentGrant.m_rbLen);
				//adjust PSSCH frame to next period
				for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = poolIt->second.m_psschTx.begin (); txIt != poolIt->second.m_psschTx.end (); txIt++)
				{
					//txIt->subframe = txIt->subframe + tmp;
					//adjust for index starting at 1
					txIt->subframe.frameNo++;
					txIt->subframe.subframeNo++;
					NS_LOG_INFO (this << " PSSCH: Subframe " << txIt->subframe.frameNo << "/" << txIt->subframe.subframeNo << ": rbStart=" << (uint32_t) txIt->rbStart << ", rbLen=" << (uint32_t) txIt->nbRb);
				}

				//compute the tb size
				poolIt->second.m_currentGrant.m_tbSize = m_amc->GetUlTbSizeFromMcs (poolIt->second.m_currentGrant.m_mcs, poolIt->second.m_currentGrant.m_rbLen) / 8;
				NS_LOG_INFO ("Sidelink Tb size = " << poolIt->second.m_currentGrant.m_tbSize << " bytes (mcs=" << (uint32_t) poolIt->second.m_currentGrant.m_mcs << ")");

				stats_params.m_rnti = m_rnti;
				stats_params.m_mcs = poolIt->second.m_currentGrant.m_mcs;
				stats_params.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
				stats_params.m_psschTxStartRB = poolIt->second.m_currentGrant.m_rbStart;
				stats_params.m_psschTxLengthRB = poolIt->second.m_currentGrant.m_rbLen;
				stats_params.m_psschItrp = poolIt->second.m_currentGrant.m_trp;
				stats_params.m_timestamp = Simulator::Now ().GetMilliSeconds ();

				// Call trace
				m_slUeScheduling (stats_params);

				//clear the grant
				poolIt->second.m_grant_received = false;
			}
		}

		std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator allocIt;
		//check if we need to transmit PSCCH
		allocIt = poolIt->second.m_pscchTx.begin();
		if (allocIt != poolIt->second.m_pscchTx.end() && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
		{
			//transmission of PSCCH, no need for HARQ
			if (poolIt->second.m_pscchTx.size () == 2) {
				NS_LOG_INFO (this << " First PSCCH transmission");
			} else {
				NS_LOG_INFO (this << " Second PSCCH transmission");
			}
			//create SCI message
			NistSciListElement_s sci;
			sci.m_rnti = m_rnti;
			sci.m_resPscch = poolIt->second.m_currentGrant.m_resPscch;
			sci.m_rbStart = poolIt->second.m_currentGrant.m_rbStart;
			sci.m_rbLen = poolIt->second.m_currentGrant.m_rbLen;
			sci.m_trp = poolIt->second.m_currentGrant.m_trp;
			sci.m_mcs = poolIt->second.m_currentGrant.m_mcs;
			sci.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
			sci.m_groupDstId = (poolIt->first & 0xFF);

			Ptr<SciNistLteControlMessage> msg = Create<SciNistLteControlMessage> ();
			msg->SetSci (sci);
			m_uePhySapProvider->SendNistLteControlMessage (msg);

			poolIt->second.m_pscchTx.erase (allocIt);
		}

		//check if we need to transmit PSSCH
		allocIt = poolIt->second.m_psschTx.begin();
		if (allocIt != poolIt->second.m_psschTx.end() && (*allocIt).subframe.frameNo == frameNo && (*allocIt).subframe.subframeNo == subframeNo)
		{
			// Collect statistics for SL share channel UE mac scheduling trace
			NistSlUeMacStatParameters stats_sch_params;
			stats_sch_params.m_frameNo = poolIt->second.m_currentScPeriod.frameNo;
			stats_sch_params.m_subframeNo = poolIt->second.m_currentScPeriod.subframeNo;
			stats_sch_params.m_psschFrame = frameNo;
			stats_sch_params.m_psschSubframe = subframeNo;
			stats_sch_params.m_cellId = 0;
			stats_sch_params.m_imsi = 0 ;
			stats_sch_params.m_pscchRi = 0 ;
			stats_sch_params.m_pscchFrame1 = 0;
			stats_sch_params.m_pscchSubframe1 = 0;
			stats_sch_params.m_pscchFrame2 = 0;
			stats_sch_params.m_pscchSubframe2 = 0;
			stats_sch_params.m_psschItrp = 0;
			stats_sch_params.m_psschFrameStart = 0;
			stats_sch_params.m_psschSubframeStart = 0;

			//Get first subframe of PSSCH
			SidelinkCommResourcePool::SubframeInfo currScPeriod;
			currScPeriod.frameNo = poolIt->second.m_currentScPeriod.frameNo-1;
			currScPeriod.subframeNo = poolIt->second.m_currentScPeriod.subframeNo-1;

			std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> psschTx = poolIt->second.m_pool->GetPsschTransmissions (currScPeriod, 0, poolIt->second.m_currentGrant.m_rbStart, poolIt->second.m_currentGrant.m_rbLen);
			for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt = psschTx.begin (); txIt != psschTx.end (); txIt++)
			{
				//adjust for index starting at 1
				txIt->subframe.frameNo++;
				txIt->subframe.subframeNo++;
				stats_sch_params.m_psschFrameStart = txIt->subframe.frameNo;
				stats_sch_params.m_psschSubframeStart = txIt->subframe.subframeNo;
				break; //Just need the first one!
			}

			stats_sch_params.m_rnti = m_rnti;
			stats_sch_params.m_mcs = poolIt->second.m_currentGrant.m_mcs;
			stats_sch_params.m_tbSize = poolIt->second.m_currentGrant.m_tbSize;
			stats_sch_params.m_psschTxStartRB = poolIt->second.m_currentGrant.m_rbStart;
			stats_sch_params.m_psschTxLengthRB = poolIt->second.m_currentGrant.m_rbLen;
			stats_sch_params.m_timestamp = Simulator::Now ().GetMilliSeconds ();

			// Call trace
			m_slSharedChUeScheduling (stats_sch_params);


			if (poolIt->second.m_psschTx.size () % 4 == 0)
			{
				NS_LOG_INFO (this << " New PSSCH transmission");
				Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
				poolIt->second.m_miSlHarqProcessPacket = emptyPb;

				//get the BSR for this pool
				//if we have data in the queue
				//find the BSR for that pool (will also give the SidleinkLcIdentifier)
				std::map <SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters>::iterator itBsr;
				for (itBsr = m_slBsrReceived.begin () ;  itBsr != m_slBsrReceived.end () ; itBsr++)
				{
					if (itBsr->first.dstL2Id == poolIt->first)
					{
						//this is the BSR for the pool
						std::map <SidelinkLcIdentifier, NistLcInfo>::iterator it = m_slLcInfoMap.find (itBsr->first);
						//for sidelink we should never have retxQueueSize since it is unacknowledged mode
						//we still keep the process similar to uplink to be more generic (and maybe handle
						//future modifications)
						if ( ((*itBsr).second.statusPduSize > 0)
								|| ((*itBsr).second.retxQueueSize > 0)
								|| ((*itBsr).second.txQueueSize > 0))
						{

							//We have data to send in the PSSCH, notify the RRC to start/continue sending SLSS if appropriate
							m_slHasDataToTx = true;
							m_cmacSapUser->NotifyMacHasSlDataToSend();

							NS_ASSERT ((*itBsr).second.statusPduSize == 0 && (*itBsr).second.retxQueueSize == 0);
							//similar code as uplink transmission
							uint32_t bytesForThisLc = poolIt->second.m_currentGrant.m_tbSize;
							NS_LOG_LOGIC (this << " RNTI " << m_rnti << " Sidelink Tx " << bytesForThisLc << " bytes to LC " << (uint32_t)(*itBsr).first.lcId << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
							if (((*itBsr).second.statusPduSize > 0) && (bytesForThisLc > (*itBsr).second.statusPduSize))
							{
								(*it).second.macSapUser->NotifyTxOpportunity ((*itBsr).second.statusPduSize, 0, 0);
								bytesForThisLc -= (*itBsr).second.statusPduSize; //decrement size available for data
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
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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

									NS_LOG_DEBUG (this << " serve tx DATA, bytes " << bytesForThisLc << ", RLC overhead " << rlcOverhead);
									(*it).second.macSapUser->NotifyTxOpportunity (bytesForThisLc, 0, 0);
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
									if (poolIt->second.m_pool->GetSchedulingType() == SidelinkCommResourcePool::SCHEDULED)
									{
										// resend BSR info for updating eNB peer MAC
										m_freshSlBsr = true;
									}
								}
							}
							NS_LOG_LOGIC (this << " RNTI " << m_rnti << " Sidelink Tx " << bytesForThisLc << "\t new queues " << (uint32_t)(*it).first.lcId << " statusQueue " << (*itBsr).second.statusPduSize << " retxQueue" << (*itBsr).second.retxQueueSize << " txQueue" <<  (*itBsr).second.txQueueSize);
						}
						break;
					}
				}
			}
			else
			{
				NS_LOG_INFO (this << " PSSCH retransmission " << (4 - poolIt->second.m_psschTx.size () % 4));
				Ptr<PacketBurst> pb = poolIt->second.m_miSlHarqProcessPacket;
				for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
				{
					Ptr<Packet> pkt = (*j)->Copy ();
					m_uePhySapProvider->SendMacPdu (pkt);
				}
			}

			poolIt->second.m_psschTx.erase (allocIt);
		}
	}
}

int64_t
NistLteUeMac::AssignStreams (int64_t stream)
{
	NS_LOG_FUNCTION (this << stream);
	m_raPreambleUniformVariable->SetStream (stream);
	return 1;
}

void
NistLteUeMac::DoAddSlDestination (uint32_t destination)
{
	std::list <uint32_t>::iterator it;
	for (it = m_sidelinkDestinations.begin (); it != m_sidelinkDestinations.end ();it++) {
		if ((*it) == destination) {
			break;
		}
	}
	if (it == m_sidelinkDestinations.end ()) {
		//did not find it, so insert
		m_sidelinkDestinations.push_back (destination);
	}
}


void
NistLteUeMac::DoRemoveSlDestination (uint32_t destination)
{
	std::list <uint32_t>::iterator it = m_sidelinkDestinations.begin ();
	while (it != m_sidelinkDestinations.end ()) {
		if ((*it) == destination) {
			m_sidelinkDestinations.erase (it);
			break;//leave the loop
		}
		it++;
	}
}

void
NistLteUeMac::DoNotifyChangeOfTiming(uint32_t frameNo, uint32_t subframeNo)
{
	NS_LOG_FUNCTION (this);

	//there is a delay between the MAC scheduling and the transmission so we assume that we are ahead
	subframeNo += 4;
	if (subframeNo > 10)
	{
		++frameNo;
		if (frameNo > 1024)
			frameNo = 1;
		subframeNo -= 10;
	}

	std::map <uint32_t, PoolInfo>::iterator poolIt;
	for (poolIt = m_sidelinkTxPoolsMap.begin() ; poolIt != m_sidelinkTxPoolsMap.end() ; poolIt++)
	{
		poolIt->second.m_currentScPeriod = poolIt->second.m_pool->GetCurrentScPeriod (frameNo, subframeNo);
		poolIt->second.m_nextScPeriod = poolIt->second.m_pool->GetNextScPeriod (poolIt->second.m_currentScPeriod.frameNo, poolIt->second.m_currentScPeriod.subframeNo);
		//adjust because scheduler starts with frame/subframe = 1
		poolIt->second.m_nextScPeriod.frameNo++;
		poolIt->second.m_nextScPeriod.subframeNo++;
		NS_LOG_INFO (this << " Adapting the period for pool of group " << poolIt->first << ". Next period at " << poolIt->second.m_nextScPeriod.frameNo << "/" << poolIt->second.m_nextScPeriod.subframeNo);
	}
}

std::list< Ptr<SidelinkRxDiscResourcePool> > 
NistLteUeMac::GetDiscRxPools ()
{
  NS_LOG_FUNCTION (this);
  return m_discRxPools;
}

Ptr<SidelinkTxDiscResourcePool>
NistLteUeMac::GetDiscTxPool ()
{
  NS_LOG_FUNCTION (this);
  return m_discTxPools.m_pool; 
}

} // namespace ns3
