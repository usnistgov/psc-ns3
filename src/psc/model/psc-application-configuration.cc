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

#include "ns3/log.h"

#include "psc-application-configuration.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PscApplicationConfiguration");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (PscApplicationConfiguration);

PscApplicationConfiguration::PscApplicationConfiguration (
  std::string name,
  TypeId sType,
  uint16_t basePort
  )
{
  NS_LOG_FUNCTION (this << name << sType << basePort);

  m_name = name;
  m_socketType = sType;
  m_port = basePort;
  m_instances = 0;
}

PscApplicationConfiguration::~PscApplicationConfiguration ()
{
  m_packetsPerSessionRandomVariable = 0;
  m_packetIntervalRandomVariable  = 0;
  m_sessionIntervalRandomVariable = 0;
}

void PscApplicationConfiguration::SetApplicationPattern (
  Ptr<RandomVariableStream> pktsPerSessionRandomVariable,
  Ptr<RandomVariableStream> pktsIntervalRandomVariable,
  Ptr<RandomVariableStream> sessionIntervalRandomVariable,
  uint32_t pSizeClient, uint32_t pSizeServer)
{
  NS_LOG_FUNCTION (this << pktsPerSessionRandomVariable << pktsIntervalRandomVariable << sessionIntervalRandomVariable << pSizeClient << pSizeServer);

  m_packetsPerSessionRandomVariable = pktsPerSessionRandomVariable;
  m_packetIntervalRandomVariable = pktsIntervalRandomVariable;
  m_sessionIntervalRandomVariable = sessionIntervalRandomVariable;
  m_packetSizeClient = pSizeClient;
  m_packetSizeServer = pSizeServer;

}

std::string
PscApplicationConfiguration::GetName (void) const
{
  NS_LOG_FUNCTION (this);

  return m_name;
}

TypeId
PscApplicationConfiguration::GetSocketType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_socketType;
}

uint16_t
PscApplicationConfiguration::GetNextPort (void)
{
  NS_LOG_FUNCTION (this);

  return m_port++;
}

uint32_t
PscApplicationConfiguration::GetPacketSizeClient ()
{
  NS_LOG_FUNCTION (this);

  return m_packetSizeClient;
}

uint32_t
PscApplicationConfiguration::GetPacketSizeServer ()
{
  NS_LOG_FUNCTION (this);

  return m_packetSizeServer;
}

Ptr<RandomVariableStream>
PscApplicationConfiguration::GetPacketsPerSession ()
{
  NS_LOG_FUNCTION (this);

  return m_packetsPerSessionRandomVariable;
}

Ptr<RandomVariableStream>
PscApplicationConfiguration::GetPacketInterval ()
{
  NS_LOG_FUNCTION (this);

  return m_packetIntervalRandomVariable;
}

Ptr<RandomVariableStream>
PscApplicationConfiguration::GetSessionInterval ()
{
  NS_LOG_FUNCTION (this);

  return m_sessionIntervalRandomVariable;
}

uint32_t
PscApplicationConfiguration::IncrementInstances (void)
{
  NS_LOG_FUNCTION (this);

  return m_instances++;
}

} // namespace psc
} // namespace ns3
