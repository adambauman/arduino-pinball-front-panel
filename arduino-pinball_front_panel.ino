// BauTek Mini Pinball Front Panel Controller
// (March 2016) Adam J. Bauman, https://bitbucket.org/adambauman/arduino-pinball_front_panel
//
// This code runs the microcontroller that handles all of the front panel button pushes and LEDs.
// The front panel includes two LEDS for the coin slots, four main face buttons for volume control
// and table selection as well a hidden button that can be used for utility functions in the future. The front panel also contains a LIS3DH 3-axis
// accelerometer for tilt control (coming in the future once I figure out the acceleration math.)
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <Adafruit_Sensor.h> //https://github.com/adafruit/Adafruit_Sensor/archive/master.zip
#include <Adafruit_LIS3DH.h> //https://github.com/adafruit/Adafruit_LIS3DH/archive/master.zip
#include <HID-Project.h> //https://github.com/NicoHood/HID
#include <HID-Settings.h>

// *****
// Pin Setup
// *****
#define pinExit 4
#define pinVolDwn 8
#define pinVolUp 7
#define pinMute 21
#define pinCoinLeft 5
#define pinCoinRight 6
#define pinUtility 9

// *****
// Variable Setup
// *****
long debounceDelay = 300; // increase if buttons are unstable
#define CLICKTHRESHHOLD 40 // accel tap sensitivity

// *****
// Object Setup
// *****
Adafruit_LIS3DH lis = Adafruit_LIS3DH();


void setup() {
  if (! lis.begin(0x18)) { 
    Serial.println("Couldnt start Accel I2C connection");
    while (1);
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  // 0 = turn off click detection & interrupt
  // 1 = single click only interrupt output
  // 2 = double click only interrupt output, detect single click
  // Adjust threshhold, higher numbers are less sensitive
  lis.setClick(2, CLICKTHRESHHOLD);

  Keyboard.begin();
  Consumer.begin();
  
  Serial.begin(9600);
  
  // *****
  // Init pins, set pull-up resistors on buttons
  // *****
  pinMode(pinExit, INPUT);
  pinMode(pinVolDwn, INPUT);
  pinMode(pinVolUp, INPUT);
  pinMode(pinMute, INPUT);
  pinMode(pinUtility, INPUT);
  
  pinMode(pinCoinLeft, OUTPUT);
  pinMode(pinCoinRight, OUTPUT);

  digitalWrite(pinExit, HIGH);
  digitalWrite(pinVolDwn, HIGH);
  digitalWrite(pinVolUp, HIGH);
  digitalWrite(pinMute, HIGH);
  digitalWrite(pinUtility, HIGH);

} // End setup()

void loop() {
  int readingExit = digitalRead(pinExit);
  int readingVolDwn = digitalRead(pinVolDwn);
  int readingVolUp = digitalRead(pinVolUp);
  int readingMute = digitalRead(pinMute);
  int readingUtility = digitalRead(pinUtility);

  analogWrite(pinCoinLeft, 5);
  analogWrite(pinCoinRight, 5);

  if (readingExit == LOW) {
    Keyboard.write(KEY_ESC);
    analogWrite(pinCoinLeft, 50);
    analogWrite(pinCoinRight, 50);
    delay(debounceDelay);
  }

  if (readingVolDwn == LOW) {
    Consumer.write(MEDIA_VOLUME_DOWN);
    analogWrite(pinCoinLeft, 50);
    analogWrite(pinCoinRight, 50);
    delay(debounceDelay);
  }

  if (readingVolUp == LOW) {
    Consumer.write(MEDIA_VOLUME_UP);
    analogWrite(pinCoinLeft, 50);
    analogWrite(pinCoinRight, 50);
    delay(debounceDelay);
  }

 if (readingMute == LOW) {
    Consumer.write(MEDIA_VOLUME_MUTE);
    analogWrite(pinCoinLeft, 50);
    analogWrite(pinCoinRight, 50);
    delay(debounceDelay);
  }

  if (readingUtility == LOW) {
    Keyboard.write('p');
    analogWrite(pinCoinLeft, 50);
    analogWrite(pinCoinRight, 50);
    delay(debounceDelay);
  }

  // Grab Accel data
  lis.read();      // get X Y and Z data at once
  // Then print out the raw data
  Serial.print("X:  "); Serial.print(lis.x); 
  Serial.print("  \tY:  "); Serial.print(lis.y); 
  Serial.print("  \tZ:  "); Serial.print(lis.z); 
  Serial.println();

  uint8_t click = lis.getClick();
  if (click == 0) return;
  if (! (click & 0x30)) return;
  Serial.print("Click detected (0x"); Serial.print(click, HEX); Serial.print("): ");
  if (click & 0x10) Serial.print(" single click");
  if (click & 0x20) Serial.print(" double click");
  
} // End loop()

