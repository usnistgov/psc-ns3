ParabolicAntennaModel
---------------------

The unit test suite ``parabolic-antenna-model`` checks that the
``ParabolicAntennaModel`` class works properly. Several test cases are
provided that check for the antenna gain value calculated at different
directions and for different values of the orientation, the maximum attenuation
and the beamwidth. The reference gain is calculated by hand. Each
test case passes if the reference gain in dB is equal to the value returned
by ``ParabolicAntennaModel`` within a tolerance of 0.001, which accounts
for the approximation done for the calculation of the reference
values.


Parabolic3dAntennaModel
-----------------------

The unit test suite ``parabolic-3d-antenna-model`` is based on the 
``ParabolicAntennaModel`` tests.  A sequence of test cases at different
directions is defined:

1.  test horizontal plane with a 60 deg beamwidth; gain is -20dB at +-77.460 degrees from boresight
2.  test positive orientation with a 60 deg beamwidth; gain is -10dB at +-54.772 degrees from boresight
3.  test negative orientation and different beamwidths with a 80 deg beamwidth; gain is -20dB at +- 73.030 degrees from boresight
4.  test vertical plane
5.  test tilt
 
The reference gain is calculated by hand. Each test case passes if the 
reference gain in dB is equal to the value returned
by ``Parabolic3dAntennaModel`` within a tolerance of 0.001, which accounts
for the approximation done for the calculation of the reference values.
