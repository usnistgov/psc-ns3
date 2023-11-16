NR Sidelink HARQ
----------------

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

.. _NrSidelinkHarqModelDescription:

Model Description
*****************

Hybrid Automatic Repeat ReQuest (HARQ) for NR Sidelink provides improved
probability of transport block delivery at the expense of additional
transmissions on the channel.  HARQ combines forward error correction (FEC)
techniques with automatic repeat request (ARQ) retransmission techniques
using positive acknowledgments.

HARQ is used in 4G LTE and 5G NR, on both the uplink and downlink.  Additional
modes of operation are defined for NR sidelink.  HARQ is a somewhat
complicated process with elements located at different parts of the NR
stack.  Using typical terminology, a HARQ Entity manages a number of HARQ
Processes and Buffers that interact with the scheduler, MAC, and PHY layers.

Sidelink HARQ is implemented in the following ``nr`` module source files:

- nr-ue-mac.cc
- nr-sl-comm-resource-pool-factory.{cc,h}
- nr-sl-ue-mac-sched-sap.h
- nr-sl-ue-mac-scheduler-simple.cc
- nr-phy-mac-common.h
- nr-sl-phy-mac-common.h
- nr-ue-phy-mac-common.{cc,h}
- nr-control-messages.{cc,h}

The sidelink HARQ support leverages previous UL/DL HARQ support in the
following ``nr`` module source files:

- nr-harq-phy.{cc,h}
- nr-mac-harq-process.h
- nr-mac-harq-vector.{cc,h}
- nr-mac-scheduler-harq-rr.{cc,h}
- nr-sl-ue-mac-harq.{cc,h}

Sidelink HARQ support is also implemented in the following ``lte`` module source files:

- lte-rrc-sap.{cc,h}
- nr-comm-resource-pool-factory.{cc,h}

.. _NrSidelinkHarqDesign:

Design
======

The following capabilities are implemented, following 3GPP TS 38.321, section
5.22.1.3:

- A sidelink HARQ entity with a configurable number of sidelink HARQ processes;
  the default for mode 2 is four processes.

- Each sidelink process supports one transport block (TB), and a HARQ process
  ID is assigned to the MAC PDU, and the sidelink process is associated with
  a HARQ buffer.

- HARQ feedback can be enabled or disabled for groupcast, unicast, or broadcast.

- For groupcast, the positive-negative acknowledgment option is implemented;
  all receivers will return either a positive or negative acknowledgment
  (negative acknowledgment relies on successful SCI decode).

- Support of configuring a feedback channel (PSFCH) with a periodicity of
  one, two, or four sidelink slots.  The PSFCH can deliver HARQ feedback
  from a receiver to a sender using an idealized (i.e., no error model)
  channel.  The PSFCH logically occupies one symbol of a slot, but due to
  guard interval, three symbols for that slot become unusable for PSSCH.

- Support of MinTimeGapPsfch parameter of either two or three slots, to
  ensure that HARQ feedback is delayed due to notional processing delays.

- Scheduling support to ensure that constraints are met around scheduling
  future resources for HARQ retransmissions.

PSFCH slots
###########

The pattern of slots that correspond to sidelink slots is established by
the TDD pattern establishing the UL slots, and the sidelink bitmap that
is overlaid on the UL slots.  PSFCH, if enabled, is present in every
one, two, or four sidelink slots.  If PSFCH periodicity is set to zero slots,
HARQ feedback will be disabled (although blind transmissions are still
possible).

A slot with PSFCH will consume three symbols; one for the PSFCH slot
itself, one for the AGC (which duplicates the PSFCH), and an additional
guard slot.

The scheduler needs to be aware of the SL pattern as well as of the
PSFCH pattern, to determine which slots are candidates for possible
retransmisssions, because the scheduler must allow for enough time
for a transmission of a transport block to be acknowledged before
attempting a HARQ retransmission.  Additionally, the receiver must
know the PSFCH pattern in order to schedule a feedback transmission.
In ns-3, the class ``NrSlCommResourcePool`` was extended with a
``SlotHasPsfch()`` method, complementing the ``IsSidelinkSlot()`` method,
to allow ns-3 models to determine whether any given slot
is a PSFCH-enabled slot.

Retransmission Scheduling Constraints
#####################################

According to TS 38.321 section 5.22.1, when the scheduler must configure
a grant based on sensing and/or random selection, and when HARQ is
enabled and the bandwidth pool is configured for PSFCH, the scheduler
will select resources for the sidelink data such that the following
constraints are honored:

1. the packet delay budget is not exceeded,

2. the maximum number of transmissions is not exceeded,

3. the resources selected can be indicated by a prior SCI transmission, and

4. the time gap between selected resources is at least as large as the
   minimum time gap between such resources.

The standard describes this minimum time gap as follows:

.. code-block:: shell

  -	a time gap between the end of the last symbol of a PSSCH transmission of the first resource and the start of the first symbol of the corresponding PSFCH reception determined by sl-MinTimeGapPSFCH and sl-PSFCH-Period for the pool of resources; and
  -	a time required for PSFCH reception and processing plus sidelink retransmission preparation including multiplexing of necessary physical channels and any TX-RX/RX-TX switching time.
  NOTE 4:	How to determine the time required for PSFCH reception and processing plus sidelink retransmission preparation is left to UE implementation.

This essentially means that the scheduler must ensure that there exists
enough time for HARQ feedback to be generated, sent back to the
transmitter, and processed, before the next retransmission must be prepared.
If HARQ feedback indicates that no further retransmissions are needed,
the future resources reserved for that sidelink data will go unused.
This is a lost sending opportunity but will reduce possible interference.
Additionally, note that the number of slots required to wait for
this minimum time gap depends on the specific TDD patterns of future
SL and PSFCH-enabled slots, and varies over time.
The ``NrSlUeMacSchedulerSimple`` scheduler class contains logic to
select slots for HARQ retransmissions subject to this scheduling
constraint.

It is possible that HARQ feedback may be delayed due to an internal
collision on the receiver-- because the system is half-duplex, the receiver
must make a priority decision whether to transmit or receive on a given
PSFCH symbol.  It could be the case that the scheduler plans for a
given PSFCH to deliver feedback, but the receiver decides not to send it.
The current ns-3 model does not yet implement handling for such collisions,
but even if/when it is implemented, the penalty for deferring the sending
of HARQ feedback may be an unnecessary retransmission by the sender,
which should not be much of a concern.

Path of a Packet
################

A packet is handled by HARQ aspects of the model as follows.  In the
example, the bearers are activated at time 1.91 seconds, and the application
starts sending data at 2.01 seconds.

At time 1.91 seconds, upon bearer activation, the logical channel group
and logical channel are set up for the destination.  This activity can
be seen by enabling the log component ``NrSlUeMacSchedulerNs3``:

.. code-block:: shell

  +1.910000000s -1 NrSlUeMacSchedulerNs3:DoCschedUeNrSlLcConfigReq(): [DEBUG] Created new NR SL LCG for destination 224 LCG ID =3
  +1.910000000s -1 NrSlUeMacSchedulerNs3:DoCschedUeNrSlLcConfigReq(): [INFO ] Added LC id 4 in LCG 3 cast 2 harq 1 budget +20ms

The above statements show that Logical Carrier ID (LCID) 4 was allocated to 
Logical Carrier Group (LCG) 3, for destination Layer-2 ID of 224.  It also
indicates that the cast type is 'Groupcast' (type 2 in the code), that
HARQ feedback is enabled, and that the packet delay budget is 20 ms.
This results from the following configuration in the 
``sidelink-harq-example.cc`` program:

.. code-block:: cpp

  std::string castType = "groupcast";
  bool harqEnabled = true;
  ... 
  Time delayBudget = MilliSeconds (20);
  ... 
  uint32_t dstL2Id = 224;
  ...
  tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, LteSlTft::CastType::Groupcast, groupAddress6, dstL2Id, harqEnabled, delayBudget);
  nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmitDevices, tft);
  tft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, LteSlTft::CastType::Groupcast, groupAddress6, dstL2Id, harqEnabled, delayBudget);
  nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receiveDevices, tft);
  

At time 2.01, when the packet arrives at the
RLC layer, it generates a buffer status report, which can be observed
in the log component ``LteRlcUm``:

.. code-block:: shell

  +2.010000000s 0 LteRlcUm:DoReportNrSlBufferStatus(): [LOGIC] Send NR SL ReportBufferStatus = 235, 0

In traditional on-network configuration (DL/UL), a UE buffer status report is
sent to the scheduler at the gNB.  Here, for mode 2 operation with grant
configuration based on sensing and performed by the MAC layer, the buffer
status report is sent to the scheduler associated with the MAC.  This can
be observed in the log component for ``NrSlUeMacSchedulerNs3``:

.. code-block:: shell

  +2.010000000s 0  [ CellId 0, bwpId 0, rnti 1] NrUeMac:DoReportNrSlBufferStatus(): [INFO ] Reporting for Sidelink. Tx Queue size = 235
  +2.010000000s 0 NrSlUeMacSchedulerNs3:DoSchedUeNrSlRlcBufferReq(): [INFO ] Updating NR SL LC Info: RNTI: 1 LCId: 4 RLCTxQueueSize: 235 B, RLCTXHolDel: 0 ms, RLCReTXQueueSize: 0 B, RLCReTXHolDel: 0 ms, RLCStatusPduSize: 0 B, source layer 2 id: 1, destination layer 2 id 255 in LCG: 3
 
A few key details are worth noting:

* the Logical Channel ID (LCID) is set to 4.  The NR V2X model will assign these IDs from a range of 4-19. 
* the transmit queue size is 235 bytes.  This results from a default UDP packet size of 200 bytes (application layer), plus 28 bytes of UDP and IP headers, plus5 bytes for PDCP header, plus 2 bytes estimated header size.
* the source Layer-2 ID is 1 (see below).  
* the destination Layer-2 ID is 255, which was set in the program code's TFT configuration.

The setting of source Layer-2 ID is driven by the RRC layer and the
``NrSlHelper``.  In ``NrSlHelper::PrepareSingleUeForSidelink()``, the following
code is executed:

.. code-block:: cpp

  //Since now all the BWP for SL are configured, we can configure src L2 id
  //for only SL BWP (s) (see LteUeRrc::DoSetSourceL2Id)
  uint64_t imsi = lteUeRrc->GetImsi ();
  NS_ASSERT_MSG (imsi != 0, "IMSI was not set in UE RRC");
  nrSlUeRrc->SetSourceL2Id (static_cast <uint32_t> (imsi & 0xFFFFFF)); //use lower 24 bits of IMSI as source

In turn, the UE RRC will access the MAC via the ``NrSlUeCmacSapProvider``
service access point (SAP) and set the source ID of the ``NrUeMac`` instance.

The next time of interest is time 2.011 seconds, which is the first sidelink
slot after the buffer request.  Sensing and filtering of transmit opportunities
leads to the NrUeMac instance calling the scheduler with a "trigger request".
The ``NrUeMac``
provides a list of candidate slots from within the sensing window that
meet the sensing criteria.

.. code-block:: shell

  +2.011000000s 0  [ CellId 0, bwpId 0, rnti 1] NrUeMac:DoNrSlSlotIndication(): [INFO ] IMSI 1 scheduling the destination 224 for 14 TxOpps


This is seen in the class ``NrSlUeMacSchedulerNs3``:

.. code-block:: shell

  +2.011000000s 0 NrSlUeMacSchedulerNs3:DoSchedUeNrSlTriggerReq(0x55c4ef08d480, 224)

This method calls the virtual ``NrSlUeMacScheduler::DoNrSlAllocation`` which
selects the slots in the window that will be used for the transmission.

The ``DoNrSlAllocation()`` method is specialized by each scheduler
implementation.  In the current scheduler ``NrSlUeMacSchedulerSimple``,
it will perform one of the following scheduling operations:

- If HARQ is disabled, one slot will be randomly selected
- If HARQ is enabled and the cast type is broadcast, then slots will be
  randomly selected from the available slots up until the
  ``slMaxNumPerReserve`` number of slots have been selected, or the
  number of available slots is reached (blind retransmission).
  The slots selected also have to conform to constraints about signaling
  SCH resources in the SCI-1 and SCI-2A information.
- If HARQ is enabled and the cast type is unicast or groupcast, then
  the first slot will be randomly selected, and subsequent slots will
  be tried at random up to ``slMaxNumPerReserve`` slots or the maximum
  number of available slots.  However, an additional constraint is imposed,
  such that each slot selected satisfies conditions described in
  TS 38.321-- namely, that retransmissions are not scheduled before
  a sufficient amount of time has elapsed to allow for the receiver(s)
  to successfully ACK or NACK the transmission.  The calculation of a
  sufficient amount of time is based on the sidelink and PSFCH transmission
  pattern and other factors.

Note that presently, no prioritization is performed; the scheduler is asked
to schedule each destination one-by-one (if more than one exist) by the
``NrUeMac``, rather than jointly scheduling across all destinations with
data to send.

Once scheduling is completed, the scheduler calls 
``NrUeMac::SchedUeNrSlConfigInd`` which triggers the creation of grants
as far into the future as the reselection window allows.

The next key time is the first slot in the grant (first transmit opportunity).
As usual, at the start of any sidelink slot, ``NrUeMac::DoNrSlotIndication``
is called.  The first transmission causes a HARQ process ID to be assigned,
and the packet is added to the HARQ buffer, as can be seen in the
log component ``NrSlUeMacHarq``:

.. code-block:: shell

  +2.012250000s 0 NrSlUeMacHarq:AssignNrSlHarqProcessId(0x558bae733c00, 224)
  +2.012250000s 0 NrSlUeMacHarq:AddPacket(0x558bae733c00, 224, 4, 0)

The operation of assigning the process ID causes a (20 ms) timer to be set to
handle the case in which HARQ feedback is not received.

We next describe what happens to the single packet sent with HARQ enabled
when it is successfully decoded at the receiver upon first reception.
Successful decoding
occurs if the SCI-1 and SCI-2A are both successfully decoded, and if the
PSSCH is successfully decoded.  This case can be simulated with the following
command:

.. code-block:: shell

  $ ./waf --run 'sidelink-harq-example --castType=groupcast --numPackets=1'

Decoding the control channels and the packets is the responsibility of
the ``NrSpectrumPhy`` object, which has associated with it a ``NrHarqPhy``
object.  The ``NrHarqPhy`` object maintains the receiver-side reception
history.  Internally, the ``NrHarqPhy`` maintains an unordered map of
HARQ history for each UE (indexed by RNTI), and then for each UE, a second
unordered map is maintained of history for each HARQ process ID.

In the packet burst that carries the ``NrSlSciF2aHeader`` along with the
packets representing the ``PSSCH``, the new data indicator (NDI) will cause
the reset of the ``NrHarqPhy`` state for that RNTI and HARQ process ID.

.. code-block:: cpp

  if (sciF2a.GetNdi ())
    {
       NS_LOG_DEBUG ("RemovePrevDecoded: " << +sciF2a.GetHarqId () << " for the packets received from RNTI " << tbIt.first << " rv " << +sciF2a.GetRv ());
       m_harqPhyModule->RemovePrevDecoded (tbIt.first, sciF2a.GetHarqId ());
     }

The ``NrSpectrumPhy`` then retrieves the ``NrHarqPhy`` history to make decisions
on whether the TB was successfully decoded:

.. code-block:: shell

  +2.011982140s 1 NrHarqPhy:GetHarqProcessInfoSl(0x556f219d2190)

It should be noted that at the time of this writing, |ns3| does not have
a specialized error model for sidelink channels but instead reuses the
uplink channel error model.

Finally, if the decoding was successful, the HARQ history for that process
ID and RNTI is marked as having been previously decoded, to ensure that
possible future blind retransmissions (or retransmissions that occurred
despite positive HARQ feedback) are not delivered to the ``NrPhy``:

.. code-block:: shell

  +2.011982140s 1 NrHarqPhy:ResetSlDataHarqProcessStatus(0x556f219d2190)
  +2.011982140s 1 NrHarqPhy:ResetHarqProcessStatus(0x563cc7a9b190)

The ``NrSpectrumPhy`` next uses a callback ``SlHarqFeedback`` to indicate that
a positive ACK should be sent at the next opportunity, and also delivers
the successfully decoded TB to the ``NrUePhy`` and stored in a FIFO queue:

.. code-block:: shell

  +2.011982140s 1 NrSpectrumPhy:RxSlPssch(): [DEBUG] Sending ACK HARQ feedback to SlHarqFeedback callback
  +2.011982140s 1 NrSpectrumPhy:RxSlPssch(): [DEBUG] SpectrumPhy dispatching a non corrupted TB to UE PHY

The log components of ``NrUePhy`` reflect these events:

.. code-block:: shell

  +2.011982140s 1  [ CellId 0, bwpId 0] NrUePhy:EnqueueSlHarqFeedback(0x5640bc908520)
  +2.011982140s 1  [ CellId 0, bwpId 0] NrUePhy:EnqueueSlHarqFeedback(): [DEBUG] Received SL HARQ ACK in slot 8047 for process 0
  +2.011982140s 1  [ CellId 0, bwpId 0] NrUePhy:PhyPsschPduReceived(0x5640bc908520)

The ``NrUePhy`` must next send the queued HARQ feedback at the appropriate time.
Two conditions must hold:

1.  The slot contains a PSFCH symbol, and
2.  The minimum time gap to send on PSFCH must be satisfied.

Note that it is possible in the standard for queued HARQ feedback to be
preempted by the desire for the UE to listen for feedback (of a higher
priority signal); however, this is not modelled in the |ns3| model.

The ``NrUeMac`` indicates to the ``NrUePhy`` if there is a PSFCH opportunity
when it calls ``NrUePhy::StartNrSlVarTti()``.  When ``NrUePhy::SlFeedback``
is called, the following is checked for condition 2 above:

.. code-block:: cpp

  std::list<Ptr<NrSlHarqFeedbackMessage> > feedbackList;
  uint8_t gap = m_slTxPool->GetMinTimeGapPsfch (GetBwpId (), m_nrSlUePhySapUser->GetSlActiveTxPoolId ());
  auto it = m_slHarqFbList.begin ();
  while (it != m_slHarqFbList.end ())
    {
      if (m_currentSlot.Normalize () >= gap + it->first.Normalize ())
        {
          NS_LOG_DEBUG ("Inserting HARQ FB to packet burst from slot "
          ...
          feedbackList.emplace_front (it->second);

Next, it could be the case that an older NACK (unsuccessful decode) was
later overridden by a successful decode (ACK).  The code delivers only
the latest feedback by iterating the feedback list to create a unique
list.  If this list is non-empty, the code schedules (with a small amount
of delay (1 nanosecond)) to send the sidelink feedback:

.. code-block:: cpp

  Simulator::Schedule (NanoSeconds (1.0), &NrUePhy::SendNrSlFbChannels, this,
                      uniqueFeedbackList, varTtiDuration - NanoSeconds (2.0),
                      varTtiInfo);

The TTI info is set in the ``NrUeMac`` to always occur in symbol 12.

.. code-block:: cpp

  NrSlVarTtiAllocInfo feedbackVarTtiInfo;
  feedbackVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::FEEDBACK;
  feedbackVarTtiInfo.symStart = 12; // PSFCH is always in slot 12
  feedbackVarTtiInfo.symLength = 1;
  // Current NR sidelink code assumes that all of the RBs in a BWP
  // are used; so the rbStart will always be zero and the rbLength
  // will be the number of RBs in the BWP
  feedbackVarTtiInfo.rbStart = 0;
  feedbackVarTtiInfo.rbLength = GetTotalSubCh (m_poolId) * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
  m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo (sfn, feedbackVarTtiInfo);

The delay of one nanosecond in scheduling allows the rest of 
``NrUeMac::DoNrSlSlotIndication()`` to finish before anything is sent, because
possibly data and control messages will also be scheduled for this slot.
``NrSlPhySlotAlloc`` is a std::set, so it is fine to add PSFCH before SCH/CCH
because it will be ordered.
The corresponding log statements are:

.. code-block:: shell

  +2.016214284s 1  [ CellId 0, bwpId 0] NrUePhy:SlFeedback(): [DEBUG] Inserting HARQ FB to packet burst from slot 8047 for rnti 2 dstL2Id 224 harqProcessId 0 bwpIndex 0 status ACK
  +2.016214284s 1  [ CellId 0, bwpId 0] NrUePhy:SlFeedback(): [DEBUG] Preparing HARQ feedback for RNTI 2 HARQ PID 0
  +2.016214284s 1  [ CellId 0, bwpId 0] NrUePhy:SlFeedback(): [DEBUG] UE2 TXing NR SL FEEDBACK frame for symbols 12-12     start 2.01621 end 2.01623
  +2.016214284s 1  [ CellId 0, bwpId 0] NrUePhy:StartNrSlVarTti(): [DEBUG] Scheduling EndNrSlVarTti at time +2.01623s

Note that the fifth argument to Schedule() is the notional duration of the
transmission minus two nanoseconds.  This will cause ``NrUePhy`` to end
one nanosecond earlier than the notional duration of the signal (because
one of the two nanoseconds is offset by the nanosecond dela in scheduling
the send event.  This timing pattern was inherited by the original ``NrUePhy``
code (see also ``NrUePhy::UlData``) and is not documented, but probably
is scheduled in this way to ensure that events are ordered in a particular
way.

The code next calls ``NrSpectrumPhy::StartTxSlFeedback`` to send the feedback
over the channel.  The feedback is encapsulated in a structure called
``NrSpectrumSignalParametersSlFeedback``.

The recipient of the feedback (i.e., the original sender) starts by receiving
the signal from the ``SpectrumChannel``; the end of frame event is
``NrSpectrumPhy::EndRxSlFrame`` which calls ``NrSpectrumPhy::RxSlPsfch``.
If there were an error model for the PSFCH channel, it would be used in
this method to determine the success or failure in decoding; however, at
present, the |ns3| model for PSFCH assumes an ideal feedback channel.
The ``RxSlPsfch`` simply delivers the feedback to a callback that is bound to 
``NrUePhy::PhyPsfchReceived``, which then dispatches this to the MAC layer
via the NR SL UE PHY service access point (SAP) after an assumed PSFCH
decode latency of one slot.

The ``NrUeMac`` then sends this feedback to the ``NrSlUeMacHarq`` object.
The feedback is matched with the sending HARQ buffer by the HARQ process
ID and the destination L2 ID.  Possibly stale HARQ information (indicated
by the availability of the HARQ process ID) is discarded.
If the feedback is a positive ACK, it flushes the HARQ buffer for that
HARQ process ID, marks the process ID as available again, and cancels
the timer associated with the packet (transport block) in the HARQ buffer.
Note that for groupcast, the first positive ACK is sufficient to cancel
subsequent retransmissions, even if later negative ACKs are received;
the code does not wait for the reception of (N-1) positive ACKs for
a group of size N.

In the event of lack of feedback (for instance, a receiver that never
successfully receives the PSSCH or the SCI-2A), the timer for the process
ID would eventually expire, leading to the flush of the HARQ buffer and
resetting of the process ID, avoiding that a process ID is forever tied
up with an unresponsive peer.

At this point, if HARQ has indicated positive ACK, no subsequent
retransmissions should be made of this transport block, but note that
retransmissions may have already been scheduled in advance.  The |ns3| code
suppresses these retransmissions as follows.  The grant will still be
scheduled for a future slot, but the HARQ buffer will be empty.  The
code in ``NrUeMac::DoNrSlotIndication`` will check if the grant has new
data indicator (NDI) set.  In the case of a scheduled retransmission, it
will not, so the code will next attempt to fetch the packet burst from
the HARQ buffer.  If this buffer has been cleared, the pointer returned
will be null, causing the code to skip the sending of the PSSCH PDU
and logging that the slot was a wasted retransmission grant.

.. _NrSidelinkHarqScopeAndLimitations:

Scope and Limitations
=====================

* No support for 'negative-only' groupcast HARQ mode (a.k.a. Option 1).
* PSSCH channel error model is not sidelink-based, but reuses uplink error model
* PSFCH channel is ideal (no error model)
* Receiver-side collisions on PSFCH half-duplex operation is not supported
* Need to set the RBs correctly in varTtiInfo
* Need to add NrSpectrumPhy::TxFbTrace

.. _NrSidelinkHarqReferences:

References
==========

The sidelink HARQ operation is mostly specified in Section
5.22.1.3 of the MAC layer specification 3GPP TS 38.321 [TS38321]_.
Aspects of the |ns3| HARQ implementation are described in [Ali21]_.  Aspects
of the |ns3| NR error model and strategy for combining multiple transmissions
at the physical layer are described in [Lag20]_.

.. [Ali21] Z. Ali, S. Lagén, L. Giupponi and R. Rouil, "3GPP NR V2X Mode 2: Overview, Models and System-Level Evaluation," in IEEE Access, vol. 9, pp. 89554-89579, 2021

.. [Lag20] S. Lagen, K. Wanuga, H. Elkotby, S. Goyal, N. Patriciello and L. Giupponi, "New Radio Physical Layer Abstraction for System-Level Simulations of 5G Networks," ICC 2020 - 2020 IEEE International Conference on Communications (ICC), 2020, pp. 1-7.

.. [TS38321] 3GPP TS 38.321, TSG RAN; NR; Medium Access Control (MAC) Protocol Specification, Release 17, v17.0.0, Mar. 2022.

.. _NrSidelinkHarqUsage:

Usage
*****


API
===

Some HARQ configuration is set as part of resource pool configuration.
Specifically, the following methods on class ``NrSlCommResourcePoolFactory``:

======================     =======  ========================================
Method                     Default  Description
======================     =======  ========================================
SetSlPsfchPeriod()         0        PSFCH period (slots); zero disables it
SetSlMinTimeGapPsfch()     n/a      Number of slots to prepare PSFCH
======================     =======  ========================================

The following attributes are used:

=============================  =======  ========================================
Attribute                      Default  Description
=============================  =======  ========================================
NrUeMac::NumSidelinkProcess    4        Number of SL HARQ processes
NrUeMac::MinTimeGapProcessing  2        Number of slots to process HARQ feedback
=============================  =======  ========================================

Note that the attribute ``NrUeMac::NumHarqProcess`` pertains to UL/DL HARQ only.

The cast type (unicast, groupcast, broadcast) and packet delay budget are
configured as part of ``LteSlTft`` configuration.  Whether HARQ is enabled
or disabled for the logical channel is also set as part of the TFT.

Whether chase combining (CC) or incremental redundancy (IR) is used to
manage multiple transmissions is governed by the error model, set as 
part of the ``NrSlHelper`` configuration.  Sample code follows:

.. code-block:: cpp

  /*  Error model type: ns3::NrEesmCcT1, ns3::NrEesmCcT2, ns3::NrEesmIrT1,
   *  ns3::NrEesmIrT2
   */
  std::string errorModel = "ns3::NrEesmIrT1";
  nrSlHelper->SetSlErrorModel (errorModel);


Traces
======

The following trace sources are available:

==============================  ===============================================
Trace source                    Description
==============================  ===============================================
NrSlUeMacHarq::RxHarqFeedback   Trace the received HARQ feedback
NrSlUeMacHarq::Allocate         Trace the allocation of HARQ process IDs
NrSlUeMacHarq::Deallocate       Trace the deallocation of HARQ process IDs
NrSpectrumPhy::TxFeedbackTrace  Trace the transmission of PSFCH feedback
==============================  ===============================================

Examples
========

.. _sidelinkHarqExample:

sidelink-harq-example.cc
########################

This is a basic small-scale example allowing users to explore the effect
of changing HARQ modes and configuration on packet delivery ratio and
latency.

By default, a simulation scenario similar to the cttc-nr-v2x-demo-simple.cc
program is created with two UEs configured to use HARQ to support a
unicast session.  One band with a single component carrier, and one
bandwidth part, is used.  Since there
is only one transmitter in the scenario, sensing is by default not enabled.

By default, a groupcast option 2 (positive-negative acknowledgment) HARQ
configuration is used (option 1 is not supported).  For groupcast, three UEs
are used, with one sender and two receivers.  Optionally, a unicast HARQ
configuration can be configured, in which two UEs are used with one sender
and one receiver.  Finally, a broadcast can be used, with one sending UE
and two receiving UEs, and blind retransmissions.  In all three cases, the
use of retransmissions can optionally be disabled.

The transmission process is a constant bit rate application sending
at 16Kbps.  The application starts at time 2 seconds into the simulation.
The bearer is activated at time 1.9 seconds.  The simulation then runs
for sufficient time to send and receive the configured number of
application data packets (default 100).

Delay samples are calculated by the NrUeMac reading the timestamp
from the RlcTag attached to each transmitted packet.  The delay
samples are stored in a file 'sidelink-harq-example-delay.dat' with
a format of "Time delay" (units of seconds, resolution of microseconds).
Additionally, the minimum and maximum delay are tracked and output
to the terminal.

The example configures trace sinks to listen to HARQ-related traces
(PSFCH transmissions, HARQ feedback received, and HARQ IDs allocated
and deallocated) and prints those events to stdout.

The example will also output the average Packet Inter-Reception (PIR)
type 2 computed as defined in 3GPP TR 37.885.  The PIR is measured by
tracing the reception process in the receiving (PacketSink) applications.

**To be completed-- add plots**

.. _NrSidelinkHarqTests:

Tests
*****

The following test cases are planned

- Unicast, groupcast, and broadcast (blind)
- Different numerologies
- Different TDD and SL patterns
- Different PSFCH periods
- loss of SCI-1 (forcing sender timeout)
- loss of SCI-2A and PSSCH
- loss of all HARQ feedback
- scale number of groupcast users up
- multiple senders
- overload behavior when insufficient HARQ process IDs


