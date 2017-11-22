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



0- depedencies
Cereal
opencv_contrib
opencv 3
raspicam
gstreamer 1.0


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


# Communication between Raspberry Pies

1- introduction

Computer vision is power expensive, we don't want the Raspberry Pies to burn 100% CPU for 15 minutes whereas a robot battle lasts 90 seconds. Moreover, if Raspberry Pies know when to properly shutdown, it reduces the risk to corrupt the SD cards in case we shut the power supply while the Rasp are running.
This leads us to find a way to make the Raspberry Pies know when the battle starts, and when it's over.

Another point to raise is which technology to use? TCP vs UDP vs other?
TCP can hugely increase the latency, especially if packet loss is high. We expect information to be sent to the robots at least 10 times per second. If sometimes information is lost, this should not be an issue. That is why UDP seems more appropriate than TCP.
CoAP is a protocol optimised for the IoT, and uses UDP. It could be a good candidate for the communication between the Rasps.

As packet loss could be high, the robots should be able to receive information without having to ask for it. It reduces the number of messages that must be sent. Hence, BigBrother has to know the IP addresses of the robots.
However, for debug purpose, BigBrother could send information to whoever asks for it (or an alterive would be that BigBrother can broadcast information, allowing debug clients to receive information).

A robot may want user-friendly information (x,y position for each robot), or raw information (view angle from each camera) in order to be able to do more complex operations.

Finally, when searching unexisting robots in a picture can be very expensive. BigBrother would be more efficient if it knows exactly which robots are on the table.

2- summarize

- Still need to investigate communication technologies
- Robots must be able to send messages to BigBrother (battle starts, battle is over, which robots to search for, etc.), and make sure such messages have been received
- BigBrother must send information to robots
- Sent information should be configurable (x,y or view angle)
- BigBrother should listen for debug clients who request for information

3- diagram (plantUML)

                                          ┌──────────────────────────── BigBrother ────────────────────────────┐
     ┌──────┐                               ┌──────┐                              ┌───────┐          ┌───────┐     
     │Robots│                               │Server│                              │Camera1│          │Camera2│     
     └──┬───┘                               └──┬───┘                              └───┬───┘          └───┬───┘     
        │                                      │                                      │                  │         
        │                                  ╔═══╧══════════════════════╗               │                  │         
════════╪══════════════════════════════════╣ Start up (loop until ok) ╠═══════════════╪══════════════════╪═════════
        │                                  ╚═══╤══════════════════════╝               │                  │         
        │                                      │                                      │                  │         
        │ battle starts (marker list to detect)│                                      │                  │         
        │ ─────────────────────────────────────>                                      │                  │         
        │                                      │                                      │                  │         
        │                                      │ battle starts (marker list to detect)│                  │         
        │                                      │ ─────────────────────────────────────>                  │         
        │                                      │                                      │                  │         
        │                                      │                  ok                  │                  │         
        │                                      │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─                  │         
        │                                      │                                      │                  │         
        │                                      │          battle starts (marker list to detect)          │         
        │                                      │ ────────────────────────────────────────────────────────>         
        │                                      │                                      │                  │         
        │                                      │                            ok        │                  │         
        │                                      │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─          
        │                                      │                                      │                  │         
        │                  ok                  │                                      │                  │         
        │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─                                      │                  │         
        │                                      │                                      │                  │         
        │                                      │                                      │                  │         
        │                                      │     ╔══════╗                         │                  │         
════════╪══════════════════════════════════════╪═════╣ Loop ╠═════════════════════════╪══════════════════╪═════════
        │                                      │     ╚══════╝                         │                  │         
        │                                      │                                      │                  │         
        │                                      │   marker(s) detected (pixel-level)   │                  │         
        │                                      │ <─────────────────────────────────────                  │         
        │                                      │                                      │                  │         
        │                                      │             marker(s) detected (pixel-level)            │         
        │                                      │ <────────────────────────────────────────────────────────         
        │                                      │                                      │                  │         
        │                                      │────┐                                 │                  │         
        │                                      │    │ triangulation (every x ms)      │                  │         
        │                                      │<───┘                                 │                  │         
        │                                      │                                      │                  │         
        │     robot(s) detected (x,y-level)    │                                      │                  │         
        │ <─────────────────────────────────────                                      │                  │         
        │                                      │                                      │                  │         
        │                                      │                                      │                  │         
        │                                  ╔═══╧══════════════════════╗               │                  │         
════════╪══════════════════════════════════╣ Clean up (loop until ok) ╠═══════════════╪══════════════════╪═════════
        │                                  ╚═══╤══════════════════════╝               │                  │         
        │                                      │                                      │                  │         
        │              battle ends             │                                      │                  │         
        │ ─────────────────────────────────────>                                      │                  │         
        │                                      │                                      │                  │         
        │                                      │              battle ends             │                  │         
        │                                      │ ─────────────────────────────────────>                  │         
        │                                      │                                      │                  │         
        │                                      │                  ok                  │                  │         
        │                                      │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─                  │         
        │                                      │                                      │                  │         
        │                                      │                       battle ends    │                  │         
        │                                      │ ────────────────────────────────────────────────────────>         
        │                                      │                                      │                  │         
        │                                      │                            ok        │                  │         
        │                                      │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─          
        │                                      │                                      │                  │         
        │                  ok                  │                                      │                  │         
        │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─                                      │                  │         
     ┌──┴───┐                               ┌──┴───┐                              ┌───┴───┐          ┌───┴───┐     
     │Robots│                               │Server│                              │Camera1│          │Camera2│     
     └──────┘                               └──────┘                              └───────┘          └───────┘     


(plantUML code (https://www.planttext.com/):
participant Robots
box "BigBrother"
    participant Server
    participant Camera1
    participant Camera2
== Start up (loop until ok) ==
Robots -> Server : battle starts (marker list to detect)
Server -> Camera1 : battle starts (marker list to detect)
Camera1 --> Server : ok
Server -> Camera2 : battle starts (marker list to detect)
Camera2 --> Server : ok
Server --> Robots : ok
== Loop ==
Camera1 -> Server : marker(s) detected (pixel-level)
Camera2 -> Server : marker(s) detected (pixel-level)
Server -> Server : triangulation (every x ms)
Server -> Robots : robot(s) detected (x,y-level)
== Clean up (loop until ok) ==
Robots -> Server : battle ends
Server -> Camera1 : battle ends
Camera1 --> Server : ok
Server -> Camera2 : battle ends
Camera2 --> Server : ok
Server --> Robots : ok
