/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 */

#include "nr-sl-rrc-sap.h"

namespace ns3
{

uint32_t
NrSlRrcSap::GetScSpacingValue(const NrSlRrcSap::SubcarrierSpacing& scspacingAsEnum)
{
    uint32_t spsInt = 0;
    switch (scspacingAsEnum.spacing)
    {
    case SubcarrierSpacing::kHZ15:
        spsInt = 15000;
        break;
    case SubcarrierSpacing::kHZ30:
        spsInt = 30000;
        break;
    case SubcarrierSpacing::kHZ60:
        spsInt = 60000;
        break;
    case SubcarrierSpacing::kHZ120:
        spsInt = 120000;
        break;
    case SubcarrierSpacing::kHZ240:
        spsInt = 240000;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << scspacingAsEnum.spacing
                                                    << " used for Subcarrier spacing");
    }

    return spsInt;
}

NrSlRrcSap::SubcarrierSpacing
NrSlRrcSap::GetScSpacingEnum(uint32_t scspacingAsInt)
{
    SubcarrierSpacing spsEnum;
    switch (scspacingAsInt)
    {
    case 15000:
        spsEnum.spacing = SubcarrierSpacing::kHZ15;
        break;
    case 30000:
        spsEnum.spacing = SubcarrierSpacing::kHZ30;
        break;
    case 60000:
        spsEnum.spacing = SubcarrierSpacing::kHZ60;
        break;
    case 120000:
        spsEnum.spacing = SubcarrierSpacing::kHZ120;
        break;
    case 240000:
        spsEnum.spacing = SubcarrierSpacing::kHZ240;
        break;
    default:
        NS_FATAL_ERROR("Subcarrier spacing not supported " << scspacingAsInt);
    }

    return spsEnum;
}

uint16_t
NrSlRrcSap::GetSlLengthSymbolsValue(const NrSlRrcSap::SlLengthSymbols& slLengthSymsAsEnum)
{
    uint16_t slLenInt = 0;
    switch (slLengthSymsAsEnum.symbols)
    {
    case SlLengthSymbols::SYM7:
        slLenInt = 7;
        break;
    case SlLengthSymbols::SYM8:
        slLenInt = 8;
        break;
    case SlLengthSymbols::SYM9:
        slLenInt = 9;
        break;
    case SlLengthSymbols::SYM10:
        slLenInt = 10;
        break;
    case SlLengthSymbols::SYM11:
        slLenInt = 11;
        break;
    case SlLengthSymbols::SYM12:
        slLenInt = 12;
        break;
    case SlLengthSymbols::SYM13:
        slLenInt = 13;
        break;
    case SlLengthSymbols::SYM14:
        slLenInt = 14;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slLengthSymsAsEnum.symbols
                                                    << " used for SL Symbol allocation");
    }

    return slLenInt;
}

NrSlRrcSap::SlLengthSymbols
NrSlRrcSap::GetSlLengthSymbolsEnum(uint16_t slLengthSymsAsInt)
{
    SlLengthSymbols slLenSymEnum;
    switch (slLengthSymsAsInt)
    {
    case 7:
        slLenSymEnum.symbols = SlLengthSymbols::SYM7;
        break;
    case 8:
        slLenSymEnum.symbols = SlLengthSymbols::SYM8;
        break;
    case 9:
        slLenSymEnum.symbols = SlLengthSymbols::SYM9;
        break;
    case 10:
        slLenSymEnum.symbols = SlLengthSymbols::SYM10;
        break;
    case 11:
        slLenSymEnum.symbols = SlLengthSymbols::SYM11;
        break;
    case 12:
        slLenSymEnum.symbols = SlLengthSymbols::SYM12;
        break;
    case 13:
        slLenSymEnum.symbols = SlLengthSymbols::SYM13;
        break;
    case 14:
        slLenSymEnum.symbols = SlLengthSymbols::SYM14;
        break;
    default:
        NS_FATAL_ERROR("Invalid symbol allocation for SL " << slLengthSymsAsInt);
    }

    return slLenSymEnum;
}

uint16_t
NrSlRrcSap::GetSlStartSymbolValue(const NrSlRrcSap::SlStartSymbol& slStartSym)
{
    uint16_t slStartSymInt = 0;
    switch (slStartSym.symbol)
    {
    case SlStartSymbol::SYM0:
        slStartSymInt = 0;
        break;
    case SlStartSymbol::SYM1:
        slStartSymInt = 1;
        break;
    case SlStartSymbol::SYM2:
        slStartSymInt = 2;
        break;
    case SlStartSymbol::SYM3:
        slStartSymInt = 3;
        break;
    case SlStartSymbol::SYM4:
        slStartSymInt = 4;
        break;
    case SlStartSymbol::SYM5:
        slStartSymInt = 5;
        break;
    case SlStartSymbol::SYM6:
        slStartSymInt = 6;
        break;
    case SlStartSymbol::SYM7:
        slStartSymInt = 7;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slStartSym.symbol
                                                    << " used for SL start Symbol");
    }

    return slStartSymInt;
}

NrSlRrcSap::SlStartSymbol
NrSlRrcSap::GetSlStartSymbolEnum(uint16_t slStartSym)
{
    SlStartSymbol slStartSymEnum;
    switch (slStartSym)
    {
    case 0:
        slStartSymEnum.symbol = SlStartSymbol::SYM0;
        break;
    case 1:
        slStartSymEnum.symbol = SlStartSymbol::SYM1;
        break;
    case 2:
        slStartSymEnum.symbol = SlStartSymbol::SYM2;
        break;
    case 3:
        slStartSymEnum.symbol = SlStartSymbol::SYM3;
        break;
    case 4:
        slStartSymEnum.symbol = SlStartSymbol::SYM4;
        break;
    case 5:
        slStartSymEnum.symbol = SlStartSymbol::SYM6;
        break;
    case 7:
        slStartSymEnum.symbol = SlStartSymbol::SYM7;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink symbol start value " << slStartSym);
    }

    return slStartSymEnum;
}

uint16_t
NrSlRrcSap::GetSlTResoPscchValue(const NrSlRrcSap::SlTimeResourcePscch& slTResoPscch)
{
    uint16_t slTResoPscchInt = 0;
    switch (slTResoPscch.resources)
    {
    case SlTimeResourcePscch::N1:
        slTResoPscchInt = 1;
        break;
    case SlTimeResourcePscch::N2:
        slTResoPscchInt = 2;
        break;
    case SlTimeResourcePscch::N3:
        slTResoPscchInt = 3;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slTResoPscch.resources
                                                    << " used for NrSlRrcSap::SlTimeResourcePscch");
    }

    return slTResoPscchInt;
}

NrSlRrcSap::SlTimeResourcePscch
NrSlRrcSap::GetSlTResoPscchEnum(uint16_t slTResoPscch)
{
    SlTimeResourcePscch slTResoPscchEnum;
    switch (slTResoPscch)
    {
    case 1:
        slTResoPscchEnum.resources = SlTimeResourcePscch::N1;
        break;
    case 2:
        slTResoPscchEnum.resources = SlTimeResourcePscch::N2;
        break;
    case 3:
        slTResoPscchEnum.resources = SlTimeResourcePscch::N3;
        break;
    default:
        NS_FATAL_ERROR("Invalid number of symbols : " << slTResoPscch << " used for PSCCH");
    }

    return slTResoPscchEnum;
}

uint16_t
NrSlRrcSap::GetSlFResoPscchValue(const NrSlRrcSap::SlFreqResourcePscch& slFResoPscch)
{
    uint16_t slFResoPscchInt = 0;
    switch (slFResoPscch.resources)
    {
    case SlFreqResourcePscch::N10:
        slFResoPscchInt = 10;
        break;
    case SlFreqResourcePscch::N12:
        slFResoPscchInt = 12;
        break;
    case SlFreqResourcePscch::N15:
        slFResoPscchInt = 15;
        break;
    case SlFreqResourcePscch::N20:
        slFResoPscchInt = 20;
        break;
    case SlFreqResourcePscch::N25:
        slFResoPscchInt = 25;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slFResoPscch.resources
                                                    << " used for NrSlRrcSap::SlFreqResourcePscch");
    }

    return slFResoPscchInt;
}

NrSlRrcSap::SlFreqResourcePscch
NrSlRrcSap::GetSlFResoPscchEnum(uint16_t slFResoPscch)
{
    SlFreqResourcePscch slFResoPscchEnum;
    switch (slFResoPscch)
    {
    case 10:
        slFResoPscchEnum.resources = SlFreqResourcePscch::N10;
        break;
    case 12:
        slFResoPscchEnum.resources = SlFreqResourcePscch::N12;
        break;
    case 15:
        slFResoPscchEnum.resources = SlFreqResourcePscch::N15;
        break;
    case 20:
        slFResoPscchEnum.resources = SlFreqResourcePscch::N20;
        break;
    case 25:
        slFResoPscchEnum.resources = SlFreqResourcePscch::N25;
        break;

    default:
        NS_FATAL_ERROR("Invalid number of PRBs : " << slFResoPscch << " used for PSCCH");
    }

    return slFResoPscchEnum;
}

float
NrSlRrcSap::GetSlScalingValue(const NrSlRrcSap::SlScaling& slScaling)
{
    float slScalingInt = 0;
    switch (slScaling.scaling)
    {
    case SlScaling::F0P5:
        slScalingInt = 0.5;
        break;
    case SlScaling::F0P65:
        slScalingInt = 0.65;
        break;
    case SlScaling::F0P8:
        slScalingInt = 0.8;
        break;
    case SlScaling::F1:
        slScalingInt = 1;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slScaling.scaling
                                                    << " used for NrSlRrcSap::SlScaling");
    }

    return slScalingInt;
}

NrSlRrcSap::SlScaling
NrSlRrcSap::GetSlScalingEnum(float slScaling)
{
    SlScaling slScalingEnum;
    // I need to mask the float value of the scaling factor
    // to have an integral type for the switch statement to work.
    auto slScalingMask = static_cast<uint32_t>(100 * slScaling);
    switch (slScalingMask)
    {
    case 50:
        slScalingEnum.scaling = SlScaling::F0P5;
        break;
    case 65:
        slScalingEnum.scaling = SlScaling::F0P65;
        break;
    case 80:
        slScalingEnum.scaling = SlScaling::F0P8;
        break;
    case 100:
        slScalingEnum.scaling = SlScaling::F1;
        break;

    default:
        NS_FATAL_ERROR("Invalid sidelink scaling factor value : " << slScaling);
    }

    return slScalingEnum;
}

uint16_t
NrSlRrcSap::GetNrSlSubChSizeValue(const NrSlRrcSap::SlSubchannelSize& subChSize)
{
    uint16_t subChSizeInt = 0;
    switch (subChSize.numPrbs)
    {
    case SlSubchannelSize::N10:
        subChSizeInt = 10;
        break;
    case SlSubchannelSize::N15:
        subChSizeInt = 15;
        break;
    case SlSubchannelSize::N20:
        subChSizeInt = 20;
        break;
    case SlSubchannelSize::N25:
        subChSizeInt = 25;
        break;
    case SlSubchannelSize::N50:
        subChSizeInt = 50;
        break;
    case SlSubchannelSize::N75:
        subChSizeInt = 75;
        break;
    case SlSubchannelSize::N100:
        subChSizeInt = 100;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << subChSize.numPrbs
                                                    << " NrSlRrcSap::SlSubchannelSize");
    }

    return subChSizeInt;
}

NrSlRrcSap::SlSubchannelSize
NrSlRrcSap::GetNrSlSubChSizeEnum(uint16_t subChSize)
{
    SlSubchannelSize slSubchannelSizeEnum;
    switch (subChSize)
    {
    case 10:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N10;
        break;
    case 15:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N15;
        break;
    case 20:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N20;
        break;
    case 25:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N25;
        break;
    case 50:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N50;
        break;
    case 75:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N75;
        break;
    case 100:
        slSubchannelSizeEnum.numPrbs = SlSubchannelSize::N100;
        break;
    default:
        NS_FATAL_ERROR("Invalid Subchannel size : " << subChSize << " used");
    }

    return slSubchannelSizeEnum;
}

uint16_t
NrSlRrcSap::GetSlSensWindowValue(const NrSlRrcSap::SlSensingWindow& windowSize)
{
    uint16_t sizeInt = 0;
    switch (windowSize.windSize)
    {
    case SlSensingWindow::MS100:
        sizeInt = 100;
        break;
    case SlSensingWindow::MS1100:
        sizeInt = 1100;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << windowSize.windSize
                                                    << " NrSlRrcSap::SlSensingWindow");
    }

    return sizeInt;
}

NrSlRrcSap::SlSensingWindow
NrSlRrcSap::GetSlSensWindowEnum(uint16_t windowSize)
{
    SlSensingWindow sizeEnum;
    switch (windowSize)
    {
    case 100:
        sizeEnum.windSize = SlSensingWindow::MS100;
        break;
    case 1100:
        sizeEnum.windSize = SlSensingWindow::MS1100;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink sensing window size : " << windowSize << " used");
    }

    return sizeEnum;
}

uint16_t
NrSlRrcSap::GetSlSelWindowValue(const NrSlRrcSap::SlSelectionWindow& windowSize)
{
    uint16_t sizeInt = 0;
    switch (windowSize.windSize)
    {
    case SlSelectionWindow::N1:
        sizeInt = 1;
        break;
    case SlSelectionWindow::N5:
        sizeInt = 5;
        break;
    case SlSelectionWindow::N10:
        sizeInt = 10;
        break;
    case SlSelectionWindow::N20:
        sizeInt = 20;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << windowSize.windSize
                                                    << " NrSlRrcSap::SlSelectionWindow");
    }

    return sizeInt;
}

NrSlRrcSap::SlSelectionWindow
NrSlRrcSap::GetSlSelWindowEnum(uint16_t windowSize)
{
    SlSelectionWindow sizeEnum;
    switch (windowSize)
    {
    case 1:
        sizeEnum.windSize = SlSelectionWindow::N1;
        break;
    case 5:
        sizeEnum.windSize = SlSelectionWindow::N5;
        break;
    case 10:
        sizeEnum.windSize = SlSelectionWindow::N10;
        break;
    case 20:
        sizeEnum.windSize = SlSelectionWindow::N20;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink selection window size : " << windowSize << " used");
    }

    return sizeEnum;
}

uint16_t
NrSlRrcSap::GetSlResoResvPrdValue(const NrSlRrcSap::SlResourceReservePeriod& period)
{
    uint16_t perodInt = 0;
    switch (period.period)
    {
    case SlResourceReservePeriod::MS0:
        perodInt = 0;
        break;
    case SlResourceReservePeriod::MS10:
        perodInt = 10;
        break;
    case SlResourceReservePeriod::MS20:
        perodInt = 20;
        break;
    case SlResourceReservePeriod::MS30:
        perodInt = 30;
        break;
    case SlResourceReservePeriod::MS40:
        perodInt = 40;
        break;
    case SlResourceReservePeriod::MS50:
        perodInt = 50;
        break;
    case SlResourceReservePeriod::MS60:
        perodInt = 60;
        break;
    case SlResourceReservePeriod::MS70:
        perodInt = 70;
        break;
    case SlResourceReservePeriod::MS80:
        perodInt = 80;
        break;
    case SlResourceReservePeriod::MS90:
        perodInt = 90;
        break;
    case SlResourceReservePeriod::MS100:
        perodInt = 100;
        break;
    case SlResourceReservePeriod::MS150:
        perodInt = 150;
        break;
    case SlResourceReservePeriod::MS200:
        perodInt = 200;
        break;
    case SlResourceReservePeriod::MS300:
        perodInt = 300;
        break;
    case SlResourceReservePeriod::MS350:
        perodInt = 350;
        break;
    case SlResourceReservePeriod::MS400:
        perodInt = 400;
        break;
    case SlResourceReservePeriod::MS450:
        perodInt = 450;
        break;
    case SlResourceReservePeriod::MS500:
        perodInt = 500;
        break;
    case SlResourceReservePeriod::MS550:
        perodInt = 550;
        break;
    case SlResourceReservePeriod::MS600:
        perodInt = 600;
        break;
    case SlResourceReservePeriod::MS650:
        perodInt = 650;
        break;
    case SlResourceReservePeriod::MS700:
        perodInt = 700;
        break;
    case SlResourceReservePeriod::MS750:
        perodInt = 750;
        break;
    case SlResourceReservePeriod::MS800:
        perodInt = 800;
        break;
    case SlResourceReservePeriod::MS850:
        perodInt = 850;
        break;
    case SlResourceReservePeriod::MS900:
        perodInt = 900;
        break;
    case SlResourceReservePeriod::MS950:
        perodInt = 950;
        break;
    case SlResourceReservePeriod::MS1000:
        perodInt = 1000;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << period.period
                                                    << " NrSlRrcSap::SlResourceReservePeriod");
    }

    return perodInt;
}

NrSlRrcSap::SlResourceReservePeriod
NrSlRrcSap::GetSlResoResvPrdEnum(uint16_t period)
{
    SlResourceReservePeriod periodEnum;
    switch (period)
    {
    case 0:
        periodEnum.period = SlResourceReservePeriod::MS0;
        break;
    case 10:
        periodEnum.period = SlResourceReservePeriod::MS10;
        break;
    case 20:
        periodEnum.period = SlResourceReservePeriod::MS20;
        break;
    case 30:
        periodEnum.period = SlResourceReservePeriod::MS30;
        break;
    case 40:
        periodEnum.period = SlResourceReservePeriod::MS40;
        break;
    case 50:
        periodEnum.period = SlResourceReservePeriod::MS50;
        break;
    case 60:
        periodEnum.period = SlResourceReservePeriod::MS60;
        break;
    case 70:
        periodEnum.period = SlResourceReservePeriod::MS70;
        break;
    case 80:
        periodEnum.period = SlResourceReservePeriod::MS80;
        break;
    case 90:
        periodEnum.period = SlResourceReservePeriod::MS90;
        break;
    case 100:
        periodEnum.period = SlResourceReservePeriod::MS100;
        break;
    case 150:
        periodEnum.period = SlResourceReservePeriod::MS150;
        break;
    case 200:
        periodEnum.period = SlResourceReservePeriod::MS200;
        break;
    case 250:
        periodEnum.period = SlResourceReservePeriod::MS250;
        break;
    case 300:
        periodEnum.period = SlResourceReservePeriod::MS300;
        break;
    case 350:
        periodEnum.period = SlResourceReservePeriod::MS350;
        break;
    case 400:
        periodEnum.period = SlResourceReservePeriod::MS400;
        break;
    case 450:
        periodEnum.period = SlResourceReservePeriod::MS450;
        break;
    case 500:
        periodEnum.period = SlResourceReservePeriod::MS500;
        break;
    case 550:
        periodEnum.period = SlResourceReservePeriod::MS550;
        break;
    case 600:
        periodEnum.period = SlResourceReservePeriod::MS600;
        break;
    case 650:
        periodEnum.period = SlResourceReservePeriod::MS650;
        break;
    case 700:
        periodEnum.period = SlResourceReservePeriod::MS700;
        break;
    case 750:
        periodEnum.period = SlResourceReservePeriod::MS750;
        break;
    case 800:
        periodEnum.period = SlResourceReservePeriod::MS800;
        break;
    case 850:
        periodEnum.period = SlResourceReservePeriod::MS850;
        break;
    case 900:
        periodEnum.period = SlResourceReservePeriod::MS900;
        break;
    case 950:
        periodEnum.period = SlResourceReservePeriod::MS950;
        break;
    case 1000:
        periodEnum.period = SlResourceReservePeriod::MS1000;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink reservation period : " << period << " used");
    }

    return periodEnum;
}

uint8_t
NrSlRrcSap::GetSlMaxNumPerReserveValue(const NrSlRrcSap::SlMaxNumPerReserve& slMaxReserve)
{
    uint8_t maxResInt = 0;
    switch (slMaxReserve.maxNumPerRes)
    {
    case SlMaxNumPerReserve::N1:
        maxResInt = 1;
        break;
    case SlMaxNumPerReserve::N2:
        maxResInt = 2;
        break;
    case SlMaxNumPerReserve::N3:
        maxResInt = 3;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slMaxReserve.maxNumPerRes
                                                    << " NrSlRrcSap::SlMaxNumPerReserve");
    }

    return maxResInt;
}

NrSlRrcSap::SlMaxNumPerReserve
NrSlRrcSap::GetSlMaxNumPerReserveEnum(uint8_t slMaxReserveInt)
{
    SlMaxNumPerReserve maxResPerEnum;
    switch (slMaxReserveInt)
    {
    case 1:
        maxResPerEnum.maxNumPerRes = SlMaxNumPerReserve::N1;
        break;
    case 2:
        maxResPerEnum.maxNumPerRes = SlMaxNumPerReserve::N2;
        break;
    case 3:
        maxResPerEnum.maxNumPerRes = SlMaxNumPerReserve::N3;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink value " << +slMaxReserveInt
                                                 << " used for number SlMaxNumPerReserve");
    }

    return maxResPerEnum;
}

uint8_t
NrSlRrcSap::GetSlPsfchPeriodValue(const NrSlRrcSap::SlPsfchPeriod& slPsfchPeriod)
{
    uint8_t psfchPeriod = 0;
    switch (slPsfchPeriod.period)
    {
    case SlPsfchPeriod::SL0:
        psfchPeriod = 0;
        break;
    case SlPsfchPeriod::SL1:
        psfchPeriod = 1;
        break;
    case SlPsfchPeriod::SL2:
        psfchPeriod = 2;
        break;
    case SlPsfchPeriod::SL4:
        psfchPeriod = 4;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slPsfchPeriod.period
                                                    << " NrSlRrcSap::SlPsfchPeriod");
    }

    return psfchPeriod;
}

NrSlRrcSap::SlPsfchPeriod
NrSlRrcSap::GetSlPsfchPeriodEnum(uint8_t slPsfchPeriodInt)
{
    SlPsfchPeriod psfchPeriodEnum;
    switch (slPsfchPeriodInt)
    {
    case 0:
        psfchPeriodEnum.period = SlPsfchPeriod::SL0;
        break;
    case 1:
        psfchPeriodEnum.period = SlPsfchPeriod::SL1;
        break;
    case 2:
        psfchPeriodEnum.period = SlPsfchPeriod::SL2;
        break;
    case 4:
        psfchPeriodEnum.period = SlPsfchPeriod::SL4;
        break;
    default:
        NS_FATAL_ERROR("Invalid PSFCH period value " << +slPsfchPeriodInt);
    }

    return psfchPeriodEnum;
}

uint8_t
NrSlRrcSap::GetSlMinTimeGapPsfchValue(const NrSlRrcSap::SlMinTimeGapPsfch& slMinTimeGapPsfch)
{
    uint8_t minTimeGapPsfch = 0;
    switch (slMinTimeGapPsfch.gap)
    {
    case SlMinTimeGapPsfch::SL2:
        minTimeGapPsfch = 2;
        break;
    case SlMinTimeGapPsfch::SL3:
        minTimeGapPsfch = 3;
        break;
    default:
        NS_FATAL_ERROR("Invalid enumeration value " << slMinTimeGapPsfch.gap
                                                    << " NrSlRrcSap::SlMinTimeGapPsfch");
    }

    return minTimeGapPsfch;
}

NrSlRrcSap::SlMinTimeGapPsfch
NrSlRrcSap::GetSlMinTimeGapPsfchEnum(uint8_t slMinTimeGapPsfchInt)
{
    SlMinTimeGapPsfch minTimeGapEnum;
    switch (slMinTimeGapPsfchInt)
    {
    case 2:
        minTimeGapEnum.gap = SlMinTimeGapPsfch::SL2;
        break;
    case 3:
        minTimeGapEnum.gap = SlMinTimeGapPsfch::SL3;
        break;
    default:
        NS_FATAL_ERROR("Invalid MinTimeGapPSFCH value " << +slMinTimeGapPsfchInt);
    }

    return minTimeGapEnum;
}

NrSlUeRrcSapUser::~NrSlUeRrcSapUser()
{
}

NrSlUeRrcSapProvider::~NrSlUeRrcSapProvider()
{
}

NrSlEnbRrcSapUser::~NrSlEnbRrcSapUser()
{
}

NrSlEnbRrcSapProvider::~NrSlEnbRrcSapProvider()
{
}

} // namespace ns3
