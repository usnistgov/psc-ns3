/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/simulator.h"

#include "psc-application.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscApplication");

namespace psc {
NS_OBJECT_ENSURE_REGISTERED (PscApplication);

// Initialize static member
uint64_t PscSequenceNumber::s_sequenceNumber = 0;

TypeId
PscApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::PscApplication")
    .SetParent<Application> ()
    .AddAttribute ("AppName",
                   "The application name to show in the trace",
                   StringValue ("IncidentApplication"),
                   MakeStringAccessor (&PscApplication::m_appName),
                   MakeStringChecker ())
    .AddAttribute ("Protocol",
                   "The transport protocol to use",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&PscApplication::m_socketTid),
                   MakeTypeIdChecker ())
    .AddTraceSource ("Tx",
                     "A packet is transmitted",
                     MakeTraceSourceAccessor (&PscApplication::m_txTrace),
                     "ns3::PscApplication::TxTracedCallback")
    .AddTraceSource ("Rx",
                     "A packet is received",
                     MakeTraceSourceAccessor (&PscApplication::m_rxTrace),
                     "ns3::PscApplication::RxTracedCallback")
    .AddTraceSource ("Times",
                     "Application Start and Stop times",
                     MakeTraceSourceAccessor (&PscApplication::m_startStopTimeTrace),
                     "ns3::PscApplication::TimeTracedCallback")
  ;
  return tid;
}

PscApplication::PscApplication ()
{
  NS_LOG_FUNCTION (this);
}

PscApplication::~PscApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
PscApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  Application::DoDispose ();
}

} // namespace psc
} // namespace ns3
