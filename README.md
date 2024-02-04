<img align="right" src="arduino_cbus_logo.png"  width="150" height="75">

# VCAN2040
This library provides a software CAN interface on a Raspberry Pie Pico when used in conjunction with the VLCB_Arduino library
suite.  The main VLCB_Arduino library can be found at [VLCB](https://github.com/SvenRosvall/VLCB-Arduino) 

This VLCB library code is based on a [CBUS library](https://github.com/MERG-DEV/CBUSACAN2040) created by Duncan Greenwood
and extended by members of [MERG](https://www.merg.org.uk/). 
See below under Credits.

See [Design documents](docs/Design.md) for how this library is structured.

There will be documentation for sketch authors that describe how to use this library.

## Dependencies
Note that this library depends on a number of other libraries which must also be downloaded and included in the sketch:

* [ACAN2515](https://github.com/KevinOConnor/can2040) - CAN bus transport using a software controller

## Hardware

Currently supports the Raspberry Pie Pico using Earle Philhowers Arduino IDE board support
[Pico](https://github.com/earlephilhower/arduino-pico)

## Getting help and support

At the moment this library is still in development and thus not fully supported.

If you have any questions or suggestions please contact the library maintainers
by email to martindc.merg@gmail.com or create an issue in GitHub.

## Credits

* Duncan Greenwood - Created the CBUS library for Arduinos which this VLCB library is based on.
* Sven Rosvall - Converted the CBUS library into this code base.
* Martin Da Costa - Contributor

## License

The code contained in this repository and the executable distributions are licensed under the terms of the
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](LICENSE.md).

If you have questions about licensing this library please contact [license@rosvall.ie](mailto:license@rosvall.ie)
