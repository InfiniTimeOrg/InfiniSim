#include "components/ble/SimpleWeatherService.h"

#include <algorithm>
#include <cstring>
#include <nrf_log.h>

using namespace Pinetime::Controllers;

namespace {
enum class MessageType : uint8_t { CurrentWeather, Forecast, Unknown };

uint64_t ToUInt64(const uint8_t* data) {
  return *(reinterpret_cast<const uint64_t*>(data));
}

SimpleWeatherService::CurrentWeather CreateCurrentWeather(const uint8_t* dataBuffer) {
  char cityName[33];
  std::memcpy(&cityName[0], &dataBuffer[13], 32);
  cityName[32] = '\0';
  return SimpleWeatherService::CurrentWeather {ToUInt64(&dataBuffer[2]),
                                              dataBuffer[10],
                                              dataBuffer[11],
                                              dataBuffer[12],
                                              dataBuffer[13 + 32],
                                              cityName};
}

SimpleWeatherService::Forecast CreateForecast(const uint8_t* dataBuffer) {
  auto timestamp = static_cast<uint64_t>(ToUInt64(&dataBuffer[2]));

  std::array<SimpleWeatherService::Forecast::Day, SimpleWeatherService::MaxNbForecastDays> days;
  const uint8_t nbDaysInBuffer = dataBuffer[10];
  const uint8_t nbDays = std::min(SimpleWeatherService::MaxNbForecastDays, nbDaysInBuffer);
  for (int i = 0; i < nbDays; i++) {
    days[i] = SimpleWeatherService::Forecast::Day {dataBuffer[11 + (i * 3)], dataBuffer[12 + (i * 3)], dataBuffer[13 + (i * 3)]};
  }
  return SimpleWeatherService::Forecast {timestamp, nbDays, days};
}

MessageType GetMessageType(const uint8_t* data) {
  auto messageType = static_cast<MessageType>(*data);
  if (messageType > MessageType::Unknown) {
    return MessageType::Unknown;
  }
  return messageType;
}

uint8_t GetVersion(const uint8_t* dataBuffer) {
  return dataBuffer[1];
}
}

int WeatherCallback(uint16_t /*connHandle*/, uint16_t /*attrHandle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  return static_cast<Pinetime::Controllers::SimpleWeatherService*>(arg)->OnCommand(ctxt);
}

SimpleWeatherService::SimpleWeatherService(const DateTime& dateTimeController) : dateTimeController(dateTimeController) {
}

void SimpleWeatherService::Init() {
  //ble_gatts_count_cfg(serviceDefinition);
  //ble_gatts_add_svcs(serviceDefinition);
}

int SimpleWeatherService::OnCommand(struct ble_gatt_access_ctxt* ctxt) {

  return 0;
}

std::optional<SimpleWeatherService::CurrentWeather> SimpleWeatherService::Current() const {
  if (currentWeather) {
    auto currentTime = dateTimeController.UTCDateTime().time_since_epoch();
    auto weatherTpSecond = std::chrono::seconds {currentWeather->timestamp};
    auto weatherTp = std::chrono::duration_cast<std::chrono::seconds>(weatherTpSecond);
    auto delta = currentTime - weatherTp;

    if (delta < std::chrono::hours {24}) {
      return currentWeather;
    }
  }
  return {};
}

std::optional<SimpleWeatherService::Forecast> SimpleWeatherService::GetForecast() const {
  if (forecast) {
    auto currentTime = dateTimeController.UTCDateTime().time_since_epoch();
    auto weatherTpSecond = std::chrono::seconds {forecast->timestamp};
    auto weatherTp = std::chrono::duration_cast<std::chrono::seconds>(weatherTpSecond);
    auto delta = currentTime - weatherTp;

    if (delta < std::chrono::hours {24}) {
      return this->forecast;
    }
  }
  return {};
}

bool SimpleWeatherService::CurrentWeather::operator==(const SimpleWeatherService::CurrentWeather& other) const {
  return this->iconId == other.iconId && this->temperature == other.temperature && this->timestamp == other.timestamp &&
         this->maxTemperature == other.maxTemperature && this->minTemperature == other.maxTemperature;
}
