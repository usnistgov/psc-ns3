/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This software was developed at the National Institute of Standards and
 * Technology by employees of the Federal Government in the course of
 * their official duties. Pursuant to titleElement 17 Section 105 of the United
 * States Code this software is not subject to copyright protection and
 * is in the public domain.
 * NIST assumes no responsibility whatsoever for its use by other parties,
 * and makes no guarantees, expressed or implied, about its quality,
 * reliability, or any other characteristic.

 * We would appreciate acknowledgement if the software is used.

 * NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 * FROM THE USE OF THIS SOFTWARE.
 *
 * Modified by: NIST
 */

#ifndef NIST_SL_DISC_RESOURCE_POOL_FACTORY_H
#define NIST_SL_DISC_RESOURCE_POOL_FACTORY_H

#include "nist-lte-rrc-sap.h"
#include "nist-sl-pool-factory.h"

namespace ns3 {

/**
 * Sidelink discovery resource pool factory
 */
class NistSlDiscResourcePoolFactory : public NistSlPoolFactory
{
public:
  
  /**
   * Create a discovery resource pool
   */
  NistSlDiscResourcePoolFactory ();
  
  /**
   * Set the discovery resource pool
   */
  NistLteRrcSap::SlDiscResourcePool CreatePool ();

  /**
   * Set CP length 
   * \param cpLen 
   */
  void SetDiscCpLen (std::string cpLen);
    
  /**
   * Set discovery period
   * \param period the discovery period 
   */
  void SetDiscPeriod (std::string period);
  
  /**
   * Set the number of retransmissions of the discovery message
   * \param numRetx the number of retransmissions of the discovery message
   */
  void SetNumRetx (int numRetx);

  /**
   * Set the number of repetition of the bitmap within the discovery period
   * \param numRep the number of repetition of the bitmap within the discovery period
   */
  void SetNumRepitition (int numRep);

  /**
   * Set the number of resource blocks to be used for discovery
   * \param prbNum the number of resource blocks to be used for discovery
   */
  void SetDiscPrbNum (int prbNum);

  /**
   * Set the lower bound of resource blocks to be used for discovery
   * \param prbStart the lower bound of resource blocks to be used for discovery
   */
  void SetDiscPrbStart (int prbStart);
    
  /**
   * Set the upper bound of resource blocks to be used for discovery
   * \param prbEnd the upper bound of resource blocks to be used for discovery
   */
  void SetDiscPrbEnd (int prbEnd);

  /**
   * Set the offset to start the discovery
   * \param offset the offset to start the discovery
   */
  void SetDiscOffset (int offset);

  /**
   * Set the discovery bitmap
   * \param value the discovery bitmap
   */
  void SetDiscBitmap (int value);

  /**
   * Set the tx parameters for discovery 
   * \param txParam the tx parameters for discovery 
   */
  void SetHaveTxParameters (bool txParam);

  /**
   * Set the alpha
   * \param alpha the alpha
   */
  void SetDiscTxAlpha (std::string alpha);

  /**
   * Set the P0
   * \param p0 the P0 
   */
  void SetDiscTxP0 (int p0);

  /**
   * Set the transmission probability of the discovery message 
   * \param txProb the transmission probability of the discovery message
   */
  void SetDiscTxProbability (std::string txProb);

private:
  NistLteRrcSap::SlDiscResourcePool m_pool;

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
  int8_t m_discPrbNum;
  /**
   * the lower bound of resource blocks to be used for discovery
   */
  int8_t m_discPrbStart;
  /**
   * the upper bound of resource blocks to be used for discovery
   */
  int8_t m_discPrbEnd;
  /**
   * the discoevry offset
   */
  int8_t m_discOffset;
  /**
   * the discovery bitmap
   */
  int64_t m_discBitmapValue;
  /**
   * the tramission parameters for discovery
   */
  bool m_txParam;  
  /**
   * the alpha
   */
  std::string m_discAlpha;
  /**
   * the P0
   */
  int16_t m_discP0;
  /**
   * the transmission probability of the discovery message
   */
  std::string m_discTxProb;

};

} // namespace ns3

#endif
