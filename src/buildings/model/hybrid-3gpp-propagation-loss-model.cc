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
#include "ns3/outdoor-to-outdoor-propagation-loss-model.h"
#include "ns3/indoor-to-indoor-propagation-loss-model.h"
#include "ns3/outdoor-to-indoor-propagation-loss-model.h"
#include "ns3/urbanmacrocell-propagation-loss-model.h"
#include "ns3/mobility-building-info.h"
#include "ns3/enum.h"
#include <ns3/boolean.h>
#include "ns3/hybrid-3gpp-propagation-loss-model.h"
#include "ns3/building-list.h"
#include "ns3/buildings-helper.h"

NS_LOG_COMPONENT_DEFINE ("Hybrid3gppPropagationLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Hybrid3gppPropagationLossModel);


Hybrid3gppPropagationLossModel::Hybrid3gppPropagationLossModel ()
{
  m_indoorToIndoor = CreateObject<IndoorToIndoorPropagationLossModel> ();
  m_outdoorToOutdoor = CreateObject<OutdoorToOutdoorPropagationLossModel> ();
  m_outdoorToIndoor = CreateObject<OutdoorToIndoorPropagationLossModel> ();
  m_urbanMacroCell = CreateObject<UrbanMacroCellPropagationLossModel> ();
}

Hybrid3gppPropagationLossModel::~Hybrid3gppPropagationLossModel ()
{
}

TypeId
Hybrid3gppPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Hybrid3gppPropagationLossModel")

    .SetParent<BuildingsPropagationLossModel> ()
    .SetGroupName ("Buildings")

    .AddConstructor<Hybrid3gppPropagationLossModel> ()

    .AddAttribute ("Frequency",
                   "The propagation frequency in Hz",
                   DoubleValue (2106e6),
                   MakeDoubleAccessor (&Hybrid3gppPropagationLossModel::SetFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("HeightThreshold",
                   "If the height difference between Tx and Rx nodes is greater than this threshold in meters"
                   "mark it as macro cell communication.",
                   DoubleValue (5),
                   MakeDoubleAccessor (&Hybrid3gppPropagationLossModel::SetHeightThreshold),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("CacheLoss",
                   "True, if the loss value should be cached. False otherwise"
                   "This is used to cache the pathloss in wrap-around topology and to speed up"
                   "pathloss computation in large static scenarios",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Hybrid3gppPropagationLossModel::m_cacheLoss),
                   MakeBooleanChecker ())
    .AddAttribute ("ShadowingEnabled",
                   "Activate or deactivate the shadowing computation",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Hybrid3gppPropagationLossModel::EnableShadowing),
                   MakeBooleanChecker ())
    .AddTraceSource ("Hybrid3gppPathlossValue",
                     "Pathloss value to trace",
                     MakeTraceSourceAccessor (&Hybrid3gppPropagationLossModel::m_hybrid3gppPathlossTrace),
                     "ns3::Hybrid3gppPropagationLossModel::Hybrid3gppPathlossValueTracedCallback")

  ;
  return tid;
}

void
Hybrid3gppPropagationLossModel::SetFrequency (double freq)
{
  NS_LOG_FUNCTION (this);
  m_indoorToIndoor->SetAttribute ("Frequency", DoubleValue (freq));
  m_outdoorToOutdoor->SetAttribute ("Frequency", DoubleValue (freq));
  m_outdoorToIndoor->SetAttribute ("Frequency", DoubleValue (freq));
  m_urbanMacroCell->SetAttribute ("Frequency", DoubleValue (freq));
  m_frequency = freq;
}

void
Hybrid3gppPropagationLossModel::SetHeightThreshold (double threshold)
{
  NS_LOG_FUNCTION (this << threshold);
  m_heightThreshold = threshold;
}

void
Hybrid3gppPropagationLossModel::EnableShadowing (bool enableShadowing)
{
  NS_LOG_FUNCTION (this << enableShadowing);
  m_isShadowingEnabled = enableShadowing;
  m_urbanMacroCell->EnableShadowing (enableShadowing);
}


bool
Hybrid3gppPropagationLossModel::IsMacroComm (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  double heightDiff = std::abs (a->GetPosition ().z - b->GetPosition ().z);
  m_isMacroComm = (heightDiff > m_heightThreshold ? true : false);
  NS_LOG_DEBUG ("Height difference between a and b: " << heightDiff);
  return m_isMacroComm;
}

double
Hybrid3gppPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF ((a->GetPosition ().z < 0) || (b->GetPosition ().z < 0),
                               "Hybrid3gppsPropagationLossModel does not support underground nodes (placed at z < 0)");


  //Check if loss value is cached
  double loss = 0.0;
  MobilityDuo couple;
  couple.a = a;
  couple.b = b;
  std::map<MobilityDuo, double>::iterator it_a = m_lossMap.find (couple);
  BuildingList::Iterator buildingsIt = BuildingList::Begin ();
  bool aIndoor = false;
  bool bIndoor = false;

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
          if (buildingsIt != BuildingList::End ())
            {
              // Get the MobilityBuildingInfo pointers
              Ptr<MobilityBuildingInfo> a1 = a->GetObject<MobilityBuildingInfo> ();
              Ptr<MobilityBuildingInfo> b1 = b->GetObject<MobilityBuildingInfo> ();
              NS_ABORT_MSG_IF ((a1 == 0) || (b1 == 0), "Hybrid3gppsPropagationLossModel only works with MobilityBuildingInfo");
              Vector vA = a->GetVelocity ();
              Vector vB = b->GetVelocity ();
              bool isAStatic = (vA.x == 0.0 && vA.y == 0.0 ? true : false);
              bool isBStatic = (vB.x == 0.0 && vB.y == 0.0 ? true : false);
              if (!isAStatic)
                {
                  // To tackle a case, when there are buildings and nodes have mobility,
                  // we update the info whether the node position falls inside or outside of any of the building.
                  a1->MakeConsistent (a);
                }

              if (!isBStatic)
                {
                  // To tackle a case, when there are buildings and nodes have mobility,
                  // we update the info whether the node position falls inside or outside of any of the building.
                  b1->MakeConsistent (b);
                }

              aIndoor = a1->IsIndoor ();
              bIndoor = b1->IsIndoor ();
            }

          // Verify if it is a D2D communication (UE-UE) or LTE communication (eNB-UE)
          // LTE
          if (IsMacroComm (a,b))
            {
              NS_LOG_DEBUG ("LTE comm, Height difference between a and b is greater than the threshold. Threshold value: " << m_heightThreshold);
              loss = UrbanMacroCell (a,b);
            }
          //D2D
          else
            {
              NS_LOG_DEBUG ("D2D comm, Height difference between a and b is less than the threshold. Threshold value: " << m_heightThreshold);
              // Calculate the pathloss based on the position of the nodes (outdoor/indoor)
              // a outdoor
              if (!aIndoor)
                {
                  // b outdoor
                  if (!bIndoor)
                    {
                      // Outdoor transmission
                      loss = OutdoorToOutdoor (a, b);
                      NS_LOG_INFO (this << " Outdoor to outdoor : " << loss);
                    }

                  // b indoor
                  else
                    {
                      loss = OutdoorToIndoor (a, b);
                      NS_LOG_INFO (this << " Outdoor to indoor : " << loss);
                    }
                }

              // a is indoor
              else
                {
                  // b is indoor
                  if (bIndoor)
                    {
                      loss = IndoorToIndoor (a, b).first;
                      NS_LOG_INFO (this << " Indoor to indoor : " << loss );
                    }

                  // b is outdoor
                  else
                    {
                      loss = OutdoorToIndoor (a, b);
                      NS_LOG_INFO (this << " Outdoor to indoor : " << loss);
                    }
                }
            }

          loss = std::max (loss, 0.0);
          if (m_cacheLoss)
            {
              m_lossMap[couple] = loss; //cache value
            }
        }
    }

  m_hybrid3gppPathlossTrace (loss, a->GetObject<Node> (), b->GetObject<Node> (), a->GetDistanceFrom (b), aIndoor, bIndoor);
  NS_LOG_DEBUG ("size of m_lossMap : " << m_lossMap.size ());

  return loss;
}


double
Hybrid3gppPropagationLossModel::GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
const
{
  NS_LOG_FUNCTION (this);
  if (m_isShadowingEnabled)
    {
      Ptr<MobilityBuildingInfo> a1 = a->GetObject <MobilityBuildingInfo> ();
      Ptr<MobilityBuildingInfo> b1 = b->GetObject <MobilityBuildingInfo> ();
      NS_ABORT_MSG_IF ((a1 == 0) || (b1 == 0), "Hybrid3gppsPropagationLossModel only works with MobilityBuildingInfo");

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
              double shadowingValue = m_randVariable->GetValue (0.0, (sigma * sigma));
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
          double shadowingValue = m_randVariable->GetValue (0.0, (sigma * sigma));
          m_shadowingLossMap[a][b] = shadowingValue;
          m_shadowingLossMap[b][a] = shadowingValue;
          return (shadowingValue);
        }
    }

  return 0.0;
}

double
Hybrid3gppPropagationLossModel::EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const
{
  NS_LOG_FUNCTION (this);
  //LTE
  if (m_isMacroComm)
    {
      NS_LOG_DEBUG ("Evaluating shadowing for urban macro cell");
      return m_urbanMacroCell->EvaluateSigma (a,b);
    }
  //D2D
  else
    {
      NS_LOG_DEBUG ("Evaluating shadowing for D2D scenario");
      if (!a->IsIndoor ())
        {
          if (!b->IsIndoor ())
            {
              // Outdoor to outdoor
              return 7;
            }
          else
            {
              // Outdoor to indoor
              return 7;
            }
        }
      else
      if (b->IsIndoor ())
        {
          if (a->GetBuilding () == b->GetBuilding ())
            {
              if (IndoorToIndoor (a->GetObject<MobilityModel> (), b->GetObject<MobilityModel> ()).second)
                {
                  // Indoor to indoor same building LOS
                  return 3;
                }
              else
                {
                  // Indoor to indoor same building NLOS
                  return 4;
                }
            }
          else
            {
              // Indoor to indoor different buildings
              return 10;
            }
        }
      else
        {
          // Outdoor to indoor
          return 7;
        }
    }

  return 0;
}

std::pair<double, bool>
Hybrid3gppPropagationLossModel::IndoorToIndoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_indoorToIndoor->GetLoss (a, b);
}

double
Hybrid3gppPropagationLossModel::OutdoorToOutdoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_outdoorToOutdoor->GetLoss (a, b);
}

double
Hybrid3gppPropagationLossModel::OutdoorToIndoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_outdoorToIndoor->GetLoss (a, b);
}

double
Hybrid3gppPropagationLossModel::UrbanMacroCell (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this);
  return m_urbanMacroCell->GetLoss (a, b);
}

} // namespace ns3
