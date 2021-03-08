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

#ifndef MCPTT_CALL_MSG_H
#define MCPTT_CALL_MSG_H

#include <ns3/buffer.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg-field.h"
#include "mcptt-msg.h"

namespace ns3 {

namespace psc {

class McpttCallMachineGrpBasic;
class McpttCallMachineGrpBroadcast;
class McpttCallMachinePrivate;

/**
 * MCPTT Call Message information.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Message Code  |    Octet 2    |    Octet ...  |    Octet N    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Message code     Message Type
 * 0 0 0 0 0 0 0 1  GROUP CALL PROBE
 * 0 0 0 0 0 0 1 0  GROUP CALL ANNOUNCEMENT
 * 0 0 0 0 0 0 1 1  GROUP CALL ACCEPT
 * 0 0 0 0 0 1 0 0  GROUP CALL EMERGENCY END
 * 0 0 0 0 0 1 0 1  GROUP CALL IMMINENT PERIL END
 * 0 0 0 0 0 1 1 0  GROUP CALL BROADCAST
 * 0 0 0 0 0 1 1 1  GROUP CALL BROADCAST END
 * 0 0 0 0 1 0 0 0  PRIVATE CALL SETUP REQUEST
 * 0 0 0 0 1 0 0 1  PRIVATE CALL RINGING
 * 0 0 0 0 1 0 1 0  PRIVATE CALL ACCEPT
 * 0 0 0 0 1 0 1 1  PRIVATE CALL REJECT
 * 0 0 0 0 1 1 0 0  PRIVATE CALL RELEASE
 * 0 0 0 0 1 1 0 1  PRIVATE CALL RELEASE ACK
 * 0 0 0 0 1 1 1 0  PRIVATE CALL ACCEPT ACK
 * 0 0 0 0 1 1 1 1  PRIVATE ENERGENCY CALL CANCEL
 * 0 0 0 1 0 0 0 0  PRIVATE ENERGENCY CALL CANCEL ACK
 * 0 0 0 1 0 0 0 1  GROUP EMERGENCY ALERT
 * 0 0 0 1 0 0 1 0  GROUP EMERGENCY ALERT ACK
 * 0 0 0 1 0 0 1 1  GROUP EMERGENCY ALERT CANCEL
 * 0 0 0 1 0 1 0 0  GROUP EMERGENCY ALERT CANCEL ACK
 *
 */
/**
 * \ingroup mcptt
 *
 * This class provides a common base for all off-network, MCPTT, call control
 * messages that are described in TS 24.379 v14.4.0.
 */
class McpttCallMsg : public McpttMsg
{
public:
  /**
   * Gets the type ID of the McpttCallMsg class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsg class.
   */
  McpttCallMsg (void);
  /**
   * Creates an instance of the McpttCallMsg class.
   * \param msgType The message type field.
   */
  McpttCallMsg (const McpttCallMsgFieldMsgType& msgType);
  /**
   * \brief The destructor of the McpttCallMsg class.
   */
  virtual ~McpttCallMsg (void);
  /**
   * Reads the contents of the message from the byte stream.
   * \param start The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);
  /**
   * Gets the type ID of this McpttCallMsg instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the message to the byte stream.
   * \param start The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator start) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBroadcast& callMachine) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldMsgType m_msgType; //!< The message type.

public:
  /**
   * Gets the message type.
   * \returns The message type.
   */
  virtual McpttCallMsgFieldMsgType GetMsgType (void) const;
  /**
   * Sets the message type.
   * \param msgType The message type.
   */
  virtual void SetMsgType (const McpttCallMsgFieldMsgType& msgType);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call-control GROUP CALL PROBE message. This
 * message is used to check for an ongoing group call.
 *
 * Message code = 1
 */
class McpttCallMsgGrpProbe : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpProbe class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpProbe class.
   */
  McpttCallMsgGrpProbe (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpProbe class.
   */
  virtual ~McpttCallMsgGrpProbe (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpProbe instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID field.

public:
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call-control GROUP CALL ANNOUNCEMENT message.
 * This message is used to announce an ongoing group call to other UEs.
 *
 * Message code = 2
 */
class McpttCallMsgGrpAnnoun : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpAnnoun class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpAnnoun class.
   */
  McpttCallMsgGrpAnnoun (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpAnnoun class.
   */
  virtual ~McpttCallMsgGrpAnnoun (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpAnnoun instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Indicates if the message contains a confirm mode indication IE.
   * \returns True, if the confirm mode indication IE is included.
   */
  virtual bool IsConfirmModeIndicated (void) const;
  /**
   * Indicates if the message contains a call probe response IE.
   * \returns True, if the call probe response IE is included.
   */
  virtual bool IsProbeResp (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier.
  McpttCallMsgFieldCallType m_callType; //!< The call type.
  bool m_confirmMode; //!< The confirm mode indication flag.
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID.
  McpttCallMsgFieldLastChgTime m_lastChgTime; //!< The last call type change time.
  McpttCallMsgFieldUserId m_lastChgUserId; //!< The MCPTT user ID of the last user to change the call type.
  McpttCallMsgFieldUserId m_origUserId; //!< The originating MCPTT user ID.
  bool m_probeResp; //!< The probe response flag.
  McpttCallMsgFieldRefreshInterval m_refInt; //!< The refresh interval.
  McpttCallMsgFieldSdp m_sdp; //!< The SDP.
  McpttCallMsgFieldStartTime m_startTime; //!< The call start time.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the call type field.
   * \returns The call type field.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the confirm mode indication flag.
   * \returns the flag.
   */
  virtual bool GetConfirmMode (void) const;
  /**
   * Gets the group ID field.
   * \returns The group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the last call type change time field.
   * \returns The last call type change time field.
   */
  virtual McpttCallMsgFieldLastChgTime GetLastChgTime (void) const;
  /**
   * Gets the MCPTT user ID of the last user to change the call type.
   * \returns The MCPTT user ID.
   */
  virtual McpttCallMsgFieldUserId GetLastChgUserId (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the probe response flag.
   * \returns The flag.
   */
  virtual bool GetProbeResp (void) const;
  /**
   * Gets the refresh interval field.
   * \returns The refresh interval field.
   */
  virtual McpttCallMsgFieldRefreshInterval GetRefInt (void) const;
  /**
   * Gets the SDP field.
   * \returns The SDP field.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Gets the call start time field.
   * \returns The call start time field.
   */
  virtual McpttCallMsgFieldStartTime GetStartTime (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the call type field.
   * \param callType The call type field.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType);
  /**
   * Sets the confirm mode indication flag.
   * \param confirmMode The flag.
   */
  virtual void SetConfirmMode (const bool& confirmMode);
  /**
   * Sets the group ID field.
   * \param grpId The group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the call type last change time field.
   * \param lastChgTime The call type last change time field.
   */
  virtual void SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime);
  /**
   * Sets the MCPTT user ID of the last user to change the call type.
   * \param lastChgUserId The MCPTT user ID.
   */
  virtual void SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId);
  /**
   * Sets the user ID field.
   * \param origUserId The user ID field.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origUserId);
  /**
   * Sets the probe response flag.
   * \param probeResp The flag.
   */
  virtual void SetProbeResp (const bool& probeResp);
  /**
   * Sets the refresh interval field.
   * \param refInt The refresh interval field.
   */
  virtual void SetRefInt (const McpttCallMsgFieldRefreshInterval& refInt);
  /**
   * Sets the SDP field.
   * \param sdpField The SDP field.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdpField);
  /**
   * Sets the call start time field.
   * \param startTime The call start time field.
   */
  virtual void SetStartTime (const McpttCallMsgFieldStartTime& startTime);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL ACCEPT message. This
 * message is used to indicate acceptance of a group call.
 *
 * Message code = 3
 */
class McpttCallMsgGrpAccept : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpAccept class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpAccept class.
   */
  McpttCallMsgGrpAccept (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpAccept class.
   */
  virtual ~McpttCallMsgGrpAccept (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpAccept instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldCallType m_callType; //!< The call type field.
  McpttCallMsgFieldGrpId m_grpId; //!< The group ID field.
  McpttCallMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the call type field.
   * \returns The call type field.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the group ID field.
   * \returns The group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttCallMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the call type field.
   * \param callType The call type field.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType);
  /**
   * Sets the group ID field.
   * \param grpId The group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttCallMsgFieldUserId& userId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL IMMINENT PERIL END.
 * This message is sent to indicate termination of imminent peril mode in the
 * group call.
 *
 * Message code = 5
 */
class McpttCallMsgGrpImmPerilEnd : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpImmPerilEnd class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpImmPerilEnd class.
   */
  McpttCallMsgGrpImmPerilEnd (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpImmPerilEnd class.
   */
  virtual ~McpttCallMsgGrpImmPerilEnd (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpImmPerilEnd instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID.
  McpttCallMsgFieldLastChgTime m_lastChgTime; //!< Last call type change time.
  McpttCallMsgFieldUserId m_lastChgUserId; //!< The MCPTT ID of the last user to change the call type.
  McpttCallMsgFieldUserId m_userId; //!< The sending MCPTT user ID.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the last call type change time field.
   * \returns The last call type change time field.
   */
  virtual McpttCallMsgFieldLastChgTime GetLastChgTime (void) const;
  /**
   * Gets the MCPTT user ID of the last user to change the call type.
   * \returns The MCPTT user ID of the last user to change the call type.
   */
  virtual McpttCallMsgFieldUserId GetLastChgUserId (void) const;
  /**
   * Gets the MCPTT user ID of the sending user.
   * \returns The MCPTT user ID of the sending user.
   */
  virtual McpttCallMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the last call type change time field.
   * \param lastChgTime The last call type change time field.
   */
  virtual void SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime);
  /**
   * Sets the MCPTT user ID of the last user to change the call type.
   * \param lastChgUserId The MCPTT user ID of the last user to change the call type.
   */
  virtual void SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId);
  /**
   * Sets the MCPTT user ID of the sending user.
   * \param userId The MCPTT user ID of the sending user.
   */
  virtual void SetUserId (const McpttCallMsgFieldUserId& userId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL EMERGENCY END message.
 * This message is sued to indicate termination of emergency mode in the group
 * call.
 *
 * Message code = 4
 */
class McpttCallMsgGrpEmergEnd : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpEmergEnd class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpEmergEnd class.
   */
  McpttCallMsgGrpEmergEnd (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpEmergEnd class.
   */
  virtual ~McpttCallMsgGrpEmergEnd (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpEmergEnd instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID.
  McpttCallMsgFieldLastChgTime m_lastChgTime; //!< Last call type change time.
  McpttCallMsgFieldUserId m_lastChgUserId; //!< The MCPTT ID of the last user to change the call type.
  McpttCallMsgFieldUserId m_userId; //!< The sending MCPTT user ID.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the last call type change time field.
   * \returns The last call type change time field.
   */
  virtual McpttCallMsgFieldLastChgTime GetLastChgTime (void) const;
  /**
   * Gets the MCPTT user ID of the last user to change the call type.
   * \returns The MCPTT user ID of the last user to change the call type.
   */
  virtual McpttCallMsgFieldUserId GetLastChgUserId (void) const;
  /**
   * Gets the MCPTT user ID of the sending user.
   * \returns The MCPTT user ID of the sending user.
   */
  virtual McpttCallMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the last call type change time field.
   * \param lastChgTime The last call type change time field.
   */
  virtual void SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime);
  /**
   * Sets the MCPTT user ID of the last user to change the call type.
   * \param lastChgUserId The MCPTT user ID of the last user to change the call type.
   */
  virtual void SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId);
  /**
   * Sets the MCPTT user ID of the sending user.
   * \param userId The MCPTT user ID of the sending user.
   */
  virtual void SetUserId (const McpttCallMsgFieldUserId& userId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL EMERGENCY ALERT message.
 * This message is used to indicate an emergency situation.
 *
 * Message code = 17
 */
class McpttCallMsgGrpEmergAlert : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpEmergAlert class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpEmergAlert class.
   */
  McpttCallMsgGrpEmergAlert (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpEmergAlert class.
   */
  virtual ~McpttCallMsgGrpEmergAlert (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpEmergAlert instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID field.
  McpttCallMsgFieldOrgName m_orgName; //!< The organization name field.
  McpttCallMsgFieldUserId m_userId; //!< The MCPTT user ID field.
  McpttCallMsgFieldUserLoc m_userLoc; //!< The user location field.

public:
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the organization name field.
   * \returns The organization name field.
   */
  virtual McpttCallMsgFieldOrgName GetOrgName (void) const;
  /**
   * Gets the MCPTT user ID field.
   * \returns The MCPTT user ID field.
   */
  virtual McpttCallMsgFieldUserId GetUserId (void) const;
  /**
   * Gets the user location field.
   * \returns The user location field.
   */
  virtual McpttCallMsgFieldUserLoc GetUserLoc (void) const;
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the organization name field.
   * \param orgName The organization name field.
   */
  virtual void SetOrgName (const McpttCallMsgFieldOrgName& orgName);
  /**
   * Sets the MCPTT user ID field.
   * \param userId The MCPTT user ID field.
   */
  virtual void SetUserId (const McpttCallMsgFieldUserId& userId);
  /**
   * Sets the user location field.
   * \param userLoc The user location field.
   */
  virtual void SetUserLoc (const McpttCallMsgFieldUserLoc& userLoc);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL EMERGENCY ALERT ACK
 * message.This message is used to indicate receipt of an emergency alert.
 *
 * Message code = 18
 */
class McpttCallMsgGrpEmergAlertAck : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpEmergAlertAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpEmergAlertAck class.
   */
  McpttCallMsgGrpEmergAlertAck (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpEmergAlertAck class.
   */
  virtual ~McpttCallMsgGrpEmergAlertAck (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpEmergAlertAck instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID field.
  McpttCallMsgFieldUserId m_origId; //!< The MCPTT user ID of the originating user.
  McpttCallMsgFieldUserId m_sendingId; //!< The MCPTT user ID of the sending user.

public:
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the MCPTT user ID of the originating user.
   * \returns The MCPTT user ID of the originating user.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the MCPTT user ID of the sending user.
   * \returns The MCPTT user ID of the sending user.
   */
  virtual McpttCallMsgFieldUserId GetSendingId (void) const;
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the MCPTT user ID of the originating user.
   * \param origId The MCPTT user ID of the originating user.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origId);
  /**
   * Sets the MCPTT user ID of the sending user.
   * \param sendingId The MCPTT user ID of the sending user.
   */
  virtual void SetSendingId (const McpttCallMsgFieldUserId& sendingId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL EMERGENCY ALERT CANCEL
 * message.This message is used to indicate end of emergency situation.
 *
 * Message code = 19
 */
class McpttCallMsgGrpEmergAlertCancel : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpEmergAlertCancel class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpEmergAlertCancel class.
   */
  McpttCallMsgGrpEmergAlertCancel (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpEmergAlertCancel class.
   */
  virtual ~McpttCallMsgGrpEmergAlertCancel (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpEmergAlertCancel instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID field.
  McpttCallMsgFieldUserId m_origId; //!< The MCPTT user ID of the originating user.
  McpttCallMsgFieldUserId m_sendingId; //!< The MCPTT user ID of the sending user.

public:
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the MCPTT user ID of the originating user.
   * \returns The MCPTT user ID of the originating user.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the MCPTT user ID of the sending user.
   * \returns The MCPTT user ID of the sending user.
   */
  virtual McpttCallMsgFieldUserId GetSendingId (void) const;
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the MCPTT user ID of the originating user.
   * \param origId The MCPTT user ID of the originating user.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origId);
  /**
   * Sets the MCPTT user ID of the sending user.
   * \param sendingId The MCPTT user ID of the sending user.
   */
  virtual void SetSendingId (const McpttCallMsgFieldUserId& sendingId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL EMERGENCY CANCEL ACK
 * message.This message is used to indicate receipt of an emergency alert
 * cancel.
 *
 * Message code = 20
 */
class McpttCallMsgGrpEmergAlertCancelAck : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpEmergAlertCancelAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpEmergAlertCancelAck class.
   */
  McpttCallMsgGrpEmergAlertCancelAck (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpEmergAlertCancelAck class.
   */
  virtual ~McpttCallMsgGrpEmergAlertCancelAck (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpEmergAlertCancelAck instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBasic& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID field.
  McpttCallMsgFieldUserId m_origId; //!< The MCPTT user ID of the originating user.
  McpttCallMsgFieldUserId m_sendingId; //!< The MCPTT user ID of the sending user.

public:
  /**
   * Gets the MCPTT group ID field.
   * \returns The MCPTT group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the MCPTT user ID of the originating user.
   * \returns The MCPTT user ID of the originating user.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the MCPTT user ID of the sending user.
   * \returns The MCPTT user ID of the sending user.
   */
  virtual McpttCallMsgFieldUserId GetSendingId (void) const;
  /**
   * Sets the MCPTT group ID field.
   * \param grpId The MCPTT group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the MCPTT user ID of the originating user.
   * \param origId The MCPTT user ID of the originating user.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origId);
  /**
   * Sets the MCPTT user ID of the sending user.
   * \param sendingId The MCPTT user ID of the sending user.
   */
  virtual void SetSendingId (const McpttCallMsgFieldUserId& sendingId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL BROADCAST message. This
 * message is sent by the UE to the other UEs to announce a broadcast group
 * call to other UEs
 *
 * Message code = 6
 */
class McpttCallMsgGrpBroadcast : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpBroadcast class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpBroadcast class.
   */
  McpttCallMsgGrpBroadcast (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpBroadcast class.
   */
  virtual ~McpttCallMsgGrpBroadcast (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpBroadcast instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBroadcast& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier.
  McpttCallMsgFieldCallType m_callType; //!< The call type.
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID.
  McpttCallMsgFieldUserId m_origId; //!< The originating MCPTT user ID.
  McpttCallMsgFieldSdp m_sdp; //!< The SDP.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the call type field.
   * \returns The call type field.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the group ID field.
   * \returns The group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Gets the SDP field.
   * \returns The SDP field.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the call type field.
   * \param callType The call type field.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType);
  /**
   * Sets the group ID field.
   * \param grpId The group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the user ID field.
   * \param origUserId The user ID field.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origUserId);
  /**
   * Sets the SDP field.
   * \param sdpField The SDP field.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdpField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control GROUP CALL BROADCAST END message.
 * This message is sent by the UE to the other UEs to announce a broadcast
 * group call to other UEs
 *
 * Message code = 7
 */
class McpttCallMsgGrpBroadcastEnd : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgGrpBroadcastEnd class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgGrpBroadcastEnd class.
   */
  McpttCallMsgGrpBroadcastEnd (void);
  /**
   * \brief The destructor of the McpttCallMsgGrpBroadcastEnd class.
   */
  virtual ~McpttCallMsgGrpBroadcastEnd (void);
  /**
   * Gets the type ID of this McpttCallMsgGrpBroadcastEnd instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachineGrpBroadcast& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier.
  McpttCallMsgFieldGrpId m_grpId; //!< The MCPTT group ID.
  McpttCallMsgFieldUserId m_origId; //!< The originating MCPTT user ID.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the group ID field.
   * \returns The group ID field.
   */
  virtual McpttCallMsgFieldGrpId GetGrpId (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttCallMsgFieldUserId GetOrigId (void) const;
  /**
   * Sets the call identifier field.
   * \param callId The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callId);
  /**
   * Sets the group ID field.
   * \param grpId The group ID field.
   */
  virtual void SetGrpId (const McpttCallMsgFieldGrpId& grpId);
  /**
   * Sets the user ID field.
   * \param origUserId The user ID field.
   */
  virtual void SetOrigId (const McpttCallMsgFieldUserId& origUserId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL SETUP REQUEST
 * message. This message is used to request the setup of a private call.
 *
 * Message code = 8
 */
class McpttCallMsgPrivateSetupReq : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateSetupReq class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateSetupReq class.
   */
  McpttCallMsgPrivateSetupReq (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateSetupReq class.
  */
  virtual ~McpttCallMsgPrivateSetupReq (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateSetupReq instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldCallType m_callType; //!< The call type field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.
  McpttCallMsgFieldCommMode m_commMode; //!< The commencement mode field.
  McpttCallMsgFieldSdp m_sdp; //!< The SDP field.
  McpttCallMsgFieldUserLoc m_userLoc; //!< The user location.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the call type field.
   * \returns The call type field.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Gets the commencement mode field.
   * \returns The commencement mode field.
   */
  virtual McpttCallMsgFieldCommMode GetCommMode (void) const;
  /**
   * Gets the SDP field.
   * \returns The SDP field.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Gets the user location field.
   * \returns The user location field.
   */
  virtual McpttCallMsgFieldUserLoc GetUserLoc (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the call type field.
   * \param callTypeField The call type field.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callTypeField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
  /**
   * Sets the commencement mode field.
   * \param commModeField The commencement mode field.
   */
  virtual void SetCommMode (const McpttCallMsgFieldCommMode& commModeField);
  /**
   * Sets the SDP field.
   * \param sdpField Sets the SDP field.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdpField);
  /**
   * Sets the user location field.
   * \param userLocField The user location field.
   */
  virtual void SetUserLoc (const McpttCallMsgFieldUserLoc& userLocField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL RINGING message.
 * This message is automatically sent in response to a PRIVATE CALL SETUP
 * REQUEST message, and indicates that the UE has presented the incoming call
 * notification to the user and is awaiting user response.
 *
 * Message code = 9
 */
class McpttCallMsgPrivateRinging : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateRinging class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateRinging class.
   */
  McpttCallMsgPrivateRinging (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateRinging class.
  */
  virtual ~McpttCallMsgPrivateRinging (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateRinging instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the called user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL ACCEPT message.
 * This message is sent in response to a PRIVATE CALL SETUP REQUEST message
 * when user accepts the call. This message indicates that the UE accepts the
 * call setup request.
 *
 * Message code = 10
 */
class McpttCallMsgPrivateAccept : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateAccept class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateAccept class.
   */
  McpttCallMsgPrivateAccept (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateAccept class.
  */
  virtual ~McpttCallMsgPrivateAccept (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateAccept instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.
  McpttCallMsgFieldSdp m_sdp; //!< The SDP field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Gets the SDP field.
   * \returns The SDP field.
   */
  virtual McpttCallMsgFieldSdp GetSdp (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
  /**
   * Sets the SDP field.
   * \param sdpField The SDP field.
   */
  virtual void SetSdp (const McpttCallMsgFieldSdp& sdpField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL REJECT message.
 * This message is sent in response to a PRIVATE CALL SETUP REQUEST message
 * when user rejects the call. This message indicates that the UE rejects the
 * call setup request.
 *
 * Message code = 11
 */
class McpttCallMsgPrivateReject : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateReject class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateReject class.
   */
  McpttCallMsgPrivateReject (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateReject class.
  */
  virtual ~McpttCallMsgPrivateReject (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateReject instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.
  McpttCallMsgFieldReason m_reason; //!< The reason field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Gets the reason field.
   * \returns The reason field.
   */
  virtual McpttCallMsgFieldReason GetReason (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
  /**
   * Sets the reason field.
   * \param reasonField The reason field.
   */
  virtual void SetReason (const McpttCallMsgFieldReason& reasonField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL RELEASE message. This
 * message is used to terminate an ongoing private call.
 *
 * Message code = 12
 */
class McpttCallMsgPrivateRelease : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateRelease class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateRelease class.
   */
  McpttCallMsgPrivateRelease (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateRelease class.
  */
  virtual ~McpttCallMsgPrivateRelease (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateRelease instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL RELEASE ACK message.
 * This message is sent in response to a PRIVATE CALL RELEASE message. This
 * message indicates that the UE has terminated the call.
 *
 * Message code = 13
 */
class McpttCallMsgPrivateReleaseAck : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateReleaseAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateReleaseAck class.
   */
  McpttCallMsgPrivateReleaseAck (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateReleaseAck class.
  */
  virtual ~McpttCallMsgPrivateReleaseAck (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateReleaseAck instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL ACCEPT ACK message.
 * This message is sent in response to a PRIVATE CALL ACCEPT message. This
 * message acknowledges the receipt of PRIVATE CALL ACCEPT message.
 *
 * Message code = 14
 */
class McpttCallMsgPrivateAcceptAck : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateAcceptAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateAcceptAck class.
   */
  McpttCallMsgPrivateAcceptAck (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateAcceptAck class.
  */
  virtual ~McpttCallMsgPrivateAcceptAck (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateAcceptAck instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL EMERGENCY CANCEL.
 * This message is used to indicate termination of emergency mode in private
 * call.
 *
 * Message code = 15
 */
class McpttCallMsgPrivateEmergCancel : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateEmergCancel class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateEmergCancel class.
   */
  McpttCallMsgPrivateEmergCancel (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateEmergCancel class.
  */
  virtual ~McpttCallMsgPrivateEmergCancel (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateEmergCancel instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control PRIVATE CALL EMERGENCY CANCEL ACK.
 * This message is sent to indicate receipt of PRIVATE CALL EMERGENCY CANCEL
 * message.
 *
 * Message code = 16
 */
class McpttCallMsgPrivateEmergCancelAck : public McpttCallMsg
{
public:
  /**
   * The message code.
   */
  static const uint8_t CODE;
  /**
   * Gets the type ID of the McpttCallMsgPrivateEmergCancelAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgPrivateEmergCancelAck class.
   */
  McpttCallMsgPrivateEmergCancelAck (void);
/**
  * \brief The destructor of the McpttCallMsgPrivateEmergCancelAck class.
  */
  virtual ~McpttCallMsgPrivateEmergCancelAck (void);
  /**
   * Gets the type ID of this McpttCallMsgPrivateEmergCancelAck instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Visits a call machine.
   * \param callMachine The call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  McpttCallMsgFieldCallId m_callId; //!< The call identifier field.
  McpttCallMsgFieldUserId m_calleeId; //!< The callee user ID field.
  McpttCallMsgFieldUserId m_callerId; //!< The caller user ID field.

public:
  /**
   * Gets the call identifier field.
   * \returns The call identifier field.
   */
  virtual McpttCallMsgFieldCallId GetCallId (void) const;
  /**
   * Gets the callee user ID field.
   * \returns the callee user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCalleeId (void) const;
  /**
   * Gets the caller user ID field.
   * \returns The caller user ID field.
   */
  virtual McpttCallMsgFieldUserId GetCallerId (void) const;
  /**
   * Sets the call identifier field.
   * \param callIdField The call identifier field.
   */
  virtual void SetCallId (const McpttCallMsgFieldCallId& callIdField);
  /**
   * Sets the callee user ID field.
   * \param calleeIdField The callee user ID field.
   */
  virtual void SetCalleeId (const McpttCallMsgFieldUserId& calleeIdField);
  /**
   * Sets the caller user ID field.
   * \param callerIdField The caller user ID field.
   */
  virtual void SetCallerId (const McpttCallMsgFieldUserId& callerIdField);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MSG_H */

