/*
 Name:    flysky_ibus_leonardo.ino
 Created: 05/31/2018
 Updated: 06/01/2018
 Author:  wdcossey
 Notes:   This will ONLY work on an Arduino Leonardo/Pro Mini (ATmega32u4)
          Tested with a generic IBus receiver and a FlySky/Turnigy i6 transmitter
*/

#include "Joystick.h"
#include <SoftwareSerial.h>

#define IBUS_FRAME_LENGTH 0x20      // iBus packet size (2 byte header, 14 channels x 2 bytes, 2 byte checksum)
#define IBUS_COMMAND40 0x40         // Command is always 0x40
#define IBUS_MAXCHANNELS 14         // iBus has a maximum of 14 channels
#define IBUS_OVERHEAD 3             // Overhead is <length> + <checksum_low><checksum_high>

#define IBUS_CHANNEL_MIN 1000       // 
#define IBUS_CHANNEL_MAX 2000       // 

SoftwareSerial ibusSerial(10, 11);  // RX, TX

volatile boolean ready = false;

static byte ibus_index = 0;
static byte ibus_data[IBUS_FRAME_LENGTH] = { 0 };
static uint16_t ibus_checksum = 0;

static int joystick_data[IBUS_MAXCHANNELS];



Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,   // hidReportId
  JOYSTICK_TYPE_MULTI_AXIS,     // joystickType
  0,                            // buttonCount
  0,                            // hatSwitchCount
  true,                         // includeXAxis
  true,                         // includeYAxis
  true,                         // includeZAxis
  true,                         // includeRxAxis
  true,                         // includeRyAxis
  true,                         // includeRzAxis
  false,                        // includeRudder
  false,                        // includeThrottle
  false,                        // includeAccelerator
  false,                        // includeBrake
  false                         // includeSteering
);

void setup() {

  Joystick.setXAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  Joystick.setYAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  Joystick.setZAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  
  Joystick.setRxAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  Joystick.setRyAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  Joystick.setRzAxisRange(IBUS_CHANNEL_MIN, IBUS_CHANNEL_MAX);
  
  Joystick.begin(false);
  ibusSerial.begin(115200);
}

void loop() {
  ReadRxPin();
}

bool VerifyChecksum(uint16_t compare, byte low, byte high) {
  return compare == (uint16_t)low + ((uint16_t)high << 8);
}

void ReadRxPin() {
  if (ibusSerial.available()) {
    uint8_t packet = ibusSerial.read();

    if (ibus_index == 0 && packet != IBUS_FRAME_LENGTH) {
      ready = false;
      digitalWrite(13, LOW);
      return;
    }
    else if (ibus_index == 0 && packet == IBUS_FRAME_LENGTH) {
      ibus_checksum = (0xFFFF - (uint16_t)packet);
    }

    if (ibus_index == 1 && packet != IBUS_COMMAND40) {
      ibus_index = 0;
      ready = false;
      digitalWrite(13, LOW);
      return;
    }

    if (ibus_index < IBUS_FRAME_LENGTH) {
      ibus_data[ibus_index] = packet;

      if (ibus_index > 0 && ibus_index <= (IBUS_FRAME_LENGTH - IBUS_OVERHEAD)) {
        ibus_checksum -= (uint16_t)packet;
      }
    }

    ibus_index++;

    joystick_data[0] = (ibus_data[3] << 8) + ibus_data[2];
    joystick_data[1] = (ibus_data[5] << 8) + ibus_data[4];
    joystick_data[2] = (ibus_data[7] << 8) + ibus_data[6];
    joystick_data[3] = (ibus_data[9] << 8) + ibus_data[8];
    joystick_data[4] = (ibus_data[11] << 8) + ibus_data[10];
    joystick_data[5] = (ibus_data[13] << 8) + ibus_data[12];
    joystick_data[6] = (ibus_data[15] << 8) + ibus_data[14];
    joystick_data[7] = (ibus_data[17] << 8) + ibus_data[16];
    joystick_data[8] = (ibus_data[19] << 8) + ibus_data[18];
    joystick_data[9] = (ibus_data[21] << 8) + ibus_data[20];

    if (ibus_index == IBUS_FRAME_LENGTH) {
      ibus_index = 0;

      if (VerifyChecksum(ibus_checksum, ibus_data[30], ibus_data[31])) {

        // Channel 1 is typically the elevator/pitch
        Joystick.setXAxis(joystick_data[0]);
        
        // Channel 2 is typically the aileron/yaw
        Joystick.setYAxis(joystick_data[1]);

        // Channel 3 is typically the throttle
        Joystick.setRxAxis(joystick_data[2]);
        
        // Channel 4 is typically the rudder/pitch
        Joystick.setRyAxis(joystick_data[3]);

        // Channel 5
        Joystick.setZAxis(joystick_data[4]);

        // Channel 6
        Joystick.setRzAxis(joystick_data[5]);
        
        // Channel 7
        //Joystick.(joystick_data[6]);
        
        // Channel 8
        //Joystick.(joystick_data[7]);
        
        Joystick.sendState();

        digitalWrite(13, HIGH);
      }
      else {
        digitalWrite(13, LOW);
      }

      ibus_checksum = 0;
    }
  }
}
