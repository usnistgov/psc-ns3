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

#ifndef UAV_MOBILITY_ENERGY_MODEL_H
#define UAV_MOBILITY_ENERGY_MODEL_H

#include <ns3/mobility-model.h>
#include <ns3/simple-device-energy-model.h>
#include <ns3/nstime.h>
#include <ns3/traced-value.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup psc
 * \brief NIST Model of UAV energy consumption from movement
 *
 * Provies a simple framework to model basic energy consumption of a (single/multi)rotor UAV
 * with configurable energy costs.
 * 
 * Not meant to provide results totally accurate to the real world, merely provide
 * means to model movement in a way that has some cost.
 * 
 * Movement costs are only calculated when the UAV state changes (e.g. Hover() to Move())
 */
class UavMobilityEnergyModel: public DeviceEnergyModel
{
 public:
  /**
   * Detected mobility state from the attached MobilityModel
   */
  enum State:int { ASCEND, DESCEND, HOVER, MOVE, STOP };

  /**
   * \brief Signature for simple Callbacks of this class
   * 
   * \param model The model in the state that caused the callback
   */
  typedef void (*BasicCallback)(Ptr<const UavMobilityEnergyModel> model);
  /**
   * \brief Signature for state change callback
   * 
   * \param oldState the old state
   * \param newState the new state
   * \param velocity the new velocity
   */
  typedef void (*StateChangedCallback)(State oldState, State newState, Vector velocity);
  /**
   * \brief Signature for current change callback
   * 
   * \param oldCurrent the old current
   * \param newCurrent the new current
   * \param velocity the new velocity
   */
  typedef void (*CurrentChangedCallback) (double oldCurrent, double newCurrent, Vector velocity);

  /**
   * \brief Get the class TypeId
   * 
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Construct a new Uav Mobility Energy Model
   * with no Node or EnergySource
   * 
   * The Init() must be called before this object should be considered usable 
   */
  UavMobilityEnergyModel (void);

  /**
   * \brief Sets Up a new UavMobilityEnergyModel, attaching it to the passed
   * node & setting the energy source
   * 
   * \param node The Node this energy model will serve for
   * Node must have an installed MobilityModel!
   * 
   * \param energySource The EnergySource that this model will draw from
   * Low/Recharged energy events come from this source
   */
  void Init (Ptr<Node> node, Ptr<EnergySource> energySource);

  ~UavMobilityEnergyModel (void) override;

  /**
   * \brief Attaches an EnergySource to the model
   * 
   * \param source The EnergySource to connect to this model
   */
  void SetEnergySource(Ptr<EnergySource> source) override;

  /**
   * \brief Get the EnergySource attached to this model
   * 
   * \return The attached EnergySource
   */
  Ptr<EnergySource> GetEnergySource (void) const;

  /**
   * \brief Calculates the total energy consumed by this model over its lifetime
   * 
   * \return double The total joules consumed by this energy model
   */
  double GetTotalEnergyConsumption (void) const override;

  /**
   * \brief Implemented to complete the interface DeviceEnergyModel
   * Does nothing and issues a warning
   * 
   * \param newState a state from UavMobilityEnergyModel::State
   */
  void ChangeState (int newState) override;

  /**
   * Gets the current movement state of the model
   * \return the movement state
   */
  State GetState (void) const;

  /**
   * \brief Stops all energy consumption.
   * 
   * Meant to simulate landing/disabling the drone.
   * For stopping in air see Hover()
   */ 
  void Stop (void);

  /**
   * \brief Set the current for increasing the elevation of the UAV.
   * 
   * Cost is configurable through the AscendEnergyConversionFactor attribute
   * 
   * \param speed The rate in which the UAV is moving. In m/s
   */
  void Ascend (double speed);

  /**
   * \brief Set the current for decreasing the elevation of the UAV.
   * 
   * Cost is configurable through the DescendEnergyConversionFactor attribute
   * 
   * \param speed The rate in which the UAV is moving. In m/s
   */
  void Descend (double speed);

  /**
   * \brief Set the current for moving the UAV.
   * 
   * Sets the current based on both the magnitude of the 2D velocity (x, y)
   * and adds the Ascend/Descend current based on the Z speed.
   * 
   * Based on MoveEnergyConversionFactor for 2D movement &
   * AscendEnergyConversionFactor, DescendEnergyConversionFactor
   * if the node is also moving on the Z axis
   * 
   * \param velocity The 3D velocity of the node in m/s
   */
  void Move (const Vector &velocity);

  /**
   * \brief Set the current for holding the UAV in position while runnning.
   * 
   * Cost is configurable through the HoverCurrent attribute
   */
  void Hover (void);

  /**
   * \brief Invokes the EnergyDepletionCallBack if it was set
   * 
   * Called when the attached EnergySource has crossed the low energy threshold.
   */
  void HandleEnergyDepletion (void) override;

  /**
   * \brief Invokes the EnergyRecharged if it was set
   * 
   * Called when the attached EnergySource has been recharged.
   */
  void HandleEnergyRecharged (void) override;

  /**
   * Called when energy in the attached EnergySource has been updated.
   * 
   * Not implemented
   */
  void HandleEnergyChanged (void) override
  {
  }

  /**
   * \return The Ascend Energy Conversion Factor A/(m/s)
   */
  double GetAscendEnergyConversionFactor (void) const;

  /**
   * \param ascendEnergyConversionFactor 
   * The new value of the ascend energy conversion factor.
   * In A/(m/s).
   * Pass 0 to make Ascend() take no energy
   */
  void SetAscendEnergyConversionFactor (double ascendEnergyConversionFactor);

  /**
   * \return The descend energy conversion factor in A/(1/(m/s)).
   */
  double GetDescendEnergyConversionFactor (void) const;

  /**
   * \param descendEnergyConversionFactor
   * The new value of the descend energy conversion factor.
   * In A/(1/(m/s)).
   * Pass 0 to make Descend() take no energy
   */
  void SetDescendEnergyConversionFactor (double descendEnergyConversionFactor);
  
  /**
   * \return The move energy conversion factor in A/(m/s).
   */
  double GetMoveEnergyConversionFactor (void) const;

  /**
   * \param moveEnergyConversionFactor
   * The new value of the move energy conversion factor.
   * In A/(m/s).
   * Pass 0 to make Move() take no energy
   */
  void SetMoveEnergyConversionFactor (double moveEnergyConversionFactor);

  /**
   * \return The fixed amperage to hover the UAV. In amperes.
   */
  double GetHoverCurrent (void) const;

  /**
   * \param hoverEnergyCurrent The new fixed amperage to hover the UAV
   * (In amperes).
   * Pass 0 to make Hover() take no energy.
   */
  void SetHoverCurrent (double hoverEnergyCurrent);

  /**
   * \brief Attaches the MobilityModel in node
   * 
   * Connects the CourseChangeCallback in this model to the
   * CourseChange trace on the MobilityModel aggregated
   * to node, and sets the initial state of this model
   * based on the state of the aggregated MobilityModel.
   * 
   * Note: this method will abort if a MobilityModel
   * is not found aggregated onto node.
   *
   * \param node a Node with a MobilityModel installed
   */
  void ConnectMobility (Ptr<Node> node);

 private:
  Time m_lastUpdateTime {Seconds (0)}; //!< tracks last energy update time 
  Ptr<MobilityModel> m_mobility; //!< Ptr to the mobility model
  Ptr<EnergySource> m_source; //!< Ptr to the energy source
  State m_state {State::STOP}; //!< The state of the model
  double m_current {0};  //!< The current in amperes
  TracedValue<double> m_totalEnergyConsumption {0};  //!< Trace of energy consumption
  double m_ascendEnergyConversionFactor;  //!< variable implementing AscendEnergyConversionFactor attribute
  double m_descendEnergyConversionFactor; //!< variable implementing DescendEnergyConversionFactor attribute
  double m_moveEnergyConversionFactor; //!< variable implementing MoveEnergyConversionFactor attribute
  double m_hoverEnergyCurrent; //!< variable implementing HoverCurrent attribute

  TracedCallback<State, State, Vector> m_stateChangedTrace; //!< Variable storing state change callback
  TracedCallback<double, double, Vector> m_currentChangedTrace; //!< Variable storing current change callback
  TracedCallback<Ptr<const UavMobilityEnergyModel>> m_energyDepletionTrace; //!< variable storing energy depletion trace callback
  TracedCallback<Ptr<const UavMobilityEnergyModel>> m_energyRechargedTrace; //!< variable storing energy recharged trace callback

  /**
   * Gets the current draw of this model in amperes
   * \return the current draw in amperes
   */
  double DoGetCurrentA (void) const override;

  void DoDispose (void) override;

  /**
   * \brief Changes the current and updates the energy source
   * 
   * \param current The new current to set
   */
  void UpdateCurrent (double current);

  /**
   * Handle if the Node changes Velocity
   *
   * Use to set our energy consumption mode
   * \param model pointer to the mobility model
   */
  void CourseChangeCallback (Ptr<const MobilityModel> model);

  /**
   * \brief Update the internal state of the model, notifying subscribers
   * 
   * updates m_state and calls m_stateChangedTrace (If the state is different)
   * with the current velocity from m_mobility
   * 
   * \param newState The state to set m_state to
   */
  void UpdateState (State newState);

  /**
   * \brief Calculates the current for ascending at a set speed
   * 
   * \param speed How fast the Node is ascending
   * 
   * \return double The current to set on the EnergySource in amperes
   */
  double AscendCurrent (double speed);

  /**
   * \brief Calculates the current for descending at a set speed
   * 
   * \param speed How fast the Node is descending
   * 
   * \return double The current to set on the EnergySource in amperes
   */
  double DescendCurrent (double speed);

  /**
   * \brief Calculates the current for moving at a set speed
   * 
   * \param speed How fast the Node is moving
   * 
   * \return double The current to set on the EnergySource in amperes
   */
  double MoveCurrent (double speed);

  /**
   * Refreshes the total energy consumed by this model
   */
  void UpdateTotalEnergyConsumption (void);

};

} // namespace psc
} // namespace ns3

#endif /* UAV_MOBILITY_ENERGY_MODEL_H */
