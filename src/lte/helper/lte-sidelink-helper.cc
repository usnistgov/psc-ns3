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

#include <ns3/lte-sidelink-helper.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/epc-helper.h>
#include <ns3/angles.h>
#include <ns3/random-variable-stream.h>
#include <iostream>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSidelinkHelper");


NS_OBJECT_ENSURE_REGISTERED (LteSidelinkHelper);

LteSidelinkHelper::LteSidelinkHelper ()
{
  NS_LOG_FUNCTION (this);
  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
}

LteSidelinkHelper::~LteSidelinkHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId LteSidelinkHelper::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::LteSidelinkHelper")
    .SetParent<Object> ()
    .AddConstructor<LteSidelinkHelper> ()

  ;
  return tid;
}

void
LteSidelinkHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}

void
LteSidelinkHelper::SetLteHelper (Ptr<LteHelper> h)
{
  NS_LOG_FUNCTION (this << h);
  m_lteHelper = h;
}

NetDeviceContainer
LteSidelinkHelper::RemoveNetDevice (NetDeviceContainer container, Ptr<NetDevice> item)
{
  NetDeviceContainer newContainer;
  uint32_t nDevices = container.GetN ();
  for (uint32_t i = 0; i < nDevices; ++i)
    {
      Ptr<NetDevice> p = container.Get (i);
      if (item != p)
        {
          newContainer.Add (p);
        }
    }
  return newContainer;
}


std::vector < NetDeviceContainer >
LteSidelinkHelper::AssociateForGroupcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, int nGroups, int nReceivers, SrsrpMethod_t compMethod)
{
  std::vector < NetDeviceContainer > groups; //groups created

  NetDeviceContainer remainingUes; //list of UEs not assigned to groups
  remainingUes.Add (ues);

  // Start association of groupcast links, set NUM_GROUPS_ASSOCIATED = 0.
  int32_t numGroupsAssociated = 0;

  NetDeviceContainer candidateTx; //list of UEs not assigned to groups that can be selected for transmission
  candidateTx.Add (ues);

  Ptr<Object> uplinkPathlossModel = m_lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << uplinkPathlossModel << " is not a PropagationLossModel");

  while (numGroupsAssociated < nGroups && candidateTx.GetN () > 0)
    {
      //Transmitter UE is randomly selected from the total number of UEs.
      uint32_t iTx = m_uniformRandomVariable->GetValue (0, candidateTx.GetN ());
      Ptr<NetDevice> tx = candidateTx.Get (iTx);
      NS_LOG_DEBUG (" Candidate Tx= " << tx->GetNode ()->GetId ());
      candidateTx = RemoveNetDevice (candidateTx, tx);
      //build list of candidate receivers
      NetDeviceContainer candidateRx = RemoveNetDevice (remainingUes, tx);
      NetDeviceContainer selectedRx;

      //Start selecting the receiver for the transmitter, set NUM_RECEIVERS_ASSOCIATED = 0.
      int32_t numReceiversAssociated = 0;

      //Receiver UE is randomly selected from the remaining UEs (i.e., not already part of a group).
      while (numReceiversAssociated < nReceivers && candidateRx.GetN () > 0)
        {
          uint32_t iRx = m_uniformRandomVariable->GetValue (0, candidateRx.GetN ());
          Ptr<NetDevice> rx = candidateRx.Get (iRx);
          candidateRx = RemoveNetDevice (candidateRx, rx);
          double rsrpRx = 0;

          Ptr<SpectrumPhy> txPhy = tx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();
          Ptr<SpectrumPhy> rxPhy = rx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();

          if (compMethod == LteSidelinkHelper::SLRSRP_PSBCH)
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpPsbch (lossModel, txPower, ulEarfcn, ulBandwidth, txPhy, rxPhy);
            }
          else
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, rxPhy);
            }
          //If receiver UE is not within RSRP* of X dBm of the transmitter UE then randomly reselect the receiver UE among the UEs that are
          //within the RSRP of X dBm of the transmitter UE and are not part of a group already.
          NS_LOG_DEBUG ("\tCandidate Rx= " << rx->GetNode ()->GetId () << " Rsrp=" << rsrpRx << " required=" << rsrpThreshold);
          if (rsrpRx >= rsrpThreshold)
            {
              //good receiver
              selectedRx.Add (rx);
              numReceiversAssociated++;
              NS_LOG_DEBUG ("\tAdding Rx to group");
            }
        }
      if (numReceiversAssociated == nReceivers)
        {
          NS_LOG_DEBUG (" Group successfully created");
          //found all the receivers, update lists
          //remove receivers from candidate Tx and remaining nodes
          remainingUes = RemoveNetDevice (remainingUes, tx); //remove selected Tx
          uint32_t nDevices = selectedRx.GetN ();
          for (uint32_t i = 0; i < nDevices; ++i)
            {
              Ptr<NetDevice> p = selectedRx.Get (i);
              candidateTx = RemoveNetDevice (candidateTx, p);
              remainingUes = RemoveNetDevice (remainingUes, p);
            }
          NetDeviceContainer newGroup (tx);
          newGroup.Add (selectedRx);
          groups.push_back (newGroup);
          numGroupsAssociated++;
        }
      else
        {
          NS_LOG_DEBUG (" Group failed. Found only " << numReceiversAssociated << " receivers");
        }
    }

  NS_LOG_INFO ("Groups created " << groups.size () << " expected " << nGroups);

  return groups;
}

std::vector < NetDeviceContainer >
LteSidelinkHelper::AssociateForBroadcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, SrsrpMethod_t compMethod)
{
  std::vector < NetDeviceContainer > groups; //groups created

  NetDeviceContainer remainingUes; //list of UEs not assigned to groups
  remainingUes.Add (ues);

  // Start the selection of the transmitters
  NetDeviceContainer candidateTx; //list of UEs that can be selected for transmission
  candidateTx.Add (ues);
  NetDeviceContainer selectedTx;
  uint32_t numTransmittersSelected = 0;

  Ptr<Object> uplinkPathlossModel = m_lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << uplinkPathlossModel << " is not a PropagationLossModel");

  while (numTransmittersSelected < nTransmitters)
    {
      //Transmitter UE is randomly selected from the total number of UEs.
      uint32_t iTx = m_uniformRandomVariable->GetValue (0, candidateTx.GetN ());
      Ptr<NetDevice> tx = candidateTx.Get (iTx);
      NS_LOG_DEBUG (" Candidate Tx= " << tx->GetNode ()->GetId ());
      selectedTx.Add (tx);
      candidateTx = RemoveNetDevice (candidateTx, tx);
      remainingUes = RemoveNetDevice (remainingUes, tx);
      numTransmittersSelected++;
    }

  //For each remaining UE, associate to all transmitters where RSRP is greater than X dBm
  for (uint32_t i = 0; i < numTransmittersSelected; i++)
    {
      Ptr<NetDevice> tx = selectedTx.Get (i);
      //prepare group for this transmitter
      NetDeviceContainer newGroup (tx);
      uint32_t nRxDevices = remainingUes.GetN ();
      double rsrpRx = 0;

      for (uint32_t j = 0; j < nRxDevices; ++j)
        {
          Ptr<NetDevice> rx = remainingUes.Get (j);

          Ptr<SpectrumPhy> txPhy = tx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();
          Ptr<SpectrumPhy> rxPhy = rx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();

          if (compMethod == LteSidelinkHelper::SLRSRP_PSBCH)
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpPsbch (lossModel, txPower, ulEarfcn, ulBandwidth, txPhy, rxPhy);
            }
          else
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, rxPhy);
            }
          //If receiver UE is not within RSRP* of X dBm of the transmitter UE then randomly reselect the receiver UE among the UEs
          //that are within the RSRP of X dBm of the transmitter UE and are not part of a group already.
          NS_LOG_DEBUG ("\tCandidate Rx= " << rx->GetNode ()->GetId () << " Rsrp=" << rsrpRx << " required=" << rsrpThreshold);
          if (rsrpRx >= rsrpThreshold)
            {
              //good receiver
              NS_LOG_DEBUG ("\tAdding Rx to group");
              newGroup.Add (rx);
            }
        }

      //Initializing link to other transmitters to be able to receive SLSSs from other transmitters
      for (uint32_t k = 0; k < numTransmittersSelected; k++)
        {
          if (k != i)
            {
              Ptr<NetDevice> othertx = selectedTx.Get (k);

              Ptr<SpectrumPhy> txPhy = tx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();
              Ptr<SpectrumPhy> otherTxPhy = othertx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();

              if (compMethod == LteSidelinkHelper::SLRSRP_PSBCH)
                {
                  rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpPsbch (lossModel, txPower, ulEarfcn, ulBandwidth, txPhy, otherTxPhy);
                }
              else
                {
                  rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, otherTxPhy);
                }
              NS_LOG_DEBUG ("\tOther Tx= " << othertx->GetNode ()->GetId () << " Rsrp=" << rsrpRx);
            }
        }

      groups.push_back (newGroup);
    }

  return groups;
}

std::vector < NetDeviceContainer >
LteSidelinkHelper::AssociateForBroadcastWithTxEnabledToReceive (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, SrsrpMethod_t compMethod)
{
  std::vector < NetDeviceContainer > groups; //groups created

  NetDeviceContainer remainingUes; //list of UEs not assigned to groups
  remainingUes.Add (ues);

  // Start the selection of the transmitters
  NetDeviceContainer candidateTx; //list of UEs that can be selected for transmission
  candidateTx.Add (ues);
  NetDeviceContainer selectedTx;
  uint32_t numTransmittersSelected = 0;

  Ptr<Object> uplinkPathlossModel = m_lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << uplinkPathlossModel << " is not a PropagationLossModel");

  while (numTransmittersSelected < nTransmitters)
    {
      //Transmitter UE is randomly selected from the total number of UEs.
      uint32_t iTx = m_uniformRandomVariable->GetValue (0, candidateTx.GetN ());
      Ptr<NetDevice> tx = candidateTx.Get (iTx);
      NS_LOG_DEBUG (" Candidate Tx= " << tx->GetNode ()->GetId ());
      selectedTx.Add (tx);
      candidateTx = RemoveNetDevice (candidateTx, tx);
      numTransmittersSelected++;
    }

  //For each remaining UE, associate to all transmitters where RSRP is greater than X dBm
  for (uint32_t i = 0; i < numTransmittersSelected; i++)
    {
      Ptr<NetDevice> tx = selectedTx.Get (i);
      //prepare group for this transmitter
      NetDeviceContainer newGroup (tx);
      uint32_t nRxDevices = remainingUes.GetN ();
      double rsrpRx = 0;

      for (uint32_t j = 0; j < nRxDevices; ++j)
        {
          Ptr<NetDevice> rx = remainingUes.Get (j);
          if (rx->GetNode ()->GetId () != tx->GetNode ()->GetId ()) //No loopback link possible due to half-duplex
            {
              Ptr<SpectrumPhy> txPhy = tx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();
              Ptr<SpectrumPhy> rxPhy = rx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();

              if (compMethod == LteSidelinkHelper::SLRSRP_PSBCH)
                {

                  rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpPsbch (lossModel, txPower, ulEarfcn, ulBandwidth, txPhy, rxPhy);
                }
              else
                {
                  rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, rxPhy);
                }
              //If receiver UE is not within RSRP* of X dBm of the transmitter UE then randomly reselect the receiver UE among the UEs that are within the RSRP of X dBm of the transmitter UE and are not part of a group already.
              NS_LOG_DEBUG ("\tCandidate Rx= " << rx->GetNode ()->GetId () << " Rsrp=" << rsrpRx << " required=" << rsrpThreshold);
              if (rsrpRx >= rsrpThreshold)
                {
                  //good receiver
                  NS_LOG_DEBUG ("\tAdding Rx to group");
                  newGroup.Add (rx);
                }
            }
        }
      groups.push_back (newGroup);
    }

  return groups;
}

std::vector < NetDeviceContainer >
LteSidelinkHelper::AssociateForBroadcastWithWrapAround (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, Ptr<Lte3gppHexGridEnbTopologyHelper> topologyHelper, SrsrpMethod_t compMethod)
{
  std::vector < NetDeviceContainer > groups; //groups created

  NetDeviceContainer remainingUes; //list of UEs not assigned to groups
  remainingUes.Add (ues);

  // Start the selection of the transmitters
  NetDeviceContainer candidateTx; //list of UEs that can be selected for transmission
  candidateTx.Add (ues);
  NetDeviceContainer selectedTx;
  uint32_t numTransmittersSelected = 0;

  Ptr<Object> uplinkPathlossModel = m_lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << uplinkPathlossModel << " is not a PropagationLossModel");

  while (numTransmittersSelected < nTransmitters)
    {
      //Transmitter UE is randomly selected from the total number of UEs.
      uint32_t iTx = m_uniformRandomVariable->GetValue (0, candidateTx.GetN ());
      Ptr<NetDevice> tx = candidateTx.Get (iTx);
      NS_LOG_DEBUG (" Candidate Tx= " << tx->GetNode ()->GetId ());
      selectedTx.Add (tx);
      candidateTx = RemoveNetDevice (candidateTx, tx);
      remainingUes = RemoveNetDevice (remainingUes, tx);
      numTransmittersSelected++;
    }

  //For each remaining UE, associate to all transmitters where RSRP is greater than X dBm
  for (uint32_t i = 0; i < numTransmittersSelected; i++)
    {
      Ptr<NetDevice> tx = selectedTx.Get (i);
      //prepare group for this transmitter
      NetDeviceContainer newGroup (tx);
      uint32_t nRxDevices = remainingUes.GetN ();
      double rsrpRx = 0;

      for (uint32_t j = 0; j < nRxDevices; ++j)
        {
          Ptr<NetDevice> rx = remainingUes.Get (j);
          //With wrap around, the closest location may be in one of the extended hexagon
          //store position
          Vector rxPos = rx->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
          Vector closestPos = topologyHelper->GetClosestPositionInWrapAround (tx->GetNode ()->GetObject<MobilityModel> ()->GetPosition (), rxPos);
          //assign temporary position to compute RSRP
          rx->GetNode ()->GetObject<MobilityModel> ()->SetPosition (closestPos);

          Ptr<SpectrumPhy> txPhy = tx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();
          Ptr<SpectrumPhy> rxPhy = rx->GetObject<LteUeNetDevice> ()->GetPhy ()->GetUlSpectrumPhy ();

          if (compMethod == LteSidelinkHelper::SLRSRP_PSBCH)
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpPsbch (lossModel, txPower, ulEarfcn, ulBandwidth, txPhy, rxPhy);
            }
          else
            {
              rsrpRx = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, rxPhy);
            }
          //If receiver UE is not within RSRP* of X dBm of the transmitter UE then randomly reselect the receiver UE among the UEs that are within the RSRP of X dBm of the transmitter UE and are not part of a group already.
          NS_LOG_DEBUG ("\tCandidate Rx= " << rx->GetNode ()->GetId () << " Rsrp=" << rsrpRx << " required=" << rsrpThreshold);
          if (rsrpRx >= rsrpThreshold)
            {
              //good receiver
              NS_LOG_DEBUG ("\tAdding Rx to group");
              newGroup.Add (rx);
            }

          //restore position
          rx->GetNode ()->GetObject<MobilityModel> ()->SetPosition (rxPos);
        }
      groups.push_back (newGroup);
    }

  return groups;
}



void
LteSidelinkHelper::PrintGroups (std::vector < NetDeviceContainer > groups)
{
  std::vector < NetDeviceContainer >::iterator gIt;
  for (gIt = groups.begin (); gIt != groups.end (); gIt++)
    {
      std::cout << "Tx=" << (*gIt).Get (0)->GetNode ()->GetId () << " Rx=";
      for (uint32_t i = 1; i < (*gIt).GetN (); i++)
        {
          std::cout << (*gIt).Get (i)->GetNode ()->GetId () << " ";
        }
      std::cout << std::endl;
    }
}

void
LteSidelinkHelper::PrintGroups (std::vector < NetDeviceContainer > groups, Ptr< OutputStreamWrapper > stream)
{
  *stream->GetStream () << "TxNID\tRxNID\tTxIMSI\tRxIMSI" << std::endl;
  std::vector < NetDeviceContainer >::iterator gIt;
  for (gIt = groups.begin (); gIt != groups.end (); gIt++)
    {
      if ((*gIt).GetN () < 2)
        {        //No receivers in group!
          *stream->GetStream () << (*gIt).Get (0)->GetNode ()->GetId () << "\t0" << std::endl;
        }
      else
        {
          for (uint32_t i = 1; i < (*gIt).GetN (); i++)
            {
              *stream->GetStream () << (*gIt).Get (0)->GetNode ()->GetId () << "\t" << (*gIt).Get (i)->GetNode ()->GetId () << "\t" << (*gIt).Get (0)->GetObject<LteUeNetDevice> ()->GetImsi () << "\t" << (*gIt).Get (i)->GetObject<LteUeNetDevice> ()->GetImsi () << std::endl;
            }
        }
    }
}

void
LteSidelinkHelper::ActivateSidelinkBearer (Time activationTime, NetDeviceContainer ues, Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_lteHelper, "Sidelink activation requires LteHelper to be registered with the LteSidelinkHelper");
  Simulator::Schedule (activationTime, &LteSidelinkHelper::DoActivateSidelinkBearer, this, ues, tft);
}

void
LteSidelinkHelper::DoActivateSidelinkBearer (NetDeviceContainer ues, Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  m_lteHelper->ActivateSidelinkBearer (ues, tft);
}

void
LteSidelinkHelper::StartDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint64_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StartDiscoveryApps (appCodes, role);
}
  
void
LteSidelinkHelper::StopDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint64_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StopDiscoveryApps (appCodes, role);
}

void
LteSidelinkHelper::StartRelayService (Ptr<NetDevice> ueDevice, uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StartRelayService (serviceCode, model, role);
}

void
LteSidelinkHelper::StopRelayService (Ptr<NetDevice> ueDevice, uint32_t serviceCode)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StopRelayService (serviceCode);
}

int64_t
LteSidelinkHelper::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  return 1;
}


} // namespace ns3

