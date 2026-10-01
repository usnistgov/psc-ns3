// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#ifndef NR_SL_RADIO_BEARER_INFO_H
#define NR_SL_RADIO_BEARER_INFO_H

#include "nr-sl-tft.h"

#include "ns3/nr-radio-bearer-info.h"
#include "ns3/nr-rrc-sap.h"
#include <ns3/object.h>

namespace ns3
{

/**
 * \ingroup nr
 *
 * store information on active NR sidelink data radio bearer instance
 *
 */
class NrSlDataRadioBearerInfo : public NrRadioBearerInfo
{
  public:
    /**
     * \brief Get the type ID.
     *
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    uint8_t m_logicalChannelIdentity{0};                   //!< The logical channel identity
    NrRrcSap::LogicalChannelConfig m_logicalChannelConfig; //!< The logical channel configuration
    uint32_t m_sourceL2Id{0};                              //!< The Sidelink source L2 source id
    uint32_t m_destinationL2Id{0};                         //!< The Sidelink destination L2 id
};

/**
 * \ingroup nr
 *
 * store information on active NR sidelink signalling radio bearer instance
 *
 */
class NrSlSignallingRadioBearerInfo : public NrRadioBearerInfo
{
  public:
    /**
     * \brief Get the type ID.
     *
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    uint8_t m_logicalChannelIdentity{0};                   //!< The logical channel identity
    NrRrcSap::LogicalChannelConfig m_logicalChannelConfig; //!< The logical channel configuration
    uint32_t m_sourceL2Id{0};                              //!< The Sidelink source L2 source id
    uint32_t m_destinationL2Id{0};                         //!< The Sidelink destination L2 id
};

/**
 * \ingroup nr
 *
 * store information on active NR sidelink discovery radio bearer instance
 *
 */
class NrSlDiscoveryRadioBearerInfo : public NrRadioBearerInfo
{
  public:
    /**
     * \brief Get the type ID.
     *
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    uint8_t m_logicalChannelIdentity{0};                   //!< The logical channel identity
    NrRrcSap::LogicalChannelConfig m_logicalChannelConfig; //!< The logical channel configuration
    uint32_t m_sourceL2Id{0};                              //!< The Sidelink source L2 source id
    uint32_t m_destinationL2Id{0};                         //!< The Sidelink destination L2 id
};

} // namespace ns3

#endif // NR_SL_RADIO_BEARER_INFO_H
