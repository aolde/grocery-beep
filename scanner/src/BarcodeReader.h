#pragma once
#include <functional>

typedef std::function<void(const char *barcode)> OnBarcodeScanCallback;

class BarcodeReader {
private:
  OnBarcodeScanCallback _onBarcodeScanCallback;

public:
  BarcodeReader();
  ~BarcodeReader();
  void setup();
  void loop();
  BarcodeReader &onBarcodeScan(OnBarcodeScanCallback callback);
};
