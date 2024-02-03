// Copyright (C) Martin Da Costa 2024 (martindc.merg@gmail.com)
// This file is part of VLCB-Arduino project on https://github.com/SvenRosvall/VLCB-Arduino
// Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// The full licence can be found at: http://creativecommons.org/licenses/by-nc-sa/4.0/


// 3rd party libraries
#include <Streaming.h>

// VLCB MCP2515 library
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
VCAN2040::VCAN2040(byte rx_qsize, byte tx_qsize)
  : rx_buffer(rx_qsize)
  , tx_buffer(tx_qsize)
{
  vcan2040p = this;
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

bool VCAN2040::begin() //bool poll, SPIClassRP2040 spi)
{
//  (void)(spi);
//  (void)poll;

  acan2040 = new ACAN2040(0, _gpio_tx, _gpio_rx, CANBITRATE, F_CPU, cb);
  acan2040->begin();
  return true;
}

//
/// check if a message is available in the buffer.
//
bool VCAN2040::available()
{
  return rx_buffer.available();
}

//
/// get next message if a message is available in the buffer
//

CANFrame VCAN2040::getNextCanFrame(void)
{
  ++_numMsgsRcvd;
  return rx_buffer.pop(); 
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
    Serial.printf("acan2040 cb: message received\n");

    CANFrame frame;
    frame.id = amsg->id;
    frame.len = amsg->dlc;
    frame.rtr = amsg->id & CAN2040_ID_RTR;
    frame.ext = amsg->id & CAN2040_ID_EFF;
    memcpy(frame.data, amsg->data, amsg->dlc);

    rx_buffer.put(frame);
    break;

  case CAN2040_NOTIFY_TX:
    Serial.printf("acan2040 cb: message sent ok\n");
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
  
  if (!acan2040->ok_to_send())
  {
    Serial.print("no space available to send message");
    return false;
  }
  
  msg.id - frame->id;
  
  if (frame->rtr)
    msg.id |= 0x40000000;

  if (frame->ext)
    msg.id |= 0x80000000;
  
  msg.dlc = frame->len;
  memcpy(msg.data, frame->data, frame->len);
  
  if (acan2040->send_message(&msg))
  {
  //  Serial.printf("ok\n");
    return true;
  } else {
    Serial.printf("error sending message\n");
    return false;
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
  rx_buffer.clear();
  tx_buffer.clear();
  delete acan2040;
  begin();
}

//
/// set the number of CAN frame receive buffers
/// this can be tuned according to bus load and available memory
//
/*
void VCAN2040::setNumBuffers(byte num_rx_buffers, byte num_tx_buffers) {
  rx_buffer(num_rx_buffers);
  tx_buffer(num_tx_buffers);
}
*/
}