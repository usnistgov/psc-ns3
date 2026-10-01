// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_SPECTRUM_PHY_H
#define NR_SL_SPECTRUM_PHY_H

#include "nr-sl-chunk-processor.h"
#include "nr-sl-error-model.h"
#include "nr-sl-harq-phy.h"
#include "nr-sl-interference.h"
#include "nr-sl-phy-mac-common.h"
#include "nr-sl-spectrum-signal-parameters.h"

#include "ns3/nr-spectrum-phy.h"
#include <ns3/traced-value.h>

#include <functional>

namespace ns3
{

/**
 * \ingroup ue-phy
 * \ingroup gnb-phy
 * \ingroup spectrum
 *
 * \brief Interface between the physical layer and the channel
 *
 * NrSlSpectrumPhy is the sidelink-specific variant of NrSpectrumPhy.
 * See the NrSpectrumPhy documentation for more information.
 *
 * \see NrSpectrumPhy
 */
class NrSlSpectrumPhy : public NrSpectrumPhy
{
  public:
    /**
     * \brief Get the object TypeId
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlSpectrumPhy constructor
     */
    NrSlSpectrumPhy();

    /**
     * This callback method type is used by the NrSlSpectrumPhy to notify the PHY about
     * the status of a SL HARQ feedback
     */
    typedef Callback<void, const SlHarqInfo&> NrPhySlHarqFeedbackCallback;

    /**
     * \brief Sets the callback to be called when SL HARQ feedback is generated
     */
    void SetPhySlHarqFeedbackCallback(const NrPhySlHarqFeedbackCallback& c);

    /**
     * \brief Inherited from NrSpectrumPhy. When this function is called
     * this spectrum phy starts receiving a signal from its spectrum channel.
     * \param params SpectrumSignalParameters object that will be used to process this signal
     */
    void StartRx(Ptr<SpectrumSignalParameters> params) override;

    /**
     * \brief Sets noise power spectral density to be used by this device
     * \param noisePsd SpectrumValue object holding noise PSD
     */
    void SetNoisePowerSpectralDensity(const Ptr<const SpectrumValue>& noisePsd) override;

    /**
     * \brief Structure to store NR Sidelink signal parameter being received
     *        along with the vector indicating the indexes of the RBs this signal
     *        is transmitted over.
     */
    struct SlRxSigParamInfo
    {
        Ptr<NrSpectrumSignalParametersSlFrame> params; //!< Parameters of sidelink signal
        std::vector<int> rbBitmap;                     //!< RB bitmap
    };

    /**
     * \brief SlCtrlSigParamInfo structure
     */
    struct SlCtrlSigParamInfo
    {
        double sinrAvg{0.0};                                  //!< Average SINR
        double sinrMin{0.0};                                  //!< Minimum SINR
        uint32_t index{std::numeric_limits<uint32_t>::max()}; //!< Index of the signal received in
                                                              //!< the reception buffer

        /**
         * \brief Implements equal operator
         * \param a element to compare
         * \param b element to compare
         * \return true if the elements are equal
         */
        friend bool operator==(const SlCtrlSigParamInfo& a, const SlCtrlSigParamInfo& b);

        /**
         * \brief Implements less operator
         * \param a element to compare
         * \param b element to compare
         * \return true if a is less than b
         */
        friend bool operator<(const SlCtrlSigParamInfo& a, const SlCtrlSigParamInfo& b);
    };

    /**
     * \brief struct to store the PSCCH PDU info
     *
     * This struct is used to store the packet and PSD of a
     * PSCCH transmission.
     */
    struct PscchPduInfo
    {
        Ptr<Packet> packet; //!< PSCCH packet
        SpectrumValue psd;  //!< PSD of the received packet
    };

    /**
     * \brief SlTbId struct
     */
    struct SlTbId
    {
        uint16_t m_rnti;  //!< source SL-RNTI
        uint32_t m_dstId; //!< The destination id
    };

    /**
     * \brief This callback method type is used to notify about a successful PSCCH reception
     */
    typedef std::function<void(const Ptr<Packet>&, const SpectrumValue&)> NrPhyRxPscchEndOkCallback;
    /**
     * \brief This callback method type is used to notify about a successful
     *        PSSCH reception.
     */
    typedef std::function<void(const Ptr<PacketBurst>&, const SpectrumValue&)>
        NrPhyRxPsschEndOkCallback;
    /**
     * \brief This callback method type is used to notify about a unsuccessful
     *        PSSCH reception.
     */
    typedef std::function<void(const Ptr<PacketBurst>&)> NrPhyRxPsschEndErrorCallback;
    /**
     * \brief This callback method type is used to notify about a successful
     *        PSFCH reception.
     */
    typedef std::function<void(uint32_t, SlHarqInfo)> NrPhyRxSlPsfchCallback;
    /**
     * \brief Sets the NR sidelink error model type
     *
     * \param errorModelType The TypeId of the error model to be used.
     */
    void SetSlErrorModelType(TypeId errorModelType);
    /**
     * \brief Enables or disabled NR Sidelink data error model
     * \param slDataErrorModelEnabled boolean saying whether the NR SL data error model should be
     * enabled
     */
    void SetSlDataErrorModelEnabled(bool slDataErrorModelEnabled);
    /**
     * \brief Enables or disabled NR Sidelink CTRL error model
     * \param slCtrlErrorModelEnabled boolean saying whether the NR SL CTRL error model should be
     * enabled
     */
    void SetSlCtrlErrorModelEnabled(bool slCtrlErrorModelEnabled);
    /**
     * \brief Enable or disable the drop of a NR SL TB whose RB collided with
     *        other TB.
     *  Note: The same flag is used to enable/disable the drop NR SL PSCCH
     *        packet burst.
     * \param drop If true, a TB (for PSSCH) or a packet burst (for PSCCH),
     *        regardless of SINR value, is drop if its RBs collided with other
     *        TB or packet burst. Otherwise, its reception will depend on
     *        the error model output and a random probability of decoding.
     */
    void DropTbOnRbOnCollision(bool drop);
    /**
     * \brief Starts transmission of NR SL data frames on connected spectrum channel object
     * \param pb packet burst to be transmitted
     * \param duration the duration of transmission
     */
    void StartTxSlDataFrames(const Ptr<PacketBurst>& pb, Time duration);
    /**
     * \brief Starts transmission of NR SL CTRL data frames on connected spectrum channel object
     * \param pb packet burst to be transmitted
     * \param duration the duration of transmission
     */
    void StartTxSlCtrlFrames(const Ptr<PacketBurst>& pb, Time duration);
    /**
     * \brief Starts transmission of NR SL FB symbols on connected spectrum channel object
     * \param feedbackList messages to be transmitted
     * \param duration the duration of transmission
     */
    void StartTxSlFeedback(const std::list<Ptr<NrSlHarqFeedbackMessage>>& feedbackList,
                           const Time& duration);
    /**
     * \brief Adds the NR SL chunk processor that passes the SINR of received
     *        signal (s) to this SpectrumPhy once its reception ends.
     * \param p The new NrSlChunkProcessor to be added to the NR Sidelink processing chain
     */
    void AddSlSinrChunkProcessor(Ptr<NrSlChunkProcessor> p);
    /**
     * \brief Adds the NR SL chunk processor that passes the PSD of received
     *        signal (s) to this SpectrumPhy once its reception ends.
     * \param p The new NrSlChunkProcessor to be added to the NR Sidelink processing chain
     */
    void AddSlSignalChunkProcessor(Ptr<NrSlChunkProcessor> p);
    /**
     * \brief This method will be called when the SINR for the received
     *        NR Sidelink signal, i.e., PSCCH or PSSCH is being calculated by
     *        the interference object over Sidelink chunk processor.
     * \param sinr The vector of the resulting SINR values per Sidelink packet.
     *        These SINR values are spectrum values per each RB.
     */
    void UpdateSlSinrPerceived(std::vector<SpectrumValue> sinr);
    /**
     * \brief This method will be called when the PSD for the received
     *        NR Sidelink signal, i.e., PSCCH or PSSCH is being calculated by
     *        the interference object over Sidelink chunk processor.
     * \param sig The vector of the resulting PSD values per Sidelink packet.
     *        These PSD values are spectrum values per each RB.
     */
    void UpdateSlSignalPerceived(std::vector<SpectrumValue> sig);
#if 0
    /**
     * \brief Set NR SL AMC
     *
     * I needed to add the to compute the size of PSCCH TB to compute BLER.
     * Theoretically, the SL scheduler should compute this TB size and NrUeMac
     * should include it in Tag or something. At the moment, I am already
     * consuming 20 bytes with the NrSlMacPduTag. If I will remove some fields
     * in the future, maybe, I will include PSCCH TB size there.
     *
     * \param slAmc NR SL AMC
     */
    void SetSlAmc(Ptr<NrAmc> slAmc);
#endif
    /**
     * \brief Set SL error model
     * \param slErrorModel the sidelink error model
     */
    void SetSlErrorModel(Ptr<NrSlErrorModel> slErrorModel);
    /**
     * \brief Get SL error model
     * \return the sidelink error model
     */
    Ptr<const NrSlErrorModel> GetSlErrorModel() const;
    /**
     * \brief Set the callback for the successful end of a PSCCH RX, as part of the
     * interconnections between the PHY and the MAC
     *
     * \param c The callback
     */
    void SetNrPhyRxPscchEndOkCallback(NrPhyRxPscchEndOkCallback c);
    /**
     * \brief Set the callback for the end of a successful PSCCH RX.
     * \param c The callback
     */
    void SetNrPhyRxPsschEndOkCallback(NrPhyRxPsschEndOkCallback c);
    /**
     * \brief Set the callback for the end of a unsuccessful PSCCH RX.
     * \param c The callback
     */
    void SetNrPhyRxPsschEndErrorCallback(NrPhyRxPsschEndErrorCallback c);
    /**
     * \brief Set the callback for the reception of successful PSFCH
     * \param c The callback
     */
    void SetNrPhyRxSlPsfchCallback(NrPhyRxSlPsfchCallback c);
    /**
     * \brief Add sidelink expected Transport Block (TB)
     * \param expectedTb the expected TB
     * \param dstL2Id The destination L2 id
     */
    void AddSlExpectedTb(ExpectedTb expectedTb, uint16_t dstL2Id);

    /**
     * \brief Clear the buffer of NR SL expected transport block
     */
    void ClearExpectedSlTb();

    /**
     * The type of event related to the SL channels
     */
    enum class SlEventType : uint8_t
    {
        PSCCH_TX = 0,
        PSCCH_RX_OK,
        PSCCH_RX_DECODE_FAILURE,
        PSCCH_RX_HALF_DUPLEX,
        PSSCH_TX,
        PSSCH_RX_OK,
        PSSCH_RX_DECODE_FAILURE_SCI2,
        PSSCH_RX_DECODE_FAILURE_TB,
        PSSCH_RX_HALF_DUPLEX,
        PSSCH_RX_NOT_EXPECTED,
        PSSCH_RX_ALREADY_DECODED
    };

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;
    /**
     * \brief DoInitialize method inherited from Object
     */
    void DoInitialize() override;

  private:
    /**
     * Tuple of SINR average and minimum
     */
    struct SinrStats
    {
        double sinrAvg{0.0}; //!< Average SINR
        double sinrMin{0.0}; //!< Minimum SINR
    };

    /**
     * \brief Function that is called this SpectrumPhy receives a NR Sidelink
     *        signal from the channel.
     * \param params holds NR Sidelink frame signal parameters structure
     */
    void StartRxSlFrame(Ptr<NrSpectrumSignalParametersSlFrame> params);
    /**
     * \brief End receive Sidelink frame function
     */
    void EndRxSlFrame();
    /**
     * \brief Function to process received PSCCH signals/messages
     * \param paramIndexes Indexes of received PSCCH signals/messages parameters
     */
    void RxSlPscch(std::vector<uint32_t> paramIndexes);
    /**
     * \brief Function to process received PSSCH signals/messages function
     * \param paramIndexes Indexes of received PSSCH signals/messages parameters
     */
    void RxSlPssch(std::vector<uint32_t> paramIndexes);
    /**
     * \brief Function to process received PSFCH signals/messages function
     * \param paramIndexes Indexes of received PSFCH signals/messages parameters
     */
    void RxSlPsfch(std::vector<uint32_t> paramIndexes);
    /**
     * \brief Get SINR stats function
     *
     * This method computes an average SINR and a minimum SINR among the RBs in
     * linear scale
     *
     * \param sinr The SINR values
     * \param rbBitMap The vector whose size is equal to the number active RBs
     * \return The SINR stats
     */
    const SinrStats GetSinrStats(const SpectrumValue& sinr, const std::vector<int>& rbBitmap);
    /**
     * \brief Retrieve the SCI stage 2 from the PSSCH packet burst
     * \param pktIndex The index of the packet burst received in \p m_slRxSigParamInfo
     * \return The SCI stage 2 packet
     */
    Ptr<Packet> RetrieveSci2FromPktBurst(uint32_t pktIndex);
    /**
     *
     * \param mcs the MCS of the transmission
     * \param rank the MIMO rank
     * \param nprb The number of physical resource blocks used in the transmission
     * \return the TBS in bytes
     */
    uint32_t CalculateTbSize(uint8_t mcs, uint8_t rank, uint32_t nprb) const;
    TypeId m_slErrorModelType;                    //!< Sidelink Error model default set by attribute
    Ptr<NrSlInterference> m_slInterference;       //!< the Sidelink interference
    std::vector<SpectrumValue> m_slSinrPerceived; //!< SINR for each NR Sidelink packet received
    Ptr<NrSlErrorModel> m_slErrorModel;           //!< Instance of sidelink error model
    std::vector<SpectrumValue> m_slSigPerceived;  //!< PSD for each NR Sidelink packet received
    std::vector<SlRxSigParamInfo>
        m_slRxSigParamInfo; //!< NR Sidelink received signal parameter info
    bool m_dropTbOnRbCollisionEnabled{
        false}; //!< when true, drop all receptions on colliding RBs regardless SINR value.
    bool m_slDataErrorModelEnabled{true}; //!< whether the phy error model for NR Sidelink DATA is
                                          //!< enabled, by default is enabled
    bool m_slCtrlErrorModelEnabled{true}; //!< whether the phy error model for NR Sidelink CTRL is
                                          //!< enabled, by default is enabled
    NrPhyRxPscchEndOkCallback
        m_nrPhyRxPscchEndOkCallback; //!< the callback for the NR SL PHY PSCCH successful reception
    NrPhyRxPsschEndOkCallback
        m_nrPhyRxPsschEndOkCallback; //!< The callback for the NR SL PHY PSSCH successful reception
    NrPhyRxPsschEndErrorCallback m_nrPhyRxPsschEndErrorCallback; //!< The callback for the NR SL PHY
                                                                 //!< PSSCH unsuccessful reception
    NrPhyRxSlPsfchCallback
        m_nrPhyRxSlPsfchCallback; //!< The callback for the NR SL PHY PSFCH successful reception

    NrSlHarqPhy m_slHarqPhy; //!< the HARQ module of this SL spectrum phy instance

    /**
     * \brief typedef for NR SL transport block map per RNTI of TBs which are
     *        expected to be received after successful decoding of SCI stage-1.
     */
    typedef std::unordered_map<uint16_t, std::vector<SlTransportBlockInfo>> SlTransportBlocks;

    SlTransportBlocks m_slTransportBlocks; //!< Map of type SlTransportBlocks

    /**
     * \brief Signature of the trace callback for the SL channel events
     */
    typedef TracedCallback<SlEventType, Ptr<const PacketBurst>> SlEventTypePacketBurstTraceCallback;
    SlEventTypePacketBurstTraceCallback
        m_slChannelsEvent; //!< Trace callback for PSSCH and PSSCH events

    TracedCallback<SlRxCtrlPacketTraceParams>
        m_rxPscchTraceUe; //!< trace source for PSCCH reception
    TracedCallback<SlRxDataPacketTraceParams>
        m_rxPsschTraceUe;                          //!< trace source for PSSCH reception
    TracedValue<uint64_t> m_slPscchDecodeFailures; //!< Count of observed PSCCH decode failures
    TracedValue<uint64_t> m_slSci2aDecodeFailures; //!< Count of observed SCI 2a decode failures
    TracedValue<uint64_t> m_slTbDecodeFailures;    //!< Count of observed TB decode failures

    TracedCallback<Ptr<const NrSpectrumSignalParametersSlCtrlFrame>>
        m_txPscchTrace; //!< trace source for PSCCH transmission
    TracedCallback<Ptr<const NrSpectrumSignalParametersSlDataFrame>>
        m_txPsschTrace; //!< trace source for PSSCH transmission

    NrPhySlHarqFeedbackCallback
        m_phySlHarqFeedbackCallback; //!< callback that is notified when the SL HARQ feedback is
                                     //!< being generated
    TracedCallback<Time>
        m_txFeedbackTrace; //!< trace callback that is notifying when this spectrum phy starts to
                           //!< occupy the channel with transmission of HARQ feedback
};

} // namespace ns3

#endif /* NR_SL_SPECTRUM_PHY_H */
