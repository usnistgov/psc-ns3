/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 *         Marco Miozzo <marco.miozzo@cttc.es>
 * Modified by: NIST
 */

#include "nist-lte-control-messages.h"
#include "ns3/address-utils.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "nist-lte-net-device.h"
#include "nist-lte-ue-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteControlMessage");

NistLteControlMessage::NistLteControlMessage (void)
{
}


NistLteControlMessage::~NistLteControlMessage (void)
{
}


void
NistLteControlMessage::SetMessageType (NistLteControlMessage::MessageType type)
{
  m_type = type;
}


NistLteControlMessage::MessageType
NistLteControlMessage::GetMessageType (void)
{
  return m_type;
}


// ----------------------------------------------------------------------------------------------------------


NistDlDciLteControlMessage::NistDlDciLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::DL_DCI);
}


NistDlDciLteControlMessage::~NistDlDciLteControlMessage (void)
{

}

void
NistDlDciLteControlMessage::SetDci (NistDlDciListElement_s dci)
{
  m_dci = dci;

}


NistDlDciListElement_s
NistDlDciLteControlMessage::GetDci (void)
{
  return m_dci;
}


// ----------------------------------------------------------------------------------------------------------


NistUlDciLteControlMessage::NistUlDciLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::UL_DCI);
}


NistUlDciLteControlMessage::~NistUlDciLteControlMessage (void)
{

}

void
NistUlDciLteControlMessage::SetDci (NistUlDciListElement_s dci)
{
  m_dci = dci;

}


NistUlDciListElement_s
NistUlDciLteControlMessage::GetDci (void)
{
  return m_dci;
}


// ----------------------------------------------------------------------------------------------------------


NistSlDciLteControlMessage::NistSlDciLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::SL_DCI);
}


NistSlDciLteControlMessage::~NistSlDciLteControlMessage (void)
{

}

void
NistSlDciLteControlMessage::SetDci (NistSlDciListElement_s dci)
{
  m_dci = dci;

}


NistSlDciListElement_s
NistSlDciLteControlMessage::GetDci (void)
{
  return m_dci;
}


// ----------------------------------------------------------------------------------------------------------


NistDlCqiLteControlMessage::NistDlCqiLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::DL_CQI);
}


NistDlCqiLteControlMessage::~NistDlCqiLteControlMessage (void)
{

}

void
NistDlCqiLteControlMessage::SetDlCqi (NistCqiListElement_s dlcqi)
{
  m_dlCqi = dlcqi;

}


NistCqiListElement_s
NistDlCqiLteControlMessage::GetDlCqi (void)
{
  return m_dlCqi;
}



// ----------------------------------------------------------------------------------------------------------


NistBsrLteControlMessage::NistBsrLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::BSR);
}


NistBsrLteControlMessage::~NistBsrLteControlMessage (void)
{

}

void
NistBsrLteControlMessage::SetBsr (NistMacCeListElement_s bsr)
{
  m_bsr = bsr;

}


NistMacCeListElement_s
NistBsrLteControlMessage::GetBsr (void)
{
  return m_bsr;
}



// ----------------------------------------------------------------------------------------------------------


NistRachPreambleLteControlMessage::NistRachPreambleLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::RACH_PREAMBLE);
}

void
NistRachPreambleLteControlMessage::SetRapId (uint32_t rapId)
{
  m_rapId = rapId;
}

uint32_t 
NistRachPreambleLteControlMessage::GetRapId () const
{
  return m_rapId;
}


// ----------------------------------------------------------------------------------------------------------


NistRarLteControlMessage::NistRarLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::RAR);
}


void
NistRarLteControlMessage::SetRaRnti (uint16_t raRnti)
{
  m_raRnti = raRnti;
}

uint16_t 
NistRarLteControlMessage::GetRaRnti () const
{
  return m_raRnti;
}


void
NistRarLteControlMessage::AddNistRar (NistRar rar)
{
  m_rarList.push_back (rar);
}

std::list<NistRarLteControlMessage::NistRar>::const_iterator 
NistRarLteControlMessage::NistRarListBegin () const
{
  return m_rarList.begin ();
}

std::list<NistRarLteControlMessage::NistRar>::const_iterator 
NistRarLteControlMessage::NistRarListEnd () const
{
  return m_rarList.end ();
}


// ----------------------------------------------------------------------------------------------------------



NistMibLteControlMessage::NistMibLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::MIB);
}


void
NistMibLteControlMessage::SetMib (NistLteRrcSap::NistMasterInformationBlock  mib)
{
  m_mib = mib;
}

NistLteRrcSap::NistMasterInformationBlock 
NistMibLteControlMessage::GetMib () const
{
  return m_mib;
}


// ----------------------------------------------------------------------------------------------------------



NistSib1LteControlMessage::NistSib1LteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::SIB1);
}


void
NistSib1LteControlMessage::SetSib1 (NistLteRrcSap::NistSystemInformationBlockType1 sib1)
{
  m_sib1 = sib1;
}

NistLteRrcSap::NistSystemInformationBlockType1
NistSib1LteControlMessage::GetSib1 () const
{
  return m_sib1;
}


// ----------------------------------------------------------------------------------------------------------



SciNistLteControlMessage::SciNistLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::SCI);
}


void
SciNistLteControlMessage::SetSci (NistSciListElement_s sci)
{
  m_sci = sci;
}

NistSciListElement_s
SciNistLteControlMessage::GetSci ()
{
  return m_sci;
}


// ---------------------------------------------------------------------------



NistDlHarqFeedbackLteControlMessage::NistDlHarqFeedbackLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::DL_HARQ);
}


NistDlHarqFeedbackLteControlMessage::~NistDlHarqFeedbackLteControlMessage (void)
{

}

void
NistDlHarqFeedbackLteControlMessage::SetDlHarqFeedback (NistDlInfoListElement_s m)
{
  m_dlInfoListElement = m;
}


NistDlInfoListElement_s
NistDlHarqFeedbackLteControlMessage::GetDlHarqFeedback (void)
{
  return m_dlInfoListElement;
}

// ----------------------------------------------------------------------------------------------------------
MibSLNistLteControlMessage::MibSLNistLteControlMessage (void)
{
  SetMessageType (NistLteControlMessage::MIB_SL);
}

void
MibSLNistLteControlMessage::SetMibSL (NistLteRrcSap::MasterInformationBlockSL mibSL)
{
  m_mibSL = mibSL;
}

NistLteRrcSap::MasterInformationBlockSL
MibSLNistLteControlMessage::GetMibSL ()
{
  return m_mibSL;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------

NistSlDiscMessage::NistSlDiscMessage (void)
{
  SetMessageType (NistLteControlMessage::SL_DISC_MSG);
}


void
NistSlDiscMessage::SetSlDiscMessage (NistSlDiscMsg discMsg)
{
  m_discMsg = discMsg;
}

NistSlDiscMsg
NistSlDiscMessage::GetSlDiscMessage ()
{
  return m_discMsg;
}


// ---------------------------------------------------------------------------


} // namespace ns3

