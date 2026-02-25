// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#pragma once

// header files

#include <Controller.h>
#include <CanTransport.h>
#include <ACAN2040.h>          // ACAN2040 library
//#include <CircularBuffer.h>

namespace VLCB
{

// constants

static const uint32_t tx_qsize = 16;
static const uint32_t rx_qsize = 64;
//static const byte TX_QSIZE = 8;
//static const byte RX_QSIZE = 32;
static const uint32_t CANBITRATE = 125000UL;                // 125Kb/s - fixed for CBUS


//
/// an implementation of the Transport interface class
/// to support the software CAN controller on a Pico (RP2040)
//

class VCAN2040 : public CanTransport
{
public:

  VCAN2040();
  virtual ~VCAN2040();

  // these methods are declared virtual in the base class and must be implemented by the derived class
  bool begin(); //bool poll = false, SPIClassRP2040 spi = SPI);    // note default args
  bool available() override;
  CANFrame getNextCanFrame() override;
  bool sendCanFrame(CANFrame * msg) override;
  void reset() override;
  byte getHardwareType() override { return CAN_HW_RP2040_PIO; };

  void setNumBuffers(unsigned int num_rx_buffers, unsigned int num_tx_buffers);
  void setPins(byte tx_pin, byte rx_pin);
  void setPIO(byte pioNum);
  void printStatus(void);
  void notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg);

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

private:
  unsigned int _numMsgsSent, _numMsgsRcvd, _numSendErr, _hwmRx, _hwmTx;
  unsigned int _num_rx_buffers, _num_tx_buffers;
  byte _gpio_tx, _gpio_rx;
  byte _pioNum = 0;

};
}