/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009, 2010 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by : Marco Miozzo <mmiozzo@cttc.es>
 *        (move from CQI to Ctrl and Data SINR Chunk processors)
 * Modified by : Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *        (removed all Lte***ChunkProcessor implementations
 *        and created generic NistLteChunkProcessor)
 * Modified by: NIST
 */

#ifndef NIST_LTE_SL_CHUNK_PROCESSOR_H
#define NIST_LTE_SL_CHUNK_PROCESSOR_H

#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/object.h>

namespace ns3 {

class SpectrumValue;

  /**
   * Defines callback function for receiving vector of spectral densities for messages received
   */
  typedef Callback< void, std::vector<SpectrumValue> > NistLteSlChunkProcessorCallback;


/** 
 * This abstract class is used to process the time-vs-frequency
 * SINR/interference/power chunk of a received LTE signal
 * which was calculated by the NistLteInterference object.
 */
class NistLteSlChunkProcessor : public SimpleRefCount<NistLteSlChunkProcessor>
{
public:

  /** Stores the received spectral power and duration of the received signals */
struct NistLteSlChunkValue
{
  Ptr<SpectrumValue> m_sumValues; //!< received spectral density
  Time m_totDuration; //!< duration of the signal received
};

  NistLteSlChunkProcessor ();
  virtual ~NistLteSlChunkProcessor ();

  /**
    * \brief Add callback to list
    *
    * This function adds callback c to list. Each callback pass
    * calculated value to its object and is called in
    * NistLteSlChunkProcessor::End().
    * \param c The callback to add
    */
  virtual void AddCallback (NistLteSlChunkProcessorCallback c);

  /**
    * \brief Clear internal variables
    *
    * This function clears internal variables in the beginning of
    * calculation
    * \param init argument indicates if we need to reset all variables
    */
  virtual void Start (bool init);

  /**
    * \brief Collect SpectrumValue and duration of signal
    *
    * Passed values are collected in m_sumValues and m_totDuration variables.
    * \param index The index of the message received
    * \param sinr The sinr of the message received
    * \param duration The duration of the reception
    */
  virtual void EvaluateChunk (uint32_t index, const SpectrumValue& sinr, Time duration);

  /**
    * \brief Finish calculation and inform interested objects about calculated value
    *
    * During this function all callbacks from list are executed
    * to inform interested object about calculated value. This
    * function is called at the end of calculation.
    */
  virtual void End ();

private:
  //Ptr<SpectrumValue> m_sumValues;
  //Time m_totDuration;
  std::vector<NistLteSlChunkValue> m_chunkValues;

  std::vector<NistLteSlChunkProcessorCallback> m_lteChunkProcessorCallbacks;
};

} // namespace ns3



#endif /* NIST_LTE_SL_CHUNK_PROCESSOR_H */
