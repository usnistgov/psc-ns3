++++++++++++++++++++++++++++++++++++
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

and the effective antenna height of the eNB and UE is computed as:

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


and the effective antenna heights of the eNB and UE are computed as:

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
