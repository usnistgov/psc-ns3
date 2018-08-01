cd .. include:: replace.txt


++++++++++++++++++++++++++++++++++++++
Design documentation
++++++++++++++++++++++++++++++++++++++


Overview
++++++++

The Buildings module provides:

 #. a new class (``Building``) that models the presence of a building in a simulation scenario;  
 #. a new class (``MobilityBuildingInfo``) that allows to specify the location, size and characteristics of buildings present in the simulated area, and allows the placement of nodes inside those buildings;
 #. a container class with the definition of the most useful pathloss models and the correspondent variables called ``BuildingsPropagationLossModel``.
 #. a new propagation model (``HybridBuildingsPropagationLossModel``) working with the mobility model just introduced, that allows to model the phenomenon of indoor/outdoor propagation in the presence of buildings.
 #. a simplified model working only with Okumura Hata (``OhBuildingsPropagationLossModel``) considering the phenomenon of indoor/outdoor propagation in the presence of buildings.

The models have been designed with LTE in mind, though their implementation is in fact independent from any LTE-specific code, and can be used with other ns-3 wireless technologies as well (e.g., wifi, wimax). 

The ``HybridBuildingsPropagationLossModel`` pathloss model included is obtained through a combination of several well known pathloss models in order to mimic different environmental scenarios such as urban, suburban and open areas. Moreover, the model considers both outdoor and indoor indoor and outdoor communication has to be included since HeNB might be installed either within building and either outside. In case of indoor communication, the model has to consider also the type of building in outdoor <-> indoor communication according to some general criteria such as the wall penetration losses of the common materials; moreover it includes some general configuration for the internal walls in indoor communications. 

The ``OhBuildingsPropagationLossModel`` pathloss model has been created for simplifying the previous one removing the thresholds for switching from one model to other. For doing this it has been used only one propagation model from the one available (i.e., the Okumura Hata). The presence of building is still considered in the model; therefore all the considerations of above regarding the building type are still valid. The same consideration can be done for what concern the environmental scenario and frequency since both of them are parameters of the model considered.


The Building class
++++++++++++++++++

The model includes a specific class called ``Building`` which contains a ns3 ``Box`` class for defining the dimension of the building. In order to implements the characteristics of the pathloss models included, the ``Building`` class supports the following attributes:

  * building type:

    * Residential (default value)
    * Office
    * Commercial

  * external walls type

    * Wood
    * ConcreteWithWindows (default value)
    * ConcreteWithoutWindows
    * StoneBlocks

  * number of floors (default value 1, which means only ground-floor)
  * number of rooms in x-axis (default value 1)
  * number of rooms in y-axis (default value 1)

The Building class is based on the following assumptions:

 * a buildings is represented as a rectangular parallelepiped (i.e., a box)
 * the walls are parallel to the x, y, and z axis
 * a building is divided into a grid of rooms, identified by the following parameters:
 
   * number of floors 
   * number of rooms along the x-axis
   * number of rooms along the y-axis

 * the z axis is the vertical axis, i.e., floor numbers increase for increasing z axis values
 * the x and y room indices start from 1 and increase along the x and y axis respectively
 * all rooms in a building have equal size



The MobilityBuildingInfo class
++++++++++++++++++++++++++++++

The ``MobilityBuildingInfo`` class, which inherits from the ns3 class ``Object``, is in charge of maintaining information about the position of a node with respect to building. The information managed by ``MobilityBuildingInfo`` is:

  * whether the node is indoor or outdoor
  * if indoor:

    * in which building the node is
    * in which room the node is positioned (x, y and floor room indices)  

The class ``MobilityBuildingInfo`` is used by ``BuildingsPropagationLossModel`` class, which inherits from the ns3 class ``PropagationLossModel`` and manages the pathloss computation of the single components and their composition according to the nodes' positions. Moreover, it implements also the shadowing, that is the loss due to obstacles in the main path (i.e., vegetation, buildings, etc.).

It is to be noted that, ``MobilityBuildingInfo`` can be used by any other propagation model. However, based on the information at the time of this writing, only the ones defined in the building module are designed for considering the constraints introduced by the buildings.




ItuR1238PropagationLossModel
++++++++++++++++++++++++++++

This class implements a building-dependent indoor propagation loss model based on the ITU P.1238 model, which includes losses due to type of building (i.e., residential, office and commercial).
The analytical expression is given in the following.

.. math::

  L_\mathrm{total} = 20\log f + N\log d + L_f(n)- 28 [dB]

where:

  :math:`N = \left\{ \begin{array}{lll} 28 & residential \\ 30 & office \\ 22 & commercial\end{array} \right.` : power loss coefficient [dB]

  :math:`L_f = \left\{ \begin{array}{lll} 4n & residential \\ 15+4(n-1) & office \\ 6+3(n-1) & commercial\end{array} \right.`

  :math:`n` : number of floors between base station and mobile (:math:`n\ge 1`)

  :math:`f` : frequency [MHz]

  :math:`d` : distance (where :math:`d > 1`) [m]




BuildingsPropagationLossModel
+++++++++++++++++++++++++++++

The BuildingsPropagationLossModel provides an additional set of building-dependent pathloss model elements that are used to implement different pathloss logics. These pathloss model elements are described in the following subsections.



External Wall Loss (EWL)
-------------------------

This component models the penetration loss through walls for indoor to outdoor communications and vice-versa. The values are taken from the [cost231]_ model.

  * Wood ~ 4 dB
  * Concrete with windows (not metallized) ~ 7 dB
  * Concrete without windows ~ 15 dB (spans between 10 and 20 in COST231)
  * Stone blocks ~ 12 dB


Internal Walls Loss (IWL)
-------------------------

This component models the penetration loss occurring in indoor-to-indoor communications within the same building. The total loss is calculated assuming that each single internal wall has a constant penetration loss :math:`L_{siw}`, and approximating the number of walls that are penetrated with the manhattan distance (in number of rooms) between the transmitter and the receiver. In detail, let :math:`x_1`, :math:`y_1`, :math:`x_2`, :math:`y_2` denote the room number along the :math:`x` and :math:`y` axis respectively for user 1 and 2; the total loss :math:`L_{IWL}` is calculated as 

.. math::

  L_{IWL} = L_{siw} (|x_1 -x_2| + |y_1 - y_2|)

  



Height Gain Model (HG)
-----------------------

This component model the gain due to the fact that the transmitting device is on a floor above the ground. In the literature [turkmani]_ this gain has been evaluated as about 2 dB per floor. This gain can be applied to all the indoor to outdoor communications and vice-versa.


Shadowing Model
---------------

The shadowing is modeled according to a log-normal distribution with variable standard deviation as function of the relative position (indoor or outdoor) of the MobilityModel instances involved. One random value is drawn for each pair of MobilityModels, and stays constant for that pair during the whole simulation. Thus, the model is appropriate for static nodes only. 

The model considers that the mean of the shadowing loss in dB is always 0. For the variance, the model considers three possible values of standard deviation, in detail:

 * outdoor (``m_shadowingSigmaOutdoor``, defaul value of 7 dB) :math:`\rightarrow X_\mathrm{O} \sim N(\mu_\mathrm{O}, \sigma_\mathrm{O}^2)`.
 * indoor (``m_shadowingSigmaIndoor``, defaul value of 10 dB) :math:`\rightarrow X_\mathrm{I} \sim N(\mu_\mathrm{I}, \sigma_\mathrm{I}^2)`.
 * external walls penetration (``m_shadowingSigmaExtWalls``, default value 5 dB) :math:`\rightarrow X_\mathrm{W} \sim N(\mu_\mathrm{W}, \sigma_\mathrm{W}^2)`

The simulator generates a shadowing value per each active link according to nodes' position the first time the link is used for transmitting. In case of transmissions from outdoor nodes to indoor ones, and vice-versa, the standard deviation (:math:`\sigma_\mathrm{IO}`) has to be calculated as the square root of the sum of the quadratic values of the standard deviatio in case of outdoor nodes and the one for the external walls penetration. This is due to the fact that that the components producing the shadowing are independent of each other; therefore, the variance of a distribution resulting from the sum of two independent normal ones is the sum of the variances. 

.. math::
  
  X \sim N(\mu,\sigma^2) \mbox{ and } Y \sim N(\nu,\tau^2)

  Z = X + Y \sim Z (\mu + \nu, \sigma^2 + \tau^2) 

  \Rightarrow \sigma_\mathrm{IO} = \sqrt{\sigma_\mathrm{O}^2 + \sigma_\mathrm{W}^2}





Pathloss logics
+++++++++++++++

In the following we describe the different pathloss logic that are implemented by inheriting from BuildingsPropagationLossModel.


HybridBuildingsPropagationLossModel
-----------------------------------

The ``HybridBuildingsPropagationLossModel`` pathloss model included is obtained through a combination of several well known pathloss models in order to mimic different outdoor and indoor scenarios, as well as indoor-to-outdoor and outdoor-to-indoor scenarios. In detail, the class ``HybridBuildingsPropagationLossModel`` integrates the following pathloss models:

 * OkumuraHataPropagationLossModel (OH) (at frequencies > 2.3 GHz substituted by Kun2600MhzPropagationLossModel)
 * ItuR1411LosPropagationLossModel and ItuR1411NlosOverRooftopPropagationLossModel (I1411)
 * ItuR1238PropagationLossModel (I1238)
 * the pathloss elements of the BuildingsPropagationLossModel (EWL, HG, IWL)

The following pseudo-code illustrates how the different pathloss model elements described above are integrated in  ``HybridBuildingsPropagationLossModel``::

  if (txNode is outdoor)
    then
      if (rxNode is outdoor)
        then
          if (distance > 1 km)
            then
              if (rxNode or txNode is below the rooftop)
                then
                  L = I1411
                else
                  L = OH 
            else
              L = I1411
        else (rxNode is indoor)
          if (distance > 1 km)
            then
              if (rxNode or txNode is below the rooftop)
                L = I1411 + EWL + HG
              else
                L = OH + EWL + HG
            else
              L = I1411 + EWL + HG
  else (txNode is indoor)
    if (rxNode is indoor)
      then
       if (same building)
          then
            L = I1238 + IWL
          else
            L = I1411 + 2*EWL 
     else (rxNode is outdoor)
      if (distance > 1 km)
        then 
          if (rxNode or txNode is below the rooftop)
                then
                  L = I1411 + EWL + HG
                else
                  L = OH + EWL + HG
        else
          L = I1411 + EWL




We note that, for the case of communication between two nodes below rooftop level with distance is greater then 1 km, we still consider the I1411 model, since OH is specifically designed for macro cells and therefore for antennas above the roof-top level.

For the ITU-R P.1411 model we consider both the LOS and NLoS versions. In particular, we considers the LoS propagation for distances that are shorted than a tunable threshold (``m_itu1411NlosThreshold``). In case on NLoS propagation, the over the roof-top model is taken in consideration for modeling both macro BS and SC. In case on NLoS several parameters scenario dependent have been included, such as average street width, orientation, etc. The values of such parameters have to be properly set according to the scenario implemented, the model does not calculate natively their values. In case any values is provided, the standard ones are used, apart for the height of the mobile and BS, which instead their integrity is tested directly in the code (i.e., they have to be greater then zero).  In the following we give the expressions of the components of the model.

We also note that the use of different propagation models (OH, I1411, I1238 with their variants) in HybridBuildingsPropagationLossModel can result in discontinuities of the pathloss with respect to distance. A proper tuning of the attributes (especially the distance threshold attributes) can avoid these discontinuities. However, since the behavior of each model depends on several other parameters (frequency, node height, etc), there is no default value of these thresholds that can avoid the discontinuities in all possible configurations. Hence, an appropriate tuning of these parameters is left to the user.


OhBuildingsPropagationLossModel
-------------------------------

The ``OhBuildingsPropagationLossModel`` class has been created as a simple means to solve the discontinuity problems of ``HybridBuildingsPropagationLossModel`` without doing scenario-specific  parameter tuning. The solution is to use only one propagation loss model (i.e., Okumura Hata), while retaining the structure of the pathloss logic for the calculation of other path loss components (such as wall penetration losses). The result is a model that is free of discontinuities (except those due to walls), but that is less realistic overall for a generic scenario with buildings and outdoor/indoor users, e.g., because Okumura Hata is not suitable neither for indoor communications nor for outdoor communications below rooftop level. 

In detail, the class ``OhBuildingsPropagationLossModel`` integrates the following pathloss models:

 * OkumuraHataPropagationLossModel (OH)
 * the pathloss elements of the BuildingsPropagationLossModel (EWL, HG, IWL)

The following pseudo-code illustrates how the different pathloss model elements described above are integrated in ``OhBuildingsPropagationLossModel``::

  if (txNode is outdoor)
    then
      if (rxNode is outdoor)
        then
          L = OH 
        else (rxNode is indoor)
          L = OH + EWL
  else (txNode is indoor)
    if (rxNode is indoor)
      then
       if (same building)
          then
            L = OH + IWL
          else
            L = OH + 2*EWL 
     else (rxNode is outdoor)
        L = OH + EWL
      
We note that OhBuildingsPropagationLossModel is a significant simplification with respect to HybridBuildingsPropagationLossModel, due to the fact that OH is used always. While this gives a less accurate model in some scenarios (especially below rooftop and indoor), it effectively avoids the issue of pathloss discontinuities that affects HybridBuildingsPropagationLossModel.

3GPP aligned propagation loss models
++++++++++++++++++++++++++++++++++++

In the following we describe all the propagation loss models, which are compliant with 3GPP standards. In particular, following propagation loss models are implemented:

* Hybrid3gppPropagationLossModel
* IndoorToIndoorPropagationLossModel
* OutdoorToIndoorPropagationLossModel
* OutdoorToOutdoorPropagationLossModel
* ScmUrbanMacroCellPropagationLossModel
* UrbanMacroCellPropagationLossModel

Hybrid3gppPropagationLossModel
-------------------------------

The ``Hybrid3gppPropagationLossModel`` pathloss model is a combination of the following pathloss models:

* IndoorToIndoorPropagationLossModel
* OutdoorToIndoorPropagationLossModel
* OutdoorToOutdoorPropagationLossModel
* UrbanMacroCellPropagationLossModel

This wrapper class is created to make it easier to evaluate the pathloss in different environments, e.g., Macro cell, D2D outdoor, indoor, hybrid (i.e. outdoor to indoor) and with buildings. The following pseudo-code illustrates how the different pathloss models are integrated in ``Hybrid3gppPropagationLossModel``::

 if (Macro Cell Communication)
    then
      L = UrbanMacroCell
 else (D2D communication)
    if (NodeA is outdoor)
      then
       if (NodeB is outdoor)
          then
            L = OutdoorToOutdoor
          else
            L = OutdoorToIndoor
    else (NodeA is indoor)
       if (NodeB is indoor)
          then
            L = IndoorToIndoor
          else
            L = OutdoorToIndoor

IndoorToIndoorPropagationLossModel
----------------------------------

The model is defined by 3GPP for D2D indoor to indoor scenarios [TR36843]_ [TR36814]_. It considers LOS and NLOS scenarios for 700 MHz frequency (Public Safety use cases) by taking into account the shadowing according to a log-normal distribution. For the case when UE is inside the same building as hotspot the standard deviation is 3 dB and 4 dB for LOS and NLOS, respectively. For the scenario when UE is in a different building the standard deviation is 10 dB.

UE is inside a different building as the indoor hotzone
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


.. math::
          L_\mathrm{NLOS}[\mathrm{dB}] = max(131.1 + 42.8\log_{10}(R) , 147.4 + 43.3\log_{10}(R))


UE is inside the same building as the indoor hotzone
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. math::
          L_\mathrm{LOS}[\mathrm{dB}] = 89.5 + 16.9\log_{10}(R)

          L_\mathrm{NLOS}[\mathrm{dB}] = 147.4 + 43.3\log_{10}(R)

where the probability of LOS is given as:

.. math::

   Prob(R) = \left\{ \begin{array}{ll} 1 & \mbox{if } R \le 0.018 Km \\  e^{\frac{-(R-0.018)}{0.027}} & \mbox{if } 0.018 Km \le R \le 0.037 Km \\ 0.5 & \mbox{if } R \ge 0.037 Km\end{array}\right.

According to the standard [TR36843]_, the pathloss for 700 MHz band is computed by applying :math:`20\log_{10}(f_\mathrm{c})` to the pathloss at 2 GHz as follows,

.. math::

   LOSS[\mathrm{dB}] = \begin{array}{ll} LOSS + 20\log_{10}(\frac{f_\mathrm{c}}{2}) & \mbox{if } 0.758 GHz \le f_\mathrm{c} \le 0.798 GHz \end{array}.

where

  :math:`f_\mathrm{c}` : frequency [GHz]

  :math:`R` : distance between the hotspot and UE [Km]

OutdoorToIndoorPropagationLossModel
-----------------------------------
This model is implemented for outdoor to indoor scenarios as per the specifications in [TR36843]_. The model supports both Line-of-Sight (LOS) and Non Line-of-Sight (NLOS) scenarios by taking in to account the shadowing according to a log-normal distribution with standard deviation of 7 dB for both the scenarios.

The pathloss equations used by this model is:

.. math::

  L_\mathrm{LOS}[\mathrm{dB}] = PL\_B1\_tot(d_{in} + d_{out}) + 20 + 0.5d_{in}

.. math::

  L_\mathrm{NLOS}[\mathrm{dB}] = PL\_B1\_tot(d_{in} + d_{out}) + 20 + 0.5d_{in} - 0.8h_{ms}

:math:`PL\_B1\_tot` is computed as follows,

.. math::

   PL\_B1\_tot(d_{in} + d_{out}) = max(PL_{freespace}(d),PL\_B1(d_{in} + d_{out}))

where :math:`PL_{freespace}` is free space path loss from Eq. 4.24 in [winner]_.

.. math::

  PL_{freespace} = 20\log_{10}(d) + 46.4 + 20\log_{10}(\frac{f_\mathrm{c}}{5})

and :math:`PL\_B1` is the path loss from Winner + B1 channel model for LOS and NLOS scenarios in hexagonal layout [winnerfinal]_:

For LOS
^^^^^^^

.. math::

  PL\_B1_\mathrm{LOS}[\mathrm{dB}] = \left\{ \begin{array}{ll} 22.7\log_{10}(d_{in} + d_{out}) + 27 + 20\log_{10}(f_\mathrm{c}) + LOS_{offset} & \mbox{if } 3 m \le d \le d_\mathrm{BP} \\ \\ 40\log_{10}(d_{in} + d_{out}) + 7.56 - 17.3\log_{10}(h^{'}_\mathrm{bs}) - & \mbox{if } d_\mathrm{BP} \le d \le 5000 m \\ 17.3\log_{10}(h^{'}_\mathrm{ms}) + 2.7\log_{10}(f_\mathrm{c}) + LOS_{offset}\end{array}\right.

where the :math:`LOS_{offset}` is 0 dB and the breakpoint distance is given by:

.. math::

  d_\mathrm{BP} \approx 4 h^{'}_\mathrm{bs} h^{'}_\mathrm{ms} (\frac{f_\mathrm{c}[Hz]}{c})

the LOS probability is computed as follows:

.. math::

  P_\mathrm{LOS} = min(\frac{\mathrm{18}}{d},1)(1 - e^{\frac{-d}{36}}) + e^{\frac{-d}{36}}

and the effective antenna height of the eNb and UE is computed as:

.. math::  h^{'}_\mathrm{bs} = h_{\mathrm{bs}} - 1

.. math::  h^{'}_\mathrm{ms} = h_{\mathrm{ms}} - 1

For NLOS
^^^^^^^^

The model supports frequency bands of 700 MHz for Public Safety and 2 GHz for general scenarios in NLOS. The pathloss equations used are the following:

for 700 MHz:

.. math::

  PL\_B1_\mathrm{NLOS}[\mathrm{dB}] =  \left\{ \begin{array}{ll} (44.9 - 6.55\log_{10}(h_\mathrm{bs}))\log_{10}(d_{in} + d_{out}) + 5.83\log_{10}(h_\mathrm{bs}) + & \mbox{if } 3 m \le d \le 2000 m \\ 16.33 + 26.16\log_{10}(f_\mathrm{c}) + NLOS_{offset}\end{array}\right.

for 2 GHz:

.. math::

  PL\_B1_\mathrm{NLOS}[\mathrm{dB}] =  \left\{ \begin{array}{ll} (44.9 - 6.55\log_{10}(h_\mathrm{bs}))\log_{10}(d_{in} + d_{out}) + 5.83\log_{10}(h_\mathrm{bs}) + & \mbox{if } 3 m \le d \le 2000 m \\ 14.78 + 34.97\log_{10}(f_\mathrm{c})  + NLOS_{offset}\end{array}\right.

where the :math:`NLOS_{offset}` is 5 dB.


The remaining parameters used in the above equations are:

  :math:`f_\mathrm{c}` : frequency [GHz]

  :math:`d` : distance between the eNB and UE [m]

  :math:`d_\mathrm{in}` : distance from the wall to the indoor terminal [m]

  :math:`d_\mathrm{out}` : distance between the outdoor terminal and the point on the wall that is nearest to the the indoor terminal [m]

  :math:`h_\mathrm{bs}` : eNB antenna height above the ground [m]

  :math:`h_\mathrm{ms}` : UE antenna height above the ground [m]

  :math:`h^{'}_\mathrm{bs}` : effective antenna height of the eNB [m]

  :math:`h^{'}_\mathrm{ms}` : effective antenna height of the UE [m]

  :math:`LOS_{offset}` : line-of-sight offset

  :math:`NLOS_{offset}` : non line-of-sight offset

  :math:`c` : speed of light in vacuum (:math:`3$x$10^8 m/s`)

OutdoorToOutdoorPropagationLossModel
------------------------------------

This propagation loss model is defined by 3GPP for Device to Device (D2D) outdoor to outdoor scenario [TR36843]_. The model supports both LOS and NLOS scenarios by taking in to account the shadowing according to a log-normal distribution with standard deviation of 7 dB for both the scenarios.

The pathloss equation used by this model is:

.. math::

  PL\_B1\_tot(d) = max(PL_{freespace}(d),PL\_B1(d))

where :math:`PL_{freespace}` is free space path loss from Eq. 4.24 in [winner]_.

.. math::

  PL_{freespace} = 20\log_{10}(d) + 46.4 + 20\log_{10}(\frac{f_\mathrm{c}}{5})

and :math:`PL\_B1` is the path loss from Winner + B1 channel model [winnerfinal]_ for hexagonal layout and is given by:


.. math::

  L_\mathrm{LOS}[\mathrm{dB}] = \left\{ \begin{array}{ll} 22.7\log_{10}(d) + 27 + 20\log_{10}(f_\mathrm{c}) + LOS_{offset} & \mbox{if } 3 m \le d \le d_\mathrm{BP} \\ 40\log_{10}(d) + 7.56 - 17.3\log_{10}(h^{'}_\mathrm{bs}) - 17.3\log_{10}(h^{'}_\mathrm{ms}) + 2.7\log_{10}(f_\mathrm{c}) + LOS_{offset} & \mbox{if } d_\mathrm{BP} \le d \le 5000 m\end{array}\right.

where the breakpoint distance is given by:

.. math::

  d_\mathrm{BP} \approx 4 h^{'}_\mathrm{bs} h^{'}_\mathrm{ms} (\frac{f_\mathrm{c}[Hz]}{c})


The implemented model supports two range of frequency bands 700 MHz and 2 GHz in NLOS scenarios. The pathloss equations are the following:

for 700 MHz:

.. math::

  L_\mathrm{NLOS}[\mathrm{dB}] =  \left\{ \begin{array}{ll} (44.9 - 6.55\log_{10}(h_\mathrm{bs}))\log_{10}(d) + 5.83\log_{10}(h_\mathrm{bs}) + & \mbox{if } 3 m \le d \le 2000 m \\ 16.33 + 26.16\log_{10}(f_\mathrm{c}) + NLOS_{offset}\end{array}\right.

for 2 GHz:

.. math::

  L_\mathrm{NLOS}[\mathrm{dB}] =  \left\{ \begin{array}{ll} (44.9 - 6.55\log_{10}(h_\mathrm{bs}))\log_{10}(d) + 5.83\log_{10}(h_\mathrm{bs}) + & \mbox{if } 3 m \le d \le 2000 m \\ 14.78 + 34.97\log_{10}(f_\mathrm{c})  + NLOS_{offset}\end{array}\right.

and the probability of LOS is:

.. math::

  P_\mathrm{LOS} = min(\frac{\mathrm{18}}{d},1)(1 - e^{\frac{-d}{36}}) + e^{\frac{-d}{36}}


According to the standard while calculating Winner + B1 pathloss the following values shall be used

.. math:: h_{\mathrm{bs}} = h_{\mathrm{ms}} = 1.5 m
.. math:: h^{'}_\mathrm{bs} = h^{'}_\mathrm{ms} = 0.8 m
.. math:: LOS_{offset} = 0 dB
.. math:: NLOS_{offset} = -5 dB

where

  :math:`f_\mathrm{c}` : frequency [GHz]

  :math:`d` : distance between the eNB and UE [m]

  :math:`h_\mathrm{bs}` : eNB antenna height above the ground [m]

  :math:`h_\mathrm{ms}` : UE antenna height above the ground [m]

  :math:`h^{'}_\mathrm{bs}` : effective antenna height of the eNB [m]

  :math:`h^{'}_\mathrm{ms}` : effective antenna height of the UE [m]

  :math:`LOS_{offset}` : line-of-sight offset

  :math:`NLOS_{offset}` : non line-of-sight offset

  :math:`c` : speed of light in vacuum (:math:`3$x$10^8 m/s`)

We note that, the model returns a free space path loss value if the distance between a transmitter and a receiver is less than 3 m.

ScmUrbanMacroCellPropagationLossModel
-------------------------------------

This propagation loss model is based on the specifications defined for 3GPP Spatial Channel Model (SCM) [TR25996]_ for NLOS urban macro-cell scenario. The pathloss is based on the modified COST231 Okumura Hata urban propagation model for frequencies ranging from 150 – 2000 MHz.
The model also considers shadowing according to a log-normal distribution with standard deviation of 8 dB, as defined in the standard [TR25996]_.

The pathloss expression used by this model is:

.. math::

  L[\mathrm{dB}] = (44.9 - 6.55\log_{10}{(h_\mathrm{bs})})\log_{10}(\frac{d}{1000}) + 45.5 + (35.46 - 1.1{(h_\mathrm{ms})})\log_{10}(f_\mathrm{c}) - 13.82\log_{10}(h_\mathrm{bs}) + 0.7(h_\mathrm{ms}) + C

where

  :math:`f_\mathrm{c}` : frequency [MHz]

  :math:`h_\mathrm{bs}` : eNB antenna height above the ground [m]

  :math:`h_\mathrm{ms}` : UE antenna height above the ground [m]

  :math:`d` : distance between the eNB and UE [m]

  :math:`C` : Constant factor

The value of :math:`C = 3 dB`  for urban macro-cell scenario.

UrbanMacroCellPropagationLossModel
----------------------------------

This propagation loss model is developed and documented by 3GPP in [TR36814]_. The implemented model covers an urban macro-cell scenario for the frequency range of 2 - 6 GHz with different antennas, building heights and street widths. It is designed for both LOS and NLOS scenarios by taking in to account the shadowing according to a log-normal distribution with standard deviation of 4 dB and 6 dB, for LOS and NLOS, respectively.

The pathloss expressions used by this model are:

.. math::

  L_\mathrm{LOS}[\mathrm{dB}] = \left\{ \begin{array}{ll} 22\log_{10}(d) + 28 + 20\log_{10}(f_\mathrm{c}) & \mbox{if } 10 m \le d \le d_\mathrm{BP} \\ 40\log_{10}(d) + 7.8 - 18.0\log_{10}(h^{'}_\mathrm{bs}) - 18.0\log_{10}(h^{'}_\mathrm{ms}) + 2\log_{10}(f_\mathrm{c}) & \mbox{if } d_\mathrm{BP} \le d \le 5000 m\end{array}\right.

|

.. math::

  L_\mathrm{NLOS}[\mathrm{dB}] =  \left\{ \begin{array}{ll} 161.04 - 7.1\log_{10}(W) + 7.5\log_{10}(h) - & \mbox{if } 10 m \le d \le 5000 m \\ (24.37 - 3.7(\frac{h}{h_{\mathrm{bs}}})^2)\log_{10}(h_{\mathrm{bs}}) + (43.42 - 3.1\log_{10}(h_{\mathrm{bs}}))(\log_{10}(d) - 3) + \\ 20\log_{10}(f_\mathrm{c}) - (3.2 - (\log_{10}(11.75{h_{\mathrm{ms}}}))^2 - 4.97)\end{array}\right.


where the breakpoint distance is given by:

.. math::

  d_\mathrm{BP} \approx 4 h^{'}_\mathrm{bs} h^{'}_\mathrm{ms} (\frac{f_\mathrm{c}[Hz]}{c})

The probability of LOS is given by:

.. math::

  P_\mathrm{LOS} = min(\frac{\mathrm{18}}{d},1)(1 - e^{\frac{-d}{63}}) + e^{\frac{-d}{63}}


and the effective antenna heights of the eNb and UE are computed as:

.. math::  h^{'}_\mathrm{bs} = h_{\mathrm{bs}} - 1

.. math::  h^{'}_\mathrm{ms} = h_{\mathrm{ms}} - 1

and the above parameters are

  :math:`f_\mathrm{c}` : frequency [GHz]

  :math:`d` : distance between the eNB and UE [m]

  :math:`h` : average height of the building [m]

  :math:`W` : street width [m]

  :math:`h_\mathrm{bs}` : eNB antenna height above the ground [m]

  :math:`h_\mathrm{ms}` : UE antenna height above the ground [m]

  :math:`h^{'}_\mathrm{bs}` : effective antenna height of the eNB [m]

  :math:`h^{'}_\mathrm{ms}` : effective antenna height of the UE [m]

  :math:`c` : speed of light in vacuum (:math:`3$x$10^8 m/s`)

The model returns 0 dB loss if the distance between a transmitter and a receiver is less than 10 m. Therefore, a user should carefully deploy the UEs, such that, the distance between an eNB and a UE is 10 m or above.
