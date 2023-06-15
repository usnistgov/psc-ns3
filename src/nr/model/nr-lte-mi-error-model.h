/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
*/

#ifndef NR_MI_ERROR_MODEL_H
#define NR_MI_ERROR_MODEL_H

#include "nr-harq-phy.h"
#include "nr-error-model.h"

namespace ns3 {

/**
 * \ingroup error-models
 * \brief The NrLteMiErrorModelOutput struct
 * Error model output returned by the class NrLteMiErrorModel
 * \see NrLteMiErrorModel
 */
struct NrLteMiErrorModelOutput : public NrErrorModelOutput
{
  /**
   * \brief NrLteMiErrorModelOutput default constructor (deleted)
   */
  NrLteMiErrorModelOutput () = delete;
  /**
   * \brief NrLteMiErrorModelOutput constructor with tbler parameter
   * \param tbler the reference TBler
   */
  NrLteMiErrorModelOutput (double tbler) : NrErrorModelOutput (tbler)
  {
  }
  /**
   * \brief ~NrLteMiErrorModelOutput
   */
  virtual ~NrLteMiErrorModelOutput ()
  {
  }

  double m_mi       {0.0};    //!< Mutual Information
  double m_miTotal  {0.0};    //!< Acumulated Mutual Information
  uint32_t m_infoBits {0};    //!< number of info bits
  uint32_t m_codeBits {0};    //!< number of code bits
};

/**
 * \ingroup error-models
 * This class provides the BLER estimation based on mutual information metrics,
 * assuming TC coding with block segmentation as per TS 36.212 Sect. 5.1.2 and
 * modulation and coding in TS 36.214 including up to 64-QAM. The BLER-SINR
 * curves are obtained from a link level simulator (Vienna) that uses TC coding
 * and said MCSs. In case of HARQ, the model currently follows HARQ with
 * Incremental Redundancy, so that both the ECR and the MI are updated based on
 * IEEE 802.16m-08/004r2.
 */
class NrLteMiErrorModel : public NrErrorModel
{
public:
  /**
   * \brief GetTypeId
   * \return the object type id
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the type ID of this instance
   * \return the Type ID of this instance
   */
  TypeId GetInstanceTypeId (void) const override;

  /**
   * \brief NrLteMiErrorModel constructor
   */
  NrLteMiErrorModel ();

  /**
   * \brief ~NrLteMiErrorModel
   */
  virtual ~NrLteMiErrorModel () override;

  /**
   * \brief Get an output for the decodification error probability of a given
   * transport block, assuming the MIESM method, LTE TC coding and block
   * segmentation, MCSs in LTE, and HARQ based on IR.
   *
   * \param sinr SINR vector
   * \param map RB map
   * \param size Transport block size (bytes)
   * \param mcs MCS
   * \param history History of the retransmission
   * \return A pointer to an output, with the tbler and accumulated MI, effective
   * MI, code bits, and info bits.
   */
  virtual Ptr<NrErrorModelOutput> GetTbDecodificationStats (const SpectrumValue& sinr,
                                                            const std::vector<int>& map,
                                                            uint32_t size, uint8_t mcs,
                                                            const NrErrorModelHistory &history) override;

  /**
   * \brief Get the SE for a given CQI, following the CQIs in LTE
   */
  virtual double GetSpectralEfficiencyForCqi (uint8_t cqi) override;
  /**
   * \brief Get the SE for a given MCS, following the MCSs in LTE
   */
  virtual double GetSpectralEfficiencyForMcs (uint8_t mcs) const override;
  /**
   * \brief Get the payload size, following the MCSs in LTE
   */
  virtual uint32_t GetPayloadSize (uint32_t usefulSC, uint8_t mcs, uint32_t rbNum, Mode mode) const override;
  /**
   * \brief Get the maximum code block size, as per LTE
   */
  virtual uint32_t GetMaxCbSize (uint32_t tbSize, uint8_t mcs) const override;
  virtual uint8_t GetMaxMcs () const override;

private:
  /**
   * \brief Get an output for the decodification error probability of a given
   * transport block, assuming the MIESM method, LTE TC coding and block
   * segmentation, MCSs in LTE, and HARQ based on IR.
   *
   * \param sinr SINR vector
   * \param map RB map
   * \param size Transport block size (bit)
   * \param mcs MCS
   * \param history History of the retransmission
   * \return A pointer to an output, with the tbler and accumulated MI, effective
   * MI, code bits, and info bits.
   */
  virtual Ptr<NrErrorModelOutput> GetTbBitDecodificationStats (const SpectrumValue& sinr,
                                                               const std::vector<int>& map,
                                                               uint32_t size, uint8_t mcs,
                                                               const NrErrorModelHistory &history);

  /**
   * \brief compute the mmib (mean mutual information per bit) for the
   * specified MCS and SINR, according to the MIESM method
   *
   * \param sinr the perceived SINRs in the whole bandwidth
   * \param map the actives RBs for the TB
   * \param mcs the MCS of the TB
   * \return the mmib
   */
  static double Mib (const SpectrumValue& sinr, const std::vector<int>& map, uint8_t mcs);

  /**
   * \brief map the mmib (mean mutual information per bit) into CBLER for
   * the specified MCS and CB size, according to the MIESM method
   *
   * \param mib mean mutual information per bit of a code-block
   * \param ecrId Effective Code Rate ID
   * \param cbSize the size of the CB
   * \return the code block error rate
   */
  static double MappingMiBler (double mib, uint8_t ecrId, uint32_t cbSize);
};


} // namespace ns3

#endif /* NR_MI_ERROR_MODEL_H */
