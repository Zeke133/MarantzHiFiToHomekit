#include <stdio.h>

#include <Arduino.h>
#include <port.h>

#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include "control-protocol.h"

#define ACCESSORY_NAME ("Marantz HiFi")
#define ACCESSORY_SN (__DATE__ " " __TIME__)
#define ACCESSORY_MANUFACTURER ("DH")
#define ACCESSORY_MODEL ("Marantz PM4000 DIY control by ESP8266 with Arduino Homekit")
#define ACCESSORY_FW ("0.0.17")

// TODO: move control part to separate C++ file and leave here only bridge to HomeKit

// ----------------------- power control -----------------------------------------------
// TODO: hold internally Power parameter value - replace with a real check to GPIO value
enum PowerState
{
    PowerOff = 0,
    PowerOn = 1
} powerState = PowerOff;

homekit_value_t GetPowerState()
{
    return HOMEKIT_BOOL(powerState);
}

void SetPowerState(homekit_value_t value)
{
    powerState = value.bool_value;
    ExecuteUpdatePower(powerState);
}

void ExecuteUpdatePower(enum PowerState actualState)
{
    if (actualState == PowerOn)
    {
        SendMarantzPowerOn();
    }
    else
    {
        SendMarantzPowerOff();
    }
}

// ----------------------- input control -----------------------------------------------
enum AmplifierInput
{
    InputCD,
    InputPhono,
    InputTuner,
    InputAUX,
    InputTape,
    InputMD
} amplifierInput = InputCD;

void SetInputCD(homekit_value_t);
void SetInputPhono(homekit_value_t);
void SetInputTuner(homekit_value_t);
void SetInputTape(homekit_value_t);
void SetInputAux(homekit_value_t);
void SetInputCdrMd(homekit_value_t);

homekit_characteristic_t inputCD_characteristic = HOMEKIT_CHARACTERISTIC_(ON, true, .setter = SetInputCD);
homekit_characteristic_t inputPhono_characteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetInputPhono);
homekit_characteristic_t inputTuner_characteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetInputTuner);
homekit_characteristic_t inputTape_characteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetInputTape);
homekit_characteristic_t inputAux_characteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetInputAux);
homekit_characteristic_t inputMD_characteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetInputCdrMd);

homekit_characteristic_t* GetInputCharacteristic(enum AmplifierInput value)
{
    switch (value)
    {
        case InputCD:       return &inputCD_characteristic;
        case InputPhono:    return &inputPhono_characteristic;
        case InputTuner:    return &inputTuner_characteristic;
        case InputAUX:      return &inputAux_characteristic;
        case InputTape:     return &inputTape_characteristic;
        case InputMD:       return &inputMD_characteristic;

        default:            return &inputCD_characteristic;
    }
}

void SetInputStateInHomeKit(enum AmplifierInput newValue)
{
    if (newValue == amplifierInput)
    {
        homekit_characteristic_notify(GetInputCharacteristic(amplifierInput), HOMEKIT_BOOL(true));
    }
    else
    {
        homekit_characteristic_notify(GetInputCharacteristic(amplifierInput), HOMEKIT_BOOL(false));
        amplifierInput = newValue;
    }
}

void SetInputCD(homekit_value_t value)
{
    SendMarantzInputCD();
    SetInputStateInHomeKit(InputCD);
}

void SetInputPhono(homekit_value_t value)
{
    SendMarantzInputPhono();
    SetInputStateInHomeKit(InputPhono);
}

void SetInputTuner(homekit_value_t value)
{
    SendMarantzInputTuner();
    SetInputStateInHomeKit(InputTuner);
}

void SetInputTape(homekit_value_t value)
{
    SendMarantzInputTape();
    SetInputStateInHomeKit(InputTape);
}

void SetInputAux(homekit_value_t value)
{
    SendMarantzInputAux();
    SetInputStateInHomeKit(InputAUX);
}

void SetInputCdrMd(homekit_value_t value)
{
    SendMarantzInputCdrMd();
    SetInputStateInHomeKit(InputMD);
}

// ----------------------- volume control ----------------------------------------------
void SetVolumeUp(homekit_value_t);
void SetVolumeDown(homekit_value_t);

homekit_characteristic_t volumeUpCharacteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetVolumeUp);
homekit_characteristic_t volumeDownCharacteristic = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = SetVolumeDown);

void SetVolumeUp(homekit_value_t value)
{
    SendMarantzVolumeUp();
    homekit_characteristic_notify(&volumeUpCharacteristic, HOMEKIT_BOOL(false));
}

void SetVolumeDown(homekit_value_t value)
{
    SendMarantzVolumeDown();
    homekit_characteristic_notify(&volumeDownCharacteristic, HOMEKIT_BOOL(false));
}

void SetMute(homekit_value_t value)
{
    SendMarantzMute();
}

// ----------------------- HomeKit configuration ---------------------------------------
void IdentifyAccessory(homekit_value_t _value)
{
    for (int j = 0; j < 3; j++)
    {
        SetPowerState(HOMEKIT_BOOL(true));
        delay(500);
        SetPowerState(HOMEKIT_BOOL(false));
        delay(500);
    }
}
         
homekit_accessory_t *accessories[] =
{
    HOMEKIT_ACCESSORY(
        .id = 1,
        .category = homekit_accessory_category_speaker,
        .services = (homekit_service_t *[]){
            HOMEKIT_SERVICE(
                ACCESSORY_INFORMATION,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(IDENTIFY, IdentifyAccessory), // Can be triggered from ipad on pairing
                    HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
                    HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
                    HOMEKIT_CHARACTERISTIC(NAME, ACCESSORY_NAME),
                    HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, ACCESSORY_SN),
                    HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, ACCESSORY_FW),
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .primary = true,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(ON, false, .getter = GetPowerState, .setter = SetPowerState),
                    HOMEKIT_CHARACTERISTIC(NAME, "/Power"), // Homekit bug? Only names with special characters are set.
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input CD"),
                    &inputCD_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input Phono"),
                    &inputPhono_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input Tuner"),
                    &inputTuner_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input Tape"),
                    &inputTape_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input AUX"),
                    &inputAux_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Input CD-R MD"),
                    &inputMD_characteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Volume UP"),
                    &volumeUpCharacteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(NAME, "/Volume Down"),
                    &volumeDownCharacteristic,
                    NULL}),
            HOMEKIT_SERVICE(
                SWITCH,
                .characteristics = (homekit_characteristic_t *[]){
                    HOMEKIT_CHARACTERISTIC(ON, false, .setter = SetMute),
                    HOMEKIT_CHARACTERISTIC(NAME, "/Mute"),
                    NULL}),
            NULL}),
    NULL
};

homekit_server_config_t config =
{
    .accessories = accessories,
    .password = "111-11-111",
    //.on_event = on_homekit_event,
    .setupId = "ABCD"
};

void accessory_init()
{
    pinMode(ControlLinePin, OUTPUT);
}
