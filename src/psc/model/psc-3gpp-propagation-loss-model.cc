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
#include "ns3/mobility-model.h"
#include "ns3/double.h"
#include "ns3/psc-outdoor-propagation-loss-model.h"
#include "ns3/psc-indoor-propagation-loss-model.h"
#include "ns3/psc-hybrid-propagation-loss-model.h"
#include "ns3/psc-urbanmacrocell-propagation-loss-model.h"
#include "ns3/mobility-building-info.h"
#include "ns3/enum.h"
#include <ns3/boolean.h>
#include "ns3/psc-3gpp-propagation-loss-model.h"
#include "ns3/building-list.h"

NS_LOG_COMPONENT_DEFINE ("Psc3gppPropagationLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Psc3gppPropagationLossModel);


Psc3gppPropagationLossModel::Psc3gppPropagationLossModel ()
{
  m_indoor = CreateObject<PscIndoorPropagationLossModel> ();
  m_outdoor = CreateObject<PscOutdoorPropagationLossModel> ();
  m_hybrid = CreateObject<PscHybridPropagationLossModel> ();
  m_urbanmacrocell = CreateObject<PscUrbanmacrocellPropagationLossModel> ();
}

Psc3gppPropagationLossModel::~Psc3gppPropagationLossModel ()
{
}

TypeId
Psc3gppPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Psc3gppPropagationLossModel")
    
    .SetParent<BuildingsPropagationLossModel> ()
    
    .AddConstructor<Psc3gppPropagationLossModel> ()
    
    .AddAttribute ("Frequency",
                   "The propagation frequency  (default : general cases 2100MHz band).",
                   DoubleValue (2106e6),
                   MakeDoubleAccessor (&Psc3gppPropagationLossModel::SetFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("HeightThreshold",
                   "If the height difference between Tx node and Rx node is greater than this threshold, mark it as macro cell communication.",
                   DoubleValue (5),
                   MakeDoubleAccessor (&Psc3gppPropagationLossModel::SetHeightThreshold),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("CacheLoss",
                   "Indicates if the loss value should be cached",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Psc3gppPropagationLossModel::m_cacheLoss),
                   MakeBooleanChecker ())    
    .AddTraceSource ("PscPathlossValue",
                     "Pathloss value to trace",
                     MakeTraceSourceAccessor (&Psc3gppPropagationLossModel::m_pscPathlossTrace),
                     "ns3::Psc3gppPropagationLossModel::PscPathlossValueTracedCallback")    
    
  ;
  return tid;
}

void
Psc3gppPropagationLossModel::SetFrequency (double freq)
{
  NS_LOG_FUNCTION (this);
  m_indoor->SetAttribute ("Frequency", DoubleValue (freq));
  m_outdoor->SetAttribute ("Frequency", DoubleValue (freq));
  m_hybrid->SetAttribute ("Frequency", DoubleValue (freq));
  m_urbanmacrocell->SetAttribute ("Frequency", DoubleValue (freq));
  m_frequency = freq;
}

void
Psc3gppPropagationLossModel::SetHeightThreshold (double thold)
{
  NS_LOG_FUNCTION (this << thold);
  m_heightThold = thold;
}

bool 
Psc3gppPropagationLossModel::IsMacroComm (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
 NS_LOG_FUNCTION (this);
 double heightDiff = std::abs(a->GetPosition ().z - b->GetPosition ().z);
 m_isMacroComm = (heightDiff > m_heightThold ? true : false);
 NS_LOG_DEBUG("Height difference between a and b: "<<heightDiff);
 return m_isMacroComm;
}

double
Psc3gppPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG ((a->GetPosition ().z >= 0) && (b->GetPosition ().z >= 0), "Psc3gppsPropagationLossModel does not support underground nodes (placed at z < 0)");

  //Check if loss value is cached
  double loss = 0.0;
  MobilityDuo couple;
  couple.a = a;
  couple.b = b;
  std::map<MobilityDuo, double>::iterator it_a = m_lossMap.find (couple);
  if (it_a != m_lossMap.end ())
    {
      loss = it_a->second;
    }
  else
    {
      couple.a = b;
      couple.b = a;
      std::map<MobilityDuo, double>::iterator it_b = m_lossMap.find (couple);
      if (it_b != m_lossMap.end ())
        {
          loss = it_b->second;
        }
      else
        {
          // Get the MobilityBuildingInfo pointers
          Ptr<MobilityBuildingInfo> a1 = a->GetObject<MobilityBuildingInfo> ();
          Ptr<MobilityBuildingInfo> b1 = b->GetObject<MobilityBuildingInfo> ();
          NS_ASSERT_MSG ((a1 != 0) && (b1 != 0), "Psc3gppsPropagationLossModel only works with MobilityBuildingInfo");

          //double loss = 0.0;
          Vector aPos = a->GetPosition ();
          Vector bPos = b->GetPosition ();

          bool aIndoor = false;
          bool bIndoor = false;

          // Go through the different buildings in the simulation 
          // and check if the nodes are outdoor or indoor
          for (BuildingList::Iterator bit = BuildingList::Begin (); bit != BuildingList::End (); ++bit)
            {
              NS_LOG_LOGIC ("checking building " << (*bit)->GetId () << " with boundaries " << (*bit)->GetBoundaries ());
              if ((*bit)->IsInside (aPos))
                {
                  aIndoor = true;
                  uint16_t floor = (*bit)->GetFloor (aPos);
                  uint16_t roomX = (*bit)->GetRoomX (aPos);
                  uint16_t roomY = (*bit)->GetRoomY (aPos);     
                  a1->SetIndoor (*bit, floor, roomX, roomY);       
                }
              if ((*bit)->IsInside (bPos))
                {
                  bIndoor = true;
                  uint16_t floor = (*bit)->GetFloor (bPos);
                  uint16_t roomX = (*bit)->GetRoomX (bPos);
                  uint16_t roomY = (*bit)->GetRoomY (bPos);     
                  b1->SetIndoor (*bit, floor, roomX, roomY);  
                }
            }

          // Verify if it is a D2D communication (UE-UE) or LTE communication (eNB-UE)
          // LTE
            if (IsMacroComm(a,b))
            {
              NS_LOG_DEBUG("LTE comm, Height difference between a and b is greater than the threshold. Threshold value: "<<m_heightThold);
              loss = Urbanmacrocell (a,b);
            }
          //D2D
          else
            {
              NS_LOG_DEBUG("D2D comm, Height difference between a and b is less than the threshold. Threshold value: "<<m_heightThold);
              // Calculate the pathloss based on the position of the nodes (outdoor/indoor)
              // a outdoor
              if (!aIndoor)
                {
                  // b outddor
                  if (!bIndoor)
                    {
                      // Outdoor tranmission 
                      loss = Outdoor (a, b);
                      NS_LOG_INFO (this << " Outdoor : " << loss);
                    }

                  // b indoor
                  else
                    {
                      loss = Hybrid (a, b);
                      NS_LOG_INFO (this << " Hybrid : " << loss);
                    }
                } 

              // a is indoor
              else
                {
                  // b is indoor
                  if (bIndoor)
                    {
                      loss = Indoor (a, b).first;
                      //m_los = Indoor (a, b).second;
                      NS_LOG_INFO (this << " Inddor : " << loss );  
                    }

                  // b is outdoor
                  else
                    {
                      loss = Hybrid (a, b);
                      NS_LOG_INFO (this << " Hybrid : " << loss);
                    } 
                }
            }
          
          loss = std::max (loss, 0.0);
          if (m_cacheLoss)
            {
              m_lossMap[couple] = loss; //cache value
            }
          m_pscPathlossTrace (loss, a->GetObject<Node> (), b->GetObject<Node> (), a->GetDistanceFrom (b), aIndoor, bIndoor);
        }      
    }

  
  return loss;
}


double
Psc3gppPropagationLossModel::GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
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
Psc3gppPropagationLossModel::EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const
{
  NS_LOG_FUNCTION (this);
  //LTE
  if (m_isMacroComm)
  {
    NS_LOG_DEBUG("Evaluating shadowing for urban macro cell");
    return m_urbanmacrocell->EvaluateSigma (a,b);
  }
  //D2D
  else
  {
    NS_LOG_DEBUG("Evaluating shadowing for D2D scenario");
    if (a->IsOutdoor ())
    {
      if (b->IsOutdoor ())
      {
        // Outdoor
        return 7;
      }
      else
      {
        // Hybrid
        return 7;
      }
    }
    else
    if (b->IsIndoor ())
    {
      if (a->GetBuilding () == b->GetBuilding ())
      {
        //if (m_los)
        if (Indoor (a->GetObject<MobilityModel> (), b->GetObject<MobilityModel> ()).second)
        {
          // Indoor Same building LOS
          return 3; 
        }
        else
        {
          // Indoor Same building NLOS
          return 4;
        }
      }
      else
      {
        // Indoor Different Buildings
        return 10;
      }    
    }
    else
    {
      // Hybrid
      return 7;
    }
  }
  
  return 0;
}

std::pair<double, bool>
Psc3gppPropagationLossModel::Indoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_indoor->GetLoss (a, b);
}

double
Psc3gppPropagationLossModel::Outdoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_outdoor->GetLoss (a, b);
}

double
Psc3gppPropagationLossModel::Hybrid (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_hybrid->GetLoss (a, b);
}

double
Psc3gppPropagationLossModel::Urbanmacrocell (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_urbanmacrocell->GetLoss (a, b);
}


} // namespace ns3
