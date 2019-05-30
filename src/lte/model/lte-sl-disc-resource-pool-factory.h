/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in 
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef LTE_SL_DISC_RESOURCE_POOL_FACTORY_H
#define LTE_SL_DISC_RESOURCE_POOL_FACTORY_H

#include "lte-rrc-sap.h"
#include "lte-sl-pool-factory.h"

namespace ns3 {

/**
 * Sidelink discovery resource pool factory
 */
class LteSlDiscResourcePoolFactory : public LteSlPoolFactory
{
public:
  /**
   * Create a discovery resource pool
   */
  LteSlDiscResourcePoolFactory ();

  /**
   * Set the discovery resource pool
   * \return The Sidelink discovery pool
   */
  LteRrcSap::SlDiscResourcePool CreatePool ();

  /**
   * Set cyclic prefix length, i.e., NORMAL or EXTENDED
   * \param cpLen The cyclic prefix length
   */
  void SetDiscCpLen (std::string cpLen);

  /**
   * Set the discovery period
   * \param period The discovery period
   */
  void SetDiscPeriod (std::string period);

  /**
   * Set the number of retransmissions of the discovery message
   * \param numRetx The number of retransmissions of the discovery message
   */
  void SetNumRetx (int8_t numRetx);

  /**
   * Set the number of repetition of the bitmap within the discovery period
   * \param numRep The number of repetition of the bitmap within the discovery period
   */
  void SetNumRepetition (int32_t numRep);

  /**
   * Set the number of resource blocks to be used for discovery
   * \param prbNum The number of resource blocks to be used for discovery
   */
  void SetDiscPrbNum (int32_t prbNum);

  /**
   * Set the lower bound of resource blocks to be used for discovery
   * \param prbStart The lower bound of resource blocks to be used for discovery
   */
  void SetDiscPrbStart (int32_t prbStart);

  /**
   * Set the upper bound of resource blocks to be used for discovery
   * \param prbEnd The upper bound of resource blocks to be used for discovery
   */
  void SetDiscPrbEnd (int32_t prbEnd);

  /**
   * Set the offset to start the discovery
   * \param offset The offset to start the discovery
   */
  void SetDiscOffset (int32_t offset);

  /**
   * Set the discovery bitmap
   * \param value The discovery bitmap
   */
  void SetDiscBitmap (int64_t value);

  /**
   * Set the tx parameters for discovery
   * \param txParam The tx parameters for discovery
   */
  void SetHaveTxParameters (bool txParam);

  /**
   * Set the alpha
   * \param alpha The alpha value
   */
  void SetDiscTxAlpha (std::string alpha);

  /**
   * Set the P0
   * \param p0 The P0 value
   */
  void SetDiscTxP0 (int32_t p0);

  /**
   * Set the pool selection criteria for the discovery
   * \param poolSelection The pool selection criteria: RSRPBASED or RANDOM
   */
  void SetDiscPoolSelection (std::string poolSelection);

  /**
   * Set the RSRP pool selection
   * \param rsrpBased The RSRP pool selection
   */
  void SetHaveRsrpBasedPoolSelection (bool rsrpBased);

  /**
   * Set the RSRP lower boundary
   * \param rsrpThreshLow The lower boundary
   */
  void SetRsrpThreshLow (int32_t rsrpThreshLow);

  /**
   * Set the RSRP higher boundary
   * \param rsrpThreshHigh The higher boundary
   */
  void SetRsrpThreshHigh (int32_t rsrpThreshHigh);

  /**
   * Set the transmission probability of the discovery message
   * \param txProb The transmission probability of the discovery message
   */
  void SetDiscTxProbability (std::string txProb);

  /**
   * Get discover cyclic prefix length
   * \return the discover cyclic prefix length as string
   */
  std::string GetDiscCpLen (void);

  /**
   * Get discovery period
   * \return The discovery period
   */
  std::string GetDiscPeriod (void);

  /**
   * Get the number of retransmissions of the discovery message
   * \return The number of retransmissions of the discovery message
   */
  int8_t GetNumRetx (void);

  /**
   * Get the number of repetition of the bitmap within the discovery period
   * \return The number of repetition of the bitmap within the discovery period
   */
  int32_t GetNumRepetition (void);

  /**
   * Get the number of resource blocks to be used for discovery
   * \return The number of resource blocks to be used for discovery
   */
  int32_t GetDiscPrbNum (void);

  /**
   * Get the lower bound of resource blocks to be used for discovery
   * \return The lower bound of resource blocks to be used for discovery
   */
  int32_t GetDiscPrbStart (void);

  /**
   * Get the upper bound of resource blocks to be used for discovery
   * \return The upper bound of resource blocks to be used for discovery
   */
  int32_t GetDiscPrbEnd (void);

  /**
   * Get the offset to start the discovery
   * \return The offset to start the discovery
   */
  int32_t GetDiscOffset (void);

  /**
   * Get the discovery bitmap
   * \return The discovery bitmap
   */
  int64_t GetDiscBitmap (void);

  /**
   * Get the tx parameters for discovery
   * \return True if tx parameters for discovery are set
   */
  bool GetHaveTxParameters (void);

  /**
   * Get the alpha
   * \return The alpha value
   */
  std::string GetDiscTxAlpha (void);

  /**
   * Get the P0
   * \return p0 The P0 value
   */
  int32_t GetDiscTxP0 (void);

  /**
   * Get the pool selection criteria for the discovery
   * \return The pool selection criteria: RSRPBASED or RANDOM
   */
  std::string GetDiscPoolSelection (void);

  /**
   * Get the RSRP pool selection
   * \return true if the pool selection is RSRP based
   */
  bool GetHaveRsrpBasedPoolSelection (void);

  /**
   * Get the RSRP lower boundary
   * \return The RSRP lower boundary
   */
  int32_t GetRsrpThreshLow (void);

  /**
   * Get the RSRP higher boundary
   * \return The RSRP higher boundary
   */
  int32_t GetRsrpThreshHigh (void);

  /**
   * Get the transmission probability of the discovery message
   * \return txProb The transmission probability of the discovery message
   */
  std::string GetDiscTxProbability (void);

private:
  LteRrcSap::SlDiscResourcePool m_pool; ///< The Sidelink discovery pool

protected:
  /**
   * the CP length
   */
  std::string m_discCpLen;
  /**
   * the discovery period
   */
  std::string m_period;
  /**
   * the number of repetition of the discovery message
   */
  int8_t m_numRetx;
  /**
   * the number of repetition of the bitmap within the discovery period
   */
  int32_t m_numRep;
  /**
   * the number of resource blocks to be used for discovery
   */
  int32_t m_discPrbNum;
  /**
   * the lower bound of resource blocks to be used for discovery
   */
  int32_t m_discPrbStart;
  /**
   * the upper bound of resource blocks to be used for discovery
   */
  int32_t m_discPrbEnd;
  /**
   * the discovery offset
   */
  int32_t m_discOffset;
  /**
   * the discovery bitmap
   */
  int64_t m_discBitmapValue;
  /**
   * the transmission parameters for discovery
   */
  bool m_txParam;
  /**
   * the alpha
   */
  std::string m_txAlpha;
  /**
   * the P0
   */
  int32_t m_discP0;

  /**
   * Pool selection
   * RSRP-based or random
   */
  std::string  m_poolSelection;

  /**
   * which Pool Selection to choose.
   * Is it RSRP-based
   *
   */
  bool m_rsrpBased;

  /**
   * RSRP-based Pool Selection:
   * low boundary
   */
  uint32_t m_rsrpThreshLow;

  /**
   * RSRP-based Pool Selection:
   * high boundary
   */
  uint32_t m_rsrpThreshHigh;

  /**
   * the transmission probability of the discovery message
   */
  std::string m_discTxProb;

  /**
   * the reception parameters for discovery
   */
  bool m_rxParam;
};

} // namespace ns3

#endif
