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
 * Modified by: Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
 *              Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_HELPER_H
#define LTE_HELPER_H

#include "mac-stats-calculator.h"
#include "phy-rx-stats-calculator.h"
#include "phy-stats-calculator.h"
#include "phy-tx-stats-calculator.h"
#include "radio-bearer-stats-calculator.h"
#include "radio-bearer-stats-connector.h"
#include "rrc-stats-calculator.h"

#include <ns3/component-carrier-enb.h>
#include <ns3/config.h>
#include <ns3/epc-tft.h>
#include <ns3/eps-bearer.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/lte-sl-tft.h>
#include <ns3/lte-spectrum-phy.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/mobility-model.h>
#include <ns3/names.h>
#include <ns3/net-device-container.h>
#include <ns3/net-device.h>
#include <ns3/node-container.h>
#include <ns3/node.h>
#include <ns3/simulator.h>

#include <cfloat>
#include <map>

namespace ns3
{

class LteUePhy;
class LteEnbPhy;
class SpectrumChannel;
class EpcHelper;
class PropagationLossModel;
class SpectrumPropagationLossModel;

/**
 * \ingroup lte
 *
 * Creation and configuration of LTE entities. One LteHelper instance is
 * typically enough for an LTE simulation. To create it:
 *
 *     Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
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
 *     * CCM (Carrier Component Manager)
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
class LteHelper : public Object
{
  public:
    LteHelper();
    ~LteHelper() override;

    /**
     * Register this type.
     * \return The object TypeId.
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * Set the EpcHelper to be used to setup the EPC network in
     * conjunction with the setup of the LTE radio access network.
     *
     * \note if no EpcHelper is ever set, then LteHelper will default
     * to creating an LTE-only simulation with no EPC, using LteRlcSm as
     * the RLC model, and without supporting any IP networking. In other
     * words, it will be a radio-level simulation involving only LTE PHY
     * and MAC and the FF Scheduler, with a saturation traffic model for
     * the RLC.
     *
     * \param h A pointer to the EpcHelper to be used
     */
    void SetEpcHelper(Ptr<EpcHelper> h);

    /**
     * Set the type of path loss model to be used for both DL and UL channels.
     *
     * \param type The type of path loss model, must be a type name of any class
     *             inheriting from ns3::PropagationLossModel, for example:
     *             "ns3::FriisPropagationLossModel"
     */
    void SetPathlossModelType(TypeId type);

    /**
     * Set an attribute for the path loss models to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetPathlossModelAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of scheduler to be used by eNodeB devices.
     *
     * \param type The type of scheduler, must be a type name of any class
     *             inheriting from ns3::FfMacScheduler, for example:
     *             "ns3::PfFfMacScheduler"
     *
     * Equivalent with setting the `Scheduler` attribute.
     */
    void SetSchedulerType(std::string type);

    /**
     *
     * \return The scheduler type
     */
    std::string GetSchedulerType() const;

    /**
     * Set an attribute for the scheduler to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetSchedulerAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of FFR algorithm to be used by eNodeB devices.
     *
     * \param type The type of FFR algorithm, must be a type name of any class
     *             inheriting from ns3::LteFfrAlgorithm, for example:
     *             "ns3::LteFrNoOpAlgorithm"
     *
     * Equivalent with setting the `FfrAlgorithm` attribute.
     */
    void SetFfrAlgorithmType(std::string type);

    /**
     *
     * \return The FFR algorithm type
     */
    std::string GetFfrAlgorithmType() const;

    /**
     * Set an attribute for the FFR algorithm to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetFfrAlgorithmAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of handover algorithm to be used by eNodeB devices.
     *
     * \param type The type of handover algorithm, must be a type name of any class
     *             inheriting from ns3::LteHandoverAlgorithm, for example:
     *             "ns3::NoOpHandoverAlgorithm"
     *
     * Equivalent with setting the `HandoverAlgorithm` attribute.
     */
    void SetHandoverAlgorithmType(std::string type);

    /**
     *
     * \return The handover algorithm type
     */
    std::string GetHandoverAlgorithmType() const;

    /**
     * Set an attribute for the handover algorithm to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetHandoverAlgorithmAttribute(std::string n, const AttributeValue& v);

    /**
     * Set an attribute for the eNodeB devices (LteEnbNetDevice) to be created.
     *
     * \param n The name of the attribute.
     * \param v The value of the attribute
     */
    void SetEnbDeviceAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of antenna model to be used by eNodeB devices.
     *
     * \param type The type of antenna model, must be a type name of any class
     *             inheriting from ns3::AntennaModel, for example:
     *             "ns3::IsotropicAntennaModel"
     */
    void SetEnbAntennaModelType(std::string type);

    /**
     * Set an attribute for the eNodeB antenna model to be created.
     *
     * \param n The name of the attribute.
     * \param v The value of the attribute
     */
    void SetEnbAntennaModelAttribute(std::string n, const AttributeValue& v);

    /**
     * Set an attribute for the UE devices (LteUeNetDevice) to be created.
     *
     * \param n The name of the attribute.
     * \param v The value of the attribute
     */
    void SetUeDeviceAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of antenna model to be used by UE devices.
     *
     * \param type The type of antenna model, must be a type name of any class
     *             inheriting from ns3::AntennaModel, for example:
     *             "ns3::IsotropicAntennaModel"
     */
    void SetUeAntennaModelType(std::string type);

    /**
     * Set an attribute for the UE antenna model to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetUeAntennaModelAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of spectrum channel to be used in both DL and UL.
     *
     * \param type type of spectrum channel model, must be a type name of any
     *             class inheriting from ns3::SpectrumChannel, for example:
     *             "ns3::MultiModelSpectrumChannel"
     */
    void SetSpectrumChannelType(std::string type);

    /**
     * Set an attribute for the spectrum channel to be created (both DL and UL).
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetSpectrumChannelAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of carrier component algorithm to be used by eNodeB devices.
     *
     * \param type The type of carrier component manager
     *
     */
    void SetEnbComponentCarrierManagerType(std::string type);

    /**
     *
     * \return The carrier enb component carrier manager type
     */
    std::string GetEnbComponentCarrierManagerType() const;

    /**
     * Set an attribute for the enb component carrier manager to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetEnbComponentCarrierManagerAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of Component Carrier Manager to be used by Ue devices.
     *
     * \param type The type of UE Component Carrier Manager
     *
     */
    void SetUeComponentCarrierManagerType(std::string type);

    /**
     *
     * \return The carrier UE component carrier manager type
     */
    std::string GetUeComponentCarrierManagerType() const;

    /**
     * Set an attribute for the UE component carrier manager to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetUeComponentCarrierManagerAttribute(std::string n, const AttributeValue& v);

    /**
     * Set the type of sidelink controller to be used by Ue devices.
     *
     * \param type The type of sidelink controller
     *
     */
    void SetSlUeControllerType(std::string type);

    /**
     *
     * \return The sidelink controller type
     */
    std::string GetSlUeControllerType() const;

    /**
     * Set an attribute for the sidelink controller to be created.
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetSlUeControllerAttribute(std::string n, const AttributeValue& v);

    /**
     *
     * \return the upkink pathloss model
     */
    Ptr<Object> GetUplinkPathlossModel() const;

    /**
     *
     * \return the downlink pathloss model
     */
    Ptr<Object> GetDownlinkPathlossModel() const;

    /**
     * Create a set of eNodeB devices.
     *
     * \param c The node container where the devices are to be installed
     * \return The NetDeviceContainer with the newly created devices
     */
    NetDeviceContainer InstallEnbDevice(NodeContainer c);

    /**
     * Create a set of UE devices.
     *
     * \param c The node container where the devices are to be installed
     * \return The NetDeviceContainer with the newly created devices
     */
    NetDeviceContainer InstallUeDevice(NodeContainer c);

    /**
     * \brief Enables automatic attachment of a set of UE devices to a suitable
     *        cell using Idle mode initial cell selection procedure.
     * \param ueDevices The set of UE devices to be attached
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
    void Attach(NetDeviceContainer ueDevices);

    /**
     * \brief Enables automatic attachment of a UE device to a suitable cell
     *        using Idle mode initial cell selection procedure.
     * \param ueDevice The UE device to be attached
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
    void Attach(Ptr<NetDevice> ueDevice);

    /**
     * \brief Manual attachment of a set of UE devices to the network via a given
     *        eNodeB.
     * \param ueDevices The set of UE devices to be attached
     * \param enbDevice The destination eNodeB device
     *
     * In addition, the function also instructs each UE to immediately enter
     * CONNECTED mode and activates the default EPS bearer.
     *
     * The function can be used in both LTE-only and EPC-enabled simulations.
     * Note that this function will disable Idle mode initial cell selection
     * procedure.
     */
    void Attach(NetDeviceContainer ueDevices, Ptr<NetDevice> enbDevice);

    /**
     * \brief Manual attachment of a UE device to the network via a given eNodeB.
     * \param ueDevice the UE device to be attached
     * \param enbDevice the destination eNodeB device
     * \param componentCarrierId the destination eNodeB component carrier
     *
     * In addition, the function also instructs the UE to immediately enter
     * CONNECTED mode and activates the default EPS bearer.
     *
     * The function can be used in both LTE-only and EPC-enabled simulations.
     * Note that this function will disable Idle mode initial cell selection
     * procedure.
     */
    void Attach(Ptr<NetDevice> ueDevice, Ptr<NetDevice> enbDevice, uint8_t componentCarrierId = 0);

    /**
     * \brief Manual attachment of a set of UE devices to the network via the
     *        closest eNodeB (with respect to distance) among those in the set.
     * \param ueDevices The set of UE devices to be attached
     * \param enbDevices The set of eNodeB devices to be considered
     *
     * This function finds among the eNodeB set the closest eNodeB for each UE,
     * and then invokes manual attachment between the pair.
     *
     * Users are encouraged to use automatic attachment (Idle mode cell selection)
     * instead of this function.
     *
     * \sa LteHelper::Attach(NetDeviceContainer ueDevices);
     */
    void AttachToClosestEnb(NetDeviceContainer ueDevices, NetDeviceContainer enbDevices);

    /**
     * \brief Manual attachment of a UE device to the network via the closest
     *        eNodeB (with respect to distance) among those in the set.
     * \param ueDevice The UE device to be attached
     * \param enbDevices The set of eNodeB devices to be considered
     *
     * This function finds among the eNodeB set the closest eNodeB for the UE,
     * and then invokes manual attachment between the pair.
     *
     * Users are encouraged to use automatic attachment (Idle mode cell selection)
     * instead of this function.
     *
     * \sa LteHelper::Attach(Ptr<NetDevice> ueDevice);
     */
    void AttachToClosestEnb(Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices);

    /**
     * Activate a dedicated EPS bearer on a given set of UE devices.
     *
     * \param ueDevices The set of UE devices
     * \param bearer The characteristics of the bearer to be activated
     * \param tft The Traffic Flow Template that identifies the traffic to go on this bearer
     * \return The bearer ID
     */
    uint8_t ActivateDedicatedEpsBearer(NetDeviceContainer ueDevices,
                                       EpsBearer bearer,
                                       Ptr<EpcTft> tft);

    /**
     * Activate a dedicated EPS bearer on a given UE device.
     *
     * \param ueDevice the UE device
     * \param bearer the characteristics of the bearer to be activated
     * \param tft the Traffic Flow Template that identifies the traffic to go on this bearer.
     * \returns bearer ID
     */
    uint8_t ActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice, EpsBearer bearer, Ptr<EpcTft> tft);

    /**
     * \brief Manually trigger dedicated bearer de-activation at specific simulation time
     * \param ueDevice the UE on which dedicated bearer to be de-activated must be of the type
     * LteUeNetDevice
     * \param enbDevice eNB, must be of the type LteEnbNetDevice
     * \param bearerId Bearer Identity which is to be de-activated
     *
     * \warning Requires the use of EPC mode. See SetEpcHelper() method.
     */

    void DeActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice,
                                      Ptr<NetDevice> enbDevice,
                                      uint8_t bearerId);
    /**
     * Create an X2 interface between all the eNBs in a given set.
     *
     * \param enbNodes the set of eNB nodes
     */
    void AddX2Interface(NodeContainer enbNodes);

    /**
     * Create an X2 interface between two eNBs.
     *
     * \param enbNode1 First eNB of the X2 interface
     * \param enbNode2 The other eNB of the X2 interface
     */
    void AddX2Interface(Ptr<Node> enbNode1, Ptr<Node> enbNode2);

    /**
     * Manually trigger an X2-based handover.
     *
     * \param hoTime The time when the handover shall be initiated
     * \param ueDev The UE that hands off, must be of the type LteUeNetDevice
     * \param sourceEnbDev The source eNB, must be of the type LteEnbNetDevice
     *                     (originally the UE is attached to this eNB)
     * \param targetEnbDev The target eNB, must be of the type LteEnbNetDevice
     *                     (the UE would be connected to this eNB after the
     *                     handover)
     *
     * \warning Requires the use of EPC mode. See SetEpcHelper() method
     */
    void HandoverRequest(Time hoTime,
                         Ptr<NetDevice> ueDev,
                         Ptr<NetDevice> sourceEnbDev,
                         Ptr<NetDevice> targetEnbDev);

    /**
     * Manually trigger an X2-based handover.
     *
     * \param hoTime The time when the handover shall be initiated
     * \param ueDev The UE that hands off, must be of the type LteUeNetDevice
     * \param sourceEnbDev The source eNB, must be of the type LteEnbNetDevice
     *                     (originally the UE is attached to this eNB)
     * \param targetCellId The target CellId (the UE primary component carrier will
     *                     be connected to this cell after the handover)
     *
     * \warning Requires the use of EPC mode. See SetEpcHelper() method
     */
    void HandoverRequest(Time hoTime,
                         Ptr<NetDevice> ueDev,
                         Ptr<NetDevice> sourceEnbDev,
                         uint16_t targetCellId);

    /**
     * Activate a Data Radio Bearer on a given UE devices (for LTE-only simulation).
     *
     * \param ueDevices The set of UE devices
     * \param bearer The characteristics of the bearer to be activated
     */
    void ActivateDataRadioBearer(NetDeviceContainer ueDevices, EpsBearer bearer);

    /**
     * Activate a Data Radio Bearer on a UE device (for LTE-only simulation).
     * This method will schedule the actual activation
     * the bearer so that it happens after the UE got connected.
     *
     * \param ueDevice The UE device
     * \param bearer The characteristics of the bearer to be activated
     */
    void ActivateDataRadioBearer(Ptr<NetDevice> ueDevice, EpsBearer bearer);

    /**
     * Set the type of fading model to be used in both DL and UL.
     *
     * \param type The type of fading model, must be a type name of any class
     *             inheriting from ns3::SpectrumPropagationLossModel, for
     *             example: "ns3::TraceFadingLossModel"
     */
    void SetFadingModel(std::string type);

    /**
     * Set an attribute for the fading model to be created (both DL and UL).
     *
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetFadingModelAttribute(std::string n, const AttributeValue& v);

    /**
     * Enables full-blown logging for major components of the LENA architecture.
     */
    void EnableLogComponents();

    /**
     * Enables trace sinks for PHY, MAC, RLC and PDCP. To make sure all nodes are
     * traced, traces should be enabled once all UEs and eNodeBs are in place and
     * connected, just before starting the simulation.
     */
    void EnableTraces();

    /**
     * Enable trace sinks for PHY layer.
     */
    void EnablePhyTraces();

    /**
     * Enable trace sinks for DL PHY layer.
     */
    void EnableDlPhyTraces();

    /**
     * Enable trace sinks for UL PHY layer.
     */
    void EnableUlPhyTraces();

    /**
     * Enable trace sinks for DL transmission PHY layer.
     */
    void EnableDlTxPhyTraces();

    /**
     * Enable trace sinks for UL transmission PHY layer.
     */
    void EnableUlTxPhyTraces();

    /**
     * Enable trace sinks for DL reception PHY layer.
     */
    void EnableDlRxPhyTraces();

    /**
     * Enable trace sinks for UL reception PHY layer.
     */
    void EnableUlRxPhyTraces();

    /**
     * Enable trace sinks for MAC layer.
     */
    void EnableMacTraces();

    /**
     * Enable trace sinks for DL MAC layer.
     */
    void EnableDlMacTraces();

    /**
     * Enable trace sinks for UL MAC layer.
     */
    void EnableUlMacTraces();

    /**
     * Enable trace sinks for RLC layer.
     */
    void EnableRlcTraces();

    /**
     *
     * \return The RLC stats calculator object
     */
    Ptr<RadioBearerStatsCalculator> GetRlcStats();

    /**
     * Enable trace sinks for PDCP layer
     */
    void EnablePdcpTraces();

    /**
     *
     * \return The PDCP stats calculator object
     */
    Ptr<RadioBearerStatsCalculator> GetPdcpStats();

    /**
     * Assign a fixed random variable stream number to the random variables used.
     *
     * The InstallEnbDevice() or InstallUeDevice method should have previously
     * been called by the user on the given devices.
     *
     * If TraceFadingLossModel has been set as the fading model type, this method
     * will also assign a stream number to it, if none has been assigned before.
     *
     * If an EPC has been configured, streams will be assigned on the EPC
     * nodes as well via EpcHelper::AssignStreams ().
     *
     * \param c The NetDeviceContainer of the set of net devices for which the
     *          LteNetDevice should be modified to use a fixed stream
     * \param stream The first stream index to use
     * \return The number of stream indices (possibly zero) that have been assigned
     */
    int64_t AssignStreams(NetDeviceContainer c, int64_t stream);

    /**
     * \return A pointer to the SpectrumChannel instance used for the uplink
     */
    Ptr<SpectrumChannel> GetUplinkSpectrumChannel() const;

    /**
     * \return A pointer to the SpectrumChannel instance used for the downlink
     */
    Ptr<SpectrumChannel> GetDownlinkSpectrumChannel() const;

    /**
     * Disable eNB phy function.
     * Implemented for Sidelink out-of-coverage scenarios
     *
     *\param disableEnbPhy If true, eNB PHY will be deactivated.
     *                     If false eNB PHY will be remain active.
     */
    void DisableEnbPhy(bool disableEnbPhy);

    /**
     * Activate a Sidelink bearer on a given set of UE devices.
     *
     * \param ueDevices The set of UE devices
     * \param tft The Sidelink Traffic Flow Template that identifies the traffic to go on this
     * bearer
     */
    void ActivateSidelinkBearer(NetDeviceContainer ueDevices, Ptr<LteSlTft> tft);

    /**
     * Deactivate a Sidelink bearer on a given set of UE devices.
     *
     * \param ueDevices The set of UE devices
     * \param tft The Sidelink bearer information which is to be de-activated
     */
    void DeactivateSidelinkBearer(NetDeviceContainer ueDevices, Ptr<LteSlTft> tft);

    /**
     * Activate a Sidelink bearer on a given UE device.
     *
     * \param ueDevice The UE device
     * \param tft The Traffic Flow Template that identifies the traffic to go on this bearer.
     */
    void ActivateSidelinkBearer(Ptr<NetDevice> ueDevice, Ptr<LteSlTft> tft);

    /**
     *  \brief Manually trigger Sidelink bearer de-activation at specific simulation time
     *
     *  \param ueDevice The UE on which Sidelink bearer to be de-activated must be of the type
     * LteUeNetDevice \param tft The Sidelink bearer information which is to be de-activated
     *
     *  \warning Requires the use of EPC mode. See SetEpcHelper() method.
     */

    void DeactivateSidelinkBearer(Ptr<NetDevice> ueDevice, Ptr<LteSlTft> tft);

    /**
     * Enable trace sinks for Sidelink reception PHY layer.
     */
    void EnableSlRxPhyTraces();

    /**
     * Enable trace sinks for Sidelink reception PHY layer.
     */
    void EnableSlPscchRxPhyTraces();

    /**
     * Enable trace sinks for Sidelink PSCCH UE MAC layer.
     */
    void EnableSlPscchMacTraces();

    /**
     * Enable trace sinks for Sidelink PSSCH UE MAC layer.
     */
    void EnableSlPsschMacTraces();

    /**
     * Enable trace sinks for Discovery Announcement at MAC layer.
     */
    void EnableSlPsdchMacTraces();

    /**
     * Enable trace sinks for RRC layer
     */
    void EnableRrcTraces();

    /**
     * Enable trace sinks for ProSe discovery monitoring at RRC layer.
     */
    void EnableDiscoveryMonitoringRrcTraces();

    /**
     * Deploys the Sidelink configuration to the eNodeBs
     *
     * \param enbDevices The list of devices where to configure Sidelink
     * \param slConfiguration The Sidelink configuration
     */
    void InstallSidelinkConfiguration(NetDeviceContainer enbDevices,
                                      Ptr<LteSlEnbRrc> slConfiguration);

    /**
     * Deploys the Sidelink configuration to the eNodeB
     *
     * \param enbDevice The eNodeB where to configure Sidelink
     * \param slConfiguration The Sidelink configuration
     */
    void InstallSidelinkConfiguration(Ptr<NetDevice> enbDevice, Ptr<LteSlEnbRrc> slConfiguration);

    /**
     * Deploys the Sidelink configuration to the UEs
     *
     * \param ueDevices Th list of devices where to configure Sidelink
     * \param slConfiguration The Sidelink configuration
     */
    void InstallSidelinkConfiguration(NetDeviceContainer ueDevices,
                                      Ptr<LteSlUeRrc> slConfiguration);

    /**
     * Deploys the Sidelink configuration to the UE
     *
     * \param ueDevice The UE where to configure Sidelink
     * \param slConfiguration The Sidelink configuration
     */
    void InstallSidelinkConfiguration(Ptr<NetDevice> ueDevice, Ptr<LteSlUeRrc> slConfiguration);

    /**
     * Informs the PGW/SGW of a new Remote UE connected to a relay UE
     *
     * \param relayImsi The IMSI of the relay node
     * \param ueImsi The IMSI of the remote UE
     * \param ipv6Prefix The /64 prefix assigned to the remote UE
     */
    void RemoteUeContextConnected(uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8]);

    /**
     * Informs the PGW/SGW of a Remote UE that disconnected from a relay UE
     *
     * \param relayImsi The IMSI of the relay node
     * \param ueImsi The IMSI of the remote UE
     * \param ipv6Prefix The /64 prefix assigned to the remote UE
     */
    void RemoteUeContextDisconnected(uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8]);

    /**
     * \brief Enables Sidelink trace sinks for PHY, MAC, and RRC.
     *
     * Enables sidelink trace sinks at the PHY and MAC layer, and discovery
     * traces at the RRC layer.  This method is designed to complement
     * the on-network EnableTraces() method, which enables RLC and PDCP
     * traces in addition to uplink and downlink PHY and MAC traces (if
     * uplink and downlink are present).
     *
     * RLC and PDCP traces are not generated for Sidelink bearers.  If they
     * are implemented, this method should be extended to cover them.
     *
     * To make sure all nodes are traced, this method should
     * be called just before starting the simulation.  If no sidelinks are
     * defined, this method will generate a runtime error.
     *
     * \sa LteHelper::EnableTraces
     */
    void EnableSidelinkTraces();

  protected:
    // inherited from Object
    void DoInitialize() override;

  private:
    /**
     * Configure the component carriers
     *
     * \param ulEarfcn The uplink EARFCN - not control on the validity at this point
     * \param dlEarfcn The downlink EARFCN - not control on the validity at this point
     * \param ulbw The uplink bandwidth for the current CC
     * \param dlbw The downlink bandwidth for the current CC
     */
    void DoComponentCarrierConfigure(uint32_t ulEarfcn,
                                     uint32_t dlEarfcn,
                                     uint16_t ulbw,
                                     uint16_t dlbw);
    /**
     * Create an eNodeB device (LteEnbNetDevice) on the given node.
     * \param n The node where the device is to be installed
     * \return Pointer to the created device

     */
    Ptr<NetDevice> InstallSingleEnbDevice(Ptr<Node> n);

    /**
     * Create a UE device (LteUeNetDevice) on the given node
     * \param n The node where the device is to be installed
     * \return Pointer to the created device
     */
    Ptr<NetDevice> InstallSingleUeDevice(Ptr<Node> n);

    /**
     * The actual function to trigger a manual handover.
     * \param ueDev the UE that hands off, must be of the type LteUeNetDevice
     * \param sourceEnbDev source eNB, must be of the type LteEnbNetDevice
     *                     (originally the UE is attached to this eNB)
     * \param targetCellId The target CellId (the UE primary component carrier will
     *                     be connected to this cell after the handover)
     *
     * This method is normally scheduled by HandoverRequest() to run at a specific
     * time where a manual handover is desired by the simulation user.
     */
    void DoHandoverRequest(Ptr<NetDevice> ueDev,
                           Ptr<NetDevice> sourceEnbDev,
                           uint16_t targetCellId);

    /**
     * \brief The actual function to trigger a manual bearer de-activation
     * \param ueDevice the UE on which bearer to be de-activated must be of the type LteUeNetDevice
     * \param enbDevice eNB, must be of the type LteEnbNetDevice
     * \param bearerId Bearer Identity which is to be de-activated
     *
     * This method is normally scheduled by DeActivateDedicatedEpsBearer() to run at a specific
     * time when a manual bearer de-activation is desired by the simulation user.
     */
    void DoDeActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice,
                                        Ptr<NetDevice> enbDevice,
                                        uint8_t bearerId);

    /// Function that performs a channel model initialization of all component carriers
    void ChannelModelInitialization();

    /**
     * \brief This function create the component carrier based on provided configuration parameters
     */

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
    /// Factory of FFR (frequency reuse) algorithm object.
    ObjectFactory m_ffrAlgorithmFactory;
    /// Factory of handover algorithm object.
    ObjectFactory m_handoverAlgorithmFactory;
    /// Factory of enb component carrier manager object.
    ObjectFactory m_enbComponentCarrierManagerFactory;
    /// Factory of ue component carrier manager object.
    ObjectFactory m_ueComponentCarrierManagerFactory;
    /// Factory of ue sidelink controller object.
    ObjectFactory m_slUeControllerFactory;
    /// Factory of LteEnbNetDevice objects.
    ObjectFactory m_enbNetDeviceFactory;
    /// Factory of antenna object for eNodeB.
    ObjectFactory m_enbAntennaModelFactory;
    /// Factory for LteUeNetDevice objects.
    ObjectFactory m_ueNetDeviceFactory;
    /// Factory of antenna object for UE.
    ObjectFactory m_ueAntennaModelFactory;
    /// Factory of path loss model object.
    ObjectFactory m_pathlossModelFactory;
    /// Factory of both the downlink and uplink LTE channels.
    ObjectFactory m_channelFactory;

    /// Name of fading model type, e.g., "ns3::TraceFadingLossModel".
    std::string m_fadingModelType;
    /// Factory of fading model object for both the downlink and uplink channels.
    ObjectFactory m_fadingModelFactory;
    /// The fading model used in both the downlink and uplink channels.
    Ptr<SpectrumPropagationLossModel> m_fadingModel;
    /**
     * True if a random variable stream number has been assigned for the fading
     * model. Used to prevent such assignment to be done more than once.
     */
    bool m_fadingStreamsAssigned;

    /// Container of PHY layer statistics.
    Ptr<PhyStatsCalculator> m_phyStats;
    /// Container of PHY layer statistics related to transmission.
    Ptr<PhyTxStatsCalculator> m_phyTxStats;
    /// Container of PHY layer statistics related to reception.
    Ptr<PhyRxStatsCalculator> m_phyRxStats;
    /// Container of MAC layer statistics.
    Ptr<MacStatsCalculator> m_macStats;
    /// Container of RLC layer statistics.
    Ptr<RadioBearerStatsCalculator> m_rlcStats;
    /// Container of PDCP layer statistics.
    Ptr<RadioBearerStatsCalculator> m_pdcpStats;
    /// Container of RRC layer statistics.
    Ptr<RrcStatsCalculator> m_rrcStats;

    /// Connects RLC and PDCP statistics containers to appropriate trace sources
    RadioBearerStatsConnector m_radioBearerStatsConnector;

    /**
     * Helper which provides implementation of core network. Initially empty
     * (i.e., LTE-only simulation without any core network) and then might be
     * set using SetEpcHelper().
     */
    Ptr<EpcHelper> m_epcHelper;

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
     * The `UseCa` attribute. If true, Carrier Aggregation is enabled.
     * Hence, the helper will expect a valid component carrier map
     * If it is false, the component carrier will be created within the LteHelper
     * this is to maintain the backwards compatibility with user script
     */
    bool m_useCa;

    /**
     * This contains all the information about each component carrier
     */
    std::map<uint8_t, ComponentCarrier> m_componentCarrierPhyParams;

    /**
     * Number of component carriers that will be installed by default at eNodeB and UE devices.
     */
    uint16_t m_noOfCcs;

    /**
     * The `UseSidelink` attribute. If true, the UEs will be able
     * to do Sidelink communication
     */
    bool m_useSidelink;

    /**
     * This attribute in to disable LTE eNB phy for Sidelink out-of-coverage
     * communication
     */
    bool m_disableEnbPhy;

}; // end of `class LteHelper`

} // namespace ns3

#endif // LTE_HELPER_H
