#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/logger/logger.h"

#ifdef USE_ESP8266
extern "C" {
  void IRAM_ATTR __wrap_ppEnqueueRxq(void *a);
  void IRAM_ATTR __real_ppEnqueueRxq(void *);
}
#endif

namespace espmeshmesh {
  class EspMeshMesh;
}

namespace esphome {
namespace meshmesh {

enum UARTSelection {
  UART_SELECTION_UART0 = 0,
  UART_SELECTION_UART1,
  UART_SELECTION_UART2,
};

struct MeshmeshSettings {
  char devicetag[32];
  // Log destination
  uint32_t log_destination;
  uint8_t channel;
  uint8_t txPower;
  uint32_t groups;
} __attribute__((packed));

class MeshmeshComponent : public Component {
public:
  static MeshmeshComponent *singleton;
  static MeshmeshComponent *getInstance();
  explicit MeshmeshComponent(int baud_rate, int tx_buffer, int rx_buffer);
  espmeshmesh::EspMeshMesh *getNetwork() { return mesh; }
  void setChannel(int channel) { mConfigChannel = channel; }
  void setAesPassword(const char *password) { mAesPassword = password; }
  void set_uart_selection(UARTSelection uart_selection) { /*FIXME: uart_ = uart_selection;*/}
private:
  void defaultPreferences();
  void preSetupPreferences();
  uint8_t mConfigChannel;
  const char *mAesPassword;
  ESPPreferenceObject mPreferencesObject;
  MeshmeshSettings mPreferences;
public:
  void pre_setup();
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BEFORE_CONNECTION; }
  void loop() override;
private:
  int8_t handleFrame(uint8_t *buf, uint16_t len, uint32_t from);
  espmeshmesh::EspMeshMesh *mesh;
};

void logPrintfCb(int level, const char *tag, int line, const char *format, va_list args);

}  // namespace meshmesh
}  // namespace esphome

