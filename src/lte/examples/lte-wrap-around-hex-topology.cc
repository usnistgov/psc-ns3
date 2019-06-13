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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store-module.h"

using namespace ns3;

/**
 * Sample simulation script to demonstrate the use wrap-around functionality
 * of of Lte3gppHexGridEnbTopologyHelper (i.e., hexagonal cells in ring topology).
 * Only the LTE radio access will be simulated with RLC SM.
 * Users can configure the following Global Values through command-line, e.g,
 * ./waf --run "example-wrap-around-hex-topology --nUesPerSector=2 --nRings=2 --enableTopologyLogs=1 --enableChannelLogs=0 --simTime=1 --RngRun=1"
 * Alternatively these values can also be configured through ns-3 ConfigStore system.
 */

NS_LOG_COMPONENT_DEFINE ("WrapAroundTopologyExample");

static ns3::GlobalValue g_nRings ("nRings",
                                  "Number of Rings of hexagonal cells. First ring contains 1 cell(i.e. 3 eNB nodes), "
                                  "2nd has 6 cells, 3rd has 12 cells and so on.",
                                  ns3::UintegerValue (1),
                                  ns3::MakeUintegerChecker<uint32_t> ());
static ns3::GlobalValue g_nUesPerSector ("nUesPerSector",
                                         "Number of UEs per sector of a hexagonal cells.",
                                         ns3::UintegerValue (1),
                                         ns3::MakeUintegerChecker<uint32_t> ());
static ns3::GlobalValue g_enbTxPowerDbm ("enbTxPowerDbm",
                                         "TX power [dBm] used by eNBs",
                                         ns3::DoubleValue (46.0),
                                         ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_ueTxPowerDbm ("ueTxPowerDbm",
                                        "TX power [dBm] used by UEs",
                                        ns3::DoubleValue (23.0),
                                        ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_enbDlEarfcn ("enbDlEarfcn",
                                       "DL EARFCN used by eNBs",
                                       ns3::UintegerValue (100),
                                       ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_enbBandwidth ("enbBandwidth",
                                        "bandwidth [num RBs] used by eNBs",
                                        ns3::UintegerValue (100),
                                        ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_srsPeriodicity ("srsPeriodicity",
                                          "SRS Periodicity (has to be at least "
                                          "greater than the number of UEs per eNB)",
                                          ns3::UintegerValue (160),
                                          ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_minDistance ("minDistance",
                                       "The minimum distance in meters between eNodeB and UE.",
                                       ns3::DoubleValue (12.0),
                                       ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_interSiteDistance ("interSiteDistance",
                                             "The minimum distance in meters between cells.",
                                             ns3::DoubleValue (500.0),
                                             ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_simTime ("simTime",
                                   "Total duration of the simulation [s]",
                                   ns3::DoubleValue (1),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_enableChannelLogs ("enableChannelLogs",
                                             "whether to generate channel logs when EPC is enabled without generating the REM.",
                                             ns3::BooleanValue (false),
                                             ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_enableTopologyLogs ("enableTopologyLogs",
                                              "whether to generate topology logs.",
                                              ns3::BooleanValue (false),
                                              ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_enableLteTraces ("enableLteTraces",
                                           "if true, will generate LTE traces;"
                                           "if false, will run the simulation without generating LTE traces",
                                           ns3::BooleanValue (false),
                                           ns3::MakeBooleanChecker ());

/// Pathloss structure
struct Pathloss
{
  Time time; ///< Time stamp when the pathloss trace was triggered
  double ploss; ///< Pathloss value in dB
  Ptr<Node> n1; ///< Pointer to the first node
  Ptr<Node> n2; ///< Pointer to the second node
  double dist; ///< Distance between the nodes in meter
  bool aIndoor; ///< Flag to indicate if the first node is indoor
  bool bIndoor; ///< Flag to indicate if the second node is indoor
};

/// GainLogs structure
struct GainLogs
{
  Time time; ///< Time stamp when the gain trace was triggered
  Ptr<const MobilityModel> tx; ///< Pointer to the mobility model of the transmitter
  Ptr<const MobilityModel> rx; ///< Pointer to the mobility model of the receiver
  double txAntennaGain; ///< Tx antenna gain in dB
  double rxAntennaGain; ///< Rx antenna gain in dB
  double propagationGain; ///< Propagation gain in dB
  double pathloss; ///< Pathloss in dB
};

/// ConnectionLogs structure
struct ConnectionLogs
{
  std::string context; ///< The context
  uint64_t imsi; ///< IMSI of the UE
  uint16_t cellId; ///< The cell id
  uint16_t rnti; ///< The RNTI of the UE
};


std::vector<Pathloss> g_dlPathlossLogs;
std::vector<Pathloss> g_ulPathlossLogs;
std::vector<GainLogs> g_dlgainLogs;
std::vector<GainLogs> g_ulgainLogs;
std::vector<ConnectionLogs> g_connectionLogs;


void
GetPathlossTrace (bool isDownlink, std::string context, const double ploss, Ptr<Node> n1, Ptr<Node> n2, double dist, bool aIndoor, bool bIndoor)
{
  if (isDownlink)
    {
      Pathloss dlPathloss;
      dlPathloss.time = Simulator::Now ();
      dlPathloss.ploss = ploss;
      dlPathloss.n1 = n1;
      dlPathloss.n2 = n2;
      dlPathloss.dist = dist;
      dlPathloss.aIndoor = aIndoor;
      dlPathloss.bIndoor = bIndoor;

      g_dlPathlossLogs.push_back (dlPathloss);
    }
  else
    {
      Pathloss ulPathloss;
      ulPathloss.time = Simulator::Now ();
      ulPathloss.ploss = ploss;
      ulPathloss.n1 = n1;
      ulPathloss.n2 = n2;
      ulPathloss.dist = dist;
      ulPathloss.aIndoor = aIndoor;
      ulPathloss.bIndoor = bIndoor;

      g_ulPathlossLogs.push_back (ulPathloss);

    }
}

void
GetGainTrace (bool isDownlink, std::string context, Ptr<const MobilityModel> tx,Ptr<const MobilityModel> rx, double txAntennaGain, double rxAntennaGain, double propagationGain, double pathloss)
{

  if (isDownlink)
    {
      GainLogs dlGain;
      dlGain.time = Simulator::Now ();
      dlGain.tx = tx;
      dlGain.rx = rx;
      dlGain.txAntennaGain = txAntennaGain;
      dlGain.rxAntennaGain = rxAntennaGain;
      dlGain.propagationGain = propagationGain;
      dlGain.pathloss = pathloss;

      g_dlgainLogs.push_back (dlGain);
    }
  else
    {
      GainLogs ulGain;
      ulGain.time = Simulator::Now ();
      ulGain.tx = tx;
      ulGain.rx = rx;
      ulGain.txAntennaGain = txAntennaGain;
      ulGain.rxAntennaGain = rxAntennaGain;
      ulGain.propagationGain = propagationGain;
      ulGain.pathloss = pathloss;

      g_ulgainLogs.push_back (ulGain);
    }
}

void
GetConnectionTrace (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_DEBUG ( Simulator::Now ().GetSeconds ()
                 << " eNB CellId " << cellId
                 << " successful connection of UE with IMSI " << imsi
                 << " RNTI " << rnti);

  ConnectionLogs connectionLogs;
  connectionLogs.imsi = imsi;
  connectionLogs.cellId = cellId;
  connectionLogs.rnti = rnti;
  g_connectionLogs.push_back (connectionLogs);
}


void ConnectPathlossTrace ()
{
  Config::Connect ("/ChannelList/0/PropagationLossModel/Hybrid3gppPathlossValue", MakeBoundCallback (&GetPathlossTrace,true));
  Config::Connect ("/ChannelList/1/PropagationLossModel/Hybrid3gppPathlossValue", MakeBoundCallback (&GetPathlossTrace,false));
}

void ConnectGainTrace ()
{
  Config::Connect ("/ChannelList/0/Gain", MakeBoundCallback (&GetGainTrace, true));
  Config::Connect ("/ChannelList/1/Gain", MakeBoundCallback (&GetGainTrace, false));
}

void ConnectConectionTrace ()
{
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished", MakeCallback (&GetConnectionTrace));
}

void
SavePathlossStats (std::string fileName, const std::vector<Pathloss> & pathloss)
{
  std::ofstream outFile;
  outFile.open (fileName.c_str (), std::ofstream::out | std::ofstream::trunc);
  outFile.setf (std::ios_base::fixed);

  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << fileName);
    }

  // How to read the logs:
  // At time t sec, the pathloss between node id = a Located in (Indoor/Outdoor) and node id = b Located in (Indoor/Outdoor)
  // is equal to Ploss(dB) and the distance between them is Dist(m)

  outFile << "#time(sec)\tNode id\tLoc\tNode id\tLoc\tPloss(dB)\tDist(m)" << std::endl;

  std::string aMob = "";
  std::string bMob = "";

  for (std::vector<Pathloss>::size_type i = 0; i != pathloss.size (); i++)
    {
      if (pathloss[i].aIndoor)
        {
          aMob = " Indoor ";
        }
      else
        {
          aMob = " Outdoor ";
        }
      if (pathloss[i].bIndoor)
        {
          bMob = " Indoor ";
        }
      else
        {
          bMob = " Outdoor ";
        }

      outFile << pathloss[i].time.GetSeconds () << "\t"
              << pathloss[i].n1->GetId () << "\t"
              << aMob << "\t"
              << pathloss[i].n2->GetId () << "\t"
              << bMob << "\t"
              << pathloss[i].ploss << "\t"
              << pathloss[i].dist << std::endl;
    }
}

void
SaveGainStats (std::string fileName, const std::vector<GainLogs> & gain)
{
  std::ofstream outFile;
  outFile.open (fileName.c_str (), std::ofstream::out | std::ofstream::trunc);
  outFile.setf (std::ios_base::fixed);

  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << fileName);
    }

  outFile << "#time(sec)\ttransmitter\treceiver\ttxAntennaGain(dB)\trxAntennaGain(dB)\tpropagationGain(dB)\tpathloss(dB)" << std::endl;

  for (std::vector<GainLogs>::size_type i = 0; i != gain.size (); i++)
    {
      outFile << gain[i].time.GetSeconds () << "\t"
              << gain[i].tx->GetObject<Node> ()->GetId () << "\t"
              << gain[i].rx->GetObject<Node> ()->GetId () << "\t"
              << gain[i].txAntennaGain << "\t"
              << gain[i].rxAntennaGain << "\t"
              << gain[i].propagationGain << "\t"
              << gain[i].pathloss << std::endl;
    }
}

void
SaveConnectionTrace (std::string fileName, const std::vector<ConnectionLogs> & connectionLogs)
{
  std::ofstream outFile;
  outFile.open (fileName.c_str (), std::ofstream::out | std::ofstream::trunc);
  outFile.setf (std::ios_base::fixed);

  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << fileName);
    }

  //In the logs, TxNID corresponds to a cell id of an eNB and RxNID corresponds to an IMSI of an UE
  outFile << "TxNID\tRxNID" << std::endl;

  for (std::vector<ConnectionLogs>::size_type i = 0; i != connectionLogs.size (); i++)
    {
      outFile << connectionLogs[i].cellId << "\t" << connectionLogs[i].imsi << std::endl;
    }
}

void
SaveNodesPositions (std::string fileName)
{
  std::ofstream outFile;
  outFile.open (fileName.c_str (), std::ofstream::out | std::ofstream::trunc);
  outFile.setf (std::ios_base::fixed);

  //In the logs, ID corresponds to cell id of an eNB and in case of UE to its IMSI
  outFile << "Node\tID\tX\tY" << std::endl;

  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <LteEnbNetDevice> ();
          if (enbdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "eNB" << "\t" << enbdev->GetCellId () << "\t"
                      << pos.x << "\t" << pos.y << "\t" << std::endl;
            }
        }
    }

  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
          if (uedev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "UE" << "\t" << uedev->GetImsi () << "\t"
                      << pos.x << "\t" << pos.y << "\t" << std::endl;
            }
        }
    }
}

void
SaveWrapAroundPositions (std::string filename, std::map<std::vector<double>,WrapAroundReplicas> mapOfWAround)
{
  NS_LOG_DEBUG ("Printing wrap-around position map of size " << mapOfWAround.size ());

  uint32_t siteCounter = 1;
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
    }
  outFile << "Node\tsiteCounter\tX\tY" << std::endl;
  for (auto const &i : mapOfWAround)
    {
      for (auto const &k : i.second.positions)
        {
          outFile << "enb\t" << siteCounter << "\t" << k.x << "\t" << k.y << std::endl;
          siteCounter++;
        }
    }
}

void
SaveWrapAroundAttachInfo (std::string filename, std::map<uint64_t,WrapAroundInfo_t> mapOfWAroundInfo)
{
  NS_LOG_DEBUG ("Printing wrap-around connection map of size " << mapOfWAroundInfo.size ());

  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
    }

  for (auto const &i : mapOfWAroundInfo)
    {
      outFile << i.first << " " << i.second.cellId << " " << i.second.posUe.x << " " << i.second.posUe.y << " " << i.second.posEnb.x << " " << i.second.posEnb.y << std::endl;
    }
}


int main (int argc, char *argv[])
{


  Config::SetDefault ("ns3::UrbanMacroCellPropagationLossModel::LosEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::Hybrid3gppPropagationLossModel::ShadowingEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::Hybrid3gppPropagationLossModel::CacheLoss", BooleanValue (false));

  CommandLine cmd;
  cmd.Parse (argc, argv);
  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();
  // parse again so we can override input file default values via command line
  cmd.Parse (argc, argv);

  UintegerValue uintegerValue;
  IntegerValue integerValue;
  DoubleValue doubleValue;
  BooleanValue booleanValue;

  GlobalValue::GetValueByName ("nRings", uintegerValue);
  uint32_t nRings = uintegerValue.Get ();
  GlobalValue::GetValueByName ("nUesPerSector", uintegerValue);
  uint32_t nUesPerSector = uintegerValue.Get ();
  GlobalValue::GetValueByName ("enbTxPowerDbm", doubleValue);
  double enbTxPowerDbm = doubleValue.Get ();
  GlobalValue::GetValueByName ("ueTxPowerDbm", doubleValue);
  double ueTxPowerDbm = doubleValue.Get ();
  GlobalValue::GetValueByName ("enbDlEarfcn", uintegerValue);
  uint32_t enbDlEarfcn = uintegerValue.Get ();
  GlobalValue::GetValueByName ("enbBandwidth", uintegerValue);
  uint16_t enbBandwidth = uintegerValue.Get ();
  GlobalValue::GetValueByName ("srsPeriodicity", uintegerValue);
  uint16_t srsPeriodicity = uintegerValue.Get ();
  GlobalValue::GetValueByName ("minDistance", doubleValue);
  double minDistance = doubleValue.Get ();
  GlobalValue::GetValueByName ("interSiteDistance", doubleValue);
  double interSiteDistance = doubleValue.Get ();
  GlobalValue::GetValueByName ("simTime", doubleValue);
  double simTime = doubleValue.Get ();
  GlobalValue::GetValueByName ("enableChannelLogs", booleanValue);
  bool enableChannelLogs = booleanValue.Get ();
  GlobalValue::GetValueByName ("enableTopologyLogs", booleanValue);
  bool enableTopologyLogs = booleanValue.Get ();
  GlobalValue::GetValueByName ("enableLteTraces", booleanValue);
  bool enableLteTraces = booleanValue.Get ();

  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (srsPeriodicity));

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // Set EARFCN, bandwidth and TX power
  lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (enbDlEarfcn));
  lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (enbDlEarfcn + 18000));
  lteHelper->SetUeDeviceAttribute ("DlEarfcn", UintegerValue (enbDlEarfcn));
  lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (enbBandwidth));
  lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (enbBandwidth));
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (enbTxPowerDbm));
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (ueTxPowerDbm));

  // Set Harq frequency reuse to assign different RBs to each eNb of 3 sector cell. FrCellTypeId is set by topology helper.
  lteHelper->SetAttribute ("FfrAlgorithm", StringValue ("ns3::LteFrHardAlgorithm"));

  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

  Ptr<Lte3gppHexGridEnbTopologyHelper> topoHelper = CreateObject<Lte3gppHexGridEnbTopologyHelper> ();
  topoHelper->AssignStreams (1);
  topoHelper->SetLteHelper (lteHelper);
  topoHelper->SetNumRings (nRings);
  topoHelper->SetInterSiteDistance (interSiteDistance);
  topoHelper->SetMinimumDistance (minDistance);

  NS_LOG_DEBUG ("Configured topology Helper");

  lteHelper->SetEnbAntennaModelType ("ns3::Parabolic3dAntennaModel");
  lteHelper->SetEnbAntennaModelAttribute ("MechanicalTilt", DoubleValue (20));
  lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth",   DoubleValue (70));

  NS_LOG_DEBUG ("Configured Antenna");

  NodeContainer threeSectorNodes;
  threeSectorNodes.Create (topoHelper->GetNumNodes ());

  NodeContainer ueNodes;
  ueNodes.Create (nUesPerSector * threeSectorNodes.GetN ());

  // Install mobility (UE)
  MobilityHelper mobilityUe;
  mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe.Install (ueNodes);

  // Install mobility (eNB)
  MobilityHelper mobilityEnodeB;
  mobilityEnodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityEnodeB.Install (threeSectorNodes);

  //Compute the position of each site and antenna orientation
  NetDeviceContainer enbDevs = topoHelper->SetPositionAndInstallEnbDevice (threeSectorNodes);
  NS_LOG_DEBUG ("Installed eNbs");

  NetDeviceContainer ueDevs;

  //Deploys the UEs uniformly (and equally among the cells)
  ueDevs = topoHelper->DropUEsUniformlyPerSector (ueNodes);

  NS_LOG_DEBUG ("Installed UEs");

  // Lte3gppHexGridEnbTopologyHelper wrap-around method to attach UEs to the eNBs

  //*** Important for LTE scenarios: use downlink pathoss model to calculate pathloss for attaching UEs to the eNBs ***

  Ptr<Object> downlinkPathlossModel = lteHelper->GetDownlinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = downlinkPathlossModel->GetObject<PropagationLossModel> ();

  NS_ABORT_MSG_IF (lossModel == nullptr, "No PathLossModel");
  topoHelper->AttachWithWrapAround (lossModel, ueDevs, enbDevs);
  NS_LOG_DEBUG ("Attached UE's to the eNB with wrap-around");

  std::map<std::vector<double>,WrapAroundReplicas> mapOfWAround = topoHelper->GetWrapAroundPositions ();
  std::map<uint64_t,WrapAroundInfo_t> wrapAroundAttachInfo = topoHelper->GetWrapAroundAttachInfo ();

  std::string frAlgorithmType = lteHelper->GetFfrAlgorithmType ();
  NS_LOG_DEBUG ("FrAlgorithmType: " << frAlgorithmType);

  // (RLC SM will generate flows in downlink and uplink)

  for (uint32_t u = 0; u < ueDevs.GetN (); ++u)
    {
      enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
      EpsBearer bearer (q);
      lteHelper->ActivateDataRadioBearer (ueDevs.Get (u), bearer);
    }

  Simulator::Stop (Seconds (simTime));

  if (enableLteTraces)
    {
      lteHelper->EnableMacTraces ();
      lteHelper->EnableRlcTraces ();
    }


// Channel traces, i.e., Hybrid3gppPathlossValue (only available for ns3::Hybrid3gppPropagationLossModel) and Gain traces
// from MultiModelSpectrumChannel.

  if (enableChannelLogs)
    {
      ConnectPathlossTrace ();
      ConnectGainTrace ();
    }

  if (enableTopologyLogs)
    {
      // connect custom trace sinks for RRC connection establishment notification
      ConnectConectionTrace ();
    }


  Simulator::Run ();


  if (enableChannelLogs)
    {
      SavePathlossStats ("example-wrap-around-dl-pathloss-traces.out", g_dlPathlossLogs);
      SavePathlossStats ("example-wrap-around-ul-pathloss-traces.out", g_ulPathlossLogs);
      SaveGainStats ("example-wrap-around-dl-gain-traces.out", g_dlgainLogs);
      SaveGainStats ("example-wrap-around-ul-gain-traces.out", g_ulgainLogs);
    }

  if (enableTopologyLogs)
    {
      SaveConnectionTrace ("example-wrap-around-connections.txt", g_connectionLogs);
      SaveNodesPositions ("example-wrap-around-node-positions.txt");
      SaveWrapAroundPositions ("example-wrap-around-enbs-positions.txt", mapOfWAround);
      SaveWrapAroundAttachInfo ("example-wrap-around-enbs-connections.txt", wrapAroundAttachInfo);
    }


  Simulator::Destroy ();
  return 0;
}
