.. include:: replace.txt

+++++++++++++++++++++++++
UAV Mobility Energy Model
+++++++++++++++++++++++++

The class ``ns3::psc::UavMobilityEnergyModel`` implements an
Energy Model that bases the current on how a Node is moving.

This model is roughly based on the energy model released under the
GNU GPL v2 found here:
`ns3-urbanuavmobility <https://github.com/farooqahmedrana/ns3-urbanuavmobility>`_

The model is intended to provide simple energy costs for flying a
(single/multi)rotor unmanned aerial vehicle (UAV). The default costs are not
intended to perfectly model reality, but rather to consider flight time in
cases where otherwise it may have been neglected.

-----------------
Model Description
-----------------

The implementation is provided in the following files:

* ``src/psc/model/uav-mobility-energy-model.{h,cc}`` The model
* ``src/psc/model/uav-mobility-energy-model-helper.{h,cc}`` A helper

A low-level example may be found in:

``src/psc/examples/uav-mobility-energy-example.cc``.

An example using the helper may be found in:

``src/psc/examples/uav-mobility-energy-helper-example.cc``.

The model sets the current based on five basic modes for movement:

* ``STOP``:
    Not Moving
* ``HOVER``:
    The Node has a velocity of 0 m/s, but the Node is off of the ground (z > 0)
* ``ASCEND/DESCEND``:
    The Node is moving only on the z-axis
* ``MOVE``:
    The Node is moving on the x-axis or y-axis.
    Any z movement is added to the calculated current.

The current set when the Node is stopped (not above the ground [z = 0])
is zero amps, and is not configurable.

The current set when the Node is hovering is fixed and may be set
through the ``HoverCurrent`` attribute.

The model connects to the ``CourseChange`` trace in the MobilityModel
aggregated onto the Node passed to ``ConnectMobility ()`` or ``Init ()``.
When the attached MobilityModel notifies this model of a course change
this model queries the current velocity and position of the Node.
The model will use the velocity and position to determine the
state this model should be in (e.g. Ascending, Hovering, Stopped).
Each state will incur a different current draw from the attached EnergySource.
This model then notifies the attached EnergySource of this new current
via ``UpdateEnergySource ()``.

When the attached EnergySource depletes, this model will be notified and
the ``EnergyDepleted`` trace will be called. However, there is no default
behavior beyond this. So, if you want your Node to do something when the
attached EnergySource depletes (i.e. Crash to the ground) then your program
must connect to the ``EnergyDepleted`` trace and implement the desired
behavior. See the example for how to go about this.

The current for ascending, descending, and moving is calculated based on
the speed at which the Node is moving (except for descending which is based
on the inverted speed [1/speed]) multiplied by a conversion factor.
Each mode has its own conversion factor and is configurable through the
``AscendEnergyConversionFactor``, ``DescendEnergyConversionFactor``, and
``MoveEnergyConversionFactor`` attributes respectively.

Attributes
++++++++++

The model exposes the following attributes:

* ``AscendEnergyConversionFactor``:
    The conversion factor applied to the speed
    of the Node when it is ascending. In :math:`\frac{A}{m/s}`.
* ``DescendEnergyConversionFactor``:
    The conversion factor applied to the
    inverted speed of the Node when it is descending.
    In :math:`\frac{A}{{(m/s)}^{-1}}`.
* ``MoveEnergyConversionFactor``:
    The conversion factor applied to the speed
    of the Node when it is moving. In :math:`\frac{A}{m/s}`.
* ``HoverCurrent``:
    The fixed amperage set when the Node is hovering. In amperes.
* ``EnergySource``:
    The energy source this model draws from.

Trace sources
+++++++++++++

The model exposes the following trace sources:

* ``EnergyDepleted``:
    Called when the attached Energy Source has indicated
    that it is depleted.
* ``EnergyRecharged``:
    Called when the attached Energy Source has indicated
    that it has been recharged.
* ``TotalEnergyConsumption``:
    The total energy consumed by the model. In Joules.
* ``State``:
    The current mobility state detected by the model
    along with the velocity. The possible states may be found in
    ``ns3::psc::UavMobilityEnergyModel::State``
* ``Current``:
    The current draw from the attached energy source,
    along with the velocity.

-----
Tests
-----

The following tests have been written for this model
and may be found in ``src/psc/test/uav-mobility-energy-model-test.cc``:

* ``UavMobilityEnergyModelTestCaseEnergyConsumption``:
    Tests if the model correctly sets the current on the EnergySource
    as well as if the correct amount of energy is deducted from
    the EnergySource.
* ``UavMobilityEnergyModelTestCaseZeroEnergyCost``:
    The model supports making any of the possible states consume no
    energy. Configure each state as such and make sure no energy is
    consumed.
* ``UavMobilityEnergyModelTestCaseMoveEnergy``:
    Move combines the cost of the 2D movement (x, y) and the ascend/descend
    cost (z). Tests that when a Node has both 2D movement and is ascending
    that the ascend/descend cost is accounted for and is correct, along
    with the 2D movement only.
* ``UavMobilityEnergyModelTestCaseLowEnergyTrace``:
    Tests if the ``EnergyDepleted`` trace is actually called.
* ``UavMobilityEnergyModelTestCaseLiIonEnergySource``:
    Make sure the model actually draws energy from another EnergySource
    that is not the BasicEnergySource.
* ``UavMobilityEnergyModelTestCaseMobilityModel``:
    Tests that the model correctly integrates with a MobilityModel.
    Tests that the MOVE state is correctly determined from the attached
    MobilityModel
* ``UavMobilityEnergyModelTestCaseChangeCostsRuntime``:
    The model supports changing the cost of any of the states (except for STOP)
    while the simulation is running. Tests if this feature actually works.
* ``UavMobilityEnergyModelTestCaseTotalEnergyConsumption``:
    The model tracks the total Joules consumed over the lifetime of the model.
    This total is available through both the ``GetTotalEnergyConsumption ()``
    and through the ``TotalEnergyConsumption`` trace source.
    Validate this total is correct from both sources.
* ``UavMobilityEnergyModelTestCaseInitialVelocity``:
    Tests if a velocity set before the simulation starts works correctly.
* ``UavMobilityEnergyModelTestCaseState``:
    Tests if the model correctly sets the state
    and that the ``State`` trace has the correct state.
* ``UavMobilityEnergyModelTestCaseTraceCurrent``;
    Tests if the model correctly sets the current
    and that the ``Current`` trace has the correct current.
* ``UavMobilityEnergyModelTestCaseAttributeEnergySource``:
    Tests if setting the EnergySource through the ``EnergySource`` attribute
    works properly.
* ``UavMobilityEnergyModelTestCaseFixedStateChangeSpeed``:
    Assures that if the speed/velocity at which the Node is moving is changed,
    but the state does not (i.e. moving, then moving faster in the
    same direction) that the current is updated (and the ``Current`` trace
    notified)  and the state remains the same (and the ``State`` trace is
    not notified)
* ``UavMobilityEnergyModelTestCaseVelocityTraces``:
    Several of the traces have the velocity from the MobilityModel included.
    Validate that the included velocities are correct.

-----
Usage
-----

The model may be installed on any Node with a Mobility Model installed.
The helper may also be used for a simpler installation. The helper will
create any of the necessary components on the Node and install and
init the model.

Each Node requires exactly one Energy Source passed to ``SetEnergySource()``
and an aggregated MobilityModel when it is passed to ``ConnectMobility ()``
when using the low-level API. Alternatively, both the EnergySource and
Node may be passed to ``Init ()`` for convenience.
If the helper is used, it allows for a passed Energy Source,
or an Energy Source aggregated onto the Node.
