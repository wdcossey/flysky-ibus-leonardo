#include "Joystick.h"
#include <SoftwareSerial.h>

#define IBUS_FRAME_LENGTH 0x20      // iBus packet size (2 byte header, 14 channels x 2 bytes, 2 byte checksum)
#define IBUS_COMMAND40 0x40         // Command is always 0x40
#define IBUS_MAXCHANNELS 14         // iBus has a maximum of 14 channels
#define IBUS_OVERHEAD 3             // Overhead is <length> + <checksum_low><checksum_high>

SoftwareSerial ibusSerial(10, 11);  // RX, TX

volatile boolean ready = false;

static byte ibusIndex = 0;
static byte ibus[IBUS_FRAME_LENGTH] = { 0 };
static int rcValue[IBUS_MAXCHANNELS];

static uint16_t ibusChecksum = 0;

void setup() {
	Joystick.begin(false);
	ibusSerial.begin(115200);
}

byte PacketToByteSample(int input, int midpoint = 1500, int divider = 1000, float multiplier = 255) {
	return (byte)((((float)input - midpoint) / divider) * multiplier);
}

bool VerifyChecksum(uint16_t compare, byte low, byte high) {
	return compare == (uint16_t)low + ((uint16_t)high << 8);
}

void ReadRx() {
	if (ibusSerial.available()) {
		uint8_t packet = ibusSerial.read();

		if (ibusIndex == 0 && packet != IBUS_FRAME_LENGTH) {
			ready = false;
			digitalWrite(13, LOW);
			return;
		}
		else if (ibusIndex == 0 && packet == IBUS_FRAME_LENGTH) {
			ibusChecksum = (0xFFFF - (uint16_t)packet);
		}

		if (ibusIndex == 1 && packet != IBUS_COMMAND40) {
			ibusIndex = 0;
			ready = false;
			digitalWrite(13, LOW);
			return;
		}

		if (ibusIndex < IBUS_FRAME_LENGTH) {
			ibus[ibusIndex] = packet;

			if (ibusIndex > 0 && ibusIndex <= (IBUS_FRAME_LENGTH - IBUS_OVERHEAD)) {
				ibusChecksum -= (uint16_t)packet;
			}
		}

		ibusIndex++;

		rcValue[0] = (ibus[3] << 8) + ibus[2];
		rcValue[1] = (ibus[5] << 8) + ibus[4];
		rcValue[2] = (ibus[7] << 8) + ibus[6];
		rcValue[3] = (ibus[9] << 8) + ibus[8];
		rcValue[4] = (ibus[11] << 8) + ibus[10];
		rcValue[5] = (ibus[13] << 8) + ibus[12];
		rcValue[6] = (ibus[15] << 8) + ibus[14];
		rcValue[7] = (ibus[17] << 8) + ibus[16];
		rcValue[8] = (ibus[19] << 8) + ibus[18];
		rcValue[9] = (ibus[21] << 8) + ibus[20];

		if (ibusIndex == IBUS_FRAME_LENGTH) {
			ibusIndex = 0;

			if (VerifyChecksum(ibusChecksum, ibus[30], ibus[31])) {
				Joystick.setXAxis(PacketToByteSample(rcValue[0]));
				Joystick.setYAxis(PacketToByteSample(rcValue[1]));
				Joystick.setThrottle(PacketToByteSample(rcValue[2], 1000));
				Joystick.setZAxis(PacketToByteSample(rcValue[3]));

				Joystick.sendState();

				digitalWrite(13, HIGH);
			}
			else {
				digitalWrite(13, LOW);
			}
		}
	}
}


void loop() {
	ReadRx();
}

