// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#pragma once

// header files

#include <Controller.h>
#include <CanTransport.h>
#include <ACAN2040.h>           // ACAN2515 library
#include <CircularBuffer.h>

namespace VLCB
{

// constants

static const byte tx_qsize = 8;
static const byte rx_qsize = 32;
static const byte tx_pin = 1;    //Do I need this?
static const byte rx_pin = 2;    //Do I need this?
static const uint32_t CANBITRATE = 125000UL;                // 125Kb/s - fixed for CBUS


//
/// an implementation of the Transport interface class
/// to support the software CAN controller on a Pico (RP2040)
//

class VCAN2040 : public CanTransport
{
public:

  VCAN2040();

  // these methods are declared virtual in the base class and must be implemented by the derived class
  bool begin(bool poll = false, SPIClassRP2040 spi = SPI);    // note default args
  bool available(void);
  CANFrame getNextMessage(void);
  bool sendMessage(CANFrame * msg, bool rtr = false, bool ext = false, byte priority = DEFAULT_PRIORITY);   // note default arguments
  void reset(void);

  // these methods are specific to this implementation
  // they are not declared or implemented by the base CBUS class
  void setNumBuffers(byte num_rx_buffers, byte _num_tx_buffers = 2);
  void setPins(byte tx_pin, byte rx_pin);
  void printStatus(void);
  void notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg);








private:
  unsigned int _numMsgsSent, _numMsgsRcvd;
  byte _num_rx_buffers, _num_tx_buffers;

  CircularBuffer<CANFrame> rx_buffer;
  CircularBuffer<CANFrame> tx_buffer;  // Not currently used



}