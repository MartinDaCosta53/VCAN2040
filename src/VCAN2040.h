// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#pragma once

// header files

#include <Controller.h>
#include <CanTransport.h>

// Pico SDK headers (supplied with the arduino-pico core)
#include <hardware/irq.h>
#include <pico/util/queue.h>

// can2040 software CAN driver (C library, used directly - no wrapper class)
extern "C" {
#include "can2040.h"
}

namespace VLCB
{

/// @brief Transport implementation for the software CAN controller on a RP2040 / RP2350
///
/// Talks to the can2040 C API directly. The PIO IRQ handler and the can2040
/// context are owned by this class (one instance / one PIO per sketch).
class VCAN2040 : public CanTransport
{
public:

  VCAN2040();

  /// @cond LIBRARY
  // these methods are declared virtual in the base class and must be implemented by the derived class
  virtual bool begin();
  virtual bool available() override;
  virtual CANFrame getNextCanFrame() override;
  virtual bool sendCanFrame(CANFrame * msg) override;
  virtual void reset() override;
  virtual byte getHardwareType() override { return CAN_HW_RP2040_PIO; };

  // these methods are specific to this implementation
  // they are not declared or implemented by the Transport interface class
  void setNumBuffers(unsigned int num_rx_buffers, unsigned int num_tx_buffers);
  void setPins(byte tx_pin, byte rx_pin);
  void setPIO(byte pioNum);
  void printStatus(void);
  void notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg);

  /// retrieve the underlying can2040 statistics
  void getStatistics(struct can2040_stats *stats);

  virtual unsigned int receiveCounter() override { return _numMsgsRcvd; }
  virtual unsigned int transmitCounter() override { return _numMsgsSent; }
  // bus-level errors, taken from can2040 statistics
  virtual unsigned int receiveErrorCounter() override;    // parse errors + rx overflow resets
  virtual unsigned int transmitErrorCounter() override;   // transmit attempts that did not complete

  /// frames lost locally because both the can2040 and software tx queues were full
  unsigned int droppedFrameCounter() { return _numTxDropped; }

  /// error status from the most recent CAN2040_NOTIFY_ERROR (always 0 in current can2040)
  virtual unsigned int receiveBufferSize() override { return 0; };
  virtual unsigned int transmitBufferSize() override { return 0; };
  virtual unsigned int receiveBufferUsage() override { return queue_get_level(&rx_queue); };
  virtual unsigned int transmitBufferUsage() override { return queue_get_level(&tx_queue); };
  virtual unsigned int receiveBufferPeak() override { return _hwmRx; };
  virtual unsigned int transmitBufferPeak() override { return _hwmTx; };
  virtual unsigned int errorStatus() override { return _lastErrorCode; } 
  /// @endcond

private:
  void stopDriver();

  volatile unsigned int _numMsgsSent = 0, _numMsgsRcvd = 0, _numTxDropped = 0, _numRxErr = 0, _lastErrorCode = 0;
  volatile unsigned int _hwmRx = 0, _hwmTx = 0;
  unsigned int _num_rx_buffers, _num_tx_buffers;
  byte _gpio_tx = 0, _gpio_rx = 0;
  byte _pioNum = 0;
  bool _running = false;
  queue_t tx_queue, rx_queue;
};
}
