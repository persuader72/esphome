#include "meshmesh_switch.h"
#include "esphome/core/log.h"
#include "../commands.h"

namespace esphome {
namespace meshmesh {

static const char *TAG = "meshmesh.switch";

float MeshMeshSwitch::get_setup_priority() const { return setup_priority::HARDWARE; }

struct MeshMeshSwitchState {
  uint32_t address;
  uint16_t hash;
  bool initalState;
};

void MeshMeshSwitch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MeshMesh Switch '%s'...", this->name_.c_str());
  mMeshMesh = MeshmeshComponent::getInstance();
  mPreferences = global_preferences->make_preference<MeshMeshSwitchState>(get_object_id_hash(), true);

  struct MeshMeshSwitchState preferencesdata = {mAddress, mHash, false};
  if(this->restore_mode_ == switch_::SWITCH_RESTORE_DEFAULT_ON) preferencesdata.initalState = true;

  if(mPreferences.load(&preferencesdata)) {
    mAddress = preferencesdata.address;
    mHash = preferencesdata.hash;
  }

  switch (this->restore_mode_) {
    case switch_::SWITCH_ALWAYS_OFF:
      preferencesdata.initalState = false;
      break;
    case switch_::SWITCH_ALWAYS_ON:
      preferencesdata.initalState = true;
      break;
    case switch_::SWITCH_RESTORE_DEFAULT_OFF:
    case switch_::SWITCH_RESTORE_DEFAULT_ON:
    case switch_::SWITCH_RESTORE_INVERTED_DEFAULT_OFF:
    case switch_::SWITCH_RESTORE_INVERTED_DEFAULT_ON:
    case switch_::SWITCH_RESTORE_DISABLED:
      // TODO
      break;
  }

  // write state before setup
  if(preferencesdata.initalState) this->turn_on(); else this->turn_off();
}

void MeshMeshSwitch::dump_config() {
  LOG_SWITCH("", "GPIO Switch", this);
  //LOG_PIN("  Pin: ", this->pin_);
  const char *restore_mode = "";
  switch (this->restore_mode_) {
    case switch_::SWITCH_RESTORE_DEFAULT_OFF:
      restore_mode = "Restore (Defaults to OFF)";
      break;
    case switch_::SWITCH_RESTORE_DEFAULT_ON:
      restore_mode = "Restore (Defaults to ON)";
      break;
    case switch_::SWITCH_ALWAYS_OFF:
      restore_mode = "Always OFF";
      break;
    case switch_::SWITCH_ALWAYS_ON:
      restore_mode = "Always ON";
      break;
    case switch_::SWITCH_RESTORE_INVERTED_DEFAULT_OFF:
      restore_mode = "Restore (Defaults to OFF, Inverted)";
      break;
    case switch_::SWITCH_RESTORE_INVERTED_DEFAULT_ON:
      restore_mode = "Restore (Defaults to ON, Inverted)";
      break;
    case switch_::SWITCH_RESTORE_DISABLED:
      restore_mode = "Restore (Disabled)";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Restore Mode: %s", restore_mode);
}

void MeshMeshSwitch::write_state(bool state) {
  if(mMeshMesh) {
    uint8_t buff[6];
    buff[0] = CMD_SET_ENTITY_STATE_REQ;
    buff[1] = MeshmeshComponent::SwitchEntity;
    uint16toBuffer(buff+2, mHash);
    uint16toBuffer(buff+4, state ? 10 : 0);
    mMeshMesh->uniCastSendData(buff, 6, mAddress);
  }

  this->publish_state(state);
}

void MeshMeshSwitch::set_restore_mode(switch_::SwitchRestoreMode restore_mode) { this->restore_mode_ = restore_mode; }

}  // namespace gpio
}  // namespace esphome
