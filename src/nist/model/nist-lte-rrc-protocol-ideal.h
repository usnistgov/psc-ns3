/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Modified by: NIST
 */


#ifndef NIST_LTE_RRC_PROTOCOL_IDEAL_H
#define NIST_LTE_RRC_PROTOCOL_IDEAL_H

#include <stdint.h>
#include <map>

#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {

class NistLteUeRrcSapProvider;
class NistLteUeRrcSapUser;
class NistLteEnbRrcSapProvider;
class NistLteUeRrc;


/**
 * Models the transmission of RRC messages from the UE to the eNB in
 * an ideal fashion, without errors and without consuming any radio
 * resources. 
 * 
 */
class NistLteUeRrcProtocolIdeal : public Object
{
  friend class NistMemberLteUeRrcSapUser<NistLteUeRrcProtocolIdeal>;

public:

  NistLteUeRrcProtocolIdeal ();
  virtual ~NistLteUeRrcProtocolIdeal ();

  // inherited from Object
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  void SetNistLteUeRrcSapProvider (NistLteUeRrcSapProvider* p);
  NistLteUeRrcSapUser* GetNistLteUeRrcSapUser ();
  
  void SetUeRrc (Ptr<NistLteUeRrc> rrc);
  

private:

  // methods forwarded from NistLteUeRrcSapUser
  void DoSetup (NistLteUeRrcSapUser::NistSetupParameters params);
  void DoSendNistRrcConnectionRequest (NistLteRrcSap::NistRrcConnectionRequest msg);
  void DoSendNistRrcConnectionSetupCompleted (NistLteRrcSap::NistRrcConnectionSetupCompleted msg);
  void DoSendNistRrcConnectionReconfigurationCompleted (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg);
  void DoSendNistRrcConnectionReestablishmentRequest (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg);
  void DoSendNistRrcConnectionReestablishmentComplete (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg);
  void DoSendNistMeasurementReport (NistLteRrcSap::NistMeasurementReport msg);
  void DoSendSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation msg);

  void SetEnbRrcSapProvider ();

  Ptr<NistLteUeRrc> m_rrc;
  uint16_t m_rnti;
  NistLteUeRrcSapProvider* m_ueRrcSapProvider;
  NistLteUeRrcSapUser* m_ueRrcSapUser;
  NistLteEnbRrcSapProvider* m_enbRrcSapProvider;
  
};


/**
 * Models the transmission of RRC messages from the UE to the eNB in
 * an ideal fashion, without errors and without consuming any radio
 * resources. 
 * 
 */
class NistLteEnbRrcProtocolIdeal : public Object
{
  friend class NistMemberLteEnbRrcSapUser<NistLteEnbRrcProtocolIdeal>;

public:

  NistLteEnbRrcProtocolIdeal ();
  virtual ~NistLteEnbRrcProtocolIdeal ();

  // inherited from Object
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  void SetNistLteEnbRrcSapProvider (NistLteEnbRrcSapProvider* p);
  NistLteEnbRrcSapUser* GetNistLteEnbRrcSapUser ();

  void SetCellId (uint16_t cellId);

  NistLteUeRrcSapProvider* GetUeRrcSapProvider (uint16_t rnti);
  void SetUeRrcSapProvider (uint16_t rnti, NistLteUeRrcSapProvider* p);

private:

  // methods forwarded from NistLteEnbRrcSapUser
  void DoSetupUe (uint16_t rnti, NistLteEnbRrcSapUser::NistSetupUeParameters params);
  void DoRemoveUe (uint16_t rnti);
  void DoSendNistSystemInformation (NistLteRrcSap::NistSystemInformation msg);
  void SendNistSystemInformation (NistLteRrcSap::NistSystemInformation msg);
  void DoSendNistRrcConnectionSetup (uint16_t rnti, NistLteRrcSap::NistRrcConnectionSetup msg);
  void DoSendNistRrcConnectionReconfiguration (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReconfiguration msg);
  void DoSendNistRrcConnectionReestablishment (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishment msg);
  void DoSendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentReject msg);
  void DoSendNistRrcConnectionRelease (uint16_t rnti, NistLteRrcSap::NistRrcConnectionRelease msg);
  void DoSendNistRrcConnectionReject (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReject msg);
  Ptr<Packet> DoEncodeNistHandoverPreparationInformation (NistLteRrcSap::NistHandoverPreparationInfo msg);
  NistLteRrcSap::NistHandoverPreparationInfo DoDecodeNistHandoverPreparationInformation (Ptr<Packet> p);
  Ptr<Packet> DoEncodeHandoverCommand (NistLteRrcSap::NistRrcConnectionReconfiguration msg);
  NistLteRrcSap::NistRrcConnectionReconfiguration DoDecodeHandoverCommand (Ptr<Packet> p);


  uint16_t m_rnti;
  uint16_t m_cellId;
  NistLteEnbRrcSapProvider* m_enbRrcSapProvider;
  NistLteEnbRrcSapUser* m_enbRrcSapUser;
  std::map<uint16_t, NistLteUeRrcSapProvider*> m_enbRrcSapProviderMap;
  
};



}


#endif // NIST_LTE_RRC_PROTOCOL_IDEAL_H
