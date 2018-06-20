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

#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/mobility-model.h"
#include <cmath>
#include <ns3/mobility-building-info.h>
#include "psc-indoor-propagation-loss-model.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("PscIndoorPropagationLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PscIndoorPropagationLossModel);

TypeId
PscIndoorPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PscIndoorPropagationLossModel")
    .SetParent<PropagationLossModel> ()
    .AddConstructor<PscIndoorPropagationLossModel> ()
    .AddAttribute ("Frequency",
                    "The propagation frequency in Hz",
                    DoubleValue (763e6),
                    MakeDoubleAccessor (&PscIndoorPropagationLossModel::m_frequency),
                    MakeDoubleChecker<double> ())
    ;

  return tid;
}

PscIndoorPropagationLossModel::PscIndoorPropagationLossModel ()
  : PropagationLossModel ()
{ 
  m_rand = CreateObject<UniformRandomVariable> ();
}

PscIndoorPropagationLossModel::~PscIndoorPropagationLossModel ()
{
}

// Initialize the static member
//Ptr <UniformRandomVariable> PscRandomVariable::s_rand = CreateObject<UniformRandomVariable> ();

std::pair<double, bool> 
PscIndoorPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  bool los = false;
  double loss = 0.0;
  double dist = a->GetDistanceFrom (b);
  // Calculate the pathloss based on 3GPP specifications : 3GPP TR 36.843 V12.0.1
  // The indoor model is defined by 3GPP TR 36.814 V9.0.0, Table A.2.1.1.5-1

  // Same building
  if (a->GetObject<MobilityBuildingInfo> ()->GetBuilding () == b->GetObject<MobilityBuildingInfo> ()->GetBuilding ())
  {
    // Computing the probability of line of sight (LOS)
    double plos = 0.0;
    if (dist <= 18)
    {
      plos = 1.0;
    }
    else if ((dist > 18) and (dist < 37))
    {
      plos = std::exp (-(dist - 18) / 27);
    }
    else 
    {
      plos = 0.5;
    }
  
    // Generate a random number between 0 and 1 (if it doesn't already exist) to evaluate the LOS/NLOS situation
    double r = 0.0;
    MobilityDuo couple;
    couple.a = a;
    couple.b = b;
    std::map<MobilityDuo, double>::iterator it_a = m_randomMap.find (couple);
    if (it_a != m_randomMap.end ())
    {
      r = it_a->second;
    }
    else
    {
      couple.a = b;
      couple.b = a;
      std::map<MobilityDuo, double>::iterator it_b = m_randomMap.find (couple);
      if (it_b != m_randomMap.end ())
      {
        r = it_b->second;
      }
      else
      {
        m_randomMap[couple] = m_rand->GetValue (0,1);
        r = m_randomMap[couple];
      }
    }
    
    //std::cout << "indoor = " << r << " - ";
    // Computing the pathloss when the two nodes are in the same building
    
    if (r <= plos)
    {
      // LOS
      loss = 89.5 + 16.9 * std::log10 (dist * 1e-3);
      los = true;
      NS_LOG_INFO (this << "Indoor (Same building) LOS = " << loss);
    }
    else
    {
      // NLOS
      loss = 147.5 + 43.3 * std::log10 (dist * 1e-3);
      NS_LOG_INFO (this << "Indoor (Same building) NLOS = " << loss);
    }
  }

  // Different buildings
  else
  {
    // Computing the pathloss when the two nodes are in different buildings
    loss = std::max (131.1 + 42.8 * std::log10 (dist * 1e3), 147.4 + 43.3 * std::log10 (dist * 1e3));
    NS_LOG_INFO (this << "Indoor (Different buildings) = " << loss);
  }

  // Pathloss correction for Public Safety frequency 700 MHz
  if (((m_frequency / 1e9) >= 0.758) and ((m_frequency / 1e9) <= 0.798))
  {
    loss = loss + 20 * std::log10 (m_frequency / 2e9);
  }

  return std::make_pair(std::max (0.0, loss), los);
}

double 
PscIndoorPropagationLossModel::DoCalcRxPower (double txPowerDbm,
					       Ptr<MobilityModel> a,
					       Ptr<MobilityModel> b) const
{
  return (txPowerDbm - GetLoss (a, b).first);
}

int64_t
PscIndoorPropagationLossModel::DoAssignStreams (int64_t stream)
{
  return 0;
}


} // namespace ns3

