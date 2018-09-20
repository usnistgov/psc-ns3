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
#include "scm-urbanmacrocell-propagation-loss-model.h"
#include <ns3/node.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/boolean.h>

NS_LOG_COMPONENT_DEFINE ("ScmUrbanMacroCellPropagationLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ScmUrbanMacroCellPropagationLossModel);


TypeId
ScmUrbanMacroCellPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ScmUrbanMacroCellPropagationLossModel")

    .SetParent<BuildingsPropagationLossModel> ()
    .SetGroupName ("Buildings")

    .AddConstructor<ScmUrbanMacroCellPropagationLossModel> ()

    .AddAttribute ("Frequency",
                   "The propagation frequency in Hz",
                   DoubleValue (700e6),
                   MakeDoubleAccessor (&ScmUrbanMacroCellPropagationLossModel::m_frequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("ShadowingEnabled",
                   "Activate or deactivate the shadowing computation",
                   BooleanValue (true),
                   MakeBooleanAccessor (&ScmUrbanMacroCellPropagationLossModel::m_isShadowingEnabled),
                   MakeBooleanChecker ())
  ;

  return tid;
}

ScmUrbanMacroCellPropagationLossModel::ScmUrbanMacroCellPropagationLossModel ()
  : BuildingsPropagationLossModel ()
{
  NS_LOG_FUNCTION (this);
  m_rand = CreateObject<UniformRandomVariable> ();
}

ScmUrbanMacroCellPropagationLossModel::~ScmUrbanMacroCellPropagationLossModel ()
{
}

double
ScmUrbanMacroCellPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
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

  /*********************************************/
  //3GPP TR 25.996 v6.1.0
  //non-LOS condition (low probability of occurrence)
  //hbs : BS antenna height in meter
  //hms : MS antenna height in meter
  //fc*1000 : frequency in MHz
  //dist : distance between BS and MS in meter
  loss = ((44.9 - 6.55 * std::log10 (hbs)) * std::log10 (dist / 1000)) + 45.5 + ((35.46 - (1.1 * hms)) * std::log10 (fc * 1000)) - (13.82 * std::log10 (hbs)) + (0.7 * hms) + 3;
  /*********************************************/

  return std::max (0.0, loss);
}


double
ScmUrbanMacroCellPropagationLossModel::GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
const
{
  NS_LOG_FUNCTION (this);

  Ptr<MobilityBuildingInfo> a1 = a->GetObject <MobilityBuildingInfo> ();
  Ptr<MobilityBuildingInfo> b1 = b->GetObject <MobilityBuildingInfo> ();
  NS_ABORT_MSG_IF ((a1 == 0) || (b1 == 0), "ScmUrbanMacroCellPropagationLossModel only works with MobilityBuildingInfo");

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


double
ScmUrbanMacroCellPropagationLossModel::EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const
{
  NS_LOG_FUNCTION (this);

  if (m_isShadowingEnabled)
    {
      //3GPP TR 25.996 v6.1.0
      return 8.0;
    }

  return 0.0;
}

int64_t
ScmUrbanMacroCellPropagationLossModel::DoAssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_rand->SetStream (stream);
  return 1;
}


} // namespace ns3
