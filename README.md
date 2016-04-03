# BigBrother

    j>
1+---------------------------+
 |                           |
i|                           |
v|                           |
 |                           |3
 |                           |
 |  x                        |
 |                           |
2+---------------------------+

Positions ( (i, j), (vi, vj) )  #(point, vector)
Camera 1 : ( (  0,   0), ( 1, 1) )
Camera 2 : ( (200,   0), (-1, 1) )
Camera 3 : ( (100, 300), ( 0,-1) )

For example, robot x is seen with angles
Camera 1 : 25°
Camera 2 : 0°
Camera 3 : -5°



1- convert robot detection angle (from the point of view of the camera) to global positionning

Use LUT (look up table) precomputed :
For each pixel column of a camera, compute the corresponding vector:
Example camera 1 :
    column[0] = (0, 1)
    column[1] = (0.03, 0.97)
    ...
    column[MAX/2] = (0.707, 0.707)
    ...
    column[MAX] = (1, 0)

Let T1, T2, T3 be the pixel column of the robot seen from the cameras 1, 2 and 3.
PositionRobot_camera1 = (  0 + k1 * column[T1][0],   0 + k1 * column[T1][1])
PositionRobot_camera2 = (200 + k2 * column[T2][0],   0 + k2 * column[T2][1])
PositionRobot_camera3 = (100 + k3 * column[T3][0], 300 + k3 * column[T3][1])

2- find the k1, k2 and k3 vector coefficients

Depending of the previous position of the robot, a threshold can be applied to determine if a camera found a 'false robot'.
Similarly, a threshold can be applied on the 3 positions found to discard a camera probably falsy.
Finaly, the robot is located between the 3 points of intersection of the vectors found be the cameras.

