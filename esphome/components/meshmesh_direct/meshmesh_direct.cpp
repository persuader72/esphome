#include "meshmesh_direct.h"
#include "esphome/core/log.h"
#include <functional>

static const char *TAG = "meshmesh_direct";

namespace esphome {
namespace meshmesh {

MeshMeshDirectComponent::MeshMeshDirectComponent() : Component() {}

void MeshMeshDirectComponent::setup() {
    ESP_LOGE(TAG, "Setting up MeshMeshDirectComponent");
    mMeshmesh = MeshmeshComponent::getInstance();
    mMeshmesh->addHandleFrameCb(std::bind(&MeshMeshDirectComponent::handleFrame, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void MeshMeshDirectComponent::loop() {
    ESP_LOGVV(TAG, "Looping MeshMeshDirectComponent");
}

int8_t MeshMeshDirectComponent::handleFrame(uint8_t *data, uint16_t len, uint32_t from) {
    ESP_LOGE(TAG, "Handling frame from %06X with length %d", from, len);

    switch (data[0]) {
      default:
        break;
    }

    // mMeshmesh->commandReply(data, len);
    return -1;
}

}  // namespace meshmesh
}  // namespace esphome
