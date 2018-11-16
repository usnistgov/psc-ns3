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

#ifndef MCPTT_MSG_STATS_H
#define MCPTT_MSG_STATS_H

#include <ns3/object.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/type-id.h>

namespace ns3 {

/**
 * \ingroup psc
 *
 * A class used to capture MCPTT messages.
 */
class McpttMsgStats : public Object
{
public:
 /**
  * Gets the type ID of the McpttMsgStats class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttMsgStats class.
  */
  McpttMsgStats (void);
 /**
  * \brief The destructor of the McpttMsgStats class.
  */
 virtual ~McpttMsgStats (void);
 /**
  * Gets the type ID of this  instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * The sink function for tracing the received messages.
  * \param pttApp The app.
  * \param msg The message.
  */
 virtual void ReceiveRxTrace (const McpttPttApp& pttApp, const McpttMsg& msg);
 /**
  * The sink function for tracing the transmitted messages.
  * \param pttApp The app.
  * \param msg The message.
  */
  virtual void ReceiveTxTrace (const McpttPttApp& pttApp, const McpttMsg& msg);
protected:
 /**
  * Writes to the trace.
  * \param pttApp The app.
  * \param msg The message.
  * \param rx The flag that indicates if an RX or TX should be traced.
  */
 virtual void Trace (const McpttPttApp& pttApp, const McpttMsg& msg, bool rx);
private:
 bool m_callControl; //!< The flag that indicates if call control messages should be included.
 bool m_firstMsg; //!< Flag that indicates if no message has been traced yet.
 bool m_floorControl; //!< The flag that indicates if floor control messages should be included.
 bool m_includeMsgContent; //!< The flag that indicates if the message contents should be included.
 bool m_media; //!< The flag that indicates if media messages should be included.
 std::string m_outputFileName; //!< The file name of the trace file.
};

} // namespace ns3

#endif /* MCPTT_MSG_STATS_H */

