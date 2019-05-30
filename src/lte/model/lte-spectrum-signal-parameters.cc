/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 CTTC
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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by Marco Miozzo <mmiozzo@cttc.es> (add data and ctrl diversity)
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>
#include <ns3/lte-spectrum-signal-parameters.h>
#include <ns3/lte-control-messages.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSpectrumSignalParameters");

LteSpectrumSignalParameters::LteSpectrumSignalParameters ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParameters::LteSpectrumSignalParameters (const LteSpectrumSignalParameters& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  packetBurst = p.packetBurst->Copy ();
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParameters::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<LteSpectrumSignalParameters> (*this);
  // but for some reason it doesn't work. Another alternative is
  //   return Copy<LteSpectrumSignalParameters> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParameters> lssp (new LteSpectrumSignalParameters (*this), false);
  return lssp;
}



LteSpectrumSignalParametersDataFrame::LteSpectrumSignalParametersDataFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersDataFrame::LteSpectrumSignalParametersDataFrame (const LteSpectrumSignalParametersDataFrame& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  cellId = p.cellId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
  ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersDataFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<LteSpectrumSignalParametersDataFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  //   return Copy<LteSpectrumSignalParametersDataFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersDataFrame> lssp (new LteSpectrumSignalParametersDataFrame (*this), false);
  return lssp;
}



LteSpectrumSignalParametersDlCtrlFrame::LteSpectrumSignalParametersDlCtrlFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersDlCtrlFrame::LteSpectrumSignalParametersDlCtrlFrame (const LteSpectrumSignalParametersDlCtrlFrame& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  cellId = p.cellId;
  pss = p.pss;
  ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersDlCtrlFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<LteSpectrumSignalParametersDlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  //   return Copy<LteSpectrumSignalParametersDlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersDlCtrlFrame> lssp (new LteSpectrumSignalParametersDlCtrlFrame (*this), false);
  return lssp;
}


LteSpectrumSignalParametersUlSrsFrame::LteSpectrumSignalParametersUlSrsFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersUlSrsFrame::LteSpectrumSignalParametersUlSrsFrame (const LteSpectrumSignalParametersUlSrsFrame& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  cellId = p.cellId;
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersUlSrsFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<LteSpectrumSignalParametersUlSrsFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  //   return Copy<LteSpectrumSignalParametersUlSrsFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersUlSrsFrame> lssp (new LteSpectrumSignalParametersUlSrsFrame (*this), false);
  return lssp;
}


LteSpectrumSignalParametersSlFrame::LteSpectrumSignalParametersSlFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersSlFrame::LteSpectrumSignalParametersSlFrame (const LteSpectrumSignalParametersSlFrame& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  slssId = p.slssId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersSlFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersSlFrame> lssp (new LteSpectrumSignalParametersSlFrame (*this), false);
  return lssp;
}

LteSpectrumSignalParametersSlCtrlFrame::LteSpectrumSignalParametersSlCtrlFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersSlCtrlFrame::LteSpectrumSignalParametersSlCtrlFrame (const LteSpectrumSignalParametersSlCtrlFrame& p)
  : LteSpectrumSignalParametersSlFrame (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  groupId = p.groupId;
  slssId = p.slssId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersSlCtrlFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersSlCtrlFrame> lssp (new LteSpectrumSignalParametersSlCtrlFrame (*this), false);
  return lssp;
}

LteSpectrumSignalParametersSlDataFrame::LteSpectrumSignalParametersSlDataFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersSlDataFrame::LteSpectrumSignalParametersSlDataFrame (const LteSpectrumSignalParametersSlDataFrame& p)
  : LteSpectrumSignalParametersSlFrame (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  groupId = p.groupId;
  slssId = p.slssId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersSlDataFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  // return Copy<LteSpectrumSignalParametersSlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersSlDataFrame> lssp (new LteSpectrumSignalParametersSlDataFrame (*this), false);
  return lssp;
}

LteSpectrumSignalParametersSlDiscFrame::LteSpectrumSignalParametersSlDiscFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersSlDiscFrame::LteSpectrumSignalParametersSlDiscFrame (const LteSpectrumSignalParametersSlDiscFrame& p)
  : LteSpectrumSignalParametersSlFrame (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  resNo = p.resNo;
  rv = p.rv;
  slssId = p.slssId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersSlDiscFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  // return Copy<LteSpectrumSignalParametersSlDiscFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  // return Copy<LteSpectrumSignalParametersSlDiscFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersSlDiscFrame> lssp (new LteSpectrumSignalParametersSlDiscFrame (*this), false);
  return lssp;
}

LteSpectrumSignalParametersSlMibFrame::LteSpectrumSignalParametersSlMibFrame ()
{
  NS_LOG_FUNCTION (this);
}

LteSpectrumSignalParametersSlMibFrame::LteSpectrumSignalParametersSlMibFrame (const LteSpectrumSignalParametersSlMibFrame& p)
  : LteSpectrumSignalParametersSlFrame (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  slssId = p.slssId;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
LteSpectrumSignalParametersSlMibFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  // return Copy<LteSpectrumSignalParametersSlMibFrame> (*this);
  // but for some reason it doesn't work. Another alternative is
  // return Copy<LteSpectrumSignalParametersSlMibFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<LteSpectrumSignalParametersSlMibFrame> lssp (new LteSpectrumSignalParametersSlMibFrame (*this), false);
  return lssp;
}

} // namespace ns3
