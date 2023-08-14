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

#include <algorithm>
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/vector.h"
#include "ns3/mobility-model.h"
#include "ns3/simulator.h"
#include "ns3/seq-ts-size-header.h"

#include "schoolshooting-application-trace-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SchoolShootingApplicationTraceHelper");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (SchoolShootingApplicationTraceHelper);

uint16_t SchoolShootingApplicationTraceHelper::s_outputPrecision = 11;

SchoolShootingApplicationTraceHelper::SchoolShootingApplicationTraceHelper () :
  m_applicationHelper (nullptr)
{
  NS_LOG_FUNCTION (this);
}

SchoolShootingApplicationTraceHelper::SchoolShootingApplicationTraceHelper (Ptr<SchoolShootingApplicationHelper> applicationHelper)
{
  NS_LOG_FUNCTION (this << applicationHelper);
  m_applicationHelper = applicationHelper;
}


SchoolShootingApplicationTraceHelper::~SchoolShootingApplicationTraceHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_applicationHelper = nullptr;
  m_outputStreamWrappers.clear ();
}

TypeId
SchoolShootingApplicationTraceHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::SchoolShootingApplicationTraceHelper")
    .SetParent<Object> ()
    .AddConstructor<SchoolShootingApplicationTraceHelper> ()
  ;

  return tid;
}

void
SchoolShootingApplicationTraceHelper::EnablePscAppsTimeTrace (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_appTimeOutputStreamWrappers.find (name);
  NS_ABORT_MSG_UNLESS (it == m_appTimeOutputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnablePscAppsTimeTrace (" << name << ") only once");

  PscScenarioDefinition::ApplicationInfo ai = m_applicationHelper->GetScenarioDefinition()->GetApplication (name);
  Ptr<OutputStreamWrapper> appTimeOutputStreamWrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-" + ai.name + "-appTimes.txt", std::ios::out);

  //print header line
  *appTimeOutputStreamWrapper->GetStream () << "Name\tStartTime\tStopTime" << std::endl;

  ApplicationContainer apps = ((Ptr<SchoolShootingApplicationHelper>)m_applicationHelper)->GetPscApplications (name);
  for (uint32_t i = 0; i < apps.GetN (); i++)
    {
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext ("Times", MakeBoundCallback (&TraceAppTime, appTimeOutputStreamWrapper));
    }
  m_appTimeOutputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (name, appTimeOutputStreamWrapper));
}

void
SchoolShootingApplicationTraceHelper::TraceAppTime (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, Time startTime, Time stopTime)
{
  NS_LOG_FUNCTION (appOutputStreamWrapper << appName << startTime << stopTime);

  (*appOutputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *appOutputStreamWrapper->GetStream () << appName <<
    " " << startTime.GetSeconds () <<
    " " << stopTime.GetSeconds () << std::endl;
  appOutputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnablePscAppsTrafficTrace (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_appTrafficOutputStreamWrappers.find (name);
  NS_ABORT_MSG_UNLESS (it == m_appTrafficOutputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnablePscAppsTrafficTrace (" << name << ") only once");

  PscScenarioDefinition::ApplicationInfo ai = m_applicationHelper->GetScenarioDefinition()->GetApplication (name);
  Ptr<OutputStreamWrapper> appTrafficOutputStreamWrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-" + ai.name + "-appTraffic.txt", std::ios::out);

  //print header line
  *appTrafficOutputStreamWrapper->GetStream () << "Name\tTime\tAction\tPayload\tSeqNum" << std::endl;

  ApplicationContainer apps = ((Ptr<SchoolShootingApplicationHelper>)m_applicationHelper)->GetPscApplications (name);
  for (uint32_t i = 0; i < apps.GetN (); i++)
    {
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext (
        "Rx", MakeBoundCallback (&TraceAppRxTraffic, appTrafficOutputStreamWrapper));
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext (
        "Tx", MakeBoundCallback (&TraceAppTxTraffic, appTrafficOutputStreamWrapper));
    }
  m_appTrafficOutputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (name, appTrafficOutputStreamWrapper));
}

void
SchoolShootingApplicationTraceHelper::TraceAppRxTraffic (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, SeqTsSizeHeader header)
{
  NS_LOG_FUNCTION (appOutputStreamWrapper << appName << header);

  (*appOutputStreamWrapper->GetStream ()).precision (s_outputPrecision);

  *appOutputStreamWrapper->GetStream () << appName <<
    " " << Simulator::Now ().GetSeconds () <<
    " RX " << header.GetSize () <<
    " " << header.GetSeq () << std::endl;

  appOutputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceAppTxTraffic (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, SeqTsSizeHeader header)
{
  NS_LOG_FUNCTION (appOutputStreamWrapper << appName << header);

  (*appOutputStreamWrapper->GetStream ()).precision (s_outputPrecision);

  *appOutputStreamWrapper->GetStream () << appName <<
    " " << Simulator::Now ().GetSeconds () <<
    " TX " << header.GetSize () <<
    " " << header.GetSeq () <<  std::endl;

  appOutputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableSwatHelmetCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableSwatHelmetCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-SwatHelmetCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SWAT_HELMET_CAMERA, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxSwatHelmetCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxSwatHelmetCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxSwatHelmetCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxSwatHelmetCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableDeployableCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableDeployableCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-DeployableCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_DEPLOYABLE_CAMERA, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxDeployableCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxDeployableCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxDeployableCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxDeployableCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableThrowPhoneCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE_VIDEO);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableThrowPhoneCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-ThrowPhoneCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_THROW_PHONE_VIDEO, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxThrowPhoneCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxThrowPhoneCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxThrowPhoneCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxThrowPhoneCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableHelicopterCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableHelicopterCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-HelicopterCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_HELICOPTER, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxHelicopterCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxHelicopterCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxHelicopterCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxHelicopterCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableEmsVehicleCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableEmsVehicleCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-EmsVehicleCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_EMS_VEHICLES, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxEmsVehicleCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxEmsVehicleCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxEmsVehicleCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxEmsVehicleCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableVideoConferenceVideoTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE_VIDEO);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableVideoConferenceVideoTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-VideoConferenceVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE_VIDEO, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxVideoConferenceVideo, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxVideoConferenceVideo, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxVideoConferenceVideo (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxVideoConferenceVideo (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableSchoolCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableSchoolCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-SchoolCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_SCHOOL_VIDEO, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxSchoolCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxSchoolCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxSchoolCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxSchoolCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableTrafficCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableTrafficCameraTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-TrafficCameraVideo-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_TRAFFIC_VIDEO, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxTrafficCamera, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxTrafficCamera, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxTrafficCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxTrafficCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableWebTraces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableWebTraces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-Web-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_WEB, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxWeb, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxWeb, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxWeb (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxWeb (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::EnableNg911Traces (ApplicationContainer txApps, ApplicationContainer rxApps)
{
  NS_LOG_FUNCTION (this);

  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> >::iterator it = m_outputStreamWrappers.find (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911);
  NS_ABORT_MSG_UNLESS (it == m_outputStreamWrappers.end (), "Make sure to call PscScenarioTraceHelper::EnableNg911Traces () only once");

  Ptr<OutputStreamWrapper> wrapper = Create<OutputStreamWrapper> (m_applicationHelper->GetScenarioDefinition()->GetName () + "-NG911Video-appTraffic.txt", std::ios::out);
  m_outputStreamWrappers.insert (std::pair<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > (SchoolShootingApplicationHelper::SCHOOL_SHOOTING_NG911, wrapper));
  //print header line
  *wrapper->GetStream () << "Time\tID\tAction\tSeqNum\tSize" << std::endl;

  for (uint16_t i = 0; i < txApps.GetN (); i++)
    {
      txApps.Get (i)->TraceConnectWithoutContext ("Tx", MakeBoundCallback (&TraceTxNg911, wrapper, txApps.Get (i)->GetNode ()));
    }
  for (uint16_t i = 0; i < rxApps.GetN (); i++)
    {
      rxApps.Get (i)->TraceConnectWithoutContext ("Rx", MakeBoundCallback (&TraceRxNg911, wrapper, rxApps.Get (i)->GetNode ()));
    }
}

void
SchoolShootingApplicationTraceHelper::TraceTxNg911 (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (outputStreamWrapper);

  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tTX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::TraceRxNg911 (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs)
{
  NS_LOG_FUNCTION (outputStreamWrapper);
  //Extract header information
  SeqTsSizeHeader stsh;
  p->PeekHeader (stsh);

  (*outputStreamWrapper->GetStream ()).precision (s_outputPrecision);
  *outputStreamWrapper->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << node->GetId () << "\tRX\t" << stsh.GetSeq () << "\t" << p->GetSize () << std::endl;
  outputStreamWrapper->GetStream ()->flush ();
}

void
SchoolShootingApplicationTraceHelper::SetOutputPrecision (uint16_t outputPrecision)
{
  s_outputPrecision = outputPrecision;
}

uint16_t
SchoolShootingApplicationTraceHelper::GetOutputPrecision (void)
{
  return s_outputPrecision;
}

} // namespace psc
} // namespace ns3

