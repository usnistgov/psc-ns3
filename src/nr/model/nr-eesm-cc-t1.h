// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_EESM_CC_T1_H
#define NR_EESM_CC_T1_H

#include "nr-eesm-cc.h"
#include "nr-eesm-t1.h"

namespace ns3
{

/**
 * \ingroup error-models
 * \brief The NrEesmCcT1 class
 *
 * Class that implements the IR-HARQ combining with Table 1. It can be used
 * directly in the code.
 */
class NrEesmCcT1 : public NrEesmCc
{
  public:
    /**
     * \brief Get the type id of the object
     * \return the type id of the object
     */
    static TypeId GetTypeId();
    /**
     * \brief NrEesmCcT1 constructor
     */
    NrEesmCcT1();
    /**
     * \brief ~NrEesmCcT1 deconstructor
     */
    ~NrEesmCcT1() override;

  protected:
    const std::vector<double>* GetBetaTable() const override;
    const std::vector<double>* GetMcsEcrTable() const override;
    const SimulatedBlerFromSINR* GetSimulatedBlerFromSINR() const override;
    const std::vector<uint8_t>* GetMcsMTable() const override;
    const std::vector<double>* GetSpectralEfficiencyForMcs() const override;
    const std::vector<double>* GetSpectralEfficiencyForCqi() const override;

  private:
    NrEesmT1 m_t1; //!< The reference table
};

} // namespace ns3

#endif // NR_EESM_CC_T2_H
