/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011,12 CTTC
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
 * This test, authored by NIST, is derived from test-parabolic-antenna.cc
 * originally authored by Nicola Baldo <nbaldo@cttc.es>.
 */

#include <ns3/log.h>
#include <ns3/test.h>
#include <ns3/double.h>
#include <ns3/parabolic-3d-antenna-model.h>
#include <ns3/simulator.h>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("TestParabolic3dAntennaModel");

using namespace ns3;

enum Parabolic3dAntennaModelGainTestCondition
{
  EQUAL = 0,
  LESSTHAN = 1
};

class Parabolic3dAntennaModelTestCase : public TestCase
{
public:
  static std::string BuildNameString (Angles a, double hb, double o, double hg, double vb, double e, double m, double vg);
  Parabolic3dAntennaModelTestCase (Angles a, double hb, double o, double hg, double vb, double e, double m, double vg, double expectedGainDb, Parabolic3dAntennaModelGainTestCondition cond);


private:
  virtual void DoRun (void);

  Angles m_a;
  double m_hb;
  double m_o;
  double m_hg;
  double m_vb;
  double m_e;
  double m_m;
  double m_vg;
  double m_expectedGain;
  Parabolic3dAntennaModelGainTestCondition m_cond;
};

std::string Parabolic3dAntennaModelTestCase::BuildNameString (Angles a, double hb, double o, double hg, double vb, double e, double m, double vg)
{
  std::ostringstream oss;
  oss <<  "inclination=" << a.GetInclination () << " , azimuth=" << a.GetAzimuth ()
      << ", horizontal beamdwidth=" << hb << " deg"
      << ", orientation=" << o
      << ", horizontal maxAttenuation=" << hg << " dB"
      << ", vertical beamwidth=" << vb << " deg"
      << ", electrical tilt= " << e << " deg"
      << ", mechanical tilt= " << m << " deg"
      << ", vertical maxAttenuation=" << vb << " dB"
  ;
  return oss.str ();
}

Parabolic3dAntennaModelTestCase::Parabolic3dAntennaModelTestCase (Angles a, double hb, double o, double hg, double vb, double e, double m, double vg, double expectedGainDb, Parabolic3dAntennaModelGainTestCondition cond)
  : TestCase (BuildNameString (a, hb, o, hg, vb, e, m, vg)),
    m_a (a),
    m_hb (hb),
    m_o (o),
    m_hg (hg),
    m_vb (vb),
    m_e (e),
    m_m (m),
    m_vg (vg),
    m_expectedGain (expectedGainDb),
    m_cond (cond)
{
}

void
Parabolic3dAntennaModelTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_a, m_hb, m_o, m_hg, m_vb, m_e, m_m, m_vg));

  Ptr<Parabolic3dAntennaModel> a = CreateObject<Parabolic3dAntennaModel> ();
  a->SetAttribute ("HorizontalBeamwidth", DoubleValue (m_hb));
  a->SetAttribute ("Orientation", DoubleValue (m_o));
  a->SetAttribute ("MaxHorizontalAttenuation", DoubleValue (m_hg));
  a->SetAttribute ("VerticalBeamwidth", DoubleValue (m_vb));
  a->SetAttribute ("ElectricalTilt", DoubleValue (m_e));
  a->SetAttribute ("MechanicalTilt", DoubleValue (m_m));
  a->SetAttribute ("MaxVerticalAttenuation", DoubleValue (m_vg));
  double actualGain = a->GetGainDb (m_a);
  switch (m_cond)
    {
    case EQUAL:
      NS_TEST_EXPECT_MSG_EQ_TOL (actualGain, m_expectedGain, 0.001, "wrong value of the radiation pattern");
      break;
    case LESSTHAN:
      NS_TEST_EXPECT_MSG_LT (actualGain, m_expectedGain, "gain higher than expected");
      break;
    default:
      break;
    }
}




class Parabolic3dAntennaModelTestSuite : public TestSuite
{
public:
  Parabolic3dAntennaModelTestSuite ();
};

Parabolic3dAntennaModelTestSuite::Parabolic3dAntennaModelTestSuite ()
  : TestSuite ("parabolic-3d-antenna-model", UNIT)
{

  // with a 60 deg beamwidth, gain is -20dB at +-77.460 degrees from boresight
  // testing horizontal plane
  //                                                                         azimuth,                     inclination, h beamwidth, orientation, h maxAttn, v beamwidth, etilt, mtilt, v maxAttn, expectedGain,   condition
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (30),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-30),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-90),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (90),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (100),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (150),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (180),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-100),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-150),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-180),    DegreesToRadians (90)),        60,           0,       25,           10,     0,     0,        20,          -25,     EQUAL), TestCase::QUICK);

  // with a 60 deg beamwidth, gain is -10dB at +-54.772 degrees from boresight
  // test positive orientation
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (60),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (90),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (30),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-30),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (150),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (160),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (210),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (240),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-40),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-90),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-120),    DegreesToRadians (90)),        60,           60,       15,           10,     0,     0,        20,          -15,     EQUAL), TestCase::QUICK);

  // test negative orientation and different beamwidths
  // with a 80 deg beamwidth, gain is -20dB at +- 73.030 degrees from boresight
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-150),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-110),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians (-190),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-70),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (92),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians  (-30),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (60),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (90),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (90),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians   (30),    DegreesToRadians (90)),        80,         -150,       10,           10,     0,     0,        20,          -10,     EQUAL), TestCase::QUICK);

  // testing vertical plane
  //                                                                         azimuth,                     inclination, h beamwidth, orientation, h maxAttn, v beamwidth, etilt, mtilt, v maxAttn, expectedGain,   condition
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (90)),        60,           0,       25,           10,     0,     0,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (85)),        60,           0,       25,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (95)),        60,           0,       25,           10,     0,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians   (0)),        60,           0,       25,           10,     0,     0,        20,          -20,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians (180)),        60,           0,       25,           10,     0,     0,        20,          -20,     EQUAL), TestCase::QUICK);

  // testing tilt

  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (95)),        60,           0,       25,           10,     5,     0,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (95)),        60,           0,       25,           10,     0,     5,        20,            0,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (90)),        60,           0,       25,           10,     5,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians  (90)),        60,           0,       25,           10,     0,     5,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians (100)),        60,           0,       25,           10,     5,     0,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians (100)),        60,           0,       25,           10,     0,     5,        20,           -3,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians   (0)),        60,           0,       25,           10,     5,     0,        20,          -20,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians   (0)),        60,           0,       25,           10,     0,     5,        20,          -20,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians (180)),        60,           0,       25,           10,     5,     0,        20,          -20,     EQUAL), TestCase::QUICK);
  AddTestCase (new Parabolic3dAntennaModelTestCase (Angles (DegreesToRadians    (0),  DegreesToRadians (180)),        60,           0,       25,           10,     0,     5,        20,          -20,     EQUAL), TestCase::QUICK);


}

static Parabolic3dAntennaModelTestSuite staticParabolic3dAntennaModelTestSuiteInstance;
