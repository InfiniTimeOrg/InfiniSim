#include "drivers/SpiNorFlash.h"
#include <hal/nrf_gpio.h>
#include <libraries/log/nrf_log.h>
#include "drivers/Spi.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

using namespace Pinetime::Drivers;

SpiNorFlash::SpiNorFlash(const std::string& memoryFilePath) : memoryFilePath{memoryFilePath} {
  namespace fs = std::filesystem;
  fs::path f{ memoryFilePath };
  if (fs::exists(f)) {
    memoryFile = std::fstream(memoryFilePath, std::ios::binary | std::fstream::in | std::fstream::out);
  } else {
    memoryFile = std::fstream(memoryFilePath, std::ios::trunc | std::ios::binary | std::fstream::in | std::fstream::out);
    memoryFile.seekp(memorySize - 1);
    memoryFile.write("", 1);
  }
}
SpiNorFlash::~SpiNorFlash() {
  if (memoryFile.is_open()) {
    memoryFile.close();
  }
}

void SpiNorFlash::Init() {
  device_id = ReadIdentificaion();
  NRF_LOG_INFO(
    "[SpiNorFlash] Manufacturer : %d, Memory type : %d, memory density : %d", device_id.manufacturer, device_id.type, device_id.density);
}

void SpiNorFlash::Uninit() {
}

void SpiNorFlash::Sleep() {
  NRF_LOG_INFO("[SpiNorFlash] Sleep")
}

void SpiNorFlash::Wakeup() {
  NRF_LOG_INFO("[SpiNorFlash] Wakeup")
}

SpiNorFlash::Identification SpiNorFlash::ReadIdentificaion() {
  return {};
}

uint8_t SpiNorFlash::ReadStatusRegister() {
  return 0;
}

bool SpiNorFlash::WriteInProgress() {
  return false;
}

bool SpiNorFlash::WriteEnabled() {
  return false;
}

uint8_t SpiNorFlash::ReadConfigurationRegister() {
  return 0;
}

void SpiNorFlash::Read(uint32_t address, uint8_t* buffer, size_t size) {
  static_assert(sizeof(uint8_t) == sizeof(char));
  if (address + size * sizeof(uint8_t) > memorySize) {
    throw std::runtime_error("SpiNorFlash::Read out of bounds");
  }
  memoryFile.seekp(address);
  memoryFile.read(reinterpret_cast<char *>(buffer), size);
}

void SpiNorFlash::WriteEnable() {

}

void SpiNorFlash::SectorErase(uint32_t sectorAddress) {

}

uint8_t SpiNorFlash::ReadSecurityRegister() {
  return 0;
}

bool SpiNorFlash::ProgramFailed() {
  return false;
}

bool SpiNorFlash::EraseFailed() {
  return false;
}

void SpiNorFlash::Write(uint32_t address, const uint8_t* buffer, size_t size) {
  if (address + size * sizeof(uint8_t) > memorySize) {
    throw std::runtime_error("SpiNorFlash::Write out of bounds");
  }
  memoryFile.seekp(address);
  memoryFile.write(reinterpret_cast<const char *>(buffer), size);
  memoryFile.flush();
}

SpiNorFlash::Identification SpiNorFlash::GetIdentification() const {
  return device_id;
}
