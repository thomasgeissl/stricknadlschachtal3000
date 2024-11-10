#include "./config.h"
#include <Adafruit_TinyUSB.h>
#include <Adafruit_VL53L0X.h>
#include <MIDI.h>

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// MIDI.sendNoteOn(message.firstByte, message.secondByte, channel);
// MIDI.sendNoteOff(message.firstByte, message.secondByte, channel);
// MIDI.sendControlChange(message.firstByte, message.secondByte, channel);
// MIDI.sendProgramChange(message.firstByte, channel);

void setup() {
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
  }
}

void loop() {
  auto timestamp = millis();

#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  // distance for velocity modulation
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {    // phase failures have incorrect data
    if (measure.RangeMilliMeter > MAX_DISTANCE) {
    } else {
      MIDI.sendControlChange(1, map(measure.RangeDMaxMilliMeter, MIN_DISTANCE, MAX_DISTANCE, 0, 127), 1);
    }
  }
}