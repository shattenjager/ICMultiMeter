// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define VERSION "0.1.5"
#define AUTHOR "F4HWN"
#define NAME "ICMultiMeter"

#include <Preferences.h>
#include <FastLED.h>
#include <HTTPClient.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <M5Unified.h>
#include <BluetoothSerial.h>
#include <M5StackUpdater.h>

#define BT 1
#define USB 2

#define TIMEOUT_BIN_LOADER  3                 // 3 sec
#define DEBUG 0
#define STEP 2

// Color
#define TFT_MODE_BORDER M5.Lcd.color565(115, 135, 159)
#define TFT_MODE_BACK M5.Lcd.color565(24, 57, 92)

#define TFT_FIL_BORDER M5.Lcd.color565(99, 99, 99)
#define TFT_FIL_BACK M5.Lcd.color565(66, 66, 66)

/*
#define TFT_GAUGE_1 M5.Lcd.color565(213, 144, 50)
#define TFT_GAUGE_2 M5.Lcd.color565(233, 196, 66)
#define TFT_GAUGE_3 M5.Lcd.color565(194, 106, 39)
*/

#define TFT_GAUGE_1 M5.Lcd.color565(215, 96, 32)
#define TFT_GAUGE_2 M5.Lcd.color565(255, 128, 32)
#define TFT_GAUGE_3 M5.Lcd.color565(200, 64, 16)


#define TFT_MENU_BORDER M5.Lcd.color565(115, 135, 159)
#define TFT_MENU_BACK M5.Lcd.color565(24, 57, 92)
#define TFT_MENU_SELECT M5.Lcd.color565(255, 255, 255)

// Web site Screen Capture stuff
#define GET_unknown 0
#define GET_index_page  1
#define GET_screenshot  2

// LED
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Bluetooth connector
BluetoothSerial CAT;

// Preferences
Preferences preferences;

// Global Variables
WiFiServer httpServer(80);
WiFiClient httpClient, civClient;

int8_t beep = 0;
int8_t transverter = 0;
int8_t voice = 0;
int8_t voiceMode = 0;
int8_t voiceTimeout = 0;
int8_t screensaver = 0;
uint8_t voiceCounter = 0;
uint8_t htmlGetRequest;
uint8_t option = 2;
uint8_t brightness = 64;
uint8_t sOld = 255;
uint8_t SWROld = 255;
uint8_t powerOld = 255;
uint8_t IdOld = 255;
uint8_t COMPLOld = 255;
uint8_t ALCOld = 127;
uint8_t TXOld = 127;
uint8_t agcOld = 127;
uint8_t ANOld = 127;
uint8_t NBOld = 127;
uint8_t NROld = 127;
uint8_t AMPOld = 127;
uint8_t toneOld = 127;
uint8_t AFOld = 127;
uint8_t MICOld = 127;
uint8_t SQLOld = 127;
uint8_t COMPOld = 127;
uint8_t NRLevelOld = 127;
uint8_t NBLevelOld = 127;
uint8_t VdOld = 0;
uint8_t IPOld = 0;
uint8_t batteryLevelOld = 0;

uint32_t screensaverTimer;
uint32_t transmit;


boolean screensaverMode = false;
boolean screenshot = false;
boolean settingsMode = false;
boolean settingLock = true;
boolean btConnected = false;
boolean wifiConnected = false;
boolean proxyConnected = false;
boolean txConnected = true;
boolean needClear = true;
boolean startup = true;
boolean wakeup = true;
boolean batteryCharginglOld = true;
boolean voiceRefresh = true;
boolean bandeLow;

String frequencyOld = "";
String filterOld = "";
String modeOld = "";
String RITOld = "";

// Flags for button presses via Web site Screen Capture
bool buttonLeftPressed = false;
bool buttonCenterPressed = false;
bool buttonRightPressed = false;

// Bin loader
File root;
String binFilename[128];
uint8_t binIndex = 0;

// Menu
const char *settings[] = {"Voice TX", "Transverter Mode", "Brightness", "Beep", "Screensaver", "IP Address", "Shutdown", "Exit"};
const char *choiceVoice[] = {"OFF", "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8"};
const char *choiceBrightness[] = {"BRIGHTNESS"};
const char *choiceBeep[] = {"BEEP LEVEL"};
const char *choiceScreensaver[] = {"TIMEOUT"};
const double choiceTransverter[] = {
    0,
    TRANSVERTER_LO_1,
    TRANSVERTER_LO_2,
    TRANSVERTER_LO_3,
    TRANSVERTER_LO_4,
    TRANSVERTER_LO_5
};