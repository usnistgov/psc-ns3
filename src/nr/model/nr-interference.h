// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_INTERFERENCE_H
#define NR_INTERFERENCE_H

#include "nr-chunk-processor.h"
#include "nr-interference-base.h"

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/spectrum-signal-parameters.h>
#include <ns3/spectrum-value.h>
#include <ns3/traced-callback.h>
#include <ns3/vector.h>

namespace ns3
{

// Signal ID increment used in LteInterference
static constexpr uint32_t NR_LTE_SIGNALID_INCR = 0x10000000;

class NrCovMat;
class NrSinrMatrix;
class NrErrorModel;
class NrMimoChunkProcessor;

/**
 * \ingroup spectrum
 *
 * \brief The NrInterference class inherits LteInterference which
 * implements a gaussian interference model, i.e., all
 * incoming signals are added to the total interference.
 * NrInterference class extends this functionality to support
 * energy detection functionality.
 *
 */
class NrInterference : public NrInterferenceBase
{
  public:
    /**
     * \brief NrInterference constructor
     */
    NrInterference();
    /**
     * \brief ~NrInterference
     */
    ~NrInterference() override;
    /**
     * \brief Get the object TypeId
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    void AddSignal(Ptr<const SpectrumValue> spd, Time duration) override;

    /**
     * \brief Checks if the sum of the energy, including the energies that start
     * at this moment is greater than provided energy detection threshold.
     * If yes it returns true, otherwise false.
     * \param energyW energy detection threshold used to evaluate if the channel
     * is busy
     * \return Returns true if the energy is above provided threshold. Otherwise
     * false.
     */
    bool IsChannelBusyNow(double energyW);

    /**
     * \brief Returns the duration of the energy that is above the energy
     * provided detection threshold
     * \param energyW energy detection threshold used to evaluate if the channel
     * is busy
     * \return Duration of the energy that is above provided energy detection
     * threshold.
     */
    Time GetEnergyDuration(double energyW);

    /**
     * \brief Crates events corresponding to the new energy. One event corresponds
     * to the moment when the energy starts, and another to the moment that energy
     * ends and in that event the energy is negative, or it is being subtracted.
     * This function also updates the list of events, i.e. it removed the events
     * belonging to the signals that have finished.
     * \param startTime Energy start time
     * \param endTime Energy end time
     * \param rxPowerW Power of the energy in Watts
     */
    void AppendEvent(Time startTime, Time endTime, double rxPowerW);
    /**
     * Erase all events.
     */
    void EraseEvents();

    // inherited from LteInterference
    void EndRx() override;

    /// \brief Notify that a new signal is being perceived in the medium.
    /// This method is to be called for all incoming signals, including interference.
    /// This method handles MIMO signals and also calls LteInterference to cover SISO signals.
    /// \param params The spectrum signal parameters of the new signal
    /// \param duration The duration of the new signal
    virtual void AddSignalMimo(Ptr<const SpectrumSignalParameters> params, const Time& duration);

    /// \brief Notify the intended receiver that a new signal is being received.
    /// This method is to be called only for the useful signal-of-interest.
    /// This method handles MIMO signals and also calls LteInterference to cover SISO signals.
    /// \param params The spectrum signal parameters of the new signal
    virtual void StartRxMimo(Ptr<const SpectrumSignalParameters> params);

    /// \brief Notify that a signals transmission is ending.
    /// This means that the signal will be removed from the lists of RX and interfering signals.
    /// This method handles MIMO signals and also calls LteInterference to cover SISO signals.
    /// \param params The spectrum signal parameters of the ending signal
    /// \param signalId The LteInterference signalId
    virtual void DoSubtractSignalMimo(Ptr<const SpectrumSignalParameters> params,
                                      uint32_t signalId);

    /// \brief Add a chunk processor for MIMO signals
    /// \param cp The NrMimoChunkProcessor to be added
    virtual void AddMimoChunkProcessor(Ptr<NrMimoChunkProcessor> cp);

    /**
     * @return Returns a flag that indicates whether at least one chunk processor is set. Returns
     * true if chunk processor list is not empty.
     */
    bool IsChunkProcessorSet();

  private:
    /// \brief Calculate interference-plus-noise covariance matrix for signals not in m_rxSignals
    /// This function computes the interference signals from all out-of-cell interferers. The
    /// intra-cell interference signals that are part of m_rxSignals are skipped.
    /// \return the interference+noise covariance matrix for out-of-cell interference
    NrCovMat CalcOutOfCellInterfCov() const;

    /// \brief Add the remaining interference to the interference-and-noise covariance matrix
    /// This function is required for MU-MIMO UL, where the signal from a different UE within the
    /// same cell can act as interference towards the current signal.
    /// \param rxSignal the parameters of the received signal-of-interest
    /// \param outOfCellInterfCov the covariance matrix of out-of-cell signals, plus noise
    /// \return the interference+noise covariance matrix for the current signal
    NrCovMat CalcCurrInterfCov(Ptr<const SpectrumSignalParameters> rxSignal,
                               const NrCovMat& outOfCellInterfCov) const;

    /// \brief Add the covariance of the signal to an existing covariance matrix
    /// \param covMat the existing interference-and-noise covariance matrix
    /// \param signal the signal to be added
    void AddInterference(NrCovMat& covMat, Ptr<const SpectrumSignalParameters> signal) const;

    /// \brief Compute the SINR of the current receive signal
    /// \param outOfCellInterfCov the covariance matrix of out-of-cell signals, plus noise
    /// \param rxSignal the receive signal
    /// \return the SINR of the receive signal
    NrSinrMatrix ComputeSinr(NrCovMat& outOfCellInterfCov,
                             Ptr<const SpectrumSignalParameters> rxSignal) const;

    /// Stores the params of all incoming signals, including the interference signals
    std::vector<Ptr<const SpectrumSignalParameters>> m_allSignalsMimo;

    /// Stores the params of all incoming signals intended for this receiver
    std::vector<Ptr<const SpectrumSignalParameters>> m_rxSignalsMimo;

    /// The processor instances that are notified whenever a new interference chunk is calculated
    std::list<Ptr<NrMimoChunkProcessor>> m_mimoChunkProcessors;

    /**
     * Noise and Interference (thus Ni) event.
     */
    class NiChange
    {
      public:
        /**
         * Create a NiChange at the given time and the amount of NI change.
         *
         * \param time time of the event
         * \param delta the power
         */
        NiChange(Time time, double delta);
        /**
         * Return the event time.
         *
         * \return the event time.
         */
        Time GetTime() const;
        /**
         * Return the power
         *
         * \return the power
         */
        double GetDelta() const;
        /**
         * Compare the event time of two NiChange objects (a < o).
         *
         * \param o
         * \return true if a < o.time, false otherwise
         */
        bool operator<(const NiChange& o) const;

      private:
        Time m_time;
        double m_delta;
    };

    /**
     * typedef for a vector of NiChanges
     */
    typedef std::vector<NiChange> NiChanges;

    /**
     * \brief Find a position in event list that corresponds to a given
     * moment. Note that all events are saved when they start and when
     * they end. When they start, the energy the signal brings is saved as
     * the positive value, and the event when the energy finish is
     * saved with a negative prefix. By using this position, one
     * can know which signals have finished, and can be removed from
     * the list because after the given moment they do not contribute
     * anymore to the total energy received.
     */
    NrInterference::NiChanges::iterator GetPosition(Time moment);

    // inherited from LteInterference
    void ConditionallyEvaluateChunk() override;

    /**
     * Add NiChange to the list at the appropriate position.
     * \param change
     */
    void AddNiChangeEvent(NiChange change);

  protected:
    /**
     * \brief DoDispose method inherited from Object
     */
    void DoDispose() override;

    TracedCallback<double> m_snrPerProcessedChunk;  ///<! Trace for SNR per processed chunk.
    TracedCallback<double> m_rssiPerProcessedChunk; ///<! Trace for RSSI pre processed chunk.

    /// Used for energy duration calculation, inspired by wifi/model/interference-helper
    /// implementation
    NiChanges m_niChanges; //!< List of events in which there is some change in the energy
    double m_firstPower;   //!< This contains the accumulated sum of the energy events until the
                           //!< certain moment it has been calculated
};

} // namespace ns3

#endif /* NR_INTERFERENCE_H */
