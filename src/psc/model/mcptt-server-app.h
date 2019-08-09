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

#ifndef MCPTT_SERVER_APP_H
#define MCPTT_SERVER_APP_H

#include <vector>

#include <ns3/application.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

namespace ns3 {

class McpttOnNetworkFloorArbitrator;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT server application.
 */
class McpttServerApp : public Application
{
public:
 /**
  * Get the type ID.
  * \return the object TypeId
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttServerApp class.
  */
 McpttServerApp (void);
 /**
  * The destructor of the McpttServerApp class.
  */
 virtual ~McpttServerApp (void);
 /**
  * Gets the type ID fo this McpttServerApp instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
protected:
 /**
  * The callback to fire when a message is received.
  * \param msg The message that was received.
  */
 virtual void RxCb (const McpttMsg& msg);
 /**
  * Disposes of the McpttServerApp instance.
  */
 virtual void DoDispose (void);
 /**
  * Starts the McpttServerApp application.
  */
 virtual void StartApplication (void);
 /**
  * Stops the McpttServerApp application.
  */
 virtual void StopApplication (void);
 /**
  * The callback to fire when a message is sent.
  * \param msg The message that was sent.
  */
 virtual void TxCb (const McpttMsg& msg);
 /**
  * TracedCallback signature for McpttMsg transmission or reception events
  * \param [in] app Ptr<Application>
  * \param [in] callId Call ID
  * \param [in] msg McpttMsg
  */
 typedef void (* TxRxTracedCallback) (Ptr<const Application> app, uint16_t callId, const McpttMsg& msg);
private:
 Ptr<McpttOnNetworkFloorArbitrator> m_arbitrator; //!< The arbitrator.
 Address m_localAddress; //!< The local IP address.
 TracedCallback<Ptr<const Application>, uint16_t, const McpttMsg&> m_rxTrace; //!< The Rx trace.
 TracedCallback<Ptr<const Application>, uint16_t, const McpttMsg&> m_txTrace; //!< The Tx trace.
public:
 /**
  * Gets the arbitrator.
  * \returns The arbitrator.
  */
 virtual Ptr<McpttOnNetworkFloorArbitrator> GetArbitrator (void) const;
 /**
  * Gets the address of the host.
  * \returns The address.
  */
 virtual Address GetLocalAddress (void) const;
 /**
  * Sets the arbitrator.
  * \param arbitrator The arbitrator.
  */
 virtual void SetArbitrator (const Ptr<McpttOnNetworkFloorArbitrator> arbitrator);
 /**
  * Sets the address of the host.
  * \param localAddress The address.
  */
 virtual void SetLocalAddress (const Address& localAddress);
};

} // namespace ns3

#endif /* MCPTT_SERVER_APP_H */

