#include "Spi.h"

using namespace Pinetime::Drivers::Infinisim;

Spi::Spi(Pinetime::Drivers::Infinisim::SpiMaster& spiMaster, uint8_t pinCsn) : spiMaster {spiMaster}, pinCsn {pinCsn} {

}

bool Spi::Write(const uint8_t* data, size_t size) {
  return spiMaster.Write(pinCsn, data, size);
}

bool Spi::Read(uint8_t* cmd, size_t cmdSize, uint8_t* data, size_t dataSize) {
  return spiMaster.Read(pinCsn, cmd, cmdSize, data, dataSize);
}

void Spi::Sleep() {

}

bool Spi::WriteCmdAndBuffer(const uint8_t* cmd, size_t cmdSize, const uint8_t* data, size_t dataSize) {
  return spiMaster.WriteCmdAndBuffer(pinCsn, cmd, cmdSize, data, dataSize);
}

bool Spi::Init() {

  return true;
}

void Spi::Wakeup() {

}
