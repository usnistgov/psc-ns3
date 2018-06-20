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

#ifndef PSC_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H
#define PSC_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H

#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/propagation-environment.h>

namespace ns3 {

/**
 * \ingroup psc
 *
 * \brief Urbanmacrocell propagation model for the 700 MHz frequency (Public Safety use cases)
 * 
 * This class implements the outdoor propagation model for 700 MHz based on 3GPP sepcifications:
 * 3GPP TR 36.843 V12.0.1 (2014-03) / Section A.2.1.2 
 */
class PscUrbanmacrocellPropagationLossModel : public BuildingsPropagationLossModel
{

public:
  
  /**
   * structure to save the pathloss, the line of sight and the random number
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
    friend bool operator==(const MobilityDuo& mo1, const MobilityDuo& mo2)
    {
      return (mo1.a == mo2.a && mo1.b == mo2.b);
    }
    /**
     * less than function
     * \param mo1 mobility model for node 1
     * \param mo2 mobility model for node 2
     */
    friend bool operator<(const MobilityDuo& mo1, const MobilityDuo& mo2)
    {
      return (mo1.a < mo2.a || ( (mo1.a == mo2.a) && (mo1.b < mo2.b)));
    }

  };

  // inherited from Object
  static TypeId GetTypeId (void);
  PscUrbanmacrocellPropagationLossModel ();
  virtual ~PscUrbanmacrocellPropagationLossModel ();

  /** 
   * Calculate the pathloss in dBm
   * 
   * \param a the first mobility model
   * \param b the second mobility model
   * 
   * \return the loss in dBm for the propagation between
   * the two given mobility models
   */
  double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Calculate the shadowing 
   * 
   * \param a first mibility model
   * \param b second mobility model
   *
   * \return the shadowing value
   */
  double GetShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
   * Evaluate the shadowing standard deviation based on the positions of the two nodes
   *
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the shadowing standard deviation ""sigma"" (in dBm)
  */
  double EvaluateSigma (Ptr<MobilityBuildingInfo> a, Ptr<MobilityBuildingInfo> b) const;
   
  /** 
   * Calculate the pathloss, the line of sight (LOS) and the generated number of the LOS condition
   * 
   * \param a the first mobility model
   * \param b the second mobility model
   * \param check 
   * 
   * \return the pathloss, the line of signt LOS and the the ransdom number associated to ii
   */
  PathlossType GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b, bool check);

private:

  virtual int64_t DoAssignStreams (int64_t stream);
  
  double m_frequency; ///< The propagation frequency in Hz
  double m_buildingHeight; ///< The average building heigth
  double m_streetWidth; ///< The average street width
  bool m_isShadowingEnabled; ///< Shadowing Status
  bool m_isLosEnabled; ///< LoS Condition
  Ptr<UniformRandomVariable> m_rand; ///< Random number to generate
  mutable std::map<MobilityDuo, double> m_randomMap; ///< Map to keep track of random numbers generated per pair of nodes
  mutable std::map<Ptr<MobilityModel>,  std::map<Ptr<MobilityModel>, double> > m_shadowingLossMap; ///< Map to keep track of shadowing values
  TracedCallback<Ptr<MobilityModel>, Ptr<MobilityModel>, double, double, double, double> m_losTrace; ///< Trace

};

} // namespace ns3


#endif // PSC_URBANMACROCELL_PROPAGATION_LOSS_MODEL_H
