// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-interference.h"

#include "nr-mimo-chunk-processor.h"
#include "nr-spectrum-signal-parameters.h"

#include <ns3/log.h>
#include <ns3/simulator.h>

#include <algorithm>

NS_LOG_COMPONENT_DEFINE("NrInterference");

namespace ns3
{

NrInterference::NrInterference()
    : NrInterferenceBase(),
      m_firstPower(0.0)
{
    NS_LOG_FUNCTION(this);
}

NrInterference::~NrInterference()
{
    NS_LOG_FUNCTION(this);
}

void
NrInterference::DoDispose()
{
    NS_LOG_FUNCTION(this);

    m_mimoChunkProcessors.clear();
    m_rxSignalsMimo.clear();
    m_allSignalsMimo.clear();

    NrInterferenceBase::DoDispose();
}

TypeId
NrInterference::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrInterference")
            .SetParent<Object>()
            .AddTraceSource("SnrPerProcessedChunk",
                            "Snr per processed chunk.",
                            MakeTraceSourceAccessor(&NrInterference::m_snrPerProcessedChunk),
                            "ns3::SnrPerProcessedChunk::TracedCallback")
            .AddTraceSource("RssiPerProcessedChunk",
                            "Rssi per processed chunk.",
                            MakeTraceSourceAccessor(&NrInterference::m_rssiPerProcessedChunk),
                            "ns3::RssiPerProcessedChunk::TracedCallback");
    return tid;
}

void
NrInterference::AddSignal(Ptr<const SpectrumValue> spd, Time duration)
{
    NS_LOG_FUNCTION(this << *spd << duration);

    // Integrate over our receive bandwidth.
    // Note that differently from wifi, we do not need to pass the
    // signal through the filter. This is because
    // before receiving the signal already passed through the
    // spectrum converter, thus we will consider only the power over the
    // spectrum that corresponds to the spectrum of the receiver.
    // Also, differently from wifi we do not account here for the antenna gain,
    // since this is already taken into account by the spectrum channel.
    double rxPowerW = Integral(*spd);
    // We are creating two events, one that adds the rxPowerW, and
    // another that subtracts the rxPowerW at the endTime.
    // These events will be used to determine if the channel is busy and
    // for how long.
    AppendEvent(Simulator::Now(), Simulator::Now() + duration, rxPowerW);

    NrInterferenceBase::AddSignal(spd, duration);
}

void
NrInterference::EndRx()
{
    NS_LOG_FUNCTION(this);
    if (!m_receiving)
    {
        NS_LOG_INFO("EndRx was already evaluated or RX was aborted");
    }
    else
    {
        SpectrumValue snr = (*m_rxSignal) / (*m_noise);
        double avgSnr = Sum(snr) / (snr.GetSpectrumModel()->GetNumBands());
        m_snrPerProcessedChunk(avgSnr);

        NrInterference::ConditionallyEvaluateChunk();

        m_receiving = false;
        for (auto& it : m_rsPowerChunkProcessorList)
        {
            it->End();
        }
        for (auto& it : m_interfChunkProcessorList)
        {
            it->End();
        }
        for (auto& it : m_sinrChunkProcessorList)
        {
            it->End();
        }

        for (auto& cp : m_mimoChunkProcessors)
        {
            cp->End();
        }
    }
}

void
NrInterference::ConditionallyEvaluateChunk()
{
    NS_LOG_FUNCTION(this);
    if (m_receiving)
    {
        NS_LOG_DEBUG(this << " Receiving");
    }
    NS_LOG_DEBUG(this << " now " << Now() << " last " << m_lastChangeTime);
    if (m_receiving && (Now() > m_lastChangeTime))
    {
        NS_LOG_LOGIC(this << " signal = " << *m_rxSignal << " allSignals = " << *m_allSignals
                          << " noise = " << *m_noise);
        SpectrumValue interf = (*m_allSignals) - (*m_rxSignal) + (*m_noise);
        SpectrumValue sinr = (*m_rxSignal) / interf;
        double rbWidth = (*m_rxSignal).GetSpectrumModel()->Begin()->fh -
                         (*m_rxSignal).GetSpectrumModel()->Begin()->fl;
        double rssidBm = 10 * log10(Sum((*m_noise + *m_allSignals) * rbWidth) * 1000);
        m_rssiPerProcessedChunk(rssidBm);

        NS_LOG_DEBUG("All signals: " << (*m_allSignals)[0] << ", rxSignal:" << (*m_rxSignal)[0]
                                     << " , noise:" << (*m_noise)[0]);

        Time duration = Now() - m_lastChangeTime;
        for (auto& it : m_rsPowerChunkProcessorList)
        {
            it->EvaluateChunk(*m_rxSignal, duration);
        }
        for (auto& it : m_sinrChunkProcessorList)
        {
            it->EvaluateChunk(sinr, duration);
        }

        for (auto& cp : m_mimoChunkProcessors)
        {
            // Covariance matrix of noise plus out-of-cell interference
            auto outOfCellInterfCov = CalcOutOfCellInterfCov();

            // Compute the MIMO SINR separately for each received signal.
            for (auto& rxSignal : m_rxSignalsMimo)
            {
                // Use the UE's RNTI to distinguish multiple received signals
                auto nrRxSignal = DynamicCast<const NrSpectrumSignalParametersDataFrame>(rxSignal);
                uint16_t rnti = nrRxSignal ? nrRxSignal->rnti : 0;

                // MimoSinrChunk is used to store SINR and compute TBLER of the data transmission
                auto sinrMatrix = ComputeSinr(outOfCellInterfCov, rxSignal);
                MimoSinrChunk mimoSinr{sinrMatrix, rnti, duration};
                cp->EvaluateChunk(mimoSinr);

                // MimoSignalChunk is used to compute PMI feedback.
                auto& chanSpct = *(rxSignal->spectrumChannelMatrix);
                MimoSignalChunk mimoSignal{chanSpct, outOfCellInterfCov, rnti, duration};
                cp->EvaluateChunk(mimoSignal);
            }
        }
        m_lastChangeTime = Now();
    }
}

/****************************************************************
 *       Class which records SNIR change events for a
 *       short period of time.
 ****************************************************************/

NrInterference::NiChange::NiChange(Time time, double delta)
    : m_time(time),
      m_delta(delta)
{
}

Time
NrInterference::NiChange::GetTime() const
{
    return m_time;
}

double
NrInterference::NiChange::GetDelta() const
{
    return m_delta;
}

bool
NrInterference::NiChange::operator<(const NrInterference::NiChange& o) const
{
    return (m_time < o.m_time);
}

bool
NrInterference::IsChannelBusyNow(double energyW)
{
    double detectedPowerW = Integral(*m_allSignals);
    double powerDbm = 10 * log10(detectedPowerW * 1000);

    NS_LOG_INFO("IsChannelBusyNow detected power is: "
                << powerDbm << "  detectedPowerW: " << detectedPowerW << " length spectrum: "
                << (*m_allSignals).GetValuesN() << " thresholdW:" << energyW);

    if (detectedPowerW > energyW)
    {
        NS_LOG_INFO("Channel is BUSY.");
        return true;
    }
    else
    {
        NS_LOG_INFO("Channel is IDLE.");
        return false;
    }
}

Time
NrInterference::GetEnergyDuration(double energyW)
{
    if (!IsChannelBusyNow(energyW))
    {
        return Seconds(0);
    }

    Time now = Simulator::Now();
    double noiseInterferenceW = 0.0;
    Time end = now;
    noiseInterferenceW = m_firstPower;

    NS_LOG_INFO("First power: " << m_firstPower);

    for (const auto& i : m_niChanges)
    {
        noiseInterferenceW += i.GetDelta();
        end = i.GetTime();
        NS_LOG_INFO("Delta: " << i.GetDelta() << "time: " << i.GetTime());
        if (end < now)
        {
            continue;
        }
        if (noiseInterferenceW < energyW)
        {
            break;
        }
    }

    NS_LOG_INFO("Future power dBm:" << 10 * log10(noiseInterferenceW * 1000)
                                    << " W:" << noiseInterferenceW
                                    << " and energy threshold in W is: " << energyW);

    if (end > now)
    {
        NS_LOG_INFO("Channel BUSY until." << end);
    }
    else
    {
        NS_LOG_INFO("Channel IDLE.");
    }

    return end > now ? end - now : MicroSeconds(0);
}

void
NrInterference::EraseEvents()
{
    m_niChanges.clear();
    m_firstPower = 0.0;
}

NrInterference::NiChanges::iterator
NrInterference::GetPosition(Time moment)
{
    return std::upper_bound(m_niChanges.begin(), m_niChanges.end(), NiChange(moment, 0));
}

void
NrInterference::AddNiChangeEvent(NiChange change)
{
    m_niChanges.insert(GetPosition(change.GetTime()), change);
}

void
NrInterference::AppendEvent(Time startTime, Time endTime, double rxPowerW)
{
    Time now = Simulator::Now();

    if (!m_receiving)
    {
        NiChanges::iterator nowIterator = GetPosition(now);
        // We empty the list until the current moment. To do so we
        // first we sum all the energies until the current moment
        // and save it in m_firstPower.
        for (NiChanges::iterator i = m_niChanges.begin(); i != nowIterator; i++)
        {
            m_firstPower += i->GetDelta();
        }
        // then we remove all the events up to the current moment
        m_niChanges.erase(m_niChanges.begin(), nowIterator);
        // we create an event that represents the new energy
        m_niChanges.insert(m_niChanges.begin(), NiChange(startTime, rxPowerW));
    }
    else
    {
        // for the startTime create the event that adds the energy
        AddNiChangeEvent(NiChange(startTime, rxPowerW));
    }

    // for the endTime create event that will subtract energy
    AddNiChangeEvent(NiChange(endTime, -rxPowerW));
}

void
NrInterference::AddSignalMimo(Ptr<const SpectrumSignalParameters> params, const Time& duration)
{
    NS_LOG_FUNCTION(this << *params->psd << duration);
    auto rxPowerW = Integral(*params->psd);

    NS_LOG_FUNCTION(this << *params->psd << duration);
    NrInterferenceBase::DoAddSignal(params->psd);
    m_allSignalsMimo.push_back(params);
    // Update signal ID to match signal ID in NrInterferenceBase
    if (++m_lastSignalId == m_lastSignalIdBeforeReset)
    {
        m_lastSignalIdBeforeReset += NR_LTE_SIGNALID_INCR;
    }
    Simulator::Schedule(duration,
                        &NrInterference::DoSubtractSignalMimo,
                        this,
                        params,
                        m_lastSignalId);

    AppendEvent(Simulator::Now(), Simulator::Now() + duration, rxPowerW);
}

void
NrInterference::StartRxMimo(Ptr<const SpectrumSignalParameters> params)
{
    auto rxPsd = params->psd;
    if (!m_receiving)
    {
        // This must be the first receive signal, clear any lingering previous signals
        m_rxSignalsMimo.clear();
    }
    m_rxSignalsMimo.push_back(params);
    for (auto& cp : m_mimoChunkProcessors)
    {
        // Clear the list of stored chunks
        cp->Start();
    }
    NrInterferenceBase::StartRx(rxPsd);
}

void
NrInterference::DoSubtractSignalMimo(Ptr<const SpectrumSignalParameters> params, uint32_t signalId)
{
    DoSubtractSignal(params->psd, signalId);
    auto numSignals = m_allSignalsMimo.size();
    // In many instances the signal subtracted is the last signal. Check first for speedup.
    if (m_allSignalsMimo.back() == params)
    {
        m_allSignalsMimo.pop_back();
    }
    else
    {
        m_allSignalsMimo.erase(
            std::remove_if(m_allSignalsMimo.begin(),
                           m_allSignalsMimo.end(),
                           [params](Ptr<const SpectrumSignalParameters> p) { return p == params; }),
            m_allSignalsMimo.end());
    }
    NS_ASSERT_MSG(m_allSignalsMimo.size() == (numSignals - 1),
                  "MIMO signal was not found for removal");
}

void
NrInterference::AddMimoChunkProcessor(Ptr<NrMimoChunkProcessor> cp)
{
    NS_LOG_FUNCTION(this << cp);
    m_mimoChunkProcessors.push_back(cp);
}

bool
NrInterference::IsChunkProcessorSet()
{
    return (!m_mimoChunkProcessors.empty());
}

NrCovMat
NrInterference::CalcOutOfCellInterfCov() const
{
    // Extract dimensions from first receive signal. Interference signals have equal dimensions
    NS_ASSERT_MSG(!(m_rxSignalsMimo.empty()), "At least one receive signal is required");
    const auto& firstSignal = m_rxSignalsMimo[0];
    NS_ASSERT_MSG(firstSignal->spectrumChannelMatrix, "signal must have a channel matrix");
    auto nRbs = firstSignal->spectrumChannelMatrix->GetNumPages();
    auto nRxPorts = firstSignal->spectrumChannelMatrix->GetNumRows();

    // Create white noise covariance matrix
    auto allSignalsNoiseCov = NrCovMat{ComplexMatrixArray(nRxPorts, nRxPorts, nRbs)};
    for (size_t iRb = 0; iRb < nRbs; iRb++)
    {
        for (size_t iRxPort = 0; iRxPort < nRxPorts; iRxPort++)
        {
            allSignalsNoiseCov(iRxPort, iRxPort, iRb) = m_noise->ValuesAt(iRb);
        }
    }

    // Add all external interference signals to the covariance matrix
    for (const auto& intfSignal : m_allSignalsMimo)
    {
        if (std::find(m_rxSignalsMimo.begin(), m_rxSignalsMimo.end(), intfSignal) !=
            m_rxSignalsMimo.end())
        {
            // This is one of the signals in the current cell
            continue;
        }

        AddInterference(allSignalsNoiseCov, intfSignal);
    }
    return allSignalsNoiseCov;
}

NrCovMat
NrInterference::CalcCurrInterfCov(Ptr<const SpectrumSignalParameters> rxSignal,
                                  const NrCovMat& outOfCellInterfCov) const
{
    // Add also the potential interfering signals intended for this device but belonging to other
    // transmissions. This is required for a gNB receiving MU-MIMO UL signals from multiple UEs
    auto interfNoiseCov = outOfCellInterfCov;
    for (auto& otherSignal : m_rxSignalsMimo)
    {
        if (otherSignal == rxSignal)
        {
            continue; // this is the current receive signal of interest, do not add to interference
        }
        NS_ASSERT_MSG((std::find(m_allSignalsMimo.begin(), m_allSignalsMimo.end(), otherSignal) !=
                       m_allSignalsMimo.end()),
                      "RX signal already deleted from m_allSignalsMimo");

        AddInterference(interfNoiseCov, otherSignal);
    }
    return interfNoiseCov;
}

void
NrInterference::AddInterference(NrCovMat& covMat, Ptr<const SpectrumSignalParameters> signal) const
{
    const auto& chanSpct = *(signal->spectrumChannelMatrix);
    if (signal->precodingMatrix)
    {
        auto& precMats = *(signal->precodingMatrix);
        NS_ASSERT_MSG((precMats.GetNumPages() > 0) && (chanSpct.GetNumPages() > 0),
                      "precMats and channel cannot be empty");
        NS_ASSERT_MSG(precMats.GetNumPages() == chanSpct.GetNumPages(),
                      "dim mismatch " << precMats.GetNumPages() << " vs "
                                      << chanSpct.GetNumPages());
        covMat.AddInterferenceSignal(chanSpct * precMats);
    }
    else
    {
        covMat.AddInterferenceSignal(chanSpct);
    }
}

NrSinrMatrix
NrInterference::ComputeSinr(NrCovMat& outOfCellInterfCov,
                            Ptr<const SpectrumSignalParameters> rxSignal) const
{
    // Calculate the interference+noise (I+N) covariance matrix for this signal,
    // including interference from other RX signals
    auto interfNoiseCov = CalcCurrInterfCov(rxSignal, outOfCellInterfCov);

    // Interference whitening: normalize the signal such that interference + noise covariance matrix
    // is the identity matrix
    const auto& chanSpct = *(rxSignal->spectrumChannelMatrix);
    auto intfNormChanMat = interfNoiseCov.CalcIntfNormChannel(chanSpct);

    // Get the precoding matrix or create a dummy precoding matrix
    ComplexMatrixArray precMat;
    if (rxSignal->precodingMatrix)
    {
        precMat = *(rxSignal->precodingMatrix);
    }
    else
    {
        precMat = ComplexMatrixArray{chanSpct.GetNumCols(), 1, chanSpct.GetNumPages()};
        for (size_t p = 0; p < chanSpct.GetNumPages(); p++)
        {
            precMat(0, 0, p) = 1.0;
        }
    }

    return intfNormChanMat.ComputeSinrForPrecoding(precMat);
}

} // namespace ns3
