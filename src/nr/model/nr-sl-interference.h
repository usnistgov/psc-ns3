/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * This NrSlInterference, authored by NIST
 * is derived from LteInterference originally authored by Nicola Baldo <nbaldo@cttc.es>.
 *
 */
#ifndef NR_SL_INTERFERENCE_H
#define NR_SL_INTERFERENCE_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ns3/spectrum-value.h>

#include <list>

namespace ns3 {

class NrSlChunkProcessor;


/**
 * This class implements a Gaussian interference model, i.e., all
 * incoming signals are added to the total interference.
 *
 */
class NrSlInterference : public Object
{
public:
  NrSlInterference ();
  virtual ~NrSlInterference ();

  /**
   * \brief Get the type ID.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  /**
   * Add a NrSlChunkProcessor that will use the time-vs-frequency SINR
   * calculated by this NrSlInterference instance. Note that all the
   * added NrSlChunkProcessors will work in parallel.
   *
   * \param p The chunk processor for SINR evaluation
   */
  void AddSinrChunkProcessor (Ptr<NrSlChunkProcessor> p);

  /**
   * Add a NrSlChunkProcessor that will use the time-vs-frequency
   * interference calculated by this NrSlInterference instance. Note
   * that all the added NrSlChunkProcessors will work in parallel.
   *
   * \param p The chunk processor for interference evaluation
   */
  void AddInterferenceChunkProcessor (Ptr<NrSlChunkProcessor> p);

  /**
   * Add a NrSlChunkProcessor that will use the time-vs-frequency
   * power calculated by this NrSlInterference instance. Note
   * that all the added NrSlChunkProcessors will work in parallel.
   *
   * \param p The chunk processor for Reference Signal (RS) power evaluation
   */
  void AddRsPowerChunkProcessor (Ptr<NrSlChunkProcessor> p);

  /**
   * notify an NrSlChunkProcessor that the PHY is starting an RX attempt
   *
   * \param rxPsd The power spectral density of the signal being RX
   */
  void StartRx (Ptr<const SpectrumValue> rxPsd);


  /**
   * notify that the RX attempt has ended. The receiving PHY must call
   * this method when RX ends or RX is aborted.
   *
   */
  void EndRx ();


  /**
   * notify that a new signal is being perceived in the medium. This
   * method is to be called for all incoming signal, regardless of
   * whether they are useful signals or interference.
   *
   * \param spd The power spectral density of the new signal
   * \param duration The duration of the new signal
   */
  void AddSignal (Ptr<const SpectrumValue> spd, const Time duration);


  /**
   *
   * \param noisePsd the Noise Power Spectral Density in power units
   * (Watt, Pascal...) per Hz.
   */
  void SetNoisePowerSpectralDensity (Ptr<const SpectrumValue> noisePsd);

private:
  /**
   * Conditionally evaluate chunk
   */
  void ConditionallyEvaluateChunk ();
  /**
   * Add signal function
   *
   * \param spd The power spectral density of the new signal
   */
  void DoAddSignal  (Ptr<const SpectrumValue> spd);
  /**
   * Subtract signal
   *
   * \param spd The power spectral density of the new signal
   * \param signalId The signal ID
   */
  void DoSubtractSignal  (Ptr<const SpectrumValue> spd, uint32_t signalId);



  bool m_receiving; ///< are we receiving?

  std::vector <Ptr<SpectrumValue> > m_rxSignal; /**< stores the power spectral density of
                                                 * the signal whose RX is being
                                                 * attempted
                                                 */

  Ptr<SpectrumValue> m_allSignals; /**< stores the spectral
                                    * power density of the sum of incoming signals;
                                    * does not include noise, includes the SPD of the signal being RX
                                    */

  Ptr<const SpectrumValue> m_noise; ///< the noise value

  Time m_lastChangeTime;     /**< the time of the last change in
                                m_TotalPower */

  uint32_t m_lastSignalId; ///< the last signal ID
  uint32_t m_lastSignalIdBeforeReset; ///< the last signal ID before reset

  /** all the processor instances that need to be notified whenever
  a new reference signal chunk is calculated */
  std::list<Ptr<NrSlChunkProcessor> > m_rsPowerChunkProcessorList;

  /** all the processor instances that need to be notified whenever
      a new SINR chunk is calculated */
  std::list<Ptr<NrSlChunkProcessor> > m_sinrChunkProcessorList;

  /** all the processor instances that need to be notified whenever
      a new interference chunk is calculated */
  std::list<Ptr<NrSlChunkProcessor> > m_interfChunkProcessorList;


};

} // namespace ns3

#endif /* LTE_INTERFERENCE_H */
