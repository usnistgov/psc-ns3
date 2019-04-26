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

#include <ns3/log.h>
#include <ns3/ipv4-address.h>
#include <ns3/pointer.h>

#include "mcptt-on-network-floor-arbitrator.h"

#include "mcptt-on-network-floor-server-app.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorServerApp");

NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorServerApp);

TypeId
McpttOnNetworkFloorServerApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttOnNetworkFloorServerApp")
    .SetParent<Application> ()
    .AddConstructor<McpttOnNetworkFloorServerApp>()
    .AddAttribute ("FloorArbitrator", "The floor arbitration state machine.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttOnNetworkFloorServerApp::GetArbitrator,
                                        &McpttOnNetworkFloorServerApp::SetArbitrator),
                   MakePointerChecker<McpttOnNetworkFloorArbitrator> ())
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&McpttOnNetworkFloorServerApp::m_rxTrace),
                     "ns3::McpttOnNetworkFloorServerApp::RxTrace")
    .AddTraceSource ("TxTrace", "The trace for capturing sent messages",
                     MakeTraceSourceAccessor (&McpttOnNetworkFloorServerApp::m_txTrace),
                     "ns3::McpttOnNetworkFloorServerApp::TxTrace")
  ;

    return tid;
}  

McpttOnNetworkFloorServerApp::McpttOnNetworkFloorServerApp (void)
  : Application ()
{  
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorServerApp::~McpttOnNetworkFloorServerApp (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttOnNetworkFloorServerApp::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorServerApp::GetTypeId ();
}

void
McpttOnNetworkFloorServerApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  Object::DoDispose ();
}

void
McpttOnNetworkFloorServerApp::RxCb (const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  m_rxTrace (*this, msg);
}

void
McpttOnNetworkFloorServerApp::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  GetArbitrator ()->Start ();
}

void
McpttOnNetworkFloorServerApp::StopApplication (void)
{
  NS_LOG_FUNCTION (this);

  GetArbitrator ()->Stop ();
}

void
McpttOnNetworkFloorServerApp::TxCb (const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  m_txTrace (*this, msg);
}

Ptr<McpttOnNetworkFloorArbitrator>
McpttOnNetworkFloorServerApp::GetArbitrator (void) const
{
  NS_LOG_FUNCTION (this);

  return m_arbitrator;
}

Ipv4Address
McpttOnNetworkFloorServerApp::GetLocalAddress (void) const
{
  return m_localAddress;
}

void
McpttOnNetworkFloorServerApp::SetArbitrator (const Ptr<McpttOnNetworkFloorArbitrator> arbitrator)
{
  NS_LOG_FUNCTION (this);

  if (m_arbitrator != 0)
    {
      arbitrator->SetRxCb (MakeNullCallback<void, const McpttMsg&> ());
      arbitrator->SetTxCb (MakeNullCallback<void, const McpttMsg&> ());
    }

  if (arbitrator != 0)
    {
      arbitrator->SetOwner (this);
      arbitrator->SetRxCb (MakeCallback (&McpttOnNetworkFloorServerApp::RxCb, this));
      arbitrator->SetTxCb (MakeCallback (&McpttOnNetworkFloorServerApp::TxCb, this));
    }

  m_arbitrator = arbitrator;
}

void
McpttOnNetworkFloorServerApp::SetLocalAddress (const Ipv4Address& localAddress)
{
  NS_LOG_FUNCTION (this);

  m_localAddress = localAddress;
}

} // namespace ns3

