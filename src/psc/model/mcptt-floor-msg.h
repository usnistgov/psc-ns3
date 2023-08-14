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

#ifndef MCPTT_FLOOR_MSG_H
#define MCPTT_FLOOR_MSG_H

#include <list>
#include <iostream>
#include <string>

#include <ns3/buffer.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-floor-msg-field.h"
#include "mcptt-msg.h"
#include "mcptt-queued-user-info.h"

namespace ns3 {

namespace psc {

class McpttFloorMsgSink;

/**
 * MCPTT Floor Message information.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |            length             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           SSRC                                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name (ASCII)                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                 application-dependent data                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Secure RTCP message part                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Message Name                 Subtype
 * Floor Request                = 00000
 * Floor Granted                = x0001
 * Floor Deny                   = x0011
 * Floor Release                = x0100
 * Floor Idle                   = x0101
 * Floor Taken                  = x0010
 * Floor Revoke                 = 00110
 * Floor Queue Position Request = 01000
 * Floor Queue Position Info    = x1001
 * Floor Ack                    = 01010
 *
 * NOTE: When x is coded as 0 -> Acknowledgement is NOT required
 *                          1 -> Acknowledgement is required.
 */
/**
 * \ingroup mcptt
 *
 * This class provides a common base for all off-netork, MCPTT, floor control
 * messages that are described in TS 24.380 v14.4.0.
 */
class McpttFloorMsg : public McpttMsg
{
public:
  /**
   * Gets the type ID of the McpttFloorMsg.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
  * Creates an instance of the McpttFloorMsg class.
  */
  McpttFloorMsg (void);
  /**
   * Creates an instance of the McpttFloorMsg class.
   * \param subtype The subtype (or ID) of the message.
   * \param ssrc They synchronization source (SSRC).
   */
  McpttFloorMsg (uint8_t subtype, uint32_t ssrc = 0);
  /**
   * Creates an instance of the McpttFloorMsg class.
   * \param name The name.
   * \param payloadType The payload type (PT).
   * \param subtype The subtype (or ID) of the message.
   * \param ssrc The synchronization source (SSRC).
   */
  McpttFloorMsg (const std::string& name, uint8_t payloadType, uint8_t subtype, uint32_t ssrc);
  /**
   * The destructor of the McpttFloorMsg class.
   */
  virtual ~McpttFloorMsg (void);
  /**
   * Reads the contents of the message from the byte stream.
   * \param start The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);
  /**
   * Gets the type ID of this McpttFloorMsg instance.
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
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

protected:
  /**
   * Reads the data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Updates the length of the message.
   * \param currField The current field.
   * \param newField The new field.
   */
  virtual void UpdateLength (const McpttFloorMsgField& currField, const McpttFloorMsgField& newField);
  /**
   * Writes the data of the message to the byte sream.
   * \param buff The stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;

private:
  uint16_t m_length; //!< The length of the message (not including the header).
  std::string m_name; //!< The name.
  uint8_t m_padding; //!< The padding bit value (P).
  uint8_t m_payloadType; //!< The payload type (PT).
  uint32_t m_ssrc; //!< The synchronization source (SSRC).
  uint8_t m_subtype; //!< The message type (Subtype).
  uint8_t m_version; //!< The version of the RTCP (V).

public:
  /**
   * Gets the length (in bytes) of the message (not including the header).
   * \returns The length (in bytes).
   */
  virtual uint16_t GetLength (void) const;
  /**
   * Gets the name.
   * \returns The name.
   */
  virtual std::string GetName (void) const;
  /**
   * Gets the padding bit value (P).
   * \returns The padding bit value.
   */
  virtual uint8_t GetPadding (void) const;
  /**
   * Gets the value of the payload type (PT) value.
   * \returns The payload type (PT) value.
   */
  virtual uint8_t GetPayloadType (void) const;
  /**
   * Gets the synchronization source (SSRC).
   * \returns The synchronization source.
   */
  virtual uint32_t GetSsrc (void) const;
  /**
   * Gets the message type (Subtype).
   * \returns The message type.
   */
  virtual uint8_t GetSubtype (void) const;
  /**
   * Gets the version number (V).
   * \returns The version number.
   */
  virtual uint8_t GetVersion (void) const;
  /**
   * Sets the length (in bytes) of the message (not including the header).
   * \param length The length of the message (in bytes).
   */
  virtual void SetLength (uint16_t length);
  /**
   * Sets the name.
   * \param name The name.
   */
  virtual void SetName (const std::string& name);
  /**
   * Sets the padding bit value (P).
   * \param paddingBit The paddinb bit value.
   */
  virtual void SetPadding (uint8_t paddingBit);
  /**
   * Sets the value of the payload type (PT).
   * \param payloadType The payload type (PT) value.
   */
  virtual void SetPayloadType (uint8_t payloadType);
  /**
   * Sets the synchronization source (SSRC).
   * \param ssrc The synchronization source.
   */
  virtual void SetSsrc (uint32_t ssrc);
  /**
   * Sets the message type (Subtype).
   * \param subtype The message type.
   */
  virtual void SetSubtype (uint8_t subtype);
  /**
   * Sets the value of the version number (V).
   * \param version The version number.
   */
  virtual void SetVersion (uint8_t version);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT floor request message.
 *
 * Subtype = 00000
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |          length               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  SSRC of floor participant sending the Floor Request message  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Floor Priority field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * :                       User ID field                           :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                     Track Info field                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
class McpttFloorMsgRequest : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT floor request message.
   */
  static const uint8_t SUBTYPE;
  /**
   * Gets the type ID of the floor request message.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgRequest class.
   * \param ssrc The synchronization source.
   */
  McpttFloorMsgRequest (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgRequest class.
   */
  virtual ~McpttFloorMsgRequest (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldPriority m_priority; //!< The floor priority field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the floor priority field.
   * \returns The floor priority field.
   */
  virtual McpttFloorMsgFieldPriority GetPriority (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the priority field.
   * \param priority The priority field.
   */
  virtual void SetPriority (const McpttFloorMsgFieldPriority& priority);
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT floor granted message.
 *
 * Subtype = x0001
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |          length               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               SSRC of floor control server                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Duration field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of granted floor participant field        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Floor Priority field                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        User ID field                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        Queue Size field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |             SSRC of queued floor participant field            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                     Queued User ID field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        Queue Info field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype: The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Duration: The Duration field is only applicable in on-network and shall be coded as specified in subclause 8.2.3.3.
 * SSRC of granted floor participant: The SSRC of granted floor participant is only applicable in off-network.
 * Floor Priority: The Floor Priority field contains the granted floor priority and shall be coded as specified in subclause 8.2.3.2.
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT ID of the floor participant granted the floor. The User ID field shall be coded as described in subclause 8.2.3.8.
 * Queue Size: The Queue Size field is only applicable in off-network and contains the numbers of waiting MCPTT clients in the MCPTT call. The Queue Size field shall be coded as specified in subclause 8.2.3.9.
 * For each waiting floor participant the following set of fields are included:
 *  1.SSRC of queued floor participant;
 *  2.Queued User ID field; and
 *  3.Queue info field.
 * The set shall occur as many times as the <b>Queue size</b> value in the Queue size field.
 * SSRC of queued floor participant: The SSRC of queued floor participant is only applicable in off-network and shall carry the SSRC of the floor participant in the queue.
 * Queued User ID: The Queued User ID field is only applicable in off-network and contains the MCPTT ID of the floor participant in the queue.
 * Queue Info: The Queue Info field is only applicable in off-network and defines the queue position and granted floor priority in the queue.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 * Floor Indicator: The Floor Indicator field shall be coded as described in subclause 8.2.3.15.
 */
class McpttFloorMsgGranted : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT floor granted message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of the McpttFloorMsgGranted class.
   * \return the TypeId of the class
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgGranted class.
   * \param ssrc The SSRC of the floor participant.
   */
  McpttFloorMsgGranted (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgGranted class.
   */
  virtual ~McpttFloorMsgGranted (void);
  /**
   * Adds a queued user's information.
   * \param userInfo The queued users's information.
   */
  virtual void AddUserInfo (const McpttQueuedUserInfo& userInfo);
  /**
   * Clears the collection of queued user's info.
   */
  virtual void ClearUserInfo (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Updates the collection of users.
   * \param users The new collection of users.
   */
  virtual void UpdateUsers (const std::list<McpttQueuedUserInfo>& users);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldDuration m_duration; //!< The duration field.
  McpttFloorMsgFieldSsrc m_grantedSsrc; //!< The SSRC of the granted floor participant.
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldPriority m_priority; //!< The floor priority field.
  McpttFloorMsgFieldQueueSize m_queueSize; //!< The queue size field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID of the granted floor participant.
  std::list<McpttQueuedUserInfo> m_users; //!< The collection of queued users.

public:
  /**
   * Gets the duration field.
   * \returns The duration field.
   */
  virtual McpttFloorMsgFieldDuration GetDuration (void) const;
  /**
   * Gets the SSRC of the granted floor particpant.
   * \returns The SSRC.
   */
  virtual McpttFloorMsgFieldSsrc GetGrantedSsrc (void) const;
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the floor priority field.
   * \returns The floor priority field.
   */
  virtual McpttFloorMsgFieldPriority GetPriority (void) const;
  /**
   * Gets the queue size field.
   * \returns The queue size field.
   */
  virtual McpttFloorMsgFieldQueueSize GetQueueSize (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID of the granted floor participant.
   * \returns The user ID.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Gets the collection of queued users.
   * \returns The collection of queued users.
   */
  virtual std::list<McpttQueuedUserInfo> GetUsers (void) const;
  /**
   * Sets the duration field.
   * \param duration The duration field.
   */
  virtual void SetDuration (const McpttFloorMsgFieldDuration& duration);
  /**
   * Sets the SSRC of the granted floor particpant.
   * \param grantedSsrc The SSRC of the granted floor particpant.
   */
  virtual void SetGrantedSsrc (McpttFloorMsgFieldSsrc grantedSsrc);
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the floor priority field.
   * \param priority The floor priority field.
   */
  virtual void SetPriority (const McpttFloorMsgFieldPriority& priority);
  /**
   * Sets the user ID of the granted floor participant.
   * \param userId The user ID.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);
  /**
   * Sets the collection of queued users.
   * \param users The collection of queued user info.
   */
  virtual void SetUsers (const std::list<McpttQueuedUserInfo>& users);

protected:
  /**
   * Sets the queue size field.
   * \param queueSize The queue size field.
   */
  virtual void SetQueueSize (const McpttFloorMsgFieldQueueSize& queueSize);
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Deny message.
 *
 * Subytype = x0011
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |            length             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of floor control server                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Reject Cause field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       User ID field                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype: The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Reject Cause: The Reject Cause field includes the reason for the rejecting the floor request and can be followed by a text-string explaining why the floor request was rejected. Therefore the length of the packet will vary depending on the size of the application dependent field.
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT ID of the floor participant sending Floor Deny message.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 */
class McpttFloorMsgDeny : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Deny message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of thie McpttFloorMsgDeny class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgDeny class.
   * \param ssrc The synchronization source.
   */
  McpttFloorMsgDeny (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgDeny class.
   */
  virtual ~McpttFloorMsgDeny (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldRejectCause m_rejCause; //!< The reject cause field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the reject cause field.
   * \returns The reject cause field.
   */
  virtual McpttFloorMsgFieldRejectCause GetRejCause (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the reject cause field.
   * \param rejCause The reject cause field.
   */
  virtual void SetRejCause (const McpttFloorMsgFieldRejectCause& rejCause);
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Release message.
 *
 * Subtype = x0100
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |          length               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   SSRC of floor participant with permission to send media     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       User ID field                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * SSRC: The SSRC field shall carry the SSRC of the floor participant with permission to send media.
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT ID of the floor participant sending the floor release message.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function.
 * Floor Indicator: The Floor Indicator field shall be coded as described in subclause 8.2.3.15.
 */
class McpttFloorMsgRelease : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Release message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of the McpttFloorMsgRelease class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgRelease class.
   * \param ssrc The SSRC of the floor participant with permission to send.
   */
  McpttFloorMsgRelease (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgRelease class.
   */
  virtual ~McpttFloorMsgRelease (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the indicator field.
   * \param indicator The indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Revoke message.
 *
 * Subytype = 00110
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |            length             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of floor control server                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Reject Cause field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype: The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Reject Cause: The Reject Cause field includes the reason for the rejecting the floor request and can be followed by a text-string explaining why the floor request was rejected. Therefore the length of the packet will vary depending on the size of the application dependent field.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 */
class McpttFloorMsgRevoke : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Revoke message.
   */
  static const uint8_t SUBTYPE;
  /**
   * Gets the type ID of thie McpttFloorMsgRevoke class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgRevoke class.
   * \param ssrc The synchronization source.
   */
  McpttFloorMsgRevoke (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgRevoke class.
   */
  virtual ~McpttFloorMsgRevoke (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldRejectCause m_rejCause; //!< The reject cause field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the reject cause field.
   * \returns The reject cause field.
   */
  virtual McpttFloorMsgFieldRejectCause GetRejCause (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the reject cause field.
   * \param rejCause The reject cause field.
   */
  virtual void SetRejCause (const McpttFloorMsgFieldRejectCause& rejCause);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Idle message.
 *
 * Subtype = x0101
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |           length              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of floor control server                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                 Message Sequence Number field                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype:The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Message Sequence Number: The Message Sequence Number field shall be coded as specified in to subclause 8.2.3.10.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 * Floor Indicator: The Floor Indicator field shall be coded as described in subclause 8.2.3.15.
 */
class McpttFloorMsgIdle : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Idle message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of the McpttFloorMsgIdle class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgIdle class.
   * \param ssrc The SSRC of the floor control server.
   */
  McpttFloorMsgIdle (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgIdle class.
   */
  virtual ~McpttFloorMsgIdle (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldSeqNum m_seqNum; //!< The message sequence number field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the message sequence number field.
   * \returns The message sequence number field.
   */
  virtual McpttFloorMsgFieldSeqNum GetSeqNum (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the message sequence number field.
   * \param seqNum The message sequence number field.
   */
  virtual void SetSeqNum (const McpttFloorMsgFieldSeqNum& seqNum);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Taken message.
 *
 * Subtype = x0010
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |           length              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of floor control server                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                  Granted Party's Identity field               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               Permission to Request the Floor field           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       User ID field                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                 Message Sequence Number field                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype:The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Granted Party’s Identity: The Granted Party’s Identity field shall be coded as specified in subclause 8.2.3.6.
 * Permission to request the floor: The Permission to Request the Floor field shall be coded as specified in subclause 8.2.3.7.
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT user ID of the floor participant sending the Floor Taken message.
 * Message Sequence Number: The Message Sequence Number field shall be coded as specified in to subclause 8.2.3.10.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 * Floor Indicator: The Floor Indicator field shall be coded as described in subclause 8.2.3.15.
 */
class McpttFloorMsgTaken : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Taken message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of the McpttFloorMsgTaken class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgTaken class.
   * \param ssrc The SSRC of the floor control server.
   */
  McpttFloorMsgTaken (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgTaken class.
   */
  virtual ~McpttFloorMsgTaken (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.
  McpttFloorMsgFieldGrantedPartyId m_partyId; //!< The granted party's ID field.
  McpttFloorMsgFieldPermToReq m_permission; //!< The permission to request the floor field.
  McpttFloorMsgFieldSeqNum m_seqNum; //!< The message sequence number field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the granted party's ID field.
   * \returns The granted party's ID field.
   */
  virtual McpttFloorMsgFieldGrantedPartyId GetPartyId (void) const;
  /**
   * Gets the permission to request the floor field.
   * \returns The permission to request the floor field.
   */
  virtual McpttFloorMsgFieldPermToReq GetPermission (void) const;
  /**
   * Gets the message sequence number field.
   * \returns The message sequence number field.
   */
  virtual McpttFloorMsgFieldSeqNum GetSeqNum (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);
  /**
   * Sets the granted party's ID field.
   * \param partyId The granted party's ID field.
   */
  virtual void SetPartyId (const McpttFloorMsgFieldGrantedPartyId& partyId);
  /**
   * Sets the permission to request the floor field.
   * \param permission The permission to request the floor field.
   */
  virtual void SetPermission (const McpttFloorMsgFieldPermToReq& permission);
  /**
   * Sets the message sequence number field.
   * \param seqNum The message sequence number field.
   */
  virtual void SetSeqNum (const McpttFloorMsgFieldSeqNum& seqNum);
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class that represents a Floor Queue Position Request message.
 *
 * Subtype = 01000
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |          length               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  SSRC of floor participant requesting floor queue status info |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       User ID field                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * SSRC: The SSRC field shall carry the SSRC of the floor participant that is requesting information about its position in the floor request queue.
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT user ID of the floor participant sending the Floor Queue Position Request message.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function.
 */
class McpttFloorMsgQueuePositionRequest : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Queue Position Request message.
   */
  static const uint8_t SUBTYPE;
  /**
   * Gets the type ID of the McpttFloorMsgQueuePositionRequest class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgQueuePositionRequest class.
   * \param ssrc The SSRC of the floor control server.
   */
  McpttFloorMsgQueuePositionRequest (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgQueuePositionRequest class.
   */
  virtual ~McpttFloorMsgQueuePositionRequest (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldUserId m_userId; //!< The user ID field.

public:
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the user ID field.
   * \returns The user ID field.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Sets the user ID field.
   * \param userId The user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class that represents an MCPTT Floor Queue Position Info message.
 *
 * Subtype = x1001
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |          length               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |               SSRC of floor control server                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        User ID field                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |             SSRC of queued floor participant field            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                     Queued User ID field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        Queue Info field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Floor Indicator field                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * User ID: The User ID field is used in off-network only. It shall carry the MCPTT ID of the floor participant sending the Floor Queue Position Info message.
 * SSRC of queued floor participant: The SSRC of queued floor participant is only applicable in off-network and shall carry the SSRC of the queued floor participant.
 * Queued User ID: The Queued User ID field is used in off-network only. It shall carry the MCPTT ID of the queued floor participant.
 * Queue Info: The Queue Info field is only applicable in off-network and defines the queue position and granted floor priority in the queue.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function.
 */
class McpttFloorMsgQueuePositionInfo : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Queue Position Info message.
   */
  static const uint8_t SUBTYPE;
  /**
   * The subtype of the MCPTT floor granted message with acknowledgement.
   */
  static const uint8_t SUBTYPE_ACK;
  /**
   * Gets the type ID of the McpttFloorMsgQueuePositionInfo class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgQueuePositionInfo class.
   * \param ssrc The SSRC of the floor control server.
   */
  McpttFloorMsgQueuePositionInfo (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgQueuePositionInfo class.
   */
  virtual ~McpttFloorMsgQueuePositionInfo (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldUserId m_userId; //!< The MCPTT ID of the floor participant that sent this message.
  uint32_t m_queuedSsrc; //!< The SSRC of the queued floor participant.
  McpttFloorMsgFieldQueuedUserId m_queuedUserId; //!< The MCPTT ID of the queued floor participant.
  McpttFloorMsgFieldQueuePositionInfo m_queueInfo; //!< Defines the queue position and granted floor priority.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  McpttFloorMsgFieldIndic m_indicator; //!< The floor indicator field.

public:
  /**
   * Gets the MCPTT ID of the floor partipant that sent the message.
   * \returns The MCPTT ID.
   */
  virtual McpttFloorMsgFieldUserId GetUserId (void) const;
  /**
   * Gets the SSRC of the queued floor participant.
   * \returns The SSRC.
   */
  virtual uint32_t GetQueuedSsrc (void) const;
  /**
   * Gets the MCPTT ID of the queued floor participant.
   * \returns The MCPTT ID.
   */
  virtual McpttFloorMsgFieldQueuedUserId GetQueuedUserId (void) const;
  /**
   * Gets the field that the defines the queue position and granted floor priority.
   * \returns The queue info.
   */
  virtual McpttFloorMsgFieldQueuePositionInfo GetQueuePositionInfo (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the floor indicator field.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Sets the MCPTT ID of the floor participant that sent the message.
   * \param userId The MCPTT ID.
   */
  virtual void SetUserId (const McpttFloorMsgFieldUserId& userId);
  /**
   * Sets the SSRC of the queued floor participant.
   * \param queuedSsrc The SSRC.
   */
  virtual void SetQueuedSsrc (uint32_t queuedSsrc);
  /**
   * Sets the user id of the queued floor participant.
   * \param queuedUserId The MCPTT ID.
   */
  virtual void SetQueuedUserId (const McpttFloorMsgFieldQueuedUserId& queuedUserId);
  /**
   * Sets the field that defines the queue position and granted floor priority.
   * \param queueInfo The queue info.
   */
  virtual void SetQueuePositionInfo (const McpttFloorMsgFieldQueuePositionInfo& queueInfo);
  /**
   * Sets the floor indicator field.
   * \param indicator The floor indicator field.
   */
  virtual void SetIndicator (const McpttFloorMsgFieldIndic& indicator);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};
/**
 * A class used to represent an MCPTT Floor Ack message.
 *
 * Subtype = 01010
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P| Subtype |   PT=APP=204  |           length              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                SSRC of floor control server                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          name=MCPT                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Source field                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Message Type field                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Track Info field                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Subtype:The subtype shall be coded according to table 8.2.2-1.
 * Length: The length shall be coded as specified in to subclause 8.1.2.
 * SSRC: The SSRC field shall carry the SSRC of the floor control server.
 * Source: The Message Type Number field shall be coded as specified in to subclause 8.2.3.12.
 * Message Type: The Message Type Number field shall be coded as specified in to subclause 8.2.3.14.
 * Track Info: The Track Info field shall be included when an MCPTT call involves a non-Controlling function. The coding of the Track Info field is described in subclause 8.2.3.13.
 * Floor Indicator: The Floor Indicator field shall be coded as described in subclause 8.2.3.15.
 */
class McpttFloorMsgAck : public McpttFloorMsg
{
public:
  /**
   * The subtype of the MCPTT Floor Ack message.
   */
  static const uint8_t SUBTYPE;
  /**
   * Gets the type ID of the McpttFloorMsgAck class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgAck class.
   * \param ssrc The SSRC of the floor control server.
   */
  McpttFloorMsgAck (uint32_t ssrc = 0);
  /**
   * The destructor of the McpttFloorMsgAck class.
   */
  virtual ~McpttFloorMsgAck (void);
  /**
   * Reads the application-dependent data of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t ReadData (Buffer::Iterator& buff);
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the application-dependent data of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void WriteData (Buffer::Iterator& buff) const;
  /**
   * Updates the track info field.
   * \param trackInfo The track info field.
   */
  virtual void UpdateTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
  /**
   * Visits the given floor machine.
   * \param floorMsgSink The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMsgSink) const;

private:
  McpttFloorMsgFieldSource m_source; //!< The source field.
  McpttFloorMsgFieldType m_msgType; //!< The message type field.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.

public:
  /**
   * Gets the source field.
   * \returns The source field.
   */
  virtual McpttFloorMsgFieldSource GetSource (void) const;
  /**
   * Gets the message type field.
   * \returns The message type field.
   */
  virtual McpttFloorMsgFieldType GetMsgType (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Sets the source field.
   * \param source The source field.
   */
  virtual void SetSource (const McpttFloorMsgFieldSource& source);
  /**
   * Sets the message type field.
   * \param msgType The message type field.
   */
  virtual void SetMsgType (const McpttFloorMsgFieldType& msgType);

protected:
  /**
   * Sets the track info field.
   * \param trackInfo The track info field.
   */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_FLOOR_MSG_H */

