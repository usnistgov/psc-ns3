// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-amc.h"

#include "lena-error-model.h"
#include "nr-error-model.h"
#include "nr-lte-mi-error-model.h"

#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/log.h>
#include <ns3/math.h>
#include <ns3/nr-spectrum-value-helper.h>
#include <ns3/object-factory.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrAmc");
NS_OBJECT_ENSURE_REGISTERED(NrAmc);

NrAmc::NrAmc()
{
    NS_LOG_INFO("Initialize AMC module");
}

NrAmc::~NrAmc()
{
}

void
NrAmc::SetDlMode()
{
    NS_LOG_FUNCTION(this);
    m_emMode = NrErrorModel::DL;
}

void
NrAmc::SetUlMode()
{
    NS_LOG_FUNCTION(this);
    m_emMode = NrErrorModel::UL;
}

TypeId
NrAmc::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrAmc")
            .SetParent<Object>()
            .AddAttribute("NumRefScPerRb",
                          "Number of Subcarriers carrying Reference Signals per RB",
                          UintegerValue(1),
                          MakeUintegerAccessor(&NrAmc::SetNumRefScPerRb, &NrAmc::GetNumRefScPerRb),
                          MakeUintegerChecker<uint8_t>(0, 12))
            .AddAttribute("AmcModel",
                          "AMC model used to assign CQI",
                          EnumValue(NrAmc::ErrorModel),
                          MakeEnumAccessor<AmcModel>(&NrAmc::SetAmcModel, &NrAmc::GetAmcModel),
                          MakeEnumChecker(NrAmc::ErrorModel,
                                          "ErrorModel",
                                          NrAmc::ShannonModel,
                                          "ShannonModel"))
            .AddAttribute("ErrorModelType",
                          "Type of the Error Model to use when AmcModel is set to ErrorModel. "
                          "This parameter has to match the ErrorModelType in nr-spectrum-model,"
                          "because they need to refer to same MCS tables and indexes",
                          TypeIdValue(NrLteMiErrorModel::GetTypeId()),
                          MakeTypeIdAccessor(&NrAmc::SetErrorModelType, &NrAmc::GetErrorModelType),
                          MakeTypeIdChecker())
            .AddConstructor<NrAmc>();
    return tid;
}

uint8_t
NrAmc::GetMcsFromCqi(uint8_t cqi) const
{
    NS_LOG_FUNCTION(cqi);
    NS_ASSERT_MSG(cqi >= 0 && cqi <= 15, "CQI must be in [0..15] = " << cqi);

    if (m_cachedCqiToMcsMap.find(cqi) == m_cachedCqiToMcsMap.end())
    {
        double spectralEfficiency = m_errorModel->GetSpectralEfficiencyForCqi(cqi);
        uint8_t mcs = 0;

        while ((mcs < m_errorModel->GetMaxMcs()) &&
               (m_errorModel->GetSpectralEfficiencyForMcs(mcs + 1) <= spectralEfficiency))
        {
            ++mcs;
        }
        NS_LOG_LOGIC("mcs = " << mcs);
        m_cachedCqiToMcsMap[cqi] = mcs;
    }

    return m_cachedCqiToMcsMap.at(cqi);
}

uint8_t
NrAmc::GetNumRefScPerRb() const
{
    NS_LOG_FUNCTION(this);
    return m_numRefScPerRb;
}

void
NrAmc::SetNumRefScPerRb(uint8_t nref)
{
    NS_LOG_FUNCTION(this);
    m_numRefScPerRb = nref;
}

uint32_t
NrAmc::CalculateTbSize(uint8_t mcs, uint8_t rank, uint32_t nprb) const
{
    NS_LOG_FUNCTION(this << static_cast<uint32_t>(mcs));

    NS_ASSERT_MSG(mcs <= m_errorModel->GetMaxMcs(),
                  "MCS=" << static_cast<uint32_t>(mcs) << " while maximum MCS is "
                         << static_cast<uint32_t>(m_errorModel->GetMaxMcs()));

    uint32_t payloadSize = GetPayloadSize(mcs, rank, nprb);
    uint32_t tbSize = payloadSize;

    if (m_errorModelType != LenaErrorModel::GetTypeId())
    {
        if (payloadSize >= m_crcLen)
        {
            tbSize =
                payloadSize - m_crcLen; // subtract parity bits of m_crcLen used in transport block
        }
        uint32_t cbSize =
            m_errorModel->GetMaxCbSize(payloadSize,
                                       mcs); // max size of a code block (including m_crcLen)
        if (tbSize > cbSize)                 // segmentation of the transport block occurs
        {
            double C = ceil(tbSize / cbSize);
            tbSize = payloadSize - static_cast<uint32_t>(
                                       C * m_crcLen); // subtract bits of m_crcLen used in code
                                                      // blocks, in case of code block segmentation
        }
    }

    NS_LOG_INFO(" mcs:" << (unsigned)mcs << " TB size:" << tbSize);

    return tbSize;
}

uint32_t
NrAmc::GetPayloadSize(uint8_t mcs, uint8_t rank, uint32_t nprb) const
{
    return m_errorModel->GetPayloadSize(NrSpectrumValueHelper::SUBCARRIERS_PER_RB -
                                            GetNumRefScPerRb(),
                                        mcs,
                                        rank,
                                        nprb,
                                        m_emMode);
}

uint8_t
NrAmc::CreateCqiFeedbackWbTdma(const SpectrumValue& sinr, uint8_t& mcs) const
{
    NS_LOG_FUNCTION(this);

    // produces a single CQI/MCS value

    // std::vector<int> cqi;
    uint8_t cqi = 0;
    double seAvg = 0;

    Values::const_iterator it;
    if (m_amcModel == ShannonModel)
    {
        // use shannon model
        double m_ber = GetBer(); // Shannon based model reference BER
        uint32_t rbNum = 0;
        for (it = sinr.ConstValuesBegin(); it != sinr.ConstValuesEnd(); it++)
        {
            double sinr_ = (*it);
            if (sinr_ == 0.0)
            {
                // cqi.push_back (-1); // SINR == 0 (linear units) means no signal in this RB
            }
            else
            {
                /*
                 * Compute the spectral efficiency from the SINR
                 *                                        SINR
                 * spectralEfficiency = log2 (1 + -------------------- )
                 *                                    -ln(5*BER)/1.5
                 * NB: SINR must be expressed in linear units
                 */

                double s = log2(1 + (sinr_ / ((-std::log(5.0 * m_ber)) / 1.5)));
                seAvg += s;

                int cqi_ = GetCqiFromSpectralEfficiency(s);
                rbNum++;

                NS_LOG_LOGIC(" PRB =" << sinr.GetSpectrumModel()->GetNumBands() << ", sinr = "
                                      << sinr_ << " (=" << 10 * std::log10(sinr_) << " dB)"
                                      << ", spectral efficiency =" << s << ", CQI = " << cqi_
                                      << ", BER = " << m_ber);
                // cqi.push_back (cqi_);
            }
        }
        if (rbNum != 0)
        {
            seAvg /= rbNum;
        }
        cqi = GetCqiFromSpectralEfficiency(seAvg); // ceil (cqiAvg);
        mcs = GetMcsFromSpectralEfficiency(seAvg); // ceil(mcsAvg);
    }
    else if (m_amcModel == ErrorModel)
    {
        std::vector<int> rbMap;
        int rbId = 0;
        for (it = sinr.ConstValuesBegin(); it != sinr.ConstValuesEnd(); it++)
        {
            if (*it != 0.0)
            {
                rbMap.push_back(rbId);
            }
            rbId += 1;
        }

        mcs = 0;
        Ptr<NrErrorModelOutput> output;
        while (mcs <= m_errorModel->GetMaxMcs())
        {
            uint8_t rank = 1; // This function is SISO only
            auto tbSize = CalculateTbSize(mcs, rank, rbMap.size());
            output = m_errorModel->GetTbDecodificationStats(sinr,
                                                            rbMap,
                                                            tbSize,
                                                            mcs,
                                                            NrErrorModel::NrErrorModelHistory());
            if (output->m_tbler > 0.1)
            {
                break;
            }
            mcs++;
        }

        if (mcs > 0)
        {
            mcs--;
        }

        if ((output->m_tbler > 0.1) && (mcs == 0))
        {
            cqi = 0;
        }
        else if (mcs == m_errorModel->GetMaxMcs())
        {
            cqi = 15; // all MCSs can guarantee the 10 % of BER
        }
        else
        {
            double s = m_errorModel->GetSpectralEfficiencyForMcs(mcs);
            cqi = 0;
            while ((cqi < 15) && (m_errorModel->GetSpectralEfficiencyForCqi(cqi + 1) <= s))
            {
                ++cqi;
            }
        }
        NS_LOG_DEBUG(this << "\t MCS " << (uint16_t)mcs << "-> CQI " << cqi);
    }
    return cqi;
}

uint8_t
NrAmc::GetCqiFromSpectralEfficiency(double s) const
{
    NS_LOG_FUNCTION(s);
    NS_ASSERT_MSG(s >= 0.0, "negative spectral efficiency = " << s);
    uint8_t cqi = 0;
    while ((cqi < 15) && (m_errorModel->GetSpectralEfficiencyForCqi(cqi + 1) < s))
    {
        ++cqi;
    }
    NS_LOG_LOGIC("cqi = " << cqi);
    return cqi;
}

uint8_t
NrAmc::GetMcsFromSpectralEfficiency(double s) const
{
    NS_LOG_FUNCTION(s);
    NS_ASSERT_MSG(s >= 0.0, "negative spectral efficiency = " << s);
    uint8_t mcs = 0;
    while ((mcs < m_errorModel->GetMaxMcs()) &&
           (m_errorModel->GetSpectralEfficiencyForMcs(mcs + 1) < s))
    {
        ++mcs;
    }
    NS_LOG_LOGIC("cqi = " << mcs);
    return mcs;
}

uint32_t
NrAmc::GetMaxMcs() const
{
    NS_LOG_FUNCTION(this);
    return m_errorModel->GetMaxMcs();
}

void
NrAmc::SetAmcModel(NrAmc::AmcModel m)
{
    NS_LOG_FUNCTION(this);
    m_amcModel = m;
}

NrAmc::AmcModel
NrAmc::GetAmcModel() const
{
    NS_LOG_FUNCTION(this);
    return m_amcModel;
}

void
NrAmc::SetErrorModelType(const TypeId& type)
{
    NS_LOG_FUNCTION(this);
    ObjectFactory factory;
    m_errorModelType = type;

    factory.SetTypeId(m_errorModelType);
    m_errorModel = DynamicCast<NrErrorModel>(factory.Create());
    NS_ASSERT(m_errorModel != nullptr);
    m_cachedCqiToMcsMap.clear(); // clear stale cache
}

TypeId
NrAmc::GetErrorModelType() const
{
    NS_LOG_FUNCTION(this);
    return m_errorModelType;
}

double
NrAmc::GetBer() const
{
    NS_LOG_FUNCTION(this);
    if (GetErrorModelType() == NrLteMiErrorModel::GetTypeId() ||
        GetErrorModelType() == LenaErrorModel::GetTypeId())
    {
        return 0.00005; // Value for LTE error model
    }
    else
    {
        return 0.00001; // Value for NR error model
    }
}

NrAmc::McsParams
NrAmc::GetMaxMcsParams(const NrSinrMatrix& sinrMat, size_t subbandSize) const
{
    auto wbMcs = GetMcs(sinrMat);
    auto wbCqi = GetWbCqiFromMcs(wbMcs);
    auto sbMcs = GetSbMcs(subbandSize, sinrMat);

    std::vector<uint8_t> sbCqis;
    sbCqis.resize(sbMcs.size());

    std::transform(sbMcs.begin(), sbMcs.end(), sbCqis.begin(), [&](uint8_t mcs) {
        return GetWbCqiFromMcs(mcs);
    });

    auto tbSize = CalcTbSizeForMimoMatrix(wbMcs, sinrMat);
    return McsParams{wbMcs, wbCqi, sbCqis, tbSize};
}

std::vector<uint8_t>
NrAmc::GetSbMcs(const size_t subbandSize, const NrSinrMatrix& sinrMat) const
{
    auto nRbs = sinrMat.GetNumRbs();
    auto nSbs = (nRbs + subbandSize - 1) / subbandSize;
    auto sbMcs = std::vector<uint8_t>(nSbs, 0);

    for (size_t i = 0; i < sbMcs.size(); i++)
    {
        auto avgSinrSb = ExtractSbFromMat(i, subbandSize, sinrMat);
        // Assume MCS 0 when SINR is zero (CQI 0 actually)
        if (avgSinrSb(0, 0) == 0.0)
        {
            continue;
        }
        auto sinrTmp = CreateSinrMatForSb(avgSinrSb, sinrMat);
        sbMcs[i] = GetMcs(sinrTmp);
    }
    return sbMcs;
}

NrSinrMatrix
NrAmc::ExtractSbFromMat(const uint8_t sbIndex,
                        const size_t subbandSize,
                        const NrSinrMatrix& sinrMat) const
{
    const auto rank = sinrMat.GetNumRows();
    DoubleMatrixArray sinrSb(rank, 1);
    size_t subbandSizeActual = 0;
    for (uint8_t r = 0; r < rank; r++)
    {
        for (size_t iRb = sbIndex * subbandSize; iRb < (sbIndex + 1) * subbandSize; iRb++)
        {
            sinrSb(r, 0) += sinrMat(r, iRb);
            subbandSizeActual += 1;
            if (iRb >= sinrMat.GetNumCols() - 1)
            {
                break;
            }
        }
    }
    subbandSizeActual /= rank;
    for (uint8_t r = 0; r < rank; r++)
    {
        sinrSb(r, 0) /= subbandSizeActual;
    }

    return sinrSb;
}

NrSinrMatrix
NrAmc::CreateSinrMatForSb(const NrSinrMatrix& avgSinrSb, const NrSinrMatrix& sinrMat) const
{
    auto sbSinrMat = DoubleMatrixArray{sinrMat.GetNumRows(), sinrMat.GetNumCols()};
    for (size_t iRb = 0; iRb < sinrMat.GetNumCols(); iRb++)
    {
        for (size_t layer = 0; layer < sinrMat.GetNumRows(); layer++)
        {
            sbSinrMat(layer, iRb) = avgSinrSb(layer, 0);
        }
    }

    return NrSinrMatrix{sbSinrMat};
}

uint8_t
NrAmc::GetMcs(const NrSinrMatrix& sinrMat) const
{
    auto mcs = uint8_t{0};
    switch (m_amcModel)
    {
    case ShannonModel:
        NS_ABORT_MSG("ShannonModel is not yet supported");
        break;
    case ErrorModel:
        mcs = GetMaxMcsForErrorModel(sinrMat);
        break;
    default:
        NS_ABORT_MSG("AMC model not supported");
        break;
    }
    return mcs;
}

uint8_t
NrAmc::GetMaxMcsForErrorModel(const NrSinrMatrix& sinrMat) const
{
    auto mcs = uint8_t{0};
    while (mcs <= m_errorModel->GetMaxMcs())
    {
        auto tbler = CalcTblerForMimoMatrix(mcs, sinrMat);
        // TODO: Change target TBLER from default 0.1 when using MCS table 3
        if (tbler > 0.1)
        {
            break;
        }
        // The current configuration produces a sufficiently low TBLER, try next value
        mcs++;
    }
    if (mcs > 0)
    {
        // The loop exited because the MCS exceeded max MCS or because of high TBLER. Reduce MCS
        mcs--;
    }

    return mcs;
}

uint8_t
NrAmc::GetWbCqiFromMcs(uint8_t mcs) const
{
    // Based on OSS CreateCqiFeedbackWbTdma
    auto cqi = uint8_t{0};
    if (mcs == 0)
    {
        cqi = 0;
    }
    else if (mcs == m_errorModel->GetMaxMcs())
    {
        // TODO: define constant for max CQI
        cqi = 15;
    }
    else
    {
        auto s = m_errorModel->GetSpectralEfficiencyForMcs(mcs);
        cqi = 0;
        // TODO: define constant for max CQI
        while ((cqi < 15) && (m_errorModel->GetSpectralEfficiencyForCqi(cqi + 1) <= s))
        {
            ++cqi;
        }
    }
    return cqi;
}

double
NrAmc::CalcTblerForMimoMatrix(uint8_t mcs, const NrSinrMatrix& sinrMat) const
{
    auto dummyRnti = uint16_t{0};
    auto duration = Time{1.0}; // Use an arbitrary non-zero time as the chunk duration
    auto mimoChunk = MimoSinrChunk{sinrMat, dummyRnti, duration};
    auto mimoChunks = std::vector<MimoSinrChunk>{mimoChunk};
    auto rank = sinrMat.GetRank();

    // Create the RB map (indices of used RBs, i.e., indices of RBs where SINR is non-zero)
    std::vector<int> rbMap{}; // TODO: change type of rbMap from int to size_t
    int nRbs = static_cast<int>(sinrMat.GetNumRbs());
    for (int rbIdx = 0; rbIdx < nRbs; rbIdx++)
    {
        if (sinrMat(0, rbIdx) != 0.0)
        {
            rbMap.push_back(rbIdx);
        }
    }

    auto tbSize = CalcTbSizeForMimoMatrix(mcs, sinrMat);
    auto dummyHistory = NrErrorModel::NrErrorModelHistory{}; // Create empty HARQ history
    auto outputOfEm = m_errorModel->GetTbDecodificationStatsMimo(mimoChunks,
                                                                 rbMap,
                                                                 tbSize,
                                                                 mcs,
                                                                 rank,
                                                                 dummyHistory);
    return outputOfEm->m_tbler;
}

uint32_t
NrAmc::CalcTbSizeForMimoMatrix(uint8_t mcs, const NrSinrMatrix& sinrMat) const
{
    auto nRbs = sinrMat.GetNumRbs();
    auto nRbSyms = nRbs * NR_AMC_NUM_SYMBOLS_DEFAULT;
    auto rank = sinrMat.GetRank();
    return CalculateTbSize(mcs, rank, nRbSyms);
}

} // namespace ns3
