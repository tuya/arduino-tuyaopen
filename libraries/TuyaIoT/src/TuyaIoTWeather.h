#ifndef __TUYA_IOT_WEATHER_H__
#define __TUYA_IOT_WEATHER_H__

/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#include <Arduino.h>

#include <vector>

#include "ArduinoTuyaIoTClient.h"
// tuya open sdk
#include "tuya_iot.h"

/******************************************************************************
 * CONSTANTS
 ******************************************************************************/
// Weather data
// https://developer.tuya.com/en/docs/mcu-standard-protocol/mcusdk-wifi-weather?id=Kd2fvzw7ny80s#title-14-Appendix%204%3A%20Weather%20data%20in%20UTF-8%20encoding
#define TW_WEATHER_SUNNY                    (120)
#define TW_WEATHER_HEAVY_RAIN               (101)
#define TW_WEATHER_THUNDERSTORM             (102)
#define TW_WEATHER_SANDSTORM                (103)
#define TW_WEATHER_LIGHT_SNOW               (104)
#define TW_WEATHER_SNOW                     (105)
#define TW_WEATHER_FREEZING_FOG             (106)
#define TW_WEATHER_RAINSTORM                (107)
#define TW_WEATHER_ISOLATED_SHOWER          (108)
#define TW_WEATHER_DUST                     (109)
#define TW_WEATHER_THUNDER_AND_LIGHTNING    (110)
#define TW_WEATHER_LIGHT_SHOWER             (111)
#define TW_WEATHER_RAIN                     (112)
#define TW_WEATHER_SLEET                    (113)
#define TW_WEATHER_DUST_DEVIL               (114)
#define TW_WEATHER_ICE_PELLETS              (115)
#define TW_WEATHER_STRONG_SANDSTORM         (116)
#define TW_WEATHER_SAND_BLOWING             (117)
#define TW_WEATHER_LIGHT_TO_MODERATE_RAIN   (118)
#define TW_WEATHER_MOSTLY_CLEAR             (119)
#define TW_WEATHER_FOG                      (121)
#define TW_WEATHER_SHOWER                   (122)
#define TW_WEATHER_HEAVY_SHOWER             (123)
#define TW_WEATHER_HEAVY_SNOW               (124)
#define TW_WEATHER_EXTREME_RAINSTORM        (125)
#define TW_WEATHER_BLIZZARD                 (126)
#define TW_WEATHER_HAIL                     (127)
#define TW_WEATHER_LIGHT_TO_MODERATE_SNOW   (128)
#define TW_WEATHER_PARTLY_CLOUDY            (129)
#define TW_WEATHER_LIGHT_SNOW_SHOWER        (130)
#define TW_WEATHER_MODERATE_SNOW            (131)
#define TW_WEATHER_OVERCAST                 (132)
#define TW_WEATHER_NEEDLE_ICE               (133)
#define TW_WEATHER_DOWNPOUR                 (134)
#define TW_WEATHER_THUNDERSHOWER_AND_HAIL   (136)
#define TW_WEATHER_FREEZING_RAIN            (137)
#define TW_WEATHER_SNOW_SHOWER              (138)
#define TW_WEATHER_LIGHT_RAIN               (139)
#define TW_WEATHER_HAZE                     (140)
#define TW_WEATHER_MODERATE_RAIN            (141)
#define TW_WEATHER_CLOUDY                   (142)
#define TW_WEATHER_THUNDERSHOWER            (143)
#define TW_WEATHER_MODERATE_TO_HEAVY_RAIN   (144)
#define TW_WEATHER_HEAVY_RAIN_TO_RAINSTORM  (145)
#define TW_WEATHER_CLEAR                    (146)

// Wind direction code
// https://developer.tuya.com/en/docs/mcu-standard-protocol/mcusdk-wifi-weather?id=Kd2fvzw7ny80s#title-15-Appendix%205%3A%20Wind%20direction%20code

/******************************************************************************
 * TYPEDEF
 ******************************************************************************/


/******************************************************************************
 * CLASS DECLARATION
 ******************************************************************************/
class TuyaIoTWeatherClass {
public:
  TuyaIoTWeatherClass() {};
  ~TuyaIoTWeatherClass() {};

  // Get current weather conditions
  // mbar: atmospheric pressure,  1 mbar = 100 pa = 1 hPa.
  int getCurrentConditions(int& weather, int &temp, int &humi, int &realFeel, int &mbar, int &uvi);

  // Get today's high and low temperatures
  int getTodayHighLowTemp(int &highTemp, int &lowTemp);

  // Get current wind
  int getCurrentWind(String &windDir, String &windSpeed);
  // Get current wind in China, windLevel only supports Mainland China
  int getCurrentWindCN(String &windDir, String &windSpeed, int &windLevel);

  // Get current sunrise and sunset time
  int getCurrentSunriseSunsetGMT(String &sunrise, String &sunset); // GMT time
  int getCurrentSunriseSunsetLocal(String &sunrise, String &sunset); // Local time (with timezone)

  int getCurrentSunriseSunset(String &sunrise, String &sunset) {
    return getCurrentSunriseSunsetLocal(sunrise, sunset);
  }

  // Get current AQI
  int getCurrentAQI(int &aqi, int &qualityLevel, int &pm25, int &pm10, int &o3, int &no2, int &co, int &so2);
  int getCurrentAQICN(int &aqi, String &rank, int &qualityLevel, int &pm25, int &pm10, int &o3, int &no2, int &co, int &so2); // rank only supports Mainland China

  // Get forecast weather conditions
  int getForecastConditions(int number, std::vector<int>& weather, std::vector<int>& temp, std::vector<int>& humi, std::vector<int>& uvi, std::vector<int>& mbar);
  int getForecastConditionsCN(int number, std::vector<int>& weather, std::vector<int>& humi, std::vector<int>& uvi); // Temp, pressure not support forecast in Mainland China

  int getForecastWind(int number, std::vector<String>& windDir, std::vector<String>& windSpeed);

  int getForecastHighLowTemp(int number, std::vector<int>& highTemp, std::vector<int>& lowTemp);

  // Get city information
  /*
    If the weather service API returns service city only without any content,
    it indicates the device does not provide its latitude and longitude, so the request failed.
    To resolve this issue,
    you should guide users to allow their mobile app to access the location and pair the device with the mobile app again.
  */
  int getCity(String &province, String &city, String &area);

  // need atop_base_response_free(atop_base_response_t *response); after use
  int get(const char *code, atop_base_response_t *response);

private:
  tuya_iot_client_t *_clientHandle = &ArduinoIoTClient;

  bool _allowUpdate(void);
};

#endif // !__TUYA_IOT_WEATHER_H__
