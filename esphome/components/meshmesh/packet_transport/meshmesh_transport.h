#pragma once
#include "esphome/core/defines.h"
#include "esphome/components/meshmesh/meshmesh.h"
#ifdef USE_NETWORK
#include "esphome/core/component.h"
#include "esphome/components/packet_transport/packet_transport.h"
#include <vector>

namespace esphome {
namespace meshmesh {

class MeshmeshTransport : public packet_transport::PacketTransport, public Parented<MeshmeshComponent> {
public:
  void setup() override;
  void update() override;

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_address(uint32_t address) { this->address_ = address; }

protected:
  void send_packet(const std::vector<uint8_t> &buf) const override;
  size_t get_max_packet_size() override { return 999; }

private:
  int8_t handleFrame(uint8_t *buf, uint16_t len, uint32_t from);

  private:
  uint32_t address_{0};
};

}  // namespace meshmesh
}  // namespace esphome
#endif
