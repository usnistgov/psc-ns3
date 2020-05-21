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
#include "ns3/spectrum-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include <ns3/buildings-module.h>

using namespace ns3;

/**
 * Sample simulation script to demonstrate the use of Lte3gppHexGridEnbTopologyHelper,
 * (i.e., hexagonal cells in ring topology) Parabolic3dAntennaModel and 3GPP compliant pathloss models
 * in buildings module. It only supports a downlink flow. By default, only the LTE radio access
 * will be simulated with RLC SM. Users interested to use EPC or to generate REM may
 * configure the following Global Values through command-line, e.g, command to generate REM of a single
 * hexagonal cell is :
 * ./waf --run "example-3gpp-hex-topology --generateRem=1 --nRings=1"
 * Alternatively these values can also be configured through ns-3 ConfigStore system.
 *
 */

NS_LOG_COMPONENT_DEFINE ("3gppHexTopologyExample");

static ns3::GlobalValue g_nRings ("nRings",
                                  "Number of Rings of hexagonal cells. First ring contains 1 cell(i.e. 3 eNB nodes), "
                                  "2nd has 6 cells, 3rd has 12 cells and so on.",
                                  ns3::UintegerValue (1),
                                  ns3::MakeUintegerChecker<uint32_t> ());
static ns3::GlobalValue g_nUesPerSector ("nUesPerSector",
                                         "Number of UEs per sector of a hexagonal cells.",
                                         ns3::UintegerValue (1),
                                         ns3::MakeUintegerChecker<uint32_t> ());
static ns3::GlobalValue g_epc ("epc",
                               "If true, will setup the EPC to simulate an end-to-end topology, "
                               "with real IP applications over PDCP and RLC UM (or RLC AM by changing "
                               "the default value of EpsBearerToRlcMapping e.g. to RLC_AM_ALWAYS). "
                               "If false, only the LTE radio access will be simulated with RLC SM. ",
                               ns3::BooleanValue (false),
                               ns3::MakeBooleanChecker ());
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
                                        ns3::UintegerValue (50),
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
                                   ns3::DoubleValue (5),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_generateRem ("generateRem",
                                       "if true, will generate a REM and then abort the simulation;"
                                       "if false, will run the simulation normally (without generating any REM)",
                                       ns3::BooleanValue (false),
                                       ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_useDataChannelForRem ("useDataChannelForRem",
                                                "if true, will generate a REM of Data channel;"
                                                "if false, will generate a REM of control channel",
                                                ns3::BooleanValue (true),
                                                ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_remRbId ("remRbId",
                                   "Resource Block Id of Data Channel, for which REM will be generated;"
                                   "default value is -1, which means REM will be averaged from all RBs",
                                   ns3::IntegerValue (-1),
                                   MakeIntegerChecker<int32_t> ());
static ns3::GlobalValue g_enableChannelLogs ("enableChannelLogs",
                                             "whether to generate channel logs when EPC is enabled without generating the REM.",
                                             ns3::BooleanValue (false),
                                             ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_enableTopologyLogs ("enableTopologyLogs",
                                              "whether to generate topology logs.",
                                              ns3::BooleanValue (false),
                                              ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_dropUEsIndoorOutdoor ("dropUEsIndoorOutdoor",
                                                "Drop UEs indoor and outdoor with 2 buildings per cell/sector as per TR 36.843 Table A.2.1.1-1.",
                                                ns3::BooleanValue (false),
                                                ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_enableLteTraces ("enableLteTraces",
                                           "if true, will generate LTE traces;"
                                           "if false, will run the simulation without generating LTE traces",
                                           ns3::BooleanValue (false),
                                           ns3::MakeBooleanChecker ());



struct Pathloss
{
  Time time;
  double ploss;
  Ptr<Node> n1;
  Ptr<Node> n2;
  double dist;
  bool aIndoor;
  bool bIndoor;
};

struct GainLogs
{
  Time time;
  Ptr<const MobilityModel> tx;
  Ptr<const MobilityModel> rx;
  double txAntennaGain;
  double rxAntennaGain;
  double propagationGain;
  double pathloss;
};

struct ConnectionLogs
{
  std::string context;
  uint64_t imsi;
  uint16_t cellId;
  uint16_t rnti;
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


void SchedulePathlossLogConnect ()
{
  Config::Connect ("/ChannelList/2/PropagationLossModel/Hybrid3gppPathlossValue", MakeBoundCallback (&GetPathlossTrace,true));
  Config::Connect ("/ChannelList/3/PropagationLossModel/Hybrid3gppPathlossValue", MakeBoundCallback (&GetPathlossTrace,false));
}

void ScheduleGainLogConnect ()
{
  Config::Connect ("/ChannelList/2/Gain", MakeBoundCallback (&GetGainTrace, true));
  Config::Connect ("/ChannelList/3/Gain", MakeBoundCallback (&GetGainTrace, false));
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
  // At time t sec, the pathloss between node id = a in Pos (Indoor/Outdoor) with IP = x.x.x.x and node id = b in Pos (Indoor/Outdoor) with
  // IP = x.x.x.x is equal to Ploss(dB) and the distance between them is Dist(m)

  outFile << "#time(sec)\tNode id\tPos\tIP\tNode id\tPos\tIP\tPloss(dB)\tDist(m)" << std::endl;

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
              << pathloss[i].n1->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal () << "\t"
              << pathloss[i].n2->GetId () << "\t"
              << bMob << "\t"
              << pathloss[i].n2->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal () << "\t"
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
SaveBuildingsPositions (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
      return;
    }
  Box box;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      box = (*it)->GetBoundaries ();
      outFile << box.xMin  << " " << box.xMax << " "
              << box.yMin  << " " << box.yMax
              << std::endl;
    }
}

void
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
      return;
    }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
      outFile << "set object " << index
              << " rect from " << box.xMin  << "," << box.yMin
              << " to "   << box.xMax  << "," << box.yMax
              << " front fs empty "
              << std::endl;
    }
}

void
PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
      return;
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
              outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at " << pos.x << "," << pos.y << " left font \"Helvetica,4\" textcolor rgb \"grey\" front point pt 1 ps 2 lc rgb \"grey\" offset 0,0"
                      << std::endl;
            }
        }
    }
}

void
PrintGnuplottableEnbListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_FATAL_ERROR ("Can't open file " << filename);
      return;
    }
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
              outFile << "set label \"" << enbdev->GetCellId ()
                      << "\" at " << pos.x << "," << pos.y
                      << " left font \"Helvetica,4\" textcolor rgb \"white\" front  point pt 2 ps 0.3 lc rgb \"white\" offset 0,0"
                      << std::endl;
            }
        }
    }
}




int main (int argc, char *argv[])
{

  // change some default attributes so that they are reasonable for
  // this scenario, but do this before processing command line
  // arguments, so that the user is allowed to override these settings
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (1)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (10 * 1024));
  Config::SetDefault ("ns3::LteHelper::UsePdschForCqiGeneration", BooleanValue (true));
  Config::SetDefault ("ns3::UrbanMacroCellPropagationLossModel::LosEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::Hybrid3gppPropagationLossModel::ShadowingEnabled", BooleanValue (false));

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
  GlobalValue::GetValueByName ("epc", booleanValue);
  bool epc = booleanValue.Get ();
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
  GlobalValue::GetValueByName ("generateRem", booleanValue);
  bool generateRem = booleanValue.Get ();
  GlobalValue::GetValueByName ("useDataChannelForRem", booleanValue);
  bool useDataChannelForRem = booleanValue.Get ();
  GlobalValue::GetValueByName ("remRbId", integerValue);
  int remRbId = integerValue.Get ();
  GlobalValue::GetValueByName ("enableChannelLogs", booleanValue);
  bool enableChannelLogs = booleanValue.Get ();
  GlobalValue::GetValueByName ("enableTopologyLogs", booleanValue);
  bool enableTopologyLogs = booleanValue.Get ();
  GlobalValue::GetValueByName ("dropUEsIndoorOutdoor", booleanValue);
  bool dropUEsIndoorOutdoor = booleanValue.Get ();
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


  Ptr<PointToPointEpcHelper>  epcHelper;

  if (epc)
    {
      epcHelper = CreateObject<PointToPointEpcHelper> ();
      lteHelper->SetEpcHelper (epcHelper);
    }

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

  if (!dropUEsIndoorOutdoor)
    {
      //Deploys the UEs uniformly (and equally among the cells)
      ueDevs = topoHelper->DropUEsUniformlyPerSector (ueNodes);
    }
  else
    {
      //Building dimensions: length 120m, width 50m and height 6m
      ueDevs = topoHelper->DropUEsIndoorOutdoor (ueNodes, 120, 50, 6);
      BuildingsHelper::Install (threeSectorNodes);
      BuildingsHelper::Install (ueNodes);
      NS_LOG_DEBUG ("Installed BuildingsHelper");
    }

  NS_LOG_DEBUG ("Installed UEs");

  if (epc)
    {
      // this enables handover for eNBs in case we change the mobility model
      lteHelper->AddX2Interface (threeSectorNodes);
      NS_LOG_DEBUG ("Added X2 interface");
    }

  Ipv4Address remoteHostAddr;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ipv4InterfaceContainer ueIpIface;
  Ptr<Node> remoteHost;

  if (epc)
    {
      // Create a single RemoteHost
      NodeContainer remoteHostContainer;
      remoteHostContainer.Create (1);
      remoteHost = remoteHostContainer.Get (0);
      InternetStackHelper internet;
      internet.Install (remoteHostContainer);

      // Create the Internet
      PointToPointHelper p2ph;
      p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
      p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
      p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
      Ptr<Node> pgw = epcHelper->GetPgwNode ();
      NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
      Ipv4AddressHelper ipv4h;
      ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
      Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
      // in this container, interface 0 is the pgw, 1 is the remoteHost
      //remoteHostAddr = internetIpIfaces.GetAddress (1);


      Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
      remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


      // Install the IP stack on the UEs
      internet.Install (ueNodes);
      ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

      // Attach each UE to the best available eNB
      lteHelper->Attach (ueDevs);
      NS_LOG_DEBUG ("Attached UE's to the best available eNB without wrap-around");

    }
  else // (epc == false)
    {
      // UEs attached to the closest  eNB **Since all the three eNBs of a cell is on the same location
      // the function AttachToClosestEnb attaches every UE to the first eNB (See lte MAC stats). The REM generated with
      // this will be different from what we get with epc enabled**.
      // lteHelper->AttachToClosestEnb (ueDevs, enbDevs);

      // By using this version of Attach we make sure that each eNB have UEs asper configured nUesPerSector
      uint32_t currentUe = 0;
      for (uint16_t i = 0; i < threeSectorNodes.GetN (); ++i)
        {
          for (uint32_t uePerSector = 0; uePerSector < nUesPerSector; ++uePerSector)
            {
              NS_LOG_DEBUG ("Attaching UE : " << currentUe << " to eNB : " << i);
              lteHelper->Attach (ueDevs.Get (currentUe), enbDevs.Get (i));
              // side effect: the default EPS bearer will be activated
              currentUe++;
            }
        }
    }


  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
  startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
  startTimeSeconds->SetAttribute ("Max", DoubleValue (0.0010));

  std::string frAlgorithmType = lteHelper->GetFfrAlgorithmType ();
  NS_LOG_DEBUG ("FrAlgorithmType: " << frAlgorithmType);

  Time logsStartTime;

  if (epc)

    {

      Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
      startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
      startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));
      Time appStartTime = Seconds (startTimeSeconds->GetValue ());
      logsStartTime = appStartTime;

      // Install and start applications on UEs and remote host
      uint16_t dlPort = 10000;
      ApplicationContainer clientApps;
      ApplicationContainer serverApps;

      for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
        {
          NS_LOG_DEBUG ("Installing Dl app for UE : " << u);
          Ptr<Node> ueNode = ueNodes.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

          ++dlPort;
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ueNode));

          UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          clientApps.Add (dlClient.Install (remoteHost));

          Ptr<EpcTft> tft = Create<EpcTft> ();
          EpcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPort;
          dlpf.localPortEnd = dlPort;
          tft->Add (dlpf);

          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          lteHelper->ActivateDedicatedEpsBearer (ueDevs.Get (u), bearer, tft);
          if (u == 0)
            {
              serverApps.Start (appStartTime);
              clientApps.Start (appStartTime);
            }
          else
            {
              Time startTime = Seconds (startTimeSeconds->GetValue ());
              serverApps.Start (startTime);
              clientApps.Start (startTime);
            }

        }

    }
  else // (epc == false)
    {
      for (uint32_t u = 0; u < ueDevs.GetN (); ++u)
        {
          enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
          EpsBearer bearer (q);
          lteHelper->ActivateDataRadioBearer (ueDevs.Get (u), bearer);
        }
    }

  Ptr<RadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      PrintGnuplottableBuildingListToFile ("buildings.txt");
      PrintGnuplottableEnbListToFile ("enbs.txt");
      PrintGnuplottableUeListToFile ("ues.txt");

      // Configure Radio Environment Map (REM) output
      // for LTE-only simulations always use /ChannelList/0 which is the downlink channel
      remHelper = CreateObject<RadioEnvironmentMapHelper> ();
      remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/0"));
      remHelper->SetAttribute ("OutputFile", StringValue ("rem.out"));
      remHelper->SetAttribute ("Earfcn", UintegerValue (enbDlEarfcn));
      remHelper->SetAttribute ("Bandwidth", UintegerValue (enbBandwidth));
      remHelper->SetAttribute ("XMin", DoubleValue (-interSiteDistance * nRings));
      remHelper->SetAttribute ("XMax", DoubleValue (interSiteDistance * nRings));
      remHelper->SetAttribute ("YMin", DoubleValue (-interSiteDistance * nRings));
      remHelper->SetAttribute ("YMax", DoubleValue (interSiteDistance * nRings));
      remHelper->SetAttribute ("Z", DoubleValue (1.5));

      if (useDataChannelForRem)
        {
          NS_LOG_DEBUG ("Generating Data channel REM");
          remHelper->SetAttribute ("UseDataChannel", BooleanValue (true));
          remHelper->SetAttribute ("RbId", IntegerValue (remRbId));
        }
      else
        {
          NS_LOG_DEBUG ("Generating control channel REM");
          remHelper->SetAttribute ("UseDataChannel", BooleanValue (false));
          remHelper->SetAttribute ("RbId", IntegerValue (-1));
        }

      remHelper->Install ();

    }
  else
    {
      Simulator::Stop (Seconds (simTime));
    }




  if (enableLteTraces)
    {
      lteHelper->EnableMacTraces ();
      lteHelper->EnableRlcTraces ();

      if (epc)
        {
          lteHelper->EnablePdcpTraces ();
        }
    }


// Channel traces, i.e., Hybrid3gppPathlossValue (only available for ns3::Hybrid3gppPropagationLossModel) and Gain traces
// from MultiModelSpectrumChannel.

  if (enableChannelLogs && epc && !generateRem)
    {
      Simulator::Schedule (logsStartTime, &SchedulePathlossLogConnect);
      Simulator::Schedule (logsStartTime, &ScheduleGainLogConnect);
    }

  if (enableTopologyLogs)
    {
      //connect custom trace sinks for RRC connection establishment notification
      ConnectConectionTrace ();
    }

  Simulator::Run ();


  if (enableChannelLogs && epc && !generateRem)
    {
      SavePathlossStats ("example-3gpp-hex-dl-pathloss-traces.txt", g_dlPathlossLogs);
      SavePathlossStats ("example-3gpp-hex-ul-pathloss-traces.txt", g_ulPathlossLogs);
      SaveGainStats ("example-3gpp-hex-dl-gain-traces.txt", g_dlgainLogs);
      SaveGainStats ("example-3gpp-hex-ul-gain-traces.txt", g_ulgainLogs);
    }

  if (enableTopologyLogs)
    {
      SaveConnectionTrace ("example-3gpp-hex-connections-trace.txt", g_connectionLogs);
      SaveNodesPositions ("example-3gpp-hex-node-positions.txt");
      SaveBuildingsPositions ("example-3gpp-hex-buildings-positions.txt");
    }


  Simulator::Destroy ();
  return 0;
}
