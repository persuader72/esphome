#pragma once

#include "meshmesh_direct.h"
#include "esphome/components/meshmesh/meshmesh.h"

#include "esphome/core/automation.h"
#include "esphome/core/base_automation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace meshmesh {

template<typename... Ts> class SendAction : public Action<Ts...>, public Parented<MeshMeshDirectComponent> {
  TEMPLATABLE_VALUE(uint32_t, address);
  TEMPLATABLE_VALUE(std::vector<uint8_t>, data);

  void play_complex(Ts... x) override {
    uint32_t address = this->address_.value(x...);
    std::vector<uint8_t> data = this->data_.value(x...);
    ESP_LOGI("meshmesh_direct", "Sending data %d to %d", data.size(), address);
    this->parent_->meshmesh()->uniCastSendData(data.data(), data.size(), address);
  }

  void play(Ts... x) override { /* ignore - see play_complex */
  }

}; // class SendAction

}  // namespace meshmesh
}  // namespace esphome
