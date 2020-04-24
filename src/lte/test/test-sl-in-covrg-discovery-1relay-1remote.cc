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


#include "ns3/object.h"
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store.h"
#include <cfloat>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestSlInCovrgDiscovery1Relay1Remote");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg discovery 1 relay 1 remote test case.
 */
class SlInCovrgDiscovery1Relay1RemoteTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param simTime simTime
   * \param useModelB useModelB
   */
  SlInCovrgDiscovery1Relay1RemoteTestCase (double simTime, bool useModelB);
  virtual ~SlInCovrgDiscovery1Relay1RemoteTestCase ();
  /**
   * DiscoveryMonitoringCallback
   *
   * \param imsi imsi
   * \param cellId cellId
   * \param rnti rnti
   * \param discMsg discMsg
   */
  void DiscoveryMonitoringCallback (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg);

private:
  virtual void DoRun (void);
  double m_simTime; ///< simTime
  bool m_useModelB; ///< useModelB

  /**
   * imsiRelayservicecode
   */
  struct imsiRelayservicecode
  {
    uint64_t imsi;       ///< imsi
    uint32_t relayservicecode;       ///< relayservicecode
  };
  std::vector<imsiRelayservicecode> m_imsiRelayservicecodeVector; ///< imsiRelayservicecodeVector
};

SlInCovrgDiscovery1Relay1RemoteTestCase::SlInCovrgDiscovery1Relay1RemoteTestCase (double simTime, bool useModelB)
  : TestCase ("Scenario with 1 eNodeB and 2 UEs discovery using SideLink"),
  m_simTime (simTime),
  m_useModelB (useModelB)
{
}

SlInCovrgDiscovery1Relay1RemoteTestCase::~SlInCovrgDiscovery1Relay1RemoteTestCase ()
{
}

void
SlInCovrgDiscovery1Relay1RemoteTestCase::DiscoveryMonitoringCallback (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg)
{
  bool found = false;
  for (uint32_t i = 0; i < m_imsiRelayservicecodeVector.size (); i++)
    {
      if (m_imsiRelayservicecodeVector[i].imsi == imsi && m_imsiRelayservicecodeVector[i].relayservicecode == discMsg.GetRelayServiceCode ())
        {
          found = true;
          break;
        }
    }

  if (m_imsiRelayservicecodeVector.size () == 0 || !found)
    {
      imsiRelayservicecode iA;
      iA.imsi = imsi;
      iA.relayservicecode = discMsg.GetRelayServiceCode ();
      m_imsiRelayservicecodeVector.push_back (iA);
    }
}

void
SlInCovrgDiscovery1Relay1RemoteTestCase::DoRun (void)
{
  Time simTime = Seconds (m_simTime);

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

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

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  //Create nodes (eNb + UEs)
  NodeContainer enbNode;
  enbNode.Create (1);
  NS_LOG_INFO ("eNb node id = [" << enbNode.Get (0)->GetId () << "]");
  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (10.0, 0.0, 1.5));
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (-10.0, 0.0, 1.5));

  //Install mobility
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Configure Sidelink Pool
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetDiscEnabled (true);

  LteRrcSap::SlDiscTxResourcesSetup pool;
  pool.setup = LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlDiscResourcePoolFactory pfactory;
  pfactory.SetDiscCpLen ("NORMAL");
  pfactory.SetDiscPeriod ("rf32");
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);
  pfactory.SetDiscPrbNum (10);
  pfactory.SetDiscPrbStart (10);
  pfactory.SetDiscPrbEnd (40);
  pfactory.SetDiscOffset (0);
  pfactory.SetDiscBitmap (0x11111);
  pfactory.SetDiscTxProbability ("p100");

  pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();

  //Install Sidelink configuration for eNBs
  enbSidelinkConfiguration->AddDiscPool (pool);

  //-Relay UE (re)selection
  LteRrcSap::Sib19DiscConfigRelay discConfigRelay;
  discConfigRelay.haveRemoteUeConfig = true;
  discConfigRelay.remoteUeConfig.haveReselectionInfoIc = true;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.filterCoefficient = 0;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.minHyst = 0;
  discConfigRelay.remoteUeConfig.reselectionInfoIc.qRxLevMin = -200;
  enbSidelinkConfiguration->SetDiscConfigRelay (discConfigRelay);

  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  // Install Sidelink configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  ueSidelinkConfiguration->SetDiscEnabled (true);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign IP address
  InternetStackHelper internet;
  internet.Install (ueNodes);
  Ipv6InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv6Address (NetDeviceContainer (ueDevs));


  // set the default gateway for the UE
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  //Attach each UE to the best available eNB
  lteHelper->Attach (ueDevs);

  NS_LOG_INFO ("Configuring discovery applications");

  Ptr<LteUeNetDevice> ueNetDevice0 = DynamicCast<LteUeNetDevice> (ueDevs.Get (0));
  Ptr<LteUeNetDevice> ueNetDevice1 = DynamicCast<LteUeNetDevice> (ueDevs.Get (1));
  uint32_t relayServiceCode = 35;
  if (!m_useModelB)
    {
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueNetDevice0, relayServiceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueNetDevice1, relayServiceCode, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
    }
  else
    {
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueNetDevice0, relayServiceCode, LteSlUeRrc::ModelB, LteSlUeRrc::RelayUE);
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService, proseHelper, ueNetDevice1, relayServiceCode, LteSlUeRrc::ModelB, LteSlUeRrc::RemoteUE);
    }

  ///*** End of application configuration ***///

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/DiscoveryMonitoring", MakeCallback (&SlInCovrgDiscovery1Relay1RemoteTestCase::DiscoveryMonitoringCallback, this));

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();

  bool allDiscovered = true;
  if (!m_useModelB)
    {
      if (m_imsiRelayservicecodeVector.size () != (ueNodes.GetN () - 1))
        {
          allDiscovered = false;
        }
    }
  else
    {
      if (m_imsiRelayservicecodeVector.size () != ueNodes.GetN ())
        {
          allDiscovered = false;
        }
    }

  NS_TEST_ASSERT_MSG_EQ (allDiscovered, true, "Error in Remote UE being discovered by Relay UE. Test relay service discovery failed");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg discovery 1 relay 1 remote test suite.
 */
class SlInCovrgDiscovery1Relay1RemoteTestSuite : public TestSuite
{
public:
  SlInCovrgDiscovery1Relay1RemoteTestSuite ();
};

SlInCovrgDiscovery1Relay1RemoteTestSuite::SlInCovrgDiscovery1Relay1RemoteTestSuite ()
  : TestSuite ("sl-in-covrg-discovery-1relay-1remote", SYSTEM)
{
  //Test relay discovery model A and model B. one UE is relay and one is remote.

  //Normal test case with default values
  //Pass the variables simTime, useModelB
  AddTestCase (new SlInCovrgDiscovery1Relay1RemoteTestCase (6.0, false), TestCase::QUICK);
}

static SlInCovrgDiscovery1Relay1RemoteTestSuite staticSlInCovrgDiscovery1Relay1RemoteTestSuite;
