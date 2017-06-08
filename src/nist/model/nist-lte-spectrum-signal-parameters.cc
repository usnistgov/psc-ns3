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
 * Modified by: NIST
 */

#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>
#include <ns3/nist-lte-spectrum-signal-parameters.h>
#include <ns3/nist-lte-control-messages.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteSpectrumSignalParameters");

NistLteSpectrumSignalParameters::NistLteSpectrumSignalParameters ()
{
  NS_LOG_FUNCTION (this);
}

NistLteSpectrumSignalParameters::NistLteSpectrumSignalParameters (const NistLteSpectrumSignalParameters& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  packetBurst = p.packetBurst->Copy ();
}

Ptr<SpectrumSignalParameters>
NistLteSpectrumSignalParameters::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<NistLteSpectrumSignalParameters> (*this);
  // but for some reason it doesn't work. Another alternative is 
  //   return Copy<NistLteSpectrumSignalParameters> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<NistLteSpectrumSignalParameters> lssp (new NistLteSpectrumSignalParameters (*this), false);  
  return lssp;
}



NistLteSpectrumSignalParametersDataFrame::NistLteSpectrumSignalParametersDataFrame ()
{
  NS_LOG_FUNCTION (this);
}

NistLteSpectrumSignalParametersDataFrame::NistLteSpectrumSignalParametersDataFrame (const NistLteSpectrumSignalParametersDataFrame& p)
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
NistLteSpectrumSignalParametersDataFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<NistLteSpectrumSignalParametersDataFrame> (*this);
  // but for some reason it doesn't work. Another alternative is 
  //   return Copy<NistLteSpectrumSignalParametersDataFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<NistLteSpectrumSignalParametersDataFrame> lssp (new NistLteSpectrumSignalParametersDataFrame (*this), false);  
  return lssp;
}



NistLteSpectrumSignalParametersDlCtrlFrame::NistLteSpectrumSignalParametersDlCtrlFrame ()
{
  NS_LOG_FUNCTION (this);
}

NistLteSpectrumSignalParametersDlCtrlFrame::NistLteSpectrumSignalParametersDlCtrlFrame (const NistLteSpectrumSignalParametersDlCtrlFrame& p)
: SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  cellId = p.cellId;
  pss = p.pss;
  ctrlMsgList = p.ctrlMsgList;
}

Ptr<SpectrumSignalParameters>
NistLteSpectrumSignalParametersDlCtrlFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<NistLteSpectrumSignalParametersDlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is 
  //   return Copy<NistLteSpectrumSignalParametersDlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<NistLteSpectrumSignalParametersDlCtrlFrame> lssp (new NistLteSpectrumSignalParametersDlCtrlFrame (*this), false);  
  return lssp;
}


NistLteSpectrumSignalParametersUlSrsFrame::NistLteSpectrumSignalParametersUlSrsFrame ()
{
  NS_LOG_FUNCTION (this);
}

NistLteSpectrumSignalParametersUlSrsFrame::NistLteSpectrumSignalParametersUlSrsFrame (const NistLteSpectrumSignalParametersUlSrsFrame& p)
: SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  cellId = p.cellId;
}

Ptr<SpectrumSignalParameters>
NistLteSpectrumSignalParametersUlSrsFrame::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<NistLteSpectrumSignalParametersUlSrsFrame> (*this);
  // but for some reason it doesn't work. Another alternative is 
  //   return Copy<NistLteSpectrumSignalParametersUlSrsFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<NistLteSpectrumSignalParametersUlSrsFrame> lssp (new NistLteSpectrumSignalParametersUlSrsFrame (*this), false);  
  return lssp;
}


NistLteSpectrumSignalParametersSlFrame::NistLteSpectrumSignalParametersSlFrame ()
{
  NS_LOG_FUNCTION (this);
}

NistLteSpectrumSignalParametersSlFrame::NistLteSpectrumSignalParametersSlFrame (const NistLteSpectrumSignalParametersSlFrame& p)
: SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  nodeId = p.nodeId;
  groupId = p.groupId;
  slssId = p.slssId;
  ctrlMsgList = p.ctrlMsgList;
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
}

Ptr<SpectrumSignalParameters>
NistLteSpectrumSignalParametersSlFrame::Copy ()
{
    NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<NistLteSpectrumSignalParametersSlCtrlFrame> (*this);
  // but for some reason it doesn't work. Another alternative is 
  //   return Copy<NistLteSpectrumSignalParametersSlCtrlFrame> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<NistLteSpectrumSignalParametersSlFrame> lssp (new NistLteSpectrumSignalParametersSlFrame (*this), false);  
  return lssp;
}

} // namespace ns3
