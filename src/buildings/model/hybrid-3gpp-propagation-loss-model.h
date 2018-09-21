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

#ifndef HYBRID_3GPP_PROPAGATION_LOSS_MODEL_H
#define HYBRID_3GPP_PROPAGATION_LOSS_MODEL_H

#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/propagation-environment.h>
#include <ns3/mobility-model.h>
#include <ns3/traced-callback.h>
#include "ns3/node.h"

namespace ns3 {

class IndoorToIndoorPropagationLossModel;
class OutdoorToOutdoorPropagationLossModel;
class OutdoorToIndoorPropagationLossModel;
class UrbanMacroCellPropagationLossModel;

/**
 * \ingroup buildings
 *
 *  \brief The Hybrid3gppPropagationModel is a compound of different models able to evaluate the pathloss
 *  in different environments and with buildings (i.e., indoor and outdoor communications).
 *
 *  This model includes different models defined by 3GPP TR 36.843 V12.0.0
 *  (2014-03) - Section A.2.1.2, which are combined in order
 *  to be able to evaluate the pathloss under different scenarios, in detail:
 *  - Environments: urban, suburban, open-areas;
 *  - frequency: 700 MHz for public safety and 2 GHz for general scenarios
 *  - D2D communications
 *  - Node position respect to buildings: indoor to indoor, outdoor to outdoor and outdoor to indoor
 *  - Building penetration loss
 *  - floors, etc...
 *
 *  \warning This model works only with MobilityBuildingInfo
 *
 */

class Hybrid3gppPropagationLossModel : public BuildingsPropagationLossModel
{

public:
  /**
   * structure to save the two nodes mobility models
   */
  struct MobilityDuo
  {
    Ptr<MobilityModel> a; ///< mobility model of node 1
    Ptr<MobilityModel> b; ///< mobility model of node 2

    /**
     * equality function
     * \param mo1 mobility model for node 1
     * \param mo2 mobility model for node 2
     */
    friend bool operator== (const MobilityDuo& mo1, const MobilityDuo& mo2)
    {
      return (mo1.a == mo2.a && mo1.b == mo2.b);
    }

    /**
     * less than function
     * \param mo1 mobility model for node 1
     * \param mo2 mobility model for node 2
     */
    friend bool operator< (const MobilityDuo& mo1, const MobilityDuo& mo2)
    {
      return (mo1.a < mo2.a || ( (mo1.a == mo2.a) && (mo1.b < mo2.b)));
    }

  };
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  Hybrid3gppPropagationLossModel ();
  ~Hybrid3gppPropagationLossModel ();

  /**
   * set the propagation frequency
   *
   * \param freq the propagation frequency in Hz
   */
  void SetFrequency (double freq);

  /**
   * set the height threshold
   *
   * \param threshold the height threshold in meters
   *
   * If the height difference between Tx node and Rx node is greater than this threshold,
   * the model will consider it as macro cell communication."
   */
  void SetHeightThreshold (double threshold);

  /**
   * Compute if it is a Macro Cell Communication, i.e., eNB<->UE or UE<->eNB
   *
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns true if it is Macro cell communication else it returns false
   */
  bool IsMacroComm (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Compute the pathloss based on the positions of the two nodes
   *
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the propagation loss (in dB)
   */
  virtual double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * \param a first mobility model
   * \param b second mobility model
   *
   * \return the shadowing value (in dB)
   */
  double GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Evaluate the shadowing standard deviation based on the positions of the two nodes
   *
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the shadowing standard deviation ""sigma"" (in dB)
   */
  virtual double EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const;

  /**
   * Enable or disable shadowing
   *
   * \param enableShadowing true if shadowing is to be supported,
   *                        false otherwise
   */
  void EnableShadowing (bool enableShadowing);

private:
  /**
   * Evaluate the path loss based on indoor propagation loss model from IndoorPropagationLossModel
   *
   * \param a first mobility model
   * \param b second mobility model
   *
   * \return a pair of values :
   * 1/ a double = the pathloss value (in dB) between the two given mobility models
   * 2/ a boolean = define the LOS/NLOS condition (return true if LOS)
   */
  std::pair<double, bool> IndoorToIndoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Evaluate the path loss based on outdoor propagation loss model from OutdoorPropagationLossModel
   *
   * \param a first mobility model
   * \param b second mobility model
   *
   * \return the path loss value in dB
   */
  double OutdoorToOutdoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Evaluate the path loss based on outdoor to indoor propagation loss model from OutdoorToIndoorPropagationLossModel
   *
   * \param a first mobility model
   * \param b second mobility model
   *
   * \return the path loss value in dB
   */
  double OutdoorToIndoor (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Evaluate the path loss based on urban macro cell propagation loss model from OutdoorToIndoorPropagationLossModel
   *
   * \param a first mobility model
   * \param b second mobility model
   *
   * \return the path loss value in dB
   */
  double UrbanMacroCell (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  Ptr<IndoorToIndoorPropagationLossModel> m_indoorToIndoor; ///< Pointer to IndoorPropagationLossModel
  Ptr<OutdoorToOutdoorPropagationLossModel> m_outdoorToOutdoor; ///< Pointer to OutdoorPropagationLossModel
  Ptr<OutdoorToIndoorPropagationLossModel> m_outdoorToIndoor; ///< Pointer to OutdoorToIndoorPropagationLossModel
  Ptr<UrbanMacroCellPropagationLossModel> m_urbanMacroCell; ///< Pointer to UrbanMacroCellPropagationLossModel

  double m_frequency; ///< The propagation frequency in Hz
  TracedCallback<double, Ptr<Node>, Ptr<Node>, double, bool, bool> m_hybrid3gppPathlossTrace; ///< Trace
  mutable std::map<Ptr<MobilityModel>,  std::map<Ptr<MobilityModel>, double> > m_shadowingLossMap; ///< Map to keep track of shadowing values
  bool m_cacheLoss; ///< Cache the loss or not
  mutable std::map<MobilityDuo, double> m_lossMap; ///< cache for loss values
  mutable bool m_isMacroComm; ///< True if it is macro cell communication, i.e., eNB<->UE or UE<->eNB
  double m_heightThreshold; ///< Height threshold for the difference between the height of TX and RX node
  bool m_isShadowingEnabled; ///< Shadowing Status
};

}

#endif /* HYBRID_3GPP_PROPAGATION_LOSS_MODEL_H */
