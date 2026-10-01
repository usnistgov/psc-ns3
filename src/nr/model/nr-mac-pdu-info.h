// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_PDU_INFO_H
#define NR_MAC_PDU_INFO_H

#include "nr-phy-mac-common.h"

namespace ns3
{

/**
 * \ingroup ue-mac
 * \ingroup gnb-mac
 * \brief Used to track the MAC PDU with the slot in which has to go, and the DCI
 * that generated it
 *
 */
struct NrMacPduInfo
{
    /**
     * \brief Construct a NrMacPduInfo
     * \param sfn SfnSf of the PDU
     * \param dci DCI of the PDU
     */
    NrMacPduInfo(SfnSf sfn, std::shared_ptr<DciInfoElementTdma> dci)
        : m_sfnSf(sfn),
          m_dci(dci)
    {
    }

    SfnSf m_sfnSf;                             //!< SfnSf of the PDU
    std::shared_ptr<DciInfoElementTdma> m_dci; //!< The DCI
    uint32_t m_used{0};                        //!< Bytes sent down to PHY for this PDU
};

} // namespace ns3
#endif // NR_MAC_PDU_INFO_H
