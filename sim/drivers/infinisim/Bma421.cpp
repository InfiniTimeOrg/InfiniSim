#include "Bma421.h"
#include "sim/drivers/infinisim/TwiMaster.h"

using namespace Pinetime::Drivers::Infinisim::MotionSensors;

void Bma421::Init() {

}

void Bma421::Read(uint8_t registerAddress, uint8_t* buffer, size_t size) {

}

void Bma421::Write(uint8_t registerAddress, const uint8_t* data, size_t size) {

}

Pinetime::Drivers::MotionSensors::Values Bma421::Process() {
  return {steps, 0, 0, 0};
}

bool Bma421::IsOk() const {
  return true;
}

void Bma421::ResetStepCounter() {
  steps = 0;
}

void Bma421::SoftReset() {

}

Pinetime::Drivers::MotionSensors::DeviceTypes Bma421::DeviceType() const {
  return Pinetime::Drivers::MotionSensors::DeviceTypes::BMA425;
}
