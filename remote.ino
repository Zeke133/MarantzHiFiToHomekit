#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <arduino_homekit_server.h>

#include "homekit-operation.h"

// Connections with NodeMCU board
// Label    Pin     Description
// D0       GPIO-16 Built-in led
// D1       GPIO-5  Remote control line
// D3       GPIO-0  Flash button

constexpr int BuiltInLedPin = 16; // D0

const char *ssid = "YourWiFiSsid";
const char *password = "WiFiPassword";

void setup()
{
    Serial.begin(115200);
    Serial.setRxBufferSize(32);
    Serial.setDebugOutput(false);

    pinMode(BuiltInLedPin, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.disconnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);

    printf("\n");
    printf("SketchSize: %d B\n", ESP.getSketchSize());
    printf("FreeSketchSpace: %d B\n", ESP.getFreeSketchSpace());
    printf("FlashChipSize: %d B\n", ESP.getFlashChipSize());
    printf("FlashChipRealSize: %d B\n", ESP.getFlashChipRealSize());
    printf("FlashChipSpeed: %d\n", ESP.getFlashChipSpeed());
    printf("SdkVersion: %s\n", ESP.getSdkVersion());
    printf("FullVersion: %s\n", ESP.getFullVersion().c_str());
    printf("CpuFreq: %dMHz\n", ESP.getCpuFreqMHz());
    printf("FreeHeap: %d B\n", ESP.getFreeHeap());
    printf("ResetInfo: %s\n", ESP.getResetInfo().c_str());
    printf("ResetReason: %s\n", ESP.getResetReason().c_str());
    DEBUG_HEAP();
    homekit_setup();
    DEBUG_HEAP();
    blinkInternalLed(200, 3);
}

void loop()
{
    homekit_loop();
    delay(5);
}

void blinkInternalLed(int intervalMs, int count)
{
    for (int i = 0; i < count; i++)
    {
        builtInLedSet(true);
        delay(intervalMs);
        builtInLedSet(false);
        delay(intervalMs);
    }
}

void builtInLedSet(bool on)
{
    digitalWrite(BuiltInLedPin, on);
}

//==============================
// Homekit setup and loop
//==============================

uint32_t next_heap_millis = 0;

void homekit_setup()
{
    accessory_init();
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);

    arduino_homekit_setup(&config);
}

void homekit_loop()
{
    arduino_homekit_loop();
    uint32_t time = millis();
    if (time > next_heap_millis)
    {
        INFO("heap: %d, sockets: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
        next_heap_millis = time + 5000;
    }
}
