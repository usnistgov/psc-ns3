// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "beamforming-vector.h"

#include <ns3/angles.h>
#include <ns3/uinteger.h>

namespace ns3
{

PhasedArrayModel::ComplexVector
CreateQuasiOmniBfv(const Ptr<const UniformPlanarArray>& antenna)
{
    auto antennaRows = antenna->GetNumRows();
    auto antennaColumns = antenna->GetNumColumns();
    auto numElemsPerPort = antenna->GetNumElemsPerPort();

    double power = 1 / sqrt(numElemsPerPort);
    size_t numPolarizations = antenna->IsDualPol() ? 2 : 1;

    PhasedArrayModel::ComplexVector omni(antennaRows * antennaColumns * numPolarizations);
    uint16_t bfIndex = 0;
    for (size_t pol = 0; pol < numPolarizations; pol++)
    {
        for (uint32_t ind = 0; ind < antennaRows; ind++)
        {
            std::complex<double> c = 0.0;
            if (antennaRows % 2 == 0)
            {
                c = exp(std::complex<double>(0, M_PI * ind * ind / antennaRows));
            }
            else
            {
                c = exp(std::complex<double>(0, M_PI * ind * (ind + 1) / antennaRows));
            }
            for (uint32_t ind2 = 0; ind2 < antennaColumns; ind2++)
            {
                std::complex<double> d = 0.0;
                if (antennaColumns % 2 == 0)
                {
                    d = exp(std::complex<double>(0, M_PI * ind2 * ind2 / antennaColumns));
                }
                else
                {
                    d = exp(std::complex<double>(0, M_PI * ind2 * (ind2 + 1) / antennaColumns));
                }
                omni[bfIndex] = (c * d * power);
                bfIndex++;
            }
        }
    }
    return omni;
}

PhasedArrayModel::ComplexVector
CreateDirectionalBfv(const Ptr<const UniformPlanarArray>& antenna,
                     uint16_t sector,
                     double elevation)
{
    UintegerValue uintValueNumRows;
    antenna->GetAttribute("NumRows", uintValueNumRows);

    double hAngle_radian =
        M_PI * (static_cast<double>(sector) / static_cast<double>(uintValueNumRows.Get())) -
        0.5 * M_PI;
    double vAngle_radian = elevation * M_PI / 180;
    uint16_t size = antenna->GetNumElems();
    PhasedArrayModel::ComplexVector tempVector(size);
    auto numAnalogBeamElements = (antenna->GetVElemsPerPort() * antenna->GetHElemsPerPort());
    auto power = 1.0 / sqrt(numAnalogBeamElements);
    for (auto ind = 0; ind < size; ind++)
    {
        Vector loc = antenna->GetElementLocation(ind);
        double phase =
            -2 * M_PI *
            (sin(vAngle_radian) * cos(hAngle_radian) * loc.x +
             sin(vAngle_radian) * sin(hAngle_radian) * loc.y + cos(vAngle_radian) * loc.z);
        tempVector[ind] = (exp(std::complex<double>(0, phase)) * power);
    }
    return tempVector;
}

PhasedArrayModel::ComplexVector
CreateDirectionalBfvAz(const Ptr<const UniformPlanarArray>& antenna, double azimuth, double zenith)
{
    UintegerValue uintValueNumRows;
    antenna->GetAttribute("NumRows", uintValueNumRows);

    double hAngle_radian = azimuth * M_PI / 180;
    double vAngle_radian = zenith * M_PI / 180;
    uint16_t size = antenna->GetNumElems();
    double power = 1 / sqrt(size);
    PhasedArrayModel::ComplexVector tempVector(size);
    if (size == 1)
    {
        tempVector[0] = power; // single AE, no BF
    }
    else
    {
        for (auto ind = 0; ind < size; ind++)
        {
            Vector loc = antenna->GetElementLocation(ind);
            double phase =
                -2 * M_PI *
                (sin(vAngle_radian) * cos(hAngle_radian) * loc.x +
                 sin(vAngle_radian) * sin(hAngle_radian) * loc.y + cos(vAngle_radian) * loc.z);
            tempVector[ind] = exp(std::complex<double>(0, phase)) * power;
        }
    }
    return tempVector;
}

PhasedArrayModel::ComplexVector
CreateDirectPathBfv(const Ptr<MobilityModel>& a,
                    const Ptr<MobilityModel>& b,
                    const Ptr<const UniformPlanarArray>& antenna)
{
    // retrieve the position of the two devices
    Vector aPos = a->GetPosition();
    Vector bPos = b->GetPosition();

    // compute the azimuth and the elevation angles
    Angles completeAngle(bPos, aPos);

    double hAngleRadian = completeAngle.GetAzimuth();

    double vAngleRadian = completeAngle.GetInclination(); // the elevation angle

    // retrieve the number of antenna elements
    int totNoArrayElements = antenna->GetNumElems();
    auto numElemsPerPort = antenna->GetNumElemsPerPort();

    // the total power is divided equally among the antenna elements
    double power = 1 / sqrt(numElemsPerPort);

    PhasedArrayModel::ComplexVector antennaWeights(totNoArrayElements);
    // compute the antenna weights
    for (int ind = 0; ind < totNoArrayElements; ind++)
    {
        Vector loc = antenna->GetElementLocation(ind);
        double phase = -2 * M_PI *
                       (sin(vAngleRadian) * cos(hAngleRadian) * loc.x +
                        sin(vAngleRadian) * sin(hAngleRadian) * loc.y + cos(vAngleRadian) * loc.z);
        antennaWeights[ind] = exp(std::complex<double>(0, phase)) * power;
    }

    return antennaWeights;
}

PhasedArrayModel::ComplexVector
CreateKroneckerBfv(const Ptr<const UniformPlanarArray>& antenna, double rowAngle, double colAngle)
{
    // retrieve the number of antenna elements to create bf vector
    PhasedArrayModel::ComplexVector bfVector(antenna->GetNumElems());
    auto vPhasePerEl =
        -2.0 * M_PI * antenna->GetAntennaVerticalSpacing() * cos(rowAngle * M_PI / 180.0);
    auto hPhasePerEl =
        -2.0 * M_PI * antenna->GetAntennaHorizontalSpacing() * cos(colAngle * M_PI / 180.0);

    auto numAnalogBeamElements = antenna->GetVElemsPerPort() * antenna->GetHElemsPerPort();

    // normalize because the total power is divided equally among the analog beams elements
    auto normalizer = 1.0 / sqrt(numAnalogBeamElements);

    auto numCols = antenna->GetNumColumns();
    auto numRows = antenna->GetNumRows();

    // compute the antenna weights (bfvector)
    for (auto elIdx = size_t{0}; elIdx < antenna->GetNumElems(); elIdx++)
    {
        auto colIdx = elIdx % numCols;
        auto rowIdx = elIdx / numCols;
        auto isSkippedCol = (colIdx >= antenna->GetHElemsPerPort());
        auto isSkippedRow = (rowIdx >= antenna->GetVElemsPerPort());
        if (isSkippedCol || isSkippedRow || (elIdx >= numRows * numCols))
        {
            bfVector[elIdx] = 0.0;
            continue;
        }
        auto combPhase = rowIdx * vPhasePerEl + colIdx * hPhasePerEl;
        bfVector[elIdx] = normalizer * std::complex<double>(cos(combPhase), sin(combPhase));
    }
    return bfVector;
}
} // namespace ns3
