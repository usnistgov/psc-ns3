//
// SPDX-License-Identifier: NIST-Software

#include "nr-sl-effective-bler-calculator.h"

#include <ns3/log.h>

#include <utility>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlEffectiveBlerCalculator");

void
NrSlEffectiveBlerCalculator::AddAttempt(double conditionalTbler, double sci2aBler, uint8_t rv)
{
    NS_LOG_FUNCTION(this << conditionalTbler << sci2aBler << +rv);
    m_jointTbler.push_back(m_jointTbler.back() * conditionalTbler);
    const bool selfDecodable = (rv == 0 || rv == 3);
    std::vector<std::array<double, 2>> next(m_stateProb.size() + 1, {0.0, 0.0});
    for (std::size_t m = 0; m < m_stateProb.size(); m++)
    {
        for (std::size_t flag = 0; flag < 2; flag++)
        {
            double p = m_stateProb[m][flag];
            if (p == 0.0)
            {
                continue;
            }
            // Attempt missed (SCI-2a failed to decode)
            next[m][flag] += p * sci2aBler;
            // Attempt received
            std::size_t nextFlag = (flag == 1 || selfDecodable) ? 1 : 0;
            next[m + 1][nextFlag] += p * (1.0 - sci2aBler);
        }
    }
    m_stateProb = std::move(next);
}

double
NrSlEffectiveBlerCalculator::GetEffectiveBler(bool includeSci2a) const
{
    if (!includeSci2a)
    {
        return m_jointTbler.back();
    }
    double effBler = 0.0;
    for (std::size_t m = 0; m < m_stateProb.size(); m++)
    {
        // States without a self-decodable redundancy version (including
        // zero received attempts) cannot be decoded
        effBler += m_stateProb[m][0];
        effBler += m_stateProb[m][1] * m_jointTbler[m];
    }
    return effBler;
}

} // namespace ns3
