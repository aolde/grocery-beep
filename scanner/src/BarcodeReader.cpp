#include "BarcodeReader.h"
#include <M5Atom.h>

#define BARCODE_TRIGGER_PIN 23
#define BARCODE_DLED_PIN 33

BarcodeReader::BarcodeReader() {}

BarcodeReader::~BarcodeReader() {}

BarcodeReader &BarcodeReader::onBarcodeScan(OnBarcodeScanCallback callback) {
  this->_onBarcodeScanCallback = callback;
  return *this;
}

void BarcodeReader::setup() {
  Serial2.begin(9600, SERIAL_8N1, 22, 19);
  pinMode(BARCODE_DLED_PIN, INPUT);
  pinMode(BARCODE_TRIGGER_PIN, OUTPUT);
  digitalWrite(BARCODE_TRIGGER_PIN, HIGH);
}

void BarcodeReader::loop() {
  // trigger button is pressed (LOW)
  if (digitalRead(39) == LOW) {
    digitalWrite(BARCODE_TRIGGER_PIN, LOW);
  } else {
    digitalWrite(BARCODE_TRIGGER_PIN, HIGH);
  }

  if (digitalRead(BARCODE_DLED_PIN) == HIGH) {
    while (Serial2.available() > 0) {
      String barcode = Serial2.readStringUntil(0x0d);
      this->_onBarcodeScanCallback(barcode.c_str());
    }
  }
}