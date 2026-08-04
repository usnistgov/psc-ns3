/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 */

#ifndef NR_SL_COMM_RESOURCE_POOL
#define NR_SL_COMM_RESOURCE_POOL

#include "nr-sl-ue-rrc.h"

#include "ns3/nr-rrc-sap.h"
#include <ns3/object.h>

#include <array>
#include <bitset>
#include <list>
#include <optional>
#include <stdint.h>
#include <unordered_map>

namespace ns3
{

class NrSlCommResourcePool : public Object
{
  public:
    /**
     * \brief NR Sidelink slot info.
     */
    struct SlotInfo
    {
        /**
         * \brief NrSlSlotInfo constructor
         * \param numSlPscchRbs Indicates the number of PRBs for PSCCH in a resource pool where it
         * is not greater than the number PRBs of the subchannel. \param slPscchSymStart Indicates
         * the starting symbol used for sidelink PSCCH in a slot \param slPscchSymLength Indicates
         * the total number of symbols available for sidelink PSCCH \param slPsschSymStart Indicates
         * the starting symbol used for sidelink PSSCH in a slot \param slPsschSymLength Indicates
         * the total number of symbols available for sidelink PSSCH \param slSubchannelSize
         * Indicates the subchannel size in number of RBs \param slHasPsfch Indicates whether PSFCH
         * is present in the slot \param slMaxNumPerReserve Indicates the maximum number of reserved
         * PSCCH/PSSCH resources that can be indicated by an SCI. \param absSlotIndex Indicates the
         * the absolute slot index \param slotOffset Indicates the positive offset between two slots
         */
        SlotInfo(uint16_t numSlPscchRbs,
                 uint16_t slPscchSymStart,
                 uint16_t slPscchSymLength,
                 uint16_t slPsschSymStart,
                 uint16_t slPsschSymLength,
                 bool slHasPsfch,
                 uint16_t slSubchannelSize,
                 uint16_t slMaxNumPerReserve,
                 uint64_t absSlotIndex,
                 uint32_t slotOffset)
        {
            this->numSlPscchRbs = numSlPscchRbs;
            this->slPscchSymStart = slPscchSymStart;
            this->slPscchSymLength = slPscchSymLength;
            this->slPsschSymStart = slPsschSymStart;
            this->slPsschSymLength = slPsschSymLength;
            this->slHasPsfch = slHasPsfch;
            this->slSubchannelSize = slSubchannelSize;
            this->slMaxNumPerReserve = slMaxNumPerReserve;
            this->absSlotIndex = absSlotIndex;
            this->slotOffset = slotOffset;
        }

        // PSCCH
        uint16_t numSlPscchRbs{
            0}; //!< Indicates the number of PRBs for PSCCH in a resource pool where it is not
                //!< greater than the number PRBs of the subchannel.
        uint16_t slPscchSymStart{
            0}; //!< Indicates the starting symbol used for sidelink PSCCH in a slot
        uint16_t slPscchSymLength{
            0}; //!< Indicates the total number of symbols available for sidelink PSCCH
        // PSSCH
        uint16_t slPsschSymStart{
            0}; //!< Indicates the starting symbol used for sidelink PSSCH in a slot
        uint16_t slPsschSymLength{
            0}; //!< Indicates the total number of symbols available for sidelink PSSCH
        bool slHasPsfch{false}; //!< Indicates whether PSFCH is present in the slot
        // subchannel size in RBs
        uint16_t slSubchannelSize{0};   //!< Indicates the subchannel size in number of RBs
        uint16_t slMaxNumPerReserve{0}; //!< The maximum number of reserved PSCCH/PSSCH resources
                                        //!< that can be indicated by an SCI.
        uint64_t absSlotIndex{0};       //!< Indicates the the absolute slot index
        uint32_t slotOffset{0};         //!< Indicates the positive offset between two slots
    };

    /**
     * Scheduling types of sidelink pools hold by this class. At one time,
     * either all the pools will be used for UE_SELECTED scheduling or
     * network SCHEDULED. There can not be mix-up.
     */
    enum SchedulingType
    {
        UNKNOWN = 0,
        SCHEDULED,
        UE_SELECTED
    };

    /**
     * \brief Map to store the physical SL pool per BWP and per SL pool
     *
     * Key of the first map is the BWP id
     * Value of the first map is an unordered map storing physical SL pool per SL pool
     * Key of the second map is the pool id
     * Value of the second map is a vector, which is a physical SL pool
     */
    typedef std::unordered_map<uint8_t, std::unordered_map<uint16_t, std::vector<std::bitset<1>>>>
        PhySlPoolMap;

    /**
     * \brief Checks if two NR Sidelink pool configurations are identical
     *
     * \param other The configuration of the other resource pool
     * \return true if this configuration is the same as the other one
     */
    bool operator==(const NrSlCommResourcePool& other) const;

    /**
     * \brief A struct containing the iterator of the BWP map and the pool
     *        associated to the BWP.
     */
    struct BwpAndPoolIt
    {
        NrSlCommResourcePool::PhySlPoolMap::const_iterator
            itBwp; //!< Iterator to the first map in PhySlPoolMap
        std::unordered_map<uint16_t, std::vector<std::bitset<1>>>::const_iterator
            itPool; //!< Iterator to the second map in PhySlPoolMap
    };

    /**
     * \brief Constructor
     */
    NrSlCommResourcePool();
    /**
     * \brief Destructor
     */
    ~NrSlCommResourcePool() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Set NR Sidelink preconfiguration  frequency information list
     *
     * \param slPreconfigFreqInfoList A list containing per carrier configuration for NR sidelink
     * communication
     */
    void SetNrSlPreConfigFreqInfoList(
        const std::array<NrSlRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL>&
            slPreconfigFreqInfoList);
    /**
     * \brief Set NR Sidelink physical pool map
     *
     * \param phySlPoolMap The Map containing the physical SL pool per BWP and per SL pool
     */
    void SetNrSlPhysicalPoolMap(NrSlCommResourcePool::PhySlPoolMap phySlPoolMap);
    /**
     * \brief Get NR Sidelink T2min parameter (in slots)
     * This value derives from the SelectionWindow value configured in
     * SL-UE-SelectedConfigRP, and scaled by the numerology.
     *
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \param numerology The numerology
     * \return The value of T2min in slots
     */
    uint16_t GetT2Min(uint8_t bwpId, uint16_t poolId, uint16_t numerology) const;
    /**
     * \brief Get NR Sidelink physical sidelink pool
     *
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return A vector representing the the physical sidelink pool
     */
    const std::vector<std::bitset<1>> GetNrSlPhyPool(uint8_t bwpId, uint16_t poolId) const;
    /**
     * \brief Get NR Sidelink communication opportunities
     *
     * TS 38.214 8.1.2.1 <b>"Within the slot, PSSCH resource allocation starts at symbol
     * startSLsymbols+1"</b> Since we are multiplexing the PSCCH and PSSCH in time, we are already
     * taking the TS 38.214 (8.1.2.1) into the consideration. That is,
     * <b>1.</b> \f$number of PSSCH symbols = Total symbols - number of PSCCH symbols\f$
     * TS 38.214 8.1.2.1 also says <b>"The UE shall not transmit PSSCH in the last symbol configured
     * for sidelink.</b> Therefore, we to subtract one more symbol from the PSSCH symbols, i.e.,
     * <b>2.</b> \f$Total number of PSSCH symbols = number of PSSCH symbols (from 1) - 1\f$
     *
     * \param absIndexCurretSlot The current absolute slot number
     * \param bwpId The bandwidth part id
     * \param numerology The numerology
     * \param poolId The pool id
     * \param t1 The start of the selection window in physical slots, accounting for physical layer
     * processing delay \param t2 The end of the selection window in physical slots \return A list
     * of sidelink communication opportunities for each available slot in a selection window
     */
    std::list<NrSlCommResourcePool::SlotInfo> GetNrSlCommOpportunities(uint64_t absIndexCurretSlot,
                                                                       uint8_t bwpId,
                                                                       uint16_t numerology,
                                                                       uint16_t poolId,
                                                                       uint8_t t1,
                                                                       uint16_t t2) const;
    /**
     * \brief Get NR Sidelink sensing window in slots
     *
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \param slotLength The slot length in time
     * \return The length of the sensing window in slots
     */
    uint16_t GetNrSlSensWindInSlots(uint8_t bwpId, uint16_t poolId, Time slotLength) const;
    /**
     * \brief Set NR Sidelink scheduling type to be used for the pools
     *
     * \param type The scheduling type [NrSlCommResourcePool::SchedulingType]
     */
    void SetNrSlSchedulingType(NrSlCommResourcePool::SchedulingType type);
    /**
     * \brief Get NR Sidelink scheduling type used for the pools
     *
     * \return The scheduling type [NrSlCommResourcePool::SchedulingType]
     */
    NrSlCommResourcePool::SchedulingType GetNrSlSchedulingType() const;
    /**
     * \brief Get NR Sidelink subchannel size
     *
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The subchannel size in RBs
     */
    uint16_t GetNrSlSubChSize(uint8_t bwpId, uint16_t poolId) const;
    /**
     * \brief Validate that the given resource reservation period is in the list
     *        of user provided configuration. If it is not in the list an assert
     *        will hit.
     * \param bwpId The bandwidth part id
     * \param poolId The poolId The pool id
     * \param resvPeriod The resource reservation period
     * \param slotLength The slot length in time
     */
    void ValidateResvPeriod(uint8_t bwpId, uint16_t poolId, Time resvPeriod, Time slotLength) const;
    /**
     * \brief Get the resource reservation period list
     * \param bwpId The bandwidth part id
     * \param poolId The poolId The pool id
     * \return The resource reservation period list (units of ms)
     */
    std::list<uint16_t> GetSlResourceReservePeriodList(uint8_t bwpId, uint16_t poolId) const;
    /**
     * \brief Get the reservation period in slots
     * \param bwpId The bandwidth part id
     * \param poolId The poolId The pool id
     * \param resvPeriod The reservation period in ms
     * \param slotLength The slot length in time
     * \return The reservation period in slots
     */
    uint16_t GetResvPeriodInSlots(uint8_t bwpId,
                                  uint16_t poolId,
                                  Time resvPeriod,
                                  Time slotLength) const;
    /**
     * \brief Is Sidelink slot
     * \param bwpId The bandwidth part id
     * \param poolId The poolId The pool id
     * \param absSlotIndex The current absolute slot index
     * \return true if it is the Sidelink slot, false otherwise
     */
    bool IsSidelinkSlot(uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const;
    /**
     * \brief Get Absolute index of the pool slot as pet the current absolute slot
     * \param bwpId The bandwidth part id
     * \param poolId The poolId The pool id
     * \param absSlotIndex The current absolute slot index
     * \return The absolute index of the pool slot
     */
    uint16_t GetAbsPoolIndex(uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const;
    /**
     * \brief Set the TDD pattern.
     * \param tddPattern The TDD pattern
     *
     * For example, a valid pattern would be "DL|DL|UL|UL|DL|DL|UL|UL|". The slot
     * types allowed are:
     *
     * - "DL" for downlink only
     * - "UL" for uplink only
     * - "F" for flexible (dl and ul)
     * - "S" for special slot (LTE-compatibility)
     */
    void SetTddPattern(std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern);
    /**
     * \brief Get Nr Sidelink PSSCH subchannel size in RBs
     * \param bwpId bwpId The bandwidth part id
     * \param poolId poolId The poolId The pool id
     * \return PSSCH subchannel size in RBs
     */
    uint16_t GetSlSubChSize(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Check if slot indexed by absIndexCurrentSlot is a slot with PSFCH
     *
     * This method checks whether a SL slot is configured for PSFCH, based on
     * the value of slPsfchPeriod configured in the resource pool.
     *
     * \param absIndexCurrentSlot absolute slot count from simulation time 0
     * \param bwpId bandwidth part ID (to identify the pool)
     * \param poolId pool ID
     * \return true if the indicated slot has PSFCH, false otherwise
     */
    bool SlotHasPsfch(uint64_t absIndexCurrentSlot, uint8_t bwpId, uint16_t poolId) const;

    /**
     * Return the MinTimeGapPsfch associated with the BWP and pool
     *
     * \param bwpId bandwidth part ID (to identify the pool)
     * \param poolId pool ID
     * \return value of the MinTimeGapPsfch, in slots
     */
    uint8_t GetMinTimeGapPsfch(uint8_t bwpId, uint16_t poolId) const;

    /**
     * Return the PsfchPeriod associated with the BWP and pool
     *
     * \param bwpId bandwidth part ID (to identify the pool)
     * \param poolId pool ID
     * \return value of the PsfchPeriod, in slots
     */
    uint8_t GetPsfchPeriod(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Get transport block size for given MCS and number of subchannels
     *
     * Implements TS 38.214 Section 8.1.3.2 for PSSCH TBS calculation.
     * Accounts for SCI1 and SCI2 overhead. When slHasPsfch is true,
     * the reduced PSSCH symbol count (due to PSFCH) is already reflected
     * in slotInfo.slPsschSymLength.
     *
     * \note MAC overhead is not accounted for. The returned TBS represents
     * the raw MAC PDU capacity at the MAC-PHY interface. The caller must
     * subtract MAC header and subheader overhead to determine usable payload.
     *
     * \note This implementation hardcodes the number of DMRS symbols/slot to 2,
     * and the rank to 1; these could potentially be added as configurable resource
     * pool parameters at a future date.
     *
     * \note Some transport block calculations have been checked against values calculated by the
     * <a href="https://www.nist.gov/services-resources/software/new-radio-sidelink-simulator">
     * NIST New Radio Sidelink Simulator</a>.
     *
     * \param slotInfo Sidelink slot information (PSCCH/PSSCH/PSFCH config)
     * \param mcs MCS index (0-28 for Table 1, 0-27 for Table 2)
     * \param nSubchannels Number of subchannels
     * \param mcsTable MCS table to use (1 for 64QAM, 2 for 256QAM per TS 38.214)
     * \return TBS in bytes (= MAC PDU capacity, excluding MAC overhead)
     */
    uint32_t GetTransportBlockSize(const SlotInfo& slotInfo,
                                   uint8_t mcs,
                                   uint16_t nSubchannels,
                                   uint8_t mcsTable) const;

    /**
     * \brief Get minimum subchannels needed for a given transport block size
     *
     * \param slotInfo Sidelink slot information (PSCCH/PSSCH/PSFCH config)
     * \param mcs MCS index
     * \param transportBlockSize Transport block size in bytes (MAC PDU including overhead)
     * \param maxSubchannels Maximum number of subchannels available
     * \param mcsTable MCS table to use (1 for 64QAM, 2 for 256QAM per TS 38.214)
     * \note In sidelink mode 2, MCS table 1 should be used
     * \return Minimum number of subchannels to fit transportBlockSize bytes,
     *         or std::nullopt if transportBlockSize cannot fit into the
     *         maximum number of subchannels supported by slotInfo
     */
    std::optional<uint16_t> GetMinSubchannels(const SlotInfo& slotInfo,
                                              uint8_t mcs,
                                              uint32_t transportBlockSize,
                                              uint16_t maxSubchannels,
                                              uint8_t mcsTable) const;

    /**
     * \brief Get the number of PSCCH RBs
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The number of PSCCH RBs
     */
    uint16_t GetNumSlPscchRbs(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Get the starting symbol of PSCCH
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The starting symbol of PSCCH
     */
    uint16_t GetPscchSymStart(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Get the length of PSCCH in symbols
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The length of PSCCH in symbols
     */
    uint16_t GetPscchSymLength(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Get the starting symbol of PSSCH
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The starting symbol of PSSCH
     */
    uint16_t GetPsschSymStart(uint8_t bwpId, uint16_t poolId) const;

    /**
     * \brief Get the length of PSSCH in symbols
     *
     * The PSSCH symbol length is determined from RRC configuration for the case
     * in which there is no PSFCH in the slot.  If the parameter "hasPsfch" is set to
     * true, three symbols are deducted from the RRC-based values to account for
     * the PSFCH symbol and its guards.
     *
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \param hasPsfch Whether the slot in consideration includes PSFCH
     * \return The length of PSSCH in symbols
     */
    uint16_t GetPsschSymLength(uint8_t bwpId, uint16_t poolId, bool hasPsfch) const;

    /**
     * \brief Get the value of MaxNumPerReserve (max number of Tx opportunities that can be
     * reserved in a single reservation period, for SPS grants)
     * \param bwpId The bandwidth part id
     * \param poolId The pool id
     * \return The value of MaxNumPerReserve
     */
    uint16_t GetMaxNumPerReserve(uint8_t bwpId, uint16_t poolId) const;

  private:
    /**
     * \brief Get DMRS REs per PRB for PSSCH
     *
     * Returns the number of DMRS resource elements per PRB based on
     * the number of DMRS symbols per slot, per TS 38.214 Table 8.1.3.2-1.
     *
     * Table 8.1.3.2-1 indicates that this is a function of the RRC parameter
     * sl-PSSCH-DMRS-TimePatternList, but this parameter translates to the
     * number of DMRS symbols per slot.  The allowed values are 2 through 4,
     * with 2 being appropriate for low mobility, and 4 for high mobility.
     *
     * \param nDmrsSymSlot Number of DMRS symbols per slot
     * \return Number of DMRS REs per PRB
     */
    uint32_t GetNReDmrs(uint16_t nDmrsSymSlot) const;

    /**
     * \brief Get SCI1 REs for the allocation
     *
     * SCI1 occupies PSCCH symbols * PSCCH PRBs * 12 subcarriers.
     *
     * \param slotInfo Sidelink slot information
     * \return Number of REs occupied by first stage SCI
     */
    uint32_t GetNReSci1(const SlotInfo& slotInfo) const;

    /**
     * \brief Get SCI2 REs for the allocation
     *
     * SCI2 is multiplexed with PSSCH data. The number of SCI2 REs depends on
     * the SCI2 size, beta offset, and data code rate as per TS 38.212
     * Section 8.4.4. The code rate R in the formula is the code rate indicated
     * by the "Modulation and coding scheme" field in SCI format 1-A (i.e., the
     * data channel code rate), not a fixed SCI2-specific rate.
     *
     * \param codeRate Data channel code rate from the MCS table
     * \return Number of REs occupied by second stage SCI
     */
    uint32_t GetNReSci2(double codeRate) const;

    /**
     * \brief Compute TBS from intermediate N_info value
     *
     * Implements TS 38.214 Section 5.1.3.2 TBS determination procedure.
     *
     * \param nInfo Intermediate information bits value (N_RE * Q_m * R * rank)
     * \param codeRate Code rate for the MCS
     * \return TBS in bits
     */
    uint32_t ComputeTbsFromNInfo(double nInfo, double codeRate) const;

    /**
     * \brief Get SlResourcePoolNr
     *
     * \param bwpId The BWP id
     * \param poolId The pool id
     * \return The \link NrSlRrcSap::SlResourcePoolNr \endlink, which holds the
     *         SL pool related configuration.
     */
    const NrSlRrcSap::SlResourcePoolNr GetSlResourcePoolNr(uint8_t bwpId, uint16_t poolId) const;
    /**
     * \brief Validate if the pool id is configured for the given bandwidthpart id.
     * \param bwpId The BWP id
     * \param poolId The pool id
     * \return
     */
    BwpAndPoolIt ValidateBwpAndPoolId(uint8_t bwpId, uint16_t poolId) const;
    /**
     * \brief Is Sidelink resource reservation period multiple of physical
     *        Sidelink pool length.
     * \param bwpId The BWP id
     * \param poolId The pool id
     * \param rsvpInSlots The resource reservation period in slots
     */
    void IsRsvpMultipOfPoolLen(uint8_t bwpId, uint16_t poolId, uint16_t rsvpInSlots) const;

    /**
     * \brief Check if slot indexed by absIndexCurrentSlot is a slot with PSFCH
     *
     * This method checks whether a SL slot is configured for PSFCH, based on
     * the value of slPsfchPeriod configured in the resource pool.
     *
     * This is a lower-level (private) method that is called by the public
     * variant of this method, for cases in which the phyPool and psfchPeriod
     * don't need to be repeatedly fetched.
     *
     * \param absIndexCurrentSlot absolute slot count from simulation time 0
     * \param phyPool PHY pool bitmap indicating the SL slots in a pool
     * \param psfchPerio the slPsfchPeriod value (0, 1, 2, or 4)
     *
     * \return true if the indicated slot has PSFCH, false otherwise
     */
    bool SlotHasPsfch(uint64_t absIndexCurrentSlot,
                      std::vector<std::bitset<1>>& phyPool,
                      uint8_t psfchPeriod) const;

    std::array<NrSlRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL>
        m_slPreconfigFreqInfoList; //!< A list containing per carrier configuration for NR sidelink
                                   //!< communication
    PhySlPoolMap m_phySlPoolMap;   //!< A map to store the physical SL pool per BWP and per SL pool
    SchedulingType m_schType{SchedulingType::UNKNOWN}; //!< Type of the scheduling to be used for
                                                       //!< the pools hold by this class
    std::vector<NrSlUeRrc::LteNrTddSlotType> m_tddPattern; //!< TDD pattern
};

} // namespace ns3

#endif /* NR_SL_COMM_RESOURCE_POOL */
