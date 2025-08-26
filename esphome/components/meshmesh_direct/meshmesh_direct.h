#pragma once

#include "esphome/core/component.h"
#include "esphome/components/meshmesh/meshmesh.h"

namespace esphome {
namespace meshmesh {

class MeshMeshDirectComponent : public Component {
public:
  typedef enum { UNKNOW = 0, LUX, LAST_SENSOR_TYPE } SensorTypes;
  typedef enum { AllEntities = 0, SensorEntity, BinarySensorEntity, SwitchEntity, LightEntity,TextSensorEntity, LastEntity} EnityType;
public:
  static MeshMeshDirectComponent *singleton;
  static MeshMeshDirectComponent *getInstance();
public:
  MeshMeshDirectComponent();
  void setup();
  void loop() override;
public:
#ifdef USE_SWITCH
    void publishRemoteSwitchState(uint32_t addr, uint16_t hash, bool state);
#endif
private:
#ifdef USE_SENSOR
    sensor::Sensor *findSensorByUnit(const std::string &unit);
    sensor::Sensor *findSensor(uint16_t hash);
#endif
#ifdef USE_BINARY_SENSOR
    binary_sensor::BinarySensor *findBinarySensor(uint16_t hash);
#endif
#ifdef USE_LIGHT
    light::LightState *findLightState(uint16_t hash);
#endif
#ifdef USE_TEXT_SENSOR
    text_sensor::TextSensor *findTextSensor(uint16_t hash);
#endif
#ifdef USE_SWITCH
    switch_::Switch *findSwitch(uint16_t hash);
#endif
   private:
    EnityType findEntityTypeByHash(uint16_t hash);
private:
  int8_t handleFrame(uint8_t *data, uint16_t len, uint32_t from);
public:
  MeshmeshComponent *meshmesh() const { return mMeshmesh; }
private:
  MeshmeshComponent *mMeshmesh{nullptr};
};
}  // namespace meshmesh
}  // namespace esphome
