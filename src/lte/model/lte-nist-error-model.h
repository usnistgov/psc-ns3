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

#ifndef LTE_NIST_ERROR_MODEL_H
#define LTE_NIST_ERROR_MODEL_H
#include <stdint.h>
#include <ns3/lte-harq-phy.h>

namespace ns3 {

const uint16_t XTABLE_SIZE = 3;   //!<number of element in the X tables
const uint16_t PUSCH_AWGN_SIZE = 33;   //!<number of BLER values per HARQ transmission
const uint16_t PSDCH_AWGN_SIZE = 23;   //!<number of BLER values per HARQ transmission
const uint16_t PSCCH_AWGN_SIZE = 38;   //!<number of BLER values
const uint16_t PSBCH_AWGN_SIZE = 43;   //!<number of BLER values

/**
 * Structure to report transport block error rate value and computed SINR
 */
struct TbErrorStats_t
{
  double tbler;   //!< block error rate
  double sinr;   //!< SINR value
};

/**
  * This class contains functions to access the BLER for Sidelink physical channels,
  * i.e., Pssch, Psdch, Pscch Psbch and LTE Pusch obtained by using and extending
  * (for Sidelink physical channels) MATLAB LTE toolbox.
  * For more details please refer to; http://nvlpubs.nist.gov/nistpubs/ir/2016/NIST.IR.8157.pdf
  */
class LteNistErrorModel
{

public:
  /**
   * List of possible channels
   */
  enum LtePhyChannel
  {
    PDCCH,
    PDSCH,
    PUCCH,
    PUSCH,
    PSCCH,
    PSSCH,
    PSDCH
  };

  /**
   * List of channel models. Note that not all models are available for each physical channel
   */
  enum LteFadingModel
  {
    AWGN
  };

  /**
   * List of transmission mode. Note that not all transmission modes are available for each physical channel or fading model
   */
  enum LteTxMode
  {
    SISO,
    SIMO,
    TxDiversity,
    SpatMultplex
  };

  /**
   * \brief Lookup the BLER for the given SINR
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param mcs The MCS of the TB
   * \param sinr The mean sinr of the TB
   * \param harqHistory The HARQ information
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetPsschBler (LteFadingModel fadingChannel, LteTxMode txmode, uint16_t mcs, double sinr, HarqProcessInfoList_t harqHistory);

  /**
   * \brief Lookup the BLER for the given SINR
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param sinr The mean sinr of the TB
   * \param harqHistory The HARQ information
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetPsdchBler (LteFadingModel fadingChannel, LteTxMode txmode, double sinr, HarqProcessInfoList_t harqHistory);

  /**
   * \brief Lookup the BLER for the given SINR
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param sinr The mean sinr of the TB
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetPscchBler (LteFadingModel fadingChannel, LteTxMode txmode, double sinr);
  
  /**
   * \brief Get SINR from BLER value function
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param mcs The MCS
   * \param harq The HARQ index
   * \param bler The BLER
   * \return The SINR value
   */
  static double GetPsschSinrFromBler (LteFadingModel fadingChannel, LteTxMode txmode, uint16_t mcs, uint8_t harq, double bler);

  /**
   * \brief Lookup the BLER for the given SINR
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param mcs The MCS of the TB
   * \param sinr The mean sinr of the TB
   * \param harqHistory The HARQ information
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetPuschBler (LteFadingModel fadingChannel, LteTxMode txmode, uint16_t mcs, double sinr, HarqProcessInfoList_t harqHistory);

  /**
   * \brief Lookup the BLER for the given SINR
   * \param fadingChannel The channel to use
   * \param txmode The Transmission mode used
   * \param sinr The mean sinr of the TB
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetPsbchBler (LteFadingModel fadingChannel, LteTxMode txmode, double sinr);


  //TODO: as error models for other physical channels are added, add new functions. The signature should be the same

private:
  /**
   * \brief Find the index of the data. Returns -1 if out of range.
   * \param mcs The MCS of the TB
   * \param harq The transmission number
   * \return The row index
   */
  static int16_t GetRowIndex (uint16_t mcs, uint8_t harq);

  /**
   * \brief Find the index of the column where the BLER is located. Returns -1 if out of range.
   * \param val The BLER value
   * \param min The minimum BLER value
   * \param max The maximum BLER value
   * \param step The step size
   * \return The index of the column
   */
  static int16_t GetColIndex (double val, double min, double max, double step);

  /**
   * \brief Get BLER value function
   * \param *xtable Pointer to the x-axis table
   * \param *ytable Pointer to the y-axis table
   * \param ysize The number of columns of the table containing y-axis values (BLER)
   * \param mcs The MCS
   * \param harq The HARQ index
   * \param sinr The SINR
   * \return The BLER value
   */
  static double GetBlerValue (const double (*xtable)[XTABLE_SIZE], const double *ytable, const uint16_t ysize, uint16_t mcs, uint8_t harq, double sinr);

  /**
   * \brief Get SINR value function
   * \param *xtable Pointer to the x-axis table
   * \param *ytable Pointer to the y-axis table
   * \param ysize The number of columns of the table containing y-axis values (BLER)
   * \param mcs The MCS
   * \param harq The HARQ index
   * \param bler The BLER
   * \return The SINR value
   */
  static double GetSinrValue (const double (*xtable)[XTABLE_SIZE], const double *ytable, const uint16_t ysize, uint16_t mcs, uint8_t harq, double bler);

  /**
   * \brief Compute the SINR value given the index on the table
   * \param index The index
   * \param min The minimum value
   * \param max The maximum value
   * \param step The step size
   * \return The SINR value
   */
  static double GetValueForIndex (uint16_t index, double min, double max, double step);

  /**
   * \brief Compute the index of the last column of the table given the BLER
   * \param bler The BLER
   * \param row The row number
   * \param *ytable Pointer to the y-axis table
   * \param ysize The number of columns of the table containing y-axis values
   * \return The index of the last column where BLER is above the target one
   */
  static uint16_t GetBlerIndex (double bler, uint16_t row, const double *ytable, const uint16_t ysize);

  /**
   * \brief Generic function to compute the effective BLER and SINR
   * \param *xtable Pointer to the x-axis table
   * \param *ytable Pointer to the y-axis table
   * \param ysize The number of columns of the table containing y-axis values
   * \param mcs The MCS
   * \param harq The HARQ index
   * \param prevSinr The previous SINR value in linear scale
   * \param newSinr The new SINR value in linear scale
   * \return A Struct of type TbErrorStats_t containing the TB error rate and the SINR
   */
  static TbErrorStats_t GetBler (const double (*xtable)[XTABLE_SIZE], const double *ytable, const uint16_t ysize, uint16_t mcs, uint8_t harq, double prevSinr, double newSinr);
}; //end class
} // namespace ns3
#endif /* LTE_NIST_ERROR_MODEL_H */
