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

#ifndef MCPTT_CALL_MSG_FIELD_H
#define MCPTT_CALL_MSG_FIELD_H

#include <iostream>

#include <ns3/buffer.h>
#include <ns3/ipv4-address.h>
#include <ns3/nstime.h>
#include <ns3/object-base.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This abstract class provides a common base for all off-netork, MCPTT, call
 * control message fields that are described in TS 24.379 v14.4.0.
 */
class McpttCallMsgField : public ObjectBase
{
  friend std::ostream& operator<< (std::ostream& os, const McpttCallMsgField& field);

public:
  /**
   * Gets the type ID of the McpttCallMsgField class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief The destructor of the McpttCallMsgField class.
   */
  virtual ~McpttCallMsgField (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff) = 0;
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the size (in bytes) of the field when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const = 0;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const = 0;
  /**
   * Writes the contents of the field to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const = 0;
  /**
   * Creates an instance of the McpttCallMsgField class.
   */
  McpttCallMsgField (void);
};
/**
 * The overloaded output operator of the McpttCallMsgField class.
 * \param os The output stream to write to.
 * \param field The field to write to the stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCallMsgField& field);
/**
 * \ingroup mcptt
 *
 * A class used to represent a call control message field that has a variable
 * size.
 */
class McpttCallMsgFieldType6 : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMcpttDynamic class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief The destructor of the McpttCallMsgFieldType6 class.
   */
  virtual ~McpttCallMsgFieldType6 (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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

protected:
  /**
   * Creates an instance of the McpttCallMsgFieldType6 class.
   */
  McpttCallMsgFieldType6 (void);
  /**
   * Creates an instance of the McpttCallMsgFieldType6 class.
   * \param length The length of the contents.
   */
  McpttCallMsgFieldType6 (uint16_t length);

private:
  uint16_t m_length; //!< The length (in octets) of the contents.

public:
  /**
   * Gets the length (in octets) of the contents.
   * \returns The length (in octets).
   */
  virtual uint16_t GetLength (void) const;

protected:
  /**
   * Sets the length (in octets) of the contents.
   * \param length The length (in octets).
   */
  virtual void SetLength (uint16_t length);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Message Type field. This
 * field is used to identify the type of message.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldMsgType : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldMsgType class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldMsgType class.
   */
  McpttCallMsgFieldMsgType (void);
  /**
   * Creates an instance of the McpttCallMsgFieldMsgType class.
   * \param id The ID of the message type.
   */
  McpttCallMsgFieldMsgType (uint8_t id);
  /**
   * \brief The destructor of the McpttCallMsgFieldMsgType class.
   */
  virtual ~McpttCallMsgFieldMsgType (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint8_t m_type; //!< The type.

public:
  /**
   * Gets the type.
   * \returns The type.
   */
  virtual uint8_t GetType (void) const;
  /**
   * Sets the type.
   * \param type The type.
   */
  virtual void SetType (uint8_t type);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Call Identifier field. This
 * field is used to uniquely identify a call.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldCallId : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldCallId class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCallId class.
   */
  McpttCallMsgFieldCallId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCallId class.
   * \param callId The ID of the call.
   */
  McpttCallMsgFieldCallId (uint16_t callId);
  /**
   * \brief The destructor of the McpttCallMsgFieldCallId class.
   */
  virtual ~McpttCallMsgFieldCallId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint16_t m_callId; //!< The call identifier.

public:
  /**
   * Gets the call identifier.
   * \returns The call identifier.
   */
  virtual uint16_t GetCallId (void) const;
  /**
   * Sets the call identifier.
   * \param id The call identifier.
   */
  virtual void SetCallId (uint16_t id);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Refresh Interval field. The
 * refresh interval information identifier is used to indicate the minimum
 * time period between successive periodic messages.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldRefreshInterval : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldRefreshInterval class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldRefreshInterval class.
   */
  McpttCallMsgFieldRefreshInterval (void);
  /**
   * Creates an instance of the McpttCallMsgFIeldRefreshInterval class.
   * \param interval The refresh interval (in milliseconds).
   */
  McpttCallMsgFieldRefreshInterval (uint16_t interval);
  /**
   * \brief The destructor of the McpttCallMsgFieldRefreshInterval class.
   */
  virtual ~McpttCallMsgFieldRefreshInterval (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint16_t m_interval; //!< The call refresh interval (in milliseconds).

public:
  /**
   * Gets the call refresh interval (in milliseconds).
   * \returns The call refresh interval (in milliseconds).
   */
  virtual uint16_t GetInterval (void) const;
  /**
   * Sets the call refresh interval (in milliseconds).
   * \param interval The call refresh interval (in milliseconds).
   */
  virtual void SetInterval (uint16_t interval);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control MCPTT Group ID field. This
 * field is used to indicate the destination MCPTT group identifier.
 *
 * Type 6 IE
 * Format LV-E
 */
class McpttCallMsgFieldGrpId : public McpttCallMsgFieldType6
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldGrpId class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldGrpId class.
   */
  McpttCallMsgFieldGrpId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldGrpId class.
   * \param grpId The group ID.
   */
  McpttCallMsgFieldGrpId (uint32_t grpId);
  /**
   * \brief The destructor of the McpttCallMsgFieldGrpId class.
   */
  virtual ~McpttCallMsgFieldGrpId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint32_t m_grpId; //!< The group ID.

public:
  /**
   * Gets the group ID.
   * \returns The group ID.
   */
  virtual uint32_t GetGrpId (void) const;
  /**
   * Sets the group ID.
   * \param grpId The group ID.
   */
  virtual void SetGrpId (uint32_t grpId);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control SDP. This field is used to
 * contain an SDP message.
 *
 * The format and required fields were taken from RFC 4566 but specialized to
 * what is described in TS.24.379.200.
 *
 * Type 6 IE
 * Format LV-E
 */
class McpttCallMsgFieldSdp : public McpttCallMsgFieldType6
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldSdp class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldSdp class.
   */
  McpttCallMsgFieldSdp (void);
  /**
   * \brief The destructor of the McpttCallMsgFieldSdp class.
   */
  virtual ~McpttCallMsgFieldSdp (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Updates the McpttCallMsgFieldSdp instance using the string representation.
   * \param strRep The string representation;
   */
  virtual void FromStr (const std::string& strRep);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  /**
   * Gets the string representation of the McpttCallMsgFieldSdp class instance.
   * \returns The string representation.
   */
  virtual std::string ToStr (void) const;
  /**
   * Updates the length based on the current attributes.
   */
  virtual void UpdateLength (void);

private:
  uint16_t m_floorPort; //!< The port for floor control.
  Address m_grpAddr; //!< The address of the group.
  Address m_origAddr; //!< The address of the originator.
  uint16_t m_speechPort; //!< The port for MCPTT speech.

public:
  /**
   * Gets the port for floor control.
   * \returns The port number for floor control.
   */
  virtual uint16_t GetFloorPort (void) const;
  /**
   * Gets the address of the group.
   * \returns The group address.
   */
  virtual Address GetGrpAddr (void) const;
  /**
   * Gets the address of the originator.
   * \returns The originator's address.
   */
  virtual Address GetOrigAddr (void) const;
  /**
   * Gets the port for MCPTT speech.
   * \returns The port for MCPTT speech.
   */
  virtual uint16_t GetSpeechPort (void) const;
  /**
   * Sets the port for floor control.
   * \param floorPort The port for floor control.
   */
  virtual void SetFloorPort (uint16_t floorPort);
  /**
   * Sets the address of the group.
   * \param grpAddr The address of the group.
   */
  virtual void SetGrpAddr (const Address& grpAddr);
  /**
   * Sets the address of the originator.
   * \param origAddr The address of the originator.
   */
  virtual void SetOrigAddr (const Address& origAddr);
  /**
   * Sets the MCPTT speech port.
   * \param speechPort The MCPTT speech port.
   */
  virtual void SetSpeechPort (uint16_t speechPort);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Commencement Mode field. This
 * field is used to identify the type of commencement mode of the private
 * call.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldCommMode : public McpttCallMsgField
{
public:
  static const uint8_t AUTOMATIC_MODE; //!< Automatic commencement mode value.
  static const uint8_t MANUAL_MODE;   //!< Manual commencement mode value.
  /**
   * Gets the type ID of the McpttCallMsgFieldCommMode class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCommMode class.
   */
  McpttCallMsgFieldCommMode (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCommMode class.
   * \param commMode The commencement mode.
   */
  McpttCallMsgFieldCommMode (uint8_t commMode);
  /**
   * \brief The destructor of the McpttCallMsgFieldCommMode class.
   */
  virtual ~McpttCallMsgFieldCommMode (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint8_t m_mode; //!< The commencement mode value.

public:
  /**
   * Gets the commencement mode value.
   * \returns The value.
   */
  virtual uint8_t GetMode (void) const;
  /**
   * Sets the commencement mode value.
   * \param mode The value.
   */
  virtual void SetMode (uint8_t mode);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Reason field. This field is
 * used to indicate the reason of the reject.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldReason : public McpttCallMsgField
{
public:
  static const uint8_t REJECT;          //!< The "REJECT" value.
  static const uint8_t NO_ANSWER;       //!< The "NO ANSWER" value.
  static const uint8_t BUSY;            //!< The "BUSY" value.
  static const uint8_t SECURITY_FAILURE; //!< The "E2E SECURITY CONTEXT FAILURE" value.
  static const uint8_t FAILED;          //!< The "FAILED" value.
  /**
   * Gets the type ID of the McpttCallMsgFieldReason class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldReason class.
   */
  McpttCallMsgFieldReason (void);
  /**
   * Creates an instance of the McpttCallMsgFieldReason class.
   * \param reason The reason code.
   */
  McpttCallMsgFieldReason (uint8_t reason);
  /**
   * \brief The destructor of the McpttCallMsgFieldReason class.
   */
  virtual ~McpttCallMsgFieldReason (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint8_t m_reason; //!< The reason value.

public:
  /**
   * Gets the reason value.
   * \returns The value.
   */
  virtual uint8_t GetReason (void) const;
  /**
   * Sets the reason value.
   * \param mode The value.
   */
  virtual void SetReason (uint8_t mode);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Confirm Mode Indication field.
 * This field is used to indicate that the terminating MCPTT client is
 * expected to confirm call acceptance.
 *
 * Type 2 IE
 * Format T
 */
class McpttCallMsgFieldConfirmMode : public McpttCallMsgField
{
public:
  /**
   * Gets the IE identifier.
   * \returns The IEI.
   */
  static uint8_t GetIei (void);
  /**
   * Gets the type ID of the McpttCallMsgFieldConfirmMode class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldConfirmMode class.
   */
  McpttCallMsgFieldConfirmMode (void);
  /**
   * \brief The destructor of the McpttCallMsgFieldConfirmMode class.
   */
  virtual ~McpttCallMsgFieldConfirmMode (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control MCPTT User ID field. This field
 * is used to indicate an MCPTT user ID.
 *
 * Type 6 IE
 * Format LV-E
 */
class McpttCallMsgFieldUserId : public McpttCallMsgFieldType6
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldUserId class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldUserId class.
   */
  McpttCallMsgFieldUserId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldUserId class.
   * \param id Ther user ID.
   */
  McpttCallMsgFieldUserId (uint32_t id);
  /**
   * \brief The destructor of the McpttCallMsgFieldUserId class.
   */
  virtual ~McpttCallMsgFieldUserId (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint32_t m_id; //!< The user ID.

public:
  /**
   * Gets the user ID.
   * \returns The user ID.
   */
  virtual uint32_t GetId (void) const;
  /**
   * Sets the user ID.
   * \param id The user ID.
   */
  virtual void SetId (uint32_t id);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Call Type field. This field is
 * used to identify the type of the call.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldCallType : public McpttCallMsgField
{
public:
  static const uint8_t BASIC_GROUP;         //!< Represents a "BASIC GROUP CALL".
  static const uint8_t BROADCAST_GROUP;     //!< Represents a "BROADCAST GROUP CALL".
  static const uint8_t EMERGENCY_GROUP;     //!< Represents an "EMERGENCY GROUP CALL".
  static const uint8_t IMMINENT_PERIL_GROUP; //!< Represents an "IMMINENT PERIL GROUP CALL".
  static const uint8_t PRIVATE;             //!< Represents a "PRIVATE CALL".
  static const uint8_t EMERGENCY_PRIVATE;   //!< Represents an "EMERGENCY PRIVATE CALL".
  /**
   * Gets the name of the call type.
   * \param type The call type.
   * \returns The name of the call type.
   */
  static std::string GetCallTypeName (uint8_t type);
  /**
   * Gets the ProSe per-packet-priority for the given call type.
   * \param type The given call type.
   * \returns The PPP.
   */
  static uint8_t GetCallTypePriority (uint8_t type);
  /**
   * Gets the type ID of the McpttCallMsgFieldCallType class.
   * \returns The type ID.
   * \todo Assign correct priorities.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCallType class.
   */
  McpttCallMsgFieldCallType (void);
  /**
   * Creates an instance of the McpttCallMsgFieldCallType class.
   * \param type The call type value.
   */
  McpttCallMsgFieldCallType (uint8_t type);
  /**
   * \brief The destructor of the McpttCallMsgFieldCallType class.
   */
  virtual ~McpttCallMsgFieldCallType (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  uint8_t m_type; //!< The call type.

public:
  /**
   * Gets the call type.
   * \returns The call type.
   */
  virtual uint8_t GetType (void) const;
  /**
   * Sets the call type.
   * \param type The call type.
   */
  virtual void SetType (uint8_t type);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control User Location field. This field
 * is used to indicate the current location of the MCPTT client.
 *
 * Type 6 IE
 * Format TLV-E
 */
class McpttCallMsgFieldUserLoc : public McpttCallMsgFieldType6
{
public:
  /**
   * Gets the IE identifier.
   * \returns The IEI.
   */
  static uint8_t GetIei (void);
  /**
   * Gets the type ID of the McpttCallMsgFieldUserLoc class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldUserLoc class.
   */
  McpttCallMsgFieldUserLoc (void);
  /**
   * Creates an instance of the McpttCallMsgFieldUserLoc class.
   * \param loc The location of the user.
   */
  McpttCallMsgFieldUserLoc (const Vector& loc);
  /**
   * \brief The destructor of the McpttCallMsgFieldUserLoc class.
   */
  virtual ~McpttCallMsgFieldUserLoc (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the seriailzied size of this field.
   * \returns The seriailized size (in bytes).
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
  Vector m_loc; //!< The geographical location.

public:
  /**
   * Gets the geographical location.
   * \returns The geographical location.
   */
  virtual Vector GetLoc (void) const;
  /**
   * Sets the geographical location.
   * \param loc The geographical location.
   */
  virtual void SetLoc (const Vector& loc);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Organization Name field. This
 * field is used to indicate the name of the organization to which the user
 * belongs.
 *
 * Type 6 IE
 * Format LV-E
 */
class McpttCallMsgFieldOrgName : public McpttCallMsgFieldType6
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldOrgName class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldOrgName class.
   */
  McpttCallMsgFieldOrgName (void);
  /**
   * Creates an instance of the McpttCallMsgFieldOrgName class.
   * \param name The name of the organization.
   */
  McpttCallMsgFieldOrgName (const std::string& name);
  /**
   * \brief The destructor of the McpttCallMsgFieldOrgName class.
   */
  virtual ~McpttCallMsgFieldOrgName (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
   * Update the name.
   * \param orgName The name of the organization.
   */
  virtual void UpdateName (const std::string& orgName);

private:
  std::string m_name; //!< The name.

public:
  /**
   * Gets the name.
   * \returns The name.
   */
  virtual std::string GetName (void) const;
  /**
   * Sets the name.
   * \param name The name.
   */
  virtual void SetName (const std::string& name);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Call Start Time field. This
 * field is used to indicate the UTC time when a call was started.
 *
 * For simulation purposes, this field will actually hold the time that
 * the call was started using the simulation time. This field can only
 * consist of a five byte integer, which represents the number of
 * seconds, so only four bytes will actually be used to store the
 * time even though five will be deserialized and serialized by this
 * field.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldStartTime : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldStartTime class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldStartTime class.
   */
  McpttCallMsgFieldStartTime (void);
  /**
   * Creates an instance of the McpttCallMsgFieldStartTime class.
   * \param time The start time of the call.
   */
  McpttCallMsgFieldStartTime (const Time& time);
  /**
   * \brief The destructor of the McpttCallMsgFieldStartTime class.
   */
  virtual ~McpttCallMsgFieldStartTime (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  Time m_time; //!< The simulation at which the call started.

public:
  /**
   * Gets the simulation time at which the call started.
   * \returns The simulation time at which the call started.
   */
  virtual Time GetTime (void) const;
  /**
   * Sets the simulation time at which the call started.
   * \param time The time at which the call started.
   */
  virtual void SetTime (const Time& time);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Last Call Type Change time field.
 * This field is used to indicate the last UTC time when a call priority was
 * changed.
 *
 * For simulation purposes, this field will actually hold the time that
 * the last time the call type changed using the simulation time. This field
 * can only consist of a five byte integer, which represents the number of
 * seconds, so only four bytes will actually be used to store the
 * time even though five will be deserialized and serialized by this
 * field.
 *
 * Type 3 IE
 * Format V
 */
class McpttCallMsgFieldLastChgTime : public McpttCallMsgField
{
public:
  /**
   * Gets the type ID of the McpttCallMsgFieldLastChgTime class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldLastChgTime class.
   */
  McpttCallMsgFieldLastChgTime (void);
  /**
   * Creates an instance of the McpttCallMsgFieldLastChgTime class.
   * \param time The time at which the call type was changed.
   */
  McpttCallMsgFieldLastChgTime (const Time& time);
  /**
   * \brief The destructor of the McpttCallMsgFieldLastChgTime class.
   */
  virtual ~McpttCallMsgFieldLastChgTime (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
  Time m_time; //!< The simulation at which the call started.

public:
  /**
   * Gets the simulation time at which the call type changed.
   * \returns The simulation time.
   */
  virtual Time GetTime (void) const;
  /**
   * Sets the simulation time at which the call type change.
   * \param time The simulation time.
   */
  virtual void SetTime (const Time& time);
};
/**
 * \ingroup mcptt
 *
 * A class used to represent the call control Probe Response field. This field
 * is used to indicate that the "GROUP CALL ANNOUNCEMENT" message was sent in
 * response of a "GROUP CALL PROBE" message.
 *
 * Type 2 IE
 * Format T
 */
class McpttCallMsgFieldProbeResp : public McpttCallMsgField
{
public:
  /**
   * Gets the IE identifier.
   * \returns The IEI.
   */
  static uint8_t GetIei (void);
  /**
   * Gets the type ID of the McpttCallMsgFieldProbeResp class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallMsgFieldProbeResp class.
   */
  McpttCallMsgFieldProbeResp (void);
  /**
   * \brief The destructor of the McpttCallMsgFieldProbeResp class.
   */
  virtual ~McpttCallMsgFieldProbeResp (void);
  /**
   * Reads the contents of the field from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Get the most derived TypeId for this Object.
   * \returns The TypeId assocated to the most-derived type of this instance.
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
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MSG_FIELD_H */

