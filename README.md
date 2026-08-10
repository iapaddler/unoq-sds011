# 😀 ParticleSensor


## UNO Q interface with an SDS011 Particle sensor.
## The intent is the creation of a device that can be moved around.

### This uses the GuL_NovaFitness Arduino library: https://github.com/boeserfrosch/GuL_NovaFitness
### The Arduino MsgPack library is used for debug messages from the controller to the Linux host.
---

### A few notes:
* Use the Monitor interface to create serial messages from the controller 
  * Don't use the Serial interface directly for this
* Use the Bridge interface to pass data between the controller and Linux 
* The hardware serial connections on D0 and D1 of the UNO Q are connected and available
  * Several docs refer to the fact that these are not available because the serial port is used for communication with the Linux host
  * This is not the case on the UNO Q
  * They are in fact supported
  * Linux host communication takes place over the Monitor connection and not Serial1
* The device on the UNO Q connected to the D0 & D1 pins is Serial1
* The SDS011 defaults to 9600 baud on the serial device
* Use the 5V and GND pins from UNO Q to SDS011
  * Don't use the UNO Q 3.3V source
  * SDS011 requires 5v
* Note that Arduino defined SoftwareSerial is not supported on the UNO Q
  * Zephyr OS, used on the controller, does not support this
* There is a bug in the Monitor.print & println functions
  * Floating point values are not displayed correctly
  * The accepted work around is the use of sprintf and then Monitor.print the buffer
* When using the SDS011 polled mode a delay is a good idea
  * Delay for at least 150ms after the poll before reading
  * Exact times have not been tested
  * Currently using 200ms just to be safe
* The SDS011 active mode works
  * Be aware of the period
  * Default period is a measurement every second
  * That can be changed
  * You'd want to read right after the auto measurement is made

---
### TODO
#### Controller
* Create a debug enable so that messages can be turned on & off
* Play with the LED display
* Experiment with the LED array
* Adjust the polling rate; more like every 2-3 minutes
#### Linux
* Save measurement data in an in-memory store
* Create an HTTP server
* Display measurement data in graph form


