// Copyright (c) 2009, 2010 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>
// Modified by : Marco Miozzo <mmiozzo@cttc.es>
//        (move from CQI to Ctrl and Data SINR Chunk processors)
// Modified by : Piotr Gawlowicz <gawlowicz.p@gmail.com>
//        (removed all Nr***ChunkProcessor implementations
//        and created generic NrChunkProcessor)

#ifndef NR_CHUNK_PROCESSOR_H
#define NR_CHUNK_PROCESSOR_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/ptr.h>

namespace ns3
{

class SpectrumValue;

/// Chunk processor callback typedef
typedef Callback<void, const SpectrumValue&> NrChunkProcessorCallback;

/**
 * This abstract class is used to process the time-vs-frequency
 * SINR/interference/power chunk of a received NR signal
 * which was calculated by the NrInterference object.
 */
class NrChunkProcessor : public SimpleRefCount<NrChunkProcessor>
{
  public:
    NrChunkProcessor();
    virtual ~NrChunkProcessor();

    /**
     * \brief Add callback to list
     *
     * This function adds callback c to list. Each callback pass
     * calculated value to its object and is called in
     * NrChunkProcessor::End().
     *
     * \param c callback function
     */
    virtual void AddCallback(NrChunkProcessorCallback c);

    /**
     * \brief Clear internal variables
     *
     * This function clears internal variables in the beginning of
     * calculation
     */
    virtual void Start();

    /**
     * \brief Collect SpectrumValue and duration of signal
     *
     * Passed values are collected in m_sumValues and m_totDuration variables.
     *
     * \param sinr the SINR
     * \param duration the duration
     */
    virtual void EvaluateChunk(const SpectrumValue& sinr, Time duration);

    /**
     * \brief Finish calculation and inform interested objects about calculated value
     *
     * During this function all callbacks from list are executed
     * to inform interested object about calculated value. This
     * function is called at the end of calculation.
     */
    virtual void End();

  private:
    Ptr<SpectrumValue> m_sumValues; ///< sum values
    Time m_totDuration;             ///< total duration

    std::vector<NrChunkProcessorCallback> m_nrChunkProcessorCallbacks; ///< chunk processor callback
};

/**
 * A sink to be plugged to the callback of NrChunkProcessor allowing
 * to save and later retrieve the latest reported value
 *
 */
class NrSpectrumValueCatcher
{
  public:
    /**
     * function to be plugged to NrChunkProcessor::AddCallback ()
     *
     * \param value
     */
    void ReportValue(const SpectrumValue& value);

    /**
     *
     *
     * \return the latest value reported by the NrChunkProcessor
     */
    Ptr<SpectrumValue> GetValue();

  private:
    Ptr<SpectrumValue> m_value; ///< spectrum value
};

} // namespace ns3

#endif /* NR_CHUNK_PROCESSOR_H */
