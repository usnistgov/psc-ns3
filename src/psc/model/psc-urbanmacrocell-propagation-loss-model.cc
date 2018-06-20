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
#include <ns3/boolean.h>
#include "ns3/mobility-model.h"
#include <cmath>
#include "psc-urbanmacrocell-propagation-loss-model.h"
#include <ns3/node.h>

NS_LOG_COMPONENT_DEFINE ("PscUrbanmacrocellPropagationLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PscUrbanmacrocellPropagationLossModel);


TypeId
PscUrbanmacrocellPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PscUrbanmacrocellPropagationLossModel")
    .SetParent<BuildingsPropagationLossModel> ()
    .AddConstructor<PscUrbanmacrocellPropagationLossModel> ()
    .AddAttribute ("Frequency",
                    "The propagation frequency in Hz",
                    DoubleValue (700e6),
                    MakeDoubleAccessor (&PscUrbanmacrocellPropagationLossModel::m_frequency),
                    MakeDoubleChecker<double> ())
    .AddAttribute ("BuildingHeight",
                    "The average building heigth",
                    DoubleValue (20),
                    MakeDoubleAccessor (&PscUrbanmacrocellPropagationLossModel::m_buildingHeight),
                    MakeDoubleChecker<double> ())
    .AddAttribute ("StreetWidth",
                    "The average street width",
                    DoubleValue (20),
                    MakeDoubleAccessor (&PscUrbanmacrocellPropagationLossModel::m_streetWidth),
                    MakeDoubleChecker<double> ())    
    .AddAttribute ("ShadowingEnabled",
                   "Activate or deactivate the shadowing computation",
                   BooleanValue (true),
                   MakeBooleanAccessor (&PscUrbanmacrocellPropagationLossModel::m_isShadowingEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("LosEnabled",
                    "when Line-of-Sight is enabled, all UEs are considered in Los with eNB. Otherwise, use randomness",
                    BooleanValue (false),
                    MakeBooleanAccessor (&PscUrbanmacrocellPropagationLossModel::m_isLosEnabled),
                    MakeBooleanChecker ())
    .AddTraceSource ("LineOfSight",
                    "We get the transmitter mobility, receiver mobility, pathloss, shadowing, plos, r",
                    MakeTraceSourceAccessor (&PscUrbanmacrocellPropagationLossModel::m_losTrace),
                    "ns3::SpectrumChannel::LosTracedCallback")
    ;

  return tid;
}

PscUrbanmacrocellPropagationLossModel::PscUrbanmacrocellPropagationLossModel ()
  : BuildingsPropagationLossModel ()
{  
  NS_LOG_FUNCTION (this);
  m_rand = CreateObject<UniformRandomVariable> ();
}

PscUrbanmacrocellPropagationLossModel::~PscUrbanmacrocellPropagationLossModel ()
{
}

double
PscUrbanmacrocellPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  // Pathloss
  double loss = 0.0;
  // Frequency in GHz
  double fc = m_frequency / 1e9;
  // Distance between the two nodes in meter
  double dist = a->GetDistanceFrom (b);

  // Actual antenna heights
  double hms = 0;
  double hbs = 0;

  hbs = (a->GetPosition ().z > b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z); 
  hms = (a->GetPosition ().z < b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z); 

  // Effective antenna heights
  double hbs1 = hbs - 1;
  double hms1 = hms - 1;
  // Propagation velocity in free space
  double c = 3 * std::pow (10, 8);

  double d1 = 4 * hbs1 * hms1 * m_frequency * (1 / c);

  // Calculate the LOS probability based on 3GPP specifications 
  // 3GPP TR 36.814 channel model for Urban Macrocell scenario (UMa) : Table B.1.2.1-2
  double plos = std::min ((18 / dist), 1.0) * (1 - std::exp (-dist / 63)) + std::exp (-dist / 63);

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

  // Compute the pathloss based on 3GPP specifications
  // 3GPP TR 36.814 channel model for Urban Macrocell scenario (UMa) : Table B.1.2.1-1
  // This model is only valid to a minimum distance of 10 meters 
  if (dist >= 10)
  {
    if ((r <= plos) or (m_isLosEnabled)) 
    {
      // LOS
      if (dist <= d1)
      {
        loss = 22 * std::log10 (dist) + 28.0 + 20.0 * std::log10 (fc);
        NS_LOG_INFO (this << "Urban macrocell LOS (Distance <= " << d1 << ") : the pathloss = " << loss);
      }
      else
      {
        loss = 40.0 * std::log10 (dist) + 7.8 - 18.0 * std::log10 (hbs1) - 18.0 * std::log10 (hms1) + 2.0 * std::log10 (fc);
        NS_LOG_INFO (this << "Urban macrocell LOS (Distance > " << d1 << ") : the pathloss = " << loss);
      }
    }
    else 
    {
      // NLOS
      loss = 161.04 - 7.1 * std::log10 (m_streetWidth) + 7.5 * std::log10 (m_buildingHeight) - (24.37 - 3.7 * (std::pow (m_buildingHeight/hbs, 2)) * std::log10 (hbs)) + (43.42 + 3.1 * std::log10 (hbs))* (std::log10 (dist) - 3) + 20.0 * std::log10 (fc) - (3.2 * std::pow(std::log10 (11.75 * hms), 2) - 4.97);
      NS_LOG_INFO (this << "Urban macrocell NLOS, the pathloss = " << loss);
    }  
  }
    
  m_losTrace (a, b, std::max (0.0, loss),  GetShadowing(a,b), plos, r);

  return std::max (0.0, loss);
}

PscUrbanmacrocellPropagationLossModel::PathlossType
PscUrbanmacrocellPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b, bool check)
{
  NS_LOG_FUNCTION (this);
  // Frequency in GHz
  double fc = m_frequency / 1e9;
  // Distance between the two nodes in meter
  double dist = a->GetDistanceFrom (b);

  // Actual antenna heights
  double hms = 0;
  double hbs = 0;

  hbs = (a->GetPosition ().z > b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z); 
  hms = (a->GetPosition ().z < b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z);

  // Effective antenna heights
  double hbs1 = hbs - 1;
  double hms1 = hms - 1;
  // Propagation velocity in free space
  double c = 3 * std::pow (10, 8);

  double d1 = 4 * hbs1 * hms1 * m_frequency * (1 / c);

  // Calculate the LOS probability based on 3GPP specifications 
  // 3GPP TR 36.814 channel model for Urban Macrocell scenario (UMa) : Table B.1.2.1-2
  double plos = std::min ((18 / dist), 1.0) * (1 - std::exp (-dist / 63)) + std::exp (-dist / 63);

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
  
  // Compute the pathloss based on 3GPP specifications
  // 3GPP TR 36.814 channel model for Urban Macrocell scenario (UMa) : Table B.1.2.1-1
  // This model is only valid to a minimum distance of 10 meters 
  
  // Pathloss
  double loss = 0.0;

  if (dist >= 10)
  {
    if ((r <= plos) or (m_isLosEnabled)) 
    {
      // LOS
      if (dist <= d1)
      {
        loss = 22 * std::log10 (dist) + 28.0 + 20.0 * std::log10 (fc);
        NS_LOG_INFO (this << "Urban macrocell LOS (Distance <= " << d1 << ") : the pathloss = " << loss);
      }
      else
      {
        loss = 40.0 * std::log10 (dist) + 7.8 - 18.0 * std::log10 (hbs1) - 18.0 * std::log10 (hms1) + 2.0 * std::log10 (fc);
        NS_LOG_INFO (this << "Urban macrocell LOS (Distance > " << d1 << ") : the pathloss = " << loss);
      }
    }
    else 
    {
      // NLOS
      loss = 161.04 - 7.1 * std::log10 (m_streetWidth) + 7.5 * std::log10 (m_buildingHeight) - (24.37 - 3.7 * (std::pow (m_buildingHeight/hbs, 2)) * std::log10 (hbs)) + (43.42 + 3.1 * std::log10 (hbs))* (std::log10 (dist) - 3) + 20.0 * std::log10 (fc) - (3.2 * std::pow(std::log10 (11.75 * hms), 2) - 4.97);
      NS_LOG_INFO (this << "Urban macrocell NLOS, the pathloss = " << loss);
    }  
  }
  
  PathlossType triplet;
  triplet.loss = std::max (0.0, loss);
  triplet.plos = plos;
  triplet.r = r;
  m_losTrace (a, b, std::max (0.0, loss), GetShadowing(a,b), plos, r);
  return triplet;
}


double
PscUrbanmacrocellPropagationLossModel::GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
const
{
    NS_LOG_FUNCTION (this);
    Ptr<MobilityBuildingInfo> a1 = a->GetObject <MobilityBuildingInfo> ();
    Ptr<MobilityBuildingInfo> b1 = b->GetObject <MobilityBuildingInfo> ();
    NS_ASSERT_MSG ((a1 != 0) && (b1 != 0), "BuildingsPropagationLossModel only works with MobilityBuildingInfo");
  
  std::map<Ptr<MobilityModel>,  std::map<Ptr<MobilityModel>, double> >::iterator ait = m_shadowingLossMap.find (a);
  if (ait != m_shadowingLossMap.end ())
    {
      std::map<Ptr<MobilityModel>, double>::iterator bit = ait->second.find (b);
      if (bit != ait->second.end ())
        {
          return (bit->second);
        }
      else
        {
          double sigma = EvaluateSigma (a1, b1);
          // side effect: will create new entry          
          // sigma is standard deviation, not variance
          double shadowingValue = m_randVariable->GetValue (0.0, (sigma*sigma));
          ait->second[b] = shadowingValue;
          m_shadowingLossMap[b][a] = shadowingValue;
          return (shadowingValue);
        }
    }
  else
    {
      double sigma = EvaluateSigma (a1, b1);
      // side effect: will create new entries in both maps
      // sigma is standard deviation, not variance
      double shadowingValue = m_randVariable->GetValue (0.0, (sigma*sigma));
      m_shadowingLossMap[a][b] = shadowingValue; 
      m_shadowingLossMap[b][a] = shadowingValue;
      return (shadowingValue);       
    }
}


double
PscUrbanmacrocellPropagationLossModel::EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const
{
  NS_LOG_FUNCTION (this);
  if (m_isShadowingEnabled)
  {
    Ptr<MobilityModel> a1 = a->GetObject<MobilityModel> ();
    Ptr<MobilityModel> b1 = b->GetObject<MobilityModel> ();
    double dist = a1->GetDistanceFrom (b1);
    double plos = std::min ((18 / dist), 1.0) * (1 - std::exp (-dist / 63)) + std::exp (-dist / 63);
    double r = 0.0;
  
    MobilityDuo couple;
    couple.a = a1;
    couple.b = b1;
    std::map<MobilityDuo, double>::iterator it_a = m_randomMap.find (couple);
    if (it_a != m_randomMap.end ())
    {
      r = it_a->second;
    }
    else
    {
      couple.a = b1;
      couple.b = a1;
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
    if ((r <= plos) or (m_isLosEnabled))
      {
        // LOS
        return 4.0;
      }
    else 
      {
        // NLOS
        return 6.0;
      }  
  }
  return 0.0;
}

int64_t
PscUrbanmacrocellPropagationLossModel::DoAssignStreams (int64_t stream)
{
  return 0;
}


} // namespace ns3
