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

#include <ns3/test.h>

#include <ns3/simulator.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>

#include <ns3/mobility-helper.h>
#include <ns3/lte-helper.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/lte-3gpp-hex-grid-enb-topology-helper.h>


using namespace ns3;

/**
 * This test is to verify that the calculated pathloss and the distance between an UE and a wrap-around
 * eNB to which it is connected to is less than the pathloss from eNB's original location in the
 * central cluster. In this test, only the LTE radio access will be simulated with RLC SM in a 2 ring topology
 * scenario with 2 UEs per sector.
 */

NS_LOG_COMPONENT_DEFINE ("WrapAroundTopologyTest");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Wrap around topology test case.
 */
class WrapAroundTopologyTestCase : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param nRings Number of Rings of hexagonal cells
   * \param nUesPerSector Number of UEs per sector of a hexagonal cells
   * \param simulationDuration Duration of the simulation
   */
  WrapAroundTopologyTestCase (uint32_t nRings,uint32_t nUesPerSector, Time simulationDuration)
    : TestCase ("Verifying that the calculated pathloss and the distance from a wrap-around eNB is less than the pathloss from eNB's original location in central cluster"),
      m_nRings (nRings),
      m_nUesPerSector (nUesPerSector),
      m_simulationDuration (simulationDuration)
  {
  }

private:
  virtual void DoRun (void);
  /**
   * \brief Evaluate function
   *
   * \param enbDevs eNB net device container
   * \param ueDevs UE net device container
   * \param wrapAroundAttachInfo Wrap-around attachment information
   * \param lossModel The pathloss model
   */
  void Evaluate (NetDeviceContainer enbDevs, NetDeviceContainer ueDevs,
                 std::map<uint64_t,WrapAroundInfo_t> wrapAroundAttachInfo, Ptr<PropagationLossModel> lossModel);

  uint32_t m_nRings; ///< Number of Rings of hexagonal cells
  uint32_t m_nUesPerSector; ///< Number of UEs per sector of a hexagonal cells
  Time m_simulationDuration; ///< Duration of the simulation
};

void
WrapAroundTopologyTestCase::Evaluate (NetDeviceContainer enbDevs, NetDeviceContainer ueDevs,
                                      std::map<uint64_t,WrapAroundInfo_t> wrapAroundAttachInfo, Ptr<PropagationLossModel> lossModel)
{
  Ptr<LteUeNetDevice> lteUeDev;
  Ptr<LteEnbNetDevice> lteEnbDev;

  for (auto &wIt : wrapAroundAttachInfo)
    {
      uint64_t imsiToCheck = wIt.first;
      uint64_t cellIdToCheck = wIt.second.cellId;

      for (uint32_t ueDev = 0; ueDev < ueDevs.GetN (); ++ueDev)
        {
          uint64_t imsiUe = DynamicCast<LteUeNetDevice> (ueDevs.Get (ueDev))->GetImsi ();

          if (imsiUe == imsiToCheck)
            {
              lteUeDev = DynamicCast<LteUeNetDevice> (ueDevs.Get (ueDev));
              break;
            }
        }

      for (uint32_t enbDev = 0; enbDev < enbDevs.GetN (); ++enbDev)
        {
          uint64_t cellId = DynamicCast<LteEnbNetDevice> (enbDevs.Get (enbDev))->GetCellId ();

          if (cellId == cellIdToCheck)
            {
              lteEnbDev = DynamicCast<LteEnbNetDevice> (enbDevs.Get (enbDev));
              break;
            }
        }

      Ptr<MobilityModel> mmUe = lteUeDev->GetNode ()->GetObject<MobilityModel> ();
      Ptr<MobilityModel> mmEnb = lteEnbDev->GetNode ()->GetObject<MobilityModel> ();

      double pLossWithoutWrapAround = lossModel->CalcRxPower (0, mmEnb, mmUe) * -1;
      NS_LOG_DEBUG ("imsiToCheck : " << imsiToCheck << " cellIdToCheck : " << cellIdToCheck << " pLoss without wrap-around = " << pLossWithoutWrapAround);

      Ptr<MobilityModel> mmEnbWAround = CreateObject<ConstantPositionMobilityModel> ();
      mmEnbWAround->SetPosition (wIt.second.posEnb);

      double pLossWithWrapAround = lossModel->CalcRxPower (0, mmEnbWAround, mmUe) * -1;
      NS_LOG_DEBUG ("imsiToCheck : " << imsiToCheck << " cellIdToCheck : " << cellIdToCheck << " pLoss with wrap-around = " << pLossWithWrapAround);

      double distanceFromOrignalEnb = mmUe->GetDistanceFrom (mmEnb);
      double distanceFromWrapAroundEnb = mmUe->GetDistanceFrom (mmEnbWAround);

      NS_TEST_ASSERT_MSG_LT_OR_EQ (pLossWithWrapAround, pLossWithoutWrapAround,
                                   "Pathloss from a wrap-around position of an eNB should not be greater than the pathloss from its position in central cluster");

      NS_TEST_ASSERT_MSG_LT_OR_EQ (distanceFromWrapAroundEnb, distanceFromOrignalEnb,
                                   "Distance between an UE and a wrap-around eNB should not be greater than the distance to the same eNB in central cluster");
    }

}


void
WrapAroundTopologyTestCase::DoRun ()
{
  //We do not cache the loss because we want to calculate the pathloss of an eNB in wrap-around as per its position
  //in the central cluster and then we compare that loss value by putting the eNB to its wrap-around position

  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));

  int64_t stream = 1;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // Set EARFCN, bandwidth and TX power
  lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (100.0));
  lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (100.0 + 18000.0));
  lteHelper->SetUeDeviceAttribute ("DlEarfcn", UintegerValue (100.0));
  lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (50.0));
  lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (50.0));
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (46.0));
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

  // Set Hard frequency reuse to assign different RBs to each eNb of 3 sector cell. FrCellTypeId is set by topology helper.
  lteHelper->SetAttribute ("FfrAlgorithm", StringValue ("ns3::LteFrHardAlgorithm"));

  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

  Ptr<Lte3gppHexGridEnbTopologyHelper> topoHelper = CreateObject<Lte3gppHexGridEnbTopologyHelper> ();
  topoHelper->AssignStreams (stream);
  topoHelper->SetLteHelper (lteHelper);
  topoHelper->SetNumRings (m_nRings);
  topoHelper->SetInterSiteDistance (500);
  topoHelper->SetMinimumDistance (12);

  NS_LOG_INFO ("Configured topology Helper");

  lteHelper->SetEnbAntennaModelType ("ns3::Parabolic3dAntennaModel");
  lteHelper->SetEnbAntennaModelAttribute ("MechanicalTilt", DoubleValue (20));
  lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth",   DoubleValue (70));

  NS_LOG_INFO ("Configured Antenna");

  NodeContainer threeSectorNodes;
  threeSectorNodes.Create (topoHelper->GetNumNodes ());

  NodeContainer ueNodes;
  ueNodes.Create (m_nUesPerSector * threeSectorNodes.GetN ());

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
  lteHelper->AssignStreams (enbDevs, stream);
  NS_LOG_INFO ("Installed eNbs");

  NetDeviceContainer ueDevs;

  //Deploys the UEs uniformly (and equally among the cells)
  ueDevs = topoHelper->DropUEsUniformlyPerSector (ueNodes);
  lteHelper->AssignStreams (ueDevs, stream);

  NS_LOG_INFO ("Installed UEs");

  // Lte3gppHexGridEnbTopologyHelper wrap-around method to attach UEs to the eNBs

  //*** Important for LTE scenarios: use downlink pathoss model to calculate pathloss for attaching UEs to the eNBs ***

  Ptr<Object> downlinkPathlossModel = lteHelper->GetDownlinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = downlinkPathlossModel->GetObject<PropagationLossModel> ();

  NS_ABORT_MSG_IF (lossModel == nullptr, "No PathLossModel");
  topoHelper->AttachWithWrapAround (lossModel, ueDevs, enbDevs);
  NS_LOG_INFO ("Attached UE's to the eNB with wrap-around");

  std::map<uint64_t,WrapAroundInfo_t> wrapAroundAttachInfo = topoHelper->GetWrapAroundAttachInfo ();


  std::string frAlgorithmType = lteHelper->GetFfrAlgorithmType ();
  NS_LOG_INFO ("FrAlgorithmType: " << frAlgorithmType);

  // (RLC SM will generate flows in downlink and uplink)

  for (uint32_t u = 0; u < ueDevs.GetN (); ++u)
    {
      enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
      EpsBearer bearer (q);
      lteHelper->ActivateDataRadioBearer (ueDevs.Get (u), bearer);
    }

  NS_LOG_INFO ("Running Simulation");

  Simulator::Stop (m_simulationDuration);

  Simulator::Run ();

  NS_LOG_INFO ("Evaluating wrap-around attach info");

  Evaluate (enbDevs, ueDevs, wrapAroundAttachInfo, lossModel);

  Simulator::Destroy ();

}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Wrap around topology test suite.
 */
class WrapAroundTopologyTestSuite : public TestSuite
{
public:
  WrapAroundTopologyTestSuite ();
};

WrapAroundTopologyTestSuite::WrapAroundTopologyTestSuite ()
  : TestSuite ("wrap-around-topology-test", SYSTEM)
{
  //LogComponentEnable("WrapAroundTopologyTest", LOG_LEVEL_ALL);
  AddTestCase (new WrapAroundTopologyTestCase (2, 2, Seconds (0.8)), TestCase::QUICK);
}

static WrapAroundTopologyTestSuite g_wrapAroundTopologyTestSuite;
