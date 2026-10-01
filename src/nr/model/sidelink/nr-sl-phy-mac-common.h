// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_PHY_MAC_COMMON_H
#define NR_SL_PHY_MAC_COMMON_H

#include "ns3/nr-phy-mac-common.h"
#include "ns3/sfnsf.h"

#include <limits>
#include <set>
#include <stdint.h>
#include <vector>

namespace ns3
{

/**
 * \ingroup utils
 * \brief Sidelink variant of TransportBlockInfo
 */
struct SlTransportBlockInfo : public TransportBlockInfo
{
    SlTransportBlockInfo(const ExpectedTb& expected)
        : TransportBlockInfo(expected)
    {
    }

    SlTransportBlockInfo() = delete;

    SpectrumValue m_sinrPerceived; //!< SINR that is being update at the end of the DATA reception
                                   //!< and is used for TB decoding
    bool m_sinrUpdated{false};     //!< Flag to indicate the successful update of sinrPerceived

    bool m_isSci2Corrupted{
        false}; //!< True if the ErrorModel indicates that the SCI stage 2 is corrupted.
                //    Filled at the end of data rx/tx
    bool m_isHarqEnabled{false}; //!< Indicate if the SCI2A header had HARQ enabled
    double m_dataTbler;          //!< transport block error rate
    double m_sci2Tbler;          //!< error rate for SCI-2 messages
    uint32_t m_pktIndex{std::numeric_limits<uint32_t>::max()}; //!< Index of the TB in the \p
                                                               //!< m_slRxSigParamInfo buffer
    uint16_t m_dstL2Id{std::numeric_limits<uint16_t>::max()};  //!< Destination L2 ID
};

/**
 * \ingroup utils
 * \brief NrSlInfoListElement_s
 *
 * Named similar to http://www.eurecom.fr/~kaltenbe/fapi-2.0/structDlInfoListElement__s.html
 */
struct NrSlInfoListElement_s
{
    uint32_t srcl2Id{std::numeric_limits<uint32_t>::max()};     //!< Source layer 2 id
    uint32_t dstL2Id{std::numeric_limits<uint32_t>::max()};     //!< Destination Layer 2 id
    uint8_t harqProcessId{std::numeric_limits<uint8_t>::max()}; //!< HARQ process ID

    /// HARQ status enum
    enum HarqStatus_e
    {
        ACK,
        NACK,
        INVALID
    } m_harqStatus{INVALID}; //!< HARQ status
};

/**
 * \ingroup utils
 * \brief SlPscchUeMacStatParameters structure
 */
struct SlPscchUeMacStatParameters
{
    double timeMs{0.0};                                         //!< Time stamp in MilliSeconds
    uint64_t imsi{std::numeric_limits<uint64_t>::max()};        //!< The IMSI of the scheduled UE
    uint16_t rnti{std::numeric_limits<uint16_t>::max()};        //!< The RNTI scheduled
    uint32_t frameNum{std::numeric_limits<uint32_t>::max()};    //!< The frame number
    uint32_t subframeNum{std::numeric_limits<uint32_t>::max()}; //!< The subframe number
    uint16_t slotNum{std::numeric_limits<uint16_t>::max()};     //!< The slot number
    uint16_t symStart{std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used
                                                             //!< for sidelink PSCCH in a slot
    uint16_t symLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols allocated
                                               //!< for sidelink PSCCH
    uint16_t rbStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting resource block
    uint16_t rbLength{std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of
                                                             //!< contiguous resource block
    uint8_t priority{
        std::numeric_limits<uint8_t>::max()}; //!< The priority of a LC. When multiple LCs are
                                              //!< multiplexed it is the priority of the LC with the
                                              //!< highest priority.
    uint8_t mcs{std::numeric_limits<uint8_t>::max()}; //!< The MCS for transport block
    uint16_t tbSize{
        std::numeric_limits<uint16_t>::max()}; //!< The PSSCH transport block size in bytes
    uint16_t slResourceReservePeriod{
        std::numeric_limits<uint16_t>::max()}; //!< The Resource reservation period in milliseconds
    uint16_t totalSubChannels{
        std::numeric_limits<uint16_t>::max()}; //!< The total number of sub-channels given the SL
                                               //!< bandwidth
    uint16_t slPsschSubChStart{
        std::numeric_limits<uint16_t>::max()}; //!< Index of the first subchannel allocated for data
    uint16_t slPsschSubChLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of subchannel
                                               //!< allocated for data
    uint8_t slMaxNumPerReserve{
        std::numeric_limits<uint8_t>::max()}; //!< The maximum number of reserved PSCCH/PSSCH
                                              //!< resources that can be indicated by an SCI
    uint8_t gapReTx1{std::numeric_limits<uint8_t>::max()}; //!< The gap between a transmission and
                                                           //!< its first re-transmission in slots
    uint8_t gapReTx2{std::numeric_limits<uint8_t>::max()}; //!< The gap between a transmission and
                                                           //!< its second re-transmission in slots

    /**
     * \ingroup utils
     *  TracedCallback signature.
     *
     * \param [in] params Value of the SlPscchUeMacStatParameters
     */
    typedef void (*TracedCallback)(const SlPscchUeMacStatParameters params);
};

/**
 * \ingroup utils
 * \brief SlPsschUeMacStatParameters structure
 */
struct SlPsschUeMacStatParameters
{
    double timeMs{0.0};                                         //!< Time stamp in MilliSeconds
    uint64_t imsi{std::numeric_limits<uint64_t>::max()};        //!< The IMSI of the scheduled UE
    uint16_t rnti{std::numeric_limits<uint16_t>::max()};        //!< The RNTI scheduled
    uint32_t frameNum{std::numeric_limits<uint32_t>::max()};    //!< The frame number
    uint32_t subframeNum{std::numeric_limits<uint32_t>::max()}; //!< The subframe number
    uint16_t slotNum{std::numeric_limits<uint16_t>::max()};     //!< The slot number
    uint16_t symStart{std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used
                                                             //!< for sidelink PSSCH in a slot
    uint16_t symLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols allocated
                                               //!< for sidelink PSSCH
    uint16_t subChannelSize{std::numeric_limits<uint16_t>::max()}; //!< The subchannel size in RBs
    uint16_t rbStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting resource block
    uint16_t rbLength{std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of
                                                             //!< contiguous resource block
    uint8_t harqId{std::numeric_limits<uint8_t>::max()}; //!< The NR SL HARQ process id assigned at
                                                         //!< the time of transmitting new data
    uint8_t ndi{
        std::numeric_limits<uint8_t>::max()}; //!< The flag to indicate the new data allocation
    uint8_t rv{std::numeric_limits<uint8_t>::max()};        //!< The redundancy version
    uint32_t srcL2Id{std::numeric_limits<uint32_t>::max()}; //!< The NR Sidelink Source L2 id
    uint32_t dstL2Id{std::numeric_limits<uint32_t>::max()}; //!< The destination Layer 2 Id
    uint8_t csiReq{
        std::numeric_limits<uint8_t>::max()}; //!< The channel state information request flag
    uint8_t castType{std::numeric_limits<uint8_t>::max()}; //!< The cast type
    uint8_t resoReselCounter{
        std::numeric_limits<uint8_t>::max()}; //!< The Sidelink resource re-selection counter for
                                              //!< the semi-persistently scheduled resources as per
                                              //!< TS 38.214
    uint16_t cReselCounter{
        std::numeric_limits<uint8_t>::max()}; //!< The Cresel counter for the semi-persistently
                                              //!< scheduled resources as per TS 38.214

    /**
     * \ingroup utils
     *  TracedCallback signature.
     *
     * \param [in] params Value of the SlPsschUeMacStatParameters
     */
    typedef void (*TracedCallback)(const SlPsschUeMacStatParameters params);
};

/**
 * \ingroup utils
 * \brief The SlRlcPduInfo struct
 *
 * \see SlGrantResource
 */
struct SlRlcPduInfo
{
    /**
     * \brief SlRlcPduInfo constructor
     * \param lcid The Logical channel id
     * \param size The transport block size
     */
    SlRlcPduInfo(uint8_t lcid, uint32_t size)
        : lcid(lcid),
          size(size)
    {
    }

    uint8_t lcid{0};  //!< The Logical channel id
    uint32_t size{0}; //!< The transport block size
};

/**
 * \brief Information about a sidelink candidate single-slot resource
 */
struct SlResourceInfo
{
    /**
     * \brief SlResourceInfo constructor
     *
     * \param numSlPscchRbs Indicates the number of PRBs for PSCCH in a resource pool where it
     * is not greater than the number PRBs of the subchannel. \param slPscchSymStart Indicates
     * the starting symbol used for sidelink PSCCH in a slot \param slPscchSymLength Indicates
     * the total number of symbols available for sidelink PSCCH \param slPsschSymStart Indicates
     * the starting symbol used for sidelink PSSCH in a slot \param slPsschSymLength Indicates
     * the total number of symbols available for sidelink PSSCH \param slSubchannelSize
     * Indicates the subchannel size in number of RBs \param slMaxNumPerReserve Indicates the
     * maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.
     * \param slPsfchPeriod Indicates PSFCH period for the pool
     * \param slMinTimeGapPsfch Indicates MinTimeGapPsfch for the pool
     * \param slMinTimeGapProcessing Indicates MinTimeGapProcessing value of NrUeMac
     * \param sfn The SfnSf
     * \param slSubchannelStart The starting subchannel index
     * \param slSubchannelLength The number of subchannels
     */
    SlResourceInfo(uint16_t numSlPscchRbs,
                   uint16_t slPscchSymStart,
                   uint16_t slPscchSymLength,
                   uint16_t slPsschSymStart,
                   uint16_t slPsschSymLength,
                   uint16_t slSubchannelSize,
                   uint16_t slMaxNumPerReserve,
                   uint8_t slPsfchPeriod,
                   uint8_t slMinTimeGapPsfch,
                   uint8_t slMinTimeGapProcessing,
                   SfnSf sfn,
                   uint8_t slSubchannelStart,
                   uint8_t slSubchannelLength)
    {
        this->numSlPscchRbs = numSlPscchRbs;
        this->slPscchSymStart = slPscchSymStart;
        this->slPscchSymLength = slPscchSymLength;
        this->slPsschSymStart = slPsschSymStart;
        this->slPsschSymLength = slPsschSymLength;
        this->slSubchannelSize = slSubchannelSize;
        this->slMaxNumPerReserve = slMaxNumPerReserve;
        this->slPsfchPeriod = slPsfchPeriod;
        this->slMinTimeGapPsfch = slMinTimeGapPsfch;
        this->slMinTimeGapProcessing = slMinTimeGapProcessing;
        this->sfn = sfn;
        this->slSubchannelStart = slSubchannelStart;
        this->slSubchannelLength = slSubchannelLength;
    }

    // PSCCH
    uint16_t numSlPscchRbs{0}; //!< Indicates the number of PRBs for PSCCH in a resource pool where
                               //!< it is not greater than the number PRBs of the subchannel.
    uint16_t slPscchSymStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used for
                                               //!< sidelink PSCCH in a slot
    uint16_t slPscchSymLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols
                                               //!< available for sidelink PSCCH
    // PSSCH
    uint16_t slPsschSymStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used for
                                               //!< sidelink PSSCH in a slot
    uint16_t slPsschSymLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols
                                               //!< available for sidelink PSSCH
    // subchannel size in RBs
    uint16_t slSubchannelSize{std::numeric_limits<uint16_t>::max()}; //!< Indicates the subchannel
                                                                     //!< size in number of RBs
    uint16_t slMaxNumPerReserve{
        std::numeric_limits<uint16_t>::max()}; //!< The maximum number of reserved PSCCH/PSSCH
                                               //!< resources that can be indicated by an SCI.
    uint8_t slPsfchPeriod{
        std::numeric_limits<uint8_t>::max()}; //!< The PSFCH period configured for this pool
    uint8_t slMinTimeGapPsfch{
        std::numeric_limits<uint8_t>::max()}; //!< The MinTimeGapPsfch configured for this pool
    uint8_t slMinTimeGapProcessing{
        std::numeric_limits<uint8_t>::max()}; //!< The MinTimeGapProcessing of NrUeMac

    SfnSf sfn{}; //!< The SfnSf

    uint8_t slSubchannelStart{
        std::numeric_limits<uint8_t>::max()}; //!< Starting index of subchannel for this resource
    uint8_t slSubchannelLength{
        std::numeric_limits<uint8_t>::max()}; //!< Number of continuous subchannels starting from
                                              //!< the index
    bool slotBusy{
        false}; //!< Whether a reception was detected in another subchannel in the same slot
    /**
     * \brief operator < (less than)
     * \param rhs other SlResourceInfo to compare
     * \return true if this SlResourceInfo is less than rhs
     *
     * The comparison is done on sfnSf
     */
    bool operator<(const SlResourceInfo& rhs) const;
    /**
     * \brief Indicate whether a reception was detected in another subchannel in the same slot
     * \param isBusy whether the candidate should be noted as being adjacent to other reception(s)
     *
     * This parameter is not referenced in TS 38.214 Section 8.1.4 but it provides additional
     * metadata about the resource that may be useful in scheduling, and it is more efficient
     * to gather it as part of sensing rather than to derive it later in the scheduler.
     * In brief, TS 38.214 procedures dictate that a resource be provided as a candidate if
     * the subchannels spanned do not overlap with another sensed resource, but in some cases,
     * the scheduler may wish to avoid scheduling a transmission on a slot in which it could
     * instead receive.  This parameter provides the hint that although the candidate's
     * subchannels are free, there was some other transmission sensed on different subchannel(s).
     */
    void SetSlotBusy(bool isBusy);
    /**
     * \brief Query whether a reception was detected in another subchannel in the same slot
     * \return true if the candidate was noted as being adjacent to other reception(s)
     */
    bool GetSlotBusy() const;
};

/**
 * \ingroup utils
 * \brief A struct used by the NR SL UE MAC scheduler to communicate slot
 *        allocation to UE MAC.
 */
struct SlGrantResource
{
    SfnSf sfn{};                                            //!< The SfnSf
    uint32_t dstL2Id{std::numeric_limits<uint32_t>::max()}; //!< The destination Layer 2 Id

    uint8_t ndi{
        std::numeric_limits<uint8_t>::max()}; //!< The flag to indicate the new data allocation
    uint8_t rv{std::numeric_limits<uint8_t>::max()};       //!< The redundancy version
    uint8_t priority{std::numeric_limits<uint8_t>::max()}; //!< The LC priority
    std::vector<SlRlcPduInfo>
        slRlcPduInfo; //!< The vector containing the transport block size per LC id
    uint16_t mcs{std::numeric_limits<uint16_t>::max()}; //!< The MCS
    // PSCCH
    uint16_t numSlPscchRbs{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the number of PRBs for PSCCH in a
                                               //!< resource pool where it is not greater than the
                                               //!< number PRBs of the subchannel.
    uint16_t slPscchSymStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used for sidelink
                                               //!< PSCCH in a slot
    uint16_t slPscchSymLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols available
                                               //!< for sidelink PSCCH
    // PSSCH
    uint16_t slPsschSymStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol used for sidelink
                                               //!< PSSCH in a slot
    uint16_t slPsschSymLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of symbols allocated
                                               //!< for sidelink PSSCH
    uint16_t slPsschSubChStart{
        std::numeric_limits<uint16_t>::max()}; //!< Index of the first subchannel allocated for data
    uint16_t slPsschSubChLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of subchannel
                                               //!< allocated for data
    bool slHasPsfch{false};                    //!< Indicates whether PSFCH is present in the slot

    uint16_t maxNumPerReserve{
        std::numeric_limits<uint16_t>::max()}; //!< The maximum number of reserved PSCCH/PSSCH
                                               //!< resources that can be indicated by an SCI.
    bool txSci1A{false};                       //!< Flag to indicate if the slots must carry SCI 1-A
    uint8_t slotNumInd{0};                     //!< The number of future TXs an SCI 1-A can indicate

    /**
     * \ingroup utils
     * \brief Less than operator overloaded for SlGrantResource
     * \param rhs other SlGrantResource to compare
     * \return true if this SlGrantResource SfnSf parameter values are less than the rhs
     * SlGrantResource SfnSf parameters
     *
     * The comparison is done on sfnSf
     */
    bool operator<(const SlGrantResource& rhs) const;
};

/**
 * \ingroup utils
 * \brief A struct used by UE MAC to communicate the time and frequency
 *        allocation information of a Var TTI to UE PHY.
 *
 * The UE MAC will pass this struct along with the SFnSF of the slot to
 * communicate a Var TTI allocation information to UE PHY.
 */
struct NrSlVarTtiAllocInfo
{
    uint16_t symStart{std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting symbol
    uint16_t symLength{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of contiguous symbols
    uint16_t rbStart{
        std::numeric_limits<uint16_t>::max()}; //!< Indicates the starting resource block
    uint16_t rbLength{std::numeric_limits<uint16_t>::max()}; //!< Indicates the total number of
                                                             //!< contiguous resource block

    /**
     * \ingroup utils
     * \brief The slVarTtiType enum
     */
    enum
    {
        CTRL,     //!< Used for SL CTRL
        DATA,     //!< Used for SL DATA
        FEEDBACK, //!< Used for SL FEEDBACK
        INVALID   //!< Default value. Used to initialize
    } SlVarTtiType{INVALID};

    /**
     * \ingroup utils
     * \brief Less than operator overloaded for NrSlVarTtiAllocInfo
     * \param rhs other NrSlVarTtiAllocInfo to compare
     * \return true if this NrSlVarTtiAllocInfo symbol start value is
     *         less than the rhs NrSlVarTtiAllocInfo symbol start
     *
     * The comparison is done on the symbol start
     */
    bool operator<(const NrSlVarTtiAllocInfo& rhs) const;
};

/**
 * \ingroup utils
 * \brief A struct used by UE PHY to store the time and frequency
 *        allocation information of each Var TTI communicated by UE MAC.
 *
 * All the Var TTI allocations stored by this struct should belong to
 * the same slot, and each Var TTI in slvarTtiInfoList list must differ
 * in their symbol start.
 */
struct NrSlPhySlotAlloc
{
    SfnSf sfn{};                                    //!< The SfnSf
    std::set<NrSlVarTtiAllocInfo> slvarTtiInfoList; //!< NR Sidelink variable TTI info list
};

/**
 * \ingroup utils
 * \brief The SlDataRxPacketTraceParams struct
 */
struct SlRxDataPacketTraceParams : public RxPacketTraceParams
{
    double m_timeMs{0.0};                                    //!< Time stamp in MilliSeconds
    uint16_t m_txRnti{std::numeric_limits<uint16_t>::max()}; //!< The TX UE RNTI
    uint8_t m_ndi{
        std::numeric_limits<uint8_t>::max()}; //!< The flag to indicate the new data allocation
    double m_tblerSci2;                       //!< TBLER of SCI stage 2
    bool m_sci2Corrupted{
        false};    //!< The flag to indicate the success/failure of SCI stage 2 decodification
    int m_rbStart; //!< The start RB index
    int m_rbEnd;   //!< The end RB index
    uint32_t m_dstL2Id{0}; //!< The destination layer 2 id
    uint16_t m_srcL2Id{0}; //!< The source layer 2 id

    /**
     * \ingroup utils
     *  TracedCallback signature.
     *
     * \param [in] params Value of the SlRxDataPacketTraceParams
     */
    typedef void (*TracedCallback)(const SlRxDataPacketTraceParams params);
};

/**
 * \ingroup utils
 * \brief The SlCtrlRxPacketTraceParams struct
 */
struct SlRxCtrlPacketTraceParams : public RxPacketTraceParams
{
    double m_timeMs{0.0};                                    //!< Time stamp in MilliSeconds
    uint16_t m_txRnti{std::numeric_limits<uint16_t>::max()}; //!< The TX UE RNTI
    int m_rbStart;                                           //!< The start RB index
    int m_rbEnd;                                             //!< The end RB index
    uint8_t m_priority{std::numeric_limits<uint8_t>::max()}; //!< The priority
    uint16_t m_slResourceReservePeriod{
        std::numeric_limits<uint16_t>::max()}; //!< The resource reservation period
    uint16_t m_totalSubChannels{0};            //!< The total number of subchannel
    uint8_t m_indexStartSubChannel{
        std::numeric_limits<uint8_t>::max()}; //!< The starting index of the subchannel
    uint8_t m_lengthSubChannel{
        std::numeric_limits<uint8_t>::max()}; //!< The total number of subchannel assigned
    uint8_t m_maxNumPerReserve{std::numeric_limits<uint8_t>::max()}; //!< The MaxNumPerReserved
    uint32_t m_dstL2Id{0};                                           //!< The destination layer 2 id

    /**
     * \ingroup utils
     *  TracedCallback signature.
     *
     * \param [in] params Value of the SlRxCtrlPacketTraceParams
     */
    typedef void (*TracedCallback)(const SlRxCtrlPacketTraceParams params);
};

/**
 * \ingroup utils
 * \brief The SensingData struct
 *
 * Sensing information pertaining to the reception of an SCI-1A header
 * for mode 2 sensing purposes.
 */
struct SensingData
{
    /**
     * \brief SensingData constructor
     * \param sfn The SfnSf
     * \param rsvp The resource reservation period in ms
     * \param sbChLength The total number of the sub-channel allocated
     * \param sbChStart The index of the starting sub-channel allocated
     * \param prio The priority
     * \param slRsrp The measured RSRP value over the used resource blocks
     * \param gapReTx1 Gap for a first retransmission in absolute slots
     * \param sbChStartReTx1 The index of the starting sub-channel allocated to first retransmission
     * \param gapReTx2 Gap for a second retransmission in absolute slots
     * \param sbChStartReTx2 The index of the starting sub-channel allocated to second
     * retransmission
     */
    SensingData(SfnSf sfn,
                uint16_t rsvp,
                uint8_t sbChLength,
                uint8_t sbChStart,
                uint8_t prio,
                double slRsrp,
                uint8_t gapReTx1,
                uint8_t sbChStartReTx1,
                uint8_t gapReTx2,
                uint8_t sbChStartReTx2)
    {
        this->sfn = sfn;
        this->rsvp = rsvp;
        this->sbChLength = sbChLength;
        this->sbChStart = sbChStart;
        this->prio = prio;
        this->slRsrp = slRsrp;
        this->gapReTx1 = gapReTx1;
        this->sbChStartReTx1 = sbChStartReTx1;
        this->gapReTx2 = gapReTx2;
        this->sbChStartReTx2 = sbChStartReTx2;
    }

    SfnSf sfn{};      //!< The SfnSf
    uint16_t rsvp{0}; //!< The resource reservation period in ms
    uint8_t sbChLength{
        std::numeric_limits<uint8_t>::max()}; //!< The total number of the sub-channel allocated
    uint8_t sbChStart{
        std::numeric_limits<uint8_t>::max()}; //!< The index of the starting sub-channel allocated
    uint8_t prio{std::numeric_limits<uint8_t>::max()}; //!< The priority
    double slRsrp{0.0}; //!< The measured RSRP value over the used resource blocks
    uint8_t gapReTx1{
        std::numeric_limits<uint8_t>::max()}; //!< Gap for a first retransmission in absolute slots
    uint8_t sbChStartReTx1{
        std::numeric_limits<uint8_t>::max()}; //!< The index of the starting sub-channel allocated
                                              //!< to first retransmission
    uint8_t gapReTx2{
        std::numeric_limits<uint8_t>::max()}; //!< Gap for a second retransmission in absolute slots
    uint8_t sbChStartReTx2{
        std::numeric_limits<uint8_t>::max()}; //!< The index of the starting sub-channel allocated
                                              //!< to second retransmission
};

/**
 * \ingroup utils
 * \brief Structure to denote a future resource reserved by another UE
 *
 * This data structure represents resources excluded by step 6c) of the
 * TS 38.214 Sec. 8.1.4 sensing algorithm.
 */
struct ReservedResource
{
    /**
     * \brief ReservedResource constructor
     *
     * \param sfn The SfnSf
     * \param rsvp The resource reservation period in ms
     * \param sbChLength The total number of the sub-channel allocated
     * \param sbChStart The index of the starting sub-channel allocated
     * \param prio The priority
     * \param slRsrp The measured RSRP value over the used resource blocks
     */
    ReservedResource(SfnSf sfn,
                     uint16_t rsvp,
                     uint8_t sbChLength,
                     uint8_t sbChStart,
                     uint8_t prio,
                     double slRsrp)
    {
        this->sfn = sfn;
        this->rsvp = rsvp;
        this->sbChLength = sbChLength;
        this->sbChStart = sbChStart;
        this->prio = prio;
        this->slRsrp = slRsrp;
    }

    SfnSf sfn{};      //!< The SfnSf
    uint16_t rsvp{0}; //!< The resource reservation period in ms
    uint8_t sbChLength{
        std::numeric_limits<uint8_t>::max()}; //!< The total number of the sub-channel allocated
    uint8_t sbChStart{
        std::numeric_limits<uint8_t>::max()}; //!< The index of the starting sub-channel allocated
    uint8_t prio{std::numeric_limits<uint8_t>::max()}; //!< The priority
    double slRsrp{0.0}; //!< The measured RSRP value over the used resource blocks
};

/**
 * \ingroup utils
 * \brief A struct that contains info for the SL HARQ
 */
struct SlHarqInfo : public HarqInfo
{
    uint16_t m_txRnti{55};     //!< Transmitter RNTI
    uint16_t m_dstL2Id{65535}; //!< DST L2 ID

    /**
     * \brief Status of the SL Harq: ACKed or NACKed
     */
    enum HarqStatus
    {
        ACK,
        NACK
    } m_harqStatus{NACK}; //!< HARQ status

    virtual bool IsReceivedOk() const override
    {
        return m_harqStatus == ACK;
    }
};

/**
 * \brief Stream output operator for SensingData
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const SensingData& p);

/**
 * \brief Stream output operator for ReservedResource
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const ReservedResource& p);

/**
 * \brief Stream output operator for SlGrantResource
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const SlGrantResource& p);

/**
 * \brief Stream output operator for SlHarqInfo
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const SlHarqInfo& item);
} // namespace ns3

#endif /* NR_SL_PHY_MAC_COMMON_H_ */
