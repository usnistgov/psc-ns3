/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2020 University of Washington
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
 */
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

// Some elements ported from NIST ns-2.31 SIP implementation

#ifndef SIP_HEADER_H
#define SIP_HEADER_H

#include <ns3/header.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace sip {

/**
 * \ingroup sip
 *
 * This class implements a simplified SIP header as an ns3::Header object.
 * From RFC 3261, Sec 7.3:
 *
 *        generic-message  =  start-line
 *                            *message-header
 *                            CRLF
 *                            [ message-body ]
 *        start-line       =  Request-Line / Status-Line
 *        Request-Line     =  method name, Request-URI, protocol version
 *        Status-Line      =  SIP-version, Status-Code, Reason-Phrase
 */
class SipHeader : public Header
{
public:
  /**
   * \brief Construct a null header
   */
  SipHeader ();
  /**
   * \brief Destructor
   */
  virtual ~SipHeader ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  enum SipMessageType
  {
    SIP_REQUEST = 0,
    SIP_RESPONSE = 1,
    SIP_INVALID = 2
  };
  enum SipMethod
  {
    INVITE = 0,
    BYE = 1,
    ACK = 2,
    CANCEL = 3,
    INVALID_METHOD = 4
  };

  // These eight methods need doxygen
  void SetMessageType (SipMessageType messageType);
  SipMessageType GetMessageType (void) const;
  std::string GetMessageTypeName (void) const;

  void SetMethod (SipMethod method);
  SipMethod GetMethod (void) const;
  std::string GetMethodName (void) const;

  void SetStatusCode (uint16_t statusCode);
  uint16_t GetStatusCode (void) const;

  // Will eventually convert this to a string URI but for now MCPTT uses int
  void SetRequestUri (uint32_t requestUri);
  uint32_t GetRequestUri (void) const;

  void SetFrom (uint32_t from);
  uint32_t GetFrom (void) const;

  void SetTo (uint32_t from);
  uint32_t GetTo (void) const;

  void SetCallId (uint16_t callId);
  uint16_t GetCallId (void) const;

  // Documented in ns3::Header class
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t m_messageType {
    SIP_INVALID
  };
  uint8_t m_method {
    INVALID_METHOD
  };
  uint16_t m_statusCode {
    0xFFFF
  };
  uint32_t m_requestUri {
    0xFFFFFFFF
  };
  uint32_t m_from {
    0xFFFFFFFF
  };
  uint32_t m_to {
    0xFFFFFFFF
  };
  uint16_t m_callId {
    0xFFFF
  };
};

} // namespace sip

} // namespace ns3

#endif /* SIP_HEADER_H */
