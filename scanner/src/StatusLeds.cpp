#include "StatusLeds.h"

extern uint8_t readyToScanIcon[77], crossIcon[77], productAddedIcon[77],
    productNotFoundIcon[77];

StatusLeds::StatusLeds(AppState state) { this->setState(state); }

StatusLeds::~StatusLeds() {}

unsigned long previousMillis = 0;
bool tick = false;

void _updateLedsTask(void *parameter) {
  for (;;) {
    unsigned long currentMillis = millis();
    AppState state = *((AppState *)parameter);

    CRGB colorBlue(47, 85, 223);
    CRGB colorGreen(255, 0, 0);
    CRGB colorYellow(255, 255, 0);
    CRGB colorMqttPurple(102, 0, 102);

    switch (state) {
    case AppState::INIT:
      M5.dis.drawpix(12, CRGB(255, 255, 255));
      break;

    case AppState::CONNECTING_WIFI:
      M5.dis.drawpix(13, colorBlue);
      M5.dis.drawpix(12, colorBlue);
      M5.dis.drawpix(11, colorBlue);
      break;

    case AppState::CONNECTING_MQTT:
      M5.dis.drawpix(14, colorMqttPurple);
      M5.dis.drawpix(13, colorMqttPurple);
      M5.dis.drawpix(12, colorMqttPurple);
      M5.dis.drawpix(11, colorMqttPurple);
      M5.dis.drawpix(10, colorMqttPurple);
      break;

    case AppState::READY_TO_SCAN:
      M5.dis.displaybuff(readyToScanIcon);
      break;

    case AppState::BARCODE_SCANNED:
      if (currentMillis - previousMillis >= 500) {
        if (tick) {
          M5.dis.drawpix(0, 2, colorGreen);
          M5.dis.drawpix(1, 2, colorGreen);
          M5.dis.drawpix(2, 2, colorGreen);
          M5.dis.drawpix(3, 2, colorGreen);
          M5.dis.drawpix(4, 2, colorGreen);
          tick = false;
        } else {
          M5.dis.drawpix(0, 2, CRGB(0, 0, 0));
          M5.dis.drawpix(1, 2, CRGB(0, 0, 0));
          M5.dis.drawpix(2, 2, CRGB(0, 0, 0));
          M5.dis.drawpix(3, 2, CRGB(0, 0, 0));
          M5.dis.drawpix(4, 2, CRGB(0, 0, 0));
          tick = true;
        }
        previousMillis = currentMillis;
      }

      break;

    case AppState::PRODUCT_ADDED:
      M5.dis.displaybuff(productAddedIcon);
      break;

    case AppState::PRODUCT_NOT_FOUND:
      M5.dis.displaybuff(productNotFoundIcon);
      break;

    case AppState::ERROR:
      M5.dis.displaybuff(crossIcon);
      break;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void StatusLeds::begin() {
  M5.dis.setBrightness(30);
  M5.dis.setCore(1);

  xTaskCreatePinnedToCore(_updateLedsTask, "UpdateLedsTask", 1000,
                          (void *)&this->state, 1, NULL, 0);
}

void StatusLeds::setState(AppState state) { this->state = state; }
