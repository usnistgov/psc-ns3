.. include:: replace.txt
----------------------------
LTE Sidelink PHY Error Model
----------------------------

The Sidelink physical error model consists of the data, control, and the
discovery error model. All of them are active by default. It is possible to
deactivate them with the ns3 attribute system, in detail::

  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (false));

Besides the error models, one more attribute, i.e., "DropRbOnCollisionEnabled"
is also introduced only for the Sidelink transmissions. This is implemented by
keeping in mind the scenarios in which the resources are autonomously scheduled
by a UE, which increases the probability of two UEs choosing the same RBs to
transmit. Therefore, causing a collision between the TBs. By using this
attribute, a user can choose to drop such collided TBs. It can be configured as
follows::

 Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

