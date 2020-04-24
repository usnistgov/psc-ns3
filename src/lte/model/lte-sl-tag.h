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
#ifndef LTE_SL_TAG_H
#define LTE_SL_TAG_H

#include "ns3/tag.h"
#include "ns3/nstime.h"

namespace ns3 {

class Tag;


/**
 * Tag used to define the resource number in the control pool and the
 * transport block of the shared channel
 */

class LteSlSciTag : public Tag
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create an empty LteSlSciTag
   */
  LteSlSciTag ();

  /**
   * Create a LteSlSciTag with the given resource number and transport block size
   * \param rnti The RNTI
   * \param resNo The resource number
   * \param tbSize The transport block size
   */
  LteSlSciTag (uint16_t rnti, uint16_t resNo, uint32_t tbSize);


  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  /**
   * Get RNTI function
   *
   * \returns RNTI
   */
  uint16_t GetRnti (void) const;
  /**
   * Get LCID function
   *
   * \returns LCID
   */
  uint16_t GetResNo (void) const;
  /**
   * Get layer function
   *
   * \returns layer
   */
  uint32_t GetTbSize (void) const;

private:
  uint16_t m_rnti; ///< RNTI
  uint16_t m_resNo; ///< Resource number
  uint32_t m_tbSize; ///< Transport block size

};

/**
 * Tag used to identify the secure PC5 link
 */
class LteSlPc5ContextTag : public Tag
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create an empty LteSlPc5Tag
   */
  LteSlPc5ContextTag ();

  /**
   * Create a LteSlSciTag with the given resource number and transport block size
   * \param context The context associated with the packet
   */
  LteSlPc5ContextTag (uint32_t context);


  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  /**
   * Get context value
   *
   * \returns layer
   */
  uint32_t GetContext (void) const;

private:
  uint16_t m_context; ///< The tag value
  
};

} // namespace ns3

#endif /* LTE_SL_TAG_H */
