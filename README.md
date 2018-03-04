# ola_to_rpi_multi

Allows a Raspberry Pi to output multiple DMX universes.
This programm connects to OLA as a client and registers for changes on
all configured universes. Then, it connects to the pigpiod daemon and
outputs each universe on a separate GPIO.

Based on the code of Jonathan Andrews (http://www.jonshouse.co.uk/rpidmx512.cgi)

## Getting Started

Optionally, with qmake installed, run ```qmake```. Nevertheless, I provided
a minimal Makefile so qmake is not strictly needed.

### Prerequisites

* Open Lighting Architecture (OLA) running on the Pi: https://www.openlighting.org/ola/
* PiGPIO library and pigpiod running on the Pi: http://abyz.me.uk/rpi/pigpio/index.html

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Acknowledgments

* Jonathan Andrews for the demo code sending DMX on the Raspberry Pi
* joan2937 for the PiGPIO library
* All folks from the OpenLighting team for OLA
* Flo Edelmann for the hint to use pigpiod

