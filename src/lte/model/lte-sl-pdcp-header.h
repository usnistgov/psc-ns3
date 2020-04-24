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

#ifndef LTE_SL_PDCP_HEADER_H
#define LTE_SL_PDCP_HEADER_H

#include "ns3/header.h"

#include <list>

namespace ns3 {

/**
 * \ingroup lte
 * \brief The packet header for the Packet Data Convergence Protocol (PDCP) packets
 *
 * This class has fields corresponding to those in an PDCP header used for Sidelink
 * Radio Bearers (SLRB) as well as
 * methods for serialization to and deserialization from a byte buffer.
 * It follows 3GPP TS 36.323 Packet Data Convergence Protocol (PDCP) specification
 * release 14.
 * Implementation notes:
 * The security field can be either PTK Identity (for one to many communication) or 
 * K_D-Sess ID (for one to one communication). 
 * Current version does not implement the MAC-I elements that would be used if the 
 * SLRB needed integrity protection.
 */
class LteSlPdcpHeader : public Header
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  LteSlPdcpHeader ();
  ~LteSlPdcpHeader ();

  /**
   * \brief Set the SDU type (3-bit value)
   * 
   * \param sduType The SDU type
   */
  void SetSduType (uint8_t sduType); 
  
  /**
   * \brief Set the PGK index (5-bit value)
   * 
   * \param pgkIndex The PGK index
   */
  void SetPgkIndex (uint8_t pgkIndex);
  
  /**
   * \brief Set the security identity (16-bit value)
   * 
   * \param secIdentity The security identity
   */
  void SetSecurityIdentity (uint16_t secIdentity);
  
  /**
   * \brief Set the sequence number (16-bit value)
   * 
   * \param sequenceNumber The sequence number
   */
  void SetSequenceNumber (uint16_t sequenceNumber);

  /**
   * \brief Get the SDU type
   * 
   * \return The SDU type
   */
  uint8_t GetSduType () const;
  
  /**
   * \brief Get the PGK index
   * 
   * \return The PGK index
   */
  uint8_t GetPgkIndex () const;
  
  /**
   * \brief Get the security identity
   * 
   * \return The security identity
   */
  uint16_t GetSecurityIdentity () const;
  
  /**
   * \brief Get the sequence number
   * 
   * \return The sequence number
   */
  uint16_t GetSequenceNumber () const;

  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t m_sduType; ///< The SDU type
  uint8_t m_pgkIndex; ///< The PGK index
  uint16_t m_secIdentity; ///< The security index
  uint16_t m_sequenceNumber; ///< The sequence number

};

} // namespace ns3

#endif // LTE_SL_PDCP_HEADER_H
