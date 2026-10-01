// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_SCHED_SAP_PROVIDER_H
#define NR_MAC_SCHED_SAP_PROVIDER_H

#include "nr-control-messages.h"
#include "nr-phy-mac-common.h"

namespace ns3
{

class SpectrumModel;

/**
 * \ingroup scheduler
 * \brief The SAP interface between MAC and scheduler
 */
class NrMacSchedSapProvider
{
  public:
    /**
     * \brief constructor
     */
    NrMacSchedSapProvider() = default;
    /**
     * \brief NrMacSchedSapProvider copy constructor (deleted)
     * \param o other instance
     */
    NrMacSchedSapProvider(const NrMacSchedSapProvider& o) = delete;

    /**
     * ~NrMacSchedSapProvider
     */
    virtual ~NrMacSchedSapProvider() = default;

    /**
     * \brief RLC buffer status.
     */
    struct SchedDlRlcBufferReqParameters
    {
        uint16_t m_rnti;                  //!< The RNTI identifying the UE.
        uint8_t m_logicalChannelIdentity; //!< The logical channel ID, range: 0..10
        uint32_t
            m_rlcTransmissionQueueSize; //!< The current size of the new transmission queue in byte.
        uint16_t m_rlcTransmissionQueueHolDelay; //!< Head of line delay of new transmissions in ms.
        uint32_t
            m_rlcRetransmissionQueueSize; //!< The current size of the retransmission queue in byte.
        uint16_t m_rlcRetransmissionHolDelay; //!< Head of line delay of retransmissions in ms.
        uint16_t m_rlcStatusPduSize; //!< The current size of the pending STATUS message in byte.
    };

    /**
     * \brief The SchedDlCqiInfoReqParameters struct
     */
    struct SchedDlCqiInfoReqParameters
    {
        SfnSf m_sfnsf;                           //!< SfnSf
        std::vector<struct DlCqiInfo> m_cqiList; //!< cqi list
    };

    /**
     * \brief The SchedUlMacCtrlInfoReqParameters struct
     */
    struct SchedUlMacCtrlInfoReqParameters
    {
        SfnSf m_sfnSf;                                //!< SfnSf
        std::vector<struct MacCeElement> m_macCeList; //!< MacCeElement list
    };

    /**
     * \brief The SchedUlCqiInfoReqParameters struct
     */
    struct SchedUlCqiInfoReqParameters
    {
        SfnSf m_sfnSf;            //!< SfnSf
        uint8_t m_symStart;       //!< Sym start of the transmission to which this CQI refers to
        struct UlCqiInfo m_ulCqi; //!< UL CQI
    };

    /**
     * \brief UL HARQ information to be used when scheduling UL data.
     */
    struct SchedUlTriggerReqParameters
    {
        SfnSf m_snfSf;                                   //!< SfnSf
        std::vector<struct UlHarqInfo> m_ulHarqInfoList; //!< UL HARQ info list
        LteNrTddSlotType m_slotType{F};                  //!< Indicate the type of slot requested
    };

    /**
     * \brief DL HARQ information to be used when scheduling UL data.
     */
    struct SchedDlTriggerReqParameters
    {
        SfnSf m_snfSf;                                   //!< SfnSf
        std::vector<struct DlHarqInfo> m_dlHarqInfoList; //!< DL HARQ info list
        LteNrTddSlotType m_slotType{F};                  //!< Indicate the type of slot requested
    };

    /**
     * \brief SR received from MAC, to pass to schedulers
     *
     * Scheduling request information.
     *
     * http://www.eurecom.fr/~kaltenbe/fapi-2.0/structSchedUlSrInfoReqParameters.html
     */
    struct SchedUlSrInfoReqParameters
    {
        SfnSf m_snfSf;                  //!< SnfSf in which the sr where received
        std::vector<uint16_t> m_srList; //!< List of RNTI which asked for a SR
    };

    /**
     * Parameters of the SCHED_DL_RACH_INFO_REQ primitive.
     * See section 4.2.5 for a detailed description of the parameters.
     */
    struct SchedDlRachInfoReqParameters
    {
        uint16_t m_sfnSf;                                     //!< sfn SF
        std::vector<struct nr::RachListElement_s> m_rachList; //!< RACH list

        std::vector<struct nr::VendorSpecificListElement_s>
            m_vendorSpecificList; ///< vendor specific list
    };

    virtual void SchedDlRlcBufferReq(const struct SchedDlRlcBufferReqParameters& params) = 0;

    virtual void SchedDlCqiInfoReq(const SchedDlCqiInfoReqParameters& params) = 0;

    /**
     * \brief Starts the DL MAC scheduler for this subframe.
     * \param params      DL HARQ information
     */
    virtual void SchedDlTriggerReq(const struct SchedDlTriggerReqParameters& params) = 0;

    virtual void SchedUlCqiInfoReq(const struct SchedUlCqiInfoReqParameters& params) = 0;

    /**
     * \brief Starts the UL MAC scheduler for this subframe.
     * \param params      UL HARQ information
     */
    virtual void SchedUlTriggerReq(const struct SchedUlTriggerReqParameters& params) = 0;

    /**
     * \brief Provides scheduling request reception information to the scheduler.
     * \param params Scheduling request information.
     */
    virtual void SchedUlSrInfoReq(const SchedUlSrInfoReqParameters& params) = 0;

    virtual void SchedUlMacCtrlInfoReq(const struct SchedUlMacCtrlInfoReqParameters& params) = 0;

    virtual void SchedSetMcs(uint32_t mcs) = 0;

    /**
     * \brief SCHED_DL_RACH_INFO_REQ
     *
     * \param params SchedDlRachInfoReqParameters
     */
    virtual void SchedDlRachInfoReq(const SchedDlRachInfoReqParameters& params) = 0;

    /**
     * \brief Retrieve the number of DL ctrl symbols configured in the scheduler
     * \return the number of DL ctrl symbols
     */
    virtual uint8_t GetDlCtrlSyms() const = 0;

    /**
     * \brief Retrieve the number of UL ctrl symbols configured in the scheduler
     * \return the number of UL ctrl symbols
     */
    virtual uint8_t GetUlCtrlSyms() const = 0;

  private:
};

/**
 * \ingroup scheduler
 * \brief The Interface between Scheduler and MAC
 */
class NrMacSchedSapUser
{
  public:
    /**
     * \brief ~NrMacSchedSapUser
     */
    virtual ~NrMacSchedSapUser() = default;

    /**
     * \brief The SchedConfigIndParameters struct
     */
    struct SchedConfigIndParameters
    {
        /**
         * \brief SchedConfigIndParameters
         * \param sfnSf sfnSf
         */
        SchedConfigIndParameters(const SfnSf sfnSf)
            : m_sfnSf(sfnSf),
              m_slotAllocInfo(sfnSf)
        {
        }

        const SfnSf m_sfnSf;           //!< The SfnSf
        SlotAllocInfo m_slotAllocInfo; //!< The allocation info
    };

    /**
     * \brief Install a scheduling decision
     * \param params the scheduling decision
     */
    virtual void SchedConfigInd(const struct SchedConfigIndParameters& params) = 0;

    /**
     * \brief Get the SpectrumModel
     * \return the spectrum model
     */
    virtual Ptr<const SpectrumModel> GetSpectrumModel() const = 0;

    /**
     * \brief Get the number of RB per RBG
     * \return Number of RB per RBG
     */
    virtual uint32_t GetNumRbPerRbg() const = 0;

    /**
     * \brief Get the number of HARQ process
     * \return the number of HARQ processes
     */
    virtual uint8_t GetNumHarqProcess() const = 0;

    /**
     * \brief Get the BWP ID
     * \return the BWP ID
     */
    virtual uint16_t GetBwpId() const = 0;

    /**
     * \brief Get the Cell ID
     * \return the Cell ID
     */
    virtual uint16_t GetCellId() const = 0;

    /**
     * \brief Get the Symbol per slot
     * \return the symbol per slot
     */
    virtual uint32_t GetSymbolsPerSlot() const = 0;

    /**
     * \brief Get the slot period
     * \return the slot period
     */
    virtual Time GetSlotPeriod() const = 0;

    /**
     * \brief Build RAR list from allocations and assign preamble IDs
     * @param slotAllocInfo Allocations
     */
    virtual void BuildRarList(SlotAllocInfo& slotAllocInfo) = 0;
};

inline std::ostream&
operator<<(std::ostream& os, const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& p)
{
    os << "RNTI: " << p.m_rnti << " LCId: " << static_cast<uint32_t>(p.m_logicalChannelIdentity)
       << " RLCTxQueueSize: " << p.m_rlcTransmissionQueueSize
       << " B, RLCTXHolDel: " << p.m_rlcTransmissionQueueHolDelay
       << " ms, RLCReTXQueueSize: " << p.m_rlcRetransmissionQueueSize
       << " B, RLCReTXHolDel: " << p.m_rlcRetransmissionHolDelay
       << " ms, RLCStatusPduSize: " << p.m_rlcStatusPduSize << " B.";
    return os;
}

} // namespace ns3

#endif /* NR_MAC_SCHED_SAP_PROVIDER_H */
