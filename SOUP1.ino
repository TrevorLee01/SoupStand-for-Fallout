#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

// Replace these with the real ESP32 GPIOs wired to the MAX98357A.
#define I2S_BCLK 26
#define I2S_LRC 25
#define I2S_DOUT 22

#define BLUETOOTH_DEVICE_NAME "Soup Headphone Stand"

class BluetoothHeadphoneStand {
private:
  I2SStream i2s;
  BluetoothA2DPSink bluetoothSink;

public:
  BluetoothHeadphoneStand() : bluetoothSink(i2s) {
  }

  void begin() {
    Serial.println("Starting Bluetooth headphone...");

    auto config = i2s.defaultConfig(TX_MODE);

    config.pin_bck = I2S_BCLK;
    config.pin_ws = I2S_LRC;
    config.pin_data = I2S_DOUT;

    i2s.begin(config);

    bluetoothSink.start(BLUETOOTH_DEVICE_NAME);

    Serial.println("Bluetooth is ready!");
    Serial.println("Pair your phone with: Soup Headphone Stand");
  }

  void update() {
    // library
  }
};

BluetoothHeadphoneStand headphoneStand;

void setup() {
  Serial.begin(115200);
  delay(500);

  headphoneStand.begin();
}

void loop() {
  headphoneStand.update();
}