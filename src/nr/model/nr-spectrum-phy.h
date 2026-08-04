// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SPECTRUM_PHY_H
#define NR_SPECTRUM_PHY_H

#include "beam-manager.h"
#include "nr-control-messages.h"
#include "nr-harq-phy.h"
#include "nr-interference.h"
#include "nr-phy.h"
#include "nr-spectrum-signal-parameters.h"

#include <ns3/matrix-based-channel-model.h>
#include <ns3/net-device.h>
#include <ns3/random-variable-stream.h>
#include <ns3/spectrum-channel.h>
#include <ns3/traced-callback.h>

#include <functional>

namespace ns3
{

class UniformPlanarArray;

/**
 * \ingroup ue-phy
 * \ingroup gnb-phy
 * \ingroup spectrum
 *
 * \brief Interface between the physical layer and the channel
 *
 * \section spectrum_phy_general General information
 *
 * NrSpectrumPhy models some of the basic
 * physical layer functionalities such as transmitting CTRL or DATA,
 * receiving the signals, decoding them, and distinguishing whether the
 * signal is useful (CTRL, DATA), i.e. sent to this NR device NrPhy
 * instance, or it should be considered as interference. It
 * is also responsible for obtaining measurements, and for HARQ feedback
 * generation by leveraging HARQ module.
 *
 * This class implements the interface between the NrPhy
 * and the spectrum channel, and provides to NrPhy aforementioned
 * functionalities. Each NR device's NrPhy has its
 * own NrSpectrumPhy that is in charge of providing these basic
 * phy layer functionalities. In order to be able to receive signals from the channel,
 * each NrSpectrumPhy should be registered to listen events from its channel.
 * To achieve that, during the configuration of NR device's NrPhy
 * at some point should be called AddRx function of the spectrum channel
 * to register the NrSpectrumPhy instance as a receiver on that channel.
 *
 * This class also has the interface with NrInterference class to
 * pass the necessary information for the interference calculation and to
 * obtain the interference calculation results.
 *
 * Also it has interface with HARQ module, to which it passes necessary
 * information for the HARQ feedback generation, which is then forwarded
 * to NrPhy.
 *
 * \section spectrum_phy_configuration Configuration
 *
 * The user can configure the class using the method NrHelper::SetGnbSpectrumAttribute(),
 * or NrHelper::SetUeSpectrumAttribute(), depending on the type of user
 * you want to configure, or by directly calling `SetAttribute` on the pointer.
 * The list of  attributes is reported below, in the Attributes section.
 */
class NrSpectrumPhy : public SpectrumPhy
{
  public:
    /**
     * \brief Get the object TypeId
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSpectrumPhy constructor
     */
    NrSpectrumPhy();

    /**
     * \brief ~NrSpectrumPhy
     */
    ~NrSpectrumPhy() override;

    /**
     * \brief Enum that defines possible states of the spectrum phy
     */
    enum State
    {
        IDLE = 0,   //!< IDLE state (no action in progress)
        TX,         //!< Transmitting state (data or ctrl)
        RX_DATA,    //!< Receiving data
        RX_DL_CTRL, //!< Receiving DL CTRL
        RX_UL_CTRL, //!< Receiving UL CTRL
        RX_UL_SRS,  //!< Receiving SRS
        CCA_BUSY    //!< BUSY state (channel occupied by another entity)
    };

    // callbacks typedefs and setters
    /**
     * \brief This callback method type is used to notify that DATA is received
     */
    typedef Callback<void, const Ptr<Packet>&> NrPhyRxDataEndOkCallback;
    /**
     * \brief This callback method type is used to notify that CTRL is received
     */
    typedef std::function<void(const std::list<Ptr<NrControlMessage>>&, uint8_t)>
        NrPhyRxCtrlEndOkCallback;

    /**
     * This method is used by the NrSpectrumPhy to notify the UE PHY that a
     * PSS has been received
     */
    typedef Callback<void, uint16_t, const Ptr<SpectrumValue>&> NrPhyRxPssCallback;

    /**
     * This callback method type is used by the NrSpectrumPhy to notify the PHY about
     * the status of a DL HARQ feedback
     */
    typedef Callback<void, const DlHarqInfo&> NrPhyDlHarqFeedbackCallback;
    /**
     * This callback method type is used by the NrSpectrumPhy to notify the PHY about
     * the status of a UL HARQ feedback
     */
    typedef Callback<void, const UlHarqInfo&> NrPhyUlHarqFeedbackCallback;

    /**
     * \brief Sets the callback to be called when DATA is received successfully
     * \param c the callback function
     */
    void SetPhyRxDataEndOkCallback(const NrPhyRxDataEndOkCallback& c);
    /**
     * \brief Sets the callback to be called when CTRL is received successfully
     * \param c the callback function
     */
    void SetPhyRxCtrlEndOkCallback(const NrPhyRxCtrlEndOkCallback& c);

    /**
     * set the callback for the reception of the PSS as part
     * of the interconnections between the NrSpectrumPhy and the UE PHY
     *
     * \param c the callback
     */
    void SetPhyRxPssCallback(const NrPhyRxPssCallback& c);

    /**
     * \brief Sets the callback to be called when DL HARQ feedback is generated
     */
    void SetPhyDlHarqFeedbackCallback(const NrPhyDlHarqFeedbackCallback& c);
    /**
     * \brief Sets the callback to be called when UL HARQ feedback is generated
     */
    void SetPhyUlHarqFeedbackCallback(const NrPhyUlHarqFeedbackCallback& c);

    // Methods inherited from spectrum phy
    void SetDevice(Ptr<NetDevice> d) override;
    Ptr<NetDevice> GetDevice() const override;
    void SetMobility(Ptr<MobilityModel> m) override;
    Ptr<MobilityModel> GetMobility() const override;
    void SetChannel(Ptr<SpectrumChannel> c) override;
    Ptr<const SpectrumModel> GetRxSpectrumModel() const override;
    /*
     * \brief Sets the beam manager of this spectrum phy, and that beam manager
     * is responsible of the antena array of this spectrum phy
     */
    void SetBeamManager(Ptr<BeamManager> b);

    /*
     * \brief Gets the beam manager of this spectrum phy.
     */
    Ptr<BeamManager> GetBeamManager();

    /*
     * \brief Sets the error model of this spectrum phy, overriding the default
     * that is created by the attribute NrSpectrumPhy::ErrorModelType
     *
     * \param em The error model
     */
    void SetErrorModel(Ptr<NrErrorModel> em);

    /*
     * \brief Gets a pointer to the error model (if instantiated)
     * \return Pointer to the error model
     */
    Ptr<NrErrorModel> GetErrorModel() const;

    /*
     * \brief Gets a pointer to the NrPhy instance
     * \return Pointer to the NrPhy instance
     */
    Ptr<NrPhy> GetNrPhy() const;

    /*
     * \brief Get the time of the most recent start of reception
     * \return The time value of the most recent start of reception
     */
    Time GetFirstRxStart() const;

    /*
     * \brief Set the time of the most recent start of reception
     * \param startTime The time value of the most recent start of reception
     */
    void SetFirstRxStart(Time startTime);

    /*
     * \brief Get the duration of the most recent start of reception
     * \return The time value of the duration of the most recent start of reception
     */
    Time GetFirstRxDuration() const;

    /*
     * \brief Set the duration of the most recent start of reception
     * \param duration The time value of the duration of the most recent start of reception
     */
    void SetFirstRxDuration(Time duration);

    /**
     * \brief Inherited from SpectrumPhy
     * Note: Implements GetRxAntenna function from SpectrumPhy.
     * \return Antenna of this NrSpectrumPhy
     */
    Ptr<Object> GetAntenna() const override;

    /*
     * \brief Used to enable generation and triggering of DL DATA pathloss trace
     */
    void EnableDlDataPathlossTrace();

    /*
     * \brief Used to enable generation and triggering of DL CTRL pahtloss trace
     */
    void EnableDlCtrlPathlossTrace();

    /**
     * \brief Inherited from SpectrumPhy. When this function is called
     * this spectrum phy starts receiving a signal from its spectrum channel.
     * \param params SpectrumSignalParameters object that will be used to process this signal
     */
    void StartRx(Ptr<SpectrumSignalParameters> params) override;

    /**
     * Set RNTI of this spectrum phy
     * \param rnti RNTI to be set
     */
    void SetRnti(uint16_t rnti);

    /**
     * Retrieve RNTI of this spectrum phy
     * \return RNTI
     */
    uint16_t GetRnti() const;

    // Attributes setters
    /**
     * \brief Set clear channel assessment (CCA) threshold
     * \param thresholdDBm - CCA threshold in dBms
     */
    void SetCcaMode1Threshold(double thresholdDBm);
    /**
     * Returns clear channel assessment (CCA) threshold
     * \return CCA threshold in dBms
     */
    double GetCcaMode1Threshold() const;
    /**
     * \brief Sets whether to perform in unlicensed mode in which the channel monitoring is enabled
     * \param unlicensedMode if true the unlicensed mode is enabled
     */
    void SetUnlicensedMode(bool unlicensedMode);
    /**
     * \brief Enables or disabled data error model
     * \param dataErrorModelEnabled boolean saying whether the data error model should be enabled
     */
    void SetDataErrorModelEnabled(bool dataErrorModelEnabled);
    /**
     * \brief Sets the error model type
     */
    void SetErrorModelType(TypeId errorModelType);

    // other methods
    /**
     * \brief Sets noise power spectral density to be used by this device
     * \param noisePsd SpectrumValue object holding noise PSD
     */
    virtual void SetNoisePowerSpectralDensity(const Ptr<const SpectrumValue>& noisePsd);
    /**
     * \brief Sets transmit power spectral density
     * \param txPsd transmit power spectral density to be used for the upcoming transmissions by
     * this spectrum phy
     */
    void SetTxPowerSpectralDensity(const Ptr<SpectrumValue>& txPsd);
    /*
     * \brief Returns a const pointer to the TX PSD
     * \return the TX PSD
     */
    Ptr<const SpectrumValue> GetTxPowerSpectralDensity();
    /**
     * \brief Starts transmission of data frames on connected spectrum channel object
     * \param pb packet burst to be transmitted
     * \param ctrlMsgList control message list
     * \param dci downlink control information
     * \param duration the duration of transmission
     */
    void StartTxDataFrames(const Ptr<PacketBurst>& pb,
                           const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                           const std::shared_ptr<DciInfoElementTdma> dci,
                           const Time& duration);

    /**
     * \brief Return true if the current Phy State is TX
     */
    bool IsTransmitting();

    /**
     * \brief Starts transmission of DL CTRL
     * \param duration the duration of this transmission
     */
    void StartTxDlControlFrames(const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                                const Time& duration); // control frames from Gnb to ue

    /**
     * Start transmission of CSI-RS
     * \param rnti the rnti of the user towards which is directed the CSI-RS
     * \param beamId the ID of the beam that is used to transmit this CSI-RS
     */
    void StartTxCsiRs(uint16_t rnti, uint16_t beamId);

    /**
     * \brief Start transmission of UL CTRL
     * \param ctrlMsgList the list of control messages to be transmitted in UL
     * \param duration the duration of the CTRL messages transmission
     */
    void StartTxUlControlFrames(const std::list<Ptr<NrControlMessage>>& ctrlMsgList,
                                const Time& duration);
    /**
     * \brief Adds the chunk processor that will process the power for the data
     * \param p the chunk processor
     */
    void AddDataPowerChunkProcessor(const Ptr<NrChunkProcessor>& p);
    /**
     * \brief Adds the chunk processor that will process the interference
     * \param p the chunk processor
     */
    void AddDataSinrChunkProcessor(const Ptr<NrChunkProcessor>& p);

    /*
     * \brief Adds the chunk processor that will process the interference for SRS signals at gNBs
     * \param p the chunk processor
     */
    void AddSrsSinrChunkProcessor(const Ptr<NrChunkProcessor>& p);

    /**
     * \brief Adds the chunk processor that will process the received power
     * \param p the chunk processor
     */
    void AddRsPowerChunkProcessor(const Ptr<NrChunkProcessor>& p);

    /**
     * \brief Adds the chunk processor that will process the received power
     * \param p the chunk processor
     */
    void AddDlCtrlSinrChunkProcessor(const Ptr<NrChunkProcessor>& p);
    /**
     * \brief SpectrumPhy that will be called when the SINR for the received
     * DATA is being calculated by the interference object over DATA chunk
     * processor
     * \param sinr the resulting SINR spectrum value
     */
    void UpdateSinrPerceived(const SpectrumValue& sinr);

    /**
     * \brief Called when DlCtrlSinr is fired
     * \param sinr the sinr PSD
     */
    void ReportDlCtrlSinr(const SpectrumValue& sinr);

    /**
     * \brief SpectrumPhy that will be called when the SINR for the received
     * SRS at gNB is being calculated by the interference object over SRS chunk
     * processor
     * \param srsSinr the resulting SRS SINR spectrum value
     */
    void UpdateSrsSinrPerceived(const SpectrumValue& srsSinr);
    /**
     * \brief SpectrumPhy that will be called when the SNR for the received
     * SRS at gNB is being calculated
     * \param srsSnr the resulting SRS SNR
     */
    void UpdateSrsSnrPerceived(const double srsSnr);
    /**
     * \brief Set NrPhy of this spectrum phy in order to be able
     * to obtain information such as cellId, bwpId, etc.
     */
    void InstallPhy(const Ptr<NrPhy>& phyModel);

    Ptr<NrPhy> GetPhy() const;
    /**
     * \brief Sets the antenna of this NrSpectrumPhy instance,
     * currently in NR module it is expected to be of type UniformPlannarArray
     * \param antenna the antenna to be set to this NrSpectrumPhy instance
     */
    void SetAntenna(Ptr<Object> antenna);
    /**
     * \brief Returns spectrum channel object to which is attached this spectrum phy instance
     */
    Ptr<SpectrumChannel> GetSpectrumChannel() const;
    /**
     * \return NrInterference instance of this spectrum phy
     */
    Ptr<NrInterference> GetNrInterference() const;
    /**
     * \brief Instruct the Spectrum Model of a incoming transmission.
     * \param expectedTb Expected transport block
     */
    void AddExpectedTb(ExpectedTb expectedTb);

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

    /**
     * \brief TracedCallback signature for RB statistics
     *
     * \param [in] sfnSf SfnSf
     * \param [in] v rxPsd values
     * \param [in] t duration of the reception
     * \param [in] bwpId BWP ID
     * \param [in] cellId Cell ID
     */
    typedef void (*RxDataTracedCallback)(const SfnSf& sfnSf,
                                         Ptr<const SpectrumValue> v,
                                         const Time& t,
                                         uint16_t bwpId,
                                         uint16_t cellId);

    void AddExpectedSrsRnti(uint16_t rnti);
    /**
     * \brief Keeps track of when DL CTRL should finish. Needed for CSI-RS and
     * CSI-IM implementation to be able to schedule CSI-IM just at the beginning
     * of the PDSCH.
     * \param ctrlEndTime Expected time when DL CTRL will end
     */
    void AddExpectedDlCtrlEnd(Time ctrlEndTime);
    /*
     * \brief SRS SINR callback whose input parameters are cellid, rnti, SRS SINR value
     */
    typedef Callback<void, uint16_t, uint16_t, double> SrsSinrReportCallback;
    typedef Callback<void, uint16_t, uint16_t, double> SrsSnrReportCallback;

    /**
     * \brief It adds callback to the list of callbacks that will be notified
     * once SRS is being received
     * \param callback callback to be added to the list of callbacks
     */
    void AddSrsSinrReportCallback(SrsSinrReportCallback callback);
    /**
     * \brief It adds callback to the list of callbacks that will be notified
     * once SRS is being received
     * \param callback callback to be added to the list of callbacks
     */
    void AddSrsSnrReportCallback(SrsSnrReportCallback callback);
    /**
     * \brief Set whether this spectrum PHY belongs to Gnb or UE
     * TODO NrHelper should be declared as friend and this function should be private
     * \param isGnb whether the spectrum PHY belongs to Gnb or UE
     */
    void SetIsGnb(bool isGnb);
    /**
     * \return the cell id
     */
    uint16_t GetCellId() const;
    /**
     * \return the bwp id
     */
    uint16_t GetBwpId() const;
    /**
     * \param [in] sfnSf SfnSf
     * \param [in] cellId
     * \param [in] bwpId
     * \param [in] imsi
     * \param [in] snr
     */
    typedef void (*DataSnrTracedCallback)(const SfnSf& sfnSf,
                                          const uint16_t cellId,
                                          const uint8_t bwpId,
                                          const uint64_t imsi,
                                          const double snr);
    /**
     * \brief Report wideband perceived downlink data SNR
     *
     * \param dlDataSnr the downlink data SNR
     */
    void ReportWbDlDataSnrPerceived(const double dlDataSnr);

    /**
     * \brief Connect DATA chunk processor with the corresponding DATA interference object
     * \param p the DATA chunk processor
     */
    void AddDataMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p);

    /**
     * \brief Connect CSI-RS chunk processor with the corresponding CSI-RS interference object
     * \param p the CSI-RS chunk processor
     */
    void AddCsiRsMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p);

    /**
     * \brief Connect CSI-IM chunk processor with the corresponding CSI-IM interference object
     * \param p the CSI-IM chunk processor
     */
    void AddCsiImMimoChunkProcessor(const Ptr<NrMimoChunkProcessor>& p);

    /// \brief Store the SINR chunks for all received signals at end of interference calculations
    /// \param sinr The vector of all SINR values of receive signals. A new chunk is generated for
    /// each different receive signal (for example for each UL reception of a signal from a
    /// different UE) and at each time instant where the interference changes.
    void UpdateMimoSinrPerceived(const std::vector<MimoSinrChunk>& sinr);
    /**
     * \return true if this class is inside an enb/gnb
     */
    bool IsGnb() const;

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;

    /**
     * \brief Get current state
     * \return current state
     */
    State GetState() const;

    /**
     * \brief Get pointer to SpectrumChannel
     * \return Pointer to spectrum channel
     */
    Ptr<SpectrumChannel> GetChannel() const;

    /**
     * \brief Get pointer to error model random variable
     * \return Pointer to error model random variable
     */
    Ptr<UniformRandomVariable> GetErrorModelRv() const;

    /**
     * \brief Update the state of the spectrum phy. The states are:
     *  IDLE, TX, RX_DATA, RX_DL_CTRL, RX_UL_CTRL, CCA_BUSY.
     * \param newState the new state
     * \param duration how much time the spectrum phy will be in the new state
     */
    void ChangeState(State newState, Time duration);

    /**
     * \brief Function that is called when the transmission has ended. It is
     * used to update spectrum phy state.
     */
    void EndTx();

    /**
     * \brief Increase the counter of active transmissions
     */
    void IncrementActiveTransmissions();

    /**
     * \brief call RxDataTrace from subclass
     * \param sfnSf SfnSf
     * \param spectrumValue rxPsd values
     * \param duration duration of the reception
     * \param bwpId BWP ID
     * \param cellId Cell ID
     */
    void NotifyRxDataTrace(const SfnSf& sfn,
                           Ptr<const SpectrumValue> spectrumValue,
                           const Time& duration,
                           uint16_t bwpId,
                           uint16_t cellId) const;

    /**
     * \brief call TxCtrlTrace from subclass
     * \param duration Duration that the transmitter will occupy channel with control transmission
     */
    void NotifyTxCtrlTrace(Time duration) const;

    /**
     * \brief call TxDataTrace from subclass
     * \param duration Duration that the transmitter will occupy channel with data transmission
     */
    void NotifyTxDataTrace(Time duration) const;

  private:
    std::vector<MimoSinrChunk>
        m_mimoSinrPerceived; //!< received SINR values during data reception for TB decoding, to
                             //!< replace m_sinrPerceived for all (MIMO and SISO) receivers

    /**
     * \brief Function is called when what is being received is holding data
     * \para params spectrum parameters that are holding information regarding data frame
     */
    void StartRxData(const Ptr<NrSpectrumSignalParametersDataFrame>& params);
    /**
     * \brief Function that is called when is being received DL CTRL
     * \param params holds DL CTRL frame signal parameters structure
     */
    void StartRxDlCtrl(const Ptr<NrSpectrumSignalParametersDlCtrlFrame>& params);
    /**
     * \brief Function that is called when is being received UL CTRL
     * \param params holds UL CTRL frame signal parameters structure
     */
    void StartRxUlCtrl(const Ptr<NrSpectrumSignalParametersUlCtrlFrame>& params);
    /**
     * \brief Function that is called when is being received CSI-RS signal
     * \param csiRsParams holds CSI-RS signal parameters
     */
    void StartRxCsiRs(const Ptr<NrSpectrumSignalParametersCsiRs>& csiRsParams);
    /**
     * \brief Function that is called when is being received SRS
     * \param param should hold UL CTRL frame signal parameters containing only
     * one CTRL message which should be of type SRS
     */
    void StartRxSrs(const Ptr<NrSpectrumSignalParametersUlCtrlFrame>& params);
    /**
     * \brief Checks if CSI-IM measurement is needed, if not, then it checks if needed to
     * call directly the generation of CSI feedback
     * \param csiRsParams CSI-RS parameters that will be reused for CSI-IM measurement
     */
    void CheckIfCsiImNeeded(const Ptr<NrSpectrumSignalParametersCsiRs>& csiRsParams);

    /// \brief Filter the received SINR chunks for a particular DL or UL signal
    /// \param rnti The RNTI for the expected receive signal (transmitting or receiving UE)
    /// \param rank The number of MIMO layers of the expected signal
    /// \return the SINR chunks in m_mimoSinrPerceived which match the rnti, or a chunk with an
    /// all-zero SINR matrix when no matching signal is found
    std::vector<MimoSinrChunk> GetMimoSinrForRnti(uint16_t rnti, uint8_t rank);

    /**
     * \brief Function that is called when the spectrum phy finishes the reception of DATA. This
     * function processed the data being received and generated HARQ feedback.
     * It also updates spectrum phy state.
     */
    void EndRxData();
    /**
     * \brief Schedule CsiIm period on the CSI-IM instance of the NrInterference to measure the
     * interference.
     */
    void ScheduleCsiIm(Ptr<SpectrumSignalParameters> csiRsParams) const;
    /**
     * \return Whether this UE has scheduled DL DATA in this slot
     */
    bool IsUeScheduled() const;
    /**
     * \brief Function that is called when the spectrum phy finishes the reception of CTRL.
     * It stores CTRL messages and updates spectrum phy state.
     */
    void EndRxCtrl();
    /**
     * \brief Function that is celled when the spectrum phy finishes the reception of SRS.
     * It stores SRS message, calls the interference calculator to notify the end of the
     * reception which will trigger SRS SINR calculation, and it also updates the spectrum phy
     * state.
     */
    void EndRxSrs();
    /**
     * \brief Check if the channel is busy. If yes, updates the spectrum phy state.
     */
    void MaybeCcaBusy();
    /**
     * \brief Function used to schedule an event to check if state should be switched from CCA_BUSY
     * to IDLE. This function should be used only for this transition of state machine. After
     * finishing reception (RX_DL_CTRL or RX_UL_CTRL or RX_DATA) function MaybeCcaBusy should be
     * called instead to check if to switch to IDLE or CCA_BUSY, and then new event may be created
     * in the case that the channel is BUSY to switch back from busy to idle.
     */
    void CheckIfStillBusy();
    /**
     * \brief Checks whether the CTRL message list contains only SRS control message.
     * Only if the list has only one CTRL message and that message is SRS the function
     * will return true, otherwise it will return false.
     * \ctrlMsgList uplink control message list
     * \returns an indicator whether the ctrlListMessage contains only SRS message
     */
    bool IsOnlySrs(const std::list<Ptr<NrControlMessage>>& ctrlMsgList);

    /**
     * \brief Function that is called when the rx signal does not contain an expected channel
     * matrix.
     * \param params the signal parameters which contains the spectrum channel matrix
     * \returns the channel matrix
     */
    Ptr<MatrixBasedChannelModel::Complex3DVector> CreateSpectrumChannelMatrix(
        const Ptr<SpectrumSignalParameters> params) const;

    /**
     * \brief Checks whether transport blocks were correctly received or were corrupted.
     */
    void CheckTransportBlockCorruptionStatus();

    /**
     * \brief Process received packets bursts
     *
     * Packets are received in case the transport block is not corrupted, being forwarded
     * to the NrPhy. Traces are collected. Harq feedback is then sent.
     */
    void ProcessReceivedPacketBurst();

    /**
     * \brief Send uplink HARQ feedback
     * \param rnti UE RNTI
     * \param tbInfo TransportBlockInfo of corresponding packet burst
     */
    void SendUlHarqFeedback(uint16_t rnti, TransportBlockInfo& tbInfo);

    /**
     * \brief Send downlink HARQ feedback
     * \param rnti UE RNTI
     * \param tbInfo TransportBlockInfo of corresponding packet burst
     * \returns DlHarqInfo structure for bookkeeping
     */
    DlHarqInfo SendDlHarqFeedback(uint16_t rnti, TransportBlockInfo& tbInfo);

    // attributes
    TypeId m_errorModelType{
        Object::GetTypeId()}; //!< Error model type by default is NrLteMiErrorModel
    bool m_dataErrorModelEnabled{
        true}; //!< whether the phy error model for DATA is enabled, by default is enabled
    double m_ccaMode1ThresholdW{0}; //!< Clear channel assessment (CCA) threshold in Watts,
                                    //!< attribute that it configures it is
                                    //   CcaMode1Threshold and is configured in dBm
    bool m_unlicensedMode{
        false}; //!< Whether this spectrum phy is configure to work in an unlicensed mode.
                //   Unlicensed mode additionally to licensed mode allows channel monitoring to
                //   discover if is busy before transmission.

    Ptr<SpectrumChannel> m_channel{
        nullptr}; //!< channel is needed to be able to connect listener spectrum phy (AddRx) or to
                  //!< start transmission StartTx
    Ptr<const SpectrumModel> m_rxSpectrumModel{
        nullptr}; //!< the spectrum model of this spectrum phy
    Ptr<BeamManager> m_beamManager{
        nullptr}; //!< the beam manager corresponding to the antenna of this spectrum phy
    Ptr<MobilityModel> m_mobility{
        nullptr}; //!< the mobility model of the node to which belongs this spectrum phy
    Ptr<NetDevice> m_device{nullptr}; //!< the device to which belongs this spectrum phy
    Ptr<NrPhy> m_phy{nullptr}; //!< a pointer to phy instance to which belongs this spectrum phy
    Ptr<NrErrorModel> m_errorModel{nullptr}; //!< a pointer to the error model instance
    Ptr<Object> m_antenna{nullptr}; //!< antenna object of this NrSpectrumPhy, currently supported
                                    //!< UniformPlannarArray type of antenna
    Ptr<NrInterference> m_interferenceData{nullptr}; //!< the interference object used to calculate
                                                     //!< the interference for this spectrum phy
    Ptr<NrInterference> m_interferenceCtrl{nullptr}; //!< the interference object used to calculate
                                                     //!< the interference for this spectrum phy
    Ptr<NrInterference> m_interferenceSrs{
        nullptr}; //!< the interference object used to calculate the interference for this spectrum
                  //!< phy, exists only at gNB phy
    Ptr<NrInterference> m_interferenceCsiRs{
        nullptr}; //!< the interference object used to obtain the CSI-RS measurements
    Ptr<NrInterference> m_interferenceCsiIm{
        nullptr}; //!< the interference object used to obtain the CSI-IM measurements

    Ptr<SpectrumValue> m_txPsd{nullptr};          //!< tx power spectral density
    Ptr<UniformRandomVariable> m_random{nullptr}; //!< the random variable used for TB decoding

    NrHarqPhy m_harqPhyModule; //!< the HARQ module of this spectrum phy instance

    std::unordered_map<uint16_t, TransportBlockInfo>
        m_transportBlocks; //!< Transport block map per RNTI of TBs which are expected to be
                           //!< received by reading DL or UL DCIs
    std::list<Ptr<PacketBurst>> m_rxPacketBurstList; //!< the list of received packets
    std::list<Ptr<NrControlMessage>>
        m_rxControlMessageList; //!< the list of received control messages

    Time m_firstRxStart{
        Seconds(0)}; //!< this is needed to save the time at which we lock down onto signal
    Time m_firstRxDuration{Seconds(0)}; //!< the duration of the current reception
    State m_state{IDLE};                //!< spectrum phy state
    SpectrumValue m_sinrPerceived; //!< SINR that is being update at the end of the DATA reception
                                   //!< and is used for TB decoding

    uint16_t m_rnti{0};    //!< RNTI; only set if this instance belongs to a UE
    bool m_hasRnti{false}; //!< set to true if m_rnti was set and this instance belongs to a UE
    uint16_t m_activeTransmissions{0}; //!< the counter that is used in EndRx function to know when
                                       //!< to change the state from TX to IDLE mode

    std::list<SrsSinrReportCallback> m_srsSinrReportCallback; //!< list of SRS SINR callbacks
    std::list<SrsSnrReportCallback> m_srsSnrReportCallback;   //!< list of SRS SNR callbacks
    uint16_t m_currentSrsRnti{0};
    Time m_ctrlEndTime;           //!< Needed to schedule the interference measurements CSI-IM
    EventId m_checkIfIsIdleEvent; //!< Event used to check if state should be switched from CCA_BUSY
                                  //!< to IDLE.
    Time m_busyTimeEnds{
        Seconds(0)}; //!< Used to schedule switch from CCA_BUSY to IDLE, this is absolute time

    // callbacks for CTRL and DATA, and UL/DL HARQ
    NrPhyRxCtrlEndOkCallback
        m_phyRxCtrlEndOkCallback; //!< callback that is notified when the CTRL is received
    NrPhyRxDataEndOkCallback
        m_phyRxDataEndOkCallback;          //!< callback that is notified when the DATA is received
    NrPhyRxPssCallback m_phyRxPssCallback; ///< the phy receive PSS callback
    NrPhyDlHarqFeedbackCallback
        m_phyDlHarqFeedbackCallback; //!< callback that is notified when the DL HARQ feedback is
                                     //!< being generated
    NrPhyUlHarqFeedbackCallback
        m_phyUlHarqFeedbackCallback; //!< callback that is notified when the UL HARQ feedback is
                                     //!< being generated
    // traces
    TracedCallback<Time>
        m_channelOccupied; //!< trace callback that is notifying of total time that this spectrum
                           //!< phy sees the channel occupied, by others and by itself
    TracedCallback<Time> m_txDataTrace; //!< trace callback that is notifying when this spectrum phy
                                        //!< starts to occupy the channel with data transmission
    TracedCallback<Time> m_txCtrlTrace; //!< trace callback that is notifying when this spectrum phy
                                        //!< starts to occupy the channel with transmission of CTRL
    TracedCallback<RxPacketTraceParams>
        m_rxPacketTraceGnb; //!< trace callback that is notifying when Gnb received the packet
    TracedCallback<RxPacketTraceParams>
        m_rxPacketTraceUe; //!< trace callback that is notifying when UE received the packet
    TracedCallback<GnbPhyPacketCountParameter>
        m_txPacketTraceGnb; //!< trace callback that is notifying when Gnb transmts the packet
    TracedCallback<const SfnSf&, Ptr<const SpectrumValue>, const Time&, uint16_t, uint16_t>
        m_rxDataTrace;
    TracedCallback<const SfnSf,
                   const uint16_t,
                   const uint8_t,
                   const uint64_t,
                   const double>
        m_dlDataSnrTrace; //!< DL data SNR trace source

    /*
     * \brief Trace source that reports the following: Cell ID, Bwp ID, UE node ID, DL
     * CTRL pathloss
     */
    typedef TracedCallback<uint16_t, uint8_t, uint32_t, double> DlPathlossTrace;
    DlPathlossTrace m_dlCtrlPathlossTrace; //!< DL CTRL pathloss trace
    bool m_enableDlCtrlPathlossTrace =
        false; //!< By default this trace is disabled to not slow done simulations
    /*
     * \brief Trace source that reports the following: Cell ID, Bwp ID, UE node ID, DL
     * CTRL pathloss, CQI that corresponds to the current SINR
     */
    typedef TracedCallback<uint16_t, uint8_t, uint32_t, double, uint8_t> DlDataPathlossTrace;
    DlDataPathlossTrace m_dlDataPathlossTrace; //!< DL DATA pathloss trace
    bool m_enableDlDataPathlossTrace =
        false; //!< By default this trace is disabled to not slow done simulations
    bool m_isGnb = false;
};

} // namespace ns3

#endif /* NR_SPECTRUM_PHY_H */
