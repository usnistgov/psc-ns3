/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef PSC_APPLICATION_H
#define PSC_APPLICATION_H

#include "ns3/seq-ts-size-header.h"
#include "ns3/traced-callback.h"
#include "ns3/application.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 * \brief Packet id for PSC applications
 *
 * This class provides a sequential packet number that can be used for packet IDs
 */
class PscSequenceNumber
{
public:
  /**
   * Variable with the last packet ID generated
   */
  static uint64_t s_sequenceNumber;
  /**
   * Return a new packet ID
   * \return A new packet ID value
   */
  static uint64_t GetSequenceNumber ()
  {
    return ++s_sequenceNumber;
  }
};

/**
 * \ingroup pscincident
 *
 * Class that provides a common base for the Applications
 * used for incident modeling.
 *
 * The purpose of this class is to provide a common parent class, the
 * definition of those members used by the helpers, and to provide common
 * trace sources.
 *
 * THIS CLASS IS NOT INTENDED TO BE INSTANTIATED.
 *
 */
class PscApplication : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Default constructor
   */
  PscApplication ();
  /**
   * Default destructor
   */
  virtual ~PscApplication ();

  /**
   * Stop the application. This cancels the stop event at the original stop time
   * and invokes the StopApplication method
   */
  virtual void StopNow () = 0;

protected:
  /**
   * Method used to break reference cycles between smart pointers.
   *
   * Inherited from the ns3::Application parent class.
   */
  virtual void DoDispose (void);

  /**
   * Application name
   */
  std::string m_appName;

  /**
   * TID of the socket to be used.
   * Needed because we may be using TCP or UDP sockets.
   */
  TypeId m_socketTid;

  /**
   *
   * Trace source for transmitted packets.
   * Assumes that the packet has a SeqTsSizeHeader
   */
  TracedCallback<std::string, SeqTsSizeHeader> m_txTrace;
  /**
   * Trace source for received packets
   * Assumes that the packet has a SeqTsSizeHeader
   */
  TracedCallback<std::string, SeqTsSizeHeader> m_rxTrace;
  /**
   * Trace source for the start and stop time of the application
   */
  TracedCallback<std::string, Time, Time> m_startStopTimeTrace;

  /**
   * Type definition for the trace source for transmitted packets.
   */
  typedef void (*TxTracedCallback)(std::string, SeqTsSizeHeader);
  /**
   * Type definition for the trace source for received packets.
   */
  typedef void (*RxTracedCallback)(std::string, SeqTsSizeHeader);
  /**
   * Type definition for the trace source for start and stop time.
   */
  typedef void (*TimeTracedCallback)(std::string, Time, Time);

}; // class PscApplication

} // namespace psc
} // namespace ns3

#endif // PSC_APPLICATION_H

