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
#include <ns3/assert.h>
#include "ns3/net-device-queue-interface.h"
#include <ns3/traffic-control-layer.h>
#include <ns3/queue-disc.h>
#include <iostream>
#include <ns3/lte-sl-resource-pool-factory.h>
#include <ns3/lte-sl-disc-resource-pool-factory.h>
#include <ns3/ipv6-static-routing-helper.h>
#include <ns3/lte-sl-preconfig-pool-factory.h>
#include <ns3/lte-sl-disc-preconfig-pool-factory.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSidelinkHelper");


NS_OBJECT_ENSURE_REGISTERED (LteSidelinkHelper);

LteSidelinkHelper::LteSidelinkHelper ()
{
  NS_LOG_FUNCTION (this);
  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
  //Set default addressing scheme for relay
  SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (60));
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
LteSidelinkHelper::StartDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StartDiscoveryApps (appCodes, role);
}
  
void
LteSidelinkHelper::StopDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ()->StopDiscoveryApps (appCodes, role);
}

void
LteSidelinkHelper::StartRelayService (Ptr<NetDevice> ueDevice, uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this);

  Ptr<Ipv4> ipv4 = ueDevice->GetNode ()->GetObject<Ipv4> ();
  if (ipv4->GetNInterfaces() > 1)
    {
      NS_FATAL_ERROR("UE-to-Network Relay is only supported in Ipv6 networks");
    }

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

void
LteSidelinkHelper::SetIpv6BaseForRelayCommunication (Ipv6Address network, Ipv6Prefix prefix)
{
  NS_LOG_FUNCTION (this << network << prefix);

  m_ipv6AddressRelayNetwork = network;
  m_ipv6AddressRelayPrefix = prefix;
  
  //60 is currently set as the maximum allowed number of bits for the subnet part of the address provided by the user
  //We add 4 more bits along with the user provided 60 bits for the 64 bit prefix, which gives us 2^4 combinations of prefixes
  NS_ASSERT_MSG (prefix.GetPrefixLength () <= 64, "Provided a network address of more than 64 bits!");
  
  m_relayIpv6ah.SetBase (network, Ipv6Prefix (64));
}

Ipv6InterfaceContainer
LteSidelinkHelper::AssignIpv6AddressForRelayCommunication (Ptr<NetDevice> dev, uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role)
{
  Ipv6InterfaceContainer retval;

  LteSlO2OIpv6AddressKey key;
  key.relayL2Id  = relayL2Id;
  key.remoteL2Id = remoteL2Id;
  
  LteSlO2OIpv6AddressValue value;
  std::map <LteSlO2OIpv6AddressKey, LteSlO2OIpv6AddressValue>::iterator it = m_ipv6AddressValueMap.find (key);
  if (it != m_ipv6AddressValueMap.end ())
  {
	  value = it->second;
  }
  
  
  if (role == LteSlUeRrc::RelayUE)
  {
      if (value.relay_address == Ipv6Address::GetOnes())
        {
          //first time
	  retval = m_relayIpv6ah.Assign (NetDeviceContainer (dev));
	  value.relay_address = retval.GetAddress (0,1);
	  value.ipv6ah = m_relayIpv6ah;	  
          NS_ASSERT_MSG (value.relay_address.CombinePrefix (m_ipv6AddressRelayPrefix)==m_ipv6AddressRelayNetwork, "Address assigned outside allowed values");
	  m_ipv6AddressValueMap.insert (std::pair<LteSlO2OIpv6AddressKey, LteSlO2OIpv6AddressValue> (key, value));
          NS_LOG_DEBUG ("Assigned IPv6 address " << value.relay_address << " to relay UE node");
          
          //increase value for next assignment
          m_relayIpv6ah.NewNetwork();
        }
      else
        {
          //reuse previously assigned address for given pair of relay-remote UEs
          std::vector<Ipv6Address> addresses;
	  addresses.push_back (value.relay_address);
	  retval = value.ipv6ah.AssignManualAddress (NetDeviceContainer(dev), addresses);
        }
  }
  
  if (role == LteSlUeRrc::RemoteUE)
  {
      if (value.remote_address == Ipv6Address::GetOnes())
        {
          //first time
	  NS_ASSERT_MSG (value.relay_address != Ipv6Address::GetOnes(), "Remote UE cannot initiate IPv6 Address allocation procedure!");
	  
	  retval = value.ipv6ah.Assign (NetDeviceContainer (dev));
	  value.remote_address = retval.GetAddress (0,1);
	  m_ipv6AddressValueMap[key] = value;
          NS_LOG_DEBUG ("Assigned IPv6 address " << value.remote_address << " to remote UE node");
        }
      else
        {
          //reuse previously assigned address for given pair of relay-remote UEs
          std::vector<Ipv6Address> addresses;
	  addresses.push_back (value.remote_address);
	  retval = value.ipv6ah.AssignManualAddress (NetDeviceContainer(dev), addresses);
        }
  }

  //Re-initialize since we have a new NetDevice
  Ptr<TrafficControlLayer> tc = dev->GetNode ()->GetObject<TrafficControlLayer> ();
  if (tc)
    {
      //rescan
      tc->ScanDevices ();
      Ptr<NetDeviceQueueInterface> ndqi = dev->GetObject<NetDeviceQueueInterface> ();
      if (ndqi)
        {
            tc->GetRootQueueDiscOnDevice (dev)->Initialize ();
        }
    }

  return retval;
}

Ipv6Address
LteSidelinkHelper::GetRelayIpv6AddressFromMap (uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role)
{
  LteSlO2OIpv6AddressKey key;
  key.relayL2Id  = relayL2Id;
  key.remoteL2Id = remoteL2Id;
  
  Ipv6Address ipv6a_relay = Ipv6Address("::");
  if (role == LteSlUeRrc::RemoteUE)
  {
	  std::map <LteSlO2OIpv6AddressKey, LteSlO2OIpv6AddressValue>::iterator it_av_map = m_ipv6AddressValueMap.find (key);
	  if (it_av_map == m_ipv6AddressValueMap.end ())
	  {
		  NS_ABORT_MSG ("Relay UE IPv6 address not present in map!");
	  }
	  else
	  {
		  ipv6a_relay = it_av_map->second.relay_address;  
	  }
  }  
  else
  {
	  NS_ABORT_MSG ("Relay UE requesting its own IPv6 address!");
  }
  return ipv6a_relay;
}

Ipv6Address
LteSidelinkHelper::GetSelfIpv6AddressFromMap (uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << relayL2Id << remoteL2Id << role);
  LteSlO2OIpv6AddressKey key;
  key.relayL2Id  = relayL2Id;
  key.remoteL2Id = remoteL2Id;
  
  Ipv6Address ipv6a = Ipv6Address("::");
  
  std::map <LteSlO2OIpv6AddressKey, LteSlO2OIpv6AddressValue>::iterator it_av_map = m_ipv6AddressValueMap.find (key);
  if (it_av_map == m_ipv6AddressValueMap.end ())
  {
	  NS_ABORT_MSG ("IPv6 address not present in map!");
  }
  else
  {
	  if (role == LteSlUeRrc::RemoteUE)
	  {
		  ipv6a = it_av_map->second.remote_address;
	  }
	  else
	  {
		  ipv6a = it_av_map->second.relay_address;
	  }
  }
  return ipv6a;
}

void
LteSidelinkHelper::RemoteUeContextConnected (uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8])
{
  NS_LOG_FUNCTION (this << relayImsi << ueImsi << ipv6Prefix);
  std::ostringstream oss;
  uint8_t address[16];
  std::fill_n (std::begin (address), 16, 0);
  std::memmove (address, ipv6Prefix, 8);
  oss << Ipv6Address (address);
  NS_LOG_DEBUG (" prefix=" << oss.str());
  m_lteHelper->RemoteUeContextConnected (relayImsi, ueImsi, ipv6Prefix);
}

void
LteSidelinkHelper::RemoteUeContextDisconnected (uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8])
{
  NS_LOG_FUNCTION (this << relayImsi << ueImsi << ipv6Prefix);
  m_lteHelper->RemoteUeContextDisconnected (relayImsi, ueImsi, ipv6Prefix);
}

LteRrcSap::SlCommTxResourcesSetup
LteSidelinkHelper::GetDefaultSlCommTxResourcesSetupUeSelected () const
{
  LteRrcSap::SlCommTxResourcesSetup pool;
  pool.setup = LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlResourcePoolFactory pfactory;
  pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();

  return pool;
}

LteRrcSap::SlCommTxResourcesSetup
LteSidelinkHelper::GetDefaultSlCommTxResourcesSetupScheduled () const
{
  LteRrcSap::SlCommTxResourcesSetup pool;

  pool.setup = LteRrcSap::SlCommTxResourcesSetup::SCHEDULED;
  pool.scheduled.macMainConfig.periodicBsrTimer.period = LteRrcSap::PeriodicBsrTimer::sf16;
  pool.scheduled.macMainConfig.retxBsrTimer.period = LteRrcSap::RetxBsrTimer::sf640;
  pool.scheduled.haveMcs = true;
  pool.scheduled.mcs = 16;

  LteSlResourcePoolFactory pfactory;
  pfactory.SetHaveUeSelectedResourceConfig (false);

  pool.scheduled.commTxConfig = pfactory.CreatePool ();

  return pool;

}

LteRrcSap::SlDiscTxResourcesSetup
LteSidelinkHelper::GetDefaultSlDiscTxResourcesSetupUeSelected () const
{
  LteRrcSap::SlDiscTxResourcesSetup pool;

  pool.setup =  LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlDiscResourcePoolFactory pDiscFactory;
  pool.ueSelected.poolToAddModList.pools[0].pool = pDiscFactory.CreatePool ();

  return pool;
}

LteRrcSap::Sib19DiscConfigRelay
LteSidelinkHelper::GetDefaultSib19DiscConfigRelay () const
{
  LteRrcSap::Sib19DiscConfigRelay configRelay;

  configRelay.haveRemoteUeConfig = true;
  configRelay.remoteUeConfig.haveReselectionInfoIc = true;
  configRelay.remoteUeConfig.reselectionInfoIc.filterCoefficient = 0;
  configRelay.remoteUeConfig.reselectionInfoIc.minHyst = 0;
  configRelay.remoteUeConfig.reselectionInfoIc.qRxLevMin = -125;

  return configRelay;
}

void
LteSidelinkHelper::ConfigurePgwToUeToNetworkRelayRoute (Ptr<Node> pgw)
{
  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> pgwStaticRouting = ipv6RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv6> ());
  pgwStaticRouting->AddNetworkRouteTo (m_ipv6AddressRelayNetwork, m_ipv6AddressRelayPrefix, Ipv6Address ("::"), 1, 0);

}

Ipv6Address
LteSidelinkHelper::GetIpv6NetworkForRelayCommunication () const
{
  return m_ipv6AddressRelayNetwork;
}

Ipv6Prefix
LteSidelinkHelper::GetIpv6PrefixForRelayCommunication () const
{
  return m_ipv6AddressRelayPrefix;
}

LteRrcSap::SlPreconfigCommPoolList
LteSidelinkHelper::GetDefaultSlPreconfigCommPoolList () const
{
  LteRrcSap::SlPreconfigCommPoolList preconfigComm;
  preconfigComm.nbPools = 1;

  LteSlPreconfigPoolFactory preconfCommPoolFactory;
  preconfigComm.pools[0] = preconfCommPoolFactory.CreatePool ();

  return preconfigComm;
}

LteRrcSap::SlPreconfigDiscPoolList
LteSidelinkHelper::GetDefaultSlPreconfigDiscPoolList () const
{
  LteRrcSap::SlPreconfigDiscPoolList preconfigDisc;
  preconfigDisc.nbPools = 1;
  LteSlDiscPreconfigPoolFactory preconfDiscPoolFactory;
  preconfigDisc.pools[0] = preconfDiscPoolFactory.CreatePool ();

  return preconfigDisc;
}

LteRrcSap::SlPreconfigRelay
LteSidelinkHelper::GetDefaultSlPreconfigRelay () const
{
  LteRrcSap::SlPreconfigRelay preconfigRelay;
  preconfigRelay.haveReselectionInfoOoc = true;
  preconfigRelay.reselectionInfoOoc.filterCoefficient = 0;
  preconfigRelay.reselectionInfoOoc.minHyst = 0;
  preconfigRelay.reselectionInfoOoc.qRxLevMin = -125;

  return preconfigRelay;
}

} // namespace ns3

