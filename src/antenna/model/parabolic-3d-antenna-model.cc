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
 * This model, authored by NIST, is derived from parabolic-antenna-model.cc
 * originally authored by Nicola Baldo <nbaldo@cttc.es>.
 */

#include <ns3/log.h>
#include <ns3/double.h>
#include <cmath>

#include "parabolic-3d-antenna-model.h"

NS_LOG_COMPONENT_DEFINE ("Parabolic3dAntennaModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Parabolic3dAntennaModel);

TypeId
Parabolic3dAntennaModel::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Parabolic3dAntennaModel")
    .SetParent<AntennaModel> ()
    .SetGroupName ("Antenna")
    .AddConstructor<Parabolic3dAntennaModel> ()
    .AddAttribute ("HorizontalBeamwidth",
                   "The 3dB horizontal beamwidth (degrees)",
                   DoubleValue (70),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::SetHorizontalBeamwidth,
                                       &Parabolic3dAntennaModel::GetHorizontalBeamwidth),
                   MakeDoubleChecker<double> (0, 180))
    .AddAttribute ("Orientation",
                   "The angle (degrees) that expresses the orientation of the antenna on the x-y plane relative to the x axis",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::SetOrientation,
                                       &Parabolic3dAntennaModel::GetOrientation),
                   MakeDoubleChecker<double> (-360, 360))
    .AddAttribute ("MaxHorizontalAttenuation",
                   "The maximum horizontal attenuation (dB) of the antenna radiation pattern.",
                   DoubleValue (25.0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::m_maxHAttenuation),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("VerticalBeamwidth",
                   "The 3dB vertical beamwidth (degrees)",
                   DoubleValue (10),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::SetVerticalBeamwidth,
                                       &Parabolic3dAntennaModel::GetVerticalBeamwidth),
                   MakeDoubleChecker<double> (0, 180))
    .AddAttribute ("ElectricalTilt",
                   "The angle (degrees) that expresses the orientation of the antenna on the y-z plane relative to the y axis",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::SetElectricalTilt,
                                       &Parabolic3dAntennaModel::GetElectricalTilt),
                   MakeDoubleChecker<double> (-90, 90))
    .AddAttribute ("MechanicalTilt",
                   "The angle (degrees) that expresses the orientation of the antenna on the y-z plane relative to the y axis",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::SetMechanicalTilt,
                                       &Parabolic3dAntennaModel::GetMechanicalTilt),
                   MakeDoubleChecker<double> (-90, 90))
    .AddAttribute ("MaxVerticalAttenuation",
                   "The maximum vertical attenuation (dB) of the antenna radiation pattern.",
                   DoubleValue (20.0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::m_maxVAttenuation),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("BoresightGain",
                   "The boresight gain of the antenna (dB).",
                   DoubleValue (0),
                   MakeDoubleAccessor (&Parabolic3dAntennaModel::m_boresightGain),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

void
Parabolic3dAntennaModel::SetHorizontalBeamwidth (double horizontalBeamwidthDegrees)
{
  NS_LOG_FUNCTION (this << horizontalBeamwidthDegrees);
  m_hBeamwidthRadians = DegreesToRadians (horizontalBeamwidthDegrees);
}

double
Parabolic3dAntennaModel::GetHorizontalBeamwidth () const
{
  return RadiansToDegrees (m_hBeamwidthRadians);
}

void
Parabolic3dAntennaModel::SetOrientation (double orientationDegrees)
{
  NS_LOG_FUNCTION (this << orientationDegrees);
  m_orientationRadians = DegreesToRadians (orientationDegrees);
}

double
Parabolic3dAntennaModel::GetOrientation () const
{
  return RadiansToDegrees (m_orientationRadians);
}

void
Parabolic3dAntennaModel::SetVerticalBeamwidth (double verticalBeamwidthDegrees)
{
  NS_LOG_FUNCTION (this << verticalBeamwidthDegrees);
  m_vBeamwidthRadians = DegreesToRadians (verticalBeamwidthDegrees);
}

double
Parabolic3dAntennaModel::GetVerticalBeamwidth () const
{
  return RadiansToDegrees (m_vBeamwidthRadians);
}

void
Parabolic3dAntennaModel::SetElectricalTilt (double electricalTiltDegrees)
{
  NS_LOG_FUNCTION (this << electricalTiltDegrees);
  m_eTiltRadians = DegreesToRadians (electricalTiltDegrees);
}

double
Parabolic3dAntennaModel::GetElectricalTilt () const
{
  return RadiansToDegrees (m_eTiltRadians);
}

void
Parabolic3dAntennaModel::SetMechanicalTilt (double mechanicalTiltDegrees)
{
  NS_LOG_FUNCTION (this << mechanicalTiltDegrees);
  m_mTiltRadians = DegreesToRadians (mechanicalTiltDegrees);
}

double
Parabolic3dAntennaModel::GetMechanicalTilt () const
{
  return RadiansToDegrees (m_mTiltRadians);
}


double
Parabolic3dAntennaModel::GetGainDb (Angles a)
{
  NS_LOG_FUNCTION (this << a);

  double gainDb = m_boresightGain -std::min (-(GetHorizontalGainDb (a) + GetVerticalGainDb (a)), m_maxHAttenuation);


  NS_LOG_LOGIC ("gain = " << gainDb);
  return gainDb;
}

double
Parabolic3dAntennaModel::GetHorizontalGainDb (Angles a)
{
  NS_LOG_FUNCTION (this << a);

  // azimuth angle w.r.t. the reference system of the antenna
  double azimuth = a.GetAzimuth () - m_orientationRadians;

  // make sure azimuth is in (-pi, pi]
  while (azimuth <= -M_PI)
    {
      azimuth += M_PI + M_PI;
    }
  while (azimuth > M_PI)
    {
      azimuth -= M_PI + M_PI;
    }

  NS_LOG_LOGIC ("azimuth = " << azimuth );

  double gainDb = -std::min (12 * pow (azimuth / m_hBeamwidthRadians, 2), m_maxHAttenuation);

  NS_LOG_LOGIC ("Horizontal gain = " << gainDb);
  return gainDb;
}

double
Parabolic3dAntennaModel::GetVerticalGainDb (Angles a)
{
  NS_LOG_FUNCTION (this << a);
  // inclination angle w.r.t. the reference system of the antenna
  double inclination = a.GetInclination () - M_PI / 2 - (m_mTiltRadians + m_eTiltRadians);

  // make sure inclination is in [-pi, pi]
  NS_ASSERT (inclination <= M_PI && inclination >= -M_PI);

  NS_LOG_LOGIC ("inclination = " << inclination );

  double gainDb = -std::min (12 * pow (inclination / m_vBeamwidthRadians, 2), m_maxVAttenuation);

  NS_LOG_LOGIC ("Vertical gain = " << gainDb);
  return gainDb;
}

}

