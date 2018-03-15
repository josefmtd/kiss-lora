#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "kiss.h"

#define FEND  0xC0
#define FESC  0xDB
#define TFEND 0xDC
#define TFESC 0xDD

#define KISS_NO_CRC   0x00
#define KISS_FLEXNET  0x20
#define KISS_SMACK    0x80

kiss::kiss(const uint16_t maxPacketSizeIn, bool (* peekRadioIn)(),
void (* getRadioIn)(uint8_t *const recv_buffer, uint16_t *const recv_buffer_size),
void (* putRadioIn)(const uint8_t *const send_buffer, const uint16_t send_buffer_size),
uint16_t (* peekSerialIn)(), bool (* getSerialIn)(uint8_t *const recv_buffer,
const uint16_t recv_buffer_size, const unsigned long int time_out),
void (* putSerialIn)(const uint8_t *const send_buffer, const uint16_t send_buffer_size),
bool (* resetRadioIn)()/*, void (* flushSerialIn)()*/) : maxPacketSize(maxPacketSizeIn), bufferBig(new uint8_t[maxPacketSizeIn * 2]),
bufferSmall(new uint8_t[maxPacketSizeIn]), peekRadio(peekRadioIn), getRadio(getRadioIn),
putRadio(putRadioIn), peekSerial(peekSerialIn), getSerial(getSerialIn), putSerial(putSerialIn),
resetRadio(resetRadioIn)/*, flushSerial(flushSerialIn)*/ {
  debug("Starting KISS mode");
}

kiss::~kiss() {
  delete [] bufferSmall;
  delete [] bufferBig;
}

void kiss::begin() {

}

void put_byte(uint8_t *const out, uint16_t *const offset, const uint8_t buffer) {
  if (buffer == FEND) {
    out[(*offset)++] = FESC;
    out[(*offset)++] = TFEND;
  }

  else if (buffer == FESC) {
    out[(*offset)++] = FESC;
    out[(*offset)++] = TFESC;
  }

  else {
    out[(*offset)++] = buffer;
  }
}

void kiss::debug(const char *const debug_string) {
  uint8_t myBuffer[128];
  const uint8_t ax25_ident[] = { 0x92, 0x88, 0x8A, 0x9C, 0xA8, 0x40, 0xE0, 0x88,
  0x8A, 0x84, 0xAA, 0x8E, 0x60, 0x63, 0x03, 0xF0};

  uint16_t offset = 0;
  myBuffer[offset++] = 0x00;

  // Adding AX25 Address to the Buffer
  for (uint8_t i = 0; i < sizeof(ax25_ident); i++) {
    myBuffer[offset++] = ax25_ident[i];
  }

  uint8_t length = strlen(debug_string);

  for (uint8_t i = 0; i < length; i++) {
    myBuffer[offset++] = debug_string[i];
  }

  const uint8_t fend = FEND;
  putSerial(&fend, 1);

  for (uint8_t i = 0; i < offset; i++) {
    uint8_t tinyBuffer[4];
    uint16_t offset1 = 0;
    put_byte(tinyBuffer, &offset1, myBuffer[i]);
    putSerial(tinyBuffer, offset1);
  }

  putSerial(&fend, 1);
}


void kiss::debugFrame(const uint8_t *const send_buffer, const uint16_t send_buffer_size) {
  uint8_t myBuffer[256];
  const uint8_t ax25_ident[] = { 0x92, 0x88, 0x8A, 0x9C, 0xA8, 0x40, 0xE0, 0x88,
  0x8A, 0x84, 0xAA, 0x8E, 0x60, 0x63, 0x03, 0xF0};

  uint16_t offset = 0;
  myBuffer[offset++] = 0x00;

  // Adding AX25 Address to the Buffer
  for (uint8_t i = 0; i < sizeof(ax25_ident); i++) {
    myBuffer[offset++] = ax25_ident[i];
  }

  for (uint16_t i = 0; i < send_buffer_size; i++) {
    myBuffer[offset++] = send_buffer[i];
  }

  const uint8_t fend = FEND;
  putSerial(&fend, 1);

  for (uint8_t i = 0; i < offset; i++) {
    uint8_t tinyBuffer[4];
    uint16_t offset1 = 0;
    put_byte(tinyBuffer, &offset1, myBuffer[i]);
    putSerial(tinyBuffer, offset1);
  }

  putSerial(&fend, 1);
}


void kiss::processRadio() {
  uint16_t nBytes = maxPacketSize;
  getRadio(bufferSmall, &nBytes);

  uint16_t offset = 0;

  bufferBig[offset++] = FEND;

  for (uint16_t i = 0; i < nBytes; i++) {
    put_byte(bufferBig, &offset, bufferSmall[i]);
  }

  bufferBig[offset++] = FEND;

  putSerial(bufferBig,offset);
}

void kiss::processSerial() {
  bool first = true;
  bool frame_ok = false;
  bool escape = false;

  uint16_t offset = 0;

  const unsigned long int end = millis() + 5000;

  while(millis() < end && frame_ok == false && offset < maxPacketSize) {
    uint8_t buffer = 0;

    if (!getSerial(&buffer, 1, end)) {
      //debug("Serial Receive Timeout");
      //debugFrame(&bufferSmall[0], offset);
      break;
    }

    switch(buffer) {
      case FEND:
        if (first) {
          first = false;
        }
        else {
          // flushSerial();
          frame_ok = true;
        }
        break;
      case FESC:
        escape = true;
        break;
      default:
        if (escape) {
          if (buffer == TFEND) {
            bufferSmall[offset++] = FEND;
          }
          else if (buffer == TFESC) {
            bufferSmall[offset++] = FESC;
          }
          else {
            debug("error escape");
          }

          escape = false;
        }
        else {
          bufferSmall[offset++] = buffer;
        }
    }
  }

  if (frame_ok) {
    if (offset > 1) {
      switch(bufferSmall[0]) {
        case KISS_NO_CRC:
          putRadio(&bufferSmall[0], offset);
          break;
        case KISS_FLEXNET:
          debug("Cannot accept FlexNet");
          break;
        case KISS_SMACK:
          debug("Cannot accept SMACK");
          break;
        default:
          debug("Frame Unknown");
      }
    }
    else {
      debug("Too Small for a KISS Frame");
    }
  }
}

void kiss::loop() {
  if (peekRadio()) {
    processRadio();
  }

  uint16_t serialIn = peekSerial();
  if (serialIn != 0) {
    if (serialIn == 127) {
      debug("Serial Buffer Overrun");
    }
    processSerial();
  }
}
