// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-spectrum-signal-parameters.h"

#include "nr-sl-control-messages.h"

#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlSpectrumSignalParameters");

NrSpectrumSignalParametersSlFrame::NrSpectrumSignalParametersSlFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersSlFrame::NrSpectrumSignalParametersSlFrame(
    const NrSpectrumSignalParametersSlFrame& p)
    : SpectrumSignalParameters(p)
{
    NS_LOG_FUNCTION(this << &p);
    nodeId = p.nodeId;
    // slssId = p.slssId; //TODO
    if (p.packetBurst)
    {
        packetBurst = p.packetBurst->Copy();
    }
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersSlFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSlSpectrumSignalParametersSlCtrlFrame> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSlSpectrumSignalParametersSlCtrlFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersSlFrame> lssp(new NrSpectrumSignalParametersSlFrame(*this),
                                                false);
    return lssp;
}

NrSpectrumSignalParametersSlCtrlFrame::NrSpectrumSignalParametersSlCtrlFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersSlCtrlFrame::NrSpectrumSignalParametersSlCtrlFrame(
    const NrSpectrumSignalParametersSlCtrlFrame& p)
    : NrSpectrumSignalParametersSlFrame(p)
{
    NS_LOG_FUNCTION(this << &p);
    nodeId = p.nodeId;
    // slssId = p.slssId; //TODO
    if (p.packetBurst)
    {
        packetBurst = p.packetBurst->Copy();
    }
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersSlCtrlFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersSlCtrlFrame> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSpectrumSignalParametersSlCtrlFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersSlCtrlFrame> lssp(
        new NrSpectrumSignalParametersSlCtrlFrame(*this),
        false);
    return lssp;
}

NrSpectrumSignalParametersSlDataFrame::NrSpectrumSignalParametersSlDataFrame()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersSlDataFrame::NrSpectrumSignalParametersSlDataFrame(
    const NrSpectrumSignalParametersSlDataFrame& p)
    : NrSpectrumSignalParametersSlFrame(p)
{
    NS_LOG_FUNCTION(this << &p);
    nodeId = p.nodeId;
    // slssId = p.slssId; //TODO
    if (p.packetBurst)
    {
        packetBurst = p.packetBurst->Copy();
    }
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersSlDataFrame::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersSlCtrlFrame> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSpectrumSignalParametersSlCtrlFrame> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersSlDataFrame> lssp(
        new NrSpectrumSignalParametersSlDataFrame(*this),
        false);
    return lssp;
}

NrSpectrumSignalParametersSlFeedback::NrSpectrumSignalParametersSlFeedback()
{
    NS_LOG_FUNCTION(this);
}

NrSpectrumSignalParametersSlFeedback::NrSpectrumSignalParametersSlFeedback(
    const NrSpectrumSignalParametersSlFeedback& p)
    : NrSpectrumSignalParametersSlFrame(p)
{
    NS_LOG_FUNCTION(this << &p);
    nodeId = p.nodeId;
    feedbackList = p.feedbackList;
}

Ptr<SpectrumSignalParameters>
NrSpectrumSignalParametersSlFeedback::Copy() const
{
    NS_LOG_FUNCTION(this);
    // Ideally we would use:
    //   return Copy<NrSpectrumSignalParametersSlFeedback> (*this);
    // but for some reason it doesn't work. Another alternative is
    //   return Copy<NrSpectrumSignalParametersSlFeedback> (this);
    // but it causes a double creation of the object, hence it is less efficient.
    // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
    Ptr<NrSpectrumSignalParametersSlFeedback> lssp(new NrSpectrumSignalParametersSlFeedback(*this),
                                                   false);
    return lssp;
}

} // namespace ns3
