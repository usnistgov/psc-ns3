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

#include "mcptt-pusher-orchestrator-session-cdf.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusherOrchestratorSessionCdf");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttPusherOrchestratorSessionCdf);

const std::vector<std::pair<double, double> > McpttPusherOrchestratorSessionCdf::CDF_POINTS = {
  std::pair<double, double> (1.8, 0.001302649),
  std::pair<double, double> (1.9, 0.001519757),
  std::pair<double, double> (2.1, 0.004125054),
  std::pair<double, double> (2.2, 0.009335649),
  std::pair<double, double> (2.3, 0.012158055),
  std::pair<double, double> (2.4, 0.016283109),
  std::pair<double, double> (2.5, 0.018237082),
  std::pair<double, double> (2.6, 0.021276596),
  std::pair<double, double> (2.7, 0.028441164),
  std::pair<double, double> (2.8, 0.035605732),
  std::pair<double, double> (2.9, 0.044724273),
  std::pair<double, double> (3, 0.053842814),
  std::pair<double, double> (3.1, 0.066217977),
  std::pair<double, double> (3.2, 0.08315241),
  std::pair<double, double> (3.3, 0.096396005),
  std::pair<double, double> (3.4, 0.113547547),
  std::pair<double, double> (3.5, 0.130916196),
  std::pair<double, double> (3.6, 0.151107251),
  std::pair<double, double> (3.7, 0.165870604),
  std::pair<double, double> (3.8, 0.186495875),
  std::pair<double, double> (3.9, 0.200607903),
  std::pair<double, double> (4, 0.215588363),
  std::pair<double, double> (4.1, 0.232522796),
  std::pair<double, double> (4.2, 0.24945723),
  std::pair<double, double> (4.3, 0.26899696),
  std::pair<double, double> (4.4, 0.279201042),
  std::pair<double, double> (4.5, 0.29787234),
  std::pair<double, double> (4.6, 0.311550152),
  std::pair<double, double> (4.7, 0.32913591),
  std::pair<double, double> (4.8, 0.339339991),
  std::pair<double, double> (4.9, 0.35453756),
  std::pair<double, double> (5, 0.368866696),
  std::pair<double, double> (5.1, 0.376682588),
  std::pair<double, double> (5.2, 0.388189318),
  std::pair<double, double> (5.3, 0.399478941),
  std::pair<double, double> (5.4, 0.40990013),
  std::pair<double, double> (5.5, 0.420538428),
  std::pair<double, double> (5.6, 0.445505862),
  std::pair<double, double> (5.7, 0.451367781),
  std::pair<double, double> (5.8, 0.458315241),
  std::pair<double, double> (5.9, 0.466999566),
  std::pair<double, double> (6, 0.474164134),
  std::pair<double, double> (6.1, 0.482197134),
  std::pair<double, double> (6.2, 0.490664351),
  std::pair<double, double> (6.3, 0.496960486),
  std::pair<double, double> (6.4, 0.505427703),
  std::pair<double, double> (6.5, 0.513026487),
  std::pair<double, double> (6.6, 0.522145028),
  std::pair<double, double> (6.7, 0.531046461),
  std::pair<double, double> (6.8, 0.537776813),
  std::pair<double, double> (6.9, 0.545375597),
  std::pair<double, double> (7, 0.554494138),
  std::pair<double, double> (7.1, 0.560573165),
  std::pair<double, double> (7.2, 0.567737733),
  std::pair<double, double> (7.3, 0.57620495),
  std::pair<double, double> (7.4, 0.582718194),
  std::pair<double, double> (7.5, 0.591836735),
  std::pair<double, double> (7.6, 0.598784195),
  std::pair<double, double> (7.7, 0.603126357),
  std::pair<double, double> (7.8, 0.608336952),
  std::pair<double, double> (7.9, 0.61550152),
  std::pair<double, double> (8, 0.620712115),
  std::pair<double, double> (8.1, 0.62700825),
  std::pair<double, double> (8.2, 0.631567521),
  std::pair<double, double> (8.3, 0.639817629),
  std::pair<double, double> (8.4, 0.645245332),
  std::pair<double, double> (8.5, 0.650455927),
  std::pair<double, double> (8.6, 0.657837603),
  std::pair<double, double> (8.7, 0.662396874),
  std::pair<double, double> (8.8, 0.668041685),
  std::pair<double, double> (8.9, 0.671949631),
  std::pair<double, double> (9, 0.67781155),
  std::pair<double, double> (9.1, 0.681502388),
  std::pair<double, double> (9.2, 0.687798524),
  std::pair<double, double> (9.3, 0.692357794),
  std::pair<double, double> (9.4, 0.696917065),
  std::pair<double, double> (9.5, 0.701042119),
  std::pair<double, double> (9.6, 0.704298741),
  std::pair<double, double> (9.7, 0.709509336),
  std::pair<double, double> (9.8, 0.712331741),
  std::pair<double, double> (9.9, 0.714502822),
  std::pair<double, double> (10, 0.719496309),
  std::pair<double, double> (10.1, 0.723621363),
  std::pair<double, double> (10.2, 0.727095093),
  std::pair<double, double> (10.3, 0.730785931),
  std::pair<double, double> (10.4, 0.73556231),
  std::pair<double, double> (10.5, 0.73903604),
  std::pair<double, double> (10.6, 0.74359531),
  std::pair<double, double> (10.7, 0.747937473),
  std::pair<double, double> (10.8, 0.752496743),
  std::pair<double, double> (10.9, 0.755753365),
  std::pair<double, double> (11, 0.759878419),
  std::pair<double, double> (11.1, 0.763135041),
  std::pair<double, double> (11.2, 0.766608771),
  std::pair<double, double> (11.3, 0.769865393),
  std::pair<double, double> (11.4, 0.771602258),
  std::pair<double, double> (11.5, 0.775075988),
  std::pair<double, double> (11.6, 0.777464177),
  std::pair<double, double> (11.7, 0.78180634),
  std::pair<double, double> (11.8, 0.785931394),
  std::pair<double, double> (11.9, 0.788319583),
  std::pair<double, double> (12, 0.791359097),
  std::pair<double, double> (12.1, 0.794181502),
  std::pair<double, double> (12.2, 0.797003908),
  std::pair<double, double> (12.3, 0.799174989),
  std::pair<double, double> (12.4, 0.801563178),
  std::pair<double, double> (12.5, 0.805036908),
  std::pair<double, double> (12.6, 0.809379071),
  std::pair<double, double> (12.7, 0.813069909),
  std::pair<double, double> (12.8, 0.814589666),
  std::pair<double, double> (12.9, 0.818497612),
  std::pair<double, double> (13, 0.820234477),
  std::pair<double, double> (13.1, 0.822622666),
  std::pair<double, double> (13.2, 0.824359531),
  std::pair<double, double> (13.3, 0.828267477),
  std::pair<double, double> (13.4, 0.83022145),
  std::pair<double, double> (13.5, 0.833043856),
  std::pair<double, double> (13.6, 0.834780721),
  std::pair<double, double> (13.7, 0.838254451),
  std::pair<double, double> (13.8, 0.841728181),
  std::pair<double, double> (13.9, 0.844767694),
  std::pair<double, double> (14, 0.847372992),
  std::pair<double, double> (14.1, 0.849761181),
  std::pair<double, double> (14.2, 0.851715154),
  std::pair<double, double> (14.3, 0.853452019),
  std::pair<double, double> (14.4, 0.855405992),
  std::pair<double, double> (14.5, 0.857142857),
  std::pair<double, double> (14.6, 0.859965263),
  std::pair<double, double> (14.7, 0.861702128),
  std::pair<double, double> (14.8, 0.862787668),
  std::pair<double, double> (14.9, 0.863873209),
  std::pair<double, double> (15, 0.865392966),
  std::pair<double, double> (15.1, 0.867998263),
  std::pair<double, double> (15.2, 0.870169344),
  std::pair<double, double> (15.3, 0.872774642),
  std::pair<double, double> (15.4, 0.874511507),
  std::pair<double, double> (15.5, 0.877333912),
  std::pair<double, double> (15.6, 0.87993921),
  std::pair<double, double> (15.7, 0.882327399),
  std::pair<double, double> (15.8, 0.884281372),
  std::pair<double, double> (15.9, 0.885366913),
  std::pair<double, double> (16, 0.88797221),
  std::pair<double, double> (16.1, 0.889926183),
  std::pair<double, double> (16.2, 0.891228832),
  std::pair<double, double> (16.3, 0.892748589),
  std::pair<double, double> (16.4, 0.894702562),
  std::pair<double, double> (16.5, 0.896222319),
  std::pair<double, double> (16.6, 0.899478941),
  std::pair<double, double> (16.7, 0.901432914),
  std::pair<double, double> (16.8, 0.902735562),
  std::pair<double, double> (16.9, 0.904689535),
  std::pair<double, double> (17, 0.905775076),
  std::pair<double, double> (17.1, 0.906860617),
  std::pair<double, double> (17.2, 0.907511941),
  std::pair<double, double> (17.3, 0.909031698),
  std::pair<double, double> (17.4, 0.910334347),
  std::pair<double, double> (17.5, 0.911419887),
  std::pair<double, double> (17.6, 0.913156752),
  std::pair<double, double> (17.7, 0.914676509),
  std::pair<double, double> (17.8, 0.916413374),
  std::pair<double, double> (17.9, 0.918150239),
  std::pair<double, double> (18, 0.919452888),
  std::pair<double, double> (18.1, 0.920104212),
  std::pair<double, double> (18.2, 0.920972644),
  std::pair<double, double> (18.3, 0.922058185),
  std::pair<double, double> (18.4, 0.923360834),
  std::pair<double, double> (18.5, 0.924880591),
  std::pair<double, double> (18.6, 0.925531915),
  std::pair<double, double> (18.7, 0.927485888),
  std::pair<double, double> (18.8, 0.928788537),
  std::pair<double, double> (18.9, 0.929874077),
  std::pair<double, double> (19, 0.931176726),
  std::pair<double, double> (19.1, 0.932045158),
  std::pair<double, double> (19.2, 0.932696483),
  std::pair<double, double> (19.3, 0.933564915),
  std::pair<double, double> (19.4, 0.934433348),
  std::pair<double, double> (19.5, 0.935518888),
  std::pair<double, double> (19.6, 0.936170213),
  std::pair<double, double> (19.7, 0.936821537),
  std::pair<double, double> (19.8, 0.937907078),
  std::pair<double, double> (19.9, 0.939426835),
  std::pair<double, double> (20, 0.939643943),
  std::pair<double, double> (20.1, 0.939861051),
  std::pair<double, double> (20.2, 0.940512375),
  std::pair<double, double> (20.3, 0.9411637),
  std::pair<double, double> (20.4, 0.941815024),
  std::pair<double, double> (20.5, 0.942683456),
  std::pair<double, double> (20.6, 0.943986105),
  std::pair<double, double> (20.7, 0.944854538),
  std::pair<double, double> (20.8, 0.945288754),
  std::pair<double, double> (20.9, 0.946157186),
  std::pair<double, double> (21, 0.947242727),
  std::pair<double, double> (21.1, 0.948111159),
  std::pair<double, double> (21.2, 0.948979592),
  std::pair<double, double> (21.3, 0.950716457),
  std::pair<double, double> (21.4, 0.951801997),
  std::pair<double, double> (21.5, 0.952453322),
  std::pair<double, double> (21.6, 0.953104646),
  std::pair<double, double> (21.7, 0.95375597),
  std::pair<double, double> (21.8, 0.954624403),
  std::pair<double, double> (21.9, 0.956361268),
  std::pair<double, double> (22, 0.957012592),
  std::pair<double, double> (22.1, 0.957663917),
  std::pair<double, double> (22.2, 0.958315241),
  std::pair<double, double> (22.3, 0.958749457),
  std::pair<double, double> (22.5, 0.959400782),
  std::pair<double, double> (22.6, 0.95961789),
  std::pair<double, double> (22.7, 0.960052106),
  std::pair<double, double> (22.8, 0.961354755),
  std::pair<double, double> (22.9, 0.962440295),
  std::pair<double, double> (23, 0.963525836),
  std::pair<double, double> (23.1, 0.963742944),
  std::pair<double, double> (23.2, 0.96417716),
  std::pair<double, double> (23.3, 0.964394268),
  std::pair<double, double> (23.4, 0.965045593),
  std::pair<double, double> (23.5, 0.965479809),
  std::pair<double, double> (23.6, 0.965696917),
  std::pair<double, double> (23.7, 0.966348241),
  std::pair<double, double> (23.8, 0.966782458),
  std::pair<double, double> (23.9, 0.967433782),
  std::pair<double, double> (24, 0.967867998),
  std::pair<double, double> (24.1, 0.968085106),
  std::pair<double, double> (24.2, 0.968519323),
  std::pair<double, double> (24.3, 0.968736431),
  std::pair<double, double> (24.4, 0.969821971),
  std::pair<double, double> (24.5, 0.970256188),
  std::pair<double, double> (24.6, 0.970907512),
  std::pair<double, double> (24.7, 0.971558836),
  std::pair<double, double> (24.8, 0.971993053),
  std::pair<double, double> (24.9, 0.972210161),
  std::pair<double, double> (25, 0.972644377),
  std::pair<double, double> (25.1, 0.972861485),
  std::pair<double, double> (25.2, 0.973295701),
  std::pair<double, double> (25.5, 0.973512809),
  std::pair<double, double> (25.6, 0.973947026),
  std::pair<double, double> (25.9, 0.974381242),
  std::pair<double, double> (26, 0.97459835),
  std::pair<double, double> (26.1, 0.974815458),
  std::pair<double, double> (26.2, 0.975032566),
  std::pair<double, double> (26.3, 0.975249674),
  std::pair<double, double> (26.4, 0.975900999),
  std::pair<double, double> (26.6, 0.976552323),
  std::pair<double, double> (26.8, 0.976769431),
  std::pair<double, double> (26.9, 0.977203647),
  std::pair<double, double> (27.1, 0.977637864),
  std::pair<double, double> (27.3, 0.97807208),
  std::pair<double, double> (27.4, 0.978506296),
  std::pair<double, double> (27.8, 0.978940512),
  std::pair<double, double> (27.9, 0.97915762),
  std::pair<double, double> (28, 0.979808945),
  std::pair<double, double> (28.1, 0.980243161),
  std::pair<double, double> (28.3, 0.980894485),
  std::pair<double, double> (28.4, 0.981328702),
  std::pair<double, double> (28.5, 0.98154581),
  std::pair<double, double> (28.6, 0.981980026),
  std::pair<double, double> (28.7, 0.982197134),
  std::pair<double, double> (28.9, 0.982848459),
  std::pair<double, double> (29.1, 0.983065567),
  std::pair<double, double> (29.2, 0.983282675),
  std::pair<double, double> (29.4, 0.983499783),
  std::pair<double, double> (29.5, 0.983716891),
  std::pair<double, double> (29.6, 0.983933999),
  std::pair<double, double> (29.8, 0.984368215),
  std::pair<double, double> (30, 0.984585323),
  std::pair<double, double> (30.1, 0.984802432),
  std::pair<double, double> (30.2, 0.985236648),
  std::pair<double, double> (30.3, 0.985453756),
  std::pair<double, double> (30.5, 0.985670864),
  std::pair<double, double> (30.9, 0.985887972),
  std::pair<double, double> (31, 0.98610508),
  std::pair<double, double> (31.1, 0.986539297),
  std::pair<double, double> (31.6, 0.986756405),
  std::pair<double, double> (31.7, 0.986973513),
  std::pair<double, double> (32.1, 0.987190621),
  std::pair<double, double> (32.4, 0.987624837),
  std::pair<double, double> (32.6, 0.988059053),
  std::pair<double, double> (33.1, 0.988276162),
  std::pair<double, double> (33.2, 0.988710378),
  std::pair<double, double> (33.6, 0.988927486),
  std::pair<double, double> (33.7, 0.989144594),
  std::pair<double, double> (33.8, 0.989361702),
  std::pair<double, double> (33.9, 0.98957881),
  std::pair<double, double> (35.1, 0.989795918),
  std::pair<double, double> (35.3, 0.990230135),
  std::pair<double, double> (35.5, 0.990447243),
  std::pair<double, double> (35.6, 0.990664351),
  std::pair<double, double> (35.9, 0.990881459),
  std::pair<double, double> (36.4, 0.991098567),
  std::pair<double, double> (36.8, 0.991315675),
  std::pair<double, double> (36.9, 0.991532783),
  std::pair<double, double> (37.1, 0.991749891),
  std::pair<double, double> (37.7, 0.991967),
  std::pair<double, double> (39.2, 0.992184108),
  std::pair<double, double> (39.4, 0.992401216),
  std::pair<double, double> (39.9, 0.992618324),
  std::pair<double, double> (40.2, 0.992835432),
  std::pair<double, double> (40.6, 0.99305254),
  std::pair<double, double> (41.1, 0.993486756),
  std::pair<double, double> (42.5, 0.993920973),
  std::pair<double, double> (42.7, 0.994138081),
  std::pair<double, double> (43.9, 0.994355189),
  std::pair<double, double> (44.3, 0.994572297),
  std::pair<double, double> (44.5, 0.994789405),
  std::pair<double, double> (44.9, 0.995006513),
  std::pair<double, double> (45, 0.995223621),
  std::pair<double, double> (45.6, 0.995440729),
  std::pair<double, double> (46.2, 0.995657838),
  std::pair<double, double> (46.4, 0.995874946),
  std::pair<double, double> (47.2, 0.996092054),
  std::pair<double, double> (48.2, 0.996309162),
  std::pair<double, double> (48.8, 0.99652627),
  std::pair<double, double> (50, 0.996743378),
  std::pair<double, double> (51.1, 0.996960486),
  std::pair<double, double> (51.4, 0.997177594),
  std::pair<double, double> (51.6, 0.997394703),
  std::pair<double, double> (52.8, 0.997611811),
  std::pair<double, double> (52.9, 0.997828919),
  std::pair<double, double> (55, 0.998046027),
  std::pair<double, double> (58.2, 0.998263135),
  std::pair<double, double> (58.7, 0.998480243),
  std::pair<double, double> (58.8, 0.998697351),
  std::pair<double, double> (59.9, 0.998914459),
  std::pair<double, double> (60.8, 0.999131568),
  std::pair<double, double> (61.7, 0.999348676),
  std::pair<double, double> (62.8, 0.999565784),
  std::pair<double, double> (78.6, 0.999782892),
  std::pair<double, double> (79.5, 1.0)
};

const double McpttPusherOrchestratorSessionCdf::CDF_POINTS_AVG = 8.58;

TypeId
McpttPusherOrchestratorSessionCdf::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttPusherOrchestratorSessionCdf")
    .SetParent<McpttPusherOrchestratorInterface> ()
    .AddConstructor<McpttPusherOrchestratorSessionCdf>()
    .AddAttribute ("ActivityFactor", "The desired activity factor.",
                   DoubleValue (.50),
                   MakeDoubleAccessor (&McpttPusherOrchestratorSessionCdf::GetActivityFactor,
                                       &McpttPusherOrchestratorSessionCdf::SetActivityFactor),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Orchestrator", "The underlying orchestrator to contend with.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttPusherOrchestratorSessionCdf::GetOrchestrator,
                                        &McpttPusherOrchestratorSessionCdf::SetOrchestrator),
                   MakePointerChecker<McpttPusherOrchestratorInterface> ())
    .AddTraceSource ("SessionInterarrivalTimeTrace",
                     "The trace for capturing session interarrival times.",
                     MakeTraceSourceAccessor (&McpttPusherOrchestratorSessionCdf::m_sessionIatTrace),
                     "ns3::Time::TracedCallback")
    .AddTraceSource ("SessionDurationTrace",
                     "The trace for capturing session durations.",
                     MakeTraceSourceAccessor (&McpttPusherOrchestratorSessionCdf::m_sessionDurationTrace),
                     "ns3::Time::TracedCallback")
    .AddTraceSource ("SessionState", "Trace state changes of active sessions state",
                     MakeTraceSourceAccessor (&McpttPusherOrchestratorSessionCdf::m_active),
                     "ns3::TracedValueCallback::Boolean")
  ;

  return tid;
}

McpttPusherOrchestratorSessionCdf::McpttPusherOrchestratorSessionCdf (void)
  : McpttPusherOrchestratorInterface (),
    m_avgSessionDuration (CDF_POINTS_AVG),
    m_nextEvent (EventId ()),
    m_sessionDurationVariable (CreateObject<EmpiricalRandomVariable> ()),
    m_sessionIatVariable (CreateObject<ExponentialRandomVariable> ())
{
  NS_LOG_FUNCTION (this);

  for (uint32_t i = 0; i < CDF_POINTS.size (); i++)
    {
      m_sessionDurationVariable->CDF (CDF_POINTS[i].first,
                                      CDF_POINTS[i].second);
    }
}

McpttPusherOrchestratorSessionCdf::~McpttPusherOrchestratorSessionCdf (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusherOrchestratorSessionCdf::AddPusher (Ptr<McpttPusher> pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  m_orchestrator->AddPusher (pusher);
}

int64_t
McpttPusherOrchestratorSessionCdf::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);

  int64_t streams = m_orchestrator->AssignStreams (stream);
  streams++;
  m_sessionDurationVariable->SetStream (stream + streams);
  streams++;
  m_sessionIatVariable->SetStream (stream + streams);

  return streams;
}

void
McpttPusherOrchestratorSessionCdf::ChangeCdf (Ptr<EmpiricalRandomVariable> sessionDurationVariable, double avgSessionDuration)
{
  NS_LOG_FUNCTION (this << sessionDurationVariable << avgSessionDuration);

  m_sessionDurationVariable = sessionDurationVariable;
  m_avgSessionDuration = avgSessionDuration;

  UpdateSessionIatVariable ();
}
std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSessionCdf::GetPushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->GetPushers ();
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSessionCdf::GetActivePushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->GetActivePushers ();
}

Time
McpttPusherOrchestratorSessionCdf::NextPttIat (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttIat ();
}

Time
McpttPusherOrchestratorSessionCdf::NextPttDuration (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttDuration ();
}

void
McpttPusherOrchestratorSessionCdf::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_orchestrator == 0, "No underlying orchestrator.");

  StopSession ();
}

void
McpttPusherOrchestratorSessionCdf::Stop (void)
{
  NS_LOG_FUNCTION (this);

  m_nextEvent.Cancel ();

  if (m_active)
    {
      m_orchestrator->Stop ();
      m_active = false;
    }
}

void
McpttPusherOrchestratorSessionCdf::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_nextEvent.Cancel ();
  m_orchestrator = 0;
  m_sessionDurationVariable = 0;
  m_sessionIatVariable = 0;
}

void
McpttPusherOrchestratorSessionCdf::StartSession (void)
{
  NS_LOG_FUNCTION (this);

  Time sessionDuration = Seconds (m_sessionDurationVariable->GetValue ());
  m_nextEvent = Simulator::Schedule (sessionDuration, &McpttPusherOrchestratorSessionCdf::StopSession, this);

  m_active = true;

  m_orchestrator->Start ();

  m_sessionDurationTrace (sessionDuration);
}

void
McpttPusherOrchestratorSessionCdf::StopSession (void)
{
  Time sessionIat = Seconds (m_sessionIatVariable->GetValue ());
  m_nextEvent = Simulator::Schedule (sessionIat, &McpttPusherOrchestratorSessionCdf::StartSession, this);

  m_orchestrator->Stop ();

  m_active = false;

  m_sessionIatTrace (sessionIat);
}

void
McpttPusherOrchestratorSessionCdf::UpdateSessionIatVariable (void)
{
  NS_LOG_FUNCTION (this);

  double iat;
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
      iat = m_avgSessionDuration * ((1.0 / m_af) - 1);
    }

  m_sessionIatVariable->SetAttribute ("Mean", DoubleValue (iat));
  m_sessionIatVariable->SetAttribute ("Bound", DoubleValue (0.0));
}

double
McpttPusherOrchestratorSessionCdf::GetActivityFactor (void) const
{
  NS_LOG_FUNCTION (this);

  return m_af;
}

Ptr<McpttPusherOrchestratorInterface>
McpttPusherOrchestratorSessionCdf::GetOrchestrator (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator;
}

void
McpttPusherOrchestratorSessionCdf::SetActivityFactor (double af)
{
  NS_LOG_FUNCTION (this << af);

  m_af = af;

  UpdateSessionIatVariable ();
}

void
McpttPusherOrchestratorSessionCdf::SetOrchestrator (Ptr<McpttPusherOrchestratorInterface> orchestrator)
{
  NS_LOG_FUNCTION (this << orchestrator);

  m_orchestrator = orchestrator;
}

} // namespace psc
} // namespace ns3
