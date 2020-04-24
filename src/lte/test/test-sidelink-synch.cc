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


#include <sstream>
#include "ns3/object.h"
#include "ns3/test.h"
#include "ns3/lte-helper.h"
#include "ns3/lte-sidelink-helper.h"
#include "ns3/lte-sl-preconfig-pool-factory.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-epc-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/application-container.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/log.h"
#include "ns3/udp-echo-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestSidelinkSynch");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink synchronization test case.
 */
class SidelinkSynchTestCase : public TestCase
{
public:
  /**
   * \brief Constructor
   *
   * \param topologyIndex Index of the topology
   * \param onoff True if traffic in onoff
   * \param bidirectionalTft True if TFT if bidirectional
   * \param stream Stream to write output
   * \param description additional description of the test case
   */
  SidelinkSynchTestCase (uint32_t topologyIndex, bool onoff,
                         bool bidirectionalTft, uint64_t stream, std::string description);
  virtual ~SidelinkSynchTestCase ();

private:
  virtual void DoRun (void);
  /**
   * \brief Build name string function
   *
   * \param topologyIndex Index of the topology
   * \param onoff True if traffic in onoff
   * \param bidirectionalTft True if TFT if bidirectional
   * \param stream Stream to write output
   * \param description additional description of the test case
   * \returns the name string
   */
  static std::string BuildNameString (uint32_t topologyIndex, bool onoff,
                                      bool bidirectionalTft, uint64_t stream, std::string description = "");
  /**
   * \brief Notify change of SyncRef function
   *
   * \param param LteUeRrc::SlChangeOfSyncRefStatParameters
   */
  void NotifyChangeOfSyncRef (LteUeRrc::SlChangeOfSyncRefStatParameters param);
  /**
   * \brief Check system frame number alignment
   *
   * This method is triggered 0.3 ms after the NotifyChangeOfSyncRef.
   * In the simulation, if the SynchRef UE is instantiated after
   * the UE, i.e., SynchRef UE node id > the node id of the UE,
   * which is trying to synchronize with it; then the subframe
   * indication of the SyncRef UE will happen after this UE.
   *
   * \param param LteUeRrc::SlChangeOfSyncRefStatParameters
   */
  void CheckSfnAlignment (LteUeRrc::SlChangeOfSyncRefStatParameters param);
  uint32_t m_topologyIndex; ///< Index of the topology to used for the scenario
  NetDeviceContainer m_txUeDevs; ///< UE NetDevice Container
  bool m_onoff; ///< Flag to choose a traffic type, i.e., True for OnOff, False for UdpEchoClient
  bool m_bidirectionalTft; ///< Flag to choose the direction of the traffic while building TFT
  uint64_t m_stream; ///< Stream number
};

SidelinkSynchTestCase::SidelinkSynchTestCase (
  uint32_t topologyIndex, bool onoff, bool bidirectionalTft, uint64_t stream, std::string description)
  : TestCase (BuildNameString (topologyIndex, onoff, bidirectionalTft, stream, description)),
    m_topologyIndex (topologyIndex),
    m_onoff (onoff),
    m_bidirectionalTft (bidirectionalTft),
    m_stream (stream)
{
  NS_LOG_FUNCTION (this);
}

SidelinkSynchTestCase::~SidelinkSynchTestCase ()
{
}

void
SidelinkSynchTestCase::NotifyChangeOfSyncRef (LteUeRrc::SlChangeOfSyncRefStatParameters param)
{
  Simulator::Schedule (Seconds (0.0003), &SidelinkSynchTestCase::CheckSfnAlignment, this, param);
}

void
SidelinkSynchTestCase::CheckSfnAlignment (LteUeRrc::SlChangeOfSyncRefStatParameters param)
{
  bool synchRefFound = false;
  NS_LOG_DEBUG ("Time : " << Simulator::Now () << "\n"
                          << "IMSI : " << param.imsi << "\n"
                          << "Previous SLSSID : " << param.prevSlssid << "\n"
                          << "Previous Rx offset : " << param.prevRxOffset << "\n"
                          << "Previous frame : " << param.prevFrameNo << "\n"
                          << "Previous subframe : " << param.prevSubframeNo << "\n"
                          << "Current SLSSID : " << param.currSlssid << "\n"
                          << "Current Rx offset : " << param.currRxOffset << "\n"
                          << "New frame : " << param.currFrameNo << "\n"
                          << "New subframe : " << param.currSubframeNo);

  for (uint16_t i = 0; i < m_txUeDevs.GetN (); i++)
    {
      uint64_t synchRefImsi = m_txUeDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
      //Check sidelink design doc, current model assigns SLSS-ID = 10 * IMSI
      if (param.currSlssid / 10 == synchRefImsi)
        {
          synchRefFound = true;
          Ptr<LteUeRrc> ueRrc =  m_txUeDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ();
          NS_LOG_INFO ("SynchRef IMSI " << synchRefImsi
                                        << " Frame number : " << ueRrc->GetFrameNumber ()
                                        << " Subframe number : " << ueRrc->GetSubFrameNumber ());

          NS_TEST_ASSERT_MSG_EQ (param.currFrameNo, ueRrc->GetFrameNumber (),
                                 "\nIMSI " << param.imsi << " Frame number mismatched with the selected SynchRef");

          NS_TEST_ASSERT_MSG_EQ (param.currSubframeNo, ueRrc->GetSubFrameNumber (),
                                 "\nIMSI " << param.imsi << " subframe number mismatched with the selected SynchRef");
        }
    }

  NS_TEST_ASSERT_MSG_EQ (synchRefFound, true, "unable to find the NetDevice of synchRef with SLSSID"
                         << param.prevSlssid << " in NetDevice container");
}


void
SidelinkSynchTestCase::DoRun (void)
{
/*
  LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
  LogComponentEnable ("LteUePhy", logLevel);
  LogComponentEnable ("LteUeRrc", logLevel);
*/
  NS_LOG_DEBUG ("m_txUeDevs size " << m_txUeDevs.GetN ());

  /* Synchronization*/
  //Configure synchronization protocol
  Config::SetDefault ("ns3::LteUePhy::UeRandomInitialSubframeIndication", BooleanValue (true));

  Config::SetDefault ("ns3::LteUeRrc::UeSlssTransmissionEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteUePhy::UeSlssInterScanningPeriodMax", TimeValue (MilliSeconds (5000)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssInterScanningPeriodMin", TimeValue (MilliSeconds (5000)));
  uint32_t scaningPeriod = 40; // ms
  uint32_t measPeriod = 400; // ms
  Config::SetDefault ("ns3::LteUePhy::UeSlssScanningPeriod", TimeValue (MilliSeconds (scaningPeriod)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssMeasurementPeriod", TimeValue (MilliSeconds (measPeriod)));
  /* END Synchronization*/

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (10)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (true)); //use default Trp index of 0

  //Set the frequency
  uint32_t ulEarfcn = 18100;
  uint16_t ulBandwidth = 50;

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

  //Sidelink bearers activation time
  Time slBearersActivationTime = Seconds (2.0);

  //Create the helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //Create and set the EPC helper
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ////Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));
  // channel model initialization
  lteHelper->Initialize ();

  // Since we are not installing eNB, we need to set the frequency attribute of pathloss model here
  double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (ulEarfcn);
  NS_LOG_LOGIC ("UL freq: " << ulFreq);
  Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ABORT_MSG_IF (lossModel == nullptr, "No PathLossModel");
  bool ulFreqOk = uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

  NS_LOG_INFO ("Deploying UE's...");

  //Create nodes (UEs)
  NodeContainer ueNodes;
  MobilityHelper ueMobility;
  Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator> ();
  std::vector<uint32_t> txNodeId;
  std::vector<uint32_t> rxNodeId;


  switch (m_topologyIndex)
    {
    case 1:
      ueNodes.Create (2);
      positionAllocUe->Add (Vector (0.0, 0.0, 1.5));
      positionAllocUe->Add (Vector (20.0, 0.0, 1.5));
      ueMobility.SetPositionAllocator (positionAllocUe);
      ueMobility.Install (ueNodes);
      txNodeId.push_back (0); // 0 is the index of the Node 1 object in ueNodes
      rxNodeId.push_back (1);
      break;
    case 2:
      ueNodes.Create (4);
      positionAllocUe->Add (Vector (0.0, 0.0, 1.5));
      positionAllocUe->Add (Vector (20.0, 0.0, 1.5));
      positionAllocUe->Add (Vector (40.0, 0.0, 1.5));
      positionAllocUe->Add (Vector (60.0, 0.0, 1.5));
      ueMobility.SetPositionAllocator (positionAllocUe);
      ueMobility.Install (ueNodes);
      txNodeId.push_back (0);
      txNodeId.push_back (3);
      rxNodeId.push_back (1);
      rxNodeId.push_back (2);
      break;
    default:
      NS_FATAL_ERROR ("Unknown topology index " << m_topologyIndex);
      break;
    }

  for (uint32_t i = 0; i < ueNodes.GetN (); i++)
    {
      double xCoordinate = ueNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().x;
      double yCoordinate = ueNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().y;
      NS_LOG_INFO ("UE with node id = [" << ueNodes.Get (i)->GetId () << "]" << " at " << xCoordinate << "," << yCoordinate);
    }

  //Install LTE UE devices to the nodes
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Fix the random number stream
  uint64_t randomStream = m_stream;
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  //Sidelink pre-configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = ulEarfcn;
  preconfiguration.preconfigGeneral.slBandwidth = ulBandwidth;
  preconfiguration.preconfigComm.nbPools = 1;

  LteSlPreconfigPoolFactory pfactory;

  //Control
  pfactory.SetControlPeriod ("sf40");
  pfactory.SetControlBitmap (0x00000000FF); //8 subframes for PSCCH
  pfactory.SetControlOffset (0);
  pfactory.SetControlPrbNum (22);
  pfactory.SetControlPrbStart (0);
  pfactory.SetControlPrbEnd (49);

  //Data
  pfactory.SetDataBitmap (0xFFFFFFFFFF);
  pfactory.SetDataOffset (8); //After 8 subframes of PSCCH
  pfactory.SetDataPrbNum (25);
  pfactory.SetDataPrbStart (0);
  pfactory.SetDataPrbEnd (49);

  preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();

  /* Synchronization*/
  //Synchronization parameters
  preconfiguration.preconfigSync.syncOffsetIndicator1 = 18;
  preconfiguration.preconfigSync.syncOffsetIndicator2 = 29;
  preconfiguration.preconfigSync.syncTxThreshOoC = -60; //dBm;
  preconfiguration.preconfigSync.syncRefDiffHyst = 0; //dB;
  preconfiguration.preconfigSync.syncRefMinHyst = 0; //dB;
  preconfiguration.preconfigSync.filterCoefficient = 0;  //k = 4 ==> a = 0.5, k = 0 ==> a = 1 No filter;
  /* END Synchronization*/

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  /*Synchronization*/
  //Set initial SLSSID and start of the first scanning for all UEs
  uint32_t baseTime = 2000; //ms
  uint32_t firstScanningTime = 0;
  for (uint32_t i = 0; i < ueDevs.GetN (); i++)
    {
      uint64_t tempSlssid = i + 10;
      ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ()->SetSlssid (tempSlssid);
      firstScanningTime = baseTime + (i * baseTime);
      ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetPhy ()->SetFirstScanningTime (MilliSeconds (firstScanningTime));
      uint64_t imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ()->GetImsi ();
      NS_LOG_INFO ("IMSI " << imsi << " SLSSID " << tempSlssid
                           << " First Scan "  << firstScanningTime << " ms");
    }

  InternetStackHelper internet;
  internet.Install (ueNodes);
  uint32_t groupL2Address = 200;
  Ipv4Address groupAddress4 ("225.0.0.0");     //use multicast address as destination
  Address remoteAddress;
  Address localAddress;
  Ptr<LteSlTft> tft;

  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

  // set the default gateway for the UE
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  remoteAddress = InetSocketAddress (groupAddress4, 8000);
  localAddress = InetSocketAddress (Ipv4Address::GetAny (), 8000);

  //Set Application in the UEs
  ApplicationContainer clientApps;
  // uint32_t ueCounter = 0;

  for (uint32_t i = 0; i < txNodeId.size (); ++i)
    {
      NS_LOG_INFO ("Installing client app on UE node id : " << ueNodes.Get (txNodeId [i])->GetId ());
      m_txUeDevs.Add (ueDevs.Get (txNodeId [i]));

      if (m_onoff)
        {
          // OnOff application
          OnOffHelper sidelinkOnOffClient ("ns3::UdpSocketFactory", remoteAddress);
          sidelinkOnOffClient.SetConstantRate (DataRate ("16kb/s"), 200);
          clientApps.Add (sidelinkOnOffClient.Install (ueNodes.Get (txNodeId [i])));
        }
    }

  Time clientAppStartTime = slBearersActivationTime + Seconds (0.9);
  clientApps.Start (clientAppStartTime);

  ApplicationContainer serverApps;
  NetDeviceContainer rxUeDevs;
  PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", localAddress);
  for (uint32_t i = 0; i < rxNodeId.size (); ++i)
    {
      NS_LOG_INFO ("Installing server app on UE node " << ueNodes.Get (rxNodeId [i])->GetId ());
      serverApps.Add (sidelinkSink.Install (ueNodes.Get (rxNodeId [i])));
      rxUeDevs.Add (ueDevs.Get (rxNodeId [i]));
    }

  serverApps.Start (slBearersActivationTime);

  //Set Sidelink bearers
  if (m_bidirectionalTft)
    {
      tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress4, groupL2Address);
      proseHelper->ActivateSidelinkBearer (slBearersActivationTime, ueDevs, tft);
    }
  else
    {
      tft = Create<LteSlTft> (LteSlTft::TRANSMIT, groupAddress4, groupL2Address);
      proseHelper->ActivateSidelinkBearer (slBearersActivationTime, m_txUeDevs, tft);
      tft = Create<LteSlTft> (LteSlTft::RECEIVE, groupAddress4, groupL2Address);
      proseHelper->ActivateSidelinkBearer (slBearersActivationTime, rxUeDevs, tft);
    }

  //Tracing the change of synchronization reference
  for (uint32_t i = 0; i < rxUeDevs.GetN (); ++i)
    {
      Ptr<LteUeRrc> ueRrc =  rxUeDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ();
      ueRrc->TraceConnectWithoutContext ("ChangeOfSyncRef", MakeCallback (&SidelinkSynchTestCase::NotifyChangeOfSyncRef, this));
    }
  ///*** End of application configuration ***///

  /*
   NS_LOG_INFO ("Enabling Sidelink traces...");
   lteHelper->EnableSlPscchMacTraces ();
   lteHelper->EnableSlPsschMacTraces ();

   lteHelper->EnableSlRxPhyTraces ();
   lteHelper->EnableSlPscchRxPhyTraces ();
 */
  NS_LOG_INFO ("Starting simulation...");

  Time simTime = clientAppStartTime + MilliSeconds (firstScanningTime) + MilliSeconds (scaningPeriod + measPeriod);

  NS_LOG_INFO ("------------------Simulation time = " << simTime.GetSeconds () << " -----------------------");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();
}

std::string
SidelinkSynchTestCase::BuildNameString (uint32_t topologyIndex, bool onoff,
                                        bool bidirectionalTft, uint64_t stream, std::string description)
{
  std::ostringstream oss;
  oss << description << " Topology index = " << topologyIndex;
  if (onoff)
    {
      oss << ", Traffic type = OnOff";
    }
  else
    {
      oss << ", Traffic type = UdpEchoClient";
    }

  if (bidirectionalTft)
    {
      oss << ", Tft traffic direction = BIDIRECTIONAL";
    }
  else
    {
      oss << ", Tft traffic direction = Transmit and Receive";
    }

  oss << ", stream number = " << stream;

  return oss.str ();
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink synchronization test suite.
 */
class SidelinkSynchTestSuite : public TestSuite
{
public:
  SidelinkSynchTestSuite ();
};

SidelinkSynchTestSuite::SidelinkSynchTestSuite ()
  : TestSuite ("sidelink-synch", SYSTEM)
{
  // LogComponentEnable ("TestSidelinkSynch", LOG_LEVEL_ALL);

  //Test 1
  //topologyIndex= 1, onoff = true, bidirectionalTft = false, stream number
  for (uint64_t i = 1; i < 11; ++i)
    {
      AddTestCase (new SidelinkSynchTestCase (1, true, false, i, "Test parameters : "), TestCase::QUICK);
    }

  //Test 2
  //topologyIndex= 1, onoff = true, bidirectionalTft = true, stream number
  for (uint64_t i = 11; i < 21; ++i)
    {
      AddTestCase (new SidelinkSynchTestCase (1, true, true, i, "Test parameters : "), TestCase::QUICK);
    }

  //Test 3
  //topologyIndex= 2, onoff = true, bidirectionalTft = false, stream number
  for (uint64_t i = 21; i < 31; ++i)
    {
      AddTestCase (new SidelinkSynchTestCase (2, true, false, i, "Test parameters : "), TestCase::QUICK);
    }

  //Test 4
  //topologyIndex= 2, onoff = true, bidirectionalTft = true, stream number
  for (uint64_t i = 31; i < 41; ++i)
    {
      AddTestCase (new SidelinkSynchTestCase (2, true, true, i, "Test parameters : "), TestCase::QUICK);
    }

}

static SidelinkSynchTestSuite g_sidelinkSynchTestSuite;
