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
 * Author: Nicola Baldo  <nbaldo@cttc.es>
 */


#ifndef NIST_EPC_TFT_CLASSIFIER_H
#define NIST_EPC_TFT_CLASSIFIER_H


#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"
#include "ns3/nist-epc-tft.h"

#include <map>


namespace ns3 {

class NistEpcTft;
class Packet;

/**
 * \brief classifies IP packets accoding to Traffic Flow Templates (TFTs)
 * 
 * \note this implementation works with IPv4 only.
 */
class NistEpcTftClassifier : public SimpleRefCount<NistEpcTftClassifier>
{
public:
  
  NistEpcTftClassifier ();
  
  /** 
   * add a TFT to the Classifier
   * 
   * \param tft the TFT to be added
   * 
   */
  void Add (Ptr<NistEpcTft> tft, uint32_t id);

  /** 
   * delete an existing TFT from the classifier
   * 
   * \param id the identifier of the TFT to be deleted
   */
  void Delete (uint32_t id);
  

  /** 
   * classify an IP packet
   * 
   * \param p the IP packet. It is assumed that the outmost header is an IPv4 header.
   * 
   * \return the identifier (>0) of the first TFT that matches with the IP packet; 0 if no TFT matched.
   */
  uint32_t Classify (Ptr<Packet> p, NistEpcTft::Direction direction);
  
protected:
  
  std::map <uint32_t, Ptr<NistEpcTft> > m_tftMap;
  
};



} // namespace ns3
  
#endif /* TFT_CLASSIFIER_H */ 
