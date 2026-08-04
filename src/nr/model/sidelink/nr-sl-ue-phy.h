// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_UE_PHY_H
#define NR_SL_UE_PHY_H

#include "nr-sl-control-messages.h"
#include "nr-sl-mac-pdu-tag.h"
#include "nr-sl-sci-f1a-header.h"
#include "nr-sl-sci-f2a-header.h"
#include "nr-sl-ue-cphy-sap.h"
#include "nr-sl-ue-phy-sap.h"

#include "ns3/nr-ue-phy.h"

#include <queue>
#include <utility>

namespace ns3
{

extern const Time NR_DEFAULT_PMI_INTERVAL_WB; // Wideband PMI update interval
extern const Time NR_DEFAULT_PMI_INTERVAL_SB; // Subband PMI update interval

class NrChAccessManager;
class BeamManager;
class BeamId;
class NrUePowerControl;
class NrSlCommResourcePool;

/**
 * \ingroup ue-phy
 * \brief The SL UE PHY class
 *
 * This class represents the Sidelink PHY in the User Equipment.
 */
class NrSlUePhy : public NrUePhy
{
    /// allow MemberNrSlUePhySapProvider<NrUePhy> class friend access
    friend class MemberNrSlUeCphySapProvider<NrSlUePhy>;
    friend class MemberNrSlUePhySapProvider<NrUePhy>;

  public:
    /**
     * \brief Get the object TypeId
     * \return the object type id
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlUePhy constructor.
     */
    NrSlUePhy();

    /**
     * \brief NrSlUePhy destructor.
     */
    ~NrSlUePhy() override;

    /**
     * \brief Receive the HARQ feedback (on the transmission) from
     * NrSpectrumPhy and store it for PSFCH transmission
     *
     * Connected by the helper to a NrSpectrumPhy callback
     *
     * \param m the HARQ feedback
     */
    void EnqueueSlHarqFeedback(const SlHarqInfo& m);

    /**
     * \brief pre-configure sidelink bandwidth
     *
     * This method will used in out of coverage
     * scenarios to set the channel bandwidth.
     * In in-coverage scenario the channel bandwidth
     * is configured by RRC after receiving the MIB.
     *
     * \param slBandwidth The total sidelink channel bandwidth
     */
    void PreConfigSlBandwidth(uint16_t slBandwidth);
    /**
     * \brief Register sidelink bandwidthpart id
     *
     * \param bwpId The bandwidthpart id
     */
    void RegisterSlBwpId(uint16_t bwpId);

    /**
     * \brief Get the NR Sidelik UE PHY SAP offered by UE PHY to UE MAC
     *
     * \return the NR Sidelik UE PHY SAP provider interface offered by
     *         UE PHY to UE MAC
     */
    NrSlUePhySapProvider* GetNrSlUePhySapProvider();

    /**
     * \brief Get the NR Sidelink UE Control PHY SAP offered by PHY to RRC
     *
     * \return the NR Sidelink UE Control PHY SAP provider interface offered by
     *         PHY to RRC.
     */
    NrSlUeCphySapProvider* GetNrSlUeCphySapProvider();

    /**
     * \brief Set the NR Sidelink UE Control MAC SAP offered by RRC to PHY
     *
     * \param s the NR Sidelink UE Control MAC SAP user interface offered by
     *          RRC to PHY.
     */
    void SetNrSlUeCphySapUser(NrSlUeCphySapUser* s);

    /**
     * \brief Set the NR Sidelink UE PHY SAP offered by UE MAC to UE PHY
     *
     * \param s the NR Sidelink UE PHY SAP user interface offered to the
     *          UE PHY by UE MAC
     */
    void SetNrSlUePhySapUser(NrSlUePhySapUser* s);
    /**
     * \brief Receive new PSCCH PHY pdu from SpectrumPhy
     * \param p The packet received
     */
    void PhyPscchPduReceived(const Ptr<Packet>& p, const SpectrumValue& psd);
    /**
     * \brief Receive new successfully decoded PSSCH PHY pdu from SpectrumPhy
     * \param pb The packet burst received
     * \param psd The power spectral density received
     */
    void PhyPsschPduReceived(const Ptr<PacketBurst>& pb, const SpectrumValue& psd);
    /**
     * \brief Receive new successfully decoded PSFCH from SpectrumPhy
     * \param sendingNodeId sending nodeId
     * \param harqInfo the HARQ info
     */
    void PhyPsfchReceived(uint32_t sendingNodeId, SlHarqInfo harqInfo);

    /**
     * \brief Get the slot period
     * \return the slot period (depend on the numerology)
     */
    Time DoGetSlotPeriod() const;
    /**
     * \brief Ask the PHY the bandwidth in RBs
     *
     * \return the bandwidth in RBs
     */
    uint32_t DoGetBwInRbs() const;
    /**
     * \brief Get the numerology
     * \return the numerology
     */
    uint16_t DoGetNumerology() const;
    /**
     * \brief Send NR Sidelink PSCCH MAC PDU
     *
     * In future, when PSCCH could go without data, consider receiving
     * the pscch pkt and its var tti info using one method, e.g.,
     * SendPscchMacPdu (const SfnSf &sfn, const NrSlVarTtiAllocInfo& varTtiInfo, Ptr<Packet> p).
     * This will make sure that PHY receives var tti info of each MAC PDU
     * transmission. Then, within this method call DoSetNrSlVarTtiAllocInfo
     * and SetPscchMacPdu.
     *
     * \param p The packet
     */
    void DoSendPscchMacPdu(Ptr<Packet> p);
    /**
     * \brief Send NR Sidelink PSSCH MAC PDU
     *
     * In future, when PSCCH could go without data, consider receiving
     * the pssch pkt and its var tti info using one method, e.g.,
     * SendPsschMacPdu (const SfnSf &sfn, const NrSlVarTtiAllocInfo& varTtiInfo, Ptr<Packet> p).
     * This will make sure that PHY receives var tti info of each MAC PDU
     * transmission. Then, within this method call DoSetNrSlVarTtiAllocInfo
     * and SetPscchMacPdu.
     *
     * \param p The packet
     * \param dstL2Id The destination L2 ID
     */
    void DoSendPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id);
    /**
     * \brief Set the allocation info for NR SL slot in PHY
     * \param sfn The SfnSf
     * \param varTtiInfo The Variable TTI allocation info
     */
    void DoSetNrSlVarTtiAllocInfo(const SfnSf& sfn, const NrSlVarTtiAllocInfo& varTtiInfo);

    /**
     * \brief Indicate if HARQ feedback has been enqueued for PSFCH
     * transmission
     *
     * \returns True if there is pending HARQ feedback to send; otherwise,
     * false
     */
    bool DoIsSlHarqFeedbackPending() const;

    /**
     * TracedCallback signature for reporting SD-RSRP measurements
     * \param [in] rnti The current RNTI of the UE
     * \param [in] peerL2Id L2 ID of the peer UE
     * \param [in] rsrp The rsrp measurement
     */
    typedef void (*SdRsrpMeasurementsTracedCallback)(uint16_t rnti, uint32_t peerL2Id, double rsrp);

    /**
     * TracedCallback signature for reporting SL-RSRP measurements
     * \param [in] rnti The current RNTI of the UE
     * \param [in] peerL2Id L2 ID of the peer UE
     * \param [in] rsrp The rsrp measurement
     */
    typedef void (*SlRsrpMeasurementsTracedCallback)(uint16_t rnti, uint32_t peerL2Id, double rsrp);

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;

    /**
     * \brief Add NR Sidelink communication transmission pool
     *
     * Adds transmission pool for NR Sidelink communication
     *
     * \param txPool The pointer to the NrSlCommResourcePool
     */
    void DoAddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool);
    /**
     * \brief Add NR Sidelink communication reception pool
     *
     * Adds reception pool for NR Sidelink communication
     *
     * \param rxPool The pointer to the NrSlCommResourcePool
     */
    void DoAddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool);

  private:
    // SAP methods
    void DoReset() override;

    // overrides
    void StartSlot(const SfnSf& s) override;

    /**
     * \brief Sidelink resource assignment information about the expected NR SL transport
     *        block at a certain point in the slot
     *
     * This information will be passed by the NrUePhy to NrSpectrumPhy through a
     * call to AddSlExpectedTb
     */
    struct ResourceAssignmentInfo
    {
        /**
         * \brief constructor
         * \param rnti Tx RNTI
         * \param dstId Destination id
         * \param tbSize TB Size
         * \param mcs MCS
         * \param rbMap RB map
         * \param symStart Starting symbol index
         * \param numSym Total number of symbols
         * \param sfn SfnSf
         */
        ResourceAssignmentInfo(uint16_t rnti,
                               uint32_t dstId,
                               uint32_t tbSize,
                               uint8_t mcs,
                               const std::vector<int>& rbMap,
                               uint8_t symStart,
                               uint8_t numSym,
                               const SfnSf& sfn)
            : rnti{rnti},
              dstId{dstId},
              tbSize(tbSize),
              mcs(mcs),
              rbBitmap(rbMap),
              symStart(symStart),
              numSym(numSym),
              sfn(sfn)
        {
        }

        ResourceAssignmentInfo() = delete;
        ResourceAssignmentInfo(const ResourceAssignmentInfo& o) = default;

        ResourceAssignmentInfo(ResourceAssignmentInfo&& other)
            : rnti{other.rnti},
              dstId{other.dstId},
              tbSize(other.tbSize),
              mcs(other.mcs),
              rbBitmap(other.rbBitmap),
              symStart(other.symStart),
              numSym(other.numSym),
              sfn(other.sfn)
        {
        }

        ResourceAssignmentInfo& operator=(const ResourceAssignmentInfo& o) = default;

        uint16_t rnti{0};          //!< Tx RNTI
        uint32_t dstId{0};         //!< Destination id
        uint32_t tbSize{0};        //!< TBSize
        uint8_t mcs{0};            //!< MCS
        std::vector<int> rbBitmap; //!< RB Bitmap
        uint8_t symStart{0};       //!< Sym start
        uint8_t numSym{0};         //!< Num sym
        SfnSf sfn;                 //!< SFN
    };

    /**
     * \brief Start the NR SL slot processing
     * \param s the slot number
     */
    void StartNrSlSlot(const SfnSf& s);
    /**
     * \brief Start the processing of a NR Sidelink variable TTI
     * \param varTtiInfo the slot VarTti allocation info of the variable TTI
     *
     * This time can be a SL CTRL, a SL data, or a SL PSFCH, with
     * an appropriate number of symbols (limited to the number of symbols per
     * slot).
     *
     * At the end of processing, it schedules the method EndNrSlVarTti that will finish
     * the processing of the variable TTI allocation.
     *
     * \see EndNrSlVarTti
     */
    void StartNrSlVarTti(const NrSlVarTtiAllocInfo& varTtiInfo);
    /**
     * \brief End the processing of a NR Sidelink variable TTI
     * \param varTtiInfo the slot VarTti allocation info of the variable TTI
     *
     * The end of the NR SL variable TTI indicates that the allocation has been
     * transmitted. Depending on the variable TTI left with the slot, this method
     * will schedule another NR SL var TTI (StartNrSlVarTti()) or will start
     * new slot.
     *
     * \see StartNrSlVarTti
     * \see StartNrSlSlot
     */
    void EndNrSlVarTti(const NrSlVarTtiAllocInfo& varTtiInfo);
    /**
     * \brief Transmit NR SL CTRL and return the time at which the transmission will end
     * \param varTtiInfo the current slot VarTti allocation info to TX NR SL CTRL
     * \return the time at which the transmission of NR SL CTRL will end
     */
    Time SlCtrl(const NrSlVarTtiAllocInfo& varTtiInfo) __attribute__((warn_unused_result));
    /**
     * \brief Transmit to the spectrum phy the NR SL CTRL packet burst
     *
     * \param pb Packet burst to transmit
     * \param varTtiPeriod period of transmission
     * \param varTtiInfo the slot VarTti allocation info of the variable TTI
     */
    void SendNrSlCtrlChannels(const Ptr<PacketBurst>& pb,
                              const Time& varTtiPeriod,
                              const NrSlVarTtiAllocInfo& varTtiInfo);
    /**
     * \brief Transmit to the spectrum phy the NR SL CTRL packet burst
     *
     * \param pb Packet burst to transmit
     * \param varTtiPeriod period of transmission
     * \param varTtiInfo the slot VarTti allocation info of the variable TTI
     */
    void SendNrSlDataChannels(const Ptr<PacketBurst>& pb,
                              const Time& varTtiPeriod,
                              const NrSlVarTtiAllocInfo& varTtiInfo);
    /**
     * \brief Transmit to the spectrum phy the NR SL FB message list
     *
     * \param feedbackList list of messages to transmit
     * \param varTtiPeriod period of transmission
     * \param varTtiInfo the slot VarTti allocation info of the variable TTI
     */
    void SendNrSlFbChannels(const std::list<Ptr<NrSlHarqFeedbackMessage>>& feedbackList,
                            const Time& varTtiPeriod,
                            const NrSlVarTtiAllocInfo& varTtiInfo);
    /**
     * \brief Transmit NR SL DATA and return the time at which the transmission will end
     * \param varTtiInfo the current slot VarTti allocation info to TX NR SL DATA
     * \return the time at which the transmission of NR SL DATA will end
     */
    Time SlData(const NrSlVarTtiAllocInfo& varTtiInfo) __attribute__((warn_unused_result));
    /**
     * \brief Transmit NR SL feedback and return the time at which the transmission will end
     * \param varTtiInfo the current slot VarTti allocation info to TX NR SL FEEDBACK
     * \return the time at which the transmission of NR SL FEEDBACK will end
     */
    Time SlFeedback(const NrSlVarTtiAllocInfo& varTtiInfo) __attribute__((warn_unused_result));

    /**
     * \brief Get the Sidelink RSRP value in dBm
     *
     * At the moment, SL RSRP is computed using the PSD of the signal
     * for which we have successfully decoded the corresponding message.
     *
     * \param psd the power spectral density per each RB
     * \param nResEle the number of resource elements used for the calculation
     * \return a pair of Sidelink RSRP values in Watt and in dBm
     */
    std::pair<double, double> GetSidelinkRsrp(SpectrumValue psd, uint32_t nResEle);

    /**
     * \brief Save the future resource assignments indicated by SCI 1-A
     * \param sciF1a SCI 1-A header
     * \param tag NrSlMacPduTag
     * \param sbChSize The sub-channel size in RBs
     */
    void SaveFutureResourceAssignments(const NrSlSciF1aHeader& sciF1a,
                                       const NrSlMacPduTag& tag,
                                       const uint16_t sbChSize);
    /**
     * \brief Send Sidelink expected TB info to NrSpectrumPhy
     * \param s The SfnSf
     *
     * This method will go over the \link m_assignedResources \endlink list, which stores
     * the info about the possible expected TBs to be received in the current
     * slot without SCI 1-A, and send this info to NrSpectrumPhy.
     */
    void SendSlExpectedTbInfo(const SfnSf& s);
    NrSlUeCphySapProvider* m_nrSlUeCphySapProvider; //!< Control SAP interface to receive calls from
                                                    //!< the UE RRC instance
    NrSlUeCphySapUser* m_nrSlUeCphySapUser{
        nullptr}; //!< Control SAP interface to call the methods of UE RRC instance
    NrSlUePhySapUser* m_nrSlUePhySapUser{
        nullptr}; //!< SAP interface to call the methods of UE MAC instance
    Ptr<const NrSlCommResourcePool> m_slTxPool; //!< Sidelink communication transmission pools
    Ptr<const NrSlCommResourcePool> m_slRxPool; //!< Sidelink communication reception pools
    std::unordered_map<uint64_t, std::vector<ResourceAssignmentInfo>>
        m_assignedResources; //!< Assigned resources indicted by SCI 1-A
    std::list<std::pair<SfnSf, Ptr<NrSlHarqFeedbackMessage>>>
        m_slHarqFbList; // List of pending SL HARQ FB messages

    /**
     * Structure to keep track of the RSRP measurements of a specific UE
     * within a layer-1 filtering period
     */
    struct UeRsrpMeasurementsElement
    {
        double rsrpSum;   ///< Sum of RSRP sample values in linear unit.
        uint16_t rsrpNum; ///< Number of RSRP samples.
    };

    /**
     * Structure to store the SD-RSRP measurements of the current layer-1 filtering period.
     * Indexed by the L2Id of the UE the measurements come from
     */
    std::map<uint32_t, UeRsrpMeasurementsElement> m_ueSdRsrpMeasurementsMap;

    /**
     * Structure to store the SL-RSRP measurements of the current layer-1 filtering period.
     * Indexed by the L2Id of the UE the measurements come from
     */
    std::map<uint32_t, UeRsrpMeasurementsElement> m_ueSlRsrpMeasurementsMap;

    /**
     * True if the UE is measuring and reporting UE's SD-RSRP
     */
    bool m_ueSdRsrpMeasurementsEnabled;

    /**
     * True if the UE is measuring and reporting UE's SL-RSRP
     */
    bool m_ueSlRsrpMeasurementsEnabled;

    /**
     * The `SdRsrpMeasurementsReport` trace source. Contains trace information
     * regarding sidelink RSRP measured.
     * Exporting the current RNTI of the UE, the L2 ID of the peer UE, and the SD-RSRP (in
     * dBm)
     */
    TracedCallback<uint16_t, uint32_t, double> m_sdRsrpMeasurementsTrace;

    /**
     * The `SlRsrpMeasurementsReport` trace source. Contains trace information
     * regarding sidelink RSRP measured.
     * Exporting the current RNTI of the UE, the L2 ID of the peer UE, and the SL-RSRP (in
     * dBm)
     */
    TracedCallback<uint16_t, uint32_t, double> m_slRsrpMeasurementsTrace;

    /**
     * The RRC instructs the PHY to enable the SD-RSRP measurements of the UEs in proximity
     */
    void DoEnableUeSdRsrpMeasurements();

    /**
     * The RRC instructs the PHY to disable the SD-RSRP measurements of the UEs in proximity
     */
    void DoDisableUeSdRsrpMeasurements();

    /**
     * The RRC instructs the PHY to enable the SL-RSRP measurements of the UEs in proximity
     */
    void DoEnableUeSlRsrpMeasurements();

    /**
     * The RRC instructs the PHY to disable the SL-RSRP measurements of the UEs in proximity
     */
    void DoDisableUeSlRsrpMeasurements();

    /**
     * Perform the layer-1 filtering of SD-RSRP measurements and report the
     * results to the RRC entity.
     */
    void ReportUeSdRsrpMeasurements();

    /**
     * Perform the layer-1 filtering of SL-RSRP measurements and report the
     * results to the RRC entity.
     */
    void ReportUeSlRsrpMeasurements();

    Time m_rsrpFilterPeriod; //!< L1 Filter Period for RSRP measurements

    /**
     * \brief Pop the NR Sidelink PSCCH packet burst
     * \return The packet burst
     */
    Ptr<PacketBurst> PopPscchPacketBurst();
    /**
     * \brief Pop the NR Sidelink PSSCH packet burst
     * \return The packet burst
     */
    Ptr<PacketBurst> PopPsschPacketBurst();
    /**
     * \brief Check if there is an allocation for NR SL slot
     * \param sfn the sfn
     * \return true, if allocation info sfn matches the \p sfn and the
     *         CTRL and data queues have packet (s)
     */
    bool NrSlSlotAllocInfoExists(const SfnSf& sfn) const;

    std::vector<Ptr<PacketBurst>>
        m_nrSlPscchPacketBurstQueue; //!< A queue of NR SL PSCCH (SCI format 0) packet bursts to be
                                     //!< sent
    std::map<uint32_t, Ptr<PacketBurst>>
        m_nrSlPsschPacketBurstQueue; //!< A queue of NR SL PSSCH (SCI format 1 + Data) packet bursts
                                     //!< to be sent.
    std::deque<NrSlPhySlotAlloc> m_nrSlAllocInfoQueue; //!< Current NR SL allocation info for a slot
    NrSlPhySlotAlloc m_nrSlCurrentAlloc;               //!< Current NR SL allocation info for a slot

    /**
     * \brief Set NR Sidelink PSCCH MAC PDU
     *
     * Add the PSCCH MAC PDU to the queue
     *
     * \param p The packet
     */
    void SetPscchMacPdu(Ptr<Packet> p);
    /**
     * \brief Set NR Sidelink PSSCH MAC PDU
     *
     * Add the PSSCH MAC PDU to the queue
     *
     * \param p The packet
     * \param dstL2Id The destination L2 ID
     */
    void SetPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id);
    NrSlUePhySapProvider* m_nrSlUePhySapProvider; //!< SAP interface to receive calls from UE MAC
                                                  //!< instance for NR Sidelink
};

} // namespace ns3

#endif /* NR_SL_UE_PHY_H */
