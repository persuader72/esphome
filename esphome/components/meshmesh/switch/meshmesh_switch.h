#pragma once

#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/switch/switch.h"
#include "../meshmesh.h"

namespace esphome {
namespace meshmesh {

class MeshMeshSwitch : public switch_::Switch, public Component {
public:
  void set_target(uint16_t hash, uint32_t address) { mHash = hash; mAddress = address; }
  void set_restore_mode(switch_::SwitchRestoreMode restore_mode);
  float get_setup_priority() const override;
  void setup() override;
  void dump_config() override;
protected:
  void write_state(bool state) override;
private:
  uint16_t mHash;
  uint32_t mAddress;
  ESPPreferenceObject mPreferences;
  MeshmeshComponent *mMeshMesh{0};
  switch_::SwitchRestoreMode restore_mode_{switch_::SWITCH_RESTORE_DEFAULT_OFF};
};

}  // namespace gpio
}  // namespace esphome
