/*
  This is a Colored Wheel Clock showing the time as colored arcs.
  It is using a round 1.28-inches 240 x 240 pixel GC9A01 display.
  The display library is TFT_eSPI, the sketch is running on an
  ESP32-C6 Supermini, mounted on an Expansion Board.

  It is based on the TFT_eSPI Clour Wheel example.
  The time is retrieved from NTP time servers and it is using the
  Timezone feature of the ESP32 built-in Time library.
*/

/*
  Version Management
06.09.2025 V06 Tutorial version  
05.09.2025 V05 Code cleaning
05.09.2025 V04 Now drawing all arc in display sprite, the seconds/minutes/hours arcs
               are getting cleared by animation
               The day and month is displayed in the centre of the display
05.09.2025 V03 Seconds are disülayed every second to get an nice animation
04.09.2025 V02 Drawing of the arcs is done in transparent sprites to avoid flickering  
               This is working, but after one complete round (e.g. seconds) the arc is
               not deleted.
04.09.2025 V01 Initial programming
*/

// --------------------------------------------------------------
// Programm Information
const char *PROGRAM_VERSION = "ESP32-C6 Supermini TFT_eSPI GC9A01 Colored Wheel Clock V06";
const char *PROGRAM_VERSION_SHORT = "Wheel Clock V06";

// --------------------------------------------------------------
// User Settings
#include "User_Settings.h"

//#define RUN_DEMO_MODE

// --------------------------------------------------------------
// TFT display and TFT_eSPI library

#include "SPI.h"
#include "TFT_eSPI.h"  // https://github.com/Bodmer/TFT_eSPI
TFT_eSPI tft = TFT_eSPI();
// sprite for hour, minutes, seconds, day and month
TFT_eSprite dispSpr = TFT_eSprite(&tft);

const uint16_t SPRITE_WIDTH = tft.width();
const uint16_t SPRITE_HEIGHT = tft.height();


const uint8_t LED_BACKLIGHT_PIN = 0;
uint8_t ledBrightnessPercent = 100;

const uint8_t DISPLAY_ORIENTATION_PORTRAIT = 0;
const uint16_t DISPLAY_COLOR_TRANSPARENT = TFT_TRANSPARENT;
const uint16_t DISPLAY_BACKGROUND_COLOR = TFT_BLACK;
const uint16_t DISPLAY_FOREGROUND_COLOR = TFT_WHITE;
const uint16_t DISPLAY_COLOR_RED = TFT_RED;
const uint16_t DISPLAY_COLOR_GREEN = TFT_GREEN;
const uint16_t DISPLAY_COLOR_GOLD = TFT_GOLD;
const uint16_t DISPLAY_COLOR_SKYBLUE = TFT_SKYBLUE;
const uint16_t DISPLAY_COLOR_YELLOW = TFT_YELLOW;

// --------------------------------------------------------------
// Wi-Fi client
#include <WiFi.h>  // Built-in
int wiFiRssi = 0;
#include "WiFi_Methods.h"

// --------------------------------------------------------------
// Timezone
#include "time.h"
bool isTimeAvailable = false;
String timeString, dateString;  // takes the last update time & date data
uint8_t currentHour, currentMin, currentSec;
struct tm timeInfo;
#include "Time_Methods.h"

// --------------------------------------------------------------
// Fonts
//#include "fonts.h"
#include "DSEG7_Modern_Bold_20.h"

// --------------------------------------------------------------
// Global vars
long lastDisplayUpdateMillis = 0;
const long DISPLAY_UPDATE_INTERVAL_MILLIS = 1000;  // update every second
uint16_t colors[12];                               // the colors of the wheels
const uint16_t rDelta = (tft.width() - 1) / 10;
const uint16_t x = tft.width() / 2;
const uint16_t y = tft.height() / 2;
bool smooth = true;
bool isClearSecondsArch = false;
bool isClearMinutesArch = false;
bool isClearHoursArch = false;
bool waitForClearSecondsArch = false;
bool waitForClearMinutesArch = false;
bool waitForClearHoursArch = false;

// --------------------------------------------------------------
// Display control

void setDisplayBrightness(uint8_t brightnessPercent) {
  if (brightnessPercent > 100) brightnessPercent = 100;
  if (brightnessPercent < 0) brightnessPercent = 0;
  analogWrite(LED_BACKLIGHT_PIN, 255 * ledBrightnessPercent / 100);
}

void InitializeDisplay() {

  tft.init();
  // Backlight PWM Pin
  pinMode(LED_BACKLIGHT_PIN, OUTPUT);
  // set the brightness of the display
  setDisplayBrightness(ledBrightnessPercent);
  delay(10);
  tft.setRotation(DISPLAY_ORIENTATION_PORTRAIT);
  tft.fillScreen(DISPLAY_BACKGROUND_COLOR);
  tft.setTextColor(DISPLAY_FOREGROUND_COLOR, DISPLAY_BACKGROUND_COLOR);
  Serial.println("Initialization of the display done");
}

void prepareWheelColors() {
  // Create the outer ring colours
  for (uint8_t c = 0; c < 2; c++) {
    colors[c + 10] = tft.alphaBlend(128 + c * 127, TFT_RED, TFT_MAGENTA);
    colors[c + 8] = tft.alphaBlend(128 + c * 127, TFT_MAGENTA, TFT_BLUE);
    colors[c + 6] = tft.alphaBlend(128 + c * 127, TFT_BLUE, TFT_GREEN);
    colors[c + 4] = tft.alphaBlend(128 + c * 127, TFT_GREEN, TFT_YELLOW);
    colors[c + 2] = tft.alphaBlend(128 + c * 127, TFT_YELLOW, TFT_ORANGE);
    colors[c + 0] = tft.alphaBlend(128 + c * 127, TFT_ORANGE, TFT_RED);
  }
}

// create the sprite
void createSprite() {
  dispSpr.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
  dispSpr.fillScreen(DISPLAY_COLOR_TRANSPARENT);
  dispSpr.setFreeFont(&DSEG7_Modern_Bold_20);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(PROGRAM_VERSION);

  InitializeDisplay();
  tft.setTextColor(DISPLAY_COLOR_GREEN, DISPLAY_BACKGROUND_COLOR);
  tft.drawCentreString(PROGRAM_VERSION_SHORT, tft.width() / 2, 20, 2);
  tft.drawCentreString("Display init done", tft.width() / 2, 40, 2);
  // start Wi-Fi
  if (StartWiFi() == WL_CONNECTED) {
    tft.drawCentreString("Wi-Fi conntected to", tft.width() / 2, 60, 2);
    tft.drawCentreString(WIFI_SSID, tft.width() / 2, 80, 2);
    // get the NTP time
    if (InitTime(TIMEZONE) == true) {
      tft.drawCentreString("Get current time in TIMEZONE", tft.width() / 2, 100, 2);
      tft.drawCentreString(timeString, tft.width() / 2, 120, 2);
      tft.drawCentreString(dateString, tft.width() / 2, 140, 2);
      tft.drawCentreString("Prepare the display ...", tft.width() / 2, 160, 2);
      prepareWheelColors();
      isTimeAvailable = true;
    } else {
      // time update failed
      tft.drawCentreString("Time update failed, aborting", tft.width() / 2, 100, 2);
    }
  } else {
    tft.drawCentreString("Wi-Fi failure", tft.width() / 2, 60, 2);
    tft.drawCentreString("Check your Wi-Fi", tft.width() / 2, 80, 2);
  }
  createSprite();
  tft.drawCentreString("Sprite created", tft.width() / 2, 180, 2);

#ifdef RUN_DEMO_MODE
  // demo
  tft.drawCentreString("Demo mode", tft.width() / 2, 200, 2);
  StopWiFi();  // avoid unwanted time correction by NTP clock
  delay(200);
  //setTimeManual(2025, 9, 5, 23, 59, 50, true);
  //setTimeManual(2025, 9, 5, 9, 59, 50, true);
  //setTimeManual(2025, 9, 5, 0, 59, 50, true);
  //setTimeManual(2025, 9, 5, 0, 59, 56, true);
  //setTimeManual(2025, 12, 31, 23, 59, 51, false);
  setTimeManual(2026, 1, 1, 0, 59, 51, false);
  delay(2000);
#endif

  delay(1000);
  tft.fillScreen(DISPLAY_BACKGROUND_COLOR);
}

void animateRingClearing(uint8_t ring) {
  uint16_t radius = ring * rDelta;
  for (uint16_t i = 20; i < 41; i++) {
    dispSpr.drawArc(x, y, radius, radius - rDelta, 180, i * 9, DISPLAY_BACKGROUND_COLOR, DISPLAY_BACKGROUND_COLOR, smooth);
    dispSpr.pushSprite(0, 0, TFT_TRANSPARENT);
  }
  for (uint16_t i = 0; i < 21; i++) {
    dispSpr.drawArc(x, y, radius, radius - rDelta, 0, i * 9, DISPLAY_BACKGROUND_COLOR, DISPLAY_BACKGROUND_COLOR, smooth);
    dispSpr.pushSprite(0, 0, TFT_TRANSPARENT);
  }
}

void loop() {
  if (isTimeAvailable) {
    if (millis() - lastDisplayUpdateMillis > DISPLAY_UPDATE_INTERVAL_MILLIS) {
      getLocalTime(&timeInfo);
      uint8_t hh = timeInfo.tm_hour;
      uint8_t mm = timeInfo.tm_min;
      uint8_t mm5 = mm / 5;
      uint8_t ss = timeInfo.tm_sec;
      uint8_t ss5 = ss / 5;
      uint8_t dy = timeInfo.tm_mday;
      uint8_t mn = timeInfo.tm_mon + 1;

      uint8_t dy1, dy2, mn1, mn2;  // single digits
      dy1 = dy / 10;
      dy2 = dy % 10;
      mn1 = mn / 10;
      mn2 = mn % 10;

      uint8_t firstDigit, secondDigit;

      // adjust the time values
      // 1 = first segment on, 12 = 12 segments on, 13 = 1 segment on...
      if (hh == 0) {
        hh = 12;
      } else if (hh > 12) {
        hh = hh - 12;
      }
      if (mm5 == 0) {
        mm5 = 12;
      }
      if ((mm > 0) && (mm < 5)) {
        mm5 = 0;
      }
      if (ss5 == 0) {
        ss5 = 12;
      }
      if ((ss > 0) && (ss < 5)) {
        ss5 = 0;
      }

      // if a drawing is too slow this will catch them
      if (waitForClearSecondsArch && (ss > 0) && (ss < 5)) {
        isClearSecondsArch = true;
        waitForClearSecondsArch = false;
      }

      if (waitForClearMinutesArch && (ss > 0) && (ss < 5)) {
        if (mm == 1) {
          isClearMinutesArch = true;
          waitForClearMinutesArch = false;
        }
      }

      if (waitForClearHoursArch && (mm == 0)) {
        isClearHoursArch = true;
        waitForClearHoursArch = false;
      }

      if (isClearHoursArch) {
        animateRingClearing(3);
        isClearHoursArch = false;
      }

      if (isClearMinutesArch) {
        animateRingClearing(4);
        isClearMinutesArch = false;
      }

      if (isClearSecondsArch) {
        animateRingClearing(5);
        isClearSecondsArch = false;
      }

      if (ss > 55) {
        waitForClearSecondsArch = true;
      }
      if ((mm == 59) && (ss > 50)) {
        waitForClearMinutesArch = true;
      }
      if ((hh == 12) && (mm == 59) && (ss > 50)) {
        waitForClearHoursArch = true;
      }

      dispSpr.fillScreen(DISPLAY_COLOR_TRANSPARENT);
      // Draw rings as a series of arcs, increasingly blend colour with white towards middle
      // as the zero angle point is bottom we need to adjust the wheel clockwise
      // draw the hour ring first
      smooth = true;
      uint8_t iRing = 3;
      uint16_t adjustStartAngle, adjustEndAngle;
      uint16_t radius = iRing * rDelta;
      uint16_t angle;
      for (uint8_t iIndex = 1; iIndex <= hh; iIndex++) {
        uint16_t wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[iIndex - 1], TFT_WHITE);
        angle = (iIndex - 1) * 30;
        adjustStartAngle = angle + 180;
        if (adjustStartAngle > 360) adjustStartAngle += -360;
        adjustEndAngle = adjustStartAngle + 30;
        if (adjustEndAngle > 360) adjustEndAngle += -360;
        dispSpr.drawArc(x, y, radius, radius - rDelta, adjustStartAngle, adjustEndAngle, wheelColor, DISPLAY_BACKGROUND_COLOR, smooth);
      }

      // now the minutes
      iRing = 4;
      radius = iRing * rDelta;
      if (mm5 > 0) {
        for (uint8_t iIndex = 1; iIndex <= mm5; iIndex++) {
          uint16_t wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[iIndex - 1], TFT_WHITE);
          angle = (iIndex - 1) * 30;
          adjustStartAngle = angle + 180;
          if (adjustStartAngle > 360) adjustStartAngle += -360;
          adjustEndAngle = adjustStartAngle + 30;
          if (adjustEndAngle > 360) adjustEndAngle += -360;
          dispSpr.drawArc(x, y, radius, radius - rDelta, adjustStartAngle, adjustEndAngle, wheelColor, TFT_BLACK, smooth);
          if (iIndex == mm5) {
            uint8_t additionalMinutes = mm % 5;
            wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[iIndex], TFT_WHITE);
            if (adjustEndAngle > 359) adjustEndAngle = 0;
            dispSpr.drawArc(x, y, radius, radius - rDelta, adjustEndAngle, adjustEndAngle + (additionalMinutes * 6), wheelColor, TFT_BLACK, smooth);
          }
        }
      } else {
        // this are the first 5 minutes
        uint16_t wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[0], TFT_WHITE);
        // draw additional seconds when the upper bound is reached
        uint8_t additionalMinutes = mm % 5;
        dispSpr.drawArc(x, y, radius, radius - rDelta, 180, 180 + (additionalMinutes * 6), wheelColor, TFT_BLACK, smooth);
      }

      // seconds
      smooth = true;  // only the outest seconds ring is smooth
      iRing = 5;
      radius = iRing * rDelta;
      if (ss5 > 0) {
        for (uint8_t iIndex = 1; iIndex <= ss5; iIndex++) {
          uint16_t wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[iIndex - 1], TFT_WHITE);
          angle = (iIndex - 1) * 30;
          adjustStartAngle = angle + 180;
          if (adjustStartAngle > 360) adjustStartAngle += -360;
          adjustEndAngle = adjustStartAngle + 30;
          if (adjustEndAngle > 360) adjustEndAngle += -360;
          dispSpr.drawArc(x, y, radius, radius - rDelta, adjustStartAngle, adjustEndAngle, wheelColor, TFT_BLACK, smooth);

          // draw additional seconds when the upper bound is reached
          if (iIndex == ss5) {
            uint8_t additionalSeconds = ss % 5;
            wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[iIndex], TFT_WHITE);
            if (adjustEndAngle > 359) adjustEndAngle = 0;
            dispSpr.drawArc(x, y, radius, radius - rDelta, adjustEndAngle, adjustEndAngle + (additionalSeconds * 6), wheelColor, TFT_BLACK, smooth);
          }
        }
      } else {
        // this are the first 5 seconds
        uint16_t wheelColor = tft.alphaBlend((iRing * 255.0) / 5.0, colors[0], TFT_WHITE);
        // draw additional seconds when the upper bound is reached
        uint8_t additionalSeconds = ss % 5;
        dispSpr.drawArc(x, y, radius, radius - rDelta, 180, 180 + (additionalSeconds * 6), wheelColor, TFT_BLACK, smooth);
      }

      dispSpr.drawString(String(dy1) + String(dy2) + "." + String(mn1) + String(mn2), x - 33, y - 9);
      //dispSpr.drawString(String(dy1) + String(dy2) +  ":" + String(mn1) + String(mn2), x - 37, y - 9);

      dispSpr.pushSprite(0, 0, TFT_TRANSPARENT);

      lastDisplayUpdateMillis = millis();
    }
  }  // lastDisplayUpdate
}  // isTimeAvailable
