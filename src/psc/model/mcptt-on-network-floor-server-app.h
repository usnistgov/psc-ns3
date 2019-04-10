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

#ifndef MCPTT_ON_NETWORK_FLOOR_SERVER_APP_H
#define MCPTT_ON_NETWORK_FLOOR_SERVER_APP_H

#include <vector>

#include <ns3/application.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

namespace ns3 {

class McpttOnNetworkFloorArbitrator;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT floor control application.
 */
class McpttOnNetworkFloorServerApp : public Application
{
public:
 /**
  * Get the type ID.
  * \return the object TypeId
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttPtt class.
  */
 McpttOnNetworkFloorServerApp (void);
 /**
  * The destructor of the McpttOnNetworkFloorServerApp class.
  */
 virtual ~McpttOnNetworkFloorServerApp (void);
 /**
  * Gets the type ID fo this McpttOnNetworkFloorServerApp instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
protected:
 /**
  * Disposes of the McpttOnNetworkFloorServerApp instance.
  */
 virtual void DoDispose (void);
 /**
  * Starts the McpttOnNetworkFloorServerApp application.
  */
 virtual void StartApplication (void);
 /**
  * Stops the McpttOnNetworkFloorServerApp application.
  */
 virtual void StopApplication (void);
private:
 Ptr<McpttOnNetworkFloorArbitrator> m_arbitrator; //!< The arbitrator.
 Ipv4Address m_localAddress; //!< The local IP address.
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
 virtual Ipv4Address GetLocalAddress (void) const;
 /**
  * Sets the arbitrator.
  * \param arbitrator The arbitrator.
  */
 virtual void SetArbitrator (const Ptr<McpttOnNetworkFloorArbitrator> arbitrator);
 /**
  * Sets the address of the host.
  * \param localAddress The address.
  */
 virtual void SetLocalAddress (const Ipv4Address& localAddress);
};

} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_SERVER_APP_H */

