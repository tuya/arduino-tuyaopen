/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#include "TuyaIoTWeather.h"

// tuya open sdk
#include "tal_time_service.h"
#include "tal_memory.h"
#include "tal_log.h"
#include "atop_base.h"
#include "cJSON.h"

/******************************************************************************
 * CONSTANT
 ******************************************************************************/
#define ENABLE_WEATHER_DEBUG      1

#define WEATHER_API              "thing.weather.get"
#define API_VERSION              "1.0"

/******************************************************************************
 * TYPEDEF DEFINE
 ******************************************************************************/


/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/


/******************************************************************************
 * LOCAL MODULE FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * CTOR/DTOR
 ******************************************************************************/


/******************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/
int TuyaIoTWeatherClass::get(const char* code, atop_base_response_t *response)
{
  int rt = OPRT_OK;
  TIME_T timestamp = 0;
  char *postData = NULL;
  int postDataLen = 0;
  atop_base_request_t atopRequest;

  rt = tal_time_check_time_sync();
  if (OPRT_OK != rt) {
    PR_ERR("tal_time_check_time_sync error:%d", rt);
    return rt;
  }

  // network check
  if (!_clientHandle->config.network_check) {
    PR_ERR("network_check is NULL");
    return OPRT_COM_ERROR;
  }
  if (!_clientHandle->config.network_check()) {
    PR_ERR("network is not connected");
    return OPRT_COM_ERROR;
  }

  timestamp = tal_time_get_posix();

  postDataLen = snprintf(NULL, 0, "{\"codes\":[%s], \"t\":%d}", code, timestamp);
  postDataLen++; // add '\0'

  postData = (char *)tal_malloc(postDataLen);
  if (NULL == postData) {
    return OPRT_MALLOC_FAILED;
  }
  memset(postData, 0, postDataLen);

  snprintf(postData, postDataLen, "{\"codes\":[%s], \"t\":%d}", code, timestamp);
  PR_DEBUG("Post: %s", postData);

  memset(&atopRequest, 0, sizeof(atop_base_request_t));
  atopRequest.devid = _clientHandle->activate.devid;
  atopRequest.key  = _clientHandle->activate.seckey;
  atopRequest.path = "/d.json";
  atopRequest.timestamp = timestamp;
  atopRequest.api = WEATHER_API;
  atopRequest.version = API_VERSION;
  atopRequest.data =  reinterpret_cast<void *>(postData);
  atopRequest.datalen = strlen(postData);

  rt = atop_base_request(&atopRequest, response);
  if (OPRT_OK != rt) {
    PR_ERR("atop_base_request error:%d", rt);
  }

  tal_free(postData);
  postData = NULL;

  return rt;
}

// Today weather
int TuyaIoTWeatherClass::getCurrentConditions(int& weather, int &temp, int &humi, int &realFeel, int &mbar, int &uvi)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.conditionNum\",\"w.temp\",\"w.humidity\",\"w.realFeel\",\"w.pressure\",\"w.uvi\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get current conditions error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.conditionNum");
    if (item) {
      String weatherStr = item->valuestring;
      weather = weatherStr.toInt();
    }

    item = cJSON_GetObjectItem(dataObj, "w.temp");
    if (item) {
      temp = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.humidity");
    if (item) {
      humi = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.realFeel");
    if (item) {
      realFeel = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.pressure");
    if (item) {
      mbar = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.uvi");
    if (item) {
      uvi = item->valueint;
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getTodayHighLowTemp(int &highTemp, int &lowTemp)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.thigh\",\"w.tlow\",\"w.date.1\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.thigh.0");
    if (item) {
      highTemp = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.tlow.0");
    if (item) {
      lowTemp = item->valueint;
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentWind(String &windDir, String &windSpeed)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.windDir\",\"w.windSpeed\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.windDir");
    if (item) {
      windDir = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.windSpeed");
    if (item) {
      windSpeed = String(item->valuestring);
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentWindCN(String &windDir, String &windSpeed, int &windLevel)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.windDir\",\"w.windSpeed\",\"w.windLevel\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.windDir");
    if (item) {
      windDir = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.windSpeed");
    if (item) {
      windSpeed = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.windLevel");
    if (item) {
      windLevel = item->valueint;
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentSunriseSunsetGMT(String &sunrise, String &sunset)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.sunrise\",\"w.sunset\",\"t.unix\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.sunrise");
    if (item) {
      sunrise = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.sunset");
    if (item) {
      sunset = String(item->valuestring);
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentSunriseSunsetLocal(String &sunrise, String &sunset)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.sunrise\",\"w.sunset\",\"t.local\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.sunrise");
    if (item) {
      sunrise = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.sunset");
    if (item) {
      sunset = String(item->valuestring);
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentAQI(int &aqi, int &qualityLevel, int &pm25, int &pm10, int &o3, int &no2, int &co, int &so2)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.aqi\",\"w.qualityLevel\",\"w.pm25\",\"w.pm10\",\"w.o3\",\"w.no2\",\"w.co\",\"w.so2\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.aqi");
    if (item) {
      aqi = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.qualityLevel");
    if (item) {
      qualityLevel = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.pm25");
    if (item) {
      pm25 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.pm10");
    if (item) {
      pm10 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.o3");
    if (item) {
      o3 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.no2");
    if (item) {
      no2 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.co");
    if (item) {
      co = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.so2");
    if (item) {
      so2 = item->valueint;
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCurrentAQICN(int &aqi, String &rank, int &qualityLevel, int &pm25, int &pm10, int &o3, int &no2, int &co, int &so2)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"w.aqi\",\"w.rank\",\"w.qualityLevel\",\"w.pm25\",\"w.pm10\",\"w.o3\",\"w.no2\",\"w.co\",\"w.so2\",\"w.currdate\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "w.aqi");
    if (item) {
      aqi = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.rank");
    if (item) {
      rank = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "w.qualityLevel");
    if (item) {
      qualityLevel = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.pm25");
    if (item) {
      pm25 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.pm10");
    if (item) {
      pm10 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.o3");
    if (item) {
      o3 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.no2");
    if (item) {
      no2 = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.co");
    if (item) {
      co = item->valueint;
    }

    item = cJSON_GetObjectItem(dataObj, "w.so2");
    if (item) {
      so2 = item->valueint;
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getForecastConditions(int number, std::vector<int>& weather, std::vector<int>& temp, std::vector<int>& humi, std::vector<int>& uvi, std::vector<int>& mbar)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  weather.clear();
  temp.clear();
  humi.clear();
  uvi.clear();
  mbar.clear();

  if (number < 1 || number > 7) {
    return OPRT_INVALID_PARM;
  }

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  char requestCode[80] = {0};
  snprintf(requestCode, sizeof(requestCode), "\"w.conditionNum\",\"w.humidity\",\"w.temp\",\"w.uvi\",\"w.pressure\",\"w.date.%d\"", number);

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    for (int i = 0; i < number; i++) {
      item = cJSON_GetObjectItem(dataObj, String("w.conditionNum." + String(i)).c_str());
      if (item) {
        String weatherStr = item->valuestring;
        weather.push_back(weatherStr.toInt());
      }

      item = cJSON_GetObjectItem(dataObj, String("w.temp." + String(i)).c_str());
      if (item) {
        temp.push_back(item->valueint);
      } else {
        temp.push_back(0); // Not support forecast temperature in Mainland China
      }

      item = cJSON_GetObjectItem(dataObj, String("w.pressure." + String(i)).c_str());
      if (item) {
        mbar.push_back(item->valueint);
      } else {
        mbar.push_back(0); // Not support forecast atmospheric pressure in Mainland China
      }

      item = cJSON_GetObjectItem(dataObj, String("w.humidity." + String(i)).c_str());
      if (item) {
        humi.push_back(item->valueint);
      }

      item = cJSON_GetObjectItem(dataObj, String("w.uvi." + String(i)).c_str());
      if (item) {
        uvi.push_back(item->valueint);
      }
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getForecastConditionsCN(int number, std::vector<int>& weather, std::vector<int>& humi, std::vector<int>& uvi)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  weather.clear();
  humi.clear();
  uvi.clear();

  if (number < 1 || number > 7) {
    return OPRT_INVALID_PARM;
  }

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  char requestCode[80] = {0};
  snprintf(requestCode, sizeof(requestCode), "\"w.conditionNum\",\"w.humidity\",\"w.uvi\",\"w.date.%d\"", number);

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    for (int i = 0; i < number; i++) {
      item = cJSON_GetObjectItem(dataObj, String("w.conditionNum." + String(i)).c_str());
      if (item) {
        String weatherStr = item->valuestring;
        weather.push_back(weatherStr.toInt());
      }

      item = cJSON_GetObjectItem(dataObj, String("w.humidity." + String(i)).c_str());
      if (item) {
        humi.push_back(item->valueint);
      }

      item = cJSON_GetObjectItem(dataObj, String("w.uvi." + String(i)).c_str());
      if (item) {
        uvi.push_back(item->valueint);
      }
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getForecastWind(int number, std::vector<String>& windDir, std::vector<String>& windSpeed)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  windDir.clear();
  windSpeed.clear();

  if (number < 1 || number > 7) {
    return OPRT_INVALID_PARM;
  }

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  char requestCode[80] = {0};
  snprintf(requestCode, sizeof(requestCode), "\"w.windDir\",\"w.windSpeed\",\"w.date.%d\"", number);

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    for (int i = 0; i < number; i++) {
      item = cJSON_GetObjectItem(dataObj, String("w.windDir." + String(i)).c_str());
      if (item) {
        windDir.push_back(String(item->valuestring));
      }

      item = cJSON_GetObjectItem(dataObj, String("w.windSpeed." + String(i)).c_str());
      if (item) {
        windSpeed.push_back(String(item->valuestring));
      }
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getForecastHighLowTemp(int number, std::vector<int>& highTemp, std::vector<int>& lowTemp)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  highTemp.clear();
  lowTemp.clear();

  if (number < 1 || number > 7) {
    return OPRT_INVALID_PARM;
  }

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  char requestCode[80] = {0};
  snprintf(requestCode, sizeof(requestCode), "\"w.thigh\",\"w.tlow\",\"w.date.%d\"", number);

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    for (int i = 0; i < number; i++) {
      item = cJSON_GetObjectItem(dataObj, String("w.thigh." + String(i)).c_str());
      if (item) {
        highTemp.push_back(item->valueint);
      }

      item = cJSON_GetObjectItem(dataObj, String("w.tlow." + String(i)).c_str());
      if (item) {
        lowTemp.push_back(item->valueint);
      }
    }
  }

  atop_base_response_free(&response);

  return rt;
}

int TuyaIoTWeatherClass::getCity(String &province, String &city, String &area)
{
  int rt = OPRT_OK;
  atop_base_response_t response;

  if (!_allowUpdate()) {
    return OPRT_COM_ERROR;
  }

  const char* requestCode = "\"c.province\",\"c.city\",\"c.area\"";

  memset(&response, 0, sizeof(atop_base_response_t));

  rt = get(requestCode, &response);
  if (OPRT_OK != rt || !response.success) {
    PR_ERR("get today high low temp error:%d", rt);
    return OPRT_COM_ERROR;
  }

#if ENABLE_WEATHER_DEBUG
  char *result_value = cJSON_PrintUnformatted(response.result);
  PR_DEBUG("result: %s", result_value);
  cJSON_free(result_value);
#endif

  if (cJSON_HasObjectItem(response.result, "data")) {
    cJSON *item  = NULL;
    cJSON *dataObj = cJSON_GetObjectItem(response.result, "data");

    item = cJSON_GetObjectItem(dataObj, "c.province");
    if (item) {
      province = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "c.city");
    if (item) {
      city = String(item->valuestring);
    }

    item = cJSON_GetObjectItem(dataObj, "c.area");
    if (item) {
      area = String(item->valuestring);
    }
  }

  atop_base_response_free(&response);

  return rt;
}

/******************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 ******************************************************************************/
bool TuyaIoTWeatherClass::_allowUpdate(void)
{
  if (false == tuya_iot_activated(_clientHandle)) {
    return false;
  }

  // check network status
  if (_clientHandle->config.network_check && _clientHandle->config.network_check()) {
    return true;
  } else {
    return false;
  }
}
