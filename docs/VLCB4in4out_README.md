# VLCB4IN4OUT

An Arduino program to allocate 4 switches as inputs and 4 outputs to LEDs.

Key Features:
- MERG VLCB interface.
- LED flash rate selectable from event variables.
- Switch function controllable by node variables.
- Modular construction to ease adaptation to your application.
- Runs on a Raspberry Pie Pico using a software CAN Controller.

The example sketches are fully functional modules.  Whilst they can be used "as is", the
primary objective is to be a guide for developers to create their own modules to meet their
specific requirements.

## Overview

The program is written in C++ but you do not need to understand this to use the program.
The program includes a library that manages the LED functionality.

NOTE: It can get difficult when using DEBUG to know where the message has come from. Those
emanating from the sketch are preceded with the letters sk for sketch. In the case of dual
core use, the numebr 0 or 1 appears before the > to indicate which core generated the message.
Debug can be turned on by making #define DEBUG at the beginning of the sketch equal to 1.
Similarly, debug can be turned off by making #define DEBUG equal to 0.  In teh Arduino IDE,
debug logging will be shown in the Serial Monitor window.

## Using VLCB4in4out

This example is configured for use with a Pico. There are two versions: one runs the sketch
on a single core whilst the other uses dual cores.  In the case of the dual core example,
the VLCB code runs on core 0 and the 4 in 4 out application runs on core 1. Functionally,
the two versions are identical.

Pin | Description
--- | ---
Pin 19 GP14 | VLCB Green LED
Pin 20 GP15 | VLCB Yellow LED
Pin 17 GP13 | VLCB Switch

**It is the users responsibility that the total current that the Pico is asked to supply 
stays within the capacity of the on board regulator.  Failure to do this will result in 
terminal damage to your Pico.**

Pins defined as inputs are active low.  That is to say that they are pulled up by an 
internal resistor. The input switch should connect the pin to 0 Volts.

Pins defined as outputs are active low.  They will sink current from (say) an LED. It is 
important that a suitable current limiting resistor is fitted between +3V3 and the LED 
anode.  The LED cathode should be connected to the output pin.

This additonal circuitry can be built using any breadboard technique with which the user
is comfortable.  A circuit schematic of the authors test set is here [test circuit](PDFs/Pico 4in4out Schematic.pdf)

It maybe that the Shield for Pico to CBus interface taht was renently show cased on the [MERG Forum](https://www.merg.org.uk/forum/viewtopic.php?f=316&t=18021)
could be used but there is no information is currently available to the author for assessment.

### Library Dependencies

As well as the [VLCB-Arduino](https://github.com/SvenRosvall/VLCB-Arduino) library suite
associated with this example, the following third party libraries are required:

Library | Purpose
---------------|-----------------
Streaming.h  |*C++ stream style output, v5, (http://arduiniana.org/libraries/streaming/)*
Bounce2.h    |*Debounce of switch inputs*
ACAN2040.h   |*library to support the software CAN controller*

### Application Configuration

These examples use the following pins for LEDs (output) and pushbutton switches (Inputs).

Pin | Description
--- | ---
Pin 24 GP18 | Switch 1
Pin 25 GP19 | Switch 2
Pin 26 GP20 | Switch 3
Pin 27 GP21 | Switch 4
Pin 29 GP22 | LED 1
Pin 31 GP26 | LED 2
Pin 32 GP27 | LED 3
Pin 34 GP28 | LED 4


### VLCB Op Codes

The following Op codes are supported:

OP_CODE | HEX | Function
----------|---------|---------
 OPC_ACON | 0x90 | On event
 OPC_ACOF | 0x91 | Off event
 OPC_ASON | 0x98 | Short event on
 OPC_ASOF | 0x99 | Short event off

## Events

### Event Variables

Event Variable 1 (EV1) identifies a consumed only event if its value is zero.
A non-zero value will identify the source (in this case switch) for a produced
event.

### Consumed Events

Event Variables control the action to take when a consumed event is received.
The number of Event Variables (EV) is equal to the number of LEDs plus one.

Event Variable 2 (EV2) controls the first LED pin in the ```LED``` array. 
EV3 controls the second LED pin, etc.

The LEDs are controlled by the LEDControl class.  This allows for any LED to be
switched on, switched off or flashed at a rate determined in the call:
LEDControl::flash(unsigned int period)

The following EV values are defined to control the LEDs in this example:

 EV Value | Function
--------|-----------
 0 | LED off
 1 | LED on
 2 | LED flash at 500mS
 3 | LED flash at 250mS

### Produced Events

The Events Table in an Uninitialised module is empty. When the module changes to
Normal Mode from Uninitialised, the Events Table is populated with a default
producer event for each of the four switches (or, in general, each producer item).
These default events can be removed by using OPC_EVULN whilst the module is in
learn mode.

A module can also be taught a short event or, indeed, a spoof event. This is
easily done using any of the normal FCU teach techniques.  For example, a short
event can be dragged from the Software Node and dropped onto the VLCB4in4out in
the Node Window.  When the event variable dialgue opens, put the index number of
the switch to be associated with the event in EV1 and press OK.  The selected
switch will now generate that short event (but see FCU Anomolies below).

If a switch default has been unlearnt and that switch not been assigned to an
event by the FCU, operation of the switch will result in a new default event
being generated as long as its NV Value is set for a function other than
"Do Nothing" (See Node Variables below).

### Consume Own Events

If the Produced Events Service and the Consume Events Service are both applied,
the Consume Own Events Service can also be enabled.  This service provides a 
buffer that will pass the produced event back to the Consumed Event Service.
A Consume Own Event still only has one entry in the Event Table.  If the Event
Variables 2 onwards are left as 0 or any undefined value, then the
Consume Events Service will do nothing.  Note that an EV will have a default
value of 0xFF if not written to. Event Variable 1 will contain the value of
the producer trigger or switch. If the Event Variables are populated as shown
in the table in the Consumed Events Section above, the LEDs will respond
accordingly to a Produced Event.

## Node Variables

Node Variables control the action to take when a switch is pressed or depressed.

The number of Node Variables (NV) is equal to the number of switches.
NV1 corresponds to the first pin defined in the array ```SWITCH```, 
NV2 corresponds to the second pin in that array, etc.

The following NV values define input switch function:

NV Value | Function
--------|--------
 0 | Do nothing
 1 | On/Off switch
 2 | On only push button
 3 | Off only push button
 4 | On/Off toggle push button
 
## Set Up

The module is initialised by pressing the VLCB button (formerly CBUS button) for
6 seconds when the green LED goes off and the Yellow LED flashes. A module name and
Node Number can then be set via the FCU in the normal manner.

## FCU Anomolies

Whilst the FCU will show newly taught events, if these re-assign a switch, as when
teaching a short event, it will not, for some reason, remove the now gone original
event automatically. It is necessary to highlight the redundant event and use
alt-D to remove it.

It should be noted that the use of alt-D only removes an event from the FCU internal
table.  It does not remove the event from the Pico events table.


 
 
 
 
