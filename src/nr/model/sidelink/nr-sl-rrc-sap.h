/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 */

#ifndef NR_SL_RRC_SAP_H
#define NR_SL_RRC_SAP_H

#include "ns3/nr-rrc-sap.h"

#include <array>
#include <bitset>

namespace ns3
{

class NrSlDataRadioBearerInfo;
class NrSlSignallingRadioBearerInfo;
class NrSlDiscoveryRadioBearerInfo;

class NrSlRrcSap
{
  public:
    virtual ~NrSlRrcSap() = default;

    // NR Sidelink IE TS 38.331

    /// Sidelink Constraint values (from 6.4 RRC multiplicity and type constraint values 3GPP
    /// TS 38.331)
// Since NR code emulate multiple carriers using BWP, we fixed the size of
// this list to 1 instead of 16.
#define MAX_NUM_OF_FREQ_SL 1 //!< Maximum number of carrier frequency for NR sidelink communication
#define MAX_SCSs                                                                                   \
    5 //!< Maximum number of subcarrier spacing specifications. Equal to the number of numerlogies
      //!< supported (guessed) //TODO
#define MAX_NUM_OF_RX_POOL 16 //!< Maximum number of Rx resource poolfor NR sidelink communication
#define MAX_NUM_OF_TX_POOL 8  //!< Maximum number of Tx resource poolfor NR sidelink communication
#define MAX_NUM_OF_POOL_ID 16 //!< Maximum index of resource pool for NR sidelink communication
#define MAX_NUM_OF_SL_BWPs 4  //!< Maximum number of BWP for for NR sidelink communication

    /**
     * \brief Struct for SubcarrierSpacing enumeration
     */
    struct SubcarrierSpacing
    {
        /// SubcarrierSpacing enumeration
        enum
        {
            kHZ15,
            kHZ30,
            kHZ60,
            kHZ120,
            kHZ240,
            INVALID
        } spacing{INVALID}; //!< subcarrier spacing
    };

    /**
     * \brief Struct for sl-LengthSymbols enumeration
     */
    struct SlLengthSymbols
    {
        /// SlLengthSymbols enumeration
        enum
        {
            SYM7,
            SYM8,
            SYM9,
            SYM10,
            SYM11,
            SYM12,
            SYM13,
            SYM14,
            INVALID
        } symbols{INVALID}; //!< Number of symbols for Sidelink
    };

    /**
     * \brief Struct for sl-StartSymbol enumeration
     */
    struct SlStartSymbol
    {
        /// SlStartSymbol enumeration
        enum
        {
            SYM0,
            SYM1,
            SYM2,
            SYM3,
            SYM4,
            SYM5,
            SYM6,
            SYM7,
            INVALID
        } symbol{INVALID}; //!< Starting symbol index for Sidelink
    };

    /**
     * \brief Struct for CyclicPrefix enumeration
     */
    struct CyclicPrefix
    {
        /// CyclicPrefix enumeration
        enum
        {
            NORMAL,
            EXTENDED,
            INVALID
        } cp{INVALID}; //!< cyclic prefix
    };

    /**
     * \brief Struct for sl-TimeResourcePSCCH enumeration
     */
    struct SlTimeResourcePscch
    {
        /// SlTimeResourcePscch enumeration
        enum
        {
            N1, //!< 1 symbol //Added for modeling purpose
            N2, //!< 2 symbols
            N3, //!< 3 symbols
            INVALID
        } resources{INVALID}; //!< Number of symbols for PSCCH
    };

    /**
     * \brief Struct for sl-FreqResourcePSCCH enumeration
     */
    struct SlFreqResourcePscch
    {
        /// SlFreqResourcePscch enumeration
        enum
        {
            N10, //!< 10 PRBS
            N12,
            N15,
            N20,
            N25,
            INVALID
        } resources{INVALID}; //!< Number of RBs for PSCCH
    };

    /**
     * \brief Struct for sl-Scaling-r16 enumeration
     */
    struct SlScaling
    {
        /// SlScaling enumeration
        enum
        {
            F0P5,
            F0P65,
            F0P8,
            F1,
            INVALID
        } scaling{INVALID}; //!< scaling
    };

    /**
     * \brief Struct for sl-SubchannelSize enumeration
     */
    struct SlSubchannelSize
    {
        /// SlSubchannelSize enumeration
        enum
        {
            N10,
            N15,
            N20,
            N25,
            N50,
            N75,
            N100,
            INVALID
        } numPrbs{INVALID}; //!< Sidelink subchannel size in PRBs
    };

    /**
     * \brief Struct for sl-MCS-Table enumeration
     */
    struct SlMcsTable
    {
        /// SlMcsTable enumeration
        enum
        {
            QAM64,
            QAM256,
            // QAM64LOWSE, //TODO not supported by NR module
            INVALID
        } mcsTable{INVALID}; //!< MCS table to be used for Sidelink
    };

    /**
     * \brief Struct for sl-SensingWindow enumeration
     */
    struct SlSensingWindow
    {
        /// SlSensingWindow enumeration
        enum
        {
            MS100,
            MS1100,
            INVALID
        } windSize{INVALID}; //!< Sidelink sensing window length in milliseconds
    };

    /**
     * \brief Struct for sl-SelectionWindow
     */
    struct SlSelectionWindow
    {
        /// SlSelectionWindow enumeration
        enum
        {
            N1,
            N5,
            N10,
            N20,
            INVALID
        } windSize{INVALID}; //!< Sidelink selection window length in slots
    };

    /**
     * \brief Struct for SL-ResourceReservePeriod enumeration
     *
     * At the time of implementing this IE, 38.331 standard
     * seems confused. However, 38.214 (8.1.4) says the following:
     * The resource reservation interval, if provided, is converted
     * from units of ms to units of logical slots. So, the following
     * is ms
     */
    struct SlResourceReservePeriod
    {
        /// SlResourceReservePeriod enumeration
        enum
        {
            MS0, //!< Milliseconds
            MS10,
            MS20,
            MS30,
            MS40,
            MS50,
            MS60,
            MS70,
            MS80,
            MS90,
            MS100,
            MS150,
            MS200,
            MS250,
            MS300,
            MS350,
            MS400,
            MS450,
            MS500,
            MS550,
            MS600,
            MS650,
            MS700,
            MS750,
            MS800,
            MS850,
            MS900,
            MS950,
            MS1000,
            INVALID
        } period{INVALID}; //!< Sidelink resource reservation period in milliseconds.
    };

    /**
     * \brief struct for sl-MaxNumPerReserve-r16 enumeration
     */
    struct SlMaxNumPerReserve
    {
        /// SlMaxNumPerReserve enumeration
        enum
        {
            N1, // Added for modeling
            N2,
            N3,
            INVALID
        } maxNumPerRes{INVALID}; //!< Sidelink MaxNumPerReserve in slots
    };

    /**
     * \brief struct for slPSFCH-Period-r16 enumeration
     */
    struct SlPsfchPeriod
    {
        /// SlPsfchPeriod enumeration
        enum
        {
            SL0, // PSFCH disabled
            SL1,
            SL2,
            SL4
        } period{SL0}; //!< PSFCH period in slots
    };

    /**
     * \brief struct for sl-MinTimeGapPSFCH-r16 enumeration
     */
    struct SlMinTimeGapPsfch
    {
        /// SlMinTimeGapPsfch enumeration
        enum
        {
            SL2,
            SL3,
            INVALID
        } gap{INVALID};
    };

    /**
     * \brief SL PSFCH-Config field within SL ResourcePool information element
     */
    struct SlPsfchConfig
    {
        SlPsfchPeriod slPsfchPeriod;         //!< Parameter for indicating the PSFCH period in slots
        SlMinTimeGapPsfch slMinTimeGapPsfch; //!< Parameter for indicating the minimum time gap
                                             //!< between PSSCH and PSFCH (slots)
                                             // Other members will be added as needed
    };

    /**
     * \brief Get subcarrier spacing value
     *
     * This method converts the enum of type NrSlRrcSap::SubcarrierSpacing
     * to its unsigned integer representation.
     *
     * \param scs The object of type NrSlRrcSap::SubcarrierSpacing
     * \returns The unsigned integer representation of subcarrier spacing value in Hz
     */
    static uint32_t GetScSpacingValue(const NrSlRrcSap::SubcarrierSpacing& scs);

    /**
     * \brief Get Subcarrier spacing enum
     *
     * This method converts the subcarrier spacing unsigned value to
     * an enum of type NrSlRrcSap::SubcarrierSpacing
     *
     * \param scs The subcarrier spacing in Hz
     * \returns The object of type NrSlRrcSap::SubcarrierSpacing
     */
    static NrSlRrcSap::SubcarrierSpacing GetScSpacingEnum(uint32_t scs);

    /**
     * \brief Get sidelink length symbols value
     *
     * This method converts the enum of type NrSlRrcSap::SlLengthSymbols
     * to its unsigned integer representation.
     *
     * \param slLengthSyms The object of type NrSlRrcSap::SlLengthSymbols
     * \returns The unsigned integer representation of NrSlRrcSap::SlLengthSymbols
     */
    static uint16_t GetSlLengthSymbolsValue(const NrSlRrcSap::SlLengthSymbols& slLengthSyms);

    /**
     * \brief Get sidelink length symbols enum
     *
     * This method converts the sidelink length in symbols to
     * an enum of type NrSlRrcSap::SlLengthSymbols.
     *
     * \param slLengthSyms The total number of symbols allocated to SL in a slot
     * \returns The object of type NrSlRrcSap::SlLengthSymbols
     */
    static NrSlRrcSap::SlLengthSymbols GetSlLengthSymbolsEnum(uint16_t slLengthSyms);

    /**
     * \brief Get sidelink start symbol value
     *
     * This method converts the enum of type NrSlRrcSap::SlStartSymbol
     * to its unsigned integer representation.
     *
     * \param slStartSym The object of type NrSlRrcSap::SlStartSymbol
     * \returns The unsigned integer representation of NrSlRrcSap::SlStartSymbol
     */
    static uint16_t GetSlStartSymbolValue(const NrSlRrcSap::SlStartSymbol& slStartSym);

    /**
     * \brief Get sidelink start symbol enum
     *
     * This method converts the sidelink start symbol to
     * an enum of type NrSlRrcSap::SlStartSymbol.
     *
     * \param slStartSym The starting symbol of SL allocation in a slot
     * \returns The object of type NrSlRrcSap::SlStartSymbol
     */
    static NrSlRrcSap::SlStartSymbol GetSlStartSymbolEnum(uint16_t slStartSym);

    /**
     * \brief Get sidelink time resource PSCCH value
     *
     * This method converts the enum of type NrSlRrcSap::SlTimeResourcePscch
     * to its unsigned integer representation.
     *
     * \param slTResoPscch The object of type NrSlRrcSap::SlTimeResourcePscch
     * \returns The unsigned integer representation of NrSlRrcSap::SlTimeResourcePscch
     */
    static uint16_t GetSlTResoPscchValue(const NrSlRrcSap::SlTimeResourcePscch& slTResoPscch);

    /**
     * \brief Get sidelink time resource PSCCH enum
     *
     * This method converts the sidelink time resources for PSCCH length (in
     * symbols) to an enum of type NrSlRrcSap::SlTimeResourcePscch.
     *
     * \param slTResoPscch The number of symbols assigned for PSCCH transmission in a slot
     * \returns The object of type NrSlRrcSap::SlTimeResourcePscch
     */
    static NrSlRrcSap::SlTimeResourcePscch GetSlTResoPscchEnum(uint16_t slTResoPscch);

    /**
     * \brief Get sidelink frequency resource PSCCH value
     *
     * This method converts the enum of type NrSlRrcSap::SlFreqResourcePscch
     * to its unsigned integer representation.
     *
     * \param slFResoPscch The object of type NrSlRrcSap::SlFreqResourcePscch
     * \returns The unsigned integer representation of NrSlRrcSap::SlFreqResourcePscch
     */
    static uint16_t GetSlFResoPscchValue(const NrSlRrcSap::SlFreqResourcePscch& slFResoPscch);

    /**
     * \brief Get sidelink frequency resource PSCCH enum
     *
     * This method converts the sidelink frequency resources for PSCCH length (in
     * PRBs) to an enum of type NrSlRrcSap::SlFreqResourcePscch.
     *
     * \param slFResoPscch The number of symbols assigned for PSCCH transmission in a slot
     * \returns The object of type NrSlRrcSap::SlFreqResourcePscch
     */
    static NrSlRrcSap::SlFreqResourcePscch GetSlFResoPscchEnum(uint16_t slFResoPscch);

    /**
     * \brief Get sidelink scaling value
     *
     * This method converts the enum of type NrSlRrcSap::SlScaling
     * to its float representation.
     *
     * \param slScaling The object of type NrSlRrcSap::SlScaling
     * \returns The float representation of NrSlRrcSap::SlScaling
     */
    static float GetSlScalingValue(const NrSlRrcSap::SlScaling& slScaling);

    /**
     * \brief Get sidelink scaling enum.
     *
     * This method converts the sidelink scaling value to an enum of type
     * NrSlRrcSap::SlScaling.
     *
     *  <b>Only the first digit after decimal is considered</b>
     *
     *
     * \param slScaling The sidelink scaling factor
     * \returns The object of type NrSlRrcSap::SlScaling
     */
    static NrSlRrcSap::SlScaling GetSlScalingEnum(float slScaling);

    /**
     * \brief Get sidelink subchannel size value
     *
     * This method converts the enum of type NrSlRrcSap::SlSubchannelSize
     * to its unsigned integer representation.
     *
     * \param subChSize The object of type NrSlRrcSap::SlSubchannelSize
     * \returns The unsigned integer representation of NrSlRrcSap::SlSubchannelSize
     */
    static uint16_t GetNrSlSubChSizeValue(const NrSlRrcSap::SlSubchannelSize& subChSize);

    /**
     * \brief Get sidelink subchannel size enum.
     *
     * This method converts the sidelink subchannel size value to an enum of type
     * NrSlRrcSap::SlSubchannelSize.
     *
     * \param subChSize The sidelink subchannel size in PRBs
     * \returns The object of type NrSlRrcSap::SlSubchannelSize
     */
    static NrSlRrcSap::SlSubchannelSize GetNrSlSubChSizeEnum(uint16_t subChSize);

    /**
     * \brief Get sidelink sensing window value
     *
     * This method converts the enum of type NrSlRrcSap::SlSensingWindow
     * to its unsigned integer representation, such that, 100 is 100 ms,
     * 1100 is 1100 ms, and so on.
     *
     * \param windowSize The object of type NrSlRrcSap::SlSensingWindow
     * \returns The unsigned integer representation of NrSlRrcSap::SlSensingWindow
     */
    static uint16_t GetSlSensWindowValue(const NrSlRrcSap::SlSensingWindow& windowSize);

    /**
     * \brief Get sidelink sensing window enum.
     *
     * This method converts the sidelink sensing window size value to an enum of type
     * NrSlRrcSap::SlSensingWindow.
     *
     * \param windowSize The sidelink sensing window size in millisecond
     * \returns The object of type NrSlRrcSap::SlSensingWindow
     */
    static NrSlRrcSap::SlSensingWindow GetSlSensWindowEnum(uint16_t windowSize);

    /**
     * \brief Get sidelink selection window value
     *
     * This method converts the enum of type NrSlRrcSap::SlSelectionWindow
     * to its unsigned integer representation.
     *
     * \param windowSize The object of type NrSlRrcSap::SlSelectionWindow
     * \returns The unsigned integer representation of NrSlRrcSap::SlSelectionWindow
     */
    static uint16_t GetSlSelWindowValue(const NrSlRrcSap::SlSelectionWindow& windowSize);

    /**
     * \brief Get sidelink selection window enum.
     *
     * This method converts the sidelink selection window size value to an enum
     * of type NrSlRrcSap::SlSelectionWindow.
     *
     * \param windowSize The sidelink selection window size in milliseconds
     * \returns The object of type NrSlRrcSap::SlSelectionWindow
     */
    static NrSlRrcSap::SlSelectionWindow GetSlSelWindowEnum(uint16_t windowSize);

    /**
     * \brief Get sidelink resource reserve period value
     *
     * This method converts the enum of type NrSlRrcSap::SlResourceReservePeriod
     * to its unsigned integer representation, such that, 0 is 0 seconds,
     * 100 is 100 seconds, and so on.
     *
     * \param period The object of type NrSlRrcSap::SlResourceReservePeriod
     * \returns The unsigned integer representation of NrSlRrcSap::SlResourceReservePeriod
     */
    static uint16_t GetSlResoResvPrdValue(const NrSlRrcSap::SlResourceReservePeriod& period);

    /**
     * \brief Get sidelink resource reserve period enum.
     *
     * This method converts the sidelink resource reserve period value to an enum
     * of type NrSlRrcSap::SlResourceReservePeriod.
     *
     * \param period The sidelink resource reserve period in seconds
     * \returns The object of type NrSlRrcSap::SlResourceReservePeriod
     */
    static NrSlRrcSap::SlResourceReservePeriod GetSlResoResvPrdEnum(uint16_t period);

    /**
     * \brief Get sidelink maximum number per reserve value
     *
     * This method converts the enum of type NrSlRrcSap::SlMaxNumPerReserve
     * to its unsigned integer representation, such the, N2 is 2 slots,
     * N3 is 3 slots, and so on. This value is a maximum number of reserved
     * PSCCH/PSSCH resources that can be indicated by an SCI.
     *
     * \param slMaxReserve The object of type NrSlRrcSap::SlMaxNumPerReserve
     * \returns The unsigned integer representation of NrSlRrcSap::SlMaxNumPerReserve
     */
    static uint8_t GetSlMaxNumPerReserveValue(const NrSlRrcSap::SlMaxNumPerReserve& slMaxReserve);

    /**
     * \brief Get sidelink PSFCH period value
     *
     * This method converts the enum of type NrSlRrcSap::SlPsfchPeriod
     * to its unsigned integer representation, such as SL0 is 0 slots,
     * SL1 is 1 slot, and so on. The value of zero indicates that PSFCH is
     * never sent, and HARQ feedback is disabled.
     *
     * \param slPsfchPeriod The enum of type NrSlRrcSap::SlPsfchPeriod
     * \returns The unsigned integer representation of NrSlRrcSap::SlPsfchPeriod
     */
    static uint8_t GetSlPsfchPeriodValue(const NrSlRrcSap::SlPsfchPeriod& slPsfchPeriod);

    /**
     * \brief Get sidelink PSFCH period enum
     *
     * This method converts the sidelink PSFCH period value to an enum
     * of type NrSlRrcSap::SlPsfchPeriod.
     *
     * \param slPsfchPeriodInt The sidelink PSFCH period in slots
     * \returns The object of type NrSlRrcSap::SlPsfchPeriod
     */
    static NrSlRrcSap::SlPsfchPeriod GetSlPsfchPeriodEnum(uint8_t slPsfchPeriodInt);

    /**
     * \brief Get sidelink PSFCH minimum time gap value
     *
     * This method converts the enum of type NrSlRrcSap::SlMinTimeGapPsfch
     * to its unsigned integer representation, either SL2 (2 slots) or
     * SL3 (3 slots).  This value is the minimum time gap in slots between
     * the received PSSCH and the associated PSFCH.
     *
     * \param slMinTimeGapPsfch The enum of type NrSlRrcSap::SlMinTimeGapPsfch
     * \returns The unsigned integer representation of NrSlRrcSap::SlMinTimeGapPsfch
     */
    static uint8_t GetSlMinTimeGapPsfchValue(
        const NrSlRrcSap::SlMinTimeGapPsfch& slMinTimeGapPsfch);

    /**
     * \brief Get sidelink MinTimeGapPSFCH enum
     *
     * This method converts the sidelink minimum time gap for PSFCH value to an
     * of type NrSlRrcSap::SlMinTimeGapPsfch.
     *
     * \param slMinTimeGapInt The sidelink MinTimeGapPSFCH in slots
     * \returns The object of type NrSlRrcSap::SlMinTimeGapPsfch
     */
    static NrSlRrcSap::SlMinTimeGapPsfch GetSlMinTimeGapPsfchEnum(uint8_t slMinTimeGapInt);

    /**
     * \brief Get sidelink maximum number per reserve enum
     *
     * This method converts the sidelink maximum number per reserve value to
     * an enum of type NrSlRrcSap::SlMaxNumPerReserve.
     *
     * \param slMaxReserveInt The sidelink maximum number per reserve value in number of slots
     * \returns The object of type NrSlRrcSap::SlMaxNumPerReserve
     */
    static NrSlRrcSap::SlMaxNumPerReserve GetSlMaxNumPerReserveEnum(uint8_t slMaxReserveInt);

    /**
     * \brief SCS-SpecificCarrier information element
     */
    struct ScsSpecificCarrier
    {
        // uint16_t offsetToCarrier {3000}; //!< Offset in frequency domain between Point A and the
        // lowest usable subcarrier on this carrier in number of PRBs (0..275*8-1).
        SubcarrierSpacing subcarrierSpacing; //!< Subcarrier spacing
        uint16_t carrierBandwidth{
            3000}; //!< carrier bandwidth in number of PRBs. Valid range is [1, 275]
    };

    /**
     * \brief ARFCN-ValueNR information element
     */
    struct ArfcnValueNR
    {
        uint32_t arfcn{4000000}; //!< ARFCN valid range is [0, 3279165] 38.331 sec 6.4
    };

    /**
     * \brief BWP information element
     */
    struct Bwp
    {
        // uint32_t locationAndBandwidth {3000}; //!< Resource Indicator value (RIV). 38.214
        // sec 5.1.2.2.2 SubcarrierSpacing subcarrierSpacing; //!< Subcarrier spacing CyclicPrefix
        // cyclicPrefix; //!< Cyclic prefix  //optional filed Following parameters are not standard
        // compliant. We use them to configure UE in out-of-coverage scenarios
        uint16_t numerology{99};     //!< The numerology
        uint16_t symbolsPerSlots{0}; //!< Total number of symbols per slot
        uint32_t rbPerRbg{0};        //!< Resource block per resource block group
        uint16_t bandwidth{0};       //!< Bandwidth
    };

    /**
     * \brief Sl-Bwp-Generic information element
     */
    struct SlBwpGeneric
    {
        Bwp bwp;                         //!< BWP information element
        SlLengthSymbols slLengthSymbols; //!< This field indicates the number of symbols used for
                                         //!< sidelink in a slot without SL-SSB
        SlStartSymbol slStartSymbol; //!< This field indicates the starting symbol used for sidelink
                                     //!< in a slot without SL-SSB
        // slFilterCoefficient; //!< Filter coefficient used for L3 filtering [not supported]
    };

    /**
     * \brief SL-PSCCH-Config information element
     */
    struct SlPscchConfig
    {
        /// setuprelease enumeration
        enum
        {
            RELEASE,
            SETUP,
            INVALID
        } setupRelease{INVALID}; //!< Indicates if it is allocating or releasing resources

        SlTimeResourcePscch
            slTimeResourcePscch; //!< Indicates the number of symbols of PSCCH in a resource pool.
        SlFreqResourcePscch slFreqResourcePscch; //!< Indicates the number of PRBs for PSCCH in a
                                                 //!< resource pool where it is not greater than the
                                                 //!< number PRBs of the subchannel.
        // uint32_t slDmrsScreambleId {70000}; //TODO //!< Indicates the initialization value for
        // PSCCH DMRS scrambling. Valid range [0, 65535] uint16_t slNumReservedBits {0}; //TODO //!<
        // Indicates the number of reserved bits in first stage SCI. Valid range [2, 4]
    };

    /**
     * \brief SL-BetaOffset information element
     */
    struct SlBetaOffsets
    {
        uint16_t offset{32}; //!< Beta-offset.
    };

    /**
     * \brief SL-PSSCH-Config information element
     */
    struct SlPsschConfig
    {
        // sl-PSSCH-DMRS-TimePattern-r16 //!< Indicates the set of PSSCH DMRS time domain patterns
        // that can be used in the resource pool.
        std::array<SlBetaOffsets, 4>
            slBetaOffsets2ndSci; //!< Indicates candidates of beta-offset values to determine the
                                 //!< number of coded modulation symbols for second stage SCI.
        SlScaling slScaling;     //!< Indicates a scaling factor to limit the number of resource
                                 //!< elements assigned to the second stage SCI on PSSCH
    };

    /**
     * \brief SL-UE-SelectedConfigRP information element
     */
    struct SlUeSelectedConfigRp
    {
        SlSensingWindow
            slSensingWindow; //!< Parameter that indicates the start of the sensing window.
        SlSelectionWindow slSelectionWindow; //!< Parameter that determines the end of the selection
                                             //!< window in the resource selection for a TB with
                                             //!< respect to priority indicated in SCI.
        bool slMultiReserveResource{
            false}; //!< Flag to enable the reservation of an initial transmission of a TB by an SCI
                    //!< associated with a different TB.
        std::list<SlResourceReservePeriod>
            slResourceReservePeriodList; //!< Set of possible resource reservation period allowed in
                                         //!< the resource pool.
        SlMaxNumPerReserve
            slMaxNumPerReserve; //!< Indicates the maximum number of reserved PSCCH/PSSCH resources
                                //!< that can be indicated by an SCI.

        // sl-CBR-Priority-TxConfigList-r16 //TODO
        // We probably going to simplify the RSRP threshold
        // by introducing an attribute directly in UE PHY class.
        // sl-ThresPSSCH-RSRP-List-r16 //TODO //!< Indicates a threshold used for sensing based UE
        // autonomous resource selection.

        // since we do not support DMRS signals
        // we will rely on the PSCCH RSRP and SCI message
        // decodefication.
        // sl-RS-ForSensing-r16 //TODO //!< Flag to enable the use of DMRS of PSCCH or PSSCH for L1
        // RSRP measurement in the sensing operation.
    };

    /**
     * \brief SL-ResourcePool information element
     */
    struct SlResourcePoolNr
    {
        SlPscchConfig slPscchConfig;                //!< SL-PSCCH field
        SlPsfchConfig slPsfchConfig;                //!< SL-PSFCH field
        SlSubchannelSize slSubchannelSize;          //!< Sidelink subchannel size in PRBs
        SlUeSelectedConfigRp slUeSelectedConfigRp;  //!< SL-UE-SelectedConfigRP
        std::vector<std::bitset<1>> slTimeResource; //!< Sidelink bitmap
        // uint16_t slStartRbSubchannel {3000}; //!< First RB of a sidelink subchannel. Valid range
        // [0, 265] uint16_t slNumSubchannel {3000}; //!< Number of subchannels. Valid range [1, 27]
        // SlMcsTable slMcsTable; //It is configurable via error model attribute //!< Indicates the
        // MCS table used for the resource pool. SlPsschConfig slPsschHConfig; //!< SL-PSSCH field
        // sl-PSFCH-Config-r16 //TODO
        // sl-SyncAllowed-r16 //TODO
        // sl-Period-r16 [FFS]
        // sl-ThreshS-RSSI-CBR-r16 //TODO
        // sl-TimeWindowSizeCBR-r16 //TODO
        // sl-TimeWindowSizeCR-r16 //TODO
        // SL-PTRS-Config //TODO
        // SL-ConfiguredGrantConfigList //TODO
        // sl-RxParametersNcell-r16 //TODO
    };

    /**
     * \brief SL-ResourcePoolID information element
     */
    struct SlResourcePoolIdNr
    {
        uint16_t id{333}; //!< Sidelink pool id. Valid range [1, 16] by standard. In the simulator
                          //!< it is from [0, 15]
    };

    /**
     * \brief SL-ResourcePoolConfig information element
     */
    struct SlResourcePoolConfigNr
    {
        bool haveSlResourcePoolConfigNr{
            false}; //!< true if the fields in this IE are set, false otherwise
        SlResourcePoolIdNr slResourcePoolId; //!< Sidelink pool id
        SlResourcePoolNr slResourcePool;     //!< Sidelink resource pool
    };

    /**
     * \brief SL-BWP-PoolConfigCommon information element
     */
    struct SlBwpPoolConfigCommonNr
    {
        // std::array <SlResourcePoolNr, MAX_NUM_OF_RX_POOL> slRxPool; //!< List of sidelink
        // resource pools for RX
        std::array<SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL>
            slTxPoolSelectedNormal; //!< List of sidelink resource pools for TX
        // sl-TxPoolExceptional-r16 //TODO //!< Indicates the resources by which the UE is allowed
        // to transmit NR sidelink communication in exceptional conditions on the configured BWP.
    };

    /**
     * \brief SL-BWP-ConfigCommon information element
     */
    struct SlBwpConfigCommonNr
    {
        bool haveSlBwpGeneric{false}; //!< true if slBwpGeneric is set, false otherwise.
        SlBwpGeneric slBwpGeneric;    //!< This field indicates the generic parameters on the
                                      //!< configured sidelink BWP
        bool haveSlBwpPoolConfigCommonNr{
            false}; //!< true if slBwpPoolConfigCommonNr is set, false otherwise.
        SlBwpPoolConfigCommonNr
            slBwpPoolConfigCommonNr; //!< This field indicates the resource pool configurations on
                                     //!< the configured sidelink BWP
    };

    /**
     * \brief SL-FreqConfigCommon information element
     */
    struct SlFreqConfigCommonNr
    {
        // std::array <ScsSpecificCarrier, MAX_SCSs> slScsSpecificCarrierList; //!< A list per
        // numerology for UE specific channel bandwidth and location configurations
        std::array<SlBwpConfigCommonNr, MAX_NUM_OF_SL_BWPs>
            slBwpList; //!< List of sidelink BWP(s) for NR sidelink communication
        // ArfcnValueNR slAbsoluteFrequencyPointA; //!< Absolute frequency of the reference resource
        // block (Common RB 0). ArfcnValueNR slAbsoluteFrequencySSB; //!< Frequency location of
        // sidelink Synchronization Signal/PBCH block (SSB) bool frequencyShift7p5khzSL {false};
        // //!< Enable the NR SL transmission with a 7.5 kHz shift to the LTE raster [we don't use
        // this] int valueN {2}; //!< Indicate the NR SL transmission with a valueN * 5kHz shift to
        // the LTE raster. Valid range : [-1,1] sl-SyncPriority-r16 //TODO //!< This field indicates
        // synchronization priority order, as specified in sub-clause TS 38.331 5.X.6 bool
        // sl-NbAsSync-r16 //TODO //!< If True, an out of coverage / mode 2 enabled UE is allowed to
        // use an available network as sync source. Only present in SL-PreconfigurationNR
        // sl-SyncConfigList //TODO //!< List of configurations for Tx/RX synch information for NR
        // SL communication. sl-PowerControl //TODO //!< Sidelink power control related
        // configuration
    };

    /**
     * \brief TDD-UL-DL-ConfigCommon
     *
     * This IE is not implemented as per the 3GPP standard
     * since in the simulator we support our own customized
     * TDD pattern.
     *
     */
    struct TddUlDlConfigCommon
    {
        std::string tddPattern{"F|F|F|F|F|F|F|F|F|F|"}; //!< TDD pattern

        // SubcarrierSpacing referenceSubcarrierSpacing //TODO
        // TDD-UL-DL-Pattern pattern1 //TODO
    };

    /**
     * \brief SL-PreconfigGeneral information element
     */
    struct SlPreconfigGeneralNr
    {
        TddUlDlConfigCommon slTddConfig; //!< TDD pattern
                                         // BIT STRING (SIZE (2)) reservedBits-r16 //TODO
    };

    /**
     * \brief SL-PSSCH-TxParameters information element
     */
    struct SlPsschTxParameters
    {
        uint8_t slMaxTxTransNumPssch; /**< Indicates the maximum transmission number
                                           (including new transmission and
                                           retransmission) for PSSCH.
                                           */
                                      // sl-MinMCS-PSSCH-r16  INTEGER (0..27); /TODO
        // sl-MaxMCS-PSSCH-r16  INTEGER (0..31); /TODO
        // sl-MinSubChannelNumPSSCH-r16 INTEGER (1..27); /TODO
        // sl-MaxSubchannelNumPSSCH-r16 INTEGER (1..27); /TODO
        // sl-MaxTxPower-r16 SL-TxPower-r16; /TODO
    };

    /**
     * \brief SL-PSSCH-TxConfigList information element
     */
    struct SlPsschTxConfigList
    {
        std::array<SlPsschTxParameters, 1> slPsschTxParameters; //!< List of SlPsschTxParameters
    };

    /**
     * \brief SL-UE-SelectedConfig-r16
     */
    struct SlUeSelectedConfig
    {
        double slProbResourceKeep{0.0}; /**< Indicates the probability with which
                                            the UE keeps the current resource when
                                            the resource reselection counter reaches
                                            zero for sensing based UE autonomous
                                            resource selection (see TS 38.321).
                                            Standard values for this parameter are
                                            0, 0.2, 0.4, 0.6, 0.8, however, in the
                                            simulator we are not restricting it to
                                            evaluate other values.
                                            */

        SlPsschTxConfigList slPsschTxConfigList; //!< Indicates PSSCH TX parameters

        // slPrioritizationThres; //TODO
        // SlReselAfter; //TODO
        // UlPrioritizationThres; //TODO
    };

    /**
     * \brief SL-PreconfigNR information element
     */
    struct SidelinkPreconfigNr
    {
        std::array<SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL>
            slPreconfigFreqInfoList; //!< List containing per carrier configuration for NR sidelink
                                     //!< communication
        SlPreconfigGeneralNr slPreconfigGeneral;  //!< SlPreconfigGeneralNr
        SlUeSelectedConfig slUeSelectedPreConfig; //!< SlUeSelectedConfig
                                                  /*
                                                  sl-PreconfigNR-AnchorCarrierFreqList-r16
                                                  sl-PreconfigEUTRA-AnchorCarrierFreqList-r16
                                                  sl-RadioBearerPreConfigList-r16
                                                  sl-RLC-BearerPreConfigList-r16
                                                  sl-MeasPreConfig-r16
                                                  sl-OffsetDFN-r16
                                                  t400-r16
                                                  sl-SSB-PriorityNR-r16
                                                  sl-UE-SelectedPreConfig-r16
                                                  */
    };

    /**
     * \brief SL-PreconfigurationNR information element
     *
     * Even though the NR 38.331 specify the following
     * structure, we do not see the use of it. We
     * should use directly the SidelinkPreconfigNr
     * structure.
     */
    // struct SlPreconfigurationNr
    // {
    //   SidelinkPreconfigNr sidelinkPreconfigNr;
    // };

    /**
     * \brief SlReselectionConfig
     */
    struct SlReselectionConfig
    {
        double slRsrpThres; //!< Indicates the threshold (dBm) of SL communication/ discovery RSRP
                            //!< for a U2N remote UE to perform relay UE selection/ reselection.
        double slFilterCoefficientRsrp; //!< Specifies L3 filter coefficient for SL communication/
                                        //!< discovery RSRP measurement results from L1 filter.
        uint8_t slHystMin; //!< Specifies the minimum hysteresis value for a relay to be eligible.
    };

    // SL-RelayUe-Config IE structure is defined but the logic to determine eligibility to serve as
    // a relay is not implemented
    /**
     * \brief SL-RelayUe-Config Information Element
     * The IE SL-RelayUE-Config specifies the configuration information for NR sidelink U2N Relay
     * UE.
     */
    struct SlRelayUeConfig
    {
        double thresHighRelay; //!< Indicates the upper threshold of Uu RSRP for a UE that is in
                               //!< network coverage to evaluate AS layer conditions for U2N relay
                               //!< UE operation.
        double
            thresLowRelay; //!< Indicates the lower threshold of Uu RSRP for a UE that is in network
                           //!< coverage to evaluate AS layer conditions for U2N relay UE operation
        uint8_t hystMaxRelay;
        uint8_t hystMinRelay;
    };

    // SL-RemoteUe-Config IE structure is defined but is only used to define relay (re)selection
    // criteria
    /**
     * \brief SL-RemoteUe-Config Information Element
     * The IE SL-RemoteUE-Config specifies the configuration information for NR sidelink U2N Remote
     * UE.
     */
    struct SlRemoteUeConfig
    {
        double thresHighRemote; //!< Indicates the threshold of Uu RSRP for a UE that is in network
                                //!< coverage to evaluate AS layer conditions for U2N remote UE
                                //!< operation.
        uint8_t hystMaxRemote;  //!<  Indicates the hysteresis defining how far below the threshold
                                //!<  the input is required to be
        SlReselectionConfig slReselectionConfig; //!< Includes the parameters used by the U2N remote
                                                 //!< UE when selecting/ reselecting a U2N relay UE.
    };

    /**
     * \brief SL-DiscConfigCommon Information Element
     */
    struct SlDiscConfigCommon
    {
        SlRelayUeConfig slRelayUeConfigCommon;
        SlRemoteUeConfig slRemoteUeConfigCommon;
    };
};

/**
 * \ingroup nr
 *
 * \brief User part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the gc and the NrSlUeRrc. In
 * particular, this class implements the User part of the
 * SAP, i.e., the sidelink related methods exported by the
 * NrSlUeRrc and called by the gc.
 */
class NrSlUeRrcSapUser : public NrSlRrcSap
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeRrcSapUser();
    /**
     * \brief Get NR sidelink preconfiguration
     *
     * \return The sidelink preconfiguration
     */
    virtual const NrSlRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration() = 0;
    /**
     * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
     *
     * \param slBitMap The sidelink bitmap
     * \return A vector representing the physical sidelink pool
     */
    virtual const std::vector<std::bitset<1>> GetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap) = 0;
    /**
     * \brief Get Bwp Id Container
     *
     * \return The container of SL BWP ids
     */
    virtual const std::set<uint8_t> GetBwpIdContainer() = 0;
    /**
     * \brief Add NR sidelink data radio bearer
     *
     * Attempts to add a sidelink radio bearer
     *
     * \param slTxDrb NrSlDataRadioBearerInfo pointer
     */
    virtual void AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) = 0;
    /**
     * \brief Add NR Reception sidelink data radio bearer
     *
     * Attempts to add a sidelink radio bearer for RX
     *
     * \param slRxDrb NrSlDataRadioBearerInfo pointer
     */
    virtual void AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) = 0;
    /**
     * \brief Get NR Sidelink data radio bearer
     *
     * \param dstL2Id The remote/destination layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo
     */
    virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkTxDataRadioBearer(uint32_t dstL2Id,
                                                                      uint8_t lcId) = 0;
    /**
     * \brief Get all NR Sidelink Tx data radio bearers to a destination L2 Id
     *
     * \param dstL2Id The remote/destination layer 2 id
     * \return A map containing all of the NrSlDataRadioBearerInfo, by logical channel ID
     */
    virtual std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
    GetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id) = 0;
    /**
     * \brief Get NR Sidelink Rx data radio bearer
     *
     * \param srcL2Id The source layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo
     */
    virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkRxDataRadioBearer(uint32_t srcL2Id,
                                                                      uint8_t lcId) = 0;
    /**
     * \brief Get all NR Sidelink Rx data radio bearers from a source L2 ID
     *
     * \param srcL2Id The source layer 2 id
     * \return A map containing all of the NrSlDataRadioBearerInfo, by logical channel ID
     */
    virtual std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
    GetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id) = 0;

    /**
     * \brief Remove NR SL data bearer transmission
     *
     * \param slTxdDb NrSlDataRadioBearerInfo pointer
     */
    virtual void RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) = 0;

    /**
     * \brief Remove NR SL data bearer for reception
     *
     * \param slRxDrb NrSlDataRadioBearerInfo pointer
     */
    virtual void RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) = 0;
    /**
     * \brief Get next LCID for setting up NR SL DRB towards the given destination
     *
     * As per, table 6.2.4-1 of 38.321 LCID for SL-SCH range from 4-19, i.e.,
     * total 16 LCIDs
     *
     * \param dstL2Id The destination layer 2 ID
     * \return the next available NR SL DRB LCID
     */
    virtual uint8_t GetNextLcid(uint32_t dstL2Id) = 0;
    /**
     * \brief Add NR sidelink signalling radio bearer for transmission
     *
     * \param slSrb NrSlSignallingRadioBearerInfo pointer
     */
    virtual void AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) = 0;
    /**
     * \brief Add NR sidelink signalling radio bearer for reception
     *
     * \param slSrb NrSlSignallingRadioBearerInfo pointer
     */
    virtual void AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) = 0;
    /**
     * \brief Get NR Sidelink signalling radio bearer for transmission
     *
     * \param dstL2Id The peer layer 2 id
     * \param lcId the logical channel id
     * \return The NrSlSignallingRadioBearerInfo
     */
    virtual Ptr<NrSlSignallingRadioBearerInfo> GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                              uint8_t lcId) = 0;
    /**
     * \brief Get the UE layer 2 ID
     *
     * \return The layer 2 Id
     */
    virtual uint32_t GetSourceL2Id() = 0;
    /**
     * \brief Add NR sidelink discovery radio bearer for transmission
     *
     * \param slDiscRb NrSlDiscoveryRadioBearerInfo pointer
     */
    virtual void AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) = 0;
    /**
     * \brief Add NR sidelink discovery radio bearer for reception
     *
     * \param slDiscRb NrSlDiscoveryRadioBearerInfo pointer
     */
    virtual void AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) = 0;
    /**
     * \brief Get NR Sidelink discovery radio bearer for transmission
     *
     * \param dstL2Id The peer layer 2 id
     * \return The NrSlDiscoveryRadioBearerInfo
     */
    virtual Ptr<NrSlDiscoveryRadioBearerInfo> GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the gc and the NrSlUeRrc. In
 * particular, this class implements the Provider part of the
 * SAP, i.e., the sidelink related methods exported by the
 * gc and called by the NrSlUeRrc.
 *
 */
class NrSlUeRrcSapProvider : public NrSlRrcSap
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeRrcSapProvider();
    /**
     * \brief Populate NR Sidelink pools
     *
     * After getting the pre-configuration
     * NrSlUeRrc instruct the gc to
     * populate the pools.
     *
     */
    virtual void PopulatePools() = 0;
    /**
     * \brief Set Sidelink source layer 2 id
     *
     * \param srcL2Id The Sidelink layer 2 id of the source
     */
    virtual void SetSourceL2Id(uint32_t srcL2Id) = 0;
    /**
     * \brief Set NR Sidelink discovery and (re)selection requiremenmts for the relay UE
     *
     * \param relayConfig the relay UE config
     */
    virtual void SetRelayRequirements(const NrSlRrcSap::SlRelayUeConfig config) = 0;

    /**
     * \brief Set NR Sidelink discovery and (re)selection requiremenmts for the remote UE
     *
     * \param relayConfig the remote UE config
     */
    virtual void SetRemoteRequirements(const NrSlRrcSap::SlRemoteUeConfig config) = 0;
};

////////////////////////////////////
//   templates UE RRC side
////////////////////////////////////

/**
 * Template for the implementation of the NrSlUeRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapUser : public NrSlUeRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeRrcSapUser(C* owner);

    // inherited from NRSlUeRrcSapUser
    const NrSlRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration() override;
    const std::vector<std::bitset<1>> GetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap) override;
    const std::set<uint8_t> GetBwpIdContainer() override;
    void AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) override;
    void AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) override;
    Ptr<NrSlDataRadioBearerInfo> GetSidelinkTxDataRadioBearer(uint32_t dstL2Id,
                                                              uint8_t lcid) override;
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkTxDataRadioBearers(
        uint32_t dstL2Id) override;

    Ptr<NrSlDataRadioBearerInfo> GetSidelinkRxDataRadioBearer(uint32_t srcL2Id,
                                                              uint8_t lcid) override;
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkRxDataRadioBearers(
        uint32_t srcL2Id) override;
    uint8_t GetNextLcid(uint32_t dstL2Id) override;
    void RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxTDrb) override;
    void RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) override;
    void AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) override;
    void AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) override;
    Ptr<NrSlSignallingRadioBearerInfo> GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                      uint8_t lcId) override;
    uint32_t GetSourceL2Id() override;
    void AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) override;
    void AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) override;
    Ptr<NrSlDiscoveryRadioBearerInfo> GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id) override;

  private:
    MemberNrSlUeRrcSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser()
{
}

template <class C>
const NrSlRrcSap::SidelinkPreconfigNr
MemberNrSlUeRrcSapUser<C>::GetNrSlPreconfiguration()
{
    return m_owner->DoGetNrSlPreconfiguration();
}

template <class C>
const std::vector<std::bitset<1>>
MemberNrSlUeRrcSapUser<C>::GetPhysicalSlPool(const std::vector<std::bitset<1>>& slBitMap)
{
    return m_owner->DoGetPhysicalSlPool(slBitMap);
}

template <class C>
const std::set<uint8_t>
MemberNrSlUeRrcSapUser<C>::GetBwpIdContainer()
{
    return m_owner->DoGetBwpIdContainer();
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slDrb)
{
    m_owner->DoAddNrSlTxDataRadioBearer(slDrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    m_owner->DoAddNrSlRxDataRadioBearer(slRxDrb);
}

template <class C>
Ptr<NrSlDataRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetSidelinkTxDataRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    return m_owner->DoGetSidelinkTxDataRadioBearer(dstL2Id, lcId);
}

template <class C>
std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
MemberNrSlUeRrcSapUser<C>::GetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id)
{
    return m_owner->DoGetAllSidelinkTxDataRadioBearers(dstL2Id);
}

template <class C>
Ptr<NrSlDataRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetSidelinkRxDataRadioBearer(uint32_t srcL2Id, uint8_t lcId)
{
    return m_owner->DoGetSidelinkRxDataRadioBearer(srcL2Id, lcId);
}

template <class C>
std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
MemberNrSlUeRrcSapUser<C>::GetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id)
{
    return m_owner->DoGetAllSidelinkRxDataRadioBearers(srcL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    m_owner->DoRemoveNrSlTxDataRadioBearer(slTxDrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    m_owner->DoRemoveNrSlRxDataRadioBearer(slRxDrb);
}

template <class C>
uint8_t
MemberNrSlUeRrcSapUser<C>::GetNextLcid(uint32_t dstL2Id)
{
    return m_owner->DoGetNextLcid(dstL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    m_owner->DoAddTxNrSlSignallingRadioBearer(slSrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    m_owner->DoAddRxNrSlSignallingRadioBearer(slSrb);
}

template <class C>
Ptr<NrSlSignallingRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    return m_owner->DoGetTxNrSlSignallingRadioBearer(dstL2Id, lcId);
}

template <class C>
uint32_t
MemberNrSlUeRrcSapUser<C>::GetSourceL2Id()
{
    return m_owner->DoGetSourceL2Id();
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddTxNrSlDiscoveryRadioBearer(
    Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb)
{
    m_owner->DoAddTxNrSlDiscoveryRadioBearer(slTxDiscRb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddRxNrSlDiscoveryRadioBearer(
    Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb)
{
    m_owner->DoAddRxNrSlDiscoveryRadioBearer(slRxDiscRb);
}

template <class C>
Ptr<NrSlDiscoveryRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    return m_owner->DoGetTxNrSlDiscoveryRadioBearer(dstL2Id);
}

/**
 * Template for the implementation of the NrSlUeRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapProvider : public NrSlUeRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeRrcSapProvider(C* owner);

    // inherited from NRSlUeRrcSapProvider
    void PopulatePools() override;
    void SetSourceL2Id(uint32_t srcL2Id) override;
    void SetRelayRequirements(const NrSlRrcSap::SlRelayUeConfig config) override;
    void SetRemoteRequirements(const NrSlRrcSap::SlRemoteUeConfig config) override;

  private:
    MemberNrSlUeRrcSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider()
{
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::PopulatePools()
{
    m_owner->DoPopulatePools();
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetSourceL2Id(uint32_t srsL2Id)
{
    m_owner->DoSetSourceL2Id(srsL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetRelayRequirements(const NrSlRrcSap::SlRelayUeConfig config)
{
    m_owner->DoSetRelayRequirements(config);
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetRemoteRequirements(const NrSlRrcSap::SlRemoteUeConfig config)
{
    m_owner->DoSetRemoteRequirements(config);
}

// eNB side SAPs

/**
 * \brief User part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlEnbRrcSapUser();
};

/**
 * \brief Provider part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlEnbRrcSapProvider();
};

////////////////////////////////////
//   templates eNB RRC side
////////////////////////////////////

/**
 * Template for the implementation of the NrSlEnbRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapUser : public NrSlEnbRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlEnbRrcSapUser(C* owner);

    // inherited from NRSlUeRrcSapUser
    //  virtual void Setup (SetupParameters params);

  private:
    MemberNrSlEnbRrcSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapUser<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/

/**
 * Template for the implementation of the NrSlEnbRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapProvider : public NrSlEnbRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlEnbRrcSapProvider(C* owner);

    // inherited from NRSlUeRrcSapUser
    //  virtual void Setup (SetupParameters params);

  private:
    MemberNrSlEnbRrcSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapProvider<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/

} // namespace ns3

#endif /* NR_SL_RRC_SAP_H */
