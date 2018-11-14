/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 * 
 * Author: Evan Black <evan.black@nist.gov>
 */

#include <cmath>
#include <ns3/simulator.h>
#include <ns3/pointer.h>
#include <ns3/trace-source-accessor.h>
#include <ns3/energy-source.h>
#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include "uav-mobility-energy-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UavMobilityEnergyModel");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (UavMobilityEnergyModel);

TypeId UavMobilityEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::UavMobilityEnergyModel")
    .SetParent<DeviceEnergyModel> ()
    .SetGroupName ("Psc")
    .AddConstructor<UavMobilityEnergyModel> ()
    .AddAttribute ("AscendEnergyConversionFactor",
                   "Conversion factor applied to the speed (m/s) of the node "
                   " to determine the amperage as it ascends. In A/(m/s)",
                   DoubleValue(250),
                   MakeDoubleAccessor (&UavMobilityEnergyModel::SetAscendEnergyConversionFactor,
                                      &UavMobilityEnergyModel::GetAscendEnergyConversionFactor),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DescendEnergyConversionFactor",
                   "Conversion factor applied to the inverted speed (1/(m/s)) "
                   "of the node to determine the amperage as it descends."
                   "In A/(1/(m/s))",
                   DoubleValue(210),
                   MakeDoubleAccessor (&UavMobilityEnergyModel::SetDescendEnergyConversionFactor,
                                      &UavMobilityEnergyModel::GetDescendEnergyConversionFactor),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MoveEnergyConversionFactor",
                   "Conversion factor applied to the speed (m/s) of the node "
                   " to determine the amperage as it moves. In A/(m/s)",
                   DoubleValue(220),
                   MakeDoubleAccessor (&UavMobilityEnergyModel::SetMoveEnergyConversionFactor,
                                      &UavMobilityEnergyModel::GetMoveEnergyConversionFactor),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("HoverCurrent",
                   "Fixed amperage to hover. In amperes",
                   DoubleValue(210),
                   MakeDoubleAccessor (&UavMobilityEnergyModel::SetHoverCurrent,
                                      &UavMobilityEnergyModel::GetHoverCurrent),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("EnergySource",
                   "The energy source this model draws from",
                   PointerValue (),
                   MakePointerAccessor (&UavMobilityEnergyModel::SetEnergySource,
                                       &UavMobilityEnergyModel::GetEnergySource),
                   MakePointerChecker<EnergySource> ())
    .AddTraceSource ("EnergyDepleted",
                    "The attached energy source has indicated that "
                    "it is depleted",
                    MakeTraceSourceAccessor
                      (&UavMobilityEnergyModel::m_energyDepletionTrace),
                    "ns3::UavMobilityEnergyModel::BasicCallback")
    .AddTraceSource ("EnergyRecharged",
                    "The attached energy source has indicated that "
                    "it has been recharged",
                    MakeTraceSourceAccessor
                      (&UavMobilityEnergyModel::m_energyRechargedTrace),
                    "ns3::UavMobilityEnergyModel::BasicCallback")
    .AddTraceSource ("TotalEnergyConsumption",
                    "The total energy consumed by this model (in Joules)",
                    MakeTraceSourceAccessor
                      (&UavMobilityEnergyModel::m_totalEnergyConsumption),
                    "ns3::TracedValueCallback::Double")
    .AddTraceSource ("State",
                    "The current mobility state determined from the "
                    " attached Mobility Model, along with the new velocity",
                    MakeTraceSourceAccessor
                      (&UavMobilityEnergyModel::m_stateChangedTrace),
                    "ns3::UavMobilityEnergyModel::StateChangedCallback")
    .AddTraceSource ("Current",
                    "The current draw from the attached energy source. "
                    "Along with the current velocity",
                    MakeTraceSourceAccessor
                      (&UavMobilityEnergyModel::m_currentChangedTrace),
                    "ns3::UavMobilityEnergyModel::CurrentChangedCallback")
    ;
  return tid;
}

UavMobilityEnergyModel::UavMobilityEnergyModel (void)
{
  NS_LOG_FUNCTION (this);
}

void UavMobilityEnergyModel::Init (Ptr<Node> node, Ptr<EnergySource> energySource)
{
  NS_LOG_FUNCTION (this);

  // Create the two way relationship between us and the energy source
  SetEnergySource (energySource);

  // Attaches the CourseChange callback
  ConnectMobility (node);
}

UavMobilityEnergyModel::~UavMobilityEnergyModel (void)
{
  NS_LOG_FUNCTION (this);
}

void
UavMobilityEnergyModel::SetEnergySource(Ptr<EnergySource> source)
{
  NS_LOG_FUNCTION (this << source);
  source->AppendDeviceEnergyModel (this);
  m_source = source;
}

Ptr<EnergySource>
UavMobilityEnergyModel::GetEnergySource (void) const
{
  NS_LOG_FUNCTION (this);
  return m_source;
}

double 
UavMobilityEnergyModel::GetTotalEnergyConsumption (void) const
{
  NS_LOG_FUNCTION (this);
  Time deltaUpdate = Simulator::Now () - m_lastUpdateTime;

  double deltaEnergy = 
    deltaUpdate.GetSeconds () * m_current * m_source->GetSupplyVoltage ();

  // We can't actually preform an update from a const context
  // So just calculate the increase and return it
  return m_totalEnergyConsumption + deltaEnergy;
}

void
UavMobilityEnergyModel::ChangeState (int newState)
{
  NS_LOG_FUNCTION (this << newState);
  NS_LOG_WARN ("UavMobilityEnergyModel::ChangeState does nothing");
}

UavMobilityEnergyModel::State
UavMobilityEnergyModel::GetState (void) const
{
  NS_LOG_FUNCTION (this);
  return m_state;
}

void
UavMobilityEnergyModel::Stop (void)
{
  NS_LOG_FUNCTION (this);
  
  UpdateState (UavMobilityEnergyModel::State::STOP);
  UpdateCurrent (0);
}

void
UavMobilityEnergyModel::Ascend (double speed)
{
  NS_LOG_FUNCTION (this << speed);

  UpdateState (UavMobilityEnergyModel::State::ASCEND);
  UpdateCurrent (AscendCurrent (speed));

}

void
UavMobilityEnergyModel::Descend (double speed)
{
  NS_LOG_FUNCTION (this << speed);

  UpdateState (UavMobilityEnergyModel::State::DESCEND);
  UpdateCurrent (DescendCurrent (speed));
}

void
UavMobilityEnergyModel::Move (const Vector &velocity)
{
  NS_LOG_FUNCTION (this << velocity);
  double moveMagnitude = std::sqrt (velocity.x * velocity.x + 
                                   velocity.y * velocity.y);
  
  double newCurrent = MoveCurrent (moveMagnitude);
  if (velocity.z > 0)
      newCurrent += AscendCurrent (velocity.z);
  else if (velocity.z < 0)
      newCurrent += DescendCurrent (std::abs (velocity.z));

  UpdateState (UavMobilityEnergyModel::State::MOVE);
  UpdateCurrent (newCurrent);
}

void
UavMobilityEnergyModel::Hover (void)
{
  NS_LOG_FUNCTION (this);

  UpdateState (UavMobilityEnergyModel::State::HOVER);
  UpdateCurrent (m_hoverEnergyCurrent);
}

void
UavMobilityEnergyModel::HandleEnergyDepletion (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("UavMobilityEnergyModel: Energy depleted");
  
  m_energyDepletionTrace (this);
}

void
UavMobilityEnergyModel::HandleEnergyRecharged (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("UavMobilityEnergyModel: Energy recharged");

  m_energyRechargedTrace (this);
}

double
UavMobilityEnergyModel::GetAscendEnergyConversionFactor (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ascendEnergyConversionFactor;
}

void
UavMobilityEnergyModel::SetAscendEnergyConversionFactor (double ascendEnergyConversionFactor)
{
  NS_LOG_FUNCTION (this << ascendEnergyConversionFactor);
  m_ascendEnergyConversionFactor = ascendEnergyConversionFactor;

  // If this is the current state,
  // We'll have to update the presant current
  if (m_state == UavMobilityEnergyModel::State::ASCEND)
    {
      auto zSpeed = std::abs (m_mobility->GetVelocity ().z);
      UpdateCurrent (AscendCurrent (zSpeed));
    }
}

double
UavMobilityEnergyModel::GetDescendEnergyConversionFactor (void) const
{
  NS_LOG_FUNCTION (this);
  return m_descendEnergyConversionFactor;
}

void
UavMobilityEnergyModel::SetDescendEnergyConversionFactor (double descendEnergyConversionFactor)
{
  NS_LOG_FUNCTION (this << descendEnergyConversionFactor);
  m_descendEnergyConversionFactor = descendEnergyConversionFactor;

  // If this is the current state,
  // We'll have to update the presant current
  if (m_state == UavMobilityEnergyModel::State::DESCEND)
    {
      auto zSpeed = std::abs (m_mobility->GetVelocity ().z);
      UpdateCurrent (DescendCurrent (zSpeed));
    }
}

double
UavMobilityEnergyModel::GetMoveEnergyConversionFactor (void) const
{
  NS_LOG_FUNCTION (this);
  return m_moveEnergyConversionFactor;
}

void
UavMobilityEnergyModel::SetMoveEnergyConversionFactor (double moveEnergyConversionFactor)
{
  NS_LOG_FUNCTION (this << moveEnergyConversionFactor);
  m_moveEnergyConversionFactor = moveEnergyConversionFactor;

  // If this is the current state,
  // We'll have to update the presant current
  if (m_state == UavMobilityEnergyModel::State::MOVE)
    {
      Move (m_mobility->GetVelocity ());
    }
}

double
UavMobilityEnergyModel::GetHoverCurrent () const
{
  NS_LOG_FUNCTION (this);
  return m_hoverEnergyCurrent;
}

void
UavMobilityEnergyModel::SetHoverCurrent (double hoverEnergyCurrent)
{
  NS_LOG_FUNCTION (this << hoverEnergyCurrent);
  m_hoverEnergyCurrent = hoverEnergyCurrent;

  // If this is the current state,
  // We'll have to update the presant current
  if (m_state == UavMobilityEnergyModel::State::HOVER)
    {
      UpdateCurrent (m_hoverEnergyCurrent);
    }
}

void UavMobilityEnergyModel::ConnectMobility (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_mobility = node->GetObject<MobilityModel> ();
  NS_ABORT_MSG_IF (m_mobility == 0, "No MobilityModel attached to node");

  m_mobility->TraceConnectWithoutContext ("CourseChange",
    MakeCallback (&UavMobilityEnergyModel::CourseChangeCallback, this));

  // Set our initial state based on the given model
  CourseChangeCallback (m_mobility);
}

double
UavMobilityEnergyModel::DoGetCurrentA (void) const
{
  NS_LOG_FUNCTION (this);
  return m_current;
}

void
UavMobilityEnergyModel::DoDispose (void)
{
  m_mobility = 0;
  m_source = 0;
  // Implementations of DoDispose () are expected to chain
  // to their parrent, even though DeviceEnergyModel::DoDispose
  // is currently unimplemented
  DeviceEnergyModel::DoDispose ();
}

void
UavMobilityEnergyModel::UpdateCurrent (double current)
{
  NS_LOG_FUNCTION (this << current);

  // Only perform updates if the current has actually changed
  // Prevents calls to the model in Low/High energy handlers
  // calling the handlers again
  if (m_current != current) 
    {
      UpdateTotalEnergyConsumption ();
      m_source->UpdateEnergySource ();
      double oldCurrent = m_current;
      m_current = current;
      m_currentChangedTrace (oldCurrent, current, m_mobility->GetVelocity ());
    }
}

void
UavMobilityEnergyModel::CourseChangeCallback (Ptr<const MobilityModel> model)
{
  NS_LOG_FUNCTION (this << model);
  Vector velocity = model->GetVelocity ();
  Vector speed = {std::abs (velocity.x),
    std::abs (velocity.y),
    std::abs (velocity.z)};

  // normal, as in not 0, not NaN, not Infinite, etc.
  if (std::fpclassify (speed.x + speed.y) == FP_NORMAL)
      Move (velocity);
  // If we only have vertical movement
  else if (speed.z > 0)
    {
      if (velocity.z > 0)
        Ascend (speed.z);
      else
        Descend (speed.z);
    }
  // No movement at all
  else
    {
      // Not moving or hovering, So no energy should be consumed
      Vector position = model->GetPosition ();
      if (std::fpclassify (position.z) == FP_ZERO)
        Stop ();
      // if we have a non-zero z position, then we are hovering
      else
        Hover ();
    }
}

void
UavMobilityEnergyModel::UpdateState (UavMobilityEnergyModel::State newState)
{
  NS_LOG_FUNCTION (this << newState);
  UavMobilityEnergyModel::State oldState = m_state;
  m_state = newState;

  // Only notify if the state is different
  if (oldState != newState)
    m_stateChangedTrace (oldState, newState, m_mobility->GetVelocity ());
}

double
UavMobilityEnergyModel::AscendCurrent (double speed)
{
  NS_LOG_FUNCTION (this << speed);
  return m_ascendEnergyConversionFactor * speed;
}

double
UavMobilityEnergyModel::DescendCurrent (double speed)
{
  NS_LOG_FUNCTION (this << speed);

  // Range for speed is [0, inf)
  //
  // Handle speed at 0
  // even though it should be classified as
  // Hover/Stop
  if (speed > 0)
    {
      return m_descendEnergyConversionFactor * ( 1.0/speed );
    }
  // If speed is 0, then current should be zero
  return 0;
}

double
UavMobilityEnergyModel::MoveCurrent (double speed)
{
  NS_LOG_FUNCTION (this << speed);
  return m_moveEnergyConversionFactor * speed;
}

void
UavMobilityEnergyModel::UpdateTotalEnergyConsumption (void)
{
  NS_LOG_FUNCTION (this);
  Time deltaUpdate = Simulator::Now () - m_lastUpdateTime;

  double deltaEnergy = 
    deltaUpdate.GetSeconds () * m_current * m_source->GetSupplyVoltage ();
  
  m_totalEnergyConsumption += deltaEnergy;
  m_lastUpdateTime = Simulator::Now ();
}

} // namespace psc
} // namespace ns3
