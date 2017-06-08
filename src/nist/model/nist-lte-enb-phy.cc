/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 *         Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST
 */

#include <ns3/object-factory.h>
#include <ns3/log.h>
#include <cfloat>
#include <cmath>
#include <ns3/simulator.h>
#include <ns3/attribute-accessor-helper.h>
#include <ns3/double.h>


#include "nist-lte-enb-phy.h"
#include "nist-lte-ue-phy.h"
#include "nist-lte-net-device.h"
#include "nist-lte-spectrum-value-helper.h"
#include "nist-lte-control-messages.h"
#include "nist-lte-enb-net-device.h"
#include "nist-lte-ue-rrc.h"
#include "nist-lte-enb-mac.h"
#include <ns3/nist-lte-common.h>
#include <ns3/nist-lte-vendor-specific-parameters.h>

// WILD HACK for the inizialization of direct eNB-UE ctrl messaging
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nist-lte-ue-net-device.h>
#include <ns3/pointer.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteEnbPhy");

NS_OBJECT_ENSURE_REGISTERED (NistLteEnbPhy);

/**
 * Duration of the data portion of a DL subframe.
 * Equals to "TTI length * (11/14) - margin".
 * Data portion is fixed to 11 symbols out of the available 14 symbols.
 * 1 nanosecond margin is added to avoid overlapping simulator events.
 */
static const Time DL_DATA_DURATION = NanoSeconds (785714 -1);

/**
 * Delay from the start of a DL subframe to transmission of the data portion.
 * Equals to "TTI length * (3/14)".
 * Control portion is fixed to 3 symbols out of the available 14 symbols.
 */
static const Time DL_CTRL_DELAY_FROM_SUBFRAME_START = NanoSeconds (214286);

////////////////////////////////////////
// member SAP forwarders
////////////////////////////////////////

/// \todo SetBandwidth() and SetCellId() can be removed.
class NistEnbMemberLteEnbPhySapProvider : public NistLteEnbPhySapProvider
{
public:
  NistEnbMemberLteEnbPhySapProvider (NistLteEnbPhy* phy);

  // inherited from NistLteEnbPhySapProvider
  virtual void SendMacPdu (Ptr<Packet> p);
  virtual void SetBandwidth (uint8_t ulBandwidth, uint8_t dlBandwidth);
  virtual void SetCellId (uint16_t cellId);
  virtual void SendNistLteControlMessage (Ptr<NistLteControlMessage> msg);
  virtual uint8_t GetMacChTtiDelay ();


private:
  NistLteEnbPhy* m_phy;
};

NistEnbMemberLteEnbPhySapProvider::NistEnbMemberLteEnbPhySapProvider (NistLteEnbPhy* phy) : m_phy (phy)
{

}

void
NistEnbMemberLteEnbPhySapProvider::SendMacPdu (Ptr<Packet> p)
{
  m_phy->DoSendMacPdu (p);
}

void
NistEnbMemberLteEnbPhySapProvider::SetBandwidth (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  m_phy->DoSetBandwidth (ulBandwidth, dlBandwidth);
}

void
NistEnbMemberLteEnbPhySapProvider::SetCellId (uint16_t cellId)
{
  m_phy->DoSetCellId (cellId);
}

void
NistEnbMemberLteEnbPhySapProvider::SendNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
  m_phy->DoSendNistLteControlMessage (msg);
}

uint8_t
NistEnbMemberLteEnbPhySapProvider::GetMacChTtiDelay ()
{
  return (m_phy->DoGetMacChTtiDelay ());
}


////////////////////////////////////////
// generic NistLteEnbPhy methods
////////////////////////////////////////



NistLteEnbPhy::NistLteEnbPhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

NistLteEnbPhy::NistLteEnbPhy (Ptr<NistLteSpectrumPhy> dlPhy, Ptr<NistLteSpectrumPhy> ulPhy, bool EnbEnablePhyLayer)
  : NistLtePhy (dlPhy, ulPhy),
    m_enbPhySapUser (0),
    m_enbCphySapUser (0),
    m_nrFrames (0),
    m_nrSubFrames (0),
    m_srsPeriodicity (0),
    m_srsStartTime (Seconds (0)),
    m_currentSrsOffset (0),
    m_interferenceSampleCounter (0)
{
  m_enbPhySapProvider = new NistEnbMemberLteEnbPhySapProvider (this);
  m_enbCphySapProvider = new MemberNistLteEnbCphySapProvider<NistLteEnbPhy> (this);
  m_harqPhyModule = Create <NistLteHarqPhy> ();
  m_downlinkSpectrumPhy->SetHarqPhyModule (m_harqPhyModule);
  m_uplinkSpectrumPhy->SetHarqPhyModule (m_harqPhyModule);

  if (EnbEnablePhyLayer)
  {
    Simulator::ScheduleNow (&NistLteEnbPhy::StartFrame, this);
  }
  else
  {
    NS_LOG_INFO ("Created eNB disabled!");
  }
}

TypeId
NistLteEnbPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteEnbPhy")
    .SetParent<NistLtePhy> ()
    .AddConstructor<NistLteEnbPhy> ()
    .AddAttribute ("TxPower",
                   "Transmission power in dBm",
                   DoubleValue (30.0),
                   MakeDoubleAccessor (&NistLteEnbPhy::SetTxPower, 
                                       &NistLteEnbPhy::GetTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NoiseFigure",
                   "Loss (dB) in the Signal-to-Noise-Ratio due to "
                   "non-idealities in the receiver.  According to Wikipedia "
                   "(http://en.wikipedia.org/wiki/Noise_figure), this is "
                   "\"the difference in decibels (dB) between"
                   " the noise output of the actual receiver to "
                   "the noise output of an ideal receiver with "
                   "the same overall gain and bandwidth when the receivers "
                   "are connected to sources at the standard noise "
                   "temperature T0.\"  In this model, we consider T0 = 290K.",
                   DoubleValue (5.0),
                   MakeDoubleAccessor (&NistLteEnbPhy::SetNoiseFigure, 
                                       &NistLteEnbPhy::GetNoiseFigure),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MacToChannelDelay",
                   "The delay in TTI units that occurs between "
                   "a scheduling decision in the MAC and the actual "
                   "start of the transmission by the PHY. This is "
                   "intended to be used to model the latency of real PHY "
                   "and MAC implementations.",
                   UintegerValue (2),
                   MakeUintegerAccessor (&NistLteEnbPhy::SetMacChDelay, 
                                         &NistLteEnbPhy::GetMacChDelay),
                   MakeUintegerChecker<uint8_t> ())
    .AddTraceSource ("ReportUeSinr",
                     "Report UEs' averaged linear SINR",
                     MakeTraceSourceAccessor (&NistLteEnbPhy::m_reportUeSinr),
                     "ns3::NistLteEnbPhy::ReportUeSinrTracedCallback")
    .AddAttribute ("UeSinrSamplePeriod",
                   "The sampling period for reporting UEs' SINR stats.",
                   UintegerValue (1),  /// \todo In what unit is this?
                   MakeUintegerAccessor (&NistLteEnbPhy::m_srsSamplePeriod),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("ReportInterference",
                     "Report linear interference power per PHY RB",
                     MakeTraceSourceAccessor (&NistLteEnbPhy::m_reportInterferenceTrace),
                     "ns3::NistLteEnbPhy::ReportInterferenceTracedCallback")
    .AddAttribute ("InterferenceSamplePeriod",
                   "The sampling period for reporting interference stats",
                   UintegerValue (1),  /// \todo In what unit is this?
                   MakeUintegerAccessor (&NistLteEnbPhy::m_interferenceSamplePeriod),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("DlPhyTransmission",
                     "DL transmission PHY layer statistics.",
                     MakeTraceSourceAccessor (&NistLteEnbPhy::m_dlPhyTransmission),
                     "ns3::NistPhyTransmissionStatParameters::TracedCallback")
    .AddAttribute ("DlSpectrumPhy",
                   "The downlink NistLteSpectrumPhy associated to this NistLtePhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbPhy::GetDlSpectrumPhy),
                   MakePointerChecker <NistLteSpectrumPhy> ())
    .AddAttribute ("UlSpectrumPhy",
                   "The uplink NistLteSpectrumPhy associated to this NistLtePhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbPhy::GetUlSpectrumPhy),
                   MakePointerChecker <NistLteSpectrumPhy> ())
  ;
  return tid;
}


NistLteEnbPhy::~NistLteEnbPhy ()
{
}

void
NistLteEnbPhy::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_ueAttached.clear ();
  m_srsUeOffset.clear ();
  delete m_enbPhySapProvider;
  delete m_enbCphySapProvider;
  NistLtePhy::DoDispose ();
}

void
NistLteEnbPhy::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  Ptr<SpectrumValue> noisePsd = NistLteSpectrumValueHelper::CreateNoisePowerSpectralDensity (m_ulEarfcn, m_ulBandwidth, m_noiseFigure);
  m_uplinkSpectrumPhy->SetNoisePowerSpectralDensity (noisePsd);
  NistLtePhy::DoInitialize ();
}


void
NistLteEnbPhy::SetNistLteEnbPhySapUser (NistLteEnbPhySapUser* s)
{
  m_enbPhySapUser = s;
}

NistLteEnbPhySapProvider*
NistLteEnbPhy::GetNistLteEnbPhySapProvider ()
{
  return (m_enbPhySapProvider);
}

void
NistLteEnbPhy::SetNistLteEnbCphySapUser (NistLteEnbCphySapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_enbCphySapUser = s;
}

NistLteEnbCphySapProvider*
NistLteEnbPhy::GetNistLteEnbCphySapProvider ()
{
  NS_LOG_FUNCTION (this);
  return (m_enbCphySapProvider);
}

void
NistLteEnbPhy::SetTxPower (double pow)
{
  NS_LOG_FUNCTION (this << pow);
  m_txPower = pow;
}

double
NistLteEnbPhy::GetTxPower () const
{
  NS_LOG_FUNCTION (this);
  return m_txPower;
}

int8_t
NistLteEnbPhy::DoGetReferenceSignalPower () const
{
  NS_LOG_FUNCTION (this);
  return m_txPower;
}

void
NistLteEnbPhy::SetNoiseFigure (double nf)
{
  NS_LOG_FUNCTION (this << nf);
  m_noiseFigure = nf;
}

double
NistLteEnbPhy::GetNoiseFigure () const
{
  NS_LOG_FUNCTION (this);
  return m_noiseFigure;
}

void
NistLteEnbPhy::SetMacChDelay (uint8_t delay)
{
  NS_LOG_FUNCTION (this);
  m_macChTtiDelay = delay;
  for (int i = 0; i < m_macChTtiDelay; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      m_packetBurstQueue.push_back (pb);
      std::list<Ptr<NistLteControlMessage> > l;
      m_controlMessagesQueue.push_back (l);
      std::list<NistUlDciLteControlMessage> l1;
      m_ulDciQueue.push_back (l1);
    }
  for (int i = 0; i < UL_PUSCH_TTIS_DELAY; i++)
    {
      std::list<NistUlDciLteControlMessage> l1;
      m_ulDciQueue.push_back (l1);
    }
}

uint8_t
NistLteEnbPhy::GetMacChDelay (void) const
{
  return (m_macChTtiDelay);
}

Ptr<NistLteSpectrumPhy>
NistLteEnbPhy::GetDlSpectrumPhy () const
{
  return m_downlinkSpectrumPhy;
}

Ptr<NistLteSpectrumPhy>
NistLteEnbPhy::GetUlSpectrumPhy () const
{
  return m_uplinkSpectrumPhy;
}

bool
NistLteEnbPhy::AddUePhy (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  std::set <uint16_t>::iterator it;
  it = m_ueAttached.find (rnti);
  if (it == m_ueAttached.end ())
    {
      m_ueAttached.insert (rnti);
      return (true);
    }
  else
    {
      NS_LOG_ERROR ("UE already attached");
      return (false);
    }
}

bool
NistLteEnbPhy::DeleteUePhy (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  std::set <uint16_t>::iterator it;
  it = m_ueAttached.find (rnti);
  if (it == m_ueAttached.end ())
    {
      NS_LOG_ERROR ("UE not attached");
      return (false);
    }
  else
    {
      m_ueAttached.erase (it);
      return (true);
    }
}



void
NistLteEnbPhy::DoSendMacPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  SetMacPdu (p);
}

uint8_t
NistLteEnbPhy::DoGetMacChTtiDelay ()
{
  return (m_macChTtiDelay);
}


void
NistLteEnbPhy::PhyPduReceived (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  m_enbPhySapUser->ReceivePhyPdu (p);
}

void
NistLteEnbPhy::SetDownlinkSubChannels (std::vector<int> mask)
{
  NS_LOG_FUNCTION (this);
  m_listOfDownlinkSubchannel = mask;
  Ptr<SpectrumValue> txPsd = CreateTxPowerSpectralDensity ();
  m_downlinkSpectrumPhy->SetTxPowerSpectralDensity (txPsd);
}

void
NistLteEnbPhy::SetDownlinkSubChannelsWithPowerAllocation (std::vector<int> mask)
{
  NS_LOG_FUNCTION (this);
  m_listOfDownlinkSubchannel = mask;
  Ptr<SpectrumValue> txPsd = CreateTxPowerSpectralDensityWithPowerAllocation ();
  m_downlinkSpectrumPhy->SetTxPowerSpectralDensity (txPsd);
}

std::vector<int>
NistLteEnbPhy::GetDownlinkSubChannels (void)
{
  NS_LOG_FUNCTION (this);
  return m_listOfDownlinkSubchannel;
}

void
NistLteEnbPhy::GeneratePowerAllocationMap (uint16_t rnti, int rbId)
{
  NS_LOG_FUNCTION (this);
  double rbgTxPower = m_txPower;

  std::map<uint16_t, double>::iterator it = m_paMap.find (rnti);
  if (it != m_paMap.end ())
    {
      rbgTxPower = m_txPower + it->second;
    }

  m_dlPowerAllocationMap.insert (std::pair<int, double> (rbId, rbgTxPower));
}

Ptr<SpectrumValue>
NistLteEnbPhy::CreateTxPowerSpectralDensity ()
{
  NS_LOG_FUNCTION (this);

  Ptr<SpectrumValue> psd = NistLteSpectrumValueHelper::CreateTxPowerSpectralDensity (m_dlEarfcn, m_dlBandwidth, m_txPower, GetDownlinkSubChannels ());

  return psd;
}

Ptr<SpectrumValue>
NistLteEnbPhy::CreateTxPowerSpectralDensityWithPowerAllocation ()
{
  NS_LOG_FUNCTION (this);

  Ptr<SpectrumValue> psd = NistLteSpectrumValueHelper::CreateTxPowerSpectralDensity (m_dlEarfcn, m_dlBandwidth, m_txPower, m_dlPowerAllocationMap, GetDownlinkSubChannels ());

  return psd;
}


void
NistLteEnbPhy::CalcChannelQualityForUe (std::vector <double> sinr, Ptr<NistLteSpectrumPhy> ue)
{
  NS_LOG_FUNCTION (this);
}


void
NistLteEnbPhy::DoSendNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);
  // queues the message (wait for MAC-PHY delay)
  SetControlMessages (msg);
}



void
NistLteEnbPhy::ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg)
{
  NS_FATAL_ERROR ("Obsolete function");
  NS_LOG_FUNCTION (this << msg);
  m_enbPhySapUser->ReceiveNistLteControlMessage (msg);
}

void
NistLteEnbPhy::ReceiveNistLteControlMessageList (std::list<Ptr<NistLteControlMessage> > msgList)
{
  NS_LOG_FUNCTION (this);
  std::list<Ptr<NistLteControlMessage> >::iterator it;
  for (it = msgList.begin (); it != msgList.end (); it++)
    {
      switch ((*it)->GetMessageType ())
        {
        case NistLteControlMessage::RACH_PREAMBLE:
          {
            Ptr<NistRachPreambleLteControlMessage> rachPreamble = DynamicCast<NistRachPreambleLteControlMessage> (*it);
            m_enbPhySapUser->ReceiveRachPreamble (rachPreamble->GetRapId ());
          }
          break;
        case NistLteControlMessage::DL_CQI:
          {
            Ptr<NistDlCqiLteControlMessage> dlcqiMsg = DynamicCast<NistDlCqiLteControlMessage> (*it);
            NistCqiListElement_s dlcqi = dlcqiMsg->GetDlCqi ();
            // check whether the UE is connected
            if (m_ueAttached.find (dlcqi.m_rnti) != m_ueAttached.end ())
              {
                m_enbPhySapUser->ReceiveNistLteControlMessage (*it);
              }
          }
          break;
        case NistLteControlMessage::BSR:
          {
            Ptr<NistBsrLteControlMessage> bsrMsg = DynamicCast<NistBsrLteControlMessage> (*it);
            NistMacCeListElement_s bsr = bsrMsg->GetBsr ();
            // check whether the UE is connected
            if (m_ueAttached.find (bsr.m_rnti) != m_ueAttached.end ())
              {
                m_enbPhySapUser->ReceiveNistLteControlMessage (*it);
              }
          }
          break;
        case NistLteControlMessage::DL_HARQ:
          {
            Ptr<NistDlHarqFeedbackLteControlMessage> dlharqMsg = DynamicCast<NistDlHarqFeedbackLteControlMessage> (*it);
            NistDlInfoListElement_s dlharq = dlharqMsg->GetDlHarqFeedback ();
            // check whether the UE is connected
            if (m_ueAttached.find (dlharq.m_rnti) != m_ueAttached.end ())
              {
                m_enbPhySapUser->ReceiveNistLteControlMessage (*it);
              }
          }
          break;
        case NistLteControlMessage::SCI:
          NS_LOG_INFO ("eNodeB receiving SL_DCI...ignore");
          break;
        case NistLteControlMessage::MIB_SL:
          NS_LOG_INFO ("eNodeB receiving MIB_SL...ignore");
          break;
        default:
          NS_FATAL_ERROR ("Unexpected NistLteControlMessage type");
          break;
        }
    }
}



void
NistLteEnbPhy::StartFrame (void)
{
  NS_LOG_FUNCTION (this);

  ++m_nrFrames;
  NS_LOG_INFO ("-----frame " << m_nrFrames << "-----");
  m_nrSubFrames = 0;

  // send MIB at beginning of every frame
  m_mib.systemFrameNumber = m_nrSubFrames;
  Ptr<NistMibLteControlMessage> mibMsg = Create<NistMibLteControlMessage> ();
  mibMsg->SetMib (m_mib);
  m_controlMessagesQueue.at (0).push_back (mibMsg);

  StartSubFrame ();
}


void
NistLteEnbPhy::StartSubFrame (void)
{
  NS_LOG_FUNCTION (this);

  ++m_nrSubFrames;

  /*
   * Send SIB1 at 6th subframe of every odd-numbered radio frame. This is
   * equivalent with Section 5.2.1.2 of 3GPP TS 36.331, where it is specified
   * "repetitions are scheduled in subframe #5 of all other radio frames for
   * which SFN mod 2 = 0," except that 3GPP counts frames and subframes starting
   * from 0, while ns-3 counts starting from 1.
   */
  if ((m_nrSubFrames == 6) && ((m_nrFrames % 2) == 1))
    {
      Ptr<NistSib1LteControlMessage> msg = Create<NistSib1LteControlMessage> ();
      msg->SetSib1 (m_sib1);
      m_controlMessagesQueue.at (0).push_back (msg);
    }

  if (m_srsPeriodicity>0)
    { 
      // might be 0 in case the eNB has no UEs attached
      NS_ASSERT_MSG (m_nrFrames > 1, "the SRS index check code assumes that frameNo starts at 1");
      NS_ASSERT_MSG (m_nrSubFrames > 0 && m_nrSubFrames <= 10, "the SRS index check code assumes that subframeNo starts at 1");
      m_currentSrsOffset = (((m_nrFrames-1)*10 + (m_nrSubFrames-1)) % m_srsPeriodicity);
    }
  NS_LOG_INFO ("-----sub frame " << m_nrSubFrames << "-----");
  m_harqPhyModule->SubframeIndication (m_nrFrames, m_nrSubFrames);

  // update info on TB to be received
  std::list<NistUlDciLteControlMessage> uldcilist = DequeueUlDci ();
  std::list<NistUlDciLteControlMessage>::iterator dciIt = uldcilist.begin ();
  NS_LOG_DEBUG (this << " eNB Expected TBs " << uldcilist.size ());
  for (dciIt = uldcilist.begin (); dciIt!=uldcilist.end (); dciIt++)
    {
      std::set <uint16_t>::iterator it2;
      it2 = m_ueAttached.find ((*dciIt).GetDci ().m_rnti);

      if (it2 == m_ueAttached.end ())
        {
          NS_LOG_ERROR ("UE not attached");
        }
      else
        {
          // send info of TB to NistLteSpectrumPhy 
          // translate to allocation map
          std::vector <int> rbMap;
          for (int i = (*dciIt).GetDci ().m_rbStart; i < (*dciIt).GetDci ().m_rbStart + (*dciIt).GetDci ().m_rbLen; i++)
            {
              rbMap.push_back (i);
            }
          m_uplinkSpectrumPhy->AddExpectedTb ((*dciIt).GetDci ().m_rnti, (*dciIt).GetDci ().m_ndi, (*dciIt).GetDci ().m_tbSize, (*dciIt).GetDci ().m_mcs, rbMap, 0 /* always SISO*/, 0 /* no HARQ proc id in UL*/, 0 /*evaluated by NistLteSpectrumPhy*/, false /* UL*/);
          if ((*dciIt).GetDci ().m_ndi==1)
            {
              NS_LOG_DEBUG (this << " RNTI " << (*dciIt).GetDci ().m_rnti << " NEW TB");
            }
          else
            {
              NS_LOG_DEBUG (this << " RNTI " << (*dciIt).GetDci ().m_rnti << " HARQ RETX");
            }
        }
    }

  // process the current burst of control messages
  std::list<Ptr<NistLteControlMessage> > ctrlMsg = GetControlMessages ();
  m_dlDataRbMap.clear ();
  m_dlPowerAllocationMap.clear ();
  if (ctrlMsg.size () > 0)
    {
      std::list<Ptr<NistLteControlMessage> >::iterator it;
      it = ctrlMsg.begin ();
      while (it != ctrlMsg.end ())
        {
          Ptr<NistLteControlMessage> msg = (*it);
          if (msg->GetMessageType () == NistLteControlMessage::DL_DCI)
            {
              Ptr<NistDlDciLteControlMessage> dci = DynamicCast<NistDlDciLteControlMessage> (msg);
              // get the tx power spectral density according to DL-DCI(s)
              // translate the DCI to Spectrum framework
              uint32_t mask = 0x1;
              for (int i = 0; i < 32; i++)
                {
                  if (((dci->GetDci ().m_rbBitmap & mask) >> i) == 1)
                    {
                      for (int k = 0; k < GetRbgSize (); k++)
                        {
                          m_dlDataRbMap.push_back ((i * GetRbgSize ()) + k);
                          //NS_LOG_DEBUG(this << " [enb]DL-DCI allocated PRB " << (i*GetRbgSize()) + k);
                          GeneratePowerAllocationMap (dci->GetDci ().m_rnti, (i * GetRbgSize ()) + k );
                        }
                    }
                  mask = (mask << 1);
                }
              // fire trace of DL Tx PHY stats
              for (uint8_t i = 0; i < dci->GetDci ().m_mcs.size (); i++)
                {
                  NistPhyTransmissionStatParameters params;
                  params.m_cellId = m_cellId;
                  params.m_imsi = 0; // it will be set by DlPhyTransmissionCallback in NistLteHelper
                  params.m_timestamp = Simulator::Now ().GetMilliSeconds ();
                  params.m_rnti = dci->GetDci ().m_rnti;
                  params.m_txMode = 0; // TBD
                  params.m_layer = i;
                  params.m_mcs = dci->GetDci ().m_mcs.at (i);
                  params.m_size = dci->GetDci ().m_tbsSize.at (i);
                  params.m_rv = dci->GetDci ().m_rv.at (i);
                  params.m_ndi = dci->GetDci ().m_ndi.at (i);
                  m_dlPhyTransmission (params);
                }

            }
          else if (msg->GetMessageType () == NistLteControlMessage::UL_DCI)
            {
              Ptr<NistUlDciLteControlMessage> dci = DynamicCast<NistUlDciLteControlMessage> (msg);
              QueueUlDci (*dci);
            }
          else if (msg->GetMessageType () == NistLteControlMessage::RAR)
            {
              Ptr<NistRarLteControlMessage> rarMsg = DynamicCast<NistRarLteControlMessage> (msg);
              for (std::list<NistRarLteControlMessage::NistRar>::const_iterator it = rarMsg->NistRarListBegin (); it != rarMsg->NistRarListEnd (); ++it)
                {
                  if (it->rarPayload.m_grant.m_ulDelay == true)
                    {
                      NS_FATAL_ERROR (" RAR delay is not yet implemented");
                    }
                  NistUlGrant_s ulGrant = it->rarPayload.m_grant;
                  // translate the UL grant in a standard UL-DCI and queue it
                  NistUlDciListElement_s dci;
                  dci.m_rnti = ulGrant.m_rnti;
                  dci.m_rbStart = ulGrant.m_rbStart;
                  dci.m_rbLen = ulGrant.m_rbLen;
                  dci.m_tbSize = ulGrant.m_tbSize;
                  dci.m_mcs = ulGrant.m_mcs;
                  dci.m_hopping = ulGrant.m_hopping;
                  dci.m_tpc = ulGrant.m_tpc;
                  dci.m_cqiRequest = ulGrant.m_cqiRequest;
                  dci.m_ndi = 1;
                  NistUlDciLteControlMessage msg;
                  msg.SetDci (dci);
                  QueueUlDci (msg);
                }
            }
          it++;

        }
    }

  SendControlChannels (ctrlMsg);

  // send data frame
  Ptr<PacketBurst> pb = GetPacketBurst ();
  if (pb)
    {
      Simulator::Schedule (DL_CTRL_DELAY_FROM_SUBFRAME_START, // ctrl frame fixed to 3 symbols
                           &NistLteEnbPhy::SendDataChannels,
                           this,pb);
    }

  // trigger the MAC
  m_enbPhySapUser->SubframeIndication (m_nrFrames, m_nrSubFrames);

  Simulator::Schedule (Seconds (GetTti ()),
                       &NistLteEnbPhy::EndSubFrame,
                       this);

}

void
NistLteEnbPhy::SendControlChannels (std::list<Ptr<NistLteControlMessage> > ctrlMsgList)
{
  NS_LOG_FUNCTION (this << " eNB " << m_cellId << " start tx ctrl frame");
  // set the current tx power spectral density (full bandwidth)
  std::vector <int> dlRb;
  for (uint8_t i = 0; i < m_dlBandwidth; i++)
    {
      dlRb.push_back (i);
    }
  SetDownlinkSubChannels (dlRb);
  NS_LOG_LOGIC (this << " eNB start TX CTRL");
  bool pss = false;
  if ((m_nrSubFrames == 1) || (m_nrSubFrames == 6))
    {
      pss = true;
    }
  m_downlinkSpectrumPhy->StartTxDlCtrlFrame (ctrlMsgList, pss);

}

void
NistLteEnbPhy::SendDataChannels (Ptr<PacketBurst> pb)
{
  // set the current tx power spectral density
  SetDownlinkSubChannelsWithPowerAllocation (m_dlDataRbMap);
  // send the current burts of packets
  NS_LOG_LOGIC (this << " eNB start TX DATA");
  std::list<Ptr<NistLteControlMessage> > ctrlMsgList;
  ctrlMsgList.clear ();
  m_downlinkSpectrumPhy->StartTxDataFrame (pb, ctrlMsgList, DL_DATA_DURATION);
}


void
NistLteEnbPhy::EndSubFrame (void)
{
  NS_LOG_FUNCTION (this << Simulator::Now ().GetSeconds ());
  if (m_nrSubFrames == 10)
    {
      Simulator::ScheduleNow (&NistLteEnbPhy::EndFrame, this);
    }
  else
    {
      Simulator::ScheduleNow (&NistLteEnbPhy::StartSubFrame, this);
    }
}


void
NistLteEnbPhy::EndFrame (void)
{
  NS_LOG_FUNCTION (this << Simulator::Now ().GetSeconds ());
  Simulator::ScheduleNow (&NistLteEnbPhy::StartFrame, this);
}


void 
NistLteEnbPhy::GenerateCtrlCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr << Simulator::Now () << m_srsStartTime);
  // avoid processing SRSs sent with an old SRS configuration index
  if (Simulator::Now () > m_srsStartTime)
    {
      NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi = CreateSrsCqiReport (sinr);
      m_enbPhySapUser->UlCqiReport (ulcqi);
    }
}

void
NistLteEnbPhy::GenerateDataCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr);
  NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi = CreatePuschCqiReport (sinr);
  m_enbPhySapUser->UlCqiReport (ulcqi);
}

void
NistLteEnbPhy::ReportInterference (const SpectrumValue& interf)
{
  NS_LOG_FUNCTION (this << interf);
  Ptr<SpectrumValue> interfCopy = Create<SpectrumValue> (interf);
  m_interferenceSampleCounter++;
  if (m_interferenceSampleCounter == m_interferenceSamplePeriod)
    {
      m_reportInterferenceTrace (m_cellId, interfCopy);
      m_interferenceSampleCounter = 0;
    }
}

void
NistLteEnbPhy::ReportRsReceivedPower (const SpectrumValue& power)
{
  // not used by eNB
}



NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters
NistLteEnbPhy::CreatePuschCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr);
  Values::const_iterator it;
  NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi;
  ulcqi.m_ulCqi.m_type = NistUlCqi_s::PUSCH;
  int i = 0;
  for (it = sinr.ConstValuesBegin (); it != sinr.ConstValuesEnd (); it++)
    {
      double sinrdb = 10 * std::log10 ((*it));
//       NS_LOG_DEBUG ("ULCQI RB " << i << " value " << sinrdb);
      // convert from double to fixed point notation Sxxxxxxxxxxx.xxx
      int16_t sinrFp = NistLteFfConverter::double2fpS11dot3 (sinrdb);
      ulcqi.m_ulCqi.m_sinr.push_back (sinrFp);
      i++;
    }
  return (ulcqi);
	
}


void
NistLteEnbPhy::DoSetBandwidth (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << (uint32_t) ulBandwidth << (uint32_t) dlBandwidth);
  m_ulBandwidth = ulBandwidth;
  m_dlBandwidth = dlBandwidth;

  static const int Type0AllocationRbg[4] = {
    10,     // RGB size 1
    26,     // RGB size 2
    63,     // RGB size 3
    110     // RGB size 4
  };  // see table 7.1.6.1-1 of 36.213
  for (int i = 0; i < 4; i++)
    {
      if (dlBandwidth < Type0AllocationRbg[i])
        {
          m_rbgSize = i + 1;
          break;
        }
    }
}

void 
NistLteEnbPhy::DoSetEarfcn (uint16_t ulEarfcn, uint16_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << ulEarfcn << dlEarfcn);
  m_ulEarfcn = ulEarfcn;
  m_dlEarfcn = dlEarfcn;
}


void 
NistLteEnbPhy::DoAddUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
 
  bool success = AddUePhy (rnti);
  NS_ASSERT_MSG (success, "AddUePhy() failed");

  // add default P_A value
  DoSetPa (rnti, 0);
}

void 
NistLteEnbPhy::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
 
  bool success = DeleteUePhy (rnti);
  NS_ASSERT_MSG (success, "DeleteUePhy() failed");

  // remove also P_A value
  std::map<uint16_t, double>::iterator it = m_paMap.find (rnti);
  if (it != m_paMap.end ())
    {
      m_paMap.erase (it);
    }

}

void
NistLteEnbPhy::DoSetPa (uint16_t rnti, double pa)
{
  NS_LOG_FUNCTION (this << rnti);

  std::map<uint16_t, double>::iterator it = m_paMap.find (rnti);

  if (it == m_paMap.end ())
    {
      m_paMap.insert (std::pair<uint16_t, double> (rnti, pa));
    }
  else
    {
      it->second = pa;
    }

}

NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters
NistLteEnbPhy::CreateSrsCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr);
  Values::const_iterator it;
  NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi;
  ulcqi.m_ulCqi.m_type = NistUlCqi_s::SRS;
  int i = 0;
  double srsSum = 0.0;
  for (it = sinr.ConstValuesBegin (); it != sinr.ConstValuesEnd (); it++)
    {
      double sinrdb = 10 * log10 ((*it));
      //       NS_LOG_DEBUG ("ULCQI RB " << i << " value " << sinrdb);
      // convert from double to fixed point notation Sxxxxxxxxxxx.xxx
      int16_t sinrFp = NistLteFfConverter::double2fpS11dot3 (sinrdb);
      srsSum += (*it);
      ulcqi.m_ulCqi.m_sinr.push_back (sinrFp);
      i++;
    }
  // Insert the user generated the srs as a vendor specific parameter
  NS_LOG_DEBUG (this << " ENB RX UL-CQI of " << m_srsUeOffset.at (m_currentSrsOffset));
  NistVendorSpecificListElement_s vsp;
  vsp.m_type = SRS_CQI_RNTI_VSP;
  vsp.m_length = sizeof(NistSrsCqiRntiVsp);
  Ptr<NistSrsCqiRntiVsp> rnti  = Create <NistSrsCqiRntiVsp> (m_srsUeOffset.at (m_currentSrsOffset));
  vsp.m_value = rnti;
  ulcqi.m_vendorSpecificList.push_back (vsp);
  // call SRS tracing method
  CreateSrsReport (m_srsUeOffset.at (m_currentSrsOffset),
                   (i > 0) ? (srsSum / i) : DBL_MAX);
  return (ulcqi);

}


void
NistLteEnbPhy::CreateSrsReport (uint16_t rnti, double srs)
{
  NS_LOG_FUNCTION (this << rnti << srs);
  std::map <uint16_t,uint16_t>::iterator it = m_srsSampleCounterMap.find (rnti);
  if (it==m_srsSampleCounterMap.end ())
    {
      // create new entry
      m_srsSampleCounterMap.insert (std::pair <uint16_t,uint16_t> (rnti, 0));
      it = m_srsSampleCounterMap.find (rnti);
    }
  (*it).second++;
  if ((*it).second == m_srsSamplePeriod)
    {
      m_reportUeSinr (m_cellId, rnti, srs);
      (*it).second = 0;
    }
}

void
NistLteEnbPhy::DoSetTransmissionMode (uint16_t  rnti, uint8_t txMode)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t)txMode);
  // UL supports only SISO MODE
}

void
NistLteEnbPhy::QueueUlDci (NistUlDciLteControlMessage m)
{
  NS_LOG_FUNCTION (this);
  m_ulDciQueue.at (UL_PUSCH_TTIS_DELAY - 1).push_back (m);
}

std::list<NistUlDciLteControlMessage>
NistLteEnbPhy::DequeueUlDci (void)
{
  NS_LOG_FUNCTION (this);
  if (m_ulDciQueue.at (0).size ()>0)
    {
      std::list<NistUlDciLteControlMessage> ret = m_ulDciQueue.at (0);
      m_ulDciQueue.erase (m_ulDciQueue.begin ());
      std::list<NistUlDciLteControlMessage> l;
      m_ulDciQueue.push_back (l);
      return (ret);
    }
  else
    {
      m_ulDciQueue.erase (m_ulDciQueue.begin ());
      std::list<NistUlDciLteControlMessage> l;
      m_ulDciQueue.push_back (l);
      std::list<NistUlDciLteControlMessage> emptylist;
      return (emptylist);
    }
}

void
NistLteEnbPhy::DoSetSrsConfigurationIndex (uint16_t  rnti, uint16_t srcCi)
{
  NS_LOG_FUNCTION (this);
  uint16_t p = GetSrsPeriodicity (srcCi);
  if (p!=m_srsPeriodicity)
    {
      // resize the array of offset -> re-initialize variables
      m_srsUeOffset.clear ();
      m_srsUeOffset.resize (p, 0);
      m_srsPeriodicity = p;
      // inhibit SRS until RRC Connection Reconfiguration propagates
      // to UEs, otherwise we might be wrong in determining the UE who
      // actually sent the SRS (if the UE was using a stale SRS config)
      // if we use a static SRS configuration index, we can have a 0ms guard time
      m_srsStartTime = Simulator::Now () + MilliSeconds (m_macChTtiDelay) + MilliSeconds (0);
    }

  NS_LOG_DEBUG (this << " ENB SRS P " << m_srsPeriodicity << " RNTI " << rnti << " offset " << GetSrsSubframeOffset (srcCi) << " CI " << srcCi);
  std::map <uint16_t,uint16_t>::iterator it = m_srsCounter.find (rnti);
  if (it != m_srsCounter.end ())
    {
      (*it).second = GetSrsSubframeOffset (srcCi) + 1;
    }
  else
    {
      m_srsCounter.insert (std::pair<uint16_t, uint16_t> (rnti, GetSrsSubframeOffset (srcCi) + 1));
    }
  m_srsUeOffset.at (GetSrsSubframeOffset (srcCi)) = rnti;

}


void 
NistLteEnbPhy::DoSetNistMasterInformationBlock (NistLteRrcSap::NistMasterInformationBlock mib)
{
  NS_LOG_FUNCTION (this);
  m_mib = mib;
}


void
NistLteEnbPhy::DoSetNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 sib1)
{
  NS_LOG_FUNCTION (this);
  m_sib1 = sib1;
}


void
NistLteEnbPhy::SetHarqPhyModule (Ptr<NistLteHarqPhy> harq)
{
  m_harqPhyModule = harq;
}


void
NistLteEnbPhy::ReceiveLteUlHarqFeedback (NistUlInfoListElement_s mes)
{
  NS_LOG_FUNCTION (this);
  // forward to scheduler
  m_enbPhySapUser->UlInfoListElementHarqFeeback (mes);
}

};
