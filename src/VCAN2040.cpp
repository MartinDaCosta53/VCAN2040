// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/


// 3rd party libraries
#include <Streaming.h>


#include <VCAN2040.h>

namespace VLCB
{

// static pointer to object
VCAN2040 *vcan2040p;

// static callback function
static void cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
  vcan2040p->notify_cb(cd, notify, msg);
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

VCAN2040::~VCAN2040()
{}

void VCAN2040::setPins(byte gpio_tx, byte gpio_rx)
{
  _gpio_tx = gpio_tx;
  _gpio_rx = gpio_rx;
}

//
/// initialise the CAN controller and buffers, and attach the ISR
/// default poll arg is set to false, so as not to break existing code
//

bool VCAN2040::begin()
{
  _numMsgsSent = 0;
  _numMsgsRcvd = 0;
  _numSendErr = 0;
  
  /// allocate tx and rx buffers - using Pico SDK queue API

  queue_init(&tx_queue, sizeof(struct can2040_msg), _num_tx_buffers);
  queue_init(&rx_queue, sizeof(struct can2040_msg), _num_rx_buffers);

  /// initialise the can2040 CAN driver

  acan2040 = new ACAN2040(_pioNum, _gpio_tx, _gpio_rx, CANBITRATE, F_CPU, cb);
  acan2040->begin();
  return true;
}

//
/// Called by CanService process function on a regular basis
/// attempt to send any buffered messages in the tx buffer
/// check for one or more messages in the receive buffer
//
bool VCAN2040::available()
{  
  CANFrame frame;
  struct can2040_msg tx_msg;

  /// attempt to drain down the tx buffer

  while (acan2040->ok_to_send() && queue_try_remove(&tx_queue, &tx_msg)) {
    acan2040->send_message(&tx_msg);
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
  CANFrame frame;

  if (queue_try_remove(&rx_queue, &rx_msg)) {

    frame.id = rx_msg.id;
    frame.len = rx_msg.dlc;

    frame.rtr = (rx_msg.id & CAN2040_ID_RTR);
    frame.ext = (rx_msg.id & CAN2040_ID_EFF);

    for (byte i = 0; i < rx_msg.dlc && i < 8; i++) {
      frame.data[i] = rx_msg.data[i];
    }

    ++_numMsgsRcvd;
  }

  return frame;
}

//
/// callback
//

void VCAN2040::notify_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *amsg)
{
  (void)(cd);

  switch (notify)
  {
  case CAN2040_NOTIFY_RX:
    //Serial.printf("acan2040 cb: message received\n");

   queue_try_add(&rx_queue, amsg);
    break;

  case CAN2040_NOTIFY_TX:
    //sendFrames--;
    //Serial.printf("acan2040 cb: message sent ok\n");
    break;
  case CAN2040_NOTIFY_ERROR:
    Serial.printf("acan2040 cb: an error occurred\n");
    break;
  default:
    Serial.printf("acan2040 cb: unknown event type\n");
    break;
  }

  return;
}

//
/// send a VLCB message
//
bool VCAN2040::sendCanFrame(CANFrame *frame)
{
  struct can2040_msg msg;
  
  //sendFrames++;
  
 
  //DEBUG_SERIAL << F("vcan2040> Send Frames waiting = ") << sendFrames << F(". Wait Time = ") << waitTime << F(" usecs") << endl;
  msg.id = frame->id;  
  msg.dlc = frame->len;
  
  if (frame->rtr)
    msg.id |= 0x40000000;

  if (frame->ext)
    msg.id |= 0x80000000;

  memcpy(msg.data, frame->data, frame->len);
  
  if (acan2040->ok_to_send())
  {
    //Serial.printf("vcan2040> ok\n");
    ++_numMsgsSent;
    return (acan2040->send_message(&tx_msg));
  } else {
    //Serial.printf("vcan2040> error sending message\n");
    return (queue_try_add(&tx_queue, &tx_msg));
  }  
}

//
/// display the CAN bus status instrumentation
//
void VCAN2040::printStatus()
{
  // removed so that no libraries produce serial output
  // can be implemented in user's sketch

  /*
    DEBUG_SERIAL << F("> VLCB status:");
    DEBUG_SERIAL << F(" messages received = ") << _numMsgsRcvd << F(", sent = ") << _numMsgsSent << F(", receive errors = ") << endl;
           // acan2040->receiveErrorCounter() << F(", transmit errors = ") << acan2040->transmitErrorCounter() << F(", error flag = ")
           // << acan2040->errorFlagRegister()
           // << endl;
   */
}

//
/// reset the CAN transceiver
//

void VCAN2040::reset(void)
{
  acan2040->stop();
  queue_free(&rx_queue);
  queue_free(&tx_queue);
  delete acan2040;
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