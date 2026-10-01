// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2015 NYU WIRELESS, Tandon School of Engineering, New York University
// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#define NS_LOG_APPEND_CONTEXT                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << "] ";              \
    } while (false);

#include "nr-ue-phy.h"

#include "beam-manager.h"
#include "nr-ch-access-manager.h"
#include "nr-radio-bearer-tag.h"
#include "nr-ue-net-device.h"
#include "nr-ue-power-control.h"

#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>

#include <algorithm>
#include <cfloat>

namespace ns3
{

const Time NR_DEFAULT_PMI_INTERVAL_WB{MilliSeconds(10)}; // Wideband PMI update interval
const Time NR_DEFAULT_PMI_INTERVAL_SB{MilliSeconds(2)};  // Subband PMI update interval

NS_LOG_COMPONENT_DEFINE("NrUePhy");
NS_OBJECT_ENSURE_REGISTERED(NrUePhy);

NrUePhy::NrUePhy()
{
    NS_LOG_FUNCTION(this);
    m_wbCqiLast = Simulator::Now();
    m_ueCphySapProvider = new MemberNrUeCphySapProvider<NrUePhy>(this);
    m_powerControl = CreateObject<NrUePowerControl>(this);
    m_isConnected = false;
    Simulator::Schedule(m_ueMeasurementsFilterPeriod, &NrUePhy::ReportUeMeasurements, this);
}

NrUePhy::~NrUePhy()
{
    NS_LOG_FUNCTION(this);
}

void
NrUePhy::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_ueCphySapProvider;
    if (m_powerControl)
    {
        m_powerControl->Dispose();
        m_powerControl = nullptr;
    }
    if (m_cam)
    {
        m_cam->Dispose();
        m_cam = nullptr;
    }
    NrPhy::DoDispose();
}

TypeId
NrUePhy::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrUePhy")
            .SetParent<NrPhy>()
            .AddConstructor<NrUePhy>()
            .AddAttribute("TxPower",
                          "Transmission power in dBm",
                          DoubleValue(2.0),
                          MakeDoubleAccessor(&NrUePhy::m_txPower),
                          MakeDoubleChecker<double>())
            .AddAttribute(
                "NoiseFigure",
                "Loss (dB) in the Signal-to-Noise-Ratio due to non-idealities in the receiver."
                " According to Wikipedia (http://en.wikipedia.org/wiki/Noise_figure), this is "
                "\"the difference in decibels (dB) between"
                " the noise output of the actual receiver to the noise output of an "
                " ideal receiver with the same overall gain and bandwidth when the receivers "
                " are connected to sources at the standard noise temperature T0.\" "
                "In this model, we consider T0 = 290K.",
                DoubleValue(5.0), // nr code from NYU and UniPd assumed in the code the value of
                                  // 5dB, that is why we configure the default value to that
                MakeDoubleAccessor(&NrPhy::SetNoiseFigure, &NrPhy::GetNoiseFigure),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "PowerAllocationType",
                "Defines the type of the power allocation. Currently are supported "
                "two types: \"UniformPowerAllocBw\", which is a uniform power allocation over all "
                "bandwidth (over all RBs), and \"UniformPowerAllocBw\", which is a uniform "
                "power allocation over used (active) RBs. By default is set a uniform power "
                "allocation over used RBs .",
                EnumValue(NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_USED),
                MakeEnumAccessor<NrSpectrumValueHelper::PowerAllocationType>(
                    &NrPhy::SetPowerAllocationType,
                    &NrPhy::GetPowerAllocationType),
                MakeEnumChecker(NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_BW,
                                "UniformPowerAllocBw",
                                NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_USED,
                                "UniformPowerAllocUsed"))
            .AddAttribute("SpectrumPhy",
                          "The SpectrumPhy associated to this NrPhy",
                          TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&NrPhy::GetSpectrumPhy),
                          MakePointerChecker<NrSpectrumPhy>())
            .AddAttribute("LBTThresholdForCtrl",
                          "After a DL/UL transmission, if we have less than this value to send the "
                          "UL CTRL, we consider the channel as granted",
                          TimeValue(MicroSeconds(25)),
                          MakeTimeAccessor(&NrUePhy::m_lbtThresholdForCtrl),
                          MakeTimeChecker())
            .AddAttribute("TbDecodeLatency",
                          "Transport block decode latency",
                          TimeValue(MicroSeconds(100)),
                          MakeTimeAccessor(&NrPhy::SetTbDecodeLatency, &NrPhy::GetTbDecodeLatency),
                          MakeTimeChecker())
            .AddAttribute("EnableUplinkPowerControl",
                          "If true, Uplink Power Control will be enabled.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrUePhy::SetEnableUplinkPowerControl),
                          MakeBooleanChecker())
            .AddAttribute("WbPmiUpdateInterval",
                          "Wideband PMI update interval",
                          TimeValue(NR_DEFAULT_PMI_INTERVAL_WB),
                          MakeTimeAccessor(&NrUePhy::m_wbPmiUpdateInterval),
                          MakeTimeChecker())
            .AddAttribute("SbPmiUpdateInterval",
                          "Subband PMI update interval",
                          TimeValue(NR_DEFAULT_PMI_INTERVAL_SB),
                          MakeTimeAccessor(&NrUePhy::m_sbPmiUpdateInterval),
                          MakeTimeChecker())
            .AddAttribute("AlphaCovMat",
                          "The alpha parameter for the calculation of the interference covariance "
                          "matrix moving average",
                          DoubleValue(1),
                          MakeDoubleAccessor(&NrUePhy::SetAlphaCovMat, &NrUePhy::GetAlphaCovMat),
                          MakeDoubleChecker<double>(0.0, 1))
            .AddAttribute(
                "CsiImDuration",
                "CSI-IM duration in the number of OFDM symbols",
                UintegerValue(1),
                MakeUintegerAccessor(&NrUePhy::SetCsiImDuration, &NrUePhy::GetCsiImDuration),
                MakeUintegerChecker<uint8_t>(1, 12))
            .AddTraceSource("DlDataSinr",
                            "DL DATA SINR statistics.",
                            MakeTraceSourceAccessor(&NrUePhy::m_dlDataSinrTrace),
                            "ns3::NrUePhy::DlDataSinrTracedCallback")
            .AddTraceSource("DlCtrlSinr",
                            "Report the SINR computed for DL CTRL",
                            MakeTraceSourceAccessor(&NrUePhy::m_dlCtrlSinrTrace),
                            "ns3::NrUePhy::DlCtrlSinrTracedCallback")
            .AddAttribute("UeMeasurementsFilterPeriod",
                          "Time period for reporting UE measurements, i.e., the"
                          "length of layer-1 filtering.",
                          TimeValue(MilliSeconds(200)),
                          MakeTimeAccessor(&NrUePhy::m_ueMeasurementsFilterPeriod),
                          MakeTimeChecker())
            .AddTraceSource("ReportUplinkTbSize",
                            "Report allocated uplink TB size for trace.",
                            MakeTraceSourceAccessor(&NrUePhy::m_reportUlTbSize),
                            "ns3::UlTbSize::TracedCallback")
            .AddTraceSource("ReportDownlinkTbSize",
                            "Report allocated downlink TB size for trace.",
                            MakeTraceSourceAccessor(&NrUePhy::m_reportDlTbSize),
                            "ns3::DlTbSize::TracedCallback")
            .AddTraceSource("ReportRsrp",
                            "RSRP statistics.",
                            MakeTraceSourceAccessor(&NrUePhy::m_reportRsrpTrace),
                            "ns3::CurrentRsrp::TracedCallback")
            .AddTraceSource("UePhyRxedCtrlMsgsTrace",
                            "Ue PHY Control Messages Traces.",
                            MakeTraceSourceAccessor(&NrUePhy::m_phyRxedCtrlMsgsTrace),
                            "ns3::NrPhyRxTrace::RxedUePhyCtrlMsgsTracedCallback")
            .AddTraceSource("UePhyTxedCtrlMsgsTrace",
                            "Ue PHY Control Messages Traces.",
                            MakeTraceSourceAccessor(&NrUePhy::m_phyTxedCtrlMsgsTrace),
                            "ns3::NrPhyRxTrace::TxedUePhyCtrlMsgsTracedCallback")
            .AddTraceSource("UePhyRxedDlDciTrace",
                            "Ue PHY DL DCI Traces.",
                            MakeTraceSourceAccessor(&NrUePhy::m_phyUeRxedDlDciTrace),
                            "ns3::NrPhyRxTrace::RxedUePhyDlDciTracedCallback")
            .AddTraceSource("UePhyTxedHarqFeedbackTrace",
                            "Ue PHY DL HARQ Feedback Traces.",
                            MakeTraceSourceAccessor(&NrUePhy::m_phyUeTxedHarqFeedbackTrace),
                            "ns3::NrPhyRxTrace::TxedUePhyHarqFeedbackTracedCallback")
            .AddTraceSource("ReportPowerSpectralDensity",
                            "Power Spectral Density data.",
                            MakeTraceSourceAccessor(&NrUePhy::m_reportPowerSpectralDensity),
                            "ns3::NrUePhy::PowerSpectralDensityTracedCallback")
            .AddTraceSource("CqiFeedbackTrace",
                            "Mimo CQI feedback traces containing RNTI, WB CQI, MCS, and RI ",
                            MakeTraceSourceAccessor(&NrUePhy::m_cqiFeedbackTrace),
                            "ns3::NrUePhy::CqiFeedbackTracedCallback")
            .AddTraceSource("ReportUeMeasurements",
                            "Report UE measurements RSRP (dBm) and RSRQ (dB).",
                            MakeTraceSourceAccessor(&NrUePhy::m_reportUeMeasurements),
                            "ns3::NrUePhy::RsrpRsrqTracedCallback")
            .AddAttribute("EnableRlfDetection",
                          "If true, RLF detection will be enabled.",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NrUePhy::m_enableRlfDetection),
                          MakeBooleanChecker());
    return tid;
}

void
NrUePhy::ChannelAccessGranted([[maybe_unused]] const Time& time)
{
    NS_LOG_FUNCTION(this);
    // That will be granted only till the end of the slot
    m_channelStatus = GRANTED;
}

void
NrUePhy::ChannelAccessDenied()
{
    NS_LOG_FUNCTION(this);
    m_channelStatus = NONE;
}

void
NrUePhy::SetUeCphySapUser(NrUeCphySapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_ueCphySapUser = s;
}

NrUeCphySapProvider*
NrUePhy::GetUeCphySapProvider()
{
    NS_LOG_FUNCTION(this);
    return (m_ueCphySapProvider);
}

void
NrUePhy::SetEnableUplinkPowerControl(bool enable)
{
    m_enableUplinkPowerControl = enable;
}

void
NrUePhy::SetAlphaCovMat(double alpha)
{
    m_alphaCovMat = alpha;
}

double
NrUePhy::GetAlphaCovMat() const
{
    return m_alphaCovMat;
}

void
NrUePhy::SetCsiImDuration(uint8_t csiImDuration)
{
    m_csiImDuration = csiImDuration;
}

uint8_t
NrUePhy::GetCsiImDuration() const
{
    return m_csiImDuration;
}

void
NrUePhy::SetTxPower(double pow)
{
    m_txPower = pow;
    m_powerControl->SetTxPower(pow);
}

double
NrUePhy::GetTxPower() const
{
    return m_txPower;
}

double
NrUePhy::GetRsrp() const
{
    return m_rsrp;
}

Ptr<NrUePowerControl>
NrUePhy::GetUplinkPowerControl() const
{
    NS_LOG_FUNCTION(this);
    return m_powerControl;
}

void
NrUePhy::SetUplinkPowerControl(Ptr<NrUePowerControl> pc)
{
    m_powerControl = pc;
}

void
NrUePhy::SetDlAmc(const Ptr<const NrAmc>& amc)
{
    m_amc = amc;

    if (m_pmSearch)
    {
        m_pmSearch->SetAmc(amc);
    }
}

void
NrUePhy::SetSubChannelsForTransmission(const std::vector<int>& mask, uint32_t numSym)
{
    Ptr<SpectrumValue> txPsd = GetTxPowerSpectralDensity(mask);
    NS_ASSERT(txPsd);

    m_reportPowerSpectralDensity(m_currentSlot,
                                 txPsd,
                                 numSym * GetSymbolPeriod(),
                                 m_rnti,
                                 m_imsi,
                                 GetBwpId(),
                                 GetCellId());
    m_spectrumPhy->SetTxPowerSpectralDensity(txPsd);
}

void
NrUePhy::DoSendControlMessage(Ptr<NrControlMessage> msg)
{
    NS_LOG_FUNCTION(this << msg);
    EnqueueCtrlMessage(msg);
}

void
NrUePhy::DoSendControlMessageNow(Ptr<NrControlMessage> msg)
{
    NS_LOG_FUNCTION(this << msg);
    EnqueueCtrlMsgNow(msg);
}

void
NrUePhy::ProcessDataDci(const SfnSf& ulSfnSf,
                        const std::shared_ptr<DciInfoElementTdma>& dciInfoElem)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_DEBUG("UE" << m_rnti << " UL-DCI received for slot " << ulSfnSf << " symStart "
                      << static_cast<uint32_t>(dciInfoElem->m_symStart) << " numSym "
                      << static_cast<uint32_t>(dciInfoElem->m_numSym) << " tbs "
                      << dciInfoElem->m_tbSize << " harqId "
                      << static_cast<uint32_t>(dciInfoElem->m_harqProcess));

    if (ulSfnSf == m_currentSlot)
    {
        InsertAllocation(dciInfoElem);
    }
    else
    {
        InsertFutureAllocation(ulSfnSf, dciInfoElem);
    }
}

void
NrUePhy::SendRachPreamble(uint32_t PreambleId, uint32_t Rnti)
{
    NS_LOG_FUNCTION(this << PreambleId);
    m_raPreambleId = PreambleId;
    Ptr<NrRachPreambleMessage> msg = Create<NrRachPreambleMessage>();
    msg->SetSourceBwp(GetBwpId());
    msg->SetRapId(PreambleId);
    EnqueueCtrlMsgNow(msg);
}

void
NrUePhy::ProcessSrsDci(const SfnSf& ulSfnSf, const std::shared_ptr<DciInfoElementTdma>& dciInfoElem)
{
    NS_LOG_FUNCTION(this);
    // Instruct PHY for transmitting the SRS
    if (ulSfnSf == m_currentSlot)
    {
        InsertAllocation(dciInfoElem);
    }
    else
    {
        InsertFutureAllocation(ulSfnSf, dciInfoElem);
    }
}

void
NrUePhy::RegisterToGnb(uint16_t bwpId)
{
    NS_LOG_FUNCTION(this);

    InitializeMessageList();
    DoSetCellId(bwpId);
}

void
NrUePhy::SetUlCtrlSyms(uint8_t ulCtrlSyms)
{
    m_ulCtrlSyms = ulCtrlSyms;
}

void
NrUePhy::SetDlCtrlSyms(uint8_t dlCtrlSyms)
{
    m_dlCtrlSyms = dlCtrlSyms;
}

void
NrUePhy::SetNumRbPerRbg(uint32_t numRbPerRbg)
{
    m_numRbPerRbg = numRbPerRbg;
}

void
NrUePhy::SetPattern(const std::string& pattern)
{
    NS_LOG_FUNCTION(this);

    static std::unordered_map<std::string, LteNrTddSlotType> lookupTable = {
        {"DL", LteNrTddSlotType::DL},
        {"UL", LteNrTddSlotType::UL},
        {"S", LteNrTddSlotType::S},
        {"F", LteNrTddSlotType::F},
    };

    std::vector<LteNrTddSlotType> vector;
    std::stringstream ss(pattern);
    std::string token;
    std::vector<std::string> extracted;

    while (std::getline(ss, token, '|'))
    {
        extracted.push_back(token);
    }

    vector.reserve(extracted.size());
    for (const auto& v : extracted)
    {
        vector.push_back(lookupTable[v]);
    }

    m_tddPattern = vector;
    // Check if pure UL BWP
    const auto ulSlots = std::count(m_tddPattern.begin(), m_tddPattern.end(), LteNrTddSlotType::UL);
    if (static_cast<size_t>(ulSlots) == m_tddPattern.size())
    {
        // In case Downlink CSI feedback is enabled, disable it
        m_csiFeedbackType = 0;
    }
}

uint32_t
NrUePhy::GetNumRbPerRbg() const
{
    return m_numRbPerRbg;
}

void
NrUePhy::SetCurrentSfnSf(const SfnSf& currentSfnSf)
{
    m_currentSlot = currentSfnSf;
}

void
NrUePhy::SetLastSlotStart(Time startTime)
{
    m_lastSlotStart = startTime;
}

Time
NrUePhy::GetLastSlotStart() const
{
    return m_lastSlotStart;
}

NrUePhySapUser*
NrUePhy::GetPhySapUser() const
{
    return m_phySapUser;
}

double
NrUePhy::ComputeAvgSinr(const SpectrumValue& sinr)
{
    // averaged SINR among RBs
    double sum = 0.0;
    uint16_t rbNum = 0;
    Values::const_iterator it;

    for (it = sinr.ConstValuesBegin(); it != sinr.ConstValuesEnd(); it++)
    {
        sum += (*it);
        rbNum++;
    }

    double avrgSinr = (rbNum > 0) ? (sum / rbNum) : DBL_MAX;

    return avrgSinr;
}

void
NrUePhy::InsertAllocation(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    VarTtiAllocInfo varTtiInfo(dci);
    m_currSlotAllocInfo.m_varTtiAllocInfo.push_back(varTtiInfo);
    std::stable_sort(m_currSlotAllocInfo.m_varTtiAllocInfo.begin(),
                     m_currSlotAllocInfo.m_varTtiAllocInfo.end());
}

void
NrUePhy::InsertFutureAllocation(const SfnSf& sfnSf, const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    VarTtiAllocInfo varTtiInfo(dci);
    if (SlotAllocInfoExists(sfnSf))
    {
        auto& ulSlot = PeekSlotAllocInfo(sfnSf);
        ulSlot.m_varTtiAllocInfo.push_back(varTtiInfo);
        std::stable_sort(ulSlot.m_varTtiAllocInfo.begin(), ulSlot.m_varTtiAllocInfo.end());
    }
    else
    {
        SlotAllocInfo slotAllocInfo = SlotAllocInfo(sfnSf);
        slotAllocInfo.m_varTtiAllocInfo.push_back(varTtiInfo);
        PushBackSlotAllocInfo(slotAllocInfo);
    }
}

void
NrUePhy::PhyCtrlMessagesReceived(const Ptr<NrControlMessage>& msg)
{
    NS_LOG_FUNCTION(this);

    if (msg->GetMessageType() == NrControlMessage::DL_DCI)
    {
        auto dciMsg = DynamicCast<NrDlDciMessage>(msg);
        auto dciInfoElem = dciMsg->GetDciInfoElement();

        m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);

        if (dciInfoElem->m_rnti != 0 && dciInfoElem->m_rnti != m_rnti)
        {
            return; // DCI not for me
        }

        SfnSf dciSfn = m_currentSlot;
        uint32_t k0Delay = dciMsg->GetKDelay();
        dciSfn.Add(k0Delay);

        NS_LOG_DEBUG("UE" << m_rnti << " DL-DCI received for slot " << dciSfn << " symStart "
                          << static_cast<uint32_t>(dciInfoElem->m_symStart) << " numSym "
                          << static_cast<uint32_t>(dciInfoElem->m_numSym) << " tbs "
                          << dciInfoElem->m_tbSize << " harqId "
                          << static_cast<uint32_t>(dciInfoElem->m_harqProcess));

        /* BIG ASSUMPTION: We assume that K0 is always 0 */

        auto it = m_harqIdToK1Map.find(dciInfoElem->m_harqProcess);
        if (it != m_harqIdToK1Map.end())
        {
            m_harqIdToK1Map.erase(m_harqIdToK1Map.find(dciInfoElem->m_harqProcess));
        }

        m_harqIdToK1Map.insert(std::make_pair(dciInfoElem->m_harqProcess, dciMsg->GetK1Delay()));

        m_phyUeRxedDlDciTrace(m_currentSlot,
                              GetCellId(),
                              m_rnti,
                              GetBwpId(),
                              dciInfoElem->m_harqProcess,
                              dciMsg->GetK1Delay());

        InsertAllocation(dciInfoElem);

        m_phySapUser->ReceiveControlMessage(msg);

        if (m_enableUplinkPowerControl)
        {
            m_powerControl->ReportTpcPusch(dciInfoElem->m_tpc);
            m_powerControl->ReportTpcPucch(dciInfoElem->m_tpc);
        }
    }
    else if (msg->GetMessageType() == NrControlMessage::UL_DCI)
    {
        auto dciMsg = DynamicCast<NrUlDciMessage>(msg);
        auto dciInfoElem = dciMsg->GetDciInfoElement();

        m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);

        if (dciInfoElem->m_rnti != 0 && dciInfoElem->m_rnti != m_rnti)
        {
            return; // DCI not for me
        }

        SfnSf ulSfnSf = m_currentSlot;
        uint32_t k2Delay = dciMsg->GetKDelay();
        ulSfnSf.Add(k2Delay);

        if (dciInfoElem->m_type == DciInfoElementTdma::DATA)
        {
            ProcessDataDci(ulSfnSf, dciInfoElem);
            m_phySapUser->ReceiveControlMessage(msg);
        }
        else if (dciInfoElem->m_type == DciInfoElementTdma::SRS)
        {
            ProcessSrsDci(ulSfnSf, dciInfoElem);
            // Do not pass the DCI to MAC
        }
    }
    else if (msg->GetMessageType() == NrControlMessage::MIB)
    {
        NS_LOG_DEBUG("received MIB");
        Ptr<NrMibMessage> msg2 = DynamicCast<NrMibMessage>(msg);
        m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);
        m_ueCphySapUser->RecvMasterInformationBlock(GetCellId(), msg2->GetMib());
    }
    else if (msg->GetMessageType() == NrControlMessage::SIB1)
    {
        Ptr<NrSib1Message> msg2 = DynamicCast<NrSib1Message>(msg);
        m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);
        m_ueCphySapUser->RecvSystemInformationBlockType1(GetCellId(), msg2->GetSib1());
    }
    else if (msg->GetMessageType() == NrControlMessage::RAR)
    {
        Ptr<NrRarMessage> rarMsg = DynamicCast<NrRarMessage>(msg);

        ProcessRar(rarMsg);
    }
    else
    {
        NS_LOG_INFO("Message type not recognized " << msg->GetMessageType());
        m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);
        m_phySapUser->ReceiveControlMessage(msg);
    }
}

void
NrUePhy::ProcessRar(const Ptr<NrRarMessage>& rarMsg)
{
    NS_LOG_FUNCTION(this);
    bool myRar = false;
    {
        for (auto it = rarMsg->RarListBegin(); it != rarMsg->RarListEnd(); ++it)
        {
            NS_LOG_INFO("Received RAR in slot" << m_currentSlot << " with RA preamble ID: "
                                               << std::to_string(it->rarPayload.raPreambleId));
            if (it->rarPayload.raPreambleId == m_raPreambleId)
            {
                NS_LOG_INFO("Received RAR with RA preamble ID:" << +it->rarPayload.raPreambleId
                                                                << " current RA preamble ID is :"
                                                                << m_raPreambleId);
                // insert allocation
                SfnSf ulSfnSf = m_currentSlot;
                uint32_t k2Delay = it->rarPayload.k2Delay;
                ulSfnSf.Add(k2Delay);
                NS_LOG_DEBUG("Insert RAR UL DCI allocation for " << ulSfnSf);
                ProcessDataDci(ulSfnSf, (*it).rarPayload.ulMsg3Dci);
                myRar = true;
                // notify MAC and above about transmission opportunity
                m_phySapUser->ReceiveControlMessage(rarMsg);
                // fire CTRL msg trace
                m_phyRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), rarMsg);
                // reset RACH variables with out of range values
                m_raPreambleId = 255;
            }
        }
        if (!myRar)
        {
            NS_LOG_DEBUG("Skipping RAR, does not contain preamble ID."
                         << "\n My preamble id: " << std::to_string(m_raPreambleId) << " found:");
            for (auto it = rarMsg->RarListBegin(); it != rarMsg->RarListEnd(); ++it)
            {
                NS_LOG_DEBUG("rapId: " << std::to_string(it->rapId));
            }
        }
    }
}

void
NrUePhy::TryToPerformLbt()
{
    NS_LOG_FUNCTION(this);
    uint8_t ulCtrlSymStart = 0;
    uint8_t ulCtrlNumSym = 0;

    for (const auto& alloc : m_currSlotAllocInfo.m_varTtiAllocInfo)
    {
        if (alloc.m_dci->m_type == DciInfoElementTdma::CTRL &&
            alloc.m_dci->m_format == DciInfoElementTdma::UL)
        {
            ulCtrlSymStart = alloc.m_dci->m_symStart;
            ulCtrlNumSym = alloc.m_dci->m_numSym;
            break;
        }
    }

    if (ulCtrlNumSym != 0)
    {
        // We have an UL CTRL symbol scheduled and we have to transmit CTRLs..
        // .. so we check that we have at least 25 us between the latest DCI,
        // or we have to schedule an LBT event.

        Time limit = m_lastSlotStart + GetSlotPeriod() -
                     ((GetSymbolsPerSlot() - ulCtrlSymStart) * GetSymbolPeriod()) -
                     m_lbtThresholdForCtrl;

        for (const auto& alloc : m_currSlotAllocInfo.m_varTtiAllocInfo)
        {
            int64_t symbolPeriod = GetSymbolPeriod().GetMicroSeconds();
            int64_t dciEndsAt = m_lastSlotStart.GetMicroSeconds() +
                                ((alloc.m_dci->m_numSym + alloc.m_dci->m_symStart) * symbolPeriod);

            if (alloc.m_dci->m_type != DciInfoElementTdma::DATA &&
                alloc.m_dci->m_type != DciInfoElementTdma::MSG3)
            {
                continue;
            }

            if (limit.GetMicroSeconds() < dciEndsAt)
            {
                NS_LOG_INFO("This data DCI ends at "
                            << MicroSeconds(dciEndsAt)
                            << " which is inside the LBT shared COT (the limit is " << limit
                            << "). No need for LBT");
                m_lbtEvent.Cancel(); // Forget any LBT we previously set, because of the new
                // DCI information
                m_channelStatus = GRANTED;
            }
            else
            {
                NS_LOG_INFO("This data DCI starts at "
                            << +alloc.m_dci->m_symStart << " for " << +alloc.m_dci->m_numSym
                            << " ends at " << MicroSeconds(dciEndsAt)
                            << " which is outside the LBT shared COT (the limit is " << limit
                            << ").");
            }
        }
        if (m_channelStatus != GRANTED)
        {
            Time sched = m_lastSlotStart - Simulator::Now() + (GetSymbolPeriod() * ulCtrlSymStart) -
                         MicroSeconds(25);
            NS_LOG_DEBUG("Scheduling an LBT for sending the UL CTRL at "
                         << Simulator::Now() + sched);
            m_lbtEvent.Cancel();
            m_lbtEvent = Simulator::Schedule(sched, &NrUePhy::RequestAccess, this);
        }
        else
        {
            NS_LOG_DEBUG("Not scheduling LBT: the UE has a channel status that is GRANTED");
        }
    }
    else
    {
        NS_LOG_DEBUG("Not scheduling LBT; the UE has no UL CTRL symbols available");
    }
}

void
NrUePhy::RequestAccess()
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Request access because we have to transmit UL CTRL");
    m_cam->RequestAccess(); // This will put the m_channelStatus to granted when
                            // the channel will be granted.
}

void
NrUePhy::PushCtrlAllocations(const SfnSf currentSfnSf)
{
    NS_LOG_FUNCTION(this);

    // The UE does not know anything from the GNB yet, so listen on the default
    // bandwidth.
    std::vector<bool> rbgBitmask(GetRbNum(), true);

    // The UE still doesn't know the TDD pattern, so just add a DL CTRL
    if (m_tddPattern.empty())
    {
        NS_LOG_INFO("TDD Pattern unknown, insert DL CTRL at the beginning of the slot");
        VarTtiAllocInfo dlCtrlSlot(std::make_shared<DciInfoElementTdma>(0,
                                                                        m_dlCtrlSyms,
                                                                        DciInfoElementTdma::DL,
                                                                        DciInfoElementTdma::CTRL,
                                                                        rbgBitmask));
        m_currSlotAllocInfo.m_varTtiAllocInfo.push_front(dlCtrlSlot);
        return;
    }

    uint64_t currentSlotN = currentSfnSf.Normalize() % m_tddPattern.size();

    if (m_tddPattern[currentSlotN] < LteNrTddSlotType::UL)
    {
        NS_LOG_DEBUG("The current TDD pattern indicates that we are in a "
                     << m_tddPattern[currentSlotN]
                     << " slot, so insert DL CTRL at the beginning of the slot");
        VarTtiAllocInfo dlCtrlSlot(std::make_shared<DciInfoElementTdma>(0,
                                                                        m_dlCtrlSyms,
                                                                        DciInfoElementTdma::DL,
                                                                        DciInfoElementTdma::CTRL,
                                                                        rbgBitmask));
        m_currSlotAllocInfo.m_varTtiAllocInfo.push_front(dlCtrlSlot);
    }
    if (m_tddPattern[currentSlotN] > LteNrTddSlotType::DL)
    {
        NS_LOG_DEBUG("The current TDD pattern indicates that we are in a "
                     << m_tddPattern[currentSlotN]
                     << " slot, so insert UL CTRL at the end of the slot");
        VarTtiAllocInfo ulCtrlSlot(
            std::make_shared<DciInfoElementTdma>(GetSymbolsPerSlot() - m_ulCtrlSyms,
                                                 m_ulCtrlSyms,
                                                 DciInfoElementTdma::UL,
                                                 DciInfoElementTdma::CTRL,
                                                 rbgBitmask));
        m_currSlotAllocInfo.m_varTtiAllocInfo.push_back(ulCtrlSlot);
    }
}

void
NrUePhy::StartSlot(const SfnSf& s)
{
    NS_LOG_FUNCTION(this);
    m_currentSlot = s;
    m_lastSlotStart = Simulator::Now();

    // Call MAC before doing anything in PHY
    m_phySapUser->SlotIndication(m_currentSlot); // trigger mac

    // update the current slot object, and insert DL/UL CTRL allocations depending on the TDD
    // pattern
    bool nrAllocationExists = SlotAllocInfoExists(m_currentSlot);
    FinishSlotProcessing(s, nrAllocationExists);
}

void
NrUePhy::FinishSlotProcessing(const SfnSf& s, bool nrAllocationExists)
{
    NS_LOG_FUNCTION(this << s);
    if (nrAllocationExists)
    {
        m_currSlotAllocInfo = RetrieveSlotAllocInfo(m_currentSlot);
    }
    else
    {
        m_currSlotAllocInfo = SlotAllocInfo(m_currentSlot);
    }

    PushCtrlAllocations(m_currentSlot);
    NS_ASSERT(m_currSlotAllocInfo.m_sfnSf == m_currentSlot);

    NS_LOG_DEBUG("UE " << m_rnti << " start slot " << m_currSlotAllocInfo.m_sfnSf
                       << " composed by the following allocations, total "
                       << m_currSlotAllocInfo.m_varTtiAllocInfo.size());
    for (const auto& alloc : m_currSlotAllocInfo.m_varTtiAllocInfo)
    {
        std::string direction;
        std::string type;

        if (alloc.m_dci->m_format == DciInfoElementTdma::UL)
        {
            direction = "UL";
        }
        else
        {
            direction = "DL";
        }

        switch (alloc.m_dci->m_type)
        {
        case DciInfoElementTdma::VarTtiType::SRS:
            type = "SRS";
            NS_LOG_DEBUG("Allocation from sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_symStart) << " to sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_numSym + alloc.m_dci->m_symStart)
                         << " direction " << direction << " type " << type);
            break;
        case DciInfoElementTdma::VarTtiType::DATA:
            type = "DATA";
            NS_LOG_INFO("Allocation from sym "
                        << static_cast<uint32_t>(alloc.m_dci->m_symStart) << " to sym "
                        << static_cast<uint32_t>(alloc.m_dci->m_numSym + alloc.m_dci->m_symStart)
                        << " direction " << direction << " type " << type);
            break;
        case DciInfoElementTdma::VarTtiType::CTRL:
            type = "CTRL";
            NS_LOG_DEBUG("Allocation from sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_symStart) << " to sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_numSym + alloc.m_dci->m_symStart)
                         << " direction " << direction << " type " << type);
            break;
        case DciInfoElementTdma::VarTtiType::MSG3:
            type = "MSG3";
            NS_LOG_DEBUG("Allocation from sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_symStart) << " to sym "
                         << static_cast<uint32_t>(alloc.m_dci->m_numSym + alloc.m_dci->m_symStart)
                         << " direction " << direction << " type " << type);
            break;
        default:
            NS_LOG_ERROR("Unknown type DciInfoElementTdma::VarTtiType " << alloc.m_dci->m_type);
        }
    }

    TryToPerformLbt();

    VarTtiAllocInfo allocation = m_currSlotAllocInfo.m_varTtiAllocInfo.front();
    m_currSlotAllocInfo.m_varTtiAllocInfo.pop_front();

    auto nextVarTtiStart = GetSymbolPeriod() * allocation.m_dci->m_symStart;

    auto ctrlMsgs = PopCurrentSlotCtrlMsgs();
    if (m_netDevice)
    {
        DynamicCast<NrUeNetDevice>(m_netDevice)->RouteOutgoingCtrlMsgs(ctrlMsgs, GetBwpId());
    }
    else
    {
        // No netDevice (that could happen in tests) so just redirect them to us
        for (const auto& msg : ctrlMsgs)
        {
            EncodeCtrlMsg(msg);
        }
    }

    Simulator::Schedule(nextVarTtiStart, &NrUePhy::StartVarTti, this, allocation.m_dci);
}

Time
NrUePhy::DlCtrl(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    Time varTtiDuration = GetSymbolPeriod() * dci->m_numSym;

    NS_LOG_DEBUG("UE" << m_rnti
                      << " RXing DL CTRL frame for"
                         " symbols "
                      << +dci->m_symStart << "-" << +(dci->m_symStart + dci->m_numSym - 1)
                      << "\t start " << Simulator::Now() << " end "
                      << (Simulator::Now() + varTtiDuration));

    m_tryToPerformLbt = true;

    m_spectrumPhy->AddExpectedDlCtrlEnd(Simulator::Now() + varTtiDuration);

    return varTtiDuration;
}

Time
NrUePhy::UlSrs(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    std::vector<int> channelRbs;
    for (uint32_t i = 0; i < GetRbNum(); i++)
    {
        channelRbs.push_back(static_cast<int>(i));
    }
    SetSubChannelsForTransmission(channelRbs, dci->m_numSym);

    std::list<Ptr<NrControlMessage>> srsMsg;
    Ptr<NrSrsMessage> srs = Create<NrSrsMessage>();
    srs->SetSourceBwp(GetBwpId());
    srsMsg.emplace_back(srs);
    Time varTtiDuration = GetSymbolPeriod() * dci->m_numSym;

    m_phyTxedCtrlMsgsTrace(m_currentSlot, GetCellId(), dci->m_rnti, GetBwpId(), *srsMsg.begin());
    m_spectrumPhy->StartTxUlControlFrames(srsMsg, varTtiDuration - NanoSeconds(1.0));

    NS_LOG_DEBUG("UE" << m_rnti << " TXing UL SRS frame for symbols " << +dci->m_symStart << "-"
                      << +(dci->m_symStart + dci->m_numSym - 1) << "\t start " << Simulator::Now()
                      << " end " << (Simulator::Now() + varTtiDuration - NanoSeconds(1.0)));

    ChannelAccessDenied(); // Reset the channel status
    return varTtiDuration;
}

Time
NrUePhy::UlCtrl(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    Time varTtiDuration = GetSymbolPeriod() * dci->m_numSym;

    if (m_ctrlMsgs.empty())
    {
        NS_LOG_DEBUG("UE" << m_rnti << " reserved space for UL CTRL frame for symbols "
                          << +dci->m_symStart << "-" << +(dci->m_symStart + dci->m_numSym - 1)
                          << "\t start " << Simulator::Now() << " end "
                          << (Simulator::Now() + varTtiDuration - NanoSeconds(1.0))
                          << " but no data to transmit");
        m_cam->Cancel();
        return varTtiDuration;
    }
    else if (m_channelStatus != GRANTED)
    {
        NS_LOG_INFO("UE" << m_rnti << " has to transmit CTRL but channel not granted");
        m_cam->Cancel();
        return varTtiDuration;
    }

    for (const auto& msg : m_ctrlMsgs)
    {
        m_phyTxedCtrlMsgsTrace(m_currentSlot, GetCellId(), dci->m_rnti, GetBwpId(), msg);

        if (msg->GetMessageType() == NrControlMessage::DL_HARQ)
        {
            Ptr<NrDlHarqFeedbackMessage> harqMsg = DynamicCast<NrDlHarqFeedbackMessage>(msg);
            uint8_t harqId = harqMsg->GetDlHarqFeedback().m_harqProcessId;

            auto it = m_harqIdToK1Map.find(harqId);
            if (it != m_harqIdToK1Map.end())
            {
                m_phyUeTxedHarqFeedbackTrace(m_currentSlot,
                                             GetCellId(),
                                             m_rnti,
                                             GetBwpId(),
                                             static_cast<uint32_t>(harqId),
                                             it->second);
            }
        }
    }

    std::vector<int> channelRbs;
    for (uint32_t i = 0; i < GetRbNum(); i++)
    {
        channelRbs.push_back(static_cast<int>(i));
    }

    if (m_enableUplinkPowerControl)
    {
        m_txPower = m_powerControl->GetPucchTxPower(channelRbs.size());
    }
    SetSubChannelsForTransmission(channelRbs, dci->m_numSym);

    NS_LOG_DEBUG("UE" << m_rnti << " TXing UL CTRL frame for symbols " << +dci->m_symStart << "-"
                      << +(dci->m_symStart + dci->m_numSym - 1) << "\t start " << Simulator::Now()
                      << " end " << (Simulator::Now() + varTtiDuration - NanoSeconds(1.0)));

    SendCtrlChannels(varTtiDuration - NanoSeconds(1.0));

    ChannelAccessDenied(); // Reset the channel status
    return varTtiDuration;
}

Time
NrUePhy::DlData(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);

    m_receptionEnabled = true;
    Time varTtiDuration = GetSymbolPeriod() * dci->m_numSym;
    NS_ASSERT(dci->m_rnti == m_rnti);
    m_spectrumPhy->AddExpectedTb({dci->m_ndi,
                                  dci->m_tbSize,
                                  dci->m_mcs,
                                  dci->m_rank,
                                  dci->m_rnti,
                                  FromRBGBitmaskToRBAssignment(dci->m_rbgBitmask),
                                  dci->m_harqProcess,
                                  dci->m_rv,
                                  true,
                                  dci->m_symStart,
                                  dci->m_numSym,
                                  m_currentSlot});
    m_reportDlTbSize(m_netDevice->GetObject<NrUeNetDevice>()->GetImsi(), dci->m_tbSize);
    NS_LOG_INFO("UE" << m_rnti << " RXing DL DATA frame for symbols " << +dci->m_symStart << "-"
                     << +(dci->m_symStart + dci->m_numSym - 1) << " num of rbg assigned: "
                     << FromRBGBitmaskToRBAssignment(dci->m_rbgBitmask).size()
                     << ". RX will take place for " << varTtiDuration);

    return varTtiDuration;
}

Time
NrUePhy::UlData(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);
    if (m_enableUplinkPowerControl)
    {
        m_txPower = m_powerControl->GetPuschTxPower(
            (FromRBGBitmaskToRBAssignment(dci->m_rbgBitmask)).size());
    }
    SetSubChannelsForTransmission(FromRBGBitmaskToRBAssignment(dci->m_rbgBitmask), dci->m_numSym);
    Time varTtiDuration = GetSymbolPeriod() * dci->m_numSym;
    std::list<Ptr<NrControlMessage>> ctrlMsg;
    Ptr<PacketBurst> pktBurst = GetPacketBurst(m_currentSlot, dci->m_symStart, dci->m_rnti);
    if (pktBurst && pktBurst->GetNPackets() > 0)
    {
        std::list<Ptr<Packet>> pkts = pktBurst->GetPackets();
        NrRadioBearerTag bearerTag;
        if (!pkts.front()->PeekPacketTag(bearerTag))
        {
            NS_FATAL_ERROR("No radio bearer tag");
        }
    }
    else
    {
        // put an error, as something is wrong. The UE should not be scheduled
        // if there is no data for him...
        if (dci->m_type != DciInfoElementTdma::MSG3)
        {
            NS_FATAL_ERROR("The UE " << dci->m_rnti << " has been scheduled without data");
        }
        else
        {
            NS_LOG_WARN("Not sending MSG3. Probably in RRC IDEAL mode.");
            return varTtiDuration;
        }
    }
    m_reportUlTbSize(m_netDevice->GetObject<NrUeNetDevice>()->GetImsi(), dci->m_tbSize);

    NS_LOG_DEBUG("UE" << m_rnti << " TXing UL DATA frame for"
                      << " symbols " << +dci->m_symStart << "-"
                      << +(dci->m_symStart + dci->m_numSym - 1) << "\t start " << Simulator::Now()
                      << " end " << (Simulator::Now() + varTtiDuration));

    Simulator::Schedule(NanoSeconds(1.0),
                        &NrUePhy::SendDataChannels,
                        this,
                        pktBurst,
                        ctrlMsg,
                        dci,
                        varTtiDuration - NanoSeconds(2.0));
    return varTtiDuration;
}

void
NrUePhy::StartVarTti(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);
    Time varTtiDuration;

    m_currTbs = dci->m_tbSize;
    m_receptionEnabled = false;

    if (dci->m_type == DciInfoElementTdma::CTRL && dci->m_format == DciInfoElementTdma::DL)
    {
        varTtiDuration = DlCtrl(dci);
    }
    else if (dci->m_type == DciInfoElementTdma::CTRL && dci->m_format == DciInfoElementTdma::UL)
    {
        varTtiDuration = UlCtrl(dci);
    }
    else if (dci->m_type == DciInfoElementTdma::SRS && dci->m_format == DciInfoElementTdma::UL)
    {
        varTtiDuration = UlSrs(dci);
    }
    else if (dci->m_type == DciInfoElementTdma::DATA && dci->m_format == DciInfoElementTdma::DL)
    {
        varTtiDuration = DlData(dci);
    }
    else if ((dci->m_type == DciInfoElementTdma::DATA || dci->m_type == DciInfoElementTdma::MSG3) &&
             dci->m_format == DciInfoElementTdma::UL)
    {
        varTtiDuration = UlData(dci);
    }

    Simulator::Schedule(varTtiDuration, &NrUePhy::EndVarTti, this, dci);
}

void
NrUePhy::EndVarTti(const std::shared_ptr<DciInfoElementTdma>& dci)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("DCI started at symbol "
                 << static_cast<uint32_t>(dci->m_symStart) << " which lasted for "
                 << static_cast<uint32_t>(dci->m_numSym) << " symbols finished");

    if (m_tryToPerformLbt)
    {
        TryToPerformLbt();
        m_tryToPerformLbt = false;
    }

    if (m_currSlotAllocInfo.m_varTtiAllocInfo.empty())
    {
        // end of slot
        m_currentSlot.Add(1);

        Simulator::Schedule(m_lastSlotStart + GetSlotPeriod() - Simulator::Now(),
                            &NrUePhy::StartSlot,
                            this,
                            m_currentSlot);
    }
    else
    {
        VarTtiAllocInfo allocation = m_currSlotAllocInfo.m_varTtiAllocInfo.front();
        m_currSlotAllocInfo.m_varTtiAllocInfo.pop_front();

        Time nextVarTtiStart = GetSymbolPeriod() * allocation.m_dci->m_symStart;

        Simulator::Schedule(nextVarTtiStart + m_lastSlotStart - Simulator::Now(),
                            &NrUePhy::StartVarTti,
                            this,
                            allocation.m_dci);
    }

    m_receptionEnabled = false;
}

void
NrUePhy::PhyDataPacketReceived(const Ptr<Packet>& p)
{
    Simulator::ScheduleWithContext(m_netDevice->GetNode()->GetId(),
                                   GetTbDecodeLatency(),
                                   &NrUePhySapUser::ReceivePhyPdu,
                                   m_phySapUser,
                                   p);
    // m_phySapUser->ReceivePhyPdu (p);
}

void
NrUePhy::SendDataChannels(const Ptr<PacketBurst>& pb,
                          const std::list<Ptr<NrControlMessage>>& ctrlMsg,
                          const std::shared_ptr<DciInfoElementTdma>& dci,
                          const Time& duration)
{
    if (pb->GetNPackets() > 0)
    {
        NrRadioBearerTag tag;
        if (!pb->GetPackets().front()->PeekPacketTag(tag))
        {
            NS_FATAL_ERROR("No radio bearer tag");
        }
    }

    m_spectrumPhy->StartTxDataFrames(pb, ctrlMsg, dci, duration);
}

void
NrUePhy::SendCtrlChannels(Time duration)
{
    m_spectrumPhy->StartTxUlControlFrames(m_ctrlMsgs, duration);
    m_ctrlMsgs.clear();
}

Ptr<NrDlCqiMessage>
NrUePhy::CreateDlCqiFeedbackMessage(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this);
    // Create DL CQI CTRL message
    Ptr<NrDlCqiMessage> msg = Create<NrDlCqiMessage>();
    msg->SetSourceBwp(GetBwpId());
    DlCqiInfo dlcqi;

    dlcqi.m_rnti = m_rnti;
    dlcqi.m_cqiType = DlCqiInfo::WB;

    std::vector<int> cqi;
    dlcqi.m_wbCqi = m_amc->CreateCqiFeedbackWbTdma(sinr, dlcqi.m_mcs);
    msg->SetDlCqi(dlcqi);

    m_cqiFeedbackTrace(m_rnti, dlcqi.m_wbCqi, dlcqi.m_mcs, 1);
    return msg;
}

void
NrUePhy::GenerateDlCqiReport(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this);
    // Not totally sure what this is about. We have to check.
    if (m_ulConfigured && (m_rnti > 0) && m_receptionEnabled)
    {
        m_dlDataSinrTrace(GetCellId(), m_rnti, ComputeAvgSinr(sinr), GetBwpId());

        if (Simulator::Now() > m_wbCqiLast)
        {
            Ptr<NrDlCqiMessage> msg = CreateDlCqiFeedbackMessage(sinr);

            if (msg)
            {
                DoSendControlMessage(msg);
            }
        }
    }
}

void
NrUePhy::EnqueueDlHarqFeedback(const DlHarqInfo& m)
{
    NS_LOG_FUNCTION(this);
    // get the feedback from NrSpectrumPhy and send it through ideal PUCCH to gNB
    Ptr<NrDlHarqFeedbackMessage> msg = Create<NrDlHarqFeedbackMessage>();
    msg->SetSourceBwp(GetBwpId());
    msg->SetDlHarqFeedback(m);

    auto k1It = m_harqIdToK1Map.find(m.m_harqProcessId);

    NS_LOG_DEBUG("ReceiveNrDlHarqFeedback" << " Harq Process " << static_cast<uint32_t>(k1It->first)
                                           << " K1: " << k1It->second << " Frame "
                                           << m_currentSlot);

    Time event = m_lastSlotStart + (GetSlotPeriod() * k1It->second);
    if (event <= Simulator::Now())
    {
        Simulator::ScheduleNow(&NrUePhy::DoSendControlMessageNow, this, msg);
    }
    else
    {
        Simulator::Schedule(event - Simulator::Now(), &NrUePhy::DoSendControlMessageNow, this, msg);
    }
}

void
NrUePhy::SetCam(const Ptr<NrChAccessManager>& cam)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(cam != nullptr);
    m_cam = cam;
    m_cam->SetAccessGrantedCallback(
        std::bind(&NrUePhy::ChannelAccessGranted, this, std::placeholders::_1));
    m_cam->SetAccessDeniedCallback(std::bind(&NrUePhy::ChannelAccessDenied, this));
}

const SfnSf&
NrUePhy::GetCurrentSfnSf() const
{
    return m_currentSlot;
}

uint16_t
NrUePhy::GetRnti() const
{
    return m_rnti;
}

void
NrUePhy::DoReset()
{
    NS_LOG_FUNCTION(this);
    m_raPreambleId = 255; // value out of range
    m_isConnected = false;
}

void
NrUePhy::DoStartCellSearch(uint16_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << dlEarfcn);
    DoSetInitialBandwidth();
}

void
NrUePhy::DoSynchronizeWithGnb(uint16_t cellId, uint16_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << cellId << dlEarfcn);
    DoSynchronizeWithGnb(cellId);
}

void
NrUePhy::DoSetPa(double pa)
{
    NS_LOG_FUNCTION(this << pa);
}

void
NrUePhy::DoSetRsrpFilterCoefficient(uint8_t rsrpFilterCoefficient)
{
    NS_LOG_FUNCTION(this << +rsrpFilterCoefficient);
}

void
NrUePhy::DoSynchronizeWithGnb(uint16_t cellId)
{
    NS_LOG_FUNCTION(this << cellId);
    DoSetCellId(cellId);
    DoSetInitialBandwidth();
}

BeamId
NrUePhy::GetBeamId([[maybe_unused]] uint16_t rnti) const
{
    NS_LOG_FUNCTION(this);
    // That's a bad specification: the UE PHY doesn't know anything about its beam id.
    NS_FATAL_ERROR("ERROR");
}

void
NrUePhy::ScheduleStartEventLoop(uint32_t nodeId, uint16_t frame, uint8_t subframe, uint16_t slot)
{
    NS_LOG_FUNCTION(this);
    Simulator::ScheduleWithContext(nodeId,
                                   MilliSeconds(0),
                                   &NrUePhy::StartEventLoop,
                                   this,
                                   frame,
                                   subframe,
                                   slot);
}

void
NrUePhy::ReportRsReceivedPower(const SpectrumValue& rsReceivedPower)
{
    NS_LOG_FUNCTION(this << rsReceivedPower);
    m_rsrp = 10 * log10(Integral(rsReceivedPower)) + 30;
    NS_LOG_DEBUG("RSRP value updated: " << m_rsrp << " dBm");

    if (m_enableUplinkPowerControl)
    {
        m_powerControl->SetLoggingInfo(GetCellId(), m_rnti);
        m_powerControl->SetRsrp(m_rsrp);
    }
}

void
NrUePhy::ReceivePss(uint16_t cellId, const Ptr<SpectrumValue>& p)
{
    NS_LOG_FUNCTION(this);

    double sum = 0.0;
    uint16_t nRB = 0;

    uint32_t subcarrierSpacing;
    subcarrierSpacing = 15000 * static_cast<uint32_t>(std::pow(2, GetNumerology()));

    Values::const_iterator itPi;
    for (itPi = p->ConstValuesBegin(); itPi != p->ConstValuesEnd(); itPi++)
    {
        // convert PSD [W/Hz] to linear power [W] for the single RE
        double powerTxW = (*itPi) * subcarrierSpacing;
        sum += powerTxW;
        nRB++;
    }

    // measure instantaneous RSRP now (in dBm)
    double rsrp = 10 * log10(1000 * (sum / static_cast<double>(nRB)));

    NS_LOG_DEBUG("RSRP value updated: " << rsrp << " dBm"
                                        << " for Cell Id: " << cellId << " RNTI: " << m_rnti);

    // store RSRP measurements
    std::map<uint16_t, UeMeasurementsElement>::iterator itMeasMap =
        m_ueMeasurementsMap.find(cellId);
    if (itMeasMap == m_ueMeasurementsMap.end())
    {
        // insert new entry
        UeMeasurementsElement newEl;
        newEl.rsrpSum = rsrp;
        newEl.rsrpNum = 1;
        newEl.rsrqSum = 0;
        newEl.rsrqNum = 0;

        NS_LOG_DEBUG("New RSRP entry for Cell Id: " << cellId << " RNTI: " << m_rnti
                                                    << " RSRP: " << newEl.rsrpSum << " dBm"
                                                    << " number of entries: " << +newEl.rsrpNum);

        m_ueMeasurementsMap.insert(std::pair<uint16_t, UeMeasurementsElement>(cellId, newEl));
    }
    else
    {
        (*itMeasMap).second.rsrpSum += rsrp;
        (*itMeasMap).second.rsrpNum++;

        NS_LOG_DEBUG("Update RSRP entry for Cell Id: "
                     << cellId << " RNTI: " << m_rnti
                     << " RSRP Sum: " << (*itMeasMap).second.rsrpSum << " dBm"
                     << " number of entries: " << +((*itMeasMap).second.rsrpNum));
    }
}

void
NrUePhy::ReportUeMeasurements()
{
    NS_LOG_FUNCTION(this);

    NrUeCphySapUser::UeMeasurementsParameters ret;

    std::map<uint16_t, UeMeasurementsElement>::iterator it;
    for (it = m_ueMeasurementsMap.begin(); it != m_ueMeasurementsMap.end(); it++)
    {
        double avg_rsrp;
        double avg_rsrq = 0;
        if ((*it).second.rsrpNum != 0)
        {
            avg_rsrp = (*it).second.rsrpSum / static_cast<double>((*it).second.rsrpNum);
        }
        else
        {
            NS_LOG_WARN(" RSRP nSamples is zero!");
            avg_rsrp = 0;
        }

        NS_LOG_DEBUG(" Report UE Measurements for CellId "
                     << (*it).first << " Reporting UE " << m_rnti << " Av. RSRP " << avg_rsrp
                     << " (nSamples " << +((*it).second.rsrpNum) << ")"
                     << " BwpID " << GetBwpId());

        m_reportRsrpTrace(GetCellId(), m_imsi, m_rnti, avg_rsrp, GetBwpId());

        // trigger RLF detection only when UE has an active RRC connection
        // and RLF detection attribute is set to true
        if (m_isConnected && m_enableRlfDetection)
        {
            double avrgSinrForRlf = ComputeAvgSinr(m_ctrlSinrForRlf);
            RlfDetection(10 * log10(avrgSinrForRlf));
        }

        NrUeCphySapUser::UeMeasurementsElement newEl;
        newEl.m_cellId = (*it).first;
        newEl.m_rsrp = avg_rsrp;
        newEl.m_rsrq = avg_rsrq; // LEAVE IT 0 FOR THE MOMENT
        ret.m_ueMeasurementsList.push_back(newEl);
        ret.m_componentCarrierId = GetBwpId();

        m_reportUeMeasurements(m_rnti,
                               (*it).first,
                               avg_rsrp,
                               avg_rsrq,
                               (*it).first == GetCellId(),
                               ret.m_componentCarrierId);
    }

    // report to RRC
    m_ueCphySapUser->ReportUeMeasurements(ret);

    m_ueMeasurementsMap.clear();
    Simulator::Schedule(m_ueMeasurementsFilterPeriod, &NrUePhy::ReportUeMeasurements, this);
}

void
NrUePhy::SetCsiFeedbackType(uint8_t csiFeedbackType)
{
    m_csiFeedbackType = csiFeedbackType;
}

void
NrUePhy::ReportDlCtrlSinr(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this);
    uint32_t rbUsed = 0;
    double sinrSum = 0.0;

    for (uint32_t i = 0; i < sinr.GetValuesN(); i++)
    {
        double currentSinr = sinr.ValuesAt(i);
        if (currentSinr != 0)
        {
            rbUsed++;
            sinrSum += currentSinr;
        }
    }

    NS_ASSERT(rbUsed);
    m_dlCtrlSinrTrace(GetCellId(), m_rnti, sinrSum / rbUsed, GetBwpId());
}

uint8_t
NrUePhy::ComputeCqi(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this);
    uint8_t mcs; // it is initialized by AMC in the following call
    uint8_t wbCqi = m_amc->CreateCqiFeedbackWbTdma(sinr, mcs);
    return wbCqi;
}

void
NrUePhy::StartEventLoop(uint16_t frame, uint8_t subframe, uint16_t slot)
{
    NS_LOG_FUNCTION(this);

    if (GetChannelBandwidth() == 0)
    {
        NS_LOG_INFO("Initial bandwidth not set, configuring the default one for Cell ID: "
                    << GetCellId() << ", RNTI: " << GetRnti() << ", BWP ID: " << GetBwpId());
        DoSetInitialBandwidth();
    }

    NS_LOG_INFO("PHY starting. Configuration: "
                << std::endl
                << "\t TxPower: " << m_txPower << " dBm" << std::endl
                << "\t NoiseFigure: " << m_noiseFigure << std::endl
                << "\t TbDecodeLatency: " << GetTbDecodeLatency().GetMicroSeconds() << " us "
                << std::endl
                << "\t Numerology: " << GetNumerology() << std::endl
                << "\t SymbolsPerSlot: " << GetSymbolsPerSlot() << std::endl
                << "\t Pattern: " << NrPhy::GetPattern(m_tddPattern) << std::endl
                << "Attached to physical channel: " << std::endl
                << "\t Channel bandwidth: " << GetChannelBandwidth() << " Hz" << std::endl
                << "\t Channel central freq: " << GetCentralFrequency() << " Hz" << std::endl
                << "\t Num. RB: " << GetRbNum());
    SfnSf startSlot(frame, subframe, slot, GetNumerology());
    StartSlot(startSlot);
}

void
NrUePhy::DoSetInitialBandwidth()
{
    NS_LOG_FUNCTION(this);
    // configure initial bandwidth to 6 RBs, numerology 0
    double initialBandwidthHz =
        6 * GetSubcarrierSpacing() * NrSpectrumValueHelper::SUBCARRIERS_PER_RB;
    // divided by 100*1000 because the parameter should be in 100KHz
    uint16_t initialBandwidthIn100KHz = ceil(initialBandwidthHz / (100 * 1000));
    // account for overhead that will be reduced when determining real BW
    uint16_t initialBandwidthWithOverhead = initialBandwidthIn100KHz / (1 - GetRbOverhead());

    NS_ABORT_MSG_IF(initialBandwidthWithOverhead == 0,
                    " Initial bandwidth could not be set. Parameters provided are: "
                    "\n dlBandwidthInRBNum = "
                        << 6 << "\n m_subcarrierSpacing = " << GetSubcarrierSpacing()
                        << "\n NrSpectrumValueHelper::SUBCARRIERS_PER_RB  = "
                        << (unsigned)NrSpectrumValueHelper::SUBCARRIERS_PER_RB
                        << "\n m_rbOh = " << GetRbOverhead());

    DoSetDlBandwidth(initialBandwidthWithOverhead);
}

uint16_t
NrUePhy::DoGetCellId() const
{
    return GetCellId();
}

uint32_t
NrUePhy::DoGetDlEarfcn()
{
    // TBD See how to get rid of this function in future
    // Added for the compatibility with 810 MR to LTE.
    NS_LOG_FUNCTION(this);
    NS_LOG_WARN("DoGetDlEarfcn function is called. This function should be removed in future once "
                "NR has its own RRC.");
    return 0;
}

void
NrUePhy::DoSetDlBandwidth(uint16_t dlBandwidth)
{
    NS_LOG_FUNCTION(this << +dlBandwidth);

    SetChannelBandwidth(dlBandwidth);

    NS_LOG_DEBUG("PHY reconfiguring. Result: "
                 << std::endl
                 << "\t TxPower: " << m_txPower << " dBm" << std::endl
                 << "\t NoiseFigure: " << m_noiseFigure << std::endl
                 << "\t TbDecodeLatency: " << GetTbDecodeLatency().GetMicroSeconds() << " us "
                 << std::endl
                 << "\t Numerology: " << GetNumerology() << std::endl
                 << "\t SymbolsPerSlot: " << GetSymbolsPerSlot() << std::endl
                 << "\t Pattern: " << NrPhy::GetPattern(m_tddPattern) << std::endl
                 << "Attached to physical channel: " << std::endl
                 << "\t Channel bandwidth: " << GetChannelBandwidth() << " Hz" << std::endl
                 << "\t Channel central freq: " << GetCentralFrequency() << " Hz" << std::endl
                 << "\t Num. RB: " << GetRbNum());
}

void
NrUePhy::DoConfigureUplink(uint16_t ulEarfcn, uint8_t ulBandwidth)
{
    NS_LOG_FUNCTION(this << ulEarfcn << +ulBandwidth);
    // Ignore this; should be equal to dlBandwidth
    m_ulConfigured = true;
}

void
NrUePhy::DoConfigureReferenceSignalPower(int8_t referenceSignalPower)
{
    NS_LOG_FUNCTION(this << referenceSignalPower);
    m_powerControl->ConfigureReferenceSignalPower(referenceSignalPower);
}

void
NrUePhy::DoSetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << rnti);
    GetSpectrumPhy()->SetRnti(rnti);
    m_rnti = rnti;
}

void
NrUePhy::DoSetTransmissionMode(uint8_t txMode)
{
    NS_LOG_FUNCTION(this << +txMode);
}

void
NrUePhy::DoSetSrsConfigurationIndex(uint16_t srcCi)
{
    NS_LOG_FUNCTION(this << srcCi);
}

void
NrUePhy::SetPhySapUser(NrUePhySapUser* ptr)
{
    m_phySapUser = ptr;
}

void
NrUePhy::DoNotifyConnectionSuccessful()
{
    /**
     * Radio link failure detection should take place only on the
     * primary carrier to avoid errors due to multiple calls to the
     * same methods at the RRC layer
     */
    if (GetBwpId() == 0)
    {
        m_isConnected = true;
        // Initialize the parameters for radio link failure detection
        InitializeRlfParams();
    }
}

void
NrUePhy::DoResetPhyAfterRlf()
{
    NS_LOG_FUNCTION(this);
    // m_spectrumPhy->m_harqPhyModule->ClearDlHarqBuffer(m_rnti); // flush HARQ buffers
    DoReset();
}

void
NrUePhy::DoResetRlfParams()
{
    NS_LOG_FUNCTION(this);
    InitializeRlfParams();
}

void
NrUePhy::DoStartInSyncDetection()
{
    NS_LOG_FUNCTION(this);
    // indicates that the downlink radio link quality has to be monitored for in-sync indications
    m_downlinkInSync = false;
}

void
NrUePhy::InitializeRlfParams()
{
    NS_LOG_FUNCTION(this);
    m_numOfSubframes = 0;
    m_sinrDbFrame = 0;
    m_numOfFrames = 0;
    m_downlinkInSync = true;
}

void
NrUePhy::RlfDetection(double sinrDb)
{
    NS_LOG_FUNCTION(this << sinrDb);
    m_sinrDbFrame += sinrDb;
    m_numOfSubframes++;
    NS_LOG_LOGIC("No of Subframes: " << m_numOfSubframes
                                     << " UE synchronized: " << m_downlinkInSync);
    // check for out_of_sync indications first when UE is both DL and UL synchronized
    // m_downlinkInSync=true indicates that the evaluation is for out-of-sync indications
    if (m_downlinkInSync && m_numOfSubframes == 10)
    {
        /**
         * For every frame, if the downlink radio link quality(avg SINR)
         * is less than the threshold Qout, then the frame cannot be decoded
         */
        if ((m_sinrDbFrame / m_numOfSubframes) < m_qOut)
        {
            m_numOfFrames++; // increment the counter if a frame cannot be decoded
            NS_LOG_LOGIC("No of Frames which cannot be decoded: " << m_numOfFrames);
        }
        else
        {
            /**
             * If the downlink radio link quality(avg SINR) is greater
             * than the threshold Qout, then the frame counter is reset
             * since only consecutive frames should be considered.
             */
            NS_LOG_INFO("Resetting frame counter at phy. Current value = " << m_numOfFrames);
            m_numOfFrames = 0;
            // Also reset the sync indicator counter at RRC
            m_ueCphySapUser->ResetSyncIndicationCounter();
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
        NS_LOG_LOGIC("At " << Simulator::Now().As(Time::MS)
                           << " ms UE PHY sending out of sync indication to UE RRC layer");
        m_ueCphySapUser->NotifyOutOfSync();
        m_numOfFrames = 0;
    }
    // check for in_sync indications when T310 timer is started
    // m_downlinkInSync=false indicates that the evaluation is for in-sync indications
    if (!m_downlinkInSync && m_numOfSubframes == 10)
    {
        /**
         * For every frame, if the downlink radio link quality(avg SINR)
         * is greater than the threshold Qin, then the frame can be
         * successfully decoded.
         */
        if ((m_sinrDbFrame / m_numOfSubframes) > m_qIn)
        {
            m_numOfFrames++; // increment the counter if a frame can be decoded
            NS_LOG_LOGIC("No of Frames successfully decoded: " << m_numOfFrames);
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
            m_ueCphySapUser->ResetSyncIndicationCounter();
        }
        m_numOfSubframes = 0;
        m_sinrDbFrame = 0;
    }
    /**
     * Once the number of consecutive frames which can be decoded equals the Qin evaluation period
     * (i.e 100ms), then an in-sync indication is sent to the RRC layer
     */
    if (!m_downlinkInSync && (m_numOfFrames * 10) == m_numOfQinEvalSf)
    {
        NS_LOG_LOGIC("At " << Simulator::Now().As(Time::MS)
                           << " ms UE PHY sending in sync indication to UE RRC layer");
        m_ueCphySapUser->NotifyInSync();
        m_numOfFrames = 0;
    }
}

void
NrUePhy::DoSetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this);
    m_imsi = imsi;
}

void
NrUePhy::GenerateDlCqiReportMimo(const NrMimoSignal& rxSignal,
                                 NrPmSearch::PmiUpdate pmiUpdateParams)
{
    NS_LOG_FUNCTION(this);
    // Adopted from NrUePhy::GenerateDlCqiReport: CQI feedback requires properly configured UE
    if (!m_ulConfigured || (m_rnti == 0))
    {
        return;
    }

    // Create DL CQI message for CQI, PMI, and RI. PMI values are updated only if specified by
    // pmiUpdateParams, otherwise assume same PMI values as during last CQI feedback
    auto cqi = m_pmSearch->CreateCqiFeedbackMimo(rxSignal, pmiUpdateParams);
    auto dlcqi = DlCqiInfo{
        .m_rnti = m_rnti,
        .m_ri = cqi.m_rank,
        .m_cqiType = cqi.m_cqiType,
        .m_wbCqi = cqi.m_wbCqi,
        .m_wbPmi = cqi.m_wbPmi,
        .m_sbCqis = cqi.m_sbCqis,
        .m_sbPmis = cqi.m_sbPmis,
        .m_mcs = cqi.m_mcs,
        .m_optPrecMat = cqi.m_optPrecMat,
    };

    m_cqiFeedbackTrace(m_rnti, cqi.m_wbCqi, cqi.m_mcs, cqi.m_rank);

    auto msg = Create<NrDlCqiMessage>();
    msg->SetSourceBwp(GetBwpId());
    msg->SetDlCqi(dlcqi);

    DoSendControlMessage(msg);
}

uint8_t
NrUePhy::GetCsiFeedbackType() const
{
    return m_csiFeedbackType;
}

void
NrUePhy::CsiRsReceived(const std::vector<MimoSignalChunk>& csiRsMimoSignal)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(csiRsMimoSignal.size() == 1);
    m_csiRsMimoSignal = NrMimoSignal(csiRsMimoSignal);
    m_lastCsiRsMimoSignalTime = Simulator::Now();
}

void
NrUePhy::GenerateCsiRsCqi()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_csiRsMimoSignal.m_chanMat.GetSize() != 0);
    NrMimoSignal csiFeedbackSignal = m_csiRsMimoSignal;
    // if there is some old interference information use it,
    // otherwise, just use the plain CSI-RS signal for the CQI feedback
    // (i.e., no interference information). This may happen before any
    // PDSCH for this UE is scheduled, and CSI-IM is disabled.
    if (m_avgIntCovMat.GetSize() != 0)
    {
        csiFeedbackSignal.m_covMat = m_avgIntCovMat;
    }
    TriggerDlCqiGeneration(csiFeedbackSignal, NrPmSearch::PmiUpdate(true, true));
}

void
NrUePhy::CsiImEnded(const std::vector<MimoSignalChunk>& csiImSignalChunks)
{
    NS_LOG_FUNCTION(this);
    // Combine multiple CSI-IM signal chunks into a single channel,
    // and interference covariance
    auto csiFeedbackSignal = NrMimoSignal(csiImSignalChunks);
    CalcAvgIntCovMat(&m_avgIntCovMat, csiFeedbackSignal.m_covMat);
    // CSI-IM does not have RX spectrum channel matrix, because it only contains the interference
    // hence the channel spectrum matrix to be used is from CSI-RS signal
    if (m_alphaCovMat != 1)
    {
        csiFeedbackSignal.m_covMat = m_csiRsMimoSignal.m_covMat;
    }
    csiFeedbackSignal.m_chanMat = m_csiRsMimoSignal.m_chanMat;
    TriggerDlCqiGeneration(csiFeedbackSignal, NrPmSearch::PmiUpdate(true, true));
}

void
NrUePhy::PdschMimoReceived(const std::vector<MimoSignalChunk>& pdschMimoChunks)
{
    NS_LOG_FUNCTION(this);
    // Combine multiple signal chunks into a single channel matrix and interference covariance
    auto csiFeedbackSignal = NrMimoSignal(pdschMimoChunks);
    // if alpha != 1, calculate the interference covariance moving average
    CalcAvgIntCovMat(&m_avgIntCovMat, csiFeedbackSignal.m_covMat);
    if (m_alphaCovMat != 1)
    {
        csiFeedbackSignal.m_covMat = m_avgIntCovMat;
    }
    // if CSI-RS enabled, use the spectrum channel matrix from CSI-RS signal
    if (m_csiFeedbackType & CQI_CSI_RS)
    {
        NS_ASSERT_MSG(m_csiRsMimoSignal.m_chanMat.GetSize(),
                      "CSI-RS based channel matrix not available");
        csiFeedbackSignal.m_chanMat = m_csiRsMimoSignal.m_chanMat;
    }

    // CSI-RS slot, or PDSCH only based CQI feedback
    // Determine if an update to wideband or subband PMI is needed and possible
    auto pmiUpdateParams = CheckUpdatePmi();
    TriggerDlCqiGeneration(csiFeedbackSignal, pmiUpdateParams);
}

void
NrUePhy::TriggerDlCqiGeneration(const NrMimoSignal& csiFeedbackSignal,
                                NrPmSearch::PmiUpdate pmiUpdateParams)
{
    NS_LOG_FUNCTION(this);
    if (m_pmSearch)
    {
        GenerateDlCqiReportMimo(csiFeedbackSignal, pmiUpdateParams);
    }
    else
    {
        // Interference whitening: normalize the signal such that interference + noise covariance
        // matrix is the identity matrix
        auto intfNormChanMat =
            csiFeedbackSignal.m_covMat.CalcIntfNormChannel(csiFeedbackSignal.m_chanMat);
        // Create a dummy precoding matrix
        ComplexMatrixArray precMat = ComplexMatrixArray(
            csiFeedbackSignal.m_chanMat.GetNumCols(),
            csiFeedbackSignal.m_chanMat.GetNumRows(),
            csiFeedbackSignal.m_chanMat.GetNumPages(),
            std::valarray<std::complex<double>>(std::complex<double>(1.0, 0.0),
                                                csiFeedbackSignal.m_chanMat.GetSize()));
        ;

        NrSinrMatrix sinrMatrix = intfNormChanMat.ComputeSinrForPrecoding(precMat);
        GenerateDlCqiReport(sinrMatrix.GetVectorizedSpecVal());
    }
}

void
NrUePhy::CalcAvgIntCovMat(NrCovMat* avgIntCovMat, const NrCovMat& newCovMat) const
{
    NS_LOG_FUNCTION(this);
    if (avgIntCovMat->GetSize() == 0)
    {
        *avgIntCovMat = ComplexMatrixArray(newCovMat.GetNumRows(),
                                           newCovMat.GetNumCols(),
                                           newCovMat.GetNumPages());
    };

    *avgIntCovMat = newCovMat * std::complex<double>{m_alphaCovMat, 0.0} +
                    *avgIntCovMat * std::complex<double>{1 - m_alphaCovMat, 0.0};
}

NrPmSearch::PmiUpdate
NrUePhy::CheckUpdatePmi()
{
    // This implementation only checks if sufficient time has passed since the last update.
    // TODO: Improve following logic that defines when to update wideband and/or
    // subband PMIs for two-stage codebooks. The algorithm must allow managing the computational
    // complexity of PMI updates, and take into account availability of PUCCH/PUSCH resources for
    // sending PMI.
    auto pmiUpdate = NrPmSearch::PmiUpdate{};
    auto now = Simulator::Now();
    if (now > m_wbPmiLastUpdate + m_wbPmiUpdateInterval)
    {
        pmiUpdate.updateWb = true;
        m_wbPmiLastUpdate = now;
    }
    if (now > m_sbPmiLastUpdate + m_sbPmiUpdateInterval)
    {
        pmiUpdate.updateSb = true;
        m_sbPmiLastUpdate = now;
    }
    return pmiUpdate;
}

void
NrUePhy::SetPmSearch(Ptr<NrPmSearch> pmSearch)
{
    m_pmSearch = pmSearch;
    NS_ASSERT(m_amc);
    m_pmSearch->SetAmc(m_amc);
}

Ptr<NrPmSearch>
NrUePhy::GetPmSearch() const
{
    return m_pmSearch;
}

} // namespace ns3
