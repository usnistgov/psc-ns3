// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_UE_MAC_H
#define NR_UE_MAC_H

#include "nr-ccm-mac-sap.h"
#include "nr-phy-mac-common.h"
#include "nr-ue-cmac-sap.h"

#include <ns3/traced-callback.h>

#include <map>

namespace ns3
{

class NrUePhySapUser;
class NrPhySapProvider;
class NrControlMessage;
class UniformRandomVariable;
class PacketBurst;
class NrUlDciMessage;

/**
 * \ingroup ue-mac
 * \brief The MAC class for the UE
 *
 * \section ue_mac_general General information
 *
 * The UE MAC has not much freedom, as it is directed by the GNB at which it
 * is attached to. For the attachment phase, we follow (more or less) what is
 * happening in LENA. After the UE is attached, the things become different.
 *
 * \section ue_mac_sr Scheduling Request
 *
 * When a RLC (please remember, there are as many RLC as many bearer the UE has)
 * tells the UE that there is some data in its queue, the UE MAC activates the
 * SR state machine. So, the UE sends a control message (called Scheduling Request)
 * to the PHY, that in turn has to deliver it to the GNB. This message says that
 * the UE has some data to transmit (without indicating the precise quantity).
 *
 * The GNB then allocates some data (a quantity that is implementation-defined)
 * to the UE, in which it can transmit data and a SHORT_BSR.
 *
 * \section ue_mac_response_to_dci Response to a DCI
 *
 * When the UE receives an UL_DCI, it can use a part of it to send a Control Element.
 * The most used control element (and, by the way, the only we support right now)
 * is SHORT_BSR, in which the UE informs the GNB of its buffer status. The rest
 * of the bytes can be used to send data.
 *
 * In the standard, the UE is allowed to send a single PDU in response to a
 * single UL DCI. Such PDU can be formed by one or more subPDU, each one consisting
 * in a header and a data (the data is optional). However, due to limitations in
 * serialization/deserialization of packets in the ns-3 simulator, we are bending
 * a little the definition. The MAC is allowed to send as many PDU as it wants,
 * but these PDU (that are, in fact, packets) should be enqueued in the PHY at
 * the same frame/subframe/slot/symbol. The PHY will use the concept of `PacketBurst`
 * to consider all these PDUs as a single, big, PDU. Practically speaking, the
 * result is the same as grouping these subPDU in a single PDU, just that the
 * single PDU is in reality a PacketBurst. As the order in a PacketBurst cannot
 * be maintained, it is impossible to respect the standard at the ordering part
 * (DL and UL PDU are formed in an opposite way, with CE at the beginning or
 * at the end of the PDU).
 *
 * The action of sending a subPDU to the PHY is done by the method DoTransmitPdu().
 * However, the MAC has to evaluate the received TBS, in light of how many
 * Logical Channel ID are active, and what data they have to transmit. In doing
 * all this, the MAC has to keep in consideration that each subPDU will have
 * a header prefixed, which is an overhead, using bytes that were originally
 * supposed to be assigned to data.
 *
 * The core of this small "scheduling" is done in method SendNewData(), in which
 * the MAC will try to send as many status-subPDUs as possible, then will try
 * to send as many as retx-subPDUs as possible, and finally as many as tx-subPDUs
 * as possible. At the end of all the subPDUs, it will be sent a SHORT_BSR, to
 * indicate to the GNB the new status of the RLC queues. As the SHORT_BSR is
 * a CE and is treated in the same way as data, it may be lost. Please note that
 * the code subtract the amount of bytes devoted to the SHORT_BSR from the
 * available ones, so there will always be a space to send it. The only
 * exception (theoretically possible) is when the status PDUs use all the
 * available space; in this case, a rework of the code will be needed.
 *
 * The SHORT_BSR is not reflecting the standard, but it is the same data that
 * was sent in LENA, indicating the status of 4 LCG at once with an 8-bit value.
 * Making this part standard-compliant is a good novice exercise.
 *
 * \section ue_mac_configuration Configuration
 *
 * The user can configure the class using the method NrHelper::SetUeMacAttribute(),
 * or by directly calling `SetAttribute` on the MAC pointer. The list of
 * attributes is reported below, in the Attributes section.
 *
 * \section ue_mac_traces CTRL-trace Traces for CTRL messages
 *
 * The class has two attributes that signals to the eventual listener the
 * transmission or the reception of CTRL messages. One is UeMacRxedCtrlMsgsTrace,
 * and the other is UeMacTxedCtrlMsgsTrace. For what regards the PHY, you will
 * find more information in the NrUePhy class documentation.
 */
class NrUeMac : public Object
{
    friend class UeMemberNrUeCmacSapProvider;
    friend class UeMemberNrMacSapProvider;
    friend class MacUeMemberPhySapUser;

  public:
    /**
     * \brief Get the Type id
     * \return the type id
     */
    static TypeId GetTypeId();

    /**
     * \brief NrUeMac constructor
     */
    NrUeMac();
    /**
     * \brief Deconstructor
     */
    ~NrUeMac() override;

    /**
     * \brief Set the C MAC SAP user (AKA the RRC representation for the MAC)
     * \param s the SAP pointer
     */
    void SetUeCmacSapUser(NrUeCmacSapUser* s);

    /**
     * \brief Get the C MAC SAP provider (AKA the MAC representation for the RRC)
     * \return  C MAC SAP provider (AKA the MAC representation for the RRC)
     */
    NrUeCmacSapProvider* GetUeCmacSapProvider();

    /**
     * \brief Get the Mac SAP provider (AKA the MAC representation for the RLC)
     * \return the Mac SAP provider (AKA the MAC representation for the RLC)
     */
    NrMacSapProvider* GetUeMacSapProvider();

    /**
     * \brief Get the PHY SAP User (AKA the MAC representation for the PHY)
     * \return the PHY SAP User (AKA the MAC representation for the PHY)
     */
    NrUePhySapUser* GetPhySapUser();

    /**
     * \brief Set PHY SAP provider (AKA the PHY representation for the MAC)
     * \param ptr the PHY SAP provider (AKA the PHY representation for the MAC)
     */
    void SetPhySapProvider(NrPhySapProvider* ptr);

    /**
     *  TracedCallback signature for Ue Mac Received Control Messages.
     * \param [in] sfnSf Frame number, subframe number, slot number, VarTti
     * \param [in] nodeId the node ID
     * \param [in] rnti the RNTI
     * \param [in] bwpId the BWP ID
     * \param [in] ctrlMessage the pointer to msg to get the msg type
     */
    typedef void (*RxedUeMacCtrlMsgsTracedCallback)(const SfnSf sfnSf,
                                                    const uint16_t nodeId,
                                                    const uint16_t rnti,
                                                    const uint8_t bwpId,
                                                    Ptr<NrControlMessage> ctrlMessage);
    /**
     *  TracedCallback signature for Ue Mac Transmitted Control Messages.
     * \param [in] sfnSf the frame number, subframe number, slot number, VarTti
     * \param [in] nodeId the node ID
     * \param [in] rnti the RNTI
     * \param [in] bwpId the BWP ID
     * \param [in] ctrlMessage the pointer to msg to get the msg type
     */
    typedef void (*TxedUeMacCtrlMsgsTracedCallback)(const SfnSf sfnSf,
                                                    const uint16_t nodeId,
                                                    const uint16_t rnti,
                                                    const uint8_t bwpId,
                                                    Ptr<NrControlMessage> ctrlMessage);

    /**
     * \brief Sets the number of HARQ processes.
     * Called by the helper at the moment of UE attachment
     * \param numHarqProcesses the maximum number of harq processes
     */
    void SetNumHarqProcess(uint8_t numHarqProcesses);

    /**
     * \brief Please remember that this number is obtained by the GNB, the UE
     * cannot configure it.
     *
     * \return number of HARQ processes
     */
    uint8_t GetNumHarqProcess() const;

    /**
     * \brief Get the bwp id of this MAC
     * \return the bwp id
     */
    uint16_t GetBwpId() const;

    /**
     * \brief Get the cell id of this MAC
     * \return the cell id
     */
    uint16_t GetCellId() const;

    /**
     * \brief Get the RNTI
     * \return the RNTI
     */
    uint16_t GetRnti() const;

    /**
     * \brief Get the IMSI
     * \return the IMSI
     */
    uint64_t GetImsi() const;

    /**
     * \brief Assign a fixed random variable stream number to the random variables
     * used by this model. Returns the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;

    /**
     * \brief Set the frame/subframe/slot counter
     * \param sfn the SfnSf
     */
    void SetCurrentSlot(const SfnSf& sfn);

    /**
     * Assign a fixed random variable stream number to the random variables used by this model.
     *
     * Subclasses that override this method are expected to chain up to
     * their parent's implementation and then set their own streams
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    virtual int64_t DoAssignStreams(int64_t stream);

  private:
    /**
     * \brief Received a RA response
     * \param raResponse the response
     */
    void RecvRaResponse(NrBuildRarListElement_s raResponse);
    /**
     * \brief Set the RNTI
     */
    void SetRnti(uint16_t);
    /**
     * \brief Do some work, we begin a new slot
     * \param sfn the new slot
     */
    virtual void DoSlotIndication(const SfnSf& sfn);

    /**
     * \brief Get the total size of the RLC buffers.
     * \return The number of bytes that are in the RLC buffers
     */
    uint32_t GetTotalBufSize() const __attribute__((warn_unused_result));

    /**
     * \brief Send to the PHY a SR
     */
    void SendSR() const;
    /**
     * \brief Called by RLC to transmit a RLC PDU
     * \param params the RLC params
     *
     * Please note that this call is triggered by communicating to the RLC
     * that there is a new transmission opportunity with NotifyTxOpportunity().
     *
     * The method is called DoTransmitPdu, however, it may happen that multiple
     * PDUs need to be send in the same frame/subframe/slot/symbol, in this case,
     * they will be grouped (to imitate subPdus) by PHY into a PacketBurst that
     * represents a PDU.
     */
    void DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params);

    /**
     * \brief Called by CCM
     * \param params the BSR params
     *
     * The CCM is calling this function for all the MAC of the UE. This method
     * will send SR only for CC ID = 0 (BwpManagerGnb will take care of
     * routing the SR to the appropriate MAC).
     *
     * \see DoSlotIndication
     */
    void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params);

    // forwarded from PHY SAP
    void DoReceivePhyPdu(Ptr<Packet> p);
    void DoReceiveControlMessage(Ptr<NrControlMessage> msg);
    // void DoNotifyHarqDeliveryFailure (uint8_t harqId);
    void RaResponseTimeout(bool contention);

    // forwarded from UE CMAC SAP
    void DoConfigureRach(NrUeCmacSapProvider::RachConfig rc);
    void DoStartContentionBasedRandomAccessProcedure();
    void DoStartNonContentionBasedRandomAccessProcedure(uint16_t rnti,
                                                        uint8_t rapId,
                                                        uint8_t prachMask);
    void AddLc(uint8_t lcId, NrUeCmacSapProvider::LogicalChannelConfig lcConfig, NrMacSapUser* msu);
    void DoRemoveLc(uint8_t lcId);
    void DoReset();
    void DoNotifyConnectionSuccessful();
    void DoSetImsi(uint64_t imsi);

    void RandomlySelectAndSendRaPreamble();
    void SendRaPreamble(bool contention);

    /**
     * \brief Send a Report Buffer Status
     * \param dataSfn data slot
     * \param symStart symStart
     *
     * Please note that the BSR is not saved in the HARQ buffer, so it will
     * not get retransmitted.
     */
    void SendReportBufferStatus(const SfnSf& dataSfn, uint8_t symStart);
    void RefreshHarqProcessesPacketBuffer();

    /**
     * \brief Process the received UL DCI
     * \param dciMsg the UL DCI received
     *
     * The method will call SendNewData() or TransmitRetx() (depending on the UL
     * DCI type), that will take care of sending data out taking into account the
     * header overhead. After sending new data, the method is allowed to enqueue
     * a BSR if there are still bytes in the queue.
     *
     */
    void ProcessUlDci(const Ptr<NrUlDciMessage>& dciMsg);

    /**
     * \brief Transmit a retransmission (good joke, eh?)
     *
     * The method uses the DCI stored in m_ulDci to take the HARQ process id,
     * preparing the subPDUs that are waiting in such HARQ process,
     * and sending them again.
     */
    void TransmitRetx();

    /**
     * \brief Send data after an UL DCI
     *
     * The method takes care of checking how many subPDUs we have to send,
     * and with a very rough estimation, tries to allocate data to all the active
     * LCID.
     *
     * \see SendNewStatusData()
     * \see SendRetxData()
     * \see SendTxData()
     */
    void SendNewData();

    /**
     * \brief Send STATUS PDUs
     *
     * This method will try to use the allocated resources by the UL_DCI to send
     * StatusPDU, if they are present, for all the LCID.
     */
    void SendNewStatusData();

    /**
     * \brief Send RETX data
     *
     * \param usefulTbs TBS that we can use (data only)
     * \param activeRetx number of active LCID with some data in the retxQueue
     *
     * The method will try to use the allocated resources by the UL_DCI to send
     * data in the retxQueue of the various active LCID.
     *
     * \todo the code is similar to SendTxData, maybe they can be unified.
     */
    void SendRetxData(uint32_t usefulTbs, uint32_t activeRetx);

    /**
     * \brief Send TX data
     *
     * \param usefulTbs TBS that we can use (data only)
     * \param activeTx number of active LCID with some data in the txQueue
     *
     * The method will try to use the allocated resources by the UL_DCI to send
     * data in the txQueue of the various active LCID.
     *
     * \todo the code is similar to SendReTxData, maybe they can be unified.
     */
    void SendTxData(uint32_t usefulTbs, uint32_t activeTx);

  private:
    NrUeCmacSapUser* m_cmacSapUser{nullptr};
    NrUeCmacSapProvider* m_cmacSapProvider{nullptr};
    NrPhySapProvider* m_phySapProvider{nullptr};
    NrUePhySapUser* m_phySapUser{nullptr};
    NrMacSapProvider* m_macSapProvider{nullptr};

    SfnSf m_currentSlot;          //!< The current slot
    uint8_t m_numHarqProcess{20}; //!< number of HARQ processes

    std::shared_ptr<DciInfoElementTdma>
        m_ulDci;                  //!< Received a DCI. While we process it, store it here.
    SfnSf m_ulDciSfnsf;           //!< Received a DCI for transmitting data in this slot.
    uint32_t m_ulDciTotalUsed{0}; //!< Received a DCI, put the total count of bytes we sent.

    std::unordered_map<uint8_t, NrMacSapProvider::ReportBufferStatusParameters>
        m_ulBsrReceived; //!< BSR received from RLC (the last one)

    /**
     * \brief States for the SR/BSR mechanism.
     *
     * The SR/BSR mechanism is based on a variable in which
     * it is saved the state (INACTIVE/ACTIVE).
     *
     * The machine is starting from the INACTIVE state. When the RLC notifies
     * to MAC that there are new bytes in its queue (DoReportBufferStatus()),
     * if the machine is in INACTIVE state, it enters the ACTIVE state.
     * Entering the ACTIVE state means to send a SR, which is enqueued in the PHY layer.
     * It will suffer slots of CTRL latency. If the state is already ACTIVE, then
     * the BSR can be sent in the same slot as data. It means that the MAC prepares
     * together the data and the BSR.
     *
     * If the BSR is not sent (we don't have any data in the queue) and we don't
     * have any more reserved space to send BSR, then the state goes back to the
     * INACTIVE state.
     */
    enum SrBsrMachine : uint8_t
    {
        INACTIVE, //!< no SR nor BSR.. initial state
        TO_SEND,  //!< We have to send the BSR when possible
        ACTIVE //!< SR or BSR sent; now the source of information is the vector m_bsrReservedSpace
    };

    SrBsrMachine m_srState{INACTIVE}; //!< Current state for the SR/BSR machine.

    Ptr<UniformRandomVariable> m_raPreambleUniformVariable;
    uint8_t m_raPreambleId{0}; //!< The RA Preamble ID
    uint8_t m_raRnti{0};       //!< The RA Rnti
    uint64_t m_imsi{0};        ///< IMSI

    // The HARQ part has to be reviewed
    struct UlHarqProcessInfo
    {
        Ptr<PacketBurst> m_pktBurst;
        // maintain list of LCs contained in this TB
        // used to signal HARQ failure to RLC handlers
        std::vector<uint8_t> m_lcidList;
    };

    // uint8_t m_harqProcessId;
    std::vector<UlHarqProcessInfo>
        m_miUlHarqProcessesPacket; //!< Packets under transmission of the UL HARQ processes
    std::vector<uint8_t> m_miUlHarqProcessesPacketTimer; //!< timer for packet life in the buffer

    struct LcInfo
    {
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig;
        NrMacSapUser* macSapUser;
    };

    std::map<uint8_t, LcInfo> m_lcInfoMap;                 //!< Map of logical channel ID to LcInfo
    uint16_t m_rnti{std::numeric_limits<uint16_t>::max()}; //!< RNTI assigned by the RRC

    bool m_waitingForRaResponse{true}; //!< Indicates if we are waiting for a RA response
    static uint8_t g_raPreambleId;     //!< Preamble ID, fixed, the UEs will not have any collision

    /**
     * Trace information regarding Ue MAC Received Control Messages
     * Frame number, Subframe number, slot, VarTtti, nodeId, rnti, bwpId,
     * pointer to message in order to get the msg type
     */
    TracedCallback<SfnSf, uint16_t, uint16_t, uint8_t, Ptr<const NrControlMessage>>
        m_macRxedCtrlMsgsTrace;

    /**
     * Trace information regarding Ue MAC Transmitted Control Messages
     * Frame number, Subframe number, slot, VarTtti, nodeId, rnti, bwpId,
     * pointer to message in order to get the msg type
     */
    TracedCallback<SfnSf, uint16_t, uint16_t, uint8_t, Ptr<const NrControlMessage>>
        m_macTxedCtrlMsgsTrace;

    /**
     * \brief The `RaResponseTimeout` trace source. Fired RA response timeout.
     * Exporting IMSI, contention flag, preamble transmission counter
     * and the max limit of preamble transmission.
     */
    TracedCallback<uint64_t, bool, uint8_t, uint8_t> m_raResponseTimeoutTrace;

    void StartWaitingForRaResponse();
    bool m_rachConfigured = false;                ///< is RACH configured?
    NrUeCmacSapProvider::RachConfig m_rachConfig; ///< RACH configuration
    uint8_t m_preambleTransmissionCounter{0};     ///< preamble transmission counter
    EventId m_noRaResponseReceivedEvent;          ///< no RA response received event ID
};

} // namespace ns3

#endif /* NR_UE_MAC_H */
