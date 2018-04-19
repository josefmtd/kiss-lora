/* This is a software implementation of a KISS TNC using Arduino Sketch
 * This implementation is based on a work done by folkert@vanheusden.com
 * This program is modified by Josef Matondang and is available from
 * https://github.com/josefmtd/kiss-lora.
 *
 * If you have any questions of implementation of the software, please feel
 * free to contact me on josstemat@gmail.com
 */

#include "kiss.h"

/* Including a KISS class that is defined in a separate file, kiss.cpp and
 * kiss.h, this is an attempt to keep the program clean, so that KISS functions
 * can remain generic, and the hardware specific functions can remain in this
 * program.
 */

#include <SPI.h>
#include <RH_RF95.h>

/* To communicate with the LoRa device, RadioHead library is used. RadioHead
 * is ideal for this implementation since it can access LoRa device's packet
 * mode, in particular the Variable Length Packet Mode. This allows possibility
 * for using RadioHead to construct packet from 4-255 bytes. Current RH_RF95
 * implementation uses 4 bytes for Addressing.
 *
 * For more informations regarding RadioHead library you can find them in
 * http://www.airspayce.com/mikem/arduino/RadioHead/
 * This implementation uses RadioHead-1.83 which can be accessed from:
 * http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.83.zip
 */

/* for LoRa32u4
 *  #define pinReset 4
 *  static RH_RF95 rf95(8,7)
 */

#define pinReset 7
static RH_RF95 rf95;

// Hardware Specific Functions Pointer

bool peekRadio() {
  return rf95.available();
}

void getRadio(uint8_t *const recv_buffer, uint16_t *const recv_buffer_size) {
  uint8_t dummy = *recv_buffer_size;
  rf95.recv(recv_buffer, &dummy);
  *recv_buffer_size = dummy;
}

void putRadio(const uint8_t *const send_buffer, const uint16_t send_buffer_size) {
  rf95.send(send_buffer, send_buffer_size);
  rf95.waitPacketSent();
}

uint16_t peekSerial() {
  return Serial.available();
}
/*
void flushSerial() {
  Serial.flush();
}
*/
bool getSerial(uint8_t *const recv_buffer, const uint16_t recv_buffer_size, const unsigned long int time_out) {
  for (uint16_t i = 0; i < recv_buffer_size; i++) {
    while(!Serial.available()) {
      if (millis() >= time_out) {
        return false;
      }
    }
    recv_buffer[i] = Serial.read();
  }

  return true;
}

void putSerial(const uint8_t *const send_buffer, const uint16_t send_buffer_size) {
  Serial.write(send_buffer, send_buffer_size);
}

bool initRadio() {
  if (rf95.init()) {
    delay(100);
    return true;
  }
  return false;
}

bool resetRadio() {
  digitalWrite(pinReset, LOW);
  delay(1);
  digitalWrite(pinReset, HIGH);
  delay(5+1); // Must wait 5 ms before init

  return initRadio();
}

kiss k(255, peekRadio, getRadio, putRadio, peekSerial, getSerial, putSerial, resetRadio/*, flushSerial*/);

void setup() {
  Serial.begin(19200);

  while(!Serial);

  pinMode(pinReset, OUTPUT);
  digitalWrite(pinReset, HIGH);

  k.begin();

  if (!resetRadio()) {
    k.debug("Radio init failed");
  }
  else {
    k.debug("Radio init success");
  }
}

void loop() {
  k.loop();

  const unsigned long int now = millis();

  static unsigned long int lastReset = 0;
  const unsigned long int resetInterval = 301000;

  if (now - lastReset >= resetInterval) {
    // k.debug("Reset Radio");
    resetRadio();
    lastReset = now;
  }
}
