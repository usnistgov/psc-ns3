/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 CTTC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This model, authored by NIST, is derived from parabolic-antenna-model.h
 * originally authored by Nicola Baldo <nbaldo@cttc.es>.
 */

#ifndef PARABOLIC_3D_ANTENNA_MODEL_H
#define PARABOLIC_3D_ANTENNA_MODEL_H

#include <ns3/object.h>
#include "antenna-model.h"

namespace ns3 {

/**
 *
 * \brief  Antenna model based on a parabolic approximation of the main lobe radiation pattern.
 *
 * This class implements the parabolic model as described in 3GPP document TR 36.814
 *
 */
class Parabolic3dAntennaModel : public AntennaModel
{
public:
  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from AntennaModel
  virtual double GetGainDb (Angles a);

  /**
   * Sets the horizontal beamwidth
   * \param horizontalBeamwidthDegrees The horizontal beamwidth in degrees
   */
  void SetHorizontalBeamwidth (double horizontalBeamwidthDegrees);

  /**
   * Gets the horizontal beamwidth
   * \return The horizontal beamwidth in degrees
   */
  double GetHorizontalBeamwidth () const;

  /**
   * Sets the orientation (azimuth)
   * \param orientationDegrees The orientation (azimuth) in degrees
   */
  void SetOrientation (double orientationDegrees);

  /**
   * Gets the orientation
   * \return The orientation in degrees
   */
  double GetOrientation () const;

  /**
   * Sets the vertical beamwidth
   * \param verticalBeamwidthDegrees The vetical beamwidth in degrees
   */
  void SetVerticalBeamwidth (double verticalBeamwidthDegrees);

  /**
   * Gets the vertical beamwidth
   * \return The vertical beamwidth in degrees
   */
  double GetVerticalBeamwidth () const;

  /**
   * Sets the electrical tilt
   * \param electricalTiltDegrees The electrical tilt in degrees
   */
  void SetElectricalTilt (double electricalTiltDegrees);

  /**
   * Gets the electrical tilt
   * \return The electrical tilt in degrees
   */
  double GetElectricalTilt () const;

  /**
   * Sets the mechanical tilt
   * \param mechanicalTiltDegrees The mechinacal tilt in degrees
   */
  void SetMechanicalTilt (double mechanicalTiltDegrees);

  /**
   * Gets the mechanical tilt
   * \return The mechanical tilt in degrees
   */
  double GetMechanicalTilt () const;

private:
  //Computes the gain on horizontal plane
  double GetHorizontalGainDb (Angles a);
  //Computes the gain on the vertical plane
  double GetVerticalGainDb (Angles a);

  double m_hBeamwidthRadians;

  double m_orientationRadians;

  double m_maxHAttenuation;

  double m_vBeamwidthRadians;

  double m_eTiltRadians;

  double m_mTiltRadians;

  double m_maxVAttenuation;
  
  double m_boresightGain;

};

} // namespace ns3

#endif // PARABOLIC_3D_ANTENNA_MODEL_H
