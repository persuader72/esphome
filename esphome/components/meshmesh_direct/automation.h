#pragma once

#include "meshmesh_direct.h"

#include "esphome/core/automation.h"
#include "esphome/core/base_automation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace meshmesh {

template<typename... Ts> class SendAction : public Action<Ts...>, public Parented<MeshMeshDirectComponent> {
  TEMPLATABLE_VALUE(uint32_t, address);
  TEMPLATABLE_VALUE(std::vector<uint8_t>, data);

  void play(Ts... x) override {
    ESP_LOGI("meshmesh_direct", "Sending data to %d", this->address_.value(x...));
  }

}; // class SendAction

}  // namespace meshmesh
}  // namespace esphome
