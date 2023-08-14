/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
#include "ns3/abort.h"

#include "ns3/point-to-point-epc-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/buildings-helper.h"
#include "ns3/lte-module.h"
#include "ns3/psc-module.h"

#include "schoolshooting-lte-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SchoolShootingLteHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (SchoolShootingLteHelper);

SchoolShootingLteHelper::SchoolShootingLteHelper ()
  : m_initialized (false)
{
  NS_LOG_FUNCTION (this);
}

SchoolShootingLteHelper::~SchoolShootingLteHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SchoolShootingLteHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::SchoolShootingLteHelper")
    .SetParent<SchoolShootingNetworkTechnologyHelper> ()
    .AddConstructor<SchoolShootingLteHelper> ()
    .AddAttribute ("PathLossModelType",
                   "Path loss model to use",
                   StringValue ("ns3::Hybrid3gppPropagationLossModel"),
                   MakeStringAccessor (&SchoolShootingLteHelper::m_pathLossModelType),
                   MakeStringChecker ())
    .AddAttribute ("SchedulerType",
                   "LTE scheduler to use",
                   StringValue ("ns3::RrFfMacScheduler"),
                   MakeStringAccessor (&SchoolShootingLteHelper::m_schedulerType),
                   MakeStringChecker ())
    .AddAttribute ("DlBandwidth",
                   "Number of RBs to use in the DL",
                   UintegerValue (50),
                   MakeUintegerAccessor (&SchoolShootingLteHelper::m_dlBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("UlBandwidth",
                   "Number of RBs to use in the UL",
                   UintegerValue (50),
                   MakeUintegerAccessor (&SchoolShootingLteHelper::m_ulBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlEarfcn",
                   "Downlink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) as per 3GPP 36.101 Section 5.7.3.",
                   UintegerValue (5330),
                   MakeUintegerAccessor (&SchoolShootingLteHelper::m_dlEarfcn),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UlEarfcn",
                   "Uplink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) as per 3GPP 36.101 Section 5.7.3.",
                   UintegerValue (23330),
                   MakeUintegerAccessor (&SchoolShootingLteHelper::m_ulEarfcn),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UeAntennaHeight",
                   "Height of the antenna for the UEs",
                   DoubleValue (1.5),
                   MakeDoubleAccessor (&SchoolShootingLteHelper::m_ueAntennaHeight),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("EnbAntennaHeight",
                   "Height of the antenna for the eNBs",
                   DoubleValue (30),
                   MakeDoubleAccessor (&SchoolShootingLteHelper::m_eNbAntennaHeight),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("UeTransmissionPower",
                   "UE Transmission Power, in dBm",
                   DoubleValue (23),
                   MakeDoubleAccessor (&SchoolShootingLteHelper::m_ueTxPower),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("EnbTransmissionPower",
                   "eNB Transmission Power, in dBm",
                   DoubleValue (46),
                   MakeDoubleAccessor (&SchoolShootingLteHelper::m_eNbTxPower),
                   MakeDoubleChecker<double> (0))
    .AddAttribute ("DefaultTransmissionMode",
                   "Default Transmission Mode (SISO, MIMO Spatial Multiplexing, etc.",
                   UintegerValue (2),
                   MakeUintegerAccessor (&SchoolShootingLteHelper::m_defaultTransmissionMode),
                   MakeUintegerChecker<uint8_t> (0, 2))
    .AddAttribute ("LinkDataRate",
                   "The data rate to be used for the next links to be created",
                   DataRateValue (DataRate ("10Gb/s")),
                   MakeDataRateAccessor (&SchoolShootingLteHelper::m_linkDataRate),
                   MakeDataRateChecker ())
    .AddAttribute ("LinkDelay",
                   "The delay to be used for the next links to be created",
                   TimeValue (MilliSeconds (10)),
                   MakeTimeAccessor (&SchoolShootingLteHelper::m_linkDelay),
                   MakeTimeChecker ())
  ;

  return tid;
}

void
SchoolShootingLteHelper::SetScenarioDefinitionHelper (Ptr<SchoolShootingDefinitionHelper> definitionHelper)
{
  NS_LOG_FUNCTION (this << definitionHelper);

  m_scenarioDefinitionHelper = definitionHelper;
}

Ptr<PscScenarioDefinition> 
SchoolShootingLteHelper::GetScenarioDefinition () const
{
  Ptr<PscScenarioDefinition> scenarioDef = nullptr;
  if (m_scenarioDefinitionHelper != nullptr)
   {
    scenarioDef = m_scenarioDefinitionHelper->GetScenarioDefinition();
   }
  return scenarioDef;
}

  
void 
SchoolShootingLteHelper::Initialize ()
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_UNLESS (!m_initialized, "Error: The scenario has already been initialized");

  m_scenarioDefinitionHelper->CreateScenario ("SchoolShootingLte");

  InitNetwork ();
  InitUes ();
}

NodeContainer SchoolShootingLteHelper::GetEnodeBs ()
{
  return m_enbNodesContainer;
}

Ptr<Node> SchoolShootingLteHelper::GetSgw ()
{
  return m_epcHelper->GetSgwNode ();
}

Ptr<Node> SchoolShootingLteHelper::GetPgw ()
{
  return m_epcHelper->GetPgwNode ();
}

Ptr<Node> SchoolShootingLteHelper::GetPsServer ()
{
  return m_psServer;
}

Ptr<Node> SchoolShootingLteHelper::GetSchoolServer ()
{
  return m_schoolServer;
}

Ptr<Node> SchoolShootingLteHelper::GetCityServer ()
{
  return m_cityServer;
}

Ptr<Node> SchoolShootingLteHelper::GetPublicServer ()
{
  return m_publicServer;
}

NetDeviceContainer SchoolShootingLteHelper::GetNetDevicesForGroup (PscScenarioDefinition::GroupId id)
{
  std::map<PscScenarioDefinition::GroupId, NetDeviceContainer>::iterator it = m_netDeviceGroups.find (id);
  NS_ABORT_MSG_UNLESS (it != m_netDeviceGroups.end (), "Group " << id << " not found in scenario");
  return it->second;
}

void SchoolShootingLteHelper::InitNetwork ()
{
  NS_LOG_FUNCTION (this);

  // Set the SRS periodicity to the highest value, to allow for as many
  // UEs in the same sector as possible
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (320));
  Config::SetDefault ("ns3::LteEnbRrc::DefaultTransmissionMode", UintegerValue (m_defaultTransmissionMode));
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (2000 * 1024));
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (m_eNbTxPower));

  //Must configure S5 link before creating the EPC helper
  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDelay", TimeValue (m_linkDelay));
  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDataRate", DataRateValue (m_linkDataRate));

  m_lteHelper = CreateObject<LteHelper> ();
  m_epcHelper = CreateObject<PointToPointEpcHelper> ();
  m_epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue (15000));
  m_epcHelper->SetAttribute ("S1uLinkDataRate", DataRateValue (m_linkDataRate));
  m_epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (m_linkDelay));
  m_epcHelper->SetAttribute ("X2LinkMtu", UintegerValue (15000));
  m_lteHelper->SetEpcHelper (m_epcHelper);

  // Configure the LTE parameters
  m_lteHelper->SetAttribute ("PathlossModel", StringValue (m_pathLossModelType));
  m_lteHelper->SetAttribute ("UseIdealRrc", BooleanValue (false));
  m_lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (m_dlBandwidth));
  m_lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (m_ulBandwidth));
  m_lteHelper->SetSchedulerType (m_schedulerType);
  m_lteHelper->SetSchedulerAttribute ("HarqEnabled", BooleanValue (true));
  m_lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (m_dlEarfcn));
  m_lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (m_ulEarfcn));
  m_lteHelper->SetUeDeviceAttribute ("DlEarfcn", UintegerValue (m_dlEarfcn));

  // Create the eNodeBs
  m_enbNodesContainer.Create (3);

  //Retrieve the P-GW node
  Ptr<Node> pgw = m_epcHelper->GetPgwNode ();
  Ptr<Node> sgw = m_epcHelper->GetSgwNode ();

  // Create 4 servers: Public safety, school, city, and general public
  NodeContainer serverContainer;
  serverContainer.Create (4);
  m_psServer = serverContainer.Get (0);
  m_schoolServer = serverContainer.Get (1);
  m_cityServer = serverContainer.Get (2);
  m_publicServer = serverContainer.Get (3);
  InternetStackHelper internet;
  internet.Install (serverContainer);

  // Define address helper for the wired nodes
  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));

  Ipv6StaticRoutingHelper ipv6RoutingHelper;

  for (uint16_t i = 0; i < serverContainer.GetN (); i++)
    {
      // Create the P2P links between P-GW and servers
      PointToPointHelper p2ph;
      p2ph.SetDeviceAttribute ("DataRate", DataRateValue (m_linkDataRate));
      p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
      if (i == 0)
        { //The public safety server is directly connected to the P-GW
          p2ph.SetChannelAttribute ("Delay", TimeValue (m_linkDelay));
        }
      else
        { //We assume other servers have to go through internet and takes more time
          p2ph.SetChannelAttribute ("Delay", TimeValue (2 * m_linkDelay));
        }
      NetDeviceContainer internetDevices = p2ph.Install (pgw, serverContainer.Get (i));

      // Assign IPv6 addresses
      Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);
      // ...and configure the routing
      internetIpIfaces.SetForwarding (0, true);
      internetIpIfaces.SetDefaultRouteInAllNodes (0);
      //Define route from server to UEs
      Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (serverContainer.Get (i)->GetObject<Ipv6> ());
      remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);
      //Define route from P-GW to the server
      Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
      pgwStaticRouting->AddNetworkRouteTo (internetIpIfaces.GetAddress (1, 1), Ipv6Prefix (128), internetIpIfaces.GetAddress (1, 1), 2 + i, 0);
    }

  //Set locations for the eNodeB, S-GW, P-GW, and servers
  //Note that location for wired nodes does not affect simulation results
  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (-100, -100.0, 32.0));
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (m_enbNodesContainer);
  //S-GW
  Ptr<ListPositionAllocator> positionAllocSgw = CreateObject<ListPositionAllocator> ();
  positionAllocSgw->Add (Vector (-270, 7.5, 1.5));
  MobilityHelper mobilitySgw;
  mobilitySgw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilitySgw.SetPositionAllocator (positionAllocSgw);
  mobilitySgw.Install (sgw);
  //P-GW
  Ptr<ListPositionAllocator> positionAllocPgw = CreateObject<ListPositionAllocator> ();
  positionAllocPgw->Add (Vector (-280, 7.5, 1.5));
  MobilityHelper mobilityPgw;
  mobilityPgw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityPgw.SetPositionAllocator (positionAllocPgw);
  mobilityPgw.Install (pgw);
  //Servers
  Ptr<ListPositionAllocator> positionAllocServers = CreateObject<ListPositionAllocator> ();
  for (uint32_t rm = 0; rm < serverContainer.GetN (); rm++)
    {
      positionAllocServers->Add (Vector (-290, rm * 5, 1.0));
    }
  MobilityHelper mobilityServers;
  mobilityServers.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityServers.SetPositionAllocator (positionAllocServers);
  mobilityServers.Install (serverContainer);

  //Configure LTE
  m_lteHelper->SetEnbAntennaModelType ("ns3::Parabolic3dAntennaModel");
  m_lteHelper->SetEnbAntennaModelAttribute ("BoresightGain", DoubleValue (16));
  for (uint32_t i = 0; i < m_enbNodesContainer.GetN (); i++)
    {
      m_lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue ( int(i * 120) % 360 ));
      m_lteHelper->InstallEnbDevice (m_enbNodesContainer.Get (i));
    }

  BuildingsHelper::Install (m_enbNodesContainer);
}

void SchoolShootingLteHelper::InitUes ()
{
  NS_LOG_FUNCTION (this);

  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (m_ueTxPower));

  //build list of all the nodes so it is easier to assign internet stack
  NodeContainer allNodes;
  NodeContainer nodes;
  NetDeviceContainer netdevices;
  NetDeviceContainer allNetDevices;

  //Configure vehicles
  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM_VEHICLE, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition ()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE_VEHICLES).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE_VEHICLES, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SA_VEHICLE, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::EMS_VEHICLES, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE_VEHICLES).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::FIRE_VEHICLES, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::HELICOPTER).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::HELICOPTER, netdevices));

  //Configure handheld devices
  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::DEPLOYABLE_CAMERA).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::DEPLOYABLE_CAMERA, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_1, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_2, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_3, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SWAT_TEAM_4, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::UC_TEAM, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::TASK_FORCE, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::EMS).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::EMS, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::FIRE).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::FIRE, netdevices));

  //The device for the offender will be the throw phone
  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::OFFENDER).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::OFFENDER, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::SCHOOL_CAMERAS).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::SCHOOL_CAMERAS, netdevices));

  nodes = m_scenarioDefinitionHelper->GetScenarioDefinition()->GetNodesForGroup (SchoolShootingDefinitionHelper::UserGroups::TRAFFIC_CAMERAS).nodes;
  allNodes.Add (nodes);
  netdevices = m_lteHelper->InstallUeDevice (nodes);
  allNetDevices.Add (netdevices);
  m_netDeviceGroups.insert (std::pair<SchoolShootingDefinitionHelper::UserGroups, NetDeviceContainer> (SchoolShootingDefinitionHelper::UserGroups::TRAFFIC_CAMERAS, netdevices));

  //TODO: configure batteries


  // For this scenario, the school and traffic cameras are assumed to be connected via wired network
  // to their respective servers so we do not need to create NetDevices. Even if they were wireless,
  // they most likely would be connected to a commercial network

  InternetStackHelper internetHelper;
  // Install the Internet stack, assign addresses, and configure routing in the UEs
  internetHelper.Install (allNodes);
  Ipv6InterfaceContainer ueIpv6Ifaces;

  ueIpv6Ifaces = m_epcHelper->AssignUeIpv6Address (NetDeviceContainer (allNetDevices));

  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  for (uint32_t u = 0; u < allNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = allNodes.Get (u);
      // Set the default gateway for the UEs
      Ptr<Ipv6StaticRouting> ueStaticRouting = ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (m_epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  m_lteHelper->Attach (allNetDevices);

  BuildingsHelper::Install (allNodes);
}

void
SchoolShootingLteHelper::EnableLteTraces (void)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_IF (!m_lteHelper, "LteHelper cannot be NULL!");

  m_lteHelper->EnableTraces ();
}

} // namespace psc
} // namespace ns3
