<img align="right" src="ArduinoVLCB.png"  width="150" height="75">

# VCAN2040
This library provides a software CAN interface on a Raspberry Pi Pico when used in conjunction with the VLCB_Arduino library
suite.  The main VLCB-Arduino library can be found at [VLCB](https://github.com/SvenRosvall/VLCB-Arduino) 

See [Design documents](https://github.com/SvenRosvall/VLCB-Arduino/blob/main/docs/Design.md) for how this library is structured.

## Examples
There are two versions of the example that, from the users perspective, are functionally idential.
The first uses a single core in the Pico and is identified as VLCB_4in4out_Pico_s, where 's' stands
for single core.  The other core will be dormant in a low power state.
The second version makes use of both cores in the processer and is identifed as VLCB_4in4out_Pico_d,
where 'd' stands for dual core.  This is organised such that the VLCB library runs in core 0 and the
application runs in core 1.

The docs folder in this repository provides notes on how to use this library with the 4in4out examples.
It also holds a circuit schematic suitable for the code as written.

## Dependencies
Note that this library depends on a number of other libraries which must be downloaded and included in the sketch:

* [VLCB](https://github.com/SvenRosvall/VLCB-Arduino) - The core VLCB library
* [ACAN2040](https://github.com/obdevel/ACAN2040) - CAN bus transport using the MCP2515 controller
* [Streaming](https://github.com/janelia-arduino/Streaming) - C++ style output
  
## Hardware

Currently supports the Raspberry Pi Pico using Earle Philhowers Arduino IDE board support
[Pico Arduino IDE](https://github.com/earlephilhower/arduino-pico) Full instructions on how to do this in the associated
[documentation](https://arduino-pico.readthedocs.io/en/latest/)

