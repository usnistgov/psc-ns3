/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/type-id.h>

#include "mcptt-pusher.h"

#include "mcptt-pusher-orchestrator-spurt-cdf.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusherOrchestratorSpurtCdf");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttPusherOrchestratorSpurtCdf);

const std::vector<std::pair<double, double> > McpttPusherOrchestratorSpurtCdf::CDF_POINTS = {
  std::pair<double, double> (0, 0.0),
  std::pair<double, double> (0.1, 0.000955338),
  std::pair<double, double> (0.2, 0.002030093),
  std::pair<double, double> (0.3, 0.003224266),
  std::pair<double, double> (0.4, 0.005254359),
  std::pair<double, double> (0.5, 0.006806783),
  std::pair<double, double> (0.6, 0.008956293),
  std::pair<double, double> (0.7, 0.010986386),
  std::pair<double, double> (0.8, 0.012658228),
  std::pair<double, double> (0.9, 0.01516599),
  std::pair<double, double> (1, 0.017196083),
  std::pair<double, double> (1.1, 0.018867925),
  std::pair<double, double> (1.2, 0.020539766),
  std::pair<double, double> (1.3, 0.022569859),
  std::pair<double, double> (1.4, 0.026391211),
  std::pair<double, double> (1.5, 0.031048483),
  std::pair<double, double> (1.6, 0.035586339),
  std::pair<double, double> (1.7, 0.042512539),
  std::pair<double, double> (1.8, 0.050513494),
  std::pair<double, double> (1.9, 0.059828039),
  std::pair<double, double> (2, 0.069620253),
  std::pair<double, double> (2.1, 0.080248388),
  std::pair<double, double> (2.2, 0.096489133),
  std::pair<double, double> (2.3, 0.109505613),
  std::pair<double, double> (2.4, 0.12694053),
  std::pair<double, double> (2.5, 0.142703606),
  std::pair<double, double> (2.6, 0.159063769),
  std::pair<double, double> (2.7, 0.179961786),
  std::pair<double, double> (2.8, 0.202412228),
  std::pair<double, double> (2.9, 0.22486267),
  std::pair<double, double> (3, 0.24826845),
  std::pair<double, double> (3.1, 0.274540244),
  std::pair<double, double> (3.2, 0.302722713),
  std::pair<double, double> (3.3, 0.330427514),
  std::pair<double, double> (3.4, 0.359923573),
  std::pair<double, double> (3.5, 0.386314784),
  std::pair<double, double> (3.6, 0.419632195),
  std::pair<double, double> (3.7, 0.445187485),
  std::pair<double, double> (3.8, 0.477668975),
  std::pair<double, double> (3.9, 0.501552424),
  std::pair<double, double> (4, 0.52901839),
  std::pair<double, double> (4.1, 0.551588249),
  std::pair<double, double> (4.2, 0.574994029),
  std::pair<double, double> (4.3, 0.597444471),
  std::pair<double, double> (4.4, 0.612252209),
  std::pair<double, double> (4.5, 0.634583234),
  std::pair<double, double> (4.6, 0.649749224),
  std::pair<double, double> (4.7, 0.667422976),
  std::pair<double, double> (4.8, 0.680081204),
  std::pair<double, double> (4.9, 0.696560783),
  std::pair<double, double> (5, 0.711846191),
  std::pair<double, double> (5.1, 0.723668498),
  std::pair<double, double> (5.2, 0.735610222),
  std::pair<double, double> (5.3, 0.746477191),
  std::pair<double, double> (5.4, 0.757702412),
  std::pair<double, double> (5.5, 0.767375209),
  std::pair<double, double> (5.6, 0.786004299),
  std::pair<double, double> (5.7, 0.793647003),
  std::pair<double, double> (5.8, 0.801170289),
  std::pair<double, double> (5.9, 0.80761882),
  std::pair<double, double> (6, 0.815022689),
  std::pair<double, double> (6.1, 0.82147122),
  std::pair<double, double> (6.2, 0.827203248),
  std::pair<double, double> (6.3, 0.832696441),
  std::pair<double, double> (6.4, 0.8379508),
  std::pair<double, double> (6.5, 0.844160497),
  std::pair<double, double> (6.6, 0.849773107),
  std::pair<double, double> (6.7, 0.85443038),
  std::pair<double, double> (6.8, 0.860162407),
  std::pair<double, double> (6.9, 0.863267256),
  std::pair<double, double> (7, 0.868879866),
  std::pair<double, double> (7.1, 0.87305947),
  std::pair<double, double> (7.2, 0.87652257),
  std::pair<double, double> (7.3, 0.879627418),
  std::pair<double, double> (7.4, 0.882493432),
  std::pair<double, double> (7.5, 0.887628374),
  std::pair<double, double> (7.6, 0.890852639),
  std::pair<double, double> (7.7, 0.893479818),
  std::pair<double, double> (7.8, 0.895509912),
  std::pair<double, double> (7.9, 0.89861476),
  std::pair<double, double> (8, 0.901122522),
  std::pair<double, double> (8.1, 0.904346788),
  std::pair<double, double> (8.2, 0.907809888),
  std::pair<double, double> (8.3, 0.909959398),
  std::pair<double, double> (8.4, 0.911870074),
  std::pair<double, double> (8.5, 0.914616671),
  std::pair<double, double> (8.6, 0.917005016),
  std::pair<double, double> (8.7, 0.918796274),
  std::pair<double, double> (8.8, 0.920348698),
  std::pair<double, double> (8.9, 0.92285646),
  std::pair<double, double> (9, 0.925125388),
  std::pair<double, double> (9.1, 0.926916647),
  std::pair<double, double> (9.2, 0.928707905),
  std::pair<double, double> (9.3, 0.93026033),
  std::pair<double, double> (9.4, 0.932887509),
  std::pair<double, double> (9.5, 0.934201099),
  std::pair<double, double> (9.6, 0.93587294),
  std::pair<double, double> (9.7, 0.937783616),
  std::pair<double, double> (9.8, 0.938977788),
  std::pair<double, double> (9.9, 0.941007882),
  std::pair<double, double> (10, 0.94279914),
  std::pair<double, double> (10.1, 0.944232147),
  std::pair<double, double> (10.2, 0.945665154),
  std::pair<double, double> (10.3, 0.947098161),
  std::pair<double, double> (10.4, 0.948650585),
  std::pair<double, double> (10.5, 0.949605923),
  std::pair<double, double> (10.6, 0.95103893),
  std::pair<double, double> (10.7, 0.95235252),
  std::pair<double, double> (10.8, 0.953546692),
  std::pair<double, double> (10.9, 0.954263196),
  std::pair<double, double> (11, 0.956054454),
  std::pair<double, double> (11.1, 0.957129209),
  std::pair<double, double> (11.2, 0.958442799),
  std::pair<double, double> (11.3, 0.959159303),
  std::pair<double, double> (11.4, 0.960114641),
  std::pair<double, double> (11.5, 0.961308813),
  std::pair<double, double> (11.6, 0.961905899),
  std::pair<double, double> (11.7, 0.963577741),
  std::pair<double, double> (11.8, 0.964294244),
  std::pair<double, double> (11.9, 0.965130165),
  std::pair<double, double> (12, 0.965966086),
  std::pair<double, double> (12.1, 0.967399092),
  std::pair<double, double> (12.2, 0.968115596),
  std::pair<double, double> (12.3, 0.969429186),
  std::pair<double, double> (12.4, 0.970384524),
  std::pair<double, double> (12.5, 0.971339861),
  std::pair<double, double> (12.6, 0.972175782),
  std::pair<double, double> (12.7, 0.97313112),
  std::pair<double, double> (12.8, 0.973728206),
  std::pair<double, double> (12.9, 0.975041796),
  std::pair<double, double> (13, 0.975877717),
  std::pair<double, double> (13.1, 0.976474803),
  std::pair<double, double> (13.2, 0.97659422),
  std::pair<double, double> (13.3, 0.977430141),
  std::pair<double, double> (13.5, 0.978624313),
  std::pair<double, double> (13.6, 0.9792214),
  std::pair<double, double> (13.7, 0.979818486),
  std::pair<double, double> (13.8, 0.980296155),
  std::pair<double, double> (13.9, 0.980654406),
  std::pair<double, double> (14, 0.980893241),
  std::pair<double, double> (14.1, 0.981251493),
  std::pair<double, double> (14.2, 0.981729162),
  std::pair<double, double> (14.3, 0.981848579),
  std::pair<double, double> (14.4, 0.982206831),
  std::pair<double, double> (14.5, 0.982445665),
  std::pair<double, double> (14.6, 0.983042751),
  std::pair<double, double> (14.7, 0.983639838),
  std::pair<double, double> (14.8, 0.983878672),
  std::pair<double, double> (14.9, 0.984475758),
  std::pair<double, double> (15, 0.98483401),
  std::pair<double, double> (15.2, 0.985431096),
  std::pair<double, double> (15.3, 0.985550513),
  std::pair<double, double> (15.4, 0.985789348),
  std::pair<double, double> (15.5, 0.986267017),
  std::pair<double, double> (15.6, 0.986625269),
  std::pair<double, double> (15.7, 0.986744686),
  std::pair<double, double> (15.8, 0.987102938),
  std::pair<double, double> (15.9, 0.987461189),
  std::pair<double, double> (16, 0.987700024),
  std::pair<double, double> (16.1, 0.987819441),
  std::pair<double, double> (16.3, 0.988177693),
  std::pair<double, double> (16.4, 0.98829711),
  std::pair<double, double> (16.5, 0.988416527),
  std::pair<double, double> (16.6, 0.988774779),
  std::pair<double, double> (16.7, 0.988894196),
  std::pair<double, double> (16.9, 0.989013614),
  std::pair<double, double> (17, 0.989133031),
  std::pair<double, double> (17.1, 0.989371865),
  std::pair<double, double> (17.3, 0.9896107),
  std::pair<double, double> (17.4, 0.989968952),
  std::pair<double, double> (17.5, 0.990088369),
  std::pair<double, double> (17.7, 0.990327203),
  std::pair<double, double> (17.9, 0.99044662),
  std::pair<double, double> (18, 0.990685455),
  std::pair<double, double> (18.2, 0.990804872),
  std::pair<double, double> (18.3, 0.990924289),
  std::pair<double, double> (18.5, 0.991043707),
  std::pair<double, double> (18.6, 0.991282541),
  std::pair<double, double> (18.7, 0.991640793),
  std::pair<double, double> (18.8, 0.991879627),
  std::pair<double, double> (18.9, 0.992237879),
  std::pair<double, double> (19, 0.992357296),
  std::pair<double, double> (19.1, 0.992596131),
  std::pair<double, double> (19.2, 0.992715548),
  std::pair<double, double> (19.3, 0.9930738),
  std::pair<double, double> (19.4, 0.993312634),
  std::pair<double, double> (19.5, 0.993670886),
  std::pair<double, double> (19.7, 0.993790303),
  std::pair<double, double> (19.8, 0.993909721),
  std::pair<double, double> (20, 0.994029138),
  std::pair<double, double> (20.1, 0.994148555),
  std::pair<double, double> (20.3, 0.994267972),
  std::pair<double, double> (20.4, 0.994506807),
  std::pair<double, double> (20.5, 0.994626224),
  std::pair<double, double> (20.7, 0.994745641),
  std::pair<double, double> (21, 0.994984476),
  std::pair<double, double> (21.2, 0.995103893),
  std::pair<double, double> (21.4, 0.99522331),
  std::pair<double, double> (21.5, 0.995342727),
  std::pair<double, double> (21.8, 0.995462145),
  std::pair<double, double> (21.9, 0.995581562),
  std::pair<double, double> (22.6, 0.995700979),
  std::pair<double, double> (22.8, 0.995820396),
  std::pair<double, double> (23.2, 0.995939814),
  std::pair<double, double> (23.5, 0.996059231),
  std::pair<double, double> (23.8, 0.996178648),
  std::pair<double, double> (23.9, 0.996298065),
  std::pair<double, double> (24, 0.996417483),
  std::pair<double, double> (24.4, 0.9965369),
  std::pair<double, double> (24.5, 0.996656317),
  std::pair<double, double> (24.8, 0.996775734),
  std::pair<double, double> (27.7, 0.996895152),
  std::pair<double, double> (28.2, 0.997014569),
  std::pair<double, double> (28.4, 0.997133986),
  std::pair<double, double> (28.6, 0.997253403),
  std::pair<double, double> (29.2, 0.997372821),
  std::pair<double, double> (29.9, 0.997492238),
  std::pair<double, double> (30, 0.997731072),
  std::pair<double, double> (30.4, 0.99785049),
  std::pair<double, double> (31.4, 0.997969907),
  std::pair<double, double> (32.9, 0.998089324),
  std::pair<double, double> (33.3, 0.998208741),
  std::pair<double, double> (33.7, 0.998328159),
  std::pair<double, double> (35.5, 0.998447576),
  std::pair<double, double> (37.2, 0.998566993),
  std::pair<double, double> (38.2, 0.99868641),
  std::pair<double, double> (40.3, 0.998805828),
  std::pair<double, double> (41.1, 0.998925245),
  std::pair<double, double> (41.7, 0.999044662),
  std::pair<double, double> (41.8, 0.999164079),
  std::pair<double, double> (42.3, 0.999283497),
  std::pair<double, double> (42.8, 0.999402914),
  std::pair<double, double> (45, 0.999522331),
  std::pair<double, double> (48.8, 0.999641748),
  std::pair<double, double> (56.2, 0.999761166),
  std::pair<double, double> (58.8, 0.999880583),
  std::pair<double, double> (60.8, 1.0)
};

const double McpttPusherOrchestratorSpurtCdf::CDF_POINTS_AVG = 4.69;

TypeId
McpttPusherOrchestratorSpurtCdf::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttPusherOrchestratorSpurtCdf")
    .SetParent<McpttPusherOrchestratorInterface> ()
    .AddConstructor<McpttPusherOrchestratorSpurtCdf>()
    .AddAttribute ("ActivityFactor", "The desired activity factor.",
                   DoubleValue (.50),
                   MakeDoubleAccessor (&McpttPusherOrchestratorSpurtCdf::GetActivityFactor,
                                       &McpttPusherOrchestratorSpurtCdf::SetActivityFactor),
                   MakeDoubleChecker<double> ())
  ;

  return tid;
}

McpttPusherOrchestratorSpurtCdf::McpttPusherOrchestratorSpurtCdf (void)
  : McpttPusherOrchestratorInterface (),
    m_avgPttDuration (CDF_POINTS_AVG),
    m_orchestrator (CreateObject<McpttPusherOrchestrator> ())
{
  NS_LOG_FUNCTION (this);

  Ptr<ExponentialRandomVariable> pttIatVariable = CreateObject<ExponentialRandomVariable> ();
  Ptr<EmpiricalRandomVariable> pttDurationVariable = CreateObject<EmpiricalRandomVariable> ();
  pttDurationVariable->SetInterpolate (true);

  for (uint32_t i = 0; i < CDF_POINTS.size (); i++)
    {
      pttDurationVariable->CDF (CDF_POINTS[i].first,
                                CDF_POINTS[i].second);
    }

  m_orchestrator->SetAttribute ("PttDurationVariable", PointerValue (pttDurationVariable));
  m_orchestrator->SetAttribute ("PttInterarrivalTimeVariable", PointerValue (pttIatVariable));
  m_orchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback (&McpttPusherOrchestratorSpurtCdf::PttDurationTrace, this));
  m_orchestrator->TraceConnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback (&McpttPusherOrchestratorSpurtCdf::PttIatTrace, this));
}

McpttPusherOrchestratorSpurtCdf::~McpttPusherOrchestratorSpurtCdf (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusherOrchestratorSpurtCdf::AddPusher (Ptr<McpttPusher> pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  m_orchestrator->AddPusher (pusher);
}

int64_t
McpttPusherOrchestratorSpurtCdf::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);

  return m_orchestrator->AssignStreams (stream);
}

void
McpttPusherOrchestratorSpurtCdf::ChangeCdf (Ptr<EmpiricalRandomVariable> pttDurationVariable, double avgPttDuration)
{
  NS_LOG_FUNCTION (this << pttDurationVariable << avgPttDuration);

  m_orchestrator->SetAttribute ("PttDurationVariable", PointerValue (pttDurationVariable));

  m_avgPttDuration = avgPttDuration;

  UpdatePttIatVariable ();
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSpurtCdf::GetPushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->GetPushers ();
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSpurtCdf::GetActivePushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->GetActivePushers ();
}

Time
McpttPusherOrchestratorSpurtCdf::NextPttIat (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttIat ();
}

Time
McpttPusherOrchestratorSpurtCdf::NextPttDuration (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttDuration ();
}

void
McpttPusherOrchestratorSpurtCdf::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_orchestrator == 0, "No underlying orchestrator.");

  m_orchestrator->Start ();
}

void
McpttPusherOrchestratorSpurtCdf::Stop (void)
{
  NS_LOG_FUNCTION (this);

  m_orchestrator->Stop ();
}

void
McpttPusherOrchestratorSpurtCdf::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_orchestrator->TraceDisconnectWithoutContext ("PttDurationTrace", MakeCallback (&McpttPusherOrchestratorSpurtCdf::PttDurationTrace, this));
  m_orchestrator->TraceDisconnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback (&McpttPusherOrchestratorSpurtCdf::PttIatTrace, this));

  m_orchestrator = 0;
}

void
McpttPusherOrchestratorSpurtCdf::PttDurationTrace (uint32_t userId, Time duration)
{
  NS_LOG_FUNCTION (this << duration);

  TracePttDuration (userId, duration);
}

void
McpttPusherOrchestratorSpurtCdf::PttIatTrace (uint32_t userId, Time iat)
{
  NS_LOG_FUNCTION (this << iat);

  TracePttIat (userId, iat);
}

void
McpttPusherOrchestratorSpurtCdf::UpdatePttIatVariable (void)
{
  NS_LOG_FUNCTION (this);

  double iat;
  PointerValue pttIatVariableValue;
  Ptr<ExponentialRandomVariable> pttIatVariable = 0;

  m_orchestrator->GetAttribute ("PttInterarrivalTimeVariable", pttIatVariableValue);
  pttIatVariable = pttIatVariableValue.Get<ExponentialRandomVariable> ();

  if (m_af <= 0.0)
    {
      NS_ABORT_MSG ("The activity factor must be greater than zero.");
    }
  else if (m_af >= 1)
    {
      iat = 0.0;
    }
  else
    {
      iat = m_avgPttDuration * ((1.0 / m_af) - 1);
    }

  pttIatVariable->SetAttribute ("Mean", DoubleValue (iat));
  pttIatVariable->SetAttribute ("Bound", DoubleValue (0.0));
}

double
McpttPusherOrchestratorSpurtCdf::GetActivityFactor (void) const
{
  NS_LOG_FUNCTION (this);

  return m_af;
}

void
McpttPusherOrchestratorSpurtCdf::SetActivityFactor (double af)
{
  NS_LOG_FUNCTION (this << af);

  m_af = af;

  UpdatePttIatVariable ();
}

} // namespace psc
} // namespace ns3
