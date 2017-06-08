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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_HELPER_H
#define NIST_LTE_HELPER_H

#include <ns3/config.h>
#include <ns3/simulator.h>
#include <ns3/names.h>
#include <ns3/net-device.h>
#include <ns3/net-device-container.h>
#include <ns3/node.h>
#include <ns3/node-container.h>
#include <ns3/nist-eps-bearer.h>
#include <ns3/nist-phy-stats-calculator.h>
#include <ns3/nist-phy-tx-stats-calculator.h>
#include <ns3/nist-phy-rx-stats-calculator.h>
#include <ns3/nist-mac-stats-calculator.h>
#include <ns3/nist-radio-bearer-stats-calculator.h>
#include <ns3/nist-radio-bearer-stats-connector.h>
#include <ns3/nist-epc-tft.h>
#include <ns3/nist-lte-enb-rrc.h>
#include <ns3/nist-lte-ue-rrc.h>
#include <ns3/nist-lte-spectrum-phy.h>
#include <ns3/mobility-model.h>
#include <ns3/nist-sl-tft.h>

namespace ns3 {


class NistLteUePhy;
class NistLteEnbPhy;
class SpectrumChannel;
class NistEpcHelper;
class PropagationLossModel;
class SpectrumPropagationLossModel;

/**
 * \ingroup lte
 *
 * Creation and configuration of LTE entities. One NistLteHelper instance is
 * typically enough for an LTE simulation. To create it:
 *
 *     Ptr<NistLteHelper> lteHelper = CreateObject<NistLteHelper> ();
 *
 * The general responsibility of the helper is to create various LTE objects
 * and arrange them together to make the whole LTE system. The overall
 * arrangement would look like the following:
 * - Downlink spectrum channel
 *   + Path loss model
 *   + Fading model
 * - Uplink spectrum channel
 *   + Path loss model
 *   + Fading model
 * - eNodeB node(s)
 *   + Mobility model
 *   + eNodeB device(s)
 *     * Antenna model
 *     * eNodeB PHY (includes spectrum PHY, interference model, HARQ model)
 *     * eNodeB MAC
 *     * eNodeB RRC (includes RRC protocol)
 *     * Scheduler
 *     * Handover algorithm
 *     * FFR (frequency reuse) algorithm
 *     * ANR (automatic neighbour relation)
 *   + EPC related models (EPC application, Internet stack, X2 interface)
 * - UE node(s)
 *   + Mobility model
 *   + UE device(s)
 *     * Antenna model
 *     * UE PHY (includes spectrum PHY, interference model, HARQ model)
 *     * UE MAC
 *     * UE RRC (includes RRC protocol)
 *     * NAS
 * - EPC helper
 * - Various statistics calculator objects
 *
 * Spetrum channels are created automatically: one for DL, and one for UL.
 * eNodeB devices are created by calling InstallEnbDevice(), while UE devices
 * are created by calling InstallUeDevice(). EPC helper can be set by using
 * SetEpcHelper().
 */
class NistLteHelper : public Object
{
public:
  NistLteHelper (void);
  virtual ~NistLteHelper (void);

  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose (void);

  /** 
   * Set the NistEpcHelper to be used to setup the EPC network in
   * conjunction with the setup of the LTE radio access network.
   *
   * \note if no NistEpcHelper is ever set, then NistLteHelper will default
   * to creating an LTE-only simulation with no EPC, using NistLteRlcSm as
   * the RLC model, and without supporting any IP networking. In other
   * words, it will be a radio-level simulation involving only LTE PHY
   * and MAC and the FF Scheduler, with a saturation traffic model for
   * the RLC.
   * 
   * \param h a pointer to the NistEpcHelper to be used
   */
  void SetEpcHelper (Ptr<NistEpcHelper> h);

  /**
   * Set eNBs created after this call to enable.
   *
   */
  void EnableNewEnbPhy ();

  /**
   * Set eNBs created after this call to disable.
   *
   */
  void DisableNewEnbPhy ();


  /** 
   * Set the type of path loss model to be used for both DL and UL channels.
   * 
   * \param type type of path loss model, must be a type name of any class
   *             inheriting from ns3::PropagationLossModel, for example:
   *             "ns3::FriisPropagationLossModel"
   */
  void SetPathlossModelType (std::string type);

  /**
   * Set an attribute for the path loss models to be created.
   * 
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetPathlossModelAttribute (std::string n, const AttributeValue &v);

  /** 
   * Set the type of scheduler to be used by eNodeB devices.
   * 
   * \param type type of scheduler, must be a type name of any class
   *             inheriting from ns3::NistFfMacScheduler, for example:
   *             "ns3::PfNistFfMacScheduler"
   *
   * Equivalent with setting the `Scheduler` attribute.
   */
  void SetSchedulerType (std::string type);

  /**
   *
   * \return the scheduler type
   */
  std::string GetSchedulerType () const; 

  /** 
   * Set the type of UL scheduler to be used by UEs devices.
   *
   * \param type the UE scheduler to set 
   */
  void SetUlSchedulerType (std::string type);

  /**
   *
   * \return the UL scheduler type
   */
  std::string GetUlSchedulerType () const; 

  /**
   * Set an attribute for the scheduler to be created.
   * 
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetSchedulerAttribute (std::string n, const AttributeValue &v);

  /**
   * Set the type of FFR algorithm to be used by eNodeB devices.
   *
   * \param type type of FFR algorithm, must be a type name of any class
   *             inheriting from ns3::NistLteFfrAlgorithm, for example:
   *             "ns3::NistLteFrNoOpAlgorithm"
   *
   * Equivalent with setting the `FfrAlgorithm` attribute.
   */
  void SetFfrAlgorithmType (std::string type);

  /**
   *
   * \return the FFR algorithm type
   */
  std::string GetFfrAlgorithmType () const;

  /**
   * Set an attribute for the FFR algorithm to be created.
   *
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetFfrAlgorithmAttribute (std::string n, const AttributeValue &v);

  /**
   * Set the type of handover algorithm to be used by eNodeB devices.
   *
   * \param type type of handover algorithm, must be a type name of any class
   *             inheriting from ns3::NistLteHandoverAlgorithm, for example:
   *             "ns3::NistNoOpHandoverAlgorithm"
   *
   * Equivalent with setting the `HandoverAlgorithm` attribute.
   */
  void SetHandoverAlgorithmType (std::string type);

  /**
   *
   * \return the handover algorithm type
   */
  std::string GetHandoverAlgorithmType () const;

  /**
   * Set an attribute for the handover algorithm to be created.
   *
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetHandoverAlgorithmAttribute (std::string n, const AttributeValue &v);

  /**
   * Set an attribute for the eNodeB devices (NistLteEnbNetDevice) to be created.
   * 
   * \param n the name of the attribute.
   * \param v the value of the attribute
   */
  void SetEnbDeviceAttribute (std::string n, const AttributeValue &v);

  /** 
   * Set the type of antenna model to be used by eNodeB devices.
   * 
   * \param type type of antenna model, must be a type name of any class
   *             inheriting from ns3::AntennaModel, for example:
   *             "ns3::IsotropicAntennaModel"
   */
  void SetEnbAntennaModelType (std::string type);

  /**
   * Set an attribute for the eNodeB antenna model to be created.
   * 
   * \param n the name of the attribute.
   * \param v the value of the attribute
   */
  void SetEnbAntennaModelAttribute (std::string n, const AttributeValue &v);

  /**
   * Set an attribute for the UE devices (NistLteUeNetDevice) to be created.
   *
   * \param n the name of the attribute.
   * \param v the value of the attribute
   */
  void SetUeDeviceAttribute (std::string n, const AttributeValue &v);

  /** 
   * Set the type of antenna model to be used by UE devices.
   * 
   * \param type type of antenna model, must be a type name of any class
   *             inheriting from ns3::AntennaModel, for example:
   *             "ns3::IsotropicAntennaModel"
   */
  void SetUeAntennaModelType (std::string type);

  /**
   * Set an attribute for the UE antenna model to be created.
   * 
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetUeAntennaModelAttribute (std::string n, const AttributeValue &v);

  /** 
   * Set the type of spectrum channel to be used in both DL and UL.
   *
   * \param type type of spectrum channel model, must be a type name of any
   *             class inheriting from ns3::SpectrumChannel, for example:
   *             "ns3::MultiModelSpectrumChannel"
   */
  void SetSpectrumChannelType (std::string type);

  /**
   * Set an attribute for the spectrum channel to be created (both DL and UL).
   * 
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetSpectrumChannelAttribute (std::string n, const AttributeValue &v);

  /**
   * Create a set of eNodeB devices.
   *
   * \param c the node container where the devices are to be installed
   * \return the NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer InstallEnbDevice (NodeContainer c);

  /**
   * Create a set of UE devices.
   *
   * \param c the node container where the devices are to be installed
   * \return the NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer InstallUeDevice (NodeContainer c);

  /**
   * \brief Enables automatic attachment of a set of UE devices to a suitable
   *        cell using Idle mode initial cell selection procedure.
   * \param ueDevices the set of UE devices to be attached
   *
   * By calling this, the UE will start the initial cell selection procedure at
   * the beginning of simulation. In addition, the function also instructs each
   * UE to immediately enter CONNECTED mode and activates the default EPS
   * bearer.
   *
   * If this function is called when the UE is in a situation where entering
   * CONNECTED mode is not possible (e.g. before the simulation begin), then the
   * UE will attempt to connect at the earliest possible time (e.g. after it
   * camps to a suitable cell).
   *
   * Note that this function can only be used in EPC-enabled simulation.
   */
  void Attach (NetDeviceContainer ueDevices);

  /**
   * \brief Enables automatic attachment of a UE device to a suitable cell
   *        using Idle mode initial cell selection procedure.
   * \param ueDevice the UE device to be attached
   *
   * By calling this, the UE will start the initial cell selection procedure at
   * the beginning of simulation. In addition, the function also instructs the
   * UE to immediately enter CONNECTED mode and activates the default EPS
   * bearer.
   *
   * If this function is called when the UE is in a situation where entering
   * CONNECTED mode is not possible (e.g. before the simulation begin), then the
   * UE will attempt to connect at the earliest possible time (e.g. after it
   * camps to a suitable cell).
   *
   * Note that this function can only be used in EPC-enabled simulation.
   */
  void Attach (Ptr<NetDevice> ueDevice);

  /**
   * \brief Manual attachment of a set of UE devices to the network via a given
   *        eNodeB.
   * \param ueDevices the set of UE devices to be attached
   * \param enbDevice the destination eNodeB device
   *
   * In addition, the function also instructs each UE to immediately enter
   * CONNECTED mode and activates the default EPS bearer.
   *
   * The function can be used in both LTE-only and EPC-enabled simulations.
   * Note that this function will disable Idle mode initial cell selection
   * procedure.
   */
  void Attach (NetDeviceContainer ueDevices, Ptr<NetDevice> enbDevice);

  /**
   * \brief Manual attachment of a UE device to the network via a given eNodeB.
   * \param ueDevice the UE device to be attached
   * \param enbDevice the destination eNodeB device
   *
   * In addition, the function also instructs the UE to immediately enter
   * CONNECTED mode and activates the default EPS bearer.
   *
   * The function can be used in both LTE-only and EPC-enabled simulations.
   * Note that this function will disable Idle mode initial cell selection
   * procedure.
   */
  void Attach (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice);

  /** 
   * \brief Manual attachment of a set of UE devices to the network via the
   *        closest eNodeB (with respect to distance) among those in the set.
   * \param ueDevices the set of UE devices to be attached
   * \param enbDevices the set of eNodeB devices to be considered
   * 
   * This function finds among the eNodeB set the closest eNodeB for each UE,
   * and then invokes manual attachment between the pair.
   * 
   * Users are encouraged to use automatic attachment (Idle mode cell selection)
   * instead of this function.
   * 
   * \sa NistLteHelper::Attach(NetDeviceContainer ueDevices);
   */
  void AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer enbDevices);

  /** 
   * \brief Manual attachment of a UE device to the network via the closest
   *        eNodeB (with respect to distance) among those in the set.
   * \param ueDevice the UE device to be attached
   * \param enbDevices the set of eNodeB devices to be considered
   *
   * This function finds among the eNodeB set the closest eNodeB for the UE,
   * and then invokes manual attachment between the pair.
   * 
   * Users are encouraged to use automatic attachment (Idle mode cell selection)
   * instead of this function.
   *
   * \sa NistLteHelper::Attach(Ptr<NetDevice> ueDevice);
   */
  void AttachToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices);

  /**
   * Activate a dedicated EPS bearer on a given set of UE devices.
   *
   * \param ueDevices the set of UE devices
   * \param bearer the characteristics of the bearer to be activated
   * \param tft the Traffic Flow Template that identifies the traffic to go on this bearer
   */
  uint8_t ActivateDedicatedNistEpsBearer (NetDeviceContainer ueDevices, NistEpsBearer bearer, Ptr<NistEpcTft> tft);

  /**
   * Activate a dedicated EPS bearer on a given UE device.
   *
   * \param ueDevice the UE device
   * \param bearer the characteristics of the bearer to be activated
   * \param tft the Traffic Flow Template that identifies the traffic to go on this bearer.
   */
  uint8_t ActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice, NistEpsBearer bearer, Ptr<NistEpcTft> tft);

  /**
   *  \brief Manually trigger dedicated bearer de-activation at specific simulation time
   *  \param ueDevice the UE on which dedicated bearer to be de-activated must be of the type NistLteUeNetDevice
   *  \param enbDevice eNB, must be of the type NistLteEnbNetDevice
   *  \param bearerId Bearer Identity which is to be de-activated
   *
   *  \warning Requires the use of EPC mode. See SetNistEpcHelper() method.
   */

  void DeActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice, uint8_t bearerId);

  /**
   * Activate a dedicated EPS bearer on a given set of UE devices.
   *
   * \param ueDevices the set of UE devices
   * \param tft the Traffic Flow Template that identifies the traffic to go on this bearer
   */
  void ActivateSidelinkBearer (NetDeviceContainer ueDevices, Ptr<NistSlTft> tft);

  /**
   * Deactivate a sidelink bearer on a given set of UE devices.
   *
   * \param ueDevices the set of UE devices
   * \param tft Sidelink bearer information which is to be de-activated
   */
  void DeactivateSidelinkBearer (NetDeviceContainer ueDevices, Ptr<NistSlTft> tft);

  /**
   * Activate a sidelink bearer on a given UE device.
   *
   * \param ueDevice the UE device
   * \param tft the Traffic Flow Template that identifies the traffic to go on this bearer.
   */
  void ActivateSidelinkBearer (Ptr<NetDevice> ueDevice, Ptr<NistSlTft> tft);

  /**
   *  \brief Manually trigger sidelink bearer de-activation at specific simulation time
   *  \param ueDevice the UE on which sidelink bearer to be de-activated must be of the type NistLteUeNetDevice
   *  \param tft Sidelink bearer information which is to be de-activated
   *
   *  \warning Requires the use of EPC mode. See SetNistEpcHelper() method.
   */

  void DeactivateSidelinkBearer (Ptr<NetDevice> ueDevice, Ptr<NistSlTft> tft);
  
 /**
   * Activate discovery for given UEs for certain applications
   * 
   * \param ueDevices the set of UE devices
   * \param apps the applications to start
   * \param rxtx the interest in monitoring or announcing (0 for rx and 1 for tx)
   */
  void StartDiscovery (NetDeviceContainer ueDevices, std::list<uint32_t> apps, bool rxtx);

  /**
   * Deactivate discovery for given UEs for certain applications
   *
   * \param ueDevices the set of UE devices
   * \param apps the applicaions to stop
   * \param rxtx the interest in monitoring or announcing (0 for rx and 1 for tx)
   */
  void StopDiscovery (NetDeviceContainer ueDevices, std::list<uint32_t> apps, bool rxtx);

  /**
   *  Activate discovery for one UE for given applications
   *
   * \param ueDevice the UE device
   * \param apps the applications to start
   * \param rxtx the interest in monitoring or announcing (0 for rx and 1 for tx)
   */
  void StartDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> apps, bool rxtx);

  /**
   *  Deactivate discovery for one UE for given applications
   *  \param ueDevice the UE device
   *  \param apps the applications to stop
   *  \param rxtx the interest in monitoring or announcing (0 for rx and 1 for tx)
   */
  void StopDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> apps, bool rxtx);

  /**
   * Create an X2 interface between all the eNBs in a given set.
   *
   * \param enbNodes the set of eNB nodes
   */
  void AddX2Interface (NodeContainer enbNodes);

  /**
   * Create an X2 interface between two eNBs.
   *
   * \param enbNode1 one eNB of the X2 interface
   * \param enbNode2 the other eNB of the X2 interface
   */
  void AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2);

  /**
   * Manually trigger an X2-based handover.
   *
   * \param hoTime when the handover shall be initiated
   * \param ueDev the UE that hands off, must be of the type NistLteUeNetDevice
   * \param sourceEnbDev source eNB, must be of the type NistLteEnbNetDevice
   *                     (originally the UE is attached to this eNB)
   * \param targetEnbDev target eNB, must be of the type NistLteEnbNetDevice
   *                     (the UE would be connected to this eNB after the
   *                     handover)
   *
   * \warning Requires the use of EPC mode. See SetNistEpcHelper() method
   */
  void HandoverRequest (Time hoTime, Ptr<NetDevice> ueDev,
                        Ptr<NetDevice> sourceEnbDev, Ptr<NetDevice> targetEnbDev);


  /** 
   * Activate a Data Radio Bearer on a given UE devices (for LTE-only simulation).
   * 
   * \param ueDevices the set of UE devices
   * \param bearer the characteristics of the bearer to be activated
   */
  void ActivateDataRadioBearer (NetDeviceContainer ueDevices,  NistEpsBearer bearer);

  /** 
   * Activate a Data Radio Bearer on a UE device (for LTE-only simulation).
   * This method will schedule the actual activation
   * the bearer so that it happens after the UE got connected.
   * 
   * \param ueDevice the UE device
   * \param bearer the characteristics of the bearer to be activated
   */
  void ActivateDataRadioBearer (Ptr<NetDevice> ueDevice,  NistEpsBearer bearer);

  /** 
   * Set the type of fading model to be used in both DL and UL.
   * 
   * \param type type of fading model, must be a type name of any class
   *             inheriting from ns3::SpectrumPropagationLossModel, for
   *             example: "ns3::NistTraceFadingLossModel"
   */
  void SetFadingModel (std::string type);

  /**
   * Set an attribute for the fading model to be created (both DL and UL).
   *
   * \param n the name of the attribute
   * \param v the value of the attribute
   */
  void SetFadingModelAttribute (std::string n, const AttributeValue &v);

  /**
   * Enables full-blown logging for major components of the LENA architecture.
   */
  void EnableLogComponents (void);

  /**
   * Enables trace sinks for PHY, MAC, RLC and PDCP. To make sure all nodes are
   * traced, traces should be enabled once all UEs and eNodeBs are in place and
   * connected, just before starting the simulation.
   */
  void EnableTraces (void);

  /**
   * Enable trace sinks for PHY layer.
   */
  void EnablePhyTraces (void);

  /**
   * Enable trace sinks for DL PHY layer.
   */
  void EnableDlPhyTraces (void);

  /**
   * Enable trace sinks for UL PHY layer.
   */
  void EnableUlPhyTraces (void);

  /**
   * Enable trace sinks for SL PHY layer.
   */
  void EnableSlPhyTraces (void);

  /**
   * Enable trace sinks for DL transmission PHY layer.
   */
  void EnableDlTxPhyTraces (void);

  /**
   * Enable trace sinks for UL transmission PHY layer.
   */
  void EnableUlTxPhyTraces (void);

  /**
   * Enable trace sinks for SL transmission PHY layer.
   */
  void EnableSlTxPhyTraces (void);

  /**
   * Enable trace sinks for DL reception PHY layer.
   */
  void EnableDlRxPhyTraces (void);

  /**
   * Enable trace sinks for UL reception PHY layer.
   */
  void EnableUlRxPhyTraces (void);

  /**
   * Enable trace sinks for SL reception PHY layer.
   */
  void EnableSlRxPhyTraces (void);

  /**
   * Enable trace sinks for SL reception PHY layer.
   */
  void EnableSlPscchRxPhyTraces (void);

  /**
   * Enable trace sinks for MAC layer.
   */
  void EnableMacTraces (void);

  /**
   * Enable trace sinks for DL MAC layer.
   */
  void EnableDlMacTraces (void);

  /**
   * Enable trace sinks for UL MAC layer.
   */
  void EnableUlMacTraces (void);

  /**
   * Enable trace sinks for SL UE MAC layer.
   */
  void EnableSlUeMacTraces (void);

  /**
   * Enable trace sinks for SL UE MAC layer.
   */
  void EnableSlSchUeMacTraces (void);

  /**
   * Enable trace sinks for RLC layer.
   */
  void EnableRlcTraces (void);

  /** 
   * 
   * \return the RLC stats calculator object
   */
  Ptr<NistRadioBearerStatsCalculator> GetRlcStats (void);

  /**
   * Enable trace sinks for PDCP layer
   */
  void EnablePdcpTraces (void);

  /** 
   * 
   * \return the PDCP stats calculator object
   */
  Ptr<NistRadioBearerStatsCalculator> GetPdcpStats (void);

  /**
   * Assign a fixed random variable stream number to the random variables used.
   *
   * The InstallEnbDevice() or InstallUeDevice method should have previously
   * been called by the user on the given devices.
   *
   * If NistTraceFadingLossModel has been set as the fading model type, this method
   * will also assign a stream number to it, if none has been assigned before.
   *
   * \param c NetDeviceContainer of the set of net devices for which the
   *          NistLteNetDevice should be modified to use a fixed stream
   * \param stream first stream index to use
   * \return the number of stream indices (possibly zero) that have been assigned
  */
  int64_t AssignStreams (NetDeviceContainer c, int64_t stream);

  /**
   * Deploys the Sidelink configuration to the eNodeBs
   * \param enbDevices List of devices where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
  void InstallSidelinkConfiguration (NetDeviceContainer enbDevices, Ptr<LteEnbRrcSl> slConfiguration);

  /**
   * Deploys the Sidelink configuration to the eNodeB
   * \param enbDevice The eNodeB where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
  void InstallSidelinkConfiguration (Ptr<NetDevice> enbDevice, Ptr<LteEnbRrcSl> slConfiguration);

  /**
   * Deploys the Sidelink configuration to the UEs
   * \param ueDevices List of devices where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
  void InstallSidelinkConfiguration (NetDeviceContainer ueDevices, Ptr<LteUeRrcSl> slConfiguration);

  /**
   * Deploys the Sidelink configuration to the Ue
   * \param ueDevice The UE where to configure sidelink
   * \param slConfiguration Sidelink configuration
   */
  void InstallSidelinkConfiguration (Ptr<NetDevice> ueDevice, Ptr<LteUeRrcSl> slConfiguration);

  /**
   * Compute the RSRP between the given nodes for the given propagation loss model
   * This code is derived from the multi-model-spectrum-channel class. It can be used for both uplink and downlink
   * \param propagationLoss The loss model
   * \param psd The power spectral density of the transmitter
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   * \return The RSRP 
   */
  double DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, Ptr<SpectrumValue> psd, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);

  /**
   * Compute the RSRP between the given nodes for the given propagation loss model
   * This code is derived from the multi-model-spectrum-channel class. It can be used for both uplink and downlink
   * \param propagationLoss The loss model
   * \param txPower The transmit power
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   * \return The RSRP 
   */
  double DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);

  /**
   * Computes the S-RSRP between 2 UEs. Information about the uplink frequency and band is necessary  to be able to call the function before the simulation starts.
   * \param txPower Transmit power for the reference signal
   * \param ulEarfcn Uplink frequency
   * \param ulBandwidth Uplink bandwidth
   * \param txDevice Transmitter UE
   * \param rxDevice Receiver UE
   * \return RSRP value
   */
  double CalcSidelinkRsrp (double txPower, double ulEarfcn, double ulBandwidth, Ptr<NetDevice> txDevice, Ptr<NetDevice> rxDevice);

  /**
   * Computes the RSRP between a transmitter UE and a receiver UE as defined in TR 36.843. Information about the uplink frequency and band is necessary  to be able to call the function before the simulation starts. 
   * \param txPower Transmit power for the reference signal
   * \param ulEarfcn Uplink frequency
   * \param txDevice Transmitter UE
   * \param rxDevice Receiver UE
   * \return RSRP value
   */
  double CalcSidelinkRsrpEval (double txPower, double ulEarfcn, Ptr<NetDevice> txDevice, Ptr<NetDevice> rxDevice);

protected:
  // inherited from Object
  virtual void DoInitialize (void);

private:
  /**
   * Create an eNodeB device (NistLteEnbNetDevice) on the given node.
   * \param n the node where the device is to be installed
   * \return pointer to the created device
   */
  Ptr<NetDevice> InstallSingleEnbDevice (Ptr<Node> n);

  /**
   * Create a UE device (NistLteUeNetDevice) on the given node
   * \param n the node where the device is to be installed
   * \return pointer to the created device
   */
  Ptr<NetDevice> InstallSingleUeDevice (Ptr<Node> n);

  /**
   * The actual function to trigger a manual handover.
   * \param ueDev the UE that hands off, must be of the type NistLteUeNetDevice
   * \param sourceEnbDev source eNB, must be of the type NistLteEnbNetDevice
   *                     (originally the UE is attached to this eNB)
   * \param targetEnbDev target eNB, must be of the type NistLteEnbNetDevice
   *                     (the UE would be connected to this eNB after the
   *                     handover)
   *
   * This method is normally scheduled by HandoverRequest() to run at a specific
   * time where a manual handover is desired by the simulation user.
   */
  void DoHandoverRequest (Ptr<NetDevice> ueDev,
                          Ptr<NetDevice> sourceEnbDev,
                          Ptr<NetDevice> targetEnbDev);


  /**
   *  \brief The actual function to trigger a manual bearer de-activation
   *  \param ueDevice the UE on which bearer to be de-activated must be of the type NistLteUeNetDevice
   *  \param enbDevice eNB, must be of the type NistLteEnbNetDevice
   *  \param bearerId Bearer Identity which is to be de-activated
   *
   *  This method is normally scheduled by DeActivateDedicatedNistEpsBearer() to run at a specific
   *  time when a manual bearer de-activation is desired by the simulation user.
   */
  void DoDeActivateDedicatedNistEpsBearer (Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice, uint8_t bearerId);


  /// The downlink LTE channel used in the simulation.
  Ptr<SpectrumChannel> m_downlinkChannel;
  /// The uplink LTE channel used in the simulation.
  Ptr<SpectrumChannel> m_uplinkChannel;
  /// The path loss model used in the downlink channel.
  Ptr<Object> m_downlinkPathlossModel;
  /// The path loss model used in the uplink channel.
  Ptr<Object> m_uplinkPathlossModel;

  /// Factory of MAC scheduler object.
  ObjectFactory m_schedulerFactory;

  /// Factory of MAC UE scheduler object.
  ObjectFactory m_UlschedulerFactory;

  /// Factory of FFR (frequency reuse) algorithm object.
  ObjectFactory m_ffrAlgorithmFactory;
  /// Factory of handover algorithm object.
  ObjectFactory m_handoverAlgorithmFactory;
  /// Factory of NistLteEnbNetDevice objects.
  ObjectFactory m_enbNetDeviceFactory;
  /// Factory of antenna object for eNodeB.
  ObjectFactory m_enbAntennaModelFactory;
  /// Factory for NistLteUeNetDevice objects.
  ObjectFactory m_ueNetDeviceFactory;
  /// Factory of antenna object for UE.
  ObjectFactory m_ueAntennaModelFactory;
  /// Factory of path loss model object for the downlink channel.
  ObjectFactory m_dlPathlossModelFactory;
  /// Factory of path loss model object for the uplink channel.
  ObjectFactory m_ulPathlossModelFactory;
  /// Factory of both the downlink and uplink LTE channels.
  ObjectFactory m_channelFactory;

  /// Name of fading model type, e.g., "ns3::NistTraceFadingLossModel".
  std::string m_fadingModelType;
  /// Factory of fading model object for both the downlink and uplink channels.
  ObjectFactory m_fadingModelFactory;
  /// The fading model used in both the downlink and uplink channels.
  Ptr<SpectrumPropagationLossModel> m_fadingModule;
  /**
   * True if a random variable stream number has been assigned for the fading
   * model. Used to prevent such assignment to be done more than once.
   */
  bool m_fadingStreamsAssigned;

  /// Container of PHY layer statistics.
  Ptr<NistPhyStatsCalculator> m_phyStats;
  /// Container of PHY layer statistics related to transmission.
  Ptr<NistPhyTxStatsCalculator> m_phyTxStats;
  /// Container of PHY layer statistics related to reception.
  Ptr<NistPhyRxStatsCalculator> m_phyRxStats;
  /// Container of MAC layer statistics.
  Ptr<NistMacStatsCalculator> m_macStats;
  /// Container of RLC layer statistics.
  Ptr<NistRadioBearerStatsCalculator> m_rlcStats;
  /// Container of PDCP layer statistics.
  Ptr<NistRadioBearerStatsCalculator> m_pdcpStats;
  /// Connects RLC and PDCP statistics containers to appropriate trace sources
  NistRadioBearerStatsConnector m_radioBearerStatsConnector;

  /**
   * Helper which provides implementation of core network. Initially empty
   * (i.e., LTE-only simulation without any core network) and then might be
   * set using SetNistEpcHelper().
   */
  Ptr<NistEpcHelper> m_epcHelper;

  /**
   * Keep track of the number of IMSI allocated. Increases by one every time a
   * new UE is installed (by InstallSingleUeDevice()). The first UE will have
   * an IMSI of 1. The maximum number of UE is 2^64 (~4.2e9).
   */
  uint64_t m_imsiCounter;
  /**
   * Keep track of the number of cell ID allocated. Increases by one every time
   * a new eNodeB is installed (by InstallSingleEnbDevice()). The first eNodeB
   * will have a cell ID of 1. The maximum number of eNodeB is 65535.
   */
  uint16_t m_cellIdCounter;

  /**
   * The `UseIdealRrc` attribute. If true, LteRrcProtocolIdeal will be used for
   * RRC signaling. If false, LteRrcProtocolReal will be used.
   */
  bool m_useIdealRrc;
  /**
   * The `AnrEnabled` attribute. Activate or deactivate Automatic Neighbour
   * Relation function.
   */
  bool m_isAnrEnabled;
  /**
   * The `UsePdschForCqiGeneration` attribute. If true, DL-CQI will be
   * calculated from PDCCH as signal and PDSCH as interference. If false,
   * DL-CQI will be calculated from PDCCH as signal and PDCCH as interference.
   */
  bool m_usePdschForCqiGeneration;

  /**
   * The `UseSidelink` attribute. If true, the UEs will contain additional 
   * spectrum phy model to receive sidelink communication
   */
  bool m_useSidelink;

  /** 
   * This attribute check if the UEs are allowed to perform ProSe direct discovery
   * Annoncements and Monitoring requests are not implemented, 
   * only direct discovery messages are exchanged
   */
  bool m_useDiscovery;

  /**
   * The `UseSameUlDlPropagationCondition` attribute. If true, the UEs will have 
   * the same shadowing and LOS condition for both UL and DL
   * False per default
   */
  bool m_sameUlDlPropagationCondition;

 /** Enables/Disables eNB physical layer upon creation
 *   True per defaul
 */
  bool m_EnbEnablePhyLayer;

}; // end of `class NistLteHelper`


} // namespace ns3



#endif // NIST_LTE_HELPER_H
