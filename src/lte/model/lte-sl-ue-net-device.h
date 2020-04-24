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

#ifndef LTE_SL_UE_NET_DEVICE_H
#define LTE_SL_UE_NET_DEVICE_H

#include "ns3/lte-net-device.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Packet;
class EpcUeNas;

/**
 * \ingroup lte
 * The LteSlUeNetDevice serves as a virtual device that directs any sent
 * packets to the Nas. It requires an underlying LteUeNetDevice to be installed.
 *
 */
class LteSlUeNetDevice : public LteNetDevice
{

public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LteSlUeNetDevice (void);
  virtual ~LteSlUeNetDevice (void);
  virtual void DoDispose ();

  // inherited from NetDevice
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  /**
   * \brief Set the NAS.
   * \param nas the LTE UE NAS
   */
  void SetNas (const Ptr<EpcUeNas> nas);

  /**
   * \brief Get the NAS.
   * \return the LTE UE NAS
   */
  Ptr<EpcUeNas> GetNas (void) const;


protected:
  // inherited from Object
  virtual void DoInitialize (void);


private:
  Ptr<EpcUeNas> m_nas; ///< the NAS

}; // end of class LteSlUeNetDevice

} // namespace ns3

#endif /* LTE_SL_UE_NET_DEVICE_H */
