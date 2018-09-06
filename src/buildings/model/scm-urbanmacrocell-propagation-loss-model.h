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

#ifndef SCM_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H
#define SCM_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H

#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/propagation-environment.h>

namespace ns3 {

/**
 * \ingroup buildings
 *
 * \brief SCM urban macro cell propagation model based on 3GPP TR 25.996 v6.1.0
 *
 */
class ScmUrbanMacroCellPropagationLossModel : public BuildingsPropagationLossModel
{

public:
  /**
   * structure of the pathloss type to save the pathloss, probability of line-of-sight and random number to determine LOS condition
   */
  struct PathlossType
  {
    double loss; ///< pathloss calculated
    double plos; ///< probability of line-of-sight
    double r; ///< random number to determine LOS condition
  };

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

  // inherited from Object
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  ScmUrbanMacroCellPropagationLossModel ();
  virtual ~ScmUrbanMacroCellPropagationLossModel ();

  /**
   * Calculate the pathloss in dB
   *
   * \param a the first mobility model
   * \param b the second mobility model
   *
   * \return the loss in dB for the propagation between the two given mobility models
   */
  double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Calculate the shadowing
   *
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
  double EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const;

private:
  virtual int64_t DoAssignStreams (int64_t stream);

  double m_frequency; ///< The propagation frequency in Hz
  bool m_isShadowingEnabled; ///< Shadowing Status
  Ptr<UniformRandomVariable> m_rand; ///< Random number to generate
  mutable std::map<MobilityDuo, double> m_randomMap; ///< Map to keep track of random numbers generated per pair of nodes
  mutable std::map<Ptr<MobilityModel>,  std::map<Ptr<MobilityModel>, double> > m_shadowingLossMap; ///< Map to keep track of shadowing values
};

} // namespace ns3


#endif // SCM_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H

