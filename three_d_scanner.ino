/*
  Arduino code to operate a 3D scanner  - a stepper motor spinning the platform with the object scanned,  trigering camera shutter and (optionally) an external flash regularly.
  Can be used with Canon DSLRs with Magic Lantern's FRSP (electronic shutter) mode, with an external flash. Confirmed to work with 50D and 6D. For 6D, the program mlv_dump from
  http://www.magiclantern.fm/forum/index.php?topic=10526.msg188558#msg188558 works well under Windows.

  Includes modified Adafruit PCD8544 library.

  Written by Sergey Mashchenko.
*/

#include <math.h>
#include <stdio.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Adafruit_PCD8544.h"
#include "Keypad.h"
#include "stdio.h"
#include "three_d_scanner.h"


void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(AF_PIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);
  digitalWrite(STEP_PIN, 0);
  digitalWrite(ENABLE_PIN, 1);
  digitalWrite(AF_PIN, 0);
  digitalWrite(FLASH_PIN, 0);

  pinMode(PIN_LCD_LED, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS3_PIN, HIGH);

  g.t = micros();


  // Checking if EEPROM was never used:
  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255)
  {
    // Initializing with a factory reset (setting EEPROM values to the initial state):
    initialize(1);
  }
  else
  {
    // Initializing, with EEPROM data read from the device
    initialize(0);
  }
  // This should not be done in initialize():
  keypad.key[0].kstate = (KeyState)0;
  keypad.key[1].kstate = (KeyState)0;

}


void loop()
{
  g.t = micros();

  process_keypad(); // reading the keypad

  camera();  // Operating the camera's shutter

  flash();  // Operating the flash

  motor(); // Operating the motor

  cleanup();  // Should be the last line
}



