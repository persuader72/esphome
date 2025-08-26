#include "meshmesh_direct.h"
#include "entities.h"

#include "esphome/components/meshmesh/commands.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

#include <functional>

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_LIGHT
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_output.h"
#endif

static const char *TAG = "meshmesh_direct";

namespace esphome {
namespace meshmesh {

MeshMeshDirectComponent *MeshMeshDirectComponent::singleton = nullptr;

MeshMeshDirectComponent *MeshMeshDirectComponent::getInstance() { return singleton; }

MeshMeshDirectComponent::MeshMeshDirectComponent() : Component() {
  if(singleton == nullptr)
    singleton = this;
}

void MeshMeshDirectComponent::setup() {
    ESP_LOGE(TAG, "Setting up MeshMeshDirectComponent");
    mMeshmesh = MeshmeshComponent::getInstance();
    mMeshmesh->addHandleFrameCb(std::bind(&MeshMeshDirectComponent::handleFrame, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void MeshMeshDirectComponent::loop() {
    ESP_LOGVV(TAG, "Looping MeshMeshDirectComponent");
}

int8_t MeshMeshDirectComponent::handleFrame(uint8_t *buf, uint16_t len, uint32_t from) {
    ESP_LOGE(TAG, "Handling frame from %06X with length %d", from, len);

    int8_t err = 0;
    switch (buf[0]) {
      case CMD_ENTITIES_COUNT_REQ:
      if (len == 1) {
        uint8_t rep[LastEntity + 1];
        rep[0] = CMD_ENTITIES_COUNT_REP;
        uint8_t *buf = rep + 1;
        os_memset(buf, 0, LastEntity);

#ifdef USE_SENSOR
        buf[SensorEntity] = (uint8_t) App.get_sensors().size();
        buf[AllEntities] += buf[SensorEntity];
#endif

#ifdef USE_BINARY_SENSOR
        buf[BinarySensorEntity] = (uint8_t) App.get_binary_sensors().size();
        buf[AllEntities] += buf[BinarySensorEntity];
#endif

#ifdef USE_SWITCH
        buf[SwitchEntity] = (uint8_t) App.get_switches().size();
        buf[AllEntities] += buf[SwitchEntity];
#endif

#ifdef USE_LIGHT
        buf[LightEntity] = (uint8_t) App.get_lights().size();
        buf[AllEntities] += buf[LightEntity];
#endif
#ifdef USE_TEXT_SENSOR
        buf[TextSensorEntity] = (uint8_t) App.get_text_sensors().size();
        buf[AllEntities] += buf[TextSensorEntity];
#endif
        mMeshmesh->commandReply(rep, LastEntity + 1);
        err = 0;
      }
      break;

    case CMD_ENTITY_HASH_REQ:
      if (len == 3) {
        uint8_t service = buf[1];
        uint8_t index = buf[2];
        uint16_t hash = 0;
        std::string info;
        bool hashfound = false;

        switch (service) {
          case SensorEntity:
#ifdef USE_SENSOR
            if (index < App.get_sensors().size()) {
              auto sensor = App.get_sensors()[index];
              hash = sensor->get_object_id_hash() & 0xFFFF;
              info = sensor->get_name() + "," + sensor->get_object_id() + "," + sensor->get_unit_of_measurement();
              hashfound = true;
            }
#endif
            break;
          case BinarySensorEntity:
#ifdef USE_BINARY_SENSOR
            if (index < App.get_binary_sensors().size()) {
              auto binary = App.get_binary_sensors()[index];
              hash = binary->get_object_id_hash() & 0xFFFF;
              info = binary->get_name() + "," + binary->get_object_id();
              hashfound = true;
            }
#endif
            break;
          case SwitchEntity:
#ifdef USE_SWITCH
            if (index < App.get_switches().size()) {
              auto switch_ = App.get_switches()[index];
              hash = switch_->get_object_id_hash() & 0xFFFF;
              info = switch_->get_name() + "," + switch_->get_object_id();
              hashfound = true;
            }
#endif
            break;
          case LightEntity:
#ifdef USE_LIGHT
            if (index < App.get_lights().size()) {
              auto light = App.get_lights()[index];
              hash = light->get_object_id_hash() & 0xFFFF;
              info = light->get_name() + "," + light->get_object_id();
              hashfound = true;
            }
#endif
            break;
          case TextSensorEntity:
#ifdef USE_TEXT_SENSOR
            if (index < App.get_text_sensors().size()) {
              auto texts = App.get_text_sensors()[index];
              hash = texts->get_object_id_hash() & 0xFFFF;
              info = texts->get_name() + "," + texts->get_object_id();
              hashfound = true;
            }
#endif
            break;
          case AllEntities:
          case LastEntity:
            break;
        }

        if (hashfound) {
          auto rep = new uint8_t[info.length() + 3];
          rep[0] = CMD_ENTITY_HASH_REP;
          uint16toBuffer(rep + 1, hash);
          os_memcpy(rep + 3, info.c_str(), info.length());
          mMeshmesh->commandReply(rep, 3 + info.length());
          err = 0;
          delete rep;
        } else {
          uint8_t rep[5];
          rep[0] = CMD_ENTITY_HASH_REP;
          rep[1] = 0;
          rep[2] = 0;
          rep[3] = 'E';
          rep[4] = '!';
          mMeshmesh->commandReply(rep, 5);
          err = 0;
        }
      }
      break;

    case CMD_GET_ENTITY_STATE_REQ:
      if (len == 4) {
        EnityType type = (EnityType) buf[1];
        uint16_t hash = uint16FromBuffer(buf + 2);
        ESP_LOGD(TAG, "CMD_GET_ENTITY_STATE_REQ %04X hash %d type", hash, type);
        int16_t value = 0;
        std::string value_str;
        uint8_t value_type = 0;

        switch (type) {
          case SensorEntity: {
#ifdef USE_SENSOR
            sensor::Sensor *sensor = findSensor(hash);
            value = (int16_t) (sensor->state * 10.0);
            value_type = 1;
#endif
          } break;
          case BinarySensorEntity: {
#ifdef USE_BINARY_SENSOR
            binary_sensor::BinarySensor *binary = findBinarySensor(hash);
            value = binary->state ? 10 : 0;
            value_type = 1;
#endif
          } break;
          case SwitchEntity: {
#ifdef USE_SWITCH
            auto switch_ = findSwitch(hash);
            value = switch_->state ? 1 : 0;
            value_type = 1;
#endif
          } break;
          case LightEntity: {
#ifdef USE_LIGHT
            light::LightState *state = findLightState(hash);
            if (state->current_values.get_state() == 0)
              value = 0;
            else
              value = (uint16_t) (state->current_values.get_brightness() * 1024.0);
            value_type = 1;
#endif
          } break;
          case TextSensorEntity: {
#ifdef USE_TEXT_SENSOR
            text_sensor::TextSensor *texts = findTextSensor(hash);
            value_str = texts->state;
            value_type = 2;
#endif
          } break;
          case AllEntities:
          case LastEntity:
            break;
        }

        if (value_type == 1) {
          uint8_t rep[3];
          rep[0] = CMD_GET_ENTITY_STATE_REP;
          uint16toBuffer(rep + 1, value);
          mMeshmesh->commandReply(rep, 3);
          err = 0;
        } else if (value_type == 2) {
          uint16_t rep_size = 2 + value_str.length();
          auto *rep = new uint8_t[rep_size];
          rep[0] = CMD_GET_ENTITY_STATE_REP;
          rep[1] = value_type;
          os_memcpy(rep + 2, value_str.data(), value_str.length());
          mMeshmesh->commandReply(rep, rep_size);
          err = 0;
        }
      }
      break;
    case CMD_SET_ENTITY_STATE_REQ:
      if (len == 6) {
        EnityType type = (EnityType) buf[1];
        uint16_t hash = uint16FromBuffer(buf + 2);
        uint16_t value = uint16FromBuffer(buf + 4);
        ESP_LOGD(TAG, "CMD_SET_ENTITY_STATE_REQ type %d hash %04X value %d", type, hash, value);
        bool valuefound = 0;

        switch (type) {
          case LightEntity: {
#ifdef USE_LIGHT
            light::LightState *state = findLightState(hash);
            if (state != nullptr) {
              auto call = state->make_call();
              call.set_state(value > 0 ? 1 : 0);
              call.set_brightness((float) value / 1024.0f);
              call.perform();
              valuefound = 1;
            }
#endif
          } break;
          case SwitchEntity: {
#ifdef USE_SWITCH
            auto *state = findSwitch(hash);
            if (state != nullptr) {
              if (value)
                state->turn_on();
              else
                state->turn_off();
              valuefound = 1;
            }
#endif
          } break;
          case SensorEntity:
          case BinarySensorEntity:
          case TextSensorEntity:
            break;
          case AllEntities:
          case LastEntity:
            break;
        }

        if (valuefound) {
          uint8_t rep[1];
          rep[0] = CMD_SET_ENTITY_STATE_REP;
          mMeshmesh->commandReply(rep, 1);
          err = 0;
        }
      }
      break;

    case CMD_PUB_ENTITY_STATE_REQ:
      if (len == 6) {
        EnityType type = (EnityType) buf[1];
        uint16_t hash = uint16FromBuffer(buf + 2);
        uint16_t value = uint16FromBuffer(buf + 4);
        ESP_LOGD(TAG, "CMD_PUB_ENTITY_STATE_REQ type %d hash %04X value %d", type, hash, value);

        switch (type) {
          case LightEntity: {
#ifdef USE_LIGHT
            // TODO: Implement light publish
#endif
          } break;
          case SwitchEntity: {
#ifdef USE_SWITCH
            auto *state = findSwitch(hash);
            if (state != nullptr) {
              state->publish_state(value > 0 ? true : false);
            }
#endif
          } break;
          case SensorEntity:
          case BinarySensorEntity:
          case TextSensorEntity:
            break;
          case AllEntities:
          case LastEntity:
            break;
        }

        uint8_t rep[1];
        rep[0] = CMD_PUB_ENTITY_STATE_REP;
        mMeshmesh->commandReply(rep, 1);
        err = 0;
      }
      break;

    case CMD_ENTITY_REQ:  // 48
      if (len > 1) {
        ESP_LOGD(TAG, "CMD_ENTITY_REQ len %d", len);
        err = Entities::handle_frame(buf + 1, len - 1, mMeshmesh);
      }
      break;
    default:
        err = -1;
        break;
    }

    // mMeshmesh->commandReply(data, len);
    return err;
}

#ifdef USE_SENSOR
sensor::Sensor *MeshMeshDirectComponent::findSensorByUnit(const std::string &unit) {
  sensor::Sensor *result = nullptr;
  auto sensors = App.get_sensors();
  for (auto sensor : sensors) {
    result = sensor;
    if (unit == sensor->get_unit_of_measurement()) {
      result = sensor;
      break;
    }
  }
  return result;
}

sensor::Sensor *MeshMeshDirectComponent::findSensor(uint16_t hash) {
  sensor::Sensor *result = nullptr;
  auto sensors = App.get_sensors();
  for (auto sensor : sensors) {
    result = sensor;
    if (hash == (sensor->get_object_id_hash() & 0xFFFF)) {
      result = sensor;
      break;
    }
  }
  return result;
}
#endif

#ifdef USE_BINARY_SENSOR

binary_sensor::BinarySensor *MeshMeshDirectComponent::findBinarySensor(uint16_t hash) {
  binary_sensor::BinarySensor *result = nullptr;
  auto sensors = App.get_binary_sensors();
  for (auto sensor : sensors) {
    if (hash == (sensor->get_object_id_hash() & 0xFFFF)) {
      result = sensor;
      break;
    }
  }
  return result;
}
#endif

#ifdef USE_LIGHT
light::LightState *MeshMeshDirectComponent::findLightState(uint16_t hash) {
  light::LightState *result = nullptr;
  auto lights = App.get_lights();
  for (auto light : lights) {
    if (hash == (light->get_object_id_hash() & 0xFFFF)) {
      result = light;
      break;
    }
  }
  return result;
}
#endif

#ifdef USE_TEXT_SENSOR
text_sensor::TextSensor *MeshMeshDirectComponent::findTextSensor(uint16_t hash) {
  text_sensor::TextSensor *result = nullptr;
  auto sensors = App.get_text_sensors();
  for (auto sensor : sensors) {
    if (hash == (sensor->get_object_id_hash() & 0xFFFF)) {
      result = sensor;
      break;
    }
  }
  return result;
}
#endif

#ifdef USE_SWITCH
switch_::Switch *MeshMeshDirectComponent::findSwitch(uint16_t hash) {
  switch_::Switch *result = nullptr;
  auto switches = App.get_switches();
  for (auto switch_ : switches) {
    if (hash == (switch_->get_object_id_hash() & 0xFFFF)) {
      result = switch_;
      break;
    }
  }
  return result;
}

void MeshMeshDirectComponent::publishRemoteSwitchState(uint32_t addr, uint16_t hash, bool state) {
  uint8_t buff[6];
  buff[0] = CMD_PUB_ENTITY_STATE_REQ;
  buff[1] = SwitchEntity;
  uint16toBuffer(buff+2, hash);
  uint16toBuffer(buff+4, state ? 10 : 0);
  mMeshmesh->uniCastSendData(buff, 6, addr);
}
#endif

MeshMeshDirectComponent::EnityType MeshMeshDirectComponent::findEntityTypeByHash(uint16_t hash) {
#ifdef USE_SENSOR
  if (findSensor(hash) != nullptr)
    return SensorEntity;
#endif
#ifdef USE_BINARY_SENSOR
  if (findBinarySensor(hash) != nullptr)
    return BinarySensorEntity;
#endif
#ifdef USE_SWITCH
    // if(findSwitch(hash) != nullptr) return SwitchEntity;
#endif
#ifdef USE_LIGHT
  if (findLightState(hash) != nullptr)
    return LightEntity;
#endif
  return AllEntities;
}


}  // namespace meshmesh
}  // namespace esphome
