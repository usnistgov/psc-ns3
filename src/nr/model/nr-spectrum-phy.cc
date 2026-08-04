// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-spectrum-phy.h"

#include "nr-chunk-processor.h"
#include "nr-gnb-net-device.h"
#include "nr-gnb-phy.h"
#include "nr-lte-mi-error-model.h"
#include "nr-radio-bearer-tag.h"
#include "nr-ue-net-device.h"
#include "nr-ue-phy.h"

#include "ns3/node.h"
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/matrix-based-channel-model.h>
#include <ns3/trace-source-accessor.h>

#include <numeric>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSpectrumPhy");
NS_OBJECT_ENSURE_REGISTERED(NrSpectrumPhy);

std::ostream&
operator<<(std::ostream& os, const enum NrSpectrumPhy::State state)
{
    switch (state)
    {
    case NrSpectrumPhy::TX:
        os << "TX";
        break;
    case NrSpectrumPhy::RX_DL_CTRL:
        os << "RX_DL_CTRL";
        break;
    case NrSpectrumPhy::RX_UL_CTRL:
        os << "RX_UL_CTRL";
        break;
    case NrSpectrumPhy::CCA_BUSY:
        os << "CCA_BUSY";
        break;
    case NrSpectrumPhy::RX_DATA:
        os << "RX_DATA";
        break;
    case NrSpectrumPhy::IDLE:
        os << "IDLE";
        break;
    case NrSpectrumPhy::RX_UL_SRS:
        os << "RX_UL_SRS";
        break;
    default:
        NS_ABORT_MSG("Unknown state.");
    }
    return os;
}

NrSpectrumPhy::NrSpectrumPhy()
    : SpectrumPhy()
{
    NS_LOG_FUNCTION(this);
    m_interferenceData = CreateObject<NrInterference>();
    m_interferenceCtrl = CreateObject<NrInterference>();
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(1.0));
}

NrSpectrumPhy::~NrSpectrumPhy()
{
}

void
NrSpectrumPhy::DoDispose()
{
    NS_LOG_FUNCTION(this);
    if (m_channel)
    {
        m_channel->Dispose();
    }

    m_channel = nullptr;

    if (m_interferenceData)
    {
        m_interferenceData->Dispose();
    }

    if (m_interferenceCtrl)
    {
        m_interferenceCtrl->Dispose();
    }

    if (m_interferenceSrs)
    {
        m_interferenceSrs->Dispose();
        m_interferenceSrs = nullptr;
    }

    if (m_interferenceCsiRs)
    {
        m_interferenceCsiRs->Dispose();
        m_interferenceCsiRs = nullptr;
    }

    if (m_interferenceCsiIm)
    {
        m_interferenceCsiIm->Dispose();
        m_interferenceCsiIm = nullptr;
    }

    m_interferenceData = nullptr;
    m_interferenceCtrl = nullptr;
    m_mobility = nullptr;
    m_phy = nullptr;
    m_rxSpectrumModel = nullptr;
    m_txPsd = nullptr;

    m_phyRxDataEndOkCallback = MakeNullCallback<void, const Ptr<Packet>&>();
    m_phyDlHarqFeedbackCallback = MakeNullCallback<void, const DlHarqInfo&>();
    m_phyUlHarqFeedbackCallback = MakeNullCallback<void, const UlHarqInfo&>();
    m_phyRxPssCallback = MakeNullCallback<void, uint16_t, const Ptr<SpectrumValue>&>();

    SpectrumPhy::DoDispose();
}

TypeId
NrSpectrumPhy::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSpectrumPhy")
            .SetParent<SpectrumPhy>()
            .AddConstructor<NrSpectrumPhy>()
            .AddAttribute("DataErrorModelEnabled",
                          "Activate/Deactivate the error model of data (TBs of PDSCH and PUSCH) "
                          "[by default is active].",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NrSpectrumPhy::SetDataErrorModelEnabled),
                          MakeBooleanChecker())
            .AddAttribute("ErrorModelType",
                          "Default type of the Error Model to apply to TBs of PDSCH and PUSCH",
                          TypeIdValue(NrLteMiErrorModel::GetTypeId()),
                          MakeTypeIdAccessor(&NrSpectrumPhy::SetErrorModelType),
                          MakeTypeIdChecker())
            .AddAttribute(
                "UnlicensedMode",
                "Activate/Deactivate unlicensed mode in which energy detection is performed"
                " and PHY state machine has an additional state CCA_BUSY.",
                BooleanValue(false),
                MakeBooleanAccessor(&NrSpectrumPhy::SetUnlicensedMode),
                MakeBooleanChecker())
            .AddAttribute("CcaMode1Threshold",
                          "The energy of a received signal should be higher than "
                          "this threshold (dbm) to allow the PHY layer to declare CCA BUSY state.",
                          DoubleValue(-62.0),
                          MakeDoubleAccessor(&NrSpectrumPhy::SetCcaMode1Threshold,
                                             &NrSpectrumPhy::GetCcaMode1Threshold),
                          MakeDoubleChecker<double>())
            .AddTraceSource("RxPacketTraceGnb",
                            "The no. of packets received and transmitted by the Base Station",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_rxPacketTraceGnb),
                            "ns3::RxPacketTraceParams::TracedCallback")
            .AddTraceSource("TxPacketTraceGnb",
                            "Traces when the packet is being transmitted by the Base Station",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_txPacketTraceGnb),
                            "ns3::GnbPhyPacketCountParameter::TracedCallback")
            .AddTraceSource("RxPacketTraceUe",
                            "The no. of packets received and transmitted by the User Device",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_rxPacketTraceUe),
                            "ns3::RxPacketTraceParams::TracedCallback")
            .AddTraceSource(
                "ChannelOccupied",
                "This traced callback is triggered every time that the channel is occupied",
                MakeTraceSourceAccessor(&NrSpectrumPhy::m_channelOccupied),
                "ns3::Time::TracedCallback")
            .AddTraceSource("TxDataTrace",
                            "Indicates when the channel is being occupied by a data transmission",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_txDataTrace),
                            "ns3::Time::TracedCallback")
            .AddTraceSource("TxCtrlTrace",
                            "Indicates when the channel is being occupied by a ctrl transmission",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_txCtrlTrace),
                            "ns3::Time::TracedCallback")
            .AddTraceSource("RxDataTrace",
                            "Indicates the reception of data from this cell (reporting the rxPsd "
                            "without interferences)",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_rxDataTrace),
                            "ns3::RxDataTracedCallback::TracedCallback")
            .AddTraceSource("DlDataSnrTrace",
                            "Report the SNR computed for each TB in DL",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_dlDataSnrTrace),
                            "ns3::NrSpectrumPhy::DataSnrTracedCallback")
            .AddTraceSource("DlCtrlPathloss",
                            "Pathloss calculated for CTRL",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_dlCtrlPathlossTrace),
                            "ns3::NrSpectrumPhy::DlPathlossTrace")
            .AddTraceSource("DlDataPathloss",
                            "Pathloss calculated for CTRL",
                            MakeTraceSourceAccessor(&NrSpectrumPhy::m_dlDataPathlossTrace),
                            "ns3::NrSpectrumPhy::DlPathlossTrace");
    return tid;
}

NrSpectrumPhy::State
NrSpectrumPhy::GetState() const
{
    return m_state;
}

Ptr<UniformRandomVariable>
NrSpectrumPhy::GetErrorModelRv() const
{
    return m_random;
}

Ptr<NrPhy>
NrSpectrumPhy::GetNrPhy() const
{
    return m_phy;
}

Time
NrSpectrumPhy::GetFirstRxStart() const
{
    return m_firstRxStart;
}

void
NrSpectrumPhy::SetFirstRxStart(Time startTime)
{
    m_firstRxStart = startTime;
}

Time
NrSpectrumPhy::GetFirstRxDuration() const
{
    return m_firstRxDuration;
}

void
NrSpectrumPhy::SetFirstRxDuration(Time duration)
{
    m_firstRxDuration = duration;
}

void
NrSpectrumPhy::IncrementActiveTransmissions()
{
    m_activeTransmissions++;
}

void
NrSpectrumPhy::NotifyRxDataTrace(const SfnSf& sfn,
                                 Ptr<const SpectrumValue> spectrumValue,
                                 const Time& duration,
                                 uint16_t bwpId,
                                 uint16_t cellId) const
{
    m_rxDataTrace(sfn, spectrumValue, duration, bwpId, cellId);
}

void
NrSpectrumPhy::NotifyTxCtrlTrace(Time duration) const
{
    m_txCtrlTrace(duration);
}

void
NrSpectrumPhy::NotifyTxDataTrace(Time duration) const
{
    m_txDataTrace(duration);
}

// set callbacks

void
NrSpectrumPhy::SetPhyRxDataEndOkCallback(const NrPhyRxDataEndOkCallback& c)
{
    NS_LOG_FUNCTION(this);
    m_phyRxDataEndOkCallback = c;
}

void
NrSpectrumPhy::SetPhyRxCtrlEndOkCallback(const NrPhyRxCtrlEndOkCallback& c)
{
    NS_LOG_FUNCTION(this);
    m_phyRxCtrlEndOkCallback = c;
}

void
NrSpectrumPhy::SetPhyRxPssCallback(const NrPhyRxPssCallback& c)
{
    NS_LOG_FUNCTION(this);
    m_phyRxPssCallback = c;
}

void
NrSpectrumPhy::SetPhyDlHarqFeedbackCallback(const NrPhyDlHarqFeedbackCallback& c)
{
    NS_LOG_FUNCTION(this);
    m_phyDlHarqFeedbackCallback = c;
}

void
NrSpectrumPhy::SetPhyUlHarqFeedbackCallback(const NrPhyUlHarqFeedbackCallback& c)
{
    NS_LOG_FUNCTION(this);
    m_phyUlHarqFeedbackCallback = c;
}

// inherited from SpectrumPhy
void
NrSpectrumPhy::SetDevice(Ptr<NetDevice> d)
{
    NS_LOG_FUNCTION(this << d);
    m_device = d;
    // It would be appropriate that the creation of interference for SRS is in the constructor.
    // But, in the constructor since the device is yet not configured we don't know if we
    // need or not to create the interference object for SRS. It should be only created at gNBs, and
    // not at UEs. That is why we postpone the creation to the moment of setting the device.
    // The other option would be to pass the device as a parameter to the constructor of the
    // NrSpectrumPhy. But since NrSpectrumPhy inherits this SetDevice function from SpectrumPhy
    // class, so passing also device as a parameter to constructor would create a more complicate
    // interface.

    if (m_isGnb)
    {
        m_interferenceSrs = CreateObject<NrInterference>();
        m_interferenceSrs->TraceConnectWithoutContext(
            "SnrPerProcessedChunk",
            MakeCallback(&NrSpectrumPhy::UpdateSrsSnrPerceived, this));
    }
    else
    {
        m_interferenceCsiRs = CreateObject<NrInterference>();
        m_interferenceCsiIm = CreateObject<NrInterference>();
        m_interferenceData->TraceConnectWithoutContext(
            "SnrPerProcessedChunk",
            MakeCallback(&NrSpectrumPhy::ReportWbDlDataSnrPerceived, this));
    }
}

Ptr<NetDevice>
NrSpectrumPhy::GetDevice() const
{
    return m_device;
}

void
NrSpectrumPhy::SetMobility(Ptr<MobilityModel> m)
{
    NS_LOG_FUNCTION(this << m);
    m_mobility = m;
}

Ptr<MobilityModel>
NrSpectrumPhy::GetMobility() const
{
    return m_mobility;
}

void
NrSpectrumPhy::SetChannel(Ptr<SpectrumChannel> c)
{
    NS_LOG_FUNCTION(this << c);
    m_channel = c;
}

Ptr<SpectrumChannel>
NrSpectrumPhy::GetChannel() const
{
    return m_channel;
}

Ptr<const SpectrumModel>
NrSpectrumPhy::GetRxSpectrumModel() const
{
    return m_rxSpectrumModel;
}

Ptr<Object>
NrSpectrumPhy::GetAntenna() const
{
    return m_antenna;
}

// set/get attributes

void
NrSpectrumPhy::SetBeamManager(Ptr<BeamManager> b)
{
    NS_LOG_FUNCTION(this << b);
    m_beamManager = b;
}

Ptr<BeamManager>
NrSpectrumPhy::GetBeamManager()
{
    return m_beamManager;
}

void
NrSpectrumPhy::SetErrorModel(Ptr<NrErrorModel> em)
{
    NS_LOG_FUNCTION(this << em);
    m_errorModel = em;
}

Ptr<NrErrorModel>
NrSpectrumPhy::GetErrorModel() const
{
    return m_errorModel;
}

void
NrSpectrumPhy::EnableDlDataPathlossTrace()
{
    NS_LOG_FUNCTION(this);
    m_enableDlDataPathlossTrace = true;
}

void
NrSpectrumPhy::EnableDlCtrlPathlossTrace()
{
    NS_LOG_FUNCTION(this);
    m_enableDlCtrlPathlossTrace = true;
}

void
NrSpectrumPhy::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << rnti);
    m_rnti = rnti;
    m_hasRnti = true;
}

uint16_t
NrSpectrumPhy::GetRnti() const
{
    return m_rnti;
}

void
NrSpectrumPhy::SetCcaMode1Threshold(double thresholdDBm)
{
    NS_LOG_FUNCTION(this << thresholdDBm);
    // convert dBm to Watt
    m_ccaMode1ThresholdW = (std::pow(10.0, thresholdDBm / 10.0)) / 1000.0;
}

double
NrSpectrumPhy::GetCcaMode1Threshold() const
{
    // convert Watt to dBm
    return 10.0 * std::log10(m_ccaMode1ThresholdW * 1000.0);
}

void
NrSpectrumPhy::SetUnlicensedMode(bool unlicensedMode)
{
    NS_LOG_FUNCTION(this << unlicensedMode);
    m_unlicensedMode = unlicensedMode;
}

void
NrSpectrumPhy::SetDataErrorModelEnabled(bool dataErrorModelEnabled)
{
    NS_LOG_FUNCTION(this << dataErrorModelEnabled);
    m_dataErrorModelEnabled = dataErrorModelEnabled;
}

void
NrSpectrumPhy::SetErrorModelType(TypeId errorModelType)
{
    NS_LOG_FUNCTION(this << errorModelType.GetName());
    m_errorModelType = errorModelType;
}

// other

void
NrSpectrumPhy::SetNoisePowerSpectralDensity(const Ptr<const SpectrumValue>& noisePsd)
{
    NS_LOG_FUNCTION(this << noisePsd);
    NS_ASSERT(noisePsd);
    m_rxSpectrumModel = noisePsd->GetSpectrumModel();
    m_interferenceData->SetNoisePowerSpectralDensity(noisePsd);
    m_interferenceCtrl->SetNoisePowerSpectralDensity(noisePsd);
    if (m_interferenceSrs)
    {
        m_interferenceSrs->SetNoisePowerSpectralDensity(noisePsd);
    }
    if (m_interferenceCsiRs)
    {
        m_interferenceCsiRs->SetNoisePowerSpectralDensity(noisePsd);
        m_interferenceCsiIm->SetNoisePowerSpectralDensity(noisePsd);
    }
}

void
NrSpectrumPhy::SetTxPowerSpectralDensity(const Ptr<SpectrumValue>& TxPsd)
{
    NS_LOG_FUNCTION(this << TxPsd);
    m_txPsd = TxPsd;
}

Ptr<const SpectrumValue>
NrSpectrumPhy::GetTxPowerSpectralDensity()
{
    return m_txPsd;
}

Ptr<MatrixBasedChannelModel::Complex3DVector>
NrSpectrumPhy::CreateSpectrumChannelMatrix(const Ptr<SpectrumSignalParameters> params) const
{
    auto rxPsd = params->psd;
    uint32_t nRb = rxPsd->GetValuesN();
    auto txAntenna = DynamicCast<PhasedArrayModel>(params->txPhy->GetAntenna());
    auto rxAntenna = DynamicCast<PhasedArrayModel>(GetAntenna());
    NS_ABORT_MSG_UNLESS(txAntenna && rxAntenna, "Only phased antenna array models are supported.");

    auto txAntennaPorts = txAntenna->GetNumPorts();
    auto rxAntennaPorts = rxAntenna->GetNumPorts();
    NS_ASSERT_MSG(txAntennaPorts == 1 && rxAntennaPorts == 1,
                  "The conversion only fits to a single antenna port in Tx and Rx");
    Ptr<MatrixBasedChannelModel::Complex3DVector> channelSpct =
        Create<MatrixBasedChannelModel::Complex3DVector>(rxAntennaPorts, txAntennaPorts, nRb);
    auto vit = params->psd->ValuesBegin();
    size_t iRb = 0;
    while (vit != params->psd->ValuesEnd())
    {
        if (*vit != 0.0)
        {
            auto sqrtvit = sqrt(*vit);
            for (size_t u = 0; u < 1; u++)
            {
                for (size_t s = 0; s < 1; s++)
                {
                    channelSpct->Elem(u, s, iRb) = sqrtvit;
                }
            }
        }
        vit++;
        iRb++;
    }
    return channelSpct;
}

void
NrSpectrumPhy::StartRx(Ptr<SpectrumSignalParameters> params)
{
    NS_LOG_FUNCTION(this);
    Ptr<const SpectrumValue> rxPsd = params->psd;
    Time duration = params->duration;
    NS_LOG_INFO("Start receiving signal: " << params->psd << " duration= " << duration);

    // phased-array mimo expects a channel
    if (!params->spectrumChannelMatrix &&
        GetSpectrumChannel()->GetPhasedArraySpectrumPropagationLossModel())
    {
        params->spectrumChannelMatrix = CreateSpectrumChannelMatrix(params);
    }

    // pass it to interference calculations regardless of the type (nr or non-nr)
    m_interferenceData->AddSignalMimo(params, duration);

    // pass the signal to the interference calculator regardless of the type (nr or non-nr)
    if (m_interferenceSrs)
    {
        m_interferenceSrs->AddSignalMimo(params, duration);
    }

    Ptr<NrSpectrumSignalParametersDataFrame> nrDataRxParams =
        DynamicCast<NrSpectrumSignalParametersDataFrame>(params);

    Ptr<NrSpectrumSignalParametersDlCtrlFrame> dlCtrlRxParams =
        DynamicCast<NrSpectrumSignalParametersDlCtrlFrame>(params);

    Ptr<NrSpectrumSignalParametersUlCtrlFrame> ulCtrlRxParams =
        DynamicCast<NrSpectrumSignalParametersUlCtrlFrame>(params);

    Ptr<NrSpectrumSignalParametersCsiRs> csiRsRxParams =
        DynamicCast<NrSpectrumSignalParametersCsiRs>(params);

    if (nrDataRxParams)
    {
        if (m_interferenceCsiIm && m_interferenceCsiIm->IsChunkProcessorSet() &&
            nrDataRxParams->cellId != m_phy->GetCellId())
        {
            m_interferenceCsiIm->AddSignalMimo(params, duration);
        }

        if (nrDataRxParams->cellId == GetCellId())
        {
            // Receive only signals intended for this receiver. Receive only
            //  - if the receiver is a UE and the signal's RNTI matches the UE's RNTI,
            //  - or if the receiver device is either a gNB or not configured (has no RNTI)
            auto isIntendedRx = (nrDataRxParams->rnti == m_rnti) || !m_hasRnti;

            // Workaround: always receive the signal, to keep performance equal to OSS code.
            // This ensures all UEs can generate CQI feedback from data signals, even from those
            // signals that are intended for other UEs in the same cell.
            // TODO: implement proper CSI-RS signalling for CQI and disable this workaround
            isIntendedRx = true;

            if (isIntendedRx)
            {
                StartRxData(nrDataRxParams);
            }
            if (!m_isGnb and m_enableDlDataPathlossTrace)
            {
                Ptr<const SpectrumValue> txPsd =
                    DynamicCast<NrSpectrumPhy>(nrDataRxParams->txPhy)->GetTxPowerSpectralDensity();
                Ptr<const SpectrumValue> rxPsd = nrDataRxParams->psd;
                double pathloss = 10 * log10(Integral(*txPsd)) - 10 * log10(Integral(*rxPsd));
                Ptr<NrUePhy> phy = (DynamicCast<NrUePhy>(m_phy));
                m_dlDataPathlossTrace(GetCellId(),
                                      GetBwpId(),
                                      GetMobility()->GetObject<Node>()->GetId(),
                                      pathloss,
                                      phy->ComputeCqi(m_sinrPerceived));
            }
        }
        else
        {
            NS_LOG_INFO(" Received DATA not in sync with this signal (cellId="
                        << nrDataRxParams->cellId << ", m_cellId=" << GetCellId() << ")");
        }
    }
    else if (dlCtrlRxParams != nullptr)
    {
        m_interferenceCtrl->AddSignalMimo(params, duration);

        if (!m_isGnb)
        {
            if (dlCtrlRxParams->pss)
            {
                if (dlCtrlRxParams->cellId == GetCellId())
                {
                    NS_LOG_DEBUG(
                        "Receiving PSS from Serving Cell with Id: " << dlCtrlRxParams->cellId);
                }
                else
                {
                    NS_LOG_DEBUG(
                        "Receiving PSS from Neighbor Cell with Id: " << dlCtrlRxParams->cellId);
                }

                if (!m_phyRxPssCallback.IsNull())
                {
                    m_phyRxPssCallback(dlCtrlRxParams->cellId, dlCtrlRxParams->psd);
                }
            }

            if (dlCtrlRxParams->cellId == GetCellId())
            {
                m_interferenceCtrl->StartRxMimo(params);
                StartRxDlCtrl(dlCtrlRxParams);

                if (m_enableDlCtrlPathlossTrace)
                {
                    Ptr<const SpectrumValue> txPsd =
                        DynamicCast<NrSpectrumPhy>(dlCtrlRxParams->txPhy)
                            ->GetTxPowerSpectralDensity();
                    Ptr<const SpectrumValue> rxPsd = dlCtrlRxParams->psd;
                    double pathloss = 10 * log10(Integral(*txPsd)) - 10 * log10(Integral(*rxPsd));
                    m_dlCtrlPathlossTrace(GetCellId(),
                                          GetBwpId(),
                                          GetMobility()->GetObject<Node>()->GetId(),
                                          pathloss);
                }
            }
            else
            {
                NS_LOG_INFO("Received DL CTRL, but not in sync with this signal (cellId="
                            << dlCtrlRxParams->cellId << ", m_cellId=" << GetCellId() << ")");
            }
        }
        else
        {
            NS_LOG_DEBUG("DL CTRL ignored at gNB");
        }
    }
    else if (ulCtrlRxParams != nullptr)
    {
        if (m_isGnb) // only gNBs should enter into reception of UL CTRL signals
        {
            if (ulCtrlRxParams->cellId == GetCellId())
            {
                if (IsOnlySrs(ulCtrlRxParams->ctrlMsgList))
                {
                    StartRxSrs(ulCtrlRxParams);
                }
                else
                {
                    StartRxUlCtrl(ulCtrlRxParams);
                }
            }
            else
            {
                NS_LOG_INFO("Received UL CTRL, but not in sync with this signal (cellId="
                            << ulCtrlRxParams->cellId << ", m_cellId=" << GetCellId() << ")");
            }
        }
        else
        {
            NS_LOG_DEBUG("UL CTRL ignored at UE device");
        }
    }
    else if (csiRsRxParams != nullptr)
    {
        if (m_hasRnti && csiRsRxParams->cellId == GetCellId())
        {
            StartRxCsiRs(csiRsRxParams);
        }
    }
    else
    {
        NS_LOG_INFO("Received non-nr signal of duration:" << duration);
    }

    // If in RX or TX state, do not change to CCA_BUSY until is finished
    // RX or TX state. If in IDLE state, then ok, move to CCA_BUSY if the
    // channel is found busy.
    if (m_unlicensedMode && m_state == IDLE)
    {
        MaybeCcaBusy();
    }
}

void
NrSpectrumPhy::StartTxDataFrames(const Ptr<PacketBurst>& pb,
                                 const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                                 const std::shared_ptr<DciInfoElementTdma> dci,
                                 const Time& duration)
{
    NS_LOG_FUNCTION(this);
    switch (m_state)
    {
    case RX_DATA:
        /* no break */
        [[fallthrough]];
    case RX_DL_CTRL:
        /* no break */
        [[fallthrough]];
    case RX_UL_CTRL:
        /* no break*/
        [[fallthrough]];
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot TX while RX.");
        break;
    case TX:
        // No break, gNB may transmit multiple times to multiple UEs
        [[fallthrough]];
    case CCA_BUSY:
        NS_LOG_WARN("Start transmitting DATA while in CCA_BUSY state.");
        /* no break */
        [[fallthrough]];
    case IDLE: {
        NS_ASSERT(m_txPsd);

        ChangeState(TX, duration);

        Ptr<NrSpectrumSignalParametersDataFrame> txParams =
            Create<NrSpectrumSignalParametersDataFrame>();
        txParams->duration = duration;
        txParams->txPhy = this->GetObject<SpectrumPhy>();
        txParams->psd = m_txPsd;
        txParams->packetBurst = pb;
        txParams->cellId = GetCellId();
        txParams->ctrlMsgList = ctrlMsgList;
        txParams->rnti = dci->m_rnti;
        txParams->precodingMatrix = dci->m_precMats;

        /* This section is used for trace */
        if (m_isGnb)
        {
            GnbPhyPacketCountParameter traceParam;
            traceParam.m_noBytes = (txParams->packetBurst) ? txParams->packetBurst->GetSize() : 0;
            traceParam.m_cellId = txParams->cellId;
            traceParam.m_isTx = true;
            traceParam.m_subframeno = 0; // TODO extend this

            m_txPacketTraceGnb(traceParam);
        }

        m_txDataTrace(duration);

        if (m_channel)
        {
            m_channel->StartTx(txParams);
        }
        else
        {
            NS_LOG_WARN("Working without channel (i.e., under test)");
        }

        Simulator::Schedule(duration, &NrSpectrumPhy::EndTx, this);
        m_activeTransmissions++;
    }
    break;
    default:
        NS_LOG_FUNCTION(this << "Programming Error. Code should not reach this point");
    }
}

bool
NrSpectrumPhy::IsTransmitting()
{
    return m_state == TX;
}

void
NrSpectrumPhy::StartTxDlControlFrames(const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                                      const Time& duration)
{
    NS_LOG_FUNCTION(this << duration.As(Time::S));
    NS_LOG_LOGIC(this << " state: " << m_state);

    switch (m_state)
    {
    case RX_DATA:
        /* no break */
    case RX_DL_CTRL:
        /* no break */
    case RX_UL_CTRL:
        /* no break*/
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot TX while RX.");
        break;
    case TX:
        NS_FATAL_ERROR("Cannot TX while already TX.");
        break;
    case CCA_BUSY:
        NS_LOG_WARN("Start transmitting DL CTRL while in CCA_BUSY state.");
        /* no break */
    case IDLE: {
        NS_ASSERT(m_txPsd);
        ChangeState(TX, duration);
        Ptr<NrSpectrumSignalParametersDlCtrlFrame> txParams =
            Create<NrSpectrumSignalParametersDlCtrlFrame>();
        txParams->duration = duration;
        txParams->txPhy = GetObject<SpectrumPhy>();
        txParams->psd = m_txPsd;
        txParams->cellId = GetCellId();
        txParams->pss = true;
        txParams->ctrlMsgList = ctrlMsgList;

        m_txCtrlTrace(duration);
        if (m_channel)
        {
            m_channel->StartTx(txParams);
        }
        else
        {
            NS_LOG_WARN("Working without channel (i.e., under test)");
        }

        Simulator::Schedule(duration, &NrSpectrumPhy::EndTx, this);
        m_activeTransmissions++;
    }
    }
}

void
NrSpectrumPhy::StartTxCsiRs(uint16_t rnti, uint16_t beamId)
{
    NS_LOG_LOGIC(this << " state: " << m_state);
    // we simulate 1ns signals for CSi-RS during DL CTRL duration,
    // the real overhead is correctly calculated in the TB size
    Time duration = NanoSeconds(1);

    switch (m_state)
    {
    case RX_DATA:
        /* no break */
    case RX_DL_CTRL:
        /* no break */
    case RX_UL_CTRL:
        /* no break*/
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot TX while RX.");
        break;
    case TX:
        NS_FATAL_ERROR("Cannot TX while already TX.");
        break;
    case CCA_BUSY:
        NS_LOG_WARN("Start transmitting CSI-RS while in CCA_BUSY state.");
        /* no break */
    case IDLE: {
        NS_ASSERT(m_txPsd);
        Ptr<NrSpectrumSignalParametersCsiRs> csiRs = Create<NrSpectrumSignalParametersCsiRs>();
        csiRs->duration = duration;
        csiRs->txPhy = GetObject<SpectrumPhy>();
        csiRs->psd = m_txPsd;
        csiRs->cellId = GetCellId();
        csiRs->rnti = rnti;
        csiRs->beamId = beamId;

        if (m_channel)
        {
            NS_LOG_DEBUG("gNB with cellId " << GetCellId()
                                            << " transmitting CSI-RS for RNTI:" << csiRs->rnti);
            m_channel->StartTx(csiRs);
        }
        else
        {
            NS_LOG_WARN("Working without channel (i.e., under test)");
        }
    }
    }
}

void
NrSpectrumPhy::StartTxUlControlFrames(const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                                      const Time& duration)
{
    NS_LOG_FUNCTION(this << duration.As(Time::S));
    NS_LOG_LOGIC(this << " state: " << m_state);

    switch (m_state)
    {
    case RX_DATA:
        /* no break */
    case RX_DL_CTRL:
        /* no break */
    case RX_UL_CTRL:
        /* no break */
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot TX while RX.");
        break;
    case TX:
        NS_FATAL_ERROR("Cannot TX while already TX.");
        break;
    case CCA_BUSY:
        NS_LOG_WARN("Start transmitting UL CTRL while in CCA_BUSY state");
        /* no break */
    case IDLE: {
        NS_ASSERT(m_txPsd);
        ChangeState(TX, duration);
        Ptr<NrSpectrumSignalParametersUlCtrlFrame> txParams =
            Create<NrSpectrumSignalParametersUlCtrlFrame>();
        txParams->duration = duration;
        txParams->txPhy = GetObject<SpectrumPhy>();
        txParams->psd = m_txPsd;
        txParams->cellId = GetCellId();
        txParams->ctrlMsgList = ctrlMsgList;

        m_txCtrlTrace(duration);
        if (m_channel)
        {
            m_channel->StartTx(txParams);
        }
        else
        {
            NS_LOG_WARN("Working without channel (i.e., under test)");
        }
        Simulator::Schedule(duration, &NrSpectrumPhy::EndTx, this);
        m_activeTransmissions++;
    }
    }
}

void
NrSpectrumPhy::AddDataPowerChunkProcessor(const Ptr<NrChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this << p);
    m_interferenceData->AddRsPowerChunkProcessor(p);
}

void
NrSpectrumPhy::AddDataSinrChunkProcessor(const Ptr<NrChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceData->AddSinrChunkProcessor(p);
}

void
NrSpectrumPhy::AddSrsSinrChunkProcessor(const Ptr<NrChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_isGnb && m_interferenceSrs,
                  "SRS interference object does not exist or this device is not gNb so the "
                  "function should not be called.");
    m_interferenceSrs->AddSinrChunkProcessor(p);
}

void
NrSpectrumPhy::ReportDlCtrlSinr(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrUePhy> phy = (DynamicCast<NrUePhy>(m_phy));
    NS_ABORT_MSG_UNLESS(
        phy,
        "This function should only be called for NrSpectrumPhy belonging to NrUEPhy");
    phy->ReportDlCtrlSinr(sinr);
}

void
NrSpectrumPhy::UpdateSrsSinrPerceived(const SpectrumValue& srsSinr)
{
    NS_LOG_FUNCTION(this << srsSinr);
    NS_LOG_INFO("Update SRS SINR perceived with this value: " << srsSinr);

    for (auto& srsCallback : m_srsSinrReportCallback)
    {
        srsCallback(GetCellId(),
                    m_currentSrsRnti,
                    Sum(srsSinr) / (srsSinr.GetSpectrumModel()->GetNumBands()));
    }
}

void
NrSpectrumPhy::UpdateSrsSnrPerceived(const double srsSnr)
{
    NS_LOG_FUNCTION(this << srsSnr);
    NS_LOG_INFO("Update SRS SNR perceived with this value: " << srsSnr);

    for (auto& srsSnrCallback : m_srsSnrReportCallback)
    {
        srsSnrCallback(GetCellId(), m_currentSrsRnti, srsSnr);
    }
}

void
NrSpectrumPhy::AddRsPowerChunkProcessor(const Ptr<NrChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceCtrl->AddRsPowerChunkProcessor(p);
}

void
NrSpectrumPhy::AddDlCtrlSinrChunkProcessor(const Ptr<NrChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceCtrl->AddSinrChunkProcessor(p);
}

void
NrSpectrumPhy::UpdateSinrPerceived(const SpectrumValue& sinr)
{
    NS_LOG_FUNCTION(this << sinr);
    NS_LOG_INFO("Update SINR perceived with this value: " << sinr);
    m_sinrPerceived = sinr;
}

void
NrSpectrumPhy::InstallPhy(const Ptr<NrPhy>& phyModel)
{
    m_phy = phyModel;
}

Ptr<NrPhy>
NrSpectrumPhy::GetPhy() const
{
    return m_phy;
}

void
NrSpectrumPhy::SetAntenna(const Ptr<Object> antenna)
{
    m_antenna = antenna;
}

Ptr<SpectrumChannel>
NrSpectrumPhy::GetSpectrumChannel() const
{
    return m_channel;
}

Ptr<NrInterference>
NrSpectrumPhy::GetNrInterference() const
{
    NS_LOG_FUNCTION(this);
    return m_interferenceData;
}

void
NrSpectrumPhy::AddExpectedTb(ExpectedTb expectedTb)
{
    NS_LOG_FUNCTION(this);

    if (!IsGnb())
    {
        NS_ASSERT_MSG(m_hasRnti, "Cannot send TB to a UE whose RNTI has not been set");
        NS_ASSERT_MSG(m_rnti == expectedTb.m_rnti,
                      "RNTI of the receiving UE must match the RNTI of the TB");
    }

    auto it = m_transportBlocks.find(expectedTb.m_rnti);
    if (it != m_transportBlocks.end())
    {
        // might be a TB of an unreceived packet (due to high propagation losses)
        m_transportBlocks.erase(it);
    }

    m_transportBlocks.emplace(expectedTb.m_rnti, expectedTb);
    NS_LOG_INFO("Add expected TB for rnti "
                << expectedTb.m_rnti << " size=" << expectedTb.m_tbSize
                << " mcs=" << static_cast<uint32_t>(expectedTb.m_mcs)
                << " symstart=" << static_cast<uint32_t>(expectedTb.m_symStart)
                << " numSym=" << static_cast<uint32_t>(expectedTb.m_numSym));
}

void
NrSpectrumPhy::AddExpectedSrsRnti(uint16_t rnti)
{
    m_currentSrsRnti = rnti;
}

void
NrSpectrumPhy::AddSrsSinrReportCallback(SrsSinrReportCallback callback)
{
    m_srsSinrReportCallback.push_back(callback);
}

void
NrSpectrumPhy::AddSrsSnrReportCallback(SrsSnrReportCallback callback)
{
    m_srsSnrReportCallback.push_back(callback);
}

// private

void
NrSpectrumPhy::StartRxData(const Ptr<NrSpectrumSignalParametersDataFrame>& params)
{
    NS_LOG_FUNCTION(this);

    m_rxDataTrace(m_phy->GetCurrentSfnSf(),
                  params->psd,
                  params->duration,
                  m_phy->GetBwpId(),
                  m_phy->GetCellId());

    switch (m_state)
    {
    case TX:
        if (m_isGnb) // I am gNB. We are here because some of my rebellious UEs is transmitting
                     // at the same time as me. -> invalid state.
        {
            NS_FATAL_ERROR("gNB transmission overlaps in time with UE transmission. CellId:"
                           << params->cellId);
        }
        else // I am UE, and while I am transmitting, someone else also transmits. If we are
             // transmitting on orthogonal TX PSDs then this is most probably valid situation
             // (UEs transmitting to gNB).
        {
            // Sanity check, that we do not transmit on the same RBs; this sanity check will not
            // be the same for sidelink/V2X
            NS_ASSERT_MSG((Sum((*m_txPsd) * (*params->psd)) == 0),
                          "Transmissions overlap in frequency. Their cellId is:" << params->cellId);
            return;
        }
        break;
    case RX_DL_CTRL:
        /* no break */
    case RX_UL_CTRL:
        /* no break */
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot receive DATA while receiving CTRL.");
        break;
    case CCA_BUSY:
        NS_LOG_INFO("Start receiving DATA while in CCA_BUSY state.");
        /* no break */
    case RX_DATA: // RX_DATA while RX_DATA is possible with OFDMA, i.e. gNB receives from
                  // multiple UEs at the same time
        /* no break */
    case IDLE: {
        m_interferenceData->StartRxMimo(params);

        if (m_rxPacketBurstList.empty())
        {
            NS_ASSERT(m_state == IDLE || m_state == CCA_BUSY);
            // first transmission, i.e., we're IDLE and we start RX
            m_firstRxStart = Simulator::Now();
            m_firstRxDuration = params->duration;
            NS_LOG_LOGIC(this << " scheduling EndRx with delay " << params->duration.GetSeconds()
                              << "s");

            Simulator::Schedule(params->duration, &NrSpectrumPhy::EndRxData, this);
        }
        else
        {
            NS_ASSERT(m_state == RX_DATA);
            // sanity check: if there are multiple RX events, they
            // should occur at the same time and have the same
            // duration, otherwise the interference calculation
            // won't be correct
            NS_ASSERT((m_firstRxStart == Simulator::Now()) &&
                      (m_firstRxDuration == params->duration));
        }

        ChangeState(RX_DATA, params->duration);

        if (params->packetBurst && !params->packetBurst->GetPackets().empty())
        {
            m_rxPacketBurstList.push_back(params->packetBurst);
        }
        // NS_LOG_DEBUG (this << " insert msgs " << params->ctrlMsgList.size ());
        m_rxControlMessageList.insert(m_rxControlMessageList.end(),
                                      params->ctrlMsgList.begin(),
                                      params->ctrlMsgList.end());

        NS_LOG_LOGIC(this << " numSimultaneousRxEvents = " << m_rxPacketBurstList.size());
    }
    break;
    default:
        NS_FATAL_ERROR("Programming Error: Unknown State");
    }
}

void
NrSpectrumPhy::StartRxDlCtrl(const Ptr<NrSpectrumSignalParametersDlCtrlFrame>& params)
{
    // The current code of this function assumes:
    // that this function is called only when cellId = m_cellId, which means
    // that UE can start to receive DL CTRL only from its own cellId,
    // and CTRL from other cellIds will be ignored
    NS_LOG_FUNCTION(this);
    NS_ASSERT(params->cellId == GetCellId() && !m_isGnb);
    // RDF: method currently supports Downlink control only!
    switch (m_state)
    {
    case TX:
        NS_FATAL_ERROR("Cannot RX while TX.");
        break;
    case RX_DATA:
        NS_FATAL_ERROR("Cannot RX CTRL while receiving DATA.");
        break;
    case RX_DL_CTRL:
        NS_FATAL_ERROR("Cannot RX DL CTRL while already receiving DL CTRL.");
        break;
    case RX_UL_CTRL:
        /* no break */
    case RX_UL_SRS:
        NS_FATAL_ERROR("UE should never be in RX_UL_CTRL or RX_UL_SRS state.");
        break;
    case CCA_BUSY:
        NS_LOG_INFO("Start receiving CTRL while channel in CCA_BUSY state.");
        /* no break */
    case IDLE: {
        NS_ASSERT(m_rxControlMessageList.empty());
        NS_LOG_LOGIC(this << "receiving DL CTRL from cellId:" << params->cellId
                          << "and scheduling EndRx with delay " << params->duration);
        // store the DCIs
        m_rxControlMessageList = params->ctrlMsgList;
        Simulator::Schedule(params->duration, &NrSpectrumPhy::EndRxCtrl, this);
        ChangeState(RX_DL_CTRL, params->duration);
        break;
    }
    default: {
        NS_FATAL_ERROR("Unknown state.");
        break;
    }
    }
}

void
NrSpectrumPhy::StartRxUlCtrl(const Ptr<NrSpectrumSignalParametersUlCtrlFrame>& params)
{
    // The current code of this function assumes:
    // 1) that this function is called only when cellId = m_cellId
    // 2) this function should be only called for gNB, only gNB should enter into reception of
    // UL CTRL signals 3) gNB can receive simultaneously signals from various UEs
    NS_LOG_FUNCTION(this);
    NS_ASSERT(params->cellId == GetCellId() && m_isGnb);
    // RDF: method currently supports Uplink control only!
    switch (m_state)
    {
    case TX:
        NS_FATAL_ERROR("Cannot RX UL CTRL while TX.");
        break;
    case RX_DATA:
        NS_FATAL_ERROR("Cannot RX UL CTRL while receiving DATA.");
        break;
    case RX_UL_SRS:
        NS_FATAL_ERROR("Cannot start RX UL CTRL while already receiving SRS.");
        break;
    case RX_DL_CTRL:
        NS_FATAL_ERROR("gNB should not be in RX_DL_CTRL state.");
        break;
    case CCA_BUSY:
        NS_LOG_INFO("Start receiving UL CTRL while channel in CCA_BUSY state.");
        /* no break */
    case RX_UL_CTRL:
        /* no break */
    case IDLE: {
        // at the gNB we can receive more UL CTRL signals simultaneously
        if (m_state == IDLE || m_state == CCA_BUSY)
        {
            // first transmission, i.e., we're IDLE and we start RX
            NS_ASSERT(m_rxControlMessageList.empty());
            m_firstRxStart = Simulator::Now();
            m_firstRxDuration = params->duration;
            NS_LOG_LOGIC(this << " scheduling EndRx with delay " << params->duration);
            // store the DCIs
            m_rxControlMessageList = params->ctrlMsgList;
            Simulator::Schedule(params->duration, &NrSpectrumPhy::EndRxCtrl, this);
            ChangeState(RX_UL_CTRL, params->duration);
        }
        else // already in RX_UL_CTRL state, just add new CTRL messages from other UE
        {
            NS_ASSERT((m_firstRxStart == Simulator::Now()) &&
                      (m_firstRxDuration == params->duration));
            m_rxControlMessageList.insert(m_rxControlMessageList.end(),
                                          params->ctrlMsgList.begin(),
                                          params->ctrlMsgList.end());
        }
        break;
    }
    default: {
        NS_FATAL_ERROR("unknown state");
        break;
    }
    }
}

void
NrSpectrumPhy::StartRxSrs(const Ptr<NrSpectrumSignalParametersUlCtrlFrame>& params)
{
    NS_LOG_FUNCTION(this);
    // The current code of this function assumes:
    // 1) that this function is called only when cellId = m_cellId
    // 2) this function should be only called for gNB, only gNB should enter into reception of
    // UL SRS signals 3) SRS should be received only one at a time, otherwise this function
    // should assert 4) CTRL message list contains only one message and that one is SRS CTRL
    // message
    NS_ASSERT(params->cellId == GetCellId() && m_isGnb && m_state != RX_UL_SRS &&
              params->ctrlMsgList.size() == 1 &&
              (*params->ctrlMsgList.begin())->GetMessageType() == NrControlMessage::SRS);

    switch (m_state)
    {
    case TX:
        NS_FATAL_ERROR("Cannot RX SRS while TX.");
        break;
    case RX_DATA:
        NS_FATAL_ERROR("Cannot RX SRS while receiving DATA.");
        break;
    case RX_DL_CTRL:
        NS_FATAL_ERROR("gNB should not be in RX_DL_CTRL state.");
        break;
    case RX_UL_CTRL:
        NS_FATAL_ERROR(
            "gNB should not receive simultaneously non SRS and SRS uplink control signals");
        break;
    case CCA_BUSY:
        NS_LOG_INFO("Start receiving UL SRS while channel in CCA_BUSY state.");
        /* no break */
    case IDLE: {
        // at the gNB we can receive only one SRS at a time, and the only allowed states before
        // starting it are IDLE or BUSY
        m_interferenceSrs->StartRxMimo(params);
        // first transmission, i.e., we're IDLE and we start RX, CTRL message list should be
        // empty
        NS_ASSERT(m_rxControlMessageList.empty());
        m_firstRxStart = Simulator::Now();
        m_firstRxDuration = params->duration;
        NS_LOG_LOGIC(this << " scheduling EndRx for SRS signal reception with delay "
                          << params->duration);
        // store the SRS message in the CTRL message list
        m_rxControlMessageList = params->ctrlMsgList;
        Simulator::Schedule(params->duration, &NrSpectrumPhy::EndRxSrs, this);
        ChangeState(RX_UL_SRS, params->duration);
    }
    break;
    default: {
        // not allowed state for starting the SRS reception
        NS_FATAL_ERROR("Not allowed state for starting SRS reception.");
        break;
    }
    }
}

uint16_t
NrSpectrumPhy::GetCellId() const
{
    return m_phy->GetCellId();
}

uint16_t
NrSpectrumPhy::GetBwpId() const
{
    return m_phy->GetBwpId();
}

bool
NrSpectrumPhy::IsGnb() const
{
    return m_isGnb;
}

void
NrSpectrumPhy::SetIsGnb(bool isGnb)
{
    m_isGnb = isGnb;
}

void
NrSpectrumPhy::ChangeState(State newState, Time duration)
{
    NS_LOG_LOGIC(this << " change state: " << m_state << " -> " << newState);
    m_state = newState;

    if (newState == RX_DATA || newState == RX_DL_CTRL || newState == RX_UL_CTRL || newState == TX ||
        newState == CCA_BUSY)
    {
        m_channelOccupied(duration);
    }
}

void
NrSpectrumPhy::EndTx()
{
    NS_LOG_FUNCTION(this);

    // In case of OFDMA DL, this function will be called multiple times, after each transmission
    // to a different UE. In the first call to this function, m_state is changed to IDLE.
    NS_ASSERT_MSG(m_state == TX, "In EndTx() but state is not TX; state: " << m_state);
    NS_LOG_DEBUG("Number of active transmissions (before decrement): " << m_activeTransmissions);
    NS_ASSERT_MSG(m_activeTransmissions, "Ending Tx but no active transmissions");
    m_activeTransmissions--;

    // change to BUSY or IDLE mode when this is the end of the last transmission
    if (m_activeTransmissions == 0)
    {
        // if in unlicensed mode check after transmission if we are in IDLE or CCA_BUSY mode
        if (m_unlicensedMode)
        {
            MaybeCcaBusy();
        }
        else
        {
            ChangeState(IDLE, Seconds(0));
        }
    }
}

std::vector<MimoSinrChunk>
NrSpectrumPhy::GetMimoSinrForRnti(uint16_t rnti, uint8_t rank)
{
    // Filter chunks by RNTI of the expected TB. For DL, this step selects only the RX signals
    // that were sent towards this UE. For UL, it selects only signals that were sent from the
    // UE that is currently being decoded.
    std::vector<MimoSinrChunk> res;
    for (const auto& chunk : m_mimoSinrPerceived)
    {
        if (chunk.rnti == rnti)
        {
            res.emplace_back(chunk);
        }
    }
    if (res.empty())
    {
        // No received signal found, create all-zero SINR matrix with minimum duration
        NS_LOG_WARN("Did not find any SINR matrix matching the current UE's RNTI " << rnti);
        auto sinrMat = NrSinrMatrix{rank, m_rxSpectrumModel->GetNumBands()};
        auto dur = NanoSeconds(1);
        res.emplace_back(MimoSinrChunk{sinrMat, rnti, dur});
    }
    return res;
}

void
TransportBlockInfo::UpdatePerceivedSinr(const SpectrumValue& perceivedSinr)
{
    m_sinrAvg = 0.0;
    m_sinrMin = 99999999999;
    for (const auto& rbIndex : m_expected.m_rbBitmap)
    {
        m_sinrAvg += perceivedSinr.ValuesAt(rbIndex);
        if (perceivedSinr.ValuesAt(rbIndex) < m_sinrMin)
        {
            m_sinrMin = perceivedSinr.ValuesAt(rbIndex);
        }
    }

    m_sinrAvg = m_sinrAvg / m_expected.m_rbBitmap.size();

    NS_LOG_INFO("Finishing RX, sinrAvg=" << m_sinrAvg << " sinrMin=" << m_sinrMin
                                         << " SinrAvg (dB) " << 10 * log(m_sinrAvg) / log(10));
}

void
NrSpectrumPhy::CheckTransportBlockCorruptionStatus()
{
    for (auto& tbIt : m_transportBlocks)
    {
        auto rnti = tbIt.first;
        auto& tbInfo = tbIt.second;

        tbInfo.UpdatePerceivedSinr(m_sinrPerceived);

        if ((!m_dataErrorModelEnabled) || (m_rxPacketBurstList.empty()))
        {
            continue;
        }

        const NrErrorModel::NrErrorModelHistory& harqInfoList =
            m_harqPhyModule.GetHarqProcessInfoDlUl(tbInfo.m_expected.m_isDownlink,
                                                   rnti,
                                                   tbInfo.m_expected.m_harqProcessId);

        NS_ABORT_MSG_IF(!m_errorModelType.IsChildOf(NrErrorModel::GetTypeId()),
                        "The error model must be a child of NrErrorModel");

        if (!m_errorModel)
        {
            ObjectFactory emFactory;
            emFactory.SetTypeId(m_errorModelType);
            m_errorModel = DynamicCast<NrErrorModel>(emFactory.Create());
            NS_ABORT_IF(m_errorModel == nullptr);
        }

        // Output is the output of the error model. From the TBLER we decide
        // if the entire TB is corrupted or not

        if (!m_mimoSinrPerceived.empty())
        {
            // The received signal information supports MIMO
            const auto& expectedTb = tbInfo.m_expected;
            auto sinrChunks = GetMimoSinrForRnti(expectedTb.m_rnti, expectedTb.m_rank);
            NS_ASSERT(!sinrChunks.empty());

            tbInfo.m_outputOfEM = m_errorModel->GetTbDecodificationStatsMimo(sinrChunks,
                                                                             expectedTb.m_rbBitmap,
                                                                             expectedTb.m_tbSize,
                                                                             expectedTb.m_mcs,
                                                                             expectedTb.m_rank,
                                                                             harqInfoList);
        }
        else
        {
            // SISO code, required only when there is no NrMimoChunkProcessor
            // TODO: change nr-uplink-power-control-test to create a 3gpp channel, and remove this
            // code
            tbInfo.m_outputOfEM =
                m_errorModel->GetTbDecodificationStats(m_sinrPerceived,
                                                       tbInfo.m_expected.m_rbBitmap,
                                                       tbInfo.m_expected.m_tbSize,
                                                       tbInfo.m_expected.m_mcs,
                                                       harqInfoList);
        }

        tbInfo.m_isCorrupted = m_random->GetValue() <= tbInfo.m_outputOfEM->m_tbler;

        if (tbInfo.m_isCorrupted)
        {
            NS_LOG_INFO(
                "RNTI " << rnti << " processId " << +tbInfo.m_expected.m_harqProcessId << " size "
                        << tbInfo.m_expected.m_tbSize << " mcs "
                        << (uint32_t)tbInfo.m_expected.m_mcs << "rank" << +tbInfo.m_expected.m_rank
                        << " bitmap " << tbInfo.m_expected.m_rbBitmap.size()
                        << " rv from MAC: " << +tbInfo.m_expected.m_rv
                        << " elements in the history: " << harqInfoList.size() << " TBLER "
                        << tbInfo.m_outputOfEM->m_tbler << " corrupted " << tbInfo.m_isCorrupted);
        }
    }
}

void
NrSpectrumPhy::SendUlHarqFeedback(uint16_t rnti, TransportBlockInfo& tbInfo)
{
    // Generate the feedback
    UlHarqInfo harqUlInfo;
    harqUlInfo.m_rnti = rnti;
    harqUlInfo.m_tpc = 0;
    harqUlInfo.m_harqProcessId = tbInfo.m_expected.m_harqProcessId;
    harqUlInfo.m_numRetx = tbInfo.m_expected.m_rv;
    if (tbInfo.m_isCorrupted)
    {
        harqUlInfo.m_receptionStatus = UlHarqInfo::NotOk;
    }
    else
    {
        harqUlInfo.m_receptionStatus = UlHarqInfo::Ok;
    }

    // Send the feedback
    m_phyUlHarqFeedbackCallback(harqUlInfo);

    // Arrange the history
    if (!tbInfo.m_isCorrupted || tbInfo.m_expected.m_rv == 3)
    {
        m_harqPhyModule.ResetUlHarqProcessStatus(rnti, tbInfo.m_expected.m_harqProcessId);
    }
    else
    {
        m_harqPhyModule.UpdateUlHarqProcessStatus(rnti,
                                                  tbInfo.m_expected.m_harqProcessId,
                                                  tbInfo.m_outputOfEM);
    }
}

DlHarqInfo
NrSpectrumPhy::SendDlHarqFeedback(uint16_t rnti, TransportBlockInfo& tbInfo)
{
    // Generate the feedback
    DlHarqInfo harqDlInfo;
    harqDlInfo.m_rnti = rnti;
    harqDlInfo.m_harqProcessId = tbInfo.m_expected.m_harqProcessId;
    harqDlInfo.m_numRetx = tbInfo.m_expected.m_rv;
    harqDlInfo.m_bwpIndex = GetBwpId();
    if (tbInfo.m_isCorrupted)
    {
        harqDlInfo.m_harqStatus = DlHarqInfo::NACK;
    }
    else
    {
        harqDlInfo.m_harqStatus = DlHarqInfo::ACK;
    }

    // Send the feedback
    m_phyDlHarqFeedbackCallback(harqDlInfo);

    // Arrange the history
    if (!tbInfo.m_isCorrupted || tbInfo.m_expected.m_rv == 3)
    {
        NS_LOG_DEBUG("Reset Dl process: " << +tbInfo.m_expected.m_harqProcessId << " for RNTI "
                                          << rnti);
        m_harqPhyModule.ResetDlHarqProcessStatus(rnti, tbInfo.m_expected.m_harqProcessId);
    }
    else
    {
        NS_LOG_DEBUG("Update Dl process: " << +tbInfo.m_expected.m_harqProcessId << " for RNTI "
                                           << rnti);
        m_harqPhyModule.UpdateDlHarqProcessStatus(rnti,
                                                  tbInfo.m_expected.m_harqProcessId,
                                                  tbInfo.m_outputOfEM);
    }
    return harqDlInfo;
}

void
NrSpectrumPhy::ProcessReceivedPacketBurst()
{
    NS_LOG_FUNCTION(this);
    Ptr<NrGnbNetDevice> gnbRx = DynamicCast<NrGnbNetDevice>(GetDevice());
    Ptr<NrUeNetDevice> ueRx = DynamicCast<NrUeNetDevice>(GetDevice());
    std::map<uint16_t, DlHarqInfo> harqDlInfoMap;
    for (auto packetBurst : m_rxPacketBurstList)
    {
        for (auto packet : packetBurst->GetPackets())
        {
            if (packet->GetSize() == 0)
            {
                continue;
            }

            NrRadioBearerTag bearerTag;
            if (!packet->PeekPacketTag(bearerTag))
            {
                NS_FATAL_ERROR("No radio bearer tag found");
            }
            uint16_t rnti = bearerTag.GetRnti();

            auto itTb = m_transportBlocks.find(rnti);
            if (itTb == m_transportBlocks.end())
            {
                // Packet for other device...
                continue;
            }
            auto& tbInfo = itTb->second;

            if (!tbInfo.m_isCorrupted)
            {
                m_phyRxDataEndOkCallback(packet);
            }
            else
            {
                NS_LOG_INFO("TB failed");
            }

            if (gnbRx)
            {
                RxPacketTraceParams traceParams(tbInfo,
                                                m_dataErrorModelEnabled,
                                                rnti,
                                                gnbRx->GetCellId(),
                                                GetBwpId(),
                                                255);
                m_rxPacketTraceGnb(traceParams);
            }
            else if (ueRx)
            {
                Ptr<NrUePhy> phy = (DynamicCast<NrUePhy>(m_phy));
                uint8_t cqi = phy->ComputeCqi(m_sinrPerceived);
                RxPacketTraceParams traceParams(tbInfo,
                                                m_dataErrorModelEnabled,
                                                rnti,
                                                ueRx->GetTargetGnb()->GetCellId(),
                                                GetBwpId(),
                                                cqi);
                m_rxPacketTraceUe(traceParams);
            }

            // send HARQ feedback (if not already done for this TB)
            if (!tbInfo.m_harqFeedbackSent)
            {
                tbInfo.m_harqFeedbackSent = true;
                if (tbInfo.m_expected.m_isDownlink) // DL TB
                {
                    NS_ASSERT(harqDlInfoMap.find(rnti) == harqDlInfoMap.end());
                    auto harqDlInfo = SendDlHarqFeedback(rnti, tbInfo);
                    harqDlInfoMap.insert(std::make_pair(rnti, harqDlInfo));
                }
                else
                {
                    SendUlHarqFeedback(rnti, tbInfo);
                }
            }
        }
    }
}

void
NrSpectrumPhy::EndRxData()
{
    NS_LOG_FUNCTION(this);
    m_interferenceData->EndRx();

    NS_ASSERT(m_state == RX_DATA);

    // check if transport blocks are corrupted
    CheckTransportBlockCorruptionStatus();

    // trace packet bursts, then receive non-corrupted and send harq feedback
    ProcessReceivedPacketBurst();

    // forward control messages of this frame to NrPhy
    if (!m_rxControlMessageList.empty() && m_phyRxCtrlEndOkCallback)
    {
        m_phyRxCtrlEndOkCallback(m_rxControlMessageList, GetBwpId());
    }

    // if in unlicensed mode check after reception if the state should be
    // changed to IDLE or CCA_BUSY
    if (m_unlicensedMode)
    {
        MaybeCcaBusy();
    }
    else
    {
        ChangeState(IDLE, Seconds(0));
    }

    m_rxPacketBurstList.clear();
    m_transportBlocks.clear();
    m_rxControlMessageList.clear();
}

void
NrSpectrumPhy::EndRxCtrl()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_state == RX_DL_CTRL || m_state == RX_UL_CTRL);

    m_interferenceCtrl->EndRx();

    // control error model not supported
    // forward control messages of this frame to NrPhy
    if (!m_rxControlMessageList.empty())
    {
        if (m_phyRxCtrlEndOkCallback)
        {
            m_phyRxCtrlEndOkCallback(m_rxControlMessageList, GetBwpId());
        }
    }

    // if in unlicensed mode check after reception if we are in IDLE or CCA_BUSY mode
    if (m_unlicensedMode)
    {
        MaybeCcaBusy();
    }
    else
    {
        ChangeState(IDLE, Seconds(0));
    }

    m_rxControlMessageList.clear();
}

void
NrSpectrumPhy::EndRxSrs()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_state == RX_UL_SRS && m_rxControlMessageList.size() == 1);

    // notify interference calculator that the reception of SRS is finished,
    // so that chunk processors can be notified to calculate SINR, and if other
    // processor is registered
    m_interferenceSrs->EndRx();

    if (m_phyRxCtrlEndOkCallback)
    {
        m_phyRxCtrlEndOkCallback(m_rxControlMessageList, GetBwpId());
    }

    // if in unlicensed mode check after reception if we are in IDLE or CCA_BUSY mode
    if (m_unlicensedMode)
    {
        MaybeCcaBusy();
    }
    else
    {
        ChangeState(IDLE, Seconds(0));
    }

    m_rxControlMessageList.clear();
}

void
NrSpectrumPhy::MaybeCcaBusy()
{
    NS_LOG_FUNCTION(this);
    Time delayUntilCcaEnd = m_interferenceData->GetEnergyDuration(m_ccaMode1ThresholdW);
    if (!delayUntilCcaEnd.IsZero())
    {
        NS_LOG_DEBUG("Channel detected BUSY for:" << delayUntilCcaEnd << " ns.");

        ChangeState(CCA_BUSY, delayUntilCcaEnd);

        // check if with the new energy the channel will be for longer time in CCA_BUSY
        if (m_busyTimeEnds < Simulator::Now() + delayUntilCcaEnd)
        {
            m_busyTimeEnds = Simulator::Now() + delayUntilCcaEnd;

            if (m_checkIfIsIdleEvent.IsPending())
            {
                m_checkIfIsIdleEvent.Cancel();
            }

            NS_LOG_DEBUG("Check if still BUSY in:" << delayUntilCcaEnd
                                                   << " us, and that is at "
                                                      " time:"
                                                   << Simulator::Now() + delayUntilCcaEnd
                                                   << " and current time is:" << Simulator::Now());

            m_checkIfIsIdleEvent =
                Simulator::Schedule(delayUntilCcaEnd, &NrSpectrumPhy::CheckIfStillBusy, this);
        }
    }
    else
    {
        NS_ABORT_MSG_IF(m_checkIfIsIdleEvent.IsPending(),
                        "Unexpected state: returning to IDLE while there is an event "
                        "running that should switch from CCA_BUSY to IDLE ?!");
        NS_LOG_DEBUG("Channel detected IDLE after being in: " << m_state << " state.");
        ChangeState(IDLE, Seconds(0));
    }
}

void
NrSpectrumPhy::CheckIfStillBusy()
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(m_state == IDLE, "This function should not be called when in IDLE state.");
    // If in state of RX/TX do not switch to CCA_BUSY until RX/TX is finished.
    // When RX/TX finishes, check if the channel is still busy.
    if (m_state == CCA_BUSY)
    {
        MaybeCcaBusy();
    }
    else // RX_DL_CTRL, RX_UL_CTRL, RX_DATA, TX
    {
        Time delayUntilCcaEnd = m_interferenceData->GetEnergyDuration(m_ccaMode1ThresholdW);

        if (delayUntilCcaEnd.IsZero())
        {
            NS_LOG_INFO(" Channel found IDLE as expected.");
        }
        else
        {
            NS_LOG_INFO(" Wait while channel BUSY for: " << delayUntilCcaEnd << " ns.");
        }
    }
}

bool
NrSpectrumPhy::IsOnlySrs(const std::list<Ptr<NrControlMessage>>& ctrlMsgList)
{
    NS_ASSERT_MSG(!ctrlMsgList.empty(), "Passed an empty uplink control list");

    return ctrlMsgList.size() == 1 &&
           (*ctrlMsgList.begin())->GetMessageType() == NrControlMessage::SRS;
}

void
NrSpectrumPhy::ReportWbDlDataSnrPerceived(const double dlDataSnr)
{
    NS_LOG_FUNCTION(this << dlDataSnr);

    Ptr<NrUeNetDevice> ueNetDevice = DynamicCast<NrUeNetDevice>(GetDevice());

    m_dlDataSnrTrace(m_phy->GetCurrentSfnSf(),
                     m_phy->GetCellId(),
                     m_phy->GetBwpId(),
                     ueNetDevice->GetImsi(),
                     dlDataSnr);
}

int64_t
NrSpectrumPhy::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_random->SetStream(stream);
    return 1;
}

void
NrSpectrumPhy::UpdateMimoSinrPerceived(const std::vector<MimoSinrChunk>& mimoChunks)
{
    m_mimoSinrPerceived = mimoChunks;
}

void
NrSpectrumPhy::AddDataMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceData->AddMimoChunkProcessor(p);
}

void
NrSpectrumPhy::AddCsiRsMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceCsiRs->AddMimoChunkProcessor(p);
}

void
NrSpectrumPhy::AddCsiImMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p)
{
    NS_LOG_FUNCTION(this);
    m_interferenceCsiIm->AddMimoChunkProcessor(p);
}

void
NrSpectrumPhy::StartRxCsiRs(const Ptr<NrSpectrumSignalParametersCsiRs>& csiRsParams)
{
    // TODO extend in future to support a predefined set of beams
    if (csiRsParams->rnti == m_rnti)
    {
        // add this signal to all the signals
        m_interferenceCsiRs->AddSignalMimo(csiRsParams, csiRsParams->duration);
        // declare the start of the reception of the CSI-RS signal
        m_interferenceCsiRs->StartRxMimo(csiRsParams);
        // declare the end of the reception of the CSI-RS signal
        Simulator::Schedule(csiRsParams->duration, &NrInterference::EndRx, m_interferenceCsiRs);

        Simulator::Schedule(m_ctrlEndTime - Simulator::Now() + NanoSeconds(2),
                            &NrSpectrumPhy::CheckIfCsiImNeeded,
                            this,
                            csiRsParams);
    }
}

void
NrSpectrumPhy::CheckIfCsiImNeeded(const Ptr<NrSpectrumSignalParametersCsiRs>& csiRsParams)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrUePhy> nrUePhy = DynamicCast<NrUePhy>(m_phy);
    bool pdschCsiEnabled = nrUePhy->GetCsiFeedbackType() & CsiFeedbackFlag::CQI_PDSCH_MIMO;

    // CSI-IM enabled
    if (m_interferenceCsiIm->IsChunkProcessorSet())
    {
        // Schedule only CSI-IM if either PDSCH is disabled, or it is enabled but the UE is not
        // scheduled in this slot
        if ((!pdschCsiEnabled) || (pdschCsiEnabled && !IsUeScheduled()))
        {
            ScheduleCsiIm(csiRsParams);
        }
    }
    else // CSI-IM not enabled
    {
        // if CSI-IM and PDSCH are both disable, generate CSI based on CSI-RS
        if (!pdschCsiEnabled)
        {
            nrUePhy->GenerateCsiRsCqi();
        }
        // else CSI will be generated once PDSCH is being received
    }
}

void
NrSpectrumPhy::ScheduleCsiIm(Ptr<SpectrumSignalParameters> csiRsParams) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_interferenceCsiIm);
    // add fake signal to trigger NrInterference calculation for the duration of the fake CSI-IM
    // signal, simulating device wake-up triggered by CSI-RS for interference measurement of channel
    Ptr<SpectrumSignalParameters> fakeCsiImSignal = Create<SpectrumSignalParameters>();
    fakeCsiImSignal->duration =
        m_phy->GetSymbolPeriod() * (DynamicCast<NrUePhy>(m_phy))->GetCsiImDuration();
    fakeCsiImSignal->psd = csiRsParams->psd;
    fakeCsiImSignal->spectrumChannelMatrix = csiRsParams->spectrumChannelMatrix;
    m_interferenceCsiIm->AddSignalMimo(fakeCsiImSignal, fakeCsiImSignal->duration);
    m_interferenceCsiIm->StartRxMimo(fakeCsiImSignal);
    // schedule NrInterference event when CSI-IM ends to calculate pass the CSI-IM interference
    // to the corresponding callback function
    Simulator::Schedule(fakeCsiImSignal->duration, &NrInterference::EndRx, m_interferenceCsiIm);
}

bool
NrSpectrumPhy::IsUeScheduled() const
{
    return m_transportBlocks.contains(m_rnti);
}

void
NrSpectrumPhy::AddExpectedDlCtrlEnd(Time ctrlEndTime)
{
    m_ctrlEndTime = ctrlEndTime;
}

} // namespace ns3
