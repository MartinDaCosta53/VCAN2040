// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#pragma once

// header files

#include <Controller.h>
#include <CanTransport.h>
#include "ACAN2040.h"           // ACAN2040 library
#include <CircularBuffer.h>

namespace VLCB
{

// constants

static const byte TX_QSIZE = 8;
static const byte RX_QSIZE = 32;
static const uint32_t CANBITRATE = 125000UL;                // 125Kb/s - fixed for CBUS


//
/// an implementation of the Transport interface class
/// to support the software CAN controller on a Pico (RP2040)
//

class VCAN2040 : public CanTransport
{
public:

  VCAN2040(byte rx_qsize = RX_QSIZE, byte tx_qsize = TX_QSIZE);
  virtual ~VCAN2040();

  // these methods are declared virtual in the base class and must be implemented by the derived class
  bool begin(); //bool poll = false, SPIClassRP2040 spi = SPI);    // note default args
  bool available() override;
  CANFrame getNextCanFrame() override;
  bool sendCanFrame(CANFrame * msg) override;
  void reset() override;

  //void setNumBuffers(byte num_rx_buffers, byte _num_tx_buffers = 2);
  void setPins(byte tx_pin, byte rx_pin);
  void setPIO(byte pioNum);
  void printStatus(void);
  void notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg);

  virtual unsigned int receiveCounter()override { return _numMsgsRcvd; }
  virtual unsigned int transmitCounter()override { return _numMsgsSent; }
  virtual unsigned int receiveErrorCounter()override { return 0; }
<<<<<<< HEAD
  virtual unsigned int transmitErrorCounter()override { return _numSendErr; }
=======
  virtual unsigned int transmitErrorCounter()override { return 0; }
<<<<<<< HEAD
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
=======
>>>>>>> 4f8ef86a687c152d7e1e246fb4015a51268a9df2
  virtual unsigned int receiveBufferUsage() override { return 0; };
  virtual unsigned int transmitBufferUsage() override { return 0; };
  virtual unsigned int receiveBufferPeak() override { return 0; };
  virtual unsigned int transmitBufferPeak() override { return 0; };
  virtual unsigned int errorStatus()override { return 0; }

  ACAN2040 *acan2040;   // pointer to CAN object so user code can access its members

private:
  unsigned int _numMsgsSent, _numMsgsRcvd, _numSendErr;
  byte _num_rx_buffers, _num_tx_buffers;
  byte _gpio_tx, _gpio_rx;
  byte _pioNum = 0;

  CircularBuffer<CANFrame> rx_buffer;
  CircularBuffer<CANFrame> tx_buffer;  // Not currently used

};
}