/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_SL_UE_CPHY_SAP_H
#define NR_SL_UE_CPHY_SAP_H

#include <ns3/ptr.h>
#include <ns3/simulator.h>

#include <stdint.h>

namespace ns3
{

class NrSlCommResourcePool;

/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC
 * for control purposes of NR Sidelink
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the PHY methods called by the RRC
 */
class NrSlUeCphySapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeCphySapProvider();

    // Sidelink Communication
    /**
     * \brief Add NR Sidelink communication transmission pool
     *
     * Adds transmission pool for NR Sidelink communication
     *
     * \param txPool The pointer to the NrSlCommResourcePool
     */
    virtual void AddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool) = 0;
    /**
     * \brief Add NR Sidelink communication reception pool
     *
     * Adds reception pool for NR Sidelink communication
     *
     * \param rxPool The pointer to the NrSlCommResourcePool
     */
    virtual void AddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool) = 0;
    /**
     * \brief Ask the PHY the bandwidth in RBs
     *
     * \return the bandwidth in RBs
     */
    virtual uint32_t GetBwInRbs() const = 0;
    /**
     * \brief Ask the PHY to enable the RSRP measurements
     */
    virtual void EnableUeSlRsrpMeasurements() = 0;
    /**
     * \brief Ask the PHY to disable the RSRP measurements
     */
    virtual void DisableUeSlRsrpMeasurements() = 0;
};

/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlUeCphySapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE PHY class, which are called by UE RRC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeCphySapProvider : public NrSlUeCphySapProvider
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner The owner class
     */
    MemberNrSlUeCphySapProvider(C* owner);
    MemberNrSlUeCphySapProvider() = delete;

    // methods inherited from NrSlUeCphySapProvider go here
    // NR Sidelink communication
    void AddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool) override;
    void AddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool) override;
    uint32_t GetBwInRbs() const override;
    void EnableUeSlRsrpMeasurements() override;
    void DisableUeSlRsrpMeasurements() override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeCphySapProvider<C>::MemberNrSlUeCphySapProvider(C* owner)
    : m_owner(owner)
{
}

// Sidelink communication

template <class C>
void
MemberNrSlUeCphySapProvider<C>::AddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool)
{
    m_owner->DoAddNrSlCommTxPool(txPool);
}

template <class C>
void
MemberNrSlUeCphySapProvider<C>::AddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool)
{
    m_owner->DoAddNrSlCommRxPool(rxPool);
}

template <class C>
uint32_t
MemberNrSlUeCphySapProvider<C>::GetBwInRbs() const
{
    return m_owner->DoGetBwInRbs();
}

template <class C>
void
MemberNrSlUeCphySapProvider<C>::EnableUeSlRsrpMeasurements()
{
    m_owner->DoEnableUeSlRsrpMeasurements();
}

template <class C>
void
MemberNrSlUeCphySapProvider<C>::DisableUeSlRsrpMeasurements()
{
    m_owner->DoDisableUeSlRsrpMeasurements();
}

/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC
 * for control purposes of NR Sidelink
 *
 * This is the CPHY SAP User, i.e., the part of the SAP that contains the RRC
 * methods called by the PHY
 */
class NrSlUeCphySapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeCphySapUser();

    /**
     *  Structure used by the Phy to report RSRP measurements to the RRC
     *  for relay selection purposes
     */
    struct RsrpElement
    {
        uint32_t l2Id;
        double rsrp;
    };

    /**
     *  Structure used to store RSRP measurements to be reported to the RRC by the PHY
     *  for relay selection purposes
     */
    struct RsrpElementsList
    {
        std::vector<struct RsrpElement> rsrpMeasurementsList; ///< List of the RSRP measurements
    };

    /**
     *  \brief Repot RSRP measurements to the RRC
     *
     *  \param l the structure containing a list of RSRP measuremnet elements,
     *           including the peer UE L2ID (relay) and related RSRP value
     */
    virtual void ReceiveUeSlRsrpMeasurements(RsrpElementsList l) = 0;
    /**
     * \brief Set L1 measurement period in the RRC
     *
     * \param period L1 RSRP filter period
     */
    virtual void SetRsrpFilterPeriod(Time period) = 0;
};

/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlUeCphySapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE RRC class, which are called by UE PHY
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeCphySapUser : public NrSlUeCphySapUser
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner The owner class
     */
    MemberNrSlUeCphySapUser(C* owner);

    // methods inherited from NrSlUeCphySapUser go here
    void ReceiveUeSlRsrpMeasurements(RsrpElementsList l) override;
    void SetRsrpFilterPeriod(Time period) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeCphySapUser<C>::MemberNrSlUeCphySapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrSlUeCphySapUser<C>::ReceiveUeSlRsrpMeasurements(RsrpElementsList l)
{
    m_owner->DoReceiveUeSlRsrpMeasurements(l);
}

template <class C>
void
MemberNrSlUeCphySapUser<C>::SetRsrpFilterPeriod(Time period)
{
    m_owner->DoSetRsrpFilterPeriod(period);
}

} // namespace ns3

#endif // NR_SL_UE_CPHY_SAP_H
