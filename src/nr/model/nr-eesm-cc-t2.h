// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_EESM_CC_T2_H
#define NR_EESM_CC_T2_H

#include "nr-eesm-cc.h"
#include "nr-eesm-t2.h"

namespace ns3
{

/**
 * \ingroup error-models
 * \brief The NrEesmCcT2 class
 *
 * Class that implements the CC-HARQ combining with Table 2. It can be used
 * directly in the code.
 */
class NrEesmCcT2 : public NrEesmCc
{
  public:
    /**
     * \brief Get the type id of the object
     * \return the type id of the object
     */
    static TypeId GetTypeId();
    /**
     * \brief NrEesmCcT2 constructor
     */
    NrEesmCcT2();
    /**
     * \brief ~NrEesmCcT2 deconstructor
     */
    ~NrEesmCcT2() override;

  protected:
    const std::vector<double>* GetBetaTable() const override;
    const std::vector<double>* GetMcsEcrTable() const override;
    const SimulatedBlerFromSINR* GetSimulatedBlerFromSINR() const override;
    const std::vector<uint8_t>* GetMcsMTable() const override;
    const std::vector<double>* GetSpectralEfficiencyForMcs() const override;
    const std::vector<double>* GetSpectralEfficiencyForCqi() const override;

  private:
    NrEesmT2 m_t2; //!< The reference table
};

} // namespace ns3

#endif // NR_EESM_CC_T2_H
