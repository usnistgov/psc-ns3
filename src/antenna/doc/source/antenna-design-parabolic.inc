ParabolicAntennaModel
+++++++++++++++++++++

This model is based on the parabolic approximation of the main lobe radiation pattern. It is often used in the context of cellular system to model the radiation pattern of a cell sector, see for instance [R4-092042a]_ and [Calcev]_. The antenna gain in dB is determined as:

.. math::
 
  g_{dB}(\phi, \theta) = -\min \left( 12 \left(\frac{\phi  - \phi_{0}}{\phi_{3dB}} \right)^2, A_{max} \right)

where :math:`\phi_{0}` is the azimuthal orientation of the antenna
(i.e., its direction of maximum gain), :math:`\phi_{3dB}` is its 3 dB
beamwidth, and :math:`A_{max}` is the maximum attenuation in dB of the
antenna. Note that this radiation pattern is independent of the inclination angle
:math:`\theta`. 


.. [Balanis] C.A. Balanis, "Antenna Theory - Analysis and Design",  Wiley, 2nd Ed.

.. [Chunjian] Li Chunjian, "Efficient Antenna Patterns for
   Three-Sector WCDMA Systems", Master of Science Thesis, Chalmers
   University of Technology, Göteborg, Sweden, 2003

.. [Calcev] George Calcev and Matt Dillon, "Antenna Tilt Control in
   CDMA Networks", in Proc. of the 2nd Annual International Wireless
   Internet Conference (WICON), 2006 

.. [R4-092042a]  3GPP TSG RAN WG4 (Radio) Meeting #51, R4-092042, Simulation
   assumptions and parameters for FDD HeNB RF requirements.


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

