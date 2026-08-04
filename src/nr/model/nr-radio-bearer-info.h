// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#ifndef NR_RADIO_BEARER_INFO_H
#define NR_RADIO_BEARER_INFO_H

#include "nr-eps-bearer.h"
#include "nr-rrc-sap.h"

#include <ns3/ipv4-address.h>
#include <ns3/object.h>
#include <ns3/pointer.h>

namespace ns3
{

class NrRlc;
class NrPdcp;

/**
 * store information on active radio bearer instance
 *
 */
class NrRadioBearerInfo : public Object
{
  public:
    NrRadioBearerInfo();
    ~NrRadioBearerInfo() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    Ptr<NrRlc> m_rlc;   ///< RLC
    Ptr<NrPdcp> m_pdcp; ///< PDCP
};

/**
 * store information on active signaling radio bearer instance
 *
 */
class NrSignalingRadioBearerInfo : public NrRadioBearerInfo
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    uint8_t m_srbIdentity;                                 ///< SRB identity
    NrRrcSap::LogicalChannelConfig m_logicalChannelConfig; ///< logical channel config
};

/**
 * store information on active data radio bearer instance
 *
 */
class NrDataRadioBearerInfo : public NrRadioBearerInfo
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrEpsBearer m_epsBearer;                               ///< EPS bearer
    uint8_t m_epsBearerIdentity;                           ///< EPS bearer identity
    uint8_t m_drbIdentity;                                 ///< DRB identity
    NrRrcSap::RlcConfig m_rlcConfig;                       ///< RLC config
    uint8_t m_logicalChannelIdentity;                      ///< logical channel identity
    NrRrcSap::LogicalChannelConfig m_logicalChannelConfig; ///< logical channel config
    uint32_t m_gtpTeid; /**< S1-bearer GTP tunnel endpoint identifier, see 36.423 9.2.1 */
    Ipv4Address m_transportLayerAddress; /**< IP Address of the SGW, see 36.423 9.2.1 */
};

} // namespace ns3

#endif // NR_RADIO_BEARER_INFO_H
