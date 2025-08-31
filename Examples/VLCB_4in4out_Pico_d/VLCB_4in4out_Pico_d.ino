// VLCB4IN4OUT
// Version for use with Raspberry Pi Pico with software CAN Controller.
// Uses both cores of the RP2040.

/*
   Copyright (C) 2023 Martin Da Costa
  This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
  Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
  The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0

  3rd party libraries needed for compilation:

  Streaming   -- C++ stream style output, v5, (http://arduiniana.org/libraries/streaming/)
  ACAN2040    -- library to support Pico PIO CAN controller implementation
*/
///////////////////////////////////////////////////////////////////////////////////
// Pin Use map:
// Pin 1   GP0  CAN Rx
// Pin 2   GP1  CAN Tx
// Pin 3        0V
// Pin 4   GP2  Not Used
// Pin 5   GP3  Not Used
// Pin 6   GP4  Not Used
// Pin 7   GP5  Not Used
// Pin 8        0V
// Pin 9   GP6  Not Used
// Pin 10  GP7  Not Used
// Pin 11  GP8  Not Used
// Pin 12  GP9  Not Used
// Pin 13   0V
// Pin 14  GP10 Not Used
// Pin 15  GP11 Not Used
// Pin 16  GP12 Not Used
// Pin 17  GP13 Request Switch (CBUS)
// Pin 18       0V
// Pin 19  GP14 CBUS green LED pin
// Pin 20  GP15 CBUS yellow LED pin
// Pin 21  GP16 Not Used
// Pin 22  GP17 Not Used
// Pin 23       0V
// Pin 24  GP18 Switch 1
// Pin 25  GP19 Switch 2
// Pin 26  GP20 Switch 3
// Pin 27  GP21 Switch 4
// Pin 28       0V
// Pin 29  GP22 LED 1
// Pin 30  RUN Reset (active low)
// Pin 31  GP26 LED 2
// Pin 32  GP27 LED 3
// Pin 33       0V
// Pin 34  GP28 LED 4
// Pin 35   ADC_VREF
// Pin 36   3V3
// Pin 37   3V3_EN
// Pin 38   0V
// Pin 39   VSYS
// Pin 40   VBUS
//////////////////////////////////////////////////////////////////////////

#define DEBUG 1  // set to 0 for no serial debug

#if DEBUG
#define DEBUG_PRINT(S) Serial << S << endl
#else
#define DEBUG_PRINT(S)
#endif

// 3rd party libraries
#include <Streaming.h>
#include <Bounce2.h>

// VLCB library header files
#include <VLCB.h>                   // Controller class
#include <VCAN2040.h>               // CAN controller
<<<<<<< HEAD
=======
#include <Configuration.h>             // module configuration
#include <Parameters.h>             // VLCB parameters
#include <vlcbdefs.hpp>               // VLCB constants
#include <LEDUserInterface.h>
#include "MinimumNodeServiceWithDiagnostics.h"
#include "CanServiceWithDiagnostics.h"
#include "NodeVariableService.h"
#include "EventConsumerServiceWithDiagnostics.h"
#include "EventProducerServiceWithDiagnostics.h"
#include "ConsumeOwnEventsService.h"
#include "EventTeachingServiceWithDiagnostics.h"
#include "SerialUserInterface.h"
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

// Module library header files
#include "LEDControl.h"

// forward function declarations
void eventhandler(byte, const VLCB::VlcbMessage *);
void requesthandler(byte, const VLCB::VlcbMessage *);
void printConfig();
void processSwitches();

// constants
const byte VER_MAJ = 1;          // code major version
const char VER_MIN = 'a';        // code minor version
const byte VER_BETA = 0;         // code beta sub-version
const byte MANUFACTURER = MANU_DEV;   // Module Manufacturer set to Development
const byte MODULE_ID = 82;       // VLCB module type

const byte LED_GRN = 14;         // VLCB green Unitialised LED pin
const byte LED_YLW = 15;         // VLCB yellow Normal LED pin
const byte SWITCH0 = 13;         // VLCB push button switch pin

<<<<<<< HEAD
=======
// Controller objects
VLCB::Configuration modconfig;               // configuration object
VLCB::VCAN2040 vcan2040; // (16,4);                  // CAN transport object
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
VLCB::SerialUserInterface serialUserInterface(&vcan2040);
VLCB::MinimumNodeServiceWithDiagnostics mnService;
VLCB::CanServiceWithDiagnostics canService(&vcan2040);
VLCB::NodeVariableService nvService;
VLCB::ConsumeOwnEventsService coeService;
VLCB::EventConsumerServiceWithDiagnostics ecService;
VLCB::EventTeachingServiceWithDiagnostics etService;
VLCB::EventProducerServiceWithDiagnostics epService;
VLCB::Controller controller(&modconfig,
                            {&mnService, &ledUserInterface, &serialUserInterface, &canService, &nvService, &ecService, &epService, &etService, &coeService}); // Controller object

>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
// module name, must be 7 characters, space padded.
char mname[] = "4IN4OUT";

// Module objects
const byte LED[] = { 22, 26, 27, 28 };     // LED pin connections through typ. 1K8 resistor
const byte SWITCH[] = { 18, 19, 20, 21 };  // Module Switch takes input to 0V.

const bool active = 0;  // 0 is for active low LED drive. 1 is for active high

const byte NUM_LEDS = sizeof(LED) / sizeof(LED[0]);
const byte NUM_SWITCHES = sizeof(SWITCH) / sizeof(SWITCH[0]);

// module objects
Bounce moduleSwitch[NUM_SWITCHES];  //  switch as input
LEDControl moduleLED[NUM_LEDS];     //  LED as output
bool state[NUM_SWITCHES];

<<<<<<< HEAD
VLCB::VCAN2040 vcan2040 (16,4);                  // CAN transport object

// Service objects
VLCB::LEDUserInterface ledUserInterface(LED_GRN, LED_YLW, SWITCH0);
VLCB::SerialUserInterface serialUserInterface;
VLCB::MinimumNodeService mnService;
VLCB::CanService canService(&vcan2040);
VLCB::NodeVariableService nvService;
VLCB::ConsumeOwnEventsService coeService;
VLCB::EventConsumerService ecService;
VLCB::EventTeachingService etService;
VLCB::EventProducerService epService;
=======
// forward function declarations
void eventhandler(byte, const VLCB::VlcbMessage *);
void requesthandler(byte, const VLCB::VlcbMessage *);
void printConfig();
void processSwitches();
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

//
///  setup VLCB - runs once at power on called from setup()
//
void setupVLCB()
{
  VLCB::checkStartupAction(LED_GRN, LED_YLW, SWITCH0);

  VLCB::setServices({
    &mnService, &ledUserInterface, &serialUserInterface, &canService, &nvService,
    &ecService, &epService, &etService, &coeService});
  // set config layout parameters
<<<<<<< HEAD
<<<<<<< HEAD
  VLCB::setNumNodeVariables(NUM_SWITCHES);
  VLCB::setMaxEvents(20);
  VLCB::setNumProducedEvents(NUM_SWITCHES);
  VLCB::setNumEventVariables(1 + NUM_LEDS);
=======
=======
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
  modconfig.EE_NUM_NVS = NUM_SWITCHES;
  modconfig.EE_MAX_EVENTS = 20;
  modconfig.EE_PRODUCED_EVENTS = NUM_SWITCHES;
  modconfig.EE_NUM_EVS = 1 + NUM_LEDS;  

  // initialise and load configuration
  controller.begin();

  Serial << F("> mode = ") << ((modconfig.currentMode) ? "Normal" : "Uninitialised") << F(", CANID = ") << modconfig.CANID;
  Serial << F(", NN = ") << modconfig.nodeNum << endl;
<<<<<<< HEAD
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

  // set module parameters
  VLCB::setVersion(VER_MAJ, VER_MIN, VER_BETA);
  VLCB::setModuleId(MANUFACTURER, MODULE_ID);

  // set module name
  VLCB::setName(mname);

  // register our VLCB event handler, to receive event messages of learned events
  ecService.setEventHandler(loadrcvdmess);
  epService.setRequestEventHandler(loadrcvdmess);
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

  // set Controller LEDs to indicate mode
  controller.indicateMode(modconfig.currentMode);
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

  // configure and start CAN bus and VLCB message processing
  //vcan2040.setNumBuffers(16, 4);  // more buffers = more memory used, fewer = less
  //vcan2040.setPio(0);             // PIO 0 is the default so this line commented out.  Alternative is value of 1
  vcan2040.setPins(1, 0);         // select pins for CAN Tx & Rx
  if (!vcan2040.begin()) {
    Serial << get_core_num() << F("> error starting VLCB") << endl;
  } else {
    Serial << get_core_num() << F("> VLCB started") << endl;
  }
  
  // initialise and load configuration
  VLCB::begin();

  Serial << F("> mode = (") << _HEX(VLCB::getCurrentMode()) << ") " << VLCB::Configuration::modeString(VLCB::getCurrentMode());
  Serial << F(", CANID = ") << VLCB::getCANID();
  Serial << F(", NN = ") << VLCB::getNodeNum() << endl;

  // show code version and copyright notice
  printConfig();
}

//
///  setup Module - runs once at power on called from setup()
//

void setupModule()
{
  // configure the module switches, active low
  for (byte i = 0; i < NUM_SWITCHES; i++)
  {
    moduleSwitch[i].attach(SWITCH[i], INPUT_PULLUP);
    moduleSwitch[i].interval(5);
    state[i] = false;
  }

  // configure the module LEDs
  for (byte i = 0; i < NUM_LEDS; i++)
  {
    moduleLED[i].setPin(LED[i], active);  //Second arguement sets 0 = active low, 1 = active high. Default if no second arguement is active high.
    moduleLED[i].off();
  }

  Serial << get_core_num() << "> Module has " << NUM_LEDS << " LEDs and " << NUM_SWITCHES << " switches." << endl;
}

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  Serial << endl << get_core_num() << F("> ** VLCB 4 in 4 out Pico Dual Core ** ") << __FILE__ << endl;

  // show code version and copyright notice
  printConfig();

  setupModule();

  // end of setup
  DEBUG_PRINT(get_core_num() << F("> Module ready"));
  // give core 1 the go ahead
  rp2040.fifo.push(0);
}

void setup1()
{
  // wait for go ahead from core 0
  rp2040.fifo.pop();

  setupVLCB();

<<<<<<< HEAD
<<<<<<< HEAD
  DEBUG_PRINT(get_core_num() << F("> VLCB ready"));
  digitalWrite(LED_BUILTIN, LOW);

  // end of setup
=======
  // end of setup
  DEBUG_PRINT(get_core_num() << F("> VLCB ready"));

>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
  // end of setup
  DEBUG_PRINT(get_core_num() << F("> VLCB ready"));

>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
}

void loop()
{
  if (rp2040.fifo.available() > 0)
  {
    byte msgLen = rp2040.fifo.pop();
    // Ensure Core 0 has loaded complete message into FIFO
    while((msgLen + 1) != rp2040.fifo.available())
    {}
    receivedData(msgLen);  // Action FIFO contents
  }

  // Run the LED code
  for (byte i = 0; i < NUM_LEDS; i++)
  {
    moduleLED[i].run();
  }

  // test for switch input
  processSwitches();

// end of loop0()
}

void loop1()
{
  // do VLCB message processing
<<<<<<< HEAD
<<<<<<< HEAD
  VLCB::process();
=======
  controller.process();
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
  controller.process();
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2

  if (rp2040.fifo.available() == 3)
  {
    uint8_t event = (uint8_t)rp2040.fifo.pop();
    uint8_t _state = (uint8_t)rp2040.fifo.pop();
    uint16_t sendData = (uint16_t)rp2040.fifo.pop();
    
    if (event)
    {
      epService.sendEvent(_state, sendData);
    }
    else
    {
      epService.sendEventResponse(_state, sendData);
    }
  }

// end of loop1()
}

void processSwitches(void)
{
  
  for (byte i = 0; i < NUM_SWITCHES; i++)
  {
    moduleSwitch[i].update();
    if (moduleSwitch[i].changed())
    {
      byte nv = i + 1;
<<<<<<< HEAD
      byte nvval = VLCB::readNV(nv);
=======
      byte nvval = modconfig.readNV(nv);
<<<<<<< HEAD
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
      byte swNum = i + 1;
      bool event = true;

      // DEBUG_PRINT(get_core_num() << F(" sk> Button ") << i << F(" state change detected ") << nvval);     

      switch (nvval)
      {
        case 1:
          // ON and OFF
          state[i] = (moduleSwitch[i].fell());
          DEBUG_PRINT(get_core_num() << F("sk> Button ") << i << (state[i] ? F(" pressed, send state: ") : F(" released, send state: ")) << state[i]);
          rp2040.fifo.push(event);
          rp2040.fifo.push(state[i]);
          rp2040.fifo.push(swNum);
          
          break;

        case 2:
          // Only ON
          if (moduleSwitch[i].fell())
          {
            state[i] = true;
            DEBUG_PRINT(get_core_num() << F("sk> Button ") << i << F(" pressed, send state: ") << state[i]);
            rp2040.fifo.push(event);
            rp2040.fifo.push(state[i]);
            rp2040.fifo.push(swNum);
          }
          break;

        case 3:
          // Only OFF
          if (moduleSwitch[i].fell())
          {
            state[i] = false;
            DEBUG_PRINT(get_core_num() << F("> Button ") << i << F(" pressed, send 0x") << state[i]);
            rp2040.fifo.push(event);
            rp2040.fifo.push(state[i]);
            rp2040.fifo.push(swNum);
          }
          break;

        case 4:
          // Toggle button
          if (moduleSwitch[i].fell())
          {
            state[i] = !state[i];
            
            DEBUG_PRINT(get_core_num() << F("> Button ") << i << (state[i] ? F(" pressed, send state: ") : F(" released, send state: ")) << state[i]);
            rp2040.fifo.push(event);
            rp2040.fifo.push(state[i]);
            rp2040.fifo.push(swNum);
          }
          break;

        default:
          DEBUG_PRINT(get_core_num() << F("sk> Button ") << i << F(" do nothing."));
          break;
      }
    }
  }
}

//
/// called from the VLCB library when a learned event is received
//

void loadrcvdmess(byte index, const VLCB::VlcbMessage *msg)
{
  rp2040.fifo.push(msg->len);
  rp2040.fifo.push(index);
  for (byte n = 0; n < msg->len; n++)
  {
    rp2040.fifo.push(msg->data[n]);
  }
 // DEBUG_PRINT(get_core_num() << F("> received message put: index = ") << index << F(", opcode = 0x") << _HEX(msg->data[0]));
 // DEBUG_PRINT(get_core_num() << F("> received message put: length = ") << msg->len);
}

void receivedData(byte length)
{
  byte rcvdData[length];
  byte index = rp2040.fifo.pop();
  for (byte n = 0; n < length; n++)
  {
    rcvdData[n] = rp2040.fifo.pop();
  }

  byte opc = rcvdData[0];
  delay(50);
 // DEBUG_PRINT(get_core_num() << F("> received data popped: index = ") << index << F(", opcode = 0x") << _HEX(rcvdData[0]));
 // DEBUG_PRINT(get_core_num() << F("> received data popped: length = ") << length);
/*
#if DEBUG
  unsigned int node_number = (rcvdData[1] << 8) + rcvdData[2];
  unsigned int event_number = (rcvdData[3] << 8) + rcvdData[4];
#endif

  DEBUG_PRINT(get_core_num() << F("> NN = ") << node_number << F(", EN = ") << event_number);
  DEBUG_PRINT(get_core_num() << F("> op_code = 0x") << _HEX(opc));
*/
  switch (opc) {
    case OPC_ACON:
    case OPC_ASON:
      for (byte i = 0; i < NUM_LEDS; i++)
      {
        byte ev = i + 2;
        byte evval = VLCB::getEventEVval(index, ev);

        switch (evval) 
        {
          case 1:
            moduleLED[i].on();
            break;

          case 2:
            moduleLED[i].flash(500);
            break;

          case 3:
            moduleLED[i].flash(250);
            break;

          default:
            break;
        }
      }
      break;

    case OPC_ACOF:
    case OPC_ASOF:
      for (byte i = 0; i < NUM_LEDS; i++) 
      {
        byte ev = i + 2;
        byte evval = VLCB::getEventEVval(index, ev);

        if (evval > 0) {
          moduleLED[i].off();
        }
      }
      break;

    case OPC_AREQ:
    case OPC_ASRQ:
      bool event = false;
<<<<<<< HEAD
<<<<<<< HEAD
      byte evval = VLCB::getEventEVval(index, 1) - 1;
=======
      byte evval = modconfig.getEventEVval(index, 1) - 1;
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
      byte evval = modconfig.getEventEVval(index, 1) - 1;
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
      DEBUG_PRINT(get_core_num() << F("> Handling request op =  ") << _HEX(opc) << F(", request input = ") << evval << F(", state = ") << state[evval]);
      rp2040.fifo.push(event);
      rp2040.fifo.push(state[evval]);
      rp2040.fifo.push(index);
      break;
  }
}

void printConfig(void) {
  // code version
  Serial << get_core_num() << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << get_core_num() << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;

  // copyright
  Serial << get_core_num() << F("> © Martin Da Costa (MERG M6223) 2024") << endl;
  
}
