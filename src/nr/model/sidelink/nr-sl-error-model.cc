//
// SPDX-License-Identifier: NIST-Software

#include "nr-sl-error-model.h"

#include <ns3/log.h>

#include <span>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlErrorModel");

NS_OBJECT_ENSURE_REGISTERED(NrSlErrorModel);

NrSlErrorModel::NrSlErrorModel()
    : Object()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlErrorModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlErrorModel").SetParent<Object>();
    return tid;
}

double
NrSlErrorModel::Interpolate(double snrDb,
                            std::span<const double> snrVector,
                            std::span<const double> blerVector,
                            double threshold,
                            double extrapolationLimit) const
{
    NS_LOG_FUNCTION(this << snrDb << threshold << extrapolationLimit);
    NS_ASSERT_MSG(snrVector.size() == blerVector.size(),
                  "Mismatch container size " << snrVector.size() << " " << blerVector.size());
    NS_ASSERT_MSG(snrVector.size() > 0, "Invalid SNR vector");
    if (snrDb <= snrVector[0])
    {
        return 1;
    }
    if (snrDb >= snrVector[snrVector.size() - 1])
    {
        // Do not extrapolate beyond largest SNR value
        return 0;
    }
    // SNR is stored as a dB value in the vector, but convert to linear for interpolation
    double snrLinear = pow(10, snrDb / 10);
    double bler = 0;
    // The relationship between values:  currentSnr <= snrLinear <= lastSnr
    double lastSnr = 0;
    double lastBler = 0;
    double currentSnr = 0;
    double currentBler = 0;
    // Reverse iterate; SNR values in simulations will often be right of the curve
    // The following variable ensures that two data points are found (extrapolation case)
    std::size_t numDataPointsFound{0};
    for (std::size_t i = snrVector.size(); i-- > 0;)
    {
        bler = blerVector[i];
        if (threshold && bler < threshold)
        {
            continue;
        }
        if (snrDb >= snrVector[i] && numDataPointsFound)
        {
            currentSnr = pow(10, snrVector[i] / 10);
            currentBler = blerVector[i];
            break;
        }
        else
        {
            lastSnr = pow(10, snrVector[i] / 10);
            lastBler = blerVector[i];
            numDataPointsFound++;
        }
    }
    // Linearly extrapolate on the log/log scale (log(bler) vs SNR in dB)
    if (snrDb > 10 * log10(lastSnr))
    {
        auto logBler = std::lerp(log10(currentBler),
                                 log10(lastBler),
                                 (snrDb - 10 * log10(currentSnr)) /
                                     (10 * log10(lastSnr) - 10 * log10(currentSnr)));
        if (pow(10, logBler) < extrapolationLimit)
        {
            return 0;
        }
        NS_LOG_DEBUG("Extrapolation result: snrDb "
                     << snrDb << " bler " << pow(10, logBler) << " currentSnr "
                     << 10 * log10(currentSnr) << " lastSnr " << 10 * log10(lastSnr)
                     << " currentBler " << currentBler << " lastBler " << lastBler);

        return pow(10, logBler);
    }
    // Interpolate on the linear scale
    bler = std::lerp(currentBler, lastBler, (snrLinear - currentSnr) / (lastSnr - currentSnr));
    NS_ASSERT_MSG(bler >= 0, "Data possibly not monotonically increasing");
    NS_LOG_DEBUG("Interpolation result: snrDb "
                 << snrDb << " bler " << bler << " currentSnr " << 10 * log10(currentSnr)
                 << " lastSnr " << 10 * log10(lastSnr) << " currentBler " << currentBler
                 << " lastBler " << lastBler);
    return bler;
}

double
NrSlErrorModel::GetAverageSnr(double snrDb, const NrErrorModel::NrErrorModelHistory& history) const
{
    // Perform averaging on linear SNR values
    double averageSnr{pow(10, snrDb / 10)};
    for (const auto& x : history)
    {
        auto emOutput = DynamicCast<NrSlErrorModelOutput>(x);
        NS_ASSERT_MSG(emOutput, "ErrorModelOutput type mismatch");
        averageSnr += pow(10, emOutput->m_snrDb / 10);
    }
    averageSnr /= (history.size() + 1);
    return (10 * log10(averageSnr));
}

} // namespace ns3
