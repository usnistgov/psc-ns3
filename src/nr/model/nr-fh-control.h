// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_FH_CONTROL_H
#define NR_FH_CONTROL_H

#include "nr-fh-phy-sap.h"
#include "nr-fh-sched-sap.h"

#include <ns3/object.h>

namespace ns3
{

class NrFhPhySapUser;
class NrFhPhySapProvider;
class NrFhSchedSapUser;
class NrFhSchedSapProvider;

/**
 * \ingroup
 * \brief Fronthaul Capacity Control
 *
 * This class is used to simulate a limited-capacity fronthaul (FH) link based
 * on the FhCapacity (m_fhCapacity) set by the user, and to apply FH control
 * methods (m_fhControlMethod) in order to restrict user allocations, if they do
 * not fit in the available FH capacity. Functional split 7.2x is assumed.
 *
 * Notice that for each gNB a NrFhControl instance is created, therefore, in
 * case that there are more than 1 BWPs defined, the FH link, and consequently
 * the configured FhCapacity will be shared among the active BWPs. For more
 * details have a look at the method SetCellFhCapacity().
 *
 * The NrFhControl can exchange information with the scheduler and the PHY layer
 * through the SAP interfaces NrFhSchedSapProvider/NrFhSchedSapUser and
 * NrFhPhySapProvider/NrFhPhySapUser, respectively.
 *
 * To enable the Fronthaul Capacity Control, the user must call in the example
 * the method NrHelper::EnableFhControl() and configure it, as desired, through
 * the NrHelper::SetFhControlAttribute(). Important note is that the method
 * NrHelper::ConfigureFhControl() must be called after the device install, so
 * that the NrFhControl can be configured correctly. In particular, with this
 * method the numerology and the error model of each BWP will be stored in the
 * NrFhControl maps.
 *
 * Let us point out, that the current implementation of the NrFhControl is
 * focused on DL traffic. In order to apply it for UL, there is the need for
 * further extensions. Moreover, current implementation supports only OFDMA.
 *
 */

class NrFhControl : public Object
{
  public:
    /**
     * \brief NrFhControl constructor
     */
    NrFhControl();

    /**
     * \brief ~NrFhControl deconstructor
     */
    ~NrFhControl() override;

    /**
     * \brief GetTypeId
     * \return the TypeId of the Object
     */
    static TypeId GetTypeId();

    /**
     * \brief Set the Fh control - PHY SAP User
     *        PHY is per bwp as such we store in a map the bwpId
     *        and the corresponding NrFhPhySapUser ptr.
     * \param bwpId The bwpId
     * \param s The ptr of the SAP User
     */
    void SetNrFhPhySapUser(uint16_t bwpId, NrFhPhySapUser* s);

    /**
     * \brief Get the Fh control - PHY SAP User ptr
     * \return the ptr of the SAP User
     */
    NrFhPhySapProvider* GetNrFhPhySapProvider();

    /**
     * \brief Set the Fh control - Sched SAP User
     *        Sched is per bwp as such we store in a map the bwpId
     *        and the corresponding SetNrFhSchedSapUser ptr.
     * \param bwpId The bwpId
     * \param s The ptr of the SAP User
     */
    void SetNrFhSchedSapUser(uint16_t bwpId, NrFhSchedSapUser* s);

    /**
     * \brief Get the Fh control - Sched SAP User ptr
     * \return the ptr of the SAP User
     */
    NrFhSchedSapProvider* GetNrFhSchedSapProvider();

    /// let the forwarder class access the protected and private members
    friend class MemberNrFhPhySapProvider<NrFhControl>;
    /// let the forwarder class access the protected and private members
    friend class MemberNrFhSchedSapProvider<NrFhControl>;

    /**
     * \brief The optimization models (FH Control method) of the NrFhControl
     */
    enum FhControlMethod
    {
        Dropping,    //!< Drop DCI + DATA at the PHY Layer
        Postponing,  //!< Postpone sending data (MAC Layer)
        OptimizeMcs, //!< Optimize MCS
        OptimizeRBs, //!< Optimize RBs allocated
    };

    /**
     * \brief Set the FH Control method type.
     * \param model The FH Control method type
     */
    void SetFhControlMethod(FhControlMethod model);

    /**
     * \brief Set the available fronthaul capacity of the cell.
     *        Notice that throughout the code, the capacity will
     *        be shared among all the active BWPs of the cell.
     *        ActiveBWPs are considered the BWPs that at least
     *        one of its UEs has data.
     * \param capacity The fronthaul capacity (in Mbps)
     */
    void SetCellFhCapacity(uint32_t capacity);

    /**
     * \brief Set the overhead for dynamic modulation compression
     * \param overhead The overhead for dynamic modulation compression (in bits)
     */
    void SetOverheadDyn(uint8_t overhead);

    /**
     * \brief Set the ErrorModelType based on which the MCS Table
     *        (1 or 2) will be set."
              "ns3::NrEesmIrT1 and ns3::NrEesmCcT1 for MCS Table 1"
              "ns3::NrEesmIrT2 and ns3::NrEesmCcT2 for MCS Table 2.
     * \param errorModelType The error model type
     */
    void SetErrorModelType(std::string errorModelType);

    /**
     * \brief Set the physical cell Id of the cell to which this NrFhControl
     *        instance belongs to.
     * \param physCellId The physical cell Id
     */
    void SetPhysicalCellId(uint16_t physCellId);

    /**
     * \brief Set the numerology
     * \param num the numerology
     *
     */
    void SetFhNumerology(uint16_t bwpId, uint16_t num);

  private:
    /**
     * \brief Get the FH Control method.
     * \return the FH Control method type
     */
    FhControlMethod GetFhControlMethod() const;

    /**
     * \brief Get the FH control method through the SAP interfaces.
     * \return the FH control method (uint8_t)
     */
    uint8_t DoGetFhControlMethod() const;

    /**
     * \brief Get the physical CellId for this NrFhControl instance.
     * \return the physical CellId
     */
    uint16_t DoGetPhysicalCellId() const;

    /**
     * \brief Set a UE as active (with data in RLC queue) for a slot, saving
     *        a map of the bwpId, and rnti for such a UE, and the amount of
     *        bytes in its RLC buffers.
     *
     * \param bwpId the BWP ID
     * \param rnti the RNTI
     * \param bytes the bytes in RLC buffers of the UE
     */
    void DoSetActiveUe(uint16_t bwpId, uint16_t rnti, uint32_t bytes);

    /**
     * \brief Updates the traces and the map of the UEs that have been served
     *        by this cell in a bwpId, based on the allocation and the scheduler
     *        UE map, respectively.
     *
     * \param bwpId the BWP ID
     * \param allocation the allocation structure of a slot
     * \param ueMap UE representation (in the scheduler)
     */
    void DoUpdateActiveUesMap(
        uint16_t bwpId,
        const std::deque<VarTtiAllocInfo>& allocation,
        const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap);

    /**
     * \brief Set a UE as active (with active HARQ) for a slot, saving
     *        a map of the bwpId, and rnti for such a UE.
     *
     * \param bwpId the BWP ID
     * \param rnti the RNTI
     */
    void DoSetActiveHarqUes(uint16_t bwpId, uint16_t rnti);

    /**
     * \brief Returns a boolean indicating whether the current allocation can
     *        fit in the available FH bandwidth.
     *
     * \param bwpId the BWP ID
     * \param rnti the allocated MCS
     * \param nRegs the number of allocated REGs (1 REGs = 1 RB (12 subcarriers) x 1 symbol)
     * \param dlRank the DL rank (number of MIMO layers)
     *
     * \return true if the current allocation can fit, false if not
     */
    bool DoGetDoesAllocationFit(uint16_t bwpId, uint32_t mcs, uint32_t nRegs, uint8_t dlRank);

    /**
     * \brief Returns the maximum MCS that can be assigned to a
     *        specific UE (rnti, bwpId) with a RB allocation.
     *
     * \param bwpId the BWP ID
     * \param reg the allocated REGs
     * \param rnti the RNTI
     * \param dlRank the DL rank (number of MIMO layers)
     *
     * \return the maximum MCS
     */
    uint8_t DoGetMaxMcsAssignable(uint16_t bwpId, uint32_t reg, uint32_t rnti, uint8_t dlRank);

    /**
     * \brief Returns the maximum number of REGs that can be assigned to a
     *        specific UE (rnti, bwpId) with a specific MCS (mcs).
     *
     * \param bwpId the BWP ID
     * \param mcs the MCS
     * \param rnti the RNTI
     * \param dlRank the DL rank (number of MIMO layers)
     *
     * \return the maximum number of REGs
     */
    uint32_t DoGetMaxRegAssignable(uint16_t bwpId, uint32_t mcs, uint32_t rnti, uint8_t dlRank);

    /**
     * \brief Updates the FH DL trace based on some dropped data+allocation,
     *        called from phy.
     *
     * \param bwpId the BWP ID
     * \param mcs the MCS
     * \param nRbgs the number of RBGs
     * \param nSymb the number of symbols
     * \param dlRank the DL rank (number of MIMO layers)
     */
    void DoUpdateTracesBasedOnDroppedData(uint16_t bwpId,
                                          uint32_t mcs,
                                          uint32_t nRbgs,
                                          uint32_t nSymb,
                                          uint8_t dlRank);

    /**
     * \brief End slot notification from gnb-phy, to track the required fronthaul
     *        throughput for that slot.
     *
     * \param currentSlot The current slot
     */
    void DoNotifyEndSlot(uint16_t bwpId, SfnSf currentSlot);

    /**
     * \brief Returns the FH throughput associated to a specific allocation.
     *
     * \param bwpId the BWP ID
     * \param mcs the allocated MCS
     * \param nRegs the number of allocated REGs (1 REGs = 1 RB (12 subcarriers) x 1 symbol)
     * \param dlRank the DL rank (number of MIMO layers)
     *
     * \return the calculated FH throughput
     */
    uint64_t GetFhThr(uint16_t bwpId, uint32_t mcs, uint32_t nRegs, uint8_t dlRank) const;

    /**
     * \brief Returns the number of all active BWPs, i.e., BWPs with new data
     *        in their RLC queues and BWPs with active HARQ.
     *
     * \return the number of active BWPs
     */
    uint16_t GetNumberActiveBwps() const;

    /**
     * \brief Returns the number of active UEs in a BWP
     *
     * \param bwpId The bwpId for which we want the number of active UEs
     * \return the number of active UEs in a BWP
     */
    uint16_t GetNumberActiveUes(uint16_t bwpId) const;

    /**
     * \brief Returns the max MCS based on the MCS Table (1 or 2)
     *        and the max modulation order.
     *
     * \param mcsTable The MCS table to use
     * \param modOrder The modulation order
     *
     * \return the max MCS
     */
    uint8_t GetMaxMcs(uint8_t mcsTable, uint16_t modOrder) const;

    /**
     * \brief Returns the modulation order of a specific mcs of MCS Table1.
     */
    uint32_t GetModulationOrderTable1(const uint32_t mcs) const;

    /**
     * \brief Returns the modulation order of a specific mcs of MCS Table2.
     */
    uint32_t GetModulationOrderTable2(const uint32_t mcs) const;

    /**
     * \brief Returns the max MCS index of a given modulation order of MCS Table1.
     */
    uint8_t GetMcsTable1(const uint8_t modOrder) const;

    /**
     * \brief Returns the max MCS index of a given modulation order of MCS Table2.
     */
    uint8_t GetMcsTable2(const uint8_t modOrder) const;

    uint16_t m_physicalCellId; //!< Physical cell ID to which the NrFhControl instance belongs to.

    // FH Control - PHY SAP
    std::map<uint16_t, NrFhPhySapUser*> m_fhPhySapUser; //!< FH Control - PHY SAP User (per bwpId)
    NrFhPhySapProvider* m_fhPhySapProvider;             //!< FH Control - PHY SAP Provider

    // FH Control - SCHEDULER SAP
    std::map<uint16_t, NrFhSchedSapUser*>
        m_fhSchedSapUser;                       //!< FH Control - PHY SAP User (per bwpId)
    NrFhSchedSapProvider* m_fhSchedSapProvider; //!< FH Control -  SCHED SAP Provider

    enum FhControlMethod m_fhControlMethod;
    uint32_t m_fhCapacity{
        1000}; //!< the available FH capacity (in Mbps) for DL and UL (full-duplex FH link)
    uint8_t m_overheadDyn{32};    //!< the overhead (OH) for dynamic adaptation (in bits)
    uint8_t m_mcsTable{2};        //!< the MCS table
    std::string m_errorModelType; //!< the error model type based on which the MCS Table will be set

    std::unordered_map<uint16_t, uint16_t> m_numerologyPerBwp; //!< Map of bwpIds and numerologies
    std::unordered_map<uint32_t, uint32_t>
        m_rntiQueueSize; //!< Map for the number of bytes in RLC queues of a specific UE (bwpId,
                         //!< rnti, bytes)
    // std::unordered_map<uint16_t, uint16_t>
    std::unordered_map<uint16_t, std::set<uint16_t>>
        m_activeUesPerBwp; //!< Map of active UEs (with new data) per BWP (bwpId, set of rntis)
    // std::unordered_map<uint16_t, uint16_t>
    //     m_activeBwps; //!< Map of active BWPs - with UEs with new data (bwpId, number of UEs)
    std::unordered_map<uint16_t, std::set<uint16_t>>
        m_activeHarqUesPerBwp; //!< Map of UEs with active HARQ per BWP (bwpId, set of rntis)
    // std::unordered_map<uint16_t, uint16_t>
    //     m_activeHarqBwps; //!< Map of active BWPs - with UEs with active HARQ (bwpId, number of
    //     UEs)

    uint64_t m_allocThrPerCell{0}; //!< the allocated fronthaul throughput after scheduling (in DL)
    std::unordered_map<uint16_t, uint64_t>
        m_allocThrPerBwp; //!< Map for FH allocated throughput of a specific bwpId (in DL)

    std::unordered_map<uint16_t, uint64_t>
        m_reqFhDlThrTracedValuePerBwp; //!< the required fronthaul throughput (in DL) per BWP
    std::unordered_map<uint16_t, uint32_t>
        m_rbsAirTracedValue; //!< Map for the used RBs of the air of a specific bwpId
    std::unordered_map<uint16_t, SfnSf> m_waitingSlotPerBwp;

    // SfnSf, physicalCellId, bwpId, FH throughput
    TracedCallback<const SfnSf&, uint16_t, uint16_t, uint64_t>
        m_reqFhDlThrTrace; //!< Report the required FH throughput (in DL) per BWP
    // SfnSf, physicalCellId, bwpId, RBs used
    TracedCallback<const SfnSf&, uint16_t, uint16_t, uint32_t>
        m_rbsAirTrace; //!< Report the RBs used of the AI (in DL) per BWP
};

} // end namespace ns3

#endif // NR_FH_CONTROL_H
