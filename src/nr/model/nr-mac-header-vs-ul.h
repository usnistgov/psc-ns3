// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_HEADER_VS_UL_H
#define NR_MAC_HEADER_VS_UL_H

#include "nr-mac-header-vs.h"

namespace ns3
{

/**
 * \ingroup ue-mac
 * \ingroup gnb-mac
 * \brief Mac variable-size Header for UL
 *
 * This header performs some sanity check for the LCID value, but the functionality
 * is almost the same as NrMacHeaderVs. Please note that, by standard, only
 * some LCID can be used in UL transmissions.
 *
 * Please refer to TS 38.321 section 6.1.2 for more information.
 *
 * <b>Users, don't use this header directly: you've been warned.</b>
 *
 * \internal
 *
 * This header must be used to report some variable-sized CE to the GNB. At
 * the moment, we don't use it.
 */
class NrMacHeaderVsUl : public NrMacHeaderVs
{
  public:
    /**
     * \brief GetTypeId
     * \return the type id of the object
     */
    static TypeId GetTypeId();
    /**
     * \brief GetInstanceTypeId
     * \return the instance type id
     */
    TypeId GetInstanceTypeId() const override;

    /**
     * \brief NrMacHeaderVsUl constructor
     */
    NrMacHeaderVsUl();

    /**
     * \brief ~NrMacHeaderVsUl
     */
    ~NrMacHeaderVsUl() override;

    // const uint8_t CCCH_LARGE  = 0, //!< CCCH of size 64 bit (is it fixed or variable?)
    // const uint8_t CCCH_SMALL = 52, //!< CCCH of size 48 (is it fixed or variable?)

    static const uint8_t MULTIPLE_ENTRY_PHR_FOUR_OCTET =
        54;                                                 //!< Multiple entry PHR (four octet C_i)
    static const uint8_t MULTIPLE_ENTRY_PHR_ONE_OCTET = 56; //!< Multiple entry PHR (one octet C_i)
    static const uint8_t LONG_TRUNCATED_BSR = 60;           //!< Long Truncated BSR
    static const uint8_t LONG_BSR = 62;                     //!< Long BSR

    /**
     * \brief Set the LC ID
     * \param lcId LC ID
     *
     * It will assert if the value is not inside the vector of allowed one.
     * To not make any error, please use one of the pre-defined const values in
     * this class.
     */
    void SetLcId(uint8_t lcId) override;

    /**
     * \brief Check if it really a variable-size header
     * \return true if the lcId value stored internally matches with a variable-size header
     */
    bool IsVariableSizeHeader() const;
};

} // namespace ns3

#endif /* NR_MAC_HEADER_VS_UL_H */
