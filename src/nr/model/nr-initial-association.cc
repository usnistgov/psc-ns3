// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-initial-association.h"

#include "beamforming-vector.h"

#include "ns3/nr-module.h"
#include "ns3/object.h"

#include <numeric>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NrInitialAssociation");
NS_OBJECT_ENSURE_REGISTERED(NrInitialAssociation);

using LocalSearchParams = NrInitialAssociation::LocalSearchParams;

TypeId
NrInitialAssociation::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrInitialAssociation")
            .SetParent<Object>()
            .SetGroupName("Initial Access")
            .AddConstructor<NrInitialAssociation>()
            .AddAttribute(
                "HandoffMargin",
                "handoff margin (dB); UE attaches to any gNB whose RSRP is within hand off margin",
                DoubleValue(0),
                MakeDoubleAccessor(&NrInitialAssociation::SetHandoffMargin,
                                   &NrInitialAssociation::GetHandoffMargin),
                MakeDoubleChecker<double>())
            .AddAttribute("PrimaryCarrierIndex",
                          "primary carrier index",
                          DoubleValue(0),
                          MakeDoubleAccessor(&NrInitialAssociation::SetPrimaryCarrier,
                                             &NrInitialAssociation::GetPrimaryCarrier),
                          MakeDoubleChecker<double>())

            .AddAttribute("NumMainInterfererGnb",
                          "Number of main interferer gNBs",
                          UintegerValue(6),
                          MakeDoubleAccessor(&NrInitialAssociation::SetNumMainInterfererGnb,
                                             &NrInitialAssociation::GetNumMainInterfererGnb),
                          MakeDoubleChecker<double>());

    return tid;
}

void
NrInitialAssociation::SetNumMainInterfererGnb(uint8_t numInterfere)
{
    m_numMainInterfererGnb = numInterfere;
}

uint8_t
NrInitialAssociation::GetNumMainInterfererGnb() const
{
    return m_numMainInterfererGnb;
}

Ptr<NetDevice>
NrInitialAssociation::GetAssociatedGnb() const
{
    NS_ASSERT(m_associatedGnb != nullptr);
    return m_associatedGnb;
}

NetDeviceContainer
NrInitialAssociation::GetInterferingGnbs() const
{
    return m_intfGnbDevs;
}

double
NrInitialAssociation::GetMaxRsrp(uint64_t gnbId) const
{
    return m_maxRsrps[gnbId];
}

NrAnglePair
NrInitialAssociation::GetBestBfv(uint64_t gnbId) const
{
    return m_bestBfVectors[gnbId];
}

double
NrInitialAssociation::GetRelativeRsrpRatio() const
{
    return m_rsrpRatio;
}

void
NrInitialAssociation::SetStartSsbRb(uint16_t startSsb)
{
    m_startSsb = startSsb;
}

void
NrInitialAssociation::SetNumSsbRb(uint16_t numSsbRb)
{
    m_numBandsSsb = numSsbRb;
}

bool
NrInitialAssociation::CheckNumBeamsAllowed() const
{
    NS_ASSERT_MSG(m_freq, "Error freq in initial association must set first");

    auto numGnbBeams = m_colBeamAngles.size() * m_rowBeamAngles.size();
    if (m_freq <= 3e9)
    {
        return (numGnbBeams <= 4);
    }
    else if (m_freq <= 6e9)
    {
        return (numGnbBeams <= 8);
    }
    else
    {
        return (numGnbBeams <= 64);
    }
}

void
NrInitialAssociation::SetHandoffMargin(double margin)
{
    m_handoffMargin = margin;
}

double
NrInitialAssociation::GetCarrierFrequency() const
{
    return m_freq;
}

double
NrInitialAssociation::GetHandoffMargin() const
{
    return m_handoffMargin;
}

std::vector<double>
NrInitialAssociation::GetRowBeamAngles() const
{
    return m_rowBeamAngles;
}

void
NrInitialAssociation::SetRowBeamAngles(std::vector<double> rowBfVect)
{
    m_rowBeamAngles = rowBfVect;
}

std::vector<double>
NrInitialAssociation::GetColBeamAngles() const
{
    return m_colBeamAngles;
}

void
NrInitialAssociation::SetColBeamAngles(std::vector<double> colBfVect)
{
    m_colBeamAngles = colBfVect;
}

Ptr<const NetDevice>
NrInitialAssociation::GetUeDevice() const
{
    return m_ueDevice;
}

void
NrInitialAssociation::SetUeDevice(const Ptr<NetDevice>& ueDev)
{
    m_ueDevice = ueDev;
}

void
NrInitialAssociation::SetGnbDevices(const NetDeviceContainer& gnbDevices)
{
    m_gnbDevices = gnbDevices;
}

void
NrInitialAssociation::SetPrimaryCarrier(double index)
{
    m_primaryCarrierIndex = index;
}

double
NrInitialAssociation::GetPrimaryCarrier() const
{
    return m_primaryCarrierIndex;
}

LocalSearchParams
NrInitialAssociation::ExtractUeParameters() const
{
    auto ueDev = m_ueDevice->GetObject<NrUeNetDevice>();
    auto phy = ueDev->GetPhy(m_primaryCarrierIndex);
    auto spectrumPhy = phy->GetSpectrumPhy();
    auto spectrumChannel = spectrumPhy->GetSpectrumChannel();
    auto spectrumPropModel = StaticCast<ThreeGppSpectrumPropagationLossModel>(
        spectrumChannel->GetPhasedArraySpectrumPropagationLossModel());
    AntennaArrayModels antModel;
    auto bPhasedArrayModel = spectrumPhy->GetAntenna()->GetObject<PhasedArrayModel>();
    antModel.ueArrayModel =
        Copy<UniformPlanarArray>(DynamicCast<UniformPlanarArray>(bPhasedArrayModel));
    antModel.ueArrayModel->SetNumVerticalPorts(bPhasedArrayModel->GetNumRows());
    antModel.ueArrayModel->SetNumHorizontalPorts(bPhasedArrayModel->GetNumColumns());

    auto channel =
        StaticCast<ThreeGppSpectrumPropagationLossModel>(spectrumPropModel)->GetChannelModel();
    ChannelParams chParams;
    chParams.channelModel = StaticCast<ThreeGppChannelModel>(channel);
    chParams.spectrumPropModel = spectrumPropModel;
    NS_ASSERT_MSG(chParams.channelModel != nullptr,
                  "Channel Model should be to be ThreeGppChannelModel");

    Mobilities mobility;
    mobility.ueMobility = spectrumPhy->GetMobility();
    chParams.pathLossModel =
        StaticCast<ThreeGppPropagationLossModel>(spectrumChannel->GetPropagationLossModel());
    LocalSearchParams lsps{.chParams = chParams, .mobility = mobility, .antennaArrays = antModel};
    return lsps;
}

Ptr<UniformPlanarArray>
NrInitialAssociation::ExtractGnbParameters(const Ptr<NetDevice>& gnbDevice,
                                           LocalSearchParams& lsps) const
{
    auto& chParams = lsps.chParams;
    auto& mobility = lsps.mobility;
    auto& antenna = lsps.antennaArrays;

    const auto gnbDev = gnbDevice->GetObject<NrGnbNetDevice>();
    const auto phy = gnbDev->GetPhy(m_primaryCarrierIndex);
    const auto spectrumPhy = phy->GetSpectrumPhy();
    chParams.spectralModel = spectrumPhy->GetRxSpectrumModel();
    auto bPhasedArrayModel = spectrumPhy->GetAntenna()->GetObject<PhasedArrayModel>();

    // Local copy of antenna model is modified so actual model used after initial access is not
    // affected
    antenna.gnbArrayModel =
        Copy<UniformPlanarArray>(DynamicCast<UniformPlanarArray>(bPhasedArrayModel));
    mobility.gnbMobility = spectrumPhy->GetMobility();
    const auto rowElemsPerPort = antenna.gnbArrayModel->GetVElemsPerPort();
    const auto colElemsPerPort = antenna.gnbArrayModel->GetHElemsPerPort();

    // For initial access beams typically have wider beams so limit the beams to first port
    //  only.
    // This reduces the complexity of the channel model
    antenna.gnbArrayModel->SetNumVerticalPorts(1);
    antenna.gnbArrayModel->SetNumHorizontalPorts(1);
    antenna.gnbArrayModel->SetNumRows(rowElemsPerPort);
    antenna.gnbArrayModel->SetNumColumns(colElemsPerPort);

    return antenna.gnbArrayModel;
}

PhasedArrayModel::ComplexVector
NrInitialAssociation::GenBeamforming(double angRow,
                                     double angCol,
                                     Ptr<UniformPlanarArray> gnbArrayModel) const
{
    auto bfVector = CreateKroneckerBfv(gnbArrayModel, angRow, angCol);
    return bfVector;
}

double
NrInitialAssociation::ComputeRxPsd(Ptr<const SpectrumSignalParameters> spectrumSigParam) const
{
    auto& spectrumChannelMatrix = spectrumSigParam->spectrumChannelMatrix;
    auto numUePorts = spectrumChannelMatrix->GetNumRows();
    double totalPsd = 0;
    for (size_t iRb = m_startSsb; iRb < m_startSsb + m_numBandsSsb; iRb++)
    {
        auto psdPerRb{0.0};
        // Compute the PSD from the MIMO channel matrix.
        for (size_t idxAnt = 0; idxAnt < numUePorts; idxAnt++)
        {
            psdPerRb += norm(spectrumChannelMatrix->Elem(idxAnt, 0, iRb));
        }
        totalPsd += psdPerRb;
    }
    return totalPsd;
}

double
NrInitialAssociation::ComputeMaxRsrp(const Ptr<NetDevice>& gnbDevice, LocalSearchParams& lsps)
{
    double maxPsd = 0;
    auto& chParams = lsps.chParams;
    auto& mobility = lsps.mobility;
    auto& antennas = lsps.antennaArrays;
    antennas.gnbArrayModel = ExtractGnbParameters(gnbDevice, lsps);
    std::vector<int> activeRbs;
    for (size_t rbId = m_startSsb; rbId < m_numBandsSsb + m_startSsb; rbId++)
    {
        activeRbs.push_back(rbId);
    }
    NrAnglePair bfAngles;
    Ptr<const SpectrumValue> fakePsd = NrSpectrumValueHelper::CreateTxPowerSpectralDensity(
        TRANSMIT_POWER_INIT_ASSOC,
        activeRbs,
        chParams.spectralModel,
        NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_USED);
    auto txParams = Create<SpectrumSignalParameters>();
    PhasedArrayModel::ComplexVector ueBfv(antennas.ueArrayModel->GetNumElems());
    ueBfv[0] = 1.0;
    antennas.ueArrayModel->SetBeamformingVector(ueBfv);

    // Calculate maximum achievable RSRP on UE side
    for (size_t i = 0; i < m_colBeamAngles.size(); i++)
    {
        for (size_t j = 0; j < m_rowBeamAngles.size(); j++)
        {
            auto bf =
                GenBeamforming(m_rowBeamAngles[j], m_colBeamAngles[i], antennas.gnbArrayModel);
            antennas.gnbArrayModel->SetBeamformingVector(bf);
            txParams->psd = Copy<SpectrumValue>(fakePsd);
            auto rxParam =
                chParams.spectrumPropModel->DoCalcRxPowerSpectralDensity(txParams,
                                                                         mobility.gnbMobility,
                                                                         mobility.ueMobility,
                                                                         antennas.gnbArrayModel,
                                                                         antennas.ueArrayModel);
            auto eng = ComputeRxPsd(rxParam);
            if (eng > maxPsd)
            {
                maxPsd = eng;
                bfAngles = {m_rowBeamAngles[j], m_colBeamAngles[i]};
            }
        }
    }
    auto attenuation =
        chParams.pathLossModel->CalcRxPower(0, mobility.gnbMobility, mobility.ueMobility);
    m_bestBfVectors.push_back(bfAngles);
    return pow(10.0, attenuation / 10.0) * maxPsd;
}

double
NrInitialAssociation::ComputeRsrpRatio(const double totalRsrp, const std::vector<uint16_t> idxVal)
{
    NS_ASSERT_MSG(m_numIntfGnbs > 0,
                  "Number of main interfering gNBs should not be negative " << m_numIntfGnbs);
    NS_ASSERT_MSG(m_numIntfGnbs < m_maxRsrps.size(),
                  "Number of main interfering gNBs should be less than the number of gNB ");
    double intfRsrp = 0;
    auto nIntf = 0;
    auto j = m_maxRsrps.size() - nIntf;
    while (nIntf < int(m_numIntfGnbs))
    {
        j--;
        auto gnbDev = m_gnbDevices.Get(idxVal[j]);
        if (gnbDev != m_associatedGnb)
        {
            m_intfGnbDevs.Add(gnbDev);
            intfRsrp += std::pow(10.0, m_maxRsrps[idxVal[j]] / 10.0);
            nIntf++;
        }
    }
    return (totalRsrp - intfRsrp) / intfRsrp;
}

void
NrInitialAssociation::InitializeIntfSet(uint16_t numIntf, bool useRelRsrp, double relRsrpThreshold)
{
    NS_ASSERT_MSG(!m_maxRsrps.empty(), "Populate RSRP values first");
    NS_ASSERT_MSG(m_associatedGnb, "Association should be completed first");
    m_numIntfGnbs = numIntf;

    std::vector<uint16_t> idxVal(m_maxRsrps.size());
    std::iota(idxVal.begin(), idxVal.end(), 0); // Initializing the indexes

    // getting the index of the gNBs in increasing order of received power to the UE
    std::sort(idxVal.begin(), idxVal.end(), [&](int i, int j) {
        return (m_maxRsrps[i] < m_maxRsrps[j]);
    });

    auto cumSumIntf = GetInterference(idxVal);
    auto totalInterference = GetTotalInterference(cumSumIntf);
    NS_ASSERT_MSG(totalInterference > 0,
                  "Initial detected power of interferer should be greater than 0");

    m_numIntfGnbs = useRelRsrp
                        ? GetNumIntfGnbsByRelRsrp(cumSumIntf, relRsrpThreshold, totalInterference)
                        : m_numIntfGnbs;
    m_rsrpRatio = ComputeRsrpRatio(totalInterference, idxVal);
}

std::vector<double>
NrInitialAssociation::GetInterference(const std::vector<uint16_t>& idxVal) const
{
    std::vector<double> cumSumIntf(m_maxRsrps.size()); // Cumulative sum of RSRP values from gNBs
    cumSumIntf[0] = std::pow(10.0, m_maxRsrps[idxVal[0]] / 10.0);
    for (size_t i = 1; i < m_maxRsrps.size(); ++i)
    {
        // Getting cumulative sum of received RSRP from gNB wherein RSRP are in increasing order
        cumSumIntf[i] = cumSumIntf[i - 1] + std::pow(10.0, m_maxRsrps[idxVal[i]] / 10.0);
    }
    return cumSumIntf;
}

double
NrInitialAssociation::GetTotalInterference(const std::vector<double>& cumSumIntf) const
{
    auto totalInterference =
        cumSumIntf[m_maxRsrps.size() - 1] -
        std::pow(10.0,
                 m_rsrpAsscGnb / 10.0); // subtract the power of the associated gNB to get
                                        // overall interference. Note that because hand off
                                        // margin, the associated gNB may not be the one with
                                        // highest received power
    return totalInterference;
}

size_t
NrInitialAssociation::GetNumIntfGnbsByRelRsrp(const std::vector<double> cumSumIntf,
                                              const double relRsrpThreshold,
                                              const double totalInterference) const
{
    auto numIntfGnbs = m_maxRsrps.size() - 1;
    for (size_t i = 0; i < m_maxRsrps.size(); ++i)
    {
        // This line is equivalent to  cumSumIntf[i] < relPsd * Main Interference.
        // Note that main interference is totalInterference - cumSumIntf[i]
        if ((1 + relRsrpThreshold) * cumSumIntf[i] > relRsrpThreshold * totalInterference)
        {
            numIntfGnbs -= i; // -1 for the associated gNB
            break;
        }
    }
    return numIntfGnbs;
}

void
NrInitialAssociation::PopulateRsrps(LocalSearchParams& lsps)
{
    // Compute maximum RSRP per each UE and all m_gnbDevices in dB
    std::vector<double> powers;
    powers.resize(m_gnbDevices.GetN());
    std::transform(m_gnbDevices.Begin(),
                   m_gnbDevices.End(),
                   powers.begin(),
                   [&](const Ptr<NetDevice> gnbDev) { return ComputeMaxRsrp(gnbDev, lsps); });
    m_maxRsrps.resize(powers.size());
    std::transform(powers.begin(), powers.end(), m_maxRsrps.begin(), [&](const double val) {
        return 10 * log10(val); // in dB
    });
}

std::pair<Ptr<NetDevice>, double>
NrInitialAssociation::FindAssociatedGnb()
{
    auto localParams = ExtractUeParameters();
    m_freq = localParams.chParams.channelModel->GetFrequency();
    if (m_maxRsrps.empty())
    {
        PopulateRsrps(localParams);
    }
    auto maxVal = std::max_element(m_maxRsrps.begin(), m_maxRsrps.end());
    std::vector<bool> assocFlag;
    assocFlag.resize(m_maxRsrps.size());

    // Keep gnbDevs with RSRP values lower than maxVal and handoffMargin
    std::transform(m_maxRsrps.begin(), m_maxRsrps.end(), assocFlag.begin(), [&](const double val) {
        return (*maxVal - val) <= m_handoffMargin;
    });
    auto numPossibleGnb = std::accumulate(assocFlag.begin(), assocFlag.end(), 0);

    // Choose Randomly gnbDev from possible gNB
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable>();
    x->SetAttribute("Min", DoubleValue(1));
    x->SetAttribute("Max", DoubleValue(numPossibleGnb));
    auto value = x->GetInteger();
    auto count = 0;
    for (size_t i = 0; i < m_gnbDevices.GetN(); i++)
    {
        count = (assocFlag[i] ? count + 1 : count);
        if (count == int(value))
        {
            m_associatedGnb = m_gnbDevices.Get(i);
            m_beamformingVector = GenBeamforming(m_bestBfVectors[i].rowAng,
                                                 m_bestBfVectors[i].colAng,
                                                 localParams.antennaArrays.gnbArrayModel);
            m_rsrpAsscGnb = m_maxRsrps[i];
            return std::make_pair(m_associatedGnb, m_rsrpAsscGnb);
        }
    }
    NS_FATAL_ERROR("Method should have returned");
}

double
NrInitialAssociation::GetAssociatedRsrp() const
{
    return m_rsrpAsscGnb;
}

} // namespace ns3
