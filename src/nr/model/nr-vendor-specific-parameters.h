// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Marco Miozzo  <marco.miozzo@cttc.es>
///

#ifndef NR_VENDOR_SPECIFIC_PARAMETERS
#define NR_VENDOR_SPECIFIC_PARAMETERS

#include "nr-phy-mac-common.h"

#define SRS_CQI_RNTI_VSP 1

namespace ns3
{

/**
 * \brief Define the RNTI that has generated the
 */
class NrSrsCqiRntiVsp : public nr::VendorSpecificValue
{
  public:
    /**
     * \brief SRS CQI RNTI VSP
     *
     * \param rnti the RNTI
     */
    NrSrsCqiRntiVsp(uint16_t rnti);
    ~NrSrsCqiRntiVsp() override;

    /**
     * \brief Get RNTI function
     *
     * \returns the RNTI
     */
    uint16_t GetRnti() const;

  private:
    uint16_t m_rnti; ///< the rnti
};

}; // namespace ns3

#endif /* NR_VENDOR_SPECIFIC_PARAMETERS */
