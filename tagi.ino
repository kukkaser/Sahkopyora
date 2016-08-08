#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Adafruit_NeoPixel.h> // For the pretty lights

Adafruit_PN532 nfc(13, 12, 11, 10);
Adafruit_NeoPixel ls = Adafruit_NeoPixel(16,6); // For the pretty lights

bool found = false;
bool bcon = false; // Is charging
// Below is only for the pretty lights:
bool bset = false; // Bike light control
bool oset = false; // Other tag light control
int bp = 0; // Bike light pixel number
int op = 1; // Other tag light pixel number

void setup() {
  // Initialize PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    while (1); // halt if PN532 not found
  }
  nfc.SAMConfig();
  // Initialize NeoPixel
  ls.begin();
  ls.show();
  /*
   * Following lines for the power relay and voltage select controls:
   * pinMode(*powerRelayPin*, OUTPUT);
   * pinMode(*vSelectPin*, OUTPUT);
   */
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t buid[] = {4,87,204,18,98,62,129}; // Predefined bike connector tag id
  uint8_t uidl; // Tag UID length
  found = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidl, 200);
  if(found) {
  	found = false;
  	/*
  	 * Test for known id
  	 */
  	if(uidl == 7 && buid[0] == uid[0] && buid[1] == uid[1] && buid[2] == uid[2] && buid[3] == uid[3] && buid[4] == uid[4] && buid[5] == uid[5] && buid[6] == uid[6]) {
  		// Bike id detected
  		if(!oset) ls.setPixelColor(op,0,0,0);
  		if(!oset || bp != op) ls.setPixelColor(bp,0,0,0);
  		bp = bp + 1;
  		if(bp > ls.numPixels() - 1) bp = 0;
  		ls.setPixelColor(bp,137,248,15);
  		bset = true;
  		if(oset) oset = false;
  		// Above lines are for the pretty lights
  		/*
  		 * Choose voltage from two choices, retrieved from tag information:
  		 * digitalWrite(*vSelectPin*, *HIGH/LOW*);
  		 */
  		if(!bcon) {
  			bcon = true;
  			delay(3000); // Delay before charging, to make sure bike is connected
  		} else {
  			/*
  			 * Power on:
  			 * digitalWrite(*powerRelayPin*, HIGH);
  			 */
  		}
  	} else {
  		// Some other tag detected
  		if(!bset) ls.setPixelColor(bp,0,0,0);
  		if(!bset || bp != op) ls.setPixelColor(op,0,0,0);
  		op = op - 1;
  		if(op < 0) op = ls.numPixels() - 1;
  		ls.setPixelColor(op,47,90,199);
  		oset = true;
  		if(bset) bset = false;
  		// Above lines for the pretty lights
  		/*
  		 * Should check for matching unlock tag, if found then power off:
  		 * digitalWrite(*powerRelayPin*, LOW);
  		 * bcon = false;
  		 * delay(10000);
  		 */
  	}
  	ls.show(); // Lights on
  	delay(50);
  } else {
  	ls.setPixelColor(bp,0,0,0);
  	ls.setPixelColor(op,0,0,0);
  	bset = false;
  	oset = false;
  	ls.show();
  	// Above shuts down the lights
  	/*
  	 * No tag present... end charging:
  	 * digitalWrite(*powerRelayPin*, LOW);
  	 */
  	bcon = false;
  	delay(50);
  }
}
