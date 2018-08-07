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

#ifndef LTE_SL_RESOURCE_POOL_FACTORY_H
#define LTE_SL_RESOURCE_POOL_FACTORY_H

#include "lte-rrc-sap.h"
#include "lte-sl-pool-factory.h"

namespace ns3 {

/** Class to configure and generate resource pools */
class LteSlResourcePoolFactory : public LteSlPoolFactory
{
public:
  LteSlResourcePoolFactory ();

  /**
   * Creates a new resource pool based on the factory configuration
   * \return The new resource pool
   */
  LteRrcSap::SlCommResourcePool CreatePool ();

  /**
   * Sets the cyclic prefix length for the control channel
   * \param cpLen The cyclic prefix length
   */
  void SetControlCpLen (std::string cpLen);

  /** 
   * Set the Sidelink period duration
   * \param period The Sidelink period
   */
  void SetControlPeriod (std::string period);

  /**
   * Sets the number of PRBs in  the control channel (from the start up and the end down)
   * \param prbNum The number of PRBs
   */
  void SetControlPrbNum (int8_t prbNum);

  /**
   * Sets the index of the PRB where the control channel starts
   * \param prbStart The index of the PRB
   */
  void SetControlPrbStart (int8_t prbStart);

  /**
   * Sets the index of the PRB where the control channel ends
   * \param prbEnd The index of the PRB
   */
  void SetControlPrbEnd (int8_t prbEnd);

  /**
   * Sets the offset (in subframe) for the control channel
   * \param offset the number of subframes from the beginning of the Sidelink period
   */
  void SetControlOffset (int16_t offset);

  /**
   * Sets the bitmap identifying which subframes belong to the control channel
   * \param value The bitmap identifying which subframes belong to the control channel
   */
  void SetControlBitmap (int64_t value);

  /**
   * Sets the cyclic prefix for the data channel
   * \param cpLen The cyclic prefix
   */
  void SetDataCpLen (std::string cpLen);

  /**
   * Sets the frequency hopping parameters
   * \param hoppingParameter The frequency hopping parameter
   */
  void SetDataHoppingParameter (uint16_t hoppingParameter);

  /**
   * Sets the bands for frequency hoping
   * \param subbands The bands for frequency hopping
   */
  void SetDataHoppingSubbands (std::string subbands);

  /**
   * Sets the RB offset for frequency hopping
   * \param rbOffset The RB offset to use
   */
  void SetDataHoppingOffset (int8_t rbOffset);

  /**
   * Sets the hopping information for frequency hopping
   * \param hoppingInfo The hopping info type to use
   */
  void SetDataHoppingInfo (int8_t hoppingInfo);

  // UE Selected Parameters
  /** 
   * Indicates if the configuration is for a UE selected pool
   * \param ueSelected true if the configuration is for a UE selected pool
   */
  void SetHaveUeSelectedResourceConfig (bool ueSelected);

  /**
   * Indicates if the configuration contains the TRP subset information
   * \param haveTrptSubset True if the configuration contain the TRP subset information
   */
  void SetHaveTrptSubset (bool haveTrptSubset);

  /**
   * Sets the TRP subset value
   * \param value The TRP subset value
   */
  void SetTrptSubset (int64_t value);

  /**
   * Sets the number of PRBs for the shared channel (from the start up and end down)
   * \param prbNum The number of PRBs for the shared channel
   */
  void SetDataPrbNum (int8_t prbNum);

  /**
   * Sets the index of the PRB where the shared channel starts
   * \param prbStart the index of the PRB where the shared channel starts
   */
  void SetDataPrbStart (int8_t prbStart);

  /**
   * Sets the index of the PRB where the shared channel ends
   * \param prbEnd the index of the PRB where the shared channel ends
   */
  void SetDataPrbEnd (int8_t prbEnd);

  /**
   * Sets the offset of the shared channel (from the start of the period)
   * \param offset The offset (in subframe)
   */
  void SetDataOffset (int16_t offset);

  /**
   * Sets the bitmap identifying the subframes to use for the shared channel
   * \param value The bitmap value
   */
  void SetDataBitmap (int64_t value);

  // Tx Parameters
  /**
   * Indicates if the configuration provides transmission parameters to use
   * \param txParam True if the configuration provides transmission parameters to use
   */ 
  void SetHaveTxParameters (bool txParam);

  /**
   * Sets the alpha parameter for the power control algorithm in the control channel
   * \param alpha The alpha parameter for the power control algorithm
   */
  void SetControlTxAlpha (std::string alpha);

  /**
   * Sets the p0 parameter for the power control algorithm in the control channel
   * \param p0 The p0 parameter for the power control algorithm
   */
  void SetControlTxP0 (int16_t p0);

  /**
   * Sets the alpha parameter for the power control algorithm in the shared channel
   * \param alpha The alpha parameter for the power control algorithm
   */
  void SetDataTxAlpha (std::string alpha);

  /**
   * Sets the p0 parameter for the power control algorithm in the shared channel
   * \param p0 The p0 parameter for the power control algorithm
   */
  void SetDataTxP0 (int16_t p0);

  /**
   * Gets the cyclic prefix length for the control channel
   * \return The control cyclic prefix length
   */
  std::string GetControlCpLen (void);

  /**
   * Gets the Sidelink period duration
   * \return The Sidelink control period
   */
  std::string GetControlPeriod (void);

  /**
   * Gets the number of PRBs in  the control channel (from the start up and the end down)
   * \return The number of PRBs for transmitting the control
   */
  int8_t GetControlPrbNum (void);

  /**
   * Gets the index of the PRB where the control channel starts
   * \return The index of the starting PRB
   */
  int8_t GetControlPrbStart (void);

  /**
   * Gets the index of the PRB where the control channel ends
   * \return The index of the last PRB
   */
  int8_t GetControlPrbEnd (void);

  /**
   * Gets the offset (in subframe) for the control channel
   * \return The offset (in subframe) for control transmission from the beginning of the Sidelink period
   */
  int16_t GetControlOffset (void);

  /**
   * Gets the bitmap identifying which subframes belong to the control channel
   * \return The bitmap identifying which subframes belong to the control channel
   */
  int64_t GetControlBitmap (void);

  /**
   * Gets the cyclic prefix for the data channel
   * \return The data cyclic prefix
   */
  std::string GetDataCpLen (void);

  /**
   * Gets the frequency hopping parameters
   * \return The frequency hopping parameter
   */
  uint16_t GetDataHoppingParameter (void);

  /**
   * Gets the bands for frequency hoping
   * \return The bands for frequency hopping
   */
  std::string GetDataHoppingSubbands (void);

  /**
   * Gets the RB offset for frequency hopping
   * \return The RB offset to use for frequency hopping
   */
  int8_t GetDataHoppingOffset (void);

  /**
   * Gets the hopping information for frequency hopping
   * \return The hopping info type to use
   */
  int8_t GetDataHoppingInfo (void);

  // UE Selected Parameters
  /**
   * Gets HaveUeSelectedResourceConfig flag
   * \return True true if The configuration is for a UE selected pool
   */
  bool GetHaveUeSelectedResourceConfig (void);

  /**
   * Gets HaveTrptSubset flag
   * \return True if the configuration contain the TRP subset information
   */
  bool GetHaveTrptSubset (void);

  /**
   * Gets the TRP subset value
   * \return The TRP subset value
   */
  int64_t GetTrptSubset (void);

  /**
   * Gets the number of PRBs for the shared channel (from the start up and end down)
   * \return The number of PRBs for the data shared channel
   */
  int8_t GetDataPrbNum (void);

  /**
   * Gets the index of the PRB where the shared channel starts
   * \return The index of the PRB where the data shared channel starts
   */
  int8_t GetDataPrbStart (void);

  /**
   * Gets the index of the PRB where the shared channel ends
   * \return The index of the PRB where the shared channel ends
   */
  int8_t GetDataPrbEnd (void);

  /**
   * Gets the offset of the shared channel (from the start of the period)
   * \return The data offset (in subframe)
   */
  int16_t GetDataOffset (void);

  /**
   * Gets the bitmap identifying the subframes to use for the shared channel
   * \return The bitmap value
   */
  int64_t GetDataBitmap (void);

  // Tx Parameters
  /**
   * Gets have tx parameters
   * \return True if the transmission parameters are set
   */
  bool GetHaveTxParameters (void);

  /**
   * Gets the alpha parameter for the power control algorithm in the control channel
   * \return The alpha parameter for control transmission for the power control algorithm
   */
  std::string GetControlTxAlpha (void);

  /**
   * Gets the p0 parameter for the power control algorithm in the control channel
   * \return The p0 parameter value for control transmission for the power control algorithm
   */
  int16_t GetControlTxP0 (void);

  /**
   * Gets the alpha parameter for the power control algorithm in the shared channel
   * \return The alpha parameter for data transmission for the power control algorithm
   */
  std::string GetDataTxAlpha (void);

  /**
   * Gets the p0 parameter for the power control algorithm in the shared channel
   * \return The p0 parameter for data transmission for the power control algorithm
   */
  int16_t GetDataTxP0 (void);

private:
  LteRrcSap::SlCommResourcePool m_pool; ///< Sidelink communication pool

protected:
  // Control
  std::string m_scCpLen; //!< cyclic prefix for the control channel
  std::string m_period; //!< Sidelink period duration
  int8_t m_scPrbNum; //!< number of PRBs in the control channel
  int8_t m_scPrbStart; //!< index of the PRB where the control channel starts
  int8_t m_scPrbEnd; //!< index of the PRB where the control channel ends
  int16_t m_scOffset; //!< offset when the control channel starts (subframe)
  //int m_scBitmapSize;
  int64_t m_scBitmapValue; //!< the bitmap specifying which subframe to use in the control channel

  // Data
  std::string m_dataCpLen; //!< cyclic prefix for the shared channel
  uint16_t m_hoppingParameter; //!< frequency hopping for the shared channel
  std::string m_subbands; //!< number of subbands
  int8_t m_rbOffset; //!< offset in the PRBs
  int8_t m_hoppingInfo; //!< Information in hopping bits.

  // UE Selected Parameters
  bool m_ueSelected; //!< indicates if the pool if for UE selected mode
  bool m_haveTrptSubset; //!< indicates if the TRP subset is defined
  //int m_trptSubsetSize; 
  int64_t m_trptSubsetValue; //!< value for the TRP subset
  int8_t m_dataPrbNum; //!< number of PRBs in the shared channel
  int8_t m_dataPrbStart; //!< index of the PRB where the shared channel starts
  int8_t m_dataPrbEnd; //!< index of the PRB where the shared channel ends
  int16_t m_dataOffset; //!< offset when the shared channel starts (subframe)
  //int m_dataBitmapSize;
  int64_t m_dataBitmapValue; //!< bitmap specifying which subframe to use in the shared channel

  // Tx Parameters
  bool m_txParam; //!< indicates if the transmission parameters are defined
  std::string m_txAlpha; //!< the alpha parameter for power control in the control channel
  int16_t m_txP0; //!< the p0 parameter for the power control in the control channel
  std::string m_dataAlpha; //!< the alpha parameter for power control in the shared channel
  int16_t m_dataP0; //!< the p0 parameter for the power control in the shared channel

};

}

#endif
