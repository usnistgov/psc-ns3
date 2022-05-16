/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
 * Copyright (c) 2018 Fraunhofer ESK : RLF extensions
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
 *         Marco Miozzo <marco.miozzo@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 * Modified by:
 *          Vignesh Babu <ns3-dev@esk.fraunhofer.de> (RLF extensions)
 *          NIST // Contributions may not be subject to US copyright. (D2D extensions)
 */

#include <ns3/object-factory.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <cfloat>
#include <cmath>
#include <ns3/simulator.h>
#include <ns3/double.h>
#include "lte-ue-phy.h"
#include "lte-enb-phy.h"
#include "lte-net-device.h"
#include "lte-ue-net-device.h"
#include "lte-enb-net-device.h"
#include "lte-spectrum-value-helper.h"
#include "lte-amc.h"
#include "lte-ue-mac.h"
#include "ff-mac-common.h"
#include "lte-chunk-processor.h"
#include "lte-ue-phy-sap.h"
#include "lte-sl-tag.h"
#include "lte-rrc-header.h"
#include <ns3/lte-common.h>
#include <ns3/pointer.h>
#include <ns3/boolean.h>
#include <ns3/lte-ue-power-control.h>
#include <ns3/lte-radio-bearer-tag.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteUePhy");



/**
 * Duration of the data portion of a UL subframe.
 * Equals to "TTI length - 1 symbol length for SRS - margin".
 * The margin is 1 nanosecond and is intended to avoid overlapping simulator
 * events. The duration of one symbol is TTI/14 (rounded). In other words,
 * duration of data portion of UL subframe = 1 ms * (13/14) - 1 ns.
 */
static const Time UL_DATA_DURATION = NanoSeconds (1e6 - 71429 - 1);

/**
 * Delay from subframe start to transmission of SRS.
 * Equals to "TTI length - 1 symbol for SRS".
 */
static const Time UL_SRS_DELAY_FROM_SUBFRAME_START = NanoSeconds (1e6 - 71429);




////////////////////////////////////////
// member SAP forwarders
////////////////////////////////////////

/// UeMemberLteUePhySapProvider class
class UeMemberLteUePhySapProvider : public LteUePhySapProvider
{
public:
  /**
   * Constructor
   *
   * \param phy the LTE UE Phy
   */
  UeMemberLteUePhySapProvider (LteUePhy* phy);

  // inherited from LtePhySapProvider
  virtual void SendMacPdu (Ptr<Packet> p);
  virtual void SendSlMacPdu (Ptr<Packet> p, TransmitSlPhySduParameters params);
  virtual void SendLteControlMessage (Ptr<LteControlMessage> msg);
  virtual void SendRachPreamble (uint32_t prachId, uint32_t raRnti);
  virtual void NotifyConnectionSuccessful ();

private:
  LteUePhy* m_phy; ///< the Phy
};

UeMemberLteUePhySapProvider::UeMemberLteUePhySapProvider (LteUePhy* phy) : m_phy (phy)
{}

void
UeMemberLteUePhySapProvider::SendMacPdu (Ptr<Packet> p)
{
  m_phy->DoSendMacPdu (p);
}

void
UeMemberLteUePhySapProvider::SendSlMacPdu (Ptr<Packet> p, TransmitSlPhySduParameters params)
{
  m_phy->DoSendSlMacPdu (p, params);
}

void
UeMemberLteUePhySapProvider::SendLteControlMessage (Ptr<LteControlMessage> msg)
{
  m_phy->DoSendLteControlMessage (msg);
}

void
UeMemberLteUePhySapProvider::SendRachPreamble (uint32_t prachId, uint32_t raRnti)
{
  m_phy->DoSendRachPreamble (prachId, raRnti);
}

void
UeMemberLteUePhySapProvider::NotifyConnectionSuccessful ()
{
  m_phy->DoNotifyConnectionSuccessful ();
}


////////////////////////////////////////
// LteUePhy methods
////////////////////////////////////////

/// Map each of UE PHY states to its string representation.
static const std::string g_uePhyStateName[LteUePhy::NUM_STATES] =
{
  "CELL_SEARCH",
  "SYNCHRONIZED"
};

/**
 * \param s The UE PHY state.
 * \return The string representation of the given state.
 */
static inline const std::string & ToString (LteUePhy::State s)
{
  return g_uePhyStateName[s];
}


NS_OBJECT_ENSURE_REGISTERED (LteUePhy);


LteUePhy::LteUePhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

LteUePhy::LteUePhy (Ptr<LteSpectrumPhy> dlPhy, Ptr<LteSpectrumPhy> ulPhy)
  : LtePhy (dlPhy, ulPhy),
  m_uePhySapUser (0),
  m_ueCphySapUser (0),
  m_state (CELL_SEARCH),
  m_subframeNo (0),
  m_rsReceivedPowerUpdated (false),
  m_rsInterferencePowerUpdated (false),
  m_dataInterferencePowerUpdated (false),
  m_pssReceived (false),
  m_ueMeasurementsFilterPeriod (MilliSeconds (200)),
  m_ueMeasurementsFilterLast (MilliSeconds (0)),
  m_rsrpSinrSampleCounter (0),
  m_imsi (0),
  m_tFirstScanning (MilliSeconds (0)),
  m_ueSlssScanningInProgress (false),
  m_ueSlssMeasurementInProgress (false),
  m_currNMeasPeriods (0),
  m_currFrameNo (0),
  m_currSubframeNo (0),
  m_resyncRequested (false),
  m_waitingNextScPeriod (false),
  m_ueSdRsrpMeasurementsEnabled (false)
{
  m_amc = CreateObject <LteAmc> ();
  m_powerControl = CreateObject <LteUePowerControl> ();
  m_uePhySapProvider = new UeMemberLteUePhySapProvider (this);
  m_ueCphySapProvider = new MemberLteUeCphySapProvider<LteUePhy> (this);
  m_macChTtiDelay = UL_PUSCH_TTIS_DELAY;

  m_nextScanRdm = CreateObject<UniformRandomVariable> ();
  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();

  NS_ASSERT_MSG (Simulator::Now ().GetNanoSeconds () == 0,
                 "Cannot create UE devices after simulation started");
  Simulator::Schedule (m_ueMeasurementsFilterPeriod, &LteUePhy::ReportUeMeasurements, this);

  m_slTxPoolInfo.m_pool = nullptr;
  m_slTxPoolInfo.m_currentScPeriod.frameNo = 0;
  m_slTxPoolInfo.m_currentScPeriod.subframeNo = 0;
  m_slTxPoolInfo.m_nextScPeriod.frameNo = 0;
  m_slTxPoolInfo.m_nextScPeriod.subframeNo = 0;

  DoReset ();
}


LteUePhy::~LteUePhy ()
{
  m_txModeGain.clear ();
}

void
LteUePhy::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_packetParamsQueue.clear ();
  delete m_uePhySapProvider;
  delete m_ueCphySapProvider;
  if (m_sidelinkSpectrumPhy)
    {
      m_sidelinkSpectrumPhy->Dispose ();
      m_sidelinkSpectrumPhy = 0;
    }
  LtePhy::DoDispose ();
}



TypeId
LteUePhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteUePhy")
    .SetParent<LtePhy> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteUePhy> ()
    .AddAttribute ("TxPower",
                   "Transmission power in dBm",
                   DoubleValue (10.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxPower,
                                       &LteUePhy::GetTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NoiseFigure",
                   "Loss (dB) in the Signal-to-Noise-Ratio due to non-idealities in the receiver."
                   " According to Wikipedia (http://en.wikipedia.org/wiki/Noise_figure), this is "
                   "\"the difference in decibels (dB) between"
                   " the noise output of the actual receiver to the noise output of an "
                   " ideal receiver with the same overall gain and bandwidth when the receivers "
                   " are connected to sources at the standard noise temperature T0.\" "
                   "In this model, we consider T0 = 290K.",
                   DoubleValue (9.0),
                   MakeDoubleAccessor (&LteUePhy::SetNoiseFigure,
                                       &LteUePhy::GetNoiseFigure),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode1Gain",
                   "Transmission mode 1 gain in dB",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode1Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode2Gain",
                   "Transmission mode 2 gain in dB",
                   DoubleValue (4.2),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode2Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode3Gain",
                   "Transmission mode 3 gain in dB",
                   DoubleValue (-2.8),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode3Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode4Gain",
                   "Transmission mode 4 gain in dB",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode4Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode5Gain",
                   "Transmission mode 5 gain in dB",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode5Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode6Gain",
                   "Transmission mode 6 gain in dB",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode6Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxMode7Gain",
                   "Transmission mode 7 gain in dB",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&LteUePhy::SetTxMode7Gain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SlRxGain",
                   "Sidelink SIMO gain in dB",
                   DoubleValue (4.0),
                   MakeDoubleAccessor (&LteUePhy::SetSlRxGain),
                   MakeDoubleChecker<double> ())
    .AddTraceSource ("ReportCurrentCellRsrpSinr",
                     "RSRP and SINR statistics.",
                     MakeTraceSourceAccessor (&LteUePhy::m_reportCurrentCellRsrpSinrTrace),
                     "ns3::LteUePhy::RsrpSinrTracedCallback")
    .AddAttribute ("RsrpSinrSamplePeriod",
                   "The sampling period for reporting RSRP-SINR stats (default value 1)",
                   UintegerValue (1),
                   MakeUintegerAccessor (&LteUePhy::m_rsrpSinrSamplePeriod),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("ReportUlPhyResourceBlocks",
                     "UL transmission PHY layer resource blocks.",
                     MakeTraceSourceAccessor (&LteUePhy::m_reportUlPhyResourceBlocks),
                     "ns3::LteUePhy::UlPhyResourceBlocksTracedCallback")
    .AddTraceSource ("ReportPowerSpectralDensity",
                     "Power Spectral Density data.",
                     MakeTraceSourceAccessor (&LteUePhy::m_reportPowerSpectralDensity),
                     "ns3::LteUePhy::PowerSpectralDensityTracedCallback")
    .AddTraceSource ("UlPhyTransmission",
                     "DL transmission PHY layer statistics.",
                     MakeTraceSourceAccessor (&LteUePhy::m_ulPhyTransmission),
                     "ns3::PhyTransmissionStatParameters::TracedCallback")
    .AddAttribute ("DlSpectrumPhy",
                   "The downlink LteSpectrumPhy associated to this LtePhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&LteUePhy::GetDlSpectrumPhy),
                   MakePointerChecker <LteSpectrumPhy> ())
    .AddAttribute ("UlSpectrumPhy",
                   "The uplink LteSpectrumPhy associated to this LtePhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&LteUePhy::GetUlSpectrumPhy),
                   MakePointerChecker <LteSpectrumPhy> ())
    .AddAttribute ("SlSpectrumPhy",
                   "The Sidelink LteSpectrumPhy associated to this LtePhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&LteUePhy::GetSlSpectrumPhy),
                   MakePointerChecker <LteSpectrumPhy> ())
    .AddAttribute ("RsrqUeMeasThreshold",
                   "Receive threshold for PSS on RSRQ [dB]",
                   DoubleValue (-1000.0),
                   MakeDoubleAccessor (&LteUePhy::m_pssReceptionThreshold),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RsrpUeMeasThreshold",
                   "Receive threshold for RSRP [dB]",
                   DoubleValue (-1000.0), //to avoid changing the default behavior, make it low so that it acts as if it was not used
                   MakeDoubleAccessor (&LteUePhy::m_rsrpReceptionThreshold),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("UeMeasurementsFilterPeriod",
                   "Time period for reporting UE measurements, i.e., the"
                   "length of layer-1 filtering.",
                   TimeValue (MilliSeconds (200)),
                   MakeTimeAccessor (&LteUePhy::m_ueMeasurementsFilterPeriod),
                   MakeTimeChecker ())
    .AddAttribute ("DownlinkCqiPeriodicity",
                   "Periodicity in milliseconds for reporting the"
                   "wideband and subband downlink CQIs to the eNB",
                   TimeValue (MilliSeconds (1)),
                   MakeTimeAccessor (&LteUePhy::SetDownlinkCqiPeriodicity),
                   MakeTimeChecker ())
    .AddTraceSource ("ReportUeMeasurements",
                     "Report UE measurements RSRP (dBm) and RSRQ (dB).",
                     MakeTraceSourceAccessor (&LteUePhy::m_reportUeMeasurements),
                     "ns3::LteUePhy::RsrpRsrqTracedCallback")
    .AddTraceSource ("StateTransition",
                     "Trace fired upon every UE PHY state transition",
                     MakeTraceSourceAccessor (&LteUePhy::m_stateTransitionTrace),
                     "ns3::LteUePhy::StateTracedCallback")
    .AddAttribute ("EnableUplinkPowerControl",
                   "If true, Uplink Power Control will be enabled.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LteUePhy::m_enableUplinkPowerControl),
                   MakeBooleanChecker ())
    .AddAttribute ("Qout",
                   "corresponds to 10% block error rate of a hypothetical PDCCH transmission"
                   "taking into account the PCFICH errors with transmission parameters."
                   "see 3GPP TS 36.213 4.2.1 and TS 36.133 7.6",
                   DoubleValue (-5),
                   MakeDoubleAccessor (&LteUePhy::m_qOut),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Qin",
                   "corresponds to 2% block error rate of a hypothetical PDCCH transmission"
                   "taking into account the PCFICH errors with transmission parameters."
                   "see 3GPP TS 36.213 4.2.1 and TS 36.133 7.6",
                   DoubleValue (-3.9),
                   MakeDoubleAccessor (&LteUePhy::m_qIn),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NumQoutEvalSf",
                   "This specifies the total number of consecutive subframes"
                   "which corresponds to the Qout evaluation period",
                   UintegerValue (200), //see 3GPP 3GPP TS 36.133 7.6.2.1
                   MakeUintegerAccessor (&LteUePhy::SetNumQoutEvalSf,
                                         &LteUePhy::GetNumQoutEvalSf),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("NumQinEvalSf",
                   "This specifies the total number of consecutive subframes"
                   "which corresponds to the Qin evaluation period",
                   UintegerValue (100), //see 3GPP 3GPP TS 36.133 7.6.2.1
                   MakeUintegerAccessor (&LteUePhy::SetNumQinEvalSf,
                                         &LteUePhy::GetNumQinEvalSf),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("EnableRlfDetection",
                   "If true, RLF detection will be enabled.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LteUePhy::m_enableRlfDetection),
                   MakeBooleanChecker ())
    .AddAttribute ("UeSlssInterScanningPeriodMax",
                   "The upper bound of the uniform random variable for the interval between SyncRef selection processes",
                   TimeValue (MilliSeconds (2000)),
                   MakeTimeAccessor (&LteUePhy::SetUeSlssInterScanningPeriodMax),
                   MakeTimeChecker ())
    .AddAttribute ("UeSlssInterScanningPeriodMin",
                   "The lower bound of the uniform random variable for the interval between SyncRef selection processes",
                   TimeValue (MilliSeconds (2000)),
                   MakeTimeAccessor (&LteUePhy::SetUeSlssInterScanningPeriodMin),
                   MakeTimeChecker ())
    .AddAttribute ("UeSlssScanningPeriod",
                   "How long the UE will search for SyncRefs (scanning)",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&LteUePhy::m_ueSlssScanningPeriod),
                   MakeTimeChecker ())
    .AddAttribute ("UeSlssMeasurementPeriod",
                   "How long the UE will perform SLSS L1 measurements for SyncRef selection (measurement)",
                   TimeValue (MilliSeconds (400)),
                   MakeTimeAccessor (&LteUePhy::m_ueSlssMeasurementPeriod),
                   MakeTimeChecker ())
    .AddAttribute ("UeSlssEvaluationPeriod",
                   "How long the UE will perform SLSS L1 measurements to determine cease/initiation of SLSS transmission (evaluation)",
                   TimeValue (MilliSeconds (800)),
                   MakeTimeAccessor (&LteUePhy::m_ueSlssEvaluationPeriod),
                   MakeTimeChecker ())
    .AddAttribute ("NSamplesSrsrpMeas",
                   "The maximum number of samples to take during SLSS L1 measurements for each SyncRef",
                   UintegerValue (4),
                   MakeUintegerAccessor (&LteUePhy::m_nSamplesSrsrpMeas),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UeRandomInitialSubframeIndication",
                   "If True, the first frame and subframe values (beginning of the simulation) are chosen randomly, if False they are fixed to 1,1 respectively",
                   BooleanValue (false),
                   MakeBooleanAccessor (&LteUePhy::m_chooseFrameAndSubframeRandomly),
                   MakeBooleanChecker ())
    .AddAttribute ("MinSrsrp",
                   "The minimum S-RSRP required to consider a SyncRef detectable",
                   DoubleValue (-125),
                   MakeDoubleAccessor (&LteUePhy::m_minSrsrp),
                   MakeDoubleChecker<double> ())
    .AddTraceSource ("ReportUeSdRsrpMeasurements",
                     "Report Relay UE SD-RSRP measurements (dBm).",
                     MakeTraceSourceAccessor (&LteUePhy::m_reportUeSdRsrpMeasurements),
                     "ns3::LteUePhy::SdRsrpTracedCallback")
  ;
  return tid;
}

void
LteUePhy::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_netDevice == nullptr, "LteNetDevice is not available in LteUePhy");
  Ptr<Node> node = m_netDevice->GetNode ();
  NS_ABORT_MSG_IF (node == nullptr, "Node is not available in the LteNetDevice of LteUePhy");
  uint32_t nodeId = node->GetId ();

  if (m_chooseFrameAndSubframeRandomly)
    {
      Simulator::ScheduleWithContext (nodeId, Seconds (0), &LteUePhy::StartRandomInitialSubframeIndication, this);
    }
  else
    {
      uint32_t frameNo = 1;
      uint32_t subframeNo = 1;
      NS_LOG_LOGIC ("Fixed initial frameNo = " << frameNo << " and subframeNo = " << subframeNo);

      Simulator::ScheduleWithContext (nodeId, Seconds (0), &LteUePhy::SubframeIndication, this, frameNo, subframeNo);
    }

  LtePhy::DoInitialize ();
}

void
LteUePhy::StartRandomInitialSubframeIndication ()
{
  NS_LOG_FUNCTION (this);
  uint32_t frameNo = 1;
  uint32_t subframeNo = 1;

  m_uniformRandomVariable->SetAttribute ("Min", DoubleValue (1));
  m_uniformRandomVariable->SetAttribute ("Max", DoubleValue (1024));
  frameNo = m_uniformRandomVariable->GetInteger ();
  m_uniformRandomVariable->SetAttribute ("Min", DoubleValue (1));
  m_uniformRandomVariable->SetAttribute ("Max", DoubleValue (10));
  subframeNo = m_uniformRandomVariable->GetInteger ();

  NS_LOG_DEBUG ("Stream used = " << m_uniformRandomVariable->GetStream ());

  NS_LOG_LOGIC ("Random initial frameNo = " << frameNo << " and subframeNo = " << subframeNo);

  Simulator::ScheduleNow (&LteUePhy::SubframeIndication, this, frameNo, subframeNo);
}

void
LteUePhy::SetLteUePhySapUser (LteUePhySapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_uePhySapUser = s;
  if (m_sidelinkSpectrumPhy)
    {
      //Notify MAC
      m_uePhySapUser->NotifySidelinkEnabled ();
    }
}

LteUePhySapProvider*
LteUePhy::GetLteUePhySapProvider ()
{
  NS_LOG_FUNCTION (this);
  return (m_uePhySapProvider);
}


void
LteUePhy::SetLteUeCphySapUser (LteUeCphySapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_ueCphySapUser = s;
}

LteUeCphySapProvider*
LteUePhy::GetLteUeCphySapProvider ()
{
  NS_LOG_FUNCTION (this);
  return (m_ueCphySapProvider);
}

void
LteUePhy::SetNoiseFigure (double nf)
{
  NS_LOG_FUNCTION (this << nf);
  m_noiseFigure = nf;
}

double
LteUePhy::GetNoiseFigure () const
{
  NS_LOG_FUNCTION (this);
  return m_noiseFigure;
}

void
LteUePhy::SetTxPower (double pow)
{
  NS_LOG_FUNCTION (this << pow);
  m_txPower = pow;
  m_powerControl->SetTxPower (pow);
}

double
LteUePhy::GetTxPower () const
{
  NS_LOG_FUNCTION (this);
  return m_txPower;
}

Ptr<LteUePowerControl>
LteUePhy::GetUplinkPowerControl () const
{
  NS_LOG_FUNCTION (this);
  return m_powerControl;
}

uint8_t
LteUePhy::GetMacChDelay (void) const
{
  NS_LOG_FUNCTION (this);
  return (m_macChTtiDelay);
}

Ptr<LteSpectrumPhy>
LteUePhy::GetDlSpectrumPhy () const
{
  NS_LOG_FUNCTION (this);
  return m_downlinkSpectrumPhy;
}

Ptr<LteSpectrumPhy>
LteUePhy::GetUlSpectrumPhy () const
{
  NS_LOG_FUNCTION (this);
  return m_uplinkSpectrumPhy;
}

void
LteUePhy::SetNumQoutEvalSf (uint16_t numSubframes)
{
  NS_LOG_FUNCTION (this << numSubframes);
  NS_ABORT_MSG_IF (numSubframes % 10 != 0, "Number of subframes used for Qout "
                   "evaluation must be multiple of 10");
  m_numOfQoutEvalSf = numSubframes;
}

void
LteUePhy::SetNumQinEvalSf (uint16_t numSubframes)
{
  NS_LOG_FUNCTION (this << numSubframes);
  NS_ABORT_MSG_IF (numSubframes % 10 != 0, "Number of subframes used for Qin "
                   "evaluation must be multiple of 10");
  m_numOfQinEvalSf = numSubframes;
}

uint16_t
LteUePhy::GetNumQoutEvalSf (void) const
{
  NS_LOG_FUNCTION (this);
  return m_numOfQoutEvalSf;
}

uint16_t
LteUePhy::GetNumQinEvalSf (void) const
{
  NS_LOG_FUNCTION (this);
  return m_numOfQinEvalSf;
}

void
LteUePhy::DoSendMacPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);

  SetMacPdu (p);
}

void
LteUePhy::SetSlSpectrumPhy (Ptr<LteSpectrumPhy> phy)
{
  NS_LOG_FUNCTION (this);
  m_sidelinkSpectrumPhy = phy;
  // forward the info to SL LteSpectrumPhy
  m_sidelinkSpectrumPhy->SetSlRxGain (m_slRxGain);
}

Ptr<LteSpectrumPhy>
LteUePhy::GetSlSpectrumPhy () const
{
  NS_LOG_FUNCTION (this);
  return m_sidelinkSpectrumPhy;
}

void
LteUePhy::DoSendSlMacPdu (Ptr<Packet> p, LteUePhySapProvider::TransmitSlPhySduParameters params)
{
  NS_LOG_FUNCTION (this);

  SetMacPdu (p);

  NS_ASSERT_MSG (m_packetParamsQueue.at (m_packetParamsQueue.size () - 1).size () == 0, "Error: Can only send one sidelink message per TTI");
  m_packetParamsQueue.at (m_packetParamsQueue.size () - 1).push_back (params);
}

std::list <LteUePhySapProvider::TransmitSlPhySduParameters>
LteUePhy::GetSlPhyParameters (Ptr<PacketBurst> pb)
{
  NS_LOG_FUNCTION (this);

  if (m_packetParamsQueue.at (0).size () > 0)
    {
      std::list< LteUePhySapProvider::TransmitSlPhySduParameters > ret = m_packetParamsQueue.at (0);
      m_packetParamsQueue.erase (m_packetParamsQueue.begin ());
      std::list< LteUePhySapProvider::TransmitSlPhySduParameters > newlist;
      m_packetParamsQueue.push_back (newlist);
      return (ret);
    }
  else
    {
      m_packetParamsQueue.erase (m_packetParamsQueue.begin ());
      std::list< LteUePhySapProvider::TransmitSlPhySduParameters > newlist;
      m_packetParamsQueue.push_back (newlist);
      std::list< LteUePhySapProvider::TransmitSlPhySduParameters > emptylist;
      return (emptylist);
    }
}

void
LteUePhy::PhyPduReceived (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  m_uePhySapUser->ReceivePhyPdu (p);
}

void
LteUePhy::PhyPscchPduReceived (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);

  LteSlSciHeader sciHeader;
  p->PeekHeader (sciHeader);
  LteSlSciTag tag;
  p->PeekPacketTag (tag);
  //must check if the destination is one to monitor
  std::list <uint32_t>::iterator it;
  bool for_me = false;
  for (it = m_destinations.begin (); it != m_destinations.end () && !for_me; it++)
    {
      if (sciHeader.GetGroupDstId () == ((*it) & 0xFF))
        {
          NS_LOG_INFO ("received SCI for group " << (uint32_t)((*it) & 0xFF) << " from rnti " << tag.GetRnti ());

          //todo, how to find the pool among the available ones?
          //right now just use the first one
          std::list <RxPoolInfo>::iterator poolIt = m_sidelinkRxPools.begin ();
          if (poolIt == m_sidelinkRxPools.end ())
            {
              NS_LOG_INFO (this << " No Rx pool configured");
            }
          else
            {
              //this is the first transmission of PSCCH
              std::map<uint16_t, SidelinkGrantInfo>::iterator grantIt = poolIt->m_currentGrants.find (tag.GetRnti ());
              if (grantIt == poolIt->m_currentGrants.end ())
                {
                  SidelinkGrantInfo txInfo;

                  txInfo.m_grantReceived = true;
                  txInfo.m_grant.m_rnti = tag.GetRnti ();
                  txInfo.m_grant.m_resPscch = tag.GetResNo ();
                  txInfo.m_grant.m_rbStart = sciHeader.GetRbStart ();
                  txInfo.m_grant.m_rbLen = sciHeader.GetRbLen ();
                  txInfo.m_grant.m_hopping = sciHeader.IsHopping ();
                  txInfo.m_grant.m_hoppingInfo = sciHeader.GetHoppingInfo ();
                  txInfo.m_grant.m_trp = sciHeader.GetTrp ();
                  txInfo.m_grant.m_groupDstId = sciHeader.GetGroupDstId ();
                  txInfo.m_grant.m_mcs = sciHeader.GetMcs ();
                  txInfo.m_grant.m_tbSize = tag.GetTbSize ();

                  //insert grant
                  poolIt->m_currentGrants.insert (std::pair <uint16_t, SidelinkGrantInfo> (tag.GetRnti (), txInfo));
                }           //else it should be the retransmission and the data should be the same...add check
              else
                {
                  NS_LOG_DEBUG ("SCI Grant already present");
                }
            }
        }
    }
  //Pass information to the MAC
  m_uePhySapUser->ReceiveSlSciPhyPdu (p);
}

void
LteUePhy::PhyPsdchPduReceived (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  m_uePhySapUser->ReceiveSlDiscPhyPdu (p);
}

void
LteUePhy::PhyPsbchPduReceived (Ptr<Packet> p, uint16_t slssid)
{
  NS_LOG_FUNCTION (this);

  //Store the received MIB-SL during the SyncRef search
  if (m_ueSlssScanningInProgress)
    {
      MasterInformationBlockSlHeader mibSlHeader;
      p->PeekHeader (mibSlHeader);
      LteRrcSap::MasterInformationBlockSL mibSl = mibSlHeader.GetMessage ();

      uint16_t rxOffset = Simulator::Now ().GetMilliSeconds () % 40;
      m_detectedMibSl.insert (std::pair <std::pair<uint16_t, uint16_t>, LteRrcSap::MasterInformationBlockSL>
                                (std::pair<uint16_t, uint16_t> (slssid, rxOffset), mibSl));
    }

  //Pass the message to the RRC
  m_ueCphySapUser->ReceiveMibSL (p, slssid);

}


void
LteUePhy::SetSubChannelsForTransmission (std::vector <int> mask)
{
  NS_LOG_FUNCTION (this);

  m_subChannelsForTransmission = mask;

  Ptr<SpectrumValue> txPsd = CreateTxPowerSpectralDensity ();
  m_uplinkSpectrumPhy->SetTxPowerSpectralDensity (txPsd);
}


void
LteUePhy::SetSubChannelsForReception (std::vector <int> mask)
{
  NS_LOG_FUNCTION (this);
  m_subChannelsForReception = mask;
}


std::vector <int>
LteUePhy::GetSubChannelsForTransmission ()
{
  NS_LOG_FUNCTION (this);
  return m_subChannelsForTransmission;
}


std::vector <int>
LteUePhy::GetSubChannelsForReception ()
{
  NS_LOG_FUNCTION (this);
  return m_subChannelsForReception;
}


Ptr<SpectrumValue>
LteUePhy::CreateTxPowerSpectralDensity ()
{
  NS_LOG_FUNCTION (this);
  LteSpectrumValueHelper psdHelper;
  Ptr<SpectrumValue> psd = psdHelper.CreateUlTxPowerSpectralDensity (m_ulEarfcn, m_ulBandwidth, m_txPower, GetSubChannelsForTransmission ());
  m_reportPowerSpectralDensity (m_rnti, psd);

  return psd;
}

void
LteUePhy::GenerateCtrlCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);
  /**
   * We do not generate the CQI report
   * when the UE is not synchronized to any cell.
   *
   * Also, the RLF is detected after the DL CTRL
   * is received by the UE,therefore, we do not need
   * to generate the CQI reports and the UE measurements
   * for a CTRL for which the RLF has been detected.
   */
  if (m_cellId == 0)
    {
      return;
    }
  m_ctrlSinrForRlf = sinr;
  GenerateCqiRsrpRsrq (sinr);
}

void
LteUePhy::GenerateCqiRsrpRsrq (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr);

  NS_ASSERT (m_state != CELL_SEARCH);
  NS_ASSERT (m_cellId > 0);

  if (m_dlConfigured && m_ulConfigured && (m_rnti > 0))
    {
      // check periodic wideband CQI
      if (Simulator::Now () > m_p10CqiLast + m_p10CqiPeriodicity)
        {
          NS_LOG_DEBUG ("Reporting P10 CQI at : " << Simulator::Now ().As (Time::MS)
                                                  << ". Last reported at : " << m_p10CqiLast.As (Time::MS));
          Ptr<LteUeNetDevice> thisDevice = GetDevice ()->GetObject<LteUeNetDevice> ();
          Ptr<DlCqiLteControlMessage> msg = CreateDlCqiFeedbackMessage (sinr);
          if (msg)
            {
              DoSendLteControlMessage (msg);
              m_uePhySapUser->NotifyUlTransmission ();
            }
          m_p10CqiLast = Simulator::Now ();
        }
      // check aperiodic high-layer configured subband CQI
      if  (Simulator::Now () > m_a30CqiLast + m_a30CqiPeriodicity)
        {
          NS_LOG_DEBUG ("Reporting A30 CQI at : " << Simulator::Now ().As (Time::MS)
                                                  << ". Last reported at : " << m_a30CqiLast.As (Time::MS));
          Ptr<LteUeNetDevice> thisDevice = GetDevice ()->GetObject<LteUeNetDevice> ();
          Ptr<DlCqiLteControlMessage> msg = CreateDlCqiFeedbackMessage (sinr);
          if (msg)
            {
              DoSendLteControlMessage (msg);
              m_uePhySapUser->NotifyUlTransmission ();
            }
          m_a30CqiLast = Simulator::Now ();
        }
    }

  // Generate PHY trace
  m_rsrpSinrSampleCounter++;
  if (m_rsrpSinrSampleCounter == m_rsrpSinrSamplePeriod)
    {
      NS_ASSERT_MSG (m_rsReceivedPowerUpdated, " RS received power info obsolete");
      // RSRP evaluated as averaged received power among RBs
      double sum = 0.0;
      uint8_t rbNum = 0;
      Values::const_iterator it;
      for (it = m_rsReceivedPower.ConstValuesBegin (); it != m_rsReceivedPower.ConstValuesEnd (); it++)
        {
          // convert PSD [W/Hz] to linear power [W] for the single RE
          // we consider only one RE for the RS since the channel is
          // flat within the same RB
          double powerTxW = ((*it) * 180000.0) / 12.0;
          sum += powerTxW;
          rbNum++;
        }
      double rsrp = (rbNum > 0) ? (sum / rbNum) : DBL_MAX;
      // averaged SINR among RBs
      double avSinr = ComputeAvgSinr (sinr);

      NS_LOG_INFO (this << " cellId " << m_cellId << " rnti " << m_rnti << " RSRP " << rsrp << " SINR " << avSinr << " ComponentCarrierId " << (uint16_t) m_componentCarrierId);
      //trigger RLF detection only when UE has an active RRC connection
      //and RLF detection attribute is set to true
      if (m_isConnected && m_enableRlfDetection)
        {
          double avrgSinrForRlf = ComputeAvgSinr (m_ctrlSinrForRlf);
          RlfDetection (10 * log10 (avrgSinrForRlf));
        }

      m_reportCurrentCellRsrpSinrTrace (m_cellId, m_rnti, rsrp, avSinr, (uint16_t) m_componentCarrierId);
      m_rsrpSinrSampleCounter = 0;
    }

  if (m_pssReceived)
    {
      // measure instantaneous RSRQ now
      NS_ASSERT_MSG (m_rsInterferencePowerUpdated, " RS interference power info obsolete");

      std::list <PssElement>::iterator itPss = m_pssList.begin ();
      while (itPss != m_pssList.end ())
        {
          uint16_t rbNum = 0;
          double rssiSum = 0.0;

          Values::const_iterator itIntN = m_rsInterferencePower.ConstValuesBegin ();
          Values::const_iterator itPj = m_rsReceivedPower.ConstValuesBegin ();
          for (itPj = m_rsReceivedPower.ConstValuesBegin ();
               itPj != m_rsReceivedPower.ConstValuesEnd ();
               itIntN++, itPj++)
            {
              rbNum++;
              // convert PSD [W/Hz] to linear power [W] for the single RE
              double interfPlusNoisePowerTxW = ((*itIntN) * 180000.0) / 12.0;
              double signalPowerTxW = ((*itPj) * 180000.0) / 12.0;
              rssiSum += (2 * (interfPlusNoisePowerTxW + signalPowerTxW));
            }

          NS_ASSERT (rbNum == (*itPss).nRB);
          double rsrq_dB = 10 * log10 ((*itPss).pssPsdSum / rssiSum);

          if (rsrq_dB > m_pssReceptionThreshold)
            {
              NS_LOG_INFO (this << " PSS RNTI " << m_rnti << " cellId " << m_cellId
                                << " has RSRQ " << rsrq_dB << " and RBnum " << rbNum);
              // store measurements
              std::map <uint16_t, UeMeasurementsElement>::iterator itMeasMap;
              itMeasMap = m_ueMeasurementsMap.find ((*itPss).cellId);
              if (itMeasMap != m_ueMeasurementsMap.end ())
                {
                  (*itMeasMap).second.rsrqSum += rsrq_dB;
                  (*itMeasMap).second.rsrqNum++;
                }
              else
                {
                  NS_LOG_WARN ("race condition of bug 2091 occurred");
                }
            }

          itPss++;

        } // end of while (itPss != m_pssList.end ())

      m_pssList.clear ();

    } // end of if (m_pssReceived)

} // end of void LteUePhy::GenerateCtrlCqiReport (const SpectrumValue& sinr)

double
LteUePhy::ComputeAvgSinr (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);

  // averaged SINR among RBs
  double sum = 0.0;
  uint8_t rbNum = 0;
  Values::const_iterator it;

  for (it = sinr.ConstValuesBegin (); it != sinr.ConstValuesEnd (); it++)
    {
      sum += (*it);
      rbNum++;
    }

  double avrgSinr = (rbNum > 0) ? (sum / rbNum) : DBL_MAX;

  return avrgSinr;
}

void
LteUePhy::GenerateDataCqiReport (const SpectrumValue& sinr)
{
  // Not used by UE, CQI are based only on RS
}

void
LteUePhy::GenerateMixedCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);

  /**
   * We do not generate the CQI report
   * when the UE is not synchronized to any cell.
   *
   * Also, the RLF is detected after the DL CTRL
   * is received by the UE,therefore, we do not need
   * to generate the CQI reports and the UE measurements
   * for a CTRL for which the RLF has been detected.
   */
  if (m_cellId == 0)
    {
      return;
    }

  NS_ASSERT (m_state != CELL_SEARCH);
  //NOTE: The SINR received by this method is
  //based on CTRL, which is not used to compute
  //PDSCH (i.e., data) based SINR. It is used
  //for RLF detection.
  m_ctrlSinrForRlf = sinr;

  SpectrumValue mixedSinr = (m_rsReceivedPower * m_paLinear);
  if (m_dataInterferencePowerUpdated)
    {
      // we have a measurement of interf + noise for the denominator
      // of SINR = S/(I+N)
      mixedSinr /= m_dataInterferencePower;
      m_dataInterferencePowerUpdated = false;
      NS_LOG_LOGIC ("data interf measurement available, SINR = " << mixedSinr);
    }
  else
    {
      // we did not see any interference on data, so interference is
      // there and we have only noise at the denominator of SINR
      mixedSinr /= (*m_noisePsd);
      NS_LOG_LOGIC ("no data interf measurement available, SINR = " << mixedSinr);
    }

  /*
   * some RBs are not used in PDSCH and their SINR is very high
   * for example with bandwidth 25, last RB is not used
   * it can make avgSinr value very high, what is incorrect
   */
  uint32_t rbgSize = GetRbgSize ();
  uint32_t modulo = m_dlBandwidth % rbgSize;
  double avgMixedSinr = 0;
  uint32_t usedRbgNum = 0;
  for (uint32_t i = 0; i < (m_dlBandwidth - 1 - modulo); i++)
    {
      usedRbgNum++;
      avgMixedSinr += mixedSinr[i];
    }
  avgMixedSinr = avgMixedSinr / usedRbgNum;
  for (uint32_t i = 0; i < modulo; i++)
    {
      mixedSinr[m_dlBandwidth - 1 - i] = avgMixedSinr;
    }

  GenerateCqiRsrpRsrq (mixedSinr);
}

void
LteUePhy::ReportInterference (const SpectrumValue& interf)
{
  NS_LOG_FUNCTION (this << interf);
  m_rsInterferencePowerUpdated = true;
  m_rsInterferencePower = interf;
}

void
LteUePhy::ReportDataInterference (const SpectrumValue& interf)
{
  NS_LOG_FUNCTION (this << interf);

  m_dataInterferencePowerUpdated = true;
  m_dataInterferencePower = interf;
}

void
LteUePhy::ReportRsReceivedPower (const SpectrumValue& power)
{
  NS_LOG_FUNCTION (this << power);
  m_rsReceivedPowerUpdated = true;
  m_rsReceivedPower = power;

  if (m_enableUplinkPowerControl)
    {
      double sum = 0;
      uint32_t rbNum = 0;
      Values::const_iterator it;
      for (it = m_rsReceivedPower.ConstValuesBegin (); it != m_rsReceivedPower.ConstValuesEnd (); it++)
        {
          double powerTxW = ((*it) * 180000);
          sum += powerTxW;
          rbNum++;
        }
      double rsrp = 10 * log10 (sum) + 30;

      NS_LOG_INFO ("RSRP: " << rsrp);
      m_powerControl->SetRsrp (rsrp);
    }
}

Ptr<DlCqiLteControlMessage>
LteUePhy::CreateDlCqiFeedbackMessage (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);


  // apply transmission mode gain
  NS_ASSERT (m_transmissionMode < m_txModeGain.size ());
  SpectrumValue newSinr = sinr;
  newSinr *= m_txModeGain.at (m_transmissionMode);

  // CREATE DlCqiLteControlMessage
  Ptr<DlCqiLteControlMessage> msg = Create<DlCqiLteControlMessage> ();
  CqiListElement_s dlcqi;
  std::vector<int> cqi;
  if (Simulator::Now () > m_p10CqiLast + m_p10CqiPeriodicity)
    {
      cqi = m_amc->CreateCqiFeedbacks (newSinr, m_dlBandwidth);

      int nLayer = TransmissionModesLayers::TxMode2LayerNum (m_transmissionMode);
      int nbSubChannels = cqi.size ();
      double cqiSum = 0.0;
      int activeSubChannels = 0;
      // average the CQIs of the different RBs
      for (int i = 0; i < nbSubChannels; i++)
        {
          if (cqi.at (i) != -1)
            {
              cqiSum += cqi.at (i);
              activeSubChannels++;
            }
          NS_LOG_DEBUG (this << " subch " << i << " cqi " <<  cqi.at (i));
        }
      dlcqi.m_rnti = m_rnti;
      dlcqi.m_ri = 1; // not yet used
      dlcqi.m_cqiType = CqiListElement_s::P10; // Periodic CQI using PUCCH wideband
      NS_ASSERT_MSG (nLayer > 0, " nLayer negative");
      NS_ASSERT_MSG (nLayer < 3, " nLayer limit is 2s");
      for (int i = 0; i < nLayer; i++)
        {
          if (activeSubChannels > 0)
            {
              dlcqi.m_wbCqi.push_back ((uint16_t) cqiSum / activeSubChannels);
            }
          else
            {
              // approximate with the worst case -> CQI = 1
              dlcqi.m_wbCqi.push_back (1);
            }
        }
      //NS_LOG_DEBUG (this << " Generate P10 CQI feedback " << (uint16_t) cqiSum / activeSubChannels);
      dlcqi.m_wbPmi = 0; // not yet used
      // dl.cqi.m_sbMeasResult others CQI report modes: not yet implemented
    }
  else if (Simulator::Now () > m_a30CqiLast + m_a30CqiPeriodicity)
    {
      cqi = m_amc->CreateCqiFeedbacks (newSinr, GetRbgSize ());
      int nLayer = TransmissionModesLayers::TxMode2LayerNum (m_transmissionMode);
      int nbSubChannels = cqi.size ();
      int rbgSize = GetRbgSize ();
      double cqiSum = 0.0;
      int cqiNum = 0;
      SbMeasResult_s rbgMeas;
      //NS_LOG_DEBUG (this << " Create A30 CQI feedback, RBG " << rbgSize << " cqiNum " << nbSubChannels << " band "  << (uint16_t)m_dlBandwidth);
      for (int i = 0; i < nbSubChannels; i++)
        {
          if (cqi.at (i) != -1)
            {
              cqiSum += cqi.at (i);
            }
          // else "nothing" no CQI is treated as CQI = 0 (worst case scenario)
          cqiNum++;
          if (cqiNum == rbgSize)
            {
              // average the CQIs of the different RBGs
              //NS_LOG_DEBUG (this << " RBG CQI "  << (uint16_t) cqiSum / rbgSize);
              HigherLayerSelected_s hlCqi;
              hlCqi.m_sbPmi = 0; // not yet used
              for (int i = 0; i < nLayer; i++)
                {
                  hlCqi.m_sbCqi.push_back ((uint16_t) cqiSum / rbgSize);
                }
              rbgMeas.m_higherLayerSelected.push_back (hlCqi);
              cqiSum = 0.0;
              cqiNum = 0;
            }
        }
      dlcqi.m_rnti = m_rnti;
      dlcqi.m_ri = 1; // not yet used
      dlcqi.m_cqiType = CqiListElement_s::A30; // Aperiodic CQI using PUSCH
      //dlcqi.m_wbCqi.push_back ((uint16_t) cqiSum / nbSubChannels);
      dlcqi.m_wbPmi = 0; // not yet used
      dlcqi.m_sbMeasResult = rbgMeas;
    }

  msg->SetDlCqi (dlcqi);
  return msg;
}


void
LteUePhy::ReportUeMeasurements ()
{
  NS_LOG_FUNCTION (this << Simulator::Now ());
  NS_LOG_DEBUG (this << " Report UE Measurements ");

  LteUeCphySapUser::UeMeasurementsParameters ret;
  ret.m_componentCarrierId = m_componentCarrierId;

  std::map <uint16_t, UeMeasurementsElement>::iterator it;
  for (it = m_ueMeasurementsMap.begin (); it != m_ueMeasurementsMap.end (); it++)
    {
      double avg_rsrp = (*it).second.rsrpSum / static_cast<double> ((*it).second.rsrpNum);
      double avg_rsrq = (*it).second.rsrqSum / static_cast<double> ((*it).second.rsrqNum);
      /*
       * In CELL_SEARCH state, this may result in avg_rsrq = 0/0 = -nan.
       * UE RRC must take this into account when receiving measurement reports.
       * TODO remove this shortcoming by calculating RSRQ during CELL_SEARCH
       */
      NS_LOG_DEBUG (this << " CellId " << (*it).first
                         << " RSRP " << avg_rsrp
                         << " (nSamples " << (uint16_t)(*it).second.rsrpNum << ")"
                         << " RSRQ " << avg_rsrq
                         << " (nSamples " << (uint16_t)(*it).second.rsrqNum << ")"
                         << " ComponentCarrierID " << (uint16_t)m_componentCarrierId);

      if (avg_rsrp >= m_rsrpReceptionThreshold)
        {
          LteUeCphySapUser::UeMeasurementsElement newEl;
          newEl.m_cellId = (*it).first;
          newEl.m_rsrp = avg_rsrp;
          newEl.m_rsrq = avg_rsrq;
          ret.m_ueMeasurementsList.push_back (newEl);
        }
      // report to UE measurements trace
      m_reportUeMeasurements (m_rnti, (*it).first, avg_rsrp, avg_rsrq, ((*it).first == m_cellId ? 1 : 0), m_componentCarrierId);
    }

  // report to RRC
  m_ueCphySapUser->ReportUeMeasurements (ret);

  m_ueMeasurementsMap.clear ();
  Simulator::Schedule (m_ueMeasurementsFilterPeriod, &LteUePhy::ReportUeMeasurements, this);
}

void
LteUePhy::SetDownlinkCqiPeriodicity (Time cqiPeriodicity)
{
  NS_LOG_FUNCTION (this << cqiPeriodicity);
  m_a30CqiPeriodicity = cqiPeriodicity;
  m_p10CqiPeriodicity = cqiPeriodicity;
}

void
LteUePhy::DoSendLteControlMessage (Ptr<LteControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);

  SetControlMessages (msg);
}

void
LteUePhy::DoSendRachPreamble (uint32_t raPreambleId, uint32_t raRnti)
{
  NS_LOG_FUNCTION (this << raPreambleId);

  // unlike other control messages, RACH preamble is sent ASAP
  Ptr<RachPreambleLteControlMessage> msg = Create<RachPreambleLteControlMessage> ();
  msg->SetRapId (raPreambleId);
  m_raPreambleId = raPreambleId;
  m_raRnti = raRnti;
  m_controlMessagesQueue.at (0).push_back (msg);
}

void
LteUePhy::DoNotifyConnectionSuccessful ()
{
  /**
   * Radio link failure detection should take place only on the
   * primary carrier to avoid errors due to multiple calls to the
   * same methods at the RRC layer
   */
  if (m_componentCarrierId == 0)
    {
      m_isConnected = true;
      // Initialize the parameters for radio link failure detection
      InitializeRlfParams ();
    }
}



void
LteUePhy::ReceiveLteControlMessageList (std::list<Ptr<LteControlMessage> > msgList)
{
  NS_LOG_FUNCTION (this);

  std::list<Ptr<LteControlMessage> >::iterator it;
  NS_LOG_DEBUG (this << " I am rnti = " << m_rnti << " and I received msgs " << (uint16_t) msgList.size ());
  for (it = msgList.begin (); it != msgList.end (); it++)
    {
      Ptr<LteControlMessage> msg = (*it);

      if (msg->GetMessageType () == LteControlMessage::DL_DCI)
        {
          Ptr<DlDciLteControlMessage> msg2 = DynamicCast<DlDciLteControlMessage> (msg);

          DlDciListElement_s dci = msg2->GetDci ();
          if (dci.m_rnti != m_rnti)
            {
              // DCI not for me
              continue;
            }

          if (dci.m_resAlloc != 0)
            {
              NS_FATAL_ERROR ("Resource Allocation type not implemented");
            }

          std::vector <int> dlRb;

          // translate the DCI to Spectrum framework
          uint32_t mask = 0x1;
          for (int i = 0; i < 32; i++)
            {
              if (((dci.m_rbBitmap & mask) >> i) == 1)
                {
                  for (int k = 0; k < GetRbgSize (); k++)
                    {
                      dlRb.push_back ((i * GetRbgSize ()) + k);
//             NS_LOG_DEBUG(this << " RNTI " << m_rnti << " RBG " << i << " DL-DCI allocated PRB " << (i*GetRbgSize()) + k);
                    }
                }
              mask = (mask << 1);
            }
          if (m_enableUplinkPowerControl)
            {
              m_powerControl->ReportTpc (dci.m_tpc);
            }


          // send TB info to LteSpectrumPhy
          NS_LOG_DEBUG (this << " UE " << m_rnti << " DL-DCI " << dci.m_rnti << " bitmap "  << dci.m_rbBitmap);
          for (uint8_t i = 0; i < dci.m_tbsSize.size (); i++)
            {
              m_downlinkSpectrumPhy->AddExpectedTb (dci.m_rnti, dci.m_ndi.at (i), dci.m_tbsSize.at (i), dci.m_mcs.at (i), dlRb, i, dci.m_harqProcess, dci.m_rv.at (i), true /* DL */);
            }

          SetSubChannelsForReception (dlRb);

          //Since we are receiving packets in the downlink, we will need to send an HARQ feedback, let's tell the MAC
          //about it now so it does not schedule sidelink transmission
          m_uePhySapUser->NotifyUlTransmission ();
        }
      else if (msg->GetMessageType () == LteControlMessage::UL_DCI)
        {
          // set the uplink bandwidth according to the UL-CQI
          Ptr<UlDciLteControlMessage> msg2 = DynamicCast<UlDciLteControlMessage> (msg);
          UlDciListElement_s dci = msg2->GetDci ();
          if (dci.m_rnti != m_rnti)
            {
              // DCI not for me
              continue;
            }
          NS_LOG_INFO (this << " UL DCI");
          std::vector <int> ulRb;
          for (int i = 0; i < dci.m_rbLen; i++)
            {
              ulRb.push_back (i + dci.m_rbStart);
              //NS_LOG_DEBUG (this << " UE RB " << i + dci.m_rbStart);
            }
          m_reportUlPhyResourceBlocks (m_rnti, ulRb);
          QueueSubChannelsForTransmission (ulRb);
          // fire trace of UL Tx PHY stats
          HarqProcessInfoList_t harqInfoList = m_harqPhyModule->GetHarqProcessInfoUl (m_rnti, 0);
          PhyTransmissionStatParameters params;
          params.m_cellId = m_cellId;
          params.m_imsi = 0; // it will be set by DlPhyTransmissionCallback in LteHelper
          params.m_timestamp = Simulator::Now ().GetMilliSeconds () + UL_PUSCH_TTIS_DELAY;
          params.m_rnti = m_rnti;
          params.m_txMode = 0; // always SISO for UE
          params.m_layer = 0;
          params.m_mcs = dci.m_mcs;
          params.m_size = dci.m_tbSize;
          params.m_rv = harqInfoList.size ();
          params.m_ndi = dci.m_ndi;
          params.m_ccId = m_componentCarrierId;
          m_ulPhyTransmission (params);
          // pass the info to the MAC
          m_uePhySapUser->ReceiveLteControlMessage (msg);
        }
      else if (msg->GetMessageType () == LteControlMessage::RAR)
        {
          Ptr<RarLteControlMessage> rarMsg = DynamicCast<RarLteControlMessage> (msg);
          if (rarMsg->GetRaRnti () == m_raRnti)
            {
              for (std::list<RarLteControlMessage::Rar>::const_iterator it = rarMsg->RarListBegin (); it != rarMsg->RarListEnd (); ++it)
                {
                  if (it->rapId != m_raPreambleId)
                    {
                      // UL grant not for me
                      continue;
                    }
                  else
                    {
                      NS_LOG_INFO ("received RAR RNTI " << m_raRnti);
                      // set the uplink bandwidth according to the UL grant
                      std::vector <int> ulRb;
                      for (int i = 0; i < it->rarPayload.m_grant.m_rbLen; i++)
                        {
                          ulRb.push_back (i + it->rarPayload.m_grant.m_rbStart);
                        }

                      QueueSubChannelsForTransmission (ulRb);
                      // pass the info to the MAC
                      m_uePhySapUser->ReceiveLteControlMessage (msg);
                      // reset RACH variables with out of range values
                      m_raPreambleId = 255;
                      m_raRnti = 11;
                    }
                }
            }
        }
      else if (msg->GetMessageType () == LteControlMessage::MIB)
        {
          NS_LOG_INFO ("received MIB");
          NS_ASSERT (m_cellId > 0);
          Ptr<MibLteControlMessage> msg2 = DynamicCast<MibLteControlMessage> (msg);
          m_ueCphySapUser->RecvMasterInformationBlock (m_cellId, msg2->GetMib ());
        }
      else if (msg->GetMessageType () == LteControlMessage::SIB1)
        {
          NS_LOG_INFO ("received SIB1");
          NS_ASSERT (m_cellId > 0);
          Ptr<Sib1LteControlMessage> msg2 = DynamicCast<Sib1LteControlMessage> (msg);
          m_ueCphySapUser->RecvSystemInformationBlockType1 (m_cellId, msg2->GetSib1 ());
        }
      else if (msg->GetMessageType () == LteControlMessage::SL_DCI)
        {
          Ptr<SlDciLteControlMessage> msg2 = DynamicCast<SlDciLteControlMessage> (msg);
          SlDciListElement_s dci = msg2->GetDci ();
          if (dci.m_rnti != m_rnti)
            {
              // DCI not for me
              continue;
            }
          NS_LOG_INFO ("received SL_DCI");
          // pass the info to the MAC
          m_uePhySapUser->ReceiveLteControlMessage (msg);
        }
      else
        {
          // pass the message to UE-MAC
          m_uePhySapUser->ReceiveLteControlMessage (msg);
        }
    }
}

void
LteUePhy::ReceivePss (uint16_t cellId, Ptr<SpectrumValue> p)
{
  NS_LOG_FUNCTION (this << cellId << (*p));

  double sum = 0.0;
  uint16_t nRB = 0;
  Values::const_iterator itPi;
  for (itPi = p->ConstValuesBegin (); itPi != p->ConstValuesEnd (); itPi++)
    {
      // convert PSD [W/Hz] to linear power [W] for the single RE
      double powerTxW = ((*itPi) * 180000.0) / 12.0;
      sum += powerTxW;
      nRB++;
    }

  // measure instantaneous RSRP now
  double rsrp_dBm = 10 * log10 (1000 * (sum / static_cast<double> (nRB)));
  NS_LOG_INFO (this << " PSS RNTI " << m_rnti << " cellId " << m_cellId
                    << " has RSRP " << rsrp_dBm << " and RBnum " << nRB);
  // note that m_pssReceptionThreshold does not apply here

  // store measurements
  std::map <uint16_t, UeMeasurementsElement>::iterator itMeasMap = m_ueMeasurementsMap.find (cellId);
  if (itMeasMap == m_ueMeasurementsMap.end ())
    {
      // insert new entry
      UeMeasurementsElement newEl;
      newEl.rsrpSum = rsrp_dBm;
      newEl.rsrpNum = 1;
      newEl.rsrqSum = 0;
      newEl.rsrqNum = 0;
      m_ueMeasurementsMap.insert (std::pair <uint16_t, UeMeasurementsElement> (cellId, newEl));
    }
  else
    {
      (*itMeasMap).second.rsrpSum += rsrp_dBm;
      (*itMeasMap).second.rsrpNum++;
    }

  /*
   * Collect the PSS for later processing in GenerateCtrlCqiReport()
   * (to be called from ChunkProcessor after RX is finished).
   */
  m_pssReceived = true;
  PssElement el;
  el.cellId = cellId;
  el.pssPsdSum = sum;
  el.nRB = nRB;
  m_pssList.push_back (el);

} // end of void LteUePhy::ReceivePss (uint16_t cellId, Ptr<SpectrumValue> p)


void
LteUePhy::QueueSubChannelsForTransmission (std::vector <int> rbMap)
{
  m_subChannelsForTransmissionQueue.at (m_macChTtiDelay - 1) = rbMap;
}


void
LteUePhy::SubframeIndication (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);

  NS_ASSERT_MSG (frameNo > 0, "the SRS index check code assumes that frameNo starts at 1");

  // refresh internal variables
  m_rsReceivedPowerUpdated = false;
  m_rsInterferencePowerUpdated = false;
  m_pssReceived = false;
  // Clear expected TB not received in previous subframes
  if (m_sidelinkSpectrumPhy)
    {
      m_sidelinkSpectrumPhy->ClearExpectedSlTb ();
      //Notify RRC about the current Subframe indication
      m_ueCphySapUser->ReportSubframeIndication (frameNo, subframeNo);
    }
  //If a change of timing (resynchronization) was requested before, do the change of frameNo and subframeNo if possible
  // Do it here for avoiding  miss alignments of subframe indications
  if (m_resyncRequested)
    {
      NS_LOG_LOGIC ("(re)synchronization requested ");
      if (ChangeOfTiming (frameNo, subframeNo) )
        {
          frameNo = m_currFrameNo;
          subframeNo = m_currSubframeNo;
          NS_LOG_LOGIC ("(re)synchronization successfully performed");
        }
      else
        {
          NS_LOG_LOGIC ("(re)synchronization postponed");
        }
    }

  if (m_ulConfigured)
    {
      InitializeDiscRxPool (frameNo, subframeNo);

      if (m_slTxPoolInfo.m_pool)
        {
          //Check if we need to initialize the Tx pool
          if (m_slTxPoolInfo.m_nextScPeriod.frameNo == 0)
            {
              //pool not initialized yet
              m_slTxPoolInfo.m_nextScPeriod = m_slTxPoolInfo.m_pool->GetNextScPeriod (frameNo - 1, subframeNo - 1);
              //adjust because scheduler starts with frame/subframe = 1
              m_slTxPoolInfo.m_nextScPeriod.frameNo++;
              m_slTxPoolInfo.m_nextScPeriod.subframeNo++;
              NS_LOG_INFO ("Tx Pool initialized");
            }
          //Check if this is a new SC period
          if (frameNo == m_slTxPoolInfo.m_nextScPeriod.frameNo && subframeNo == m_slTxPoolInfo.m_nextScPeriod.subframeNo)
            {
              m_slTxPoolInfo.m_currentScPeriod = m_slTxPoolInfo.m_nextScPeriod;
              m_slTxPoolInfo.m_nextScPeriod = m_slTxPoolInfo.m_pool->GetNextScPeriod (frameNo - 1, subframeNo - 1);
              //adjust because scheduler starts with frame/subframe = 1
              m_slTxPoolInfo.m_nextScPeriod.frameNo++;
              m_slTxPoolInfo.m_nextScPeriod.subframeNo++;
              NS_LOG_INFO ("Starting new SC period for TX pool " << ". Next period at " << m_slTxPoolInfo.m_nextScPeriod.frameNo << "/" << m_slTxPoolInfo.m_nextScPeriod.subframeNo);

              if (m_waitingNextScPeriod)
                {
                  NS_LOG_LOGIC ("The UE was waiting for next SC period and it just started");
                  m_waitingNextScPeriod = false;
                }
            }
        }

      //check if we received grants for Sidelink
      //compute the reception slots for the PSSCH. Do this here because
      //we did not have access to the frame/subframe at the reception

      std::list <RxPoolInfo>::iterator it;
      for (it = m_sidelinkRxPools.begin (); it != m_sidelinkRxPools.end (); it++)
        {
          std::map <uint16_t, SidelinkGrantInfo>::iterator grantIt = it->m_currentGrants.begin ();
          while (grantIt != it->m_currentGrants.end ())
            {
              std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator rxIt;

              if (grantIt->second.m_grantReceived)
                {
                  NS_LOG_INFO ("New grant received");
                  //TODO: how to identify pool if multiple are presents?
                  SidelinkCommResourcePool::SubframeInfo tmp = it->m_pool->GetCurrentScPeriod (frameNo - 1, subframeNo - 1);
                  grantIt->second.m_psschTx = it->m_pool->GetPsschTransmissions (tmp, grantIt->second.m_grant.m_trp,
                                                                                 grantIt->second.m_grant.m_rbStart, grantIt->second.m_grant.m_rbLen);

                  for (rxIt = grantIt->second.m_psschTx.begin (); rxIt != grantIt->second.m_psschTx.end (); rxIt++)
                    {
                      //adjust for index starting at 1
                      rxIt->subframe.frameNo++;
                      rxIt->subframe.subframeNo++;
                      NS_LOG_INFO ("Subframe Rx" << rxIt->subframe.frameNo << "/" << rxIt->subframe.subframeNo << ": rbStart=" << (uint32_t) rxIt->rbStart << ", rbLen=" << (uint32_t) rxIt->nbRb);
                    }

                  grantIt->second.m_grantReceived = false;
                }

              //now check if there is any grant for the current subframe
              rxIt = grantIt->second.m_psschTx.begin ();

              if (rxIt != grantIt->second.m_psschTx.end ())
                {
                  NS_LOG_DEBUG (frameNo << "/" << subframeNo << " RNTI=" << m_rnti << " next pssch at " << (*rxIt).subframe.frameNo << "/" << (*rxIt).subframe.subframeNo);
                }

              if (rxIt != grantIt->second.m_psschTx.end () && (*rxIt).subframe.frameNo == frameNo && (*rxIt).subframe.subframeNo == subframeNo)
                {
                  //reception
                  NS_LOG_INFO ("Expecting PSSCH reception RB " << (uint16_t) rxIt->rbStart << " to " << (uint16_t) (rxIt->rbStart + rxIt->nbRb - 1));
                  std::vector <int> rbMap;
                  for (int i = rxIt->rbStart; i < rxIt->rbStart + rxIt->nbRb; i++)
                    {
                      rbMap.push_back (i);
                    }
                  //Compute redundancy version number
                  uint8_t rv = grantIt->second.m_psschTx.size () % 4;
                  rv = rv == 0 ? rv : 4 - rv;

                  NS_LOG_DEBUG ("RV = " << (uint16_t) rv << " Number of PSSCH transmissions "
                                        << grantIt->second.m_psschTx.size ());

                  m_sidelinkSpectrumPhy->AddExpectedTb (grantIt->second.m_grant.m_rnti,
                                                        grantIt->second.m_grant.m_groupDstId,
                                                        grantIt->second.m_psschTx.size () % 4 == 0,
                                                        grantIt->second.m_grant.m_tbSize,
                                                        grantIt->second.m_grant.m_mcs, rbMap, rv);
                  //remove reception information
                  grantIt->second.m_psschTx.erase (rxIt);

                }
              if (grantIt->second.m_psschTx.size () == 0)
                {
                  //no more PSSCH transmission, clear the grant
                  it->m_currentGrants.erase (grantIt++);
                }
              else
                {
                  grantIt++;
                }
            } // end of while (grantIt != it->m_currentGrants.end ())
        } // end of for (it = m_sidelinkRxPools.begin () ; it != m_sidelinkRxPools.end () ; it++)


      // update uplink transmission mask according to previous UL-CQIs
      std::vector <int> rbMask = m_subChannelsForTransmissionQueue.at (0);
      SetSubChannelsForTransmission (m_subChannelsForTransmissionQueue.at (0));

      // shift the queue
      for (uint8_t i = 1; i < m_macChTtiDelay; i++)
        {
          m_subChannelsForTransmissionQueue.at (i - 1) = m_subChannelsForTransmissionQueue.at (i);
        }
      m_subChannelsForTransmissionQueue.at (m_macChTtiDelay - 1).clear ();

      if (m_srsConfigured && (m_srsStartTime <= Simulator::Now ()))
        {

          NS_ASSERT_MSG (subframeNo > 0 && subframeNo <= 10, "the SRS index check code assumes that subframeNo starts at 1");
          if ((((frameNo - 1) * 10 + (subframeNo - 1)) % m_srsPeriodicity) == m_srsSubframeOffset)
            {
              NS_LOG_INFO ("frame " << frameNo << " subframe " << subframeNo << " sending SRS (offset=" << m_srsSubframeOffset << ", period=" << m_srsPeriodicity << ")");
              m_sendSrsEvent = Simulator::Schedule (UL_SRS_DELAY_FROM_SUBFRAME_START,
                                                    &LteUePhy::SendSrs,
                                                    this);
            }
        }

      // retrieve the list control messages from UL control queue at LtePhy
      std::list<Ptr<LteControlMessage> > ctrlMsg = GetControlMessages ();
      // retrieve the current burst of packets from UL data queue at LtePhy
      Ptr<PacketBurst> pb = GetPacketBurst ();
      // retrieve burst information
      std::list<LteUePhySapProvider::TransmitSlPhySduParameters> params = GetSlPhyParameters (pb);

      if (rbMask.size () == 0)
        {
          //we do not have uplink data to send, make sure that if we are sending sidelink messages that 
          //there is no control messages also being sent (like DL_HARQ or CQI feedbacks)
          std::list<Ptr<LteControlMessage> >::iterator ctrlIt;
          for (ctrlIt = ctrlMsg.begin (); ctrlIt != ctrlMsg.end (); ctrlIt++)
            {
              NS_LOG_INFO ("Message type = " << (*ctrlIt)->GetMessageType ());
            }
          NS_ASSERT_MSG (!pb || ctrlMsg.size () == 0, "Trying to send sidelink along with UL control messages");
        } // end of if (rbMask.size () == 0)

      if (rbMask.size () != 0 || ctrlMsg.size () > 0)
        {
          // send packets in queue
          NS_LOG_LOGIC (this << " UE - start slot for PUSCH + PUCCH - RNTI " << m_rnti << " CELLID " << m_cellId);

          if (pb)
            {
              //sanity check if this is a Sidelink
              Ptr<Packet> packet = (*(pb->Begin ()))->Copy ();
              LteRadioBearerTag tag;
              packet->RemovePacketTag (tag);
              NS_ASSERT (tag.GetDestinationL2Id () == 0);

              if (m_enableUplinkPowerControl)
                {
                  m_txPower = m_powerControl->GetPuschTxPower (rbMask);
                  SetSubChannelsForTransmission (rbMask);
                }
              m_uplinkSpectrumPhy->StartTxDataFrame (pb, ctrlMsg, UL_DATA_DURATION);
            }
          else
            {
              // send only PUCCH (ideal: fake null bandwidth signal)
              if (ctrlMsg.size () > 0)
                {
                  NS_LOG_LOGIC (this << " UE - start TX PUCCH (NO PUSCH)");
                  std::vector <int> dlRb;

                  if (m_enableUplinkPowerControl)
                    {
                      m_txPower = m_powerControl->GetPucchTxPower (dlRb);
                    }
                  SetSubChannelsForTransmission (dlRb);
                  m_uplinkSpectrumPhy->StartTxDataFrame (pb, ctrlMsg, UL_DATA_DURATION);
                }
              else
                {
                  NS_LOG_LOGIC (this << " UE - UL NOTHING TO SEND");
                }
            }
        }
      else
        {
          //check Sidelink

          if (!m_waitingNextScPeriod)
            {
              //since we only have 1 Tx pool we can either send PSCCH or PSSCH but not both
              NS_LOG_LOGIC (this << " UE - start slot for PSSCH + PSCCH - RNTI " << m_rnti << " CELLID " << m_cellId);

              if (pb)
                {
                  std::list<LteUePhySapProvider::TransmitSlPhySduParameters>::iterator paramIt = params.begin ();
                  NS_ASSERT_MSG (paramIt != params.end (), "Found packet burst without associated parameters");
                  NS_ASSERT_MSG (ctrlMsg.size () == 0, "Should not have control messages while sending SL packet burst");

                  std::vector <int> slRb;
                  for (int i = (*paramIt).rbStart; i < (*paramIt).rbStart + (*paramIt).rbLen; i++)
                    {
                      NS_LOG_LOGIC (this << " Transmitting sidelink on RB " << i);
                      slRb.push_back (i);
                    }

                  switch ((*paramIt).channel)
                    {
                    case LteUePhySapProvider::TransmitSlPhySduParameters::PSBCH:
                      NS_LOG_LOGIC (this << " UE - start TX PSBCH");

                      NS_ASSERT_MSG (params.size () == 1, "Found more than one packet in PSBCH burst");

                      if (m_enableUplinkPowerControl)
                        {
                          m_txPower = m_powerControl->GetPscchTxPower (slRb); //what's the power control for PSBCH?
                        }

                      SetSubChannelsForTransmission (slRb);
                      m_uplinkSpectrumPhy->StartTxSlMibFrame (pb, UL_DATA_DURATION);
                      break;
                    case LteUePhySapProvider::TransmitSlPhySduParameters::PSCCH:
                      NS_LOG_LOGIC (this << " UE - start TX PSCCH");
                      //access the control message to store the PSSCH grant and be able to
                      //determine the subframes/RBs for PSSCH transmissions/ discovery

                      NS_ASSERT_MSG (params.size () == 1, "Found more than one packet in PSCCH burst");

                      if (m_enableUplinkPowerControl)
                        {
                          m_txPower = m_powerControl->GetPscchTxPower (slRb);
                        }

                      if (m_ueSlssScanningInProgress)
                        {
                          NS_LOG_LOGIC (this << "trying to do a PSCCH transmission while there is a scanning in progress... Ignoring transmission");
                        }
                      else if (m_ueSlssMeasurementsSched.find (Simulator::Now ().GetMilliSeconds ()) != m_ueSlssMeasurementsSched.end ())         //Measurement in this subframe
                        {
                          NS_LOG_LOGIC ("trying to do a PSCCH transmission while measuring S-RSRP in the same subframe... Ignoring transmission");
                        }
                      else
                        {
                          SetSubChannelsForTransmission (slRb);
                          m_uplinkSpectrumPhy->StartTxSlCtrlFrame (pb, UL_DATA_DURATION);
                        }

                      break;
                    case LteUePhySapProvider::TransmitSlPhySduParameters::PSSCH:
                      //NS_ASSERT (ctrlMsg.size () == 0); //(In the future we can have PSSCH and MIB-SL in the same subframe)
                      NS_LOG_LOGIC (this << " UE - start TX PSSCH");
                      NS_LOG_DEBUG (this << " TX Burst containing " << pb->GetNPackets () << " packets");

                      if (m_enableUplinkPowerControl)
                        {
                          m_txPower = m_powerControl->GetPsschTxPower (slRb);
                        }

                      if (m_ueSlssScanningInProgress)
                        {
                          NS_LOG_LOGIC (this << " trying to do a PSSCH transmission while there is a scanning in progress... Ignoring transmission");
                        }
                      else if (m_ueSlssMeasurementsSched.find (Simulator::Now ().GetMilliSeconds ()) != m_ueSlssMeasurementsSched.end ())
                        {
                          NS_LOG_LOGIC ("trying to do a PSSCH transmission while measuring S-RSRP in the same subframe... Ignoring transmission");
                        }
                      else
                        {
                          SetSubChannelsForTransmission (slRb);
                          m_uplinkSpectrumPhy->StartTxSlDataFrame (pb, UL_DATA_DURATION, (*paramIt).dstId);
                        }
                      break;
                    case LteUePhySapProvider::TransmitSlPhySduParameters::PSDCH:
                      NS_LOG_LOGIC (this << " UE - start Tx PSDCH");

                      if (m_enableUplinkPowerControl)
                        {
                          m_txPower = m_powerControl->GetPsdchTxPower (slRb);
                        }

                      SetSubChannelsForTransmission (slRb);

                      //0 added to pass by the group Id
                      //to be double checked
                      //
                      m_uplinkSpectrumPhy->StartTxSlDiscFrame (pb, (*paramIt).resNo, (*paramIt).rv, UL_DATA_DURATION);

                      break;
                    default:
                      NS_LOG_ERROR ("Unknown sidelink message type");
                    }
                }
              else
                {
                  NS_LOG_LOGIC (this << " UE - SL/UL NOTHING TO SEND");
                }
            }  //end if !m_waitingNextScPeriod
          else
            {
              NS_LOG_LOGIC (this << " the UE changed of timing and it is waiting for the start of a new SC period using the new timing... Delaying transmissions ");
            }
        }
    }  // end of if (m_ulConfigured)

  // trigger the MAC
  m_uePhySapUser->SubframeIndication (frameNo, subframeNo);

  m_subframeNo = subframeNo;
  ++subframeNo;
  if (subframeNo > 10)
    {
      ++frameNo;
      if (frameNo > 1024)
        {
          frameNo = 1;
        }
      subframeNo = 1;
    }

  // schedule next subframe indication
  Simulator::Schedule (Seconds (GetTti ()), &LteUePhy::SubframeIndication, this, frameNo, subframeNo);
}


void
LteUePhy::SendSrs ()
{
  NS_LOG_FUNCTION (this << " UE " << m_rnti << " start tx SRS, cell Id " << (uint32_t) m_cellId);
  NS_ASSERT (m_cellId > 0);
  // set the current tx power spectral density (full bandwidth)
  std::vector <int> dlRb;
  for (uint8_t i = 0; i < m_ulBandwidth; i++)
    {
      dlRb.push_back (i);
    }

  if (m_enableUplinkPowerControl)
    {
      m_txPower = m_powerControl->GetSrsTxPower (dlRb);
    }

  SetSubChannelsForTransmission (dlRb);
  m_uplinkSpectrumPhy->StartTxUlSrsFrame ();
}


void
LteUePhy::DoReset ()
{
  NS_LOG_FUNCTION (this);

  m_rnti = 0;
  m_cellId = 0;
  m_isConnected = false;
  m_transmissionMode = 0;
  m_srsPeriodicity = 0;
  m_srsConfigured = false;
  m_dlConfigured = false;
  m_ulConfigured = false;
  m_ulEarfcn = 0;
  m_ulBandwidth = 0;
  m_raPreambleId = 255; // value out of range
  m_raRnti = 11; // value out of range
  m_rsrpSinrSampleCounter = 0;
  m_p10CqiLast = Simulator::Now ();
  m_a30CqiLast = Simulator::Now ();
  m_paLinear = 1;

  m_rsReceivedPowerUpdated = false;
  m_rsInterferencePowerUpdated = false;
  m_dataInterferencePowerUpdated = false;

  m_packetBurstQueue.clear ();
  m_controlMessagesQueue.clear ();
  m_subChannelsForTransmissionQueue.clear ();
  m_packetParamsQueue.clear();
  for (int i = 0; i < m_macChTtiDelay; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      m_packetBurstQueue.push_back (pb);
      std::list<Ptr<LteControlMessage> > l;
      m_controlMessagesQueue.push_back (l);
      std::list < LteUePhySapProvider::TransmitSlPhySduParameters > l2;
      m_packetParamsQueue.push_back (l2);

    }
  std::vector <int> ulRb;
  m_subChannelsForTransmissionQueue.resize (m_macChTtiDelay, ulRb);
  m_sendSrsEvent.Cancel ();
  m_downlinkSpectrumPhy->Reset ();
  m_uplinkSpectrumPhy->Reset ();
  m_pssList.clear ();
  /**
   * Call the EndRx() method of the interference model for DL control and data
   * to cancel any ongoing downlink reception of control and data info.
   */
  m_downlinkSpectrumPhy->m_interferenceCtrl->EndRx ();
  m_downlinkSpectrumPhy->m_interferenceData->EndRx ();

} // end of void LteUePhy::DoReset ()

void
LteUePhy::DoStartCellSearch (uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << dlEarfcn);
  m_dlEarfcn = dlEarfcn;
  DoSetDlBandwidth (6); // configure DL for receiving PSS
  SwitchToState (CELL_SEARCH);
}

void
LteUePhy::DoSynchronizeWithEnb (uint16_t cellId, uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << cellId << dlEarfcn);
  m_dlEarfcn = dlEarfcn;
  DoSynchronizeWithEnb (cellId);
}

void
LteUePhy::DoSynchronizeWithEnb (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);

  if (cellId == 0)
    {
      NS_FATAL_ERROR ("Cell ID shall not be zero");
    }

  m_cellId = cellId;
  m_downlinkSpectrumPhy->SetCellId (cellId);
  m_uplinkSpectrumPhy->SetCellId (cellId);

  // configure DL for receiving the BCH with the minimum bandwidth
  DoSetDlBandwidth (6);

  m_dlConfigured = false;
  m_ulConfigured = false;

  SwitchToState (SYNCHRONIZED);
}

uint16_t
LteUePhy::DoGetCellId ()
{
  return m_cellId;
}

uint32_t
LteUePhy::DoGetDlEarfcn ()
{
  return m_dlEarfcn;
}

void
LteUePhy::DoSetDlBandwidth (uint16_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << (uint32_t) dlBandwidth);
  if (m_dlBandwidth != dlBandwidth or !m_dlConfigured)
    {
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

      m_noisePsd = LteSpectrumValueHelper::CreateNoisePowerSpectralDensity (m_dlEarfcn, m_dlBandwidth, m_noiseFigure);
      m_downlinkSpectrumPhy->SetNoisePowerSpectralDensity (m_noisePsd);
      m_downlinkSpectrumPhy->GetChannel ()->AddRx (m_downlinkSpectrumPhy);
    }
  m_dlConfigured = true;
}


void
LteUePhy::DoConfigureUplink (uint32_t ulEarfcn, uint16_t ulBandwidth)
{
  NS_LOG_FUNCTION (this << " Uplink : EARFCN = " << ulEarfcn
                        << " bandwidth = " << (uint32_t) ulBandwidth);

  if ((m_ulEarfcn != ulEarfcn) || (m_ulBandwidth != ulBandwidth) || (!m_ulConfigured))
    {
      m_ulEarfcn = ulEarfcn;
      m_ulBandwidth = ulBandwidth;
      m_ulConfigured = true;
      //configure Sidelink with UL
      if (m_sidelinkSpectrumPhy)
        {
          NS_LOG_DEBUG ("Adding Sidelink Spectrum phy to the channel");
          m_slNoisePsd = LteSpectrumValueHelper::CreateNoisePowerSpectralDensity (m_ulEarfcn, m_ulBandwidth, m_noiseFigure);
          m_sidelinkSpectrumPhy->SetNoisePowerSpectralDensity (m_slNoisePsd);
          m_sidelinkSpectrumPhy->GetChannel ()->AddRx (m_sidelinkSpectrumPhy);
        }
    }
}

void
LteUePhy::DoConfigureReferenceSignalPower (int8_t referenceSignalPower)
{
  NS_LOG_FUNCTION (this);
  m_powerControl->ConfigureReferenceSignalPower (referenceSignalPower);
}

void
LteUePhy::DoSetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  m_rnti = rnti;

  m_powerControl->SetCellId (m_cellId);
  m_powerControl->SetRnti (m_rnti);
}

void
LteUePhy::DoSetTransmissionMode (uint8_t txMode)
{
  NS_LOG_FUNCTION (this << (uint16_t)txMode);
  m_transmissionMode = txMode;
  m_downlinkSpectrumPhy->SetTransmissionMode (txMode);
}

void
LteUePhy::DoSetSrsConfigurationIndex (uint16_t srcCi)
{
  NS_LOG_FUNCTION (this << srcCi);
  m_srsPeriodicity = GetSrsPeriodicity (srcCi);
  m_srsSubframeOffset = GetSrsSubframeOffset (srcCi);
  m_srsConfigured = true;

  // a guard time is needed for the case where the SRS periodicity is changed dynamically at run time
  // if we use a static one, we can have a 0ms guard time
  m_srsStartTime = Simulator::Now () + MilliSeconds (0);
  NS_LOG_DEBUG (this << " UE SRS P " << m_srsPeriodicity << " RNTI " << m_rnti << " offset " << m_srsSubframeOffset << " cellId " << m_cellId << " CI " << srcCi);
}

void
LteUePhy::DoSetPa (double pa)
{
  NS_LOG_FUNCTION (this << pa);
  m_paLinear = pow (10,(pa / 10));
}

void
LteUePhy::DoSetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient)
{
  NS_LOG_FUNCTION (this << (uint16_t) (rsrpFilterCoefficient));
  m_powerControl->SetRsrpFilterCoefficient (rsrpFilterCoefficient);
}

void
LteUePhy::DoResetPhyAfterRlf ()
{
  NS_LOG_FUNCTION (this);
  m_downlinkSpectrumPhy->m_harqPhyModule->ClearDlHarqBuffer (m_rnti); //flush HARQ buffers
  m_dataInterferencePowerUpdated = false;
  m_rsInterferencePowerUpdated = false;
  m_pssReceived = false;
  DoReset ();
}

void
LteUePhy::DoResetRlfParams ()
{
  NS_LOG_FUNCTION (this);

  InitializeRlfParams ();
}

void
LteUePhy::DoStartInSnycDetection ()
{
  NS_LOG_FUNCTION (this);
  // indicates that the downlink radio link quality has to be monitored for in-sync indications
  m_downlinkInSync = false;
}

void
LteUePhy::DoSetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this);
  m_imsi = imsi;
}

void
LteUePhy::InitializeRlfParams ()
{
  NS_LOG_FUNCTION (this);
  m_numOfSubframes = 0;
  m_sinrDbFrame = 0;
  m_numOfFrames = 0;
  m_downlinkInSync = true;
}

void
LteUePhy::RlfDetection (double sinrDb)
{
  NS_LOG_FUNCTION (this << sinrDb);
  m_sinrDbFrame += sinrDb;
  m_numOfSubframes++;
  NS_LOG_LOGIC ("No of Subframes: " << m_numOfSubframes << " UE synchronized: " << m_downlinkInSync);
  //check for out_of_snyc indications first when UE is both DL and UL synchronized
  //m_downlinkInSync=true indicates that the evaluation is for out-of-sync indications
  if (m_downlinkInSync && m_numOfSubframes == 10)
    {
      /**
       * For every frame, if the downlink radio link quality(avg SINR)
       * is less than the threshold Qout, then the frame cannot be decoded
       */
      if ((m_sinrDbFrame / m_numOfSubframes) < m_qOut)
        {
          m_numOfFrames++; //increment the counter if a frame cannot be decoded
          NS_LOG_LOGIC ("No of Frames which cannot be decoded: " << m_numOfFrames);
        }
      else
        {
          /**
           * If the downlink radio link quality(avg SINR) is greater
           * than the threshold Qout, then the frame counter is reset
           * since only consecutive frames should be considered.
           */
          NS_LOG_INFO ("Resetting frame counter at phy. Current value = " << m_numOfFrames);
          m_numOfFrames = 0;
          // Also reset the sync indicator counter at RRC
          m_ueCphySapUser->ResetSyncIndicationCounter ();
        }
      m_numOfSubframes = 0;
      m_sinrDbFrame = 0;
    }
  /**
   * Once the number of consecutive frames which cannot be decoded equals
   * the Qout evaluation period (i.e 200ms), then an out-of-sync indication
   * is sent to the RRC layer
   */
  if (m_downlinkInSync && (m_numOfFrames * 10) == m_numOfQoutEvalSf)
    {
      NS_LOG_LOGIC ("At " << Simulator::Now ().As (Time::MS)
                          << " ms UE PHY sending out of snyc indication to UE RRC layer");
      m_ueCphySapUser->NotifyOutOfSync ();
      m_numOfFrames = 0;
    }
  //check for in_snyc indications when T310 timer is started
  //m_downlinkInSync=false indicates that the evaluation is for in-sync indications
  if (!m_downlinkInSync && m_numOfSubframes == 10)
    {
      /**
       * For every frame, if the downlink radio link quality(avg SINR)
       * is greater than the threshold Qin, then the frame can be
       * successfully decoded.
       */
      if ((m_sinrDbFrame / m_numOfSubframes) > m_qIn)
        {
          m_numOfFrames++; //increment the counter if a frame can be decoded
          NS_LOG_LOGIC ("No of Frames successfully decoded: " << m_numOfFrames);
        }
      else
        {
          /**
           * If the downlink radio link quality(avg SINR) is less
           * than the threshold Qin, then the frame counter is reset
           * since only consecutive frames should be considered
           */
          m_numOfFrames = 0;
          // Also reset the sync indicator counter at RRC
          m_ueCphySapUser->ResetSyncIndicationCounter ();
        }
      m_numOfSubframes = 0;
      m_sinrDbFrame = 0;
    }
  /**
   * Once the number of consecutive frames which can be decoded equals the Qin evaluation period (i.e 100ms),
   * then an in-sync indication is sent to the RRC layer
   */
  if (!m_downlinkInSync && (m_numOfFrames * 10) == m_numOfQinEvalSf)
    {
      NS_LOG_LOGIC ("At " << Simulator::Now ().As (Time::MS)
                          << " ms UE PHY sending in snyc indication to UE RRC layer");
      m_ueCphySapUser->NotifyInSync ();
      m_numOfFrames = 0;
    }
}


void
LteUePhy::SetTxMode1Gain (double gain)
{
  SetTxModeGain (1, gain);
}

void
LteUePhy::SetTxMode2Gain (double gain)
{
  SetTxModeGain (2, gain);
}

void
LteUePhy::SetTxMode3Gain (double gain)
{
  SetTxModeGain (3, gain);
}

void
LteUePhy::SetTxMode4Gain (double gain)
{
  SetTxModeGain (4, gain);
}

void
LteUePhy::SetTxMode5Gain (double gain)
{
  SetTxModeGain (5, gain);
}

void
LteUePhy::SetTxMode6Gain (double gain)
{
  SetTxModeGain (6, gain);
}

void
LteUePhy::SetTxMode7Gain (double gain)
{
  SetTxModeGain (7, gain);
}

void
LteUePhy::SetSlRxGain (double gain)
{
  NS_LOG_FUNCTION (this << gain);
  m_slRxGain = gain;
}

void
LteUePhy::SetTxModeGain (uint8_t txMode, double gain)
{
  NS_LOG_FUNCTION (this << gain);
  // convert to linear
  double gainLin = std::pow (10.0, (gain / 10.0));
  if (m_txModeGain.size () < txMode)
    {
      m_txModeGain.resize (txMode);
    }
  std::vector <double> temp;
  temp = m_txModeGain;
  m_txModeGain.clear ();
  for (uint8_t i = 0; i < temp.size (); i++)
    {
      if (i == txMode - 1)
        {
          m_txModeGain.push_back (gainLin);
        }
      else
        {
          m_txModeGain.push_back (temp.at (i));
        }
    }
  // forward the info to DL LteSpectrumPhy
  m_downlinkSpectrumPhy->SetTxModeGain (txMode, gain);
}

void
LteUePhy::EnqueueDlHarqFeedback (DlInfoListElement_s m)
{
  NS_LOG_FUNCTION (this);
  // get the feedback from LteSpectrumPhy and send it through ideal PUCCH to eNB
  Ptr<DlHarqFeedbackLteControlMessage> msg = Create<DlHarqFeedbackLteControlMessage> ();
  msg->SetDlHarqFeedback (m);
  SetControlMessages (msg);  
  //we do not notify MAC here because it was done when we processed the DL_DCI
}

void
LteUePhy::SetHarqPhyModule (Ptr<LteHarqPhy> harq)
{
  NS_LOG_FUNCTION (this);
  m_harqPhyModule = harq;
}


LteUePhy::State
LteUePhy::GetState () const
{
  NS_LOG_FUNCTION (this);
  return m_state;
}


void
LteUePhy::SwitchToState (State newState)
{
  NS_LOG_FUNCTION (this << newState);
  State oldState = m_state;
  m_state = newState;
  NS_LOG_INFO ("cellId=" << m_cellId << " rnti=" << m_rnti
                         << " UePhy " << ToString (oldState)
                         << " --> " << ToString (newState));
  m_stateTransitionTrace (m_cellId, m_rnti, oldState, newState);
}

void
LteUePhy::DoRemoveSlCommTxPool ()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Removing Sidelink Communication Tx pool");
  m_slTxPoolInfo.m_pool = nullptr;
}

void
LteUePhy::DoSetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  NS_LOG_FUNCTION (this);

  std::list<Ptr<SidelinkRxDiscResourcePool> >::iterator poolIt;
  for (poolIt = pools.begin (); poolIt != pools.end (); poolIt++)
    {
      bool found = false;
      std::list<DiscPoolInfo >::iterator currentPoolIt;
      for (currentPoolIt = m_discRxPools.begin (); currentPoolIt != m_discRxPools.end () && !found; currentPoolIt++)
        {
          if (*poolIt == currentPoolIt->m_pool)
            {
              found = true;
            }
        }
      if (!found)
        {
          DiscPoolInfo newpool;
          newpool.m_pool = *poolIt;
          newpool.m_npsdch = (*poolIt)->GetNPsdch ();
          newpool.m_currentGrants.clear ();
          newpool.m_nextDiscPeriod.frameNo = 0;
          newpool.m_nextDiscPeriod.subframeNo = 0;
          m_discRxPools.push_back (newpool);

          m_sidelinkSpectrumPhy->SetRxPool (newpool.m_pool);
          //This is needed to configure the number of retransmissions in LteSlHarqPhy
          //if a UE is only monitoring the discovery announcements.
          m_sidelinkSpectrumPhy->SetDiscNumRetx (newpool.m_pool->GetNumRetx ());
        }
    }
}

void
LteUePhy::InitializeDiscRxPool (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this);

  for (auto &it : m_discRxPools)
    {
      if (it.m_nextDiscPeriod.frameNo == 0)
        {
          //pool not initialized yet
          it.m_nextDiscPeriod = it.m_pool->GetNextDiscPeriod (frameNo - 1, subframeNo - 1);
          it.m_nextDiscPeriod.frameNo++;
          it.m_nextDiscPeriod.subframeNo++;
          NS_LOG_DEBUG ("Total discovery RX pool = " << m_discRxPools.size ());
          NS_LOG_DEBUG ("At " << Simulator::Now ().GetMilliSeconds ()
                              << " ms Discovery Rx Pool initialized"
                              << ". Next period at " << it.m_nextDiscPeriod.frameNo
                              << "/" << it.m_nextDiscPeriod.subframeNo);
        }
      //Check if this is a new discovery period
      if (frameNo == it.m_nextDiscPeriod.frameNo && subframeNo == it.m_nextDiscPeriod.subframeNo)
        {
          //clear the info from expextedTb and Tx count map of previous discovery period
          m_sidelinkSpectrumPhy->ClearExpectedDiscTb ();
          it.m_currentDiscPeriod = it.m_nextDiscPeriod;
          it.m_nextDiscPeriod = it.m_pool->GetNextDiscPeriod (frameNo - 1, subframeNo - 1);
          //adjust because scheduler starts with frame/subframe = 1
          it.m_nextDiscPeriod.frameNo++;
          it.m_nextDiscPeriod.subframeNo++;
          NS_LOG_DEBUG ("At " << Simulator::Now ().GetMilliSeconds ()
                              << " ms Starting new discovery period for RX pool"
                              << ". Next period at " << it.m_nextDiscPeriod.frameNo
                              << "/" << it.m_nextDiscPeriod.subframeNo);
        }
    }
}

void
LteUePhy::DoSetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool)
{
  NS_LOG_FUNCTION (this << pool );
  m_slTxPoolInfo.m_pool = pool;
  m_slTxPoolInfo.m_nextScPeriod.frameNo = 0; //init to 0 to make it invalid
  m_slTxPoolInfo.m_nextScPeriod.subframeNo = 0; //init to 0 to make it invalid
}

void
LteUePhy::DoSetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
  NS_LOG_FUNCTION (this);
  //update the pools that have changed
  std::list<Ptr<SidelinkRxCommResourcePool> >::iterator poolIt;
  for (poolIt = pools.begin (); poolIt != pools.end (); poolIt++)
    {
      bool found = false;
      std::list<RxPoolInfo >::iterator currentPoolIt;
      for (currentPoolIt = m_sidelinkRxPools.begin (); currentPoolIt != m_sidelinkRxPools.end () && !found; currentPoolIt++)
        {
          if (*poolIt == currentPoolIt->m_pool)
            {
              found = true;
            }
        }
      if (!found)
        {
          RxPoolInfo newpool;
          newpool.m_pool = *poolIt;
          newpool.m_currentGrants.clear ();
          m_sidelinkRxPools.push_back (newpool);
        }
    }
  //TODO: should remove the ones no longer needed.
  //Find a clean way to handle updates
  //m_sidelinkRxPools.clear ();

}

void
LteUePhy::DoAddSlDestination (uint32_t destination)
{
  std::list <uint32_t>::iterator it;
  for (it = m_destinations.begin (); it != m_destinations.end (); it++)
    {
      if ((*it) == destination)
        {
          break;
        }
    }
  if (it == m_destinations.end ())
    {
      //did not find it, so insert
      m_destinations.push_back (destination);

      if (m_sidelinkSpectrumPhy)
        {
          m_sidelinkSpectrumPhy->AddL1GroupId (destination);
        }
    }
}


void
LteUePhy::DoRemoveSlDestination (uint32_t destination)
{
  std::list <uint32_t>::iterator it = m_destinations.begin ();
  while (it != m_destinations.end ())
    {
      if ((*it) == destination)
        {
          m_destinations.erase (it);
          if (m_sidelinkSpectrumPhy)
            {
              m_sidelinkSpectrumPhy->RemoveL1GroupId (destination);
            }
          break; //leave the loop
        }
      it++;
    }
}

void
LteUePhy::SetFirstScanningTime (Time t)
{
  NS_LOG_FUNCTION (this);
  m_tFirstScanning = t;
  Simulator::Schedule (m_tFirstScanning,&LteUePhy::StartSlssScanning, this);
}

Time
LteUePhy::GetFirstScanningTime ()
{
  NS_LOG_FUNCTION (this);
  return m_tFirstScanning;
}

void
LteUePhy::ReceiveSlss (uint16_t slssid, Ptr<SpectrumValue> p)
{
  NS_LOG_FUNCTION (this << slssid);

  if (m_ueSlssScanningInProgress || m_ueSlssMeasurementInProgress)
    {
      NS_LOG_LOGIC ("The UE is currently performing the SyncRef scanning or S-RSRP measurement");

      //Measure instantaneous S-RSRP...
      double sum = 0.0;
      uint16_t nRB = 0;
      Values::const_iterator itPi;
      for (itPi = p->ConstValuesBegin (); itPi != p->ConstValuesEnd (); itPi++)
        {
          if ((*itPi))
            {
              double powerTxW = ((*itPi) * 180000.0) / 12.0; // convert PSD [W/Hz] to linear power [W]
              sum += powerTxW;
              nRB++;
            }
        }
      double s_rsrp_W = (sum / static_cast<double> (nRB));
      double s_rsrp_dBm = 10 * log10 (1000 * (s_rsrp_W));
      uint16_t offset = Simulator::Now ().GetMilliSeconds () % 40;

      NS_LOG_INFO ("UE RNTI " << m_rnti << " received SLSS from SyncRef with SLSSID " << slssid
                              << " offset " << offset << " with S-RSRP " << s_rsrp_dBm << " dBm" );

      //If it is not detectable, ignore
      if (s_rsrp_dBm < m_minSrsrp)
        {
          NS_LOG_LOGIC ("The S-RSRP is below the minimum required... Ignoring");
          return;
        }

      //Store the SLSS and S-RSRP
      //Note that a SyncRef is identified by SLSSID and reception offset.
      //SLSSs coming from different UEs, but having the same SyncRef info (same SLSSID and reception offset)
      //are considered as different S-RSRP samples of the same SyncRef
      std::map <std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator
        itMeasMap = m_ueSlssMeasurementsMap.find (std::pair<uint16_t, uint16_t> (slssid,offset));

      if (itMeasMap == m_ueSlssMeasurementsMap.end ()) //First entry
        {
          UeSlssMeasurementsElement newEl;
          newEl.srsrpSum = s_rsrp_W;
          newEl.srsrpNum = 1;

          if (m_ueSlssScanningInProgress)
            {
              NS_LOG_LOGIC ("SyncRef scan in progress, first detected entry");
              m_ueSlssDetectionMap.insert (std::pair< std::pair<uint16_t, uint16_t>,
                                                      UeSlssMeasurementsElement> (std::pair<uint16_t, uint16_t> (slssid,offset), newEl));
            }
          else if (m_ueSlssMeasurementInProgress)
            {
              NS_LOG_LOGIC ("S-RSRP measurement in progress, first measurement entry");
              //Insert new measurement only if it was already detected
              std::map <std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator
                itDetectionMap = m_ueSlssDetectionMap.find (std::pair<uint16_t, uint16_t> (slssid,offset));
              if (itDetectionMap != m_ueSlssDetectionMap.end ())
                {
                  NS_LOG_LOGIC ("SyncRef already detected, storing measurement");
                  m_ueSlssMeasurementsMap.insert (std::pair< std::pair<uint16_t, uint16_t>,
                                                             UeSlssMeasurementsElement> (std::pair<uint16_t, uint16_t> (slssid,offset), newEl));
                }
              else
                {
                  NS_LOG_LOGIC ("SyncRef was not detected during SyncRef search/scanning... Ignoring");
                }
            }
        }
      else
        {
          NS_LOG_LOGIC ("Measurement entry found... Adding values");

          (*itMeasMap).second.srsrpSum += s_rsrp_W;
          (*itMeasMap).second.srsrpNum++;
        }
    }
  else
    {
      NS_LOG_LOGIC (this << " The UE is not currently performing SyncRef scanning or S-RSRP measurement... Ignoring");
    }
}

void
LteUePhy::SetUeSlssInterScanningPeriodMax (Time t)
{
  NS_LOG_FUNCTION (this);
  m_nextScanRdm->SetAttribute ("Max",DoubleValue (t.GetMilliSeconds ()) );
}

void
LteUePhy::SetUeSlssInterScanningPeriodMin (Time t)
{
  NS_LOG_FUNCTION (this);
  m_nextScanRdm->SetAttribute ("Min",DoubleValue (t.GetMilliSeconds ()) );
}

void
LteUePhy::StartSlssScanning ()
{
  NS_LOG_FUNCTION (this);
  m_ueSlssScanningInProgress = true;
  m_detectedMibSl.clear ();
  Simulator::Schedule (m_ueSlssScanningPeriod, &LteUePhy::EndSlssScanning, this);
}

void
LteUePhy::EndSlssScanning ()
{
  NS_LOG_FUNCTION (this);
  m_ueSlssScanningInProgress = false;

  //Filter to keep only the SyncRefs with received MIB-SL
  std::map <std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator itDetectionMap;
  for (itDetectionMap = m_ueSlssDetectionMap.begin (); itDetectionMap != m_ueSlssDetectionMap.end (); itDetectionMap++)
    {
      NS_LOG_LOGIC ("UE RNTI " << m_rnti << " detected SyncRef with SLSSID " << itDetectionMap->first.first << " offset " << itDetectionMap->first.second << " S-RSRP " << itDetectionMap->second.srsrpSum / itDetectionMap->second.srsrpNum);
      std::map <std::pair<uint16_t, uint16_t>, LteRrcSap::MasterInformationBlockSL>::iterator itMap =
        m_detectedMibSl.find (std::pair<uint16_t, uint16_t> (itDetectionMap->first.first, itDetectionMap->first.second));
      //If the MIB-SL wasn't received, erase it from the detection map
      if (itMap == m_detectedMibSl.end ())
        {
          NS_LOG_LOGIC ("MIB-SL was not found... Removing from detection list");
          m_ueSlssDetectionMap.erase (itDetectionMap);
        }
    }

  //Select the 6 SyncRefs with higher S-RSRP. Remove the others form the detected list
  std::map <double, std::pair<uint16_t, uint16_t> > tmp;
  for (itDetectionMap = m_ueSlssDetectionMap.begin (); itDetectionMap != m_ueSlssDetectionMap.end (); itDetectionMap++)
    {
      tmp.insert (std::pair<double, std::pair<uint16_t, uint16_t> > (itDetectionMap->second.srsrpSum / itDetectionMap->second.srsrpNum, itDetectionMap->first));
    }
  while (tmp.size () > 6)
    {
      NS_LOG_LOGIC ("The UE detected more than 6 SyncRefs... Removing lowest S-RSRP SyncRef: SLSSID" << tmp.begin ()->second.first << "ofset " << tmp.begin ()->second.second << "S-RSRP " << tmp.begin ()->first);
      tmp.erase (tmp.begin ()->first);
    }
  std::map <std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement> ret;
  std::map <double, std::pair<uint16_t, uint16_t> >::iterator itTmp;
  for (itTmp = tmp.begin (); itTmp != tmp.end (); itTmp++)
    {
      std::map<std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator
        itDetectionMapTwo = m_ueSlssDetectionMap.find (itTmp->second);
      if (itDetectionMapTwo != m_ueSlssDetectionMap.end ())
        {
          ret.insert (std::pair<std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement> (itDetectionMapTwo->first, itDetectionMapTwo->second));
        }
    }

  m_ueSlssDetectionMap = ret; // It contains now only the 6 SyncRefs with higher S-RSRP
  m_ueSlssMeasurementsMap = ret; // It contains now only the 6 SyncRefs with higher S-RSRP (we use the S-RSRP measurements during scanning as first measurements)

  uint32_t nDetectedSyncRef = m_ueSlssDetectionMap.size ();

  if (nDetectedSyncRef > 0)
    {
      NS_LOG_LOGIC ("At least one SyncRef detected, creating measurement schedule and starting measurement sub-process");
      //Create measurement schedule
      std::map <std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator itMeasMap;
      for (itMeasMap = m_ueSlssMeasurementsMap.begin (); itMeasMap != m_ueSlssMeasurementsMap.end (); itMeasMap++)
        {
          uint16_t currOffset = Simulator::Now ().GetMilliSeconds () % 40;
          int64_t t;
          if ( currOffset < itMeasMap->first.second)
            {
              t = Simulator::Now ().GetMilliSeconds () + (itMeasMap->first.second - currOffset);
            }
          else
            {
              t = Simulator::Now ().GetMilliSeconds () + (40 - currOffset + itMeasMap->first.second);
            }
          uint16_t count = 1;
          while (t < (Simulator::Now ().GetMilliSeconds () + m_ueSlssMeasurementPeriod.GetMilliSeconds () - 40))
            {
              NS_LOG_INFO ("UE RNTI " << m_rnti << " will measure S-RSRP of SyncRef SLSSID " << itMeasMap->first.first << " offset " << itMeasMap->first.second << " at t:" << t << " ms");
              m_ueSlssMeasurementsSched.insert (std::pair<int64_t, std::pair<uint16_t, uint16_t> > (t,itMeasMap->first));
              count++;
              if (count > m_nSamplesSrsrpMeas)
                {
                  break;
                }
              t = t + 40;
            }
        }
      //Start measurement process of the 6 SyncRefs with higher S-RSRP
      StartSlssMeasurements (0,0);
    }
  else
    {
      NS_LOG_LOGIC ("No SyncRef detected... Ending SyncRef selection process");
      ScheduleNextSyncRefReselection (0); //The process ended after scanning
    }
}

void
LteUePhy::StartSlssMeasurements (uint64_t slssid, uint16_t offset)
{
  NS_LOG_FUNCTION (this);

  m_ueSlssMeasurementInProgress = true;
  Time t;
  if (slssid == 0) //Measurement
    {
      t = m_ueSlssMeasurementPeriod;
      NS_LOG_LOGIC ("Starting S-RSRP measurement corresponding to the measurement sub-process... Report happening in " << t);
    }
  else  //Evaluation
    {
      t = m_ueSlssEvaluationPeriod;
      NS_LOG_LOGIC ("Starting S-RSRP measurement corresponding to the evaluation sub-process... Report happening in " << t);
    }
  Simulator::Schedule (t, &LteUePhy::ReportSlssMeasurements, this, slssid, offset);
}

void
LteUePhy::ReportSlssMeasurements (uint64_t slssid, uint16_t offset)
{
  NS_LOG_FUNCTION (this);

  LteUeCphySapUser::UeSlssMeasurementsParameters ret;
  std::map<std::pair<uint16_t, uint16_t>, UeSlssMeasurementsElement>::iterator it;

  if (slssid == 0) //Report all
    {
      NS_LOG_LOGIC ("End of S-RSRP measurement corresponding to the measurement sub-process... Reporting L1 filtered S-RSRP values of detected SyncRefs");

      for (it = m_ueSlssMeasurementsMap.begin (); it != m_ueSlssMeasurementsMap.end (); it++)
        {
          //L1 filtering: linear average
          double avg_s_rsrp_W = (*it).second.srsrpSum / static_cast<double> ((*it).second.srsrpNum);
          //The stored values are in W, the report to the RRC should be in dBm
          double avg_s_rsrp_dBm = 10 * log10 (1000 * (avg_s_rsrp_W));

          NS_LOG_INFO ("UE RNTI " << m_rnti << " report SyncRef with SLSSID "
                                  << (*it).first.first << " offset " << (*it).first.second << " L1 filtered S-RSRP " << avg_s_rsrp_dBm
                                  << " from " << (double) (*it).second.srsrpNum << " samples");

          LteUeCphySapUser::UeSlssMeasurementsElement newEl;
          newEl.m_slssid = (*it).first.first;
          newEl.m_srsrp = avg_s_rsrp_dBm;
          newEl.m_offset = (*it).first.second;
          ret.m_ueSlssMeasurementsList.push_back (newEl);
        }

    }
  else // Report only of the selected SyncRef
    {
      NS_LOG_LOGIC ("End of S-RSRP measurement corresponding to the evaluation sub-process");
      NS_LOG_LOGIC ("Reporting L1 filtered S-RSRP values of the SyncRef SLSSID " << slssid << " offset " << offset);

      it = m_ueSlssMeasurementsMap.find (std::pair<uint16_t, uint16_t> (slssid, offset));
      if (it != m_ueSlssMeasurementsMap.end ())
        {
          //L1 filtering: linear average
          double avg_s_rsrp_W = (*it).second.srsrpSum / static_cast<double> ((*it).second.srsrpNum);
          //The stored values are in W, the report to the RRC should be in dBm
          double avg_s_rsrp_dBm = 10 * log10 (1000 * (avg_s_rsrp_W));

          NS_LOG_INFO (Simulator::Now ().GetMilliSeconds () << " UE RNTI " << m_rnti << " Report SyncRef with SLSSID "
                                                            << (*it).first.first << " offset " << (*it).first.second << " L1 filtered S-RSRP " << avg_s_rsrp_dBm
                                                            << " from " << (double) (*it).second.srsrpNum << " samples");

          LteUeCphySapUser::UeSlssMeasurementsElement newEl;
          newEl.m_slssid = (*it).first.first;
          newEl.m_srsrp = avg_s_rsrp_dBm;
          newEl.m_offset = (*it).first.second;
          ret.m_ueSlssMeasurementsList.push_back (newEl);
        }
    }

  //Report to RRC
  m_ueCphySapUser->ReportSlssMeasurements (ret,slssid,offset);

  //Cleaning for next process
  m_ueSlssMeasurementsMap.clear ();
  m_ueSlssMeasurementsSched.clear ();
  m_ueSlssMeasurementInProgress = false;

  //Schedule the start of the measurement period for evaluation of selected SyncRef if appropriated
  m_currNMeasPeriods++;
  if (m_currNMeasPeriods == 1 && m_resyncRequested)
    {
      NS_LOG_LOGIC ("The measurement sub-process ended and RRC selected a SyncRef for (re)synchronization");

      //Schedule the measurement for evaluation of the selected SyncRef for initiation/cease of SlSS transmission
      NS_LOG_INFO ("UE RNTI " << m_rnti << " will start evaluation of selected SyncRef with SLSSID " << m_resyncParams.slssid << " offset" << m_resyncParams.offset );
      Simulator::ScheduleNow (&LteUePhy::StartSlssMeasurements, this, m_resyncParams.slssid, m_resyncParams.offset);

      //Create measurement schedule for the evaluation
      uint16_t currOffset = Simulator::Now ().GetMilliSeconds () % 40;
      int64_t t;
      if ( currOffset < m_resyncParams.offset)
        {
          t = Simulator::Now ().GetMilliSeconds () + (m_resyncParams.offset - currOffset);
        }
      else
        {
          t = Simulator::Now ().GetMilliSeconds () + (40 - currOffset + m_resyncParams.offset);
        }
      uint16_t count = 1;
      while (t < (Simulator::Now ().GetMilliSeconds () + m_ueSlssMeasurementPeriod.GetMilliSeconds () - 40))
        {
          NS_LOG_INFO ("UE RNTI " << m_rnti << " will measure SyncRef with SLSSID" << m_resyncParams.slssid << " offset " <<  m_resyncParams.offset << " at t:" << t << " ms");
          m_ueSlssMeasurementsSched.insert (std::pair<int64_t, std::pair<uint16_t, uint16_t> > (t, std::pair<uint16_t, uint16_t> (m_resyncParams.slssid, m_resyncParams.offset)));
          count++;
          if (count > m_nSamplesSrsrpMeas)
            {
              break;
            }
          t = t + 40;
        }
    }
  else
    {
      //End of the selection+evaluation process, reinitialize variables for next process and schedule it
      m_ueSlssDetectionMap.clear ();

      if (m_currNMeasPeriods == 1)
        {
          NS_LOG_LOGIC ("The measurement sub-process ended and RRC did not selected a SyncRef... Ending SyncRef selection process");
          ScheduleNextSyncRefReselection (1); //The process ended after measurement
        }
      if (m_currNMeasPeriods == 2)
        {
          NS_LOG_LOGIC ("The evaluation sub-process ended... Ending SyncRef selection process");
          ScheduleNextSyncRefReselection (2); // The process ended after evaluation
        }
      m_currNMeasPeriods = 0;
    }
}

void
LteUePhy::ScheduleNextSyncRefReselection (uint16_t endOfPrevious)
{
  NS_LOG_FUNCTION (this);

  int32_t t_nextProcess = m_nextScanRdm->GetInteger ();

  NS_LOG_DEBUG ("t_nextProcess " << t_nextProcess << " m_ueSlssScanningPeriod.GetMilliSeconds "  << m_ueSlssScanningPeriod.GetMilliSeconds ());

  switch (endOfPrevious)
    {
    case 0:
      NS_LOG_LOGIC ("SyncRef selection process ended after scanning sub-process");
      t_nextProcess = t_nextProcess - m_ueSlssScanningPeriod.GetMilliSeconds ();
      break;
    case 1:
      NS_LOG_LOGIC ("SyncRef selection process ended after measurement sub-process");
      t_nextProcess = t_nextProcess - (m_ueSlssScanningPeriod.GetMilliSeconds () + m_ueSlssMeasurementPeriod.GetMilliSeconds ());
      break;
    case 2:
      NS_LOG_LOGIC ("SyncRef selection process ended after evaluation sub-process");
      t_nextProcess = t_nextProcess - (m_ueSlssScanningPeriod.GetMilliSeconds () + m_ueSlssMeasurementPeriod.GetMilliSeconds () + m_ueSlssEvaluationPeriod.GetMilliSeconds ());
      break;
    }

  //The standard requires at least one SyncRef selection process within 20s
  if (t_nextProcess > 20000)
    {
      NS_LOG_LOGIC ("Attempted to schedule the next SyncRef selection process for a period larger than 20 s... Scheduling it for 20 s");
      t_nextProcess = 20000;
    }

  //Do not travel to the past
  if (t_nextProcess <= 0)
    {
      NS_LOG_LOGIC ("Attempted to schedule the next SyncRef selection process for the past... Scheduling it for next subframe");
      t_nextProcess = 1;
    }
  NS_LOG_INFO ("UE RNTI " << m_rnti << " will start the next SyncRef selection process in t: " << t_nextProcess << " ms");
  Simulator::Schedule (MilliSeconds (t_nextProcess), &LteUePhy::StartSlssScanning, this);
}

bool
LteUePhy::ChangeOfTiming (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);

  if (m_slTxPoolInfo.m_pool)
    {
      NS_LOG_LOGIC ("The UE is currently transmitting Sidelink communication");

      //Is it the start of a new period?
      if (((frameNo == m_slTxPoolInfo.m_nextScPeriod.frameNo && subframeNo
            == m_slTxPoolInfo.m_nextScPeriod.subframeNo)
           || m_slTxPoolInfo.m_nextScPeriod.frameNo == 0))
        {
          NS_LOG_LOGIC ("The current subframe corresponds to the start of a new Sidelink communication period... Applying the change of timing");

          //Calculate the new subframe indication
          SidelinkCommResourcePool::SubframeInfo mibTime;
          NS_LOG_INFO ("mibTime frame/subframe = " << m_resyncParams.syncRefMib.directFrameNo << "/" << m_resyncParams.syncRefMib.directSubframeNo);
          mibTime.frameNo = m_resyncParams.syncRefMib.directFrameNo - 1;
          mibTime.subframeNo = m_resyncParams.syncRefMib.directSubframeNo - 1;

          SidelinkCommResourcePool::SubframeInfo currentTime;
          NS_LOG_INFO ("currentTime frame/subframe = " << frameNo << "/" << subframeNo);
          currentTime.frameNo = frameNo - 1;
          currentTime.subframeNo = subframeNo - 1;

          NS_LOG_INFO ("rxSubframe frame/subframe = " << m_resyncParams.rxSubframe.frameNo << "/" << m_resyncParams.rxSubframe.subframeNo);
          SidelinkCommResourcePool::SubframeInfo rxTime;
          rxTime.frameNo = m_resyncParams.rxSubframe.frameNo -1;
          rxTime.subframeNo = m_resyncParams.rxSubframe.subframeNo -1;

          m_resyncParams.newSubframe = mibTime + (currentTime - rxTime);

          m_resyncParams.newSubframe.frameNo++;
          m_resyncParams.newSubframe.subframeNo++;

          m_resyncRequested = false;
          NS_LOG_INFO ("UE RNTI " << m_rnti
                                  << " has a TxPool and changed the Subframe Indication"
                                  << " from " << frameNo << "/" << subframeNo
                                  << " to " << m_resyncParams.newSubframe.frameNo << "/" << m_resyncParams.newSubframe.subframeNo);

          //Updating values used bellow
          frameNo = m_resyncParams.newSubframe.frameNo;
          subframeNo = m_resyncParams.newSubframe.subframeNo;


          //Notify RRC about the successful change of SyncRef and timing
          m_ueCphySapUser->ReportChangeOfSyncRef (m_resyncParams);

          //Notify MAC about the successful change of SyncRef and timing. Some adjustments first

          //Adjusting MAC subframe indication:
          //There is a delay between the MAC scheduling and the PHY: the MAC is 4 subframes ahead
          uint32_t macSubframeNo = subframeNo;
          uint32_t macFrameNo = frameNo;
          if (macSubframeNo + UL_PUSCH_TTIS_DELAY > 10)
            {
              macFrameNo++;
              if (macFrameNo > 1024)
                {
                  macFrameNo = 1;
                }
              macSubframeNo = (macSubframeNo + UL_PUSCH_TTIS_DELAY) % 10;
            }
          else
            {
              macSubframeNo = macSubframeNo + UL_PUSCH_TTIS_DELAY;
            }
          //Adjusting the Sidelink communication parameters
          //We calculate the next period using the frameNo/subframeNo of the MAC.
          //Thus we avoid miss alignment due to the delay
          m_slTxPoolInfo.m_currentScPeriod
            = m_slTxPoolInfo.m_pool->GetCurrentScPeriod (macFrameNo,macSubframeNo);
          m_slTxPoolInfo.m_nextScPeriod
            = m_slTxPoolInfo.m_pool->GetNextScPeriod (
                m_slTxPoolInfo.m_currentScPeriod.frameNo,
                m_slTxPoolInfo.m_currentScPeriod.subframeNo);
          //adjust because scheduler starts with frame/subframe = 1
          m_slTxPoolInfo.m_nextScPeriod.frameNo++;
          m_slTxPoolInfo.m_nextScPeriod.subframeNo++;
          NS_LOG_INFO ("UE RNTI " << m_rnti << " Next Sidelink communication Tx period at frame/subframe: "
                                  << m_slTxPoolInfo.m_nextScPeriod.frameNo << "/"
                                  << m_slTxPoolInfo.m_nextScPeriod.subframeNo);

          //Don't try to send Sidelink communication until the start of the next period
          m_waitingNextScPeriod = true;

          //Finally, notify the MAC (Note the parameters are the PHY frameNo and subframeNo)
          m_uePhySapUser->NotifyChangeOfTiming (frameNo, subframeNo);

          //Store the new values
          m_currFrameNo = frameNo;
          m_currSubframeNo = subframeNo;

          //Notify the SpectrumPhy about the change of SLSSID
          m_uplinkSpectrumPhy->SetSlssid (m_resyncParams.slssid);
          m_sidelinkSpectrumPhy->SetSlssid (m_resyncParams.slssid);

          return true;
        }
      else
        { //Delay the change of Timing
          NS_LOG_LOGIC ("The current subframe does not correspond to the start of a new Sidelink communication period... Delaying the change of timing");
          return false;
        }
    }
  else
    {
      //No pool, apply directly the change of Timing
      NS_LOG_LOGIC ("The UE is not currently transmitting Sidelink communication... Applying the change of timing");

      //Calculate the new subframe indication
      SidelinkCommResourcePool::SubframeInfo mibTime;
      NS_LOG_INFO ("mibTime frame/subframe = " << m_resyncParams.syncRefMib.directFrameNo << "/" << m_resyncParams.syncRefMib.directSubframeNo);
      mibTime.frameNo = m_resyncParams.syncRefMib.directFrameNo - 1;
      mibTime.subframeNo = m_resyncParams.syncRefMib.directSubframeNo - 1;

      SidelinkCommResourcePool::SubframeInfo currentTime;
      NS_LOG_INFO ("currentTime frame/subframe = " << frameNo << "/" << subframeNo);
      currentTime.frameNo = frameNo - 1;
      currentTime.subframeNo = subframeNo - 1;

      SidelinkCommResourcePool::SubframeInfo rxTime;
      NS_LOG_INFO ("rxSubframe frame/subframe = " << m_resyncParams.rxSubframe.frameNo << "/" << m_resyncParams.rxSubframe.subframeNo);
      rxTime.frameNo = m_resyncParams.rxSubframe.frameNo -1;
      rxTime.subframeNo = m_resyncParams.rxSubframe.subframeNo -1;
      m_resyncParams.newSubframe = mibTime + (currentTime - rxTime);

      m_resyncParams.newSubframe.frameNo++;
      m_resyncParams.newSubframe.subframeNo++;

      m_resyncRequested = false;

      NS_LOG_INFO ("UE RNTI " << m_rnti
                              << " does not have a Tx pool and changed the Subframe Indication"
                              << " from " << frameNo << "/" << subframeNo
                              << " to " << m_resyncParams.newSubframe.frameNo << "/" << m_resyncParams.newSubframe.subframeNo);

      frameNo = m_resyncParams.newSubframe.frameNo;
      subframeNo = m_resyncParams.newSubframe.subframeNo;

      //Notify RRC about the successful change of SyncRef and timing
      m_ueCphySapUser->ReportChangeOfSyncRef (m_resyncParams);

      m_currFrameNo = m_resyncParams.newSubframe.frameNo;
      m_currSubframeNo = m_resyncParams.newSubframe.subframeNo;

      //Notify the SpectrumPhy about the change of SLSSID
      m_uplinkSpectrumPhy->SetSlssid (m_resyncParams.slssid);
      m_sidelinkSpectrumPhy->SetSlssid (m_resyncParams.slssid);

      return true;
    }
}

void
LteUePhy::DoSetSlssId (uint64_t slssid)
{
  NS_LOG_FUNCTION (this);
  m_uplinkSpectrumPhy->SetSlssid (slssid);
  m_sidelinkSpectrumPhy->SetSlssid (slssid);
}

void
LteUePhy::DoSynchronizeToSyncRef (LteSlSyncParams synchParams)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("Synchronizing to SyncRef SLSSSID " << synchParams.slssid << " offset " << synchParams.offset);
  // NS_LOG_INFO ("The estimated CURRENT subframe indication of the SyncRef (frame/subframe): " << synchParams.newFrameNo << "/" << synchParams.newSubframeNo);
  NS_LOG_INFO ("The CURRENT subframe indication of this UE (frame/subframe): " << m_currFrameNo << "/" << m_currSubframeNo);

  //Request the synchronization (change of timing) for the next subframe
  m_resyncRequested = true;
  m_resyncParams = synchParams;
}

int64_t
LteUePhy::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  m_nextScanRdm->SetStream (stream + 1);
  return 2;
}

void
LteUePhy::ReceivePsdchSdRsrp (Ptr<Packet> p, Ptr<SpectrumValue> psd, const std::vector<int> &rbs )
{
  NS_LOG_FUNCTION (this << m_rnti);

  LteSlDiscHeader discHeader;
  p->PeekHeader (discHeader);
  uint8_t msgType = discHeader.GetDiscoveryMsgType ();

  //We only monitor SD-RSRP for Relay Announcements (Discovery Model A)
  //or Relay Responses (Discovery Model B)
  if (msgType == LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT
      || msgType == LteSlDiscHeader::DISC_RELAY_RESPONSE)
    {
      //Get indexes
      uint64_t relayUeId = discHeader.GetRelayUeId ();
      uint32_t serviceCode = discHeader.GetRelayServiceCode ();

      //Measure SD-RSRP
      double sum = 0.0;
      uint16_t nRB = 0;
      for (uint16_t i = 0; i < rbs.size (); i++)
        {
          int rb = rbs[i];
          // convert PSD [W/Hz] to linear power [W] for the single RE
          double powerTxW = (psd->ValuesAt (rb) * 180000.0) / 12.0;
          sum += powerTxW;
          nRB++;
        }
      double sd_rsrp_W = (sum / static_cast<double> (nRB));
      double sd_rsrp_dBm = 10 * log10 (1000 * (sd_rsrp_W));

      NS_LOG_INFO (this << " UE RNTI " << m_rnti << " Rx relay discovery message from Relay UE Id " << relayUeId
                        << " Service Code " << serviceCode << " SD-RSRP (dBm) " << sd_rsrp_dBm << " nRB " << nRB);

      //Store SD-RSRP
      std::map <std::pair<uint64_t, uint32_t>, UeSdRsrpMeasurementsElement>::iterator
        itMeasMap = m_ueSdRsrpMeasurementsMap.find (std::pair<uint64_t, uint32_t> (relayUeId,serviceCode));
      if (itMeasMap == m_ueSdRsrpMeasurementsMap.end ())
        {
          NS_LOG_LOGIC (this << "First SD-RSRP measurement entry");
          UeSdRsrpMeasurementsElement newEl;
          newEl.sdRsrpSum = sd_rsrp_W;
          newEl.sdRsrpNum = 1;
          m_ueSdRsrpMeasurementsMap.insert (std::pair< std::pair<uint64_t, uint32_t>,
                                                       UeSdRsrpMeasurementsElement> (std::pair<uint64_t, uint32_t> (relayUeId,serviceCode), newEl));
        }
      else
        {
          NS_LOG_LOGIC (this << "SD-RSRP Measurement entry found... Adding values");
          (*itMeasMap).second.sdRsrpSum += sd_rsrp_W;
          (*itMeasMap).second.sdRsrpNum++;
        }
    }
}

Time
LteUePhy::GetUeSdRsrpMeasurementsFilterPeriod ()
{
  Time filterPeriod;
  int32_t discPeriod;

  // TODO: How to choose the pool to use if multiple are present?.
  // For the moment use the first one.
  if (m_discRxPools.size () > 0)
    {
      discPeriod = m_discRxPools.begin ()->m_pool->GetDiscPeriod ();
    }
  // Use the minimum discovery period lenght if no discovery rx pool is configured yet
  // This can happen when in-coverage due to the delay between the start of the
  // relay service and the provisioning of the discovery rx pool by the eNodeB.
  else
    {
      discPeriod = 320;
    }
  filterPeriod = MilliSeconds (4.0 * discPeriod);

  return filterPeriod;
}

void
LteUePhy::DoEnableUeSdRsrpMeasurements ()
{
  NS_LOG_FUNCTION (this);
  if (!m_ueSdRsrpMeasurementsEnabled)
    {
      Simulator::Schedule (GetUeSdRsrpMeasurementsFilterPeriod (), &LteUePhy::ReportUeSdRsrpMeasurements, this);
      m_ueSdRsrpMeasurementsEnabled = true;
    }
}

void
LteUePhy::DoDisableUeSdRsrpMeasurements ()
{
  NS_LOG_FUNCTION (this);
  if (m_ueSdRsrpMeasurementsEnabled)
    {
      m_ueSdRsrpMeasurementsEnabled = false;
    }
}

void
LteUePhy::ReportUeSdRsrpMeasurements ()
{
  NS_LOG_FUNCTION (this << m_rnti);
  if (m_ueSdRsrpMeasurementsEnabled)
    {
      //Perform the L1 filtering of all measured Relay UEs and service codes
      LteUeCphySapUser::UeSdRsrpMeasurementsParameters ret;
      std::map <std::pair<uint64_t, uint32_t>, UeSdRsrpMeasurementsElement>::iterator itMeasMap;
      for (itMeasMap = m_ueSdRsrpMeasurementsMap.begin (); itMeasMap != m_ueSdRsrpMeasurementsMap.end (); itMeasMap++)
        {
          //L1 filtering: linear average
          double avg_sd_rsrp_W = (*itMeasMap).second.sdRsrpSum / static_cast<double> ((*itMeasMap).second.sdRsrpNum);
          //The stored values are in W, the report to the RRC should be in dBm
          double avg_sd_rsrp_dBm = 10 * log10 (1000 * (avg_sd_rsrp_W));

          NS_LOG_INFO (this << " Relay UE Id " << (*itMeasMap).first.first
                            << "Service Code " << (*itMeasMap).first.second
                            << " averaged SD-RSRP (dBm) " << avg_sd_rsrp_dBm
                            << " number of measurements " << (*itMeasMap).second.sdRsrpNum );

          LteUeCphySapUser::UeSdRsrpMeasurementsElement newEl;
          newEl.m_relayId = (*itMeasMap).first.first;
          newEl.m_serviceCode = (*itMeasMap).first.second;
          newEl.m_sdRsrp = avg_sd_rsrp_dBm;
          ret.m_ueSdRsrpMeasurementsList.push_back (newEl);

          m_reportUeSdRsrpMeasurements (m_rnti, (*itMeasMap).first.first,
                                        (*itMeasMap).first.second, avg_sd_rsrp_dBm);
        }
      //Report to RRC
      m_ueCphySapUser->ReportUeSdRsrpMeasurements (ret);

      //Schedule next L1 filtering
      Simulator::Schedule (GetUeSdRsrpMeasurementsFilterPeriod (), &LteUePhy::ReportUeSdRsrpMeasurements, this);

      m_ueSdRsrpMeasurementsMap.clear ();
    }
}



} // namespace ns3
