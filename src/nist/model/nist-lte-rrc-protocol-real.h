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
 * Authors: Nicola Baldo <nbaldo@cttc.es>
 *          Lluis Parcerisa <lparcerisa@cttc.cat>
 * Modified by: NIST
 */


#ifndef NIST_LTE_RRC_PROTOCOL_REAL_H
#define NIST_LTE_RRC_PROTOCOL_REAL_H

#include <stdint.h>
#include <map>

#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <ns3/nist-lte-pdcp-sap.h>
#include <ns3/nist-lte-rlc-sap.h>
#include <ns3/nist-lte-rrc-header.h>

namespace ns3 {

class NistLteUeRrcSapProvider;
class NistLteUeRrcSapUser;
class NistLteEnbRrcSapProvider;
class NistLteUeRrc;


/**
 * Models the transmission of RRC messages from the UE to the eNB in
 * a real fashion, by creating real RRC PDUs and transmitting them
 * over Signaling Radio Bearers using radio resources allocated by the
 * LTE MAC scheduler.
 * 
 */
class NistLteUeRrcProtocolReal : public Object
{
  friend class NistMemberLteUeRrcSapUser<NistLteUeRrcProtocolReal>;
  friend class NistLteRlcSpecificLteRlcSapUser<NistLteUeRrcProtocolReal>;
  friend class NistLtePdcpSpecificLtePdcpSapUser<NistLteUeRrcProtocolReal>;

public:
  NistLteUeRrcProtocolReal ();
  virtual ~NistLteUeRrcProtocolReal ();

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
  void DoReceivePdcpPdu (Ptr<Packet> p);
  void DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params);

  Ptr<NistLteUeRrc> m_rrc;
  uint16_t m_rnti;
  NistLteUeRrcSapProvider* m_ueRrcSapProvider;
  NistLteUeRrcSapUser* m_ueRrcSapUser;
  NistLteEnbRrcSapProvider* m_enbRrcSapProvider;

  NistLteUeRrcSapUser::NistSetupParameters m_setupParameters;
  NistLteUeRrcSapProvider::CompleteNistSetupParameters m_completeNistSetupParameters;

};


/**
 * Models the transmission of RRC messages from the UE to the eNB in
 * a real fashion, by creating real RRC PDUs and transmitting them
 * over Signaling Radio Bearers using radio resources allocated by the
 * LTE MAC scheduler.
 *
 */
class NistLteEnbRrcProtocolReal : public Object
{
  friend class NistMemberLteEnbRrcSapUser<NistLteEnbRrcProtocolReal>;
  friend class NistLtePdcpSpecificLtePdcpSapUser<NistLteEnbRrcProtocolReal>;
  friend class NistLteRlcSpecificLteRlcSapUser<NistLteEnbRrcProtocolReal>;
  friend class NistRealProtocolRlcSapUser;

public:
  NistLteEnbRrcProtocolReal ();
  virtual ~NistLteEnbRrcProtocolReal ();

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

  void DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params);
  void DoReceivePdcpPdu (uint16_t rnti, Ptr<Packet> p);

  uint16_t m_rnti;
  uint16_t m_cellId;
  NistLteEnbRrcSapProvider* m_enbRrcSapProvider;
  NistLteEnbRrcSapUser* m_enbRrcSapUser;
  std::map<uint16_t, NistLteUeRrcSapProvider*> m_enbRrcSapProviderMap;
  std::map<uint16_t, NistLteEnbRrcSapUser::NistSetupUeParameters> m_setupUeParametersMap;
  std::map<uint16_t, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters> m_completeNistSetupUeParametersMap;

};

///////////////////////////////////////

class NistRealProtocolRlcSapUser : public NistLteRlcSapUser
{
public:
  NistRealProtocolRlcSapUser (NistLteEnbRrcProtocolReal* pdcp, uint16_t rnti);

  // Interface implemented from NistLteRlcSapUser
  virtual void ReceivePdcpPdu (Ptr<Packet> p);

private:
  NistRealProtocolRlcSapUser ();
  NistLteEnbRrcProtocolReal* m_pdcp;
  uint16_t m_rnti;
};


}


#endif // NIST_LTE_RRC_PROTOCOL_REAL_H
