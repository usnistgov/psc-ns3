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

#ifndef MCPTT_FLOOR_MSG_FIELD_H
#define MCPTT_FLOOR_MSG_FIELD_H

#include <iostream>
#include <vector>

#include <ns3/buffer.h>
#include <ns3/object-base.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

/**
 * MCPTT Floor field information.
 *
 * Each floor control specific field consists of n 8-bit field ID, an 8-bit
 * octet length value describing the length of the field value not including
 * the field ID or length value.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Field ID    | Length value  |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * :                      <    F i e l d  v a l u e   >            :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Field Name                      Field ID (decimal-binary)
 * Floor Priority                        000-00000000
 * Duration                              001-00000001
 * Reject Cause                          002-00000010
 * Queue Info                            003-00000011
 * Granted Party's Identity              004-00000100
 * Permission to Request the Floor       005-00000101
 * User ID                               006-00000110
 * Queue Size                            007-00000111
 * Message Sequence-Number               008-00001000
 * Queued User ID                        009-00001001
 * Source                                010-00001010
 * Track Info                            011-00001011
 * Message Type                          012-00001100
 * Floor Indicator                       013-00001101
 * SSRC                                  014-00001110
 */
/**
 * \ingroup mcptt
 *
 * This abstract class provides a common base for all off-network, MCPTT, floor
 * control message fields that are described in TS 24.380 v14.4.0.
 */
class McpttFloorMsgField : public ObjectBase
{
  friend std::ostream& operator<< (std::ostream& os, const McpttFloorMsgField& field);

public:
  /**
   * Gets the type ID of the McpttFloorMsgField class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgField class.
   */
  McpttFloorMsgField (void);
  /**
   * Creates an instance of the McpttFloorMsgField class.
   * \param id The field ID.
   * \param length The length of the field.
   */
  McpttFloorMsgField (uint8_t id, uint8_t length);
  /**
   * The destructor of the McpttFloorMsgField class.
   */
  virtual ~McpttFloorMsgField (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the field when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint8_t m_id; //!< The ID of the field.
  uint8_t m_length; //!< The length (in octets) of the field.

public:
  /**
   * Gets the ID of the field.
   * \returns The ID.
   */
  virtual uint8_t GetId (void) const;
  /**
   * Gets the length (in octets) of the field.
   * \returns The length (in octets).
   */
  virtual uint8_t GetLength (void) const;

protected:
  /**
   * Sets the ID of the field.
   * \param id The ID.
   */
  virtual void SetId (uint8_t id);
  /**
   * Sets the length (in octets) of the field.
   * \param length The length (in octets).
   */
  virtual void SetLength (uint8_t length);
};
/**
 * The overloaded output operator for the McpttFloorMsgField class.
 * \param os The output stream to the print the field to.
 * \param field The field to print.
 * \returns The output stream that the field field was printed to.
 */
std::ostream& operator<< (std::ostream& os, const McpttFloorMsgField& field);
/**
 * \ingroup mcptt
 *
 * A class used to represent the MCPTT floor priority field.
 *
 * Field ID = 000
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Floor Priority |Floor Priority |Floor Priority |spare          |
 * |field ID value |Length value   |value          |               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Floor Priority</b> value field shall consist of 16 bit parameter giving
 * the floor priority  ('0' to '255') where '0' is the lowest priority and '255' is
 * the highest priority.
 *
 */
class McpttFloorMsgFieldPriority : public McpttFloorMsgField
{
public:
  /**
  * Gets the type ID of the McpttFloorMsgFieldPriority class.
  * \returns The type ID.
  */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldPriority class.
   */
  McpttFloorMsgFieldPriority (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldPriority class.
   * \param priority The priority value.
   */
  McpttFloorMsgFieldPriority (uint8_t priority);
  /**
   * The destructor of the McpttFloorMsgFieldPriority class.
   */
  virtual ~McpttFloorMsgFieldPriority (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint8_t m_priority; //!< The priority value.

public:
  /**
   * Gets the floor priority value.
   * \returns The priority value.
   */
  virtual uint8_t GetPriority (void) const;
  /**
   * Sets the floor priority value.
   * \param priority The priority value.
   */
  virtual void SetPriority (uint8_t priority);
};

/**
 * A class used to represent the MCPTT Duration field
 *
 * Field ID = 001
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Duration       |Duration       |Duration value                 |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Duration</b> value is a binary value in seconds.
 */
class McpttFloorMsgFieldDuration : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldDuration class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldDuration class.
   */
  McpttFloorMsgFieldDuration (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldDuration class.
   * \param duration The duration (in seconds).
   */
  McpttFloorMsgFieldDuration (uint16_t duration);
  /**
   * The destructor of the McpttFloorMsgFieldDuration class.
   */
  virtual ~McpttFloorMsgFieldDuration (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_duration; //!< The duration (in seconds).

public:
  /**
   * Gets the duration value (in seconds).
   * \returns The value (in seconds).
   */
  virtual uint16_t GetDuration (void) const;
  /**
   * Sets the duration value (in seconds).
   * \param duration The value (in seconds).
   */
  virtual void SetDuration (uint16_t duration);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the MCPTT Reject Cause field.
 *
 * Field ID = 002
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Reject Cause   |Reject Cause   |       Reject Cause value      |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * :                   Reject Phrase value                         :
 * :                                                               |
 * |                                       (Padding)               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Reject Causes:
 * Cause #1 - Another MCPTT client has permission; The <b>Reject cause</b> value set to 1 indicates that another MCPTT user has permission to send a media.
 * Cause #2 - Internal floor control server error; The <b>Reject cause</b> value set to 2 indicates that the floor control server cannot grant the floor request due to an internal error.
 * Cause #3 - Only one participant;    The <b>Reject cause</b> value set to 3 indicates that the floor control server cannot grant the floor request, because the requesting party is the only participant in the MCPTT session.
 * Cause #4 - Retry-after timer has not expired; The <b>Reject cause</b> value set to 4 indicates that the floor control server cannot grant the floor request, because timer T9 (Retry-after) has not expired after permission to send media has been revoked.
 * Cause #5 - Receive only; The <b>Reject cause</b> value set to 5 indicates that the floor control server cannot grant the floor request, because the requesting party only has receive privilege.
 * Cause #6 - No resources available; The <b>Reject cause</b> value set to 6 indicates that the floor control server cannot grant the floor request due to congestion.
 * Cause #7  Queue full; The <b>Reject cause</b> value set to 7 indicates that the floor control server cannot queue the floor request, because the queue is full.
 * Cause #255 - Other reason; The <b>Reject cause</b> value set to 255 indicates that the floor control server does not grant the floor request due to the floor control server local policy.
 *
 * \todo Implement Reject Phrase (standard states that it is not mandatory though).
 */
class McpttFloorMsgFieldRejectCause : public McpttFloorMsgField
{
public:
  static const uint16_t CAUSE_1; //!< Another MCPTT client has permission.
  static const uint16_t CAUSE_2; //!< Internal floor control server error.
  static const uint16_t CAUSE_3; //!< Only one participant.
  static const uint16_t CAUSE_4; //!< Retry after timer has not expired.
  static const uint16_t CAUSE_5; //!< Receive only.
  static const uint16_t CAUSE_6; //!< No resources available.
  static const uint16_t CAUSE_7; //!< Queue full.
  static const uint16_t CAUSE_255; //!< Other reason.
  /**
   * Gets the type ID of the McpttFloorMsgFieldRejectCause class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldRejectCause class.
   */
  McpttFloorMsgFieldRejectCause (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldRejectCause class.
   * \param cause The reject cause value.
   */
  McpttFloorMsgFieldRejectCause (uint16_t cause);
  /**
   * The destructor of the McpttFloorMsgFieldRejectCause class.
   */
  virtual ~McpttFloorMsgFieldRejectCause (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_cause; //!< The reject cause value.

public:
  /**
   * Gets the reject cause value.
   * \returns The reject cause value.
   */
  virtual uint16_t GetCause (void) const;
  /**
   * Sets the reject cause value.
   * \param cause The reject cause value.
   */
  virtual void SetCause (uint16_t cause);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Queue Info field.
 *
 * Field ID = 003
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Queue Info     |Queue Info     |Queue Position |Queue Priority |
 * |field ID value |length value   |Info value     | Level value   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Queue Info length</b> value is a binary value and shall have the value 2 indicating the total length in octets of the <b>Queue position info</b> value and the <b>Queue Priority Level</b> value items.
 * The <b>Queue Position Info</b> value is a binary value. It shall have value 254 if the MCPTT client is not queued. It shall have the max value (255) if the MCPTT client is queued but the MCPTT server is unable to determine the queue position or if MCPTT server policy is not to release information of the queue position to the MCPTT client.
 * The <b>Queue Priority Level</b> value is coded as the <b>Floor Priority</b> value
 */
class McpttFloorMsgFieldQueuePositionInfo : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldQueuePositionInfo class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueuePositionInfo class.
   */
  McpttFloorMsgFieldQueuePositionInfo (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueuePositionInfo class.
   * \param position The queue position value.
   * \param priority The queue priority value.
   */
  McpttFloorMsgFieldQueuePositionInfo (uint8_t position, uint8_t priority);
  /**
   * The destructor of the McpttFloorMsgFieldQueuePositionInfo class.
   */
  virtual ~McpttFloorMsgFieldQueuePositionInfo (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint8_t m_position; //!< The queue position info value.
  uint8_t m_priority; //!< The queue priority value.

public:
  /**
   * Gets the queue position info value.
   * \returns The value.
   */
  virtual uint8_t GetPosition (void) const;
  /**
   * Gets the queue priority value.
   * \returns The priority value.
   */
  virtual uint8_t GetPriority (void) const;
  /**
   * Sets the queue position info value.
   * \param position The queue position info value.
   */
  virtual void SetPosition (uint8_t position);
  /**
   * Sets the queue priority value.
   * \param priority The queue priority value.
   */
  virtual void SetPriority (uint8_t priority);
};

/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Granted Party's Identity field.
 *
 * Field ID = 004
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Granted Party's|Granted Party's| Granted Party's Identity value|
 * |Identity field |Identity length|                               :
 * |ID             |value          |                               :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               :
 * :                                                               :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Granted Party's Identity</b> value is coded as the <b>User ID</b> value in subclause 8.2.3.8.
 * If the length of the <b>Granted Party's Identity</b> value is not a multiple of 4 bytes, the Granted Party's Identity field shall be padded to a multiple of 4 bytes. The value of the padding bytes should be set to zero. The padding bytes shall be ignored.
 */
class McpttFloorMsgFieldGrantedPartyId : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttGrandPartyIdentityField class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldGrantedPartyId class.
   */
  McpttFloorMsgFieldGrantedPartyId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldGrantedPartyId class.
   * \param partyId The granted party's identity value.
   */
  McpttFloorMsgFieldGrantedPartyId (uint32_t partyId);
  /**
   * The destructor of the McpttGrantedPartyField class.
   */
  virtual ~McpttFloorMsgFieldGrantedPartyId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint32_t m_partyId; //!< The granted party's identity value.

public:
  /**
   * Gets the granted party's identity value.
   * \returns The granted party's identity value.
   */
  virtual uint32_t GetPartyId (void) const;
  /**
   * Sets the granted party's identity value.
   * \param partyId The granted party's identity value.
   */
  virtual void SetPartyId (uint32_t partyId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Permision to Request the Floor field.
 *
 * Field ID = 005
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Permission to  |Permission to  | Permission to Request         |
 * |Request the    |Request the    | the Floor value               |
 * |Floor field ID |Floor length   |                               |
 * |               |value          |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Permission to Request the Floor length</b> value is a binary value and shall have the value 2 indicating the total length in octets of the <b>Permission to Request the Floor</b> value item.
 * The <b>Permission to Request the Floor</b> value is binary and coded as follows:
 *  0    The receiver is not permitted to request floor.
 *  1    The receiver is permitted to request floor.
 */
class McpttFloorMsgFieldPermToReq : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldPermToReq class.
   * \returns The type Id.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldPermToReq class.
   */
  McpttFloorMsgFieldPermToReq (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldPermToReq class.
   * \param permission The permission to request the floor value.
   */
  McpttFloorMsgFieldPermToReq (uint16_t permission);
  /**
   * The destructor of the McpttFloorMsgFieldPermToReq class.
   */
  virtual ~McpttFloorMsgFieldPermToReq (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_permission; //!< The permission to request the floor value.

public:
  /**
   * Gets the permission to request the floor value.
   * \returns The permission to request the floor value.
   */
  virtual uint16_t GetPermission (void) const;
  /**
   * Sets the permission to request the floor value.
   * \param permission The permission to request the floor value.
   */
  virtual void SetPermission (uint16_t permission);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT User ID field.
 *
 * Field ID = 006
 *
 * Coding
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |User ID         |User ID       |         User ID value         |
 * |field ID value  |length value  |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               :
 * :                                                               :
 * :                                                               |
 * |                                        Padding                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>User ID length</b> value is a binary value and shall include the value indicating the length in octets of the <b>User ID</b> value item except padding.
 * If the length of the <b>User ID</b> value is not a multiple of 4 bytes User ID field shall be padded to a multiple of 4 bytes. The value of the padding bytes should be set to zero. The floor control client shall ignore the value of the padding bytes.
 */
class McpttFloorMsgFieldUserId : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldUserId class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldUserId class.
   */
  McpttFloorMsgFieldUserId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldUserId class.
   * \param userId The user ID value.
   */
  McpttFloorMsgFieldUserId (uint32_t userId);
  /**
   * The destructor of the McpttFloorMsgFieldUserId class.
   */
  virtual ~McpttFloorMsgFieldUserId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

protected:
  /**
   * Creates an instance of the McpttFloorMsgFieldUserId class.
   * \param id The message ID value.
   * \param length The message length value.
   * \param userId The user ID value.
   */
  McpttFloorMsgFieldUserId (uint8_t id, uint8_t length, uint16_t userId);

private:
  uint32_t m_userId; //!< The user ID value.

public:
  /**
   * Gets the user ID value.
   * \returns The user ID value.
   */
  virtual uint32_t GetUserId (void) const;
  /**
   * Sets the user ID value.
   * \param userId The user ID value.
   */
  virtual void SetUserId (uint32_t userId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Queue Size field.
 *
 * Field ID = 007
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Queue Size     |Queue Size     |Queue Size value               |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Queue Size length</b> value is a binary value and shall have the value 2 indicating the total length in octets of the <b>Queue Size</b> value item.
 * The <b>Queue Size</b> value is a binary value.
 */
class McpttFloorMsgFieldQueueSize : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldQueueSize class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueueSize class.
   */
  McpttFloorMsgFieldQueueSize (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueueSize class.
   * \param queueSize The queue size value.
   */
  McpttFloorMsgFieldQueueSize (uint16_t queueSize);
  /**
   * The destructor of the McpttFloorMsgFieldQueueSize class.
   */
  virtual ~McpttFloorMsgFieldQueueSize (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_queueSize; //!< The queue size value.

public:
  /**
   * Gets the queue size value.
   * \returns The queue size value.
   */
  virtual uint16_t GetQueueSize (void) const;
  /**
   * Sets the queueSize value.
   * \param queueSize The queue size value.
   */
  virtual void SetQueueSize (uint16_t queueSize);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Message Sequence Number field.
 *
 * Field ID = 008
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Message        |Message        |Message Sequence Number value  |
 * |Sequence Number|Sequence Number|                               |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Message Sequence Number length</b> value is a binary value and shall have the value 2 indicating the total length in octets of the <b>Message Sequence Number</b> value item.
 * The <b>Message Sequence Number</b> value is a binary value. The <b>Message Sequence Number</b> value can be between 0 and 65535. When the 65535 value is reached, <b>Message Sequence Number</b> value starts from 0 again
 */
class McpttFloorMsgFieldSeqNum : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldSeqNum class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSeqNum class.
   */
  McpttFloorMsgFieldSeqNum (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSeqNum class.
   * \param seqNum The message sequence number value.
   */
  McpttFloorMsgFieldSeqNum (uint16_t seqNum);
  /**
   * The destructor of the McpttFloorMsgFieldSeqNum class.
   */
  virtual ~McpttFloorMsgFieldSeqNum (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_seqNum; //!< The message sequence number value.

public:
  /**
   * Gets the message sequence number value.
   * \returns The message sequence number value.
   */
  virtual uint16_t GetSeqNum (void) const;
  /**
   * Sets the message sequence number value.
   * \param seqNum The message sequence number value.
   */
  virtual void SetSeqNum (uint16_t seqNum);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Queued User ID field.
 *
 * Field ID = 009
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Queued User ID  |Queued User ID|  Queued User ID value         |
 * |field ID value  |length value  |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               :
 * :                                                               :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Queued User ID length</b> value is coded as the <b>User ID length</b> value in subclause 8.2.3.11.
 * The <b>Queued User ID</b> value is coded as the <b>User ID</b> value in subclause 8.2.3.11.
 * If the length of the <b>Queue User ID</b> value is not a multiple of 4 bytes, the Queue User ID field shall be padded to a multiple of 4 bytes. The value of the padding bytes should be set to zero. The padding bytes shall be ignored.
 */
class McpttFloorMsgFieldQueuedUserId : public McpttFloorMsgFieldUserId
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldQueuedUserId class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueuedUserId class.
   */
  McpttFloorMsgFieldQueuedUserId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldQueuedUserId class.
   * \param userId The queued user ID value.
   */
  McpttFloorMsgFieldQueuedUserId (uint32_t userId);
  /**
   * The destructor of the McpttFloorMsgFieldQueuedUserId class.
   */
  virtual ~McpttFloorMsgFieldQueuedUserId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Source field.
 *
 * Field ID = 010
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Source         |Source         |          Source value         |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Source length</b> value is a binary value and shall have the value 2 indicating the total length in octets of the <b>Source</b> value item.
 * The <b>Source</b> value is a 16 bit binary value where:
 *  0    the floor participant is the source
 *  1    the participating MCPTT function is the source
 *  2    the controlling MCPTT function is the source
 *  3    the non-controlling MCPTT function is the source
 * All other values are reserved for future use.
 */
class McpttFloorMsgFieldSource : public McpttFloorMsgField
{
public:
  static const uint16_t FLOOR_PARTICIPANT; //!< The floor participant source value.
  static const uint16_t PARTICIPATING_FUNCTION; //!< The participating MCPTT function source value.
  static const uint16_t CONTROLLING_FUNCTION; //!< The controlling MCPTT function source value.
  static const uint16_t NONCONTROLLING_FUNCTION; //!< The non-controlling MCPTT function source value.
  /**
   * Gets the type Id of the McpttFloorMsgFieldSource class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSource class.
   */
  McpttFloorMsgFieldSource (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSource class.
   * \param source The source value.
   */
  McpttFloorMsgFieldSource (uint16_t source);
  /**
   * The destructor of the McpttFloorMsgFieldSource class.
   */
  virtual ~McpttFloorMsgFieldSource (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint16_t m_source; //!< The source value.

public:
  /**
   * Gets the source value.
   * \returns The source value.
   */
  virtual uint16_t GetSource (void) const;
  /**
   * Sets the source value.
   * \param source The source value.
   */
  virtual void SetSource (uint16_t source);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Track Info field.
 *
 * Field ID = 011
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Track Info     |Track Info     |Queueing       |Participant    |
 * |field ID value |length value   |Capability     |Type Length    |
 * |               |               |value          |value          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Participant Type value                |
 * :                                                               :
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                  Floor Participant Reference 1                |
 * :                               |                               :
 * |                  Floor Participant Reference n                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Track Info length</b> value is a binary value and shall have a value indicating the total length in octets of the <b>Queueing Capability</b> value, <b>Track Info Priority Level</b> value and one or more <b>Floor Participant Reference</b> value items.
 * The <b>Queueing Capability</b> value is an 8 bit binary value where:
 *  0    the floor participant in the MCPTT client do not support queueing
 *  1    the floor participant in the MCPTT client do not support queueing
 * All other values are reserved for future use.
 * The spare bits are reserved for future use and shall be set to zero. The spare bits shall be ignored.
 * The <b>Participant Type Length</b> value is 8 bit binary value set to the length of the <b>Participant Type</b> value.
 * If the length of the <b>Participant</b> value is not a multiple of 4 bytes, the Track Info field shall be padded to a multiple of 4 bytes. The value of the padding bytes should be set to zero. The padding bytes shall be ignored.
 * NOTE 1: The content of the <b>Participant Type</b> value is MCPTT service provider specific and out of scope of the present document.
 * All other values are reserved for future use.
 * The <b>Floor Participant Reference</b> value is a 32 bit binary value containing a reference to the floor participant in the non-Controlling function of an MCPTT group.
 * NOTE 2: The reference to the floor participant is a value only understandable by the floor control server interface in the non-Controlling function of an MCPTT group.
 */
class McpttFloorMsgFieldTrackInfo : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldTrackInfo class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldTrackInfo class.
   */
  McpttFloorMsgFieldTrackInfo (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldTrackInfo class.
   * \param queueCap The queuing capability value.
   */
  McpttFloorMsgFieldTrackInfo (uint8_t queueCap);
  /**
   * The destructor of the McpttFloorMsgFieldTrackInfo class.
   */
  virtual ~McpttFloorMsgFieldTrackInfo (void);
  /**
   * Adds a reference to the collection of floor participant references.
   * \param ref The reference to add.
   */
  virtual void AddRef (uint32_t ref);
  /**
   * Removes all floor participant references from the collection.
   */
  virtual void ClearRefs (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the participant type length value including the number of octets needed for padding.
   * \returns The participant type length with padding octets included.
   */
  virtual uint8_t GetPtLengthWithPadding (void) const;
  /**
   * Gets the size (in bytes) of the field when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;
  /**
   * Updates the participant type length value.
   * \param ptLength The updated value to use.
   */
  virtual void UpdatePtLength (uint8_t ptLength);

private:
  uint8_t m_ptLength; //!< The participant type length value.
  uint8_t m_queueCap; //!< The queuing capability value.
  std::vector<uint32_t> m_refs; //!< The collection of floor participant references.

public:
  /**
   * Gets the participant type length value.
   * \returns The participant type length value.
   */
  virtual uint8_t GetPtLength (void) const;
  /**
   * Gets the queuing capability value.
   * \returns The queuing capability value.
   */
  virtual uint8_t GetQueueCap (void) const;
  /**
   * Gets the collection of floor participant references.
   * \returns The collection of floor participant references.
   */
  virtual std::vector<uint32_t> GetRefs (void) const;
  /**
   * Sets the queing capability value.
   * \param queueCap The queuing capability value.
   */
  virtual void SetQueueCap (uint8_t queueCap);

protected:
  /**
   * Sets the participant length value.
   * \param ptLength The participant length value.
   */
  virtual void SetPtLength (uint8_t ptLength);
  /**
   * Sets the collection of floor participant references.
   * \param refs The collection of floor participant references.
   */
  virtual void SetRefs (const std::vector<uint32_t>  refs);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Message Type field.
 *
 * Field ID = 012
 *
 * Coding:
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Message Type   |Message Type   |Message Type   |Spare          |
 * |field ID value |Length value   |value          |               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Message Type Length</b> value is a binary value and shall have the value 2.
 * The <b>Message Type</b> value is an 8 bit binary value containing the binary value of the message type as coded in table 8.2.3.1-2.
 */
class McpttFloorMsgFieldType : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldType class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldType class.
   */
  McpttFloorMsgFieldType (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldType class.
   * \param msgType The message type value.
   */
  McpttFloorMsgFieldType (uint8_t msgType);
  /**
   * The destructor of the McpttFloorMsgFieldType class.
   */
  virtual ~McpttFloorMsgFieldType (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  uint8_t m_msgType; //!< The message type value.

public:
  /**
   * Gets the message type value.
   * \returns The message type value.
   */
  virtual uint8_t GetMsgType (void) const;
  /**
   * Sets the message type value.
   * \param msgType The message type value.
   */
  virtual void SetMsgType (uint8_t msgType);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an MCPTT Floor Indicator field.
 *
 * Field ID = 013
 *
 * Coding:
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Floor Indicator|Floor Indicator|Floor Indicator value          |
 * |field ID value |Length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>Floor Indicator Length</b> value is a binary value and shall have the value 2.
 * The <b>Floor Indicator</b> value is a 16 bit bit-map named as shown:
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * A  =  Normal call
 * B  =  Broadcast group call
 * C  =  System call
 * D  =  Emergency call
 * E  =  Imminent peril call
 * F  =  Queueing supported
 * G  =  Dual floor
 * H  =  Temporary group call
 *
 * \note  The indicators C, D and E are only informative. There are no procedures specified for the C, D and E indicators in this release of the present document and the use of the indicators are implementation specific.
 * Bits F to P are reserved for future use and shall be set to 0.
 * There can be more than one bit set to 1 at the same time. The local policy in the floor control server decides which combinations are possible and the priority of the indications.
 */
class McpttFloorMsgFieldIndic : public McpttFloorMsgField
{
public:
  static const uint16_t BROADCAST_CALL; //!< The broadcast call bit pattern.
  static const uint16_t DUAL_FLOOR; //!< The dual floor bit pattern.
  static const uint16_t EMERGENCY_CALL; //!< The emergency call bit pattern.
  static const uint16_t IMMINENT_CALL; //!< The imminent call bit pattern.
  static const uint16_t NORMAL_CALL; //!< The normal call bit pattern.
  static const uint16_t SYSTEM_CALL; //!< The system call bit pattern.
  static const uint16_t QUEUING_SUPP; //!< The queuing request bit pattern.
  static const uint16_t TEMPORARY_GROUP_CALL; //!< The temporary group call bit pattern.
  /**
   * Gets the type ID of the McpttFloorMsgFieldIndic class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldIndic class.
   */
  McpttFloorMsgFieldIndic (void);
  /**
   * The destructor of the McpttFloorMsgFieldIndic class.
   */
  virtual ~McpttFloorMsgFieldIndic (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Sets all bits in the given pattern to 1.
   * \param pattern The given pattern.
   */
  virtual void Indicate (uint16_t pattern);
  /**
   * Indicates whether or not the given pattern has been indicated.
   * \param pattern The bit pattern to check for.
   * \returns True, if all bits in the given pattern are set.
   */
  virtual bool IsIndicated (uint16_t pattern) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;
  /**
   * Sets all bits in the given pattern to 0.
   * \param pattern The given pattern.
   */
  virtual void Unindicate (uint16_t pattern);

private:
  uint16_t m_flags; //!< The floor indicator value.

protected:
  /**
   * Gets the floor indicator value.
   * \returns the floor indicator value.
   */
  virtual uint16_t GetFlags (void) const;
  /**
   * Sets the floor indicator value.
   * \param flags The floor indicator value.
   */
  virtual void SetFlags (uint16_t flags);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent an SSRC field.
 *
 * Field ID = 006
 *
 * Coding
 *
 *   0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |SSRC           |SSRC           |SSRC value                     |
 * |field ID value |length value   |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |          SSRC value           |Spare                          |
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The <b>SSRC field ID</b> value is a binary value and is set according to table 8.2.3.1-2
 * The <b>SSRC length</b> value is a binary value of '6'.
 * The <b>SSRC</b> value is coded as the SSRC specified in IETF RFC 3550.
 */
class McpttFloorMsgFieldSsrc : public McpttFloorMsgField
{
public:
  /**
   * Gets the type ID of the McpttFloorMsgFieldSsrc class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSsrc class.
   */
  McpttFloorMsgFieldSsrc (void);
  /**
   * Creates an instance of the McpttFloorMsgFieldSsrc class.
   * \param ssrc The SSRC value.
   */
  McpttFloorMsgFieldSsrc (uint32_t ssrc);
  /**
   * The destructor of the McpttFloorMsgFieldSsrc class.
   */
  virtual ~McpttFloorMsgFieldSsrc (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the most derived TypeId for this Object.
   * \returns The TypeId associated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

protected:
  /**
   * Creates an instance of the McpttFloorMsgFieldSsrc class.
   * \param id The message ID value.
   * \param length The message length value.
   * \param ssrc The SSRC value.
   */
  McpttFloorMsgFieldSsrc (uint8_t id, uint8_t length, uint32_t ssrc);

private:
  uint32_t m_ssrc; //!< The SSRC value.

public:
  /**
   * Gets the SSRC value.
   * \returns The SSRC value.
   */
  virtual uint32_t GetSsrc (void) const;
  /**
   * Sets the SSRC value.
   * \param ssrc The SSRC value.
   */
  virtual void SetSsrc (uint32_t ssrc);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_FLOOR_MSG_FIELD_H */

