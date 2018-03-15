#ifndef __INCLUDE_KISS_H__
#define __INCLUDE_KISS_H__

#include <stdint.h>

class kiss {
private:
  uint8_t *const bufferBig, *const bufferSmall;
  const uint16_t maxPacketSize;
  bool (* peekRadio)();
  void (* getRadio)(uint8_t *const recv_buffer, uint16_t *const recv_buffer_size);
  void (* putRadio)(const uint8_t *const send_buffer, const uint16_t send_buffer_size);
  uint16_t (* peekSerial)();
  bool (* getSerial)(uint8_t *const recv_buffer, const uint16_t recv_buffer_size, const unsigned long int time_out);
  void (* putSerial)(const uint8_t *const send_buffer, const uint16_t send_buffer_size);
  bool (* resetRadio)();
  // void (* flushSerial)();

  void debugFrame(const uint8_t *const send_buffer, const uint16_t send_buffer_size);
  void processRadio();
  void processSerial();

public:
  kiss(const uint16_t maxPacketSize, bool (* peekRadio)(),
  void (* getRadio)(uint8_t *const recv_buffer, uint16_t *const recv_buffer_size),
  void (* putRadio)(const uint8_t *const send_buffer, const uint16_t send_buffer_size),
  uint16_t (* peekSerial)(), bool (* getSerial)(uint8_t *const recv_buffer,
  const uint16_t recv_buffer_size, const unsigned long int time_out),
  void (* putSerial)(const uint8_t *const send_buffer, const uint16_t send_buffer_size),
  bool (* resetRadio)()/*, void (* flushSerial)()*/);
  ~kiss();

  void debug(const char *const debug_string);

  void begin();
  void loop();
};

#endif
