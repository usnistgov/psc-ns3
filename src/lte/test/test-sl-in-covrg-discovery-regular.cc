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

NS_LOG_COMPONENT_DEFINE ("TestSlInCovrgDiscoveryRegular");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg discovery regular test case.
 */
class SlInCovrgDiscoveryRegularTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param simTime simTime
   * \param useModelB useModelB
   * \param pfactory pfactory
   */
  SlInCovrgDiscoveryRegularTestCase (double simTime, bool useModelB, LteSlDiscResourcePoolFactory pfactory);
  virtual ~SlInCovrgDiscoveryRegularTestCase ();
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
  LteSlDiscResourcePoolFactory m_pfactory; ///< pfactory
  
  /**
   * imsiAppcode
   */
  struct imsiAppcode
  {
	  uint64_t imsi; ///< imsi
	  uint32_t appcode; ///< appcode
  };
  std::vector<imsiAppcode> m_imsiAppcodeVector; ///< imsiAppcodeVector
};

SlInCovrgDiscoveryRegularTestCase::SlInCovrgDiscoveryRegularTestCase (double simTime, bool useModelB, LteSlDiscResourcePoolFactory pfactory)
  : TestCase ("Scenario with 1 eNodeB and 2 UEs discovery using SideLink"),
	m_simTime (simTime),
	m_useModelB (useModelB)
{
	m_pfactory = pfactory;
}

SlInCovrgDiscoveryRegularTestCase::~SlInCovrgDiscoveryRegularTestCase ()
{
}

void 
SlInCovrgDiscoveryRegularTestCase::DiscoveryMonitoringCallback (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discMsg)
{
	bool found = false;
	for (uint32_t i = 0; i < m_imsiAppcodeVector.size(); i++)
	{
		if (m_imsiAppcodeVector[i].imsi == imsi && m_imsiAppcodeVector[i].appcode == discMsg.GetApplicationCode())
		{
			found = true;
			break;
		}
	}
	
	if (m_imsiAppcodeVector.size() == 0 || !found)
	{
		imsiAppcode iA;
		iA.imsi = imsi;
		iA.appcode = discMsg.GetApplicationCode();
		m_imsiAppcodeVector.push_back (iA);
	}
}

void
SlInCovrgDiscoveryRegularTestCase::DoRun (void)
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
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

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

  
  pool.ueSelected.poolToAddModList.pools[0].pool =  m_pfactory.CreatePool ();

  //Install Sidelink configuration for eNBs
  enbSidelinkConfiguration->AddDiscPool (pool);
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

  //Attach each UE to the best available eNB
  lteHelper->Attach (ueDevs);

  NS_LOG_INFO ("Configuring discovery applications");
  
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announceApps;
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorApps;

  for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
    {
      announceApps[ueDevs.Get (i - 1)].push_back (i);
	  NS_LOG_DEBUG ("Ue Device " << i - 1 << " announce app = " << i);
      for (uint32_t j = 1; j <= ueNodes.GetN (); ++j)
        {
          if (i != j)
            {
              monitorApps[ueDevs.Get (i - 1)].push_back (j);
			  NS_LOG_DEBUG ("Ue Device " << i - 1 << " monitor app = " << j);
            }
        }
    }

  for (auto itAnnounceApps : announceApps)
    {
      Ptr<LteUeNetDevice> ueNetDevice = DynamicCast<LteUeNetDevice> (itAnnounceApps.first);
      std::list<uint32_t> apps = itAnnounceApps.second;
      std::cout << "Scheduling " << apps.size () << " announce apps for UE with IMSI = " << ueNetDevice->GetImsi () << std::endl;
      std::list<uint32_t>::iterator itAppList;
      for (auto itAppList : apps)
        {
          std::cout << "Announcing App code = " << itAppList << std::endl;
        }

      if (!m_useModelB)
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Announcing);
        }
      else
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Discoveree);
        }
    }

  for (auto itMonitorApps : monitorApps)
    {
      Ptr<LteUeNetDevice> ueNetDevice = DynamicCast<LteUeNetDevice> (itMonitorApps.first);
      std::list<uint32_t> apps = itMonitorApps.second;
      std::cout << "Scheduling " << apps.size () << " monitor apps for UE with IMSI = " << ueNetDevice->GetImsi () << std::endl;
      std::list<uint32_t>::iterator itAppList;
      for (auto itAppList : apps)
        {
          std::cout << "Monitoring App code = " << itAppList << std::endl;
        }

      if (!m_useModelB)
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Monitoring);
        }
      else
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Discoverer);
        }
    }

  ///*** End of application configuration ***///

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/DiscoveryMonitoring", MakeCallback (&SlInCovrgDiscoveryRegularTestCase::DiscoveryMonitoringCallback, this));

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();
  
  std::cout<<std::endl;
  
  std::map<uint64_t,uint32_t> imsiAppcodeCountsMap;
  for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
    {
		imsiAppcodeCountsMap.insert ( std::pair<uint64_t,uint32_t>(i,0) );
	}
  
  for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
    {
		uint32_t count = 0;
		for (uint32_t j = 0; j < m_imsiAppcodeVector.size(); j++)
		  {
			  if (i == m_imsiAppcodeVector[j].imsi)
			  {
				  count++;
				  imsiAppcodeCountsMap[ m_imsiAppcodeVector[j].imsi ] = count;
			  }
		  }
	}
	
	bool allDiscovered = true;
	for(std::map<uint64_t,uint32_t>::const_iterator it = imsiAppcodeCountsMap.begin(); it != imsiAppcodeCountsMap.end(); ++it)
	  {
		  if (!m_useModelB)
		  {
			  if (it->second != (ueNodes.GetN () - 1))
			  {
				  allDiscovered = false;
			  }
		  }
		  else
		  {
			  if (it->second != ueNodes.GetN ())
			  {
				  allDiscovered = false;
			  }
		  }
	  }
	  
	  NS_TEST_ASSERT_MSG_EQ (allDiscovered, true, "Error in all UEs being discovered by other UEs. Test application discovery failed");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink in-covrg discovery regular test suite.
 */
class SlInCovrgDiscoveryRegularTestSuite : public TestSuite
{
public:
  SlInCovrgDiscoveryRegularTestSuite ();
};

SlInCovrgDiscoveryRegularTestSuite::SlInCovrgDiscoveryRegularTestSuite ()
  : TestSuite ("sl-in-covrg-discovery-regular", SYSTEM)
{
  // LogComponentEnable ("TestSlInCovrgDiscoveryRegular", LOG_LEVEL_ALL);
  
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
  
  //Test application discovery model A and model B. Check that UEs discover each other. Test that applications are discovered by each UE.
  
  //Normal test case with default values
  //Pass the variables simTime, useModelB, pfactory
  AddTestCase (new SlInCovrgDiscoveryRegularTestCase (6.0, false, pfactory), TestCase::QUICK);
  
  //Set number of retransmissions to 1
  //Pass the variables simTime, useModelB, pfactory
  pfactory.SetNumRetx (1);
  AddTestCase (new SlInCovrgDiscoveryRegularTestCase (6.0, false, pfactory), TestCase::QUICK);
  
  //Reset number of retransmissions to 0, Set number of repetitions to 2
  //Pass the variables simTime, useModelB, pfactory
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (2);
  AddTestCase (new SlInCovrgDiscoveryRegularTestCase (6.0, false, pfactory), TestCase::QUICK);
  
  //Reset number of retransmissions to 0, Reset number of repetitions to 1, Set transmit probability to 75 percent
  //Pass the variables simTime, useModelB, pfactory
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);
  pfactory.SetDiscTxProbability ("p75");
  AddTestCase (new SlInCovrgDiscoveryRegularTestCase (6.0, false, pfactory), TestCase::QUICK);
}

static SlInCovrgDiscoveryRegularTestSuite staticSlInCovrgDiscoveryRegularTestSuite;
