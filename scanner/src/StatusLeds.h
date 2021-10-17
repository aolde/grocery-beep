#pragma once
#include <Arduino.h>
#include <M5Atom.h>

typedef enum {
  INIT = 0,
  CONNECTING_WIFI,
  CONNECTING_MQTT,
  READY_TO_SCAN,
  BARCODE_SCANNED,
  PRODUCT_ADDED,
  PRODUCT_NOT_FOUND,
  ERROR
} AppState;

class StatusLeds {
private:
  AppState state;

public:
  StatusLeds(AppState state);
  ~StatusLeds();

  void begin();

  void setState(AppState state);
};
