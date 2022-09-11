#include <Arduino.h>
#include "esp_log.h"
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

#include "test_digit_gpio1819.h"

static const char *TAG = "test_digit_gpio1819.cpp";

static const uint16_t RECV_PIN = 19; // "IN"  WHITE
static const uint16_t SEND_PIN = 18; // "OUT" YELLOW
static const uint16_t CAPTURE_BUFFER_SIZE = 1024;
static const uint8_t TIMEOUT = 50;

// DIGITAL
//  M5STACK UNIT IR
//  https://docs.m5stack.com/en/unit/ir
IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);

decode_results results;

void init_digit_gpio1819(void)
{
    irrecv.enableIRIn();
}

void get_digit_unitir_data(void)
{
    if (irrecv.decode(&results)) {
        // Display the basic output of what we found.
        ESP_LOGI(TAG, "%s", resultToHumanReadableBasic(&results).c_str());

        // Display any extra A/C info if we have it.
        String description = IRAcUtils::resultAcToString(&results);
        if (description.length()) {
            ESP_LOGI(TAG, "%s", (D_STR_MESGDESC ": " + description).c_str());
        }
        yield();

        ESP_LOGI(TAG, "%s", resultToSourceCode(&results).c_str());
        yield();
    }
}
