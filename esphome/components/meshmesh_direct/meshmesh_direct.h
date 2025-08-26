#pragma once

#include "esphome/core/component.h"
#include "esphome/components/meshmesh/meshmesh.h"

namespace esphome {
namespace meshmesh {

class MeshMeshDirectComponent : public Component {
public:
  MeshMeshDirectComponent();
  void setup();
  void loop() override;
private:
  int8_t handleFrame(uint8_t *data, uint16_t len, uint32_t from);
private:
  MeshmeshComponent *mMeshmesh{nullptr};
};
}  // namespace meshmesh
}  // namespace esphome
