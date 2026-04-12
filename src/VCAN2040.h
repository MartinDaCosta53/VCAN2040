/** \mainpage
@copyright Copyright © Martin Da Costa 2024 (martindc.merg@gmail.com)
 
 This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
 Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

# Introduction to VCAN2040
This library provides a software CAN interface on a Raspberry Pi Pico when used in conjunction with the VLCB_Arduino library
suite.  The main VLCB-Arduino library can be found at [VLCB](https://github.com/SvenRosvall/VLCB-Arduino).  It uses the ACAN2040 wrapper by Duncan Greenwood (https://github.com/obdevel/ACAN2040) for Kevin O'Connor's CAN driver using
the PIOs of the RP2040/RP235x microcontrollers (https://github.com/KevinOConnor/can2040).  ACAN2040 needs to be installed
seperately in an appropriate directory. It is a registered library in the Arduino IDE environment.

See [Design documents](https://github.com/SvenRosvall/VLCB-Arduino/blob/main/docs/Design.md) for how this library is structured.

## Examples
There are two versions of the example that, from the users perspective, are functionally identical.
The first uses a single core in the Pico and is identified as VLCB_4in4out_Pico_s, where 's' stands
for single core.  The other core will be dormant in a low power state.

The second version makes use of both cores in the processer and is identifed as VLCB_4in4out_Pico_d,
where 'd' stands for dual core.  This is organised such that the VLCB library runs in core 0 and the
application runs in core 1.

The docs folder in the VCAN2040 repository provides notes on how to use this library with the 4in4out examples.
It also holds a circuit schematic suitable for the examples.

## Hardware

Currently supports the Raspberry Pi Pico using Earle Philhowers Arduino IDE board support
[Pico Arduino IDE](https://github.com/earlephilhower/arduino-pico) Full instructions on how to do this in the associated
[documentation](https://arduino-pico.readthedocs.io/en/latest/)

*/

#pragma once

// header files

#include <Controller.h>
#include <CanTransport.h>
#include <ACAN2040.h>          // ACAN2040 library

namespace VLCB
{
/// # VCAN2040


// constants

static const uint32_t tx_qsize = 32;
static const uint32_t rx_qsize = 32;

static const uint32_t CANBITRATE = 125000UL;                // 125Kb/s - fixed for VLCB


//
/// an implementation of the Transport interface class
/// to support the software CAN controller on a Pico (RP2040)
//

class VCAN2040 : public CanTransport
{
public:
/// \cond
  VCAN2040();
  virtual ~VCAN2040();

  // these methods are declared virtual in the base class and must be implemented by the derived class
  bool begin(); //bool poll = false, SPIClassRP2040 spi = SPI);    // note default args
  bool available() override;
  CANFrame getNextCanFrame() override;
  bool sendCanFrame(CANFrame * msg) override;
  void reset() override;
  byte getHardwareType() override { return CAN_HW_RP2040_PIO; };
/// \endcond

  /// Define here the connections to the CAN Bus transceiver.
  /// Ensure that the RP2040/RP235x GPIO number is specified and NOT the physical device pin number.
  void setPins(byte tx_pin, byte rx_pin);
  
  /// This function allows the PIO that is used to be specified.  There are two PIOs in the RP2040,
  /// identified as PIO0 and PIO1) and thre in the RP253x, identified as PIO0, PIO1 and PIO2.
  /// If this function is not called, the default PIO is 0.
  void setPIO(byte pioNum);
  
  /// There are two buffer queues, one for transmit and one for receive.  These buffers pace
  /// the message rate so as not to overwhelm slower processes. The larger the buffer, the
  /// more memory it uses. This function allows the user to specify the size of the buffer.
  /// The default size is 32 for transmit and 32 for receive.
  void setNumBuffers(unsigned int num_rx_buffers, unsigned int num_tx_buffers);
  
/// \cond
  void notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg);
  void printStatus(void);
  
  virtual unsigned int receiveCounter()override { return _numMsgsRcvd; }
  virtual unsigned int transmitCounter()override { return _numMsgsSent; }
  virtual unsigned int receiveErrorCounter()override { return 0; }
  virtual unsigned int transmitErrorCounter()override { return _numSendErr; }
  virtual unsigned int receiveBufferUsage() override { return queue_get_level(&rx_queue); };
  virtual unsigned int transmitBufferUsage() override { return queue_get_level(&tx_queue); };
  virtual unsigned int receiveBufferPeak() override { return _hwmRx; };
  virtual unsigned int transmitBufferPeak() override { return _hwmTx; };
  virtual unsigned int errorStatus()override { return 0; }

  ACAN2040 *acan2040;   // pointer to CAN object so user code can access its members
  queue_t tx_queue, rx_queue;
/// \endcond
private:
  unsigned int _numMsgsSent, _numMsgsRcvd, _numSendErr, _hwmRx, _hwmTx;
  unsigned int _num_rx_buffers, _num_tx_buffers;
  byte _gpio_tx, _gpio_rx;
  byte _pioNum = 0;

};
}