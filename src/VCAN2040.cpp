// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/

#include "VCAN2040.h"

namespace VLCB
{

// constants
static const uint32_t tx_qsize = 16;
static const uint32_t rx_qsize = 64;
static const uint32_t CANBITRATE = 125000UL;                // 125Kb/s - fixed for CBUS

// static pointer to object
static VCAN2040 *vcan2040p;

// can2040 context - owned here, since can2040 is driven directly.
// Only one bus is supported per VCAN2040 object / sketch, on PIO 0, 1 or (RP2350) 2.
static struct can2040 can_ctx;

// static callback function passed to can2040
static void cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
  vcan2040p->notify_cb(cd, notify, msg);
}

// PIO IRQ handlers - must be plain static functions, not class members
static void pio0_irq_handler(void)
{
  can2040_pio_irq_handler(&can_ctx);
}

static void pio1_irq_handler(void)
{
  can2040_pio_irq_handler(&can_ctx);
}

#ifdef PICO_RP2350
static void pio2_irq_handler(void)
{
  can2040_pio_irq_handler(&can_ctx);
}
#endif

// map PIO number to its IRQ number and handler; returns false if unsupported
static bool pioIrqInfo(byte pioNum, uint &irq, irq_handler_t &handler)
{
  switch (pioNum)
  {
  case 0:
    irq = PIO0_IRQ_0;
    handler = pio0_irq_handler;
    return true;
  case 1:
    irq = PIO1_IRQ_0;
    handler = pio1_irq_handler;
    return true;
#ifdef PICO_RP2350
  case 2:
    irq = PIO2_IRQ_0;
    handler = pio2_irq_handler;
    return true;
#endif
  default:
    return false;
  }
}

//
/// constructor
//
VCAN2040::VCAN2040()
{
  _num_rx_buffers = rx_qsize;
  _num_tx_buffers = tx_qsize;
  
  vcan2040p = this;
}

void VCAN2040::setPIO(byte pioNum)
{
  _pioNum = pioNum;
}

void VCAN2040::setPins(byte gpio_tx, byte gpio_rx)
{
  _gpio_tx = gpio_tx;
  _gpio_rx = gpio_rx;
}

//
/// initialise the CAN controller and buffers, and attach the ISR
//

bool VCAN2040::begin()
{
  uint irq;
  irq_handler_t handler;

  if (!pioIrqInfo(_pioNum, irq, handler))
  {
    return false;   // PIO instance not available on this chip
  }

  _numMsgsSent = 0;
  _numMsgsRcvd = 0;
  _numTxDropped = 0;
  _numRxErr = 0;
  _lastErrorCode = 0;
  _hwmRx = 0;
  _hwmTx = 0;
  
  /// allocate tx and rx buffers - using Pico SDK queue API

  queue_init(&tx_queue, sizeof(struct can2040_msg), _num_tx_buffers);
  queue_init(&rx_queue, sizeof(struct can2040_msg), _num_rx_buffers);

  /// initialise the can2040 CAN driver
  /// https://github.com/KevinOConnor/can2040/blob/master/docs/API.md#startup

  can2040_setup(&can_ctx, _pioNum);
  can2040_callback_config(&can_ctx, cb);

  // enable the PIO irq
  irq_set_exclusive_handler(irq, handler);
  irq_set_priority(irq, 1);
  irq_set_enabled(irq, true);

  // start canbus message processing
  can2040_start(&can_ctx, F_CPU, CANBITRATE, _gpio_rx, _gpio_tx);
  _running = true;
  return true;
}

//
/// stop can2040 and detach the PIO irq handler
//
void VCAN2040::stopDriver()
{
  uint irq;
  irq_handler_t handler;

  if (!_running)
  {
    return;
  }

  can2040_stop(&can_ctx);

  if (pioIrqInfo(_pioNum, irq, handler))
  {
    irq_set_enabled(irq, false);
    irq_remove_handler(irq, handler);
  }
  _running = false;
}

//
/// Called by CanService process function on a regular basis
/// attempt to send any buffered messages in the tx buffer
/// check for one or more messages in the receive buffer
//
bool VCAN2040::available()
{  
  struct can2040_msg tx_msg;

  /// attempt to drain down the tx buffer

  while (can2040_check_transmit(&can_ctx) && queue_try_remove(&tx_queue, &tx_msg))
  {
    if (can2040_transmit(&can_ctx, &tx_msg) != 0)
    {
      ++_numTxDropped;
    }
  }

  /// check for new received messages
  return (!queue_is_empty(&rx_queue));
}

//
/// get next message if a message is available in the buffer
//

CANFrame VCAN2040::getNextCanFrame(void)
{
  struct can2040_msg rx_msg;
  CANFrame frame = {};

  if (queue_try_remove(&rx_queue, &rx_msg))
  {
    frame.id = rx_msg.id & 0x1fffffffUL;     // strip can2040 RTR/EFF flag bits
    frame.len = rx_msg.dlc;

    frame.rtr = (rx_msg.id & CAN2040_ID_RTR) != 0;
    frame.ext = (rx_msg.id & CAN2040_ID_EFF) != 0;

    for (byte i = 0; i < rx_msg.dlc && i < 8; i++)
    {
      frame.data[i] = rx_msg.data[i];
    }
  }

  return frame;
}

//
/// callback - called from the PIO IRQ handler, so keep it short and don't print
//

void VCAN2040::notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg)
{
  (void)(cd);

  // error notifications carry an error code in the low bits, so test flags rather than equality
  if (notify & CAN2040_NOTIFY_RX)
  {
    ++_numMsgsRcvd;
    queue_try_add(&rx_queue, amsg);
    unsigned int level = queue_get_level(&rx_queue);
    if (level > _hwmRx)
    {
      _hwmRx = level;
    }
  }
  else if (notify & CAN2040_NOTIFY_ERROR)
  {
    ++_numRxErr;
    _lastErrorCode = notify & 0xfffffUL;   // error code is in the low 20 bits
  }
  else if (notify & CAN2040_NOTIFY_TX)
  {
    // frame has completed on the bus with a good ack
    ++_numMsgsSent;
  }
}

//
/// send a VLCB message
//
bool VCAN2040::sendCanFrame(CANFrame *frame)
{
  struct can2040_msg msg = {};

  msg.id = frame->id;
  msg.dlc = frame->len;

  if (frame->rtr)
    msg.id |= CAN2040_ID_RTR;

  if (frame->ext)
    msg.id |= CAN2040_ID_EFF;

  memcpy(msg.data, frame->data, frame->len > 8 ? 8 : frame->len);

  // send directly only if nothing is already waiting, to preserve ordering
  if (queue_is_empty(&tx_queue) && can2040_check_transmit(&can_ctx))
  {
    if (can2040_transmit(&can_ctx, &msg) == 0)
    {
      return true;
    }
    ++_numTxDropped;
    return false;
  }

  // can2040 tx queue full (or backlog exists) - buffer it, available() will drain it
  bool queued = queue_try_add(&tx_queue, &msg);
  unsigned int level = queue_get_level(&tx_queue);
  if (level > _hwmTx)
  {
    _hwmTx = level;
  }
  if (!queued)
  {
    ++_numTxDropped;
  }
  return queued;
}

//
/// retrieve can2040 statistics - see can2040 API docs
//
void VCAN2040::getStatistics(struct can2040_stats *stats)
{
  can2040_get_statistics(&can_ctx, stats);
}

//
/// bus-level error counters, derived from can2040 statistics
//
unsigned int VCAN2040::receiveErrorCounter()
{
  struct can2040_stats stats;
  can2040_get_statistics(&can_ctx, &stats);
  return stats.parse_error + _numRxErr;
}

unsigned int VCAN2040::transmitErrorCounter()
{
  struct can2040_stats stats;
  can2040_get_statistics(&can_ctx, &stats);
  // an attempt may be in flight, so clamp rather than go negative
  return (stats.tx_attempt > stats.tx_total) ? (stats.tx_attempt - stats.tx_total) : 0;
}

//
/// display the CAN bus status instrumentation
//
void VCAN2040::printStatus()
{
  // removed so that no libraries produce serial output
  // can be implemented in user's sketch, using getStatistics() and the counters above
}

//
/// reset the CAN transceiver
//

void VCAN2040::reset(void)
{
  stopDriver();
  queue_free(&rx_queue);
  queue_free(&tx_queue);
  begin();
}

//
/// set the number of CAN frame receive buffers
/// this can be tuned according to bus load and available memory
//

void VCAN2040::setNumBuffers(unsigned int num_rx_buffers, unsigned int num_tx_buffers)
{
  _num_rx_buffers = num_rx_buffers;
  _num_tx_buffers = num_tx_buffers;
}

}
