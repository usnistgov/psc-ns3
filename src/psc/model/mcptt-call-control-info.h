/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 * 
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 * 
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef MCPTT_CALL_CONTROL_INFO_H
#define MCPTT_CALL_CONTROL_INFO_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3 {

/**
 * \ingroup mcptt
 *
 * This class is used to access call control information
 */
class McpttCallControlInfo : public Object
{
public:
 /**
  * Gets the type ID of the  class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttCallControlInfo class.
  */
  McpttCallControlInfo (void);
 /**
  * \brief The destructor of the McpttCallControlInfo class.
  */
 virtual ~McpttCallControlInfo (void);
 /**
  * Gets the type ID of this McpttCallControlInfo instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Indicates if the call is configured for ambient listening.
  * \returns True, if the call is configured for ambient listening; othwerise, false.
  */
 virtual bool IsAmbientListening (void) const;
 /**
  * Indicates if the call is configured for a temporary group session.
  * \returns True, if the call is configured for a temporary group session; false, otherwise.
  */
 virtual bool IsTemporaryGroup (void) const;
 /**
  * Gets the call type ID of the call.
  * \returns the call type ID.
  */
 virtual uint8_t GetCallTypeId (void) const;
private:
 bool m_ambientListening; //!< The flag that indicates if the call is configured for ambient listening.
 bool m_temporaryGroup; //!< The flag that indicates if the call is configured for a temporary group.
 uint8_t m_callTypeId; //!< The call type ID of the call.
};

} // namespace ns3

#endif /* MCPTT_CALL_CONTROL_INFO_H */
