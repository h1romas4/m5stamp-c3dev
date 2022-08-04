#include "Arduino.h"
#include "esp_log.h"
#include "WiFi.h"
#include "Preferences.h"

static const char *TAG = "test_nvs_wifi.cpp";

#define NTP_SERVER1 CONFIG_C3DEV_NTP_SERVER1
#define NTP_SERVER2 CONFIG_C3DEV_NTP_SERVER2
#define NTP_SERVER3 CONFIG_C3DEV_NTP_SERVER3

void sync_wifi_ntp(void)
{
    Preferences preferences;

    if(!preferences.begin("wifi", true)) return;

    String ssid = preferences.getString("ssid");
    String passwd = preferences.getString("passwd");

    ESP_LOGI(TAG, "Connect to %s", ssid);
    WiFi.begin(ssid.c_str(), passwd.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
    }
    ESP_LOGI(TAG, "Connected!");
    configTime(9 * 3600L, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
    // Wait Time Sync
    struct tm timeInfo;
    while(true) {
        getLocalTime(&timeInfo);
        if(timeInfo.tm_year > 70) {
            break;
        }
        delay(500);
        ESP_LOGI(TAG, "waiting time sync..(%d:%d:%d %d)", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, timeInfo.tm_year);
    }
    ESP_LOGI(TAG, "Configured time from NTP");
    WiFi.disconnect();
}
