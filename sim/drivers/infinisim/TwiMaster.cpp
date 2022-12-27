#include "TwiMaster.h"
#include <cstring>

using namespace Pinetime::Drivers::Infinisim;

TwiMaster::TwiMaster() {
}

void TwiMaster::Init() {

}

TwiMaster::ErrorCodes TwiMaster::Read(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* data, size_t size) {
  return TwiMaster::ErrorCodes::NoError;
}

TwiMaster::ErrorCodes TwiMaster::Write(uint8_t deviceAddress, uint8_t registerAddress, const uint8_t* data, size_t size) {
  return TwiMaster::ErrorCodes::NoError;
}

void TwiMaster::Sleep() {

}

void TwiMaster::Wakeup() {

}
