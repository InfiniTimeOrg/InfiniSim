#include "components/ble/ImmediateAlertClient.h"
#include <cstring>
#include <nrf_log.h>
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid16_t ImmediateAlertClient::immediateAlertClientUuid;
constexpr ble_uuid16_t ImmediateAlertClient::alertLevelCharacteristicUuid;

ImmediateAlertClient::ImmediateAlertClient(Pinetime::System::SystemTask& systemTask)
  : systemTask {systemTask} {
}

void ImmediateAlertClient::Init() {
}

void ImmediateAlertClient::Discover(uint16_t connectionHandle, std::function<void(uint16_t)> onServiceDiscovered) {
  NRF_LOG_INFO("[IAS] Starting discovery");
  this->onServiceDiscovered = onServiceDiscovered;
  // ble_gattc_disc_svc_by_uuid(connectionHandle, &immediateAlertClientUuid.u, OnDiscoveryEventCallback, this);
}

bool ImmediateAlertClient::SendImmediateAlert(ImmediateAlertClient::Levels level) {
  return false;
}
