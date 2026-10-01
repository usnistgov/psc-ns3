// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef LENA_ERROR_MODEL_H
#define LENA_ERROR_MODEL_H

#include "nr-lte-mi-error-model.h"

namespace ns3
{

/**
 * \ingroup error-models
 * \brief LENA error model
 *
 * LENA is calculating the transport block sizes in a different way than we
 * employ in our module. In particular, it uses tables that come from the standard.
 * This error model is created to have the same values as used in LENA.
 *
 * Please note that, since LENA always assigns all the available symbols to the
 * UE, the transport block calculation is way more limited than in NR. To account
 * for symbol granularity, in NR we are calling the function GetPayloadSize()
 * by passing the number of RB in frequency, multiplied by the number of symbols
 * assigned. To avoid a costly API rewrite through the NR module, we assume that
 * the code that will be calling this function (hopefully, and OFDMA scheduler)
 * will assign all the symbols except the *single* DL or UL CTRL symbol.
 *
 * For matching the two RB values, we divide by 13 (hence, assuming 1 CTRL symbol)
 * the value of RB that is coming as input. As example, imagine that the scheduler
 * is assigning 2 RB over 13 symbols. In LENA, the function GetPayloadSize()
 * would have been called with the input parameter RB set to 2; in NR, we call
 * it with the same input parameter set to 26. To be able to retrieve the
 * same value from the table, we have to adapt it, and here is explained why
 * we divide by 13 the RB number.
 *
 * Please note that we assume 1 symbol for CTRL. If you use more than one,
 * then the calculation will be wrong.
 */
class LenaErrorModel : public NrLteMiErrorModel
{
  public:
    /**
     * \brief GetTypeId
     * \return the object type id
     */
    static TypeId GetTypeId();

    /**
     * \brief NrLteMiErrorModel constructor
     */
    LenaErrorModel();

    /**
     * \brief ~NrLteMiErrorModel
     */
    ~LenaErrorModel() override;

    /**
     * \brief Get the payload size, following the MCSs in LTE
     * \param usefulSC Useful Subcarriers (ignored)
     * \param mcs MCS
     * \param rank MIMO rank
     * \param rbNum Resource Block number (please pay attention)
     * \param mode UL or DL
     *
     * The RB value will be divided by 13.
     */
    uint32_t GetPayloadSize(uint32_t usefulSC,
                            uint8_t mcs,
                            uint8_t rank,
                            uint32_t rbNum,
                            Mode mode) const override;
};

} // namespace ns3

#endif // LENA_ERROR_MODEL_H
