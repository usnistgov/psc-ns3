/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es> (re-wrote from scratch this helper)
 *         Giuseppe Piro <g.piro@poliba.it> (parts of the PHY & channel  creation & configuration copied from the GSoC 2011 code)
 * Modified by: NIST
 */


#include "nist-lte-helper.h"
#include <ns3/string.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/nist-lte-enb-rrc.h>
#include <ns3/nist-epc-ue-nas.h>
#include <ns3/nist-epc-enb-application.h>
#include <ns3/nist-lte-ue-rrc.h>
#include <ns3/nist-lte-ue-mac.h>
#include <ns3/nist-lte-enb-mac.h>
#include <ns3/nist-lte-enb-net-device.h>
#include <ns3/nist-lte-enb-phy.h>
#include <ns3/nist-lte-ue-phy.h>
#include <ns3/nist-lte-spectrum-phy.h>
#include <ns3/nist-lte-chunk-processor.h>
#include <ns3/nist-lte-sl-chunk-processor.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/friis-spectrum-propagation-loss.h>
#include <ns3/nist-trace-fading-loss-model.h>
#include <ns3/isotropic-antenna-model.h>
#include <ns3/nist-lte-enb-net-device.h>
#include <ns3/nist-lte-ue-net-device.h>
#include <ns3/nist-ff-mac-scheduler.h>
#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/nist-lte-handover-algorithm.h>
#include <ns3/nist-lte-anr.h>
#include <ns3/nist-lte-rlc.h>
#include <ns3/nist-lte-rlc-um.h>
#include <ns3/nist-lte-rlc-am.h>
#include <ns3/nist-epc-enb-s1-sap.h>
#include <ns3/nist-lte-rrc-protocol-ideal.h>
#include <ns3/nist-lte-rrc-protocol-real.h>
#include <ns3/nist-mac-stats-calculator.h>
#include <ns3/nist-phy-stats-calculator.h>
#include <ns3/nist-phy-tx-stats-calculator.h>
#include <ns3/nist-phy-rx-stats-calculator.h>
#include <ns3/nist-epc-helper.h>
#include <iostream>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/nist-lte-spectrum-value-helper.h>
#include <ns3/nist-epc-x2.h>
#include <cfloat>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteHelper");

NS_OBJECT_ENSURE_REGISTERED (NistLteHelper);

NistLteHelper::NistLteHelper (void)
  : m_fadingStreamsAssigned (false),
    m_imsiCounter (0),
    m_cellIdCounter (0),
    m_EnbEnablePhyLayer (true)
{
  NS_LOG_FUNCTION (this);
  m_enbNetDeviceFactory.SetTypeId (NistLteEnbNetDevice::GetTypeId ());
  m_enbAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());
  m_ueNetDeviceFactory.SetTypeId (NistLteUeNetDevice::GetTypeId ());
  m_ueAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());
  m_channelFactory.SetTypeId (MultiModelSpectrumChannel::GetTypeId ());
}

void 
NistLteHelper::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_downlinkChannel = m_channelFactory.Create<SpectrumChannel> ();
  m_uplinkChannel = m_channelFactory.Create<SpectrumChannel> ();

  m_downlinkPathlossModel = m_dlPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> dlSplm = m_downlinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();
  if (dlSplm != 0)
    {
      NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in DL");
      m_downlinkChannel->AddSpectrumPropagationLossModel (dlSplm);
    }
  else
    {
      NS_LOG_LOGIC (this << " using a PropagationLossModel in DL");
      Ptr<PropagationLossModel> dlPlm = m_downlinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (dlPlm != 0, " " << m_downlinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_downlinkChannel->AddPropagationLossModel (dlPlm);
    }

  //Nist
  if (m_sameUlDlPropagationCondition)
  {
    m_uplinkPathlossModel = m_downlinkPathlossModel;
  }
  else
  {
    m_uplinkPathlossModel = m_ulPathlossModelFactory.Create ();
  }
  //
  Ptr<SpectrumPropagationLossModel> ulSplm = m_uplinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();
  if (ulSplm != 0)
    {
      NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in UL");
      m_uplinkChannel->AddSpectrumPropagationLossModel (ulSplm);
    }
  else
    {
      NS_LOG_LOGIC (this << " using a PropagationLossModel in UL");
      Ptr<PropagationLossModel> ulPlm = m_uplinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (ulPlm != 0, " " << m_uplinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_uplinkChannel->AddPropagationLossModel (ulPlm);
    }
  
  if (!m_fadingModelType.empty ())
    {
      m_fadingModule = m_fadingModelFactory.Create<SpectrumPropagationLossModel> ();
      m_fadingModule->Initialize ();
      m_downlinkChannel->AddSpectrumPropagationLossModel (m_fadingModule);
      m_uplinkChannel->AddSpectrumPropagationLossModel (m_fadingModule);
    }
  m_phyStats = CreateObject<NistPhyStatsCalculator> ();
  m_phyTxStats = CreateObject<NistPhyTxStatsCalculator> ();
  m_phyRxStats = CreateObject<NistPhyRxStatsCalculator> ();
  m_macStats = CreateObject<NistMacStatsCalculator> ();
  Object::DoInitialize ();

}

NistLteHelper::~NistLteHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId NistLteHelper::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NistLteHelper")
    .SetParent<Object> ()
    .AddConstructor<NistLteHelper> ()
    .AddAttribute ("Scheduler",
                   "The type of scheduler to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::NistFfMacScheduler.",
                   StringValue ("ns3::NistRrFfMacScheduler"),
                   MakeStringAccessor (&NistLteHelper::SetSchedulerType,
                                       &NistLteHelper::GetSchedulerType),
                   MakeStringChecker ())
    .AddAttribute ("UlScheduler",    
                   "The type of UL scheduler to be used for UEs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::NistFfMacScheduler.",
                   StringValue ("ns3::NistRrFfMacScheduler"),
                   MakeStringAccessor (&NistLteHelper::SetUlSchedulerType,
                                       &NistLteHelper::GetUlSchedulerType),
                   MakeStringChecker ())
    .AddAttribute ("FfrAlgorithm",
                   "The type of FFR algorithm to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::NistLteFfrAlgorithm.",
                   StringValue ("ns3::NistLteFrNoOpAlgorithm"),
                   MakeStringAccessor (&NistLteHelper::SetFfrAlgorithmType,
                                       &NistLteHelper::GetFfrAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("HandoverAlgorithm",
                   "The type of handover algorithm to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::NistLteHandoverAlgorithm.",
                   StringValue ("ns3::NistNoOpHandoverAlgorithm"),
                   MakeStringAccessor (&NistLteHelper::SetHandoverAlgorithmType,
                                       &NistLteHelper::GetHandoverAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("PathlossModel",
                   "The type of pathloss model to be used. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::PropagationLossModel.",
                   StringValue ("ns3::FriisPropagationLossModel"),
                   MakeStringAccessor (&NistLteHelper::SetPathlossModelType),
                   MakeStringChecker ())
    .AddAttribute ("FadingModel",
                   "The type of fading model to be used."
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::SpectrumPropagationLossModel."
                   "If the type is set to an empty string, no fading model is used.",
                   StringValue (""),
                   MakeStringAccessor (&NistLteHelper::SetFadingModel),
                   MakeStringChecker ())
    .AddAttribute ("UseIdealRrc",
                   "If true, LteRrcProtocolIdeal will be used for RRC signaling. "
                   "If false, LteRrcProtocolReal will be used.",
                   BooleanValue (true), 
                   MakeBooleanAccessor (&NistLteHelper::m_useIdealRrc),
                   MakeBooleanChecker ())
    .AddAttribute ("AnrEnabled",
                   "Activate or deactivate Automatic Neighbour Relation function",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NistLteHelper::m_isAnrEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("UsePdschForCqiGeneration",
                   "If true, DL-CQI will be calculated from PDCCH as signal and PDSCH as interference "
                   "If false, DL-CQI will be calculated from PDCCH as signal and PDCCH as interference  ",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NistLteHelper::m_usePdschForCqiGeneration),
                   MakeBooleanChecker ())
    .AddAttribute ("UseSidelink",
                   "If true, UEs will be able to receive sidelink communication. "
                   "If false, sidelink communication will not be possible.",
                   BooleanValue (false), 
                   MakeBooleanAccessor (&NistLteHelper::m_useSidelink),
                   MakeBooleanChecker ())
    .AddAttribute ("UseDiscovery",
                   "If true, UEs will be able to do discovery. "
                   "If false, discovery will not be possible.",
                   BooleanValue (false), 
                   MakeBooleanAccessor (&NistLteHelper::m_useDiscovery),
                   MakeBooleanChecker ())
    .AddAttribute ("EnbEnablePhyLayer",
                   "If true, normal eNB operation (default). "
                   "If false, eNB physical layer disable.",
                   BooleanValue (true), 
                   MakeBooleanAccessor (&NistLteHelper::m_EnbEnablePhyLayer),
                   MakeBooleanChecker ())
    .AddAttribute ("UseSameUlDlPropagationCondition",
                   "If true, same conditions for both UL and DL"
                   "If false, different instances of the pathloss model",
                   BooleanValue (false),
                   MakeBooleanAccessor (&NistLteHelper::m_sameUlDlPropagationCondition),
                   MakeBooleanChecker ())
  ;
  return tid;
}

void
NistLteHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_downlinkChannel = 0;
  m_uplinkChannel = 0;
  Object::DoDispose ();
}

void
NistLteHelper::EnableNewEnbPhy ()
{
  NS_LOG_FUNCTION (this);
  m_EnbEnablePhyLayer = true;
}

void
NistLteHelper::DisableNewEnbPhy ()
{
  NS_LOG_FUNCTION (this);
  m_EnbEnablePhyLayer = false;
}

void 
NistLteHelper::SetEpcHelper (Ptr<NistEpcHelper> h)
{
  NS_LOG_FUNCTION (this << h);
  m_epcHelper = h;
}

void 
NistLteHelper::SetSchedulerType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);

  if ( (type =="ns3::NistPriorityFfMacScheduler") || ( type == "ns3::NistPfFfMacScheduler") || ( type == "ns3::NistMtFfMacScheduler" )  || ( type == "ns3::NistRrSlFfMacScheduler") || (type == "ns3::Nist3GPPcalMacScheduler")) 
    {
       //this DL scheduler has a correspandant UL scheduler 
       SetUlSchedulerType (type);   
    }
  else if ( (type =="ns3::NistFdMtFfMacScheduler") || (type == "ns3::NistTdMtFfMacScheduler") || (type == "ns3::NistFdBetFfMacScheduler")  || (type == "ns3::NistTdBetFfMacScheduler")  || (type == "ns3::NistTtaFfMacScheduler") || (type  == "ns3::NistTdTbfqFfMacScheduler")  || (type == "ns3::NistFdTbfqFfMacScheduler")   || (type == "ns3::NistPssFfMacScheduler")  || (type == "ns3::NistRrFfMacScheduler")) 
   {
      // these DL schedulers don't have a corresponding UE scheduler 
      SetUlSchedulerType ("ns3::NistRrFfMacScheduler");
   }

  else
   {
      NS_FATAL_ERROR ("unknown Scheduler type " << type);
    }

  m_schedulerFactory = ObjectFactory ();
  m_schedulerFactory.SetTypeId (type);
}

void 
NistLteHelper::SetUlSchedulerType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  if ( (type == "ns3::NistRrFfMacScheduler") || (type == "ns3::NistPriorityFfMacScheduler") || (type == "ns3::NistPfFfMacScheduler") || (type == "ns3::NistMtFfMacScheduler") || ( type == "ns3::NistRrSlFfMacScheduler") || (type == "ns3::Nist3GPPcalMacScheduler"))
   {
     m_UlschedulerFactory = ObjectFactory ();
     m_UlschedulerFactory.SetTypeId (type);
   }
  else
     NS_FATAL_ERROR ("unknown UE Scheduler type " << type << " ; The only schedulers implemented are : RR, MT, PF, Priority schedulers, FDM: All in from 3GPPcal.");
}

std::string
NistLteHelper::GetSchedulerType () const
{
  return m_schedulerFactory.GetTypeId ().GetName ();
} 

std::string
NistLteHelper::GetUlSchedulerType () const
{
  return m_UlschedulerFactory.GetTypeId ().GetName ();
} 



void 
NistLteHelper::SetSchedulerAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_schedulerFactory.Set (n, v);
}

std::string
NistLteHelper::GetFfrAlgorithmType () const
{
  return m_ffrAlgorithmFactory.GetTypeId ().GetName ();
}

void
NistLteHelper::SetFfrAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_ffrAlgorithmFactory = ObjectFactory ();
  m_ffrAlgorithmFactory.SetTypeId (type);
}

void
NistLteHelper::SetFfrAlgorithmAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_ffrAlgorithmFactory.Set (n, v);
}

std::string
NistLteHelper::GetHandoverAlgorithmType () const
{
  return m_handoverAlgorithmFactory.GetTypeId ().GetName ();
}

void
NistLteHelper::SetHandoverAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_handoverAlgorithmFactory = ObjectFactory ();
  m_handoverAlgorithmFactory.SetTypeId (type);
}

void
NistLteHelper::SetHandoverAlgorithmAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_handoverAlgorithmFactory.Set (n, v);
}


void 
NistLteHelper::SetPathlossModelType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_dlPathlossModelFactory = ObjectFactory ();
  m_dlPathlossModelFactory.SetTypeId (type);
  m_ulPathlossModelFactory = ObjectFactory ();
  m_ulPathlossModelFactory.SetTypeId (type);
}

void 
NistLteHelper::SetPathlossModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this << n);
  m_dlPathlossModelFactory.Set (n, v);
  m_ulPathlossModelFactory.Set (n, v);
}

void
NistLteHelper::SetEnbDeviceAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_enbNetDeviceFactory.Set (n, v);
}


void 
NistLteHelper::SetEnbAntennaModelType (std::string type)
{
  NS_LOG_FUNCTION (this);
  m_enbAntennaModelFactory.SetTypeId (type);
}

void 
NistLteHelper::SetEnbAntennaModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_enbAntennaModelFactory.Set (n, v);
}

void
NistLteHelper::SetUeDeviceAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_ueNetDeviceFactory.Set (n, v);
}

void 
NistLteHelper::SetUeAntennaModelType (std::string type)
{
  NS_LOG_FUNCTION (this);
  m_ueAntennaModelFactory.SetTypeId (type);
}

void 
NistLteHelper::SetUeAntennaModelAttribute (std::string n, const AttributeValue &v)
{
  NS_LOG_FUNCTION (this);
  m_ueAntennaModelFactory.Set (n, v);
}

void 
NistLteHelper::SetFadingModel (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_fadingModelType = type;
  if (!type.empty ())
    {
      m_fadingModelFactory = ObjectFactory ();
      m_fadingModelFactory.SetTypeId (type);
    }
}

void 
NistLteHelper::SetFadingModelAttribute (std::string n, const AttributeValue &v)
{
  m_fadingModelFactory.Set (n, v);
}

void 
NistLteHelper::SetSpectrumChannelType (std::string type) 
{
  NS_LOG_FUNCTION (this << type);
  m_channelFactory.SetTypeId (type);
}

void 
NistLteHelper::SetSpectrumChannelAttribute (std::string n, const AttributeValue &v)
{
  m_channelFactory.Set (n, v);
}


NetDeviceContainer
NistLteHelper::InstallEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();  // will run DoInitialize () if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleEnbDevice (node);
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
NistLteHelper::InstallUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleUeDevice (node);
      devices.Add (device);
    }
  return devices;
}


Ptr<NetDevice>
NistLteHelper::InstallSingleEnbDevice (Ptr<Node> n)
{

  NS_ABORT_MSG_IF (m_cellIdCounter == 65535, "max num eNBs exceeded");
  uint16_t cellId = ++m_cellIdCounter;

  Ptr<NistLteSpectrumPhy> dlPhy = CreateObject<NistLteSpectrumPhy> ();
  Ptr<NistLteSpectrumPhy> ulPhy = CreateObject<NistLteSpectrumPhy> ();
  
  Ptr<NistLteEnbPhy> phy = CreateObject<NistLteEnbPhy> (dlPhy, ulPhy, m_EnbEnablePhyLayer);
  
  Ptr<NistLteHarqPhy> harq = Create<NistLteHarqPhy> ();
  dlPhy->SetHarqPhyModule (harq);
  ulPhy->SetHarqPhyModule (harq);
  phy->SetHarqPhyModule (harq);

  Ptr<NistLteChunkProcessor> pCtrl = Create<NistLteChunkProcessor> ();
  pCtrl->AddCallback (MakeCallback (&NistLteEnbPhy::GenerateCtrlCqiReport, phy));
  ulPhy->AddCtrlSinrChunkProcessor (pCtrl); // for evaluating SRS UL-CQI

  Ptr<NistLteChunkProcessor> pData = Create<NistLteChunkProcessor> ();
  pData->AddCallback (MakeCallback (&NistLteEnbPhy::GenerateDataCqiReport, phy));
  pData->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSinrPerceived, ulPhy));
  ulPhy->AddDataSinrChunkProcessor (pData); // for evaluating PUSCH UL-CQI

  Ptr<NistLteChunkProcessor> pInterf = Create<NistLteChunkProcessor> ();
  pInterf->AddCallback (MakeCallback (&NistLteEnbPhy::ReportInterference, phy));
  ulPhy->AddInterferenceDataChunkProcessor (pInterf); // for interference power tracing

  dlPhy->SetChannel (m_downlinkChannel);
  ulPhy->SetChannel (m_uplinkChannel);

  Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
  NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling NistLteHelper::InstallUeDevice ()");
  dlPhy->SetMobility (mm);
  ulPhy->SetMobility (mm);

  Ptr<AntennaModel> antenna = (m_enbAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
  dlPhy->SetAntenna (antenna);
  ulPhy->SetAntenna (antenna);

  Ptr<NistLteEnbMac> mac = CreateObject<NistLteEnbMac> ();
  Ptr<NistFfMacScheduler> sched = m_schedulerFactory.Create<NistFfMacScheduler> ();
  Ptr<NistLteFfrAlgorithm> ffrAlgorithm = m_ffrAlgorithmFactory.Create<NistLteFfrAlgorithm> ();
  Ptr<NistLteHandoverAlgorithm> handoverAlgorithm = m_handoverAlgorithmFactory.Create<NistLteHandoverAlgorithm> ();
  Ptr<NistLteEnbRrc> rrc = CreateObject<NistLteEnbRrc> ();

  if (m_useIdealRrc)
    {
      Ptr<NistLteEnbRrcProtocolIdeal> rrcProtocol = CreateObject<NistLteEnbRrcProtocolIdeal> ();
      rrcProtocol->SetNistLteEnbRrcSapProvider (rrc->GetNistLteEnbRrcSapProvider ());
      rrc->SetNistLteEnbRrcSapUser (rrcProtocol->GetNistLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }
  else
    {
      Ptr<NistLteEnbRrcProtocolReal> rrcProtocol = CreateObject<NistLteEnbRrcProtocolReal> ();
      rrcProtocol->SetNistLteEnbRrcSapProvider (rrc->GetNistLteEnbRrcSapProvider ());
      rrc->SetNistLteEnbRrcSapUser (rrcProtocol->GetNistLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }

  if (m_epcHelper != 0)
    {
      EnumValue epsBearerToRlcMapping;
      rrc->GetAttribute ("NistEpsBearerToRlcMapping", epsBearerToRlcMapping);
      // it does not make sense to use RLC/SM when also using the EPC
      if (epsBearerToRlcMapping.Get () == NistLteEnbRrc::RLC_SM_ALWAYS)
        {
          rrc->SetAttribute ("NistEpsBearerToRlcMapping", EnumValue (NistLteEnbRrc::RLC_UM_ALWAYS));
        }
    }

  rrc->SetNistLteEnbCmacSapProvider (mac->GetNistLteEnbCmacSapProvider ());
  mac->SetNistLteEnbCmacSapUser (rrc->GetNistLteEnbCmacSapUser ());
  rrc->SetNistLteMacSapProvider (mac->GetNistLteMacSapProvider ());

  rrc->SetNistLteHandoverManagementSapProvider (handoverAlgorithm->GetNistLteHandoverManagementSapProvider ());
  handoverAlgorithm->SetNistLteHandoverManagementSapUser (rrc->GetNistLteHandoverManagementSapUser ());

  mac->SetNistFfMacSchedSapProvider (sched->GetNistFfMacSchedSapProvider ());
  mac->SetNistFfMacCschedSapProvider (sched->GetNistFfMacCschedSapProvider ());

  sched->SetNistFfMacSchedSapUser (mac->GetNistFfMacSchedSapUser ());
  sched->SetNistFfMacCschedSapUser (mac->GetNistFfMacCschedSapUser ());

  phy->SetNistLteEnbPhySapUser (mac->GetNistLteEnbPhySapUser ());
  mac->SetNistLteEnbPhySapProvider (phy->GetNistLteEnbPhySapProvider ());

  phy->SetNistLteEnbCphySapUser (rrc->GetNistLteEnbCphySapUser ());
  rrc->SetNistLteEnbCphySapProvider (phy->GetNistLteEnbCphySapProvider ());

  //FFR SAP
  sched->SetNistLteFfrSapProvider (ffrAlgorithm->GetNistLteFfrSapProvider ());
  ffrAlgorithm->SetNistLteFfrSapUser (sched->GetNistLteFfrSapUser ());

  rrc->SetNistLteFfrRrcSapProvider (ffrAlgorithm->GetNistLteFfrRrcSapProvider ());
  ffrAlgorithm->SetNistLteFfrRrcSapUser (rrc->GetNistLteFfrRrcSapUser ());
  //FFR SAP END

  Ptr<NistLteEnbNetDevice> dev = m_enbNetDeviceFactory.Create<NistLteEnbNetDevice> ();
  dev->SetNode (n);
  dev->SetAttribute ("CellId", UintegerValue (cellId)); 
  dev->SetAttribute ("NistLteEnbPhy", PointerValue (phy));
  dev->SetAttribute ("NistLteEnbMac", PointerValue (mac));
  dev->SetAttribute ("NistFfMacScheduler", PointerValue (sched));
  dev->SetAttribute ("NistLteEnbRrc", PointerValue (rrc)); 
  dev->SetAttribute ("NistLteHandoverAlgorithm", PointerValue (handoverAlgorithm));
  dev->SetAttribute ("NistLteFfrAlgorithm", PointerValue (ffrAlgorithm));

  if (m_isAnrEnabled)
    {
      Ptr<NistLteAnr> anr = CreateObject<NistLteAnr> (cellId);
      rrc->SetNistLteAnrSapProvider (anr->GetNistLteAnrSapProvider ());
      anr->SetNistLteAnrSapUser (rrc->GetNistLteAnrSapUser ());
      dev->SetAttribute ("NistLteAnr", PointerValue (anr));
    }

  phy->SetDevice (dev);
  dlPhy->SetDevice (dev);
  ulPhy->SetDevice (dev);

  n->AddDevice (dev);
  ulPhy->SetNistLtePhyRxDataEndOkCallback (MakeCallback (&NistLteEnbPhy::PhyPduReceived, phy));
  ulPhy->SetNistLtePhyRxCtrlEndOkCallback (MakeCallback (&NistLteEnbPhy::ReceiveNistLteControlMessageList, phy));
  ulPhy->SetNistLtePhyUlHarqFeedbackCallback (MakeCallback (&NistLteEnbPhy::ReceiveLteUlHarqFeedback, phy));
  rrc->SetForwardUpCallback (MakeCallback (&NistLteEnbNetDevice::Receive, dev));

  NS_LOG_LOGIC ("set the propagation model frequencies");
  double dlFreq = NistLteSpectrumValueHelper::GetCarrierFrequency (dev->GetDlEarfcn ());
  NS_LOG_LOGIC ("DL freq: " << dlFreq);
  bool dlFreqOk = m_downlinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (dlFreq));
  if (!dlFreqOk)
    {
      NS_LOG_WARN ("DL propagation model does not have a Frequency attribute");
    }
  double ulFreq = NistLteSpectrumValueHelper::GetCarrierFrequency (dev->GetUlEarfcn ());
  NS_LOG_LOGIC ("UL freq: " << ulFreq);
  bool ulFreqOk = m_uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

  dev->Initialize ();

  m_uplinkChannel->AddRx (ulPhy);

  if (m_epcHelper != 0)
    {
      NS_LOG_INFO ("adding this eNB to the EPC");
      m_epcHelper->AddEnb (n, dev, dev->GetCellId ());
      Ptr<NistEpcEnbApplication> enbApp = n->GetApplication (0)->GetObject<NistEpcEnbApplication> ();
      NS_ASSERT_MSG (enbApp != 0, "cannot retrieve NistEpcEnbApplication");

      // S1 SAPs
      rrc->SetS1SapProvider (enbApp->GetS1SapProvider ());
      enbApp->SetS1SapUser (rrc->GetS1SapUser ());

      // X2 SAPs
      Ptr<NistEpcX2> x2 = n->GetObject<NistEpcX2> ();
      x2->SetNistEpcX2SapUser (rrc->GetNistEpcX2SapUser ());
      rrc->SetNistEpcX2SapProvider (x2->GetNistEpcX2SapProvider ());
    }

  return dev;
}

Ptr<NetDevice>
NistLteHelper::InstallSingleUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);
  Ptr<NistLteSpectrumPhy> dlPhy = CreateObject<NistLteSpectrumPhy> ();
  Ptr<NistLteSpectrumPhy> ulPhy = CreateObject<NistLteSpectrumPhy> ();
  Ptr<NistLteSpectrumPhy> slPhy;
  if (m_useSidelink || m_useDiscovery) {
    slPhy = CreateObject<NistLteSpectrumPhy> ();
    slPhy->SetAttribute ("HalfDuplexPhy", PointerValue (ulPhy));
  }

  Ptr<NistLteUePhy> phy = CreateObject<NistLteUePhy> (dlPhy, ulPhy);
  if (m_useSidelink || m_useDiscovery) {
    phy->SetSlSpectrumPhy (slPhy);
  }

  Ptr<NistLteHarqPhy> harq = Create<NistLteHarqPhy> ();
  dlPhy->SetHarqPhyModule (harq);
  ulPhy->SetHarqPhyModule (harq);
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetHarqPhyModule (harq);
  }
  phy->SetHarqPhyModule (harq);

  Ptr<NistLteChunkProcessor> pRs = Create<NistLteChunkProcessor> ();
  pRs->AddCallback (MakeCallback (&NistLteUePhy::ReportRsReceivedPower, phy));
  dlPhy->AddRsPowerChunkProcessor (pRs);

  Ptr<NistLteChunkProcessor> pInterf = Create<NistLteChunkProcessor> ();
  pInterf->AddCallback (MakeCallback (&NistLteUePhy::ReportInterference, phy));
  dlPhy->AddInterferenceCtrlChunkProcessor (pInterf); // for RSRQ evaluation of UE Measurements

  Ptr<NistLteChunkProcessor> pCtrl = Create<NistLteChunkProcessor> ();
  pCtrl->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSinrPerceived, dlPhy));
  dlPhy->AddCtrlSinrChunkProcessor (pCtrl);

  Ptr<NistLteChunkProcessor> pData = Create<NistLteChunkProcessor> ();
  pData->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSinrPerceived, dlPhy));
  dlPhy->AddDataSinrChunkProcessor (pData);

  if (m_useSidelink || m_useDiscovery) {
    Ptr<NistLteSlChunkProcessor> pSlSinr = Create<NistLteSlChunkProcessor> ();
    pSlSinr->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSlSinrPerceived, slPhy));
    slPhy->AddSlSinrChunkProcessor (pSlSinr);
    
    Ptr<NistLteSlChunkProcessor> pSlSignal = Create<NistLteSlChunkProcessor> ();
    pSlSignal->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSlSigPerceived, slPhy));
    slPhy->AddSlSignalChunkProcessor (pSlSignal);

    Ptr<NistLteSlChunkProcessor> pSlInterference = Create<NistLteSlChunkProcessor> ();
    pSlInterference->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSlIntPerceived, slPhy));
    slPhy->AddSlInterferenceChunkProcessor (pSlInterference);
    /*
    Ptr<NistLteChunkProcessor> pData = Create<NistLteChunkProcessor> ();
    pData->AddCallback (MakeCallback (&NistLteSpectrumPhy::UpdateSinrPerceived, slPhy));
    slPhy->AddDataSinrChunkProcessor (pData);
    */
  }
  
  if (m_usePdschForCqiGeneration)
    {
      // CQI calculation based on PDCCH for signal and PDSCH for interference
      pCtrl->AddCallback (MakeCallback (&NistLteUePhy::GenerateMixedCqiReport, phy));
      Ptr<NistLteChunkProcessor> pDataInterf = Create<NistLteChunkProcessor> ();      
      pDataInterf->AddCallback (MakeCallback (&NistLteUePhy::ReportDataInterference, phy));
      dlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
    }
  else
    {
      // CQI calculation based on PDCCH for both signal and interference
      pCtrl->AddCallback (MakeCallback (&NistLteUePhy::GenerateCtrlCqiReport, phy));
    }



  dlPhy->SetChannel (m_downlinkChannel);
  ulPhy->SetChannel (m_uplinkChannel);
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetChannel (m_uplinkChannel); //want the UE to receive sidelink messages on the uplink
  }


  Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
  NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling NistLteHelper::InstallUeDevice ()");
  dlPhy->SetMobility (mm);
  ulPhy->SetMobility (mm);
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetMobility (mm);
  }

  Ptr<AntennaModel> antenna = (m_ueAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
  dlPhy->SetAntenna (antenna);
  ulPhy->SetAntenna (antenna);
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetAntenna (antenna);
  }

  // create a UE mac with an attribute to indicate the UE scheduler
  Ptr<NistLteUeMac> mac = CreateObjectWithAttributes<NistLteUeMac> (
                          "NistUlScheduler",
                           StringValue (NistLteHelper::GetUlSchedulerType ()));
  Ptr<NistLteUeRrc> rrc = CreateObject<NistLteUeRrc> ();

  if (m_useIdealRrc)
    {
      Ptr<NistLteUeRrcProtocolIdeal> rrcProtocol = CreateObject<NistLteUeRrcProtocolIdeal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetNistLteUeRrcSapProvider (rrc->GetNistLteUeRrcSapProvider ());
      rrc->SetNistLteUeRrcSapUser (rrcProtocol->GetNistLteUeRrcSapUser ());
    }
  else
    {
      Ptr<NistLteUeRrcProtocolReal> rrcProtocol = CreateObject<NistLteUeRrcProtocolReal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetNistLteUeRrcSapProvider (rrc->GetNistLteUeRrcSapProvider ());
      rrc->SetNistLteUeRrcSapUser (rrcProtocol->GetNistLteUeRrcSapUser ());
    }
  
  if (m_epcHelper != 0)
    {
      rrc->SetUseRlcSm (false);
    }
  Ptr<NistEpcUeNas> nas = CreateObject<NistEpcUeNas> ();
 
  nas->SetAsSapProvider (rrc->GetAsSapProvider ());
  rrc->SetAsSapUser (nas->GetAsSapUser ());

  rrc->SetNistLteUeCmacSapProvider (mac->GetNistLteUeCmacSapProvider ());
  mac->SetNistLteUeCmacSapUser (rrc->GetNistLteUeCmacSapUser ());
  rrc->SetNistLteMacSapProvider (mac->GetNistLteMacSapProvider ());

  phy->SetNistLteUePhySapUser (mac->GetNistLteUePhySapUser ());
  mac->SetNistLteUePhySapProvider (phy->GetNistLteUePhySapProvider ());

  phy->SetNistLteUeCphySapUser (rrc->GetNistLteUeCphySapUser ());
  rrc->SetNistLteUeCphySapProvider (phy->GetNistLteUeCphySapProvider ());
  mac->SetNistLteUeCphySapProvider (phy->GetNistLteUeCphySapProvider ());
  
  NS_ABORT_MSG_IF (m_imsiCounter >= 0xFFFFFFFF, "max num UEs exceeded");
  uint64_t imsi = ++m_imsiCounter;

  //Initialize sidelink configuration
  Ptr<LteUeRrcSl> ueSidelinkConfiguration = CreateObject<LteUeRrcSl> ();
  ueSidelinkConfiguration->SetSourceL2Id ((uint32_t) (imsi & 0xFFFFFF)); //use lower 24 bits of IMSI as source
  rrc->SetAttribute ("SidelinkConfiguration", PointerValue (ueSidelinkConfiguration));
  
  Ptr<NistLteUeNetDevice> dev = m_ueNetDeviceFactory.Create<NistLteUeNetDevice> ();
  dev->SetNode (n);
  dev->SetAttribute ("Imsi", UintegerValue (imsi));
  dev->SetAttribute ("NistLteUePhy", PointerValue (phy));
  dev->SetAttribute ("NistLteUeMac", PointerValue (mac));
  dev->SetAttribute ("NistLteUeRrc", PointerValue (rrc));
  dev->SetAttribute ("NistEpcUeNas", PointerValue (nas));

  phy->SetDevice (dev);
  dlPhy->SetDevice (dev);
  ulPhy->SetDevice (dev);
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetDevice (dev);
  }
  nas->SetDevice (dev);

  n->AddDevice (dev);
  dlPhy->SetNistLtePhyRxDataEndOkCallback (MakeCallback (&NistLteUePhy::PhyPduReceived, phy));
  dlPhy->SetNistLtePhyRxCtrlEndOkCallback (MakeCallback (&NistLteUePhy::ReceiveNistLteControlMessageList, phy));
  dlPhy->SetNistLtePhyRxPssCallback (MakeCallback (&NistLteUePhy::ReceivePss, phy));
  dlPhy->SetNistLtePhyDlHarqFeedbackCallback (MakeCallback (&NistLteUePhy::ReceiveLteDlHarqFeedback, phy));
  nas->SetForwardUpCallback (MakeCallback (&NistLteUeNetDevice::Receive, dev));
  if (m_useSidelink || m_useDiscovery) {
    slPhy->SetNistLtePhyRxDataEndOkCallback (MakeCallback (&NistLteUePhy::PhyPduReceived, phy));
    slPhy->SetNistLtePhyRxCtrlEndOkCallback (MakeCallback (&NistLteUePhy::ReceiveNistLteControlMessageList, phy));
    slPhy->SetNistLtePhyRxSlssCallback (MakeCallback (&NistLteUePhy::ReceiveSlss, phy));
  }
  
  if (m_epcHelper != 0)
    {
      m_epcHelper->AddUe (dev, dev->GetImsi ());
    }

  dev->Initialize ();

  return dev;
}


void
NistLteHelper::Attach (NetDeviceContainer ueDevices)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      Attach (*i);
    }
}

void
NistLteHelper::Attach (Ptr<NetDevice> ueDevice)
{
  NS_LOG_FUNCTION (this);

  if (m_epcHelper == 0)
    {
      NS_FATAL_ERROR ("This function is not valid without properly configured EPC");
    }

  Ptr<NistLteUeNetDevice> ueLteDevice = ueDevice->GetObject<NistLteUeNetDevice> ();
  if (ueLteDevice == 0)
    {
      NS_FATAL_ERROR ("The passed NetDevice must be an NistLteUeNetDevice");
    }

  // initiate cell selection
  Ptr<NistEpcUeNas> ueNas = ueLteDevice->GetNas ();
  NS_ASSERT (ueNas != 0);
  uint16_t dlEarfcn = ueLteDevice->GetDlEarfcn ();
  ueNas->StartCellSelection (dlEarfcn);

  // instruct UE to immediately enter CONNECTED mode after camping
  ueNas->Connect ();

  // activate default EPS bearer
  m_epcHelper->ActivateNistEpsBearer (ueDevice, ueLteDevice->GetImsi (),
                                  NistEpcTft::Default (),
                                  NistEpsBearer (NistEpsBearer::NGBR_VIDEO_TCP_DEFAULT));
}

void
NistLteHelper::Attach (NetDeviceContainer ueDevices, Ptr<NetDevice> enbDevice)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      Attach (*i, enbDevice);
    }
}

void
NistLteHelper::Attach (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice)
{
  NS_LOG_FUNCTION (this);
  //enbRrc->SetCellId (enbDevice->GetObject<NistLteEnbNetDevice> ()->GetCellId ());

  Ptr<NistLteUeNetDevice> ueLteDevice = ueDevice->GetObject<NistLteUeNetDevice> ();
  Ptr<NistLteEnbNetDevice> enbLteDevice = enbDevice->GetObject<NistLteEnbNetDevice> ();

  Ptr<NistEpcUeNas> ueNas = ueLteDevice->GetNas ();
  ueNas->Connect (enbLteDevice->GetCellId (), enbLteDevice->GetDlEarfcn ());

  if (m_epcHelper != 0)
    {
      // activate default EPS bearer
      m_epcHelper->ActivateNistEpsBearer (ueDevice, ueLteDevice->GetImsi (), NistEpcTft::Default (), NistEpsBearer (NistEpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

  // tricks needed for the simplified LTE-only simulations 
  if (m_epcHelper == 0)
    {
      ueDevice->GetObject<NistLteUeNetDevice> ()->SetTargetEnb (enbDevice->GetObject<NistLteEnbNetDevice> ());
    }
}

void
NistLteHelper::AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      AttachToClosestEnb (*i, enbDevices);
    }
}

void
NistLteHelper::AttachToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (enbDevices.GetN () > 0, "empty enb device container");
  Vector uepos = ueDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  double minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> closestEnbDevice;
  for (NetDeviceContainer::Iterator i = enbDevices.Begin (); i != enbDevices.End (); ++i)
    {
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          closestEnbDevice = *i;
        }
    }
  NS_ASSERT (closestEnbDevice != 0);
  Attach (ueDevice, closestEnbDevice);
}

uint8_t
NistLteHelper::ActivateDedicatedNistEpsBearer (NetDeviceContainer ueDevices, NistEpsBearer bearer, Ptr<NistEpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      uint8_t bearerId = ActivateDedicatedNistEpsBearer (*i, bearer, tft);
      return bearerId;
    }
  return 0;
}


uint8_t
NistLteHelper::ActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice, NistEpsBearer bearer, Ptr<NistEpcTft> tft)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "dedicated EPS bearers cannot be set up when the EPC is not used");

  uint64_t imsi = ueDevice->GetObject<NistLteUeNetDevice> ()->GetImsi ();
  uint8_t bearerId = m_epcHelper->ActivateNistEpsBearer (ueDevice, imsi, tft, bearer);
  return bearerId;
}

void
NistLteHelper::ActivateSidelinkBearer (NetDeviceContainer ueDevices, Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      ActivateSidelinkBearer (*i, tft->Copy());
    }
}


void
NistLteHelper::ActivateSidelinkBearer (Ptr<NetDevice> ueDevice, Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "sidelink bearers cannot be set up when the EPC is not used");

  m_epcHelper->ActivateSidelinkBearer (ueDevice, tft);
}  

void
NistLteHelper::DeactivateSidelinkBearer (NetDeviceContainer ueDevices, Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      DeactivateSidelinkBearer (*i, tft);
    }
}


void
NistLteHelper::DeactivateSidelinkBearer (Ptr<NetDevice> ueDevice, Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "sidelink bearers cannot be set up when the EPC is not used");

  m_epcHelper->DeactivateSidelinkBearer (ueDevice, tft);
}  
void
NistLteHelper::StartDiscovery (NetDeviceContainer ueDevices, std::list<uint32_t> apps, bool rxtx)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      StartDiscovery (*i, apps, rxtx);
    }
}

void
NistLteHelper::StartDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> apps, bool rxtx)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "discovery can't start when the EPC is not used");

  m_epcHelper->StartDiscovery (ueDevice, apps, rxtx);
}  

void
NistLteHelper::StopDiscovery (NetDeviceContainer ueDevices, std::list<uint32_t> apps, bool rxtx)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      StopDiscovery (*i, apps, rxtx);
    }
}

void
NistLteHelper::StopDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> apps, bool rxtx)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "no EPC is used");

  m_epcHelper->StopDiscovery (ueDevice, apps, rxtx);
}


/**
 * \ingroup lte
 *
 * DrbActivatior allows user to activate bearers for UEs
 * when EPC is not used. Activation function is hooked to
 * the Enb RRC Connection Estabilished trace source. When
 * UE change its RRC state to CONNECTED_NORMALLY, activation
 * function is called and bearer is activated.
*/
class NistDrbActivator : public SimpleRefCount<NistDrbActivator>
{
public:
  /**
  * NistDrbActivator Constructor
  *
  * \param ueDevice the UeNetDevice for which bearer will be activated
  * \param bearer the bearer configuration
  */
  NistDrbActivator (Ptr<NetDevice> ueDevice, NistEpsBearer bearer);

  /**
   * Function hooked to the Enb RRC Connection Established trace source
   * Fired upon successful RRC connection establishment.
   *
   * \param a NistDrbActivator object
   * \param context
   * \param imsi
   * \param cellId
   * \param rnti
   */
  static void ActivateCallback (Ptr<NistDrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Procedure firstly checks if bearer was not activated, if IMSI
   * from trace source equals configured one and if UE is really
   * in RRC connected state. If all requirements are met, it performs
   * bearer activation.
   *
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti);
private:
  /**
   * Bearer can be activated only once. This value stores state of
   * bearer. Initially is set to false and changed to true during
   * bearer activation.
   */
  bool m_active;
  /**
   * UeNetDevice for which bearer will be activated
   */
  Ptr<NetDevice> m_ueDevice;
  /**
   * Configuration of bearer which will be activated
   */
  NistEpsBearer m_bearer;
  /**
   * imsi the unique UE identifier
   */
  uint64_t m_imsi;
};

NistDrbActivator::NistDrbActivator (Ptr<NetDevice> ueDevice, NistEpsBearer bearer)
  : m_active (false),
    m_ueDevice (ueDevice),
    m_bearer (bearer),
    m_imsi (m_ueDevice->GetObject<NistLteUeNetDevice> ()->GetImsi ())
{
}

void
NistDrbActivator::ActivateCallback (Ptr<NistDrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (a << context << imsi << cellId << rnti);
  a->ActivateDrb (imsi, cellId, rnti);
}

void
NistDrbActivator::ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti)
{ 
  NS_LOG_FUNCTION (this << imsi << cellId << rnti << m_active);
  if ((!m_active) && (imsi == m_imsi))
    {
      Ptr<NistLteUeRrc> ueRrc = m_ueDevice->GetObject<NistLteUeNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetState () == NistLteUeRrc::CONNECTED_NORMALLY);
      uint16_t rnti = ueRrc->GetRnti ();
      Ptr<NistLteEnbNetDevice> enbLteDevice = m_ueDevice->GetObject<NistLteUeNetDevice> ()->GetTargetEnb ();
      Ptr<NistLteEnbRrc> enbRrc = enbLteDevice->GetObject<NistLteEnbNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetCellId () == enbLteDevice->GetCellId ());
      Ptr<NistUeManager> ueManager = enbRrc->GetNistUeManager (rnti);
      NS_ASSERT (ueManager->GetState () == NistUeManager::CONNECTED_NORMALLY
                 || ueManager->GetState () == NistUeManager::CONNECTION_RECONFIGURATION);
      NistEpcEnbS1SapUser::NistDataRadioBearerSetupRequestParameters params;
      params.rnti = rnti;
      params.bearer = m_bearer;
      params.bearerId = 0;
      params.gtpTeid = 0; // don't care
      enbRrc->GetS1SapUser ()->DataRadioBearerSetupRequest (params);
      m_active = true;
    }
}


void 
NistLteHelper::ActivateDataRadioBearer (Ptr<NetDevice> ueDevice, NistEpsBearer bearer)
{
  NS_LOG_FUNCTION (this << ueDevice);
  NS_ASSERT_MSG (m_epcHelper == 0, "this method must not be used when the EPC is being used");

  // Normally it is the EPC that takes care of activating DRBs
  // when the UE gets connected. When the EPC is not used, we achieve
  // the same behavior by hooking a dedicated DRB activation function
  // to the Enb RRC Connection Established trace source


  Ptr<NistLteEnbNetDevice> enbLteDevice = ueDevice->GetObject<NistLteUeNetDevice> ()->GetTargetEnb ();

  std::ostringstream path;
  path << "/NodeList/" << enbLteDevice->GetNode ()->GetId () 
       << "/DeviceList/" << enbLteDevice->GetIfIndex ()
       << "/NistLteEnbRrc/ConnectionEstablished";
  Ptr<NistDrbActivator> arg = Create<NistDrbActivator> (ueDevice, bearer);
  Config::Connect (path.str (), MakeBoundCallback (&NistDrbActivator::ActivateCallback, arg));
}

void
NistLteHelper::AddX2Interface (NodeContainer enbNodes)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "X2 interfaces cannot be set up when the EPC is not used");

  for (NodeContainer::Iterator i = enbNodes.Begin (); i != enbNodes.End (); ++i)
    {
      for (NodeContainer::Iterator j = i + 1; j != enbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
        }
    }
}

void
NistLteHelper::AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("setting up the X2 interface");

  m_epcHelper->AddX2Interface (enbNode1, enbNode2);
}

void
NistLteHelper::HandoverRequest (Time hoTime, Ptr<NetDevice> ueDev, Ptr<NetDevice> sourceEnbDev, Ptr<NetDevice> targetEnbDev)
{
  NS_LOG_FUNCTION (this << ueDev << sourceEnbDev << targetEnbDev);
  NS_ASSERT_MSG (m_epcHelper, "Handover requires the use of the EPC - did you forget to call NistLteHelper::SetNistEpcHelper () ?");
  Simulator::Schedule (hoTime, &NistLteHelper::DoHandoverRequest, this, ueDev, sourceEnbDev, targetEnbDev);
}

void
NistLteHelper::DoHandoverRequest (Ptr<NetDevice> ueDev, Ptr<NetDevice> sourceEnbDev, Ptr<NetDevice> targetEnbDev)
{
  NS_LOG_FUNCTION (this << ueDev << sourceEnbDev << targetEnbDev);

  uint16_t targetCellId = targetEnbDev->GetObject<NistLteEnbNetDevice> ()->GetCellId ();
  Ptr<NistLteEnbRrc> sourceRrc = sourceEnbDev->GetObject<NistLteEnbNetDevice> ()->GetRrc ();
  uint16_t rnti = ueDev->GetObject<NistLteUeNetDevice> ()->GetRrc ()->GetRnti ();
  sourceRrc->SendHandoverRequest (rnti, targetCellId);
}

void
NistLteHelper::DeActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice,Ptr<NetDevice> enbDevice, uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << ueDevice << bearerId);
  NS_ASSERT_MSG (m_epcHelper != 0, "Dedicated EPS bearers cannot be de-activated when the EPC is not used");
  NS_ASSERT_MSG (bearerId != 1, "Default bearer cannot be de-activated until and unless and UE is released");

  DoDeActivateDedicatedNistEpsBearer (ueDevice, enbDevice, bearerId);
}

void
NistLteHelper::DoDeActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice, uint8_t bearerId)
{
  NS_LOG_FUNCTION (this << ueDevice << bearerId);

  //Extract IMSI and rnti
  uint64_t imsi = ueDevice->GetObject<NistLteUeNetDevice> ()->GetImsi ();
  uint16_t rnti = ueDevice->GetObject<NistLteUeNetDevice> ()->GetRrc ()->GetRnti ();


  Ptr<NistLteEnbRrc> enbRrc = enbDevice->GetObject<NistLteEnbNetDevice> ()->GetRrc ();

  enbRrc->DoSendReleaseDataRadioBearer (imsi,rnti,bearerId);
}


void 
NistLteHelper::ActivateDataRadioBearer (NetDeviceContainer ueDevices, NistEpsBearer bearer)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      ActivateDataRadioBearer (*i, bearer);
    }
}

void
NistLteHelper::EnableLogComponents (void)
{
  LogComponentEnable ("NistLteHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteEnbRrc", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteUeRrc", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteEnbMac", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteUeMac", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteRlc", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteRlcUm", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteRlcAm", LOG_LEVEL_ALL);
  LogComponentEnable ("RrNistFfMacScheduler", LOG_LEVEL_ALL);
  LogComponentEnable ("NistPfFfMacScheduler", LOG_LEVEL_ALL);

  LogComponentEnable ("NistLtePhy", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteEnbPhy", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteUePhy", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteSpectrumValueHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteSpectrumPhy", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteInterference", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteChunkProcessor", LOG_LEVEL_ALL);

  std::string propModelStr = m_dlPathlossModelFactory.GetTypeId ().GetName ().erase (0,5).c_str ();
  LogComponentEnable ("NistLteNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteUeNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteEnbNetDevice", LOG_LEVEL_ALL);

  LogComponentEnable ("NistRadioBearerStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("NistLteStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("NistMacStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("NistPhyTxStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("NistPhyRxStatsCalculator", LOG_LEVEL_ALL);
  LogComponentEnable ("NistPhyStatsCalculator", LOG_LEVEL_ALL);


}

void
NistLteHelper::EnableTraces (void)
{
  EnablePhyTraces ();
  EnableMacTraces ();
  EnableRlcTraces ();
  EnablePdcpTraces ();
}

void
NistLteHelper::EnableRlcTraces (void)
{
  NS_ASSERT_MSG (m_rlcStats == 0, "please make sure that NistLteHelper::EnableRlcTraces is called at most once");
  m_rlcStats = CreateObject<NistRadioBearerStatsCalculator> ("RLC");
  m_radioBearerStatsConnector.EnableRlcStats (m_rlcStats);
}

int64_t
NistLteHelper::AssignStreams (NetDeviceContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  if ((m_fadingModule != 0) && (m_fadingStreamsAssigned == false))
    {
      Ptr<NistTraceFadingLossModel> tflm = m_fadingModule->GetObject<NistTraceFadingLossModel> ();
      if (tflm != 0)
        {
          currentStream += tflm->AssignStreams (currentStream);
          m_fadingStreamsAssigned = true;
        }
    }
  Ptr<NetDevice> netDevice;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      netDevice = (*i);
      Ptr<NistLteEnbNetDevice> lteEnb = DynamicCast<NistLteEnbNetDevice> (netDevice);
      if (lteEnb)
        {
          Ptr<NistLteSpectrumPhy> dlPhy = lteEnb->GetPhy ()->GetDownlinkSpectrumPhy ();
          Ptr<NistLteSpectrumPhy> ulPhy = lteEnb->GetPhy ()->GetUplinkSpectrumPhy ();
          currentStream += dlPhy->AssignStreams (currentStream);
          currentStream += ulPhy->AssignStreams (currentStream);
        }
      Ptr<NistLteUeNetDevice> lteUe = DynamicCast<NistLteUeNetDevice> (netDevice);
      if (lteUe)
        {
          Ptr<NistLteSpectrumPhy> dlPhy = lteUe->GetPhy ()->GetDownlinkSpectrumPhy ();
          Ptr<NistLteSpectrumPhy> ulPhy = lteUe->GetPhy ()->GetUplinkSpectrumPhy ();
          Ptr<NistLteUeMac> ueMac = lteUe->GetMac ();
          currentStream += dlPhy->AssignStreams (currentStream);
          currentStream += ulPhy->AssignStreams (currentStream);
          currentStream += ueMac->AssignStreams (currentStream);
        }
    }
  return (currentStream - stream);
}  

void
NistLteHelper::EnablePhyTraces (void)
{
  EnableDlPhyTraces ();
  EnableUlPhyTraces ();
  EnableSlPhyTraces ();
  EnableDlTxPhyTraces ();
  EnableUlTxPhyTraces ();
  EnableSlTxPhyTraces ();
  EnableDlRxPhyTraces ();
  EnableUlRxPhyTraces ();
  EnableSlRxPhyTraces ();
  EnableSlPscchRxPhyTraces ();
}

void
NistLteHelper::EnableDlTxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbPhy/DlPhyTransmission",
                   MakeBoundCallback (&NistPhyTxStatsCalculator::DlPhyTransmissionCallback, m_phyTxStats));
}

void
NistLteHelper::EnableUlTxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/UlPhyTransmission",
                   MakeBoundCallback (&NistPhyTxStatsCalculator::UlPhyTransmissionCallback, m_phyTxStats));
}

void
NistLteHelper::EnableSlTxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/SlPhyTransmission",
                   MakeBoundCallback (&NistPhyTxStatsCalculator::SlPhyTransmissionCallback, m_phyTxStats));
}

void
NistLteHelper::EnableDlRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/DlSpectrumPhy/DlPhyReception",
                   MakeBoundCallback (&NistPhyRxStatsCalculator::DlPhyReceptionCallback, m_phyRxStats));
}

void
NistLteHelper::EnableUlRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbPhy/UlSpectrumPhy/UlPhyReception",
                   MakeBoundCallback (&NistPhyRxStatsCalculator::UlPhyReceptionCallback, m_phyRxStats));
}

void
NistLteHelper::EnableSlRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/SlSpectrumPhy/SlPhyReception",
                   MakeBoundCallback (&NistPhyRxStatsCalculator::SlPhyReceptionCallback, m_phyRxStats));
}

void
NistLteHelper::EnableSlPscchRxPhyTraces (void)
{
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/SlSpectrumPhy/SlPscchReception",
                   MakeBoundCallback (&NistPhyRxStatsCalculator::SlPscchReceptionCallback, m_phyRxStats));
}

void
NistLteHelper::EnableMacTraces (void)
{
  EnableDlMacTraces ();
  EnableUlMacTraces ();
  EnableSlUeMacTraces ();
  EnableSlSchUeMacTraces ();
}


void
NistLteHelper::EnableDlMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbMac/DlScheduling",
                   MakeBoundCallback (&NistMacStatsCalculator::DlSchedulingCallback, m_macStats));
}

void
NistLteHelper::EnableUlMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbMac/UlScheduling",
                   MakeBoundCallback (&NistMacStatsCalculator::UlSchedulingCallback, m_macStats));
}

void
NistLteHelper::EnableSlUeMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeMac/SlUeScheduling",
                   MakeBoundCallback (&NistMacStatsCalculator::SlUeSchedulingCallback, m_macStats));
}

void
NistLteHelper::EnableSlSchUeMacTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeMac/SlSharedChUeScheduling",
                   MakeBoundCallback (&NistMacStatsCalculator::SlSharedChUeSchedulingCallback, m_macStats));
}

void
NistLteHelper::EnableDlPhyTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteUePhy/ReportCurrentCellRsrpSinr",
                   MakeBoundCallback (&NistPhyStatsCalculator::ReportCurrentCellRsrpSinrCallback, m_phyStats));
}

void
NistLteHelper::EnableUlPhyTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbPhy/ReportUeSinr",
                   MakeBoundCallback (&NistPhyStatsCalculator::ReportUeSinr, m_phyStats));
  Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbPhy/ReportInterference",
                   MakeBoundCallback (&NistPhyStatsCalculator::ReportInterference, m_phyStats));

}

void
NistLteHelper::EnableSlPhyTraces (void)
{
  //TBD
}

Ptr<NistRadioBearerStatsCalculator>
NistLteHelper::GetRlcStats (void)
{
  return m_rlcStats;
}

void
NistLteHelper::EnablePdcpTraces (void)
{
  NS_ASSERT_MSG (m_pdcpStats == 0, "please make sure that NistLteHelper::EnablePdcpTraces is called at most once");
  m_pdcpStats = CreateObject<NistRadioBearerStatsCalculator> ("PDCP");
  m_radioBearerStatsConnector.EnablePdcpStats (m_pdcpStats);
}

Ptr<NistRadioBearerStatsCalculator>
NistLteHelper::GetPdcpStats (void)
{
  return m_pdcpStats;
}

  /**
   * Deploys the Sidelink configuration to the eNodeB
   * \param enbDevices List of devices where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
void
NistLteHelper::InstallSidelinkConfiguration (NetDeviceContainer enbDevices, Ptr<LteEnbRrcSl> slConfiguration)
{
  //for each device, install a copy of the configuration
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = enbDevices.Begin (); i != enbDevices.End (); ++i)
    {
      InstallSidelinkConfiguration (*i, slConfiguration->Copy());
    }
}

  /**
   * Deploys the Sidelink configuration to the eNodeB
   * \param enbDevice List of devices where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
void
NistLteHelper::InstallSidelinkConfiguration (Ptr<NetDevice> enbDevice, Ptr<LteEnbRrcSl> slConfiguration)
{
  Ptr<NistLteEnbRrc> rrc = enbDevice->GetObject<NistLteEnbNetDevice> ()->GetRrc();
  NS_ASSERT_MSG (rrc != 0, "RRC layer not found");
  rrc->SetAttribute ("SidelinkConfiguration", PointerValue (slConfiguration));
}

  /**
   * Deploys the Sidelink configuration to the UEs
   * \param ueDevices List of devices where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
void
NistLteHelper::InstallSidelinkConfiguration (NetDeviceContainer ueDevices, Ptr<LteUeRrcSl> slConfiguration)
{
  //for each device, install a copy of the configuration
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      InstallSidelinkConfiguration (*i, slConfiguration);
    }
}

  /**
   * Deploys the Sidelink configuration to the UE
   * \param ueDevice The UE to configure
   * \param slConfiguration Sidelink configuration
   */
void
NistLteHelper::InstallSidelinkConfiguration (Ptr<NetDevice> ueDevice, Ptr<LteUeRrcSl> slConfiguration)
{
  Ptr<NistLteUeRrc> rrc = ueDevice->GetObject<NistLteUeNetDevice> ()->GetRrc();
  NS_ASSERT_MSG (rrc != 0, "RRC layer not found");
  PointerValue ptr;
  rrc->GetAttribute ("SidelinkConfiguration", ptr);
  Ptr<LteUeRrcSl> ueConfig = ptr.Get<LteUeRrcSl> ();
  ueConfig->SetSlPreconfiguration (slConfiguration->GetSlPreconfiguration());
  ueConfig->SetSlEnabled (slConfiguration->IsSlEnabled());
  ueConfig->SetDiscEnabled (slConfiguration->IsDiscEnabled());
  ueConfig->SetDiscTxResources (slConfiguration->GetDiscTxResources ());
  ueConfig->SetDiscInterFreq (slConfiguration->GetDiscInterFreq ());
}

/**
 * Compute the RSRP between the given nodes for the given propagation loss model
 * This code is derived from the multi-model-spectrum-channel class
 * \param propagationLoss The loss model
 * \param psd The power spectral density of the transmitter
 * \param txPhy The transmitter
 * \param rxPhy The receiver
 * \return The RSRP 
 */
double
NistLteHelper::DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, Ptr<SpectrumValue> psd, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{
  Ptr<MobilityModel> txMobility = txPhy->GetMobility ();
  Ptr<MobilityModel> rxMobility = rxPhy->GetMobility ();

  double pathLossDb = 0;
  if (txPhy->GetRxAntenna() != 0)
    {
      Angles txAngles (rxMobility->GetPosition (), txMobility->GetPosition ());
      double txAntennaGain = txPhy->GetRxAntenna()->GetGainDb (txAngles);
      NS_LOG_DEBUG ("txAntennaGain = " << txAntennaGain << " dB");
      pathLossDb -= txAntennaGain;
    }
  Ptr<AntennaModel> rxAntenna = rxPhy->GetRxAntenna ();
  if (rxAntenna != 0)
    {
      Angles rxAngles (txMobility->GetPosition (), rxMobility->GetPosition ());
      double rxAntennaGain = rxAntenna->GetGainDb (rxAngles);
      NS_LOG_DEBUG ("rxAntennaGain = " << rxAntennaGain << " dB");
      pathLossDb -= rxAntennaGain;
    }
  if (propagationLoss)
    {
      double propagationGainDb = propagationLoss->CalcRxPower (0, txMobility, rxMobility);
      NS_LOG_DEBUG ("propagationGainDb = " << propagationGainDb << " dB");
      pathLossDb -= propagationGainDb;
    }                    
  NS_LOG_DEBUG ("total pathLoss = " << pathLossDb << " dB");  

  double pathGainLinear = std::pow (10.0, (-pathLossDb) / 10.0);
  Ptr<SpectrumValue> rxPsd = Copy<SpectrumValue> (psd);
  *rxPsd *= pathGainLinear;

  // RSRP evaluated as averaged received power among RBs
  double sum = 0.0;
  uint8_t rbNum = 0;
  Values::const_iterator it;
  for (it = (*rxPsd).ValuesBegin (); it != (*rxPsd).ValuesEnd (); it++)
    {
      //The non active RB will be set to -inf
      //We count only the active
      if((*it))
        {          
          // convert PSD [W/Hz] to linear power [W] for the single RE
          // we consider only one RE for the RS since the channel is 
          // flat within the same RB 
          double powerTxW = ((*it) * 180000.0) / 12.0;
          sum += powerTxW;
          rbNum++;
        }
    }
  double rsrp = (rbNum > 0) ? (sum / rbNum) : DBL_MAX;

  NS_LOG_INFO ("RSRP linear=" << rsrp << " (" << 10 * std::log10 (rsrp) + 30 << "dBm)");

  return 10 * std::log10 (rsrp) + 30;
}

/**
 * Compute the RSRP between the given nodes for the given propagation loss model
 * This code is derived from the multi-model-spectrum-channel class
 * \param propagationLoss The loss model
 * \param txPower The transmit power
 * \param txPhy The transmitter
 * \param rxPhy The receiver
 * \return The RSRP 
 */
double
NistLteHelper::DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{
  Ptr<MobilityModel> txMobility = txPhy->GetMobility ();
  Ptr<MobilityModel> rxMobility = rxPhy->GetMobility ();

  double pathLossDb = 0;
  if (txPhy->GetRxAntenna() != 0)
    {
      Angles txAngles (rxMobility->GetPosition (), txMobility->GetPosition ());
      double txAntennaGain = txPhy->GetRxAntenna()->GetGainDb (txAngles);
      NS_LOG_DEBUG ("txAntennaGain = " << txAntennaGain << " dB");
      pathLossDb -= txAntennaGain;
    }
  Ptr<AntennaModel> rxAntenna = rxPhy->GetRxAntenna ();
  if (rxAntenna != 0)
    {
      Angles rxAngles (txMobility->GetPosition (), rxMobility->GetPosition ());
      double rxAntennaGain = rxAntenna->GetGainDb (rxAngles);
      NS_LOG_DEBUG ("rxAntennaGain = " << rxAntennaGain << " dB");
      pathLossDb -= rxAntennaGain;
    }
  if (propagationLoss)
    {
      double propagationGainDb = propagationLoss->CalcRxPower (0, txMobility, rxMobility);
      NS_LOG_DEBUG ("propagationGainDb = " << propagationGainDb << " dB");
      pathLossDb -= propagationGainDb;
    }                    
  NS_LOG_DEBUG ("total pathLoss = " << pathLossDb << " dB");  
  
  double rsrp = txPower - pathLossDb;

  NS_LOG_INFO ("RSRP=" << rsrp << " dBm");

  return rsrp;
}


/**
 * Computes the S-RSRP between 2 UEs. Information about the uplink frequency and band is necessary  to be able to call the function before the simulation starts.
 * \param txPower Transmit power for the reference signal
 * \param ulEarfcn Uplink frequency
 * \param ulBandwidth Uplink bandwidth
 * \param txDevice Transmitter UE
 * \param rxDevice Receiver UE
 * \return RSRP value
 */
double
NistLteHelper::CalcSidelinkRsrp (double txPower, double ulEarfcn, double ulBandwidth, Ptr<NetDevice> txDevice, Ptr<NetDevice> rxDevice)
{
  Ptr<PropagationLossModel> lossModel = m_uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << m_uplinkPathlossModel << " is not a PropagationLossModel");

  /*
    Sidelink Reference Signal Received Power (S-RSRP) is defined as the linear average over the
    power contributions (in [W]) of the resource elements that carry demodulation reference signals
    associated with PSBCH, within the central 6 PRBs of the applicable subframes. 
  */
  //This method returned very low values of RSRP 
  std::vector <int> rbMask;
  for (int i = 22; i < 28 ; i++)
    {
      rbMask.push_back (i);
    }
  NistLteSpectrumValueHelper psdHelper;
  Ptr<SpectrumValue> psd = psdHelper.CreateUlTxPowerSpectralDensity (ulEarfcn, ulBandwidth, txPower, rbMask);
  
  double rsrp= DoCalcRsrp (lossModel, psd, txDevice->GetObject<NistLteUeNetDevice> ()->GetPhy()->GetUlSpectrumPhy (), rxDevice->GetObject<NistLteUeNetDevice> ()->GetPhy()->GetUlSpectrumPhy ());
  
  NS_LOG_INFO ("S-RSRP=" << rsrp);

  return rsrp;
}

/**
 * Computes the RSRP between a transmitter UE and a receiver UE as defined in TR 36.843. Information about the uplink frequency and band is necessary  to be able to call the function before the simulation starts. 
 * \param txPower Transmit power for the reference signal
 * \param ulEarfcn Uplink frequency
 * \param txDevice Transmitter UE
 * \param rxDevice Receiver UE
 * \return RSRP value
 */
double
NistLteHelper::CalcSidelinkRsrpEval (double txPower, double ulEarfcn, Ptr<NetDevice> txDevice, Ptr<NetDevice> rxDevice)
{
  Ptr<PropagationLossModel> lossModel = m_uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ASSERT_MSG (lossModel != 0, " " << m_uplinkPathlossModel << " is not a PropagationLossModel");

  /*
    36.843: RSRP is calculated for transmit power of 23dBm by the transmitter UE and is the received power at the receiver UE calculated after accounting for large scale path loss and shadowing. Additionally note that wrap around is used for path loss calculations except for the case of partial -coverage.
  */
  double rsrp= DoCalcRsrp (lossModel, txPower, txDevice->GetObject<NistLteUeNetDevice> ()->GetPhy()->GetUlSpectrumPhy (), rxDevice->GetObject<NistLteUeNetDevice> ()->GetPhy()->GetUlSpectrumPhy ());
  
  NS_LOG_INFO ("RSRP=" << rsrp);

  return rsrp;
}


} // namespace ns3
