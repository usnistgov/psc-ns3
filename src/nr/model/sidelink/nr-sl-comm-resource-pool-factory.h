// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_COMM_RESOURCE_POOL_FACTORY_H
#define NR_SL_COMM_RESOURCE_POOL_FACTORY_H

#include "nr-sl-rrc-sap.h"

#include <ns3/simple-ref-count.h>

#include <bitset>

namespace ns3
{

/** Class to configure and generate resource pools */
class NrSlCommResourcePoolFactory : public SimpleRefCount<NrSlCommResourcePoolFactory>
{
  public:
    NrSlCommResourcePoolFactory();
    virtual ~NrSlCommResourcePoolFactory();

    /**
     * \brief Create pool
     *
     * Until 3GPP TS 38.331 is not making a clear distinction between a dedicated
     * and preconfigured pool, we are making this method virtual so child class
     * can override it.
     *
     * \return The struct of type NrSlRrcSap::SlResourcePoolNr defining the SL pool
     */
    virtual const NrSlRrcSap::SlResourcePoolNr CreatePool();

    /**
     * \brief Setup PSCCH resources
     */
    void SetupPscchResources();
    /**
     * \brief Is setup PscchResources
     * \return flag indicating if it is setting up or releasing PSCCH resources
     */
    bool IsSetupPscchResources() const;
    /**
     * \brief Get sidelink time resource PSCCH
     * \return The number of symbols for PSCCH transmission
     */
    uint16_t GetSlTimeResourcePscch() const;
    /**
     * \brief Set sidelink time resource PSCCH
     * \param slTimeResourcePscch The number of symbols for PSCCH transmission
     */
    void SetSlTimeResourcePscch(uint16_t slTimeResourcePscch);
    /**
     * \brief Get sidelink frequency resource PSCCH
     * \return The number of resource blocks allocated to PSCCH transmission
     */
    uint16_t GetSlFreqResourcePscch() const;
    /**
     * \brief Set sidelink frequency resource PSCCH
     * \param  slFreqResourcePscch The number of resource blocks allocated to PSCCH transmission
     */
    void SetSlFreqResourcePscch(uint16_t slFreqResourcePscch);
    /**
     * \brief Get sidelink subchannel size
     * \return The subchannel size in RBs
     */
    uint16_t GetSlSubchannelSize() const;
    /**
     * \brief Set sidelink subchannel size
     * \param slSubchannelSize The subchannel size in RBs
     */
    void SetSlSubchannelSize(uint16_t slSubchannelSize);
    /**
     * \brief Get sidelink sensing window
     * \return The sensing window start time in milliseconds
     */
    uint16_t GetSlSensingWindow() const;
    /**
     * \brief Set sidelink sensing window
     * \param slSensingWindow The sensing window start time in milliseconds
     */
    void SetSlSensingWindow(uint16_t slSensingWindow);
    /**
     * \brief Get Sidelink selection window
     * \return The T2min of the end of the selection window in milliseconds
     */
    uint16_t GetSlSelectionWindow() const;
    /**
     * \brief Set Sidelink selection window
     *
     * This corresponds to sl-SelectionWindowList-r16 in TS 38.331 and is an enumerated
     * value with values of {n1, n5, n10, n20}.  The value is later scaled to units of slots.
     *
     * \param slSelectionWindow The T2min of the end of the selection window in milliseconds
     */
    void SetSlSelectionWindow(uint16_t slSelectionWindow);
    /**
     * \brief Get Sidelink resource reservation period list
     *
     * \return A list of possible resource reservation period allowed in the resource pool
     */
    const std::list<uint16_t>& GetSlResourceReservePeriodList() const;
    /**
     * \brief Set Sidelink resource reservation period list
     *
     * \param slResourceReservePeriodList A list of possible resource reservation period
     * allowed in the resource pool
     */
    void SetSlResourceReservePeriodList(std::list<uint16_t>& slResourceReservePeriodList);
    /**
     * \brief Get Sidelink time resources
     *
     * \return A vector representing a bitmap for sidelink time resources
     */
    const std::vector<std::bitset<1>>& GetSlTimeResources() const;
    /**
     * \brief Set Sidelink time resources
     *
     * \param slBitmap A vector representing a bitmap for sidelink time resources
     */
    void SetSlTimeResources(std::vector<std::bitset<1>>& slBitmap);
    /**
     * \brief Get Sidelink maximum number resource reserve for PSCCH/PSSCH
     *
     * \return The maximum number of reserved PSCCH/PSSCH resources that can be
     *         indicated by an SCI.
     */
    uint16_t GetSlMaxNumPerReserve() const;
    /**
     * \brief Set Sidelink maximum number resource reserve for PSCCH/PSSCH
     *
     * \param maxNumPerReserve The maximum number of reserved PSCCH/PSSCH
     *        resources that can be indicated by an SCI.
     */
    void SetSlMaxNumPerReserve(uint16_t maxNumPerReserve);
    /**
     * \brief Get Sidelink PSFCH period value (slots)
     *
     * Valid values are 0 (no PSFCH or HARQ), 1, 2, or 4 slots.
     *
     * \return The PSFCH period value in slots
     */
    uint16_t GetSlPsfchPeriod() const;
    /**
     * \brief Set Sidelink PSFCH period value (slots)
     *
     * Valid values are 0 (no PSFCH or HARQ), 1, 2, or 4 slots.
     *
     * \param psfchPeriod The PSFCH period value in slots
     */
    void SetSlPsfchPeriod(uint16_t psfchPeriod);
    /**
     * \brief Get Sidelink PSFCH minimum time gap interval (slots)
     *
     * Valid values are 2 or 3 slots.
     *
     * \return The PSFCH minimum time gap interval, in slots
     */
    uint16_t GetSlMinTimeGapPsfch() const;
    /**
     * \brief Set Sidelink PSFCH minimum time gap interval (slots)
     *
     * Valid values are 2 or 3 slots.
     *
     * \param minTimeGap The PSFCH minimum time gap interval, in slots
     */
    void SetSlMinTimeGapPsfch(uint16_t minTimeGap);

  private:
    NrSlRrcSap::SlResourcePoolNr m_pool; //!< Sidelink communication pool

  protected:
    // SlPscchConfig
    std::string m_setupReleasePscch; //!< Indicates if it is allocating or releasing PSCCH resources
    uint16_t
        m_slTimeResourcePscch; //!< Indicates the number of symbols of PSCCH in a resource pool.
    uint16_t
        m_slFreqResourcePscch; //!< Indicates the number of PRBs for PSCCH in a resource pool where
                               //!< it is not greater than the number PRBs of the subchannel.

    uint16_t m_slSubchannelSize; //!< Sidelink subchannel size in PRBs

    uint16_t m_slSensingWindow;   //!< Start of the sensing window in milliseconds.
    uint16_t m_slSelectionWindow; //!< T2min of the end of the selection window in milliseconds.

    std::list<uint16_t> m_slResourceReservePeriodList; //!< List of possible resource reservation
                                                       //!< period allowed in the resource pool.
    std::vector<std::bitset<1>> m_slTimeResource;      //!< The sidelink time resource bitmap
    uint16_t m_slMaxNumPerReserve; //!< The maximum number of reserved PSCCH/PSSCH resources that
                                   //!< can be indicated by an SCI.

    uint16_t m_slPsfchPeriod;     //!< PSFCH period value
    uint16_t m_slMinTimeGapPsfch; //!< Minimum Time Gap PSFCH value
};

} // namespace ns3

#endif /* NR_SL_COMM_RESOURCE_POOL_FACTORY_H */
