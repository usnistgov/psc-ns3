Parabolic3dAntennaModel
+++++++++++++++++++++++

Another 3GPP-defined antenna model is the ``Parabolic3dAntennaModel``,
drawn from 3GPP TR 36.814 [TR36814]_.  The model, for 3-sector cell sites with 
fixed antenna patterns, is defined in Table A.2.1.1-2, 3GPP Case 1 and 3 
(Macro-cell).  Both a horizontal and vertical antenna pattern is defined,
and an equation for combining the two methods is provided.  So in contrast
to ``ParabolicAntennaModel``, different horizontal and vertical configuration
parameters are required.  The attributes ``HorizontalBeamwidth``, 
``Orientation``, ``MaxHorizontalAttenuation``, ``VerticalBeamwidth``, and 
``MaxVerticalAttenuation`` are configured with the suggested default values.
In addition, attributes for mechanical and electrical tilt are defined; these 
help to adjust the azimuth angle with respect to the reference system of 
the antenna. 

.. [TR36814]  3GPP TR 36.814, "Further Advancements for E-UTRA Physical Layer Aspects (Release 9)"
