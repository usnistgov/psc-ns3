// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_MODEL_NR_PHY_MAC_COMMON_H
#define SRC_NR_MODEL_NR_PHY_MAC_COMMON_H

#include "nr-error-model.h"
#include "sfnsf.h"

#include <ns3/log.h>
#include <ns3/matrix-array.h>
#include <ns3/object.h>
#include <ns3/string.h>

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace ns3
{

struct GetFirst
{
    template <class First, class Second>
    First& operator()(std::pair<First, Second>& p)
    {
        return p.first;
    }

    template <class First, class Second>
    First& operator()(typename std::map<First, Second>::iterator& p)
    {
        return p.first;
    }

    template <class First, class Second>
    const First& operator()(const std::pair<First, Second>& p)
    {
        return p.first;
    }

    template <class First, class Second>
    const First& operator()(const typename std::map<First, Second>::iterator& p)
    {
        return p.first;
    }
};

struct GetSecond
{
    template <class First, class Second>
    Second& operator()(std::pair<First, Second>& p)
    {
        return p.second;
    }

    template <class First, class Second>
    Second& operator()(typename std::map<First, Second>::iterator& p)
    {
        return p.second;
    }

    template <class First, class Second>
    Second& operator()(typename std::unordered_map<First, Second>::iterator& p)
    {
        return p.second;
    }

    template <class First, class Second>
    const Second& operator()(const std::pair<First, Second>& p)
    {
        return p.second;
    }

    template <class First, class Second>
    const Second& operator()(const typename std::map<First, Second>::iterator& p)
    {
        return p.second;
    }

    template <class First, class Second>
    const Second& operator()(const typename std::unordered_map<First, Second>::iterator& p)
    {
        return p.second;
    }
};

/**
 * \brief Flags for the CSI feedback configuration
 */
enum CsiFeedbackFlag : uint8_t
{
    CQI_PDSCH_MIMO = 0x01, //!< Initialize CSI feedback using MIMO PDSCH
    CQI_CSI_RS = 0x02,     //!< Initialize CSI feedback to use CSI-RS
    CQI_CSI_IM = 0x04,     //!< Initialize CSI feedback to use CSI-IM if PDSCH not available
    CQI_PDSCH_SISO = 0x08  //!< Initialize CSI feedback using SISO PDSCH
};

/**
 * \ingroup utils
 * \brief Scheduling information. Despite the name, it is not TDMA.
 */
struct DciInfoElementTdma
{
    /**
     * \brief Format of the DCI
     */
    enum DciFormat
    {
        DL = 0, //!< DL DCI
        UL = 1  //!< UL DCI
    };

    /**
     * \brief The VarTtiType enum
     */
    enum VarTtiType
    {
        SRS = 0,  //!< Used for SRS (it would be like DCI format 2_3)
        DATA = 1, //!< Used for DL/UL DATA
        CTRL = 2, //!< Used for DL/UL CTRL
        MSG3 = 3, //!< Used for UL MSG3
    };

    /**
     * \brief Constructor used in NrUePhy to build local DCI for DL and UL control
     * \param symStart Sym start
     * \param numSym Num sym
     * \param rbgBitmask Bitmask of RBG
     */
    DciInfoElementTdma(uint8_t symStart,
                       uint8_t numSym,
                       DciFormat format,
                       VarTtiType type,
                       const std::vector<bool>& rbgBitmask)
        : m_format(format),
          m_symStart(symStart),
          m_numSym(numSym),
          m_type(type),
          m_rbgBitmask(rbgBitmask)
    {
    }

    /**
     * \brief Construct to build brand new DCI. Please remember to update manually
     * the HARQ process ID and the RBG bitmask
     *
     * \param rnti RNTI of the UE
     * \param format DCI format
     * \param symStart starting symbol index for flexible TTI scheme
     * \param numSym number of symbols for flexible TTI scheme
     * \param mcs MCS
     * \param rank the Rank number
     * \param precMats the precoding matrix
     * \param tbs TB size
     * \param ndi New Data Indicator
     * \param rv Redundancy Version
     */
    DciInfoElementTdma(uint16_t rnti,
                       DciFormat format,
                       uint8_t symStart,
                       uint8_t numSym,
                       uint8_t mcs,
                       uint8_t rank,
                       Ptr<const ComplexMatrixArray> precMats,
                       uint32_t tbs,
                       uint8_t ndi,
                       uint8_t rv,
                       VarTtiType type,
                       uint8_t bwpIndex,
                       uint8_t tpc)
        : m_rnti(rnti),
          m_format(format),
          m_symStart(symStart),
          m_numSym(numSym),
          m_mcs(mcs),
          m_rank(rank),
          m_precMats(precMats),
          m_tbSize(tbs),
          m_ndi(ndi),
          m_rv(rv),
          m_type(type),
          m_bwpIndex(bwpIndex),
          m_tpc(tpc)
    {
    }

    /**
     * \brief Copy constructor except for some values that have to be overwritten
     * \param symStart Sym start
     * \param numSym Num sym
     * \param ndi New Data Indicator: 0 for Retx, 1 for New Data
     * \param rv Retransmission value
     * \param o Other object from which copy all that is not specified as parameter
     */
    DciInfoElementTdma(uint8_t symStart,
                       uint8_t numSym,
                       uint8_t ndi,
                       uint8_t rv,
                       const DciInfoElementTdma& o)
        : m_rnti(o.m_rnti),
          m_format(o.m_format),
          m_symStart(symStart),
          m_numSym(numSym),
          m_mcs(o.m_mcs),
          m_rank(o.m_rank),
          m_precMats(o.m_precMats),
          m_tbSize(o.m_tbSize),
          m_ndi(ndi),
          m_rv(rv),
          m_type(o.m_type),
          m_bwpIndex(o.m_bwpIndex),
          m_harqProcess(o.m_harqProcess),
          m_rbgBitmask(o.m_rbgBitmask),
          m_tpc(o.m_tpc)
    {
    }

    const uint16_t m_rnti{0};     //!< RNTI of the UE
    const DciFormat m_format{DL}; //!< DCI format
    const uint8_t m_symStart{0};  //!< starting symbol index for flexible TTI scheme
    const uint8_t m_numSym{0};    //!< number of symbols for flexible TTI scheme
    const uint8_t m_mcs{0};       //!< MCS
    const uint8_t m_rank{1};      //!< the rank number (the number of MIMO layers)
    Ptr<const ComplexMatrixArray> m_precMats{nullptr};
    const uint32_t m_tbSize{0};       //!< TB size
    const uint8_t m_ndi{0};           //!< New Data Indicator
    const uint8_t m_rv{0};            //!< Redundancy Version
    const VarTtiType m_type{SRS};     //!< Var TTI type
    const uint8_t m_bwpIndex{0};      //!< BWP Index to identify to which BWP this DCI applies to.
    uint8_t m_harqProcess{0};         //!< HARQ process id
    std::vector<bool> m_rbgBitmask{}; //!< RBG mask: 0 if the RBG is not used, 1 otherwise
    const uint8_t m_tpc{0};           //!< Tx power control command
};

/**
 * \ingroup utils
 * \brief The RlcPduInfo struct
 */
struct RlcPduInfo
{
    RlcPduInfo() = default;
    RlcPduInfo(const RlcPduInfo& o) = default;
    ~RlcPduInfo() = default;

    RlcPduInfo(uint8_t lcid, uint32_t size)
        : m_lcid(lcid),
          m_size(size)
    {
    }

    uint8_t m_lcid{0};
    uint32_t m_size{0};
};

struct VarTtiAllocInfo
{
    VarTtiAllocInfo(const VarTtiAllocInfo& o) = default;

    VarTtiAllocInfo(const std::shared_ptr<DciInfoElementTdma>& dci)
        : m_dci(dci)
    {
    }

    bool m_isOmni{false};
    std::shared_ptr<DciInfoElementTdma> m_dci;
    std::vector<RlcPduInfo> m_rlcPduInfo;

    bool operator<(const VarTtiAllocInfo& o) const
    {
        NS_ASSERT(m_dci != nullptr);
        return (m_dci->m_symStart < o.m_dci->m_symStart);
    }
};

struct NrBuildRarListElement_s
{
    std::shared_ptr<DciInfoElementTdma>
        ulMsg3Dci;             //!< UL MSG3 DCI that will be sent through RAR message
    uint8_t raPreambleId{255}; //!< RA preamble ID, initialize with out of range values
    uint32_t k2Delay{100}; //!< delay (in slots) between DL/UL DCI reception and subframe to which
                           //!< it applies for reception/transmission of Data (k0/k2)
};

/**
 * \ingroup utils
 * \brief The SlotAllocInfo struct
 */
struct SlotAllocInfo
{
    SlotAllocInfo(SfnSf sfn)
        : m_sfnSf(sfn)
    {
    }

    /**
     * \brief Enum which indicates the allocations that are inside the allocation info
     */
    enum AllocationType
    {
        NONE = 0, //!< No allocations
        DL = 1,   //!< DL Allocations
        UL = 2,   //!< UL Allocations
        BOTH = 3  //!< DL and UL allocations
    };

    /**
     * \brief Merge the input parameter to this SlotAllocInfo
     * \param other SlotAllocInfo to merge in this allocation
     *
     * After the merge, order the allocation by symStart in DCI
     */
    void Merge(const SlotAllocInfo& other);

    /**
     * \brief Check if we have data allocations
     * \return true if m_varTtiAllocInfo contains data allocations
     */
    bool ContainsDataAllocation() const;

    /**
     * \brief Check if we have UL MSG3 allocations
     * \return true if m_varTtiAllocInfo contains data allocations
     */
    bool ContainsUlMsg3Allocation() const;

    /**
     * \return true if m_varTtiAllocInfo contains a DL ctrl allocation
     */
    bool ContainsDlCtrlAllocation() const;

    /**
     * \return true if m_varTtiAllocInfo contains a scheduled UL ctrl allocation (e.g., SRS)
     */
    bool ContainsUlCtrlAllocation() const;

    SfnSf m_sfnSf{};                                     //!< SfnSf of this allocation
    uint32_t m_numSymAlloc{0};                           //!< Number of allocated symbols
    std::deque<VarTtiAllocInfo> m_varTtiAllocInfo;       //!< queue of allocations
    AllocationType m_type{NONE};                         //!< Allocations type
    std::vector<NrBuildRarListElement_s> m_buildRarList; //!< build rar list that will be sent to UE
    /**
     * \brief operator < (less than)
     * \param rhs other SlotAllocInfo to compare
     * \return true if this SlotAllocInfo is less than rhs
     *
     * The comparison is done on sfnSf
     */
    bool operator<(const SlotAllocInfo& rhs) const;
};

/**
 * \ingroup utils
 * \brief The DlCqiInfo struct
 */
struct DlCqiInfo
{
    uint16_t m_rnti{0}; //!< The RNTI
    // TODO: Rename to m_rank (m_ri is set directly to the rank).
    uint8_t m_ri{0}; //!< the rank indicator, or simply the rank number

    // TODO: use NrMacSchedulerUeInfo::CqiInfo
    enum DlCqiType
    {
        WB,
        SB
    } m_cqiType{WB}; //!< The type of the CQI

    uint8_t m_wbCqi{0}; //!< Wideband CQI
    size_t m_wbPmi{0};  //!< Wideband precoding matrix index

    std::vector<uint8_t> m_sbCqis; //!< Subband CQI values
    std::vector<size_t> m_sbPmis;  //!< Subband PMI values (i2, indices of W2 matrices)
    uint8_t m_mcs{0};              //!< MCS (can be derived from CQI feedback)
    Ptr<const ComplexMatrixArray> m_optPrecMat{}; ///< Precoding matrix for each RB
};

/**
 * \brief The structure used for the CQI feedback message that contains the optimum CQI, RI, PMI,
 and full precoding matrix (dimensions: nGnbPorts * rank * nRbs).
 */
struct PmCqiInfo
{
    uint8_t m_mcs{0};              //!< Modulation and coding scheme supported by current channel
    uint8_t m_rank{0};             //!< Rank of the channel matrix (supported number of MIMO layers)
    size_t m_wbPmi{0};             //!< Wideband precoding matrix index
    uint8_t m_wbCqi{0};            //!< Wideband CQI
    std::vector<uint8_t> m_sbCqis; //!< Subband CQI values
    std::vector<size_t> m_sbPmis;  //!< Subband PMI values (i2, indices of W2 matrices)
    Ptr<const ComplexMatrixArray> m_optPrecMat{}; ///< Precoding matrix for each RB

    // TODO: Fix/remove empty DlSBCQIReported, then change default type to SB.

    DlCqiInfo::DlCqiType m_cqiType{DlCqiInfo::WB}; ///< CQI type (WB or SB)
    size_t m_tbSize{}; //!< Expected TB size when allocating all resources
};

/**
 * \ingroup utils
 * \brief The UlCqiInfo struct
 */
struct UlCqiInfo
{
    // std::vector <uint16_t> m_sinr;
    std::vector<double> m_sinr;

    enum UlCqiType
    {
        SRS,
        PUSCH,
        PUCCH_1,
        PUCCH_2,
        PRACH
    } m_type;
};

/**
 * \ingroup utils
 * \brief The MacCeValue struct
 */
struct MacCeValue
{
    MacCeValue()
        : m_phr(0),
          m_crnti(0)
    {
    }

    uint8_t m_phr;
    uint8_t m_crnti;
    std::vector<uint8_t> m_bufferStatus;
};

/**
 * \ingroup utils
 * \brief See section 4.3.14 macCEListElement
 */
struct MacCeElement
{
    MacCeElement()
        : m_rnti(0)
    {
    }

    uint16_t m_rnti;

    enum MacCeType
    {
        BSR,
        PHR,
        CRNTI
    } m_macCeType;
    struct MacCeValue m_macCeValue;
};

/**
 * \ingroup utils
 * \brief The RlcListElement struct
 */
struct RlcListElement
{
    std::vector<struct RlcPduInfo> m_rlcPduElements;
};

/**
 * \ingroup utils
 * \brief The UePhyPacketCountParameter struct
 */
struct UePhyPacketCountParameter
{
    uint64_t m_imsi;
    uint32_t m_noBytes;
    bool m_isTx; // Set to false if Rx and true if tx
    uint32_t m_subframeno;
};

/**
 * \ingroup utils
 * \brief The GnbPhyPacketCountParameter struct
 */
struct GnbPhyPacketCountParameter
{
    uint64_t m_cellId;
    uint32_t m_noBytes;
    bool m_isTx; // Set to false if Rx and true if tx
    uint32_t m_subframeno;
};

/**
 * \ingroup utils
 * \brief Information about the expected transport block at a certain point in the slot
 *
 * Information passed by the PHY through a call to AddExpectedTb
 */
struct ExpectedTb
{
    ExpectedTb(uint8_t ndi,
               uint32_t tbSize,
               uint8_t mcs,
               uint8_t rank,
               uint16_t rnti,
               const std::vector<int>& rbBitmap,
               uint8_t harqProcessId,
               uint8_t rv,
               bool isDownlink,
               uint8_t symStart,
               uint8_t numSym,
               const SfnSf& sfn)
        : m_ndi(ndi),
          m_tbSize(tbSize),
          m_mcs(mcs),
          m_rank(rank),
          m_rnti(rnti),
          m_rbBitmap(rbBitmap),
          m_harqProcessId(harqProcessId),
          m_rv(rv),
          m_isDownlink(isDownlink),
          m_symStart(symStart),
          m_numSym(numSym),
          m_sfn(sfn)
    {
    }

    ExpectedTb() = delete;
    ExpectedTb(const ExpectedTb& o) = default;

    uint8_t m_ndi{0};            //!< New data indicator
    uint32_t m_tbSize{0};        //!< TBSize
    uint8_t m_mcs{0};            //!< MCS
    uint8_t m_rank{1};           //!< MIMO rank
    uint16_t m_rnti{0};          //!< RNTI
    std::vector<int> m_rbBitmap; //!< RB Bitmap
    uint8_t m_harqProcessId{0};  //!< HARQ process ID (MAC)
    uint8_t m_rv{0};             //!< RV
    bool m_isDownlink{false};    //!< is Downlink?
    uint8_t m_symStart{0};       //!< Sym start
    uint8_t m_numSym{0};         //!< Num sym
    SfnSf m_sfn;                 //!< SFN
};

struct TransportBlockInfo
{
    TransportBlockInfo(const ExpectedTb& expected)
        : m_expected(expected)
    {
    }

    TransportBlockInfo() = delete;

    /**
     * \brief Update minimum and average SINR of the transport block based on perceived SINR
     * \param perceivedSinr SpectrumValue with perceived SINR
     */
    void UpdatePerceivedSinr(const SpectrumValue& perceivedSinr);

    ExpectedTb m_expected;          //!< Expected data from the PHY. Filled by AddExpectedTb
    bool m_isCorrupted{false};      //!< True if the ErrorModel indicates that the TB is corrupted.
                                    //    Filled at the end of data rx/tx
    bool m_harqFeedbackSent{false}; //!< Indicate if the feedback has been sent for an entire TB
    Ptr<NrErrorModelOutput> m_outputOfEM; //!< Output of the Error Model (depends on the EM type)
    double m_sinrAvg{0.0};                //!< AVG SINR (only for the RB used to transmit the TB)
    double m_sinrMin{0.0}; //!< MIN SINR (only between the RB used to transmit the TB)
};

/**
 * \ingroup utils
 * \brief The RxPacketTraceParams struct
 */
struct RxPacketTraceParams
{
    RxPacketTraceParams(TransportBlockInfo tbInfo,
                        bool errorModelEnabled,
                        uint16_t rnti,
                        uint16_t cellId,
                        uint16_t bwpId,
                        uint8_t cqi)
        : m_cellId(cellId),
          m_rnti(rnti),
          m_frameNum(tbInfo.m_expected.m_sfn.GetFrame()),
          m_subframeNum(tbInfo.m_expected.m_sfn.GetSubframe()),
          m_slotNum(tbInfo.m_expected.m_sfn.GetSlot()),
          m_symStart(tbInfo.m_expected.m_symStart),
          m_numSym(tbInfo.m_expected.m_numSym),
          m_tbSize(tbInfo.m_expected.m_tbSize),
          m_mcs(tbInfo.m_expected.m_mcs),
          m_rank(tbInfo.m_expected.m_rank),
          m_rv(tbInfo.m_expected.m_rv),
          m_sinr(tbInfo.m_sinrAvg),
          m_sinrMin(tbInfo.m_sinrMin),
          m_tbler(errorModelEnabled ? tbInfo.m_outputOfEM->m_tbler : 0),
          m_corrupt(errorModelEnabled && tbInfo.m_isCorrupted),
          m_bwpId(bwpId),
          m_rbAssignedNum(static_cast<uint32_t>(tbInfo.m_expected.m_rbBitmap.size())),
          m_cqi(cqi) {};

    RxPacketTraceParams() = default;
    uint64_t m_cellId{0};
    uint16_t m_rnti{0};
    uint32_t m_frameNum{std::numeric_limits<uint32_t>::max()};
    uint8_t m_subframeNum{std::numeric_limits<uint8_t>::max()};
    uint16_t m_slotNum{std::numeric_limits<uint16_t>::max()};
    uint8_t m_symStart{std::numeric_limits<uint8_t>::max()};
    uint8_t m_numSym{std::numeric_limits<uint8_t>::max()};
    uint32_t m_tbSize{0};
    uint8_t m_mcs{std::numeric_limits<uint8_t>::max()};
    uint8_t m_rank{std::numeric_limits<uint8_t>::max()};
    uint8_t m_rv{std::numeric_limits<uint8_t>::max()};
    double m_sinr{-1.0};
    double m_sinrMin{-1.0};
    double m_tbler{-1.0};
    bool m_corrupt{false};
    uint16_t m_bwpId{std::numeric_limits<uint16_t>::max()};
    uint32_t m_rbAssignedNum{std::numeric_limits<uint32_t>::max()};
    uint8_t m_cqi{std::numeric_limits<uint8_t>::max()};
};

/**
 * \ingroup utils
 * \brief Store information about HARQ
 *
 * \see DlHarqInfo
 * \see UlHarqInfo
 */
struct HarqInfo
{
    virtual ~HarqInfo()
    {
    }

    uint16_t m_rnti{UINT16_MAX};        //!< RNTI
    uint8_t m_harqProcessId{UINT8_MAX}; //!< ProcessId
    uint8_t m_bwpIndex{UINT8_MAX};      //!< BWP identifier, uniquely identifies BWP within the UE

    /**
     * \return true if the HARQ should be eliminated, since the info has been
     * correctly received
     */
    virtual bool IsReceivedOk() const = 0;
};

/**
 * \ingroup utils
 * \brief A struct that contains info for the DL HARQ
 *
 * http://www.eurecom.fr/~kaltenbe/fapi-2.0/structDlInfoListElement__s.html
 * Note: This should really be called DlInfoListElement ...
 */
struct DlHarqInfo : public HarqInfo
{
    /**
     * \brief Status of the DL Harq: ACKed or NACKed
     */
    enum HarqStatus
    {
        ACK,
        NACK
    } m_harqStatus{NACK}; //!< HARQ status

    uint8_t m_numRetx; //!< Num of Retx

    bool IsReceivedOk() const override
    {
        return m_harqStatus == ACK;
    }
};

/**
 * \ingroup utils
 * \brief A struct that contains info for the UL HARQ
 */
struct UlHarqInfo : public HarqInfo
{
    std::vector<uint16_t> m_ulReception;

    enum ReceptionStatus
    {
        Ok,
        NotOk,
        NotValid
    } m_receptionStatus;

    uint8_t m_tpc{UINT8_MAX};     //!< Transmit Power Control
    uint8_t m_numRetx{UINT8_MAX}; //!< Num of Retx

    bool IsReceivedOk() const override
    {
        return m_receptionStatus == Ok;
    }
};

namespace nr
{

/**
 * \brief Base class for storing the values of vendor specific parameters
 */
struct VendorSpecificValue : public SimpleRefCount<VendorSpecificValue>
{
    virtual ~VendorSpecificValue() {};
};

/**
 * \brief See section 4.3.3 vendorSpecificListElement
 * \struct VendorSpecificListElement_s
 */
struct VendorSpecificListElement_s
{
    uint32_t m_type{UINT32_MAX};      ///< type
    uint32_t m_length{UINT32_MAX};    ///< length
    Ptr<VendorSpecificValue> m_value; ///< value
};

/**
 * \brief See section 4.3.4 logicalChannelConfigListElement
 * \struct LogicalChannelConfigListElement_s
 */
struct LogicalChannelConfigListElement_s
{
    uint8_t m_logicalChannelIdentity{UINT8_MAX}; ///< logical channel identity
    uint8_t m_logicalChannelGroup{UINT8_MAX};    ///< logical channel group

    /// Direction enum
    enum Direction_e
    {
        DIR_UL,
        DIR_DL,
        DIR_BOTH,
        NotValid
    } m_direction{NotValid}; ///< the direction

    /// QosBearerType enum
    enum QosBearerType_e
    {
        QBT_NON_GBR,
        QBT_GBR,
        QBT_DGBR,
        NotValid_QosBearerType
    } m_qosBearerType{NotValid_QosBearerType}; ///< the QOS bearer type

    uint8_t m_qci{UINT8_MAX};                       ///< QCI
    uint64_t m_eRabMaximulBitrateUl{UINT64_MAX};    ///< ERAB maximum bit rate UL
    uint64_t m_eRabMaximulBitrateDl{UINT64_MAX};    ///< ERAB maximum bit rate DL
    uint64_t m_eRabGuaranteedBitrateUl{UINT64_MAX}; ///< ERAB guaranteed bit rate UL
    uint64_t m_eRabGuaranteedBitrateDl{UINT64_MAX}; ///< ERAB guaranteed bit rate DL
};

/**
 * \brief See section 4.3.6 rachListElement
 * \struct RachListElement_s
 */
struct RachListElement_s
{
    uint16_t m_rnti{UINT16_MAX};          ///< RNTI
    uint16_t m_estimatedSize{UINT16_MAX}; ///< estimated size
};

/**
 * \brief See section 4.3.15 macCEValue
 */
struct MacCeValue_u
{
    uint8_t m_phr{UINT8_MAX};            ///< phr
    uint8_t m_crnti{UINT8_MAX};          ///< NRTI
    std::vector<uint8_t> m_bufferStatus; ///< buffer status
};

/**
 * \brief See section 4.3.14 macCEListElement
 */
struct MacCeListElement_s
{
    uint16_t m_rnti{UINT16_MAX}; ///< RNTI

    /// MAC CE type enum
    enum MacCeType_e
    {
        BSR,
        PHR,
        CRNTI,
        NotValid
    } m_macCeType{NotValid};          ///< MAC CE type
    struct MacCeValue_u m_macCeValue; ///< MAC CE value
};

} // namespace nr

std::ostream& operator<<(std::ostream& os, const DciInfoElementTdma& item);
std::ostream& operator<<(std::ostream& os, const DciInfoElementTdma::DciFormat& item);
std::ostream& operator<<(std::ostream& os, const DlHarqInfo& item);
std::ostream& operator<<(std::ostream& os, const UlHarqInfo& item);
std::ostream& operator<<(std::ostream& os, const SfnSf& item);
std::ostream& operator<<(std::ostream& os, const SlotAllocInfo& item);
std::ostream& operator<<(std::ostream& os, const SlotAllocInfo::AllocationType& item);
} // namespace ns3

/// NrSchedulingCallbackInfo structure
struct NrSchedulingCallbackInfo
{
    uint16_t m_frameNum{UINT16_MAX};  //!< frame number
    uint8_t m_subframeNum{UINT8_MAX}; //!< subframe number
    uint16_t m_slotNum{UINT16_MAX};   //!< slot number
    uint8_t m_symStart{UINT8_MAX};    //!< starting symbol index
    uint8_t m_numSym{UINT8_MAX};      //!< number of symbols
    uint16_t m_rnti{UINT16_MAX};      //!< RNTI
    uint8_t m_mcs{UINT8_MAX};         //!< MCS
    uint32_t m_tbSize{UINT32_MAX};    //!< TB size
    uint8_t m_bwpId{UINT8_MAX};       //!< Bandwidth Part ID
    uint8_t m_ndi{UINT8_MAX};         //!< New data indicator
    uint8_t m_rv{UINT8_MAX};          //!< RV
    uint8_t m_harqId{UINT8_MAX};      //!< HARQ id
};

#endif /* SRC_NR_MODEL_NR_PHY_MAC_COMMON_H_ */
