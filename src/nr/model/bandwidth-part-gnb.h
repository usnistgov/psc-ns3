// Copyright (c) 2017 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef COMPONENT_CARRIER_GNB_H
#define COMPONENT_CARRIER_GNB_H

#include "nr-component-carrier.h"
#include "nr-gnb-phy.h"

#include <ns3/nstime.h>
#include <ns3/object.h>

namespace ns3
{

class NrGnbMac;
class NrMacScheduler;

/**
 * \ingroup gnb-bwp
 * \brief GNB bandwidth part representation
 *
 * Defines a single bandwidth part for the GNB.
 */
class BandwidthPartGnb : public NrComponentCarrier
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    BandwidthPartGnb();

    ~BandwidthPartGnb() override;

    /**
     * \return a pointer to the physical layer.
     */
    Ptr<NrGnbPhy> GetPhy();

    /**
     * \return a pointer to the MAC layer.
     */
    Ptr<NrGnbMac> GetMac();

    /**
     * \return a pointer to the Mac Scheduler.
     */
    Ptr<NrMacScheduler> GetScheduler();

    /**
     * Set the NrGnbPhy
     * \param s a pointer to the NrGnbPhy
     */
    void SetPhy(Ptr<NrGnbPhy> s);
    /**
     * Set the NrGnbMac
     * \param s a pointer to the NrGnbMac
     */
    void SetMac(Ptr<NrGnbMac> s);

    /**
     * Set the NrMacScheduler Algorithm
     * \param s a pointer to the NrMacScheduler
     */
    void SetNrMacScheduler(Ptr<NrMacScheduler> s);

    void SetDlBandwidth(uint16_t bw) override
    {
        m_dlBandwidth = bw;
    }

    void SetUlBandwidth(uint16_t bw) override
    {
        m_ulBandwidth = bw;
    }

    /**
     * \brief Set this bandwidth part as primary.
     * \param primaryCarrier true or false.
     *
     * Unfortunately, for the "false" value, the method will do nothing. Every carrier
     * starts as "not primary", so please, if you have to use SetAsPrimary (false)
     * think two times.
     */
    void SetAsPrimary(bool primaryCarrier);

    /**
     * Get cell identifier
     * \return cell identifier
     */
    uint16_t GetCellId() const;

    /**
     * Set physical cell identifier
     * \param cellId cell identifier
     */
    void SetCellId(uint16_t cellId);

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;

  private:
    Ptr<NrGnbPhy> m_phy;             ///< the Phy instance of this eNodeB component carrier
    Ptr<NrGnbMac> m_mac;             ///< the MAC instance of this eNodeB component carrier
    Ptr<NrMacScheduler> m_scheduler; ///< the scheduler instance of this eNodeB component carrier
    uint16_t m_cellId{0};            ///< Cell identifier
};

} // namespace ns3

#endif /* COMPONENT_CARRIER_H */
