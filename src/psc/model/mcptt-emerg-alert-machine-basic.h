/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef MCPTT_EMERG_ALERT_MACHINE_BASIC_H
#define MCPTT_EMERG_ALERT_MACHINE_BASIC_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttCallMachineGrp;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT emergency alert state machine described in
 * TS 24.379 v14.4.0.
 *
 * State E1: Not in emergency state
 *  - This state is the start state of this state machine.
 *  - The UE stays in this state while not in emergency state.
 *
 * State E1: Emergency state
 *  - This state exists for UE, when the UE has sent a "GROUP EMERGENCY ALERT" message.
 *
 * Timer TFE1 (Emergency Alert)
 *  - Default value: 30 seconds.
 *  - Maximum value: 60 seconds.
 *  - Cause of start: Receipt of "GROUP EMERGENCY ALERT" message.
 *  - Normarl stop: Receipt of "GROUP EMERGENCY ALERT CANCEL" message.
 *  - On expiry: Assume end of emergency state and remove assocated user from list.
 *
 * Timer TFE2 (Emergency Alert Retransmission)
 *  - Default value: 5 seconds.
 *  - Maximum value: 10 seconds.
 *  - Cause of start: Transmisstion of "GROUP EMERGENCY ALERT" message.
 *  - Normal stop: Transmission of "GROUP EMERGENCY ALERT CANCEL" message.
 *  - On expiry: Transmit "GROUP EMERGENCY ALERT" message.
 */
class McpttEmergAlertMachineBasic : public McpttEmergAlertMachine
{
public:
  /**
   * A struct used to store information about a user in an emergency.
   */
  struct EmergUser
  {
    uint32_t Id; //!< The MCPTT ID of the user.
    Vector Loc; //!< The location of the user.
    Ptr<McpttTimer> Tfe1; //!< The timer assocated with the emergency user.
    /**
     * The destructor of the EmergUser struct.
     */
    virtual ~EmergUser (void);
  };
  /**
   * Gets the type ID of the McpttEmergAlertMachineBasic class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the  class.
   * \param owner The owner of the machine.
   */
  McpttEmergAlertMachineBasic (Ptr<McpttCallMachineGrp> owner = 0);
  /**
   * \brief The destructor of the McpttEmergAlertMachineBasic class.
   */
  virtual ~McpttEmergAlertMachineBasic (void);
  /**
   * Indicates that the emergency alert should be canceled.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Gets the type ID of this McpttEmergAlertMachineBasic instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates if the machine is in the emergency state.
   * \returns True, if the machine is in the emergency state; otherwise, false.
   */
  virtual bool IsInEmergState (void) const;
  /**
   * Indicates if the machine has been started.
   * \returns True, if the machine has been started; false, otherwise.
   */
  virtual bool IsStarted (void) const;
  /**
   * Indicates if another user is in the emergency state.
   * \returns True, if another user is still in the emergency state; otherwise false.
   */
  virtual bool IsOtherInEmergState (void) const;
  /**
   * Receive a "GROUP EMERGENCY ALERT" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg);
  /**
   * Receive a "GROUP EMERGENCY ALERT CANCEL" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg);
  /**
   * Sends a call control message.
   * \param msg The message to send.
   */
  virtual void Send (const McpttCallMsg& msg);
  /**
   * Indicates to the machine that an emergency alert should be sent.
   */
  virtual void SendEmergAlert (void);
  /**
   * Sets the delay for timer TFE2.
   * \param delayTfe2 The delay to use.
   */
  virtual void SetDelayTfe2 (const Time& delayTfe2);
  /**
   * Starts the emergency alert machine.
   */
  virtual void Start (void);
  /**
   * Stops the emergency alert machine.
   */
  virtual void Stop (void);

protected:
  /**
   * Adds a user to list of users in emergency.
   * \param user The user to add.
   */
  virtual void AddEmergUser (const EmergUser& user);
  /**
   * Disposes of the McpttEmergAlertMachineBasic.
   */
  virtual void DoDispose (void);
  /**
   * Indicates expiration of TFE1.
   * \param userId The ID of the user whose TFE1 timer expired.
   */
  virtual void ExpiryOfTfe1 (uint32_t userId);
  /**
   * Indicates expiration of TFE2.
   */
  virtual void ExpiryOfTfe2 (void);
  /**
   * Searches for the user with the given ID.
   * \param userId The ID of the user to search for.
   * \param it An iterator that points to the emergency user, if the user was found.
   * \returns True, if the emergency user was found; otherwise, false.
   */
  virtual bool FindEmergUser (uint32_t userId, std::list<EmergUser>::iterator& it);
  /**
   * Removes a user from the list of users in emergency.
   * \param userId The ID of the user to remove.
   * \returns True, if the user ID was found and removed.
   */
  virtual bool RemoveEmergUser (uint32_t userId);

private:
  Time m_delayTfe1; //!< The delay to use for timer TFE1.
  bool m_emerg; //!< The flag that indicates if the machine is in the emergency state.
  std::list<EmergUser> m_emergUsers; //!< The list of users in emergency.
  McpttCallMsgFieldOrgName m_orgName; //!< The name of the MCPTT organization.
  Ptr<McpttCallMachineGrp> m_owner; //!< The owner of the emergency alert machine.
  bool m_started; //!< A flag that indicates if the emergency alert machine has been started.
  Callback<void, bool> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  Ptr<McpttTimer> m_tfe2; //!< The emergency alert retransmission timer.

protected:
  /**
   * Gets the list of users in emergency.
   * \returns The list of users.
   */
  virtual std::list<EmergUser> GetEmergUsers (void) const;
  /**
   * Get the TFE2 timer.
   * \returns The TFE2 timer.
   */
  virtual Ptr<McpttTimer> GetTfe2 (void) const;
  /**
   * Sets the list of users in emergency.
   * \param emergUsers The list of users.
   */
  virtual void SetEmergUsers (const std::list<EmergUser>& emergUsers);
  /**
   * Sets the TFE2 timer.
   * \param tfe2 The TFE2 timer.
   */
  virtual void SetTfe2 (Ptr<McpttTimer>  tfe2);

public:
  /**
   * Gets the flag that indicates if the machine is in the emergency state.
   * \returns The flag.
   */
  virtual bool GetEmerg (void) const;
  /**
   * Gets the name of the MCPTT organization.
   * \returns The name of the MCPTT organization.
   */
  virtual McpttCallMsgFieldOrgName GetOrgName (void) const;
  /**
   * Gets the owner of this emergency alert state machine.
   * \returns The owner.
   */
  virtual Ptr<McpttCallMachineGrp> GetOwner (void) const;
  /**
   * Gets the flag used to indicate if the emergency alert machine has been started.
   * \returns The flag.
   */
  virtual bool GetStarted (void) const;
  /**
   * Sets the flag that indicates if the machine is in the emergency state.
   * \param emerg The flag.
   */
  virtual void SetEmerg (bool emerg);
  /**
   * Sets the name of the MCPTT organization.
   * \param orgName The name of the MCPTT organization.
   */
  virtual void SetOrgName (const McpttCallMsgFieldOrgName& orgName);
  /**
   * Sets the owner of this emergency alert state machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttCallMachineGrp> owner);
  /**
   * Sets the flag used to indicate if the machine has been started.
   * \param started The flag.
   */
  virtual void SetStarted (bool started);
  /**
   * Sets the state change callback, to track changes to the SetEmerg() flag.
   * \param stateChangeCb The state change callback.
   */
  virtual void SetStateChangeCb (const Callback<void, bool>  stateChangeCb);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_EMERG_ALERT_MACHINE_BASIC_H */

