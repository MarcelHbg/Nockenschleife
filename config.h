/*************************************************************************************
   Konfiguration für Nockenschleifmaschine
   Version: 1.6

   Arduino UNO

   Autor: Marcel Herzberg 04.2021
 *************************************************************************************/
#ifndef config_h
#define config_h
/* LCD Display Anschlüsse *************************************************************
  LCD > Arduino Normal
  -------------
  VSS > GND
  VDD > +5V
  V0  > 10k Poti + 1kOhm Widerstand > GND
  RS  > Pin 2
  R/W > Pin 3
  E   > Pin 4
  D4  > Pin 5
  D5  > Pin 6
  D6  > Pin 7
  D7  > Pin 8
  A   > +5V
  K   > 10 Ohm Widerstand > GND
********************************************
  LCD > Arduino I2C (NANO)

  A4 = I2C DATA
  A5 = I2C CLOCk

*/
/**/
#define lcdCol 20 //display charakters per line
#define lcdRow 4  //display num of lines
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, lcdCol, lcdRow);

/*
#define lcdCol 16 //display charakters per line
#define lcdRow 2  //display num of lines
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 8);
*/
/**************************************************************************************/

/* Konfiguration des Encoders *********************************************************/
#include <RotaryEncoder.h>
RotaryEncoder encoder(A1, A0); // A / B
/**************************************************************************************/

/* Konfiguration des Menues **********************************************************/
#include "menue.h"
/* Basic Cofigurations */
const static int numMenues = 7; // number of Menue lines
const unsigned int spaceValue = 7; // space for the value on lcd
const unsigned int decimals = 3;  // Kommastellen für LCD
// Different Menue lines
// Scale gibt an um wie viel sich der Wert pro tick des encoder ändert
//("Titel", Wert , Scale)
Menue_t curr = {"GD IST", 35.040, 0.01};
Menue_t aim = {"GD SOLL", 35.000, 0.01};
Menue_t feedZ = {"FEED", 0.020, 0.002};
Menue_t speedC = {"C RPM", 10.0, 1.0};
Menue_t rotationsC = {"C RPF", 2.0, 1.0}; // amount of rotations per feed
Menue_t finishRotC = {"C Nachlauf", 5.0, 1.0}; // amount of rotations when finished feed
Menue_t feedOutZ = {"Z Rückzug", 5.0, 1.0}; // way the Z axis moves back after finish grinding (mm)

// Array of Menues Index represents Position in Menue
Menue_t *menue[numMenues + 1] = {NULL, &curr, &aim , &feedZ,
                                &speedC, &rotationsC, &finishRotC, &feedOutZ};
/**************************************************************************************/

/* Konfiguration der Achsen **********************************************************
   Pins der C Achse festlegen
  direction -> 11
  step      -> 12

   Pins der Z Achse festlegen
  direction -> 9
  step      -> 10

  !!! Für STEP -> Nur Digital PIN 8 bis 12 möglich !!!
*/

#include <AccelStepper.h>
AccelStepper axisZ(1, 10, 9);  // mode, step, dir
#define pinStepAxisC 12            //step signal Pin for Axis C
#define pinDirAxisC 11              //direction signal Pin for Axis C
/* Parameter der Achsen */
const bool directionC = false;        // set Direction of the C Axis
const bool directionZ = false;        // set Direction of the Z Axis
const float gearC = 5.7;        // transmission ratio for one revolution of stepper
const float gearZ = 2.0;        // transmission ratio for one revolution of stepper
const float threadPitchZ = 1.2;   // pitch of the thread for linear motion
const unsigned int driverStepsC = 800;                // driver steps of C_axis
const unsigned int driverStepsZ = 1600;                // driver steps of Z_axis
/* Eilgang Antasten */
const float fastFeedZ = 0.5; // mm per encoder tick
/**************************************************************************************/

/* Konfiguration der Input PINS *************************************************************/
/* Taster mit pin und Masse verbinden */
#include "digitalin.h"
DigitalIn C_on(2);           // Taster zum einschalten der Drehachse C
DigitalIn touchOff(3);       // Taster zum "ankratzen" schaltet in den "ankratzmodus" 
DigitalIn autoStart(4);      // Taster zum starten des automatischen zustellens
DigitalIn choose(5);         // Taster des encoders (menuesteuerung/ankratzen)
/**************************************************************************************/


#endif
